/*******************************************************************************
 * dtfMultiplication.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfMultiplication.c 1472 2011-07-20 03:23:06Z lym999 $
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
 *      numeric의 overflow 처리는 postgres와 동일하게 되어 있으나 검증 못했음.
 *      => oracle과 비교하여 검증하기 어려움.
 *         (oracle은 precision이 최대 38이나 계산 결과는 38~40까지 나옴.)
 *
 * @TODO numeric은 precision, scale과 연관되어 검증되어야 함으로 상위단에서
 *       세밀하게 검증이 필요함.
 ******************************************************************************/

/**
 * @file dtfMultiplication.c
 * @brief Multiplication Function DataType Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfNumeric.h>
#include <dtfArithmetic.h>

/**
 * @addtogroup dtfMultiplication Multiplication
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * multiplication ( * )
 * ex) 2 * 3  =>  6
 ******************************************************************************/

/**
 * @brief integer(smallint/integer/bigint) value의 곱셈 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (BIGINT) * (BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * multiplication ( * )
 * <BR>  ex) 2 * 3  =>  6
 */
stlStatus dtfBigIntMul( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /* mul 연산을 한다. */
    DTF_BIGINT( sResultValue ) = DTF_BIGINT( sValue1 ) * DTF_BIGINT( sValue2 );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    /*
     *  mul 연산후 overflow또는 underflow가 났는지 확인한다.
     * 방법: 결과를 두번째 인자로 나누면 첫번째 인자가 된다.
     * 주의: 두번째 인자가 0이면 나누기 오류가 된다. (이 경우 overflow는 아니다.)
     * 주의: overflow가 나서 결과가 MIN값이 된값을 / -1 하면 c언어상에서 runtime error가 발생한다.
     * 변수 두개가 모두 32bit 변수 범위일경우 * 연산으로
     * overflow가 불가능함으로 overflow 체크를 하지 않는다.
     */

    /*
     * 두변수 모두 32bit만 사용되는 BigInt는 overflow 날 경우가 없음으로 pass한다.
     * =>32bit를 넘어서는 경우에만 체크한다.
     */
    if( ( DTF_CHECK_VALUE_INT32( DTF_BIGINT( sValue1 ) ) == STL_TRUE ) &&
        ( DTF_CHECK_VALUE_INT32( DTF_BIGINT( sValue2 ) ) == STL_TRUE ) )
    {
        /*
         * 이 경우 overflow가 날수 없다
         */
        /* Do Nothing */
    }
    else
    {
        /*
         * sValue2가 0이면 overflow가 아니다.
         * '/' 연산시 문제가 되기 때문에 미리 체크한다.
         */
        if( DTF_BIGINT( sValue2 ) != 0 )
        {
            /*
             * 결과가 MIN값이고 aVlue2가 -1인 경우는 overflow이다.
             * 왜냐하면 sValue2가 -1이고 결과가 MIN이 나오려면
             * (MAX+1) * -1 이 되어야 하는데 MAX + 1 은 불가능한 수이다.
             * MIN / -1 은 runtime error가 발생함으로 연산전에 체크한다.
             */
            if( ( DTF_BIGINT( sResultValue ) == DTL_NATIVE_BIGINT_MIN ) &&
                ( DTF_BIGINT( sValue2 ) == -1 ) )
            {
                STL_THROW( ERROR_OUT_OF_RANGE );
            }
            else
            {
                /* Do Nothing */
            }

            /*
             * 연산 결과를 '/' sValue2 하여 sValue1값이 나오지 않으면 overflow이다.
             */
            if( ( DTF_BIGINT( sResultValue ) / DTF_BIGINT( sValue2 ) )
                              != DTF_BIGINT( sValue1 ) )
            {
                STL_THROW( ERROR_OUT_OF_RANGE );
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

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
 * @brief  numeric value의 곱셈 연산 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NUMERIC) * (NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * multiplication ( * )
 * <BR>  ex) 2 * 3  =>  6
 */
stlStatus dtfNumericMul( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );


    /**
     * Numeric Multiplication
     */

    STL_TRY( dtfNumericMultiplication( sValue1,
                                       sValue2,
                                       sResultValue,
                                       sErrorStack )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  smallint/integer/bigint/real/double value의 곱셈 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (DOUBLE) * (DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * multiplication ( * )
 * <BR>  ex) 2 * 3  =>  6
 */
stlStatus dtfDoubleMul( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

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

    /* mul 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = DTF_DOUBLE( sValue1 ) * DTF_DOUBLE( sValue2 );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    /* mul 연산후 overflow또는 underflow가 났는지 확인한다. */
    STL_TRY_THROW( stlIsinfinite( DTF_DOUBLE( sResultValue ) )
                   == STL_IS_INFINITE_FALSE,
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

