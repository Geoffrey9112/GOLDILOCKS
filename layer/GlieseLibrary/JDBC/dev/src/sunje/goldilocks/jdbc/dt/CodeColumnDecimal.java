package sunje.goldilocks.jdbc.dt;

import java.math.BigDecimal;
import java.sql.SQLException;
import java.sql.Types;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class CodeColumnDecimal extends CodeColumn
{
    static CodeColumnDecimal PROTOTYPE = new CodeColumnDecimal();
    
    private CodeColumnDecimal()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_DECIMAL;
    }

    public int getRepresentativeSqlType()
    {
        return Types.DECIMAL;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_DECIMAL;
    }
    
    public String getClassName()
    {
        return BigDecimal.class.getName();
    }

    Column getInstance() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "DECIMAL type");
        return null;
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
