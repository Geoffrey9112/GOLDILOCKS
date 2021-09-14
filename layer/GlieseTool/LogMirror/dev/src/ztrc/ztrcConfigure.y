/*******************************************************************************
 * ztrcConfigure.y
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

%{

#include <goldilocks.h>
#include <ztr.h>

extern stlInt32     gZtrcConfigureLineNo;
extern ztrConfigure gConfigure;

stlInt32 ztrcConfigureerror( ztrConfigureParam * aParam,
                             void              * aScanner, 
                             const stlChar     * aMsg );

void ztrcConfigure_scan_bytes( stlChar   * aBuffer,
                               stlInt32    aLength,
                               void      * aScanner );

%}

%union {
    stlChar   str_val[1024];
}

%{
stlInt32 ztrcConfigurelex( YYSTYPE *, void * );    
%}

%pure_parser
%parse-param   { ztrConfigureParam  * aParam }
%parse-param   { void               * aScanner }
%lex-param     { yyscan_t           * aScanner }

%token ZTRC_TOK_USER_ID; 
%token ZTRC_TOK_USER_PW;
%token ZTRC_TOK_PORT;
%token ZTRC_TOK_MASTER_IP;
%token ZTRC_TOK_LOG_PATH;
%token ZTRC_TOK_EQUALS;
%token ZTRC_TOK_COMMA;
%token ZTRC_TOK_ERROR;
%token ZTRC_TOK_HOST_IP;
%token ZTRC_TOK_HOST_PORT;
%token ZTRC_TOK_DSN;
%token ZTRC_TOK_IB_DEVICE_NAME;
%token ZTRC_TOK_IB_PORT;

%token <str_val>   ZTRC_TOK_STRING;
%token <str_val>   ZTRC_TOK_NUMBER;

%%

start                   : configure_clauses 
                        ;

configure_clauses       : configure_clauses configure_clause
                        | configure_clause
                        ;

configure_clause        : user_id_clause
                        | user_pw_clause
                        | port_clause
                        | master_ip_clause
                        | log_path_clause
                        | dsn_clause
                        | host_ip_clause
                        | host_port_clause
                        ;
    
user_id_clause          : ZTRC_TOK_USER_ID ZTRC_TOK_EQUALS ZTRC_TOK_STRING
                          {
                              ztrcSetUserId( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                (void) ztrcConfigureerror( aParam, NULL, $3 );
                                YYABORT;
                              }
                          }
                        | ZTRC_TOK_USER_ID ZTRC_TOK_EQUALS ZTRC_TOK_NUMBER
                          {
                              ztrcSetUserId( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                (void) ztrcConfigureerror( aParam, NULL, $3 );
                                YYABORT;
                              }
                          };
    
user_pw_clause          : ZTRC_TOK_USER_PW ZTRC_TOK_EQUALS ZTRC_TOK_STRING
                          {
                              ztrcSetUserPw( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                (void) ztrcConfigureerror( aParam, NULL, $3 );
                                YYABORT;
                              }
                          }
                        | ZTRC_TOK_USER_PW ZTRC_TOK_EQUALS ZTRC_TOK_NUMBER
                          {
                              ztrcSetUserPw( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                (void) ztrcConfigureerror( aParam, NULL, $3 );
                                YYABORT;
                              }
                          };

log_path_clause         : ZTRC_TOK_LOG_PATH ZTRC_TOK_EQUALS ZTRC_TOK_STRING
                          {
                              ztrcSetLogPath( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                (void) ztrcConfigureerror( aParam, NULL, $3 );
                                YYABORT;
                              }
                          };

port_clause             : ZTRC_TOK_PORT ZTRC_TOK_EQUALS ZTRC_TOK_NUMBER
                          {
                              ztrcSetPort( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                (void) ztrcConfigureerror( aParam, NULL, $3 );
                                YYABORT;
                              }
                          };

master_ip_clause        : ZTRC_TOK_MASTER_IP ZTRC_TOK_EQUALS ZTRC_TOK_STRING
                          {
                              ztrcSetMasterIp( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                (void) ztrcConfigureerror( aParam, NULL, $3 );
                                YYABORT;
                              }
                          };

dsn_clause              : ZTRC_TOK_DSN ZTRC_TOK_EQUALS ZTRC_TOK_STRING
                          {
                              ztrcSetDsn( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                (void) ztrcConfigureerror( aParam, NULL, $3 );
                                YYABORT;
                              }
                          }
                        | ZTRC_TOK_DSN ZTRC_TOK_EQUALS ZTRC_TOK_NUMBER
                          {
                              ztrcSetDsn( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                (void) ztrcConfigureerror( aParam, NULL, $3 );
                                YYABORT;
                              }
                          };

host_ip_clause          : ZTRC_TOK_HOST_IP ZTRC_TOK_EQUALS ZTRC_TOK_STRING
                          {
                              ztrcSetHostIp( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                (void) ztrcConfigureerror( aParam, NULL, $3 );
                                YYABORT;
                              }
                          };

host_port_clause        : ZTRC_TOK_HOST_PORT ZTRC_TOK_EQUALS ZTRC_TOK_NUMBER
                          {
                              ztrcSetHostPort( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                (void) ztrcConfigureerror( aParam, NULL, $3 );
                                YYABORT;
                              }
                          };

%%

stlInt32 ztrcConfigureerror( ztrConfigureParam * aParam,
                             void              * aScanner, 
                             const stlChar     * aMsg )
{
    stlInt32   sLineNo   = 1;
    stlChar  * sStartPtr = aParam->mBuffer;
    stlChar  * sEndPtr   = NULL;
    stlChar    sErrMsg[1024] = {0,};
    
    while( sLineNo < gZtrcConfigureLineNo )
    {
        if( *sStartPtr == '\n' )
        {
            sLineNo++;
        }
        sStartPtr++;
    }
    
    sEndPtr = sStartPtr;
    
    while( (*sEndPtr != '\n') && (*sEndPtr != '\0') )
    {
        sEndPtr++;
    }
    
    stlStrnncat( sErrMsg, sStartPtr, 1024, sEndPtr - sStartPtr + 1 );
    
    stlSnprintf( aParam->mErrMsg,
                 STL_SIZEOF( aParam->mErrMsg ),
                 "\n[ %s ] Error at line [ %d ]\n",
                 sErrMsg,
                 gZtrcConfigureLineNo );
    
    return -1;   
}

void ztrcSetUserId( ztrConfigureParam * aParam,
                    stlChar           * aUserId )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztrmSetUserId( aUserId,
                       sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztrcSetUserPw( ztrConfigureParam * aParam,
                    stlChar           * aUserPw )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztrmSetUserPw( aUserPw,
                       sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztrcSetLogPath( ztrConfigureParam * aParam,
                     stlChar           * aLogPath )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztrmSetLogPath( aLogPath,
                        sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztrcSetPort( ztrConfigureParam * aParam,
                  stlChar           * aPort )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztrmSetPort( aPort,
                     sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztrcSetMasterIp( ztrConfigureParam * aParam,
                      stlChar           * aMasterIp )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztrmSetMasterIp( aMasterIp,
                         sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztrcSetDsn( ztrConfigureParam * aParam,
                 stlChar           * aDsn )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztrmSetDsn( aDsn,
                    sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztrcSetHostIp( ztrConfigureParam * aParam,
                    stlChar           * aHostIp )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztrmSetHostIp( aHostIp,
                       sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztrcSetHostPort( ztrConfigureParam * aParam,
                      stlChar           * aPort )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztrmSetHostPort( aPort,
                         sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}


stlStatus ztrcReadConfigure( stlErrorStack * aErrorStack )
{
    stlFile     sFile;
    stlInt32    sRet;
    stlInt32    sState          = 0;
    stlChar   * sReadBuffer     = NULL;
    stlBool     sExist          = STL_FALSE;
    stlOffset   sFileSize       = 0;
    stlOffset   sOffset         = 0;
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    
    ztrConfigureParam sParam;
    
    gZtrcConfigureLineNo = 1;
    
    STL_TRY( ztrmGetConfigureFileName( sFileName, 
                                       aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    sFileSize = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlAllocHeap( (void**)&sReadBuffer,
                           sFileSize + 1,
                           aErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    stlMemset( sReadBuffer, 0x00, sFileSize + 1 );
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlReadFile( &sFile,
                          sReadBuffer,
                          sFileSize,
                          NULL,
                          aErrorStack ) == STL_SUCCESS );
    
    sParam.mBuffer             = sReadBuffer;
    sParam.mErrorStack         = aErrorStack;
    sParam.mErrStatus          = STL_SUCCESS;
    sParam.mPosition           = 0;
    sParam.mLength             = sFileSize + 1;
    stlMemset( sParam.mErrMsg, 0x00, STL_SIZEOF( sParam.mErrMsg ) );

    ztrcConfigurelex_init( &sParam.mYyScanner );
    ztrcConfigureset_extra( &sParam, sParam.mYyScanner );
    sRet = ztrcConfigureparse( &sParam, sParam.mYyScanner );
    ztrcConfigurelex_destroy( sParam.mYyScanner );
    
    STL_TRY_THROW( ( sRet == 0 ) && 
                   ( sParam.mErrStatus == STL_SUCCESS ) , RAMP_ERR_PARSE_FAIL );
    
    sState = 1;
    (void) stlFreeHeap( sReadBuffer );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
                
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_CATCH( RAMP_ERR_PARSE_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_CONFIGURE_FILE_PARSE,
                      sParam.mErrMsg,
                      aErrorStack );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            (void) stlFreeHeap( sReadBuffer );
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break;   
        
    }
    
    return STL_FAILURE;
    
}

