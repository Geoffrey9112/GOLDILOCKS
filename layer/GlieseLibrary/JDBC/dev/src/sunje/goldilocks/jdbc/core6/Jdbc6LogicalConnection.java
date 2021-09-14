package sunje.goldilocks.jdbc.core6;

import java.sql.Array;
import java.sql.Blob;
import java.sql.Clob;
import java.sql.NClob;
import java.sql.SQLClientInfoException;
import java.sql.SQLException;
import java.sql.SQLXML;
import java.util.Properties;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4LogicalConnection;
import sunje.goldilocks.jdbc.core4.Jdbc4PooledConnection;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Jdbc6LogicalConnection extends Jdbc4LogicalConnection
{
    public Jdbc6LogicalConnection(Jdbc4Connection aPhysicalConnection, Jdbc4PooledConnection aPooledConnection)
    {
        super(aPhysicalConnection, aPooledConnection);
    }

    public boolean isWrapperFor(Class<?> aClass) throws SQLException
    {
        return aClass.isInstance(this) || aClass.isInstance(mPhysicalConnection);
    }

    public <T> T unwrap(Class<T> aClass) throws SQLException
    {
        if (aClass.isInstance(this))
        {
            return aClass.cast(this);
        }
        else if (aClass.isInstance(mPhysicalConnection))
        {
            return aClass.cast(mPhysicalConnection);
        }
        ErrorMgr.raise(ErrorMgr.INVALID_WRAPPER_TYPE, aClass.toString());
        return null;
    }

    public Array createArrayOf(String aTypeName, Object[] aElements) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.createArrayOf(aTypeName, aElements);
    }

    public Blob createBlob() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.createBlob();
    }

    public Clob createClob() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.createClob();
    }

    public NClob createNClob() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.createNClob();
    }

    public SQLXML createSQLXML() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.createSQLXML();
    }

    public Properties getClientInfo() throws SQLException
    {
        return mPhysicalConnection.getClientInfo();
    }

    public String getClientInfo(String aName) throws SQLException
    {
        return mPhysicalConnection.getClientInfo(aName);
    }

    public boolean isValid(int aTimeout) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.isValid(aTimeout);
    }

    public void setClientInfo(Properties aClientInfo) throws SQLClientInfoException
    {
        mPhysicalConnection.setClientInfo(aClientInfo);
    }

    public void setClientInfo(String aName, String aValue)
            throws SQLClientInfoException
    {
        mPhysicalConnection.setClientInfo(aName, aValue);
    }
}
