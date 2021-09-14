/*******************************************************************************
 * dtfAsin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfAsin.c 1296 2011-06-30 07:24:30Z ehpark $
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
 * @file dtfAsin.c
 * @brief dtfAsin Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfMathematical.h>
#include <dtfNumeric.h>


/**
 * @addtogroup dtfAsin Asin
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * asin(x)
 *   asin(x) => [ P, M, O ]
 ******************************************************************************/

/* /\** */
/*  * @brief asin(x) 함수 */
/*  *        <BR> asin(x) */
/*  *        <BR>   asin(x) => [ P, M, O ] */
/*  * */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  *                               <BR> asin(DOUBLE) */
/*  * @param[out] aResultValue      연산 결과 (결과타입 BIGINT) */
/*  * @param[out] aEnv              environment (stlErrorStack) */
/*  *\/ */
/* stlStatus dtfBigIntAsin( stlUInt16        aInputArgumentCnt, */
/*                          dtlValueEntry  * aInputArgument, */
/*                          void           * aResultValue, */
/*                          void           * aEnv ) */
/* { */
/*     dtlDataValue  * sValue1; */
/*     dtlDataValue  * sResultValue; */
/*     stlErrorStack * sErrorStack; */

/*     DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv ); */

/*     sValue1 = aInputArgument[0].mValue.mDataValue; */
/*     sResultValue = (dtlDataValue *)aResultValue; */
/*     sErrorStack = (stlErrorStack *)aEnv; */

/*     DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack ); */

/*     /\* sResultValue는 length 체크를 하지 않는다. *\/ */
/*     DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack ); */


/*     switch( DTF_BIGINT( sValue1 ) ) */
/*     { */
/*         case -1 : */
/*             { */
/*                 DTF_BIGINT( sResultValue ) = -2; */
/*                 break; */
/*             } */
/*         case 0 : */
/*             { */
/*                 DTF_BIGINT( sResultValue ) = 0; */
/*                 break; */
/*             } */
/*         case 1 : */
/*             { */
/*                 DTF_BIGINT( sResultValue ) = 2; */
/*                 break; */
/*             } */
            
/*         default : */
/*             { */
/*                 STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE ); */
/*                 break; */
/*             } */
/*     } */

/*     sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE; */

    
/*     return STL_SUCCESS; */

/*     STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_INVALID_NUMBER_OF_ARGUMENTS, */
/*                       NULL, */
/*                       sErrorStack ); */
/*     } */

/*     STL_FINISH; */

/*     return STL_FAILURE; */
/* } */

/**
 * @brief asin(x) 함수
 *        <BR> asin(x)
 *        <BR>   asin(x) => [ P, M, O ]
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> asin(NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericAsin( stlUInt16        aInputArgumentCnt,
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
    STL_TRY_THROW( ( sDouble >= -1.0 ) && ( sDouble <= 1.0 ), ERROR_ARGUMENT_OUT_OF_RANGE );

    /* double -> numeric */
    STL_TRY( dtdToNumericFromFloat64( stlAsin( sDouble ),
                                      DTL_NUMERIC_MAX_PRECISION,
                                      DTL_SCALE_NA,
                                      sResultValue,
                                      sErrorStack )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      "ASIN" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief asin(x) 함수
 *        <BR> asin(x)
 *        <BR>   asin(x) => [ P, M, O ]
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> asin(DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleAsin( stlUInt16        aInputArgumentCnt,
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

    STL_TRY_THROW( ( DTF_DOUBLE( sValue1 ) >= -1.0 ) && ( DTF_DOUBLE( sValue1 ) <= 1.0 ),
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    /* asin 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = stlAsin( DTF_DOUBLE( sValue1 ) );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      "ASIN" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
