package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolFreeStatement extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_FREESTMT;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceFreeStatement sSource = (SourceFreeStatement)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
        aChannel.writeVarInt(sSource.getFreeStmtOption());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceFreeStatement sSource = (SourceFreeStatement)aSource;
        
        sSource.notifyStatementFreed();
    }
}

public interface SourceFreeStatement extends SourceStatementIdHolder
{
    short getFreeStmtOption();
    void notifyStatementFreed();
}
