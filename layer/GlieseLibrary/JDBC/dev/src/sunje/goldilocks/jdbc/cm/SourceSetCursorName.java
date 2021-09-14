package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolSetCursorName extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_SETCURSORNAME;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceSetCursorName sSource = (SourceSetCursorName)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
        aChannel.writeSystemString(sSource.getCursorName());        
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceSetCursorName sSource = (SourceSetCursorName)aSource;
        sSource.setStatementId(aChannel.readStatementId());
    }
}

public interface SourceSetCursorName extends SourceStatementIdCreator
{
    String getCursorName();
}
