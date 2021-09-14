package sunje.goldilocks.jdbc;

import java.io.InputStream;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4ResultSet;
import sunje.goldilocks.jdbc.core6.Jdbc6PreparedStatement;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class GoldilocksPreparedStatement extends Jdbc6PreparedStatement
{
    public GoldilocksPreparedStatement(Jdbc4Connection aConnection, String aSql, int aType, int aConcurrency, int aHoldability) throws SQLException
    {
        super(aConnection, aSql, aType, aConcurrency, aHoldability);
    }
    
    protected Jdbc4ResultSet createConcreteResultSet(int aResultSetType) throws SQLException
    {
        return new GoldilocksResultSet(this, aResultSetType);
    }
    
    @Deprecated
    public void setUnicodeStream(int aColNum, InputStream aValue, int aLength)
            throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "UnicodeStream type");
    }
}
