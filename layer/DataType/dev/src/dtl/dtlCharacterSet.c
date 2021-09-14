/*******************************************************************************
 * dtlCharacterSet.c
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
 * @file dtlCharacterSet.c
 * @brief DataType Layer CharacterSet 관련 redirect 함수 정의
 */

#include <dtl.h>
#include <dtsCharacterSet.h>

/**
 * @addtogroup dtlCharSet
 * @{
 */

/**
 * @brief CHARACTER SET 의 String 상수
 */
stlChar * gDtlCharacterSetString[DTL_CHARACTERSET_MAX] =
{
    "SQL_ASCII",    /**< DTL_SQL_ASCII */
    "UTF8",         /**< DTL_UTF8 */
    "UHC",          /**< DTL_UHC */
    "GB18030"       /**< DTL_GB18030 */
};

/**
 * @brief CHARACTER SET 이름과 ID MAP
 */
const dtlCharsetMap gDtlCharacterSet[] =
{
    { "UTF-8",          DTL_UTF8             },
    { "UTF8",           DTL_UTF8             },
    { "CP65001",        DTL_UTF8             },
    
    { "UHC",            DTL_UHC              },
    { "EUCKR",          DTL_UHC              },
    { "EUC-KR",         DTL_UHC              },
    { "IBM-EUCKR",      DTL_UHC              },
    { "DECKOREAN",      DTL_UHC              },
    { "5601",           DTL_UHC              },
    { "CP949",          DTL_UHC              },
    { "CP20949",        DTL_UHC              },
    { "CP51949",        DTL_UHC              },
    
    { "GB18030",        DTL_GB18030          },
    { "CP54936",        DTL_GB18030          },
    
    { "ASCII",          DTL_SQL_ASCII        },
    { "SQL_ASCII",      DTL_SQL_ASCII        },
    { "646",            DTL_SQL_ASCII        },
    { "ANSI_X3.4-1968", DTL_SQL_ASCII        },
    { "ISO88591",       DTL_SQL_ASCII        },
    { "ISO_8859-1",     DTL_SQL_ASCII        },
    { "ISO8859-1",      DTL_SQL_ASCII        },
    { "ISO-8859-1",     DTL_SQL_ASCII        },
    { "CP1252",         DTL_SQL_ASCII        },
    { "CP20127",        DTL_SQL_ASCII        },
    { "CP28591",        DTL_SQL_ASCII        },
    
    { NULL,             DTL_CHARACTERSET_MAX }
};

/**
 * @brief CharacterSet 이름에 해당하는 dtlCharacterSet 반환
 * @param[in]  aName          character set 이름
 * @param[out] aCharacterSet  @a aName 해당하는 dtlCharacterSet
 * @param[out] aErrorStack    에러 스택
 */
stlStatus dtlGetCharacterSet( stlChar         * aName,
                              dtlCharacterSet * aCharacterSet,
                              stlErrorStack   * aErrorStack )
{
    const dtlCharsetMap * sMap;
    dtlCharacterSet       sCharacterSet = DTL_CHARACTERSET_MAX;

    for( sMap = gDtlCharacterSet; sMap->mName != NULL; sMap++ )
    {
        if( stlStrcasecmp( aName, sMap->mName ) == 0 )
        {
            sCharacterSet = sMap->mCharset;
            break;
        }
    }

    STL_TRY_THROW( sCharacterSet != DTL_CHARACTERSET_MAX,
                   RAMP_ERR_INVALID_CHARACTER_SET_IDENTIFIER );

    if( aCharacterSet != NULL )
    {
        *aCharacterSet = sCharacterSet;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_SET_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_SET_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief System의 CharacterSet 반환
 * @param[out] aCharacterSet  @a system에서 사용하는 dtlCharacterSet
 * @param[out] aErrorStack    에러 스택
 */
stlStatus dtlGetSystemCharacterset( dtlCharacterSet * aCharacterSet,
                                    stlErrorStack   * aErrorStack )
{
    stlChar * sCharacterset = NULL;
    stlChar   sNLS_CHARACTERSET[128];
    stlBool   sFound = STL_FALSE;

    *aCharacterSet = DTL_CHARACTERSET_MAX;

    STL_TRY( stlGetEnv( sNLS_CHARACTERSET,
                        128,
                        "GOLDILOCKS_NLS_CHARACTERSET",
                        &sFound,
                        aErrorStack ) == STL_SUCCESS );

    if( sFound == STL_TRUE )
    {
        /*
         * 환경변수 DB_NLS_CHARACTERSET
         */
        sCharacterset = sNLS_CHARACTERSET;
    }
    else
    {
        /*
         * 환경변수 LANG
         */
        sCharacterset = stlGetDefaultCharacterSet();
    }

    STL_TRY( dtlGetCharacterSet( sCharacterset,
                                 aCharacterSet,
                                 aErrorStack ) == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Character Set ID 의 유효성 검사 
 * @param[in] aCharacterSet      Character Set ID
 * @param[in] aErrorStack        Error Stack
 * @remarks
 */
stlStatus dtlCheckCharacterSet( dtlCharacterSet   aCharacterSet,
                                stlErrorStack   * aErrorStack )
{
    STL_TRY_THROW( (aCharacterSet >= DTL_SQL_ASCII) &&
                   (aCharacterSet < DTL_CHARACTERSET_MAX),
                   RAMP_ERR_INVALID_CHARACTERSET );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_CHARACTERSET )
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
 * @brief 문자가 지정된 character set에 속하는지 검사.
 * @param[in]  aCharacterSet  character set
 * @param[in]  aSource        aSource
 * @param[in]  aLength        입력 문자의 길이
 * @param[out] aMbLength      입력 문자의 multi byte length
 *                            valid인 경우( 입력문자의 multi byte length )
 * @param[out] aErrorStack    에러 스택
 */
stlStatus dtlMbStrVerify( dtlCharacterSet   aCharacterSet,
                          stlChar         * aSource,
                          stlInt64          aLength,
                          stlInt8         * aMbLength,
                          stlErrorStack   * aErrorStack )
{
    STL_TRY( dtsMbVerifyFuncList[aCharacterSet]( aSource,
                                                 aLength,
                                                 aMbLength,
                                                 aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 characterSet의 주어진 한문자에 대한 byte length를 구한다.
 * @param[in]  aCharacterSet character set
 * @param[in]  aSource       aSource
 * @param[out] aMbLength     aSource에 대한 byte length
 * @param[out] aErrorStack   에러 스택
 */
stlStatus dtlGetMbLength( dtlCharacterSet   aCharacterSet,
                          stlChar         * aSource, 
                          stlInt8         * aMbLength,
                          stlErrorStack   * aErrorStack )
{
    STL_TRY( dtsGetMbLengthFuncList[aCharacterSet]( aSource,
                                                    aMbLength,
                                                    aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 characterSet에 속하는 한문자에 대한 최대 byte length를 구한다.
 * @param[in]  aCharacterSet  character set
 * @return     characterSet의 한문자에 대한 최대 byte length
 */
stlInt8 dtlGetMbMaxLength( dtlCharacterSet   aCharacterSet )
{
    return dtsGetMbMaxLengthFuncList[aCharacterSet]();
}

/**
 * @brief 주어진 characterSet의 주어진 한 문자에 대한 display length를 구한다.
 * @param[in]  aCharacterSet  character set
 * @param[in]  aChar          aChar
 * @return display length
 *         <BR> null 인 경우 0 을 리턴 
 *         <BR> control or non-printable charater 인 경우 -1 을 리턴
 *         <BR> 그외의 경우 해당 display length를 리턴 
 */
stlInt32 dtlGetMbDisplayLength( dtlCharacterSet   aCharacterSet,
                                stlUInt8        * aChar )
{
    return dtsGetMbDisplayLengthFuncList[aCharacterSet]( aChar );
}

/**
 * @brief 각 characterSet string의 문자길이(문자갯수)를 구한다.
 * @param[in]  aCharacterSet  character set
 * @param[in]  aString        aString
 * @param[in]  aLimit         aSrcValue의 길이(byte length)
 * @param[out] aMbStrLen      계산된 문자길이 
 * @param[in]  aVectorFunc    Function Vector
 * @param[in]  aVectorArg     Vector Argument
 * @param[out] aErrorStack    에러 스택 
 */
stlStatus dtlGetMbstrlenWithLen( dtlCharacterSet   aCharacterSet,
                                 stlChar         * aString,
                                 stlInt64          aLimit,
                                 stlInt64        * aMbStrLen,
                                 dtlFuncVector   * aVectorFunc,
                                 void            * aVectorArg,
                                 stlErrorStack   * aErrorStack )
{
    return dtsGetMbstrlenWithLenFuncList[aCharacterSet]( aString,
                                                         aLimit,
                                                         aMbStrLen,
                                                         aVectorFunc,
                                                         aVectorArg,
                                                         aErrorStack );
}

/**
 * @brief character set간의 conversion 수행
 * @param[in]  aSourceCharacterSet source character set
 * @param[in]  aDestCharacterSet   dest character set
 * @param[in]  aSource             source string(uhc code)
 * @param[in]  aSourceLen          source string 길이
 * @param[out] aDest               dest를 저장할 메모리주소
 * @param[in]  aDestLen            dest string 길이
 * @param[out] aSourceOffset       변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength        @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack         aErrorStack
 */
stlStatus dtlMbConversion( dtlCharacterSet   aSourceCharacterSet,
                           dtlCharacterSet   aDestCharacterSet,
                           stlChar         * aSource,
                           stlInt64          aSourceLen,
                           stlChar         * aDest,
                           stlInt64          aDestLen,
                           stlInt64        * aSourceOffset,
                           stlInt64        * aTotalLength,
                           stlErrorStack   * aErrorStack )
{
    STL_TRY( dtsMbConversionFuncList[aSourceCharacterSet][aDestCharacterSet]
             ( aSource,
               aSourceLen,
               aDest,
               aDestLen,
               aSourceOffset,
               aTotalLength,
               aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 string의 주어진 string length에 대한 display 길이를 구한다. 
 * @param[in]  aCharacterSet  character set
 * @param[in]  aSource        aSource string
 * @param[in]  aSourceLength  aSource string length(byte length)
 * @param[out] aDisplayLength 계산된 display Length
 * @param[out] aErrorStack    에러 스택
 */
stlStatus dtlGetMbDisplayLengthWithLen( dtlCharacterSet   aCharacterSet,
                                        stlChar         * aSource,
                                        stlInt64          aSourceLength,
                                        stlInt64        * aDisplayLength,
                                        stlErrorStack   * aErrorStack )
{
    stlChar   * sStr   = aSource;
    stlInt64    sLimit = aSourceLength;
    stlInt8     sMbLen;
    stlInt32    sMbDspLen;
    stlInt64    sDisplayLength = 0;
    
    while( (sLimit > 0) && (*sStr) )
    {      
        STL_TRY( dtsGetMbLengthFuncList[aCharacterSet]( sStr,
                                                        & sMbLen,
                                                        aErrorStack )
                 == STL_SUCCESS );
        
        sMbDspLen = dtsGetMbDisplayLengthFuncList[aCharacterSet]( (stlUInt8 *)sStr );

//        if( sMbDspLen == -1 )
//        {
//            // -1인 리턴된 경우는 control or non-printable charator
//
//            /**
//             * @todo control or non-printable charator인 경우의 처리 ??? ...
//             */
//            sMbDspLen = 0;
//        }
//        else
//        {
//            // Do Nothing
//        }
                                                               
        sLimit = sLimit - sMbLen;
        sStr   = sStr + sMbLen;

        sDisplayLength = sDisplayLength + sMbDspLen;
    }

    *aDisplayLength = sDisplayLength;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief unicode encoding( utf16, utf32 ) 에서 Mb(muti byte) characterset간의 conversion 수행
 * @param[in]  aUniEncoding  source 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aMbCharset    dest 의 Mb Character set
 * @param[in]  aSource       source string(utf16, utf32의 code)
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtlUnicodeToMbConversion( dtlUnicodeEncoding   aUniEncoding,
                                    dtlCharacterSet      aMbCharset,
                                    void               * aSource,
                                    stlInt64             aSourceLen,
                                    stlChar            * aDest,
                                    stlInt64             aDestLen,
                                    stlInt64           * aSourceOffset,
                                    stlInt64           * aTotalLength,
                                    stlErrorStack      * aErrorStack )
{
    STL_TRY( dtsUnicodeToMbConversionFuncList[ aMbCharset ]( aUniEncoding,
                                                             aSource,
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
 * @brief Mb(muti byte) character set 을 unicode encoding ( utf16, utf32 ) 로 변환
 * @param[in]  aMbCharset    source 의 Mb Character set
 * @param[in]  aUniEncoding  dest 의 unicode encoding ( utf16 , utf32 )
 * @param[in]  aSource       source string ( Gb18030 )
 * @param[in]  aSourceLen    source string 길이
 * @param[out] aDest         dest를 저장할 메모리주소
 * @param[in]  aDestLen      @a aDest 의 길이
 * @param[out] aSourceOffset 변환이 성공한 @a aSource의 마지막 Offset
 * @param[out] aTotalLength  @a aDest의 공간이 충분할 경우 변환 성공 후 @a aDest의 전체 길이
 * @param[out] aErrorStack   aErrorStack
 */
stlStatus dtlMbToUnicodeConversion( dtlCharacterSet      aMbCharset,
                                    dtlUnicodeEncoding   aUniEncoding,
                                    stlChar            * aSource,
                                    stlInt64             aSourceLen,
                                    void               * aDest,
                                    stlInt64             aDestLen,
                                    stlInt64           * aSourceOffset,
                                    stlInt64           * aTotalLength,
                                    stlErrorStack      * aErrorStack )
{
    STL_TRY( dtsMbToUnicodeConversionFuncList[ aMbCharset ]( aUniEncoding,
                                                             aSource,
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



/** @} */
