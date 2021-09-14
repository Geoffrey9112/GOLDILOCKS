package sunje.goldilocks.tool.diff;

import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.sql.SQLException;

public class FileOutByteBuffer
{
    private FileOutputStream mOut;    
    private byte[] mBufferArray = new byte[32768];
    private ByteBuffer mBuf;

    public FileOutByteBuffer(String aFileName) throws SQLException
    {
        mBuf = ByteBuffer.wrap(mBufferArray);
        try
        {
            mOut = new FileOutputStream(aFileName);
        }
        catch (IOException sException)
        {
            throw new SQLException("error on creating file: " + sException.getMessage());
        }
    }

    private void check(int aLen) throws SQLException
    {
        if (mBuf.remaining() < aLen)
        {
            int sDataLen = mBuf.position();
            mBuf.clear();
            try
            {
                mOut.write(mBufferArray, 0, sDataLen);
            }
            catch (IOException sException)
            {
                throw new SQLException("error on writing file: " + sException.getMessage());
            }            
        }
    }
    
    public void writeByte(byte aByte) throws SQLException
    {
        check(1);
        mBuf.put(aByte);
    }
    
    public void writeShort(short aShort) throws SQLException
    {
        check(2);
        mBuf.putShort(aShort);
    }
    
    public void writeInt(int aInt) throws SQLException
    {
        check(4);
        mBuf.putInt(aInt);
    }
    
    public void writeLong(long aLong) throws SQLException
    {
        check(8);
        mBuf.putLong(aLong);
    }
    
    public void writeString(String aString) throws SQLException
    {
        byte[] sBytes = aString.getBytes();
        check(2+sBytes.length);
        mBuf.putShort((short)sBytes.length);
        mBuf.put(sBytes);
    }
    
    public void writeBytes(byte[] aTarget, int aOffset, int aLength) throws SQLException
    {
        check(aLength);
        mBuf.put(aTarget, aOffset, aLength);
    }
    
    public void flush() throws SQLException
    {
        if (mBuf.position() > 0)
        {
            try
            {
                mOut.write(mBufferArray, 0, mBuf.position());
            }
            catch (IOException sException)
            {
                throw new SQLException("error on writing file: " + sException.getMessage());
            }            
        }
        try
        {
            mOut.flush();
        }
        catch (IOException sException)
        {
            throw new SQLException("error on flush: " + sException.getMessage());
        }
    }
    
    public void close() throws SQLException
    {
        try
        {
            mOut.close();
        }
        catch (IOException sException)
        {
            throw new SQLException("error on flush: " + sException.getMessage());
        }
    }
}
