/*******************************************************************************
 * dtfNegative.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfNegative.c 1472 2011-07-20 03:23:06Z lym999 $
 *
 * NOTES
 *   1. BigInt, Double, Numeric공통.
 *      함수의 피연산자에 NULL값이 오지 않는다고 가정하고 구현되어 있다.
 *   2. 각 값에 대하여 '-1'을 곱한 것과 같은 효과를 갖는다.
 ******************************************************************************/

/**
 * @file dtfNegative.c
 * @brief Negative Function DataType Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfNumeric.h>

/**
 * @addtogroup dtfNegative Negative
 * @ingroup dtf
 * @internal
 * @{
 */

dtlBuiltInFuncPtr  gDtfNegativeFunc[ DTL_TYPE_MAX ] = 
{
    NULL,                /**< BOOLEAN */
    dtfSmallIntNegative, /**< NATIVE_SMALLINT */
    dtfIntegerNegative,  /**< NATIVE_INTEGER */
    dtfBigIntNegative,   /**< NATIVE_BIGINT */

    dtfRealNegative,     /**< NATIVE_REAL */
    dtfDoubleNegative,   /**< NATIVE_DOUBLE */

    dtfNumericNegative,  /**< FLOAT */
    dtfNumericNegative,  /**< NUMBER */
    NULL,                /**< unsupported feature, DECIMAL */
    
    NULL,                /**< CHARACTER */
    NULL,                /**< CHARACTER VARYING */
    NULL,                /**< CHARACTER LONG VARYING */
    NULL,                /**< unsupported feature, CHARACTER LARGE OBJECT */
    
    NULL,                /**< BINARY */
    NULL,                /**< BINARY VARYING */
    NULL,                /**< BINARY LONG VARYING */
    NULL,                /**< unsupported feature, BINARY LARGE OBJECT */

    NULL,                /**< DATE */
    NULL,                /**< TIME WITHOUT TIME ZONE */
    NULL,                /**< TIMESTAMP WITHOUT TIME ZONE */
    NULL,                /**< TIME WITH TIME ZONE */
    NULL,                /**< TIMESTAMP WITH TIME ZONE */

    NULL,                /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    NULL,                /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

    NULL                 /**< DTL_TYPE_ROWID */
};


/*******************************************************************************
 * negative ( - )
 * ex) - 3  =>  -3
 ******************************************************************************/

/**
 * @brief NATIVE_SMALLINT value의 negative 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_SMALLINT)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_SMALLINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * negative ( - )
 * <BR>  ex) - 3  =>  -3
 */
stlStatus dtfSmallIntNegative( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,                             
                               void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue, DTL_TYPE_NATIVE_SMALLINT, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_SMALLINT, sErrorStack );

    /* overflow 체크 */
    STL_TRY_THROW( DTF_SMALLINT( sValue ) > DTL_NATIVE_SMALLINT_MIN, ERROR_OUT_OF_RANGE );

    /* negative 연산을 한다. */
    DTF_SMALLINT( sResultValue ) = - DTF_SMALLINT( sValue );
    sResultValue->mLength = DTL_NATIVE_SMALLINT_SIZE;

    return STL_SUCCESS;

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
 * @brief NATIVE_INTEGER value의 negative 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_INTEGER)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * negative ( - )
 * <BR>  ex) - 3  =>  -3
 */
stlStatus dtfIntegerNegative( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,                             
                              void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    /* overflow 체크 */
    STL_TRY_THROW( DTF_INTEGER( sValue ) > DTL_NATIVE_INTEGER_MIN, ERROR_OUT_OF_RANGE );

    /* negative 연산을 한다. */
    DTF_INTEGER( sResultValue ) = - DTF_INTEGER( sValue );
    sResultValue->mLength = DTL_NATIVE_INTEGER_SIZE;

    return STL_SUCCESS;

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
 * @brief NATIVE_BIGINT value의 negative 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_BIGINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * negative ( - )
 * <BR>  ex) - 3  =>  -3
 */
stlStatus dtfBigIntNegative( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,                             
                             void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /* overflow 체크 */
    STL_TRY_THROW( DTF_BIGINT( sValue ) > STL_INT64_MIN, ERROR_OUT_OF_RANGE );

    /* negative 연산을 한다. */
    DTF_BIGINT( sResultValue ) = - DTF_BIGINT( sValue );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    return STL_SUCCESS;

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
 * @brief  numeric value의 Negative 연산 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * negative ( - )
 * <BR>  ex) - 3  =>  -3
 */
stlStatus dtfNumericNegative( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,                              
                              void           * aEnv )
{
    dtlDataValue    * sSrcValue;
    dtlDataValue    * sResultValue;
    dtlNumericType  * sSrcNumeric;
    dtlNumericType  * sResultNumeric;
    stlUInt8        * sSrcDigit;
    stlUInt8        * sDestDigit;
    stlInt64          sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sSrcValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE2( sSrcValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    sSrcNumeric    = DTF_NUMERIC( sSrcValue );
    sResultNumeric = DTF_NUMERIC( sResultValue );

    /* sign 반대로 설정 */
    if( DTL_NUMERIC_IS_ZERO( sSrcNumeric, sSrcValue->mLength ) )
    {
        DTL_NUMERIC_SET_ZERO( sResultNumeric, sResultValue->mLength );
    }
    else
    {
        /* numeric value 설정 */
        DTL_NUMERIC_SET_REVERSE_SIGN_AND_EXPONENT( sSrcNumeric, sResultNumeric );
        sLength    = DTL_NUMERIC_GET_DIGIT_COUNT( sSrcValue->mLength );
        sSrcDigit  = DTL_NUMERIC_GET_DIGIT_PTR( sSrcNumeric );
        sDestDigit = DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric );
        while( sLength )
        {
            sLength--;
            sDestDigit[ sLength ] = DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sSrcDigit[ sLength ] );
        }
        sResultValue->mLength = sSrcValue->mLength;
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  NATIVE_REAL value의 Negative 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_REAL)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_REAL)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * negative ( - )
 * <BR>  ex) - 3  =>  -3
 */
stlStatus dtfRealNegative( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,                             
                           void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;


    DTF_CHECK_DTL_VALUE( sValue, DTL_TYPE_NATIVE_REAL, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_REAL, sErrorStack );

    /* negative 연산을 한다. */
    DTF_REAL( sResultValue ) = - DTF_REAL( sValue );
    sResultValue->mLength = DTL_NATIVE_REAL_SIZE;

    /* negative 연산후 overflow또는 underflow가 났는지 확인한다. */
    STL_DASSERT( stlIsinfinite( DTF_REAL( sResultValue ) )
                 == STL_IS_INFINITE_FALSE );

    STL_TRY_THROW( ( DTF_REAL( sValue ) + DTF_REAL( sResultValue ) )
                   == 0,
                   ERROR_OUT_OF_RANGE );

    return STL_SUCCESS;

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
 * @brief  NATIVE_DOUBLE value의 Negative 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_DOUBLE)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * negative ( - )
 * <BR>  ex) - 3  =>  -3
 */
stlStatus dtfDoubleNegative( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,                             
                             void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;


    DTF_CHECK_DTL_VALUE( sValue, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* negative 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = - DTF_DOUBLE( sValue );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    /* negative 연산후 overflow또는 underflow가 났는지 확인한다. */
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sResultValue ) )
                 == STL_IS_INFINITE_FALSE );

    STL_TRY_THROW( ( DTF_DOUBLE( sValue ) + DTF_DOUBLE( sResultValue ) )
                   == 0,
                   ERROR_OUT_OF_RANGE );

    return STL_SUCCESS;

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

