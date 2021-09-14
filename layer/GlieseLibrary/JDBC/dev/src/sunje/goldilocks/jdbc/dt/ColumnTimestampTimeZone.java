package sunje.goldilocks.jdbc.dt;

import java.sql.SQLException;
import java.sql.Timestamp;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.util.Utils;

public class ColumnTimestampTimeZone extends ColumnTimeZoneCommon
{
    private static String[] LEADING_ZEROS = { "", "0", "00", "000", "0000", "00000", "000000", "0000000", "00000000" };
    
    protected static String signString(int aValue)
    {
        if (aValue >= 0)
        {
            return "+";
        }
        return "-";
    }

    protected static String fourDigitString(int aValue)
    {
        String sSign = "";
        
        if (aValue < 0)
        {
            aValue = -aValue;
            sSign = "-";
        }
        if (aValue == 0)
        {
            return "0000";
        }
        else if (aValue < 10)
        {
            return sSign + "000" + aValue;
        }
        else if (aValue < 100)
        {
            return sSign + "00" + aValue;            
        }
        else if (aValue < 1000)
        {
            return sSign + "0" + aValue;
        }
        else
        {
            return sSign + aValue;
        }        
    }
    
    protected static String twoDigitString(int aValue)
    {
        if (aValue < 0)
        {
            aValue = -aValue;
        }
        if (aValue == 0)
        {
            return "00";
        }
        else if (aValue >= 10)
        {
            return String.valueOf(aValue);
        }
        else
        {
            return "0" + aValue;
        }
    }
    
    protected static String fractionString(int aValue)
    {
        /*
         * 이 메소드는 JDK1.4에서 String.format()을 지원하지 않아 구현한 것이다.
         * aPrecision은 6보다 크지 않다고 가정하고 작성하였다.
         */
        String sResult = String.valueOf(aValue);
        if (sResult.length() < 9)
        {
            sResult = LEADING_ZEROS[9 - sResult.length()] + sResult;
        }
        int i;
        for (i=sResult.length(); i>= 1; i--)
        {
            if (sResult.charAt(i-1) != '0')
            {
                break;
            }
        }
        return sResult.substring(0, i);
    }
    
    ColumnTimestampTimeZone(CodeColumn aCode)
    {
        super(aCode);
    }
    
    protected long getTsValueDateTimeSpecific()
    {
        /* Timestamp 컬럼에는 날짜, 시간 정보를 모두 넣는다. Date와 Time은 각각 날짜만, 시간만 넣는다. */
        return mGoldilocksTsValue;
    }

    protected void storeValueToCursorImpl() throws SQLException
    {
        mValueBuffer.clear();
        mValueBuffer.putLong(mGoldilocksTsValue + getTimeZoneOffsetUSec());
        mValueBuffer.putInt(mTimeZoneOffset);
        mCache.writeBytes(mId, mValueBytes, mValueBytes.length);
    }
    
    protected void setStringImpl(String aValue) throws SQLException
    {
        try
        {
            java.sql.Timestamp sTimestamp = new java.sql.Timestamp(mCache.getEnv().getTimestampTZFormat().parse(aValue).getTime());
            Calendar sCal = Utils.getLocalCalendar(); /* setString을 통해서는 timezone 정보를 넣을 수 없다. 무조건 로컬 타임존으로 세팅 */
            setTimeZone(sCal.getTimeZone());
            mGoldilocksTsValue = javaTsToGoldilocksTs(sTimestamp, sCal);
        }
        catch (java.text.ParseException sException)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "TIMESTAMP WITH TIMEZONE");
        }
    }
    
    protected String getStringImpl() throws SQLException
    {
        Calendar sCal = Utils.getCalendar(-mTimeZoneOffset);
        SimpleDateFormat sFormat = (SimpleDateFormat)mCache.getEnv().getTimestampTZFormat().clone();
        sFormat.setTimeZone(sCal.getTimeZone());
        return sFormat.format(getTimestampImpl(sCal));
    }
    
    protected Object getObjectImpl() throws SQLException
    {
        return getTimestampImpl(null);
    }
    
    protected String getStringImplOld() throws SQLException
    {
        Calendar sCal = Utils.getLocalCalendar();
        Timestamp sValue = goldilocksTsToJavaTimestamp(mGoldilocksTsValue, sCal);
        
        /*
         * Timestamp with time zone 컬럼 데이터를 getString으로 얻으면 time zone 정보를 같이 줘야 한다.
         */
        StringBuffer sBuf = new StringBuffer(40);
        sBuf.append(fourDigitString(sCal.get(Calendar.YEAR)))
            .append('-')
            .append(twoDigitString(sCal.get(Calendar.MONTH) + 1))
            .append('-')
            .append(twoDigitString(sCal.get(Calendar.DATE)))
            .append(' ')
            .append(twoDigitString(sCal.get(Calendar.HOUR_OF_DAY)))
            .append(':')
            .append(twoDigitString(sCal.get(Calendar.MINUTE)))
            .append(':')            
            .append(twoDigitString(sCal.get(Calendar.SECOND)));
        if (sValue.getNanos() > 0)
        {
            sBuf.append('.');
            int sNano = sValue.getNanos();
            sBuf.append(fractionString(sNano));
        }
        sBuf.append(' ')
            .append(signString(-mTimeZoneOffset))
            .append(twoDigitString(mTimeZoneOffset / 3600))
            .append(':')
            .append(twoDigitString((mTimeZoneOffset / 60) % 60));
        return sBuf.toString();
    }
}
