package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnDouble extends CodeColumn
{
    static CodeColumnDouble PROTOTYPE = new CodeColumnDouble();
    
    private CodeColumnDouble()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_NATIVE_DOUBLE;
    }
    
    public int getRepresentativeSqlType()
    {
        return Types.DOUBLE;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_NATIVE_DOUBLE;
    }
    
    public String getClassName()
    {
        return Double.class.getName();
    }

    Column getInstance()
    {
        return new ColumnDouble(PROTOTYPE);
    }
    
    long getDefaultArgNum1()
    {
        return DEFAULT_PRECISION_NATIVE_DOUBLE;
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
