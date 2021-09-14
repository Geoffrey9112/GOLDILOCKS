package sunje.goldilocks.jdbc.core6;

import java.sql.Array;
import java.sql.Blob;
import java.sql.Clob;
import java.sql.NClob;
import java.sql.ResultSet;
import java.sql.SQLClientInfoException;
import java.sql.SQLException;
import java.sql.SQLXML;
import java.util.Properties;

import sunje.goldilocks.jdbc.core4.Jdbc4CommonDataSource;
import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Jdbc6Connection extends Jdbc4Connection
{
    private Properties mClientInfo;

    public Jdbc6Connection(String aHost, int aPort, String aDBName, Properties aProp, Jdbc4CommonDataSource aFrom) throws SQLException
    {
        super(aHost, aPort, aDBName, aProp, aFrom);
    }
    
    public boolean isWrapperFor(Class<?> aClass) throws SQLException
    {
        mLogger.logTrace();
        return aClass.isInstance(this);
    }

    public <T> T unwrap(Class<T> aClass) throws SQLException
    {
        mLogger.logTrace();
        if (!aClass.isInstance(this))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_WRAPPER_TYPE, aClass.toString());
        }
        return aClass.cast(this);
    }

    public Array createArrayOf(String aTypeName, Object[] aElements) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Array type");
        return null;
    }

    public Blob createBlob() throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Blob type");
        return null;
    }

    public Clob createClob() throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "Clob type");
        return null;
    }

    public NClob createNClob() throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "NClob type");
        return null;
    }

    public SQLXML createSQLXML() throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "XML type");
        return null;
    }
    
    public Properties getClientInfo() throws SQLException
    {
        mLogger.logTrace();
        if (mClientInfo == null)
        {
            return null;
        }
        synchronized (mClientInfo)
        {
            return (Properties)mClientInfo.clone();
        }
    }

    public String getClientInfo(String aName) throws SQLException
    {
        mLogger.logTrace();
        if (mClientInfo == null)
        {
            return null;
        }
        synchronized (mClientInfo)
        {
            return (String)mClientInfo.get(aName);
        }
    }

    public synchronized boolean isValid(int aTimeout) throws SQLException
    {
        mLogger.logTrace();
        if (isClosed())
        {
            return false;
        }
        
        verifyInternalStmt();
        try
        {
            int sOrgTimeout = mInternalStmt.getQueryTimeout();
            mInternalStmt.setQueryTimeout(aTimeout);
            executeInternalSql(INTERNAL_SQL_HEART_BEAT_CHECK);
            ResultSet sRs = mInternalStmt.getResultSet();
            sRs.close();            
            mInternalStmt.setQueryTimeout(sOrgTimeout);
            return true;
        }
        catch (SQLException sException)
        {
            return false;
        }
    }

    public void setClientInfo(Properties aClientInfo) throws SQLClientInfoException
    {
        mLogger.logTrace();
        mClientInfo = (Properties)aClientInfo.clone();
    }

    public void setClientInfo(String aName, String aValue) throws SQLClientInfoException
    {
        mLogger.logTrace();        
        if (mClientInfo == null)
        {
            mClientInfo = new Properties();
        }
        
        synchronized (mClientInfo)
        {
            mClientInfo.put(aName, aValue);
        }
    }
}
