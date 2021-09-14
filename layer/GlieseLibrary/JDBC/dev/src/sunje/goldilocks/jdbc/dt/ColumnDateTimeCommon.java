package sunje.goldilocks.jdbc.dt;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.sql.Date;
import java.sql.SQLException;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.Calendar;
import java.util.TimeZone;

import sunje.goldilocks.jdbc.core4.RowIdBase;
import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.util.Utils;

public abstract class ColumnDateTimeCommon extends Column
{
    /* 2000년 1월 1일과 1970년 1월 1일 사이의 날 수 */
    protected static final long USEC_PER_DAY = 86400000000l;
    protected static final long USEC_PER_SEC = 1000000;
    protected static final int EPOCH_JDATE = 2451545;
    protected static final long USEC_PER_HOUR = 3600000000l;
    protected static final long USEC_PER_MINUTE = 60000000;
    protected static final int NSEC_PER_USEC = 1000;
    protected static final int USEC_PER_MSEC = 1000;
    protected static final int MSEC_PER_SEC = 1000;
    
    private static final int DATETIME_TARGET_TYPE_DATE = 1;
    private static final int DATETIME_TARGET_TYPE_TIME = 2;
    private static final int DATETIME_TARGET_TYPE_TIMESTAMP = 3;
    
    protected long mGoldilocksTsValue; // for getter
    
    protected long mFractionalSecPrecision;
 
    ColumnDateTimeCommon(CodeColumn aCode)
    {
        super(aCode);
    }
    
    protected abstract long getTsValueDateTimeSpecific();
    
    protected void setTimeZone(TimeZone aTimeZone)
    {
        /* Date, Time, Timestamp는 TimeZone 값을 사용하지 않기 때문에 아무런 작업을 하지 않는다.
         * Time with TimeZone과 Timestamp with TimeZone은 이 값을 서버로 전달해야 한다. 
         */
    }
    
    public long getArgNum1()
    {
        return mFractionalSecPrecision;
    }
    
    public long getArgNum2()
    {
        return mScale;
    }

    protected void setArgNum1(long aArgNum)
    {
        mPrecision = CodeColumn.DEFAULT_PRECISION_NA;
        mFractionalSecPrecision = aArgNum;
    }
    
    protected void setArgNum2(long aArgNum)
    {
        mScale = CodeColumn.SCALE_NA;
        /* timestamp 계열 타입은 arg2를 사용하지 않는다. */
    }
    
    protected void loadValueFromCursorImpl() throws SQLException
    {
        mGoldilocksTsValue = mCache.readLong(mId);
    }

    protected void storeValueToCursorImpl() throws SQLException
    {
        mCache.writeLong(mId, getTsValueDateTimeSpecific());
    }

    protected void setBooleanImpl(boolean aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "boolean", mCode.getTypeName());
    }

    protected void setByteImpl(byte aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "byte", mCode.getTypeName());
    }

    protected void setBytesImpl(byte[] aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "byte[]", mCode.getTypeName());
    }
    
    protected void setShortImpl(short aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "short", mCode.getTypeName());
    }

    protected void setIntImpl(int aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "int", mCode.getTypeName());
    }

    protected void setLongImpl(long aValue) throws SQLException
    {
        Timestamp sTimestamp = new Timestamp(aValue);
        setTimestampImpl(sTimestamp, Utils.getLocalCalendar());
    }

    protected void setFloatImpl(float aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "float", mCode.getTypeName());
    }

    protected void setDoubleImpl(double aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "double", mCode.getTypeName());
    }

    protected void setObjectImpl(Object aValue) throws SQLException
    {
        if (aValue instanceof Timestamp)
        {
            setTimestampImpl((Timestamp)aValue, Utils.getLocalCalendar());
        }
        else if (aValue instanceof Time)
        {
            setTimeImpl((Time)aValue, Utils.getLocalCalendar());
        }
        else if (aValue instanceof Date)
        {
            setDateImpl((Date)aValue, Utils.getLocalCalendar());
        }
        else
        {
            ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, aValue.getClass().getName(), mCode.getTypeName());
        }
    }

    protected void setDateImpl(Date aValue, Calendar aCalendar) throws SQLException
    {
        if (aCalendar == null)
        {
            aCalendar = Utils.getLocalCalendar();
        }
        setTimeZone(aCalendar.getTimeZone());
        mGoldilocksTsValue = javaTsToGoldilocksTs(aValue, aCalendar);
    }
    
    protected void setTimeImpl(Time aValue, Calendar aCalendar) throws SQLException
    {
        if (aCalendar == null)
        {
            aCalendar = Utils.getLocalCalendar();
        }
        setTimeZone(aCalendar.getTimeZone());
        mGoldilocksTsValue = javaTsToGoldilocksTs(aValue, aCalendar);
    }
    
    protected void setTimestampImpl(Timestamp aValue, Calendar aCalendar) throws SQLException
    {
        if (aCalendar == null)
        {
            aCalendar = Utils.getLocalCalendar();
        }
        setTimeZone(aCalendar.getTimeZone());
        mGoldilocksTsValue = javaTsToGoldilocksTs(aValue, aCalendar);
    }

    protected void setBigDecimalImpl(BigDecimal aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "BigDecimal", mCode.getTypeName());
    }

    protected void setBinaryStreamImpl(InputStream aValue, long aLength) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "InputStream", mCode.getTypeName());
    }
    
    protected void setCharacterStreamImpl(Reader aValue, long aLength) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "Reader", mCode.getTypeName());
    }
    
    protected void setRowIdImpl(RowIdBase aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "RowId", mCode.getTypeName());
    }

    protected boolean getBooleanImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "boolean");
        return false;
    }

    protected byte getByteImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "byte");
        return 0;
    }

    protected short getShortImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "short");
        return 0;
    }

    protected int getIntImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "int");
        return 0;
    }

    protected long getLongImpl() throws SQLException
    {
        return getTimestampImpl(Utils.getLocalCalendar()).getTime();
    }

    protected float getFloatImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "float");
        return 0;
    }

    protected double getDoubleImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "double");
        return 0;
    }

    protected Date getDateImpl(Calendar aCalendar) throws SQLException
    {
        if (aCalendar == null)
        {
            aCalendar = Utils.getLocalCalendar();
        }
        return goldilocksTsToJavaDate(mGoldilocksTsValue, aCalendar);
    }
    
    protected Time getTimeImpl(Calendar aCalendar) throws SQLException
    {
        if (aCalendar == null)
        {
            aCalendar = Utils.getLocalCalendar();
        }
        return goldilocksTsToJavaTime(mGoldilocksTsValue, aCalendar);
    }
    
    protected Timestamp getTimestampImpl(Calendar aCalendar) throws SQLException
    {
        if (aCalendar == null)
        {
            aCalendar = Utils.getLocalCalendar();
        }
        return goldilocksTsToJavaTimestamp(mGoldilocksTsValue, aCalendar);
    }

    protected BigDecimal getBigDecimalImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "BigDecimal");
        return null;
    }
    
    protected Reader getCharacterStreamImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "Reader");
        return null;
    }

    protected RowIdBase getRowIdImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "RowId");
        return null;
    }

    protected static long javaTsToGoldilocksTs(java.util.Date aValue, Calendar aCalendar)
    {
        /* dtdDateTimeCommon.c의 구현을 가져온 것이다. */

        aCalendar.setTime(aValue);
        
        int sYear = aCalendar.get(Calendar.YEAR);
        int sMonth = aCalendar.get(Calendar.MONTH) + 1;
        int sDay = aCalendar.get(Calendar.DATE);
        int sHour = aCalendar.get(Calendar.HOUR_OF_DAY);
        int sMinute = aCalendar.get(Calendar.MINUTE);
        int sSecond = aCalendar.get(Calendar.SECOND);
        int sFractional = 0;
        int sJulian;
        int sCentury;
        
        if( sMonth > 2 )
        {
            sMonth += 1;
            sYear += 4800;
        }
        else
        {
            sMonth += 13;
            sYear += 4799;
        }

        if (aValue instanceof Timestamp)
        {
            sFractional = ((Timestamp)aValue).getNanos() / NSEC_PER_USEC;
        }
        else
        {
            sFractional = aCalendar.get(Calendar.MILLISECOND) * USEC_PER_MSEC;
        }
        
        sCentury = sYear / 100;
        sJulian = sYear * 365 - 32167;
        sJulian += sYear / 4 - sCentury + sCentury / 4;
        sJulian += 7834 * sMonth / 256 + sDay;
        sJulian -= EPOCH_JDATE;
        
        if (aValue instanceof java.sql.Date)
        {
            /* Date 컬럼에는 년월일뿐만 아니라 시간 정보도 들어간다. 단 fractional은 안들어간다. */
            return sJulian * USEC_PER_DAY + sHour * USEC_PER_HOUR + sMinute * USEC_PER_MINUTE + sSecond * USEC_PER_SEC;
        }
        else if (aValue instanceof Time)
        {
            return sHour * USEC_PER_HOUR + sMinute * USEC_PER_MINUTE + sSecond * USEC_PER_SEC + sFractional;
        }
        else if (aValue instanceof Timestamp)
        {
            return sJulian * USEC_PER_DAY + sHour * USEC_PER_HOUR + sMinute * USEC_PER_MINUTE + sSecond * USEC_PER_SEC + sFractional;
        }
        
        ErrorMgr.raiseRuntimeError("Invalid java.util.Date Object: " + aValue);
        return 0;
    }

    private static java.util.Date goldilocksTsToJavaTs(long aValue, Calendar aCalendar, int aTargetType)
    {
        java.util.Date sResult = null;
        
        /* 이 메소드는 dtdDateTimeCommon.c의 날짜 추출 구현을 그대로 옮겨온 것이다. */
        long sDays = aValue / USEC_PER_DAY;
        long sTime = aValue - sDays * USEC_PER_DAY;
        long sJulian;
        long sQuad;
        long sExtra;
        int sYear;
        int sMonth;
        int sDay;
        int sHour;
        int sMinute;
        int sSecond;
        int sMicroSec;

        if (sTime < 0)
        {
            sTime += USEC_PER_DAY;
            sDays--;
        }
        
        sDays += EPOCH_JDATE;
        sJulian = sDays;
        sJulian += 32044;
        sQuad = sJulian / 146097;
        sExtra = (sJulian - sQuad * 146097) * 4 + 3;
        sJulian += 60 + sQuad * 3 + sExtra / 146097;
        sQuad = sJulian / 1461;
        sJulian -= sQuad * 1461;
        sYear = (int)(sJulian * 4 / 1461);
        sJulian = ((sYear != 0) ? ((sJulian + 305) % 365) : ((sJulian + 306) % 366));
        sJulian += 123;

        sYear += sQuad * 4;
        sYear -= 4800;
        sQuad = sJulian * 2141 / 65536;
        sDay = (int)(sJulian - 7834 * sQuad / 256);
        sMonth = (int)((sQuad + 10) % 12 + 1);

        sHour = (int)(sTime / USEC_PER_HOUR);
        sTime -= sHour * USEC_PER_HOUR;
        sMinute = (int)(sTime / USEC_PER_MINUTE);
        sTime -= sMinute * USEC_PER_MINUTE;
        sSecond = (int)(sTime / USEC_PER_SEC);
        sMicroSec = (int)(sTime - (sSecond * USEC_PER_SEC));

        aCalendar.set(sYear, sMonth - 1, sDay, sHour, sMinute, sSecond);
        aCalendar.set(Calendar.MILLISECOND, sMicroSec / USEC_PER_MSEC);
        
        if (aTargetType == DATETIME_TARGET_TYPE_DATE)
        {
            sResult = new java.sql.Date(aCalendar.getTimeInMillis()); 
        }
        else if (aTargetType == DATETIME_TARGET_TYPE_TIME)
        {
            sResult = new Time(aCalendar.getTimeInMillis()); 
        }
        else if (aTargetType == DATETIME_TARGET_TYPE_TIMESTAMP)
        {
            sResult = new Timestamp(aCalendar.getTimeInMillis()); 
            ((Timestamp)sResult).setNanos(((Timestamp)sResult).getNanos() + (sMicroSec % USEC_PER_MSEC) * NSEC_PER_USEC);            
        }
        return sResult;
    }
    
    public static Timestamp goldilocksTsToJavaTimestamp(long aValue, Calendar aCalendar)
    {
        return (Timestamp)goldilocksTsToJavaTs(aValue, aCalendar, DATETIME_TARGET_TYPE_TIMESTAMP);
    }
    
    public static Date goldilocksTsToJavaDate(long aValue, Calendar aCalendar)
    {
        return (Date)goldilocksTsToJavaTs(aValue, aCalendar, DATETIME_TARGET_TYPE_DATE);
    }
    
    public static Time goldilocksTsToJavaTime(long aValue, Calendar aCalendar)
    {
        return (Time)goldilocksTsToJavaTs(aValue, aCalendar, DATETIME_TARGET_TYPE_TIME);
    }
}
