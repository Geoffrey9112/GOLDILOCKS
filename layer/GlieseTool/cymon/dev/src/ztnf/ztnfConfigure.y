/*******************************************************************************
 * ztnConfigure.y
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
#include <ztn.h>

extern stlInt32     gZtnConfigureLineNo;
stlBool             gZtnBraceStarted   = STL_FALSE;
stlBool             gZtnParseMinInfo   = STL_TRUE;

stlInt32 ztnfConfigureerror( ztnConfigureParam * aParam,
                             void              * aScanner, 
                             const stlChar     * aMsg );

void ztnfConfigure_scan_bytes( stlChar   * aBuffer,
                               stlInt32    aLength,
                               void      * aScanner );

%}

%union {
    stlChar   str_val[1024];
}

%{
stlInt32 ztnfConfigurelex( YYSTYPE *, void * );    
%}

%pure_parser
%parse-param   { ztnConfigureParam  * aParam }
%parse-param   { void               * aScanner }
%lex-param     { yyscan_t           * aScanner }

%token ZTN_TOK_CAPTURE_TABLE;
%token ZTN_TOK_APPLY_TABLE;
%token ZTN_TOK_TO;
%token ZTN_TOK_GROUP_NAME;
%token ZTN_TOK_USER_ID; 
%token ZTN_TOK_USER_PW;
%token ZTN_TOK_USER_ENCRYPT_PW;
%token ZTN_TOK_PORT;
%token ZTN_TOK_DSN;
%token ZTN_TOK_HOST_IP;
%token ZTN_TOK_HOST_PORT;
%token ZTN_TOK_MASTER_IP;
%token ZTN_TOK_TRANS_SORT_AREA_SIZE;
%token ZTN_TOK_TRANS_FILE_PATH;
%token ZTN_TOK_APPLY_COMMIT_SIZE;
%token ZTN_TOK_APPLY_ARRAY_SIZE;
%token ZTN_TOK_APPLIER_COUNT;
%token ZTN_TOK_READ_LOG_BLOCK_COUNT;
%token ZTN_TOK_COMM_CHUNK_COUNT;
%token ZTN_TOK_PROPAGATE_MODE;
%token ZTN_TOK_LOG_PATH;
%token ZTN_TOK_GIVEUP_INTERVAL;
%token ZTN_TOK_GOLDILOCKS_LISTEN_PORT;
%token ZTN_TOK_SYNCHER_COUNT;
%token ZTN_TOK_SYNC_ARRAY_SIZE;
%token ZTN_TOK_LIB_CONFIG;
%token ZTN_TOK_OPEN_BRACKET;
%token ZTN_TOK_CLOSE_BRACKET;
%token ZTN_TOK_OPEN_BRACE;
%token ZTN_TOK_CLOSE_BRACE;
%token ZTN_TOK_EQUALS;
%token ZTN_TOK_ERROR;
%token ZTN_TOK_COMMA;
%token ZTN_TOK_DOT;

%token <str_val>   ZTN_TOK_STRING;
%token <str_val>   ZTN_TOK_NUMBER;

%%

start                   : configure_clauses 
                        ;

configure_clauses       : configure_clauses configure_clause
                        | configure_clause
                        ;

configure_clause        : capture_table_clause
                        | group_name_start_clause
                        | group_name_close_clause
                        | user_id_clause
                        | user_pw_clause
                        | user_encrypt_pw_clause
                        | port_clause
                        | dsn_clause
                        | host_ip_clause
                        | host_port_clause
                        | trans_sort_clause
                        | read_log_block_clause
                        | comm_chunk_clause
                        | log_path_clause
                        | giveup_interval_clause
                        | goldilocks_listen_port_clause
                        | syncher_count_clause
                        | sync_array_clause
                        | lib_config_clause
                        ;
    
capture_table_clause    : ZTN_TOK_CAPTURE_TABLE ZTN_TOK_EQUALS ZTN_TOK_OPEN_BRACKET tables_clause ZTN_TOK_CLOSE_BRACKET
                        | ZTN_TOK_CAPTURE_TABLE ZTN_TOK_EQUALS table_clause 
                        ;

tables_clause           : tables_clause ZTN_TOK_COMMA table_clause
                        | table_clause
                        ;
                          
table_clause            : ZTN_TOK_STRING
                          {
                              
                          }
                        | ZTN_TOK_STRING ZTN_TOK_DOT ZTN_TOK_STRING
                          {
                            
                          };

trans_sort_clause       : ZTN_TOK_TRANS_SORT_AREA_SIZE ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                          {
                              
                          };

read_log_block_clause   : ZTN_TOK_READ_LOG_BLOCK_COUNT ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                          {
                              
                          };

comm_chunk_clause       : ZTN_TOK_COMM_CHUNK_COUNT ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                          {
                              
                          };

log_path_clause         : ZTN_TOK_LOG_PATH ZTN_TOK_EQUALS ZTN_TOK_STRING
                          {
                              
                          };

giveup_interval_clause  : ZTN_TOK_GIVEUP_INTERVAL ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                          {
                              
                          };

goldilocks_listen_port_clause : ZTN_TOK_GOLDILOCKS_LISTEN_PORT ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                           {
    
                           };

syncher_count_clause    : ZTN_TOK_SYNCHER_COUNT ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                          {
    
                          };

sync_array_clause       : ZTN_TOK_SYNC_ARRAY_SIZE ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                          {
    
                          };
                          
lib_config_clause        : ZTN_TOK_LIB_CONFIG ZTN_TOK_EQUALS ZTN_TOK_STRING
                           {

                           };


group_name_start_clause : ZTN_TOK_GROUP_NAME ZTN_TOK_EQUALS ZTN_TOK_STRING ZTN_TOK_OPEN_BRACE
                          {
                              gZtnBraceStarted = STL_TRUE;
                              
                              ztnfAddGroupItem( aParam, $3 );
                              
                              if ( aParam->mErrStatus != STL_SUCCESS )
                              {
                                  (void) ztnfConfigureerror( aParam, NULL, $3 );
                                  YYABORT;
                              }
                          };

group_name_close_clause : ZTN_TOK_CLOSE_BRACE
                          { 
                              gZtnBraceStarted = STL_FALSE;
                          };

user_id_clause          : ZTN_TOK_USER_ID ZTN_TOK_EQUALS ZTN_TOK_STRING
                          {
                              if( gZtnBraceStarted == STL_FALSE )
                              {
                                  ztnfSetUserIdToGlobalArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                              else
                              {
                                  ztnfSetUserIdToGroupArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          }
                        | ZTN_TOK_USER_ID ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                          {
                              if( gZtnBraceStarted == STL_FALSE )
                              {
                                  ztnfSetUserIdToGlobalArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                              else
                              {
                                  ztnfSetUserIdToGroupArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };
    
user_pw_clause          : ZTN_TOK_USER_PW ZTN_TOK_EQUALS ZTN_TOK_STRING
                          {
                              if( gZtnBraceStarted == STL_FALSE )
                              {
                                  ztnfSetUserPwToGlobalArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                              else
                              {
                                  ztnfSetUserPwToGroupArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          }
                        | ZTN_TOK_USER_PW ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                          {
                              if( gZtnBraceStarted == STL_FALSE )
                              {
                                  ztnfSetUserPwToGlobalArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                              else
                              {
                                  ztnfSetUserPwToGroupArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

user_encrypt_pw_clause  : ZTN_TOK_USER_ENCRYPT_PW ZTN_TOK_EQUALS ZTN_TOK_STRING
                          {
                              if( gZtnBraceStarted == STL_FALSE )
                              {
                                  ztnfSetUserEncryptPwToGlobalArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                              else
                              {
                                  ztnfSetUserEncryptPwToGroupArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          }
                        | ZTN_TOK_USER_ENCRYPT_PW ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                          {
                              if( gZtnBraceStarted == STL_FALSE )
                              {
                                  ztnfSetUserEncryptPwToGlobalArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                              else
                              {
                                  ztnfSetUserEncryptPwToGroupArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };


port_clause             : ZTN_TOK_PORT ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                          {
                              if( gZtnBraceStarted == STL_TRUE )
                              {
                                  ztnfSetPortToGroupArea( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

dsn_clause              : ZTN_TOK_DSN ZTN_TOK_EQUALS ZTN_TOK_STRING
                          {
                              if( gZtnBraceStarted == STL_FALSE )
                              {
                                  ztnfSetDsnToGlobalArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                              else
                              {
                                  ztnfSetDsnToGroupArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

host_ip_clause          : ZTN_TOK_HOST_IP ZTN_TOK_EQUALS ZTN_TOK_STRING
                          {
                              if( gZtnBraceStarted == STL_FALSE )
                              {
                                  ztnfSetHostIpToGlobalArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                              else
                              {
                                  ztnfSetHostIpToGroupArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

host_port_clause        : ZTN_TOK_HOST_PORT ZTN_TOK_EQUALS ZTN_TOK_NUMBER
                          {
                              if( gZtnBraceStarted == STL_FALSE )
                              {
                                  ztnfSetHostPortToGlobalArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                              else
                              {
                                  ztnfSetHostPortToGroupArea( aParam, $3 );
                          
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztnfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

%%
            
                                  
stlInt32 ztnfConfigureerror( ztnConfigureParam * aParam,
                             void              * aScanner, 
                             const stlChar     * aMsg )
{
    stlInt32   sLineNo   = 1;
    stlChar  * sStartPtr = aParam->mBuffer;
    stlChar  * sEndPtr   = NULL;
    stlChar    sErrMsg[1024] = {0,};
    
    while( sLineNo < gZtnConfigureLineNo )
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
                 gZtnConfigureLineNo );
    
    return -1;   
}


stlStatus ztnfReadConfigure( stlErrorStack * aErrorStack )
{
    stlFile     sFile;
    stlInt32    sRet;
    stlInt32    sState          = 0;
    stlChar   * sReadBuffer     = NULL;
    stlBool     sExist          = STL_FALSE;
    stlOffset   sFileSize       = 0;
    stlOffset   sOffset         = 0;
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    
    ztnConfigureParam sParam;
    
    gZtnConfigureLineNo = 1;
    
    STL_TRY( ztnmGetConfigureFileName( sFileName, 
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

    ztnfConfigurelex_init( &sParam.mYyScanner );
    ztnfConfigureset_extra( &sParam, sParam.mYyScanner );
    sRet = ztnfConfigureparse( &sParam, sParam.mYyScanner );
    ztnfConfigurelex_destroy( sParam.mYyScanner );

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
                      ZTN_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_CATCH( RAMP_ERR_PARSE_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_CONFIGURE_FILE_PARSE,
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

void ztnfAddGroupItem( ztnConfigureParam * aParam,
                       stlChar           * aGroupName )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmAddGroupItem( aGroupName,
                          sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztnfSetUserIdToGlobalArea( ztnConfigureParam * aParam, 
                                stlChar           * aUserId )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetUserIdToGlobalArea( aUserId,
                                   sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztnfSetUserIdToGroupArea( ztnConfigureParam * aParam, 
                               stlChar           * aUserId )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetUserIdToGroupArea( aUserId,
                                  sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztnfSetUserPwToGlobalArea( ztnConfigureParam * aParam, 
                                stlChar           * aUserPw )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetUserPwToGlobalArea( aUserPw,
                                   sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztnfSetUserPwToGroupArea( ztnConfigureParam * aParam, 
                               stlChar           * aUserPw )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetUserPwToGroupArea( aUserPw,
                                  sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztnfSetUserEncryptPwToGlobalArea( ztnConfigureParam * aParam, 
                                       stlChar           * aUserPw )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetUserEncryptPwToGlobalArea( aUserPw,
                                          sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztnfSetUserEncryptPwToGroupArea( ztnConfigureParam * aParam, 
                                      stlChar           * aUserPw )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetUserEncryptPwToGroupArea( aUserPw,
                                         sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztnfSetPortToGroupArea( ztnConfigureParam * aParam, 
                             stlChar           * aPort )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetPort( aPort,
                     sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
    }
}

void ztnfSetDsnToGlobalArea( ztnConfigureParam * aParam, 
                             stlChar           * aDsn )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetDsnToGlobalArea( aDsn,
                                sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
    }
}

void ztnfSetDsnToGroupArea( ztnConfigureParam * aParam, 
                            stlChar           * aDsn )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetDsnToGroupArea( aDsn,
                               sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
    }
}

void ztnfSetHostIpToGlobalArea( ztnConfigureParam * aParam, 
                                stlChar           * aHostIp )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetHostIpToGlobalArea( aHostIp,
                                   sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
    }
}

void ztnfSetHostIpToGroupArea( ztnConfigureParam * aParam, 
                               stlChar           * aHostIp )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetHostIpToGroupArea( aHostIp,
                                  sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
    }
}

void ztnfSetHostPortToGlobalArea( ztnConfigureParam * aParam, 
                                  stlChar           * aPort )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetHostPortToGlobalArea( aPort,
                                     sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
    }
}

void ztnfSetHostPortToGroupArea( ztnConfigureParam * aParam, 
                                 stlChar           * aPort )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztnmSetHostPortToGroupArea( aPort,
                                    sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
    }
}

