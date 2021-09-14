/*******************************************************************************
 * dtfDateTimeMultiplication.c
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

/**
 * @file dtfDateTimeMultiplication.c
 * @brief Date and Time Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfNumeric.h>

/**
 * @addtogroup dtfDateTime Date and Time
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * multiplication ( * )
 *   ( * ) => [ S, P, O ]
 * ex) interval '1'month * 3 =>  interval'0-3'year to month
 *******************************************************************************/

/**
 * @brief multiplication ( * ) : interval year to month * bigint
 *        <BR> multiplication ( * )
 *        <BR>   ( * ) => [ S, P, O ]
 *        <BR> ex) interval '1'month * 3 =>  interval'0-3'year to month
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL YEAR TO MONTH * BIGINT
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 INTERVAL YEAR TO MONTH)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalYearToMonthMulBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlIntervalYearToMonthType  sIntervalYearToMonth;
    dtlBigIntType               sBigInt;
    stlInt64                    sTempValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1         = aInputArgument[0].mValue.mDataValue;
    sValue2         = aInputArgument[1].mValue.mDataValue;
    sResultValue    = (dtlDataValue *)aResultValue;
    sErrorStack     = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );

    /* 초기화 */
    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_NA;
    sIntervalYearToMonth.mMonth = 0;

    sBigInt = *((dtlBigIntType *)sValue2->mValue);

    /* 곱셈 */
    if( (((dtlIntervalYearToMonthType *)sValue1->mValue)->mMonth == 0) ||
        sBigInt == 0 )
    {
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mIndicator
            = ((dtlIntervalYearToMonthType *)sValue1->mValue)->mIndicator;
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mMonth = 0;
        sResultValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    }
    else
    {
        STL_TRY_THROW( (sBigInt >= STL_INT32_MIN) && (sBigInt <= STL_INT32_MAX),
                       ERROR_OUT_OF_RANGE );
        sTempValue = (stlInt64)((dtlIntervalYearToMonthType *)sValue1->mValue)->mMonth * (stlInt64)sBigInt;
        STL_TRY_THROW( (sTempValue >= STL_INT32_MIN) && (sTempValue <= STL_INT32_MAX),
                       ERROR_OUT_OF_RANGE );

        switch( ((dtlIntervalYearToMonthType *)sValue1->mValue)->mIndicator )
        {
            case DTL_INTERVAL_INDICATOR_YEAR:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_YEAR;
                    sIntervalYearToMonth.mMonth
                        = ((stlInt32)sTempValue / DTL_MONTHS_PER_YEAR) * DTL_MONTHS_PER_YEAR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MONTH:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_MONTH;
                    sIntervalYearToMonth.mMonth = (stlInt32)sTempValue;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH;
                    sIntervalYearToMonth.mMonth = (stlInt32)sTempValue;
                    break;
                }
            default:
                STL_DASSERT( 0 );
                break;
        }

        /* 결과 타입의 precision 체크 */
        STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonth,
                                               sIntervalYearToMonth.mIndicator,
                                               DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                               sErrorStack )
                 == STL_SUCCESS );

        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mIndicator
            = sIntervalYearToMonth.mIndicator;
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mMonth = sIntervalYearToMonth.mMonth;
        sResultValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief multiplication ( * ) : bigint * interval year to month 
 *        <BR> multiplication ( * )
 *        <BR>   ( * ) => [ S, P, O ]
 *        <BR> ex) interval '1'month * 3 =>  interval'0-3'year to month
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> BIGINT * INTERVAL YEAR TO MONTH
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 INTERVAL YEAR TO MONTH)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntMulIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlIntervalYearToMonthType  sIntervalYearToMonth;
    dtlBigIntType               sBigInt;
    stlInt64                    sTempValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1         = aInputArgument[0].mValue.mDataValue;
    sValue2         = aInputArgument[1].mValue.mDataValue;
    sResultValue    = (dtlDataValue *)aResultValue;
    sErrorStack     = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );

    /* 초기화 */
    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_NA;
    sIntervalYearToMonth.mMonth = 0;

    sBigInt = *((dtlBigIntType *)sValue1->mValue);

    /* 곱셈 */
    if( (((dtlIntervalYearToMonthType *)sValue2->mValue)->mMonth == 0) ||
        sBigInt == 0 )
    {
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mIndicator
            = ((dtlIntervalYearToMonthType *)sValue2->mValue)->mIndicator;
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mMonth = 0;
        sResultValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    }
    else
    {
        STL_TRY_THROW( (sBigInt >= STL_INT32_MIN) && (sBigInt <= STL_INT32_MAX),
                       ERROR_OUT_OF_RANGE );
        sTempValue = (stlInt64)((dtlIntervalYearToMonthType *)sValue2->mValue)->mMonth * (stlInt64)sBigInt;
        STL_TRY_THROW( (sTempValue >= STL_INT32_MIN) && (sTempValue <= STL_INT32_MAX),
                       ERROR_OUT_OF_RANGE );

        switch( ((dtlIntervalYearToMonthType *)sValue2->mValue)->mIndicator )
        {
            case DTL_INTERVAL_INDICATOR_YEAR:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_YEAR;
                    sIntervalYearToMonth.mMonth
                        = ((stlInt32)sTempValue / DTL_MONTHS_PER_YEAR) * DTL_MONTHS_PER_YEAR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MONTH:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_MONTH;
                    sIntervalYearToMonth.mMonth = (stlInt32)sTempValue;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH;
                    sIntervalYearToMonth.mMonth = (stlInt32)sTempValue;
                    break;
                }
            default:
                STL_DASSERT( 0 );
                break;
        }

        /* 결과 타입의 precision 체크 */
        STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonth,
                                               sIntervalYearToMonth.mIndicator,
                                               DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                               sErrorStack )
                 == STL_SUCCESS );

        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mIndicator
            = sIntervalYearToMonth.mIndicator;
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mMonth = sIntervalYearToMonth.mMonth;
        sResultValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief multiplication ( * ) : interval year to month * numeric
 *        <BR> multiplication ( * )
 *        <BR>   ( * ) => [ S, P, O ]
 *        <BR> ex) interval '1'month * 3.5 =>  interval'0-3'year to month
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL YEAR TO MONTH * NUMERIC
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 INTERVAL YEAR TO MONTH)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalYearToMonthMulNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlIntervalYearToMonthType  sIntervalYearToMonth = { 0, 0 };

    dtlDataValue        sNumeric1;
    dtlDataValue        sNumericResult;
    stlInt64            sTempValue;
    stlBool             sIsTruncated;

    stlUInt8            sNumericInterval[DTL_NUMERIC_MAX_SIZE];
    stlUInt8            sNumericMulResult[DTL_NUMERIC_MAX_SIZE];

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1         = aInputArgument[0].mValue.mDataValue;
    sValue2         = aInputArgument[1].mValue.mDataValue;
    sResultValue    = (dtlDataValue *)aResultValue;
    sErrorStack     = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );

    /* 초기화 */
    sNumeric1.mType   = DTL_TYPE_NUMBER;
    sNumeric1.mLength = 0;
    sNumeric1.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumeric1.mValue  = (dtlNumericType *)(&sNumericInterval);
    
    sNumericResult.mType   = DTL_TYPE_NUMBER;
    sNumericResult.mLength = 0;
    sNumericResult.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumericResult.mValue  = (dtlNumericType *)(&sNumericMulResult);
    
    /*
     * 곱셈
     */
    
    if( (((dtlIntervalYearToMonthType *)sValue1->mValue)->mMonth == 0) ||
        (DTL_NUMERIC_IS_ZERO( DTF_NUMERIC( sValue2 ), sValue2->mLength ) == STL_TRUE) )
    {
        /* 0 * value 또는 value * 0  => 0 */
        
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mIndicator
            = ((dtlIntervalYearToMonthType *)sValue1->mValue)->mIndicator;
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mMonth = 0;
        sResultValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    }
    else
    {
        /* Interval type value를 Numeric type value로 변환 후, 곱셈 연산 수행 */

        /* Interval type value를 Numeric type value로 변환 */
        STL_TRY( dtdToNumericFromInt64(
                     (stlInt64)(((dtlIntervalYearToMonthType *)sValue1->mValue)->mMonth),
                     DTL_NUMERIC_MAX_PRECISION,
                     0,
                     & sNumeric1,
                     sErrorStack )
                 == STL_SUCCESS );

        /* 곱셈 */
        STL_TRY( dtlNumericMultiplication( & sNumeric1,
                                           sValue2,
                                           & sNumericResult,
                                           sErrorStack )
                 == STL_SUCCESS );
        
        /* 곱셈 결과를 Int64 value로 변환 */       
        STL_TRY( dtlNumericToInt64( & sNumericResult,
                                    & sTempValue,
                                    & sIsTruncated,
                                    sErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( (sTempValue >= STL_INT32_MIN) && (sTempValue <= STL_INT32_MAX),
                       ERROR_OUT_OF_RANGE );

        switch( ((dtlIntervalYearToMonthType *)sValue1->mValue)->mIndicator )
        {
            case DTL_INTERVAL_INDICATOR_YEAR:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_YEAR;
                    sIntervalYearToMonth.mMonth
                        = ((stlInt32)sTempValue / DTL_MONTHS_PER_YEAR) * DTL_MONTHS_PER_YEAR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MONTH:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_MONTH;
                    sIntervalYearToMonth.mMonth = (stlInt32)sTempValue;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH;
                    sIntervalYearToMonth.mMonth = (stlInt32)sTempValue;
                    break;
                }
            default:
                STL_DASSERT( 0 );
                break;
        }

        /* 결과 타입의 precision 체크 */
        STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonth,
                                               sIntervalYearToMonth.mIndicator,
                                               DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                               sErrorStack )
                 == STL_SUCCESS );

        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mIndicator
            = sIntervalYearToMonth.mIndicator;    
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mMonth = sIntervalYearToMonth.mMonth;
        sResultValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief multiplication ( * ) : interval year to month * numeric
 *        <BR> multiplication ( * )
 *        <BR>   ( * ) => [ S, P, O ]
 *        <BR> ex) interval '1'month * 3.5 =>  interval'0-3'year to month
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> NUMERIC * INTERVAL YEAR TO MONTH
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 INTERVAL YEAR TO MONTH)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericMulIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlIntervalYearToMonthType  sIntervalYearToMonth = { 0, 0 };

    dtlDataValue        sNumeric1;
    dtlDataValue        sNumericResult;
    stlInt64            sTempValue;
    stlBool             sIsTruncated;

    stlUInt8            sNumericInterval[DTL_NUMERIC_MAX_SIZE];
    stlUInt8            sNumericMulResult[DTL_NUMERIC_MAX_SIZE];

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1         = aInputArgument[0].mValue.mDataValue;
    sValue2         = aInputArgument[1].mValue.mDataValue;
    sResultValue    = (dtlDataValue *)aResultValue;
    sErrorStack     = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );    
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );

    /* 초기화 */
    sNumeric1.mType   = DTL_TYPE_NUMBER;
    sNumeric1.mLength = 0;
    sNumeric1.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumeric1.mValue  = (dtlNumericType *)(&sNumericInterval);
    
    sNumericResult.mType   = DTL_TYPE_NUMBER;
    sNumericResult.mLength = 0;
    sNumericResult.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumericResult.mValue  = (dtlNumericType *)(&sNumericMulResult);
    
    /*
     * 곱셈
     */
    
    if( (((dtlIntervalYearToMonthType *)sValue2->mValue)->mMonth == 0) ||
        (DTL_NUMERIC_IS_ZERO( DTF_NUMERIC( sValue1 ), sValue1->mLength ) == STL_TRUE) )
    {
        /* 0 * value 또는 value * 0  => 0 */
        
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mIndicator
            = ((dtlIntervalYearToMonthType *)sValue2->mValue)->mIndicator;
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mMonth = 0;
        sResultValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    }
    else
    {
        /* Interval type value를 Numeric type value로 변환 후, 곱셈 연산 수행 */
        
        /* Interval type value를 Numeric type value로 변환 */
        STL_TRY( dtdToNumericFromInt64(
                     (stlInt64)(((dtlIntervalYearToMonthType *)sValue2->mValue)->mMonth),
                     DTL_NUMERIC_MAX_PRECISION,
                     0,
                     & sNumeric1,
                     sErrorStack )
                 == STL_SUCCESS );
        
        /* 곱셈 */
        STL_TRY( dtlNumericMultiplication( & sNumeric1,
                                           sValue1,
                                           & sNumericResult,
                                           sErrorStack )
                 == STL_SUCCESS );
        
        /* 곱셈 결과를 Int64 value로 변환 */       
        STL_TRY( dtlNumericToInt64( & sNumericResult,
                                    & sTempValue,
                                    & sIsTruncated,
                                    sErrorStack )
                 == STL_SUCCESS );

        STL_TRY_THROW( (sTempValue >= STL_INT32_MIN) && (sTempValue <= STL_INT32_MAX),
                       ERROR_OUT_OF_RANGE );

        switch( ((dtlIntervalYearToMonthType *)sValue2->mValue)->mIndicator )
        {
            case DTL_INTERVAL_INDICATOR_YEAR:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_YEAR;
                    sIntervalYearToMonth.mMonth
                        = ((stlInt32)sTempValue / DTL_MONTHS_PER_YEAR) * DTL_MONTHS_PER_YEAR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MONTH:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_MONTH;
                    sIntervalYearToMonth.mMonth = (stlInt32)sTempValue;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                {
                    sIntervalYearToMonth.mIndicator = DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH;
                    sIntervalYearToMonth.mMonth = (stlInt32)sTempValue;
                    break;
                }
            default:
                STL_DASSERT( 0 );
                break;
        }

        /* 결과 타입의 precision 체크 */
        STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonth,
                                               sIntervalYearToMonth.mIndicator,
                                               DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                               sErrorStack )
                 == STL_SUCCESS );

        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mIndicator
            = sIntervalYearToMonth.mIndicator;    
        ((dtlIntervalYearToMonthType *)sResultValue->mValue)->mMonth = sIntervalYearToMonth.mMonth;
        sResultValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * multiplication ( * )
 *   ( * ) => [ S, P, O ]
 * ex) interval '1' hour * 3  =>  interval '0 03:00:00'day to second
 *******************************************************************************/

/**
 * @brief multiplication ( * ) : interval day to second * bigint
 *        <BR> multiplication ( * )
 *        <BR>   ( * ) => [ S, P, O ]
 *        <BR> ex) interval '1' hour * 3  =>  interval '0 03:00:00'day to second
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL DAY TO SECOND * BIGINT
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 INTERVAL DAY TO SECOND)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalDayToSecondMulBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlIntervalDayToSecondType  sIntervalDayToSecond = { 0, 0, 0 };
    dtlBigIntType               sBigInt;
    stlInt64                    sTime;
    stlBool                     sIsNegativeTimeResult;
    stlUInt128                  sTimeResult;
    stlInt64                    sTempValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1         = aInputArgument[0].mValue.mDataValue;
    sValue2         = aInputArgument[1].mValue.mDataValue;
    sResultValue    = (dtlDataValue *)aResultValue;
    sErrorStack     = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    sTime = ( ((dtlIntervalDayToSecondType *)sValue1->mValue)->mDay * DTL_USECS_PER_DAY ) +
            ((dtlIntervalDayToSecondType *)sValue1->mValue)->mTime;

    sBigInt = *((dtlBigIntType *)sValue2->mValue);

    if( (sTime == 0) || (sBigInt == 0) )
    {
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mIndicator
            = ((dtlIntervalDayToSecondType *)sValue1->mValue)->mIndicator;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mDay = 0;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mTime = 0;
        sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    }
    else
    {
        if( sTime < 0 )
        {
            if( sBigInt < 0 )
            {
                sIsNegativeTimeResult = STL_FALSE;
                sBigInt = -sBigInt;
            }
            else
            {
                sIsNegativeTimeResult = STL_TRUE;
            }
            sTime = -sTime;
        }
        else
        {
            if( sBigInt < 0 )
            {
                sIsNegativeTimeResult = STL_TRUE;
                sBigInt = -sBigInt;
            }
            else
            {
                sIsNegativeTimeResult = STL_FALSE;
            }
        }

        /* 곱셈 */
        STL_TRY_THROW( stlMulUInt64( (stlUInt64)sTime, (stlUInt64)sBigInt, &sTimeResult )
                       == STL_TRUE,
                       ERROR_OUT_OF_RANGE );

        STL_TRY_THROW( (sTimeResult.mHighDigit == 0) && (sTimeResult.mLowDigit <= STL_INT64_MAX),
                       ERROR_OUT_OF_RANGE );

        if( sIsNegativeTimeResult == STL_TRUE )
        {
            sTempValue = -(stlInt64)(sTimeResult.mLowDigit);
        }
        else
        {
            sTempValue = (stlInt64)(sTimeResult.mLowDigit);
        }

        switch( ((dtlIntervalDayToSecondType *)sValue1->mValue)->mIndicator )
        {
            case DTL_INTERVAL_INDICATOR_DAY:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY;
                    sTempValue = (sTempValue / DTL_USECS_PER_DAY) * DTL_USECS_PER_DAY;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR;
                    sTempValue = (sTempValue / DTL_USECS_PER_HOUR) * DTL_USECS_PER_HOUR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_HOUR;
                    sTempValue = (sTempValue / DTL_USECS_PER_HOUR) * DTL_USECS_PER_HOUR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND;
                    break;
                }
            default:
                STL_DASSERT( 0 );
                break;
        }

        sIntervalDayToSecond.mDay       = sTempValue / DTL_USECS_PER_DAY;
        sIntervalDayToSecond.mTime      = sTempValue % DTL_USECS_PER_DAY;

        /* 결과 타입의 precision 체크 */
        STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecond,
                                               sIntervalDayToSecond.mIndicator,
                                               DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                               DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,
                                               sErrorStack )
                 == STL_SUCCESS );

        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mIndicator
            = sIntervalDayToSecond.mIndicator;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mDay = sIntervalDayToSecond.mDay;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mTime = sIntervalDayToSecond.mTime;
        sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_DT_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief multiplication ( * ) : bigint * interval day to second
 *        <BR> multiplication ( * )
 *        <BR>   ( * ) => [ S, P, O ]
 *        <BR> ex) interval '1' hour * 3  =>  interval '0 03:00:00'day to second
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> BIGINT * INTERVAL DAY TO SECOND
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 INTERVAL DAY TO SECOND)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntMulIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlIntervalDayToSecondType  sIntervalDayToSecond = { 0, 0, 0 };
    dtlBigIntType               sBigInt;
    stlInt64                    sTime;
    stlBool                     sIsNegativeTimeResult;
    stlUInt128                  sTimeResult;
    stlInt64                    sTempValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1         = aInputArgument[0].mValue.mDataValue;
    sValue2         = aInputArgument[1].mValue.mDataValue;
    sResultValue    = (dtlDataValue *)aResultValue;
    sErrorStack     = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );    
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    sTime = ( ((dtlIntervalDayToSecondType *)sValue2->mValue)->mDay * DTL_USECS_PER_DAY ) +
            ((dtlIntervalDayToSecondType *)sValue2->mValue)->mTime;

    sBigInt = *((dtlBigIntType *)sValue1->mValue);

    if( (sTime == 0) || (sBigInt == 0) )
    {
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mIndicator
            = ((dtlIntervalDayToSecondType *)sValue2->mValue)->mIndicator;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mDay = 0;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mTime = 0;
        sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    }
    else
    {
        if( sTime < 0 )
        {
            if( sBigInt < 0 )
            {
                sIsNegativeTimeResult = STL_FALSE;
                sBigInt = -sBigInt;
            }
            else
            {
                sIsNegativeTimeResult = STL_TRUE;
            }
            sTime = -sTime;
        }
        else
        {
            if( sBigInt < 0 )
            {
                sIsNegativeTimeResult = STL_TRUE;
                sBigInt = -sBigInt;
            }
            else
            {
                sIsNegativeTimeResult = STL_FALSE;
            }
        }

        /* 곱셈 */
        STL_TRY_THROW( stlMulUInt64( (stlUInt64)sTime, (stlUInt64)sBigInt, &sTimeResult )
                       == STL_TRUE,
                       ERROR_OUT_OF_RANGE );

        STL_TRY_THROW( (sTimeResult.mHighDigit == 0) && (sTimeResult.mLowDigit <= STL_INT64_MAX),
                       ERROR_OUT_OF_RANGE );

        if( sIsNegativeTimeResult == STL_TRUE )
        {
            sTempValue = -(stlInt64)(sTimeResult.mLowDigit);
        }
        else
        {
            sTempValue = (stlInt64)(sTimeResult.mLowDigit);
        }

        switch( ((dtlIntervalDayToSecondType *)sValue2->mValue)->mIndicator )
        {
            case DTL_INTERVAL_INDICATOR_DAY:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY;
                    sTempValue = (sTempValue / DTL_USECS_PER_DAY) * DTL_USECS_PER_DAY;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR;
                    sTempValue = (sTempValue / DTL_USECS_PER_HOUR) * DTL_USECS_PER_HOUR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_HOUR;
                    sTempValue = (sTempValue / DTL_USECS_PER_HOUR) * DTL_USECS_PER_HOUR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND;
                    break;
                }
            default:
                STL_DASSERT( 0 );
                break;
        }

        sIntervalDayToSecond.mDay       = sTempValue / DTL_USECS_PER_DAY;
        sIntervalDayToSecond.mTime      = sTempValue % DTL_USECS_PER_DAY;

        /* 결과 타입의 precision 체크 */
        STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecond,
                                               sIntervalDayToSecond.mIndicator,
                                               DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                               DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,
                                               sErrorStack )
                 == STL_SUCCESS );

        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mIndicator
            = sIntervalDayToSecond.mIndicator;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mDay = sIntervalDayToSecond.mDay;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mTime = sIntervalDayToSecond.mTime;
        sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_DT_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief multiplication ( * ) : interval day to second * numeric
 *        <BR> multiplication ( * )
 *        <BR>   ( * ) => [ S, P, O ]
 *        <BR> ex) interval '1' hour * 3.5  =>  interval '0 03:30:00'day to second
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> INTERVAL DAY TO SECOND * NUMERIC
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 INTERVAL DAY TO SECOND)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntervalDayToSecondMulNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlIntervalDayToSecondType  sIntervalDayToSecond = { 0, 0, 0 };

    dtlDataValue        sNumeric1;
    dtlDataValue        sNumericMulResult;
    dtlDataValue        sNumericResult;
    stlInt64            sTempValue;
    stlBool             sIsTruncated;
    
    stlUInt8            sNumericTemp[DTL_NUMERIC_MAX_SIZE];
    stlUInt8            sNumericMul[DTL_NUMERIC_MAX_SIZE];
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1         = aInputArgument[0].mValue.mDataValue;
    sValue2         = aInputArgument[1].mValue.mDataValue;
    sResultValue    = (dtlDataValue *)aResultValue;
    sErrorStack     = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    
    /* 초기화 */
    sNumeric1.mType   = DTL_TYPE_NUMBER;
    sNumeric1.mLength = 0;
    sNumeric1.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumeric1.mValue  = (dtlNumericType *)(&sNumericTemp);
    
    sNumericMulResult.mType   = DTL_TYPE_NUMBER;
    sNumericMulResult.mLength = 0;
    sNumericMulResult.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumericMulResult.mValue  = (dtlNumericType *)(&sNumericMul);

    sNumericResult.mType   = DTL_TYPE_NUMBER;
    sNumericResult.mLength = 0;
    sNumericResult.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumericResult.mValue  = (dtlNumericType *)(&sNumericTemp);
    
    /*
     * 곱셈
     */
    
    sTempValue = ( ((dtlIntervalDayToSecondType *)sValue1->mValue)->mDay * DTL_USECS_PER_DAY ) +
        ((dtlIntervalDayToSecondType *)sValue1->mValue)->mTime;

    /* 곱셈 */
    if( (sTempValue == 0) ||
        (DTL_NUMERIC_IS_ZERO( DTF_NUMERIC( sValue2 ), sValue2->mLength ) == STL_TRUE) )
    {
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mIndicator
            = ((dtlIntervalDayToSecondType *)sValue1->mValue)->mIndicator;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mDay = 0;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mTime = 0;
        sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    }
    else
    {
        /* Interval type value를 Numeric type value로 변환 후, 곱셈 연산 수행 */

        /* Interval type value를 Numeric type value로 변환 */
        STL_TRY( dtdToNumericFromInt64( sTempValue,
                                        DTL_NUMERIC_MAX_PRECISION,
                                        0,
                                        & sNumeric1,
                                        sErrorStack )
                 == STL_SUCCESS );
        
        /* 곱셈 */
        STL_TRY( dtlNumericMultiplication( & sNumeric1,
                                           sValue2,
                                           & sNumericMulResult,
                                           sErrorStack )
                 == STL_SUCCESS );

        /*  반올림 round( sNumericMulResult, 0 ) */
        STL_TRY( dtfNumericRounding( & sNumericMulResult,
                                     0,
                                     & sNumericResult,
                                     sErrorStack )
                 == STL_SUCCESS );
        
        /* 나눗셈 결과를 Int64 value로 변환
         * 소수점이하 반올림하지 않음. */        
        STL_TRY( dtlNumericToInt64( & sNumericResult,
                                    & sTempValue,
                                    & sIsTruncated,
                                    sErrorStack )
                 == STL_SUCCESS );

        switch( ((dtlIntervalDayToSecondType *)sValue1->mValue)->mIndicator )
        {
            case DTL_INTERVAL_INDICATOR_DAY:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY;
                    sTempValue = (sTempValue / DTL_USECS_PER_DAY) * DTL_USECS_PER_DAY;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR;
                    sTempValue = (sTempValue / DTL_USECS_PER_HOUR) * DTL_USECS_PER_HOUR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_HOUR;
                    sTempValue = (sTempValue / DTL_USECS_PER_HOUR) * DTL_USECS_PER_HOUR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND;
                    break;
                }
            default:
                STL_DASSERT( 0 );
                break;
        }

        sIntervalDayToSecond.mDay       = sTempValue / DTL_USECS_PER_DAY;
        sIntervalDayToSecond.mTime      = sTempValue % DTL_USECS_PER_DAY;

        /* 결과 타입의 precision 체크 */
        STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecond,
                                               sIntervalDayToSecond.mIndicator,
                                               DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                               DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,
                                               sErrorStack )
                 == STL_SUCCESS );

        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mIndicator
            = sIntervalDayToSecond.mIndicator;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mDay = sIntervalDayToSecond.mDay;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mTime = sIntervalDayToSecond.mTime;
        sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief multiplication ( * ) : numeric * interval day to second
 *        <BR> multiplication ( * )
 *        <BR>   ( * ) => [ S, P, O ]
 *        <BR> ex) interval '1' hour * 3.5  =>  interval '0 03:30:00'day to second
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> NUMERIC * INTERVAL DAY TO SECOND
 * @param[out] aResultValue      덧셈 연산 결과 (결과타입 INTERVAL DAY TO SECOND)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericMulIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlIntervalDayToSecondType  sIntervalDayToSecond = { 0, 0, 0 };

    dtlDataValue        sNumeric1;
    dtlDataValue        sNumericMulResult;
    dtlDataValue        sNumericResult;
    stlInt64            sTempValue;
    stlBool             sIsTruncated;
    
    stlUInt8            sNumericTemp[DTL_NUMERIC_MAX_SIZE];
    stlUInt8            sNumericMul[DTL_NUMERIC_MAX_SIZE];

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1         = aInputArgument[0].mValue.mDataValue;
    sValue2         = aInputArgument[1].mValue.mDataValue;
    sResultValue    = (dtlDataValue *)aResultValue;
    sErrorStack     = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );    
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );
    
    /* 초기화 */
    sNumeric1.mType   = DTL_TYPE_NUMBER;
    sNumeric1.mLength = 0;
    sNumeric1.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumeric1.mValue  = (dtlNumericType *)(&sNumericTemp);
    
    sNumericMulResult.mType   = DTL_TYPE_NUMBER;
    sNumericMulResult.mLength = 0;
    sNumericMulResult.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumericMulResult.mValue  = (dtlNumericType *)(&sNumericMul);

    sNumericResult.mType   = DTL_TYPE_NUMBER;
    sNumericResult.mLength = 0;
    sNumericResult.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumericResult.mValue  = (dtlNumericType *)(&sNumericTemp);
    
    /*
     * 곱셈
     */
    
    sTempValue = ( ((dtlIntervalDayToSecondType *)sValue2->mValue)->mDay * DTL_USECS_PER_DAY ) +
        ((dtlIntervalDayToSecondType *)sValue2->mValue)->mTime;
    
    /* 곱셈 */
    if( (sTempValue == 0) ||
        (DTL_NUMERIC_IS_ZERO( DTF_NUMERIC( sValue1 ), sValue1->mLength ) == STL_TRUE) )
    {
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mIndicator
            = ((dtlIntervalDayToSecondType *)sValue2->mValue)->mIndicator;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mDay = 0;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mTime = 0;
        sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    }
    else
    {
        /* Interval type value를 Numeric type value로 변환 후, 곱셈 연산 수행 */
        
        /* Interval type value를 Numeric type value로 변환 */
        STL_TRY( dtdToNumericFromInt64( sTempValue,
                                        DTL_NUMERIC_MAX_PRECISION,
                                        0,
                                        & sNumeric1,
                                        sErrorStack )
                 == STL_SUCCESS );
        
        /* 곱셈 */
        STL_TRY( dtlNumericMultiplication( & sNumeric1,
                                           sValue1,
                                           & sNumericMulResult,
                                           sErrorStack )
                 == STL_SUCCESS );

        /*  반올림 round( sNumericMulResult, 0 ) */
        STL_TRY( dtfNumericRounding( & sNumericMulResult,
                                     0,
                                     & sNumericResult,
                                     sErrorStack )
                 == STL_SUCCESS );
        
        /* 나눗셈 결과를 Int64 value로 변환
         * 소수점이하 반올림하지 않음. */        
        STL_TRY( dtlNumericToInt64( & sNumericResult,
                                    & sTempValue,
                                    & sIsTruncated,
                                    sErrorStack )
                 == STL_SUCCESS );

        switch( ((dtlIntervalDayToSecondType *)sValue2->mValue)->mIndicator )
        {
            case DTL_INTERVAL_INDICATOR_DAY:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY;
                    sTempValue = (sTempValue / DTL_USECS_PER_DAY) * DTL_USECS_PER_DAY;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR;
                    sTempValue = (sTempValue / DTL_USECS_PER_HOUR) * DTL_USECS_PER_HOUR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_HOUR;
                    sTempValue = (sTempValue / DTL_USECS_PER_HOUR) * DTL_USECS_PER_HOUR;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE;
                    sTempValue = (sTempValue / DTL_USECS_PER_MINUTE) * DTL_USECS_PER_MINUTE;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND;
                    break;
                }
            case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                {
                    sIntervalDayToSecond.mIndicator = DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND;
                    break;
                }
            default:
                STL_DASSERT( 0 );
                break;
        }

        sIntervalDayToSecond.mDay       = sTempValue / DTL_USECS_PER_DAY;
        sIntervalDayToSecond.mTime      = sTempValue % DTL_USECS_PER_DAY;

        /* 결과 타입의 precision 체크 */
        STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecond,
                                               sIntervalDayToSecond.mIndicator,
                                               DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,
                                               DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,
                                               sErrorStack )
                 == STL_SUCCESS );

        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mIndicator
            = sIntervalDayToSecond.mIndicator;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mDay = sIntervalDayToSecond.mDay;
        ((dtlIntervalDayToSecondType *)sResultValue->mValue)->mTime = sIntervalDayToSecond.mTime;
        sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
