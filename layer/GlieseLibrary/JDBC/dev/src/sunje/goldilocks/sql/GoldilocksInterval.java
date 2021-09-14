package sunje.goldilocks.sql;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.GoldilocksDriver;
import sunje.goldilocks.jdbc.core4.VersionSpecific;

/**
 * Interval 타입들의 계층 관계
 * 
 *                                 Interval
 *                                    |
 *              +-------------------------------------------+
 *              |                                           |
 *    IntervalYearToMonth                            IntervalDayToSecond
 *              |                                           |
 *   +----------------------+             +----------------------------------------+
 *   |                      |             |                                        |
 * IntervalYear       IntervalMonth   IntervalDayToMinute               IntervalHourToSecond
 *                                        |                                        |
 *                                        |                            +---------------------------+
 *                                        |                            |                           |
 *                                    IntervalDayToHour        IntervalHourToMinute      IntervalMinuteToSecond
 *                                        |                            |                           |
 *                                        |                 +----------------+                     |
 *                                        |                 |                |                     |
 *                                    IntervalDay      IntervalHour     IntervalMinute       IntervalSecond
 * 
 * 
 * 
 * 
 * @author egon
 *
 */
public abstract class GoldilocksInterval
{
    static
    {
        /*
         * Driver 클래스를 로딩해야 VersionSpecific이 초기화된다.
         * log10 메소드때문에 VersionSpecific을 참조해야 한다.
         */
        GoldilocksDriver.DRIVER_SINGLETON.getClass();
    }
    
    protected static String INTERVAL_TYPE_NAME[] = {
        "NONE",
        "INTERVAL YEAR",
        "INTERVAL MONTH",
        "INTERVAL DAY",
        "INTERVAL HOUR",
        "INTERVAL MINUTE",
        "INTERVAL SECOND",
        "INTERVAL YEAR TO MONTH",
        "INTERVAL DAY TO HOUR",
        "INTERVAL DAY TO MINUTE",
        "INTERVAL DAY TO SECOND",
        "INTERVAL HOUR TO MINUTE",
        "INTERVAL HOUR TO SECOND",
        "INTERVAL MINUTE TO SECOND"
    };
    
    public static GoldilocksInterval createIntervalYearToMonth(int aYearPrecision, boolean aSign, int aYear, int aMonth) throws SQLException
    {
        return new IntervalYearToMonth(aYearPrecision, aSign, aYear, aMonth);
    }

    public static GoldilocksInterval createIntervalYearToMonth(int aYearPrecision, String aYearToMonth) throws SQLException
    {
        return new IntervalYearToMonth(aYearPrecision, aYearToMonth);
    }
    
    public static GoldilocksInterval createIntervalYear(int aYearPrecision, boolean aSign, int aYear) throws SQLException
    {
        return new IntervalYear(aYearPrecision, aSign, aYear);
    }
    
    public static GoldilocksInterval createIntervalYear(int aYearPrecision, String aYear) throws SQLException
    {
        return new IntervalYear(aYearPrecision, aYear);
    }
    
    public static GoldilocksInterval createIntervalMonth(int aMonthPrecision, boolean aSign, int aMonth) throws SQLException
    {
        return new IntervalMonth(aMonthPrecision, aSign, aMonth);
    }
    
    public static GoldilocksInterval createIntervalMonth(int aMonthPrecision, String aMonth) throws SQLException
    {
        return new IntervalMonth(aMonthPrecision, aMonth);
    }
    
    public static GoldilocksInterval createIntervalDayToSecond(int aDayPrecision, int aFractionalPrecision, boolean aSign, int aDay, int aHour, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        return new IntervalDayToSecond(aDayPrecision, aFractionalPrecision, aSign, aDay, aHour, aMinute, aSecond, aMicroSec);
    }
    
    public static GoldilocksInterval createIntervalDayToSecond(int aDayPrecision, int aFractionalPrecision, String aDayToSecond) throws SQLException
    {
        return new IntervalDayToSecond(aDayPrecision, aFractionalPrecision, aDayToSecond);
    }
    
    public static GoldilocksInterval createIntervalDayToMinute(int aDayPrecision, boolean aSign, int aDay, int aHour, int aMinute) throws SQLException
    {
        return new IntervalDayToMinute(aDayPrecision, aSign, aDay, aHour, aMinute);
    }
    
    public static GoldilocksInterval createIntervalDayToMinute(int aDayPrecision, String aDayToMinute) throws SQLException
    {
        return new IntervalDayToMinute(aDayPrecision, aDayToMinute);
    }
    
    public static GoldilocksInterval createIntervalDayToHour(int aDayPrecision, boolean aSign, int aDay, int aHour) throws SQLException
    {
        return new IntervalDayToHour(aDayPrecision, aSign, aDay, aHour);
    }
    
    public static GoldilocksInterval createIntervalDayToHour(int aDayPrecision, String aDayToHour) throws SQLException
    {
        return new IntervalDayToHour(aDayPrecision, aDayToHour);
    }
    
    public static GoldilocksInterval createIntervalHourToSecond(int aHourPrecision, int aFractionalPrecision, boolean aSign, int aHour, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        return new IntervalHourToSecond(aHourPrecision, aFractionalPrecision, aSign, aHour, aMinute, aSecond, aMicroSec);
    }
    
    public static GoldilocksInterval createIntervalHourToSecond(int aHourPrecision, int aFractionalPrecision, boolean aSign, int aDay, int aHour, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        return new IntervalHourToSecond(aHourPrecision, aFractionalPrecision, aSign, aDay, aHour, aMinute, aSecond, aMicroSec);
    }
    
    public static GoldilocksInterval createIntervalHourToSecond(int aHourPrecision, int aFractionalPrecision, String aHourToSecond) throws SQLException
    {
        return new IntervalHourToSecond(aHourPrecision, aFractionalPrecision, aHourToSecond);
    }
    
    public static GoldilocksInterval createIntervalHourToMinute(int aHourPrecision, boolean aSign, int aHour, int aMinute) throws SQLException
    {
        return new IntervalHourToMinute(aHourPrecision, aSign, aHour, aMinute);
    }
    
    public static GoldilocksInterval createIntervalHourToMinute(int aHourPrecision, boolean aSign, int aDay, int aHour, int aMinute) throws SQLException
    {
        return new IntervalHourToMinute(aHourPrecision, aSign, aDay, aHour, aMinute);
    }
    
    public static GoldilocksInterval createIntervalHourToMinute(int aHourPrecision, String aHourToMinute) throws SQLException
    {
        return new IntervalHourToMinute(aHourPrecision, aHourToMinute);
    }
    
    public static GoldilocksInterval createIntervalMinuteToSecond(int aHourPrecision, int aFractionalPrecision, boolean aSign, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        return new IntervalMinuteToSecond(aHourPrecision, aFractionalPrecision, aSign, aMinute, aSecond, aMicroSec);
    }
    
    public static GoldilocksInterval createIntervalMinuteToSecond(int aHourPrecision, int aFractionalPrecision, boolean aSign, int aDay, int aHour, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        return new IntervalMinuteToSecond(aHourPrecision, aFractionalPrecision, aSign, aDay, aHour, aMinute, aSecond, aMicroSec);
    }
    
    public static GoldilocksInterval createIntervalMinuteToSecond(int aHourPrecision, int aFractionalPrecision, String aMinuteToSecond) throws SQLException
    {
        return new IntervalMinuteToSecond(aHourPrecision, aFractionalPrecision, aMinuteToSecond);
    }
    
    public static GoldilocksInterval createIntervalDay(int aDayPrecision, boolean aSign, int aDay) throws SQLException
    {
        return new IntervalDay(aDayPrecision, aSign, aDay);
    }
    
    public static GoldilocksInterval createIntervalDay(int aDayPrecision, String aDay) throws SQLException
    {
        return new IntervalDay(aDayPrecision, aDay);
    }
    
    public static GoldilocksInterval createIntervalHour(int aHourPrecision, boolean aSign, int aHour) throws SQLException
    {
        return new IntervalHour(aHourPrecision, aSign, aHour);
    }
    
    public static GoldilocksInterval createIntervalHour(int aHourPrecision, boolean aSign, int aDay, int aHour) throws SQLException
    {
        return new IntervalHour(aHourPrecision, aSign, aDay, aHour);
    }
    
    public static GoldilocksInterval createIntervalHour(int aHourPrecision, String aHour) throws SQLException
    {
        return new IntervalHour(aHourPrecision, aHour);
    }
    
    public static GoldilocksInterval createIntervalMinute(int aMinutePrecision, boolean aSign, int aMinute) throws SQLException
    {
        return new IntervalMinute(aMinutePrecision, aSign, aMinute);
    }
    
    public static GoldilocksInterval createIntervalMinute(int aMinutePrecision, boolean aSign, int aDay, int aHour, int aMinute) throws SQLException
    {
        return new IntervalMinute(aMinutePrecision, aSign, aDay, aHour, aMinute);
    }
    
    public static GoldilocksInterval createIntervalMinute(int aMinutePrecision, String aDay) throws SQLException
    {
        return new IntervalMinute(aMinutePrecision, aDay);
    }
    
    public static GoldilocksInterval createIntervalSecond(int aSecondPrecision, int aFractionalPrecision, boolean aSign, int aSecond, int aMicroSec) throws SQLException
    {
        return new IntervalSecond(aSecondPrecision, aFractionalPrecision, aSign, aSecond, aMicroSec);
    }
    
    public static GoldilocksInterval createIntervalSecond(int aSecondPrecision, int aFractionalPrecision, boolean aSign, double aSecond) throws SQLException
    {
        return new IntervalSecond(aSecondPrecision, aFractionalPrecision, aSign, aSecond);
    }
    
    public static GoldilocksInterval createIntervalSecond(int aSecondPrecision, int aFractionalPrecision, boolean aSign, int aDay, int aHour, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        return new IntervalSecond(aSecondPrecision, aFractionalPrecision, aSign, aDay, aHour, aMinute, aSecond, aMicroSec);
    }
    
    public static GoldilocksInterval createIntervalSecond(int aSecondPrecision, int aFractionalPrecision, String aSecond) throws SQLException
    {
        return new IntervalSecond(aSecondPrecision, aFractionalPrecision, aSecond);
    }
    
    protected static boolean isOutOfPrecision(int aPrecision, long aValue)
    {
        if (aValue == 0)
        {
            return false;
        }
        if (aValue < 0)
        {
            aValue = -aValue;
        }
        return VersionSpecific.SINGLETON.log10ToInt(aValue) + 1 > aPrecision;
    }

    protected static boolean isOutOfFractionalPrecision(int aFractionalPrecision, int aValue)
    {
        if (aValue == 0)
        {
            return false;
        }
        if (aValue < 0)
        {
            aValue = -aValue;
        }
        String sValue = String.valueOf(aValue);
        int sFollowingZero = 0;
        for (int i=sValue.length() - 1; i>=0; i--)
        {
            if (sValue.charAt(i) == '0')
            {
                sFollowingZero++;
            }
            else
            {
                break;
            }
        }
        return aFractionalPrecision < (6 - sFollowingZero);
    }
    
    protected static String[] split(String aString, char aDelimiter)
    {
        int sElemCount = 1;
        
        for (int i=0; i<aString.length(); i++)
        {
            if (aString.charAt(i) == aDelimiter)
            {
                sElemCount++;
            }
        }
        
        String[] sElems = new String[sElemCount];
        sElemCount = 0;
        int sBefore = 0;
        int sIndex;
        for (sIndex=0; sIndex<aString.length(); sIndex++)
        {
            if (aString.charAt(sIndex) == aDelimiter)
            {
                sElems[sElemCount++] = aString.substring(sBefore, sIndex);
                sBefore = sIndex + 1;
            }
        }
        sElems[sElemCount] = aString.substring(sBefore, sIndex);
        
        return sElems;
    }
    
    private static String[] LEADING_ZEROS = { "", "0", "00", "000", "0000", "00000", "000000" };
    
    protected static String format(int aPrecision, int aValue)
    {
        /*
         * 이 메소드는 JDK1.4에서 String.format()을 지원하지 않아 구현한 것이다.
         * aPrecision은 6보다 크지 않다고 가정하고 작성하였다.
         */
        String sResult = String.valueOf(aValue);
        if (sResult.length() < aPrecision)
        {
            sResult = LEADING_ZEROS[aPrecision - sResult.length()] + sResult;
        }
        return sResult;
    }
    
    protected int mLeadingPrecision;
    protected int mFractionalPrecision;
    protected boolean mSign;
    protected int mYear;
    protected int mMonth;
    protected int mDay;
    protected int mHour;
    protected int mMinute;
    protected int mSecond;
    protected int mMicroSec;
    
    protected GoldilocksInterval(int aLeadingPrecision, int aFractionalPrecision, boolean aSign, int aYear, int aMonth, int aDay, int aHour, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        mLeadingPrecision = aLeadingPrecision;
        mFractionalPrecision = aFractionalPrecision;
        mSign = aSign;
        mYear = aYear;
        mMonth = aMonth;
        mDay = aDay;
        mHour = aHour;
        mMinute = aMinute;
        mSecond = aSecond;
        mMicroSec = aMicroSec;
        
        validate();
        alignSign();
    }
    
    protected GoldilocksInterval(int aLeadingPrecision, int aFractionalPrecision, String aValueString) throws SQLException
    {
        mLeadingPrecision = aLeadingPrecision;
        mFractionalPrecision = aFractionalPrecision;
    
        aValueString = parseSign(aValueString);
        parseTypedInterval(aValueString);

        validate();
        alignSign();
    }
    
    public final int getSign()
    {
        return mSign ? 1 : -1;
    }
    
    public final int getYear()
    {
        return mYear;
    }
    
    public final int getMonth()
    {
        return mMonth;
    }
    
    public final int getAccumulatedMonth()
    {
        return mYear * 12 + mMonth;
    }
    
    public final int getDay()
    {
        return mDay;
    }
    
    public final int getHour()
    {
        return mHour;
    }
    
    public final int getAccumulatedHour()
    {
        return mDay * 24 + mHour;
    }
    
    public final int getMinute()
    {
        return mMinute;
    }
    
    public final long getAccumulatedMinute()
    {
        return getAccumulatedHour() * 60l + mMinute;
    }
    
    public final int getSecond()
    {
        return mSecond;
    }
    
    public final long getAccumulatedSecond()
    {
        return getAccumulatedMinute() * 60l + mSecond;
    }
    
    public final int getMicroSecond()
    {
        return mMicroSec;
    }
    
    public final long getAccumulatedMicroSecond()
    {
        return getAccumulatedSecond() * 1000000l + mMicroSec;
    }
    
    public final String getTypeName()
    {
        String sResult = INTERVAL_TYPE_NAME[0];
        switch (getSqlType())
        {
            case GoldilocksTypes.INTERVAL_YEAR:
                sResult = INTERVAL_TYPE_NAME[1];
                break;
            case GoldilocksTypes.INTERVAL_MONTH:
                sResult = INTERVAL_TYPE_NAME[2];
                break;
            case GoldilocksTypes.INTERVAL_DAY:
                sResult = INTERVAL_TYPE_NAME[3];
                break;
            case GoldilocksTypes.INTERVAL_HOUR:
                sResult = INTERVAL_TYPE_NAME[4];
                break;
            case GoldilocksTypes.INTERVAL_MINUTE:
                sResult = INTERVAL_TYPE_NAME[5];
                break;
            case GoldilocksTypes.INTERVAL_SECOND:
                sResult = INTERVAL_TYPE_NAME[6];
                break;
            case GoldilocksTypes.INTERVAL_YEAR_TO_MONTH:
                sResult = INTERVAL_TYPE_NAME[7];
                break;
            case GoldilocksTypes.INTERVAL_DAY_TO_HOUR:
                sResult = INTERVAL_TYPE_NAME[8];
                break;
            case GoldilocksTypes.INTERVAL_DAY_TO_MINUTE:
                sResult = INTERVAL_TYPE_NAME[9];
                break;
            case GoldilocksTypes.INTERVAL_DAY_TO_SECOND:
                sResult = INTERVAL_TYPE_NAME[10];
                break;
            case GoldilocksTypes.INTERVAL_HOUR_TO_MINUTE:
                sResult = INTERVAL_TYPE_NAME[11];
                break;
            case GoldilocksTypes.INTERVAL_HOUR_TO_SECOND:
                sResult = INTERVAL_TYPE_NAME[12];
                break;
            case GoldilocksTypes.INTERVAL_MINUTE_TO_SECOND:
                sResult = INTERVAL_TYPE_NAME[13];
                break;
        }
        return sResult;
    }
    
    protected final String signString()
    {
        return mSign ? "+" : "-";
    }
    
    protected final void validateLeadingPrecision() throws SQLException
    {
        if (mLeadingPrecision < 0 || mLeadingPrecision > 6)
        {
            raise(String.valueOf(mLeadingPrecision));
        }
    }
    
    protected final String parseSign(String aValueString) throws SQLException
    {
        if (aValueString == null || aValueString.length() == 0)
        {
            raise(aValueString);
        }
        
        int i;
        mSign = true;
        for (i=0; i<aValueString.length(); i++)
        {
            if (aValueString.charAt(i) == '-')
            {
                mSign = !mSign;
            }
            else if (aValueString.charAt(i) == '+')
            {
                // just skip
            }
            else
            {
                break;
            }
        }
        return aValueString.substring(i, aValueString.length());
    }

    protected final void raise(String aValue) throws SQLException
    {
        throw new SQLException("Invalid interval format or out of range for [" + getTypeName() + "]: " + aValue, "99999");
    }
    
    private void alignSign()
    {
        if (!mSign && mYear == 0 && mMonth == 0 && mDay == 0 && mHour == 0 && mMinute == 0 && mSecond == 0 && mMicroSec == 0)
        {
            mSign = true;
        }
    }
    
    private void validate() throws SQLException
    {
        validateYear();
        validateMonth();
        validateDay();
        validateHour();
        validateMinute();
        validateSecond();
        validateMicroSecond();
        validateLeadingPrecision();
        validateFractionalPrecision();
        validateByLeadingPrecision();
        validateByFractionalPrecision();
    }
    
    public abstract String toString();
    public abstract int getSqlType();
    protected abstract void parseTypedInterval(String aValueString) throws SQLException;
    protected abstract void validateYear() throws SQLException;
    protected abstract void validateMonth() throws SQLException;
    protected abstract void validateDay() throws SQLException;
    protected abstract void validateHour() throws SQLException;
    protected abstract void validateMinute() throws SQLException;
    protected abstract void validateSecond() throws SQLException;
    protected abstract void validateMicroSecond() throws SQLException;
    protected abstract void validateFractionalPrecision() throws SQLException;
    protected abstract void validateByLeadingPrecision() throws SQLException;
    protected abstract void validateByFractionalPrecision() throws SQLException;
}

class IntervalYearToMonth extends GoldilocksInterval
{
    protected IntervalYearToMonth(int aLeadingPrecision, boolean aSign, int aYear, int aMonth) throws SQLException
    {
        super(aLeadingPrecision, 0, aSign, aYear, aMonth, 0, 0, 0, 0, 0);
    }
    
    protected IntervalYearToMonth(int aLeadingPrecision, String aYearToMonth) throws SQLException
    {
        super(aLeadingPrecision, 0, aYearToMonth);
    }
    
    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_YEAR_TO_MONTH;
    }

    protected final void parseTypedInterval(String aValueString) throws SQLException
    {
        if (aValueString.endsWith("-"))
        {
            raise(aValueString);
        }
        
        String[] sElems = split(aValueString, '-');
        
        if (sElems.length > 2 || sElems.length == 0)
        {
            raise(aValueString);
        }
        else if (sElems.length == 1)
        {
            parse1Elem(sElems[0]);
        }
        else
        {
            parseYear(sElems[0]);
            parseMonth(sElems[1]);
        }
    }
    
    protected final void parseYear(String aElem) throws SQLException
    {
        try
        {
            mYear = Integer.parseInt(aElem);
        }
        catch (Exception sException)
        {
            raise(aElem);
        }
    }
    
    protected final void parseMonth(String aElem) throws SQLException
    {
        try
        {
            mMonth = Integer.parseInt(aElem);
        }
        catch (Exception sException)
        {
            raise(aElem);
        }
    }
    
    protected void parse1Elem(String aElem) throws SQLException
    {
        raise(aElem);
    }
    
    @Override
    protected final void validateYear() throws SQLException
    {
        if (mYear < 0)
        {
            raise(String.valueOf(mYear));
        }
    }

    @Override
    protected void validateMonth() throws SQLException
    {
        if (mMonth >= 12 || mMonth < 0)
        {
            raise(String.valueOf(mMonth));
        }
    }

    @Override
    protected final void validateDay() throws SQLException
    {
        if (mDay != 0)
        {
            raise(String.valueOf(mDay));
        }
    }

    @Override
    protected final void validateHour() throws SQLException
    {
        if (mHour != 0)
        {
            raise(String.valueOf(mHour));
        }
    }

    @Override
    protected final void validateMinute() throws SQLException
    {
        if (mMinute != 0)
        {
            raise(String.valueOf(mMinute));
        }
    }

    @Override
    protected final void validateSecond() throws SQLException
    {
        if (mSecond != 0)
        {
            raise(String.valueOf(mSecond));
        }
    }

    @Override
    protected final void validateMicroSecond() throws SQLException
    {
        if (mMicroSec != 0)
        {
            raise(String.valueOf(mMicroSec));
        }        
    }
    
    @Override
    protected final void validateFractionalPrecision() throws SQLException
    {
        if (mFractionalPrecision != 0)
        {
            raise(String.valueOf(mFractionalPrecision));
        }
    }
    
    @Override
    protected void validateByLeadingPrecision() throws SQLException
    {
        if (isOutOfPrecision(mLeadingPrecision, mYear))
        {
            raise(String.valueOf(mYear));
        }
    }

    @Override
    protected final void validateByFractionalPrecision() throws SQLException
    {
        /*
         * nothing to do
         */
    }

    @Override
    public String toString()
    {
        return signString() + format(mLeadingPrecision, mYear) + "-" + format(2, mMonth);
    }
}

class IntervalYear extends IntervalYearToMonth
{
    protected IntervalYear(int aLeadingPrecision, boolean aSign, int aYear) throws SQLException
    {
        super(aLeadingPrecision, aSign, aYear, 0);
    }
    
    protected IntervalYear(int aLeadingPrecision, String aYear) throws SQLException
    {
        super(aLeadingPrecision, aYear);
    }
    
    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_YEAR;
    }

    @Override
    protected void parse1Elem(String aElem) throws SQLException
    {
        parseYear(aElem);
    }
    
    @Override
    protected void validateMonth() throws SQLException
    {
        if (mMonth != 0)
        {
            raise(String.valueOf(mMonth));
        }
    }
}

class IntervalMonth extends IntervalYearToMonth
{
    protected IntervalMonth(int aLeadingPrecision, boolean aSign, int aMonth) throws SQLException
    {
        super(aLeadingPrecision, aSign, aMonth / 12, aMonth % 12);
    }
    
    protected IntervalMonth(int aLeadingPrecision, String aMonth) throws SQLException
    {
        super(aLeadingPrecision, aMonth);
    }

    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_MONTH;
    }

    @Override
    protected void parse1Elem(String aElem) throws SQLException
    {
        parseMonth(aElem);
        normalizeMonth();
    }

    private void normalizeMonth()
    {
        if (mMonth >= 12)
        {
            mYear += mMonth / 12;
            mMonth = mMonth % 12;
        }
    }

    @Override
    protected void validateByLeadingPrecision() throws SQLException
    {
        if (isOutOfPrecision(mLeadingPrecision, getAccumulatedMonth()))
        {
            raise(String.valueOf(getAccumulatedMonth()));
        }
    }
}

class IntervalDayToSecond extends GoldilocksInterval
{
    protected IntervalDayToSecond(int aLeadingPrecision, int aFractionalPrecision, boolean aSign, int aDay, int aHour, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        super(aLeadingPrecision, aFractionalPrecision, aSign, 0, 0, aDay, aHour, aMinute, aSecond, aMicroSec);
    }
    
    protected IntervalDayToSecond(int aLeadingPrecision, int aFractionalPrecision, String aDayToSecond) throws SQLException
    {
        super(aLeadingPrecision, aFractionalPrecision, aDayToSecond);
    }

    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_DAY_TO_SECOND;
    }

    protected int getSecondPrecision()
    {
        /*
         * 기본적으로 second의 precision은 2이다.
         * IntervalSecond만 leading precision을 second precision으로 사용한다.
         */
        return 2;
    }
    
    protected final void parseTypedInterval(String aValueString) throws SQLException
    {
        String[] sElems = split(aValueString, ' ');
        if (sElems.length > 2 || sElems.length == 0)
        {
            raise(aValueString);
        }
        else if (sElems.length == 2)
        {
            parseDay(sElems[0]);
            parseTime(sElems[1], true);
        }
        else
        {
            parseTime(sElems[0], false);
        }
    }
    
    protected final void parseDay(String aDay) throws SQLException
    {
        try
        {
            mDay = Integer.parseInt(aDay);
        }
        catch (NumberFormatException sException)
        {
            raise(aDay);
        }
    }
    
    protected final void parseTime(String aTime, boolean aFollowedByDay) throws SQLException
    {
        String[] sTimeElems = split(aTime, ':');
        if (sTimeElems.length == 3)
        {
            parseHourSecond(sTimeElems);
            if (!aFollowedByDay)
            {
                /*
                 * day 없이 time만 온 경우 hour는 24보다 클 수 있다.
                 */
                normalizeHour();
            }
        }
        else if (sTimeElems.length == 2)
        {
            if (aFollowedByDay)
            {
                parseHourMinute(sTimeElems);
            }
            else
            {
                parse2TimeElem(sTimeElems);
            }
        }
        else if (sTimeElems.length == 1)
        {
            if (aFollowedByDay)
            {
                parseHour(sTimeElems[0]);
            }
            else
            {
                parse1TimeElem(sTimeElems[0]);
            }
        }
        else
        {
            raise(aTime);
        }
    }
    
    protected final void parseHourSecond(String[] aTimeElems) throws SQLException
    {
        parseHourMinute(aTimeElems);
        parseSecond(aTimeElems[2]);
    }
    
    protected final void parseHourMinute(String[] aTimeElems) throws SQLException
    {
        parseHour(aTimeElems[0]);
        parseMinute(aTimeElems[1]);
    }
    
    protected final void parseHour(String aTimeElem) throws SQLException
    {
        try
        {
            mHour = Integer.parseInt(aTimeElem);
        }
        catch (NumberFormatException sException)
        {
            raise(aTimeElem);
        }
    }
    
    protected final void parseMinute(String aTimeElem) throws SQLException
    {
        try
        {
            mMinute = Integer.parseInt(aTimeElem);
        }
        catch (NumberFormatException sException)
        {
            raise(aTimeElem);
        }
    }
    
    protected final void parseSecond(String aSec) throws SQLException
    {
        try
        {
            String[] sSecElems = split(aSec, '.');
            if (sSecElems.length > 2)
            {
                raise(aSec);
            }
            mSecond = Integer.parseInt(sSecElems[0]);

            if (sSecElems.length == 2)
            {
                if (sSecElems[1].length() > 6)
                {
                    for (int i=6; i<sSecElems[1].length(); i++)
                    {
                        if (sSecElems[1].charAt(i) != '0')
                        {
                            raise(aSec);
                        }
                    }
                    sSecElems[1] = sSecElems[1].substring(0, 6);
                }
                else if (sSecElems[1].length() < 6)
                {
                    sSecElems[1] = (sSecElems[1] + "000000").substring(0, 6);
                }
                mMicroSec = Integer.parseInt(sSecElems[1]);
            }
        }
        catch (Exception sException)
        {
            raise(aSec);
        }
    }

    protected void parse2TimeElem(String[] aTimeElems) throws SQLException
    {
        /*
         * 이 메소드는 override 가능하다.
         * MinuteToSecond는 이 메소드를 override해야 한다.
         * 일반적으로 "12:34"는 '시:분'을 의미하지만,
         * MinuteToSecond는 '분:초'를 나타낸다.
         */
        parseHourMinute(aTimeElems);
        normalizeHour();
    }

    protected void parse1TimeElem(String aTimeElem) throws SQLException
    {
        /*
         * Interval day, hour, minute, second 등 한개짜리 타입이 아닌 경우 모두 에러다.
         */
        raise(aTimeElem);
    }
    
    protected void normalizeHour()
    {
        if (mHour >= 24)
        {
            mDay += mHour / 24;
            mHour = mHour % 24;
        }
    }

    protected void normalizeMinute()
    {
        if (mMinute >= 60)
        {
            mHour += mMinute / 60;
            mMinute = mMinute % 60;
        }
        normalizeHour();
    }
    
    protected void normalizeSecond()
    {
        if (mSecond >= 60)
        {
            mMinute += mSecond / 60;
            mSecond = mSecond % 60;
        }
        normalizeMinute();
    }

    @Override
    protected final void validateYear() throws SQLException
    {
        if (mYear != 0)
        {
            raise(String.valueOf(mYear));
        }
    }

    @Override
    protected final void validateMonth() throws SQLException
    {
        if (mMonth != 0)
        {
            raise(String.valueOf(mMonth));
        }
    }

    @Override
    protected final void validateDay() throws SQLException
    {
        if (mDay < 0)
        {
            raise(String.valueOf(mDay));
        }
    }

    @Override
    protected void validateHour() throws SQLException
    {
        if (mHour >= 24 || mHour < 0)
        {
            raise(String.valueOf(mHour));
        }
    }

    @Override
    protected void validateMinute() throws SQLException
    {
        if (mMinute >= 60 || mMinute < 0)
        {
            raise(String.valueOf(mMinute));
        }
    }

    @Override
    protected void validateSecond() throws SQLException
    {
        if (mSecond >= 60 || mSecond < 0)
        {
            raise(String.valueOf(mSecond));
        }
    }

    @Override
    protected void validateMicroSecond() throws SQLException
    {
        if (mMicroSec >= 1000000 || mMicroSec < 0)
        {
            raise(String.valueOf(mMicroSec));
        }        
    }
    
    @Override
    protected void validateFractionalPrecision() throws SQLException
    {
        if (mFractionalPrecision > 6 || mFractionalPrecision < 0)
        {
            raise(String.valueOf(mFractionalPrecision));
        }
    }

    @Override
    protected void validateByLeadingPrecision() throws SQLException
    {
        if (isOutOfPrecision(mLeadingPrecision, mDay))
        {
            raise(String.valueOf(mDay));
        }
    }

    @Override
    protected void validateByFractionalPrecision() throws SQLException
    {
        if (isOutOfFractionalPrecision(mFractionalPrecision, mMicroSec))
        {
            raise(String.valueOf(mMicroSec));
        }
    }

    private String getFractionString()
    {
        String sResult = format(6, mMicroSec);
        return sResult.substring(0, mFractionalPrecision);
    }
    
    @Override
    public final String toString()
    {
        StringBuffer sBuf = new StringBuffer(30);
        sBuf.append(signString())
            .append(format(mLeadingPrecision, mDay))
            .append(' ')
            .append(format(2, mHour))
            .append(':')
            .append(format(2, mMinute))
            .append(':')
            .append(format(2, mSecond));
        if (mFractionalPrecision > 0)
        {
            sBuf.append('.').append(getFractionString());
        }
        return sBuf.toString();
    }
}

class IntervalDayToMinute extends IntervalDayToSecond
{
    protected IntervalDayToMinute(int aLeadingPrecision, boolean aSign, int aDay, int aHour, int aMinute) throws SQLException
    {
        super(aLeadingPrecision, 0, aSign, aDay, aHour, aMinute, 0, 0);
    }
    
    protected IntervalDayToMinute(int aLeadingPrecision, String aDayToMinute) throws SQLException
    {
        super(aLeadingPrecision, 0, aDayToMinute);
    }
    
    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_DAY_TO_MINUTE;
    }
    
    @Override
    protected void validateSecond() throws SQLException
    {
        if (mSecond != 0)
        {
            raise(String.valueOf(mSecond));
        }
    }
    
    @Override
    protected void validateMicroSecond() throws SQLException
    {
        if (mMicroSec != 0)
        {
            raise(String.valueOf(mMicroSec));
        }
    }
    
    @Override
    protected void validateFractionalPrecision() throws SQLException
    {
        if (mFractionalPrecision != 0)
        {
            raise(String.valueOf(mFractionalPrecision));
        }
    }
}

class IntervalDayToHour extends IntervalDayToMinute
{
    protected IntervalDayToHour(int aLeadingPrecision, boolean aSign, int aDay, int aHour) throws SQLException
    {
        super(aLeadingPrecision, aSign, aDay, aHour, 0);
    }
    
    protected IntervalDayToHour(int aLeadingPrecision, String aDayToHour) throws SQLException
    {
        super(aLeadingPrecision, aDayToHour);
    }
    
    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_DAY_TO_HOUR;
    }
    
    @Override
    protected void validateMinute() throws SQLException
    {
        if (mMinute != 0)
        {
            raise(String.valueOf(mMinute));
        }
    }
}

class IntervalDay extends IntervalDayToHour
{
    protected IntervalDay(int aLeadingPrecision, boolean aSign, int aDay) throws SQLException
    {
        super(aLeadingPrecision, aSign, aDay, 0);
    }
    
    protected IntervalDay(int aLeadingPrecision, String aDay) throws SQLException
    {
        super(aLeadingPrecision, aDay);
    }
    
    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_DAY;
    }
    
    @Override
    protected void parse1TimeElem(String aTimeElem) throws SQLException
    {
        parseDay(aTimeElem);
    }
    
    @Override
    protected void validateHour() throws SQLException
    {
        if (mHour != 0)
        {
            raise(String.valueOf(mHour));
        }
    }
}

class IntervalHourToSecond extends IntervalDayToSecond
{
    protected IntervalHourToSecond(int aLeadingPrecision, int aFractionalPrecision, boolean aSign, int aHour, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        super(aLeadingPrecision, aFractionalPrecision, aSign, aHour / 24, aHour % 24, aMinute, aSecond, aMicroSec);
    }
    
    protected IntervalHourToSecond(int aLeadingPrecision, int aFractionalPrecision, boolean aSign, int aDay, int aHour, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        super(aLeadingPrecision, aFractionalPrecision, aSign, aDay, aHour, aMinute, aSecond, aMicroSec);
    }
    
    protected IntervalHourToSecond(int aLeadingPrecision, int aFractionalPrecision, String aHourToSecond) throws SQLException
    {
        super(aLeadingPrecision, aFractionalPrecision, aHourToSecond);
    }
    
    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_HOUR_TO_SECOND;
    }
    
    @Override
    protected void validateByLeadingPrecision() throws SQLException
    {
        if (isOutOfPrecision(mLeadingPrecision, getAccumulatedHour()))
        {
            raise(String.valueOf(getAccumulatedHour()));
        }
    }
}

class IntervalHourToMinute extends IntervalHourToSecond
{
    protected IntervalHourToMinute(int aLeadingPrecision, boolean aSign, int aHour, int aMinute) throws SQLException
    {
        super(aLeadingPrecision, 0, aSign, aHour, aMinute, 0, 0);
    }
    
    protected IntervalHourToMinute(int aLeadingPrecision, boolean aSign, int aDay, int aHour, int aMinute) throws SQLException
    {
        super(aLeadingPrecision, 0, aSign, aDay, aHour, aMinute, 0, 0);
    }
    
    protected IntervalHourToMinute(int aLeadingPrecision, String aHourToMinute) throws SQLException
    {
        super(aLeadingPrecision, 0, aHourToMinute);
    }
    
    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_HOUR_TO_MINUTE;
    }
    
    @Override
    protected void validateSecond() throws SQLException
    {
        if (mSecond != 0)
        {
            raise(String.valueOf(mSecond));
        }
    }
    
    @Override
    protected void validateMicroSecond() throws SQLException
    {
        if (mMicroSec != 0)
        {
            raise(String.valueOf(mMicroSec));
        }
    }
    
    @Override
    protected void validateFractionalPrecision() throws SQLException
    {
        if (mFractionalPrecision != 0)
        {
            raise(String.valueOf(mFractionalPrecision));
        }
    }
}

class IntervalHour extends IntervalHourToMinute
{
    protected IntervalHour(int aLeadingPrecision, boolean aSign, int aHour) throws SQLException
    {
        super(aLeadingPrecision, aSign, aHour, 0);
    }
    
    protected IntervalHour(int aLeadingPrecision, boolean aSign, int aDay, int aHour) throws SQLException
    {
        super(aLeadingPrecision, aSign, aDay, aHour, 0);
    }
    
    protected IntervalHour(int aLeadingPrecision, String aHour) throws SQLException
    {
        super(aLeadingPrecision, aHour);
    }
    
    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_HOUR;
    }
    
    @Override
    protected void parse1TimeElem(String aTimeElem) throws SQLException
    {
        parseHour(aTimeElem);
        normalizeHour();
    }
    
    @Override
    protected void validateMinute() throws SQLException
    {
        if (mMinute != 0)
        {
            raise(String.valueOf(mMinute));
        }
    }
}

class IntervalMinute extends IntervalHourToMinute
{
    protected IntervalMinute(int aLeadingPrecision, boolean aSign, int aMinute) throws SQLException
    {
        super(aLeadingPrecision, aSign, aMinute / 60, aMinute % 60);
    }
    
    protected IntervalMinute(int aLeadingPrecision, boolean aSign, int aDay, int aHour, int aMinute) throws SQLException
    {
        super(aLeadingPrecision, aSign, aDay, aHour, aMinute);
    }
    
    protected IntervalMinute(int aLeadingPrecision, String aMinute) throws SQLException
    {
        super(aLeadingPrecision, aMinute);
    }
    
    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_MINUTE;
    }
    
    @Override
    protected void parse1TimeElem(String aTimeElem) throws SQLException
    {
        parseMinute(aTimeElem);
        normalizeMinute();
    }

    @Override
    protected void validateByLeadingPrecision() throws SQLException
    {
        if (isOutOfPrecision(mLeadingPrecision, getAccumulatedMinute()))
        {
            raise(String.valueOf(getAccumulatedMinute()));
        }
    }
}

class IntervalMinuteToSecond extends IntervalHourToSecond
{
    protected IntervalMinuteToSecond(int aLeadingPrecision, int aFractionalPrecision, boolean aSign, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        super(aLeadingPrecision, aFractionalPrecision, aSign, aMinute / 60, aMinute % 60, aSecond, aMicroSec);
    }
    
    protected IntervalMinuteToSecond(int aLeadingPrecision, int aFractionalPrecision, boolean aSign, int aDay, int aHour, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        super(aLeadingPrecision, aFractionalPrecision, aSign, aDay, aHour, aMinute, aSecond, aMicroSec);
    }
    
    protected IntervalMinuteToSecond(int aLeadingPrecision, int aFractionalPrecision, String aMinuteToSecond) throws SQLException
    {
        super(aLeadingPrecision, aFractionalPrecision, aMinuteToSecond);
    }
    
    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_MINUTE_TO_SECOND;
    }
    
    @Override
    protected void parse2TimeElem(String[] aTimeElems) throws SQLException
    {
        parseMinute(aTimeElems[0]);
        parseSecond(aTimeElems[1]);
        normalizeMinute();
    }
    
    @Override
    protected void validateByLeadingPrecision() throws SQLException
    {
        if (isOutOfPrecision(mLeadingPrecision, getAccumulatedMinute()))
        {
            raise(String.valueOf(getAccumulatedMinute()));
        }
    }
}

class IntervalSecond extends IntervalMinuteToSecond
{
    protected IntervalSecond(int aLeadingPrecision, int aFractionalPrecision, boolean aSign, int aSecond, int aMicroSec) throws SQLException
    {
        super(aLeadingPrecision, aFractionalPrecision, aSign, aSecond / 60, aSecond % 60, aMicroSec);
    }
    
    protected IntervalSecond(int aLeadingPrecision, int aFractionalPrecision, boolean aSign, double aSecond) throws SQLException
    {
        this(aLeadingPrecision, aFractionalPrecision, aSign, (int)aSecond, (int)((aSecond - (int)aSecond) * 1000000));
    }
    
    protected IntervalSecond(int aLeadingPrecision, int aFractionalPrecision, boolean aSign, int aDay, int aHour, int aMinute, int aSecond, int aMicroSec) throws SQLException
    {
        super(aLeadingPrecision, aFractionalPrecision, aSign, aDay, aHour, aMinute, aSecond, aMicroSec);
    }
    
    protected IntervalSecond(int aLeadingPrecision, int aFractionalPrecision, String aSecond) throws SQLException
    {
        super(aLeadingPrecision, aFractionalPrecision, aSecond);
    }
    
    @Override
    public int getSqlType()
    {
        return GoldilocksTypes.INTERVAL_SECOND;
    }
    
    protected int getSecondPrecision()
    {
        return mLeadingPrecision;
    }
    
    @Override
    protected void parse2TimeElem(String[] aTimeElems) throws SQLException
    {
        /*
         * IntervalSecond는 '12:23'의 경우 '시:분'으로 해석해야 한다.
         * 상위 클래스인 IntervalMinuteToSecond만이 '분:초'로 해석하기 때문에
         * 이 메소드를 다시 override해서 원래대로 '시:분'으로 해석해야 한다.
         */
        parseHourMinute(aTimeElems);
        normalizeHour();
    }
    
    @Override
    protected void parse1TimeElem(String aTimeElem) throws SQLException
    {
        parseSecond(aTimeElem);
        normalizeSecond();
    }
    
    @Override
    protected void validateByLeadingPrecision() throws SQLException
    {
        if (isOutOfPrecision(mLeadingPrecision, getAccumulatedSecond()))
        {
            raise(String.valueOf(getAccumulatedSecond()));
        }
    }
}
