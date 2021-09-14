/*******************************************************************************
 * dtfBucket.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfBucket.c 1296 2011-06-30 07:24:30Z ehpark $
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
 * @file dtfBucket.c
 * @brief dtfBucket Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfNumeric.h>
#include <dtfMathematical.h>



/**
 * @addtogroup dtfBucket Bucket
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * width_bucket( operand, bound1, bound2, count )
 *   width_bucket( operand, bound1, bound2, count ) => [ P, S, O ]
 * ex) width_bucket(5.35, 0.024, 10.06, 5)  =>  3
 ******************************************************************************/

/**
 * @brief width_bucket(operand, bound1, bound2, count) 함수
 *        <BR> 동일한 넓이를 갖는 히스토그램을 생성
 *        <BR> width_bucket( operand, bound1, bound2, count )
 *        <BR>   width_bucket( operand, bound1, bound2, count ) => [ P, S, O ]
 *        <BR> ex) width_bucket(5.35, 0.024, 10.06, 5)  =>  3
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> width_bucket(BIGINT, BIGINT, BIGINT, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 INTEGER)
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntWidthBucket( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue  * sOperand;
    dtlDataValue  * sBound1;
    dtlDataValue  * sBound2;
    dtlDataValue  * sCountValue;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    dtlNumericType  * sNumeric;
    stlInt64          sCount;
    stlInt32          sLoop;
    stlInt32          sExpo;
    stlUInt8        * sCountDigit;

    stlInt32          sArgNum = 0;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 4, (stlErrorStack *)aEnv );

    sOperand = aInputArgument[0].mValue.mDataValue;
    sBound1 = aInputArgument[1].mValue.mDataValue;
    sBound2 = aInputArgument[2].mValue.mDataValue;
    sCountValue = aInputArgument[3].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sOperand, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    DTF_CHECK_DTL_VALUE( sBound1, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    DTF_CHECK_DTL_VALUE( sBound2, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sCountValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_INTEGER, sErrorStack );


    sNumeric = DTF_NUMERIC( sCountValue );

    /*
     * count는 항상 양의 정수이어야 한다.
     */
    
    sArgNum = 4;
    STL_TRY_THROW( ( DTL_NUMERIC_IS_POSITIVE( sNumeric ) ) &&
                   ( DTL_NUMERIC_IS_ZERO( sNumeric, sCountValue->mLength  ) == STL_FALSE ),
                   ERROR_ARGUMENT_OUT_OF_RANGE );
    
    sExpo = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric );
    STL_TRY_THROW( sExpo * 2 < DTL_DECIMAL_INTEGER_DEFAULT_PRECISION,
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    sLoop = DTL_NUMERIC_GET_DIGIT_COUNT( sCountValue->mLength );
    sExpo = ( sExpo - sLoop + 1 );
    STL_TRY_THROW( sExpo >= 0,
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    sCountDigit = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
    sCount = 0;
    while( sLoop )
    {
        sCount *= 100;
        sCount += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sCountDigit );
        ++sCountDigit;
        --sLoop;
    }
    sCount *= gPredefined10PowValue[ sExpo * 2 ];

    STL_TRY_THROW( sCount <=  DTL_NATIVE_INTEGER_MAX,
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    sArgNum = 3;
    STL_TRY_THROW( DTF_BIGINT( sBound1 ) != DTF_BIGINT( sBound2),
                   ERROR_ARGUMENT_OUT_OF_RANGE );
    
    if( DTF_BIGINT( sBound1 ) < DTF_BIGINT( sBound2 ) )
    {
        if( DTF_BIGINT( sOperand ) < DTF_BIGINT( sBound1 ) )
        {
            DTF_INTEGER( sResultValue ) = 0;
        }
        else if( DTF_BIGINT( sOperand ) >= DTF_BIGINT( sBound2 ) )
        {
            DTF_INTEGER( sResultValue ) = sCount + 1;
        }
        else
        {
            DTF_INTEGER( sResultValue ) =
                ( ( sCount *
                    ( DTF_BIGINT( sOperand ) - DTF_BIGINT( sBound1 ) ) /
                    ( DTF_BIGINT( sBound2 ) - DTF_BIGINT( sBound1 ) )
                    ) + 1 );
        }
    }
    else
    {
        /* if( DTF_BIGINT( sBound1 ) > DTF_BIGINT( sBound2 ) ) */
        if( DTF_BIGINT( sOperand ) > DTF_BIGINT( sBound1 ) )
        {
            DTF_INTEGER( sResultValue ) = 0;
        }
        else if( DTF_BIGINT( sOperand ) <= DTF_BIGINT( sBound2 ) )
        {
            DTF_INTEGER( sResultValue ) = sCount + 1;
        }
        else
        {
            DTF_INTEGER( sResultValue ) =
                ( ( sCount *
                    ( DTF_BIGINT( sBound1 ) - DTF_BIGINT( sOperand ) ) /
                    ( DTF_BIGINT( sBound1 ) - DTF_BIGINT( sBound2 ) )
                    ) + 1 );
        }
    }

    STL_TRY_THROW( DTF_INTEGER( sResultValue ) >= 0, ERROR_OUT_OF_RANGE );
    
    sResultValue->mLength = DTL_NATIVE_INTEGER_SIZE;


    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      sArgNum,
                      "WIDTH_BUCKET" );
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
 * @brief width_bucket(operand, bound1, bound2, count) 함수
 *        <BR> 동일한 넓이를 갖는 히스토그램을 생성
 *        <BR> width_bucket( operand, bound1, bound2, count )
 *        <BR>   width_bucket( operand, bound1, bound2, count ) => [ P, S, O ]
 *        <BR> ex) width_bucket(5.35, 0.024, 10.06, 5)  =>  3
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> width_bucket(NUMERIC, NUMERIC, NUMERIC, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 INTEGER)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericWidthBucket( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    dtlDataValue     * sNumeric1;
    dtlDataValue     * sNumeric2;
    dtlDataValue     * sNumeric3;
    dtlDataValue     * sNumeric4;
    dtlDataValue     * sResultValue;
    stlErrorStack    * sErrorStack;

    dtlNumericType   * sOperand;
    dtlNumericType   * sBound1;
    dtlNumericType   * sBound2;
    dtlNumericType   * sCount;
    
    dtlDataValue       sTemp1;
    dtlDataValue       sTemp2;

    stlChar            sTempBuf1[ DTL_NUMERIC_MAX_SIZE ];
    stlChar            sTempBuf2[ DTL_NUMERIC_MAX_SIZE ];
    
    stlInt8          * sTable;
    stlUInt8         * sOperandDigit;
    stlUInt8         * sBoundDigit1;
    stlUInt8         * sBoundDigit2;
    stlUInt8         * sCountDigit;

    stlBool            sIsTruncate;
    stlInt64           sCountValue;
    stlInt32           sLoop;
    stlInt32           sExpo;
    stlInt32           sArgNum = 0;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 4, (stlErrorStack *)aEnv );

    sNumeric1 = aInputArgument[0].mValue.mDataValue;
    sNumeric2 = aInputArgument[1].mValue.mDataValue;
    sNumeric3 = aInputArgument[2].mValue.mDataValue;
    sNumeric4 = aInputArgument[3].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sNumeric1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sNumeric2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sNumeric3, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sNumeric4, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    sOperand = DTF_NUMERIC( sNumeric1 );
    sBound1  = DTF_NUMERIC( sNumeric2 );
    sBound2  = DTF_NUMERIC( sNumeric3 );
    sCount   = DTF_NUMERIC( sNumeric4 );


    /*
     * count는 항상 양의 정수이어야 한다.
     */

    sArgNum = 4;
    STL_TRY_THROW( ( DTL_NUMERIC_IS_POSITIVE( sCount ) ) &&
                   ( DTL_NUMERIC_IS_ZERO( sCount, sNumeric4->mLength  ) == STL_FALSE ),
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    sExpo = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sCount );
    STL_TRY_THROW( sExpo * 2 < DTL_DECIMAL_INTEGER_DEFAULT_PRECISION,
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    sLoop = DTL_NUMERIC_GET_DIGIT_COUNT( sNumeric4->mLength );
    sExpo = ( sExpo - sLoop + 1 );
    STL_TRY_THROW( sExpo >= 0,
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    sCountDigit = DTL_NUMERIC_GET_DIGIT_PTR( sCount );
    sCountValue = 0;
    while( sLoop )
    {
        sCountValue *= 100;
        sCountValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sCountDigit );
        ++sCountDigit;
        --sLoop;
    }
    sCountValue *= gPredefined10PowValue[ sExpo * 2 ];

    STL_TRY_THROW( sCountValue <=  DTL_NATIVE_INTEGER_MAX,
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    /* bucket 구하기 */
    sBoundDigit1 = sBound1->mData;
    sBoundDigit2 = sBound2->mData;
    
    sTable = dtlNumericLenCompTable[ sNumeric2->mLength ][ sNumeric3->mLength ];    
    sLoop  = sTable[1];

    while( sLoop )
    {
        if( *sBoundDigit1 != *sBoundDigit2 )
        {
            break;
        }
        --sLoop;
        ++sBoundDigit1;
        ++sBoundDigit2;
    }

    sArgNum = 3;
    STL_TRY_THROW( ( *sBoundDigit1 != *sBoundDigit2 ) ||
                   ( ( *sBoundDigit1 == *sBoundDigit2 ) && ( sTable[0] != 0 ) ),
                   ERROR_ARGUMENT_OUT_OF_RANGE );
    
    if( ( *sBoundDigit1 < *sBoundDigit2 ) ||
        ( ( *sBoundDigit1 == *sBoundDigit2 ) &&
          ( DTL_NUMERIC_IS_POSITIVE( sBound1 ) ? sTable[0] < 0 : sTable[0] > 0 ) ) )
    {
        sOperandDigit = sOperand->mData;
        sBoundDigit1  = sBound1->mData;

        sTable = dtlNumericLenCompTable[ sNumeric1->mLength ][ sNumeric2->mLength ];
        sLoop  = sTable[1];

        while( sLoop )
        {
            if( *sOperandDigit != *sBoundDigit1 )
            {
                break;
            }
            --sLoop;
            ++sOperandDigit;
            ++sBoundDigit1;
        }
        
        if( ( *sOperandDigit < *sBoundDigit1 ) ||
            ( ( *sOperandDigit == *sBoundDigit1 ) &&
              ( DTL_NUMERIC_IS_POSITIVE( sOperand ) ? sTable[0] < 0 : sTable[0] > 0 ) ) )
        {
            DTF_INTEGER( sResultValue ) = 0;
        }
        else
        {
            sOperandDigit = sOperand->mData;
            sBoundDigit2  = sBound2->mData;

            sTable = dtlNumericLenCompTable[ sNumeric1->mLength ][ sNumeric3->mLength ];
            sLoop  = sTable[1];

            while( sLoop )
            {
                if( *sOperandDigit != *sBoundDigit2 )
                {
                    break;
                }
                --sLoop;
                ++sOperandDigit;
                ++sBoundDigit2;
            }

            if( ( *sOperandDigit > *sBoundDigit2 ) ||
                ( ( *sOperandDigit == *sBoundDigit2 ) &&
                  ( DTL_NUMERIC_IS_POSITIVE( sOperand ) ? sTable[0] > 0 : sTable[0] < 0 ) ) )
            {
                STL_TRY_THROW( sCountValue < DTL_NATIVE_INTEGER_MAX,
                               ERROR_OUT_OF_RANGE );
                
                DTF_INTEGER( sResultValue ) = sCountValue + 1;
            }
            else
            {
                sTemp1.mType   = DTL_TYPE_NUMBER;
                sTemp1.mLength = 0;
                sTemp1.mBufferSize = DTL_NUMERIC_MAX_SIZE;
                sTemp1.mValue  = (dtlNumericType*) & sTempBuf1;

                sTemp2.mType   = DTL_TYPE_NUMBER;
                sTemp2.mLength = 0;
                sTemp2.mBufferSize = DTL_NUMERIC_MAX_SIZE;
                sTemp2.mValue  = (dtlNumericType*) & sTempBuf2;

                STL_TRY( dtfNumericSubtraction( sNumeric1,
                                                sNumeric2,
                                                & sTemp1,
                                                sErrorStack )
                         == STL_SUCCESS );

                STL_TRY( dtfNumericSubtraction( sNumeric3,
                                                sNumeric2,
                                                & sTemp2,
                                                sErrorStack )
                         == STL_SUCCESS );
            
                STL_TRY( dtfNumericDivision( & sTemp1,
                                             & sTemp2,
                                             & sTemp1,
                                             sErrorStack )
                         == STL_SUCCESS );
            
                STL_TRY( dtfNumericMultiplication( & sTemp1,
                                                   sNumeric4,
                                                   & sTemp1,
                                                   sErrorStack )
                         == STL_SUCCESS );

                STL_TRY( dtlNumericToInt64( & sTemp1,
                                            & sCountValue,
                                            & sIsTruncate,
                                            sErrorStack )
                         == STL_SUCCESS );
                
                DTF_INTEGER( sResultValue ) = sCountValue + 1;
            }
        }
    }
    else
    {
        sOperandDigit = sOperand->mData;
        sBoundDigit1  = sBound1->mData;

        sTable = dtlNumericLenCompTable[ sNumeric1->mLength ][ sNumeric2->mLength ];
        sLoop  = sTable[1];

        while( sLoop )
        {
            if( *sOperandDigit != *sBoundDigit1 )
            {
                break;
            }
            --sLoop;
            ++sOperandDigit;
            ++sBoundDigit1;
        }
        
        if( ( *sOperandDigit > *sBoundDigit1 ) ||
            ( ( *sOperandDigit == *sBoundDigit1 ) &&
              ( DTL_NUMERIC_IS_POSITIVE( sOperand ) ? sTable[0] > 0 : sTable[0] < 0 ) ) )
        {
            DTF_INTEGER( sResultValue ) = 0;
        }
        else
        {
            sOperandDigit = sOperand->mData;
            sBoundDigit2  = sBound2->mData;

            sTable = dtlNumericLenCompTable[ sNumeric1->mLength ][ sNumeric3->mLength ];
            sLoop  = sTable[1];

            while( sLoop )
            {
                if( *sOperandDigit != *sBoundDigit2 )
                {
                    break;
                }
                --sLoop;
                ++sOperandDigit;
                ++sBoundDigit2;
            }
            
            if( ( *sOperandDigit > *sBoundDigit2 ) ||
                ( ( *sOperandDigit == *sBoundDigit2 ) && 
                  ( DTL_NUMERIC_IS_POSITIVE( sOperand ) ? sTable[0] > 0 : sTable[0] < 0 ) ) )
            {
                sTemp1.mType   = DTL_TYPE_NUMBER;
                sTemp1.mLength = 0;
                sTemp1.mValue  = (dtlNumericType*) & sTempBuf1;

                sTemp2.mType   = DTL_TYPE_NUMBER;
                sTemp2.mLength = 0;
                sTemp2.mValue  = (dtlNumericType*) & sTempBuf2;

                STL_TRY( dtfNumericSubtraction( sNumeric2,
                                                sNumeric1,
                                                & sTemp1,
                                                sErrorStack )
                         == STL_SUCCESS );

                STL_TRY( dtfNumericSubtraction( sNumeric2,
                                                sNumeric3,
                                                & sTemp2,
                                                sErrorStack )
                         == STL_SUCCESS );
            
                STL_TRY( dtfNumericDivision( & sTemp1,
                                             & sTemp2,
                                             & sTemp1,
                                             sErrorStack )
                         == STL_SUCCESS );
            
                STL_TRY( dtfNumericMultiplication( & sTemp1,
                                                   sNumeric4,
                                                   & sTemp1,
                                                   sErrorStack )
                         == STL_SUCCESS );

                STL_TRY( dtlNumericToInt64( & sTemp1,
                                            & sCountValue,
                                            & sIsTruncate,
                                            sErrorStack )
                         == STL_SUCCESS );
                
                DTF_INTEGER( sResultValue ) = sCountValue + 1;
            }
            else
            {
                STL_TRY_THROW( sCountValue < DTL_NATIVE_INTEGER_MAX,
                               ERROR_OUT_OF_RANGE );
                
                DTF_INTEGER( sResultValue ) = sCountValue + 1;
            }
        }
    }

    STL_TRY_THROW( DTF_INTEGER( sResultValue ) >= 0, ERROR_OUT_OF_RANGE );

    sResultValue->mLength = DTL_NATIVE_INTEGER_SIZE;


    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      sArgNum,
                      "WIDTH_BUCKET" );        
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
 * @brief width_bucket(operand, bound1, bound2, count) 함수
 *        <BR> 동일한 넓이를 갖는 히스토그램을 생성
 *        <BR> width_bucket( operand, bound1, bound2, count )
 *        <BR>   width_bucket( operand, bound1, bound2, count ) => [ P, S, O ]
 *        <BR> ex) width_bucket(5.35, 0.024, 10.06, 5)  =>  3
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> width_bucket(DOUBLE, DOUBLE, DOUBLE, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 INTEGER)
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDoubleWidthBucket( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue  * sOperand;
    dtlDataValue  * sBound1;
    dtlDataValue  * sBound2;
    dtlDataValue  * sCountValue;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    dtlNumericType  * sNumeric;
    stlInt64          sCount;
    stlInt32          sLoop;
    stlInt32          sExpo;
    stlUInt8        * sCountDigit;
    stlInt32          sArgNum = 0;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 4, (stlErrorStack *)aEnv );

    sOperand = aInputArgument[0].mValue.mDataValue;
    sBound1 = aInputArgument[1].mValue.mDataValue;
    sBound2 = aInputArgument[2].mValue.mDataValue;
    sCountValue = aInputArgument[3].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sOperand, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sBound1, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );
    DTF_CHECK_DTL_VALUE( sBound2, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );
    DTF_CHECK_DTL_VALUE2( sCountValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    /* Double의 경우는 input이 finite인지 확인한다. */
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sOperand ) )
                 == STL_IS_INFINITE_FALSE );
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sBound1 ) )
                 == STL_IS_INFINITE_FALSE );
    STL_DASSERT( stlIsinfinite( DTF_DOUBLE( sBound2 ) )
                 == STL_IS_INFINITE_FALSE );


    sNumeric = DTF_NUMERIC( sCountValue );

    /*
     * count는 항상 양의 정수이어야 한다.
     */

    sArgNum = 4;
    STL_TRY_THROW( ( DTL_NUMERIC_IS_POSITIVE( sNumeric ) ) &&
                   ( DTL_NUMERIC_IS_ZERO( sNumeric, sCountValue->mLength ) == STL_FALSE ),
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    sExpo = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric );
    STL_TRY_THROW( sExpo * 2 < DTL_DECIMAL_INTEGER_DEFAULT_PRECISION,
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    sLoop = DTL_NUMERIC_GET_DIGIT_COUNT( sCountValue->mLength );
    sExpo = ( sExpo - sLoop + 1 );
    STL_TRY_THROW( sExpo >= 0,
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    sCountDigit = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
    sCount = 0;
    while( sLoop )
    {
        sCount *= 100;
        sCount += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sCountDigit );
        ++sCountDigit;
        --sLoop;
    }
    sCount *= gPredefined10PowValue[ sExpo * 2 ];

    STL_TRY_THROW( sCount <=  DTL_NATIVE_INTEGER_MAX,
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    sArgNum = 3;
    STL_TRY_THROW( DTF_DOUBLE( sBound1 ) != DTF_DOUBLE( sBound2),
                   ERROR_ARGUMENT_OUT_OF_RANGE );
    
    if( DTF_DOUBLE( sBound1 ) < DTF_DOUBLE( sBound2 ) )
    {
        if( DTF_DOUBLE( sOperand ) < DTF_DOUBLE( sBound1 ) )
        {
            DTF_INTEGER( sResultValue ) = 0;
        }
        else if( DTF_DOUBLE( sOperand ) >= DTF_DOUBLE( sBound2 ) )
        {
            DTF_INTEGER( sResultValue ) = sCount + 1;
        }
        else
        {
            DTF_INTEGER( sResultValue ) =
                ( (stlFloat64)sCount *
                  ( DTF_DOUBLE( sOperand ) - DTF_DOUBLE( sBound1 ) ) /
                  ( DTF_DOUBLE( sBound2 ) - DTF_DOUBLE( sBound1 ) )
                    ) + 1;
        }
    }
    else
    {
        /* if( DTF_DOUBLE( sBound1 ) > DTF_DOUBLE( sBound2 ) ) */
        if( DTF_DOUBLE( sOperand ) > DTF_DOUBLE( sBound1 ) )
        {
            DTF_INTEGER( sResultValue ) = 0;
        }
        else if( DTF_DOUBLE( sOperand ) <= DTF_DOUBLE( sBound2 ) )
        {
            STL_TRY_THROW( sCount < DTL_NATIVE_INTEGER_MAX,
                           ERROR_OUT_OF_RANGE );

            DTF_INTEGER( sResultValue ) = sCount + 1;
        }
        else
        {
            DTF_INTEGER( sResultValue ) =
                ( (stlFloat64)sCount *
                  ( DTF_DOUBLE( sBound1 ) - DTF_DOUBLE( sOperand ) ) /
                  ( DTF_DOUBLE( sBound1 ) - DTF_DOUBLE( sBound2 ) )
                    ) + 1;
        }
    }

    STL_TRY_THROW( DTF_INTEGER( sResultValue ) >= 0, ERROR_OUT_OF_RANGE );

    sResultValue->mLength = DTL_NATIVE_INTEGER_SIZE;


    return STL_SUCCESS;

    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      sArgNum,
                      "WIDTH_BUCKET" );        
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
