package sunje.goldilocks.jdbc.dt;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.RowIdBase;
import sunje.goldilocks.jdbc.core4.VersionSpecific;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class ColumnRowId extends ColumnNonDateTime
{
    private byte[] mValue = new byte[16];
    
    ColumnRowId(CodeColumn aCode)
    {
        super(aCode);
    }
    
    public long getArgNum1()
    {
        return mPrecision;
    }
    
    public long getArgNum2()
    {
        return mScale;
    }

    protected void setArgNum1(long aArgNum)
    {
        mPrecision = (int)aArgNum;
    }
    
    protected void setArgNum2(long aArgNum)
    {
        mScale = (int)aArgNum;
    }
    
    protected void loadValueFromCursorImpl() throws SQLException
    {
        mCache.readBytes(mId, mValue);
    }

    protected void storeValueToCursorImpl() throws SQLException
    {
        mCache.writeBytes(mId, mValue, mValue.length);        
    }

    protected void setBooleanImpl(boolean aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "boolean", mCode.getTypeName());        
    }

    protected void setByteImpl(byte aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "byte", mCode.getTypeName());        
    }

    protected void setBytesImpl(byte[] aValue) throws SQLException
    {
        if (aValue.length != mValue.length)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_LENGTH_MISMATCH, String.valueOf(aValue.length), String.valueOf(mValue.length));
        }
        System.arraycopy(aValue, 0, mValue, 0, aValue.length);
    }

    protected void setShortImpl(short aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "short", mCode.getTypeName());        
    }

    protected void setIntImpl(int aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "int", mCode.getTypeName());        
    }

    protected void setLongImpl(long aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "long", mCode.getTypeName());        
    }

    protected void setFloatImpl(float aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "float", mCode.getTypeName());        
    }

    protected void setDoubleImpl(double aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "double", mCode.getTypeName());        
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        RowIdBase.toBytes(aValue, mValue);        
    }

    protected void setObjectImpl(Object aValue) throws SQLException
    {
        if (aValue instanceof RowIdBase)
        {
            setRowId((RowIdBase)aValue);
        }
        else
        {
            ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, aValue.getClass().getName(), mCode.getTypeName());
        }
    }

    protected void setBigDecimalImpl(BigDecimal aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "BigDecimal", mCode.getTypeName());        
    }

    protected void setBinaryStreamImpl(InputStream aValue, long aLength)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "InputStream", mCode.getTypeName());        
    }

    protected void setCharacterStreamImpl(Reader aValue, long aLength)
            throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "Reader", mCode.getTypeName());
    }

    protected void setRowIdImpl(RowIdBase aValue) throws SQLException
    {
        setBytesImpl(aValue.getBytes());
    }

    protected boolean getBooleanImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "boolean");
        return false;
    }

    protected byte getByteImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "byte");
        return 0;
    }

    protected short getShortImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "short");
        return 0;
    }

    protected int getIntImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "int");
        return 0;
    }

    protected long getLongImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "long");
        return 0;
    }

    protected float getFloatImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "float");
        return 0;
    }

    protected double getDoubleImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "double");
        return 0;
    }

    protected String getStringImpl() throws SQLException
    {
        return RowIdBase.toString(mValue);
    }

    protected Object getObjectImpl() throws SQLException
    {
        return getRowId();
    }

    protected BigDecimal getBigDecimalImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "BigDecimal");
        return null;
    }

    protected Reader getCharacterStreamImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "Reader");
        return null;
    }

    protected RowIdBase getRowIdImpl() throws SQLException
    {
        return VersionSpecific.SINGLETON.createConcreteRowId(mValue);
    }
}
