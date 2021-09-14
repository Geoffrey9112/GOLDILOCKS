package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

public class CodeColumnVarchar extends CodeColumn
{
    static CodeColumnVarchar PROTOTYPE = new CodeColumnVarchar();
    
    private CodeColumnVarchar()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_VARCHAR;
    }

    public int getRepresentativeSqlType()
    {
        return Types.VARCHAR;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_VARCHAR;
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
        return DEFAULT_PRECISION_VARCHAR;
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
