package sunje.goldilocks.jdbc.dt;

import java.sql.Timestamp;
import java.sql.Types;

public class CodeColumnTimestampTimeZone extends CodeColumn
{
    static CodeColumnTimestampTimeZone PROTOTYPE = new CodeColumnTimestampTimeZone();
    
    private CodeColumnTimestampTimeZone()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_TIMESTAMP_WITH_TIME_ZONE;
    }

    public int getRepresentativeSqlType()
    {
        return Types.TIMESTAMP;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_TIMESTAMP_WITH_TIME_ZONE;
    }
    
    public String getClassName()
    {
        return Timestamp.class.getName();
    }

    Column getInstance()
    {
        return new ColumnTimestampTimeZone(PROTOTYPE);
    }
    
    long getDefaultArgNum1()
    {
        return DEFAULT_FRACTIONAL_SECOND_PRECISION;
    }
    
    long getDefaultArgNum2()
    {
        return SCALE_NA;
    }
    
    byte getDefaultStringLengthUnit()
    {
        return STRING_LENGTH_UNIT_NA;
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_NA;
    }
}
