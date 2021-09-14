package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.dt.CodeColumn;

class ProtocolExecuteDirect extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_EXECDIRECT;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceExecuteDirect sSource = (SourceExecuteDirect)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
        aChannel.writeBoolean(sSource.getIsFirst());
        // TODO: charset 고려하기
        byte[] sSql = sSource.getSql();
        aChannel.writeVariable(sSql, 0, sSql.length);
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceExecuteDirect sSource = (SourceExecuteDirect)aSource;
        
        sSource.initExecutionResult();
        sSource.setStatementId(aChannel.readStatementId());
        sSource.setStatementType((int)aChannel.readVarInt());
        sSource.setAffectedRowCount(aChannel.readVarInt());
        sSource.setRecompiled(aChannel.readBoolean());
        sSource.setHasResultSet(aChannel.readBoolean());
        sSource.setResultSetHoldability(aChannel.readBoolean());
        sSource.setResultSetConcurrency(aChannel.readBoolean());
        sSource.setResultSetSensitivity(aChannel.readByte());
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

public interface SourceExecuteDirect extends SourceExecute
{
    byte[] getSql();

    void setStatementType(int aType);
    void setResultSetConcurrency(boolean aUpdatable);
    void setResultSetSensitivity(byte aSensitivity);
    void setResultSetHoldability(boolean aCloseAtCommit);
}
