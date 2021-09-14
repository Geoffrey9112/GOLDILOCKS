/*******************************************************************************
 * dtfModulus.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfModulus.c 1472 2011-07-20 03:23:06Z lym999 $
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
 * @file dtfModulus.c
 * @brief Modulus Function DataType Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfNumeric.h>
#include <dtfArithmetic.h>

/**
 * @addtogroup dtfModulus Modulus
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * modulo ( % ),  mod(y, x),  y MOD m
 *   modulo ( % ) => [ P, M ]
 *   mod(y, x)    => [ P, S, M, O ]
 *   y MOD m      => [ M ]
 * ex) 5 % 4  =>  1
 *     mod(9, 4)  =>  1
 ******************************************************************************/

/**
 * @brief integer(smallint/integer/bigint) value의 나머지 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (BIGINT) % (BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 Double)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * modulo ( % ), mod(y, x)
 * <BR>  ex) 5 % 4      =>  1
 * <BR>      mod(9, 4)  =>  1
 */
stlStatus dtfBigIntMod( stlUInt16        aInputArgumentCnt,
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

    /* 0으로 나누면 안된다. */
    STL_TRY_THROW( DTF_BIGINT( sValue2 ) != 0, ERROR_DIV_BY_ZERO );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    /* mod 연산전 overflow또는 underflow가 났는지 확인한다.
     * mod 는 div와 동일한 조건을 가진다.
     * INT64_MIN / -1 를 하면 overflow가 아닌 runtime error (Floating point exception)가
     * 발생함으로 div연산후 overflow체크 하는 방법을 사용하지 않고 div연산전에 체크 한다.
     * 문제가 되는 유일한 경우는 arg1 / arg2 에서 arg1 = INT64_MIN, arg2 = -1 인경우다.
     * 그럴경우 -INT64_MIN 가 되는데 이것은 표현 불가다.
     * if(arg1 == INT64_MIN && arg2 == -1 ){ overflow }
     */
    if( ( DTF_BIGINT( sValue1 ) == DTL_NATIVE_BIGINT_MIN ) &&
        ( DTF_BIGINT( sValue2 ) == -1 ) )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }
    else
    {
        /* Do Nothing */
    }

    /* mod 연산을 한다. */
    DTF_BIGINT( sResultValue ) = DTF_BIGINT( sValue1 ) % DTF_BIGINT( sValue2 );

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
 * @brief  smallint/integer/bigint/real/double value의  나머지 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (DOUBLE) % (DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 DOUBLE)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * modulo ( % ), mod(y, x)
 * <BR>  ex) 5 % 4      =>  1
 * <BR>      mod(9, 4)  =>  1
 */
stlStatus dtfDoubleMod( stlUInt16        aInputArgumentCnt,
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

    /* mod 연산을 한다. */
    DTF_DOUBLE( sResultValue ) = stlFmod( DTF_DOUBLE( sValue1 ),
                                          DTF_DOUBLE( sValue2 ) );
    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;

    /* real mod 에대한 overflow check는 없다. */

    return STL_SUCCESS;

    STL_CATCH( ERROR_DIV_BY_ZERO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMERIC_DIVISION_BY_ZERO,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/******************************************************************************
 * NUMERIC MOD 연산을 위한 내부 함수
 ******************************************************************************/

#define DTF_MOD_INT_ARR_DIGITS                        ( 2 )
#define DTF_MOD_INT_ARRY_BASE_VALUE                   ( 100 )
#define DTF_MOD_INT_ARRY_HALF_BASE_VALUE              ( 50 )
#define DTF_MOD_INTERMEDIATE_RESULT_INT_ARR_MAX_COUNT ( 200 )
#define DTF_MOD_MUL_GUARD_DIGITS                      ( 3 )



/**
 * @brief  numeric value의 나머지 연산 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NUMERIC) % (NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * modulo ( % ), mod(y, x)
 * <BR>  ex) 5 % 4      =>  1
 * <BR>      mod(9, 4)  =>  1
 */
stlStatus dtfNumericMod( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlNumericType  * sNumeric1;
    dtlNumericType  * sNumeric2;
    dtlNumericType  * sResult;

    stlUInt8        * sResultDigitPtr;    

    stlBool           sLeftIsPositive;
    stlBool           sRightIsPositive;

    stlInt32          sLeftDigitCount;
    stlInt32          sRightDigitCount;

    stlInt32          sLeftExponent;
    stlInt32          sRightExponent;
    stlInt32          sRightDigitCountAfterDecimal;

    stlUInt8        * sLeftDigitPtr;
    stlUInt8        * sRightDigitPtr;

    stlInt32          sLoop;
    stlInt8         * sTable;
    
#define DTF_MOD_INT_ARR_MAX_COUNT ( 61 )    
    
    stlInt8           sLeftIntArr[ DTF_MOD_INT_ARR_MAX_COUNT + 1 ] = {0, };
    stlInt8           sRightIntArr[ DTF_MOD_INT_ARR_MAX_COUNT + 1 ] = {0, };
    stlInt8           sTmpIntArr[ DTF_MOD_INTERMEDIATE_RESULT_INT_ARR_MAX_COUNT + 1 ] = {0, };

    stlBool           sTmpIsPositive;
    stlInt32          sTmpDigitCount;
    stlInt32          sTmpExponent;
    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    
    /* dtlNumericType -> dtfNumericType */
    sNumeric1 = DTF_NUMERIC( sValue1 );
    sNumeric2 = DTF_NUMERIC( sValue2 );
    sResult = DTF_NUMERIC( sResultValue );

#define DTF_MOD_GET_NUMERIC_INFO( aNumeric,                                             \
                                  aLength,                                              \
                                  aIsPositive,                                          \
                                  aDigitCount,                                          \
                                  aExponent,                                            \
                                  aDigitPtr )                                           \
    {                                                                                   \
        if( DTL_NUMERIC_IS_POSITIVE( (aNumeric) ) )                                     \
        {                                                                               \
            (aIsPositive) = STL_TRUE;                                                   \
            (aDigitCount) = DTL_NUMERIC_GET_DIGIT_COUNT( (aLength) );                   \
            (aExponent) = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( (aNumeric) );   \
            (aDigitPtr) = DTL_NUMERIC_GET_DIGIT_PTR( (aNumeric) );                      \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (aIsPositive) = STL_FALSE;                                                  \
            (aDigitCount) = DTL_NUMERIC_GET_DIGIT_COUNT( (aLength) );                   \
            (aExponent) = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( (aNumeric) );   \
            (aDigitPtr) = DTL_NUMERIC_GET_DIGIT_PTR( (aNumeric) );                      \
        }                                                                               \
    }
    


    /**
     * 기본정보 획득
     */

    STL_TRY_THROW( DTL_NUMERIC_IS_ZERO( sNumeric2, sValue2->mLength ) == STL_FALSE, ERROR_DIV_BY_ZERO );

    if( DTL_NUMERIC_IS_ZERO( sNumeric1, sValue1->mLength ) == STL_TRUE )
    {
        DTL_NUMERIC_SET_ZERO( sResult, sResultValue->mLength );
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }
    
    DTF_MOD_GET_NUMERIC_INFO( sNumeric1,
                              sValue1->mLength,
                              sLeftIsPositive,
                              sLeftDigitCount,
                              sLeftExponent,
                              sLeftDigitPtr );
    
    DTF_MOD_GET_NUMERIC_INFO( sNumeric2,
                              sValue2->mLength,
                              sRightIsPositive,
                              sRightDigitCount,
                              sRightExponent,
                              sRightDigitPtr );
    
    /**
     * Modulus 연산 1
     */
    
    if( sLeftExponent < sRightExponent )
    {
        /**
         * Left Value가 Right Value보다 작은 경우
         * ----------------------------
         *  몫이 0이므로 Left Value가 결과값이다.
         *  부호의 경우 Left Value를 따라간다.
         */
        
        stlMemcpy( sResult, sNumeric1, sValue1->mLength );
        sResultValue->mLength = sValue1->mLength;
        
        STL_THROW( RAMP_FINISH );
    }
    else if( sLeftExponent == sRightExponent )
    {
        sLeftDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric1 );
        sRightDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric2 );

        sTable = dtlNumericLenCompTable[ sValue1->mLength ][ sValue2->mLength ];

        for( sLoop = 0 ; sLoop < sTable[1] ; sLoop++ )
        {
            if( DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sLeftIsPositive, sLeftDigitPtr[sLoop] ) !=
                DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sRightIsPositive, sRightDigitPtr[sLoop] ) )
            {
                if( DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sLeftIsPositive, sLeftDigitPtr[sLoop] ) <
                    DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sRightIsPositive, sRightDigitPtr[sLoop] ) )
                {
                    stlMemcpy( sResult, sNumeric1, sValue1->mLength );
                    sResultValue->mLength = sValue1->mLength;
                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    break;
                }
            }
        }

        if( sLoop == sTable[1] )
        {
            if( sTable[0] < 0 )
            {
                /* right가 더 digit이 많은 경우 */
                stlMemcpy( sResult, sNumeric1, sValue1->mLength );
                sResultValue->mLength = sValue1->mLength;
                STL_THROW( RAMP_FINISH );
            }
            else
            {
                if( sTable[0] == 0 )
                {
                    DTL_NUMERIC_SET_ZERO( sResult, sResultValue->mLength );
                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    /* left가 더 digit이 많은 경우 */
                    /* Do Nothing */
                }
            }
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    
    /**
     * Modulus 연산 2
     * Left Value - ( TRUNC( Left Value / Right Value ) * Right Value ) 
     */
    
    /**
     * Left / Right Value 의 int arr 생성.
     */
    
    for( sLoop = 0; sLoop < sLeftDigitCount; sLoop++ )
    {
        sLeftIntArr[sLoop] = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sLeftIsPositive, sLeftDigitPtr[sLoop] );
    }
    
    for( sLoop = 0; sLoop < sRightDigitCount; sLoop++ )
    {
        sRightIntArr[sLoop] = DTL_NUMERIC_GET_VALUE_FROM_DIGIT( sRightIsPositive, sRightDigitPtr[sLoop] );
    }
    
    /**
     * Left Value / Right Value의 몫을 정수형태로 구한다.
     */

    STL_TRY( dtfNumericIntArrDivisionForMod( sLeftIsPositive,
                                             sLeftExponent,
                                             sLeftDigitCount,
                                             sLeftIntArr,
                                             sRightIsPositive,
                                             sRightExponent,
                                             sRightDigitCount,
                                             sRightIntArr,
                                             & sTmpIsPositive,
                                             & sTmpExponent,
                                             & sTmpDigitCount,
                                             sTmpIntArr,
                                             sErrorStack )
             == STL_SUCCESS );
    
    /**
     * Right Value와 sQuot를 곱한다.
     */

    /* Right Value의 소수점이하 digit count 정보 */
    
    if( sRightExponent < 0 )
    {
        sRightDigitCountAfterDecimal = (sRightDigitCount - sRightExponent - 1);        
    }
    else if( sRightDigitCount > (sRightExponent + 1) )
    {
        sRightDigitCountAfterDecimal = (sRightDigitCount - sRightExponent - 1);        
    }
    else
    {
        sRightDigitCountAfterDecimal = 0;
    }
    
    STL_TRY( dtfNumericIntArrMultiplicationForMod( sTmpIsPositive,
                                                   sTmpExponent,
                                                   sTmpDigitCount,
                                                   sTmpIntArr,
                                                   sRightIsPositive,
                                                   sRightExponent,
                                                   sRightDigitCount,
                                                   sRightIntArr,
                                                   sRightDigitCountAfterDecimal,
                                                   & sTmpIsPositive,
                                                   & sTmpExponent,
                                                   & sTmpDigitCount,
                                                   sTmpIntArr,
                                                   sErrorStack )
             == STL_SUCCESS );
    
    /**
     * sNumeric1과 sMul을 뺀다.
     */
    
    STL_TRY( dtfNumericIntArrSubtractionForMod( sLeftIsPositive,
                                                sLeftExponent,
                                                sLeftDigitCount,
                                                sLeftIntArr,
                                                sTmpIsPositive,
                                                sTmpExponent,
                                                sTmpDigitCount,
                                                sTmpIntArr,
                                                & sTmpIsPositive,
                                                & sTmpExponent,
                                                & sTmpDigitCount,
                                                sTmpIntArr,
                                                sErrorStack )
             == STL_SUCCESS );
    
    /**
     * 결과를 numeric type value로 생성.
     */

    STL_DASSERT( sTmpDigitCount <= DTL_NUMERIC_MAX_DIGIT_COUNT );    
    
    sResultDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sResult );
    
    sResultValue->mLength = sTmpDigitCount + 1;

    if( ( sTmpDigitCount == 0 ) || ( sTmpExponent < DTL_NUMERIC_MIN_EXPONENT ) )
    {
        DTL_NUMERIC_SET_ZERO( sResult, sResultValue->mLength );
    }
    else
    {
        for( sLoop = 0; sLoop < sTmpDigitCount; sLoop++ )
        {
            *sResultDigitPtr =
                DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sTmpIsPositive, sTmpIntArr[sLoop] );
            sResultDigitPtr++;
        }
        
        DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sResult, sTmpIsPositive, sTmpExponent );    
    }
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_DIV_BY_ZERO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMERIC_DIVISION_BY_ZERO,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/******************************************************************************
 * NUMERIC MOD 연산을 위한 내부 함수 ( 나누기 )
 ******************************************************************************/

/**
 * @brief Numeric Mod 연산을 위한 Int Arry( 2 digit ) 에 대한 나눗셈 연산 <BR>
 *        ( 결과는 정수이며, 유효숫자는 실제계산결과길이만큼 )
 * @param[in]  aLeftIsPositive    Left Value 부호
 * @param[in]  aLeftExponent      Left Value Exponent
 * @param[in]  aLeftDigitCount    Left Value Digit Count
 * @param[in]  aLeftIntArr        Left Value Int Arr
 * @param[in]  aRightIsPositive   Right Value 부호
 * @param[in]  aRightExponent     Right Value Exponent
 * @param[in]  aRightDigitCount   Right Value Digit Count
 * @param[in]  aRightIntArr       Right Value Int Arr
 * @param[out] aResultIsPositive  Result Value 부호
 * @param[out] aResultExponent    Result Value Exponent
 * @param[out] aResultDigitCount  Result Value Digit Count
 * @param[out] aResultIntArr      나눗셈 연산 결과 Result Value Int Arr 
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtfNumericIntArrDivisionForMod( stlBool               aLeftIsPositive,
                                          stlInt32              aLeftExponent,
                                          stlInt32              aLeftDigitCount,
                                          stlInt8             * aLeftIntArr,
                                          stlBool               aRightIsPositive,
                                          stlInt32              aRightExponent,
                                          stlInt32              aRightDigitCount,
                                          stlInt8             * aRightIntArr,
                                          stlBool             * aResultIsPositive,
                                          stlInt32            * aResultExponent,
                                          stlInt32            * aResultDigitCount,
                                          stlInt8             * aResultIntArr,
                                          stlErrorStack       * aErrorStack )
{
    stlBool           sResultIsPositive;
    stlInt32          sResultExponent;
    stlInt32          sResultDigitCount;

    stlInt32          sDivDigitCount;

    stlInt8         * sDividend;
    stlInt8         * sDivisor;
    
    stlInt32          sDivisor1;
    stlInt32          sDivisor2;
    stlInt32          sCarry;
    stlInt32          sBorrow;    
    stlInt32          sFactor;
    stlInt32          sNext2Digits;
    stlInt32          sQhat;
//    stlInt32          sDecimalDigit;

    stlInt32          i;
    stlInt32          j;

    stlInt8           sDividendIntArr[ DTF_MOD_INTERMEDIATE_RESULT_INT_ARR_MAX_COUNT ] = {0, };

    
    /*
     * Left Value와 Right Value는 0이 아닌 값이어야 한다.
     */
    
    STL_DASSERT( ( aLeftDigitCount > 0 ) && ( aRightDigitCount > 0 ) );
    
    /*
     * result value에 대한 정보.
     */
    
    sResultIsPositive = (aLeftIsPositive == aRightIsPositive) ? STL_TRUE : STL_FALSE;
    sResultExponent = aLeftExponent - aRightExponent;
    sResultDigitCount = sResultExponent + 1 + ( DTF_MOD_INT_ARR_DIGITS - 1 ) / DTF_MOD_INT_ARR_DIGITS;
    sResultDigitCount = STL_MAX( sResultDigitCount, 1 );
    
    /*
     * 중간 결과 처리를 위한 정보
     */
    
    sDivDigitCount = sResultDigitCount + aRightDigitCount;
    sDivDigitCount = STL_MAX( sDivDigitCount, aLeftDigitCount);

    /*
     * We need a workspace with room for the working dividend (div_ndigits+1
     * digits) plus room for the possibly-normalized divisor (var2ndigits
     * digits).  It is convenient also to have a zero at divisor[0] with the
     * actual divisor data in divisor[1 .. var2ndigits].  Transferring the
     * digits into the workspace also allows us to realloc the result (which
     * might be the same as either input var) before we begin the main loop.
     * Note that we use palloc0 to ensure that divisor[0], dividend[0], and
     * any additional dividend positions beyond var1ndigits, start out 0.
     */
    
    STL_DASSERT( (sDivDigitCount + aRightDigitCount + 2) <= DTF_MOD_INTERMEDIATE_RESULT_INT_ARR_MAX_COUNT );
    
    sDividend = sDividendIntArr;
    sDivisor = sDividend + sDivDigitCount + 1;

    stlMemcpy( sDividend + 1, aLeftIntArr, aLeftDigitCount );
    stlMemcpy( sDivisor + 1, aRightIntArr, aRightDigitCount );
    
    /*
     * DIVIDE
     */ 
    
    if( aRightDigitCount == 1 )
    {
        /*
         * If there's only a single divisor digit, we can use a fast path (cf.
         * Knuth section 4.3.1 exercise 16).
         */
        
        sDivisor1 = sDivisor[1];
        sCarry = 0;
        
        for( i = 0; i < sResultDigitCount; i++ )
        {
            sCarry = sCarry * DTF_MOD_INT_ARRY_BASE_VALUE + sDividend[i + 1];
            aResultIntArr[i] = sCarry / sDivisor1;
            sCarry = sCarry - ( sCarry / sDivisor1 * sDivisor1 );
        }
        
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * sRightDigitCount > 1
     */

    /*
     * The full multiple-place algorithm is taken from Knuth volume 2,
     * Algorithm 4.3.1D.
     *
     * We need the first divisor digit to be >= NBASE/2.  If it isn't,
     * make it so by scaling up both the divisor and dividend by the
     * factor "d".	(The reason for allocating dividend[0] above is to
     * leave room for possible carry here.)
     */
    if( sDivisor[1] < DTF_MOD_INT_ARRY_HALF_BASE_VALUE )
    {
        sFactor = DTF_MOD_INT_ARRY_BASE_VALUE / (sDivisor[1] + 1);
        
        sCarry = 0;
        for( i = aRightDigitCount; i > 0; i-- )
        {
            sCarry += sDivisor[i] * sFactor;
            sDivisor[i] = sCarry - ( sCarry / DTF_MOD_INT_ARRY_BASE_VALUE * DTF_MOD_INT_ARRY_BASE_VALUE );
            sCarry = sCarry / DTF_MOD_INT_ARRY_BASE_VALUE;
        }
        
        STL_DASSERT( sCarry == 0 );
        
        sCarry = 0;
        /* at this point only var1ndigits of dividend can be nonzero */
        for( i = aLeftDigitCount; i >= 0; i-- )
        {
            sCarry += sDividend[i] * sFactor;
            sDividend[i] = sCarry - ( sCarry / DTF_MOD_INT_ARRY_BASE_VALUE * DTF_MOD_INT_ARRY_BASE_VALUE );
            sCarry = sCarry / DTF_MOD_INT_ARRY_BASE_VALUE;
        }
        
        STL_DASSERT( sCarry == 0 );
        STL_DASSERT( sDivisor[1] >= DTF_MOD_INT_ARRY_HALF_BASE_VALUE );
    }
    else
    {
        /* Do Nothing */
    }

    sDivisor1 = sDivisor[1];
    sDivisor2 = sDivisor[2];

    /*
     * Begin the main loop.  Each iteration of this loop produces the j'th
     * quotient digit by dividing dividend[j .. j + var2ndigits] by the
     * divisor; this is essentially the same as the common manual
     * procedure for long division.
     */
    for( j = 0; j < sResultDigitCount; j++ )
    {
        /* Estimate quotient digit from the first two dividend digits */
        sNext2Digits = sDividend[j] * DTF_MOD_INT_ARRY_BASE_VALUE + sDividend[j+1];

        /*
         * If next2digits are 0, then quotient digit must be 0 and there's
         * no need to adjust the working dividend.	It's worth testing
         * here to fall out ASAP when processing trailing zeroes in a
         * dividend.
         */
        if( sNext2Digits == 0 )
        {
            aResultIntArr[j] = 0;
            continue;
        }
        else
        {
            /* Do Nothing */
        }

        if( sDividend[j] == sDivisor1 )
        {
            sQhat = DTF_MOD_INT_ARRY_BASE_VALUE - 1;
        }
        else
        {
            sQhat = sNext2Digits / sDivisor1;
        }

        /*
         * Adjust quotient digit if it's too large.  Knuth proves that
         * after this step, the quotient digit will be either correct or
         * just one too large.	(Note: it's OK to use dividend[j+2] here
         * because we know the divisor length is at least 2.)
         */
        while( ( sDivisor2 * sQhat ) >
               ( ( sNext2Digits - sQhat * sDivisor1 ) * DTF_MOD_INT_ARRY_BASE_VALUE + sDividend[j+2] ) )
        {
            sQhat--;
        }

        /* As above, need do nothing more when quotient digit is 0 */
        if( sQhat > 0 )
        {
            /*
             * Multiply the divisor by qhat, and subtract that from the
             * working dividend.  "carry" tracks the multiplication,
             * "borrow" the subtraction (could we fold these together?)
             */
            sCarry = 0;
            sBorrow = 0;

            for( i = aRightDigitCount; i >= 0; i-- )
            {
                sCarry += sDivisor[i] * sQhat;
                sBorrow -= sCarry - ( sCarry / DTF_MOD_INT_ARRY_BASE_VALUE * DTF_MOD_INT_ARRY_BASE_VALUE );
                sCarry = sCarry / DTF_MOD_INT_ARRY_BASE_VALUE;
                sBorrow += sDividend[j+i];

                if( sBorrow < 0 )
                {
                    sDividend[j+i] = sBorrow + DTF_MOD_INT_ARRY_BASE_VALUE;
                    sBorrow = -1;
                }
                else
                {
                    sDividend[j+i] = sBorrow;
                    sBorrow = 0;
                }
            }
            
            STL_DASSERT( sCarry == 0 );

            /*
             * If we got a borrow out of the top dividend digit, then
             * indeed qhat was one too large.  Fix it, and add back the
             * divisor to correct the working dividend.  (Knuth proves
             * that this will occur only about 3/NBASE of the time; hence,
             * it's a good idea to test this code with small NBASE to be
             * sure this section gets exercised.)
             */
            if( sBorrow )
            {
                sQhat--;
                sCarry = 0;
                
                for( i = aRightDigitCount; i >= 0; i-- )
                {
                    sCarry += sDividend[j+i] + sDivisor[i];
                    if( sCarry >= DTF_MOD_INT_ARRY_BASE_VALUE )
                    {
                        sDividend[j+i] = sCarry - DTF_MOD_INT_ARRY_BASE_VALUE;
                        sCarry = 1;
                    }
                    else
                    {
                        sDividend[j+i] = sCarry;
                        sCarry = 0;
                    }
                }

                /* A carry should occur here to cancel the borrow above */
                STL_DASSERT( sCarry == 1 );
                
            }
            else
            {
                /* Do Nothing */
            }
        }

        aResultIntArr[j] = sQhat;        
    }

    STL_RAMP( RAMP_FINISH );

    dtfIntArrRemoveLeadingAndTrailingZeroForMod( & sResultIsPositive,
                                                 & sResultExponent,
                                                 & sResultDigitCount,
                                                 aResultIntArr );

    *aResultIsPositive = sResultIsPositive;
    *aResultExponent = sResultExponent;
    *aResultDigitCount = sResultDigitCount;

    return STL_SUCCESS;

    /* STL_FINISH; */

    /* return STL_FAILURE; */
}


/******************************************************************************
 * NUMERIC MOD 연산을 위한 내부 함수 ( 곱하기 )
 ******************************************************************************/

/**
 * @brief Numeric Mod 연산을 위한 Int Arry( 2 digit ) 에 대한 곱셈 연산 <BR>
 *        ( 유효숫자는 실제계산결과길이만큼 )
 * @param[in]  aLeftIsPositive    Left Value 부호
 * @param[in]  aLeftExponent      Left Value Exponent
 * @param[in]  aLeftDigitCount    Left Value Digit Count
 * @param[in]  aLeftIntArr        Left Value Int Arr
 * @param[in]  aRightIsPositive   Right Value 부호
 * @param[in]  aRightExponent     Right Value Exponent
 * @param[in]  aRightDigitCount   Right Value Digit Count
 * @param[in]  aRightIntArr       Right Value Int Arr
 * @param[in]  aRightDigitCountAfterDecimal     Right Value 이 소수점 이하 digit count
 * @param[out] aResultIsPositive  Result Value 부호
 * @param[out] aResultExponent    Result Value Exponent
 * @param[out] aResultDigitCount  Result Value Digit Count
 * @param[out] aResultIntArr      나눗셈 연산 결과 Result Value Int Arr 
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtfNumericIntArrMultiplicationForMod( stlBool               aLeftIsPositive,
                                                stlInt32              aLeftExponent,
                                                stlInt32              aLeftDigitCount,
                                                stlInt8             * aLeftIntArr,
                                                stlBool               aRightIsPositive,
                                                stlInt32              aRightExponent,
                                                stlInt32              aRightDigitCount,
                                                stlInt8             * aRightIntArr,
                                                stlInt32              aRightDigitCountAfterDecimal,
                                                stlBool             * aResultIsPositive,
                                                stlInt32            * aResultExponent,
                                                stlInt32            * aResultDigitCount,
                                                stlInt8             * aResultIntArr,
                                                stlErrorStack       * aErrorStack )
{
    stlBool           sResultIsPositive;
    stlInt32          sResultExponent;
    stlInt32          sResultDigitCount;
    stlInt32          sMaxDigitCount;

    stlInt32          sMaxDig;
    stlInt32          sNewDig;
    stlInt32          sDig[ DTF_MOD_INTERMEDIATE_RESULT_INT_ARR_MAX_COUNT ] = {0, };

    stlInt32          sResultIdx;
    stlInt32          sLeftIdx;
    stlInt32          sRightIdx;
    stlInt32          i;
    stlInt32          sValue;
    stlInt32          sCarry;
    

    if( ( aLeftDigitCount == 0 ) || ( aRightDigitCount == 0 ) )
    {
        sResultIsPositive = STL_TRUE;
        sResultExponent = 0;
        sResultDigitCount = 0;
        aResultIntArr[0] = 0;
        
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * result value에 대한 정보.
     */
    
    sResultIsPositive = (aLeftIsPositive == aRightIsPositive) ? STL_TRUE : STL_FALSE;
    sResultExponent = aLeftExponent + aRightExponent + 2;

    /*
     * Determine number of result digits to compute.  If the exact result
     * would have more than rscale fractional digits, truncate the computation
     * with MUL_GUARD_DIGITS guard digits.	We do that by pretending that one
     * or both inputs have fewer digits than they really do.
     */
    
    sResultDigitCount = aLeftDigitCount + aRightDigitCount + 1;
    sMaxDigitCount = sResultExponent + 1 + ( aRightDigitCountAfterDecimal * DTF_MOD_INT_ARR_DIGITS ) + DTF_MOD_MUL_GUARD_DIGITS;

    if( sResultDigitCount > sMaxDigitCount )
    {
        if( sMaxDigitCount < 3 )
        {
            /* no useful precision at all in the result... */
            sResultIsPositive = STL_TRUE;
            sResultExponent = 0;
            sResultDigitCount = 0;
            aResultIntArr[0] = 0;
            
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /* Do Nothing */
        }

        /* force maxdigits odd so that input ndigits can be equal */
        if( ( sMaxDigitCount & 1 ) == 0 )
        {
            STL_DASSERT( (sMaxDigitCount + 1) <= DTF_MOD_INTERMEDIATE_RESULT_INT_ARR_MAX_COUNT );    
            sMaxDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
        
        if( aLeftDigitCount > aRightDigitCount )
        {
            aLeftDigitCount -= sResultDigitCount - sMaxDigitCount;
            if( aLeftDigitCount < aRightDigitCount )
            {
                aLeftDigitCount = ( aLeftDigitCount + aRightDigitCount ) / 2;
                aRightDigitCount = aLeftDigitCount;
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            aRightDigitCount -= sResultDigitCount - sMaxDigitCount;
            if( aRightDigitCount < aLeftDigitCount )
            {
                aLeftDigitCount = ( aLeftDigitCount + aRightDigitCount ) / 2;
                aRightDigitCount = aLeftDigitCount;
            }
            else
            {
                /* Do Nothing */
            }
        }

        sResultDigitCount = sMaxDigitCount;
        STL_DASSERT( sResultDigitCount == ( aLeftDigitCount + aRightDigitCount + 1 ) );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * We do the arithmetic in an array "dig[]" of signed int's.  Since
     * INT_MAX is noticeably larger than NBASE*NBASE, this gives us headroom
     * to avoid normalizing carries immediately.
     *
     * maxdig tracks the maximum possible value of any dig[] entry; when this
     * threatens to exceed INT_MAX, we take the time to propagate carries. To
     * avoid overflow in maxdig itself, it actually represents the max
     * possible value divided by NBASE-1.
     */
    
    sMaxDig = 0;
    sResultIdx = sResultDigitCount - 1;
    for( sLeftIdx = (aLeftDigitCount - 1); sLeftIdx >= 0; sResultIdx--, sLeftIdx-- )
    {
        sValue = aLeftIntArr[sLeftIdx];

        if( sValue == 0 )
        {
            continue;
        }
        else
        {
            /* Do Nothing */
        }
        
        sMaxDig += sValue;
        if( sMaxDig > ( STL_INT32_MAX / (DTF_MOD_INT_ARRY_BASE_VALUE - 1) ) )
        {
            sCarry = 0;
            for( i = (sResultDigitCount - 1); i >= 0; i-- )
            {
                sNewDig = sDig[i] + sCarry;
                if( sNewDig >= DTF_MOD_INT_ARRY_BASE_VALUE )
                {
                    sCarry = sNewDig / DTF_MOD_INT_ARRY_BASE_VALUE;
                    sNewDig -= sCarry * DTF_MOD_INT_ARRY_BASE_VALUE;
                }
                else
                {
                    sCarry = 0;
                }

                sDig[i] = sNewDig;
            }

            STL_DASSERT( sCarry == 0 );
            /* Reset maxdig to indicate new worst-case */
            sMaxDig = 1 + sValue;
        }
        else
        {
            /* Do Nothing */
        }

        /* Add appropriate multiple of var2 into the accumulator */
        i = sResultIdx;
        for( sRightIdx = (aRightDigitCount - 1); sRightIdx >= 0; sRightIdx-- )
        {
            sDig[i--] += sValue * aRightIntArr[sRightIdx];
        }
    }

    /*
     * Now we do a final carry propagation pass to normalize the result, which
     * we combine with storing the result digits into the output. Note that
     * this is still done at full precision w/guard digits.
     */

    sCarry = 0;
    for( i = (sResultDigitCount - 1); i >= 0; i-- )
    {
        sNewDig = sDig[i] + sCarry;
        if( sNewDig >= DTF_MOD_INT_ARRY_BASE_VALUE )
        {
            sCarry = sNewDig / DTF_MOD_INT_ARRY_BASE_VALUE;
            sNewDig -= sCarry * DTF_MOD_INT_ARRY_BASE_VALUE;
        }
        else
        {
            sCarry = 0;
        }

        aResultIntArr[i] = sNewDig;
    }

    STL_DASSERT( sCarry == 0 );

    /*
     * 정수 * numeric 인 연산이므로  ROUND 적용할 필요없이
     * 위에서 계산된 결과를 주면 됨.
     */

    STL_RAMP( RAMP_FINISH );
    
    dtfIntArrRemoveLeadingAndTrailingZeroForMod( & sResultIsPositive,
                                                 & sResultExponent,
                                                 & sResultDigitCount,
                                                 aResultIntArr );
    
    *aResultIsPositive = sResultIsPositive;
    *aResultExponent = sResultExponent;
    *aResultDigitCount = sResultDigitCount;    
    
    return STL_SUCCESS;
    
    /* STL_FINISH; */
    
    /* return STL_FAILURE; */
}


/******************************************************************************
 * NUMERIC MOD 연산을 위한 내부 함수 ( 빼기 )
 ******************************************************************************/

/**
 * @brief Numeric Mod 연산을 위한 Int Arry( 2 digit ) 에 대한 빼기 연산 <BR>
 *        ( 유효숫자는 실제계산결과길이만큼 )
 * @param[in]  aLeftIsPositive    Left Value 부호
 * @param[in]  aLeftExponent      Left Value Exponent
 * @param[in]  aLeftDigitCount    Left Value Digit Count
 * @param[in]  aLeftIntArr        Left Value Int Arr
 * @param[in]  aRightIsPositive   Right Value 부호
 * @param[in]  aRightExponent     Right Value Exponent
 * @param[in]  aRightDigitCount   Right Value Digit Count
 * @param[in]  aRightIntArr       Right Value Int Arr
 * @param[out] aResultIsPositive  Result Value 부호
 * @param[out] aResultExponent    Result Value Exponent
 * @param[out] aResultDigitCount  Result Value Digit Count
 * @param[out] aResultIntArr      나눗셈 연산 결과 Result Value Int Arr 
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtfNumericIntArrSubtractionForMod( stlBool               aLeftIsPositive,
                                             stlInt32              aLeftExponent,
                                             stlInt32              aLeftDigitCount,
                                             stlInt8             * aLeftIntArr,
                                             stlBool               aRightIsPositive,
                                             stlInt32              aRightExponent,
                                             stlInt32              aRightDigitCount,
                                             stlInt8             * aRightIntArr,
                                             stlBool             * aResultIsPositive,
                                             stlInt32            * aResultExponent,
                                             stlInt32            * aResultDigitCount,
                                             stlInt8             * aResultIntArr,
                                             stlErrorStack       * aErrorStack )
{
    stlInt32 sCmpResult = 0;
    
    if( aLeftIsPositive == STL_TRUE )
    {
        if( aRightIsPositive == STL_FALSE )
        {
            /* ----------
             * var1 is positive, var2 is negative
             * result = +(ABS(var1) + ABS(var2))
             * ----------
             */

            STL_TRY( dtfNumericIntArrAddAbsForMod( aLeftIsPositive,
                                                   aLeftExponent,
                                                   aLeftDigitCount,
                                                   aLeftIntArr,
                                                   aRightIsPositive,
                                                   aRightExponent,
                                                   aRightDigitCount,
                                                   aRightIntArr,
                                                   aResultIsPositive,
                                                   aResultExponent,
                                                   aResultDigitCount,
                                                   aResultIntArr,
                                                   aErrorStack )
                     == STL_SUCCESS );
            
            *aResultIsPositive = STL_TRUE;            
        }
        else
        {
            /* ----------
             * Both are positive
             * Must compare absolute values
             * ----------
             */
            
            sCmpResult = dtfNumericIntArrCmpAbsForMod( aLeftExponent,
                                                       aLeftDigitCount,
                                                       aLeftIntArr,
                                                       aRightExponent,
                                                       aRightDigitCount,
                                                       aRightIntArr );
            
            switch( sCmpResult )
            {
                case 0 :
                    {
                        /* ----------
                         * ABS(var1) == ABS(var2)
                         * result = ZERO
                         * ----------
                         */
                        
                        *aResultIsPositive = STL_TRUE;
                        *aResultExponent = 0;
                        *aResultDigitCount = 0;
                        aResultIntArr[0] = 0;
                        
                        break;
                    }
                case 1 :
                    {
                        /* ----------
                         * ABS(var1) > ABS(var2)
                         * result = +(ABS(var1) - ABS(var2))
                         * ----------
                         */

                        STL_TRY( dtfNumericIntArrSubAbsForMod( aLeftIsPositive,
                                                               aLeftExponent,
                                                               aLeftDigitCount,
                                                               aLeftIntArr,
                                                               aRightIsPositive,
                                                               aRightExponent,
                                                               aRightDigitCount,
                                                               aRightIntArr,
                                                               aResultIsPositive,
                                                               aResultExponent,
                                                               aResultDigitCount,
                                                               aResultIntArr,
                                                               aErrorStack )
                                 == STL_SUCCESS );

                        *aResultIsPositive = STL_TRUE;
                        
                        break;
                    }
                case -1 :
                    {
                        /* ----------
                         * ABS(var1) < ABS(var2)
                         * result = -(ABS(var2) - ABS(var1))
                         * ----------
                         */
                        
                        STL_TRY( dtfNumericIntArrSubAbsForMod( aRightIsPositive,
                                                               aRightExponent,
                                                               aRightDigitCount,
                                                               aRightIntArr,
                                                               aLeftIsPositive,
                                                               aLeftExponent,
                                                               aLeftDigitCount,
                                                               aLeftIntArr,
                                                               aResultIsPositive,
                                                               aResultExponent,
                                                               aResultDigitCount,
                                                               aResultIntArr,
                                                               aErrorStack )
                                 == STL_SUCCESS );
                        
                        *aResultIsPositive = STL_FALSE;
                        
                        break;
                    }
                default :
                    /* Do Nothing */
                    break;
            }
        }
    }
    else
    {
        if( aRightIsPositive == STL_FALSE )
        {
            /* ----------
             * Both are negative
             * Must compare absolute values
             * ----------
             */

            sCmpResult = dtfNumericIntArrCmpAbsForMod( aLeftExponent,
                                                       aLeftDigitCount,
                                                       aLeftIntArr,
                                                       aRightExponent,
                                                       aRightDigitCount,
                                                       aRightIntArr );
            
            switch( sCmpResult )
            {
                case 0 :
                    {
                        /* ----------
                         * ABS(var1) == ABS(var2)
                         * result = ZERO
                         * ----------
                         */

                        *aResultIsPositive = STL_TRUE;
                        *aResultExponent = 0;
                        *aResultDigitCount = 0;
                        aResultIntArr[0] = 0;
                        
                        break;
                    }
                case 1 :
                    {
                        /* ----------
                         * ABS(var1) > ABS(var2)
                         * result = -(ABS(var1) - ABS(var2))
                         * ----------
                         */

                        STL_TRY( dtfNumericIntArrSubAbsForMod( aLeftIsPositive,
                                                               aLeftExponent,
                                                               aLeftDigitCount,
                                                               aLeftIntArr,
                                                               aRightIsPositive,
                                                               aRightExponent,
                                                               aRightDigitCount,
                                                               aRightIntArr,
                                                               aResultIsPositive,
                                                               aResultExponent,
                                                               aResultDigitCount,
                                                               aResultIntArr,
                                                               aErrorStack )
                                 == STL_SUCCESS );

                        *aResultIsPositive = STL_FALSE;
                        
                        break;
                    }
                case -1 :
                    {
                        STL_TRY( dtfNumericIntArrSubAbsForMod( aRightIsPositive,
                                                               aRightExponent,
                                                               aRightDigitCount,
                                                               aRightIntArr,
                                                               aLeftIsPositive,
                                                               aLeftExponent,
                                                               aLeftDigitCount,
                                                               aLeftIntArr,
                                                               aResultIsPositive,
                                                               aResultExponent,
                                                               aResultDigitCount,
                                                               aResultIntArr,
                                                               aErrorStack )
                                 == STL_SUCCESS );
                        
                        *aResultIsPositive = STL_TRUE;
                        
                        break;
                    }
                default:
                    /* Do Nothing */
                    break;
            }
        }
        else
        {
            /* ----------
             * var1 is negative, var2 is positive
             * result = -(ABS(var1) + ABS(var2))
             * ----------
             */
            
            STL_TRY( dtfNumericIntArrAddAbsForMod( aLeftIsPositive,
                                                   aLeftExponent,
                                                   aLeftDigitCount,
                                                   aLeftIntArr,
                                                   aRightIsPositive,
                                                   aRightExponent,
                                                   aRightDigitCount,
                                                   aRightIntArr,
                                                   aResultIsPositive,
                                                   aResultExponent,
                                                   aResultDigitCount,
                                                   aResultIntArr,
                                                   aErrorStack )
                     == STL_SUCCESS );
            
            *aResultIsPositive = STL_FALSE;
        }
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief Numeric Mod 연산을 위한 Int Arry( 2 digit ) 에 대한 ABS 덧셈 연산 <BR>
 *        ( 유효숫자는 실제계산결과길이만큼 )
 * @param[in]  aLeftIsPositive    Left Value 부호
 * @param[in]  aLeftExponent      Left Value Exponent
 * @param[in]  aLeftDigitCount    Left Value Digit Count
 * @param[in]  aLeftIntArr        Left Value Int Arr
 * @param[in]  aRightIsPositive   Right Value 부호
 * @param[in]  aRightExponent     Right Value Exponent
 * @param[in]  aRightDigitCount   Right Value Digit Count
 * @param[in]  aRightIntArr       Right Value Int Arr
 * @param[out] aResultIsPositive  Result Value 부호
 * @param[out] aResultExponent    Result Value Exponent
 * @param[out] aResultDigitCount  Result Value Digit Count
 * @param[out] aResultIntArr      나눗셈 연산 결과 Result Value Int Arr 
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtfNumericIntArrAddAbsForMod( stlBool               aLeftIsPositive,
                                        stlInt32              aLeftExponent,
                                        stlInt32              aLeftDigitCount,
                                        stlInt8             * aLeftIntArr,
                                        stlBool               aRightIsPositive,
                                        stlInt32              aRightExponent,
                                        stlInt32              aRightDigitCount,
                                        stlInt8             * aRightIntArr,
                                        stlBool             * aResultIsPositive,
                                        stlInt32            * aResultExponent,
                                        stlInt32            * aResultDigitCount,
                                        stlInt8             * aResultIntArr,
                                        stlErrorStack       * aErrorStack )
{
    stlBool   sResultIsPositive;
    stlInt32  sResultExponent;
    stlInt32  sResultDigitCount;

    stlInt32  sLeftDigitCountAfterDecimal;
    stlInt32  sRightDigitCountAfterDecimal;
    stlInt32  sResultDigitCountAfterDecimal;

    stlInt32  i;
    stlInt32  sLeftIdx;
    stlInt32  sRightIdx;
    stlInt32  sCarry = 0;


    sResultExponent = STL_MAX( aLeftExponent, aRightExponent ) + 1;

    sLeftDigitCountAfterDecimal = aLeftDigitCount - aLeftExponent - 1;
    sRightDigitCountAfterDecimal = aRightDigitCount - aRightExponent - 1;
    sResultDigitCountAfterDecimal = STL_MAX( sLeftDigitCountAfterDecimal, sRightDigitCountAfterDecimal );

    sResultDigitCount = sResultDigitCountAfterDecimal + sResultExponent + 1;
    sResultDigitCount = STL_MAX( sResultDigitCount, 1 );

    /* sResultIntArr[0] = 0; /\* spare digit for later rounding *\/ */
    /* sResultIntArr++; */
    
    sLeftIdx = sResultDigitCountAfterDecimal + aLeftExponent + 1;
    sRightIdx = sResultDigitCountAfterDecimal + aRightExponent + 1;
    for( i = (sResultDigitCount - 1); i >= 0; i-- )
    {
        sLeftIdx--;
        sRightIdx--;
        
        if( ( sLeftIdx >= 0 ) && ( sLeftIdx < aLeftDigitCount ) )
        {
            sCarry += aLeftIntArr[sLeftIdx];
        }
        
        if( ( sRightIdx >= 0 ) && ( sRightIdx < aRightDigitCount ) )
        {
            sCarry += aRightIntArr[sRightIdx];
        }
        
        if( sCarry >= DTF_MOD_INT_ARRY_BASE_VALUE )
        {
            aResultIntArr[i] = sCarry - DTF_MOD_INT_ARRY_BASE_VALUE;
            sCarry = 1;
        }
        else
        {
            aResultIntArr[i] = sCarry;
            sCarry = 0;
        }
    }
    
    STL_DASSERT( sCarry == 0 );
    
    dtfIntArrRemoveLeadingAndTrailingZeroForMod( & sResultIsPositive,
                                                 & sResultExponent,
                                                 & sResultDigitCount,
                                                 aResultIntArr );
    
    *aResultIsPositive = sResultIsPositive;
    *aResultExponent = sResultExponent;
    *aResultDigitCount = sResultDigitCount;
    
    return STL_SUCCESS;
    
    /* STL_FINISH; */
    
    /* return STL_FAILURE; */    
}


/**
 * @brief Numeric Mod 연산을 위한 Int Arry( 2 digit ) 에 대한 ABS 뺄셈 연산 <BR>
 *        ( 유효숫자는 실제계산결과길이만큼 )
 * @param[in]  aLeftIsPositive    Left Value 부호
 * @param[in]  aLeftExponent      Left Value Exponent
 * @param[in]  aLeftDigitCount    Left Value Digit Count
 * @param[in]  aLeftIntArr        Left Value Int Arr
 * @param[in]  aRightIsPositive   Right Value 부호
 * @param[in]  aRightExponent     Right Value Exponent
 * @param[in]  aRightDigitCount   Right Value Digit Count
 * @param[in]  aRightIntArr       Right Value Int Arr
 * @param[out] aResultIsPositive  Result Value 부호
 * @param[out] aResultExponent    Result Value Exponent
 * @param[out] aResultDigitCount  Result Value Digit Count
 * @param[out] aResultIntArr      나눗셈 연산 결과 Result Value Int Arr 
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtfNumericIntArrSubAbsForMod( stlBool               aLeftIsPositive,
                                        stlInt32              aLeftExponent,
                                        stlInt32              aLeftDigitCount,
                                        stlInt8             * aLeftIntArr,
                                        stlBool               aRightIsPositive,
                                        stlInt32              aRightExponent,
                                        stlInt32              aRightDigitCount,
                                        stlInt8             * aRightIntArr,
                                        stlBool             * aResultIsPositive,
                                        stlInt32            * aResultExponent,
                                        stlInt32            * aResultDigitCount,
                                        stlInt8             * aResultIntArr,
                                        stlErrorStack       * aErrorStack )
{
    stlBool   sResultIsPositive;
    stlInt32  sResultExponent;
    stlInt32  sResultDigitCount;
    
    stlInt32  sLeftDigitCountAfterDecimal;
    stlInt32  sRightDigitCountAfterDecimal;
    stlInt32  sResultDigitCountAfterDecimal;
    
    stlInt32  i;
    stlInt32  sLeftIdx;
    stlInt32  sRightIdx;
    stlInt32  sBorrow = 0;
    

    sResultExponent = aLeftExponent;
    
    sLeftDigitCountAfterDecimal = aLeftDigitCount - aLeftExponent - 1;
    sRightDigitCountAfterDecimal = aRightDigitCount - aRightExponent - 1;
    sResultDigitCountAfterDecimal = STL_MAX( sLeftDigitCountAfterDecimal, sRightDigitCountAfterDecimal );

    sResultDigitCount = sResultDigitCountAfterDecimal + sResultExponent + 1;
    sResultDigitCount = STL_MAX( sResultDigitCount, 1 );

    /* sResultIntArr[0] = 0; /\* spare digit for later rounding *\/ */
    /* sResultIntArr++; */

    sLeftIdx = sResultDigitCountAfterDecimal + aLeftExponent + 1;
    sRightIdx = sResultDigitCountAfterDecimal + aRightExponent + 1;
    for( i = (sResultDigitCount - 1); i >= 0; i-- )
    {
        sLeftIdx--;
        sRightIdx--;

        if( ( sLeftIdx >= 0 ) && ( sLeftIdx < aLeftDigitCount ) )
        {
            sBorrow += aLeftIntArr[sLeftIdx];
        }
        
        if( ( sRightIdx >= 0 ) && ( sRightIdx < aRightDigitCount ) )
        {
            sBorrow -= aRightIntArr[sRightIdx];
        }

        if( sBorrow < 0 )
        {
            aResultIntArr[i] = sBorrow + DTF_MOD_INT_ARRY_BASE_VALUE;
            sBorrow = -1;
        }
        else
        {
            aResultIntArr[i] = sBorrow;
            sBorrow = 0;
        }
    }

    STL_DASSERT( sBorrow == 0 );
    
    dtfIntArrRemoveLeadingAndTrailingZeroForMod( & sResultIsPositive,
                                                 & sResultExponent,
                                                 & sResultDigitCount,
                                                 aResultIntArr );
    
    *aResultIsPositive = sResultIsPositive;
    *aResultExponent = sResultExponent;
    *aResultDigitCount = sResultDigitCount;
    
    return STL_SUCCESS;
    
    /* STL_FINISH; */
    
    /* return STL_FAILURE; */    
}


/**
 * @brief Numeric Mod 연산을 위한 Int Arry( 2 digit ) 에 대한 비교 연산 <BR>
 *        ( 유효숫자는 실제계산결과길이만큼 )
 * @param[in]  aLeftExponent      Left Value Exponent
 * @param[in]  aLeftDigitCount    Left Value Digit Count
 * @param[in]  aLeftIntArr        Left Value Int Arr
 * @param[in]  aRightExponent     Right Value Exponent
 * @param[in]  aRightDigitCount   Right Value Digit Count
 * @param[in]  aRightIntArr       Right Value Int Arr
 */
stlInt32 dtfNumericIntArrCmpAbsForMod( stlInt32              aLeftExponent,
                                       stlInt32              aLeftDigitCount,
                                       stlInt8             * aLeftIntArr,
                                       stlInt32              aRightExponent,
                                       stlInt32              aRightDigitCount,
                                       stlInt8             * aRightIntArr )
{
    stlInt32   sLeftIdx = 0;
    stlInt32   sRightIdx = 0;
    stlInt32   sResult = 0;
    stlInt32   sSubValue = 0;
    
    
    /* Check any digits before the first common digit */
    while( ( aLeftExponent > aRightExponent ) && ( sLeftIdx < aLeftDigitCount ) )
    {
        if( aLeftIntArr[sLeftIdx++] != 0 )
        {
            sResult = 1;
            STL_THROW( RAMP_FINISH );
        }
        
        aLeftExponent--;
    }
    
    while( ( aRightExponent > aLeftExponent ) && ( sRightIdx < aRightDigitCount ) )
    {
        if( aRightIntArr[sRightIdx++] != 0 )
        {
            sResult = -1;
            STL_THROW( RAMP_FINISH );
        }

        aRightExponent--;
    }

    /* At this point, either w1 == w2 or we've run out of digits */
    
    if( aLeftExponent == aRightExponent )
    {
        while( ( sLeftIdx < aLeftDigitCount ) && ( sRightIdx < aRightDigitCount ) )
        {
            sSubValue = aLeftIntArr[sLeftIdx++] - aRightIntArr[sRightIdx++];

            if( sSubValue > 0 )
            {
                sResult = 1;
                STL_THROW( RAMP_FINISH );
            }
            else if( sSubValue < 0 )
            {
                sResult = -1;
                STL_THROW( RAMP_FINISH );
            }
            else
            {
                /* Do Nothing */
            }
        }
    }

    /*
     * At this point, we've run out of digits on one side or the other; so any
     * remaining nonzero digits imply that side is larger
     */
    while( sLeftIdx < aLeftDigitCount )
    {
        if( aLeftIntArr[sLeftIdx++] != 0 )
        {
            sResult = 1;
            STL_THROW( RAMP_FINISH );
        }
    }

    while( sRightIdx < aRightDigitCount )
    {
        if( aRightIntArr[sRightIdx++] != 0 )
        {
            sResult = -1;
            STL_THROW( RAMP_FINISH );
        }
    }
    
    STL_RAMP( RAMP_FINISH );    
    
    return sResult;
}


/******************************************************************************
 * NUMERIC MOD 연산을 위한 내부 함수 ( 중간결과값 정리 )
 ******************************************************************************/

/**
 * @brief 중간 결과값 정리( leading / trailing zero 정리 )
 * @param[in,out] aIsPositive  aIntArr 부호
 * @param[in,out] aExponent    aIntArr Exponent
 * @param[in,out] aDigitCount  aIntArr Digit Count
 * @param[in,out] aIntArr      Int Arr 
 */
void dtfIntArrRemoveLeadingAndTrailingZeroForMod( stlBool             * aIsPositive,
                                                  stlInt32            * aExponent,
                                                  stlInt32            * aDigitCount,
                                                  stlInt8             * aIntArr )
{
    stlInt8  * sDigit = aIntArr;
    stlInt32   sGap;
    stlInt32   i;
    stlInt32   j;
    
    /*
     * leading zero 
     */
    while( ( *aDigitCount > 0 ) && ( *sDigit == 0 ) )
    {
        sDigit++;
        (*aExponent)--;
        (*aDigitCount)--;
    }
    
    /*
     * trailing zero
     */
    while( ( *aDigitCount > 0 ) && ( sDigit[*aDigitCount - 1] == 0 ) )
    {
        (*aDigitCount)--;
    }

    /* leading zero가 제거된 경우 자리 이동 */
    if( sDigit > aIntArr )
    {
        sGap = sDigit - aIntArr;
        for( i = 0, j = sGap; i < *aDigitCount; i++, j++ )
        {
            aIntArr[i] = aIntArr[j];
            aIntArr[j] = 0;
        }
    }
    
    if( *aDigitCount == 0 )
    {
        *aIsPositive = STL_TRUE;
        *aExponent = 0;
    }
}


/** @} */

