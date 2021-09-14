/*******************************************************************************
 * dtfDegrees.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfDegrees.c 1296 2011-06-30 07:24:30Z ehpark $
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
 * @file dtfDegrees.c
 * @brief dtfDegrees Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfMathematical.h>
#include <dtfNumeric.h>



/**
 * @addtogroup dtfDegrees Degrees
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * degrees(x)
 *   degrees(x) => [ P ]
 * ex) degrees(0.5)  =>  28.6478897565412
 ******************************************************************************/

/* /\** */
/*  * @brief degrees(x) 함수 */
/*  *        <BR> degrees(x) */
/*  *        <BR>   degrees(x) => [ P ] */
/*  *        <BR> ex) degrees(0.5)  =>  28.6478897565412 */
/*  * */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  *                               <BR> degrees(BIGINT) */
/*  * @param[out] aResultValue      연산 결과 (결과타입 BIGINT) */
/*  * @param[out] aEnv              environment (stlErrorStack) */
/*  *\/ */
/* stlStatus dtfBigIntDegrees( stlUInt16        aInputArgumentCnt, */
/*                             dtlValueEntry  * aInputArgument, */
/*                             void           * aResultValue, */
/*                             void           * aEnv ) */
/* { */
/*     dtlDataValue  * sValue1; */
/*     dtlDataValue  * sResultValue; */
/*     stlErrorStack * sErrorStack; */

/*     stlFloat64      sDouble; */

/*     DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv ); */

/*     sValue1 = aInputArgument[0].mValue.mDataValue; */
/*     sResultValue = (dtlDataValue *)aResultValue; */
/*     sErrorStack = (stlErrorStack *)aEnv; */

/*     DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack ); */

/*     /\* sResultValue는 length 체크를 하지 않는다. *\/ */
/*     DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack ); */

/*     /\* degrees 연산을 한다. *\/ */
/*     sDouble = ( DTF_BIGINT( sValue1 ) * ( 180.0 / stlPi() ) ); */

/*     /\* degrees 연산후 overflow가 났는지 확인한다. *\/ */
/*     STL_TRY_THROW( stlIsinfinite( sDouble ) */
/*                    == STL_IS_INFINITE_FALSE, */
/*                    ERROR_OUT_OF_RANGE ); */

/*     STL_TRY_THROW( ( sDouble >= DTL_NATIVE_BIGINT_MIN ) && */
/*                    ( sDouble <= DTL_NATIVE_BIGINT_MAX ), */
/*                    ERROR_OUT_OF_RANGE ); */

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

/*     STL_CATCH( ERROR_OUT_OF_RANGE ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED, */
/*                       NULL, */
/*                       sErrorStack ); */
/*     } */

/*     STL_FINISH; */

/*     return STL_FAILURE; */
/* } */


/**
 * @brief degrees(x) 함수
 *        <BR> degrees(x)
 *        <BR>   degrees(x) => [ P ]
 *        <BR> ex) degrees(0.5)  =>  28.6478897565412
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> degrees(NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericDegrees( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    stlFloat64        sDouble;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* numeric -> double */
    STL_TRY( dtdToFloat64FromNumeric( sValue1,
                                      & sDouble,
                                      sErrorStack )
             == STL_SUCCESS );

    /* degrees 연산을 한다. */
    sDouble = sDouble * ( 180.0 / stlPi() );

    /* degrees 연산후 overflow가 났는지 확인한다. */
    STL_TRY_THROW( stlIsinfinite( sDouble )
                   == STL_IS_INFINITE_FALSE,
                   ERROR_OUT_OF_RANGE );

    /* double -> numeric */
    STL_TRY( dtdToNumericFromFloat64( sDouble,
                                      DTL_NUMERIC_MAX_PRECISION,
                                      DTL_SCALE_NA,
                                      sResultValue,
                                      sErrorStack )
             == STL_SUCCESS );


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
 * @brief degrees(x) 함수
 *        <BR> degrees(x)
 *        <BR>   degrees(x) => [ P ]
 *        <BR> ex) degrees(0.5)  =>  28.6478897565412
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> degrees(DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleDegrees( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* Double의 경우는 input이 finite인지 확인한다. */
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sValue1 ) )
                 == STL_IS_INFINITE_FALSE );

    /* degrees 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = DTF_DOUBLE( sValue1 ) * ( 180.0 / stlPi() );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    /* degrees 연산후 overflow가 났는지 확인한다. */
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
