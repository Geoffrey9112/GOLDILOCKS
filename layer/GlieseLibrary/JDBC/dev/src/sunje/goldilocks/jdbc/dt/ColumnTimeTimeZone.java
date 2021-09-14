package sunje.goldilocks.jdbc.dt;

import java.sql.SQLException;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.util.Utils;

public class ColumnTimeTimeZone extends ColumnTimeZoneCommon
{
    ColumnTimeTimeZone(CodeColumn aCode)
    {
        super(aCode);
    }
    
    protected long getTsValueDateTimeSpecific()
    {
        /* Time 컬럼에는 시간 정보만 넣는다. */
        return mGoldilocksTsValue % USEC_PER_DAY;
    }

    protected void storeValueToCursorImpl() throws SQLException
    {
        mValueBuffer.clear();

        long sTime = mGoldilocksTsValue + getTimeZoneOffsetUSec();
        
        if (mGoldilocksTsValue < 0)
        {
            sTime += USEC_PER_DAY;
        }
        else if (mGoldilocksTsValue >= USEC_PER_DAY)
        {
            sTime -= USEC_PER_DAY;
        }

        mValueBuffer.putLong(sTime);
        mValueBuffer.putInt(mTimeZoneOffset);
        mCache.writeBytes(mId, mValueBytes, mValueBytes.length);
    }
    
    protected void setStringImpl(String aValue) throws SQLException
    {
        try
        {
            java.sql.Time sTime = new java.sql.Time(mCache.getEnv().getTimeTZFormat().parse(aValue).getTime());
            Calendar sCal = Utils.getLocalCalendar(); /* setString을 통해서는 timezone 정보를 넣을 수 없다. 무조건 로컬 타임존으로 세팅 */
            setTimeZone(sCal.getTimeZone());
            mGoldilocksTsValue = javaTsToGoldilocksTs(sTime, sCal);
        }
        catch (java.text.ParseException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "TIME WITH TIMEZONE");
        }
    }
    
    protected String getStringImpl() throws SQLException
    {
        Calendar sCal = Utils.getCalendar(-mTimeZoneOffset);
        SimpleDateFormat sFormat = (SimpleDateFormat)mCache.getEnv().getTimeTZFormat().clone();
        sFormat.setTimeZone(sCal.getTimeZone());
        return sFormat.format(getTimeImpl(sCal));
    }
    
    protected Object getObjectImpl() throws SQLException
    {
        return getTimeImpl(null);
    }
}
