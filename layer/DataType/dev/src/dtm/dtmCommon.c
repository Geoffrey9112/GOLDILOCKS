/*******************************************************************************
 * dtmCommon.c
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
 * @file dtmCommon.c
 * @brief DataType Layer 비교 연산 공통 파일
 */

#include <dtl.h>

#include <dtDef.h>
#include <dtdDataType.h>
#include <dtfNumeric.h>

/**
 * @addtogroup dtlCharStringType
 * @{
 */

/*******************************************************************************
 * STATIC 함수
 ******************************************************************************/

/**
 * @brief IS EQUAL 연산 ( CHARACTER  )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsEqualFixedLengthChar_STATIC( dtlCharType   aLeftVal,
                                                 stlInt64      aLeftLen,
                                                 dtlCharType   aRightVal,
                                                 stlInt64      aRightLen )
{
    stlInt64         sCompLen;
    stlInt64         sDiffLen;
    stlUInt8       * sLeftVal  = (stlUInt8 *) aLeftVal;
    stlUInt8       * sRightVal = (stlUInt8 *) aRightVal;

    
    if( (sDiffLen = (aLeftLen - aRightLen)) > 0 )
    {
        sCompLen = aRightLen;
    }
    else
    {
        sCompLen = aLeftLen;
    }

    while( sCompLen > 0 )
    {
        if( *sLeftVal != *sRightVal )
        {
            return STL_FALSE;
        }
        else
        {
            /* Do Nothing */
        }

        ++sLeftVal;
        ++sRightVal;
        --sCompLen;
    }
    
    if( sDiffLen == 0 )
    {
        return STL_TRUE;
    }
    else
    {
        if( sDiffLen > 0 )
        {
            while( (sDiffLen > 0) & (*sLeftVal == ' ') )
            {
                ++sLeftVal;
                --sDiffLen;
            }

            return ( sDiffLen == 0 );
        }
        else
        {
            while( (sDiffLen < 0) & (*sRightVal == ' ') )
            {
                ++sRightVal;
                ++sDiffLen;
            }

            return ( sDiffLen == 0 );
        }
    }
}

/**
 * @brief IS LESS THAN 연산 ( CHARACTER  )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanFixedLengthChar_STATIC( dtlCharType  aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    dtlCharType  aRightVal,
                                                    stlInt64     aRightLen )
{
    stlInt64         sCompLen;
    stlInt64         sDiffLen;
    stlUInt8       * sLeftVal  = (stlUInt8 *) aLeftVal;
    stlUInt8       * sRightVal = (stlUInt8 *) aRightVal;

    
    if( (sDiffLen = (aLeftLen - aRightLen)) > 0 )
    {
        sCompLen = aRightLen;
    }
    else
    {
        sCompLen = aLeftLen;
    }

    while( sCompLen > 0 )
    {
        if( *sLeftVal == *sRightVal )
        {
            /* Do Nothing */
        }
        else
        {
            if( *sLeftVal < *sRightVal )
            {
                return STL_TRUE;
            }
            else
            {
                return STL_FALSE;
            }
        }

        ++sLeftVal;
        ++sRightVal;
        --sCompLen;
    }

    if( sDiffLen == 0 )
    {
        return STL_FALSE;
    }
    else
    {
        if( sDiffLen > 0 )
        {
            while( (sDiffLen > 0) & (*sLeftVal == ' ') )
            {
                ++sLeftVal;
                --sDiffLen;
            }

            if( sDiffLen == 0 )
            {
                return STL_FALSE;
            }
            else
            {
                return ( *sLeftVal < ' ' );
            }
        }
        else
        {
            while( (sDiffLen < 0) & (*sRightVal == ' ') )
            {
                ++sRightVal;
                ++sDiffLen;
            }

            if( sDiffLen == 0 )
            {
                return STL_FALSE;
            }
            else
            {
                return ( *sRightVal > ' ' );
            }
        }
    }
}

/**
 * @brief IS LESS THAN EQUAL 연산 ( CHARACTER  )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanEqualFixedLengthChar_STATIC( dtlCharType  aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         dtlCharType  aRightVal,
                                                         stlInt64     aRightLen )
{
    stlInt64         sCompLen;
    stlInt64         sDiffLen;
    stlUInt8       * sLeftVal  = (stlUInt8 *) aLeftVal;
    stlUInt8       * sRightVal = (stlUInt8 *) aRightVal;

    
    if( (sDiffLen = (aLeftLen - aRightLen)) > 0 )
    {
        sCompLen = aRightLen;
    }
    else
    {
        sCompLen = aLeftLen;
    }

    while( sCompLen > 0 )
    {
        if( *sLeftVal == *sRightVal )
        {
            /* Do Nothing */
        }
        else
        {
            if( *sLeftVal < *sRightVal )
            {
                return STL_TRUE;
            }
            else
            {
                return STL_FALSE;
            }
        }

        ++sLeftVal;
        ++sRightVal;
        --sCompLen;
    }
    
    if( sDiffLen == 0 )
    {
        return STL_TRUE;
    }
    else
    {
        if( sDiffLen > 0 )
        {
            while( (sDiffLen > 0) & (*sLeftVal == ' ') )
            {
                ++sLeftVal;
                --sDiffLen;
            }

            if( sDiffLen == 0 )
            {
                return STL_TRUE;
            }
            else
            {
                return ( *sLeftVal < ' ' );
            }
        }
        else
        {
            while( (sDiffLen < 0) & (*sRightVal == ' ') )
            {
                ++sRightVal;
                ++sDiffLen;
            }

            if( sDiffLen == 0 )
            {
                return STL_TRUE;
            }
            else
            {
                return ( *sRightVal > ' ' );
            }
        }
    }
}


/*******************************************************************************
 * INLINE 함수
 ******************************************************************************/

inline stlBool dtlIsEqualFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 dtlCharType  aRightVal,
                                                 stlInt64     aRightLen )
{
    return dtlIsEqualFixedLengthChar_STATIC( aLeftVal,
                                             aLeftLen,
                                             aRightVal,
                                             aRightLen );
}

inline stlBool dtlIsNotEqualFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    dtlCharType  aRightVal,
                                                    stlInt64     aRightLen )
{
    return !dtlIsEqualFixedLengthChar_STATIC( aLeftVal,
                                              aLeftLen,
                                              aRightVal,
                                              aRightLen );
}

inline stlBool dtlIsLessThanFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    dtlCharType  aRightVal,
                                                    stlInt64     aRightLen )
{
    return dtlIsLessThanFixedLengthChar_STATIC( aLeftVal,
                                                aLeftLen,
                                                aRightVal,
                                                aRightLen );
}

inline stlBool dtlIsLessThanEqualFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         dtlCharType  aRightVal,
                                                         stlInt64     aRightLen )
{
    return dtlIsLessThanEqualFixedLengthChar_STATIC( aLeftVal,
                                                     aLeftLen,
                                                     aRightVal,
                                                     aRightLen );
}


inline stlBool dtlIsGreaterThanFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       dtlCharType  aRightVal,
                                                       stlInt64     aRightLen )
{
    return dtlIsLessThanFixedLengthChar_STATIC( aRightVal,
                                                aRightLen,
                                                aLeftVal,
                                                aLeftLen );
}

inline stlBool dtlIsGreaterThanEqualFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            dtlCharType  aRightVal,
                                                            stlInt64     aRightLen )
{
    return dtlIsLessThanEqualFixedLengthChar_STATIC( aRightVal,
                                                     aRightLen,
                                                     aLeftVal,
                                                     aLeftLen );
}

/** @} dtlCharStringType */

/**
 * @addtogroup dtlBinaryStringType
 * @{
 */

/*******************************************************************************
 * STATIC 함수
 ******************************************************************************/

/**
 * @brief IS EQUAL 연산 ( BINARY  )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsEqualFixedLengthBinary_STATIC( dtlBinaryType   aLeftVal,
                                                   stlInt64        aLeftLen,
                                                   dtlBinaryType   aRightVal,
                                                   stlInt64        aRightLen )
{
    stlInt64    sCompLen;
    stlInt64    sDiffLen;
    stlChar   * sTmpPtr;

    if( (sDiffLen = (aLeftLen - aRightLen)) > 0 )
    {
        sCompLen = aRightLen;
    }
    else
    {
        sCompLen = aLeftLen;
    }
    
    if( stlMemcmp( aLeftVal, aRightVal, sCompLen ) == 0 )
    {
        if( sDiffLen == 0 )
        {
            return STL_TRUE;
        }
        else
        {            
            if( sDiffLen > 0 )
            {
                sTmpPtr = aLeftVal + sCompLen;
                sCompLen = sDiffLen;
            }
            else
            {
                sTmpPtr = aRightVal + sCompLen;
                sCompLen = -sDiffLen;
            }

            while( (sCompLen > 0) && (*sTmpPtr == 0x00) )
            {
                ++sTmpPtr;
                --sCompLen;
            }

            if( sCompLen == 0 )
            {
                return STL_TRUE;
            }
            else
            {
                return STL_FALSE;
            }
        }
    }
    else
    {
        return STL_FALSE;
    }
}

/**
 * @brief IS LESS THAN 연산 ( BINARY  )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanFixedLengthBinary_STATIC( dtlBinaryType   aLeftVal,
                                                      stlInt64        aLeftLen,
                                                      dtlBinaryType   aRightVal,
                                                      stlInt64        aRightLen )
{
    stlInt64    sCompLen;
    stlInt64    sDiffLen;
    stlChar   * sTmpPtr;
    stlInt32    sCompResult = 0;    

    if( (sDiffLen = (aLeftLen - aRightLen)) > 0 )
    {
        sCompLen = aRightLen;
    }
    else
    {
        sCompLen = aLeftLen;
    }
 
    sCompResult = stlMemcmp(aLeftVal, aRightVal, sCompLen);
        
    if( (sDiffLen < 0) && (sCompResult == 0) )
    {
        sTmpPtr = aRightVal + sCompLen;
        sCompLen = -sDiffLen;

        while( (sCompLen > 0) && (*sTmpPtr == 0x00) )
        {
            ++sTmpPtr;
            --sCompLen;
        }
            
        if( sCompLen == 0 )
        {
            return STL_FALSE;
        }
        else
        {
            return STL_TRUE;
        }                
    }
    else
    {
        return ( sCompResult < 0 );
    }
}

/**
 * @brief IS LESS THAN EQUAL 연산 ( BINARY  )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanEqualFixedLengthBinary_STATIC( dtlBinaryType   aLeftVal,
                                                           stlInt64        aLeftLen,
                                                           dtlBinaryType   aRightVal,
                                                           stlInt64        aRightLen )
{
    stlInt64    sCompLen;
    stlInt64    sDiffLen;
    stlChar   * sTmpPtr;
    stlInt32    sCompResult = 0;    

    if( (sDiffLen = (aLeftLen - aRightLen)) > 0 )
    {
        sCompLen = aRightLen;
    }
    else
    {
        sCompLen = aLeftLen;
    }
 
    sCompResult = stlMemcmp(aLeftVal, aRightVal, sCompLen);
        
    if( (sDiffLen != 0) && (sCompResult == 0) )
    {
        if( sDiffLen > 0 )
        {
            sTmpPtr = aLeftVal + sCompLen;
            sCompLen = sDiffLen;

            while( (sCompLen > 0) && (*sTmpPtr == 0x00) )
            {
                ++sTmpPtr;
                --sCompLen;
            }
            
            if( sCompLen == 0 )
            {
                return STL_TRUE;
            }
            else
            {
                return STL_FALSE;
            }
        }
        else
        {
            return STL_TRUE;                
        }
    }
    else
    {
        return ( sCompResult <= 0 );
    }
}

/*******************************************************************************
 * INLINE 함수
 ******************************************************************************/

inline stlBool dtlIsEqualFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                   stlInt64       aLeftLen,
                                                   dtlBinaryType  aRightVal,
                                                   stlInt64       aRightLen )
{
    return dtlIsEqualFixedLengthBinary_STATIC( aLeftVal,
                                               aLeftLen,
                                               aRightVal,
                                               aRightLen );
}

inline stlBool dtlIsNotEqualFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                      stlInt64       aLeftLen,
                                                      dtlBinaryType  aRightVal,
                                                      stlInt64       aRightLen )
{
    return !dtlIsEqualFixedLengthBinary_STATIC( aLeftVal,
                                                aLeftLen,
                                                aRightVal,
                                                aRightLen );
}

inline stlBool dtlIsLessThanFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                      stlInt64       aLeftLen,
                                                      dtlBinaryType  aRightVal,
                                                      stlInt64       aRightLen )
{
    return dtlIsLessThanFixedLengthBinary_STATIC( aLeftVal,
                                                  aLeftLen,
                                                  aRightVal,
                                                  aRightLen );
}

inline stlBool dtlIsLessThanEqualFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                           stlInt64       aLeftLen,
                                                           dtlBinaryType  aRightVal,
                                                           stlInt64       aRightLen )
{
    return dtlIsLessThanEqualFixedLengthBinary_STATIC( aLeftVal,
                                                       aLeftLen,
                                                       aRightVal,
                                                       aRightLen );
}


inline stlBool dtlIsGreaterThanFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                         stlInt64       aLeftLen,
                                                         dtlBinaryType  aRightVal,
                                                         stlInt64       aRightLen )
{
    return dtlIsLessThanFixedLengthBinary_STATIC( aRightVal,
                                                  aRightLen,
                                                  aLeftVal,
                                                  aLeftLen );
}

inline stlBool dtlIsGreaterThanEqualFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                              stlInt64       aLeftLen,
                                                              dtlBinaryType  aRightVal,
                                                              stlInt64       aRightLen )
{
    return dtlIsLessThanEqualFixedLengthBinary_STATIC( aRightVal,
                                                       aRightLen,
                                                       aLeftVal,
                                                       aLeftLen );
}

/** @} dtlBinaryStringType */

/**
 * @addtogroup dtlNumericType
 * @{
 */

/*******************************************************************************
 * STATIC 함수
 ******************************************************************************/

/**
 * @brief IS EQUAL 연산 ( NUMERIC TO SMALLINT )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsEqualNumericToSmallInt_STATIC( dtlNumericType  * aLeftVal,
                                                   stlInt64          aLeftLen,
                                                   dtlSmallIntType   aRightVal,
                                                   stlInt64          aRightLen )
{
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlInt64            sTempValue;
    stlUInt8          * sDigit;

    if( DTL_NUMERIC_IS_POSITIVE( aLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( aRightVal < 0 )
        {
            return STL_FALSE;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aLeftVal, aLeftLen ) )
        {
            return ( aRightVal == 0 );
        }

        if( aRightVal == 0 )
        {
            return STL_FALSE;
        }

        /* small int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aLeftVal );
        if( sNumExponent < 0 || sNumExponent > 2 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        ++sNumExponent;
        if( sNumLength > sNumExponent )
        {
            return STL_FALSE;
        }
        else
        {
            sTempValue = 0;
            sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );

            /* numeric을 UInt64로 conversion하여 비교 */
            if( sNumLength == sNumExponent )
            {
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
            }
            else
            {
                sNumExponent -= sNumLength;                
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sNumExponent * 2 ];
            }
            
            return ( sTempValue == aRightVal );
        }
    }
    else
    {
        /*
         * 부호가 다른지 검사
         * zero 검사도 같이 수행됨.
         */
        if( aRightVal >= 0 )
        {
            return STL_FALSE;
        }

        /* /\* zero 검사 *\/ */
        /* if( aRightVal == 0 ) */
        /* { */
        /*     return STL_FALSE; */
        /* } */

        /* small int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aLeftVal );
        if( sNumExponent < 0 || sNumExponent > 2 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        ++sNumExponent;
        if( sNumLength > sNumExponent )
        {
            return STL_FALSE;
        }
        else
        {
            sTempValue = 0;
            sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
            
            /* numeric을 UInt64로 conversion하여 비교 */
            if( sNumLength == sNumExponent )
            {
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
            }
            else
            {
                sNumExponent -= sNumLength;                                
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sNumExponent * 2 ];
            }

            return ( -(sTempValue) == aRightVal );
        }
    }
}

/**
 * @brief IS EQUAL 연산 ( NUMERIC TO INTEGER )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsEqualNumericToInteger_STATIC( dtlNumericType  * aLeftVal,
                                                  stlInt64          aLeftLen,
                                                  dtlIntegerType    aRightVal,
                                                  stlInt64          aRightLen )
{
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlInt64            sTempValue;
    stlUInt8          * sDigit;

    if( DTL_NUMERIC_IS_POSITIVE( aLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( aRightVal < 0 )
        {
            return STL_FALSE;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aLeftVal, aLeftLen ) )
        {
            return ( aRightVal == 0 );
        }

        if( aRightVal == 0 )
        {
            return STL_FALSE;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aLeftVal );
        if( sNumExponent < 0 || sNumExponent > 4 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        ++sNumExponent;
        if( sNumLength > sNumExponent )
        {
            return STL_FALSE;
        }
        else
        {
            sTempValue = 0;
            sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );

            if( ( aRightVal < gPreDefinedPow[ ( sNumExponent - 1 ) * 2 ] ) ||
                ( aRightVal >= gPreDefinedPow[ sNumExponent * 2 ] ) )
            {
                return STL_FALSE;
            }

            /* numeric을 UInt64로 conversion하여 비교 */
            if( sNumLength == sNumExponent )
            {
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
            }
            else
            {
                sNumExponent -= sNumLength;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sNumExponent * 2 ];
            }
            
            return ( sTempValue == aRightVal );
        }
    }
    else
    {
        /*
         * 부호가 다른지 검사
         * zero 검사도 같이 수행됨.
         */
        if( aRightVal >= 0 )
        {
            return STL_FALSE;
        }

        /* /\* zero 검사 *\/ */
        /* if( aRightVal == 0 ) */
        /* { */
        /*     return STL_FALSE; */
        /* } */

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aLeftVal );
        if( sNumExponent < 0 || sNumExponent > 4 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        ++sNumExponent;
        if( sNumLength > sNumExponent )
        {
            return STL_FALSE;
        }
        else
        {
            sTempValue = 0;
            sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );

            if( ( aRightVal > -(stlInt64)gPreDefinedPow[ ( sNumExponent - 1 ) * 2 ] ) ||
                ( aRightVal <= -(stlInt64)gPreDefinedPow[ sNumExponent * 2 ] ) )
            {
                return STL_FALSE;
            }

            /* numeric을 UInt64로 conversion하여 비교 */
            if( sNumLength == sNumExponent )
            {
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
            }
            else
            {
                sNumExponent -= sNumLength;                
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sNumExponent * 2 ];
            }

            return ( -(sTempValue) == aRightVal );
        }
    }
}

/**
 * @brief IS EQUAL 연산 ( NUMERIC TO BIGINT )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsEqualNumericToBigInt_STATIC( dtlNumericType  * aLeftVal,
                                                 stlInt64          aLeftLen,
                                                 dtlBigIntType     aRightVal,
                                                 stlInt64          aRightLen )
{
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlUInt64           sTempValue;
    stlUInt64           sTempRightValue;
    stlUInt8          * sDigit;

    if( DTL_NUMERIC_IS_POSITIVE( aLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( aRightVal < 0 )
        {
            return STL_FALSE;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aLeftVal, aLeftLen ) )
        {
            return ( aRightVal == 0 );
        }

        if( aRightVal == 0 )
        {
            return STL_FALSE;
        }

        /* big int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aLeftVal );
        if( sNumExponent < 0 || sNumExponent > 9 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        ++sNumExponent;
        if( sNumLength > sNumExponent )
        {
            return STL_FALSE;
        }
        else
        {
            sTempValue = 0;
            sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );

            if( ( aRightVal < gPreDefinedPow[ ( sNumExponent - 1 ) * 2 ] ) ||
                ( aRightVal >= gPreDefinedPow[ ( sNumExponent - 1 ) * 2 ] * 100.0 ) )
            {
                return STL_FALSE;
            }

            /* numeric을 UInt64로 conversion하여 비교 */
            if( sNumLength == sNumExponent )
            {
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
            }
            else
            {
                sNumExponent -= sNumLength;                
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sNumExponent * 2 ];
            }
            
            return ( sTempValue == aRightVal );
        }
    }
    else
    {
        /*
         * 부호가 다른지 검사
         * zero 검사도 같이 수행됨.
         */
        if( aRightVal >= 0 )
        {
            return STL_FALSE;
        }
        
        /* /\* zero 검사 *\/ */
        /* if( aRightVal == 0 ) */
        /* { */
        /*     return STL_FALSE; */
        /* } */
        
        /* big int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aLeftVal );
        if( sNumExponent < 0 || sNumExponent > 9 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        ++sNumExponent;
        if( sNumLength > sNumExponent )
        {
            return STL_FALSE;
        }
        else
        {
            sTempValue = 0;
            sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
            
            if( ( aRightVal > -(stlInt64)gPreDefinedPow[ ( sNumExponent - 1 ) * 2 ] ) ||
                ( aRightVal <= (stlInt64)gPreDefinedPow[ ( sNumExponent - 1 ) * 2 ] * -100.0 ) )
            {
                return STL_FALSE;
            }

            /* numeric을 UInt64로 conversion하여 비교 */
            if( sNumLength == sNumExponent )
            {
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
            }
            else
            {
                sNumExponent -= sNumLength;                
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sNumExponent * 2 ];
            }

            /*
             * sTempValue가 Int64 범위를 넘어갈 수 있으므로, UInt64로 계산한다.
             * aRightVal을 양수로 계산해서 비교함.
             * ( 이 단계에서의 aRightVal은 음수값임 )
             */ 
            
            if( aRightVal == DTL_NATIVE_BIGINT_MIN )
            {
                sTempRightValue = (stlUInt64)(-(aRightVal + 1));
                sTempRightValue += 1;
                
                STL_DASSERT( sTempRightValue == ((stlUInt64)DTL_NATIVE_BIGINT_MAX + (stlUInt64)1) );
            }
            else
            {
                sTempRightValue = (stlUInt64)(-(aRightVal));
            }

            return ( sTempValue == sTempRightValue );
        }
    }
}

/**
 * @brief IS LESS THAN 연산 ( SMALLINT TO NUMERIC )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanSmallIntToNumeric_STATIC( dtlSmallIntType   aLeftVal,
                                                      stlInt64          aLeftLen,
                                                      dtlNumericType  * aRightVal,
                                                      stlInt64          aRightLen )
{
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlInt64            sTempValue;
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    if( DTL_NUMERIC_IS_POSITIVE( aRightVal ) )
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal < 0 )
        {
            return STL_TRUE;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aRightVal, aRightLen  ) )
        {
            return STL_FALSE;
        }

        if( aLeftVal == 0 )
        {
            return STL_TRUE;
        }

        /* small int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aRightVal );
        if( sNumExponent > 2 )
        {
            return STL_TRUE;
        }

        if( sNumExponent < 0 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            return ( aLeftVal < sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( aLeftVal <= sTempValue );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( aLeftVal < sTempValue );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal >= 0 )
        {
            return STL_FALSE;
        }
        
        /* small int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aRightVal );
        if( sNumExponent > 2 )
        {
            return STL_FALSE;
        }

        if( sNumExponent < 0 )
        {
            return STL_TRUE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;
        
        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            return ( aLeftVal < -(sTempValue) );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( aLeftVal < -(sTempValue) );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( aLeftVal < -(sTempValue) );
            }
        }
    }
}    

/**
 * @brief IS LESS THAN 연산 ( INTEGER VS NUMERIC )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanIntegerToNumeric_STATIC( dtlIntegerType    aLeftVal,
                                                     stlInt64          aLeftLen,
                                                     dtlNumericType  * aRightVal,
                                                     stlInt64          aRightLen )
{
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlInt64            sTempValue;
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    if( DTL_NUMERIC_IS_POSITIVE( aRightVal ) )
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal < 0 )
        {
            return STL_TRUE;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aRightVal, aRightLen ) )
        {
            return STL_FALSE;
        }

        if( aLeftVal == 0 )
        {
            return STL_TRUE;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aRightVal );
        if( sNumExponent > 4 )
        {
            return STL_TRUE;
        }

        if( sNumExponent < 0 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            return ( aLeftVal < sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( aLeftVal <= sTempValue );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( aLeftVal < sTempValue );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal >= 0 )
        {
            return STL_FALSE;
        }
        
        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aRightVal );
        if( sNumExponent > 4 )
        {
            return STL_FALSE;
        }

        if( sNumExponent < 0 )
        {
            return STL_TRUE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;
        
        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            return ( aLeftVal < -(sTempValue) );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( aLeftVal < -(sTempValue) );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( aLeftVal < -(sTempValue) );
            }
        }
    }
}

/**
 * @brief IS LESS THAN 연산 ( BIGINT NUMERIC )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanBigIntToNumeric_STATIC( dtlBigIntType     aLeftVal,
                                                    stlInt64          aLeftLen,
                                                    dtlNumericType  * aRightVal,
                                                    stlInt64          aRightLen )
{
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlUInt64           sTempValue;
    stlUInt64           sTempLeftValue;    
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    if( DTL_NUMERIC_IS_POSITIVE( aRightVal ) )
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal < 0 )
        {
            return STL_TRUE;
        }
        
        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aRightVal, aRightLen ) )
        {
            return STL_FALSE;
        }

        if( aLeftVal == 0 )
        {
            return STL_TRUE;
        }

        /* big int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aRightVal );
        if( sNumExponent > 9 )
        {
            return STL_TRUE;
        }

        if( sNumExponent < 0 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;

        if( aLeftVal < gPreDefinedPow[ ( sNumExponent - 1 ) * 2 ] )
        {
            return STL_TRUE;
        }
        
        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            return ( aLeftVal < sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( aLeftVal <= sTempValue );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( aLeftVal < sTempValue );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal >= 0 )
        {
            return STL_FALSE;
        }
        
        /* big int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aRightVal );
        if( sNumExponent > 9 )
        {
            return STL_FALSE;
        }

        if( sNumExponent < 0 )
        {
            return STL_TRUE;
        }

        /*
         * sTempValue가 Int64 범위를 넘어갈 수 있으므로, UInt64로 계산한다.
         * aLeftVal을 양수로 계산해서 비교함.
         * ( 이 단계에서의 aLeftVal은 음수값임 )
         */

        if( aLeftVal == DTL_NATIVE_BIGINT_MIN )
        {
            sTempLeftValue = (stlUInt64)(-(aLeftVal + 1));
            sTempLeftValue += 1;
                
            STL_DASSERT( sTempLeftValue == ((stlUInt64)DTL_NATIVE_BIGINT_MAX + (stlUInt64)1) );
        }
        else
        {
            sTempLeftValue = (stlUInt64)(-(aLeftVal));
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;
        
        if (aLeftVal > -(stlInt64)gPreDefinedPow[(sNumExponent - 1) * 2])
        {
            return STL_FALSE;
        }

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            /*
             * 양수로 계산해서 비교
             * 예) -1212 < -1200 => true 이므로
             *     1212 > 1200 로 비교해서 true 결과를 낸다.
             */            
            return ( sTempLeftValue > sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( sTempLeftValue > sTempValue );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( sTempLeftValue > sTempValue );
            }
        }
    }
}

/**
 * @brief IS LESS THAN EQUAL 연산 ( SMALLINT TO NUMERIC )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanEqualSmallIntToNumeric_STATIC( dtlSmallIntType   aLeftVal,
                                                           stlInt64          aLeftLen,
                                                           dtlNumericType  * aRightVal,
                                                           stlInt64          aRightLen )
{
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlInt64            sTempValue;
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    if( DTL_NUMERIC_IS_POSITIVE( aRightVal ) )
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal < 0 )
        {
            return STL_TRUE;
        }
        
        /* zero 검사 */
        if( aLeftVal == 0 )
        {
            return STL_TRUE;
        }

        if( DTL_NUMERIC_IS_ZERO( aRightVal, aRightLen ) )
        {
            return STL_FALSE;
        }

        /* small int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aRightVal );
        if( sNumExponent > 2 )
        {
            return STL_TRUE;
        }

        if( sNumExponent < 0 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            return ( aLeftVal <= sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( aLeftVal <= sTempValue );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( aLeftVal <= sTempValue );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal >= 0 )
        {
            return STL_FALSE;
        }
        
        /* small int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aRightVal );
        if( sNumExponent > 2 )
        {
            return STL_FALSE;
        }

        if( sNumExponent < 0 )
        {
            return STL_TRUE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;
        
        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            return ( aLeftVal <= -(sTempValue) );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( aLeftVal < -(sTempValue) );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( aLeftVal <= -(sTempValue) );
            }
        }
    }
}

/**
 * @brief IS LESS THAN EQUAL 연산 ( INTEGER TO NUMERIC )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanEqualIntegerToNumeric_STATIC( dtlIntegerType    aLeftVal,
                                                          stlInt64          aLeftLen,
                                                          dtlNumericType  * aRightVal,
                                                          stlInt64          aRightLen )
{
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlInt64            sTempValue;
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    if( DTL_NUMERIC_IS_POSITIVE( aRightVal ) )
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal < 0 )
        {
            return STL_TRUE;
        }
        
        /* zero 검사 */
        if( aLeftVal == 0 )
        {
            return STL_TRUE;
        }

        if( DTL_NUMERIC_IS_ZERO( aRightVal, aRightLen ) )
        {
            return STL_FALSE;
        }

        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aRightVal );
        if( sNumExponent > 4 )
        {
            return STL_TRUE;
        }

        if( sNumExponent < 0 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            return ( aLeftVal <= sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( aLeftVal <= sTempValue );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( aLeftVal <= sTempValue );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal >= 0 )
        {
            return STL_FALSE;
        }
        
        /* integer 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aRightVal );
        if( sNumExponent > 4 )
        {
            return STL_FALSE;
        }

        if( sNumExponent < 0 )
        {
            return STL_TRUE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;
        
        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            return ( aLeftVal <= -(sTempValue) );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( aLeftVal < -(sTempValue) );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( aLeftVal <= -(sTempValue) );
            }
        }
    }
}

/**
 * @brief IS LESS THAN EQUAL 연산 ( BIGINT TO NUMERIC )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanEqualBigIntToNumeric_STATIC( dtlBigIntType     aLeftVal,
                                                         stlInt64          aLeftLen,
                                                         dtlNumericType  * aRightVal,
                                                         stlInt64          aRightLen )
{
    stlInt64            sNumExponent;
    stlInt16            sNumLength;
    stlUInt64           sTempValue;
    stlUInt64           sTempLeftValue;        
    stlInt64            sDiffExpo;
    stlUInt8          * sDigit;

    if( DTL_NUMERIC_IS_POSITIVE( aRightVal ) )
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal < 0 )
        {
            return STL_TRUE;
        }
        
        /* zero 검사 */
        if( aLeftVal == 0 )
        {
            return STL_TRUE;
        }

        if( DTL_NUMERIC_IS_ZERO( aRightVal, aRightLen ) )
        {
            return STL_FALSE;
        }

        /* big int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aRightVal );
        if( sNumExponent > 9 )
        {
            return STL_TRUE;
        }

        if( sNumExponent < 0 )
        {
            return STL_FALSE;
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;

        if( aLeftVal < gPreDefinedPow[ ( sNumExponent - 1 ) * 2 ] )
        {
            return STL_TRUE;
        }

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            return ( aLeftVal <= sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( aLeftVal <= sTempValue );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( aLeftVal <= sTempValue );
            }
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( aLeftVal >= 0 )
        {
            return STL_FALSE;
        }
        
        /* big int 범위 체크 */
        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aRightVal );
        if( sNumExponent > 9 )
        {
            return STL_FALSE;
        }

        if( sNumExponent < 0 )
        {
            return STL_TRUE;
        }

        /*
         * sTempValue가 Int64 범위를 넘어갈 수 있으므로, UInt64로 계산한다.
         * aLeftVal을 양수로 계산해서 비교함.
         * ( 이 단계에서의 aLeftVal은 음수값임 )
         */

        if( aLeftVal == DTL_NATIVE_BIGINT_MIN )
        {
            sTempLeftValue = (stlUInt64)(-(aLeftVal + 1));
            sTempLeftValue += 1;
                
            STL_DASSERT( sTempLeftValue == ((stlUInt64)DTL_NATIVE_BIGINT_MAX + (stlUInt64)1) );
        }
        else
        {
            sTempLeftValue = (stlUInt64)(-(aLeftVal));
        }

        /* numeric이 소수점 자리를 가지는지 검사 */
        sTempValue = 0;
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aRightLen );
        sDigit     = DTL_NUMERIC_GET_DIGIT_PTR( aRightVal );
        ++sNumExponent;
        
        if (aLeftVal > -(stlInt64)gPreDefinedPow[(sNumExponent - 1) * 2])
        {
            return STL_FALSE;
        }

        if( sNumLength == sNumExponent )
        {
            /* 소수점 자리 없음 */
            while( sNumLength )
            {
                sTempValue *= 100;
                sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                ++sDigit;
                --sNumLength;
            }
            /*
             * 양수로 계산해서 비교
             * 예) -1212 <= -1200 => true 이므로
             *     1212 >= 1200 로 비교해서 true 결과를 낸다.
             */                                    
            return ( sTempLeftValue >= sTempValue );
        }
        else
        {
            if( sNumLength > sNumExponent )
            {
                /* 소수점 자리 존재 */
                while( sNumExponent )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    sNumExponent--;
                }
                return ( sTempLeftValue > sTempValue );
            }
            else
            {
                /* 소수점 자리 없음 */
                sDiffExpo  = ( sNumExponent - sNumLength ) * 2;
                while( sNumLength )
                {
                    sTempValue *= 100;
                    sTempValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                    ++sDigit;
                    --sNumLength;
                }
                sTempValue *= gPreDefinedPow[ sDiffExpo ];
                return ( sTempLeftValue >= sTempValue );
            }
        }
    }
}

/**
 * @brief IS EQUAL 연산 ( NUMERIC TO REAL )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsEqualNumericToReal_STATIC( dtlNumericType  * aLeftVal,
                                               stlInt64          aLeftLen,
                                               dtlRealType       aRightVal,
                                               stlInt64          aRightLen )
{
    dtlRealType         sReal = aRightVal;
    stlInt16            sNumLength;
    stlUInt8          * sDigit;

    stlInt32            sMaxExponent;
    stlInt32            sNumExponent;
    stlInt32            sRoundExponent;
    stlInt64            sLeftInt;
    stlInt64            sRightInt;


    /* 유효 숫자 체크 */
    sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
    if( sNumLength > 4 )
    {
        return STL_FALSE;
    }

    if( DTL_NUMERIC_IS_POSITIVE( aLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sReal < 0.0 )
        {
            return STL_FALSE;
        }

        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aLeftVal, aLeftLen ) )
        {
            return ( sReal == 0.0 );
        }
        
        if( sReal == 0.0 )
        {
            return STL_FALSE;
        }

        /* real 범위 체크 */
        if( sReal < 1.0 )
        {
            sMaxExponent = stlLog10f( sReal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10f( sReal );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aLeftVal );
        if( ( sMaxExponent < sNumExponent * 2 ) || ( sMaxExponent >= ++sNumExponent * 2 ) )
        {
            return STL_FALSE;
        }

        sRoundExponent = STL_FLT_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 100;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;
        sNumExponent = 4 - sNumLength;

        while( sNumLength )
        {
            sLeftInt *= 100;
            sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
            sDigit++;
            sNumLength--;
        }
        sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

        return ( sLeftInt == sRightInt );
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sReal >= 0.0 )
        {
            return STL_FALSE;
        }

        /* real 범위 체크 */
        if( sReal > -1.0 )
        {
            sMaxExponent = stlLog10f( -sReal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10f( -sReal );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aLeftVal );
        if( ( sMaxExponent < sNumExponent * 2 ) || ( sMaxExponent >= ++sNumExponent * 2 ) )
        {
            return STL_FALSE;
        }

        sRoundExponent = STL_FLT_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 100;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;
        sNumExponent = 4 - sNumLength;

        while( sNumLength )
        {
            sLeftInt *= 100;
            sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
            sDigit++;
            sNumLength--;
        }
        sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

        return ( sLeftInt == sRightInt );
    }
}

/**
 * @brief IS LESS THAN 연산 ( NUMERIC TO REAL )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanNumericToReal_STATIC( dtlNumericType  * aLeftVal,
                                                  stlInt64          aLeftLen,
                                                  dtlRealType       aRightVal,
                                                  stlInt64          aRightLen )
{
    dtlRealType         sReal = aRightVal;
    stlInt16            sNumLength;
    stlUInt8          * sDigit;

    stlInt32            sMaxExponent;
    stlInt32            sNumExponent;
    stlInt32            sRoundExponent;
    stlInt64            sLeftInt;
    stlInt64            sRightInt;


    if( DTL_NUMERIC_IS_POSITIVE( aLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sReal <= 0.0 )
        {
            return STL_FALSE;
        }

        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aLeftVal, aLeftLen ) )
        {
            return STL_TRUE;
        }

        /* real 범위 체크 */
        if( sReal < 1.0 )
        {
            sMaxExponent = stlLog10f( sReal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10f( sReal );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aLeftVal );
        if( sMaxExponent < sNumExponent * 2 ) 
        {
            return STL_FALSE;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return STL_TRUE;
        }

        sRoundExponent = STL_FLT_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 100;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;

        if( sNumLength > 4 )
        {
            sNumLength = 4;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            return ( sLeftInt < sRightInt );
        }
        else
        {
            sNumExponent = 4 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            return ( sLeftInt < sRightInt );
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sReal >= 0.0 )
        {
            return STL_TRUE;
        }

        /* real 범위 체크 */
        if( sReal > -1.0 )
        {
            sMaxExponent = stlLog10f( -sReal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10f( -sReal );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aLeftVal );
        if( sMaxExponent < sNumExponent * 2 ) 
        {
            return STL_TRUE;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return STL_FALSE;
        }

        sRoundExponent = STL_FLT_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 100;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;

        if( sNumLength > 4 )
        {
            sNumLength = 4;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            return ( sLeftInt <= sRightInt );
        }
        else
        {
            sNumExponent = 4 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            return ( sLeftInt < sRightInt );
        }
    }
}

/**
 * @brief IS LESS THAN EQUAL 연산 ( NUMERIC TO REAL )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanEqualNumericToReal_STATIC( dtlNumericType  * aLeftVal,
                                                       stlInt64          aLeftLen,
                                                       dtlRealType       aRightVal,
                                                       stlInt64          aRightLen )
{
    dtlRealType         sReal = aRightVal;
    stlInt16            sNumLength;
    stlUInt8          * sDigit;

    stlInt32            sMaxExponent;
    stlInt32            sNumExponent;
    stlInt32            sRoundExponent;
    stlInt64            sLeftInt;
    stlInt64            sRightInt;


    if( DTL_NUMERIC_IS_POSITIVE( aLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sReal < 0.0 )
        {
            return STL_FALSE;
        }

        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aLeftVal, aLeftLen ) )
        {
            return STL_TRUE;
        }

        if( sReal == 0.0 )
        {
            return STL_FALSE;
        }

        /* real 범위 체크 */
        if( sReal < 1.0 )
        {
            sMaxExponent = stlLog10f( sReal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10f( sReal );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aLeftVal );
        if( sMaxExponent < sNumExponent * 2 ) 
        {
            return STL_FALSE;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return STL_TRUE;
        }

        sRoundExponent = STL_FLT_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 100;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;

        if( sNumLength > 4 )
        {
            sNumLength = 4;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            return sLeftInt < sRightInt;
        }
        else
        {
            sNumExponent = 4 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            return sLeftInt <= sRightInt;
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sReal >= 0.0 )
        {
            return STL_TRUE;
        }

        /* real 범위 체크 */
        if( sReal > -1.0 )
        {
            sMaxExponent = stlLog10f( -sReal ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10f( -sReal );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aLeftVal );
        if( sMaxExponent < sNumExponent * 2 ) 
        {
            return STL_TRUE;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return STL_FALSE;
        }

        sRoundExponent = STL_FLT_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 100;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sReal * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;

        if( sNumLength > 4 )
        {
            sNumLength = 4;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            return sLeftInt <= sRightInt;
        }
        else
        {
            sNumExponent = 4 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            return ( sLeftInt <= sRightInt );
        }
    }
}

/**
 * @brief IS EQUAL 연산 ( NUMERIC TO DOUBLE )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsEqualNumericToDouble_STATIC( dtlNumericType  * aLeftVal,
                                                 stlInt64          aLeftLen,
                                                 dtlDoubleType     aRightVal,
                                                 stlInt64          aRightLen )
{
    dtlDoubleType       sDouble = aRightVal;
    stlInt16            sNumLength;
    stlUInt8          * sDigit;

    stlInt32            sMaxExponent;
    stlInt32            sNumExponent;
    stlInt32            sRoundExponent;
    stlInt64            sLeftInt;
    stlInt64            sRightInt;


    /* 유효 숫자 체크 */
    sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
    if( sNumLength > 8 )
    {
        return STL_FALSE;
    }

    if( DTL_NUMERIC_IS_POSITIVE( aLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sDouble < 0.0 )
        {
            return STL_FALSE;
        }

        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aLeftVal, aLeftLen ) )
        {
            return ( sDouble == 0.0 );
        }
        
        if( sDouble == 0.0 )
        {
            return STL_FALSE;
        }

        /* double 범위 체크 */
        if( sDouble < 1.0 )
        {
            sMaxExponent = stlLog10( sDouble ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10( sDouble );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aLeftVal );
        if( ( sMaxExponent < sNumExponent * 2 ) || ( sMaxExponent >= ++sNumExponent * 2 ) )
        {
            return STL_FALSE;
        }

        sRoundExponent = STL_DBL_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = ( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;
        sNumExponent = 8 - sNumLength;

        while( sNumLength )
        {
            sLeftInt *= 100;
            sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
            sDigit++;
            sNumLength--;
        }
        sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

        return ( sLeftInt == sRightInt );
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sDouble >= 0.0 )
        {
            return STL_FALSE;
        }

        /* double 범위 체크 */
        if( sDouble > -1.0 )
        {
            sMaxExponent = stlLog10( -sDouble ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10( -sDouble );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aLeftVal );
        if( ( sMaxExponent < sNumExponent * 2 ) || ( sMaxExponent >= ++sNumExponent * 2 ) )
        {
            return STL_FALSE;
        }

        sRoundExponent = STL_DBL_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = ( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;
        sNumExponent = 8 - sNumLength;

        while( sNumLength )
        {
            sLeftInt *= 100;
            sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
            sDigit++;
            sNumLength--;
        }
        sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

        return ( sLeftInt == sRightInt );
    }
}

/**
 * @brief IS LESS THAN 연산 ( NUMERIC TO DOUBLE )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value
 * @param[in]  aRightLen    right value length
 * @return 비교 결과
 */
static stlBool dtlIsLessThanNumericToDouble_STATIC( dtlNumericType  * aLeftVal,
                                                    stlInt64          aLeftLen,
                                                    dtlDoubleType     aRightVal,
                                                    stlInt64          aRightLen )
{
    dtlDoubleType       sDouble = aRightVal;
    stlInt16            sNumLength;
    stlUInt8          * sDigit;

    stlInt32            sMaxExponent;
    stlInt32            sNumExponent;
    stlInt32            sRoundExponent;
    stlInt64            sLeftInt;
    stlInt64            sRightInt;


    if( DTL_NUMERIC_IS_POSITIVE( aLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sDouble <= 0.0 )
        {
            return STL_FALSE;
        }

        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aLeftVal, aLeftLen ) )
        {
            return STL_TRUE;
        }

        /* double 범위 체크 */
        if( sDouble < 1.0 )
        {
            sMaxExponent = stlLog10( sDouble ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10( sDouble );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aLeftVal );
        if( sMaxExponent < sNumExponent * 2 ) 
        {
            return STL_FALSE;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return STL_TRUE;
        }

        sRoundExponent = STL_DBL_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = ( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;

        if( sNumLength > 8 )
        {
            sNumLength = 8;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            return ( sLeftInt < sRightInt );
        }
        else
        {
            sNumExponent = 8 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            return ( sLeftInt < sRightInt );
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sDouble >= 0.0 )
        {
            return STL_TRUE;
        }

        /* double 범위 체크 */
        if( sDouble > -1.0 )
        {
            sMaxExponent = stlLog10( -sDouble ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10( -sDouble );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aLeftVal );
        if( sMaxExponent < sNumExponent * 2 ) 
        {
            return STL_TRUE;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return STL_FALSE;
        }

        sRoundExponent = STL_DBL_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = ( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;

        if( sNumLength > 8 )
        {
            sNumLength = 8;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            return ( sLeftInt <= sRightInt );
        }
        else
        {
            sNumExponent = 8 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            return ( sLeftInt < sRightInt );
        }
    }
}

/**
 * @brief IS LESS THAN EQUAL 연산 ( NUMERIC TO DOUBLE )
 *   <BR> align이 맞는 포인터가 넘어와야 함.
 * @param[in]  aLeftVal     left value 
 * @param[in]  aLeftLen     left value length
 * @param[in]  aRightVal    right value 
 * @param[in]  aRightLen    right value length
 * @return 비교 결과 
 */
static stlBool dtlIsLessThanEqualNumericToDouble_STATIC( dtlNumericType  * aLeftVal,
                                                         stlInt64          aLeftLen,
                                                         dtlDoubleType     aRightVal,
                                                         stlInt64          aRightLen )
{
    dtlDoubleType       sDouble = aRightVal;
    stlInt16            sNumLength;
    stlUInt8          * sDigit;

    stlInt32            sMaxExponent;
    stlInt32            sNumExponent;
    stlInt32            sRoundExponent;
    stlInt64            sLeftInt;
    stlInt64            sRightInt;


    if( DTL_NUMERIC_IS_POSITIVE( aLeftVal ) )
    {
        /* 부호가 다른지 검사 */
        if( sDouble < 0.0 )
        {
            return STL_FALSE;
        }

        /* zero 검사 */
        if( DTL_NUMERIC_IS_ZERO( aLeftVal, aLeftLen ) )
        {
            return STL_TRUE;
        }

        if( sDouble == 0.0 )
        {
            return STL_FALSE;
        }

        /* double 범위 체크 */
        if( sDouble < 1.0 )
        {
            sMaxExponent = stlLog10( sDouble ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10( sDouble );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( aLeftVal );
        if( sMaxExponent < sNumExponent * 2 ) 
        {
            return STL_FALSE;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return STL_TRUE;
        }

        sRoundExponent = STL_DBL_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = ( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) + 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;

        if( sNumLength > 8 )
        {
            sNumLength = 8;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            return sLeftInt < sRightInt;
        }
        else
        {
            sNumExponent = 8 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            return sLeftInt <= sRightInt;
        }
    }
    else
    {
        /* 부호가 다른지 검사 */
        if( sDouble >= 0.0 )
        {
            return STL_TRUE;
        }

        /* double 범위 체크 */
        if( sDouble > -1.0 )
        {
            sMaxExponent = stlLog10( -sDouble ) - 1;
        }
        else
        {
            sMaxExponent = stlLog10( -sDouble );
        }

        sNumExponent = DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( aLeftVal );
        if( sMaxExponent < sNumExponent * 2 ) 
        {
            return STL_TRUE;
        }
        if( sMaxExponent >= ++sNumExponent * 2 )
        {
            return STL_FALSE;
        }

        sRoundExponent = STL_DBL_DIG - sMaxExponent;
        if( sRoundExponent & 0x01 )
        {
            sRightInt = ( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10;
        }
        else
        {
            sRightInt = (( ((stlInt64) ( sDouble * gPredefined10PowValue[ sRoundExponent ] )) - 5 ) / 10 ) * 10;
        }

        sDigit = DTL_NUMERIC_GET_DIGIT_PTR( aLeftVal );
        sNumLength = DTL_NUMERIC_GET_DIGIT_COUNT( aLeftLen );
        sLeftInt = 0;

        if( sNumLength > 8 )
        {
            sNumLength = 8;
            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            return sLeftInt <= sRightInt;
        }
        else
        {
            sNumExponent = 8 - sNumLength;

            while( sNumLength )
            {
                sLeftInt *= 100;
                sLeftInt -= DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit );
                sDigit++;
                sNumLength--;
            }
            sLeftInt *= gPreDefinedPow[ sNumExponent * 2 ];

            return ( sLeftInt <= sRightInt );
        }
    }
}


/*******************************************************************************
 * INLINE 함수
 ******************************************************************************/

/* IS EQUAL */

inline stlBool dtlIsEqualNumericToSmallInt_INLINE( dtlNumericType  * aLeftVal,
                                                   stlInt64          aLeftLen,
                                                   dtlSmallIntType   aRightVal,
                                                   stlInt64          aRightLen )
{
    return dtlIsEqualNumericToSmallInt_STATIC( aLeftVal,
                                               aLeftLen,
                                               aRightVal,
                                               aRightLen );
}

inline stlBool dtlIsEqualNumericToInteger_INLINE( dtlNumericType  * aLeftVal,
                                                  stlInt64          aLeftLen,
                                                  dtlIntegerType    aRightVal,
                                                  stlInt64          aRightLen )
{
    return dtlIsEqualNumericToInteger_STATIC( aLeftVal,
                                              aLeftLen,
                                              aRightVal,
                                              aRightLen );
}

inline stlBool dtlIsEqualNumericToBigInt_INLINE( dtlNumericType  * aLeftVal,
                                                 stlInt64          aLeftLen,
                                                 dtlBigIntType     aRightVal,
                                                 stlInt64          aRightLen )
{
    return dtlIsEqualNumericToBigInt_STATIC( aLeftVal,
                                             aLeftLen,
                                             aRightVal,
                                             aRightLen );
}

/* IS NOT EQUAL */
inline stlBool dtlIsNotEqualNumericToSmallInt_INLINE( dtlNumericType  * aLeftVal,
                                                      stlInt64          aLeftLen,
                                                      dtlSmallIntType   aRightVal,
                                                      stlInt64          aRightLen )
{
    return !dtlIsEqualNumericToSmallInt_STATIC( aLeftVal,
                                                aLeftLen,
                                                aRightVal,
                                                aRightLen );
}

inline stlBool dtlIsNotEqualNumericToInteger_INLINE( dtlNumericType  * aLeftVal,
                                                     stlInt64          aLeftLen,
                                                     dtlIntegerType    aRightVal,
                                                     stlInt64          aRightLen )
{
    return !dtlIsEqualNumericToInteger_STATIC( aLeftVal,
                                               aLeftLen,
                                               aRightVal,
                                               aRightLen );
}

inline stlBool dtlIsNotEqualNumericToBigInt_INLINE( dtlNumericType  * aLeftVal,
                                                    stlInt64          aLeftLen,
                                                    dtlBigIntType     aRightVal,
                                                    stlInt64          aRightLen )
{
    return !dtlIsEqualNumericToBigInt_STATIC( aLeftVal,
                                              aLeftLen,
                                              aRightVal,
                                              aRightLen );
}

/* IS LESS THAN */
inline stlBool dtlIsLessThanSmallIntToNumeric_INLINE( dtlSmallIntType   aLeftVal,
                                                      stlInt64          aLeftLen,
                                                      dtlNumericType  * aRightVal,
                                                      stlInt64          aRightLen )
{
    return dtlIsLessThanSmallIntToNumeric_STATIC( aLeftVal,
                                                  aLeftLen,
                                                  aRightVal,
                                                  aRightLen );
}

inline stlBool dtlIsLessThanIntegerToNumeric_INLINE( dtlIntegerType    aLeftVal,
                                                     stlInt64          aLeftLen,
                                                     dtlNumericType  * aRightVal,
                                                     stlInt64          aRightLen )
{
    return dtlIsLessThanIntegerToNumeric_STATIC( aLeftVal,
                                                 aLeftLen,
                                                 aRightVal,
                                                 aRightLen );
}

inline stlBool dtlIsLessThanBigIntToNumeric_INLINE( dtlBigIntType     aLeftVal,
                                                    stlInt64          aLeftLen,
                                                    dtlNumericType  * aRightVal,
                                                    stlInt64          aRightLen )
{
    return dtlIsLessThanBigIntToNumeric_STATIC( aLeftVal,
                                                aLeftLen,
                                                aRightVal,
                                                aRightLen );
}

/* IS LESS THAN EQUAL */
inline stlBool dtlIsLessThanEqualSmallIntToNumeric_INLINE( dtlSmallIntType   aLeftVal,
                                                           stlInt64          aLeftLen,
                                                           dtlNumericType  * aRightVal,
                                                           stlInt64          aRightLen )
{
    return dtlIsLessThanEqualSmallIntToNumeric_STATIC( aLeftVal,
                                                       aLeftLen,
                                                       aRightVal,
                                                       aRightLen );
}

inline stlBool dtlIsLessThanEqualIntegerToNumeric_INLINE( dtlIntegerType    aLeftVal,
                                                          stlInt64          aLeftLen,
                                                          dtlNumericType  * aRightVal,
                                                          stlInt64          aRightLen )
{
    return dtlIsLessThanEqualIntegerToNumeric_STATIC( aLeftVal,
                                                      aLeftLen,
                                                      aRightVal,
                                                      aRightLen );
}

inline stlBool dtlIsLessThanEqualBigIntToNumeric_INLINE( dtlBigIntType     aLeftVal,
                                                         stlInt64          aLeftLen,
                                                         dtlNumericType  * aRightVal,
                                                         stlInt64          aRightLen )
{
    return dtlIsLessThanEqualBigIntToNumeric_STATIC( aLeftVal,
                                                     aLeftLen,
                                                     aRightVal,
                                                     aRightLen );
}

/* IS GREATER THAN */
inline stlBool dtlIsGreaterThanSmallIntToNumeric_INLINE( dtlSmallIntType   aLeftVal,
                                                         stlInt64          aLeftLen,
                                                         dtlNumericType  * aRightVal,
                                                         stlInt64          aRightLen )
{
    return !dtlIsLessThanEqualSmallIntToNumeric_STATIC( aLeftVal,
                                                        aLeftLen,
                                                        aRightVal,
                                                        aRightLen );
}

inline stlBool dtlIsGreaterThanIntegerToNumeric_INLINE( dtlIntegerType    aLeftVal,
                                                        stlInt64          aLeftLen,
                                                        dtlNumericType  * aRightVal,
                                                        stlInt64          aRightLen )
{
    return !dtlIsLessThanEqualIntegerToNumeric_STATIC( aLeftVal,
                                                       aLeftLen,
                                                       aRightVal,
                                                       aRightLen );
}

inline stlBool dtlIsGreaterThanBigIntToNumeric_INLINE( dtlBigIntType     aLeftVal,
                                                       stlInt64          aLeftLen,
                                                       dtlNumericType  * aRightVal,
                                                       stlInt64          aRightLen )
{
    return !dtlIsLessThanEqualBigIntToNumeric_STATIC( aLeftVal,
                                                      aLeftLen,
                                                      aRightVal,
                                                      aRightLen );
}

/* IS GREATER THAN EQUAL */
inline stlBool dtlIsGreaterThanEqualSmallIntToNumeric_INLINE( dtlSmallIntType   aLeftVal,
                                                              stlInt64          aLeftLen,
                                                              dtlNumericType  * aRightVal,
                                                              stlInt64          aRightLen )
{
    return !dtlIsLessThanSmallIntToNumeric_STATIC( aLeftVal,
                                                   aLeftLen,
                                                   aRightVal,
                                                   aRightLen );
}

inline stlBool dtlIsGreaterThanEqualIntegerToNumeric_INLINE( dtlIntegerType    aLeftVal,
                                                             stlInt64          aLeftLen,
                                                             dtlNumericType  * aRightVal,
                                                             stlInt64          aRightLen )
{
    return !dtlIsLessThanIntegerToNumeric_STATIC( aLeftVal,
                                                  aLeftLen,
                                                  aRightVal,
                                                  aRightLen );
}

inline stlBool dtlIsGreaterThanEqualBigIntToNumeric_INLINE( dtlBigIntType     aLeftVal,
                                                            stlInt64          aLeftLen,
                                                            dtlNumericType  * aRightVal,
                                                            stlInt64          aRightLen )
{
    return !dtlIsLessThanBigIntToNumeric_STATIC( aLeftVal,
                                                 aLeftLen,
                                                 aRightVal,
                                                 aRightLen );
}

inline stlBool dtlIsEqualNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                               stlInt64          aLeftLen,
                                               dtlRealType       aRightVal,
                                               stlInt64          aRightLen )
{
    return dtlIsEqualNumericToReal_STATIC( aLeftVal,
                                           aLeftLen,
                                           aRightVal,
                                           aRightLen );
}

inline stlBool dtlIsNotEqualNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                                  stlInt64          aLeftLen,
                                                  dtlRealType       aRightVal,
                                                  stlInt64          aRightLen )
{
    return !dtlIsEqualNumericToReal_STATIC( aLeftVal,
                                            aLeftLen,
                                            aRightVal,
                                            aRightLen );
}

inline stlBool dtlIsLessThanNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                                  stlInt64          aLeftLen,
                                                  dtlRealType       aRightVal,
                                                  stlInt64          aRightLen )
{
    return dtlIsLessThanNumericToReal_STATIC( aLeftVal,
                                              aLeftLen,
                                              aRightVal,
                                              aRightLen );
}

inline stlBool dtlIsLessThanEqualNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                                       stlInt64          aLeftLen,
                                                       dtlRealType       aRightVal,
                                                       stlInt64          aRightLen )
{
    return dtlIsLessThanEqualNumericToReal_STATIC( aLeftVal,
                                                   aLeftLen,
                                                   aRightVal,
                                                   aRightLen );
}

inline stlBool dtlIsGreaterThanNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                                     stlInt64          aLeftLen,
                                                     dtlRealType       aRightVal,
                                                     stlInt64          aRightLen )
{
    return !dtlIsLessThanEqualNumericToReal_STATIC( aLeftVal,
                                                    aLeftLen,
                                                    aRightVal,
                                                    aRightLen );
}

inline stlBool dtlIsGreaterThanEqualNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                                          stlInt64          aLeftLen,
                                                          dtlRealType       aRightVal,
                                                          stlInt64          aRightLen )
{
    return !dtlIsLessThanNumericToReal_STATIC( aLeftVal,
                                               aLeftLen,
                                               aRightVal,
                                               aRightLen );
}

inline stlBool dtlIsEqualNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                 stlInt64          aLeftLen,
                                                 dtlDoubleType     aRightVal,
                                                 stlInt64          aRightLen )
{
    return dtlIsEqualNumericToDouble_STATIC( aLeftVal,
                                             aLeftLen,
                                             aRightVal,
                                             aRightLen );
}

inline stlBool dtlIsNotEqualNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                    stlInt64          aLeftLen,
                                                    dtlDoubleType     aRightVal,
                                                    stlInt64          aRightLen )
{
    return !dtlIsEqualNumericToDouble_STATIC( aLeftVal,
                                              aLeftLen,
                                              aRightVal,
                                              aRightLen );
}

inline stlBool dtlIsLessThanNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                    stlInt64          aLeftLen,
                                                    dtlDoubleType     aRightVal,
                                                    stlInt64          aRightLen )
{
    return dtlIsLessThanNumericToDouble_STATIC( aLeftVal,
                                                aLeftLen,
                                                aRightVal,
                                                aRightLen );
}

inline stlBool dtlIsLessThanEqualNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                         stlInt64          aLeftLen,
                                                         dtlDoubleType     aRightVal,
                                                         stlInt64          aRightLen )
{
    return dtlIsLessThanEqualNumericToDouble_STATIC( aLeftVal,
                                                     aLeftLen,
                                                     aRightVal,
                                                     aRightLen );
}

inline stlBool dtlIsGreaterThanNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                       stlInt64          aLeftLen,
                                                       dtlDoubleType     aRightVal,
                                                       stlInt64          aRightLen )
{
    return !dtlIsLessThanEqualNumericToDouble_STATIC( aLeftVal,
                                                      aLeftLen,
                                                      aRightVal,
                                                      aRightLen );
}

inline stlBool dtlIsGreaterThanEqualNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                            stlInt64          aLeftLen,
                                                            dtlDoubleType     aRightVal,
                                                            stlInt64          aRightLen )
{
    return !dtlIsLessThanNumericToDouble_STATIC( aLeftVal,
                                                 aLeftLen,
                                                 aRightVal,
                                                 aRightLen );
}

/** @} dtlNumericType */
