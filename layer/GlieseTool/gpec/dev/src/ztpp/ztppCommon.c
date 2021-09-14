/*******************************************************************************
 * ztppCommon.c
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

#include <stl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztpDef.h>
#include <ztppCommon.h>
#include <ztppMacro.h>
#include <ztppExpandFile.h>
#include <ztpfPathControl.h>

/**
 * @file ztppCommon.c
 * @brief C Preprocessor Parser Routines
 */

/**
 * @addtogroup ztppCommon
 * @{
 */

stlStatus ztppFindMacro( stlChar          *aMacroName,
                         ztpMacroSymbol  **aMacroSymbol,
                         stlErrorStack    *aErrorStack )
{
    STL_TRY( ztppLookupMacroSymbol( aErrorStack,
                                    aMacroName,
                                    aMacroSymbol )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztppConvertLiteral( stlChar          *aLiteral,
                              stlInt64         *aResult,
                              stlErrorStack    *aErrorStack )
{
    stlChar      *sLiteral;
    stlChar      *sEndPtr;
    stlInt64      sLen;
    stlInt64      sResult;

    *aResult = 0;
    sLiteral = aLiteral;

    while( (*sLiteral == 'u')
           || (*sLiteral == 'U')
           || (*sLiteral == 'L') )
    {
        sLiteral ++;
    }

    sLen = stlStrlen( sLiteral );

    if( sLiteral[0] == '\'' )
    {
        /*
         * Character constant
         */
        if( sLiteral[1] == '\\' )
        {
            switch( sLiteral[2] )
            {
                case '0':
                    sResult = 0;
                    break;
                case 'a':
                    sResult = 7;
                    break;
                case 'b':
                    sResult = 8;
                    break;
                case 'f':
                    sResult = 12;
                    break;
                case 'n':
                    sResult = 10;
                    break;
                case 'r':
                    sResult = 13;
                    break;
                case 't':
                    sResult = 9;
                    break;
                case 'v':
                    sResult = 11;
                    break;
                default:
                    sResult = sLiteral[2];
                    break;
            }

            STL_TRY_THROW( sLiteral[3] == '\'', RAMP_ERR_MULTI_CHARACTER );
        }
        else
        {
            sResult = sLiteral[1];
            STL_TRY_THROW( sLiteral[2] == '\'', RAMP_ERR_MULTI_CHARACTER );
        }
    }
    else if( sLiteral[0] == '"' )
    {
        /*
         * String literal
         */
        STL_THROW( RAMP_ERR_STRING_LITERAL );
    }
    else
    {
        /*
         * MUST BE: Numeric literal
         */
        STL_TRY( stlStrToInt64( sLiteral,
                                sLen,
                                &sEndPtr,
                                0,
                                &sResult,
                                aErrorStack )
                 == STL_SUCCESS );

        if( sEndPtr != (sLiteral + sLen))
        {
            STL_TRY_THROW( *sEndPtr != '.', RAMP_ERR_FLOATING_CONTANT );
            STL_TRY_THROW( (*sEndPtr == 'l')
                           || (*sEndPtr == 'L')
                           || (*sEndPtr == 'u')
                           || (*sEndPtr == 'U'),
                           RAMP_ERR_INVALID_SUFFIX );
        }
    }

    *aResult = sResult;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MULTI_CHARACTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_MULTI_CHARACTER_CHARACTER_CONSTANT,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_STRING_LITERAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_INVALID_PP_TOKEN,
                      NULL,
                      aErrorStack,
                      aLiteral );
    }

    STL_CATCH( RAMP_ERR_FLOATING_CONTANT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_FLOATING_CONSTANT_IN_PP,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_SUFFIX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_INVALID_SUFFIX_ON_INTEGER_CONSTANT,
                      NULL,
                      aErrorStack,
                      sEndPtr );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztppReplaceMacro( stlChar             *aReplacedBuffer,
                            stlChar             *aMacroContents,
                            ztpArgNameList      *aArgNameList,
                            ztpPPParameterList  *aParameter,
                            stlInt32             aParameterCount,
                            stlErrorStack       *aErrorStack )
{
    stlChar             *sTempBuffer = NULL;
    stlChar             *sSrc;
    stlChar             *sDest;
    stlChar              sSearchPattern[100];
    stlChar              sTargetPattern[100];
    stlInt32             i;
    stlBool              sIsReplace = STL_FALSE;
    ztpPPParameterList  *sParameter;

    if( aMacroContents != NULL )
    {
        STL_TRY( stlAllocHeap( (void **)&sTempBuffer,
                               ZTP_MACRO_REPLACE_BUFFER_SIZE,
                               aErrorStack )
                 == STL_SUCCESS );

        sSrc = sTempBuffer;

        stlStrncpy( sSrc, aMacroContents, ZTP_MACRO_REPLACE_BUFFER_SIZE );
        sDest = aReplacedBuffer;

        if( aParameterCount != 0 )
        {
            sParameter = aParameter;
            for( i = 1; i <= aParameterCount; i ++ )
            {
                stlSnprintf( sSearchPattern, 100, "___GPEC_MACRO_ARGS_%d___", i );
                stlSnprintf( sTargetPattern, 100, "%ld", sParameter->mValue );

                do
                {
                    STL_TRY( ztppReplaceText( sSrc, sSearchPattern,
                                              sDest, sTargetPattern,
                                              &sIsReplace )
                             == STL_SUCCESS );
                    stlStrncpy( sSrc, sDest, ZTP_MACRO_REPLACE_BUFFER_SIZE );
                } while( sIsReplace == STL_TRUE );

                sParameter = sParameter->mNext;
            }
        }
        else
        {
            stlStrcpy( sDest, aMacroContents );
        }

        stlFreeHeap( sTempBuffer );
    }
    else
    {
        /*
         * Macro contents is empty.
         *  Do nothing
         */
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sTempBuffer != NULL )
    {
        stlFreeHeap( sTempBuffer );
    }

    return STL_FAILURE;
}

stlStatus ztppReplaceText( stlChar        *aSrcText,
                           stlChar        *aSrcPattern,
                           stlChar        *aDestText,
                           stlChar        *aDestPattern,
                           stlBool        *aIsReplace )
{
    stlChar    *sReplaceBegin;
    stlInt64    sSrcPatternLen;

    sSrcPatternLen = stlStrlen( aSrcPattern );
    sReplaceBegin = stlStrstr( aSrcText, aSrcPattern );

    if( sReplaceBegin != NULL )
    {
        stlStrncpy( aDestText, aSrcText, sReplaceBegin - aSrcText + 1 );
        stlStrcat( aDestText, aDestPattern );
        stlStrcat( aDestText, sReplaceBegin + sSrcPatternLen );

        *aIsReplace = STL_TRUE;
    }
    else
    {
        *aIsReplace = STL_FALSE;
    }

    return STL_SUCCESS;
}

stlChar *ztppScanToken( stlChar            **aGivenString,
                        ztpPPMacroTokenId   *aTokenId )
{
    stlChar  *sCurLoc  = *aGivenString;
    stlChar  *sPrevLoc = *aGivenString;
    stlBool   sReady = STL_FALSE;

    if( *sCurLoc == '\0' )
    {
        *aTokenId = ZTP_MACRO_TOKEN_NULL;
    }
    else if( stlIsalpha( *sCurLoc ) != 0 )
    {
        while( stlIsalnum( *sCurLoc ) != 0 )
        {
            sCurLoc ++;
        }

        *aTokenId = ZTP_MACRO_TOKEN_ID;
    }
    else
    {
        while( *sCurLoc != '\0' )
        {
            /*
             * 현재 나오는 Character가 alphabet일 경우
             * ID로 해석이 가능한지에 대한 Check를 한다.
             *  sReady == STL_TRUE
             *    : ID 시작 가능
             *  sReady == STL_FALSE
             *    : alphbet이 나와도 ID로 해석되지 않고 이전
             *      문자열에 이어지는 stream으로 해석
             *
             *
             *  ex)
             *   (abc)
             *    ^ -- ID 시작; sReady == STL_TRUE
             *   (0b1)
             *     ^  -- 여기서 b는 ID가 될 수 없음; sReady == STL_FALSE
             *   ($a)
             *     ^  -- $ 뒤의 alphabet도 ID가 될 수 없음
             */
            if( sCurLoc == sPrevLoc )
            {
                /*
                 * 주어진 string의 처음이므로, alphabet이 나온다면
                 * ID 가 됨
                 */
                sReady = STL_TRUE;
            }
            else if( ( stlIsdigit( *(sCurLoc - 1) ) != 0 )
                || ( *(sCurLoc - 1) == '$' ) )
            {
                /*
                 * 이전 문자가 숫자 이거나 '$' 라면
                 * alphabet 이 나와도 ID가 되지 않고 이어진 stream이 됨
                 */
                sReady = STL_FALSE;
            }
            else
            {
                sReady = STL_TRUE;
            }

            if( stlIsalpha( *sCurLoc ) != 0 )
            {
                if( sReady == STL_TRUE )
                {
                    break;
                }
            }

            sCurLoc ++;
        }

        *aTokenId = ZTP_MACRO_TOKEN_ETC;
    }

    *aGivenString = sCurLoc;

    return sPrevLoc;
}

/** @} */

