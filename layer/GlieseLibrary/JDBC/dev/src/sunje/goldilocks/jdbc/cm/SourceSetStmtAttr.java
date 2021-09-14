package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolSetStmtAttr extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_SETSTMTATTR;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceSetStmtAttr sSource = (SourceSetStmtAttr)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
        aChannel.writeVarInt(sSource.getAttributeType());
        byte sValueType = sSource.getAttributeValueType();
        aChannel.writeByte(sValueType);
        if (sValueType == ATTR_DATA_TYPE_INT)
        {
            aChannel.writeVarInt(sSource.getAttributeIntValue());
        }
        else
        {
            aChannel.writeSystemString(sSource.getAttributeStringValue());
        }
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceSetStmtAttr sSource = (SourceSetStmtAttr)aSource;                
        sSource.setStatementId(aChannel.readStatementId());
    }
}

public interface SourceSetStmtAttr extends SourceStatementIdCreator
{
    int getAttributeType();
    byte getAttributeValueType();
    int getAttributeIntValue() throws SQLException;
    String getAttributeStringValue() throws SQLException;
}
