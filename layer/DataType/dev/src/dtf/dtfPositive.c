/*******************************************************************************
 * dtfPositive.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfPositive.c 1472 2011-07-20 03:23:06Z lym999 $
 *
 * NOTES
 *   1. BigInt, Double, Numeric공통.
 *      함수의 피연산자에 NULL값이 오지 않는다고 가정하고 구현되어 있다.
 *   2. 각 값에 대하여 Validation만 체크한다.
 ******************************************************************************/

/**
 * @file dtfPositive.c
 * @brief Positive Function DataType Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfArithmetic.h>

/**
 * @addtogroup dtfPositive Positive
 * @ingroup dtf
 * @internal
 * @{
 */

dtlBuiltInFuncPtr  gDtfPositiveFunc[ DTL_TYPE_MAX ] = 
{
    NULL,                /**< BOOLEAN */
    dtfSmallIntPositive, /**< NATIVE_SMALLINT */
    dtfIntegerPositive,  /**< NATIVE_INTEGER */
    dtfBigIntPositive,   /**< NATIVE_BIGINT */

    dtfRealPositive,     /**< NATIVE_REAL */
    dtfDoublePositive,   /**< NATIVE_DOUBLE */

    dtfNumericPositive,  /**< FLOAT */
    dtfNumericPositive,  /**< NUMBER */
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
 * positive ( + )
 * ex) + 3  =>  3
 ******************************************************************************/

/**
 * @brief NATIVE_SMALLINT value의 positive 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_SMALLINT)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_SMALLINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * positive ( + )
 * <BR>  ex) + 3  =>  3
 */
stlStatus dtfSmallIntPositive( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue, DTL_TYPE_NATIVE_SMALLINT, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_SMALLINT, (stlErrorStack *)aEnv );

    /* positive 연산을 한다. */
    DTF_SMALLINT( sResultValue ) = DTF_SMALLINT( sValue );
    sResultValue->mLength = DTL_NATIVE_SMALLINT_SIZE;

    /*
     * positive 연산은 overflow가 발생하지 않는다.
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NATIVE_INTEGER value의 positive 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_INTEGER)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * positive ( + )
 * <BR>  ex) + 3  =>  3
 */
stlStatus dtfIntegerPositive( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );

    /* positive 연산을 한다. */
    DTF_INTEGER( sResultValue ) = DTF_INTEGER( sValue );
    sResultValue->mLength = DTL_NATIVE_INTEGER_SIZE;

    /*
     * positive 연산은 overflow가 발생하지 않는다.
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NATIVE_BIGINT value의 positive 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_BIGINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * positive ( + )
 * <BR>  ex) + 3  =>  3
 */
stlStatus dtfBigIntPositive( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /* positive 연산을 한다. */
    DTF_BIGINT( sResultValue ) = DTF_BIGINT( sValue );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    /*
     * positive 연산은 overflow가 발생하지 않는다.
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  numeric value의 Positive 연산 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * positive ( + )
 * <BR>  ex) + 3  =>  3
 */
stlStatus dtfNumericPositive( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE2( sValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /* positive 연산을 한다. (단순 대입연산) */
    DTL_COPY_DATA_VALUE( sResultValue->mBufferSize, sValue, sResultValue );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  NATIVE_REAL value의 Positive 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_REAL)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_REAL)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * positive ( + )
 * <BR>  ex) + 3  =>  3
 */
stlStatus dtfRealPositive( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue, DTL_TYPE_NATIVE_REAL, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_REAL, (stlErrorStack *)aEnv );

    /* positive 연산을 한다. */
    DTF_REAL( sResultValue ) = DTF_REAL( sValue );
    sResultValue->mLength = DTL_NATIVE_REAL_SIZE;

    /*
     *  positive 연산은 overflow가 발생하지 않는다.
     *
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  NATIVE_DOUBLE value의 Positive 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_DOUBLE)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * positive ( + )
 * <BR>  ex) + 3  =>  3
 */
stlStatus dtfDoublePositive( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue, DTL_TYPE_NATIVE_DOUBLE, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, (stlErrorStack *)aEnv );

    /* positive 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = DTF_DOUBLE( sValue );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    /*
     *  positive 연산은 overflow가 발생하지 않는다.
     *
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */

