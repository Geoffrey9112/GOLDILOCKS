/*******************************************************************************
 * slp.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: slp.c 13685 2014-10-07 09:35:24Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file slp.c
 * @brief Gliese Server Library Protocol Routines
 */

#include <stl.h>
#include <cml.h>
#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <ell.h>
#include <qll.h>
#include <ssl.h>
#include <sll.h>
#include <slDef.h>
#include <slb.h>
#include <slp.h>

extern stlBool gSlbWarmedUp;

/**
 * version에 따라 여기의 함수가 다르게 동작해야 하는 경우
 * 함수별로 버전 관리를 하지 않고 각 함수 내부에서 분기 하여 처리한다.
 */


sllCommandFunc gSllCommandFunc[CML_COMMAND_MAX] =
{
    slpCmdNone,                /* 0   CML_COMMAND_NONE */
    slpCmdNone,                /* 1   CML_COMMAND_HANDSHAKE */
    slpCmdConnect,             /* 2   CML_COMMAND_CONNECT */
    slpCmdDisconnect,          /* 3   CML_COMMAND_DISCONNECT */
    slpCmdGetConnAttr,         /* 4   CML_COMMAND_GETCONNATTR */
    slpCmdSetConnAttr,         /* 5   CML_COMMAND_SETCONNATTR */
    slpCmdFreeStmt,            /* 6   CML_COMMAND_FREESTMT */
    slpCmdCancel,              /* 7   CML_COMMAND_CANCEL */
    slpCmdDescribeCol,         /* 8   CML_COMMAND_DESCRIBECOL */
    slpCmdTransaction,         /* 9   CML_COMMAND_TRANSACTION */
    slpCmdPrepare,             /* 10  CML_COMMAND_PREPARE */
    slpCmdExecDirect,          /* 11  CML_COMMAND_EXECDIRECT */
    slpCmdExecute,             /* 12  CML_COMMAND_EXECUTE */
    slpCmdGetOutBindData,      /* 13  CML_COMMAND_GETOUTBINDDATA */
    slpCmdFetch,               /* 14  CML_COMMAND_FETCH */
    slpCmdTargetType,          /* 15  CML_COMMAND_TARGETTYPE */
    slpCmdBindType,            /* 16  CML_COMMAND_BINDTYPE */
    slpCmdBindData,            /* 17  CML_COMMAND_BINDDATA */
    slpCmdGetStmtAttr,         /* 18  CML_COMMAND_GETSTMTATTR */
    slpCmdSetStmtAttr,         /* 19  CML_COMMAND_SETSTMTATTR */
    slpCmdNumParams,           /* 20  CML_COMMAND_NUMPARAMS */
    slpCmdNumResultCols,       /* 21  CML_COMMAND_NUMRESULTCOLS */
    slpCmdGetCursorName,       /* 22  CML_COMMAND_GETCURSORNAME */
    slpCmdSetCursorName,       /* 23  CML_COMMAND_SETCURSORNAME */
    slpCmdSetPos,              /* 24  CML_COMMAND_SETPOS */
    slpCmdCloseCursor,         /* 25  CML_COMMAND_CLOSECURSOR */
    slpCmdStartUp,             /* 26  CML_COMMAND_STARTUP */
    slpCmdXaClose,             /* 27  CML_COMMAND_XA_CLOSE */
    slpCmdXaCommit,            /* 28  CML_COMMAND_XA_COMMIT */
    slpCmdXaRollback,          /* 29  CML_COMMAND_XA_ROLLBACK */
    slpCmdXaPrepare,           /* 30  CML_COMMAND_XA_PREPARE */
    slpCmdXaStart,             /* 31  CML_COMMAND_XA_START */
    slpCmdXaEnd,               /* 32  CML_COMMAND_XA_END */
    slpCmdXaForget,            /* 33  CML_COMMAND_XA_FORGET */
    slpCmdXaRecover,           /* 34  CML_COMMAND_XA_RECOVER */
    slpCmdParameterType,       /* 35  CML_COMMAND_PARAMETERTYPE */
    slpCmdPeriod,              /* 36  CML_COMMAND_PERIOD */
    slpCmdControlPacket,       /* 37  CML_COMMAND_CONTROLPACKET */
    NULL                       /* 38  CML_COMMAND_PRE_HANDSHAKE */
};

stlStatus slpCmdNone( sllCommandArgs * aArgs,
                      sllEnv         * aEnv )
{
    sclHandle * sSclHandle = SCL_HANDLE(SLL_COMMAND_ARG1(aArgs));

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SLL_ERRCODE_INVALID_PROTOCOL,
                  "invalid protocol command",
                  KNL_ERROR_STACK(aEnv) );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_NONE,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdConnect( sllCommandArgs * aArgs,
                         sllEnv         * aEnv )
{
    stlChar         sUserName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1] = { '\0' };
    stlChar         sEncryptedPassword[STL_MAX_SQL_IDENTIFIER_LENGTH + 1] = { '\0' };
    stlChar         sOldPassword[STL_MAX_SQL_IDENTIFIER_LENGTH + 1] = { '\0' };
    stlChar         sNewPassword[STL_MAX_SQL_IDENTIFIER_LENGTH + 1] = { '\0' };
    stlChar         sProgramName[STL_PATH_MAX + 1] = { '\0' };
    stlInt32        sProcessId;
    stlInt16        sCharacterset;
    stlInt32        sTimezone;
    dtlCharacterSet sNlsCharacterset;
    dtlCharacterSet sNlsNCharCharacterset;
    stlInt32        sDBTimezone;
    cmlDbcType      sDbcType;
    stlInt8         sControl = CML_CONTROL_NONE;
    stlProc         sProc;
    stlChar         sAddress[STL_MAX_HOST_LEN + 1];
    stlPort         sPort;
    sllHandle     * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle     * sSclHandle = SCL_HANDLE(sSllHandle);

    SLL_INIT_PROTOCOLSET_STACK( SLL_PROTOCOL_STACK(sSllHandle) );

    STL_TRY( sclReadConnectCommand( sSclHandle,
                                    &sControl,
                                    sUserName,
                                    sEncryptedPassword,
                                    sOldPassword,
                                    sNewPassword,
                                    sProgramName,
                                    &sProcessId,
                                    &sCharacterset,
                                    &sTimezone,
                                    &sDbcType,
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    /**
     * connect이전에 env allocation시 에러가 발생하는 경우
     * tailing protocol을 처리하지 않고 에러가 클라이어트로 전송된다.
     * 개발자의 실수를 막기 위해서 Connect는 HEAD이자 TAIL로 제한한다.
     */
    STL_DASSERT( SCL_IS_TAIL(sControl) == STL_TRUE );

    /*
     * Warm-up이 되지 않았다면 FAIL
     */
    if( gSlbWarmedUp == STL_FALSE )
    {
        /**
         * 여기서 stlPushError를 하고 RAMP_ERROR_SEND로 간다.
         */
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SLL_ERRCODE_SERVER_IS_NOT_RUNNING,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );

        STL_THROW( RAMP_ERROR_SEND );
    }


    sProc.mProcID = sProcessId;

    STL_TRY( stlGetSockAddr( &sSclHandle->mClientAddr,
                             sAddress,
                             STL_MAX_HOST_LEN + 1,
                             &sPort,
                             NULL,   /* aFilePathBuf */
                             0,      /* aFilePathBufLen */
                             KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    if( (sOldPassword[0] == '\0') || (sNewPassword[0] == '\0') )
    {
        STL_TRY_THROW( sslConnect( sUserName,
                                   sEncryptedPassword,
                                   sSllHandle->mRoleName,
                                   NULL,
                                   NULL,
                                   sProc,
                                   sProgramName,
                                   sAddress,
                                   sPort,
                                   sCharacterset,
                                   sTimezone,
                                   &sNlsCharacterset,
                                   &sNlsNCharCharacterset,
                                   &sDBTimezone,
                                   SSL_ENV(aEnv) )
                       == STL_SUCCESS, RAMP_ERROR_SEND );
    }
    else
    {
        STL_TRY_THROW( sslConnect( sUserName,
                                   NULL,
                                   sSllHandle->mRoleName,
                                   sOldPassword,
                                   sNewPassword,
                                   sProc,
                                   sProgramName,
                                   sAddress,
                                   sPort,
                                   sCharacterset,
                                   sTimezone,
                                   &sNlsCharacterset,
                                   &sNlsNCharCharacterset,
                                   &sDBTimezone,
                                   SSL_ENV(aEnv) )
                       == STL_SUCCESS, RAMP_ERROR_SEND );
    }

    STL_TRY( sclWriteConnectResult( sSclHandle,
                                    sSllHandle->mSessionId,
                                    sSllHandle->mSessionSeq,
                                    sNlsCharacterset,
                                    sNlsNCharCharacterset,
                                    sDBTimezone,
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    /* stlPushError로 error설정을 한뒤에 RAMP_ERROR_SEND 로 와야 함 */
    STL_RAMP( RAMP_ERROR_SEND );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_CONNECT,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    sSclHandle->mSocketPeriod = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdDisconnect( sllCommandArgs * aArgs,
                            sllEnv         * aEnv )
{
    sllHandle * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadDisconnectCommand( sSclHandle,
                                       SCL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( sslDisconnect( SSL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( sclWriteDisconnectResult( sSclHandle,
                                       KNL_ERROR_STACK(aEnv),
                                       SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sSclHandle->mSocketPeriod = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdGetConnAttr( sllCommandArgs * aArgs,
                             sllEnv         * aEnv )
{
    stlInt8       sControl = CML_CONTROL_NONE;
    stlInt32      sAttrType;
    dtlDataType   sAttrDataType = DTL_TYPE_NATIVE_INTEGER;
    stlInt64      sStrValue[SSL_ATTRIBUTE_BUFFER_SIZE / STL_SIZEOF(stlInt64)];
    stlInt32      sInt32Value = 0;
    stlChar     * sAttrValue = NULL;
    stlInt32      sAttrValueSize;
    sllHandle   * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle   * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadGetConnAttrCommand( sSclHandle,
                                        &sControl,
                                        &sAttrType,
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    switch( sAttrType )
    {
        case SSL_CONNECTION_ATTR_ACCESS_MODE :
        case SSL_CONNECTION_ATTR_TXN_ISOLATION :
            sAttrDataType = DTL_TYPE_NATIVE_INTEGER;
            sAttrValue = (stlChar*)&sInt32Value;
            sAttrValueSize = STL_SIZEOF(stlInt32);
            break;
        default:
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid attribute type",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
            break;
    }

    STL_TRY_THROW( sslGetSessionAttr( sAttrType,
                                      &sInt32Value,
                                      (stlChar*)sStrValue,
                                      SSL_ATTRIBUTE_BUFFER_SIZE,
                                      &sAttrValueSize,
                                      SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );
    
    STL_TRY( sclWriteGetConnAttrResult( sSclHandle,
                                        sAttrType,
                                        sAttrDataType,
                                        sAttrValue,
                                        sAttrValueSize,
                                        KNL_ERROR_STACK(aEnv),
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_GETCONNATTR,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;

}


stlStatus slpCmdSetConnAttr( sllCommandArgs * aArgs,
                             sllEnv         * aEnv )
{
    stlInt32     sBuffer[SSL_ATTRIBUTE_BUFFER_SIZE / STL_SIZEOF(stlInt32)];
    stlInt32     sAttrType;
    stlInt16     sAttrValueSize;
    dtlDataType  sDataType;
    stlInt8      sControl = CML_CONTROL_NONE;
    sllHandle  * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle  * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadSetConnAttrCommand( sSclHandle,
                                        &sControl,
                                        &sAttrType,
                                        &sDataType,
                                        sBuffer,
                                        &sAttrValueSize,
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    STL_TRY_THROW( sslSetSessionAttr( sAttrType,
                                      *(stlInt32*)sBuffer,
                                      (stlChar*)sBuffer,
                                      sAttrValueSize,
                                      SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteSetConnAttrResult( sSclHandle,
                                        KNL_ERROR_STACK(aEnv),
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_SETCONNATTR,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdFreeStmt( sllCommandArgs * aArgs,
                          sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlInt16                sOption;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadFreeStmtCommand( sSclHandle,
                                     &sControl,
                                     &sStmtId,
                                     &sOption,
                                     SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }

    if( sStmtId == -1 )
    {
        if( sProtocolSentence->mCachedStmtId == -1 )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid statement id",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
        }
        sStmtId = sProtocolSentence->mCachedStmtId;
    }

    switch( sOption )
    {
        case SSL_FREE_STMT_OPTION_DROP :
        case SSL_FREE_STMT_OPTION_RESET_PARAMS :
            STL_TRY_THROW( sslFreeStatement( sStmtId,
                                             sOption,
                                             SSL_ENV(aEnv) )
                           == STL_SUCCESS, RAMP_FAIL_OPERATION );
            break;
        default:
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid free statement option",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
            break;
    }

    STL_TRY( sclWriteFreeStmtResult( sSclHandle,
                                     KNL_ERROR_STACK(aEnv),
                                     SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_FREESTMT,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_FREESTMT,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdCancel( sllCommandArgs * aArgs,
                        sllEnv         * aEnv )
{
    stlInt8       sControl = CML_CONTROL_NONE;
    stlInt32      sSessionId;
    stlInt64      sSessionSeq;
    stlInt64      sStatementId;
    sllHandle   * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle   * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadCancelCommand( sSclHandle,
                                   &sControl,
                                   &sSessionId,
                                   &sSessionSeq,
                                   &sStatementId,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    /*
    STL_TRY_THROW( sslCancel()
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );
    */

    STL_TRY( sclWriteCancelResult( sSclHandle,
                                   KNL_ERROR_STACK(aEnv),
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    /*
    STL_RAMP(RAMP_FAIL_OPERATION);

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_CANCEL,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;
    */

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdDescribeCol( sllCommandArgs * aArgs,
                             sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStatementId;
    stlInt32                sColumnNumber;
    qllTarget             * sTargetInfo;
    stlBool                 sIgnore = STL_FALSE;
    stlInt32                i;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadDescribeColCommand( sSclHandle,
                                        &sControl,
                                        &sStatementId,
                                        &sColumnNumber,
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }

    if( sStatementId == -1 )
    {
        if( sProtocolSentence->mCachedStmtId == -1 )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid statement id",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
        }
        sStatementId = sProtocolSentence->mCachedStmtId;
    }

    STL_TRY_THROW( sslDescribeCol( sStatementId,
                                   &sTargetInfo,
                                   SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    for( i = 1; i < sColumnNumber; i ++ )
    {
        sTargetInfo = sTargetInfo->mNext;
    }

    STL_TRY( sclWriteDescribeColResult( sSclHandle,
                                        sTargetInfo->mCatalogName,
                                        sTargetInfo->mSchemaName,
                                        sTargetInfo->mTableName,
                                        sTargetInfo->mBaseTableName,
                                        sTargetInfo->mColumnAlias,
                                        sTargetInfo->mColumnLabel,
                                        sTargetInfo->mBaseColumnName,
                                        sTargetInfo->mLiteralPrefix,
                                        sTargetInfo->mLiteralSuffix,
                                        sTargetInfo->mDBType,
                                        sTargetInfo->mIntervalCode,
                                        sTargetInfo->mNumPrecRadix,
                                        sTargetInfo->mDateTimeIntervalPrec,
                                        sTargetInfo->mDisplaySize,
                                        sTargetInfo->mStringLengthUnit,
                                        sTargetInfo->mCharacterLength,
                                        sTargetInfo->mOctetLength,
                                        sTargetInfo->mPrecision,
                                        sTargetInfo->mScale,
                                        sTargetInfo->mIsNullable,
                                        sTargetInfo->mIsAliasUnnamed,
                                        sTargetInfo->mIsCaseSensitive,
                                        sTargetInfo->mUnsigned,
                                        sTargetInfo->mIsExactNumeric,
                                        sTargetInfo->mIsUpdatable,
                                        sTargetInfo->mIsAutoUnique,
                                        sTargetInfo->mIsRowVersion,
                                        sTargetInfo->mIsAbleLike,
                                        KNL_ERROR_STACK(aEnv),
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_DESCRIBECOL,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_DESCRIBECOL,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdParameterType( sllCommandArgs * aArgs,
                               sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStatementId;
    stlInt32                sParameterNumber;
    stlInt16                sInputOutputType;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadParameterTypeCommand( sSclHandle,
                                          &sControl,
                                          &sStatementId,
                                          &sParameterNumber,
                                          SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }

    if( sStatementId == -1 )
    {
        if( sProtocolSentence->mCachedStmtId == -1 )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid statement id",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
        }
        sStatementId = sProtocolSentence->mCachedStmtId;
    }

    STL_TRY_THROW( sslGetParameterType( sStatementId,
                                        sParameterNumber,
                                        &sInputOutputType,
                                        SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteParameterTypeResult( sSclHandle,
                                          sInputOutputType,
                                          KNL_ERROR_STACK(aEnv),
                                          SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_PARAMETERTYPE,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_PARAMETERTYPE,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdTransaction( sllCommandArgs * aArgs,
                             sllEnv         * aEnv )
{
    stlInt8     sControl = CML_CONTROL_NONE;
    stlInt16    sOption;
    sllHandle * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadTransactionCommand( sSclHandle,
                                        &sControl,
                                        &sOption,
                                        SCL_ENV(aEnv) ) == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    else
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SLL_ERRCODE_INVALID_PROTOCOL,
                      "invalid control flag",
                      KNL_ERROR_STACK(aEnv) );
        STL_THROW( RAMP_FAIL_OPERATION );
    }

    switch( sOption )
    {
        case SSL_COMPLETION_TYPE_COMMIT :
            STL_TRY_THROW( sslCommit( SSL_ENV(aEnv) ) == STL_SUCCESS,
                           RAMP_FAIL_OPERATION );
            break;
        case SSL_COMPLETION_TYPE_ROLLBACK :
            STL_TRY_THROW( sslRollback( SSL_ENV(aEnv) ) == STL_SUCCESS,
                           RAMP_FAIL_OPERATION );
            break;
        default :
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid transaction option",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
            break;
    }

    STL_TRY( sclWriteTransactionResult( sSclHandle,
                                        KNL_ERROR_STACK(aEnv),
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP(RAMP_FAIL_OPERATION);

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_TRANSACTION,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdPrepare( sllCommandArgs * aArgs,
                         sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStatementId;
    stlChar               * sSqlText;
    stlInt64                sSqlTextSize;
    stlBool                 sHasTrans;
    stlInt32                sStmtType;
    stlBool                 sResultSet;
    stlBool                 sWithoutHoldCursor;
    stlBool                 sIsUpdatable;
    ellCursorSensitivity    sSensitivity;
    stlBool                 sAllocStmt = STL_FALSE;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadPrepareCommand( sSclHandle,
                                    &sControl,
                                    &sStatementId,
                                    &sSqlText,
                                    &sSqlTextSize,
                                    SCL_ENV(aEnv) ) == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );

        if( sStatementId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                STL_TRY_THROW( sslAllocStatement( &sProtocolSentence->mCachedStmtId,
                                                  SSL_ENV(aEnv) ) == STL_SUCCESS,
                               RAMP_FAIL_OPERATION );

                sAllocStmt = STL_TRUE;
            }

            sStatementId = sProtocolSentence->mCachedStmtId;
        }
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );

        if( sStatementId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              SLL_ERRCODE_INVALID_PROTOCOL,
                              "invalid statement id",
                              KNL_ERROR_STACK(aEnv) );
                STL_THROW( RAMP_FAIL_OPERATION );
            }
            sStatementId = sProtocolSentence->mCachedStmtId;
        }
    }

    STL_TRY_THROW( sslPrepare( sSqlText,
                               sStatementId,
                               &sHasTrans,
                               &sStmtType,
                               &sResultSet,
                               &sWithoutHoldCursor,
                               &sIsUpdatable,
                               &sSensitivity,
                               SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWritePrepareResult( sSclHandle,
                                    sStatementId,
                                    sStmtType,
                                    sHasTrans,
                                    sResultSet,
                                    sWithoutHoldCursor,
                                    sIsUpdatable,
                                    (stlInt8)sSensitivity,
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) ) == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_PREPARE,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_PREPARE,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sAllocStmt == STL_TRUE )
    {
        STL_TRY( sslFreeStatement( sProtocolSentence->mCachedStmtId,
                                   SSL_FREE_STMT_OPTION_DROP,
                                   SSL_ENV(aEnv) )
                 == STL_SUCCESS );
        sProtocolSentence->mCachedStmtId = -1;
    }

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdExecDirect( sllCommandArgs * aArgs,
                            sllEnv         * aEnv )
{
    stlInt8                sControl = CML_CONTROL_NONE;
    stlInt64               sStatementId;
    stlBool                sIsFirst;
    stlChar              * sSqlText;
    stlInt64               sSqlTextSize;
    stlBool                sHasTrans;
    stlBool                sIsRecompile;
    stlBool                sResultSet;
    stlBool                sWithoutHoldCursor;
    stlBool                sIsUpdatable;
    ellCursorSensitivity   sSensitivity;
    stlInt32               sStmtType;
    stlInt64               sAffectedRowCount;
    sslDCLContext          sDCLContext;

    stlInt8                 sDclAttrCount = 0;
    stlInt32                sDclAttrType = 0;
    dtlDataType             sDclAttrDataType = DTL_TYPE_NATIVE_INTEGER;
    stlInt32                sDclAttrValue = 0;
    stlInt32                sDclAttrValueSize = 4;
    stlBool                 sAllocStmt = STL_FALSE;
    stlBool                 sIgnore = STL_FALSE;
    stlInt32                sCharacterSet = 0;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    SSL_INIT_DCL_CONTEXT( &sDCLContext );
    
    STL_TRY( sclReadExecDirectCommand( sSclHandle,
                                       &sControl,
                                       &sStatementId,
                                       &sIsFirst,
                                       &sSqlText,
                                       &sSqlTextSize,
                                       SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );

        if( sStatementId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                STL_TRY_THROW( sslAllocStatement( &sProtocolSentence->mCachedStmtId,
                                                  SSL_ENV(aEnv) )
                               == STL_SUCCESS,
                               RAMP_FAIL_OPERATION );

                sAllocStmt = STL_TRUE;
            }

            sStatementId = sProtocolSentence->mCachedStmtId;
        }
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );

        if( sStatementId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              SLL_ERRCODE_INVALID_PROTOCOL,
                              "invalid statement id",
                              KNL_ERROR_STACK(aEnv) );
                STL_THROW( RAMP_FAIL_OPERATION );
            }
            sStatementId = sProtocolSentence->mCachedStmtId;
        }
    }

    STL_TRY_THROW( sslExecDirect( sSqlText,
                                  sStatementId,
                                  sIsFirst,
                                  &sHasTrans,
                                  &sIsRecompile,
                                  &sStmtType,
                                  &sResultSet,
                                  &sWithoutHoldCursor,
                                  &sIsUpdatable,
                                  &sSensitivity,
                                  &sAffectedRowCount,
                                  &sDCLContext,
                                  SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    switch( sDCLContext.mType )
    {
        case SSL_DCL_TYPE_NONE :
            break;
        case SSL_DCL_TYPE_END_TRANSACTION :
            sDclAttrType = sDCLContext.mType;
            sDclAttrCount = 1;
            break;
        case SSL_DCL_TYPE_SET_TIME_ZONE :
            sDclAttrType = sDCLContext.mType;
            sDclAttrCount = 1;
            break;
        case SSL_DCL_TYPE_OPEN_DATABASE :
            sDclAttrType = sDCLContext.mType;
            sDclAttrCount = 1;

            sCharacterSet = sDCLContext.mCharacterSet;
            sCharacterSet += sDCLContext.mNCharCharacterSet * CML_NCHAR_CHARACTERSET_OFFSET;

            sDclAttrValue = sCharacterSet;
            break;
        case SSL_DCL_TYPE_CLOSE_DATABASE :
            sDclAttrType = sDCLContext.mType;
            sDclAttrCount = 1;
            sProtocolSentence->mCloseDatabase = STL_TRUE;
            break;
        default :
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid dcl type",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
            break;
    }

    STL_TRY( sclWriteExecDirectResult( sSclHandle,
                                       sStatementId,
                                       sStmtType,
                                       sAffectedRowCount,
                                       sIsRecompile,
                                       sResultSet,
                                       sWithoutHoldCursor,
                                       sIsUpdatable,
                                       (stlInt8)sSensitivity,
                                       sHasTrans,
                                       sDclAttrCount,            /* ? */
                                       sDclAttrType,             /* ? */
                                       sDclAttrDataType,         /* ? */
                                       (stlChar*)&sDclAttrValue, /* ? */
                                       sDclAttrValueSize,        /* ? */
                                       KNL_ERROR_STACK(aEnv),
                                       SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    if( sDCLContext.mType == SSL_DCL_TYPE_CLOSE_DATABASE )
    {
        STL_TRY( sslFreeStatement( sStatementId,
                                   SSL_FREE_STMT_OPTION_DROP,
                                   SSL_ENV(aEnv) )
                 == STL_SUCCESS );
        sProtocolSentence->mCachedStmtId = -1;
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_EXECDIRECT,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_EXECDIRECT,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS);

    if( sAllocStmt == STL_TRUE )
    {
        STL_TRY( sslFreeStatement( sProtocolSentence->mCachedStmtId,
                                   SSL_FREE_STMT_OPTION_DROP,
                                   SSL_ENV(aEnv) )
                 == STL_SUCCESS );
        sProtocolSentence->mCachedStmtId = -1;
    }

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdExecute( sllCommandArgs * aArgs,
                         sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStatementId;
    stlBool                 sIsFirst;
    stlBool                 sHasTrans;
    stlBool                 sIsRecompile;
    stlBool                 sResultSet;
    stlInt64                sAffectedRowCount;
    sslDCLContext           sDCLContext;

    stlInt8                 sDclAttrCount = 0;
    stlInt32                sDclAttrType = 0;
    dtlDataType             sDclAttrDataType = DTL_TYPE_NATIVE_INTEGER;
    stlInt32                sDclAttrValue = 0;
    stlInt32                sDclAttrValueSize = 4;
    stlBool                 sIgnore = STL_FALSE;
    stlInt32                sCharacterSet = 0;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    SSL_INIT_DCL_CONTEXT( &sDCLContext );
    
    STL_TRY( sclReadExecuteCommand( sSclHandle,
                                    &sControl,
                                    &sStatementId,
                                    &sIsFirst,
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }

    if(sStatementId == -1)
    {
        if( sProtocolSentence->mCachedStmtId == -1 )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid statement id",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
        }
        sStatementId = sProtocolSentence->mCachedStmtId;
    }

    STL_TRY_THROW( sslExecute( sStatementId,
                               sIsFirst,
                               &sHasTrans,
                               &sIsRecompile,
                               &sResultSet,
                               &sAffectedRowCount,
                               &sDCLContext,
                               SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    switch( sDCLContext.mType )
    {
        case SSL_DCL_TYPE_NONE :
            break;
        case SSL_DCL_TYPE_END_TRANSACTION :
            sDclAttrType = sDCLContext.mType;
            sDclAttrCount = 1;
            break;
        case SSL_DCL_TYPE_SET_TIME_ZONE :
            sDclAttrType = sDCLContext.mType;
            sDclAttrCount = 1;
            break;
        case SSL_DCL_TYPE_OPEN_DATABASE :
            sDclAttrType = sDCLContext.mType;
            sDclAttrCount = 1;

            sCharacterSet = sDCLContext.mCharacterSet;
            sCharacterSet += sDCLContext.mNCharCharacterSet * CML_NCHAR_CHARACTERSET_OFFSET;

            sDclAttrValue = sCharacterSet;
            break;
        case SSL_DCL_TYPE_CLOSE_DATABASE :
            sDclAttrType = sDCLContext.mType;
            sDclAttrCount = 1;
            sProtocolSentence->mCloseDatabase = STL_TRUE;
            break;
        default :
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid dcl type",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
            break;
    }

    STL_TRY( sclWriteExecuteResult( sSclHandle,
                                    sAffectedRowCount,
                                    sIsRecompile,
                                    sResultSet,
                                    sHasTrans,
                                    sDclAttrCount,            /* ? */
                                    sDclAttrType,             /* ? */
                                    sDclAttrDataType,         /* ? */
                                    (stlChar*)&sDclAttrValue, /* ? */
                                    sDclAttrValueSize,        /* ? */
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    if( sDCLContext.mType == SSL_DCL_TYPE_CLOSE_DATABASE )
    {
        STL_TRY( sslFreeStatement( sStatementId,
                                   SSL_FREE_STMT_OPTION_DROP,
                                   SSL_ENV(aEnv) )
                 == STL_SUCCESS );
        sProtocolSentence->mCachedStmtId = -1;
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_EXECUTE,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_EXECUTE,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdGetOutBindData( sllCommandArgs * aArgs,
                                sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlInt32                sColumnNumber;
    dtlDataValue          * sBindValue;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadGetOutBindDataCommand( sSclHandle,
                                           &sControl,
                                           &sStmtId,
                                           &sColumnNumber,
                                           SCL_ENV(aEnv) ) == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }

    if( sStmtId == -1 )
    {
        if( sProtocolSentence->mCachedStmtId == -1 )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid statement id",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
        }
        sStmtId = sProtocolSentence->mCachedStmtId;
    }

    STL_TRY_THROW( sslGetParameterData( sStmtId,
                                        sColumnNumber,
                                        NULL,
                                        &sBindValue,
                                        SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteGetOutBindDataResult( sSclHandle,
                                           sBindValue,
                                           KNL_ERROR_STACK(aEnv),
                                           SCL_ENV(aEnv) ) == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_GETOUTBINDDATA,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_GETOUTBINDDATA,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdFetch( sllCommandArgs * aArgs,
                       sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlInt64                sOffset;
    stlInt32                sReqRowCount;
    knlValueBlockList     * sValueBlockList;

    stlBool                 sNoData = STL_FALSE;
    stlInt64                sRowsetFirstRow;
    stlInt64                sRowsetCurrentPos;
    stlInt64                sRowsetLastPos;
    ellCursorSensitivity    sSensitivity;
    ellCursorUpdatability   sUpdatability;
    ellCursorScrollability  sScrollability;
    
    stlBool                 sIsEndOfScan = STL_FALSE;
    stlBool                 sIsBlockRead;
    qllCursorRowStatus      sRowStatus = QLL_CURSOR_ROW_STATUS_NO_CHANGE;
    stlInt32                sOneRowIdx;
    stlInt64                sLastResultRow = SSL_LAST_ROW_IDX_UNKNOWN;

    stlInt64                sSentRowCount = 0;
    stlInt64                sSentPacketMark;

    stlInt64                sRowCount   = 0;
    stlBool                 sFetchBegun = STL_FALSE;
    stlBool                 sFetchSent  = STL_FALSE;
    stlBool                 sIgnore = STL_FALSE;
    stlBool                 sCursorClosed = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );
    STL_TRY( sclReadFetchCommand( sSclHandle,
                                  &sControl,
                                  &sStmtId,
                                  &sOffset,
                                  &sReqRowCount,
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }
    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

    if( sStmtId == -1 )
    {
        if( sProtocolSentence->mCachedStmtId == -1 )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid statement id",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
        }
        sStmtId = sProtocolSentence->mCachedStmtId;
    }
    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

    STL_TRY_THROW( sslGetFetchPosition( sStmtId,
                                        &sRowsetFirstRow,
                                        &sRowsetCurrentPos,
                                        &sRowsetLastPos,
                                        &sLastResultRow,
                                        &sSensitivity,
                                        &sUpdatability,
                                        &sScrollability,
                                        &sValueBlockList,
                                        SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY_THROW( sSensitivity != ELL_CURSOR_SENSITIVITY_NA, RAMP_IGNORE );
    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

    while( (sReqRowCount == SLL_FETCH_COUNT_UNLIMITED) ||
           ((sReqRowCount == SLL_FETCH_COUNT_AUTO) && (sFetchSent == STL_FALSE)) ||
           ((sReqRowCount != SLL_FETCH_COUNT_UNLIMITED) && (sReqRowCount != SLL_FETCH_COUNT_AUTO) && (sSentRowCount < sReqRowCount)) )
    {
        /* cursor의 sensitivity가 SENSITIVE인 경우 및 레코드가 rowset에 없는 경우 */
        if( (sSensitivity == ELL_CURSOR_SENSITIVITY_SENSITIVE) ||
            (sRowsetFirstRow == SSL_CURSOR_POSITION_BEFORE_FIRST) ||
            (sOffset < sRowsetFirstRow ) ||
            (sOffset >= sRowsetFirstRow + sRowsetLastPos) )
        {
            if( sIsEndOfScan == STL_TRUE )
            {
                sNoData = STL_TRUE;
                break;
            }
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

            STL_TRY_THROW( sslFetch( sStmtId,
                                     sOffset,
                                     &sValueBlockList,
                                     &sIsEndOfScan,
                                     &sIsBlockRead,
                                     &sOneRowIdx,
                                     &sRowStatus,
                                     &sRowsetFirstRow, /* fetch된 row block의 첫번째 row의 절대 위치 */
                                     &sLastResultRow,  /* 전체 row result의 마지막 row의 절대 위치. 모를 수도 있음 (-1) */
                                     SSL_ENV(aEnv) ) == STL_SUCCESS,
                           RAMP_FAIL_OPERATION );
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

            if( sLastResultRow != SSL_LAST_ROW_IDX_UNKNOWN )
            {
                /* result가 왔지만 유효한 범위가 아닌 경우 end of scan으로 간주한다. */
                if( (sOffset == 0) ||
                    (sOffset > sLastResultRow) ||
                    (sOffset < -sLastResultRow) )
                {
                    sNoData = STL_TRUE;
                    break;
                }
            }
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

            if( sOffset < 0 )
            {
                /* sOffset이 음수이면 반드시 마지막 row idx가 세팅되어있을 것이다. */
                STL_DASSERT( sLastResultRow != SSL_LAST_ROW_IDX_UNKNOWN );

                /* sIsEndOfScan이 false가 아니므로 반드시 결과 row가 있을 것이고,
                   row가 있다는 의미는 sOffset이 반드시 유효한 값이란 뜻이다.
                   따라서 sKnownLastRowIdx + sOffset + 1은 1이하일 수가 없다. */
                sOffset = sLastResultRow + sOffset + 1;
            }
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

            if( sIsBlockRead == STL_TRUE )
            {
                sRowsetCurrentPos = DTL_GET_BLOCK_SKIP_CNT( sValueBlockList );
                sRowsetLastPos    = DTL_GET_BLOCK_USED_CNT( sValueBlockList );

                if( sRowsetLastPos == 0 )
                {
                    sNoData = STL_TRUE;
                    break;
                }

                if( sOffset != sRowsetFirstRow )
                {
                    /* rowset의 첫번째 row가 요청한 offset 위치의 row가 아니라면
                       offset보다 이전의 row여야 한다. */
                    STL_DASSERT( sOffset > sRowsetFirstRow );

                    /* 이 경우 offset 이전의 row들은 버린다. */
                    sRowsetCurrentPos = sOffset - sRowsetFirstRow;
                }

                /* sRowsetCurrentPos가 sRowsetLastPos와 같다면,
                 * rowset에 원하는 offset의 row가 없다는 의미이다. */
                STL_DASSERT( sRowsetLastPos > sRowsetCurrentPos );
                STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

                if( sReqRowCount == SLL_FETCH_COUNT_AUTO )
                {
                    sRowCount = sRowsetLastPos - sRowsetCurrentPos;
                }
                else if( sReqRowCount == SLL_FETCH_COUNT_UNLIMITED )
                {
                    sRowCount = sRowsetLastPos - sRowsetCurrentPos;
                }
                else
                {
                    sRowCount = STL_MIN( sRowsetLastPos - sRowsetCurrentPos,
                                         (sReqRowCount - sSentRowCount) );
                }
            }
            else
            {
                if( sOneRowIdx < 0 )
                {
                    sNoData = STL_TRUE;
                    break;
                }
                STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

                sRowsetCurrentPos = sOneRowIdx;
                sRowsetLastPos    = sOneRowIdx;

                STL_DASSERT( sOffset == sRowsetFirstRow );

                sRowCount = 1;
            }
        }
        else
        {
            sRowsetCurrentPos = sOffset - sRowsetFirstRow;
            if( sReqRowCount == SLL_FETCH_COUNT_AUTO )
            {
                sRowCount = sRowsetLastPos - sRowsetCurrentPos;
            }
            else if( sReqRowCount == SLL_FETCH_COUNT_UNLIMITED )
            {
                sRowCount = sRowsetLastPos - sRowsetCurrentPos;
            }
            else
            {
                sRowCount = STL_MIN( sRowsetLastPos - sRowsetCurrentPos,
                                     (sReqRowCount - sSentRowCount) );
            }
        }
        STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

        if( sFetchBegun == STL_FALSE )
        {
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

            STL_TRY( sclWriteFetchBegin( sSclHandle,
                                         sOffset,
                                         sValueBlockList,
                                         KNL_ERROR_STACK(aEnv),
                                         SCL_ENV(aEnv) ) == STL_SUCCESS );
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

            sFetchBegun = STL_TRUE;
        }

        SCL_HANDLE_SENT_MARK( sSclHandle, sSentPacketMark );

        STL_DASSERT( sRowCount <= STL_INT32_MAX );

        if( sRowCount > 0 )
        {
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

            STL_TRY( sclWriteFetchResult( sSclHandle,
                                          sRowsetCurrentPos,
                                          (stlInt32)sRowCount,
                                          sValueBlockList,
                                          (stlInt8)sRowStatus,
                                          SCL_ENV(aEnv) )
                     == STL_SUCCESS );
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

        }

        sFetchSent = SCL_HANDLE_HAS_SENT( sSclHandle, sSentPacketMark );
        STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

        /* sIsEndOfScan이 false이면 sOffset은 반드시 양수로 보정된 값이어야 한다. */
        STL_DASSERT( sOffset > 0 );

        sSentRowCount += sRowCount;
        sOffset += sRowCount;
    }
    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

    if( sNoData == STL_TRUE )
    {
        sRowsetCurrentPos = 0;
        sRowsetLastPos    = 0;
        sRowCount         = 0;

        if( sOffset <= 0 )
        {
            sRowsetFirstRow = SSL_CURSOR_POSITION_BEFORE_FIRST;
        }
        else
        {
            sRowsetFirstRow = SSL_CURSOR_POSITION_AFTER_END;
        }

        if( sFetchBegun == STL_FALSE )
        {
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

            STL_TRY( sclWriteFetchBegin( sSclHandle,
                                         sOffset,
                                         sValueBlockList,
                                         KNL_ERROR_STACK(aEnv),
                                         SCL_ENV(aEnv) ) == STL_SUCCESS );
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

            sFetchBegun = STL_TRUE;
        }

        if( (sUpdatability == ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY) &&
            (sScrollability == ELL_CURSOR_SCROLLABILITY_NO_SCROLL) )
        {
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

            STL_TRY_THROW( sslCloseCursor( sStmtId,
                                           SSL_ENV(aEnv) ) == STL_SUCCESS,
                           RAMP_FAIL_OPERATION );
            STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

            sCursorClosed = STL_TRUE;
        }
    }
    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

    STL_TRY_THROW( sslSetFetchPosition( sStmtId,
                                        sRowsetFirstRow,
                                        sRowsetCurrentPos,
                                        sRowsetLastPos,
                                        SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );
    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

    STL_DASSERT( sFetchBegun == STL_TRUE );
    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

    STL_TRY( sclWriteFetchEnd( sSclHandle,
                               sLastResultRow,
                               sCursorClosed,
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_FETCH,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    if( sFetchBegun == STL_TRUE )
    {
        /* fetch 도중에 에러가 난 경우 */
        STL_TRY( sclWriteFetchEndWithError( sSclHandle,
                                            SCL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /* fetch 결과를 아직 하나도 기록하지 않은 상태에서 에러난 경우 */
        STL_TRY( sclWriteErrorResult( sSclHandle,
                                      CML_COMMAND_FETCH,
                                      KNL_ERROR_STACK(aEnv),
                                      SCL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    STL_DASSERT( sProtocolSentence->mSclProtocolSentence.mWriteStartPos != NULL );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpGetTargetType( void               * sSllHandle,
                            dtlValueBlockList  * aTargetColumn,
                            stlInt8            * aDataType,
                            stlInt8            * aStringLengthUnit,
                            stlInt8            * aIntervalIndicator,
                            stlInt64           * aArgNum1,
                            stlInt64           * aArgNum2,
                            void               * aEnv )
{
    dtlValueBlock * sValueBlock;

    sValueBlock = aTargetColumn->mValueBlock;

    *aDataType = sValueBlock->mDataValue->mType;
    *aStringLengthUnit = sValueBlock->mStringLengthUnit;
    *aIntervalIndicator = sValueBlock->mIntervalIndicator;
    *aArgNum1 = sValueBlock->mArgNum1;
    *aArgNum2 = sValueBlock->mArgNum2;

    return STL_SUCCESS;
}


stlStatus slpCmdTargetType( sllCommandArgs * aArgs,
                            sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlBool                 sChanged = STL_FALSE;
    dtlValueBlockList     * sTargetBlockList = NULL;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadTargetTypeCommand( sSclHandle,
                                       &sControl,
                                       &sStmtId,
                                       SCL_ENV(aEnv) ) == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }

    if( sStmtId == -1 )
    {
        if( sProtocolSentence->mCachedStmtId == -1 )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid statement id",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
        }
        sStmtId = sProtocolSentence->mCachedStmtId;
    }

    STL_TRY_THROW( sslGetTargetType( sStmtId,
                                     &sChanged,
                                     &sTargetBlockList,
                                     SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteTargetTypeResult( sSclHandle,
                                       sChanged,
                                       sTargetBlockList,
                                       slpGetTargetType,
                                       KNL_ERROR_STACK(aEnv),
                                       SCL_ENV(aEnv) ) == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_TARGETTYPE,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_TARGETTYPE,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdBindType( sllCommandArgs * aArgs,
                          sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlInt32                sColumnNumber;
    stlInt8                 sBindType;
    stlInt8                 sDataType;
    stlInt64                sArg1;
    stlInt64                sArg2;
    stlInt8                 sStringLengthUnit;
    stlInt8                 sIntervalIndicator;
    stlBool                 sAllocStmt = STL_FALSE;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadBindTypeCommand( sSclHandle,
                                     &sControl,
                                     &sStmtId,
                                     &sColumnNumber,
                                     &sBindType,
                                     &sDataType,
                                     &sArg1,
                                     &sArg2,
                                     &sStringLengthUnit,
                                     &sIntervalIndicator,
                                     SCL_ENV(aEnv) ) == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                STL_TRY_THROW( sslAllocStatement( &sProtocolSentence->mCachedStmtId,
                                                  SSL_ENV(aEnv) ) == STL_SUCCESS,
                               RAMP_FAIL_OPERATION );

                sAllocStmt = STL_TRUE;
            }

            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              SLL_ERRCODE_INVALID_PROTOCOL,
                              "invalid statement id",
                              KNL_ERROR_STACK(aEnv) );
                STL_THROW( RAMP_FAIL_OPERATION );
            }
            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }

    /*
     * 파라메터 할당 및 설정
     */

    STL_TRY_THROW( sslCreateParameter( sStmtId,
                                       sColumnNumber,
                                       (sslParamType)sBindType,
                                       (dtlDataType)sDataType,
                                       sArg1,
                                       sArg2,
                                       (dtlStringLengthUnit)sStringLengthUnit,
                                       (dtlIntervalIndicator)sIntervalIndicator,
                                       NULL,
                                       NULL,
                                       SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteBindTypeResult( sSclHandle,
                                     sStmtId,
                                     KNL_ERROR_STACK(aEnv),
                                     SCL_ENV(aEnv) ) == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_BINDTYPE,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_BINDTYPE,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sAllocStmt == STL_TRUE )
    {
        STL_TRY( sslFreeStatement( sProtocolSentence->mCachedStmtId,
                                   SSL_FREE_STMT_OPTION_DROP,
                                   SSL_ENV(aEnv) )
                 == STL_SUCCESS );
        sProtocolSentence->mCachedStmtId = -1;
    }

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void slpCheckControlCallback( void           * aSllHandle,
                              stlInt64       * aStatementId,
                              stlInt8          aControl,
                              stlBool        * aIgnore,
                              stlBool        * aProtocolError,
                              void           * aEnv )
{
    sllProtocolSentence  * sProtocolSentence = SLL_PROTOCOL_SENTENCE(aSllHandle);

    *aIgnore = STL_FALSE;
    *aProtocolError = STL_FALSE;

    if( SCL_IS_HEAD(aControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(aSllHandle) );
    }
    else
    {
        *aIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(aSllHandle) );
    }

    if( *aIgnore == STL_FALSE )
    {
        if( *aStatementId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              SLL_ERRCODE_INVALID_PROTOCOL,
                              "invalid statement id",
                              KNL_ERROR_STACK(aEnv) );
                *aIgnore = STL_TRUE;
                *aProtocolError = STL_TRUE;
            }
            *aStatementId = sProtocolSentence->mCachedStmtId;
        }
    }
}

stlStatus slpGetBindValueCallback( void           * aSllHandle,
                                   stlInt64         aStatementId,
                                   stlInt32         aColumnNumber,
                                   stlInt8          aControl,
                                   dtlDataValue  ** aBindValue,
                                   void           * aEnv )
{
    stlBool      sIgnore = STL_FALSE;
    sllHandle  * sHandle = (sllHandle*)aSllHandle;

    if( SCL_IS_HEAD(aControl) == STL_FALSE )
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }

    STL_TRY( sslGetParameterData( aStatementId,
                                  aColumnNumber,
                                  aBindValue,
                                  NULL,
                                  SSL_ENV(aEnv) ) == STL_SUCCESS );

    STL_RAMP( RAMP_IGNORE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpReallocLongVaryingMemCallback( void           * aSllHandle,
                                            dtlDataValue   * aBindValue,
                                            void           * aCallbackContext,
                                            void           * aEnv )
{
    STL_TRY( sslReallocLongVaryingMem( aBindValue,
                                       SSL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdBindData( sllCommandArgs * aArgs,
                          sllEnv         * aEnv )
{
    stlInt8     sControl = CML_CONTROL_NONE;
    stlBool     sIgnore = STL_FALSE;
    stlBool     sProtocolError = STL_FALSE;
    sllHandle * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadBindDataCommand( sSclHandle,
                                     &sControl,
                                     &sProtocolError,
                                     slpCheckControlCallback,
                                     slpGetBindValueCallback,
                                     slpReallocLongVaryingMemCallback,
                                     SCL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY_THROW( sProtocolError == STL_FALSE, RAMP_FAIL_OPERATION );

    if( SCL_IS_HEAD(sControl) == STL_FALSE )
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }

    STL_TRY( sclWriteBindDataResult( sSclHandle,
                                     KNL_ERROR_STACK(aEnv),
                                     SCL_ENV(aEnv) ) == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_BINDDATA,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_BINDDATA,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdGetStmtAttr( sllCommandArgs * aArgs,
                             sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlInt32                sAttrType;
    stlInt32                sBufferLength;
    dtlDataType             sAttrDataType = DTL_TYPE_NATIVE_INTEGER;
    stlChar               * sStrValue = NULL;
    stlInt64                sInt64Value;
    stlInt32                sInt32Value;
    stlChar               * sAttrValue = NULL;
    stlInt32                sAttrValueSize = 0;
    stlInt32                sStringLength;
    stlBool                 sAllocStmt = STL_FALSE;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadGetStmtAttrCommand( sSclHandle,
                                        &sControl,
                                        &sStmtId,
                                        &sAttrType,
                                        &sBufferLength,
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                STL_TRY_THROW( sslAllocStatement( &sProtocolSentence->mCachedStmtId,
                                                  SSL_ENV(aEnv) ) == STL_SUCCESS,
                               RAMP_FAIL_OPERATION );

                sAllocStmt = STL_TRUE;
            }

            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              SLL_ERRCODE_INVALID_PROTOCOL,
                              "invalid statement id",
                              KNL_ERROR_STACK(aEnv) );
                STL_THROW( RAMP_FAIL_OPERATION );
            }
            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }

    switch( sAttrType )
    {
        case SSL_STATEMENT_ATTR_PARAMSET_SIZE:
        case SSL_STATEMENT_ATTR_CONCURRENCY:
        case SSL_STATEMENT_ATTR_CURSOR_SCROLLABLE:
        case SSL_STATEMENT_ATTR_CURSOR_SENSITIVITY:
        case SSL_STATEMENT_ATTR_CURSOR_TYPE:
        case SSL_STATEMENT_ATTR_ATOMIC_EXECUTION:
        case SSL_STATEMENT_ATTR_EXPLAIN_PLAN_OPTION:
        case SSL_STATEMENT_ATTR_QUERY_TIMEOUT:
            STL_TRY_THROW( sslGetStmtAttr( sStmtId,
                                           sAttrType,
                                           &sInt64Value,
                                           NULL,
                                           0,
                                           NULL,
                                           SSL_ENV(aEnv) )
                           == STL_SUCCESS, RAMP_FAIL_OPERATION );

            sInt32Value = sInt64Value; 

            sAttrDataType  = DTL_TYPE_NATIVE_INTEGER;
            sAttrValue     = (stlChar*)(&sInt32Value);
            sAttrValueSize = STL_SIZEOF(stlInt32);
            sStringLength  = 0;
            break;
        case SSL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT:
            if( sBufferLength == 0 )
            {
                STL_TRY_THROW( sslGetStmtAttr( sStmtId,
                                               sAttrType,
                                               NULL,
                                               NULL,
                                               0,
                                               &sStringLength,
                                               SSL_ENV(aEnv) )
                               == STL_SUCCESS, RAMP_FAIL_OPERATION );

                sAttrValueSize = 0;
            }
            else
            {
                STL_TRY_THROW( stlAllocHeap( (void**)&sStrValue,
                                             sBufferLength,
                                             KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                               RAMP_FAIL_OPERATION );

                stlMemset( sStrValue, 0x00, sBufferLength );
                                         
                STL_TRY_THROW( sslGetStmtAttr( sStmtId,
                                               sAttrType,
                                               NULL,
                                               sStrValue,
                                               sBufferLength,
                                               &sStringLength,
                                               SSL_ENV(aEnv) )
                               == STL_SUCCESS, RAMP_FAIL_OPERATION );

                sAttrValueSize = stlStrlen(sStrValue);
            }

            sAttrDataType = DTL_TYPE_CHAR;
            sAttrValue    = sStrValue;
            break;
        default:
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid statement attribute type",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
            break;
    }

    STL_TRY( sclWriteGetStmtAttrResult( sSclHandle,
                                        sStmtId,
                                        sAttrType,
                                        sAttrDataType,
                                        sAttrValue,
                                        sAttrValueSize,
                                        sStringLength,
                                        KNL_ERROR_STACK(aEnv),
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    if( sStrValue != NULL )
    {
        stlFreeHeap( sStrValue );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_GETSTMTATTR,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    if( sStrValue != NULL )
    {
        stlFreeHeap( sStrValue );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_GETSTMTATTR,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sAllocStmt == STL_TRUE )
    {
        STL_TRY( sslFreeStatement( sProtocolSentence->mCachedStmtId,
                                   SSL_FREE_STMT_OPTION_DROP,
                                   SSL_ENV(aEnv) )
                 == STL_SUCCESS );
        sProtocolSentence->mCachedStmtId = -1;
    }

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    if( sStrValue != NULL )
    {
        stlFreeHeap( sStrValue );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sStrValue != NULL )
    {
        stlFreeHeap( sStrValue );
    }

    return STL_FAILURE;
}


stlStatus slpCmdSetStmtAttr( sllCommandArgs * aArgs,
                             sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlInt32                sAttrType;
    dtlDataType             sAttrDataType;
    stlInt32                sAttrValue[SSL_ATTRIBUTE_BUFFER_SIZE / STL_SIZEOF(stlInt32)];
    stlInt16                sAttrValueSize;
    stlInt64                sIntValue = 0;
    stlChar               * sStrValue = NULL;
    stlBool                 sAllocStmt = STL_FALSE;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadSetStmtAttrCommand( sSclHandle,
                                        &sControl,
                                        &sStmtId,
                                        &sAttrType,
                                        &sAttrDataType,
                                        (void *)sAttrValue,
                                        &sAttrValueSize,
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                STL_TRY_THROW( sslAllocStatement( &sProtocolSentence->mCachedStmtId,
                                                  SSL_ENV(aEnv) ) == STL_SUCCESS,
                               RAMP_FAIL_OPERATION );

                sAllocStmt = STL_TRUE;
            }

            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              SLL_ERRCODE_INVALID_PROTOCOL,
                              "invalid statement id",
                              KNL_ERROR_STACK(aEnv) );
                STL_THROW( RAMP_FAIL_OPERATION );
            }
            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }

    switch( sAttrDataType )
    {
        case DTL_TYPE_NATIVE_INTEGER:
            sIntValue = *(stlInt32 *)sAttrValue;
            sStrValue = NULL;
            break;
        case DTL_TYPE_CHAR:
            sIntValue = 0;
            sStrValue = (stlChar*)sAttrValue;
            break;
        default:
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid attribute data type",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
            break;
    }

    STL_TRY_THROW( sslSetStmtAttr( sStmtId,
                                   sAttrType,
                                   sIntValue,
                                   sStrValue,
                                   sAttrValueSize,
                                   SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteSetStmtAttrResult( sSclHandle,
                                        sStmtId,
                                        KNL_ERROR_STACK(aEnv),
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_SETSTMTATTR,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_SETSTMTATTR,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sAllocStmt == STL_TRUE )
    {
        STL_TRY( sslFreeStatement( sProtocolSentence->mCachedStmtId,
                                   SSL_FREE_STMT_OPTION_DROP,
                                   SSL_ENV(aEnv) )
                 == STL_SUCCESS );
        sProtocolSentence->mCachedStmtId = -1;
    }
    
    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdNumParams( sllCommandArgs * aArgs,
                           sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlInt16                sParamCount;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadNumParamsCommand( sSclHandle,
                                      &sControl,
                                      &sStmtId,
                                      SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }

    if( sStmtId == -1 )
    {
        if( sProtocolSentence->mCachedStmtId == -1 )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid statement id",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
        }
        sStmtId = sProtocolSentence->mCachedStmtId;
    }

    STL_TRY_THROW( sslNumParams( sStmtId,
                                 &sParamCount,
                                 SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteNumParamsResult( sSclHandle,
                                      sParamCount,
                                      KNL_ERROR_STACK(aEnv),
                                      SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_NUMPARAMS,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_NUMPARAMS,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdNumResultCols( sllCommandArgs * aArgs,
                               sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlInt16                sInt16Value;
    stlInt32                sInt32Value;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadNumResultColsCommand( sSclHandle,
                                          &sControl,
                                          &sStmtId,
                                          SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }
    
    if( sStmtId == -1 )
    {
        if( sProtocolSentence->mCachedStmtId == -1 )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid statement id",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
        }
        sStmtId = sProtocolSentence->mCachedStmtId;
    }

    STL_TRY_THROW( sslNumResultCols( sStmtId,
                                     &sInt16Value,
                                     SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    sInt32Value = sInt16Value;

    STL_TRY( sclWriteNumResultColsResult( sSclHandle,
                                          sInt32Value,
                                          KNL_ERROR_STACK(aEnv),
                                          SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_NUMRESULTCOLS,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_NUMRESULTCOLS,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdGetCursorName( sllCommandArgs * aArgs,
                               sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlBool                 sAllocStmt = STL_FALSE;
    stlChar                 sCursorName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    stlMemset( sCursorName, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );

    STL_TRY( sclReadGetCursorNameCommand( sSclHandle,
                                          &sControl,
                                          &sStmtId,
                                          SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                STL_TRY_THROW( sslAllocStatement( &sProtocolSentence->mCachedStmtId,
                                                  SSL_ENV(aEnv) ) == STL_SUCCESS,
                               RAMP_FAIL_OPERATION );

                sAllocStmt = STL_TRUE;
            }

            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              SLL_ERRCODE_INVALID_PROTOCOL,
                              "invalid statement id",
                              KNL_ERROR_STACK(aEnv) );
                STL_THROW( RAMP_FAIL_OPERATION );
            }
            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }

    STL_TRY_THROW( sslGetCursorName( sStmtId,
                                     sCursorName,
                                     SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteGetCursorNameResult( sSclHandle,
                                          sStmtId,
                                          sCursorName,
                                          KNL_ERROR_STACK(aEnv),
                                          SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_GETCURSORNAME,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_GETCURSORNAME,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sAllocStmt == STL_TRUE )
    {
        STL_TRY( sslFreeStatement( sProtocolSentence->mCachedStmtId,
                                   SSL_FREE_STMT_OPTION_DROP,
                                   SSL_ENV(aEnv) )
                 == STL_SUCCESS );
        sProtocolSentence->mCachedStmtId = -1;
    }
    
    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdSetCursorName( sllCommandArgs * aArgs,
                               sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlBool                 sAllocStmt = STL_FALSE;
    stlChar                 sCursorName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    stlMemset( sCursorName, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );

    STL_TRY( sclReadSetCursorNameCommand( sSclHandle,
                                          &sControl,
                                          &sStmtId,
                                          sCursorName,
                                          SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                STL_TRY_THROW( sslAllocStatement( &sProtocolSentence->mCachedStmtId,
                                                  SSL_ENV(aEnv) )
                               == STL_SUCCESS,
                               RAMP_FAIL_OPERATION );

                sAllocStmt = STL_TRUE;
            }

            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              SLL_ERRCODE_INVALID_PROTOCOL,
                              "invalid statement id",
                              KNL_ERROR_STACK(aEnv) );
                STL_THROW( RAMP_FAIL_OPERATION );
            }
            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }

    STL_TRY_THROW( sslSetCursorName( sStmtId,
                                     sCursorName,
                                     SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteSetCursorNameResult( sSclHandle,
                                          sStmtId,
                                          KNL_ERROR_STACK(aEnv),
                                          SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_SETCURSORNAME,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_SETCURSORNAME,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sAllocStmt == STL_TRUE )
    {
        STL_TRY( sslFreeStatement( sProtocolSentence->mCachedStmtId,
                                   SSL_FREE_STMT_OPTION_DROP,
                                   SSL_ENV(aEnv) )
                 == STL_SUCCESS );
        sProtocolSentence->mCachedStmtId = -1;
    }
    
    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdSetPos( sllCommandArgs * aArgs,
                        sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlBool                 sAllocStmt = STL_FALSE;
    stlInt64                sRowNum;
    stlInt16                sOperation;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadSetPosCommand( sSclHandle,
                                   &sControl,
                                   &sStmtId,
                                   &sRowNum,
                                   &sOperation,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                STL_TRY_THROW( sslAllocStatement( &sProtocolSentence->mCachedStmtId,
                                                  SSL_ENV(aEnv) )
                               == STL_SUCCESS,
                               RAMP_FAIL_OPERATION );

                sAllocStmt = STL_TRUE;
            }

            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );

        if( sStmtId == -1 )
        {
            if( sProtocolSentence->mCachedStmtId == -1 )
            {
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              SLL_ERRCODE_INVALID_PROTOCOL,
                              "invalid statement id",
                              KNL_ERROR_STACK(aEnv) );
                STL_THROW( RAMP_FAIL_OPERATION );
            }
            sStmtId = sProtocolSentence->mCachedStmtId;
        }
    }

    STL_TRY_THROW( sslSetPos( sStmtId,
                              sRowNum,
                              sOperation,
                              SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteSetPosResult( sSclHandle,
                                   sStmtId,
                                   KNL_ERROR_STACK(aEnv),
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_SETCURSORNAME,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_SETCURSORNAME,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sAllocStmt == STL_TRUE )
    {
        STL_TRY( sslFreeStatement( sProtocolSentence->mCachedStmtId,
                                   SSL_FREE_STMT_OPTION_DROP,
                                   SSL_ENV(aEnv) )
                 == STL_SUCCESS );
        sProtocolSentence->mCachedStmtId = -1;
    }
    
    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdCloseCursor( sllCommandArgs * aArgs,
                             sllEnv         * aEnv )
{
    stlInt8                 sControl = CML_CONTROL_NONE;
    stlInt64                sStmtId;
    stlBool                 sIgnore = STL_FALSE;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadCloseCursorCommand( sSclHandle,
                                        &sControl,
                                        &sStmtId,
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }
    else
    {
        sIgnore = SLL_GET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle) );

        STL_TRY_THROW( sIgnore == STL_FALSE, RAMP_IGNORE );
    }

    if( sStmtId == -1 )
    {
        if( sProtocolSentence->mCachedStmtId == -1 )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SLL_ERRCODE_INVALID_PROTOCOL,
                          "invalid statement id",
                          KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FAIL_OPERATION );
        }
        sStmtId = sProtocolSentence->mCachedStmtId;
    }

    STL_TRY_THROW( sslCloseCursor( sStmtId,
                                   SSL_ENV(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteCloseCursorResult( sSclHandle,
                                        KNL_ERROR_STACK(aEnv),
                                        SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_IGNORE );

    STL_TRY( sclWriteIgnoreResult( sSclHandle,
                                   CML_COMMAND_CLOSECURSOR,
                                   SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_CLOSECURSOR,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus slpCmdStartUp( sllCommandArgs * aArgs,
                         sllEnv         * aEnv )
{
    stlInt8          sControl;
    stlChar        * sArgv[3];
    sllHandle      * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle      * sSclHandle = SCL_HANDLE(sSllHandle);

    SLL_INIT_PROTOCOLSET_STACK( SLL_PROTOCOL_STACK(sSllHandle) );

    STL_TRY( sclReadStartupCommand( sSclHandle,
                                    &sControl,
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    /**
     * shared mode에서는 startup 못하도록 함.
     * shutdown은 execDirect로 호출됨으로 slp함수에서 막을수 없음.
     * startup은 여기서 막지 않으면 session이 끊어졌다 붙었다함.(closeDatabase 후 connectSQL처리 ..)
     */
    if( sSclHandle->mCommunication.mType == SCL_COMMUNICATION_IPC )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SLL_ERRCODE_STARTUP_MUST_DEDICATED,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );

        STL_THROW( RAMP_FAIL_OPERATION );
    }

    /* gmaster를 시작한다. */
    sArgv[0] = "gmaster";
    sArgv[1] = NULL;

    STL_TRY_THROW( stlExecuteProcAndWait( sArgv,
                                          KNL_ERROR_STACK(aEnv) )
                   == STL_SUCCESS, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteStartupResult( sSclHandle,
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    SLL_SET_PROTOCOLSET_ERROR( SLL_PROTOCOL_STACK(sSllHandle), STL_TRUE );

    STL_TRY( sclWriteErrorResult( sSclHandle,
                                  CML_COMMAND_STARTUP,
                                  KNL_ERROR_STACK(aEnv),
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    sSclHandle->mSocketPeriod = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdXaClose( sllCommandArgs * aArgs,
                         sllEnv         * aEnv )
{
    stlInt32                sReturn;
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadXaCloseCommand( sSclHandle,
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sReturn = sslXaClose( SSL_ENV(aEnv) );
    STL_TRY_THROW( sReturn == SSL_XA_OK, RAMP_FAIL_OPERATION );
                   
    STL_TRY( sclWriteXaResult( sSclHandle,
                               CML_COMMAND_XA_CLOSE,
                               sReturn,
                               KNL_ERROR_STACK(aEnv),
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sProtocolSentence->mCachedStmtId = -1;

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaErrorResult( sSclHandle,
                                    CML_COMMAND_XA_CLOSE,
                                    sReturn,
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sProtocolSentence->mCachedStmtId = -1;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdXaCommit( sllCommandArgs * aArgs,
                          sllEnv         * aEnv )
{
    stlXid      sXid;
    stlInt32    sRmId;
    stlInt64    sFlags;
    stlInt32    sReturn;
    stlInt16    sCommandType;
    sllHandle * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadXaCommand( sSclHandle,
                               &sCommandType,
                               &sXid,
                               &sRmId,
                               &sFlags,
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sReturn = sslXaCommit( &sXid,
                           sRmId,
                           sFlags,
                           SSL_ENV(aEnv) );
    STL_TRY_THROW( sReturn == SSL_XA_OK, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaResult( sSclHandle,
                               sCommandType,
                               sReturn,
                               KNL_ERROR_STACK(aEnv),
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaErrorResult( sSclHandle,
                                    sCommandType,
                                    sReturn,
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdXaRollback( sllCommandArgs * aArgs,
                            sllEnv         * aEnv )
{
    stlXid      sXid;
    stlInt32    sRmId;
    stlInt64    sFlags;
    stlInt32    sReturn;
    stlInt16    sCommandType;
    sllHandle * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadXaCommand( sSclHandle,
                               &sCommandType,
                               &sXid,
                               &sRmId,
                               &sFlags,
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sReturn = sslXaRollback( &sXid,
                             sRmId,
                             sFlags,
                             SSL_ENV(aEnv) );
    STL_TRY_THROW( sReturn == SSL_XA_OK, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaResult( sSclHandle,
                               sCommandType,
                               sReturn,
                               KNL_ERROR_STACK(aEnv),
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaErrorResult( sSclHandle,
                                    sCommandType,
                                    sReturn,
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdXaPrepare( sllCommandArgs * aArgs,
                           sllEnv         * aEnv )
{
    stlXid      sXid;
    stlInt32    sRmId;
    stlInt64    sFlags;
    stlInt32    sReturn;
    stlInt16    sCommandType;
    sllHandle * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadXaCommand( sSclHandle,
                               &sCommandType,
                               &sXid,
                               &sRmId,
                               &sFlags,
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sReturn = sslXaPrepare( &sXid,
                            sRmId,
                            sFlags,
                            SSL_ENV(aEnv) );
    STL_TRY_THROW( sReturn == SSL_XA_OK, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaResult( sSclHandle,
                               sCommandType,
                               sReturn,
                               KNL_ERROR_STACK(aEnv),
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaErrorResult( sSclHandle,
                                    sCommandType,
                                    sReturn,
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdXaStart( sllCommandArgs * aArgs,
                         sllEnv         * aEnv )
{
    stlXid      sXid;
    stlInt32    sRmId;
    stlInt64    sFlags;
    stlInt32    sReturn;
    stlInt16    sCommandType;
    sllHandle * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadXaCommand( sSclHandle,
                               &sCommandType,
                               &sXid,
                               &sRmId,
                               &sFlags,
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sReturn = sslXaStart( &sXid,
                          sRmId,
                          sFlags,
                          SSL_ENV(aEnv) );
    STL_TRY_THROW( sReturn == SSL_XA_OK, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaResult( sSclHandle,
                               sCommandType,
                               sReturn,
                               KNL_ERROR_STACK(aEnv),
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaErrorResult( sSclHandle,
                                    sCommandType,
                                    sReturn,
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdXaEnd( sllCommandArgs * aArgs,
                       sllEnv         * aEnv )
{
    stlXid      sXid;
    stlInt32    sRmId;
    stlInt64    sFlags;
    stlInt32    sReturn;
    stlInt16    sCommandType;
    sllHandle * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadXaCommand( sSclHandle,
                               &sCommandType,
                               &sXid,
                               &sRmId,
                               &sFlags,
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sReturn = sslXaEnd( &sXid,
                        sRmId,
                        sFlags,
                        SSL_ENV(aEnv) );
    STL_TRY_THROW( sReturn == SSL_XA_OK, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaResult( sSclHandle,
                               sCommandType,
                               sReturn,
                               KNL_ERROR_STACK(aEnv),
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaErrorResult( sSclHandle,
                                    sCommandType,
                                    sReturn,
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdXaForget( sllCommandArgs * aArgs,
                          sllEnv         * aEnv )
{
    stlXid      sXid;
    stlInt32    sRmId;
    stlInt64    sFlags;
    stlInt32    sReturn;
    stlInt16    sCommandType;
    sllHandle * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadXaCommand( sSclHandle,
                               &sCommandType,
                               &sXid,
                               &sRmId,
                               &sFlags,
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sReturn = sslXaForget( &sXid,
                           sRmId,
                           sFlags,
                           SSL_ENV(aEnv) );
    STL_TRY_THROW( sReturn == SSL_XA_OK, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaResult( sSclHandle,
                               sCommandType,
                               sReturn,
                               KNL_ERROR_STACK(aEnv),
                               SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_RAMP( RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaErrorResult( sSclHandle,
                                    sCommandType,
                                    sReturn,
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdXaRecover( sllCommandArgs * aArgs,
                           sllEnv         * aEnv )
{
    stlXid    * sXids;
    stlInt32    sRmId;
    stlInt64    sFlags;
    stlInt32    sReturn = SSL_XA_OK;
    stlInt64    sCount;
    stlInt32    sState = 0;
    sllHandle * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle * sSclHandle = SCL_HANDLE(sSllHandle);

    STL_TRY( sclReadXaRecoverCommand( sSclHandle,
                                      &sCount,
                                      &sRmId,
                                      &sFlags,
                                      SCL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( stlAllocHeap( (void**)&sXids,
                                 sCount * STL_SIZEOF(stlXid),
                                 KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                   RAMP_FAIL_ALLOC );
    sState = 1;

    sReturn = sslXaRecover( sXids,
                            sCount,
                            sRmId,
                            sFlags,
                            SSL_ENV(aEnv) );
    
    STL_TRY_THROW( sReturn >= 0, RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaRecoverResult( sSclHandle,
                                      sReturn,
                                      sXids,
                                      SSL_XA_OK,
                                      KNL_ERROR_STACK(aEnv),
                                      SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sState = 0;
    stlFreeHeap( sXids );

    return STL_SUCCESS;

    STL_CATCH( RAMP_FAIL_ALLOC )
    {
        sReturn = SSL_XAER_RMERR;
    }
    
    STL_RAMP( RAMP_FAIL_OPERATION );

    STL_TRY( sclWriteXaErrorResult( sSclHandle,
                                    CML_COMMAND_XA_RECOVER,
                                    sReturn,
                                    KNL_ERROR_STACK(aEnv),
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( sState == 1 )
    {
        stlFreeHeap( sXids );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        stlFreeHeap( sXids );
    }

    return STL_FAILURE;
}

stlStatus slpCmdPeriod( sllCommandArgs * aArgs,
                        sllEnv         * aEnv )
{
    sllHandle             * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle             * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence   * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclPeriodEnd( sSclHandle,
                           SCL_ENV(aEnv) )
             == STL_SUCCESS );

    sProtocolSentence->mCachedStmtId = -1;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus slpCmdControlPacket( sllCommandArgs * aArgs,
                               sllEnv         * aEnv )
{
    stlInt8               sControl = CML_CONTROL_NONE;
    stlInt8               sControlOp;
    sllHandle           * sSllHandle = SLL_COMMAND_ARG1(aArgs);
    sclHandle           * sSclHandle = SCL_HANDLE(sSllHandle);
    sllProtocolSentence * sProtocolSentence = SLL_PROTOCOL_SENTENCE(sSllHandle);

    STL_TRY( sclReadControlPacketCommand( sSclHandle,
                                          &sControl,
                                          &sControlOp,
                                          SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_HEAD(sControl) == STL_TRUE )
    {
        SLL_ADD_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    if( sControlOp == SLL_CONTROL_PACKET_OP_POOLING )
    {
        /* tcp가 아닌 ipc통신에서는 mPoolingFlag가 의미 없음으로 STL_TRUE로 설정 안함 */
        if( sSclHandle->mCommunication.mType == SCL_COMMUNICATION_TCP )
        {
            sProtocolSentence->mSclProtocolSentence.mPoolingFlag = STL_TRUE;
        }
    }

    STL_TRY( sclWriteControlPacketResult( sSclHandle,
                                          KNL_ERROR_STACK(aEnv),
                                          SCL_ENV(aEnv) )
             == STL_SUCCESS );

    if( SCL_IS_TAIL(sControl) == STL_TRUE )
    {
        SLL_REMOVE_PROTOCOLSET( SLL_PROTOCOL_STACK(sSllHandle) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_SUCCESS;
}
