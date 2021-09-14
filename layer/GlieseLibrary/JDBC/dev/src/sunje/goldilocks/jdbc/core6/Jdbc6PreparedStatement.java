package sunje.goldilocks.jdbc.core6;

import java.io.InputStream;
import java.io.Reader;
import java.sql.NClob;
import java.sql.RowId;
import java.sql.SQLException;
import java.sql.SQLXML;

import sunje.goldilocks.jdbc.core4.Jdbc4Connection;
import sunje.goldilocks.jdbc.core4.Jdbc4PreparedStatement;
import sunje.goldilocks.jdbc.core4.RowIdBase;
import sunje.goldilocks.jdbc.dt.CodeColumn;
import sunje.goldilocks.jdbc.dt.Column;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class Jdbc6PreparedStatement extends Jdbc4PreparedStatement
{
    public Jdbc6PreparedStatement(Jdbc4Connection aConnection, String aSql, int aType, int aConcurrency, int aHoldability) throws SQLException
    {
        super(aConnection, aSql, aType, aConcurrency, aHoldability);
    }
    
    protected void notifyStatementFreed()
    {
        super.notifyStatementFreed();
    }
    
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

    public boolean isClosed() throws SQLException
    {
        mLogger.logTrace();
        return mClosed;
    }

    public boolean isPoolable() throws SQLException
    {
        mLogger.logTrace();
        return false;
    }
    
    public void setPoolable(boolean aPoolable) throws SQLException
    {
        mLogger.logTrace();
        /*
         * 아무런 작업을 하지 않음: 아직 구현하지 않음
         */
    }
    
    public synchronized void setAsciiStream(int aColNum, InputStream aValue) throws SQLException
    {
        mLogger.logTrace();
        // aValue의 길이를 알 수 없기 때문에 longvarchar로 매핑한다.
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_LONGVARCHAR, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setBinaryStream(aValue, Column.STREAM_LENGTH_UNLIMITED);
        }
    }

    public synchronized void setAsciiStream(int aColNum, InputStream aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, getVarcharTypeByLength(aLength), CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setBinaryStream(aValue, aLength);
        }
    }

    public synchronized void setBinaryStream(int aColNum, InputStream aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, getVarbinaryTypeByLength(aLength), CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setBinaryStream(aValue, aLength);
        }
    }

    public synchronized void setCharacterStream(int aColNum, Reader aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        if (aLength > Integer.MAX_VALUE)
        {
            aLength = Integer.MAX_VALUE;
        }
        Column sColumn = mBindOp.getTypedColumn(aColNum, getVarcharTypeByLength((int)aLength), CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else
        {
            sColumn.setCharacterStream(aValue, aLength);
        }
    }

    public void setBlob(int aColNum, InputStream aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "blob type");
    }

    public void setClob(int aColNum, Reader aValue, long aLength) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "clob type");        
    }

    public void setNCharacterStream(int aColNum, Reader aValue, long aLength)
            throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "nchar type");        
    }

    public void setNCharacterStream(int aColNum, Reader aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "nchar type");
    }

    public void setNClob(int aColNum, NClob aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "nclob type");
    }

    public void setNClob(int aColNum, Reader aValue, long aLength) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "nclob type");
    }

    public void setNClob(int aColNum, Reader aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "nclob type");
    }

    public void setNString(int aColNum, String aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "national type");
    }

    public void setSQLXML(int aColNum, SQLXML aValue) throws SQLException
    {
        mLogger.logTrace();
        ErrorMgr.raise(ErrorMgr.UNSUPPERTED_FEATURE, "xml type");
    }
    
    
    public synchronized void setRowId(int aColNum, RowId aValue) throws SQLException
    {
        mLogger.logTrace();
        Column sColumn = mBindOp.getTypedColumn(aColNum, CodeColumn.GOLDILOCKS_DATA_TYPE_ROWID, CodeColumn.INTERVAL_INDICATOR_NA);
        if (aValue == null)
        {
            sColumn.setNull();
        }
        else if (aValue instanceof RowIdBase)
        {
            sColumn.setRowId((RowIdBase)aValue);
        }
        else
        {
            ErrorMgr.raise(ErrorMgr.ROWID_IS_NOT_GOLDILOCKS_ROWID);
        }
    }
}
