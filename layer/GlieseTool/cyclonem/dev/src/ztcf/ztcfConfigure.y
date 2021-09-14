/*******************************************************************************
 * ztcfConfigure.y
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
#include <ztc.h>

extern stlInt32     gZtcfConfigureLineNo;
extern ztcConfigure gConfigure;
stlBool             gZtcfBraceStarted   = STL_FALSE;
stlBool             gZtcfParseMinInfo   = STL_TRUE;
stlBool             gZtcfGroupMatched   = STL_FALSE;

stlInt32 ztcfConfigureerror( ztcConfigureParam * aParam,
                             void              * aScanner, 
                             const stlChar     * aMsg );

void ztcfConfigure_scan_bytes( stlChar   * aBuffer,
                               stlInt32    aLength,
                               void      * aScanner );

%}

%union {
    stlChar   str_val[1024];
}

%{
stlInt32 ztcfConfigurelex( YYSTYPE *, void * );    
%}

%pure_parser
%parse-param   { ztcConfigureParam  * aParam }
%parse-param   { void               * aScanner }
%lex-param     { yyscan_t           * aScanner }

%token ZTCF_TOK_CAPTURE_TABLE;
%token ZTCF_TOK_APPLY_TABLE;
%token ZTCF_TOK_TO;
%token ZTCF_TOK_GROUP_NAME;
%token ZTCF_TOK_USER_ID; 
%token ZTCF_TOK_USER_PW;
%token ZTCF_TOK_USER_ENCRYPT_PW;
%token ZTCF_TOK_PORT;
%token ZTCF_TOK_DSN;
%token ZTCF_TOK_HOST_IP;
%token ZTCF_TOK_HOST_PORT;
%token ZTCF_TOK_MASTER_IP;
%token ZTCF_TOK_TRANS_SORT_AREA_SIZE;
%token ZTCF_TOK_TRANS_FILE_PATH;
%token ZTCF_TOK_APPLY_COMMIT_SIZE;
%token ZTCF_TOK_APPLY_ARRAY_SIZE;
%token ZTCF_TOK_APPLIER_COUNT;
%token ZTCF_TOK_READ_LOG_BLOCK_COUNT;
%token ZTCF_TOK_COMM_CHUNK_COUNT;
%token ZTCF_TOK_PROPAGATE_MODE;
%token ZTCF_TOK_LOG_PATH;
%token ZTCF_TOK_GIVEUP_INTERVAL;
%token ZTCF_TOK_GOLDILOCKS_LISTEN_PORT;
%token ZTCF_TOK_SYNCHER_COUNT;
%token ZTCF_TOK_SYNC_ARRAY_SIZE;
%token ZTCF_TOK_UPDATE_APPLY_MODE
%token ZTCF_TOK_LIB_CONFIG;
%token ZTCF_TOK_SQL_FILE_NAME;
%token ZTCF_TOK_SQL_FILE_SIZE;
%token ZTCF_TOK_MONITOR_FILE;
%token ZTCF_TOK_MONITOR_CYCLE_MS;
%token ZTCF_TOK_OPEN_BRACKET;
%token ZTCF_TOK_CLOSE_BRACKET;
%token ZTCF_TOK_OPEN_BRACE;
%token ZTCF_TOK_CLOSE_BRACE;
%token ZTCF_TOK_EQUALS;
%token ZTCF_TOK_ERROR;
%token ZTCF_TOK_COMMA;
%token ZTCF_TOK_DOT;

%token <str_val>   ZTCF_TOK_STRING;
%token <str_val>   ZTCF_TOK_NUMBER;

%%

start                   : configure_clauses 
                        ;

configure_clauses       : configure_clauses configure_clause
                        | configure_clause
                        ;

configure_clause        : capture_table_clause
                        | apply_table_clause
                        | group_name_start_clause
                        | group_name_close_clause
                        | user_id_clause
                        | user_pw_clause
                        | user_encrypt_pw_clause
                        | port_clause
                        | dsn_clause
                        | host_ip_clause
                        | host_port_clause
                        | master_ip_clause
                        | trans_sort_clause
                        | trans_filePath_clause
                        | apply_commit_clause
                        | apply_array_clause
                        | applier_count_clause
                        | read_log_block_clause
                        | comm_chunk_clause
                        | propagate_mode_clause
                        | log_path_clause
                        | giveup_interval_clause
                        | goldilocks_listen_port_clause
                        | syncher_count_clause
                        | sync_array_clause
                        | update_apply_mode_clause
                        | lib_config_clause
                        | sql_file_name_clause
                        | sql_file_size_clause
                        | monitor_file_clause
                        | monitor_cycle_ms_clause
                        ;
    
capture_table_clause    : ZTCF_TOK_CAPTURE_TABLE ZTCF_TOK_EQUALS ZTCF_TOK_OPEN_BRACKET tables_clause ZTCF_TOK_CLOSE_BRACKET
                        | ZTCF_TOK_CAPTURE_TABLE ZTCF_TOK_EQUALS table_clause 
                        ;

tables_clause           : tables_clause ZTCF_TOK_COMMA table_clause
                        | table_clause
                        ;
                          
table_clause            : ZTCF_TOK_STRING
                          {
                              if ( gZtcfBraceStarted != STL_TRUE )
                              {
                                  (void) ztcfConfigureerror( aParam, NULL, $1 );
                                  YYABORT;
                              }
                              
                              if ( gZtcfParseMinInfo != STL_TRUE )
                              {
                                  if( gZtcfGroupMatched == STL_TRUE )
                                  {
                                      ztcfAddTableInfo( aParam, NULL, $1 );
                              
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $1 );
                                          YYABORT;
                                      }
                                  }
                              }
                          }
                        | ZTCF_TOK_STRING ZTCF_TOK_DOT ZTCF_TOK_STRING
                          {
                              if ( gZtcfBraceStarted != STL_TRUE )
                              {
                                  (void) ztcfConfigureerror( aParam, NULL, $3 );
                                  YYABORT;
                              }
                              
                              if ( gZtcfParseMinInfo != STL_TRUE )
                              {
                                  if( gZtcfGroupMatched == STL_TRUE )
                                  {
                                      ztcfAddTableInfo( aParam, $1, $3 );
                              
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                              }
                          };

apply_table_clause      : ZTCF_TOK_APPLY_TABLE ZTCF_TOK_EQUALS ZTCF_TOK_OPEN_BRACKET applies_clause ZTCF_TOK_CLOSE_BRACKET
                        | ZTCF_TOK_APPLY_TABLE ZTCF_TOK_EQUALS apply_clause
                        ;

applies_clause          : applies_clause ZTCF_TOK_COMMA apply_clause
                        | apply_clause
                        ;
                                      
apply_clause            : ZTCF_TOK_STRING ZTCF_TOK_TO ZTCF_TOK_STRING
                          {
                              if ( gZtcfBraceStarted != STL_TRUE )
                              {
                                  (void) ztcfConfigureerror( aParam, NULL, $3 );
                                  YYABORT;
                              }
                              
                              if ( gZtcfParseMinInfo != STL_TRUE )
                              {
                                  if( gZtcfGroupMatched == STL_TRUE )
                                  {
                                      ztcfAddApplyInfo( aParam, NULL, $1, NULL, $3 );
                              
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                              }
                          }
                        | ZTCF_TOK_STRING ZTCF_TOK_DOT ZTCF_TOK_STRING ZTCF_TOK_TO ZTCF_TOK_STRING
                          {
                              if ( gZtcfBraceStarted != STL_TRUE )
                              {
                                  (void) ztcfConfigureerror( aParam, NULL, $5 );
                                  YYABORT;
                              }
                              
                              if ( gZtcfParseMinInfo != STL_TRUE )
                              {
                                  if( gZtcfGroupMatched == STL_TRUE )
                                  {
                                      ztcfAddApplyInfo( aParam, $1, $3, NULL, $5 );
                              
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $5 );
                                          YYABORT;
                                      }
                                  }
                              }
                          }
                        | ZTCF_TOK_STRING ZTCF_TOK_TO ZTCF_TOK_STRING ZTCF_TOK_DOT ZTCF_TOK_STRING 
                          {
                              if ( gZtcfBraceStarted != STL_TRUE )
                              {
                                  (void) ztcfConfigureerror( aParam, NULL, $5 );
                                  YYABORT;
                              }
                              
                              if ( gZtcfParseMinInfo != STL_TRUE )
                              {
                                  if( gZtcfGroupMatched == STL_TRUE )
                                  {
                                      ztcfAddApplyInfo( aParam, NULL, $1, $3, $5 );
                              
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $5 );
                                          YYABORT;
                                      }
                                  }
                              }
                          }
                        | ZTCF_TOK_STRING ZTCF_TOK_DOT ZTCF_TOK_STRING ZTCF_TOK_TO ZTCF_TOK_STRING ZTCF_TOK_DOT ZTCF_TOK_STRING 
                          {
                              if ( gZtcfBraceStarted != STL_TRUE )
                              {
                                  (void) ztcfConfigureerror( aParam, NULL, $7 );
                                  YYABORT;
                              }
                              
                              if ( gZtcfParseMinInfo != STL_TRUE )
                              {
                                  if( gZtcfGroupMatched == STL_TRUE )
                                  {
                                      ztcfAddApplyInfo( aParam, $1, $3, $5, $7 );
                              
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $7 );
                                          YYABORT;
                                      }
                                  }
                              }
                          };


group_name_start_clause : ZTCF_TOK_GROUP_NAME ZTCF_TOK_EQUALS ZTCF_TOK_STRING ZTCF_TOK_OPEN_BRACE
                          {
                              gZtcfBraceStarted = STL_TRUE;
                              
                              if ( gZtcfParseMinInfo == STL_TRUE )
                              {
                                  if( stlStrlen( gConfigure.mSetGroupName ) == 0 )
                                  {
                                      // 특정 그룹을 선택하지 않고 실행했을 경우
                                      ztcfAddGroupItem( aParam, $3 );
                              
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                                  else
                                  {
                                      // 특정 그룹을 선택하고 실행했을 경우. 선택한 그룹만 처리하도록 한다.
                                      if( stlStrcasecmp( gConfigure.mSetGroupName, $3 ) == 0 )
                                      {
                                          ztcfAddGroupItem( aParam, $3 );
                              
                                          if ( aParam->mErrStatus != STL_SUCCESS )
                                          {
                                              (void) ztcfConfigureerror( aParam, NULL, $3 );
                                              YYABORT;
                                          }
                                          gZtcfGroupMatched = STL_TRUE;
                                      }
                                  }
                              }
                              else
                              {
                                  if ( stlStrcasecmp( $3, gConfigure.mCurrentGroupPtr->mName ) == 0 )
                                  {
                                      gZtcfGroupMatched = STL_TRUE;
                                  }
                                  else
                                  {
                                      gZtcfGroupMatched = STL_FALSE;
                                  }
                              }
                          };

group_name_close_clause : ZTCF_TOK_CLOSE_BRACE
                          { 
                              gZtcfBraceStarted = STL_FALSE;
                              gZtcfGroupMatched = STL_FALSE;
                          };

user_id_clause          : ZTCF_TOK_USER_ID ZTCF_TOK_EQUALS ZTCF_TOK_STRING
                          {
                              if( gZtcfParseMinInfo == STL_TRUE )
                              {
                                  if( gZtcfBraceStarted == STL_FALSE )
                                  {
                                      ztcfSetUserIdToGlobalArea( aParam, $3 );
                          
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                                  else
                                  {
                                      if( stlStrlen( gConfigure.mSetGroupName ) == 0 )
                                      {
                                          ztcfSetUserIdToGroupArea( aParam, $3 );
                          
                                          if( aParam->mErrStatus != STL_SUCCESS )
                                          {
                                              (void) ztcfConfigureerror( aParam, NULL, $3 );
                                              YYABORT;
                                          }
                                      }
                                      else
                                      {
                                          if( gZtcfGroupMatched == STL_TRUE )
                                          {
                                              ztcfSetUserIdToGroupArea( aParam, $3 );
                          
                                              if( aParam->mErrStatus != STL_SUCCESS )
                                              {
                                                  (void) ztcfConfigureerror( aParam, NULL, $3 );
                                                  YYABORT;
                                              }
                                          }
                                      }
                                  }
                              }
                          }
                        | ZTCF_TOK_USER_ID ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if( gZtcfParseMinInfo == STL_TRUE )
                              {
                                  if( gZtcfBraceStarted == STL_FALSE )
                                  {
                                      ztcfSetUserIdToGlobalArea( aParam, $3 );
                          
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                                  else
                                  {
                                      if( stlStrlen( gConfigure.mSetGroupName ) == 0 )
                                      {
                                          ztcfSetUserIdToGroupArea( aParam, $3 );
                          
                                          if( aParam->mErrStatus != STL_SUCCESS )
                                          {
                                              (void) ztcfConfigureerror( aParam, NULL, $3 );
                                              YYABORT;
                                          }
                                      }
                                      else
                                      {
                                          if( gZtcfGroupMatched == STL_TRUE )
                                          {
                                              ztcfSetUserIdToGroupArea( aParam, $3 );
                          
                                              if( aParam->mErrStatus != STL_SUCCESS )
                                              {
                                                  (void) ztcfConfigureerror( aParam, NULL, $3 );
                                                  YYABORT;
                                              }
                                          }
                                      }
                                  }
                              }
                          };
    
user_pw_clause          : ZTCF_TOK_USER_PW ZTCF_TOK_EQUALS ZTCF_TOK_STRING
                          {
                              if( gZtcfParseMinInfo == STL_TRUE )
                              {
                                  if( gZtcfBraceStarted == STL_FALSE )
                                  {
                                      ztcfSetUserPwToGlobalArea( aParam, $3 );
                          
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                                  else
                                  {
                                      if( stlStrlen( gConfigure.mSetGroupName ) == 0 )
                                      {
                                          ztcfSetUserPwToGroupArea( aParam, $3 );
                          
                                          if( aParam->mErrStatus != STL_SUCCESS )
                                          {
                                              (void) ztcfConfigureerror( aParam, NULL, $3 );
                                              YYABORT;
                                          }
                                      }
                                      else
                                      {
                                          if( gZtcfGroupMatched == STL_TRUE )
                                          {
                                              ztcfSetUserPwToGroupArea( aParam, $3 );
                          
                                              if( aParam->mErrStatus != STL_SUCCESS )
                                              {
                                                  (void) ztcfConfigureerror( aParam, NULL, $3 );
                                                  YYABORT;
                                              }
                                          }
                                      }
                                  }
                              }
                          }
                        | ZTCF_TOK_USER_PW ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if( gZtcfParseMinInfo == STL_TRUE )
                              {
                                  if( gZtcfBraceStarted == STL_FALSE )
                                  {
                                      ztcfSetUserPwToGlobalArea( aParam, $3 );
                          
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                                  else
                                  {
                                      if( stlStrlen( gConfigure.mSetGroupName ) == 0 )
                                      {
                                          ztcfSetUserPwToGroupArea( aParam, $3 );
                          
                                          if( aParam->mErrStatus != STL_SUCCESS )
                                          {
                                              (void) ztcfConfigureerror( aParam, NULL, $3 );
                                              YYABORT;
                                          }
                                      }
                                      else
                                      {
                                          if( gZtcfGroupMatched == STL_TRUE )
                                          {
                                              ztcfSetUserPwToGroupArea( aParam, $3 );
                          
                                              if( aParam->mErrStatus != STL_SUCCESS )
                                              {
                                                  (void) ztcfConfigureerror( aParam, NULL, $3 );
                                                  YYABORT;
                                              }
                                          }
                                      }
                                  }
                              }
                          };
                          
user_encrypt_pw_clause  : ZTCF_TOK_USER_ENCRYPT_PW ZTCF_TOK_EQUALS ZTCF_TOK_STRING
                          {
                              if( gZtcfParseMinInfo == STL_TRUE )
                              {
                                  if( gZtcfBraceStarted == STL_FALSE )
                                  {
                                      ztcfSetUserEncryptPwToGlobalArea( aParam, $3 );
                          
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                                  else
                                  {
                                      if( stlStrlen( gConfigure.mSetGroupName ) == 0 )
                                      {
                                          ztcfSetUserEncryptPwToGroupArea( aParam, $3 );
                          
                                          if( aParam->mErrStatus != STL_SUCCESS )
                                          {
                                              (void) ztcfConfigureerror( aParam, NULL, $3 );
                                              YYABORT;
                                          }
                                      }
                                      else
                                      {
                                          if( gZtcfGroupMatched == STL_TRUE )
                                          {
                                              ztcfSetUserEncryptPwToGroupArea( aParam, $3 );
                          
                                              if( aParam->mErrStatus != STL_SUCCESS )
                                              {
                                                  (void) ztcfConfigureerror( aParam, NULL, $3 );
                                                  YYABORT;
                                              }
                                          }
                                      }
                                  }
                              }
                          }
                        | ZTCF_TOK_USER_ENCRYPT_PW ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if( gZtcfParseMinInfo == STL_TRUE )
                              {
                                  if( gZtcfBraceStarted == STL_FALSE )
                                  {
                                      ztcfSetUserEncryptPwToGlobalArea( aParam, $3 );
                          
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                                  else
                                  {
                                      if( stlStrlen( gConfigure.mSetGroupName ) == 0 )
                                      {
                                          ztcfSetUserEncryptPwToGroupArea( aParam, $3 );
                          
                                          if( aParam->mErrStatus != STL_SUCCESS )
                                          {
                                              (void) ztcfConfigureerror( aParam, NULL, $3 );
                                              YYABORT;
                                          }
                                      }
                                      else
                                      {
                                          if( gZtcfGroupMatched == STL_TRUE )
                                          {
                                              ztcfSetUserEncryptPwToGroupArea( aParam, $3 );
                          
                                              if( aParam->mErrStatus != STL_SUCCESS )
                                              {
                                                  (void) ztcfConfigureerror( aParam, NULL, $3 );
                                                  YYABORT;
                                              }
                                          }
                                      }
                                  }
                              }
                          };

port_clause             : ZTCF_TOK_PORT ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE )
                              {
                                  ztcfSetPort( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

dsn_clause              : ZTCF_TOK_DSN ZTCF_TOK_EQUALS ZTCF_TOK_STRING
                          {
                              if( gZtcfParseMinInfo == STL_TRUE )
                              {
                                  if( gZtcfBraceStarted == STL_FALSE )
                                  {
                                      ztcfSetDsnToGlobalArea( aParam, $3 );
                          
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                                  else
                                  {
                                      if( stlStrlen( gConfigure.mSetGroupName ) == 0 )
                                      {
                                          ztcfSetDsnToGroupArea( aParam, $3 );
                          
                                          if( aParam->mErrStatus != STL_SUCCESS )
                                          {
                                              (void) ztcfConfigureerror( aParam, NULL, $3 );
                                              YYABORT;
                                          }
                                      }
                                      else
                                      {
                                          if( gZtcfGroupMatched == STL_TRUE )
                                          {
                                              ztcfSetDsnToGroupArea( aParam, $3 );
                          
                                              if( aParam->mErrStatus != STL_SUCCESS )
                                              {
                                                  (void) ztcfConfigureerror( aParam, NULL, $3 );
                                                  YYABORT;
                                              }
                                          }
                                      }
                                  }
                              }
                          };

host_ip_clause          : ZTCF_TOK_HOST_IP ZTCF_TOK_EQUALS ZTCF_TOK_STRING
                          {
                              if( gZtcfParseMinInfo == STL_TRUE )
                              {
                                  if( gZtcfBraceStarted == STL_FALSE )
                                  {
                                      ztcfSetHostIpToGlobalArea( aParam, $3 );
                          
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                                  else
                                  {
                                      if( stlStrlen( gConfigure.mSetGroupName ) == 0 )
                                      {
                                          ztcfSetHostIpToGroupArea( aParam, $3 );
                          
                                          if( aParam->mErrStatus != STL_SUCCESS )
                                          {
                                              (void) ztcfConfigureerror( aParam, NULL, $3 );
                                              YYABORT;
                                          }
                                      }
                                      else
                                      {
                                          if( gZtcfGroupMatched == STL_TRUE )
                                          {
                                              ztcfSetHostIpToGroupArea( aParam, $3 );
                          
                                              if( aParam->mErrStatus != STL_SUCCESS )
                                              {
                                                  (void) ztcfConfigureerror( aParam, NULL, $3 );
                                                  YYABORT;
                                              }
                                          }
                                      }
                                  }
                              }
                          };

host_port_clause          : ZTCF_TOK_HOST_PORT ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if( gZtcfParseMinInfo == STL_TRUE )
                              {
                                  if( gZtcfBraceStarted == STL_FALSE )
                                  {
                                      ztcfSetHostPortToGlobalArea( aParam, $3 );
                          
                                      if ( aParam->mErrStatus != STL_SUCCESS )
                                      {
                                          (void) ztcfConfigureerror( aParam, NULL, $3 );
                                          YYABORT;
                                      }
                                  }
                                  else
                                  {
                                      if( stlStrlen( gConfigure.mSetGroupName ) == 0 )
                                      {
                                          ztcfSetHostIpToGroupArea( aParam, $3 );
                          
                                          if( aParam->mErrStatus != STL_SUCCESS )
                                          {
                                              (void) ztcfConfigureerror( aParam, NULL, $3 );
                                              YYABORT;
                                          }
                                      }
                                      else
                                      {
                                          if( gZtcfGroupMatched == STL_TRUE )
                                          {
                                              ztcfSetHostPortToGroupArea( aParam, $3 );
                          
                                              if( aParam->mErrStatus != STL_SUCCESS )
                                              {
                                                  (void) ztcfConfigureerror( aParam, NULL, $3 );
                                                  YYABORT;
                                              }
                                          }
                                      }
                                  }
                              }
                          };

master_ip_clause        : ZTCF_TOK_MASTER_IP ZTCF_TOK_EQUALS ZTCF_TOK_STRING
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetMasterIp( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

trans_sort_clause       : ZTCF_TOK_TRANS_SORT_AREA_SIZE ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetTransSortAreaSize( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

trans_filePath_clause   : ZTCF_TOK_TRANS_FILE_PATH ZTCF_TOK_EQUALS ZTCF_TOK_STRING
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetTransFilePath( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

applier_count_clause    : ZTCF_TOK_APPLIER_COUNT ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetApplierCount( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

apply_commit_clause     : ZTCF_TOK_APPLY_COMMIT_SIZE ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetApplyCommitSize( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

apply_array_clause      : ZTCF_TOK_APPLY_ARRAY_SIZE ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetApplyArraySize( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

read_log_block_clause   : ZTCF_TOK_READ_LOG_BLOCK_COUNT ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetReadLogBlockCount( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

comm_chunk_clause       : ZTCF_TOK_COMM_CHUNK_COUNT ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetCommChunkCount( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

propagate_mode_clause   : ZTCF_TOK_PROPAGATE_MODE ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetPropagateMode( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };
      
log_path_clause         : ZTCF_TOK_LOG_PATH ZTCF_TOK_EQUALS ZTCF_TOK_STRING
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetUserLogFilePath( aParam, $3 );
                              
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

giveup_interval_clause  : ZTCF_TOK_GIVEUP_INTERVAL ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetGiveupInterval( aParam, $3 );
                                  
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

goldilocks_listen_port_clause : ZTCF_TOK_GOLDILOCKS_LISTEN_PORT ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                           {
                               if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                    ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                               {
                                   ztcfSetGoldilocksListenPort( aParam, $3 );
                                  
                                   if ( aParam->mErrStatus != STL_SUCCESS )
                                   {
                                       (void) ztcfConfigureerror( aParam, NULL, $3 );
                                       YYABORT;
                                   }
                               }       
                           };

syncher_count_clause    : ZTCF_TOK_SYNCHER_COUNT ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetSyncherCount( aParam, $3 );
                                  
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

sync_array_clause       : ZTCF_TOK_SYNC_ARRAY_SIZE ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetSyncArraySize( aParam, $3 );
                                  
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

update_apply_mode_clause : ZTCF_TOK_UPDATE_APPLY_MODE ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                          {
                              if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                   ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                              {
                                  ztcfSetUpdateApplyMode( aParam, $3 );
                                  
                                  if ( aParam->mErrStatus != STL_SUCCESS )
                                  {
                                      (void) ztcfConfigureerror( aParam, NULL, $3 );
                                      YYABORT;
                                  }
                              }
                          };

lib_config_clause       : ZTCF_TOK_LIB_CONFIG ZTCF_TOK_EQUALS ZTCF_TOK_STRING
                           {
                               if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                    ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                               {
                                   ztcfSetLibConfig( aParam, $3 );
                                  
                                   if ( aParam->mErrStatus != STL_SUCCESS )
                                   {
                                       (void) ztcfConfigureerror( aParam, NULL, $3 );
                                       YYABORT;
                                   }
                               }       
                           };
                           
sql_file_name_clause    : ZTCF_TOK_SQL_FILE_NAME ZTCF_TOK_EQUALS ZTCF_TOK_STRING
                           {
                               if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                    ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                               {
                                   ztcfSetSqlFileName( aParam, $3 );
                                  
                                   if ( aParam->mErrStatus != STL_SUCCESS )
                                   {
                                       (void) ztcfConfigureerror( aParam, NULL, $3 );
                                       YYABORT;
                                   }
                               }       
                           };
                           
sql_file_size_clause    : ZTCF_TOK_SQL_FILE_SIZE ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                           {
                               if ( ( gZtcfParseMinInfo == STL_FALSE && gZtcfGroupMatched == STL_TRUE ) ||
                                    ( gZtcfParseMinInfo == STL_FALSE && gZtcfBraceStarted == STL_FALSE ) )
                               {
                                   ztcfSetSqlFileSize( aParam, $3 );
                                  
                                   if ( aParam->mErrStatus != STL_SUCCESS )
                                   {
                                       (void) ztcfConfigureerror( aParam, NULL, $3 );
                                       YYABORT;
                                   }
                               }       
                           };

monitor_file_clause      : ZTCF_TOK_MONITOR_FILE ZTCF_TOK_EQUALS ZTCF_TOK_STRING
                           {
                           };

monitor_cycle_ms_clause  : ZTCF_TOK_MONITOR_CYCLE_MS ZTCF_TOK_EQUALS ZTCF_TOK_NUMBER
                           {
                           };
                                                      
%%
            
stlInt32 ztcfConfigureerror( ztcConfigureParam * aParam,
                             void              * aScanner, 
                             const stlChar     * aMsg )
{
    stlInt32   sLineNo   = 1;
    stlChar  * sStartPtr = aParam->mBuffer;
    stlChar  * sEndPtr   = NULL;
    stlChar    sErrMsg[1024] = {0,};
    
    while( sLineNo < gZtcfConfigureLineNo )
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
                 gZtcfConfigureLineNo );
    
    return -1;   
}

void ztcfAddGroupItem( ztcConfigureParam * aParam,
                       stlChar           * aGroupName )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( stlStrlen( gConfigure.mSetGroupName ) == 0 )
    {
        if( ztcmAddGroupItem( aGroupName, 
                              sErrorStack ) != STL_SUCCESS )
        {
            aParam->mErrStatus  = STL_FAILURE;
        }
    }
    else
    {
        if( stlStrcasecmp( gConfigure.mSetGroupName, aGroupName ) == 0 )
        {
            if( ztcmAddGroupItem( aGroupName, 
                                  sErrorStack ) != STL_SUCCESS )
            {
                aParam->mErrStatus  = STL_FAILURE;
            }   
        }
    }
}

void ztcfSetUserIdToGlobalArea( ztcConfigureParam * aParam, 
                                stlChar           * aUserId )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetUserIdToGlobalArea( aUserId,
                                   sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetUserIdToGroupArea( ztcConfigureParam * aParam, 
                               stlChar           * aUserId )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetUserIdToGroupArea( aUserId,
                                  sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetUserPwToGlobalArea( ztcConfigureParam * aParam, 
                                stlChar           * aUserPw )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetUserPwToGlobalArea( aUserPw,
                                   sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetUserPwToGroupArea( ztcConfigureParam * aParam, 
                               stlChar           * aUserPw )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetUserPwToGroupArea( aUserPw,
                                  sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}


void ztcfSetUserEncryptPwToGlobalArea( ztcConfigureParam * aParam, 
                                       stlChar           * aUserEncryptPw )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetUserEncryptPwToGlobalArea( aUserEncryptPw,
                                          sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetUserEncryptPwToGroupArea( ztcConfigureParam * aParam, 
                                      stlChar           * aUserEncryptPw )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetUserEncryptPwToGroupArea( aUserEncryptPw,
                                         sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

stlStatus ztcfIdentifyGroup( stlErrorStack * aErrorStack )
{
    stlFile     sFile;
    stlInt32    sRet;
    stlInt32    sState          = 0;
    stlChar   * sReadBuffer     = NULL;
    stlBool     sExist          = STL_FALSE;
    stlOffset   sFileSize       = 0;
    stlOffset   sOffset         = 0;
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    
    ztcConfigureParam sParam;
    
    gZtcfConfigureLineNo   = 1;
    gConfigure.mGroupCount = 0;
    
    /**
     * 이 단계에서는 GroupName과 UserID/PW만 처리한다.
     * 만약 Group을 설정해서 실행할 경우에는 해당 Group 정보만 처리하게 된다.
     */
    gZtcfParseMinInfo = STL_TRUE;
    
    STL_TRY( ztcmGetConfigureFileName( sFileName, 
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

    ztcfConfigurelex_init( &sParam.mYyScanner );
    ztcfConfigureset_extra( &sParam, sParam.mYyScanner );
    sRet = ztcfConfigureparse( &sParam, sParam.mYyScanner );
    ztcfConfigurelex_destroy( sParam.mYyScanner );
    
    STL_TRY_THROW( ( sRet == 0 ) && 
                   ( sParam.mErrStatus == STL_SUCCESS ) , RAMP_ERR_PARSE_FAIL );
    
    sState = 1;
    (void) stlFreeHeap( sReadBuffer );
    
    sState = 0;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );

    gZtcfParseMinInfo = STL_FALSE;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_CATCH( RAMP_ERR_PARSE_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_CONFIGURE_FILE_PARSE,
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

stlStatus ztcfReadConfigure( stlErrorStack * aErrorStack )
{
    stlFile     sFile;
    stlInt32    sRet;
    stlInt32    sState          = 0;
    stlChar   * sReadBuffer     = NULL;
    stlBool     sExist          = STL_FALSE;
    stlOffset   sFileSize       = 0;
    stlOffset   sOffset         = 0;
    stlChar     sFileName[STL_MAX_FILE_PATH_LENGTH];
    
    ztcConfigureParam sParam;
    
    gZtcfConfigureLineNo = 1;
    gZtcfParseMinInfo  = STL_FALSE;
    gZtcfGroupMatched    = STL_FALSE;
    
    STL_TRY( ztcmGetConfigureFileName( sFileName, 
                                       aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
    
    STL_TRY( ztcmLogMsg( aErrorStack,
                         "Read Configure File[%s]\n",
                         sFileName ) == STL_SUCCESS );

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

    ztcfConfigurelex_init( &sParam.mYyScanner );
    ztcfConfigureset_extra( &sParam, sParam.mYyScanner );
    sRet = ztcfConfigureparse( &sParam, sParam.mYyScanner );
    ztcfConfigurelex_destroy( sParam.mYyScanner );

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
                      ZTC_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_CATCH( RAMP_ERR_PARSE_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_CONFIGURE_FILE_PARSE,
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

void ztcfAddTableInfo( ztcConfigureParam * aParam,
                       stlChar           * aSchemaName,
                       stlChar           * aTableName )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmAddTableInfo( aSchemaName, 
                          aTableName,
                          sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfAddApplyInfo( ztcConfigureParam * aParam,
                       stlChar           * aMasterSchema,
                       stlChar           * aMasterTable,
                       stlChar           * aSlaveSchema,
                       stlChar           * aSlaveTable )
{
    stlErrorStack * sErrorStack   = aParam->mErrorStack;
        
    if( ztcmAddApplyInfo( aMasterSchema, 
                          aMasterTable, 
                          aSlaveSchema,
                          aSlaveTable,
                          sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetPort( ztcConfigureParam * aParam, 
                  stlChar           * aPort )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetPort( aPort,
                     sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetHostIpToGlobalArea( ztcConfigureParam * aParam, 
                                stlChar           * aHostIp )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetHostIpToGlobalArea( aHostIp,
                                   sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetHostIpToGroupArea( ztcConfigureParam * aParam, 
                               stlChar           * aHostIp )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetHostIpToGroupArea( aHostIp,
                                  sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}


void ztcfSetHostPortToGlobalArea( ztcConfigureParam * aParam, 
                                  stlChar           * aPort )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetHostPortToGlobalArea( aPort,
                                     sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}
                           
void ztcfSetHostPortToGroupArea( ztcConfigureParam * aParam, 
                                 stlChar           * aPort )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetHostPortToGroupArea( aPort,
                                    sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetDsnToGlobalArea( ztcConfigureParam * aParam, 
                             stlChar           * aDsn )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetDsnToGlobalArea( aDsn,
                                sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}
                           
void ztcfSetDsnToGroupArea( ztcConfigureParam * aParam, 
                            stlChar           * aDsn )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetDsnToGroupArea( aDsn,
                               sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetMasterIp( ztcConfigureParam * aParam, 
                      stlChar           * aMasterIp )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetMasterIp( aMasterIp,
                         sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetTransSortAreaSize( ztcConfigureParam * aParam, 
                               stlChar           * aSize )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetTransSortAreaSize( aSize,
                                  sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetApplierCount( ztcConfigureParam * aParam, 
                          stlChar           * aSize )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetApplierCount( aSize,
                             sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetApplyCommitSize( ztcConfigureParam * aParam, 
                             stlChar           * aSize )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetApplyCommitSize( aSize,
                                sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetApplyArraySize( ztcConfigureParam * aParam, 
                            stlChar           * aSize )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetApplyArraySize( aSize,
                               sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetReadLogBlockCount( ztcConfigureParam * aParam, 
                               stlChar            * aCount )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetReadLogBlockCount( aCount,
                                  sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetCommChunkCount( ztcConfigureParam * aParam, 
                            stlChar           * aCount )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetCommChunkCount( aCount,
                               sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetPropagateMode( ztcConfigureParam * aParam, 
                           stlChar           * aMode )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetPropagateMode( aMode,
                              sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetUserLogFilePath( ztcConfigureParam * aParam, 
                             stlChar           * aLogPath )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetUserLogFilePath( aLogPath,
                                sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetTransFilePath( ztcConfigureParam * aParam, 
                           stlChar           * aPath )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetTransFilePath( aPath,
                              sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetGiveupInterval( ztcConfigureParam * aParam, 
                            stlChar           * aInterval )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetGiveupInterval( aInterval,
                               sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetGoldilocksListenPort( ztcConfigureParam * aParam, 
                             stlChar           * aPort )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetGoldilocksListenPort( aPort,
                                sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
} 

void ztcfSetSyncherCount( ztcConfigureParam * aParam, 
                          stlChar           * aCount )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetSyncherCount( aCount,
                             sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
} 

void ztcfSetSyncArraySize( ztcConfigureParam * aParam, 
                           stlChar           * aSize )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetSyncArraySize( aSize,
                              sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
} 


void ztcfSetUpdateApplyMode( ztcConfigureParam * aParam, 
                             stlChar           * aMode )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetUpdateApplyMode( aMode,
                                sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
} 

void ztcfSetLibConfig( ztcConfigureParam * aParam, 
                       stlChar           * aLibConfig )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetLibConfig( aLibConfig,
                          sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetSqlFileName( ztcConfigureParam * aParam, 
                         stlChar           * aFileName )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetSqlFileName( aFileName,
                            sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

void ztcfSetSqlFileSize( ztcConfigureParam * aParam, 
                         stlChar           * aFileSize )
{
    stlErrorStack * sErrorStack = aParam->mErrorStack;
    
    if( ztcmSetSqlFileSize( aFileSize,
                            sErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus  = STL_FAILURE;
    }
}

