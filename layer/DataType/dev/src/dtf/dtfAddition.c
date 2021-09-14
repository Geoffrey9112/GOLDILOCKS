/*******************************************************************************
 * dtfAddition.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfAddition.c 1734 2011-08-23 04:59:53Z lym999 $
 *
 * NOTES
 *    현재 NaN은 고려되어 있지 않다.
 *    함수의 피연산자에 NULL값이 오지 않는다고 가정하고 구현되어 있다.
 ******************************************************************************/

/**
 * @file 
 * @brief Addition Function DataType Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfNumeric.h>

/**
 * @ingroup dtfArithmetic
 * @internal
 * @{
 */

/*******************************************************************************
 * Addition ( + )
 * ex) 2 + 3  =>  5 
 *******************************************************************************/

/**
 * @brief BigInt value의 덧셈 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (BIGINT) + (BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Addition ( + )
 * <BR> ex) 2 + 3  =>  5 
 */
stlStatus dtfBigIntAdd( stlUInt16        aInputArgumentCnt,
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

    /* Addition 연산을 한다. */
    DTF_BIGINT( sResultValue ) = DTF_BIGINT( sValue1 ) + DTF_BIGINT( sValue2 );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    /* Addition 연산후 overflow또는 underflow가 났는지 확인한다.
     * sValue1, sValue2 sign이 다르면 overflow가 나지 않는다.
     * sign이 같으면 sValue1, sValue2, result는 모두 같은 sign을 가지게 된다.
     * result가 sValue1(sValue2와 sign이 같다.)와 sign이 다르면 overflow(or underflow)이다.
     */
    if( ( DTF_SAMESIGN( DTF_BIGINT( sValue1 ), DTF_BIGINT( sValue2 ) )
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
 * @brief Numeric value의 덧셈 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NUMERIC) + (NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Addition ( + )
 * <BR>  ex) 2 + 3  =>  5
 */
stlStatus dtfNumericAdd( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    DTF_CHECK_DTL_VALUE2( aInputArgument[0].mValue.mDataValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE2( aInputArgument[1].mValue.mDataValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    STL_TRY( dtfNumericAddition( aInputArgument[0].mValue.mDataValue,
                                 aInputArgument[1].mValue.mDataValue,
                                 (dtlDataValue *)aResultValue,
                                 (stlErrorStack *)aEnv )
        == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Double value의 덧셈 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (DOUBLE) + (DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * Addition ( + )
 * <BR>  ex) 2 + 3  =>  5 
 */
stlStatus dtfDoubleAdd( stlUInt16        aInputArgumentCnt,
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

    /* Addition 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = DTF_DOUBLE( sValue1 ) + DTF_DOUBLE( sValue2 );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    /*
     *  Addition 연산후 overflow또는 underflow가 났는지 확인한다.
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

