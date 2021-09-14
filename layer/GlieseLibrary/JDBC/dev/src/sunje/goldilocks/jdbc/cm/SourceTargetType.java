package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolTargetType extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_TARGETTYPE;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceTargetType sSource = (SourceTargetType)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());        
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceTargetType sSource = (SourceTargetType)aSource;
        
        sSource.setChanged(aChannel.readBoolean());
        int sColCount = (int)aChannel.readVarInt();
        sSource.setColCount(sColCount);
        for (int i=0; i<sColCount; i++)
        {
            byte sType = aChannel.readByte();
            long sPrecision = aChannel.readVarInt();
            long sScale = aChannel.readVarInt();
            byte sLenUnit = aChannel.readByte();
            byte sIntervalInd = aChannel.readByte();
            sSource.setColSpec(i, sType, sPrecision, sScale, sLenUnit, sIntervalInd);
        }
    }
}

public interface SourceTargetType extends SourceStatementIdHolder
{
    void setChanged(boolean aChanged);
    void setColCount(int aCount);
    void setColSpec(int aColIndex, byte aType, long aPrecision, long aScale, byte aLenUnit, byte aIntervalInd) throws SQLException;
}
