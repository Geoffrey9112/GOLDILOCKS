/*******************************************************************************
 * ztlmConf.c
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
 * @file ztlmConf.c
 * @brief Gliese Listener Main Routines
 */

#include <stl.h>
#include <ztlDef.h>
#include <ztlc.h>
#include <ztlm.h>

extern stlLogger gZtlLogger;

ztlmTcpFilter    gZtlmTcpFilter;
stlChar          gZtlmDataDir[STL_MAX_FILE_PATH_LENGTH];

stlIniFormat gZtlmIniFormat[] =
{
    {
            "LISTENER",                     /* ini object name */
            "TCP_HOST",                     /* ini property name */
            STL_INI_DATA_TYPE_STRING,       /* data type */
            STL_INADDR_ANY_STR,             /* default value */
            7,                              /* min value (STL_INADDR_ANY_STR 인경우 min length) */
            15,                             /* max value (STL_INADDR_ANY_STR 인경우 max length) */
            gZtlConfig.mTcpHost,            /* ini에서 값 읽어서 저장할 변수 */
            STL_SIZEOF(gZtlConfig.mTcpHost) /* 변수 size */
    },
    {
            "LISTENER",
            "LISTEN_PORT",
            STL_INI_DATA_TYPE_INT64,
            "22581",
            1024,   /* IANA 권고 1024 ~ 49151 */
            49151,
            &gZtlConfig.mTcpPort,
            STL_SIZEOF(gZtlConfig.mTcpPort)
    },
    {
            "LISTENER",
            "BACKLOG",
            STL_INI_DATA_TYPE_INT64,
            "1024",
            1,                              /* min value (STL_INADDR_ANY_STR 인경우 min length) */
            32768,                          /* max value (STL_INADDR_ANY_STR 인경우 max length) */
            &gZtlConfig.mBacklog,
            STL_SIZEOF(gZtlConfig.mTcpPort)
    },
    {
            "LISTENER",
            "DEFAULT_CS_MODE",
            STL_INI_DATA_TYPE_STRING,
            "dedicated",
            6,
            9,
            &gZtlConfig.mDefaultCsMode,
            STL_SIZEOF(gZtlConfig.mDefaultCsMode)
    },
    {
            "LISTENER",
            "TCP_VALIDNODE_CHECKING",
            STL_INI_DATA_TYPE_STRING,
            "NO",
            2,
            8,
            &gZtlConfig.mTcpValidNodeChecking,
            STL_SIZEOF(gZtlConfig.mTcpValidNodeChecking)
    },
    {
            "LISTENER",
            "TCP_INVITED_FILE",
            STL_INI_DATA_TYPE_STRING,
            "goldilocks.invited.conf",
            1,
            STL_MAX_INI_PROPERTY_VALUE,
            &gZtlConfig.mTcpInvitedFile,
            STL_SIZEOF(gZtlConfig.mTcpInvitedFile)
    },
    {
            "LISTENER",
            "TCP_EXCLUDED_FILE",
            STL_INI_DATA_TYPE_STRING,
            "goldilocks.excluded.conf",
            1,
            STL_MAX_INI_PROPERTY_VALUE,
            &gZtlConfig.mTcpExcludedFile,
            STL_SIZEOF(gZtlConfig.mTcpExcludedFile)
    },
    {
            "LISTENER",
            "TIMEOUT",
            STL_INI_DATA_TYPE_INT64,
            "100",
            0,
            2147483647,     /* gZtlConfig.mTimeout변수가 32bit임으로 32bit 최대값 */
            &gZtlConfig.mTimeout,
            STL_SIZEOF(gZtlConfig.mTimeout)
    },
    {
            "LISTENER",
            "LISTENER_LOG_DIR",
            STL_INI_DATA_TYPE_STRING,
            "<GOLDILOCKS_DATA>/trc",
            1,
            STL_SIZEOF(gZtlConfig.mLogDir),
            &gZtlConfig.mLogDir,
            STL_SIZEOF(gZtlConfig.mLogDir)
    },
    {
            NULL,
            NULL,
            0,
            NULL,
            0,
            0,
            NULL,
            0
    },
};



/**
 * @brief network filter를 설정한다.
 * filter를 사용하는 경우만 이 함수가 호출된다.
 * mTcpInvitedFile, mTcpExcludedFile가 둘다 설정되어 있으면 mTcpInvitedFile를 적용한다
 */
stlStatus ztlmSetNetworkFilter( stlErrorStack * aErrorStack )
{
    stlInt32         sState = 0;
    stlFile          sFile;
    stlChar          sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlBool          sFileExist;
    stlChar          sLine[STL_MAX_INI_PROPERTY_VALUE + 1];
    stlInt32         sLineCount = 0;
    stlInt32         sIdx = 0;
    stlOffset        sOffset = 0;

    /* check INVITED file  */
    if( gZtlConfig.mTcpValidNodeChecking == ZTLM_TCP_FILTER_INVITED )
    {
        stlSnprintf( sFileName,
                     STL_MAX_FILE_PATH_LENGTH,
                     "%s%s%s%s%s",
                     gZtlmDataDir,
                     STL_PATH_SEPARATOR,
                     "conf",
                     STL_PATH_SEPARATOR,
                     gZtlConfig.mTcpInvitedFile );

        STL_TRY( stlExistFile( sFileName,
                               &sFileExist,
                               aErrorStack )
                 == STL_SUCCESS );

        if( sFileExist == STL_TRUE )
        {
            (void)stlLogMsg( &gZtlLogger,
                             aErrorStack,
                             "[LISTENER] client filtering setting : [INVITED] \n" );
        }
        else
        {
            (void)stlLogMsg( &gZtlLogger,
                             aErrorStack,
                             "[LISTENER] client filtering file not found %s \n", sFileName );

            gZtlConfig.mTcpValidNodeChecking = ZTLM_TCP_FILTER_NONE;
        }
    }
    /* check INVITED file  */
    else if( gZtlConfig.mTcpValidNodeChecking == ZTLM_TCP_FILTER_EXCLUDED )
    {
        stlSnprintf( sFileName,
                     STL_MAX_FILE_PATH_LENGTH,
                     "%s%s%s%s%s",
                     gZtlmDataDir,
                     STL_PATH_SEPARATOR,
                     "conf",
                     STL_PATH_SEPARATOR,
                     gZtlConfig.mTcpExcludedFile );

        STL_TRY( stlExistFile( sFileName,
                               &sFileExist,
                               aErrorStack )
                 == STL_SUCCESS );

        if( sFileExist == STL_TRUE )
        {
            (void)stlLogMsg( &gZtlLogger,
                             aErrorStack,
                             "[LISTENER] client filtering setting : [EXCLUDED] \n" );
        }
        else
        {
            (void)stlLogMsg( &gZtlLogger,
                             aErrorStack,
                             "[LISTENER] client filtering file not found %s \n", sFileName );

            gZtlConfig.mTcpValidNodeChecking = ZTLM_TCP_FILTER_NONE;
        }
    }

    /* mTcpInvitedFile, mTcpExcludedFile 둘다 없으면 그냥 리턴한다 */
    if( gZtlConfig.mTcpValidNodeChecking == ZTLM_TCP_FILTER_NONE )
    {
        STL_THROW( RAMP_SUCCESS );
    }

    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;

    /* 일단 line이 몇개인지 확인한다 */
    while( STL_TRUE )
    {
        if( stlGetStringFile( sLine,
                              STL_MAX_INI_LINE + 1,
                              &sFile,
                              aErrorStack ) != STL_SUCCESS )
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
            (void)stlPopError( aErrorStack );

            break;
        }

        stlTrimString( sLine );

        if( sLine[0] == '#' )
        {
            continue;
        }

        if( sLine[0] == '\0' )
        {
            continue;
        }

        if( stlStrlen( sLine ) > 15 )
        {
            continue;
        }

        sLineCount++;
    }

    gZtlmTcpFilter.mIpCount = sLineCount;

    /* 16byte(ZTLM_MAX_IPADDRESS) * line 메모리 할당한다 */
    STL_TRY( stlAllocHeap( (void **)&gZtlmTcpFilter.mIpAddress,
                           STL_SIZEOF(gZtlmTcpFilter.mIpAddress) * sLineCount,
                           aErrorStack )
             == STL_SUCCESS );
    sState = 2;

    /* seek로 file의 처음으로 다시 간다 */
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack )
             == STL_SUCCESS );

    sIdx = 0;

    /* 이번엔 파일을 읽으면서 gZtlmTcpFilter.mIpAddress에 저장한다 */
    while( STL_TRUE )
    {
        if( stlGetStringFile( sLine,
                              STL_MAX_INI_LINE + 1,
                              &sFile,
                              aErrorStack ) != STL_SUCCESS )
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
            (void)stlPopError( aErrorStack );

            break;
        }

        stlTrimString( sLine );

        if( sLine[0] == '#' )
        {
            continue;
        }

        if( sLine[0] == '\0' )
        {
            continue;
        }

        if( stlStrlen( sLine ) > 15 )
        {
            continue;
        }

        if( gZtlConfig.mTcpValidNodeChecking == ZTLM_TCP_FILTER_INVITED )
        {
            (void)stlLogMsg( &gZtlLogger,
                             aErrorStack,
                             "[LISTENER] invited ipaddress : %s\n",
                             sLine );
        }
        else if( gZtlConfig.mTcpValidNodeChecking == ZTLM_TCP_FILTER_EXCLUDED )
        {
            (void)stlLogMsg( &gZtlLogger,
                             aErrorStack,
                             "[LISTENER] excluded ipaddress : %s\n",
                             sLine );
        }

        /* 현재 ipv4만 가능함 */
        STL_TRY( stlGetBinAddrFromStr( AF_INET,
                                       sLine,
                                       &gZtlmTcpFilter.mIpAddress[sIdx],
                                       aErrorStack )
                 == STL_SUCCESS );

        sIdx++;
    }


    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );



    STL_RAMP( RAMP_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)stlFreeHeap( gZtlmTcpFilter.mIpAddress );
        case 1:
            (void)stlCloseFile( &sFile,
                                aErrorStack );
            break;

    }

    return STL_FAILURE;
}

/**
 * @brief client ip address가 접속 가능한지 확인한다.
 * TODO (lym): filter에 wildcard 적용
 */
stlStatus ztlmCheckClientIpAddress( ztlcContext   * aContext,
                                    stlBool       * aIsAccess,
                                    stlErrorStack * aErrorStack )
{
    stlInt32    sIdx;
    stlInt32    sFound = STL_FALSE;
    stlChar     sAddrBuf[STL_MAX_FILE_NAME_LENGTH];
    stlChar     sFilePathBuf[STL_MAX_FILE_PATH_LENGTH];
    stlInt32    sBinAddr;
    stlSockAddr sClientAddr;

    *aIsAccess = STL_FALSE;

    STL_TRY( stlGetPeerName( STL_SOCKET_IN_CONTEXT(aContext->mStlContext),
                             &sClientAddr,
                             aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetSockAddr( &sClientAddr,
                             sAddrBuf,
                             STL_MAX_FILE_NAME_LENGTH,
                             NULL,
                             sFilePathBuf,
                             STL_MAX_FILE_PATH_LENGTH,
                             aErrorStack )
             == STL_SUCCESS );

    /* 현재 ipv4만 가능함 */
    STL_TRY( stlGetBinAddrFromStr( AF_INET,
                                   sAddrBuf,
                                   (void *)&sBinAddr,
                                   aErrorStack )
             == STL_SUCCESS );

    if( gZtlConfig.mTcpValidNodeChecking == ZTLM_TCP_FILTER_NONE )
    {
        *aIsAccess = STL_TRUE;
        STL_THROW( RAMP_SUCCESS );
    }

    for( sIdx = 0; sIdx < gZtlmTcpFilter.mIpCount; sIdx++ )
    {
        if( gZtlmTcpFilter.mIpAddress[sIdx] == sBinAddr )
        {
            sFound = STL_TRUE;
            break;
        }
    }

    if( gZtlConfig.mTcpValidNodeChecking == ZTLM_TCP_FILTER_INVITED )
    {
        /* sFound가 STL_TRUE면 접속 가능 */
        if( sFound == STL_TRUE )
        {
            *aIsAccess = STL_TRUE;
        }
        else
        {
            STL_TRY( stlLogMsg( &gZtlLogger,
                                aErrorStack,
                                "[TCP FILTER INVITED] access denied : client ip address [%s] \n",
                                sAddrBuf )
                     == STL_SUCCESS );

            *aIsAccess = STL_FALSE;
        }
    }
    else
    {
        /* 여기는 sFound가 STL_TRUE면 접속 불가임 */
        if( sFound == STL_TRUE )
        {
            STL_TRY( stlLogMsg( &gZtlLogger,
                                aErrorStack,
                                "[TCP FILTER EXCLUDED] access denied : client ip address [%s] \n",
                                sAddrBuf )
                     == STL_SUCCESS );

            *aIsAccess = STL_FALSE;
        }
        else
        {
            *aIsAccess = STL_TRUE;
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Gliese listener의 configration file path를 설정한다.
 * @remark
 */
stlStatus ztlmGetConfFilePath( stlChar       * aPath,
                               stlSize         aSize,
                               stlErrorStack * aErrorStack )
{
    stlSnprintf( aPath,
                 aSize,
                 "%s%s%s",
                 gZtlmDataDir,
                 STL_PATH_SEPARATOR,
                 ZTLM_LISTENER_CONF_FILE );

    return STL_SUCCESS;
}

stlStatus ztlmGetValueFromEnv( stlChar       * aName,
                               stlChar       * aValue,
                               stlSize         aValueLen,
                               stlBool       * aIsFound,
                               stlErrorStack * aErrorStack )
{
    stlChar     sEnvName[STL_MAX_INI_PROPERTY_VALUE];

    stlSnprintf( sEnvName, STL_MAX_INI_PROPERTY_VALUE,
                 "GOLDILOCKS_%s", aName );

    STL_TRY( stlGetEnv( aValue,
                        aValueLen,
                        sEnvName,
                        aIsFound,
                        aErrorStack )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztlmGetConf( stlErrorStack * aErrorStack )
{
    stlBool          sFileExist = STL_FALSE;

    /* gZtlConfFilePath에 config 파일 path를 저장한다 */
    STL_TRY( ztlmGetConfFilePath( gZtlConfFilePath,
                                  STL_SIZEOF(gZtlConfFilePath),
                                  aErrorStack )
             == STL_SUCCESS );

    /* 파일 존재 확인 */
    STL_TRY( stlExistFile( gZtlConfFilePath,
                           &sFileExist,
                           aErrorStack )
             == STL_SUCCESS );
    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERROR_FILE_NOT_EXIST );

    /* ini 포멧 config 파일을 parsing */
    STL_TRY( ztlmParseIni( gZtlConfFilePath,
                           gZtlmIniFormat,
                           aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTLE_ERRCODE_CONF_FILE_NOT_EXIST,
                      NULL,
                      aErrorStack,
                      gZtlConfFilePath );
    }
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief format의 value에 aValue값을 설정한다.
 * @param[in] aFormat           INI format
 * @param[in] aValue            설정할 value
 * @param[out] aErrorStack      에러 스택
 */
stlStatus ztlmSetValueIni( stlIniFormat  * aFormat,
                           stlChar       * aValue,
                           stlErrorStack * aErrorStack )
{
    stlInt64        sValueInt64;
    stlChar         sPropertyData[STL_MAX_INI_PROPERTY_NAME * 2];
    stlSize         sLength = 0;
    stlChar         sTempValue[STL_MAX_INI_PROPERTY_VALUE + 1];

    stlSnprintf( sPropertyData,
                 STL_SIZEOF(sPropertyData),
                 "%s = %s",
                 aFormat->mName,
                 aValue );

    switch( aFormat->mDataType )
    {
        case STL_INI_DATA_TYPE_INT64:
            STL_TRY( stlStrToInt64( aValue,
                                    stlStrlen( aValue ),
                                    NULL,
                                    10,
                                    &sValueInt64,
                                    aErrorStack)
                     == STL_SUCCESS );

            /* format의 min, max 체크 */
            STL_TRY_THROW( sValueInt64 >= aFormat->mMin,
                           RAMP_ERR_INT_TYPE );

            STL_TRY_THROW( sValueInt64 <= aFormat->mMax,
                           RAMP_ERR_INT_TYPE );

            *((stlInt64 *)aFormat->mValue) = sValueInt64;

            break;
        case STL_INI_DATA_TYPE_STRING:
            if( stlStrcmp(aFormat->mName, "DEFAULT_CS_MODE") == 0 )
            {
                /* mDefaultCsMode는 일반적이지 않은 type이어서 설정 */
                if( stlStrcasecmp(aValue, "shared") == 0 )
                {
                    gZtlConfig.mDefaultCsMode = CML_SESSION_SHARED;
                }
                else if( stlStrcasecmp(aValue, "dedicated") == 0 )
                {
                    gZtlConfig.mDefaultCsMode = CML_SESSION_DEDICATE;
                }
                else
                {
                    STL_THROW( RAMP_ERROR_DEFAULT_CS_MODE );
                }
            }
            else if( stlStrcmp(aFormat->mName, "TCP_VALIDNODE_CHECKING") == 0 )
            {
                /* mTcpValidNodeChecking는 일반적이지 않은 type이어서 설정 */
                if( stlStrcasecmp(aValue, "no") == 0 )
                {
                    gZtlConfig.mTcpValidNodeChecking = ZTLM_TCP_FILTER_NONE;
                }
                else if( stlStrcasecmp(aValue, "invited") == 0 )
                {
                    gZtlConfig.mTcpValidNodeChecking = ZTLM_TCP_FILTER_INVITED;
                }
                else if( stlStrcasecmp(aValue, "excluded") == 0 )
                {
                    gZtlConfig.mTcpValidNodeChecking = ZTLM_TCP_FILTER_EXCLUDED;
                }
                else
                {
                    STL_THROW( RAMP_ERROR_TCP_VALIDNODE_CHECKING );
                }
            }
            else
            {
                STL_DASSERT( stlStrlen( aValue ) < aFormat->mValueSize );

                /* format의 min, max 체크 (string은 string길이의 min, max 이다) */
                STL_TRY_THROW( stlStrlen( aValue ) >= aFormat->mMin,
                               RAMP_ERR_STRING_TYPE );

                STL_TRY_THROW( stlStrlen( aValue ) <= aFormat->mMax,
                               RAMP_ERR_STRING_TYPE );

                /** Single Quotation 제거 */
                if( stlStrncmp( aValue, "'", 1 ) == 0 )
                {
                    /** 마지막 문자가 Single Quotation인지 검증 */
                    sLength = stlStrlen(aValue);
                    STL_TRY_THROW( stlStrncmp( aValue + sLength - 1, "'", 1 ) == 0,
                                   RAMP_ERR_STRING_TYPE );

                    /** Single Quotation 제거 하고 복사
                     * 실제로는 copy되는 length가 'sLength - 2' 지만 stlStrncpy에서 \0까지
                     * 복사함으로 'sLength - 1'로 설정한다.
                     */
                    stlStrncpy( aFormat->mValue, aValue + 1, sLength - 1 );
                }
                else
                {
                    stlStrcpy( aFormat->mValue, aValue );
                }

                /**
                 * <GOLDILOCKS_DATA> 변환
                 */
                if( stlStrncmp( aFormat->mValue, ZTLM_DB_HOME_DIR, stlStrlen(ZTLM_DB_HOME_DIR) ) == 0 )
                {
                    stlStrcpy( sTempValue, aFormat->mValue );

                    STL_TRY_THROW( aFormat->mMax > stlStrlen(gZtlmDataDir) + stlStrlen(sTempValue + stlStrlen(ZTLM_DB_HOME_DIR)),
                                   RAMP_ERROR_BUFFER_OVERFLOW );

                    stlSnprintf( aFormat->mValue, aFormat->mMax,
                                 "%s%s",
                                 gZtlmDataDir,
                                 sTempValue + stlStrlen(ZTLM_DB_HOME_DIR) );
                }
            }
            break;
        case STL_INI_DATA_TYPE_BOOLEAN:

            /* boolean type은 YES|NO, 1|0 두가지 형식 모두 가능 */
            if( stlStrcasecmp( aValue, "YES" ) == 0 )
            {
                *((stlBool *)aFormat->mValue) = STL_TRUE;
            }
            else if( stlStrcasecmp( aValue, "1" ) == 0 )
            {
                *((stlBool *)aFormat->mValue) = STL_TRUE;
            }
            else if( stlStrcasecmp( aValue, "NO" ) == 0 )
            {
                *((stlBool *)aFormat->mValue) = STL_FALSE;
            }
            else if( stlStrcasecmp( aValue, "0" ) == 0 )
            {
                *((stlBool *)aFormat->mValue) = STL_FALSE;
            }
            else
            {
                STL_THROW( RAMP_ERR_BOOLEAN_TYPE );
            }
            break;
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_BUFFER_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTLE_ERRCODE_BUFFER_OVERFLOW,
                      NULL,
                      aErrorStack,
                      sPropertyData );
    }
    STL_CATCH( RAMP_ERR_INT_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTLE_ERRCODE_INVALID_PROPERTY_INT_TYPE,
                      NULL,
                      aErrorStack,
                      aFormat->mMin,
                      aFormat->mMax,
                      sPropertyData );
    }
    STL_CATCH( RAMP_ERROR_DEFAULT_CS_MODE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTLE_ERRCODE_INVALID_PROPERTY_DEFAULT_CS_MODE,
                      NULL,
                      aErrorStack,
                      sPropertyData );
    }
    STL_CATCH( RAMP_ERROR_TCP_VALIDNODE_CHECKING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTLE_ERRCODE_INVALID_PROPERTY_VALIDNODE_CHECKING,
                      NULL,
                      aErrorStack,
                      sPropertyData );
    }
    STL_CATCH( RAMP_ERR_STRING_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTLE_ERRCODE_INVALID_PROPERTY_STRING_TYPE,
                      NULL,
                      aErrorStack,
                      aFormat->mMin,
                      aFormat->mMax,
                      sPropertyData );
    }
    STL_CATCH( RAMP_ERR_BOOLEAN_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTLE_ERRCODE_INVALID_PROPERTY_BOOLEAN_TYPE,
                      NULL,
                      aErrorStack,
                      sPropertyData );
    }
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ini파일을 읽어서 aIniFormat 구조체에 값을 설정한다.
 * @param[in] aFileName         parsing할 ini 파일
 * @param[in] aIniFormat        ini파일의 format 정의 구조체
 * @param[out] aErrorStack      에러 스택
 */
stlStatus ztlmParseIni( stlChar       * aFileName,
                        stlIniFormat  * aIniFormat,
                        stlErrorStack * aErrorStack )
{
    stlInt32        sState = 0;
    stlIni          sIni;
    stlIniFormat  * sFormat;
    stlIniObject  * sObject;
    stlBool         sFoundObject;
    stlBool         sFoundProperty;
    stlBool         sFoundEnv;
    stlChar         sProperty[STL_MAX_INI_PROPERTY_VALUE + 1];

    sFormat = aIniFormat;

    STL_TRY( stlOpenIni( &sIni,
                         aFileName,
                         aErrorStack ) == STL_SUCCESS );
    sState = 1;

    /* format의 내용을 ini 파일에서 하나씩 꺼내서 값을 설정한다 */
    while( sFormat->mOjbect != NULL )
    {
        sFoundProperty = STL_FALSE;

        /* ini파일에서 Object를 찾는다 */
        sObject = NULL;
        
        STL_TRY( stlFindIniObject( &sIni,
                                   sFormat->mOjbect,
                                   &sFoundObject,
                                   &sObject,
                                   aErrorStack )
                 == STL_SUCCESS );
        if( sFoundObject == STL_TRUE )
        {
            /* ini파일에서 Property를 찾는다 */
            STL_TRY( stlFindIniProperty( sObject,
                                         sFormat->mName,
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );
        }

        if( sFoundObject == STL_FALSE || sFoundProperty == STL_FALSE )
        {
            /**
             * ini에서 object or Property를 못찾으면 env값으로 설정한다
             */

            STL_TRY( ztlmGetValueFromEnv( sFormat->mName,
                                          sProperty,
                                          STL_SIZEOF(sProperty),
                                          &sFoundEnv,
                                          aErrorStack )
                     == STL_SUCCESS );

            /**
             * env에서도 못찾으면 default값으로 설정한다.
             */
            if( sFoundEnv == STL_FALSE )
            {
                stlStrcpy( sProperty, sFormat->mDefaultValue );
            }
        }

        /* 설정된 값을 type에 맞게 변경한다. */
        STL_TRY( ztlmSetValueIni( sFormat,
                                  sProperty,
                                  aErrorStack )
                 == STL_SUCCESS );

        /* 다음 format으로 이동 */
        sFormat++;
    }

    sState = 0;
    STL_TRY( stlCloseIni( &sIni,
                          aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlCloseIni( &sIni, aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}



stlInt32 ztlmPrintf( const stlChar *aFormat, ... )
{
    stlInt32  sReturn = 0;
    va_list   sVarArgList;

    STL_TRY_THROW( gZtlSilentMode == STL_FALSE, RAMP_SKIP );

    va_start(sVarArgList, aFormat);
    sReturn = stlVprintf( aFormat, sVarArgList );
    va_end(sVarArgList);

    STL_RAMP( RAMP_SKIP );

    return sReturn;
}

