package sunje.goldilocks.jdbc.util;

public class NoLogger extends Logger
{
    NoLogger()
    {
        
    }
    
    public boolean isLogOn()
    {
        return false;
    }

    public void logTrace()
    {
        // nothing to do
    }
    
    public void logQuery(String aSql)
    {
        // nothing to do
    }
    
    public void logProtocol(String aOp, String aContent)
    {
        // nothing to do
    }
}
