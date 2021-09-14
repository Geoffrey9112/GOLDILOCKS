package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

abstract class ProtocolXACommon extends Protocol
{
    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceXACommand sSource = (SourceXACommand)aSource;
        aChannel.writeLong(sSource.getXidFormatId());
        byte[] sGlobalTransId = sSource.getXidGlobalTransId();
        byte[] sBranchQualifier = sSource.getXidBranchQualifier();
        aChannel.writeLong(sGlobalTransId.length);
        aChannel.writeLong(sBranchQualifier.length);
        aChannel.writeVarLen(sGlobalTransId.length + sBranchQualifier.length);
        aChannel.writeVariableBody(sGlobalTransId, 0, sGlobalTransId.length);
        aChannel.writeVariableBody(sBranchQualifier, 0, sBranchQualifier.length);
        aChannel.writeInt(sSource.getRmId());
        aChannel.writeLong(sSource.getXAFlags());
    }

    boolean readProtocolCommon(CmChannel aChannel, Source aSource) throws SQLException
    {
        /* XA protocol이 추가되면서 error result를 읽는 '일반적인 룰'이 깨지고 말았다.
         * 부득이하게 readProtocolCommon() 메소드를 상속받아 재구현한다.
         */
        short sCode = (short)aChannel.readVarInt();
        if (sCode != getProtocolCode())
        {
            ErrorMgr.raise(ErrorMgr.INVALID_PROTOCOL, String.valueOf(sCode));
        }
        byte sResultCode = aChannel.readByte();
        ((SourceXACommand)aSource).setXAResult(aChannel.readInt());
        return checkErrorResult(aChannel, sResultCode, aSource);
    }

    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        // 읽을 결과가 없음
    }
}

class ProtocolXAClose extends ProtocolXACommon
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_XA_CLOSE;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        // XA_CLOSE는 특별히 보낼 데이터가 없다.
    }
}

class ProtocolXACommit extends ProtocolXACommon
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_XA_COMMIT;
    }
}

class ProtocolXAEnd extends ProtocolXACommon
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_XA_END;
    }
}

class ProtocolXAForget extends ProtocolXACommon
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_XA_FORGET;
    }
}

class ProtocolXAPrepare extends ProtocolXACommon
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_XA_PREPARE;
    }
}

class ProtocolXARecover extends ProtocolXACommon
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_XA_RECOVER;
    }

    void writeProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceXACommand sSource = (SourceXACommand)aSource;
        aChannel.writeLong(sSource.getXidReqCount());
        aChannel.writeInt(sSource.getRmId());
        aChannel.writeLong(sSource.getXAFlags());
    }
    
    void readProtocol(CmChannel aChannel, Source aSource) throws SQLException
    {
        SourceXACommand sSource = (SourceXACommand)aSource;
        
        long sXidCount = aChannel.readLong();
        sSource.setXidCount((int)sXidCount);
        byte[] sGlobalTransId = null;
        byte[] sBranchQualifier = null;
        for (long i=0; i<sXidCount; i++)
        {
            int sFormatId = (int)aChannel.readLong();
            int sGlobalTransIdLen = (int)aChannel.readLong();
            int sBranchQualifierLen = (int)aChannel.readLong();
            if (sGlobalTransId == null || sGlobalTransId.length != sGlobalTransIdLen)
            {
                sGlobalTransId = new byte[sGlobalTransIdLen];
            }
            if (sBranchQualifier == null || sBranchQualifier.length != sBranchQualifierLen)
            {
                sBranchQualifier = new byte[sBranchQualifierLen];
            }
            aChannel.readVarLen(); // global trans id와 branch qualifier가 붙은 데이터의 길이이므로 필요없다.
            aChannel.readVariableBody(sGlobalTransId, 0, sGlobalTransIdLen);
            aChannel.readVariableBody(sBranchQualifier, 0, sBranchQualifierLen);
            sSource.setXid(sFormatId, sGlobalTransId, sBranchQualifier);
        }
    }
}

class ProtocolXARollback extends ProtocolXACommon
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_XA_ROLLBACK;
    }
}

class ProtocolXAStart extends ProtocolXACommon
{
    short getProtocolCode()
    {
        return PROTOCOL_CODE_XA_START;
    }
}

public interface SourceXACommand extends Source
{
    long getXidFormatId();
    byte[] getXidGlobalTransId();
    byte[] getXidBranchQualifier();
    int getRmId();
    long getXAFlags();
    long getXidReqCount();
    
    void setXAResult(int aResult);
    void setXidCount(int aXidCount);
    void setXid(int aXidFormat, byte[] GlobalTransId, byte[] aBranchQualifier);
}
