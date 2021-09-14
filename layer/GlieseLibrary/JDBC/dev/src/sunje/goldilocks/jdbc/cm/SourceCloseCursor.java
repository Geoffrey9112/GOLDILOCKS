package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolCloseCursor extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_CLOSECURSOR;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceCloseCursor sSource = (SourceCloseCursor)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());        
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceCloseCursor sSource = (SourceCloseCursor)aSource;
        
        sSource.notifyCursorClosed();
    }
}

public interface SourceCloseCursor extends SourceStatementIdHolder
{
    void notifyCursorClosed();
}
