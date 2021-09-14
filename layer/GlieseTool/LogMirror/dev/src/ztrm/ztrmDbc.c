/*******************************************************************************
 * ztrmGeneral.c
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
 * @file ztrmGeneral.c
 * @brief GlieseTool LogMirror General Routines
 */

#include <goldilocks.h>
#include <ztr.h>

stlStatus ztrmAllocEnvHandle( SQLHENV       * aEnvHandle,
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
        (void) ztrmWriteDiagnostic( SQL_HANDLE_ENV,
                                    *aEnvHandle,
                                    aErrorStack );
            
        (void) SQLFreeHandle( SQL_HANDLE_ENV,
                              *aEnvHandle );
        
    }

    return STL_FAILURE;
}

stlStatus ztrmFreeEnvHandle( SQLHENV         aEnvHandle,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN    sReturn;
    
    sReturn = SQLFreeHandle( SQL_HANDLE_ENV,
                             aEnvHandle );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_FREEENV );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_FREEENV )
    {
        (void) ztrmWriteDiagnostic( SQL_HANDLE_ENV,
                                    aEnvHandle,
                                    aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;    
}



stlStatus ztrmSQLAllocStmt( SQLHANDLE       aInputHandle,
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
        (void) ztrmWriteDiagnostic( SQL_HANDLE_DBC,
                                    aInputHandle,
                                    aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrmSQLFreeStmt( SQLHANDLE       aHandle,
                           stlErrorStack * aErrorStack )
{
    SQLRETURN    sReturn;
    
    sReturn = SQLFreeHandle( SQL_HANDLE_STMT,
                             aHandle );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_FREESTMT );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_FREESTMT )
    {
        (void) ztrmWriteDiagnostic( SQL_HANDLE_STMT,
                                    aHandle,
                                    aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}



stlStatus ztrmCreateConnection( SQLHDBC       * aDbcHandle,
                                SQLHENV         aEnvHandle,
                                stlErrorStack * aErrorStack )
{
    stlChar  * sDsn        = ztrmGetDsn();
    stlChar  * sHost       = ztrmGetHostIp();
    stlChar    sConnStr[1024];
    stlInt32   sState      = 0;
    stlInt32   sHostPort;
    
    stlChar    sID[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar    sPW[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    SQLRETURN  sReturn     = SQL_SUCCESS;
    
    STL_TRY( ztrmGetUserId( sID, aErrorStack ) == STL_SUCCESS );
    STL_TRY( ztrmGetUserPw( sPW, aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( SQLAllocHandle( SQL_HANDLE_DBC,
                             aEnvHandle,
                             aDbcHandle ) == SQL_SUCCESS );
    sState = 1;

    if( sHost != NULL )
    {
        STL_TRY( ztrmGetHostPort( &sHostPort, 
                                  aErrorStack ) == STL_SUCCESS );
        
        stlSnprintf( sConnStr,
                     1024,
                     "HOST=%s;UID=%s;PWD=%s;PORT=%d",
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
        STL_THROW( RAMP_CONN_INFO_NOT_EXIST );
    }
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_SERVICE_NOT_AVAILABLE );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_SERVICE_NOT_AVAILABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_SERVICE_NOT_AVAILABLE,
                      NULL,
                      aErrorStack );
    }
    STL_CATCH( RAMP_CONN_INFO_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_CONN_INFO,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztrmPrintDiagnostic( SQL_HANDLE_DBC,
                                    *aDbcHandle,
                                    aErrorStack );
        
        (void) SQLFreeHandle( SQL_HANDLE_DBC,
                              *aDbcHandle );
    }

    return STL_FAILURE;
    
}


stlStatus ztrmDestroyConnection( SQLHDBC         aDbcHandle,
                                 stlErrorStack * aErrorStack )
{
    (void) SQLDisconnect( aDbcHandle );
    
    (void) SQLFreeHandle( SQL_HANDLE_DBC,
                          aDbcHandle );
    
    return STL_SUCCESS;
}


stlStatus ztrmSQLPrepare( SQLHSTMT        aStmtHandle,
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
        (void) ztrmWriteDiagnostic( SQL_HANDLE_STMT,
                                    aStmtHandle,
                                    aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrmSQLBindCol( SQLHSTMT        aStmtHandle,
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
        (void) ztrmWriteDiagnostic( SQL_HANDLE_STMT,
                                    aStmtHandle,
                                    aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztrmSQLExecDirect( SQLHSTMT        aStmtHandle,
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
        (void) ztrmWriteDiagnostic( SQL_HANDLE_STMT,
                                    aStmtHandle,
                                    aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztrmSQLExecute( SQLHSTMT        aStmtHandle,
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
        (void) ztrmWriteDiagnostic( SQL_HANDLE_STMT,
                                    aStmtHandle,
                                    aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztrmSQLFetch( SQLHSTMT        aStmtHandle,
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
        (void) ztrmWriteDiagnostic( SQL_HANDLE_STMT,
                                    aStmtHandle,
                                    aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztrmPrintDiagnostic( SQLSMALLINT     aHandleType,
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
                                           ZTR_DATA_SOURCE_IDENTIFIER ) +
                                stlStrlen( ZTR_DATA_SOURCE_IDENTIFIER ) );

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
                      ZTD_ERRCODE_UNACCEPTABLE_VALUE,
                      NULL,
                      aErrorStack );
        */
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


void ztrmWriteDiagnostic( SQLSMALLINT     aHandleType,
                          SQLHANDLE       aHandle,
                          stlErrorStack * aErrorStack )
{
    stlChar  sDiagMsg[ZTR_TRACE_BUFFER_SIZE] = {0,};
    
    (void) ztrmGetDiagnosticMsg( aHandleType,
                                 aHandle,
                                 1,
                                 sDiagMsg,
                                 aErrorStack );
        
    if( stlStrlen( sDiagMsg ) > 0 )
    {
        (void) ztrmLogMsg( aErrorStack,
                           "%s\n",
                           sDiagMsg );
    }
}

stlStatus ztrmGetDiagnosticMsg( SQLSMALLINT     aHandleType,
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
    stlChar       sMsg[ZTR_TRACE_BUFFER_SIZE] = {0,};

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
                                       ZTR_DATA_SOURCE_IDENTIFIER ) +
                            stlStrlen( ZTR_DATA_SOURCE_IDENTIFIER ) );

    while( stlIsspace( *sRefinedMessageText ) == STL_TRUE )
    {
        sRefinedMessageText++;
    }
  
    stlSnprintf( sMsg,
                 ZTR_TRACE_BUFFER_SIZE,
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


/** @} */

