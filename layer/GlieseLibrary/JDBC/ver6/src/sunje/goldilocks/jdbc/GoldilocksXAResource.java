package sunje.goldilocks.jdbc;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4XAResource;
import sunje.goldilocks.jdbc.core4.Jdbc4Xid;

public class GoldilocksXAResource extends Jdbc4XAResource
{
    public GoldilocksXAResource(Jdbc4Connection aConnection)
    {
        super(aConnection);
    }
    
    protected Jdbc4Xid createConcreteXid(int aFormatId, byte[] aGlobalTransIdSrc, byte[] aBranchQualifierSrc) throws SQLException
    {
        return new GoldilocksXid(aFormatId, aGlobalTransIdSrc, aBranchQualifierSrc);
    }
}
