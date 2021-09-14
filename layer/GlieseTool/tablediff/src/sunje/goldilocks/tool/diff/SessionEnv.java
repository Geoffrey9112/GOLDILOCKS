package sunje.goldilocks.tool.diff;

import java.nio.ByteOrder;
import java.nio.charset.Charset;

public class SessionEnv
{
    private Charset mCharset;
    private ByteOrder mByteOrder;
    
    SessionEnv(Charset aCharset, ByteOrder aByteOrder)
    {
        mCharset = aCharset;
        mByteOrder = aByteOrder;
    }
    
    public Charset getCharset()
    {
        return mCharset;
    }
    
    public ByteOrder getByteOrder()
    {
        return mByteOrder;
    }
}
