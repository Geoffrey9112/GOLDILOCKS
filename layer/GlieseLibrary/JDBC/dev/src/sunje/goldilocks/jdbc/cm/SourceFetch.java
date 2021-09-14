package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

class ProtocolFetch extends Protocol
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_FETCH;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceFetch sSource = (SourceFetch)aSource;
        
        aChannel.writeStatementId(sSource.getStatementId());
        aChannel.writeVarInt(sSource.getFetchOffset());
        aChannel.writeVarInt(sSource.getFetchCount());
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceFetch sSource = (SourceFetch)aSource;
        int sColCount = (int)aChannel.readVarInt();
        int sRowCount = 0;
        
        sSource.beginInsertion(aChannel.readVarInt());
        while (true)
        {
            sRowCount = (int)aChannel.readVarInt();
            if (sRowCount <= 0)
            {
                break;
            }
            for (int i=0; i<sRowCount; i++)
            {
                byte sRowStatus = aChannel.readByte();
                sSource.setRowStatus(sRowStatus);
                if (sRowStatus != FETCH_ROW_STATUS_DELETED)
                {
                    for (int j=0; j<sColCount; j++)
                    {
                        sSource.setColumnData(j, aChannel);
                    }
                }
                sSource.nextInsertion();
            }
        }

        if (sRowCount == 0)
        {
            sSource.finishInsertion(aChannel.readVarInt(), aChannel.readBoolean());
        }
        else
        {
            /*
             * sRowCount가 음수이면 fetch 도중에 에러가 발생한 경우이다.
             */           
            checkErrorResult(aChannel, RESULT_ABORT, sSource);
        }
    }
}

public interface SourceFetch extends SourceStatementIdHolder
{
    long getFetchOffset();
    int getFetchCount();
    
    void beginInsertion(long aFirstRowIdx);
    void nextInsertion();
    void finishInsertion(long aLastRowIdx, boolean aClosed);
    void setRowStatus(byte aRowStatus);
    void setColumnData(int aColIndex, CmChannel aChannel) throws SQLException;
}
