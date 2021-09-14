package sunje.goldilocks.jdbc.core4;

import java.sql.ResultSetMetaData;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.cm.SourceDescribeCol;
import sunje.goldilocks.jdbc.dt.CodeColumn;
import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.internal.BaseProtocolSource;
import sunje.goldilocks.jdbc.internal.ErrorHolder;
import sunje.goldilocks.jdbc.util.Logger;

class ColumnDescription extends BaseProtocolSource implements SourceDescribeCol
{
    protected int mNumber;
    private Jdbc4Statement mStatement;
    private CodeColumn mColumn;
    private String mCatalogName;
    private String mSchemaName;
    private String mTableName;
    private String mBaseTableName;
    private String mColLabel;
    private String mLiteralPrefix;
    private String mLiteralSuffix;
    private byte mDBType;
    private short mIntervalCode;
    private short mNumericPrecisionRadix;
    private short mDatetimeIntervalPrecision;
    private long mDisplaySize;
    private long mCharLength;
    private long mOctetLength;
    private int mPrecision;
    private int mScale;
    private boolean mNullable;
    private boolean mAliasUnnamed;
    private boolean mCaseSensitive;
    private boolean mUnsigned;
    private boolean mFixedPrecisionScale;
    private boolean mUpdatable;
    private boolean mAutoUnique;
    private boolean mRowVersion;
    private boolean mAbleLike;

    public ColumnDescription(ErrorHolder aErrorHolder, Jdbc4Statement aStatement, int aNumber)
    {
        super(aErrorHolder);
        mStatement = aStatement;
        mNumber = aNumber;
    }

    public CodeColumn getColumn()
    {
        return mColumn;
    }

    public void addDescribeColProtocol(CmChannel aChannel) throws SQLException
    {
        aChannel.writeProtocolAlone(Protocol.PROTOCOL_DESCRIBE_COL, this);
    }

    public long getStatementId()
    {
        return mStatement.getId();
    }

    public int getColumnNum()
    {
        return mNumber;
    }

    public void setCatalogName(String aCatalogName)
    {
        mCatalogName = aCatalogName;
    }

    public void setSchemaName(String aSchemaName)
    {
        mSchemaName = aSchemaName;
    }

    public void setTableName(String aTableName)
    {
        mTableName = aTableName;
    }

    public void setBaseTableName(String aBaseTableName)
    {
        mBaseTableName = aBaseTableName;
    }

    public void setColAliasName(String aColAliasName)
    {
        /*
         * alias name은 JDBC에서 사용하지 않음
         */
    }

    public void setColLabel(String aColLabel)
    {
        mColLabel = aColLabel;
    }

    public void setColBaseName(String aColBaseName)
    {
        /*
         * base column name도 JDBC에서 사용하지 않음
         * column name == column label임
         */
    }

    public void setLiteralPrefix(String aPrefix)
    {
        mLiteralPrefix = aPrefix;
    }

    public void setLiteralSuffix(String aSuffix)
    {
        mLiteralSuffix = aSuffix;
    }

    public void setDBType(byte aDBType)
    {
        mDBType = aDBType;
    }

    public void setIntervalCode(short aIntervalCode)
    {
        mIntervalCode = aIntervalCode;
        mColumn = CodeColumn.getCodeColumn(mDBType, (byte)mIntervalCode);
    }

    public void setNumericPrecisionRadix(short aNumPrecisionRadix)
    {
        mNumericPrecisionRadix = aNumPrecisionRadix;
    }

    public void setDatetimeIntervalPrecision(short aDatetimeIntervalPrecision)
    {
        mDatetimeIntervalPrecision = aDatetimeIntervalPrecision;
    }

    public void setDisplaySize(long aDisplaySize)
    {
        mDisplaySize = aDisplaySize;
    }

    public void setCharLengthUnit(byte aCharLenUnit)
    {
        /*
         * char length unit은 현재 사용하는 곳이 없다.
         */
    }
    
    public void setCharLength(long aCharLength)
    {
        mCharLength = aCharLength;
    }

    public void setOctetLength(long aOctetLength)
    {
        mOctetLength = aOctetLength;
    }

    public void setPrecision(int aPrecision)
    {
        mPrecision = aPrecision;
    }

    public void setScale(int aScale)
    {
        mScale = aScale;
    }

    public void setNullable(boolean aNullable)
    {
        mNullable = aNullable;
    }

    public void setAliasUnnamed(boolean aAliasUnnamed)
    {
        mAliasUnnamed = aAliasUnnamed;
    }

    public void setCaseSensitive(boolean aCaseSensitive)
    {
        mCaseSensitive = aCaseSensitive;
    }

    public void setUnsigned(boolean aUnsigned)
    {
        mUnsigned = aUnsigned;
    }

    public void setFixedPrecisionScale(boolean aFixedPrecisionScale)
    {
        mFixedPrecisionScale = aFixedPrecisionScale;
    }

    public void setUpdatable(boolean aUpdatable)
    {
        mUpdatable = aUpdatable;
    }

    public void setAutoUnique(boolean aAutoUnique)
    {
        mAutoUnique = aAutoUnique;
    }

    public void setRowVersion(boolean aRowVersion)
    {
        mRowVersion = aRowVersion;
    }

    public void setAbleLike(boolean aAbleLike)
    {
        mAbleLike = aAbleLike;
    }

    public void setLogicalDataTypeName(String aTypeName)
    {
        // nothing to do
    }

    public String getCatalogName()
    {
        return mCatalogName;
    }

    public String getSchemaName()
    {
        return mSchemaName;
    }

    public String getTableName()
    {
        return mTableName;
    }

    public String getBaseTableName()
    {
        return mBaseTableName;
    }

    public String getColLabel()
    {
        return mColLabel;
    }

    public String getLiteralPrefix()
    {
        return mLiteralPrefix;
    }

    public String getLiteralSuffix()
    {
        return mLiteralSuffix;
    }

    public byte getDBType()
    {
        return mDBType;
    }

    public short getIntervalCode()
    {
        return mIntervalCode;
    }

    public short getNumericPrecisionRadix()
    {
        return mNumericPrecisionRadix;
    }

    public short getDatetimeIntervalPrecision()
    {
        return mDatetimeIntervalPrecision;
    }

    public long getDisplaySize()
    {
        return mDisplaySize;
    }

    public long getCharLength()
    {
        return mCharLength;
    }

    public long getOctetLength()
    {
        return mOctetLength;
    }

    public int getPrecision()
    {
        return mPrecision;
    }

    public int getScale()
    {
        return mScale;
    }

    public boolean getNullable()
    {
        return mNullable;
    }

    public boolean getAliasUnnamed()
    {
        return mAliasUnnamed;
    }

    public boolean getCaseSensitive()
    {
        return mCaseSensitive;
    }

    public boolean getUnsigned()
    {
        return mUnsigned;
    }

    public boolean getFixedPrecisionScale()
    {
        return mFixedPrecisionScale;
    }

    public boolean getUpdatable()
    {
        return mUpdatable;
    }

    public boolean getAutoUnique()
    {
        return mAutoUnique;
    }

    public boolean getRowVersion()
    {
        return mRowVersion;
    }

    public boolean getAbleLike()
    {
        return mAbleLike;
    }

    public String getDataTypeName()
    {
        return mColumn.getTypeName();
    }
}

public class Jdbc4ResultSetMetaData implements ResultSetMetaData
{
    private ColumnDescription[] mColumnDescs;
    private Logger mLogger;
    
    Jdbc4ResultSetMetaData(ColumnDescription[] aColumnDescs, Logger aLogger)
    {
        mColumnDescs = aColumnDescs;
        mLogger = aLogger;
    }
    
    private void checkColumnIndex(int aColNum) throws SQLException
    {
        if (aColNum <= 0 || aColNum > mColumnDescs.length)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_ARGUMENT, "column index: " + aColNum);
        }
    }
    
    public boolean isWrapperFor(Class<?> aClass) throws SQLException
    {
        mLogger.logTrace();
        return aClass.isInstance(this);
    }

    public <T> T unwrap(Class<T> aClass) throws SQLException
    {
        mLogger.logTrace();
        if (!aClass.isInstance(this))
        {
            ErrorMgr.raise(ErrorMgr.INVALID_WRAPPER_TYPE, aClass.toString());
        }
        return aClass.cast(this);
    }

    public String getCatalogName(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getCatalogName();
    }

    public String getColumnClassName(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getColumn().getClassName();
    }

    public int getColumnCount() throws SQLException
    {
        mLogger.logTrace();
        return mColumnDescs.length;
    }

    public int getColumnDisplaySize(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return (int)mColumnDescs[aColNum - 1].getDisplaySize();
    }

    public String getColumnLabel(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getColLabel();
    }

    public String getColumnName(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getColLabel();
    }

    public int getColumnType(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getColumn().getRepresentativeSqlType();
    }

    public String getColumnTypeName(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getDataTypeName();
    }

    public int getPrecision(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        int sPrecision = mColumnDescs[aColNum - 1].getPrecision();
        return sPrecision == CodeColumn.DEFAULT_PRECISION_NA ? 0 : sPrecision;
    }

    public int getScale(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        int sScale = mColumnDescs[aColNum - 1].getScale();
        return sScale == CodeColumn.SCALE_NA ? 0 : sScale;
    }

    public String getSchemaName(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getSchemaName();
    }

    public String getTableName(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getTableName();
    }

    public boolean isAutoIncrement(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getAutoUnique();
    }

    public boolean isCaseSensitive(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getCaseSensitive();
    }

    public boolean isCurrency(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        // 지원하지 않음
        return false;
    }

    public boolean isDefinitelyWritable(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        // Goldilocks에서 definitelyWritable은 지원하지 않는다.
        // updatable view에서 조건에 따라 updatable하지 않는 컬럼은
        // writable하지만 definitelyWritable하진 않다.
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getUpdatable();
    }

    public int isNullable(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getNullable() ? columnNullable : columnNoNulls;
    }

    public boolean isReadOnly(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return !mColumnDescs[aColNum - 1].getUpdatable();
    }

    public boolean isSearchable(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        // Goldilocks는 현재 모든 target column이 where 조건에 사용가능 하다.
        // 다만 like 연산에 쓸 수 있는 컬럼과 아닌 컬럼은 구분되지만,
        // JDBC에서 like-able의 여부를 묻는 메소드는 없다.
        return true;
    }

    public boolean isSigned(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return !mColumnDescs[aColNum - 1].getUnsigned();
    }

    public boolean isWritable(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return mColumnDescs[aColNum - 1].getUpdatable();
    }    
}
