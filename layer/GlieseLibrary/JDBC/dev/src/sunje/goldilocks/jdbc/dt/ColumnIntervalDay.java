package sunje.goldilocks.jdbc.dt;

import java.math.BigDecimal;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.sql.GoldilocksInterval;

public class ColumnIntervalDay extends ColumnIntervalDayTimeCommon
{
    ColumnIntervalDay(CodeColumn aCode)
    {
        super(aCode);
    }
    
    protected void setByteImpl(byte aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalDay((int)mPrecision, aValue >= 0, Math.abs(aValue)));
    }

    protected void setShortImpl(short aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalDay((int)mPrecision, aValue >= 0, Math.abs(aValue)));
    }

    protected void setIntImpl(int aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalDay((int)mPrecision, aValue >= 0, Math.abs(aValue)));
    }

    protected void setLongImpl(long aValue) throws SQLException
    {
        if (aValue > Integer.MAX_VALUE)
        {
            aValue = Integer.MAX_VALUE;
        }
        if (aValue < Integer.MIN_VALUE)
        {
            aValue = Integer.MIN_VALUE;
        }
        setDayTime(GoldilocksInterval.createIntervalDay((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
    }

    protected void setFloatImpl(float aValue) throws SQLException
    {
        if (aValue > Integer.MAX_VALUE)
        {
            aValue = Integer.MAX_VALUE;
        }
        if (aValue < Integer.MIN_VALUE)
        {
            aValue = Integer.MIN_VALUE;
        }
        setDayTime(GoldilocksInterval.createIntervalDay((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
    }

    protected void setDoubleImpl(double aValue) throws SQLException
    {
        if (aValue > Integer.MAX_VALUE)
        {
            aValue = Integer.MAX_VALUE;
        }
        if (aValue < Integer.MIN_VALUE)
        {
            aValue = Integer.MIN_VALUE;
        }
        setDayTime(GoldilocksInterval.createIntervalDay((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalDay((int)mPrecision, aValue));
    }

    protected void setObjectImpl(Object aValue) throws SQLException
    {
        if (!(aValue instanceof GoldilocksInterval))
        {
            ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, aValue.getClass().getName(), mCode.getTypeName());
        }
        setDayTime((GoldilocksInterval)aValue);
    }

    protected void setBigDecimalImpl(BigDecimal aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalDay((int)mPrecision, aValue.intValue() >= 0, Math.abs(aValue.intValue())));
    }

    protected byte getByteImpl() throws SQLException
    {
        return (byte)getDay();
    }

    protected short getShortImpl() throws SQLException
    {
        return (short)getDay();
    }

    protected int getIntImpl() throws SQLException
    {
        return getDay();
    }

    protected long getLongImpl() throws SQLException
    {
        return getDay();
    }

    protected float getFloatImpl() throws SQLException
    {
        return getDay();
    }

    protected double getDoubleImpl() throws SQLException
    {
        return getDay();
    }

    protected String getStringImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalDay((int)mPrecision, getSign(), getDay()).toString();
    }

    protected Object getObjectImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalDay((int)mPrecision, getSign(), getDay());
    }

    protected BigDecimal getBigDecimalImpl() throws SQLException
    {
        return new BigDecimal(getDay());
    }
}
