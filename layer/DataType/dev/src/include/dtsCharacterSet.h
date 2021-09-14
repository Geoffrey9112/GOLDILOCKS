/*******************************************************************************
 * dtsCharacterSet.h
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
#ifndef _DTSCHARACTERSET_H_
#define _DTSCHARACTERSET_H_ 1

/**
 * @file dtsCharacterSet.h
 * @brief DataType Layer CharacterSet 관련 함수
 */

/**
 * @defgroup dts DataType Layer CharacterSet 관련 함수
 * @ingroup dtInternal
 * @{
 */

#define DTS_HIGH_BIT       (0x80)

#define DTS_IS_HIGH_BIT_SET(sChr) ((((stlUInt8)(sChr) & DTS_HIGH_BIT) ? STL_TRUE : STL_FALSE))

#define DTS_IS_UHC_2BYTE_HEAD(c)                                                \
    ((((stlUInt8)(c) >= 0x81) && ((stlUInt8)(c) <= 0xFE)) ? STL_TRUE : STL_FALSE) 
#define DTS_IS_UHC_2BYTE_TAIL1(c) (((stlUInt8)(c) >= 0x41) && ((stlUInt8)(c) <= 0x5A))
#define DTS_IS_UHC_2BYTE_TAIL2(c) (((stlUInt8)(c) >= 0x61) && ((stlUInt8)(c) <= 0x7A))
#define DTS_IS_UHC_2BYTE_TAIL3(c) (((stlUInt8)(c) >= 0x81) && ((stlUInt8)(c) <= 0xFE))
#define DTS_IS_UHC_2BYTE_TAIL(c)     \
    ( ( DTS_IS_UHC_2BYTE_TAIL1(c) || \
        DTS_IS_UHC_2BYTE_TAIL2(c) || \
        DTS_IS_UHC_2BYTE_TAIL3(c) ) ? STL_TRUE : STL_FALSE )

/*
 * UTF8 <-> UHC
 */
#define DTS_UTF8_UHC_HASH_BUCKET            (77377)
#define DTS_UTF8_UHC_HASH_MODULUS_CONSTANT  (0xD8D30723)
#define DTS_UTF8_UHC_HASH_MODULUS_SHIFT_BIT (16)
#define DTS_UTF8_UHC_HASH_ADJUST_CONSTANT   (0)

#define DTS_UHC_UTF8_HASH_BUCKET            (32191)
#define DTS_UHC_UTF8_HASH_MODULUS_CONSTANT  (0x824B57A1)
#define DTS_UHC_UTF8_HASH_MODULUS_SHIFT_BIT (14)
#define DTS_UHC_UTF8_HASH_ADJUST_CONSTANT   (1)

/*
 * UTF8 <-> GB18030
 */
#define DTS_UTF8_GB18030_HASH_BUCKET            (202049)
#define DTS_UTF8_GB18030_HASH_MODULUS_CONSTANT  (0xA6121DA8)
#define DTS_UTF8_GB18030_HASH_MODULUS_SHIFT_BIT (17)
#define DTS_UTF8_GB18030_HASH_ADJUST_CONSTANT   (0)

#define DTS_GB18030_UTF8_HASH_BUCKET            (380503)
#define DTS_GB18030_UTF8_HASH_MODULUS_CONSTANT  (0xB05E6ACC)
#define DTS_GB18030_UTF8_HASH_MODULUS_SHIFT_BIT (18)
#define DTS_GB18030_UTF8_HASH_ADJUST_CONSTANT   (0)

/**
 * @brief Modulus 연산없는 Modulus
 * @param[in]  aB    Bucket Count
 * @param[in]  aK    Hash Key
 * @param[in]  aC    Modulus Constant
 * @param[in]  aS    Modulus Shift Bit
 * @param[in]  aA    Adjust Constant
 */
#define DTS_HASH_BUCKET_IDX( aB, aK, aC, aS, aA )                                       \
    ( (aK) - (((((((stlUInt64)(aK)) * (aC)) >> 31) + (aA)) >> 1) >> (aS)) * (aB) )

/*******************************************************************************
 * 각 CHARACTER SET 한 문자에 대한 최대 byte length
 ******************************************************************************/ 

/**
 * @brief ascii 한 문자에 대한 최대 byte length
 */
#define DTS_GET_ASCII_MB_MAX_LENGTH   ( 1 )

/**
 * @brief utf8 한 문자에 대한 최대 byte length
 */
#define DTS_GET_UTF8_MB_MAX_LENGTH    ( 4 )

/**
 * @brief uhc 한 문자에 대한 최대 byte length
 */
#define DTS_GET_UHC_MB_MAX_LENGTH     ( 2 )

/**
 * @brief gb18030 한 문자에 대한 최대 byte length
 */
#define DTS_GET_GB18030_MB_MAX_LENGTH ( 4 )


/*******************************************************************************
 * 각 CHARACTER SET 한 문자에 대한 byte length
 ******************************************************************************/ 

/**
 * @brief  ascii 한 문자에 대한 byte length를 구한다.
 * @param[in]  aSource     aSource
 * @param[out] aMbLength   byte length
 */
#define DTS_GET_ASCII_MBLENGTH( aSource, aMbLength )         \
    ( *(aMbLength) = 1 )

/**
 * @brief  utf8 한 문자에 대한 byte length를 구한다.
 * @param[in]  aSource     aSource
 * @param[out] aMbLength   byte length 
 */
#define DTS_GET_UTF8_MBLENGTH( aSource, aMbLength )         \
    {                                                       \
        stlUInt8  sSource;                                  \
        sSource = (stlUInt8)(aSource)[0];                   \
                                                            \
        if( (sSource & 0x80) == 0 )                         \
        {                                                   \
            *(aMbLength) = 1;                               \
        }                                                   \
        else if ( (sSource & 0xe0) == 0xc0 )                \
        {                                                   \
            *(aMbLength) = 2;                               \
        }                                                   \
        else if ( (sSource & 0xf0) == 0xe0 )                \
        {                                                   \
            *(aMbLength) = 3;                               \
        }                                                   \
        else if ( (sSource & 0xf8) == 0xf0 )                \
        {                                                   \
            *(aMbLength) = 4;                               \
        }                                                   \
        else                                                \
        {                                                   \
            STL_THROW( ERROR_INVALID_CHARACTER_LENGTH );    \
        }                                                   \
    }

/**
 * @brief  uhc 한 문자에 대한 byte length를 구한다.
 * @param[in]  aSource     aSource
 * @param[out] aMbLength   byte length 
 */
#define DTS_GET_UHC_MBLENGTH( aSource, aMbLength )          \
    {                                                       \
        if( DTS_IS_UHC_2BYTE_HEAD(*(aSource)) == STL_TRUE ) \
        {                                                   \
            *(aMbLength) = 2;                               \
        }                                                   \
        else                                                \
        {                                                   \
            *(aMbLength) = 1;                               \
        }                                                   \
    }

/**
 * @brief  gb18030 한 문자에 대한 byte length를 구한다.
 * @param[in]  aSource     aSource
 * @param[out] aMbLength   byte length 
 */
#define DTS_GET_GB18030_MBLENGTH( aSource, aMbLength )                              \
    {                                                                               \
        stlUInt8 * __sSource = (stlUInt8*)(aSource);                                \
                                                                                    \
        if( DTS_IS_HIGH_BIT_SET(*(__sSource)) == STL_FALSE )                        \
        {                                                                           \
            *(aMbLength) = 1; /* ASCII */                                           \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            if( (((*(__sSource) >= 0x81)) && ((*(__sSource) <= 0xfe))) &&           \
                (((*(__sSource + 1) >= 0x40) && (*(__sSource + 1) <= 0x7e)) ||      \
                 ((*(__sSource + 1) >= 0x80) && (*(__sSource + 1) <= 0xfe))) )      \
            {                                                                       \
                *(aMbLength) = 2;                                                   \
            }                                                                       \
            else if( (((*(__sSource) >= 0x81)) && ((*(__sSource) <= 0xfe))) &&      \
                     ((*(__sSource + 1) >= 0x30) && (*(__sSource + 1) <= 0x39)) &&  \
                     ((*(__sSource + 2) >= 0x81) && (*(__sSource + 2) <= 0xfe)) &&  \
                     ((*(__sSource + 3) >= 0x30) && (*(__sSource + 3) <= 0x39)) )   \
            {                                                                       \
                *(aMbLength) = 4;                                                   \
            }                                                                       \
            else                                                                    \
            {                                                                       \
                STL_THROW( ERROR_CHARACTER_NOT_IN_REPERTOIRE );                     \
            }                                                                       \
        }                                                                           \
    }

/**
 * @brief utf16 한 문자에 대한 byte length를 구한다.
 * @param[in]  aSource     aSource
 * @param[out] aMbLength   byte length 
 */
#define DTS_GET_UTF16_MBLENGTH( aSource, aMbLength )                      \
    {                                                                     \
        if( ( (aSource)[0] >= 0xD800 ) && ( (aSource)[0] <= 0xDBFF ) )    \
        {                                                                 \
            *(aMbLength) = 4;                                             \
        }                                                                 \
        else                                                              \
        {                                                                 \
            *(aMbLength) = 2;                                             \
        }                                                                 \
    }


/*******************************************************************************
 * 각 CHARACTER SET 한 문자에 대한 Verify 여부
 ******************************************************************************/

/**
 * @brief ascii 한 문자에 대한 Verify 여부
 * @param[out] aMbLength      한 문자에 대한 byte length
 * @param[out] aVerify        Verify 여부
 */
#define DTS_IS_VERIFY_ASCII( aMbLength, aVerify )                               \
    {                                                                           \
        *(aMbLength) = 1;                                                       \
        *(aVerify) = STL_TRUE;                                                  \
    }

/**
 * @brief uhc 한 문자에 대한 Verify 여부
 * @param[in]  aSource        source str
 * @param[in]  aSourceLength  source str length
 * @param[out] aMbLength      한 문자에 대한 byte length
 * @param[out] aVerify        Verify 여부
 */
#define DTS_IS_VERIFY_UHC( aSource, aSourceLength, aMbLength, aVerify )         \
    {                                                                           \
        stlChar    sChr;                                                        \
        *(aVerify) = STL_TRUE;                                                  \
                                                                                \
        DTS_GET_UHC_MBLENGTH( (aSource), (aMbLength) );                         \
                                                                                \
        if( (aSourceLength) >= *(aMbLength) )                                   \
        {                                                                       \
            /* Do Nothing */                                                    \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            *(aVerify) = STL_FALSE;                                             \
        }                                                                       \
                                                                                \
        if( *(aVerify) == STL_TRUE )                                            \
        {                                                                       \
            if( DTS_IS_HIGH_BIT_SET( *(aSource) ) == STL_TRUE )                 \
            {                                                                   \
                /* 2byte length */                                              \
                sChr = (aSource)[0];                                            \
                                                                                \
                if( DTS_IS_UHC_2BYTE_HEAD(sChr) == STL_TRUE )                   \
                {                                                               \
                    STL_DASSERT( *(aMbLength) == 2 );                           \
                    sChr = (aSource)[1];                                        \
                                                                                \
                    if( DTS_IS_UHC_2BYTE_TAIL(sChr) == STL_TRUE )               \
                    {                                                           \
                        /* Do Nothing */                                        \
                    }                                                           \
                    else                                                        \
                    {                                                           \
                        *(aVerify) = STL_FALSE;                                 \
                    }                                                           \
                }                                                               \
                else                                                            \
                {                                                               \
                    *(aVerify) = STL_FALSE;                                     \
                }                                                               \
            }                                                                   \
            else                                                                \
            {                                                                   \
                /* 1byte length */                                              \
                /* Do Nothing */                                                \
                STL_DASSERT( *(aMbLength) == 1 );                               \
            }                                                                   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            /* Do Nothing */                                                    \
        }                                                                       \
    }

/**
 * @brief utf8 한 문자에 대한 validate
 * @param[in]  aSource      aSource
 * @param[in]  aSourceLen   aSourceLen
 * @param[out] aResult      validate결과
 */
#define DTS_UTF8_VALIDATE( aSource, aSourceLen, aResult )                       \
    {                                                                           \
        stlUInt8   sChr;                                                        \
        *(aResult) = STL_TRUE;                                                  \
                                                                                \
        switch( (aSourceLen) )                                                  \
        {                                                                       \
            default:                                                            \
                {                                                               \
                    /* length > 4면, invalid */                                 \
                    STL_DASSERT( STL_FALSE );                                   \
                    break;                                                      \
                }                                                               \
            case 4:                                                             \
                {                                                               \
                    sChr = (stlUInt8)(aSource)[3];                              \
                    if( (sChr < 0x80) || (sChr > 0xBF) )                        \
                    {                                                           \
                        *(aResult) = STL_FALSE;                                 \
                        break;                                                  \
                    }                                                           \
                    else                                                        \
                    {                                                           \
                        /* Do Nothing */                                        \
                    }                                                           \
                    /* FALL_THROUGH */                                          \
                }                                                               \
            case 3:                                                             \
                {                                                               \
                    sChr = (stlUInt8)(aSource)[2];                              \
                    if( (sChr < 0x80) || (sChr > 0xBF) )                        \
                    {                                                           \
                        *(aResult) = STL_FALSE;                                 \
                        break;                                                  \
                    }                                                           \
                    else                                                        \
                    {                                                           \
                        /* Do Nothing */                                        \
                    }                                                           \
                    /* FALL_THROUGH */                                          \
                }                                                               \
            case 2:                                                             \
                {                                                               \
                    sChr = (stlUInt8)(aSource)[1];                              \
                    switch( (stlUInt8)(*(aSource)) )                            \
                    {                                                           \
                        case 0xE0:                                              \
                            {                                                   \
                                if( (sChr < 0xA0) || (sChr > 0xBF) )            \
                                {                                               \
                                    *(aResult) = STL_FALSE;                     \
                                    break;                                      \
                                }                                               \
                                else                                            \
                                {                                               \
                                    /* Do Nothing */                            \
                                }                                               \
                                break;                                          \
                            }                                                   \
                        case 0xED:                                              \
                            {                                                   \
                                if( (sChr < 0x80) || (sChr > 0x9F) )            \
                                {                                               \
                                    *(aResult) = STL_FALSE;                     \
                                }                                               \
                                else                                            \
                                {                                               \
                                    /* Do Nothing */                            \
                                }                                               \
                                break;                                          \
                            }                                                   \
                        case 0xF0:                                              \
                            {                                                   \
                                if( (sChr < 0x90) || (sChr > 0xBF) )            \
                                {                                               \
                                    *(aResult) = STL_FALSE;                     \
                                }                                               \
                                else                                            \
                                {                                               \
                                    /* Do Nothing */                            \
                                }                                               \
                                break;                                          \
                            }                                                   \
                        case 0xF4:                                              \
                            {                                                   \
                                if( (sChr < 0x80) || (sChr > 0x8F) )            \
                                {                                               \
                                    *(aResult) = STL_FALSE;                     \
                                }                                               \
                                else                                            \
                                {                                               \
                                    /* Do Nothing */                            \
                                }                                               \
                                break;                                          \
                            }                                                   \
                        default:                                                \
                            {                                                   \
                                if( (sChr < 0x80) || (sChr > 0xBF) )            \
                                {                                               \
                                    *(aResult) = STL_FALSE;                     \
                                }                                               \
                                else                                            \
                                {                                               \
                                    /* Do Nothing */                            \
                                }                                               \
                                break;                                          \
                            }                                                   \
                    }                                                           \
                    /* FALL_THROUGH */                                          \
                }                                                               \
            case 1:                                                             \
                {                                                               \
                    sChr = (stlUInt8)(*(aSource));                              \
                    if ( (sChr >= 0x80) && (sChr < 0xC2) )                      \
                    {                                                           \
                        *(aResult) = STL_FALSE;                                 \
                    }                                                           \
                    else                                                        \
                    {                                                           \
                        if( sChr > 0xF4 )                                       \
                        {                                                       \
                            *(aResult) = STL_FALSE;                             \
                        }                                                       \
                        else                                                    \
                        {                                                       \
                            /* Do Nothing */                                    \
                        }                                                       \
                    }                                                           \
                    break;                                                      \
                }                                                               \
        }                                                                       \
    }

/**
 * @brief utf8 한 문자에 대한 Verify 여부
 * @param[in]  aSource        source str
 * @param[in]  aSourceLength  source str length
 * @param[out] aMbLength      한 문자에 대한 byte length
 * @param[out] aVerify        Verify 여부
 */
#define DTS_IS_VERIFY_UTF8( aSource, aSourceLength, aMbLength, aVerify ) \
    {                                                           \
        stlUInt8  sSourceChr;                                   \
        sSourceChr = (stlUInt8)(aSource)[0];                    \
                                                                \
        *(aVerify) = STL_TRUE;                                  \
        if( (sSourceChr & 0x80) == 0 )                          \
        {                                                       \
            *(aMbLength) = 1;                                   \
        }                                                       \
        else if ( (sSourceChr & 0xe0) == 0xc0 )                 \
        {                                                       \
            *(aMbLength) = 2;                                   \
        }                                                       \
        else if ( (sSourceChr & 0xf0) == 0xe0 )                 \
        {                                                       \
            *(aMbLength) = 3;                                   \
        }                                                       \
        else if ( (sSourceChr & 0xf8) == 0xf0 )                 \
        {                                                       \
            *(aMbLength) = 4;                                   \
        }                                                       \
        else                                                    \
        {                                                       \
            *(aVerify) = STL_FALSE;                             \
        }                                                       \
                                                                \
        if( *(aVerify) == STL_TRUE )                            \
        {                                                       \
            if( (aSourceLength) >= *(aMbLength) )               \
            {                                                   \
                /* Do Nothing */                                \
            }                                                   \
            else                                                \
            {                                                   \
                *(aVerify) = STL_FALSE;                         \
            }                                                   \
        }                                                       \
                                                                \
        if( *(aVerify) == STL_TRUE )                            \
        {                                                       \
            DTS_UTF8_VALIDATE( (aSource),                       \
                               *(aMbLength),                    \
                               (aVerify) );                     \
        }                                                       \
    }

/**
 * @brief gb18030 한 문자에 대한 Verify 여부
 * @param[in]  aSource        source str
 * @param[in]  aSourceLength  source str length
 * @param[out] aMbLength      한 문자에 대한 byte length
 * @param[out] aVerify        Verify 여부
 */
#define DTS_IS_VERIFY_GB18030( aSource, aSourceLength, aMbLength, aVerify )     \
    {                                                                           \
        *(aVerify) = STL_TRUE;                                                  \
                                                                                \
        DTS_GET_GB18030_MBLENGTH( (aSource), (aMbLength) );                     \
                                                                                \
        if( (aSourceLength) >= *(aMbLength) )                                   \
        {                                                                       \
            /* Do Nothing */                                                    \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            *(aVerify) = STL_FALSE;                                             \
        }                                                                       \
    }


/**
 * @brief byte length가 같은 한 문자에 대해서 같은 문자인지를 비교한다.
 * @param[in]  aChr1     
 * @param[in]  aChr2
 * @param[in]  aChrLen
 * @param[out] aIsEqual 
 */
#define DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( aChr1,          \
                                            aChr2,          \
                                            aChrLen,        \
                                            aIsEqual )      \
    {                                                       \
        *(aIsEqual) = STL_FALSE;                            \
                                                            \
        if( (aChrLen) == 1 )                                \
        {                                                   \
            if( (aChr1)[0] == (aChr2)[0] )                  \
            {                                               \
                *(aIsEqual) = STL_TRUE;                     \
            }                                               \
        }                                                   \
        else if( (aChrLen) == 2 )                           \
        {                                                   \
            if( ( (aChr1)[0] == (aChr2)[0] ) &&             \
                ( (aChr1)[1] == (aChr2)[1] ) )              \
            {                                               \
                *(aIsEqual) = STL_TRUE;                     \
            }                                               \
        }                                                   \
        else if( (aChrLen) == 3 )                           \
        {                                                   \
            if( ( (aChr1)[0] == (aChr2)[0] ) &&             \
                ( (aChr1)[1] == (aChr2)[1] ) &&             \
                ( (aChr1)[2] == (aChr2)[2] ) )              \
            {                                               \
                *(aIsEqual) = STL_TRUE;                     \
            }                                               \
        }                                                   \
        else if( (aChrLen) == 4 )                           \
        {                                                   \
            if( ( (aChr1)[0] == (aChr2)[0] ) &&             \
                ( (aChr1)[1] == (aChr2)[1] ) &&             \
                ( (aChr1)[2] == (aChr2)[2] ) &&             \
                ( (aChr1)[3] == (aChr2)[3] ) )              \
            {                                               \
                *(aIsEqual) = STL_TRUE;                     \
            }                                               \
        }                                                   \
        else                                                \
        {                                                   \
            STL_DASSERT( STL_FALSE );                       \
        }                                                   \
    }

/**
 * @brief 한 문자에 대한 복사
 * @param[out] aDestCharPtr      저장할 주소
 * @param[in]  aSourceCharPtr    복사할 one character ptr
 * @param[in]  aSourceCharLength 복사할 one character의 길이
 */
#define DTS_COPY_ONE_CHAR( aDestCharPtr, aSourceCharPtr, aSourceCharLength ) \
    {                                                                        \
        if( (aSourceCharLength) == 1 )                                       \
        {                                                                    \
            (aDestCharPtr)[0] = (aSourceCharPtr)[0];                         \
        }                                                                    \
        else if( (aSourceCharLength) == 2 )                                  \
        {                                                                    \
            (aDestCharPtr)[0] = (aSourceCharPtr)[0];                         \
            (aDestCharPtr)[1] = (aSourceCharPtr)[1];                         \
        }                                                                    \
        else if( (aSourceCharLength) == 3 )                                  \
        {                                                                    \
            (aDestCharPtr)[0] = (aSourceCharPtr)[0];                         \
            (aDestCharPtr)[1] = (aSourceCharPtr)[1];                         \
            (aDestCharPtr)[2] = (aSourceCharPtr)[2];                         \
        }                                                                    \
        else if( (aSourceCharLength) == 4 )                                  \
        {                                                                    \
            (aDestCharPtr)[0] = (aSourceCharPtr)[0];                         \
            (aDestCharPtr)[1] = (aSourceCharPtr)[1];                         \
            (aDestCharPtr)[2] = (aSourceCharPtr)[2];                         \
            (aDestCharPtr)[3] = (aSourceCharPtr)[3];                         \
        }                                                                    \
        else                                                                 \
        {                                                                    \
            STL_DASSERT( STL_FALSE );                                        \
        }                                                                    \
    }

    
typedef stlUInt32  dtsWchar;


/**
 * @todo public domain
 */
/*
 * This is an implementation of wcwidth() and wcswidth() as defined in
 * "The Single UNIX Specification, Version 2, The Open Group, 1997"
 * <http://www.UNIX-systems.org/online.html>
 *
 * Markus Kuhn -- 2001-09-08 -- public domain
 *
 * customised
 *
 * original available at : http://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
 */
typedef struct dtsMbInterval
{
    stlUInt16  mFirst;
    stlUInt16  mLast;
} dtsMbInterval;


/*******************************************************************************
 * CHARACTERSET FUNCTION POINTER 정의 
 ******************************************************************************/
// pg_do_encoding_conversion() mbutils.c
// pg_client_to_server(const char *s, int len)
// int pg_verify_mbstr_len(int encoding, const char *mbstr, int len, bool noError)
// mb2wchar_with_len_converter
// int pg_mb2wchar(const char *from, pg_wchar *to)
// int8 pg_mb2wchar_with_len(const char *from, pg_wchar *to, int len)
// int pg_encoding_mb2wchar_with_len(int encoding, const char *from, pg_wchar *to, int len)
// int pg_mbstrlen(const char *mbstr)
// pg_encoding_verifymb( , , , )

/**
 * @brief  주어진 characterSet의 문자에 대한 byte length를 얻는다.
 * @param[in]   aSource    aSource
 * @param[out]  aMbLength  aSource의 byte length
 */
typedef stlStatus (*dtsGetMbLengthFunc)( stlChar       * aSource,
                                         stlInt8       * aMbLength,
                                         stlErrorStack * aErrorStack );

/**
 * @brief  각 characterSet의 한 문자에 대한 최대 byte length를 얻는다.
 * @param[out]  aMbLength  최대 byte length
 */
typedef stlInt8 (*dtsGetMbMaxLengthFunc)();

/**
 * @brief  각 characterSet의 문자에 대한 validate
 * @param[in]   aStr
 * @param[in]   aLength
 * @param[out]  aMbLength
 */
typedef stlStatus (*dtsMbVerifyFunc)( stlChar       * aStr,
                                      stlInt64        aLength,
                                      stlInt8       * aMbLength,
                                      stlErrorStack * aErrorStack );

/**
 * @brief  주어진 characterSet의 문자에 대한 display length를 얻는다.
 * @param[in] aStr
 */
typedef stlInt32 (*dtsGetMbDisplayLengthFunc)( stlUInt8      * aChar );

/**
 * @brief 주어진 characterSet의 문자와 문자길이에 대해서 문자 갯수를 얻는다.
 * @param[in]  aString      aString
 * @param[in]  aLimit       aString의 길이(byte length)
 * @param[out] aMbStrLen    계산된 문자길이 
 * @param[in]  aVectorFunc  Function Vector
 * @param[in]  aVectorArg   Vector Argument
 * @param[out] aErrorStack  에러 스택 
 */
typedef stlStatus (*dtsGetMbstrlenWithLenFunc)( stlChar         * aString,
                                                stlInt64          aLimit,
                                                stlInt64        * aMbStrLen,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack );

/*******************************************************************************
 * CHARACTERSET CONVERSION FUNCTION POINTER 정의 
 ******************************************************************************/

/**
 * @brief characterset간의 conversion 수행
 * @param[in]  aSource      source string
 * @param[in]  aSourceLen   source string 길이
 * @param[out] aDest        dest를 저장할 메모리주소
 * @param[in]  aDestLen     dest string 길이
 * @param[out] aErrorStack  aErrorStack
 */
typedef stlStatus (*dtsMbConversionFunc)( stlChar         * aSource,
                                          stlInt64          aSourceLen,
                                          stlChar         * aDest,
                                          stlInt64          aDestLen,
                                          stlInt64        * aSourceOffset,
                                          stlInt64        * aTotalLength,
                                          stlErrorStack   * aErrorStack );

/**
 * @brief unicode encoding( utf16, utf32 ) 에서 Mb(muti byte) characterset간의 conversion 수행
 * @param[in]  aUniEncoding  source 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aSource       source string(utf16, utf32의 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
typedef stlStatus (*dtsUnicodeToMbConversionFunc)( dtlUnicodeEncoding   aUniEncoding,
                                                   void               * aSource,
                                                   stlInt64             aSourceLen,
                                                   stlChar            * aDest,
                                                   stlInt64             aDestLen,
                                                   stlInt64           * aSourceOffset,
                                                   stlInt64           * aTotalLength,
                                                   stlErrorStack      * aErrorStack );

/**
 * @brief Mb(muti byte) character set 을 unicode encoding ( utf16, utf32 ) 로 변환
 * @param[in]  aUniEncoding  dest 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aSource       source string ( Gb18030 )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
typedef stlStatus (*dtsMbToUnicodeConversionFunc)( dtlUnicodeEncoding   aUniEncoding,
                                                   stlChar            * aSource,
                                                   stlInt64             aSourceLen,
                                                   void               * aDest,
                                                   stlInt64             aDestLen,
                                                   stlInt64           * aSourceOffset,
                                                   stlInt64           * aTotalLength,
                                                   stlErrorStack      * aErrorStack );

/**
 * @brief unicode encodin 간의 변환 ( utf16 <-> utf8, utf32 <-> utf8 )
 * @param[in]  aSource               source( utf8 ) 문자
 * @param[in]  aSourceLen            source( utf8 ) 문자 길이
 * @param[out] aDest                 dest( utf16 )를 저장할 메모리 주소  
 * @param[in]  aDestLen              dest( utf16 )를 저장할 메모리 크기
 * @param[out] aSourceConvertedLen   변환이 성공한 @a aSource의 byte length
 * @param[out] aDestConvertedLen     변환이 성공한 @a aDest의 byte length
 * @param[in]  aErrorStack           stlErrorStack
 */
typedef stlStatus (*dtsUnicodeConversionFunc)( void           * aSource,
                                               stlInt64         aSourceLen,
                                               void           * aDest,
                                               stlInt64         aDestLen,
                                               stlInt64       * aSourceConvertedLen,
                                               stlInt64       * aDestConvertedLen,
                                               stlErrorStack  * aErrorStack );


/*******************************************************************************
 * CHARACTERSET FUNCTION 정의 
 ******************************************************************************/

/* with_len_converter */
/* mbDisplayLen ??? */


/*
 * dtsGetMbLengthFunc의  각 함수들
 */

stlStatus dtsGetAsciiMbLength( stlChar       * aSource,
                               stlInt8       * aMbLength,
                               stlErrorStack * aErrorStack );

stlStatus dtsGetUtf8MbLength( stlChar       * aSource,
                              stlInt8       * aMbLength,
                              stlErrorStack * aErrorStack );

stlStatus dtsGetUhcMbLength( stlChar       * aSource,
                             stlInt8       * aMbLength,
                             stlErrorStack * aErrorStack );

stlStatus dtsGetGb18030MbLength( stlChar       * aSource,
                                 stlInt8       * aMbLength,
                                 stlErrorStack * aErrorStack );

/*
 * dtsGetMbMaxLengthFunc의  각 함수들
 */

stlInt8 dtsGetAsciiMbMaxLength();

stlInt8 dtsGetUtf8MbMaxLength();

stlInt8 dtsGetUhcMbMaxLength();

stlInt8 dtsGetGb18030MbMaxLength();

/*
 * dtsMbVerifyFunc의  각 함수들
 */
stlStatus dtsAsciiVerify( stlChar       * aSource,
                          stlInt64        aLength,
                          stlInt8       * aMbLength,
                          stlErrorStack * aErrorStack );

stlStatus dtsUtf8Verify( stlChar       * aSource,
                         stlInt64        aLength,
                         stlInt8       * aMbLength,
                         stlErrorStack * aErrorStack );

stlStatus dtsUhcVerify( stlChar       * aSource,
                        stlInt64        aLength,
                        stlInt8       * aMbLength,
                        stlErrorStack * aErrorStack );

stlStatus dtsGb18030Verify( stlChar       * aSource,
                            stlInt64        aLength,
                            stlInt8       * aMbLength,
                            stlErrorStack * aErrorStack );

/*
 * dtsGetMbDisplayLengthFunc의 각 함수들
 */

stlInt32 dtsGetAsciiDisplayLength( stlUInt8      * aChar );

stlInt32 dtsGetUtf8DisplayLength( stlUInt8      * aChar );

stlInt32 dtsGetUhcDisplayLength( stlUInt8      * aChar );

stlInt32 dtsGetGb18030DisplayLength( stlUInt8      * aChar );

/*
 * dtsGetMbstrlenWithLenFunc의 각 함수들
 */

stlStatus dtsGetAsciiMbstrlenWithLen( stlChar         * aString,
                                      stlInt64          aLimit,
                                      stlInt64        * aMbStrLen,
                                      dtlFuncVector   * aVectorFunc,
                                      void            * aVectorArg,
                                      stlErrorStack   * aErrorStack );

stlStatus dtsGetUtf8MbstrlenWithLen( stlChar         * aString,
                                     stlInt64          aLimit,
                                     stlInt64        * aMbStrLen,
                                     dtlFuncVector   * aVectorFunc,
                                     void            * aVectorArg,
                                     stlErrorStack   * aErrorStack );

stlStatus dtsGetUhcMbstrlenWithLen( stlChar         * aString,
                                    stlInt64          aLimit,
                                    stlInt64        * aMbStrLen,
                                    dtlFuncVector   * aVectorFunc,
                                    void            * aVectorArg,
                                    stlErrorStack   * aErrorStack );

stlStatus dtsGetGb18030MbstrlenWithLen( stlChar         * aString,
                                        stlInt64          aLimit,
                                        stlInt64        * aMbStrLen,
                                        dtlFuncVector   * aVectorFunc,
                                        void            * aVectorArg,
                                        stlErrorStack   * aErrorStack );


/*******************************************************************************
 * CHARACTERSET CONVERSION FUNCTION 정의 
 ******************************************************************************/

/* dtsMbConversionFunc의 각 함수들 */
stlStatus dtsConversionNotApplicable( stlChar         * aSource,
                                      stlInt64          aSourceLen,
                                      stlChar         * aDest,
                                      stlInt64          aDestLen,
                                      stlInt64        * aSourceOffset,
                                      stlInt64        * aTotalLength,
                                      stlErrorStack   * aErrorStack );

stlStatus dtsConversionAsciiToUtf8( stlChar         * aSource,
                                    stlInt64          aSourceLen,
                                    stlChar         * aDest,
                                    stlInt64          aDestLen,
                                    stlInt64        * aSourceOffset,
                                    stlInt64        * aTotalLength,
                                    stlErrorStack   * aErrorStack );

stlStatus dtsConversionAsciiToUhc( stlChar         * aSource,
                                   stlInt64          aSourceLen,
                                   stlChar         * aDest,
                                   stlInt64          aDestLen,
                                   stlInt64        * aSourceOffset,
                                   stlInt64        * aTotalLength,
                                   stlErrorStack   * aErrorStack );

stlStatus dtsConversionAsciiToGb18030( stlChar         * aSource,
                                       stlInt64          aSourceLen,
                                       stlChar         * aDest,
                                       stlInt64          aDestLen,
                                       stlInt64        * aSourceOffset,
                                       stlInt64        * aTotalLength,
                                       stlErrorStack   * aErrorStack );

stlStatus dtsConversionUtf8ToAscii( stlChar         * aSource,
                                    stlInt64          aSourceLen,
                                    stlChar         * aDest,
                                    stlInt64          aDestLen,
                                    stlInt64        * aSourceOffset,
                                    stlInt64        * aTotalLength,
                                    stlErrorStack   * aErrorStack );

stlStatus dtsConversionUtf8ToUhc( stlChar         * aSource,
                                  stlInt64          aSourceLen,
                                  stlChar         * aDest,
                                  stlInt64          aDestLen,
                                  stlInt64        * aSourceOffset,
                                  stlInt64        * aTotalLength,
                                  stlErrorStack   * aErrorStack );

stlStatus dtsConversionUtf8ToGb18030( stlChar         * aSource,
                                      stlInt64          aSourceLen,
                                      stlChar         * aDest,
                                      stlInt64          aDestLen,
                                      stlInt64        * aSourceOffset,
                                      stlInt64        * aTotalLength,
                                      stlErrorStack   * aErrorStack );

stlStatus dtsConversionUhcToAscii( stlChar         * aSource,
                                   stlInt64          aSourceLen,
                                   stlChar         * aDest,
                                   stlInt64          aDestLen,
                                   stlInt64        * aSourceOffset,
                                   stlInt64        * aTotalLength,
                                   stlErrorStack   * aErrorStack );

stlStatus dtsConversionUhcToUtf8( stlChar         * aSource,
                                  stlInt64          aSourceLen,
                                  stlChar         * aDest,
                                  stlInt64          aDestLen,
                                  stlInt64        * aSourceOffset,
                                  stlInt64        * aTotalLength,
                                  stlErrorStack   * aErrorStack );

stlStatus dtsConversionGb18030ToAscii( stlChar         * aSource,
                                       stlInt64          aSourceLen,
                                       stlChar         * aDest,
                                       stlInt64          aDestLen,
                                       stlInt64        * aSourceOffset,
                                       stlInt64        * aTotalLength,
                                       stlErrorStack   * aErrorStack );

stlStatus dtsConversionGb18030ToUtf8( stlChar         * aSource,
                                      stlInt64          aSourceLen,
                                      stlChar         * aDest,
                                      stlInt64          aDestLen,
                                      stlInt64        * aSourceOffset,
                                      stlInt64        * aTotalLength,
                                      stlErrorStack   * aErrorStack );

/*******************************************************************************
 * CHARACTERSET FUNCTION POINTER LIST
 ******************************************************************************/
                                            
extern dtsGetMbLengthFunc  dtsGetMbLengthFuncList[ DTL_CHARACTERSET_MAX ];

extern dtsGetMbMaxLengthFunc  dtsGetMbMaxLengthFuncList[ DTL_CHARACTERSET_MAX ];

extern dtsMbVerifyFunc  dtsMbVerifyFuncList[ DTL_CHARACTERSET_MAX ];

extern dtsGetMbDisplayLengthFunc dtsGetMbDisplayLengthFuncList[ DTL_CHARACTERSET_MAX ];

extern dtsGetMbstrlenWithLenFunc dtsGetMbstrlenWithLenFuncList[ DTL_CHARACTERSET_MAX ];


/*******************************************************************************
 * CHARACTERSET CONVERSION FUNCTION POINTER LIST
 ******************************************************************************/
extern dtsMbConversionFunc dtsMbConversionFuncList[DTL_CHARACTERSET_MAX][DTL_CHARACTERSET_MAX];

extern dtsUnicodeToMbConversionFunc dtsUnicodeToMbConversionFuncList[DTL_CHARACTERSET_MAX];

extern dtsMbToUnicodeConversionFunc dtsMbToUnicodeConversionFuncList[DTL_CHARACTERSET_MAX];

extern dtsUnicodeConversionFunc dtsUnicodeConversionFuncList[DTL_UNICODE_MAX][DTL_UNICODE_MAX];


/*******************************************************************************
 * 기타 내부 함수들 
 ******************************************************************************/

stlStatus dtsUtf8Validate( stlChar       * aSource,
                           stlInt8         aLength,
                           stlErrorStack * aErrorStack );

dtsWchar dtsUtf2Ucs( const stlUInt8  * aChar );

stlInt32 dtsUcsWcWidth( dtsWchar aUcsChar );

stlInt32 dtsMbIntervalBinarySearch( dtsWchar              aUcsChar,
                                    const dtsMbInterval * aMbInterval,
                                    stlInt32              aMax );

/*******************************************************************************
 * conversion 내부 함수들
 ******************************************************************************/

stlStatus dtsAsciiToMic( stlChar        * aSource,
                         stlInt64         aSourceLen,
                         stlChar        * aDest,
                         stlInt64         aDestLen,
                         stlInt64       * aSourceOffset,
                         stlInt64       * aTotalLength,
                         stlErrorStack  * aErrorStack );

stlStatus dtsMicToAscii( stlChar        * aSource,
                         stlInt64         aSourceLen,
                         stlChar        * aDest,
                         stlInt64         aDestLen,
                         stlInt64       * aSourceOffset,
                         stlInt64       * aTotalLength,
                         stlErrorStack  * aErrorStack );

/*******************************************************************************
 * conversion 내부 함수들 ( unicode encodin 간의 conversion )
 ******************************************************************************/

stlStatus dtsUtf16ToUtf8( void           * aSource,
                          stlInt64         aSourceLen,
                          void           * aDest,
                          stlInt64         aDestLen,
                          stlInt64       * aSourceConvertedLen,
                          stlInt64       * aDestConvertedLen,
                          stlErrorStack  * aErrorStack );

stlStatus dtsUtf32ToUtf8( void           * aSource,
                          stlInt64         aSourceLen,
                          void           * aDest,
                          stlInt64         aDestLen,
                          stlInt64       * aSourceConvertedLen,
                          stlInt64       * aDestConvertedLen,
                          stlErrorStack  * aErrorStack );

stlStatus dtsUtf8ToUtf16( void           * aSource,
                          stlInt64         aSourceLen,
                          void           * aDest,
                          stlInt64         aDestLen,
                          stlInt64       * aSourceConvertedLen,
                          stlInt64       * aDestConvertedLen,
                          stlErrorStack  * aErrorStack );

stlStatus dtsUtf8ToUtf32( void           * aSource,
                          stlInt64         aSourceLen,
                          void           * aDest,
                          stlInt64         aDestLen,
                          stlInt64       * aSourceConvertedLen,
                          stlInt64       * aDestConvertedLen,
                          stlErrorStack  * aErrorStack );

stlStatus dtsUnicodeConversionNotApplicable( void               * aSource,
                                             stlInt64             aSourceLen,
                                             void               * aDest,
                                             stlInt64             aDestLen,
                                             stlInt64           * aSourceOffset,
                                             stlInt64           * aTotalLength,
                                             stlErrorStack      * aErrorStack );


/*******************************************************************************
 * conversion 내부 함수들 ( unicode 에서 db character set 으로의 conversion
 ******************************************************************************/

stlStatus dtsConversionUnicodeToAscii( dtlUnicodeEncoding   aUniEncoding,
                                       void               * aSource,
                                       stlInt64             aSourceLen,
                                       stlChar            * aDest,
                                       stlInt64             aDestLen,
                                       stlInt64           * aSourceOffset,
                                       stlInt64           * aTotalLength,
                                       stlErrorStack      * aErrorStack );

stlStatus dtsConversionUnicodeToUtf8( dtlUnicodeEncoding   aUniEncoding,
                                      void               * aSource,
                                      stlInt64             aSourceLen,
                                      stlChar            * aDest,
                                      stlInt64             aDestLen,
                                      stlInt64           * aSourceOffset,
                                      stlInt64           * aTotalLength,
                                      stlErrorStack      * aErrorStack );

stlStatus dtsConversionUnicodeToUhc( dtlUnicodeEncoding   aUniEncoding,
                                     void               * aSource,
                                     stlInt64             aSourceLen,
                                     stlChar            * aDest,
                                     stlInt64             aDestLen,
                                     stlInt64           * aSourceOffset,
                                     stlInt64           * aTotalLength,
                                     stlErrorStack      * aErrorStack );

stlStatus dtsConversionUnicodeToGb18030( dtlUnicodeEncoding   aUniEncoding,
                                         void               * aSource,
                                         stlInt64             aSourceLen,
                                         stlChar            * aDest,
                                         stlInt64             aDestLen,
                                         stlInt64           * aSourceOffset,
                                         stlInt64           * aTotalLength,
                                         stlErrorStack      * aErrorStack );

stlStatus dtsUnicodeToMbConversionNotApplicable( dtlUnicodeEncoding   aUniEncoding,
                                                 void               * aSource,
                                                 stlInt64             aSourceLen,
                                                 stlChar            * aDest,
                                                 stlInt64             aDestLen,
                                                 stlInt64           * aSourceOffset,
                                                 stlInt64           * aTotalLength,
                                                 stlErrorStack      * aErrorStack );

/*******************************************************************************
 * conversion 내부 함수들 ( db character set 에서 unicode 로의 conversion
 ******************************************************************************/

stlStatus dtsConversionAsciiToUnicode( dtlUnicodeEncoding   aUniEncoding,
                                       stlChar            * aSource,
                                       stlInt64             aSourceLen,
                                       void               * aDest,
                                       stlInt64             aDestLen,
                                       stlInt64           * aSourceOffset,
                                       stlInt64           * aTotalLength,
                                       stlErrorStack      * aErrorStack );

stlStatus dtsConversionUtf8ToUnicode( dtlUnicodeEncoding   aUniEncoding,
                                      stlChar            * aSource,
                                      stlInt64             aSourceLen,
                                      void               * aDest,
                                      stlInt64             aDestLen,
                                      stlInt64           * aSourceOffset,
                                      stlInt64           * aTotalLength,
                                      stlErrorStack      * aErrorStack );

stlStatus dtsConversionUhcToUnicode( dtlUnicodeEncoding   aUniEncoding,
                                     stlChar            * aSource,
                                     stlInt64             aSourceLen,
                                     void               * aDest,
                                     stlInt64             aDestLen,
                                     stlInt64           * aSourceOffset,
                                     stlInt64           * aTotalLength,
                                     stlErrorStack      * aErrorStack );

stlStatus dtsConversionGb18030ToUnicode( dtlUnicodeEncoding   aUniEncoding,
                                         stlChar            * aSource,
                                         stlInt64             aSourceLen,
                                         void               * aDest,
                                         stlInt64             aDestLen,
                                         stlInt64           * aSourceOffset,
                                         stlInt64           * aTotalLength,
                                         stlErrorStack      * aErrorStack );

stlStatus dtsConversionUtf8ToUtf16( stlChar            * aSource,
                                    stlInt64             aSourceLen,
                                    void               * aDest,
                                    stlInt64             aDestLen,
                                    stlInt64           * aSourceOffset,
                                    stlInt64           * aTotalLength,
                                    stlErrorStack      * aErrorStack );

stlStatus dtsConversionUtf8ToUtf32( stlChar            * aSource,
                                    stlInt64             aSourceLen,
                                    void               * aDest,
                                    stlInt64             aDestLen,
                                    stlInt64           * aSourceOffset,
                                    stlInt64           * aTotalLength,
                                    stlErrorStack      * aErrorStack );

stlStatus dtsConversionUhcToUtf16( stlChar            * aSource,
                                   stlInt64             aSourceLen,
                                   void               * aDest,
                                   stlInt64             aDestLen,
                                   stlInt64           * aSourceOffset,
                                   stlInt64           * aTotalLength,
                                   stlErrorStack      * aErrorStack );

stlStatus dtsConversionUhcToUtf32( stlChar            * aSource,
                                   stlInt64             aSourceLen,
                                   void               * aDest,
                                   stlInt64             aDestLen,
                                   stlInt64           * aSourceOffset,
                                   stlInt64           * aTotalLength,
                                   stlErrorStack      * aErrorStack );

stlStatus dtsConversionGb18030ToUtf16( stlChar            * aSource,
                                       stlInt64             aSourceLen,
                                       void               * aDest,
                                       stlInt64             aDestLen,
                                       stlInt64           * aSourceOffset,
                                       stlInt64           * aTotalLength,
                                       stlErrorStack      * aErrorStack );

stlStatus dtsConversionGb18030ToUtf32( stlChar            * aSource,
                                       stlInt64             aSourceLen,
                                       void               * aDest,
                                       stlInt64             aDestLen,
                                       stlInt64           * aSourceOffset,
                                       stlInt64           * aTotalLength,
                                       stlErrorStack      * aErrorStack );


/** @} */

#endif /* _DTSCHARACTERSET_H_ */
