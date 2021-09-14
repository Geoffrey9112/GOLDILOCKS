package sunje.goldilocks.jdbc.core4;

import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.HashMap;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.util.Logger;

public abstract class Jdbc4DatabaseMetaData implements DatabaseMetaData
{
    private static Object LOCK_FOR_GLOBAL_INFO = new Object();
    private static HashMap<String, String> GLOBAL_INFO_TABLE = null;
    
    protected static final String SERVER_INFO_ID_MAX_CONNECTIONS = "0";
    protected static final String SERVER_INFO_ID_SEARCH_STRING_ESCAPE = "14";
    protected static final String SERVER_INFO_ID_EXPRESSIONS_IN_ORDER_BY = "27";
    protected static final String SERVER_INFO_ID_IDENTIFIER_QUOTE_CHAR = "29";
    protected static final String SERVER_INFO_ID_MAX_COLUMN_NAME_LENGTH = "30";
    protected static final String SERVER_INFO_ID_MAX_CURSOR_NAME_LENGTH = "31";
    protected static final String SERVER_INFO_ID_MAX_SCHEMA_NAME_LENGTH = "32";
    protected static final String SERVER_INFO_ID_MAX_PROCEDURE_NAME_LENGTH = "33";
    protected static final String SERVER_INFO_ID_MAX_CATALOG_NAME_LENGTH = "34";
    protected static final String SERVER_INFO_ID_MAX_TABLE_NAME_LENGTH = "35";
    protected static final String SERVER_INFO_ID_SCHEMA_TERM = "39";
    protected static final String SERVER_INFO_ID_PROCEDURE_TERM = "40";
    protected static final String SERVER_INFO_ID_CATALOG_NAME_SEPARATOR = "41";
    protected static final String SERVER_INFO_ID_CATALOG_TERM = "42";
    protected static final String SERVER_INFO_ID_TXN_ISOLATION_LEVEL = "72";
    protected static final String SERVER_INFO_ID_INTEGRITY = "73";
    protected static final String SERVER_INFO_ID_CORRELATION_NAME = "74";
    protected static final String SERVER_INFO_ID_COLUMN_ALIASING = "87";
    protected static final String SERVER_INFO_ID_SQL_KEYWORDS = "89";
    protected static final String SERVER_INFO_ID_UNION = "96";
    protected static final String SERVER_INFO_ID_MAX_COLUMNS_IN_GROUP_BY = "97";
    protected static final String SERVER_INFO_ID_MAX_COLUMNS_IN_INDEX = "98";
    protected static final String SERVER_INFO_ID_MAX_COLUMNS_IN_ORDER_BY = "99";
    protected static final String SERVER_INFO_ID_MAX_COLUMNS_IN_SELECT = "100";
    protected static final String SERVER_INFO_ID_MAX_COLUMNS_IN_TABLE = "101";
    protected static final String SERVER_INFO_ID_MAX_INDEX_LENGTH = "102";
    protected static final String SERVER_INFO_ID_MAX_ROW_SIZE = "104";
    protected static final String SERVER_INFO_ID_MAX_STATEMENT_LENGTH = "105";
    protected static final String SERVER_INFO_ID_MAX_TABLES_IN_SELECT = "106";
    protected static final String SERVER_INFO_ID_MAX_USER_NAME_LENGTH = "107";
    protected static final String SERVER_INFO_ID_MAX_CHAR_LITERAL_LENGTH = "108";
    protected static final String SERVER_INFO_ID_MAX_BINARY_LITERAL_LENGTH = "112";
    protected static final String SERVER_INFO_ID_LIKE_ESCAPE = "113";
    
    protected static final String SERVER_STATIC_INFO_IDENTIFIER_EXTRA_CHARACTERS = "-$";
    
    protected static final int SERVER_INFO_BASE_CORRELATTION_NAME_NONE = 0;
    protected static final int SERVER_INFO_BASE_CORRELATTION_NAME_DIFFERENT = 1;
    protected static final int SERVER_INFO_BASE_TXN_READ_UNCOMMITTED = 1;
    protected static final int SERVER_INFO_BASE_TXN_READ_COMMITTED = 2;
    protected static final int SERVER_INFO_BASE_TXN_REPEATABLE_READ = 4;
    protected static final int SERVER_INFO_BASE_TXN_SERIALIZABLE = 8;
    protected static final int SERVER_INFO_BASE_UNION = 1;
    protected static final int SERVER_INFO_BASE_UNION_ALL = 2;
    protected static final String SERVER_INFO_BASE_IS_THE_FUNCTION_SUPPERTTED = "Y";
    //SERVER_INFO_BASE_CORRELATTION_NAME_ANY = 2 는 사용하지 않음
    
    private static final String INTERNAL_SQL_CURRENT_USER = "SELECT CURRENT_USER FROM DUAL";
    private static final String INTERNAL_SQL_TABLES = "SELECT" +
            " TABLE_CATALOG TABLE_CAT, " +
            " TABLE_SCHEMA TABLE_SCHEM, " +
            " TABLE_NAME TABLE_NAME, " +
            " DBC_TABLE_TYPE TABLE_TYPE, " +
            " COMMENTS REMARKS, " +
            " USER_DEFINED_TYPE_CATALOG TYPE_CAT, " +
            " USER_DEFINED_TYPE_SCHEMA TYPE_SCHEM, " +
            " USER_DEFINED_TYPE_NAME TYPE_NAME, " +
            " SELF_REFERENCING_COLUMN_NAME SELF_REFERENCING_COLUMN_NAME, " +
            " REFERENCE_GENERATION REF_GENERATION " +
            " FROM INFORMATION_SCHEMA.TABLES WHERE 1 = 1";
    private static final String INTERNAL_SQL_TABLES_ORDER = " ORDER BY TABLE_TYPE, TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME";
    private static final String INTERNAL_SQL_CATALOGS = "SELECT CATALOG_NAME TABLE_CAT FROM INFORMATION_SCHEMA.INFORMATION_SCHEMA_CATALOG_NAME ORDER BY CATALOG_NAME";
    protected static final String INTERNAL_SQL_SCHEMAS = "SELECT" +
            " DISTINCT TABLE_SCHEMA TABLE_SCHEM, " +
            " TABLE_CATALOG TABLE_CAT " +
            " FROM INFORMATION_SCHEMA.TABLES WHERE 1 = 1";
    protected static final String INTERNAL_SQL_SCHEMAS_ORDER = " ORDER BY TABLE_CATALOG, TABLE_SCHEMA";
    private static final String INTERNAL_SQL_TABLE_TYPES = "SELECT DBC_TABLE_TYPE AS TABLE_TYPE FROM DICTIONARY_SCHEMA.DBC_TABLE_TYPE_INFO WHERE IS_SUPPORTED = TRUE ORDER BY DBC_TABLE_TYPE_ID";
    private static final String INTERNAL_SQL_COLUMNS = "SELECT" +
            "  CAST(TABLE_CATALOG  AS VARCHAR(128 OCTETS)) AS TABLE_CAT, " +
            "  CAST(TABLE_SCHEMA   AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, " +
            "  CAST(TABLE_NAME     AS VARCHAR(128 OCTETS)) AS TABLE_NAME, " +
            "  CAST(COLUMN_NAME    AS VARCHAR(128 OCTETS)) AS COLUMN_NAME, " +
            "  CAST(JDBC_DATA_TYPE AS NATIVE_INTEGER)      AS DATA_TYPE, " +
            "  CAST(DATA_TYPE      AS VARCHAR(128 OCTETS)) AS TYPE_NAME, " +
            "  CAST(CASE " +
            "         WHEN DATA_TYPE = 'CHARACTER' " +
            "           OR DATA_TYPE = 'CHARACTER VARYING' " +
            "           OR DATA_TYPE = 'CHARACTER LONG VARYING' " +
            "           OR DATA_TYPE = 'CHARACTER LARGE OBJECT' " +
            "           OR DATA_TYPE = 'BINARY' " +
            "           OR DATA_TYPE = 'BINARY VARYING' " +
            "           OR DATA_TYPE = 'BINARY LONG VARYING' " +
            "           OR DATA_TYPE = 'BINARY LARGE OBJECT' " +
            "           OR DATA_TYPE = 'BIT' " +
            "           OR DATA_TYPE = 'BIT VARYING' " +
            "           THEN CHARACTER_MAXIMUM_LENGTH " +
            "         WHEN DATA_TYPE = 'NUMBER' " +
            "           OR DATA_TYPE = 'NUMERIC' " +
            "           OR DATA_TYPE = 'DECIMAL' " +
            "           OR DATA_TYPE = 'FLOAT' " +
            "           OR DATA_TYPE = 'NATIVE_SMALLINT' " +
            "           OR DATA_TYPE = 'NATIVE_INTEGER' " +
            "           OR DATA_TYPE = 'NATIVE_BIGINT' " +
            "           OR DATA_TYPE = 'NATIVE_REAL' " +
            "           OR DATA_TYPE = 'NATIVE_DOUBLE' " +
            "           THEN NUMERIC_PRECISION " +
            "         WHEN DATA_TYPE = 'ROWID'   THEN 23 " +
            "         WHEN DATA_TYPE = 'BOOLEAN' THEN 5 " +
            "         WHEN DATA_TYPE = 'DATE'    THEN 10 " +
            "         WHEN DATA_TYPE = 'TIME WITHOUT TIME ZONE' THEN " +
            "           CASE " +
            "             WHEN DATETIME_PRECISION > 0 THEN 9 + DATETIME_PRECISION " +
            "             ELSE 8 " +
            "             END " +
            "         WHEN DATA_TYPE = 'TIMESTAMP WITHOUT TIME ZONE' THEN " +
            "           CASE " +
            "             WHEN DATETIME_PRECISION > 0 THEN 20 + DATETIME_PRECISION " +
            "             ELSE 19 " +
            "             END " +
            "         WHEN DATA_TYPE = 'TIME WITH TIME ZONE' THEN " +
            "           CASE " +
            "             WHEN DATETIME_PRECISION > 0 THEN 15 + DATETIME_PRECISION " +
            "             ELSE 14 " +
            "             END " +
            "         WHEN DATA_TYPE = 'TIMESTAMP WITH TIME ZONE' THEN " +
            "           CASE " +
            "             WHEN DATETIME_PRECISION > 0 THEN 26 + DATETIME_PRECISION " +
            "             ELSE 25 " +
            "             END " +
            "         WHEN DATA_TYPE = 'INTERVAL YEAR' " +
            "           OR DATA_TYPE = 'INTERVAL MONTH' THEN COLS.INTERVAL_PRECISION " +
            "         WHEN DATA_TYPE = 'INTERVAL YEAR TO MONTH' THEN 3 + COLS.INTERVAL_PRECISION " +
            "         WHEN DATA_TYPE = 'INTERVAL DAY' " +
            "           OR DATA_TYPE = 'INTERVAL HOUR' " +
            "           OR DATA_TYPE = 'INTERVAL MINUTE' THEN COLS.INTERVAL_PRECISION " +
            "         WHEN DATA_TYPE = 'INTERVAL SECOND' THEN " +
            "           CASE " +
            "           WHEN COLS.INTERVAL_PRECISION > 0 " +
            "             THEN COLS.INTERVAL_PRECISION + DATETIME_PRECISION + 1 " +
            "           ELSE COLS.INTERVAL_PRECISION " +
            "           END " +
            "         WHEN DATA_TYPE = 'INTERVAL DAY TO HOUR' THEN 3 + COLS.INTERVAL_PRECISION " +
            "         WHEN DATA_TYPE = 'INTERVAL DAY TO MINUTE' THEN 6 + COLS.INTERVAL_PRECISION " +
            "         WHEN DATA_TYPE = 'INTERVAL DAY TO SECOND' THEN " +
            "           CASE " +
            "           WHEN COLS.INTERVAL_PRECISION > 0 " +
            "             THEN 10 + COLS.INTERVAL_PRECISION + DATETIME_PRECISION " +
            "           ELSE 9 + COLS.INTERVAL_PRECISION " +
            "           END " +
            "         WHEN DATA_TYPE = 'INTERVAL HOUR TO MINUTE' THEN 3 + COLS.INTERVAL_PRECISION " +
            "         WHEN DATA_TYPE = 'INTERVAL HOUR TO SECOND' THEN " +
            "           CASE " +
            "           WHEN COLS.INTERVAL_PRECISION > 0 " +
            "             THEN 7 + COLS.INTERVAL_PRECISION + DATETIME_PRECISION " +
            "           ELSE 6 + COLS.INTERVAL_PRECISION " +
            "           END " +
            "         WHEN DATA_TYPE = 'INTERVAL MINUTE TO SECOND' THEN " +
            "           CASE " +
            "           WHEN COLS.INTERVAL_PRECISION > 0 " +
            "             THEN 4 + COLS.INTERVAL_PRECISION + DATETIME_PRECISION " +
            "           ELSE 3 + COLS.INTERVAL_PRECISION " +
            "           END " +
            "       END AS NATIVE_INTEGER) AS COLUMN_SIZE, " +
            "  CAST(CASE " +
            "         WHEN DATA_TYPE = 'CHARACTER' " +
            "           OR DATA_TYPE = 'CHARACTER VARYING' " +
            "           OR DATA_TYPE = 'CHARACTER LONG VARYING' " +
            "           OR DATA_TYPE = 'CHARACTER LARGE OBJECT' " +
            "           OR DATA_TYPE = 'BINARY' " +
            "           OR DATA_TYPE = 'BINARY VARYING' " +
            "           OR DATA_TYPE = 'BINARY LONG VARYING' " +
            "           OR DATA_TYPE = 'BINARY LARGE OBJECT' " +
            "           OR DATA_TYPE = 'BIT' " +
            "           OR DATA_TYPE = 'BIT VARYING' " +
            "           THEN CHARACTER_OCTET_LENGTH " +
            "         WHEN DATA_TYPE = 'DECIMAL' " +
            "           OR DATA_TYPE = 'NUMERIC' " +
            "           OR DATA_TYPE = 'NUMBER' " +
            "           THEN NUMERIC_PRECISION + 2 " +
            "         WHEN DATA_TYPE = 'BOOLEAN'         THEN 1 " +
            "         WHEN DATA_TYPE = 'NATIVE_SMALLINT' THEN 2 " +
            "         WHEN DATA_TYPE = 'NATIVE_INTEGER'  THEN 4 " +
            "         WHEN DATA_TYPE = 'NATIVE_BIGINT'   THEN 8 " +
            "         WHEN DATA_TYPE = 'NATIVE_REAL'     THEN 4 " +
            "         WHEN DATA_TYPE = 'FLOAT'           THEN 8 " +
            "         WHEN DATA_TYPE = 'NATIVE_DOUBLE'   THEN 8 " +
            "         WHEN DATA_TYPE = 'DATE' " +
            "           OR DATA_TYPE = 'TIME WITHOUT TIME ZONE' " +
            "           THEN 6 " +
            "         WHEN DATA_TYPE = 'TIME WITH TIME ZONE' " +
            "           OR DATA_TYPE = 'TIMESTAMP WITHOUT TIME ZONE' " +
            "           THEN 16 " +
            "         WHEN DATA_TYPE = 'TIMESTAMP WITH TIME ZONE' " +
            "           THEN 20 " +
            "         WHEN DATA_TYPE = 'INTERVAL YEAR TO MONTH' " +
            "           OR DATA_TYPE = 'INTERVAL YEAR' " +
            "           OR DATA_TYPE = 'INTERVAL MONTH' " +
            "           OR DATA_TYPE = 'INTERVAL YEAR TO MONTH' " +
            "           OR DATA_TYPE = 'INTERVAL DAY' " +
            "           OR DATA_TYPE = 'INTERVAL HOUR' " +
            "           OR DATA_TYPE = 'INTERVAL MINUTE' " +
            "           OR DATA_TYPE = 'INTERVAL SECOND' " +
            "           OR DATA_TYPE = 'INTERVAL DAY TO HOUR' " +
            "           OR DATA_TYPE = 'INTERVAL DAY TO MINUTE' " +
            "           OR DATA_TYPE = 'INTERVAL DAY TO SECOND'" +
            "           OR DATA_TYPE = 'INTERVAL HOUR TO MINUTE' " +
            "           OR DATA_TYPE = 'INTERVAL HOUR TO SECOND'" +
            "           OR DATA_TYPE = 'INTERVAL MINUTE TO SECOND' " +
            "           OR DATA_TYPE = 'INTERVAL DAY TO SECOND' " +
            "           THEN 34 " +
            "         WHEN DATA_TYPE = 'ROWID' " +
            "           THEN 23 " +
            "       END AS NATIVE_INTEGER) AS BUFFER_LENGTH, " +
            "  CAST(CASE " +
            "         WHEN DATA_TYPE = 'TIME WITHOUT TIME ZONE' " +
            "           OR DATA_TYPE = 'TIMESTAMP WITHOUT TIME ZONE' " +
            "           OR DATA_TYPE = 'TIME WITH TIME ZONE' " +
            "           OR DATA_TYPE = 'TIMESTAMP WITH TIME ZONE' " +
            "           THEN DATETIME_PRECISION " +
            "         WHEN DATA_TYPE = 'NUMERIC' " +
            "           OR DATA_TYPE = 'NUMBER' " +
            "           OR DATA_TYPE = 'DECIMAL' " +
            "           THEN NUMERIC_SCALE " +
            "         WHEN DATA_TYPE = 'NATIVE_SMALLINT' " +
            "           OR DATA_TYPE = 'NATIVE_INTEGER' " +
            "           OR DATA_TYPE = 'NATIVE_BIGINT' " +
            "           THEN 0 " +
            "         WHEN DATA_TYPE = 'INTERVAL SECOND' " +
            "           OR DATA_TYPE = 'INTERVAL DAY TO SECOND' " +
            "           OR DATA_TYPE = 'INTERVAL HOUR TO SECOND' " +
            "           OR DATA_TYPE = 'INTERVAL MINUTE TO SECOND' " +
            "           THEN DATETIME_PRECISION " +
            "         ELSE NULL " +
            "       END AS NATIVE_INTEGER) AS DECIMAL_DIGITS, " +
            "  CAST(NUMERIC_PRECISION_RADIX AS NATIVE_INTEGER) AS NUM_PREC_RADIX, " +
            "  CAST(CASE " +
            "         WHEN IS_NULLABLE = 'TRUE' THEN 1 " +
            "         ELSE 0 " +
            "         END AS NATIVE_INTEGER) AS NULLABLE, " +
            "  COMMENTS AS REMARKS, " +
            "  COLUMN_DEFAULT AS COLUMN_DEF, " +
            "  0 AS SQL_DATA_TYPE, " +
            "  0 AS SQL_DATETIME_SUB, " +
            "  CAST(CHARACTER_OCTET_LENGTH AS NATIVE_INTEGER) AS CHAR_OCTET_LENGTH, " +
            "  CAST(ORDINAL_POSITION AS NATIVE_INTEGER) AS ORDINAL_POSITION, " +
            "  CAST(CASE " +
            "         WHEN IS_NULLABLE = 'TRUE' THEN 'YES' " +
            "         ELSE 'NO' " +
            "         END AS VARCHAR(254 OCTETS)) AS IS_NULLABLE, " +
            "  SCOPE_CATALOG AS SCOPE_CATLOG, " +
            "  SCOPE_SCHEMA, " +
            "  SCOPE_NAME AS SCOPE_TABLE, " +
            "  NULL AS SOURCE_DATA_TYPE, " +
            "  CAST(CASE " +
            "         WHEN IS_IDENTITY = 'TRUE' THEN 'YES' " +
            "         ELSE 'NO' " +
            "         END AS VARCHAR(254 OCTETS)) AS IS_AUTOINCREMENT" +
            " FROM INFORMATION_SCHEMA.COLUMNS COLS, DEFINITION_SCHEMA.TYPE_INFO TYPEINFO WHERE COLS.DATA_TYPE = TYPEINFO.TYPE_NAME";
    private static final String INTERNAL_SQL_COLUMNS_ORDER = " ORDER BY TABLE_CAT, TABLE_SCHEM, TABLE_NAME, ORDINAL_POSITION";
    private static final String INTERNAL_SQL_PRODUCT_INFO = "SELECT NAME, VERSION, MAJOR_VERSION, MINOR_VERSION FROM DICTIONARY_SCHEMA.PRODUCT";
    private static final String INTERNAL_SQL_INDEX_INFO = "SELECT" +
            " TABLE_CATALOG AS TABLE_CAT, " +
            " TABLE_SCHEMA AS TABLE_SCHEM, " +
            " TABLE_NAME AS TABLE_NAME, " +
            " NON_UNIQUE AS NON_UNIQUE, " +
            " INDEX_SCHEMA AS INDEX_QUALIFIER, " + // JDBC spec에는 INDEX_CATALOG라 명시되어 있지만, 퀄리파이어로는 카탈로그가 사용될 수 없으므로 스키마를 쓴다.
            " INDEX_NAME AS INDEX_NAME, " +
            " CAST(CASE STAT_TYPE " +
            "         WHEN 'TABLE STAT'          THEN " + tableIndexStatistic +
            "         WHEN 'INDEX CLUSTERED'     THEN " + tableIndexClustered +
            "         WHEN 'INDEX HASHED'        THEN " + tableIndexHashed +
            "         WHEN 'INDEX OTHER'         THEN " + tableIndexOther +
            "         ELSE NULL " +
            "         END AS NATIVE_SMALLINT) AS TYPE, " +
            " ORDINAL_POSITION AS ORDINAL_POSITION, " +
            " COLUMN_NAME AS COLUMN_NAME, " +
            " CAST(CASE IS_ASCENDING_ORDER " +
            "         WHEN TRUE THEN 'A' " +
            "         WHEN FALSE THEN 'D' " +
            "         ELSE NULL " +
            "         END AS VARCHAR(32 OCTETS)) AS ASC_OR_DESC, " +
            " CARDINALITY AS CARDINALITY, " +
            " PAGES AS PAGES, " +
            " FILTER_CONDITION AS FILTER_CONDITION " +
            " FROM INFORMATION_SCHEMA.STATISTICS WHERE STAT_TYPE <> 'TABLE STAT' ";
    private static final String INTERNAL_SQL_INDEX_INFO_ORDER = " ORDER BY NON_UNIQUE, TYPE, INDEX_NAME, ORDINAL_POSITION";
    private static final String INTERNAL_SQL_IMPORTED_EXPORTED_KEYS = "SELECT" +
            " CAST(UNIQUE_CONSTRAINT_CATALOG     AS VARCHAR(128 OCTETS)) AS PKTABLE_CAT, " +
            " CAST(UNIQUE_CONSTRAINT_SCHEMA      AS VARCHAR(128 OCTETS)) AS PKTABLE_SCHEM, " +
            " CAST(UNIQUE_CONSTRAINT_TABLE_NAME  AS VARCHAR(128 OCTETS)) AS PKTABLE_NAME, " +
            " CAST(UNIQUE_CONSTRAINT_COLUMN_NAME AS VARCHAR(128 OCTETS)) AS PKCOLUMN_NAME, " +
            " CAST(CONSTRAINT_CATALOG            AS VARCHAR(128 OCTETS)) AS FKTABLE_CAT, " +
            " CAST(CONSTRAINT_SCHEMA             AS VARCHAR(128 OCTETS)) AS FKTABLE_SCHEM, " +
            " CAST(CONSTRAINT_TABLE_NAME         AS VARCHAR(128 OCTETS)) AS FKTABLE_NAME, " +
            " CAST(CONSTRAINT_COLUMN_NAME        AS VARCHAR(128 OCTETS)) AS FKCOLUMN_NAME, " +
            " CAST(ORDINAL_POSITION              AS NATIVE_SMALLINT)     AS KEY_SEQ, " +
            " CAST(CASE UPDATE_RULE " +
            "        WHEN 'CASCADE'     THEN " + importedKeyCascade +
            "        WHEN 'SET NULL'    THEN " + importedKeySetNull +
            "        WHEN 'NO ACTION'   THEN " + importedKeyNoAction +
            "        WHEN 'SET DEFAULT' THEN " + importedKeySetDefault +
            "        END                         AS NATIVE_SMALLINT)     AS UPDATE_RULE, " +
            " CAST(CASE DELETE_RULE " +
            "        WHEN 'CASCADE'     THEN " + importedKeyCascade +
            "        WHEN 'SET NULL'    THEN " + importedKeySetNull +
            "        WHEN 'NO ACTION'   THEN " + importedKeyNoAction +
            "        WHEN 'SET DEFAULT' THEN " + importedKeySetDefault +
            "        END AS NATIVE_SMALLINT) AS DELETE_RULE, " +
            "  CAST(CONSTRAINT_NAME               AS VARCHAR(128 OCTETS)) AS FK_NAME, " +
            "  CAST(UNIQUE_CONSTRAINT_NAME        AS VARCHAR(128 OCTETS)) AS PK_NAME, " +
            "  CAST(CASE WHEN IS_DEFERRABLE = FALSE THEN " + importedKeyNotDeferrable +
            "            ELSE " +
            "              CASE WHEN INITIALLY_DEFERRED = TRUE THEN " + importedKeyInitiallyDeferred +
            "                   ELSE " + importedKeyInitiallyImmediate +
            "                   END " +
            "            END                      AS SMALLINT)            AS DEFERRABILITY" +
            " FROM INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS WHERE IS_PRIMARY_KEY = TRUE";
    private static final String INTERNAL_SQL_IMPORTED_KEYS_ORDER = " ORDER BY PKTABLE_CAT, PKTABLE_SCHEM, PKTABLE_NAME, KEY_SEQ";
    private static final String INTERNAL_SQL_EXPORTED_KEYS_ORDER = " ORDER BY FKTABLE_CAT, FKTABLE_SCHEM, FKTABLE_NAME, KEY_SEQ";
    private static final String INTERNAL_SQL_PRIMARY_KEYS = "SELECT" +
            " CAST(K.TABLE_CATALOG    AS VARCHAR(128 OCTETS))    AS TABLE_CAT, " +
            " CAST(K.TABLE_SCHEMA     AS VARCHAR(128 OCTETS))    AS TABLE_SCHEM, " +
            " CAST(K.TABLE_NAME       AS VARCHAR(128 OCTETS))    AS TABLE_NAME, " +
            " CAST(K.COLUMN_NAME      AS VARCHAR(128 OCTETS))    AS COLUMN_NAME, " +
            " CAST(K.ORDINAL_POSITION AS NATIVE_SMALLINT) AS KEY_SEQ, " +
            " CAST(K.CONSTRAINT_NAME  AS VARCHAR(128 OCTETS))    AS PK_NAME " +
            " FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE  AS K, " +
            "      INFORMATION_SCHEMA.TABLE_CONSTRAINTS AS P " +
            " WHERE K.CONSTRAINT_CATALOG = P.CONSTRAINT_CATALOG AND " +
            "       K.CONSTRAINT_SCHEMA  = P.CONSTRAINT_SCHEMA AND " +
            "       K.CONSTRAINT_NAME    = P.CONSTRAINT_NAME AND " +
            "       P.CONSTRAINT_TYPE    = 'PRIMARY KEY' ";
    private static final String INTERNAL_SQL_PRIMARY_KEYS_ORDER = " ORDER BY COLUMN_NAME";
    private static final String INTERNAL_SQL_PRIMARY_KEYS_ORDER2 = " ORDER BY KEY_SEQ";
    
    private static final String INTERNAL_SQL_PROCEDURES = "SELECT" +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_CAT, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_SCHEM, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_NAME, " +
            " CAST(NULL AS NATIVE_INTEGER)      AS NUM_INPUT_PARAMS, " +
            " CAST(NULL AS NATIVE_INTEGER)      AS NUM_OUTPUT_PARAMS, " +
            " CAST(NULL AS NATIVE_INTEGER)      AS NUM_RESULT_SETS, " +
            " CAST(NULL AS VARCHAR(254 OCTETS)) AS REMARKS, " +
            " CAST(NULL AS NATIVE_SMALLINT)     AS PROCEDURE_TYPE, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS SPECIFIC_NAME " +
            " FROM DUAL WHERE 1=2 ";
    private static final String INTERNAL_SQL_BEST_ROW_IDENTIFIER = "SELECT" +
            " CAST(" + bestRowSession + " AS NATIVE_SMALLINT)  AS SCOPE, " +
            " CAST('ROWID'             AS VARCHAR(128 OCTETS)) AS COLUMN_NAME, " +
            " CAST(INFO.JDBC_DATA_TYPE AS NATIVE_SMALLINT)     AS DATA_TYPE, " +
            " CAST(INFO.TYPE_NAME      AS VARCHAR(128 OCTETS)) AS TYPE_NAME, " +
            " CAST(INFO.COLUMN_SIZE    AS NATIVE_INTEGER)      AS COLUMN_SIZE, " +
            " CAST(INFO.COLUMN_SIZE    AS NATIVE_INTEGER)      AS BUFFER_SIZE, " +
            " CAST(NULL                AS NATIVE_SMALLINT)     AS DECIMAL_DIGITS, " +
            " CAST(" + bestRowPseudo + " AS NATIVE_SMALLINT)   AS PSEUDO_COLUMN " +
            " FROM INFORMATION_SCHEMA.TABLES   AS TAB, " +
            "      DEFINITION_SCHEMA.TYPE_INFO AS INFO WHERE " +
            " INFO.TYPE_NAME = 'ROWID' ";
    private static final String INTERNAL_SQL_TYPE_INFO = "SELECT" +
            " TYPE_NAME," +
            " JDBC_DATA_TYPE AS DATA_TYPE," +
            " COLUMN_SIZE AS PRECISION," +
            " LITERAL_PREFIX," +
            " LITERAL_SUFFIX," +
            " CREATE_PARAMS," +
            " NULLABLE," +
            " CASE_SENSITIVE," +
            " SEARCHABLE," +
            " UNSIGNED_ATTRIBUTE," +
            " FIXED_PREC_SCALE," +
            " AUTO_UNIQUE_VALUE AS AUTO_INCREMENT," +
            " LOCAL_TYPE_NAME," +
            " MINIMUM_SCALE," +
            " MAXIMUM_SCALE," +
            " SQL_DATA_TYPE," +
            " SQL_DATETIME_SUB," +
            " NUM_PREC_RADIX" +
            " FROM DEFINITION_SCHEMA.TYPE_INFO ORDER BY JDBC_DATA_TYPE";
    private static final String INTERNAL_SQL_UDT_ATTRIBUTES = "SELECT" +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_CAT, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_SCHEM, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_NAME, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS ATTR_NAME, " +
            " CAST(NULL AS NATIVE_INTEGER) AS DATA_TYPE, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS ATTR_TYPE_NAME, " +
            " CAST(NULL AS NATIVE_INTEGER) AS ATTR_SIZE, " +
            " CAST(NULL AS NATIVE_INTEGER) AS DECIMAL_DIGITS, " +
            " CAST(NULL AS NATIVE_INTEGER) AS NUM_PREC_RADIX, " +
            " CAST(NULL AS NATIVE_INTEGER) AS NULLABLE, " +
            " CAST(NULL AS VARCHAR(254 OCTETS)) AS REMARKS, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS ATTR_DEF, " +
            " CAST(NULL AS NATIVE_INTEGER) AS SQL_DATA_TYPE, " +
            " CAST(NULL AS NATIVE_INTEGER) AS SQL_DATETIME_SUB, " +
            " CAST(NULL AS NATIVE_INTEGER) AS CHAR_OCTET_LENGTH, " +
            " CAST(NULL AS NATIVE_INTEGER) AS ORDINAL_POSITION, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS SCOPE_CATALOG, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS SCOPE_SCHEMA, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS SCOPE_TABLE, " +
            " CAST(NULL AS NATIVE_SMALLINT) AS SOURCE_DATA_TYPE " +
            " FROM DUAL WHERE 1=2 ";
    private static final String INTERNAL_SQL_COLUMN_PRIVILEGES = "SELECT" +
            "  CAST(TABLE_CATALOG  AS VARCHAR(128 OCTETS)) AS TABLE_CAT, " +
            "  CAST(TABLE_SCHEMA   AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, " +
            "  CAST(TABLE_NAME     AS VARCHAR(128 OCTETS)) AS TABLE_NAME, " +
            "  CAST(COLUMN_NAME    AS VARCHAR(128 OCTETS)) AS COLUMN_NAME, " +
            "  CAST(GRANTOR        AS VARCHAR(128 OCTETS)) AS GRANTOR, " +
            "  CAST(GRANTEE        AS VARCHAR(128 OCTETS)) AS GRANTEE, " +
            "  CAST(PRIVILEGE_TYPE AS VARCHAR(128 OCTETS)) AS PRIVILEGE, " +
            "  CAST(CASE IS_GRANTABLE " +
            "         WHEN TRUE  THEN 'YES' " +
            "         WHEN FALSE THEN 'NO' " +
            "         ELSE            'NULL' " +
            "         END          AS VARCHAR(3 OCTETS))   AS IS_GRANTABLE " +
            "FROM INFORMATION_SCHEMA.COLUMN_PRIVILEGES WHERE 1 = 1";
    private static final String INTERNAL_SQL_COLUMN_PRIVILEGES_ORDER =
            " ORDER BY COLUMN_NAME, PRIVILEGE ";
    private static final String INTERNAL_SQL_CROSS_REFERENCE = "SELECT" +
            "  CAST(UNIQUE_CONSTRAINT_CATALOG     AS VARCHAR(128 OCTETS)) AS PKTABLE_CAT, " +
            "  CAST(UNIQUE_CONSTRAINT_SCHEMA      AS VARCHAR(128 OCTETS)) AS PKTABLE_SCHEM, " +
            "  CAST(UNIQUE_CONSTRAINT_TABLE_NAME  AS VARCHAR(128 OCTETS)) AS PKTABLE_NAME, " +
            "  CAST(UNIQUE_CONSTRAINT_COLUMN_NAME AS VARCHAR(128 OCTETS)) AS PKCOLUMN_NAME, " +
            "  CAST(CONSTRAINT_CATALOG            AS VARCHAR(128 OCTETS)) AS FKTABLE_CAT, " +
            "  CAST(CONSTRAINT_SCHEMA             AS VARCHAR(128 OCTETS)) AS FKTABLE_SCHEM, " +
            "  CAST(CONSTRAINT_TABLE_NAME         AS VARCHAR(128 OCTETS)) AS FKTABLE_NAME, " +
            "  CAST(CONSTRAINT_COLUMN_NAME        AS VARCHAR(128 OCTETS)) AS FKCOLUMN_NAME, " +
            "  CAST(ORDINAL_POSITION              AS NATIVE_SMALLINT)     AS KEY_SEQ, " +
            "  CAST(CASE UPDATE_RULE " +
            "         WHEN 'CASCADE'     THEN " + importedKeyCascade +
            "         WHEN 'SET NULL'    THEN " + importedKeySetNull +
            "         WHEN 'NO ACTION'   THEN " + importedKeyNoAction +
            "         WHEN 'SET DEFAULT' THEN " + importedKeySetDefault +
            "         END                         AS NATIVE_SMALLINT)     AS UPDATE_RULE, " +
            "  CAST(CASE DELETE_RULE " +
            "         WHEN 'CASCADE'     THEN " + importedKeyCascade +
            "         WHEN 'SET NULL'    THEN " + importedKeySetNull +
            "         WHEN 'NO ACTION'   THEN " + importedKeyNoAction +
            "         WHEN 'SET DEFAULT' THEN " + importedKeySetDefault +
            "         END                         AS NATIVE_SMALLINT)     AS DELETE_RULE, " +
            "  CAST(CONSTRAINT_NAME               AS VARCHAR(128 OCTETS)) AS FK_NAME, " +
            "  CAST(UNIQUE_CONSTRAINT_NAME        AS VARCHAR(128 OCTETS)) AS PK_NAME, " +
            "  CAST(CASE WHEN IS_DEFERRABLE = FALSE THEN " + importedKeyNotDeferrable +
            "            ELSE " +
            "              CASE WHEN INITIALLY_DEFERRED = TRUE THEN " + importedKeyInitiallyDeferred +
            "                   ELSE " + importedKeyInitiallyImmediate +
            "                   END " +
            "            END                      AS SMALLINT)            AS DEFERRABILITY " +
            "FROM INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS WHERE 1=1";
    private static final String INTERNAL_SQL_CROSS_REFERENCE_ORDER =
            " ORDER BY FKTABLE_CAT, FKTABLE_SCHEM, FKTABLE_NAME, KEY_SEQ";
    private static final String INTERNAL_SQL_DEFAULT_TX_LEVEL =
            "SELECT INTEGER_VALUE FROM DEFINITION_SCHEMA.SQL_IMPLEMENTATION_INFO WHERE IMPLEMENTATION_INFO_ID = 26";
    private static final String INTERNAL_SQL_SERVER_INFO = "SELECT " +
            " IMPLEMENTATION_INFO_ID, INTEGER_VALUE, CHARACTER_VALUE " +
            " FROM DICTIONARY_SCHEMA.IMPLEMENTATION_INFO";
    private static final String INTERNAL_SQL_NUMERIC_FUNCTIONS = "SELECT" +
            " SUBSTR(SUB_NAME, 12)" +
            " FROM DICTIONARY_SCHEMA.IMPLEMENTATION_INFO_BASE" +
            " WHERE ID='49' AND SUB_ID <> ' '";
    private static final String INTERNAL_SQL_STRING_FUNCTIONS = "SELECT" +
            " SUBSTR(SUB_NAME, 12)" +
            " FROM DICTIONARY_SCHEMA.IMPLEMENTATION_INFO_BASE" +
            " WHERE ID='50' AND SUB_ID <> ' '";
    private static final String INTERNAL_SQL_SYSTEM_FUNCTIONS = "SELECT" +
            " SUBSTR(SUB_NAME, 12)" +
            " FROM DICTIONARY_SCHEMA.IMPLEMENTATION_INFO_BASE" +
            " WHERE ID='51' AND SUB_ID <> ' '";
    private static final String INTERNAL_SQL_TIMEDATE_FUNCTIONS = "SELECT" +
            " SUBSTR(SUB_NAME, 11)" +
            " FROM DICTIONARY_SCHEMA.IMPLEMENTATION_INFO_BASE" +
            " WHERE ID='52' AND SUB_ID <> ' '";
    private static final String INTERNAL_SQL_PROCEDURE_COLUMNS = "SELECT" +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_CAT, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_SCHEM, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_NAME, " +
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
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS COLUMN_DEF, " +
            " CAST(NULL AS NATIVE_INTEGER) AS SQL_DATA_TYPE, " +
            " CAST(NULL AS NATIVE_INTEGER) AS SQL_DATETIME_SUB, " +
            " CAST(NULL AS NATIVE_INTEGER) AS CHAR_OCTET_LENGTH, " +
            " CAST(NULL AS NATIVE_INTEGER) AS ORDINAL_POSITION, " +
            " CAST(NULL AS VARCHAR(3 OCTETS)) AS IS_NULLABLE, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS SPECIFIC_NAME " +
            " FROM DUAL WHERE 1=2 ";
    private static final String INTERNAL_SQL_SUPER_TABLES = "SELECT" +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TABLE_CAT, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TABLE_NAME, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS SUPERTABLE_NAME " +
            " FROM DUAL WHERE 1=2 ";
    private static final String INTERNAL_SQL_SUPER_TYPES = "SELECT" +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_CAT, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_SCHEM, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_NAME, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS SUPERTYPE_CAT, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS SUPERTYPE_SCHEM, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS SUPERTYPE_NAME " +
            " FROM DUAL WHERE 1=2 ";
    private static final String INTERNAL_SQL_TABLE_PRIVILEGES = "SELECT " +
            "  CAST(TABLE_CATALOG  AS VARCHAR(128 OCTETS)) AS TABLE_CAT, " +
            "  CAST(TABLE_SCHEMA   AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, " +
            "  CAST(TABLE_NAME     AS VARCHAR(128 OCTETS)) AS TABLE_NAME, " +
            "  CAST(GRANTOR        AS VARCHAR(128 OCTETS)) AS GRANTOR, " +
            "  CAST(GRANTEE        AS VARCHAR(128 OCTETS)) AS GRANTEE, " +
            "  CAST(PRIVILEGE_TYPE AS VARCHAR(128 OCTETS)) AS PRIVILEGE, " +
            "  CAST(CASE IS_GRANTABLE " +
            "         WHEN TRUE  THEN 'YES' " +
            "         WHEN FALSE THEN 'NO' " +
            "         ELSE            'NULL' " +
            "         END          AS VARCHAR(3 OCTETS))   AS IS_GRANTABLE " +
            "FROM INFORMATION_SCHEMA.TABLE_PRIVILEGES WHERE 1 = 1";
    private static final String INTERNAL_SQL_TABLE_PRIVILEGES_ORDER =
            " ORDER BY TABLE_CAT, TABLE_SCHEM, TABLE_NAME, PRIVILEGE";
    private static final String INTERNAL_SQL_UDTS = "SELECT" +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_CAT, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_SCHEM, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_NAME, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS CLASS_NAME, " +
            " CAST(NULL AS NATIVE_INTEGER) AS DATA_TYPE, " +
            " CAST(NULL AS VARCHAR(256 OCTETS)) AS REMARKS, " +
            " CAST(NULL AS NATIVE_SMALLINT) AS BASE_TYPE " +
            " FROM DUAL WHERE 1=2 ";
    private static final String INTERNAL_SQL_VERSION_COLUMNS = "SELECT" +
            " CAST(NULL AS NATIVE_SMALLINT) AS SCOPE, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS COLUMN_NAME, " +
            " CAST(NULL AS NATIVE_INTEGER) AS DATA_TYPE, " +
            " CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_NAME, " +
            " CAST(NULL AS NATIVE_INTEGER) AS COLUMN_SIZE, " +
            " CAST(NULL AS NATIVE_INTEGER) AS BUFFER_LENGTH, " +
            " CAST(NULL AS NATIVE_SMALLINT) AS DECIMAL_DIGITS, " +
            " CAST(NULL AS NATIVE_SMALLINT) AS PSEUDO_COLUMN " +
            " FROM DUAL WHERE 1=2 ";
    
    private static String getServerInfo(Jdbc4DatabaseMetaData aCaller, String aInfoId) throws SQLException
    {
        if (GLOBAL_INFO_TABLE == null)
        {
            synchronized (LOCK_FOR_GLOBAL_INFO)
            {
                // 동시성 보장을 위해 lock을 획득한 후 다시 검사해야 한다.
                if (GLOBAL_INFO_TABLE == null)
                {
                    GLOBAL_INFO_TABLE = new HashMap<String, String>();
                    ResultSet sRs = aCaller.mConnection.executeInternalQuery(INTERNAL_SQL_SERVER_INFO);
                    while (sRs.next())
                    {
                        String sId = sRs.getString(1);
                        int sIntValue = sRs.getInt(2);
                        if (sRs.wasNull())
                        {
                            GLOBAL_INFO_TABLE.put(sId, sRs.getString(3));
                        }
                        else
                        {
                            GLOBAL_INFO_TABLE.put(sId, String.valueOf(sIntValue));                        
                        }
                    }
                }
            }
        }
        
        // HashMap의 표준 문서에 의하면, HashMap의 get 메소드만 쓸 때는 동시성 문제가 없다.
        return GLOBAL_INFO_TABLE.get(aInfoId);
    }
    
    private static int getIntServerInfo(Jdbc4DatabaseMetaData aCaller, String aInfoId) throws SQLException
    {
        try
        {
            return Integer.parseInt(getServerInfo(aCaller, aInfoId));
        }
        catch (NumberFormatException sException)
        {
            ErrorMgr.raiseRuntimeError("Invalid server info type: the value is not a number");
            return -1;
        }
    }
    
    private static String convertToCommaSeparatedResult(ResultSet sRs) throws SQLException
    {
        StringBuffer sBuf = new StringBuffer();
        
        if (sRs.next())
        {
            sBuf.append(sRs.getString(1));
            while (sRs.next())
            {
                sBuf.append(',').append(sRs.getString(1));
            }
        }
        sRs.close();
        return sBuf.toString();
    }
    
    protected Jdbc4Connection mConnection;
    protected Logger mLogger;
    protected String mProductName;
    protected String mProductVersion;
    protected int mProductMajorVersion;
    protected int mProductMinorVersion;
    protected boolean mProductInfoSet = false;
    
    public Jdbc4DatabaseMetaData(Jdbc4Connection aConnection)
    {
        mConnection = aConnection;
        mLogger = aConnection.mLogger;
    }
    
    private synchronized void prepareProductInfo() throws SQLException
    {
        if (!mProductInfoSet)
        {
            ResultSet sRs = mConnection.executeInternalQuery(INTERNAL_SQL_PRODUCT_INFO);
            sRs.next();
            mProductName = sRs.getString("NAME");
            mProductVersion = sRs.getString("VERSION");
            mProductMajorVersion = sRs.getShort("MAJOR_VERSION");
            mProductMinorVersion = sRs.getShort("MINOR_VERSION");
            mProductInfoSet = true;
            sRs.close();
        }
    }
    
    public boolean allProceduresAreCallable() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean allTablesAreSelectable() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean dataDefinitionCausesTransactionCommit() throws SQLException
    {
        mLogger.logTrace();
        // DDL 구문 수행시 자동으로 commit 되게 하지 않는다.
        return false;
    }

    public boolean dataDefinitionIgnoredInTransactions() throws SQLException
    {
        mLogger.logTrace();
        // DDL도 transaction에 포함된다.
        return false;
    }

    public boolean deletesAreDetected(int aResultSetType) throws SQLException
    {
        mLogger.logTrace();
        return (aResultSetType == ResultSet.TYPE_SCROLL_SENSITIVE);
    }

    public boolean doesMaxRowSizeIncludeBlobs() throws SQLException
    {
        mLogger.logTrace();
        return false;
    }

    public ResultSet getAttributes(String aCatalog, String aSchemaPattern,
            String aTypeNamePattern, String aAttributeNamePattern) throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_UDT_ATTRIBUTES);
    }

    public ResultSet getBestRowIdentifier(String aCatalog, String aSchema, String aTable, int aScope, boolean aNullable) throws SQLException
    {
        mLogger.logTrace();
        if (aTable == null)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "table name should not be null");
        }
        
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_BEST_ROW_IDENTIFIER);
        if (aCatalog != null)
        {
            sSql.append(" AND TAB.TABLE_CATALOG = '").append(aCatalog).append('\'');
        }
        if (aSchema != null)
        {
            sSql.append(" AND TAB.TABLE_SCHEMA = '").append(aSchema).append('\'');
        }
        sSql.append(" AND TAB.TABLE_NAME = '").append(aTable).append('\'')
            .append(" AND ").append(bestRowSession).append(" = ").append(aScope);
        return mConnection.executeInternalQuery(sSql.toString());
    }

    public String getCatalogSeparator() throws SQLException
    {
        mLogger.logTrace();
        return getServerInfo(this, SERVER_INFO_ID_CATALOG_NAME_SEPARATOR);
    }

    public String getCatalogTerm() throws SQLException
    {
        mLogger.logTrace();
        return getServerInfo(this, SERVER_INFO_ID_CATALOG_TERM);
    }

    public ResultSet getCatalogs() throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_CATALOGS);
    }

    public ResultSet getColumnPrivileges(String aCatalog, String aSchema, String aTable, String aColumnNamePattern) throws SQLException
    {
        mLogger.logTrace();
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_COLUMN_PRIVILEGES);
        if (aCatalog != null)
        {
            sSql.append(" AND TABLE_CATALOG = '").append(aCatalog).append('\'');
        }
        if (aSchema != null)
        {
            sSql.append(" AND TABLE_SCHEMA = '").append(aSchema).append('\'');
        }
        if (aTable != null)
        {
            sSql.append(" AND TABLE_NAME = '").append(aTable).append('\'');
        }
        if (aColumnNamePattern != null)
        {
            sSql.append(" AND COLUMN_NAME LIKE '").append(aColumnNamePattern).append('\'');
        }
        sSql.append(INTERNAL_SQL_COLUMN_PRIVILEGES_ORDER);
        return mConnection.executeInternalQuery(sSql.toString());
    }

    public ResultSet getColumns(String aCatalog, String aSchemaPattern, String aTableNamePattern, String aColumnNamePattern)
            throws SQLException
    {
        mLogger.logTrace();
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_COLUMNS);
        if (aCatalog != null)
        {
            sSql.append(" AND TABLE_CATALOG = '").append(aCatalog).append('\'');
        }
        if (aSchemaPattern != null)
        {
            sSql.append(" AND TABLE_SCHEMA LIKE '").append(aSchemaPattern).append('\'');
        }
        if (aTableNamePattern != null)
        {
            sSql.append(" AND TABLE_NAME LIKE '").append(aTableNamePattern).append('\'');
        }
        if (aColumnNamePattern != null)
        {
            sSql.append(" AND COLUMN_NAME LIKE '").append(aColumnNamePattern).append('\'');
        }

        sSql.append(INTERNAL_SQL_COLUMNS_ORDER);
        return mConnection.executeInternalQuery(sSql.toString());
    }

    public Connection getConnection() throws SQLException
    {
        mLogger.logTrace();
        return mConnection;
    }

    public ResultSet getCrossReference(String aParentCatalog, String aParentSchema, String aParentTable,
            String aForeignCatalog, String aForeignSchema, String aForeignTable) throws SQLException
    {
        mLogger.logTrace();
        
        if (aParentTable == null)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "parent table name should not be null");
        }
        if (aForeignTable == null)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "foreign table name should not be null");
        }
        
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_CROSS_REFERENCE);
        if (aParentCatalog != null)
        {
            sSql.append(" AND UNIQUE_CONSTRAINT_CATALOG = '").append(aParentCatalog).append('\'');
        }
        if (aParentSchema != null)
        {
            sSql.append(" AND UNIQUE_CONSTRAINT_SCHEMA = '").append(aParentSchema).append('\'');
        }
        sSql.append(" AND UNIQUE_CONSTRAINT_TABLE_NAME = '").append(aParentTable).append('\'');
        if (aForeignCatalog != null)
        {
            sSql.append(" AND CONSTRAINT_CATALOG = '").append(aForeignCatalog).append('\'');
        }
        if (aForeignSchema != null)
        {
            sSql.append(" AND CONSTRAINT_SCHEMA = '").append(aForeignSchema).append('\'');
        }
        sSql.append(" AND CONSTRAINT_TABLE_NAME = '").append(aForeignTable).append("' ").append(INTERNAL_SQL_CROSS_REFERENCE_ORDER);
        return mConnection.executeInternalQuery(sSql.toString());
    }

    public int getDatabaseMajorVersion() throws SQLException
    {
        mLogger.logTrace();
        prepareProductInfo();
        return mProductMajorVersion;
    }

    public int getDatabaseMinorVersion() throws SQLException
    {
        mLogger.logTrace();
        prepareProductInfo();
        return mProductMinorVersion;
    }

    public String getDatabaseProductName() throws SQLException
    {
        mLogger.logTrace();
        prepareProductInfo();
        return mProductName;
    }

    public String getDatabaseProductVersion() throws SQLException
    {
        mLogger.logTrace();
        prepareProductInfo();
        return mProductVersion;
    }

    public int getDefaultTransactionIsolation() throws SQLException
    {
        mLogger.logTrace();
        ResultSet sRs = mConnection.executeInternalQuery(INTERNAL_SQL_DEFAULT_TX_LEVEL);
        int sIsolationLevel = Jdbc4Connection.TX_ISOLATION_NONE;
        if (sRs.next())
        {
            sIsolationLevel = sRs.getInt(1);
        }
        sRs.close();
        return Jdbc4Connection.isolationLevelGoldilocksToJdbc(sIsolationLevel);                
    }

    public int getDriverMajorVersion()
    {
        mLogger.logTrace();
        return Jdbc4Driver.GOLDILOCKS_DRIVER_MAJOR_VERSION;
    }

    public int getDriverMinorVersion()
    {
        mLogger.logTrace();        
        return Jdbc4Driver.GOLDILOCKS_DRIVER_MINOR_VERSION;
    }

    public String getDriverName() throws SQLException
    {
        mLogger.logTrace();
        return Jdbc4Driver.GOLDILOCKS_JDBC_DRIVER_NAME;
    }

    public String getDriverVersion() throws SQLException
    {
        mLogger.logTrace();
        return Jdbc4Driver.GOLDILOCKS_JDBC_DRIVER_VERSION;
    }

    public ResultSet getExportedKeys(String aCatalog, String aSchema, String aTable)
            throws SQLException
    {
        mLogger.logTrace();
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_IMPORTED_EXPORTED_KEYS);
        
        if (aCatalog != null)
        {
            sSql.append(" AND UNIQUE_CONSTRAINT_CATALOG = '").append(aCatalog).append('\'');
        }
        if (aSchema != null)
        {
            sSql.append(" AND UNIQUE_CONSTRAINT_SCHEMA = '").append(aSchema).append('\'');
        }
        if (aTable != null)
        {
            sSql.append(" AND UNIQUE_CONSTRAINT_TABLE_NAME = '").append(aTable).append('\'');
        }
        sSql.append(INTERNAL_SQL_EXPORTED_KEYS_ORDER);
        return mConnection.executeInternalQuery(sSql.toString());
    }

    public String getExtraNameCharacters() throws SQLException
    {
        mLogger.logTrace();
        return SERVER_STATIC_INFO_IDENTIFIER_EXTRA_CHARACTERS;
    }

    public String getIdentifierQuoteString() throws SQLException
    {
        mLogger.logTrace();
        return getServerInfo(this, SERVER_INFO_ID_IDENTIFIER_QUOTE_CHAR);
    }

    public ResultSet getImportedKeys(String aCatalog, String aSchema, String aTable)
            throws SQLException
    {
        mLogger.logTrace();
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_IMPORTED_EXPORTED_KEYS);
        
        if (aCatalog != null)
        {
            sSql.append(" AND CONSTRAINT_CATALOG = '").append(aCatalog).append('\'');
        }
        if (aSchema != null)
        {
            sSql.append(" AND CONSTRAINT_SCHEMA = '").append(aSchema).append('\'');
        }
        if (aTable != null)
        {
            sSql.append(" AND CONSTRAINT_TABLE_NAME = '").append(aTable).append('\'');
        }
        sSql.append(INTERNAL_SQL_IMPORTED_KEYS_ORDER);
        return mConnection.executeInternalQuery(sSql.toString());
    }

    public ResultSet getIndexInfo(String aCatalog, String aSchema, String aTable,
            boolean aUnique, boolean aApproximate) throws SQLException
    {
        mLogger.logTrace();
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_INDEX_INFO);
        
        if (aCatalog != null)
        {
            sSql.append(" AND TABLE_CATALOG = '").append(aCatalog).append('\'');
        }
        if (aSchema != null)
        {
            sSql.append(" AND TABLE_SCHEMA = '").append(aSchema).append('\'');
        }
        if (aTable != null)
        {
            sSql.append(" AND TABLE_NAME = '").append(aTable).append('\'');
        }
        if (aUnique)
        {
            sSql.append(" AND NON_UNIQUE = FALSE");
        }
        sSql.append(INTERNAL_SQL_INDEX_INFO_ORDER);
        return mConnection.executeInternalQuery(sSql.toString());
    }

    public int getMaxBinaryLiteralLength() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_BINARY_LITERAL_LENGTH);
    }

    public int getMaxCatalogNameLength() throws SQLException
    {
        mLogger.logTrace();        
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_CATALOG_NAME_LENGTH);
    }

    public int getMaxCharLiteralLength() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_CHAR_LITERAL_LENGTH);
    }

    public int getMaxColumnNameLength() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_COLUMN_NAME_LENGTH);
    }

    public int getMaxColumnsInGroupBy() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_COLUMNS_IN_GROUP_BY);
    }

    public int getMaxColumnsInIndex() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_COLUMNS_IN_INDEX);
    }

    public int getMaxColumnsInOrderBy() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_COLUMNS_IN_ORDER_BY);
    }

    public int getMaxColumnsInSelect() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_COLUMNS_IN_SELECT);
    }

    public int getMaxColumnsInTable() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_COLUMNS_IN_TABLE);
    }

    public int getMaxConnections() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_CONNECTIONS);
    }

    public int getMaxCursorNameLength() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_CURSOR_NAME_LENGTH);
    }

    public int getMaxIndexLength() throws SQLException
    {
        mLogger.logTrace();
        /*
         *  JDBC 표준 문서에는 
         *  "Retrieves the maximum number of bytes this database allows for an index, including all of the parts of the index."
         *  라고 나와 있다. 즉 인덱스 전체의 크기를 의미하는 것 같은데,
         *  이 값은 별로 의미가 없고, ODBC 처럼 field 하나의 크기를 의미하는 값이 더 필요할 것으로 판단되어
         *  ODBC와 같은 값을 반환하도록 구현하였다.
         */
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_INDEX_LENGTH);
    }

    public int getMaxProcedureNameLength() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_PROCEDURE_NAME_LENGTH);
    }

    public int getMaxRowSize() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_ROW_SIZE);
    }

    public int getMaxSchemaNameLength() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_SCHEMA_NAME_LENGTH);
    }

    public int getMaxStatementLength() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_STATEMENT_LENGTH);
    }

    public int getMaxStatements() throws SQLException
    {
        mLogger.logTrace();
        // statement 개수 제한은 없다. SERVER_INFO에 기술되어 있지 않다.
        return 0;
    }

    public int getMaxTableNameLength() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_TABLE_NAME_LENGTH);
    }

    public int getMaxTablesInSelect() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_TABLES_IN_SELECT);
    }

    public int getMaxUserNameLength() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_MAX_USER_NAME_LENGTH);
    }

    public String getNumericFunctions() throws SQLException
    {
        mLogger.logTrace();
        return convertToCommaSeparatedResult(mConnection.executeInternalQuery(INTERNAL_SQL_NUMERIC_FUNCTIONS));
    }

    public ResultSet getPrimaryKeys(String aCatalog, String aSchema, String aTable)
            throws SQLException
    {
        mLogger.logTrace();
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_PRIMARY_KEYS);
        
        if (aCatalog != null)
        {
            sSql.append(" AND K.TABLE_CATALOG = '").append(aCatalog).append('\'');
        }
        if (aSchema != null)
        {
            sSql.append(" AND K.TABLE_SCHEMA = '").append(aSchema).append('\'');
        }
        if (aTable != null)
        {
            sSql.append(" AND K.TABLE_NAME = '").append(aTable).append('\'');
        }
        sSql.append(INTERNAL_SQL_PRIMARY_KEYS_ORDER);
        return mConnection.executeInternalQuery(sSql.toString());
    }

    public ResultSet getPrimaryKeysOrderByKeySeq(String aCatalog, String aSchema, String aTable)
            throws SQLException
    {
        mLogger.logTrace();
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_PRIMARY_KEYS);
        
        if (aCatalog != null)
        {
            sSql.append(" AND K.TABLE_CATALOG = '").append(aCatalog).append('\'');
        }
        if (aSchema != null)
        {
            sSql.append(" AND K.TABLE_SCHEMA = '").append(aSchema).append('\'');
        }
        if (aTable != null)
        {
            sSql.append(" AND K.TABLE_NAME = '").append(aTable).append('\'');
        }
        sSql.append(INTERNAL_SQL_PRIMARY_KEYS_ORDER2);
        return mConnection.executeInternalQuery(sSql.toString());
    }
    
    public ResultSet getProcedureColumns(String aCatalog, String aSchemaPattern, String aProcedureNamePattern, String aColumnNamePattern) throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_PROCEDURE_COLUMNS);
    }

    public String getProcedureTerm() throws SQLException
    {
        mLogger.logTrace();
        return getServerInfo(this, SERVER_INFO_ID_PROCEDURE_TERM);
    }

    public ResultSet getProcedures(String aCatalog, String aSchemaPattern, String aProcedureNamePattern) throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_PROCEDURES);
    }

    public int getResultSetHoldability() throws SQLException
    {
        mLogger.logTrace();
        return Jdbc4Connection.GOLDILOCKS_DEFAULT_RESULT_SET_HOLDABILITY;
    }

    public String getSQLKeywords() throws SQLException
    {
        mLogger.logTrace();
        return getServerInfo(this, SERVER_INFO_ID_SQL_KEYWORDS);
    }

    public int getSQLStateType() throws SQLException
    {
        mLogger.logTrace();
        return sqlStateSQL99;
    }

    public String getSchemaTerm() throws SQLException
    {
        mLogger.logTrace();
        return getServerInfo(this, SERVER_INFO_ID_SCHEMA_TERM);
    }

    public ResultSet getSchemas() throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_SCHEMAS + INTERNAL_SQL_SCHEMAS_ORDER);        
    }

    public String getSearchStringEscape() throws SQLException
    {
        mLogger.logTrace();
        return getServerInfo(this, SERVER_INFO_ID_SEARCH_STRING_ESCAPE);
    }

    public String getStringFunctions() throws SQLException
    {
        mLogger.logTrace();
        return convertToCommaSeparatedResult(mConnection.executeInternalQuery(INTERNAL_SQL_STRING_FUNCTIONS));
    }

    public ResultSet getSuperTables(String aCatalog, String aSchemaPattern, String aTableNamePattern) throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_SUPER_TABLES);
    }

    public ResultSet getSuperTypes(String aCatalog, String aSchemaPattern, String aTypeNamePattern) throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_SUPER_TYPES);
    }

    public String getSystemFunctions() throws SQLException
    {
        mLogger.logTrace();
        return convertToCommaSeparatedResult(mConnection.executeInternalQuery(INTERNAL_SQL_SYSTEM_FUNCTIONS));
    }

    public ResultSet getTablePrivileges(String aCatalog, String aSchemaPattern, String aTableNamePattern) throws SQLException
    {
        mLogger.logTrace();
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_TABLE_PRIVILEGES);
        if (aCatalog != null)
        {
            sSql.append(" AND TABLE_CATALOG = '").append(aCatalog).append('\'');
        }
        if (aSchemaPattern != null)
        {
            sSql.append(" AND TABLE_SCHEMA LIKE '").append(aSchemaPattern).append('\'');
        }
        if (aTableNamePattern != null)
        {
            sSql.append(" AND TABLE_NAME LIKE '").append(aTableNamePattern).append('\'');
        }
        sSql.append(INTERNAL_SQL_TABLE_PRIVILEGES_ORDER);
        return mConnection.executeInternalQuery(sSql.toString());
    }

    public ResultSet getTableTypes() throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_TABLE_TYPES);
    }

    public ResultSet getTables(String aCatalog, String aSchemaPattern, String aTableNamePattern, String[] aTypes) throws SQLException
    {
        mLogger.logTrace();
        StringBuffer sSql = new StringBuffer(INTERNAL_SQL_TABLES);
        
        if (aCatalog != null)
        {
            sSql.append(" AND TABLE_CATALOG = '").append(aCatalog).append('\'');
        }
        if (aSchemaPattern != null)
        {
            sSql.append(" AND TABLE_SCHEMA LIKE '").append(aSchemaPattern).append('\'');
        }
        if (aTableNamePattern != null)
        {
            sSql.append(" AND TABLE_NAME LIKE '").append(aTableNamePattern).append('\'');
        }
        if (aTypes != null)
        {
            sSql.append(" AND (DBC_TABLE_TYPE = '").append(aTypes[0]).append('\'');
            for (int i=1; i<aTypes.length; i++)
            {
                sSql.append(" OR DBC_TABLE_TYPE = '").append(aTypes[i]).append('\'');
            }
            sSql.append(')');
        }
        
        sSql.append(INTERNAL_SQL_TABLES_ORDER);
        return mConnection.executeInternalQuery(sSql.toString());
    }

    public String getTimeDateFunctions() throws SQLException
    {
        mLogger.logTrace();
        return convertToCommaSeparatedResult(mConnection.executeInternalQuery(INTERNAL_SQL_TIMEDATE_FUNCTIONS));
    }

    public ResultSet getTypeInfo() throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_TYPE_INFO);
    }

    public ResultSet getUDTs(String aCatalog, String aSchemaPattern, String aTypeNamePattern, int[] aTypes) throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_UDTS);
    }

    public String getURL() throws SQLException
    {
        mLogger.logTrace();
        return Jdbc4Driver.makeURL(mConnection.getHost(), mConnection.getPort(), mConnection.getDBName());
    }

    public String getUserName() throws SQLException
    {
        mLogger.logTrace();
        return mConnection.getValueByQuery(INTERNAL_SQL_CURRENT_USER);
    }

    public ResultSet getVersionColumns(String aCatalog, String aSchema, String aTable) throws SQLException
    {
        mLogger.logTrace();
        return mConnection.executeInternalQuery(INTERNAL_SQL_VERSION_COLUMNS);
    }

    public boolean insertsAreDetected(int aType) throws SQLException
    {
        mLogger.logTrace();
        return false;
    }

    public boolean isCatalogAtStart() throws SQLException
    {
        mLogger.logTrace();
        // fully qualified table name에 catalog 이름은 붙지 않는다.
        return false;
    }

    public boolean isReadOnly() throws SQLException
    {
        mLogger.logTrace();
        return mConnection.isReadOnly();
    }

    public boolean locatorsUpdateCopy() throws SQLException
    {
        mLogger.logTrace();
        // lob을 지원하지 않으므로 false를...
        return false;
    }

    public boolean nullPlusNonNullIsNull() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean nullsAreSortedAtEnd() throws SQLException
    {
        mLogger.logTrace();
        // null만 따로 정렬하진 않는다.
        return false;
    }

    public boolean nullsAreSortedAtStart() throws SQLException
    {
        mLogger.logTrace();
        // null만 따로 정렬하진 않는다.
        return false;
    }

    public boolean nullsAreSortedHigh() throws SQLException
    {
        mLogger.logTrace();
        // default로 null은 last에 위치한다.
        return true;
    }

    public boolean nullsAreSortedLow() throws SQLException
    {
        mLogger.logTrace();
        // default로 null은 last에 위치한다.
        return false;
    }

    public boolean othersDeletesAreVisible(int aType) throws SQLException
    {
        mLogger.logTrace();
        return aType == ResultSet.TYPE_SCROLL_SENSITIVE;
    }

    public boolean othersInsertsAreVisible(int aType) throws SQLException
    {
        mLogger.logTrace();
        // insert는 볼 수 없다.
        return false;
    }

    public boolean othersUpdatesAreVisible(int aType) throws SQLException
    {
        mLogger.logTrace();
        return aType == ResultSet.TYPE_SCROLL_SENSITIVE;
    }

    public boolean ownDeletesAreVisible(int aType) throws SQLException
    {
        mLogger.logTrace();
        return aType == ResultSet.TYPE_SCROLL_SENSITIVE;
    }

    public boolean ownInsertsAreVisible(int aType) throws SQLException
    {
        mLogger.logTrace();
        // insert는 볼 수 없다.
        return false;
    }

    public boolean ownUpdatesAreVisible(int aType) throws SQLException
    {
        mLogger.logTrace();
        return aType == ResultSet.TYPE_SCROLL_SENSITIVE;
    }

    public boolean storesLowerCaseIdentifiers() throws SQLException
    {
        mLogger.logTrace();
        return false;
    }

    public boolean storesLowerCaseQuotedIdentifiers() throws SQLException
    {
        mLogger.logTrace();
        return false;
    }

    public boolean storesMixedCaseIdentifiers() throws SQLException
    {
        mLogger.logTrace();
        return false;
    }

    public boolean storesMixedCaseQuotedIdentifiers() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean storesUpperCaseIdentifiers() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean storesUpperCaseQuotedIdentifiers() throws SQLException
    {
        mLogger.logTrace();
        return false;
    }

    public boolean supportsANSI92EntryLevelSQL() throws SQLException
    {
        mLogger.logTrace();
        // 안된다고 봐야 한다.
        return false;
    }

    public boolean supportsANSI92FullSQL() throws SQLException
    {
        mLogger.logTrace();
        // 안된다고 봐야 한다.
        return false;
    }

    public boolean supportsANSI92IntermediateSQL() throws SQLException
    {
        mLogger.logTrace();
        // 안된다고 봐야 한다.
        return false;
    }

    public boolean supportsAlterTableWithAddColumn() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsAlterTableWithDropColumn() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsBatchUpdates() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsCatalogsInDataManipulation() throws SQLException
    {
        mLogger.logTrace();
        // DML에 Catalog 이름을 사용할 수 없다.
        return false;
    }

    public boolean supportsCatalogsInIndexDefinitions() throws SQLException
    {
        mLogger.logTrace();
        // catalog는 안씀
        return false;
    }

    public boolean supportsCatalogsInPrivilegeDefinitions() throws SQLException
    {
        mLogger.logTrace();
        // catalog는 안씀
        return false;
    }

    public boolean supportsCatalogsInProcedureCalls() throws SQLException
    {
        mLogger.logTrace();
        // catalog는 안씀
        return false;
    }

    public boolean supportsCatalogsInTableDefinitions() throws SQLException
    {
        mLogger.logTrace();
        // catalog는 안씀
        return false;
    }

    public boolean supportsColumnAliasing() throws SQLException
    {
        mLogger.logTrace();
        return SERVER_INFO_BASE_IS_THE_FUNCTION_SUPPERTTED.equals(getServerInfo(this, SERVER_INFO_ID_COLUMN_ALIASING));
    }

    public boolean supportsConvert() throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }

    public boolean supportsConvert(int aFromType, int aToType)
            throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }

    public boolean supportsCoreSQLGrammar() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsCorrelatedSubqueries() throws SQLException
    {
        mLogger.logTrace();
        /*
         * 위키 페이지에 'correlated subquery를 다음처럼 정의하고 있다.
         * In a SQL database query, a correlated subquery (also known as a synchronized subquery) is
         * a subquery (a query nested inside another query) that uses values from outer query.
         * The subquery is evaluated once for each row processed by the outer query.
         * 
         * 즉 inner에서 outer의 결과를 참조하여 다시 inner의 조건에 반영하는 구조를 말한다.
         * TPCH에 이런 쿼리가 있다고 한다. 
         */
        return true;
    }

    public boolean supportsDataDefinitionAndDataManipulationTransactions()
            throws SQLException
    {
        mLogger.logTrace();
        // 한 트랜잭션으로 DDL과 DML을 수행할 수 있다.
        return true;
    }

    public boolean supportsDataManipulationTransactionsOnly()
            throws SQLException
    {
        mLogger.logTrace();
        return false;
    }

    public boolean supportsDifferentTableCorrelationNames() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_CORRELATION_NAME) == SERVER_INFO_BASE_CORRELATTION_NAME_DIFFERENT;
    }

    public boolean supportsExpressionsInOrderBy() throws SQLException
    {
        mLogger.logTrace();
        return SERVER_INFO_BASE_IS_THE_FUNCTION_SUPPERTTED.equals(getServerInfo(this, SERVER_INFO_ID_EXPRESSIONS_IN_ORDER_BY));
    }

    public boolean supportsExtendedSQLGrammar() throws SQLException
    {
        mLogger.logTrace();
        /*
         *  outer-join, function call, binary literal 등 { } 문법 표기법을 말한다.
         *  JDBC에 구현되어 있지 않고 서버 파서에 구현되어 있다.
         *  함수는 구현되어 있지 않다.
         */
        return true;
    }

    public boolean supportsFullOuterJoins() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsGetGeneratedKeys() throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }

    public boolean supportsGroupBy() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsGroupByBeyondSelect() throws SQLException
    {
        mLogger.logTrace();
        /*
         * 예를 들어 다음과 같은 쿼리를 말한다.
         * select a from t1 group by a,b
         */
        return true;
    }

    public boolean supportsGroupByUnrelated() throws SQLException
    {
        mLogger.logTrace();
        /*
         * 예를 들어 다음과 같은 쿼리를 말한다.
         * select sum(a) from t1 group by b
         */
        return true;
    }

    public boolean supportsIntegrityEnhancementFacility() throws SQLException
    {
        mLogger.logTrace();
        return SERVER_INFO_BASE_IS_THE_FUNCTION_SUPPERTTED.equals(getServerInfo(this, SERVER_INFO_ID_INTEGRITY));
    }

    public boolean supportsLikeEscapeClause() throws SQLException
    {
        mLogger.logTrace();
        return SERVER_INFO_BASE_IS_THE_FUNCTION_SUPPERTTED.equals(getServerInfo(this, SERVER_INFO_ID_LIKE_ESCAPE));
    }

    public boolean supportsLimitedOuterJoins() throws SQLException
    {
        mLogger.logTrace();
        /*
         * JDBC 표준 문서에 다음처럼 설명된다.
         * Retrieves whether this database provides limited support for outer joins.
         * (This will be true if the method supportsFullOuterJoins returns true) --> 하지만 이문장이 맞는지 의심스럽다.
         * 일단은 문서대로 구현한다.
         */
        return true;
    }

    public boolean supportsMinimumSQLGrammar() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsMixedCaseIdentifiers() throws SQLException
    {
        mLogger.logTrace();
        return false;
    }

    public boolean supportsMixedCaseQuotedIdentifiers() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsMultipleOpenResults() throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }

    public boolean supportsMultipleResultSets() throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }

    public boolean supportsMultipleTransactions() throws SQLException
    {
        mLogger.logTrace();
        /*
         * JDBC 표준 문서에 다음처럼 설명되어 있다.
         * Retrieves whether this database allows having multiple transactions open at once (on different connections).
         * different connections라면 당연히 지원한다고 봐야한다.
         */
        return true;
    }

    public boolean supportsNamedParameters() throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }

    public boolean supportsNonNullableColumns() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsOpenCursorsAcrossCommit() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsOpenCursorsAcrossRollback() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsOpenStatementsAcrossCommit() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsOpenStatementsAcrossRollback() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsOrderByUnrelated() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsOuterJoins() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsPositionedDelete() throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }

    public boolean supportsPositionedUpdate() throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }

    public boolean supportsResultSetConcurrency(int aType, int aConcurrency)
            throws SQLException
    {
        mLogger.logTrace();
        // 모든 type, 모든 concurrency를 지원한다.
        return (aType == ResultSet.TYPE_FORWARD_ONLY ||
                aType == ResultSet.TYPE_SCROLL_INSENSITIVE ||
                aType == ResultSet.TYPE_SCROLL_SENSITIVE) &&
               (aConcurrency == ResultSet.CONCUR_READ_ONLY ||
                aConcurrency == ResultSet.CONCUR_UPDATABLE);
    }

    public boolean supportsResultSetHoldability(int aHoldability)
            throws SQLException
    {
        mLogger.logTrace();
        // 모든 holdability를 지원한다.
        return aHoldability == ResultSet.CLOSE_CURSORS_AT_COMMIT ||
               aHoldability == ResultSet.HOLD_CURSORS_OVER_COMMIT; 
    }

    public boolean supportsResultSetType(int aType) throws SQLException
    {
        mLogger.logTrace();
        // 모든 type을 지원한다.
        return aType == ResultSet.TYPE_FORWARD_ONLY ||
               aType == ResultSet.TYPE_SCROLL_INSENSITIVE ||
               aType == ResultSet.TYPE_SCROLL_SENSITIVE;
    }

    public boolean supportsSavepoints() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsSchemasInDataManipulation() throws SQLException
    {
        mLogger.logTrace();
        // DML에 Schema 이름을 사용할 수 있다.
        return true;
    }

    public boolean supportsSchemasInIndexDefinitions() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsSchemasInPrivilegeDefinitions() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsSchemasInProcedureCalls() throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }

    public boolean supportsSchemasInTableDefinitions() throws SQLException
    {
        mLogger.logTrace();
        // DDL에 Schema 이름을 사용할 수 있다.
        return true;
    }

    public boolean supportsSelectForUpdate() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsStatementPooling() throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }

    public boolean supportsStoredProcedures() throws SQLException
    {
        mLogger.logTrace();
        // TODO 추후 구현하면 바꿀 것
        return false;
    }

    public boolean supportsSubqueriesInComparisons() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsSubqueriesInExists() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsSubqueriesInIns() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsSubqueriesInQuantifieds() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsTableCorrelationNames() throws SQLException
    {
        mLogger.logTrace();
        return getIntServerInfo(this, SERVER_INFO_ID_CORRELATION_NAME) != SERVER_INFO_BASE_CORRELATTION_NAME_NONE;
    }

    public boolean supportsTransactionIsolationLevel(int aLevel)
            throws SQLException
    {
        mLogger.logTrace();
        int sSupportedIsolationLevels = getIntServerInfo(this, SERVER_INFO_ID_TXN_ISOLATION_LEVEL);
        if (aLevel == Connection.TRANSACTION_READ_UNCOMMITTED)
        {
            return (sSupportedIsolationLevels & SERVER_INFO_BASE_TXN_READ_UNCOMMITTED) != 0;
        }
        else if (aLevel == Connection.TRANSACTION_READ_COMMITTED)
        {
            return (sSupportedIsolationLevels & SERVER_INFO_BASE_TXN_READ_COMMITTED) != 0;
        }
        else if (aLevel == Connection.TRANSACTION_REPEATABLE_READ)
        {
            return (sSupportedIsolationLevels & SERVER_INFO_BASE_TXN_REPEATABLE_READ) != 0;
        }
        else if (aLevel == Connection.TRANSACTION_SERIALIZABLE)
        {
            return (sSupportedIsolationLevels & SERVER_INFO_BASE_TXN_SERIALIZABLE) != 0;
        }
        else
        {
            return false;
        }
    }

    public boolean supportsTransactions() throws SQLException
    {
        mLogger.logTrace();
        return true;
    }

    public boolean supportsUnion() throws SQLException
    {
        mLogger.logTrace();
        return (getIntServerInfo(this, SERVER_INFO_ID_UNION) & SERVER_INFO_BASE_UNION) != 0;
    }

    public boolean supportsUnionAll() throws SQLException
    {
        mLogger.logTrace();
        return (getIntServerInfo(this, SERVER_INFO_ID_UNION) & SERVER_INFO_BASE_UNION_ALL) != 0;
    }

    public boolean updatesAreDetected(int aResultSetType) throws SQLException
    {
        mLogger.logTrace();
        return (aResultSetType == ResultSet.TYPE_SCROLL_SENSITIVE);
    }

    public boolean usesLocalFilePerTable() throws SQLException
    {
        mLogger.logTrace();
        return false;
    }

    public boolean usesLocalFiles() throws SQLException
    {
        mLogger.logTrace();
        return false;
    }
}

