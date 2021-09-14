package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolListenerHandshake extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_LISTENER_HANDSHAKE;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        aChannel.writeInt(((SourceListenerHandshake)aSource).getSessionType());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceHandshake sSource = (SourceHandshake)aSource;
        
        sSource.setIsLittleEndian(aChannel.readByte() == HANDSHAKE_LITTLE_ENDIAN);
    }
}

public interface SourceListenerHandshake extends Source
{
    byte getSessionType();
    void setIsLittleEndian(boolean aIsLittleEndian);
}
