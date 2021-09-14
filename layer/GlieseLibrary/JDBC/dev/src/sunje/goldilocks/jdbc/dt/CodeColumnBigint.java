package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnBigint extends CodeColumn
{
    static CodeColumnBigint PROTOTYPE = new CodeColumnBigint();
    
    private CodeColumnBigint()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_NATIVE_BIGINT;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_NATIVE_BIGINT;
    }
    
    public int getRepresentativeSqlType()
    {
        return Types.BIGINT;
    }

    public String getClassName()
    {
        return Long.class.getName();
    }
    
    Column getInstance()
    {
        return new ColumnBigint(PROTOTYPE);
    }

    long getDefaultArgNum1()
    {
        return DEFAULT_PRECISION_NATIVE_BIGINT;
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
