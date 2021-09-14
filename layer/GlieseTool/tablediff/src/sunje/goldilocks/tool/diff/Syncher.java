package sunje.goldilocks.tool.diff;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;

import sunje.goldilocks.jdbc.GoldilocksConnection;
import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.dt.CodeColumn;

class BatchResult
{
    int mBatchCount;
    TableColumnList[] mKey;
    SQLException[] mError;
}

abstract class JobSql extends ErrorHandler
{
    protected long mStatementId = Protocol.STATEMENT_ID_UNDEFINED;
    protected CmChannel mChannel;
    protected boolean mFirstExecution = true;
    protected boolean mIsSource;
    protected long mUpdated = 0;
    private int mExecutionCount = 0;
    private int mSuccessCount = 0;
    private int mBatchCount = 0;
    private int mBoundTypeCount;
    private int mMaxBatchCount;
    private BatchResult mBatchResult;
    
    protected static void appendParameters(StringBuffer aBuf, int aParamCount)
    {
        if (aParamCount > 0)
        {
            aBuf.append('?');
            for (int i=1; i<aParamCount; i++)
            {
                aBuf.append(',');
                aBuf.append('?');
            }
        }        
    }
    
    JobSql(boolean aIsSource, Connection aCon, String aTable, TableColumn aRowId, TableColumnList aKey, TableColumnList aRow, int aMaxBatchCount) throws SQLException
    {
        mIsSource = aIsSource;
        mChannel = ((GoldilocksConnection)aCon).getChannel();
        mMaxBatchCount = aMaxBatchCount;
        protocolPrepare(makeSql(aTable, aRowId, aKey, aRow).getBytes());
    }
    
    private void protocolPrepare(byte[] aSql) throws SQLException
    {
        mChannel.initToSend();
        
        mChannel.writeVarInt(Protocol.PROTOCOL_CODE_PREPARE);
        mChannel.writeByte(Protocol.DEPENDENCY_ALONE);
        mChannel.writeStatementId(mStatementId);
        mChannel.writeVariable(aSql, 0, aSql.length);
        
        mChannel.sendAndReceive();

        if (FetchHandler.readProtocolCommon(mChannel, Protocol.PROTOCOL_CODE_PREPARE, this))
        {
            mStatementId = mChannel.readStatementId();
            mChannel.readVarInt(); // statement type
            mChannel.readBoolean(); // has transaction
            mChannel.readBoolean(); // has resultset
            mChannel.readBoolean(); // holdability
            mChannel.readBoolean(); // concurrency
            mChannel.readByte(); // sensitivity
        }
        else
        {
            testError();
        }
    }

    void execute(TableColumn aRowId, TableColumnList aKey, TableColumnList aRow) throws SQLException
    {
        mExecutionCount++;
        clearError();
        
        mChannel.initToSend();
        byte sBindDataDependency = Protocol.DEPENDENCY_HEAD;
        if (mFirstExecution)
        {
            writeBindTypeProtocol(aRowId, aKey, aRow);
            sBindDataDependency = Protocol.DEPENDENCY_BODY;
        }
        mChannel.writeVarInt(Protocol.PROTOCOL_CODE_BINDDATA);
        mChannel.writeByte(sBindDataDependency);
        writeBindDataProtocol(aRowId, aKey, aRow);
        writeExecuteProtocol(true);
        writeCommitProtocol();
        
        mChannel.sendAndReceive();
        
        if (mFirstExecution)
        {
            readBindTypeProtocol(getBoundTypeCount(aKey, aRow));
        }
        readBindDataProtocol();
        readExecuteProtocol();
        readCommitProtocol();
        
        mFirstExecution = false;
        
        testError();
        mSuccessCount++;
    }
    
    void addBatch(TableColumn aRowId, TableColumnList aKey, TableColumnList aRow) throws SQLException
    {
        if (mBatchResult == null)
        {
            mBatchResult = new BatchResult();
            mBatchResult.mKey = new TableColumnList[mMaxBatchCount];
            mBatchResult.mError = new SQLException[mMaxBatchCount];
        }
        
        if (mBatchCount == 0)
        {
            mChannel.initToSend();
            writeControlPacketProtocol();
            mBoundTypeCount = getBoundTypeCount(aKey, aRow);
        }
        byte sBindDataDependency = Protocol.DEPENDENCY_HEAD;
        if (mFirstExecution && mBatchCount == 0)
        {
            writeBindTypeProtocol(aRowId, aKey, aRow);
            sBindDataDependency = Protocol.DEPENDENCY_BODY;
        }
        mChannel.writeVarInt(Protocol.PROTOCOL_CODE_BINDDATA);
        mChannel.writeByte(sBindDataDependency);
        writeBindDataProtocol(aRowId, aKey, aRow);
        writeExecuteProtocol(mBatchCount == 0 ? true : false);
        mBatchResult.mKey[mBatchCount] = aKey;
        mExecutionCount++;
        mBatchCount++;
    }
    
    BatchResult executeBatch() throws SQLException
    {
        if (mBatchCount > 0)
        {
            mBatchResult.mBatchCount = mBatchCount;
            clearError();
            writeCommitProtocol();
            
            mChannel.sendAndReceive();
            
            readControlPacketProtocol();
            
            if (mFirstExecution)
            {
                readBindTypeProtocol(mBoundTypeCount);
                mFirstExecution = false;
            }
            for (int i=0; i<mBatchCount; i++)
            {
                readBindDataProtocol();
                readExecuteProtocol();
                mBatchResult.mError[i] = mException;
                if (mException == null)
                {
                    mSuccessCount++;
                }
                clearError();
            }
            readCommitProtocol();
            mBatchCount = 0;
        }
        return mBatchResult;
    }
    
    void free() throws SQLException
    {
        if (mStatementId != Protocol.STATEMENT_ID_UNDEFINED)
        {
            mChannel.initToSend();
            writeFreeProtocol();
            mChannel.sendAndReceive();
            readFreeProtocol();
            mStatementId = Protocol.STATEMENT_ID_UNDEFINED;
        }
    }
    
    void writeExecuteProtocol(boolean aIsFirst) throws SQLException
    {
        mChannel.writeVarInt(Protocol.PROTOCOL_CODE_EXECUTE);
        mChannel.writeByte(Protocol.DEPENDENCY_TAIL);
        mChannel.writeStatementId(mStatementId);
        mChannel.writeBoolean(aIsFirst);
    }

    void writeControlPacketProtocol() throws SQLException
    {
        mChannel.writeVarInt(Protocol.PROTOCOL_CODE_CONTROL_PACKET);
        mChannel.writeByte(Protocol.DEPENDENCY_ALONE);
        mChannel.writeByte(Protocol.CONTROL_PACKET_OP_POOLING);
    }
    
    void writeCommitProtocol() throws SQLException
    {
        mChannel.writeVarInt(Protocol.PROTOCOL_CODE_TRANSACTION);
        mChannel.writeByte(Protocol.DEPENDENCY_ALONE);
        mChannel.writeVarInt(Protocol.TRANSACTION_COMMIT);        
    }
    
    void writeFreeProtocol() throws SQLException
    {
        mChannel.writeVarInt(Protocol.PROTOCOL_CODE_FREESTMT);
        mChannel.writeByte(Protocol.DEPENDENCY_ALONE);
        mChannel.writeStatementId(mStatementId);
        mChannel.writeVarInt(Protocol.FREESTMT_OPTION_DROP);
    }
    
    void readBindTypeProtocol(int aCount) throws SQLException
    {
        for (int i=0; i<aCount; i++)
        {
            if (FetchHandler.readProtocolCommon(mChannel, Protocol.PROTOCOL_CODE_BINDTYPE, this))
            {
                mChannel.readStatementId();
            }
        }
    }
    
    void readBindDataProtocol() throws SQLException
    {
        FetchHandler.readProtocolCommon(mChannel, Protocol.PROTOCOL_CODE_BINDDATA, this);
    }
    
    void readExecuteProtocol() throws SQLException
    {
        if (FetchHandler.readProtocolCommon(mChannel, Protocol.PROTOCOL_CODE_EXECUTE, this))
        {
            int sUpdated = (int)mChannel.readVarInt(); // affected row count
            mUpdated += sUpdated;
            mChannel.readBoolean(); // recompiled
            mChannel.readBoolean(); // has resultset
            mChannel.readBoolean(); // has transaction
            byte sDclAttrCount = mChannel.readByte();
            if (sDclAttrCount > 0)
            {
                mChannel.readVarInt(); // dcl attribute type
                byte sDclAttrValueType = mChannel.readByte();
                if (sDclAttrValueType == CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER)
                {
                    String.valueOf(mChannel.readVarInt());
                }
                else
                {
                    mChannel.readString();
                }
            }
        }
    }
    
    void readControlPacketProtocol() throws SQLException
    {
        FetchHandler.readProtocolCommon(mChannel, Protocol.PROTOCOL_CODE_CONTROL_PACKET, this);
    }
    
    void readCommitProtocol() throws SQLException
    {
        FetchHandler.readProtocolCommon(mChannel, Protocol.PROTOCOL_CODE_TRANSACTION, this);
    }
    
    void readFreeProtocol() throws SQLException
    {
        FetchHandler.readProtocolCommon(mChannel, Protocol.PROTOCOL_CODE_FREESTMT, this);
    }
    
    int getFailureCount()
    {
        return mExecutionCount - mSuccessCount;
    }
    
    int getSuccessCount()
    {
        return mSuccessCount;
    }
    
    abstract String getJobTypeName();
    protected abstract String makeSql(String aTable, TableColumn aRowId, TableColumnList aKey, TableColumnList aRow);
    protected abstract void writeBindTypeProtocol(TableColumn aRowId, TableColumnList aKey, TableColumnList aRow) throws SQLException;
    protected abstract void writeBindDataProtocol(TableColumn aRowId, TableColumnList aKey, TableColumnList aRow) throws SQLException;
    protected abstract int getBoundTypeCount(TableColumnList aKey, TableColumnList aRow);
}

class JobSqlInsert extends JobSql
{
    JobSqlInsert(boolean aIsSource, Connection aCon, String aTable, TableColumnList aKey, TableColumnList aRow, int aMaxBatchCount) throws SQLException
    {
        super(aIsSource, aCon, aTable, null, aKey, aRow, aMaxBatchCount);
    }
    
    String getJobTypeName()
    {
        return mIsSource ? "[SOURCE INSERT]" : "[TARGET INSERT]";
    }

    protected String makeSql(String aTable, TableColumn aRowId, TableColumnList aKey, TableColumnList aRow)
    {
        StringBuffer sBuf = new StringBuffer("INSERT INTO ");
        sBuf.append(aTable);
        sBuf.append('(');
        aKey.appendNameList(sBuf);
        if (aRow.getColumnCount() > 0)
        {
            sBuf.append(',');
            aRow.appendNameList(sBuf);
        }
        sBuf.append(") VALUES (");
        appendParameters(sBuf, aKey.getColumnCount() + aRow.getColumnCount());
        sBuf.append(')');
        return sBuf.toString();
    }
    
    protected void writeBindTypeProtocol(TableColumn aRowId, TableColumnList aKey, TableColumnList aRow) throws SQLException
    {
        int sColIndex = 1;
        sColIndex = aKey.writeType(mChannel, mStatementId, sColIndex, Protocol.DEPENDENCY_HEAD);
        aRow.writeType(mChannel, mStatementId, sColIndex, Protocol.DEPENDENCY_BODY);
    }
    
    protected void writeBindDataProtocol(TableColumn aRowId, TableColumnList aKey, TableColumnList aRow) throws SQLException
    {
        mChannel.writeStatementId(mStatementId);
        mChannel.writeVarInt(aKey.getColumnCount() + aRow.getColumnCount());
        mChannel.writeVarInt(1);
        aKey.writeData(mChannel);
        aRow.writeData(mChannel);
    }
    
    protected int getBoundTypeCount(TableColumnList aKey, TableColumnList aRow)
    {
        return aKey.getColumnCount() + aRow.getColumnCount();
    }
}

class JobSqlUpdate extends JobSql
{
    JobSqlUpdate(Connection aCon, String aTable, TableColumn aRowId, TableColumnList aKey, TableColumnList aRow, int aMaxBatchCount) throws SQLException
    {
        super(false, aCon, aTable, aRowId, aKey, aRow, aMaxBatchCount);
    }

    String getJobTypeName()
    {
        return "[TARGET UPDATE]";
    }
    
    protected String makeSql(String aTable, TableColumn aRowId, TableColumnList aKey, TableColumnList aRow)
    {
        StringBuffer sBuf = new StringBuffer("UPDATE ");
        sBuf.append(aTable);
        sBuf.append(" SET ");
        if (aRow.getColumnCount() == 0)
        {   /* row column이 하나도 없을 경우 update 구문이 오류가 난다.
               row column이 하나도 없다는 의미는 update할 일이 없다는 뜻인데,
               이 경우에도 update 구문은 prepare되어야 정상 수행되기 때문에 key column으로 구문은 만들어둔다. */ 
            aKey.appendSetList(sBuf, ",");
        }
        else
        {
            aRow.appendSetList(sBuf, ",");
        }
        sBuf.append(" WHERE ROWID=?");
        return sBuf.toString();
    }

    protected void writeBindTypeProtocol(TableColumn aRowId, TableColumnList aKey, TableColumnList aRow) throws SQLException
    {
        int sColIndex = 1;
        sColIndex = aRow.writeType(mChannel, mStatementId, sColIndex, Protocol.DEPENDENCY_HEAD);
        aRowId.writeType(mChannel, mStatementId, sColIndex, Protocol.DEPENDENCY_BODY);
    }

    protected void writeBindDataProtocol(TableColumn aRowId, TableColumnList aKey, TableColumnList aRow) throws SQLException
    {
        mChannel.writeStatementId(mStatementId);
        mChannel.writeVarInt(aRow.getColumnCount() + 1);
        mChannel.writeVarInt(1);
        aRow.writeData(mChannel);
        aRowId.writeData(mChannel);
    }
    
    protected int getBoundTypeCount(TableColumnList aKey, TableColumnList aRow)
    {
        return aRow.getColumnCount() + 1;
    }
}

class JobSqlDelete extends JobSql
{
    JobSqlDelete(Connection aCon, String aTable, TableColumn aRowId, TableColumnList aRow, int aMaxBatchCount) throws SQLException
    {
        super(false, aCon, aTable, aRowId, null, aRow, aMaxBatchCount);
    }

    String getJobTypeName()
    {
        return "[TARGET DELETE]";
    }

    protected String makeSql(String aTable, TableColumn aRowId, TableColumnList aKey, TableColumnList aRow)
    {
        StringBuffer sBuf = new StringBuffer("DELETE FROM ");
        sBuf.append(aTable);
        sBuf.append(" WHERE ROWID=?");
        return sBuf.toString();
    }
    
    protected void writeBindTypeProtocol(TableColumn aRowId, TableColumnList aKey, TableColumnList aRow) throws SQLException
    {
        aRowId.writeType(mChannel, mStatementId, 1, Protocol.DEPENDENCY_HEAD);
    }
    
    protected void writeBindDataProtocol(TableColumn aRowId, TableColumnList aKey, TableColumnList aRow) throws SQLException
    {
        mChannel.writeStatementId(mStatementId);
        mChannel.writeVarInt(1);
        mChannel.writeVarInt(1);
        aRowId.writeData(mChannel);
    }
    
    protected int getBoundTypeCount(TableColumnList aKey, TableColumnList aRow)
    {
        return 1;
    }
}

public abstract class Syncher extends Thread
{
    public static Syncher createSyncher(JobQueue aQueue, Connection aSourceCon, String aSourceTable, Connection aTargetCon, String aTargetTable, TableColumn aRowId, TableColumnList aKey, TableColumnList aRow, String aOutFileName, boolean aSuccessLog, int aMaxBatchCount) throws SQLException
    {
        if (aSourceCon == null)
        {
            return new PesSyncher(aQueue, aTargetCon, aTargetTable, aRowId, aKey, aRow, aOutFileName, aSuccessLog, aMaxBatchCount);
        }
        else
        {
            return new OptSyncher(aQueue, aSourceCon, aSourceTable, aTargetCon, aTargetTable, aRowId, aKey, aRow, aOutFileName, aSuccessLog, aMaxBatchCount);
        }
    }
    
    protected Connection mTargetCon;
    protected JobQueue mQueue;
    protected JobSql[] mJobs = new JobSql[Job.JOB_TYPE_COUNT];
    private BufferedWriter mOut;
    private boolean mSuccessLog;
    
    protected Syncher(JobQueue aQueue, Connection aTargetCon, String aTargetTable, TableColumn aRowId, TableColumnList aKey, TableColumnList aRow, String aOutFileName, boolean aSuccessLog, int aMaxBatchCount) throws SQLException
    {
        mTargetCon = aTargetCon;
        mQueue = aQueue;
        
        Statement sStmt = mTargetCon.createStatement();
        sStmt.execute("alter session set block_read_count=20");
        sStmt.close();

        try
        {
            mJobs[Job.JOB_TYPE_INSERT] = new JobSqlInsert(false, mTargetCon, aTargetTable, aKey, aRow, aMaxBatchCount);
            mJobs[Job.JOB_TYPE_UPDATE] = new JobSqlUpdate(mTargetCon, aTargetTable, aRowId, aKey, aRow, aMaxBatchCount);
        }
        catch (SQLException sException)
        {
            freeAll();
            throw sException;
        }

        try
        {
            if (mOut == null)
            {
                mOut = new BufferedWriter(new FileWriter(aOutFileName));
            }
        }
        catch (IOException sException)
        {
            throw new SQLException("File creation error: " + aOutFileName + ": " + sException.getMessage());
        }
        mSuccessLog = aSuccessLog;
    }
    
    public void run()
    {
        while (true)
        {
            Job sJob = mQueue.dequeue();
            if (sJob == null)
            {
                /*
                 * 작업이 끝났다.
                 */
                break;
            }
            if (!sJob.mBatchPossible)
            {
                for (int i=0; i<sJob.mJobCount; i++)
                {
                    try
                    {
                        mJobs[sJob.mJobType[i]].execute(sJob.mRowId[i], sJob.mKey[i], sJob.mRow[i]);
                        if (mSuccessLog)
                        {
                            try
                            {
                                mOut.write(mJobs[sJob.mJobType[i]].getJobTypeName() + " [" + sJob.mKey[i].getValueList() + "] [SUCCESS]");
                                mOut.newLine();
                            }
                            catch (IOException sException)
                            {
                                
                            }
                        }
                    }
                    catch (SQLException sException)
                    {
                        try
                        {
                            mOut.write(mJobs[sJob.mJobType[i]].getJobTypeName() + " [" + sJob.mKey[i].getValueList() + "] [FAILURE]: " + sException.getMessage());
                            mOut.newLine();
                        }
                        catch (IOException sException2)
                        {
                        }
                    }
                }
            }
            else
            {
                for (int i=0; i<sJob.mJobCount; i++)
                {
                    try
                    {
                        mJobs[sJob.mJobType[i]].addBatch(sJob.mRowId[i], sJob.mKey[i], sJob.mRow[i]);
                    }
                    catch (SQLException sException)
                    {
                        try
                        {
                            mOut.write(mJobs[sJob.mJobType[i]].getJobTypeName() + " [" + sJob.mKey[i].getValueList() + "] [FAILURE]: " + sException.getMessage());
                            mOut.newLine();
                        }
                        catch (IOException sException2)
                        {
                        }
                    }
                }
                for (int i=0; i<Job.JOB_TYPE_COUNT; i++)
                {
                    if (mJobs[i] != null)
                    {
                        try
                        {
                            BatchResult sResult = mJobs[i].executeBatch();
                            if (sResult != null)
                            {
                                for (int j=0; j<sResult.mBatchCount; j++)
                                {
                                    if (sResult.mError[j] == null)
                                    {
                                        if (mSuccessLog)
                                        {
                                            try
                                            {
                                                mOut.write(mJobs[i].getJobTypeName() + " [" + sResult.mKey[j].getValueList() + "] [SUCCESS]");
                                                mOut.newLine();
                                            }
                                            catch (IOException sException)
                                            {
                                                
                                            }
                                        }
                                    }
                                    else
                                    {
                                        try
                                        {
                                            mOut.write(mJobs[i].getJobTypeName() + " [" + sResult.mKey[j].getValueList() + "] [FAILURE]: " + sResult.mError[j].getMessage());
                                            mOut.newLine();
                                        }
                                        catch (IOException sException2)
                                        {
                                        }
                                    }
                                }
                            }
                        }
                        catch (SQLException sException)
                        {
                        }
                    }
                }
            }
            mQueue.done(sJob);
        }
        try
        {
            mOut.flush();
            mOut.close();
        }
        catch (IOException sException)
        {
            
        }
    }
    
    public void freeAll() throws SQLException
    {
        SQLException sFirstException = null;
        for (int i=0; i<Job.JOB_TYPE_COUNT; i++)
        {
            if (mJobs[i] != null)
            {
                try
                {
                    mJobs[i].free();
                }
                catch (SQLException sException)
                {
                    if (sFirstException == null)
                    {
                        sFirstException = sException;
                    }
                    else
                    {
                        sFirstException.setNextException(sException);
                    }
                }
            }
        }
        if (sFirstException != null)
        {
            throw sFirstException;
        }
    }
    
    public long getTotalUpdatedRowCount()
    {
        if (mJobs[Job.JOB_TYPE_INSERT] == null || mJobs[Job.JOB_TYPE_UPDATE] == null)
        {
            return 0;
        }
        long sResult = mJobs[Job.JOB_TYPE_INSERT].mUpdated + mJobs[Job.JOB_TYPE_UPDATE].mUpdated;
        if (mJobs[Job.JOB_TYPE_DELETE] != null)
        {
            sResult += mJobs[Job.JOB_TYPE_DELETE].mUpdated;
        }
        if (mJobs[Job.JOB_TYPE_INSERT_TO_SOURCE] != null)
        {
            sResult += mJobs[Job.JOB_TYPE_INSERT_TO_SOURCE].mUpdated; 
        }
        return sResult;
    }
    
    public int getTargetInsertedCount()
    {
        if (mJobs[Job.JOB_TYPE_INSERT] == null)
        {
            return 0;
        }
        return mJobs[Job.JOB_TYPE_INSERT].getSuccessCount();
    }
    
    public int getTargetUpdatedCount()
    {
        if (mJobs[Job.JOB_TYPE_UPDATE] == null)
        {
            return 0;
        }
        return mJobs[Job.JOB_TYPE_UPDATE].getSuccessCount();                
    }
    
    public abstract int getInsertedOrDeletedCount();
    
    public int getTargetInsertFailureCount()
    {
        if (mJobs[Job.JOB_TYPE_INSERT] == null)
        {
            return 0;
        }
        return mJobs[Job.JOB_TYPE_INSERT].getFailureCount();
    }
    
    public int getTargetUpdateFailureCount()
    {
        if (mJobs[Job.JOB_TYPE_UPDATE] == null)
        {
            return 0;
        }
        return mJobs[Job.JOB_TYPE_UPDATE].getFailureCount();                
    }
    
    public abstract int getInsertOrDeleteFailureCount();
}
