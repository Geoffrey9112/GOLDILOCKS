package sunje.goldilocks.jdbc.dt;

import java.sql.SQLException;

import sunje.goldilocks.sql.GoldilocksInterval;

public abstract class ColumnIntervalDayTimeCommon extends ColumnInterval
{
    private int mDay;
    private long mTime;
    
    ColumnIntervalDayTimeCommon(CodeColumn aCode)
    {
        super(aCode);
    }
    
    protected int getValueSize()
    {
        return 16;
    }

    protected void setDayTime(GoldilocksInterval aInterval) throws SQLException
    {
        mDay = aInterval.getDay();
        mTime = aInterval.getAccumulatedMicroSecond() - mDay * 24 * 3600000000l;
        mDay = aInterval.getSign() * mDay;
        mTime = aInterval.getSign() * mTime;
    }

    protected boolean getSign()
    {
        /*
         * 음수인 경우 day와 time 모두 음수이다.
         */
        return mDay >= 0 && mTime >= 0;
    }
    
    protected int getDay()
    {
        return Math.abs(mDay);
    }
    
    protected int getHour()
    {
        return Math.abs((int)(mTime / 3600000000l));
    }
    
    protected int getMinute()
    {
        return Math.abs((int)((mTime % 3600000000l) / 60000000));
    }
    
    protected int getSecond()
    {
        return Math.abs((int)(mTime % 60000000) / 1000000);
    }
    
    protected int getMicroSec()
    {
        return Math.abs((int)(mTime % 1000000));
    }
    
    protected void loadValueFromCursorImpl() throws SQLException
    {
        mValueBuffer.clear();
        mCache.readBytes(mId, mValue);
        mTime = mValueBuffer.getLong();
        mDay = mValueBuffer.getInt();
        
        /* value에 있는 indicator는 필요없다.
         * 이미 mCode의 getDefaultIntervalIndicator()를 통해 얻어올 수 있다. 
         */
    }

    protected void storeValueToCursorImpl() throws SQLException
    {
        mValueBuffer.clear();
        mValueBuffer.putLong(mTime);
        mValueBuffer.putInt(mDay);
        mValueBuffer.putInt(mIntervalIndicator);
        mCache.writeBytes(mId, mValue, mValue.length);
    }
}
