package sunje.goldilocks.jdbc.core4;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.LinkedList;

import javax.sql.ConnectionEvent;
import javax.sql.ConnectionEventListener;
import javax.sql.PooledConnection;

import sunje.goldilocks.jdbc.cm.LinkFailureListener;

public abstract class Jdbc4PooledConnection implements PooledConnection, LinkFailureListener
{
    protected Jdbc4LogicalConnection mLogicalConnection;
    private LinkedList<ConnectionEventListener> mConnEventListener;
    
    public Jdbc4PooledConnection(Jdbc4Connection aPhysicalConnection)
    {
        aPhysicalConnection.getChannel().setLinkFailureListener(this);
        mLogicalConnection = createLogicalConnection(aPhysicalConnection);
        mConnEventListener = new LinkedList<ConnectionEventListener>();
    }

    protected abstract Jdbc4LogicalConnection createLogicalConnection(Jdbc4Connection aPhysicalConnection);

    protected void notifyLogicalConnectionClosed()
    {
        for (ConnectionEventListener sListener : mConnEventListener)
        {
            sListener.connectionClosed(new ConnectionEvent(this));
        }
    }
    
    /**************************************************************
     * LinkFailureListener Interface Methods
     **************************************************************/
    public void notifyLinkFailure(SQLException aError)
    {
        for (ConnectionEventListener sListener : mConnEventListener)
        {
            sListener.connectionErrorOccurred(new ConnectionEvent(this, aError));
        }
    }

    public void close() throws SQLException
    {
        mLogicalConnection.setClosed();
        mLogicalConnection.getPhysicalConnection().close();
    }

    public Connection getConnection() throws SQLException
    {
        if (mLogicalConnection.getPhysicalConnection().isClosed())
        {
            mLogicalConnection.setClosed();
        }
        else
        {
            mLogicalConnection.unsetClosed();
        }
        return mLogicalConnection;
    }
    
    public void addConnectionEventListener(ConnectionEventListener aListener)
    {
        mConnEventListener.add(aListener);
    }

    public void removeConnectionEventListener(ConnectionEventListener aListener)
    {
        mConnEventListener.remove(aListener);        
    }
}
