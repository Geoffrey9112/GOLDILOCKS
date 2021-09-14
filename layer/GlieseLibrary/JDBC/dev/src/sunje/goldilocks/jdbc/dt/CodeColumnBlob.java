package sunje.goldilocks.jdbc.dt;

import java.sql.Blob;
import java.sql.SQLException;
import java.sql.Types;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class CodeColumnBlob extends CodeColumn
{
    static CodeColumnBlob PROTOTYPE = new CodeColumnBlob();
    
    private CodeColumnBlob()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_BLOB;
    }

    public int getRepresentativeSqlType()
    {
        return Types.BLOB;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_BLOB;
    }
    
    public String getClassName()
    {
        return Blob.class.getName();
    }

    Column getInstance() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "BLOB type");
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
