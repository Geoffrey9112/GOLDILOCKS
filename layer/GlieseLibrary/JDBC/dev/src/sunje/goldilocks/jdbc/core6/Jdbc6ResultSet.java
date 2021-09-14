package sunje.goldilocks.jdbc.core6;

import java.io.Reader;
import java.sql.NClob;
import java.sql.RowId;
import java.sql.SQLException;
import java.sql.SQLXML;

import sunje.goldilocks.jdbc.core4.Jdbc4ResultSet;
import sunje.goldilocks.jdbc.core4.Jdbc4Statement;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Jdbc6ResultSet extends Jdbc4ResultSet
{
    protected Jdbc6ResultSet(Jdbc4Statement aStatement, int aResultSetType) throws SQLException
    {
        super(aStatement, aResultSetType);
    }
    
    /**************************************************************
     * Wrapper interface methods
     **************************************************************/
    public boolean isWrapperFor(Class<?> aClass) throws SQLException
    {
        mLogger.logTrace();
        return aClass.isInstance(this);
    }

    public <T> T unwrap(Class<T> aClass) throws SQLException
    {
        mLogger.logTrace();
        if (!aClass.isInstance(this))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_WRAPPER_TYPE, aClass.toString());
        }
        return aClass.cast(this);
    }

    public synchronized Reader getNCharacterStream(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "National character type");
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return null;
    }

    public synchronized Reader getNCharacterStream(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "National character type");
        checkClosed();
        checkCursorPosition();
        return null;
    }

    public synchronized NClob getNClob(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "National character type");
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return null;
    }

    public synchronized NClob getNClob(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "National character type");
        checkClosed();
        checkCursorPosition();
        return null;
    }

    public synchronized String getNString(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "National character type");
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return null;
    }

    public synchronized String getNString(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "National character type");
        checkClosed();
        checkCursorPosition();
        return null;
    }

    public synchronized RowId getRowId(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);        
        return (RowId)mColumns[aColumnIndex - 1].getRowId();
    }

    public synchronized RowId getRowId(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        checkCursorPosition();
        return (RowId)getColumn(aColumnLabel).getRowId();
    }

    public synchronized SQLXML getSQLXML(int aColumnIndex) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "XML type");
        checkClosed();
        checkCursorPosition();
        checkColumnIndex(aColumnIndex);
        return null;
    }

    public synchronized SQLXML getSQLXML(String aColumnLabel) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "XML type");
        checkClosed();
        checkCursorPosition();
        return null;
    }

    public void updateNClob(int aColumnIndex, NClob aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNClob(aColumnIndex, aValue);
    }

    public void updateNClob(int aColumnIndex, Reader aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNClob(aColumnIndex, aValue, aLength);
    }

    public void updateNClob(int aColumnIndex, Reader aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNClob(aColumnIndex, aValue);
    }

    public void updateNClob(String aColumnLabel, NClob aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNClob(aColumnLabel, aValue);
    }

    public void updateNClob(String aColumnLabel, Reader aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNClob(aColumnLabel, aValue, aLength);
    }

    public void updateNClob(String aColumnLabel, Reader aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNClob(aColumnLabel, aValue);
    }

    public void updateNString(int aColumnIndex, String aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNString(aColumnIndex, aValue);
    }

    public void updateNString(String aColumnLabel, String aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateNString(aColumnLabel, aValue);
    }

    public void updateRowId(int aColumnIndex, RowId aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateRowId(aColumnIndex, aValue);
    }

    public void updateRowId(String aColumnLabel, RowId aValue) throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateRowId(aColumnLabel, aValue);
    }

    public void updateSQLXML(int aColumnIndex, SQLXML aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateSQLXML(aColumnIndex, aValue);
    }

    public void updateSQLXML(String aColumnLabel, SQLXML aValue)
            throws SQLException
    {
        mLogger.logTrace();
        checkClosed();
        mUpdateHelper.updateSQLXML(aColumnLabel, aValue);
    }


}
