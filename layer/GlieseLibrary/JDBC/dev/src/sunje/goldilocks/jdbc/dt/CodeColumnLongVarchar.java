package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnLongVarchar extends CodeColumn
{
    static CodeColumnLongVarchar PROTOTYPE = new CodeColumnLongVarchar();
    
    private CodeColumnLongVarchar()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_LONGVARCHAR;
    }

    public int getRepresentativeSqlType()
    {
        return Types.LONGVARCHAR;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_LONGVARCHAR;
    }
    
    public String getClassName()
    {
        return String.class.getName();
    }

    Column getInstance()
    {
        return new ColumnChar(PROTOTYPE);
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
        return STRING_LENGTH_UNIT_OCTETS;
    }
    
    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_NA;
    }
}
