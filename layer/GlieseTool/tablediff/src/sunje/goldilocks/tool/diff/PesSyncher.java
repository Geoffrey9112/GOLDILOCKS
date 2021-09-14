package sunje.goldilocks.tool.diff;

import java.sql.Connection;
import java.sql.SQLException;

public class PesSyncher extends Syncher
{
    protected PesSyncher(JobQueue aQueue, Connection aTargetCon, String aTargetTable, TableColumn aRowId, TableColumnList aKey, TableColumnList aRow, String aOutFileName, boolean aSuccessLog, int aMaxBatchCount) throws SQLException
    {
        super(aQueue, aTargetCon, aTargetTable, aRowId, aKey, aRow, aOutFileName, aSuccessLog, aMaxBatchCount);
        try
        {
            mJobs[Job.JOB_TYPE_DELETE] = new JobSqlDelete(mTargetCon, aTargetTable, aRowId, aRow, aMaxBatchCount);
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
        if (mJobs[Job.JOB_TYPE_DELETE] == null)
        {
            return 0;
        }
        return mJobs[Job.JOB_TYPE_DELETE].getSuccessCount();
    }

    @Override
    public int getInsertOrDeleteFailureCount()
    {
        if (mJobs[Job.JOB_TYPE_DELETE] == null)
        {
            return 0;
        }
        return mJobs[Job.JOB_TYPE_DELETE].getFailureCount();
    }

}
