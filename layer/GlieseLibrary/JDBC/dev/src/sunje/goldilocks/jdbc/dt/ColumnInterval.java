package sunje.goldilocks.jdbc.dt;

import java.io.InputStream;
import java.io.Reader;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.RowIdBase;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public abstract class ColumnInterval extends ColumnNonDateTime
{
    protected byte[] mValue;
    protected ByteBuffer mValueBuffer;
    
    ColumnInterval(CodeColumn aCode)
    {
        super(aCode);
        
        mValue = new byte[getValueSize()];
        mValueBuffer = ByteBuffer.wrap(mValue);
    }
    
    protected void setColumnSpec(int aId,
                                 long aArgNum1,
                                 long aArgNum2,
                                 byte aLenUnit,
                                 byte aIntervalInd,
                                 byte aBindType,
                                 RowCache aCache)
    {
        super.setColumnSpec(aId, aArgNum1, aArgNum2, aLenUnit, aIntervalInd, aBindType, aCache);
        if (mCache.isLittleEndian())
        {
            mValueBuffer.order(ByteOrder.LITTLE_ENDIAN);
        }
        else
        {
            mValueBuffer.order(ByteOrder.BIG_ENDIAN);
        }
    }
    
    protected abstract int getValueSize();
    
    public long getArgNum1()
    {
        return mPrecision;
    }

    public long getArgNum2()
    {
        // 실제로 scale이 아니라 fractional precision이다.
        return mScale;
    }

    protected void setArgNum1(long aArgNum)
    {
        mPrecision = (int)aArgNum;        
    }

    protected void setArgNum2(long aArgNum)
    {
        mScale = (int)aArgNum;
    }
    
    protected void setBooleanImpl(boolean aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "boolean", mCode.getTypeName());
    }

    protected void setBytesImpl(byte[] aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "byte[]", mCode.getTypeName());
    }
    
    protected void setBinaryStreamImpl(InputStream aValue, long aLength) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "InputStream", mCode.getTypeName());        
    }

    protected void setCharacterStreamImpl(Reader aValue, long aLength) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "Reader", mCode.getTypeName());
    }

    protected void setRowIdImpl(RowIdBase aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "RowId", mCode.getTypeName());
    }

    protected boolean getBooleanImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "boolean");
        return false;
    }

    protected Reader getCharacterStreamImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "Reader");
        return null;
    }

    protected RowIdBase getRowIdImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "RowId");
        return null;
    }
}
