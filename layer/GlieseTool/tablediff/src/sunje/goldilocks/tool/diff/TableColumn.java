package sunje.goldilocks.tool.diff;

import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CoderResult;
import java.sql.SQLException;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import sunje.goldilocks.jdbc.GoldilocksConnection;
import sunje.goldilocks.jdbc.Version6Specific;
import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.core4.VersionSpecific;
import sunje.goldilocks.jdbc.dt.CodeColumn;
import sunje.goldilocks.jdbc.dt.Column;
import sunje.goldilocks.jdbc.dt.ColumnDate;
import sunje.goldilocks.jdbc.dt.ColumnNumber;
import sunje.goldilocks.jdbc.dt.ColumnTime;
import sunje.goldilocks.jdbc.dt.ColumnTimestamp;
import sunje.goldilocks.jdbc.util.Utils;
import sunje.goldilocks.sql.GoldilocksInterval;

public class TableColumn
{
    class Comparer
    {
        int compare(TableColumn aTarget)
        {
            int sResult = 0;
            int sLen = mLength;
            if (mLength > aTarget.mLength)
            {
                sLen = aTarget.mLength;
                sResult = 1;
            }
            else if (mLength < aTarget.mLength)
            {
                sLen = mLength;
                sResult = -1;
            }

            for (int i=0; i<sLen; i++)
            {
                if ((mData[mOffset+i] & 0xFF) > (aTarget.mData[mOffset+i] & 0xFF))
                {
                    return 1;
                }
                else if ((mData[mOffset+i] & 0xFF) < (aTarget.mData[mOffset+i] & 0xFF))
                {
                    return -1;
                }
            }
            return sResult;
        }
    }

    class NativeSmallintComparer extends Comparer
    {
        private ByteBuffer mBuf;

        int compare(TableColumn aTarget)
        {
            if (mBuf == null)
            {
                mBuf = ByteBuffer.allocate(2);
                mBuf.order(mEnv.getByteOrder());
            }
            mBuf.clear();
            mBuf.put(mData, mOffset, 2);
            mBuf.rewind();
            short sSource = mBuf.getShort();
            mBuf.clear();
            mBuf.put(aTarget.mData, aTarget.mOffset, 2);
            mBuf.rewind();
            short sTarget = mBuf.getShort();

            int sResult = 0;
            if (sSource > sTarget)
            {
                sResult = 1;
            }
            else if (sSource < sTarget)
            {
                sResult = -1;
            }
            return sResult;
        }
    }

    class NativeIntComparer extends Comparer
    {
        private ByteBuffer mBuf;

        int compare(TableColumn aTarget)
        {
            if (mBuf == null)
            {
                mBuf = ByteBuffer.allocate(4);
                mBuf.order(mEnv.getByteOrder());
            }
            mBuf.clear();
            mBuf.put(mData, mOffset, 4);
            mBuf.rewind();
            int sSource = mBuf.getInt();
            mBuf.clear();
            mBuf.put(aTarget.mData, aTarget.mOffset, 4);
            mBuf.rewind();
            int sTarget = mBuf.getInt();

            int sResult = 0;
            if (sSource > sTarget)
            {
                sResult = 1;
            }
            else if (sSource < sTarget)
            {
                sResult = -1;
            }
            return sResult;
        }
    }

    class NativeBigintComparer extends Comparer
    {
        private ByteBuffer mBuf;

        int compare(TableColumn aTarget)
        {
            if (mBuf == null)
            {
                mBuf = ByteBuffer.allocate(8);
                mBuf.order(mEnv.getByteOrder());
            }
            mBuf.clear();
            mBuf.put(mData, mOffset, 8);
            mBuf.rewind();
            long sSource = mBuf.getLong();
            mBuf.clear();
            mBuf.put(aTarget.mData, aTarget.mOffset, 8);
            mBuf.rewind();
            long sTarget = mBuf.getLong();

            int sResult = 0;
            if (sSource > sTarget)
            {
                sResult = 1;
            }
            else if (sSource < sTarget)
            {
                sResult = -1;
            }
            return sResult;
        }
    }

    class RealComparer extends Comparer
    {
        private ByteBuffer mBuf;

        int compare(TableColumn aTarget)
        {
            if (mBuf == null)
            {
                mBuf = ByteBuffer.allocate(4);
                mBuf.order(mEnv.getByteOrder());
            }
            mBuf.clear();
            mBuf.put(mData, mOffset, 4);
            mBuf.rewind();
            float sSource = mBuf.getFloat();
            mBuf.clear();
            mBuf.put(aTarget.mData, aTarget.mOffset, 4);
            mBuf.rewind();
            float sTarget = mBuf.getFloat();

            int sResult = 0;
            if (sSource > sTarget)
            {
                sResult = 1;
            }
            else if (sSource < sTarget)
            {
                sResult = -1;
            }
            return sResult;
        }
    }

    class DoubleComparer extends Comparer
    {
        private ByteBuffer mBuf;
    
        int compare(TableColumn aTarget)
        {
            if (mBuf == null)
            {
                mBuf = ByteBuffer.allocate(8);
                mBuf.order(mEnv.getByteOrder());
            }
            mBuf.clear();
            mBuf.put(mData, mOffset, 8);
            mBuf.rewind();
            double sSource = mBuf.getDouble();
            mBuf.clear();
            mBuf.put(aTarget.mData, aTarget.mOffset, 8);
            mBuf.rewind();
            double sTarget = mBuf.getDouble();

            int sResult = 0;
            if (sSource > sTarget)
            {
                sResult = 1;
            }
            else if (sSource < sTarget)
            {
                sResult = -1;
            }
            return sResult;
        }
    }

    class NumberComparer extends Comparer
    {
        private static final int NUMERIC_SIGN_ZERO = 0x80;
        int compare(TableColumn aTarget)
        {
            int sResult = 0;
            int sLen = mLength;
            if (mLength > aTarget.mLength)
            {
                sLen = aTarget.mLength;
                if ((mData[mOffset] & 0xFF) < NUMERIC_SIGN_ZERO)
                {
                    // 둘다 음수인 경우엔 길이가 긴쪽이 더 작은 값이다.
                    sResult = -1;
                }
                else
                {
                    sResult = 1;
                }
            }
            else if (mLength < aTarget.mLength)
            {
                sLen = mLength;
                if ((mData[mOffset] & 0xFF) < NUMERIC_SIGN_ZERO)
                {
                    // 둘다 음수인 경우엔 길이가 긴쪽이 더 작은 값이다.
                    sResult = 1;
                }
                else
                {
                    sResult = -1;
                }
            }

            for (int i=0; i<sLen; i++)
            {
                if ((mData[mOffset+i] & 0xFF) > (aTarget.mData[mOffset+i] & 0xFF))
                {
                    return 1;
                }
                else if ((mData[mOffset+i] & 0xFF) < (aTarget.mData[mOffset+i] & 0xFF))
                {
                    return -1;
                }
            }
            return sResult;
        }
    }

    class DayTimeIntervalComparer extends Comparer
    {
        private ByteBuffer mBuf;

        int compare(TableColumn aTarget)
        {
            if (mBuf == null)
            {
                mBuf = ByteBuffer.allocate(12);
                mBuf.order(mEnv.getByteOrder());
            }
            mBuf.clear();
            mBuf.put(mData, mOffset, 12);
            mBuf.rewind();
            long sSourceTime = mBuf.getLong();
            int sSourceDay = mBuf.getInt();
            mBuf.clear();
            mBuf.put(aTarget.mData, aTarget.mOffset, 12);
            mBuf.rewind();
            long sTargetTime = mBuf.getLong();
            int sTargetDay = mBuf.getInt();

            int sResult = 0;
            if (sSourceDay > sTargetDay)
            {
                sResult = 1;
            }
            else if (sSourceDay < sTargetDay)
            {
                sResult = -1;
            }
            else
            {
                if (sSourceTime > sTargetTime)
                {
                    sResult = 1;
                }
                else if (sSourceTime < sTargetTime)
                {
                    sResult = -1;
                }
            }
            return sResult;
        }
    }

    int mIndex;
    String mName;
    byte mType;
    long mPrecision;
    long mScale;
    byte mLenUnit;
    byte mIntervalInd;
    
    byte[] mData;
    int mOffset;
    int mLength;
    int mMaxLength;
    CharsetDecoder mDecoder;
    CharBuffer mCharBuf;
    SessionEnv mEnv;
    boolean mRowId = false;
    Comparer mComp = null;

    static
    {
        Version6Specific.initialize();
    }
    
    public TableColumn(SessionEnv aEnv, int aColIndex, String aName, byte aType, long aPrecision, long aScale, byte aLenUnit, byte aIntervalInd)
    {
        mEnv = aEnv;
        mIndex = aColIndex;
        mName = aName;
        mType = aType;
        mPrecision = aPrecision;
        mScale = aScale;
        mLenUnit = aLenUnit;
        mIntervalInd = aIntervalInd;
        
        switch (mType)
        {
            case CodeColumn.GOLDILOCKS_DATA_TYPE_BOOLEAN:
                mMaxLength = 1;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_SMALLINT:
                mMaxLength = 2;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER:
                mMaxLength = 4;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_BIGINT:
                mMaxLength = 8;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_REAL:
                mMaxLength = 4;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_DOUBLE:
                mMaxLength = 8;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_FLOAT:
                mMaxLength = 22;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NUMBER:
                mMaxLength = 22;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_DECIMAL:
                mMaxLength = 22;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_CHAR:
                mMaxLength = (int)mPrecision;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_VARCHAR:
                mMaxLength = (int)mPrecision;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_LONGVARCHAR:
                mMaxLength = (int)mPrecision;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_CLOB:
                mMaxLength = (int)mPrecision;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_BINARY:
                mMaxLength = (int)mPrecision;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_VARBINARY:
                mMaxLength = (int)mPrecision;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_LONGVARBINARY:
                mMaxLength = (int)mPrecision;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_BLOB:
                mMaxLength = (int)mPrecision;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_DATE:
                mMaxLength = 8;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_TIME:
                mMaxLength = 8;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_TIMESTAMP:
                mMaxLength = 8;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_TIME_WITH_TIME_ZONE:
                mMaxLength = 12;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                mMaxLength = 12;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH:
                mMaxLength = 8;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND:
                mMaxLength = 16;
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_ROWID:
                mMaxLength = 16;
                break;
        }
    }
    
    public void setToRowId()
    {
        mData = new byte[mMaxLength];
        mRowId = true;
    }
    
    public void setDataBuf(byte[] aData)
    {
        mData = aData;
    }
    
    public void writeType(CmChannel aChannel, long aStatementId, int aIndex, byte aDependency) throws SQLException
    {
        aChannel.writeVarInt(Protocol.PROTOCOL_CODE_BINDTYPE);
        aChannel.writeByte(aDependency);
        aChannel.writeStatementId(aStatementId);
        aChannel.writeVarInt(aIndex);
        aChannel.writeByte(Column.BIND_TYPE_INPUT);
        aChannel.writeByte(mType);
        aChannel.writeVarInt(mPrecision);
        aChannel.writeVarInt(mScale);
        aChannel.writeByte(mLenUnit);
        aChannel.writeByte(mIntervalInd);
    }
    
    public void writeData(CmChannel aChannel) throws SQLException
    {
        aChannel.writeByte(mType);
        aChannel.writeVariable(mData, mOffset, mLength);
    }
    
    public void readData(CmChannel aChannel, int aOffset) throws SQLException
    {
        mOffset = aOffset;
        mLength = aChannel.readVarLen();
        aChannel.readVariableBody(mData, mOffset, mLength);
    }
    
    public int readLengthFrom(FileInByteBuffer aBuf, int aOffset) throws SQLException
    {
        mOffset = aOffset;
        mLength = aBuf.readInt();
        return mLength;
    }
    
    public void readDataFrom(FileInByteBuffer aBuf) throws SQLException
    {
        /*
         * ROWID 컬럼에 대해서만 불려야 한다.
         */
        mOffset = 0;
        mLength = aBuf.readInt();
        aBuf.readBytes(mData, mOffset, mLength);
    }
    
    public void writeLengthTo(FileOutByteBuffer aBuf) throws SQLException
    {
        aBuf.writeInt(mLength);
    }
    
    public void writeDataTo(FileOutByteBuffer aBuf) throws SQLException
    {
        aBuf.writeInt(mLength);
        aBuf.writeBytes(mData, mOffset, mLength);
    }
    
    public void writeCodeTo(FileOutByteBuffer aBuf) throws SQLException
    {
        aBuf.writeInt(mIndex);
        aBuf.writeString(mName);
        aBuf.writeByte(mType);
        aBuf.writeLong(mPrecision);
        aBuf.writeLong(mScale);
        aBuf.writeByte(mLenUnit);
        aBuf.writeByte(mIntervalInd);
    }
    
    public int compare(TableColumn aTarget)
    {
        if (mComp == null)
        {
            switch (mType)
            {
                case CodeColumn.GOLDILOCKS_DATA_TYPE_BOOLEAN:
                    mComp = new Comparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_SMALLINT:
                    mComp = new NativeSmallintComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER:
                    mComp = new NativeIntComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_BIGINT:
                    mComp = new NativeBigintComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_REAL:
                    mComp = new RealComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_DOUBLE:
                    mComp = new DoubleComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_FLOAT:
                    mComp = new NumberComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_NUMBER:
                    mComp = new NumberComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_DECIMAL:
                    mComp = new Comparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_CHAR:
                    mComp = new Comparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_VARCHAR:
                    mComp = new Comparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_LONGVARCHAR:
                    mComp = new Comparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_CLOB:
                    mComp = new Comparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_BINARY:
                    mComp = new Comparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_VARBINARY:
                    mComp = new Comparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_LONGVARBINARY:
                    mComp = new Comparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_BLOB:
                    mComp = new Comparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_DATE:
                    mComp = new NativeBigintComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_TIME:
                    mComp = new NativeBigintComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_TIMESTAMP:
                    mComp = new NativeBigintComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_TIME_WITH_TIME_ZONE:
                    mComp = new NativeBigintComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                    mComp = new NativeBigintComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH:
                    mComp = new NativeIntComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND:
                    mComp = new DayTimeIntervalComparer();
                    break;
                case CodeColumn.GOLDILOCKS_DATA_TYPE_ROWID:
                    mComp = new Comparer();
                    break;
            }
        }
        return mComp.compare(aTarget);
    }
    
    public void copyDataFrom(TableColumn aSource)
    {
        mOffset = aSource.mOffset;
        mLength = aSource.mLength;
        if (mRowId)
        {
            System.arraycopy(aSource.mData, mOffset, mData, mOffset, mLength);
        }
    }
    
    public TableColumn cloneCode()
    {
        TableColumn sCol = new TableColumn(mEnv, mIndex, mName, mType, mPrecision, mScale, mLenUnit, mIntervalInd);
        if (mRowId)
        {
            sCol.setToRowId();
        }
        return sCol;
    }
    
    public String getValue()
    {
        if (mLength == 0)
        {
            return "null";
        }
        String sResult = "";
        ByteBuffer sBuf = ByteBuffer.wrap(mData, mOffset, mLength);
        sBuf.order(mEnv.getByteOrder());
        switch (mType)
        {
            case CodeColumn.GOLDILOCKS_DATA_TYPE_BOOLEAN:
            {
                if (mData[mOffset] == 0)
                {
                    sResult = "FALSE";
                }
                else
                {
                    sResult = "TRUE";
                }
                break;
            }
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_SMALLINT:
                sResult = String.valueOf(sBuf.getShort());
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER:
                sResult = String.valueOf(sBuf.getInt());
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_BIGINT:
                sResult = String.valueOf(sBuf.getLong());
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_REAL:
                sResult = String.valueOf(sBuf.getFloat());
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_DOUBLE:
                sResult = String.valueOf(sBuf.getDouble());
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_FLOAT:
            case CodeColumn.GOLDILOCKS_DATA_TYPE_NUMBER:
            case CodeColumn.GOLDILOCKS_DATA_TYPE_DECIMAL:
                sResult = ColumnNumber.numericToString(sBuf, GoldilocksConnection.GOLDILOCKS_DEFAULT_MAX_TRAILING_ZERO_COUNT, GoldilocksConnection.GOLDILOCKS_DEFAULT_MAX_LEADING_ZERO_COUNT);
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_CHAR:
            case CodeColumn.GOLDILOCKS_DATA_TYPE_VARCHAR:
            case CodeColumn.GOLDILOCKS_DATA_TYPE_LONGVARCHAR:
            {
                if (mDecoder == null)
                {
                    mDecoder = mEnv.getCharset().newDecoder();
                }
                else
                {
                    mDecoder.reset();
                }
                if (mCharBuf == null)
                {
                    mCharBuf = CharBuffer.allocate((int)mPrecision);
                }
                else
                {
                    mCharBuf.clear();
                }
                try
                {
                    CoderResult sCoderResult = mDecoder.decode(sBuf, mCharBuf, true);
                    if (sCoderResult.isError())
                    {
                        sResult = "undecoderable bytes";
                    }
                    else
                    {
                        mDecoder.flush(mCharBuf);
                        mCharBuf.flip();
                        sResult = mCharBuf.toString();
                    }
                }
                catch (Exception sException)
                {
                    sResult = "undecoderable bytes by exception";
                }
                break;
            }
            case CodeColumn.GOLDILOCKS_DATA_TYPE_CLOB:
            {
                sResult = "Clob data";
                break;
            }
            case CodeColumn.GOLDILOCKS_DATA_TYPE_BINARY:
            case CodeColumn.GOLDILOCKS_DATA_TYPE_VARBINARY:
            case CodeColumn.GOLDILOCKS_DATA_TYPE_LONGVARBINARY:
            {
                byte[] x = new byte[mLength];
                System.arraycopy(mData, mOffset, x, 0, mLength);
                sResult = VersionSpecific.SINGLETON.byteToHexString(x, true);
                break;
            }
            case CodeColumn.GOLDILOCKS_DATA_TYPE_BLOB:
            {
                sResult = "Blob data";
                break;
            }
            case CodeColumn.GOLDILOCKS_DATA_TYPE_DATE:
            {
                long sDateValue = sBuf.getLong();
                sResult = ColumnDate.goldilocksTsToJavaDate(sDateValue, Utils.getLocalCalendar()).toString();
                break;
            }
            case CodeColumn.GOLDILOCKS_DATA_TYPE_TIME:
            {
                long sTimeValue = sBuf.getLong();
                sResult = ColumnTime.goldilocksTsToJavaTime(sTimeValue, Utils.getLocalCalendar()).toString();
                break;
            }
            case CodeColumn.GOLDILOCKS_DATA_TYPE_TIMESTAMP:
            {
                long sTimestampValue = sBuf.getLong();
                sResult = ColumnTimestamp.goldilocksTsToJavaTimestamp(sTimestampValue, Utils.getLocalCalendar()).toString();
                mMaxLength = 8;
                break;
            }
            case CodeColumn.GOLDILOCKS_DATA_TYPE_TIME_WITH_TIME_ZONE:
            {
                long sTimeValue = sBuf.getLong();
                int sTimeOffsetSec = -sBuf.getInt();
                Calendar sCal = Utils.getCalendar(sTimeOffsetSec);
                SimpleDateFormat sFormat = new SimpleDateFormat("HH:mm:ss Z");
                sFormat.setTimeZone(sCal.getTimeZone());
                sResult = sFormat.format(ColumnTime.goldilocksTsToJavaTime(sTimeValue, sCal));
                break;
            }
            case CodeColumn.GOLDILOCKS_DATA_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                long sTimeValue = sBuf.getLong();
                int sTimeOffsetSec = -sBuf.getInt();
                Calendar sCal = Utils.getCalendar(sTimeOffsetSec);
                SimpleDateFormat sFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS Z");
                sFormat.setTimeZone(sCal.getTimeZone());
                sResult = sFormat.format(ColumnTime.goldilocksTsToJavaTimestamp(sTimeValue, sCal));
                break;
            }
            case CodeColumn.GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH:
                int sMonth = sBuf.getInt();
                try
                {
                    sResult = GoldilocksInterval.createIntervalYearToMonth((int)mPrecision, sMonth >= 0, sMonth / 12, sMonth % 12).toString();
                }
                catch (SQLException sException)
                {
                    sResult = "Invalid interval value";
                }
                break;
            case CodeColumn.GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                long sTime = sBuf.getLong();
                int sDay = sBuf.getInt();
                try
                {
                    sResult = GoldilocksInterval.createIntervalDayToSecond((int)mPrecision, (int)mScale, sDay >= 0 && sTime >= 0,
                            Math.abs(sDay), Math.abs((int)(sTime / 3600000000l)), Math.abs((int)((sTime % 3600000000l) / 60000000)), Math.abs((int)(sTime % 60000000) / 1000000), Math.abs((int)(sTime % 1000000))).toString();
                }
                catch (SQLException sException)
                {
                    sResult = "Invalid interval value";
                }
                break;
            }
            case CodeColumn.GOLDILOCKS_DATA_TYPE_ROWID:
            {
                byte[] x = new byte[mLength];
                System.arraycopy(mData, mOffset, x, 0, mLength);
                sResult = VersionSpecific.SINGLETON.createConcreteRowId(x).toString();
                break;
            }
            default:
            {
                sResult = "Unknown type data";
                break;
            }
        }
        return sResult;
    }
    
    public void verifyCodeSame(TableColumn aTarget) throws SQLException
    {
        if (mIndex != aTarget.mIndex)
        {
            throw new SQLException("column number is different: " + mName);
        }
        if (!mName.equals(aTarget.mName))
        {
            throw new SQLException("column name is different: " + mName);
        }
        if (mType != aTarget.mType)
        {
            throw new SQLException("column type is different: " + mName);
        }
    }
}
