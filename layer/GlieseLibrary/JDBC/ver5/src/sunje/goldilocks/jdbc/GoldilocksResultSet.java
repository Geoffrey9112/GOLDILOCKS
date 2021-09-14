package sunje.goldilocks.jdbc;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.Jdbc4ResultSet;
import sunje.goldilocks.jdbc.core4.Jdbc4Statement;

public class GoldilocksResultSet extends Jdbc4ResultSet
{
    protected GoldilocksResultSet(Jdbc4Statement aStatement, int aResultSetType) throws SQLException
    {
        super(aStatement, aResultSetType);
    }
    
    public synchronized GoldilocksRowId getRowId(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);        
        return (GoldilocksRowId)mColumns[aColumnIndex - 1].getRowId();
    }

    public synchronized GoldilocksRowId getRowId(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return (GoldilocksRowId)getColumn(aColumnLabel).getRowId();
    }
}
