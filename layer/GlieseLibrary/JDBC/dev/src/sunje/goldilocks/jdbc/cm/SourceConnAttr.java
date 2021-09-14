package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolGetConnAttr extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_GETCONNATTR;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceConnAttr sSource = (SourceConnAttr)aSource;
        
        aChannel.writeVarInt(sSource.getAttributeType());        
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceConnAttr sSource = (SourceConnAttr)aSource;
        
        aChannel.readVarInt(); // attribute type 은 필요없다.
        byte sValueType = aChannel.readByte();
        if (sValueType == ATTR_DATA_TYPE_INT)
        {
            sSource.setAttributeValue((int)aChannel.readVarInt());
        }
        else
        {
            sSource.setAttributeValue(aChannel.readString());
        }
    }
}

class ProtocolSetConnAttr extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_SETCONNATTR;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceConnAttr sSource = (SourceConnAttr)aSource;
        
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
        // 특별한 결과가 없다.        
    }
}

public interface SourceConnAttr extends Source
{
    int getAttributeType();
    void setAttributeValue(int aValue) throws SQLException;
    void setAttributeValue(String aValue) throws SQLException;
    byte getAttributeValueType();
    int getAttributeIntValue() throws SQLException;
    String getAttributeStringValue() throws SQLException;
}
