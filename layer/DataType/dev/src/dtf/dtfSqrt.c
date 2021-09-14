/*******************************************************************************
 * dtfSqrt.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfSqrt.c 1296 2011-06-30 07:24:30Z ehpark $
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
 * @file dtfSqrt.c
 * @brief dtfSqrt Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfMathematical.h>
#include <dtfNumeric.h>


/**
 * @addtogroup dtfSqrt Sqrt
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * square root ( |/ ) , sqrt(x)
 *       |/  => [ P ]
 *   sqrt(x) => [ P, S, M, O ]
 * ex) |/25.0  => 5
 * ex) sqrt(2.0)  =>  1.4142135623731
 ******************************************************************************/

/* /\** */
/*  * @brief square root 함수 [ square root ( |/ ), sqrt(x) ] */
/*  *        <BR> square root ( |/ ) , sqrt(x) */
/*  *        <BR>   |/  => [ P ] */
/*  *        <BR>   sqrt(x) => [ P, S, M, O ] */
/*  *        <BR> ex) |/25.0  => 5 */
/*  *        <BR> ex) sqrt(2.0)  =>  1.4142135623731 */
/*  * */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  *                               <BR> |/(BIGINT), sqrt(BIGINT) */
/*  * @param[out] aResultValue      연산 결과 (결과타입 BIGINT) */
/*  * @param[out] aEnv              environment (stlErrorStack) */
/*  *\/ */
/* stlStatus dtfBigIntSqrt( stlUInt16        aInputArgumentCnt, */
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

/*     if( DTF_BIGINT( sValue1 ) < 0 ) */
/*     { */
/*         STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE ); */
/*     } */
/*     else */
/*     { */
/*         /\* Do Nothing *\/ */
/*     } */

/*     /\* sqrt 연산을 한다. *\/ */
/*     DTF_BIGINT( sResultValue ) = (stlInt64) ( stlSqrt( DTF_BIGINT( sValue1 ) ) + 0.5 ); */
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
 * @brief square root 함수 [ square root ( |/ ), sqrt(x) ]
 *        <BR> square root ( |/ ) , sqrt(x)
 *        <BR>   |/  => [ P ]
 *        <BR>   sqrt(x) => [ P, S, M, O ]
 *        <BR> ex) |/25.0  => 5
 *        <BR> ex) sqrt(2.0)  =>  1.4142135623731
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> |/(NUMERIC), sqrt(NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericSqrt( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlNumericType  * sNumeric;
    stlFloat64        sDouble;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    
    STL_TRY_THROW( DTL_NUMERIC_IS_NEGATIVE( DTF_NUMERIC( sValue1 ) ) == STL_FALSE,
                   ERROR_ARGUMENT_OUT_OF_RANGE );
    
    sNumeric = DTF_NUMERIC( sValue1 );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric, sValue1->mLength ) )
    {
        DTL_NUMERIC_SET_ZERO( DTF_NUMERIC( sResultValue ), sResultValue->mLength );
        STL_THROW( RAMP_FINISH );
    }

    /* numeric -> double */
    STL_TRY( dtdToFloat64FromNumeric( sValue1,
                                      & sDouble,
                                      sErrorStack )
             == STL_SUCCESS );
    
    /* sqrt 연산을 한다. */
    sDouble = stlSqrt( sDouble );
    
    /* double -> numeric */
    STL_TRY( dtdToNumericFromFloat64( sDouble,
                                      DTL_NUMERIC_MAX_PRECISION,
                                      DTL_SCALE_NA,
                                      sResultValue,
                                      sErrorStack )
             == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH );
        
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      "SQRT" );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/* /\** */
/*  * @brief square root 함수 [ square root ( |/ ), sqrt(x) ] */
/*  *        <BR> square root ( |/ ) , sqrt(x) */
/*  *        <BR>   |/  => [ P ] */
/*  *        <BR>   sqrt(x) => [ P, S, M, O ] */
/*  *        <BR> ex) |/25.0  => 5 */
/*  *        <BR> ex) sqrt(2.0)  =>  1.4142135623731 */
/*  * */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  *                               <BR> |/(NUMERIC), sqrt(NUMERIC) */
/*  * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC) */
/*  * @param[out] aEnv              environment (stlErrorStack) */
/*  *\/ */
/* stlStatus dtfNumericSqrt( stlUInt16        aInputArgumentCnt, */
/*                           dtlValueEntry  * aInputArgument, */
/*                           void           * aResultValue, */
/*                           void           * aEnv ) */
/* { */
/*     dtlDataValue    * sValue1; */
/*     dtlDataValue    * sResultValue; */
/*     stlErrorStack   * sErrorStack; */

/*     dtlNumericType  * sNumeric; */
/*     dtlNumericType  * sNumXi; */

/*     stlInt32          i; */
/*     stlInt32          sExponent; */
/*     stlInt32          sResultDigitCount; */
/*     stlInt32          sResultExponent; */

/*     stlChar           sTmpFx1Buffer[DTL_NUMERIC_MAX_SIZE]; */
/*     stlChar           sTmpFx2Buffer[DTL_NUMERIC_MAX_SIZE]; */
/*     stlChar           sTmpXiBuffer[DTL_NUMERIC_MAX_SIZE]; */
/*     stlChar           sTmpXjBuffer[DTL_NUMERIC_MAX_SIZE]; */
/*     stlChar           sTmpBuffer[DTL_NUMERIC_MAX_SIZE]; */
/*     stlChar           sTmpBuffer2[DTL_NUMERIC_MAX_SIZE];     */
    
/*     dtlDataValue      sFx1Value; */
/*     dtlDataValue      sFx2Value; */
/*     dtlDataValue      sXiValue; */
/*     dtlDataValue      sXjValue; */
/*     dtlDataValue      sTmpValue; */
/*     dtlDataValue      sTmpValue2;     */

/*     dtlDataValue    * sFx1; */
/*     dtlDataValue    * sFx2; */
/*     dtlDataValue    * sXi; */
/*     dtlDataValue    * sXj; */
/*     dtlDataValue    * sTmp; */
/*     dtlDataValue    * sTmp2;     */
/*     dtlDataValue    * sSwap; */

/*     stlUInt8        * sXiDigit; */
/*     stlUInt8        * sXjDigit; */
/*     stlUInt8        * sResultDigitPtr; */
/*     stlUInt8          sValue; */

/*     stlBool           sCarry;     */

/*     /\* 2 *\/ */
/*     dtlDataValue      sNumeric2 = { DTL_TYPE_NUMBER, 2, 2, (dtlNumericType*)"\xC1\x3" }; */
/*     /\* 9.9E-37 *\/ */
/*     dtlDataValue      sToleranceNumeric = { DTL_TYPE_NUMBER, 2, 2, (dtlNumericType*)"\xAE\x64" }; */

/*     dtlValueEntry     sInputArgument[1]; */
    

/*     DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv ); */

/*     sValue1 = aInputArgument[0].mValue.mDataValue; */
/*     sResultValue = (dtlDataValue *)aResultValue; */
/*     sErrorStack = (stlErrorStack *)aEnv; */

/*     DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack ); */

/*     /\* sResultValue는 length 체크를 하지 않는다. *\/ */
/*     DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack ); */

/*     STL_TRY_THROW( DTL_NUMERIC_IS_NEGATIVE( DTF_NUMERIC( sValue1 ) ) == STL_FALSE, */
/*                    ERROR_ARGUMENT_OUT_OF_RANGE ); */

/*     sNumeric = DTF_NUMERIC( sValue1 ); */

/*     if( DTL_NUMERIC_IS_ZERO( sNumeric, sValue1->mLength ) ) */
/*     { */
/*         DTL_NUMERIC_SET_ZERO( DTF_NUMERIC( sResultValue ), sResultValue->mLength ); */
/*         STL_THROW( RAMP_FINISH ); */
/*     } */

/*     sFx1Value.mType   = DTL_TYPE_NUMBER; */
/*     sFx1Value.mLength = 0; */
/*     sFx1Value.mBufferSize = DTL_NUMERIC_MAX_SIZE; */
/*     sFx1Value.mValue  = (dtlNumericType*)&sTmpFx1Buffer; */
/*     sFx1 = & sFx1Value; */

/*     sFx2Value.mType   = DTL_TYPE_NUMBER; */
/*     sFx2Value.mLength = 0; */
/*     sFx2Value.mBufferSize = DTL_NUMERIC_MAX_SIZE; */
/*     sFx2Value.mValue  = (dtlNumericType*)&sTmpFx2Buffer; */
/*     sFx2 = & sFx2Value; */

/*     sXiValue.mType   = DTL_TYPE_NUMBER; */
/*     sXiValue.mLength = 0; */
/*     sXiValue.mBufferSize = DTL_NUMERIC_MAX_SIZE; */
/*     sXiValue.mValue  = (dtlNumericType*)&sTmpXiBuffer; */
/*     sXi = & sXiValue; */

/*     sXjValue.mType   = DTL_TYPE_NUMBER; */
/*     sXjValue.mLength = 0; */
/*     sXjValue.mBufferSize = DTL_NUMERIC_MAX_SIZE; */
/*     sXjValue.mValue  = (dtlNumericType*)&sTmpXjBuffer; */
/*     sXj = & sXjValue; */

/*     sTmpValue.mType   = DTL_TYPE_NUMBER; */
/*     sTmpValue.mLength = 0; */
/*     sTmpValue.mBufferSize = DTL_NUMERIC_MAX_SIZE; */
/*     sTmpValue.mValue  = (dtlNumericType*)&sTmpBuffer; */
/*     sTmp = & sTmpValue; */

/*     sTmpValue2.mType   = DTL_TYPE_NUMBER; */
/*     sTmpValue2.mLength = 0; */
/*     sTmpValue2.mBufferSize = DTL_NUMERIC_MAX_SIZE; */
/*     sTmpValue2.mValue  = (dtlNumericType*)&sTmpBuffer2; */
/*     sTmp2 = & sTmpValue2; */
    
/*     sNumXi = DTF_NUMERIC( sXi ); */

/*     /\** */
/*      * 초기 Xi값 설정 */
/*      *\/ */

/*     sExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric ); */

/*     if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *(DTL_NUMERIC_GET_DIGIT_PTR( sNumeric )) ) < 10 ) */
/*     { */
/*         DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( */
/*             sNumXi, */
/*             DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric ) / 2 */
/*             ); */

/*         if( sExponent & 0x00000001 ) */
/*         { */
/*             *(DTL_NUMERIC_GET_DIGIT_PTR( sNumXi )) = */
/*                 DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE(  */
/*                     DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( */
/*                         *(DTL_NUMERIC_GET_DIGIT_PTR( sNumeric )) */
/*                     ) * 10 */
/*                 ); */

/*             sXi->mLength = 2; */
/*         } */
/*         else */
/*         { */
/*             *(DTL_NUMERIC_GET_DIGIT_PTR( sNumXi )) = *(DTL_NUMERIC_GET_DIGIT_PTR( sNumeric )); */
/*             sXi->mLength = 2; */
/*         } */
/*     } */
/*     else */
/*     { */
/*         DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( */
/*             sNumXi, */
/*             DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric ) / 2 */
/*             ); */

/*         *(DTL_NUMERIC_GET_DIGIT_PTR( sNumXi )) = *(DTL_NUMERIC_GET_DIGIT_PTR( sNumeric )); */
/*         sXi->mLength = 2; */
/*     } */


/*     /\** */
/*      * Newton Raphson Method */
/*      *\/ */

/*     while( STL_TRUE ) */
/*     { */
/*         /\** */
/*          * Fx1 계산 */
/*          *\/ */

/*         STL_TRY( dtfNumericMultiplication( sXi, sXi, sTmp, sErrorStack ) */
/*                  == STL_SUCCESS ); */

/*         STL_TRY( dtfNumericSubtraction( sTmp, sValue1, sFx1, sErrorStack ) */
/*                  == STL_SUCCESS ); */


/*         /\** */
/*          * Fx2 계산 */
/*          *\/ */

/*         STL_TRY( dtfNumericMultiplication( & sNumeric2, sXi, sFx2, sErrorStack ) */
/*                  == STL_SUCCESS ); */


/*         /\** */
/*          * Xj 계산 */
/*          *\/ */

/*         STL_TRY( dtfNumericDivision( sFx1, sFx2, sTmp, sErrorStack ) */
/*                  == STL_SUCCESS ); */

/*         STL_TRY( dtfNumericSubtraction( sXi, sTmp, sXj, sErrorStack ) */
/*                  == STL_SUCCESS ); */


/*         /\** */
/*          * 오차범위 체크 */
/*          * if(abs(x1 - x0)/abs(x1) < tolerance)   */
/*          *\/ */

/*         STL_TRY( dtfNumericSubtraction( sXj, sXi, sTmp2, sErrorStack ) */
/*                  == STL_SUCCESS ); */

/*         sInputArgument[0].mValue.mDataValue = sTmp2; */

/*         STL_TRY( dtfNumericAbs( 1, */
/*                                 sInputArgument, */
/*                                 sTmp, */
/*                                 NULL, */
/*                                 NULL, */
/*                                 NULL, */
/*                                 aEnv ) */
/*                  == STL_SUCCESS ); */
        

/*         STL_TRY( dtfNumericDivision( sTmp, sXj, sTmp2, sErrorStack ) */
/*                  == STL_SUCCESS ); */

/*         STL_TRY_THROW( dtlPhysicalCompareNumericToNumeric( sTmp2->mValue, */
/*                                                            sTmp2->mLength, */
/*                                                            sToleranceNumeric.mValue, */
/*                                                            sToleranceNumeric.mLength ) */
/*                        != DTL_COMPARISON_LESS, */
/*                        RAMP_END_OF_WHILE ); */

/*         /\** */
/*          * Xi와 Xj 비교 */
/*          *\/ */

/*         if( sXi->mLength == sXj->mLength ) */
/*         { */
/*             if( sXi->mLength == 1 ) */
/*             { */
/*                 STL_THROW( RAMP_END_OF_WHILE ); */
/*             } */

/*             sXiDigit = ((stlUInt8*)sXi->mValue); */
/*             sXjDigit = ((stlUInt8*)sXj->mValue); */
/*             for( i = 1 ; sXiDigit[i] == sXjDigit[i] ; i++ ) */
/*             { */
/*                 if( i == (sXi->mLength < 20 ? sXi->mLength - 1 : sXi->mLength - 2) ) */
/*                 { */
/*                     STL_THROW( RAMP_END_OF_WHILE ); */
/*                 } */
/*             } */
/*         } */


/*         /\** */
/*          * Swap Xi, Xj */
/*          *\/ */

/*         sSwap = sXi; */
/*         sXi = sXj; */
/*         sXj = sSwap; */
/*     } */

/*     STL_RAMP( RAMP_END_OF_WHILE ); */

/*     DTL_COPY_DATA_VALUE( sResultValue->mBufferSize, sXj, sResultValue ); */

/*     STL_DASSERT( DTL_NUMERIC_IS_NEGATIVE( DTF_NUMERIC( sResultValue ) ) == STL_FALSE ); */
    
    
/*     /\** */
/*      * 반올림 처리 */
/*      *\/ */
    
/*     sResultDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sResultValue->mLength ); */
/*     sResultExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( DTF_NUMERIC( sResultValue ) );     */
/*     sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC( sResultValue ) ); */
    
/*     if( sResultDigitCount == DTL_NUMERIC_MAX_DIGIT_COUNT ) */
/*     { */
/*         /\* */
/*          * 유효숫자 38 자리를 맞춘다. */
/*          *\/ */
        
/*         if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sResultDigitPtr[0] ) < 10 ) */
/*         { */
/*             /\* 첫 Digit이 1자리인 경우 *\/ */
            
/*             if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( */
/*                     sResultDigitPtr[ sResultDigitCount - 1 ] */
/*                     ) >= 95 ) */
/*             { */
/*                 sCarry = STL_TRUE; */
/*                 sResultDigitCount--; */
/*             } */
/*             else */
/*             { */
/*                 sValue = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( */
/*                     sResultDigitPtr[ sResultDigitCount - 1 ] ); */
/*                 sValue = ( sValue + 5 ) / 10 * 10; */
                
/*                 if( sValue == 0 ) */
/*                 { */
/*                     sResultDigitCount--; */
/*                 } */
/*                 else */
/*                 { */
/*                     sResultDigitPtr[ sResultDigitCount - 1 ] = */
/*                         DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( sValue ); */
/*                 } */

/*                 sCarry = STL_FALSE; */
/*             } */
/*         } */
/*         else */
/*         { */
/*             /\* 첫 Digit이 2자리인 경우 *\/ */
            
/*             if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( */
/*                     sResultDigitPtr[ sResultDigitCount - 1 ] */
/*                     ) >= 50 ) */
/*             { */
/*                 sCarry = STL_TRUE; */
/*             } */
/*             else */
/*             { */
/*                 sCarry = STL_FALSE; */
/*             } */
/*             sResultDigitCount--; */
/*         } */
        
/*         while( (sCarry == STL_TRUE) && (sResultDigitCount > 0) ) */
/*         { */
/*             if( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( */
/*                     sResultDigitPtr[sResultDigitCount-1] ) == */
/*                 99 ) */
/*             { */
/*                 sResultDigitCount--; */
/*             } */
/*             else */
/*             { */
/*                 sResultDigitPtr[sResultDigitCount-1]++; */
/*                 sCarry = STL_FALSE; */
/*                 break; */
/*             } */
/*         } */

/*         if( sResultDigitCount == 0 ) */
/*         { */
/*             /\* 첫번째 Digit에서도 반올림이 일어난 경우 *\/ */
/*             STL_DASSERT( sCarry == STL_TRUE ); */
/*             sResultDigitPtr[0] = */
/*                 DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 ); */
/*             sResultExponent++; */
/*             sResultDigitCount = 1; */
/*         } */
/*     } */

/*     /\** */
/*      * Traling Zero 제거 */
/*      *\/ */

/*     i = sResultDigitCount - 1; */
/*     while( ( i >= 0 ) && */
/*            ( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sResultDigitPtr[ i ] ) == 0 ) ) */
/*     { */
/*         i--; */
/*     } */

/*     sResultDigitCount = i + 1; */

/*     STL_DASSERT( sResultDigitCount > 0 ); */
    
/*     STL_TRY_THROW( sResultExponent <= DTL_NUMERIC_MAX_EXPONENT, */
/*                    ERROR_OUT_OF_RANGE ); */
    
/*     if( sResultExponent < DTL_NUMERIC_MIN_EXPONENT ) */
/*     { */
/*         DTL_NUMERIC_SET_ZERO( DTF_NUMERIC( sResultValue ), sResultValue->mLength ); */
/*         STL_THROW( RAMP_FINISH ); */
/*     } */
    
/*     sResultValue->mLength = sResultDigitCount + 1; */
/*     DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( DTF_NUMERIC( sResultValue ), sResultExponent ); */

/*     STL_RAMP( RAMP_FINISH ); */

/*     return STL_SUCCESS; */

/*     STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_INVALID_ARGUMENT_VALUE, */
/*                       NULL, */
/*                       sErrorStack, */
/*                       "SQRT" ); */
/*     } */

/*     STL_CATCH( ERROR_OUT_OF_RANGE ) */
/*     { */
/*         stlPushError( STL_ERROR_LEVEL_ABORT, */
/*                       DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED, */
/*                       NULL, */
/*                       sErrorStack ); */
/*     } */

/*     STL_FINISH; */

/*     return STL_FAILURE; */
/* } */



/**
 * @brief square root 함수 [ square root ( |/ ), sqrt(x) ]
 *        <BR> square root ( |/ ) , sqrt(x)
 *        <BR>   |/  => [ P ]
 *        <BR>   sqrt(x) => [ P, S, M, O ]
 *        <BR> ex) |/25.0  => 5
 *        <BR> ex) sqrt(2.0)  =>  1.4142135623731
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> |/(DOUBLE), sqrt(DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleSqrt( stlUInt16        aInputArgumentCnt,
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

    if( DTF_DOUBLE( sValue1 ) < 0.0 )
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* sqrt 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = stlSqrt( DTF_DOUBLE( sValue1 ) );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;


    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      "SQRT" );        
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
