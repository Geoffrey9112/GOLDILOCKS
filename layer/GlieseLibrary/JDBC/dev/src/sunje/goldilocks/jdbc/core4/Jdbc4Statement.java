package sunje.goldilocks.jdbc.core4;

import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;
import java.sql.BatchUpdateException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.SQLWarning;
import java.sql.Statement;
import java.util.LinkedList;

import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Dependency;
import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.cm.SourceExecuteDirect;
import sunje.goldilocks.jdbc.cm.SourceFreeStatement;
import sunje.goldilocks.jdbc.cm.SourceGetStmtAttr;
import sunje.goldilocks.jdbc.cm.SourcePrepare;
import sunje.goldilocks.jdbc.cm.SourceSetCursorName;
import sunje.goldilocks.jdbc.cm.SourceSetStmtAttr;
import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.internal.BaseProtocolSource;
import sunje.goldilocks.jdbc.internal.ErrorHolder;
import sunje.goldilocks.jdbc.util.Logger;

public abstract class Jdbc4Statement extends ErrorHolder implements Statement
{
    public static final int EXPLAIN_PLAN_OPTION_OFF = Protocol.STMT_ATTR_VALUE_EXPLAIN_PLAN_OFF;
    public static final int EXPLAIN_PLAN_OPTION_ON = Protocol.STMT_ATTR_VALUE_EXPLAIN_PLAN_ON;
    public static final int EXPLAIN_PLAN_OPTION_ON_VERBOSE = Protocol.STMT_ATTR_VALUE_EXPLAIN_PLAN_ON_VERBOSE;
    public static final int EXPLAIN_PLAN_OPTION_ONLY = Protocol.STMT_ATTR_VALUE_EXPLAIN_PLAN_ONLY;
    
    protected static void validateSql(String aSql) throws SQLException
    {
        if (aSql == null)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "null sql");
        }        
    }
    
    class ExecutionEnv extends BaseProtocolSource implements SourceExecuteDirect, SourcePrepare
    {
        private long mAffectedRowCount = 0;
        private boolean mHasResultSet = false;
        private int mStatementType;
        private Jdbc4ResultSet mResultSet;
        private boolean mUpdatableFromServer;
        private byte mSensitivityFromServer;
        private boolean mCloseAtCommitFromServer;
        private byte[] mSql;
        private int[] mAffectedRowCounts = null;
        private int mBatchResultIndex = 0;
        private boolean mBatchExecution = false;
        private boolean mIsFirst = true;
        
        ExecutionEnv(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public void notifyError(Protocol aProtocol, String aMessage, String aSqlState, int aErrorCode)
        {
            if (mBatchExecution)
            {
                int[] sUpdateCounts = new int[mBatchResultIndex];
                System.arraycopy(mAffectedRowCounts, 0, sUpdateCounts, 0, mBatchResultIndex);
                mErrorHolder.addError(new BatchUpdateException(aMessage, aSqlState, aErrorCode, sUpdateCounts));
                mAffectedRowCounts[mBatchResultIndex] = Statement.EXECUTE_FAILED;
                /* error 발생시 affected row count의 세팅이 되지 않기 때문에 여기서 mBatchResultIndex를 더한다.*/
                mBatchResultIndex++;
            }
            else
            {
                super.notifyError(aProtocol, aMessage, aSqlState, aErrorCode);
            }
        }
        
        public long getStatementId()
        {
            return mId;
        }
        
        public byte[] getSql()
        {
            return mSql;
        }

        public boolean getIsFirst()
        {
            return mIsFirst;
        }

        public void initExecutionResult()
        {
            mHasResultSet = false;
        }
        
        public void setAffectedRowCount(long aRowCount)
        {
            if (mBatchExecution)
            {
                mAffectedRowCounts[mBatchResultIndex++] = (int)aRowCount;
            }
            else
            {
                mAffectedRowCount = aRowCount;
            }
        }

        public void setRecompiled(boolean aRecompiled)
        {
            /*
             * 이 정보는 JDBC에서 사용될 일이 없다.
             */
        }
        
        public void setHasResultSet(boolean aHasResultSet)
        {
            mHasResultSet = aHasResultSet;
            if (!mHasResultSet)
            {
                /*
                 * execution 결과로 resultSet을 가지지 않으면 mResultSet을 null로 세팅한다.
                 * 즉, 이전에 ResultSet이 있었다면 이 객체를 버린다. 
                 */
                mResultSet = null;
            }
        }
        
        public void setHasTransaction(boolean aHasTransaction)
        {
            /*
             * 이 정보는 JDBC에서 사용될 일이 없다.
             */
        }
        
        public void setDclAttr(int aAttrType, String aAttrValue)
        {
            /*
             * 이 정보는 JDBC에서 사용될 일이 없다.
             */
        }
        
        public void setStatementId(long aStatementId)
        {
            Jdbc4Statement.this.mId = aStatementId;
        }
        
        public void setStatementType(int aType)
        {
            mStatementType = aType;        
        }

        public void setResultSetConcurrency(boolean aUpdatable)
        {
            mUpdatableFromServer = aUpdatable;        
        }
        
        public void setResultSetSensitivity(byte aSensitivity)
        {
            mSensitivityFromServer = aSensitivity;        
        }
        
        public void setResultSetHoldability(boolean aCloseAtCommit)
        {
            mCloseAtCommitFromServer = aCloseAtCommit;        
        }
        
        public void setHasTransactionAtPreparation(boolean aRecompiled)
        {
            // 사용할 일이 없다.
        }
        
        public void setHasResultSetAtPreparation(boolean aHasResultSet)
        {
            mHasResultSet = aHasResultSet;
        }
        
        void setSql(byte[] aSql)
        {
            mSql = aSql;
        }
        
        void startBatchExecution(int aBatchCount)
        {
            mBatchExecution = true;
            mIsFirst = true;
            mAffectedRowCounts = new int[aBatchCount];
            mBatchResultIndex = 0;
        }
        
        void endBatchExecution()
        {
            mBatchExecution = false;
            mIsFirst = true;
        }
        
        void invalidateFirstBatchExecution()
        {
            /*
             * batch execution시 첫번째 execution을 한 다음에 이 메소드를 불러야 한다.
             */
            mIsFirst = false;
        }
        
        long getAffectedRowCount()
        {
            if (mHasResultSet)
            {
                return -1;
            }
            else
            {
                return mAffectedRowCount;
            }
        }

        int[] getAffectedRowCounts()
        {
            return mAffectedRowCounts;
        }
        
        int getStatementType()
        {
            return mStatementType;
        }
        
        void writeExecute(Dependency aDependency) throws SQLException
        {
            if (mResultSet != null && !mResultSet.isClosed())
            {
                mResultSet.closeByStatement();
            }

            if (isPreparedStatement())
            {
                mChannel.writeProtocol(Protocol.PROTOCOL_EXECUTE, this, aDependency);
            }
            else
            {
                if (mHasResultSet && mResultSet == null)
                {
                    /*
                     * 이전 execution때 서버에 cursor가 열려있는데 클라이언트엔 ResultSet이 없다면
                     * Fetch를 한번도 수행 안한 경우이다.(Statement.execute()등을 통해서 select를 수행한 경우)
                     * 이 경우 ResultSet을 생성하여 닫아줘야 한다.
                     * PreparedStatement는 이 고민을 할 필요가 없다.
                     */
                    mResultSet = Jdbc4ResultSet.createResultSet(Jdbc4Statement.this, mResultSetType, null);
                    mResultSet.closeByStatement();
                    mResultSet = null;
                }
                mChannel.writeProtocol(Protocol.PROTOCOL_EXECUTE_DIRECT, this, aDependency);
            }
        }
        
        void writePrepare(Dependency aDependency) throws SQLException
        {
            mChannel.writeProtocol(Protocol.PROTOCOL_PREPARE, this, aDependency);
        }
        
        void writeFirstFetch(Dependency aDependency) throws SQLException
        {
            if (!isPreparedStatement())
            {
                /*
                 * direct execution에서는 이전 ResultSet을 재활용하지 않는다.
                 */
                mResultSet = null;
            }
            mResultSet = Jdbc4ResultSet.createResultSet(Jdbc4Statement.this, mResultSetType, mResultSet);
            mResultSet.addFirstFetchProtocol(aDependency);
        }
        
        Jdbc4ResultSet getResultSet() throws SQLException
        {        
            if (!mHasResultSet)
            {
                return null;
            }
            
            if (mResultSet == null)
            {
                mResultSet = Jdbc4ResultSet.createResultSet(Jdbc4Statement.this, mResultSetType, mResultSet);
                synchronized (mChannel)
                {
                    mChannel.initToSend();
                    mResultSet.addFirstFetchProtocol(mChannel.mDependency.alone());
                    mChannel.sendAndReceive();
                }
            }
            if (!mResultSet.isDetermined())
            {
                mResultSet.determineResultSetType(mSensitivityFromServer, mUpdatableFromServer, mCloseAtCommitFromServer);
            }
            
            Jdbc4ResultSet sResult = mResultSet;
            if (mForInternal)
            {
                mResultSet = null;
                mHasResultSet = false;
                sResult.testError();
                sResult.isolateFromStatement();
            }
            else
            {
                sResult.testError();
            }
            return sResult;
        }
        
        boolean getMoreResults() throws SQLException
        {
            if (mHasResultSet && mResultSet != null && !mResultSet.isClosed())
            {
                /*
                 * mResultSet이 null인 경우의 처리: 특별히 아무것도 안한다.
                 * mHasResultSet이 true이면서 mResultSet이 null이라는 의미는,
                 * 서버에 result set이 있지만, JDBC에서 ResultSet 객체를 아직 만들지 않은 상태인데,
                 * 이 경우 서버에 cursor가 남아 있지만 별도로 close해주진 않는다.
                 * execute만 하고 ResultSet을 얻지 않은 상태에서 다시 execute하면 에러가 발생하는데,
                 * 이건 사용자의 몫이다.
                 */
                mResultSet.close();
            }
            
            /*
             * 현재는 한번의 execution당 하나의 result만 가진다.
             * 추후 PSM이 구현되어야 multiple result set의 경우가 발생한다.
             */
            return false;
        }
        
        boolean hasResultSet()
        {
            return mHasResultSet;
        }
        
        void notifyCommitted()
        {
            if (mResultSet != null)
            {
                mResultSet.notifyCommitted();
            }
        }

        void notifyRolledBack()
        {
            if (mResultSet != null)
            {
                mResultSet.notifyRolledBack();
            }
        }
        
        void notifyStatementFreed()
        {
            if (mResultSet != null)
            {
                mResultSet.notifyStatementFreed();
            }
        }
        
        boolean hasOpenedResultSet()
        {
            /* 이 메소드는 서버에 커서가 있거나 클라이언트가 open된 result set을 가지고 있으면 true를 반환한다.
             * setCursorName()때 이 메소드를 사용한다.
             */
            if (mHasResultSet)
            {
                if (mResultSet == null)
                {
                    return true;
                }
                else
                {
                    try
                    {
                        if (mResultSet.isClosed())
                        {
                            return false;
                        }
                        else
                        {
                            return true;
                        }
                    }
                    catch (SQLException sException)
                    {
                        /* isClose에서는 exception이 발생할 일이 없다. */
                    }
                }
            }
            return false;
        }
    }

    protected abstract class CommonStmtAttr extends BaseProtocolSource implements SourceSetStmtAttr
    {
        CommonStmtAttr(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }

        public long getStatementId()
        {
            return mId;
        }

        public void setStatementId(long aStatementId)
        {
            mId = aStatementId;
        }
        
        public byte getAttributeValueType()
        {
            return Protocol.ATTR_DATA_TYPE_INT;
        }
        
        public String getAttributeStringValue() throws SQLException
        {
            ErrorMgr.raiseRuntimeError("Invalid method call");
            return null;
        }
    }

    protected abstract class CommonImmutableStmtAttr extends CommonStmtAttr
    {
        CommonImmutableStmtAttr(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        abstract boolean isNotDefault();
    }
    
    protected abstract class CommonMutableStmtAttr extends CommonStmtAttr
    {
        protected boolean mModified = false;
        protected int mValue;
        
        CommonMutableStmtAttr(ErrorHolder aErrorHolder, int aInitValue)
        {
            super(aErrorHolder);
            mValue = aInitValue;
        }
        
        public int getAttributeIntValue() throws SQLException
        {
            return mValue;
        }
        
        void resetModified()
        {
            mModified = false;
        }
        
        void setValue(int aNewValue)
        {
            if (mValue != aNewValue)
            {
                mModified = true;
            }
            mValue = aNewValue;
        }

        boolean isModified()
        {
            return mModified;
        }
    }
    
    protected class StmtAttrConcurrency extends CommonImmutableStmtAttr
    {
        StmtAttrConcurrency(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public int getAttributeType()
        {
            return Protocol.STMT_ATTR_TYPE_CONCURRENCY; 
        }
        
        public int getAttributeIntValue() throws SQLException
        {
            return mResultSetConcurrency == ResultSet.CONCUR_READ_ONLY ?
                    Protocol.STMT_ATTR_VALUE_CONCURRENCY_READ_ONLY :
                    Protocol.STMT_ATTR_VALUE_CONCURRENCY_UPDATABLE;
        }
        
        boolean isNotDefault()
        {
            return mResultSetConcurrency != Jdbc4Connection.GOLDILOCKS_DEFAULT_RESULT_SET_CONCURRENCY;
        }
    }

    protected class StmtAttrScrollability extends CommonImmutableStmtAttr
    {
        StmtAttrScrollability(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public int getAttributeType()
        {
            return Protocol.STMT_ATTR_TYPE_SCROLLABILITY; 
        }
        
        public int getAttributeIntValue() throws SQLException
        {
            return mResultSetType == ResultSet.TYPE_FORWARD_ONLY ?
                    Protocol.STMT_ATTR_VALUE_SCROLLABILITY_NO_SCROLL :
                    Protocol.STMT_ATTR_VALUE_SCROLLABILITY_SCROLL;
        }
        
        boolean isNotDefault()
        {
            return mResultSetType != ResultSet.TYPE_FORWARD_ONLY;
        }
    }

    protected class StmtAttrSensitivity extends CommonImmutableStmtAttr
    {
        StmtAttrSensitivity(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public int getAttributeType()
        {
            return Protocol.STMT_ATTR_TYPE_SENSITIVITY; 
        }
        
        public int getAttributeIntValue() throws SQLException
        {
            return mResultSetType == ResultSet.TYPE_SCROLL_SENSITIVE ?
                    Protocol.STMT_ATTR_VALUE_SENSITIVITY_SENSITIVE :
                    Protocol.STMT_ATTR_VALUE_SENSITIVITY_INSENSITIVE;
        }
        
        boolean isNotDefault()
        {
            return mResultSetType == ResultSet.TYPE_SCROLL_SENSITIVE;
        }
    }

    protected class StmtAttrCursorType extends CommonImmutableStmtAttr
    {
        StmtAttrCursorType(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public int getAttributeType()
        {
            return Protocol.STMT_ATTR_TYPE_CURSOR_TYPE; 
        }
        
        public int getAttributeIntValue() throws SQLException
        {
            return mResultSetType == ResultSet.TYPE_SCROLL_SENSITIVE ?
                    Protocol.STMT_ATTR_VALUE_CURSOR_TYPE_KEYSET :
                    Protocol.STMT_ATTR_VALUE_CURSOR_TYPE_ISO;
        }
        
        boolean isNotDefault()
        {
            return mResultSetType == ResultSet.TYPE_SCROLL_SENSITIVE;
        }
    }

    protected class StmtAttrHoldability extends CommonImmutableStmtAttr
    {
        StmtAttrHoldability(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }

        public int getAttributeType()
        {
            return Protocol.STMT_ATTR_TYPE_HOLDABILITY; 
        }
        
        public int getAttributeIntValue() throws SQLException
        {
            return mResultSetHoldability == ResultSet.CLOSE_CURSORS_AT_COMMIT ?
                    Protocol.STMT_ATTR_VALUE_HOLDABILITY_WITHOUT_HOLD :
                    Protocol.STMT_ATTR_VALUE_HOLDABILITY_WITH_HOLD;
        }
        
        boolean isNotDefault()
        {
            return mResultSetHoldability != Jdbc4Connection.GOLDILOCKS_DEFAULT_RESULT_SET_HOLDABILITY;
        }
    }

    protected class StmtAttrQueryTimeout extends CommonMutableStmtAttr
    {
        StmtAttrQueryTimeout(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder, Protocol.DEFAULT_STATEMENT_QUERY_TIMEOUT);
        }

        public int getAttributeType()
        {
            return Protocol.STMT_ATTR_TYPE_QUERY_TIMEOUT;
        }
        
        int getQueryTimeout()
        {
            return mValue;
        }
    }
    
    protected class StmtAttrExplainPlan extends CommonMutableStmtAttr
    {
        StmtAttrExplainPlan(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder, Protocol.STMT_ATTR_VALUE_EXPLAIN_PLAN_OFF);
        }

        public int getAttributeType()
        {
            return Protocol.STMT_ATTR_TYPE_EXPLAIN_PLAN_OPTION;
        }
        
        int getQueryTimeout()
        {
            return mValue;
        }
    }

    protected class GetQueryTimeout extends BaseProtocolSource implements SourceGetStmtAttr
    {
        private int mGotQueryTimeout = 0;
        
        GetQueryTimeout(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }

        int getGotQueryTimeout()
        {
            return mGotQueryTimeout;
        }
        
        public long getStatementId()
        {
            return mId;
        }

        public int getAttributeType()
        {
            return Protocol.STMT_ATTR_TYPE_QUERY_TIMEOUT;
        }

        public int getBufferSizeForAttributeStringValue()
        {
            /*
             * query timeout은 int value를 받기 때문에 string을 위한 buffer가 필요없다.
             */
            return 0;
        }
        
        public void setStatementId(long aStatementId)
        {
            mId = aStatementId;
        }
        
        public void setAttributeType(int aType)
        {
            if (aType != Protocol.STMT_ATTR_TYPE_QUERY_TIMEOUT)
            {
                ErrorMgr.raiseRuntimeError("Invalid protocol: GET_QUERY_TIMEOUT expected");
            }
        }

        public void setAttributeValueType(byte aValueType)
        {
            if (aValueType != Protocol.ATTR_DATA_TYPE_INT)
            {
                ErrorMgr.raiseRuntimeError("Invalid protocol: int value expected");
            }            
        }

        public void setAttributeIntValue(int aValue)
        {
            mGotQueryTimeout = aValue;
        }

        public void setAttributeStringValue(String aValue)
        {
            ErrorMgr.raiseRuntimeError("Invalid method call");
        }
        
        public void setNeedBufferSize(int aSize)
        {
            /*
             * Nothing to do
             */
        }
    }
    
    protected class GetExplainPlan extends BaseProtocolSource implements SourceGetStmtAttr
    {
        private static final int DEFAULT_BUFFER_SIZE = 4096;
        private String mExplainPlan = null;
        private int mBufferSize = DEFAULT_BUFFER_SIZE; // default buffer size
        private boolean mNeedRetry = false;
        
        GetExplainPlan(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }

        String getExplainPlan()
        {
            return mExplainPlan;
        }
        
        boolean isRetryingNeeded()
        {
            return mNeedRetry;
        }
        
        void reset()
        {
            mBufferSize = DEFAULT_BUFFER_SIZE;
            mNeedRetry = false;
        }
        
        public long getStatementId()
        {
            return mId;
        }

        public int getAttributeType()
        {
            return Protocol.STMT_ATTR_TYPE_EXPLAIN_PLAN_TEXT;
        }

        public int getBufferSizeForAttributeStringValue()
        {
            return mBufferSize;
        }
        
        public void setStatementId(long aStatementId)
        {
            mId = aStatementId;
        }
        
        public void setAttributeType(int aType)
        {
            if (aType != Protocol.STMT_ATTR_TYPE_EXPLAIN_PLAN_TEXT)
            {
                ErrorMgr.raiseRuntimeError("Invalid protocol: GET_EXPLAIN_PLAN_TEXT expected");
            }
        }

        public void setAttributeValueType(byte aValueType)
        {
            if (aValueType != Protocol.ATTR_DATA_TYPE_STRING)
            {
                ErrorMgr.raiseRuntimeError("Invalid protocol: String value expected");
            }            
        }

        public void setAttributeIntValue(int aValue)
        {
            ErrorMgr.raiseRuntimeError("Invalid method call");            
        }

        public void setAttributeStringValue(String aValue)
        {
            mExplainPlan = aValue;
        }
        
        public void setNeedBufferSize(int aSize)
        {
            if (aSize > mBufferSize)
            {
                mBufferSize = aSize;
                mNeedRetry = true;
            }
        }
    }
    
    protected class FreeStatementOp extends BaseProtocolSource implements SourceFreeStatement
    {
        FreeStatementOp(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public long getStatementId()
        {
            return mId;
        }

        public short getFreeStmtOption()
        {
            return Protocol.FREESTMT_OPTION_DROP;
        }

        public void notifyStatementFreed()
        {
            Jdbc4Statement.this.notifyStatementFreed();
        }        
    }
    
    protected class SetCursorNameOp extends BaseProtocolSource implements SourceSetCursorName
    {
        private String mCursorName;

        SetCursorNameOp(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        void setCursorName(String aCursorName)
        {
            mCursorName = aCursorName;
        }
        
        public long getStatementId()
        {
            return mId;
        }

        public void setStatementId(long aStatementId)
        {
            mId = aStatementId;
        }
        
        public String getCursorName()
        {
            return mCursorName;
        }
    }
    
    protected Jdbc4Connection mConnection;
    protected CmChannel mChannel;
    protected ExecutionEnv mExecutionEnv;
    protected long mId = Protocol.STATEMENT_ID_UNDEFINED;
    protected int mFetchCount = Protocol.DEFAULT_FETCH_COUNT;
    protected int mMaxRows;
    protected int mMaxFieldSize;
    protected final int mResultSetType;
    protected final int mResultSetConcurrency;
    protected final int mResultSetHoldability;
    protected boolean mClosed;
    protected Logger mLogger;
    protected LinkedList<CommonImmutableStmtAttr> mImmutableStmtAttrList;
    protected LinkedList<CommonMutableStmtAttr> mMutableStmtAttrList;
    protected StmtAttrQueryTimeout mSetQueryTimeoutOp;
    protected StmtAttrExplainPlan mSetExplainPlanOp;
    protected GetQueryTimeout mGetQueryTimeoutOp;
    protected GetExplainPlan mGetExplainPlanOp;
    protected FreeStatementOp mFreeStatementOp;
    protected SetCursorNameOp mSetCursorNameOp;
    protected boolean mForInternal = false;
    protected LinkedList<String> mBatchJobs = null;
    protected Charset mCharset;
    
    public Jdbc4Statement(Jdbc4Connection aConnection, int aResultSetType, int aResultSetConcurrency, int aResultSetHoldability)
    {
        mConnection = aConnection;
        mCharset = mConnection.getCharset();
        mChannel = aConnection.getChannel();
        mResultSetType = aResultSetType;
        mResultSetConcurrency = aResultSetConcurrency;
        mResultSetHoldability = aResultSetHoldability;
        mLogger = mConnection.mLogger;
        mExecutionEnv = new ExecutionEnv(this);
        mImmutableStmtAttrList = new LinkedList<CommonImmutableStmtAttr>();
        mMutableStmtAttrList = new LinkedList<CommonMutableStmtAttr>();
        mSetQueryTimeoutOp = new StmtAttrQueryTimeout(this);
        mSetExplainPlanOp = new StmtAttrExplainPlan(this);                
        mGetQueryTimeoutOp = new GetQueryTimeout(this);
        mGetExplainPlanOp = new GetExplainPlan(this);
        mFreeStatementOp = new FreeStatementOp(this);
        mSetCursorNameOp = new SetCursorNameOp(this);
        mImmutableStmtAttrList.add(new StmtAttrConcurrency(this));
        mImmutableStmtAttrList.add(new StmtAttrScrollability(this));
        mImmutableStmtAttrList.add(new StmtAttrSensitivity(this));
        mImmutableStmtAttrList.add(new StmtAttrCursorType(this));
        mImmutableStmtAttrList.add(new StmtAttrHoldability(this));
        mMutableStmtAttrList.add(mSetQueryTimeoutOp);
        mMutableStmtAttrList.add(mSetExplainPlanOp);
        mMaxRows = 0;
        mClosed = false;
    }
    
    protected abstract Jdbc4ResultSet createConcreteResultSet(int aResultSetType) throws SQLException;
    
    protected void checkClosed() throws SQLException
    {
        if (mClosed)
        {
            ErrorMgr.raise(ErrorMgr.STATEMENT_CLOSED);
        }
    }

    protected void writeImmutableStmtAttr() throws SQLException
    {
        for (CommonImmutableStmtAttr sAttr : mImmutableStmtAttrList)
        {
            if (sAttr.isNotDefault())
            {
                mChannel.writeProtocolAlone(Protocol.PROTOCOL_SET_STMT_ATTR, sAttr);
            }
        }
    }
    
    protected void writeMutableStmtAttr() throws SQLException
    {
        for (CommonMutableStmtAttr sAttr : mMutableStmtAttrList)
        {
            if (sAttr.isModified())
            {
                mChannel.writeProtocolAlone(Protocol.PROTOCOL_SET_STMT_ATTR, sAttr);
            }
        }
    }
    
    protected void resetMutableStmtAttrs()
    {
        for (CommonMutableStmtAttr sAttr : mMutableStmtAttrList)
        {
            sAttr.resetModified();
        }
    }
    
    private void writeStmtAttr() throws SQLException
    {
        writeImmutableStmtAttr();
        writeMutableStmtAttr();
        /*
         * immutable attr은 한번만 보내면 된다.
         */
        mImmutableStmtAttrList.clear();
    }
    
    protected byte[] getBytes(String aSql)
    {
        try
        {
            return aSql.getBytes(mCharset.name());
        }
        catch (UnsupportedEncodingException sException)
        {
            return null;
        }
    }
    
    CmChannel getChannel()
    {
        return mChannel;
    }
    
    void setForInternal()
    {
        mForInternal = true;
    }
    
    boolean isForInternal()
    {
        return mForInternal;
    }
    
    boolean isPreparedStatement()
    {
        return false;
    }
    
    synchronized boolean closeByConnection() throws SQLException
    {
        if (mId == Protocol.STATEMENT_ID_UNDEFINED)
        {
            notifyStatementFreed();
            return false;
        }
        else
        {
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_FREE_STATEMENT, mFreeStatementOp);
            return true;
        }
    }

    void notifyCommitted()
    {
        if (mExecutionEnv != null)
        {
            mExecutionEnv.notifyCommitted();
        }
    }
    
    void notifyRolledBack()
    {
        if (mExecutionEnv != null)
        {
            mExecutionEnv.notifyRolledBack();
        }
    }
    
    long getId()
    {
        return mId;
    }
    
    void setId(long aStatementId)
    {
        mId = aStatementId;
    }
    
    protected void notifyStatementFreed()
    {
        mExecutionEnv.notifyStatementFreed();
        mClosed = true;
    }

    /**************************************************************
     * GoldilocksStatement Specific API 
     **************************************************************/    
    public long getUpdateRowCount() throws SQLException
    {
        // java.sql.Statement.getUpdateCount()는 int로 리턴하는데,
        // update count가 int 범위를 넘어설 수 있기 때문에
        // long으로 리턴해야 하지만, JDBC API를 바꿀 순 없으므로,
        // 따로 이 메소드를 제공한다.
        return mExecutionEnv.getAffectedRowCount();
    }

    public void setExplainPlanOption(int aOption) throws SQLException
    {
        if (aOption != EXPLAIN_PLAN_OPTION_OFF &&
            aOption != EXPLAIN_PLAN_OPTION_ON &&
            aOption != EXPLAIN_PLAN_OPTION_ON_VERBOSE &&
            aOption != EXPLAIN_PLAN_OPTION_ONLY)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, String.valueOf(aOption));
        }
        
        mSetExplainPlanOp.setValue(aOption);
    }

    public int getExplainPlanOption() throws SQLException
    {
        return mSetExplainPlanOp.getAttributeIntValue();
    }
    
    public String getExplainPlan() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        
        synchronized (mChannel)
        {
            mChannel.initToSend();
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_GET_STMT_ATTR, mGetExplainPlanOp);
            mChannel.sendAndReceive();
        }
        testError();
            
        if (mGetExplainPlanOp.isRetryingNeeded())
        {
            synchronized (mChannel)
            {
                mChannel.initToSend();
                mChannel.writeProtocolAlone(Protocol.PROTOCOL_GET_STMT_ATTR, mGetExplainPlanOp);
                mChannel.sendAndReceive();
            }
            mGetExplainPlanOp.reset();
            testError();
        }
        return mGetExplainPlanOp.getExplainPlan();
    }
    
    /**************************************************************
     * Statement Methods 
     **************************************************************/    
    public synchronized void addBatch(String aSql) throws SQLException
    {
        mLogger.logTrace();
        if (mBatchJobs == null)
        {
            mBatchJobs = new LinkedList<String>();
        }
        mBatchJobs.add(aSql);
    }
    
    public void cancel() throws SQLException
    {
        mLogger.logTrace();
        /*
         * cancel은 지원하지 않는다.
         */
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Cancel");
    }

    public synchronized void clearBatch() throws SQLException
    {
        mLogger.logTrace();
        if (mBatchJobs != null)
        {
            mBatchJobs.clear();
        }
    }

    public void clearWarnings() throws SQLException
    {
        clearWarning();
    }
    
    public synchronized void close() throws SQLException
    {
        mLogger.logTrace();
        if (!mClosed)
        {
            if (mId == Protocol.STATEMENT_ID_UNDEFINED)
            {
                notifyStatementFreed();
            }
            else
            {
                synchronized (mChannel)
                {
                    mChannel.initToSend();
                    mChannel.writeProtocolAlone(Protocol.PROTOCOL_FREE_STATEMENT, mFreeStatementOp);
                    mChannel.sendAndReceive();
                }
                testError();
            }
            mConnection.removeStatement(this);
        }
    }
    
    public boolean execute(String aSql, int aAutoGeneratedKeys) throws SQLException
    {
        mLogger.logTrace();
        if ((aAutoGeneratedKeys != NO_GENERATED_KEYS) &&
            (aAutoGeneratedKeys != RETURN_GENERATED_KEYS))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, String.valueOf(aAutoGeneratedKeys));
        }
        if (aAutoGeneratedKeys == RETURN_GENERATED_KEYS)
        {
            ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Auto generated keys");
        }
        return execute(aSql);
    }
    
    public boolean execute(String aSql, int[] aColumnIndexes) throws SQLException
    {
        mLogger.logTrace();
        if (aColumnIndexes != null)
        {
            ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Auto generated keys");
        }
        return execute(aSql);
    }
    
    public boolean execute(String aSql, String[] aColumnNames) throws SQLException
    {
        mLogger.logTrace();
        if (aColumnNames != null)
        {
            ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Auto generated keys");
        }
        return execute(aSql);
    }
    
    public synchronized boolean execute(String aSql) throws SQLException
    {
        mLogger.logTrace();
        validateSql(aSql);
        mLogger.logQuery(aSql);
        checkClosed();
        if (mBatchJobs != null && !mBatchJobs.isEmpty())
        {
            ErrorMgr.raise(ErrorMgr.BATCH_JOB_VIOLATION);
        }

        mExecutionEnv.setSql(getBytes(aSql));
        
        synchronized (mChannel)
        {
            mChannel.initToSend();
            writeStmtAttr();
            mExecutionEnv.writeExecute(mChannel.mDependency.alone());
            mConnection.addCommitProtocolOnAutoCommit(mForInternal);
            mChannel.sendAndReceive();
        }
        resetMutableStmtAttrs();
        testError();
        
        return mExecutionEnv.hasResultSet();
    }
    
    public synchronized int[] executeBatch() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();        

        if (mBatchJobs == null || mBatchJobs.isEmpty())
        {
            /* #644 batch job이 없을 때 에러대신 무시하게 수정한다.
             * 조건부로 addBatch를 한 후 executeBatch를 수행할 때
             * add된 batch job이 있는지 없는지 체크한 후 execute하게 하는건 불편할 수 있다.
             */
            return new int[0];
        }
        
        mExecutionEnv.startBatchExecution(mBatchJobs.size());
        synchronized (mChannel)
        {
            for (int i=0; i<mBatchJobs.size(); i++)
            {
                String sSql = mBatchJobs.get(i); 
                mLogger.logQuery(sSql);
                mExecutionEnv.setSql(getBytes(sSql));
                mChannel.initToSend();
                writeStmtAttr();
                mExecutionEnv.writeExecute(mChannel.mDependency.alone());
                mConnection.addCommitProtocolOnAutoCommit(mForInternal);
                mChannel.sendAndReceive();
            }
        }
        mExecutionEnv.endBatchExecution();
        resetMutableStmtAttrs();
        
        /* 아래 코드는 원래 없었으나 r12817버전에 들어간 코드이다.
         * 왜 넣었는지 이유는 기억할 수 없으나 오라클은 clear를 해준다.
         * clear를 해 주는게 편리성 면에서 더 좋아보이고 클라이언트의 잠재적 오류를 방지할 수 있어
         * GOLDILOCKS도 이 정책을 따르기로 한다. 2015-10-22 
         */
        clearBatch();
        testError();
        return mExecutionEnv.getAffectedRowCounts();
    }
    
    public synchronized ResultSet executeQuery(String aSql) throws SQLException
    {
        mLogger.logTrace();
        validateSql(aSql);
        mLogger.logQuery(aSql);
        checkClosed();        
        
        if (mBatchJobs != null && !mBatchJobs.isEmpty())
        {
            ErrorMgr.raise(ErrorMgr.BATCH_JOB_VIOLATION);
        }
        
        mExecutionEnv.setSql(getBytes(aSql));
        synchronized (mChannel)
        {
            mChannel.initToSend();
            writeStmtAttr();
            mExecutionEnv.writeExecute(mChannel.mDependency.first());
            mConnection.addCommitProtocolOnAutoCommit(mForInternal);
            mExecutionEnv.writeFirstFetch(mChannel.mDependency.last());
            mChannel.sendAndReceive();
        }
        
        resetMutableStmtAttrs();
        testError();
        
        if (mExecutionEnv.getStatementType() != Protocol.EXECDIR_STMT_TYPE_SELECT_CURSOR)
        {
            ErrorMgr.raise(ErrorMgr.NOT_SELECT_QUERY);
        }
        
        return mExecutionEnv.getResultSet();
    }

    public int executeUpdate(String aSql, int aAutoGeneratedKeys) throws SQLException
    {
        mLogger.logTrace();
        if ((aAutoGeneratedKeys != NO_GENERATED_KEYS) &&
            (aAutoGeneratedKeys != RETURN_GENERATED_KEYS))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, String.valueOf(aAutoGeneratedKeys));
        }
        if (aAutoGeneratedKeys == RETURN_GENERATED_KEYS)
        {
            ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Auto generated keys");
        }
        return executeUpdate(aSql);
    }
    
    public int executeUpdate(String aSql, int[] aColumnIndexes) throws SQLException
    {
        mLogger.logTrace();
        if (aColumnIndexes != null)
        {
            ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Auto generated keys");
        }
        return executeUpdate(aSql);
    }
    
    public int executeUpdate(String aSql, String[] aColumnNames) throws SQLException
    {
        mLogger.logTrace();
        if (aColumnNames != null)
        {
            ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Auto generated keys");
        }
        return executeUpdate(aSql);
    }

    public int executeUpdate(String aSql) throws SQLException
    {
        mLogger.logTrace();
        if (execute(aSql))
        {
            ErrorMgr.raise(ErrorMgr.SQL_PRODUCE_RESULTSET);
        }
        return (int)mExecutionEnv.getAffectedRowCount();
    }

    public Connection getConnection() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mConnection.getHandleForClient();
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

    public ResultSet getGeneratedKeys() throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Auto generated keys");
        return null;
    }

    public int getMaxFieldSize() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mMaxFieldSize;
    }
    
    public int getMaxRows() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mMaxRows;
    }
    
    public boolean getMoreResults() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mExecutionEnv.getMoreResults();
    }
    
    public synchronized boolean getMoreResults(int aCurrent) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        if (aCurrent != Statement.CLOSE_CURRENT_RESULT)
        {
            ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "either KEEP_CURRENT_RESULT or CLOSE_ALL_RESULTS");
        }
        return mExecutionEnv.getMoreResults();
    }
    
    public int getQueryTimeout() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        if (mSetQueryTimeoutOp.getQueryTimeout() == Protocol.DEFAULT_STATEMENT_QUERY_TIMEOUT)
        {
            synchronized (mChannel)
            {
                mChannel.initToSend();
                if (mSetQueryTimeoutOp.isModified())
                {
                    /*
                     * query timeout을 다른 값에서 다시 -1로 세팅한 경우
                     * 아직 쿼리를 실행하지 않았으면(execute 전이라면) 서버에 반영되어 있지 않기 때문에
                     * get attribute하면 올바른 값을 얻어오지 못한다.
                     * 따라서 get하기 전에 set을 먼저 보낸다.
                     */
                    mChannel.writeProtocolAlone(Protocol.PROTOCOL_SET_STMT_ATTR, mSetQueryTimeoutOp);
                    mSetQueryTimeoutOp.resetModified();
                }
                mChannel.writeProtocolAlone(Protocol.PROTOCOL_GET_STMT_ATTR, mGetQueryTimeoutOp);
                mChannel.sendAndReceive();
            }
            testError();
            return mGetQueryTimeoutOp.getGotQueryTimeout();
        }
        else
        {
            return mSetQueryTimeoutOp.getQueryTimeout();
        }
    }

    public synchronized ResultSet getResultSet() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        // 원래 스펙에는 result 하나당 한번만 불려야 한다고 명시되어 있지만,
        // 굳이 이 메소드를 부른다고 해서 현재 결과를 날릴 필요는 없을 것 같아,
        // 여러번 불려도 계속 현재 ResultSet을 리턴하도록 구현한다. (좀더 현실성을 반영)
        // 오라클도 그렇게 동작한다.
        
        return mExecutionEnv.getResultSet();
    }
    
    public int getResultSetConcurrency() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mResultSetConcurrency;
    }
    
    public int getResultSetHoldability() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mResultSetHoldability;
    }

    public int getResultSetType() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return mResultSetType;
    }

    public int getUpdateCount() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        return (int)getUpdateRowCount();
    }

    public SQLWarning getWarnings() throws SQLException
    {
        mLogger.logTrace();
        return mWarning;
    }
    
    public synchronized void setCursorName(String aCursorName) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        
        /*
         * 현재 열려져 있는 ResultSet이 있으면 setCursorName을 할 수 없다.
         */
        if (mExecutionEnv.hasOpenedResultSet())
        {
            ErrorMgr.raise(ErrorMgr.STATEMENT_HAS_AN_OPENED_RESULTSET);
        }
        
        mSetCursorNameOp.setCursorName(aCursorName);
        synchronized (mChannel)
        {
            mChannel.initToSend();
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_SET_CURSOR_NAME, mSetCursorNameOp);
            mChannel.sendAndReceive();
        }
        testError();
    }
    
    public void setEscapeProcessing(boolean aEnable) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        // escape syntax는 서버의 파서에서 처리되기 때문에 드라이버에서 escape 처리를 막을 수 없다.
        // nothing to do
    }

    public void setFetchDirection(int aFetchDirection) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        if (aFetchDirection != ResultSet.FETCH_FORWARD)
        {
            ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Non-forward fetch direction");
        }
    }

    public void setFetchSize(int aFetchCount) throws SQLException
    {
        mLogger.logTrace();
        if (aFetchCount < 0)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, String.valueOf(aFetchCount));
        }
        checkClosed();
        mFetchCount = aFetchCount;        
    }

    public void setMaxFieldSize(int aMaxFieldSize) throws SQLException
    {
        mLogger.logTrace();
        if (aMaxFieldSize < 0)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, String.valueOf(aMaxFieldSize));
        }
        checkClosed();
        mMaxFieldSize = aMaxFieldSize;
    }

    public void setMaxRows(int aMaxRows) throws SQLException
    {
        mLogger.logTrace();
        if (aMaxRows < 0)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, String.valueOf(aMaxRows));
        }
        checkClosed();
        mMaxRows = aMaxRows;
    }

    public void setQueryTimeout(int aQueryTimeout) throws SQLException
    {
        mLogger.logTrace();
        if (aQueryTimeout < -1)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, String.valueOf(aQueryTimeout));
        }
        mSetQueryTimeoutOp.setValue(aQueryTimeout);
    }    
}
