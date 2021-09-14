package sunje.goldilocks.jdbc.internal;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.sql.Array;
import java.sql.Blob;
import java.sql.Clob;
import java.sql.Date;
import java.sql.Ref;
import java.sql.SQLException;
import java.sql.Time;
import java.sql.Timestamp;

public interface ResultSetUpdater
{
    int getConcurrency() throws SQLException;
    
    void moveToCurrentRow() throws SQLException;

    void moveToInsertRow() throws SQLException;

    void cancelRowUpdates() throws SQLException;
    
    void deleteRow() throws SQLException;
    
    void insertRow() throws SQLException;
    
    void updateRow() throws SQLException;
    
    void updateArray(int columnIndex, Array x) throws SQLException;
    
    void updateArray(String columnLabel, Array x) throws SQLException;
    
    void updateAsciiStream(int columnIndex, InputStream x, int length) throws SQLException;
    
    void updateAsciiStream(int columnIndex, InputStream x, long length) throws SQLException;
    
    void updateAsciiStream(int columnIndex, InputStream x) throws SQLException;
    
    void updateAsciiStream(String columnLabel, InputStream x, int length) throws SQLException;
    
    void updateAsciiStream(String columnLabel, InputStream x, long length) throws SQLException;
    
    void updateAsciiStream(String columnLabel, InputStream x) throws SQLException;
    
    void updateBigDecimal(int columnIndex, BigDecimal x) throws SQLException;
    
    void updateBigDecimal(String columnLabel, BigDecimal x) throws SQLException;
    
    void updateBinaryStream(int columnIndex, InputStream x, int length) throws SQLException;
    
    void updateBinaryStream(int columnIndex, InputStream x, long length) throws SQLException;
    
    void updateBinaryStream(int columnIndex, InputStream x) throws SQLException;
    
    void updateBinaryStream(String columnLabel, InputStream x, int length) throws SQLException;
    
    void updateBinaryStream(String columnLabel, InputStream x, long length) throws SQLException;
    
    void updateBinaryStream(String columnLabel, InputStream x) throws SQLException;
    
    void updateBlob(int columnIndex, Blob x) throws SQLException;
   
    void updateBlob(int columnIndex, InputStream inputStream, long length) throws SQLException;
    
    void updateBlob(int columnIndex, InputStream inputStream) throws SQLException;
    
    void updateBlob(String columnLabel, Blob x) throws SQLException;
     
    void updateBlob(String columnLabel, InputStream inputStream, long length) throws SQLException;
    
    void updateBlob(String columnLabel, InputStream inputStream) throws SQLException;
    
    void updateBoolean(int columnIndex, boolean x) throws SQLException;
    
    void updateBoolean(String columnLabel, boolean x) throws SQLException;
    
    void updateByte(int columnIndex, byte x) throws SQLException;
    
    void updateByte(String columnLabel, byte x) throws SQLException;
    
    void updateBytes(int columnIndex, byte[] x) throws SQLException;
    
    void updateBytes(String columnLabel, byte[] x) throws SQLException;
    
    void updateCharacterStream(int columnIndex, Reader x, int length) throws SQLException;
    
    void updateCharacterStream(int columnIndex, Reader x, long length) throws SQLException;
    
    void updateCharacterStream(int columnIndex, Reader x) throws SQLException;
    
    void updateCharacterStream(String columnLabel, Reader reader, int length) throws SQLException;
    
    void updateCharacterStream(String columnLabel, Reader reader, long length) throws SQLException;
    
    void updateCharacterStream(String columnLabel, Reader reader) throws SQLException;
    
    void updateClob(int columnIndex, Clob x) throws SQLException;
    
    void updateClob(int columnIndex, Reader reader, long length) throws SQLException;
    
    void updateClob(int columnIndex, Reader reader) throws SQLException;
    
    void updateClob(String columnLabel, Clob x) throws SQLException;
    
    void updateClob(String columnLabel, Reader reader, long length) throws SQLException;
    
    void updateClob(String columnLabel, Reader reader) throws SQLException;
    
    void updateDate(int columnIndex, Date x) throws SQLException;
    
    void updateDate(String columnLabel, Date x) throws SQLException;
    
    void updateDouble(int columnIndex, double x) throws SQLException;
    
    void updateDouble(String columnLabel, double x) throws SQLException;
    
    void updateFloat(int columnIndex, float x) throws SQLException;
    
    void updateFloat(String columnLabel, float x) throws SQLException;
    
    void updateInt(int columnIndex, int x) throws SQLException;
    
    void updateInt(String columnLabel, int x) throws SQLException;
    
    void updateLong(int columnIndex, long x) throws SQLException;
    
    void updateLong(String columnLabel, long x) throws SQLException;
    
    void updateNCharacterStream(int columnIndex, Reader x, long length) throws SQLException;
    
    void updateNCharacterStream(int columnIndex, Reader x) throws SQLException;
    
    void updateNCharacterStream(String columnLabel, Reader reader, long length) throws SQLException;
    
    void updateNCharacterStream(String columnLabel, Reader reader) throws SQLException;
    
    void updateNClob(int columnIndex, Object nClob) throws SQLException;
    
    void updateNClob(int columnIndex, Reader reader, long length) throws SQLException;
    
    void updateNClob(int columnIndex, Reader reader) throws SQLException;
    
    void updateNClob(String columnLabel, Object nClob) throws SQLException;
    
    void updateNClob(String columnLabel, Reader reader, long length) throws SQLException;
    
    void updateNClob(String columnLabel, Reader reader) throws SQLException;
    
    void updateNString(int columnIndex, String nString) throws SQLException;
    
    void updateNString(String columnLabel, String nString) throws SQLException;
    
    void updateNull(int columnIndex) throws SQLException;
    
    void updateNull(String columnLabel) throws SQLException;
    
    void updateObject(int columnIndex, Object x, int scaleOrLength) throws SQLException;
    
    void updateObject(int columnIndex, Object x) throws SQLException;
    
    void updateObject(String columnLabel, Object x, int scaleOrLength) throws SQLException;
    
    void updateObject(String columnLabel, Object x) throws SQLException;
    
    void updateRef(int columnIndex, Ref x) throws SQLException;
    
    void updateRef(String columnLabel, Ref x) throws SQLException;
    
    void updateRowId(int columnIndex, Object x) throws SQLException;
    
    void updateRowId(String columnLabel, Object x) throws SQLException;
    
    void updateSQLXML(int columnIndex, Object xmlObject) throws SQLException;
    
    void updateSQLXML(String columnLabel, Object xmlObject) throws SQLException;
    
    void updateShort(int columnIndex, short x) throws SQLException;
    
    void updateShort(String columnLabel, short x) throws SQLException;
    
    void updateString(int columnIndex, String x) throws SQLException;
    
    void updateString(String columnLabel, String x) throws SQLException;
    
    void updateTime(int columnIndex, Time x) throws SQLException;
    
    void updateTime(String columnLabel, Time x) throws SQLException;
    
    void updateTimestamp(int columnIndex, Timestamp x) throws SQLException;
    
    void updateTimestamp(String columnLabel, Timestamp x) throws SQLException;
}
