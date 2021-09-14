package sunje.goldilocks.jdbc.cm;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.SocketChannel;
import java.sql.SQLException;
import java.util.LinkedList;

import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.util.Logger;

public class CmChannel implements SourceHandshake, SourceListenerHandshake
{
    private static final int PACKET_HEADER_SIZE = 8;
    private static final int MTU_SIZE = 32;
    private static final int MAX_VAR_STRING_LEN = 256;
    
    private static final int VAR_LEN_2BYTE_PREFIX = 0xFE;
    private static final int VAR_LEN_4BYTE_PREFIX = 0xFF;
    
    private ByteBuffer mBuf;
    private final SocketChannel mSockChannel;
    private final LinkedList<Protocol> mProtocols;
    private final LinkedList<Source> mSources;
    private final Logger mLogger;
    private LinkFailureListener mListener;
    private byte[] mStringBuf;
    private long mCachedStatementId = Protocol.STATEMENT_ID_UNDEFINED;
    private byte mSessionType;
    private boolean mPeriod = false;
    private String mRoleName;
    private int mSeqNo = 0;

    public Dependency mDependency = new Dependency();
    
    public CmChannel(SocketChannel aChannel, Logger aLogger, int aInitBufSize, boolean aIsLittleEndian, byte aSessionType, String aRoleName)
    {
        mSockChannel = aChannel;
        mLogger = aLogger;
        mBuf = ByteBuffer.allocateDirect(aInitBufSize);
        mProtocols = new LinkedList<Protocol>();
        mSources = new LinkedList<Source>();        
        mStringBuf = new byte[MAX_VAR_STRING_LEN];
        mListener = null;
        mSessionType = aSessionType;
        mRoleName = aRoleName;
        setIsLittleEndian(aIsLittleEndian);
    }
    
    public boolean isLittleEndian()
    {
        return mBuf.order() == ByteOrder.LITTLE_ENDIAN;
    }
    
    public void send() throws SQLException
    {
        if (mLogger.isLogOn())
        {
            mLogger.logProtocol("SEND", String.valueOf(mBuf.position()));
        }
        
        int sLen = mBuf.position();
        if (sLen <= 0)
        {
            ErrorMgr.raiseRuntimeError("nothing to send");
        }
        if (sLen < MTU_SIZE)
        {
            mBuf.position(MTU_SIZE);
        }
        mBuf.flip();
        writeInt(sLen - PACKET_HEADER_SIZE);
        writeByte((byte)(mSeqNo | (mPeriod ? Protocol.PACKET_HEADER_PERIOD_END : Protocol.PACKET_HEADER_PERIOD_CONTINUE)));
        mBuf.rewind();

        /* for debugging
        int sPos = mBuf.position();
        mBuf.position(8);
        short op = mBuf.getShort();
        mBuf.position(sPos);
        System.out.println("");
        System.out.println("dump: "+ op);
        dumpChannel(mBuf);*/
        
        sendBuffer();

        if (mPeriod)
        {
            mSeqNo = (mSeqNo + 1) & 0x7F;
        }
    }

    private void sendBuffer() throws SQLException
    {
        try
        {
            while (mBuf.remaining() > 0)
            {
                mSockChannel.write(mBuf);
            }
            mBuf.clear();
        }
        catch (IOException sException)
        {
            String sMessage = sException.getMessage();
            if (sMessage == null)
            {
                sMessage = "null exception";                
            }
            SQLException sError = ErrorMgr.getException(ErrorMgr.LINK_FAILURE, sMessage);
            if (mListener != null)
            {
                mListener.notifyLinkFailure(sError);
            }
            throw sError;
        }
    }
    
    public void receive() throws SQLException
    {
        int sPayloadSize;
        try
        {
            /* read header */
            mBuf.clear();
            mBuf.limit(MTU_SIZE);
            
            while (mBuf.remaining() > 0)
            {
                if (mSockChannel.read(mBuf) < 0)
                {
                    throw new IOException("Socket closed by server");
                }
            }

            mBuf.flip();
            sPayloadSize = readInt();
            readByte(); // period
            skip(3);
            if (sPayloadSize < 0 || sPayloadSize > mBuf.capacity())
            {
                ErrorMgr.raise(ErrorMgr.LINK_FAILURE, "Invalid packet payload: " + sPayloadSize);
            }
            
            if (sPayloadSize > MTU_SIZE - PACKET_HEADER_SIZE)
            {
                /* 데이터를 더 읽어야 한다.*/
                int sOrgPos = mBuf.position();
                mBuf.position(mBuf.limit());
                mBuf.limit(sOrgPos + sPayloadSize);
                while (mBuf.remaining() > 0)
                {
                    if (mSockChannel.read(mBuf) < 0)
                    {
                        throw new IOException("Server socket closed");
                    }
                }
                mBuf.position(sOrgPos);
            }
            else
            {
                /* 읽을 데이터를 다 읽었고, 뒤에 MTU를 위한 패딩을 제거하기 위해 limit을 재설정한다.*/
                mBuf.limit(mBuf.position() + sPayloadSize);
            }
        }
        catch (IOException sException)
        {
            SQLException sError = ErrorMgr.getException(ErrorMgr.LINK_FAILURE, sException.getMessage());
            if (mListener != null)
            {
                mListener.notifyLinkFailure(sError);
            }
            throw sError;
        }
        
        if (mLogger.isLogOn())
        {
            mLogger.logProtocol("RECEIVED", String.valueOf(mBuf.limit()));
        }
    }
    
    private void readProtocol() throws SQLException
    {
        try
        {
            while (mProtocols.size() > 0)
            {
                Protocol sProtocol = mProtocols.removeFirst();
                Source sSource = mSources.removeFirst();
                if (sProtocol.readProtocolCommon(this, sSource))
                {
                    sProtocol.readProtocol(this, sSource);
                }
                if (mLogger.isLogOn())
                {
                    mLogger.logProtocol("READ PROCOTOL", Protocol.PROTOCOL_NAME[sProtocol.getProtocolCode()]);
                }
            }
        }
        catch (SQLException sException)
        {
            mProtocols.clear();
            mSources.clear();
            throw sException;
        }
    }
    
    public void sendAndReceive() throws SQLException
    {
        writePeriod();
        send();
        invalidatePeriod();
        receive();
        readProtocol();
    }

    public void writeProtocol(Protocol aProtocol, Source aSource, Dependency aDependency) throws SQLException
    {
        writeProtocolInternal(aProtocol, aSource, aDependency.getCode());
        aDependency.next();
    }

    public void writeProtocolAlone(Protocol aProtocol, Source aSource) throws SQLException
    {
        writeProtocolInternal(aProtocol, aSource, Protocol.DEPENDENCY_ALONE);
    }
    
    private void writeProtocolInternal(Protocol aProtocol, Source aSource, byte aDependency) throws SQLException
    {
        if (mLogger.isLogOn())
        {
            mLogger.logProtocol("WRITE PROCOTOL", Protocol.PROTOCOL_NAME[aProtocol.getProtocolCode()]);
        }
        
        writeVarInt(aProtocol.getProtocolCode());
        writeByte(aDependency);
        mProtocols.add(aProtocol);
        mSources.add(aSource);
        aProtocol.writeProtocol(this, aSource);
    }
    
    public void writePeriod() throws SQLException
    {
    	mPeriod = true;
        /* 한 패킷 내에서 유용한 cached statement id를 무효화한다. */
        mCachedStatementId = Protocol.STATEMENT_ID_UNDEFINED;
    }
    
    private void invalidatePeriod()
    {
    	mPeriod = false;
    }
    
    public byte getSessionType()
    {
    	return mSessionType;
    }
    
    public String getRoleName()
    {
        return mRoleName;
    }

    public void setIsLittleEndian(boolean aIsLittleEndian)
    {
        if (aIsLittleEndian)
        {
            mBuf.order(ByteOrder.LITTLE_ENDIAN);
        }
        else
        {
            mBuf.order(ByteOrder.BIG_ENDIAN);
        }
    }
    
    public ByteOrder getByteOrder()
    {
        return mBuf.order();
    }
    
    public void setServerCmBufSize(int aSize)
    {
        if (aSize != mBuf.capacity())
        {
            ByteOrder sOrder = mBuf.order();
            mBuf = ByteBuffer.allocateDirect(aSize);
            mBuf.order(sOrder);
        }
    }
    
    public void close() throws SQLException
    {
        try
        {
            mSockChannel.close();
        }
        catch (IOException sException)
        {
            ErrorMgr.raise(ErrorMgr.LINK_FAILURE, sException.getMessage());
        }
    }
    
    public void sendVersion() throws SQLException
    {
        mBuf.clear();
        mBuf.putShort(Protocol.PROTOCOL_MAJOR_VERSION);
        mBuf.putShort(Protocol.PROTOCOL_MINOR_VERSION);
        mBuf.putShort(Protocol.PROTOCOL_PATCH_VERSION);
        mBuf.flip();
        sendBuffer();
    }

    public void initToSend()
    {
        mBuf.clear();
        mBuf.position(PACKET_HEADER_SIZE);
    }
    
    private void checkSendThreshold(int aSize) throws SQLException
    {
        if (mBuf.remaining() < aSize)
        {
            send();
            initToSend();
        }
    }
    
    private void checkMoreReceive(int aSize) throws SQLException
    {
        if (mBuf.remaining() < aSize)
        {
            if (mBuf.remaining() != 0)
            {
                ErrorMgr.raiseRuntimeError("packet remains");
            }
            receive();
        }
    }
    
    public boolean readBoolean() throws SQLException
    {
        checkMoreReceive(1);
        return mBuf.get() == 0 ? false : true;
    }
    
    public byte readByte() throws SQLException
    {
        checkMoreReceive(1);
        return mBuf.get();
    }
    
    public short readShort() throws SQLException
    {
        checkMoreReceive(2);
        return mBuf.getShort();
    }
    
    public int readInt() throws SQLException
    {
        checkMoreReceive(4);
        return mBuf.getInt(); 
    }
    
    public long readLong() throws SQLException
    {
        checkMoreReceive(8);
        return mBuf.getLong();
    }
    
    public float readFloat() throws SQLException
    {
        checkMoreReceive(4);
        return mBuf.getFloat();
    }
    
    public double readDouble() throws SQLException
    {
        checkMoreReceive(8);
        return mBuf.getDouble();
    }
    
    public int readVarLen() throws SQLException
    {
        checkMoreReceive(1);
        int sLen = mBuf.get() & 0xFF;
        if (sLen < VAR_LEN_2BYTE_PREFIX)
        {
        }
        else if (sLen == VAR_LEN_2BYTE_PREFIX)
        {
            sLen = readShort() & 0xFFFF;
        }
        else
        {
            sLen = readInt();
        }

        return sLen;
    }
    
    public long readVarInt() throws SQLException
    {
        checkMoreReceive(1);
        byte sOne = mBuf.get();
        long sResult;
        
        if (sOne >= Protocol.VAR_INT_1BYTE_MIN && sOne <= Protocol.VAR_INT_1BYTE_MAX)
        {
            sResult = sOne;
        }
        else if (sOne == Protocol.VAR_INT_2BYTE_FLAG)
        {
            checkMoreReceive(2);
            sResult = ((mBuf.get() & 0xFF) << 8) |
                       (mBuf.get() & 0xFF);
        }
        else if (sOne == Protocol.VAR_INT_4BYTE_FLAG)
        {
            checkMoreReceive(4);
            sResult = ((mBuf.get() & 0xFF) << 24) |
                      ((mBuf.get() & 0xFF) << 16) |
                      ((mBuf.get() & 0xFF) << 8) |
                       (mBuf.get() & 0xFF);
        }
        else
        {
            checkMoreReceive(8);
            sResult = ((mBuf.get() & 0xFF) << 56) |
                      ((mBuf.get() & 0xFF) << 48) |
                      ((mBuf.get() & 0xFF) << 40) |
                      ((mBuf.get() & 0xFF) << 32) |
                      ((mBuf.get() & 0xFF) << 24) |
                      ((mBuf.get() & 0xFF) << 16) |
                      ((mBuf.get() & 0xFF) << 8) |
                       (mBuf.get() & 0xFF);
        }
        
        return sResult;
    }
    
    public long readStatementId() throws SQLException
    {
        checkMoreReceive(1);
        byte sOne = mBuf.get();
        long sResult;

        if ((sOne == Protocol.BYTE_STATEMENT_ID_UNDEFIEND) ||
            (sOne == Protocol.BYTE_STATEMENT_ID_PREDEFIEND))
        {
            sResult = Protocol.STATEMENT_ID_UNDEFINED;
            ErrorMgr.raiseRuntimeError("Invalid protocol data: invalid statement id-1");
        }
        
        if ((sOne & 0xFF) <= Protocol.STATEMENT_ID_1BYTE_MAX)
        {
            sResult = sOne & 0xFF;
        }
        else
        {
            if (sOne != Protocol.STATEMENT_ID_2BYTE_FLAG)
            {
                ErrorMgr.raiseRuntimeError("Invalid protocol data: invalid statement id-2");
            }
            checkMoreReceive(2);
            sResult = ((mBuf.get() & 0xFF) << 8) +
                       (mBuf.get() & 0xFF);
        }
        sResult = sResult << 48;
        checkMoreReceive(1);
        sOne = mBuf.get();
        if ((sOne & 0xFF) <= Protocol.STATEMENT_ID_1BYTE_MAX)
        {
            sResult += sOne & 0xFF;
        }
        else if (sOne == Protocol.STATEMENT_ID_2BYTE_FLAG)
        {
            checkMoreReceive(2);
            sResult += ((mBuf.get() & 0xFF) << 8) +
                        (mBuf.get() & 0xFF);
        }
        else if (sOne == Protocol.STATEMENT_ID_4BYTE_FLAG)
        {
            checkMoreReceive(4);
            sResult += ((mBuf.get() & 0xFF) << 24) +
                       ((mBuf.get() & 0xFF) << 16) +
                       ((mBuf.get() & 0xFF) << 8) +
                        (mBuf.get() & 0xFF);
        }
        else
        {
            checkMoreReceive(6);
            sResult += ((mBuf.get() & 0xFF) << 40) +
                       ((mBuf.get() & 0xFF) << 32) +
                       ((mBuf.get() & 0xFF) << 24) +
                       ((mBuf.get() & 0xFF) << 16) +
                       ((mBuf.get() & 0xFF) << 8) +
                        (mBuf.get() & 0xFF);
        }
        return sResult;
    }
    
    /* 이 메소드는 string이 MAX_VAR_STRING_LEN byte 미만인게 보장될 때만 사용해야 한다.
     * 에러 메시지나 attribute 값들에 대해 사용된다.
     */
    public String readString() throws SQLException
    {
        int sLen = readVarLen();
        if (sLen > mStringBuf.length)
        {
            mStringBuf = new byte[sLen];
        }
        readVariableBody(mStringBuf, 0, sLen);
        return new String(mStringBuf, 0, sLen);
    }

    public void readVariable(ByteBuffer aBuf) throws SQLException
    {
        int sLen = readVarLen();
        readVariableBody(aBuf, sLen);
    }
    
    public void readVariableBody(ByteBuffer aBuf, int aLen) throws SQLException
    {
        if (aLen > aBuf.remaining())
        {
            ErrorMgr.raiseRuntimeError("buffer insufficient for variable column");
        }
        
        while (aLen > 0)
        {
            if (aLen <= mBuf.remaining())
            {
                int sOrgLimit = mBuf.limit();
                mBuf.limit(mBuf.position() + aLen);
                aBuf.put(mBuf);
                mBuf.limit(sOrgLimit);
                aLen = 0;
            }
            else
            {
                aLen -= mBuf.remaining();
                aBuf.put(mBuf);
                receive();
            }
        }
    }
    
    public void skip(int aSkip) throws SQLException
    {
        while (aSkip > 0)
        {
            if (aSkip <= mBuf.remaining())
            {
                mBuf.position(mBuf.position() + aSkip);
                aSkip = 0;
            }
            else
            {
                aSkip -= mBuf.remaining();
                mBuf.position(mBuf.limit());
                receive();
            }
        }
    }

    public void readVariableBody(byte[] aBuf, int aOffset, int aLen) throws SQLException
    {
        if (aLen > aBuf.length)
        {
            ErrorMgr.raiseRuntimeError("buffer insufficient for variable column");
        }
        int sOffset = aOffset;
        int sLen;
        while (true)
        {
            sLen = Math.min(aLen, mBuf.remaining());
            mBuf.get(aBuf, sOffset, sLen);
            sOffset += sLen;
            aLen -= sLen;
            if (aLen == 0)
            {
                break;
            }
            receive();
        }
    }
    
    public void writeByte(byte aValue) throws SQLException
    {
        checkSendThreshold(1);
        mBuf.put(aValue);
    }
    
    public void writeBoolean(boolean aValue) throws SQLException
    {
        checkSendThreshold(1);
        if (aValue)
        {
            mBuf.put((byte)1);
        }
        else
        {
            mBuf.put((byte)0);
        }
    }
    
    public void writeShort(short aValue) throws SQLException
    {
        checkSendThreshold(2);
        mBuf.putShort(aValue);
    }
    
    public void writeInt(int aValue) throws SQLException
    {
        checkSendThreshold(4);
        mBuf.putInt(aValue);
    }
    
    public void writeLong(long aValue) throws SQLException
    {
        checkSendThreshold(8);
        mBuf.putLong(aValue);
    }
    
    public void writeFloat(float aValue) throws SQLException
    {
        checkSendThreshold(4);
        mBuf.putFloat(aValue);        
    }
    
    public void writeDouble(double aValue) throws SQLException
    {
        checkSendThreshold(8);
        mBuf.putDouble(aValue);
    }
    
    public void writeVarLen(int aLen) throws SQLException
    {
        checkSendThreshold(5);
        if (aLen < VAR_LEN_2BYTE_PREFIX)
        {
            mBuf.put((byte)aLen);
        }
        else if (aLen == VAR_LEN_2BYTE_PREFIX)
        {
            mBuf.put((byte)VAR_LEN_2BYTE_PREFIX);
            writeShort((short)aLen);
        }
        else
        {
            mBuf.put((byte)VAR_LEN_4BYTE_PREFIX);
            writeInt(aLen);
        }
    }
    
    public void writeVarInt(long aValue) throws SQLException
    {
        checkSendThreshold(9);
        if (aValue >= Protocol.VAR_INT_1BYTE_MIN && aValue <= Protocol.VAR_INT_1BYTE_MAX)
        {
            mBuf.put((byte)aValue);
        }
        else if (aValue >= Short.MIN_VALUE && aValue <= Short.MAX_VALUE)
        {
            mBuf.put(Protocol.VAR_INT_2BYTE_FLAG);
            mBuf.put((byte)((aValue & 0xFF00) >> 8));
            mBuf.put((byte)(aValue & 0xFF));
        }
        else if (aValue >= Integer.MIN_VALUE && aValue <= Integer.MAX_VALUE)
        {
            mBuf.put(Protocol.VAR_INT_4BYTE_FLAG);
            mBuf.put((byte)((aValue & 0xFF000000) >> 24));
            mBuf.put((byte)((aValue & 0xFF0000) >> 16));
            mBuf.put((byte)((aValue & 0xFF00) >> 8));
            mBuf.put((byte)(aValue & 0xFF));
        }
        else
        {
            mBuf.put(Protocol.VAR_INT_8BYTE_FLAG);
            mBuf.put((byte)((aValue & 0xFF00000000000000l) >>> 56));
            mBuf.put((byte)((aValue & 0xFF000000000000l) >> 48));
            mBuf.put((byte)((aValue & 0xFF0000000000l) >> 40));
            mBuf.put((byte)((aValue & 0xFF00000000l) >> 32));
            mBuf.put((byte)((aValue & 0xFF000000) >> 24));
            mBuf.put((byte)((aValue & 0xFF0000) >> 16));
            mBuf.put((byte)((aValue & 0xFF00) >> 8));
            mBuf.put((byte)(aValue & 0xFF));
        }
    }
    
    public void writeStatementId(long aStatementId) throws SQLException
    {
        checkSendThreshold(9);
        if (aStatementId == Protocol.STATEMENT_ID_UNDEFINED)
        {
            mBuf.put(Protocol.BYTE_STATEMENT_ID_UNDEFIEND);
        }
        else if (aStatementId == mCachedStatementId)
        {
            /* 한 패킷 내에서 같은 statement id는 predefined 값으로 대체한다. */
            mBuf.put(Protocol.BYTE_STATEMENT_ID_PREDEFIEND);
        }
        else
        {
            int sUpper = (int)((aStatementId & 0xFFFF000000000000l) >>> 48);
            if (sUpper <= Protocol.STATEMENT_ID_1BYTE_MAX)
            {
                mBuf.put((byte)sUpper);
            }
            else
            {
                mBuf.put(Protocol.STATEMENT_ID_2BYTE_FLAG);
                mBuf.put((byte)((sUpper & 0xFF00) >> 8));
                mBuf.put((byte)(sUpper & 0xFF));
            }
            long sLower = aStatementId & 0x0000FFFFFFFFFFFFl;
            if (sLower <= Protocol.STATEMENT_ID_1BYTE_MAX)
            {
                mBuf.put((byte)sLower);
            }
            else if (sLower <= 0xFFFF)
            {
                mBuf.put(Protocol.STATEMENT_ID_2BYTE_FLAG);
                mBuf.put((byte)((sLower & 0xFF00) >> 8));
                mBuf.put((byte)(sLower & 0xFF));
            }
            else if (sLower <= 0xFFFFFFFF)
            {
                mBuf.put(Protocol.STATEMENT_ID_4BYTE_FLAG);
                mBuf.put((byte)((sLower & 0xFF000000) >> 24));
                mBuf.put((byte)((sLower & 0xFF0000) >> 16));
                mBuf.put((byte)((sLower & 0xFF00) >> 8));
                mBuf.put((byte)(sLower & 0xFF));
            }
            else
            {
                mBuf.put(Protocol.STATEMENT_ID_8BYTE_FLAG);
                mBuf.put((byte)((sLower & 0xFF0000000000l) >> 40));
                mBuf.put((byte)((sLower & 0xFF00000000l) >> 32));
                mBuf.put((byte)((sLower & 0xFF000000) >> 24));
                mBuf.put((byte)((sLower & 0xFF0000) >> 16));
                mBuf.put((byte)((sLower & 0xFF00) >> 8));
                mBuf.put((byte)(sLower & 0xFF));
            }
            
            /* 현재 statement id를 cache한다. */
            mCachedStatementId = aStatementId;
        }
    }
    
    /*
     * 이 메소드는 사용자 데이터가 아닌, 내부 문자열을 보낼 때만 사용해야 한다.
     * Charset을 고려한 인코딩을 하지 않으며 데이터가 항상 영문자라고 가정하기 때문이다.
     */
    public void writeSystemString(String aValue) throws SQLException
    {
        int sLen = aValue.length();
        if (mBuf.remaining() < sLen)
        {
            ErrorMgr.raiseRuntimeError("write divided string");
        }
        
        writeVarLen(sLen);
        mBuf.put(aValue.getBytes(), 0, sLen);
    }
    
    public void writeVariable(ByteBuffer aBuf) throws SQLException
    {
        int sLen = aBuf.remaining();
        writeVarLen(sLen);
        
        while (sLen > 0)
        {
            int sRemainLen = mBuf.remaining();
            if (sLen <= sRemainLen)
            {
                mBuf.put(aBuf);
            }
            else
            {
                int sOrgLimit = aBuf.limit();
                aBuf.limit(aBuf.position() + sRemainLen);
                mBuf.put(aBuf);
                aBuf.limit(sOrgLimit);
                send();
                initToSend();
            }
            sLen = aBuf.remaining();
        }
    }

    public void writeVariableBody(ByteBuffer aBuf) throws SQLException
    {
        int sLen = aBuf.remaining();
        
        while (sLen > 0)
        {
            int sRemainLen = mBuf.remaining();
            if (sLen <= sRemainLen)
            {
                mBuf.put(aBuf);
            }
            else
            {
                int sOrgLimit = aBuf.limit();
                aBuf.limit(aBuf.position() + sRemainLen);
                mBuf.put(aBuf);
                aBuf.limit(sOrgLimit);
                send();
                initToSend();
            }
            sLen = aBuf.remaining();
        }
    }

    public void writeVariable(byte[] aBuf, int aOffset, int aLen) throws SQLException
    {
        writeVarLen(aLen);
        writeVariableBody(aBuf, aOffset, aLen);
    }
    
    public void writeVariableBody(byte[] aBuf, int aOffset, int aLen) throws SQLException
    {
        int sPos = aOffset;
        int sWritingLen;
        
        while (aLen > 0)
        {
            if (aLen <= mBuf.remaining())
            {
                mBuf.put(aBuf, sPos, aLen);
                aLen = 0;
            }
            else
            {
                sWritingLen = mBuf.remaining();
                mBuf.put(aBuf, sPos, sWritingLen);
                sPos += sWritingLen;
                aLen -= sWritingLen;
                send();
                initToSend();
            }
        }
    }
    
    public void notifyWarning(Protocol aProtocol, String aMessage, String aSqlState, int aErrorCode)
    {
        /*
         * handshake 과정에서 발생하는 warning은 무시한다.
         */        
    }
    
    public void notifyError(Protocol aProtocol, String aMessage, String aSqlState, int aErrorCode)
    {
        try
        {
            close();
        }
        catch (SQLException sException)
        {
            // exception은 무시한다.
        }
    }
    
    public void notifyIgnoredError(Protocol aProtocol)
    {
        /*
         * 특별히 처리할 게 없다.
         */
    }
    
    public void setLinkFailureListener(LinkFailureListener aListener)
    {
        mListener = aListener;
    }
    
    private static final char[] HEX = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
    protected static void dumpChannel(ByteBuffer aBuf)
    {
        int sPos = aBuf.position();
        int i=1;
        while (aBuf.remaining() > 0)
        {
            int sByte = aBuf.get() & 0xFF;
            int sUpper = sByte >> 4;
            int sLower = sByte & 0x0F;
            System.out.print(HEX[sUpper] + "" + HEX[sLower] + " ");
            if (i % 100 == 0)
            {
                System.out.println("");
                i = 0;                
            }
            i++;
        }
        aBuf.position(sPos);
    }
}
