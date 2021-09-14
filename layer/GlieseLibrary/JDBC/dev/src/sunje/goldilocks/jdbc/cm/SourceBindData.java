package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolBindData extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_BINDDATA;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceBindData sSource = (SourceBindData)aSource;
        aChannel.writeStatementId(sSource.getStatementId());
        int sColCount = sSource.getColumnCount();
        aChannel.writeVarInt(sColCount);
        aChannel.writeVarInt(sSource.getArraySize());
        for (int i=1; i<=sColCount; i++)
        {
            aChannel.writeByte(sSource.getDataType(i));
            sSource.writeTo(aChannel, i);
        }
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        // 특별한 result가 없다.
    }
}

public interface SourceBindData extends SourceStatementIdHolder
{
    int getColumnCount();
    long getArraySize();
    byte getDataType(int aColIndex);
    void writeTo(CmChannel aChannel, int aColIndex) throws SQLException;
}
