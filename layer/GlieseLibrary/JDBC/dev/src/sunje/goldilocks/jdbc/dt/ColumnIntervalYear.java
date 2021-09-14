package sunje.goldilocks.jdbc.dt;

import java.math.BigDecimal;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.sql.GoldilocksInterval;

public class ColumnIntervalYear extends ColumnIntervalYearMonthCommon
{
    ColumnIntervalYear(CodeColumn aCode)
    {
        super(aCode);
    }

    private void setYear(GoldilocksInterval aInterval) throws SQLException
    {
        mMonth = aInterval.getSign() * aInterval.getYear() * 12;
    }
    
    protected void setByteImpl(byte aValue) throws SQLException
    {
        setYear(GoldilocksInterval.createIntervalYear((int)mPrecision, aValue >= 0, Math.abs(aValue)));
    }

    protected void setShortImpl(short aValue) throws SQLException
    {
        setYear(GoldilocksInterval.createIntervalYear((int)mPrecision, aValue >= 0, Math.abs(aValue)));
    }

    protected void setIntImpl(int aValue) throws SQLException
    {
        setYear(GoldilocksInterval.createIntervalYear((int)mPrecision, aValue >= 0, Math.abs(aValue)));
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
        setYear(GoldilocksInterval.createIntervalYear((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
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
        setYear(GoldilocksInterval.createIntervalYear((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
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
        setYear(GoldilocksInterval.createIntervalYear((int)mPrecision, aValue >= 0, (int)Math.abs(aValue)));
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        setYear(GoldilocksInterval.createIntervalYear((int)mPrecision, aValue));
    }

    protected void setObjectImpl(Object aValue) throws SQLException
    {
        if (!(aValue instanceof GoldilocksInterval))
        {
            ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, aValue.getClass().getName(), mCode.getTypeName());
        }
        setYear((GoldilocksInterval)aValue);
    }

    protected void setBigDecimalImpl(BigDecimal aValue) throws SQLException
    {
        setYear(GoldilocksInterval.createIntervalYear((int)mPrecision, aValue.intValue() >= 0, (int)Math.abs(aValue.intValue())));
    }

    protected byte getByteImpl() throws SQLException
    {
        return (byte)(mMonth / 12);
    }

    protected short getShortImpl() throws SQLException
    {
        return (short)(mMonth / 12);
    }

    protected int getIntImpl() throws SQLException
    {
        return mMonth / 12;
    }

    protected long getLongImpl() throws SQLException
    {
        return mMonth / 12;
    }

    protected float getFloatImpl() throws SQLException
    {
        return mMonth / 12;
    }

    protected double getDoubleImpl() throws SQLException
    {
        return mMonth / 12;
    }

    protected String getStringImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalYear((int)mPrecision, mMonth >= 0, Math.abs(mMonth) / 12).toString();
    }

    protected Object getObjectImpl() throws SQLException
    {
        return GoldilocksInterval.createIntervalYear((int)mPrecision, mMonth >= 0, Math.abs(mMonth) / 12);
    }

    protected BigDecimal getBigDecimalImpl() throws SQLException
    {
        return new BigDecimal(mMonth / 12);
    }
}
