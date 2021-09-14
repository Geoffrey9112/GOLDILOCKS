package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnBoolean extends CodeColumn
{
    static CodeColumnBoolean PROTOTYPE = new CodeColumnBoolean();
    
    private CodeColumnBoolean()
    {
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_BOOLEAN;
    }
    
    public int getRepresentativeSqlType()
    {
        return Types.BOOLEAN;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_BOOLEAN;
    }
    
    public String getClassName()
    {
        return Boolean.class.getName();
    }

    Column getInstance()
    {
        return new ColumnBoolean(PROTOTYPE);
    }
    
    long getDefaultArgNum1()
    {
        return DEFAULT_PRECISION_NA;
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
