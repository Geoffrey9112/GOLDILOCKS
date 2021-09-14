package sunje.goldilocks.jdbc.dt;

import java.sql.Date;
import java.sql.Types;

public class CodeColumnDate extends CodeColumn
{
    static CodeColumnDate PROTOTYPE = new CodeColumnDate();
    
    private CodeColumnDate()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_DATE;
    }

    public int getRepresentativeSqlType()
    {
        return Types.DATE;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_DATE;
    }
    
    public String getClassName()
    {
        return Date.class.getName();
    }

    Column getInstance()
    {
        return new ColumnDate(PROTOTYPE);
    }
    
    long getDefaultArgNum1()
    {
        return DEFAULT_FRACTIONAL_SECOND_PRECISION_FOR_DATE;
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
