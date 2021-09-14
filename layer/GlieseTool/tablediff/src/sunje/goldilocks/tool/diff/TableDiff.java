package sunje.goldilocks.tool.diff;

import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintStream;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Properties;
import java.util.StringTokenizer;

public class TableDiff extends Thread
{
    public static final String KEY_SOURCE_URL = "SOURCE_URL";
    public static final String KEY_SOURCE_USER = "SOURCE_USER";
    public static final String KEY_SOURCE_PASSWORD = "SOURCE_PASSWORD";
    public static final String KEY_SOURCE_SCHEMA = "SOURCE_SCHEMA";
    public static final String KEY_SOURCE_TABLE = "SOURCE_TABLE";
    public static final String KEY_TARGET_URL = "TARGET_URL";
    public static final String KEY_TARGET_USER ="TARGET_USER";
    public static final String KEY_TARGET_PASSWORD = "TARGET_PASSWORD";    
    public static final String KEY_TARGET_SCHEMA = "TARGET_SCHEMA";
    public static final String KEY_TARGET_TABLE = "TARGET_TABLE";
    public static final String KEY_OPERATION = "OPERATION";
    public static final String KEY_EXCLUDED_COLUMNS = "EXCLUDED_COLUMNS";
    public static final String KEY_WHERE_CLAUSE = "WHERE_CLAUSE";
    public static final String KEY_SOURCE_INSERT = "SOURCE_INSERT";
    public static final String KEY_TARGET_INSERT = "TARGET_INSERT";
    public static final String KEY_TARGET_UPDATE = "TARGET_UPDATE";
    public static final String KEY_TARGET_DELETE = "TARGET_DELETE";
    public static final String KEY_SYNC_OUT_FILE = "SYNC_OUT_FILE";
    public static final String KEY_DIFF_OUT_FILE = "DIFF_OUT_FILE";
    public static final String KEY_DIFF_BIN_FILE = "DIFF_BIN_FILE";
    public static final String KEY_LOGGING_ON_SUCCESS = "LOGGING_ON_SUCCESS";
    public static final String KEY_LOGGING_ON_DIFF = "LOGGING_ON_DIFF";
    public static final String KEY_JOB_QUEUE_SIZE = "JOB_QUEUE_SIZE";
    public static final String KEY_JOB_THREAD = "JOB_THREAD";
    public static final String KEY_JOB_UNIT_SIZE = "JOB_UNIT_SIZE";
    public static final String KEY_DISPLAY_ROW_UNIT = "DISPLAY_ROW_UNIT";
    public static final String KEY_DISPLAY_CALL_STACK = "DISPLAY_CALL_STACK";
    public static final String KEY_PARTITION_RANGE = "PARTITION_RANGE";
    private static boolean mTimePrint = true;

    private FetchHandler mSource = null;
    private FetchHandler mTarget = null;
    private TableColumn mTargetRowId;
    private TableColumnList mSourceKey;
    private TableColumnList mSourceRow;
    private TableColumnList mTargetKey;
    private TableColumnList mTargetRow;
    private Syncher[] mSync;
    private boolean mSourceModifiable;
    private long mRowCount = 0;
    private boolean mTargetInsert = false;
    private boolean mTargetUpdate = false;
    private boolean mInsertOrDelete = false;
    private boolean mDiffLog = false;
    private boolean mOpSync = true;
    private PrintStream mOut;
    private FileOutByteBuffer mBin;
    private JobQueue mQueue;
    private boolean mPrintCallStack;
    private byte mDeleteJobType;
    
    private Connection mSourceFetchCon = null;
    private Connection mTargetFetchCon = null;
    private Connection mSourceSyncCon[] = null;
    private Connection mTargetSyncCon[] = null;
    
    long mInsertSuccessCount = 0;
    long mInsertFailureCount = 0;
    long mUpdateSuccessCount = 0;
    long mUpdateFailureCount = 0;
    long mDeleteSuccessCount = 0;
    long mDeleteFailureCount = 0;
    long mRowDiff = 0;
    long mKeyDiffSourceOnly = 0;
    long mKeyDiffTargetOnly = 0;
    long mTotalUpdatedRow = 0;
    boolean mEnd = false;

    public TableDiff(String aUrl1, String aUser1, String aPasswd1, String aSchema1, String aTable1,
            String aUrl2, String aUser2, String aPasswd2, String aSchema2, String aTable2, String aOperation, String[] aExclude, String aWhere, String aPartitionRange,
            boolean aTargetInsert, boolean aTargetUpdate, boolean aTargetDelete, boolean aSourceInsert, String aSyncOut, boolean aSuccessLog, String aDiffOut, boolean aDiffLog, String aDiffBin, int aJobQueueSize, int aThreadCount, boolean aPrintCallStack) throws SQLException
    {
        /*
         * validate arguments
         */
        if (aUrl1 == null)
        {
            throw new SQLException(KEY_SOURCE_URL + " is null");
        }
        if (aUser1 == null)
        {
            throw new SQLException(KEY_SOURCE_USER + " is null");
        }
        if (aPasswd1 == null)
        {
            throw new SQLException(KEY_SOURCE_PASSWORD + " is null");
        }
        if (aSchema1 == null)
        {
            throw new SQLException(KEY_SOURCE_SCHEMA + " is null");
        }
        if (aTable1 == null)
        {
            throw new SQLException(KEY_SOURCE_TABLE + " is null");
        }
        if (aUrl2 == null)
        {
            throw new SQLException(KEY_TARGET_URL + " is null");
        }
        if (aUser2 == null)
        {
            throw new SQLException(KEY_TARGET_USER + " is null");
        }
        if (aPasswd2 == null)
        {
            throw new SQLException(KEY_TARGET_PASSWORD + " is null");
        }
        if (aSchema2 == null)
        {
            throw new SQLException(KEY_TARGET_SCHEMA + " is null");
        }
        if (aTable2 == null)
        {
            throw new SQLException(KEY_TARGET_TABLE + " is null");
        }
        if (aOperation == null)
        {
            throw new SQLException(KEY_OPERATION + " is null");
        }
        if (aOperation.equalsIgnoreCase("SYNC"))
        {
            mOpSync = true;
        }
        else if (aOperation.equalsIgnoreCase("DIFF"))
        {
            mOpSync = false;
        }
        else
        {
            throw new SQLException(KEY_OPERATION + " is invalid: " + aOperation);
        }
        
        try
        {
            Class.forName("sunje.goldilocks.jdbc.GoldilocksDriver");
        }
        catch (ClassNotFoundException sException)
        {
            sException.printStackTrace();
        }
        
        mTargetInsert = aTargetInsert;
        mTargetUpdate = aTargetUpdate;
        if (aTargetDelete)
        {
            if (aSourceInsert)
            {
                throw new SQLException("Both TARGET_DELETE and SOURCE_INSERT cannot be ON");
            }
            mSourceModifiable = false;
            mInsertOrDelete = true;
            mDeleteJobType = Job.JOB_TYPE_DELETE;
        }
        else if (aSourceInsert)
        {
            mSourceModifiable = true;
            mInsertOrDelete = true;
            mDeleteJobType = Job.JOB_TYPE_INSERT_TO_SOURCE;
        }
        else
        {
            mSourceModifiable = false;
            mInsertOrDelete = false;
            mDeleteJobType = Job.JOB_TYPE_DELETE; // diff 로깅시 delete나 insert_to_source로 설정되어 있어야 로깅된다.
        }
        if (aSyncOut == null)
        {
            aSyncOut = "tablesync.log";
        }
        if (aDiffOut == null)
        {
            aDiffOut = "tablediff.log";
        }

        mDiffLog = aDiffLog;
        if (mDiffLog)
        {
            try
            {
                mOut = new PrintStream(new FileOutputStream(aDiffOut));
            }
            catch (IOException sException)
            {
                throw new SQLException("File creation error: " + aDiffOut + ": " + sException.getMessage());
            }
        }
        
        mPrintCallStack = aPrintCallStack;
        
        try
        {
            mSourceFetchCon = DriverManager.getConnection(aUrl1, aUser1, aPasswd1);
            mTargetFetchCon = DriverManager.getConnection(aUrl2, aUser2, aPasswd2);
            
            String sWhere = aWhere;
            if (sWhere == null)
            {
                if (aPartitionRange.length() == 0)
                {
                    sWhere = null;
                }
                else
                {
                    sWhere = aPartitionRange;
                }
            }
            else
            {
                if (aPartitionRange.length() == 0)
                {
                    sWhere = aWhere;
                }
                else
                {
                    sWhere = aWhere + " AND " + aPartitionRange;
                }
            }
            mSource = new FetchHandler(mSourceFetchCon, aSchema1, aTable1, aExclude, sWhere);
            mTarget = new FetchHandler(mTargetFetchCon, aSchema2, aTable2, aExclude, sWhere);
            
            mTargetRowId = mTarget.getRowId();
            mSourceKey = mSource.getKey();
            mTargetKey = mTarget.getKey();
            mSourceRow = mSource.getRow();
            mTargetRow = mTarget.getRow();
    
            try
            {
                mSourceKey.verifyCodeSame(mTargetKey);
            }
            catch (SQLException sException)
            {
                throw new SQLException("key column is different: " + sException.getMessage());
            }
            try
            {
                mSourceRow.verifyCodeSame(mTargetRow);
            }
            catch (SQLException sException)
            {
                throw new SQLException("row column is different: " + sException.getMessage());
            }
    
            mQueue = new JobQueue(mTargetRowId, mTargetKey, mTargetRow, aJobQueueSize, 1);
            if (mOpSync)
            {
                mSync = new Syncher[aThreadCount];
                mSourceSyncCon = new Connection[aThreadCount];
                mTargetSyncCon = new Connection[aThreadCount];
                for (int i=0; i<aThreadCount; i++)
                {
                    mSync[i] = null;
                    mSourceSyncCon[i] = null;
                    mTargetSyncCon[i] = null;
                }
                
                if (mSourceModifiable)
                {                    
                    for (int i=0; i<aThreadCount; i++)
                    {
                        String sSyncOut = aSyncOut;
                        if (aThreadCount > 1)
                        {
                            sSyncOut = sSyncOut + i;
                        }
                        
                        mSourceSyncCon[i] = DriverManager.getConnection(aUrl1, aUser1, aPasswd1);
                        mTargetSyncCon[i] = DriverManager.getConnection(aUrl2, aUser2, aPasswd2);
                        
                        mSync[i] = Syncher.createSyncher(mQueue,
                                                         mSourceSyncCon[i],
                                                         aSchema1+"."+aTable1,
                                                         mTargetSyncCon[i],
                                                         aSchema2+"."+aTable2,
                                                         mTargetRowId,
                                                         mTargetKey,
                                                         mTargetRow,
                                                         sSyncOut,
                                                         aSuccessLog,
                                                         1);
                    }
                }
                else
                {
                    for (int i=0; i<aThreadCount; i++)
                    {
                        String sSyncOut = aSyncOut;
                        if (aThreadCount > 1)
                        {
                            sSyncOut = sSyncOut + i;
                        }
                        mTargetSyncCon[i] = DriverManager.getConnection(aUrl2, aUser2, aPasswd2);
                        mSync[i] = Syncher.createSyncher(mQueue,
                                                         null,
                                                         null,
                                                         mTargetSyncCon[i],
                                                         aSchema2+"."+aTable2,
                                                         mTargetRowId,
                                                         mTargetKey,
                                                         mTargetRow,
                                                         sSyncOut,
                                                         aSuccessLog,
                                                         1);
                    }
                }
            }
        }
        catch (Exception sException)
        {
            closeAll();
            if (sException instanceof SQLException)
            {
                throw (SQLException)sException;
            }
            throw new SQLException(sException.getMessage());
        }
        /*
         * diff binary를 기록한다.
         */
        if (!mOpSync)
        {
            try
            {
                mBin = new FileOutByteBuffer(aDiffBin);
                mTargetRowId.writeCodeTo(mBin);
                mSourceKey.writeCodeTo(mBin);
                mSourceRow.writeCodeTo(mBin);
            }
            catch (Exception sException)
            {
                closeAll();
                throw new SQLException(sException.getMessage());
            }
        }
    }
    
    private void closeAll()
    {
        try
        {
            if (mSource != null)
            {
                mSource.free();
            }
        }
        catch (SQLException sException)
        {
        }
        try
        {
            if (mTarget != null)
            {
                mTarget.free();
            }
        }
        catch (SQLException sException)
        {
        }

        // syncher들을 free해준다; statement들을 해제해준다.
        if (mSync != null)
        {
            for (int i=0; i<mSync.length; i++)
            {
                if (mSync[i] != null)
                {
                    try
                    {
                        mSync[i].freeAll();
                    }
                    catch (SQLException sException)
                    {
                    }
                }
            }
        }

        try
        {
            if (mSourceFetchCon != null && !mSourceFetchCon.isClosed())
            {
                mSourceFetchCon.close();
            }
        }
        catch (SQLException sException)
        {
        }
        try
        {
            if (mTargetFetchCon != null && !mTargetFetchCon.isClosed())
            {
                mTargetFetchCon.close();
            }
        }
        catch (SQLException sException)
        {
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
    
    private void logDiffSourceOnly()
    {
        mOut.println("[KEY DIFF-SOURCE ONLY] [" + mSourceKey.getValueList() + "] [" + mTargetKey.getValueList() + "]");
    }
    
    private void logDiffTargetOnly()
    {
        mOut.println("[KEY DIFF-TARGET ONLY] [" + mSourceKey.getValueList() + "] [" + mTargetKey.getValueList() + "]");
    }
    
    private void logDiffRowDiff()
    {
        mOut.println("[ROW DIFF] [" + mSourceKey.getValueList() + "] [" + mSourceRow.getValueList() + "] [" + mTargetRow.getValueList() + "]");
    }
    
    private void onDiff(byte aJobType) throws SQLException
    {
        if (aJobType == Job.JOB_TYPE_UPDATE)
        {
            if (mDiffLog)
            {
                logDiffRowDiff();
            }
            if (mOpSync)
            {
                if (mTargetUpdate)
                {
                    mQueue.enqueue(Job.JOB_TYPE_UPDATE, mTargetRowId, mTargetKey, mSourceRow);
                }
            }
            else
            {
                mBin.writeByte(Job.JOB_TYPE_UPDATE);
                mTargetRowId.writeDataTo(mBin);
                mTargetKey.writeDataTo(mBin);
                mSourceRow.writeDataTo(mBin);
            }
        }
        else if (aJobType == Job.JOB_TYPE_INSERT)
        {
            if (mDiffLog)
            {
                logDiffSourceOnly();
            }
            if (mOpSync)
            {
                if (mTargetInsert)
                {
                    mQueue.enqueue(Job.JOB_TYPE_INSERT, mTargetRowId, mSourceKey, mSourceRow);
                }
            }
            else
            {
                mBin.writeByte(Job.JOB_TYPE_INSERT);
                mSourceKey.writeDataTo(mBin);
                mSourceRow.writeDataTo(mBin);
            }
        }
        else if (aJobType == Job.JOB_TYPE_DELETE)
        {
            if (mDiffLog)
            {
                logDiffTargetOnly();
            }
            if (mOpSync)
            {
                if (mInsertOrDelete)
                {
                    mQueue.enqueue(Job.JOB_TYPE_DELETE, mTargetRowId, mTargetKey, mTargetRow);
                }
            }
            else
            {
                mBin.writeByte(Job.JOB_TYPE_DELETE);
                mTargetRowId.writeDataTo(mBin);
                mTargetKey.writeDataTo(mBin);
            }
        }
        else if (aJobType == Job.JOB_TYPE_INSERT_TO_SOURCE)
        {
            if (mDiffLog)
            {
                logDiffTargetOnly();
            }
            if (mOpSync)
            {
                if (mInsertOrDelete)
                {
                    mQueue.enqueue(Job.JOB_TYPE_INSERT_TO_SOURCE, mTargetRowId, mTargetKey, mTargetRow);
                }
            }
            else
            {
                mBin.writeByte(Job.JOB_TYPE_INSERT_TO_SOURCE);
                mTargetKey.writeDataTo(mBin);
                mTargetRow.writeDataTo(mBin);
            }
        }
    }
    
    public void run()
    {
        try
        {
            if (mOpSync)
            {
                for (int i=0; i<mSync.length; i++)
                {
                    mSync[i].start();
                }
            }
            while (mSource.next())
            {
                if (mTarget.next())
                {
                    int sComp = mSourceKey.compare(mTargetKey);
                    if (sComp == 0)
                    {
                        if (mSourceRow.isSame(mTargetRow))
                        {
                            mRowCount++;
                            continue;
                        }
                        else
                        {
                            mRowDiff++;
                            onDiff(Job.JOB_TYPE_UPDATE);
                        }
                    }
                    else if (sComp < 0)
                    {
                        /*
                         * source 쪽이 더 작은 경우, target 쪽이 key가 빠진 경우이다. 
                         */
                        mKeyDiffSourceOnly++;
                        onDiff(Job.JOB_TYPE_INSERT);
                        mTarget.hold();
                    }
                    else
                    {
                        /*
                         * source 쪽이 더 큰 경우, source 쪽이 key가 빠진 경우이다.
                         */
                        mKeyDiffTargetOnly++;
                        onDiff(mDeleteJobType);
                        mSource.hold();
                    }
                }
                else
                {
                    mKeyDiffSourceOnly++;
                    onDiff(Job.JOB_TYPE_INSERT);
                }
                mRowCount++;
            }
            
            while (mTarget.next())
            {
                mKeyDiffTargetOnly++;
                onDiff(mDeleteJobType);
                mRowCount++;
            }
            
            mQueue.end();
    
            if (mDiffLog)
            {
                mOut.flush();
                mOut.close();
            }
    
            if (!mOpSync)
            {
                mBin.flush();
                mBin.close();
            }
            
            if (mOpSync)
            {
                for (int i=0; i<mSync.length; i++)
                {
                    try
                    {
                        mSync[i].join();
                    }
                    catch (InterruptedException sException)
                    {
                    }
                }
            }
        }
        catch (SQLException sException)
        {
            mOut.println("Error: " + sException.getMessage());
            if (mPrintCallStack)
            {
                mOut.println("[Call Stack]");
                sException.printStackTrace(mOut);
            }
        }
        catch (Exception sException)
        {
        }
        finally
        {
            closeAll();
        }
        sum();
        mEnd = true;
    }
    
    public static Properties parseConfig(String aFileName) throws Exception
    {
        Properties sProp = new Properties();
        BufferedReader sIn = new BufferedReader(new FileReader(aFileName));
        String sLine = sIn.readLine();
        while (sLine != null)
        {
            if (sLine.indexOf('#') >= 0)
            {
                sLine = sLine.substring(0, sLine.indexOf('#')); 
            }
            sLine = sLine.trim();
            if (sLine.length() > 0)
            {
                int sIndex = sLine.indexOf('=');
                if (sIndex == -1)
                {
                    sIn.close();
                    throw new Exception("Invalid property string: " + sLine);
                }
                String sKey = sLine.substring(0, sIndex).trim().toUpperCase();
                String sValue = sLine.substring(sIndex + 1, sLine.length()).trim();
                if (sKey.startsWith(KEY_PARTITION_RANGE))
                {
                    sKey = sKey.substring(KEY_PARTITION_RANGE.length(), sKey.length());
                    Properties sPartition = (Properties)sProp.get(KEY_PARTITION_RANGE);
                    if (sPartition == null)
                    {
                        sPartition = new Properties();
                        sProp.put(KEY_PARTITION_RANGE, sPartition);
                    }
                    sPartition.put(sKey, sValue);
                }
                else
                {
                    sProp.put(sKey, sValue);
                }
            }
            sLine = sIn.readLine();
        }
        sIn.close();
        return sProp;
    }
    
    public static String[] toArray(String aCommaSeparated)
    {
        StringTokenizer sTokenizer = new StringTokenizer(aCommaSeparated, ",");
        ArrayList<String> sList = new ArrayList<String>();
        while (sTokenizer.hasMoreTokens())
        {
            sList.add(sTokenizer.nextToken().trim());
        }
        return sList.toArray(new String[0]);
    }
    
    /*
     * For debugging
     */
    public static void mainForDebug(String[] args) throws Exception
    {
        TableDiff sDiff = new TableDiff("jdbc:goldilocks://127.0.0.1:22581/test",
                    "TEST",
                    "test",
                    "PUBLIC",
                    "T1",
                    "jdbc:goldilocks://127.0.0.1:22581/test",
                    "TEST",
                    "test",
                    "PUBLIC",
                    "T2",
                    "DIFF",
                    new String[0],//new String[]{"B"},
                    "",
                    "",
                    true,
                    true,
                    true,
                    false,
                    "tablesync.log",
                    true,
                    "/home/egon/workspace/product/regress/Gliese/tablediff/tablediff.log",
                    true,
                    "/home/egon/workspace/product/regress/Gliese/tablediff/tablediff.bin",
                    100,
                    1,
                    true);
        sDiff.start();
    }
    
    public static void modifyPortNumber(Properties aProp, String aNewPortNumber)
    {
        String sUrl = aProp.getProperty(KEY_SOURCE_URL);
        if (sUrl != null)
        {
            String sNewUrl = sUrl.substring(0, sUrl.lastIndexOf(":") + 1) + aNewPortNumber + sUrl.substring(sUrl.lastIndexOf("/"), sUrl.length());
            aProp.put(KEY_SOURCE_URL, sNewUrl);
        }
        sUrl = aProp.getProperty(KEY_TARGET_URL);
        if (sUrl != null)
        {
            String sNewUrl = sUrl.substring(0, sUrl.lastIndexOf(":") + 1) + aNewPortNumber + sUrl.substring(sUrl.lastIndexOf("/"), sUrl.length());
            aProp.put(KEY_TARGET_URL, sNewUrl);
        }
    }

    private void sum()
    {
        if (mOpSync)
        {
            for (int i=0; i<mSync.length; i++)
            {
                mInsertSuccessCount += mSync[i].getTargetInsertedCount();
                mInsertFailureCount += mSync[i].getTargetInsertFailureCount();
                mUpdateSuccessCount += mSync[i].getTargetUpdatedCount();
                mUpdateFailureCount += mSync[i].getTargetUpdateFailureCount();
                mDeleteSuccessCount += mSync[i].getInsertedOrDeletedCount();
                mDeleteFailureCount += mSync[i].getInsertOrDeleteFailureCount();
                mTotalUpdatedRow += mSync[i].getTotalUpdatedRowCount();
            }
        }
    }
    
    public static void printReport(boolean aTableDiff, long aRowCount, long aElapsed, boolean aSourceInsert, long aRowDiff, long aKeyDiffSource, long aKeyDiffTarget, long aUpdateSuccess, long aUpdateFailure, long aInsertSuccess, long aInsertFailure, long aDeleteSuccess, long aDeleteFailure, long aTotalUpdated, boolean aTimePrint)
    {
        System.out.println("Total " + aRowCount + " rows processed");
        System.out.println("  > row diff            : " + aRowDiff + ", update target(success/failure): " + aUpdateSuccess + "/" + aUpdateFailure);
        System.out.println("  > key diff source only: " + aKeyDiffSource + ", insert into target(success/failure): " + aInsertSuccess + "/" + aInsertFailure);
        if (aSourceInsert)
        {
            System.out.println("  > key diff target only: " + aKeyDiffTarget + ", insert into source(success/failure): " + aDeleteSuccess + "/" + aDeleteFailure);
        }
        else
        {
            System.out.println("  > key diff target only: " + aKeyDiffTarget + ", delete from target(success/failure): " + aDeleteSuccess + "/" + aDeleteFailure);
        }

        if (aTableDiff)
        {
            System.out.println("TableDiff completed");
        }
        else
        {
            System.out.println("TableSync completed: Total " + aTotalUpdated + " updated");
        }
        if (aTimePrint)
        {
            System.out.println("elapsed time = " + aElapsed/1000.0 + " sec");
        }
    }
    
    public static int getIntValue(String aValue, int aDefault)
    {
        if (aValue == null || aValue.length() == 0)
        {
            return aDefault;
        }
        int sValue = aDefault;
        try
        {
            sValue = Integer.parseInt(aValue);
        }
        catch (NumberFormatException sException)
        {            
        }
        return sValue;
    }
    
    public static boolean getBooleanValue(String aValue, boolean aDefault)
    {
        if (aValue == null)
        {
            return aDefault;
        }
        if (aValue.equalsIgnoreCase("ON"))
        {
            return true;
        }
        else if (aValue.equalsIgnoreCase("OFF"))
        {
            return false;
        }
        return aDefault;
    }
    
    public static void main(String[] args)
    {
        if (args.length < 1)
        {
            System.out.println("usage> java sunje.goldilocks.tool.diff.TableDiff [configure file]");
            System.exit(0);
        }
    
        Properties sProp = null;
        
        try
        {
            sProp = parseConfig(args[0]);
            /*
             * default properties
             */
            if (sProp.getProperty(KEY_EXCLUDED_COLUMNS) == null)
            {
                sProp.put(KEY_EXCLUDED_COLUMNS, "");
            }
        }
        catch (Exception sException)
        {
            System.out.println("Error at paring the configure file: " + sException.getMessage());
            System.exit(1);
        }
        
        try
        {
            if (sProp.getProperty(KEY_TARGET_INSERT) == null || sProp.getProperty(KEY_TARGET_INSERT).length() == 0)
            {
                throw new SQLException(KEY_TARGET_INSERT + " property is not defined");
            }
            if (sProp.getProperty(KEY_TARGET_UPDATE) == null || sProp.getProperty(KEY_TARGET_UPDATE).length() == 0)
            {
                throw new SQLException(KEY_TARGET_UPDATE + " property is not defined");
            }
            if (sProp.getProperty(KEY_TARGET_DELETE) == null || sProp.getProperty(KEY_TARGET_DELETE).length() == 0)
            {
                throw new SQLException(KEY_TARGET_DELETE + " property is not defined");
            }
            if (sProp.getProperty(KEY_SOURCE_INSERT) == null || sProp.getProperty(KEY_SOURCE_INSERT).length() == 0)
            {
                throw new SQLException(KEY_SOURCE_INSERT + " property is not defined");
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
                    modifyPortNumber(sProp, args[i+1]);
                    i++;
                }
                else if (args[i].equalsIgnoreCase("-n"))
                {
                    mTimePrint = false;
                }
            }
        }
        
        Properties sPartitions = (Properties)sProp.get(KEY_PARTITION_RANGE);
        if (sPartitions == null)
        {
            sPartitions = new Properties();
            sPartitions.put("", "");
        }
        
        String sDiffOut = sProp.getProperty(KEY_DIFF_OUT_FILE);
        String sSyncOut = sProp.getProperty(KEY_SYNC_OUT_FILE);
        String sSyncBin = sProp.getProperty(KEY_DIFF_BIN_FILE);
        if (sDiffOut == null)
        {
            sDiffOut = "tablediff.log";
        }
        if (sSyncOut == null)
        {
            sSyncOut = "tablesync.log";
        }
        if (sSyncBin == null)
        {
            sSyncBin = "tablediff.bin";
        }
            
        TableDiff[] sDiff = new TableDiff[sPartitions.size()];
        int i = 0;
        long sBefore = System.currentTimeMillis();
        try
        {
            for (Enumeration<Object> sEnum = sPartitions.keys(); sEnum.hasMoreElements(); i++)
            {
                String sPartitionName = (String)sEnum.nextElement();
                String sPartitionRange = (String)sPartitions.getProperty(sPartitionName);
 
                sDiff[i] = new TableDiff(sProp.getProperty(KEY_SOURCE_URL),
                                         sProp.getProperty(KEY_SOURCE_USER),
                                         sProp.getProperty(KEY_SOURCE_PASSWORD),
                                         sProp.getProperty(KEY_SOURCE_SCHEMA),
                                         sProp.getProperty(KEY_SOURCE_TABLE),
                                         sProp.getProperty(KEY_TARGET_URL),
                                         sProp.getProperty(KEY_TARGET_USER),
                                         sProp.getProperty(KEY_TARGET_PASSWORD),
                                         sProp.getProperty(KEY_TARGET_SCHEMA),
                                         sProp.getProperty(KEY_TARGET_TABLE),
                                         sProp.getProperty(KEY_OPERATION),
                                         toArray(sProp.getProperty(KEY_EXCLUDED_COLUMNS)),
                                         sProp.getProperty(KEY_WHERE_CLAUSE),
                                         sPartitionRange,
                                         sProp.getProperty(KEY_TARGET_INSERT).equalsIgnoreCase("ON"),
                                         sProp.getProperty(KEY_TARGET_UPDATE).equalsIgnoreCase("ON"),
                                         sProp.getProperty(KEY_TARGET_DELETE).equalsIgnoreCase("ON"),
                                         sProp.getProperty(KEY_SOURCE_INSERT).equalsIgnoreCase("ON"),
                                         sSyncOut + sPartitionName,
                                         getBooleanValue(sProp.getProperty(KEY_LOGGING_ON_SUCCESS), false),
                                         sDiffOut + sPartitionName,
                                         getBooleanValue(sProp.getProperty(KEY_LOGGING_ON_DIFF), false),
                                         sSyncBin + sPartitionName,
                                         getIntValue(sProp.getProperty(KEY_JOB_QUEUE_SIZE), 100),
                                         getIntValue(sProp.getProperty(KEY_JOB_THREAD), 1),
                                         getBooleanValue(sProp.getProperty(KEY_DISPLAY_CALL_STACK), false));
            }
        }
        catch (SQLException sException)
        {
            System.out.println("Error: " + sException.getMessage());
            if (getBooleanValue(sProp.getProperty(KEY_DISPLAY_CALL_STACK), false))
            {
                System.out.println("[Call Stack]");
                sException.printStackTrace();
            }
            for (int j=0; j<i; j++)
            {
                sDiff[j].closeAll();
            }
            System.exit(1);
        }

        try
        {
            int sDisplayUnit = getIntValue(sProp.getProperty(KEY_DISPLAY_ROW_UNIT), 100000);
    
            for (i=0; i<sDiff.length; i++)
            {
                sDiff[i].start();
            }
            
            long sRowCount = 0;
            long sDisplayRow = sDisplayUnit;
            while (true)
            {
                try
                {
                    Thread.sleep(10);
                }
                catch (InterruptedException sException)
                {
                    
                }
                boolean sEnd = true;
                sRowCount = 0;
                for (i=0; i<sDiff.length; i++)
                {
                    sRowCount += sDiff[i].mRowCount;
                    if (!sDiff[i].mEnd)
                    {
                        sEnd = false;
                    }
                }
                if (sRowCount > sDisplayRow)
                {
                    System.out.println(sRowCount/sDisplayRow*sDisplayRow + " rows are processed");
                    sDisplayRow += sDisplayUnit;
                }
                if (sEnd)
                {
                    break;
                }
            }
            
            sRowCount = 0; /* row count를 다시 구해야 한다. end 직전의 값은 정확한 값이 아닐 수 있다. */
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
            for (i=0; i<sDiff.length; i++)
            {
                sRowCount += sDiff[i].mRowCount;
                sRowDiff += sDiff[i].mRowDiff;
                sKeyDiffSourceOnly += sDiff[i].mKeyDiffSourceOnly;
                sKeyDiffTargetOnly += sDiff[i].mKeyDiffTargetOnly;
                sInsertSuccess += sDiff[i].mInsertSuccessCount;
                sInsertFailure += sDiff[i].mInsertFailureCount;
                sUpdateSuccess += sDiff[i].mUpdateSuccessCount;
                sUpdateFailure += sDiff[i].mUpdateFailureCount;
                sDeleteSuccess += sDiff[i].mDeleteSuccessCount;
                sDeleteFailure += sDiff[i].mDeleteFailureCount;
                sTotalUpdated += sDiff[i].mTotalUpdatedRow;
            }
            long sAfter = System.currentTimeMillis();
            
            printReport(true, sRowCount, sAfter - sBefore, sDiff[0].mSourceModifiable, sRowDiff, sKeyDiffSourceOnly, sKeyDiffTargetOnly, sUpdateSuccess, sUpdateFailure, sInsertSuccess, sInsertFailure, sDeleteSuccess, sDeleteFailure, sTotalUpdated, mTimePrint);
        }
        catch (Exception sException)
        {
            for (i=0; i<sDiff.length; i++)
            {
                if (sDiff[i] != null)
                {
                    sDiff[i].closeAll();
                }
            }
        }
    }
}
