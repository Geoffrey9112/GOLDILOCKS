package sunje.goldilocks.tool.diff;

import java.sql.SQLException;


class Job
{
    /*
     * 숫자값 바꾸면 안됨. array index로 사용됨
     */
    static final byte JOB_TYPE_INSERT = 0;
    static final byte JOB_TYPE_UPDATE = 1;
    static final byte JOB_TYPE_DELETE = 2;
    static final byte JOB_TYPE_INSERT_TO_SOURCE = 3;
    static final byte JOB_TYPE_COUNT = 4;
    
    boolean mAssigned;
    Object mLock;
    int mJobCount;
    boolean mBatchPossible;
    int[] mJobType;
    TableColumn[] mRowId;
    TableColumnList[] mKey;
    TableColumnList[] mRow;
}

/*
 * 1-thread enqueue, n-thread dequeue 가능한 잡큐
 */
public class JobQueue
{
    private Job[] mQueue;
    private int mEnqueuePos;
    private int mDequeuePos;
    private boolean mEnd = false;
    
    public JobQueue(TableColumn aRowId, TableColumnList aKeyOrigin, TableColumnList aRowOrigin, int aSize, int aJobUnitSize)
    {
        mQueue = new Job[aSize];
        for (int i=0; i<aSize; i++)
        {
            mQueue[i] = new Job();
            mQueue[i].mAssigned = false;
            mQueue[i].mLock = new Object();
            
            mQueue[i].mJobType = new int[aJobUnitSize];
            mQueue[i].mRowId = new TableColumn[aJobUnitSize];
            mQueue[i].mKey = new TableColumnList[aJobUnitSize];
            mQueue[i].mRow = new TableColumnList[aJobUnitSize];
            
            for (int j=0; j<aJobUnitSize; j++)
            {
                mQueue[i].mRowId[j] = aRowId.cloneCode();
                mQueue[i].mKey[j] = aKeyOrigin.cloneCode();
                mQueue[i].mRow[j] = aRowOrigin.cloneCode();
            }
        }
        mEnqueuePos = 0;
        mDequeuePos = 0;
    }
    
    private int nextPos(int aCurrent)
    {
        if (aCurrent == mQueue.length - 1)
        {
            return 0;
        }
        else
        {
            return aCurrent + 1;
        }        
    }
    
    public void enqueue(int aJobType, TableColumn aRowId, TableColumnList aKeyOrgin, TableColumnList aRowOrigin)
    {
        /*
         * enqueue는 하나의 쓰레드에 의해서만 불려야 한다.
         */
        Job sJob = mQueue[mEnqueuePos];
        synchronized (sJob.mLock)
        {
            while (sJob.mAssigned == true)
            {
                try
                {
                    sJob.mLock.wait();
                }
                catch (InterruptedException sException)
                {
                }
            }
        }

        sJob.mJobCount = 1;
        sJob.mBatchPossible = false;
        sJob.mJobType[0] = aJobType;
        if (aJobType == Job.JOB_TYPE_INSERT)
        {
            sJob.mKey[0].copyDataFrom(aKeyOrgin);
            sJob.mRow[0].copyDataFrom(aRowOrigin);
        }
        else if (aJobType == Job.JOB_TYPE_UPDATE)
        {
            sJob.mRowId[0].copyDataFrom(aRowId);
            sJob.mKey[0].copyDataFrom(aKeyOrgin);
            sJob.mRow[0].copyDataFrom(aRowOrigin);
        }
        else if (aJobType == Job.JOB_TYPE_DELETE)
        {            
            sJob.mRowId[0].copyDataFrom(aRowId);
            sJob.mKey[0].copyDataFrom(aKeyOrgin);
        }
        else if (aJobType == Job.JOB_TYPE_INSERT_TO_SOURCE)
        {
            sJob.mKey[0].copyDataFrom(aKeyOrgin);
            sJob.mRow[0].copyDataFrom(aRowOrigin);
        }
        int sNext = nextPos(mEnqueuePos);
        synchronized (this)
        {
            while (sNext == mDequeuePos)
            {
                try
                {
                    wait();
                }
                catch (InterruptedException sException)
                {
                }
            }
            sJob.mAssigned = true;
            mEnqueuePos = sNext;
            notifyAll();
        }
    }
    
    public int enqueueUnit(FileInByteBuffer aBuf, int aUnitSize) throws SQLException
    {
        /*
         * enqueue는 하나의 쓰레드에 의해서만 불려야 한다.
         */
        Job sJob = mQueue[mEnqueuePos];
        synchronized (sJob.mLock)
        {
            while (sJob.mAssigned == true)
            {
                try
                {
                    sJob.mLock.wait();
                }
                catch (InterruptedException sException)
                {
                }
            }
        }

        int i=0;
        boolean sBatchPossible = true;
        for (; i<aUnitSize; i++)
        {
            if (!aBuf.remains())
            {
                break;
            }
            int sJobType = aBuf.readByte();
            sJob.mJobType[i] = sJobType;
            if (i > 0 && sBatchPossible)
            {
                if (sJob.mJobType[i-1] != sJobType)
                {
                    sBatchPossible = false;
                }
            }
            if (sJobType == Job.JOB_TYPE_INSERT)
            {
                sJob.mKey[i].readDataFrom(aBuf);
                sJob.mRow[i].readDataFrom(aBuf);
            }
            else if (sJobType == Job.JOB_TYPE_UPDATE)
            {
                sJob.mRowId[i].readDataFrom(aBuf);
                sJob.mKey[i].readDataFrom(aBuf);
                sJob.mRow[i].readDataFrom(aBuf);
            }
            else if (sJobType == Job.JOB_TYPE_DELETE)
            {
                sJob.mRowId[i].readDataFrom(aBuf);
                sJob.mKey[i].readDataFrom(aBuf);
            }
            else if (sJobType == Job.JOB_TYPE_INSERT_TO_SOURCE)
            {
                sJob.mKey[i].readDataFrom(aBuf);
                sJob.mRow[i].readDataFrom(aBuf);
            }
        }
        
        sJob.mBatchPossible = sBatchPossible;
        sJob.mJobCount = i;
        int sNext = nextPos(mEnqueuePos);
        synchronized (this)
        {
            while (sNext == mDequeuePos)
            {
                try
                {
                    wait();
                }
                catch (InterruptedException sException)
                {
                }
            }
            sJob.mAssigned = true;
            mEnqueuePos = sNext;
            notifyAll();
        }
        
        /*
         * aBuf의 remain 여부를 반환한다; false-더이상 읽을게 없음. true-알 수 없음(remain()해봐야 알 수 있다)
         */
        return i;
    }
    
    public Job dequeue()
    {
        int sPos = 0;
        synchronized (this)
        {
            while (mDequeuePos == mEnqueuePos)
            {
                if (mEnd)
                {
                    return null;
                }
                try
                {
                    wait();
                }
                catch (InterruptedException sException)
                {
                }
            }
            sPos = mDequeuePos;
            mDequeuePos = nextPos(mDequeuePos);
            notifyAll();
        }

        return mQueue[sPos];
    }
 
    public void done(Job aJob)
    {
        synchronized (aJob.mLock)
        {
            aJob.mAssigned = false;
            aJob.mLock.notify();
        }
    }

    public synchronized void end()
    {
        mEnd = true;
        notifyAll();
    }
    
    public void reset()
    {
        mEnd = false;
        mEnqueuePos = 0;
        mDequeuePos = 0;
    }
}
