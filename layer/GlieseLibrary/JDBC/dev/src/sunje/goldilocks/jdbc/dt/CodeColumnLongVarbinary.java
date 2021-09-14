package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnLongVarbinary extends CodeColumn
{
    static CodeColumnLongVarbinary PROTOTYPE = new CodeColumnLongVarbinary();
    
    private CodeColumnLongVarbinary()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_LONGVARBINARY;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_LONGVARBINARY;
    }
    
    public int getRepresentativeSqlType()
    {
        return Types.LONGVARBINARY;
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
