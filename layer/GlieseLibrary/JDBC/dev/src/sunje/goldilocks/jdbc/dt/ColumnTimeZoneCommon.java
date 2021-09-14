package sunje.goldilocks.jdbc.dt;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.sql.Date;
import java.sql.SQLException;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.Calendar;
import java.util.TimeZone;

import sunje.goldilocks.jdbc.util.Utils;

public abstract class ColumnTimeZoneCommon extends ColumnDateTimeCommon
{
    protected static final int TIMESTAMP_TIMEZONE_DATA_SIZE = 12;

    protected byte[] mValueBytes;
    protected ByteBuffer mValueBuffer;
    protected int mTimeZoneOffset;
    
    ColumnTimeZoneCommon(CodeColumn aCode)
    {
        super(aCode);
        
        mValueBytes = new byte[TIMESTAMP_TIMEZONE_DATA_SIZE];
        mValueBuffer = ByteBuffer.wrap(mValueBytes);
    }
    
    protected long getTimeZoneOffsetUSec()
    {
        return (long)mTimeZoneOffset * MSEC_PER_SEC * USEC_PER_MSEC;
    }
    
    protected void setTimeZone(TimeZone aTimeZone)
    {
        mTimeZoneOffset = -aTimeZone.getRawOffset() / MSEC_PER_SEC;
    }
    
    protected void setColumnSpec(int aId,
            long aArgNum1,
            long aArgNum2,
            byte aLenUnit,
            byte aIntervalInd,
            byte aBindType,
            RowCache aCache)
    {
        super.setColumnSpec(aId, aArgNum1, aArgNum2, aLenUnit, aIntervalInd, aBindType, aCache);
        if (mCache.isLittleEndian())
        {
            mValueBuffer.order(ByteOrder.LITTLE_ENDIAN);
        }
        else
        {
            mValueBuffer.order(ByteOrder.BIG_ENDIAN);
        }
    }

    protected void loadValueFromCursorImpl() throws SQLException
    {
        mValueBuffer.clear();
        mCache.readBytes(mId, mValueBytes);
        long sGMTTsValue = mValueBuffer.getLong();
        mTimeZoneOffset = mValueBuffer.getInt();
        mGoldilocksTsValue = sGMTTsValue - getTimeZoneOffsetUSec();
    }

    protected Date getDateImpl(Calendar aCalendar) throws SQLException
    {
        if (aCalendar == null)
        {
            aCalendar = Utils.getCalendar(-mTimeZoneOffset);
        }
        return goldilocksTsToJavaDate(mGoldilocksTsValue, aCalendar);
    }
    
    protected Time getTimeImpl(Calendar aCalendar) throws SQLException
    {
        if (aCalendar == null)
        {
            aCalendar = Utils.getCalendar(-mTimeZoneOffset);
        }
        return goldilocksTsToJavaTime(mGoldilocksTsValue, aCalendar);
    }
    
    protected Timestamp getTimestampImpl(Calendar aCalendar) throws SQLException
    {
        if (aCalendar == null)
        {
            aCalendar = Utils.getCalendar(-mTimeZoneOffset);
        }
        return goldilocksTsToJavaTimestamp(mGoldilocksTsValue, aCalendar);
    }
}
