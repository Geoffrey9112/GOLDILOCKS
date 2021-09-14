package sunje.goldilocks.jdbc;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4DatabaseMetaData;

public class GoldilocksDatabaseMetaData extends Jdbc4DatabaseMetaData
{
    public GoldilocksDatabaseMetaData(Jdbc4Connection aConnection)
    {
        super(aConnection);
    }
    
    public int getJDBCMajorVersion() throws SQLException
    {
        mLogger.logTrace();
        return GoldilocksDriver.JDBC_MAJOR_VERSION;
    }

    public int getJDBCMinorVersion() throws SQLException
    {
        mLogger.logTrace();
        return GoldilocksDriver.JDBC_MINOR_VERSION;
    }
}
