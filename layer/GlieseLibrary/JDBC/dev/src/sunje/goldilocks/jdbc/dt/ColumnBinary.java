package sunje.goldilocks.jdbc.dt;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.RowIdBase;
import sunje.goldilocks.jdbc.core4.VersionSpecific;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class ColumnBinary extends ColumnNonDateTime
{
    protected static final int VALUE_TYPE_NONE = 0;
    protected static final int VALUE_TYPE_BYTES = 2;
    protected static final int VALUE_TYPE_STREAM = 3;
    
    protected byte[] mValueBytes = null;
    protected InputStream mValueStream = null;
    protected int mValueStreamLength = 0;
    protected int mValueType = VALUE_TYPE_NONE;
    
    ColumnBinary(CodeColumn aCode)
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
        mValueBytes = mCache.readBytes(mId);
        if (mValueBytes.length == 0)
        {
            mValueBytes = null;
        }
        mValueType = VALUE_TYPE_BYTES;
    }
    
    protected void storeValueToCursorImpl() throws SQLException
    {
        if (mValueType == VALUE_TYPE_BYTES)
        {
            mCache.writeBytes(mId, mValueBytes, mValueBytes.length);
        }
        else if (mValueType == VALUE_TYPE_STREAM)
        {
            mCache.writeBytes(mId, mValueStream, mValueStreamLength);
        }
        else
        {
            ErrorMgr.raiseRuntimeError("Invalid value type");
        }
    }
    
    protected void verifyBytesValue()
    {
        if (mValueType == VALUE_TYPE_STREAM)
        {
            ErrorMgr.raiseRuntimeError("Must be implemented");
        }
        mValueType = VALUE_TYPE_BYTES;
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
        mValueBytes = aValue;
        mValueType = VALUE_TYPE_BYTES;
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

    private byte toByte(char aChar) throws SQLException
    {
        if (aChar >= '0' && aChar <= '9')
        {
            return (byte)(aChar - '0');
        }
        else if (aChar >= 'A' && aChar <= 'F')
        {
            return (byte)(10 + aChar - 'A');
        }
        ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aChar), mCode.getTypeName());
        return -1;
    }
    
    protected void setStringImpl(String aValue) throws SQLException
    {
        if (aValue.length() % 2 == 1)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, mCode.getTypeName());
        }
        mValueBytes = new byte[aValue.length()/2];
        for (int i=0; i<mValueBytes.length; i++)
        {
            char sUpper = aValue.charAt(i*2);
            char sLower = aValue.charAt(i*2+1);
            
            mValueBytes[i] = (byte)((toByte(sUpper) << 4) | toByte(sLower));
        }
        mValueType = VALUE_TYPE_BYTES;
    }

    protected void setObjectImpl(Object aValue) throws SQLException
    {
        if (aValue instanceof byte[])
        {
            mValueBytes = (byte[])aValue;
            mValueType = VALUE_TYPE_BYTES;
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

    protected void setBinaryStreamImpl(InputStream aValue, long aLength) throws SQLException
    {
        mValueStream = aValue;
        mValueStreamLength = (int)aLength;
        mValueType = VALUE_TYPE_STREAM;
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
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "Double");
        return 0;
    }

    protected String getStringImpl() throws SQLException
    {
        verifyBytesValue();
        return VersionSpecific.SINGLETON.byteToHexString(mValueBytes, true);
    }

    protected Object getObjectImpl() throws SQLException
    {
        verifyBytesValue();
        return mValueBytes;
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
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "RowId");
        return null;
    }
}
