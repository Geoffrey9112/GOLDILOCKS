package sunje.goldilocks.jdbc.core6;

import java.sql.ResultSet;
import java.sql.RowIdLifetime;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4DatabaseMetaData;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Jdbc6DatabaseMetaData extends Jdbc4DatabaseMetaData
{
    private static final String INTERNAL_SQL_CLIENT_PROPS = "SELECT" +
            " * FROM DICTIONARY_SCHEMA.JDBC_CLIENT_PROPS";
    private static final String INTERNAL_SQL_FUNCTION_COLUMNS = "SELECT" +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS FUNCTION_CAT, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS FUNCTION_SCHEM, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS FUNCTION_NAME, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS COLUMN_NAME, " +
            " CAST(NULL AS NATIVE_SMALLINT) AS COLUMN_TYPE, " +
            " CAST(NULL AS NATIVE_INTEGER) AS DATA_TYPE, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_NAME, " +
            " CAST(NULL AS NATIVE_INTEGER) AS PRECISION, " +
            " CAST(NULL AS NATIVE_INTEGER) AS LENGTH, " +
            " CAST(NULL AS NATIVE_SMALLINT) AS SCALE, " +
            " CAST(NULL AS NATIVE_SMALLINT) AS RADIX, " +
            " CAST(NULL AS NATIVE_SMALLINT) AS NULLABLE, " +
            " CAST(NULL AS VARCHAR(256 OCTETS)) AS REMARKS, " +
            " CAST(NULL AS NATIVE_INTEGER) AS CHAR_OCTET_LENGTH, " +
            " CAST(NULL AS NATIVE_INTEGER) AS ORDINAL_POSITION, " +
            " CAST(NULL AS VARCHAR(3 OCTETS)) AS IS_NULLABLE, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS SPECIFIC_NAME " +
            " FROM DUAL WHERE 1=2 ";

    private static final String INTERNAL_SQL_FUNCTIONS = "SELECT" +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS FUNCTION_CAT, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS FUNCTION_SCHEM, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS FUNCTION_NAME, " +
            " CAST(NULL AS VARCHAR(256 OCTETS)) AS REMARKS, " +
            " CAST(NULL AS NATIVE_SMALLINT) AS FUNCTION_TYPE, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS SPECIFIC_NAME " +
            " FROM DUAL WHERE 1=2 ";

    public Jdbc6DatabaseMetaData(Jdbc4Connection aConnection)
    {
        super(aConnection);
    }

    public <T> T unwrap(Class<T> aClass) throws SQLException
    {
        mLogger.logTrace();
        if (!aClass.isInstance(this))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_WRAPPER_TYPE, aClass.toString());
        }
        return aClass.cast(this);
    }

    public boolean isWrapperFor(Class<?> aClass) throws SQLException
    {
        mLogger.logTrace();
        if (!aClass.isInstance(this))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_WRAPPER_TYPE, aClass.toString());
        }
        return aClass.isInstance(this);
    }

    public int getSQLStateType() throws SQLException
    {
        mLogger.logTrace();
        return sqlStateSQL;
    }

    public RowIdLifetime getRowIdLifetime() throws SQLException
    {
        mLogger.logTrace();
        return RowIdLifetime.ROWID_VALID_FOREVER;
    }

    public ResultSet getClientInfoProperties() throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_CLIENT_PROPS);
    }

    public ResultSet getFunctions(String aCatalog, String aSchemaPattern, String aFunctionNamePattern) throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_FUNCTIONS);
    }

    public ResultSet getFunctionColumns(String aCatalog, String aSchemaPattern, String aFunctionNamePattern, String aColumnNamePattern) throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_FUNCTION_COLUMNS);
    }

    public ResultSet getSchemas(String aCatalog, String aSchemaPattern)
            throws SQLException
    {
        mLogger.logTrace();
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_SCHEMAS);
        
        if (aCatalog != null)
        {
            sSql.append(" AND TABLE_CATALOG = '").append(aCatalog).append('\'');
        }
        if (aSchemaPattern != null)
        {
            sSql.append(" AND TABLE_SCHEMA LIKE '").append(aSchemaPattern).append('\'');
        }

        sSql.append(INTERNAL_SQL_SCHEMAS_ORDER);
        return mConnection.executeInternalQuery(sSql.toString());
    }

    public boolean supportsStoredFunctionsUsingCallSyntax() throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }
    
    public boolean autoCommitFailureClosesAllResultSets() throws SQLException
    {
        mLogger.logTrace();
        // SQLException 날때, 특별히 commit, rollback, close cursor등을 하지 않는다.
        return false;
    }
}
