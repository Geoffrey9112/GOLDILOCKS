package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolPrepare extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_PREPARE;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourcePrepare sSource = (SourcePrepare)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
        byte[] sSql = sSource.getSql();
        aChannel.writeVariable(sSql, 0, sSql.length);
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourcePrepare sSource = (SourcePrepare)aSource;
        
        sSource.setStatementId(aChannel.readStatementId());
        sSource.setStatementType((int)aChannel.readVarInt());
        sSource.setHasTransactionAtPreparation(aChannel.readBoolean());
        sSource.setHasResultSetAtPreparation(aChannel.readBoolean());
        sSource.setResultSetHoldability(aChannel.readBoolean());
        sSource.setResultSetConcurrency(aChannel.readBoolean());
        sSource.setResultSetSensitivity(aChannel.readByte());
    }
}

public interface SourcePrepare extends SourceStatementIdCreator
{
    byte[] getSql();
    
    void setStatementType(int aType);
    void setHasTransactionAtPreparation(boolean aHasTransaction);
    void setHasResultSetAtPreparation(boolean aHasResultSet);
    void setResultSetConcurrency(boolean aUpdatable);
    void setResultSetSensitivity(byte aSensitivity);
    void setResultSetHoldability(boolean aCloseAtCommit);
}
