package sunje.goldilocks.jdbc.dt;

import java.sql.Date;
import java.sql.SQLException;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.Calendar;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class ColumnNonDateTime extends Column
{
    ColumnNonDateTime(CodeColumn aCode)
    {
        super(aCode);
    }
    protected void setDateImpl(Date aValue, Calendar aCalendar) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "Date", mCode.getTypeName());
    }
    
    protected void setTimeImpl(Time aValue, Calendar aCalendar) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "Time", mCode.getTypeName());
    }
    
    protected void setTimestampImpl(Timestamp aValue, Calendar aCalendar) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "Timestamp", mCode.getTypeName());
    }
    protected Date getDateImpl(Calendar aCalendar) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "Date");
        return null;
    }
    
    protected Time getTimeImpl(Calendar aCalendar) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "Time");
        return null;
    }
    
    protected Timestamp getTimestampImpl(Calendar aCalendar) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "Timestamp");
        return null;
    }
}
