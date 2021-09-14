/*******************************************************************************
 * dtfAbs.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfAbs.c 1296 2011-06-30 07:24:30Z ehpark $
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
 * @file dtfAbs.c
 * @brief dtfAbs Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfMathematical.h>
#include <dtfNumeric.h>



/**
 * @addtogroup dtfAbs Abs
 * @ingroup dtf
 * @internal
 * @{
 */

dtlBuiltInFuncPtr  gDtfAbsFunc[ DTL_TYPE_MAX ] = 
{
    NULL,                /**< BOOLEAN */
    dtfSmallIntAbs,      /**< NATIVE_SMALLINT */
    dtfIntegerAbs,       /**< NATIVE_INTEGER */
    dtfBigIntAbs,        /**< NATIVE_BIGINT */

    dtfRealAbs,          /**< NATIVE_REAL */
    dtfDoubleAbs,        /**< NATIVE_DOUBLE */

    dtfNumericAbs,       /**< FLOAT */
    dtfNumericAbs,       /**< NUMBER */
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
 * absolute value ( @ ), abs(x)
 *       @  => [ P ]
 *   abs(x) => [ P, S, M, O ]
 * ex) @ -5.0   =>  5
 * ex) abs(-5)  =>  5
 ******************************************************************************/

/**
 * @brief NATIVE_SMALLINT value의 절대값 연산
 *        <BR> absolute value ( @ ), abs(x)
 *        <BR>   @  => [ P ]
 *        <BR>   abs(x) => [ P, S, M, O ]
 *        <BR> ex) @ -5.0   =>  5
 *        <BR> ex) abs(-5)  =>  5
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> abs(NATIVE_SMALLINT)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_SMALLINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfSmallIntAbs( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    
    dtlSmallIntType sValue;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_SMALLINT, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_SMALLINT, sErrorStack );

    sValue = DTF_SMALLINT( sValue1 );
    
    /* abs 연산을 한다. */
    if( sValue < 0 )
    {
        STL_TRY_THROW( sValue != DTL_NATIVE_SMALLINT_MIN, ERROR_OUT_OF_RANGE );
        
        sValue = sValue * (-1);
    }
    else
    {
        /* Do Nothing */
    }
    
    DTF_SMALLINT( sResultValue ) = sValue;
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
 * @brief NATIVE_INTEGER value의 절대값 연산
 *        <BR> absolute value ( @ ), abs(x)
 *        <BR>   @  => [ P ]
 *        <BR>   abs(x) => [ P, S, M, O ]
 *        <BR> ex) @ -5.0   =>  5
 *        <BR> ex) abs(-5)  =>  5
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> abs(NATIVE_INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_INTEGER)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntegerAbs( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    
    dtlIntegerType  sValue;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_INTEGER, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_INTEGER, sErrorStack );
    
    sValue = DTF_INTEGER( sValue1 );
    
    /* abs 연산을 한다. */
    if( sValue < 0 )
    {
        STL_TRY_THROW( sValue != DTL_NATIVE_INTEGER_MIN, ERROR_OUT_OF_RANGE );
        
        sValue = sValue * (-1);
    }
    else
    {
        /* Do Nothing */
    }
    
    DTF_INTEGER( sResultValue ) = sValue;
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
 * @brief NATIVE_BIGINT value의 절대값 연산
 *        <BR> absolute value ( @ ), abs(x)
 *        <BR>   @  => [ P ]
 *        <BR>   abs(x) => [ P, S, M, O ]
 *        <BR> ex) @ -5.0   =>  5
 *        <BR> ex) abs(-5)  =>  5
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> abs(NATIVE_BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_BIGINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntAbs( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    
    dtlBigIntType   sValue;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    
    sValue = DTF_BIGINT( sValue1 );
    
    /* abs 연산을 한다. */
    if( sValue < 0 )
    {
        STL_TRY_THROW( sValue != DTL_NATIVE_BIGINT_MIN, ERROR_OUT_OF_RANGE );
        
        sValue = sValue * (-1);
    }
    else
    {
        /* Do Nothing */
    }
    
    DTF_BIGINT( sResultValue ) = sValue;
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
 * @brief  numeric value의 절대값 연산
 *        <BR> absolute value ( @ ), abs(x)
 *        <BR>   @  => [ P ]
 *        <BR>   abs(x) => [ P, S, M, O ]
 *        <BR> ex) @ -5.0   =>  5
 *        <BR> ex) abs(-5)  =>  5
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> abs(NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericAbs( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    dtlNumericType  * sSrcNumeric;
    dtlNumericType  * sResultNumeric;
    stlUInt8        * sSrcDigit;
    stlUInt8        * sDestDigit;
    stlInt64          sLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /*
     * abs 연산을 한다.
     * Numeric에서는 음수이면 reverse한다.
     */

    sSrcNumeric    = DTF_NUMERIC( sValue1 );
    sResultNumeric = DTF_NUMERIC( sResultValue );

    if( DTL_NUMERIC_IS_POSITIVE( sSrcNumeric ) )
    {
        DTL_COPY_DATA_VALUE( sResultValue->mBufferSize, sValue1, sResultValue );
    }
    else
    {
        /* reverse */
        DTL_NUMERIC_SET_REVERSE_SIGN_AND_EXPONENT( sSrcNumeric, sResultNumeric );
        sLength    = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
        sSrcDigit  = DTL_NUMERIC_GET_DIGIT_PTR( sSrcNumeric );
        sDestDigit = DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric );
        while( sLength )
        {
            sLength--;
            sDestDigit[ sLength ] = DTL_NUMERIC_GET_REVERSE_DIGIT_FROM_DIGIT( & sSrcDigit[ sLength ] );
        }

        /* data value 설정 */
        sResultValue->mLength = sValue1->mLength;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  NATIVE_REAL value의 절대값 연산
 *        <BR> absolute value ( @ ), abs(x)
 *        <BR>   @  => [ P ]
 *        <BR>   abs(x) => [ P, S, M, O ]
 *        <BR> ex) @ -5.0   =>  5
 *        <BR> ex) abs(-5)  =>  5
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> abs(NATIVE_REAL)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_REAL)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfRealAbs( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aInfo,
                      dtlFuncVector  * aVectorFunc,
                      void           * aVectorArg,
                      void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_REAL, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_REAL, (stlErrorStack *)aEnv );

    /* Double의 경우는 input이 finite인지 확인한다. */
    STL_DASSERT( stlIsinfinite( DTF_REAL( sValue1 ) )
                 == STL_IS_INFINITE_FALSE );

    /* fabs 연산을 한다. */
    DTF_REAL( sResultValue ) = stlAbsDouble( DTF_REAL( sValue1 ) );
    sResultValue->mLength = DTL_NATIVE_REAL_SIZE;

    /*
     * fabs 연산후 overflow또는 underflows 나지 않음.
     * postgres에도 체크하지 않음.
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  NATIVE_DOUBLE value의 절대값 연산
 *        <BR> absolute value ( @ ), abs(x)
 *        <BR>   @  => [ P ]
 *        <BR>   abs(x) => [ P, S, M, O ]
 *        <BR> ex) @ -5.0   =>  5
 *        <BR> ex) abs(-5)  =>  5
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> abs(NATIVE_DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_DOUBLE)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleAbs( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, (stlErrorStack *)aEnv );

    /* Double의 경우는 input이 finite인지 확인한다. */
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sValue1 ) )
                 == STL_IS_INFINITE_FALSE );

    /* fabs 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = stlAbsDouble( DTF_DOUBLE( sValue1 ) );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    /*
     * fabs 연산후 overflow또는 underflows 나지 않음.
     * postgres에도 체크하지 않음.
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
