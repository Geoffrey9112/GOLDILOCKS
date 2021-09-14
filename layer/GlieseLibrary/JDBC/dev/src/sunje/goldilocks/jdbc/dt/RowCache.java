package sunje.goldilocks.jdbc.dt;

import java.io.IOException;
import java.io.InputStream;
import java.io.Reader;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.CharBuffer;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;
import java.nio.charset.CoderResult;
import java.nio.charset.CodingErrorAction;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.env.SessionEnv;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

/* structure로 사용하기 위한 클래스임 */
class RowBufChunk
{
    int mChunkId;
    ByteBuffer mRowBuf;
    RowBufChunk mPrev;
    RowBufChunk mNext;
}

class ColumnInfoChunk
{
    int mBaseRowIdx;
    int[][][] mInfo;
    ColumnInfoChunk mPrev;
    ColumnInfoChunk mNext;
    byte[] mRowStatus;
}

class ExtColumnInfoChunk
{
    int mChunkId;
    int[][] mInfo;
    ExtColumnInfoChunk mPrev;
    ExtColumnInfoChunk mNext;
}

public class RowCache
{
    /**
     *  버퍼 크기 설정
     *  일반적으로 컬럼은 4000보다 작기때문에(한글의 경우 varchar에 최대한 16000바이트까지 차지할 순 있음)
     *  32K 버퍼에 저장한다. 32K 버퍼가 여러개 생겨 리스트로 관리된다.
     *  하지만 longvarchar 같이 큰 컬럼이 있을 경우에는 32K 버퍼의 개수가 많아져
     *  리스트가 많이 달릴 수 있는데, 큰 문제는 없으나 리스트 관리 비용이 증가되기 때문에
     *  버퍼를 크게 할당한다. 이를 위해 ROW_CACHE_BUFFER_SIZE_CLASS를 둔다.
     */
    private static final int DEFAULT_ROW_CACHE_BUFFER_SIZE = 32768;
    private static final int[] ROW_CACHE_BUFFER_SIZE_CLASS = { 256 * 1024, 1024 * 1024 };
    private static final int STREAM_READ_BUFFER_SIZE = Column.LONG_TYPE_MAX_ALLOC_SIZE;
    
    private static final int COL_INFO_COUNT = 3;
    private static final int CHUNK_ID = 0;
    private static final int OFFSET = 1;
    private static final int LENGTH = 2;
    private static final int OPTION_EXT_CHUNK_ID = 0;
    private static final int OPTION_EXT_IDX = 1;
    private static final int OPTION_EXT_TOTAL_LEN = 2;
    
    private static final int EXT_COL_INFO_COUNT = 5;
    private static final int EXT_NEXT_CHUNK_ID = 3;
    private static final int EXT_NEXT_IDX = 4;

    /* 이 값은 컬럼이 쪼개질 수 없는 최대 버퍼 공간크기이다. 고정 길이 컬럼들보다 크게 잡아야 한다.*/
    private static final int ROW_BUF_MAX_REMAIN_SIZE = 32;
    private static final int EXT_INFO_SIZE = 32;
    
    private static final int NO_NEXT = -1;
    
    private RowBufChunk mRowBufChunkList;
    private RowBufChunk mCurInsRowBuf;
    private RowBufChunk mCurFetRowBuf;
    private ColumnInfoChunk mColInfoChunkList;
    private ColumnInfoChunk mCurInsCol;
    private ColumnInfoChunk mCurFetCol;
    private int mInsRowPos;
    private int mFetRowPos;
    private int mRowCount;
    private int mColCount;
    private int mInsertedRowCount;
    private ExtColumnInfoChunk mExtInfoList;
    private ExtColumnInfoChunk mInsExtInfo;
    private ExtColumnInfoChunk mFetExtInfo;
    private int mInsExtIdx;
    private CharsetEncoder mEncoder;
    private CharsetDecoder mDecoder;
    private SessionEnv mEnv;
    private boolean mIsLittleEndian;
    private int[] mCurWritingColInfo;
    private byte[] mBufForStream;
    private char[] mBufForReader;
    private CharBuffer mWrapBuf;
    private ByteBuffer mUnderflowBuf;
    private long mAccumulatedInsertedCount = 0;
    
    public RowCache(int aColCount, boolean aIsLittleEndian, int aRowCount, SessionEnv aEnv, boolean aRowStatus)
    {
        mColCount = aColCount;
        mIsLittleEndian = aIsLittleEndian;
        mRowCount = aRowCount;
        
        mRowBufChunkList = new RowBufChunk();
        mRowBufChunkList.mRowBuf = allocByteBuffer(DEFAULT_ROW_CACHE_BUFFER_SIZE);
        mRowBufChunkList.mChunkId = 0;
        mRowBufChunkList.mPrev = null;
        mRowBufChunkList.mNext = null;
        mCurInsRowBuf = mRowBufChunkList;
        mCurFetRowBuf = mRowBufChunkList;
        
        mColInfoChunkList = new ColumnInfoChunk();
        mColInfoChunkList.mBaseRowIdx = 0;
        mColInfoChunkList.mInfo = new int[mRowCount][mColCount][COL_INFO_COUNT];
        mColInfoChunkList.mPrev = null;
        mColInfoChunkList.mNext = null;
        mColInfoChunkList.mRowStatus = null;
        if (aRowStatus)
        {
            mColInfoChunkList.mRowStatus = new byte[mRowCount];
        }
        
        mCurInsCol = mColInfoChunkList;
        mCurFetCol = mColInfoChunkList;
        mInsRowPos = 0;
        mFetRowPos = -1;
        
        mExtInfoList = null;
        mInsExtInfo = null;
        mInsExtIdx = -1;
        mFetExtInfo = null;
        
        mEnv = aEnv;
        mEncoder = mEnv.getNewEncoder();
        mDecoder = mEnv.getNewDecoder();
        mDecoder.replaceWith(mEnv.getDecodingReplacement());
        mDecoder.onMalformedInput(CodingErrorAction.REPLACE);
        mDecoder.onUnmappableCharacter(CodingErrorAction.REPLACE);
        
        /* decode하는 과정에서 ByteBuffer의 짜투리가 남는 경우 이 버퍼로 옮겨 decode를 계속한다. */
        mUnderflowBuf = ByteBuffer.allocate(maxBytesForChars(1));
    }

    private ByteBuffer allocByteBuffer(int aBufSize)
    {
        ByteBuffer sBuf = ByteBuffer.allocate(aBufSize);
        if (mIsLittleEndian)
        {
            sBuf.order(ByteOrder.LITTLE_ENDIAN);
        }
        else
        {
            sBuf.order(ByteOrder.BIG_ENDIAN);
        }
        return sBuf;
    }

    private void allocNewBufChunk(int aBufSize)
    {
        RowBufChunk sChunk = new RowBufChunk();
        sChunk.mRowBuf = allocByteBuffer(aBufSize);
        sChunk.mChunkId = mCurInsRowBuf.mChunkId + 1;
        sChunk.mPrev = mCurInsRowBuf;
        sChunk.mNext = null;
        mCurInsRowBuf.mNext = sChunk;
        mCurInsRowBuf = sChunk;
    }
    
    private void allocNewColInfo()
    {
        ColumnInfoChunk sChunk = new ColumnInfoChunk();
        sChunk.mBaseRowIdx = mCurInsCol.mBaseRowIdx + mRowCount;
        sChunk.mInfo = new int[mRowCount][mColCount][COL_INFO_COUNT];
        sChunk.mPrev = mCurInsCol;
        sChunk.mNext = null;
        sChunk.mRowStatus = null;
        if (mCurInsCol.mRowStatus != null)
        {
            sChunk.mRowStatus = new byte[mRowCount];
        }
        
        mCurInsCol.mNext = sChunk;
        mCurInsCol = sChunk;
    }
    
    private void prepareNewExtInfo()
    {
        if (mInsExtInfo == null)
        {
            mExtInfoList = new ExtColumnInfoChunk();
            mExtInfoList.mChunkId = 1; /* ext info의 chunk id는 1, 2, ...로 진행한다. */
            mExtInfoList.mInfo = new int[EXT_INFO_SIZE][EXT_COL_INFO_COUNT];
            mExtInfoList.mNext = null;
            mExtInfoList.mPrev = null;
            mInsExtInfo = mExtInfoList;
            mInsExtIdx = 0;
        }
        else if (mInsExtIdx >= EXT_INFO_SIZE - 1)
        {
            mInsExtInfo.mNext = new ExtColumnInfoChunk();
            mInsExtInfo.mNext.mPrev = mInsExtInfo;
            mInsExtInfo = mInsExtInfo.mNext;
            mInsExtInfo.mChunkId = mInsExtInfo.mPrev.mChunkId + 1; /* ext info의 chunk id는 1, 2, ...로 진행한다. */
            mInsExtInfo.mInfo = new int[EXT_INFO_SIZE][EXT_COL_INFO_COUNT];
            mInsExtInfo.mNext = null;
            mInsExtIdx = 0;
        }
        else
        {
            mInsExtIdx++;
        }
    }
    
    private int[] getExtInfo(int aChunkId, int aIdx) throws SQLException
    {
        /* ext info의 chunk id는 1, 2, ...로 진행한다. */
        if (mFetExtInfo == null)
        {
            mFetExtInfo = mExtInfoList;
        }
        if (mFetExtInfo.mChunkId == aChunkId)
        {
            return mFetExtInfo.mInfo[aIdx];
        }
        else if (mFetExtInfo.mChunkId < aChunkId)
        {
            do
            {
                mFetExtInfo = mFetExtInfo.mNext;
                if (mFetExtInfo == null)
                {
                    ErrorMgr.raiseRuntimeError("Invalid extended info chunk id");
                }
            } while (mFetExtInfo.mChunkId < aChunkId);            
        }
        else
        {
            do
            {
                mFetExtInfo = mFetExtInfo.mPrev;
                if (mFetExtInfo == null)
                {
                    ErrorMgr.raiseRuntimeError("Invalid extended info chunk id");
                }
            } while (mFetExtInfo.mChunkId > aChunkId);
        }
        return mFetExtInfo.mInfo[aIdx];
    }
    
    private int checkRowBufRemains(int aLength)    
    {
        int sRemain = mCurInsRowBuf.mRowBuf.remaining();
        if (sRemain < aLength)
        {
            if (sRemain > ROW_BUF_MAX_REMAIN_SIZE)
            {
                return sRemain;
            }
            else
            {
                if (mCurInsRowBuf.mNext == null)
                {
                    if (aLength < DEFAULT_ROW_CACHE_BUFFER_SIZE)
                    {
                        allocNewBufChunk(DEFAULT_ROW_CACHE_BUFFER_SIZE);
                    }
                    else                        
                    {
                        /* longvarchar 같은 타입의 경우 여기로 올 수 있다. 버퍼 크기를 크게 할당한다. */
                        for (int i = 0; i < ROW_CACHE_BUFFER_SIZE_CLASS.length; i++)
                        {
                            if (aLength <= ROW_CACHE_BUFFER_SIZE_CLASS[i] ||
                                i == ROW_CACHE_BUFFER_SIZE_CLASS.length - 1)
                            {
                                allocNewBufChunk(ROW_CACHE_BUFFER_SIZE_CLASS[i]);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    mCurInsRowBuf = mCurInsRowBuf.mNext;
                }
                mCurInsRowBuf.mRowBuf.clear();
                return Math.min(aLength, mCurInsRowBuf.mRowBuf.remaining());
            }
        }
        return aLength;
    }
    
    private void verifyRowBuf(int aChunkId)
    {
        if (aChunkId == mCurFetRowBuf.mChunkId)
        {
            return;
        }
        else if (aChunkId > mCurFetRowBuf.mChunkId)
        {
            while (aChunkId > mCurFetRowBuf.mChunkId)
            {
                mCurFetRowBuf = mCurFetRowBuf.mNext;
            }            
        }
        else
        {
            while (aChunkId < mCurFetRowBuf.mChunkId)
            {
                mCurFetRowBuf = mCurFetRowBuf.mPrev;
            }
        }
    }

    private int prepareWriteColumn(int aColIndex, int aLen)
    {
        if (mInsRowPos >= mCurInsCol.mInfo.length)
        {
            if (mCurInsCol.mNext == null)
            {
                allocNewColInfo();
            }
            else
            {
                mCurInsCol = mCurInsCol.mNext;
            }
            mInsRowPos = 0;
        }
        
        int sWritingLen = checkRowBufRemains(aLen);
        
        if (aLen == sWritingLen)
        {
            /* buffer에 column을 쪼개지 않고 넣을 수 있는 경우 */
            mCurInsCol.mInfo[mInsRowPos][aColIndex][CHUNK_ID] = mCurInsRowBuf.mChunkId;
            mCurInsCol.mInfo[mInsRowPos][aColIndex][OFFSET] = mCurInsRowBuf.mRowBuf.position();
            mCurInsCol.mInfo[mInsRowPos][aColIndex][LENGTH] = sWritingLen;
            mCurWritingColInfo = mCurInsCol.mInfo[mInsRowPos][aColIndex];
            return 0;
        }
        else
        {
            /* buffer에 column을 쪼개서 넣어야 하는 경우 */
            prepareNewExtInfo();
            mCurInsCol.mInfo[mInsRowPos][aColIndex][OPTION_EXT_CHUNK_ID] = -mInsExtInfo.mChunkId; /* ext info를 표시하기 위해 -값을 세팅 */
            mCurInsCol.mInfo[mInsRowPos][aColIndex][OPTION_EXT_IDX] = mInsExtIdx;
            mCurInsCol.mInfo[mInsRowPos][aColIndex][OPTION_EXT_TOTAL_LEN] = aLen;
            mInsExtInfo.mInfo[mInsExtIdx][CHUNK_ID] = mCurInsRowBuf.mChunkId;
            mInsExtInfo.mInfo[mInsExtIdx][OFFSET] = mCurInsRowBuf.mRowBuf.position();
            mInsExtInfo.mInfo[mInsExtIdx][LENGTH] = sWritingLen;
            mInsExtInfo.mInfo[mInsExtIdx][EXT_NEXT_CHUNK_ID] = NO_NEXT; // 코드-A; ext info가 실제로 필요없는 경우도 있다.
            mInsExtInfo.mInfo[mInsExtIdx][EXT_NEXT_IDX] = NO_NEXT;
            mCurWritingColInfo = mInsExtInfo.mInfo[mInsExtIdx];
            return aLen - sWritingLen;
        }
    }

    private int prepareWriteColumnNextExt(int aColIndex, int aLen)
    {
        int sWritingLen = checkRowBufRemains(aLen);

        int[] sLastExtInfo = mInsExtInfo.mInfo[mInsExtIdx];
        prepareNewExtInfo();
        sLastExtInfo[EXT_NEXT_CHUNK_ID] = mInsExtInfo.mChunkId;
        sLastExtInfo[EXT_NEXT_IDX] = mInsExtIdx;
        mInsExtInfo.mInfo[mInsExtIdx][CHUNK_ID] = mCurInsRowBuf.mChunkId;
        mInsExtInfo.mInfo[mInsExtIdx][OFFSET] = mCurInsRowBuf.mRowBuf.position();
        mInsExtInfo.mInfo[mInsExtIdx][LENGTH] = sWritingLen;
        mInsExtInfo.mInfo[mInsExtIdx][EXT_NEXT_CHUNK_ID] = NO_NEXT;
        mInsExtInfo.mInfo[mInsExtIdx][EXT_NEXT_IDX] = NO_NEXT;
        mCurWritingColInfo = mInsExtInfo.mInfo[mInsExtIdx];
        
        if (sWritingLen == aLen)
        {
            return 0;
        }
        else
        {
            return aLen - sWritingLen;
        }
    }
    
    private int[] prepareReadColumn(int aColIndex) throws SQLException
    {
        if (readRowStatus() == Protocol.FETCH_ROW_STATUS_DELETED)
        {
            ErrorMgr.raise(ErrorMgr.READ_THE_DELETED_ROW);
        }

        int[] sTargetCol = mCurFetCol.mInfo[mFetRowPos][aColIndex];
        
        if (sTargetCol[CHUNK_ID] >= 0)
        {
            verifyRowBuf(sTargetCol[CHUNK_ID]);
            mCurFetRowBuf.mRowBuf.clear();
            mCurFetRowBuf.mRowBuf.position(sTargetCol[OFFSET]);
            mCurFetRowBuf.mRowBuf.limit(sTargetCol[OFFSET] + sTargetCol[LENGTH]);
            return null;
        }
        else
        {
            int[] sInfo = getExtInfo(-sTargetCol[OPTION_EXT_CHUNK_ID], sTargetCol[OPTION_EXT_IDX]);
            verifyRowBuf(sInfo[CHUNK_ID]);
            mCurFetRowBuf.mRowBuf.clear();
            mCurFetRowBuf.mRowBuf.position(sInfo[OFFSET]);
            mCurFetRowBuf.mRowBuf.limit(sInfo[OFFSET] + sInfo[LENGTH]);
            if (sInfo[EXT_NEXT_CHUNK_ID] == NO_NEXT)
            {
                /*
                 * prepareWriteColumn()에서 처음에 ext info가 필요하다고 판단했으나
                 * 실제로 데이터를 써보니 남은 공간에 다 쓴 경우엔
                 * prepareWriteColumnNextExt()를 호출하지 않는 경우도 있다.
                 * 이럴 경우 코드-A에 의해 이 조건에 들어오게 된다.
                 * ext info가 없는 경우이다.
                 */
                return null;
            }
            return getExtInfo(sInfo[EXT_NEXT_CHUNK_ID], sInfo[EXT_NEXT_IDX]);
        }
    }
    
    private int[] prepareReadColumnNextExt(int[] aExtInfo) throws SQLException
    {
        verifyRowBuf(aExtInfo[CHUNK_ID]);
        mCurFetRowBuf.mRowBuf.clear();
        mCurFetRowBuf.mRowBuf.position(aExtInfo[OFFSET]);
        mCurFetRowBuf.mRowBuf.limit(aExtInfo[OFFSET] + aExtInfo[LENGTH]);
        
        if (aExtInfo[EXT_NEXT_CHUNK_ID] == NO_NEXT)
        {
            return null;
        }
        else
        {
            return getExtInfo(aExtInfo[EXT_NEXT_CHUNK_ID], aExtInfo[EXT_NEXT_IDX]);
        }
    }

    private int resize(int aSize)
    {
        int sBufSize;
        
        if (aSize < STREAM_READ_BUFFER_SIZE)
        {
            // aLength가 Integer.MAX_VALUE일 경우 aLength * 2를 하면 다시 작아지기 때문에
            // 이 조건을 건다.
            sBufSize = Math.min(STREAM_READ_BUFFER_SIZE, aSize * 2);
        }
        else
        {
            sBufSize = STREAM_READ_BUFFER_SIZE;
        }
        
        return sBufSize;
    }
    
    private int verifyLength(int aLength)
    {
        return Math.min(aLength, STREAM_READ_BUFFER_SIZE);
    }
    
    private void verifyBufferForStream(int aLength)
    {
        aLength = verifyLength(aLength);
        
        if (mBufForStream == null)
        {
            mBufForStream = new byte[aLength];
        }
        else if (mBufForStream.length < aLength)
        {
            mBufForStream = new byte[resize(aLength)];
        }
    }
    
    private void verifyBufferForReader(int aLength)
    {
        aLength = verifyLength(aLength);
        
        if (mBufForReader == null)
        {
            mBufForReader = new char[aLength];
            mWrapBuf = CharBuffer.wrap(mBufForReader);
        }
        else if (mBufForReader.length < aLength)
        {
            mBufForReader = new char[resize(aLength)];
            mWrapBuf = CharBuffer.wrap(mBufForReader);
        }
        mWrapBuf.clear();
    }
    
    int maxBytesForChars(int aCharLength)
    {
        return aCharLength * (int)Math.ceil(mEncoder.maxBytesPerChar());
    }
    
    SessionEnv getEnv()
    {
        return mEnv;
    }
    
    String getCharsetName()
    {
        return mEncoder.charset().name();
    }

    public boolean isLittleEndian()
    {
        return mIsLittleEndian;
    }
    
    public void writeColumn(CmChannel aChannel, int aColIndex) throws SQLException
    {
        int sLen = aChannel.readVarLen();
        int sRemain = prepareWriteColumn(aColIndex, sLen);
        aChannel.readVariableBody(mCurInsRowBuf.mRowBuf, sLen - sRemain);
        while (sRemain > 0)
        {
            sLen = sRemain;
            sRemain = prepareWriteColumnNextExt(aColIndex, sLen);
            aChannel.readVariableBody(mCurInsRowBuf.mRowBuf, sLen - sRemain);            
        }
    }
    
    public void writeColumn(CmChannel aChannel, int aColIndex, int aMaxFieldSize) throws SQLException
    {
        int sSkip = 0;
        int sLen = aChannel.readVarLen();
        
        if (sLen > aMaxFieldSize)
        {
            sSkip = sLen - aMaxFieldSize;
            sLen = aMaxFieldSize;            
        }

        int sRemain = prepareWriteColumn(aColIndex, sLen);
        aChannel.readVariableBody(mCurInsRowBuf.mRowBuf, sLen - sRemain);
        while (sRemain > 0)
        {
            sLen = sRemain;
            sRemain = prepareWriteColumnNextExt(aColIndex, sLen);
            aChannel.readVariableBody(mCurInsRowBuf.mRowBuf, sLen - sRemain);            
        }
        
        if (sSkip > 0)
        {
            aChannel.skip(sSkip);
        }
    }

    public void writeRowStatus(byte aStatus)
    {
        if (mCurInsCol.mRowStatus != null)
        {
            mCurInsCol.mRowStatus[mInsRowPos] = aStatus;
        }
    }
    
    public byte readRowStatus()
    {
        if (mCurFetCol.mRowStatus != null)
        {
            return mCurFetCol.mRowStatus[mFetRowPos];
        }
        return Protocol.FETCH_ROW_STATUS_NA;
    }
    
    public void readColumn(CmChannel aChannel, int aColIndex) throws SQLException
    {
        int[] sExtInfo = prepareReadColumn(aColIndex);
        if (sExtInfo == null)
        {
            aChannel.writeVariable(mCurFetRowBuf.mRowBuf);
        }
        else
        {
            aChannel.writeVarLen(mCurFetCol.mInfo[mFetRowPos][aColIndex][OPTION_EXT_TOTAL_LEN]);
            aChannel.writeVariableBody(mCurFetRowBuf.mRowBuf);
            while (sExtInfo != null)
            {
                sExtInfo = prepareReadColumnNextExt(sExtInfo);
                aChannel.writeVariableBody(mCurFetRowBuf.mRowBuf);
            }
        }
    }

    public boolean isNull(int aColIndex)
    {
        return mCurFetCol.mInfo[mFetRowPos][aColIndex][LENGTH] == 0;
    }
    
    public byte readByte(int aColIndex) throws SQLException
    {
        prepareReadColumn(aColIndex);
        return mCurFetRowBuf.mRowBuf.get();
    }
    
    public short readShort(int aColIndex) throws SQLException
    {
        prepareReadColumn(aColIndex);
        return mCurFetRowBuf.mRowBuf.getShort();
    }
    
    public int readInt(int aColIndex) throws SQLException
    {
        prepareReadColumn(aColIndex);
        return mCurFetRowBuf.mRowBuf.getInt();
    }
    
    public long readLong(int aColIndex) throws SQLException
    {
        prepareReadColumn(aColIndex);
        return mCurFetRowBuf.mRowBuf.getLong();
    }
    
    public float readFloat(int aColIndex) throws SQLException
    {
        prepareReadColumn(aColIndex);
        return mCurFetRowBuf.mRowBuf.getFloat();
    }
    
    public double readDouble(int aColIndex) throws SQLException
    {
        prepareReadColumn(aColIndex);
        return mCurFetRowBuf.mRowBuf.getDouble();
    }
    
    public byte[] readBytes(int aColIndex) throws SQLException
    {
        byte[] sResult = new byte[mCurFetCol.mInfo[mFetRowPos][aColIndex][LENGTH]];
        int[] sExtInfo = prepareReadColumn(aColIndex);
        int sOffset = 0;
        int sLen = mCurFetRowBuf.mRowBuf.remaining();
        if (sLen > 0)
        {
            mCurFetRowBuf.mRowBuf.get(sResult, sOffset, sLen);
            sOffset += sLen;
            while (sExtInfo != null)
            {
                sExtInfo = prepareReadColumnNextExt(sExtInfo);
                sLen = mCurFetRowBuf.mRowBuf.remaining();
                mCurFetRowBuf.mRowBuf.get(sResult, sOffset, sLen);
                sOffset += sLen;
            }
        }
        return sResult;
    }
    
    public void readBytes(int aColIndex, byte[] aBuf) throws SQLException
    {
        int[] sExtInfo = prepareReadColumn(aColIndex);
        int sOffset = 0;
        int sLen = mCurFetRowBuf.mRowBuf.remaining();
        if (sLen > 0)
        {
            if (sLen > aBuf.length)
            {                
                ErrorMgr.raiseRuntimeError("Invalid call of readBytes(int, byte[])");
            }
            mCurFetRowBuf.mRowBuf.get(aBuf, sOffset, sLen);
            sOffset += sLen;
            while (sExtInfo != null)
            {
                sExtInfo = prepareReadColumnNextExt(sExtInfo);
                sLen = mCurFetRowBuf.mRowBuf.remaining();
                if (sOffset + sLen > aBuf.length)
                {
                    ErrorMgr.raiseRuntimeError("Invalid call of readBytes(int, byte[])");
                }
                mCurFetRowBuf.mRowBuf.get(aBuf, sOffset, sLen);
                sOffset += sLen;
            }
        }
    }

    public void readBytes(int aColIndex, ByteBuffer aBuf) throws SQLException
    {
        int[] sExtInfo = prepareReadColumn(aColIndex);
        int sLen = mCurFetRowBuf.mRowBuf.remaining();
        if (sLen > 0)
        {
            if (sLen > aBuf.remaining())
            {
                ErrorMgr.raiseRuntimeError("Invalid call of readBytes(int, ByteBuffer)");
            }
            aBuf.put(mCurFetRowBuf.mRowBuf);
            while (sExtInfo != null)
            {
                sExtInfo = prepareReadColumnNextExt(sExtInfo);
                sLen = mCurFetRowBuf.mRowBuf.remaining();
                if (sLen > aBuf.remaining())
                {
                    ErrorMgr.raiseRuntimeError("Invalid call of readBytes(int, ByteBuffer)");
                }
                aBuf.put(mCurFetRowBuf.mRowBuf);
            }
        }
    }
    
    public String readString(int aColIndex) throws SQLException
    {
        boolean sRemains = false;
        verifyBufferForReader(getReadSize(aColIndex));
        mDecoder.reset();
        
        int[] sExtInfo = prepareReadColumn(aColIndex);
        CoderResult sCoderResult = mDecoder.decode(mCurFetRowBuf.mRowBuf, mWrapBuf, sExtInfo == null);
        if (sCoderResult.isError())
        {
            ErrorMgr.raise(ErrorMgr.DECODING_ERROR, sCoderResult.toString());
        }
        
        if (mCurFetRowBuf.mRowBuf.remaining() > 0)
        {
            /* 짜투리가 남았다. mUnderflowBuf를 이용해 짜투리를 decode해야 한다. */
            mUnderflowBuf.clear();
            mUnderflowBuf.put(mCurFetRowBuf.mRowBuf);
            sRemains = true;
        }

        while (sExtInfo != null)
        {
            sExtInfo = prepareReadColumnNextExt(sExtInfo);
            
            if (sRemains)
            {
                int sSupplement = mUnderflowBuf.remaining();
                int sOrgLimit = mCurFetRowBuf.mRowBuf.limit();
                mCurFetRowBuf.mRowBuf.limit(mCurFetRowBuf.mRowBuf.position() + sSupplement);
                mUnderflowBuf.put(mCurFetRowBuf.mRowBuf);
                mCurFetRowBuf.mRowBuf.limit(sOrgLimit);
                mUnderflowBuf.flip();
                sCoderResult = mDecoder.decode(mUnderflowBuf, mWrapBuf, false);
                if (sCoderResult.isError())
                {
                    ErrorMgr.raise(ErrorMgr.DECODING_ERROR, sCoderResult.toString());
                }
                
                // sAgainRemain는 짜투리를 decode하고 남은 바이트이다.
                // mCurFetRowBuf.mRowBuf로 되돌려야 하는데, 애초 mCurFetRowBuf.mRowBuf로부터 가져온 것(sSupplement)보다
                // 같거나 길수는 없다.
                int sAgainRemain = mUnderflowBuf.remaining();
                if (sAgainRemain >= sSupplement)
                {
                    ErrorMgr.raise(ErrorMgr.DECODING_ERROR, "Unexpected decoding error");
                }
                int sRightPos = mCurFetRowBuf.mRowBuf.position() - sAgainRemain;
                mCurFetRowBuf.mRowBuf.position(sRightPos);
                mCurFetRowBuf.mRowBuf.put(mUnderflowBuf);
                mCurFetRowBuf.mRowBuf.position(sRightPos);
                sRemains = false;
            }
            
            sCoderResult = mDecoder.decode(mCurFetRowBuf.mRowBuf, mWrapBuf, sExtInfo == null);
            if (sCoderResult.isError())
            {
                ErrorMgr.raise(ErrorMgr.DECODING_ERROR, sCoderResult.toString());
            }

            if (mCurFetRowBuf.mRowBuf.remaining() > 0)
            {
                /* 짜투리가 남았다. mUnderflowBuf를 이용해 짜투리를 decode해야 한다. */
                mUnderflowBuf.clear();
                mUnderflowBuf.put(mCurFetRowBuf.mRowBuf);
                sRemains = true;
            }
        }
        mDecoder.flush(mWrapBuf);
        mWrapBuf.flip();
        return mWrapBuf.toString();
    }

    public int getReadSize(int aColIndex) throws SQLException
    {
        return mCurFetCol.mInfo[mFetRowPos][aColIndex][LENGTH];
    }
    
    public void writeNull(int aColIndex) throws SQLException
    {
        prepareWriteColumn(aColIndex, 0);
    }
    
    public void writeByte(int aColIndex, byte aValue) throws SQLException
    {
        prepareWriteColumn(aColIndex, 1);
        mCurInsRowBuf.mRowBuf.put(aValue);
    }
    
    public void writeShort(int aColIndex, short aValue) throws SQLException
    {
        prepareWriteColumn(aColIndex, 2);
        mCurInsRowBuf.mRowBuf.putShort(aValue);
    }
    
    public void writeInt(int aColIndex, int aValue) throws SQLException
    {
        prepareWriteColumn(aColIndex, 4);
        mCurInsRowBuf.mRowBuf.putInt(aValue);
    }
    
    public void writeLong(int aColIndex, long aValue) throws SQLException
    {
        prepareWriteColumn(aColIndex, 8);
        mCurInsRowBuf.mRowBuf.putLong(aValue);
    }
    
    public void writeFloat(int aColIndex, float aValue) throws SQLException
    {
        prepareWriteColumn(aColIndex, 4);
        mCurInsRowBuf.mRowBuf.putFloat(aValue);
    }
    
    public void writeDouble(int aColIndex, double aValue) throws SQLException
    {
        prepareWriteColumn(aColIndex, 8);
        mCurInsRowBuf.mRowBuf.putDouble(aValue);
    }

    public void writeBytes(int aColIndex, ByteBuffer aValue) throws SQLException
    {
        int sRemain = prepareWriteColumn(aColIndex, aValue.remaining());
        mCurInsRowBuf.mRowBuf.put(aValue);
        while (sRemain > 0)
        {
            sRemain = prepareWriteColumnNextExt(aColIndex, aValue.remaining());
            mCurInsRowBuf.mRowBuf.put(aValue);
        }
    }

    public void writeBytes(int aColIndex, byte[] aValue, int aLength) throws SQLException
    {
        if (aLength > aValue.length)
        {
            aLength = aValue.length;
        }
        int sRemain = prepareWriteColumn(aColIndex, aLength);
        int sOffset = 0;
        int sWritingLen = aLength - sRemain;
        mCurInsRowBuf.mRowBuf.put(aValue, sOffset, sWritingLen);
        sOffset += sWritingLen;
        while (sRemain > 0)
        {
            sWritingLen = sRemain;
            sRemain = prepareWriteColumnNextExt(aColIndex, sWritingLen);
            sWritingLen -= sRemain;
            mCurInsRowBuf.mRowBuf.put(aValue, sOffset, sWritingLen);
            sOffset += sWritingLen;            
        }
    }
    
    public void writeBytes(int aColIndex, InputStream aValue, int aLength) throws SQLException
    {
        int sRead = 0;
        
        verifyBufferForStream(aLength);
        
        try
        {
            sRead = aValue.read(mBufForStream);
        }
        catch (IOException sException)
        {
            ErrorMgr.raise(ErrorMgr.STREAM_ERROR, sException.getMessage());
        }

        if (sRead == STREAM_READ_BUFFER_SIZE)
        {
            try
            {
                if (aValue.read() >= 0)
                {
                    ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "Bound data is too long");
                }
            }
            catch (IOException sException)
            {
                // 더이상 읽을 게 없으므로 정상이다.
            }
        }
        writeBytes(aColIndex, mBufForStream, sRead);
    }
    
    public void writeBytes(int aColIndex, Reader aValue, int aLength) throws SQLException
    {
        int sRead = 0;
        
        verifyBufferForReader(aLength);

        try
        {
            sRead = aValue.read(mBufForReader);
        }
        catch (IOException sException)
        {
            ErrorMgr.raise(ErrorMgr.STREAM_ERROR, sException.getMessage());
        }
        
        mWrapBuf.limit(sRead);
        writeBytes(aColIndex, mWrapBuf);
    }

    public void writeString(int aColIndex, String aValue) throws SQLException
    {
        try
        {
            byte[] sValue = aValue.getBytes(getCharsetName());
            if (sValue.length > Column.LONG_TYPE_MAX_ALLOC_SIZE)
            {
                ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "Bound data is too long");
            }
            writeBytes(aColIndex, sValue, sValue.length);
        }
        catch (UnsupportedEncodingException sException)
        {
            ErrorMgr.raise(ErrorMgr.ENCODING_ERROR, sException.getMessage());
        }
    }
        
    private void writeBytes(int aColIndex, CharBuffer aValue) throws SQLException
    {
        mEncoder.reset();

        int sActualLength = 0;
        int sLength = maxBytesForChars(aValue.remaining());
        
        prepareWriteColumn(aColIndex, sLength);
        int sBeforePos = mCurInsRowBuf.mRowBuf.position();
        CoderResult sCoderResult = mEncoder.encode(aValue, mCurInsRowBuf.mRowBuf, true);
        if (sCoderResult.isError())
        {
            ErrorMgr.raise(ErrorMgr.ENCODING_ERROR, sCoderResult.toString());
        }
        sActualLength = mCurInsRowBuf.mRowBuf.position() - sBeforePos;
        mCurWritingColInfo[LENGTH] = sActualLength; // 실제 써진 길이로 보정한다.
        
        while (sCoderResult.isOverflow())
        {
            prepareWriteColumnNextExt(aColIndex, maxBytesForChars(aValue.remaining()));
            sBeforePos = mCurInsRowBuf.mRowBuf.position();
            sCoderResult = mEncoder.encode(aValue, mCurInsRowBuf.mRowBuf, true);
            if (sCoderResult.isError())
            {
                ErrorMgr.raise(ErrorMgr.ENCODING_ERROR, sCoderResult.toString());
            }
            mCurWritingColInfo[LENGTH] = mCurInsRowBuf.mRowBuf.position() - sBeforePos; // 실제 써진 길이로 보정한다.
            sActualLength += mCurWritingColInfo[LENGTH];
            
            if (sActualLength > Column.LONG_TYPE_MAX_ALLOC_SIZE)
            {
                ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "Bound data is too long");
            }
        }

        // 위에서 prepareWriteColumn(aColIndex, sLength)를 할 때
        // sLength만큼 쓰겠다고 했으나 실제 encoding해보니 쓴 바이트의 총 길이가
        // sLength보다 작을 수 있는데 실제 써진 길이(sActualLength)를 다시 세팅한다.
        mCurInsCol.mInfo[mInsRowPos][aColIndex][LENGTH] = sActualLength;
    }
    
    public void initToInsert()
    {
        mAccumulatedInsertedCount += mInsertedRowCount;
        mCurInsCol = mColInfoChunkList;
        mCurInsRowBuf = mRowBufChunkList;
        mCurInsRowBuf.mRowBuf.clear();
        mInsRowPos = 0;
        mInsertedRowCount = 0;
        beforeFirst();
    }
    
    public void nextForInsert()
    {
        mInsRowPos++;
        mInsertedRowCount++;
    }
    
    public int getCount()
    {
        return mInsertedRowCount;
    }

    public int getCurrentIdx()
    {
        // idx는 0-base이다.
        return mCurFetCol.mBaseRowIdx + mFetRowPos;
    }
    
    public long getAccumulatedIdx()
    {
        return mAccumulatedInsertedCount + mCurFetCol.mBaseRowIdx + mFetRowPos;
    }
    
    public void beforeFirst()
    {        
        mCurFetCol = mColInfoChunkList;
        mFetRowPos = -1;
    }
    
    public void afterLast()
    {
        mCurFetCol = mCurInsCol;
        mFetRowPos = mInsRowPos;
        mCurFetRowBuf = mCurInsRowBuf;
    }
    
    public boolean previous()
    {
        if (mFetRowPos == 0)
        {
            if (mCurFetCol.mPrev == null)
            {
                mFetRowPos = -1;
                return false;
            }
            mCurFetCol = mCurFetCol.mPrev;
            mFetRowPos = mCurFetCol.mInfo.length - 1;
            return true;
        }
        else if (mFetRowPos < 0)
        {
            return false;
        }
        mFetRowPos--;
        return true;
    }

    public boolean next()
    {
        while (true)
        {
            if (mCurFetCol == mCurInsCol)
            {
                if (mFetRowPos + 1 == mInsRowPos)
                {
                    mFetRowPos = mInsRowPos;
                    return false;
                }
                else if (mFetRowPos == mInsRowPos)
                {
                    return false;
                }
                mFetRowPos++;
                break;
            }
            else
            {
                if (mFetRowPos + 1 == mCurFetCol.mInfo.length)
                {
                    mCurFetCol = mCurFetCol.mNext;
                    mFetRowPos = -1;
                    continue;
                }
                else
                {
                    mFetRowPos++;
                    break;
                }
            }
        }
        return true;
    }
    
    public boolean relative(int aPos)
    {
        return absolute(getCurrentIdx() + aPos); 
    }
    
    public boolean absolute(int aPos)
    {
        // [주의] ResultSet과는 다르게 aPos는 0-base이다.
        if (aPos >= getCount())
        {
            afterLast();
            return false;
        }
        else if (aPos < 0)
        {
            beforeFirst();
            return false;
        }
        while (aPos < mCurFetCol.mBaseRowIdx)
        {
            mCurFetCol = mCurFetCol.mPrev;
        }
        while (aPos >= mCurFetCol.mBaseRowIdx + mCurFetCol.mInfo.length)
        {
            mCurFetCol = mCurFetCol.mNext;
        }
        mFetRowPos = aPos - mCurFetCol.mBaseRowIdx;
        return true;
    }
    
    public boolean isAfterLast()
    {
        return mCurFetCol == mCurInsCol && mFetRowPos == mInsRowPos;
    }
    
    public boolean isBeforeFirst()
    {
        return mCurFetCol == mColInfoChunkList && mFetRowPos == -1;
    }
    
    public boolean isLast()
    {
        return mCurFetCol == mCurInsCol && mFetRowPos == mInsRowPos - 1;
    }
    
    public boolean isFirst()
    {
        return mCurFetCol == mColInfoChunkList && mFetRowPos == 0;
    }
}
