package sunje.goldilocks.jdbc.core4;

import java.net.URI;
import java.net.URISyntaxException;
import java.sql.Connection;
import java.sql.Driver;
import java.sql.DriverPropertyInfo;
import java.sql.SQLException;
import java.util.LinkedList;
import java.util.Properties;
import java.util.StringTokenizer;

import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Jdbc4Driver implements Driver
{
    protected static final String GOLDILOCKS_JDBC_DRIVER_NAME = "GOLDILOCKS JDBC Driver";
    protected static final int GOLDILOCKS_DRIVER_MAJOR_VERSION = 1;
    protected static final int GOLDILOCKS_DRIVER_MINOR_VERSION = 0;
    protected static final String GOLDILOCKS_JDBC_DRIVER_VERSION =
            GOLDILOCKS_DRIVER_MAJOR_VERSION + "." +
            GOLDILOCKS_DRIVER_MINOR_VERSION +
            " Procotol-" + 
            Protocol.PROTOCOL_MAJOR_VERSION + "." +
            Protocol.PROTOCOL_MINOR_VERSION + "." +
            Protocol.PROTOCOL_PATCH_VERSION;
    protected static final String GOLDILOCKS_JDBC_CONNECT_URL_JDBC_SYMBOL = "jdbc:";
    protected static final String GOLDILOCKS_JDBC_CONNECT_URL_DBMS_NAME = "goldilocks";
    protected static final String GOLDILOCKS_JDBC_CONNECT_URL_PATH_DELIMITER = "/";
    protected static final String GOLDILOCKS_JDBC_CONNECT_URL_DBNAME_PATTERN = "[a-zA-Z0-9_]+";
    protected static final String GOLDILOCKS_ACCEPTABLE_JDBC_CONNECT_URL_PREFIX = 
            GOLDILOCKS_JDBC_CONNECT_URL_JDBC_SYMBOL + GOLDILOCKS_JDBC_CONNECT_URL_DBMS_NAME + ":" + GOLDILOCKS_JDBC_CONNECT_URL_PATH_DELIMITER + GOLDILOCKS_JDBC_CONNECT_URL_PATH_DELIMITER;
    protected static final String GOLDILOCKS_JDBC_CONNECT_URL_QUERY_PARAM_DELIMITER = "&";
    protected static final String GOLDILOCKS_JDBC_CONNECT_URL_QUERY_PARAM_KEY_VALUE_DELIMITER = "=";
    
    private static LinkedList<ConnectionProperty> mProps = new LinkedList<ConnectionProperty>();
    
    public static String makeURL(String aHost, int aPort, String aDBName)
    {
        return GOLDILOCKS_JDBC_CONNECT_URL_JDBC_SYMBOL + GOLDILOCKS_JDBC_CONNECT_URL_DBMS_NAME + ":" +
                GOLDILOCKS_JDBC_CONNECT_URL_PATH_DELIMITER + GOLDILOCKS_JDBC_CONNECT_URL_PATH_DELIMITER +
                aHost + ":" + aPort + GOLDILOCKS_JDBC_CONNECT_URL_PATH_DELIMITER + aDBName;
    }
    
    /*
     * 이 메소드의 리턴값은 {serverName(String), portNumber(Integer), databaseName(String)} 이다.
     */
    public static String[] parseURL(String aURL, Properties aInfo) throws SQLException
    {
        // 예) jdbc:goldilocks://127.0.0.1:1234/test
        // 예2) direct attach 경우 - jdbc:goldilocks:da/test
        // 규칙) jdbc:goldilocks://[ip_address]:[port_no]/[db_name]
        // IPV4, IPV6 모두 고려해야 함.
 
        // url test1: null이 아니어야 하고 길이가 최소 5이상이 맞는가?
        if (aURL == null || aURL.length() <= GOLDILOCKS_JDBC_CONNECT_URL_JDBC_SYMBOL.length())
        {
            ErrorMgr.raise(ErrorMgr.INVALID_CONNECT_URL, "1", aURL);
        }

        // url test2: jdbc:로 시작하는가?
        if (!aURL.startsWith(GOLDILOCKS_JDBC_CONNECT_URL_JDBC_SYMBOL))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_CONNECT_URL, "2", aURL);
        }
        
        String sURLBody = aURL.substring(GOLDILOCKS_JDBC_CONNECT_URL_JDBC_SYMBOL.length());
        
        URI sURI = null;
        
        try
        {
            sURI = new URI(sURLBody);
        }
        catch (URISyntaxException sException)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_CONNECT_URL, "3", aURL);
        }

        // url test3: jdbc:다음에 goldilocks: 인가?
        if (!GOLDILOCKS_JDBC_CONNECT_URL_DBMS_NAME.equals(sURI.getScheme()))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_CONNECT_URL, "4", aURL);
        }
        // url test4: db 이름이 제대로 온 것인가?
        if (sURI.getPath() == null || !sURI.getPath().startsWith(GOLDILOCKS_JDBC_CONNECT_URL_PATH_DELIMITER))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_CONNECT_URL, "5", aURL);
        }

        String sHost = sURI.getHost();
        String sDBName = sURI.getPath().substring(GOLDILOCKS_JDBC_CONNECT_URL_PATH_DELIMITER.length());
        int sPort = sURI.getPort();
        
        // url test5: host 이름이 제대로 왔는가?
        if (sHost == null)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_CONNECT_URL, "6", aURL);
        }

        // url test6: db 이름 제대로 왔는가?
        if (sDBName == null)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_CONNECT_URL, "7", aURL);
        }
        if (!sDBName.matches(GOLDILOCKS_JDBC_CONNECT_URL_DBNAME_PATTERN))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_CONNECT_URL, "8", aURL);
        }

        // url test7: port 번호가 제대로 왔는가?
        if (sPort <= 0)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_CONNECT_URL, "9", aURL);
        }
        
        String sParams = sURI.getQuery();
        if (sParams != null)
        {
            try
            {
                StringTokenizer sParamTokens = new StringTokenizer(sParams, GOLDILOCKS_JDBC_CONNECT_URL_QUERY_PARAM_DELIMITER);
                while (sParamTokens.hasMoreTokens())
                {
                    String sParam = sParamTokens.nextToken();
                    int sEqualIndex = sParam.indexOf(GOLDILOCKS_JDBC_CONNECT_URL_QUERY_PARAM_KEY_VALUE_DELIMITER);
                    aInfo.put(sParam.substring(0, sEqualIndex).trim(), sParam.substring(sEqualIndex + 1).trim());
                }
            }
            catch (Exception sException)
            {
                ErrorMgr.raise(ErrorMgr.INVALID_CONNECT_URL, "10", aURL);
            }
        }
        
        String[] sResult = new String[3];
        sResult[0] = sHost;
        sResult[1] = String.valueOf(sPort);
        sResult[2] = sDBName;
        
        return sResult;
    }
    
    static void registerProperty(ConnectionProperty aProp)
    {
        mProps.add(aProp);
    }
    
    public Jdbc4Driver()
    {
        // tool에서 Driver를 직접 생성하는 경우가 있으므로 public으로 선언함.
    }
    
    public abstract int getJDBCMajorVersion();
    
    public abstract int getJDBCMinorVersion();
    
    public abstract String getJDKVersion();

    public abstract Jdbc4Connection createConcreteConnection(String aHost, int aPort, String aDBName, Properties aProp, Jdbc4CommonDataSource aFrom) throws SQLException;
    
    public boolean acceptsURL(String aURL) throws SQLException
    {
        if (aURL != null && aURL.startsWith(GOLDILOCKS_ACCEPTABLE_JDBC_CONNECT_URL_PREFIX))
        {
            return true;
        }
        return false;
    }

    public Connection connect(String aURL, Properties aInfo) throws SQLException
    {
        /*
         * aURL이 jdbc:goldilocks로 시작하지 않으면 exception을 던질게 아니라 null을 리턴해야 한다.
         */
        if (!acceptsURL(aURL))
        {
            return null;
        }

        String[] sURLResult = parseURL(aURL, aInfo);
        return createConcreteConnection(sURLResult[0], Integer.parseInt(sURLResult[1]), sURLResult[2], aInfo, null);
    }

    public int getMajorVersion()
    {
        return GOLDILOCKS_DRIVER_MAJOR_VERSION;
    }

    public int getMinorVersion()
    {
        return GOLDILOCKS_DRIVER_MINOR_VERSION;
    }

    public DriverPropertyInfo[] getPropertyInfo(String aURL, Properties aInfo) throws SQLException
    {
        synchronized (mProps)
        {
            if (mProps.isEmpty())
            {
                Jdbc4Connection.registerProperties();
            }
        }
        
        LinkedList<DriverPropertyInfo> sProps = new LinkedList<DriverPropertyInfo>();
        
        if (aInfo == null)
        {
            aInfo = new Properties();
        }
        
        for (ConnectionProperty sRegisteredProp : mProps)
        {
            DriverPropertyInfo sProp = new DriverPropertyInfo(sRegisteredProp.mName, aInfo.getProperty(sRegisteredProp.mName));
            sProp.required = sRegisteredProp.mRequired;
            sProp.choices = sRegisteredProp.mChoices;
            sProp.description = sRegisteredProp.mDescription;
            sProps.add(sProp);
        }
        
        return sProps.toArray(new DriverPropertyInfo[0]);
    }

    public boolean jdbcCompliant()
    {
        return false;
    }
    
    public String getDriverName()
    {
        return GOLDILOCKS_JDBC_DRIVER_NAME;
    }
    
    public String getDriverVersion()
    {
        return GOLDILOCKS_JDBC_DRIVER_VERSION;
    }
}
