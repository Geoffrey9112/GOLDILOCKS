/*******************************************************************************
 * dtfSubtraction.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfSubtraction.c 1472 2011-07-20 03:23:06Z lym999 $
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
 * @file dtfSubtraction.c
 * @brief Subtraction Function DataType Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfNumeric.h>
#include <dtfArithmetic.h>

/**
 * @addtogroup dtfSubtraction Subtraction
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * subtraction ( - )
 * ex) 2 - 3  =>  -1
 ******************************************************************************/

/**
 * @brief integer(smallint/integer/bigint) value의 뺄셈 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (BIGINT) - (BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * subtraction ( - )
 * <BR>  ex) 2 - 3  =>  -1
 */
stlStatus dtfBigIntSub( stlUInt16        aInputArgumentCnt,
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

    /* sub 연산을 한다. */
    DTF_BIGINT( sResultValue ) = DTF_BIGINT( sValue1) - DTF_BIGINT( sValue2 );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    /*
     * sub 연산후 overflow또는 underflow가 났는지 확인한다.
     * sign이 같으면 overflow가 나지 않는다.
     * sign이 다르면 첫번째인 입력인 sValue1와 result는 같은 sign을 가지게 된다.
     * result가 sValue1와 sign이 다르면 overflow(or underflow)이다.
     */
    if( ( DTF_DIFFSIGN( DTF_BIGINT( sValue1 ), DTF_BIGINT( sValue2 ) )
                        == STL_TRUE ) &&
        ( DTF_DIFFSIGN( DTF_BIGINT( sResultValue ), DTF_BIGINT( sValue1 ) )
                        == STL_TRUE ) )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
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
 * @brief  numeric value의 뺄셈 연산 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NUMERIC) - (NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * subtraction ( - )
 * <BR>  ex) 2 - 3  =>  -1
 */
stlStatus dtfNumericSub( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    DTF_CHECK_DTL_VALUE2( aInputArgument[0].mValue.mDataValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE2( aInputArgument[1].mValue.mDataValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    STL_TRY( dtfNumericSubtraction( aInputArgument[0].mValue.mDataValue,
                                    aInputArgument[1].mValue.mDataValue,
                                    (dtlDataValue *)aResultValue,
                                    (stlErrorStack *)aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  smallint/integer/bigint/real/double value의 뺄셈 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (DOUBLE) - (DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * subtraction ( - )
 * <BR>  ex) 2 - 3  =>  -1
 */
stlStatus dtfDoubleSub( stlUInt16        aInputArgumentCnt,
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

    /* sub 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = DTF_DOUBLE( sValue1 ) - DTF_DOUBLE( sValue2 );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    /*
     *  sub 연산후 overflow또는 underflow가 났는지 확인한다.
     *
     */
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

