package sunje.goldilocks.tool.diff;

import java.sql.SQLException;

public class ErrorHandler
{
    protected SQLException mException;
    
    public void clearError()
    {
        mException = null;
    }
    
    public void addError(SQLException aException)
    {
        if (mException == null)
        {
            mException = aException;
        }
        else
        {
            mException.setNextException(aException);
        }
    }
    
    public void testError() throws SQLException
    {
        if (mException != null)
        {
            throw mException;
        }
    }
}
