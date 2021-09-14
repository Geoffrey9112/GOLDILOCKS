package sunje.goldilocks.jdbc.cm;


public interface Source
{
    void notifyWarning(Protocol aProtocol, String aMessage, String aSqlState, int aErrorCode);
    void notifyError(Protocol aProtocol, String aMessage, String aSqlState, int aErrorCode);
    void notifyIgnoredError(Protocol aProtocol);
}
