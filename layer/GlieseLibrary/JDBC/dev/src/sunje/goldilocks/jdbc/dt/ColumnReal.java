package sunje.goldilocks.jdbc.dt;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.RowIdBase;
import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.util.Utils;

public class ColumnReal extends ColumnNonDateTime
{
    private float mValue = 0;
    
    ColumnReal(CodeColumn aCode)
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
        mValue = mCache.readFloat(mId);
    }
    
    protected void storeValueToCursorImpl() throws SQLException
    {
        mCache.writeFloat(mId, mValue);
    }

    protected void setBooleanImpl(boolean aValue) throws SQLException
    {
        mValue = (aValue ? 1 : 0);
    }

    protected void setByteImpl(byte aValue) throws SQLException
    {
        mValue = aValue;
    }

    protected void setBytesImpl(byte[] aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "byte[]", mCode.getTypeName());
    }

    protected void setShortImpl(short aValue) throws SQLException
    {
        mValue = aValue;
    }

    protected void setIntImpl(int aValue) throws SQLException
    {
        mValue = aValue;
    }
    
    protected void setLongImpl(long aValue) throws SQLException
    {
        mValue = aValue;
    }
    
    protected void setFloatImpl(float aValue) throws SQLException
    {
        mValue = aValue;
    }

    protected void setDoubleImpl(double aValue) throws SQLException
    {
        mValue = (float)aValue;
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        try
        {
            mValue = Float.parseFloat(aValue);
        }
        catch(NumberFormatException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, mCode.getTypeName());
        }
    }
    
    protected void setObjectImpl(Object aValue) throws SQLException
    {
        if (aValue instanceof Number)
        {
            mValue = ((Number)aValue).floatValue();
        }
        else
        {
            ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, aValue.getClass().getName(), mCode.getTypeName());
        }
    }

    protected void setBigDecimalImpl(BigDecimal aValue) throws SQLException
    {
        mValue = aValue.floatValue();
    }

    protected void setBinaryStreamImpl(InputStream aValue, long aLength) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "InputStream", mCode.getTypeName());
    }
    
    protected void setCharacterStreamImpl(Reader aValue, long aLength) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "Reader", mCode.getTypeName());
    }
    
    protected void setRowIdImpl(RowIdBase aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "RowId", mCode.getTypeName());
    }

    protected boolean getBooleanImpl() throws SQLException
    {
        if (mValue != 0 && mValue != 1)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(mValue), "boolean");
        }
        return mValue != 0;
    }

    protected byte getByteImpl() throws SQLException
    {
        return (byte)mValue;
    }
    
    protected short getShortImpl() throws SQLException
    {
        return (short)mValue;
    }
        
    protected int getIntImpl() throws SQLException
    {
        return (int)mValue;
    }
    
    protected long getLongImpl() throws SQLException
    {
        return (long)mValue;
    }
    
    protected float getFloatImpl() throws SQLException
    {
        return mValue;
    }
    
    protected double getDoubleImpl() throws SQLException
    {
        return mValue;
    }
    
    protected String getStringImpl() throws SQLException
    {
        return String.valueOf(mValue);
    }
    
    protected Object getObjectImpl() throws SQLException
    {
        return new Float(mValue);
    }
    
    protected BigDecimal getBigDecimalImpl() throws SQLException
    {
        return Utils.makeBigDecimal(mValue);
    }
    
    protected Reader getCharacterStreamImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "Reader");
        return null;
    }

    protected RowIdBase getRowIdImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "RowId");
        return null;
    }
}
