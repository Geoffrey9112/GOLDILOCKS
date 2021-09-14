package sunje.goldilocks.jdbc.dt;

import java.io.InputStream;
import java.io.Reader;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.math.BigDecimal;
import java.sql.Date;
import java.sql.SQLException;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.Calendar;

import sunje.goldilocks.jdbc.core4.RowIdBase;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class ColumnChar extends Column
{
    protected static final int VALUE_TYPE_NONE = 0;
    protected static final int VALUE_TYPE_STRING = 1;
    protected static final int VALUE_TYPE_BYTES = 2;
    protected static final int VALUE_TYPE_STREAM = 3;
    protected static final int VALUE_TYPE_READER = 4;
    
    protected String mValueString = null;
    protected byte[] mValueBytes = null;
    protected InputStream mValueStream = null;
    protected Reader mValueReader = null;
    protected int mValueLength = 0;
    protected int mValueType = VALUE_TYPE_NONE;
    
    ColumnChar(CodeColumn aCode)
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
        mValueString = mCache.readString(mId);
        if (mValueString.length() == 0)
        {
            mValueString = null;
        }
        mValueType = VALUE_TYPE_STRING;
    }
    
    protected void storeValueToCursorImpl() throws SQLException
    {
        if (mValueType == VALUE_TYPE_STRING)
        {
            mCache.writeString(mId, mValueString);
        }
        else if (mValueType == VALUE_TYPE_BYTES)
        {
            mCache.writeBytes(mId, mValueBytes, mValueLength);
        }
        else if (mValueType == VALUE_TYPE_STREAM)
        {
            mCache.writeBytes(mId, mValueStream, mValueLength);
        }
        else if (mValueType == VALUE_TYPE_READER)
        {
            mCache.writeBytes(mId, mValueReader, mValueLength);
        }
        else
        {
            ErrorMgr.raiseRuntimeError("Invalid value type");
        }
    }
    
    protected void setBooleanImpl(boolean aValue) throws SQLException
    {
        mValueString = (aValue ? "TRUE" : "FALSE");
        mValueType = VALUE_TYPE_STRING;
    }
    
    protected void setByteImpl(byte aValue) throws SQLException
    {
        mValueString = String.valueOf(aValue);
        mValueType = VALUE_TYPE_STRING;
    }

    protected void setBytesImpl(byte[] aValue) throws SQLException
    {
        mValueBytes = aValue;
        mValueLength = aValue.length;
        mValueType = VALUE_TYPE_BYTES;
    }
    
    protected void setShortImpl(short aValue) throws SQLException
    {
        mValueString = String.valueOf(aValue);
        mValueType = VALUE_TYPE_STRING;
    }

    protected void setIntImpl(int aValue) throws SQLException
    {
        mValueString = String.valueOf(aValue);
        mValueType = VALUE_TYPE_STRING;
    }

    protected void setLongImpl(long aValue) throws SQLException
    {
        mValueString = String.valueOf(aValue);
        mValueType = VALUE_TYPE_STRING;
    }

    protected void setFloatImpl(float aValue) throws SQLException
    {
        mValueString = String.valueOf(aValue);
        mValueType = VALUE_TYPE_STRING;
    }

    protected void setDoubleImpl(double aValue) throws SQLException
    {
        mValueString = String.valueOf(aValue);
        mValueType = VALUE_TYPE_STRING;
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        mValueString = aValue;
        mValueType = VALUE_TYPE_STRING;
    }

    protected void setObjectImpl(Object aValue) throws SQLException
    {
        mValueString = aValue.toString();
        mValueType = VALUE_TYPE_STRING;
    }

    protected void setDateImpl(Date aValue, Calendar aCalendar) throws SQLException
    {
        mValueString = aValue.toString();
        mValueType = VALUE_TYPE_STRING;
    }
    
    protected void setTimeImpl(Time aValue, Calendar aCalendar) throws SQLException
    {
        mValueString = aValue.toString();
        mValueType = VALUE_TYPE_STRING;
    }
    
    protected void setTimestampImpl(Timestamp aValue, Calendar aCalendar) throws SQLException
    {
        mValueString = aValue.toString();
        mValueType = VALUE_TYPE_STRING;
    }

    protected void setBigDecimalImpl(BigDecimal aValue) throws SQLException
    {
        mValueString = aValue.toString();
        mValueType = VALUE_TYPE_STRING;
    }

    protected void setBinaryStreamImpl(InputStream aValue, long aLength) throws SQLException
    {
        mValueStream = aValue;
        mValueLength = (int)aLength;
        mValueType = VALUE_TYPE_STREAM;
    }
    
    protected void setCharacterStreamImpl(Reader aValue, long aLength) throws SQLException
    {
        mValueReader = aValue;
        mValueLength = (int)aLength;
        mValueType = VALUE_TYPE_READER;
    }
    
    protected void setRowIdImpl(RowIdBase aValue) throws SQLException
    {
        mValueString = aValue.toString();
        mValueType = VALUE_TYPE_STRING;
    }

    protected void verifyStringValue()
    {
        if (mValueType == VALUE_TYPE_BYTES)
        {
            try
            {
                mValueString = new String(mValueBytes, 0, mValueLength, mCache.getCharsetName());
            }
            catch (UnsupportedEncodingException sException)
            {
                mValueString = null;
            }
            catch (IndexOutOfBoundsException sException)
            {
                mValueString = null;
            }
        }
        else if (mValueType == VALUE_TYPE_STREAM)
        {
            // 이 경우가 불리려면 setAsciiStream()한 다음에 getString()을 호출했을 때인데,
            // 아직 이런 경우가 없을 것 같아 구현하지 않음.
            ErrorMgr.raiseRuntimeError("Must be implemented");
        }
        else if (mValueType == VALUE_TYPE_READER)
        {
            // 이 경우가 불리려면 setCharacterStream()한 다음에 getString()을 호출했을 때인데,
            // 아직 이런 경우가 없을 것 같아 구현하지 않음.
            ErrorMgr.raiseRuntimeError("Must be implemented");
        }
        mValueType = VALUE_TYPE_STRING;
    }
    
    protected boolean getBooleanImpl() throws SQLException
    {
        verifyStringValue();
        if (!mValueString.trim().equalsIgnoreCase("true") &&
            !mValueString.trim().equalsIgnoreCase("false"))
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, mValueString, "boolean");
        }
        return mValueString.trim().equalsIgnoreCase("true");
    }
    
    protected byte getByteImpl() throws SQLException
    {
        verifyStringValue();
        try
        {
            return (byte)Double.parseDouble(mValueString);
        }
        catch (NumberFormatException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, mValueString, "byte");
        }
        return 0;
    }

    protected short getShortImpl() throws SQLException
    {
        verifyStringValue();
        try
        {
            return (short)Double.parseDouble(mValueString);
        }
        catch (NumberFormatException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, mValueString, "short");
        }
        return 0;
    }

    protected int getIntImpl() throws SQLException
    {
        verifyStringValue();
        try
        {
            long x = (long)Double.parseDouble(mValueString);
            return (int)x;
        }
        catch (NumberFormatException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, mValueString, "int");
        }
        return 0;
    }

    protected long getLongImpl() throws SQLException
    {
        verifyStringValue();
        try
        {
            return (long)Double.parseDouble(mValueString);
        }
        catch (NumberFormatException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, mValueString, "long");
        }
        return 0;
    }

    protected float getFloatImpl() throws SQLException
    {
        verifyStringValue();
        try
        {
            return Float.parseFloat(mValueString);
        }
        catch (NumberFormatException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, mValueString, "float");
        }
        return 0;
    }

    protected double getDoubleImpl() throws SQLException
    {
        verifyStringValue();
        try
        {
            return Double.parseDouble(mValueString);
        }
        catch (NumberFormatException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, mValueString, "double");
        }
        return 0;
    }

    protected String getStringImpl() throws SQLException
    {
        verifyStringValue();
        return mValueString;
    }

    protected Object getObjectImpl() throws SQLException
    {
        verifyStringValue();
        return mValueString;
    }
    
    protected Date getDateImpl(Calendar aCalendar) throws SQLException
    {
        verifyStringValue();
        Date sResult = null;
        try
        {
            sResult = Date.valueOf(mValueString);
        }
        catch (IllegalArgumentException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, mValueString, "Date");
        }
        return sResult;
    }
    
    protected Time getTimeImpl(Calendar aCalendar) throws SQLException
    {
        verifyStringValue();
        Time sResult = null;
        try
        {
            sResult = Time.valueOf(mValueString);
        }
        catch (IllegalArgumentException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, mValueString, "Time");
        }
        return sResult;
    }
    
    protected Timestamp getTimestampImpl(Calendar aCalendar) throws SQLException
    {
        verifyStringValue();
        Timestamp sResult = null;
        try
        {
            sResult = Timestamp.valueOf(mValueString);
        }
        catch (IllegalArgumentException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, mValueString, "Timestamp");
        }
        return sResult;
    }
    
    protected BigDecimal getBigDecimalImpl() throws SQLException
    {
        verifyStringValue();
        try
        {
            return new BigDecimal(mValueString);
        }
        catch (NumberFormatException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, mValueString, "BigDecimal");
        }
        return null;
    }
    
    protected Reader getCharacterStreamImpl() throws SQLException
    {
        verifyStringValue();
        return new StringReader(mValueString);
    }

    protected RowIdBase getRowIdImpl() throws SQLException
    {
        verifyStringValue();
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "RowId");
        return null;
    }
}
