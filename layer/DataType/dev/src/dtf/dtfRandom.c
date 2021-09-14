/*******************************************************************************
 * dtfRandom.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfRandom.c 1296 2011-06-30 07:24:30Z ehpark $
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


/*
 * @TODO random관련 함수는 rand, srand로 해야 함.
 * postgres의 random은 thread safe하지 않음.
 */

/**
 * @file dtfRandom.c
 * @brief dtfRandom Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfMathematical.h>
#include <dtfNumeric.h>



/**
 * @addtogroup dtfRandom Random
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * random(), rand()  random value in the range 0.0 <= x < 1.0
 *   random() => [ P ]
 *   rand()   => [ M ]
 * ex) random()
 ******************************************************************************/

/**
 * @brief random함수 random value in the range 0.0 <= x < 1.0
 *        <BR> random(), rand()  random value in the range 0.0 <= x < 1.0
 *        <BR>   random() => [ P ]
 *        <BR>   rand()   => [ M ]
 *        <BR> ex) random()
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 *                               0.0 ~ 1.0 사이의 값
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntRandom( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    dtlBigIntType   sMinValue;
    dtlBigIntType   sMaxValue;
    dtlBigIntType   sRandomValue = 0;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    if( DTF_BIGINT( sValue1 ) <= DTF_BIGINT( sValue2 ) )
    {
        sMinValue = DTF_BIGINT( sValue1 );
        sMaxValue = DTF_BIGINT( sValue2 );
    }
    else
    {
        sMinValue = DTF_BIGINT( sValue2 );
        sMaxValue = DTF_BIGINT( sValue1 );
    }

    /* result [0.0 - 1.0) */
    /*
     * postgres는 (RAND_MAX + 1) 로 나누는데 이유를 모르겠음.
     * +1 해서 문제가 없음으로 일단 따라함.
     */
//    DTF_BIGINT( sResultValue ) = (stlFloat64)stlRandom() /
//                                 ( (stlFloat64)STL_RAND_MAX + 1 );

    /*
     * random값 생성 
     */
    
    // 0.0 ~ 1.0 사이의 값 생성
    sRandomValue = ( (stlFloat64)stlRand() / ((stlFloat64)STL_RAND_MAX + 1) );

    // minvalue와 maxvalue 사이의 값 생성
    sRandomValue = ( sRandomValue * (sMaxValue - sMinValue) ) + sMinValue;

    STL_DASSERT( (sRandomValue >= sMinValue) &&
                 (sRandomValue <= sMaxValue) );

    /*
     * result value 설정.
     */ 
    DTF_BIGINT( sResultValue ) = (stlInt64) sRandomValue;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief random함수 random value in the range 0.0 <= x < 1.0
 *        <BR> random(), rand()  random value in the range 0.0 <= x < 1.0
 *        <BR>   random() => [ P ]
 *        <BR>   rand()   => [ M ]
 *        <BR> ex) random()
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 *                               0.0 ~ 1.0 사이의 값
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericRandom( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlDoubleType     sMinValue;
    dtlDoubleType     sMaxValue;
    dtlDoubleType     sRandomValue = 0.0;

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
    
    if( sDouble1 <= sDouble2 )
    {
        sMinValue = sDouble1;
        sMaxValue = sDouble2;
    }
    else
    {
        sMinValue = sDouble2;
        sMaxValue = sDouble1;
    }

    /* result [0.0 - 1.0) */
    /*
     * postgres는 (RAND_MAX + 1) 로 나누는데 이유를 모르겠음.
     * +1 해서 문제가 없음으로 일단 따라함.
     */
//    DTF_DOUBLE( sResultValue ) = (stlFloat64)stlRandom() /
//                                 ( (stlFloat64)STL_RAND_MAX + 1 );

    /*
     * random값 생성 
     */
    
    // 0.0 ~ 1.0 사이의 값 생성
    sRandomValue = ( (stlFloat64)stlRand() / ((stlFloat64)STL_RAND_MAX + 1) );

    // minvalue와 maxvalue 사이의 값 생성
    sRandomValue = ( sRandomValue * (sMaxValue - sMinValue) ) + sMinValue;

    STL_DASSERT( (sRandomValue >= sMinValue) &&
                 (sRandomValue <= sMaxValue) );

    /*
     * result value 설정.
     */ 
    STL_TRY( dtdToNumericFromFloat64( sRandomValue,
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
 * @brief random함수 random value in the range 0.0 <= x < 1.0
 *        <BR> random(), rand()  random value in the range 0.0 <= x < 1.0
 *        <BR>   random() => [ P ]
 *        <BR>   rand()   => [ M ]
 *        <BR> ex) random()
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 *                               0.0 ~ 1.0 사이의 값
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleRandom( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    dtlDoubleType   sMinValue;
    dtlDoubleType   sMaxValue;
    dtlDoubleType   sRandomValue = 0;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_DOUBLE, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, (stlErrorStack *)aEnv );

    /* Double의 경우는 input이 finite인지 확인한다. */
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sValue1 ) )
                 == STL_IS_INFINITE_FALSE );
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sValue2 ) )
                 == STL_IS_INFINITE_FALSE );

    if( DTF_DOUBLE( sValue1 ) <= DTF_DOUBLE( sValue2 ) )
    {
        sMinValue = DTF_DOUBLE( sValue1 );
        sMaxValue = DTF_DOUBLE( sValue2 );
    }
    else
    {
        sMinValue = DTF_DOUBLE( sValue2 );
        sMaxValue = DTF_DOUBLE( sValue1 );
    }

    /* result [0.0 - 1.0) */
    /*
     * postgres는 (RAND_MAX + 1) 로 나누는데 이유를 모르겠음.
     * +1 해서 문제가 없음으로 일단 따라함.
     */
//    DTF_DOUBLE( sResultValue ) = (stlFloat64)stlRandom() /
//                                 ( (stlFloat64)STL_RAND_MAX + 1 );

    /*
     * random값 생성 
     */
    
    // 0.0 ~ 1.0 사이의 값 생성
    sRandomValue = ( (stlFloat64)stlRand() / ((stlFloat64)STL_RAND_MAX + 1) );

    // minvalue와 maxvalue 사이의 값 생성
    sRandomValue = ( sRandomValue * (sMaxValue - sMinValue) ) + sMinValue;

    STL_DASSERT( (sRandomValue >= sMinValue) &&
                 (sRandomValue <= sMaxValue) );

    /*
     * result value 설정.
     */ 
    DTF_DOUBLE( sResultValue ) = sRandomValue;    
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * setseed(x)
 *   setseed(x) => [ P ]
 * ex) setseed(0.54823)
 ******************************************************************************/

/**
 * @brief setseed(x) 함수
 *        <BR> setseed(x)
 *        <BR>   setseed(x) => [ P ]
 *        <BR> ex) setseed(0.54823)
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> setseed(DOUBLE)
 *                               <BR> -1.0 ~ 1.0 사이의 값.
 * @param[out] aResultValue      연산 결과 (없음.)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfSetseed( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aEnv )
{
    dtlDataValue  * sValue1;
    stlErrorStack * sErrorStack;
    stlInt32        sSeed;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* Double의 경우는 input이 finite인지 확인한다. */
    STL_TRY_THROW( stlIsinfinite( DTF_DOUBLE( sValue1 ) )
                   == STL_IS_INFINITE_FALSE,
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    if( ( DTF_DOUBLE( sValue1 ) < -1.0 ) || ( DTF_DOUBLE( sValue1 ) > 1.0 ) )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* srand 연산을 한다. */
    sSeed = DTF_DOUBLE( sValue1 ) * STL_RAND_MAX;

    /*
     * stlSrandom에 들어가는값은 unsigned여서 cast한다.
     */
    stlSrandom( (stlUInt32)sSeed );

    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      "SETSEED" );                
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
