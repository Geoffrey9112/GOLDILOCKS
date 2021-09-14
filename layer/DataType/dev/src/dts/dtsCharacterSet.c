/*******************************************************************************
 * dtsCharacterSet.c
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
 * @file dtsCharacterSet.c
 * @brief DataType Layer Character set 관련 함수 정의
 */

#include <dtl.h>
#include <dtDef.h>

#include <dtsCharacterSet.h>

/**
 * @addtogroup dts
 * @{
 */

/* sorted list of non-overlapping intervals of non-spacing characters */
static const dtsMbInterval dtsCombining[] = {
    {0x0300, 0x034E}, {0x0360, 0x0362}, {0x0483, 0x0486},
    {0x0488, 0x0489}, {0x0591, 0x05A1}, {0x05A3, 0x05B9},
    {0x05BB, 0x05BD}, {0x05BF, 0x05BF}, {0x05C1, 0x05C2},
    {0x05C4, 0x05C4}, {0x064B, 0x0655}, {0x0670, 0x0670},
    {0x06D6, 0x06E4}, {0x06E7, 0x06E8}, {0x06EA, 0x06ED},
    {0x070F, 0x070F}, {0x0711, 0x0711}, {0x0730, 0x074A},
    {0x07A6, 0x07B0}, {0x0901, 0x0902}, {0x093C, 0x093C},
    {0x0941, 0x0948}, {0x094D, 0x094D}, {0x0951, 0x0954},
    {0x0962, 0x0963}, {0x0981, 0x0981}, {0x09BC, 0x09BC},
    {0x09C1, 0x09C4}, {0x09CD, 0x09CD}, {0x09E2, 0x09E3},
    {0x0A02, 0x0A02}, {0x0A3C, 0x0A3C}, {0x0A41, 0x0A42},
    {0x0A47, 0x0A48}, {0x0A4B, 0x0A4D}, {0x0A70, 0x0A71},
    {0x0A81, 0x0A82}, {0x0ABC, 0x0ABC}, {0x0AC1, 0x0AC5},
    {0x0AC7, 0x0AC8}, {0x0ACD, 0x0ACD}, {0x0B01, 0x0B01},
    {0x0B3C, 0x0B3C}, {0x0B3F, 0x0B3F}, {0x0B41, 0x0B43},
    {0x0B4D, 0x0B4D}, {0x0B56, 0x0B56}, {0x0B82, 0x0B82},
    {0x0BC0, 0x0BC0}, {0x0BCD, 0x0BCD}, {0x0C3E, 0x0C40},
    {0x0C46, 0x0C48}, {0x0C4A, 0x0C4D}, {0x0C55, 0x0C56},
    {0x0CBF, 0x0CBF}, {0x0CC6, 0x0CC6}, {0x0CCC, 0x0CCD},
    {0x0D41, 0x0D43}, {0x0D4D, 0x0D4D}, {0x0DCA, 0x0DCA},
    {0x0DD2, 0x0DD4}, {0x0DD6, 0x0DD6}, {0x0E31, 0x0E31},
    {0x0E34, 0x0E3A}, {0x0E47, 0x0E4E}, {0x0EB1, 0x0EB1},
    {0x0EB4, 0x0EB9}, {0x0EBB, 0x0EBC}, {0x0EC8, 0x0ECD},
    {0x0F18, 0x0F19}, {0x0F35, 0x0F35}, {0x0F37, 0x0F37},
    {0x0F39, 0x0F39}, {0x0F71, 0x0F7E}, {0x0F80, 0x0F84},
    {0x0F86, 0x0F87}, {0x0F90, 0x0F97}, {0x0F99, 0x0FBC},
    {0x0FC6, 0x0FC6}, {0x102D, 0x1030}, {0x1032, 0x1032},
    {0x1036, 0x1037}, {0x1039, 0x1039}, {0x1058, 0x1059},
    {0x1160, 0x11FF}, {0x17B7, 0x17BD}, {0x17C6, 0x17C6},
    {0x17C9, 0x17D3}, {0x180B, 0x180E}, {0x18A9, 0x18A9},
    {0x200B, 0x200F}, {0x202A, 0x202E}, {0x206A, 0x206F},
    {0x20D0, 0x20E3}, {0x302A, 0x302F}, {0x3099, 0x309A},
    {0xFB1E, 0xFB1E}, {0xFE20, 0xFE23}, {0xFEFF, 0xFEFF},
    {0xFFF9, 0xFFFB}
};


dtsGetMbLengthFunc  dtsGetMbLengthFuncList[ DTL_CHARACTERSET_MAX ] =
{
    dtsGetAsciiMbLength,
    dtsGetUtf8MbLength,
    dtsGetUhcMbLength,
    dtsGetGb18030MbLength
};

dtsGetMbMaxLengthFunc  dtsGetMbMaxLengthFuncList[ DTL_CHARACTERSET_MAX ] =
{
    dtsGetAsciiMbMaxLength,
    dtsGetUtf8MbMaxLength,
    dtsGetUhcMbMaxLength,
    dtsGetGb18030MbMaxLength
};

dtsMbVerifyFunc  dtsMbVerifyFuncList[ DTL_CHARACTERSET_MAX ] =
{
    dtsAsciiVerify,
    dtsUtf8Verify,
    dtsUhcVerify,
    dtsGb18030Verify
};

dtsGetMbDisplayLengthFunc dtsGetMbDisplayLengthFuncList[ DTL_CHARACTERSET_MAX ] =
{
    dtsGetAsciiDisplayLength,
    dtsGetUtf8DisplayLength,
    dtsGetUhcDisplayLength,
    dtsGetGb18030DisplayLength
};

dtsGetMbstrlenWithLenFunc dtsGetMbstrlenWithLenFuncList[ DTL_CHARACTERSET_MAX ] =
{
    dtsGetAsciiMbstrlenWithLen,
    dtsGetUtf8MbstrlenWithLen,
    dtsGetUhcMbstrlenWithLen,
    dtsGetGb18030MbstrlenWithLen
};


const stlBool dtsCharsetBinaryComparable[ DTL_CHARACTERSET_MAX ] =
{
    STL_TRUE,  /* DTL_SQL_ASCII */
    STL_TRUE,  /* DTL_UTF8 */
    STL_FALSE, /* DTL_UHC */
    STL_FALSE  /* DTL_GB18030 */
};

/*******************************************************************************
 *  dtsGetMbLengthFunc의  각 함수들 
 ******************************************************************************/

/**
 * @brief 주어진 Ascii 문자의 byte length를 구한다.
 * @param[in]  aSource      aSource
 * @param[out] aMbLength    aSource의 byte length
 * @param[out] aErrorStack  에러 스택 
 */
stlStatus dtsGetAsciiMbLength( stlChar       * aSource,
                               stlInt8       * aMbLength,
                               stlErrorStack * aErrorStack )
{
    DTS_GET_ASCII_MBLENGTH( aSource,
                            aMbLength );
    
    return STL_SUCCESS;
}

/**
 * @brief 주어진 Utf8 문자의 byte length를 구한다.
 * @param[in]  aSource      aSource
 * @param[out] aMbLength    aSource의 byte length
 * @param[out] aErrorStack  에러 스택
 * @remarks
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
stlStatus dtsGetUtf8MbLength( stlChar       * aSource,
                              stlInt8       * aMbLength,
                              stlErrorStack * aErrorStack )
{
    DTS_GET_UTF8_MBLENGTH( aSource,
                           aMbLength );
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_CHARACTER_LENGTH )
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
 * @brief 주어진 uhc(ms949) 문자의 byte length를 구한다.
 * @param[in]  aSource      aSource
 * @param[out] aMbLength    aSource의 byte length
 * @param[out] aErrorStack  에러 스택 
 */
stlStatus dtsGetUhcMbLength( stlChar       * aSource,
                             stlInt8       * aMbLength,
                             stlErrorStack * aErrorStack )
{
    DTS_GET_UHC_MBLENGTH( aSource,
                          aMbLength );
    
    return STL_SUCCESS;
}

/**
 * @brief 주어진 GB18030 문자의 byte length를 구한다.
 * @param[in]  aSource      aSource
 * @param[out] aMbLength    aSource의 byte length
 * @param[out] aErrorStack  에러 스택 
 */
stlStatus dtsGetGb18030MbLength( stlChar       * aSource,
                                 stlInt8       * aMbLength,
                                 stlErrorStack * aErrorStack )
{
    DTS_GET_GB18030_MBLENGTH( aSource,
                              aMbLength );
    
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

/*******************************************************************************
 *  dtsGetMbMaxLengthFunc의  각 함수들 
 ******************************************************************************/

/**
 * @brief Ascii 문자의 최대 byte length를 구한다.
 * @return 최대 byte length
 */
stlInt8 dtsGetAsciiMbMaxLength()
{
    /* 1 */ 
    return DTS_GET_ASCII_MB_MAX_LENGTH;
}

/**
 * @brief Utf8 문자의 최대 byte length를 구한다.
 * @return 최대 byte length
 */
stlInt8 dtsGetUtf8MbMaxLength()
{
    /* 4 */ 
    return DTS_GET_UTF8_MB_MAX_LENGTH;
}

/**
 * @brief uhc(ms949) 문자의 최대 byte length를 구한다.
 * @return 최대 byte length
 */
stlInt8 dtsGetUhcMbMaxLength()
{
    /* 2 */ 
    return DTS_GET_UHC_MB_MAX_LENGTH;
}

/**
 * @brief gb18030 문자의 최대 byte length를 구한다.
 * @return 최대 byte length
 */
stlInt8 dtsGetGb18030MbMaxLength()
{
    /* 4 */ 
    return DTS_GET_GB18030_MB_MAX_LENGTH;
}

/*******************************************************************************
 *  dtsMbVerifyFunc의  각 함수들 
 ******************************************************************************/

/**
 * @brief  ascii 문자에 대한 validate
 * @param[in]   aSource
 * @param[in]   aLength
 * @param[out]  aMbLength
 * @param[out]  aErrorStack 
 * @todo ascii 문자에 대해서는 특별한 검사를 진행하지 않는다.
 *       필요한 곳에서 00~7F까지의 character만 검사한다. 
 */
stlStatus dtsAsciiVerify( stlChar       * aSource,
                          stlInt64        aLength,
                          stlInt8       * aMbLength,
                          stlErrorStack * aErrorStack )
{
    *aMbLength = 1;
    
    return STL_SUCCESS;    
}

/**
 * @brief  Utf8 문자에 대한 validate
 * @param[in]   aSource
 * @param[in]   aLength
 * @param[out]  aMbLength
 * @param[out]  aErrorStack
 */
stlStatus dtsUtf8Verify( stlChar       * aSource,
                         stlInt64        aLength,
                         stlInt8       * aMbLength,
                         stlErrorStack * aErrorStack )
{
    stlInt8  sMbLength;
    
    DTS_GET_UTF8_MBLENGTH( aSource,
                           &sMbLength );
    
    STL_TRY_THROW( aLength >= sMbLength, ERROR_CHARACTER_NOT_IN_REPERTOIRE );
    
    STL_TRY( dtsUtf8Validate( aSource,
                              sMbLength,
                              aErrorStack )
             == STL_SUCCESS );
    
    *aMbLength = sMbLength;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_CHARACTER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }    
    
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
 * @brief  uhc(ms949) 문자에 대한 validate
 * @param[in]   aSource
 * @param[in]   aLength
 * @param[out]  aMbLength
 * @param[out]  aErrorStack 
 * @remarks
 * 
 * <BR>
 *  <table>
 *    <tr>
 *      <td>   </td>
 *      <td> 코드범위 </td>
 *    </tr>
 *    <tr>
 *      <td> first byte range td>
 *      <td> @code  0x81 ~ 0xFE  @endcode </td>
 *    </tr>
 *    <tr>
 *      <td> second byte range </td>
 *      <td> @code  0x41 ~ 0x5A  @endcode <BR>
 *           @code  0x61 ~ 0x7A  @endcode <BR>
 *           @code  0x81 ~ 0xFE  @endcode   </td>
 *    </tr>
 *  </table> 
 */
stlStatus dtsUhcVerify( stlChar       * aSource,
                        stlInt64        aLength,
                        stlInt8       * aMbLength,
                        stlErrorStack * aErrorStack )
{
    stlInt8    sMbLength;
    stlBool    sResult;
    
    sResult = STL_TRUE;

    DTS_IS_VERIFY_UHC( aSource,
                       aLength,
                       & sMbLength,
                       & sResult );
    
    STL_TRY_THROW( sResult == STL_TRUE, ERROR_CHARACTER_NOT_IN_REPERTOIRE );
    
    *aMbLength = sMbLength;
    
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
 * @brief  Gb18030 문자에 대한 validate
 * @param[in]   aSource
 * @param[in]   aLength
 * @param[out]  aMbLength
 * @param[out]  aErrorStack
 * @remarks
 * 
 * <BR>
 *  <table>
 *    <tr>
 *      <td>   </td>
 *      <td> 코드범위 </td>
 *    </tr>
 *    <tr>
 *      <td> 1 byte range td>
 *      <td> @code  0x00 ~ 0x7F  @endcode </td>
 *    </tr>
 *    <tr>
 *      <td> 2 byte range </td>
 *      <td> @code  0x81 ~ 0xFE | 0x40 ~ 0x7E  @endcode <BR>
 *           @code  0X81 ~ 0xFE | 0x80 ~ 0xFE  @endcode <BR>
 *    </tr>
 *    <tr>
 *      <td> 4 byte range </td>
 *      <td> @code  0x81 ~ 0xFE | 0x30 ~ 0x39 | 0x81 ~ 0xFE | 0x30 ~ 0x39  @endcode <BR>
 *    </tr>
 *  </table> 
 */
stlStatus dtsGb18030Verify( stlChar       * aSource,
                            stlInt64        aLength,
                            stlInt8       * aMbLength,
                            stlErrorStack * aErrorStack )
{
    stlInt8  sMbLength;
    stlBool  sResult;
    
    sResult = STL_TRUE;
    
    DTS_IS_VERIFY_GB18030( aSource,
                           aLength,
                           & sMbLength,
                           & sResult );
    
    STL_TRY_THROW( sResult == STL_TRUE, ERROR_CHARACTER_NOT_IN_REPERTOIRE );
    
    *aMbLength = sMbLength;
    
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

/*******************************************************************************
 * dtsGetMbDisplayLengthFunc의  각 함수들 
 ******************************************************************************/

stlInt32 dtsGetAsciiDisplayLength( stlUInt8 * aChar )
{
    /* stlInt32   sDisplayLength; */
    
    /* if( *aChar == '\0' ) */
    /* { */
    /*     sDisplayLength = 0; */
    /* } */
    /* else if( (*aChar < 0x20) || (*aChar == 0x7f) ) */
    /* { */
    /*     /\** */
    /*      * @todo control or non-printable character에 대한 처리.  */
    /*      *\/ */
        
    /*     // control or non-printable characters */
    /*     sDisplayLength = -1; */
    /* } */
    /* else */
    /* { */
    /*     sDisplayLength = 1; */
    /* } */

    return DTS_GET_ASCII_MB_MAX_LENGTH;
}

stlInt32 dtsGetUtf8DisplayLength( stlUInt8 * aChar )
{
    dtsWchar   sWchar;
    stlInt32   sDisplayLength;

    sWchar = dtsUtf2Ucs( aChar );
    
    sDisplayLength = dtsUcsWcWidth( sWchar );

    return sDisplayLength;
}

stlInt32 dtsGetUhcDisplayLength( stlUInt8 * aChar )
{
    stlInt32 sDisplayLength;
    
    if( DTS_IS_HIGH_BIT_SET(*aChar) == STL_TRUE )
    {
        sDisplayLength = 2;
    }
    else
    {
        sDisplayLength = dtsGetAsciiDisplayLength( aChar );
    }

    return sDisplayLength;
}

stlInt32 dtsGetGb18030DisplayLength( stlUInt8 * aChar )
{
    stlInt32 sDisplayLength;
    
    if( DTS_IS_HIGH_BIT_SET(*aChar) == STL_TRUE )
    {
        sDisplayLength = 2;
    }
    else
    {
        sDisplayLength = dtsGetAsciiDisplayLength( aChar );
    }

    return sDisplayLength;
}


/*******************************************************************************
 *  dtsGetMbstrlenWithLenFunc의 각 함수들
 ******************************************************************************/

/**
 * @brief 주어진 Ascii characterSet string의 문자길이를 구한다.(문자갯수)
 * @param[in]  aString      aString
 * @param[in]  aLimit       aSrcValue의 길이(byte length)
 * @param[out] aMbStrLen    계산된 문자길이 
 * @param[in]  aVectorFunc  Function Vector
 * @param[in]  aVectorArg   Vector Argument
 * @param[out] aErrorStack  에러 스택 
 */
stlStatus dtsGetAsciiMbstrlenWithLen( stlChar         * aString,
                                      stlInt64          aLimit,
                                      stlInt64        * aMbStrLen,
                                      dtlFuncVector   * aVectorFunc,
                                      void            * aVectorArg,
                                      stlErrorStack   * aErrorStack )
{
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( (aLimit == 0) || (aString != NULL), aErrorStack);
    DTL_PARAM_VALIDATE( aMbStrLen != NULL, aErrorStack);
    
    *aMbStrLen = aLimit;

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 utf8 characterSet string의 문자길이를 구한다.(문자갯수)
 * @param[in]  aString      aString
 * @param[in]  aLimit       aSrcValue의 길이(byte length)
 * @param[out] aMbStrLen    계산된 문자길이 
 * @param[in]  aVectorFunc  Function Vector
 * @param[in]  aVectorArg   Vector Argument
 * @param[out] aErrorStack  에러 스택 
 */
stlStatus dtsGetUtf8MbstrlenWithLen( stlChar         * aString,
                                     stlInt64          aLimit,
                                     stlInt64        * aMbStrLen,
                                     dtlFuncVector   * aVectorFunc,
                                     void            * aVectorArg,
                                     stlErrorStack   * aErrorStack )
{
    stlInt8     sLen;
    stlInt64    sMbStrLen;
    stlInt64    sLimit;
    stlChar   * sStr;

    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( (aLimit == 0) || (aString != NULL), aErrorStack);
    DTL_PARAM_VALIDATE( aMbStrLen != NULL, aErrorStack);
    
    sLimit    = aLimit;
    sStr      = aString;
    sLen      = 0;
    sMbStrLen = 0;

    while( sLimit > 0 )
    {
        DTS_GET_UTF8_MBLENGTH( sStr,
                               & sLen );
        
        sLimit = sLimit - sLen;
        sStr   = sStr + sLen;
        
        sMbStrLen = sMbStrLen + 1;
    }
    
    *aMbStrLen = sMbStrLen;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_CHARACTER_LENGTH )
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
 * @brief 주어진 uhc characterSet string의 문자길이를 구한다.(문자갯수)
 * @param[in]  aString      aString
 * @param[in]  aLimit       aSrcValue의 길이(byte length)
 * @param[out] aMbStrLen    계산된 문자길이 
 * @param[in]  aVectorFunc  Function Vector
 * @param[in]  aVectorArg   Vector Argument
 * @param[out] aErrorStack  에러 스택 
 */
stlStatus dtsGetUhcMbstrlenWithLen( stlChar         * aString,
                                    stlInt64          aLimit,
                                    stlInt64        * aMbStrLen,
                                    dtlFuncVector   * aVectorFunc,
                                    void            * aVectorArg,
                                    stlErrorStack   * aErrorStack )
{
    stlInt8     sLen;
    stlInt64    sMbStrLen;
    stlInt64    sLimit;
    stlChar   * sStr;

    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( (aLimit == 0) || (aString != NULL), aErrorStack);
    DTL_PARAM_VALIDATE( aMbStrLen != NULL, aErrorStack);
    
    sLimit    = aLimit;
    sStr      = aString;
    sLen      = 0;
    sMbStrLen = 0;

    while( sLimit > 0 )
    {
        DTS_GET_UHC_MBLENGTH( sStr,
                              & sLen );
        
        sLimit    = sLimit - sLen;
        sStr      = sStr + sLen;
        
        sMbStrLen = sMbStrLen + 1;
    }

    *aMbStrLen = sMbStrLen;

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 gb18030 characterSet string의 문자길이를 구한다.(문자갯수)
 * @param[in]  aString      aString
 * @param[in]  aLimit       aSrcValue의 길이(byte length)
 * @param[out] aMbStrLen    계산된 문자길이 
 * @param[in]  aVectorFunc  Function Vector
 * @param[in]  aVectorArg   Vector Argument
 * @param[out] aErrorStack  에러 스택 
 */
stlStatus dtsGetGb18030MbstrlenWithLen( stlChar         * aString,
                                        stlInt64          aLimit,
                                        stlInt64        * aMbStrLen,
                                        dtlFuncVector   * aVectorFunc,
                                        void            * aVectorArg,
                                        stlErrorStack   * aErrorStack )
{
    stlInt8     sLen;
    stlInt64    sMbStrLen;
    stlInt64    sLimit;
    stlChar   * sStr;

    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( (aLimit == 0) || (aString != NULL), aErrorStack);
    DTL_PARAM_VALIDATE( aMbStrLen != NULL, aErrorStack);
    
    sLimit    = aLimit;
    sStr      = aString;
    sLen      = 0;
    sMbStrLen = 0;

    while( sLimit > 0 )
    {
        DTS_GET_GB18030_MBLENGTH( sStr,
                                  & sLen );
        
        sLimit    = sLimit - sLen;
        sStr      = sStr + sLen;
        
        sMbStrLen = sMbStrLen + 1;
    }

    *aMbStrLen = sMbStrLen;

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


/*******************************************************************************
 *  기타 내부 함수들 
 ******************************************************************************/

/**
 * @brief 주어진 문자가 utf8인지를 검사한다.
 * @param[in]  aSource      aSource
 * @param[in]  aLength      aSource의 byte length   
 * @param[out] aErrorStack  에러 스택
 * @remarks
 * RFC 3629 UTF8 코드범위는 아래표와 같으며, C0, C1, F5~FF 는 코드범위에 나타나지 않는다.
 * <BR>
 *  <table>
 *    <tr>
 *      <td>   </td>
 *      <td> 코드범위 </td>
 *    </tr>
 *    <tr>
 *      <td> UTF8-1  </td>
 *      <td> @code  %x00-7F  @endcode </td>
 *    </tr>
 *    <tr>
 *      <td> UTF8-2 </td>
 *      <td> @code  %xC2-DF  %x80-BF  @endcode </td>
 *    </tr>
 *    <tr>
 *      <td> UTF8-3  </td>
 *      <td> @code  %xE0     %xA0-BF  %x80-BF  @endcode <BR>
 *           @code  %xE1-EC  %x80-BF  %x80-BF  @endcode <BR>
 *           @code  %xED     %x80-9F  %x80-BF  @endcode <BR>
 *           @code  %xEE-EF  %x80-BF  %x80-BF  @endcode
 *            </td>
 *    </tr>
 *    <tr>
 *      <td> UTF8-4 </td>
 *      <td> @code  %xF0     %x90-BF  %x80-BF  %x80-BF  @endcode <BR>
 *           @code  %xF1-F3  %x80-BF  %x80-BF  %x80-BF  @endcode <BR>
 *           @code  %xF4     %x80-8F  %x80-BF  %x80-BF  @endcode </td>
 *    </tr>
 *  </table> 
 */
stlStatus dtsUtf8Validate( stlChar       * aSource,
                           stlInt8         aLength,
                           stlErrorStack * aErrorStack )
{
    stlBool    sResult;
    
    sResult = STL_TRUE;
    
    DTS_UTF8_VALIDATE( aSource,
                       aLength,
                       & sResult );

    STL_TRY_THROW( sResult == STL_TRUE, ERROR_CHARACTER_NOT_IN_REPERTOIRE );
    
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
 * @brief utf8 code를 ucs code로 변환
 * @param[in] aChar  utf8 code
 * @return ucs code
 */
dtsWchar dtsUtf2Ucs( const stlUInt8  * aChar )
{
    dtsWchar sUcsChar;
    
    if( (*aChar & 0x80) == 0 )
    {
        sUcsChar = aChar[0];
    }
    else if( (*aChar & 0xe0) == 0xc0 )
    {
        sUcsChar = ( ((aChar[0] & 0x1f) << 6) | (aChar[1] & 0x3f) );
    }
    else if( (*aChar & 0xf0) == 0xe0 )
    {
        sUcsChar = ( ((aChar[0] & 0x0f) << 12) | ((aChar[1] & 0x3f) << 6) |
                     (aChar[2] & 0x3f) );
    }
    else if( (*aChar & 0xf8) == 0xf0 )
    {
        sUcsChar = ( ((aChar[0] & 0x07) << 18) | ((aChar[1] & 0x3f) << 12) |
                     ((aChar[2] & 0x3f) << 6) | (aChar[3] & 0x3f) );
    }
    else
    {
        /* invalid */
        sUcsChar = 0xffffffff;
    }

    return sUcsChar;
}

/*
 * <참조> http://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
 * 
 * The following functions define the column width of an ISO 10646
 * character as follows:
 *
 *	  - The null character (U+0000) has a column width of 0.
 *
 *	  - Other C0/C1 control characters and DEL will lead to a return
 *		value of -1.
 *
 *	  - Non-spacing and enclosing combining characters (general
 *		category code Mn or Me in the Unicode database) have a
 *		column width of 0.
 *
 *	  - Other format characters (general category code Cf in the Unicode
 *		database) and ZERO WIDTH SPACE (U+200B) have a column width of 0.
 *
 *	  - Hangul Jamo medial vowels and final consonants (U+1160-U+11FF)
 *		have a column width of 0.
 *
 *	  - Spacing characters in the East Asian Wide (W) or East Asian
 *		FullWidth (F) category as defined in Unicode Technical
 *		Report #11 have a column width of 2.
 *
 *	  - All remaining characters (including all printable
 *		ISO 8859-1 and WGL4 characters, Unicode control characters,
 *		etc.) have a column width of 1.
 *
 * This implementation assumes that wchar_t characters are encoded
 * in ISO 10646.
 */
stlInt32 dtsUcsWcWidth( dtsWchar aUcsChar )
{
    stlInt32  sUcsWidth;
        
    /* test for 8-bit control characters */    
    if( aUcsChar == 0 )
    {
        sUcsWidth = 0;
    }
    else
    {
        // Do Nothing
    }

    if( (aUcsChar < 0x20) ||
        ((aUcsChar >= 0x7f) && (aUcsChar < 0xa0)) ||
        (aUcsChar > 0x0010ffff) )
    {
        /* sUcsWidth = -1; */
        sUcsWidth = 0;        
    }
    else
    {
        // Do Nothing
    }

    /* binary search in table of non-spacing characters */
    if( dtsMbIntervalBinarySearch(
            aUcsChar,
            dtsCombining,
            STL_SIZEOF(dtsCombining) / STL_SIZEOF(dtsMbInterval) - 1) )
    {
        sUcsWidth = 0;        
    }
    else
    {
        // Do Nothing
    }

    /*
     * if we arrive here, ucs is not a combining or C0/C1 control character
     */
    
    sUcsWidth = 1 +
        ((aUcsChar >= 0x1100) &&
         ((aUcsChar <= 0x115f) ||               /* Hangul Jamo init. consonants */
          ((aUcsChar >= 0x2e80) && (aUcsChar <= 0xa4cf) &&
           ((aUcsChar & ~0x0011) != 0x300a) &&
           (aUcsChar != 0x303f)) ||             /* CJK ... Yi */
          ((aUcsChar >= 0xac00) && (aUcsChar <= 0xd7a3)) ||  /* Hangul Syllables */
          ((aUcsChar >= 0xf900) && (aUcsChar <= 0xfaff)) ||  /* CJK Compatibility
                                                              * Ideographs */
          ((aUcsChar >= 0xfe30) && (aUcsChar <= 0xfe6f)) ||  /* CJK Compatibility Forms */
          ((aUcsChar >= 0xff00) && (aUcsChar <= 0xff5f)) ||  /* Fullwidth Forms */
          ((aUcsChar >= 0xffe0) && (aUcsChar <= 0xffe6)) ||
          ((aUcsChar >= 0x20000) && (aUcsChar <= 0x2ffff))));
    
    return sUcsWidth;
}

stlInt32 dtsMbIntervalBinarySearch( dtsWchar              aUcsChar,
                                    const dtsMbInterval * aMbInterval,
                                    stlInt32              aMax )
{
    stlInt32   sMin = 0;
    stlInt32   sMid;
    stlInt32   sResult = 0;

    if( (aUcsChar < aMbInterval[0].mFirst) ||
        (aUcsChar > aMbInterval[aMax].mLast) )
    {
        sResult = 0;
    }
    else
    {
        while( aMax >= sMin )
        {
            sMid = (sMin + aMax) / 2;
            
            if( aUcsChar > aMbInterval[sMid].mLast )
            {
                sMin = sMid + 1;
            }
            else if( aUcsChar < aMbInterval[sMid].mFirst )
            {
                aMax = sMid - 1;
            }
            else
            {
                sResult = 1;
                break;
            }
        }
    }
    
    return sResult;
}
    

/** @} */
