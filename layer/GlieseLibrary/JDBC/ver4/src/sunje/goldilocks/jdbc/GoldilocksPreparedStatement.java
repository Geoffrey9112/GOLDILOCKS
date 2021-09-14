package sunje.goldilocks.jdbc;

import java.io.InputStream;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4PreparedStatement;
import sunje.goldilocks.jdbc.core4.Jdbc4ResultSet;
import sunje.goldilocks.jdbc.dt.CodeColumn;
import sunje.goldilocks.jdbc.dt.Column;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class GoldilocksPreparedStatement extends Jdbc4PreparedStatement
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
    
    public synchronized void setRowId(int aColNum, GoldilocksRowId aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_ROWID, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setRowId(aValue);
        }
    }
}
