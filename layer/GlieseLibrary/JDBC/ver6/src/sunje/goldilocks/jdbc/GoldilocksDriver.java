package sunje.goldilocks.jdbc;

import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

import sunje.goldilocks.jdbc.core4.Jdbc4CommonDataSource;
import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4Driver;

public class GoldilocksDriver extends Jdbc4Driver
{
    public static final Jdbc4Driver DRIVER_SINGLETON = new sunje.goldilocks.jdbc.GoldilocksDriver();
    
    static
    {
        try
        {
            DriverManager.registerDriver(DRIVER_SINGLETON);
        }
        catch (SQLException sException)
        {
        }
        
        Version6Specific.initialize();
    }
    
    static final int JDBC_MAJOR_VERSION = 4;
    static final int JDBC_MINOR_VERSION = 0;
    static final String JDK_VERSION = "JDK1.6";
    
    /*
     * main method for JAR execution
     */
    public static void main(String[] aArgs)
    {
        System.out.println(GOLDILOCKS_JDBC_DRIVER_NAME + " " + GOLDILOCKS_JDBC_DRIVER_VERSION + ", JDBC" + DRIVER_SINGLETON.getJDBCMajorVersion() + "." + DRIVER_SINGLETON.getJDBCMinorVersion() + " compiled with " + DRIVER_SINGLETON.getJDKVersion());
    }
    
    public int getJDBCMajorVersion()
    {
        return JDBC_MAJOR_VERSION;
    }
    
    public int getJDBCMinorVersion()
    {
        return JDBC_MINOR_VERSION;
    }
    
    public String getJDKVersion()
    {
        return JDK_VERSION;
    }
    
    public Jdbc4Connection createConcreteConnection(String aHost, int aPort, String aDBName, Properties aProp, Jdbc4CommonDataSource aFrom) throws SQLException
    {
        return new GoldilocksConnection(aHost, aPort, aDBName, aProp, aFrom);
    }
}
