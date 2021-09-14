package sunje.goldilocks.jdbc.dt;

import java.sql.SQLException;
import java.util.Calendar;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.util.Utils;

public class ColumnDate extends ColumnDateTimeCommon
{
    ColumnDate(CodeColumn aCode)
    {
        super(aCode);
    }
    
    protected long getTsValueDateTimeSpecific()
    {
        /* Date 컬럼에는 fractional 값을 뺀다. */
        return mGoldilocksTsValue / USEC_PER_SEC * USEC_PER_SEC;
    }
    
    protected void setStringImpl(String aValue) throws SQLException
    {
        try
        {
            java.sql.Date sDate = new java.sql.Date(mCache.getEnv().getDateFormat().parse(aValue).getTime());
            Calendar sCal = Utils.getLocalCalendar();
            setTimeZone(sCal.getTimeZone());
            mGoldilocksTsValue = javaTsToGoldilocksTs(sDate, sCal);
        }
        catch (java.text.ParseException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "DATE");
        }
    }
    
    protected String getStringImpl() throws SQLException
    {
        return mCache.getEnv().getDateFormat().format(getDateImpl(Utils.getLocalCalendar()));
    }
    
    protected Object getObjectImpl() throws SQLException
    {
        return getDateImpl(Utils.getLocalCalendar());
    }
}
