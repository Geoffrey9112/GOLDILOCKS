package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnReal extends CodeColumn
{
    static CodeColumnReal PROTOTYPE = new CodeColumnReal();
    
    private CodeColumnReal()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_NATIVE_REAL;
    }
    
    public int getRepresentativeSqlType()
    {
        return Types.REAL;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_NATIVE_REAL;
    }
    
    public String getClassName()
    {
        return Float.class.getName();
    }

    Column getInstance()
    {
        return new ColumnReal(PROTOTYPE);
    }
    
    long getDefaultArgNum1()
    {
        return DEFAULT_PRECISION_NATIVE_REAL;
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
