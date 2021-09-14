package sunje.goldilocks.jdbc.dt;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.RowIdBase;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class ColumnBoolean extends ColumnNonDateTime
{
    private boolean mValue = false;
    
    ColumnBoolean(CodeColumn aCode)
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
        mValue = mCache.readByte(mId) != 0;
    }
    
    protected void storeValueToCursorImpl() throws SQLException
    {
        mCache.writeByte(mId, mValue ? (byte)1 : 0);
    }

    protected void setBooleanImpl(boolean aValue) throws SQLException
    {
        mValue = aValue;
    }

    protected void setByteImpl(byte aValue) throws SQLException
    {
        if (aValue != 0 && aValue != 1)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aValue), mCode.getTypeName());
        }
        mValue = aValue != 0;
    }

    protected void setBytesImpl(byte[] aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "byte[]", mCode.getTypeName());
    }
    
    protected void setShortImpl(short aValue) throws SQLException
    {
        if (aValue != 0 && aValue != 1)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aValue), mCode.getTypeName());
        }
        mValue = (aValue != 0);
    }

    protected void setIntImpl(int aValue) throws SQLException
    {
        if (aValue != 0 && aValue != 1)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aValue), mCode.getTypeName());
        }
        mValue = (aValue != 0);
    }
    
    protected void setLongImpl(long aValue) throws SQLException
    {
        if (aValue != 0 && aValue != 1)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aValue), mCode.getTypeName());
        }
        mValue = (aValue != 0);
    }
    
    protected void setFloatImpl(float aValue) throws SQLException
    {
        if (aValue != 0 && aValue != 1)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aValue), mCode.getTypeName());
        }
        mValue = (aValue != 0);
    }

    protected void setDoubleImpl(double aValue) throws SQLException
    {
        if (aValue != 0 && aValue != 1)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aValue), mCode.getTypeName());
        }
        mValue = (aValue != 0);
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        if (!aValue.equalsIgnoreCase("true") && !aValue.equalsIgnoreCase("false"))
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, mCode.getTypeName());
        }
        mValue = aValue.equalsIgnoreCase("true");
    }
    
    protected void setObjectImpl(Object aValue) throws SQLException
    {
        if (aValue instanceof Number)
        {
            int sValue = ((Number)aValue).intValue();
            if ((sValue != 0) && (sValue != 1))
            {
                ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aValue), mCode.getTypeName());
            }
            mValue = (sValue != 0);
        }
        else
        {
            ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, aValue.getClass().getName(), mCode.getTypeName());
        }
    }

    protected void setBigDecimalImpl(BigDecimal aValue) throws SQLException
    {
        long sValue = aValue.longValue();
        if (sValue != 0 && sValue != 1)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aValue), mCode.getTypeName());
        }
        mValue = (sValue != 0);
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
        return mValue;
    }

    protected byte getByteImpl() throws SQLException
    {
        return mValue ? (byte)1 : 0;
    }
    
    protected short getShortImpl() throws SQLException
    {
        return mValue ? (short)1 : 0;
    }
        
    protected int getIntImpl() throws SQLException
    {
        return mValue ? 1 : 0;
    }
    
    protected long getLongImpl() throws SQLException
    {
        return mValue ? 1 : 0;
    }
    
    protected float getFloatImpl() throws SQLException
    {
        return mValue ? 1 : 0;
    }
    
    protected double getDoubleImpl() throws SQLException
    {
        return mValue ? 1 : 0;
    }
    
    protected String getStringImpl() throws SQLException
    {
        return mValue ? "TRUE" : "FALSE";
    }
    
    protected Object getObjectImpl() throws SQLException
    {
        return mValue ? Boolean.TRUE : Boolean.FALSE;
    }
    
    protected BigDecimal getBigDecimalImpl() throws SQLException
    {
        return mValue ? BigDecimal.ONE : BigDecimal.ZERO;
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
