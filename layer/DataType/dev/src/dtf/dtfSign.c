/*******************************************************************************
 * dtfSign.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfSign.c 1296 2011-06-30 07:24:30Z ehpark $
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
 * @file dtfSign.c
 * @brief dtfSign Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfMathematical.h>



/**
 * @addtogroup dtfSign Sign
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * sign(x)   결과값 ( -1, 0, 1 )
 *   sign(x) => [ P, M, O ]
 * ex) sign(-8.4)  =>  -1
 ******************************************************************************/

/**
 * @brief sign(x) 함수
 *        <BR> sign(x)   결과값 ( -1, 0, 1 )
 *        <BR>   sign(x) => [ P, M, O ]
 *        <BR> ex) sign(-8.4)  =>  -1
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> sign(BIGINT)
 * @param[out] aResultValue 연산 결과 (결과타입 BIGINT)
 *                          <BR> 결과값 (-1, 0, 1)
 * @param[out] aEnv         environment (stlErrorStack)
 */
stlStatus dtfBigIntSign( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
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

    /* 연산을 한다. */
    if( DTF_BIGINT( sValue1 ) > 0 )
    {
        DTF_BIGINT( sResultValue ) = 1;
    }
    else if( DTF_BIGINT( sValue1 ) < 0 )
    {
        DTF_BIGINT( sResultValue ) = -1;
    }
    else
    {
        DTF_BIGINT( sResultValue ) = 0;
    }
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief sign(x) 함수
 *        <BR> sign(x)   결과값 ( -1, 0, 1 )
 *        <BR>   sign(x) => [ P, M, O ]
 *        <BR> ex) sign(-8.4)  =>  -1
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> sign(NUMERIC)
 * @param[out] aResultValue 연산 결과 (결과타입 DOUBLE)
 *                          <BR> 결과값 (-1, 0, 1)
 * @param[out] aEnv         environment (stlErrorStack)
 */
stlStatus dtfNumericSign( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;

    dtlNumericType  * sNumeric;
    dtlNumericType  * sResultNumeric;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /* 연산을 한다. */
    sNumeric = DTF_NUMERIC( sValue1 );
    sResultNumeric = DTF_NUMERIC( sResultValue );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric, sValue1->mLength ) )
    {
        DTL_NUMERIC_SET_ZERO( sResultNumeric, sResultValue->mLength );
    }
    else
    {
        if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
        {
            DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, 0 );
            *DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric ) = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
            sResultValue->mLength = 2;
        }
        else
        {
            DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumeric, 0 );
            *DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric ) = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
            sResultValue->mLength = 2;
        }
    }
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief sign(x) 함수
 *        <BR> sign(x)   결과값 ( -1, 0, 1 )
 *        <BR>   sign(x) => [ P, M, O ]
 *        <BR> ex) sign(-8.4)  =>  -1
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> sign(DOUBLE)
 * @param[out] aResultValue 연산 결과 (결과타입 DOUBLE)
 *                          <BR> 결과값 (-1, 0, 1)
 * @param[out] aEnv         environment (stlErrorStack)
 */
stlStatus dtfDoubleSign( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
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

    /* 연산을 한다. */
    if( DTF_DOUBLE( sValue1 ) > 0.0 )
    {
        DTF_DOUBLE( sResultValue ) = 1.0;
    }
    else if( DTF_DOUBLE( sValue1 ) < 0.0 )
    {
        DTF_DOUBLE( sResultValue ) = -1.0;
    }
    else
    {
        DTF_DOUBLE( sResultValue ) = 0.0;
    }
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
