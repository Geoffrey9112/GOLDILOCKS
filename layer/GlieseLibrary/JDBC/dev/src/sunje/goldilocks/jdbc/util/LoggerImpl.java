package sunje.goldilocks.jdbc.util;

import java.io.PrintWriter;
import java.util.Date;

public class LoggerImpl extends Logger
{
    private LogWriterHost mHost;
    private boolean mTraceLog;
    private boolean mQueryLog;
    private boolean mProtocolLog;
    
    LoggerImpl(LogWriterHost aHost, boolean aTraceLog, boolean aQueryLog, boolean aProtocolLog)
    {
        mHost = aHost;
        mTraceLog = aTraceLog;
        mQueryLog = aQueryLog;
        mProtocolLog = aProtocolLog;
    }
    
    private String currentTime()
    {
        return new Date(System.currentTimeMillis()).toString();
    }
    
    public boolean isLogOn()
    {
        return true;
    }
    
    public void logTrace()
    {
        if (mTraceLog)
        {
            PrintWriter sWriter = mHost.getLogWriter();
            
            if (sWriter != null)
            {
                StackTraceElement[] sStacks = Thread.currentThread().getStackTrace();
                StackTraceElement sCaller = sStacks[2];
                if (!sStacks[3].getClassName().startsWith("sunje.goldilocks")) // client가 직접 api를 호출한 경우만 출력한다.
                {
                    synchronized (sWriter)
                    {
                        sWriter.println("[TRACE LOG] [" + currentTime() + "] [" + Thread.currentThread().getId() +"] " + sCaller.getClassName() + "." + sCaller.getMethodName());
                        sWriter.flush();
                    }
                }
            }
        }
    }
    
    public void logQuery(String aSql)
    {
        if (mQueryLog)
        {
            PrintWriter sWriter = mHost.getLogWriter();
            
            if (sWriter != null)
            {
                StackTraceElement[] sStacks = Thread.currentThread().getStackTrace();
                StackTraceElement sCaller = sStacks[2];
                synchronized (sWriter)
                {
                    sWriter.println("[QUERY LOG] ["  + currentTime() + "] [" + Thread.currentThread().getId() +"] " + sCaller.getClassName() + "." + sCaller.getMethodName() + ": " + aSql);
                    sWriter.flush();
                }
            }
        }
    }
    
    public void logProtocol(String aOp, String aContent)
    {
        if (mProtocolLog)
        {
            PrintWriter sWriter = mHost.getLogWriter();
            
            if (sWriter != null)
            {
                synchronized (sWriter)
                {
                    sWriter.println("[PROTOCOL LOG] ["  + currentTime() + "] [" + Thread.currentThread().getId() +"] " + aOp + ": " + aContent);
                    sWriter.flush();
                }
            }
        }
    }
}
