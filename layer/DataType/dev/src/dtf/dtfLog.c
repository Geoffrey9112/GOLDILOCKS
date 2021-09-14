/*******************************************************************************
 * dtfLog.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfLog.c 1296 2011-06-30 07:24:30Z ehpark $
 *
 * NOTES
 *   1. BigInt, Double, Numeric공통.
 *      함수의 피연산자에 NULL값이 오지 않는다고 가정하고 구현되어 있다.
 *   2. Double형 연산 관련.
 *      현재 NaN은 고려되어 있지 않다.
 *      infinite 값이 들어오면 argument out of range 로 처리한다.
 *   3. Numeric형 연산 관련.
 *      numeric은 precision, scale과 연관되어 검증되어야 함으로 상위단에서
 *      세밀하게 검증이 필요함.
 *
 ******************************************************************************/

/**
 * @file dtfLog.c
 * @brief dtfLog Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfMathematical.h>
#include <dtfNumeric.h>



/**
 * @addtogroup dtfLog Log
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * ln(x) natural logarithm
 *   ln(x) => [ P, S, M, O ]
 * ex) ln(2.0)  =>  0.693147180559945
 ******************************************************************************/

/**
 * @brief ln 함수 natural logarithm
 *        <BR> ln(x) natural logarithm
 *        <BR>   ln(x) => [ P, S, M, O ]
 *        <BR> ex) ln(2.0)  =>  0.693147180559945
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ln(BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntLn( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    if( DTF_BIGINT( sValue1 ) <= 0 )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* log 연산을 한다. */
    DTF_BIGINT( sResultValue ) = (stlInt64) ( stlLog( DTF_BIGINT( sValue1 ) ) + 0.5 );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;


    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      "LN" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ln 함수 natural logarithm
 *        <BR> ln(x) natural logarithm
 *        <BR>   ln(x) => [ P, S, M, O ]
 *        <BR> ex) ln(2.0)  =>  0.693147180559945
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ln(NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericLn( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    stlFloat64        sDouble;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* numeric -> double */
    STL_TRY( dtdToFloat64FromNumeric( sValue1,
                                      & sDouble,
                                      sErrorStack )
             == STL_SUCCESS );

    if( sDouble <= 0.0 )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* log 연산을 한다. */
    STL_TRY( dtdToNumericFromFloat64( stlLog( sDouble ),
                                      DTL_NUMERIC_MAX_PRECISION,
                                      DTL_SCALE_NA,
                                      sResultValue,
                                      sErrorStack )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      "LN" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ln 함수 natural logarithm
 *        <BR> ln(x) natural logarithm
 *        <BR>   ln(x) => [ P, S, M, O ]
 *        <BR> ex) ln(2.0)  =>  0.693147180559945
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ln(DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleLn( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* Double의 경우는 input이 finite인지 확인한다. */
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sValue1 ) )
                 == STL_IS_INFINITE_FALSE );

    if( DTF_DOUBLE( sValue1 ) <= 0.0 )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* log 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = stlLog( DTF_DOUBLE( sValue1 ) );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;


    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      "LN" );        
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/*******************************************************************************
 * log(x) base 10 logarithm
 *   log(x) => [ P ]
 * ex) log(100.0)  =>  2
 ******************************************************************************/

/**
 * @brief log 함수 base 10 logarithm
 *        <BR> log(x) base 10 logarithm
 *        <BR>   log(x) => [ P ]
 *        <BR> ex) log(100.0)  =>  2
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> log(BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntLog10( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    if( DTF_BIGINT( sValue1 ) <= 0 )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* log 연산을 한다. */
    DTF_BIGINT( sResultValue ) = (stlInt64) stlLog10( DTF_BIGINT( sValue1 ) );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;


    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      "LOG" );        
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief log 함수 base 10 logarithm
 *        <BR> log(x) base 10 logarithm
 *        <BR>   log(x) => [ P ]
 *        <BR> ex) log(100.0)  =>  2
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> log(NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericLog10( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    stlFloat64        sDouble;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* numeric -> double */
    STL_TRY( dtdToFloat64FromNumeric( sValue1,
                                      & sDouble,
                                      sErrorStack )
             == STL_SUCCESS );

    if( sDouble <= 0.0 )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* log 연산을 한다. */
    STL_TRY( dtdToNumericFromFloat64( stlLog10( sDouble ),
                                      DTL_NUMERIC_MAX_PRECISION,
                                      DTL_SCALE_NA,
                                      sResultValue,
                                      sErrorStack )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      "LOG" );                
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief log 함수 base 10 logarithm
 *        <BR> log(x) base 10 logarithm
 *        <BR>   log(x) => [ P ]
 *        <BR> ex) log(100.0)  =>  2
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> log(DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleLog10( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* Double의 경우는 input이 finite인지 확인한다. */
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sValue1 ) )
                 == STL_IS_INFINITE_FALSE );

    if( DTF_DOUBLE( sValue1 ) <= 0.0 )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* log10 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = stlLog10( DTF_DOUBLE( sValue1 ) );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;


    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      "LOG" );                
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * log(x, y) logarithm to base b
 *   log(x, y) => [ P, M, 0 ]
 * ex) log(2.0, 64.0)  =>  6.0000000000
 ******************************************************************************/

/**
 * @brief log(x, y) logarithm to base b
 *        <BR> log(x, y) logarithm to base b
 *        <BR>   log(x, y) => [ P, M, 0 ]
 *        <BR> ex) log(2.0, 64.0)  =>  6.0000000000
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> log((BIGINT) + (BIGINT))
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntLog( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv )
{
    /*
     * log b(X) = log c(X) / log c(b)
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlFloat64      sResult1;
    stlFloat64      sResult2;
    stlErrorStack * sErrorStack;

    stlInt32        sArgNum = 0;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    if( ( DTF_BIGINT( sValue1 ) <= 0 ) || ( DTF_BIGINT( sValue2 ) <= 0 ) )
    {
        if( DTF_BIGINT( sValue1 ) <= 0 )
        {
            sArgNum = 1;
        }
        else
        {
            sArgNum = 2;            
        }
        
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else if( DTF_BIGINT( sValue1 ) == 1 )
    {
        /*
         * log(1.0)하면 0이 나와 이걸로 나누기를 해야 함으로 error이다.
         */
        STL_THROW( ERROR_DIV_BY_ZERO );
    }

    /* 각 log 연산을 한다. */
    sResult1 = stlLog( DTF_BIGINT( sValue1 ) );
    sResult2 = stlLog( DTF_BIGINT( sValue2 ) );
    sResult2 = (stlInt64) ( sResult2 / sResult1 );

    STL_TRY_THROW( stlIsinfinite( sResult2 )
                   == STL_IS_INFINITE_FALSE,
                   ERROR_OUT_OF_RANGE );

    STL_TRY_THROW( ( sResult2 >= DTL_NATIVE_BIGINT_MIN ) &&
                   ( sResult2 <= DTL_NATIVE_BIGINT_MAX ),
                   ERROR_OUT_OF_RANGE );

    DTF_BIGINT( sResultValue ) = sResult2;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;


    return STL_SUCCESS;

    STL_CATCH( ERROR_DIV_BY_ZERO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMERIC_DIVISION_BY_ZERO,
                      NULL,
                      sErrorStack );
    }
    
    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      sArgNum,
                      "LOG" );
    }
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief log(x, y) logarithm to base b
 *        <BR> log(x, y) logarithm to base b
 *        <BR>   log(x, y) => [ P, M, 0 ]
 *        <BR> ex) log(2.0, 64.0)  =>  6.0000000000
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> log((NUMERIC) + (NUMERIC))
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericLog( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    /*
     * log b(X) = log c(X) / log c(b)
     */
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlFloat64        sResult1;
    stlFloat64        sResult2;
    stlErrorStack   * sErrorStack;

    stlFloat64        sDouble1;
    stlFloat64        sDouble2;

    stlInt32          sArgNum = 0;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* numeric -> double */
    STL_TRY( dtdToFloat64FromNumeric( sValue1,
                                      & sDouble1,
                                      sErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( dtdToFloat64FromNumeric( sValue2,
                                      & sDouble2,
                                      sErrorStack )
             == STL_SUCCESS );

    if( ( sDouble1 <= 0.0 ) || ( sDouble2 <= 0.0 ) )
    {
        if( sDouble1 <= 0.0 )
        {
            sArgNum = 1;
        }
        else
        {
            sArgNum = 2;            
        }
        
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else if( sDouble1 == 1.0 )
    {
        /*
         * log(1.0)하면 0이 나와 이걸로 나누기를 해야 함으로 error이다.
         */
        STL_THROW( ERROR_DIV_BY_ZERO );
    }

    /* 각 log 연산을 한다. */
    sResult1 = stlLog( sDouble1 );
    sResult2 = stlLog( sDouble2 );
    sResult2 = sResult2 / sResult1;

    STL_TRY_THROW( stlIsinfinite( sResult2 )
                   == STL_IS_INFINITE_FALSE,
                   ERROR_OUT_OF_RANGE );

    STL_TRY( dtdToNumericFromFloat64( sResult2,
                                      DTL_NUMERIC_MAX_PRECISION,
                                      DTL_SCALE_NA,
                                      sResultValue,
                                      sErrorStack )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_CATCH( ERROR_DIV_BY_ZERO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMERIC_DIVISION_BY_ZERO,
                      NULL,
                      sErrorStack );
    }

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      sArgNum,
                      "LOG" );        
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief log(x, y) logarithm to base b
 *        <BR> log(x, y) logarithm to base b
 *        <BR>   log(x, y) => [ P, M, 0 ]
 *        <BR> ex) log(2.0, 64.0)  =>  6.0000000000
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> log((DOUBLE) + (DOUBLE))
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleLog( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv )
{
    /*
     * log b(X) = log c(X) / log c(b)
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlFloat64      sResult1;
    stlFloat64      sResult2;
    stlErrorStack * sErrorStack;

    stlInt32        sArgNum = 0;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* Double의 경우는 input이 finite인지 확인한다. */
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sValue1 ) )
                 == STL_IS_INFINITE_FALSE );
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sValue2 ) )
                 == STL_IS_INFINITE_FALSE );

    if( ( DTF_DOUBLE( sValue1 ) <= 0.0 ) || ( DTF_DOUBLE( sValue2 ) <= 0.0 ) )
    {
        if( DTF_DOUBLE( sValue1 ) <= 0.0 )
        {
            sArgNum = 1;
        }
        else
        {
            sArgNum = 2;            
        }

        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else if( DTF_DOUBLE( sValue1 ) == 1.0 )
    {
        /*
         * log(1.0)하면 0이 나와 이걸로 나누기를 해야 함으로 error이다.
         */
        STL_THROW( ERROR_DIV_BY_ZERO );
    }

    /* 각 log 연산을 한다. */
    sResult1 = stlLog( DTF_DOUBLE( sValue1 ) );
    sResult2 = stlLog( DTF_DOUBLE( sValue2 ) );
    DTF_DOUBLE( sResultValue ) = sResult2 / sResult1;
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    /* log 연산후 overflow또는 underflow가 났는지 확인한다. */
    STL_TRY_THROW( stlIsinfinite( DTF_DOUBLE( sResultValue ) )
                   == STL_IS_INFINITE_FALSE,
                   ERROR_OUT_OF_RANGE );


    return STL_SUCCESS;

    STL_CATCH( ERROR_DIV_BY_ZERO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMERIC_DIVISION_BY_ZERO,
                      NULL,
                      sErrorStack );
    }

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      sArgNum,
                      "LOG" );        
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
