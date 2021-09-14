package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.dt.CodeColumn;

class ProtocolExecute extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_EXECUTE;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceExecute sSource = (SourceExecute)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
        aChannel.writeBoolean(sSource.getIsFirst());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceExecute sSource = (SourceExecute)aSource;
        
        sSource.initExecutionResult();
        sSource.setAffectedRowCount(aChannel.readVarInt());
        sSource.setRecompiled(aChannel.readBoolean());
        sSource.setHasResultSet(aChannel.readBoolean());
        sSource.setHasTransaction(aChannel.readBoolean());
        byte sDclAttrCount = aChannel.readByte();
        if (sDclAttrCount > 0)
        {
            int sDclAttrType = (int)aChannel.readVarInt();
            byte sDclAttrValueType = aChannel.readByte();
            String sDclAttrValue;
            if (sDclAttrValueType == CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER)
            {
                sDclAttrValue = String.valueOf(aChannel.readVarInt());
            }
            else
            {
                sDclAttrValue = aChannel.readString();
            }
            sSource.setDclAttr(sDclAttrType, sDclAttrValue);
        }
    }
}

public interface SourceExecute extends SourceStatementIdCreator
{
    boolean getIsFirst();
    
    void initExecutionResult();
    void setAffectedRowCount(long aRowCount);
    void setRecompiled(boolean aRecompiled);
    void setHasResultSet(boolean aHasResultSet);
    void setHasTransaction(boolean aHasTransaction);
    void setDclAttr(int aAttrType, String aAttrValue);
}
