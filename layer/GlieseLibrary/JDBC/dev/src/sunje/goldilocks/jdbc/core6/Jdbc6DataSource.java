package sunje.goldilocks.jdbc.core6;

import java.sql.SQLException;

import javax.sql.DataSource;

import sunje.goldilocks.jdbc.core4.Jdbc4CommonDataSource;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Jdbc6DataSource extends Jdbc4CommonDataSource implements DataSource
{
    public <T> T unwrap(Class<T> aClass) throws SQLException
    {
        if (!aClass.isInstance(this))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_WRAPPER_TYPE, aClass.toString());
        }
        return aClass.cast(this);
    }

    public boolean isWrapperFor(Class<?> aClass) throws SQLException
    {
        return aClass.isInstance(this);
    }
}
