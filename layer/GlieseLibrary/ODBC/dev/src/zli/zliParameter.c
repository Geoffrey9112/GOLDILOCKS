/*******************************************************************************
 * zliParameter.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <cml.h>
#include <goldilocks.h>

#if defined( ODBC_ALL ) || defined( ODBC_DA )
#include <ssl.h>
#endif

#include <zlDef.h>
#include <zle.h>
#include <zld.h>
#include <zli.h>

#include <zlvCToSql.h>
#include <zlvSqlToC.h>

#include <zlai.h>
#include <zlci.h>

/**
 * @file zliParameter.c
 * @brief ODBC API Internal Parameter Routines.
 */

/**
 * @addtogroup zli
 * @{
 */

#if defined( ODBC_ALL )
stlStatus (*gZliNumParams[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                    zlsStmt       * aStmt,
                                                    stlInt16      * aParameterCount,
                                                    stlErrorStack * aErrorStack ) =
{
    zlaiNumParams,
    zlciNumParams,
};

stlStatus (*gZliGetParameterType[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                           zlsStmt       * aStmt,
                                                           stlUInt16       aParamIdx,
                                                           stlInt16      * aInputOutputType,
                                                           stlErrorStack * aErrorStack ) =
{
    zlaiGetParameterType,
    zlciGetParameterType,
};
#endif

/*
 * SQL type의 Default Type을 반환한다.
 * Default Type이 없다면, SQL_C_DEFAULT를 반환한다.
 */
static stlStatus zliGetDefaultType( stlInt16 aSQLType )
{
    stlInt16 sDefaultType = SQL_C_DEFAULT;

    /*
     * SQL_C_DEFAULT 타입을 검사하고, 환경을 설정한다.
     *  ===========================================================
     *   SQL type                         | ODBC type
     *  ===========================================================
     *   SQL_BOOLEAN                      | SQL_C_BOOLEAN
     *   SQL_CHAR                         | SQL_C_CHAR
     *   SQL_VARCHAR                      | SQL_C_CHAR
     *   SQL_LONGVARCHAR                  | SQL_C_CHAR
     *   SQL_NUMERIC                      | SQL_C_CHAR
     *   SQL_DECIMAL                      | SQL_C_CHAR
     *   SQL_SMALLINT                     | SQL_C_SSHORT
     *   SQL_INTEGER                      | SQL_C_SLONG
     *   SQL_BIGINT                       | SQL_C_SBIGINT
     *   SQL_TINYINT                      | SQL_C_STINYINT
     *   SQL_REAL                         | SQL_C_FLOAT
     *   SQL_FLOAT                        | SQL_C_DOUBLE
     *   SQL_DOUBLE                       | SQL_C_DOUBLE
     *   SQL_BINARY                       | SQL_C_BINARY
     *   SQL_VARBINARY                    | SQL_C_BINARY
     *   SQL_LONGVARBINARY                | SQL_C_BINARY
     *   SQL_BIT                          | SQL_C_BIT
     *   SQL_TYPE_DATE                    | SQL_C_TYPE_DATE
     *   SQL_TYPE_TIME                    | SQL_C_TYPE_TIME
     *   SQL_TYPE_TIMESTAMP               | SQL_C_TYPE_TIMESTAMP
     *   SQL_TYPE_TIME_WITH_TIMEZONE      | SQL_C_TYPE_TIME_WITH_TIMEZONE
     *   SQL_TYPE_TIMESTAMP_WITH_TIMEZONE | SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE
     *   INTERVAL_SQL(YEAR-MONUTH)        | SQL_C_INTERVAL_YEAR_TO_MONTH
     *   INTERVAL_SQL(DATE_TIME)          | SQL_C_INTERVAL_DAY_TO_SECOND
     *  ===========================================================
     */
    switch( aSQLType )
    {
        case SQL_BOOLEAN:
            sDefaultType = SQL_C_BOOLEAN;
            break;
        case SQL_SMALLINT:
            sDefaultType = SQL_C_SSHORT;
            break;
        case SQL_INTEGER:
            sDefaultType = SQL_C_SLONG;
            break;
        case SQL_BIGINT:
            sDefaultType = SQL_C_SBIGINT;
            break;
        case SQL_REAL:
            sDefaultType = SQL_C_FLOAT;
            break;
        case SQL_FLOAT:
        case SQL_DOUBLE:
            sDefaultType = SQL_C_DOUBLE;
            break;
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_NUMERIC:
        case SQL_ROWID:
            sDefaultType = SQL_C_CHAR;            
            break;
        case SQL_BINARY:
        case SQL_VARBINARY:
        case SQL_LONGVARBINARY:
            sDefaultType = SQL_C_BINARY;
            break;
        case SQL_BIT:
            sDefaultType = SQL_C_BIT;
            break;
        case SQL_TINYINT:
            sDefaultType = SQL_C_STINYINT;
            break;
        case SQL_TYPE_DATE:
            sDefaultType = SQL_C_TYPE_DATE;
            break;
        case SQL_TYPE_TIME:
            sDefaultType = SQL_C_TYPE_TIME;
            break;
        case SQL_TYPE_TIME_WITH_TIMEZONE:
            sDefaultType = SQL_C_TYPE_TIME_WITH_TIMEZONE;
            break;
        case SQL_TYPE_TIMESTAMP:
            sDefaultType = SQL_C_TYPE_TIMESTAMP;
            break;
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE:
            sDefaultType = SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE;
            break;
        case SQL_INTERVAL_YEAR:
            sDefaultType = SQL_C_INTERVAL_YEAR;
            break;
        case SQL_INTERVAL_MONTH:
            sDefaultType = SQL_C_INTERVAL_MONTH;
            break;
        case SQL_INTERVAL_YEAR_TO_MONTH:
            sDefaultType = SQL_C_INTERVAL_YEAR_TO_MONTH;
            break;
        case SQL_INTERVAL_DAY:
            sDefaultType = SQL_C_INTERVAL_DAY;
            break;
        case SQL_INTERVAL_HOUR:
            sDefaultType = SQL_C_INTERVAL_HOUR;
            break;
        case SQL_INTERVAL_MINUTE:
            sDefaultType = SQL_C_INTERVAL_MINUTE;
            break;
        case SQL_INTERVAL_SECOND:
            sDefaultType = SQL_C_INTERVAL_SECOND;
            break;
        case SQL_INTERVAL_DAY_TO_HOUR:
            sDefaultType = SQL_C_INTERVAL_DAY_TO_HOUR;
            break;
        case SQL_INTERVAL_DAY_TO_MINUTE:
            sDefaultType = SQL_C_INTERVAL_DAY_TO_MINUTE;
            break;
        case SQL_INTERVAL_DAY_TO_SECOND:
            sDefaultType = SQL_C_INTERVAL_DAY_TO_SECOND;
            break;
        case SQL_INTERVAL_HOUR_TO_MINUTE:
            sDefaultType = SQL_C_INTERVAL_HOUR_TO_MINUTE;
            break;
        case SQL_INTERVAL_HOUR_TO_SECOND:
            sDefaultType = SQL_C_INTERVAL_HOUR_TO_SECOND;
            break;
        case SQL_INTERVAL_MINUTE_TO_SECOND:
            sDefaultType = SQL_C_INTERVAL_MINUTE_TO_SECOND;
            break;
        default:
            /*
             * Default type가 없는 경우, SQL_C_DEFAULT를 반환한다.
             */
            break;
    }

    return sDefaultType;
}

stlStatus zliGetDtlDataType( stlInt16        aSqlType,
                             dtlDataType   * aDtlDataType,
                             stlErrorStack * aErrorStack )
{
    dtlDataType sDtlDataType;

    switch( aSqlType )
    {
        case SQL_CHAR :
            sDtlDataType = DTL_TYPE_CHAR;
            break;
        case SQL_VARCHAR :
            sDtlDataType = DTL_TYPE_VARCHAR;
            break;
        case SQL_LONGVARCHAR :
            sDtlDataType = DTL_TYPE_LONGVARCHAR;
            break;
        case SQL_DECIMAL :
            sDtlDataType = DTL_TYPE_DECIMAL;
            break;
        case SQL_NUMERIC :
            sDtlDataType = DTL_TYPE_NUMBER;
            break;
        case SQL_SMALLINT :
        case SQL_TINYINT :
            sDtlDataType = DTL_TYPE_NATIVE_SMALLINT;
            break;
        case SQL_INTEGER :
            sDtlDataType = DTL_TYPE_NATIVE_INTEGER;
            break;
        case SQL_REAL :
            sDtlDataType = DTL_TYPE_NATIVE_REAL;
            break;
        case SQL_FLOAT :
            sDtlDataType = DTL_TYPE_FLOAT;
            break;
        case SQL_DOUBLE :
            sDtlDataType = DTL_TYPE_NATIVE_DOUBLE;
            break;
        case SQL_BIT :
        case SQL_BOOLEAN :
            sDtlDataType = DTL_TYPE_BOOLEAN;
            break;
        case SQL_BIGINT :
            sDtlDataType = DTL_TYPE_NATIVE_BIGINT;
            break;
        case SQL_BINARY :
            sDtlDataType = DTL_TYPE_BINARY;
            break;
        case SQL_VARBINARY :
            sDtlDataType = DTL_TYPE_VARBINARY;
            break;
        case SQL_LONGVARBINARY :
            sDtlDataType = DTL_TYPE_LONGVARBINARY;
            break;
        case SQL_TYPE_DATE :
            sDtlDataType = DTL_TYPE_DATE;
            break;
        case SQL_TYPE_TIME :
            sDtlDataType = DTL_TYPE_TIME;
            break;
        case SQL_TYPE_TIME_WITH_TIMEZONE :
            sDtlDataType = DTL_TYPE_TIME_WITH_TIME_ZONE;
            break;
        case SQL_TYPE_TIMESTAMP :
            sDtlDataType = DTL_TYPE_TIMESTAMP;
            break;
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            sDtlDataType = DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE;
            break;
        case SQL_INTERVAL_MONTH :
        case SQL_INTERVAL_YEAR :
        case SQL_INTERVAL_YEAR_TO_MONTH :
            sDtlDataType = DTL_TYPE_INTERVAL_YEAR_TO_MONTH;
            break;
        case SQL_INTERVAL_DAY :
        case SQL_INTERVAL_HOUR :
        case SQL_INTERVAL_MINUTE :
        case SQL_INTERVAL_SECOND :
        case SQL_INTERVAL_DAY_TO_HOUR :
        case SQL_INTERVAL_DAY_TO_MINUTE :
        case SQL_INTERVAL_DAY_TO_SECOND :
        case SQL_INTERVAL_HOUR_TO_MINUTE :
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            sDtlDataType = DTL_TYPE_INTERVAL_DAY_TO_SECOND;
            break;
        case SQL_ROWID :
            sDtlDataType = DTL_TYPE_ROWID;
            break;
        case SQL_WCHAR :
        case SQL_WVARCHAR :
        case SQL_WLONGVARCHAR :
        default :
            STL_THROW( RAMP_ERR_INVALID_SQL_DATA_TYPE );
            break;
    }

    if( aDtlDataType != NULL )
    {
        *aDtlDataType = sDtlDataType;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SQL_DATA_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_SQL_DATA_TYPE,
                      "The value specified for the argument ParameterType was "
                      "neither a valid ODBC SQL data type identifier nor "
                      "a driver-specific SQL data type identifier supported by the driver.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

void zliGetDtlDataArgs( zldDescElement * aRec,
                        stlInt64       * aArg1,
                        stlInt64       * aArg2 )
{
    switch( aRec->mConsiceType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
        case SQL_WCHAR :
        case SQL_WVARCHAR :
        case SQL_BINARY :
        case SQL_VARBINARY :
            *aArg1 = aRec->mLength;
            *aArg2 = DTL_SCALE_NA;
            break;
        case SQL_LONGVARCHAR :
        case SQL_WLONGVARCHAR :
            *aArg1 = DTL_LONGVARCHAR_MAX_PRECISION;
            *aArg2 = DTL_SCALE_NA;
            break;
        case SQL_LONGVARBINARY :
            *aArg1 = DTL_LONGVARBINARY_MAX_PRECISION;
            *aArg2 = DTL_SCALE_NA;
            break;
        case SQL_TYPE_DATE :
            *aArg1 = DTL_PRECISION_NA;
            *aArg2 = DTL_SCALE_NA;
            break;
        case SQL_TYPE_TIME :
        case SQL_TYPE_TIME_WITH_TIMEZONE :
        case SQL_TYPE_TIMESTAMP :
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            *aArg1 = aRec->mPrecision;
            *aArg2 = DTL_SCALE_NA;
            break;
        case SQL_INTERVAL_YEAR :
        case SQL_INTERVAL_MONTH :
        case SQL_INTERVAL_DAY :
        case SQL_INTERVAL_HOUR :
        case SQL_INTERVAL_MINUTE :
            *aArg1 = aRec->mDatetimeIntervalPrecision;
            *aArg2 = DTL_SCALE_NA;
            break;
        case SQL_INTERVAL_SECOND :
            *aArg1 = aRec->mDatetimeIntervalPrecision;
            *aArg2 = aRec->mPrecision;
            break;
        case SQL_INTERVAL_YEAR_TO_MONTH :
        case SQL_INTERVAL_DAY_TO_HOUR :
        case SQL_INTERVAL_DAY_TO_MINUTE :
            *aArg1 = aRec->mDatetimeIntervalPrecision;
            *aArg2 = DTL_SCALE_NA;
            break;
        case SQL_INTERVAL_DAY_TO_SECOND :
            *aArg1 = aRec->mDatetimeIntervalPrecision;
            *aArg2 = aRec->mPrecision;
            break;
        case SQL_INTERVAL_HOUR_TO_MINUTE :
            *aArg1 = aRec->mDatetimeIntervalPrecision;
            *aArg2 = DTL_SCALE_NA;
            break;
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            *aArg1 = aRec->mDatetimeIntervalPrecision;
            *aArg2 = aRec->mPrecision;
            break;
        case SQL_REAL :
        case SQL_DOUBLE :
        case SQL_DECIMAL :
        case SQL_SMALLINT :
        case SQL_INTEGER :
        case SQL_BIGINT :
            *aArg1 = DTL_PRECISION_NA;
            *aArg2 = DTL_SCALE_NA;
            break;
        case SQL_FLOAT :
            *aArg1 = aRec->mPrecision;
            *aArg2 = DTL_SCALE_NA;
            break;
        case SQL_NUMERIC :
            *aArg1 = aRec->mPrecision;
            *aArg2 = aRec->mScale;
            break;
        case SQL_ROWID :
            *aArg1 = DTL_PRECISION_NA;
            *aArg2 = DTL_SCALE_NA;
            break;                
        default :
            *aArg1 = DTL_PRECISION_NA;
            *aArg2 = DTL_SCALE_NA;
            break;
    }
}

dtlStringLengthUnit zliGetDtlDataUnit( zlsStmt        * aStmt,
                                       zldDescElement * aRec,
                                       stlInt16         aCType,
                                       stlInt16         aSqlType )
{
    dtlStringLengthUnit sStringLengthUnit;

    switch( aSqlType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
            if( (aCType == SQL_C_CHAR) || (aCType == SQL_C_LONGVARCHAR) )
            {
                switch( aRec->mStringLengthUnit )
                {
                    case DTL_STRING_LENGTH_UNIT_OCTETS :
                    case DTL_STRING_LENGTH_UNIT_CHARACTERS :
                        break;
                    case DTL_STRING_LENGTH_UNIT_NA :
                        aRec->mStringLengthUnit = aStmt->mStringLengthUnit;
                        break;
                    default :
                        STL_ASSERT( STL_FALSE );
                        break;
                }
                
                sStringLengthUnit = aRec->mStringLengthUnit;
            }
            else
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
            }
            break;
        case SQL_LONGVARCHAR :
            sStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
            break;
        default :
            sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
            break;
    }

    return sStringLengthUnit;
}

dtlIntervalIndicator zliGetDtlDataIndicator( stlInt16 aSqlType )
{
    dtlIntervalIndicator sIntervalIndicator;
    
    switch( aSqlType )
    {
        case SQL_INTERVAL_YEAR:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_YEAR;
            break;
        case SQL_INTERVAL_MONTH:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_MONTH;
            break;
        case SQL_INTERVAL_DAY:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY;
            break;
        case SQL_INTERVAL_HOUR:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_HOUR;
            break;
        case SQL_INTERVAL_MINUTE:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_MINUTE;
            break;
        case SQL_INTERVAL_SECOND:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_SECOND;
            break;
        case SQL_INTERVAL_YEAR_TO_MONTH:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH;
            break;
        case SQL_INTERVAL_DAY_TO_HOUR:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_HOUR;
            break;
        case SQL_INTERVAL_DAY_TO_MINUTE:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE;
            break;            
        case SQL_INTERVAL_DAY_TO_SECOND:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
            break;                        
        case SQL_INTERVAL_HOUR_TO_MINUTE:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE;
            break;                        
        case SQL_INTERVAL_HOUR_TO_SECOND:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND;
            break;                        
        case SQL_INTERVAL_MINUTE_TO_SECOND:            
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND;
            break;
        default:
            sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
            break;
    }

    return sIntervalIndicator;
}

stlStatus zliBindParameter( zlsStmt       * aStmt,
                            stlUInt16       aParamIdx,
                            stlInt16        aInOutType,
                            stlInt16        aCType,
                            stlInt16        aSQLType,
                            stlUInt64       aColSize,
                            stlInt16        aDigits,
                            void          * aBuffer,
                            SQLLEN          aBufferLen,
                            SQLLEN        * aInd,
                            stlErrorStack * aErrorStack )
{
    zldDesc        * sApd;
    zldDesc        * sIpd;
    zldDescElement * sApdRec;
    zldDescElement * sIpdRec;
    stlInt16         sCType;
    stlInt16         sSQLType;

    sApd = aStmt->mApd;
    sIpd = &aStmt->mIpd;

    /*
     * aCType을 sCType에 넣어준다.
     * 만약, aCType이 SQL_C_DEFAULT라면 Default type을 구한다. 
     */
    if( aCType == SQL_C_DEFAULT )
    {
        sCType = zliGetDefaultType( aSQLType );
    }
    else
    {
        sCType = aCType;
    }

    if( aParamIdx > sApd->mCount )
    {
        STL_TRY( zldDescAllocRec( sApd,
                                  aParamIdx,
                                  &sApdRec,
                                  aErrorStack ) == STL_SUCCESS );

        aStmt->mParamChanged = STL_TRUE;
        sApd->mChanged = STL_TRUE;
    }
    else
    {
        STL_TRY( zldDescFindRec( sApd, aParamIdx, &sApdRec ) == STL_SUCCESS );
    }

    STL_TRY( zldDescSetTargetType( aStmt, sApdRec, sCType, aBufferLen ) == STL_SUCCESS );

    sApdRec->mCtoSqlFunc       = zlvInvalidCtoSql;
    sApdRec->mOctetLength      = aBufferLen;
    sApdRec->mDataPtr          = aBuffer;
    sApdRec->mIndicatorPtr     = aInd;
    sApdRec->mOctetLengthPtr   = aInd;
    sApdRec->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;

    STL_TRY( zldDescCheckConsistency( sApd,
                                      sApdRec,
                                      aErrorStack ) == STL_SUCCESS );

    if( aParamIdx > sIpd->mCount )
    {
        STL_TRY( zldDescAllocRec( sIpd,
                                  aParamIdx,
                                  &sIpdRec,
                                  aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( zldDescFindRec( sIpd, aParamIdx, &sIpdRec ) == STL_SUCCESS );

        /*
         * 기존 정보와 동일한지 확인
         */
        if( (sIpdRec->mChanged == STL_FALSE ) &&
            (sIpdRec->mConsiceType == aSQLType) &&
            (sIpdRec->mParameterType == aInOutType) )
        {
            switch( aSQLType )
            {
                case SQL_CHAR :
                case SQL_VARCHAR :
                case SQL_WCHAR :
                case SQL_WVARCHAR :
                case SQL_WLONGVARCHAR :
                case SQL_BINARY :
                case SQL_VARBINARY :
                    STL_TRY_THROW( sIpdRec->mLength != aColSize, RAMP_SUCCESS );
                    break;
                case SQL_LONGVARCHAR :
                case SQL_LONGVARBINARY :
                case SQL_TYPE_DATE :
                    STL_THROW( RAMP_SUCCESS );
                    break;
                case SQL_TYPE_TIME :
                case SQL_TYPE_TIME_WITH_TIMEZONE :
                case SQL_TYPE_TIMESTAMP :
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    STL_TRY_THROW( sIpdRec->mPrecision != aDigits, RAMP_SUCCESS );
                    break;
                case SQL_FLOAT :
                    STL_TRY_THROW( sIpdRec->mPrecision != aColSize, RAMP_SUCCESS );
                    break;
                case SQL_NUMERIC :
                    STL_TRY_THROW( ((sIpdRec->mPrecision != aColSize) ||
                                    (sIpdRec->mScale != aDigits)), RAMP_SUCCESS );
                    break;
                case SQL_INTERVAL_YEAR :
                case SQL_INTERVAL_MONTH :
                case SQL_INTERVAL_DAY :
                case SQL_INTERVAL_HOUR :
                case SQL_INTERVAL_MINUTE :
                case SQL_INTERVAL_SECOND :
                case SQL_INTERVAL_YEAR_TO_MONTH :
                case SQL_INTERVAL_DAY_TO_HOUR :
                case SQL_INTERVAL_DAY_TO_MINUTE :
                case SQL_INTERVAL_HOUR_TO_MINUTE :
                    STL_TRY_THROW( sIpdRec->mDatetimeIntervalPrecision != DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,
                                   RAMP_SUCCESS );
                    break;
                case SQL_INTERVAL_DAY_TO_SECOND :
                case SQL_INTERVAL_HOUR_TO_SECOND :
                case SQL_INTERVAL_MINUTE_TO_SECOND :                    
                    STL_TRY_THROW( ((sIpdRec->mPrecision != aDigits) ||
                                    (sIpdRec->mDatetimeIntervalPrecision != DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION)),
                                   RAMP_SUCCESS );
                    break;
                default :
                    break;
            }

            switch( aInOutType )
            {
                case SQL_PARAM_INPUT :
                    STL_TRY_THROW( aStmt->mHasParamIN != STL_TRUE, RAMP_SUCCESS );
                    break;
                case SQL_PARAM_INPUT_OUTPUT :
                case SQL_PARAM_INPUT_OUTPUT_STREAM :
                    STL_TRY_THROW( aStmt->mHasParamIN != STL_TRUE, RAMP_SUCCESS );
                    STL_TRY_THROW( aStmt->mHasParamOUT != STL_TRUE, RAMP_SUCCESS );
                    break;
                case SQL_PARAM_OUTPUT_STREAM :
                case SQL_PARAM_OUTPUT :
                    STL_TRY_THROW( aStmt->mHasParamOUT != STL_TRUE, RAMP_SUCCESS );
                    break;
                default :
                    STL_ASSERT(0);
                    break;
            }
        }
    }

    aStmt->mParamChanged = STL_TRUE;


    /**
     * Fixed Length String Type은 Variable Length String Type처럼 취급
     */

    if( aInOutType != SQL_PARAM_INPUT )
    {
        if( aSQLType == SQL_CHAR )
        {
            sSQLType = SQL_VARCHAR;
        }
        else if( aSQLType == SQL_BINARY )
        {
            sSQLType = SQL_VARBINARY;
        }
        else
        {
            sSQLType = aSQLType;
        }
    }
    else
    {
        sSQLType = aSQLType;
    }
    
    STL_TRY( zldDescSetParameterType( sIpdRec,
                                      sSQLType,
                                      aColSize,
                                      aDigits,
                                      aErrorStack ) == STL_SUCCESS );

    sIpdRec->mParameterType = aInOutType;

    switch( sIpdRec->mStringLengthUnit )
    {
        case DTL_STRING_LENGTH_UNIT_CHARACTERS :
        case DTL_STRING_LENGTH_UNIT_OCTETS :
            break;
        case DTL_STRING_LENGTH_UNIT_NA :
            sIpdRec->mStringLengthUnit = aStmt->mStringLengthUnit;
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    switch( aInOutType )
    {
        case SQL_PARAM_INPUT :
            aStmt->mHasParamIN = STL_TRUE;
            break;
        case SQL_PARAM_INPUT_OUTPUT :
            aStmt->mHasParamIN = STL_TRUE;
            aStmt->mHasParamOUT = STL_TRUE;
            break;
        case SQL_PARAM_INPUT_OUTPUT_STREAM :
            aStmt->mHasParamIN = STL_TRUE;
            aStmt->mHasParamOUT = STL_TRUE;
            break;
        case SQL_PARAM_OUTPUT_STREAM :
            aStmt->mHasParamOUT = STL_TRUE;
            break;
        case SQL_PARAM_OUTPUT :
            aStmt->mHasParamOUT = STL_TRUE;
            break;
        default :
            STL_ASSERT(0);
            break;
    }

    STL_TRY( zldDescCheckConsistency( sIpd,
                                      sIpdRec,
                                      aErrorStack ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Execute 전 DATA_AT_EXEC Parameter가 존재하는지 확인한다.
 * @param[in]  aStmt            Session Statement
 * @param[in]  aApd             Application Parameter Descriptor
 * @param[in]  aIpd             Implemention Parameter Desciptor
 * @param[out] aHasDataAtExec   DATA_AT_EXEC 존재 여부
 * @param[in]  aErrorStack      Error Stack
 * @remarks
 */
stlStatus zliHasDataAtExecIndicator( zlsStmt       * aStmt,
                                     zldDesc       * aApd,
                                     zldDesc       * aIpd,
                                     stlBool       * aHasDataAtExec,
                                     stlErrorStack * aErrorStack )
{
    zldDescElement * sApdRec = NULL;
    zldDescElement * sIpdRec = NULL;
    stlInt32         sParamIdx = 0;
    stlUInt64        sArraySize;
    SQLLEN         * sIndicator;
    stlUInt64        i;

    *aHasDataAtExec = STL_FALSE;

    sArraySize = aApd->mArraySize;
    STL_DASSERT( sArraySize > 0 );

    sIpdRec = STL_RING_GET_FIRST_DATA( &aIpd->mDescRecRing );

    STL_RING_FOREACH_ENTRY( &aApd->mDescRecRing, sApdRec )
    {
        sParamIdx++;

        if( aStmt->mParamCount != ZLS_PARAM_COUNT_UNKNOWN )
        {
            if( sParamIdx > aStmt->mParamCount )
            {
                break;
            }
        }

        /**
         * IN parameter 만 설정
         */

        switch( sIpdRec->mParameterType )
        {
            case SQL_PARAM_INPUT :
            case SQL_PARAM_INPUT_OUTPUT :
            case SQL_PARAM_INPUT_OUTPUT_STREAM :
                break;
            default :
                STL_THROW( RAMP_NEXT_IPD );
                break;
        }

        /*
         * PutData 관련 정보 구축
         */
        
        for( i = 1; i <= sArraySize; i++ )
        {
            if( aApd->mArrayStatusPtr != NULL )
            {
                if( aApd->mArrayStatusPtr[i-1] == SQL_PARAM_IGNORE )
                {
                    continue;
                }
            }

            sIndicator = zliGetIndicatorBuffer( aApd,
                                                sApdRec,
                                                i );
            if( sIndicator == NULL )
            {
                continue;
            }

            if( (*sIndicator == SQL_DATA_AT_EXEC) ||
                (*sIndicator <= SQL_LEN_DATA_AT_EXEC_OFFSET) )
            {
                aStmt->mCurrentPutDataApdRec   = sApdRec;
                aStmt->mCurrentPutDataIpdRec   = sIpdRec;
                aStmt->mCurrentPutDataIdx      = sParamIdx;
                aStmt->mCurrentPutDataArrayIdx = i;
                
                *aHasDataAtExec = STL_TRUE;

                STL_THROW( RAMP_FINISH );
            }
        }
        
        STL_RAMP( RAMP_NEXT_IPD );
        
        sIpdRec = STL_RING_GET_NEXT_DATA( &aIpd->mDescRecRing,
                                          sIpdRec );
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;
}


/**
 * @brief Execute 전 DATA_AT_EXEC Parameter가 존재하는지 확인한다.
 * @param[in]  aStmt            Session Statement
 * @param[in]  aApd             Application Parameter Descriptor
 * @param[in]  aIpd             Implemention Parameter Desciptor
 * @param[out] aHasDataAtExec   DATA_AT_EXEC 존재 여부
 * @param[in]  aErrorStack      Error Stack
 * @remarks
 */
stlStatus zliReadyDataAtExec( zlsStmt       * aStmt,
                              zldDesc       * aApd,
                              zldDesc       * aIpd,
                              stlBool       * aHasDataAtExec,
                              stlErrorStack * aErrorStack )
{
    zldDescElement * sApdRec = NULL;
    zldDescElement * sStartApdRec = NULL;
    zldDescElement * sIpdRec = NULL;
    stlInt32         sParamIdx = 0;
    stlUInt64        sArraySize;
    SQLLEN         * sIndicator;
    stlBool          sSetCurrentPutData = STL_FALSE;
    stlInt32         sSize;
    stlUInt64        sStartIdx;
    stlUInt64        i;

    *aHasDataAtExec = STL_FALSE;

    sArraySize = aApd->mArraySize;
    STL_DASSERT( sArraySize > 0 );

    STL_DASSERT( aStmt->mParamCount != ZLS_PARAM_COUNT_UNKNOWN );

    if( aStmt->mCurrentPutDataApdRec != NULL )
    {
        sStartApdRec = aStmt->mCurrentPutDataApdRec;
    }
    else
    {
        sStartApdRec = STL_RING_GET_FIRST_DATA( &aApd->mDescRecRing );
    }

    if( aStmt->mCurrentPutDataIpdRec != NULL )
    {
        sIpdRec = aStmt->mCurrentPutDataIpdRec;
    }
    else
    {
        sIpdRec = STL_RING_GET_FIRST_DATA( &aIpd->mDescRecRing );
    }

    if( aStmt->mCurrentPutDataArrayIdx != 0 )
    {
        sStartIdx = aStmt->mCurrentPutDataArrayIdx;
    }
    else
    {
        sStartIdx = 1;
    }

    if( aStmt->mCurrentPutDataIdx != 0 )
    {
        sSetCurrentPutData = STL_TRUE;

        sParamIdx = aStmt->mCurrentPutDataIdx - 1;
    }
    else
    {
        sParamIdx = 0;
    }
    
    STL_RING_AT_FOREACH_ENTRY( &aApd->mDescRecRing, sStartApdRec, sApdRec )
    {
        sParamIdx++;

        if( sParamIdx > aStmt->mParamCount )
        {
            break;
        }

        /**
         * IN parameter 만 설정
         */

        switch( sIpdRec->mParameterType )
        {
            case SQL_PARAM_INPUT :
            case SQL_PARAM_INPUT_OUTPUT :
            case SQL_PARAM_INPUT_OUTPUT_STREAM :
                break;
            default :
                STL_THROW( RAMP_NEXT_IPD );
                break;
        }

        /*
         * PutData 관련 정보 초기화
         */

        if( sArraySize != aApd->mPutDataArraySize )
        {
            if( sApdRec->mPutDataBuffer != NULL )
            {
                for( i = 0; i < aApd->mPutDataArraySize; i++ )
                {
                    if( sApdRec->mPutDataBuffer[i] != NULL )
                    {
                        STL_TRY( stlFreeDynamicMem( &aStmt->mLongVaryingMem,
                                                    sApdRec->mPutDataBuffer[i],
                                                    aErrorStack ) == STL_SUCCESS );

                        sApdRec->mPutDataBuffer[i] = NULL;
                    }
                }

                STL_TRY( stlFreeDynamicMem( &aStmt->mLongVaryingMem,
                                            (void*)sApdRec->mPutDataBuffer,
                                            aErrorStack ) == STL_SUCCESS );

                sApdRec->mPutDataBuffer = NULL;
            }

            if( sApdRec->mPutDataIndicator != NULL )
            {
                STL_TRY( stlFreeDynamicMem( &aStmt->mLongVaryingMem,
                                            (void*)sApdRec->mPutDataIndicator,
                                            aErrorStack ) == STL_SUCCESS );

                sApdRec->mPutDataIndicator = NULL;
            }

            if( sApdRec->mPutDataBufferSize != NULL )
            {
                STL_TRY( stlFreeDynamicMem( &aStmt->mLongVaryingMem,
                                            (void*)sApdRec->mPutDataBufferSize,
                                            aErrorStack ) == STL_SUCCESS );

                sApdRec->mPutDataBufferSize = NULL;

                aApd->mPutDataArraySize = 0;
            }
        }
        else
        {
            for( i = 0; i < aApd->mPutDataArraySize; i++ )
            {
                sApdRec->mPutDataIndicator[i] = 0;
            }
        }

        /*
         * PutData 관련 정보 구축
         */
        
        for( i = sStartIdx; i <= sArraySize; i++ )
        {
            if( aApd->mArrayStatusPtr != NULL )
            {
                if( aApd->mArrayStatusPtr[i-1] == SQL_PARAM_IGNORE )
                {
                    continue;
                }
            }

            sIndicator = zliGetIndicatorBuffer( aApd,
                                                sApdRec,
                                                i );
            if( sIndicator == NULL )
            {
                continue;
            }

            if( (*sIndicator == SQL_DATA_AT_EXEC) ||
                (*sIndicator <= SQL_LEN_DATA_AT_EXEC_OFFSET) )
            {
                if( sApdRec->mPutDataBuffer == NULL )
                {
                    STL_DASSERT( aApd->mPutDataArraySize == 0 );
                    
                    sSize = STL_SIZEOF(void*) * sArraySize;
                    
                    STL_TRY( stlAllocDynamicMem( &aStmt->mLongVaryingMem,
                                                 sSize,
                                                 (void**)&sApdRec->mPutDataBuffer,
                                                 aErrorStack )
                             == STL_SUCCESS );

                    stlMemset( (void*)sApdRec->mPutDataBuffer, 0x00, sSize );

                    STL_DASSERT( sApdRec->mPutDataBufferSize == NULL );
                    
                    sSize = STL_SIZEOF(SQLULEN) * sArraySize;

                    STL_TRY( stlAllocDynamicMem( &aStmt->mLongVaryingMem,
                                                 sSize,
                                                 (void**)&sApdRec->mPutDataBufferSize,
                                                 aErrorStack )
                             == STL_SUCCESS );

                    stlMemset( (void*)sApdRec->mPutDataBufferSize, 0x00, sSize );

                    STL_DASSERT( sApdRec->mPutDataIndicator == NULL );
                    
                    sSize = STL_SIZEOF(SQLLEN) * sArraySize;

                    STL_TRY( stlAllocDynamicMem( &aStmt->mLongVaryingMem,
                                                 sSize,
                                                 (void**)&sApdRec->mPutDataIndicator,
                                                 aErrorStack )
                             == STL_SUCCESS );

                    stlMemset( (void*)sApdRec->mPutDataIndicator, 0x00, sSize );
                }

                if( sSetCurrentPutData == STL_FALSE )
                {
                    aStmt->mCurrentPutDataApdRec   = sApdRec;
                    aStmt->mCurrentPutDataIpdRec   = sIpdRec;
                    aStmt->mCurrentPutDataIdx      = sParamIdx;
                    aStmt->mCurrentPutDataArrayIdx = i;

                    sSetCurrentPutData = STL_TRUE;
                }
                
                *aHasDataAtExec = STL_TRUE;
                
                break;
            }
        }
        
        STL_RAMP( RAMP_NEXT_IPD );
        
        sStartIdx = 1;

        sIpdRec = STL_RING_GET_NEXT_DATA( &aIpd->mDescRecRing,
                                          sIpdRec );
    }

    if( *aHasDataAtExec == STL_TRUE )
    {
        aApd->mPutDataArraySize = sArraySize;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 다음 DATA_AT_EXEC Parameter를 반환한다.
 * @param[in]  aStmt            Session Statement
 * @param[in]  aApd             Application Parameter Descriptor
 * @param[in]  aIpd             Implemention Parameter Desciptor
 * @param[out] aHasDataAtExec   DATA_AT_EXEC 존재 여부
 * @param[in]  aErrorStack      Error Stack
 * @remarks
 */
stlStatus zliProecessDataAtExec( zlsStmt       * aStmt,
                                 zldDesc       * aApd,
                                 zldDesc       * aIpd,
                                 stlBool       * aHasDataAtExec,
                                 stlErrorStack * aErrorStack )
{
    zldDescElement * sApdRec = NULL;
    zldDescElement * sIpdRec = NULL;
    stlInt32         sParamIdx = 0;
    stlUInt64        sArraySize;
    SQLLEN         * sIndicator;
    stlBool          sIsFirst = STL_TRUE;
    stlUInt64        sStartIdx;
    stlUInt64        i;

    *aHasDataAtExec = STL_FALSE;

    sArraySize = aApd->mArraySize;
    STL_DASSERT( sArraySize > 0 );

    STL_DASSERT( aStmt->mCurrentPutDataApdRec != NULL );
    STL_DASSERT( aStmt->mCurrentPutDataIpdRec != NULL );
    STL_DASSERT( aStmt->mCurrentPutDataArrayIdx != 0 );
    STL_DASSERT( aStmt->mCurrentPutDataIdx != 0 );

    STL_DASSERT( sArraySize == aApd->mPutDataArraySize );
    
    sIpdRec = aStmt->mCurrentPutDataIpdRec;
    sParamIdx = aStmt->mCurrentPutDataIdx - 1;
    
    STL_RING_AT_FOREACH_ENTRY( &aApd->mDescRecRing, aStmt->mCurrentPutDataApdRec, sApdRec )
    {
        sParamIdx++;

        if( sParamIdx > aStmt->mParamCount )
        {
            break;
        }

        /**
         * IN parameter 만 설정
         */

        switch( sIpdRec->mParameterType )
        {
            case SQL_PARAM_INPUT :
            case SQL_PARAM_INPUT_OUTPUT :
            case SQL_PARAM_INPUT_OUTPUT_STREAM :
                break;
            default :
                STL_THROW( RAMP_NEXT_IPD );
                break;
        }

        /*
         * PutData 관련 정보 구축
         */

        if( sIsFirst == STL_TRUE )
        {
            sStartIdx = aStmt->mCurrentPutDataArrayIdx + 1;
        }
        else
        {
            sStartIdx = 1;
        }
        
        for( i = sStartIdx; i <= sArraySize; i++ )
        {
            if( aApd->mArrayStatusPtr != NULL )
            {
                if( aApd->mArrayStatusPtr[i-1] == SQL_PARAM_IGNORE )
                {
                    continue;
                }
            }

            sIndicator = zliGetIndicatorBuffer( aApd,
                                                sApdRec,
                                                i );
            if( sIndicator == NULL )
            {
                continue;
            }

            if( (*sIndicator == SQL_DATA_AT_EXEC) ||
                (*sIndicator <= SQL_LEN_DATA_AT_EXEC_OFFSET) )
            {
                STL_DASSERT( sApdRec->mPutDataBuffer != NULL );
                STL_DASSERT( sApdRec->mPutDataIndicator != NULL );
                STL_DASSERT( sApdRec->mPutDataBufferSize != NULL );
        
                aStmt->mCurrentPutDataApdRec   = sApdRec;
                aStmt->mCurrentPutDataIpdRec   = sIpdRec;
                aStmt->mCurrentPutDataIdx      = sParamIdx;
                aStmt->mCurrentPutDataArrayIdx = i;

                *aHasDataAtExec = STL_TRUE;

                STL_THROW( RAMP_FINISH );
            }
        }
        
        STL_RAMP( RAMP_NEXT_IPD );
        
        sIsFirst = STL_FALSE;
        
        sIpdRec = STL_RING_GET_NEXT_DATA( &aIpd->mDescRecRing,
                                          sIpdRec );
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;
}


/**
 * @brief 현재 parameter에 데이터를 입력한다.
 * @param[in]  aStmt            Session Statement
 * @param[in]  aDataPtr         입력할 데이터
 * @param[in]  aStrLen_or_Ind   입력 데이터의 길이 또는 indicator
 * @param[in]  aErrorStack      Error Stack
 * @remarks
 */
stlStatus zliPutData( zlsStmt       * aStmt,
                      void          * aDataPtr,
                      SQLLEN          aStrLen_or_Ind,
                      stlErrorStack * aErrorStack )
{
    zldDescElement * sApdRec = NULL;
    stlUInt64        sArrayPos;
    stlInt32         sSize = 0;
    void           * sBuffer = NULL;
    stlBool          sFixedSize = STL_FALSE;

    STL_DASSERT( aStmt->mCurrentPutDataApdRec != NULL );
    STL_DASSERT( aStmt->mCurrentPutDataIpdRec != NULL );
    STL_DASSERT( aStmt->mCurrentPutDataArrayIdx != 0 );
    STL_DASSERT( aStmt->mCurrentPutDataIdx != 0 );

    sApdRec = aStmt->mCurrentPutDataApdRec;
    sArrayPos = aStmt->mCurrentPutDataArrayIdx - 1;

    STL_TRY_THROW( sApdRec->mPutDataIndicator[sArrayPos] != SQL_NULL_DATA,
                   RAMP_ERR_ATTEMPT_TO_CONCATENATE_A_NULL_VALUE );

    switch( sApdRec->mConsiceType )
    {
        case SQL_C_CHAR :
        case SQL_C_WCHAR :
        case SQL_C_BINARY :
            break;
        default :
            STL_TRY_THROW( sApdRec->mPutDataBuffer[sArrayPos] == NULL,
                           RAMP_ERR_NON_CHARACTER_AND_NON_BINARY_DATA_SENT_IN_PIECES );
            break;
    }
    
    if( aStrLen_or_Ind == SQL_NULL_DATA )
    {
        sApdRec->mPutDataIndicator[sArrayPos] = SQL_NULL_DATA;
    }
    else
    {
        switch( sApdRec->mConsiceType )
        {
            case SQL_C_WCHAR :
                /**
                 * @todo unicode odbc
                 */
            case SQL_C_CHAR :
            case SQL_C_BINARY :
                if( aStrLen_or_Ind == SQL_NTS )
                {
                    aStrLen_or_Ind = stlStrlen( (stlChar*)aDataPtr );
                }
                
                if( sApdRec->mPutDataBuffer[sArrayPos] == NULL )
                {
                    sSize = aStrLen_or_Ind + 1;
                    
                    STL_TRY( stlAllocDynamicMem( &aStmt->mLongVaryingMem,
                                                 sSize,
                                                 (void**)&sApdRec->mPutDataBuffer[sArrayPos],
                                                 aErrorStack )
                             == STL_SUCCESS );

                    stlMemset( sApdRec->mPutDataBuffer[sArrayPos], 0x00, sSize );

                    sApdRec->mPutDataBufferSize[sArrayPos] = sSize;
                }
                else
                {
                    sSize = sApdRec->mPutDataIndicator[sArrayPos] + aStrLen_or_Ind + 1;
                    
                    if( sApdRec->mPutDataBufferSize[sArrayPos] < sSize )
                    {
                        sSize = STL_MAX( sApdRec->mPutDataBufferSize[sArrayPos] * 2, sSize );

                        STL_TRY( stlAllocDynamicMem( &aStmt->mLongVaryingMem,
                                                     sSize,
                                                     (void**)&sBuffer,
                                                     aErrorStack )
                                 == STL_SUCCESS );
                        
                        stlMemset( sBuffer, 0x00, sSize );

                        stlMemcpy( sBuffer,
                                   sApdRec->mPutDataBuffer[sArrayPos],
                                   sApdRec->mPutDataIndicator[sArrayPos] );

                        STL_TRY( stlFreeDynamicMem( &aStmt->mLongVaryingMem,
                                                    sApdRec->mPutDataBuffer[sArrayPos],
                                                    aErrorStack )
                                 == STL_SUCCESS );

                        sApdRec->mPutDataBufferSize[sArrayPos] = sSize;
                        sApdRec->mPutDataBuffer[sArrayPos] = sBuffer;
                    }
                }

                stlMemcpy( (stlChar*)sApdRec->mPutDataBuffer[sArrayPos] + sApdRec->mPutDataIndicator[sArrayPos],
                           aDataPtr,
                           aStrLen_or_Ind );
                    
                sApdRec->mPutDataIndicator[sArrayPos] += aStrLen_or_Ind;

                ((stlChar*)(sApdRec->mPutDataBuffer[sArrayPos]))[sApdRec->mPutDataIndicator[sArrayPos]] = '\0';
                break;
            case SQL_C_LONGVARCHAR :
            case SQL_C_LONGVARBINARY :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT );
                break;
            case SQL_C_SSHORT :
            case SQL_C_USHORT :
            case SQL_C_SHORT :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( stlInt16 );
                break;
            case SQL_C_SLONG :
            case SQL_C_ULONG :
            case SQL_C_LONG :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( stlInt32 );
                break;
            case SQL_C_FLOAT :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( stlFloat32 );
                break;
            case SQL_C_DOUBLE :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( stlFloat64 );
                break;
            case SQL_C_BIT :
            case SQL_C_BOOLEAN :
            case SQL_C_STINYINT :
            case SQL_C_UTINYINT :
            case SQL_C_TINYINT :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( stlInt8 );
                break;
            case SQL_C_SBIGINT :
            case SQL_C_UBIGINT :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( stlInt64 );
                break;
            case SQL_C_TYPE_DATE :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( SQL_DATE_STRUCT );
                break;
            case SQL_C_TYPE_TIME :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( SQL_TIME_STRUCT );
                break;
            case SQL_C_TYPE_TIME_WITH_TIMEZONE :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( SQL_TIME_WITH_TIMEZONE_STRUCT );
                break;
            case SQL_C_TYPE_TIMESTAMP :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( SQL_TIMESTAMP_STRUCT );
                break;
            case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT );
                break;
            case SQL_C_INTERVAL_YEAR :
            case SQL_C_INTERVAL_MONTH :
            case SQL_C_INTERVAL_DAY :
            case SQL_C_INTERVAL_HOUR :
            case SQL_C_INTERVAL_MINUTE :
            case SQL_C_INTERVAL_SECOND :
            case SQL_C_INTERVAL_YEAR_TO_MONTH :
            case SQL_C_INTERVAL_DAY_TO_HOUR :
            case SQL_C_INTERVAL_DAY_TO_MINUTE :
            case SQL_C_INTERVAL_DAY_TO_SECOND :
            case SQL_C_INTERVAL_HOUR_TO_MINUTE :
            case SQL_C_INTERVAL_HOUR_TO_SECOND :
            case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( SQL_INTERVAL_STRUCT );
                break;
            case SQL_C_NUMERIC :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( SQL_NUMERIC_STRUCT );
                break;
            case SQL_C_GUID :
                sFixedSize = STL_TRUE;
                sSize = STL_SIZEOF( SQLGUID );
                break;
            default :
                stlPrintf("UNKNOWN TYPE %d\n", sApdRec->mConsiceType );
                STL_ASSERT( 0 );
                break;
        }

        if( sFixedSize == STL_TRUE )
        {
            STL_DASSERT( sApdRec->mPutDataBuffer[sArrayPos] == NULL );
            
            STL_TRY( stlAllocDynamicMem( &aStmt->mLongVaryingMem,
                                         sSize,
                                         (void**)&sApdRec->mPutDataBuffer[sArrayPos],
                                         aErrorStack )
                     == STL_SUCCESS );

            stlMemcpy( sApdRec->mPutDataBuffer[sArrayPos], aDataPtr, sSize );
                    
            sApdRec->mPutDataBufferSize[sArrayPos] = sSize;
            sApdRec->mPutDataIndicator[sArrayPos] = sSize;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ATTEMPT_TO_CONCATENATE_A_NULL_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_ATTEMPT_TO_CONCATENATE_A_NULL_VALUE,
                      "SQLPutData was called more than once since the call that "
                      "returned SQL_NEED_DATA, and in one of those calls, "
                      "the StrLen_or_Ind argument contained SQL_NULL_DATA or "
                      "SQL_DEFAULT_PARAM.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_NON_CHARACTER_AND_NON_BINARY_DATA_SENT_IN_PIECES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NON_CHARACTER_AND_NON_BINARY_DATA_SENT_IN_PIECES,
                      "SQLPutData was called more than once for a parameter or column, and "
                      "it was not being used to send character C data to a column with "
                      "a character, binary, or data source–specific data type or "
                      "to send binary C data to a column with a character, binary, or "
                      "data source–specific data type.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Execute 전 IN Parameter Value 를 설정한다.
 * @param[in]  aStmt            Session Statement
 * @param[in]  aApd             Application Parameter Descriptor
 * @param[in]  aIpd             Implemention Parameter Desciptor
 * @param[in]  aApdArrayIdx     APD Array Idx
 * @param[in]  aIpdArrayIdx     IPD Array Idx
 * @param[out] aParamWithInfo   Parameter With Information 여부 
 * @param[out] aParamError      Parameter Error 여부
 * @param[in]  aErrorStack      Error Stack
 * @remarks
 */
stlStatus zliSetParamValueIN( zlsStmt       * aStmt,
                              zldDesc       * aApd,
                              zldDesc       * aIpd,
                              stlUInt64       aApdArrayIdx,
                              stlUInt64       aIpdArrayIdx,
                              stlBool       * aParamWithInfo,
                              stlBool       * aParamError,
                              stlErrorStack * aErrorStack )
{
    zldDescElement   * sApdRec;
    zldDescElement   * sIpdRec;

    void              * sBuffer;
    SQLLEN            * sIndicator;
    stlBool             sIsNULL;

    dtlDataValue      * sDataValue;
    
    stlInt32  sParamIdx;
    stlBool   sSuccessWithInfo = STL_FALSE;
    stlBool   sParamError      = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aApd != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aIpd != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aParamWithInfo != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aParamError != NULL, aErrorStack );
    
    /**
     * parameter 정보 구축
     */
    
    sParamIdx = 0;
    
    sIpdRec = STL_RING_GET_FIRST_DATA( &aIpd->mDescRecRing );

    STL_RING_FOREACH_ENTRY( &aApd->mDescRecRing, sApdRec )
    {
        sParamIdx++;

        /**
         * IN parameter 만 설정
         */

        switch( sIpdRec->mParameterType )
        {
            case SQL_PARAM_INPUT :
            case SQL_PARAM_INPUT_OUTPUT :
            case SQL_PARAM_INPUT_OUTPUT_STREAM :
                break;
            default :
                STL_THROW( RAMP_NEXT_IPD );
                break;
        }

        /**
         * IPD 의 Data 공간 획득
         */
        
        sDataValue = DTL_GET_BLOCK_DATA_VALUE( sIpdRec->mInputValueBlock, aIpdArrayIdx );
            
        /**
         * C TYPE -> SQL TYPE
         */
        
        sBuffer = zliGetDataBuffer( aApd,
                                    sApdRec,
                                    aApdArrayIdx + 1 );

        sIndicator = zliGetIndicatorBuffer( aApd,
                                            sApdRec,
                                            aApdArrayIdx + 1 );

        sIsNULL = STL_FALSE;
        if( sIndicator != NULL )
        {
            if( *sIndicator == SQL_NULL_DATA )
            {
                sIsNULL = STL_TRUE;
            }

            if( sApdRec->mDataPtr == NULL )
            {
                STL_TRY_THROW( (*sIndicator == 0) ||
                               (*sIndicator == SQL_NULL_DATA) ||
                               (*sIndicator == SQL_DATA_AT_EXEC) ||
                               (*sIndicator == SQL_DEFAULT_PARAM) ||
                               (*sIndicator <= SQL_LEN_DATA_AT_EXEC_OFFSET),
                               RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH1 );
            }
            else
            {
                if( (sApdRec->mType == SQL_C_BINARY) ||
                    (sApdRec->mType == SQL_C_CHAR) )
                {
                    if( *sIndicator < 0 )
                    {
                        STL_TRY_THROW( (*sIndicator == SQL_NTS) ||
                                       (*sIndicator == SQL_NULL_DATA) ||
                                       (*sIndicator == SQL_DEFAULT_PARAM) ||
                                       (*sIndicator == SQL_DATA_AT_EXEC) ||
                                       (*sIndicator <= SQL_LEN_DATA_AT_EXEC_OFFSET),
                                       RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH2 );
                    }
                }
            }

            if( (*sIndicator == SQL_DATA_AT_EXEC) ||
                (*sIndicator <= SQL_LEN_DATA_AT_EXEC_OFFSET) )
            {
                sBuffer = sApdRec->mPutDataBuffer[aApdArrayIdx];
                sIndicator = &sApdRec->mPutDataIndicator[aApdArrayIdx];
            }
        }
        else
        {
            STL_TRY_THROW( sBuffer != NULL, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH1 );
        }

        if( sIsNULL == STL_TRUE )
        {
            DTL_SET_NULL( sDataValue );
        }
        else
        {
            if( sApdRec->mCtoSqlFunc == zlvInvalidCtoSql )
            {
                STL_TRY( zlvGetCToSqlFunc( aStmt,
                                           sApdRec->mConsiceType,
                                           sIpdRec->mConsiceType,
                                           &sApdRec->mCtoSqlFunc,
                                           aErrorStack ) == STL_SUCCESS );
            }

            if( sApdRec->mCtoSqlFunc( aStmt,
                                      sBuffer,
                                      sIndicator,
                                      sApdRec,
                                      sIpdRec,
                                      sDataValue,
                                      &sSuccessWithInfo,
                                      aErrorStack ) == STL_SUCCESS )
            {
                /* 성공은 했지만 warning이 있으면 */
                if( sSuccessWithInfo == STL_TRUE )
                {
                    zldDiagAdds( SQL_HANDLE_STMT,
                                 (void*)aStmt,
                                 aApdArrayIdx + 1,
                                 sParamIdx,
                                 aErrorStack );
                }
            }
            else
            {
                zldDiagAdds( SQL_HANDLE_STMT,
                             (void*)aStmt,
                             aApdArrayIdx + 1,
                             sParamIdx,
                             aErrorStack );

                if( aIpd->mArrayStatusPtr != NULL )
                {
                    aIpd->mArrayStatusPtr[aApdArrayIdx] = SQL_PARAM_ERROR;
                }

                sParamError = STL_TRUE;
                break;
            }
        }

        STL_RAMP( RAMP_NEXT_IPD );
        
        sIpdRec = STL_RING_GET_NEXT_DATA( &aIpd->mDescRecRing,
                                          sIpdRec );
    }

    /**
     * Output 설정
     */

    *aParamWithInfo = sSuccessWithInfo;
    *aParamError = sParamError;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "A parameter value, set with SQLBindParameter, "
                      "was a null pointer, and the parameter length value "
                      "was not 0, SQL_NULL_DATA, SQL_DATA_AT_EXEC, "
                      "SQL_DEFAULT_PARAM, or less than or equal to "
                      "SQL_LEN_DATA_AT_EXEC_OFFSET.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "A parameter value, set with SQLBindParameter, "
                      "was not a null pointer; the C data type was "
                      "SQL_C_BINARY or SQL_C_CHAR; and the parameter "
                      "length value was less than 0 but was not SQL_NTS, "
                      "SQL_NULL_DATA, SQL_DEFAULT_PARAM, or "
                      "SQL_DATA_AT_EXEC, or less than or equal to "
                      "SQL_LEN_DATA_AT_EXEC_OFFSET.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Execute 후 OUT Parameter Value 를 설정한다.
 * @param[in]  aStmt            Session Statement
 * @param[in]  aApd             Application Parameter Descriptor
 * @param[in]  aIpd             Implemention Parameter Desciptor
 * @param[in]  aApdArrayIdx     APD Array Idx
 * @param[in]  aIpdArrayIdx     IPD Array Idx
 * @param[out] aParamWithInfo   Parameter With Information 여부 
 * @param[out] aParamError      Parameter Error 여부
 * @param[in]  aErrorStack      Error Stack
 * @remarks
 */
stlStatus zliSetParamValueOUT( zlsStmt       * aStmt,
                               zldDesc       * aApd,
                               zldDesc       * aIpd,
                               stlUInt64       aApdArrayIdx,
                               stlUInt64       aIpdArrayIdx,
                               stlBool       * aParamWithInfo,
                               stlBool       * aParamError,
                               stlErrorStack * aErrorStack )
{
    zldDescElement    * sApdRec;
    zldDescElement    * sIpdRec;

    stlInt16            sBindType;

    void              * sBuffer;
    SQLLEN            * sIndicator;

    dtlDataValue      * sDataValue;
    
    stlInt32  sParamIdx;
    stlBool   sSuccessWithInfo = STL_FALSE;
    stlBool   sParamError      = STL_FALSE;

    SQLLEN    sOffset = 0;
    SQLRETURN sReturn = SQL_SUCCESS;

    

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aApd != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aIpd != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aParamWithInfo != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aParamError != NULL, aErrorStack );
    
    /**
     * parameter 정보 구축
     */
    
    sParamIdx = 0;
    
    sIpdRec = STL_RING_GET_FIRST_DATA( &aIpd->mDescRecRing );

    STL_RING_FOREACH_ENTRY( &aApd->mDescRecRing, sApdRec )
    {
        sParamIdx++;

        /**
         * bind가 안되어 있음
         */
        
        if( sApdRec->mDataPtr == NULL )
        {
            STL_THROW( RAMP_NEXT_IPD );
        }

        /**
         * OUT parameter 만 설정
         */

        sBindType = sIpdRec->mParameterType;
        
        if ( (sBindType != SQL_PARAM_OUTPUT) &&
             (sBindType != SQL_PARAM_INPUT_OUTPUT) )
        {
            STL_THROW( RAMP_NEXT_IPD );
        }

        sOffset = 0;

        /**
         * IPD 의 Data 공간 획득
         */
        
        sDataValue = DTL_GET_BLOCK_DATA_VALUE( sIpdRec->mOutputValueBlock, aIpdArrayIdx );
            
        /**
         * SQL TYPE -> C TYPE
         */
        
        sBuffer = zliGetDataBuffer( aApd,
                                    sApdRec,
                                    aApdArrayIdx + 1 );

        sIndicator = zliGetIndicatorBuffer( aApd,
                                            sApdRec,
                                            aApdArrayIdx + 1 );

        if ( DTL_IS_NULL(sDataValue) == STL_TRUE )
        {
            if( sIndicator != NULL )
            {
                *sIndicator = SQL_NULL_DATA;
            }
        }
        else
        {
            if( sApdRec->mSqlToCFunc == zlvInvalidSqlToC )
            {
                STL_TRY( zlvGetSqlToCFunc( aStmt,
                                           sDataValue,
                                           sApdRec->mConsiceType,
                                           &sApdRec->mSqlToCFunc,
                                           aErrorStack )
                         == STL_SUCCESS );
            }

            /**
             * @todo ValueBlockList가 DataValue와 동일한지 검증이 필요
             */
            if( sApdRec->mSqlToCFunc( aStmt,
                                      sIpdRec->mOutputValueBlock,
                                      sDataValue,
                                      &sOffset,
                                      sBuffer,
                                      sIndicator,
                                      sApdRec,
                                      &sReturn,
                                      aErrorStack ) == STL_SUCCESS )
            {
                switch( sReturn )
                {
                    case SQL_SUCCESS:
                        break;
                    case SQL_SUCCESS_WITH_INFO:
                        sSuccessWithInfo = STL_TRUE;

                        zldDiagAdds( SQL_HANDLE_STMT,
                                     (void*)aStmt,
                                     aApdArrayIdx + 1,
                                     sParamIdx,
                                     aErrorStack );
                        break;
                    default:
                        STL_ASSERT( STL_FALSE );
                        break;
                }
            }
            else
            {
                zldDiagAdds( SQL_HANDLE_STMT,
                             (void*)aStmt,
                             aApdArrayIdx + 1,
                             sParamIdx,
                             aErrorStack );
                                
                if( aIpd->mArrayStatusPtr != NULL )
                {
                    aIpd->mArrayStatusPtr[aApdArrayIdx] = SQL_PARAM_ERROR;
                }
                                
                sParamError = STL_TRUE;
                break;
            }
        }
                        
        STL_RAMP( RAMP_NEXT_IPD );
        
        sIpdRec = STL_RING_GET_NEXT_DATA( &aIpd->mDescRecRing,
                                          sIpdRec );
    }

    /**
     * Output 설정
     */

    *aParamWithInfo = sSuccessWithInfo;
    *aParamError = sParamError;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus zliDescribeParam( zlsStmt       * aStmt,
                            stlUInt16       aParamIdx,
                            stlInt16      * aDataType,
                            stlUInt64     * aParamSize,
                            stlInt16      * aDecimnalDigits,
                            stlInt16      * aNullable,
                            stlErrorStack * aErrorStack )
{
    /*
     * execute 전까지 정확한 값을 알 수 없어
     * 무조건 VARCHAR(4000)을 전달
     */

    if( aDataType != NULL )
    {
        *aDataType = SQL_VARCHAR;
    }

    if( aParamSize != NULL )
    {
        *aParamSize = 4000;
    }

    if( aDecimnalDigits != NULL )
    {
        *aDecimnalDigits = 0;
    }

    if( aNullable != NULL )
    {
        *aNullable = SQL_NULLABLE;
    }

    return STL_SUCCESS;
}

stlStatus zliInitParamBlock( zlsStmt              * aStmt,
                             zldDesc              * aIpd,
                             zldDescElement       * aApdRec,
                             zldDescElement       * aIpdRec,
                             dtlDataType          * aDtlDataType,
                             stlInt64             * aArg1,
                             stlInt64             * aArg2,
                             dtlStringLengthUnit  * aStringLengthUnit,
                             dtlIntervalIndicator * aIntervalIndicator,
                             stlErrorStack        * aErrorStack )
{
    dtlDataType            sDtlDataType;
    stlInt64               sArg1;
    stlInt64               sArg2;
    dtlStringLengthUnit    sStringLengthUnit;
    dtlIntervalIndicator   sIntervalIndicator;

    dtlValueBlockList    * sBlockList  = NULL;
    dtlValueBlock        * sValueBlock = NULL;
    dtlDataValue         * sDataValue  = NULL;
    void                 * sDataBuffer = NULL;

    stlInt64               sBufferSize = 0;
    stlInt64               sAlignSize = 0;

    stlAllocator         * sAllocator;
    stlInt32               sSize = 0;

    stlInt32               sBlockCount;
    stlInt32               i = 0;
    
    STL_PARAM_VALIDATE( aIpd->mArraySize > 0, aErrorStack );

    sBlockCount = (stlInt32)aIpd->mArraySize;

    zliGetDtlDataArgs( aIpdRec, &sArg1, &sArg2 );

    /*
     * dtlDataValue를 생성 후 해당 타입에 맞는 값 설정
     */

    STL_TRY( zliGetDtlDataType( aIpdRec->mConsiceType,
                                &sDtlDataType,
                                aErrorStack ) == STL_SUCCESS );

    /*
     * 해당 타입에 맞는 string length unit 값을 구한다.
     */ 
    sStringLengthUnit = zliGetDtlDataUnit( aStmt,
                                           aIpdRec,
                                           aApdRec->mConsiceType,
                                           aIpdRec->mConsiceType );

    /*
     * 해당 타입에 맞는 interval indicator 값을 구한다.
     */
    sIntervalIndicator = zliGetDtlDataIndicator( aIpdRec->mConsiceType );

    /*
     * Block Count 만큼 Data Value 정보 설정 
     */

    sAllocator = &aIpdRec->mAllocator;
    sSize = STL_SIZEOF(dtlDataValue) * sBlockCount;
    
    STL_TRY( stlClearRegionMem( sAllocator,
                                aErrorStack ) == STL_SUCCESS );

    /*
     * Data Buffer 크기
     */

    STL_TRY( dtlGetDataValueBufferSize( sDtlDataType,
                                        sArg1,
                                        sStringLengthUnit,
                                        &sBufferSize,
                                        ZLS_STMT_NLS_DT_VECTOR( aStmt ),
                                        (void*)aStmt,
                                        aErrorStack )
             == STL_SUCCESS );

    aIpdRec->mOctetLength = sBufferSize;

    STL_TRY( stlAllocRegionMem( sAllocator,
                                sSize,
                                (void **)&sDataValue,
                                aErrorStack ) == STL_SUCCESS );
    stlMemset( sDataValue, 0x00, sSize );

    for ( i = 0; i < sBlockCount; i++ )
    {
        sDataValue[i].mType   = sDtlDataType;
        sDataValue[i].mLength = 0;
        sDataValue[i].mBufferSize = 0;
        sDataValue[i].mValue  = NULL;
    }

    /*
     * Value Block 정보 설정
     */

    sSize = STL_SIZEOF(dtlValueBlock);
    
    STL_TRY( stlAllocRegionMem( sAllocator,
                                sSize,
                                (void **)&sValueBlock,
                                aErrorStack )
             == STL_SUCCESS );
    stlMemset( sValueBlock, 0x00, sSize );

    sValueBlock->mIsSepBuff         = STL_TRUE;
    sValueBlock->mAllocLayer        = STL_LAYER_GLIESE_LIBRARY;
    sValueBlock->mAllocBlockCnt     = sBlockCount;
    sValueBlock->mUsedBlockCnt      = 0;
    sValueBlock->mSkipBlockCnt      = 0;
    sValueBlock->mArgNum1           = sArg1;
    sValueBlock->mArgNum2           = sArg2;
    sValueBlock->mStringLengthUnit  = sStringLengthUnit;
    sValueBlock->mIntervalIndicator = sIntervalIndicator;
    sValueBlock->mAllocBufferSize   = sBufferSize;
    sValueBlock->mDataValue         = sDataValue;

    sAlignSize = STL_ALIGN_DEFAULT( sBufferSize );

    if( (sDtlDataType == DTL_TYPE_LONGVARCHAR) ||
        (sDtlDataType == DTL_TYPE_LONGVARBINARY) )
    {
        for( i = 0; i < sBlockCount; i++ )
        {
            STL_TRY( stlAllocDynamicMem( &aStmt->mLongVaryingMem,
                                         sAlignSize,
                                         &sDataBuffer,
                                         aErrorStack )
                     == STL_SUCCESS );
            
            sDataValue[i].mBufferSize = sAlignSize;
            sDataValue[i].mValue = sDataBuffer;
        }
    }
    else
    {
        STL_TRY( stlAllocRegionMem( sAllocator,
                                    sAlignSize * sBlockCount,
                                    &sDataBuffer,
                                    aErrorStack )
                 == STL_SUCCESS );
        
        for ( i = 0; i < sBlockCount; i++ )
        {
            sDataValue[i].mBufferSize = sAlignSize;
            sDataValue[i].mValue = sDataBuffer;
            sDataBuffer = (stlChar*)sDataBuffer + sAlignSize;
        }
    }

    /*
     * Block List 정보 설정
     */

    sSize = STL_SIZEOF(dtlValueBlockList);

    STL_TRY( stlAllocRegionMem( sAllocator,
                                sSize,
                                (void **)&sBlockList,
                                aErrorStack )
             == STL_SUCCESS );
    stlMemset( sBlockList, 0x00, sSize );

    sBlockList->mTableID     = 0;
    sBlockList->mColumnOrder = 0;
    sBlockList->mValueBlock = sValueBlock;
    sBlockList->mNext        = NULL;
    
    /*
     * Output 설정
     */

    aIpdRec->mInputValueBlock  = sBlockList;
    aIpdRec->mOutputValueBlock = sBlockList;

    if( aDtlDataType != NULL )
    {
        *aDtlDataType = sDtlDataType;
    }

    if( aArg1 != NULL )
    {
        *aArg1 = sArg1;
    }

    if( aArg2 != NULL )
    {
        *aArg2 = sArg2;
    }

    if( aStringLengthUnit != NULL )
    {
        *aStringLengthUnit = sStringLengthUnit;
    }

    if( aIntervalIndicator != NULL )
    {
        *aIntervalIndicator = sIntervalIndicator;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zliNumParams( zlsStmt       * aStmt,
                        stlInt16      * aParameterCount,
                        stlErrorStack * aErrorStack )
{
    zlcDbc * sDbc = aStmt->mParentDbc;

#if defined( ODBC_ALL )
    STL_TRY( gZliNumParams[sDbc->mProtocolType]( sDbc,
                                                 aStmt,
                                                 aParameterCount,
                                                 aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaiNumParams( sDbc,
                            aStmt,
                            aParameterCount,
                            aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlciNumParams( sDbc,
                            aStmt,
                            aParameterCount,
                            aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zliExtendedDescribeParam( zlsStmt       * aStmt,
                                    stlUInt16       aParamIdx,
                                    stlInt16      * aInputOutputType,
                                    stlInt16      * aDataType,
                                    stlUInt64     * aParamSize,
                                    stlInt16      * aDecimnalDigits,
                                    stlInt16      * aNullable,
                                    stlErrorStack * aErrorStack )
{
    zlcDbc   * sDbc;
    stlInt16   sInputOutputType = SQL_PARAM_TYPE_UNKNOWN;

    sDbc = aStmt->mParentDbc;

    STL_TRY( zliDescribeParam( aStmt,
                               aParamIdx,
                               aDataType,
                               aParamSize,
                               aDecimnalDigits,
                               aNullable,
                               aErrorStack ) == STL_SUCCESS );

#if defined( ODBC_ALL )
    STL_TRY( gZliGetParameterType[sDbc->mProtocolType]( sDbc,
                                                        aStmt,
                                                        aParamIdx,
                                                        &sInputOutputType,
                                                        aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaiGetParameterType( sDbc,
                                   aStmt,
                                   aParamIdx,
                                   &sInputOutputType,
                                   aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlciGetParameterType( sDbc,
                                   aStmt,
                                   aParamIdx,
                                   &sInputOutputType,
                                   aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif

    if( aInputOutputType != NULL )
    {
        *aInputOutputType = sInputOutputType;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
