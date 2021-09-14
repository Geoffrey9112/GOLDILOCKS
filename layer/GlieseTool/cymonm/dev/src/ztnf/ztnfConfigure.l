/*******************************************************************************
 * ztnConfigure.l
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: enigma $
 *
 * NOTES
 *        
 *
 ******************************************************************************/
 
%{

#include <goldilocks.h>
#include <ztn.h>
#include <ztnfConfigure.tab.h>

#define ZTN_CONF_PARAM yyget_extra( yyscanner )

#define YY_INPUT( aBuffer, aRes, aMaxSize )                                 \
    do                                                                      \
    {                                                                       \
        if( ZTN_CONF_PARAM->mPosition >= ZTN_CONF_PARAM->mLength )          \
        {                                                                   \
            aRes = YY_NULL;                                                 \
        }                                                                   \
        else                                                                \
        {                                                                   \
            aRes = ZTN_CONF_PARAM->mLength - ZTN_CONF_PARAM->mPosition;     \
            aRes > (stlInt32)aMaxSize ? aRes = aMaxSize : 0;                \
            stlMemcpy( aBuffer,                                             \
                       ZTN_CONF_PARAM->mBuffer + ZTN_CONF_PARAM->mPosition, \
                       aRes );                                              \
            ZTN_CONF_PARAM->mPosition += aRes;                              \
        }                                                                   \
    } while( 0 )
    
stlInt32       gZtnConfigureLineNo = 1;

%}

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge
%option case-insensitive
%option nounput noinput

DIGIT                 [0-9]*
LETTER                [A-Za-z_]
LETTER_OR_DIGIT       [A-Za-z_0-9\$]
DOUBLE_QUOTE          "\""
QUOTED_STRING         \'([^'\\\n]|\\.|\'\')*\'
DOUBLE_QUOTE_STRING   {DOUBLE_QUOTE}[^\"]*{DOUBLE_QUOTE}
STRING                {LETTER}{LETTER_OR_DIGIT}*
IP_STRING             {DIGIT}"."{DIGIT}"."{DIGIT}"."{DIGIT}
        
%%
capture_table               return ZTN_TOK_CAPTURE_TABLE;
apply_table                 return ZTN_TOK_APPLY_TABLE;
to                          return ZTN_TOK_TO;
group_name                  return ZTN_TOK_GROUP_NAME;
user_id                     return ZTN_TOK_USER_ID;
user_pw                     return ZTN_TOK_USER_PW;
user_encrypt_pw             return ZTN_TOK_USER_ENCRYPT_PW;
port                        return ZTN_TOK_PORT;
dsn                         return ZTN_TOK_DSN;
host_ip                     return ZTN_TOK_HOST_IP;
host_port                   return ZTN_TOK_HOST_PORT;
master_ip                   return ZTN_TOK_MASTER_IP;
trans_sort_area_size        return ZTN_TOK_TRANS_SORT_AREA_SIZE;
trans_file_path             return ZTN_TOK_TRANS_FILE_PATH;
apply_commit_size           return ZTN_TOK_APPLY_COMMIT_SIZE;
apply_array_size            return ZTN_TOK_APPLY_ARRAY_SIZE;
applier_count               return ZTN_TOK_APPLIER_COUNT;
read_log_block_count        return ZTN_TOK_READ_LOG_BLOCK_COUNT;
comm_chunk_count            return ZTN_TOK_COMM_CHUNK_COUNT;
propagate_mode              return ZTN_TOK_PROPAGATE_MODE;
log_path                    return ZTN_TOK_LOG_PATH;
giveup_interval             return ZTN_TOK_GIVEUP_INTERVAL;
goldilocks_listen_port           return ZTN_TOK_GOLDILOCKS_LISTEN_PORT;
syncher_count               return ZTN_TOK_SYNCHER_COUNT;
sync_array_size             return ZTN_TOK_SYNC_ARRAY_SIZE;
update_apply_mode           return ZTN_TOK_UPDATE_APPLY_MODE;
lib_config                  return ZTN_TOK_LIB_CONFIG;
sql_file_name               return ZTN_TOK_SQL_FILE_NAME;
sql_file_size               return ZTN_TOK_SQL_FILE_SIZE;
monitor_file                return ZTN_TOK_MONITOR_FILE;
monitor_cycle_ms            return ZTN_TOK_MONITOR_CYCLE_MS;
"("                         return ZTN_TOK_OPEN_BRACKET;
")"                         return ZTN_TOK_CLOSE_BRACKET;
"{"                         return ZTN_TOK_OPEN_BRACE;
"}"                         return ZTN_TOK_CLOSE_BRACE;
"="                         return ZTN_TOK_EQUALS;
","                         return ZTN_TOK_COMMA;
"."                         return ZTN_TOK_DOT;

\n                          gZtnConfigureLineNo++;
[ \t\r]+                    /* ignore whitespace */;
#.*                         /* ignore Comment    */;


{STRING}                    {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTN_TOK_STRING;
                            };
{QUOTED_STRING}             {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTN_TOK_STRING;
                            };
{IP_STRING}                 {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTN_TOK_STRING;
                            };
{DIGIT}                     {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTN_TOK_NUMBER;
                            }
{DOUBLE_QUOTE_STRING}       {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTN_TOK_STRING;
                            }
                            
%%
        
