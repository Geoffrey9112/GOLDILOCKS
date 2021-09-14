package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolTransaction extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_TRANSACTION;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceTransaction sSource = (SourceTransaction)aSource;
        
        aChannel.writeVarInt(sSource.getOperation());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceTransaction sSource = (SourceTransaction)aSource;
        sSource.notifyDone();
    }
}

public interface SourceTransaction extends Source
{
    short getOperation();
    void notifyDone();
}
