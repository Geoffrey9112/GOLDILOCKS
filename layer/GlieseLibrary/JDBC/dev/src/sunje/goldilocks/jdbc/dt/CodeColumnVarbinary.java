package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnVarbinary extends CodeColumn
{
    static CodeColumnVarbinary PROTOTYPE = new CodeColumnVarbinary();
    
    private CodeColumnVarbinary()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_VARBINARY;
    }

    public int getRepresentativeSqlType()
    {
        return Types.VARBINARY;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_VARBINARY;
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
        return DEFAULT_PRECISION_VARBINARY;
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
