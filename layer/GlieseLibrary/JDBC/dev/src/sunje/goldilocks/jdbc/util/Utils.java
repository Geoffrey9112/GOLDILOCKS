package sunje.goldilocks.jdbc.util;

import java.math.BigDecimal;
import java.util.Calendar;
import java.util.TimeZone;

public class Utils
{
    private static final long LOCAL_TIMEZONE_OFFSET = Calendar.getInstance().getTimeZone().getRawOffset();
    
    public static Calendar getLocalCalendar()
    {
        return Calendar.getInstance();
    }

    public static Calendar getCalendar(int aTZOffsetSec)
    {
        /*
         * offset으로 timezone을 얻으면 위치가 부정확할 수 있다.
         * 가령 seoul과 같은 시간대를 사용하는 다른 위치의 timezone을 얻을 수 있으므로
         * 먼저 기본 timezone의 offset과 비교한다.
         * 
         * 이 코드를 넣은 이유는 TimeZone.getAvailableIDs()를 호출하는 순간,
         * 이후부터 String 생성 성능이 현저히 떨어지기 때문이다.
         * 이유는 알 수 없지만(자바 내부 문제), 어쨌든 대부분 timezone을 로컬로 사용하고 있다고
         * 가정하고 최대한 피해갈 수 있기 위해 이 코드를 넣었다.
         * 해당 성능 테스트는 <PM-010-01 Fetch size에 따른 Fetch 성능>의 두번째 varchar fetch 성능 참조할 것.
         */
        if (aTZOffsetSec * 1000 == LOCAL_TIMEZONE_OFFSET)
        {
            return getLocalCalendar();
        }
        
        aTZOffsetSec = aTZOffsetSec / 1800 * 1800; /* 30분 단위로 절삭한다. */
        String[] sTimeZoneIds = TimeZone.getAvailableIDs(aTZOffsetSec * 1000);
        if (sTimeZoneIds == null || sTimeZoneIds.length == 0)
        {
            return getLocalCalendar();
        }
        else
        {
            return Calendar.getInstance(TimeZone.getTimeZone(sTimeZoneIds[0]));
        }
    }
    
    public static BigDecimal makeBigDecimal(double x)
    {
        /*
         * new BigDecimal(6.05) 하면 6.04999999... 이런식으로 나오는 문제가 있음
         */
        if (x == (long)x)
        {
            /*
             * 정수로 떨어질 땐, String.valueOf()를 사용하면 .0이 붙어 나오는 문제가 있다.
             */
            return new BigDecimal(x);
        }
        else
        {
            return new BigDecimal(String.valueOf(x));
        }
    }
    
    public static BigDecimal makeBigDecimal(String x)
    {
        return new BigDecimal(x);
    }
}
