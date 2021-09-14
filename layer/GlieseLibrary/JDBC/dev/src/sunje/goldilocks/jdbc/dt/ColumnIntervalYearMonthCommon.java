package sunje.goldilocks.jdbc.dt;

import java.sql.SQLException;

public abstract class ColumnIntervalYearMonthCommon extends ColumnInterval
{
    protected int mMonth;
    
    ColumnIntervalYearMonthCommon(CodeColumn aCode)
    {
        super(aCode);
    }
    
    protected int getValueSize()
    {
        return 8;
    }
    
    protected void loadValueFromCursorImpl() throws SQLException
    {
        mValueBuffer.clear();
        mCache.readBytes(mId, mValue);
        mMonth = mValueBuffer.getInt();
        /* value에 있는 indicator는 필요없다.
         * 이미 mCode의 getDefaultIntervalIndicator()를 통해 얻어올 수 있다. 
         */
    }

    protected void storeValueToCursorImpl() throws SQLException
    {
        mValueBuffer.clear();
        mValueBuffer.putInt(mMonth);
        mValueBuffer.putInt(mIntervalIndicator);
        mCache.writeBytes(mId, mValue, mValue.length);
    }
}
