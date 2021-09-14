package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Protocol
{
    public static final short PROTOCOL_MAJOR_VERSION = 1;
    public static final short PROTOCOL_MINOR_VERSION = 2;
    public static final short PROTOCOL_PATCH_VERSION = 1;
    
    public static final byte RESULT_SUCCESS = 0;
    public static final byte RESULT_WARNING = 1;
    public static final byte RESULT_ABORT = 2;
    public static final byte RESULT_FATAL = 3;
    public static final byte RESULT_IGNORE = 4;

    public static final long STATEMENT_ID_UNDEFINED = -1;
    
    public static final boolean PROTOCOL_HEAD_PROTOCOL = true;
    public static final boolean PROTOCOL_NON_HEAD_PROTOCOL = false;
    
    public static final byte HANDSHAKE_BIG_ENDIAN = 0;
    public static final byte HANDSHAKE_LITTLE_ENDIAN = -1;
    
    public static final short NLS_CHARSET_ASCII = 0;
    public static final short NLS_CHARSET_UTF8 = 1;
    public static final short NLS_CHARSET_UHC = 2;
    public static final short NLS_CHARSET_GB18030 = 3;
    public static final short NLS_CHARSET_MAX = 4;
    
    public static final byte CONNECT_SESSION_DEDICATE = 1;
    public static final byte CONNECT_SESSION_SHARED = 2;
    public static final byte CONNECT_SESSION_DEFAULT = 3;
    
    public static final byte ATTR_DATA_TYPE_INT = sunje.goldilocks.jdbc.dt.CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER;
    public static final byte ATTR_DATA_TYPE_STRING = sunje.goldilocks.jdbc.dt.CodeColumn.GOLDILOCKS_DATA_TYPE_CHAR;
    
    public static final byte CONNECT_DBC_ODBC = 1;
    public static final byte CONNECT_DBC_JDBC = 2;

    public static final short FREESTMT_OPTION_DROP = 1;
    public static final short FREESTMT_OPTION_RESET_PARAM = 3;
    
    public static final short TRANSACTION_COMMIT = 0;
    public static final short TRANSACTION_ROLLBACK = 1;
    
    public static final int EXECDIR_STMT_TYPE_ALTER_DOMAIN          = 0;
    public static final int EXECDIR_STMT_TYPE_ALTER_TABLE           = 1;
    public static final int EXECDIR_STMT_TYPE_CALL                  = 2;
    public static final int EXECDIR_STMT_TYPE_CREATE_ASSERTION      = 3;
    public static final int EXECDIR_STMT_TYPE_CREATE_CHARACTER_SET  = 4;
    public static final int EXECDIR_STMT_TYPE_CREATE_COLLATION      = 5;
    public static final int EXECDIR_STMT_TYPE_CREATE_DOMAIN         = 6;
    public static final int EXECDIR_STMT_TYPE_CREATE_INDEX          = 7;
    public static final int EXECDIR_STMT_TYPE_CREATE_SCHEMA         = 8;
    public static final int EXECDIR_STMT_TYPE_CREATE_TABLE          = 9;
    public static final int EXECDIR_STMT_TYPE_CREATE_TRANSLATION    = 10;
    public static final int EXECDIR_STMT_TYPE_CREATE_VIEW           = 11;
    public static final int EXECDIR_STMT_TYPE_DELETE_WHERE          = 12;
    public static final int EXECDIR_STMT_TYPE_DROP_ASSERTION        = 13;
    public static final int EXECDIR_STMT_TYPE_DROP_CHARACTER_SET    = 14;
    public static final int EXECDIR_STMT_TYPE_DROP_COLLATION        = 15;
    public static final int EXECDIR_STMT_TYPE_DROP_DOMAIN           = 16;
    public static final int EXECDIR_STMT_TYPE_DROP_INDEX            = 17;
    public static final int EXECDIR_STMT_TYPE_DROP_SCHEMA           = 18;
    public static final int EXECDIR_STMT_TYPE_DROP_TABLE            = 19;
    public static final int EXECDIR_STMT_TYPE_DROP_TRANSLATION      = 20;
    public static final int EXECDIR_STMT_TYPE_DROP_VIEW             = 21;
    public static final int EXECDIR_STMT_TYPE_DYNAMIC_DELETE_CURSOR = 22;
    public static final int EXECDIR_STMT_TYPE_DYNAMIC_UPDATE_CURSOR = 23;
    public static final int EXECDIR_STMT_TYPE_GRANT                 = 24;
    public static final int EXECDIR_STMT_TYPE_INSERT                = 25;
    public static final int EXECDIR_STMT_TYPE_REVOKE                = 26;
    public static final int EXECDIR_STMT_TYPE_SELECT_CURSOR         = 27;
    public static final int EXECDIR_STMT_TYPE_UPDATE_WHERE          = 28;
    public static final int EXECDIR_STMT_TYPE_UNKNOWN               = 29;

    public static final long ROWSET_LAST_ROW_IDX_UNKNOWN = -1;
    
    public static final int STMT_ATTR_TYPE_PARAMSET_SIZE = 0;
    public static final int STMT_ATTR_TYPE_CONCURRENCY = 1;
    public static final int STMT_ATTR_TYPE_SCROLLABILITY = 2;
    public static final int STMT_ATTR_TYPE_SENSITIVITY = 3;
    public static final int STMT_ATTR_TYPE_CURSOR_TYPE = 4;
    public static final int STMT_ATTR_TYPE_ATOMIC_EXECUTION = 5;
    public static final int STMT_ATTR_TYPE_EXPLAIN_PLAN_OPTION = 6;
    public static final int STMT_ATTR_TYPE_EXPLAIN_PLAN_TEXT = 7;
    public static final int STMT_ATTR_TYPE_HOLDABILITY = 8;
    public static final int STMT_ATTR_TYPE_QUERY_TIMEOUT = 9;
    
    public static final int STMT_ATTR_VALUE_CONCURRENCY_READ_ONLY = 1;
    public static final int STMT_ATTR_VALUE_CONCURRENCY_UPDATABLE = 2;
    public static final int STMT_ATTR_VALUE_SCROLLABILITY_NO_SCROLL = 1;
    public static final int STMT_ATTR_VALUE_SCROLLABILITY_SCROLL = 2;
    public static final int STMT_ATTR_VALUE_SENSITIVITY_INSENSITIVE = 1;
    public static final int STMT_ATTR_VALUE_SENSITIVITY_SENSITIVE = 2;
    public static final int STMT_ATTR_VALUE_CURSOR_TYPE_ISO = 1;
    public static final int STMT_ATTR_VALUE_CURSOR_TYPE_KEYSET = 3;
    public static final int STMT_ATTR_VALUE_HOLDABILITY_WITH_HOLD = 1;
    public static final int STMT_ATTR_VALUE_HOLDABILITY_WITHOUT_HOLD = 2;
    public static final int STMT_ATTR_VALUE_ATOMIC_EXECUTION_OFF = 0;
    public static final int STMT_ATTR_VALUE_ATOMIC_EXECUTION_ON = 1;
    public static final int STMT_ATTR_VALUE_EXPLAIN_PLAN_OFF = 0;
    public static final int STMT_ATTR_VALUE_EXPLAIN_PLAN_ON = 1;
    public static final int STMT_ATTR_VALUE_EXPLAIN_PLAN_ON_VERBOSE = 2;
    public static final int STMT_ATTR_VALUE_EXPLAIN_PLAN_ONLY = 3;
    
    public static final byte FETCH_ROW_STATUS_NA = 0;
    public static final byte FETCH_ROW_STATUS_NO_CHANGE = 1;
    public static final byte FETCH_ROW_STATUS_INSERTED = 2;
    public static final byte FETCH_ROW_STATUS_UPDATED = 3;
    public static final byte FETCH_ROW_STATUS_DELETED = 4;
    
    public static final short PARAMETER_TYPE_IN = 1;
    public static final short PARAMETER_TYPE_INOUT = 2;
    public static final short PARAMETER_TYPE_OUT = 4;
    
    public static final short CHARSET_ID_ASCII = 0;
    public static final short CHARSET_ID_UTF8 = 1;
    public static final short CHARSET_ID_UHC = 2;
    public static final short CHARSET_ID_MAX = 3;
    
    public static final int FETCH_COUNT_AUTO = 0;
    public static final int FETCH_COUNT_UNLIMITED = -1;
    public static final int DEFAULT_FETCH_COUNT = FETCH_COUNT_AUTO;
    public static final int SERVER_PROPOSED_FETCH_COUNT = 100;
    public static final int DEFAULT_FETCH_COUNT_FOR_SCROLLABLE_CURSOR = SERVER_PROPOSED_FETCH_COUNT;
    
    public static final int DEFAULT_STATEMENT_QUERY_TIMEOUT = -1; // default는 session query timeout에 따르는 것이다. 0은 무한대기
    
    public static final byte CONTROL_PACKET_OP_POOLING = 1;
    
    public static final int HANDSHAKE_BUF_SIZE = 32*1024; // 서버에서 정한 버퍼 크기임

    public static final int XA_RESLUT_RBROLLBACK = 100;
    public static final int XA_RESLUT_RBCOMMFAIL = 101;
    public static final int XA_RESLUT_RBDEADLOCK = 102;
    public static final int XA_RESLUT_RBINTEGRITY = 103;
    public static final int XA_RESLUT_RBOTHER = 104;
    public static final int XA_RESLUT_RBPROTO = 105;
    public static final int XA_RESLUT_RBTIMEOUT = 106;
    public static final int XA_RESLUT_RBTRANSIENT = 107;
    public static final int XA_RESLUT_RBEND = XA_RESLUT_RBTRANSIENT;
    public static final int XA_RESLUT_NOMIGRATE = 9;
    public static final int XA_RESLUT_HEURHAZ = 8;
    public static final int XA_RESLUT_HEURCOM = 7;
    public static final int XA_RESLUT_HEURRB = 6;
    public static final int XA_RESLUT_HEURMIX = 5;
    public static final int XA_RESLUT_RETRY = 4;
    public static final int XA_RESLUT_RDONLY = 3;
    public static final int XA_RESLUT_OK = 0;
    public static final int XA_RESLUT_ER_ASYNC = -2;
    public static final int XA_RESLUT_ER_RMERR = -3;
    public static final int XA_RESLUT_ER_NOTA = -4;
    public static final int XA_RESLUT_ER_INVAL = -5;
    public static final int XA_RESLUT_ER_PROTO = -6;
    public static final int XA_RESLUT_ER_RMFAIL = -7;
    public static final int XA_RESLUT_ER_DUPID = -8;
    public static final int XA_RESLUT_ER_OUTSIDE = -9;    

    public static final long XA_FLAGS_NOFLAGS = 0x00000000l;   /**< no resource manager features selected */
    public static final long XA_FLAGS_REGISTER = 0x00000001l;  /**< resource manager dynamically registers */
    public static final long XA_FLAGS_NOMIGRATE = 0x00000002l; /**< resource manager does not support migration */
    public static final long XA_FLAGS_USEASYNC = 0x00000004l;  /**< resource manager supports asynch operations */

    public static final long XA_FLAGS_ASYNC = 0x80000000l;     /**< perform routine asynchronously */
    public static final long XA_FLAGS_ONEPHASE = 0x40000000l;  /**< one-phase commit */
    public static final long XA_FLAGS_FAIL = 0x20000000l;      /**< dissociates caller and marks branch rollback-only */
    public static final long XA_FLAGS_NOWAIT = 0x10000000l;    /**< return if blocking condition exists */
    public static final long XA_FLAGS_RESUME = 0x08000000l;    /**< resuming association */
    public static final long XA_FLAGS_SUCCESS = 0x04000000l;   /**< dissociates caller from branch */
    public static final long XA_FLAGS_SUSPEND = 0x02000000l;   /**< suspending, not ending, association */
    public static final long XA_FLAGS_STARTRSCAN = 0x01000000l;/**< start a recovery scan */
    public static final long XA_FLAGS_ENDRSCAN = 0x00800000l;  /**< end a recovery scan */
    public static final long XA_FLAGS_MULTIPLE = 0x00400000l;  /**< wait for any async operation */
    public static final long XA_FLAGS_JOIN = 0x00200000l;      /**< joining existing branch */
    public static final long XA_FLAGS_MIGRATE = 0x00100000l;   /**< intends to perform migration */

    public static final Protocol PROTOCOL_BIND_DATA = new ProtocolBindData();
    public static final Protocol PROTOCOL_BIND_TYPE = new ProtocolBindType();
    public static final Protocol PROTOCOL_CLOSE_CURSOR = new ProtocolCloseCursor();
    public static final Protocol PROTOCOL_CONNECT = new ProtocolConnect();
    public static final Protocol PROTOCOL_DESCRIBE_COL = new ProtocolDescribeCol();
    public static final Protocol PROTOCOL_DISCONNECT = new ProtocolDisconnect();
    public static final Protocol PROTOCOL_EXECUTE = new ProtocolExecute();
    public static final Protocol PROTOCOL_EXECUTE_DIRECT = new ProtocolExecuteDirect();
    public static final Protocol PROTOCOL_FETCH = new ProtocolFetch();
    public static final Protocol PROTOCOL_FREE_STATEMENT = new ProtocolFreeStatement();
    public static final Protocol PROTOCOL_GET_CONN_ATTR = new ProtocolGetConnAttr();
    public static final Protocol PROTOCOL_GET_CURSOR_NAME = new ProtocolGetCursorName();
    public static final Protocol PROTOCOL_HANDSHAKE = new ProtocolHandshake();
    public static final Protocol PROTOCOL_PREPARE = new ProtocolPrepare();
    public static final Protocol PROTOCOL_SET_CONN_ATTR = new ProtocolSetConnAttr();
    public static final Protocol PROTOCOL_SET_CURSOR_NAME = new ProtocolSetCursorName();
    public static final Protocol PROTOCOL_GET_STMT_ATTR = new ProtocolGetStmtAttr();
    public static final Protocol PROTOCOL_SET_STMT_ATTR = new ProtocolSetStmtAttr();
    public static final Protocol PROTOCOL_NUM_PARAMS = new ProtocolNumParams();
    public static final Protocol PROTOCOL_NUM_RESULT_COLS = new ProtocolNumResultCols();
    public static final Protocol PROTOCOL_TARGET_TYPE = new ProtocolTargetType();
    public static final Protocol PROTOCOL_TRANSACTION = new ProtocolTransaction();
    public static final Protocol PROTOCOL_XA_CLOSE = new ProtocolXAClose();
    public static final Protocol PROTOCOL_XA_COMMIT = new ProtocolXACommit();
    public static final Protocol PROTOCOL_XA_ROLLBACK = new ProtocolXARollback();
    public static final Protocol PROTOCOL_XA_PREPARE = new ProtocolXAPrepare();
    public static final Protocol PROTOCOL_XA_START = new ProtocolXAStart();
    public static final Protocol PROTOCOL_XA_END = new ProtocolXAEnd();
    public static final Protocol PROTOCOL_XA_FORGET = new ProtocolXAForget();
    public static final Protocol PROTOCOL_XA_RECOVER = new ProtocolXARecover();
    public static final Protocol PROTOCOL_PARAMETER_TYPE = new ProtocolParameterType();
    public static final Protocol PROTOCOL_CONTROL_PACKET = new ProtocolControlPacket();
    public static final Protocol PROTOCOL_LISTENER_HANDSHAKE = new ProtocolListenerHandshake();
    
    public static final short PROTOCOL_CODE_HANDSHAKE = 1;
    public static final short PROTOCOL_CODE_CONNECT = 2;
    public static final short PROTOCOL_CODE_DISCONNECT = 3;
    public static final short PROTOCOL_CODE_GETCONNATTR = 4;
    public static final short PROTOCOL_CODE_SETCONNATTR = 5;
    public static final short PROTOCOL_CODE_FREESTMT = 6;
    public static final short PROTOCOL_CODE_CANCEL = 7;
    public static final short PROTOCOL_CODE_DESCRIBECOL = 8;
    public static final short PROTOCOL_CODE_TRANSACTION = 9;
    public static final short PROTOCOL_CODE_PREPARE = 10;
    public static final short PROTOCOL_CODE_EXECDIRECT = 11;             
    public static final short PROTOCOL_CODE_EXECUTE = 12;                
    public static final short PROTOCOL_CODE_GETOUTBINDDATA = 13;
    public static final short PROTOCOL_CODE_FETCH = 14;
    public static final short PROTOCOL_CODE_TARGETTYPE = 15;
    public static final short PROTOCOL_CODE_BINDTYPE = 16;
    public static final short PROTOCOL_CODE_BINDDATA = 17;
    public static final short PROTOCOL_CODE_GETSTMTATTR = 18;
    public static final short PROTOCOL_CODE_SETSTMTATTR = 19;
    public static final short PROTOCOL_CODE_NUMPARAMS = 20;
    public static final short PROTOCOL_CODE_NUMRESULTCOLS = 21;
    public static final short PROTOCOL_CODE_GETCURSORNAME = 22;
    public static final short PROTOCOL_CODE_SETCURSORNAME = 23;
    public static final short PROTOCOL_CODE_SETPOS = 24;
    public static final short PROTOCOL_CODE_CLOSECURSOR = 25;
    public static final short PROTOCOL_CODE_STARTUP = 26;
    public static final short PROTOCOL_CODE_XA_CLOSE = 27;
    public static final short PROTOCOL_CODE_XA_COMMIT = 28;
    public static final short PROTOCOL_CODE_XA_ROLLBACK = 29;
    public static final short PROTOCOL_CODE_XA_PREPARE = 30;
    public static final short PROTOCOL_CODE_XA_START = 31;
    public static final short PROTOCOL_CODE_XA_END = 32;
    public static final short PROTOCOL_CODE_XA_FORGET = 33;
    public static final short PROTOCOL_CODE_XA_RECOVER = 34;
    public static final short PROTOCOL_CODE_PARAMETER_TYPE = 35;
    public static final short PROTOCOL_CODE_PERIOD = 36;
    public static final short PROTOCOL_CODE_CONTROL_PACKET = 37;
    public static final short PROTOCOL_CODE_LISTENER_HANDSHAKE = 38;
    
    public static final byte DEPENDENCY_HEAD = 1;
    public static final byte DEPENDENCY_TAIL = 2;
    public static final byte DEPENDENCY_BODY = 0;
    public static final byte DEPENDENCY_ALONE = 3;

    static final byte VAR_INT_1BYTE_MIN = (byte)0x83;
    static final byte VAR_INT_1BYTE_MAX = (byte)0x7F;
    static final byte VAR_INT_2BYTE_FLAG = (byte)0x82;
    static final byte VAR_INT_4BYTE_FLAG = (byte)0x81;
    static final byte VAR_INT_8BYTE_FLAG = (byte)0x80;
    
    static final short STATEMENT_ID_1BYTE_MAX = (short)0xFC;    
    static final byte STATEMENT_ID_2BYTE_FLAG = (byte)0xFD;
    static final byte STATEMENT_ID_4BYTE_FLAG = (byte)0xFE;
    static final byte STATEMENT_ID_8BYTE_FLAG = (byte)0xFF;
    
    static final byte PACKET_HEADER_PERIOD_CONTINUE = 0;
    static final byte PACKET_HEADER_PERIOD_END = (byte)0x80;
    
    static final byte BYTE_STATEMENT_ID_UNDEFIEND = STATEMENT_ID_8BYTE_FLAG;
    static final byte BYTE_STATEMENT_ID_PREDEFIEND = STATEMENT_ID_4BYTE_FLAG;
    
    static final String[] PROTOCOL_NAME = {
        "Invalid",
        "Handshake",
        "Connect",
        "Disconnect",
        "GetConnAttr",
        "SetConnAttr",
        "FreeStmt",
        "Cancel",
        "DescribeCol",
        "Transaction",
        "Prepare",
        "ExecuteDirect",             
        "Execute",
        "GetOutBindData",
        "Fetch",
        "TargetType",
        "BindType",
        "BindData",
        "GetStmtAttr",
        "SetStmtAttr",
        "NumParams",
        "NumResultCols",
        "GetCursorName",
        "SetCursorName",
        "SetPos",
        "CloseCursor",
        "Startup",
        "XaClose",
        "XaCommit",
        "XaRollback",
        "XaPrepare",
        "XaStart",
        "XaEnd",
        "XaForget",
        "XaRecover",
        "ParameterType",
        "Period",
        "ControlPacket",
        "ListenerHandshake"};

    public Protocol()
    {
    }
    
    abstract short getProtocolCode();
    
    abstract void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException;
    
    abstract void readProtocol(CmChannel aChannel, Source aSource) throws SQLException;

    public static String makeErrorMessage(String aMessage, String aDetailedMessage)
    {
        return aMessage + ":" + aDetailedMessage;
    }
    
    public static String decodeSqlState(int aStateCode)
    {
        char[] sState = new char[5];
        
        for (int i=0; i<5; i++)
        {
            sState[i] = (char)('0' + (aStateCode & 0x3F));
            aStateCode = aStateCode >> 6;
        }
        
        return new String(sState);
    }

    boolean readProtocolCommon(CmChannel aChannel, Source aSource) throws SQLException
    {
        short sCode = (short)aChannel.readVarInt();
        if (sCode != getProtocolCode())
        {
            ErrorMgr.raise(ErrorMgr.INVALID_PROTOCOL, sCode + "!=" + getProtocolCode());
        }
        byte sResultCode = aChannel.readByte();
        
        return checkErrorResult(aChannel, sResultCode, aSource);
    }
    
    boolean checkErrorResult(CmChannel aChannel, byte aResultCode, Source aSource) throws SQLException
    {
        boolean sResult; // 현재 result를 계속해서 읽어야 하는지의 여부를 알려준다.
        
        if (aResultCode == RESULT_SUCCESS)
        {
            sResult = true;
        }
        else if (aResultCode == RESULT_WARNING)
        {
            byte sErrorCount = aChannel.readByte();
            
            if (sErrorCount <= 0)
            {
                ErrorMgr.raise(ErrorMgr.INVALID_PROTOCOL, "Error Result");
            }
            
            for (byte i=0; i<sErrorCount; i++)
            {
                int sStateCode = aChannel.readInt();
                int sCode = aChannel.readInt();
                String sMessage = aChannel.readString();
                String sDetailedMessage = aChannel.readString();
                aSource.notifyWarning(this, makeErrorMessage(sMessage, sDetailedMessage), decodeSqlState(sStateCode), sCode);
            }
            sResult = true;
        }
        else if (aResultCode == RESULT_IGNORE)
        {
            aSource.notifyIgnoredError(this);
            sResult = false;
        }
        else
        {
            if ((aResultCode != RESULT_ABORT) && (aResultCode != RESULT_FATAL))
            {
                ErrorMgr.raise(ErrorMgr.INVALID_PROTOCOL, "Invalid result code");
            }
            
            byte sErrorCount = aChannel.readByte();
            
            if (sErrorCount <= 0)
            {
                ErrorMgr.raise(ErrorMgr.INVALID_PROTOCOL, "Error Result");
            }
            
            for (byte i=0; i<sErrorCount; i++)
            {
                int sStateCode = aChannel.readInt();
                int sCode = aChannel.readInt();
                String sMessage = aChannel.readString();
                String sDetailedMessage = aChannel.readString();
                aSource.notifyError(this, makeErrorMessage(sMessage, sDetailedMessage), decodeSqlState(sStateCode), sCode);
            }
            sResult = false;
        }
        return sResult;
    }
}
