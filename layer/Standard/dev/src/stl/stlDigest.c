/*******************************************************************************
 * stlStrings.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stlStrings.c 1533 2011-07-26 05:37:58Z mkkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file stlStrings.c
 * @brief Standard Layer Strings Routines
 */

#include <stl.h>
#include <strDigest.h>

/**
 * @addtogroup stlStrings
 * @{
 */



/**
 * @brief sha1 input에 대한 digest된 결과값을 output한다.
 * @param[in]  aValue       digest할 data
 * @param[in]  aValueLen    digest할 data의 길이
 * @param[out] aResult      digest된 값이 저장될 버퍼.
 *                          <BR> 여기에는 STL_DIGEST_SIZE(20)byte의
 *                          <BR> 변수가 들어와야 한다.
 * @param[out] aErrorStack  에러 스택
 */
stlStatus stlDigest( stlUInt8           * aValue,
                     stlUInt64            aValueLen,
                     stlUInt8             aResult[STL_DIGEST_SIZE],
                     stlErrorStack      * aErrorStack )
{
    return strDigest( aValue,
                      aValueLen,
                      aResult,
                      aErrorStack );
}


/**
 * @brief Plain Text 에 대한 Encrypted Hex String 을 생성한다.
 * @param[in]  aPlainText           Plain Text
 * @param[in]  aPlainTextLen        Plain Text Length
 * @param[out] aEncryptedText       Encrypted Text
 * @param[in]  aEncryptedBufferSize aEncryptedText 의 버퍼 공간의 크기
 * @param[in]  aErrorStack          Error Stack
 * @remarks
 * aEncryptedBufferSize 는 STL_DIGEST_HEX_SIZE (41bytes) 보다 크거나 같아야 한다.
 */
stlStatus stlMakeEncryptedPassword( stlChar * aPlainText,
                                    stlInt32  aPlainTextLen,
                                    stlChar * aEncryptedText,
                                    stlInt32  aEncryptedBufferSize,
                                    stlErrorStack * aErrorStack )
{
    stlInt32     i = 0;
    stlUInt8     sResult[STL_DIGEST_SIZE] = {0,};
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPlainText != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aPlainTextLen > 0, aErrorStack );
    STL_PARAM_VALIDATE( aEncryptedText != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aEncryptedBufferSize >= STL_DIGEST_HEX_SIZE, aErrorStack );

    /**
     * SHA-1 암호화 
     */

    STL_TRY( stlDigest( (stlUInt8 *) aPlainText,
                        aPlainTextLen,
                        sResult,
                        aErrorStack )
             == STL_SUCCESS );

    /**
     * Hex 변환
     */

    for ( i = 0; i < STL_DIGEST_SIZE; i++ )
    {
        stlSnprintf( & aEncryptedText[i*2],
                     aEncryptedBufferSize,
                     "%02x",
                     sResult[i] );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Data input에 대한 Crc32 값을 계산한다.
 * @param [in]     aData            Crc32 계산되는 Data
 * @param [in]     aSize            Data의 Size
 * @param [out]    aCrc32           Crc32 초기값이자 반환되는 값
 * @param [in,out] aErrorStack      에러 스택
 */ 
stlStatus stlCrc32( void          * aData,
                    stlInt32        aSize,
                    stlUInt32     * aCrc32,
                    stlErrorStack * aErrorStack )
{    
    STL_TRY( strCrc32( aData,
                       aSize,
                       aCrc32,
                       aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
