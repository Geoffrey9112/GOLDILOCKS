package sunje.goldilocks.jdbc.core4;

import java.sql.Connection;
import java.sql.SQLException;

import javax.sql.ConnectionPoolDataSource;

public abstract class Jdbc4ConnectionPoolDataSource extends Jdbc4CommonDataSource implements ConnectionPoolDataSource
{
    public Jdbc4ConnectionPoolDataSource()
    {
        super();
    }
    
    protected abstract Connection getPhysicalConnection() throws SQLException;
    protected abstract Connection getPhysicalConnection(String aUser, String aPassword) throws SQLException;

    public String toString()
    {
        StringBuffer sBuf = new StringBuffer("GOLDILOCKS JDBC ConnectionPoolDataSource: ");
        sBuf.append(getSpec());
        return sBuf.toString();
    }
}
