package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolGetStmtAttr extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_GETSTMTATTR;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceGetStmtAttr sSource = (SourceGetStmtAttr)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
        aChannel.writeVarInt(sSource.getAttributeType());
        aChannel.writeVarInt(sSource.getBufferSizeForAttributeStringValue()); /* string을 담을 버퍼의 크기: 제한을 두지 않음 */
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceGetStmtAttr sSource = (SourceGetStmtAttr)aSource;

        sSource.setStatementId(aChannel.readStatementId());
        sSource.setAttributeType((int)aChannel.readVarInt());
        byte sValueType = aChannel.readByte();
        sSource.setAttributeValueType(sValueType);
        if (sValueType == ATTR_DATA_TYPE_INT)
        {
            sSource.setAttributeIntValue((int)aChannel.readVarInt());
        }
        else
        {
            sSource.setAttributeStringValue(aChannel.readString());
        }

        /*
         * string data의 전체 길이
         */
        sSource.setNeedBufferSize((int)aChannel.readVarInt());
    }
}

public interface SourceGetStmtAttr extends SourceStatementIdCreator
{
    int getAttributeType();
    int getBufferSizeForAttributeStringValue();
    void setAttributeType(int aType);
    void setAttributeValueType(byte aValueType);
    void setAttributeIntValue(int aValue);
    void setAttributeStringValue(String aValue);
    void setNeedBufferSize(int aSize);
}
