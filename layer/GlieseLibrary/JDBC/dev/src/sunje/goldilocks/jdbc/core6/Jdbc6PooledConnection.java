package sunje.goldilocks.jdbc.core6;

import javax.sql.StatementEventListener;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4PooledConnection;

public abstract class Jdbc6PooledConnection extends Jdbc4PooledConnection
{
    /*
     * statement event는 statement pooling을 지원하지 않기 때문에 구현하지 않는다.
     */
    public Jdbc6PooledConnection(Jdbc4Connection aPhysicalConnection)
    {
        super(aPhysicalConnection);
    }
    
    public void addStatementEventListener(StatementEventListener aListener)
    {
    }

    public void removeStatementEventListener(StatementEventListener aListener)
    {
    }
}
