package sunje.goldilocks.jdbc.core4;

import java.sql.SQLException;

import javax.transaction.xa.Xid;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Jdbc4Xid implements Xid
{
    private int mFormatId;
    private byte[] mGlobalTransId;
    private byte[] mBranchQualifier;
    
    public Jdbc4Xid(int aFormatId, byte[] aGlobalTransIdSrc, byte[] aBranchQualifierSrc) throws SQLException
    {
        if (aGlobalTransIdSrc == null)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "Global transaction id is null");
        }
        if (aBranchQualifierSrc == null)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "Branch qualifier is null");
        }
        mFormatId = aFormatId;
        mGlobalTransId = new byte[aGlobalTransIdSrc.length];
        mBranchQualifier = new byte[aBranchQualifierSrc.length];
        System.arraycopy(aGlobalTransIdSrc, 0, mGlobalTransId, 0, aGlobalTransIdSrc.length);
        System.arraycopy(aBranchQualifierSrc, 0, mBranchQualifier, 0, aBranchQualifierSrc.length);
    }
    
    public int getFormatId()
    {
        return mFormatId;
    }

    public byte[] getGlobalTransactionId()
    {
        return mGlobalTransId;
    }

    public byte[] getBranchQualifier()
    {
        return mBranchQualifier;
    }
}
