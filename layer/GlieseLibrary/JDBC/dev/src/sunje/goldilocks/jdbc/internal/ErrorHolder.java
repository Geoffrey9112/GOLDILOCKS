package sunje.goldilocks.jdbc.internal;

import java.sql.SQLException;
import java.sql.SQLWarning;

public abstract class ErrorHolder
{
    protected SQLException mError;
    protected SQLWarning mWarning;

    protected void clearWarning()
    {
        mWarning = null;
    }
    
    public void testError() throws SQLException
    {
        if (mError != null)
        {
            SQLException sErrorToThrow = mError;
            mError = null;
            throw sErrorToThrow;
        }
    }
    
    public void addWarning(SQLWarning aWarning)
    {
        if (mWarning == null)
        {
            mWarning = aWarning;
        }
        else
        {
            mWarning.setNextWarning(aWarning);
        }
    }
    
    public void addError(SQLException aError)
    {
        if (mError == null)
        {
            mError = aError;
        }
        else
        {
            mError.setNextException(aError);
        }
    }
}
