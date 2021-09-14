package sunje.goldilocks.jdbc.env;

import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;
import java.text.SimpleDateFormat;

public interface SessionEnv
{
    int getMaxTrailingZeroCount();
    int getMaxLeadingZeroCount();
    CharsetEncoder getNewEncoder();
    CharsetDecoder getNewDecoder();
    String getDecodingReplacement();
    SimpleDateFormat getDateFormat();
    SimpleDateFormat getTimeFormat();
    SimpleDateFormat getTimestampFormat();
    SimpleDateFormat getTimeTZFormat();
    SimpleDateFormat getTimestampTZFormat();
    //TimeZone getSessionTimeZone(); /* 현재 JDBC에서 session timezone을 사용할 일이 없다. */
}
