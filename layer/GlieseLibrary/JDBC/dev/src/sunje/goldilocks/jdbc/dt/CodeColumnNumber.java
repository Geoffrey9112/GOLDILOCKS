package sunje.goldilocks.jdbc.dt;

import java.math.BigDecimal;
import java.sql.Types;

public class CodeColumnNumber extends CodeColumn
{
    static CodeColumnNumber PROTOTYPE = new CodeColumnNumber();
    
    private CodeColumnNumber()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_NUMBER;
    }

    public int getRepresentativeSqlType()
    {
        return Types.NUMERIC;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_NUMBER;
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
        return DEFAULT_PRECISION_NUMERIC;
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
