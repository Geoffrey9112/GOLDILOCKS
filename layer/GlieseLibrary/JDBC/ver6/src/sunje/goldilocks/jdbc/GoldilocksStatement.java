package sunje.goldilocks.jdbc;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4ResultSet;
import sunje.goldilocks.jdbc.core6.Jdbc6Statement;

public class GoldilocksStatement extends Jdbc6Statement
{
    public GoldilocksStatement(Jdbc4Connection aConnection, int aResultSetType, int aResultSetConcurrency, int aResultSetHoldability)
    {
        super(aConnection, aResultSetType, aResultSetConcurrency, aResultSetHoldability);
    }
    
    protected Jdbc4ResultSet createConcreteResultSet(int aResultSetType) throws SQLException
    {
        return new GoldilocksResultSet(this, aResultSetType);
    }
}
