package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolConnect extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_CONNECT;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceConnect sSource = (SourceConnect)aSource;
        
        aChannel.writeSystemString(sSource.getUserName());
        aChannel.writeSystemString(sSource.getPassword());
        aChannel.writeSystemString(sSource.getOldPassword());
        aChannel.writeSystemString(sSource.getNewPassword());
        aChannel.writeSystemString(sSource.getProgramName());
        aChannel.writeInt(sSource.getProcessId());
        aChannel.writeShort(NLS_CHARSET_UTF8);
        aChannel.writeInt(sSource.getClientTimeZone());
        aChannel.writeByte(CONNECT_DBC_JDBC);
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceConnect sSource = (SourceConnect)aSource;
        
        sSource.setSessionId((int)aChannel.readVarInt());
        sSource.setSessionSequence((int)aChannel.readVarInt());
        sSource.setCharset(aChannel.readShort());
        sSource.setNCharset(aChannel.readShort());
        sSource.setTimeZone(aChannel.readInt());
    }
}

class ProtocolDisconnect extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_DISCONNECT;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        // nothing to do        
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        // nothing to do
    }
}

public interface SourceConnect extends Source
{
    String getUserName();
    String getPassword() throws SQLException;
    String getOldPassword();
    String getNewPassword();
    String getProgramName();
    String getRoleName();
    int getProcessId();
    int getClientTimeZone();
    
    void setSessionId(int aSessionId);
    void setSessionSequence(long aSequence);
    void setCharset(short aCharset) throws SQLException;
    void setNCharset(short aNCharset) throws SQLException;
    void setTimeZone(int aTimeZone);
}
