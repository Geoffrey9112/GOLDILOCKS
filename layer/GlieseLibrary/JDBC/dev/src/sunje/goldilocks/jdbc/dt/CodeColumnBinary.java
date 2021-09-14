package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnBinary extends CodeColumn
{
    static CodeColumnBinary PROTOTYPE = new CodeColumnBinary();
    
    private CodeColumnBinary()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_BINARY;
    }

    public int getRepresentativeSqlType()
    {
        return Types.BINARY;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_BINARY;
    }
    
    public String getClassName()
    {
        return byte[].class.getName();
    }

    Column getInstance()
    {
        return new ColumnBinary(PROTOTYPE);
    }
    
    long getDefaultArgNum1()
    {
        return DEFAULT_PRECISION_BINARY;
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
