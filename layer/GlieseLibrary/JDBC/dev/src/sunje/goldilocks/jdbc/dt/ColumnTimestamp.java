package sunje.goldilocks.jdbc.dt;

import java.sql.SQLException;
import java.util.Calendar;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.util.Utils;

public class ColumnTimestamp extends ColumnDateTimeCommon
{
    ColumnTimestamp(CodeColumn aCode)
    {
        super(aCode);
    }

    protected long getTsValueDateTimeSpecific()
    {
        /* Timestamp 컬럼에는 날짜, 시간 정보를 모두 넣는다. Date와 Time은 각각 날짜만, 시간만 넣는다. */
        return mGoldilocksTsValue;
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        try
        {
            java.sql.Timestamp sTimestamp = new java.sql.Timestamp(mCache.getEnv().getTimestampFormat().parse(aValue).getTime());
            Calendar sCal = Utils.getLocalCalendar();
            setTimeZone(sCal.getTimeZone());
            mGoldilocksTsValue = javaTsToGoldilocksTs(sTimestamp, sCal);
        }
        catch (java.text.ParseException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "TIMESTAMP");
        }
    }
    
    protected String getStringImpl() throws SQLException
    {
        return mCache.getEnv().getTimestampFormat().format(getTimestampImpl(Utils.getLocalCalendar()));
    }
    
    protected Object getObjectImpl() throws SQLException
    {
        return getTimestampImpl(Utils.getLocalCalendar());
    }
}
