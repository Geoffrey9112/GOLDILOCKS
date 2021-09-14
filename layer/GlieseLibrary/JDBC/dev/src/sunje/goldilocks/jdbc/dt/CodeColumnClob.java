package sunje.goldilocks.jdbc.dt;

import java.sql.Clob;
import java.sql.SQLException;
import java.sql.Types;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class CodeColumnClob extends CodeColumn
{
    static CodeColumnClob PROTOTYPE = new CodeColumnClob();
    
    private CodeColumnClob()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_CLOB;
    }

    public int getRepresentativeSqlType()
    {
        return Types.CLOB;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_CLOB;
    }
    
    public String getClassName()
    {
        return Clob.class.getName();
    }

    Column getInstance() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "CLOB type");
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
