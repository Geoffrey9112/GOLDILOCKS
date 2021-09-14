package sunje.goldilocks.jdbc.dt;

import java.sql.Types;

import sunje.goldilocks.sql.GoldilocksInterval;

public abstract class CodeColumnInterval extends CodeColumn
{
    protected CodeColumnInterval()
    {        
    }
    
    public int getRepresentativeSqlType()
    {
        return Types.OTHER;
    }

    public String getClassName()
    {
        return GoldilocksInterval.class.getName();
    }
    
    long getDefaultArgNum1()
    {
        return DEFAULT_LEADING_PRECISION_INTERVAL;
    }
    
    long getDefaultArgNum2()
    {
        return DEFAULT_FRACTIONAL_PRECISION_INTERVAL;
    }
    
    byte getDefaultStringLengthUnit()
    {
        return STRING_LENGTH_UNIT_NA;
    }
}
