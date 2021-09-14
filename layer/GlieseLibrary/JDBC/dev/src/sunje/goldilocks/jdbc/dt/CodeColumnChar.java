package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnChar extends CodeColumn
{
    static CodeColumnChar PROTOTYPE = new CodeColumnChar();
    
    private CodeColumnChar()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_CHAR;
    }

    public int getRepresentativeSqlType()
    {
        return Types.CHAR;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_CHAR;
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
        return DEFAULT_PRECISION_CHAR;
    }
    
    long getDefaultArgNum2()
    {
        return SCALE_NA;
    }
    
    byte getDefaultStringLengthUnit()
    {
        return STRING_LENGTH_UNIT_CHARACTERS;
    }
    
    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_NA;
    }
}
