package sunje.goldilocks.jdbc.dt;

import java.math.BigDecimal;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.sql.GoldilocksInterval;

public class ColumnIntervalMinute extends ColumnIntervalDayTimeCommon
{
    ColumnIntervalMinute(CodeColumn aCode)
    {
        super(aCode);
    }
    
    protected void setByteImpl(byte aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalMinute((int)mPrecision, aValue >= 0, Math.abs(aValue)));
    }

    protected void setShortImpl(short aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalMinute((int)mPrecision, aValue >= 0, Math.abs(aValue)));
    }

    protected void setIntImpl(int aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalMinute((int)mPrecision, aValue >= 0, Math.abs(aValue)));
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
        setDayTime(GoldilocksInterval.createIntervalMinute((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
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
        setDayTime(GoldilocksInterval.createIntervalMinute((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
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
        setDayTime(GoldilocksInterval.createIntervalMinute((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalMinute((int)mPrecision, aValue));
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
        setDayTime(GoldilocksInterval.createIntervalMinute((int)mPrecision, aValue.intValue() >= 0, Math.abs(aValue.intValue())));
    }

    protected byte getByteImpl() throws SQLException
    {
        return (byte)GoldilocksInterval.createIntervalMinute((int)mPrecision, getSign(), getDay(), getHour(), getMinute()).getAccumulatedMinute();
    }

    protected short getShortImpl() throws SQLException
    {
        return (short)GoldilocksInterval.createIntervalMinute((int)mPrecision, getSign(), getDay(), getHour(), getMinute()).getAccumulatedMinute();
    }

    protected int getIntImpl() throws SQLException
    {
        return (int)GoldilocksInterval.createIntervalMinute((int)mPrecision, getSign(), getDay(), getHour(), getMinute()).getAccumulatedMinute();
    }

    protected long getLongImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalMinute((int)mPrecision, getSign(), getDay(), getHour(), getMinute()).getAccumulatedMinute();
    }

    protected float getFloatImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalMinute((int)mPrecision, getSign(), getDay(), getHour(), getMinute()).getAccumulatedMinute();
    }

    protected double getDoubleImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalMinute((int)mPrecision, getSign(), getDay(), getHour(), getMinute()).getAccumulatedMinute();
    }

    protected String getStringImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalMinute((int)mPrecision, getSign(), getDay(), getHour(), getMinute()).toString();
    }

    protected Object getObjectImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalMinute((int)mPrecision, getSign(), getDay(), getHour(), getMinute());
    }

    protected BigDecimal getBigDecimalImpl() throws SQLException
    {
        return new BigDecimal(GoldilocksInterval.createIntervalMinute((int)mPrecision, getSign(), getDay(), getHour(), getMinute()).getAccumulatedMinute());
    }
}
