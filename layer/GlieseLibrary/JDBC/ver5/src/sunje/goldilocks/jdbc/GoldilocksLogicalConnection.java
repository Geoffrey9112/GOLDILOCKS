package sunje.goldilocks.jdbc;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4LogicalConnection;
import sunje.goldilocks.jdbc.core4.Jdbc4PooledConnection;


public class GoldilocksLogicalConnection extends Jdbc4LogicalConnection
{
    public GoldilocksLogicalConnection(Jdbc4Connection aPhysicalConnection, Jdbc4PooledConnection aPooledConnection)
    {
        super(aPhysicalConnection, aPooledConnection);
    }
}
