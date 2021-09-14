package sunje.goldilocks.jdbc.dt;

import java.math.BigDecimal;
import java.sql.Types;

public class CodeColumnFloat extends CodeColumn
{
    static CodeColumnFloat PROTOTYPE = new CodeColumnFloat();
    
    private CodeColumnFloat()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_FLOAT;
    }

    public int getRepresentativeSqlType()
    {
        return Types.FLOAT;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_FLOAT;
    }
    
    public String getClassName()
    {
        return BigDecimal.class.getName();
    }

    Column getInstance()
    {
        return new ColumnNumber(PROTOTYPE);
    }
    
    long getDefaultArgNum1()
    {
        return DEFAULT_PRECISION_FLOAT;
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
