package sunje.goldilocks.jdbc.dt;

import java.sql.SQLException;
import java.sql.Types;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.sql.GoldilocksTypes;

public abstract class CodeColumn
{
    /* 
     * Goldilocks Data Types
     */
    public static final byte GOLDILOCKS_DATA_TYPE_BOOLEAN = 0;
    public static final byte GOLDILOCKS_DATA_TYPE_NATIVE_SMALLINT = 1;
    public static final byte GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER = 2;
    public static final byte GOLDILOCKS_DATA_TYPE_NATIVE_BIGINT = 3;
    public static final byte GOLDILOCKS_DATA_TYPE_NATIVE_REAL = 4;
    public static final byte GOLDILOCKS_DATA_TYPE_NATIVE_DOUBLE = 5;
    public static final byte GOLDILOCKS_DATA_TYPE_FLOAT = 6;    
    public static final byte GOLDILOCKS_DATA_TYPE_NUMBER = 7;
    public static final byte GOLDILOCKS_DATA_TYPE_DECIMAL = 8;
    public static final byte GOLDILOCKS_DATA_TYPE_CHAR = 9;
    public static final byte GOLDILOCKS_DATA_TYPE_VARCHAR = 10;
    public static final byte GOLDILOCKS_DATA_TYPE_LONGVARCHAR = 11;
    public static final byte GOLDILOCKS_DATA_TYPE_CLOB = 12;
    public static final byte GOLDILOCKS_DATA_TYPE_BINARY = 13;
    public static final byte GOLDILOCKS_DATA_TYPE_VARBINARY = 14;
    public static final byte GOLDILOCKS_DATA_TYPE_LONGVARBINARY = 15;
    public static final byte GOLDILOCKS_DATA_TYPE_BLOB = 16;
    public static final byte GOLDILOCKS_DATA_TYPE_DATE = 17;
    public static final byte GOLDILOCKS_DATA_TYPE_TIME = 18;
    public static final byte GOLDILOCKS_DATA_TYPE_TIMESTAMP = 19;
    public static final byte GOLDILOCKS_DATA_TYPE_TIME_WITH_TIME_ZONE = 20;
    public static final byte GOLDILOCKS_DATA_TYPE_TIMESTAMP_WITH_TIME_ZONE = 21;
    public static final byte GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH = 22;
    public static final byte GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND = 23;
    public static final byte GOLDILOCKS_DATA_TYPE_ROWID = 24;
    public static final byte GOLDILOCKS_DATA_TYPE_MAX = 25;
    
    public static final String GOLDILOCKS_DATA_TYPE_NAME_BOOLEAN = "BOOLEAN";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_NATIVE_SMALLINT = "NATIVE_SMALLINT";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_NATIVE_INTEGER = "NATIVE_INTEGER";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_NATIVE_BIGINT = "NATIVE_BIGINT";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_NATIVE_REAL = "NATIVE_REAL";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_NATIVE_DOUBLE = "NATIVE_DOUBLE";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_FLOAT = "FLOAT";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_NUMBER = "NUMBER";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_DECIMAL = "DECIMAL";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_CHAR = "CHAR";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_VARCHAR = "VARCHAR";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_LONGVARCHAR = "LONGVARCHAR";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_CLOB = "CLOB";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_BINARY = "BINARY";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_VARBINARY = "VARBINARY";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_LONGVARBINARY = "LONGVARBINARY";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_BLOB = "BLOB";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_DATE = "DATE";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_TIME = "TIME";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_TIMESTAMP = "TIMESTAMP";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_TIME_WITH_TIME_ZONE = "TIME WITH TIME ZONE";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_TIMESTAMP_WITH_TIME_ZONE = "TIMESTAMP WITH TIME ZONE";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_YEAR_TO_MONTH = "INTERVAL YEAR TO MONTH";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_YEAR = "INTERVAL YEAR";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_MONTH = "INTERVAL MONTH";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_DAY_TO_SECOND = "INTERVAL DAY TO SECOND";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_DAY = "INTERVAL DAY";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_HOUR = "INTERVAL HOUR";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_MINUTE = "INTERVAL MINUTE";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_SECOND = "INTERVAL SECOND";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_DAY_TO_HOUR = "INTERVAL DAY TO HOUR";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_DAY_TO_MINUTE = "INTERVAL DAY TO MINUTE";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_HOUR_TO_MINUTE = "INTERVAL HOUR TO MINUTE";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_HOUR_TO_SECOND = "INTERVAL HOUR TO SECOND";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_MINUTE_TO_SECOND = "INTERVAL MINUTE TO SECOND";
    public static final String GOLDILOCKS_DATA_TYPE_NAME_ROWID = "ROWID";
    
    /* default precision 정의 */
    public static final int DEFAULT_PRECISION_NATIVE_SMALLINT = 15;
    public static final int DEFAULT_PRECISION_NATIVE_INTEGER = 31;
    public static final int DEFAULT_PRECISION_NATIVE_BIGINT = 63;
    public static final int DEFAULT_PRECISION_NATIVE_REAL = 23;
    public static final int DEFAULT_PRECISION_NATIVE_DOUBLE = 52;
    public static final int DEFAULT_PRECISION_FLOAT = 126;
    public static final int DEFAULT_PRECISION_NUMERIC = 38;
    public static final int DEFAULT_PRECISION_CHAR = 2000;
    public static final int DEFAULT_PRECISION_VARCHAR = 4000;
    public static final int DEFAULT_PRECISION_BINARY = 2000;
    public static final int DEFAULT_PRECISION_VARBINARY = 4000;
    public static final int DEFAULT_PRECISION_TIME = 6;
    public static final int DEFAULT_PRECISION_TIMESTAMP = 6;
    public static final int DEFAULT_PRECISION_NA = Integer.MIN_VALUE;
    public static final int DEFAULT_LEADING_PRECISION_INTERVAL = 6; // 최대 precision이 default여야 한다.
    public static final int DEFAULT_FRACTIONAL_PRECISION_INTERVAL = 6;
    public static final int NUMERIC_PRECISION_FOR_BIGINT = 19;
    
    public static final int DEFAULT_FRACTIONAL_SECOND_PRECISION = 6;
    public static final int DEFAULT_FRACTIONAL_SECOND_PRECISION_FOR_DATE = 0;

    public static final int NUMERIC_SCALE_FOR_BIGINT = 0;    
    public static final int SCALE_NA = Integer.MIN_VALUE;
    
    public static final byte STRING_LENGTH_UNIT_NA = 0;
    public static final byte STRING_LENGTH_UNIT_CHARACTERS = 1;
    public static final byte STRING_LENGTH_UNIT_OCTETS = 2;
    
    public static final byte INTERVAL_INDICATOR_NA = 0;
    public static final byte INTERVAL_INDICATOR_YEAR = 1;
    public static final byte INTERVAL_INDICATOR_MONTH = 2;
    public static final byte INTERVAL_INDICATOR_DAY = 3;
    public static final byte INTERVAL_INDICATOR_HOUR = 4;
    public static final byte INTERVAL_INDICATOR_MINUTE = 5;
    public static final byte INTERVAL_INDICATOR_SECOND = 6;
    public static final byte INTERVAL_INDICATOR_YEAR_TO_MONTH = 7;
    public static final byte INTERVAL_INDICATOR_DAY_TO_HOUR = 8;
    public static final byte INTERVAL_INDICATOR_DAY_TO_MINUTE = 9;
    public static final byte INTERVAL_INDICATOR_DAY_TO_SECOND = 10;
    public static final byte INTERVAL_INDICATOR_HOUR_TO_MINUTE = 11;
    public static final byte INTERVAL_INDICATOR_HOUR_TO_SECOND = 12;
    public static final byte INTERVAL_INDICATOR_MINUTE_TO_SECOND = 13;
    public static final byte INTERVAL_INDICATOR_MAX = 14;
    
    private static CodeColumn[] mColumnPrototypes = new CodeColumn[GOLDILOCKS_DATA_TYPE_MAX]; 
    private static CodeColumn[] mIntervalColumnPrototypes = new CodeColumn[INTERVAL_INDICATOR_MAX];
    
    static
    {
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_BOOLEAN] = CodeColumnBoolean.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_NATIVE_SMALLINT] = CodeColumnSmallint.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER] = CodeColumnInteger.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_NATIVE_BIGINT] = CodeColumnBigint.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_NATIVE_REAL] = CodeColumnReal.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_NATIVE_DOUBLE] = CodeColumnDouble.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_FLOAT] = CodeColumnFloat.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_NUMBER] = CodeColumnNumber.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_DECIMAL] = CodeColumnDecimal.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_CHAR] = CodeColumnChar.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_VARCHAR] = CodeColumnVarchar.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_LONGVARCHAR] = CodeColumnLongVarchar.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_CLOB] = CodeColumnClob.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_BINARY] = CodeColumnBinary.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_VARBINARY] = CodeColumnVarbinary.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_LONGVARBINARY] = CodeColumnLongVarbinary.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_BLOB] = CodeColumnBlob.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_DATE] = CodeColumnDate.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_TIME] = CodeColumnTime.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_TIMESTAMP] = CodeColumnTimestamp.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_TIME_WITH_TIME_ZONE] = CodeColumnTimeTimeZone.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_TIMESTAMP_WITH_TIME_ZONE] = CodeColumnTimestampTimeZone.PROTOTYPE;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH] = null;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND] = null;
        mColumnPrototypes[GOLDILOCKS_DATA_TYPE_ROWID] = CodeColumnRowId.PROTOTYPE;
        
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_NA] = null;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_YEAR] = CodeColumnIntervalYear.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_MONTH] = CodeColumnIntervalMonth.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_DAY] = CodeColumnIntervalDay.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_HOUR] = CodeColumnIntervalHour.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_MINUTE] = CodeColumnIntervalMinute.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_SECOND] = CodeColumnIntervalSecond.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_YEAR_TO_MONTH] = CodeColumnIntervalYearToMonth.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_DAY_TO_HOUR] = CodeColumnIntervalDayToHour.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_DAY_TO_MINUTE] = CodeColumnIntervalDayToMinute.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_DAY_TO_SECOND] = CodeColumnIntervalDayToSecond.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_HOUR_TO_MINUTE] = CodeColumnIntervalHourToMinute.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_HOUR_TO_SECOND] = CodeColumnIntervalHourToSecond.PROTOTYPE;
        mIntervalColumnPrototypes[INTERVAL_INDICATOR_MINUTE_TO_SECOND] = CodeColumnIntervalMinuteToSecond.PROTOTYPE;        
    }

    private static String sqlTypeName(int aSqlType)
    {
        String sResult = null;
        switch (aSqlType)
        {
            case Types.BIGINT:
                sResult = "BIGINT";
                break;
            case Types.BINARY:
                sResult = "BINARY";
                break;
            case Types.BIT:
                sResult = "BIT";
                break;
            case Types.BOOLEAN:
                sResult = "BOOLEAN";
                break;
            case Types.BLOB:
                sResult = "BLOB";
                break;
            case Types.CHAR:
                sResult = "CHAR";
                break;
            case Types.CLOB:
                sResult = "CLOB";
                break;
            case Types.DATE:
                sResult = "DATE";
                break;
            case Types.DECIMAL:
                sResult = "DECIMAL";
                break;
            case Types.DOUBLE:
                sResult = "DOUBLE";
                break;
            case Types.FLOAT:
                sResult = "FLOAT";
                break;
            case Types.NUMERIC:
                sResult = "NUMERIC";
                break;
            case Types.INTEGER:
                sResult = "INTEGER";
                break;
            case Types.LONGVARBINARY:
                sResult = "LONGVARBINARY";
                break;
            case Types.LONGVARCHAR:
                sResult = "LONGVARCHAR";
                break;
            case Types.REAL:
                sResult = "REAL";
                break;
            case GoldilocksTypes.ROWID:
                sResult = "ROWID";
                break;
            case Types.SMALLINT:
                sResult = "SMALLINT";
                break;
            case Types.TINYINT:
                sResult = "TINYINT";
                break;
            case Types.TIME:
                sResult = "TIME";
                break;
            case Types.TIMESTAMP:
                sResult = "TIMESTAMP";
                break;
            case Types.VARBINARY:
                sResult = "VARBINARY";
                break;
            case Types.VARCHAR:
                sResult = "VARCHAR";
                break;
            case Types.NULL:
                sResult = "NULL";
                break;
            case GoldilocksTypes.INTERVAL_YEAR:
                sResult = "INTERVAL_YEAR";
                break;
            case GoldilocksTypes.INTERVAL_MONTH:
                sResult = "INTERVAL_MONTH";
                break;
            case GoldilocksTypes.INTERVAL_YEAR_TO_MONTH:
                sResult = "INTERVAL_YEAR_TO_MONTH";
                break;
            case GoldilocksTypes.INTERVAL_DAY:
                sResult = "INTERVAL_DAY";
                break;
            case GoldilocksTypes.INTERVAL_HOUR:
                sResult = "INTERVAL_HOUR";
                break;
            case GoldilocksTypes.INTERVAL_MINUTE:
                sResult = "INTERVAL_MINUTE";
                break;
            case GoldilocksTypes.INTERVAL_SECOND:
                sResult = "INTERVAL_SECOND";
                break;
            case GoldilocksTypes.INTERVAL_DAY_TO_HOUR:
                sResult = "INTERVAL_DAY_TO_HOUR";
                break;
            case GoldilocksTypes.INTERVAL_DAY_TO_MINUTE:
                sResult = "INTERVAL_DAY_TO_MINUTE";
                break;
            case GoldilocksTypes.INTERVAL_DAY_TO_SECOND:
                sResult = "INTERVAL_DAY_TO_SECOND";
                break;
            case GoldilocksTypes.INTERVAL_HOUR_TO_MINUTE:
                sResult = "INTERVAL_HOUR_TO_MINUTE";
                break;
            case GoldilocksTypes.INTERVAL_HOUR_TO_SECOND:
                sResult = "INTERVAL_HOUR_TO_SECOND";
                break;
            case GoldilocksTypes.INTERVAL_MINUTE_TO_SECOND:
                sResult = "INTERVAL_MINUTE_TO_SECOND";
                break;
            case Types.OTHER:
                sResult = "OTHER";
                break;
            case Types.ARRAY:
                sResult = "ARRAY";
                break;
            case Types.DATALINK:
                sResult = "DATALINK";
                break;
            case Types.DISTINCT:
                sResult = "DISTINCT";
                break;
            case GoldilocksTypes.NCHAR:
                sResult = "NCHAR";
                break;
            case GoldilocksTypes.NCLOB:
                sResult = "NCLOB";
                break;
            case GoldilocksTypes.NVARCHAR:
                sResult = "NVARCHAR";
                break;
            case Types.JAVA_OBJECT:
                sResult = "JAVA_OBJECT";
                break;
            case Types.REF:
                sResult = "REF";
                break;
            case GoldilocksTypes.SQLXML:
                sResult = "SQLXML";
                break;
            case Types.STRUCT:
                sResult = "STRUCT";
                break;
            default:
                sResult = "UNKNOWN";
                break;
        }
        return sResult;
    }
    
    public static byte[] getDataTypeForSqlType(int aSqlType) throws SQLException
    {
        byte[] sResult = new byte[2];
        sResult[0] = GOLDILOCKS_DATA_TYPE_MAX;
        sResult[1] = INTERVAL_INDICATOR_NA;
        switch (aSqlType)
        {
            case Types.BIGINT:
                sResult[0] = GOLDILOCKS_DATA_TYPE_NATIVE_BIGINT;
                break;
            case Types.BINARY:
                sResult[0] = GOLDILOCKS_DATA_TYPE_BINARY;
                break;
            case Types.BIT:
            case Types.BOOLEAN:
                sResult[0] = GOLDILOCKS_DATA_TYPE_BOOLEAN;
                break;
            case Types.BLOB:
                sResult[0] = GOLDILOCKS_DATA_TYPE_BLOB;
                break;
            case Types.CHAR:
                sResult[0] = GOLDILOCKS_DATA_TYPE_CHAR;
                break;
            case Types.CLOB:
                sResult[0] = GOLDILOCKS_DATA_TYPE_CLOB;
                break;
            case Types.DATE:
                sResult[0] = GOLDILOCKS_DATA_TYPE_DATE;
                break;
            case Types.DECIMAL:
                sResult[0] = GOLDILOCKS_DATA_TYPE_DECIMAL;
                break;
            case Types.DOUBLE:
                sResult[0] = GOLDILOCKS_DATA_TYPE_NATIVE_DOUBLE;
                break;
            case Types.FLOAT:
                sResult[0] = GOLDILOCKS_DATA_TYPE_FLOAT;
                break;
            case Types.NUMERIC:
                sResult[0] = GOLDILOCKS_DATA_TYPE_NUMBER;
                break;
            case Types.INTEGER:
                sResult[0] = GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER;
                break;
            case Types.LONGVARBINARY:
                sResult[0] = GOLDILOCKS_DATA_TYPE_LONGVARBINARY;
                break;
            case Types.LONGVARCHAR:
                sResult[0] = GOLDILOCKS_DATA_TYPE_LONGVARCHAR;
                break;
            case Types.REAL:
                sResult[0] = GOLDILOCKS_DATA_TYPE_NATIVE_REAL;
                break;
            case GoldilocksTypes.ROWID:
                sResult[0] = GOLDILOCKS_DATA_TYPE_ROWID;
                break;
            case Types.SMALLINT:
            case Types.TINYINT:
                sResult[0] = GOLDILOCKS_DATA_TYPE_NATIVE_SMALLINT;
                break;
            case Types.TIME:
                sResult[0] = GOLDILOCKS_DATA_TYPE_TIME;
                break;
            case Types.TIMESTAMP:
                sResult[0] = GOLDILOCKS_DATA_TYPE_TIMESTAMP;
                break;
            case Types.VARBINARY:
                sResult[0] = GOLDILOCKS_DATA_TYPE_VARBINARY;
                break;
            case Types.VARCHAR:
            case Types.NULL:
                sResult[0] = GOLDILOCKS_DATA_TYPE_VARCHAR;
                break;
            case GoldilocksTypes.TIME_WITH_TIME_ZONE:
                sResult[0] = GOLDILOCKS_DATA_TYPE_TIME_WITH_TIME_ZONE;
                break;
            case GoldilocksTypes.TIMESTAMP_WITH_TIME_ZONE:
                sResult[0] = GOLDILOCKS_DATA_TYPE_TIMESTAMP_WITH_TIME_ZONE;
                break;
            case GoldilocksTypes.INTERVAL_YEAR:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH;
                sResult[1] = INTERVAL_INDICATOR_YEAR;
                break;
            case GoldilocksTypes.INTERVAL_MONTH:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH;
                sResult[1] = INTERVAL_INDICATOR_MONTH;
                break;
            case GoldilocksTypes.INTERVAL_YEAR_TO_MONTH:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH;
                sResult[1] = INTERVAL_INDICATOR_YEAR_TO_MONTH;
                break;
            case GoldilocksTypes.INTERVAL_DAY:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
                sResult[1] = INTERVAL_INDICATOR_DAY;
                break;
            case GoldilocksTypes.INTERVAL_HOUR:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
                sResult[1] = INTERVAL_INDICATOR_HOUR;
                break;
            case GoldilocksTypes.INTERVAL_MINUTE:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
                sResult[1] = INTERVAL_INDICATOR_MINUTE;
                break;
            case GoldilocksTypes.INTERVAL_SECOND:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
                sResult[1] = INTERVAL_INDICATOR_SECOND;
                break;
            case GoldilocksTypes.INTERVAL_DAY_TO_HOUR:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
                sResult[1] = INTERVAL_INDICATOR_DAY_TO_HOUR;
                break;
            case GoldilocksTypes.INTERVAL_DAY_TO_MINUTE:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
                sResult[1] = INTERVAL_INDICATOR_DAY_TO_MINUTE;
                break;
            case GoldilocksTypes.INTERVAL_DAY_TO_SECOND:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
                sResult[1] = INTERVAL_INDICATOR_DAY_TO_SECOND;
                break;
            case GoldilocksTypes.INTERVAL_HOUR_TO_MINUTE:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
                sResult[1] = INTERVAL_INDICATOR_HOUR_TO_MINUTE;
                break;
            case GoldilocksTypes.INTERVAL_HOUR_TO_SECOND:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
                sResult[1] = INTERVAL_INDICATOR_HOUR_TO_SECOND;
                break;
            case GoldilocksTypes.INTERVAL_MINUTE_TO_SECOND:
                sResult[0] = GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
                sResult[1] = INTERVAL_INDICATOR_MINUTE_TO_SECOND;
                break;
            case Types.OTHER:
            case Types.ARRAY:
            case Types.DATALINK:
            case Types.DISTINCT:
            case GoldilocksTypes.NCHAR:
            case GoldilocksTypes.NCLOB:
            case GoldilocksTypes.NVARCHAR:
            case Types.JAVA_OBJECT:
            case Types.REF:
            case GoldilocksTypes.SQLXML:
            case Types.STRUCT:
                ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, sqlTypeName(aSqlType) + " type");
            default:
                ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, sqlTypeName(aSqlType));
        }
        return sResult;
    }
    
    static Column createColumn(byte aType, byte aIndicator) throws SQLException
    {
        return getCodeColumn(aType, aIndicator).getInstance();
    }
    
    public static CodeColumn getCodeColumn(byte aType, byte aIndicator)
    {
        if (aType == GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH ||
            aType == GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND)
        {            
            return mIntervalColumnPrototypes[aIndicator];
        }
        else
        {
            return mColumnPrototypes[aType];
        }
    }
    
    public abstract byte getDataType();
    
    public abstract String getTypeName();
    
    public abstract int getRepresentativeSqlType();

    public abstract String getClassName();
    
    abstract Column getInstance() throws SQLException;
    
    abstract long getDefaultArgNum1();

    abstract long getDefaultArgNum2();
    
    abstract byte getDefaultStringLengthUnit();

    abstract byte getDefaultIntervalIndicator();
}
