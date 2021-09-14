package sunje.goldilocks.jdbc.core4;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.net.URL;
import java.sql.Array;
import java.sql.Blob;
import java.sql.Clob;
import java.sql.Date;
import java.sql.ParameterMetaData;
import java.sql.PreparedStatement;
import java.sql.Ref;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.sql.Time;
import java.sql.Timestamp;
import java.sql.Types;
import java.util.Calendar;

import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Dependency;
import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.cm.SourceBindData;
import sunje.goldilocks.jdbc.cm.SourceBindType;
import sunje.goldilocks.jdbc.cm.SourceControlPacket;
import sunje.goldilocks.jdbc.cm.SourceNumParams;
import sunje.goldilocks.jdbc.cm.SourceNumResultCols;
import sunje.goldilocks.jdbc.dt.CodeColumn;
import sunje.goldilocks.jdbc.dt.Column;
import sunje.goldilocks.jdbc.dt.RowCache;
import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.internal.BaseProtocolSource;
import sunje.goldilocks.jdbc.internal.ErrorHolder;
import sunje.goldilocks.jdbc.util.Utils;
import sunje.goldilocks.sql.GoldilocksInterval;
import sunje.goldilocks.sql.GoldilocksTypes;

public abstract class Jdbc4PreparedStatement extends Jdbc4Statement implements PreparedStatement
{
    private static final int DEFAULT_ARRAY_SIZE = 100;
    private static final int EXECUTION_MODE_NORMAL = 1;
    private static final int EXECUTION_MODE_BATCH = 2;
    private static final int EXECUTION_MODE_ARRAY = 3;

    protected class StmtAttrArraySize extends CommonMutableStmtAttr
    {
        static final int ARRAY_SIZE_NONE_ARRAY = 1;
        
        private StmtAttrAtomicExecution mAtomicExec;
        
        StmtAttrArraySize(ErrorHolder aErrorHolder, StmtAttrAtomicExecution aAtomicExec)
        {
            super(aErrorHolder, ARRAY_SIZE_NONE_ARRAY);
            mAtomicExec = aAtomicExec;
        }

        public int getAttributeType()
        {
            return Protocol.STMT_ATTR_TYPE_PARAMSET_SIZE; 
        }
        
        @Override
        void setValue(int aNewValue)
        {
            super.setValue(aNewValue);
            if (aNewValue > ARRAY_SIZE_NONE_ARRAY)
            {
                mAtomicExec.setValue(Protocol.STMT_ATTR_VALUE_ATOMIC_EXECUTION_ON);
            }
            else
            {
                mAtomicExec.setValue(Protocol.STMT_ATTR_VALUE_ATOMIC_EXECUTION_OFF);
            }
        }
    }
    
    protected class StmtAttrAtomicExecution extends CommonMutableStmtAttr
    {
        StmtAttrAtomicExecution(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder, Protocol.STMT_ATTR_VALUE_ATOMIC_EXECUTION_OFF);
        }

        public int getAttributeType()
        {
            return Protocol.STMT_ATTR_TYPE_ATOMIC_EXECUTION; 
        }
    }
    
    protected class NumColsOp extends BaseProtocolSource implements SourceNumParams, SourceNumResultCols
    {
        private int mBindColCount;
        private int mResultColCount;
        
        NumColsOp(ErrorHolder aErrorHolder)
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

        public void setResultColCount(int aColCount)
        {
            mResultColCount = aColCount;
        }

        public void setParamCount(int aColCount)
        {
            mBindColCount = aColCount;
        }
        
        int getResultColCount()
        {
            return mResultColCount;
        }
        
        int getParamCount()
        {
            return mBindColCount;
        }
    }

    protected class BindOp extends BaseProtocolSource implements SourceBindData, SourceBindType
    {
        protected Column[] mBindColumns;
        protected Column mCurrentColumn;
        protected boolean mColumnChanged = false;

        BindOp(ErrorHolder aErrorHolder, int aParamCount)
        {
            super(aErrorHolder);
            mBindColumns = new Column[aParamCount];
        }
        
        public void addBindTypeProtocol(Dependency aDependency) throws SQLException
        {
            /*
             * bind type이 바뀌거나, array size 속성이 써졌거나 atomic 옵션 속성이 써졌으면
             * BindType을 다시 해야 한다.
             */
            if (mColumnChanged || mSetAtomicOp.isModified() || mSetArraySizeOp.isModified())
            {
                aDependency = aDependency.derive();
                for (Column sCol : mBindColumns)
                {
                    mCurrentColumn = sCol;
                    if (sCol == mBindColumns[mBindColumns.length-1])
                    {
                        aDependency.last();
                    }
                    mChannel.writeProtocol(Protocol.PROTOCOL_BIND_TYPE, mBindOp, aDependency);
                }
                mColumnChanged = false;
            }
        }
        
        public Column getTypedColumn(int aColNum, byte aDataType, byte aIntervalInd) throws SQLException
        {
            if (aColNum <= 0)
            {
                ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "column index=" + aColNum);
            }
            
            int sColIndex = aColNum - 1;
            if (mBindColumns[sColIndex] == null ||
                mBindColumns[sColIndex].getCode().getDataType() != aDataType)
            {
                if (mBindColumns[sColIndex] != null && mBatchAdded)
                {
                    ErrorMgr.raise(ErrorMgr.INVALID_PARAMETER_TYPE_DURING_BATCH,
                                   CodeColumn.getCodeColumn(aDataType, aIntervalInd).getTypeName(),
                                   String.valueOf(mBindColumns[sColIndex].getCode().getTypeName()));
                }
                mBindColumns[sColIndex] = Column.createBindColumn(sColIndex,
                                                                  aDataType,
                                                                  Column.BIND_TYPE_INPUT,
                                                                  aIntervalInd,
                                                                  mCache);
                mColumnChanged = true;
            }
            return mBindColumns[sColIndex];
        }
        
        void checkParameters() throws SQLException
        {
            int sColIdx = 1;
            for (Column sCol : mBindColumns)
            {
                if (sCol == null)
                {
                    ErrorMgr.raise(ErrorMgr.INSUFFICIENT_PARAMETERS, String.valueOf(sColIdx));
                }
                sColIdx++;
            }
        }

        void clearParameters()
        {
            for (int i=0; i<mBindColumns.length; i++)
            {
                mBindColumns[i] = null;
            }
        }
        
        void storeColumnValueToCursor() throws SQLException
        {
            for (Column sCol : mBindColumns)
            {
                sCol.storeValueToCursor();
            }
        }
        
        public long getStatementId()
        {
            return mId;
        }
        
        /**
         * SourceBindType 인터페이스
         */
        public int getColumnNum()
        {
            return mCurrentColumn.getId() + 1;
        }

        public byte getBindType()
        {
            return mCurrentColumn.getBindType();
        }

        public byte getDataType()
        {
            return mCurrentColumn.getCode().getDataType();
        }

        public long getArgNum1()
        {
            return mCurrentColumn.getArgNum1();
        }

        public long getArgNum2()
        {
            return mCurrentColumn.getArgNum2();
        }

        public byte getStringLengthUnit()
        {
            return mCurrentColumn.getStringLengthUnit();
        }

        public byte getIntervalIndicator()
        {
            return mCurrentColumn.getIntervalIndicator();
        }

        public void setStatementId(long aStatementId)
        {
            mId = aStatementId;
        }
        
        /**
         * SourceBindData 인터페이스
         */
        public int getColumnCount()
        {
            return mBindColumns.length;
        }
        
        public byte getDataType(int aColIndex)
        {
            return mBindColumns[aColIndex-1].getCode().getDataType();
        }

        public long getArraySize()
        {
            if (mExecutionMode == EXECUTION_MODE_NORMAL ||
                mExecutionMode == EXECUTION_MODE_BATCH)
            {
                return 1;
            }
            else if (mExecutionMode == EXECUTION_MODE_ARRAY)
            {
                return mCache.getCount();
            }
            return 0;
        }

        /*
         * Bind Data시 실제 데이터를 프로토콜에 기록함
         */
        public void writeTo(CmChannel aChannel, int aColIndex) throws SQLException
        {
            if (mExecutionMode == EXECUTION_MODE_NORMAL || mExecutionMode == EXECUTION_MODE_ARRAY)
            {
                mCache.beforeFirst();
                while (mCache.next())
                {
                    mCache.readColumn(aChannel, mBindColumns[aColIndex-1].getId());
                }
            }
            else if (mExecutionMode == EXECUTION_MODE_BATCH)
            {
                mCache.readColumn(aChannel, mBindColumns[aColIndex-1].getId());
            }
        }
    }
    
    protected class PoolingOp extends BaseProtocolSource implements SourceControlPacket
    {
        PoolingOp(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public byte getOperation()
        {
            return Protocol.CONTROL_PACKET_OP_POOLING;
        }
    }
    
    protected RowCache mCache;
    protected boolean mBatchAdded = false;
    protected int mExecutionMode = EXECUTION_MODE_NORMAL;
    protected String mSqlString;
    protected StmtAttrAtomicExecution mSetAtomicOp;
    protected StmtAttrArraySize mSetArraySizeOp;
    protected NumColsOp mNumColsOp;
    protected BindOp mBindOp;
    protected PoolingOp mPoolingOp;
    protected Jdbc4ResultSet mLastResultSet = null;
    
    public Jdbc4PreparedStatement(Jdbc4Connection aConnection, String aSql, int aType, int aConcurrency, int aHoldability) throws SQLException
    {
        super(aConnection, aType, aConcurrency, aHoldability);
        
        mSetAtomicOp = new StmtAttrAtomicExecution(this);
        mSetArraySizeOp = new StmtAttrArraySize(this, mSetAtomicOp);
        
        mMutableStmtAttrList.add(mSetArraySizeOp);
        mMutableStmtAttrList.add(mSetAtomicOp);
        
        mSqlString = aSql;
        byte[] sSqlBytes = getBytes(aSql);
        mExecutionEnv.setSql(sSqlBytes);
        
        mNumColsOp = new NumColsOp(this);
        
        synchronized (mChannel)
        {
            mChannel.initToSend();
            mExecutionEnv.writePrepare(mChannel.mDependency.first());
            writeImmutableStmtAttr(); // 처음 prepare시 immutable attr을 한번만 보내면 된다.
            mChannel.writeProtocol(Protocol.PROTOCOL_NUM_PARAMS, mNumColsOp, mChannel.mDependency);
            mChannel.writeProtocol(Protocol.PROTOCOL_NUM_RESULT_COLS, mNumColsOp, mChannel.mDependency.last());
            mChannel.sendAndReceive();
        }
        testError();
        
        mCache = new RowCache(mNumColsOp.getParamCount(),
                              mChannel.isLittleEndian(),
                              DEFAULT_ARRAY_SIZE,
                              mConnection,
                              false);

        mBindOp = new BindOp(this, mNumColsOp.getParamCount());
        mPoolingOp = new PoolingOp(this);
        
        mCache.initToInsert();
    }

    protected void storeColumnValuesToCursor() throws SQLException
    {
        mBindOp.storeColumnValueToCursor();
        mCache.nextForInsert();
    }
    
    protected byte getVarcharTypeByLength(long aLength)
    {
        if (aLength <= CodeColumn.DEFAULT_PRECISION_VARCHAR)
        {
            /* aLength는 character 길이라 가정한다.
             * aLength가 4000보다 작으면 VARCHAR(4000)에 무조건 들어갈 수 있다.
             * 하지만 aLength가 binary 길이라면
             * 역시 VARCHAR(4000)에 들어갈 수 있기 때문에
             * binary 길이의 aLength도 이 조건을 만족하면 varchar에 넣을 수 있다.
             */
            return CodeColumn.GOLDILOCKS_DATA_TYPE_VARCHAR;
        }
        else
        {
            return CodeColumn.GOLDILOCKS_DATA_TYPE_LONGVARCHAR;
        }
    }

    protected byte getVarbinaryTypeByLength(long aLength)
    {
        if (aLength <= CodeColumn.DEFAULT_PRECISION_VARBINARY)
        {
            return CodeColumn.GOLDILOCKS_DATA_TYPE_VARBINARY;
        }
        else
        {
            return CodeColumn.GOLDILOCKS_DATA_TYPE_LONGVARBINARY;
        }
    }
    
    protected int getSqlVarcharTypeByLength(int aLength)
    {
        if (aLength <= CodeColumn.DEFAULT_PRECISION_VARCHAR)
        {
            return Types.VARCHAR;
        }
        else
        {
            return Types.LONGVARCHAR;
        }
    }

    protected int getSqlVarbinaryTypeByLength(int aLength)
    {
        if (aLength <= CodeColumn.DEFAULT_PRECISION_VARBINARY)
        {
            return Types.VARBINARY;
        }
        else
        {
            return Types.LONGVARBINARY;
        }
    }
    
    boolean isPreparedStatement()
    {
        return true;
    }

    public synchronized void addBatch() throws SQLException
    {
        mLogger.logTrace();
        mBindOp.checkParameters();
        storeColumnValuesToCursor();
        mBatchAdded = true;
    }
    
    public void addBatch(String aSql) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.INVALID_CALL_FOR_PREPARED_STMT);        
    }

    public synchronized void clearParameters() throws SQLException
    {
        mLogger.logTrace();
        if (!mBatchAdded)
        {
            mBindOp.clearParameters();
        }
    }

    public synchronized void clearBatch() throws SQLException
    {
        mLogger.logTrace();
        if (mBatchAdded)
        {
            mCache.initToInsert();
            mBatchAdded = false;
        }
    }

    public synchronized boolean execute() throws SQLException
    {
        mLogger.logTrace();
        mLogger.logQuery(mSqlString);
        
        checkClosed();
        mBindOp.checkParameters();
        
        if (mBatchAdded)
        {
            ErrorMgr.raise(ErrorMgr.BATCH_JOB_VIOLATION);
        }
        
        mExecutionMode = EXECUTION_MODE_NORMAL;
        storeColumnValuesToCursor();
        if (mExecutionEnv.hasResultSet())
        {
            /*
             * select 쿼리인 경우에는 반드시 executeQuery()를 수행해야 한다.
             * 왜냐하면 target type 프로토콜을 반드시 call해야 하는데,
             * 만약 처음 execute때 target type을 호출하지 않으면
             * 두번째 execute때 target type을 요청해도 정보가 오지 않는다. (rebuild가 되지 않는한...)
             * 따라서 select의 경우엔 반드시 target type을 갖이 요청하게 하기 위해
             * 사용자가 execute()를 호출했더라도 내부적으로 executeQuery()를 수행하도록 한다.
             */
            executeQueryInternal();
        }
        else
        {
            executeUpdateInternal();
        }
        return mExecutionEnv.getStatementType() == Protocol.EXECDIR_STMT_TYPE_SELECT_CURSOR ? true : false;
    }

    public synchronized ResultSet executeQuery() throws SQLException
    {
        mLogger.logTrace();
        mLogger.logQuery(mSqlString);
        
        checkClosed();
        mBindOp.checkParameters();
        
        if (mBatchAdded)
        {
            ErrorMgr.raise(ErrorMgr.BATCH_JOB_VIOLATION);
        }
        
        mExecutionMode = EXECUTION_MODE_NORMAL;
        storeColumnValuesToCursor();

        return executeQueryInternal();
    }

    private void executeUpdateInternal() throws SQLException
    {
        mSetArraySizeOp.setValue(StmtAttrArraySize.ARRAY_SIZE_NONE_ARRAY);
        try
        {
            synchronized (mChannel)
            {
                mChannel.initToSend();
                writeMutableStmtAttr();
                mBindOp.addBindTypeProtocol(mChannel.mDependency.first());
                mChannel.writeProtocol(Protocol.PROTOCOL_BIND_DATA, mBindOp, mChannel.mDependency);
                mExecutionEnv.writeExecute(mChannel.mDependency.last());
                mConnection.addCommitProtocolOnAutoCommit(mForInternal);
                mChannel.sendAndReceive();
            }
        }
        finally
        {
            mCache.initToInsert();
            resetMutableStmtAttrs();
        }
        testError();
    }
    
    private Jdbc4ResultSet executeQueryInternal() throws SQLException
    {
        mSetArraySizeOp.setValue(StmtAttrArraySize.ARRAY_SIZE_NONE_ARRAY);
        try
        {
            synchronized (mChannel)
            {
                mChannel.initToSend();
                writeMutableStmtAttr();
                mBindOp.addBindTypeProtocol(mChannel.mDependency.first());
                mChannel.writeProtocol(Protocol.PROTOCOL_BIND_DATA, mBindOp, mChannel.mDependency);
                mExecutionEnv.writeExecute(mChannel.mDependency);
                mConnection.addCommitProtocolOnAutoCommit(mForInternal);
                mExecutionEnv.writeFirstFetch(mChannel.mDependency.last());
                mChannel.sendAndReceive();
            }

            if (mExecutionEnv.getStatementType() != Protocol.EXECDIR_STMT_TYPE_SELECT_CURSOR)
            {
                ErrorMgr.raise(ErrorMgr.NOT_SELECT_QUERY);
            }
        }
        finally
        {
            mCache.initToInsert();
            resetMutableStmtAttrs();
        }
        testError();
        
        return mExecutionEnv.getResultSet();
    }

    public synchronized int executeUpdate() throws SQLException
    {
        mLogger.logTrace();
        mLogger.logQuery(mSqlString);
        
        checkClosed();
        mBindOp.checkParameters();
        
        if (mBatchAdded)
        {
            ErrorMgr.raise(ErrorMgr.BATCH_JOB_VIOLATION);
        }
        
        mExecutionMode = EXECUTION_MODE_NORMAL;
        storeColumnValuesToCursor();

        if (mExecutionEnv.hasResultSet())
        {
            ErrorMgr.raise(ErrorMgr.SQL_PRODUCE_RESULTSET);
        }
        else
        {
            executeUpdateInternal();
        }

        return (int)mExecutionEnv.getAffectedRowCount();
    }

    public synchronized int[] executeBatch() throws SQLException
    {
        mLogger.logTrace();
        mLogger.logQuery(mSqlString);
        
        checkClosed();
        mBindOp.checkParameters();
        
        if (!mBatchAdded)
        {
            /* #644 batch job이 없을 때 에러대신 무시하게 수정한다.
             * 조건부로 addBatch를 한 후 executeBatch를 수행할 때
             * add된 batch job이 있는지 없는지 체크한 후 execute하게 하는건 불편할 수 있다.
             */
            return new int[0];
        }
        if (mExecutionEnv.hasResultSet())
        {
            ErrorMgr.raise(ErrorMgr.SQL_PRODUCE_RESULTSET);
        }

        int sBatchCount = mCache.getCount();
        mExecutionMode = EXECUTION_MODE_BATCH;
        mCache.beforeFirst();
        mExecutionEnv.startBatchExecution(sBatchCount);
        mSetArraySizeOp.setValue(StmtAttrArraySize.ARRAY_SIZE_NONE_ARRAY);
        
        try
        {
            synchronized (mChannel)
            {
                mChannel.initToSend();
                /* 다량의 batch operation을 전송하기 위해 pooling option을 준다.
                 * 그러면 서버는 batch execution result들을 바로 send하지 않고 버퍼에 풀링한 후
                 * 나중에 한꺼번에 보내게 된다.
                 */
                mChannel.writeProtocolAlone(Protocol.PROTOCOL_CONTROL_PACKET, mPoolingOp);
                writeMutableStmtAttr();
                mBindOp.addBindTypeProtocol(mChannel.mDependency.first());
                for (int i=0; i<sBatchCount; i++)
                {
                    boolean sSendFlag = (i % mConnection.mContinuousBatchCount == mConnection.mContinuousBatchCount - 1);
                    boolean sLastFlag = (i == sBatchCount - 1);
                    
                    mCache.next();
                    mChannel.writeProtocol(Protocol.PROTOCOL_BIND_DATA, mBindOp, mChannel.mDependency);
                    mChannel.mDependency.last();
                    mExecutionEnv.writeExecute(mChannel.mDependency);
                    if (sLastFlag || sSendFlag)
                    {
                        if (sLastFlag)
                        {
                            mConnection.addCommitProtocolOnAutoCommit(mForInternal);
                        }
                        mChannel.sendAndReceive();
                        if (!sLastFlag)
                        {
                            mChannel.initToSend();
                        }
                    }
                    mChannel.mDependency.first();
                    
                    /* batch execution시 첫번째 execution을 한 다음부터
                     * 이 메소드를 불러줘야 한다. 한번만 불러주면 된다.
                     */
                    if (i == 0)
                    {
                        mExecutionEnv.invalidateFirstBatchExecution();
                    }
                }
            }
        }
        finally
        {
            /* 아래 코드는 원래 없었으나 r12817버전에 들어간 코드이다.
             * 왜 넣었는지 이유는 기억할 수 없으나 오라클은 clear를 해준다.
             * clear를 해 주는게 편리성 면에서 더 좋아보이고 클라이언트의 잠재적 오류를 방지할 수 있어
             * GOLDILOCKS도 이 정책을 따르기로 한다. 2015-10-22 
             */
            clearBatch(); 
            resetMutableStmtAttrs();
        }
        mExecutionEnv.endBatchExecution();
        testError();
        
        return mExecutionEnv.getAffectedRowCounts();
    }

    /*
     * JDBC 표준 API인 executeBatch()는 각각의 성공 여부를 따로 가지고 있지만,
     * atomic하게 처리하는게 가장 빠르기 때문에 비표준 메소드인 executeBatchAtomic()을 제공한다. 
     */
    public synchronized boolean executeBatchAtomic() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mBindOp.checkParameters();
        
        if (!mBatchAdded)
        {
            /* #644 batch job이 없을 때 에러대신 무시하게 수정한다.
             * 조건부로 addBatch를 한 후 executeBatch를 수행할 때
             * add된 batch job이 있는지 없는지 체크한 후 execute하게 하는건 불편할 수 있다.
             */
            return false;
        }
        if (mExecutionEnv.hasResultSet())
        {
            ErrorMgr.raise(ErrorMgr.SQL_PRODUCE_RESULTSET);
        }

        mExecutionMode = EXECUTION_MODE_ARRAY;
        mSetArraySizeOp.setValue(mCache.getCount());
        
        try
        {
            synchronized (mChannel)
            {
                mChannel.initToSend();
                writeMutableStmtAttr();
                mBindOp.addBindTypeProtocol(mChannel.mDependency.first());
                mChannel.writeProtocol(Protocol.PROTOCOL_BIND_DATA, mBindOp, mChannel.mDependency);
                mExecutionEnv.writeExecute(mChannel.mDependency.last());
                mConnection.addCommitProtocolOnAutoCommit(mForInternal);
                mChannel.sendAndReceive();
            }
        }
        finally
        {
            /* 아래 코드는 원래 없었으나 r12817버전에 들어간 코드이다.
             * 왜 넣었는지 이유는 기억할 수 없으나 오라클은 clear를 해준다.
             * clear를 해 주는게 편리성 면에서 더 좋아보이고 클라이언트의 잠재적 오류를 방지할 수 있어
             * GOLDILOCKS도 이 정책을 따르기로 한다. 2015-10-22 
             */
            clearBatch();
            resetMutableStmtAttrs();
        }
        
        testError();
        
        return mExecutionEnv.getStatementType() == Protocol.EXECDIR_STMT_TYPE_SELECT_CURSOR ? true : false;
    }
    
    public boolean execute(String aSql) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.INVALID_CALL_FOR_PREPARED_STMT);
        return false;
    }
    
    public ResultSet executeQuery(String aSql) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.INVALID_CALL_FOR_PREPARED_STMT);
        return null;
    }
    
    public int executeUpdate(String aSql) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.INVALID_CALL_FOR_PREPARED_STMT);
        return 0;
    }
    
    public ResultSetMetaData getMetaData() throws SQLException
    {
        mLogger.logTrace();
        
        ColumnDescription[] sCols = new ColumnDescription[mNumColsOp.getResultColCount()];
        synchronized (mChannel)
        {
            mChannel.initToSend();
            for (int i = 0; i < sCols.length; i++)
            {
                sCols[i] = new ColumnDescription(this, this, i + 1);
                sCols[i].addDescribeColProtocol(mChannel);
            }
            mChannel.sendAndReceive();
        }
        testError();
        return new Jdbc4ResultSetMetaData(sCols, mLogger);
    }

    public ParameterMetaData getParameterMetaData() throws SQLException
    {
        mLogger.logTrace();
        
        ParamDescription[] sParams = new ParamDescription[mNumColsOp.getParamCount()];
        synchronized (mChannel)
        {
            mChannel.initToSend();
            for (int i = 0; i < sParams.length; i++)
            {
                sParams[i] = new ParamDescription(this, this, i + 1);
                sParams[i].addParameterTypeProtocol(mChannel);
            }
            mChannel.sendAndReceive();
        }
        testError();
        return new Jdbc4ParameterMetaData(sParams, mLogger);
    }

    public void setArray(int aColNum, Array aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "array type");
    }

    public synchronized void setAsciiStream(int aColNum, InputStream aValue, int aLength) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, getVarcharTypeByLength(aLength), CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setBinaryStream(aValue, aLength);
        }
    }

    public synchronized void setBigDecimal(int aColNum, BigDecimal aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_NUMBER, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setBigDecimal(aValue);
        }
    }

    public synchronized void setBinaryStream(int aColNum, InputStream aValue, int aLength)
            throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, getVarbinaryTypeByLength(aLength), CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setBinaryStream(aValue, aLength);
        }
    }

    public synchronized void setBinaryStream(int aColNum, InputStream aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_LONGVARBINARY, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setBinaryStream(aValue, Column.STREAM_LENGTH_UNLIMITED);
        }
    }

    public void setBlob(int aColNum, Blob aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "blob type");        
    }

    public void setBlob(int aColNum, InputStream aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "blob type");        
    }

    public synchronized void setBoolean(int aColNum, boolean aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_BOOLEAN, CodeColumn.INTERVAL_INDICATOR_NA);
        sColumn.setBoolean(aValue);
    }

    public synchronized void setByte(int aColNum, byte aValue) throws SQLException
    {
        mLogger.logTrace();
        /* Goldilocks에 tinyint가 없기 때문에 smallint에 매핑한다. */
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_SMALLINT, CodeColumn.INTERVAL_INDICATOR_NA);
        sColumn.setByte(aValue);
    }

    public synchronized void setBytes(int aColNum, byte[] aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, getVarbinaryTypeByLength(aValue == null ? 0 : aValue.length), CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setBytes(aValue);
        }
    }

    public synchronized void setCharacterStream(int aColNum, Reader aValue, int aLength)
            throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, getVarcharTypeByLength(aLength), CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setCharacterStream(aValue, aLength);
        }
    }

    public synchronized void setCharacterStream(int aColNum, Reader aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_LONGVARCHAR, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setCharacterStream(aValue, Column.STREAM_LENGTH_UNLIMITED);
        }
    }

    public void setClob(int aColNum, Clob aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "clob type");        
    }

    public void setClob(int aColNum, Reader aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "clob type");        
    }

    public synchronized void setDate(int aColNum, Date aValue, Calendar aCalendar) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_DATE, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setDate(aValue, aCalendar);
        }
    }

    public synchronized void setDate(int aColNum, Date aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_DATE, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setDate(aValue, Utils.getLocalCalendar());
        }
    }

    public synchronized void setDouble(int aColNum, double aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_DOUBLE, CodeColumn.INTERVAL_INDICATOR_NA);
        sColumn.setDouble(aValue);
    }

    public synchronized void setFloat(int aColNum, float aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_REAL, CodeColumn.INTERVAL_INDICATOR_NA);
        sColumn.setFloat(aValue);
    }

    public synchronized void setInt(int aColNum, int aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER, CodeColumn.INTERVAL_INDICATOR_NA);
        sColumn.setInt(aValue);
    }

    public synchronized void setLong(int aColNum, long aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_BIGINT, CodeColumn.INTERVAL_INDICATOR_NA);
        sColumn.setLong(aValue);
    }

    public synchronized void setNull(int aColNum, int aTargetSqlType, String aTypeName) throws SQLException
    {
        mLogger.logTrace();
        byte[] sDataType = CodeColumn.getDataTypeForSqlType(aTargetSqlType);
        Column sColumn = mBindOp.getTypedColumn(aColNum, sDataType[0], sDataType[1]);
        sColumn.setNull();
    }

    public synchronized void setNull(int aColNum, int aTargetSqlType) throws SQLException
    {
        mLogger.logTrace();
        byte[] sDataType = CodeColumn.getDataTypeForSqlType(aTargetSqlType);
        Column sColumn = mBindOp.getTypedColumn(aColNum, sDataType[0], sDataType[1]);
        sColumn.setNull();
    }

    public synchronized void setObject(int aColNum, Object aValue, int aTargetSqlType, int aScaleOrLength)
            throws SQLException
    {
        /*
         * aScaleOrLength는 InputStream이나 Reader의 길이로만 사용된다.
         * spec에는 DECIMAL, NUMERIC 타입에 대해 소수점 아래 자리수를 정할 때도 사용된다 명시하지만,
         * NUMBER 타입에 대해 항상 최대 precision으로 서버에 전달하기 때문에 필요없다.
         */
        mLogger.logTrace();
        Column sColumn = null;
        
        byte[] sDataType = CodeColumn.getDataTypeForSqlType(aTargetSqlType);
        sColumn = mBindOp.getTypedColumn(aColNum, sDataType[0], sDataType[1]);
        
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else if (aValue instanceof Boolean)
        {
            sColumn.setBoolean(((Boolean)aValue).booleanValue());
        }
        else if (aValue instanceof Byte)
        {
            sColumn.setByte(((Byte)aValue).byteValue());
        }
        else if (aValue instanceof Short)
        {
            sColumn.setShort(((Short)aValue).shortValue());                    
        }
        else if (aValue instanceof Integer)
        {
            sColumn.setInt(((Integer)aValue).intValue());
        }
        else if (aValue instanceof Long)
        {
            sColumn.setLong(((Long)aValue).longValue());
        }
        else if (aValue instanceof Float)
        {
            sColumn.setFloat(((Float)aValue).floatValue());
        }
        else if (aValue instanceof Double)
        {
            sColumn.setDouble(((Double)aValue).doubleValue());
        }
        else if (aValue instanceof BigDecimal)
        {
            sColumn.setBigDecimal((BigDecimal)aValue);
        }
        else if (aValue instanceof String)
        {
            sColumn.setString((String)aValue);
        }
        else if (aValue instanceof byte[])
        {
            sColumn.setBytes((byte[])aValue);
        }
        else if (aValue instanceof Date)
        {
            sColumn.setDate((Date)aValue, Utils.getLocalCalendar());
        }
        else if (aValue instanceof Time)
        {
            sColumn.setTime((Time)aValue, Utils.getLocalCalendar());
        }
        else if (aValue instanceof Timestamp)
        {
            sColumn.setTimestamp((Timestamp)aValue, Utils.getLocalCalendar());
        }
        else if (aValue instanceof InputStream)
        {
            sColumn.setBinaryStream((InputStream)aValue, aScaleOrLength);
        }
        else if (aValue instanceof Reader)
        {
            sColumn.setCharacterStream((Reader)aValue, aScaleOrLength);
        }
        else if (aValue instanceof RowIdBase)            
        {
            sColumn.setRowId((RowIdBase)aValue);
        }
        else
        {
            sColumn.setObject(aValue);
        }
    }

    public synchronized void setObject(int aColNum, Object aValue, int aTargetSqlType) throws SQLException
    {
        mLogger.logTrace();
        setObject(aColNum, aValue, aTargetSqlType, Column.STREAM_LENGTH_UNLIMITED);
    }

    public void setObject(int aColNum, Object aValue) throws SQLException
    {
        mLogger.logTrace();
        if (aValue == null)
        {
            setObject(aColNum, aValue, Types.VARCHAR);
        }
        else if (aValue instanceof Boolean)
        {
            setObject(aColNum, aValue, Types.BOOLEAN);
        }
        else if (aValue instanceof Byte)
        {
            setObject(aColNum, aValue, Types.SMALLINT);
        }
        else if (aValue instanceof Short)
        {
            setObject(aColNum, aValue, Types.SMALLINT);
        }
        else if (aValue instanceof Integer)
        {
            setObject(aColNum, aValue, Types.INTEGER);
        }
        else if (aValue instanceof Long)
        {
            setObject(aColNum, aValue, Types.BIGINT);
        }
        else if (aValue instanceof Float)
        {
            setObject(aColNum, aValue, Types.REAL);
        }
        else if (aValue instanceof Double)
        {
            setObject(aColNum, aValue, Types.DOUBLE);
        }
        else if (aValue instanceof BigInteger)
        {
            setObject(aColNum, aValue, Types.BIGINT);
        }
        else if (aValue instanceof BigDecimal)
        {
            setObject(aColNum, aValue, Types.NUMERIC);
        }
        else if (aValue instanceof String)
        {
            setObject(aColNum, aValue, getSqlVarcharTypeByLength(((String)aValue).length()));
        }
        else if (aValue instanceof byte[])
        {
            setObject(aColNum, aValue, getSqlVarbinaryTypeByLength(((byte[])aValue).length));
        }
        else if (aValue instanceof Date)
        {
            setObject(aColNum, aValue, Types.DATE);
        }
        else if (aValue instanceof Time)
        {
            setObject(aColNum, aValue, Types.TIME);
        }
        else if (aValue instanceof Timestamp)
        {
            setObject(aColNum, aValue, Types.TIMESTAMP);
        }
        else if (aValue instanceof Blob)
        {
            setObject(aColNum, aValue, Types.BLOB);
        }
        else if (aValue instanceof Clob)
        {
            setObject(aColNum, aValue, Types.CLOB);
        }
        else if (aValue instanceof InputStream)
        {
            setObject(aColNum, aValue, Types.LONGVARBINARY);
        }
        else if (aValue instanceof Reader)
        {
            setObject(aColNum, aValue, Types.LONGVARCHAR);
        }
        else if (aValue instanceof GoldilocksInterval)
        {
            setObject(aColNum, aValue, ((GoldilocksInterval)aValue).getSqlType());
        }
        else if (aValue instanceof RowIdBase)
        {
            setObject(aColNum, aValue, GoldilocksTypes.ROWID);
        }
        else
        {
            ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, aValue.getClass().getName(), "any");
        }
    }

    public void setRef(int aColNum, Ref aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "ref type");        
    }

    public synchronized void setShort(int aColNum, short aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_SMALLINT, CodeColumn.INTERVAL_INDICATOR_NA);
        sColumn.setShort(aValue);
    }

    public synchronized void setString(int aColNum, String aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn;
        if (aValue == null)
        {
            sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_VARCHAR, CodeColumn.INTERVAL_INDICATOR_NA);
            sColumn.setNull();
        }
        else
        {
            sColumn = mBindOp.getTypedColumn(aColNum, getVarcharTypeByLength(aValue.length()), CodeColumn.INTERVAL_INDICATOR_NA);
            sColumn.setString(aValue);
        }
    }

    public synchronized void setTimeTimeZone(int aColNum, Time aValue, Calendar aCalendar) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_TIME_WITH_TIME_ZONE, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setTime(aValue, aCalendar);
        }
    }
    
    public synchronized void setTime(int aColNum, Time aValue, Calendar aCalendar) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_TIME, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setTime(aValue, aCalendar);
        }
    }

    public synchronized void setTime(int aColNum, Time aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_TIME, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setTime(aValue,Utils.getLocalCalendar());
        }
    }

    public synchronized void setTimestampTimeZone(int aColNum, Timestamp aValue, Calendar aCalendar)
            throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_TIMESTAMP_WITH_TIME_ZONE, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setTimestamp(aValue, aCalendar);
        }
    }

    public synchronized void setTimestamp(int aColNum, Timestamp aValue, Calendar aCalendar)
            throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_TIMESTAMP, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setTimestamp(aValue, aCalendar);
        }
    }

    public synchronized void setTimestamp(int aColNum, Timestamp aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_TIMESTAMP, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setTimestamp(aValue, Utils.getLocalCalendar());
        }
    }

    public void setURL(int aColNum, URL aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "URL type");
    }
}
