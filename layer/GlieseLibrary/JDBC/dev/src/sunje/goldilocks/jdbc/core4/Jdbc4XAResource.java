package sunje.goldilocks.jdbc.core4;

import java.sql.SQLException;

import javax.transaction.xa.XAException;
import javax.transaction.xa.XAResource;
import javax.transaction.xa.Xid;

import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.cm.SourceXACommand;
import sunje.goldilocks.jdbc.internal.BaseProtocolSource;
import sunje.goldilocks.jdbc.internal.ErrorHolder;
import sunje.goldilocks.jdbc.util.Logger;

public abstract class Jdbc4XAResource extends ErrorHolder implements XAResource
{
    private static final int XID_REQ_COUNT = 20;
    private static int CURRENT_RMID = 1;
    private static Object LOCK_FOR_RMID = new Object();
    
    private static int generateNewRmid()
    {
        /* 다음 사이트를 보면,
         * http://docs.jboss.org/jbosstm/5.0.0.M1/guides/development_guide/ch03.html
         * JDBC XA interface에는 rmid를 넘겨주는 부분이 없고, XAResource 객체 개별적으로
         * 고유의 rmid를 가진다고 설명되어 있다.
         */
        int sResult;

        synchronized (LOCK_FOR_RMID)
        {
            sResult = CURRENT_RMID;
            CURRENT_RMID++;
        }
        return sResult; 
    }
    
    private static int convertToJDBCXAResult(int aProtocolXAResult)
    {
        int sResult = 0;
        
        switch (aProtocolXAResult)
        {
            case Protocol.XA_RESLUT_RBROLLBACK:
                sResult = XAException.XA_RBROLLBACK;
                break;
            case Protocol.XA_RESLUT_RBCOMMFAIL:
                sResult = XAException.XA_RBCOMMFAIL;
                break;
            case Protocol.XA_RESLUT_RBDEADLOCK:
                sResult = XAException.XA_RBDEADLOCK;
                break;
            case Protocol.XA_RESLUT_RBINTEGRITY:
                sResult = XAException.XA_RBINTEGRITY;
                break;
            case Protocol.XA_RESLUT_RBOTHER:
                sResult = XAException.XA_RBOTHER;
                break;
            case Protocol.XA_RESLUT_RBPROTO:
                sResult = XAException.XA_RBPROTO;
                break;
            case Protocol.XA_RESLUT_RBTIMEOUT:
                sResult = XAException.XA_RBTIMEOUT;
                break;
            case Protocol.XA_RESLUT_NOMIGRATE:
                sResult = XAException.XA_NOMIGRATE;
                break;
            case Protocol.XA_RESLUT_RBEND:
                sResult = XAException.XA_RBEND;
                break;
            case Protocol.XA_RESLUT_HEURHAZ:
                sResult = XAException.XA_HEURHAZ;
                break;
            case Protocol.XA_RESLUT_HEURCOM:
                sResult = XAException.XA_HEURCOM;
                break;
            case Protocol.XA_RESLUT_HEURRB:
                sResult = XAException.XA_HEURRB;
                break;
            case Protocol.XA_RESLUT_HEURMIX:
                sResult = XAException.XA_HEURMIX;
                break;
            case Protocol.XA_RESLUT_RETRY:
                sResult = XAException.XA_RETRY;
                break;
            case Protocol.XA_RESLUT_RDONLY:
                sResult = XAException.XA_RDONLY;
                break;
            case Protocol.XA_RESLUT_OK:
                sResult = XAResource.XA_OK;
                break;
            case Protocol.XA_RESLUT_ER_ASYNC:
                sResult = XAException.XAER_ASYNC;
                break;
            case Protocol.XA_RESLUT_ER_RMERR:
                sResult = XAException.XAER_RMERR;
                break;
            case Protocol.XA_RESLUT_ER_NOTA:
                sResult = XAException.XAER_NOTA;
                break;
            case Protocol.XA_RESLUT_ER_INVAL:
                sResult = XAException.XAER_INVAL;
                break;
            case Protocol.XA_RESLUT_ER_PROTO:
                sResult = XAException.XAER_PROTO;
                break;
            case Protocol.XA_RESLUT_ER_DUPID:
                sResult = XAException.XAER_DUPID;
                break;
            case Protocol.XA_RESLUT_ER_OUTSIDE:
                sResult = XAException.XAER_OUTSIDE;
                break;
            case Protocol.XA_RESLUT_ER_RMFAIL:
                sResult = XAException.XAER_RMFAIL;
                break;
        }
        return sResult;
    }
    
    protected class XAOp extends BaseProtocolSource implements SourceXACommand
    {
        XAOp(ErrorHolder aErrorHolder)
        {
            super(aErrorHolder);
        }
        
        /****************************************************************
         * SourceXACommand interface methods
         ****************************************************************/
        public long getXidFormatId()
        {
            return mFormatId;
        }

        public byte[] getXidGlobalTransId()
        {
            return mGlobalTransId;
        }

        public byte[] getXidBranchQualifier()
        {
            return mBranchQualifier;
        }

        public int getRmId()
        {
            return mRmId;
        }

        public long getXAFlags()
        {
            return mFlags;
        }

        public long getXidReqCount()
        {
            return XID_REQ_COUNT;
        }

        public void setXAResult(int aResult)
        {
            mResult = aResult;        
        }

        public void setXidCount(int aXidCount)
        {
            if (mXids == null || mXids.length != aXidCount)
            {
                mXids = new Jdbc4Xid[aXidCount];
            }
            mXidIndex = 0;
        }

        public void setXid(int aFormatId, byte[] aGlobalTransId, byte[] aBranchQualifier)
        {
            try
            {
                mXids[mXidIndex] = createConcreteXid(aFormatId, aGlobalTransId, aBranchQualifier);
            }
            catch (SQLException sException)
            {
                // 무시한다.
            }
            mXidIndex++;
        }
    }
    
    private CmChannel mChannel;
    private Logger mLogger;
    private int mResult;
    private int mRmId;
    private long mFormatId;
    private long mFlags;
    private byte[] mGlobalTransId;
    private byte[] mBranchQualifier;
    private Jdbc4Xid[] mXids;
    private int mXidIndex = -1;
    private int mUselessTimeout = 0;
    private XAOp mOp;
    
    public Jdbc4XAResource(Jdbc4Connection aConnection)
    {
        mChannel = aConnection.getChannel();
        mLogger = aConnection.mLogger;
        mRmId = generateNewRmid();
        mOp = new XAOp(this);
    }
    
    protected abstract Jdbc4Xid createConcreteXid(int aFormatId, byte[] aGlobalTransIdSrc, byte[] aBranchQualifierSrc) throws SQLException;
    
    private void commandProtocol(Protocol aProtocol) throws XAException
    {
        try
        {
            synchronized (mChannel)
            {
                mChannel.initToSend();
                mChannel.writeProtocolAlone(aProtocol, mOp);
                mChannel.sendAndReceive();
            }
        }
        catch (SQLException sException)
        {
            // XA error는 mResult로 알 수 있다.
            // SQLException은 무시한다.
        }
        
        if (mResult < Protocol.XA_RESLUT_OK)
        {
            throw new XAException(convertToJDBCXAResult(mResult));
        }
    }
    
    private void setXid(Xid aXid)
    {
        mFormatId = aXid.getFormatId();
        mGlobalTransId = aXid.getGlobalTransactionId();
        mBranchQualifier = aXid.getBranchQualifier();
    }
    
    /****************************************************************
     * XAResource interface methods
     ****************************************************************/
    
    void close() throws XAException
    {
        mLogger.logTrace();
        commandProtocol(Protocol.PROTOCOL_XA_CLOSE);
    }
    
    public void commit(Xid aXid, boolean aFlag) throws XAException
    {
        mLogger.logTrace();
        setXid(aXid);
        mFlags = Protocol.XA_FLAGS_NOFLAGS;
        if (aFlag)
        {
            mFlags = Protocol.XA_FLAGS_ONEPHASE;
        }
        commandProtocol(Protocol.PROTOCOL_XA_COMMIT);
    }

    public void end(Xid aXid, int aFlags) throws XAException
    {
        mLogger.logTrace();
        setXid(aXid);
        mFlags = aFlags;
        commandProtocol(Protocol.PROTOCOL_XA_END);        
    }

    public void forget(Xid aXid) throws XAException
    {
        mLogger.logTrace();
        setXid(aXid);
        mFlags = Protocol.XA_FLAGS_NOFLAGS;
        commandProtocol(Protocol.PROTOCOL_XA_FORGET);        
    }

    public int prepare(Xid aXid) throws XAException
    {
        mLogger.logTrace();
        setXid(aXid);
        mFlags = Protocol.XA_FLAGS_NOFLAGS;
        commandProtocol(Protocol.PROTOCOL_XA_PREPARE);
        return convertToJDBCXAResult(mResult);
    }

    public Xid[] recover(int aFlags) throws XAException
    {
        mLogger.logTrace();
        mFlags = aFlags;
        commandProtocol(Protocol.PROTOCOL_XA_RECOVER);
        return mXids;
    }

    public void rollback(Xid aXid) throws XAException
    {
        mLogger.logTrace();
        setXid(aXid);
        mFlags = Protocol.XA_FLAGS_NOFLAGS;
        commandProtocol(Protocol.PROTOCOL_XA_ROLLBACK);        
    }

    public void start(Xid aXid, int aFlags) throws XAException
    {
        mLogger.logTrace();
        setXid(aXid);
        mFlags = aFlags;
        commandProtocol(Protocol.PROTOCOL_XA_START);
    }

    public int getTransactionTimeout() throws XAException
    {
        mLogger.logTrace();
        return mUselessTimeout;
    }

    public boolean setTransactionTimeout(int aTimeout) throws XAException
    {
        mLogger.logTrace();
        // Goldilocks는 transaction timeout을 지원하지 않는다.
        mUselessTimeout = aTimeout;
        return false;
    }

    public boolean isSameRM(XAResource aResource) throws XAException
    {
        mLogger.logTrace();
        if (aResource instanceof Jdbc4XAResource)
        {
            return ((Jdbc4XAResource)aResource).mRmId == mRmId;
        }
        return false;
    }
}
