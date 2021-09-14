package sunje.goldilocks.jdbc;

import java.io.Serializable;
import java.sql.Connection;
import java.sql.SQLException;
import java.util.Properties;

import javax.sql.PooledConnection;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4ConnectionPoolDataSource;

public class GoldilocksConnectionPoolDataSource extends Jdbc4ConnectionPoolDataSource implements Serializable
{
    private static final long serialVersionUID = -7172450966669513302L;

    static
    {
        Version5Specific.initialize();
    }

    protected Connection getPhysicalConnection() throws SQLException
    {
        return new GoldilocksConnection(mServerName, mPortNo, mDBName, mProp, this);
    }

    protected Connection getPhysicalConnection(String aUser, String aPassword) throws SQLException
    {
        Properties sProp = new Properties(mProp);
        sProp.put(Jdbc4Connection.GOLDILOCKS_PROP_NAME_USER, aUser);
        sProp.put(Jdbc4Connection.GOLDILOCKS_PROP_NAME_PASSWORD, aPassword);
        return new GoldilocksConnection(mServerName, mPortNo, mDBName, sProp, this);
    }

    public PooledConnection getPooledConnection() throws SQLException
    {
        return new GoldilocksPooledConnection((Jdbc4Connection)getPhysicalConnection());
    }

    public PooledConnection getPooledConnection(String aUser, String aPassword) throws SQLException
    {
        return new GoldilocksPooledConnection((Jdbc4Connection)getPhysicalConnection(aUser, aPassword));
    }
}
