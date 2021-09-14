package sunje.goldilocks.jdbc.core4;

import java.sql.ParameterMetaData;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.cm.SourceParameterType;
import sunje.goldilocks.jdbc.dt.CodeColumn;
import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.internal.BaseProtocolSource;
import sunje.goldilocks.jdbc.internal.ErrorHolder;
import sunje.goldilocks.jdbc.util.Logger;

class ParamDescription extends BaseProtocolSource implements SourceParameterType
{
    private int mNumber;
    private short mInOutType;
    private Jdbc4Statement mStatement;
    
    ParamDescription(ErrorHolder aErrorHolder, Jdbc4Statement aStatement, int aNumber)
    {
        super(aErrorHolder);
        mNumber = aNumber;
        mStatement = aStatement;
    }

    public void setStatementId(long aStatementId)
    {
        mStatement.setId(aStatementId);
    }
    
    public long getStatementId()
    {
        return mStatement.getId();
    }
    
    public void addParameterTypeProtocol(CmChannel aChannel) throws SQLException
    {
        aChannel.writeProtocolAlone(Protocol.PROTOCOL_PARAMETER_TYPE, this);
    }
    
    public int getParameterNumber()
    {
        return mNumber;
    }

    public void setParameterType(short aInOutType)
    {
        mInOutType = aInOutType;        
    }
    
    short getInOutType()
    {
        return mInOutType;
    }
}

public class Jdbc4ParameterMetaData implements ParameterMetaData
{
    private static CodeColumn DEFAULT_PARAM = CodeColumn.getCodeColumn(CodeColumn.GOLDILOCKS_DATA_TYPE_VARCHAR, CodeColumn.INTERVAL_INDICATOR_NA);
    private static int DEFAULT_PARAM_PRECISION = CodeColumn.DEFAULT_PRECISION_VARCHAR;
    private static int DEFAULT_PARAM_SCALE = 0;
    private static int DEFAULT_PARAM_NULLABLE = parameterNullableUnknown;
    private static boolean DEFAULT_PARAM_SIGNED = false;
    
    private ParamDescription[] mParamDescs;
    private Logger mLogger;
    
    Jdbc4ParameterMetaData(ParamDescription[] aColumnDescs, Logger aLogger)
    {
        mParamDescs = aColumnDescs;
        mLogger = aLogger;
    }
    
    private void checkColumnIndex(int aColNum) throws SQLException
    {
        if (aColNum <= 0 || aColNum > mParamDescs.length)
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

    public int getParameterCount() throws SQLException
    {
        return mParamDescs.length;
    }

    public String getParameterClassName(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return DEFAULT_PARAM.getClassName();
    }

    public int getParameterMode(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        if (mParamDescs[aColNum-1].getInOutType() == Protocol.PARAMETER_TYPE_IN)
        {
            return parameterModeIn;
        }
        else if (mParamDescs[aColNum-1].getInOutType() == Protocol.PARAMETER_TYPE_INOUT)
        {
            return parameterModeInOut;
        }
        else if (mParamDescs[aColNum-1].getInOutType() == Protocol.PARAMETER_TYPE_OUT)
        {
            return parameterModeOut;
        }
        return parameterModeUnknown;
    }

    public int getParameterType(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return DEFAULT_PARAM.getRepresentativeSqlType();
    }

    public String getParameterTypeName(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return DEFAULT_PARAM.getTypeName();
    }

    public int getPrecision(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return DEFAULT_PARAM_PRECISION;
    }

    public int getScale(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return DEFAULT_PARAM_SCALE;
    }

    public int isNullable(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return DEFAULT_PARAM_NULLABLE;
    }

    public boolean isSigned(int aColNum) throws SQLException
    {
        mLogger.logTrace();
        checkColumnIndex(aColNum);
        return DEFAULT_PARAM_SIGNED;
    }
}
