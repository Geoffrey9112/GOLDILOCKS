package sunje.goldilocks.jdbc.dt;

import java.math.BigDecimal;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.sql.GoldilocksInterval;

public class ColumnIntervalSecond extends ColumnIntervalDayTimeCommon
{
    ColumnIntervalSecond(CodeColumn aCode)
    {
        super(aCode);
    }
    
    protected void setByteImpl(byte aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, aValue >= 0, Math.abs(aValue), 0));
    }

    protected void setShortImpl(short aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, aValue >= 0, Math.abs(aValue), 0));
    }

    protected void setIntImpl(int aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, aValue >= 0, Math.abs(aValue), 0));
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
        setDayTime(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, aValue >= 0, (int)Math.abs(aValue), 0));
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
        setDayTime(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, aValue >= 0, Math.abs(aValue)));
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
        setDayTime(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, aValue >= 0, Math.abs(aValue)));
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        setDayTime(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, aValue));
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
        setDayTime(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, aValue.doubleValue() >= 0, Math.abs(aValue.doubleValue())));
    }

    protected byte getByteImpl() throws SQLException
    {
        return (byte)(Math.round((float)(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, getSign(), getDay(), getHour(), getMinute(), getSecond(), getMicroSec()).getAccumulatedMicroSecond() / 1000000.0)));
    }

    protected short getShortImpl() throws SQLException
    {
        return (short)(Math.round((float)(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, getSign(), getDay(), getHour(), getMinute(), getSecond(), getMicroSec()).getAccumulatedMicroSecond() / 1000000.0)));
    }

    protected int getIntImpl() throws SQLException
    {
        return Math.round((float)(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, getSign(), getDay(), getHour(), getMinute(), getSecond(), getMicroSec()).getAccumulatedMicroSecond() / 1000000.0));
    }

    protected long getLongImpl() throws SQLException
    {
        return Math.round((float)(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, getSign(), getDay(), getHour(), getMinute(), getSecond(), getMicroSec()).getAccumulatedMicroSecond() / 1000000.0));
    }

    protected float getFloatImpl() throws SQLException
    {
        return (float)(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, getSign(), getDay(), getHour(), getMinute(), getSecond(), getMicroSec()).getAccumulatedMicroSecond() / 1000000.0);
    }
    
    protected double getDoubleImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, getSign(), getDay(), getHour(), getMinute(), getSecond(), getMicroSec()).getAccumulatedMicroSecond() / 1000000.0;
    }

    protected String getStringImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, getSign(), getDay(), getHour(), getMinute(), getSecond(), getMicroSec()).toString();
    }

    protected Object getObjectImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, getSign(), getDay(), getHour(), getMinute(), getSecond(), getMicroSec());
    }

    protected BigDecimal getBigDecimalImpl() throws SQLException
    {
        return new BigDecimal(GoldilocksInterval.createIntervalSecond((int)mPrecision, (int)mScale, getSign(), getDay(), getHour(), getMinute(), getSecond(), getMicroSec()).getAccumulatedMicroSecond() / 1000000.0);
    }
}
