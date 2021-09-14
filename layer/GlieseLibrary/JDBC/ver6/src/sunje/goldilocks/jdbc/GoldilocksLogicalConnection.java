package sunje.goldilocks.jdbc;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4PooledConnection;
import sunje.goldilocks.jdbc.core6.Jdbc6LogicalConnection;


public class GoldilocksLogicalConnection extends Jdbc6LogicalConnection
{
    public GoldilocksLogicalConnection(Jdbc4Connection aPhysicalConnection, Jdbc4PooledConnection aPooledConnection)
    {
        super(aPhysicalConnection, aPooledConnection);
    }
}
