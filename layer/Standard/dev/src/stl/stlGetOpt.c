/*******************************************************************************
 * stiGetOpt.c
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

#include <stc.h>
#include <stlDef.h>
#include <stlError.h>
#include <stlStrings.h>

#define EMSG    ""

/**
 * @brief stlGetOption()의 파싱을 위해 인자를 초기화 한다.
 * @param[in,out] aGetOpt 명형행 인자 정보를 저장하기 위한 구조체
 * @param[in] aArgc 인자의 수
 * @param[in] aArgv 인자 배열의 포인터
 * @param[out] aErrorStack 에러 스택
 */
stlStatus stlInitGetOption(stlGetOpt             * aGetOpt,
                           stlInt32                aArgc,
                           const stlChar * const * aArgv,
                           stlErrorStack         * aErrorStack)
{
    aGetOpt->mArgc = aArgc;
    aGetOpt->mArgv = aArgv;
    aGetOpt->mIndex = 1;
    aGetOpt->mOptCh = 0;
    aGetOpt->mOptArg = EMSG;
    
    return STL_SUCCESS;
}

/**
 * @brief stlInitGetOption()으로 초기화된 프로그램 실행 명령행 인자를 파싱한다.
 * @param[in] aGetOpt 초기화된 명형행 인자 구조체
 * @param[in] aOptString 규칙에 맞는 옵션 문자들이 포함되어 있는 문자열을 나타낸다.
 *                       만일 이러한 문자뒤에 콜론이 있다면, 해당 옵션이 인자를 요구한다는 것을 의미한다.
 * @param[out] aOptCh 해당 옵션
 * @param[out] aOptArg 해당 옵션의 인자
 * @param[out] aErrorStack 에러 스택
 * @return 다음 옵션이 존재할 경우 STL_SUCCESS를 반환한다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_EOF]
 *     파싱할 옵션이 더 이상 존재하지 않음
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     잘못된 옵션 문자나 필요한 인자가 존재하지 않을 경우
 * @endcode
 */
stlStatus stlGetOption(stlGetOpt      * aGetOpt,
                       const stlChar  * aOptString,
                       stlChar        * aOptCh,
                       const stlChar ** aOptArg,
                       stlErrorStack  * aErrorStack)
{
    const stlChar *sOptionIdx;

    /* update scanning pointer */
    if( aGetOpt->mOptArg[0] == 0 )
    {
        STL_TRY_THROW( aGetOpt->mIndex < aGetOpt->mArgc, RAMP_ERR_EOF );

        aGetOpt->mOptArg = aGetOpt->mArgv[aGetOpt->mIndex];

        if( aGetOpt->mOptArg[0] != '-' )
        {
            aGetOpt->mOptArg = EMSG;
            STL_THROW( RAMP_ERR_EOF );
        }

        if( aGetOpt->mOptArg[1] != 0 )
        {
            aGetOpt->mOptArg++;

            /* found "--" */
            if( aGetOpt->mOptArg[0] == '-' )
            {
                aGetOpt->mIndex++;
                aGetOpt->mOptArg = EMSG;
                STL_THROW( RAMP_ERR_EOF );
            }
        }
    }

    aGetOpt->mOptCh = (stlInt32)(*(aGetOpt->mOptArg));
    aGetOpt->mOptArg++;

    sOptionIdx = strchr(aOptString, aGetOpt->mOptCh);

    /* option letter okay? */
    if( (aGetOpt->mOptCh == (stlInt32)':') ||
        (sOptionIdx == 0) )
    {
        /*
         * if the user didn't specify '-' as an option,
         * assume it means -1.
         */
        STL_TRY_THROW( aGetOpt->mOptCh != (stlInt32)'-', RAMP_ERR_EOF );

        if( aGetOpt->mOptArg[0] == 0 )
        {
            aGetOpt->mIndex++;
        }

        STL_THROW( RAMP_ERR_INVALID_ARGUMENT );        
    }

    sOptionIdx++;

    if( sOptionIdx[0] != ':')
    {
        /* don't need argument */

        *aOptArg = NULL;

        if( aGetOpt->mOptArg[0] == 0 )
        {
            aGetOpt->mIndex++;
        }
    }
    else
    {
        /* need an argument */

        /* no white space */
        if( aGetOpt->mOptArg[0] != 0 )
        {
            *aOptArg = aGetOpt->mOptArg;
        }
        else
        {
            aGetOpt->mIndex++;

            /* no arg */
            if( aGetOpt->mArgc <= aGetOpt->mIndex )
            {
                aGetOpt->mOptArg = EMSG;

                STL_THROW(RAMP_ERR_INVALID_ARGUMENT);
            }
            else
            {
                /* white space */
                *aOptArg = aGetOpt->mArgv[aGetOpt->mIndex];                
            }
        }

        aGetOpt->mOptArg = EMSG;
        aGetOpt->mIndex++;
    }

    *aOptCh = aGetOpt->mOptCh;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EOF )
    {
        *aOptCh = aGetOpt->mOptCh;

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_EOF,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT )
    {
        *aOptCh = aGetOpt->mOptCh;

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief stlInitGetOption()으로 초기화된 프로그램 실행 명령행 인자를 파싱한다.
 * @param[in] aGetOpt 초기화된 명형행 인자 구조체
 * @param[in] aOption 옵션이 정의된 Structure의 배열 포인터
 * @param[out] aOptCh 해당 옵션
 * @param[out] aOptArg 해당 옵션의 인자
 * @param[out] aErrorStack 에러 스택
 * @return 다음 옵션이 존재할 경우 STL_SUCCESS를 반환한다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_EOF]
 *     파싱할 옵션이 더 이상 존재하지 않음
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     잘못된 옵션 문자나 필요한 인자가 존재하지 않을 경우
 * @endcode
 */
stlStatus stlGetOptionLong(stlGetOpt        * aGetOpt,
                           stlGetOptOption  * aOption,
                           stlChar          * aOptCh,
                           const stlChar   ** aOptArg,
                           stlErrorStack    * aErrorStack)
{
    stlChar const * sStr;
    stlInt32        i;
    stlInt32        sNameLen = 0;

    /* update scanning pointer */
    if( aGetOpt->mOptArg[0] == 0 )
    {
        STL_TRY_THROW( aGetOpt->mIndex < aGetOpt->mArgc, RAMP_ERR_EOF );

        aGetOpt->mOptArg = aGetOpt->mArgv[aGetOpt->mIndex];

        if( aGetOpt->mOptArg[0] != '-' )
        {
            aGetOpt->mOptArg = EMSG;
            STL_THROW( RAMP_ERR_EOF );
        }
    }

    sStr = aGetOpt->mArgv[aGetOpt->mIndex] + 1;
    aGetOpt->mIndex++;

    if( (sStr[0] == '-') && ( sStr[1] != '\0') )
    {
        sStr++;

        for( i = 0; ; i++ )
        {
            if( aOption[i].mOptCh == 0 )
            {
                STL_THROW( RAMP_ERR_INVALID_ARGUMENT );
            }

            if( aOption[i].mName != NULL )
            {
                sNameLen = stlStrlen( aOption[i].mName );

                if( (stlStrncmp( sStr, aOption[i].mName, sNameLen ) == 0) &&
                    ( (sStr[sNameLen] == '\0') || (sStr[sNameLen] == '=') ) )
                {
                    break;
                }
            }
        }

        *aOptCh = aOption[i].mOptCh;

        if( aOption[i].mHasArg == STL_TRUE )
        {
            if( sStr[sNameLen] == '=' )
            {
                *aOptArg = sStr + sNameLen + 1;
            }
            else
            {
                STL_TRY_THROW( aGetOpt->mIndex < aGetOpt->mArgc,
                               RAMP_ERR_INVALID_ARGUMENT );

                *aOptArg = aGetOpt->mArgv[aGetOpt->mIndex];
                aGetOpt->mIndex++;
            }
        }
        else
        {
            *aOptArg = NULL;

            STL_TRY_THROW( sStr[sNameLen] != '=',
                           RAMP_ERR_INVALID_ARGUMENT );
        }

        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        STL_TRY_THROW( sStr[0] != '-', RAMP_ERR_INVALID_ARGUMENT );
        STL_TRY_THROW( sStr[0] != '\0', RAMP_ERR_INVALID_ARGUMENT );
    }

    for( i = 0; ; i++ )
    {
        if( aOption[i].mOptCh == 0 )
        {
            STL_THROW( RAMP_ERR_INVALID_ARGUMENT );
        }

        if( aOption[i].mOptCh == *sStr )
        {
            break;
        }
    }

    *aOptCh = *sStr;
    sStr++;

    if( aOption[i].mHasArg == STL_TRUE )
    {
        if( *sStr != '\0' )
        {
            *aOptArg = sStr;
        }
        else
        {
            STL_TRY_THROW( aGetOpt->mIndex < aGetOpt->mArgc,
                           RAMP_ERR_INVALID_ARGUMENT );

            *aOptArg = aGetOpt->mArgv[aGetOpt->mIndex];
            aGetOpt->mIndex++;
        }
    }
    else
    {
        if( *sStr != '\0' )
        {
            /**
             * [AS-IS]
             *  - Argument가 없어야 하는데 사용자가 지정했다면 무시한다.
             *  - "cyclone -stop" 하면 cyclone이 시작하는 문제가 발생한다.
             * [TO-BE]
             *  - Argument가 없어야 하는데 사용자가 지정했다면 에러를 발생시킨다.
             */
            STL_THROW( RAMP_ERR_INVALID_ARGUMENT );
        }
        
        *aOptArg = NULL;
    }
    
    STL_RAMP( RAMP_SUCCESS );

    aGetOpt->mOptArg = EMSG;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EOF )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_EOF,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}
