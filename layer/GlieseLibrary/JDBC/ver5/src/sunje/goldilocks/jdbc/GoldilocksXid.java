package sunje.goldilocks.jdbc;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.Jdbc4Xid;

public class GoldilocksXid extends Jdbc4Xid
{
    public GoldilocksXid(int aFormatId, byte[] aGlobalTransIdSrc, byte[] aBranchQualifierSrc) throws SQLException
    {
        super(aFormatId, aGlobalTransIdSrc, aBranchQualifierSrc);
    }
}
