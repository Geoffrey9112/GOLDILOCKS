package sunje.goldilocks.jdbc.dt;

import java.sql.Time;
import java.sql.Types;

public class CodeColumnTimeTimeZone extends CodeColumn
{
    static CodeColumnTimeTimeZone PROTOTYPE = new CodeColumnTimeTimeZone();
    
    private CodeColumnTimeTimeZone()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_TIME_WITH_TIME_ZONE;
    }

    public int getRepresentativeSqlType()
    {
        return Types.TIME;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_TIME_WITH_TIME_ZONE;
    }
    
    public String getClassName()
    {
        return Time.class.getName();
    }

    Column getInstance()
    {
        return new ColumnTimeTimeZone(PROTOTYPE);
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
