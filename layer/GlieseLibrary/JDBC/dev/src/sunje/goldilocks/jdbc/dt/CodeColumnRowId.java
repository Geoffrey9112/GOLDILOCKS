package sunje.goldilocks.jdbc.dt;

import sunje.goldilocks.jdbc.core4.VersionSpecific;
import sunje.goldilocks.sql.GoldilocksTypes;

public class CodeColumnRowId extends CodeColumn
{
    static CodeColumnRowId PROTOTYPE = new CodeColumnRowId();
    
    private CodeColumnRowId()
    {        
    }

    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_ROWID;
    }

    public int getRepresentativeSqlType()
    {
        return GoldilocksTypes.ROWID;
    }

    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_ROWID;
    }
    
    public String getClassName()
    {
        return VersionSpecific.SINGLETON.getRowIdClassName();
    }

    Column getInstance()
    {
        return new ColumnRowId(PROTOTYPE);
    }
    
    long getDefaultArgNum1()
    {
        return DEFAULT_PRECISION_NATIVE_BIGINT;
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
