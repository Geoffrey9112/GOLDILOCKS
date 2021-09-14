package sunje.goldilocks.jdbc;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.Jdbc4Statement;
import sunje.goldilocks.jdbc.core6.Jdbc6ResultSet;

public class GoldilocksResultSet extends Jdbc6ResultSet
{
    protected GoldilocksResultSet(Jdbc4Statement aStatement, int aResultSetType) throws SQLException
    {
        super(aStatement, aResultSetType);
    }
}
