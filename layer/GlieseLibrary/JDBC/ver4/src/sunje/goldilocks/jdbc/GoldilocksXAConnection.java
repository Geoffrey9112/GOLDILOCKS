package sunje.goldilocks.jdbc;

import java.sql.SQLException;

import javax.sql.XAConnection;
import javax.transaction.xa.XAResource;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4LogicalConnection;
import sunje.goldilocks.jdbc.core4.Jdbc4PooledConnection;
import sunje.goldilocks.jdbc.core4.Jdbc4XAResource;

public class GoldilocksXAConnection extends Jdbc4PooledConnection implements XAConnection
{
    private Jdbc4XAResource mResource = null;

    public GoldilocksXAConnection(Jdbc4Connection aPhysicalConnection) throws SQLException
    {
        super(aPhysicalConnection);
        aPhysicalConnection.setAutoCommit(false);
    }

    protected Jdbc4LogicalConnection createLogicalConnection(Jdbc4Connection aPhysicalConnection)
    {
        return new GoldilocksLogicalConnection(aPhysicalConnection, this);
    }

    public XAResource getXAResource() throws SQLException
    {
        mLogicalConnection.getPhysicalConnection().getLogger().logTrace();
        if (mResource == null)
        {
            mResource = new GoldilocksXAResource(mLogicalConnection.getPhysicalConnection());
        }
        return mResource;
    }
}
