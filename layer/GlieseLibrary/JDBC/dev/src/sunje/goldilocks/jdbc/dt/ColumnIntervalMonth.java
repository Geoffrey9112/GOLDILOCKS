package sunje.goldilocks.jdbc.dt;

import java.math.BigDecimal;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.sql.GoldilocksInterval;

public class ColumnIntervalMonth extends ColumnIntervalYearMonthCommon
{
    ColumnIntervalMonth(CodeColumn aCode)
    {
        super(aCode);
    }
    
    private void setMonth(GoldilocksInterval aInterval) throws SQLException
    {
        mMonth = aInterval.getSign() * (aInterval.getYear() * 12 + aInterval.getMonth());
    }
    
    protected void setByteImpl(byte aValue) throws SQLException
    {
        setMonth(GoldilocksInterval.createIntervalMonth((int)mPrecision, aValue >= 0, Math.abs(aValue)));
    }

    protected void setShortImpl(short aValue) throws SQLException
    {
        setMonth(GoldilocksInterval.createIntervalMonth((int)mPrecision, aValue >= 0, Math.abs(aValue)));
    }

    protected void setIntImpl(int aValue) throws SQLException
    {
        setMonth(GoldilocksInterval.createIntervalMonth((int)mPrecision, aValue >= 0, Math.abs(aValue)));
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
        setMonth(GoldilocksInterval.createIntervalMonth((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
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
        setMonth(GoldilocksInterval.createIntervalMonth((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
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
        setMonth(GoldilocksInterval.createIntervalMonth((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        setMonth(GoldilocksInterval.createIntervalMonth((int)mPrecision, aValue));
    }

    protected void setObjectImpl(Object aValue) throws SQLException
    {
        if (!(aValue instanceof GoldilocksInterval))
        {
            ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, aValue.getClass().getName(), mCode.getTypeName());
        }
        setMonth((GoldilocksInterval)aValue);
    }

    protected void setBigDecimalImpl(BigDecimal aValue) throws SQLException
    {
        setMonth(GoldilocksInterval.createIntervalMonth((int)mPrecision, aValue.intValue() >= 0, Math.abs(aValue.intValue())));
    }

    protected byte getByteImpl() throws SQLException
    {
        return (byte)mMonth;
    }

    protected short getShortImpl() throws SQLException
    {
        return (short)mMonth;
    }

    protected int getIntImpl() throws SQLException
    {
        return mMonth;
    }

    protected long getLongImpl() throws SQLException
    {
        return mMonth;
    }

    protected float getFloatImpl() throws SQLException
    {
        return mMonth;
    }

    protected double getDoubleImpl() throws SQLException
    {
        return mMonth;
    }

    protected String getStringImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalMonth((int)mPrecision, mMonth >= 0, Math.abs(mMonth)).toString();
    }

    protected Object getObjectImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalMonth((int)mPrecision, mMonth >= 0, Math.abs(mMonth));
    }

    protected BigDecimal getBigDecimalImpl() throws SQLException
    {
        return new BigDecimal(mMonth);
    }
}
