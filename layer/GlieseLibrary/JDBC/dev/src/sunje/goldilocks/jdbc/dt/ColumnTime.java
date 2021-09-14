package sunje.goldilocks.jdbc.dt;

import java.sql.SQLException;
import java.util.Calendar;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.util.Utils;


public class ColumnTime extends ColumnTimestamp
{
    ColumnTime(CodeColumn aCode)
    {
        super(aCode);
    }

    protected long getTsValueDateTimeSpecific()
    {
        /* Time 컬럼에는 시간 정보만 넣는다. */
        return mGoldilocksTsValue % USEC_PER_DAY;
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        try
        {
            java.sql.Time sTime = new java.sql.Time(mCache.getEnv().getTimeFormat().parse(aValue).getTime());
            Calendar sCal = Utils.getLocalCalendar();
            setTimeZone(sCal.getTimeZone());
            mGoldilocksTsValue = javaTsToGoldilocksTs(sTime, sCal);
        }
        catch (java.text.ParseException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "TIME");
        }
    }
    
    protected String getStringImpl() throws SQLException
    {
        return mCache.getEnv().getTimeFormat().format(getTimeImpl(Utils.getLocalCalendar()));
    }
    
    protected Object getObjectImpl() throws SQLException
    {
        return getTimeImpl(Utils.getLocalCalendar());
    }
}
