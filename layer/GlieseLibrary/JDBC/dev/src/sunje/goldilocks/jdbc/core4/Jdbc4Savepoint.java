package sunje.goldilocks.jdbc.core4;

import java.sql.SQLException;
import java.sql.Savepoint;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Jdbc4Savepoint implements Savepoint
{
    private int mId = -1;
    private String mName = null;
    private boolean mNamed;
    
    public Jdbc4Savepoint(int aId)
    {
        mId = aId;
        mNamed = false;
    }

    public Jdbc4Savepoint(String aName)
    {
        mName = aName;
        mNamed = true;
    }
    
    public int getSavepointId() throws SQLException
    {
        if (mNamed)
        {
            ErrorMgr.raise(ErrorMgr.SAVEPOINT_ERROR, "Named savepoint has no id");
        }
        return mId;
    }

    public String getSavepointName() throws SQLException
    {
        if (!mNamed)
        {
            ErrorMgr.raise(ErrorMgr.SAVEPOINT_ERROR, "Unnamed savepoint has no name");
        }
        return mName;
    }
    
    String getQuerySavepointName()
    {
        if (mNamed)
        {
            return mName;
        }
        return "UNNAMED_SAVEPOINT_" + mId;
    }
}
