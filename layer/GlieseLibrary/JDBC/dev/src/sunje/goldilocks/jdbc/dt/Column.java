package sunje.goldilocks.jdbc.dt;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.sql.Date;
import java.sql.SQLException;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.Calendar;

import sunje.goldilocks.jdbc.core4.RowIdBase;

public abstract class Column
{
    public static final byte BIND_TYPE_UNKNOWN = 0;
    public static final byte BIND_TYPE_INPUT = 1;
    public static final byte BIND_TYPE_INPUT_OUTPUT = 2;
    public static final byte BIND_TYPE_OUTPUT = 4;

    public static final int LONG_TYPE_MAX_ALLOC_SIZE = 10485760; // longvarchar나 longvarbinary는 최대 10M까지 가능
    
    // JDBC spec에 NULL일 경우 리턴값으로 명시된 값들 
    public static final boolean BOOLEAN_NULL_VALUE = false;
    public static final byte BYTE_NULL_VALUE = 0;
    public static final short SHORT_NULL_VALUE = 0;
    public static final int INT_NULL_VALUE = 0;
    public static final long LONG_NULL_VALUE = 0;
    public static final float FLOAT_NULL_VALUE = 0;
    public static final double DOUBLE_NULL_VALUE = 0;
    
    public static final int STREAM_LENGTH_UNLIMITED = Integer.MAX_VALUE;

    public static Column createTargetColumn(int aId,
                                            byte aDataType,
                                            long aArgNum1,
                                            long aArgNum2,
                                            byte aLenUnit,
                                            byte aIntervalInd,
                                            RowCache aCache) throws SQLException
    {
        Column sResult = CodeColumn.createColumn(aDataType, aIntervalInd);
        sResult.setColumnSpec(aId,
                              aArgNum1,
                              aArgNum2,
                              aLenUnit,
                              aIntervalInd,
                              BIND_TYPE_UNKNOWN,
                              aCache);
        return sResult;
    }
    
    public static Column createBindColumn(int aId,
                                          byte aDataType,
                                          byte aBindType,
                                          byte aIntervalInd,
                                          RowCache aCache) throws SQLException
    {
        Column sResult = CodeColumn.createColumn(aDataType, aIntervalInd);
        sResult.setColumnSpec(aId,
                              sResult.mCode.getDefaultArgNum1(),
                              sResult.mCode.getDefaultArgNum2(),
                              sResult.mCode.getDefaultStringLengthUnit(),
                              sResult.mCode.getDefaultIntervalIndicator(),
                              aBindType,
                              aCache);
        return sResult;
    }

    protected int mId;
    protected byte mBindType;
    protected int mPrecision;
    protected int mScale;
    protected byte mStringLengthUnit;
    protected byte mIntervalIndicator;
    protected CodeColumn mCode;
    protected RowCache mCache = null;
    
    private boolean mIsNull = false;    
    private long mLastFetchedIndex = -1;
    
    Column(CodeColumn aCode)
    {
        mCode = aCode;
    }
    
    protected void setColumnSpec(int aId,
                               long aArgNum1,
                               long aArgNum2,
                               byte aLenUnit,
                               byte aIntervalInd,
                               byte aBindType,
                               RowCache aCache)
    {
        mId = aId;
        mStringLengthUnit = aLenUnit;
        mIntervalIndicator = aIntervalInd;
        mBindType = aBindType;
        mCache = aCache;
        setArgNum1(aArgNum1);
        setArgNum2(aArgNum2);
    }
    
    private void validateFetchedValue() throws SQLException
    {
        long sCurrentAccumulatedIdx = mCache.getAccumulatedIdx();
        if (mLastFetchedIndex != sCurrentAccumulatedIdx)
        {
            loadValueFromCursor();
            mLastFetchedIndex = sCurrentAccumulatedIdx;
        }
    }
    
    public CodeColumn getCode()
    {
        return mCode;
    }
    
    public int getId()
    {
        return mId;
    }

    public byte getBindType()
    {
        return mBindType;
    }

    public int getPrecision()
    {
        return mPrecision;
    }
    
    public int getScale()
    {
        return mScale;
    }
    
    public byte getStringLengthUnit()
    {
        return mStringLengthUnit;
    }
    
    public byte getIntervalIndicator()
    {
        return mIntervalIndicator;
    }    

    public final boolean isNull() throws SQLException
    {
        return mIsNull;
    }

    public void loadValueFromCursor() throws SQLException
    {
        mIsNull = mCache.isNull(mId);
        if (!mIsNull)
        {
            loadValueFromCursorImpl();
        }
    }
    
    public void storeValueToCursor() throws SQLException
    {
        if (mIsNull)
        {
            mCache.writeNull(mId);    
        }
        else
        {
            storeValueToCursorImpl();
        }
    }

    public void setNull() throws SQLException
    {
        mIsNull = true;
    }
    
    public void setBoolean(boolean aValue) throws SQLException
    {
        mIsNull = false;
        setBooleanImpl(aValue);
    }
    
    public void setByte(byte aValue) throws SQLException
    {
        mIsNull = false;
        setByteImpl(aValue);
    }
    
    public void setBytes(byte[] aValue) throws SQLException
    {
        if (aValue == null)
        {
            mIsNull = true;
        }
        else
        {
            mIsNull = false;
            setBytesImpl(aValue);
        }
    }

    public void setShort(short aValue) throws SQLException
    {
        mIsNull = false;
        setShortImpl(aValue);
    }

    public void setInt(int aValue) throws SQLException
    {
        mIsNull = false;
        setIntImpl(aValue);
    }

    public void setLong(long aValue) throws SQLException
    {
        mIsNull = false;
        setLongImpl(aValue);
    }

    public void setFloat(float aValue) throws SQLException
    {
        mIsNull = false;
        setFloatImpl(aValue);
    }

    public void setDouble(double aValue) throws SQLException
    {
        mIsNull = false;
        setDoubleImpl(aValue);
    }

    public void setString(String aValue) throws SQLException
    {
        if (aValue == null)
        {
            mIsNull = true;
        }
        else
        {
            mIsNull = false;
            setStringImpl(aValue);
        }
    }

    public void setObject(Object aValue) throws SQLException
    {
        if (aValue == null)
        {
            mIsNull = true;
        }
        else
        {
            mIsNull = false;
            setObjectImpl(aValue);
        }
    }

    public void setDate(Date aValue, Calendar aCalendar) throws SQLException
    {
        if (aValue == null)
        {
            mIsNull = true;
        }
        else
        {
            mIsNull = false;
            setDateImpl(aValue, aCalendar);
        }
    }

    public void setTime(Time aValue, Calendar aCalendar) throws SQLException
    {
        if (aValue == null)
        {
            mIsNull = true;
        }
        else
        {
            mIsNull = false;
            setTimeImpl(aValue, aCalendar);
        }
    }

    public void setTimestamp(Timestamp aValue, Calendar aCalendar) throws SQLException
    {
        if (aValue == null)
        {
            mIsNull = true;
        }
        else
        {
            mIsNull = false;
            setTimestampImpl(aValue, aCalendar);
        }
    }
    
    public void setBigDecimal(BigDecimal aValue) throws SQLException
    {
        if (aValue == null)
        {
            mIsNull = true;
        }
        else
        {
            mIsNull = false;
            setBigDecimalImpl(aValue);
        }
    }

    public void setBinaryStream(InputStream aValue, long aLength) throws SQLException
    {
        if (aValue == null)
        {
            mIsNull = true;
        }
        else
        {
            mIsNull = false;
            setBinaryStreamImpl(aValue, aLength);
        }
    }
    
    public void setCharacterStream(Reader aValue, long aLength) throws SQLException
    {
        if (aValue == null)
        {
            mIsNull = true;
        }
        else
        {
            mIsNull = false;
            setCharacterStreamImpl(aValue, aLength);
        }
    }

    public void setRowId(RowIdBase aValue) throws SQLException
    {
        if (aValue == null)
        {
            mIsNull = true;
        }
        else
        {
            mIsNull = false;
            setRowIdImpl(aValue);
        }
    }
    
    public boolean getBoolean() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return BOOLEAN_NULL_VALUE;
        }
        return getBooleanImpl();
    }
    
    public byte getByte() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return BYTE_NULL_VALUE;
        }
        return getByteImpl();
    }
    
    public short getShort() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return SHORT_NULL_VALUE;
        }
        return getShortImpl();
    }

    public int getInt() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return INT_NULL_VALUE;
        }
        return getIntImpl();
    }

    public long getLong() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return LONG_NULL_VALUE;
        }
        return getLongImpl();
    }

    public float getFloat() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return FLOAT_NULL_VALUE;
        }
        return getFloatImpl();
    }

    public double getDouble() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return DOUBLE_NULL_VALUE;
        }
        return getDoubleImpl();
    }

    public String getString() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return null;
        }
        return getStringImpl();
    }

    public Object getObject() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return null;
        }
        return getObjectImpl();
    }

    public Date getDate(Calendar aCalendar) throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return null;
        }
        return getDateImpl(aCalendar);
    }

    public Time getTime(Calendar aCalendar) throws SQLException
    {        
        validateFetchedValue();
        if (mIsNull)
        {
            return null;
        }
        return getTimeImpl(aCalendar);
    }

    public Timestamp getTimestamp(Calendar aCalendar) throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return null;
        }
        return getTimestampImpl(aCalendar);
    }

    public BigDecimal getBigDecimal() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return null;
        }
        return getBigDecimalImpl();
    }

    public Reader getCharacterStream() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return null;
        }
        return getCharacterStreamImpl();
    }

    public RowIdBase getRowId() throws SQLException
    {
        validateFetchedValue();
        if (mIsNull)
        {
            return null;
        }
        return getRowIdImpl();
    }

    public byte[] getBytes() throws SQLException
    {
        validateFetchedValue();
        return mCache.readBytes(mId);
    }

    public InputStream getBinaryStream() throws SQLException
    {
        return new ByteArrayInputStream(getBytes());
    }
    
    public abstract long getArgNum1();
    
    public abstract long getArgNum2();
    
    protected abstract void setArgNum1(long aArgNum);
    
    protected abstract void setArgNum2(long aArgNum);
    
    protected abstract void loadValueFromCursorImpl() throws SQLException;
    
    protected abstract void storeValueToCursorImpl() throws SQLException;
    
    protected abstract void setBooleanImpl(boolean aValue) throws SQLException;
    
    protected abstract void setByteImpl(byte aValue) throws SQLException;
    
    protected abstract void setBytesImpl(byte[] aValue) throws SQLException;
    
    protected abstract void setShortImpl(short aValue) throws SQLException;
    
    protected abstract void setIntImpl(int aValue) throws SQLException;
    
    protected abstract void setLongImpl(long aValue) throws SQLException;
    
    protected abstract void setFloatImpl(float aValue) throws SQLException;
    
    protected abstract void setDoubleImpl(double aValue) throws SQLException;
    
    protected abstract void setStringImpl(String aValue) throws SQLException;
    
    protected abstract void setObjectImpl(Object aValue) throws SQLException;
    
    protected abstract void setDateImpl(Date aValue, Calendar aCalendar) throws SQLException;
    
    protected abstract void setTimeImpl(Time aValue, Calendar aCalendar) throws SQLException;
    
    protected abstract void setTimestampImpl(Timestamp aValue, Calendar aCalendar) throws SQLException;
    
    protected abstract void setBigDecimalImpl(BigDecimal aValue) throws SQLException;

    protected abstract void setBinaryStreamImpl(InputStream aValue, long aLength) throws SQLException;
    
    protected abstract void setCharacterStreamImpl(Reader aValue, long aLength) throws SQLException;
    
    protected abstract void setRowIdImpl(RowIdBase aValue) throws SQLException;
    
    protected abstract boolean getBooleanImpl() throws SQLException;
    
    protected abstract byte getByteImpl() throws SQLException;
    
    protected abstract short getShortImpl() throws SQLException;
    
    protected abstract int getIntImpl() throws SQLException;
    
    protected abstract long getLongImpl() throws SQLException;
    
    protected abstract float getFloatImpl() throws SQLException;
    
    protected abstract double getDoubleImpl() throws SQLException;
    
    protected abstract String getStringImpl() throws SQLException;
    
    protected abstract Object getObjectImpl() throws SQLException;
    
    protected abstract Date getDateImpl(Calendar aCalendar) throws SQLException;
    
    protected abstract Time getTimeImpl(Calendar aCalendar) throws SQLException;
    
    protected abstract Timestamp getTimestampImpl(Calendar aCalendar) throws SQLException;

    protected abstract BigDecimal getBigDecimalImpl() throws SQLException;
    
    protected abstract Reader getCharacterStreamImpl() throws SQLException;
    
    protected abstract RowIdBase getRowIdImpl() throws SQLException;
}