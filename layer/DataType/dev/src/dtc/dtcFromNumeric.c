/*******************************************************************************
 * dtcFromNumeric.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtcFromNumeric.c
 * @brief DataType Layer Numeric으로부터 다른 타입으로의 변환 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtcCast.h>
#include <dtfNumeric.h>

/**
 * @ingroup dtcFromNumeric
 * @{
 */

/**
 * @brief Numeric -> SmallInt 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    dtlDataValue    * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
//  dtlDataValue    * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
//  dtlDataValue    * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
//  dtlDataValue    * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
//  dtlDataValue    * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue    * sResultValue;
    dtlNumericType  * sNumeric;
    stlInt64          sResult;

    stlUInt8        * sDigitPtr;
    stlInt32          sDigitCount;
    stlInt32          sExponent;

    stlErrorStack   * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
//  sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE2( sSourceValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_SMALLINT, sErrorStack );


    sNumeric = DTF_NUMERIC( sSourceValue );

    sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );

    STL_TRY_THROW( sExponent < 3, ERROR_OUT_OF_RANGE );

    if( sExponent < 0 )
    {
        if( sExponent == -1 )
        {
            sResult   = 0;
            sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
            
            if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
            {
                /* 양수 */
                sResult = ( (DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
            }
            else
            {
                /* 음수 */
                sResult = ( (DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
                sResult = -sResult;
            }            
        }
        else
        {
            sResult = 0;
        }
        
        STL_THROW( RAMP_FINISH );
    }
    
    if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
    {
        /**
         * 양수
         */

        sResult = 0;
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sSourceValue->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

        if( (sDigitCount -1) > sExponent )
        {
            /**
             * Digit Count가 크기 때문에 Exponent + 1 만큼
             * Decoding하고 Exponent + 2의 Digit을 이용하여
             * 반올림을 판단한다.
             */

            while( sExponent >= 0 )
            {
                sResult *= 100;
                sResult += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr );
                sDigitPtr++;
                sExponent--;
            }

            sResult += ( (DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
        }
        else
        {
            /**
             * Exponent가 Digit Count와 같거나 크기 때문에
             * Digit Count만큼 Decoding한다.
             */

            sExponent -= (sDigitCount - 1);
            while( sDigitCount > 0 )
            {
                sResult *= 100;
                sResult += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr );
                sDigitPtr++;
                sDigitCount--;
            }

            sResult *= gPreDefinedPow[sExponent*2];
        }

        STL_TRY_THROW( sResult <= STL_INT16_MAX, ERROR_OUT_OF_RANGE );
    }
    else
    {
        /**
         * 음수
         */

        sResult = 0;
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sSourceValue->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

        if( (sDigitCount - 1) > sExponent )
        {
            /**
             * Digit Count가 크기 때문에 Exponent + 1 만큼
             * Decoding하고 Exponent + 2의 Digit을 이용하여
             * 반올림을 판단한다.
             */

            while( sExponent >= 0 )
            {
                sResult *= 100;
                sResult += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr );
                sDigitPtr++;
                sExponent--;
            }

            sResult += ( (DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
        }
        else
        {
            /**
             * Exponent가 Digit Count와 같거나 크기 때문에
             * Digit Count만큼 Decoding한다.
             */

            sExponent -= (sDigitCount - 1);
            while( sDigitCount > 0 )
            {
                sResult *= 100;
                sResult += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr );
                sDigitPtr++;
                sDigitCount--;
            }

            sResult *= gPreDefinedPow[sExponent*2];
        }

        sResult = -sResult;
        STL_TRY_THROW( sResult >= STL_INT16_MIN, ERROR_OUT_OF_RANGE );
    }

    STL_RAMP( RAMP_FINISH );

    *(dtlSmallIntType *)(sResultValue->mValue) = sResult;
    sResultValue->mLength = DTL_NATIVE_SMALLINT_SIZE;


    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Numeric -> Integer 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastNumericToInteger( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   void           * aInfo,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv )
{
    dtlDataValue    * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
//  dtlDataValue    * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
//  dtlDataValue    * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
//  dtlDataValue    * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
//  dtlDataValue    * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue    * sResultValue;
    dtlNumericType  * sNumeric;
    stlInt64          sResult;

    stlUInt8        * sDigitPtr;
    stlInt32          sDigitCount;
    stlInt32          sExponent;

    stlErrorStack   * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
//  sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE2( sSourceValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_INTEGER, sErrorStack );


    sNumeric = DTF_NUMERIC( sSourceValue );

    sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );

    STL_TRY_THROW( sExponent < 5, ERROR_OUT_OF_RANGE );

    if( sExponent < 0 )
    {
        if( sExponent == -1 )
        {
            sResult   = 0;
            sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
            
            if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
            {
                /* 양수 */
                sResult = ( (DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
            }
            else
            {
                /* 음수 */
                sResult = ( (DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
                sResult = -sResult;
            }            
        }
        else
        {
            sResult = 0;
        }
        
        STL_THROW( RAMP_FINISH );
    }

    if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
    {
        /**
         * 양수
         */

        sResult = 0;
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sSourceValue->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

        if( (sDigitCount - 1) > sExponent )
        {
            /**
             * Digit Count가 크기 때문에 Exponent + 1 만큼
             * Decoding하고 Exponent + 2의 Digit을 이용하여
             * 반올림을 판단한다.
             */

            while( sExponent >= 0 )
            {
                sResult *= 100;
                sResult += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr );
                sDigitPtr++;
                sExponent--;
            }

            sResult += ( (DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
        }
        else
        {
            /**
             * Exponent가 Digit Count와 같거나 크기 때문에
             * Digit Count만큼 Decoding한다.
             */

            sExponent -= (sDigitCount - 1);
            while( sDigitCount > 0 )
            {
                sResult *= 100;
                sResult += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr );
                sDigitPtr++;
                sDigitCount--;
            }

            sResult *= gPreDefinedPow[sExponent*2];
        }

        STL_TRY_THROW( sResult <= STL_INT32_MAX, ERROR_OUT_OF_RANGE );
    }
    else
    {
        /**
         * 음수
         */

        sResult = 0;
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sSourceValue->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

        if( (sDigitCount - 1) > sExponent )
        {
            /**
             * Digit Count가 크기 때문에 Exponent + 1 만큼
             * Decoding하고 Exponent + 2의 Digit을 이용하여
             * 반올림을 판단한다.
             */

            while( sExponent >= 0 )
            {
                sResult *= 100;
                sResult += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr );
                sDigitPtr++;
                sExponent--;
            }

            sResult += ( (DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
        }
        else
        {
            /**
             * Exponent가 Digit Count와 같거나 크기 때문에
             * Digit Count만큼 Decoding한다.
             */

            sExponent -= (sDigitCount - 1);
            while( sDigitCount > 0 )
            {
                sResult *= 100;
                sResult += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr );
                sDigitPtr++;
                sDigitCount--;
            }

            sResult *= gPreDefinedPow[sExponent*2];
        }

        sResult = -sResult;
        STL_TRY_THROW( sResult >= STL_INT32_MIN, ERROR_OUT_OF_RANGE );
    }

    STL_RAMP( RAMP_FINISH );
    
    *(dtlIntegerType *)(sResultValue->mValue) = sResult;
    sResultValue->mLength = DTL_NATIVE_INTEGER_SIZE;

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }    

    STL_FINISH;
        
    return STL_FAILURE;
}

/**
 * @brief Numeric -> BigInt 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    dtlDataValue    * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
//  dtlDataValue    * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
//  dtlDataValue    * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
//  dtlDataValue    * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
//  dtlDataValue    * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue    * sResultValue;
    dtlNumericType  * sNumeric;
    stlInt64          sResult;

    stlInt64          sMaxFence = STL_INT64_MAX / 100;
    stlInt64          sMinFence = STL_INT64_MIN / 100;

    stlUInt8        * sDigitPtr;
    stlInt32          sDigitCount;
    stlInt32          sExponent;

    stlErrorStack   * sErrorStack;
    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
//  sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE2( sSourceValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );


    sNumeric  = DTF_NUMERIC( sSourceValue );
    sResult   = 0;

    sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );

    if( sExponent < 0 )
    {
        if( sExponent == -1 )
        {
            sResult   = 0;
            sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
            
            if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
            {
                /* 양수 */
                sResult = ( (DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
            }
            else
            {
                /* 음수 */
                sResult = ( (DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
                sResult = -sResult;
            }            
        }
        else
        {
            sResult = 0;
        }
        
        STL_THROW( RAMP_FINISH );
    }
    else if( sExponent > 9 )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }


    if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
    {
        /**
         * 양수
         */

        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sSourceValue->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

        if( (sDigitCount - 1) < sExponent )
        {
            /**
             * Exponent가 Digit Count와 같거나 크기 때문에
             * Digit Count만큼 Decoding한다.
             */

            sExponent -= sDigitCount;
            while( sDigitCount > 0 )
            {
                sResult = sResult * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[0] );
                ++sDigitPtr;
                --sDigitCount;
            }
            
            sResult *= (stlInt64)gPreDefinedPow[sExponent*2];
            STL_TRY_THROW( sResult <= sMaxFence, ERROR_OUT_OF_RANGE );
            sResult *= 100;
        }
        else
        {
            sDigitCount -= (sExponent + 1);

            /* sExponent 개수만큼 decoding */
            while( sExponent > 0 )
            {
                sResult = sResult * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[0] );
                ++sDigitPtr;
                --sExponent;
            }

            /* out of range 체크 */
            if( sResult > sMaxFence )
            {
                STL_THROW( ERROR_OUT_OF_RANGE );
            }

            if( sDigitCount == 0 )
            {
                if( sResult == sMaxFence )
                {
                    STL_TRY_THROW(
                        sDigitPtr[0] < DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 8 ),
                        ERROR_OUT_OF_RANGE );
                }

                sResult =
                    sResult * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[0] );

            }
            else
            {
                if( sResult == sMaxFence )
                {
                    STL_TRY_THROW(
                        (sDigitPtr[0] < DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 7 )) ||
                        ( (sDigitPtr[0] == DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 7 )) &&
                          (sDigitPtr[1] < DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 50 )) ),
                        ERROR_OUT_OF_RANGE );
                }

                sResult =
                    sResult * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( sDigitPtr[0] ) +
                    ((sDigitPtr[1] > DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 49 )) ? 1 : 0);
            }
        }
    }
    else
    {
        /**
         * 음수
         */

        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sSourceValue->mLength );
        sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

        if( (sDigitCount - 1) < sExponent )
        {
            /**
             * Exponent가 Digit Count와 같거나 크기 때문에
             * Digit Count만큼 Decoding한다.
             */

            sExponent -= sDigitCount;
            while( sDigitCount > 0 )
            {
                sResult = sResult * 100 -
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[0] );
                ++sDigitPtr;
                --sDigitCount;
            }
            
            sResult *= (stlInt64)gPreDefinedPow[sExponent*2];
            STL_TRY_THROW( sResult >= sMinFence, ERROR_OUT_OF_RANGE );
            sResult *= 100;
        }
        else
        {
            sDigitCount -= (sExponent + 1);

            /* sExponent 개수만큼 decoding */
            while( sExponent > 0 )
            {
                sResult = sResult * 100 -
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[0] );
                ++sDigitPtr;
                --sExponent;
            }

            /* out of range 체크 */
            if( sResult < sMinFence )
            {
                STL_THROW( ERROR_OUT_OF_RANGE );
            }

            if( sDigitCount == 0 )
            {
                if( sResult == sMinFence )
                {
                    STL_TRY_THROW(
                        sDigitPtr[0] > DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 9 ),
                        ERROR_OUT_OF_RANGE );
                }

                sResult =
                    sResult * 100 -
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[0] );
            }
            else
            {
                if( sResult == sMinFence )
                {
                    STL_TRY_THROW(
                        (sDigitPtr[0] > DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 8 )) ||
                        ( (sDigitPtr[0] == DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 8 )) &&
                          (sDigitPtr[1] > DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 50 )) ),
                        ERROR_OUT_OF_RANGE );
                }

                sResult =
                    sResult * 100 -
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( sDigitPtr[0] ) -
                    ((sDigitPtr[1] < DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 49 )) ? 1 : 0);
            }
        }
    }

    
    STL_RAMP( RAMP_FINISH );
    
    *(dtlBigIntType *)(sResultValue->mValue) = sResult;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;
        
    return STL_FAILURE;
}

/**
 * @brief Numeric -> Real 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastNumericToReal( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue    * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    /* dtlDataValue    * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue */
    /* dtlDataValue    * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue */
    /* dtlDataValue    * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue */
    /* dtlDataValue    * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue */

    dtlDataValue    * sResultValue;
    
    stlErrorStack   * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    /* sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue; */
    /* sDestScaleValue              = aInputArgument[2].mValue.mDataValue; */
    /* sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue; */
    /* sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue; */

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE2( sSourceValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_REAL, sErrorStack );

    /* numeric to real */
    STL_TRY( dtdToFloat32FromNumeric( sSourceValue,
                                      & DTF_REAL( sResultValue ),
                                      sErrorStack )
             == STL_SUCCESS );

    sResultValue->mLength = DTL_NATIVE_REAL_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Numeric -> Double 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastNumericToDouble( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    dtlDataValue    * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    /* dtlDataValue    * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue */
    /* dtlDataValue    * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue */
    /* dtlDataValue    * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue */
    /* dtlDataValue    * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue */

    dtlDataValue    * sResultValue;
    
    stlErrorStack   * sErrorStack;
    
    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    /* sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue; */
    /* sDestScaleValue              = aInputArgument[2].mValue.mDataValue; */
    /* sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue; */
    /* sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue; */

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE2( sSourceValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );

    /* numeric to double */
    STL_TRY( dtdToFloat64FromNumeric( sSourceValue,
                                      & DTF_DOUBLE( sResultValue ),
                                      sErrorStack )
             == STL_SUCCESS );

    sResultValue->mLength = DTL_NATIVE_DOUBLE_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Numeric -> Numeric 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   void           * aInfo,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv )
{
    dtlDataValue   * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue   * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    dtlDataValue   * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
//  dtlDataValue   * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
//  dtlDataValue   * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue   * sResultValue;

    stlInt64         sDestPrecision;
    stlInt64         sDestScale;
    dtlNumericType * sSrcNumericValue;
    dtlNumericType * sResultNumericValue;

    stlBool          sIsOdd;
    stlBool          sIsLastDigit;
    stlUInt8         sCarry;
    stlInt32         sIsPositive;
    stlChar        * sDigitString;
    stlUInt8       * sDigit;
    stlUInt8       * sResultDigit;
    stlInt32         sDigitLen;
    stlInt32         sExponent;
    stlInt32         sDiffScale;
    stlErrorStack  * sErrorStack;
    
    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *) aResultValue;

    sErrorStack = (stlErrorStack *) aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE2( sSourceValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    sSrcNumericValue = DTF_NUMERIC( sSourceValue );
    sResultNumericValue = DTF_NUMERIC( sResultValue );

    if( DTL_NUMERIC_IS_ZERO( sSrcNumericValue, sSourceValue->mLength ) == STL_TRUE )
    {
        /* set numeric */
        DTL_NUMERIC_SET_ZERO( sResultNumericValue, sResultValue->mLength );
    }
    else
    {
        sDestPrecision = DTL_GET_NUMBER_DECIMAL_PRECISION( sResultValue->mType,
                                                           DTF_BIGINT( sDestPrecisionValue ) );
        sDestScale = *(stlInt64 *)( sDestScaleValue->mValue );

        sIsPositive = DTL_NUMERIC_IS_POSITIVE( sSrcNumericValue );
        sDigitLen   = DTL_NUMERIC_GET_DIGIT_COUNT( sSourceValue->mLength );
        sDigit      = DTL_NUMERIC_GET_DIGIT_PTR( sSrcNumericValue );
        
        sIsOdd      = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0';
        sExponent   = ( ( ( DTL_NUMERIC_GET_EXPONENT( sSrcNumericValue ) + 1 ) << 1 ) - sIsOdd );

        if( sDestScale == DTL_SCALE_NA )
        {
            /*
             * sDestScale이  scale n/a일 경우,
             * Digit가 Precision 으로 모두 표현이 된다면 memcpy가 가능하다.
             *
             *  ex ) sDestPrecision이 4 일 때,
             *      memcpy가 가능한 sDigitLen은 1234 또는 0123 sDigitLen = 2 이하.
             */
            if( ( sDigitLen * 2 ) <= sDestPrecision )
            {
                stlMemcpy( sResultNumericValue, sSrcNumericValue, sSourceValue->mLength );
                sResultValue->mLength = sSourceValue->mLength;
                STL_THROW( RAMP_EXIT );
            }
            else
            {
                sDiffScale = ((sDigitLen * 2 ) - sIsOdd ) - sExponent;
                
                sDestScale = ((sDigitLen * 2 ) - sIsOdd ) - sDestPrecision;
                sDestScale = sDiffScale - sDestScale;
            }
        }
        else
        {
            sDiffScale  = ( sDestPrecision - sDestScale ) - sExponent;
            
            STL_TRY_THROW( sDiffScale >= 0, ERROR_OUT_OF_RANGE );
        }
        
        if( sDigitLen == 1 )
        {
            sDigitLen = 2 - sIsOdd;
        }
        else
        {
            sDigitLen = ( ( sDigitLen << 1 ) - sIsOdd -
                          ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sSrcNumericValue->mData[ sDigitLen ] )[ 0 ] == '0' ) );
        }
        
        if( -sDestScale > sExponent )
        {
            /**
             * Set Zero : out of under bound
             */
            
            DTL_NUMERIC_SET_ZERO( sResultNumericValue, sResultValue->mLength );
        }
        else if( sDigitLen - sExponent <= sDestScale )
        {
            /**
             * Copy
             */

            stlMemcpy( sResultNumericValue, sSrcNumericValue, sSourceValue->mLength );
            sResultValue->mLength = sSourceValue->mLength;
        }
        else
        {
            /**
             * Round
             */

            sIsOdd       = ( sDestScale + sExponent + sIsOdd ) & 0x01;
            sDigitLen    = ( ( sDestScale + sExponent + sIsOdd - 1 ) >> 1 );
            sDigitLen++;
            sDigit       = & sSrcNumericValue->mData[ sDigitLen ];
            sResultDigit = & sResultNumericValue->mData[ sDigitLen ];
            
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );


            /**
             * Carry 설정
             */

            
            if( sIsOdd )
            {
                if( sDigitString[ 1 ] > '4' )
                {
                    sCarry = 0x3A - sDigitString[ 1 ];
                }
                else
                {
                    sCarry = -dtdHexLookup[ (stlInt32) sDigitString[ 1 ] ];
                }
            }
            else
            {
                sCarry = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *(sDigit + 1) )[ 0 ] > '4';
            }

            sIsLastDigit = STL_TRUE;
            if( sIsPositive )
            {
                /* positive value */
                while( sDigit > sSrcNumericValue->mData )
                {
                    *sResultDigit = *sDigit + sCarry;

                    if( DTD_NUMERIC_GET_POS_DIGIT_STRING( *sResultDigit )[ 0 ] == 'X' )
                    {
                        *sResultDigit = DTL_NUMERIC_DIGIT_POSITIVE_FENCE_VALUE;
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }

                    if( ( sIsLastDigit ) &&
                        ( *sResultDigit == DTL_NUMERIC_DIGIT_POSITIVE_FENCE_VALUE ) )
                    {
                        sDigitLen--;
                    }
                    else
                    {
                        sIsLastDigit = STL_FALSE;
                    }
                    
                    sResultDigit--;
                    sDigit--;
                }

                sExponent = DTL_NUMERIC_GET_EXPONENT( sSrcNumericValue );
                if( sCarry == 1 )
                {
                    if( sExponent == DTL_NUMERIC_MIN_EXPONENT )
                    {
                        /* set zero */
                        DTL_NUMERIC_SET_ZERO( sResultNumericValue, sResultValue->mLength );
                        STL_THROW( RAMP_EXIT );
                    }
                    else
                    {
                        /* exponent 1 증가하여 설정 */
                        STL_TRY_THROW( sExponent < DTL_NUMERIC_MAX_EXPONENT, ERROR_OUT_OF_RANGE );
                        sExponent++;
                        
                        /* 첫번째 digit을 1로 설정 */
                        sResultNumericValue->mData[1] = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
                        sDigitLen = 1;
                    }
                }
                else
                {
                    /* Do Nothing */
                }

                if( sDigitLen == 0 )
                {
                    DTL_NUMERIC_SET_ZERO( sResultNumericValue, sResultValue->mLength );
                }
                else
                {
                    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumericValue, sExponent );
                    sResultValue->mLength = sDigitLen + 1;
                }
            }
            else
            {
                /* negative value */
                while( sDigit > sSrcNumericValue->mData )
                {
                    *sResultDigit = *sDigit - sCarry;

                    if( DTD_NUMERIC_GET_NEG_DIGIT_STRING( *sResultDigit )[ 0 ] == 'X' )
                    {
                        *sResultDigit = DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE;
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }
                    
                    if( ( sIsLastDigit ) &&
                        ( *sResultDigit == DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE ) )
                    {
                        sDigitLen--;
                    }
                    else
                    {
                        sIsLastDigit = STL_FALSE;
                    }

                    sResultDigit--;
                    sDigit--;
                }

                sExponent = DTL_NUMERIC_GET_EXPONENT( sSrcNumericValue );
                if( sCarry == 1 )
                {
                    if( sExponent == DTL_NUMERIC_MIN_EXPONENT )
                    {
                        /* set zero */
                        DTL_NUMERIC_SET_ZERO( sResultNumericValue, sResultValue->mLength );
                        STL_THROW( RAMP_EXIT );
                    }
                    else
                    {
                        /* exponent 1 증가하여 설정 */
                        STL_TRY_THROW( sExponent < DTL_NUMERIC_MAX_EXPONENT, ERROR_OUT_OF_RANGE );
                        sExponent++;
                        
                        /* 첫번째 digit을 1로 설정 */
                        sResultNumericValue->mData[1] = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 1 );
                        sDigitLen = 1;
                    }
                }
                else
                {
                    /* Do Nothing */
                }

                if( sDigitLen == 0 )
                {
                    DTL_NUMERIC_SET_ZERO( sResultNumericValue, sResultValue->mLength );
                }
                else
                {
                    DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sResultNumericValue, sExponent );
                    sResultValue->mLength = sDigitLen + 1;
                }
            }
        }
    }

    STL_RAMP( RAMP_EXIT );

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Numeric -> Char 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastNumericToChar( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    /* dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue */
    /* dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue */
    /* dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue */

    stlInt64        sDestPrecision;

    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    /* sDestScaleValue              = aInputArgument[2].mValue.mDataValue; */
    /* sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue; */
    /* sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue; */

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE2( sSourceValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_CHAR, sErrorStack );

    sDestPrecision = *(stlInt64 *)(sDestPrecisionValue->mValue);
    STL_TRY( dtdCastNumericToNonTruncateString( sSourceValue,
                                                *(stlInt64 *)(sDestPrecisionValue->mValue),
                                                sResultValue->mValue,
                                                & sResultValue->mLength,
                                                sErrorStack )
             == STL_SUCCESS );

    if( sDestPrecision > sResultValue->mLength )
    {
        stlMemset( & ((stlChar*)sResultValue->mValue)[ sResultValue->mLength ],
                   ' ',
                   sDestPrecision - sResultValue->mLength );
    }
    else
    {
        // Do Nothing
    }

    sResultValue->mLength = *(stlInt64 *)(sDestPrecisionValue->mValue);
    

    return STL_SUCCESS;

    STL_FINISH;    

    return STL_FAILURE;    
}

/**
 * @brief Numeric -> Varchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastNumericToVarchar( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   void           * aInfo,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    /* dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue */
    /* dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue */
    /* dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue */

    dtlDataValue  * sResultValue;
    
    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    /* sDestScaleValue              = aInputArgument[2].mValue.mDataValue; */
    /* sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue; */
    /* sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue; */

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE2( sSourceValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    STL_TRY( dtdCastNumericToString( sSourceValue,
                                     *(stlInt64 *)(sDestPrecisionValue->mValue),
                                     sResultValue->mValue,
                                     & sResultValue->mLength,
                                     sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;    

    return STL_FAILURE;    
}

/**
 * @brief Numeric -> Longvarchar 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastNumericToLongvarchar( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
    /* dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue */
    /* dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue */
    /* dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue */

    dtlDataValue  * sResultValue;

    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    /* sDestScaleValue              = aInputArgument[2].mValue.mDataValue; */
    /* sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue; */
    /* sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue; */

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE2( sSourceValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );

    STL_TRY( dtdCastNumericToString( sSourceValue,
                                     *(stlInt64 *)(sDestPrecisionValue->mValue),
                                     sResultValue->mValue,
                                     & sResultValue->mLength,
                                     sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;    

    return STL_FAILURE;    
}


/**
 * @brief Numeric -> Interval Year To Month 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastNumericToIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue    * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue    * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue]
    dtlDataValue    * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
    dtlDataValue    * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue    * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue    * sResultValue;

    stlBool           sSuccessWithInfo = STL_FALSE;

    stlErrorStack   * sErrorStack;

    dtlNumericType  * sNumeric;
    stlInt64          sInteger = 0;
    
    stlUInt8        * sDigitPtr;    
    stlInt32          sDigitCount;
    stlInt32          sExponent;   
   
    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sSourceValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_YEAR_TO_MONTH, sErrorStack );
    
    sNumeric  = DTF_NUMERIC( sSourceValue );
    sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );

    /*
     * DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION (6)을 넘는지 체크.
     */   
    STL_TRY_THROW( sExponent < 3, ERROR_OUT_OF_PRECISION );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric, sSourceValue->mLength  ) )
    {
        sInteger = 0;
    }
    else
    {    
        /*
         * truncate가 발생하는지 체크.
         * 변환하고자 하는 Numeric 은 소수점이 없는 값이어야 한다.
         */    
        sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sSourceValue->mLength );
        
        STL_TRY_THROW((sDigitCount - 1) <= sExponent, RAMP_ERROR_BAD_FORMAT);
        
        /*
         * 소수점 없는 Numeric 을 Integer 로 변환
         */

        sDigitPtr   = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
        sExponent  -= (sDigitCount - 1);    

        if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
        {
            /*
             * 양수
             */

            while( sDigitCount > 0 )
            {
                sInteger *= 100;
                sInteger += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr );
                sDigitPtr++;
                sDigitCount--;
            }
        
            sInteger *= gPreDefinedPow[sExponent*2];        
        }
        else
        {
            /*
             * 음수
             */

            while( sDigitCount > 0 )
            {
                sInteger *= 100;
                sInteger += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr );
                sDigitPtr++;
                sDigitCount--;
            }
        
            sInteger *= gPreDefinedPow[sExponent*2];
            sInteger = -sInteger;        
        }
    }

    /*
     * interval type으로 cast
     */
    STL_TRY( dtdIntervalYearToMonthSetValueFromInteger(
                 sInteger,
                 *(stlInt64 *)sDestPrecisionValue->mValue,
                 *(stlInt64 *)sDestScaleValue->mValue,
                 *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                 *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                 dtlDataTypeMaxBufferSize[sResultValue->mType], //메모리할당은 충분히 받았다고 가정.
                 sResultValue,
                 & sSuccessWithInfo,
                 aVectorFunc,
                 aVectorArg,
                 sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH(RAMP_ERROR_BAD_FORMAT)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL,
                      NULL,
                      sErrorStack );
    };
        
    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief Numeric -> Interval Day To Second 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastNumericToIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    dtlDataValue    * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue    * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue]
    dtlDataValue    * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
    dtlDataValue    * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
    dtlDataValue    * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue    * sResultValue;

    stlBool           sSuccessWithInfo = STL_FALSE;

    stlErrorStack   * sErrorStack;

    dtlNumericType  * sNumeric;    
    stlInt64          sValue = 0;

    stlUInt8        * sDigitPtr;    
    stlInt32          sDigitCount;
    stlInt32          sExponent;
    stlInt32          sCount;
    
    dtlIntervalIndicator        sIntervalIndicator;
    dtlIntervalDayToSecondType  sIntervalDayToSecondType; 
    
    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
    sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
    sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
    sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE2( sSourceValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_INTERVAL_DAY_TO_SECOND, sErrorStack );

    sIntervalIndicator = *(dtlIntervalIndicator*)(sDestIntervalIndicatorValue->mValue);

    sIntervalDayToSecondType.mIndicator = DTL_INTERVAL_INDICATOR_NA;
    sIntervalDayToSecondType.mDay  = 0;
    sIntervalDayToSecondType.mTime = 0;
    
    sNumeric  = DTF_NUMERIC( sSourceValue );
    sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );
    
    /*
     * DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION (6)을 넘는지 체크.
     */   
    STL_TRY_THROW( sExponent < 3, ERROR_OUT_OF_PRECISION );
    
    if( (sIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY)  ||
        (sIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
        (sIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE ) )
    {
        /*
         * DTL_INTERVAL_INDICATOR_DAY,
         * DTL_INTERVAL_INDICATOR_HOUR,
         * DTL_INTERVAL_INDICATOR_MINUTE
         */

        if( DTL_NUMERIC_IS_ZERO( sNumeric, sSourceValue->mLength ) )
        {
            sValue = 0;
        }
        else
        {
            /*
             * truncate가 발생하는지 체크.
             * 변환하고자 하는 Numeric 은 소수점이 없는 값이어야 한다.
             */    
            sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sSourceValue->mLength );
        
            STL_TRY_THROW((sDigitCount - 1) <= sExponent, RAMP_ERROR_BAD_FORMAT);
        
            /*
             * 소수점 없는 Numeric 을 Integer 로 변환
             */

            sDigitPtr   = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
            sExponent  -= (sDigitCount - 1);    

            if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
            {
                /*
                 * 양수
                 */

                while( sDigitCount > 0 )
                {
                    sValue *= 100;
                    sValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr );
                    sDigitPtr++;
                    sDigitCount--;
                }
        
                sValue *= gPreDefinedPow[sExponent*2];        
            }
            else
            {
                /*
                 * 음수
                 */

                while( sDigitCount > 0 )
                {
                    sValue *= 100;
                    sValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr );
                    sDigitPtr++;
                    sDigitCount--;
                }
        
                sValue *= gPreDefinedPow[sExponent*2];
                sValue = -sValue;        
            }
        }                 
        
        /*
         * interval type으로 cast
         */
        STL_TRY( dtdIntervalDayToSecondSetValueFromInteger(
                     sValue,
                     *(stlInt64 *)sDestPrecisionValue->mValue,
                     *(stlInt64 *)sDestScaleValue->mValue,
                     *(dtlStringLengthUnit*)sDestStringLengthUnitValue->mValue,
                     *(dtlIntervalIndicator*)sDestIntervalIndicatorValue->mValue,
                     dtlDataTypeMaxBufferSize[sResultValue->mType], //메모리할당은 충분히 받았다고 가정.
                     sResultValue,
                     & sSuccessWithInfo,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /*
         * DTL_INTERVAL_INDICATOR_SECOND
         */
        
        if( DTL_NUMERIC_IS_ZERO( sNumeric, sSourceValue->mLength ) )
        {
            sValue = 0;
        }
        else
        {
            if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
            {
                /**
                 * 양수
                 */
                
                sValue = 0;
                sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sSourceValue->mLength );
                sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
                
                /**
                 * Digit Count가 크기 때문에 Exponent + 1 만큼
                 * Decoding하고 Exponent + 2의 Digit을 이용하여
                 * 반올림을 판단한다.
                 */
                    
                if( ( (sDigitCount - 1) - sExponent ) > 3 )
                {
                    /* 소수점자리가 7이상인 경우, scale 6에 맞추어 반올림. */
                    
                    sCount = sExponent + 3;                    
                    
                    while( sCount >= 0 )
                    {
                        sValue *= 100;
                        sValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr );
                        sDigitPtr++;
                        sCount--;
                    }
                        
                    sValue += ( (DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
                }
                else
                {
                    sCount = sDigitCount;
                        
                    while( sCount > 0 )
                    {
                        sValue *= 100;
                        sValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr );
                        sDigitPtr++;
                        sCount--;
                    }

                    sValue *= (stlInt64)gPreDefinedPow[ DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION -
                                                        ((sDigitCount - 1 - sExponent) * 2) ];
                }
            }
            else
            {
                /**
                 * 음수
                 */

                sValue = 0;
                sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( sSourceValue->mLength );
                sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

                if( ( (sDigitCount - 1) - sExponent ) > 3 )
                {
                    /* 소수점자리가 7이상인 경우, scale 6에 맞추어 반올림. */
                        
                    sCount = sExponent + 3;

                    while( sCount >= 0 )
                    {
                        sValue *= 100;
                        sValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr );
                        sDigitPtr++;
                        sCount--;
                    }
                        
                    sValue += ( (DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr ) + 50) / 100 );
                }
                else
                {
                    sCount = sDigitCount;
                        
                    while( sCount > 0 )
                    {
                        sValue *= 100;
                        sValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr );
                        sDigitPtr++;
                        sCount--;
                    }
                    
                    sValue *= (stlInt64)gPreDefinedPow[ DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION -
                                                        ((sDigitCount - 1 - sExponent) * 2) ];                    
                }
                
                sValue = -sValue;
            }
        }

        sIntervalDayToSecondType.mIndicator = sIntervalIndicator;
        sIntervalDayToSecondType.mDay  = sValue / DTL_USECS_PER_DAY; 
        sIntervalDayToSecondType.mTime = sValue % DTL_USECS_PER_DAY; 
	
        STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecondType, 
                                               sIntervalIndicator, 
                                               *(stlInt64 *)sDestPrecisionValue->mValue, 
                                               *(stlInt64 *)sDestScaleValue->mValue, 
                                               sErrorStack )
                 == STL_SUCCESS );
        
        /*   
         * result value 값 설정. 
         */         
        ((dtlIntervalDayToSecondType *)(sResultValue->mValue))->mIndicator 
            = sIntervalDayToSecondType.mIndicator; 
        ((dtlIntervalDayToSecondType *)(sResultValue->mValue))->mDay = sIntervalDayToSecondType.mDay; 
        ((dtlIntervalDayToSecondType *)(sResultValue->mValue))->mTime = sIntervalDayToSecondType.mTime; 
        
        sResultValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE; 
    }
    
    return STL_SUCCESS;
    
    STL_CATCH(RAMP_ERROR_BAD_FORMAT)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL,
                      NULL,
                      sErrorStack );
    };
        
    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;    
}

/** @} */
