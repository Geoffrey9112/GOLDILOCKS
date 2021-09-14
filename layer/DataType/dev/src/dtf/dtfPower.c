/*******************************************************************************
 * dtfPower.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfPower.c 1296 2011-06-30 07:24:30Z ehpark $
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
 * @file dtfPower.c
 * @brief dtfPower Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfMathematical.h>
#include <dtfNumeric.h>



/**
 * @addtogroup dtfPower Power
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * exponentiation ( ^ ) , power(a, b)
 *            ^  => [ P ]
 *   power(a, b) => [ P, S, M, O ]
 * ex) 2.0 ^ 3.0        =>  8
 * ex) power(9.0, 3.0)  =>  729
 ******************************************************************************/

/**
 * @brief  power 함수 [ exponentiation ( ^ ) , power(a, b) ]
 *         <BR> exponentiation ( ^ ) , power(a, b)
 *         <BR>   ^  => [ P ]
 *         <BR>   power(a, b) => [ P, S, M, O ]
 *         <BR> ex) 2.0 ^ 3.0        =>  8
 *         <BR> ex) power(9.0, 3.0)  =>  729
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (BIGINT) ^ (BIGINT), power(BIGINT, BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntPower( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    stlFloat64      sDouble;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    if( ( DTF_BIGINT( sValue1 ) == 0 ) &&
        ( DTF_BIGINT( sValue2 ) < 0 ) )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    if( ( DTF_BIGINT( sValue1 ) < 0 ) &&
        ( stlFloor( DTF_BIGINT( sValue2 ) ) != DTF_BIGINT( sValue2 ) ) )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* pow 연산을 한다. */
    errno = 0;
    STL_TRY_THROW( stlPow( DTF_BIGINT( sValue1 ),
                           DTF_BIGINT( sValue2 ),
                           & sDouble,
                           sErrorStack ) == STL_SUCCESS,
                   ERROR_OUT_OF_RANGE );
    
    /*
     *  pow 연산후 overflow또는 underflow가 났는지 확인한다.
     */
    STL_TRY_THROW( stlIsinfinite( sDouble ) == STL_IS_INFINITE_FALSE,
                   ERROR_OUT_OF_RANGE );
    /* STL_TRY_THROW( DTL_CHECK_FLOAT_VALUE_OVER_AND_UNDER_FLOW( sDouble, */
    /*                                                           STL_TRUE, */
    /*                                                           STL_TRUE ) */
    /*                == STL_TRUE, */
    /*                ERROR_OUT_OF_RANGE ); */

    STL_TRY_THROW( ( sDouble >= DTL_NATIVE_BIGINT_MIN ) &&
                   ( sDouble <= DTL_NATIVE_BIGINT_MAX ),
                   ERROR_OUT_OF_RANGE );

    if( sDouble >= 0.0 )
    {
        DTF_BIGINT( sResultValue ) = (stlInt64) ( sDouble + 0.5 );
    }
    else
    {
        DTF_BIGINT( sResultValue ) = (stlInt64) ( sDouble - 0.5 );
    }
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;


    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      2,
                      "POWER" );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        (void)stlPopError( sErrorStack );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  power 함수 [ exponentiation ( ^ ) , power(a, b) ]
 *         <BR> exponentiation ( ^ ) , power(a, b)
 *         <BR>   ^  => [ P ]
 *         <BR>   power(a, b) => [ P, S, M, O ]
 *         <BR> ex) 2.0 ^ 3.0        =>  8
 *         <BR> ex) power(9.0, 3.0)  =>  729
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NUMERIC) ^ (NUMERIC), power(NUMERIC, NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericPower( stlUInt16        aInputArgumentCnt,
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
    
    if( ( sDouble1 == 0.0 ) &&
        ( sDouble2 < 0.0 ) )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    if( ( sDouble1 < 0.0 ) &&
        ( stlFloor( sDouble2 ) != sDouble2 ) )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* pow 연산을 한다. */
    errno = 0;
    STL_TRY_THROW( stlPow( sDouble1,
                           sDouble2,
                           &sDouble2,
                           sErrorStack ) == STL_SUCCESS,
                   ERROR_OUT_OF_RANGE);

    /*
     *  pow 연산후 overflow또는 underflow가 났는지 확인한다.
     */
    STL_TRY_THROW( stlIsinfinite( sDouble2 ) == STL_IS_INFINITE_FALSE,
                   ERROR_OUT_OF_RANGE );
    /* STL_TRY_THROW( DTL_CHECK_FLOAT_VALUE_OVER_AND_UNDER_FLOW( sDouble2, */
    /*                                                           STL_TRUE, */
    /*                                                           STL_TRUE ) */
    /*                == STL_TRUE, */
    /*                ERROR_OUT_OF_RANGE ); */

    /* double -> numeric */
    STL_TRY( dtdToNumericFromFloat64( sDouble2,
                                      DTL_NUMERIC_MAX_PRECISION,
                                      DTL_SCALE_NA,
                                      sResultValue,
                                      sErrorStack )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      2,
                      "POWER" );
    }
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        (void)stlPopError( sErrorStack );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  power 함수 [ exponentiation ( ^ ) , power(a, b) ]
 *         <BR> exponentiation ( ^ ) , power(a, b)
 *         <BR>   ^  => [ P ]
 *         <BR>   power(a, b) => [ P, S, M, O ]
 *         <BR> ex) 2.0 ^ 3.0        =>  8
 *         <BR> ex) power(9.0, 3.0)  =>  729
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (DOUBLE) ^ (DOUBLE), power(DOUBLE, DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoublePower( stlUInt16        aInputArgumentCnt,
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

    if( ( DTF_DOUBLE( sValue1 ) == 0.0 ) &&
        ( DTF_DOUBLE( sValue2 ) < 0.0 ) )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    if( ( DTF_DOUBLE( sValue1 ) < 0.0 ) &&
        ( stlFloor( DTF_DOUBLE( sValue2 ) ) != DTF_DOUBLE( sValue2 ) ) )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* pow 연산을 한다. */
    errno = 0;
    STL_TRY_THROW( stlPow( DTF_DOUBLE( sValue1 ),
                           DTF_DOUBLE( sValue2 ),
                           ((dtlDoubleType *)(sResultValue)->mValue),
                           sErrorStack ) == STL_SUCCESS,
                   ERROR_OUT_OF_RANGE );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    if( ( errno == EDOM ) && ( stlIsnan(DTF_DOUBLE(sResultValue)) ) )
    {
        if( ( ( stlAbsDouble(DTF_DOUBLE(sValue1)) > 1 ) && ( DTF_DOUBLE(sValue2) >= 0 ) ) ||
            ( ( stlAbsDouble(DTF_DOUBLE(sValue1)) < 1 ) && ( DTF_DOUBLE(sValue2) < 0 ) ) )
        {
            DTF_DOUBLE( sResultValue ) = STL_FLOAT64_INFINITY;
        }
        else if( stlAbsDouble(DTF_DOUBLE(sValue1)) != 1)
        {
            DTF_DOUBLE( sResultValue ) = 0;
        }
        else
        {
            DTF_DOUBLE( sResultValue ) = 1;
        }
    }
    else if( ( errno == ERANGE ) && ( DTF_DOUBLE(sResultValue) != 0 ) && ( !stlIsnan(DTF_DOUBLE(sResultValue)) ) )
    {
        DTF_DOUBLE( sResultValue ) = STL_FLOAT64_INFINITY;
    }
    else
    {
        /* Do Nothing */
    }

    /* pow 연산후 overflow또는 underflow가 났는지 확인한다. */
    STL_TRY_THROW( DTL_CHECK_FLOAT_VALUE_OVER_AND_UNDER_FLOW(
                       DTF_DOUBLE( sResultValue ),
                       stlIsinfinite( DTF_DOUBLE(sValue1) ) || stlIsinfinite( DTF_DOUBLE(sValue2) ),
                       DTF_DOUBLE( sValue1 ) == 0 )
                   == STL_TRUE,
                   ERROR_OUT_OF_RANGE );
  
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      2,
                      "POWER" );        
    }
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        (void)stlPopError( sErrorStack );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
