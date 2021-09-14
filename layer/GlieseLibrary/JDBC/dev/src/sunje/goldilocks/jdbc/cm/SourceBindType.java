package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolBindType extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_BINDTYPE;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceBindType sSource = (SourceBindType)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
        aChannel.writeVarInt(sSource.getColumnNum());
        aChannel.writeByte(sSource.getBindType());
        aChannel.writeByte(sSource.getDataType());
        aChannel.writeVarInt(sSource.getArgNum1());
        aChannel.writeVarInt(sSource.getArgNum2());
        aChannel.writeByte(sSource.getStringLengthUnit());
        aChannel.writeByte(sSource.getIntervalIndicator());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        ((SourceBindType)aSource).setStatementId(aChannel.readStatementId());
    }
}

public interface SourceBindType extends SourceStatementIdCreator
{
    int getColumnNum();
    byte getBindType();
    byte getDataType();
    long getArgNum1();
    long getArgNum2();
    byte getStringLengthUnit();
    byte getIntervalIndicator();
}
