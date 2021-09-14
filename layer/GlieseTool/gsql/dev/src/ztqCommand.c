/*******************************************************************************
 * ztqCommand.c
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
#include <ztqDef.h>
#include <ztqCommand.h>
#include <ztqDisplay.h>
#include <ztqOdbcBridge.h>
#include <ztqHostVariable.h>
#include <ztqSqlLex.h>
#include <ztqSqlParser.h>
#include <ztqDesc.h>
#include <ztqExecute.h>
#include <ztqQueryHistory.h>
#include <ztqNamedStmt.h>
#include <ztqNamedCursor.h>
#include <ztqMain.h>
#include <ztqPrintDDL.h>
#include <ztqEditCommand.h>

#define ZTQ_AS_OPTION_SIZE (24)

extern stlInt32 gZtqPageSize;
extern stlInt32 gZtqColSize;
extern stlInt32 gZtqNumSize;
extern stlInt32 gZtqDdlSize;
extern stlBool  gZtqTiming;
extern stlBool  gZtqVerbose;
extern stlInt32 gZtqFinish;
extern stlBool  gZtqPrintCallstack;
extern stlBool  gZtqPrintResult;
extern stlBool  gZtqPrintErrorMessage;
extern stlBool  gZtqVerticalColumnHeaderOrder;
extern stlInt32 gZtqLineSize;
extern stlTime  gZtqBeginTime;
extern stlTime  gZtqEndTime;
extern stlBool  gZtqConnectionEstablished;
extern stlBool  gZtqAdmin;
extern stlBool  gZtqSystemAdmin;
extern stlChar  gZtqUserName[];
extern stlChar  gZtqPassword[];
extern stlChar  gZtqDsn[];

extern SQLHDBC   gZtqDbcHandle;
extern SQLHENV   gZtqEnvHandle;
extern stlVarInt gZtqOdbcVersion;

extern stlChar gZtqConnString[];

ztqSqlStmt * gZtqSqlStmt = NULL;

ztqCmdFunc gZtqCmdFuncs[ZTQ_COMMAND_TYPE_MAX] =
{
    NULL,
    NULL,
    ztqCmdExecSql,
    ztqCmdPrepareSql,
    ztqCmdDynamicSql,
    ztqCmdExecPrepared,
    ztqCmdExecHistory,
    ztqCmdExecAssign,
    
    ztqCmdSetCallstack,
    ztqCmdSetVertical,
    ztqCmdSetTiming,
    ztqCmdSetVerbose,
    ztqCmdSetLinesize,
    ztqCmdSetPagesize,
    ztqCmdSetColsize,
    ztqCmdSetNumsize,
    ztqCmdSetDdlsize,
    ztqCmdSetAutocommit,
    ztqCmdSetResult,
    ztqCmdSetHistory,
    ztqCmdSetColor,
    ztqCmdSetError,
    
    ztqCmdQuit,
    ztqCmdHelp,
    ztqCmdPrint,
    ztqCmdDesc,
    ztqCmdIdesc,
    ztqCmdVar,
    ztqCmdImport,
    ztqCmdPrintHistory,
    ztqCmdExplainPlan,
    ztqCmdAllocStmt,
    ztqCmdFreeStmt,
    ztqCmdUseStmtExecSql,
    ztqCmdUseStmtPrepareSql,
    ztqCmdUseStmtExecPrepared,
    
    ztqCmdStartup,
    ztqCmdShutdown,
    ztqCmdConnect,
    
    ztqCmdXaOpen,
    ztqCmdXaClose,
    ztqCmdXaStart,
    ztqCmdXaEnd,
    ztqCmdXaCommit,
    ztqCmdXaRollback,
    ztqCmdXaPrepare,
    ztqCmdXaRecover,
    ztqCmdXaForget,

    ztqCmdPrintDatabaseDDL,
    ztqCmdPrintSpaceDDL,
    ztqCmdPrintProfileDDL,
    ztqCmdPrintAuthDDL,
    ztqCmdPrintSchemaDDL,
    ztqCmdPrintPublicSynonymDDL,
    ztqCmdPrintTableDDL,
    ztqCmdPrintViewDDL,
    ztqCmdPrintSequenceDDL,
    ztqCmdPrintIndexDDL,
    ztqCmdPrintConstraintDDL,
    ztqCmdPrintSynonymDDL,

    ztqCmdEdit,
    ztqCmdSpool
};

/**
 * @file ztqCommand.c
 * @brief Command Routines
 */

/**
 * @addtogroup ztqCommand
 * @{
 */

/**
 * @brief '\\QUIT' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 프로그램을 종료 시킨다.
 */
stlStatus ztqCmdQuit( ztqParseTree  * aParseTree,
                      stlChar       * aCmdStr,
                      stlAllocator  * aAllocator,
                      stlErrorStack * aErrorStack )
{
    gZtqFinish = 1;
    ztqSpoolEnd( NULL, ZTQ_SPOOL_FLAG_END, NULL, aErrorStack );
    ztqFreeSpoolBuffer();

    return STL_SUCCESS;
}

/**
 * @brief '\\HELP' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks HELP 메세지를 출력한다.
 */
stlStatus ztqCmdHelp( ztqParseTree  * aParseTree,
                      stlChar       * aCmdStr,
                      stlAllocator  * aAllocator,
                      stlErrorStack * aErrorStack )
{
    STL_TRY( ztqPrintCommandHelp( aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\DESC [SCHEMA.]TABLE' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 테이블의 SCHEMA 정보를 출력한다.
 */
stlStatus ztqCmdDesc( ztqParseTree  * aParseTree,
                      stlChar       * aCmdStr,
                      stlAllocator  * aAllocator,
                      stlErrorStack * aErrorStack )
{
    ztqCmdDescTree * sParseTree;

    sParseTree = (ztqCmdDescTree*)aParseTree;

    STL_TRY( ztqExecuteTableDesc( sParseTree->mIdentifierList,
                                  aAllocator,
                                  aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\IDESC [SCHEMA.]INDEX' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 인덱스의 SCHEMA 정보를 출력한다.
 */
stlStatus ztqCmdIdesc( ztqParseTree  * aParseTree,
                       stlChar       * aCmdStr,
                       stlAllocator  * aAllocator,
                       stlErrorStack * aErrorStack )
{
    ztqCmdIdescTree * sParseTree;

    sParseTree = (ztqCmdIdescTree*)aParseTree;

    STL_TRY( ztqExecuteIndexDesc( sParseTree->mIdentifierList,
                                  aAllocator,
                                  aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\CALLSTACK { ON | OFF }' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 에러 발생시 콜스택을 에러 메세지로 출력한다.
 */
stlStatus ztqCmdSetCallstack( ztqParseTree  * aParseTree,
                              stlChar       * aCmdStr,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    ztqCmdSetCallstackTree * sParseTree;
    stlChar                * sSqlStr;
    stlChar                * sOnSqlStr = "ALTER SESSION SET SHOW_CALLSTACK = ON";
    stlChar                * sOffSqlStr = "ALTER SESSION SET SHOW_CALLSTACK = OFF";
    ztqParseTree           * sSqlParseTree;

    /**
     * Client-side Callstack 정보를 변경한다.
     */
    sParseTree = (ztqCmdSetCallstackTree*)aParseTree;
    gZtqPrintCallstack = sParseTree->mOn;

    /**
     * Server-side Callstack 정보를 변경한다.
     */

    if( sParseTree->mOn == STL_TRUE )
    {
        sSqlStr = sOnSqlStr;
    }
    else
    {
        sSqlStr = sOffSqlStr;
    }

    STL_TRY( ztqSqlParseIt( sSqlStr,
                            aAllocator,
                            &sSqlParseTree,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqPrepareExecuteSql( sSqlStr,
                                   aAllocator,
                                   sSqlParseTree,
                                   STL_FALSE,  /* aPrintResult */
                                   aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\SET VERTICAL { ON | OFF }' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 검색 결과에 대한 컬럼 순서를 변경한다.(수직적 또는 수평적)
 */
stlStatus ztqCmdSetVertical( ztqParseTree  * aParseTree,
                             stlChar       * aCmdStr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    ztqCmdSetVerticalTree * sParseTree;

    sParseTree = (ztqCmdSetVerticalTree*)aParseTree;
    gZtqVerticalColumnHeaderOrder = sParseTree->mOn;

    return STL_SUCCESS;
}

/**
 * @brief '\\SET TIMING { ON | OFF }' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks SQL 수행 시간을 출력한다.
 */
stlStatus ztqCmdSetTiming( ztqParseTree  * aParseTree,
                           stlChar       * aCmdStr,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack )
{
    ztqCmdSetTimingTree * sParseTree;

    sParseTree = (ztqCmdSetTimingTree*)aParseTree;
    gZtqTiming = sParseTree->mOn;

    return STL_SUCCESS;
}

/**
 * @brief '\\SET VERBOSE { ON | OFF }' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 추가적인 메세지를 출력한다.
 */
stlStatus ztqCmdSetVerbose( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    ztqCmdSetVerboseTree * sParseTree;

    sParseTree = (ztqCmdSetVerboseTree*)aParseTree;
    gZtqVerbose = sParseTree->mOn;

    return STL_SUCCESS;
}

/**
 * @brief '\\SET AUTOCOMMIT { ON | OFF }' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 트랜잭션의 AUTOCOMMIT 모드를 변경한다.
 */
stlStatus ztqCmdSetAutocommit( ztqParseTree  * aParseTree,
                               stlChar       * aCmdStr,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    ztqCmdSetAutocommitTree * sParseTree;

    sParseTree = (ztqCmdSetAutocommitTree*)aParseTree;

    STL_TRY( ztqExecuteAutocommit( sParseTree->mOn,
                                   aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\SET RESULT { ON | OFF }' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks QUERY 결과에 대한 출격을 제어한다.
 */
stlStatus ztqCmdSetResult( ztqParseTree  * aParseTree,
                           stlChar       * aCmdStr,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack )
{
    ztqCmdSetResultTree * sParseTree;

    sParseTree = (ztqCmdSetResultTree*)aParseTree;
    gZtqPrintResult = sParseTree->mOn;

    return STL_SUCCESS;
}

/**
 * @brief '\\SET COLOR { ON | OFF }' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks SQL 구문 결과를 컬러와 함께 출력한다.
 */
stlStatus ztqCmdSetColor( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack )
{
    ztqCmdSetColorTree * sParseTree;

    sParseTree = (ztqCmdSetColorTree*)aParseTree;

    if( sParseTree->mOn == STL_TRUE )
    {
        STL_TRY( ztqEnableColoredText( aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqDisableColoredText( aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\SET ERROR { ON | OFF }' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks SQL 구문 에러시 에러 메세지(에러 코드는 제외) 출력을 제어한다.
 */
stlStatus ztqCmdSetError( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack )
{
    ztqCmdSetColorTree * sParseTree;

    sParseTree = (ztqCmdSetColorTree*)aParseTree;

    if( sParseTree->mOn == STL_TRUE )
    {
        gZtqPrintErrorMessage = STL_TRUE;
    }
    else
    {
        gZtqPrintErrorMessage = STL_FALSE;
    }

    return STL_SUCCESS;
}

/**
 * @brief '\\SET LINESIZE {n}' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 한 레코드가 가지는 최대 길이를 설정한다.
 */
stlStatus ztqCmdSetLinesize( ztqParseTree  * aParseTree,
                             stlChar       * aCmdStr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    ztqCmdSetLinesizeTree * sParseTree;

    sParseTree = (ztqCmdSetLinesizeTree*)aParseTree;

    STL_TRY_THROW( (sParseTree->mLineSize >= ZTQ_MIN_LINE_SIZE) &&
                   (sParseTree->mLineSize <= ZTQ_MAX_LINE_SIZE),
                   RAMP_ERR_VALUE );
    
    gZtqLineSize = sParseTree->mLineSize;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_LINESIZE_VALUE,
                      NULL,
                      aErrorStack,
                      ZTQ_MIN_LINE_SIZE,
                      ZTQ_MAX_LINE_SIZE );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\SET COLSIZE {n}' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 한 컬럼이 가지는 최대 길이를 설정한다.
 */
stlStatus ztqCmdSetColsize( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    ztqCmdSetColsizeTree * sParseTree;

    sParseTree = (ztqCmdSetColsizeTree*)aParseTree;

    STL_TRY_THROW( (sParseTree->mColSize >= ZTQ_MIN_COL_SIZE) &&
                   (sParseTree->mColSize <= ZTQ_MAX_COL_SIZE),
                   RAMP_ERR_VALUE );
    
    gZtqColSize = sParseTree->mColSize;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_COLSIZE_VALUE,
                      NULL,
                      aErrorStack,
                      ZTQ_MIN_COL_SIZE,
                      ZTQ_MAX_COL_SIZE );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\SET NUMSIZE {n}' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 한 컬럼이 가지는 최대 길이를 설정한다.
 */
stlStatus ztqCmdSetNumsize( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    ztqCmdSetNumsizeTree * sParseTree;

    sParseTree = (ztqCmdSetNumsizeTree*)aParseTree;

    STL_TRY_THROW( (sParseTree->mNumSize >= ZTQ_MIN_NUM_SIZE) &&
                   (sParseTree->mNumSize <= ZTQ_MAX_NUM_SIZE),
                   RAMP_ERR_VALUE );
    
    gZtqNumSize = sParseTree->mNumSize;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_NUMSIZE_VALUE,
                      NULL,
                      aErrorStack,
                      ZTQ_MIN_NUM_SIZE,
                      ZTQ_MAX_NUM_SIZE );
    }

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief '\\SET DDLSIZE {n}' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 한 컬럼이 가지는 최대 길이를 설정한다.
 */
stlStatus ztqCmdSetDdlsize( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    ztqCmdSetDdlsizeTree * sParseTree;

    sParseTree = (ztqCmdSetDdlsizeTree*)aParseTree;

    STL_TRY_THROW( (sParseTree->mDdlSize >= ZTQ_MIN_DDL_SIZE) &&
                   (sParseTree->mDdlSize <= ZTQ_MAX_DDL_SIZE),
                   RAMP_ERR_VALUE );
    
    gZtqDdlSize = sParseTree->mDdlSize;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_DDLSIZE_VALUE,
                      NULL,
                      aErrorStack,
                      ZTQ_MIN_DDL_SIZE,
                      ZTQ_MAX_DDL_SIZE );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\SET PAGESIZE {n}' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 한 페이지가 가지는 최대 레코드 개수를 설정한다.
 */
stlStatus ztqCmdSetPagesize( ztqParseTree  * aParseTree,
                             stlChar       * aCmdStr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    ztqCmdSetPagesizeTree * sParseTree;

    sParseTree = (ztqCmdSetPagesizeTree*)aParseTree;

    STL_TRY_THROW( (sParseTree->mPageSize >= ZTQ_MIN_PAGE_SIZE) &&
                   (sParseTree->mPageSize <= ZTQ_MAX_PAGE_SIZE),
                   RAMP_ERR_VALUE );
    
    gZtqPageSize = sParseTree->mPageSize;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_PAGESIZE_VALUE,
                      NULL,
                      aErrorStack,
                      ZTQ_MIN_PAGE_SIZE,
                      ZTQ_MAX_PAGE_SIZE );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\SET HOSTORY {n}' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks HISTORY 버퍼에 저장할수 있는 SQL의 최대 개수를 설정한다.
 */
stlStatus ztqCmdSetHistory( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    ztqCmdSetHistoryTree * sParseTree;

    sParseTree = (ztqCmdSetHistoryTree*)aParseTree;

    if( sParseTree->mHistorySize < 0 )
    {
        STL_TRY( ztqClearHistoryBuffer( aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY_THROW( (sParseTree->mHistorySize >= ZTQ_MIN_HISTORY_BUFFER_SIZE) &&
                       (sParseTree->mHistorySize <= ZTQ_MAX_HISTORY_BUFFER_SIZE),
                       RAMP_ERR_VALUE );
    
        STL_TRY( ztqSetHistoryBufferSize( sParseTree->mHistorySize,
                                          aErrorStack )
                 != STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_HISTORY_VALUE,
                      NULL,
                      aErrorStack,
                      ZTQ_MAX_HISTORY_BUFFER_SIZE );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\VAR host_variable type' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 호스트 변수를 선언한다.
 */
stlStatus ztqCmdVar( ztqParseTree  * aParseTree,
                     stlChar       * aCmdStr,
                     stlAllocator  * aAllocator,
                     stlErrorStack * aErrorStack )
{
    ztqCmdDeclareVarTree * sParseTree;

    sParseTree = (ztqCmdDeclareVarTree*)aParseTree;

    STL_TRY( ztqDeclareHostVariable( sParseTree->mName,
                                     sParseTree->mDataType,
                                     aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\PRINT [host_variable]' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 호스트 변수의 값을 출력한다.
 */
stlStatus ztqCmdPrint( ztqParseTree  * aParseTree,
                       stlChar       * aCmdStr,
                       stlAllocator  * aAllocator,
                       stlErrorStack * aErrorStack )
{
    ztqCmdPrintTree * sParseTree;

    sParseTree = (ztqCmdPrintTree*)aParseTree;

    if( stlStrlen( sParseTree->mName ) == 0 )
    {
        STL_TRY( ztqPrintAllHostVariable( aAllocator,
                                          aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqPrintHostVariable( sParseTree->mName,
                                       aAllocator,
                                       aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\EXEC :host_variable := expression' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 호스트 변수의 값을 설정한다.
 */
stlStatus ztqCmdExecAssign( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    ztqCmdExecAssignTree * sParseTree;

    sParseTree = (ztqCmdExecAssignTree*)aParseTree;

    STL_TRY( ztqAssignHostVariable( sParseTree,
                                    aCmdStr,
                                    aAllocator,
                                    aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\EXEC SQL sql_statement' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks SQL 문장을 수행한다.
 */
stlStatus ztqCmdExecSql( ztqParseTree  * aParseTree,
                         stlChar       * aCmdStr,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack )
{
    ztqCmdExecSqlTree * sParseTree;
    ztqBypassTree     * sSqlTree;
    stlChar           * sSqlString;

    sParseTree = (ztqCmdExecSqlTree*)aParseTree;

    /*
     * Command String에서 SQL String을 추출한다.
     * "\execute select * from ..." ==> "select * from ..."
     */
    sSqlString = aCmdStr + sParseTree->mSqlStartPos;

    /*
     * White space 제거
     */
    while( stlIsspace( *sSqlString ) )
    {
        sSqlString++;
    }

    STL_TRY( ztqSqlParseIt( sSqlString,
                            aAllocator,
                            (ztqParseTree**)&sSqlTree,
                            aErrorStack )
             == STL_SUCCESS );

    /*
     * Empty SQL은 skip한다.
     */
    STL_TRY_THROW( sSqlTree != ZTQ_EMPTY_PARSE_TREE, RAMP_SKIP );

    STL_TRY( ztqPrepareExecuteSql( sSqlString,
                                   aAllocator,
                                   (ztqParseTree*)sSqlTree,
                                   STL_TRUE,   /* aPrintResult */
                                   aErrorStack )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\PREPARE SQL sql_statement' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks SQL 문장에 대해서 PREPARE 과정을 진행한다.
 */
stlStatus ztqCmdPrepareSql( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    ztqCmdExecSqlTree * sParseTree;
    ztqBypassTree     * sSqlTree;
    stlChar           * sSqlString;
    SQLHSTMT            sOdbcStmt = NULL;
    stlInt32            sState = 0;
    ztqSqlStmt        * sZtqSqlStmt = NULL;

    sParseTree = (ztqCmdExecSqlTree*)aParseTree;

    /*
     * Command String에서 SQL String을 추출한다.
     * "\execute select * from ..." ==> " select * from ..."
     */
    sSqlString = aCmdStr + sParseTree->mSqlStartPos;

    /*
     * White space 제거
     */
    while( stlIsspace( *sSqlString ) )
    {
        sSqlString++;
    }

    STL_TRY( ztqSqlParseIt( sSqlString,
                            aAllocator,
                            (ztqParseTree**)&sSqlTree,
                            aErrorStack )
             == STL_SUCCESS );

    /*
     * Empty SQL은 skip한다.
     */
    STL_TRY_THROW( sSqlTree != ZTQ_EMPTY_PARSE_TREE, RAMP_SKIP );

    ztqClearElapsedTime();

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sOdbcStmt,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    STL_TRY( ztqAllocSqlStmt( &sZtqSqlStmt,
                              (ztqParseTree*)sSqlTree,
                              sOdbcStmt,
                              NULL,        /* aAllocator */
                              aErrorStack )
             == STL_SUCCESS );
    sState = 2;

    ZTQ_TIME_NOW( gZtqBeginTime );

    STL_TRY( ztqPrepareSql( sSqlString,
                            aAllocator,
                            sZtqSqlStmt,
                            aErrorStack )
             == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqEndTime );

    STL_TRY( ztqDestPreparedStmt( aErrorStack ) == STL_SUCCESS );

    gZtqSqlStmt = sZtqSqlStmt;

    STL_TRY( ztqPrintPreparedAnswerMessage( aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztqPrintElapsedTime( aErrorStack ) == STL_SUCCESS );

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            ZTQ_TIME_NOW( gZtqEndTime );

            (void) ztqPrintElapsedTime( aErrorStack );

            (void)ztqFreeSqlStmt( sZtqSqlStmt,
                                  aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sOdbcStmt,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief '\\DYNAMIC SQL :v1' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks SQL 문장에 대해서 PREPARE 과정을 진행한다.
 */
stlStatus ztqCmdDynamicSql( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    ztqCmdDynamicSqlTree * sParseTree;
    ztqParseTree         * sSqlTree;
    stlChar              * sSqlString;
    ztqHostVariable      * sHostVariable;

    sParseTree = (ztqCmdDynamicSqlTree*)aParseTree;

    /*
     * Host variable 에서 SQL String을 추출한다.
     */

    STL_TRY( ztqGetHostVariable( sParseTree->mVariableName,
                                 &sHostVariable,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( (sHostVariable->mDataType == SQL_CHAR)    ||
                   (sHostVariable->mDataType == SQL_VARCHAR) ||
                   (sHostVariable->mDataType == SQL_LONGVARCHAR),
                   RAMP_ERR_INVALID_HOST_TYPE );
                   
    STL_TRY_THROW( sHostVariable->mIndicator != SQL_NULL_DATA,
                   RAMP_ERR_NULL_SQL );
    
    sSqlString = sHostVariable->mValue;

    /*
     * White space 제거
     */
    while( stlIsspace( *sSqlString ) )
    {
        sSqlString++;
    }

    STL_TRY( ztqSqlParseIt( sSqlString,
                            aAllocator,
                            &sSqlTree,
                            aErrorStack )
             == STL_SUCCESS );

    /*
     * Empty SQL은 skip한다.
     */
    STL_TRY_THROW( sSqlTree != ZTQ_EMPTY_PARSE_TREE, RAMP_SKIP );

    STL_TRY( ztqDirectExecuteSql( sSqlString,
                                  aAllocator,
                                  sSqlTree,
                                  STL_TRUE,  /* aPrintResult */
                                  aErrorStack )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NULL_SQL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NULL_SQL_STRING,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HOST_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_HOST_TYPE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\EXEC' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks PREPARED SQL 문장에 대해서 EXECUTE 과정을 진행한다.
 */
stlStatus ztqCmdExecPrepared( ztqParseTree  * aParseTree,
                              stlChar       * aCmdStr,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    ztqClearElapsedTime();

    ZTQ_TIME_NOW( gZtqBeginTime );

    STL_TRY( ztqExecuteSql( NULL,
                            aAllocator,
                            gZtqSqlStmt,
                            STL_FALSE,  /* aDirectExecute */
                            STL_TRUE,   /* aPrintResult */
                            aErrorStack )
             == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqEndTime );

    STL_TRY( ztqPrintElapsedTime( aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    ZTQ_TIME_NOW( gZtqEndTime );

    (void) ztqPrintElapsedTime( aErrorStack );

    return STL_FAILURE;
}

/**
 * @brief '\\{n}' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks n'th HISTORY 엔트리를 수행한다.
 */
stlStatus ztqCmdExecHistory( ztqParseTree  * aParseTree,
                             stlChar       * aCmdStr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    ztqCmdExecHistoryTree * sParseTree;

    sParseTree = (ztqCmdExecHistoryTree*)aParseTree;

    STL_TRY( ztqExecHistory( sParseTree->mHistoryNum,
                             aAllocator,
                             aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\HISTORY' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 모든 HISTORY 엔트리들을 출력한다.
 */
stlStatus ztqCmdPrintHistory( ztqParseTree  * aParseTree,
                              stlChar       * aCmdStr,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    STL_TRY( ztqPrintHistory( aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\IMPORT '{sql_file}'' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks SQL 파일내의 모든 SQL을 수행한다.
 */
stlStatus ztqCmdImport( ztqParseTree  * aParseTree,
                        stlChar       * aCmdStr,
                        stlAllocator  * aAllocator,
                        stlErrorStack * aErrorStack )
{
    ztqCmdImportTree * sParseTree;
    stlFile            sFile;
    stlInt32           sState = 0;

    sParseTree = (ztqCmdImportTree*)aParseTree;

    STL_TRY( stlOpenFile( &sFile,
                          sParseTree->mImportFile,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( ztqProcessCommandStream( &sFile,
                                      STL_FALSE, /* aDisplayPrompt */
                                      STL_TRUE,  /* aEcho */
                                      aAllocator,
                                      aErrorStack )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)stlCloseFile( &sFile, aErrorStack );
    }

    return STL_FAILURE;
}


/**
 * @brief '\\EXPLAIN PLAN [ ON | ONLY ] sql_statement' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks SQL 문장에 대해서 PLAN 출력을 진행한다.
 */
stlStatus ztqCmdExplainPlan( ztqParseTree  * aParseTree,
                             stlChar       * aCmdStr,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    ztqCmdExplainPlanTree * sParseTree;
    ztqBypassTree         * sSqlTree;
    stlChar               * sSqlString;
    SQLHSTMT                sOdbcStmt = NULL;
    stlInt32                sState = 0;
    ztqSqlStmt            * sZtqSqlStmt;
    stlBool                 sPrintResult = STL_TRUE;

    sParseTree = (ztqCmdExplainPlanTree*)aParseTree;

    /*
     * Command String에서 SQL String을 추출한다.
     * "\execute select * from ..." ==> "select * from ..."
     */
    sSqlString = aCmdStr + sParseTree->mSqlStartPos;

    /*
     * White space 제거
     */
    while( stlIsspace( *sSqlString ) )
    {
        sSqlString++;
    }

    STL_TRY( ztqSqlParseIt( sSqlString,
                            aAllocator,
                            (ztqParseTree**)&sSqlTree,
                            aErrorStack )
             == STL_SUCCESS );

    /*
     * Empty SQL은 skip한다.
     */
    STL_TRY_THROW( sSqlTree != ZTQ_EMPTY_PARSE_TREE, RAMP_SKIP );

    ztqClearElapsedTime();

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sOdbcStmt,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    STL_TRY( ztqAllocSqlStmt( &sZtqSqlStmt,
                              (ztqParseTree*)sSqlTree,
                              sOdbcStmt,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 2;

    ZTQ_TIME_NOW( gZtqBeginTime );

    sPrintResult = ( sParseTree->mExplainPlanType != SQL_EXPLAIN_PLAN_ONLY );

    switch( sParseTree->mExplainPlanType )
    {
        case SQL_EXPLAIN_PLAN_OFF:
            sZtqSqlStmt->mExplainPlan = STL_FALSE;
            STL_TRY( ztqSQLSetStmtAttr( sOdbcStmt,
                                        SQL_ATTR_EXPLAIN_PLAN_OPTION,
                                        (SQLPOINTER)SQL_EXPLAIN_PLAN_OFF,
                                        0,
                                        aErrorStack )
                     == STL_SUCCESS );
            break;
        case SQL_EXPLAIN_PLAN_ON:
            sZtqSqlStmt->mExplainPlan = STL_TRUE;
            STL_TRY( ztqSQLSetStmtAttr( sOdbcStmt,
                                        SQL_ATTR_EXPLAIN_PLAN_OPTION,
                                        (SQLPOINTER)SQL_EXPLAIN_PLAN_ON,
                                        0,
                                        aErrorStack )
                     == STL_SUCCESS );
            break;
        case SQL_EXPLAIN_PLAN_ON_VERBOSE:
            sZtqSqlStmt->mExplainPlan = STL_TRUE;
            STL_TRY( ztqSQLSetStmtAttr( sOdbcStmt,
                                        SQL_ATTR_EXPLAIN_PLAN_OPTION,
                                        (SQLPOINTER)SQL_EXPLAIN_PLAN_ON_VERBOSE,
                                        0,
                                        aErrorStack )
                     == STL_SUCCESS );
            break;
        case SQL_EXPLAIN_PLAN_ONLY:
            sZtqSqlStmt->mExplainPlan = STL_TRUE;
            STL_TRY( ztqSQLSetStmtAttr( sOdbcStmt,
                                        SQL_ATTR_EXPLAIN_PLAN_OPTION,
                                        (SQLPOINTER)SQL_EXPLAIN_PLAN_ONLY,
                                        0,
                                        aErrorStack )
                     == STL_SUCCESS );
            break;
        default:
            break;
    }

    STL_TRY( ztqExecuteSql( sSqlString,
                            aAllocator,
                            sZtqSqlStmt,
                            STL_TRUE,     /* aDirectExecute */
                            sPrintResult,
                            aErrorStack )
             == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqEndTime );

    if( sPrintResult == STL_TRUE )
    {
        STL_TRY( ztqPrintElapsedTime( aErrorStack ) == STL_SUCCESS );
    }

    /*
     * Direct Execute에서는 Region Memory를 사용해 ztqSqlStmt를
     * 할당했기 때문에 할당된 ztqSqlStmt를 해제할 필요가 없다.
     *
     * STL_TRY( ztqFreeSqlStmt( sZtqSqlStmt,
     *                          aErrorStack )
     *        == STL_SUCCESS );
     */

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sOdbcStmt,
                               aErrorStack )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:

            ZTQ_TIME_NOW( gZtqEndTime );

            if( sPrintResult == STL_TRUE )
            {
                (void) ztqPrintElapsedTime( aErrorStack );
            }
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sOdbcStmt,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief '\\ALLOC '{stmt_name}'' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdAllocStmt( ztqParseTree  * aParseTree,
                           stlChar       * aCmdStr,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack )
{
    ztqCmdAllocStmtTree * sParseTree;

    sParseTree = (ztqCmdAllocStmtTree *) aParseTree;

    STL_TRY( ztqAllocNamedStmt( sParseTree->mStmtName, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\FREE '{stmt_name}'' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdFreeStmt( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack )
{
    ztqCmdFreeStmtTree * sParseTree;

    sParseTree = (ztqCmdFreeStmtTree *) aParseTree;

    STL_TRY( ztqFreeNamedStmt( sParseTree->mStmtName, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\USE '{stmt_name}' EXEC SQL '{sql string}'' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdUseStmtExecSql( ztqParseTree  * aParseTree,
                                stlChar       * aCmdStr,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack )
{
    ztqCmdUseStmtExecSqlTree * sParseTree;

    ztqNamedStmt      * sNamedStmt;
    ztqBypassTree     * sSqlTree;
    stlChar           * sSqlString;

    sParseTree = (ztqCmdUseStmtExecSqlTree *) aParseTree;

    /*******************************************
     * Named Statement 검색
     *******************************************/

    sNamedStmt = NULL;
    STL_TRY( ztqFindNamedStmt( sParseTree->mStmtName,
                               & sNamedStmt,
                               aErrorStack )
             == STL_SUCCESS );
    STL_TRY_THROW( sNamedStmt != NULL, RAMP_ERR_NAMED_STMT_NOT_EXIST );

    STL_TRY( ztqResetNamedStmt( sNamedStmt, aErrorStack ) == STL_SUCCESS );

    /*******************************************
     * \EXEC SQL ... 과 유사한 처리 과정을 거침
     * - 단, SQLFreeHandle() 을 호출하지 않는다.
     *******************************************/

    /*
     * SQL String 추출
     */

    sSqlString = aCmdStr + sParseTree->mSqlStartPos;

    /*
     * White space 제거
     */

    while( stlIsspace( *sSqlString ) )
    {
        sSqlString++;
    }

    /*
     * Parsing
     */

    STL_TRY( ztqSqlParseIt( sSqlString,
                            aAllocator,
                            (ztqParseTree**)&sSqlTree,
                            aErrorStack )
             == STL_SUCCESS );

    /*
     * Empty SQL은 skip한다.
     */
    STL_TRY_THROW( sSqlTree != ZTQ_EMPTY_PARSE_TREE, RAMP_SKIP );

    /*
     * Named Statement 정보 설정
     */

    sNamedStmt->mZtqSqlStmt.mStatementType = sSqlTree->mStatementType;
    sNamedStmt->mZtqSqlStmt.mObjectType    = sSqlTree->mObjectType;
    if ( sSqlTree->mObjectName != NULL )
    {
        stlStrncpy( sNamedStmt->mZtqSqlStmt.mObjectName, sSqlTree->mObjectName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    else
    {
        sNamedStmt->mZtqSqlStmt.mObjectName[0] = '\0';
    }
    
    if ( sSqlTree->mHostVarList != NULL )
    {
        STL_TRY( ztqCloneHostVarList( sSqlTree->mHostVarList,
                                      & sNamedStmt->mZtqSqlStmt.mHostVarList,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        sNamedStmt->mZtqSqlStmt.mHostVarList = NULL;
    }

    ZTQ_TIME_NOW( gZtqBeginTime );

    /*
     * Host 변수 처리를 위한 Prepare
     */
    STL_TRY( ztqPrepareSql( sSqlString,
                            aAllocator,
                            & sNamedStmt->mZtqSqlStmt,
                            aErrorStack )
             == STL_SUCCESS );

    /*
     * Execution
     */

    STL_TRY( ztqExecuteSql( sSqlString,
                            aAllocator,
                            & sNamedStmt->mZtqSqlStmt,
                            STL_FALSE,   /* aDirectExecute */
                            STL_TRUE,    /* Print Result */
                            aErrorStack )
             == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqEndTime );

    STL_TRY( ztqPrintElapsedTime( aErrorStack ) == STL_SUCCESS );

    /*******************************************
     * DIRECT EXECUTION
     *******************************************/

    sNamedStmt->mIsPrepared  = STL_FALSE;

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMED_STMT_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NAMED_STMT_NOT_EXIST,
                      NULL,
                      aErrorStack,
                      sParseTree->mStmtName );
    }

    STL_FINISH;

    if ( sNamedStmt != NULL )
    {
        (void) ztqResetNamedStmt( sNamedStmt, aErrorStack );
    }

    return STL_FAILURE;
}

/**
 * @brief '\\USE '{stmt_name}' PREPARE SQL '{sql string}'' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdUseStmtPrepareSql( ztqParseTree  * aParseTree,
                                   stlChar       * aCmdStr,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack )
{
    stlInt32    sState = 0;

    ztqCmdUseStmtPrepareSqlTree * sParseTree;

    ztqNamedStmt      * sNamedStmt;
    ztqBypassTree     * sSqlTree;
    stlChar           * sSqlString;

    sParseTree = (ztqCmdUseStmtPrepareSqlTree *) aParseTree;

    /*******************************************
     * Named Statement 검색
     *******************************************/

    sNamedStmt = NULL;
    STL_TRY( ztqFindNamedStmt( sParseTree->mStmtName,
                               & sNamedStmt,
                               aErrorStack )
             == STL_SUCCESS );
    STL_TRY_THROW( sNamedStmt != NULL, RAMP_ERR_NAMED_STMT_NOT_EXIST );

    STL_TRY( ztqResetNamedStmt( sNamedStmt, aErrorStack ) == STL_SUCCESS );

    /*******************************************
     * \PREPARE SQL ... 과 유사한 처리 과정을 거침
     * - 단, SQLFreeHandle() 을 호출하지 않는다.
     *******************************************/

    /*
     * SQL String 추출
     */

    sSqlString = aCmdStr + sParseTree->mSqlStartPos;

    /*
     * White space 제거
     */

    while( stlIsspace( *sSqlString ) )
    {
        sSqlString++;
    }

    /*
     * Parsing
     */

    STL_TRY( ztqSqlParseIt( sSqlString,
                            aAllocator,
                            (ztqParseTree**)&sSqlTree,
                            aErrorStack )
             == STL_SUCCESS );

    /*
     * Named Statement 정보 설정
     */

    sNamedStmt->mZtqSqlStmt.mStatementType = sSqlTree->mStatementType;
    sNamedStmt->mZtqSqlStmt.mObjectType    = sSqlTree->mObjectType;
    if ( sSqlTree->mObjectName != NULL )
    {
        stlStrncpy( sNamedStmt->mZtqSqlStmt.mObjectName, sSqlTree->mObjectName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    else
    {
        sNamedStmt->mZtqSqlStmt.mObjectName[0] = '\0';
    }
    
    if ( sSqlTree->mHostVarList != NULL )
    {
        STL_TRY( ztqCloneHostVarList( sSqlTree->mHostVarList,
                                      & sNamedStmt->mZtqSqlStmt.mHostVarList,
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        sNamedStmt->mZtqSqlStmt.mHostVarList = NULL;
    }

    ZTQ_TIME_NOW( gZtqBeginTime );

    /*
     * Prepare 수행
     */

    sState = 1;
    STL_TRY( ztqPrepareSql( sSqlString,
                            aAllocator,
                            & sNamedStmt->mZtqSqlStmt,
                            aErrorStack )
             == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqEndTime );

    STL_TRY( ztqPrintPreparedAnswerMessage( aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztqPrintElapsedTime( aErrorStack ) == STL_SUCCESS );

    /*******************************************
     * PREPARE EXECUTION
     *******************************************/

    sNamedStmt->mIsPrepared  = STL_TRUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMED_STMT_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NAMED_STMT_NOT_EXIST,
                      NULL,
                      aErrorStack,
                      sParseTree->mStmtName );
    }

    STL_FINISH;

    switch ( sState )
    {
        case 1:
            ZTQ_TIME_NOW( gZtqEndTime );
            (void) ztqPrintElapsedTime( aErrorStack );
        case 0:
        default:
            break;
    }

    if ( sNamedStmt != NULL )
    {
        (void) ztqResetNamedStmt( sNamedStmt, aErrorStack );
    }

    return STL_FAILURE;
}



/**
 * @brief '\\USE '{stmt_name}' EXEC' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdUseStmtExecPrepared( ztqParseTree  * aParseTree,
                                     stlChar       * aCmdStr,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack )
{
    stlInt32    sState = 0;

    ztqCmdUseStmtExecPreparedTree * sParseTree;

    ztqNamedStmt      * sNamedStmt;

    sParseTree = (ztqCmdUseStmtExecPreparedTree *) aParseTree;

    /*******************************************
     * Named Statement 검색
     *******************************************/

    sNamedStmt = NULL;
    STL_TRY( ztqFindNamedStmt( sParseTree->mStmtName,
                               & sNamedStmt,
                               aErrorStack )
             == STL_SUCCESS );
    STL_TRY_THROW( sNamedStmt != NULL, RAMP_ERR_NAMED_STMT_NOT_EXIST );

    STL_TRY_THROW( sNamedStmt->mIsPrepared == STL_TRUE,
                   RAMP_ERR_NAMED_STMT_NOT_PREPARED );

    /*******************************************
     * \EXEC 와 유사한 처리 과정을 거침
     * - 단, SQLFreeHandle() 을 호출하지 않는다.
     *******************************************/

    ztqClearElapsedTime();

    ZTQ_TIME_NOW( gZtqBeginTime );

    sState = 1;
    STL_TRY( ztqExecuteSql( NULL,
                            aAllocator,
                            & sNamedStmt->mZtqSqlStmt,
                            STL_FALSE,  /* aDirectExecute */
                            STL_TRUE,   /* aPrintResult */
                            aErrorStack )
             == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqEndTime );

    STL_TRY( ztqPrintElapsedTime( aErrorStack ) == STL_SUCCESS );

    /*******************************************
     * PREPARE EXECUTION
     *******************************************/

    sNamedStmt->mIsPrepared  = STL_TRUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMED_STMT_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NAMED_STMT_NOT_EXIST,
                      NULL,
                      aErrorStack,
                      sParseTree->mStmtName );
    }

    STL_CATCH( RAMP_ERR_NAMED_STMT_NOT_PREPARED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NAMED_STMT_NOT_PREPARED,
                      NULL,
                      aErrorStack,
                      sParseTree->mStmtName );
    }

    STL_FINISH;

    switch ( sState )
    {
        case 1:
            ZTQ_TIME_NOW( gZtqEndTime );
            (void) ztqPrintElapsedTime( aErrorStack );
        case 0:
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief '\\STARTUP {startup_phase}' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdStartup( ztqParseTree  * aParseTree,
                         stlChar       * aCmdStr,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack )
{
    ztqCmdStartupTree * sParseTree;
    stlChar             sBuffer[1024];
    stlChar           * sArgv[3];

    STL_TRY_THROW( gZtqAdmin == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    STL_TRY( ztqCloseDatabase( aErrorStack ) == STL_SUCCESS );

    sParseTree = (ztqCmdStartupTree *) aParseTree;

    if( gZtqOperationMode == ZTQ_OPERATION_MODE_DA )
    {
        sArgv[0] = "gmaster";
        sArgv[1] = NULL;

        /*
         * 에러가 발생했을때 Connection은 살아있어야 한다.
         */
        (void)stlExecuteProcAndWait( sArgv, aErrorStack );

        STL_TRY( ztqOpenDatabase( gZtqDsn,
                                  gZtqUserName,
                                  gZtqPassword,
                                  NULL,        /* aNewPassword */
                                  STL_FALSE,   /* Start-Up */
                                  aAllocator,
                                  aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( stlGetErrorStackDepth( aErrorStack ) == 0 );
    }
    else
    {
        STL_DASSERT( gZtqOperationMode == ZTQ_OPERATION_MODE_CS );

        if( ztqOpenDatabase( gZtqDsn,
                             gZtqUserName,
                             gZtqPassword,
                             NULL,        /* aNewPassword */
                             STL_TRUE,    /* Start-Up */
                             aAllocator,
                             aErrorStack ) == STL_FAILURE )
        {
            /*
             * 에러가 발생했을때 Connection은 살아있어야 한다.
             */
            
            STL_TRY( ztqOpenDatabase( gZtqDsn,
                                      gZtqUserName,
                                      gZtqPassword,
                                      NULL,  /* aNewPassword */
                                      STL_FALSE,    /* Start-Up */
                                      aAllocator,
                                      aErrorStack )
                     == STL_SUCCESS );

            STL_TRY( STL_FALSE );
        }
    }

    if( sParseTree->mStartupPhase >= ZTQ_STARTUP_MOUNT )
    {
        stlSnprintf( sBuffer,
                     1024,
                     "ALTER SYSTEM MOUNT DATABASE" );

        STL_TRY( ztqProcessSql( sBuffer,
                                aAllocator,
                                STL_FALSE,  /* aPrintResult */
                                aErrorStack ) == STL_SUCCESS );
    }

    if( sParseTree->mStartupPhase >= ZTQ_STARTUP_OPEN )
    {
        stlSnprintf( sBuffer,
                     1024,
                     "ALTER SYSTEM OPEN DATABASE" );

        STL_TRY( ztqProcessSql( sBuffer,
                                aAllocator,
                                STL_FALSE,  /* aPrintResult */
                                aErrorStack ) == STL_SUCCESS );
    }

    ztqPrintf( "Startup success\n" );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INSUFFICIENT_PRIVILEGES,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\SHUTDOWN' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdShutdown( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack )
{
    ztqCmdShutdownTree * sParseTree;
    stlChar              sBuffer[1024];
    SQLHSTMT             sStmtHandle = NULL;
    stlInt32             sState = 0;
    SQLRETURN            sResult;
    stlUInt32            sOldCommitAttr = 0;

    /* TODO : gZtqAdmin로 PRIVILEGE 체크는 서버에서 해야 함 */
    STL_TRY_THROW( gZtqAdmin == STL_TRUE, RAMP_ERR_INSUFFICIENT_PRIVILEGES );

    sParseTree = (ztqCmdShutdownTree *) aParseTree;

    switch( sParseTree->mShutdownMode )
    {
        case ZTQ_SHUTDOWN_ABORT:
            stlSnprintf( sBuffer,
                         1024,
                         "ALTER SYSTEM CLOSE DATABASE ABORT" );
            break;
        case ZTQ_SHUTDOWN_OPERATIONAL:
            stlSnprintf( sBuffer,
                         1024,
                         "ALTER SYSTEM CLOSE DATABASE OPERATIONAL" );
            break;
        case ZTQ_SHUTDOWN_IMMEDIATE:
            stlSnprintf( sBuffer,
                         1024,
                         "ALTER SYSTEM CLOSE DATABASE IMMEDIATE" );
            break;
        case ZTQ_SHUTDOWN_TRANSACTIONAL:
            stlSnprintf( sBuffer,
                         1024,
                         "ALTER SYSTEM CLOSE DATABASE TRANSACTIONAL" );
            break;
        case ZTQ_SHUTDOWN_NORMAL:
            stlSnprintf( sBuffer,
                         1024,
                         "ALTER SYSTEM CLOSE DATABASE NORMAL" );
            break;
        default:
            STL_DASSERT(0);
            break;
    }

    STL_TRY( ztqSQLGetConnectAttr( gZtqDbcHandle,
                                   SQL_ATTR_AUTOCOMMIT,
                                   (SQLPOINTER)&sOldCommitAttr,
                                   SQL_IS_UINTEGER,
                                   NULL,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztqSQLSetConnectAttr( gZtqDbcHandle,
                                   SQL_ATTR_AUTOCOMMIT,
                                   (SQLPOINTER)SQL_AUTOCOMMIT_OFF,
                                   SQL_IS_UINTEGER,
                                   aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 2;
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*)sBuffer,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqCloseDatabase( aErrorStack ) == STL_SUCCESS );
    
    ztqPrintf( "Shutdown success\n" );

    STL_TRY( ztqOpenDatabase( gZtqDsn,
                              gZtqUserName,
                              gZtqPassword,
                              NULL,  /* aNewPassword */
                              STL_FALSE,   /* Start-Up */
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_PRIVILEGES )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INSUFFICIENT_PRIVILEGES,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                     sStmtHandle,
                                     aErrorStack );
        case 1:
            (void) ztqSQLSetConnectAttr( gZtqDbcHandle,
                                         SQL_ATTR_AUTOCOMMIT,
                                         (SQLPOINTER)(stlVarInt)sOldCommitAttr,
                                         SQL_IS_UINTEGER,
                                         aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief '\\CONNECT [userid password] [AS SYSDBA]' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdConnect( ztqParseTree  * aParseTree,
                         stlChar       * aCmdStr,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack )
{
    ztqCmdConnectTree * sParseTree;

    sParseTree = (ztqCmdConnectTree *) aParseTree;

    STL_TRY( ztqCloseDatabase( aErrorStack ) == STL_SUCCESS );       

    /*
     * \\CONNECT test test - mAsAdmin : FALSE, mAsSystemAdmin : FALSE
     * \\CONNECT AS SYSDBA - mAsAdmin : TRUE,  mAsSystemAdmin : FALSE
     * \\CONNECT AS ADMIN  - mAsAdmin : TRUE,  mAsSystemAdmin : TRUE
     */

    switch( sParseTree->mAsUser )
    {
        case ZTQ_CONNECT_AS_USER:
            gZtqAdmin = STL_FALSE;
            gZtqSystemAdmin = STL_FALSE;
            break;
        case ZTQ_CONNECT_AS_SYSDBA:
            gZtqAdmin = STL_TRUE;
            gZtqSystemAdmin = STL_FALSE;
            break;
        case ZTQ_CONNECT_AS_ADMIN:
            gZtqAdmin = STL_TRUE;
            gZtqSystemAdmin = STL_TRUE;
            break;
        default:
            STL_ASSERT( 0 );
            break;
    } 

    if ( sParseTree->mNewPassword[0] == '\0' )
    {
        STL_TRY( ztqOpenDatabase( gZtqDsn,
                                  sParseTree->mUserId,
                                  sParseTree->mPassword,
                                  NULL,  /* aNewPassword */
                                  STL_FALSE, /* Start-Up */
                                  aAllocator,
                                  aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqOpenDatabase( gZtqDsn,
                                  sParseTree->mUserId,
                                  sParseTree->mPassword,
                                  sParseTree->mNewPassword,
                                  STL_FALSE, /* Start-Up */
                                  aAllocator,
                                  aErrorStack )
                 == STL_SUCCESS );
    }

    stlStrncpy( gZtqUserName,
                sParseTree->mUserId,
                STL_MAX_SQL_IDENTIFIER_LENGTH );

    stlToupperString( gZtqUserName, gZtqUserName );

    stlStrncpy( gZtqPassword,
                sParseTree->mPassword,
                STL_MAX_SQL_IDENTIFIER_LENGTH );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\XA OPEN' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdXaOpen( ztqParseTree  * aParseTree,
                        stlChar       * aCmdStr,
                        stlAllocator  * aAllocator,
                        stlErrorStack * aErrorStack )
{
    ztqCmdXaOpenTree * sParseTree;
    stlInt32           sState = 0;

    sParseTree = (ztqCmdXaOpenTree *) aParseTree;

    STL_TRY( ztqCloseDatabase( aErrorStack ) == STL_SUCCESS );
    sState = 1;

    if( gZtqEnvHandle == NULL )
    {
        STL_TRY_THROW( ztqSQLAllocHandle( SQL_HANDLE_ENV,
                                          NULL,
                                          &gZtqEnvHandle,
                                          aErrorStack )
                       == STL_SUCCESS,
                       RAMP_ERR_SERVICE_NOT_AVAILABLE );
    }

    STL_TRY( ztqSQLSetEnvAttr( gZtqEnvHandle,
                               SQL_ATTR_ODBC_VERSION,
                               (SQLPOINTER)gZtqOdbcVersion,
                               0,
                               aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqOpenXa( sParseTree->mXaInfo,
                        sParseTree->mFlags,
                        aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqGetXaConnectionHandle( &gZtqDbcHandle,
                                       aErrorStack )
             == STL_SUCCESS );
    
    gZtqConnectionEstablished = STL_TRUE;
    sState = 0;

    ztqPrintf( "XA Session opened\n" );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SERVICE_NOT_AVAILABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_SERVICE_NOT_AVAILABLE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void) ztqOpenDatabase( gZtqDsn,
                                gZtqUserName,
                                gZtqPassword,
                                NULL,  /* aNewPassword */
                                STL_FALSE,   /* Start-Up */
                                aAllocator,
                                aErrorStack );
    }

    return STL_FAILURE;
}

/**
 * @brief '\\XA CLOSE' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdXaClose( ztqParseTree  * aParseTree,
                         stlChar       * aCmdStr,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack )
{
    ztqCmdXaCloseTree * sParseTree;

    sParseTree = (ztqCmdXaCloseTree *) aParseTree;

    STL_TRY( ztqCloseXa( sParseTree->mXaInfo,
                         sParseTree->mFlags,
                         aErrorStack )
             == STL_SUCCESS );
    
    gZtqConnectionEstablished = STL_FALSE;
    gZtqDbcHandle = NULL;

    STL_TRY( ztqOpenDatabase( gZtqDsn,
                              gZtqUserName,
                              gZtqPassword,
                              NULL,        /* aNewPassword */
                              STL_FALSE,   /* Start-Up */
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    ztqPrintf( "XA Session closed\n" );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\XA START' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdXaStart( ztqParseTree  * aParseTree,
                         stlChar       * aCmdStr,
                         stlAllocator  * aAllocator,
                         stlErrorStack * aErrorStack )
{
    ztqCmdXaStartTree * sParseTree;

    sParseTree = (ztqCmdXaStartTree *) aParseTree;

    STL_TRY( ztqStartXa( &sParseTree->mXid,
                         sParseTree->mFlags,
                         aErrorStack )
             == STL_SUCCESS );

    ztqPrintf( "Global transaction started\n" );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\XA END' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdXaEnd( ztqParseTree  * aParseTree,
                       stlChar       * aCmdStr,
                       stlAllocator  * aAllocator,
                       stlErrorStack * aErrorStack )
{
    ztqCmdXaEndTree * sParseTree;

    sParseTree = (ztqCmdXaEndTree *) aParseTree;

    STL_TRY( ztqEndXa( &sParseTree->mXid,
                       sParseTree->mFlags,
                       aErrorStack )
             == STL_SUCCESS );

    ztqPrintf( "Global transaction ended\n" );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\XA COMMIT' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdXaCommit( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack )
{
    ztqCmdXaCommitTree * sParseTree;

    sParseTree = (ztqCmdXaCommitTree *) aParseTree;

    STL_TRY( ztqCommitXa( &sParseTree->mXid,
                          sParseTree->mFlags,
                          aErrorStack )
             == STL_SUCCESS );

    ztqPrintf( "Commit completed\n" );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\XA ROLLBACK' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdXaRollback( ztqParseTree  * aParseTree,
                            stlChar       * aCmdStr,
                            stlAllocator  * aAllocator,
                            stlErrorStack * aErrorStack )
{
    ztqCmdXaRollbackTree * sParseTree;

    sParseTree = (ztqCmdXaRollbackTree *) aParseTree;

    STL_TRY( ztqRollbackXa( &sParseTree->mXid,
                            sParseTree->mFlags,
                            aErrorStack )
             == STL_SUCCESS );

    ztqPrintf( "Rollback completed\n" );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\XA PREPARE' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdXaPrepare( ztqParseTree  * aParseTree,
                           stlChar       * aCmdStr,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack )
{
    ztqCmdXaPrepareTree * sParseTree;

    sParseTree = (ztqCmdXaPrepareTree *) aParseTree;

    STL_TRY( ztqPrepareXa( &sParseTree->mXid,
                           sParseTree->mFlags,
                           aErrorStack )
             == STL_SUCCESS );

    ztqPrintf( "Prepare completed\n" );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\XA FORGET' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdXaForget( ztqParseTree  * aParseTree,
                          stlChar       * aCmdStr,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack )
{
    ztqCmdXaForgetTree * sParseTree;

    sParseTree = (ztqCmdXaForgetTree *) aParseTree;

    STL_TRY( ztqForgetXa( &sParseTree->mXid,
                          sParseTree->mFlags,
                          aErrorStack )
             == STL_SUCCESS );

    ztqPrintf( "Forget completed\n" );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\XA FORGET' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks
 */
stlStatus ztqCmdXaRecover( ztqParseTree  * aParseTree,
                           stlChar       * aCmdStr,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack )
{
    ztqCmdXaRecoverTree * sParseTree;
    stlInt64              sFetchCount = 0;

    sParseTree = (ztqCmdXaRecoverTree *) aParseTree;

    STL_TRY( ztqRecoverXa( sParseTree->mXids,
                           sParseTree->mCount,
                           sParseTree->mFlags,
                           &sFetchCount,
                           aErrorStack )
             == STL_SUCCESS );

    if( sFetchCount > 0 )
    {
        STL_TRY( ztqPrintXaRecover( sParseTree->mXids,
                                    sFetchCount,
                                    aAllocator,
                                    aErrorStack )
                 == STL_SUCCESS );
    }

    STL_TRY( ztqPrintDmlAnswerMessage( ZTQ_STMT_SELECT,
                                       sFetchCount,
                                       aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

ztqTargetColumn gZtqXaRecoverTargetColumn[2] =
{
    {
        "XID",
        ZTQ_COLUMN_FORMAT_CLASS_STRING,
        STL_XID_STRING_SIZE,
        NULL,
        SQL_VARCHAR,
        SQL_NULL_DATA,
        ZTQ_GROUP_STRING,
        DTL_TYPE_NA,
        {0,0}
    }
};

stlStatus ztqPrintXaRecover( XID           * aXids,
                             stlInt64        aXidCount,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    ztqRowBuffer * sRowBuffer;
    stlInt32       i;
    stlInt32       j;
    stlInt32       sLineCount = 0;
    stlChar        sXidString[STL_XID_STRING_SIZE];

    STL_TRY( ztqAllocRowBuffer( &sRowBuffer,
                                1,                /* aColumnCount */
                                gZtqPageSize,     /* aLineBufferCount */
                                gZtqXaRecoverTargetColumn,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );

    for( i = 0; i < aXidCount; i++ )
    {
        STL_TRY( dtlXidToString( (stlXid*)&aXids[i],
                                 STL_XID_STRING_SIZE,
                                 sXidString,
                                 aErrorStack )
                 == STL_SUCCESS );
                                 
        
        gZtqXaRecoverTargetColumn[0].mDataValue = sXidString;
        gZtqXaRecoverTargetColumn[0].mIndicator = stlStrlen( sXidString );
            
        STL_TRY( ztqMakeRowString( sRowBuffer,
                                   sLineCount,
                                   gZtqXaRecoverTargetColumn,
                                   STL_TRUE,
                                   aErrorStack )
                 == STL_SUCCESS );
        sLineCount += 1;

        if( sLineCount == gZtqPageSize )
        {
            for( j = 0; j < sLineCount; j++ )
            {
                if( j == 0 )
                {
                    STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                                  gZtqXaRecoverTargetColumn )
                             == STL_SUCCESS );
                }

                STL_TRY( ztqPrintRowString( sRowBuffer,
                                            gZtqXaRecoverTargetColumn,
                                            j,
                                            aAllocator,
                                            aErrorStack )
                         == STL_SUCCESS );

            }
                
            ztqSetInitRowStringLen( sRowBuffer, gZtqXaRecoverTargetColumn );
            sLineCount = 0;
            ztqPrintf( "\n" );
        }
    }
    
    for( i = 0; i < sLineCount; i++ )
    {
        if( i == 0 )
        {
            STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                          gZtqXaRecoverTargetColumn )
                     == STL_SUCCESS );
        }

        STL_TRY( ztqPrintRowString( sRowBuffer,
                                    gZtqXaRecoverTargetColumn,
                                    i,
                                    aAllocator,
                                    aErrorStack )
                 == STL_SUCCESS );
    }
    
    ztqPrintf( "\n" );
            
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



stlStatus ztqCmdPrintDatabaseDDL( ztqParseTree  * aParseTree,
                                  stlChar       * aCmdStr,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Option 에 따른 DDL 출력 
     */
    
    switch ( sCmdTree->mDDLType )
    {
        case ZTQ_PRINT_DDL_DB_ALL:
            {
                STL_TRY( ztqPrintAllDDL( sStringDDL,
                                         aAllocator,
                                         aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_DB_GRANT:
            {
                STL_TRY( ztqPrintGrantDB( sStringDDL,
                                          aAllocator,
                                          aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_DB_COMMENT:
            {
                STL_TRY( ztqPrintCommentDB( sStringDDL,
                                            aAllocator,
                                            aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqCmdPrintSpaceDDL( ztqParseTree  * aParseTree,
                               stlChar       * aCmdStr,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    stlInt64    sSpaceID = 0;
    stlBool     sIsBuiltIn = STL_FALSE;

    /**
     * Object ID 획득
     */

    STL_TRY( ztqGetSpaceID( sCmdTree->mIdentifierList,
                            aAllocator,
                            & sSpaceID,
                            & sIsBuiltIn,
                            aErrorStack )
             == STL_SUCCESS );
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Option 에 따른 DDL 출력 
     */
    
    switch ( sCmdTree->mDDLType )
    {
        case ZTQ_PRINT_DDL_SPACE_CREATE:
            {
                STL_TRY( ztqPrintCreateSpace( sSpaceID,
                                              sStringDDL,
                                              aAllocator,
                                              aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SPACE_ALTER:
            {
                STL_TRY( ztqPrintAlterSpace( sSpaceID,
                                             sStringDDL,
                                             aAllocator,
                                             aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SPACE_TABLE:
            {
                STL_TRY( ztqPrintSpaceCreateTable( sSpaceID,
                                                   sStringDDL,
                                                   aAllocator,
                                                   aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SPACE_CONSTRAINT:
            {
                STL_TRY( ztqPrintSpaceConstraint( sSpaceID,
                                                  sStringDDL,
                                                  aAllocator,
                                                  aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SPACE_INDEX:
            {
                STL_TRY( ztqPrintSpaceCreateIndex( sSpaceID,
                                                   sStringDDL,
                                                   aAllocator,
                                                   aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SPACE_GRANT:
            {
                STL_TRY( ztqPrintGrantSpace( sSpaceID,
                                             sStringDDL,
                                             aAllocator,
                                             aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SPACE_COMMENT:
            {
                STL_TRY( ztqPrintCommentSpace( sSpaceID,
                                               sStringDDL,
                                               aAllocator,
                                               aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



stlStatus ztqCmdPrintProfileDDL( ztqParseTree  * aParseTree,
                                 stlChar       * aCmdStr,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    stlInt64    sProfileID = 0;

    /**
     * Object ID 획득
     */

    STL_TRY( ztqGetProfileID( sCmdTree->mIdentifierList,
                              aAllocator,
                              & sProfileID,
                              aErrorStack )
             == STL_SUCCESS );
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Option 에 따른 DDL 출력 
     */
    
    switch ( sCmdTree->mDDLType )
    {
        case ZTQ_PRINT_DDL_PROFILE_CREATE:
            {
                STL_TRY( ztqPrintCreateProfile( sProfileID,
                                                sStringDDL,
                                                aAllocator,
                                                aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_PROFILE_COMMENT:
            {
                STL_TRY( ztqPrintCommentProfile( sProfileID,
                                                 sStringDDL,
                                                 aAllocator,
                                                 aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief '\\ddl_auth user_name' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 프로그램을 종료 시킨다.
 */
stlStatus ztqCmdPrintAuthDDL( ztqParseTree  * aParseTree,
                              stlChar       * aCmdStr,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    stlInt64    sAuthID = 0;
    stlBool     sIsUser = STL_FALSE;

    /**
     * Object ID 획득
     */

    STL_TRY( ztqGetAuthID( sCmdTree->mIdentifierList,
                           aAllocator,
                           & sAuthID,
                           & sIsUser,
                           aErrorStack )
             == STL_SUCCESS );
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Option 에 따른 DDL 출력 
     */
    
    switch ( sCmdTree->mDDLType )
    {
        case ZTQ_PRINT_DDL_AUTH_CREATE:
            {
                /**
                 * CREATE ROLE 구문이 없다
                 */
                
                STL_TRY_THROW( sIsUser == STL_TRUE, RAMP_ERR_INVALID_OBJECT_IDENTIFIER );
                
                STL_TRY( ztqPrintCreateUser( sAuthID,
                                             sStringDDL,
                                             aAllocator,
                                             aErrorStack )
                         == STL_SUCCESS );

                break;
            }
        case ZTQ_PRINT_DDL_AUTH_SCHEMA_PATH:
            {
                STL_TRY( ztqPrintAlterUserSchemaPath( sAuthID,
                                                      sStringDDL,
                                                      aAllocator,
                                                      aErrorStack )
                         == STL_SUCCESS );
    
                break;
            }
        case ZTQ_PRINT_DDL_AUTH_SCHEMA:
            {
                STL_TRY( ztqPrintOwnerCreateSchema( sAuthID,
                                                    sStringDDL,
                                                    aAllocator,
                                                    aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_AUTH_TABLE:
            {
                STL_TRY( ztqPrintOwnerCreateTable( sAuthID,
                                                   sStringDDL,
                                                   aAllocator,
                                                   aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_AUTH_CONSTRAINT:
            {
                STL_TRY( ztqPrintOwnerConstraint( sAuthID,
                                                  sStringDDL,
                                                  aAllocator,
                                                  aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_AUTH_INDEX:
            {
                STL_TRY( ztqPrintOwnerCreateIndex( sAuthID,
                                                   sStringDDL,
                                                   aAllocator,
                                                   aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_AUTH_VIEW:
            {
                STL_TRY( ztqPrintOwnerCreateView( sAuthID,
                                                  sStringDDL,
                                                  aAllocator,
                                                  aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_AUTH_SEQUENCE:
            {
                STL_TRY( ztqPrintOwnerCreateSequence( sAuthID,
                                                      sStringDDL,
                                                      aAllocator,
                                                      aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_AUTH_SYNONYM:
            {
                STL_TRY( ztqPrintOwnerCreateSynonym( sAuthID,
                                                     sStringDDL,
                                                     aAllocator,
                                                     aErrorStack )
                         == STL_SUCCESS );
                break;
            }    
        case ZTQ_PRINT_DDL_AUTH_COMMENT:
            {
                STL_TRY( ztqPrintCommentAuth( sAuthID,
                                              sStringDDL,
                                              aAllocator,
                                              aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OBJECT_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqCmdPrintSchemaDDL( ztqParseTree  * aParseTree,
                                stlChar       * aCmdStr,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    stlInt64    sSchemaID = 0;

    /**
     * Object ID 획득
     */

    STL_TRY( ztqGetSchemaID( sCmdTree->mIdentifierList,
                             aAllocator,
                             & sSchemaID,
                             aErrorStack )
             == STL_SUCCESS );
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Option 에 따른 DDL 출력 
     */
    
    switch ( sCmdTree->mDDLType )
    {
        case ZTQ_PRINT_DDL_SCHEMA_CREATE:
            {
                STL_TRY( ztqPrintCreateSchema( sSchemaID,
                                               sStringDDL,
                                               aAllocator,
                                               aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SCHEMA_TABLE:
            {
                STL_TRY( ztqPrintSchemaCreateTable( sSchemaID,
                                                    sStringDDL,
                                                    aAllocator,
                                                    aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SCHEMA_CONSTRAINT:
            {
                STL_TRY( ztqPrintSchemaConstraint( sSchemaID,
                                                   sStringDDL,
                                                   aAllocator,
                                                   aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SCHEMA_INDEX:
            {
                STL_TRY( ztqPrintSchemaCreateIndex( sSchemaID,
                                                    sStringDDL,
                                                    aAllocator,
                                                    aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SCHEMA_VIEW:
            {
                STL_TRY( ztqPrintSchemaCreateView( sSchemaID,
                                                   sStringDDL,
                                                   aAllocator,
                                                   aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SCHEMA_SEQUENCE:
            {
                STL_TRY( ztqPrintSchemaCreateSequence( sSchemaID,
                                                       sStringDDL,
                                                       aAllocator,
                                                       aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SCHEMA_SYNONYM:
            {
                STL_TRY( ztqPrintSchemaCreateSynonym( sSchemaID,
                                                      sStringDDL,
                                                      aAllocator,
                                                      aErrorStack )
                         == STL_SUCCESS );
                break;
            }    
        case ZTQ_PRINT_DDL_SCHEMA_GRANT:
            {
                STL_TRY( ztqPrintGrantSchema( sSchemaID,
                                              sStringDDL,
                                              aAllocator,
                                              aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SCHEMA_COMMENT:
            {
                STL_TRY( ztqPrintCommentSchema( sSchemaID,
                                                sStringDDL,
                                                aAllocator,
                                                aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT( 0 );
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztqCmdPrintPublicSynonymDDL( ztqParseTree  * aParseTree,
                                       stlChar       * aCmdStr,
                                       stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    stlInt64    sSynonymID = 0;

    /**
     * Object ID 획득
     */

    STL_TRY( ztqGetPublicSynonymID( sCmdTree->mIdentifierList,
                                    aAllocator,
                                    & sSynonymID,
                                    aErrorStack )
             == STL_SUCCESS );
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * DDL 출력 
     */

    STL_TRY( ztqPrintCreatePublicSynonym( sSynonymID,
                                          sStringDDL,
                                          aAllocator,
                                          aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqCmdPrintTableDDL( ztqParseTree  * aParseTree,
                               stlChar       * aCmdStr,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    stlInt64    sTableID = 0;

    /**
     * Object ID 획득
     */

    STL_TRY( ztqGetTableID( sCmdTree->mIdentifierList,
                            aAllocator,
                            & sTableID,
                            aErrorStack )
             == STL_SUCCESS );
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Option 에 따른 DDL 출력 
     */
    
    switch ( sCmdTree->mDDLType )
    {
        case  ZTQ_PRINT_DDL_TABLE_CREATE:
            {
                STL_TRY( ztqPrintCreateTable( sTableID,
                                              sStringDDL,
                                              aAllocator,
                                              aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_TABLE_CONSTRAINT:
            {
                STL_TRY( ztqPrintTableConstraint( sTableID,
                                                  sStringDDL,
                                                  aAllocator,
                                                  aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_TABLE_INDEX:
            {
                STL_TRY( ztqPrintTableCreateIndex( sTableID,
                                                   sStringDDL,
                                                   aAllocator,
                                                   aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_TABLE_IDENTITY:
            {
                STL_TRY( ztqPrintTableIdentity( sTableID,
                                                sStringDDL,
                                                aAllocator,
                                                aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_TABLE_SUPPLEMENTAL:
            {
                STL_TRY( ztqPrintTableSupplemental( sTableID,
                                                    sStringDDL,
                                                    aAllocator,
                                                    aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_TABLE_GRANT:
            {
                STL_TRY( ztqPrintGrantRelation( sTableID,
                                                sStringDDL,
                                                aAllocator,
                                                aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_TABLE_COMMENT:
            {
                STL_TRY( ztqPrintCommentRelation( sTableID,
                                                  sStringDDL,
                                                  aAllocator,
                                                  aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqCmdPrintConstraintDDL( ztqParseTree  * aParseTree,
                                    stlChar       * aCmdStr,
                                    stlAllocator  * aAllocator,
                                    stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    stlInt64    sConstID = 0;

    /**
     * Object ID 획득
     */

    STL_TRY( ztqGetConstraintID( sCmdTree->mIdentifierList,
                                 aAllocator,
                                 & sConstID,
                                 aErrorStack )
             == STL_SUCCESS );
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Option 에 따른 DDL 출력 
     */
    
    switch ( sCmdTree->mDDLType )
    {
        case  ZTQ_PRINT_DDL_CONSTRAINT_ALTER:
            {
                STL_TRY( ztqPrintAddConstraint( sConstID,
                                                sStringDDL,
                                                aAllocator,
                                                aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_CONSTRAINT_COMMENT:
            {
                STL_TRY( ztqPrintCommentConstraint( sConstID,
                                                    sStringDDL,
                                                    aAllocator,
                                                    aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqCmdPrintIndexDDL( ztqParseTree  * aParseTree,
                               stlChar       * aCmdStr,
                               stlAllocator  * aAllocator,
                               stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    stlInt64    sIndexID = 0;

    /**
     * Object ID 획득
     */

    STL_TRY( ztqGetIndexID( sCmdTree->mIdentifierList,
                            aAllocator,
                            & sIndexID,
                            aErrorStack )
             == STL_SUCCESS );
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Option 에 따른 DDL 출력 
     */
    
    switch ( sCmdTree->mDDLType )
    {
        case  ZTQ_PRINT_DDL_INDEX_CREATE:
            {
                STL_TRY( ztqPrintCreateIndex( sIndexID,
                                              sStringDDL,
                                              aAllocator,
                                              aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_INDEX_COMMENT:
            {
                STL_TRY( ztqPrintCommentIndex( sIndexID,
                                               sStringDDL,
                                               aAllocator,
                                               aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqCmdPrintViewDDL( ztqParseTree  * aParseTree,
                              stlChar       * aCmdStr,
                              stlAllocator  * aAllocator,
                              stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    stlInt64    sViewID = 0;

    /**
     * Object ID 획득
     */

    STL_TRY( ztqGetViewID( sCmdTree->mIdentifierList,
                           aAllocator,
                           & sViewID,
                           aErrorStack )
             == STL_SUCCESS );
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Option 에 따른 DDL 출력 
     */
    
    switch ( sCmdTree->mDDLType )
    {
        case ZTQ_PRINT_DDL_VIEW_CREATE:
            {
                STL_TRY( ztqPrintCreateView( sViewID,
                                             sStringDDL,
                                             aAllocator,
                                             aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_VIEW_GRANT:
            {
                STL_TRY( ztqPrintGrantRelation( sViewID,
                                                sStringDDL,
                                                aAllocator,
                                                aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_VIEW_COMMENT:
            {
                STL_TRY( ztqPrintCommentRelation( sViewID,
                                                  sStringDDL,
                                                  aAllocator,
                                                  aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqCmdPrintSequenceDDL( ztqParseTree  * aParseTree,
                                  stlChar       * aCmdStr,
                                  stlAllocator  * aAllocator,
                                  stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    stlInt64    sSeqID = 0;

    /**
     * Object ID 획득
     */

    STL_TRY( ztqGetSequenceID( sCmdTree->mIdentifierList,
                               aAllocator,
                               & sSeqID,
                               aErrorStack )
             == STL_SUCCESS );
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Option 에 따른 DDL 출력 
     */
    
    switch ( sCmdTree->mDDLType )
    {
        case ZTQ_PRINT_DDL_SEQUENCE_CREATE:
            {
                STL_TRY( ztqPrintCreateSequence( sSeqID,
                                                 sStringDDL,
                                                 aAllocator,
                                                 aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SEQUENCE_RESTART:
            {
                STL_TRY( ztqPrintRestartSequence( sSeqID,
                                                  sStringDDL,
                                                  aAllocator,
                                                  aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SEQUENCE_GRANT:
            {
                STL_TRY( ztqPrintGrantSequence( sSeqID,
                                                sStringDDL,
                                                aAllocator,
                                                aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        case ZTQ_PRINT_DDL_SEQUENCE_COMMENT:
            {
                STL_TRY( ztqPrintCommentSequence( sSeqID,
                                                  sStringDDL,
                                                  aAllocator,
                                                  aErrorStack )
                         == STL_SUCCESS );
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqCmdPrintSynonymDDL( ztqParseTree  * aParseTree,
                                 stlChar       * aCmdStr,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    ztqCmdDDLTree * sCmdTree =  (ztqCmdDDLTree *) aParseTree;

    stlChar   * sStringDDL   = NULL;
    
    stlInt64    sSynonymID = 0;

    /**
     * Object ID 획득
     */

    STL_TRY( ztqGetSynonymID( sCmdTree->mIdentifierList,
                              aAllocator,
                              & sSynonymID,
                              aErrorStack )
             == STL_SUCCESS );
    
    /**
     * DDL 공간 할당
     */
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                gZtqDdlSize,
                                (void**) & sStringDDL,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * DDL 출력 
     */

    STL_TRY( ztqPrintCreateSynonym( sSynonymID,
                                    sStringDDL,
                                    aAllocator,
                                    aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief '\\EDIT' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 마지막으로 수행한 SQL문의 편집을 수행한다.
 */
stlStatus ztqCmdEdit( ztqParseTree  * aParseTree,
                      stlChar       * aCmdStr,
                      stlAllocator  * aAllocator,
                      stlErrorStack * aErrorStack )
{
    ztqCmdEditTree     *sParseTree;
    stlChar             sEditorName[STL_MAX_FILE_PATH_LENGTH];
    stlBool             sIsFound;

    sParseTree = (ztqCmdEditTree*)aParseTree;

    STL_TRY( stlGetEnv( sEditorName,
                        STL_MAX_FILE_PATH_LENGTH,
                        ZTQ_EDITOR_ENV,
                        &sIsFound,
                        aErrorStack )
             == STL_SUCCESS );

    if( sIsFound == STL_FALSE )
    {
        stlStrncpy( sEditorName, ZTQ_DEFAULT_EDITOR, STL_MAX_FILE_PATH_LENGTH );
    }

    STL_TRY( ztqEditCommand( sEditorName,
                             sParseTree->mSqlFile,
                             sParseTree->mHistoryNum,
                             aAllocator,
                             aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief '\\SPOOL' 구문을 처리한다.
 * @param[in]     aParseTree  Parse Tree 구조체
 * @param[in]     aCmdStr     원본 Commnand 문자열
 * @param[in]     aAllocator  메모리 Allocator
 * @param[in,out] aErrorStack Error Stack 구조체
 * @remarks 실행 결과를 File로 저장하는 Spool 기능을 수행한다.
 */
stlStatus ztqCmdSpool( ztqParseTree  * aParseTree,
                       stlChar       * aCmdStr,
                       stlAllocator  * aAllocator,
                       stlErrorStack * aErrorStack )
{
    ztqCmdSpoolTree    *sParseTree;

    sParseTree = (ztqCmdSpoolTree*)aParseTree;

    STL_TRY( ztqSpoolCommand( sParseTree->mSpoolFile,
                              sParseTree->mSpoolFlag,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
