package sunje.goldilocks.jdbc.util;


public abstract class Logger
{
    public static Logger createLogger(LogWriterHost aHost, boolean aTraceLog, boolean aQueryLog, boolean aProtocolLog)
    {
        if (!aTraceLog && !aQueryLog && !aProtocolLog)
        {
            return new NoLogger();
        }
        else
        {
            return new LoggerImpl(aHost, aTraceLog, aQueryLog, aProtocolLog);
        }
    }
    public abstract boolean isLogOn();    
    public abstract void logTrace();
    public abstract void logQuery(String aSql);
    public abstract void logProtocol(String aOp, String aContent);
}
