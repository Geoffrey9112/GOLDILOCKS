package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolGetCursorName extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_GETCURSORNAME;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceGetCursorName sSource = (SourceGetCursorName)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());        
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceGetCursorName sSource = (SourceGetCursorName)aSource;
        
        sSource.setStatementId(aChannel.readStatementId());
        sSource.setCursorName(aChannel.readString());
    }
}

public interface SourceGetCursorName extends SourceStatementIdCreator
{
    void setCursorName(String aCursorName);
}
