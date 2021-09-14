/*******************************************************************************
 * ztdDisplay.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
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
#include <zllDef.h>
#include <goldilocks.h>
#include <ztdDef.h>
#include <ztdDisplay.h>
#include <ztdFileExecute.h>

stlPaintAttr gZtdErrorPaintAttr;

/**
 * @brief ztdEnableColoredText()는 Error Msg 색을 설정한다..
 * @param [in]  aErrorStack   error stack
 */ 
stlStatus ztdEnableColoredText( stlErrorStack * aErrorStack )
{   
    gZtdErrorPaintAttr.mTextAttr  = STL_PTA_OFF;
    gZtdErrorPaintAttr.mForeColor = STL_PFC_OFF;
    gZtdErrorPaintAttr.mBackColor = STL_PBC_OFF;
    
    return STL_SUCCESS;
}

/**
 * @brief ErrorStack을 화면에 출력.
 * @param [in] aAttr    Text Color Attribute
 * @param [in] aFormat  String and Variable List
 */ 
stlInt32 ztdPaintf( stlPaintAttr * aAttr, const stlChar *aFormat, ... )
{
    stlInt32  sReturn;
    va_list   sVarArgList;
   
    va_start(sVarArgList, aFormat);
    sReturn = stlVpaintf( aAttr, aFormat, sVarArgList );
    va_end(sVarArgList);

    return sReturn;
}

/**
 * @brief ztdPrintErrorStack()는 Error Stack 출력한다.
 * @param [in] aErrorStack  error stack
 */ 
void ztdPrintErrorStack( stlErrorStack * aErrorStack )
{
    stlInt32 i;
    stlInt32 sErrorCode;
    stlInt32 sExtErrCode;
    stlChar  sSqlState[6];

    for( i = aErrorStack->mTop; i >= 0; i-- )
    {

        sErrorCode = aErrorStack->mErrorData[i].mErrorCode;
        sExtErrCode = stlGetExternalErrorCode( &aErrorStack->mErrorData[i] );
        stlMakeSqlState( sExtErrCode, sSqlState );
        
 
        ztdPaintf( &gZtdErrorPaintAttr,
                   "ERR-%s(%d): %s %s\n",
                   sSqlState,
                   sErrorCode,
                   aErrorStack->mErrorData[i].mErrorMessage,
                   aErrorStack->mErrorData[i].mDetailErrorMessage );
        
    }
}

/**
 * @brief ztdPrintDiagnostic()는 Print diagnostic record to console
 * @param [in]     aHandleType      handle type
 * @param [in]     aHandle          handle
 * @param [in/out] aErrorStack      error stack 
 */ 
stlStatus ztdPrintDiagnostic(SQLSMALLINT        aHandleType,
                             SQLHANDLE          aHandle,
                             stlErrorStack    * aErrorStack )
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

        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_UNACCEPTABLE_VALUE );

        sSqlState[5] = '\0';
        
        /*
         * front vendor string 제거
         */
        sRefinedMessageText = ( stlStrstr( (stlChar*)sMessageText,
                                           ZLL_ODBC_COMPONENT_IDENTIFIER ) +
                                stlStrlen( ZLL_ODBC_COMPONENT_IDENTIFIER ));
        /*
         * front data source 제거
         */
        sRefinedMessageText = ( stlStrstr( (stlChar*)sRefinedMessageText,
                                           ZLL_DATA_SOURCE_IDENTIFIER ) +
                                stlStrlen( ZLL_DATA_SOURCE_IDENTIFIER ));
            
        while( stlIsspace( *sRefinedMessageText ) == STL_TRUE )
        {
            sRefinedMessageText++;
        }
      
        stlPrintf("ERR-%s(%d): %s\n", sSqlState, sNaiveError, sRefinedMessageText );

        sRecNumber++;
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_UNACCEPTABLE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_UNACCEPTABLE_VALUE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ztdWriteDiagnostic()는 Write diagnostic Record log string to log file
 * @param [in]     aHandleType           handle type
 * @param [in]     aHandle               handle
 * @param [in]     aIsPrintRecordNumber  record number를 출력할지 여부
 * @param [in]     aRecCount             Total Record Count
 * @param [in/out] aFileandBuffer        file pointer and error msg
 * @param [in/out] aErrorStack           error stack 
 */ 
stlStatus ztdWriteDiagnostic(SQLSMALLINT        aHandleType,
                             SQLHANDLE          aHandle,
                             stlBool            aIsPrintRecordNumber,
                             stlInt64           aRecCount,
                             ztdFileAndBuffer * aFileAndBuffer,
                             stlErrorStack    * aErrorStack )
{
    SQLCHAR       sSqlState[6];
    SQLINTEGER    sNaiveError;
    SQLSMALLINT   sTextLength;
    SQLSMALLINT   sStateLength;
    SQLLEN        sRowNumber = 0;
    SQLINTEGER    sColNumber = 0;
    SQLLEN        sTmpRow = 0;
    SQLSMALLINT   sRecNumber = 1;
    SQLCHAR       sMessageText[SQL_MAX_MESSAGE_LENGTH];
    stlChar     * sRefinedMessageText;
    SQLRETURN     sReturn;
    stlBool       sSessionConn = STL_TRUE; /*Session 종료로 인한 에러 때 해당 process 또는 thread 종료*/

    while( STL_TRUE )
    {
        sReturn = SQLGetDiagField( aHandleType,
                                   aHandle,
                                   sRecNumber,
                                   SQL_DIAG_ROW_NUMBER,
                                   &sRowNumber,
                                   0,
                                   0);
        
        if( sReturn == SQL_NO_DATA )
        {
            break;
        }                               

        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_UNACCEPTABLE_VALUE );

        if( sRowNumber == -1 )
        {
            sRowNumber = sTmpRow;
        }
        else
        {
            sTmpRow = sRowNumber;
        }

        sReturn = SQLGetDiagField( aHandleType,
                                   aHandle,
                                   sRecNumber,
                                   SQL_DIAG_COLUMN_NUMBER,
                                   &sColNumber,
                                   0,
                                   0);
        if( sReturn == SQL_NO_DATA )
        {
            break;
        }                               

        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_UNACCEPTABLE_VALUE );
        
        sReturn = SQLGetDiagField( aHandleType,
                                   aHandle,
                                   sRecNumber,
                                   SQL_DIAG_SQLSTATE,
                                   sSqlState,
                                   6,
                                   &sStateLength);
        if( sReturn == SQL_NO_DATA )
        {
            break;
        }        

        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_UNACCEPTABLE_VALUE );

        if( stlStrcmp((stlChar*)sSqlState, ZTD_CONSTANT_STATE_LINK_FAILURE) == 0 )
        {
            sSessionConn = STL_FALSE;
        }
        
        sReturn = SQLGetDiagField( aHandleType,
                                   aHandle,
                                   sRecNumber,
                                   SQL_DIAG_NATIVE,
                                   &sNaiveError,
                                   0,
                                   0);

        if( sReturn == SQL_NO_DATA )
        {
            break;
        }        

        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_UNACCEPTABLE_VALUE );

        sReturn = SQLGetDiagField( aHandleType,
                                   aHandle,
                                   sRecNumber,
                                   SQL_DIAG_MESSAGE_TEXT,
                                   sMessageText,
                                   SQL_MAX_MESSAGE_LENGTH,
                                   &sTextLength);
        
        if( sReturn == SQL_NO_DATA )
        {
            break;
        }

        STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_UNACCEPTABLE_VALUE );

        sSqlState[5] = '\0';
        
        /*
         * front vendor string 제거
         */
        sRefinedMessageText = ( stlStrstr( (stlChar*)sMessageText,
                                           ZLL_ODBC_COMPONENT_IDENTIFIER ) +
                                stlStrlen( ZLL_ODBC_COMPONENT_IDENTIFIER ));
        /*
         * front data source 제거
         */
        sRefinedMessageText = ( stlStrstr( (stlChar*)sRefinedMessageText,
                                           ZLL_DATA_SOURCE_IDENTIFIER ) +
                                stlStrlen( ZLL_DATA_SOURCE_IDENTIFIER ));
            
        while( stlIsspace( *sRefinedMessageText ) == STL_TRUE )
        {
            sRefinedMessageText++;
        }

        if( aIsPrintRecordNumber == STL_TRUE )
        {
            /**
             * Error Message에 Record Number를 출력한다.
             */ 
            stlSnprintf( aFileAndBuffer->mLogBuffer,
                         ZTD_MAX_LOG_STRING_SIZE,
                         "Err Rec(%ld) Col(%d): %s(%d): %s",
                         aRecCount + sRowNumber,
                         sColNumber,
                         sSqlState,
                         sNaiveError,
                         sRefinedMessageText );
        }
        else
        {
            /**
             * Error Message에 Record Number를 출력하지 않는다.
             */
            stlSnprintf( aFileAndBuffer->mLogBuffer,
                         ZTD_MAX_LOG_STRING_SIZE,
                         "Err Col(%d): %s(%d): %s",
                         sColNumber,
                         sSqlState,
                         sNaiveError,
                         sRefinedMessageText );
        }
        
        STL_TRY( ztdWriteLog( aFileAndBuffer, aErrorStack )
                 == STL_SUCCESS );

        sRecNumber++;
    }
    
    STL_TRY_THROW( sSessionConn == STL_TRUE, RAMP_ERR_SESSION_CONNECTION );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_UNACCEPTABLE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_UNACCEPTABLE_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SESSION_CONNECTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief SQLExecute를 수행하고 Error 원인을 얻는다.
 * @param [in]     aEnvHandle     SQL Environment handle
 * @param [in]     aDbcHandle     SQL DBC handle
 * @param [in]     aStmHandle     SQL Statement handle
 * @param [in]     aRecCount      Total Record Count
 * @param [in/out] aFileAndBuffer FILE pointer and Log String
 * @param [in/out] aErrorStack    error stack
 */ 
stlStatus ztdCheckError( SQLHENV            aEnvHandle,
                         SQLHDBC            aDbcHandle,
                         SQLHSTMT           aStmHandle,
                         stlInt64           aRecCount,
                         ztdFileAndBuffer * aFileAndBuffer,
                         stlSemaphore     * aSemaphore,
                         stlErrorStack    * aErrorStack )                         
                         
{
    stlInt32 sState = 0;
    if( aSemaphore != NULL )
    {
        STL_TRY( stlAcquireSemaphore( aSemaphore, aErrorStack )
                 == STL_SUCCESS );
        sState = 1;
    }
    
    STL_TRY( ztdWriteDiagnostic( SQL_HANDLE_STMT,
                                 aStmHandle,
                                 STL_TRUE,  /* Is Printf Record Number */
                                 aRecCount,
                                 aFileAndBuffer,
                                 aErrorStack )
             == STL_SUCCESS );
            
    STL_TRY( ztdWriteDiagnostic( SQL_HANDLE_DBC,
                                 aDbcHandle,
                                 STL_TRUE,  /* Is Printf Record Number */
                                 aRecCount,
                                 aFileAndBuffer,
                                 aErrorStack )
             == STL_SUCCESS );    
            
    STL_TRY( ztdWriteDiagnostic( SQL_HANDLE_ENV,
                                 aEnvHandle,
                                 STL_TRUE,  /* Is Printf Record Number */
                                 aRecCount,
                                 aFileAndBuffer,
                                 aErrorStack )
             == STL_SUCCESS );    

    if( aSemaphore != NULL )
    {
        STL_TRY( stlReleaseSemaphore( aSemaphore, aErrorStack )
                 == STL_SUCCESS );
        sState = 0;
    }
    return STL_SUCCESS;

    STL_FINISH;

    /**
     * AcquireSemaphore 성공 후 ztdWriteDiagnostic 에서 STL_FAILURE 반환한다면
     * ReleaseSemaphore를 하여 Dead Lock를 생기지 않게 해야 한다.
     */
    if( aSemaphore != NULL )
    {
        switch( sState )
        {
            case 1:
                (void) stlReleaseSemaphore( aSemaphore, aErrorStack );
            default:
                break;
        }
    }
    return STL_FAILURE;
}
