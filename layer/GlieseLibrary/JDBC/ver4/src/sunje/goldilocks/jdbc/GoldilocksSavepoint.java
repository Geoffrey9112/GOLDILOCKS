package sunje.goldilocks.jdbc;

import sunje.goldilocks.jdbc.core4.Jdbc4Savepoint;

public class GoldilocksSavepoint extends Jdbc4Savepoint
{
    public GoldilocksSavepoint(int aId)
    {
        super(aId);
    }

    public GoldilocksSavepoint(String aName)
    {
        super(aName);
    }
}
