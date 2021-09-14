package sunje.goldilocks.jdbc;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4LogicalConnection;
import sunje.goldilocks.jdbc.core6.Jdbc6PooledConnection;

public class GoldilocksPooledConnection extends Jdbc6PooledConnection
{
    public GoldilocksPooledConnection(Jdbc4Connection aPhysicalConnection)
    {
        super(aPhysicalConnection);
    }
    
    protected Jdbc4LogicalConnection createLogicalConnection(Jdbc4Connection aPhysicalConnection)
    {
        return new GoldilocksLogicalConnection(aPhysicalConnection, this);
    }
}
