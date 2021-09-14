/*******************************************************************************
 * dtfAtan2.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfAtan2.c 1296 2011-06-30 07:24:30Z ehpark $
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
 * @file dtfAtan2.c
 * @brief dtfAtan2 Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfMathematical.h>
#include <dtfNumeric.h>



/**
 * @addtogroup dtfAtan2 Atan2
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * atan2(y,x)
 *   atan2(y,x) => [ P, M, O ]
 ******************************************************************************/

/* /\** */
/*  * @brief atan2(y, x) 함수 */
/*  *        <BR> atan2(y,x) */
/*  *        <BR>   atan2(y,x) => [ P, M, O ] */
/*  * */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  *                               <BR> atan2(BIGINT, BIGINT) */
/*  * @param[out] aResultValue      연산 결과 (결과타입 BIGINT) */
/*  * @param[out] aEnv              environment (stlErrorStack) */
/*  *\/ */
/* stlStatus dtfBigIntAtan2( stlUInt16        aInputArgumentCnt, */
/*                           dtlValueEntry  * aInputArgument, */
/*                           void           * aResultValue, */
/*                           void           * aEnv ) */
/* { */
/*     dtlDataValue  * sValue1; */
/*     dtlDataValue  * sValue2; */
/*     dtlDataValue  * sResultValue; */
/*     stlFloat64      sDouble; */

/*     DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv ); */

/*     sValue1 = aInputArgument[0].mValue.mDataValue; */
/*     sValue2 = aInputArgument[1].mValue.mDataValue; */
/*     sResultValue = (dtlDataValue *)aResultValue; */

/*     DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv ); */
/*     DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv ); */

/*     /\* sResultValue는 length 체크를 하지 않는다. *\/ */
/*     DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv ); */

/*     /\* atan2 연산을 한다. *\/ */
/*     sDouble = stlAtan2( DTF_BIGINT( sValue1 ), DTF_BIGINT( sValue2 ) ); */

/*     if( sDouble >= 0.0 ) */
/*     { */
/*         DTF_BIGINT( sResultValue ) = (stlInt64) ( sDouble + 0.5 ); */
/*     } */
/*     else */
/*     { */
/*         DTF_BIGINT( sResultValue ) = (stlInt64) ( sDouble - 0.5 ); */
/*     } */
/*     sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE; */

    
/*     return STL_SUCCESS; */

/*     STL_FINISH; */

/*     return STL_FAILURE; */
/* } */


/**
 * @brief atan2(y, x) 함수
 *        <BR> atan2(y,x)
 *        <BR>   atan2(y,x) => [ P, M, O ]
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> atan2(NUMERIC, NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericAtan2( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    stlFloat64        sDouble1;
    stlFloat64        sDouble2;

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
    
    /* double -> numeric */
    STL_TRY( dtdToNumericFromFloat64( stlAtan2( sDouble1, sDouble2 ),
                                      DTL_NUMERIC_MAX_PRECISION,
                                      DTL_SCALE_NA,
                                      sResultValue,
                                      sErrorStack )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief atan2(y, x) 함수
 *        <BR> atan2(y,x)
 *        <BR>   atan2(y,x) => [ P, M, O ]
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> atan2(DOUBLE, DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleAtan2( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_DOUBLE, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, (stlErrorStack *)aEnv );

    /* atan2 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = stlAtan2( DTF_DOUBLE( sValue1 ), DTF_DOUBLE( sValue2 ) );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
