package sunje.goldilocks.jdbc.core4;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.net.URL;
import java.sql.Array;
import java.sql.Blob;
import java.sql.Clob;
import java.sql.Date;
import java.sql.Ref;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.sql.SQLWarning;
import java.sql.Statement;
import java.sql.Time;
import java.sql.Timestamp;
import java.sql.Types;
import java.util.Calendar;
import java.util.Map;

import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Dependency;
import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.cm.SourceCloseCursor;
import sunje.goldilocks.jdbc.cm.SourceFetch;
import sunje.goldilocks.jdbc.cm.SourceGetCursorName;
import sunje.goldilocks.jdbc.cm.SourceTargetType;
import sunje.goldilocks.jdbc.dt.Column;
import sunje.goldilocks.jdbc.dt.RowCache;
import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.internal.BaseProtocolSource;
import sunje.goldilocks.jdbc.internal.ErrorHolder;
import sunje.goldilocks.jdbc.internal.ReadOnlyResultSetUpdater;
import sunje.goldilocks.jdbc.internal.ResultSetUpdater;
import sunje.goldilocks.jdbc.internal.UpdatableResultSetUpdater;
import sunje.goldilocks.jdbc.util.Logger;
import sunje.goldilocks.sql.GoldilocksTypes;

public abstract class Jdbc4ResultSet extends ErrorHolder implements ResultSet
{
    protected static final int DEFAULT_CACHE_SIZE = Protocol.SERVER_PROPOSED_FETCH_COUNT;
    protected static final long ROWSET_FIRST_ROW_IDX_BEFORE_FIRST = Long.MIN_VALUE;
    protected static final long ROWSET_FIRST_ROW_IDX_AFTER_LAST = Long.MAX_VALUE;
    
    public static Jdbc4ResultSet createResultSet(Jdbc4Statement aStatement, int aResultSetType, Jdbc4ResultSet aSourceForRecycle) throws SQLException
    {
        Jdbc4ResultSet sResult = null;
        sResult = aStatement.createConcreteResultSet(aResultSetType);
        
        if (aSourceForRecycle != null)
        {
            /*
             * PreparedStatement에서 생성되는 ResultSet은 항상 target column이 같기 때문에
             * 이를 재활용하면 성능상 큰 이점이 있다.
             * 뿐만 아니라, 한번 prepare된 statement에 대해 target type을 요청하면
             * 두번째부터는 정보가 오지 않기 때문에 첫번째에 얻은 정보(이전 ResultSet이 가지고 있음)를
             * 재사용할 수 밖에 없다.
             */
            
            sResult.mCache = aSourceForRecycle.mCache;
            sResult.mColumns = aSourceForRecycle.mColumns;
            sResult.mColDescs = aSourceForRecycle.mColDescs;
            sResult.mCache.initToInsert();
            sResult.mCache.beforeFirst();
        }
        return sResult;
    }

    protected class FetchOp extends BaseProtocolSource implements SourceTargetType, SourceFetch
    {
        FetchOp(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        /**************************************************************
         * SourceFetch Methods
         **************************************************************/
        public int getFetchCount()
        {
            if (mStatement.isForInternal())
            {
                /*
                 * meta 조회시 사용되는 internal statement에 의해 fetch가 수행될 때는
                 * 모든 row를 다 가져와야 한다.
                 */
                return Protocol.FETCH_COUNT_UNLIMITED;
            }
            else if (mResultSetType == TYPE_FORWARD_ONLY)
            {
                return mFetchCount;
            }
            else
            {
                // scrollable cursor에서는 fetch count를 0으로 줄 수 없다.
                // 반드시 개수를 명시해야 한다.
                return mFetchCount == 0 ? Protocol.DEFAULT_FETCH_COUNT_FOR_SCROLLABLE_CURSOR : mFetchCount;
            }
        }

        public long getFetchOffset()
        {
            return mRowIdxToFetch;
        }

        /**************************************************************
         * SourceTargetType Methods 
         **************************************************************/
        public void setChanged(boolean aChanged)
        {
            mColTypeChanged = aChanged;
        }
        
        public void setColCount(int aCount)
        {
            if (mColTypeChanged)
            {
                if (mCache == null)
                {
                    int sRowCount = mFetchCount;
                    if (sRowCount == Protocol.FETCH_COUNT_AUTO)
                    {
                        sRowCount = DEFAULT_CACHE_SIZE;
                    }
                    try
                    {
                        mCache = new RowCache(aCount,
                                              mChannel.isLittleEndian(),
                                              sRowCount,
                                              mStatement.mConnection,
                                              getType() != TYPE_FORWARD_ONLY);
                    }
                    catch (SQLException sException)
                    {                
                    }
                    
                    mColumns = new Column[aCount];
                }
                else
                {
                    if (mColumns == null || mColumns.length != aCount)
                    {
                        ErrorMgr.raiseRuntimeError("Target column count cannot be different from the previous one.");
                    }
                }
            }
        }
        
        public void setColSpec(int aColIndex, byte aType, long aPrecision, long aScale, byte aLenUnit, byte aIntervalInd) throws SQLException
        {
            if (mColTypeChanged)
            {
                mColumns[aColIndex] = Column.createTargetColumn(aColIndex,
                                                                aType,
                                                                aPrecision,
                                                                aScale,
                                                                aLenUnit,
                                                                aIntervalInd,
                                                                mCache);
            }
        }

        /**************************************************************
         * SourceFetch Methods 
         **************************************************************/
        public void notifyError(Protocol aProtocol, String aMessage, String aSqlState, int aErrorCode)
        {
            super.notifyError(aProtocol, aMessage, aSqlState, aErrorCode);
            /*
             * fetch protocol시 서버로부터 에러가 왔음.
             * 이 메소드에서 에러에 대한 처리를 하고, 프로토콜 수신이 끝난 후에
             * 사용자에게 fetch에 대한 exception이 던져짐
             */
            if (aProtocol == Protocol.PROTOCOL_TARGET_TYPE ||
                aProtocol == Protocol.PROTOCOL_FETCH ||
                aProtocol == Protocol.PROTOCOL_CLOSE_CURSOR)
            {
                mClosed = true;
            }
        }
        
        public void notifyIgnoredError(Protocol aProtocol)
        {
            /*
             * fetch protocol시 서버로부터 에러가 왔음.
             * 이 메소드에서 에러에 대한 처리를 하고, 프로토콜 수신이 끝난 후에
             * 사용자에게 fetch에 대한 exception이 던져짐
             */
            if (aProtocol == Protocol.PROTOCOL_TARGET_TYPE ||
                aProtocol == Protocol.PROTOCOL_FETCH ||
                aProtocol == Protocol.PROTOCOL_CLOSE_CURSOR)
            {
                mClosed = true;
            }
        }
        
        public long getStatementId()
        {
            return mStatement.getId();
        }
        
        public void beginInsertion(long aFirstRowIdx)
        {
            mFetchedRowSetFirstIdx = aFirstRowIdx;
            mCurrentInsertingRowIdx = mFetchedRowSetFirstIdx;
            mCache.initToInsert();
        }

        public void nextInsertion()
        {
            if (isInsertingRowAcceptable())
            {
                mCache.nextForInsert();
            }
            // 다음 insert될 row의 idx를 갱신한다.
            mCurrentInsertingRowIdx++;
        }

        public void finishInsertion(long aLastRowIdx, boolean aServerCursorClosed)
        {
            if (mMaxRows == 0)
            {
                mLastRowIdx = aLastRowIdx;
            }
            else
            {
                if (aLastRowIdx == Protocol.ROWSET_LAST_ROW_IDX_UNKNOWN)
                {
                    if (isRowSetFetched() &&
                        mFetchedRowSetFirstIdx + mCache.getCount() - 1 == mMaxRows)
                    {
                        /*
                         * 저장한 rowset이 max rows에 의해 마지막 rowset이라면 mLastRowIdx를 세팅한다.
                         */
                        mLastRowIdx = mMaxRows;
                    }
                    else
                    {
                        mLastRowIdx = aLastRowIdx; 
                    }
                }
                else
                {
                    mLastRowIdx = Math.min(aLastRowIdx, mMaxRows);
                }
            }

            mServerCursorClosed = aServerCursorClosed;
            
            /*
             * cache가 하나도 안되었다면 mFetchedRowSetFirstIdx는 요청한 offset이 그대로 온다.
             * before first나 after last를 나타내는 유효한 값으로 변경해준다.
             */
            if (mCache.getCount() == 0)
            {
                /*
                 * 캐싱이 하나도 안된 경우는 다음 두 가지이다.
                 * 1. 서버에서 하나도 안준 경우(mFetchedRowSetFirstIdx는 요청한 offset 값이 그대로 온다.
                 * 2. 서버에서 row를 줬지만 max rows에 걸려 저장하지 않은 경우
                 */
                if (mFetchedRowSetFirstIdx <= 0)
                {
                    mFetchedRowSetFirstIdx = ROWSET_FIRST_ROW_IDX_BEFORE_FIRST;
                }
                else
                {
                    mFetchedRowSetFirstIdx = ROWSET_FIRST_ROW_IDX_AFTER_LAST;
                }
            }
            
            // fetch protocol이 완료되었을 때 다음 작업을 해준다.
            mCache.beforeFirst();
        }

        public void setRowStatus(byte aRowStatus)
        {
            if (isInsertingRowAcceptable())
            {
                mCache.writeRowStatus(aRowStatus);
            }
        }

        public void setColumnData(int aColIndex, CmChannel aChannel) throws SQLException
        {
            if (isInsertingRowAcceptable())
            {
                // aColIndex는 0-base이다.
                if (mMaxFieldSize > 0)
                {
                    int sSqlType = mColumns[aColIndex].getCode().getRepresentativeSqlType();
                    if (sSqlType == Types.CHAR ||
                        sSqlType == Types.VARCHAR ||
                        sSqlType == Types.LONGVARCHAR ||
                        sSqlType == GoldilocksTypes.NCHAR ||
                        sSqlType == GoldilocksTypes.NVARCHAR ||
                        sSqlType == GoldilocksTypes.LONGVARCHAR ||
                        sSqlType == Types.BINARY ||
                        sSqlType == Types.VARBINARY ||
                        sSqlType == Types.LONGVARBINARY)
                    {
                        mCache.writeColumn(aChannel, aColIndex, mMaxFieldSize);
                    }
                    else
                    {
                        mCache.writeColumn(aChannel, aColIndex);
                    }
                }
                else
                {
                    mCache.writeColumn(aChannel, aColIndex);
                }
            }
            else
            {
                // max row를 초과한 row는 버려진다.
                int sLen = aChannel.readVarLen();
                aChannel.skip(sLen);
            }
        }
    }
    
    protected class CloseCursorOp extends BaseProtocolSource implements SourceCloseCursor
    {
        CloseCursorOp(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public long getStatementId()
        {
            return mStatement.getId();
        }

        /**************************************************************
         * SourceCloseCursor Methods
         **************************************************************/
        public void notifyCursorClosed()
        {
            Jdbc4ResultSet.this.notifyCursorClosed();
        }
    }
    
    protected class GetCursorNameOp extends BaseProtocolSource implements SourceGetCursorName
    {
        protected String mCursorName = null;

        GetCursorNameOp(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public long getStatementId()
        {
            return mStatement.getId();
        }

        public void setStatementId(long aStatementId)
        {
            mStatement.setId(aStatementId);
        }
        
        /**************************************************************
         * SourceGetCursorName Methods
         **************************************************************/
        public void setCursorName(String aCursorName)
        {
            mCursorName = aCursorName;
        }
        
        String getCursorName()
        {
            return mCursorName;
        }
    }
            
    protected Jdbc4Statement mStatement;
    protected CmChannel mChannel;
    protected SQLWarning mWarning;
    protected RowCache mCache;
    protected Column[] mColumns;
    protected ColumnDescription[] mColDescs;
    protected boolean mColTypeChanged;
    protected boolean mClosed = false;
    protected boolean mServerCursorClosed = false;
    protected int mFetchCount;
    protected int mMaxRows;
    protected int mMaxFieldSize;
    protected ResultSetUpdater mUpdateHelper;
    protected Logger mLogger;
    protected int mResultSetType;
    protected int mHoldability;
    protected long mFetchedRowSetFirstIdx = ROWSET_FIRST_ROW_IDX_BEFORE_FIRST;
    protected long mCurrentInsertingRowIdx = ROWSET_FIRST_ROW_IDX_BEFORE_FIRST;
    protected long mLastRowIdx = Protocol.ROWSET_LAST_ROW_IDX_UNKNOWN;
    protected FetchOp mFetchOp;
    protected CloseCursorOp mCloseOp;
    protected GetCursorNameOp mGetCursorNameOp;
    private int mLastFetchedColIndex;
    private boolean mDetermined = false;    
    private long mRowIdxToFetch = 1;
    private long mMarkedCurrentIdx = 0;

    protected Jdbc4ResultSet(Jdbc4Statement aStatement, int aResultSetType) throws SQLException
    {
        if ((aResultSetType != TYPE_FORWARD_ONLY) && (aResultSetType != TYPE_SCROLL_INSENSITIVE) && (aResultSetType != TYPE_SCROLL_SENSITIVE))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "ResultSetType:" + aResultSetType);
        }
        mStatement = aStatement;
        mChannel = aStatement.getChannel();
        mFetchCount = aStatement.getFetchSize();
        mMaxRows = aStatement.getMaxRows();
        mMaxFieldSize = aStatement.getMaxFieldSize();
        mLogger = aStatement.mConnection.mLogger;
        mResultSetType = aResultSetType;
        mFetchOp = new FetchOp(this);
        mCloseOp = new CloseCursorOp(this);
        mGetCursorNameOp = new GetCursorNameOp(this);
        resetLastFetchedColumn();
    }
    
    void notifyCursorClosed()
    {
        mClosed = true;
    }
    
    void notifyCommitted()
    {
        if (mHoldability == ResultSet.CLOSE_CURSORS_AT_COMMIT)
        {
            notifyCursorClosed();
        }
    }
    
    void notifyRolledBack()
    {
        notifyCursorClosed();
    }
    
    void notifyStatementFreed()
    {
        notifyCursorClosed();        
    }
    
    boolean isDetermined()
    {
        return mDetermined;
    }
    
    void determineResultSetType(byte aSensitivity, boolean aUpdatable, boolean aCloseAtCommit)
    {
        /*
         * 이 메소드가 불려지기 전에는 
         * 아직 타입이 미정이기 때문에 이 ResultSet을 사용하면 안된다.
         */
        if ((mResultSetType == TYPE_SCROLL_SENSITIVE) &&
            (aSensitivity == Protocol.STMT_ATTR_VALUE_SENSITIVITY_INSENSITIVE))
        {
            /*
             *  sensitive로 요청했으나 서버에서 insensitive로 판단하면
             *  처음 요청을 무시하고 insensitive로 바꾼다.
             */
            mResultSetType = TYPE_SCROLL_INSENSITIVE;
        }
        if (aUpdatable)
        {
            mUpdateHelper = new UpdatableResultSetUpdater();
        }
        else
        {
            mUpdateHelper = new ReadOnlyResultSetUpdater();
        }
        mHoldability = aCloseAtCommit ? ResultSet.CLOSE_CURSORS_AT_COMMIT : ResultSet.HOLD_CURSORS_OVER_COMMIT;
        mDetermined = true;
    }
    
    private boolean isInsertingRowAcceptable()
    {
        return (mMaxRows == 0) || (mCurrentInsertingRowIdx <= mMaxRows); 
    }
    
    /**************************************************************
     * 이 클래스에 소속된 메소드의 분류는 다음과 같다.
     * - Private Methods: 내부에서 사용하는 private 메소드들
     * - FetchResult Methods: FetchResult interface 메소드들
     * - TargetTypeResult Methods: TargetTypeResult interface 메소드들
     * - Common Methods: warning, close 관련 메소드들
     * - Attribute-Getter and Setter Methods: 속성 getter, setter
     * - Wrapper interface methods
     * - Scroll Methods: 스크롤 관련 메소드들
     * - Column Getter Methods: getXXX 류 메소드들
     * - Updater Methods: updateXXX 류 메소드들 + update 관련 메소드들
     * 
     * 소스 상에 이 메소드 분류 순서대로 정의되어 있다.
     */
    
    /**************************************************************
     * Internal Methods 
     **************************************************************/
    protected void fetch() throws SQLException
    {
        synchronized (mChannel)
        {
            mChannel.initToSend();
            addFetchProtocol(mChannel.mDependency.alone());
            mChannel.sendAndReceive();
        }
        testError();
    }

    protected void checkClosed() throws SQLException
    {
        if (mClosed)
        {
            ErrorMgr.raise(ErrorMgr.RESULTSET_CLOSED);
        }
    }
    
    protected void checkScrollable() throws SQLException
    {
        if (mResultSetType == TYPE_FORWARD_ONLY)
        {
            ErrorMgr.raise(ErrorMgr.FORWARD_ONLY_VIOLATION);
        }
    }
    
    protected Column getColumn(String aColumnLabel) throws SQLException
    {
        int sColIndex = -1;

        getColumnInfo();
        
        for (int i = 0; i < mColDescs.length; i++)
        {
            if (mColDescs[i].getColLabel() != null && mColDescs[i].getColLabel().equalsIgnoreCase(aColumnLabel))
            {
                sColIndex = i;
                break;
            }
        }
        
        if (sColIndex < 0 || sColIndex >= mColumns.length)
        {
            ErrorMgr.raise(ErrorMgr.COLUMN_NOT_FOUND, aColumnLabel);
        }
        mLastFetchedColIndex = sColIndex;
        return mColumns[sColIndex];
    }
    
    protected void getColumnInfo() throws SQLException
    {
        if (mColDescs == null)
        {
            mColDescs = new ColumnDescription[mColumns.length];
            synchronized (mChannel)
            {
                mChannel.initToSend();
                for (int i = 0; i < mColumns.length; i++)
                {
                    mColDescs[i] = new ColumnDescription(this, mStatement, mColumns[i].getId() + 1);
                    mColDescs[i].addDescribeColProtocol(mChannel);
                }
                mChannel.sendAndReceive();
            }
            testError();
        }
    }
    
    protected void checkColumnIndex(int aColIndex) throws SQLException
    {
        if (aColIndex < 1 || aColIndex > mColumns.length)
        {
            ErrorMgr.raise(ErrorMgr.COLUMN_NOT_FOUND, String.valueOf(aColIndex));
        }
        mLastFetchedColIndex = aColIndex - 1;
    }
    
    protected void resetLastFetchedColumn()
    {
        mLastFetchedColIndex = -1;
    }
    
    protected boolean isRowSetFetched()
    {
        /*
         * mFetchedRowSetFirstIdx 값이 유효하려면 이 메소드가 true를 반환하는 상태여야 한다.
         */
        return mFetchedRowSetFirstIdx != ROWSET_FIRST_ROW_IDX_BEFORE_FIRST &&
               mFetchedRowSetFirstIdx != ROWSET_FIRST_ROW_IDX_AFTER_LAST;
    }
    
    protected boolean isCacheFirstRowSet()
    {
        return mFetchedRowSetFirstIdx == 1;
    }
    
    protected boolean isCacheLastRowSet()
    {
        return (isRowSetFetched()) &&
               (mLastRowIdx != Protocol.ROWSET_LAST_ROW_IDX_UNKNOWN) &&
               (mFetchedRowSetFirstIdx + mCache.getCount() == mLastRowIdx + 1);
    }
    
    protected void addFetchProtocol(Dependency aDependency) throws SQLException
    {
        mChannel.writeProtocol(Protocol.PROTOCOL_FETCH, mFetchOp, aDependency);
    }
    
    protected void checkCursorPosition() throws SQLException
    {
        if (mCache.isBeforeFirst() || mCache.isAfterLast())
        {
            ErrorMgr.raise(ErrorMgr.INVALID_CURSOR_POSITION);
        }
    }

    void addFirstFetchProtocol(Dependency aDependency) throws SQLException
    {
        aDependency = aDependency.derive();
        mChannel.writeProtocol(Protocol.PROTOCOL_TARGET_TYPE, mFetchOp, aDependency);
        addFetchProtocol(aDependency.last());
    }

    synchronized void closeByStatement() throws SQLException
    {
        /*
         * 성능 향상을 위해 서버에서 미리 close를 했다면 굳이 프로토콜을 보내지 않는다.
         */
        if (mServerCursorClosed)
        {
            notifyCursorClosed();
        }
        else
        {
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_CLOSE_CURSOR, mCloseOp);
        }
    }
    
    synchronized void isolateFromStatement() throws SQLException
    {
        /*
         * statement로부터 분리하기 위해 statement가 필요한 작업들을 미리 해둔다.
         */
        getColumnInfo();
        
        /* cursor name도 statement가 있어야 얻을 수 있기 때문에
         * 여기서 미리 얻어놨었지만, meta를 얻은 ResultSet에서 cursor name은 의미도 없을 뿐더러
         * 서버에서 이미 닫힌 result이기 때문에 name을 얻어서도 안된다.
         * 따라서 커서 이름을 얻는 부분을 뺀다.
         * Meta를 얻은 ResultSet에서 커서 이름을 얻을 경우 null을 반환한다.
         * 2015-11-2
         */
        /* getCursorName();*/
        mStatement = null;
    }
    
    private void updateFetchPos(long aNewPos)
    {
        mRowIdxToFetch = aNewPos;
    }
    
    private void updateNextFetchPos()
    {
        if (mFetchedRowSetFirstIdx == ROWSET_FIRST_ROW_IDX_BEFORE_FIRST)
        {
            mRowIdxToFetch = 1;
        }
        else if (mFetchedRowSetFirstIdx == ROWSET_FIRST_ROW_IDX_AFTER_LAST)
        {
            ErrorMgr.raiseRuntimeError("Invalid fetch position");
        }
        else
        {
            mRowIdxToFetch = mFetchedRowSetFirstIdx + mCache.getCount();
        }
    }
    
    /**
     * 내부에서 scroll할 때 getRow()를 사용하지 말고 mMarkedCurrentIdx를 읽어야 한다.
     * cursor를 계산하는 도중에 mCache의 cursor가 이동하기 때문에 getRow를 사용하면 잘못된 값을 얻는다.
     * 
     */
    private void markCurrentRowIdx()
    {
        try
        {
            mMarkedCurrentIdx = getRow();
        }
        catch (SQLException sException)
        {
            // 발생할 일이 없다.
        }
    }
    
    private boolean fetchAbsolute(long aNewPos) throws SQLException
    {
        if (aNewPos < 0)
        {
            if (mLastRowIdx == Protocol.ROWSET_LAST_ROW_IDX_UNKNOWN)
            {
                updateFetchPos(aNewPos);
                fetch();

                // mLastRowIdx는 fetch된 이후기 때문에 unknown일 수 없다.
                if (mLastRowIdx == Protocol.ROWSET_LAST_ROW_IDX_UNKNOWN)
                {
                    ErrorMgr.raiseRuntimeError("FetchScroll: last row idx must be known");
                }
                
                if (mFetchedRowSetFirstIdx == ROWSET_FIRST_ROW_IDX_BEFORE_FIRST)
                {
                    /*
                     * offset을 음수로 줬는데, mFetchedRowSetFirstIdx가 before first로 세팅되었다면
                     * 서버의 row 개수를 초과한 범위의 값이다. 더이상 fetch할 수가 없다.
                     */
                    return false;
                }
                else if (mFetchedRowSetFirstIdx == ROWSET_FIRST_ROW_IDX_AFTER_LAST)
                {
                    /*
                     * offset을 음수로 줬는데, mFetchedRowSetFirstIdx가 after last로 세팅되었다면
                     * 서버에서는 row를 줬는데 max rows를 초과하여 cache되지 않고 모두 skip되었다는 의미이다.
                     * 이 경우 offset을 재조정해서 다시 fetch해야 한다.
                     * 이 if문은 필요없어 보이지만, 아래 if 문의 계산식에 우연히 맞을 수도 있으므로 필요하다.
                     */
                }
                else if (mFetchedRowSetFirstIdx == mLastRowIdx + aNewPos + 1)
                {
                    return mCache.next();
                }
                // 첫번째 row의 idx가 last idx에 의한 계산값과 다르다면
                // 재요청해야 한다.
            }
            aNewPos = Math.max(mLastRowIdx + aNewPos + 1, 0);
        }
        
        if (aNewPos > mMarkedCurrentIdx)
        {
            // 현재 위치보다 뒤쪽의 row를 요구하면
            // next에 유리하도록 해당 row보다 뒤쪽 row들을 fetch한다.
            updateFetchPos(aNewPos);
            fetch();
            return mCache.next();
        }
        else
        {
            // 현재 위치보다 앞쪽의 row를 요구하면
            // previous에 유리하도록 해당 row보다 앞쪽 row들을 fetch한다.
            long sFetchPos = aNewPos - mFetchOp.getFetchCount() + 1;
            if (sFetchPos < 1)
            {
                updateFetchPos(1);
                fetch();
                return mCache.absolute((int)aNewPos - 1); // RowCache.absolute()는 0-base이다.
            }
            else
            {
                updateFetchPos(sFetchPos);
                fetch();
                mCache.afterLast();
                mCache.previous();
                return true;
            }
        }
    }

    private boolean isIncludedToCache(long aIdx)
    {
        return (isRowSetFetched()) &&
               (mFetchedRowSetFirstIdx <= aIdx) &&
               (aIdx < mFetchedRowSetFirstIdx + mCache.getCount());
    }
    
    /**************************************************************
     * Common Methods
     **************************************************************/
    public void clearWarnings() throws SQLException
    {
        mLogger.logTrace();
        mWarning = null;
    }

    public SQLWarning getWarnings() throws SQLException
    {
        mLogger.logTrace();
        return mWarning;
    }

    public void close() throws SQLException
    {
        mLogger.logTrace();

        if (!mClosed)
        {
            /*
             * 성능 향상을 위해 서버에서 미리 close를 했다면 굳이 프로토콜을 보내지 않는다.
             * 이 조치는 성능향상에 크게 영향을 미친다.
             * prepare-[execute-fetch-close_cursor]+ 패턴에서
             * close_cursor(ResultSet.close)가 별도의 프로토콜 통신을 수행하면 성능은 2/3로 떨어지게 된다.
             * 이를 검증하는 테스트케이스가 performance의 MultiFetchPerf.java이다.
             */
            if (mServerCursorClosed)
            {
                notifyCursorClosed();
            }
            else
            {
                synchronized (mChannel)
                {
                    mChannel.initToSend();
                    mChannel.writeProtocolAlone(Protocol.PROTOCOL_CLOSE_CURSOR, mCloseOp);
                    mChannel.sendAndReceive();            
                }
                testError();
            }
        }
    }

    public boolean isClosed() throws SQLException
    {
        mLogger.logTrace();
        return mClosed;
    }

    /**************************************************************
     * ResultSet Scroll Methods
     **************************************************************/
    public int getType() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mResultSetType;
    }

    public boolean absolute(int aRow) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkScrollable();
        resetLastFetchedColumn();
        markCurrentRowIdx();
        
        if (aRow < 0)
        {
            if (isCacheLastRowSet() && mCache.absolute((int)(mCache.getCount() + aRow)))
            {
                return true;
            }
            
            if (mLastRowIdx != Protocol.ROWSET_LAST_ROW_IDX_UNKNOWN)
            {
                long sTempRow = mLastRowIdx + aRow + 1;
                if (isIncludedToCache(sTempRow))
                {
                    return mCache.absolute((int)(sTempRow - mFetchedRowSetFirstIdx));
                }                
            }
        }
        else
        {
            if (isIncludedToCache(aRow))
            {
                return mCache.absolute((int)(aRow - mFetchedRowSetFirstIdx));
            }
        }
        return fetchAbsolute(aRow);
    }

    public boolean relative(int aRows) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkScrollable();
        resetLastFetchedColumn();
        markCurrentRowIdx();
        
        if (mCache.relative(aRows))
        {
            return true;
        }
        return fetchAbsolute(Math.max(mMarkedCurrentIdx + aRows, 0));
    }
    
    public void afterLast() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkScrollable();
        resetLastFetchedColumn();
        markCurrentRowIdx();
        
        if (!isCacheLastRowSet())
        {
            fetchAbsolute(-1);
        }
        mCache.afterLast();
    }

    public void beforeFirst() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkScrollable();
        resetLastFetchedColumn();
        markCurrentRowIdx();
        
        if (isCacheFirstRowSet())
        {
            mCache.beforeFirst();
        }
        else
        {
            fetchAbsolute(1);
            mCache.previous();
        }
    }

    public boolean first() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkScrollable();
        resetLastFetchedColumn();
        markCurrentRowIdx();
        
        if (isCacheFirstRowSet())
        {
            mCache.beforeFirst();
            return mCache.next();
        }
        else
        {
            return fetchAbsolute(1);
        }
    }

    public boolean last() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkScrollable();
        resetLastFetchedColumn();
        markCurrentRowIdx();
        
        if (isCacheLastRowSet())
        {
            mCache.afterLast();
            return mCache.previous();
        }
        else
        {
            return fetchAbsolute(-1);
        }
    }

    public boolean next() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        resetLastFetchedColumn();
        
        if (mCache.next())
        {
            return true;
        }        
        else if (isCacheLastRowSet())
        {
            /* fetch가 모두 완료되었다. */
            return false;
        }
        else if (mFetchedRowSetFirstIdx == ROWSET_FIRST_ROW_IDX_AFTER_LAST)
        {
            return false;
        }
        
        updateNextFetchPos();
        fetch();
        return mCache.next();
    }

    public synchronized boolean previous() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkScrollable();
        resetLastFetchedColumn();
        markCurrentRowIdx();
        
        if (mCache.previous())
        {
            return true;
        }
        else if (isCacheFirstRowSet())
        {
            // 현재 cache에 있는 row set이 맨 처음 셋이면
            // 더이상 fetch할 게 없다.
            return false;
        }
        else if (mFetchedRowSetFirstIdx == ROWSET_FIRST_ROW_IDX_BEFORE_FIRST)
        {
            return false;
        }
        else if (mFetchedRowSetFirstIdx == ROWSET_FIRST_ROW_IDX_AFTER_LAST)
        {
            return fetchAbsolute(-1);
        }
        return fetchAbsolute(mFetchedRowSetFirstIdx - 1);
    }

    public void refreshRow() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkScrollable();
        if (mResultSetType == TYPE_SCROLL_SENSITIVE)
        {
            int sCur = mCache.getCurrentIdx();
            fetch();
            mCache.absolute(sCur);
        }
    }

    public boolean rowDeleted() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkScrollable();
        return mCache.readRowStatus() == Protocol.FETCH_ROW_STATUS_DELETED;
    }

    public boolean rowInserted() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkScrollable();
        return mCache.readRowStatus() == Protocol.FETCH_ROW_STATUS_INSERTED;
    }

    public boolean rowUpdated() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkScrollable();
        return mCache.readRowStatus() == Protocol.FETCH_ROW_STATUS_UPDATED;
    }

    /**************************************************************
     * Attribute-Getter and Setter Methods
     **************************************************************/
    public synchronized int getRow() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        if (!isRowSetFetched())
        {
            return 0;
        }
        return (int)(mFetchedRowSetFirstIdx + mCache.getCurrentIdx());
    }

    public synchronized boolean isAfterLast() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return (mFetchedRowSetFirstIdx == ROWSET_FIRST_ROW_IDX_AFTER_LAST) ||
               (isCacheLastRowSet() && mCache.isAfterLast());
    }

    public synchronized boolean isBeforeFirst() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return (mFetchedRowSetFirstIdx == ROWSET_FIRST_ROW_IDX_BEFORE_FIRST) ||
               (isCacheFirstRowSet() && mCache.isBeforeFirst());
    }

    public synchronized boolean isFirst() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return isCacheFirstRowSet() && mCache.isFirst();
    }

    public synchronized boolean isLast() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return isCacheLastRowSet() && mCache.isLast();
    }

    public int getConcurrency() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mUpdateHelper.getConcurrency();
    }

    public synchronized String getCursorName() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        
        if (mStatement == null)
        {
            /* internal statement로 meta 등을 조회한 결과로 생긴
             * ResultSet은 mStatement가 null이다.
             * 이런 경우엔 cursor name을 null을 반환한다.
             * (사실 meta를 조회한 ResultSet으로부터 커서 이름을 얻는 경우는 비정상적인 경우이다.)
             */
            return null;
        }
        
        if (mGetCursorNameOp.getCursorName() == null)
        {
            synchronized (mChannel)
            {
                mChannel.initToSend();
                mChannel.writeProtocolAlone(Protocol.PROTOCOL_GET_CURSOR_NAME, mGetCursorNameOp);
                mChannel.sendAndReceive();
            }
            testError();
        }
        return mGetCursorNameOp.getCursorName();
    }

    public int getFetchDirection() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return ResultSet.FETCH_FORWARD;
    }

    public int getFetchSize() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mFetchCount;
    }

    public int getHoldability() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mHoldability; 
    }

    public synchronized ResultSetMetaData getMetaData() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        
        getColumnInfo();
        
        return new Jdbc4ResultSetMetaData(mColDescs, mLogger);
    }

    public synchronized int findColumn(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();

        getColumnInfo();
        
        for (int i = 0; i < mColDescs.length; i++)
        {
            if (mColDescs[i].getColLabel() != null && mColDescs[i].getColLabel().equalsIgnoreCase(aColumnLabel))
            {
                return i + 1;
            }
        }
        ErrorMgr.raise(ErrorMgr.COLUMN_NOT_FOUND, aColumnLabel);
        return 0;
    }

    public boolean wasNull() throws SQLException
    {
        mLogger.logTrace();
        if (mLastFetchedColIndex < 0)
        {
            ErrorMgr.raise(ErrorMgr.NO_COLUMN_READ);
        }
        return mColumns[mLastFetchedColIndex].isNull();
    }

    public synchronized void setFetchDirection(int aDirection) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        if (aDirection == FETCH_FORWARD)
        {
            // nothing to do
        }
        else if (aDirection == FETCH_REVERSE)
        {
            mWarning = ErrorMgr.chainWarning(mWarning, ErrorMgr.IGNORED_UNSUPPERTED_FEATURE, "FETCH_REVERSE");
        }
        else if (aDirection == FETCH_UNKNOWN)
        {
            mWarning = ErrorMgr.chainWarning(mWarning, ErrorMgr.IGNORED_UNSUPPERTED_FEATURE, "FETCH_UNKNOWN");
        }
        else
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, String.valueOf(aDirection));
        }
    }

    public synchronized void setFetchSize(int aRows) throws SQLException
    {
        mLogger.logTrace();
        if (aRows < 0)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, String.valueOf(aRows));
        }
        checkClosed();
        mFetchCount = aRows;
    }

    /**************************************************************
     * Column Getter Methods
     **************************************************************/
    public synchronized Array getArray(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Array type");
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return null;
    }

    public synchronized Array getArray(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Array type");
        checkClosed();
        checkCursorPosition();
        return null;
    }

    public synchronized InputStream getAsciiStream(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getBinaryStream();
    }

    public synchronized InputStream getAsciiStream(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getBinaryStream();
    }

    @Deprecated
    public synchronized BigDecimal getBigDecimal(int aColumnIndex, int aScale)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getBigDecimal();
    }

    public synchronized BigDecimal getBigDecimal(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getBigDecimal();
    }

    @Deprecated
    public synchronized BigDecimal getBigDecimal(String aColumnLabel, int aScale)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getBigDecimal();
    }

    public synchronized BigDecimal getBigDecimal(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getBigDecimal();
    }

    public synchronized InputStream getBinaryStream(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getBinaryStream();
    }

    public synchronized InputStream getBinaryStream(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getBinaryStream();
    }

    public synchronized Blob getBlob(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Blob type");
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return null;
    }

    public synchronized Blob getBlob(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Blob type");
        checkClosed();
        checkCursorPosition();
        return null;
    }

    public synchronized boolean getBoolean(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getBoolean();
    }

    public synchronized boolean getBoolean(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getBoolean();
    }

    public synchronized byte getByte(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getByte();
    }

    public synchronized byte getByte(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getByte();
    }

    public synchronized byte[] getBytes(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getBytes();
    }

    public synchronized byte[] getBytes(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getBytes();
    }

    public synchronized Reader getCharacterStream(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getCharacterStream();
    }

    public synchronized Reader getCharacterStream(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getCharacterStream();
    }

    public synchronized Clob getClob(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Clob type");
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return null;
    }

    public synchronized Clob getClob(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Clob type");
        checkClosed();
        checkCursorPosition();
        return null;
    }

    public synchronized Date getDate(int aColumnIndex, Calendar aCalendar) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getDate(aCalendar);
    }

    public synchronized Date getDate(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getDate(null); 
    }

    public synchronized Date getDate(String aColumnLabel, Calendar aCalendar) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getDate(aCalendar);
    }

    public synchronized Date getDate(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getDate(null);
    }

    public synchronized double getDouble(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getDouble(); 
    }

    public synchronized double getDouble(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getDouble();
    }

    public synchronized float getFloat(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getFloat();
    }

    public synchronized float getFloat(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getFloat();
    }

    public synchronized int getInt(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getInt();
    }

    public synchronized int getInt(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getInt();
    }

    public synchronized long getLong(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getLong();
    }

    public synchronized long getLong(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getLong();
    }

    public synchronized Object getObject(int aColumnIndex, Map<String, Class<?>> aMap)
            throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "UDT Map");
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return null;
    }

    public synchronized Object getObject(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getObject();
    }

    public synchronized Object getObject(String aColumnLabel, Map<String, Class<?>> aMap)
            throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "UDT Map");
        checkClosed();
        checkCursorPosition();
        return null;
    }

    public synchronized Object getObject(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getObject();
    }

    public synchronized Ref getRef(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Ref type");
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return null;
    }

    public synchronized Ref getRef(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Ref type");
        checkClosed();
        checkCursorPosition();
        return null;
    }

    public synchronized short getShort(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getShort();
    }

    public synchronized short getShort(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getShort();
    }

    public Statement getStatement() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mStatement;
    }

    public synchronized String getString(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getString();
    }

    public synchronized String getString(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getString();
    }

    public synchronized Time getTime(int aColumnIndex, Calendar aCalendar) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getTime(aCalendar);
    }

    public synchronized Time getTime(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getTime(null); 
    }

    public synchronized Time getTime(String aColumnLabel, Calendar aCalendar) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getTime(aCalendar);
    }

    public synchronized Time getTime(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getTime(null);
    }

    public synchronized Timestamp getTimestamp(int aColumnIndex, Calendar aCalendar)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getTimestamp(aCalendar);
    }

    public synchronized Timestamp getTimestamp(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return mColumns[aColumnIndex - 1].getTimestamp(null);
    }

    public synchronized Timestamp getTimestamp(String aColumnLabel, Calendar aCalendar)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getTimestamp(aCalendar);
    }

    public synchronized Timestamp getTimestamp(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return getColumn(aColumnLabel).getTimestamp(null);
    }

    public synchronized URL getURL(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "URL type");
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return null;
    }

    public synchronized URL getURL(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "URL type");
        checkClosed();
        checkCursorPosition();
        return null;
    }

    @Deprecated
    public synchronized InputStream getUnicodeStream(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "UnicodeStream");
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return null;
    }

    @Deprecated
    public synchronized InputStream getUnicodeStream(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "UnicodeStream");
        checkClosed();
        checkCursorPosition();
        return null;
    }

    /**************************************************************
     * Updater Methods
     **************************************************************/
    public void moveToCurrentRow() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.moveToCurrentRow();        
    }

    public void moveToInsertRow() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.moveToInsertRow();        
    }

    public void cancelRowUpdates() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.cancelRowUpdates();        
    }

    public void deleteRow() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.deleteRow();        
    }

    public void insertRow() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.insertRow();        
    }

    public void updateRow() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateRow();        
    }

    public void updateArray(int aColumnIndex, Array aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateArray(aColumnIndex, aValue);        
    }

    public void updateArray(String aColumnLabel, Array aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateArray(aColumnLabel, aValue);        
    }

    public void updateAsciiStream(int aColumnIndex, InputStream aValue, int aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateAsciiStream(aColumnIndex, aValue, aLength);
    }

    public void updateAsciiStream(int aColumnIndex, InputStream aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateAsciiStream(aColumnIndex, aValue, aLength);        
    }

    public void updateAsciiStream(int aColumnIndex, InputStream aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateAsciiStream(aColumnIndex, aValue);
    }

    public void updateAsciiStream(String aColumnLabel, InputStream aValue, int aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateAsciiStream(aColumnLabel, aValue, aLength);
    }

    public void updateAsciiStream(String aColumnLabel, InputStream aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateAsciiStream(aColumnLabel, aValue, aLength);
    }

    public void updateAsciiStream(String aColumnLabel, InputStream aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateAsciiStream(aColumnLabel, aValue);
    }

    public void updateBigDecimal(int aColumnIndex, BigDecimal aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBigDecimal(aColumnIndex, aValue);
    }

    public void updateBigDecimal(String aColumnLabel, BigDecimal aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBigDecimal(aColumnLabel, aValue);
    }

    public void updateBinaryStream(int aColumnIndex, InputStream aValue, int aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBinaryStream(aColumnIndex, aValue, aLength);
    }

    public void updateBinaryStream(int aColumnIndex, InputStream aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();        
        mUpdateHelper.updateBinaryStream(aColumnIndex, aValue, aLength);
    }

    public void updateBinaryStream(int aColumnIndex, InputStream aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBinaryStream(aColumnIndex, aValue);
    }

    public void updateBinaryStream(String aColumnLabel, InputStream aValue, int aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBinaryStream(aColumnLabel, aValue, aLength);
    }

    public void updateBinaryStream(String aColumnLabel, InputStream aValue,
            long aLength) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBinaryStream(aColumnLabel, aValue, aLength);
    }

    public void updateBinaryStream(String aColumnLabel, InputStream aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBinaryStream(aColumnLabel, aValue);
    }

    public void updateBlob(int aColumnIndex, Blob aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBlob(aColumnIndex, aValue);
    }

    public void updateBlob(int aColumnIndex, InputStream aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBlob(aColumnIndex, aValue, aLength);
    }

    public void updateBlob(int aColumnIndex, InputStream aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBlob(aColumnIndex, aValue);
    }

    public void updateBlob(String aColumnLabel, Blob aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBlob(aColumnLabel, aValue);
    }

    public void updateBlob(String aColumnLabel, InputStream aValue,
            long aLength) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBlob(aColumnLabel, aValue, aLength);
    }

    public void updateBlob(String aColumnLabel, InputStream aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBlob(aColumnLabel, aValue);
    }

    public void updateBoolean(int aColumnIndex, boolean aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBoolean(aColumnIndex, aValue);
    }

    public void updateBoolean(String aColumnLabel, boolean aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBoolean(aColumnLabel, aValue);
    }

    public void updateByte(int aColumnIndex, byte aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateByte(aColumnIndex, aValue);
    }

    public void updateByte(String aColumnLabel, byte aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateByte(aColumnLabel, aValue);
    }

    public void updateBytes(int aColumnIndex, byte[] aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBytes(aColumnIndex, aValue);
    }

    public void updateBytes(String aColumnLabel, byte[] aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateBytes(aColumnLabel, aValue);
    }

    public void updateCharacterStream(int aColumnIndex, Reader aValue, int aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateCharacterStream(aColumnIndex, aValue, aLength);
    }

    public void updateCharacterStream(int aColumnIndex, Reader aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateCharacterStream(aColumnIndex, aValue, aLength);
    }

    public void updateCharacterStream(int aColumnIndex, Reader aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateCharacterStream(aColumnIndex, aValue);
    }

    public void updateCharacterStream(String aColumnLabel, Reader aValue,
            int aLength) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateCharacterStream(aColumnLabel, aValue, aLength);
    }

    public void updateCharacterStream(String aColumnLabel, Reader aValue,
            long aLength) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateCharacterStream(aColumnLabel, aValue, aLength);
    }

    public void updateCharacterStream(String aColumnLabel, Reader aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateCharacterStream(aColumnLabel, aValue);
    }

    public void updateClob(int aColumnIndex, Clob aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateClob(aColumnIndex, aValue);
    }

    public void updateClob(int aColumnIndex, Reader aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateClob(aColumnIndex, aValue, aLength);
    }

    public void updateClob(int aColumnIndex, Reader aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateClob(aColumnIndex, aValue);
    }

    public void updateClob(String aColumnLabel, Clob aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateClob(aColumnLabel, aValue);
    }

    public void updateClob(String aColumnLabel, Reader aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateClob(aColumnLabel, aValue, aLength);
    }

    public void updateClob(String aColumnLabel, Reader aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateClob(aColumnLabel, aValue);
    }

    public void updateDate(int aColumnIndex, Date aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateDate(aColumnIndex, aValue);
    }

    public void updateDate(String aColumnLabel, Date aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateDate(aColumnLabel, aValue);
    }

    public void updateDouble(int aColumnIndex, double aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateDouble(aColumnIndex, aValue);
    }

    public void updateDouble(String aColumnLabel, double aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateDouble(aColumnLabel, aValue);
    }

    public void updateFloat(int aColumnIndex, float aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateFloat(aColumnIndex, aValue);
    }

    public void updateFloat(String aColumnLabel, float aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateFloat(aColumnLabel, aValue);
    }

    public void updateInt(int aColumnIndex, int aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateInt(aColumnIndex, aValue);
    }

    public void updateInt(String aColumnLabel, int aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateInt(aColumnLabel, aValue);
    }

    public void updateLong(int aColumnIndex, long aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateLong(aColumnIndex, aValue);
    }

    public void updateLong(String aColumnLabel, long aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateLong(aColumnLabel, aValue);
    }

    public void updateNCharacterStream(int aColumnIndex, Reader aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNCharacterStream(aColumnIndex, aValue, aLength);
    }

    public void updateNCharacterStream(int aColumnIndex, Reader aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNCharacterStream(aColumnIndex, aValue);
    }

    public void updateNCharacterStream(String aColumnLabel, Reader aValue,
            long aLength) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNCharacterStream(aColumnLabel, aValue, aLength);
    }

    public void updateNCharacterStream(String aColumnLabel, Reader aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNCharacterStream(aColumnLabel, aValue);
    }

    public void updateNull(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNull(aColumnIndex);
    }

    public void updateNull(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNull(aColumnLabel);
    }

    public void updateObject(int aColumnIndex, Object aValue, int aScaleOrLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateObject(aColumnIndex, aValue, aScaleOrLength);
    }

    public void updateObject(int aColumnIndex, Object aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateObject(aColumnIndex, aValue);
    }

    public void updateObject(String aColumnLabel, Object aValue, int aScaleOrLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateObject(aColumnLabel, aValue, aScaleOrLength);
    }

    public void updateObject(String aColumnLabel, Object aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateObject(aColumnLabel, aValue);
    }

    public void updateRef(int aColumnIndex, Ref aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateRef(aColumnIndex, aValue);
    }

    public void updateRef(String aColumnLabel, Ref aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateRef(aColumnLabel, aValue);
    }

    public void updateShort(int aColumnIndex, short aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateShort(aColumnIndex, aValue);
    }

    public void updateShort(String aColumnLabel, short aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateShort(aColumnLabel, aValue);
    }

    public void updateString(int aColumnIndex, String aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateString(aColumnIndex, aValue);
    }

    public void updateString(String aColumnLabel, String aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateString(aColumnLabel, aValue);
    }

    public void updateTime(int aColumnIndex, Time aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateTime(aColumnIndex, aValue);
    }

    public void updateTime(String aColumnLabel, Time aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateTime(aColumnLabel, aValue);
    }

    public void updateTimestamp(int aColumnIndex, Timestamp aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateTimestamp(aColumnIndex, aValue);
    }

    public void updateTimestamp(String aColumnLabel, Timestamp aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateTimestamp(aColumnLabel, aValue);
    }
}
