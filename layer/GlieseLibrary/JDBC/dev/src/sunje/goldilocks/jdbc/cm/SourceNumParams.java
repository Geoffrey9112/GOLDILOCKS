package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolNumParams extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_NUMPARAMS;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceNumParams sSource = (SourceNumParams)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceNumParams sSource = (SourceNumParams)aSource;
        
        sSource.setParamCount((int)aChannel.readVarInt());
    }
}

public interface SourceNumParams extends SourceStatementIdHolder
{
    void setParamCount(int aColCount);
}
