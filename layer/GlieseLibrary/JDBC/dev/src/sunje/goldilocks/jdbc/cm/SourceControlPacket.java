package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolControlPacket extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_CONTROL_PACKET;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        aChannel.writeByte(((SourceControlPacket)aSource).getOperation());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        // result가 없음
    }
}

public interface SourceControlPacket extends Source
{
    byte getOperation();
}
