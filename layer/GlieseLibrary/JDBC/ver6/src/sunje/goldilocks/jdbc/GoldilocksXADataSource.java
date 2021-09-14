package sunje.goldilocks.jdbc;

import java.sql.SQLException;

import javax.sql.XAConnection;
import javax.sql.XADataSource;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;

public class GoldilocksXADataSource extends GoldilocksConnectionPoolDataSource implements XADataSource
{
    private static final long serialVersionUID = 1934268434526707272L;

    static
    {
        Version6Specific.initialize();
    }

    public GoldilocksXADataSource()
    {
        super();
    }
    
    public XAConnection getXAConnection() throws SQLException
    {
        return new GoldilocksXAConnection((Jdbc4Connection)getPhysicalConnection());
    }

    public XAConnection getXAConnection(String aUser, String aPassword)
            throws SQLException
    {
        return new GoldilocksXAConnection((Jdbc4Connection)getPhysicalConnection(aUser, aPassword));
    }
    
    public String toString()
    {
        StringBuffer sBuf = new StringBuffer("GOLDILOCKS JDBC XADataSource: ");
        sBuf.append(getSpec());
        return sBuf.toString();
    }
}
