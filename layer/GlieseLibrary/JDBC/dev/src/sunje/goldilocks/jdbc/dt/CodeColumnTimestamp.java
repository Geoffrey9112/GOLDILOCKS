package sunje.goldilocks.jdbc.dt;

import java.sql.Timestamp;
import java.sql.Types;

public class CodeColumnTimestamp extends CodeColumn
{
    static CodeColumnTimestamp PROTOTYPE = new CodeColumnTimestamp();
    
    private CodeColumnTimestamp()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_TIMESTAMP;
    }

    public int getRepresentativeSqlType()
    {
        return Types.TIMESTAMP;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_TIMESTAMP;
    }
    
    public String getClassName()
    {
        return Timestamp.class.getName();
    }

    Column getInstance()
    {
        return new ColumnTimestamp(PROTOTYPE);
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
