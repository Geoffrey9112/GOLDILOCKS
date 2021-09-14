/*******************************************************************************
 * ztcmEncrypt.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztcmEncrypt.c
 * @brief GlieseTool Cyclone Encrypt Routines
 */

#include <goldilocks.h>
#include <ztcl.h>
#include <ztc.h>

extern ztcConfigure   gConfigure;

/**
 * @brief passwd encryption
 */


/**
 * Base64 Encoding Table
 */
static const stlChar gZtcmBase64[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

/**
 * Base64 Decoding Table
 */
static const stlInt32 gZtcmDecodeBase64[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
    };


stlStatus ztcmPrintCipherText( stlChar          * aPlainText,
                               stlErrorStack    * aErrorStack)
{
    stlChar     sCipherText[ZTC_MAX_CIPHER_SIZE];
    stlChar     sB64Text[ZTC_MAX_BASE64_SIZE];
    stlSize     sCipherTextLen;

    STL_TRY_THROW( stlStrlen( gConfigure.mKey ) != 0, RAMP_NO_KEY );

    sCipherTextLen = STL_SIZEOF( sCipherText );

    /**
     * string뒤의 0x00까지 encrypt한다.
     */
    STL_TRY( ztcmEncrypt( aPlainText, stlStrlen( aPlainText ) + 1, gConfigure.mKey, sCipherText, &sCipherTextLen, aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztcmBase64Encode( sCipherText, sCipherTextLen, sB64Text, STL_SIZEOF( sB64Text ), aErrorStack )
             == STL_SUCCESS );

    stlPrintf( "Cyclonem Encrypted Passwd : \'%s\'\n", sB64Text );

    return STL_SUCCESS;

    STL_CATCH( RAMP_NO_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_NO_KEY_PARAMETER,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcmEncrypt( stlChar          * aPlainText,
                       stlSize            aPlainTextLen,
                       stlChar            aKey[16],
                       stlChar          * aCipherText,
                       stlSize          * aCipherTextLen,
                       stlErrorStack    * aErrorStack )
{
    stlInt32    sIdx;
    stlUInt8    sPadLen;
    stlSize     sLen;
    stlSize     sBufSize;

    sLen = aPlainTextLen;
    sBufSize = *aCipherTextLen;

    /**
     * padding은 최소 1개가 있어야 한다.
     * 그래야 padding제거시 padded된것이 몇byte인지 알수 있다.
     */
    sPadLen = 8 - ( sLen % 8 );

    STL_TRY_THROW( sLen + sPadLen <= sBufSize, RAMP_OVER_FLOW );

    /**
     * padding
     */
    stlMemcpy( aCipherText, aPlainText, sLen );

    for( sIdx = 0; sIdx < sPadLen; sIdx++ )
    {
        aCipherText[sLen] = sPadLen;
        sLen++;
    }

    STL_DASSERT( sLen % 8 == 0 );

    for( sIdx = 0; sIdx < sLen; sIdx += 8 )
    {
        STL_TRY( ztcmEncrypt8byte( (stlUInt32 *)(aCipherText + sIdx), (stlUInt32 *)aKey, aErrorStack)
                 == STL_SUCCESS );
    }

    *aCipherTextLen = sLen;

    return STL_SUCCESS;

    STL_CATCH( RAMP_OVER_FLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_VALUE_TOO_LONG,
                      NULL,
                      aErrorStack,
                      "PASSWD");
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmDecrypt( stlChar          * aCipherText,
                       stlSize            aCipherTextLen,
                       stlChar            aKey[16],
                       stlChar          * aPlainText,
                       stlSize          * aPlainTextLen,
                       stlErrorStack    * aErrorStack )
{
    stlInt32    sIdx;
    stlSize     sBufSize;
    stlUInt8    sPadLen;

    STL_TRY_THROW( aCipherTextLen % 8 == 0, RAMP_ERROR_DECRYPT );

    sBufSize = *aPlainTextLen;

    STL_TRY_THROW( sBufSize >= aCipherTextLen, RAMP_OVER_FLOW );

    stlMemcpy( aPlainText, aCipherText, aCipherTextLen );

    for( sIdx = 0; sIdx < aCipherTextLen; sIdx += 8 )
    {
        STL_TRY( ztcmDecrypt8byte( (stlUInt32 *)(aPlainText + sIdx), (stlUInt32 *)aKey, aErrorStack)
                 == STL_SUCCESS );
    }

    sPadLen = aPlainText[aCipherTextLen - 1];

    STL_TRY_THROW( sPadLen <= 8, RAMP_ERROR_DECRYPT );

    for( sIdx = 2; sIdx <= sPadLen; sIdx++ )
    {
        STL_TRY_THROW( aPlainText[aCipherTextLen - sIdx] == sPadLen, RAMP_ERROR_DECRYPT );
    }

    *aPlainTextLen = aCipherTextLen - sPadLen;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_DECRYPT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_ENCRYPTED_PASSWD,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_OVER_FLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_VALUE_TOO_LONG,
                      NULL,
                      aErrorStack,
                      "PASSWD");
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcmEncrypt8byte( stlUInt32       * aValue,
                            stlUInt32       * aKey,
                            stlErrorStack   * aErrorStack )
{
    stlUInt32 sValue0 = aValue[0];
    stlUInt32 sValue1 = aValue[1];
    stlUInt32 sSum = 0;
    stlUInt32 i;           /* set up */
    stlUInt32 sDelta = 0x9e3779b9;                     /* a key schedule constant */
    stlUInt32 sKey0 = aKey[0];
    stlUInt32 sKey1 = aKey[1];
    stlUInt32 sKey2 = aKey[2];
    stlUInt32 sKey3 = aKey[3];   /* cache key */

    for( i = 0; i < 32; i++ )
    {                       /* basic cycle start */
        sSum += sDelta;
        sValue0 += ((sValue1<<4) + sKey0) ^ (sValue1 + sSum) ^ ((sValue1>>5) + sKey1);
        sValue1 += ((sValue0<<4) + sKey2) ^ (sValue0 + sSum) ^ ((sValue0>>5) + sKey3);
    }                                              /* end cycle */

    aValue[0] = sValue0;
    aValue[1] = sValue1;

    return STL_SUCCESS;
}

stlStatus ztcmDecrypt8byte( stlUInt32       * aValue,
                            stlUInt32       * aKey,
                            stlErrorStack   * aErrorStack )
{
    stlUInt32 sValue0 = aValue[0];
    stlUInt32 sValue1 = aValue[1];
    stlUInt32 sSum = 0xC6EF3720;
    stlUInt32 i;  /* set up */
    stlUInt32 sDelta = 0x9e3779b9;                     /* a key schedule constant */
    stlUInt32 sKey0 = aKey[0];
    stlUInt32 sKey1 = aKey[1];
    stlUInt32 sKey2 = aKey[2];
    stlUInt32 sKey3 = aKey[3];   /* cache key */

    for( i = 0; i < 32; i++ )
    {                         /* basic cycle start */
        sValue1 -= ((sValue0<<4) + sKey2) ^ (sValue0 + sSum) ^ ((sValue0>>5) + sKey3);
        sValue0 -= ((sValue1<<4) + sKey0) ^ (sValue1 + sSum) ^ ((sValue1>>5) + sKey1);
        sSum -= sDelta;
    }                                              /* end cycle */

    aValue[0] = sValue0;
    aValue[1] = sValue1;

    return STL_SUCCESS;
}



stlStatus ztcmBase64Encode( stlChar       * aInput,
                            stlSize         aInputLen,
                            stlChar       * aBuf,
                            stlSize         aBufSize,
                            stlErrorStack * aErrorStack )
{
    stlUInt8    sInput[3]  = { 0, 0, 0 };
    stlUInt8    sOutput[4] = { 0, 0, 0, 0 };
    stlInt32    sIndex;
    stlInt32    i;
    stlInt32    j;
    stlInt32    sSize;
    stlChar   * sPos;
    stlChar   * sPosLen;

    sPosLen    = aInput + aInputLen - 1;
    sSize      = (4 * (aInputLen / 3)) + (aInputLen % 3? 4 : 0) + 1;
    j          = 0;

    STL_TRY_THROW( aBufSize >= sSize, RAMP_OVER_FLOW );

    for( i = 0, sPos = aInput; sPos <= sPosLen; i++, sPos++ )
    {
        sIndex = i % 3;
        sInput[sIndex] = *sPos;

        if( (sIndex == 2) || (sPos == sPosLen) )
        {
            sOutput[0] = ((sInput[0] & 0xFC) >> 2);
            sOutput[1] = ((sInput[0] & 0x3) << 4) | ((sInput[1] & 0xF0) >> 4);
            sOutput[2] = ((sInput[1] & 0xF) << 2) | ((sInput[2] & 0xC0) >> 6);
            sOutput[3] = (sInput[2] & 0x3F);

            aBuf[j++] = gZtcmBase64[sOutput[0]];
            aBuf[j++] = gZtcmBase64[sOutput[1]];
            aBuf[j++] = (sIndex == 0)? '=' : gZtcmBase64[sOutput[2]];
            aBuf[j++] = (sIndex <  2)? '=' : gZtcmBase64[sOutput[3]];

            sInput[0] = 0;
            sInput[1] = 0;
            sInput[2] = 0;
        }
    }

    aBuf[j] = '\0';

    return STL_SUCCESS;

    STL_CATCH( RAMP_OVER_FLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_VALUE_TOO_LONG,
                      NULL,
                      aErrorStack,
                      "BASE64 Encode");
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmBase64Decode( stlChar         * aInput,
                            stlUInt8        * aBuf,
                            stlSize         * aBufSize,
                            stlErrorStack   * aErrorStack )
{
    const stlChar * sPos;
    stlInt32        sSpaceIdx = 0;
    stlInt32        sPhase;
    stlInt32        sDec;
    stlInt32        sPrevDec = 0;
    stlUInt8        sCh;
    stlSize         sBufSize = *aBufSize;

    sSpaceIdx = 0;
    sPhase = 0;

    for( sPos = aInput; *sPos != '\0'; ++sPos )
    {
        sDec = gZtcmDecodeBase64[(stlInt32) *sPos];
        if( sDec != -1 )
        {
            switch( sPhase )
            {
                case 0:
                    ++sPhase;
                    break;
                case 1:
                    sCh = ( ( sPrevDec << 2 ) | ( ( sDec & 0x30 ) >> 4 ) );

                    STL_TRY_THROW( sSpaceIdx < sBufSize, RAMP_OVER_FLOW );

                    aBuf[sSpaceIdx++] = sCh;

                    ++sPhase;
                    break;
                case 2:
                    sCh = ( ( ( sPrevDec & 0xf ) << 4 ) | ( ( sDec & 0x3c ) >> 2 ) );

                    STL_TRY_THROW( sSpaceIdx < sBufSize, RAMP_OVER_FLOW );

                    aBuf[sSpaceIdx++] = sCh;

                    ++sPhase;
                    break;
                case 3:
                    sCh = ( ( ( sPrevDec & 0x03 ) << 6 ) | sDec );

                    STL_TRY_THROW( sSpaceIdx < sBufSize, RAMP_OVER_FLOW );

                    aBuf[sSpaceIdx++] = sCh;

                    sPhase = 0;
                    break;
            }
            sPrevDec = sDec;
        }
    }

    *aBufSize = sSpaceIdx;

    return STL_SUCCESS;

    STL_CATCH( RAMP_OVER_FLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_VALUE_TOO_LONG,
                      NULL,
                      aErrorStack,
                      "BASE64 Decode");
    }
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
