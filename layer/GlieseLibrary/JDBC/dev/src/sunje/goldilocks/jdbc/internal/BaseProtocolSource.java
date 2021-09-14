package sunje.goldilocks.jdbc.internal;

import java.sql.SQLException;
import java.sql.SQLWarning;

import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.cm.Source;

public abstract class BaseProtocolSource implements Source
{
    protected ErrorHolder mErrorHolder;
    
    public BaseProtocolSource(ErrorHolder aErrorHolder)
    {
        mErrorHolder = aErrorHolder;
    }
    
    public void notifyWarning(Protocol aProtocol, String aMessage, String aSqlState, int aErrorCode)
    {
        mErrorHolder.addWarning(new SQLWarning(aMessage, aSqlState, aErrorCode));
    }
    
    public void notifyError(Protocol aProtocol, String aMessage, String aSqlState, int aErrorCode)
    {
        mErrorHolder.addError(new SQLException(aMessage, aSqlState, aErrorCode));
    }
    
    public void notifyIgnoredError(Protocol aProtocol)
    {
        /*
         * default behavior is to do nothing
         * ignored error를 무시하면 안되는 경우는 이 메소드를 override해야 한다.
         */
    }
}
