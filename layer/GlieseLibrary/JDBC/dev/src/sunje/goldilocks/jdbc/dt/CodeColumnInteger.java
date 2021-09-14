package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnInteger extends CodeColumn
{
    static CodeColumnInteger PROTOTYPE = new CodeColumnInteger();
    
    private CodeColumnInteger()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER;
    }
    
    public int getRepresentativeSqlType()
    {
        return Types.INTEGER;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_NATIVE_INTEGER;
    }

    public String getClassName()
    {
        return Integer.class.getName();
    }

    Column getInstance()
    {
        return new ColumnInteger(PROTOTYPE);
    }
    
    long getDefaultArgNum1()
    {
        return DEFAULT_PRECISION_NATIVE_INTEGER;
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
