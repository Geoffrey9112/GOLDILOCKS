/*******************************************************************************
 * dtsConversion.c
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
 * @file dtsConversion.c
 * @brief DataType Layer Character set conversion 관련 함수 정의
 */

#include <dtl.h>
#include <dtsCharacterSet.h>

/**
 * @addtogroup dts
 * @{
 */

stlDsoHandle dtsMbConversionHandleList[DTL_CHARACTERSET_MAX]   = { NULL, };

stlUInt32 * dtsUTF8MbConversionHashList[DTL_CHARACTERSET_MAX] = { NULL, };
stlUInt32 * dtsMbUTF8ConversionHashList[DTL_CHARACTERSET_MAX] = { NULL, };

dtsMbConversionFunc dtsMbConversionFuncList[DTL_CHARACTERSET_MAX][DTL_CHARACTERSET_MAX] =
{
    /* DTL_SQL_ASCII */
    { dtsConversionNotApplicable,    /* DTL_SQL_ASCII */
      dtsConversionAsciiToUtf8,      /* DTL_UTF8      */
      dtsConversionAsciiToUhc,       /* DTL_UHC       */
      dtsConversionAsciiToGb18030 }, /* DTL_GB18030   */

    /* DTL_UTF8 */
    { dtsConversionUtf8ToAscii,      /* DTL_SQL_ASCII */
      dtsConversionNotApplicable,    /* DTL_UTF8      */
      dtsConversionUtf8ToUhc,        /* DTL_UHC       */
      dtsConversionUtf8ToGb18030 },  /* DTL_GB18030   */

    /* DTL_UHC */
    { dtsConversionUhcToAscii,       /* DTL_SQL_ASCII */
      dtsConversionUhcToUtf8,        /* DTL_UTF8      */
      dtsConversionNotApplicable,    /* DTL_UHC       */
      dtsConversionNotApplicable },  /* DTL_GB18030   */

    /* DTL_GB18030 */
    { dtsConversionGb18030ToAscii,   /* DTL_SQL_ASCII */
      dtsConversionGb18030ToUtf8,    /* DTL_UTF8      */
      dtsConversionNotApplicable,    /* DTL_UHC       */
      dtsConversionNotApplicable },  /* DTL_GB18030   */
};


dtsUnicodeToMbConversionFunc 
dtsUnicodeToMbConversionFuncList[DTL_CHARACTERSET_MAX] =
{
    dtsConversionUnicodeToAscii,     /* DTL_SQL_ASCII */
    dtsConversionUnicodeToUtf8,      /* DTL_UTF8      */
    dtsConversionUnicodeToUhc,       /* DTL_UHC       */
    dtsConversionUnicodeToGb18030    /* DTL_GB18030   */
};


dtsMbToUnicodeConversionFunc
dtsMbToUnicodeConversionFuncList[DTL_CHARACTERSET_MAX] =
{
    dtsConversionAsciiToUnicode,     /* DTL_SQL_ASCII */
    dtsConversionUtf8ToUnicode,      /* DTL_UTF8      */
    dtsConversionUhcToUnicode,       /* DTL_UHC       */
    dtsConversionGb18030ToUnicode    /* DTL_GB18030   */
};



dtsUnicodeConversionFunc dtsUnicodeConversionFuncList[DTL_UNICODE_MAX][DTL_UNICODE_MAX] =
{
    /* DTL_UNICODE_UTF8 */
    { dtsUnicodeConversionNotApplicable,     /* DTL_UNICODE_UTF8  */
      dtsUtf8ToUtf16,                        /* DTL_UNICODE_UTF16 */
      dtsUtf8ToUtf32 },                      /* DTL_UNICODE_UTF32 */

    /* DTL_UNICODE_UTF16 */
    { dtsUtf16ToUtf8,                        /* DTL_UNICODE_UTF8  */
      dtsUnicodeConversionNotApplicable,     /* DTL_UNICODE_UTF16 */
      dtsUnicodeConversionNotApplicable },   /* DTL_UNICODE_UTF32 */

    /* DTL_UNICODE_UTF32 */
    { dtsUtf32ToUtf8,                        /* DTL_UNICODE_UTF8  */
      dtsUnicodeConversionNotApplicable,     /* DTL_UNICODE_UTF16 */
      dtsUnicodeConversionNotApplicable },   /* DTL_UNICODE_UTF32 */
};

/**
 * @brief characterset library를 loaing 한다.
 * @param[in]  aCharacterSet loading 할 characterset
 * @param[out] aErrorStack   aErrorStack
 */
static stlStatus dtsOpenConversionLibrary( dtlCharacterSet   aCharacterSet,
                                           stlErrorStack   * aErrorStack )
{
    stlChar     sHomeDir[STL_PATH_MAX];
    stlChar     sLibraryDir[STL_PATH_MAX];
    stlBool     sIsFound = STL_FALSE;

    stlChar     sSymbolName[64];

    stlChar *   sCharactersetName = NULL;

    STL_TRY( stlGetEnv( sHomeDir,
                        STL_PATH_MAX,
                        "GOLDILOCKS_HOME",
                        &sIsFound,
                        aErrorStack ) == STL_SUCCESS );

    STL_ASSERT( sIsFound == STL_TRUE );

    /**
     * @todo OS에 따라 so, dll 구분이 필요한다.
     */

    sCharactersetName = gDtlCharacterSetString[aCharacterSet];

#if (STL_SIZEOF_VOIDP == 4 )    
    stlSnprintf( sLibraryDir,
                 STL_PATH_MAX,
                 "%s"STL_PATH_SEPARATOR"lib"STL_PATH_SEPARATOR""STL_SHARED_LIBRARY_PREFIX"goldilockscvt%s_32"STL_SHARED_LIBRARY_SUFFIX,
                 sHomeDir,
                 sCharactersetName );
#else
    stlSnprintf( sLibraryDir,
                 STL_PATH_MAX,
                 "%s"STL_PATH_SEPARATOR"lib"STL_PATH_SEPARATOR""STL_SHARED_LIBRARY_PREFIX"goldilockscvt%s_64"STL_SHARED_LIBRARY_SUFFIX,
                 sHomeDir,
                 sCharactersetName );
#endif
                            
    STL_TRY( stlOpenLibrary( sLibraryDir,
                             &dtsMbConversionHandleList[aCharacterSet],
                             aErrorStack ) == STL_SUCCESS );

    /*
     * UTF8 -> character set
     */

    stlSnprintf( sSymbolName,
                 64,
                 "DTS_UTF8_%s_HASH",
                 sCharactersetName );

    STL_TRY( stlGetSymbol( dtsMbConversionHandleList[aCharacterSet],
                           sSymbolName,
                           (stlDsoSymHandle*)&dtsUTF8MbConversionHashList[aCharacterSet],
                           aErrorStack ) == STL_SUCCESS );
    
    /*
     * character set -> UTF8
     */

    stlSnprintf( sSymbolName,
                 64,
                 "DTS_%s_UTF8_HASH",
                 sCharactersetName );

    STL_TRY( stlGetSymbol( dtsMbConversionHandleList[aCharacterSet],
                           sSymbolName,
                           (stlDsoSymHandle*)&dtsMbUTF8ConversionHashList[aCharacterSet],
                           aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief characterset간의 conversion 수행(conversion수행을 지원하지 않음)
 * @param[in]  aSource       source string
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest 의 공간이 충분할 경우 변환 성공 후 @a aDest 의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionNotApplicable( stlChar         * aSource,
                                      stlInt64          aSourceLen,
                                      stlChar         * aDest,
                                      stlInt64          aDestLen,
                                      stlInt64        * aSourceOffset,
                                      stlInt64        * aTotalLength,
                                      stlErrorStack   * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtsConversionNotApplicable()" );

    return STL_FAILURE;
}

/**
 * @brief ascii -> utf8 의 conversion수행 
 * @param[in]  aSource       source string(ascii code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest 의 공간이 충분할 경우 변환 성공 후 @a aDest 의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionAsciiToUtf8( stlChar         * aSource,
                                    stlInt64          aSourceLen,
                                    stlChar         * aDest,
                                    stlInt64          aDestLen,
                                    stlInt64        * aSourceOffset,
                                    stlInt64        * aTotalLength,
                                    stlErrorStack   * aErrorStack )
{
    STL_TRY( dtsAsciiToMic( aSource,
                            aSourceLen,
                            aDest,
                            aDestLen,
                            aSourceOffset,
                            aTotalLength,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ascii -> uhc 의 conversion수행 
 * @param[in]  aSource       source string(ascii code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest 의 공간이 충분할 경우 변환 성공 후 @a aDest 의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionAsciiToUhc( stlChar         * aSource,
                                   stlInt64          aSourceLen,
                                   stlChar         * aDest,
                                   stlInt64          aDestLen,
                                   stlInt64        * aSourceOffset,
                                   stlInt64        * aTotalLength,
                                   stlErrorStack   * aErrorStack )
{
    STL_TRY( dtsAsciiToMic( aSource,
                            aSourceLen,
                            aDest,
                            aDestLen,
                            aSourceOffset,
                            aTotalLength,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ascii -> gb18030 의 conversion수행 
 * @param[in]  aSource       source string(ascii code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest 의 공간이 충분할 경우 변환 성공 후 @a aDest 의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionAsciiToGb18030( stlChar         * aSource,
                                       stlInt64          aSourceLen,
                                       stlChar         * aDest,
                                       stlInt64          aDestLen,
                                       stlInt64        * aSourceOffset,
                                       stlInt64        * aTotalLength,
                                       stlErrorStack   * aErrorStack )
{
    STL_TRY( dtsAsciiToMic( aSource,
                            aSourceLen,
                            aDest,
                            aDestLen,
                            aSourceOffset,
                            aTotalLength,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief utf8 -> ascii 의 conversion수행 
 * @param[in]  aSource       source string(utf8 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUtf8ToAscii( stlChar         * aSource,
                                    stlInt64          aSourceLen,
                                    stlChar         * aDest,
                                    stlInt64          aDestLen,
                                    stlInt64        * aSourceOffset,
                                    stlInt64        * aTotalLength,
                                    stlErrorStack   * aErrorStack )
{
    stlUInt8  * sSource;
    stlUInt8  * sDest;
    stlInt8     sChr;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalLength = 0;
    stlUInt8  * sDestEnd;
    stlInt8     sMbLen;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    sSource = (stlUInt8 *)aSource;
    sDest   = (stlUInt8 *)aDest;

    sSourceLen = aSourceLen;
    sDestEnd = (sDest == NULL) ? sDest : (sDest + aDestLen);
    
    while( sSourceLen > 0 )
    {
        if( DTS_IS_HIGH_BIT_SET(*sSource) == STL_FALSE )
        {
            sChr = *sSource;

            if( sChr == 0 )
            {
                sChr = '?';
            }

            if( sDest < sDestEnd )
            {
                *sDest = sChr;
                sDest++;

                sSourceOffset = sOffset;
                sOffset++;
            }

            sTotalLength++;

            sSource++;
            sSourceLen--;
        }
        else
        {
            STL_TRY( dtlGetMbLength( DTL_UTF8,
                                     (stlChar*)sSource,
                                     &sMbLen,
                                     aErrorStack ) == STL_SUCCESS );
                                     
            if( sDest < sDestEnd )
            {
                *sDest = '?';
                sDest++;

                sSourceOffset = sOffset;
                sOffset += sMbLen;
            }

            sTotalLength++;

            sSource += sMbLen;
            sSourceLen -= sMbLen;
        }
    }

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalLength;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief utf8 -> uhc(aliase ms949) 의 conversion수행 
 * @param[in]  aSource       source string(utf8 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 * @todo 이후 character set 추가시,
 *       <BR> combined characters가 필요한 character set인 경우,
 *       <BR> 이에 대한 코드가 추가 되어야 함.
 */
stlStatus dtsConversionUtf8ToUhc( stlChar         * aSource,
                                  stlInt64          aSourceLen,
                                  stlChar         * aDest,
                                  stlInt64          aDestLen,
                                  stlInt64        * aSourceOffset,
                                  stlInt64        * aTotalLength,
                                  stlErrorStack   * aErrorStack )
{
    stlUInt8  * sUtfString;
    stlUInt8  * sUhcString;
    stlInt64    sUtfStringLen;
    stlInt8     sMbLen = 0;
    stlUInt32   sUtfCode = 0;
    stlUInt32   sUhcCode = 0;
    stlUInt8  * sUhcEnd;
    stlInt64    sUtfOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalUhcLen = 0;

    stlUInt32 * sHash = NULL;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( (dtsMbConversionHandleList[DTL_UHC] == NULL) ||
        (dtsUTF8MbConversionHashList[DTL_UHC] == NULL) )
    {
        STL_TRY( dtsOpenConversionLibrary( DTL_UHC, aErrorStack ) == STL_SUCCESS );
    }

    sHash = dtsUTF8MbConversionHashList[DTL_UHC];

    sUtfString = (stlUInt8*)aSource;
    sUhcString = (stlUInt8*)aDest;
    sUhcEnd = (sUhcString == NULL) ? sUhcString : (sUhcString + aDestLen);

    for( sUtfStringLen = aSourceLen;
         sUtfStringLen > 0;
         sUtfStringLen = sUtfStringLen - sMbLen )
    {
        /* if( *sUtfString == '\0' ) */
        /* { */
        /*     break; */
        /* } */
        /* else */
        /* { */
        /*     /\* Do Nothing *\/ */
        /* } */

        STL_TRY( dtsGetUtf8MbLength( (stlChar *)sUtfString,
                                     &sMbLen,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( dtsUtf8Validate( (stlChar *)sUtfString,
                                  sMbLen,
                                  aErrorStack )
                 == STL_SUCCESS );

        if( sMbLen == 1 )
        {
            /* ASCII 1Byte Code */

            if( sUhcString < sUhcEnd )
            {
                *sUhcString = *sUtfString;
                sUhcString++;

                sUtfOffset = sOffset;
                sOffset += sMbLen;
            }

            sTotalUhcLen++;
            sUtfString++;

            continue;
        }
        else if( sMbLen == 2 )
        {
            sUtfCode = *sUtfString << 8;
            sUtfString++;

            sUtfCode |= *sUtfString;
            sUtfString++;
        }
        else if( sMbLen == 3 )
        {
            sUtfCode = *sUtfString << 16;
            sUtfString++;

            sUtfCode |= *sUtfString << 8;
            sUtfString++;

            sUtfCode |= *sUtfString;
            sUtfString++;
        }
        else if( sMbLen == 4 )
        {
            sUtfCode = *sUtfString << 24;
            sUtfString++;

            sUtfCode |= *sUtfString << 16;
            sUtfString++;

            sUtfCode |= *sUtfString << 8;
            sUtfString++;

            sUtfCode |= *sUtfString;
            sUtfString++;
        }
        else
        {
            STL_DASSERT( STL_FALSE );
            STL_THROW( ERROR_INTERNAL );
        }

        sUhcCode = sHash[DTS_HASH_BUCKET_IDX(DTS_UTF8_UHC_HASH_BUCKET,
                                             sUtfCode,
                                             DTS_UTF8_UHC_HASH_MODULUS_CONSTANT,
                                             DTS_UTF8_UHC_HASH_MODULUS_SHIFT_BIT,
                                             DTS_UTF8_UHC_HASH_ADJUST_CONSTANT)];

        if( sUhcCode == 0 )
        {
            if( sUhcString < sUhcEnd )
            {
                *sUhcString = '?';
                sUhcString++;

                sUtfOffset = sOffset;
                sOffset += sMbLen;
            }

            sTotalUhcLen++;
        }
        else
        {
            if( sUhcCode & 0x0000ff00 )
            {
                if( sUhcString + 1 < sUhcEnd )
                {
                    *sUhcString = ( sUhcCode & 0x0000ff00 ) >> 8;
                    sUhcString++;
                    *sUhcString = sUhcCode & 0x000000ff;
                    sUhcString++;

                    sUtfOffset = sOffset;
                    sOffset += sMbLen;
                }

                sTotalUhcLen += 2;
            }
            else if( sUhcCode & 0x000000ff )
            {
                if( sUhcString < sUhcEnd )
                {
                    *sUhcString = sUhcCode & 0x000000ff;
                    sUhcString++;

                    sUtfOffset = sOffset;
                    sOffset += sMbLen;
                }

                sTotalUhcLen++;
            }
            else
            {
                STL_DASSERT( STL_FALSE );
                STL_THROW( ERROR_INTERNAL );
            }
        }
    }

    STL_TRY_THROW( sUtfStringLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sUhcString < sUhcEnd )
    {
        sUtfOffset = sOffset;
    }
    
    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sUtfOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUhcLen;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionUtf8ToUhc()" );  
    } 

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief utf8 -> gb18030 의 conversion수행 
 * @param[in]  aSource       source string(utf8 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 * @todo 이후 character set 추가시,
 *       <BR> combined characters가 필요한 character set인 경우,
 *       <BR> 이에 대한 코드가 추가 되어야 함.
 */
stlStatus dtsConversionUtf8ToGb18030( stlChar         * aSource,
                                      stlInt64          aSourceLen,
                                      stlChar         * aDest,
                                      stlInt64          aDestLen,
                                      stlInt64        * aSourceOffset,
                                      stlInt64        * aTotalLength,
                                      stlErrorStack   * aErrorStack )
{
    stlUInt8  * sUtfString;
    stlUInt8  * sGb18030String;
    stlInt64    sUtfStringLen;
    stlInt8     sMbLen = 0;
    stlUInt32   sUtfCode = 0;
    stlUInt32   sGb18030Code = 0;
    stlUInt8  * sGb18030End;
    stlInt64    sUtfOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalGb18030Len = 0;

    stlUInt32 * sHash = NULL;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( (dtsMbConversionHandleList[DTL_GB18030] == NULL) ||
        (dtsUTF8MbConversionHashList[DTL_GB18030] == NULL) )
    {
        STL_TRY( dtsOpenConversionLibrary( DTL_GB18030, aErrorStack ) == STL_SUCCESS );
    }

    sHash = dtsUTF8MbConversionHashList[DTL_GB18030];

    sUtfString = (stlUInt8*)aSource;
    sGb18030String = (stlUInt8*)aDest;
    sGb18030End = (sGb18030String == NULL) ? sGb18030String : (sGb18030String + aDestLen);

    for( sUtfStringLen = aSourceLen;
         sUtfStringLen > 0;
         sUtfStringLen = sUtfStringLen - sMbLen )
    {
        /* if( *sUtfString == '\0' ) */
        /* { */
        /*     break; */
        /* } */
        /* else */
        /* { */
        /*     /\* Do Nothing *\/ */
        /* } */

        STL_TRY( dtsGetUtf8MbLength( (stlChar *)sUtfString,
                                     &sMbLen,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( dtsUtf8Validate( (stlChar *)sUtfString,
                                  sMbLen,
                                  aErrorStack )
                 == STL_SUCCESS );

        if( sMbLen == 1 )
        {
            /* ASCII 1Byte Code */

            if( sGb18030String < sGb18030End )
            {
                *sGb18030String = *sUtfString;
                sGb18030String++;

                sUtfOffset = sOffset;
                sOffset += sMbLen;
            }

            sTotalGb18030Len++;
            sUtfString++;

            continue;
        }
        else if( sMbLen == 2 )
        {
            sUtfCode = *sUtfString << 8;
            sUtfString++;

            sUtfCode |= *sUtfString;
            sUtfString++;
        }
        else if( sMbLen == 3 )
        {
            sUtfCode = *sUtfString << 16;
            sUtfString++;

            sUtfCode |= *sUtfString << 8;
            sUtfString++;

            sUtfCode |= *sUtfString;
            sUtfString++;
        }
        else if( sMbLen == 4 )
        {
            sUtfCode = *sUtfString << 24;
            sUtfString++;

            sUtfCode |= *sUtfString << 16;
            sUtfString++;

            sUtfCode |= *sUtfString << 8;
            sUtfString++;

            sUtfCode |= *sUtfString;
            sUtfString++;
        }
        else
        {
            STL_DASSERT( STL_FALSE );
            STL_THROW( ERROR_INTERNAL );
        }

        sGb18030Code = sHash[DTS_HASH_BUCKET_IDX(DTS_UTF8_GB18030_HASH_BUCKET,
                                                 sUtfCode,
                                                 DTS_UTF8_GB18030_HASH_MODULUS_CONSTANT,
                                                 DTS_UTF8_GB18030_HASH_MODULUS_SHIFT_BIT,
                                                 DTS_UTF8_GB18030_HASH_ADJUST_CONSTANT)];

        if( sGb18030Code == 0 )
        {
            if( sGb18030String < sGb18030End )
            {
                *sGb18030String = '?';
                sGb18030String++;

                sUtfOffset = sOffset;
                sOffset += sMbLen;
            }

            sTotalGb18030Len++;
        }
        else
        {
            if( sGb18030Code & 0xff000000 )
            {
                if( sGb18030String + 3 < sGb18030End )
                {
                    *sGb18030String = ( sGb18030Code & 0xff000000 ) >> 24;
                    sGb18030String++;
                    *sGb18030String = ( sGb18030Code & 0x00ff0000 ) >> 16;
                    sGb18030String++;
                    *sGb18030String = ( sGb18030Code & 0x0000ff00 ) >> 8;
                    sGb18030String++;
                    *sGb18030String = sGb18030Code & 0x000000ff;
                    sGb18030String++;

                    sUtfOffset = sOffset;
                    sOffset += sMbLen;
                }

                sTotalGb18030Len += 4;
            }
            else if( sGb18030Code & 0x0000ff00 )
            {
                if( sGb18030String + 1 < sGb18030End )
                {
                    *sGb18030String = ( sGb18030Code & 0x0000ff00 ) >> 8;
                    sGb18030String++;
                    *sGb18030String = sGb18030Code & 0x000000ff;
                    sGb18030String++;

                    sUtfOffset = sOffset;
                    sOffset += sMbLen;
                }

                sTotalGb18030Len += 2;
            }
            else if( sGb18030Code & 0x000000ff )
            {
                if( sGb18030String < sGb18030End )
                {
                    *sGb18030String = sGb18030Code & 0x000000ff;
                    sGb18030String++;

                    sUtfOffset = sOffset;
                    sOffset += sMbLen;
                }

                sTotalGb18030Len++;
            }
            else
            {
                STL_DASSERT( STL_FALSE );
                STL_THROW( ERROR_INTERNAL );
            }
        }
    }

    STL_TRY_THROW( sUtfStringLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sGb18030String < sGb18030End )
    {
        sUtfOffset = sOffset;
    }
    
    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sUtfOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalGb18030Len;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionUtf8ToGb18030()" );  
    } 

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief uhc -> ascii 의 conversion수행 
 * @param[in]  aSource       source string(utf8 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUhcToAscii( stlChar         * aSource,
                                   stlInt64          aSourceLen,
                                   stlChar         * aDest,
                                   stlInt64          aDestLen,
                                   stlInt64        * aSourceOffset,
                                   stlInt64        * aTotalLength,
                                   stlErrorStack   * aErrorStack )
{
    stlUInt8  * sSource;
    stlUInt8  * sDest;
    stlInt8     sChr;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalLength = 0;
    stlUInt8  * sDestEnd;
    stlInt8     sMbLen;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    sSource = (stlUInt8 *)aSource;
    sDest   = (stlUInt8 *)aDest;

    sSourceLen = aSourceLen;
    sDestEnd = (sDest == NULL) ? sDest : (sDest + aDestLen);
    
    while( sSourceLen > 0 )
    {
        if( DTS_IS_HIGH_BIT_SET(*sSource) == STL_FALSE )
        {
            sChr = *sSource;

            if( sChr == 0 )
            {
                sChr = '?';
            }

            if( sDest < sDestEnd )
            {
                *sDest = sChr;
                sDest++;

                sSourceOffset = sOffset;
                sOffset++;
            }

            sTotalLength++;

            sSource++;
            sSourceLen--;
        }
        else
        {
            STL_TRY( dtlGetMbLength( DTL_UHC,
                                     (stlChar*)sSource,
                                     &sMbLen,
                                     aErrorStack ) == STL_SUCCESS );
                                     
            if( sDest < sDestEnd )
            {
                *sDest = '?';
                sDest++;

                sSourceOffset = sOffset;
                sOffset += sMbLen;
            }

            sTotalLength++;

            sSource += sMbLen;
            sSourceLen -= sMbLen;
        }
    }

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalLength;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief uhc(aliase ms949) -> utf8 의 conversion수행 
 * @param[in]  aSource       source string(uhc code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 * @todo 이후 character set 추가시,
 *       <BR> combined characters가 필요한 character set인 경우,
 *       <BR> 이에 대한 코드가 추가 되어야 함. 
 */
stlStatus dtsConversionUhcToUtf8( stlChar         * aSource,
                                  stlInt64          aSourceLen,
                                  stlChar         * aDest,
                                  stlInt64          aDestLen,
                                  stlInt64        * aSourceOffset,
                                  stlInt64        * aTotalLength,
                                  stlErrorStack   * aErrorStack )
{
    stlUInt8  * sUhcString;
    stlUInt8  * sUtfString;
    stlInt64    sUhcStringLen;
    stlInt8     sMbLen;
    stlUInt32   sUhcCode = 0;
    stlUInt32   sUtfCode = 0;
    stlUInt8  * sUtfEnd;
    stlInt64    sUhcOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalUtfLen = 0;
    
    stlUInt32 * sHash = NULL;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( (dtsMbConversionHandleList[DTL_UHC] == NULL) ||
        (dtsMbUTF8ConversionHashList[DTL_UHC] == NULL) )
    {
        STL_TRY( dtsOpenConversionLibrary( DTL_UHC, aErrorStack ) == STL_SUCCESS );
    }

    sHash = dtsMbUTF8ConversionHashList[DTL_UHC];
    
    sUhcString = (stlUInt8*)aSource;
    sUtfString = (stlUInt8*)aDest;
    sUtfEnd = (sUtfString == NULL) ? sUtfString : (sUtfString + aDestLen);

    for( sUhcStringLen = aSourceLen;
         sUhcStringLen > 0;
         sUhcStringLen = sUhcStringLen - sMbLen )
    {
        /* if( *sUhcString == '\0' ) */
        /* { */
        /*     break; */
        /* } */
        /* else */
        /* { */
        /*     /\* Do Nothing *\/ */
        /* } */

        if( DTS_IS_HIGH_BIT_SET(*sUhcString) == STL_FALSE )
        {
            /* ASCII 1Byte Code */

            sMbLen = 1;

            if( sUtfString < sUtfEnd )
            {
                *sUtfString = *sUhcString;
                sUtfString++;

                sUhcOffset = sOffset;
                sOffset++;
            }

            sTotalUtfLen++;
            sUhcString++;

            continue;
        }
        else
        {
            /* 2Byte ~ */

            if( dtlMbStrVerify( DTL_UHC,
                                (stlChar *)sUhcString,
                                sUhcStringLen,
                                &sMbLen,
                                aErrorStack ) != STL_SUCCESS )
            {
                sMbLen = 1;

                if( sUtfString < sUtfEnd )
                {
                    *sUtfString = '?';
                    sUtfString++;

                    sUhcOffset = sOffset;
                    sOffset++;
                }

                sTotalUtfLen++;
                sUhcString++;

                continue;
            }

            if( sMbLen == 1 )
            {
                sUhcCode = *sUhcString;
                sUhcString++;
            }
            else if( sMbLen == 2 )
            {
                sUhcCode = *sUhcString << 8;
                sUhcString++;

                sUhcCode |= *sUhcString;
                sUhcString++;
            }
            else
            {
                STL_DASSERT( STL_FALSE );
                STL_THROW( ERROR_INTERNAL );
            }

            sUtfCode = sHash[DTS_HASH_BUCKET_IDX(DTS_UHC_UTF8_HASH_BUCKET,
                                                 sUhcCode,
                                                 DTS_UHC_UTF8_HASH_MODULUS_CONSTANT,
                                                 DTS_UHC_UTF8_HASH_MODULUS_SHIFT_BIT,
                                                 DTS_UHC_UTF8_HASH_ADJUST_CONSTANT) ];

            if( sUtfCode == 0 )
            {
                if( sUtfString < sUtfEnd )
                {
                    *sUtfString = '?';
                    sUtfString++;

                    sUhcOffset = sOffset;
                    sOffset += sMbLen;
                }

                sTotalUtfLen++;
            }
            else
            {
                if( sUtfCode & 0xff000000 )
                {
                    if( sUtfString + 3 < sUtfEnd )
                    {
                        *sUtfString = sUtfCode >> 24;
                        sUtfString++;
                        *sUtfString = ( sUtfCode & 0x00ff0000 ) >> 16;
                        sUtfString++;
                        *sUtfString = ( sUtfCode & 0x0000ff00 ) >> 8;
                        sUtfString++;
                        *sUtfString = sUtfCode & 0x000000ff;
                        sUtfString++;

                        sUhcOffset = sOffset;
                        sOffset += sMbLen;
                    }

                    sTotalUtfLen += 4;
                }
                else if( sUtfCode & 0x00ff0000 )
                {
                    if( sUtfString + 2 < sUtfEnd )
                    {
                        *sUtfString = ( sUtfCode & 0x00ff0000 ) >> 16;
                        sUtfString++;
                        *sUtfString = ( sUtfCode & 0x0000ff00 ) >> 8;
                        sUtfString++;
                        *sUtfString = sUtfCode & 0x000000ff;
                        sUtfString++;

                        sUhcOffset = sOffset;
                        sOffset += sMbLen;
                    }

                    sTotalUtfLen += 3;
                }
                else if( sUtfCode & 0x0000ff00 )
                {
                    if( sUtfString + 1 < sUtfEnd )
                    {
                        *sUtfString = ( sUtfCode & 0x0000ff00 ) >> 8;
                        sUtfString++;
                        *sUtfString = sUtfCode & 0x000000ff;
                        sUtfString++;

                        sUhcOffset = sOffset;
                        sOffset += sMbLen;
                    }

                    sTotalUtfLen += 2;
                }
                else if( sUtfCode & 0x000000ff )
                {
                    if( sUtfString < sUtfEnd )
                    {
                        *sUtfString = sUtfCode & 0x000000ff;
                        sUtfString++;

                        sUhcOffset = sOffset;
                        sOffset += sMbLen;
                    }

                    sTotalUtfLen ++;
                }
                else
                {
                    STL_DASSERT( STL_FALSE );
                    STL_THROW( ERROR_INTERNAL );
                }  
            }
            
        }
    }

    STL_TRY_THROW( sUhcStringLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sUtfString < sUtfEnd )
    {
        sUhcOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sUhcOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUtfLen;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionUhcToUtf8()" );  
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief gb18030 -> ascii 의 conversion수행 
 * @param[in]  aSource       source string(gb18030 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionGb18030ToAscii( stlChar         * aSource,
                                       stlInt64          aSourceLen,
                                       stlChar         * aDest,
                                       stlInt64          aDestLen,
                                       stlInt64        * aSourceOffset,
                                       stlInt64        * aTotalLength,
                                       stlErrorStack   * aErrorStack )
{
    stlUInt8  * sSource;
    stlUInt8  * sDest;
    stlInt8     sChr;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalLength = 0;
    stlUInt8  * sDestEnd;
    stlInt8     sMbLen;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    sSource = (stlUInt8 *)aSource;
    sDest   = (stlUInt8 *)aDest;

    sSourceLen = aSourceLen;
    sDestEnd = (sDest == NULL) ? sDest : (sDest + aDestLen);
    
    while( sSourceLen > 0 )
    {
        if( DTS_IS_HIGH_BIT_SET(*sSource) == STL_FALSE )
        {
            sChr = *sSource;

            if( sChr == 0 )
            {
                sChr = '?';
            }

            if( sDest < sDestEnd )
            {
                *sDest = sChr;
                sDest++;

                sSourceOffset = sOffset;
                sOffset++;
            }

            sTotalLength++;

            sSource++;
            sSourceLen--;
        }
        else
        {
            STL_TRY( dtlGetMbLength( DTL_GB18030,
                                     (stlChar*)sSource,
                                     &sMbLen,
                                     aErrorStack ) == STL_SUCCESS );
                                     
            if( sDest < sDestEnd )
            {
                *sDest = '?';
                sDest++;

                sSourceOffset = sOffset;
                sOffset += sMbLen;
            }

            sTotalLength++;

            sSource += sMbLen;
            sSourceLen -= sMbLen;
        }
    }

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalLength;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief gb18030 -> utf8 의 conversion수행 
 * @param[in]  aSource       source string(gb18030 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 * @todo 이후 character set 추가시,
 *       <BR> combined characters가 필요한 character set인 경우,
 *       <BR> 이에 대한 코드가 추가 되어야 함. 
 */
stlStatus dtsConversionGb18030ToUtf8( stlChar         * aSource,
                                      stlInt64          aSourceLen,
                                      stlChar         * aDest,
                                      stlInt64          aDestLen,
                                      stlInt64        * aSourceOffset,
                                      stlInt64        * aTotalLength,
                                      stlErrorStack   * aErrorStack )
{
    stlUInt8  * sGb18030String;
    stlUInt8  * sUtfString;
    stlInt64    sGb18030StringLen;
    stlInt8     sMbLen;
    stlUInt32   sGb18030Code = 0;
    stlUInt32   sUtfCode = 0;
    stlUInt8  * sUtfEnd;
    stlInt64    sGb18030Offset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalUtfLen = 0;
    
    stlUInt32 * sHash = NULL;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( (dtsMbConversionHandleList[DTL_GB18030] == NULL) ||
        (dtsMbUTF8ConversionHashList[DTL_GB18030] == NULL) )
    {
        STL_TRY( dtsOpenConversionLibrary( DTL_GB18030, aErrorStack ) == STL_SUCCESS );
    }

    sHash = dtsMbUTF8ConversionHashList[DTL_GB18030];
    
    sGb18030String = (stlUInt8*)aSource;
    sUtfString = (stlUInt8*)aDest;
    sUtfEnd = (sUtfString == NULL) ? sUtfString : (sUtfString + aDestLen);
    
    for( sGb18030StringLen = aSourceLen;
         sGb18030StringLen > 0;
         sGb18030StringLen = sGb18030StringLen - sMbLen )
    {
        /* if( *sGb18030String == '\0' ) */
        /* { */
        /*     break; */
        /* } */
        /* else */
        /* { */
        /*     /\* Do Nothing *\/ */
        /* } */

        if( DTS_IS_HIGH_BIT_SET(*sGb18030String) == STL_FALSE )
        {
            /* ASCII 1Byte Code */

            sMbLen = 1;

            if( sUtfString < sUtfEnd )
            {
                *sUtfString = *sGb18030String;
                sUtfString++;

                sGb18030Offset = sOffset;
                sOffset++;
            }

            sTotalUtfLen++;
            sGb18030String++;

            continue;
        }
        else
        {
            /* 2Byte ~ */

            if( dtlMbStrVerify( DTL_GB18030,
                                (stlChar *)sGb18030String,
                                sGb18030StringLen,
                                &sMbLen,
                                aErrorStack ) != STL_SUCCESS )
            {
                sMbLen = 1;

                if( sUtfString < sUtfEnd )
                {
                    *sUtfString = '?';
                    sUtfString++;

                    sGb18030Offset = sOffset;
                    sOffset++;
                }

                sTotalUtfLen++;
                sGb18030String++;

                continue;
            }

            if( sMbLen == 1 )
            {
                sGb18030Code = *sGb18030String;
                sGb18030String++;
            }
            else if( sMbLen == 2 )
            {
                sGb18030Code = *sGb18030String << 8;
                sGb18030String++;

                sGb18030Code |= *sGb18030String;
                sGb18030String++;
            }
            else if( sMbLen == 4 )
            {
                sGb18030Code = *sGb18030String << 24;
                sGb18030String++;

                sGb18030Code |= *sGb18030String << 16;
                sGb18030String++;

                sGb18030Code |= *sGb18030String << 8;
                sGb18030String++;

                sGb18030Code |= *sGb18030String;
                sGb18030String++;
            }
            else
            {
                STL_DASSERT( STL_FALSE );
                STL_THROW( ERROR_INTERNAL );
            }

            sUtfCode = sHash[DTS_HASH_BUCKET_IDX(DTS_GB18030_UTF8_HASH_BUCKET,
                                                 sGb18030Code,
                                                 DTS_GB18030_UTF8_HASH_MODULUS_CONSTANT,
                                                 DTS_GB18030_UTF8_HASH_MODULUS_SHIFT_BIT,
                                                 DTS_GB18030_UTF8_HASH_ADJUST_CONSTANT) ];

            if( sUtfCode == 0 )
            {
                if( sUtfString < sUtfEnd )
                {
                    *sUtfString = '?';
                    sUtfString++;

                    sGb18030Offset = sOffset;
                    sOffset += sMbLen;
                }

                sTotalUtfLen++;
            }
            else
            {
                if( sUtfCode & 0xff000000 )
                {
                    if( sUtfString + 3 < sUtfEnd )
                    {
                        *sUtfString = sUtfCode >> 24;
                        sUtfString++;
                        *sUtfString = ( sUtfCode & 0x00ff0000 ) >> 16;
                        sUtfString++;
                        *sUtfString = ( sUtfCode & 0x0000ff00 ) >> 8;
                        sUtfString++;
                        *sUtfString = sUtfCode & 0x000000ff;
                        sUtfString++;

                        sGb18030Offset = sOffset;
                        sOffset += sMbLen;
                    }

                    sTotalUtfLen += 4;
                }
                else if( sUtfCode & 0x00ff0000 )
                {
                    if( sUtfString + 2 < sUtfEnd )
                    {
                        *sUtfString = ( sUtfCode & 0x00ff0000 ) >> 16;
                        sUtfString++;
                        *sUtfString = ( sUtfCode & 0x0000ff00 ) >> 8;
                        sUtfString++;
                        *sUtfString = sUtfCode & 0x000000ff;
                        sUtfString++;

                        sGb18030Offset = sOffset;
                        sOffset += sMbLen;
                    }

                    sTotalUtfLen += 3;
                }
                else if( sUtfCode & 0x0000ff00 )
                {
                    if( sUtfString + 1 < sUtfEnd )
                    {
                        *sUtfString = ( sUtfCode & 0x0000ff00 ) >> 8;
                        sUtfString++;
                        *sUtfString = sUtfCode & 0x000000ff;
                        sUtfString++;

                        sGb18030Offset = sOffset;
                        sOffset += sMbLen;
                    }

                    sTotalUtfLen += 2;
                }
                else if( sUtfCode & 0x000000ff )
                {
                    if( sUtfString < sUtfEnd )
                    {
                        *sUtfString = sUtfCode & 0x000000ff;
                        sUtfString++;

                        sGb18030Offset = sOffset;
                        sOffset += sMbLen;
                    }

                    sTotalUtfLen ++;
                }
                else
                {
                    STL_DASSERT( STL_FALSE );
                    STL_THROW( ERROR_INTERNAL );
                }  
            }
            
        }
    }

    STL_TRY_THROW( sGb18030StringLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sUtfString < sUtfEnd )
    {
        sGb18030Offset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sGb18030Offset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUtfLen;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionGb18030ToUtf8()" );  
    }  

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ascii -> dtl 로의 실제 변환함수
 * @param[in]  aSource       source string(ascii code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack  aErrorStack
 */
stlStatus dtsAsciiToMic( stlChar       * aSource,
                         stlInt64        aSourceLen,
                         stlChar       * aDest,
                         stlInt64        aDestLen,
                         stlInt64      * aSourceOffset,
                         stlInt64      * aTotalLength,
                         stlErrorStack * aErrorStack )
{
    stlUInt8  * sSource;
    stlUInt8  * sDest;
    stlInt8     sChr;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalLength = 0;
    stlUInt8  * sDestEnd;

    sSource = (stlUInt8*)aSource;
    sDest   = (stlUInt8*)aDest;

    sSourceLen = aSourceLen;
    sDestEnd = (sDest == NULL) ? sDest : (sDest + aDestLen);

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );
    
    while( sSourceLen > 0 )
    {
        sChr = *sSource;

        if( (sChr == 0) || (DTS_IS_HIGH_BIT_SET(sChr) == STL_TRUE) )
        {
            sChr = '?';
        }

        if( sDest < sDestEnd )
        {
            *sDest = sChr;
            sDest++;

            sSourceOffset = sOffset;
            sOffset++;
        }

        sTotalLength++;
            
        sSource++;
        sSourceLen--;
    }

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalLength;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief dtl -> ascii 로의 실제 변환함수
 * @param[in]  aSource       source string(dtl code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsMicToAscii( stlChar       * aSource,
                         stlInt64        aSourceLen,
                         stlChar       * aDest,
                         stlInt64        aDestLen,
                         stlInt64      * aSourceOffset,
                         stlInt64      * aTotalLength,
                         stlErrorStack * aErrorStack )
{
    stlUInt8  * sSource;
    stlUInt8  * sDest;
    stlInt8     sChr;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalLength = 0;
    stlUInt8  * sDestEnd;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    sSource = (stlUInt8 *)aSource;
    sDest   = (stlUInt8 *)aDest;

    sSourceLen = aSourceLen;
    sDestEnd = (sDest == NULL) ? sDest : (sDest + aDestLen);
    
    while( sSourceLen > 0 )
    {
        sChr = *sSource;

        if( (sChr == 0) || (DTS_IS_HIGH_BIT_SET(sChr) == STL_TRUE ) )
        {
            sChr = '?';
        }

        if( sDest < sDestEnd )
        {
            *sDest = sChr;
            sDest++;

            sSourceOffset = sOffset;
            sOffset++;
        }

        sTotalLength++;

        sSource++;
        sSourceLen--;
    }

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalLength;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief utf16 한문자를 utf8 로 변환한다.
 * @param[in]  aSource               source( utf16 ) 문자
 * @param[in]  aSourceLen            source( utf16 ) 문자 길이
 * @param[out] aDest                 dest( utf8 )를 저장할 메모리 주소  
 * @param[in]  aDestLen              dest( utf8 )를 저장할 메모리 크기
 * @param[out] aSourceConvertedLen   변환이 성공한 @a aSource의 byte length
 * @param[out] aDestConvertedLen     @a aDest의 공간이 충분할 경우 변환이 성공한 @a aDest의 byte length
 * @param[in]  aErrorStack           stlErrorStack
 */
stlStatus dtsUtf16ToUtf8( void           * aSource,
                          stlInt64         aSourceLen,
                          void           * aDest,
                          stlInt64         aDestLen,
                          stlInt64       * aSourceConvertedLen,
                          stlInt64       * aDestConvertedLen,
                          stlErrorStack  * aErrorStack )
{
    stlUInt16  * sSource;
    stlUInt32    sUnicode = 0xFFFFFFFF;
    stlInt64     sUnicodeLen = 0;
    
    DTL_PARAM_VALIDATE( aSource != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aSourceConvertedLen != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aDestConvertedLen != NULL, aErrorStack );

    /**
     * utf16 을  Unicode( utf32 ) 로 변경
     */

    sSource = (stlUInt16 *)aSource;

    /* utf16의 문자길이 체크 */
    DTS_GET_UTF16_MBLENGTH( sSource, aSourceConvertedLen );
    STL_TRY_THROW( aSourceLen >= *aSourceConvertedLen, ERROR_CHARACTER_NOT_IN_REPERTOIRE );    
    
    if( *aSourceConvertedLen == 2 )
    {
        sUnicode = sSource[0];
    }
    else
    {
        STL_DASSERT( *aSourceConvertedLen == 4 );

        /* if( ( ( sSource[0] >= 0xD800 ) && ( sSource[0] <= 0xDBFF ) ) && */
        /*     ( ( sSource[1] >= 0xDC00 ) && ( sSource[1] <= 0xDFFF ) ) ) */
        /* sSource[0] 은 위에서 DTS_GET_UTF16_MBLENGTH()에서 검사됨. */
        if( ( sSource[1] >= 0xDC00 ) && ( sSource[1] <= 0xDFFF ) )
        {
            /*
             * 보충문자
             */

            /* 
             * High Surrogates 계산후, 상위 자리 이동 배치
             * ( 1024 * 1024 ) 
             *  ^^^^^^
             */
            sUnicode = ( sSource[0] - 0xD800 ) << 10;
            /*
             * Low Surrogates 계산후, 하위 자리 배치
             * ( 1024 * 1024 )
             *         ^^^^^^
             */
             sUnicode = sUnicode + ( sSource[1] - 0xDC00 );

             /* 
              * 보충언어판은 0x10000 부터 시작함.
              * ( 0x10000 ~ 0x10FFFF )
              */
             sUnicode = sUnicode + 0x10000;
        }
        else
        {
            STL_THROW( ERROR_CHARACTER_NOT_IN_REPERTOIRE );
        }
    }
    
    /*
     * Unicode ( utf32 ) 는
     * 0x00000000 ~ 0x0010FFFF 의 범위를 가진다.
     */
    STL_DASSERT( (sUnicode >= 0x00000000) &&
                 (sUnicode <= 0x0010FFFF) );
    
    STL_TRY_THROW( (sUnicode >= 0x00000000) &&
                   (sUnicode <= 0x0010FFFF),
                   ERROR_CHARACTER_NOT_IN_REPERTOIRE );
    
    /**
     * utf32 를  utf8 로 변경
     */
    
    STL_TRY( dtsUtf32ToUtf8( & sUnicode,
                             STL_UINT32_SIZE,
                             aDest,
                             aDestLen,
                             & sUnicodeLen,
                             aDestConvertedLen,
                             aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief utf32 를 utf8 로 변환한다.
 * @param[in]  aSource               source( utf32 ) 문자
 * @param[in]  aSourceLen            source( utf32 ) 문자 길이
 * @param[out] aDest                 dest( utf8 )를 저장할 메모리 주소  
 * @param[in]  aDestLen              dest( utf8 )를 저장할 메모리 크기
 * @param[out] aSourceConvertedLen   변환이 성공한 @a aSource의 byte length
 * @param[out] aDestConvertedLen     @a aDest의 공간이 충분할 경우 변환이 성공한 @a aDest의 byte length
 * @param[in]  aErrorStack           stlErrorStack
 * @remark
 *  <table>
 *    <tr>
 *      <td> 코드 범위(십육진법) </td>
 *      <td> UTF-8 표현(이진법) </td>
 *      <td> 설명 </td>
 *    </tr>
 *    <tr>
 *      <td> 00000000-0000007F </td>
 *      <td> 0xxxxxxx </td>
 *      <td> 1byte, ASCII와 동일한 범위 </td>
 *    </tr>
 *    <tr>
 *      <td> 00000080-000007FF </td>
 *      <td> 110xxxxx 10xxxxxx </td>
 *      <td> 2byte, 첫 바이트는 110 으로 시작하고, 나머지 바이트들은 10으로 시작함 </td>
 *    </tr>
 *    <tr>
 *      <td> 00000800-0000FFFF </td>
 *      <td> 1110xxxx 10xxxxxx 10xxxxxx </td>
 *      <td> 3byte, 첫 바이트는 1110으로 시작하고, 나머지 바이트들은 10으로 시작함 </td>
 *    </tr>
 *    <tr>
 *      <td> 00010000-0010FFFF </td>
 *      <td> 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx </td>
 *      <td> 4byte, 첫 바이트는 11110으로 시작하고, 나머지 바이트들은 10으로 시작함 </td>
 *    </tr>
 *  </table>
 */
stlStatus dtsUtf32ToUtf8( void           * aSource,
                          stlInt64         aSourceLen,
                          void           * aDest,
                          stlInt64         aDestLen,
                          stlInt64       * aSourceConvertedLen,
                          stlInt64       * aDestConvertedLen,
                          stlErrorStack  * aErrorStack )
{
    stlUInt32  * sSource;
    stlUInt8   * sDest;
    stlUInt8   * sDestEnd;

    DTL_PARAM_VALIDATE( aSource != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aSourceConvertedLen != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aDestConvertedLen != NULL, aErrorStack );

    sSource = (stlUInt32 *)aSource;
    sDest = (stlUInt8 *)aDest;
    sDestEnd = (sDest == NULL) ? sDest : (sDest + aDestLen);

    *aSourceConvertedLen = 0;
    *aDestConvertedLen = 0; 

    if( sSource[0] <= 0x7F )
    {
      /*
       * 1 byte 문자
       *
       * (Unicode)              (utf8)
       * 00000000-0000007F  =>  0xxxxxxx
       */

        *aDestConvertedLen = 1;

        if( sDest + *aDestConvertedLen <= sDestEnd )
        {
            sDest[0] = sSource[0];

            *aSourceConvertedLen = STL_UINT32_SIZE;
        }
        /* else */
        /* { */
        /*     STL_THROW( ERROR_NOT_ENOUGH_BUFFER ); */
        /* } */
    }
    else if( sSource[0] <= 0x7FF )
    {
      /*
       * 2 byte 문자
       *
       * (Unicode)              (utf8)
       * 00000080-000007FF  =>  110xxxxx 10xxxxxx
       */

        *aDestConvertedLen = 2;

        if( sDest + *aDestConvertedLen <= sDestEnd )
        {
            sDest[0] = 0xC0 | ( (sSource[0] >> 6) & 0x1F );
            sDest[1] = 0x80 | ( sSource[0] & 0x3F );

            *aSourceConvertedLen = STL_UINT32_SIZE;
        }
        /* else */
        /* { */
        /*     STL_THROW( ERROR_NOT_ENOUGH_BUFFER ); */
        /* } */
    }
    else if( sSource[0] <= 0xFFFF )
    {
      /*
       * 3 byte 문자
       *
       * (Unicode)              (utf8)
       * 00000800-0000FFFF  =>  1110xxxx 10xxxxxx 10xxxxxx
       */
        *aDestConvertedLen = 3;

        if( sDest + *aDestConvertedLen <= sDestEnd )
        {
            sDest[0] = 0xE0 | ( (sSource[0] >> 12) & 0x0F ) ;
            sDest[1] = 0x80 | ( (sSource[0] >> 6) & 0x3F );
            sDest[2] = 0x80 | ( sSource[0] & 0x3F );

            *aSourceConvertedLen = STL_UINT32_SIZE;
        }
        /* else */
        /* { */
        /*     STL_THROW( ERROR_NOT_ENOUGH_BUFFER ); */
        /* } */
    }
    else
    {
      /*
       * 4 byte 문자
       *
       * (Unicode)              (utf8)
       * 00010000-0010FFFF  =>  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
       */
        *aDestConvertedLen = 4;

        if( sDest + *aDestConvertedLen <= sDestEnd )
        {
            sDest[0] = 0xF0 | ( (sSource[0] >> 18) & 0x07 ) ;
            sDest[1] = 0x80 | ( (sSource[0] >> 12) & 0x3F );
            sDest[2] = 0x80 | ( (sSource[0] >> 6) & 0x3F );
            sDest[3] = 0x80 | ( sSource[0] & 0x3F );

            *aSourceConvertedLen = STL_UINT32_SIZE;
        }
        /* else */
        /* { */
        /*     STL_THROW( ERROR_NOT_ENOUGH_BUFFER ); */
        /* } */
    }

    return STL_SUCCESS;

    /* STL_CATCH( ERROR_NOT_ENOUGH_BUFFER ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION, */
    /*                   NULL, */
    /*                   aErrorStack );   */
    /* } */

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief utf8 을 utf16 으로 변환한다.
 * @param[in]  aSource               source( utf8 ) 문자
 * @param[in]  aSourceLen            source( utf8 ) 문자 길이
 * @param[out] aDest                 dest( utf16 )를 저장할 메모리 주소  
 * @param[in]  aDestLen              dest( utf16 )를 저장할 메모리 크기
 * @param[out] aSourceConvertedLen   변환이 성공한 @a aSource의 byte length
 * @param[out] aDestConvertedLen     @a aDest의 공간이 충분할 경우 변환이 성공한 @a aDest의 byte length
 * @param[in]  aErrorStack           stlErrorStack
 */
stlStatus dtsUtf8ToUtf16( void           * aSource,
                          stlInt64         aSourceLen,
                          void           * aDest,
                          stlInt64         aDestLen,
                          stlInt64       * aSourceConvertedLen,
                          stlInt64       * aDestConvertedLen,
                          stlErrorStack  * aErrorStack )
{
    stlUInt16  * sDest; 
    stlUInt16  * sDestEnd;
    stlUInt32    sUnicode = 0xFFFFFFFF;
    stlInt64     sUnicodeLen = 0;

    DTL_PARAM_VALIDATE( aSource != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aSourceConvertedLen != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aDestConvertedLen != NULL, aErrorStack );

    sDest = (stlUInt16 *)aDest;
    sDestEnd = (sDest == NULL) ? sDest : (sDest + (aDestLen/STL_UINT16_SIZE));

    /*
     * utf8 문자를 unicode ( utf32 ) 로 변경한다.
     */

    STL_TRY( dtsUtf8ToUtf32( aSource,
                             aSourceLen,
                             (void *)(& sUnicode),
                             STL_UINT32_SIZE,
                             aSourceConvertedLen,
                             & sUnicodeLen,
                             aErrorStack )
             == STL_SUCCESS );

    STL_DASSERT( *aSourceConvertedLen > 0 );

    /*
     * utf32 문자를 utf16 문자로 변경한다.
     */

    /*
     * 변환
     */
    if( sUnicode <= 0xFFFF )
    {
        *aDestConvertedLen = 2;

        if( sDest + 1 <= sDestEnd )
        {
            sDest[0] = sUnicode;
        }
        else
        {
            *aSourceConvertedLen = 0;
            /* STL_THROW( ERROR_NOT_ENOUGH_BUFFER ); */
        }
    }
    else
    {
        *aDestConvertedLen = 4;

        if( sDest + 2 <= sDestEnd )
        {
            /* 
             * 보충언어판은 0x10000 부터 시작함.
             * ( 0x10000 ~ 0x10FFFF )
             */
            sUnicode = sUnicode - 0x10000;

            sDest[0] = ( sUnicode >> 10 ) + 0xD800;
            sDest[1] = ( sUnicode & 0x3FF ) + 0xDC00;
        }
        else
        {
            *aSourceConvertedLen = 0;
            /* STL_THROW( ERROR_NOT_ENOUGH_BUFFER ); */
        }
    }

    return STL_SUCCESS;

    /* STL_CATCH( ERROR_NOT_ENOUGH_BUFFER ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION, */
    /*                   NULL, */
    /*                   aErrorStack );   */
    /* } */

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief utf8 을 utf32 로 변환한다.
 * @param[in]  aSource               source( utf32 ) 문자
 * @param[in]  aSourceLen            source( utf32 ) 문자 길이
 * @param[out] aDest                 dest( utf8 )를 저장할 메모리 주소  
 * @param[in]  aDestLen              dest( utf8 )를 저장할 메모리 크기
 * @param[out] aSourceConvertedLen   변환이 성공한 @a aSource의 byte length
 * @param[out] aDestConvertedLen     @a aDest의 공간이 충분할 경우 변환이 성공한 @a aDest의 byte length
 * @param[in]  aErrorStack           stlErrorStack
 */
stlStatus dtsUtf8ToUtf32( void           * aSource,
                          stlInt64         aSourceLen,
                          void           * aDest,
                          stlInt64         aDestLen,
                          stlInt64       * aSourceConvertedLen,
                          stlInt64       * aDestConvertedLen,
                          stlErrorStack  * aErrorStack )
{
    stlUInt8  * sSource;
    stlUInt32 * sDest;
    stlUInt32 * sDestEnd;
    stlInt8     sMbLength = 0;

    DTL_PARAM_VALIDATE( aSource != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aSourceConvertedLen != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aDestConvertedLen != NULL, aErrorStack );

    sSource = (stlUInt8 *)aSource;
    sDest = (stlUInt32 *)aDest;
    sDestEnd = (sDest == NULL) ? sDest : (sDest + (aDestLen/STL_UINT32_SIZE));


    /* utf8의 문자길이 체크 */
    STL_TRY( dtsGetUtf8MbLength( (stlChar *)sSource,
                                 &sMbLength,
                                 aErrorStack ) == STL_SUCCESS );
    STL_TRY_THROW( aSourceLen >= sMbLength,
                   ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sDest < sDestEnd )
    {
        if( sMbLength == 1 )
        {
            /*
             * 1 BYTE 문자
             *
             * (UTF8)        (UNICODE)
             * 0XXXXXXX  =>  00000000-0000007F
             */
            *sDest = sSource[0];
        }
        else if( sMbLength == 2 )
        {
            /*
             * 2 BYTE 문자
             *
             * (UTF8)                 (UNICODE)
             * 110XXXXX 10XXXXXX  =>  00000080-000007FF
             */

            *sDest = ( (sSource[0] & 0x1F) << 6 ) | 
                     ( (sSource[1] & 0x3F) );      
       
        }
        else if( sMbLength == 3 )
        {
            /*
             * 3 byte 문자
             *
             * (utf8)                          (Unicode)
             * 1110xxxx 10xxxxxx 10xxxxxx  =>  00000800-0000FFFF
             */

            *sDest = ( (sSource[0] & 0x0F) << 12 ) |
                     ( (sSource[1] & 0x3F) << 6 ) |
                     ( (sSource[2] & 0x3F) );

        }
        else
        {
            /*
             * 4 byte 문자
             *
             * (utf8)                                   (Unicode)
             * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  =>  00010000-0010FFFF
             */

            STL_DASSERT( sMbLength == 4 );

            *sDest = ( (sSource[0] & 0x07) << 18 ) |
                     ( (sSource[1] & 0x3F) << 12 ) |
                     ( (sSource[2] & 0x3F) << 6 ) |
                     ( (sSource[3] & 0x3F) );
        }

        *aSourceConvertedLen = sMbLength;
    }
    else
    {
        *aSourceConvertedLen = 0;
        /* STL_THROW( ERROR_NOT_ENOUGH_BUFFER ); */
    }

    *aDestConvertedLen = STL_UINT32_SIZE;

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }

    /* STL_CATCH( ERROR_NOT_ENOUGH_BUFFER ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION, */
    /*                   NULL, */
    /*                   aErrorStack );   */
    /* } */

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief unicode -> ascii 의 conversion수행 
 * @param[in]  aUniEncoding  source 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aSource       source string(utf16, utf32의 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUnicodeToAscii( dtlUnicodeEncoding   aUniEncoding,
                                       void               * aSource,
                                       stlInt64             aSourceLen,
                                       stlChar            * aDest,
                                       stlInt64             aDestLen,
                                       stlInt64           * aSourceOffset,
                                       stlInt64           * aTotalLength,
                                       stlErrorStack      * aErrorStack )
{
    void       * sSource;
    stlUInt8   * sDest;
    stlInt8      sChr;
    stlInt64     sSourceLen;
    stlInt64     sSourceOffset = 0;
    stlInt64     sOffset = 0;
    stlInt64     sTotalLength = 0;
    stlUInt8   * sDestEnd;

    stlChar      sUtf8Buf[DTS_GET_UTF8_MB_MAX_LENGTH];
    stlUInt8   * sUtf8;
    stlInt64     sSourceConvertedLen = 0;
    stlInt64     sUtf8ConvertedLen = 0;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    sSource = aSource;
    sUtf8   = (stlUInt8 *)sUtf8Buf;
    sDest   = (stlUInt8 *)aDest;

    sSourceLen = aSourceLen;
    sDestEnd = (sDest == NULL) ? sDest : (sDest + aDestLen);

    while( sSourceLen > 0 )
    {
        /*
         * unicode -> utf8 로 변경
         */

        STL_TRY( dtsUnicodeConversionFuncList[aUniEncoding][DTL_UNICODE_UTF8]( sSource,
                                                                               sSourceLen,
                                                                               (void *)sUtf8,
                                                                               DTS_GET_UTF8_MB_MAX_LENGTH,
                                                                               & sSourceConvertedLen,
                                                                               & sUtf8ConvertedLen,
                                                                               aErrorStack )
                 == STL_SUCCESS );

        STL_DASSERT( sSourceConvertedLen > 0 );

        /*
         * utf8 -> ascii 로 변경
         */

        if( DTS_IS_HIGH_BIT_SET(*sUtf8) == STL_FALSE )
        {
            sChr = *sUtf8;

            if( sChr == 0 )
            {
                sChr = '?';
            }
        }
        else
        {
            sChr = '?';
        }

        if( sDest < sDestEnd )
        {
            *sDest = sChr;
            sDest++;
            
            sSourceOffset = sOffset;
            sOffset = sOffset + sSourceConvertedLen;
        }

        sTotalLength++;

        sSource = (stlChar*)sSource + sSourceConvertedLen;
        sSourceLen = sSourceLen - sSourceConvertedLen;
    }

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalLength;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief unicode -> utf8 의 conversion수행 
 * @param[in]  aUniEncoding  source 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aSource       source string(utf16, utf32의 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUnicodeToUtf8( dtlUnicodeEncoding   aUniEncoding,
                                      void               * aSource,
                                      stlInt64             aSourceLen,
                                      stlChar            * aDest,
                                      stlInt64             aDestLen,
                                      stlInt64           * aSourceOffset,
                                      stlInt64           * aTotalLength,
                                      stlErrorStack      * aErrorStack )
{
    void       * sSourceString;
    stlUInt8   * sDest;
    stlUInt8   * sDestEnd;
    stlInt64     sSourceStringLen;
    stlInt64     sSourceOffset = 0;
    stlInt64     sOffset = 0;
    stlInt64     sTotalUtfLen = 0;

    stlChar      sUtf8Buf[DTS_GET_UTF8_MB_MAX_LENGTH];
    stlUInt8   * sUtf8;
    stlInt64     sSourceConvertedLen = 0;
    stlInt64     sUtf8ConvertedLen = 0;
    stlInt64     i = 0;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    sSourceString = aSource;
    sUtf8 = (stlUInt8 *)sUtf8Buf;
    sDest = (stlUInt8 *)aDest;
    sDestEnd = (sDest == NULL) ? sDest : (sDest + aDestLen);

    for( sSourceStringLen = aSourceLen;
         sSourceStringLen > 0;
         sSourceStringLen = sSourceStringLen - sSourceConvertedLen )
    {
        /*
         * aUniEncoding -> utf8 로 변경
         */

        STL_TRY( dtsUnicodeConversionFuncList[aUniEncoding][DTL_UNICODE_UTF8]( sSourceString,
                                                                               sSourceStringLen,
                                                                               (void *)sUtf8,
                                                                               DTS_GET_UTF8_MB_MAX_LENGTH,
                                                                               & sSourceConvertedLen,
                                                                               & sUtf8ConvertedLen,
                                                                               aErrorStack )
                 == STL_SUCCESS );

        STL_DASSERT( sSourceConvertedLen > 0 );

        /*
         * utf8 validate
         */

        STL_TRY( dtsUtf8Validate( (stlChar *)sUtf8,
                                  sUtf8ConvertedLen,
                                  aErrorStack )
                 == STL_SUCCESS );

        if( sDest + sUtf8ConvertedLen < sDestEnd )
        {
            for( i = 0; i < sUtf8ConvertedLen; i++ )
            {
                sDest[i] = sUtf8[i];
            }

            sSourceOffset = sOffset;
            sOffset = sOffset + sSourceConvertedLen;

            sDest = sDest + sUtf8ConvertedLen;
        }

        sSourceString = (stlChar*)sSourceString + sSourceConvertedLen;
        sTotalUtfLen = sTotalUtfLen + sUtf8ConvertedLen;
    }

    STL_TRY_THROW( sSourceStringLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }
    
    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUtfLen;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief unicode -> uhc(aliase ms949) 의 conversion수행 
 * @param[in]  aUniEncoding  source 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aSource       source string(utf16, utf32의 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUnicodeToUhc( dtlUnicodeEncoding   aUniEncoding,
                                     void               * aSource,
                                     stlInt64             aSourceLen,
                                     stlChar            * aDest,
                                     stlInt64             aDestLen,
                                     stlInt64           * aSourceOffset,
                                     stlInt64           * aTotalLength,
                                     stlErrorStack      * aErrorStack )
{
    void       * sSourceString;
    stlUInt8   * sUhcString;
    stlInt64     sSourceStringLen;
    stlUInt32    sUtfCode = 0;
    stlUInt32    sUhcCode = 0;
    stlUInt8   * sUhcEnd;
    stlInt64     sSourceOffset = 0;
    stlInt64     sOffset = 0;
    stlInt64     sTotalUhcLen = 0;

    stlUInt32  * sHash = NULL;

    stlChar      sUtf8Buf[DTS_GET_UTF8_MB_MAX_LENGTH];
    stlUInt8   * sUtf8;
    stlInt64     sSourceConvertedLen = 0;
    stlInt64     sUtf8ConvertedLen = 0;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( (dtsMbConversionHandleList[DTL_UHC] == NULL) ||
        (dtsUTF8MbConversionHashList[DTL_UHC] == NULL) )
    {
        STL_TRY( dtsOpenConversionLibrary( DTL_UHC, aErrorStack ) == STL_SUCCESS );
    }

    sHash = dtsUTF8MbConversionHashList[DTL_UHC];

    sSourceString = aSource;
    sUtf8 = (stlUInt8 *)sUtf8Buf;
    sUhcString = (stlUInt8 *)aDest;
    sUhcEnd = ( sUhcString == NULL ) ? sUhcString : (sUhcString + aDestLen);

    for( sSourceStringLen = aSourceLen;
         sSourceStringLen > 0;
         sSourceStringLen = sSourceStringLen - sSourceConvertedLen )
    {
        /*
         * aUniEncoding -> utf8 로 변경
         */

        STL_TRY( dtsUnicodeConversionFuncList[aUniEncoding][DTL_UNICODE_UTF8]( (void *)sSourceString,
                                                                               sSourceStringLen,
                                                                               (void *)sUtf8,
                                                                               DTS_GET_UTF8_MB_MAX_LENGTH,
                                                                               & sSourceConvertedLen,
                                                                               & sUtf8ConvertedLen,
                                                                               aErrorStack )
                 == STL_SUCCESS );

        STL_DASSERT( sSourceConvertedLen > 0 );
        
        /*
         * utf8 -> uhc 로 변경
         */

        STL_TRY( dtsUtf8Validate( (stlChar *)sUtf8,
                                  sUtf8ConvertedLen,
                                  aErrorStack )
                 == STL_SUCCESS );

        if( sUtf8ConvertedLen == 1 )
        {
            /* ASCII 1Byte Code */

            if( sUhcString < sUhcEnd )
            {
                *sUhcString = *sUtf8;
                sUhcString++;

                sSourceOffset = sOffset;
                sOffset = sOffset + sSourceConvertedLen;
            }

            sTotalUhcLen++;
            sSourceString = (stlChar*)sSourceString + sSourceConvertedLen;

            continue;
        }
        else if( sUtf8ConvertedLen == 2 )
        {
            sUtfCode = sUtf8[0] << 8;
            sUtfCode |= sUtf8[1];
        }
        else if( sUtf8ConvertedLen == 3 )
        {
            sUtfCode = sUtf8[0] << 16;
            sUtfCode |= sUtf8[1] << 8;
            sUtfCode |= sUtf8[2];
        }
        else if( sUtf8ConvertedLen == 4 )
        {
            sUtfCode = sUtf8[0] << 24;
            sUtfCode |= sUtf8[1] << 16;
            sUtfCode |= sUtf8[2] << 8;
            sUtfCode |= sUtf8[3];
        }
        else
        {
            STL_DASSERT( STL_FALSE );
            STL_THROW( ERROR_INTERNAL );
        }

        sUhcCode = sHash[DTS_HASH_BUCKET_IDX(DTS_UTF8_UHC_HASH_BUCKET,
                                             sUtfCode,
                                             DTS_UTF8_UHC_HASH_MODULUS_CONSTANT,
                                             DTS_UTF8_UHC_HASH_MODULUS_SHIFT_BIT,
                                             DTS_UTF8_UHC_HASH_ADJUST_CONSTANT)];

        if( sUhcCode == 0 )
        {
            if( sUhcString < sUhcEnd )
            {
                *sUhcString = '?';
                sUhcString++;

                sSourceOffset = sOffset;
                sOffset = sOffset + sSourceConvertedLen;
            }

            sTotalUhcLen++;
            sSourceString = (stlChar*)sSourceString + sSourceConvertedLen;
        }
        else
        {
            if( sUhcCode & 0x0000ff00 )
            {
                if( sUhcString + 1 < sUhcEnd )
                {
                    *sUhcString = ( sUhcCode & 0x0000ff00 ) >> 8;
                    sUhcString++;
                    *sUhcString = sUhcCode & 0x000000ff;
                    sUhcString++;

                    sSourceOffset = sOffset;
                    sOffset = sOffset + sSourceConvertedLen;
                }

                sTotalUhcLen += 2;
                sSourceString = (stlChar*)sSourceString + sSourceConvertedLen;
            }
            else if( sUhcCode & 0x000000ff )
            {
                if( sUhcString < sUhcEnd )
                {
                    *sUhcString = sUhcCode & 0x000000ff;
                    sUhcString++;

                    sSourceOffset = sOffset;
                    sOffset = sOffset + sSourceConvertedLen;
                }

                sTotalUhcLen++;
                sSourceString = (stlChar*)sSourceString + sSourceConvertedLen;
            }
            else
            {
                STL_DASSERT( STL_FALSE );
                STL_THROW( ERROR_INTERNAL );
            }
        }
    }

    STL_TRY_THROW( sSourceStringLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sUhcString < sUhcEnd )
    {
        sSourceOffset = sOffset;
    }
    
    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUhcLen;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionUnicodeToUhc()" );  
    } 

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief unicode -> gb18030 의 conversion수행 
 * @param[in]  aUniEncoding  source 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aSource       source string(utf16, utf32의 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUnicodeToGb18030( dtlUnicodeEncoding   aUniEncoding,
                                         void               * aSource,
                                         stlInt64             aSourceLen,
                                         stlChar            * aDest,
                                         stlInt64             aDestLen,
                                         stlInt64           * aSourceOffset,
                                         stlInt64           * aTotalLength,
                                         stlErrorStack      * aErrorStack )
{
    void      * sSourceString;
    stlUInt8  * sGb18030String;
    stlInt64    sSourceStringLen;
    stlUInt32   sUtfCode = 0;
    stlUInt32   sGb18030Code = 0;
    stlUInt8  * sGb18030End;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalGb18030Len = 0;

    stlUInt32 * sHash = NULL;

    stlChar      sUtf8Buf[DTS_GET_UTF8_MB_MAX_LENGTH];
    stlUInt8   * sUtf8;
    stlInt64     sSourceConvertedLen = 0;
    stlInt64     sUtf8ConvertedLen = 0;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( (dtsMbConversionHandleList[DTL_GB18030] == NULL) ||
        (dtsUTF8MbConversionHashList[DTL_GB18030] == NULL) )
    {
        STL_TRY( dtsOpenConversionLibrary( DTL_GB18030, aErrorStack ) == STL_SUCCESS );
    }

    sHash = dtsUTF8MbConversionHashList[DTL_GB18030];

    sSourceString = aSource;
    sUtf8 = (stlUInt8 *)sUtf8Buf;
    sGb18030String = (stlUInt8*)aDest;
    sGb18030End = (sGb18030String == NULL ) ? sGb18030String : (sGb18030String + aDestLen);

    for( sSourceStringLen = aSourceLen;
         sSourceStringLen > 0;
         sSourceStringLen = sSourceStringLen - sSourceConvertedLen )
    {
        /*
         * aUniEncoding -> utf8 로 변경
         */

        STL_TRY( dtsUnicodeConversionFuncList[aUniEncoding][DTL_UNICODE_UTF8]( (void *)sSourceString,
                                                                               sSourceStringLen,
                                                                               (void *)sUtf8,
                                                                               DTS_GET_UTF8_MB_MAX_LENGTH,
                                                                               & sSourceConvertedLen,
                                                                               & sUtf8ConvertedLen,
                                                                               aErrorStack )
                 == STL_SUCCESS );

        STL_DASSERT( sSourceConvertedLen > 0 );

        STL_TRY( dtsUtf8Validate( (stlChar *)sUtf8,
                                  sUtf8ConvertedLen,
                                  aErrorStack )
                 == STL_SUCCESS );

        if( sUtf8ConvertedLen == 1 )
        {
            /* ASCII 1Byte Code */

            if( sGb18030String < sGb18030End )
            {
                *sGb18030String = *sUtf8;
                sGb18030String++;

                sSourceOffset = sOffset;
                sOffset = sOffset + sSourceConvertedLen;
            }

            sTotalGb18030Len++;
            sSourceString = (stlChar*)sSourceString + sSourceConvertedLen;

            continue;
        }
        else if( sUtf8ConvertedLen == 2 )
        {
            sUtfCode = sUtf8[0] << 8;
            sUtfCode |= sUtf8[1];
        }
        else if( sUtf8ConvertedLen == 3 )
        {
            sUtfCode = sUtf8[0] << 16;
            sUtfCode |= sUtf8[1] << 8;
            sUtfCode |= sUtf8[2];
        }
        else if( sUtf8ConvertedLen == 4 )
        {
            sUtfCode = sUtf8[0] << 24;
            sUtfCode |= sUtf8[1] << 16;
            sUtfCode |= sUtf8[2] << 8;
            sUtfCode |= sUtf8[3];
        }
        else
        {
            STL_DASSERT( STL_FALSE );
            STL_THROW( ERROR_INTERNAL );
        }

        sGb18030Code = sHash[DTS_HASH_BUCKET_IDX(DTS_UTF8_GB18030_HASH_BUCKET,
                                                 sUtfCode,
                                                 DTS_UTF8_GB18030_HASH_MODULUS_CONSTANT,
                                                 DTS_UTF8_GB18030_HASH_MODULUS_SHIFT_BIT,
                                                 DTS_UTF8_GB18030_HASH_ADJUST_CONSTANT)];

        if( sGb18030Code == 0 )
        {
            if( sGb18030String < sGb18030End )
            {
                *sGb18030String = '?';
                sGb18030String++;

                sSourceOffset = sOffset;
                sOffset = sOffset + sSourceConvertedLen;
            }

            sTotalGb18030Len++;
            sSourceString = (stlChar*)sSourceString + sSourceConvertedLen;
        }
        else
        {
            if( sGb18030Code & 0xff000000 )
            {
                if( sGb18030String + 3 < sGb18030End )
                {
                    *sGb18030String = ( sGb18030Code & 0xff000000 ) >> 24;
                    sGb18030String++;
                    *sGb18030String = ( sGb18030Code & 0x00ff0000 ) >> 16;
                    sGb18030String++;
                    *sGb18030String = ( sGb18030Code & 0x0000ff00 ) >> 8;
                    sGb18030String++;
                    *sGb18030String = sGb18030Code & 0x000000ff;
                    sGb18030String++;

                    sSourceOffset = sOffset;
                    sOffset = sOffset + sSourceConvertedLen;
                }

                sTotalGb18030Len += 4;
                sSourceString = (stlChar*)sSourceString + sSourceConvertedLen;
            }
            else if( sGb18030Code & 0x0000ff00 )
            {
                if( sGb18030String + 1 < sGb18030End )
                {
                    *sGb18030String = ( sGb18030Code & 0x0000ff00 ) >> 8;
                    sGb18030String++;
                    *sGb18030String = sGb18030Code & 0x000000ff;
                    sGb18030String++;

                    sSourceOffset = sOffset;
                    sOffset = sOffset + sSourceConvertedLen;
                }

                sTotalGb18030Len += 2;
                sSourceString = (stlChar*)sSourceString + sSourceConvertedLen;
            }
            else if( sGb18030Code & 0x000000ff )
            {
                if( sGb18030String < sGb18030End )
                {
                    *sGb18030String = sGb18030Code & 0x000000ff;
                    sGb18030String++;

                    sSourceOffset = sOffset;
                    sOffset = sOffset + sSourceConvertedLen;
                }

                sTotalGb18030Len++;
                sSourceString = (stlChar*)sSourceString + sSourceConvertedLen;
            }
            else
            {
                STL_DASSERT( STL_FALSE );
                STL_THROW( ERROR_INTERNAL );
            }
        }
    }

    STL_TRY_THROW( sSourceStringLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sGb18030String < sGb18030End )
    {
        sSourceOffset = sOffset;
    }
    
    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalGb18030Len;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionUtf8ToGb18030()" );  
    } 

    STL_FINISH;

    return STL_FAILURE;
}


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
stlStatus dtsUnicodeToMbConversionNotApplicable( dtlUnicodeEncoding   aUniEncoding,
                                                 void               * aSource,
                                                 stlInt64             aSourceLen,
                                                 stlChar            * aDest,
                                                 stlInt64             aDestLen,
                                                 stlInt64           * aSourceOffset,
                                                 stlInt64           * aTotalLength,
                                                 stlErrorStack      * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtsUnicodeToMbConversionNotApplicable()" );

    return STL_FAILURE;
}


/**
 * @brief unicode encoding 간의 변환 ( utf16 <-> utf8, utf32 <-> utf8 )
 * @param[in]  aSource               source( utf8 ) 문자
 * @param[in]  aSourceLen            source( utf8 ) 문자 길이
 * @param[out] aDest                 dest( utf16 )를 저장할 메모리 주소  
 * @param[in]  aDestLen              dest( utf16 )를 저장할 메모리 크기
 * @param[out] aSourceConvertedLen   변환이 성공한 @a aSource의 byte length
 * @param[out] aDestConvertedLen     변환이 성공한 @a aDest의 byte length
 * @param[in]  aErrorStack           stlErrorStack
 */
stlStatus dtsUnicodeConversionNotApplicable( void           * aSource,
                                             stlInt64         aSourceLen,
                                             void           * aDest,
                                             stlInt64         aDestLen,
                                             stlInt64       * aSourceConvertedLen,
                                             stlInt64       * aDestConvertedLen,
                                             stlErrorStack  * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtsUnicodeConversionNotApplicable()" );

    return STL_FAILURE;
}


/**
 * @brief Mb(muti byte) character set(Ascii) 를  unicode encoding ( utf16, utf32 ) 로 변환
 * @param[in]  aUniEncoding  dest 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aSource       source string ( Ascii )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionAsciiToUnicode( dtlUnicodeEncoding   aUniEncoding,
                                       stlChar            * aSource,
                                       stlInt64             aSourceLen,
                                       void               * aDest,
                                       stlInt64             aDestLen,
                                       stlInt64           * aSourceOffset,
                                       stlInt64           * aTotalLength,
                                       stlErrorStack      * aErrorStack )
{
    stlUInt8  * sSource;
    void      * sDest;
    void      * sDestEnd;
    stlInt8     sChr;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalUnicodeLen = 0;

    stlInt64    sDestConvertedLen = 0;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    sSource = (stlUInt8*)aSource;
    sDest   = (stlUInt8*)aDest;

    sSourceLen = aSourceLen;
    sDestEnd = (sDest == NULL) ? sDest : ((stlChar*)sDest + aDestLen);

    if( aUniEncoding == DTL_UNICODE_UTF16 )
    {
        sDestConvertedLen = STL_UINT16_SIZE;
    }
    else
    {
        sDestConvertedLen = STL_UINT32_SIZE;
    }
    
    while( sSourceLen > 0 )
    {
        sChr = *sSource;

        if( (sChr == 0) || (DTS_IS_HIGH_BIT_SET(sChr) == STL_TRUE) )
        {
            sChr = '?';
        }

        if( sDest < sDestEnd )
        {
            if( aUniEncoding == DTL_UNICODE_UTF16 )
            {
                *(stlUInt16*)sDest = sChr;
            }
            else
            {
                *(stlUInt32*)sDest = sChr;
            }
            
            sDest = (stlChar*)sDest + sDestConvertedLen;

            sSourceOffset = sOffset;
            sOffset++;
        }

        sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            
        sSource++;
        sSourceLen--;
    }

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUnicodeLen;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mb(muti byte) character set(Utf8) 를  unicode encoding ( utf16, utf32 ) 로 변환
 * @param[in]  aUniEncoding  dest 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aSource       source string ( utf8 )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUtf8ToUnicode( dtlUnicodeEncoding   aUniEncoding,
                                      stlChar            * aSource,
                                      stlInt64             aSourceLen,
                                      void               * aDest,
                                      stlInt64             aDestLen,
                                      stlInt64           * aSourceOffset,
                                      stlInt64           * aTotalLength,
                                      stlErrorStack      * aErrorStack )
{
    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( aUniEncoding == DTL_UNICODE_UTF16 )
    {
        STL_TRY( dtsConversionUtf8ToUtf16( aSource,
                                           aSourceLen,
                                           aDest,
                                           aDestLen,
                                           aSourceOffset,
                                           aTotalLength,
                                           aErrorStack )
                 == STL_SUCCESS );
    }
    else if( aUniEncoding == DTL_UNICODE_UTF32 )
    {
        STL_TRY( dtsConversionUtf8ToUtf32( aSource,
                                           aSourceLen,
                                           aDest,
                                           aDestLen,
                                           aSourceOffset,
                                           aTotalLength,
                                           aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( STL_FALSE );
        STL_THROW( ERROR_INTERNAL );
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionUtf8ToUnicode()" );  
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Mb(muti byte) character set(Utf8) 를  unicode encoding ( utf16 ) 로 변환
 * @param[in]  aSource       source string ( utf8 )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUtf8ToUtf16( stlChar            * aSource,
                                    stlInt64             aSourceLen,
                                    void               * aDest,
                                    stlInt64             aDestLen,
                                    stlInt64           * aSourceOffset,
                                    stlInt64           * aTotalLength,
                                    stlErrorStack      * aErrorStack )
{
    stlUInt8  * sSource;
    void      * sDest;
    void      * sDestEnd;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalUnicodeLen = 0;

    stlInt8     sMbLen;
    stlInt64    sSourceConvertedLen = 0;
    stlInt64    sDestConvertedLen = 0;
    stlUInt16   sUtf16[2] = { 0, 0 };
    stlInt64    sUtf16Len = STL_UINT16_SIZE * 2;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    sSource = (stlUInt8*)aSource;
    sDest   = aDest;

    sSourceLen = aSourceLen;
    sDestEnd = (sDest == NULL) ? sDest : ((stlChar*)sDest + aDestLen);

    while( sSourceLen > 0 )
    {
        STL_TRY( dtsGetUtf8MbLength( (stlChar *)sSource,
                                     & sMbLen,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( dtsUtf8Validate( (stlChar *)sSource,
                                  sMbLen,
                                  aErrorStack )
                 == STL_SUCCESS );

        /*
         * utf8 -> unicode(utf16) 로 변경
         */
        STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF16]( (void *)sSource,
                                                                                    sMbLen,
                                                                                    (void *)sUtf16,
                                                                                    sUtf16Len,
                                                                                    & sSourceConvertedLen,
                                                                                    & sDestConvertedLen,
                                                                                    aErrorStack )
                     == STL_SUCCESS );

        STL_DASSERT( sSourceConvertedLen > 0 );

        if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
        {
            if( sDestConvertedLen == 2 )
            {
                ((stlUInt16 *)(sDest))[0] = sUtf16[0];
            }
            else
            {
                STL_DASSERT( sDestConvertedLen == 4 );

                ((stlUInt16 *)(sDest))[0] = sUtf16[0];
                ((stlUInt16 *)(sDest))[1] = sUtf16[1];
            }

            sDest = (stlChar*)sDest + sDestConvertedLen;
            
            sSourceOffset = sOffset;
            sOffset = sOffset + sMbLen;
        }

        sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            
        sSource = sSource + sMbLen;
        sSourceLen = sSourceLen - sMbLen;
    }

    STL_TRY_THROW( sSourceLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUnicodeLen;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Mb(muti byte) character set(Utf8) 를  unicode encoding ( utf32 ) 로 변환
 * @param[in]  aSource       source string ( utf32 )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUtf8ToUtf32( stlChar            * aSource,
                                    stlInt64             aSourceLen,
                                    void               * aDest,
                                    stlInt64             aDestLen,
                                    stlInt64           * aSourceOffset,
                                    stlInt64           * aTotalLength,
                                    stlErrorStack      * aErrorStack )
{
    stlUInt8  * sSource;
    void      * sDest;
    void      * sDestEnd;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalUnicodeLen = 0;

    stlInt8     sMbLen;
    stlInt64    sSourceConvertedLen = 0;
    stlInt64    sDestConvertedLen = 0;
    stlUInt32   sUtf32;
    stlInt64    sUtf32Len = STL_UINT32_SIZE;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    sSource = (stlUInt8*)aSource;
    sDest   = aDest;

    sSourceLen = aSourceLen;
    sDestEnd = (sDest == NULL) ? sDest : ((stlChar*)sDest + aDestLen);

    while( sSourceLen > 0 )
    {
        STL_TRY( dtsGetUtf8MbLength( (stlChar *)sSource,
                                     & sMbLen,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( dtsUtf8Validate( (stlChar *)sSource,
                                  sMbLen,
                                  aErrorStack )
                 == STL_SUCCESS );

        /*
         * utf8 -> unicode(utf32) 로 변경
         */
        STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF32]( (void *)sSource,
                                                                                    sMbLen,
                                                                                    (void *)&sUtf32,
                                                                                    sUtf32Len,
                                                                                    & sSourceConvertedLen,
                                                                                    & sDestConvertedLen,
                                                                                    aErrorStack )
                     == STL_SUCCESS );

        STL_DASSERT( sSourceConvertedLen > 0 );

        if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
        {
            *((stlUInt32 *)(sDest)) = sUtf32;

            sDest = (stlChar*)sDest + sDestConvertedLen;
            
            sSourceOffset = sOffset;
            sOffset = sOffset + sMbLen;
        }

        sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            
        sSource = sSource + sMbLen;
        sSourceLen = sSourceLen - sMbLen;
    }

    STL_TRY_THROW( sSourceLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUnicodeLen;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Mb(muti byte) character set(uhc) 를  unicode encoding ( utf16, utf32 ) 로 변환
 * @param[in]  aUniEncoding  dest 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aSource       source string ( uhc )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUhcToUnicode( dtlUnicodeEncoding   aUniEncoding,
                                     stlChar            * aSource,
                                     stlInt64             aSourceLen,
                                     void               * aDest,
                                     stlInt64             aDestLen,
                                     stlInt64           * aSourceOffset,
                                     stlInt64           * aTotalLength,
                                     stlErrorStack      * aErrorStack )
{
    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( aUniEncoding == DTL_UNICODE_UTF16 )
    {
        STL_TRY( dtsConversionUhcToUtf16( aSource,
                                          aSourceLen,
                                          aDest,
                                          aDestLen,
                                          aSourceOffset,
                                          aTotalLength,
                                          aErrorStack )
                 == STL_SUCCESS );
    }
    else if( aUniEncoding == DTL_UNICODE_UTF32 )
    {
        STL_TRY( dtsConversionUhcToUtf32( aSource,
                                          aSourceLen,
                                          aDest,
                                          aDestLen,
                                          aSourceOffset,
                                          aTotalLength,
                                          aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( STL_FALSE );
        STL_THROW( ERROR_INTERNAL );
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionUhcToUnicode()" );  
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Mb(muti byte) character set(uhc) 를  unicode encoding ( utf16 ) 로 변환
 * @param[in]  aSource       source string ( uhc )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUhcToUtf16( stlChar            * aSource,
                                   stlInt64             aSourceLen,
                                   void               * aDest,
                                   stlInt64             aDestLen,
                                   stlInt64           * aSourceOffset,
                                   stlInt64           * aTotalLength,
                                   stlErrorStack      * aErrorStack )
{
    stlUInt8  * sSource;
    void      * sDest;
    void      * sDestEnd;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalUnicodeLen = 0;

    stlInt8     sMbLen;
    stlUInt32   sUhcCode = 0;
    stlUInt32   sUtfCode = 0;
    stlChar     sUtf8Buf[DTS_GET_UTF8_MB_MAX_LENGTH];
    stlUInt8  * sUtf8;
    stlInt64    sUtf8Len;
    stlInt64    sSourceConvertedLen = 0;
    stlInt64    sDestConvertedLen = 0;
    stlUInt16   sUtf16[2] = { 0, 0 };
    stlInt64    sUtf16Len = STL_UINT16_SIZE * 2;

    stlUInt32 * sHash = NULL;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( (dtsMbConversionHandleList[DTL_UHC] == NULL) ||
        (dtsMbUTF8ConversionHashList[DTL_UHC] == NULL) )
    {
        STL_TRY( dtsOpenConversionLibrary( DTL_UHC, aErrorStack ) == STL_SUCCESS );
    }

    sHash = dtsMbUTF8ConversionHashList[DTL_UHC];

    sSource = (stlUInt8*)aSource;
    sUtf8 = (stlUInt8 *)sUtf8Buf;
    sDest = (stlUInt8*)aDest;
    sDestEnd = (sDest == NULL) ? sDest : ((stlChar*)sDest + aDestLen);

    for( sSourceLen = aSourceLen;
         sSourceLen > 0;
         sSourceLen = sSourceLen - sMbLen )
    {
        if( DTS_IS_HIGH_BIT_SET(*sSource) == STL_FALSE )
        {
            /* ASCII 1Byte Code */

            sMbLen = 1;

            *sUtf8 = *sSource;

            /*
             * utf8 -> unicode(utf16) 로 변경
             */
            STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF16]( (void *)sUtf8,
                                                                                        sMbLen,
                                                                                        (void *)sUtf16,
                                                                                        sUtf16Len,
                                                                                        & sSourceConvertedLen,
                                                                                        & sDestConvertedLen,
                                                                                        aErrorStack )
                     == STL_SUCCESS );

            STL_DASSERT( sSourceConvertedLen > 0 );
            STL_DASSERT( sDestConvertedLen == 2 );

            if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
            {
                ((stlUInt16 *)(sDest))[0] = sUtf16[0];

                sDest = (stlChar*)sDest + sDestConvertedLen;
            
                sSourceOffset = sOffset;
                sOffset = sOffset + sMbLen;
            }

            sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            sSource = sSource + sMbLen;

            continue;
        }
        else
        {
            /* 2Byte ~ */

            if( dtlMbStrVerify( DTL_UHC,
                                (stlChar *)sSource,
                                sSourceLen,
                                &sMbLen,
                                aErrorStack ) != STL_SUCCESS )
            {
                sMbLen = 1;

                *sUtf8 = '?';

                /*
                 * utf8 -> unicode(utf16) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF16]( (void *)sUtf8,
                                                                                            sMbLen,
                                                                                            (void *)sUtf16,
                                                                                            sUtf16Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );
                
                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( sDestConvertedLen == 2 );
                
                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    ((stlUInt16 *)(sDest))[0] = sUtf16[0];
                    
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                    
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
                
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
                sSource = sSource + sMbLen;

                continue;
            }
            else
            {
                STL_DASSERT( sMbLen == 2 );
  
                sUhcCode = *sSource << 8;
                sSource++;
                
                sUhcCode |= *sSource;
                sSource++;
            }

            sUtfCode = sHash[DTS_HASH_BUCKET_IDX(DTS_UHC_UTF8_HASH_BUCKET,
                                                 sUhcCode,
                                                 DTS_UHC_UTF8_HASH_MODULUS_CONSTANT,
                                                 DTS_UHC_UTF8_HASH_MODULUS_SHIFT_BIT,
                                                 DTS_UHC_UTF8_HASH_ADJUST_CONSTANT) ];

            if( sUtfCode == 0 )
            {
                *sUtf8 = '?';

                /*
                 * utf8 -> unicode(utf16) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF16]( (void *)sUtf8,
                                                                                            1,
                                                                                            (void *)sUtf16,
                                                                                            sUtf16Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );
            
                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( sDestConvertedLen == 2 );
            
                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    ((stlUInt16 *)(sDest))[0] = sUtf16[0];
                
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
            
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            }
            else
            {
                if( sUtfCode & 0xff000000 )
                {
                    sUtf8[0] = sUtfCode >> 24;
                    sUtf8[1] = ( sUtfCode & 0x00ff0000 ) >> 16;
                    sUtf8[2] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[3] = sUtfCode & 0x000000ff;

                    sUtf8Len = 4;
                }
                else if( sUtfCode & 0x00ff0000 )
                {
                    sUtf8[0] = ( sUtfCode & 0x00ff0000 ) >> 16;
                    sUtf8[1] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[2] = sUtfCode & 0x000000ff;

                    sUtf8Len = 3;
                }
                else if( sUtfCode & 0x0000ff00 )
                {
                    sUtf8[0] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[1] = sUtfCode & 0x000000ff;

                    sUtf8Len = 2;
                }
                else if( sUtfCode & 0x000000ff )
                {
                    sUtf8[0] = sUtfCode & 0x000000ff;

                    sUtf8Len = 1;
                }
                else
                {
                    STL_DASSERT( STL_FALSE );
                    STL_THROW( ERROR_INTERNAL );
                }  
            
                /*
                 * utf8 -> unicode(utf16) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF16]( (void *)sUtf8,
                                                                                            sUtf8Len,
                                                                                            (void *)sUtf16,
                                                                                            sUtf16Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );
                
                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( ( sDestConvertedLen == 2 ) || ( sDestConvertedLen == 4 ) );
            
                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    if( sDestConvertedLen == 2 )
                    {
                        ((stlUInt16 *)(sDest))[0] = sUtf16[0];
                    }
                    else
                    {
                        STL_DASSERT( sDestConvertedLen == 4 );
                    
                        ((stlUInt16 *)(sDest))[0] = sUtf16[0];
                        ((stlUInt16 *)(sDest))[1] = sUtf16[1];
                    }
                
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                    
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
            
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            }
        }
    }

    STL_TRY_THROW( sSourceLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUnicodeLen;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionUhcToUtf16()" );  
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Mb(muti byte) character set(uhc) 를  unicode encoding ( utf32 ) 로 변환
 * @param[in]  aSource       source string ( uhc )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionUhcToUtf32( stlChar            * aSource,
                                   stlInt64             aSourceLen,
                                   void               * aDest,
                                   stlInt64             aDestLen,
                                   stlInt64           * aSourceOffset,
                                   stlInt64           * aTotalLength,
                                   stlErrorStack      * aErrorStack )
{
    stlUInt8  * sSource;
    void      * sDest;
    void      * sDestEnd;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalUnicodeLen = 0;

    stlInt8     sMbLen;
    stlUInt32   sUhcCode = 0;
    stlUInt32   sUtfCode = 0;
    stlChar     sUtf8Buf[DTS_GET_UTF8_MB_MAX_LENGTH];
    stlUInt8  * sUtf8;
    stlInt64    sUtf8Len;
    stlInt64    sSourceConvertedLen = 0;
    stlInt64    sDestConvertedLen = 0;
    stlUInt32   sUtf32;
    stlInt64    sUtf32Len = STL_UINT32_SIZE;

    stlUInt32 * sHash = NULL;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( (dtsMbConversionHandleList[DTL_UHC] == NULL) ||
        (dtsMbUTF8ConversionHashList[DTL_UHC] == NULL) )
    {
        STL_TRY( dtsOpenConversionLibrary( DTL_UHC, aErrorStack ) == STL_SUCCESS );
    }

    sHash = dtsMbUTF8ConversionHashList[DTL_UHC];

    sSource = (stlUInt8*)aSource;
    sUtf8 = (stlUInt8 *)sUtf8Buf;
    sDest = (stlUInt8*)aDest;
    sDestEnd = (sDest == NULL) ? sDest : ((stlChar*)sDest + aDestLen);

    for( sSourceLen = aSourceLen;
         sSourceLen > 0;
         sSourceLen = sSourceLen - sMbLen )
    {
        if( DTS_IS_HIGH_BIT_SET(*sSource) == STL_FALSE )
        {
            /* ASCII 1Byte Code */

            sMbLen = 1;

            *sUtf8 = *sSource;

            /*
             * utf8 -> unicode(utf32) 로 변경
             */
            STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF32]( (void *)sUtf8,
                                                                                        sMbLen,
                                                                                        (void *)&sUtf32,
                                                                                        sUtf32Len,
                                                                                        & sSourceConvertedLen,
                                                                                        & sDestConvertedLen,
                                                                                        aErrorStack )
                     == STL_SUCCESS );

            STL_DASSERT( sSourceConvertedLen > 0 );
            STL_DASSERT( sDestConvertedLen == 4 );

            if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
            {
                *((stlUInt32 *)(sDest)) = sUtf32;

                sDest = (stlChar*)sDest + sDestConvertedLen;
            
                sSourceOffset = sOffset;
                sOffset = sOffset + sMbLen;
            }

            sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            sSource = sSource + sMbLen;

            continue;
        }
        else
        {
            /* 2Byte ~ */

            if( dtlMbStrVerify( DTL_UHC,
                                (stlChar *)sSource,
                                sSourceLen,
                                &sMbLen,
                                aErrorStack ) != STL_SUCCESS )
            {
                sMbLen = 1;

                *sUtf8 = '?';

                /*
                 * utf8 -> unicode(utf32) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF32]( (void *)sUtf8,
                                                                                            sMbLen,
                                                                                            (void *)&sUtf32,
                                                                                            sUtf32Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );
                
                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( sDestConvertedLen == 4 );

                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    *((stlUInt32 *)(sDest)) = sUtf32;
                    
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                    
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
                
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
                sSource = sSource + sMbLen;

                continue;
            }
            else
            {
                STL_DASSERT( sMbLen == 2 );
  
                sUhcCode = *sSource << 8;
                sSource++;
                
                sUhcCode |= *sSource;
                sSource++;
            }

            sUtfCode = sHash[DTS_HASH_BUCKET_IDX(DTS_UHC_UTF8_HASH_BUCKET,
                                                 sUhcCode,
                                                 DTS_UHC_UTF8_HASH_MODULUS_CONSTANT,
                                                 DTS_UHC_UTF8_HASH_MODULUS_SHIFT_BIT,
                                                 DTS_UHC_UTF8_HASH_ADJUST_CONSTANT) ];

            if( sUtfCode == 0 )
            {
                *sUtf8 = '?';

                /*
                 * utf8 -> unicode(utf32) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF32]( (void *)sUtf8,
                                                                                            1,
                                                                                            (void *)&sUtf32,
                                                                                            sUtf32Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );
                
                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( sDestConvertedLen == 4 );

                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    *((stlUInt32 *)(sDest)) = sUtf32;
                
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                    
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
            
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            }
            else
            {
                if( sUtfCode & 0xff000000 )
                {
                    sUtf8[0] = sUtfCode >> 24;
                    sUtf8[1] = ( sUtfCode & 0x00ff0000 ) >> 16;
                    sUtf8[2] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[3] = sUtfCode & 0x000000ff;

                    sUtf8Len = 4;
                }
                else if( sUtfCode & 0x00ff0000 )
                {
                    sUtf8[0] = ( sUtfCode & 0x00ff0000 ) >> 16;
                    sUtf8[1] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[2] = sUtfCode & 0x000000ff;

                    sUtf8Len = 3;
                }
                else if( sUtfCode & 0x0000ff00 )
                {
                    sUtf8[0] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[1] = sUtfCode & 0x000000ff;

                    sUtf8Len = 2;
                }
                else if( sUtfCode & 0x000000ff )
                {
                    sUtf8[0] = sUtfCode & 0x000000ff;

                    sUtf8Len = 1;
                }
                else
                {
                    STL_DASSERT( STL_FALSE );
                    STL_THROW( ERROR_INTERNAL );
                }  

                /*
                 * utf8 -> unicode(utf32) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF32]( (void *)sUtf8,
                                                                                            sUtf8Len,
                                                                                            (void *)&sUtf32,
                                                                                            sUtf32Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );
            
                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( sDestConvertedLen == 4 );
            
                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    *((stlUInt32 *)(sDest)) = sUtf32;
                
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                    
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
            
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            }
        }
    }

    STL_TRY_THROW( sSourceLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUnicodeLen;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }
    
    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionUhcToUtf32()" );  
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Mb(muti byte) character set(Gb18030) 를  unicode encoding ( utf16, utf32 ) 로 변환
 * @param[in]  aUniEncoding  dest 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aSource       source string ( Gb18030 )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionGb18030ToUnicode( dtlUnicodeEncoding   aUniEncoding,
                                         stlChar            * aSource,
                                         stlInt64             aSourceLen,
                                         void               * aDest,
                                         stlInt64             aDestLen,
                                         stlInt64           * aSourceOffset,
                                         stlInt64           * aTotalLength,
                                         stlErrorStack      * aErrorStack )
{
    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( aUniEncoding == DTL_UNICODE_UTF16 )
    {
        STL_TRY( dtsConversionGb18030ToUtf16( aSource,
                                              aSourceLen,
                                              aDest,
                                              aDestLen,
                                              aSourceOffset,
                                              aTotalLength,
                                              aErrorStack )
                 == STL_SUCCESS );
    }
    else if( aUniEncoding == DTL_UNICODE_UTF32 )
    {
        STL_TRY( dtsConversionGb18030ToUtf32( aSource,
                                              aSourceLen,
                                              aDest,
                                              aDestLen,
                                              aSourceOffset,
                                              aTotalLength,
                                              aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( STL_FALSE );
        STL_THROW( ERROR_INTERNAL );
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionGb18030ToUnicode()" );  
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Mb(muti byte) character set(uhc) 를  unicode encoding ( utf16 ) 로 변환
 * @param[in]  aSource       source string ( uhc )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionGb18030ToUtf16( stlChar            * aSource,
                                       stlInt64             aSourceLen,
                                       void               * aDest,
                                       stlInt64             aDestLen,
                                       stlInt64           * aSourceOffset,
                                       stlInt64           * aTotalLength,
                                       stlErrorStack      * aErrorStack )
{
    stlUInt8  * sSource;
    void      * sDest;
    void      * sDestEnd;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalUnicodeLen = 0;

    stlInt8     sMbLen;
    stlUInt32   sGb18030Code = 0;
    stlUInt32   sUtfCode = 0;
    stlChar     sUtf8Buf[DTS_GET_UTF8_MB_MAX_LENGTH];
    stlUInt8  * sUtf8;
    stlInt64    sUtf8Len;
    stlInt64    sSourceConvertedLen = 0;
    stlInt64    sDestConvertedLen = 0;
    stlUInt16   sUtf16[2] = { 0, 0 };
    stlInt64    sUtf16Len = STL_UINT16_SIZE * 2;

    stlUInt32 * sHash = NULL;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( (dtsMbConversionHandleList[DTL_GB18030] == NULL) ||
        (dtsMbUTF8ConversionHashList[DTL_GB18030] == NULL) )
    {
        STL_TRY( dtsOpenConversionLibrary( DTL_GB18030, aErrorStack ) == STL_SUCCESS );
    }

    sHash = dtsMbUTF8ConversionHashList[DTL_GB18030];

    sSource = (stlUInt8*)aSource;
    sUtf8 = (stlUInt8 *)sUtf8Buf;
    sDest = (stlUInt8*)aDest;
    sDestEnd = (sDest == NULL) ? sDest : ((stlChar*)sDest + aDestLen);

    for( sSourceLen = aSourceLen;
         sSourceLen > 0;
         sSourceLen = sSourceLen - sMbLen )
    {
        if( DTS_IS_HIGH_BIT_SET(*sSource) == STL_FALSE )
        {
            /* ASCII 1Byte Code */

            sMbLen = 1;

            *sUtf8 = *sSource;

            /*
             * utf8 -> unicode(utf16) 로 변경
             */
            STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF16]( (void *)sUtf8,
                                                                                        sMbLen,
                                                                                        (void *)sUtf16,
                                                                                        sUtf16Len,
                                                                                        & sSourceConvertedLen,
                                                                                        & sDestConvertedLen,
                                                                                        aErrorStack )
                     == STL_SUCCESS );

            STL_DASSERT( sSourceConvertedLen > 0 );
            STL_DASSERT( sDestConvertedLen == 2 );

            if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
            {
                ((stlUInt16 *)(sDest))[0] = sUtf16[0];

                sDest = (stlChar*)sDest + sDestConvertedLen;
            
                sSourceOffset = sOffset;
                sOffset = sOffset + sMbLen;
            }

            sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            sSource = sSource + sMbLen;

            continue;
        }
        else
        {
            /* 2Byte ~ */

            if( dtlMbStrVerify( DTL_GB18030,
                                (stlChar *)sSource,
                                sSourceLen,
                                &sMbLen,
                                aErrorStack ) != STL_SUCCESS )
            {
                sMbLen = 1;

                *sUtf8 = '?';

                /*
                 * utf8 -> unicode(utf16) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF16]( (void *)sUtf8,
                                                                                            sMbLen,
                                                                                            (void *)sUtf16,
                                                                                            sUtf16Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );
                
                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( sDestConvertedLen == 2 );
                
                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    ((stlUInt16 *)(sDest))[0] = sUtf16[0];
                    
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                    
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
                
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
                sSource = sSource + sMbLen;

                continue;
            }

            if( sMbLen == 1 )
            {
                STL_DASSERT( STL_FALSE );
                STL_THROW( ERROR_INTERNAL );
            }
            else if( sMbLen == 2 )
            {
                sGb18030Code = *sSource << 8;
                sSource++;

                sGb18030Code |= *sSource;
                sSource++;
            }
            else if( sMbLen == 4 )
            {
                sGb18030Code = *sSource << 24;
                sSource++;

                sGb18030Code |= *sSource << 16;
                sSource++;

                sGb18030Code |= *sSource << 8;
                sSource++;

                sGb18030Code |= *sSource;
                sSource++;
            }
            else
            {
                STL_DASSERT( STL_FALSE );
                STL_THROW( ERROR_INTERNAL );
            }

            sUtfCode = sHash[DTS_HASH_BUCKET_IDX(DTS_GB18030_UTF8_HASH_BUCKET,
                                                 sGb18030Code,
                                                 DTS_GB18030_UTF8_HASH_MODULUS_CONSTANT,
                                                 DTS_GB18030_UTF8_HASH_MODULUS_SHIFT_BIT,
                                                 DTS_GB18030_UTF8_HASH_ADJUST_CONSTANT) ];
      

            if( sUtfCode == 0 )
            {
                *sUtf8 = '?';

                /*
                 * utf8 -> unicode(utf16) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF16]( (void *)sUtf8,
                                                                                            1,
                                                                                            (void *)sUtf16,
                                                                                            sUtf16Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );
            
                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( sDestConvertedLen == 2 );
                
                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    ((stlUInt16 *)(sDest))[0] = sUtf16[0];
                
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
            
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            }
            else
            {
                if( sUtfCode & 0xff000000 )
                {
                    sUtf8[0] = sUtfCode >> 24;
                    sUtf8[1] = ( sUtfCode & 0x00ff0000 ) >> 16;
                    sUtf8[2] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[3] = sUtfCode & 0x000000ff;

                    sUtf8Len = 4;
                }
                else if( sUtfCode & 0x00ff0000 )
                {
                    sUtf8[0] = ( sUtfCode & 0x00ff0000 ) >> 16;
                    sUtf8[1] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[2] = sUtfCode & 0x000000ff;

                    sUtf8Len = 3;
                }
                else if( sUtfCode & 0x0000ff00 )
                {
                    sUtf8[0] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[1] = sUtfCode & 0x000000ff;

                    sUtf8Len = 2;
                }
                else if( sUtfCode & 0x000000ff )
                {
                    sUtf8[0] = sUtfCode & 0x000000ff;

                    sUtf8Len = 1;
                }
                else
                {
                    STL_DASSERT( STL_FALSE );
                    STL_THROW( ERROR_INTERNAL );
                }  

                /*
                 * utf8 -> unicode(utf16) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF16]( (void *)sUtf8,
                                                                                            sUtf8Len,
                                                                                            (void *)sUtf16,
                                                                                            sUtf16Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );
                
                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( ( sDestConvertedLen == 2 ) || ( sDestConvertedLen == 4 ) );
                
                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    if( sDestConvertedLen == 2 )
                    {
                        ((stlUInt16 *)(sDest))[0] = sUtf16[0];
                    }
                    else
                    {
                        STL_DASSERT( sDestConvertedLen == 4 );
                    
                        ((stlUInt16 *)(sDest))[0] = sUtf16[0];
                        ((stlUInt16 *)(sDest))[1] = sUtf16[1];
                    }
                
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
            
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            }
        }
    }

    STL_TRY_THROW( sSourceLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUnicodeLen;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionGb18030ToUtf16()" );  
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Mb(muti byte) character set(uhc) 를  unicode encoding ( utf32 ) 로 변환
 * @param[in]  aSource       source string ( uhc )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtsConversionGb18030ToUtf32( stlChar            * aSource,
                                       stlInt64             aSourceLen,
                                       void               * aDest,
                                       stlInt64             aDestLen,
                                       stlInt64           * aSourceOffset,
                                       stlInt64           * aTotalLength,
                                       stlErrorStack      * aErrorStack )
{
    stlUInt8  * sSource;
    void      * sDest;
    void      * sDestEnd;
    stlInt64    sSourceLen;
    stlInt64    sSourceOffset = 0;
    stlInt64    sOffset = 0;
    stlInt64    sTotalUnicodeLen = 0;

    stlInt8     sMbLen;
    stlUInt32   sGb18030Code = 0;
    stlUInt32   sUtfCode = 0;
    stlChar     sUtf8Buf[DTS_GET_UTF8_MB_MAX_LENGTH];
    stlUInt8  * sUtf8;
    stlInt64    sUtf8Len;
    stlInt64    sSourceConvertedLen = 0;
    stlInt64    sDestConvertedLen = 0;
    stlUInt32   sUtf32;
    stlInt64    sUtf32Len = STL_UINT32_SIZE;

    stlUInt32 * sHash = NULL;

    STL_PARAM_VALIDATE( aSource != NULL, aErrorStack );

    if( (dtsMbConversionHandleList[DTL_GB18030] == NULL) ||
        (dtsMbUTF8ConversionHashList[DTL_GB18030] == NULL) )
    {
        STL_TRY( dtsOpenConversionLibrary( DTL_GB18030, aErrorStack ) == STL_SUCCESS );
    }

    sHash = dtsMbUTF8ConversionHashList[DTL_GB18030];

    sSource = (stlUInt8*)aSource;
    sUtf8 = (stlUInt8 *)sUtf8Buf;
    sDest = (stlUInt8*)aDest;
    sDestEnd = (sDest == NULL) ? sDest : ((stlChar*)sDest + aDestLen);

    for( sSourceLen = aSourceLen;
         sSourceLen > 0;
         sSourceLen = sSourceLen - sMbLen )
    {
        if( DTS_IS_HIGH_BIT_SET(*sSource) == STL_FALSE )
        {
            /* ASCII 1Byte Code */

            sMbLen = 1;

            *sUtf8 = *sSource;

            /*
             * utf8 -> unicode(utf32) 로 변경
             */
            STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF32]( (void *)sUtf8,
                                                                                        sMbLen,
                                                                                        (void *)&sUtf32,
                                                                                        sUtf32Len,
                                                                                        & sSourceConvertedLen,
                                                                                        & sDestConvertedLen,
                                                                                        aErrorStack )
                     == STL_SUCCESS );

            STL_DASSERT( sSourceConvertedLen > 0 );
            STL_DASSERT( sDestConvertedLen == 4 );

            if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
            {
                *((stlUInt32 *)(sDest)) = sUtf32;

                sDest = (stlChar*)sDest + sDestConvertedLen;
            
                sSourceOffset = sOffset;
                sOffset = sOffset + sMbLen;
            }

            sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            sSource = sSource + sMbLen;

            continue;
        }
        else
        {
            /* 2Byte ~ */

            if( dtlMbStrVerify( DTL_GB18030,
                                (stlChar *)sSource,
                                sSourceLen,
                                &sMbLen,
                                aErrorStack ) != STL_SUCCESS )
            {
                sMbLen = 1;

                *sUtf8 = '?';

                /*
                 * utf8 -> unicode(utf32) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF32]( (void *)sUtf8,
                                                                                            sMbLen,
                                                                                            (void *)&sUtf32,
                                                                                            sUtf32Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );

                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( sDestConvertedLen == 4 );
                
                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    *((stlUInt32 *)(sDest)) = sUtf32;
                    
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                    
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
                
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
                sSource = sSource + sMbLen;

                continue;
            }

            if( sMbLen == 1 )
            {
                STL_DASSERT( STL_FALSE );
                STL_THROW( ERROR_INTERNAL );
            }
            else if( sMbLen == 2 )
            {
                sGb18030Code = *sSource << 8;
                sSource++;

                sGb18030Code |= *sSource;
                sSource++;
            }
            else if( sMbLen == 4 )
            {
                sGb18030Code = *sSource << 24;
                sSource++;

                sGb18030Code |= *sSource << 16;
                sSource++;

                sGb18030Code |= *sSource << 8;
                sSource++;

                sGb18030Code |= *sSource;
                sSource++;
            }
            else
            {
                STL_DASSERT( STL_FALSE );
                STL_THROW( ERROR_INTERNAL );
            }

            sUtfCode = sHash[DTS_HASH_BUCKET_IDX(DTS_GB18030_UTF8_HASH_BUCKET,
                                                 sGb18030Code,
                                                 DTS_GB18030_UTF8_HASH_MODULUS_CONSTANT,
                                                 DTS_GB18030_UTF8_HASH_MODULUS_SHIFT_BIT,
                                                 DTS_GB18030_UTF8_HASH_ADJUST_CONSTANT) ];
      

            if( sUtfCode == 0 )
            {
                *sUtf8 = '?';

                /*
                 * utf8 -> unicode(utf32) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF32]( (void *)sUtf8,
                                                                                            1,
                                                                                            (void *)&sUtf32,
                                                                                            sUtf32Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );

                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( sDestConvertedLen == 4 );

                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    *((stlUInt32 *)(sDest)) = sUtf32;
                
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                    
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
            
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            }
            else
            {
                if( sUtfCode & 0xff000000 )
                {
                    sUtf8[0] = sUtfCode >> 24;
                    sUtf8[1] = ( sUtfCode & 0x00ff0000 ) >> 16;
                    sUtf8[2] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[3] = sUtfCode & 0x000000ff;

                    sUtf8Len = 4;
                }
                else if( sUtfCode & 0x00ff0000 )
                {
                    sUtf8[0] = ( sUtfCode & 0x00ff0000 ) >> 16;
                    sUtf8[1] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[2] = sUtfCode & 0x000000ff;

                    sUtf8Len = 3;
                }
                else if( sUtfCode & 0x0000ff00 )
                {
                    sUtf8[0] = ( sUtfCode & 0x0000ff00 ) >> 8;
                    sUtf8[1] = sUtfCode & 0x000000ff;

                    sUtf8Len = 2;
                }
                else if( sUtfCode & 0x000000ff )
                {
                    sUtf8[0] = sUtfCode & 0x000000ff;

                    sUtf8Len = 1;
                }
                else
                {
                    STL_DASSERT( STL_FALSE );
                    STL_THROW( ERROR_INTERNAL );
                }  

                /*
                 * utf8 -> unicode(utf32) 로 변경
                 */
                STL_TRY( dtsUnicodeConversionFuncList[DTL_UNICODE_UTF8][DTL_UNICODE_UTF32]( (void *)sUtf8,
                                                                                            sUtf8Len,
                                                                                            (void *)&sUtf32,
                                                                                            sUtf32Len,
                                                                                            & sSourceConvertedLen,
                                                                                            & sDestConvertedLen,
                                                                                            aErrorStack )
                         == STL_SUCCESS );

                STL_DASSERT( sSourceConvertedLen > 0 );
                STL_DASSERT( sDestConvertedLen == 4 );
            
                if( (stlChar*)sDest + sDestConvertedLen <= (stlChar*)sDestEnd )
                {
                    *((stlUInt32 *)(sDest)) = sUtf32;
                
                    sDest = (stlChar*)sDest + sDestConvertedLen;
                    
                    sSourceOffset = sOffset;
                    sOffset = sOffset + sMbLen;
                }
            
                sTotalUnicodeLen = sTotalUnicodeLen + sDestConvertedLen;
            }
        }
    }

    STL_TRY_THROW( sSourceLen >= 0, ERROR_CHARACTER_NOT_IN_REPERTOIRE );

    if( sDest < sDestEnd )
    {
        sSourceOffset = sOffset;
    }

    if( aSourceOffset != NULL )
    {
        *aSourceOffset = sSourceOffset;
    }

    if( aTotalLength != NULL )
    {
        *aTotalLength = sTotalUnicodeLen;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }

    STL_CATCH( ERROR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtsConversionGb18030ToUtf32()" );  
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
