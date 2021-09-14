package sunje.goldilocks.tool.diff;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

import sunje.goldilocks.jdbc.GoldilocksConnection;

public class TableSync
{
    private static boolean mTimePrint = true;
    private TableColumn mRowId;
    private TableColumnList mKey;
    private TableColumnList mRow;
    private SessionEnv mEnv;
    private JobQueue mQueue;
    private Syncher[] mSynchers;
    private FileInByteBuffer mBuf;
    private Connection mSourceSyncCon[] = null;
    private Connection mTargetSyncCon[] = null;
    
    /*
     * ascii character만 읽을 수 있다.
     */
    
    private void readColInfo() throws SQLException
    {
        mRowId = new TableColumn(mEnv, mBuf.readInt(), mBuf.readString(), mBuf.readByte(), mBuf.readLong(), mBuf.readLong(), mBuf.readByte(), mBuf.readByte());
        mRowId.setToRowId();
        
        int sKeyColCount = mBuf.readInt();
        TableColumn[] sKeyCols = new TableColumn[sKeyColCount];
        for (int i=0; i<sKeyColCount; i++)
        {
            sKeyCols[i] = new TableColumn(mEnv, mBuf.readInt(), mBuf.readString(), mBuf.readByte(), mBuf.readLong(), mBuf.readLong(), mBuf.readByte(), mBuf.readByte());
        }
        int sRowColCount = mBuf.readInt();
        TableColumn[] sRowCols = new TableColumn[sRowColCount];
        for (int i=0; i<sRowColCount; i++)
        {
            sRowCols[i] = new TableColumn(mEnv, mBuf.readInt(), mBuf.readString(), mBuf.readByte(), mBuf.readLong(), mBuf.readLong(), mBuf.readByte(), mBuf.readByte());
        }
        mKey = new TableColumnList(sKeyCols);
        mRow = new TableColumnList(sRowCols);
    }
        
    public void start(String aUrl1, String aUser1, String aPasswd1, String aSchema1, String aTable1,
                      String aUrl2, String aUser2, String aPasswd2, String aSchema2, String aTable2,
                      boolean aTargetInsert, boolean aTargetUpdate, boolean aTargetDelete, boolean aSourceInsert, String aOutFileName, boolean aSuccessLog,
                      String aFileName, int aQueueSize, int aThreadSize, int aJobUnitSize, int aDisplayRowUnit, String[] aPartitionNames) throws SQLException
    {
        Connection sTempCon = DriverManager.getConnection(aUrl1, aUser1, aPasswd1);
        mEnv = new SessionEnv(((GoldilocksConnection)sTempCon).getNewDecoder().charset(), ((GoldilocksConnection)sTempCon).getChannel().getByteOrder());
        sTempCon.close();

        long sRowCount = 0;
        long sPrintCount = aDisplayRowUnit;
        long sBefore = System.currentTimeMillis();
        
        try
        {
            for (int sParts=0; sParts<aPartitionNames.length; sParts++)
            {
                mBuf = new FileInByteBuffer(aFileName+aPartitionNames[sParts]);
                readColInfo();
                if (mQueue == null)
                {
                    mQueue = new JobQueue(mRowId, mKey, mRow, aQueueSize, aJobUnitSize);
                    mSynchers = new Syncher[aThreadSize];
                    mSourceSyncCon = new Connection[aThreadSize];
                    mTargetSyncCon = new Connection[aThreadSize];
                    for (int i=0; i<mSynchers.length; i++)
                    {
                        mSynchers[i] = null;
                        mSourceSyncCon[i] = null;
                        mTargetSyncCon[i] = null;
                    }
                    for (int i=0; i<mSynchers.length; i++)
                    {
                        mTargetSyncCon[i] = DriverManager.getConnection(aUrl2, aUser2, aPasswd2);
                        if (aSourceInsert)
                        {
                            mSourceSyncCon[i] = DriverManager.getConnection(aUrl1, aUser1, aPasswd1);
                        }
                        String sSyncOut = aOutFileName;
                        if (mSynchers.length > 1)
                        {
                            sSyncOut = sSyncOut + i;
                        }
                        mSynchers[i] = Syncher.createSyncher(mQueue, mSourceSyncCon[i], aTable1, mTargetSyncCon[i], aTable2, mRowId, mKey, mRow, sSyncOut, aSuccessLog, aJobUnitSize);
                        mSynchers[i].start();
                    }
                }
                
                while (mBuf.remains())
                {
                    int sJobCount = mQueue.enqueueUnit(mBuf, aJobUnitSize);
                    sRowCount += sJobCount;
                    if (sRowCount >= sPrintCount)
                    {
                        System.out.println(sRowCount/aDisplayRowUnit*aDisplayRowUnit + " rows are processed");
                        sPrintCount += aDisplayRowUnit;
                    }
                    if (sJobCount < aJobUnitSize)
                    {
                        /* mBuf를 다 읽었다. */
                        break;
                    }
                }
                
                mBuf.close();
            }
            mQueue.end();
            
            for (int i=0; i<mSynchers.length; i++)
            {
                try
                {
                    mSynchers[i].join();
                }
                catch (InterruptedException sException)
                {
                }
            }
        }
        finally
        {
            closeAll();
        }
        
        long sAfter = System.currentTimeMillis();
        long sInsertSuccess = 0;
        long sInsertFailure = 0;
        long sUpdateSuccess = 0;
        long sUpdateFailure = 0;
        long sDeleteSuccess = 0;
        long sDeleteFailure = 0;
        long sRowDiff = 0;
        long sKeyDiffSourceOnly = 0;
        long sKeyDiffTargetOnly = 0;
        long sTotalUpdated = 0;
        for (int i=0; i<mSynchers.length; i++)
        {
            sInsertSuccess += mSynchers[i].getTargetInsertedCount();
            sInsertFailure += mSynchers[i].getTargetInsertFailureCount();
            sUpdateSuccess += mSynchers[i].getTargetUpdatedCount();
            sUpdateFailure += mSynchers[i].getTargetUpdateFailureCount();
            sDeleteSuccess += mSynchers[i].getInsertedOrDeletedCount();
            sDeleteFailure += mSynchers[i].getInsertOrDeleteFailureCount();
            sTotalUpdated += mSynchers[i].getTotalUpdatedRowCount();
            sRowDiff += mSynchers[i].getTargetUpdatedCount() + mSynchers[i].getTargetUpdateFailureCount();
            sKeyDiffSourceOnly += mSynchers[i].getTargetInsertedCount() + mSynchers[i].getTargetInsertFailureCount();
            sKeyDiffTargetOnly += mSynchers[i].getInsertedOrDeletedCount() + mSynchers[i].getInsertOrDeleteFailureCount();
        }

        TableDiff.printReport(false, sRowCount, sAfter - sBefore, aSourceInsert, sRowDiff, sKeyDiffSourceOnly, sKeyDiffTargetOnly, sUpdateSuccess, sUpdateFailure, sInsertSuccess, sInsertFailure, sDeleteSuccess, sDeleteFailure, sTotalUpdated, mTimePrint);
    }
    
    private void closeAll()
    {
        // syncher들을 free해준다; statement들을 해제해준다.
        for (int i=0; i<mSynchers.length; i++)
        {
            if (mSynchers[i] != null)
            {
                try
                {
                    mSynchers[i].freeAll();
                }
                catch (SQLException sException)
                {
                }
            }
        }
        
        if (mSourceSyncCon != null)
        {
            for (int i=0; i<mSourceSyncCon.length; i++)
            {
                try
                {
                    if (mSourceSyncCon[i] != null && !mSourceSyncCon[i].isClosed())
                    {
                        mSourceSyncCon[i].close();                        
                    }
                }
                catch (SQLException sException)
                {
                }
            }                
        }
        if (mTargetSyncCon != null)
        {
            for (int i=0; i<mTargetSyncCon.length; i++)
            {
                try
                {
                    if (mTargetSyncCon[i] != null && !mTargetSyncCon[i].isClosed())
                    {
                        mTargetSyncCon[i].close();                        
                    }
                }
                catch (SQLException sException)
                {
                }
            }                
        }
    }
    
    public static void mainForDebug(String[] args)
    {
        try
        {
            Class.forName("sunje.goldilocks.jdbc.GoldilocksDriver");
        }
        catch (ClassNotFoundException sException)
        {
            sException.printStackTrace();
        }

        TableSync sSync = new TableSync();
        try
        {            
            sSync.start("jdbc:goldilocks://127.0.0.1:22581/test",
                        "TEST",
                        "test",
                        "PUBLIC",
                        "T1",
                        "jdbc:goldilocks://127.0.0.1:22581/test",
                        "TEST",
                        "test",
                        "PUBLIC",
                        "T2",
                        true,
                        true,
                        true,
                        false,
                        "/home/egon/workspace/product/regress/Gliese/tablediff/tablesync.log",
                        true,
                        "/home/egon/workspace/product/regress/Gliese/tablediff/tablediff.bin",
                        100,
                        4,
                        100,
                        100000,
                        new String[]{""});
        }
        catch (SQLException sException)
        {
            System.out.println("Error: " + sException.getMessage());
            System.exit(1);
        }
    }
    
    public static void main(String[] args)
    {
        try
        {
            Class.forName("sunje.goldilocks.jdbc.GoldilocksDriver");
        }
        catch (ClassNotFoundException sException)
        {
            sException.printStackTrace();
        }

        if (args.length < 1)
        {
            System.out.println("usage> java sunje.goldilocks.tool.diff.TableSync [configure file]");
            System.exit(0);
        }
    
        Properties sProp = null;
        
        try
        {
            sProp = TableDiff.parseConfig(args[0]);
        }
        catch (Exception sException)
        {
            System.out.println("Error at paring the configure file: " + sException.getMessage());
            System.exit(1);
        }
        
        try
        {
            if (sProp.getProperty(TableDiff.KEY_TARGET_INSERT) == null || sProp.getProperty(TableDiff.KEY_TARGET_INSERT).length() == 0)
            {
                throw new SQLException(TableDiff.KEY_TARGET_INSERT + " property is not defined");
            }
            if (sProp.getProperty(TableDiff.KEY_TARGET_UPDATE) == null || sProp.getProperty(TableDiff.KEY_TARGET_UPDATE).length() == 0)
            {
                throw new SQLException(TableDiff.KEY_TARGET_UPDATE + " property is not defined");
            }
            if (sProp.getProperty(TableDiff.KEY_TARGET_DELETE) == null || sProp.getProperty(TableDiff.KEY_TARGET_DELETE).length() == 0)
            {
                throw new SQLException(TableDiff.KEY_TARGET_DELETE + " property is not defined");
            }
            if (sProp.getProperty(TableDiff.KEY_SOURCE_INSERT) == null || sProp.getProperty(TableDiff.KEY_SOURCE_INSERT).length() == 0)
            {
                throw new SQLException(TableDiff.KEY_SOURCE_INSERT + " property is not defined");
            }
        }
        catch (SQLException sException)
        {
            System.out.println("Error: " + sException.getMessage());
            System.exit(1);
        }
        
        if (args.length > 1)
        {
            for (int i=1; i<args.length; i++)
            {
                if (args[i].equalsIgnoreCase("-p"))
                {
                    TableDiff.modifyPortNumber(sProp, args[i+1]);
                    i++;
                }
                else if (args[i].equalsIgnoreCase("-n"))
                {
                    mTimePrint = false;
                }
            }
        }
        
        Properties sPartitions = (Properties)sProp.get(TableDiff.KEY_PARTITION_RANGE);
        if (sPartitions == null)
        {
            sPartitions = new Properties();
            sPartitions.put("", "");
        }
        String[] sPartNames = sPartitions.keySet().toArray(new String[0]);
        TableSync sSync = new TableSync();
        try
        {
            sSync.start(sProp.getProperty(TableDiff.KEY_SOURCE_URL),
                        sProp.getProperty(TableDiff.KEY_SOURCE_USER),
                        sProp.getProperty(TableDiff.KEY_SOURCE_PASSWORD),
                        sProp.getProperty(TableDiff.KEY_SOURCE_SCHEMA),
                        sProp.getProperty(TableDiff.KEY_SOURCE_TABLE),
                        sProp.getProperty(TableDiff.KEY_TARGET_URL),
                        sProp.getProperty(TableDiff.KEY_TARGET_USER),
                        sProp.getProperty(TableDiff.KEY_TARGET_PASSWORD),
                        sProp.getProperty(TableDiff.KEY_TARGET_SCHEMA),
                        sProp.getProperty(TableDiff.KEY_TARGET_TABLE),
                        sProp.getProperty(TableDiff.KEY_TARGET_INSERT).equalsIgnoreCase("ON"),
                        sProp.getProperty(TableDiff.KEY_TARGET_UPDATE).equalsIgnoreCase("ON"),
                        sProp.getProperty(TableDiff.KEY_TARGET_DELETE).equalsIgnoreCase("ON"),
                        sProp.getProperty(TableDiff.KEY_SOURCE_INSERT).equalsIgnoreCase("ON"),
                        sProp.getProperty(TableDiff.KEY_SYNC_OUT_FILE),
                        TableDiff.getBooleanValue(sProp.getProperty(TableDiff.KEY_LOGGING_ON_SUCCESS), false),
                        sProp.getProperty(TableDiff.KEY_DIFF_BIN_FILE),
                        TableDiff.getIntValue(sProp.getProperty(TableDiff.KEY_JOB_QUEUE_SIZE), 100),
                        TableDiff.getIntValue(sProp.getProperty(TableDiff.KEY_JOB_THREAD), 4),
                        TableDiff.getIntValue(sProp.getProperty(TableDiff.KEY_JOB_UNIT_SIZE), 100),
                        TableDiff.getIntValue(sProp.getProperty(TableDiff.KEY_DISPLAY_ROW_UNIT), 100000),
                        sPartNames);
        }
        catch (SQLException sException)
        {
            System.out.println("Error: " + sException.getMessage());
            if (TableDiff.getBooleanValue(sProp.getProperty(TableDiff.KEY_DISPLAY_CALL_STACK), false))
            {
                System.out.println("[Call Stack]");
                sException.printStackTrace();
            }
            System.exit(1);
        }
    }
}
