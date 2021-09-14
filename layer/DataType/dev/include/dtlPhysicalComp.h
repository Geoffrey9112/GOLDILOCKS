/*******************************************************************************
 * dtlPhysicalComp.h
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
#ifndef _DTL_PHYSICAL_COMP_H_
#define _DTL_PHYSICAL_COMP_H__ 1

/**
 * @file dtlPhysicalComp.h
 * @brief Align이 보장되지 않은 value간의 연산
 */

/**
 * @addtogroup dtlPhysicalFunction
 * @{
 */

typedef struct dtlCompArg
{
    stlChar        *mChar;
    dtlBigIntType   mBigInt1;
    dtlBigIntType   mBigInt2;
    dtlDoubleType   mDouble1;
    dtlDoubleType   mDouble2;
    dtlRealType     mReal1;
    dtlRealType     mReal2;
    dtlIntegerType  mInt1;
    dtlIntegerType  mInt2;
    dtlSmallIntType mSmallInt1;
    dtlSmallIntType mSmallInt2;
} dtlCompArg;

extern stlInt8 dtlNumericLenCompTable[22][22][2];

/**
 * 이 상수는 string compare시 strncmp가 for문으로 검사하는 것보다,
 * 빠른 결과를 내는 최소한의 문자 길이이다.
 * 개인 PC를 가지고 테스트해서 찾은 값이기 때문에,
 * 적당한 값으로 재조정할 수 있으며, 프로퍼티로 뺄 수도 있다.
 */
#define DTL_STRCOMP_MIN_LEN     11

/*******************************************************************************
 * COMPARE MACROS for PERFORMANCE
 *******************************************************************************/
#define DTL_COMPARE_SMALLINT( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen ) \
    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aLeftVal, (aArg).mSmallInt1 );                    \
    DTL_GET_NATIVE_SMALLINT_FROM_PTR( aRightVal, (aArg).mSmallInt2 );                   \
    (aResult) = ( (aArg).mSmallInt1 > (aArg).mSmallInt2 ) -                             \
                ( (aArg).mSmallInt1 < (aArg).mSmallInt2 )

#define DTL_COMPARE_INTEGER( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen )  \
    DTL_GET_NATIVE_INTEGER_FROM_PTR( aLeftVal, (aArg).mInt1 );                          \
    DTL_GET_NATIVE_INTEGER_FROM_PTR( aRightVal, (aArg).mInt2 );                         \
    (aResult) = ( (aArg).mInt1 > (aArg).mInt2 ) -                                       \
                ( (aArg).mInt1 < (aArg).mInt2 )

#define DTL_COMPARE_BIGINT( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen )   \
    DTL_GET_NATIVE_BIGINT_FROM_PTR( aLeftVal, (aArg).mBigInt1 );                        \
    DTL_GET_NATIVE_BIGINT_FROM_PTR( aRightVal, (aArg).mBigInt2 );                       \
    (aResult) = ( (aArg).mBigInt1 > (aArg).mBigInt2 ) -                                 \
                ( (aArg).mBigInt1 < (aArg).mBigInt2 )

#define DTL_COMPARE_REAL( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen )     \
    DTL_GET_NATIVE_REAL_FROM_PTR( aLeftVal, (aArg).mReal1 );                            \
    DTL_GET_NATIVE_REAL_FROM_PTR( aRightVal, (aArg).mReal2 );                           \
    (aResult) = ( (aArg).mReal1 > (aArg).mReal2 ) -                                     \
                ( (aArg).mReal1 < (aArg).mReal2 )

#define DTL_COMPARE_DOUBLE( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen )   \
    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aLeftVal, (aArg).mDouble1 );                        \
    DTL_GET_NATIVE_DOUBLE_FROM_PTR( aRightVal, (aArg).mDouble2 );                       \
    (aResult) = ( (aArg).mDouble1 > (aArg).mDouble2 ) -                                 \
                ( (aArg).mDouble1 < (aArg).mDouble2 )


#ifdef STL_SUPPORT_MISSALIGNED_ADDRESS
#define DTL_COMPARE_NUMERIC( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen )  \
    {                                                                                   \
        register stlUInt8          * tmpLeftDigit;                                      \
        register stlUInt8          * tmpRightDigit;                                     \
        register stlInt32            tmpLen;                                            \
        register dtlCompareResult    tmpResult;                                         \
                                                                                        \
        if( (aLeftLen) == (aRightLen) )                                                 \
        {                                                                               \
            tmpLen = (aLeftLen);                                                        \
            tmpResult = DTL_COMPARISON_EQUAL;                                           \
        }                                                                               \
        else if( (aLeftLen) < (aRightLen) )                                             \
        {                                                                               \
            tmpLen = (aLeftLen);                                                        \
            tmpResult = DTL_COMPARISON_LESS;                                            \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            tmpLen = (aRightLen);                                                       \
            tmpResult = DTL_COMPARISON_GREATER;                                         \
        }                                                                               \
                                                                                        \
        tmpLeftDigit  = (stlUInt8*)(aLeftVal);                                          \
        tmpRightDigit = (stlUInt8*)(aRightVal);                                         \
                                                                                        \
        while( tmpLen >= 4 )                                                            \
        {                                                                               \
            if( *((stlUInt32*)tmpLeftDigit) == *((stlUInt32*)tmpRightDigit) )           \
            {                                                                           \
                tmpLeftDigit += 4;                                                      \
                tmpRightDigit += 4;                                                     \
                tmpLen -= 4;                                                            \
                continue;                                                               \
            }                                                                           \
                                                                                        \
            if( *((stlUInt16*)tmpLeftDigit) == *((stlUInt16*)tmpRightDigit) )           \
            {                                                                           \
                if( tmpLeftDigit[2] == tmpRightDigit[2] )                               \
                {                                                                       \
                    if( tmpLeftDigit[3] < tmpRightDigit[3] )                            \
                    {                                                                   \
                        tmpResult = DTL_COMPARISON_LESS;                                \
                    }                                                                   \
                    else                                                                \
                    {                                                                   \
                        tmpResult = DTL_COMPARISON_GREATER;                             \
                    }                                                                   \
                }                                                                       \
                else if( tmpLeftDigit[2] < tmpRightDigit[2] )                           \
                {                                                                       \
                    tmpResult = DTL_COMPARISON_LESS;                                    \
                }                                                                       \
                else                                                                    \
                {                                                                       \
                    tmpResult = DTL_COMPARISON_GREATER;                                 \
                }                                                                       \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                if( tmpLeftDigit[0] == tmpRightDigit[0] )                               \
                {                                                                       \
                    if( tmpLeftDigit[1] < tmpRightDigit[1] )                            \
                    {                                                                   \
                        tmpResult = DTL_COMPARISON_LESS;                                \
                    }                                                                   \
                    else                                                                \
                    {                                                                   \
                        tmpResult = DTL_COMPARISON_GREATER;                             \
                    }                                                                   \
                }                                                                       \
                else if( tmpLeftDigit[0] < tmpRightDigit[0] )                           \
                {                                                                       \
                    tmpResult = DTL_COMPARISON_LESS;                                    \
                }                                                                       \
                else                                                                    \
                {                                                                       \
                    tmpResult = DTL_COMPARISON_GREATER;                                 \
                }                                                                       \
            }                                                                           \
                                                                                        \
            tmpLen = -1;                                                                \
            break;                                                                      \
        }                                                                               \
                                                                                        \
        if( tmpLen >= 0 )                                                               \
        {                                                                               \
            switch( tmpLen )                                                            \
            {                                                                           \
                case 1:                                                                 \
                    if( tmpLeftDigit[0] < tmpRightDigit[0] )                            \
                    {                                                                   \
                        tmpResult = DTL_COMPARISON_LESS;                                \
                    }                                                                   \
                    else if( tmpLeftDigit[0] > tmpRightDigit[0] )                       \
                    {                                                                   \
                        tmpResult = DTL_COMPARISON_GREATER;                             \
                    }                                                                   \
                    else                                                                \
                    {                                                                   \
                        if( ((stlUInt8*)(aLeftVal))[0] <= 127 )                         \
                        {                                                               \
                            tmpResult = -tmpResult;                                     \
                        }                                                               \
                    }                                                                   \
                    break;                                                              \
                case 2:                                                                 \
                    if( tmpLeftDigit[0] == tmpRightDigit[0] )                           \
                    {                                                                   \
                        if( tmpLeftDigit[1] < tmpRightDigit[1] )                        \
                        {                                                               \
                            tmpResult = DTL_COMPARISON_LESS;                            \
                        }                                                               \
                        else if( tmpLeftDigit[1] > tmpRightDigit[1] )                   \
                        {                                                               \
                            tmpResult = DTL_COMPARISON_GREATER;                         \
                        }                                                               \
                        else                                                            \
                        {                                                               \
                            if( ((stlUInt8*)(aLeftVal))[0] <= 127 )                     \
                            {                                                           \
                                tmpResult = -tmpResult;                                 \
                            }                                                           \
                        }                                                               \
                    }                                                                   \
                    else if( tmpLeftDigit[0] < tmpRightDigit[0] )                       \
                    {                                                                   \
                        tmpResult = DTL_COMPARISON_LESS;                                \
                    }                                                                   \
                    else                                                                \
                    {                                                                   \
                        tmpResult = DTL_COMPARISON_GREATER;                             \
                    }                                                                   \
                    break;                                                              \
                case 3:                                                                 \
                    if( *((stlUInt16*)tmpLeftDigit) == *((stlUInt16*)tmpRightDigit) )   \
                    {                                                                   \
                        if( tmpLeftDigit[2] < tmpRightDigit[2] )                        \
                        {                                                               \
                            tmpResult = DTL_COMPARISON_LESS;                            \
                        }                                                               \
                        else if( tmpLeftDigit[2] > tmpRightDigit[2] )                   \
                        {                                                               \
                            tmpResult = DTL_COMPARISON_GREATER;                         \
                        }                                                               \
                        else                                                            \
                        {                                                               \
                            if( ((stlUInt8*)(aLeftVal))[0] <= 127 )                     \
                            {                                                           \
                                tmpResult = -tmpResult;                                 \
                            }                                                           \
                        }                                                               \
                    }                                                                   \
                    else                                                                \
                    {                                                                   \
                        if( tmpLeftDigit[0] == tmpRightDigit[0] )                       \
                        {                                                               \
                            if( tmpLeftDigit[1] < tmpRightDigit[1] )                    \
                            {                                                           \
                                tmpResult = DTL_COMPARISON_LESS;                        \
                            }                                                           \
                            else                                                        \
                            {                                                           \
                                tmpResult = DTL_COMPARISON_GREATER;                     \
                            }                                                           \
                        }                                                               \
                        else if( tmpLeftDigit[0] < tmpRightDigit[0] )                   \
                        {                                                               \
                            tmpResult = DTL_COMPARISON_LESS;                            \
                        }                                                               \
                        else                                                            \
                        {                                                               \
                            tmpResult = DTL_COMPARISON_GREATER;                         \
                        }                                                               \
                    }                                                                   \
                    break;                                                              \
                default:                                                                \
                    if( ((stlUInt8*)(aLeftVal))[0] <= 127 )                             \
                    {                                                                   \
                        tmpResult = -tmpResult;                                         \
                    }                                                                   \
                    break;                                                              \
            }                                                                           \
        }                                                                               \
                                                                                        \
        (aResult) = tmpResult;                                                          \
    }
        
#else
#define DTL_COMPARE_NUMERIC( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen )              \
    {                                                                                               \
        register stlUInt8          * tmpLeftDigit;                                                  \
        register stlUInt8          * tmpRightDigit;                                                 \
        register stlInt8           * tmpTable;                                                      \
        register stlInt32            tmpIdx;                                                        \
        register dtlCompareResult    tmpResult;                                                     \
                                                                                                    \
        tmpLeftDigit  = (stlUInt8*)(aLeftVal);                                                      \
        tmpRightDigit = (stlUInt8*)(aRightVal);                                                     \
                                                                                                    \
        if( tmpLeftDigit[0] == tmpRightDigit[0] )                                                   \
        {                                                                                           \
            tmpTable = dtlNumericLenCompTable[ (aLeftLen) ][ (aRightLen) ];                         \
                                                                                                    \
            if( tmpTable[1] == 0 )                                                                  \
            {                                                                                       \
                if( tmpLeftDigit[0] > 127 )                                                         \
                {                                                                                   \
                    tmpResult = tmpTable[0];                                                        \
                }                                                                                   \
                else                                                                                \
                {                                                                                   \
                    tmpResult = -tmpTable[0];                                                       \
                }                                                                                   \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                tmpIdx = 1;                                                                         \
                while( 1 )                                                                          \
                {                                                                                   \
                    if( tmpLeftDigit[tmpIdx] != tmpRightDigit[tmpIdx] )                             \
                    {                                                                               \
                        tmpResult = dtlNumericCompResult[ tmpLeftDigit[tmpIdx] < tmpRightDigit[tmpIdx] ]; \
                        break;                                                                      \
                    }                                                                               \
                                                                                                    \
                    if( tmpIdx == tmpTable[1] )                                                     \
                    {                                                                               \
                        if( tmpLeftDigit[0] > 127 )                                                 \
                        {                                                                           \
                            tmpResult = tmpTable[0];                                                \
                        }                                                                           \
                        else                                                                        \
                        {                                                                           \
                            tmpResult = -tmpTable[0];                                               \
                        }                                                                           \
                        break;                                                                      \
                    }                                                                               \
                    tmpIdx++;                                                                       \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            tmpResult = dtlNumericCompResult[ tmpLeftDigit[0] < tmpRightDigit[0] ];                 \
        }                                                                                           \
                                                                                                    \
        (aResult) = tmpResult;                                                                      \
    }
#endif

#define DTL_COMPARE_VAR_CHAR( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen )   \
    (aResult) = stlMemcmp( (aLeftVal),                                                    \
                           (aRightVal),                                                   \
                           STL_MIN( aLeftLen, aRightLen ) );                              \
    if( (aResult) == 0 )                                                                  \
    {                                                                                     \
        (aResult) = (aLeftLen) - (aRightLen);                                             \
    }                                                                                     \
    if( (aResult) < 0 )                                                                   \
    {                                                                                     \
        (aResult) = DTL_COMPARISON_LESS;                                                  \
    }                                                                                     \
    else                                                                                  \
    {                                                                                     \
        (aResult) = (aResult) > 0;                                                        \
    }

#define DTL_TYPED_COMPARE( aCompFunc, aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen ) \
    if( ( aCompFunc ) == dtlPhysicalCompareNumericToNumeric )                                   \
    {                                                                                           \
        DTL_COMPARE_NUMERIC( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen );         \
    }                                                                                           \
    else if( ( aCompFunc ) == dtlPhysicalCompareVariableLengthChar )                            \
    {                                                                                           \
        DTL_COMPARE_VAR_CHAR( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen );        \
    }                                                                                           \
    else if( ( aCompFunc ) == dtlPhysicalCompareSmallIntToSmallInt )                            \
    {                                                                                           \
        DTL_COMPARE_SMALLINT( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen );        \
    }                                                                                           \
    else if( ( aCompFunc ) == dtlPhysicalCompareIntegerToInteger )                              \
    {                                                                                           \
        DTL_COMPARE_INTEGER( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen );         \
    }                                                                                           \
    else if( ( aCompFunc ) == dtlPhysicalCompareBigIntToBigInt )                                \
    {                                                                                           \
        DTL_COMPARE_BIGINT( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen );          \
    }                                                                                           \
    else if( ( aCompFunc ) == dtlPhysicalCompareRealToReal )                                    \
    {                                                                                           \
        DTL_COMPARE_REAL( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen );            \
    }                                                                                           \
    else if( ( aCompFunc ) == dtlPhysicalCompareDoubleToDouble )                                \
    {                                                                                           \
        DTL_COMPARE_DOUBLE( aResult, aArg, aLeftVal, aLeftLen, aRightVal, aRightLen );          \
    }                                                                                           \
    else                                                                                        \
    {                                                                                           \
        (aResult) = (aCompFunc)( aLeftVal, aLeftLen, aRightVal, aRightLen );                    \
    }                                                                                           \


typedef dtlCompareResult (*dtlPhysicalCompareFuncPtr) ( void      * aLeftValue,
                                                        stlInt64    aLeftLength,
                                                        void      * aRightValue,
                                                        stlInt64    aRightLength );

extern dtlPhysicalCompareFuncPtr dtlPhysicalCompareFuncList[ DTL_TYPE_MAX ][ DTL_TYPE_MAX ];


/*******************************************************************************
 * COMPARE FUNCTION for INDEX SCAN (TYPE)
 ******************************************************************************/

/* Boolean */
dtlCompareResult dtlPhysicalCompareBooleanToBoolean( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength );

/* Small Int */
dtlCompareResult dtlPhysicalCompareSmallIntToSmallInt( void      * aLeftValue,
                                                       stlInt64    aLeftLength,
                                                       void      * aRightValue,
                                                       stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareSmallIntToInteger( void      * aLeftValue,
                                                      stlInt64    aLeftLength,
                                                      void      * aRightValue,
                                                      stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareSmallIntToBigInt( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareSmallIntToReal( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareSmallIntToDouble( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareSmallIntToNumeric( void      * aLeftValue,
                                                      stlInt64    aLeftLength,
                                                      void      * aRightValue,
                                                      stlInt64    aRightLength );

/* Integer */
dtlCompareResult dtlPhysicalCompareIntegerToSmallInt( void      * aLeftValue,
                                                      stlInt64    aLeftLength,
                                                      void      * aRightValue,
                                                      stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareIntegerToInteger( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareIntegerToBigInt( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareIntegerToReal( void      * aLeftValue,
                                                  stlInt64    aLeftLength,
                                                  void      * aRightValue,
                                                  stlInt64    aRightLength );                       

dtlCompareResult dtlPhysicalCompareIntegerToDouble( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareIntegerToNumeric( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength );

/* Big Int */
dtlCompareResult dtlPhysicalCompareBigIntToSmallInt( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareBigIntToInteger( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareBigIntToBigInt( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareBigIntToReal( void      * aLeftValue,
                                                 stlInt64    aLeftLength,
                                                 void      * aRightValue,
                                                 stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareBigIntToDouble( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareBigIntToNumeric( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength );

/* Real */
dtlCompareResult dtlPhysicalCompareRealToReal( void      * aLeftValue,
                                               stlInt64    aLeftLength,
                                               void      * aRightValue,
                                               stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareRealToDouble( void      * aLeftValue,
                                                 stlInt64    aLeftLength,
                                                 void      * aRightValue,
                                                 stlInt64    aRightLength );                      

dtlCompareResult dtlPhysicalCompareRealToSmallInt( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareRealToInteger( void      * aLeftValue,
                                                  stlInt64    aLeftLength,
                                                  void      * aRightValue,
                                                  stlInt64    aRightLength );                       

dtlCompareResult dtlPhysicalCompareRealToBigInt( void      * aLeftValue,
                                                 stlInt64    aLeftLength,
                                                 void      * aRightValue,
                                                 stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareRealToNumeric( void      * aLeftValue,
                                                  stlInt64    aLeftLength,
                                                  void      * aRightValue,
                                                  stlInt64    aRightLength );

/* Double */
dtlCompareResult dtlPhysicalCompareDoubleToReal( void      * aLeftValue,
                                                 stlInt64    aLeftLength,
                                                 void      * aRightValue,
                                                 stlInt64    aRightLength );                     

dtlCompareResult dtlPhysicalCompareDoubleToDouble( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareDoubleToSmallInt( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareDoubleToInteger( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareDoubleToBigInt( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareDoubleToNumeric( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength );

/* Float : not exist */

/* Numeric */
dtlCompareResult dtlPhysicalCompareNumericToReal( void      * aLeftValue,
                                                  stlInt64    aLeftLength,
                                                  void      * aRightValue,
                                                  stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareNumericToDouble( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareNumericToSmallInt( void      * aLeftValue,
                                                      stlInt64    aLeftLength,
                                                      void      * aRightValue,
                                                      stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareNumericToInteger( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareNumericToBigInt( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareNumericToNumeric( void      * aLeftValue,
                                                     stlInt64    aLeftLength,
                                                     void      * aRightValue,
                                                     stlInt64    aRightLength );

/* Decimal : not exist */

/* Character String 공통 */
dtlCompareResult dtlPhysicalCompareFixedLengthChar( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareVariableLengthChar( void      * aLeftValue,
                                                       stlInt64    aLeftLength,
                                                       void      * aRightValue,
                                                       stlInt64    aRightLength );


/* Character Large Object : not exist */

/* Binary */
dtlCompareResult dtlPhysicalCompareFixedLengthBinary( void      * aLeftValue,
                                                      stlInt64    aLeftLength,
                                                      void      * aRightValue,
                                                      stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareVariableLengthBinary( void      * aLeftValue,
                                                         stlInt64    aLeftLength,
                                                         void      * aRightValue,
                                                         stlInt64    aRightLength );

/* Binary Large Object : not exist */

/* Date */
dtlCompareResult dtlPhysicalCompareDateToDate( void      * aLeftValue,
                                               stlInt64    aLeftLength,
                                               void      * aRightValue,
                                               stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareDateToTimestamp( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength );
                                                                                              
/* Time Without Time Zone */
dtlCompareResult dtlPhysicalCompareTimeToTime( void      * aLeftValue,
                                               stlInt64    aLeftLength,
                                               void      * aRightValue,
                                               stlInt64    aRightLength );

/* Timestamp Without Time Zone */
dtlCompareResult dtlPhysicalCompareTimestampToDate( void      * aLeftValue,
                                                    stlInt64    aLeftLength,
                                                    void      * aRightValue,
                                                    stlInt64    aRightLength );

dtlCompareResult dtlPhysicalCompareTimestampToTimestamp( void      * aLeftValue,
                                                         stlInt64    aLeftLength,
                                                         void      * aRightValue,
                                                         stlInt64    aRightLength );

/* Time With Time Zone */
dtlCompareResult dtlPhysicalCompareTimeTzToTimeTz( void      * aLeftValue,
                                                   stlInt64    aLeftLength,
                                                   void      * aRightValue,
                                                   stlInt64    aRightLength );

/* Timestamp With Time Zone */
dtlCompareResult dtlPhysicalCompareTimestampTzToTimestampTz( void      * aLeftValue,
                                                             stlInt64    aLeftLength,
                                                             void      * aRightValue,
                                                             stlInt64    aRightLength );

/* dtlIntervalYearToMonth */
dtlCompareResult dtlPhysicalCompareIntervalYearToMonth( void      * aLeftValue,
                                                        stlInt64    aLeftLength,
                                                        void      * aRightValue,
                                                        stlInt64    aRightLength );

/* dtlIntervalDayToSecond */
dtlCompareResult dtlPhysicalCompareIntervalDayToSecond( void      * aLeftValue,
                                                        stlInt64    aLeftLength,
                                                        void      * aRightValue,
                                                        stlInt64    aRightLength );

/* dtlRowId */
dtlCompareResult dtlPhysicalCompareRowId( void      * aLeftValue,
                                          stlInt64    aLeftLength,
                                          void      * aRightValue,
                                          stlInt64    aRightLength );


/*******************************************************************************
 * COMPARE FUNCTION for INDEX SCAN (TYPE)
 ******************************************************************************/

/* Character Large Object : not exist */

/* Character String 공통 */
dtlCompareResult dtlLikePhysicalCompareChar( void      * aLeftValue,
                                             stlInt64    aLeftLength,
                                             void      * aRightValue,
                                             stlInt64    aRightLength );

dtlCompareResult dtlLikeAllPhysicalCompareChar( void      * aLeftValue,
                                                stlInt64    aLeftLength,
                                                void      * aRightValue,
                                                stlInt64    aRightLength );


/** @} dtlPhysicalFunction */

#endif /* _DTL_PHYSICAL_COMP_H_ */
