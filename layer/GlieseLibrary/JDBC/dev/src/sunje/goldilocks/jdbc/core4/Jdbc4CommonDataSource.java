package sunje.goldilocks.jdbc.core4;

import java.io.PrintWriter;
import java.sql.SQLException;
import java.util.Properties;

import javax.naming.NamingException;
import javax.naming.Reference;
import javax.naming.Referenceable;
import javax.naming.StringRefAddr;

public abstract class Jdbc4CommonDataSource implements Referenceable
{
    protected int mLoginTimeout;
    protected PrintWriter mLogWriter;
    protected java.util.Date mCreationTime;
    protected Properties mProp;
    protected String mDataSourceName;
    protected String mServerName;
    protected String mDBName;
    protected String mNetworkProtocol;
    protected int mPortNo;
    protected String mDescription;

    public Jdbc4CommonDataSource()
    {
        mLoginTimeout = 0;
        mLogWriter = null;
        mCreationTime = new java.util.Date();
        mProp = new Properties();
    }

    public PrintWriter getLogWriter() throws SQLException
    {
        return mLogWriter;
    }

    public void setLogWriter(PrintWriter aPrintWriter) throws SQLException
    {
        mLogWriter = aPrintWriter;
    }

    public void setLoginTimeout(int aTimeout) throws SQLException
    {
        mLoginTimeout = aTimeout;        
    }

    public int getLoginTimeout() throws SQLException
    {
        return mLoginTimeout;
    }
    
    /*
     * JBoss에서 XADataSource의 setURL() 메소드를 필요로 한다.
     */
    public void setURL(String aURL) throws SQLException
    {
        String[] sURLResult = Jdbc4Driver.parseURL(aURL, mProp);
        setServerName(sURLResult[0]);
        setPortNumber(sURLResult[1]);
        setDatabaseName(sURLResult[2]);
    }
    
    /*
     * 일부 툴에서는 setURL이 아니라 setUrl을 사용하기도 한다.
     */
    public void setUrl(String aUrl) throws SQLException
    {
        setURL(aUrl);
    }
    
    public void setDataSourceName(String aDataSourceName)
    {
        mDataSourceName = aDataSourceName;
    }
    
    public void setServerName(String aServerName)
    {
        mServerName = aServerName;
    }
    
    public void setDatabaseName(String aDBName)
    {
        mDBName = aDBName;
    }
    
    public void setNetworkProtocol(String aProtocol)
    {
        mNetworkProtocol = aProtocol;
    }
    
    public void setUser(String aUser)
    {
        mProp.put(Jdbc4Connection.GOLDILOCKS_PROP_NAME_USER, aUser);
    }
    
    public void setPassword(String aPassword)
    {
        mProp.put(Jdbc4Connection.GOLDILOCKS_PROP_NAME_PASSWORD, aPassword);
    }
    
    public void setPortNumber(int aPort)
    {
        mPortNo = aPort;
    }
    
    public void setPortNumber(String aPort)
    {
        try
        {
            mPortNo = Integer.parseInt(aPort);
        }
        catch (NumberFormatException sException)
        {
        }
    }
    
    public void setRoleName(String aRoleName)
    {
        mProp.put(Jdbc4Connection.GOLDILOCKS_PROP_NAME_ROLE, aRoleName);
    }
    
    public void setDescription(String aDescription)
    {
        mDescription = aDescription;
    }
    
    public void setConnectionProperties(Properties aProps)
    {
        mProp.putAll(aProps);
        
        String sValue = mProp.getProperty(Jdbc4Connection.GOLDILOCKS_GLOBAL_PROP_NAME_LOGGER);
        
        if (sValue != null && Jdbc4Connection.GOLDILOCKS_GLOBAL_PROP_VALUE_LOGGER_CONSOLE.equalsIgnoreCase(sValue))
        {
            try
            {
                setLogWriter(new PrintWriter(System.out));
            }
            catch (SQLException sException)
            {
                // exception을 무시한다.
            }
        }
    }
    
    public void setLogTarget(String aTarget)
    {
        /*
         * 현재는 console만 지원한다.
         */
        if (Jdbc4Connection.GOLDILOCKS_GLOBAL_PROP_VALUE_LOGGER_CONSOLE.equalsIgnoreCase(aTarget))
        {
            try
            {
                setLogWriter(new PrintWriter(System.out));
            }
            catch (SQLException sException)
            {
                // 예외가 발생하면 무시한다.
            }
        }
    }
    
    public void setTraceLog(String aMode)
    {
        if (Jdbc4Connection.GOLDILOCKS_PROP_VALUE_LOG_ON.equalsIgnoreCase(aMode))
        {
            mProp.put(Jdbc4Connection.GOLDILOCKS_PROP_NAME_TRACE_LOG, Jdbc4Connection.GOLDILOCKS_PROP_VALUE_LOG_ON);
        }
    }
    
    public void setProtocolLog(String aMode)
    {
        if (Jdbc4Connection.GOLDILOCKS_PROP_VALUE_LOG_ON.equalsIgnoreCase(aMode))
        {
            mProp.put(Jdbc4Connection.GOLDILOCKS_PROP_NAME_PROTOCOL_LOG, Jdbc4Connection.GOLDILOCKS_PROP_VALUE_LOG_ON);
        }
    }

    public void setQueryLog(String aMode)
    {
        if (Jdbc4Connection.GOLDILOCKS_PROP_VALUE_LOG_ON.equalsIgnoreCase(aMode))
        {
            mProp.put(Jdbc4Connection.GOLDILOCKS_PROP_NAME_QUERY_LOG, Jdbc4Connection.GOLDILOCKS_PROP_VALUE_LOG_ON);
        }
    }
    
    public Properties getConnectionProperties()
    {
        return mProp;
    }

    public String getURL()
    {
        return Jdbc4Driver.makeURL(mServerName, mPortNo, mDBName);
    }
    
    public String getServerName()
    {
        return mServerName;
    }
    
    public int getPortNumber()
    {
        return mPortNo;
    }
    
    public String getDatabaseName()
    {
        return mDBName;
    }
    
    public String getSpec()
    {
        StringBuffer sBuf = new StringBuffer(110);
        sBuf.append("[dataSource=").append(mDataSourceName).append("] ")
          .append("[serverName=").append(mServerName).append("] ")
          .append("[portNumber=").append(mPortNo).append("] ")
          .append("[databaseName=").append(mDBName).append("] ")
          .append("[user=").append(mProp.get(Jdbc4Connection.GOLDILOCKS_PROP_NAME_USER)).append("] ")
          .append("[password=").append(mProp.get(Jdbc4Connection.GOLDILOCKS_PROP_NAME_PASSWORD)).append("] ")
          .append("[description=").append(mDescription).append("] ")
          .append("[creation time=").append(mCreationTime.toString()).append(']');
        return sBuf.toString();
    }
    
    public String toString()
    {
        StringBuffer sBuf = new StringBuffer("GOLDILOCKS DataSource: ");
        sBuf.append(getSpec());
        return sBuf.toString();
    }
    
    public Reference getReference() throws NamingException
    {
        Reference sReference = new Reference(getClass().getName());
        
        sReference.add(new StringRefAddr("serverName", mServerName));
        sReference.add(new StringRefAddr("portNumber", String.valueOf(mPortNo)));
        sReference.add(new StringRefAddr("databaseName", mDBName));
        sReference.add(new StringRefAddr("user", mProp.get(Jdbc4Connection.GOLDILOCKS_PROP_NAME_USER).toString()));
        sReference.add(new StringRefAddr("password", mProp.get(Jdbc4Connection.GOLDILOCKS_PROP_NAME_PASSWORD).toString()));
        sReference.add(new StringRefAddr("description", mDescription));
        sReference.add(new StringRefAddr("dataSourceName", mDataSourceName));
        sReference.add(new StringRefAddr("loginTimeout", String.valueOf(mLoginTimeout)));
        return sReference;
    }
}
