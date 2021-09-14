/*******************************************************************************
 * ztpConvertExecSql.c
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
#include <dtl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztpDef.h>
#include <ztpMisc.h>
#include <ztpConvertExecSql.h>
#include <ztpCParseFunc.h>
#include <ztpcHostVar.h>
#include <ztpdType.h>
#include <ztpuFile.h>
#include <ztpwPrecomp.h>

/**
 * @file ztpConvertExecSql.c
 * @brief Parser Routines
 */

/**
 * @addtogroup ztpConvertExecSql
 * @{
 */

/*
 * ztpParseTreeType 정의와 mapping을 맞춰야 함
 */
ztpCvtSQLFunc gCvtSQLFunc[ZTP_PARSETREE_TYPE_MAX] =
{
    ztpConvertExecSqlTransaction,       /* ZTP_PARSETREE_TYPE_SQL_TRANSACTION */
    ztpConvertExecSqlBypass,            /* ZTP_PARSETREE_TYPE_SQL_BYPASS */
    ztpConvertExecSqlSetAutocommit,     /* ZTP_PARSETREE_TYPE_SET_AUTOCOMMIT */
    ztpConvertExecSqlDeclareCursor,     /* ZTP_PARSETREE_TYPE_DECLARE_CURSOR */
    ztpConvertExecSqlOpenCursor,        /* ZTP_PARSETREE_TYPE_OPEN_CURSOR */
    ztpConvertExecSqlFetchCursor,       /* ZTP_PARSETREE_TYPE_FETCH_CURSOR */
    ztpConvertExecSqlCloseCursor,       /* ZTP_PARSETREE_TYPE_CLOSE_CURSOR */
    ztpConvertExecSqlPositionedCursorDML,       /* ZTP_PARSETREE_TYPE_POSITIONED_CURSOR_DML */
    ztpConvertExecSqlIncludeToC,        /* ZTP_PARSETREE_TYPE_EXEC_SQL_INCLUDE */
    ztpConvertExecSqlException,         /* ZTP_PARSETREE_TYPE_SQL_EXCEPTION */
    ztpConvertExecSqlConnect,           /* ZTP_PARSETREE_TYPE_SQL_CONNECT */
    ztpConvertExecSqlDisconnect,        /* ZTP_PARSETREE_TYPE_SQL_DISCONNECT */
    ztpConvertExecSqlContextAllocate,   /* ZTP_PARSETREE_TYPE_CONTEXT_ALLOCATE */
    ztpConvertExecSqlContextFree,       /* ZTP_PARSETREE_TYPE_CONTEXT_FREE */
    ztpConvertExecSqlContextUse,        /* ZTP_PARSETREE_TYPE_CONTEXT_USE */
    ztpConvertExecSqlContextUseDefault, /* ZTP_PARSETREE_TYPE_CONTEXT_USE_DEFAULT */
    ztpConvertExecDynSqlExecuteImmediate,   /* ZTP_PARSETREE_TYPE_DYNAMIC_SQL_EXECUTE_IMMEDIATE */
    ztpConvertExecDynSqlPrepare,        /* ZTP_PARSETREE_TYPE_DYNAMIC_SQL_PREPARE */
    ztpConvertExecDynSqlExecute,        /* ZTP_PARSETREE_TYPE_DYNAMIC_SQL_EXECUTE */
    ztpConvertExecSqlIgnore,            /* ZTP_PARSETREE_TYPE_IGNORE */

};

stlStatus ztpConvertExecSqlIgnore( ztpCParseParam * aParam )
{
    return STL_SUCCESS;
}

stlStatus ztpConvertExecSqlTransaction(ztpCParseParam *aParam)
{
    ztpTransactionTree *sTransactionTree = (ztpTransactionTree *)aParam->mParseTree;
    stlChar             sLineBuffer[ZTP_LINE_BUF_SIZE];

    STL_TRY(ztpwPrecompBlockBegin(aParam) == STL_SUCCESS);

    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS);
    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             NULL,
                             sTransactionTree->mStatementType )
             == STL_SUCCESS);

    stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                "    GOLDILOCKSESQL_EndTran(%s, &sqlargs, %s, %s);\n",
                aParam->mCurrContext,
                sTransactionTree->mIsCommit == STL_TRUE ? "1" : "0",
                sTransactionTree->mIsRelease == STL_TRUE ? "1" : "0");
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    STL_TRY(ztpwPrecompException(aParam,
                                 STL_FALSE) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlBypass(ztpCParseParam *aParam)
{
    ztpBypassTree      *sBypassTree = (ztpBypassTree *)aParam->mParseTree;
    stlChar             sLineBuffer[ZTP_LINE_BUF_SIZE];
    stlBool             sApplyNotFound = STL_FALSE;

    STL_TRY( ztpwPrecompBlockBegin(aParam) == STL_SUCCESS );

    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS );
    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             sBypassTree->mSqlString,
                             sBypassTree->mStatementType )
             == STL_SUCCESS);

    if ( sBypassTree->mHostVarList != NULL )
    {
        STL_TRY( ztpwDeclareHostVariables( aParam,
                                           ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                           sBypassTree->mHostVarList )
                 == STL_SUCCESS );
        STL_TRY( ztpwPrecompHostVariable( aParam,
                                          ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                          sBypassTree->mHostVarList )
                 == STL_SUCCESS);
    }
    else
    {
        /* host variable 이 없는 경우 */
    }

    stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                "    GOLDILOCKSESQL_Execute(%s, &sqlargs);\n",
                aParam->mCurrContext);
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    if( sBypassTree->mStatementType == ZLPL_STMT_SELECT_INTO )
    {
        sApplyNotFound = STL_TRUE;
    }

    STL_TRY(ztpwPrecompException(aParam,
                                 sApplyNotFound) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlSetAutocommit(ztpCParseParam *aParam)
{
    ztpAutoCommit   *sAutoCommit = (ztpAutoCommit *)aParam->mParseTree;
    stlChar          sLineBuffer[ZTP_LINE_BUF_SIZE];

    STL_TRY(ztpwPrecompBlockBegin(aParam) == STL_SUCCESS);

    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS);
    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             NULL,
                             ZLPL_STMT_SET )
             == STL_SUCCESS);

    stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                "    GOLDILOCKSESQL_SetAutoCommit(%s, &sqlargs, %s);\n",
                aParam->mCurrContext,
                sAutoCommit->mAutocommitOn == STL_TRUE ? "1" : "0" );
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    STL_TRY(ztpwPrecompException(aParam,
                                 STL_FALSE) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlDeclareCursor( ztpCParseParam *aParam )
{
    ztpDeclCursor * sDeclCursor = (ztpDeclCursor *)aParam->mParseTree;
    ztpDeclCursor * sSymbolCursor = NULL;

    stlChar       * sCursorOrigin = NULL;
    stlInt32        sCursorOriginLength;

    stlChar         sLineBuffer[ZTP_LINE_BUF_SIZE];

    /***************************************************************
     * Syntax Validation
     ***************************************************************/

    /**
     * 기존에 Cursor 가 존재하는지 검사
     */

    ztpFindCursorInSymTab( sDeclCursor->mCursorName,
                           & sSymbolCursor );

    STL_TRY_THROW( sSymbolCursor == NULL, RAMP_ERR_REDEFINE_CURSOR );

    /**
     * Standing cursor일 경우 Cursor Origin을 설정
     */
    if( sDeclCursor->mOriginType == ZLPL_CURSOR_ORIGIN_STANDING_CURSOR )
    {
        sCursorOriginLength = stlStrlen( gSqlBuffer );

        STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                    sCursorOriginLength + 1,
                                    (void**)&sCursorOrigin,
                                    aParam->mErrorStack )
                 == STL_SUCCESS );
        stlMemset( sCursorOrigin, 0x00, sCursorOriginLength + 1 );
        stlStrncpy( sCursorOrigin,
                    gSqlBuffer + sDeclCursor->mCursorOriginOffset,
                    sCursorOriginLength + 1 );
        sDeclCursor->mCursorOrigin = sCursorOrigin;
    }

    /**
     * Cursor 를 Symbol Table 에 추가
     */

    STL_TRY(ztpAddCursorToSymTab( gRootSymTab, sDeclCursor ) == STL_SUCCESS);

    /***************************************************************
     * C code 작성
     ***************************************************************/

    /**
     * DECLARE cursor 를 위한 C code 를 생성
     */

    /* Write converted C code */
    STL_TRY( ztpwPrecompBlockBegin(aParam) == STL_SUCCESS );

    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS);

    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             NULL,
                             sDeclCursor->mType )
             == STL_SUCCESS);

    stlSnprintf( sLineBuffer, ZTP_LINE_BUF_SIZE,
                 "    GOLDILOCKSESQL_DeclareCursor(%s, &sqlargs);\n",
                 aParam->mCurrContext );
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    STL_TRY(ztpwPrecompException(aParam,
                                 STL_FALSE) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_REDEFINE_CURSOR )
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     ZTP_ERRCODE_CURSOR_NAME_ALREADY_DECLARED,
                     ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                          sDeclCursor->mCursorNamePos ),
                     aParam->mErrorStack,
                     sDeclCursor->mCursorName );
    }

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlOpenCursor(ztpCParseParam *aParam)
{
    ztpOpenCursor    * sOpenCursor = (ztpOpenCursor *)aParam->mParseTree;
    ztpDeclCursor    * sDeclCursor = NULL;

    stlChar            sLineBuffer[ZTP_LINE_BUF_SIZE];
    stlChar          * sCursorQuery = NULL;
    stlChar          * sStmtName    = NULL;

    /***************************************************************
     * Syntax Validation
     ***************************************************************/

    /**
     * Cursor 가 존재하는지 검사
     */

    ztpFindCursorInSymTab( sOpenCursor->mCursorName,
                           & sDeclCursor );

    STL_TRY_THROW( sDeclCursor != NULL, RAMP_ERR_SYMBOL_NOT_FOUND);

    /**
     * USING DESCRIPTOR :name 변수가 string 유형인지 검사
     */

    if ( sOpenCursor->mDynUsing != NULL )
    {
        /**
         * Cursor 가 Dynamic Cursor 인지 검사
         */

        STL_TRY_THROW( sDeclCursor->mOriginType == ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR,
                       RAMP_NOT_DYNAMIC_CURSOR );


        if ( (sOpenCursor->mDynUsing->mIsDesc == STL_TRUE) &&
             (sOpenCursor->mDynUsing->mDescName == NULL) )
        {
            /**
             * USING DESCRIPTOR :var
             */

            STL_TRY_THROW( ztpdIsHostStringType( sOpenCursor->mDynUsing->mHostVar ) == STL_TRUE,
                           RAMP_ERR_MISMATCH_USING_HOST_VARIABLE_TYPE );
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /* nothing to do */
    }

    /***************************************************************
     * C code 작성
     ***************************************************************/

    /**
     * OPEN cursor 를 위한 C code 를 생성
     */

    /* Write converted C code */
    STL_TRY( ztpwPrecompBlockBegin(aParam) == STL_SUCCESS );

    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS);

    /**
     * Cursor Query 와 Dynamic Statement Name 을 획득
     */

    switch ( sDeclCursor->mOriginType )
    {
        case ZLPL_CURSOR_ORIGIN_STANDING_CURSOR:
            sCursorQuery = sDeclCursor->mCursorOrigin;
            sStmtName = NULL;
            break;
        case ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR:
            sCursorQuery = NULL;
            sStmtName = sDeclCursor->mCursorOrigin;
            break;
        case ZLPL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR:
            sCursorQuery = NULL;
            sStmtName = NULL;
            break;
        case ZLPL_CURSOR_ORIGIN_RECEIVED_CURSOR:
            sCursorQuery = NULL;
            sStmtName = NULL;
            break;
        default:
            STL_DASSERT(0);
            break;
    }

    STL_TRY(ztpwSetSqlargs(aParam,
                           "&sqlca",
                           "SQLSTATE",
                           sCursorQuery,
                           sOpenCursor->mType)
            == STL_SUCCESS);

    STL_TRY(ztpwSetOpenCursor( aParam,
                               sDeclCursor->mCursorName,
                               sDeclCursor->mOriginType,
                               sStmtName,
                               sDeclCursor->mStandardType,
                               sDeclCursor->mSensitivity,
                               sDeclCursor->mScrollability,
                               sDeclCursor->mHoldability,
                               sDeclCursor->mUpdatability,
                               sDeclCursor->mReturnability )
            == STL_SUCCESS);

    if ( sDeclCursor->mHostVarList != NULL )
    {
        STL_TRY( ztpwDeclareHostVariables( aParam,
                                           ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                           sDeclCursor->mHostVarList )
                 == STL_SUCCESS );
        STL_TRY( ztpwPrecompHostVariable( aParam,
                                          ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                          sDeclCursor->mHostVarList )
                 == STL_SUCCESS);
    }
    else
    {
        /* host variable 이 없는 경우 */
    }

    /**
     * Dynamic USING parameter 작성
     */

    if ( sOpenCursor->mDynUsing != NULL )
    {
        /**
         * OPEN cursor_name USING ..
         */

        STL_TRY( ztpwDeclareDynamicParameter( aParam,
                                              STL_TRUE,  /* aIsUsing */
                                              ZTP_C_HOSTVAR_ARRAY_NAME_DYNAMIC_USING,
                                              sOpenCursor->mDynUsing )
                 == STL_SUCCESS );

        if ( sOpenCursor->mDynUsing->mHostVar != NULL )
        {
            /**
             * USING :sVar1, :sVar2
             * or
             * USING DESCRIPTOR :sVar
             */

            STL_TRY( ztpwPrecompHostVariable( aParam,
                                              ZTP_C_HOSTVAR_ARRAY_NAME_DYNAMIC_USING,
                                              sOpenCursor->mDynUsing->mHostVar )
                     == STL_SUCCESS);
        }
        else
        {
            /**
             * USING DESCRIPTOR 'desc_name'
             */
        }
    }
    else
    {
        /* nothing to do */
    }

    stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                "    GOLDILOCKSESQL_OpenCursor(%s, &sqlargs);\n",
                aParam->mCurrContext);
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    STL_TRY(ztpwPrecompException(aParam,
                                 STL_FALSE) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYMBOL_NOT_FOUND)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     ZTP_ERRCODE_NOT_DECLARED_CURSOR,
                     ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                          sOpenCursor->mCursorNamePos ),
                     aParam->mErrorStack,
                     sOpenCursor->mCursorName );
    }

    STL_CATCH( RAMP_NOT_DYNAMIC_CURSOR )
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     ZTP_ERRCODE_CURSOR_IS_NOT_A_DYNAMIC_CURSOR,
                     ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                          sOpenCursor->mCursorNamePos ),
                     aParam->mErrorStack,
                     sOpenCursor->mCursorName );
    }

    STL_CATCH( RAMP_ERR_MISMATCH_USING_HOST_VARIABLE_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_EXPRESSION_TYPE_DOES_NOT_MATCH_USAGE,
                      ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                           sOpenCursor->mDynUsing->mHostVar->mNamePos ),
                      aParam->mErrorStack );
    }

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlFetchCursor(ztpCParseParam *aParam)
{
    ztpFetchCursor   * sFetchCursor = (ztpFetchCursor *)aParam->mParseTree;
    ztpDeclCursor    * sDeclCursor = NULL;

    stlChar            sLineBuffer[ZTP_LINE_BUF_SIZE];
    stlInt64           sOffsetValue = 0;

    /************************************************************
     * Syntax Validation
     ************************************************************/

    /**
     * Cursor 가 존재하는지 검사
     */

    ztpFindCursorInSymTab( sFetchCursor->mCursorName,
                           & sDeclCursor );

    STL_TRY_THROW( sDeclCursor != NULL, RAMP_ERR_SYMBOL_NOT_FOUND);

    /**
     * Fetch Offset 이 Host Variable 인 경우 INTEGER 계열인지 검사
     */

    if ( sFetchCursor->mFetchOrientation != NULL )
    {
        if ( sFetchCursor->mFetchOrientation->mFetchOffsetHost != NULL )
        {
            /**
             * FETCH ABSOLUTE :sHostOffset
             * - Integer 계열의 C type 인지 검사
             */

            STL_TRY_THROW( ztpdIsHostIntegerType( sFetchCursor->mFetchOrientation->mFetchOffsetHost ) == STL_TRUE,
                           RAMP_ERR_MISMATCH_HOST_VARIABLE_TYPE );

        }
        else
        {
            /**
             * FETCH ABSOLUTE 1234
             * - INTEGER 로 변환 가능한 literal 인지 검사
             */

            STL_TRY_THROW( dtlGetIntegerFromString( sFetchCursor->mFetchOrientation->mFetchPos.mName,
                                                    stlStrlen(sFetchCursor->mFetchOrientation->mFetchPos.mName),
                                                    & sOffsetValue,
                                                    aParam->mErrorStack )
                           == STL_SUCCESS, RAMP_ERR_MISMATCH_INTEGER_TYPE );
        }
    }
    else
    {
        /* nothing to do */
    }

    /************************************************************
     * C Code 작성
     ************************************************************/

    /* Write converted C code */
    STL_TRY( ztpwPrecompBlockBegin(aParam) == STL_SUCCESS );
    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS);
    STL_TRY(ztpwSetSqlargs( aParam,
                            "&sqlca",
                            "SQLSTATE",
                            NULL,
                            sFetchCursor->mType )
            == STL_SUCCESS);

    STL_TRY(ztpwSetFetchCursor(aParam,
                               sFetchCursor->mCursorName,
                               sFetchCursor->mFetchOrientation)
            == STL_SUCCESS);

    STL_DASSERT( sFetchCursor->mHostVarList != NULL );
    STL_TRY( ztpwDeclareHostVariables( aParam,
                                       ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                       sFetchCursor->mHostVarList )
             == STL_SUCCESS );
    STL_TRY( ztpwPrecompHostVariable( aParam,
                                      ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                      sFetchCursor->mHostVarList )
             == STL_SUCCESS);

    stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                "    GOLDILOCKSESQL_FetchCursor(%s, &sqlargs);\n",
                aParam->mCurrContext);
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    STL_TRY(ztpwPrecompException(aParam,
                                 STL_TRUE) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYMBOL_NOT_FOUND)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     ZTP_ERRCODE_NOT_DECLARED_CURSOR,
                     ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                          sFetchCursor->mCursorNamePos ),
                     aParam->mErrorStack,
                     sFetchCursor->mCursorName );
    }

    STL_CATCH( RAMP_ERR_MISMATCH_HOST_VARIABLE_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_EXPRESSION_TYPE_DOES_NOT_MATCH_USAGE,
                      ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                           sFetchCursor->mFetchOrientation->mFetchOffsetHost->mNamePos ),
                      aParam->mErrorStack );
    }

    STL_CATCH( RAMP_ERR_MISMATCH_INTEGER_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_EXPRESSION_TYPE_DOES_NOT_MATCH_USAGE,
                      ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                           sFetchCursor->mFetchOrientation->mFetchPos.mNamePos ),
                      aParam->mErrorStack );
    }

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlCloseCursor(ztpCParseParam *aParam)
{
    ztpCloseCursor   * sCloseCursor = (ztpCloseCursor *)aParam->mParseTree;
    ztpDeclCursor    * sDeclCursor = NULL;

    stlChar            sLineBuffer[ZTP_LINE_BUF_SIZE];

    /************************************************************
     * Syntax Validation
     ************************************************************/

    /**
     * Cursor 가 존재하는지 검사
     */

    ztpFindCursorInSymTab( sCloseCursor->mCursorName,
                           & sDeclCursor );

    STL_TRY_THROW( sDeclCursor != NULL, RAMP_ERR_SYMBOL_NOT_FOUND);

    /************************************************************
     * C Code 작성
     ************************************************************/

    /* Write converted C code */
    STL_TRY( ztpwPrecompBlockBegin( aParam ) == STL_SUCCESS );

    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS );
    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             NULL,
                             sCloseCursor->mType )
             == STL_SUCCESS);

    STL_TRY( ztpwSetCloseCursor( aParam,
                                 sCloseCursor->mCursorName )
             == STL_SUCCESS);

    stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                "    GOLDILOCKSESQL_CloseCursor(%s, &sqlargs);\n",
                aParam->mCurrContext);
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    STL_TRY(ztpwPrecompException(aParam,
                                 STL_FALSE) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYMBOL_NOT_FOUND)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     ZTP_ERRCODE_NOT_DECLARED_CURSOR,
                     ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                          sCloseCursor->mCursorNamePos ),
                     aParam->mErrorStack,
                     sCloseCursor->mCursorName );
    }

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}



stlStatus ztpConvertExecSqlPositionedCursorDML( ztpCParseParam *aParam )
{
    ztpPositionedCursorDML  * sPosCursor = (ztpPositionedCursorDML *)aParam->mParseTree;

    ztpDeclCursor    * sDeclCursor = NULL;

    stlChar            sLineBuffer[ZTP_LINE_BUF_SIZE];

    /************************************************************
     * Syntax Validation
     ************************************************************/

    /**
     * Cursor 가 존재하는지 검사
     */

    ztpFindCursorInSymTab( sPosCursor->mCursorName,
                           & sDeclCursor );

    STL_TRY_THROW( sDeclCursor != NULL, RAMP_ERR_SYMBOL_NOT_FOUND);

    /************************************************************
     * C Code 작성
     ************************************************************/

    /* Write converted C code */
    STL_TRY( ztpwPrecompBlockBegin( aParam ) == STL_SUCCESS );

    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS );
    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             sPosCursor->mCursorDML,
                             sPosCursor->mType )
             == STL_SUCCESS);

    if ( sPosCursor->mHostVarList != NULL )
    {
        STL_TRY( ztpwDeclareHostVariables( aParam,
                                           ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                           sPosCursor->mHostVarList )
                 == STL_SUCCESS );
        STL_TRY( ztpwPrecompHostVariable( aParam,
                                          ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                          sPosCursor->mHostVarList )
                 == STL_SUCCESS);
    }
    else
    {
        /* host variable 이 없는 경우 */
    }

    STL_TRY( ztpwSetPositionedCursor( aParam,
                                      sPosCursor->mCursorName )
             == STL_SUCCESS);

    stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                "    GOLDILOCKSESQL_PositionedCursorDML(%s, &sqlargs);\n",
                aParam->mCurrContext);
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    STL_TRY(ztpwPrecompException(aParam,
                                 STL_FALSE) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYMBOL_NOT_FOUND)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     ZTP_ERRCODE_NOT_DECLARED_CURSOR,
                     ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                          sPosCursor->mCursorNamePos ),
                     aParam->mErrorStack,
                     sPosCursor->mCursorName );
    }

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlIncludeToC(ztpCParseParam *aParam)
{
    ztpParseTree       *sParseTree;
    ztpExecSqlInclude  *sExecSqlInclude = (ztpExecSqlInclude *)aParam->mParseTree;
    ztpIncludePath     *sIncludePath = &gIncludePath;
    ztpConvertContext   sContext;
    stlInt32            sState = 0;
    stlFile             sInFile;
    stlChar             sLineBuffer[ZTP_LINE_BUF_SIZE];
    stlChar             sFilePath[STL_MAX_FILE_PATH_LENGTH + 1];
    stlBool             sFileExist = STL_FALSE;

    /*
     * for Oracle compatibility
     *   SQLCA 를 include했을 경우에는 그냥 무시하도록 한다.
     */
    if( ( stlStrcasecmp( sExecSqlInclude->mFileName, "sqlca.h" ) == 0 )
        || ( stlStrcasecmp( sExecSqlInclude->mFileName, "sqlca" ) == 0 ) )
    {
        stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                    "#include \"sqlca.h\"\n");
        ztpwSendStringToOutFile(aParam, sLineBuffer);

        STL_THROW( RAMP_EXIT_SUCCESS );
    }

    sContext.mAllocator = aParam->mAllocator;
    sContext.mErrorStack = aParam->mErrorStack;
    sContext.mBuffer = NULL;
    sContext.mLength = 0;
    sContext.mInFile = NULL;
    sContext.mOutFile = aParam->mOutFile;
    sContext.mInFileName = sExecSqlInclude->mFileName;
    sContext.mOutFileName = aParam->mOutFileName;
    sContext.mParseTree = NULL;
    sContext.mNeedPrint = STL_TRUE;

    stlSnprintf( sFilePath, STL_MAX_FILE_PATH_LENGTH + 1,
                 "%s",
                 sExecSqlInclude->mFileName );

    if( stlExistFile( sFilePath,
                      &sFileExist,
                      sContext.mErrorStack )
        == STL_SUCCESS )
    {
        if( sFileExist == STL_TRUE )
        {
            STL_THROW( RAMP_FILE_CHECK );
        }
    }

    for( sIncludePath = &gIncludePath;
         sIncludePath != NULL;
         sIncludePath = sIncludePath->mNext )
    {
        stlSnprintf( sFilePath, STL_MAX_FILE_PATH_LENGTH + 1,
                     "%s%s%s",
                     sIncludePath->mIncludePath,
                     STL_PATH_SEPARATOR,
                     sExecSqlInclude->mFileName );

        if( stlExistFile( sFilePath,
                          &sFileExist,
                          sContext.mErrorStack )
            == STL_SUCCESS )
        {
            if( sFileExist == STL_TRUE )
            {
                STL_THROW( RAMP_FILE_CHECK );
            }
        }
    }

    STL_RAMP( RAMP_FILE_CHECK );

    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );

    STL_TRY(stlOpenFile(&sInFile,
                        sFilePath,
                        STL_FOPEN_READ,
                        STL_FPERM_OS_DEFAULT,
                        sContext.mErrorStack)
            == STL_SUCCESS);
    sContext.mInFile = &sInFile;
    sState = 1;

    STL_TRY(ztpuReadFile(sContext.mInFile,
                         sContext.mAllocator,
                         sContext.mErrorStack,
                         &sContext.mBuffer,
                         &sContext.mLength)
            == STL_SUCCESS);

    STL_TRY(ztpCParseIt(&sContext,
                        &sParseTree)
            == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    STL_RAMP( RAMP_EXIT_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      sContext.mErrorStack,
                      sExecSqlInclude->mFileName );
    }
    STL_FINISH;

    switch(sState)
    {
        case 1:
            stlCloseFile(sContext.mInFile,
                         sContext.mErrorStack);
            break;
        default:
            break;
    }

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlException(ztpCParseParam *aParam)
{
    ztpExecSqlException   *sException = (ztpExecSqlException *)aParam->mParseTree;
    ztpExceptionCondition *sCondition;

    STL_TRY(sException != NULL);
    sCondition = sException->mCondition;
    STL_TRY(sCondition != NULL);

    aParam->mException[sCondition->mType] = (ztpExecSqlException *)aParam->mParseTree;

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlConnect(ztpCParseParam *aParam)
{
    stlInt32            sState = 0;
    ztpConnectParam    *sConnParam = (ztpConnectParam *)aParam->mParseTree;
    stlChar             sLineBuffer[ZTP_LINE_BUF_SIZE];
    stlChar             sConnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar             sConnString[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar             sUserName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar             sPassword[STL_MAX_SQL_IDENTIFIER_LENGTH];

    if( sConnParam->mConnName != NULL )
    {
        STL_TRY(ztpMakeNameFromNameTag(aParam, sConnParam->mConnName, sConnName)
                == STL_SUCCESS);
        aParam->mConnStr = sConnName;
    }

    STL_TRY(ztpMakeNameFromNameTag(aParam, sConnParam->mConnString, sConnString)
            == STL_SUCCESS);
    STL_TRY(ztpMakeNameFromNameTag(aParam, sConnParam->mUserName, sUserName)
            == STL_SUCCESS);
    STL_TRY(ztpMakeNameFromNameTag(aParam, sConnParam->mPassword, sPassword)
            == STL_SUCCESS);

    STL_TRY( ztpwPrecompBlockBegin( aParam ) == STL_SUCCESS);

    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS);
    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             NULL,
                             ZLPL_STMT_MAX )
             == STL_SUCCESS);

    stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                "    GOLDILOCKSESQL_Connect(%s, &sqlargs, %s, %s, %s);\n",
                aParam->mCurrContext, sConnString, sUserName, sPassword);
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    STL_TRY(ztpwPrecompException(aParam,
                                 STL_FALSE) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    switch(sState)
    {
        case 1:
            break;
        default:
            break;
    }

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlDisconnect(ztpCParseParam *aParam)
{
    stlInt32            sState = 0;
    ztpDisconnectParam *sDisconnParam = (ztpDisconnectParam *)aParam->mParseTree;
    stlChar             sLineBuffer[ZTP_LINE_BUF_SIZE];
    stlChar             sConnObject[STL_MAX_SQL_IDENTIFIER_LENGTH];

    STL_TRY(ztpMakeNameFromNameTag(aParam, sDisconnParam->mConnectionObject, sConnObject)
            == STL_SUCCESS);

    STL_TRY( ztpwPrecompBlockBegin( aParam ) == STL_SUCCESS );
    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS);
    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             NULL,
                             ZLPL_STMT_MAX )
             == STL_SUCCESS);

    stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                "    GOLDILOCKSESQL_Disconnect(%s, &sqlargs, %s, %d);\n",
                aParam->mCurrContext, sConnObject, sDisconnParam->mIsAll);
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    STL_TRY(ztpwPrecompException(aParam,
                                 STL_FALSE) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    switch(sState)
    {
        case 1:
            break;
        default:
            break;
    }

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlContextAllocate(ztpCParseParam *aParam)
{
    /* 현재 미구현 */
    ztpSqlContext      *sSqlContext = (ztpSqlContext *)aParam->mParseTree;
    stlChar             sLineBuffer[ZTP_LINE_BUF_SIZE];

    STL_TRY( ztpwPrecompBlockBegin( aParam ) == STL_SUCCESS);
    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS);
    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             NULL,
                             ZLPL_STMT_MAX )
             == STL_SUCCESS);

    stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                "    GOLDILOCKSESQL_AllocContext((void **)&%s, &sqlargs);\n",
                sSqlContext->mName);
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    STL_TRY(ztpwPrecompException(aParam,
                                 STL_FALSE) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlContextFree(ztpCParseParam *aParam)
{
    /* 현재 미구현 */
    ztpSqlContext      *sSqlContext = (ztpSqlContext *)aParam->mParseTree;
    stlChar             sLineBuffer[ZTP_LINE_BUF_SIZE];

    STL_TRY( ztpwPrecompBlockBegin( aParam ) == STL_SUCCESS);
    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS );
    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             NULL,
                             ZLPL_STMT_MAX )
             == STL_SUCCESS);

    stlSnprintf(sLineBuffer, ZTP_LINE_BUF_SIZE,
                "    GOLDILOCKSESQL_FreeContext(&%s, &sqlargs);\n",
                sSqlContext->mName);
    ztpwSendStringToOutFile(aParam, sLineBuffer);

    STL_TRY(ztpwPrecompException(aParam,
                                 STL_FALSE) == STL_SUCCESS);
    STL_TRY(ztpwPrecompBlockEnd(aParam) == STL_SUCCESS);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlContextUse(ztpCParseParam *aParam)
{
    ztpSqlContext      *sSqlContext = (ztpSqlContext *)aParam->mParseTree;
    /* ztpHostVariable    *sHostVar = NULL; */
    /* stlChar             sErrorMsg[STL_MAX_ERROR_MESSAGE]; */

    /*
    STL_TRY_THROW(ztpFindHostVarInSymTab(sSqlContext->mName,
                                         &sHostVar)
                  == STL_SUCCESS, ERR_SYMBOL_NOT_FOUND);
    STL_TRY_THROW(sHostVar != NULL, ERR_SYMBOL_NOT_FOUND);
    */

    stlSnprintf(aParam->mCurrContext, STL_MAX_SQL_IDENTIFIER_LENGTH,
                "%s", sSqlContext->mName);
    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    /*
    STL_CATCH(ERR_SYMBOL_NOT_FOUND);
    {
        stlSnprintf(sErrorMsg, STL_MAX_ERROR_MESSAGE,
                    "Host variable \"%s\" not declared", sSqlContext->mName);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_NOT_DECLARED_HOST_VARIABLE,
                      sErrorMsg,
                      aParam->mErrorStack );
    }
    */
    STL_FINISH;
    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

stlStatus ztpConvertExecSqlContextUseDefault(ztpCParseParam *aParam)
{
    stlSnprintf(aParam->mCurrContext, STL_MAX_SQL_IDENTIFIER_LENGTH,
                "NULL");
    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}


/**
 * @brief EXECUTE IMMEDIATE 를 위한 C code 를 생성한다.
 * @param[in] aParam Parsed Parameter
 * @remarks
 */
stlStatus ztpConvertExecDynSqlExecuteImmediate( ztpCParseParam * aParam )
{
    ztpExecuteImmediate  * sParseTree = (ztpExecuteImmediate *)aParam->mParseTree;
    stlChar                sLineBuffer[ZTP_LINE_BUF_SIZE];

    /*************************************************************
     * Syntax Validation
     *************************************************************/

    /**
     * 비어있는 SQL 문인 경우 Pass
     */

    if(sParseTree->mHostVariable == NULL)
    {
        STL_TRY_THROW( sParseTree->mStringSQL != NULL, EXIT_FUNC );
    }

    /**
     * Host Name 인 경우 type 유효성 검사
     */

    if ( sParseTree->mHostVariable != NULL )
    {
        STL_TRY_THROW( ztpdIsHostStringType( sParseTree->mHostVariable ) == STL_TRUE,
                       RAMP_ERR_MISMATCH_HOST_VARIABLE_TYPE );
    }
    else
    {
        /* nothing to do */
    }

    /************************************************************
     * Write C Code
     ************************************************************/

    STL_TRY( ztpwPrecompBlockBegin( aParam ) == STL_SUCCESS );

    /**
     * sqlargs 작성
     */

    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS);

    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             sParseTree->mStringSQL,
                             sParseTree->mType )
             == STL_SUCCESS);

    /**
     * EXECUTE IMMEDIATE :sVariable 인 경우
     * Host Variable 의 Pre-Compile
     */

    if ( sParseTree->mHostVariable != NULL )
    {
        STL_TRY( ztpwDeclareHostVariables( aParam,
                                           ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                           sParseTree->mHostVariable )
                 == STL_SUCCESS );
        STL_TRY( ztpwPrecompHostVariable( aParam,
                                          ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                          sParseTree->mHostVariable )
                 == STL_SUCCESS);
    }
    else
    {
        /*
         * nothing to do
         */
    }

    /**
     * 호출 함수 작성
     */

    stlSnprintf( sLineBuffer,
                 ZTP_LINE_BUF_SIZE,
                 "    GOLDILOCKSESQL_DynamicExecuteImmediate(%s, &sqlargs);\n",
                 aParam->mCurrContext );
    ztpwSendStringToOutFile( aParam, sLineBuffer );

    /**
     * Exception Handling
     */

    STL_TRY( ztpwPrecompException( aParam,
                                   STL_FALSE ) == STL_SUCCESS );
    STL_TRY( ztpwPrecompBlockEnd( aParam ) == STL_SUCCESS );

    STL_RAMP(EXIT_FUNC);

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MISMATCH_HOST_VARIABLE_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_EXPRESSION_TYPE_DOES_NOT_MATCH_USAGE,
                      ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                           sParseTree->mHostVariable->mNamePos ),
                      aParam->mErrorStack );
    }

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}



/**
 * @brief PREPARE 를 위한 C code 를 생성한다.
 * @param[in] aParam Parsed Parameter
 * @remarks
 */
stlStatus ztpConvertExecDynSqlPrepare( ztpCParseParam * aParam )
{
    ztpPrepare  * sParseTree = (ztpPrepare *)aParam->mParseTree;
    stlChar       sLineBuffer[ZTP_LINE_BUF_SIZE];

    /*************************************************************
     * Syntax Validation
     *************************************************************/

    /**
     * 비어있는 SQL 문인 경우 Pass
     */

    if(sParseTree->mHostVariable == NULL)
    {
        STL_TRY_THROW( sParseTree->mStringSQL != NULL, EXIT_FUNC );
    }

    /**
     * Host Name 인 경우 type 유효성 검사
     */

    if ( sParseTree->mHostVariable != NULL )
    {
        STL_TRY_THROW( ztpdIsHostStringType( sParseTree->mHostVariable ) == STL_TRUE,
                       RAMP_ERR_MISMATCH_HOST_VARIABLE_TYPE );
    }
    else
    {
        /* nothing to do */
    }

    /*************************************************************
     * C code generation
     *************************************************************/

    STL_TRY( ztpwPrecompBlockBegin( aParam ) == STL_SUCCESS );

    /**
     * sqlargs 작성
     */

    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS );

    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             sParseTree->mStringSQL,
                             sParseTree->mType )
             == STL_SUCCESS);

    /**
     * PREPARE stmt_name FROM :sVariable 인 경우
     * Host Variable 의 Pree-Compile
     */

    if ( sParseTree->mHostVariable != NULL )
    {
        STL_TRY( ztpwDeclareHostVariables( aParam,
                                           ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                           sParseTree->mHostVariable )
                 == STL_SUCCESS );
        STL_TRY( ztpwPrecompHostVariable( aParam,
                                          ZTP_C_HOSTVAR_ARRAY_NAME_BASIC,
                                          sParseTree->mHostVariable )
                 == STL_SUCCESS);
    }
    else
    {
        /*
         * nothing to do
         */
    }

    /**
     * 호출 함수 작성
     */

    stlSnprintf( sLineBuffer,
                 ZTP_LINE_BUF_SIZE,
                 "    GOLDILOCKSESQL_DynamicPrepare( %s, &sqlargs, (char*)\"%s\" );\n",
                 aParam->mCurrContext,
                 sParseTree->mStmtName->mName );
    ztpwSendStringToOutFile( aParam, sLineBuffer );

    /**
     * Exception Handling
     */

    STL_TRY( ztpwPrecompException( aParam,
                                   STL_FALSE ) == STL_SUCCESS );
    STL_TRY( ztpwPrecompBlockEnd( aParam ) == STL_SUCCESS );

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    STL_RAMP(EXIT_FUNC);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MISMATCH_HOST_VARIABLE_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_EXPRESSION_TYPE_DOES_NOT_MATCH_USAGE,
                      ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                           sParseTree->mHostVariable->mNamePos ),
                      aParam->mErrorStack );
    }

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}


/**
 * @brief EXECUTE 를 위한 C code 를 생성한다.
 * @param[in] aParam Parsed Parameter
 * @remarks
 */
stlStatus ztpConvertExecDynSqlExecute( ztpCParseParam * aParam )
{
    ztpExecute  * sParseTree = (ztpExecute *)aParam->mParseTree;
    stlChar       sLineBuffer[ZTP_LINE_BUF_SIZE];

    /*************************************************************
     * Syntax Validation
     *************************************************************/

    /**
     * Symbol Table 에 statement Name 이 존재하는 지 검사
     * - pre-compile 단계에서 에러를 발생하지 않는다.
     * - run-time 단계에서 에러를 발생한다.
     */

    /**
     * USING DESCRIPTOR :name 변수가 string 유형인지 검사
     */

    if ( sParseTree->mDynUsing != NULL )
    {
        if ( (sParseTree->mDynUsing->mIsDesc == STL_TRUE) &&
             (sParseTree->mDynUsing->mDescName == NULL) )
        {
            /**
             * USING DESCRIPTOR :var
             */

            STL_TRY_THROW( ztpdIsHostStringType( sParseTree->mDynUsing->mHostVar ) == STL_TRUE,
                           RAMP_ERR_MISMATCH_USING_HOST_VARIABLE_TYPE );
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /* nothing to do */
    }

    /**
     * INTO DESCRIPTOR :name 변수가 string 유형인지 검사
     */

    if ( sParseTree->mDynInto != NULL )
    {
        if ( (sParseTree->mDynInto->mIsDesc == STL_TRUE) &&
             (sParseTree->mDynInto->mDescName == NULL) )
        {
            /**
             * INTO DESCRIPTOR :var
             */

            STL_TRY_THROW( ztpdIsHostStringType( sParseTree->mDynInto->mHostVar ) == STL_TRUE,
                           RAMP_ERR_MISMATCH_INTO_HOST_VARIABLE_TYPE );
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /* nothing to do */
    }


    /*************************************************************
     * C code generation
     *************************************************************/

    STL_TRY( ztpwPrecompBlockBegin( aParam ) == STL_SUCCESS );

    /**
     * sqlargs 작성
     */

    STL_TRY( ztpwDeclareSqlargs( aParam ) == STL_SUCCESS);

    STL_TRY( ztpwSetSqlargs( aParam,
                             "&sqlca",
                             "SQLSTATE",
                             NULL,
                             sParseTree->mType )
             == STL_SUCCESS);

    /**
     * Dynamic USING parameter 작성
     */

    if ( sParseTree->mDynUsing != NULL )
    {
        /**
         * USING ..
         */

        STL_TRY( ztpwDeclareDynamicParameter( aParam,
                                              STL_TRUE,  /* aIsUsing */
                                              ZTP_C_HOSTVAR_ARRAY_NAME_DYNAMIC_USING,
                                              sParseTree->mDynUsing )
                 == STL_SUCCESS );

        if ( sParseTree->mDynUsing->mHostVar != NULL )
        {
            /**
             * USING :sVar1, :sVar2
             * or
             * USING DESCRIPTOR :sVar
             */

            STL_TRY( ztpwPrecompHostVariable( aParam,
                                              ZTP_C_HOSTVAR_ARRAY_NAME_DYNAMIC_USING,
                                              sParseTree->mDynUsing->mHostVar )
                     == STL_SUCCESS);
        }
        else
        {
            /**
             * USING DESCRIPTOR 'desc_name'
             */
        }
    }
    else
    {
        /* nothing to do */
    }

    /**
     * Dynamic INTO parameter 작성
     */

    if ( sParseTree->mDynInto != NULL )
    {
        /**
         * INTO ..
         */

        STL_TRY( ztpwDeclareDynamicParameter( aParam,
                                              STL_FALSE,  /* aIsUsing */
                                              ZTP_C_HOSTVAR_ARRAY_NAME_DYNAMIC_INTO,
                                              sParseTree->mDynInto )
                 == STL_SUCCESS );

        if ( sParseTree->mDynInto->mHostVar != NULL )
        {
            /**
             * INTO :sVar1, :sVar2
             * or
             * INTO DESCRIPTOR :sVar
             */

            STL_TRY( ztpwPrecompHostVariable( aParam,
                                              ZTP_C_HOSTVAR_ARRAY_NAME_DYNAMIC_INTO,
                                              sParseTree->mDynInto->mHostVar )
                     == STL_SUCCESS);
        }
        else
        {
            /**
             * INTO DESCRIPTOR 'desc_name'
             */
        }
    }
    else
    {
        /* nothing to do */
    }

    /**
     * 호출 함수 작성
     */

    stlSnprintf( sLineBuffer,
                 ZTP_LINE_BUF_SIZE,
                 "    GOLDILOCKSESQL_DynamicExecute( %s, &sqlargs, (char*)\"%s\" );\n",
                 aParam->mCurrContext,
                 sParseTree->mStmtName->mName );
    ztpwSendStringToOutFile( aParam, sLineBuffer );

    /**
     * Exception Handling
     */

    STL_TRY( ztpwPrecompException( aParam,
                                   STL_FALSE ) == STL_SUCCESS );
    STL_TRY( ztpwPrecompBlockEnd( aParam ) == STL_SUCCESS );

    STL_TRY(aParam->mErrorStatus == STL_SUCCESS);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MISMATCH_USING_HOST_VARIABLE_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_EXPRESSION_TYPE_DOES_NOT_MATCH_USAGE,
                      ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                           sParseTree->mDynUsing->mHostVar->mNamePos ),
                      aParam->mErrorStack );
    }

    STL_CATCH( RAMP_ERR_MISMATCH_INTO_HOST_VARIABLE_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_EXPRESSION_TYPE_DOES_NOT_MATCH_USAGE,
                      ztpMakeErrPosString( & aParam->mBuffer[aParam->mSQLStartPos],
                                           sParseTree->mDynInto->mHostVar->mNamePos ),
                      aParam->mErrorStack );
    }

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return STL_FAILURE;
}

/** @} */

