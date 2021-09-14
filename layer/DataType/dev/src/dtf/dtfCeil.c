/*******************************************************************************
 * dtfCeil.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfCeil.c 1296 2011-06-30 07:24:30Z ehpark $
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
 * @file dtfCeil.c
 * @brief dtfCeil Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfNumeric.h>
#include <dtfMathematical.h>



/**
 * @addtogroup dtfCeil Ceil
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * ceil(x) , ceiling(x)
 *   ceil(x)    => [ P, S, M ]
 *   ceiling(x) => [ P, S, M, O ]
 * ex) ceil(-42.8)     =>  -42
 * ex) ceiling(-95.3)  =>  -95
 ******************************************************************************/

/**
 * @brief ceil(x) , ceiling(x) 함수
 *        <BR> 입력된 값보다 작지 않은 가장 작은 정수를 반환하는 함수
 *        <BR> ceil(x) , ceiling(x)
 *        <BR>   ceil(x)    => [ P, S, M ]
 *        <BR>   ceiling(x) => [ P, S, M, O ]
 *        <BR> ex) ceil(-42.8)     =>  -42
 *        <BR> ex) ceiling(-95.3)  =>  -95
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ceil(BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntCeil( stlUInt16        aInputArgumentCnt,
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

    /* copy */
    DTF_BIGINT( sResultValue ) = DTF_BIGINT( sValue1 );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ceil(x) , ceiling(x) 함수
 *        <BR> 입력된 값보다 작지 않은 가장 작은 정수를 반환하는 함수
 *        <BR> ceil(x) , ceiling(x)
 *        <BR>   ceil(x)    => [ P, S, M ]
 *        <BR>   ceiling(x) => [ P, S, M, O ]
 *        <BR> ex) ceil(-42.8)     =>  -42
 *        <BR> ex) ceiling(-95.3)  =>  -95
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ceil(NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericCeil( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;

    dtlNumericType  * sNumeric;
    dtlNumericType  * sResult;

    stlInt32      sDigitCount;
    stlInt32      sExponent;
    stlUInt8    * sDigitPtr;
    stlInt32      sTmpPos;

    stlUInt8    * sResultDigitPtr;


    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValue1 = aInputArgument[0].mValue.mDataValue;
    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /* ceil 연산을 한다. */

    sNumeric = DTF_NUMERIC( sValue1 );
    sResult = DTF_NUMERIC( sResultValue );

    if( DTL_NUMERIC_IS_ZERO( sNumeric, sValue1->mLength ) == STL_TRUE )
    {
        DTL_NUMERIC_SET_ZERO( sResult, sResultValue->mLength );
        STL_THROW( RAMP_FINISH );
    }

    if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) == STL_TRUE )
    {
        sExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric );
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

        if( sExponent < 0 )
        {
            sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
            *sResultDigitPtr = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
            DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResult, 0 );
            sResultValue->mLength = 2;
        }
        else if( (sExponent + 1) >= sDigitCount )
        {
            DTL_COPY_DATA_VALUE( sResultValue->mBufferSize, sValue1, sResultValue );
        }
        else
        {
            sTmpPos = sExponent;
            while( (sDigitPtr[sTmpPos] == DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 99 )) &&
                   sTmpPos >= 0 )
            {
                sTmpPos--;
            }

            if( sTmpPos < 0 )
            {
                DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResult, sExponent + 1 );
                sResultValue->mLength = 2;
                sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
                *sResultDigitPtr = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
            }
            else
            {
                DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResult, sExponent );
                sResultValue->mLength = sTmpPos + 2;
                sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
                sResultDigitPtr[sTmpPos] = sDigitPtr[sTmpPos] + 1;
                while( sTmpPos > 0 )
                {
                    sTmpPos--;
                    sResultDigitPtr[sTmpPos] = sDigitPtr[sTmpPos];
                }
            }
        }
    }
    else
    {
        sExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sNumeric );
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

        if( sExponent < 0 )
        {
            DTL_NUMERIC_SET_ZERO( sResult, sResultValue->mLength );
        }
        else if( (sExponent + 1) >= sDigitCount )
        {
            DTL_COPY_DATA_VALUE( sResultValue->mBufferSize, sValue1, sResultValue );
        }
        else
        {
            sTmpPos = sExponent;
            DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResult, sExponent );
            sResultValue->mLength = sTmpPos + 2;
            sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
            while( sTmpPos >= 0 )
            {
                sResultDigitPtr[sTmpPos] = sDigitPtr[sTmpPos];
                sTmpPos--;
            }
        }
    }

    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ceil(x) , ceiling(x) 함수
 *        <BR> 입력된 값보다 작지 않은 가장 작은 정수를 반환하는 함수
 *        <BR> ceil(x) , ceiling(x)
 *        <BR>   ceil(x)    => [ P, S, M ]
 *        <BR>   ceiling(x) => [ P, S, M, O ]
 *        <BR> ex) ceil(-42.8)     =>  -42
 *        <BR> ex) ceiling(-95.3)  =>  -95
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ceil(DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleCeil( stlUInt16        aInputArgumentCnt,
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

    /* ceil 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = stlCeil( DTF_DOUBLE( sValue1 ) );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    /*
     * ceil 연산후 overflow또는 underflows 나지 않음.
     * postgres에도 체크하지 않음.
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
