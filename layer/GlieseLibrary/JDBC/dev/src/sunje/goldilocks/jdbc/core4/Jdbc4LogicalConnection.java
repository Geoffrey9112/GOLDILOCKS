package sunje.goldilocks.jdbc.core4;

import java.sql.CallableStatement;
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.SQLWarning;
import java.sql.Savepoint;
import java.sql.Statement;
import java.sql.Struct;
import java.util.Map;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Jdbc4LogicalConnection implements Connection
{
    protected Jdbc4Connection mPhysicalConnection;
    protected Jdbc4PooledConnection mPooledConnection;
    protected boolean mClosed;
    
    public Jdbc4LogicalConnection(Jdbc4Connection aPhysicalConnection, Jdbc4PooledConnection aPooledConnection)
    {
        mPhysicalConnection = aPhysicalConnection;
        mPhysicalConnection.setHandleForClient(this);
        mPooledConnection = aPooledConnection;
    }
    
    public Jdbc4Connection getPhysicalConnection()
    {
        return mPhysicalConnection;
    }
    
    void unsetClosed()
    {
        mClosed = false;
    }
    
    void setClosed()
    {
        mClosed = true;
    }
    
    protected void checkClosed() throws SQLException
    {
        if (mClosed)
        {
            ErrorMgr.raise(ErrorMgr.CONNECTION_CLOSED);
        }
    }
    
    public void close() throws SQLException
    {
        if (!mClosed)
        {
            setClosed();
            mPooledConnection.notifyLogicalConnectionClosed();
        }
    }

    public boolean isClosed() throws SQLException
    {
        return mClosed;
    }

    public void clearWarnings() throws SQLException
    {
        mPhysicalConnection.clearWarnings();        
    }

    public void commit() throws SQLException
    {
        checkClosed();
        mPhysicalConnection.commit();        
    }

    public Statement createStatement() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.createStatement();
    }

    public Statement createStatement(int aResultSetType, int aResultSetConcurrency) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.createStatement(aResultSetType, aResultSetConcurrency);
    }

    public Statement createStatement(int aResultSetType,
                                     int aResultSetConcurrency,
                                     int aResultSetHoldability) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.createStatement(aResultSetType, aResultSetConcurrency, aResultSetHoldability);
    }

    public Struct createStruct(String aTypeName, Object[] aAttributes) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.createStruct(aTypeName, aAttributes);
    }

    public boolean getAutoCommit() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.getAutoCommit();
    }

    public String getCatalog() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.getCatalog();
    }

    public int getHoldability() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.getHoldability();
    }

    public DatabaseMetaData getMetaData() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.getMetaData();
    }

    public int getTransactionIsolation() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.getTransactionIsolation();
    }

    public SQLWarning getWarnings() throws SQLException
    {
        return mPhysicalConnection.getWarnings();
    }

    public boolean isReadOnly() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.isReadOnly();
    }

    public String nativeSQL(String aSql) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.nativeSQL(aSql);
    }

    public CallableStatement prepareCall(String aSql) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.prepareCall(aSql);
    }

    public CallableStatement prepareCall(String aSql, int aResultSetType, int aResultSetConcurrency) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.prepareCall(aSql, aResultSetType, aResultSetConcurrency);
    }

    public CallableStatement prepareCall(String aSql,
                                         int aResultSetType,
                                         int aResultSetConcurrency,
                                         int aResultSetHoldability) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.prepareCall(aSql,
                                               aResultSetType,
                                               aResultSetConcurrency,
                                               aResultSetHoldability);
    }

    public PreparedStatement prepareStatement(String aSql) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.prepareStatement(aSql);
    }

    public PreparedStatement prepareStatement(String aSql, int aAutoGeneratedKeys)
            throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.prepareStatement(aSql, aAutoGeneratedKeys);
    }

    public PreparedStatement prepareStatement(String aSql, int aResultSetType, int aResultSetConcurrency)
            throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.prepareStatement(aSql, aResultSetType, aResultSetConcurrency);
    }

    public PreparedStatement prepareStatement(String aSql,
                                              int aResultSetType,
                                              int aResultSetConcurrency,
                                              int aResultSetHoldability) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.prepareStatement(aSql,
                                                    aResultSetType,
                                                    aResultSetConcurrency,
                                                    aResultSetHoldability);
    }

    public PreparedStatement prepareStatement(String aSql, int[] aColumnIndexes)
            throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.prepareStatement(aSql, aColumnIndexes);
    }

    public PreparedStatement prepareStatement(String aSql, String[] aColumnNames)
            throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.prepareStatement(aSql, aColumnNames);
    }

    public void releaseSavepoint(Savepoint aSavepoint) throws SQLException
    {
        checkClosed();
        mPhysicalConnection.releaseSavepoint(aSavepoint);
    }

    public void rollback() throws SQLException
    {
        checkClosed();
        mPhysicalConnection.rollback();
    }

    public void rollback(Savepoint aSavepoint) throws SQLException
    {
        checkClosed();
        mPhysicalConnection.rollback(aSavepoint);
    }

    public void setAutoCommit(boolean aAutoCommit) throws SQLException
    {
        checkClosed();
        mPhysicalConnection.setAutoCommit(aAutoCommit);
    }

    public void setCatalog(String aCatalog) throws SQLException
    {
        checkClosed();
        mPhysicalConnection.setCatalog(aCatalog);
    }

    public void setHoldability(int aHoldability) throws SQLException
    {
        checkClosed();
        mPhysicalConnection.setHoldability(aHoldability);
    }

    public void setReadOnly(boolean aReadOnly) throws SQLException
    {
        checkClosed();
        mPhysicalConnection.setReadOnly(aReadOnly);        
    }

    public Savepoint setSavepoint() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.setSavepoint();
    }

    public Savepoint setSavepoint(String aSavepointName) throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.setSavepoint(aSavepointName);
    }

    public void setTransactionIsolation(int aIsolationLevel) throws SQLException
    {
        checkClosed();
        mPhysicalConnection.setTransactionIsolation(aIsolationLevel);
    }
    
    public Map<String, Class<?>> getTypeMap() throws SQLException
    {
        checkClosed();
        return mPhysicalConnection.getTypeMap();
    }

    public void setTypeMap(Map<String, Class<?>> aMap) throws SQLException
    {
        checkClosed();
        mPhysicalConnection.setTypeMap(aMap);
    }
}
