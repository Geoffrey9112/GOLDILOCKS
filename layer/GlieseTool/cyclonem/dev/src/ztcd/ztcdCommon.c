/*******************************************************************************
 * ztcdCommon.c
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
 * @file ztcdCommon.c
 * @brief GlieseTool Cyclone ODBC Operation Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcSlaveMgr   * gSlaveMgr;
extern ztcConfigure    gConfigure;

stlStatus ztcdAllocEnvHandle( SQLHENV       * aEnvHandle,
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
        (void) ztcdPrintDiagnostic( SQL_HANDLE_ENV,
                                    *aEnvHandle,
                                    aErrorStack );
            
        (void) SQLFreeHandle( SQL_HANDLE_ENV,
                              *aEnvHandle );
        
    }

    return STL_FAILURE;
}

stlStatus ztcdCreateConnection( SQLHDBC       * aDbcHandle,
                                SQLHENV         aEnvHandle,
                                stlBool         aIsAutoCommit,
                                stlErrorStack * aErrorStack )
{
    stlChar  * sDsn        = ztcmGetDsn();
    stlChar  * sHost       = ztcmGetHostIp();
    stlInt32   sState      = 0;
    SQLRETURN  sReturn     = SQL_SUCCESS;
    
    stlChar    sConnStr[1024];
    stlChar    sID[ STL_MAX_SQL_IDENTIFIER_LENGTH ];
    stlChar    sPW[ STL_MAX_SQL_IDENTIFIER_LENGTH ];
    stlInt32   sHostPort;
    
    STL_TRY( ztcmGetUserId( sID, aErrorStack ) == STL_SUCCESS );
    STL_TRY( ztcmGetUserPw( sPW, aErrorStack ) == STL_SUCCESS );
    
    /**
     * TODO: USER 생성시에는 사용자가 입력한 계정으로 접속해야함!! 
     */
    STL_TRY( SQLAllocHandle( SQL_HANDLE_DBC,
                             aEnvHandle,
                             aDbcHandle ) == SQL_SUCCESS );
    sState = 1;
    
    if( sHost != NULL )
    {
        /**
         * Host IP/Port를 지정했을 경우에는 TCP로 접속
         * 만약 DA로 접속하길 원할 경우에는 HostIP/Port를 지정하지 않도록 한다.
         */
        STL_TRY( ztcmGetHostPort( &sHostPort, 
                                  aErrorStack ) == STL_SUCCESS );
        
        stlSnprintf( sConnStr,
                     1024,
                     "HOST=%s;UID=%s;PWD=%s;PORT=%d;PROTOCOL=TCP",       //PROTOCOL=DA/TCP
                     sHost,
                     sID,
                     sPW,
                     sHostPort );   
        
    }
    else if( sDsn != NULL )
    {
        /**
         * odbc.ini
         */
        stlSnprintf( sConnStr,
                     1024,
                     "DSN=%s;UID=%s;PWD=%s",       //PROTOCOL=DA/TCP
                     sDsn,
                     sID,
                     sPW );
    }
    else
    {
        /**
         * DA로 접속..
         */
        stlSnprintf( sConnStr,
                     1024,
                     "UID=%s;PWD=%s;PROTOCOL=DA",       //PROTOCOL=DA/TCP
                     sID,
                     sPW );
    }
//    stlDebug("local conn info : %s \n", sConnStr);
    sReturn = SQLDriverConnect( *aDbcHandle,
                                NULL,
                                (SQLCHAR *)sConnStr,
                                SQL_NTS,
                                NULL,
                                0,
                                NULL,
                                SQL_DRIVER_NOPROMPT );
//    stlDebug("local conn info : %d \n", sReturn);

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_SERVICE_NOT_AVAILABLE );
    
    if( aIsAutoCommit == STL_TRUE )
    {
        STL_TRY( SQLSetConnectAttr( *aDbcHandle,
                                    SQL_ATTR_AUTOCOMMIT,
                                    (SQLPOINTER)SQL_AUTOCOMMIT_ON,
                                    SQL_IS_UINTEGER ) == SQL_SUCCESS );
    }
    else
    {
        STL_TRY( SQLSetConnectAttr( *aDbcHandle,
                                    SQL_ATTR_AUTOCOMMIT,
                                    (SQLPOINTER)SQL_AUTOCOMMIT_OFF,
                                    SQL_IS_UINTEGER ) == SQL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_SERVICE_NOT_AVAILABLE )
    {
        (void) ztcdPrintDiagnostic( SQL_HANDLE_DBC,
                                    *aDbcHandle,
                                    aErrorStack );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_SERVICE_NOT_AVAILABLE,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        /*(
        (void) ztcdPrintDiagnostic( SQL_HANDLE_DBC,
                                    *aDbcHandle,
                                    aErrorStack );
        */
        (void) SQLFreeHandle( SQL_HANDLE_DBC,
                              *aDbcHandle );
    }

    return STL_FAILURE;
}

stlStatus ztcdCreateConnection4Syncher( SQLHDBC       * aDbcHandle,
                                        SQLHENV         aEnvHandle,
                                        stlBool         aIsAutoCommit,
                                        stlErrorStack * aErrorStack )
{
    stlInt32   sState      = 0;
    SQLRETURN  sReturn     = SQL_SUCCESS;
    SQLPOINTER sAutoCommit;

    stlChar    sID[ STL_MAX_SQL_IDENTIFIER_LENGTH ] = { 0 };
    stlChar    sPW[ STL_MAX_SQL_IDENTIFIER_LENGTH ] = { 0 };
    stlChar    sConnStr[1024] = {0,};
    stlChar    sSlaveIp[32] = {0,};  
    stlInt32   sPort;  
    
    STL_TRY( ztcmGetSlaveIp( sSlaveIp, aErrorStack ) == STL_SUCCESS );
    STL_TRY( ztcmGetSlaveUserId( sID, aErrorStack ) == STL_SUCCESS );
    STL_TRY( ztcmGetSlaveUserPw( sPW, aErrorStack ) == STL_SUCCESS );
    STL_TRY( ztcmGetSlaveGoldilocksListenPort( &sPort, aErrorStack ) == STL_SUCCESS );

    STL_TRY( SQLAllocHandle( SQL_HANDLE_DBC,
                             aEnvHandle,
                             aDbcHandle ) == SQL_SUCCESS );
    sState = 1;
    
    stlSnprintf( sConnStr,
                 1024,
                 "HOST=%s;UID=%s;PWD=%s;PORT=%d;PROTOCOL=TCP",       //PROTOCOL=DA/TCP
                 sSlaveIp,
                 sID,
                 sPW,
                 sPort );   
//    stlDebug("slave conn:  %s \n", sConnStr);
    sReturn = SQLDriverConnect( *aDbcHandle,
                                NULL,
                                (SQLCHAR *)sConnStr,
                                SQL_NTS,
                                NULL,
                                0,
                                NULL,
                                SQL_DRIVER_NOPROMPT );
//    stlDebug("slave conn: rtn %d \n", sReturn);

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_SERVICE_NOT_AVAILABLE );

    if( aIsAutoCommit == STL_TRUE )
    {
        sAutoCommit = (SQLPOINTER)SQL_AUTOCOMMIT_ON;
    }
    else
    {
        sAutoCommit = (SQLPOINTER)SQL_AUTOCOMMIT_OFF;
    }

    STL_TRY( SQLSetConnectAttr( *aDbcHandle,
                                SQL_ATTR_AUTOCOMMIT,
                                sAutoCommit,
                                SQL_IS_UINTEGER ) == SQL_SUCCESS );


    if( gMasterMgr->mSlavePropagateMode == STL_FALSE )
    {
        STL_TRY( ztcdSetPropagateMode( *aDbcHandle,
                                       aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_SERVICE_NOT_AVAILABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_SERVICE_NOT_AVAILABLE,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) SQLFreeHandle( SQL_HANDLE_DBC,
                              *aDbcHandle );
    }

    return STL_FAILURE;
}


stlStatus ztcdFreeEnvHandle( SQLHENV         aEnvHandle,
                             stlErrorStack * aErrorStack )
{
    /**
     * Stop을 위한 Signal 발생시 Gliese에서 Connection을 종료한 뒤 Cyclone이 Signal을 인식하게 된다.
     * 따라서, Cyclone에서 Stop 을 위한 Finalize 과정에서는 이미 Connection은 끊긴 상태이기 때문에 오류를 리턴하게 된다.
     * 그냥 Void로 처리하고... 나중에는 이것을 풀어줘야 한다.
     * 이 문제는 ODBC 함수를 사용하기 위해서 cyclone이 Gliese에 Attach 하기 때문에 발생하는 문제이다.
     * 추후, CS Module이 완성되면 아래의 내용은 STL_TRY로 바꾸어야 한다.
     * Destroy Connection도 마찬가지임!!
     */

    (void) SQLFreeHandle( SQL_HANDLE_ENV,
                          aEnvHandle );
    return STL_SUCCESS;
}

stlStatus ztcdDestroyConnection( SQLHDBC         aDbcHandle,
                                 stlErrorStack * aErrorStack )
{
//    stlDebug("ztcdDestroyConnection \n");
    (void) SQLEndTran( SQL_HANDLE_DBC,
                       aDbcHandle,
                       SQL_ROLLBACK );
    
    (void) SQLDisconnect( aDbcHandle );
    
    (void) SQLFreeHandle( SQL_HANDLE_DBC,
                          aDbcHandle );
    
    return STL_SUCCESS;
}


void ztcdWriteDiagnostic( SQLSMALLINT     aHandleType,
                          SQLHANDLE       aHandle,
                          stlErrorStack * aErrorStack )
{
    stlChar  sDiagMsg[ZTC_TRACE_BUFFER_SIZE] = {0,};
    
    (void) ztcdGetDiagnosticMsg( aHandleType,
                                 aHandle,
                                 1,
                                 sDiagMsg,
                                 aErrorStack );

    if( stlStrlen( sDiagMsg ) > 0 )
    {
        (void) ztcmLogMsg( aErrorStack,
                           "%s\n",
                           sDiagMsg );
    }
}

stlStatus ztcdSQLAllocHandle( SQLHANDLE       aInputHandle,
                              SQLHANDLE     * aOutputHandlePtr,
                              stlErrorStack * aErrorStack )
{
    SQLRETURN    sReturn;

    STL_DASSERT( gConfigure.mApplyToFile == STL_FALSE );

    sReturn = SQLAllocHandle( SQL_HANDLE_STMT,
                              aInputHandle,
                              aOutputHandlePtr );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_ALLOCSTMT );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_ALLOCSTMT )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_DBC,
                             aInputHandle,
                             aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcdSQLFreeHandle( SQLHANDLE       aHandle,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN    sReturn;
    
    STL_DASSERT( aHandle != NULL );

    sReturn = SQLFreeHandle( SQL_HANDLE_STMT,
                             aHandle );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_FREESTMT );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_FREESTMT )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aHandle,
                             aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcdSQLEndTran( SQLSMALLINT     aHandleType,
                          SQLHANDLE       aHandle,
                          SQLSMALLINT     aCompletionType,
                          stlBool         aIgnoreFailure,
                          stlErrorStack * aErrorStack )
{
    SQLRETURN    sReturn;
    SQLRETURN    sReturnCode;
    SQLRETURN    sReturn2;

    STL_DASSERT( aHandle != NULL );

    sReturn = SQLEndTran( aHandleType,
                          aHandle,
                          aCompletionType );
    if( sReturn != 0 )
    {
        sReturn2 = SQLGetDiagField( aHandleType,
                                    aHandle,
                                    1,
                                    SQL_DIAG_NATIVE,
                                    &sReturnCode,
                                    0,
                                    0 );
        /**
         * TODO lym 수정해야 함
         * 임시로 19038 하드 코딩
         * 특정 경우만 성공처리하는걸로 수정해야 할듯.
         */
        if( SQL_SUCCEEDED(sReturn2) && (sReturnCode == 19038) )
        {
            STL_THROW( RAMP_ERR_ENDTRAN );
        }
    }
//    stlDebug("ODBC %d \n", sReturn);

    if( aIgnoreFailure == STL_FALSE )
    {
        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_ENDTRAN );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_ENDTRAN )
    {
        ztcdWriteDiagnostic( aHandleType,
                             aHandle,
                             aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
    
}

stlStatus ztcdSQLGetStmtAttr( SQLHSTMT        aStmtHandle,
                              SQLINTEGER      aAttribute,
                              SQLPOINTER      aValuePtr,
                              SQLINTEGER      aBufferLength,
                              SQLINTEGER    * aStringLength,
                              stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;

    sReturn = SQLGetStmtAttr( aStmtHandle,
                              aAttribute,
                              aValuePtr,
                              aBufferLength,
                              aStringLength );

    if( aAttribute < SQL_DRIVER_STMT_ATTR_BASE )
    {
        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_GETSTMTATTR );
    }
    else
    {
        /*
         * Vendor specific attribute 라면 에러를 무시한다.
         */
        if( !SQL_SUCCEEDED( sReturn ) )
        {
            *aStringLength = 0;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETSTMTATTR )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcdSQLGetDescField( SQLHDESC        aDescriptorHandle,
                               SQLSMALLINT     aRecNumber,
                               SQLSMALLINT     aFieldIdentifier,
                               SQLPOINTER      aValuePtr,
                               SQLINTEGER      aBufferLength,
                               SQLINTEGER    * aStringLengthPtr,
                               stlErrorStack * aErrorStack )
{
    SQLRETURN   sReturn;
    SQLINTEGER  sStringLength = 0;

    sReturn = SQLGetDescField( aDescriptorHandle,
                               aRecNumber,
                               aFieldIdentifier,
                               aValuePtr,
                               aBufferLength,
                               &sStringLength );

    if( aFieldIdentifier < SQL_DRIVER_DESC_FIELD_BASE )
    {
        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_GETDESCFIELD );

        if( aStringLengthPtr != NULL )
        {
            *aStringLengthPtr = sStringLength;
        }
    }
    else
    {
        /*
         * Vendor specific attribute 라면 에러를 무시한다.
         */
        if( !SQL_SUCCEEDED( sReturn ) )
        {
            *aStringLengthPtr = 0;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETDESCFIELD )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_DESC,
                             aDescriptorHandle,
                             aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcdSQLPrepare( SQLHSTMT        aStmtHandle,
                          SQLCHAR       * aStmtText,
                          SQLINTEGER      aTextLength,
                          stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;
    
    STL_DASSERT( aStmtHandle != NULL );

    sReturn = SQLPrepare( aStmtHandle,
                          aStmtText,
                          aTextLength );
//    stlDebug("ODBC %d \n", sReturn);

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_PREPARE );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_PREPARE )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcdSQLBindCol( SQLHSTMT        aStmtHandle,
                          SQLUSMALLINT    aColumnNumber,
                          SQLSMALLINT     aTargetType,
                          SQLPOINTER      aTargetValuePtr,
                          SQLLEN          aBufferLength,
                          SQLLEN        * aIndicator,
                          stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;
    
    STL_DASSERT( aStmtHandle != NULL );

    sReturn = SQLBindCol( aStmtHandle,
                          aColumnNumber,
                          aTargetType,
                          aTargetValuePtr,
                          aBufferLength,
                          aIndicator );
//    stlDebug("ODBC %d \n", sReturn);

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_BINDCOL );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_BINDCOL )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcdSQLBindParameter( SQLHSTMT        aStmtHandle,
                                SQLUSMALLINT    aParameterNumber,
                                SQLSMALLINT     aInputOutputType,
                                SQLSMALLINT     aValueType,
                                SQLSMALLINT     aParameterType,
                                SQLSMALLINT     aCharLengthUnit,
                                SQLULEN         aColumnSize,
                                SQLSMALLINT     aDecimalDigits,
                                SQLPOINTER      aParameterValuePtr,
                                SQLLEN          aBufferLength,
                                SQLLEN        * aIndicator,
                                stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;
    SQLHDESC   sIpd;
    SQLPOINTER sDataPtr = NULL;

    STL_DASSERT( aStmtHandle != NULL );

    sReturn = SQLGetStmtAttr( aStmtHandle,
                              SQL_ATTR_IMP_PARAM_DESC,
                              &sIpd,
                              SQL_IS_POINTER,
                              NULL );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_GETSTMTATTR );

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
//    stlDebug("ODBC %d \n", sReturn);

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_BINDPARAMETER );
    
    switch( aParameterType )
    {
        case SQL_CHAR:
        case SQL_VARCHAR :
            STL_DASSERT( (aCharLengthUnit == SQL_CLU_OCTETS) || (aCharLengthUnit == SQL_CLU_CHARACTERS) );

            sReturn = SQLSetDescField( sIpd,
                                       aParameterNumber,
                                       SQL_DESC_CHAR_LENGTH_UNITS,
                                       (SQLPOINTER)(stlVarInt) aCharLengthUnit,
                                       0 );

            STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_SETDESCFIELD );

            sDataPtr = NULL;
            sReturn = SQLSetDescField( sIpd,
                                       aParameterNumber,
                                       SQL_DESC_DATA_PTR,
                                       sDataPtr,
                                       0 );

            STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_SETDESCFIELD );
            break;
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_GETSTMTATTR )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_CATCH( RAMP_ERR_BINDPARAMETER )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_CATCH( RAMP_ERR_SETDESCFIELD )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_DESC,
                             sIpd,
                             aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcdSQLExecute( SQLHSTMT        aStmtHandle,
                          SQLRETURN     * aReturn,
                          stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;
    
    STL_DASSERT( aStmtHandle != NULL );

    sReturn = SQLExecute( aStmtHandle );
//    stlDebug("ODBC %d \n", sReturn);

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ) || (sReturn == SQL_NO_DATA),
                   RAMP_ERR_EXECUTE );
    
    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXECUTE )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcdSQLExecDirect( SQLHSTMT        aStmtHandle,
                             SQLCHAR       * aStmtText,
                             SQLINTEGER      aTextLength,
                             SQLRETURN     * aReturn,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;
    
    STL_DASSERT( aStmtHandle != NULL );

    sReturn = SQLExecDirect( aStmtHandle,
                             aStmtText,
                             aTextLength );
//    stlDebug("ODBC %d \n", sReturn);

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ) || (sReturn == SQL_NO_DATA),
                   RAMP_ERR_EXECDIRECT );

    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXECDIRECT )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcdSQLFetch( SQLHSTMT        aStmtHandle,
                        SQLRETURN     * aReturn,
                        stlErrorStack * aErrorStack )
{
    SQLRETURN sReturn;
    
    STL_DASSERT( aStmtHandle != NULL );

    sReturn = SQLFetch( aStmtHandle );
//    stlDebug("ODBC %d \n", sReturn);

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ) || (sReturn == SQL_NO_DATA),
                   RAMP_ERR_FETCH );

    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FETCH )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcdSQLGetData( SQLHSTMT        aStmtHandle,
                          SQLUSMALLINT    aCol_or_Param_Num,
                          SQLSMALLINT     aTargetType,
                          SQLPOINTER      aTargetValuePtr,
                          SQLLEN          aBufferLength,
                          SQLLEN        * aStrLen_or_IndPtr,  
                          SQLRETURN     * aReturn,
                          stlErrorStack * aErrorStack )
{
    SQLRETURN sReturn;

    STL_DASSERT( aStmtHandle != NULL );

    sReturn = SQLGetData( aStmtHandle,
                          aCol_or_Param_Num,
                          aTargetType,
                          aTargetValuePtr,
                          aBufferLength,
                          aStrLen_or_IndPtr );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ) || (sReturn == SQL_NO_DATA),
                   RAMP_ERR_GET_DATA );

    *aReturn = sReturn;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GET_DATA )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcdSQLCloseCursor( SQLHSTMT        aStmtHandle,
                              SQLRETURN     * aReturn,
                              stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;
    
    STL_DASSERT( aStmtHandle != NULL );

    sReturn = SQLCloseCursor( aStmtHandle );
    
    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ),
                   RAMP_ERR_CLOSE_CURSOR );
    
    *aReturn = sReturn;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CLOSE_CURSOR )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;    
}


stlStatus ztcdSQLFreeStmt( SQLHSTMT        aStmtHandle,
                           SQLUSMALLINT    aOption,
                           SQLRETURN     * aReturn,
                           stlErrorStack * aErrorStack )
{
    SQLRETURN  sReturn;

    STL_DASSERT( aStmtHandle != NULL );

    sReturn = SQLFreeStmt( aStmtHandle,
                           aOption );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ),
                   RAMP_ERR_FREE_STMT );

    * aReturn = sReturn;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FREE_STMT )
    {
        ztcdWriteDiagnostic( SQL_HANDLE_STMT,
                             aStmtHandle,
                             aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcdBuildLogFileInfo( SQLHDBC         aDbcHandle,
                                stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt16    sGroupId;
    stlChar     sName[STL_MAX_FILE_PATH_LENGTH];
    stlInt64    sFileSize;
    stlInt32    sState = 0;
    
    SQLHSTMT    sStmtHandle;
    SQLLEN      sIndicator[3];
    SQLRETURN   sResult;
    
    /**
     * Todo : Order By가 구현되면 GROUP_ID 로 정렬해야 함!!
     * Parallel Logging도 생각해야 함!!
     */
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT MEM.GROUP_ID, MEM.NAME, GRP.FILE_SIZE "
                 "FROM FIXED_TABLE_SCHEMA.X$LOG_MEMBER MEM, FIXED_TABLE_SCHEMA.X$LOG_GROUP GRP "
                 "WHERE MEM.GROUP_ID=GRP.GROUP_ID ORDER BY GROUP_ORD_ID ASC" );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SHORT, 
                             &sGroupId,
                             STL_SIZEOF( sGroupId ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_CHAR,
                             sName,
                             STL_SIZEOF( sName ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             3,
                             SQL_C_SBIGINT, 
                             &sFileSize,
                             STL_SIZEOF( sFileSize ),
                             &sIndicator[2],
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult,
                             aErrorStack ) == STL_SUCCESS );
    
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );

        stlMemset( sName, 0x00, STL_SIZEOF( sName ) );
        
        STL_TRY( ztcdSQLFetch( sStmtHandle, 
                               &sResult, 
                               aErrorStack ) == STL_SUCCESS );
        
        if( sResult == SQL_NO_DATA )
        {
            break;
        }
        
        STL_TRY( ztcmAddLogFileInfo( sName, 
                                     sGroupId,
                                     sFileSize,
                                     aErrorStack ) == STL_SUCCESS );   
    }
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;   
}



stlStatus ztcdGetTableIDFromSlaveMeta( SQLHDBC         aDbcHandle,
                                       stlChar       * aSchemaName,
                                       stlChar       * aTableName,
                                       stlInt32      * aTableId,
                                       stlBool       * aFound,
                                       stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32    sTableId;
    stlInt32    sState = 0;
    
    SQLHSTMT    sStmtHandle;
    SQLLEN      sIndicator;
    SQLRETURN   sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT TABLE_ID "
                 "FROM CYCLONEM_SLAVE_META "
                 "WHERE SLAVE_SCHEMA_NAME='%s' AND SLAVE_TABLE_NAME='%s' AND GROUP_NAME='%s'",
                 aSchemaName,
                 aTableName,
                 gConfigure.mCurrentGroupPtr->mName );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SLONG, 
                             &sTableId,
                             STL_SIZEOF( sTableId ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    if( sResult == SQL_NO_DATA )
    {
        *aFound = STL_FALSE;
    }
    else
    {
        *aTableId = sTableId;
        *aFound   = STL_TRUE;
    }
        
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;
}

stlStatus ztcdGetTableIDFromDB( SQLHDBC         aDbcHandle,
                                stlInt64        aSchemaId,
                                stlChar       * aTableName,
                                stlInt64      * aTableId,
                                stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt64      sTableId;
    stlInt32      sState = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator;
    SQLRETURN     sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT TABLE_ID "
                 "FROM DEFINITION_SCHEMA.TABLES "
                 "WHERE TABLE_NAME = '%s' AND SCHEMA_ID = %d",
                 aTableName,
                 aSchemaId );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT,
                             &sTableId,
                             STL_SIZEOF( sTableId ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INVALID_TABLE_NAME );
        
    *aTableId = sTableId;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_TABLE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_TABLE,
                      NULL,
                      aErrorStack,
                      aTableName );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;
}


stlStatus ztcdSetTransTableSize( SQLHDBC         aDbcHandle,
                                 stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt64      sSize;
    stlInt32      sState = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator;
    SQLRETURN     sResult;

    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT VALUE "
                 "FROM X$PROPERTY "
                 "WHERE PROPERTY_NAME='TRANSACTION_TABLE_SIZE'" );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT,
                             &sSize,
                             STL_SIZEOF( sSize ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INTERNAL_ERROR );   
    
    gConfigure.mTransTableSize = sSize;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INTERNAL_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Get TransTableSize" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;
}


stlStatus ztcdGetSchemaIDFromDB( SQLHDBC         aDbcHandle,
                                 stlChar       * aSchemaName,
                                 stlInt64      * aSchemaId,
                                 stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt64      sSchemaId;
    stlInt32      sState = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator;
    SQLRETURN     sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT SCHEMA_ID "
                 "FROM DEFINITION_SCHEMA.SCHEMATA "
                 "WHERE SCHEMA_NAME = '%s'",
                 aSchemaName );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT, 
                             &sSchemaId,
                             STL_SIZEOF( sSchemaId ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INVALID_SCHEMA_NAME );
        
    *aSchemaId = sSchemaId;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_SCHEMA_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_SCHEMA,
                      NULL,
                      aErrorStack,
                      aSchemaName );
    }
    STL_FINISH;

    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }
    
    return STL_FAILURE;
}


stlStatus ztcdGetColumnId( SQLHDBC         aDbcHandle,
                           stlInt64        aSchemaId,
                           stlInt64        aTableId,
                           stlChar       * aColumnName,
                           stlInt64      * aColumnId,
                           stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32      sState    = 0;
    stlInt64      sColumnId = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator;
    SQLRETURN     sResult;

    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "    COLUMN_ID "
                 "FROM DEFINITION_SCHEMA.COLUMNS "
                 "WHERE IS_UNUSED = FALSE "
                 "  AND SCHEMA_ID = %ld AND TABLE_ID = %ld AND COLUMN_NAME = '%s'",
                 aSchemaId, aTableId, aColumnName );

    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT,
                             &sColumnId,
                             STL_SIZEOF( sColumnId ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INVALID_COLUMN );

    *aColumnId = sColumnId;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_COLUMN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Get ColumnId" );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) ztcdSQLFreeHandle( sStmtHandle,
                                      aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus ztcdGetColumnTypeId( SQLHDBC         aDbcHandle,
                               stlInt64        aTableId, 
                               stlInt64        aColumnId,
                               stlInt64      * aDataTypeId,
                               stlInt64      * aIntervalTypeId,
                               stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt64      sDataTypeId     = 0;
    stlInt64      sIntervalTypeId = 0;
    stlInt32      sState = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator[2];
    SQLRETURN     sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT DATA_TYPE_ID, INTERVAL_TYPE_ID "
                 "FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR "
                 "WHERE TABLE_ID = %ld AND COLUMN_ID = %ld",
                 aTableId,
                 aColumnId );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT, 
                             &sDataTypeId,
                             STL_SIZEOF( sDataTypeId ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_SBIGINT, 
                             &sIntervalTypeId,
                             STL_SIZEOF( sIntervalTypeId ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INVALID_COLUMN );
    
    *aDataTypeId     = sDataTypeId;
    *aIntervalTypeId = sIntervalTypeId;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_COLUMN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Get ColumnTypeId" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;

}


stlStatus ztcdGetPrimaryConstraintId( SQLHDBC         aDbcHandle,
                                      stlInt64        aSchemaId, 
                                      stlInt64        aTableId,
                                      stlInt64      * aConstraintId,
                                      stlBool       * aFound,
                                      stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlBool       sFound         = STL_FALSE;    
    stlInt64      sConstraintId  = 0;
    stlChar       sType[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt32      sState = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator[2];
    SQLRETURN     sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT /*+ INDEX( TABLE_CONSTRAINTS, TABLE_CONSTRAINTS_PRIMARY_KEY_INDEX ) */ "
                 "CONSTRAINT_TYPE, CONSTRAINT_ID "
                 "FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS "
                 "WHERE TABLE_SCHEMA_ID = %ld AND TABLE_ID = %ld",
                 aSchemaId, aTableId );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_CHAR, 
                             sType,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_SBIGINT, 
                             &sConstraintId,
                             STL_SIZEOF( sConstraintId ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );

        STL_TRY( ztcdSQLFetch( sStmtHandle, 
                               &sResult, 
                               aErrorStack ) == STL_SUCCESS );
        
        if( sResult == SQL_NO_DATA )
        {
            break;
        }
        
        if( stlStrncmp( sType, "PRIMARY", 7 ) == 0 )
        {
            sFound         = STL_TRUE;
            *aConstraintId = sConstraintId;
            break;
        }
        
    }
    
    *aFound = sFound;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;    
    
    
}


stlStatus ztcdGetColumnUniqueConstraint( SQLHDBC         aDbcHandle,
                                         stlInt64        aSchemaId, 
                                         stlInt64        aTableId,
                                         stlInt64        aColumnId,
                                         stlBool       * aIsUnique,
                                         stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlChar       sType[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt64      sConstraintId;
    stlBool       sIsUnique = STL_FALSE;
    stlInt32      sState = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator[2];
    SQLRETURN     sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT /*+ INDEX( TABLE_CONSTRAINTS, TABLE_CONSTRAINTS_PRIMARY_KEY_INDEX ) */ "
                 "CONSTRAINT_TYPE, CONSTRAINT_ID "
                 "FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS "
                 "WHERE TABLE_SCHEMA_ID = %ld AND TABLE_ID = %ld",
                 aSchemaId, aTableId );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_CHAR, 
                             sType,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_SBIGINT, 
                             &sConstraintId,
                             STL_SIZEOF( sConstraintId ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );

        STL_TRY( ztcdSQLFetch( sStmtHandle, 
                               &sResult, 
                               aErrorStack ) == STL_SUCCESS );
        
        if( sResult == SQL_NO_DATA )
        {
            break;
        }
        
        if( stlStrncmp( sType, "UNIQUE", 6 ) == 0 )
        {
            STL_TRY( ztcdGetColumnConstraint( aDbcHandle,
                                              aSchemaId,
                                              aTableId,
                                              aColumnId,
                                              sConstraintId,
                                              &sIsUnique,
                                              aErrorStack ) == STL_SUCCESS );
            
            if( sIsUnique == STL_TRUE )
            {
                break;
            }
        }
    }
    
    *aIsUnique = sIsUnique;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;    
}


stlStatus ztcdGetColumnConstraint( SQLHDBC         aDbcHandle,
                                   stlInt64        aSchemaId, 
                                   stlInt64        aTableId,
                                   stlInt64        aColumnId,
                                   stlInt64        aConstraintId,
                                   stlBool       * aIskey,
                                   stlErrorStack * aErrorStack )
{
    stlChar      sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlBool      sFound     = STL_FALSE;   
    stlInt64     sColumnId  = 0;
    stlInt32     sState     = 0;
    
    SQLHSTMT     sStmtHandle;
    SQLLEN       sIndicator;
    SQLRETURN    sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT COLUMN_ID "
                 "FROM DEFINITION_SCHEMA.KEY_COLUMN_USAGE "
                 "WHERE CONSTRAINT_ID = %ld AND COLUMN_ID = %ld",
                 aConstraintId,
                 aColumnId );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT, 
                             &sColumnId,
                             STL_SIZEOF( sColumnId ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
        
    if( sResult != SQL_NO_DATA )
    {
        sFound = STL_TRUE;
    }
    
    *aIskey = sFound;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );   
        
    return STL_SUCCESS;

    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;    
}


stlStatus ztcdGetColumnParamInfo( SQLHDBC          aDbcHandle,
                                  stlInt64         aTableId,
                                  dtlFuncVector  * aDTVectorFunc,
                                  ztcColumnInfo  * aColumnInfo,
                                  stlErrorStack  * aErrorStack )
{
    stlInt64         sAllocSize      = 0;
    stlInt32         sAllocStrSize   = 0;
    stlInt8          sDBMbLength     = 1;
    stlInt8          sMbLength       = 1;
    stlInt16         sCharLengthUnit = SQL_CLU_NONE;

    STL_TRY( ztcdGetMbMaxLength( aDbcHandle,
                                 &sDBMbLength,
                                 aErrorStack ) == STL_SUCCESS );
    
    switch( aColumnInfo->mDataTypeId )
    {
        case DTL_TYPE_BOOLEAN:
            {
                sAllocSize    = DTL_BOOLEAN_SIZE;
                sAllocStrSize = 0;
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
            {
                sAllocSize    = DTL_NATIVE_SMALLINT_SIZE;
                sAllocStrSize = 0;
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:
            {
                sAllocSize    = DTL_NATIVE_INTEGER_SIZE;
                sAllocStrSize = 0;
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:
            {
                sAllocSize    = DTL_NATIVE_BIGINT_SIZE;
                sAllocStrSize = 0;
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
            {
                sAllocSize    = DTL_NATIVE_REAL_SIZE;
                sAllocStrSize = 0;
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                sAllocSize    = DTL_NATIVE_DOUBLE_SIZE;
                sAllocStrSize = 0;
                break;
            }
        case DTL_TYPE_CHAR:
        case DTL_TYPE_VARCHAR:
            {
                STL_TRY( ztcdGetCharacterLengthUnit( aDbcHandle,
                                                     aTableId,
                                                     aColumnInfo->mColumnId,
                                                     &sCharLengthUnit,
                                                     aErrorStack ) == STL_SUCCESS );

                STL_TRY( ztcdGetCharacterMaxLength( aDbcHandle,
                                                    aTableId,
                                                    aColumnInfo->mColumnId,
                                                    &sAllocSize,
                                                    aErrorStack ) == STL_SUCCESS );
                sMbLength     = sDBMbLength;
                sAllocStrSize = 0;
                break;
            }
        case DTL_TYPE_BINARY:
        case DTL_TYPE_VARBINARY:
            {
                STL_TRY( ztcdGetCharacterOctetLength( aDbcHandle,
                                                      aTableId,
                                                      aColumnInfo->mColumnId,
                                                      &sAllocSize,
                                                      aErrorStack ) == STL_SUCCESS );
                sAllocStrSize = 0;
                break;
                
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                STL_TRY( dtlGetDataValueBufferSize( aColumnInfo->mDataTypeId,
                                                    gDefaultDataTypeDef[aColumnInfo->mDataTypeId].mArgNum1,
                                                    gDefaultDataTypeDef[aColumnInfo->mDataTypeId].mStringLengthUnit,
                                                    &sAllocSize,
                                                    aDTVectorFunc,
                                                    NULL,
                                                    aErrorStack ) == STL_SUCCESS );
                
                sAllocStrSize = gDefaultDataTypeDef[aColumnInfo->mDataTypeId].mStringBufferSize;
                break;
            }
        case DTL_TYPE_DATE:
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
        case DTL_TYPE_FLOAT:
        case DTL_TYPE_NUMBER:
            {
                STL_TRY( dtlGetDataValueBufferSize( aColumnInfo->mDataTypeId,
                                                    gDefaultDataTypeDef[aColumnInfo->mDataTypeId].mArgNum1,
                                                    0,
                                                    &sAllocSize,
                                                    aDTVectorFunc,
                                                    NULL,
                                                    aErrorStack ) == STL_SUCCESS );
                sAllocStrSize = 0;
                break;
            }
        case DTL_TYPE_LONGVARCHAR:
        case DTL_TYPE_LONGVARBINARY:
            {
                /**
                 * Long Variable Type의 경우에는 Value에 대한 정보는 SQL_LONG_VARIABLE_LENGTH_STRUCT로 저장 된다.
                 */
                sAllocSize    = STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT );
                sAllocStrSize = 0;
                break;
            }
        case DTL_TYPE_DECIMAL:
        case DTL_TYPE_BLOB:
        case DTL_TYPE_CLOB:
            {
                /** Not Supported Yet.*/
                STL_THROW( RAMP_ERR_INVALID_DATATYPE );
                break;
            }
        default:
            break;   
    }
    
    /**
     * Execute 에러 발생시 Primary Key를 Log 파일에 출력하기 위한 String Buffer를 위한 공간
     */
    if( aColumnInfo->mDataTypeId != DTL_TYPE_LONGVARCHAR ||
        aColumnInfo->mDataTypeId != DTL_TYPE_LONGVARBINARY )
    {
        sAllocStrSize = gDefaultDataTypeDef[aColumnInfo->mDataTypeId].mStringBufferSize;
    }
        
    /**
     * Set Parameter
     */
    switch( aColumnInfo->mDataTypeId )
    {
        case DTL_TYPE_BOOLEAN:
            {
                aColumnInfo->mValueType        = SQL_C_BOOLEAN;
                aColumnInfo->mParameterType    = SQL_BOOLEAN;
                aColumnInfo->mColumnSize       = 0;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = 0;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
            {
                aColumnInfo->mValueType        = SQL_C_SSHORT;
                aColumnInfo->mParameterType    = SQL_SMALLINT;
                aColumnInfo->mColumnSize       = 0;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = 0;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:
            {
                aColumnInfo->mValueType        = SQL_C_SLONG;
                aColumnInfo->mParameterType    = SQL_INTEGER;
                aColumnInfo->mColumnSize       = 0;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = 0;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:
            {
                aColumnInfo->mValueType        = SQL_C_SBIGINT;
                aColumnInfo->mParameterType    = SQL_BIGINT;
                aColumnInfo->mColumnSize       = 0;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = 0;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
            {
                aColumnInfo->mValueType        = SQL_C_FLOAT;
                aColumnInfo->mParameterType    = SQL_REAL;
                aColumnInfo->mColumnSize       = 0;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = 0;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                aColumnInfo->mValueType        = SQL_C_DOUBLE;
                aColumnInfo->mParameterType    = SQL_DOUBLE;
                aColumnInfo->mColumnSize       = 0;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = 0;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_CHAR:
            {
                aColumnInfo->mValueType        = SQL_C_CHAR;
                aColumnInfo->mParameterType    = SQL_CHAR;
                aColumnInfo->mColumnSize       = sAllocSize;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = sAllocSize * sMbLength;
                aColumnInfo->mStringLengthUnit = sCharLengthUnit;
                break;
            }
        case DTL_TYPE_VARCHAR:
            {
                aColumnInfo->mValueType        = SQL_C_CHAR;
                aColumnInfo->mParameterType    = SQL_VARCHAR;
                aColumnInfo->mColumnSize       = sAllocSize;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = sAllocSize * sMbLength;
                aColumnInfo->mStringLengthUnit = sCharLengthUnit;
                break;
            }
        case DTL_TYPE_BINARY:
            {
                aColumnInfo->mValueType        = SQL_C_BINARY;
                aColumnInfo->mParameterType    = SQL_BINARY;
                aColumnInfo->mColumnSize       = sAllocSize;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = sAllocSize;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_VARBINARY:
            {
                aColumnInfo->mValueType        = SQL_C_BINARY;
                aColumnInfo->mParameterType    = SQL_VARBINARY;
                aColumnInfo->mColumnSize       = sAllocSize;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = sAllocSize;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_DATE:
            {
                aColumnInfo->mValueType        = SQL_C_BINARY;
                aColumnInfo->mParameterType    = SQL_TYPE_DATE;
                aColumnInfo->mColumnSize       = sAllocSize;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = sAllocSize;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_TIME:
            {
                aColumnInfo->mValueType        = SQL_C_BINARY;
                aColumnInfo->mParameterType    = SQL_TYPE_TIME;
                aColumnInfo->mColumnSize       = sAllocSize;
                aColumnInfo->mDecimalDigits    = 6;
                aColumnInfo->mBufferLength     = sAllocSize;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_TIMESTAMP:
            {
                aColumnInfo->mValueType        = SQL_C_BINARY;
                aColumnInfo->mParameterType    = SQL_TYPE_TIMESTAMP;
                aColumnInfo->mColumnSize       = sAllocSize;
                aColumnInfo->mDecimalDigits    = 6;
                aColumnInfo->mBufferLength     = sAllocSize;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            {
                aColumnInfo->mValueType        = SQL_C_BINARY;
                aColumnInfo->mParameterType    = SQL_TYPE_TIME_WITH_TIMEZONE;
                aColumnInfo->mColumnSize       = sAllocSize;
                aColumnInfo->mDecimalDigits    = 6;
                aColumnInfo->mBufferLength     = sAllocSize;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                aColumnInfo->mValueType        = SQL_C_BINARY;
                aColumnInfo->mParameterType    = SQL_TYPE_TIMESTAMP_WITH_TIMEZONE;
                aColumnInfo->mColumnSize       = sAllocSize;
                aColumnInfo->mDecimalDigits    = 6;
                aColumnInfo->mBufferLength     = sAllocSize;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                aColumnInfo->mValueType        = SQL_C_CHAR;
                aColumnInfo->mParameterType    = SQL_VARCHAR;
                aColumnInfo->mColumnSize       = sAllocStrSize;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = sAllocStrSize;
                aColumnInfo->mStringLengthUnit = SQL_CLU_OCTETS;
                break;
            }
        case DTL_TYPE_FLOAT:
            {
                aColumnInfo->mValueType        = SQL_C_BINARY;
                aColumnInfo->mParameterType    = SQL_FLOAT;
                aColumnInfo->mColumnSize       = aColumnInfo->mNumericPrecision;
                aColumnInfo->mDecimalDigits    = aColumnInfo->mNumericScale;
                aColumnInfo->mBufferLength     = sAllocSize;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_NUMBER:
            {
                aColumnInfo->mValueType        = SQL_C_BINARY;
                aColumnInfo->mParameterType    = SQL_NUMERIC;
                aColumnInfo->mColumnSize       = aColumnInfo->mNumericPrecision;
                aColumnInfo->mDecimalDigits    = aColumnInfo->mNumericScale;
                aColumnInfo->mBufferLength     = sAllocSize;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_LONGVARCHAR:
            {
                aColumnInfo->mValueType        = SQL_C_LONGVARCHAR;
                aColumnInfo->mParameterType    = SQL_LONGVARCHAR;
                aColumnInfo->mColumnSize       = 0;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = 0;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_LONGVARBINARY:
            {
                aColumnInfo->mValueType        = SQL_C_LONGVARBINARY;
                aColumnInfo->mParameterType    = SQL_LONGVARBINARY;
                aColumnInfo->mColumnSize       = 0;
                aColumnInfo->mDecimalDigits    = 0;
                aColumnInfo->mBufferLength     = 0;
                aColumnInfo->mStringLengthUnit = SQL_CLU_NONE;
                break;
            }
        case DTL_TYPE_DECIMAL:
        case DTL_TYPE_BLOB:
        case DTL_TYPE_CLOB:
            {
                /** Not Supported Yet.*/
                STL_THROW( RAMP_ERR_INVALID_DATATYPE );
                break;
            }
        default:
            break;
    }
        
    aColumnInfo->mValueMaxSize     = sAllocSize * sMbLength;
    aColumnInfo->mStrValueMaxSize  = sAllocStrSize;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_DATA_TYPE,
                      NULL,
                      aErrorStack,
                      aColumnInfo->mDataTypeId );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcdGetSlaveColumnInfo( SQLHDBC             aDbcHandle,
                                  ztcSlaveTableInfo * aTableInfo,
                                  stlErrorStack     * aErrorStack )
{
    stlChar         sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    
    stlInt64        sTableId;
    stlInt64        sSchemaId;
    stlInt64        sColumnId;
    stlInt64        sIntervalTypeId;
    stlInt64        sPrimaryConstraintId;
    stlChar         sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar         sIsNullableStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlBool         sFound;
    stlInt32        sState = 0;
    
    ztcColumnInfo * sColumnInfo = NULL;
    
    SQLHSTMT        sStmtHandle;
    SQLLEN          sIndicator[3];
    SQLRETURN       sResult;
    
    STL_TRY( ztcdGetSchemaIDFromDB( aDbcHandle,
                                    aTableInfo->mSlaveSchema,
                                    &sSchemaId,
                                    aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdGetTableIDFromDB( aDbcHandle,
                                   sSchemaId,
                                   aTableInfo->mSlaveTable,
                                   &sTableId,
                                   aErrorStack ) == STL_SUCCESS );
    
    /**
     * Primary Key IndexName 얻기
     */
    STL_TRY( ztcdGetPrimaryKeyIndexName( aDbcHandle,
                                         sSchemaId,
                                         sTableId,
                                         aTableInfo->mPKIndexName,
                                         aErrorStack ) == STL_SUCCESS );
    
    /**
     * Primary Key Constraint ID 얻기
     */
    STL_TRY( ztcdGetPrimaryConstraintId( aDbcHandle,
                                         sSchemaId, 
                                         sTableId,
                                         &sPrimaryConstraintId,
                                         &sFound,
                                         aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INVALID_PRIMARY_KEY );
    
    /**
     * DB에서 관리되는 TableID
     */
    aTableInfo->mTableIdAtDB = sTableId;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "    COLUMN_NAME, IS_NULLABLE, COLUMN_ID "
                 "FROM DEFINITION_SCHEMA.COLUMNS "
                 "WHERE IS_UNUSED = FALSE "
                 "  AND SCHEMA_ID = %ld AND TABLE_ID = %ld",
                 sSchemaId, sTableId );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_CHAR,
                             sColumnName,
                             STL_SIZEOF( sColumnName ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_CHAR,
                             sIsNullableStr,
                             STL_SIZEOF( sIsNullableStr ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             3,
                             SQL_C_SBIGINT, 
                             &sColumnId,
                             STL_SIZEOF( sColumnId ),
                             &sIndicator[2],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );

        STL_TRY( ztcdSQLFetch( sStmtHandle, 
                               &sResult, 
                               aErrorStack ) == STL_SUCCESS );
        
        if( sResult == SQL_NO_DATA )
        {
            break;
        }
        
        STL_TRY( stlAllocRegionMem( &gSlaveMgr->mRegionMem,
                                    STL_SIZEOF( ztcColumnInfo ),
                                    (void**)&sColumnInfo,
                                    aErrorStack ) == STL_SUCCESS );    
        /**
         * Initialize Member
         */
        sColumnInfo->mValueMaxSize     = 0;
        sColumnInfo->mStrValueMaxSize  = 0;
        sColumnInfo->mNumericPrecision = 0;
        sColumnInfo->mNumericScale     = 0;
        
        stlStrncpy( sColumnInfo->mColumnName, sColumnName, stlStrlen( sColumnName ) + 1 );
        sColumnInfo->mColumnId = sColumnId;
        
        STL_TRY( ztcdGetColumnConstraint( aDbcHandle,
                                          sSchemaId, 
                                          sTableId,
                                          sColumnId,
                                          sPrimaryConstraintId,
                                          &sColumnInfo->mIsPrimary,
                                          aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcdGetColumnUniqueConstraint( aDbcHandle,
                                                sSchemaId,
                                                sTableId,
                                                sColumnId,
                                                &sColumnInfo->mIsUnique,
                                                aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcdGetColumnDataTypeStr( aDbcHandle,
                                           sTableId,
                                           sColumnId,
                                           sColumnInfo->mDataTypeStr,
                                           aErrorStack ) == STL_SUCCESS );
        
        if( stlStrcmp( sIsNullableStr, "TRUE" ) == 0 )
        {
            sColumnInfo->mIsNullable = STL_TRUE;   
        }
        else
        {
            sColumnInfo->mIsNullable = STL_FALSE;
        }
        
        STL_RING_ADD_LAST( &aTableInfo->mColumnList, 
                           sColumnInfo );
        
        aTableInfo->mColumnCount++;
    }

    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    sFound = STL_FALSE;
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnInfo )
    {
        STL_TRY( ztcdGetColumnTypeId( aDbcHandle,
                                      sTableId, 
                                      sColumnInfo->mColumnId,
                                      &sColumnInfo->mDataTypeId,
                                      &sIntervalTypeId,
                                      aErrorStack ) == STL_SUCCESS );
        
        if( ( sColumnInfo->mDataTypeId == DTL_TYPE_FLOAT ) ||
            ( sColumnInfo->mDataTypeId == DTL_TYPE_NUMBER ) )
        {
            STL_TRY( ztcdGetNumericInfo( aDbcHandle,
                                         sTableId,
                                         sColumnInfo->mColumnId,
                                         &sColumnInfo->mNumericPrecision,
                                         &sColumnInfo->mNumericScale,
                                         aErrorStack ) == STL_SUCCESS );
        }

        if( ( sColumnInfo->mDataTypeId == DTL_TYPE_INTERVAL_YEAR_TO_MONTH ) ||
            ( sColumnInfo->mDataTypeId == DTL_TYPE_INTERVAL_DAY_TO_SECOND )  )
        {
            sFound = STL_TRUE;
        }
    }

    if( sFound == STL_TRUE )
    {
        /**
         * INTERVAL Type Column이 있을 경우에는 LeadingPrecision, SecondPrecision을 세팅한다.
         */
        STL_TRY( ztcdGetIntervalTypeInfo( aDbcHandle,
                                          aTableInfo->mSlaveSchema,
                                          aTableInfo->mSlaveTable,
                                          &aTableInfo->mColumnList,
                                          aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_PRIMARY_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_PRIMARY_KEY,
                      NULL,
                      aErrorStack,
                      aTableInfo->mSlaveSchema,
                      aTableInfo->mSlaveTable );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;
}

stlStatus ztcdGetIntervalTypeInfo( SQLHDBC             aDbcHandle,
                                   stlChar           * aSchemaName,
                                   stlChar           * aTableName,
                                   stlRingHead       * aColumnList,
                                   stlErrorStack     * aErrorStack )
{
    stlChar    sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32   sState = 0;
    stlInt32   sIdx   = 0;

    SQLHSTMT        sStmtHandle;
    SQLHDESC        sDescIrd;
    SQLSMALLINT     sPrecision;
    SQLINTEGER      sDatetimeIntervalPrecision;
    ztcColumnInfo * sColumnInfo = NULL;

    /**
     * 해당 Table Name 으로 결과가 없는 SELECT 구문을 수행
     */
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT * FROM \"%s\".\"%s\" WHERE 1 = 0",
                 aSchemaName,
                 aTableName );

    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );

    /**
     * Target 의 IRD(Implementaion Row Descriptor) 정보를 획득한다.
     */
    STL_TRY( ztcdSQLGetStmtAttr( sStmtHandle,
                                 SQL_ATTR_IMP_ROW_DESC,
                                 &sDescIrd,
                                 SQL_IS_POINTER,
                                 NULL,
                                 aErrorStack ) == STL_SUCCESS );

    sIdx = 1;
    STL_RING_FOREACH_ENTRY( aColumnList, sColumnInfo )
    {
        if( ( sColumnInfo->mDataTypeId == DTL_TYPE_INTERVAL_YEAR_TO_MONTH ) ||
            ( sColumnInfo->mDataTypeId == DTL_TYPE_INTERVAL_DAY_TO_SECOND )  )
        {
            /**
             * precision 값을 얻는다.
             */
            STL_TRY( ztcdSQLGetDescField( sDescIrd,
                                          (SQLSMALLINT)sIdx,
                                          SQL_DESC_PRECISION,
                                          (SQLPOINTER) & sPrecision,
                                          STL_SIZEOF( SQLSMALLINT ),
                                          NULL,
                                          aErrorStack ) == STL_SUCCESS );

            STL_TRY( ztcdSQLGetDescField( sDescIrd,
                                         (SQLSMALLINT)sIdx,
                                         SQL_DESC_DATETIME_INTERVAL_PRECISION,
                                         (SQLPOINTER) & sDatetimeIntervalPrecision,
                                         STL_SIZEOF( SQLINTEGER ),
                                         NULL,
                                         aErrorStack ) == STL_SUCCESS );

            sColumnInfo->mLeadingPrecision = sDatetimeIntervalPrecision;
            sColumnInfo->mSecondPrecision  = sPrecision;
        }
        sIdx++;
    }

    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle,
                                  aErrorStack );
    }

    return STL_FAILURE;
}


stlStatus ztcdBuildTableInfoMeta( SQLHDBC               aDbcHandle,
                                  stlAllocator        * aAllocator,
                                  ztcMasterTableMeta  * aTableMeta,
                                  stlErrorStack       * aErrorStack )
{
    stlChar    sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt64   sSchemaId            = 0;
    stlInt64   sTableId             = 0;
    stlInt64   sPrimaryConstraintId = 0;
    stlInt32   sState               = 0;
    stlBool    sIsNullable          = STL_FALSE;
    stlBool    sIsPrimaryKey        = STL_FALSE;
    stlBool    sIsUnique            = STL_FALSE;
    stlBool    sFound               = STL_FALSE;
    stlInt64   sIntervalTypeId;
    
    stlInt64   sColumnId;
    stlChar    sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar    sDataTypeStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar    sIsNullableStr[STL_MAX_SQL_IDENTIFIER_LENGTH];

    SQLHSTMT   sSelStmtHandle;
    SQLLEN     sIndicator[3];
    SQLRETURN  sResult;
    
    ztcColumnInfo  * sColumnInfo = NULL;
    dtlFuncVector    sDTVector;

    STL_TRY( ztcdGetSchemaIDFromDB( aDbcHandle,
                                    aTableMeta->mSchemaName,
                                    &sSchemaId,
                                    aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdGetTableIDFromDB( aDbcHandle,
                                   sSchemaId,
                                   aTableMeta->mTableName,
                                   &sTableId,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdGetPrimaryConstraintId( aDbcHandle,
                                         sSchemaId,
                                         sTableId,
                                         &sPrimaryConstraintId,
                                         &sFound,
                                         aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INVALID_PRIMARY_KEY );
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT "
                 "    COLUMN_NAME, IS_NULLABLE, COLUMN_ID "
                 "FROM DEFINITION_SCHEMA.COLUMNS "
                 "WHERE IS_UNUSED = FALSE "
                 "  AND SCHEMA_ID = %ld AND TABLE_ID = %ld",
                 sSchemaId, sTableId );
                                  
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sSelStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sSelStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sSelStmtHandle,
                             1,
                             SQL_C_CHAR,
                             sColumnName,
                             STL_SIZEOF( sColumnName ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sSelStmtHandle,
                             2,
                             SQL_C_CHAR,
                             sIsNullableStr,
                             STL_SIZEOF( sIsNullableStr ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sSelStmtHandle,
                             3,
                             SQL_C_SBIGINT, 
                             &sColumnId,
                             STL_SIZEOF( sColumnId ),
                             &sIndicator[2],
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLExecute( sSelStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );

        stlMemset( sIsNullableStr, 0x00, STL_SIZEOF( sIsNullableStr ) );
        stlMemset( sDataTypeStr,   0x00, STL_SIZEOF( sDataTypeStr ) );
        stlMemset( sColumnName,    0x00, STL_SIZEOF( sColumnName ) );
        
        STL_TRY( ztcdSQLFetch( sSelStmtHandle, 
                               &sResult, 
                               aErrorStack ) == STL_SUCCESS );
        
        if( sResult == SQL_NO_DATA )
        {
            break;
        }
      
        STL_TRY( ztcdGetColumnDataTypeStr( aDbcHandle,
                                           sTableId,
                                           sColumnId, 
                                           sDataTypeStr,
                                           aErrorStack ) == STL_SUCCESS );

        
        STL_TRY( ztcdGetColumnConstraint( aDbcHandle,
                                          sSchemaId,
                                          sTableId,
                                          sColumnId,
                                          sPrimaryConstraintId,
                                          &sIsPrimaryKey,
                                          aErrorStack ) == STL_SUCCESS );

        
        STL_TRY( ztcdGetColumnUniqueConstraint( aDbcHandle,
                                                sSchemaId,
                                                sTableId,
                                                sColumnId,
                                                &sIsUnique,
                                                aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcmSetSyncDTFuncVector( &sDTVector ) == STL_SUCCESS );

        if( stlStrcmp( sIsNullableStr, "TRUE" ) == 0 )
        {
            sIsNullable = STL_TRUE;
        }
        else if ( stlStrcmp( sIsNullableStr, "FALSE" ) == 0 )
        {
            sIsNullable = STL_FALSE;
        }
        
        aTableMeta->mColumnCount++;
        
        /**
         * Meta 구성
         */
        STL_TRY( stlAllocRegionMem( aAllocator,
                                    STL_SIZEOF( ztcColumnInfo ),
                                    (void**)&sColumnInfo,
                                    aErrorStack ) == STL_SUCCESS );
        
        STL_RING_INIT_DATALINK( sColumnInfo,
                                STL_OFFSETOF( ztcColumnInfo, mLink ) );
        
        stlStrncpy( sColumnInfo->mColumnName, sColumnName, stlStrlen( sColumnName ) + 1 );
        stlStrncpy( sColumnInfo->mDataTypeStr, sDataTypeStr, stlStrlen( sDataTypeStr ) + 1 );
        sColumnInfo->mIsPrimary  = sIsPrimaryKey;
        sColumnInfo->mIsUnique   = sIsUnique;
        sColumnInfo->mIsNullable = sIsNullable;
        
        STL_TRY( ztcdGetColumnId( aDbcHandle,
                                  sSchemaId,
                                  sTableId,
                                  sColumnInfo->mColumnName,
                                  &sColumnInfo->mColumnId,
                                  aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztcdGetColumnTypeId( aDbcHandle,
                                      sTableId,
                                      sColumnId,
                                      &sColumnInfo->mDataTypeId,
                                      &sIntervalTypeId,
                                      aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztcdGetColumnParamInfo( aDbcHandle,
                                         sTableId,
                                         &sDTVector,
                                         sColumnInfo,
                                         aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sColumnInfo->mStrValueMaxSize != 0 );

        STL_RING_ADD_LAST( &aTableMeta->mColumnMetaList, 
                           sColumnInfo );
    }
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sSelStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdGetIntervalTypeInfo( aDbcHandle,
                                      aTableMeta->mSchemaName,
                                      aTableMeta->mTableName,
                                      &aTableMeta->mColumnMetaList,
                                      aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PRIMARY_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_PRIMARY_KEY,
                      NULL,
                      aErrorStack,
                      aTableMeta->mSchemaName,
                      aTableMeta->mTableName );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) ztcdSQLFreeHandle( sSelStmtHandle, 
                                      aErrorStack );
            break;
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztcdGetColumnDataTypeStr( SQLHDBC         aDbcHandle,
                                    stlInt64        aTableId,
                                    stlInt64        aColumnId,
                                    stlChar       * aDataTypeStr,
                                    stlErrorStack * aErrorStack )
{
    stlChar    sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt64   sDataTypeId     = 0;
    stlInt64   sIntervalTypeId = 0;
    
    stlInt32   sNumericPrecision;
    stlInt32   sNumericScale;
    stlInt32   sNumericRadix;
    stlInt64   sCharacterLength;
    stlInt32   sDateTimePrecision;
    stlInt32   sIntervalPrecision;
    
    stlInt32   sState = 0;
    
    SQLHSTMT   sStmtHandle;
    SQLLEN     sIndicator[3];
    SQLRETURN  sResult;
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdGetColumnTypeId( aDbcHandle,
                                  aTableId,
                                  aColumnId,
                                  &sDataTypeId,
                                  &sIntervalTypeId,
                                  aErrorStack ) == STL_SUCCESS );
    
    switch ( sDataTypeId )
    {
        case DTL_TYPE_BOOLEAN:
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
        case DTL_TYPE_DATE:
        case DTL_TYPE_LONGVARBINARY:
        case DTL_TYPE_LONGVARCHAR:
        {
            /** Only DataType String */
            break;
        }
        case DTL_TYPE_FLOAT:
        case DTL_TYPE_NUMBER:
        {
            stlSnprintf( sSqlString,
                         ZTC_COMMAND_BUFFER_SIZE,
                         "SELECT NUMERIC_PRECISION, NUMERIC_SCALE, NUMERIC_PRECISION_RADIX "
                         "FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR "
                         "WHERE TABLE_ID = %ld AND COLUMN_ID = %ld",
                         aTableId,
                         aColumnId );
        
            STL_TRY( ztcdSQLBindCol( sStmtHandle,
                                     1,
                                     SQL_C_SLONG, 
                                     &sNumericPrecision,
                                     STL_SIZEOF( sNumericPrecision ),
                                     &sIndicator[0],
                                     aErrorStack ) == STL_SUCCESS );
        
            STL_TRY( ztcdSQLBindCol( sStmtHandle,
                                     2,
                                     SQL_C_SLONG, 
                                     &sNumericScale,
                                     STL_SIZEOF( sNumericScale ),
                                     &sIndicator[1],
                                     aErrorStack ) == STL_SUCCESS );

            STL_TRY( ztcdSQLBindCol( sStmtHandle,
                                     3,
                                     SQL_C_SLONG, 
                                     &sNumericRadix,
                                     STL_SIZEOF( sNumericRadix ),
                                     &sIndicator[2],
                                     aErrorStack ) == STL_SUCCESS );
        
            break;
        }
        case DTL_TYPE_CHAR:
        case DTL_TYPE_VARCHAR:
        {
            stlSnprintf( sSqlString,
                         ZTC_COMMAND_BUFFER_SIZE,
                         "SELECT CHARACTER_MAXIMUM_LENGTH "
                         "FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR "
                         "WHERE TABLE_ID = %ld AND COLUMN_ID = %ld",
                         aTableId,
                         aColumnId );
        
            STL_TRY( ztcdSQLBindCol( sStmtHandle,
                                     1,
                                     SQL_C_SBIGINT, 
                                     &sCharacterLength,
                                     STL_SIZEOF( sCharacterLength ),
                                     &sIndicator[0],
                                     aErrorStack ) == STL_SUCCESS );
            break;
        }
        case DTL_TYPE_BINARY:
        case DTL_TYPE_VARBINARY:
        {
            stlSnprintf( sSqlString,
                         ZTC_COMMAND_BUFFER_SIZE,
                         "SELECT CHARACTER_OCTET_LENGTH "
                         "FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR "
                         "WHERE TABLE_ID = %ld AND COLUMN_ID = %ld",
                         aTableId,
                         aColumnId );
        
            STL_TRY( ztcdSQLBindCol( sStmtHandle,
                                     1,
                                     SQL_C_SBIGINT, 
                                     &sCharacterLength,
                                     STL_SIZEOF( sCharacterLength ),
                                     &sIndicator[0],
                                     aErrorStack ) == STL_SUCCESS );
            break;
        }
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
        {
            stlSnprintf( sSqlString,
                         ZTC_COMMAND_BUFFER_SIZE,
                         "SELECT DATETIME_PRECISION "
                         "FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR "
                         "WHERE TABLE_ID = %ld AND COLUMN_ID = %ld",
                         aTableId,
                         aColumnId );
        
            STL_TRY( ztcdSQLBindCol( sStmtHandle,
                                     1,
                                     SQL_C_SLONG, 
                                     &sDateTimePrecision,
                                     STL_SIZEOF( sDateTimePrecision ),
                                     &sIndicator[0],
                                     aErrorStack ) == STL_SUCCESS );
        
            break;
        }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
        {
            stlSnprintf( sSqlString,
                         ZTC_COMMAND_BUFFER_SIZE,
                         "SELECT INTERVAL_PRECISION, DATETIME_PRECISION "
                         "FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR "
                         "WHERE TABLE_ID = %ld AND COLUMN_ID = %ld",
                         aTableId,
                         aColumnId );
        
            STL_TRY( ztcdSQLBindCol( sStmtHandle,
                                     1,
                                     SQL_C_SLONG, 
                                     &sIntervalPrecision,
                                     STL_SIZEOF( sIntervalPrecision ),
                                     &sIndicator[0],
                                     aErrorStack ) == STL_SUCCESS );
        
            STL_TRY( ztcdSQLBindCol( sStmtHandle,
                                     2,
                                     SQL_C_SLONG, 
                                     &sDateTimePrecision,
                                     STL_SIZEOF( sDateTimePrecision ),
                                     &sIndicator[1],
                                     aErrorStack ) == STL_SUCCESS );
            break;
        }
        case DTL_TYPE_DECIMAL:
        case DTL_TYPE_BLOB:
        case DTL_TYPE_CLOB:
        {
            /** Not Supported Yet.*/
            break;
        }
        default:
            STL_DASSERT( 0 );
            break;
    }
    
    
    if( stlStrlen( sSqlString ) > 0 )
    {
        STL_TRY( ztcdSQLExecDirect( sStmtHandle, 
                                    (SQLCHAR*)sSqlString, 
                                    stlStrlen( sSqlString ),
                                    &sResult,
                                    aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcdSQLFetch( sStmtHandle, 
                               &sResult, 
                               aErrorStack ) == STL_SUCCESS );
    
        STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INVALID_TABLE_NAME );
    }
    
    switch ( sDataTypeId )
    {
        case DTL_TYPE_BOOLEAN:
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
        case DTL_TYPE_DATE:
        case DTL_TYPE_LONGVARBINARY:
        case DTL_TYPE_LONGVARCHAR:
        {
            stlSnprintf( aDataTypeStr,
                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                         "%s",
                         dtlDataTypeName[sDataTypeId] );
            break;
        }
        case DTL_TYPE_FLOAT:
        {
            if( sIndicator[0] == SQL_NULL_DATA )
            {
                stlSnprintf( aDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             dtlDataTypeName[sDataTypeId] );
            }
            else
            {
                stlSnprintf( aDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s(%d)",
                             dtlDataTypeName[sDataTypeId], 
                             sNumericPrecision );
            }
            break;
        }
        case DTL_TYPE_NUMBER:
        {
            if( sIndicator[0] == SQL_NULL_DATA )
            {
                stlSnprintf( aDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             dtlDataTypeName[sDataTypeId] );
            }
            else
            {
                if( sIndicator[1] == SQL_NULL_DATA )
                {
                    stlSnprintf( aDataTypeStr,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
                                 "%s(%d)",
                                 dtlDataTypeName[sDataTypeId], 
                                 sNumericPrecision );
                }
                else
                {
                    if( sNumericScale == DTL_SCALE_NA )
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s",
                                     dtlDataTypeName[sDataTypeId] );
                    }
                    else
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s(%d,%d)",
                                     dtlDataTypeName[sDataTypeId], 
                                     sNumericPrecision,
                                     sNumericScale );
                    }
                }
            }
            break;
        }
        case DTL_TYPE_CHAR:
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_BINARY:
        case DTL_TYPE_VARBINARY:
        {
            if( sIndicator[0] == SQL_NULL_DATA )
            {
                stlSnprintf( aDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             dtlDataTypeName[sDataTypeId] );
            }
            else
            {
                stlSnprintf( aDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s(%ld)",
                             dtlDataTypeName[sDataTypeId],
                             sCharacterLength );
            }
            break;
        }
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIMESTAMP:
        {
            if( sIndicator[0] == SQL_NULL_DATA )
            {
                stlSnprintf( aDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             dtlDataTypeName[sDataTypeId] );
            }
            else
            {
                stlSnprintf( aDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s(%d)",
                             dtlDataTypeName[sDataTypeId],
                             sDateTimePrecision );
            }
            break;
        }
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        {
            if( sIndicator[0] == SQL_NULL_DATA )
            {
                stlSnprintf( aDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             dtlDataTypeName[sDataTypeId] );
            }
            else
            {
                stlSnprintf( aDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "TIME(%d) WITH TIME ZONE",
                             sDateTimePrecision );
            }
            break;
        }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
        {
            if( sIndicator[0] == SQL_NULL_DATA )
            {
                stlSnprintf( aDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "%s",
                             dtlDataTypeName[sDataTypeId] );
            }
            else
            {
                stlSnprintf( aDataTypeStr,
                             STL_MAX_SQL_IDENTIFIER_LENGTH,
                             "TIMESTAMP(%d) WITH TIME ZONE",
                             sDateTimePrecision );
            }
            break;
        }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
        {
            switch( sIntervalTypeId )
            {
                case DTL_INTERVAL_INDICATOR_YEAR:
                case DTL_INTERVAL_INDICATOR_MONTH:
                case DTL_INTERVAL_INDICATOR_DAY:
                case DTL_INTERVAL_INDICATOR_HOUR:
                case DTL_INTERVAL_INDICATOR_MINUTE:
                {
                    if( sIndicator[0] == SQL_NULL_DATA )
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s %s",
                                     dtlDataTypeName[sDataTypeId],
                                     gDtlIntervalIndicatorString[sIntervalTypeId] );
                    }
                    else
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s %s(%d)",
                                     dtlDataTypeName[sDataTypeId],
                                     gDtlIntervalIndicatorString[sIntervalTypeId],
                                     sIntervalPrecision );
                    }
                    break;      
                }
                case DTL_INTERVAL_INDICATOR_SECOND:
                {
                    if( sIndicator[0] == SQL_NULL_DATA )
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s %s",
                                     dtlDataTypeName[sDataTypeId],
                                     gDtlIntervalIndicatorString[sIntervalTypeId] );
                    }
                    else
                    {
                        if( sIndicator[1] == SQL_NULL_DATA )
                        {
                            stlSnprintf( aDataTypeStr,
                                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                                         "%s %s(%d)",
                                         dtlDataTypeName[sDataTypeId],
                                         gDtlIntervalIndicatorString[sIntervalTypeId],
                                         sIntervalPrecision );
                        }
                        else
                        {
                            stlSnprintf( aDataTypeStr,
                                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                                         "%s %s(%d, %d)",
                                         dtlDataTypeName[sDataTypeId],
                                         gDtlIntervalIndicatorString[sIntervalTypeId],
                                         sIntervalPrecision,
                                         sDateTimePrecision );
                        }
                    }
                    break;   
                }
                case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                {
                    if( sIndicator[0] == SQL_NULL_DATA )
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s %s",
                                     dtlDataTypeName[sDataTypeId],
                                     gDtlIntervalIndicatorString[sIntervalTypeId] );
                    }
                    else
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s YEAR(%d) TO MONTH",
                                     dtlDataTypeName[sDataTypeId],
                                     sIntervalPrecision );
                    }
                    break;
            
                }
                case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                {
                    if( sIndicator[0] == SQL_NULL_DATA )
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s %s",
                                     dtlDataTypeName[sDataTypeId],
                                     gDtlIntervalIndicatorString[sIntervalTypeId] );
                    }
                    else
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s DAY(%d) TO HOUR",
                                     dtlDataTypeName[sDataTypeId],
                                     sIntervalPrecision );
                    }
                    break;
                }
                case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                {
                    if( sIndicator[0] == SQL_NULL_DATA )
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s %s",
                                     dtlDataTypeName[sDataTypeId],
                                     gDtlIntervalIndicatorString[sIntervalTypeId] );
                    }
                    else
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s DAY(%d) TO MINUTE",
                                     dtlDataTypeName[sDataTypeId],
                                     sIntervalPrecision );
                    }
                    break;
                }
                case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                {
                    if( sIndicator[0] == SQL_NULL_DATA )
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s %s",
                                     dtlDataTypeName[sDataTypeId],
                                     gDtlIntervalIndicatorString[sIntervalTypeId] );
                    }
                    else
                    {
                        if( sIndicator[1] == SQL_NULL_DATA )
                        {
                            stlSnprintf( aDataTypeStr,
                                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                                         "%s DAY(%d) TO SECOND",
                                         dtlDataTypeName[sDataTypeId],
                                         sIntervalPrecision );
                        }
                        else
                        {
                            stlSnprintf( aDataTypeStr,
                                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                                         "%s DAY(%d) TO SECOND(%d)",
                                         dtlDataTypeName[sDataTypeId],
                                         sIntervalPrecision,
                                         sDateTimePrecision );
                        }
                    }
                    break;
                }
                case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                {
                    if( sIndicator[0] == SQL_NULL_DATA )
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s %s",
                                     dtlDataTypeName[sDataTypeId],
                                     gDtlIntervalIndicatorString[sIntervalTypeId] );
                    }
                    else
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s HOUR(%d) TO MINUTE",
                                     dtlDataTypeName[sDataTypeId],
                                     sIntervalPrecision );
                    }
                    break;
                }
                case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                {
                    if( sIndicator[0] == SQL_NULL_DATA )
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s %s",
                                     dtlDataTypeName[sDataTypeId],
                                     gDtlIntervalIndicatorString[sIntervalTypeId] );
                    }
                    else
                    {
                        if( sIndicator[1] == SQL_NULL_DATA )
                        {
                            stlSnprintf( aDataTypeStr,
                                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                                         "%s HOUR(%d) TO SECOND",
                                         dtlDataTypeName[sDataTypeId],
                                         sIntervalPrecision );
                        }
                        else
                        {
                            stlSnprintf( aDataTypeStr,
                                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                                         "%s HOUR(%d) TO SECOND(%d)",
                                         dtlDataTypeName[sDataTypeId],
                                         sIntervalPrecision,
                                         sDateTimePrecision );
                        }
                    }
                    break;
                }
                case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                {
                    if( sIndicator[0] == SQL_NULL_DATA )
                    {
                        stlSnprintf( aDataTypeStr,
                                     STL_MAX_SQL_IDENTIFIER_LENGTH,
                                     "%s %s",
                                     dtlDataTypeName[sDataTypeId],
                                     gDtlIntervalIndicatorString[sIntervalTypeId] );
                    }
                    else
                    {
                        if( sIndicator[1] == SQL_NULL_DATA )
                        {
                            stlSnprintf( aDataTypeStr,
                                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                                         "%s MINUTE(%d) TO SECOND",
                                         dtlDataTypeName[sDataTypeId],
                                         sIntervalPrecision );
                        }
                        else
                        {
                            stlSnprintf( aDataTypeStr,
                                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                                         "%s MINUTE(%d) TO SECOND(%d)",
                                         dtlDataTypeName[sDataTypeId],
                                         sIntervalPrecision,
                                         sDateTimePrecision );
                        }
                    }
                    break;
                }
                default:
                    break;
            }

            break;
        }
        case DTL_TYPE_DECIMAL:
        case DTL_TYPE_BLOB:
        case DTL_TYPE_CLOB:
        {
            STL_DASSERT( 0 );
            /** Not Supported Yet.*/
            break;
        }
        default:
            STL_DASSERT( 0 );
            break;
    }
        
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS ); 
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_TABLE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_TABLE,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }
    
    return STL_FAILURE;
}


stlStatus ztcdGetTablePhysicalIDFromDB( SQLHDBC         aDbcHandle,
                                        stlChar       * aSchemaName,
                                        stlChar       * aTableName,
                                        stlInt64      * aPhysicalId,
                                        stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt64      sPhysicalId;
    stlInt64      sTableId;
    stlInt64      sSchemaId;
    stlInt32      sState = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator;
    SQLRETURN     sResult;
    
    STL_TRY( ztcdGetSchemaIDFromDB( aDbcHandle,
                                    aSchemaName,
                                    &sSchemaId,
                                    aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdGetTableIDFromDB( aDbcHandle,
                                   sSchemaId,
                                   aTableName,
                                   &sTableId,
                                   aErrorStack ) == STL_SUCCESS );
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT PHYSICAL_ID "
                 "FROM DEFINITION_SCHEMA.TABLES "
                 "WHERE SCHEMA_ID=%ld AND TABLE_ID=%ld",
                 sSchemaId,
                 sTableId );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT, 
                             &sPhysicalId,
                             STL_SIZEOF( sPhysicalId ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INTERNAL );
    
    *aPhysicalId = sPhysicalId;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );   
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "get Table PhysicalId" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;
}


stlStatus ztcdGetCurrentLogBufferInfo( SQLHDBC         aDbcHandle,
                                       stlInt16      * aGroupId,
                                       stlInt32      * aBlockSeq,
                                       stlInt64      * aFileSeqNo,
                                       stlErrorStack * aErrorStack )
{
    stlChar          sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32         sBlockSeq    = 0;
    stlInt16         sGroupId     = 0;
    stlInt64         sFileSeqNo   = ZTC_INVALID_FILE_SEQ_NO;
    stlInt32         sState       = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLRETURN     sResult;
    SQLLEN        sIndicator[3];
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT GROUP_ID, GRP.FILE_SEQ_NO, BUF.REAR_LID_BLOCK_SEQ_NO "
                 "FROM FIXED_TABLE_SCHEMA.X$LOG_GROUP GRP, FIXED_TABLE_SCHEMA.X$LOG_BUFFER BUF " 
                 "WHERE BUF.FILE_SEQ_NO=GRP.FILE_SEQ_NO ");
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SSHORT, 
                             &sGroupId,
                             STL_SIZEOF( sGroupId ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_SBIGINT, 
                             &sFileSeqNo,
                             STL_SIZEOF( sFileSeqNo ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             3,
                             SQL_C_SLONG, 
                             &sBlockSeq,
                             STL_SIZEOF( sBlockSeq ),
                             &sIndicator[2],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INTERNAL );
    
    *aGroupId   = sGroupId;
    *aBlockSeq  = sBlockSeq;
    *aFileSeqNo = sFileSeqNo;

    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );   
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Get Current LogBuffer Info" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;
}


stlStatus ztcdGetLogFileStatus( SQLHDBC         aDbcHandle,
                                stlInt16      * aGroupId,
                                stlInt64      * aFileSeqNo,
                                stlInt32      * aBlockSeq,
                                stlErrorStack * aErrorStack )
{
    stlChar          sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32         sBlockSeq    = 0;
    stlInt16         sGroupId     = 0;
    stlInt64         sFileSeqNo   = ZTC_INVALID_FILE_SEQ_NO;
    stlInt32         sState       = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLRETURN     sResult;
    SQLLEN        sIndicator[3];
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT GROUP_ID, GRP.FILE_SEQ_NO, BUF.REAR_LID_BLOCK_SEQ_NO "
                 "FROM FIXED_TABLE_SCHEMA.X$LOG_GROUP GRP, FIXED_TABLE_SCHEMA.X$LOG_BUFFER BUF " 
                 "WHERE BUF.FILE_SEQ_NO=GRP.FILE_SEQ_NO ");
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SSHORT, 
                             &sGroupId,
                             STL_SIZEOF( sGroupId ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_SBIGINT, 
                             &sFileSeqNo,
                             STL_SIZEOF( sFileSeqNo ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             3,
                             SQL_C_SLONG, 
                             &sBlockSeq,
                             STL_SIZEOF( sBlockSeq ),
                             &sIndicator[2],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( sResult != SQL_NO_DATA );

    *aGroupId   = sGroupId;
    *aBlockSeq  = sBlockSeq;
    *aFileSeqNo = sFileSeqNo;

    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );   
    
    return STL_SUCCESS;

    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;
}


stlStatus ztcdGetCharacterOctetLength( SQLHDBC         aDbcHandle,
                                       stlInt64        aTableId,
                                       stlInt64        aColumnId,
                                       stlInt64      * aOctetLength,
                                       stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt64      sOctetLength = 0;
    stlInt32      sState       = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator;
    SQLRETURN     sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT CHARACTER_OCTET_LENGTH "
                 "FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR "
                 "WHERE TABLE_ID=%ld AND COLUMN_ID=%ld",
                 aTableId,
                 aColumnId );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT, 
                             &sOctetLength,
                             STL_SIZEOF( sOctetLength ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INTERNAL );
        
    *aOctetLength = sOctetLength;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );  
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "get Character Octet Length" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;
}


stlStatus ztcdGetNumericInfo( SQLHDBC         aDbcHandle,
                              stlInt64        aTableId,
                              stlInt64        aColumnId,
                              stlInt32      * aPrecision,
                              stlInt32      * aScale,
                              stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32      sPrecision = 0;
    stlInt32      sScale     = 0;
    stlInt32      sState     = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator[2];
    SQLRETURN     sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT NUMERIC_PRECISION, NUMERIC_SCALE "
                 "FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR "
                 "WHERE TABLE_ID=%ld AND COLUMN_ID=%ld",
                 aTableId,
                 aColumnId );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SLONG, 
                             &sPrecision,
                             STL_SIZEOF( sPrecision ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_SLONG, 
                             &sScale,
                             STL_SIZEOF( sScale ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INTERNAL );
        
    *aPrecision = sPrecision;
    *aScale     = sScale;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );  
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "get Numeric information" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;    
}

stlStatus ztcdGetCharacterMaxLength( SQLHDBC         aDbcHandle,
                                     stlInt64        aTableId,
                                     stlInt64        aColumnId,
                                     stlInt64      * aCharLength,
                                     stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt64      sCharLength = 0;
    stlInt32      sState      = 0;
    
    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator;
    SQLRETURN     sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT CHARACTER_MAXIMUM_LENGTH "
                 "FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR "
                 "WHERE TABLE_ID=%ld AND COLUMN_ID=%ld",
                 aTableId,
                 aColumnId );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT, 
                             &sCharLength ,
                             STL_SIZEOF( sCharLength  ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INTERNAL );
        
    *aCharLength  = sCharLength;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );  
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "get Character Max Length" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;
    
}


stlStatus ztcdGetCharacterLengthUnit( SQLHDBC         aDbcHandle,
                                      stlInt64        aTableId,
                                      stlInt64        aColumnId,
                                      stlInt16      * aCharLengthUnit,
                                      stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32      sCharLengthUnit = 0;
    stlInt32      sState      = 0;

    SQLHSTMT      sStmtHandle;
    SQLLEN        sIndicator;
    SQLRETURN     sResult;

    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT STRING_LENGTH_UNIT_ID "
                 "FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR "
                 "WHERE TABLE_ID=%ld AND COLUMN_ID=%ld",
                 aTableId,
                 aColumnId );

    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SLONG,
                             &sCharLengthUnit ,
                             STL_SIZEOF( sCharLengthUnit  ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLExecute( sStmtHandle,
                             &sResult,
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLFetch( sStmtHandle,
                           &sResult,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INTERNAL );

    *aCharLengthUnit = (stlInt16)sCharLengthUnit;

    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle,
                                aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "get Character Max Length" );
    }
    STL_FINISH;

    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle,
                                  aErrorStack );
    }

    return STL_FAILURE;

}

stlStatus ztcdResetSlaveMeta( SQLHDBC         aDbcHandle,
                              stlErrorStack * aErrorStack )
{
    stlChar     sSqlMeta[ZTC_COMMAND_BUFFER_SIZE];
    SQLHSTMT    sStmtHandle;
    stlInt32    sState = 0;
    SQLRETURN   sResult;
    
    stlSnprintf( sSqlMeta,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "DELETE FROM CYCLONEM_SLAVE_META WHERE GROUP_NAME='%s'",
                 gConfigure.mCurrentGroupPtr->mName );
    
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLExecDirect( sStmtHandle,
                                (SQLCHAR*)sSqlMeta,
                                SQL_NTS,
                                &sResult,
                                aErrorStack ) == STL_SUCCESS );
    
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS ); 
    
    STL_TRY( ztcdSQLEndTran( SQL_HANDLE_DBC,
                             aDbcHandle,
                             SQL_COMMIT,
                             STL_FALSE,
                             aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }
    
    return STL_FAILURE;
}

stlStatus ztcdPrintDiagnostic( SQLSMALLINT     aHandleType,
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
                                           ZTC_DATA_SOURCE_IDENTIFIER ) +
                                stlStrlen( ZTC_DATA_SOURCE_IDENTIFIER ) );

        while( stlIsspace( *sRefinedMessageText ) == STL_TRUE )
        {
            sRefinedMessageText++;
        }
        
        if( gConfigure.mIsSilent == STL_FALSE )
        {
            stlPrintf("ERR-%s(%d): %s\n", sSqlState, sNaiveError, sRefinedMessageText );
        }

        sRecNumber++;
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
    
}


stlStatus ztcdCreateMetaTable4Slave( stlErrorStack * aErrorStack )
{
    stlChar       sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlChar       sCreateString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt32      sState = 0;
    
    SQLHENV       sEnvHandle;
    SQLHDBC       sDbcHandle;
    SQLHSTMT      sStmtHandle;
    
    STL_TRY( ztcdAllocEnvHandle( &sEnvHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdCreateConnection( &sDbcHandle,
                                   sEnvHandle,
                                   STL_FALSE,
                                   aErrorStack ) == STL_SUCCESS );
    sState = 2;
    STL_TRY( ztcdSQLAllocHandle( sDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 3;

    /**
     * User가 추가되면 해당 사용자에서 처리해야 한다.
     * Error가 "table or view does not exist" 인지도 확인해야 함
     */
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT * "
                 "FROM CYCLONEM_SLAVE_META " );
    
    if( SQLPrepare( sStmtHandle,
                   (SQLCHAR*)sSqlString,
                   stlStrlen( sSqlString ) ) != SQL_SUCCESS )
    {
        stlSnprintf( sCreateString,
                     ZTC_COMMAND_BUFFER_SIZE,
                     "CREATE TABLE CYCLONEM_SLAVE_META "
                     "( GROUP_NAME VARCHAR(128), MASTER_SCHEMA_NAME VARCHAR(128), MASTER_TABLE_NAME VARCHAR(128), "
                     "SLAVE_SCHEMA_NAME VARCHAR(128), SLAVE_TABLE_NAME VARCHAR(128), TABLE_ID INTEGER )" );
        
        if( SQLExecDirect( sStmtHandle, 
                           (SQLCHAR*)sCreateString, 
                           stlStrlen( sCreateString ) ) != SQL_SUCCESS )
        {
            /** TODO : Already Created ? */
        }
    }
    

    sState = 2;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle,
                                aErrorStack ) == SQL_SUCCESS );
    
    STL_TRY( ztcdSQLEndTran( SQL_HANDLE_DBC,
                             sDbcHandle,
                             SQL_COMMIT,
                             STL_FALSE,
                             aErrorStack ) == SQL_SUCCESS );
    sState = 1;
    STL_TRY( ztcdDestroyConnection( sDbcHandle,
                                    aErrorStack ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( ztcdFreeEnvHandle( sEnvHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) ztcdSQLFreeHandle( sStmtHandle,
                                      aErrorStack );
        case 2:
            (void) ztcdDestroyConnection( sDbcHandle,
                                          aErrorStack );
        case 1:
            (void) ztcdFreeEnvHandle( sEnvHandle, 
                                      aErrorStack );
        default:
            break;
    }
    return STL_FAILURE;
}


stlStatus ztcdGetSupplLogTableValue( SQLHDBC         aDbcHandle,
                                     stlChar       * aSchemaName,
                                     stlChar       * aTableName, 
                                     stlBool       * aValue,
                                     stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlChar     sValue[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt32    sState = 0;
    
    SQLHSTMT    sStmtHandle;
    SQLLEN      sIndicator;
    SQLRETURN   sResult;

    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT SUPPLEMENTAL_LOG_DATA_PK FROM DICTIONARY_SCHEMA.SUPPLEMENTAL_LOG_TABLE_INFO "
                 "WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s'",
                 aSchemaName,
                 aTableName );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_CHAR,
                             sValue,
                             STL_SIZEOF( sValue ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INTERNAL );
    
    if( stlStrcmp( sValue, "IMPLICIT" ) == 0 ||
        stlStrcmp( sValue, "EXPLICIT" ) == 0 )
    {
        *aValue = STL_TRUE;
    }
    else
    {
        *aValue = STL_FALSE;
    }
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS ); 
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "get Supplemental Table Value" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;   
}


stlStatus ztcdGetDiagnosticMsg( SQLSMALLINT     aHandleType,
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
    stlChar       sMsg[ZTC_TRACE_BUFFER_SIZE] = {0,};
    

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
                                       ZTC_DATA_SOURCE_IDENTIFIER ) +
                            stlStrlen( ZTC_DATA_SOURCE_IDENTIFIER ) );

    while( stlIsspace( *sRefinedMessageText ) == STL_TRUE )
    {
        sRefinedMessageText++;
    }
  
    stlSnprintf( sMsg,
                 ZTC_TRACE_BUFFER_SIZE,
                 "%sERR-%s(%d) : %s",
                 sMsg,
                 sSqlState, sNaiveError, sRefinedMessageText );
    
    stlMemcpy( aMsg, sMsg, stlStrlen( sMsg ) );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
    
}


stlStatus ztcdGetMbMaxLength( SQLHDBC         aDbcHandle,
                              stlInt8       * aLength,
                              stlErrorStack * aErrorStack )
{
    stlChar         sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlChar         sCharacterSetStr[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt32        sState        = 0;
    dtlCharacterSet sCharacterSet = DTL_CHARACTERSET_MAX;
    
    SQLHSTMT    sStmtHandle;
    SQLLEN      sIndicator;
    SQLRETURN   sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT CHARACTER_SET_NAME FROM DEFINITION_SCHEMA.CATALOG_NAME" );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_CHAR,
                             sCharacterSetStr,
                             STL_SIZEOF( sCharacterSetStr ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERROR_INVALID_CHARACTERSET );
    
    if( stlStrcasecmp( sCharacterSetStr, "ASCII" ) == 0 )
    {
        sCharacterSet = DTL_SQL_ASCII;
    }
    else if( stlStrcasecmp( sCharacterSetStr, "UTF8" ) == 0 )
    {
        sCharacterSet = DTL_UTF8;
    }
    else if( stlStrcasecmp( sCharacterSetStr, "UHC" ) == 0 )
    {
        sCharacterSet = DTL_UHC;
    }
    else if( stlStrcasecmp( sCharacterSetStr, "GB18030" ) == 0 )
    {
        sCharacterSet = DTL_GB18030;
    }
    else
    {
        STL_THROW( RAMP_ERROR_INVALID_CHARACTERSET );
    }
    
    *aLength = dtlGetMbMaxLength( sCharacterSet );
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS ); 
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_CHARACTERSET )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Invalid Character Set" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;   
}

stlStatus ztcdGetPrimaryKeyIndexName( SQLHDBC         aDbcHandle,
                                      stlInt64        aSchemaId,
                                      stlInt64        aTableId,
                                      stlChar       * aIndexName,
                                      stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlChar     sIndexName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlInt32    sState = 0;
    
    SQLHSTMT    sStmtHandle;
    SQLLEN      sIndicator;
    SQLRETURN   sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT IDX.INDEX_NAME "
                 "FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS CONS, DEFINITION_SCHEMA.INDEXES IDX "
                 "WHERE CONS.TABLE_SCHEMA_ID=%ld AND CONS.TABLE_ID=%ld AND "
                 "IDX.INDEX_ID=CONS.ASSOCIATED_INDEX_ID AND CONS.CONSTRAINT_TYPE='PRIMARY KEY'",
                 aSchemaId,
                 aTableId );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_CHAR,
                             sIndexName,
                             STL_SIZEOF( sIndexName ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INTERNAL );
    
    stlStrncpy( aIndexName, sIndexName, stlStrlen( sIndexName ) + 1 );
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS ); 
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "get Primary Key IndexName" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;   
}

stlStatus ztcdSetDTFuncValue( SQLHDBC           aDbcHandle,
                              dtlCharacterSet * aCharacterSet,
                              SQLINTEGER      * aTimezone,
                              stlErrorStack   * aErrorStack )
{
    SQLCHAR         sCharsetName[ STL_MAX_SQL_IDENTIFIER_LENGTH ] = {0,};
    SQLINTEGER      sStringLength = 0;
    dtlCharacterSet sCharacterSet = DTL_CHARACTERSET_MAX;

    stlChar                    sTimezone[ DTL_MAX_TIMEZONE_LEN + 1 ] = {0,};
    dtlDataValue               sTimeZoneValue;
    dtlIntervalDayToSecondType sTimeZoneInterval;
    stlBool                    sTimeZoneWithInfo = STL_FALSE;
    stlInt32                   sTimeZoneOffset = 0;

    /*
     * SQL_ATTR_TIMEZONE
     */

    STL_TRY( SQLGetConnectAttr( aDbcHandle,
                                SQL_ATTR_TIMEZONE,
                                (SQLPOINTER)sTimezone,
                                STL_SIZEOF(sTimezone),
                                &sStringLength ) == SQL_SUCCESS );

    /*
     * TimeZone Interval String 을 dtlInterval 로 변경
     */
    sTimeZoneValue.mValue = &sTimeZoneInterval;

    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               &sTimeZoneValue,
                               aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlIntervalDayToSecondSetValueFromString( sTimezone,
                                                       sStringLength,
                                                       DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,
                                                       DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,
                                                       DTL_STRING_LENGTH_UNIT_NA,
                                                       DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                                                       DTL_INTERVAL_DAY_TO_SECOND_SIZE,
                                                       &sTimeZoneValue,
                                                       &sTimeZoneWithInfo,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       aErrorStack ) == STL_SUCCESS );

    /*
     * dtlInterval 을 Timezone Offset 으로 변경
     */

    STL_TRY( dtlTimeZoneToGMTOffset( &sTimeZoneInterval,
                                     &sTimeZoneOffset,
                                     aErrorStack )
             == STL_SUCCESS );

    *aTimezone = sTimeZoneOffset;

    /*
     * SQL_ATTR_CHARACTER_SET
     */

    STL_TRY( SQLGetConnectAttr( aDbcHandle,
                                SQL_ATTR_CHARACTER_SET,
                                (SQLPOINTER)sCharsetName,
                                STL_SIZEOF( sCharsetName ),
                                &sStringLength ) == SQL_SUCCESS );

    STL_TRY( dtlGetCharacterSet( (stlChar*)sCharsetName,
                                 &sCharacterSet,
                                 aErrorStack ) == STL_SUCCESS );

    *aCharacterSet = sCharacterSet;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcdSetPropagateMode( SQLHDBC         aDbcHandle,
                                stlErrorStack * aErrorStack )
{
    stlChar    sSqlString[ZTC_COMMAND_BUFFER_SIZE];
    SQLRETURN  sResult;
    SQLHSTMT   sStmtHandle;
    stlInt32   sState = 0;
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "ALTER SESSION SET SESSION_PROPAGATE_REDO_LOG=0" );
    
    STL_TRY( ztcdSQLExecDirect( sStmtHandle, 
                                (SQLCHAR*)sSqlString, 
                                stlStrlen( sSqlString ),
                                &sResult,
                                aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == SQL_SUCCESS, RAMP_ERR_SET_PROPAGATE_MODE );
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_SET_PROPAGATE_MODE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_PROPAGATE_MODE,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;
}

stlStatus ztcdGetArchiveLogInfo( SQLHDBC         aDbcHandle,
                                 stlChar       * aPath,
                                 stlChar       * aFilePrefix,
                                 stlBool       * aIsArchiveMode,
                                 stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlChar     sPath[STL_MAX_FILE_PATH_LENGTH] = {0,};
    stlChar     sPrefix[STL_MAX_FILE_PATH_LENGTH] = {0,};
    stlBool     sIsArchiveMode = STL_TRUE;
    stlInt32    sState         = 0;
    
    SQLHSTMT    sStmtHandle;
    SQLLEN      sIndicator[2];
    SQLRETURN   sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT ARCHIVELOG_DIR_1, ARCHIVELOG_FILE "
                 "FROM  FIXED_TABLE_SCHEMA.X$ARCHIVELOG "
                 "WHERE ARCHIVELOG_MODE='ARCHIVELOG'" );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_CHAR,
                             sPath,
                             STL_SIZEOF( sPath ),
                             &sIndicator[0],
                             aErrorStack ) == STL_SUCCESS );

    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             2,
                             SQL_C_CHAR,
                             sPrefix,
                             STL_SIZEOF( sPrefix ),
                             &sIndicator[1],
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    if( sResult == SQL_NO_DATA )
    {
        sIsArchiveMode = STL_FALSE;
    }
    else
    {
        stlStrncpy( aPath, sPath, stlStrlen( sPath ) + 1 );
        stlStrncpy( aFilePrefix, sPrefix, stlStrlen( sPrefix ) + 1 );
    }
    
    *aIsArchiveMode = sIsArchiveMode;
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS ); 
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;   
    
}


stlStatus ztcdGetMinBeginLsn( SQLHDBC         aDbcHandle,
                              stlInt64      * aBeginLsn,
                              stlErrorStack * aErrorStack )
{
    stlChar     sSqlString[ZTC_COMMAND_BUFFER_SIZE] = {0,};
    stlInt64    sBeginLsn = ZTC_INVALID_LSN;
    stlInt32    sState    = 0;
    
    SQLHSTMT    sStmtHandle;
    SQLLEN      sIndicator;
    SQLRETURN   sResult;
    
    stlSnprintf( sSqlString,
                 ZTC_COMMAND_BUFFER_SIZE,
                 "SELECT MIN(BEGIN_LSN) FROM X$TRANSACTION" );
    
    STL_TRY( ztcdSQLAllocHandle( aDbcHandle,
                                 &sStmtHandle,
                                 aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( ztcdSQLPrepare( sStmtHandle,
                             (SQLCHAR*)sSqlString,
                             stlStrlen( sSqlString ),
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLBindCol( sStmtHandle,
                             1,
                             SQL_C_SBIGINT, 
                             &sBeginLsn,
                             STL_SIZEOF( sBeginLsn ),
                             &sIndicator,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLExecute( sStmtHandle, 
                             &sResult, 
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcdSQLFetch( sStmtHandle, 
                           &sResult, 
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult != SQL_NO_DATA, RAMP_ERR_INVALID_DATA );
    
    if( sIndicator != SQL_NULL_DATA )
    {
        *aBeginLsn = sBeginLsn;
    }
    
    sState = 0;
    STL_TRY( ztcdSQLFreeHandle( sStmtHandle, 
                                aErrorStack ) == STL_SUCCESS ); 
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack,
                      "Get MinBeginLsn" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        (void) ztcdSQLFreeHandle( sStmtHandle, 
                                  aErrorStack );
    }

    return STL_FAILURE;   
}

/** @} */
