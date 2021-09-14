package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolHandshake extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_HANDSHAKE;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceHandshake sSource = (SourceHandshake)aSource;

        aChannel.writeSystemString(sSource.getRoleName());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceHandshake sSource = (SourceHandshake)aSource;
        
        sSource.setServerCmBufSize((int)aChannel.readVarInt());
    }
}

public interface SourceHandshake extends Source
{
    String getRoleName();
    void setIsLittleEndian(boolean aIsLittleEndian);
    void setServerCmBufSize(int aSize);
}
