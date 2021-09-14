package sunje.goldilocks.jdbc.internal;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.sql.Array;
import java.sql.Blob;
import java.sql.Clob;
import java.sql.Date;
import java.sql.Ref;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Time;
import java.sql.Timestamp;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class ReadOnlyResultSetUpdater implements ResultSetUpdater
{
    public ReadOnlyResultSetUpdater()
    {
        
    }
    
    public int getConcurrency() throws SQLException
    {
        return ResultSet.CONCUR_READ_ONLY;
    }
    
    public void moveToCurrentRow() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void moveToInsertRow() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void cancelRowUpdates() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void deleteRow() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void insertRow() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateRow() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateArray(int columnIndex, Array x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateArray(String columnLabel, Array x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateAsciiStream(int columnIndex, InputStream x, int length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateAsciiStream(int columnIndex, InputStream x, long length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateAsciiStream(int columnIndex, InputStream x)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateAsciiStream(String columnLabel, InputStream x, int length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateAsciiStream(String columnLabel, InputStream x, long length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateAsciiStream(String columnLabel, InputStream x)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBigDecimal(int columnIndex, BigDecimal x)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBigDecimal(String columnLabel, BigDecimal x)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBinaryStream(int columnIndex, InputStream x, int length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBinaryStream(int columnIndex, InputStream x, long length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBinaryStream(int columnIndex, InputStream x)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBinaryStream(String columnLabel, InputStream x, int length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBinaryStream(String columnLabel, InputStream x,
            long length) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBinaryStream(String columnLabel, InputStream x)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBlob(int columnIndex, Blob x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBlob(int columnIndex, InputStream inputStream, long length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBlob(int columnIndex, InputStream inputStream)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBlob(String columnLabel, Blob x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBlob(String columnLabel, InputStream inputStream,
            long length) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBlob(String columnLabel, InputStream inputStream)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBoolean(int columnIndex, boolean x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBoolean(String columnLabel, boolean x)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateByte(int columnIndex, byte x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateByte(String columnLabel, byte x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBytes(int columnIndex, byte[] x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateBytes(String columnLabel, byte[] x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateCharacterStream(int columnIndex, Reader x, int length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateCharacterStream(int columnIndex, Reader x, long length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateCharacterStream(int columnIndex, Reader x)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateCharacterStream(String columnLabel, Reader reader,
            int length) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateCharacterStream(String columnLabel, Reader reader,
            long length) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateCharacterStream(String columnLabel, Reader reader)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateClob(int columnIndex, Clob x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateClob(int columnIndex, Reader reader, long length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateClob(int columnIndex, Reader reader) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateClob(String columnLabel, Clob x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateClob(String columnLabel, Reader reader, long length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateClob(String columnLabel, Reader reader)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateDate(int columnIndex, Date x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateDate(String columnLabel, Date x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateDouble(int columnIndex, double x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateDouble(String columnLabel, double x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateFloat(int columnIndex, float x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateFloat(String columnLabel, float x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateInt(int columnIndex, int x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateInt(String columnLabel, int x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateLong(int columnIndex, long x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateLong(String columnLabel, long x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNCharacterStream(int columnIndex, Reader x, long length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNCharacterStream(int columnIndex, Reader x)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNCharacterStream(String columnLabel, Reader reader,
            long length) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNCharacterStream(String columnLabel, Reader reader)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNClob(int columnIndex, Object nClob) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNClob(int columnIndex, Reader reader, long length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNClob(int columnIndex, Reader reader) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNClob(String columnLabel, Object nClob)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNClob(String columnLabel, Reader reader, long length)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNClob(String columnLabel, Reader reader)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNString(int columnIndex, String nString)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNString(String columnLabel, String nString)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNull(int columnIndex) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateNull(String columnLabel) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateObject(int columnIndex, Object x, int scaleOrLength)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateObject(int columnIndex, Object x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateObject(String columnLabel, Object x, int scaleOrLength)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateObject(String columnLabel, Object x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateRef(int columnIndex, Ref x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateRef(String columnLabel, Ref x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateRowId(int columnIndex, Object x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateRowId(String columnLabel, Object x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateSQLXML(int columnIndex, Object xmlObject)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateSQLXML(String columnLabel, Object xmlObject)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateShort(int columnIndex, short x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateShort(String columnLabel, short x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateString(int columnIndex, String x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateString(String columnLabel, String x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateTime(int columnIndex, Time x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateTime(String columnLabel, Time x) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateTimestamp(int columnIndex, Timestamp x)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }

    public void updateTimestamp(String columnLabel, Timestamp x)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.READ_ONLY_VIOLATION);
    }
}
