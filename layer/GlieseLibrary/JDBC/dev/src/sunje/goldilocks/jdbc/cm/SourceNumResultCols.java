package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolNumResultCols extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_NUMRESULTCOLS;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceNumResultCols sSource = (SourceNumResultCols)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceNumResultCols sSource = (SourceNumResultCols)aSource;
        
        sSource.setResultColCount((int)aChannel.readVarInt());
    }
}

public interface SourceNumResultCols extends SourceStatementIdHolder
{
    void setResultColCount(int aColCount);
}
