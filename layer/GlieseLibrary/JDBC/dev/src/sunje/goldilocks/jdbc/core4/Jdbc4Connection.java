package sunje.goldilocks.jdbc.core4;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.sql.CallableStatement;
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.SQLWarning;
import java.sql.Savepoint;
import java.sql.Statement;
import java.sql.Struct;
import java.text.SimpleDateFormat;
import java.util.LinkedList;
import java.util.Map;
import java.util.Properties;

import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.cm.SourceConnAttr;
import sunje.goldilocks.jdbc.cm.SourceConnect;
import sunje.goldilocks.jdbc.cm.SourceTransaction;
import sunje.goldilocks.jdbc.env.SessionEnv;
import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.internal.BaseProtocolSource;
import sunje.goldilocks.jdbc.internal.ErrorHolder;
import sunje.goldilocks.jdbc.util.LogWriterHost;
import sunje.goldilocks.jdbc.util.Logger;
import sunje.goldilocks.jdbc.util.Utils;

class ConnectionProperty
{
    public String mName;
    public boolean mRequired;
    public boolean mIsNumber;
    public String[] mChoices;
    public String mDescription;
    
    public ConnectionProperty(String aName, boolean aRequired, boolean aIsNumber, String[] aChoices, String aDescription)
    {
        mName = aName;
        mRequired = aRequired;
        mIsNumber = aIsNumber;
        mChoices = aChoices;
        mDescription = aDescription;
    }
}

public abstract class Jdbc4Connection extends ErrorHolder implements Connection, SessionEnv, LogWriterHost
{
    public static final String GOLDILOCKS_GLOBAL_PROP_NAME_LOGGER = "global_logger";
    public static final String GOLDILOCKS_PROP_NAME_USER = "user";
    public static final String GOLDILOCKS_PROP_NAME_PASSWORD = "password";
    public static final String GOLDILOCKS_PROP_NAME_PROGRAM = "program";
    public static final String GOLDILOCKS_PROP_NAME_ROLE = "role";
    public static final String GOLDILOCKS_PROP_NAME_SESSION_TYPE = "session_type";
    public static final String GOLDILOCKS_PROP_NAME_MAX_TRAILING_ZERO_COUNT_FOR_PLACEHOLDERS = "tzeros";
    public static final String GOLDILOCKS_PROP_NAME_MAX_LEADING_ZERO_COUNT_FOR_PLACEHOLDERS = "lzeros";
    public static final String GOLDILOCKS_PROP_NAME_DECODING_REPLACEMENT = "decoding_replacement";
    public static final String GOLDILOCKS_PROP_NAME_TRACE_LOG = "trace_log";
    public static final String GOLDILOCKS_PROP_NAME_QUERY_LOG = "query_log";
    public static final String GOLDILOCKS_PROP_NAME_PROTOCOL_LOG = "protocol_log";
    public static final String GOLDILOCKS_PROP_NAME_CONTINUOUS_BATCH_COUNT = "batch_count";
    public static final String GOLDILOCKS_PROP_NAME_KEEP_ALIVE = "keep_alive";
    public static final String GOLDILOCKS_PROP_NAME_DATE_FORMAT = "date_format";
    public static final String GOLDILOCKS_PROP_NAME_TIME_FORMAT = "time_format";
    public static final String GOLDILOCKS_PROP_NAME_TIMESTAMP_FORMAT = "timestamp_format";
    public static final String GOLDILOCKS_PROP_NAME_TIME_TIMEZONE_FORMAT = "timetz_format";
    public static final String GOLDILOCKS_PROP_NAME_TIMESTAMP_TIMEZONE_FORMAT = "timestamptz_format";
    
    public static final String GOLDILOCKS_GLOBAL_PROP_VALUE_LOGGER_CONSOLE = "console";
    public static final String GOLDILOCKS_PROP_VALUE_ROLE_DEFAULT = "";
    public static final String GOLDILOCKS_PROP_VALUE_ROLE_SYSDBA = "SYSDBA";
    public static final String GOLDILOCKS_PROP_VALUE_ROLE_ADMIN = "ADMIN";
    public static final String GOLDILOCKS_PROP_VALUE_PROGRAM_UNNAMED = "Unnamed java program";
    public static final String GOLDILOCKS_PROP_VALUE_SESSION_TYPE_DEDICATE = String.valueOf(Protocol.CONNECT_SESSION_DEDICATE);
    public static final String GOLDILOCKS_PROP_VALUE_SESSION_TYPE_SHARED = String.valueOf(Protocol.CONNECT_SESSION_SHARED);
    public static final String GOLDILOCKS_PROP_VALUE_LOG_ON = "on";
    public static final String GOLDILOCKS_PROP_VALUE_KEEP_ALIVE = "on";
    
    public static final int GOLDILOCKS_DEFAULT_MAX_TRAILING_ZERO_COUNT = 15;
    public static final int GOLDILOCKS_DEFAULT_MAX_LEADING_ZERO_COUNT = 15;
    public static final String GOLDILOCKS_DEFAULT_DECODING_REPLACEMENT = "?";
    public static final SimpleDateFormat GOLDILOCKS_DEFAULT_DATE_FORMAT = new SimpleDateFormat("yyyy-MM-dd");
    public static final SimpleDateFormat GOLDILOCKS_DEFAULT_TIME_FORMAT = new SimpleDateFormat("HH:mm:ss");
    public static final SimpleDateFormat GOLDILOCKS_DEFAULT_TIMESTAMP_FORMAT = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS");
    public static final SimpleDateFormat GOLDILOCKS_DEFAULT_TIMETZ_FORMAT = new SimpleDateFormat("HH:mm:ss Z");
    public static final SimpleDateFormat GOLDILOCKS_DEFAULT_TIMESTAMPTZ_FORMAT = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS Z");
    
    public static final int CONNECTION_ATTRIBUTE_ACCESS_MODE = 0;
    public static final int CONNECTION_ATTRIBUTE_TX_ISOLATION = 1;
    public static final int CONNECTION_ATTRIBUTE_TIMEZONE = 2;
    
    public static final int GOLDILOCKS_DEFAULT_RESULT_SET_TYPE = ResultSet.TYPE_FORWARD_ONLY;
    public static final int GOLDILOCKS_DEFAULT_RESULT_SET_CONCURRENCY = ResultSet.CONCUR_READ_ONLY;
    public static final int GOLDILOCKS_DEFAULT_RESULT_SET_HOLDABILITY = ResultSet.HOLD_CURSORS_OVER_COMMIT;
    
    /*
     * 이 상수값은 JDBC에서 batch execution시 bind-execute 반복횟수의 최대값을 설정한다.
     * 이 값만큼 프로토콜을 실은 후 period를 한번 찍고 전송한 후, 다시 남은 batch job을 수행한다.
     * 이렇게 하는 이유는, 무한대로 서버로 전송하게 되면 서버는 execute 처리 후 결과를 클라이언트에게
     * 보내게 되는데, 서로 프로토콜을 보내는 상황에서 dead-lock이 발생할 수 있기 때문이다.
     * 물론 서버에는 result packet을 바로 보내지 않고 어느정도 풀링을 하기 때문에
     * dead-lock을 최대한 피하려고 하지만, 풀링이 무한대로 진행될 수는 없기 때문에
     * 이 상수를 두는 것이다. 이 값을 크게 하면 batch execute 성능이 더 좋아지긴 하지만,
     * 서버가 그만큼 결과를 풀링해야 하는 메모리 비용이 발생하게 된다.
     */
    public static int DEFAULT_CONTINUOUS_BATCH_COUNT = 1000;

    protected static final String INTERNAL_SQL_SAVEPOINT = "SAVEPOINT ";
    protected static final String INTERNAL_SQL_ROLLBACK_TO = "ROLLBACK TO SAVEPOINT ";
    protected static final String INTERNAL_SQL_RELEASE_SAVEPOINT = "RELEASE SAVEPOINT ";
    protected static final String INTERNAL_SQL_CURRENT_CATALOG = "SELECT CURRENT_CATALOG FROM DUAL";
    protected static final String INTERNAL_SQL_HEART_BEAT_CHECK = "SELECT * FROM DUAL";
    
    protected static final int TX_ISOLATION_READ_UNCOMMITTED = 0;
    protected static final int TX_ISOLATION_READ_COMMITTED = 1;
    protected static final int TX_ISOLATION_SERIALIZABLE = 2;
    protected static final int TX_ISOLATION_REPEATABLE_READ = 3;
    protected static final int TX_ISOLATION_NONE = 4;
    
    protected static int gProcessId = -1;
    protected static boolean gGlobalPropLoggerSet = false;
    
    /*
     * charset 이름을 정의. 순서는 Protocol.CHARSET_ID_XXX의 아이디 순임.
     */
    protected static final String[] CHARSET_NAMES = { "US-ASCII", "UTF-8", "x-windows-949", "GB18030" };
    
    static void registerProperties()
    {
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_GLOBAL_PROP_NAME_LOGGER, false, false, new String[]{GOLDILOCKS_GLOBAL_PROP_VALUE_LOGGER_CONSOLE}, "it defines the global log writer."));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_USER, true, false, null, "connection user name"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_PASSWORD, true, false, null, "connection password"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_PROGRAM, false, false, null, "program name"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_ROLE, false, false, new String[]{"\"\"", GOLDILOCKS_PROP_VALUE_ROLE_SYSDBA, GOLDILOCKS_PROP_VALUE_ROLE_ADMIN}, "connection role, ADMIN or SYSDBA"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_SESSION_TYPE, false, true, new String[]{GOLDILOCKS_PROP_VALUE_SESSION_TYPE_DEDICATE, GOLDILOCKS_PROP_VALUE_SESSION_TYPE_SHARED}, "session type, dedicate or shared"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_MAX_TRAILING_ZERO_COUNT_FOR_PLACEHOLDERS, false, true, null, "trailing zero count for numeric placeholders"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_MAX_LEADING_ZERO_COUNT_FOR_PLACEHOLDERS, false, true, null, "leading zero count for numeric placeholders"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_DECODING_REPLACEMENT, false, false, null, "decoder's replacement value"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_TRACE_LOG, false, false, new String[]{GOLDILOCKS_PROP_VALUE_LOG_ON}, "turns on the trace logging"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_QUERY_LOG, false, false, new String[]{GOLDILOCKS_PROP_VALUE_LOG_ON}, "turns on the query logging"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_PROTOCOL_LOG, false, false, new String[]{GOLDILOCKS_PROP_VALUE_LOG_ON}, "turns on the protocol logging"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_CONTINUOUS_BATCH_COUNT, false, true, null, "maximum continuous batch count"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_KEEP_ALIVE, false, false, new String[]{GOLDILOCKS_PROP_VALUE_KEEP_ALIVE}, "turns on the connection property 'keep alive'"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_DATE_FORMAT, false, false, null, "date format which is used at getString and setString for a 'date' column"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_TIME_FORMAT, false, false, null, "time format which is used at getString and setString for a 'time' column"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_TIMESTAMP_FORMAT, false, false, null, "timestamp format which is used at getString and setString for a 'timestamp' column"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_TIME_TIMEZONE_FORMAT, false, false, null, "time format which is used at getString and setString for a 'time with timezone' column"));
        Jdbc4Driver.registerProperty(new ConnectionProperty(GOLDILOCKS_PROP_NAME_TIMESTAMP_TIMEZONE_FORMAT, false, false, null, "timestamp format which is used at getString and setString for a 'timestamp with timezone' column"));
    }
    
    static int isolationLevelGoldilocksToJdbc(int aGoldilocksValue)
    {
        if (aGoldilocksValue == TX_ISOLATION_READ_UNCOMMITTED)
        {
            return Connection.TRANSACTION_READ_UNCOMMITTED;
        }
        else if (aGoldilocksValue == TX_ISOLATION_READ_COMMITTED)
        {
            return Connection.TRANSACTION_READ_COMMITTED;
        }
        else if (aGoldilocksValue == TX_ISOLATION_SERIALIZABLE)
        {
            return Connection.TRANSACTION_SERIALIZABLE;
        }
        else if (aGoldilocksValue == TX_ISOLATION_REPEATABLE_READ)
        {
            return Connection.TRANSACTION_REPEATABLE_READ;
        }
        else if (aGoldilocksValue == TX_ISOLATION_NONE)
        {
            return Connection.TRANSACTION_NONE;
        }
        else
        {
            ErrorMgr.raiseRuntimeError("Invalid connection attribute value");
            return Connection.TRANSACTION_NONE;
        }
    }
    
    static int isolationLevelJdbcToGoldilocks(int aJdbcValue)
    {
        if (aJdbcValue == Connection.TRANSACTION_READ_UNCOMMITTED)
        {
            return TX_ISOLATION_READ_UNCOMMITTED;
        }
        else if (aJdbcValue == Connection.TRANSACTION_READ_COMMITTED)
        {
            return TX_ISOLATION_READ_COMMITTED;
        }
        else if (aJdbcValue == Connection.TRANSACTION_SERIALIZABLE)
        {
            return TX_ISOLATION_SERIALIZABLE;
        }
        else if (aJdbcValue == Connection.TRANSACTION_REPEATABLE_READ)
        {
            return TX_ISOLATION_REPEATABLE_READ;
        }
        return TX_ISOLATION_NONE;
    }

    protected class ConnAttrAccessMode extends BaseProtocolSource implements SourceConnAttr
    {
        private static final int UPDATABLE = 1;
        private static final int READ_ONLY = 2;
        
        private boolean mAccessModeUpdatable = true;
        
        ConnAttrAccessMode(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public int getAttributeType()
        {
            return Jdbc4Connection.CONNECTION_ATTRIBUTE_ACCESS_MODE;
        }

        public void setAttributeValue(int aValue) throws SQLException
        {
            if (aValue != UPDATABLE && aValue != READ_ONLY)
            {
                ErrorMgr.raiseRuntimeError("Invalid attribute value from server");
            }
            mAccessModeUpdatable = (aValue == UPDATABLE);        
        }

        public void setAttributeValue(String aValue) throws SQLException
        {
            // nothing to do
            ErrorMgr.raiseRuntimeError(ErrorMgr.ETC_ERROR_INVALID_METHOD_CALL);
        }
        
        public byte getAttributeValueType()
        {
            return Protocol.ATTR_DATA_TYPE_INT;
        }
        
        public int getAttributeIntValue() throws SQLException
        {
            return mAccessModeUpdatable ? UPDATABLE : READ_ONLY;
        }
        
        public String getAttributeStringValue() throws SQLException
        {
            ErrorMgr.raiseRuntimeError(ErrorMgr.ETC_ERROR_INVALID_METHOD_CALL);
            return null;
        }

        boolean isUpdatable()
        {
            return mAccessModeUpdatable;
        }
        
        void setUpdatable(boolean aUpdatable)
        {
            mAccessModeUpdatable = aUpdatable;
        }
    }

    protected class ConnAttrTxIsolation extends BaseProtocolSource implements SourceConnAttr
    {
        private int mIsolation;

        ConnAttrTxIsolation(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public int getAttributeType()
        {
            return Jdbc4Connection.CONNECTION_ATTRIBUTE_TX_ISOLATION;
        }

        public void setAttributeValue(int aValue) throws SQLException
        {
            mIsolation = Jdbc4Connection.isolationLevelGoldilocksToJdbc(aValue);
        }

        public void setAttributeValue(String aValue) throws SQLException
        {
            // nothing to do
            ErrorMgr.raiseRuntimeError(ErrorMgr.ETC_ERROR_INVALID_METHOD_CALL);
        }

        public byte getAttributeValueType()
        {
            return Protocol.ATTR_DATA_TYPE_INT;
        }

        public int getAttributeIntValue() throws SQLException
        {
            return Jdbc4Connection.isolationLevelJdbcToGoldilocks(mIsolation);
        }

        public String getAttributeStringValue() throws SQLException
        {
            ErrorMgr.raiseRuntimeError(ErrorMgr.ETC_ERROR_INVALID_METHOD_CALL);
            return null;
        }
        
        int getIsolationLevel()
        {
            return mIsolation;
        }
        
        void setIsolationLevel(int aIsolationLevel)
        {
            mIsolation = aIsolationLevel;
        }
    }

    protected class CommitOp extends BaseProtocolSource implements SourceTransaction
    {
        CommitOp(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public short getOperation()
        {
            return Protocol.TRANSACTION_COMMIT;
        }

        public void notifyDone()
        {
            synchronized (mStatements)
            {
                for (Jdbc4Statement sStmt : mStatements)
                {
                    sStmt.notifyCommitted();
                }
            }
            mTransActive = false;
        }    
    }

    protected class RollbackOp extends BaseProtocolSource implements SourceTransaction
    {
        RollbackOp(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public short getOperation()
        {
            return Protocol.TRANSACTION_ROLLBACK;
        }

        public void notifyDone()
        {
            synchronized (mStatements)
            {
                for (Jdbc4Statement sStmt : mStatements)
                {
                    sStmt.notifyRolledBack();
                }
            }
            mTransActive = false;
        }
    }

    protected class ConnectOp extends BaseProtocolSource implements SourceConnect
    {
        private String mDefaultProgramName = (String)System.getProperty("sun.java.command", GOLDILOCKS_PROP_VALUE_PROGRAM_UNNAMED);
        protected int  mSessionId;
        protected long mSessionSequence;

        ConnectOp(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        public String getUserName()
        {
            return mProp.getProperty(GOLDILOCKS_PROP_NAME_USER);
        }

        public String getPassword() throws SQLException
        {
            return toSHA1(mProp.getProperty(GOLDILOCKS_PROP_NAME_PASSWORD));
        }

        public String getOldPassword()
        {
            return "";
        }

        public String getNewPassword()
        {
            return "";
        }

        public String getProgramName()
        {
            return mProp.getProperty(GOLDILOCKS_PROP_NAME_PROGRAM, mDefaultProgramName);
        }

        public String getRoleName()
        {
            return mProp.getProperty(GOLDILOCKS_PROP_NAME_ROLE, GOLDILOCKS_PROP_VALUE_ROLE_DEFAULT);
        }

        public int getProcessId()
        {
            if (gProcessId == -1)
            {
                synchronized (VersionSpecific.SINGLETON)
                {
                    /*
                     * 락을 잡고 한번더 검사해야 한다.
                     */
                    if (gProcessId == -1)
                    {
                        gProcessId = VersionSpecific.SINGLETON.getProcessId();
                    }
                }
            }
            return gProcessId;
        }

        public int getClientTimeZone()
        {
            return Utils.getLocalCalendar().getTimeZone().getRawOffset() / 1000;
        }
        
        public void setSessionId(int aSessionId)
        {
            mSessionId = aSessionId;
        }

        public void setSessionSequence(long aSequence)
        {
            mSessionSequence = aSequence;
        }
        
        public void setCharset(short aCharset) throws SQLException
        {
            if (aCharset >= Protocol.CHARSET_ID_MAX)
            {
                ErrorMgr.raise(ErrorMgr.INVALID_PROTOCOL, "Invalid charset id: " + aCharset);
            }
            mCharset = Charset.forName(CHARSET_NAMES[aCharset]);
        }
        
        public void setNCharset(short aNCharset) throws SQLException
        {
            /*
             * 아직 NCHAR를 지원하지 않으므로 무시한다.
             */
        }
        
        public void setTimeZone(int aTimeZone)
        {
            /* 서버로부터 받은 session timezone은 사용할 일이 없다. */
        }
    }
    
    protected String                      mHost;
    protected int                         mPort;
    protected String                      mDBName;
    protected Properties                  mProp;
    protected boolean                     mClosed = true;
    protected CmChannel                   mChannel = null;
    protected int                         mResultSetHoldability = GOLDILOCKS_DEFAULT_RESULT_SET_HOLDABILITY;
    protected boolean                     mAutoCommit = true;
    protected boolean                     mTransActive = false;
    protected Charset                     mCharset;
    protected Jdbc4CommonDataSource       mOrigin;
    protected int                         mUnnamedSavepointId = 0;
    protected String                      mDecodingReplacement;
    protected LinkedList<Jdbc4Statement>  mStatements;
    protected ConnAttrAccessMode          mAccessModeAttr;
    protected ConnAttrTxIsolation         mTxIsolationAttr;
    protected Jdbc4Statement              mInternalStmt;
    protected Jdbc4DatabaseMetaData       mMetaData;
    protected CommitOp                    mCommitOp;
    protected RollbackOp                  mRollbackOp;
    protected ConnectOp                   mConnectOp;
    protected Logger                      mLogger;
    protected int                         mContinuousBatchCount;
    protected Connection                  mHandleForClient = this;
    protected byte                        mSessionType;
    protected SimpleDateFormat            mDateFormat;
    protected SimpleDateFormat            mTimeFormat;
    protected SimpleDateFormat            mTimestampFormat;
    protected SimpleDateFormat            mTimeTZFormat;
    protected SimpleDateFormat            mTimestampTZFormat;
    
    /*
     * connection socket 속성으로 keep alive를 설정할 수 있다.
     * 이 속성을 키면, TCP layer에서 자체적으로 ack를 주고 받아, connection을 주기적으로 확인한다.
     * 만약 랜선을 뽑게 되면 이 속성에 의해 socket이 끊어지고, 따라서 Connection도 끊기게 된다.
     * 이 속성과 관련된 시스템 환경 변수는 다음과 같다.
     * sysctl -w net.ipv4.tcp_keepalive_intvl=5  (default=75, 처음 이 시간만큼 지난 후 ack를 주고 받음. 단위는 초)
     * sysctl -w net.ipv4.tcp_keepalive_time=5   (default=7200, 이 시간 주기만큼 ack를 주고 받음. 단위는 초)
     * sysctl -w net.ipv4.tcp_keepalive_probes=1 (default=9, 이 횟수만큼 응답이 없을 경우 소켓을 끊음. 단위는 횟수) 
     */
    protected boolean                     mKeepAlive = false;
    
    private String toSHA1(String aStr) throws SQLException
    {
        MessageDigest sMessageDigest = null;
        byte[]        sEncrypted = null;
        
        try
        {
            sMessageDigest = MessageDigest.getInstance("SHA-1");
        }
        catch (NoSuchAlgorithmException sException)
        {
            ErrorMgr.raiseRuntimeError(sException.getMessage());
        }
        
        sEncrypted = sMessageDigest.digest(aStr.getBytes());
        
        return VersionSpecific.SINGLETON.byteToHexString(sEncrypted, false);
    }
    
    public Jdbc4Connection(String aHost, int aPort, String aDBName, Properties aProp, Jdbc4CommonDataSource aFrom) throws SQLException
    {
        mHost = aHost;
        mPort = aPort;
        mDBName = aDBName;
        mProp = aProp;
        mOrigin = aFrom;
        mStatements = new LinkedList<Jdbc4Statement>();
        
        mAccessModeAttr = new ConnAttrAccessMode(this);
        mTxIsolationAttr = new ConnAttrTxIsolation(this);

        mCommitOp = new CommitOp(this);
        mRollbackOp = new RollbackOp(this);
        mConnectOp = new ConnectOp(this);
        
        checkGlobalProperties();
        checkRequiredProperties();
        checkOptionalProperties();
        
        open();
        listenerHandshake(); 
        handshake();
        connect();
    }

    public CmChannel getChannel()
    {
        return mChannel;
    }
    
    Connection getHandleForClient()
    {
        return mHandleForClient;
    }
    
    void setHandleForClient(Connection aCon)
    {
        /*
         * logical connection이 세팅되는 경우가 있다.
         * 사용자가 PooledConnection으로부터 Connection 객체를 얻어가면
         * LogicalConnection을 사용하게 되는데,
         * 이 LogicalConnection으로부터 Statement를 생성한 후
         * Statement.getConnection()하면 PhysicalConnection이 아니라
         * LogicalConnection을 얻어야 하므로 이 메소드를 통해서
         * LogicalConnection을 세팅해 놓고 getHandleForClient()를 통해
         * LogicalConnection을 얻도록 한다.
         */
        mHandleForClient = aCon;
    }
    
    protected void verifyInternalStmt() throws SQLException
    {
        if (mInternalStmt == null)
        {
            mInternalStmt = (Jdbc4Statement)createStatement();
            mInternalStmt.setForInternal();
        }
    }
    
    public Logger getLogger()
    {
        return mLogger;
    }
    
    public synchronized String getValueByQuery(String aSql) throws SQLException
    {
        verifyInternalStmt();
        ResultSet sRs = mInternalStmt.executeQuery(aSql);
        String sValue = "";
        if (sRs.next())
        {
            sValue = sRs.getString(1);
        }
        sRs.close();
        return sValue;
    }
    
    public synchronized ResultSet executeInternalQuery(String aSql) throws SQLException
    {
        verifyInternalStmt();
        return mInternalStmt.executeQuery(aSql);
    }
    
    public synchronized void executeInternalSql(String aSql) throws SQLException
    {
        verifyInternalStmt();
        mInternalStmt.execute(aSql);
    }
    
    protected void checkGlobalProperties() throws SQLException
    {
        if (!gGlobalPropLoggerSet)
        {
            String sValue = mProp.getProperty(GOLDILOCKS_GLOBAL_PROP_NAME_LOGGER);
            
            if (sValue != null)
            {
                // 현재는 console만 지원한다.
                // 생각해볼 수 있는 값으로 파일이 있겠다.
                if (GOLDILOCKS_GLOBAL_PROP_VALUE_LOGGER_CONSOLE.equalsIgnoreCase(sValue))
                {
                    if (mOrigin == null)
                    {
                        if (DriverManager.getLogWriter() == null)
                        {
                            DriverManager.setLogWriter(new PrintWriter(System.out));
                        }
                    }
                    else
                    {
                        if (mOrigin.getLogWriter() == null)
                        {
                            mOrigin.setLogWriter(new PrintWriter(System.out));
                        }
                    }
                }
                
                // 전역 변수 참조하지만 동시성에 큰 문제가 되지 않는다.
                gGlobalPropLoggerSet = true;
            }
        }
    }
    
    protected void checkRequiredProperties() throws SQLException
    {
        if (mProp.getProperty(GOLDILOCKS_PROP_NAME_USER) == null)
        {
            ErrorMgr.raise(ErrorMgr.MANDATORY_PROPERTY_OMITTED, GOLDILOCKS_PROP_NAME_USER);
        }
        if (mProp.getProperty(GOLDILOCKS_PROP_NAME_PASSWORD) == null)
        {
            ErrorMgr.raise(ErrorMgr.MANDATORY_PROPERTY_OMITTED, GOLDILOCKS_PROP_NAME_PASSWORD);
        }
    }
    
    protected void checkOptionalProperties()
    {
        boolean sTraceLog = false;
        boolean sQueryLog = false;
        boolean sProtocolLog = false;
        
        if (GOLDILOCKS_PROP_VALUE_LOG_ON.equalsIgnoreCase(mProp.getProperty(GOLDILOCKS_PROP_NAME_TRACE_LOG)))
        {
            sTraceLog = true;
        }
        if (GOLDILOCKS_PROP_VALUE_LOG_ON.equalsIgnoreCase(mProp.getProperty(GOLDILOCKS_PROP_NAME_QUERY_LOG)))
        {
            sQueryLog = true;
        }
        if (GOLDILOCKS_PROP_VALUE_LOG_ON.equalsIgnoreCase(mProp.getProperty(GOLDILOCKS_PROP_NAME_PROTOCOL_LOG)))
        {
            sProtocolLog = true;
        }
        if (GOLDILOCKS_PROP_VALUE_KEEP_ALIVE.equalsIgnoreCase(mProp.getProperty(GOLDILOCKS_PROP_NAME_KEEP_ALIVE)))
        {
            mKeepAlive = true;
        }
        
        mDateFormat = getDateFormat(GOLDILOCKS_PROP_NAME_DATE_FORMAT, GOLDILOCKS_DEFAULT_DATE_FORMAT);
        mTimeFormat = getDateFormat(GOLDILOCKS_PROP_NAME_TIME_FORMAT, GOLDILOCKS_DEFAULT_TIME_FORMAT);
        mTimestampFormat = getDateFormat(GOLDILOCKS_PROP_NAME_TIMESTAMP_FORMAT, GOLDILOCKS_DEFAULT_TIMESTAMP_FORMAT);
        mTimeTZFormat = getDateFormat(GOLDILOCKS_PROP_NAME_TIME_TIMEZONE_FORMAT, GOLDILOCKS_DEFAULT_TIMETZ_FORMAT);
        mTimestampTZFormat = getDateFormat(GOLDILOCKS_PROP_NAME_TIMESTAMP_TIMEZONE_FORMAT, GOLDILOCKS_DEFAULT_TIMESTAMPTZ_FORMAT);
        
        mLogger = Logger.createLogger(this, sTraceLog, sQueryLog, sProtocolLog);
        mDecodingReplacement = mProp.getProperty(GOLDILOCKS_PROP_NAME_DECODING_REPLACEMENT, GOLDILOCKS_DEFAULT_DECODING_REPLACEMENT);
        mContinuousBatchCount = getIntProp(GOLDILOCKS_PROP_NAME_CONTINUOUS_BATCH_COUNT, DEFAULT_CONTINUOUS_BATCH_COUNT);
        mSessionType = (byte)getIntProp(GOLDILOCKS_PROP_NAME_SESSION_TYPE, Protocol.CONNECT_SESSION_DEFAULT);
    }

    private SimpleDateFormat getDateFormat(String aPropName, SimpleDateFormat aDefaultFormat)
    {
        if (mProp.getProperty(aPropName) == null)
        {
            return aDefaultFormat;
        }
        else
        {
            try
            {
                return new SimpleDateFormat(mProp.getProperty(aPropName));
            }
            catch (Exception sException)
            {
                mWarning = ErrorMgr.chainWarning(mWarning, ErrorMgr.IGNORED_INVALID_PROPERTY, aPropName, mProp.getProperty(aPropName));
            }
            return aDefaultFormat;
        }
    }
    
    protected void open() throws SQLException
    {
        int sTimeout = 0;
        try
        {
            SocketChannel sChannel = SocketChannel.open();
            sChannel.configureBlocking(true);
            Socket sSock = sChannel.socket();
            sSock.setTcpNoDelay(false);
            sSock.setKeepAlive(mKeepAlive);
            if (mOrigin == null)
            {
                sTimeout = DriverManager.getLoginTimeout();
            }
            else
            {
                sTimeout = mOrigin.getLoginTimeout();
            }
            sSock.connect(new InetSocketAddress(mHost, mPort), sTimeout);
            mChannel = new CmChannel(sChannel,
                                     mLogger,
                                     Protocol.HANDSHAKE_BUF_SIZE,
                                     true,  // isLittleEndian
                                     mSessionType,
                                     mConnectOp.getRoleName());
        }
        catch (SocketTimeoutException sException)
        {
            ErrorMgr.raise(ErrorMgr.CONNECT_TIMEOUT, String.valueOf(sTimeout));
        }
        catch (SocketException sException)
        {
            ErrorMgr.raise(ErrorMgr.OPEN_FAILURE, sException.getMessage());
        }
        catch (IOException sException)
        {
            ErrorMgr.raise(ErrorMgr.OPEN_FAILURE, sException.getMessage());
        }
    }

    protected void listenerHandshake() throws SQLException
    {
        synchronized (mChannel)
        {
            mChannel.sendVersion();
            mChannel.initToSend();
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_LISTENER_HANDSHAKE, mChannel);
            mChannel.sendAndReceive();
        }
        testError();
    }

    protected void handshake() throws SQLException
    {
        synchronized (mChannel)
        {
            mChannel.initToSend();
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_HANDSHAKE, mChannel);
            mChannel.sendAndReceive();
        }
        testError();
    }
    
    protected void connect() throws SQLException
    {
        synchronized (mChannel)
        {
            mChannel.initToSend();
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_CONNECT, mConnectOp);
            mChannel.sendAndReceive();
        }
        
        testError();
        mClosed = false;
    }
    
    protected void checkClosed() throws SQLException
    {
        if (mClosed)
        {
            ErrorMgr.raise(ErrorMgr.CONNECTION_CLOSED);
        }
    }
    
    protected void checkNonAutoCommit() throws SQLException
    {
        if (mAutoCommit)
        {
            ErrorMgr.raise(ErrorMgr.AUTO_COMMIT_ERROR);
        }
    }
    
    protected String getHost()
    {
        return mHost;
    }
    
    protected int getPort()
    {
        return mPort;
    }
    
    protected String getDBName()
    {
        return mDBName;
    }
    
    protected void removeStatement(Jdbc4Statement aStatement)
    {
        synchronized (mStatements)
        {
            mStatements.remove(aStatement);
        }
    }
    
    protected void addCommitProtocolOnAutoCommit(boolean aIsInternalExec) throws SQLException
    {
        // 이 메소드는 sync를 걸면 안된다.
        if (mAutoCommit)
        {
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_TRANSACTION, mCommitOp);
        }
        else
        {
            if (aIsInternalExec)
            {
                if (!mTransActive)
                {
                    // internal sql인데, 현재 active 중이 아니면 자체 커밋을 해야 한다.
                    mChannel.writeProtocolAlone(Protocol.PROTOCOL_TRANSACTION, mCommitOp);
                }
            }
            else
            {
                mTransActive = true;
            }
        }
    }

    protected Charset getCharset()
    {
        return mCharset;
    }
    
    protected abstract Jdbc4Statement createConcreteStatement(int aResultSetType, int aResultSetConcurrency, int aResultSetHoldability);
    
    protected abstract Jdbc4PreparedStatement createConcretePreparedStatement(String aSqlString, int aType, int aConcurrency, int aHoldability) throws SQLException;
    
    protected abstract Jdbc4DatabaseMetaData createConcreteDatabaseMetaData();
    
    protected abstract Jdbc4Savepoint createConcreteSavepoint(int aId);
    
    protected abstract Jdbc4Savepoint createConcreteSavepoint(String aName);
    
    /**************************************************************
     * Object Method 
     **************************************************************/
    public String toString()
    {
        String sClosed;
        if (mClosed)
        {
            sClosed = "closed";
        }
        else
        {
            sClosed = "connected";
        }
        
        return "GOLDILOCKS Connection:" + mHost + ":" + mPort + ":" + sClosed + ":" + mConnectOp.mSessionId + ":" + mConnectOp.mSessionSequence;
    }
    
    /**************************************************************
     * SessionEnv Interface Method 
     **************************************************************/
    private int getIntProp(String aPropName, int aDefaultVal)
    {
        Object sVal = mProp.get(aPropName);
        if (sVal == null)
        {
            return aDefaultVal;
        }
        
        if (sVal instanceof String)
        {
            try
            {
                return Integer.parseInt((String)sVal);
            }
            catch (NumberFormatException sException)
            {
                return aDefaultVal;
            }
        }
        else if (sVal instanceof Number)
        {
            return ((Number)sVal).intValue();
        }
        
        return aDefaultVal;
    }
    
    public int getMaxTrailingZeroCount()
    {
        return getIntProp(GOLDILOCKS_PROP_NAME_MAX_TRAILING_ZERO_COUNT_FOR_PLACEHOLDERS, GOLDILOCKS_DEFAULT_MAX_TRAILING_ZERO_COUNT);
    }
    
    public int getMaxLeadingZeroCount()
    {
        return getIntProp(GOLDILOCKS_PROP_NAME_MAX_LEADING_ZERO_COUNT_FOR_PLACEHOLDERS, GOLDILOCKS_DEFAULT_MAX_LEADING_ZERO_COUNT);
    }
    
    public CharsetEncoder getNewEncoder()
    {
        return mCharset.newEncoder();
    }
    
    public CharsetDecoder getNewDecoder()
    {
        return mCharset.newDecoder();
    }
    
    public String getDecodingReplacement()
    {
        return mDecodingReplacement;
    }

    public SimpleDateFormat getDateFormat()
    {
        return mDateFormat;        
    }
    
    public SimpleDateFormat getTimeFormat()
    {
        return mTimeFormat;
    }
    
    public SimpleDateFormat getTimestampFormat()
    {
        return mTimestampFormat;
    }
    
    public SimpleDateFormat getTimeTZFormat()
    {
        return mTimeTZFormat;
    }
    
    public SimpleDateFormat getTimestampTZFormat()
    {
        return mTimestampTZFormat;
    }
    
    /**************************************************************
     * LogWriterHost Interface Methods
     **************************************************************/
    public PrintWriter getLogWriter()
    {
        if (mOrigin == null)
        {
            return DriverManager.getLogWriter();
        }
        else
        {
            try
            {
                return mOrigin.getLogWriter();
            }
            catch (SQLException sException)
            {
            }
        }
        return null;
    }
    
    /**************************************************************
     * Connection Interface Methods
     **************************************************************/
    public void clearWarnings() throws SQLException
    {
        mLogger.logTrace();
        super.clearWarning();
    }

    public synchronized void close() throws SQLException
    {
        mLogger.logTrace();
        if (!mClosed)
        {
            synchronized (mStatements)
            {
                synchronized (mChannel)
                {
                    mChannel.initToSend();
                    if (mStatements.size() > 0)
                    {
                        int sAddedFreeProtoCount = 0;
                        while (!mStatements.isEmpty())
                        {
                            if (mStatements.removeFirst().closeByConnection())
                            {
                                sAddedFreeProtoCount++;
                            }
                        }
                        /*
                         * statement들이 id가 -1이라서 실제로 프로토콜을 보내지 않았다면
                         * 여기서 send를 해서는 안된다.
                         * 하지만 실제로는 statement id가 -1인게 있으면 안된다.
                         */
                        if (sAddedFreeProtoCount > 0)
                        {
                            mChannel.sendAndReceive();
                            mChannel.initToSend();
                        }
                    }
                    mChannel.writeProtocolAlone(Protocol.PROTOCOL_DISCONNECT, mConnectOp);
                    try
                    {
                        mChannel.sendAndReceive();
                    }
                    catch (SQLException sException)
                    {
                        /*
                         * 서버에서 disconnect에 대한 응답을 보내고 socket을 끊기 때문에
                         * 응답을 정상적으로 받을 수도 있고, socket이 끊겨 에러가 발생할 수도 있다.
                         */
                    }
                    try
                    {
                        mChannel.close();
                    }
                    catch (SQLException sException)
                    {
                    }
                }
            }
            mClosed = true;
        }
    }

    public synchronized void commit() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkNonAutoCommit();
        synchronized (mChannel)
        {
            mChannel.initToSend();
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_TRANSACTION, mCommitOp);
            mChannel.sendAndReceive();
        }
        testError();
    }

    public synchronized void rollback() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkNonAutoCommit();
        synchronized (mChannel)
        {
            mChannel.initToSend();
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_TRANSACTION, mRollbackOp);
            mChannel.sendAndReceive();
        }
        testError();
    }

    public synchronized void rollback(Savepoint aSavepoint) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkNonAutoCommit();
        executeInternalSql(INTERNAL_SQL_ROLLBACK_TO + ((Jdbc4Savepoint)aSavepoint).getQuerySavepointName());
    }

    public Statement createStatement() throws SQLException
    {
        mLogger.logTrace();
        return createStatement(GOLDILOCKS_DEFAULT_RESULT_SET_TYPE,
                               GOLDILOCKS_DEFAULT_RESULT_SET_CONCURRENCY,
                               mResultSetHoldability);
    }

    public Statement createStatement(int aResultSetType, int aResultSetConcurrency) throws SQLException
    {
        mLogger.logTrace();
        return createStatement(aResultSetType,
                               aResultSetConcurrency,
                               mResultSetHoldability);
    }

    public Statement createStatement(int aResultSetType,
                                     int aResultSetConcurrency,
                                     int aResultSetHoldability) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        Jdbc4Statement sStatement = createConcreteStatement(aResultSetType, aResultSetConcurrency, aResultSetHoldability);
        synchronized (mStatements)
        {
            mStatements.add(sStatement);
        }
        
        return sStatement;
    }

    public Struct createStruct(String aTypeName, Object[] aAttributes) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Struct type");
        return null;
    }

    public boolean getAutoCommit() throws SQLException
    {
        mLogger.logTrace();
        return mAutoCommit;
    }

    public String getCatalog() throws SQLException
    {
        mLogger.logTrace();
        return getValueByQuery(INTERNAL_SQL_CURRENT_CATALOG);
    }

    public int getHoldability() throws SQLException
    {
        mLogger.logTrace();
        return mResultSetHoldability;
    }

    public DatabaseMetaData getMetaData() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        if (mMetaData == null)
        {
            mMetaData = createConcreteDatabaseMetaData();
        }
        return mMetaData;
    }

    public synchronized int getTransactionIsolation() throws SQLException
    {
        mLogger.logTrace();
        synchronized (mChannel)
        {
            mChannel.initToSend();
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_GET_CONN_ATTR, mTxIsolationAttr);
            mChannel.sendAndReceive();
        }
        testError();
        return mTxIsolationAttr.getIsolationLevel();
    }

    public SQLWarning getWarnings() throws SQLException
    {
        mLogger.logTrace();
        return mWarning;
    }

    public boolean isClosed() throws SQLException
    {
        mLogger.logTrace();
        return mClosed;
    }

    public boolean isReadOnly() throws SQLException
    {
        mLogger.logTrace();
        synchronized (mChannel)
        {
            mChannel.initToSend();
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_GET_CONN_ATTR, mAccessModeAttr);
            mChannel.sendAndReceive();
        }
        testError();

        return !mAccessModeAttr.isUpdatable();
    }

    public String nativeSQL(String aSql) throws SQLException
    {
        mLogger.logTrace();
        // 현재 driver에서 sql문을 전처리하는 게 없다.
        return aSql;
    }

    public CallableStatement prepareCall(String aSql) throws SQLException
    {
        mLogger.logTrace();    
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "CallableStatement");
        return null;
    }

    public CallableStatement prepareCall(String aSql, int aResultSetType, int aResultSetConcurrency)
            throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "CallableStatement");
        return null;
    }

    public CallableStatement prepareCall(String aSql,
                                         int aResultSetType,
                                         int aResultSetConcurrency,
                                         int aResultSetHoldability) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "CallableStatement");                
        return null;
    }

    public PreparedStatement prepareStatement(String aSql) throws SQLException
    {
        mLogger.logTrace();
        return prepareStatement(aSql,
                                GOLDILOCKS_DEFAULT_RESULT_SET_TYPE,
                                GOLDILOCKS_DEFAULT_RESULT_SET_CONCURRENCY,
                                mResultSetHoldability);
    }

    public PreparedStatement prepareStatement(String aSql, int aAutoGeneratedKeys)
            throws SQLException
    {
        mLogger.logTrace();
        if ((aAutoGeneratedKeys != Statement.NO_GENERATED_KEYS) &&
            (aAutoGeneratedKeys != Statement.RETURN_GENERATED_KEYS))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, String.valueOf(aAutoGeneratedKeys));
        }
        if (aAutoGeneratedKeys == Statement.RETURN_GENERATED_KEYS)
        {
            ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Auto generated keys");
        }
        
        return prepareStatement(aSql,
                GOLDILOCKS_DEFAULT_RESULT_SET_TYPE,
                GOLDILOCKS_DEFAULT_RESULT_SET_CONCURRENCY,
                mResultSetHoldability);
    }

    public PreparedStatement prepareStatement(String aSql, int[] aColumnIndexes)
            throws SQLException
    {
        mLogger.logTrace();
        if (aColumnIndexes != null)
        {
            ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Auto generated keys");
        }
        
        return prepareStatement(aSql,
                                GOLDILOCKS_DEFAULT_RESULT_SET_TYPE,
                                GOLDILOCKS_DEFAULT_RESULT_SET_CONCURRENCY,
                                mResultSetHoldability);
    }

    public PreparedStatement prepareStatement(String aSql, String[] aColumnNames)
            throws SQLException
    {
        mLogger.logTrace();
        if (aColumnNames != null)
        {
            ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Auto generated keys");
        }
        
        return prepareStatement(aSql,
                                GOLDILOCKS_DEFAULT_RESULT_SET_TYPE,
                                GOLDILOCKS_DEFAULT_RESULT_SET_CONCURRENCY,
                                mResultSetHoldability);
    }

    public PreparedStatement prepareStatement(String aSql, int aResultSetType, int aResultSetConcurrency)
            throws SQLException
    {
        mLogger.logTrace();
        return prepareStatement(aSql,
                                aResultSetType,
                                aResultSetConcurrency,
                                mResultSetHoldability);
    }

    public PreparedStatement prepareStatement(String aSql, int aType, int aConcurrency, int aHoldability) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        Jdbc4Statement.validateSql(aSql);
        Jdbc4PreparedStatement sStatement = createConcretePreparedStatement(aSql, aType, aConcurrency, aHoldability);
        
        synchronized (mStatements)
        {
            mStatements.add(sStatement);
        }
        return sStatement;
    }

    public void releaseSavepoint(Savepoint aSavepoint) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkNonAutoCommit();
        executeInternalSql(INTERNAL_SQL_RELEASE_SAVEPOINT + ((Jdbc4Savepoint)aSavepoint).getQuerySavepointName());
    }

    public void setAutoCommit(boolean aAutoCommit) throws SQLException
    {
        mLogger.logTrace();
        if (mTransActive && aAutoCommit)
        {
            // active transaction이고 autocommit으로 바뀔 때면 명시적으로 commit을 해줘야 한다.
            commit();
        }
        mAutoCommit = aAutoCommit;
    }

    public void setCatalog(String aCatalog) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "changing catalog");
    }

    public void setHoldability(int aHoldability) throws SQLException
    {
        mLogger.logTrace();
        mResultSetHoldability = aHoldability;
    }

    public void setReadOnly(boolean aReadOnly) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mAccessModeAttr.setUpdatable(!aReadOnly);
        synchronized (mChannel)
        {
            mChannel.initToSend();
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_SET_CONN_ATTR, mAccessModeAttr);
            mChannel.sendAndReceive();
        }
        testError();
    }

    public synchronized Savepoint setSavepoint() throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkNonAutoCommit();
        Jdbc4Savepoint sResult = createConcreteSavepoint(mUnnamedSavepointId++);
        executeInternalSql(INTERNAL_SQL_SAVEPOINT + sResult.getQuerySavepointName());
        return sResult;
    }

    public synchronized Savepoint setSavepoint(String aSavepointName) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkNonAutoCommit();
        Jdbc4Savepoint sResult = createConcreteSavepoint(aSavepointName);
        executeInternalSql(INTERNAL_SQL_SAVEPOINT + sResult.getQuerySavepointName());
        return sResult;
    }

    public synchronized void setTransactionIsolation(int aIsolationLevel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mTxIsolationAttr.setIsolationLevel(aIsolationLevel);
        synchronized (mChannel)
        {
            mChannel.initToSend();
            mChannel.writeProtocolAlone(Protocol.PROTOCOL_SET_CONN_ATTR, mTxIsolationAttr);
            mChannel.sendAndReceive();
        }
        testError();
    }
    
    public Map<String, Class<?>> getTypeMap() throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "UDT Map");
        return null;
    }
    
    public void setTypeMap(Map<String, Class<?>> aMap) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "UDT Map");
    }
}
