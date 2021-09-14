/*******************************************************************************
 * stlUInt128.h
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


#ifndef _STLUINT128_H_
#define _STLUINT128_H_ 1

/**
 * @file stlUInt128.h
 * brief Unsigned Integer 128-bit Type Definitions
 */

#include <stlTypes.h>

STL_BEGIN_DECLS

/**
 * @defgroup stlUInt128 UInt128 
 * @ingroup STL 
 * @{
 */

#define STL_UINT128_PRECISION   38  /* uint128형의 유효자리 개수 */
#define STL_UINT64_PRECISION    19  /* uint64형의 유효자리 개수 */
#define STL_FLOAT64_PRECISION   15  /* float64형의 유효자리 개수 */
#define STL_FLOAT32_PRECISION   6   /* float32형의 유효자리 개수 */

#define STL_IS_UINT128_MAX( aValue ) ( ((aValue).mHighDigit == STL_UINT64_MAX) &&       \
                                       ((aValue).mLowDigit == STL_UINT64_MAX) )

#define STL_IS_UINT128_MIN( aValue ) ( ((aValue).mHighDigit == STL_UINT64_MIN) &&       \
                                       ((aValue).mLowDigit == STL_UINT64_MIN) )

#define STL_IS_UINT128_ZERO( aValue ) ( ((aValue).mLowDigit == 0) &&    \
                                        ((aValue).mHighDigit == 0) )

#define STL_IS_NOT_UINT128_ZERO( aValue ) ( ((aValue).mLowDigit > 0) || \
                                            ((aValue).mHighDigit > 0) )

#define STL_COPY_UINT128( aDst, aSrc ) {                \
        if( (aDst) != (aSrc) )                          \
        {                                               \
            (aDst)->mHighDigit = (aSrc)->mHighDigit;    \
            (aDst)->mLowDigit  = (aSrc)->mLowDigit;     \
        }                                               \
    }

/**
 * brief 128bit의 값을 왼쪽으로 aCount만큼 shift 연산한다.
 * param aOperand shift 연산을 수행할 값  ( stlUInt128 )
 * param aCount shift할 횟수  ( stlInt32 )
 * return 없음
 */
#define stlShiftLeftUInt128WithCount( aOperand, aCount )                                            \
    {                                                                                               \
        if( (aCount) <= 0 )                                                                         \
        {                                                                                           \
            if( (aCount) < 64 )                                                                     \
            {                                                                                       \
                (aOperand).mHighDigit <<= (aCount);                                                 \
                (aOperand).mHighDigit |=                                                            \
                    ( ((aOperand).mLowDigit & gUInt64HighBitMask[(aCount)-1]) >> (64 - (aCount)) ); \
                (aOperand).mLowDigit <<= (aCount);                                                  \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                (aCount) -= 64;                                                                     \
                (aOperand).mHighDigit = ( (aOperand).mLowDigit << (aCount) );                       \
                (aOperand).mLowDigit = 0;                                                           \
            }                                                                                       \
        }                                                                                           \
    }


/**
 * brief 256bit의 값을 왼쪽으로 aCount만큼 shift 연산한다.
 * param aHighDigits shift 연산을 수행할 high 128bit 값  ( stlUInt128 )
 * param aLowDigits shift 연산을 수행할 low 128bit 값  ( stlUInt128 )
 * param aCount shift할 횟수  ( stlInt32 )
 * return 없음
 */
#define stlShiftLeftUInt256WithCount( aHighDigits, aLowDigits, aCount )                             \
    {                                                                                               \
        if( (aCount) > 0 )                                                                          \
        {                                                                                           \
            if( (aCount) < 64 )                                                                     \
            {                                                                                       \
                (aHighDigits).mHighDigit <<= (aCount);                                              \
                (aHighDigits).mHighDigit |=                                                         \
                    ( ((aHighDigits).mLowDigit & gUInt64HighBitMask[(aCount)-1]) >> (64 - (aCount)) ); \
                (aHighDigits).mLowDigit <<= (aCount);                                               \
                (aHighDigits).mLowDigit |=                                                          \
                    ( ((aLowDigits).mHighDigit & gUInt64HighBitMask[(aCount)-1]) >> (64 - (aCount)) ); \
                (aLowDigits).mHighDigit <<= (aCount);                                               \
                (aLowDigits).mHighDigit |=                                                          \
                    ( ((aLowDigits).mLowDigit & gUInt64HighBitMask[(aCount)-1]) >> (64 - (aCount)) ); \
                (aLowDigits).mLowDigit <<= (aCount);                                                \
            }                                                                                       \
            else if( (aCount) < 128 )                                                               \
            {                                                                                       \
                (aCount) -= 64;                                                                     \
                if( (aCount) == 0 )                                                                 \
                {                                                                                   \
                    (aHighDigits).mHighDigit = (aHighDigits).mLowDigit;                             \
                    (aHighDigits).mLowDigit  = (aLowDigits).mHighDigit;                             \
                    (aLowDigits).mHighDigit = (aLowDigits).mLowDigit;                               \
                    (aLowDigits).mLowDigit = 0;                                                     \
                }                                                                                   \
                else                                                                                \
                {                                                                                   \
                    (aHighDigits).mHighDigit = ( (aHighDigits).mLowDigit << (aCount) );             \
                    (aHighDigits).mHighDigit |=                                                     \
                        ( ((aLowDigits).mHighDigit & gUInt64HighBitMask[(aCount)-1]) >> (64 - (aCount)) ); \
                    (aHighDigits).mLowDigit  = ( (aLowDigits).mHighDigit << (aCount) );             \
                    (aHighDigits).mLowDigit |=                                                      \
                        ( ((aLowDigits).mLowDigit & gUInt64HighBitMask[(aCount)-1]) >> (64 - (aCount)) ); \
                    (aLowDigits).mHighDigit = ( (aLowDigits).mLowDigit << (aCount) );               \
                    (aLowDigits).mLowDigit = 0;                                                     \
                }                                                                                   \
            }                                                                                       \
            else if( (aCount) < 192 )                                                               \
            {                                                                                       \
                (aCount) -= 128;                                                                    \
                if( (aCount) == 0 )                                                                 \
                {                                                                                   \
                    (aHighDigits).mHighDigit = (aLowDigits).mHighDigit;                             \
                    (aHighDigits).mLowDigit = (aLowDigits).mLowDigit;                               \
                    (aLowDigits).mHighDigit = 0;                                                    \
                    (aLowDigits).mLowDigit = 0;                                                     \
                }                                                                                   \
                else                                                                                \
                {                                                                                   \
                    (aHighDigits).mHighDigit = ( (aLowDigits).mHighDigit << (aCount) );             \
                    (aHighDigits).mHighDigit |=                                                     \
                        ( ((aLowDigits).mLowDigit & gUInt64HighBitMask[(aCount)-1]) >> (64 - (aCount)) ); \
                    (aHighDigits).mLowDigit = ( (aLowDigits).mLowDigit << (aCount) );               \
                    (aLowDigits).mHighDigit = 0;                                                    \
                    (aLowDigits).mLowDigit = 0;                                                     \
                }                                                                                   \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                (aCount) -= 192;                                                                    \
                if( (aCount) == 0 )                                                                 \
                {                                                                                   \
                    (aHighDigits).mHighDigit = (aLowDigits).mLowDigit;                              \
                    (aHighDigits).mLowDigit = 0;                                                    \
                    (aLowDigits).mHighDigit = 0;                                                    \
                    (aLowDigits).mLowDigit = 0;                                                     \
                }                                                                                   \
                else                                                                                \
                {                                                                                   \
                    (aHighDigits).mHighDigit = ( (aLowDigits).mLowDigit << (aCount) );              \
                    (aHighDigits).mLowDigit = 0;                                                    \
                    (aLowDigits).mHighDigit = 0;                                                    \
                    (aLowDigits).mLowDigit = 0;                                                     \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
    }

/**
 * brief 256bit의 값을 왼쪽으로 shift 연산한다.
 * param aHighDigits shift 연산을 수행할 high 128bit 값  ( stlUInt128 )
 * param aLowDigits shift 연산을 수행할 low 128bit 값  ( stlUInt128 )
 * return 없음
 */
#define stlShiftLeftUInt256( aHighDigits, aLowDigits )          \
    {                                                           \
        (aHighDigits).mHighDigit    <<= 1;                      \
        if( (aHighDigits).mLowDigit & STL_UINT64_HIGH_BIT )     \
        {                                                       \
            (aHighDigits).mHighDigit    |= STL_UINT64_LOW_BIT;  \
        }                                                       \
        (aHighDigits).mLowDigit     <<= 1;                      \
                                                                \
        if( (aLowDigits).mHighDigit & STL_UINT64_HIGH_BIT )     \
        {                                                       \
            (aHighDigits).mLowDigit     |= STL_UINT64_LOW_BIT;  \
        }                                                       \
                                                                \
        (aLowDigits).mHighDigit      <<= 1;                     \
        if( (aLowDigits).mLowDigit & STL_UINT64_HIGH_BIT )      \
        {                                                       \
            (aLowDigits).mHighDigit     |= STL_UINT64_LOW_BIT;  \
        }                                                       \
        (aLowDigits).mLowDigit       <<= 1;                     \
    }

/**
 * brief 128bit의 값을 오른쪽으로 aCount만큼 shift 연산한다.
 * param aOperand shift 연산을 수행할 값  ( stlUInt128 )
 * param aCount shift할 횟수  ( stlInt32 )
 * return 없음
 */
#define stlShiftRightUInt128WithCount( aOperand, aCount )                                       \
    {                                                                                           \
        if( (aCount) <= 0 )                                                                     \
        {                                                                                       \
            /* Do Nothing */                                                                    \
        }                                                                                       \
        else if( (aCount) < 64 )                                                                \
        {                                                                                       \
            (aOperand).mLowDigit >>= (aCount);                                                  \
            (aOperand).mLowDigit |=                                                             \
                ( ((aOperand).mHighDigit & gUInt64LowBitMask[(aCount)-1]) << (64 - (aCount)) ); \
            (aOperand).mHighDigit >>= (aCount);                                                 \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            (aCount) -= 64;                                                                     \
            (aOperand).mLowDigit = ( (aOperand).mHighDigit >> (aCount) );                       \
            (aOperand).mHighDigit = 0;                                                          \
        }                                                                                       \
    }

/**
 * brief 256bit의 값을 오른쪽으로 aCount만큼 shift 연산한다.
 * param aHighDigits shift 연산을 수행할 high 128bit 값  ( stlUInt128 )
 * param aLowDigits shift 연산을 수행할 low 128bit 값  ( stlUInt128 )
 * param aCount shift할 횟수  ( stlInt32 )
 * return 없음
 */
#define stlShiftRightUInt256WithCount( aHighDigits, aLowDigits, aCount )                            \
    {                                                                                               \
        if( (aCount) <= 0 )                                                                         \
        {                                                                                           \
            /* Do Nothing */                                                                        \
        }                                                                                           \
        else if( (aCount) < 64 )                                                                    \
        {                                                                                           \
            (aLowDigits).mLowDigit >>= (aCount);                                                    \
            (aLowDigits).mLowDigit |=                                                               \
                ( ((aLowDigits).mHighDigit & gUInt64LowBitMask[(aCount)-1]) << (64 - (aCount)) );   \
            (aLowDigits).mHighDigit >>= (aCount);                                                   \
            (aLowDigits).mHighDigit |=                                                              \
                ( ((aHighDigits).mLowDigit & gUInt64LowBitMask[(aCount)-1]) << (64 - (aCount)) );   \
            (aHighDigits).mLowDigit >>= (aCount);                                                   \
            (aHighDigits).mLowDigit |=                                                              \
                ( ((aHighDigits).mHighDigit & gUInt64LowBitMask[(aCount)-1]) << (64 - (aCount)) );  \
            (aHighDigits).mHighDigit >>= (aCount);                                                  \
        }                                                                                           \
        else if( (aCount) < 128 )                                                                   \
        {                                                                                           \
            (aCount) -= 64;                                                                         \
            if( (aCount) == 0 )                                                                     \
            {                                                                                       \
                (aLowDigits).mLowDigit = (aLowDigits).mHighDigit;                                   \
                (aLowDigits).mHighDigit = (aHighDigits).mLowDigit;                                  \
                (aHighDigits).mLowDigit = (aHighDigits).mHighDigit;                                 \
                (aHighDigits).mHighDigit = 0;                                                       \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                (aLowDigits).mLowDigit = ( (aLowDigits).mHighDigit >> (aCount) );                   \
                (aLowDigits).mLowDigit |=                                                           \
                    ( ((aHighDigits).mLowDigit & gUInt64LowBitMask[(aCount)-1]) << (64 - (aCount)) ); \
                (aLowDigits).mHighDigit = ( (aHighDigits).mLowDigit >> (aCount) );                  \
                (aLowDigits).mHighDigit |=                                                          \
                    ( ((aHighDigits).mHighDigit & gUInt64LowBitMask[(aCount)-1]) << (64 - (aCount)) ); \
                (aHighDigits).mLowDigit = ( (aHighDigits).mHighDigit >> (aCount) );                 \
                (aHighDigits).mHighDigit = 0;                                                       \
            }                                                                                       \
        }                                                                                           \
        else if( (aCount) < 192 )                                                                   \
        {                                                                                           \
            (aCount) -= 128;                                                                        \
            if( (aCount) == 0 )                                                                     \
            {                                                                                       \
                (aLowDigits).mLowDigit = (aHighDigits).mLowDigit;                                   \
                (aLowDigits).mHighDigit = (aHighDigits).mHighDigit;                                 \
                (aHighDigits).mLowDigit = 0;                                                        \
                (aHighDigits).mHighDigit = 0;                                                       \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                (aLowDigits).mLowDigit = ( (aHighDigits).mLowDigit >> (aCount) );                   \
                (aLowDigits).mLowDigit |=                                                           \
                    ( ((aHighDigits).mHighDigit & gUInt64LowBitMask[(aCount)-1]) << (64 - (aCount)) ); \
                (aLowDigits).mHighDigit = ( (aHighDigits).mHighDigit >> (aCount) );                 \
                (aHighDigits).mLowDigit = 0;                                                        \
                (aHighDigits).mHighDigit = 0;                                                       \
            }                                                                                       \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            (aCount) -= 192;                                                                        \
            if( (aCount) == 0 )                                                                     \
            {                                                                                       \
                (aLowDigits).mLowDigit = (aHighDigits).mHighDigit;                                  \
                (aLowDigits).mHighDigit = 0;                                                        \
                (aHighDigits).mLowDigit = 0;                                                        \
                (aHighDigits).mHighDigit = 0;                                                       \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                (aLowDigits).mLowDigit = ( (aHighDigits).mHighDigit >> (aCount) );                  \
                (aLowDigits).mHighDigit = 0;                                                        \
                (aHighDigits).mLowDigit = 0;                                                        \
                (aHighDigits).mHighDigit = 0;                                                       \
            }                                                                                       \
        }                                                                                           \
    }

/**
 * brief 256bit의 값을 오른쪽으로 shift 연산한다.
 * param aHighDigits shift 연산을 수행할 high 128bit 값  ( stlUInt128 )
 * param aLowDigits shift 연산을 수행할 low 128bit 값  ( stlUInt128 )
 * return 없음
 */
#define stlShiftRightUInt256( aHighDigits, aLowDigits )         \
    {                                                           \
        (aLowDigits).mLowDigit   >>= 1;                         \
        if( (aLowDigits).mHighDigit & STL_UINT64_LOW_BIT )      \
        {                                                       \
            (aLowDigits).mLowDigit   |= STL_UINT64_HIGH_BIT;    \
        }                                                       \
        else                                                    \
        {                                                       \
            /* Do Nothing */                                    \
        }                                                       \
        (aLowDigits).mHighDigit  >>= 1;                         \
                                                                \
        if( (aHighDigits).mLowDigit & STL_UINT64_LOW_BIT )      \
        {                                                       \
            (aLowDigits).mHighDigit  |= STL_UINT64_HIGH_BIT;    \
        }                                                       \
        else                                                    \
        {                                                       \
            /* Do Nothing */                                    \
        }                                                       \
                                                                \
        (aHighDigits).mLowDigit  >>= 1;                         \
        if( (aHighDigits).mHighDigit & STL_UINT64_LOW_BIT )     \
        {                                                       \
            (aHighDigits).mLowDigit  |= STL_UINT64_HIGH_BIT;    \
        }                                                       \
        else                                                    \
        {                                                       \
            /* Do Nothing */                                    \
        }                                                       \
        (aHighDigits).mHighDigit >>= 1;                         \
    }

/**
 * brief uint128 값에 대해 왼쪽 shift 연산을 수행한다.
 * param aOperand shift연산을 수행할 uint128  ( stlUInt128 )
 * return 없음
 */
#define stlShiftLeftUInt128( aOperand )                         \
    {                                                           \
        if( (aOperand).mLowDigit & STL_UINT64_HIGH_BIT )        \
        {                                                       \
            (aOperand).mHighDigit    <<= 1;                     \
            (aOperand).mLowDigit     <<= 1;                     \
            (aOperand).mHighDigit    |= STL_UINT64_LOW_BIT;     \
        }                                                       \
        else                                                    \
        {                                                       \
            (aOperand).mHighDigit    <<= 1;                     \
            (aOperand).mLowDigit     <<= 1;                     \
        }                                                       \
    }

/**
 * brief uint128 값에 대해 오른쪽 shift 연산을 수행한다.
 * param aOperand shift연산을 수행할 uint128  ( stlUInt128 )
 * return 없음
 */
#define stlShiftRightUInt128( aOperand )                        \
    {                                                           \
        if( (aOperand).mHighDigit & STL_UINT64_LOW_BIT )        \
        {                                                       \
            (aOperand).mHighDigit    >>= 1;                     \
            (aOperand).mLowDigit     >>= 1;                     \
            (aOperand).mLowDigit    |= STL_UINT64_HIGH_BIT;     \
        }                                                       \
        else                                                    \
        {                                                       \
            (aOperand).mHighDigit    >>= 1;                     \
            (aOperand).mLowDigit     >>= 1;                     \
        }                                                       \
    }

/**
 * brief stlUInt64을 stlUInt128 형태로 변경한다.
 * param aValue stlUInt64형 입력 값  ( stlUInt64 )
 * param aResult stlUInt128형 결과를 저장할 공간 포인터  ( stlUInt128 * )
 */
#define stlSetToUInt128FromUInt64( aValue, aResult )    \
    {                                                   \
        (aResult)->mHighDigit = 0;                      \
        (aResult)->mLowDigit  = (aValue);               \
    }


#define SET_PRECISION_LOW_LEVEL1( aValue, aSeq, aPrecision ) {                  \
        if( (aValue).mLowDigit < gPreDefinedPowUInt128[aSeq].mLowDigit )        \
        {                                                                       \
            (aPrecision) = (aSeq);                                              \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            (aPrecision) = (aSeq) + 1;                                          \
        }                                                                       \
    }

#define SET_PRECISION_LOW_LEVEL2( aValue, aSeq, aPrecision ) {                  \
        if( (aValue).mLowDigit < gPreDefinedPowUInt128[aSeq].mLowDigit )        \
        {                                                                       \
            SET_PRECISION_LOW_LEVEL1( (aValue), (aSeq) - 1, (aPrecision) );     \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            SET_PRECISION_LOW_LEVEL1( (aValue), (aSeq) + 1, (aPrecision) );     \
        }                                                                       \
    }

#define SET_PRECISION_LOW_LEVEL3( aValue, aSeq, aPrecision ) {                  \
        if( (aValue).mLowDigit < gPreDefinedPowUInt128[aSeq].mLowDigit )        \
        {                                                                       \
            SET_PRECISION_LOW_LEVEL2( (aValue), (aSeq) - 2, (aPrecision) );     \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            SET_PRECISION_LOW_LEVEL2( (aValue), (aSeq) + 2, (aPrecision) );     \
        }                                                                       \
    }

#define SET_PRECISION_HIGHLOW_LEVEL1( aValue, aSeq, aPrecision ) {                      \
        if( ((aValue).mHighDigit < gPreDefinedPowUInt128[aSeq].mHighDigit) ||           \
            ( ((aValue).mHighDigit == gPreDefinedPowUInt128[aSeq].mHighDigit) &&        \
              ((aValue).mLowDigit < gPreDefinedPowUInt128[aSeq].mLowDigit) ) )          \
        {                                                                               \
            (aPrecision) = (aSeq);                                                      \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (aPrecision) = (aSeq) + 1;                                                  \
        }                                                                               \
    }

#define SET_PRECISION_HIGHLOW_LEVEL2( aValue, aSeq, aPrecision ) {                      \
        if( ((aValue).mHighDigit < gPreDefinedPowUInt128[aSeq].mHighDigit) ||           \
            ( ((aValue).mHighDigit == gPreDefinedPowUInt128[aSeq].mHighDigit) &&        \
              ((aValue).mLowDigit < gPreDefinedPowUInt128[aSeq].mLowDigit) ) )          \
        {                                                                               \
            SET_PRECISION_HIGHLOW_LEVEL1( (aValue), (aSeq) - 1, (aPrecision) );         \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            SET_PRECISION_HIGHLOW_LEVEL1( (aValue), (aSeq) + 1, (aPrecision) );         \
        }                                                                               \
    }

#define SET_PRECISION_HIGHLOW_LEVEL3( aValue, aSeq, aPrecision ) {                      \
        if( ((aValue).mHighDigit < gPreDefinedPowUInt128[aSeq].mHighDigit) ||           \
            ( ((aValue).mHighDigit == gPreDefinedPowUInt128[aSeq].mHighDigit) &&        \
              ((aValue).mLowDigit < gPreDefinedPowUInt128[aSeq].mLowDigit) ) )          \
        {                                                                               \
            SET_PRECISION_HIGHLOW_LEVEL2( (aValue), (aSeq) - 2, (aPrecision) );         \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            SET_PRECISION_HIGHLOW_LEVEL2( (aValue), (aSeq) + 2, (aPrecision) );         \
        }                                                                               \
    }

/**
 * brief stlUInt128의 precision을 반환한다.
 * param aValue stlUInt128형 입력 값  ( stlUInt128 )
 * return precision의 값  ( stlInt64 )
 */
#define stlGetPrecisionUInt128( aValue, aPrecision )                                    \
    {                                                                                   \
        if( (aValue).mHighDigit == 0 )                                                  \
        {                                                                               \
            if( (aValue).mLowDigit < gPreDefinedPowUInt128[8].mLowDigit )               \
            {                                                                           \
                SET_PRECISION_LOW_LEVEL3( aValue, 4, aPrecision );                      \
            }                                                                           \
            else if( (aValue).mLowDigit < gPreDefinedPowUInt128[16].mLowDigit )         \
            {                                                                           \
                SET_PRECISION_LOW_LEVEL3( aValue, 12, aPrecision );                     \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                SET_PRECISION_LOW_LEVEL2( aValue, 18, aPrecision );                     \
            }                                                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            if( ((aValue).mHighDigit < gPreDefinedPowUInt128[27].mHighDigit) ||         \
                ( ((aValue).mHighDigit == gPreDefinedPowUInt128[27].mHighDigit) &&      \
                  ((aValue).mLowDigit < gPreDefinedPowUInt128[27].mLowDigit) ) )        \
            {                                                                           \
                SET_PRECISION_HIGHLOW_LEVEL3( aValue, 23, aPrecision );                 \
            }                                                                           \
            else if( ((aValue).mHighDigit < gPreDefinedPowUInt128[35].mHighDigit) ||    \
                     ( ((aValue).mHighDigit == gPreDefinedPowUInt128[35].mHighDigit) && \
                       ((aValue).mLowDigit < gPreDefinedPowUInt128[35].mLowDigit) ) )   \
            {                                                                           \
                SET_PRECISION_HIGHLOW_LEVEL3( aValue, 31, aPrecision );                 \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                SET_PRECISION_HIGHLOW_LEVEL2( aValue, 37, aPrecision );                 \
            }                                                                           \
        }                                                                               \
    }



stlInt32 stlGetFirstBitPosUInt128( stlUInt128   aValue );

stlBool stlSetToUInt128FromInt64( stlInt64      aValue,
                                  stlUInt128  * aResult );

stlBool stlSetToUInt128FromReal( stlFloat32       aValue,
                                 stlUInt128     * aResult,
                                 stlErrorStack  * aErrorStack );

stlBool stlSetToUInt128FromDouble( stlFloat64      aValue,
                                   stlUInt128    * aResult,
                                   stlErrorStack * aErrorStack );

stlBool stlGetToInt64FromUInt128( stlUInt128    aValue,
                                  stlInt64    * aResult );

stlBool stlGetToUInt64FromUInt128( stlUInt128    aValue,
                                   stlUInt64   * aResult );

void stlGetToRealFromUInt128( stlUInt128    aValue,
                              stlFloat32  * aResult );

void stlGetToDoubleFromUInt128( stlUInt128    aValue,
                                stlFloat64  * aResult );

stlBool stlAddUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult );

stlBool stlSubUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult );

stlBool stlMulUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult );

stlBool stlMulUInt64( stlUInt64     aOperand1,
                      stlUInt64     aOperand2,
                      stlUInt128  * aResult );

stlBool stlDivisionUInt128By32( stlUInt128  * aDividend,
                                stlUInt128    aDivisor,
                                stlUInt128  * aQuotient,
                                stlUInt128  * aRemainder );

stlBool stlDivRemUInt128( stlUInt128    aOperand1,
                          stlUInt128    aOperand2,
                          stlUInt128  * aQuotient,
                          stlUInt128  * aRemainder );

void stlMultiplicationOfUInt128( stlUInt128       aOperand1,
                                 stlUInt128       aOperand2,
                                 stlUInt128     * aResultHigh,
                                 stlUInt128     * aResultLow );

stlBool stlDivisionUInt256By32( stlUInt128  * a256HighDigits,
                                stlUInt128  * a256LowDigits,
                                stlUInt128    aDivisor,
                                stlUInt128  * aQuotientHigh,
                                stlUInt128  * aQuotientLow,
                                stlUInt128  * aRemainder );

stlBool stlDivisionUInt256By128( stlUInt128  * a256HighDigits,
                                 stlUInt128  * a256LowDigits,
                                 stlUInt128    aDivisor,
                                 stlUInt128  * aQuotientHigh,
                                 stlUInt128  * aQuotientLow,
                                 stlUInt128  * aRemainder );

stlBool stlDivUInt256( stlUInt128    aDividend1,
                       stlUInt128    aDividend2,
                       stlUInt128    aDivisor,
                       stlUInt128  * aQuotient,
                       stlInt32    * aExponent );

stlBool stlDivUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult );

stlBool stlRemUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult );

stlBool stlIncUInt128( stlUInt128  * aOperand );

stlBool stlDecUInt128( stlUInt128  * aOperand );

stlBool stlIsEqUInt128( stlUInt128    aOperand1,
                        stlUInt128    aOperand2 );

stlInt32 stlCmpUInt128( stlUInt128    aOperand1,
                        stlUInt128    aOperand2 );

stlBool stlGcfUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult );

stlBool stlLcmUInt128( stlUInt128    aOperand1,
                       stlUInt128    aOperand2,
                       stlUInt128  * aResult );

stlBool stlRoundUInt128( stlUInt128     aValue,
                         stlInt32       aScale,
                         stlUInt128   * aResult );

stlBool stlCeilUInt128( stlUInt128      aValue,
                        stlInt32        aScale,
                        stlUInt128    * aResult );

stlBool stlTruncUInt128( stlUInt128     aValue,
                         stlInt32       aScale,
                         stlUInt128   * aResult );


/** @} */

STL_END_DECLS

#endif /* _STLUINT128_H_ */



