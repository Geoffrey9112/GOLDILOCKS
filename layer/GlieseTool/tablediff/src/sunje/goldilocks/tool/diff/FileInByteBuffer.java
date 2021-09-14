package sunje.goldilocks.tool.diff;

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.sql.SQLException;

public class FileInByteBuffer
{
    private FileInputStream mIn;
    private byte[] mBufferArray = new byte[32768];
    private ByteBuffer mBuf;
    private byte[] mStringReadBuffer = new byte[1024];

    public FileInByteBuffer(String aFileName) throws SQLException
    {
        mBuf = ByteBuffer.wrap(mBufferArray);
        try
        {
            mIn = new FileInputStream(aFileName);
        }
        catch (IOException sException)
        {
            throw new SQLException("error on opening file: " + sException.getMessage());
        }
        read();
    }

    private void read() throws SQLException
    {
        int sRead = 0;
        try
        {
            sRead = mIn.read(mBufferArray, 0, mBufferArray.length);
        }
        catch (IOException sException)
        {
            throw new SQLException("error on reading file: " + sException.getMessage());
        }
        mBuf.position(0);
        if (sRead > 0)
        {
            mBuf.limit(sRead);
        }
        else
        {
            mBuf.limit(0);
        }
    }
    
    private void check(int aLen) throws SQLException
    {
        if (mBuf.remaining() < aLen)
        {
            int sRemain = mBuf.remaining();
            System.arraycopy(mBufferArray, mBuf.position(), mBufferArray, 0, sRemain);
            while (sRemain < aLen)
            {
                /*
                 * 남은 데이터를 앞쪽으로 당긴다. aLen은 크지 않기 때문에 겹칠일이 없다.
                 */
                mBuf.clear();
                int sRead = 0;
                try
                {
                    sRead = mIn.read(mBufferArray, sRemain, mBufferArray.length - sRemain);
                }
                catch (IOException sException)
                {
                    throw new SQLException("error on reading file: " + sException.getMessage());
                }
                if (sRead <= 0)
                {
                    throw new SQLException("file data is insufficient. need more data: " + aLen);
                }                
                sRemain += sRead;
                mBuf.limit(sRemain);
            }
        }
    }
    
    public boolean remains() throws SQLException
    {
        if (mBuf.remaining() == 0)
        {
            read();
            if (mBuf.remaining() == 0)
            {
                return false;
            }
        }
        return true;
    }
    
    public String readString() throws SQLException
    {
        check(2);
        short sLen = mBuf.getShort();
        check(sLen);
        mBuf.get(mStringReadBuffer, 0, sLen);
        return new String(mStringReadBuffer, 0, sLen);
    }
    
    public byte readByte() throws SQLException
    {
        check(1);
        return mBuf.get();
    }
    
    public short readShort() throws SQLException
    {
        check(2);
        return mBuf.getShort();
    }
    
    public int readInt() throws SQLException
    {
        check(4);
        return mBuf.getInt();
    }
    
    public long readLong() throws SQLException
    {
        check(8);
        return mBuf.getLong();
    }
    
    public void readBytes(byte[] aTarget, int aOffset, int aLength) throws SQLException
    {
        if (aLength < mBuf.remaining())
        {
            mBuf.get(aTarget, aOffset, aLength);
        }
        else
        {
            int sRemain = mBuf.remaining();
            while (aLength > 0)
            {
                mBuf.get(aTarget, aOffset, Math.min(aLength, sRemain));
                aOffset += sRemain;
                aLength -= sRemain;
                check(aLength);
                sRemain = mBuf.remaining();
            }
        }
    }
    
    public void close() throws SQLException
    {
        try
        {
            mIn.close();
        }
        catch (IOException sException)
        {
            throw new SQLException("error on flush: " + sException.getMessage());
        }
    }
}
