/*******************************************************************************
 * ztqExecute.c
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
#include <ztqExecute.h>
#include <ztqDisplay.h>
#include <ztqOdbcBridge.h>
#include <ztqHostVariable.h>
#include <ztqNamedCursor.h>
#include <ztqNamedStmt.h>

#define ZTQ_AS_OPTION_SIZE (24)

SQLHDBC           gZtqDbcHandle = NULL;
SQLHENV           gZtqEnvHandle = NULL;
stlBool           gZtqConnectionEstablished = STL_FALSE;


extern stlInt32          gZtqPageSize;
extern stlInt32          gZtqColSize;
extern stlInt32          gZtqNumSize;
extern stlBool           gZtqTiming;
extern stlBool           gZtqPrintResult;
extern dtlCharacterSet   gZtqCharacterSet;
extern ztqSqlStmt      * gZtqSqlStmt;
extern stlVarInt         gZtqOdbcVersion;
extern stlBool           gZtqAdmin;
extern stlBool           gZtqSystemAdmin;
extern stlChar           gZtqConnString[];
extern stlBool           gZtqAutocommit;

extern dtlDateTimeFormatInfo  * gZtqNLSDateFormatInfo;
extern dtlDateTimeFormatInfo  * gZtqNLSTimeFormatInfo;
extern dtlDateTimeFormatInfo  * gZtqNLSTimeWithTimeZoneFormatInfo;
extern dtlDateTimeFormatInfo  * gZtqNLSTimestampFormatInfo;
extern dtlDateTimeFormatInfo  * gZtqNLSTimestampWithTimeZoneFormatInfo;


/**
 * @file ztqExecute.c
 * @brief Execute Routines
 */

/**
 * @addtogroup ztqExecute
 * @{
 */


stlTime gZtqBeginTime;
stlTime gZtqEndTime;
stlTime gZtqPrepareBeginTime;
stlTime gZtqPrepareEndTime;
stlTime gZtqExecuteBeginTime;
stlTime gZtqExecuteEndTime;
stlTime gZtqFetchBeginTime;
stlTime gZtqFetchEndTime;
stlTime gZtqBindBeginTime;
stlTime gZtqBindEndTime;


stlStatus ztqOpenDatabase( stlChar       * aDsn,
                           stlChar       * aUserId,
                           stlChar       * aPasswd,
                           stlChar       * aNewPassword,
                           stlBool         aStartUp,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack )
{
    stlInt32         sNativeError = 0;
    stlInt32         sTempNativeError;
    stlBool          sAllocHeap = STL_FALSE;
    stlChar        * sPasswd = NULL;
    stlChar          sPasswdBuffer[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          sNewPasswdBuffer1[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar          sNewPasswdBuffer2[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlSize          sSize;
    SQLSMALLINT      sRecNumber;
    ztqOperationMode sOperationMode;

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

    if( gZtqDbcHandle == NULL )
    {
        STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_DBC,
                                    gZtqEnvHandle,
                                    &gZtqDbcHandle,
                                    aErrorStack )
                 == STL_SUCCESS );
    }

    if( gZtqAdmin == STL_FALSE )
    {
        if( stlStrlen( gZtqConnString ) == 0 )
        {
            sPasswd = aPasswd;
            
            STL_RAMP( RAMP_CONNECT_RETRY );
            
            if( ztqSQLConnect( gZtqDbcHandle,
                               (SQLCHAR*)aDsn,
                               SQL_NTS,
                               (SQLCHAR*)aUserId,
                               SQL_NTS,
                               (SQLCHAR*)sPasswd,
                               SQL_NTS,
                               aAllocator,
                               aErrorStack ) == STL_FAILURE )
            {
                for( sRecNumber = 1; STL_TRUE; sRecNumber ++ )
                {
                    sTempNativeError = ztqGetNativeErrorByRecNumber( gZtqDbcHandle,
                                                                     SQL_HANDLE_DBC,
                                                                     sRecNumber,
                                                                     aErrorStack );
                    if( sTempNativeError == 0 )
                    {
                        break;
                    }
                    sNativeError = sTempNativeError;
                }

                /* the password has expired */
                STL_TRY( sNativeError == 16312 );

                ztqPrintf( "\n\nChanging password for %s\n", aUserId );

                if ( aNewPassword == NULL )
                {
#ifdef STL_VALGRIND
                    stlStrcpy( sNewPasswdBuffer1, "invalid1" );
                    stlStrcpy( sNewPasswdBuffer2, "invalid2" );
#else
                    STL_TRY( stlGetPassword( "New password: ",
                                             STL_FALSE,
                                             sNewPasswdBuffer1,
                                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                                             aErrorStack ) == STL_SUCCESS );
                    
                    STL_TRY( stlGetPassword( "Retype new password: ",
                                             STL_FALSE,
                                             sNewPasswdBuffer2,
                                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                                             aErrorStack ) == STL_SUCCESS );
#endif
                }
                else
                {
                    /* for regression test */
                    stlStrcpy( sNewPasswdBuffer1, aNewPassword );
                    stlStrcpy( sNewPasswdBuffer2, aNewPassword );
                }
                    
                STL_TRY_THROW( stlStrcmp( sNewPasswdBuffer1, sNewPasswdBuffer2 ) == 0,
                               RAMP_ERR_CONFIRMATION_PASSWORD_MISMATCH );
                
                STL_TRY( ztqSQLSetConnectAttr( gZtqDbcHandle,
                                               SQL_ATTR_OLDPWD,
                                               sPasswd,
                                               SQL_NTS,
                                               aErrorStack ) == STL_SUCCESS );
                
                sPasswd = sNewPasswdBuffer1;

                STL_THROW( RAMP_CONNECT_RETRY );
            }
        }
        else
        {
            STL_TRY( ztqSQLDriverConnect( gZtqDbcHandle,
                                          (SQLCHAR*)gZtqConnString,
                                          SQL_NTS,
                                          aAllocator,
                                          aErrorStack )
                     == STL_SUCCESS );
        }
        
        gZtqConnectionEstablished = STL_TRUE;
    }
    else
    {
        if( aPasswd != NULL )
        {
            sSize = stlStrlen( aPasswd ) + ZTQ_AS_OPTION_SIZE;
        }
        else
        {
            sSize = ZTQ_AS_OPTION_SIZE;
        }

        if( sSize > STL_MAX_SQL_IDENTIFIER_LENGTH )
        {
            STL_TRY( stlAllocHeap( (void**)&sPasswd,
                                   sSize,
                                   aErrorStack )
                     == STL_SUCCESS );
            sAllocHeap = STL_TRUE;
        }
        else
        {
            sPasswd = sPasswdBuffer;
        }
        
        if( gZtqSystemAdmin == STL_TRUE )
        {
            if( aPasswd != NULL )
            {
                stlSnprintf( sPasswd, sSize, "%s AS ADMIN", aPasswd );
            }
            else
            {
                stlSnprintf( sPasswd, sSize, "AS ADMIN" );
            }
        }
        else
        {
            if( aPasswd != NULL )
            {
                stlSnprintf( sPasswd, sSize, "%s AS SYSDBA", aPasswd );
            }
            else
            {
                stlSnprintf( sPasswd, sSize, "AS SYSDBA" );
            }
        }

        if( aStartUp == STL_TRUE )
        {
            /* CS mode에서 start-up이 호출될 경우 */
            stlStrcat( sPasswd, " WITH STARTUP" );
        }

        if( ztqSQLConnect( gZtqDbcHandle,
                           (SQLCHAR*)aDsn,
                           SQL_NTS,
                           (SQLCHAR*)aUserId,
                           SQL_NTS,
                           (SQLCHAR*)sPasswd,
                           SQL_NTS,
                           aAllocator,
                           aErrorStack ) == STL_FAILURE )
        {
            gZtqConnectionEstablished = STL_FALSE;

            for( sRecNumber = 1; STL_TRUE; sRecNumber ++ )
            {
                sTempNativeError = ztqGetNativeErrorByRecNumber( gZtqDbcHandle,
                                                                 SQL_HANDLE_DBC,
                                                                 sRecNumber,
                                                                 aErrorStack );
                if( sTempNativeError == 0 )
                {
                    break;
                }
                sNativeError = sTempNativeError;
            }
            /*
            sNativeError = ztqGetNativeTopError( gZtqDbcHandle,
                                                 SQL_HANDLE_DBC,
                                                 aErrorStack );
            */

            /*
             * Native Error가 13051/11031/25001 이라면 에러를 무시한다.
             * 13051: Unable to attache the shared memory segment. File does not exist
             *        - KNL_ERRCODE_FAIL_ATTACH_FILE_NOT_EXIST
             *        - Attach 정보가 들어있는 파일이 존재하는 않음.
             * 11031: Unable to attach the shared memory segment
             *        - STL_ERRCODE_SHM_ATTACH
             *        - DA로 접근할 때 connection 실패
             * 25001: Server is not running
             *        - SLL_ERRCODE_SERVER_IS_NOT_RUNNING
             *        - CS로 접근할 때, Server가 동작중이지 않음
             */
            switch( sNativeError )
            {
                case 13051:
                case 11031:
                    sOperationMode = ZTQ_OPERATION_MODE_DA;
                    break;
                case 25001:
                    sOperationMode = ZTQ_OPERATION_MODE_CS;
                    break;
                default:
                    STL_THROW( RAMP_ERR_ADMIN_CONNECT );
            }

            STL_TRY_THROW( sOperationMode == gZtqOperationMode, RAMP_ERR_MISMATCH_DRIVER );
        }
        else
        {
            gZtqConnectionEstablished = STL_TRUE;
        }

        if( sAllocHeap == STL_TRUE )
        {
            sAllocHeap = STL_FALSE;
            stlFreeHeap( sPasswd );
            sPasswd = NULL;
        }
    }

    /*
     * gsql은 AUTOCOMMIT OFF을 기본으로 한다.
     */
    if( gZtqConnectionEstablished == STL_TRUE )
    {

        /*
         * datetime session format에 대한 to_char format info를 구성한다.
         * 
         * connection이후에 아래와 같은 속성이 변경되면, 
         * SET SESSION CHARACTERISTICS AS TRANSACTION ISOLATION LEVEL SERIALIZABLE;
         * nls datetime format build를 위해 사용한 transaction들이 남아 있어 오동작하게 됨.
         * 따라서, transaction을 종료시킨다.
         */ 
        STL_TRY( ztqExecuteAutocommit( STL_TRUE, /* aAutocommit */
                                       aErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY( ztqBuildNLSToCharFormatInfo( aAllocator,
                                              aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqExecuteAutocommit( gZtqAutocommit,
                                       aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_SERVICE_NOT_AVAILABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_SERVICE_NOT_AVAILABLE,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_MISMATCH_DRIVER )
    {
        if( gZtqOperationMode == ZTQ_OPERATION_MODE_DA )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZTQ_ERRCODE_MISMATCH_DRIVER,
                          NULL,
                          aErrorStack,
                          "CS" );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          ZTQ_ERRCODE_MISMATCH_DRIVER,
                          NULL,
                          aErrorStack,
                          "DA" );
        }
    }

    STL_CATCH( RAMP_ERR_ADMIN_CONNECT )
    {
        (void)ztqPrintError( gZtqDbcHandle,
                             SQL_HANDLE_DBC,
                             aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_CONFIRMATION_PASSWORD_MISMATCH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_CONFIRMATION_PASSWORD_MISMATCH,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    /* set to not connected */
    gZtqConnectionEstablished = STL_FALSE;

    if( sAllocHeap == STL_TRUE )
    {
        stlFreeHeap( sPasswd );
    }
    else
    {
        /* do nothing */
    }

    if( gZtqDbcHandle != NULL )
    {
        (void)ztqSQLFreeHandle( SQL_HANDLE_DBC,
                                gZtqDbcHandle,
                                aErrorStack );
        gZtqDbcHandle = NULL;
    }
    else
    {
        /* do nothing */
    }
    
    if( gZtqEnvHandle != NULL )
    {
        (void)ztqSQLFreeHandle( SQL_HANDLE_ENV,
                                gZtqEnvHandle,
                                aErrorStack );
        gZtqEnvHandle = NULL;
    }
    else
    {
        /* do nothing */
    }
    
    return STL_FAILURE;
}

stlStatus ztqCloseDatabase( stlErrorStack * aErrorStack )
{
    /*
     * 정상 종료시 아직 커밋되지 않은 명령어가 살아 있다면
     * 강제 커밋시킨다.
     */
    if( gZtqConnectionEstablished == STL_TRUE )
    {
        (void)ztqSQLEndTran( SQL_HANDLE_DBC,
                             gZtqDbcHandle,
                             SQL_COMMIT,
                             STL_TRUE,
                             aErrorStack );
    }

    ztqDestAllNamedCursor( aErrorStack );
    ztqDestAllNamedStmt( aErrorStack );
    
    STL_TRY( ztqDestPreparedStmt( aErrorStack ) == STL_SUCCESS );
    
    if( gZtqConnectionEstablished == STL_TRUE )
    {
        STL_TRY( ztqSQLDisconnect( gZtqDbcHandle,
                                   aErrorStack )
                 == STL_SUCCESS );
        gZtqConnectionEstablished = STL_FALSE;
    }

    if( gZtqDbcHandle != NULL )
    {
        STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_DBC,
                                   gZtqDbcHandle,
                                   aErrorStack )
                 == STL_SUCCESS );
        gZtqDbcHandle = NULL;
    }

    if( gZtqEnvHandle != NULL )
    {
        STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_ENV,
                                   gZtqEnvHandle,
                                   aErrorStack )
                 == STL_SUCCESS );
        gZtqEnvHandle = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqAllocSqlStmt( ztqSqlStmt    ** aZtqSqlStmt,
                           ztqParseTree   * aParseTree,
                           SQLHSTMT         aOdbcStmt,
                           stlAllocator   * aAllocator,
                           stlErrorStack  * aErrorStack )
{
    ztqSqlStmt    * sZtqSqlStmt;
    ztqBypassTree * sParseTree;
    stlBool         sIsHeapAlloc = STL_FALSE;

    sParseTree = (ztqBypassTree*)aParseTree;
    
    if( aAllocator != NULL )
    {
        STL_TRY( stlAllocRegionMem( aAllocator,
                                    STL_SIZEOF( ztqSqlStmt ),
                                    (void**)&sZtqSqlStmt,
                                    aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( stlAllocHeap( (void**)&sZtqSqlStmt,
                               STL_SIZEOF( ztqSqlStmt ),
                               aErrorStack )
                 == STL_SUCCESS );

        sIsHeapAlloc = STL_TRUE;
    }

    sZtqSqlStmt->mOdbcStmt = aOdbcStmt;
    sZtqSqlStmt->mStatementType = sParseTree->mStatementType;
    sZtqSqlStmt->mObjectType = sParseTree->mObjectType;
    if ( sParseTree->mObjectName != NULL )
    {
        stlStrncpy( sZtqSqlStmt->mObjectName, sParseTree->mObjectName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    else
    {
        sZtqSqlStmt->mObjectName[0] = '\0';
    }
    sZtqSqlStmt->mHostVarList = NULL;
    sZtqSqlStmt->mExplainPlan = STL_FALSE;

    if( sParseTree->mHostVarList != NULL )
    {
        if( aAllocator != NULL )
        {
            /*
             * Region memory를 사용해야 하는 경우는 파스트리의 호스트 리스트를
             * ztqSqlStmt에 바로 연결한다.
             * - Region memory를 사용한다는 것은 PrepareExecute로 수행됨을 가정한다.
             * - 즉, 한번 사용되고 명령어가 종료되는 시점에 사라질 메모리이기 때문에
             *   재할당할 이유가 없다.
             */
            sZtqSqlStmt->mHostVarList = sParseTree->mHostVarList;
        }
        else
        {
            /*
             * Dynamic memory를 사용해야 하는 경우는 파스트리의 호스트 리스트를 복사해서
             * ztqSqlStmt에 연결해야 한다.
             * - Dynamic memory를 사용한다는 것은 1번의 Prepare로 N번의 Execute를 가정한다.
             * - 즉, 명령어가 종료되더라도 메모리는 유지되어야 한다.
             */

            STL_TRY( ztqCloneHostVarList( sParseTree->mHostVarList,
                                          &sZtqSqlStmt->mHostVarList,
                                          aErrorStack )
                     == STL_SUCCESS );
        }
    }

    *aZtqSqlStmt = sZtqSqlStmt;

    return STL_SUCCESS;

    STL_FINISH;

    /* codesonar */
    if( sIsHeapAlloc == STL_TRUE )
    {
        stlFreeHeap( sZtqSqlStmt );
    }

    return STL_FAILURE;
}

stlStatus ztqDestPreparedStmt( stlErrorStack * aErrorStack )
{
    if( gZtqSqlStmt != NULL )
    {
        if( gZtqSqlStmt->mOdbcStmt != NULL )
        {
            STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                       gZtqSqlStmt->mOdbcStmt,
                                       aErrorStack )
                     == STL_SUCCESS );
        }
        
        STL_TRY( ztqFreeSqlStmt( gZtqSqlStmt,
                                 aErrorStack )
                 == STL_SUCCESS );
        
        gZtqSqlStmt = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqFreeSqlStmt( ztqSqlStmt    * aZtqSqlStmt,
                          stlErrorStack * aErrorStack )
{
    ztqHostVariable * sHostVar;
    ztqHostVariable * sNextHostVar;
    
    sHostVar = aZtqSqlStmt->mHostVarList;

    while( sHostVar != NULL )
    {
        sNextHostVar = (ztqHostVariable*)sHostVar->mNext;
        stlFreeHeap( sHostVar );
        sHostVar = sNextHostVar;
    }
    
    stlFreeHeap( aZtqSqlStmt );

    return STL_SUCCESS;
}

stlStatus ztqPrepareSql( stlChar       * aSqlString,
                         stlAllocator  * aAllocator,
                         ztqSqlStmt    * aZtqSqlStmt,
                         stlErrorStack * aErrorStack )
{
    SQLHSTMT          sStmtHandle;
    ztqHostVariable * sHostVariable;
    ztqHostVariable * sTargetHostVariable;

    STL_PARAM_VALIDATE( aZtqSqlStmt != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aZtqSqlStmt->mOdbcStmt != NULL, aErrorStack );
    
    STL_TRY_THROW( aZtqSqlStmt->mStatementType < ZTQ_STMT_MAX,
                   RAMP_ERR_NOT_IMPLEMENTED );

    sStmtHandle = aZtqSqlStmt->mOdbcStmt;

    ZTQ_TIME_NOW( gZtqPrepareBeginTime );

    STL_TRY( ztqSQLPrepare( sStmtHandle,
                            (SQLCHAR*)aSqlString,
                            stlStrlen( aSqlString ),
                            aAllocator,
                            aErrorStack )
             == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqPrepareEndTime );

    sHostVariable = aZtqSqlStmt->mHostVarList;

    /*
     * 호스트 변수 존재 확인
     */
    
    while( sHostVariable != NULL )
    {
        STL_TRY( ztqGetHostVariable( sHostVariable->mName,
                                     &sTargetHostVariable,
                                     aErrorStack )
                 == STL_SUCCESS );
    
        sHostVariable = (ztqHostVariable*)sHostVariable->mNext;
    }

    aZtqSqlStmt->mOdbcStmt = sStmtHandle;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_SUPPORTED,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
    
}

stlStatus ztqExecuteSql( stlChar       * aSqlString,
                         stlAllocator  * aAllocator,
                         ztqSqlStmt    * aZtqSqlStmt,
                         stlBool         aDirectExecute,
                         stlBool         aPrintResult,
                         stlErrorStack * aErrorStack )
{
    SQLHSTMT          sStmtHandle;
    SQLINTEGER        i;
    ztqTargetColumn * sTargetColumn;
    SQLLEN            sRowCount = 0;          
    stlInt64          sLineFetchCount = 0;
    SQLRETURN         sResult;
    ztqRowBuffer    * sRowBuffer;
    SQLSMALLINT       sColumnCount;

    stlRegionMark     sMark;
    stlBool           sSetMark = STL_FALSE;
    stlChar         * sPlanText = NULL;
    SQLINTEGER        sBufferLength;
    SQLINTEGER        sStringLength = 0;

    STL_TRY_THROW( aZtqSqlStmt != NULL, RAMP_ERR_NOT_PREPARED );
    STL_TRY_THROW( aZtqSqlStmt->mOdbcStmt != NULL, RAMP_ERR_NOT_PREPARED );

    sStmtHandle = aZtqSqlStmt->mOdbcStmt;
    
    /*
     * 호스트 변수에 대응되는 파리미터들을 바인딩한다.
     */
    
    if( aZtqSqlStmt->mHostVarList != NULL )
    {
        STL_TRY( ztqBindParameters( sStmtHandle,
                                    aZtqSqlStmt->mHostVarList,
                                    aAllocator,
                                    aErrorStack )
                 == STL_SUCCESS );
    }
    
    ZTQ_TIME_NOW( gZtqExecuteBeginTime );

    if( aDirectExecute == STL_FALSE )
    {
        STL_TRY( ztqSQLExecute( sStmtHandle,
                                &sResult,
                                aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqSQLExecDirect( sStmtHandle,
                                   (SQLCHAR*)aSqlString,
                                   SQL_NTS,
                                   &sResult,
                                   aAllocator,
                                   aErrorStack ) == STL_SUCCESS );
    }

    ZTQ_TIME_NOW( gZtqExecuteEndTime );

   /*
     * SELECT Target 컬럼들을 위한 Column정보를 얻고,
     * Target column들을 바인딩한다.
     */
    STL_TRY( ztqSQLNumResultCols( sStmtHandle,
                                  &sColumnCount,
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztqBuildTargetColumns( sStmtHandle,
                                    sColumnCount,
                                    &sTargetColumn,
                                    aAllocator,
                                    aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqAllocRowBuffer( &sRowBuffer,
                                sColumnCount,
                                gZtqPageSize,
                                sTargetColumn,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLRowCount( sStmtHandle,
                             &sRowCount,
                             aErrorStack )
             == STL_SUCCESS );
    
    /*
     * 결과를 받아야 하는 경우가 아니라면 Fetch를 Skip한다.
     * ( INSERT/UPDATE/DELETE도 returning절을 사용할수 있다 )
     */
    STL_TRY_THROW( sColumnCount != 0, RAMP_SKIP_FETCH );
                                
    ZTQ_TIME_NOW( gZtqFetchBeginTime );

    sRowCount = 0;
    while( 1 )
    {
        STL_TRY_THROW( ztqSQLFetch( sStmtHandle,
                                    &sResult,
                                    aErrorStack ) == STL_SUCCESS,
                       RAMP_ERR_FETCH );

        if( sResult != SQL_NO_DATA )
        {
            STL_TRY( ztqMakeRowString( sRowBuffer,
                                       sLineFetchCount,
                                       sTargetColumn,
                                       aPrintResult,
                                       aErrorStack )
                     == STL_SUCCESS );
            
            sLineFetchCount += 1;
            sRowCount += 1;
        }

        if( (sLineFetchCount == gZtqPageSize) || (sResult == SQL_NO_DATA) )
        {
            /*
             * Line Buffer 가 꽉 차거나 종료 시점인 경우
             */
            for( i = 0; i < sLineFetchCount; i++ )
            {
                if( i == 0 )
                {
                    if( (aPrintResult == STL_TRUE) && (gZtqPrintResult == STL_TRUE) )
                    {
                        STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                                      sTargetColumn )
                                 == STL_SUCCESS );
                    }
                }
                
                if( (aPrintResult == STL_TRUE) && (gZtqPrintResult == STL_TRUE) )
                {
                    STL_TRY( ztqPrintRowString( sRowBuffer,
                                                sTargetColumn,
                                                i,
                                                aAllocator,
                                                aErrorStack )
                             == STL_SUCCESS );
                }
            }

            /*
             * Page에 결과가 한건이라도 있다면 현재 라인과 다음 라인을 분리함.
             */
            if( (aPrintResult == STL_TRUE) &&
                (gZtqPrintResult == STL_TRUE) &&
                (sLineFetchCount > 0) )
            {
                ztqPrintf( "\n" );
            }

            ztqSetInitRowStringLen( sRowBuffer, sTargetColumn );
            sLineFetchCount = 0;
        }

        if( sResult == SQL_NO_DATA )
        {
            STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                        aErrorStack ) == STL_SUCCESS );
            break;
        }
    }

    ZTQ_TIME_NOW( gZtqFetchEndTime );

    STL_RAMP( RAMP_SKIP_FETCH );

    if ( sRowCount == 1 )
    {
        STL_TRY( ztqPrintOutParamIfExist( sRowBuffer,
                                          aZtqSqlStmt->mHostVarList,
                                          aPrintResult,
                                          aAllocator,
                                          aErrorStack )
                 == STL_SUCCESS );
    }

    if ( aZtqSqlStmt->mStatementType == ZTQ_STMT_DECLARE_CURSOR )
    {
        STL_TRY( ztqAddNamedCursor( aZtqSqlStmt, aErrorStack ) == STL_SUCCESS );
    }
    
    if( aPrintResult == STL_TRUE )
    {
        STL_TRY( ztqPrintAnswerMessage( aZtqSqlStmt->mStatementType,
                                        aZtqSqlStmt->mObjectType,
                                        sRowCount,
                                        aErrorStack )
                 == STL_SUCCESS );
    }

    if( aZtqSqlStmt->mExplainPlan == STL_TRUE )
    {
        /*
         * Plan 출력
         */
        STL_TRY( ztqSQLGetStmtAttr( aZtqSqlStmt->mOdbcStmt,
                                    SQL_ATTR_EXPLAIN_PLAN_TEXT,
                                    NULL,
                                    0, 
                                    (SQLINTEGER *) &sStringLength,
                                    aErrorStack )
                 == STL_SUCCESS );

        if( sStringLength > 0 )
        {
            sBufferLength = sStringLength + 1;
            
            STL_TRY( stlMarkRegionMem( aAllocator,
                                       &sMark,
                                       aErrorStack ) == STL_SUCCESS );
            sSetMark = STL_TRUE;
        
            STL_TRY( stlAllocRegionMem( aAllocator,
                                        sBufferLength,
                                        (void**)&sPlanText,
                                        aErrorStack ) == STL_SUCCESS );

            stlMemset( sPlanText, 0x00, sBufferLength );
            
            STL_TRY( ztqSQLGetStmtAttr( aZtqSqlStmt->mOdbcStmt,
                                        SQL_ATTR_EXPLAIN_PLAN_TEXT,
                                        (SQLPOINTER)sPlanText,
                                        sBufferLength, 
                                        (SQLINTEGER *) &sStringLength,
                                        aErrorStack )
                     == STL_SUCCESS );
            
            ztqPrintf( "\n>>>  start print plan\n" );
            ztqPrintf( "\n%s\n", sPlanText );
            ztqPrintf( "<<<  end print plan\n\n" );

            sSetMark = STL_FALSE;
            STL_TRY( stlRestoreRegionMem( aAllocator,
                                          &sMark,
                                          STL_TRUE,
                                          aErrorStack ) == STL_SUCCESS );

            sPlanText = NULL;
        }
    }

    if( (aZtqSqlStmt->mStatementType == ZTQ_STMT_ALTER) &&
        (aZtqSqlStmt->mObjectType == ZTQ_OBJECT_SESSION) )
    {
        /*
         * NLS PROPERY 정보 구축
         */
        
        STL_TRY( ztqBuildNLSToCharFormatInfo( aAllocator,
                                              aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_NOT_PREPARED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_PREPARED,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FETCH )
    {
        (void)ztqSQLCloseCursor( sStmtHandle,
                                 aErrorStack );
    }
    
    STL_FINISH;

    if( sSetMark == STL_TRUE )
    {
        (void)stlRestoreRegionMem( aAllocator, &sMark, STL_TRUE, aErrorStack );
    }

    return STL_FAILURE;
    
}

stlStatus ztqPrepareExecuteSql( stlChar       * aSqlString,
                                stlAllocator  * aAllocator,
                                ztqParseTree  * aParseTree,
                                stlBool         aPrintResult,
                                stlErrorStack * aErrorStack )
{
    SQLHSTMT          sOdbcStmt;
    stlInt32          sState = 0;
    ztqSqlStmt      * sZtqSqlStmt;
    
    ztqClearElapsedTime();
    
    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sOdbcStmt,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    STL_TRY( ztqAllocSqlStmt( &sZtqSqlStmt,
                              aParseTree,
                              sOdbcStmt,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqBeginTime );

    STL_TRY( ztqPrepareSql( aSqlString,
                            aAllocator,
                            sZtqSqlStmt,
                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( ztqExecuteSql( aSqlString,
                            aAllocator,
                            sZtqSqlStmt,
                            STL_FALSE,   /* aDirectExecute */
                            aPrintResult,
                            aErrorStack )
             == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqEndTime );
    
    if( aPrintResult == STL_TRUE )
    {
        STL_TRY( ztqPrintElapsedTime( aErrorStack ) == STL_SUCCESS );
    }
    
    /*
     * Prepare & Execute에서는 Region Memory를 사용해 ztqSqlStmt를
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
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            ZTQ_TIME_NOW( gZtqEndTime );
                
            if( aPrintResult == STL_TRUE )
            {
                (void) ztqPrintElapsedTime( aErrorStack );
            }

            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sOdbcStmt,
                                    aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztqDirectExecuteSql( stlChar       * aSqlString,
                               stlAllocator  * aAllocator,
                               ztqParseTree  * aParseTree,
                               stlBool         aPrintResult,
                               stlErrorStack * aErrorStack )
{
    SQLHSTMT          sOdbcStmt;
    stlInt32          sState = 0;
    ztqSqlStmt      * sZtqSqlStmt;
    
    ztqClearElapsedTime();
    
    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sOdbcStmt,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;

    STL_TRY( ztqAllocSqlStmt( &sZtqSqlStmt,
                              aParseTree,
                              sOdbcStmt,
                              aAllocator,
                              aErrorStack )
             == STL_SUCCESS );

    sState = 2;
    ZTQ_TIME_NOW( gZtqBeginTime );

    STL_TRY( ztqExecuteSql( aSqlString,
                            aAllocator,
                            sZtqSqlStmt,
                            STL_TRUE,     /* aDirectExecute */
                            aPrintResult,
                            aErrorStack )
             == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqEndTime );
    
    if( aPrintResult == STL_TRUE )
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
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            ZTQ_TIME_NOW( gZtqEndTime );
            
            if( aPrintResult == STL_TRUE )
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

stlStatus ztqExecuteTran( stlBool         aIsCommit,
                          stlErrorStack * aErrorStack )
{
    SQLSMALLINT sIsCommit;

    ztqClearElapsedTime();
    
    if( aIsCommit == STL_TRUE )
    {
        sIsCommit = SQL_COMMIT;
    }
    else
    {
        sIsCommit = SQL_ROLLBACK;
    }
    
    ZTQ_TIME_NOW( gZtqBeginTime );
    
    STL_TRY( ztqSQLEndTran( SQL_HANDLE_DBC,
                            gZtqDbcHandle,
                            sIsCommit,
                            STL_FALSE,  /* aIgnoreFailure */
                            aErrorStack ) == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqEndTime );
    
    STL_TRY( ztqPrintTranAnswerMessage( aIsCommit,
                                        aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqPrintElapsedTime( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    ZTQ_TIME_NOW( gZtqEndTime );
    
    (void) ztqPrintElapsedTime( aErrorStack );
    
    return STL_FAILURE;
}

stlStatus ztqExecuteAutocommit( stlBool         aAutocommit,
                                stlErrorStack * aErrorStack )
{
    if( aAutocommit == STL_TRUE )
    {
        STL_TRY( ztqSQLSetConnectAttr( gZtqDbcHandle,
                                       SQL_ATTR_AUTOCOMMIT,
                                       (SQLPOINTER)SQL_AUTOCOMMIT_ON,
                                       SQL_IS_UINTEGER,
                                       aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztqSQLSetConnectAttr( gZtqDbcHandle,
                                       SQL_ATTR_AUTOCOMMIT,
                                       (SQLPOINTER)SQL_AUTOCOMMIT_OFF,
                                       SQL_IS_UINTEGER,
                                       aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqBindParameters( SQLHSTMT          aStmtHandle,
                             ztqHostVariable * aHostVarList,
                             stlAllocator    * aAllocator,
                             stlErrorStack   * aErrorStack )
{
    ztqHostVariable * sHostVariable;
    ztqHostVariable * sTargetHostVariable;
    ztqHostVariable * sIndicatorHostVariable;
    stlChar         * sEndPtr;
    stlInt64          sIndicatorValue;
    SQLUSMALLINT      sParamNumber = 1;
    stlInt32          sParamType = 0;

    SQLULEN           sColumnSize = 0;
    SQLSMALLINT       sDecimalDigits = 0;

    SQLSMALLINT       sBindValueType;       
    
    sHostVariable = aHostVarList;
    
    while( sHostVariable != NULL )
    {
        STL_TRY( ztqGetHostVariable( sHostVariable->mName,
                                     &sTargetHostVariable,
                                     aErrorStack )
                 == STL_SUCCESS );

        switch ( sHostVariable->mBindType )
        {
            case ZTQ_HOST_BIND_TYPE_IN:
                sParamType = SQL_PARAM_INPUT;
                break;
            case ZTQ_HOST_BIND_TYPE_OUT:
                sParamType = SQL_PARAM_OUTPUT;
                break;
            case ZTQ_HOST_BIND_TYPE_INDICATOR:
                sTargetHostVariable->mIndicatorVariable = sHostVariable;
                sHostVariable = (ztqHostVariable*)sHostVariable->mNext;
                continue;
            default:
                STL_ASSERT(0);
                break;
        }

        if( sHostVariable->mIndicatorVariable != NULL )
        {
            STL_TRY( ztqGetHostVariable( ((ztqHostVariable *)sHostVariable->mIndicatorVariable)->mName,
                                         &sIndicatorHostVariable,
                                         aErrorStack )
                     == STL_SUCCESS );

            if( sIndicatorHostVariable->mIndicator == SQL_NULL_DATA )
            {
                sIndicatorValue = SQL_NULL_DATA;
            }
            else
            {
                STL_TRY( stlStrToInt64( sIndicatorHostVariable->mValue,
                                        STL_NTS,
                                        &sEndPtr,
                                        10,
                                        &sIndicatorValue,
                                        aErrorStack )
                         == STL_SUCCESS );
            }
            sTargetHostVariable->mIndicator = sIndicatorValue;
        }

        STL_TRY( ztqGetParameterColSizeAndDecimalDigit(
                     sTargetHostVariable->mDataType,
                     sTargetHostVariable->mPrecision,
                     sTargetHostVariable->mScale,
                     & sColumnSize,
                     & sDecimalDigits,
                     aErrorStack )
                 == STL_SUCCESS );

        switch( sTargetHostVariable->mDataType )
        {
            case SQL_TYPE_DATE:
                sBindValueType = SQL_C_TYPE_DATE;
                break;
            case SQL_TYPE_TIME:
            case SQL_TYPE_TIME_WITH_TIMEZONE:
                sBindValueType = SQL_C_TYPE_TIME_WITH_TIMEZONE;
                break;
            case SQL_TYPE_TIMESTAMP:
                sBindValueType = SQL_C_TYPE_TIMESTAMP;
                break;
            case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE:
                sBindValueType = SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE;
                break;
            case SQL_INTERVAL_YEAR:
            case SQL_INTERVAL_MONTH:
            case SQL_INTERVAL_YEAR_TO_MONTH:
                sBindValueType = SQL_C_INTERVAL_YEAR_TO_MONTH;
                break;
            case SQL_INTERVAL_DAY:
            case SQL_INTERVAL_HOUR:
            case SQL_INTERVAL_MINUTE:
            case SQL_INTERVAL_SECOND:
            case SQL_INTERVAL_DAY_TO_HOUR:
            case SQL_INTERVAL_DAY_TO_MINUTE:
            case SQL_INTERVAL_DAY_TO_SECOND:
            case SQL_INTERVAL_HOUR_TO_MINUTE:
            case SQL_INTERVAL_HOUR_TO_SECOND:
            case SQL_INTERVAL_MINUTE_TO_SECOND:
                sBindValueType = SQL_C_INTERVAL_DAY_TO_SECOND;
                break;
            default:
                sBindValueType = SQL_C_CHAR;
                break;
        }
        
        STL_TRY( ztqSQLBindParameter( aStmtHandle,
                                      sParamNumber,
                                      sParamType,
                                      sBindValueType,
                                      sTargetHostVariable->mDataType,
                                      sColumnSize, 
                                      sDecimalDigits, 
                                      sTargetHostVariable->mValue,
                                      ZTQ_MAX_HOST_VALUE_SIZE,
                                      &sTargetHostVariable->mIndicator,
                                      aErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY( ztqSetCharacterLengthUnits(
                     aStmtHandle,
                     sParamNumber,
                     sTargetHostVariable->mDataType,
                     sTargetHostVariable->mStringLengthUnit,
                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( ztqSetIntervalLeadingPrecision(
                     aStmtHandle,
                     sParamNumber,
                     sTargetHostVariable->mDataType,
                     sTargetHostVariable->mPrecision,
                     aErrorStack )
                 == STL_SUCCESS );

        sParamNumber += 1;
        
        sHostVariable = (ztqHostVariable*)sHostVariable->mNext;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                                   
stlStatus ztqBuildTargetColumns( SQLHSTMT                    aStmtHandle,
                                 SQLSMALLINT                 aColumnCount,
                                 ztqTargetColumn          ** aTargetColumn,
                                 stlAllocator              * aAllocator,
                                 stlErrorStack             * aErrorStack )
{
    ztqTargetColumn * sTargetColumns;
    ztqTargetColumn * sTargetColumn;
    SQLSMALLINT       sNameLength;
    SQLSMALLINT       sDecimalDigits;
    SQLSMALLINT       sNullable;
    SQLSMALLINT       sBindValueType;    
    SQLHDESC          sDescIrd = NULL;
    SQLLEN            sDisplaySize;
    stlInt32          i;
    
    SQLHDESC          sDescArd = NULL;
    SQLSMALLINT       sPrecision;
    SQLINTEGER        sDatetimeIntervalPrecision;
    stlChar           sTypeName[STL_MAX_SQL_IDENTIFIER_LENGTH];    
    
    STL_TRY( ztqSQLGetStmtAttr( aStmtHandle,
                                SQL_ATTR_IMP_ROW_DESC,
                                &sDescIrd,
                                SQL_IS_POINTER,
                                NULL,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(ztqTargetColumn) * aColumnCount,
                                (void**)&sTargetColumns,
                                aErrorStack )
             == STL_SUCCESS );

    ZTQ_TIME_NOW( gZtqBindBeginTime );

    for( i = 1; i <= aColumnCount; i++ )
    {
        sTargetColumn = &sTargetColumns[i-1];
        
        STL_TRY( ztqSQLDescribeCol( aStmtHandle,
                                    i,
                                    (SQLCHAR*)sTargetColumn->mName,
                                    ZTQ_MAX_COLUMN_NAME,
                                    &sNameLength,
                                    &sTargetColumn->mDataType,
                                    &sTargetColumn->mDataSize,
                                    &sDecimalDigits,
                                    &sNullable,
                                    aAllocator,
                                    aErrorStack )
                 == STL_SUCCESS );

        /*
         * DISPLAY SIZE 결정
         */
        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     i,
                                     SQL_DESC_DISPLAY_SIZE,
                                     &sDisplaySize,
                                     SQL_IS_POINTER,
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY( ztqSQLGetDescField( sDescIrd,
                                     i,
                                     SQL_DESC_TYPE_NAME,
                                     sTypeName,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     NULL,
                                     aErrorStack )
                 == STL_SUCCESS );
        
        if( sDisplaySize == SQL_NO_TOTAL )
        {
            sTargetColumn->mDataSize = gZtqColSize + 1/* 종료문자 */;
        }
        else
        {
            sTargetColumn->mDtlDataType = DTL_TYPE_NA;
            
            switch( sTargetColumn->mDataType )
            {
                case SQL_TYPE_DATE:
                    sTargetColumn->mDataSize = STL_SIZEOF(SQL_DATE_STRUCT);
                    break;
                case SQL_TYPE_TIME:
                case SQL_TYPE_TIME_WITH_TIMEZONE:
                    sTargetColumn->mDataSize = STL_SIZEOF(SQL_TIME_WITH_TIMEZONE_STRUCT);
                    break;
                case SQL_TYPE_TIMESTAMP:
                    sTargetColumn->mDataSize = STL_SIZEOF(SQL_TIMESTAMP_STRUCT);

                    if( stlStrcmp( sTypeName, "DATE" ) == 0 )
                    {
                        sTargetColumn->mDtlDataType = DTL_TYPE_DATE;
                    }
                    else
                    {
                        sTargetColumn->mDtlDataType = DTL_TYPE_TIMESTAMP;
                    }
                    
                    break;
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE:
                    sTargetColumn->mDataSize = STL_SIZEOF(SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT);
                    break;
                case SQL_INTERVAL_YEAR:
                case SQL_INTERVAL_MONTH:
                case SQL_INTERVAL_YEAR_TO_MONTH:
                case SQL_INTERVAL_DAY:
                case SQL_INTERVAL_HOUR:
                case SQL_INTERVAL_MINUTE:
                case SQL_INTERVAL_SECOND:
                case SQL_INTERVAL_DAY_TO_HOUR:
                case SQL_INTERVAL_DAY_TO_MINUTE:
                case SQL_INTERVAL_DAY_TO_SECOND:
                case SQL_INTERVAL_HOUR_TO_MINUTE:
                case SQL_INTERVAL_HOUR_TO_SECOND:
                case SQL_INTERVAL_MINUTE_TO_SECOND:
                    sTargetColumn->mDataSize = STL_SIZEOF(SQL_INTERVAL_STRUCT);
                    break;
                default:
                    sTargetColumn->mDataSize = sDisplaySize;
                    break;
            }
            
            sTargetColumn->mDataSize = sTargetColumn->mDataSize + 1/* 종료문자 */;
        }
        
        /*
         * "select '' as T from dual;"과 같은 경우 mDataSize가 stlStrlen( 'T' )인 1로 된다.
         * 이때 해당 값이 NULL이면 아래 '* 2'에서 길이가 2로 되므로 'nu'까지만 출력된다.
         * 따라서, '* 2' 처리 이후 mDataSize가 ZTQ_NULL_STRING의 길이보다 작으면
         * ZTQ_NULL_STRING의 길이로 셋팅하도록 한다.
         */
        if( sTargetColumn->mDataSize < stlStrlen( ZTQ_NULL_STRING ) )
        {
            sTargetColumn->mDataSize = stlStrlen( ZTQ_NULL_STRING ) + 1/* 종료문자 */;
        }

        /*
         * 컬럼의 포맷 스트링 결정
         */
        if( (sTargetColumn->mDataType == SQL_CHAR)           ||
            (sTargetColumn->mDataType == SQL_VARCHAR)        ||
            (sTargetColumn->mDataType == SQL_LONGVARCHAR)    ||
            (sTargetColumn->mDataType == SQL_WCHAR)          ||
            (sTargetColumn->mDataType == SQL_WVARCHAR)       ||
            (sTargetColumn->mDataType == SQL_WLONGVARCHAR)   ||
            (sTargetColumn->mDataType == SQL_BINARY)         ||
            (sTargetColumn->mDataType == SQL_VARBINARY)      ||
            (sTargetColumn->mDataType == SQL_LONGVARBINARY)  ||
            (sTargetColumn->mDataType == SQL_BOOLEAN)        ||
            (sTargetColumn->mDataType == SQL_TYPE_DATE)      ||
            (sTargetColumn->mDataType == SQL_TYPE_TIME)      ||
            (sTargetColumn->mDataType == SQL_TYPE_TIME_WITH_TIMEZONE) ||
            (sTargetColumn->mDataType == SQL_TYPE_TIMESTAMP) ||
            (sTargetColumn->mDataType == SQL_TYPE_TIMESTAMP_WITH_TIMEZONE) ||
            (sTargetColumn->mDataType == SQL_INTERVAL_YEAR)           ||
            (sTargetColumn->mDataType == SQL_INTERVAL_MONTH)          ||
            (sTargetColumn->mDataType == SQL_INTERVAL_DAY)            ||
            (sTargetColumn->mDataType == SQL_INTERVAL_HOUR)           ||
            (sTargetColumn->mDataType == SQL_INTERVAL_MINUTE)         ||
            (sTargetColumn->mDataType == SQL_INTERVAL_SECOND)         ||
            (sTargetColumn->mDataType == SQL_INTERVAL_YEAR_TO_MONTH)  ||
            (sTargetColumn->mDataType == SQL_INTERVAL_DAY_TO_HOUR)    ||
            (sTargetColumn->mDataType == SQL_INTERVAL_DAY_TO_MINUTE)  ||
            (sTargetColumn->mDataType == SQL_INTERVAL_DAY_TO_SECOND)  ||
            (sTargetColumn->mDataType == SQL_INTERVAL_HOUR_TO_MINUTE) ||                        
            (sTargetColumn->mDataType == SQL_INTERVAL_HOUR_TO_SECOND) ||
            (sTargetColumn->mDataType == SQL_INTERVAL_MINUTE_TO_SECOND) )
        {
            stlStrcpy( sTargetColumn->mColumnFormat,
                       ZTQ_COLUMN_FORMAT_CLASS_STRING );
            sTargetColumn->mDataTypeGroup = ZTQ_GROUP_STRING;
        }
        else
        {
            stlStrcpy( sTargetColumn->mColumnFormat,
                       ZTQ_COLUMN_FORMAT_CLASS_NUMERIC );
            sTargetColumn->mDataTypeGroup = ZTQ_GROUP_NUMERIC;
        }

        STL_TRY( stlAllocRegionMem( aAllocator,
                                    sTargetColumn->mDataSize + 1,
                                    (void**)&sTargetColumn->mDataValue,
                                    aErrorStack )
                 == STL_SUCCESS );
        
        switch( sTargetColumn->mDataType )
        {
            case SQL_TYPE_DATE:
                sBindValueType = SQL_C_TYPE_DATE;
                break;
            case SQL_TYPE_TIME:
            case SQL_TYPE_TIME_WITH_TIMEZONE:
                sBindValueType = SQL_C_TYPE_TIME_WITH_TIMEZONE;
                break;
            case SQL_TYPE_TIMESTAMP:
                sBindValueType = SQL_C_TYPE_TIMESTAMP;
                break;
            case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE:
                sBindValueType = SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE;
                break;
            case SQL_INTERVAL_YEAR:
            case SQL_INTERVAL_MONTH:
            case SQL_INTERVAL_YEAR_TO_MONTH:
                sBindValueType = SQL_C_INTERVAL_YEAR_TO_MONTH;
                break;
            case SQL_INTERVAL_DAY:
            case SQL_INTERVAL_HOUR:
            case SQL_INTERVAL_MINUTE:
            case SQL_INTERVAL_SECOND:
            case SQL_INTERVAL_DAY_TO_HOUR:
            case SQL_INTERVAL_DAY_TO_MINUTE:
            case SQL_INTERVAL_DAY_TO_SECOND:
            case SQL_INTERVAL_HOUR_TO_MINUTE:
            case SQL_INTERVAL_HOUR_TO_SECOND:
            case SQL_INTERVAL_MINUTE_TO_SECOND:
                sBindValueType = SQL_C_INTERVAL_DAY_TO_SECOND;
                break;
            default:
                sBindValueType = SQL_C_CHAR;
                break;
        }
        
        STL_TRY( ztqSQLBindCol( aStmtHandle,
                                i,
                                sBindValueType,
                                sTargetColumn->mDataValue,
                                sTargetColumn->mDataSize,
                                &sTargetColumn->mIndicator,
                                aErrorStack )
                 == STL_SUCCESS );

        /*
         * interval type column에 대한 leading precision, second precision 정보를 얻고,
         * interval type column에 대한 SQL_DESC_DATETIME_INTERVAL_PRECISION 을 설정한다.
         */ 
        switch( sTargetColumn->mDataType )
        {
            case SQL_INTERVAL_YEAR:
            case SQL_INTERVAL_MONTH:
            case SQL_INTERVAL_YEAR_TO_MONTH:
            case SQL_INTERVAL_DAY:
            case SQL_INTERVAL_HOUR:
            case SQL_INTERVAL_MINUTE:
            case SQL_INTERVAL_SECOND:
            case SQL_INTERVAL_DAY_TO_HOUR:
            case SQL_INTERVAL_DAY_TO_MINUTE:
            case SQL_INTERVAL_DAY_TO_SECOND:
            case SQL_INTERVAL_HOUR_TO_MINUTE:
            case SQL_INTERVAL_HOUR_TO_SECOND:
            case SQL_INTERVAL_MINUTE_TO_SECOND:
                {
                    /*
                     * precision 값을 얻는다.
                     */ 
                    STL_TRY( ztqSQLGetDescField( sDescIrd,
                                                 (SQLSMALLINT)i,
                                                 SQL_DESC_PRECISION,
                                                 (SQLPOINTER) & sPrecision,
                                                 STL_SIZEOF( SQLSMALLINT ),
                                                 NULL,
                                                 aErrorStack )
                             == STL_SUCCESS );
                
                    STL_TRY( ztqSQLGetDescField( sDescIrd,
                                                 (SQLSMALLINT)i,
                                                 SQL_DESC_DATETIME_INTERVAL_PRECISION,
                                                 (SQLPOINTER) & sDatetimeIntervalPrecision,
                                                 STL_SIZEOF( SQLINTEGER ),
                                                 NULL,
                                                 aErrorStack )
                             == STL_SUCCESS );
                
                    sTargetColumn->mIntervalColInfo.mLeadingPrecision = sDatetimeIntervalPrecision;
                    sTargetColumn->mIntervalColInfo.mSecondPrecision  = sPrecision;

                    /*
                     * leading precision 값을 설정한다.
                     */
                    
                    STL_TRY( ztqSQLGetStmtAttr( aStmtHandle,
                                                SQL_ATTR_APP_ROW_DESC,
                                                & sDescArd,
                                                SQL_IS_POINTER,
                                                NULL,
                                                aErrorStack )
                             == STL_SUCCESS );
                    
                    STL_TRY( ztqSQLSetDescField( sDescArd,
                                                 (SQLSMALLINT)i,
                                                 SQL_DESC_DATETIME_INTERVAL_PRECISION,
                                                 (SQLPOINTER)((SQLLEN)sDatetimeIntervalPrecision),
                                                 0,
                                                 aErrorStack )
                             == STL_SUCCESS );
                    
                    STL_TRY( ztqSQLSetDescField( sDescArd,
                                                 (SQLSMALLINT)i,
                                                 SQL_DESC_DATA_PTR,
                                                 (SQLPOINTER)sTargetColumn->mDataValue,
                                                 0,
                                                 aErrorStack )
                             == STL_SUCCESS );
                    
                    break;
                }
            default:
                {
                    sTargetColumn->mIntervalColInfo.mLeadingPrecision = 0;
                    sTargetColumn->mIntervalColInfo.mSecondPrecision  = 0;
                    
                    break;
                }
        }
    }
    
    ZTQ_TIME_NOW( gZtqBindEndTime );

    *aTargetColumn = sTargetColumns;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqAllocRowBuffer( ztqRowBuffer   ** aRowBuffer,
                             SQLSMALLINT       aColumnCount,
                             stlInt32          aLineBufferCount,
                             ztqTargetColumn * aTargetColumn,
                             stlAllocator    * aAllocator,
                             stlErrorStack   * aErrorStack )
{
    ztqRowBuffer * sRowBuffer = NULL;
    stlInt32       i = 0;
    stlInt32       j = 0;
    

    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(ztqRowBuffer),
                                (void**)&sRowBuffer,
                                aErrorStack )
             == STL_SUCCESS );

    stlMemset( sRowBuffer, 0x00, STL_SIZEOF(ztqRowBuffer) );

    sRowBuffer->mColCnt  = aColumnCount;
    sRowBuffer->mLineCnt = aLineBufferCount;

    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(ztqColBuffer) * sRowBuffer->mColCnt,
                                (void **) & sRowBuffer->mColBuf,
                                aErrorStack )
             == STL_SUCCESS );
    stlMemset( sRowBuffer->mColBuf,
               0x00,
               STL_SIZEOF(ztqColBuffer) * sRowBuffer->mColCnt );

    /*
     * Column Buffer 할당
     */
    for ( i = 0; i < sRowBuffer->mColCnt; i++ )
    {
        sRowBuffer->mColBuf[i].mColName = aTargetColumn[i].mName;
        sRowBuffer->mColBuf[i].mMaxLength = stlStrlen( aTargetColumn[i].mName );
        sRowBuffer->mColBuf[i].mMaxLineCount = 1;
        sRowBuffer->mColBuf[i].mActualLineCount = 0;

        /*
         * Column Buffer의 Buffer Size는 NULL 문자를 위한 공간을 위해
         * Target Column의 DataSize + 1을 한다.
         */
        switch( aTargetColumn[i].mDataType )
        {
            case SQL_TYPE_DATE:
            case SQL_TYPE_TIME:
            case SQL_TYPE_TIME_WITH_TIMEZONE:
            case SQL_TYPE_TIMESTAMP:
            case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE:
                sRowBuffer->mColBuf[i].mBufSize = DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE + 1;
                break;
            case SQL_INTERVAL_YEAR:
            case SQL_INTERVAL_MONTH:
            case SQL_INTERVAL_YEAR_TO_MONTH:
                sRowBuffer->mColBuf[i].mBufSize = DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE + 1;
                break;
            case SQL_INTERVAL_DAY:
            case SQL_INTERVAL_HOUR:
            case SQL_INTERVAL_MINUTE:
            case SQL_INTERVAL_SECOND:                
            case SQL_INTERVAL_DAY_TO_HOUR:
            case SQL_INTERVAL_DAY_TO_MINUTE:
            case SQL_INTERVAL_DAY_TO_SECOND:
            case SQL_INTERVAL_HOUR_TO_MINUTE:
            case SQL_INTERVAL_HOUR_TO_SECOND:
            case SQL_INTERVAL_MINUTE_TO_SECOND:
                sRowBuffer->mColBuf[i].mBufSize = DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE + 1;
                break;
            case SQL_NUMERIC:
            case SQL_DECIMAL:
            case SQL_INTEGER:
            case SQL_SMALLINT:
            case SQL_BIGINT:
            case SQL_FLOAT:
            case SQL_REAL:
            case SQL_DOUBLE:
                sRowBuffer->mColBuf[i].mBufSize = ZTQ_NUMBER_STRING_MAX_SIZE + 1;
                break;
            default:
                sRowBuffer->mColBuf[i].mBufSize = aTargetColumn[i].mDataSize + 1;                
                break;
        }
        
        STL_TRY( stlAllocRegionMem( aAllocator,
                                    STL_SIZEOF(stlChar *) * sRowBuffer->mLineCnt,
                                    (void **) & sRowBuffer->mColBuf[i].mDataBuf,
                                    aErrorStack )
                 == STL_SUCCESS );

        for ( j = 0; j < sRowBuffer->mLineCnt; j++ )
        {
            STL_TRY( stlAllocRegionMem( aAllocator,
                                        sRowBuffer->mColBuf[i].mBufSize,
                                        (void **) & sRowBuffer->mColBuf[i].mDataBuf[j],
                                        aErrorStack )
                     == STL_SUCCESS );
            sRowBuffer->mColBuf[i].mDataBuf[j][0] = '\0';
        }
    }

    /*
     * Output 설정
     */

    *aRowBuffer = sRowBuffer;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqMakeRowString( ztqRowBuffer    * aRowBuffer,
                            stlInt32          aLineIdx,
                            ztqTargetColumn * aTargetColumn,
                            stlBool           aPrintResult,
                            stlErrorStack   * aErrorStack )
{
    stlInt32          i = 0;
    
    for ( i = 0; i < aRowBuffer->mColCnt; i++ )
    {
        if( (aPrintResult == STL_TRUE) && (gZtqPrintResult == STL_TRUE) )
        {
            STL_TRY( ztqMakeColString( &aRowBuffer->mColBuf[i],
                                       aLineIdx,
                                       &aTargetColumn[i],
                                       aErrorStack )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqPrintOutParamIfExist( ztqRowBuffer    * aRowBuffer,
                                   ztqHostVariable * aHostVarList,
                                   stlBool           aPrintResult,
                                   stlAllocator    * aAllocator,
                                   stlErrorStack   * aErrorStack )
{
    stlInt32          i = 0;
    stlInt32          sOutParamCnt = 0;
    
    ztqHostVariable * sHostVariable;
    ztqHostVariable * sTargetVariable;
    ztqHostVariable * sIndicatorVariable;

    ztqTargetColumn * sHostColumn = NULL;
    ztqRowBuffer    * sRowBuffer = NULL;
    stlChar           sIndicatorValue[ZTQ_MAX_HOST_VALUE_SIZE];

    STL_TRY_THROW( (aPrintResult == STL_TRUE) && (gZtqPrintResult == STL_TRUE),
                   RAMP_NO_PRINT );

    /*
     * Output Parameter 개수를 계산
     */
    
    for ( sHostVariable = aHostVarList;
          sHostVariable != NULL;
          sHostVariable = (ztqHostVariable*)sHostVariable->mNext )
    {
        if ( sHostVariable->mBindType == ZTQ_HOST_BIND_TYPE_OUT )
        {
            sOutParamCnt ++;
        }
    }

    STL_TRY_THROW( sOutParamCnt > 0, RAMP_NO_PRINT );

    /*
     * 가상의 Host Column 공간 확보 
     */

    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(ztqTargetColumn) * sOutParamCnt,
                                (void **) & sHostColumn,
                                aErrorStack )
             == STL_SUCCESS );

    i = 0;
    
    for ( sHostVariable = aHostVarList;
          sHostVariable != NULL;
          sHostVariable = (ztqHostVariable*)sHostVariable->mNext )
    {
        if ( sHostVariable->mBindType == ZTQ_HOST_BIND_TYPE_OUT )
        {
            STL_TRY( ztqGetHostVariable( sHostVariable->mName,
                                         &sTargetVariable,
                                         aErrorStack )
                     == STL_SUCCESS );
            
            /*
             * 가상의 컬럼 정보를 구성
             */
            
            stlStrcpy( sHostColumn[i].mName, sTargetVariable->mName );
            sHostColumn[i].mDataValue = sTargetVariable->mValue;
            if ( sTargetVariable->mIndicator == SQL_NULL_DATA )
            {
                sHostColumn[i].mDataSize  = stlStrlen( ZTQ_NULL_STRING );
            }
            else
            {
                sHostColumn[i].mDataSize = STL_MIN( sTargetVariable->mIndicator,
                                                    ZTQ_MAX_HOST_VALUE_SIZE );
            }
            sHostColumn[i].mDataType  = sTargetVariable->mDataType;
            sHostColumn[i].mDtlDataType = DTL_TYPE_NA;
            sHostColumn[i].mIndicator = sTargetVariable->mIndicator;
            sHostColumn[i].mIntervalColInfo.mLeadingPrecision = sTargetVariable->mPrecision;
            sHostColumn[i].mIntervalColInfo.mSecondPrecision = sTargetVariable->mScale;

            if( (sTargetVariable->mDataType == SQL_CHAR)           ||
                (sTargetVariable->mDataType == SQL_VARCHAR)        ||
                (sTargetVariable->mDataType == SQL_LONGVARCHAR)    ||
                (sTargetVariable->mDataType == SQL_WCHAR)          ||
                (sTargetVariable->mDataType == SQL_WVARCHAR)       ||
                (sTargetVariable->mDataType == SQL_WLONGVARCHAR)   ||
                (sTargetVariable->mDataType == SQL_BINARY)         ||
                (sTargetVariable->mDataType == SQL_VARBINARY)      ||
                (sTargetVariable->mDataType == SQL_LONGVARBINARY)  ||
                (sTargetVariable->mDataType == SQL_BOOLEAN)        ||
                (sTargetVariable->mDataType == SQL_TYPE_DATE)      ||
                (sTargetVariable->mDataType == SQL_TYPE_TIME)      ||
                (sTargetVariable->mDataType == SQL_TYPE_TIME_WITH_TIMEZONE) ||
                (sTargetVariable->mDataType == SQL_TYPE_TIMESTAMP) ||
                (sTargetVariable->mDataType == SQL_TYPE_TIMESTAMP_WITH_TIMEZONE) ||
                (sTargetVariable->mDataType == SQL_INTERVAL_YEAR)           ||
                (sTargetVariable->mDataType == SQL_INTERVAL_MONTH)          ||
                (sTargetVariable->mDataType == SQL_INTERVAL_DAY)            ||
                (sTargetVariable->mDataType == SQL_INTERVAL_HOUR)           ||
                (sTargetVariable->mDataType == SQL_INTERVAL_MINUTE)         ||
                (sTargetVariable->mDataType == SQL_INTERVAL_SECOND)         ||
                (sTargetVariable->mDataType == SQL_INTERVAL_YEAR_TO_MONTH)  ||
                (sTargetVariable->mDataType == SQL_INTERVAL_DAY_TO_HOUR)    ||
                (sTargetVariable->mDataType == SQL_INTERVAL_DAY_TO_MINUTE)  ||
                (sTargetVariable->mDataType == SQL_INTERVAL_DAY_TO_SECOND)  ||
                (sTargetVariable->mDataType == SQL_INTERVAL_HOUR_TO_MINUTE) ||
                (sTargetVariable->mDataType == SQL_INTERVAL_HOUR_TO_SECOND) ||
                (sTargetVariable->mDataType == SQL_INTERVAL_MINUTE_TO_SECOND) )
            {
                stlStrcpy( sHostColumn[i].mColumnFormat, ZTQ_COLUMN_FORMAT_CLASS_STRING );
                sHostColumn[i].mDataTypeGroup = ZTQ_GROUP_STRING;
            }
            else
            {
                stlStrcpy( sHostColumn[i].mColumnFormat, ZTQ_COLUMN_FORMAT_CLASS_NUMERIC );
                sHostColumn[i].mDataTypeGroup = ZTQ_GROUP_NUMERIC;
            }

            if( sHostVariable->mIndicatorVariable != NULL )
            {
                STL_TRY( ztqGetHostVariable( ((ztqHostVariable *)sHostVariable->mIndicatorVariable)->mName,
                                             &sIndicatorVariable,
                                             aErrorStack )
                         == STL_SUCCESS );

#if (STL_SIZEOF_VOIDP == 8 )
                stlSnprintf( sIndicatorValue, ZTQ_MAX_HOST_VALUE_SIZE,
                             "%ld",
                             sHostVariable->mIndicator );
#else
                stlSnprintf( sIndicatorValue, ZTQ_MAX_HOST_VALUE_SIZE,
                             "%d",
                             sHostVariable->mIndicator );
#endif
                
                STL_TRY( ztqAssignSysHostVariable( sIndicatorVariable->mName,
                                                   sIndicatorValue,
                                                   aErrorStack )
                         == STL_SUCCESS );

                sHostVariable->mIndicatorVariable = NULL;
            }

            i++;
        }
    }

    /*
     * Row Buffer 공간 확보 
     */
    
    STL_TRY( ztqAllocRowBuffer( &sRowBuffer,
                                sOutParamCnt,
                                1,
                                sHostColumn,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );

    ztqSetInitRowStringLen( sRowBuffer, sHostColumn );
    
    /*
     * OUTPUT Parameter 를 이용하여 Row 를 구성 
     */
    
    i = 0;
    
    for ( sHostVariable = aHostVarList;
          sHostVariable != NULL;
          sHostVariable = (ztqHostVariable*)sHostVariable->mNext )
    {
        if ( sHostVariable->mBindType == ZTQ_HOST_BIND_TYPE_OUT )
        {
            /*
             * 컬럼 Data 구성
             */

            STL_TRY( ztqMakeColString( &sRowBuffer->mColBuf[i],
                                       0,
                                       & sHostColumn[i],
                                       aErrorStack )
                     == STL_SUCCESS );

            i++;
        }
        else
        {
            /*
             * In Parameter 임
             */
        }
    }

    /*
     * Output Parameter 를 출력
     */
    
    STL_TRY( ztqPrintTableHeader( sRowBuffer, sHostColumn ) == STL_SUCCESS );
    STL_TRY( ztqPrintRowString( sRowBuffer,
                                sHostColumn,
                                0,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );

    ztqPrintf( "\n" );
    
    STL_RAMP( RAMP_NO_PRINT );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqMakeColString( ztqColBuffer    * aColBuffer,
                            stlInt32          aLineIdx,
                            ztqTargetColumn * aTargetColumn,
                            stlErrorStack   * aErrorStack )
{
    stlInt32        sPosition = 0;
    stlChar       * sValue;
    stlChar       * sEndPtr;
    stlInt8         sMbLength;
    stlInt32        sDisplayLength = 0;
    stlInt32        sMaxDisplayLength = 0;
    stlInt32        sLineCount = 1;
    stlInt32        sDisplayMaxLength = 0;

    stlChar         sFormatString[DTL_DATETIME_TO_CHAR_RESULT_NULL_TERMINATED_STRING_MAX_SIZE];
    stlInt64        sFormatStringLen;

    stlChar         sNumberString[ZTQ_NUMBER_STRING_MAX_SIZE + 1];
    stlInt64        sNumberStringLen;

    SQLULEN         sDataSize;
    stlInt32        sIndicator = 0;
    stlStatus       sReturn;

    aColBuffer->mDataBuf[aLineIdx][0] = '\0';

    /*
     * Data 복사
     */
    
    sValue = aTargetColumn->mDataValue;
    sDataSize = aTargetColumn->mDataSize;
    sIndicator = aTargetColumn->mIndicator;
    
    if ( aTargetColumn->mIndicator == SQL_NULL_DATA )
    {
        stlSnprintf( aColBuffer->mDataBuf[aLineIdx],
                     aColBuffer->mBufSize,
                     ZTQ_NULL_STRING );
        sMaxDisplayLength = stlStrlen( ZTQ_NULL_STRING );
        sDisplayMaxLength = -1;
    }
    else
    {
        /*
         * (1) datetime type은 data를
         *     session format에 맞게 string으로 변환한다.
         * (2) interval type은 data를
         *     leading precision, fractional second precision의 자리수에 맞게 string으로 변환한다.
         * (3) number type ( 숫자타입전체 ) 는
         *     numsize( default 20 ) 값에 맞게 resizing 된다. 
         */

        switch( aTargetColumn->mDataType )
        {
            case SQL_TYPE_DATE:
            case SQL_TYPE_TIME:
            case SQL_TYPE_TIME_WITH_TIMEZONE:
            case SQL_TYPE_TIMESTAMP:
            case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE:
            case SQL_INTERVAL_YEAR:
            case SQL_INTERVAL_MONTH:
            case SQL_INTERVAL_DAY:
            case SQL_INTERVAL_HOUR:
            case SQL_INTERVAL_MINUTE:
            case SQL_INTERVAL_SECOND:                
            case SQL_INTERVAL_YEAR_TO_MONTH:
            case SQL_INTERVAL_DAY_TO_HOUR:
            case SQL_INTERVAL_DAY_TO_MINUTE:
            case SQL_INTERVAL_DAY_TO_SECOND:
            case SQL_INTERVAL_HOUR_TO_MINUTE:
            case SQL_INTERVAL_HOUR_TO_SECOND:
            case SQL_INTERVAL_MINUTE_TO_SECOND:
                {
                    STL_TRY( ztqMakeColFormatString( aTargetColumn,
                                                     sFormatString,
                                                     & sFormatStringLen,
                                                     aErrorStack )
                             == STL_SUCCESS );

                    sValue = sFormatString;
                    sDataSize = sFormatStringLen;
                    sIndicator = sFormatStringLen;

                    sFormatString[sIndicator] = '\0';
                    
                    break;
                }
            case SQL_NUMERIC:
            case SQL_DECIMAL:
            case SQL_INTEGER:
            case SQL_SMALLINT:
            case SQL_BIGINT:
            case SQL_FLOAT:
            case SQL_REAL:
            case SQL_DOUBLE:
                sReturn = ztqResizeNumberString( aTargetColumn,
                                                 gZtqNumSize,
                                                 sNumberString,
                                                 & sNumberStringLen,
                                                 aErrorStack );
                if( sReturn == STL_FAILURE )
                {
                    STL_TRY( STL_HAS_ERROR( aErrorStack ) == STL_FALSE );
                    
                    stlMemset( sNumberString,
                               '#',
                               gZtqNumSize );

                    sValue = sNumberString;
                    sDataSize = gZtqNumSize;
                    sIndicator = gZtqNumSize;
                }
                else
                {
                    sValue = sNumberString;
                    sDataSize = sNumberStringLen;
                    sIndicator = sNumberStringLen;
                }

                sNumberString[sIndicator] = '\0';                
                break;
            default:
                /* Do Nothing */
                break;
        }
        
        sDisplayMaxLength = STL_MIN( sDataSize,
                                     sIndicator );
        
        stlStrncat( aColBuffer->mDataBuf[aLineIdx],
                    (stlChar*)sValue,
                    sDisplayMaxLength + 1 );
        sMaxDisplayLength = 0;
    }
    
    /*
     * Display Length 계산
     */
    
    sEndPtr = sValue + sDisplayMaxLength;
            
    while( (sValue + sPosition) < sEndPtr )
    {
        STL_TRY( dtlGetMbLength( gZtqCharacterSet,
                                 (stlChar*)sValue + sPosition,
                                 &sMbLength,
                                 aErrorStack )
                 == STL_SUCCESS );
                
        /**
         * @todo database character set이 UTF8일 경우에만 화면에 맞추어 출력합니다.
         * UTF8이 아닌 경우라면 1 바이트가 화면 1 컬럼이라고 가정합니다.
         * 필요하다면 추가적으로 구현해야 합니다.
         * 필요한 시점이 되면 아마도 dtl 인터페이스가 만들어지지 않았을까...
         */
        sDisplayLength += dtlGetMbDisplayLength( gZtqCharacterSet,
                                                 (stlUInt8*)(sValue + sPosition) );
        
        if( sMbLength == 1 )
        {
            switch( *(sValue + sPosition) )
            {
                case '\t':
                    sDisplayLength += (ZTQ_TAB_SIZE - 1);
                    break;
                case '\n':
                    sMaxDisplayLength = STL_MAX( sDisplayLength - 1, sMaxDisplayLength );
                    sDisplayLength = 0;
                    sLineCount += 1;
                    break;
                default:
                    break;
            }
        }
        
        sPosition += sMbLength;
    }

    sMaxDisplayLength = STL_MAX( sDisplayLength, sMaxDisplayLength );
    
    /*
     * 최대 라인 조정
     */
    aColBuffer->mMaxLineCount = STL_MAX( aColBuffer->mMaxLineCount, sLineCount );

    /*
     * 최대 길이 조정
     */
    aColBuffer->mMaxLength = STL_MAX( aColBuffer->mMaxLength, sMaxDisplayLength );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void ztqSetInitRowStringLen( ztqRowBuffer    * aRowBuffer,
                             ztqTargetColumn * aTargetColumn )
{
    stlInt32 i = 0;

    for ( i = 0; i < aRowBuffer->mColCnt; i++ )
    {
        aRowBuffer->mColBuf[i].mMaxLength = stlStrlen( aTargetColumn[i].mName );
    }
}


stlStatus ztqGetParameterColSizeAndDecimalDigit( stlInt32          aDataType,
                                                 stlInt64          aPrecision,
                                                 stlInt64          aScale,
                                                 SQLULEN         * aColumnSize,
                                                 SQLSMALLINT     * aDecimalDigits,
                                                 stlErrorStack   * aErrorStack )
{
    switch( aDataType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
        case SQL_SMALLINT :
        case SQL_INTEGER :
        case SQL_REAL :
        case SQL_DOUBLE :
        case SQL_NUMERIC :            
        case SQL_FLOAT :
        case SQL_BIGINT :
        case SQL_BINARY :
        case SQL_VARBINARY :
        case SQL_BOOLEAN :
        case SQL_ROWID :            
            *aColumnSize = aPrecision;
            *aDecimalDigits = aScale;
            break;
        case SQL_LONGVARCHAR :
        case SQL_LONGVARBINARY :
            *aColumnSize = (stlUInt32)SQL_NO_TOTAL;
            *aDecimalDigits = aScale;
            break;
        case SQL_TYPE_DATE :
            *aColumnSize = DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE;
            *aDecimalDigits = aScale;
            break;
        case SQL_TYPE_TIME :
            *aColumnSize = DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE;
            *aDecimalDigits = aPrecision;
            break;
        case SQL_TYPE_TIME_WITH_TIMEZONE :
            *aColumnSize = DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE;
            *aDecimalDigits = aPrecision;
            break;
        case SQL_TYPE_TIMESTAMP :
            *aColumnSize = DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE;
            *aDecimalDigits = aPrecision;
            break;
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            *aColumnSize = DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE;
            *aDecimalDigits = aPrecision;
            break;
        case SQL_INTERVAL_YEAR :
            *aColumnSize = DTL_INTERVAL_YEAR_ODBC_COLUMN_SIZE( aPrecision );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_MONTH :
            *aColumnSize = DTL_INTERVAL_MONTH_ODBC_COLUMN_SIZE( aPrecision );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_DAY :
            *aColumnSize = DTL_INTERVAL_DAY_ODBC_COLUMN_SIZE( aPrecision );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_HOUR :
            *aColumnSize = DTL_INTERVAL_HOUR_ODBC_COLUMN_SIZE( aPrecision );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_MINUTE :
            *aColumnSize = DTL_INTERVAL_MINUTE_ODBC_COLUMN_SIZE( aPrecision );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_SECOND :
            *aColumnSize = DTL_INTERVAL_SECOND_ODBC_COLUMN_SIZE( aPrecision,
                                                                 aScale );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_YEAR_TO_MONTH :
            *aColumnSize = DTL_INTERVAL_YEAR_TO_MONTH_ODBC_COLUMN_SIZE( aPrecision );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_DAY_TO_HOUR :
            *aColumnSize = DTL_INTERVAL_DAY_TO_HOUR_ODBC_COLUMN_SIZE( aPrecision );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_DAY_TO_MINUTE :
            *aColumnSize = DTL_INTERVAL_DAY_TO_MINUTE_ODBC_COLUMN_SIZE( aPrecision );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_DAY_TO_SECOND :
            *aColumnSize = DTL_INTERVAL_DAY_TO_SECOND_ODBC_COLUMN_SIZE( aPrecision,
                                                                        aScale );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_HOUR_TO_MINUTE :
            *aColumnSize = DTL_INTERVAL_HOUR_TO_MINUTE_ODBC_COLUMN_SIZE( aPrecision );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_HOUR_TO_SECOND :
            *aColumnSize = DTL_INTERVAL_HOUR_TO_SECOND_ODBC_COLUMN_SIZE( aPrecision,
                                                                         aScale );
            *aDecimalDigits = aScale;
            break;
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            *aColumnSize = DTL_INTERVAL_MINUTE_TO_SECOND_ODBC_COLUMN_SIZE( aPrecision,
                                                                           aScale );
            *aDecimalDigits = aScale;
            break;
        default :
            STL_DASSERT( 0 );
            break;
    }

    return STL_SUCCESS;
}


stlStatus ztqSetIntervalLeadingPrecision( SQLHSTMT         aStmtHandle,
                                          SQLUSMALLINT     aParameterNumber,
                                          stlInt32         aDataType,
                                          stlInt32         aLeadingPrecision,
                                          stlErrorStack  * aErrorStack )
{
    SQLHDESC     sDescIpd = NULL;
    SQLHDESC     sDescApd = NULL;
    SQLPOINTER * sDataPtr = NULL;

    switch( aDataType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
        case SQL_LONGVARCHAR :
        case SQL_NUMERIC :
        case SQL_SMALLINT :
        case SQL_INTEGER :
        case SQL_REAL :
        case SQL_DOUBLE :
        case SQL_FLOAT :
        case SQL_BIGINT :
        case SQL_BINARY :
        case SQL_VARBINARY :
        case SQL_LONGVARBINARY :
        case SQL_BOOLEAN :
        case SQL_ROWID :            
        case SQL_TYPE_DATE :
        case SQL_TYPE_TIME :
        case SQL_TYPE_TIME_WITH_TIMEZONE :
        case SQL_TYPE_TIMESTAMP :
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            /* Do Nothing */
            break;
        case SQL_INTERVAL_YEAR :
        case SQL_INTERVAL_MONTH :
        case SQL_INTERVAL_DAY :
        case SQL_INTERVAL_HOUR :
        case SQL_INTERVAL_MINUTE :
        case SQL_INTERVAL_SECOND :
        case SQL_INTERVAL_YEAR_TO_MONTH :
        case SQL_INTERVAL_DAY_TO_HOUR :
        case SQL_INTERVAL_DAY_TO_MINUTE :
        case SQL_INTERVAL_DAY_TO_SECOND :
        case SQL_INTERVAL_HOUR_TO_MINUTE :
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            {
                /*
                 * IPD
                 */
                STL_TRY( ztqSQLGetStmtAttr( aStmtHandle,
                                            SQL_ATTR_IMP_PARAM_DESC,
                                            & sDescIpd,
                                            SQL_IS_POINTER,
                                            NULL,
                                            aErrorStack )
                         == STL_SUCCESS );

                STL_TRY( ztqSQLSetDescField( sDescIpd,
                                             (SQLSMALLINT)aParameterNumber,
                                             SQL_DESC_DATETIME_INTERVAL_PRECISION,
                                             (SQLPOINTER)((SQLLEN)aLeadingPrecision),
                                             0,
                                             aErrorStack )
                         == STL_SUCCESS );

                /*
                 * check consistency
                 */
                sDataPtr = NULL;
                STL_TRY( ztqSQLSetDescField( sDescIpd,
                                             (SQLSMALLINT)aParameterNumber,
                                             SQL_DESC_DATA_PTR,
                                             (SQLPOINTER)sDataPtr,
                                             0,
                                             aErrorStack )
                         == STL_SUCCESS );

                /*
                 * APD
                 */
                STL_TRY( ztqSQLGetStmtAttr( aStmtHandle,
                                            SQL_ATTR_APP_PARAM_DESC,
                                            & sDescApd,
                                            SQL_IS_POINTER,
                                            NULL,
                                            aErrorStack )
                         == STL_SUCCESS );
                
                STL_TRY( ztqSQLGetDescField( sDescApd,
                                             (SQLSMALLINT)aParameterNumber,
                                             SQL_DESC_DATA_PTR,
                                             (SQLPOINTER)&sDataPtr,
                                             0,
                                             NULL,
                                             aErrorStack )
                         == STL_SUCCESS );
                
                STL_TRY( ztqSQLSetDescField( sDescApd,
                                             (SQLSMALLINT)aParameterNumber,
                                             SQL_DESC_DATETIME_INTERVAL_PRECISION,
                                             (SQLPOINTER)((SQLLEN)aLeadingPrecision),
                                             0,
                                             aErrorStack )
                         == STL_SUCCESS );

                STL_TRY( ztqSQLSetDescField( sDescApd,
                                             (SQLSMALLINT)aParameterNumber,
                                             SQL_DESC_DATA_PTR,
                                             (SQLPOINTER)sDataPtr,
                                             0,
                                             aErrorStack )
                         == STL_SUCCESS );

                break;
            }
        default :
            STL_DASSERT( 0 );
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqSetCharacterLengthUnits( SQLHSTMT              aStmtHandle,
                                      SQLUSMALLINT          aParameterNumber,
                                      stlInt32              aDataType,
                                      dtlStringLengthUnit   aStringLengthUnit,
                                      stlErrorStack       * aErrorStack )
{
    SQLHDESC   sDescIpd = NULL;
    stlInt16   sCharLengthUnit = SQL_CLU_NONE;
    SQLPOINTER sDataPtr = NULL;

    switch( aDataType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
            STL_TRY( ztqSQLGetStmtAttr( aStmtHandle,
                                        SQL_ATTR_IMP_PARAM_DESC,
                                        & sDescIpd,
                                        SQL_IS_POINTER,
                                        NULL,
                                        aErrorStack )
                     == STL_SUCCESS );

            switch( aStringLengthUnit )
            {
                case DTL_STRING_LENGTH_UNIT_CHARACTERS :
                    sCharLengthUnit = SQL_CLU_CHARACTERS;
                    break;
                case DTL_STRING_LENGTH_UNIT_OCTETS :
                    sCharLengthUnit = SQL_CLU_OCTETS;
                    break;
                default :
                    STL_ASSERT( STL_FALSE );
                    break;
            }
                
            STL_TRY( ztqSQLSetDescField( sDescIpd,
                                         (SQLSMALLINT)aParameterNumber,
                                         SQL_DESC_CHAR_LENGTH_UNITS,
                                         (SQLPOINTER)(stlVarInt)sCharLengthUnit,
                                         0,
                                         aErrorStack )
                     == STL_SUCCESS );

            /*
             * check consistency
             */
            sDataPtr = NULL;
            STL_TRY( ztqSQLSetDescField( sDescIpd,
                                         (SQLSMALLINT)aParameterNumber,
                                         SQL_DESC_DATA_PTR,
                                         (SQLPOINTER)sDataPtr,
                                         0,
                                         aErrorStack )
                     == STL_SUCCESS );
            break;
        case SQL_LONGVARCHAR :
        case SQL_NUMERIC :
        case SQL_SMALLINT :
        case SQL_INTEGER :
        case SQL_REAL :
        case SQL_DOUBLE :
        case SQL_FLOAT :
        case SQL_BIGINT :
        case SQL_BINARY :
        case SQL_VARBINARY :
        case SQL_LONGVARBINARY :
        case SQL_BOOLEAN :
        case SQL_ROWID :            
        case SQL_TYPE_DATE :
        case SQL_TYPE_TIME :
        case SQL_TYPE_TIME_WITH_TIMEZONE :
        case SQL_TYPE_TIMESTAMP :
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
        case SQL_INTERVAL_YEAR :
        case SQL_INTERVAL_MONTH :
        case SQL_INTERVAL_DAY :
        case SQL_INTERVAL_HOUR :
        case SQL_INTERVAL_MINUTE :
        case SQL_INTERVAL_SECOND :
        case SQL_INTERVAL_YEAR_TO_MONTH :
        case SQL_INTERVAL_DAY_TO_HOUR :
        case SQL_INTERVAL_DAY_TO_MINUTE :
        case SQL_INTERVAL_DAY_TO_SECOND :
        case SQL_INTERVAL_HOUR_TO_MINUTE :
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            /* Do Nothing */
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztqBuildNLSToCharFormatInfo( stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack )
{
    stlChar   sPropertyName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar   sValue[DTL_NLS_DATETIME_FORMAT_NULL_TERMINATED_STRING_MAX_SIZE];

    /**************************************
     * NLS_DATE_FORMAT
     **************************************/ 
    
    stlStrcpy( sPropertyName,
               "NLS_DATE_FORMAT" );

    STL_TRY( ztqGetNlsDateTimeFormatString( DTL_TYPE_DATE,
                                            sPropertyName,
                                            sValue,
                                            aAllocator,
                                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_DATE,
                                                sValue,
                                                stlStrlen(sValue),
                                                gZtqNLSDateFormatInfo,
                                                DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                NULL, // aVectorFunc
                                                NULL, // aVectorArg
                                                aErrorStack )
             == STL_SUCCESS );
    
    /**************************************
     * NLS_TIME_FORMAT
     **************************************/

    stlStrcpy( sPropertyName,
               "NLS_TIME_FORMAT" );

    STL_TRY( ztqGetNlsDateTimeFormatString( DTL_TYPE_TIME,
                                            sPropertyName,
                                            sValue,
                                            aAllocator,
                                            aErrorStack )
             == STL_SUCCESS );
   
    STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIME,
                                                sValue,
                                                stlStrlen(sValue),
                                                gZtqNLSTimeFormatInfo,
                                                DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                NULL, // aVectorFunc
                                                NULL, // aVectorArg
                                                aErrorStack )
             == STL_SUCCESS );

    /**************************************
     * NLS_TIME_WITH_TIME_ZONE_FORMAT
     **************************************/

    stlStrcpy( sPropertyName,
               "NLS_TIME_WITH_TIME_ZONE_FORMAT" );

    STL_TRY( ztqGetNlsDateTimeFormatString( DTL_TYPE_TIME_WITH_TIME_ZONE,
                                            sPropertyName,
                                            sValue,
                                            aAllocator,
                                            aErrorStack )
             == STL_SUCCESS );
   
    STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIME_WITH_TIME_ZONE,
                                                sValue,
                                                stlStrlen(sValue),
                                                gZtqNLSTimeWithTimeZoneFormatInfo,
                                                DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                NULL, // aVectorFunc
                                                NULL, // aVectorArg
                                                aErrorStack )
             == STL_SUCCESS );

    /**************************************
     * NLS_TIMESTAMP_FORMAT
     **************************************/

    stlStrcpy( sPropertyName,
               "NLS_TIMESTAMP_FORMAT" );

    STL_TRY( ztqGetNlsDateTimeFormatString( DTL_TYPE_TIMESTAMP,
                                            sPropertyName,
                                            sValue,
                                            aAllocator,
                                            aErrorStack )
             == STL_SUCCESS );
 
    STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIMESTAMP,
                                                sValue,
                                                stlStrlen(sValue),
                                                gZtqNLSTimestampFormatInfo,
                                                DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                NULL, // aVectorFunc
                                                NULL, // aVectorArg
                                                aErrorStack )
             == STL_SUCCESS );

    /**************************************
     * NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT
     **************************************/

    stlStrcpy( sPropertyName,
               "NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT" );

    STL_TRY( ztqGetNlsDateTimeFormatString( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                            sPropertyName,
                                            sValue,
                                            aAllocator,
                                            aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                                sValue,
                                                stlStrlen(sValue),
                                                gZtqNLSTimestampWithTimeZoneFormatInfo,
                                                DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                NULL, // aVectorFunc
                                                NULL, // aVectorArg
                                                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * property name에 따른 datetime format string을 리턴한다.
 *
 * 타 DBMS와의 연동을 위해 
 * X$PROPERTY가 없는 경우 default datetime format string을 리턴한다.
 * 
 */ 
stlStatus ztqGetNlsDateTimeFormatString( dtlDataType     aDataType,
                                         stlChar       * aPropertyName,
                                         stlChar       * aValue,
                                         stlAllocator  * aAllocator,
                                         stlErrorStack * aErrorStack )
{
    /*
     * DATETIME PROPERTY VALUE를 아래와 같이 2단계로 검색해와서 tochar format info를 구성한다.
     * (1) SELECT PROPERTY_ID FROM X$PROPERTY WHERE PROPERTY_NAME = '%s';
     * (2) SELECT VALUE FROM X$PROPERTY WHERE PROPERTY_NAME = PROPERTY_ID;
     * 
     * datetime property value를  
     * SELECT VALUE FROM X$PROPERTY WHERE PROPERTY_NAME = '%s';
     * 로 한번에 얻지 않고, 위와같이 2번에 걸쳐 얻어오는 이유는,
     * ALTER SESSION SET BLOCK_READ_COUNT = 65535; 가 수행된 이후,
     * 이 함수가 호출되면, 아래와 같은 에러가 발생한다.
     * gSQL>  ALTER SESSION SET BLOCK_READ_COUNT = 65535;
     * Session altered.
     * ERR-HY000(13011): Unable to extend memory: [MAX: 104857600, TOTAL: 20971696, ALLOC: 144703576] DESC: private static area
     * gsql에서 이러한 에러를 방지하기 위해, datetime property value를 두 단계로 얻도록 함.
     */
    
    stlInt16  sPropertyID;
    
    if( ztqGetNLSFormatPropertyID( aPropertyName,
                                   & sPropertyID,
                                   aAllocator,
                                   aErrorStack )
        == STL_SUCCESS )
    {    
        STL_TRY( ztqGetNLSFormatString( sPropertyID,
                                        aValue,
                                        aAllocator,
                                        aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        (void)stlPopError( aErrorStack );
        
        switch( aDataType )
        {
            case DTL_TYPE_DATE:
                stlStrcpy( aValue,
                           DTL_NOMOUNT_DATE_FORMAT );
                break;
            case DTL_TYPE_TIME:
                stlStrcpy( aValue,
                           DTL_NOMOUNT_TIME_FORMAT );
                break;
            case DTL_TYPE_TIME_WITH_TIME_ZONE:
                stlStrcpy( aValue,
                           DTL_NOMOUNT_TIME_WITH_TIME_ZONE_FORMAT );
                break;
            case DTL_TYPE_TIMESTAMP:
                stlStrcpy( aValue,
                           DTL_NOMOUNT_TIMESTAMP_FORMAT );
                break;
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                stlStrcpy( aValue,
                           DTL_NOMOUNT_TIMESTAMP_WITH_TIME_ZONE_FORMAT );
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
             
stlStatus ztqGetNLSFormatPropertyID( stlChar       * aPropertyName,
                                     stlInt16      * aPropertyID,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack )
{
    SQLHSTMT     sStmtHandle;

    stlChar      sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    SQLLEN       sIndicator;
    stlInt32     sState = 0;
    SQLRETURN    sResult;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT PROPERTY_ID "
                 "  FROM X$PROPERTY "
                 "WHERE PROPERTY_NAME = '%s'",
                 aPropertyName );

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztqDescExecDirect( sStmtHandle,
                                (SQLCHAR*)sSqlString,
                                SQL_NTS,
                                & sResult,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( sResult == SQL_SUCCESS );
    sState = 2;    

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_SSHORT,
                            aPropertyID,
                            STL_SIZEOF(stlInt16),                           
                            &sIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    if( sResult == SQL_NO_DATA ) 
    {
        /**
         * 검색되지 않는 경우는 없음.
         */
        
        STL_DASSERT( 0 );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztqGetNLSFormatString( stlInt16        aPropertyID,
                                 stlChar       * aValue,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack )
{
    SQLHSTMT     sStmtHandle;
    
    stlChar      sSqlString[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    SQLLEN       sIndicator;
    stlInt32     sState = 0;
    SQLRETURN    sResult;

    stlSnprintf( sSqlString,
                 ZTQ_MAX_COMMAND_BUFFER_SIZE,
                 "SELECT VALUE "
                 "  FROM X$PROPERTY "
                 "WHERE PROPERTY_ID = %d",
                 aPropertyID );

    STL_TRY( ztqSQLAllocHandle( SQL_HANDLE_STMT,
                                gZtqDbcHandle,
                                &sStmtHandle,
                                aErrorStack )
             == SQL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztqSQLExecDirect( sStmtHandle,
                               (SQLCHAR*)sSqlString,
                               SQL_NTS,
                               &sResult,
                               aAllocator,
                               aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( ztqSQLBindCol( sStmtHandle,
                            1,
                            SQL_C_CHAR,
                            aValue,
                            DTL_NLS_DATETIME_FORMAT_NULL_TERMINATED_STRING_MAX_SIZE,
                            &sIndicator,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqSQLFetch( sStmtHandle,
                          &sResult,
                          aErrorStack )
             == STL_SUCCESS );
    
    if( sResult == SQL_NO_DATA ) 
    {
        /**
         * 검색되지 않는 경우는 없음.
         */
        
        STL_DASSERT( 0 );
    }
    
    sState = 1;
    STL_TRY( ztqSQLCloseCursor( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( ztqSQLFreeHandle( SQL_HANDLE_STMT,
                               sStmtHandle,
                               aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)ztqSQLCloseCursor( sStmtHandle,
                                     aErrorStack );
        case 1:
            (void)ztqSQLFreeHandle( SQL_HANDLE_STMT,
                                    sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztqMakeColFormatString( ztqTargetColumn * aTargetColumn,
                                  stlChar         * aFormatString,
                                  stlInt64        * aFormatStringLen,
                                  stlErrorStack   * aErrorStack )
{
    dtlFractionalSecond   sFractionalSecond = 0;
    stlInt32              sTimeZone         = 0;
    dtlExpTime            sDtlExpTime;
    
    dtlIntervalIndicator  sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
    
    SQL_DATE_STRUCT                    * sDATE_STRUCT         = NULL;
    SQL_TIME_WITH_TIMEZONE_STRUCT      * sTIME_TZ_STRUCT      = NULL;
    SQL_TIMESTAMP_STRUCT               * sTIMESTAMP_STRUCT    = NULL;
    SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT * sTIMESTAMP_TZ_STRUCT = NULL;
    SQL_INTERVAL_STRUCT                * sINTERVAL_STRUCT     = NULL;
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );

#define ZTQ_GET_DATETIME_YEAR_FROM_C_DATETIME_STRUCT( aYear ) DTL_GET_DTLEXPTIME_YEAR_FROM_YEAR( aYear )

#define ZTQ_GET_DATETIME_FRACTIONAL_SECOND_FROM_C_DATETIME_STRUCT( aFractionalSecond )     \
    ( (aFractionalSecond) / 1000 )
    
#define ZTQ_GET_TIMEZONE_FROM_C_DATETIME_STRUCT( timezonehour, timezoneminute )                   \
    ( (((timezonehour) * DTL_SECS_PER_HOUR) + ((timezoneminute) * DTL_SECS_PER_MINUTE)) * -1 )
    
    switch( aTargetColumn->mDataType )
    {
        case SQL_TYPE_DATE:
            {
                sDATE_STRUCT = (SQL_DATE_STRUCT *)(aTargetColumn->mDataValue);
                
                sDtlExpTime.mYear  = ZTQ_GET_DATETIME_YEAR_FROM_C_DATETIME_STRUCT( sDATE_STRUCT->year );
                sDtlExpTime.mMonth = sDATE_STRUCT->month;
                sDtlExpTime.mDay   = sDATE_STRUCT->day;
                
                sFractionalSecond   = 0;
                sTimeZone           = 0;

                STL_TRY( dtlGetToCharDateTime( DTL_TYPE_DATE,
                                               & sDtlExpTime,
                                               sFractionalSecond,
                                               & sTimeZone,
                                               gZtqNLSDateFormatInfo,
                                               DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                               aFormatString,
                                               aFormatStringLen,
                                               aErrorStack )
                         == STL_SUCCESS );
                
                break;
            }
        case SQL_TYPE_TIME:
            {
                sTIME_TZ_STRUCT = (SQL_TIME_WITH_TIMEZONE_STRUCT *)(aTargetColumn->mDataValue);

                sDtlExpTime.mHour = sTIME_TZ_STRUCT->hour;
                sDtlExpTime.mMinute = sTIME_TZ_STRUCT->minute;
                sDtlExpTime.mSecond = sTIME_TZ_STRUCT->second;
                
                sFractionalSecond = ZTQ_GET_DATETIME_FRACTIONAL_SECOND_FROM_C_DATETIME_STRUCT( sTIME_TZ_STRUCT->fraction );
                sTimeZone         = 0; 

                STL_TRY( dtlGetToCharDateTime( DTL_TYPE_TIME,
                                               & sDtlExpTime,
                                               sFractionalSecond,
                                               & sTimeZone,
                                               gZtqNLSTimeFormatInfo,
                                               DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                               aFormatString,
                                               aFormatStringLen,
                                               aErrorStack )
                         == STL_SUCCESS );
                
                break;
            }
        case SQL_TYPE_TIME_WITH_TIMEZONE:
            {
                sTIME_TZ_STRUCT = (SQL_TIME_WITH_TIMEZONE_STRUCT *)(aTargetColumn->mDataValue);

                sDtlExpTime.mHour = sTIME_TZ_STRUCT->hour;
                sDtlExpTime.mMinute = sTIME_TZ_STRUCT->minute;
                sDtlExpTime.mSecond = sTIME_TZ_STRUCT->second;
                
                sFractionalSecond = ZTQ_GET_DATETIME_FRACTIONAL_SECOND_FROM_C_DATETIME_STRUCT( sTIME_TZ_STRUCT->fraction );
                sTimeZone         = ZTQ_GET_TIMEZONE_FROM_C_DATETIME_STRUCT( sTIME_TZ_STRUCT->timezone_hour,
                                                                             sTIME_TZ_STRUCT->timezone_minute );
                                                                             
                STL_TRY( dtlGetToCharDateTime( DTL_TYPE_TIME_WITH_TIME_ZONE,
                                               & sDtlExpTime,
                                               sFractionalSecond,
                                               & sTimeZone,
                                               gZtqNLSTimeWithTimeZoneFormatInfo,
                                               DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                               aFormatString,
                                               aFormatStringLen,
                                               aErrorStack )
                         == STL_SUCCESS );
                
                break;
            }
        case SQL_TYPE_TIMESTAMP:
            {
                if( aTargetColumn->mDtlDataType == DTL_TYPE_DATE )
                {
                    sTIMESTAMP_STRUCT = (SQL_TIMESTAMP_STRUCT *)(aTargetColumn->mDataValue);
                    
                    sDtlExpTime.mYear   = ZTQ_GET_DATETIME_YEAR_FROM_C_DATETIME_STRUCT( sTIMESTAMP_STRUCT->year );
                    sDtlExpTime.mMonth  = sTIMESTAMP_STRUCT->month;
                    sDtlExpTime.mDay    = sTIMESTAMP_STRUCT->day;
                    sDtlExpTime.mHour   = sTIMESTAMP_STRUCT->hour;
                    sDtlExpTime.mMinute = sTIMESTAMP_STRUCT->minute;
                    sDtlExpTime.mSecond = sTIMESTAMP_STRUCT->second;
                    
                    sFractionalSecond   = 0;
                    sTimeZone           = 0;
                    
                    STL_TRY( dtlGetToCharDateTime( DTL_TYPE_DATE,
                                                   & sDtlExpTime,
                                                   sFractionalSecond,
                                                   & sTimeZone,
                                                   gZtqNLSDateFormatInfo,
                                                   DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                                   aFormatString,
                                                   aFormatStringLen,
                                                   aErrorStack )
                             == STL_SUCCESS );
                
                    break;
                }
                else
                {
                    // STL_DASSERT( aTargetColumn->mDtlDataType == DTL_TYPE_TIMESTAMP );
                    
                    sTIMESTAMP_STRUCT = (SQL_TIMESTAMP_STRUCT *)(aTargetColumn->mDataValue);
                    
                    sDtlExpTime.mYear = ZTQ_GET_DATETIME_YEAR_FROM_C_DATETIME_STRUCT( sTIMESTAMP_STRUCT->year );
                    sDtlExpTime.mMonth = sTIMESTAMP_STRUCT->month;
                    sDtlExpTime.mDay = sTIMESTAMP_STRUCT->day;
                    sDtlExpTime.mHour = sTIMESTAMP_STRUCT->hour;
                    sDtlExpTime.mMinute = sTIMESTAMP_STRUCT->minute;
                    sDtlExpTime.mSecond = sTIMESTAMP_STRUCT->second;
                    
                    sFractionalSecond = ZTQ_GET_DATETIME_FRACTIONAL_SECOND_FROM_C_DATETIME_STRUCT( sTIMESTAMP_STRUCT->fraction );
                    sTimeZone = 0;
                    
                    STL_TRY( dtlGetToCharDateTime( DTL_TYPE_TIMESTAMP,
                                                   & sDtlExpTime,
                                                   sFractionalSecond,
                                                   & sTimeZone,
                                                   gZtqNLSTimestampFormatInfo,
                                                   DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                                   aFormatString,
                                                   aFormatStringLen,
                                                   aErrorStack )
                             == STL_SUCCESS );
                    
                    break;
                }
            }
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE:
            {
                sTIMESTAMP_TZ_STRUCT = (SQL_TIMESTAMP_WITH_TIMEZONE_STRUCT *)(aTargetColumn->mDataValue);
                
                sDtlExpTime.mYear = ZTQ_GET_DATETIME_YEAR_FROM_C_DATETIME_STRUCT( sTIMESTAMP_TZ_STRUCT->year );
                sDtlExpTime.mMonth = sTIMESTAMP_TZ_STRUCT->month;
                sDtlExpTime.mDay = sTIMESTAMP_TZ_STRUCT->day;
                sDtlExpTime.mHour = sTIMESTAMP_TZ_STRUCT->hour;
                sDtlExpTime.mMinute = sTIMESTAMP_TZ_STRUCT->minute;
                sDtlExpTime.mSecond = sTIMESTAMP_TZ_STRUCT->second;

                sFractionalSecond = ZTQ_GET_DATETIME_FRACTIONAL_SECOND_FROM_C_DATETIME_STRUCT( sTIMESTAMP_TZ_STRUCT->fraction );
                sTimeZone         = ZTQ_GET_TIMEZONE_FROM_C_DATETIME_STRUCT( sTIMESTAMP_TZ_STRUCT->timezone_hour,
                                                                             sTIMESTAMP_TZ_STRUCT->timezone_minute );
                
                STL_TRY( dtlGetToCharDateTime( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                               & sDtlExpTime,
                                               sFractionalSecond,
                                               & sTimeZone,
                                               gZtqNLSTimestampWithTimeZoneFormatInfo,
                                               DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                               aFormatString,
                                               aFormatStringLen,
                                               aErrorStack )
                         == STL_SUCCESS );
                
                break;
            }
        case SQL_INTERVAL_YEAR:
        case SQL_INTERVAL_MONTH:
        case SQL_INTERVAL_YEAR_TO_MONTH:            
            {
                sINTERVAL_STRUCT = (SQL_INTERVAL_STRUCT *)(aTargetColumn->mDataValue);
                
                switch( sINTERVAL_STRUCT->interval_type )
                {
                    case SQL_IS_YEAR_TO_MONTH:
                        {
                            sDtlExpTime.mYear = sINTERVAL_STRUCT->intval.year_month.year;
                            sDtlExpTime.mMonth = sINTERVAL_STRUCT->intval.year_month.month;
                        
                            if( sINTERVAL_STRUCT->interval_sign == (SQLSMALLINT)STL_FALSE )
                            {
                                /* Do Nothing */
                            }
                            else
                            {
                                sDtlExpTime.mYear = -(sDtlExpTime.mYear);
                                sDtlExpTime.mMonth = -(sDtlExpTime.mMonth);
                            }
                        
                            STL_TRY( dtlIntervalYearToMonthToFormatString( & sDtlExpTime,
                                                                           aTargetColumn->mIntervalColInfo.mLeadingPrecision,
                                                                           aFormatString,
                                                                           aFormatStringLen,
                                                                           aErrorStack )
                                     == STL_SUCCESS );
                        
                            break;
                        }
                    default:
                        STL_DASSERT( STL_FALSE );
                        break;
                }
                
                break;
            }
        case SQL_INTERVAL_DAY:
        case SQL_INTERVAL_HOUR:
        case SQL_INTERVAL_MINUTE:
        case SQL_INTERVAL_SECOND:
        case SQL_INTERVAL_DAY_TO_HOUR:
        case SQL_INTERVAL_DAY_TO_MINUTE:
        case SQL_INTERVAL_DAY_TO_SECOND:
        case SQL_INTERVAL_HOUR_TO_MINUTE:
        case SQL_INTERVAL_HOUR_TO_SECOND:
        case SQL_INTERVAL_MINUTE_TO_SECOND:
            {
                sINTERVAL_STRUCT = (SQL_INTERVAL_STRUCT *)(aTargetColumn->mDataValue);

                switch( sINTERVAL_STRUCT->interval_type )
                {
                    case SQL_IS_DAY_TO_SECOND:
                        {
                            sDtlExpTime.mDay = sINTERVAL_STRUCT->intval.day_second.day;
                            sDtlExpTime.mHour= sINTERVAL_STRUCT->intval.day_second.hour;
                            sDtlExpTime.mMinute = sINTERVAL_STRUCT->intval.day_second.minute;
                            sDtlExpTime.mSecond = sINTERVAL_STRUCT->intval.day_second.second;

                            sFractionalSecond = ZTQ_GET_DATETIME_FRACTIONAL_SECOND_FROM_C_DATETIME_STRUCT( sINTERVAL_STRUCT->intval.day_second.fraction );
                        
                            if( sINTERVAL_STRUCT->interval_sign == (SQLSMALLINT)STL_FALSE )
                            {
                                /* Do Nothing */
                            }
                            else
                            {
                                sDtlExpTime.mDay = -(sDtlExpTime.mDay);
                                sDtlExpTime.mHour = -(sDtlExpTime.mHour);
                                sDtlExpTime.mMinute = -(sDtlExpTime.mMinute);
                                sDtlExpTime.mSecond = -(sDtlExpTime.mSecond);

                                sFractionalSecond = -sFractionalSecond;
                            }

                            switch( aTargetColumn->mDataType )
                            {
                                case SQL_INTERVAL_DAY:
                                    sIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY;
                                    break;
                                case SQL_INTERVAL_HOUR:
                                    sIntervalIndicator = DTL_INTERVAL_INDICATOR_HOUR;
                                    break;
                                case SQL_INTERVAL_MINUTE:
                                    sIntervalIndicator = DTL_INTERVAL_INDICATOR_MINUTE;
                                    break;
                                case SQL_INTERVAL_SECOND:
                                    sIntervalIndicator = DTL_INTERVAL_INDICATOR_SECOND;
                                    break;
                                case SQL_INTERVAL_DAY_TO_HOUR:
                                    sIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_HOUR;
                                    break;
                                case SQL_INTERVAL_DAY_TO_MINUTE:
                                    sIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE;
                                    break;
                                case SQL_INTERVAL_DAY_TO_SECOND:
                                    sIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                                    break;
                                case SQL_INTERVAL_HOUR_TO_MINUTE:
                                    sIntervalIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE;
                                    break;
                                case SQL_INTERVAL_HOUR_TO_SECOND:
                                    sIntervalIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND;
                                    break;
                                case SQL_INTERVAL_MINUTE_TO_SECOND:
                                    sIntervalIndicator = DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND;
                                    break;
                                default:
                                    STL_DASSERT( STL_FALSE );
                                    break;
                            }
                        
                            STL_TRY( dtlIntervalDayToSecondToFormatString( sIntervalIndicator,
                                                                           & sDtlExpTime,
                                                                           sFractionalSecond,
                                                                           aTargetColumn->mIntervalColInfo.mLeadingPrecision,
                                                                           aTargetColumn->mIntervalColInfo.mSecondPrecision,
                                                                           aFormatString,
                                                                           aFormatStringLen,
                                                                           aErrorStack )
                                     == STL_SUCCESS );
                        
                            break;
                        }
                    default:
                        STL_DASSERT( STL_FALSE );
                        break;
                }
                
                break;
            }
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}

/**
 * @brief number string value 로 부터 새로운 number string을 생성 ( resizing )
 * @param[in]   aTargetColumn      number string 정보 및 데이터
 * @param[in]   aAvailableSize     resizing 값
 * @param[out]  aNumberString      결과 number string 이 저장될 buffer
 * @param[out]  aNumberStringLen   결과 number string 의 길이
 * @param[out]  aErrorStack        에러 스택
 *
 */
stlStatus ztqResizeNumberString( ztqTargetColumn * aTargetColumn,
                                 stlInt64          aAvailableSize,
                                 stlChar         * aNumberString,
                                 stlInt64        * aNumberStringLen,
                                 stlErrorStack   * aErrorStack )
{
    STL_TRY( dtlResizeNumberStringToString( aTargetColumn->mDataValue,
                                            stlStrlen( aTargetColumn->mDataValue ),
                                            aAvailableSize,
                                            aNumberString,
                                            aNumberStringLen,
                                            aErrorStack )
             == STL_SUCCESS );

    aNumberString[ *aNumberStringLen ] = '\0';
    
    return STL_SUCCESS;
        
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */


