package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolDescribeCol extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_DESCRIBECOL;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceDescribeCol sSource = (SourceDescribeCol)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
        aChannel.writeVarInt(sSource.getColumnNum());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceDescribeCol sSource = (SourceDescribeCol)aSource;
        
        sSource.setCatalogName(aChannel.readString());
        sSource.setSchemaName(aChannel.readString());
        sSource.setTableName(aChannel.readString());
        sSource.setBaseTableName(aChannel.readString());
        sSource.setColAliasName(aChannel.readString());
        sSource.setColLabel(aChannel.readString());
        sSource.setColBaseName(aChannel.readString());
        sSource.setLiteralPrefix(aChannel.readString());
        sSource.setLiteralSuffix(aChannel.readString());
        sSource.setDBType(aChannel.readByte());
        sSource.setIntervalCode((short)aChannel.readVarInt());
        sSource.setNumericPrecisionRadix((short)aChannel.readVarInt());
        sSource.setDatetimeIntervalPrecision((short)aChannel.readVarInt());
        sSource.setDisplaySize(aChannel.readVarInt());
        sSource.setCharLengthUnit(aChannel.readByte());
        sSource.setCharLength(aChannel.readVarInt());
        sSource.setOctetLength(aChannel.readVarInt());
        sSource.setPrecision((int)aChannel.readVarInt());
        sSource.setScale((int)aChannel.readVarInt());
        sSource.setNullable(aChannel.readBoolean());
        sSource.setAliasUnnamed(aChannel.readBoolean());
        sSource.setCaseSensitive(aChannel.readBoolean());
        sSource.setUnsigned(aChannel.readBoolean());
        sSource.setFixedPrecisionScale(aChannel.readBoolean());
        sSource.setUpdatable(aChannel.readBoolean());
        sSource.setAutoUnique(aChannel.readBoolean());
        sSource.setRowVersion(aChannel.readBoolean());
        sSource.setAbleLike(aChannel.readBoolean());
    }
}

public interface SourceDescribeCol extends SourceStatementIdHolder
{
    int getColumnNum();
    
    void setCatalogName(String aCatalogName);
    void setSchemaName(String aSchemaName);
    void setTableName(String aTableName);
    void setBaseTableName(String aBaseTableName);
    void setColAliasName(String aColAliasName);
    void setColLabel(String aColLabel);
    void setColBaseName(String aColBaseName);
    void setLiteralPrefix(String aPrefix);
    void setLiteralSuffix(String aSuffix);
    void setDBType(byte aDBType);
    void setIntervalCode(short aIntervalCode);
    void setNumericPrecisionRadix(short aNumPrecisionRadix);
    void setDatetimeIntervalPrecision(short aDatetimeIntervalPrecision);
    void setDisplaySize(long aDisplaySize);
    void setCharLengthUnit(byte aCharLenUnit);
    void setCharLength(long aCharLength);
    void setOctetLength(long aOctetLength);
    void setPrecision(int aPrecision);
    void setScale(int aScale);
    void setNullable(boolean aNullable);
    void setAliasUnnamed(boolean aAliasUnnamed);
    void setCaseSensitive(boolean aCaseSensitive);
    void setUnsigned(boolean aUnsigned);
    void setFixedPrecisionScale(boolean aFixedPrecisionScale);
    void setUpdatable(boolean aUpdatable);
    void setAutoUnique(boolean aAutoUnique);
    void setRowVersion(boolean aRowVersion);
    void setAbleLike(boolean aAbleLike);
    void setLogicalDataTypeName(String aTypeName);
}
