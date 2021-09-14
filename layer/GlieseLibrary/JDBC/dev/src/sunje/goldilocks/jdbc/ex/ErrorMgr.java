package sunje.goldilocks.jdbc.ex;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.sql.SQLException;
import java.sql.SQLWarning;

import sunje.goldilocks.jdbc.core4.VersionSpecific;

class JdbcError
{
    private static final int JDBC_ERROR_CODE_BASE = 21000;
    
    Class<? extends SQLException> mExceptionType;
    int mErrorCode;
    String mSqlState;
    String mErrorMessage;

    JdbcError(Class<? extends SQLException> aExceptionType, int aErrorCode, String aSqlState, String aErrorMessage)
    {
        mExceptionType = aExceptionType;
        mErrorCode = JDBC_ERROR_CODE_BASE + aErrorCode;
        mSqlState = aSqlState;
        mErrorMessage = aErrorMessage;
    }
    
    String getMessage(String ... aArgs)
    {
        String sResult = mErrorMessage;
        for (String sArg : aArgs)
        {
            sResult = sResult.replaceFirst("%s", sArg);
        }
        
        return sResult;
    }
}

class JdbcWarning
{
    private static final int JDBC_WARNING_CODE_BASE = 22000;
    
    Class<? extends SQLWarning> mExceptionType;
    int mErrorCode;
    String mSqlState;
    String mErrorMessage;
        
    JdbcWarning(Class<? extends SQLWarning> aExceptionType, int aErrorCode, String aSqlState, String aErrorMessage)
    {
        mExceptionType = aExceptionType;
        mErrorCode = JDBC_WARNING_CODE_BASE + aErrorCode;
        mSqlState = aSqlState;
        mErrorMessage = aErrorMessage;
    }
    
    String getMessage(String ... aArgs)
    {
        String sResult = mErrorMessage;
        for (String sArg : aArgs)
        {
            sResult = sResult.replaceFirst("%s", sArg);
        }
        
        return sResult;
    }
}

public class ErrorMgr
{
    public static final JdbcError INVALID_CONNECT_URL = new JdbcError(SQLException.class, 1, "46001", "Invalid connection url[%s]: [%s]");
    public static final JdbcError UNSUPPERTED_FEATURE = new JdbcError(VersionSpecific.SINGLETON.getNotSupportedExceptionClass(), 2, "46110", "Unsupported feature: [%s]");
    public static final JdbcError INVALID_PROTOCOL = new JdbcError(SQLException.class, 3, "08S01", "Invalid protocol: [%s]");
    public static final JdbcError READ_ONLY_VIOLATION = new JdbcError(SQLException.class, 4, "99999", "Invalid method call for read-only ResultSet");
    public static final JdbcError CONNECTION_CLOSED = new JdbcError(SQLException.class, 5, "08003", "The Connection is already closed");
    public static final JdbcError STATEMENT_CLOSED = new JdbcError(SQLException.class, 6, "08003", "The Statement is already closed");
    public static final JdbcError RESULTSET_CLOSED = new JdbcError(SQLException.class, 7, "08003", "The ResultSet is already closed");
    public static final JdbcError FORWARD_ONLY_VIOLATION = new JdbcError(SQLException.class, 8, "99999", "Invalid method call for forward-only ResultSet");
    public static final JdbcError INVALID_WRAPPER_TYPE = new JdbcError(SQLException.class, 9, "99999", "Invalid Wrapper type: [%s]");
    public static final JdbcError INVALID_ARGUMENT = new JdbcError(SQLException.class, 10, "99999", "Invalid argument: [%s]");
    public static final JdbcError OPEN_FAILURE = new JdbcError(SQLException.class, 11, "08000", "Cannot connect to the server: %s");
    public static final JdbcError LINK_FAILURE = new JdbcError(SQLException.class, 12, "08S01", "Communication link failure: %s");
    public static final JdbcError MANDATORY_PROPERTY_OMITTED = new JdbcError(SQLException.class, 13, "99999", "A mandatory property is omitted: [%s]");
    public static final JdbcError NOT_SELECT_QUERY = new JdbcError(SQLException.class, 14, "99999", "Not a select query for executeQuery method");
    public static final JdbcError SQL_PRODUCE_RESULTSET = new JdbcError(SQLException.class, 15, "99999", "The statement has produced a ResultSet"); 
    public static final JdbcError INVALID_CALL_FOR_PREPARED_STMT = new JdbcError(SQLException.class, 16, "99999", "This method cannot be called for a PreparedStatement");
    public static final JdbcError INVALID_PARAMETER_TYPE_DURING_BATCH = new JdbcError(SQLException.class, 17, "99999", "Parameter type[%s] is mismatch with previous type[%s] during batch");
    public static final JdbcError INSUFFICIENT_PARAMETERS = new JdbcError(SQLException.class, 18, "99999", "Lack of parameter: [%s]'th parameter");
    public static final JdbcError DEPRECATED_NO_BATCH_JOB = new JdbcError(SQLException.class, 19, "99999", "No batch job added"); /* warning으로 변경됨 */
    public static final JdbcError BATCH_JOB_VIOLATION = new JdbcError(SQLException.class, 20, "99999", "Batch job added");
    public static final JdbcError VALUE_OUT_OF_RANGE = new JdbcError(SQLException.class, 21, "22003", "The value[%s] is out of range of [%s] type");
    public static final JdbcError VALUE_LENGTH_MISMATCH = new JdbcError(SQLException.class, 22, "22026", "The value length is mismatch[%s != %s]");
    public static final JdbcError TYPE_CONVERSION_ERROR = new JdbcError(SQLException.class, 23, "99999", "The value([%s] type) cannot be converted to [%s] type");
    public static final JdbcError CONNECT_TIMEOUT = new JdbcError(SQLException.class, 24, "08000", "The connection timeout expired: [%s millis]");
    public static final JdbcError COLUMN_NOT_FOUND = new JdbcError(SQLException.class, 25, "99999", "The column is not found: [%s]");
    public static final JdbcError SAVEPOINT_ERROR = new JdbcError(SQLException.class, 26, "3B000", "Savepoint error: %s");
    public static final JdbcError AUTO_COMMIT_ERROR = new JdbcError(SQLException.class, 27, "99999", "Cannot execute the query in auto-commit mode");
    public static final JdbcError INVALID_CURSOR_POSITION = new JdbcError(SQLException.class, 28, "24000", "Invalid cursor position");
    public static final JdbcError NO_COLUMN_READ = new JdbcError(SQLException.class, 29, "99999", "No column has been read");
    public static final JdbcError ENCODING_ERROR = new JdbcError(SQLException.class, 30, "99999", "Encoding error: [%s]");
    public static final JdbcError DECODING_ERROR = new JdbcError(SQLException.class, 31, "99999", "Decoding error: [%s]");
    public static final JdbcError STREAM_ERROR = new JdbcError(SQLException.class, 32, "99999", "Stream error: [%s]");
    public static final JdbcError READ_THE_DELETED_ROW = new JdbcError(SQLException.class, 33, "99999", "Trying to read the deleted row");    
    public static final JdbcError INVALID_INTERVAL_FORMAT = new JdbcError(SQLException.class, 34, "99999", "Invalid interval format string: [%s]");
    public static final JdbcError STATEMENT_HAS_AN_OPENED_RESULTSET = new JdbcError(SQLException.class, 35, "99999", "The Statement has an opened ResultSet");
    public static final JdbcError ROWID_IS_NOT_GOLDILOCKS_ROWID = new JdbcError(SQLException.class, 36, "99999", "The RowId object is not an intanceof the base class of Goldilocks's RowId");
    public static final JdbcWarning IGNORED_UNSUPPERTED_FEATURE = new JdbcWarning(SQLWarning.class, 1, "46110", "Unsupported feature and ignored: [%s]");
    public static final JdbcWarning IGNORED_INVALID_PROPERTY = new JdbcWarning(SQLWarning.class, 1, "99999", "Property value is invalid: [%s:%s]");
    
    public static final String ETC_ERROR_MESSAGE_ALREADY_CLOSED = "Already closed stream";
    public static final String ETC_ERROR_MESSAGE_DECODING_ERROR = "Decoding error";
    public static final String ETC_ERROR_INVALID_METHOD_CALL = "Invalid method call";
    
    public static void raise(JdbcError aError, String ... aArgs) throws SQLException
    {
        SQLException sException = getException(aError, aArgs);
        if (sException != null)
        {
            throw sException;
        }
    }
    
    public static SQLException getException(JdbcError aError, String ... aArgs)
    {
        try
        {
            Constructor<? extends SQLException> sConstructor = aError.mExceptionType.getConstructor(String.class, String.class, Integer.TYPE);
            SQLException sException = (SQLException)sConstructor.newInstance(aError.getMessage(aArgs), aError.mSqlState, aError.mErrorCode);
            sException.fillInStackTrace();
            return sException;
        }
        catch (NoSuchMethodException sException)
        {                
        }
        catch (InstantiationException sException)
        {                
        }
        catch (IllegalAccessException sException)
        {                
        }
        catch (IllegalArgumentException sException)
        {                
        }
        catch (InvocationTargetException sException)
        {                
        }
        return null;
    }
    
    public static SQLWarning chainWarning(SQLWarning aParentWarning, JdbcWarning aError, String ... aArgs)
    {
        try
        {
            Constructor<? extends SQLWarning> sConstructor = aError.mExceptionType.getConstructor(String.class, String.class, Integer.TYPE);
            SQLWarning sWarning = (SQLWarning)sConstructor.newInstance(aError.getMessage(aArgs), aError.mSqlState, aError.mErrorCode);
            sWarning.fillInStackTrace();
            if (aParentWarning == null)
            {
                aParentWarning = sWarning;
            }
            else
            {
                aParentWarning.setNextWarning(sWarning);                
            }
        }
        catch (NoSuchMethodException sException)
        {                
        }
        catch (InstantiationException sException)
        {                
        }
        catch (IllegalAccessException sException)
        {                
        }
        catch (IllegalArgumentException sException)
        {                
        }
        catch (InvocationTargetException sException)
        {                
        }
    
        return aParentWarning;
    }
    
    public static void raiseRuntimeError(String aReason)
    {
        throw new RuntimeException("Internal error: " + aReason);
    }
}
