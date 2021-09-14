package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolParameterType extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_PARAMETER_TYPE;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceParameterType sSource = (SourceParameterType)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
        aChannel.writeVarInt(sSource.getParameterNumber());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceParameterType sSource = (SourceParameterType)aSource;
        
        sSource.setParameterType((short)aChannel.readVarInt());
    }
}

public interface SourceParameterType extends SourceStatementIdHolder 
{
    int getParameterNumber();
    void setParameterType(short aInOutType);
}
