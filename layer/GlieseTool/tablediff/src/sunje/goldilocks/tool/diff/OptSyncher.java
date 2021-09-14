package sunje.goldilocks.tool.diff;

import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;

public class OptSyncher extends Syncher
{
    private Connection mSourceCon;
    
    protected OptSyncher(JobQueue aQueue, Connection aSourceCon, String aSourceTable, Connection aTargetCon, String aTargetTable, TableColumn aRowId, TableColumnList aKey, TableColumnList aRow, String aOutFileName, boolean aSuccessLog, int aMaxBatchCount) throws SQLException
    {
        super(aQueue, aTargetCon, aTargetTable, aRowId, aKey, aRow, aOutFileName, aSuccessLog, aMaxBatchCount);
        mSourceCon = aSourceCon;
        
        try
        {
            Statement sStmt = mSourceCon.createStatement();
            sStmt.execute("alter session set block_read_count=20");
            sStmt.close();
            mJobs[Job.JOB_TYPE_INSERT_TO_SOURCE] = new JobSqlInsert(true, mSourceCon, aSourceTable, aKey, aRow, aMaxBatchCount);
        }
        catch (SQLException sException)
        {
            freeAll();
            throw sException;
        }
    }
    
    @Override
    public int getInsertedOrDeletedCount()
    {
        if (mJobs[Job.JOB_TYPE_INSERT_TO_SOURCE] == null)
        {
            return 0;
        }
        return mJobs[Job.JOB_TYPE_INSERT_TO_SOURCE].getSuccessCount();
    }

    @Override
    public int getInsertOrDeleteFailureCount()
    {
        if (mJobs[Job.JOB_TYPE_INSERT_TO_SOURCE] == null)
        {
            return 0;
        }
        return mJobs[Job.JOB_TYPE_INSERT_TO_SOURCE].getFailureCount();
    }
}
