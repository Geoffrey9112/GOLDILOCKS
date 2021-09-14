/*******************************************************************************
 * dtfBitwiseNot.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *   1. BigInt, Double, Numeric공통.
 *      함수의 피연산자에 NULL값이 오지 않는다고 가정하고 구현되어 있다.
 *   2. Double형 연산 관련.
 *      현재 NaN은 고려되어 있지 않다.
 *      infinite 값이 들어오면 argument out of range 로 처리한다.
 *   3. Bit 연산은 overflow 체크 안함.
 *      <<, >>은 overflow, underflow가 날수 있지만 별도 처리 없음.
 *      postgres: overflow 처리 없음. ( 9223372036854775807 << 4 => -16 )
 *      mysql: 9223372036854775807 << 4 => 18446744073709551600
 *             9223372036854775807 << 300 => 0
 *      <<, >> 연산에서 두번째 인자에 음수가 올경우
 *             mysql, postgres 각각 값이 다름.
 *             select 4 << -3 => -9223372036854775808   (gliese) c결과 임.
 *             select 4 << -3 => -2147483648            (postgres)
 *             select 4 << -3 => 0                      (mysql)
 *
 ******************************************************************************/

/**
 * @file dtfBitwiseNot.c
 * @brief dtfBitwiseNot Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfBitwise.h>
#include <dtfNumeric.h>

/**
 * @addtogroup dtfBitwiseNot bitwiseNot
 * @ingroup dtf
 * @internal
 * @{
 */

dtlBuiltInFuncPtr  gDtfBitwiseNotFunc[ DTL_TYPE_MAX ] = 
{
    NULL,                  /**< BOOLEAN */
    dtfSmallIntBitwiseNot, /**< NATIVE_SMALLINT */
    dtfIntegerBitwiseNot,  /**< NATIVE_INTEGER */
    dtfBigIntBitwiseNot,   /**< NATIVE_BIGINT */

    NULL,                  /**< NATIVE_REAL */
    NULL,                  /**< NATIVE_DOUBLE */

    dtfNumberBitwiseNot,   /**< FLOAT */
    dtfNumberBitwiseNot,   /**< NUMBER */
    NULL,                  /**< unsupported feature, DECIMAL */
    
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
 * bitwise NOT ( ~ )
 *      ~ => [ P, M ]
 * ex) ~1  =>  -2
 ******************************************************************************/

/**
 * @brief NATIVE_SMALLINT value의 bitwise NOT(~) 연산
 *        <BR> bitwise NOT ( ~ )
 *        <BR>   ~ => [ P, M ]
 *        <BR> ex) ~1  =>  -2
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ~(NATIVE_SMALLINT)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_SMALLINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfSmallIntBitwiseNot( stlUInt16        aInputArgumentCnt,
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

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_SMALLINT, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_SMALLINT, (stlErrorStack *)aEnv );

    /* not 연산을 한다. */
    DTF_SMALLINT( sResultValue ) = ~DTF_SMALLINT( sValue1 );
    sResultValue->mLength = DTL_NATIVE_SMALLINT_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NATIVE_INTEGER value의 bitwise NOT(~) 연산
 *        <BR> bitwise NOT ( ~ )
 *        <BR>   ~ => [ P, M ]
 *        <BR> ex) ~1  =>  -2
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ~(NATIVE_INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_INTEGER)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntegerBitwiseNot( stlUInt16        aInputArgumentCnt,
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

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );

    /* not 연산을 한다. */
    DTF_INTEGER( sResultValue ) = ~DTF_INTEGER( sValue1 );
    sResultValue->mLength = DTL_NATIVE_INTEGER_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NATIVE_BIGINT value의 bitwise NOT(~) 연산
 *        <BR> bitwise NOT ( ~ )
 *        <BR>   ~ => [ P, M ]
 *        <BR> ex) ~1  =>  -2
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ~(NATIVE_BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_BIGINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntBitwiseNot( stlUInt16        aInputArgumentCnt,
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

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /* not 연산을 한다. */
    DTF_BIGINT( sResultValue ) = ~DTF_BIGINT( sValue1 );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NUMBER value의 bitwise NOT(~) 연산
 *        <BR> bitwise NOT ( ~ )
 *        <BR>   ~ => [ P, M ]
 *        <BR> ex) ~1  =>  -2
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ~(NUMBER)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumberBitwiseNot( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;

    stlInt64        sIntNum = 0;
    stlBool         sIsTruncated = STL_FALSE;        

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /*
     * sValue을 stlInt64로 변환한다. : 반올림하지 않는다.
     * 64bit까지 BITNOT 연산지원. 
     */
    
    STL_TRY( dtlNumericToInt64( sValue1,
                                & sIntNum,
                                & sIsTruncated,
                                (stlErrorStack *)aEnv )
             == STL_SUCCESS );

    /* not 연산을 한다. */
    DTF_BIGINT( sResultValue ) = ~sIntNum;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
