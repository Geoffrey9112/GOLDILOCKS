/*******************************************************************************
 * dtfDivision.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfDivision.c 1472 2011-07-20 03:23:06Z lym999 $
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
 * @file dtfDivision.c
 * @brief Division Function DataType Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfNumeric.h>
#include <dtfArithmetic.h>

/**
 * @addtogroup dtfDivision Division
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * div(x, y),  x DIV y
 *   div( x, y ) => [ P ]
 *   x DIV y     => [ M ]
 * ex) div(9, 4)  =>  2
 ******************************************************************************/

/**
 * @brief integer division. 몫으로 native_double 결과를 내는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (BIGINT) / (BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * @note div(x, y) <BR>
 *       ex) div(9, 4)  =>  2.25
 */
stlStatus dtfBigIntDiv( stlUInt16        aInputArgumentCnt,
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
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );
    
    /* 0으로 나누면 안된다. */
    STL_TRY_THROW( DTF_BIGINT( sValue2 ) != 0, ERROR_DIV_BY_ZERO );
    
    /* div 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = (dtlDoubleType)DTF_BIGINT( sValue1 ) / (dtlDoubleType)DTF_BIGINT( sValue2 );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;
    
    /* div 연산후 overflow또는 underflow가 났는지 확인한다.
     */
    STL_TRY_THROW( stlIsinfinite( DTF_DOUBLE( sResultValue ) )
                   == STL_IS_INFINITE_FALSE,
                   ERROR_OUT_OF_RANGE );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_DIV_BY_ZERO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMERIC_DIVISION_BY_ZERO,
                      NULL,
                      sErrorStack );
    }
    
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
 * @brief  numeric value의 나눗셈 연산 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NUMERIC) / (NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * division ( / )
 * <BR>  ex) 4 / 2  =>  2
 */
stlStatus dtfNumericDiv( stlUInt16        aInputArgumentCnt,
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
     * Numeric Division
     */

    STL_TRY( dtfNumericDivision( sValue1,
                                 sValue2,
                                 sResultValue,
                                 sErrorStack )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * division ( / )
 * ex) 4 / 2  =>  2
 ******************************************************************************/

/**
 * @brief  smallint/integer/bigint/real/double value의 나눗셈 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (DOUBLE) / (DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * division ( / )
 * <BR>  ex) 4 / 2  =>  2
 */
stlStatus dtfDoubleDiv( stlUInt16        aInputArgumentCnt,
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

    /* 0으로 나누면 안된다. */
    STL_TRY_THROW( DTF_DOUBLE( sValue2 ) != 0.0, ERROR_DIV_BY_ZERO );

    /* div 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = DTF_DOUBLE( sValue1 ) / DTF_DOUBLE( sValue2 );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    /* div 연산후 overflow또는 underflow가 났는지 확인한다.
     */
    STL_TRY_THROW( stlIsinfinite( DTF_DOUBLE( sResultValue ) )
                   == STL_IS_INFINITE_FALSE,
                   ERROR_OUT_OF_RANGE );


    return STL_SUCCESS;

    STL_CATCH( ERROR_DIV_BY_ZERO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMERIC_DIVISION_BY_ZERO,
                      NULL,
                      sErrorStack );
    }

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

