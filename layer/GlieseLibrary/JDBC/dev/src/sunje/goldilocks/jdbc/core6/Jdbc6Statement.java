package sunje.goldilocks.jdbc.core6;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4Statement;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Jdbc6Statement extends Jdbc4Statement
{
    public Jdbc6Statement(Jdbc4Connection aConnection, int aResultSetType, int aResultSetConcurrency, int aResultSetHoldability)
    {
        super(aConnection, aResultSetType, aResultSetConcurrency, aResultSetHoldability);
    }

    protected byte[] getBytes(String aSql)
    {
        return aSql.getBytes(mCharset);
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

    public boolean isClosed() throws SQLException
    {
        mLogger.logTrace();
        return mClosed;
    }

    public boolean isPoolable() throws SQLException
    {
        mLogger.logTrace();
        return false;
    }
    
    public void setPoolable(boolean aPoolable) throws SQLException
    {
        mLogger.logTrace();
        // 아직 지원하지 않는다.
    }
}
