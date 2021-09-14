package sunje.goldilocks.sql;

import java.sql.Types;

public class GoldilocksTypes
{
    public static final int ARRAY = Types.ARRAY;
    public static final int BIGINT = Types.BIGINT;
    public static final int BINARY = Types.BINARY;
    public static final int BIT = Types.BIT;
    public static final int BLOB = Types.BLOB;
    public static final int BOOLEAN = Types.BOOLEAN;
    public static final int CHAR = Types.CHAR;
    public static final int CLOB = Types.CLOB;
    public static final int DATALINK = Types.DATALINK;
    public static final int DATE = Types.DATE;
    public static final int DECIMAL = Types.DECIMAL;
    public static final int DISTINCT = Types.DISTINCT;
    public static final int DOUBLE = Types.DOUBLE;
    public static final int FLOAT = Types.FLOAT;
    public static final int INTEGER = Types.INTEGER;
    public static final int JAVA_OBJECT = Types.JAVA_OBJECT;
    public static final int LONGVARBINARY = Types.LONGVARBINARY;
    public static final int NULL = Types.NULL;
    public static final int NUMERIC = Types.NUMERIC;
    public static final int OTHER = Types.OTHER;
    public static final int REAL = Types.REAL;
    public static final int REF = Types.REF;
    public static final int SMALLINT = Types.SMALLINT;
    public static final int STRUCT = Types.STRUCT;
    public static final int TIME = Types.TIME;
    public static final int TIMESTAMP = Types.TIMESTAMP;
    public static final int TINYINT = Types.TINYINT;
    public static final int VARBINARY = Types.VARBINARY;
    public static final int VARCHAR = Types.VARCHAR;
    
    /* 아래 상수는 JDBC4.0의 java.sql.Types에 명시된 값들이다.
     * 낮은 버전의 JDBC를 컴파일하기 위해 여기에 선언해둔다.
     */
    public static final int ROWID = -8; // java.sql.Types.ROWID
    public static final int NCHAR = -15; // java.sql.Types.NCHAR 
    public static final int NCLOB = 2011; // java.sql.Types.NCLOB
    public static final int NVARCHAR = -9; // java.sql.Types.NVARCHAR
    public static final int SQLXML = 2009; // java.sql.Types.SQLXML
    public static final int LONGVARCHAR = -1; // java.sql.Types.LONGVARCHAR
    public static final int LONGNVARCHAR = -16; // java.sql.Types.LONGNVARCHAR
    
    /*
     * 아래는 GOLDILOCKS 고유 타입들
     */
    public static final int INTERVAL_YEAR = -10001;
    public static final int INTERVAL_MONTH = -10002;
    public static final int INTERVAL_DAY = -10003;
    public static final int INTERVAL_HOUR = -10004;
    public static final int INTERVAL_MINUTE = -10005;
    public static final int INTERVAL_SECOND = -10006;
    public static final int INTERVAL_YEAR_TO_MONTH = -10007;
    public static final int INTERVAL_DAY_TO_HOUR = -10008;
    public static final int INTERVAL_DAY_TO_MINUTE = -10009;
    public static final int INTERVAL_DAY_TO_SECOND = -100010;
    public static final int INTERVAL_HOUR_TO_MINUTE = -10011;
    public static final int INTERVAL_HOUR_TO_SECOND = -10012;
    public static final int INTERVAL_MINUTE_TO_SECOND = -10013;
    
    public static final int TIME_WITH_TIME_ZONE = -20001;
    public static final int TIMESTAMP_WITH_TIME_ZONE = -20002;
}
