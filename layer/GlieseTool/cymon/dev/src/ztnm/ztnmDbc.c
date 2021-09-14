/*******************************************************************************
 * ztnmDbc.c
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
 * @file ztnmDbc.c
 * @brief GlieseTool Cyclone Monitor General Routines
 */

#include <goldilocks.h>
#include <ztn.h>

extern ztnConfigure gZtnConfigure;
extern ztnManager   gZtnManager;

extern const stlChar *const gZtnMasterStateString[];
extern const stlChar *const gZtnSlaveStateString[];

stlStatus ztnmAllocEnvHandle( SQLHENV       * aEnvHandle,
                              stlErrorStack * aErrorStack )
{
    stlInt32 sState = 0;
    
    STL_TRY( SQLAllocHandle( SQL_HANDLE_ENV,
                             NULL,
                             aEnvHandle ) == SQL_SUCCESS );
    sState = 1;
    
    STL_TRY( SQLSetEnvAttr( *aEnvHandle,
                            SQL_ATTR_ODBC_VERSION,
                            (SQLPOINTER)3,
                            0 ) == SQL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztnmWriteDiagnostic( SQL_HANDLE_ENV,
                                    *aEnvHandle,
                                    aErrorStack );
            
        (void) SQLFreeHandle( SQL_HANDLE_ENV,
                              *aEnvHandle );
    }

    return STL_FAILURE;
}


stlStatus ztnmFreeEnvHandle( SQLHENV         aEnvHandle,
                             stlErrorStack * aErrorStack )
{
    (void) SQLFreeHandle( SQL_HANDLE_ENV,
                          aEnvHandle );
    
    return STL_SUCCESS;    
}



stlStatus ztnmCreateConnection( SQLHDBC       * aDbcHandle,
                                SQLHENV         aEnvHandle,
                                stlErrorStack * aErrorStack )
{
    stlChar  * sServerName = "GOLDILOCKS";
    stlChar  * sDsn        = ztnmGetDsn();
    stlChar  * sHost       = ztnmGetHostIp();
    stlInt32   sHostPort   = 0;
    stlInt32   sState      = 0;
    SQLRETURN  sReturn     = SQL_SUCCESS;
    
    stlChar    sConnStr[1024];
    stlChar    sID[ STL_MAX_SQL_IDENTIFIER_LENGTH ];
    stlChar    sPW[ STL_MAX_SQL_IDENTIFIER_LENGTH ];
    
    
    STL_TRY( ztnmGetUserId( sID, aErrorStack ) == STL_SUCCESS );
    STL_TRY( ztnmGetUserPw( sPW, aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( SQLAllocHandle( SQL_HANDLE_DBC,
                             aEnvHandle,
                             aDbcHandle ) == SQL_SUCCESS );
    sState = 1;
    
    if( sHost != NULL )
    {
        STL_TRY( ztnmGetHostPort( &sHostPort, 
                                  aErrorStack ) == STL_SUCCESS );
        
        stlSnprintf( sConnStr,
                     1024,
                     "HOST=%s;UID=%s;PWD=%s;PORT=%d",       //PROTOCOL=DA/TCP
                     sHost,
                     sID,
                     sPW,
                     sHostPort );   
        
        sReturn = SQLDriverConnect( *aDbcHandle,
                                    NULL,
                                    (SQLCHAR *)sConnStr,
                                    SQL_NTS,
                                    NULL,
                                    0,
                                    NULL,
                                    SQL_DRIVER_NOPROMPT );
    }
    else if( sDsn != NULL )
    {
        sReturn = SQLConnect( *aDbcHandle,
                              (SQLCHAR*)sDsn,
                              SQL_NTS,
                              (SQLCHAR*)sID,
                              SQL_NTS,
                              (SQLCHAR*)sPW,
                              SQL_NTS );
    }
    else
    {
        sReturn = SQLConnect( *aDbcHandle,
                              (SQLCHAR*)sServerName,
                              SQL_NTS,
                              (SQLCHAR*)sID,
                              SQL_NTS,
                              (SQLCHAR*)sPW,
                              SQL_NTS );   
    }
    
    STL_TRY( SQL_SUCCEEDED( sReturn ) );
    
    STL_TRY( SQLSetConnectAttr( *aDbcHandle,
                                SQL_ATTR_AUTOCOMMIT,
                                (SQLPOINTER)SQL_AUTOCOMMIT_ON,
                                SQL_IS_UINTEGER ) == SQL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) SQLFreeHandle( SQL_HANDLE_DBC,
                              *aDbcHandle );
    }

    return STL_FAILURE;
}


stlStatus ztnmDestroyConnection( SQLHDBC         aDbcHandle,
                                 stlErrorStack * aErrorStack )
{
    (void) SQLDisconnect( aDbcHandle );
    
    (void) SQLFreeHandle( SQL_HANDLE_DBC,
                          aDbcHandle );
    
    return STL_SUCCESS;
}


void ztnmWriteDiagnostic( SQLSMALLINT     aHandleType,
                          SQLHANDLE       aHandle,
                          stlErrorStack * aErrorStack )
{
    stlChar  sDiagMsg[ZTN_TRACE_BUFFER_SIZE] = {0,};
    
    (void) ztnmGetDiagnosticMsg( aHandleType,
                                 aHandle,
                                 1,
                                 sDiagMsg,
                                 aErrorStack );
        
    if( stlStrlen( sDiagMsg ) > 0 )
    {
        (void) ztnmLogMsg( aErrorStack,
                           "%s\n",
                           sDiagMsg );
    }
}


stlStatus ztnmGetDiagnosticMsg( SQLSMALLINT     aHandleType,
                                SQLHANDLE       aHandle,
                                SQLSMALLINT     aRecNumber,
                                stlChar       * aMsg,
                                stlErrorStack * aErrorStack )
{
    SQLCHAR       sSqlState[6];
    SQLINTEGER    sNaiveError;
    SQLSMALLINT   sTextLength;
    SQLCHAR       sMessageText[SQL_MAX_MESSAGE_LENGTH];
    SQLRETURN     sReturn;
    stlChar     * sRefinedMessageText;
    stlChar       sMsg[ZTN_TRACE_BUFFER_SIZE] = {0,};

    sReturn = SQLGetDiagRec( aHandleType,
                             aHandle,
                             aRecNumber,
                             sSqlState,
                             &sNaiveError,
                             sMessageText,
                             SQL_MAX_MESSAGE_LENGTH,
                             &sTextLength );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_INVALID_VALUE );
    
    sSqlState[5] = '\0';
    sRefinedMessageText = ( stlStrstr( (stlChar *) sMessageText,
                                       ZTN_DATA_SOURCE_IDENTIFIER ) +
                            stlStrlen( ZTN_DATA_SOURCE_IDENTIFIER ) );

    while( stlIsspace( *sRefinedMessageText ) == STL_TRUE )
    {
        sRefinedMessageText++;
    }
  
    stlSnprintf( sMsg,
                 ZTN_TRACE_BUFFER_SIZE,
                 "%sERR-%s(%d) : %s",
                 sMsg,
                 sSqlState, sNaiveError, sRefinedMessageText );
    
    stlMemcpy( aMsg, sMsg, stlStrlen( sMsg ) );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        /*
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_UNACCEPTABLE_VALUE,
                      NULL,
                      aErrorStack );
        */
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


stlStatus ztnmPrintDiagnostic( SQLSMALLINT     aHandleType,
                               SQLHANDLE       aHandle,
                               stlErrorStack * aErrorStack )
{
    SQLCHAR       sSqlState[6];
    SQLINTEGER    sNaiveError;
    SQLSMALLINT   sTextLength;
    SQLCHAR       sMessageText[SQL_MAX_MESSAGE_LENGTH];
    stlChar     * sRefinedMessageText;
    SQLSMALLINT   sRecNumber = 1;
    SQLRETURN     sReturn;

    while( STL_TRUE )
    {
        sReturn = SQLGetDiagRec( aHandleType,
                                 aHandle,
                                 sRecNumber,
                                 sSqlState,
                                 &sNaiveError,
                                 sMessageText,
                                 SQL_MAX_MESSAGE_LENGTH,
                                 &sTextLength );
        
        if( sReturn == SQL_NO_DATA )
        {
            break;
        }

        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_INVALID_VALUE );
        
        sSqlState[5] = '\0';
        sRefinedMessageText = ( stlStrstr( (stlChar *) sMessageText,
                                           ZTN_DATA_SOURCE_IDENTIFIER ) +
                                stlStrlen( ZTN_DATA_SOURCE_IDENTIFIER ) );

        while( stlIsspace( *sRefinedMessageText ) == STL_TRUE )
        {
            sRefinedMessageText++;
        }
      
        stlPrintf("ERR-%s(%d): %s\n", sSqlState, sNaiveError, sRefinedMessageText );

        sRecNumber++;
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        /*
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_UNACCEPTABLE_VALUE,
                      NULL,
                      aErrorStack );
        */
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


stlStatus ztnmGetLogFileCount( SQLHDBC         aDbcHandle,
                               stlInt32      * aCount,
                               stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTN_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32      sLogFileCount = 0;
    stlInt32      sState        = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLRETURN     sResult;
    SQLLEN        sIndicator;
    
    STL_TRY( ztnmSQLAllocStmt( aDbcHandle,
                               &sStmtHandle,
                               aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    stlSnprintf( sSqlString,
                 ZTN_COMMAND_BUFFER_SIZE,
                 "SELECT COUNT(*) "
                 "FROM FIXED_TABLE_SCHEMA.X$LOG_GROUP " );

    STL_TRY( ztnmSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SLONG, 
                             &sLogFileCount,
                             STL_SIZEOF( sLogFileCount ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INTERNAL );
    
    *aCount = sLogFileCount;
    
    sState = 0;
    STL_TRY( ztnmSQLFreeStmt( sStmtHandle, 
                              aErrorStack ) == STL_SUCCESS ); 
    
    return STL_SUCCESS;  
     
    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Get LogFileCount" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztnmSQLFreeStmt( sStmtHandle, 
                                aErrorStack );
    }

    return STL_FAILURE;
    
}

stlStatus ztnmBuildLogFileInfo( SQLHDBC         aDbcHandle,
                                stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTN_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32      sLogFileCount = 0;
    stlInt16      sGroupId      = 0;
    stlInt64      sFileSize     = 0;
    stlInt32      sState        = 0;
    stlInt32      sIdx          = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLRETURN     sResult;
    SQLLEN        sIndicator[2];
    
    STL_TRY( ztnmGetLogFileCount( aDbcHandle,
                                  &sLogFileCount,
                                  aErrorStack ) == STL_SUCCESS );
    
    STL_DASSERT( sLogFileCount > 0 );
    
    STL_TRY( stlAllocRegionMem( &gZtnManager.mAllocator,
                                STL_SIZEOF( ztnLogFileInfo ) * sLogFileCount,
                                (void**)&gZtnManager.mLogFileInfo,
                                aErrorStack ) == STL_SUCCESS );
        
    STL_TRY( ztnmSQLAllocStmt( aDbcHandle,
                               &sStmtHandle,
                               aErrorStack ) == STL_SUCCESS );
    sState = 1;
        

    stlSnprintf( sSqlString,
                 ZTN_COMMAND_BUFFER_SIZE,
                 "SELECT GROUP_ID, FILE_SIZE "
                 "FROM FIXED_TABLE_SCHEMA.X$LOG_GROUP " 
                 "ORDER BY GROUP_ID ASC");
    
    STL_TRY( ztnmSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SSHORT, 
                             &sGroupId,
                             STL_SIZEOF( sGroupId ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_SBIGINT, 
                             &sFileSize,
                             STL_SIZEOF( sFileSize ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    sIdx = 0;
    while( 1 )
    {
        STL_TRY( ztnmSQLFetch( sStmtHandle, 
                               &sResult, 
                               aErrorStack ) == STL_SUCCESS );
        
        if( sResult == SQL_NO_DATA )
        {
            break;
        }
        
        gZtnManager.mLogFileInfo[sIdx].mGroupId    = sGroupId;
        gZtnManager.mLogFileInfo[sIdx].mFileSize   = sFileSize;
        gZtnManager.mLogFileInfo[sIdx].mBlockCount = sFileSize / ZTN_DEFAULT_LOG_BLOCK_SIZE;
        
        sIdx++;
    }
    
    STL_DASSERT( sIdx == sLogFileCount );
    
    gZtnManager.mLogFileCount = sLogFileCount;
    
    sState = 0;
    STL_TRY( ztnmSQLFreeStmt( sStmtHandle, 
                              aErrorStack ) == STL_SUCCESS ); 
    
    return STL_SUCCESS;  
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztnmSQLFreeStmt( sStmtHandle, 
                                aErrorStack );
    }

    return STL_FAILURE;
}
stlStatus ztnmGetDbStatus( SQLHDBC         aDbcHandle,
                           stlInt16      * aGroupId,
                           stlInt64      * aFileSeqNo,
                           stlInt32      * aBlockSeq,
                           stlErrorStack * aErrorStack )
{
    stlChar          sSqlString[ZTN_COMMAND_BUFFER_SIZE] = {0,};
    stlInt16         sGroupId     = 0;
    stlInt32         sBlockSeq    = 0;
    stlInt64         sFileSeqNo   = 0;
    stlInt32         sState       = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLRETURN     sResult;
    SQLLEN        sIndicator[3];
    
    stlSnprintf( sSqlString,
                 ZTN_COMMAND_BUFFER_SIZE,
                 "SELECT GROUP_ID, GRP.FILE_SEQ_NO, BUF.REAR_LID_BLOCK_SEQ_NO "
                 "FROM FIXED_TABLE_SCHEMA.X$LOG_GROUP GRP, FIXED_TABLE_SCHEMA.X$LOG_BUFFER BUF " 
                 "WHERE BUF.FILE_SEQ_NO=GRP.FILE_SEQ_NO ");
    
    STL_TRY( ztnmSQLAllocStmt( aDbcHandle,
                               &sStmtHandle,
                               aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztnmSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SSHORT, 
                             &sGroupId,
                             STL_SIZEOF( sGroupId ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_SBIGINT, 
                             &sFileSeqNo,
                             STL_SIZEOF( sFileSeqNo ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLBindCol( sStmtHandle,
                             3,
                             SQL_C_SLONG, 
                             &sBlockSeq,
                             STL_SIZEOF( sBlockSeq ),
                             &sIndicator[2],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( sResult != SQL_NO_DATA );

    sState = 0;
    STL_TRY( ztnmSQLFreeStmt( sStmtHandle, 
                              aErrorStack ) == STL_SUCCESS );   
    
    *aGroupId   = sGroupId;
    *aFileSeqNo = sFileSeqNo;
    *aBlockSeq  = sBlockSeq;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztnmSQLFreeStmt( sStmtHandle, 
                                aErrorStack );
    }

    return STL_FAILURE;
}

stlStatus ztnmInsertBlankMonitorInfo( SQLHDBC         aDbcHandle,
                                      stlErrorStack * aErrorStack )
{
    ztnGroupItem * sGroup;
    stlChar        sSqlString[ZTN_COMMAND_BUFFER_SIZE];    
    stlInt32       sState = 0;
    
    SQLHSTMT       sStmtHandle;
    SQLRETURN      sResult;
    
    STL_TRY( ztnmSQLAllocStmt( aDbcHandle,
                               &sStmtHandle,
                               aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * 기존 Record 제거
     */
    stlSnprintf( sSqlString,
                 ZTN_COMMAND_BUFFER_SIZE,
                 "DELETE FROM %s", 
                 ZTN_INFO_TABLE_NAME );
    
    STL_TRY( ztnmSQLExecDirect( sStmtHandle, 
                                (SQLCHAR*)sSqlString, 
                                stlStrlen( sSqlString ),
                                &sResult,
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS || sResult == SQL_NO_DATA, RAMP_ERR_DELETE );
    
    STL_RING_FOREACH_ENTRY( &gZtnManager.mGroupList, sGroup )
    {
        stlSnprintf( sSqlString,
                     ZTN_COMMAND_BUFFER_SIZE,
                     "INSERT INTO %s VALUES( '%s', TO_CHAR( SYSDATE, 'YYYY-MM-DD HH24:MI:SS' ), "
                     "'%s','%s', %d, '', 0, 0, 0, 0, 0, 0 )", 
                     ZTN_INFO_TABLE_NAME,
                     sGroup->mName, 
                     gZtnMasterStateString[ ZTN_MASTER_STATE_NA ],
                     gZtnSlaveStateString[ ZTN_SLAVE_STATE_NA ],
                     sGroup->mPort );
        
        STL_TRY( ztnmSQLExecDirect( sStmtHandle, 
                                    (SQLCHAR*)sSqlString, 
                                    stlStrlen( sSqlString ),
                                    &sResult,
                                    aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_INSERT );
    }
    
    sState = 0;
    STL_TRY( ztnmSQLFreeStmt( sStmtHandle, 
                              aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_DELETE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Delete Monitor Record" );
    }
    STL_CATCH( RAMP_ERR_INSERT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Insert Monitor Record for Init" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztnmSQLFreeStmt( sStmtHandle, 
                                aErrorStack );
    }
    
    return STL_FAILURE;
}


stlStatus ztnmInitUpdateMonitorStmt( SQLHDBC         aDbcHandle,
                                     SQLHSTMT        aStmtHandle,
                                     ztnParameter  * aParameter,
                                     stlErrorStack * aErrorStack )
{
    stlChar sSqlString[ZTN_COMMAND_BUFFER_SIZE];
    
    stlSnprintf( sSqlString,
                 ZTN_COMMAND_BUFFER_SIZE,
                 "UPDATE %s SET TIME=TO_CHAR( SYSDATE, 'YYYY-MM-DD HH24:MI:SS' ), "
                 "MASTER_STATE=?, SLAVE_STATE=?, SLAVE_IP=?, "
                 "REDO_LOG_FILESEQ=?, REDO_LOG_BLOCKSEQ=?, CAPTURE_FILESEQ=?, CAPTURE_BLOCKSEQ=?, CAPTURE_INTERVAL=?, CAPTURE_INTERVAL_SIZE=? "
                 "WHERE GROUP_NAME=? ",
                 ZTN_INFO_TABLE_NAME );
    
    STL_TRY( ztnmSQLPrepare( aStmtHandle,
                            (SQLCHAR*)sSqlString,
                            stlStrlen( sSqlString ),
                            aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLBindParameter( aStmtHandle,
                                   1,
                                   SQL_PARAM_INPUT,
                                   SQL_C_CHAR,
                                   SQL_VARCHAR,
                                   10,
                                   0,
                                   aParameter->mMasterState,
                                   10,
                                   &aParameter->mMasterStateInd,
                                   aErrorStack ) == STL_SUCCESS );
     
    STL_TRY( ztnmSQLBindParameter( aStmtHandle,
                                   2,
                                   SQL_PARAM_INPUT,
                                   SQL_C_CHAR,
                                   SQL_VARCHAR,
                                   10,
                                   0,
                                   aParameter->mSlaveState,
                                   10,
                                   &aParameter->mSlaveStateInd,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLBindParameter( aStmtHandle,
                                   3,
                                   SQL_PARAM_INPUT,
                                   SQL_C_CHAR,
                                   SQL_VARCHAR,
                                   20,
                                   0,
                                   aParameter->mSlaveIp,
                                   20,
                                   &aParameter->mSlaveIpInd,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLBindParameter( aStmtHandle,
                                   4,
                                   SQL_PARAM_INPUT,
                                   SQL_C_SBIGINT,
                                   SQL_BIGINT,
                                   0,
                                   0,
                                   &aParameter->mRedoFileSeqNo,
                                   0,
                                   &aParameter->mRedoFileSeqNoInd,
                                   aErrorStack ) == STL_SUCCESS );
     
    STL_TRY( ztnmSQLBindParameter( aStmtHandle,
                                   5,
                                   SQL_PARAM_INPUT,
                                   SQL_C_SLONG,
                                   SQL_INTEGER,
                                   0,
                                   0,
                                   &aParameter->mRedoBlockSeq,
                                   0,
                                   &aParameter->mRedoBlockSeqInd,
                                   aErrorStack ) == STL_SUCCESS );

    
    STL_TRY( ztnmSQLBindParameter( aStmtHandle,
                                   6,
                                   SQL_PARAM_INPUT,
                                   SQL_C_SBIGINT,
                                   SQL_BIGINT,
                                   0,
                                   0,
                                   &aParameter->mCaptureFileSeqNo,
                                   0,
                                   &aParameter->mCaptureFileSeqNoInd,
                                   aErrorStack ) == STL_SUCCESS );
     
    STL_TRY( ztnmSQLBindParameter( aStmtHandle,
                                   7,
                                   SQL_PARAM_INPUT,
                                   SQL_C_SLONG,
                                   SQL_INTEGER,
                                   0,
                                   0,
                                   &aParameter->mCaptureBlockSeq,
                                   0,
                                   &aParameter->mCaptureBlockSeqInd,
                                   aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztnmSQLBindParameter( aStmtHandle,
                                   8,
                                   SQL_PARAM_INPUT,
                                   SQL_C_SBIGINT,
                                   SQL_BIGINT,
                                   0,
                                   0,
                                   &aParameter->mInterval,
                                   0,
                                   &aParameter->mIntervalInd,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLBindParameter( aStmtHandle,
                                   9,
                                   SQL_PARAM_INPUT,
                                   SQL_C_SBIGINT,
                                   SQL_BIGINT,
                                   0,
                                   0,
                                   &aParameter->mIntervalSize,
                                   0,
                                   &aParameter->mIntervalSizeInd,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztnmSQLBindParameter( aStmtHandle,
                                   10,
                                   SQL_PARAM_INPUT,
                                   SQL_C_CHAR,
                                   SQL_VARCHAR,
                                   128,
                                   0,
                                   aParameter->mGroupName,
                                   128,
                                   &aParameter->mGroupNameInd,
                                   aErrorStack ) == STL_SUCCESS );
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmUpdateMonitorInfo( SQLHDBC         aDbcHandle,
                                 SQLHSTMT        aStmtHandle,
                                 ztnParameter  * aParameter,
                                 stlInt16        aGroupId,
                                 stlInt64        aFileSeqNo,
                                 stlInt32        aBlockSeq,
                                 stlErrorStack * aErrorStack )
{
    ztnGroupItem * sGroup;
    stlInt64       sInterval     = 0;
    stlInt64       sIntervalSize = 0;
    
    SQLRETURN      sResult;
    
    STL_RING_FOREACH_ENTRY( &gZtnManager.mGroupList, sGroup )
    {
        if( sGroup->mIsConnect == STL_TRUE )
        {
            if( sGroup->mMonitorInfo.mSlaveState == ZTN_SLAVE_STATE_NA )
            {
                /**
                 * CYMON은 MASTER에 정상적으로 접속하였으나, SLAVE가 아직 접속이 안되었을 경우.
                 */
                stlStrncpy( aParameter->mMasterState,
                            gZtnMasterStateString[ sGroup->mMonitorInfo.mMasterState ],
                            stlStrlen( gZtnMasterStateString[ sGroup->mMonitorInfo.mMasterState ] ) + 1);
                        
                aParameter->mMasterStateInd = stlStrlen( gZtnMasterStateString[ sGroup->mMonitorInfo.mMasterState ] );
            
                stlStrncpy( aParameter->mSlaveState,
                            gZtnSlaveStateString[ sGroup->mMonitorInfo.mSlaveState ],
                            stlStrlen( gZtnSlaveStateString[ sGroup->mMonitorInfo.mSlaveState ] ) + 1);
                        
                aParameter->mSlaveStateInd = stlStrlen( gZtnSlaveStateString[ sGroup->mMonitorInfo.mSlaveState ] );
                
                aParameter->mSlaveIpInd = SQL_NULL_DATA;
            
                stlStrncpy( aParameter->mGroupName,
                            sGroup->mName,
                            stlStrlen( sGroup->mName ) + 1 );
                
                aParameter->mGroupNameInd = stlStrlen( sGroup->mName );
                
                aParameter->mRedoFileSeqNo    = aFileSeqNo;
                aParameter->mCaptureFileSeqNo = 0;
                aParameter->mRedoBlockSeq     = aBlockSeq;
                aParameter->mCaptureBlockSeq  = 0;
                aParameter->mInterval         = 0;
                aParameter->mIntervalSize     = 0;
            }
            else
            {
                STL_TRY( ztnmGetIntervalInfo( aGroupId,
                                              aFileSeqNo,
                                              aBlockSeq,
                                              sGroup->mMonitorInfo.mFileSeqNo,
                                              sGroup->mMonitorInfo.mBlockSeq,
                                              &sInterval,
                                              &sIntervalSize,
                                              aErrorStack ) == STL_SUCCESS );
                
                stlStrncpy( aParameter->mMasterState,
                            gZtnMasterStateString[ sGroup->mMonitorInfo.mMasterState ],
                            stlStrlen( gZtnMasterStateString[ sGroup->mMonitorInfo.mMasterState ] ) + 1);
                        
                aParameter->mMasterStateInd = stlStrlen( gZtnMasterStateString[ sGroup->mMonitorInfo.mMasterState ] );
            
                stlStrncpy( aParameter->mSlaveState,
                            gZtnSlaveStateString[ sGroup->mMonitorInfo.mSlaveState ],
                            stlStrlen( gZtnSlaveStateString[ sGroup->mMonitorInfo.mSlaveState ] ) + 1);
                        
                aParameter->mSlaveStateInd = stlStrlen( gZtnSlaveStateString[ sGroup->mMonitorInfo.mSlaveState ] );
                
                stlStrncpy( aParameter->mSlaveIp,
                            sGroup->mMonitorInfo.mSlaveIp,
                            stlStrlen( sGroup->mMonitorInfo.mSlaveIp ) + 1);
                        
                aParameter->mSlaveIpInd = stlStrlen( sGroup->mMonitorInfo.mSlaveIp );

                stlStrncpy( aParameter->mGroupName,
                            sGroup->mName,
                            stlStrlen( sGroup->mName ) + 1 );
                
                aParameter->mGroupNameInd = stlStrlen( sGroup->mName );
                
                aParameter->mRedoFileSeqNo    = aFileSeqNo;
                aParameter->mCaptureFileSeqNo = sGroup->mMonitorInfo.mFileSeqNo;
                aParameter->mRedoBlockSeq     = aBlockSeq;
                aParameter->mCaptureBlockSeq  = sGroup->mMonitorInfo.mBlockSeq;
                aParameter->mInterval         = sInterval;
                aParameter->mIntervalSize     = sIntervalSize;
            }
        }
        else
        {
            /**
             * CYMON이 아직 MASTER에 접속하지 못했을 경우.
             */
            stlStrncpy( aParameter->mMasterState,
                        gZtnMasterStateString[ ZTN_MASTER_STATE_NA ],
                        stlStrlen( gZtnMasterStateString[ ZTN_MASTER_STATE_NA ] ) + 1);
                        
            aParameter->mMasterStateInd = stlStrlen( gZtnMasterStateString[ ZTN_MASTER_STATE_NA ] );
            
            stlStrncpy( aParameter->mSlaveState,
                        gZtnSlaveStateString[ ZTN_SLAVE_STATE_NA ],
                        stlStrlen( gZtnSlaveStateString[ ZTN_SLAVE_STATE_NA ] ) + 1);
                        
            aParameter->mSlaveStateInd = stlStrlen( gZtnMasterStateString[ ZTN_MASTER_STATE_NA ] );
            aParameter->mSlaveIpInd = SQL_NULL_DATA;
            
            stlStrncpy( aParameter->mGroupName,
                        sGroup->mName,
                        stlStrlen( sGroup->mName ) + 1 );
            
            aParameter->mGroupNameInd = stlStrlen( sGroup->mName );
            
            aParameter->mRedoFileSeqNo    = 0;
            aParameter->mCaptureFileSeqNo = 0;
            aParameter->mRedoBlockSeq     = 0;
            aParameter->mCaptureBlockSeq  = 0;
            aParameter->mInterval         = 0;
            aParameter->mIntervalSize     = 0;
        }
        
        sResult = SQLExecute( aStmtHandle );
    
        STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_UPDATE_STATE );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_UPDATE_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_UPDATE_INFO_FAIL,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmCheckTableExist( SQLHDBC         aDbcHandle,
                               stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTN_COMMAND_BUFFER_SIZE] = {0,};
    stlChar       sCreateString[ZTN_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32      sState = 0;
    
    SQLHSTMT      sStmtHandle;
    
    STL_TRY( ztnmSQLAllocStmt( aDbcHandle,
                               &sStmtHandle,
                               aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    stlSnprintf( sSqlString,
                 ZTN_COMMAND_BUFFER_SIZE,
                 "SELECT * "
                 "FROM CYCLONE_MONITOR_INFO" );
    
    if( SQLPrepare( sStmtHandle,
                    (SQLCHAR*)sSqlString,
                    stlStrlen( sSqlString ) ) != SQL_SUCCESS )
    {
        stlSnprintf( sCreateString,
                     ZTN_COMMAND_BUFFER_SIZE,
                     "CREATE TABLE CYCLONE_MONITOR_INFO"
                     "( GROUP_NAME VARCHAR(128), TIME VARCHAR(20), MASTER_STATE VARCHAR(10), SLAVE_STATE VARCHAR(10), MASTER_PORT INTEGER, SLAVE_IP VARCHAR(20),"
                     " REDO_LOG_FILESEQ BIGINT, REDO_LOG_BLOCKSEQ INTEGER, CAPTURE_FILESEQ BIGINT, CAPTURE_BLOCKSEQ INTEGER,"
                     " CAPTURE_INTERVAL BIGINT, CAPTURE_INTERVAL_SIZE BIGINT )" );
        
        if( SQLExecDirect( sStmtHandle, 
                           (SQLCHAR*)sCreateString, 
                           stlStrlen( sCreateString ) ) != SQL_SUCCESS )
        {
            /** TODO : Already Created ? */
        }
    }
    
    sState = 0;
    STL_TRY( ztnmSQLFreeStmt( sStmtHandle,
                              aErrorStack ) == SQL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) ztnmSQLFreeStmt( sStmtHandle,
                                    aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztnmSQLAllocStmt( SQLHANDLE       aInputHandle,
                            SQLHANDLE     * aOutputHandlePtr,
                            stlErrorStack * aErrorStack )
{
    SQLRETURN    sReturn;
    
    sReturn = SQLAllocHandle( SQL_HANDLE_STMT,
                              aInputHandle,
                              aOutputHandlePtr );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_ALLOCSTMT );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_ALLOCSTMT )
    {
        ztnmWriteDiagnostic( SQL_HANDLE_DBC,
                             aInputHandle,
                             aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmSQLFreeStmt( SQLHANDLE       aHandle,
                           stlErrorStack * aErrorStack )
{
    SQLRETURN    sReturn;
    
    sReturn = SQLFreeHandle( SQL_HANDLE_STMT,
                             aHandle );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_FREESTMT );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_FREESTMT )
    {
        ztnmWriteDiagnostic( SQL_HANDLE_STMT,
                             aHandle,
                             aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmSQLPrepare( SQLHSTMT        aStmtHandle,
                          SQLCHAR       * aStmtText,
                          SQLINTEGER      aTextLength,
                          stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;
    
    sReturn = SQLPrepare( aStmtHandle,
                          aStmtText,
                          aTextLength );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_PREPARE );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_PREPARE )
    {
        ztnmWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmSQLBindCol( SQLHSTMT        aStmtHandle,
                          SQLUSMALLINT    aColumnNumber,
                          SQLSMALLINT     aTargetType,
                          SQLPOINTER      aTargetValuePtr,
                          SQLLEN          aBufferLength,
                          SQLLEN        * aIndicator,
                          stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;
    
    sReturn = SQLBindCol( aStmtHandle,
                          aColumnNumber,
                          aTargetType,
                          aTargetValuePtr,
                          aBufferLength,
                          aIndicator );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_BINDCOL );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_BINDCOL )
    {
        ztnmWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmSQLBindParameter( SQLHSTMT        aStmtHandle,
                                SQLUSMALLINT    aParameterNumber,
                                SQLSMALLINT     aInputOutputType,
                                SQLSMALLINT     aValueType,
                                SQLSMALLINT     aParameterType,
                                SQLULEN         aColumnSize,
                                SQLSMALLINT     aDecimalDigits,
                                SQLPOINTER      aParameterValuePtr,
                                SQLLEN          aBufferLength,
                                SQLLEN        * aIndicator,
                                stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;
    
    sReturn = SQLBindParameter( aStmtHandle,
                                aParameterNumber,
                                aInputOutputType,
                                aValueType,
                                aParameterType,
                                aColumnSize,
                                aDecimalDigits,
                                aParameterValuePtr,
                                aBufferLength,
                                aIndicator );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_BINDPARAMETER );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_BINDPARAMETER )
    {
        ztnmWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmSQLExecute( SQLHSTMT        aStmtHandle,
                          SQLRETURN     * aReturn,
                          stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;
    
    sReturn = SQLExecute( aStmtHandle );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ) || (sReturn == SQL_NO_DATA),
                   RAMP_ERR_EXECUTE );
    
    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXECUTE )
    {
        ztnmWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztnmSQLExecDirect( SQLHSTMT        aStmtHandle,
                             SQLCHAR       * aStmtText,
                             SQLINTEGER      aTextLength,
                             SQLRETURN     * aReturn,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;
    
    sReturn = SQLExecDirect( aStmtHandle,
                             aStmtText,
                             aTextLength );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ) || (sReturn == SQL_NO_DATA),
                   RAMP_ERR_EXECDIRECT );

    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXECDIRECT )
    {
        ztnmWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztnmSQLFetch( SQLHSTMT        aStmtHandle,
                        SQLRETURN     * aReturn,
                        stlErrorStack * aErrorStack )
{
    SQLRETURN sReturn;
    
    sReturn = SQLFetch( aStmtHandle );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ) || (sReturn == SQL_NO_DATA),
                   RAMP_ERR_FETCH );

    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FETCH )
    {
        ztnmWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
