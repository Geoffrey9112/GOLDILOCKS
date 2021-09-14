package sunje.goldilocks.jdbc;

import java.io.Serializable;
import java.sql.Connection;
import java.sql.SQLException;
import java.util.Properties;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core6.Jdbc6DataSource;

public class GoldilocksDataSource extends Jdbc6DataSource implements Serializable
{
    private static final long serialVersionUID = 2225637644364464052L;

    static
    {
        Version6Specific.initialize();
    }

    public Connection getConnection() throws SQLException
    {
        return new GoldilocksConnection(mServerName, mPortNo, mDBName, mProp, this);
    }

    public Connection getConnection(String aUser, String aPassword) throws SQLException
    {
        Properties sProp = new Properties(mProp);
        sProp.put(Jdbc4Connection.GOLDILOCKS_PROP_NAME_USER, aUser);
        sProp.put(Jdbc4Connection.GOLDILOCKS_PROP_NAME_PASSWORD, aPassword);
        return new GoldilocksConnection(mServerName, mPortNo, mDBName, sProp, this);
    }
}
