package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnSmallint extends CodeColumn
{
    static CodeColumnSmallint PROTOTYPE = new CodeColumnSmallint();
    
    private CodeColumnSmallint()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_NATIVE_SMALLINT;
    }
    
    public int getRepresentativeSqlType()
    {
        return Types.SMALLINT;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_NATIVE_SMALLINT;
    }
    
    public String getClassName()
    {
        return Short.class.getName();
    }
    
    Column getInstance()
    {
        return new ColumnSmallint(PROTOTYPE);
    }

    long getDefaultArgNum1()
    {
        return DEFAULT_PRECISION_NATIVE_SMALLINT;
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
