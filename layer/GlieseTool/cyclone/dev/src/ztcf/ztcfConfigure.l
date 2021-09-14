/*******************************************************************************
 * ztcfConfigureParser.l
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
 
%{

#include <goldilocks.h>
#include <ztc.h>
#include <ztcfConfigure.tab.h>

#define ZTC_CONF_PARAM yyget_extra( yyscanner )

#define YY_INPUT( aBuffer, aRes, aMaxSize )                                 \
    do                                                                      \
    {                                                                       \
        if( ZTC_CONF_PARAM->mPosition >= ZTC_CONF_PARAM->mLength )          \
        {                                                                   \
            aRes = YY_NULL;                                                 \
        }                                                                   \
        else                                                                \
        {                                                                   \
            aRes = ZTC_CONF_PARAM->mLength - ZTC_CONF_PARAM->mPosition;     \
            aRes > (stlInt32)aMaxSize ? aRes = aMaxSize : 0;                \
            stlMemcpy( aBuffer,                                             \
                       ZTC_CONF_PARAM->mBuffer + ZTC_CONF_PARAM->mPosition, \
                       aRes );                                              \
            ZTC_CONF_PARAM->mPosition += aRes;                              \
        }                                                                   \
    } while( 0 )
    
stlInt32       gZtcfConfigureLineNo = 1;

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
capture_table               return ZTCF_TOK_CAPTURE_TABLE;
apply_table                 return ZTCF_TOK_APPLY_TABLE;
to                          return ZTCF_TOK_TO;
group_name                  return ZTCF_TOK_GROUP_NAME;
user_id                     return ZTCF_TOK_USER_ID;
user_pw                     return ZTCF_TOK_USER_PW;
port                        return ZTCF_TOK_PORT;
dsn                         return ZTCF_TOK_DSN;
host_ip                     return ZTCF_TOK_HOST_IP;
host_port                   return ZTCF_TOK_HOST_PORT;
master_ip                   return ZTCF_TOK_MASTER_IP;
trans_sort_area_size        return ZTCF_TOK_TRANS_SORT_AREA_SIZE;
trans_file_path             return ZTCF_TOK_TRANS_FILE_PATH;
apply_commit_size           return ZTCF_TOK_APPLY_COMMIT_SIZE;
apply_array_size            return ZTCF_TOK_APPLY_ARRAY_SIZE;
applier_count               return ZTCF_TOK_APPLIER_COUNT;
read_log_block_count        return ZTCF_TOK_READ_LOG_BLOCK_COUNT;
comm_chunk_count            return ZTCF_TOK_COMM_CHUNK_COUNT;
propagate_mode              return ZTCF_TOK_PROPAGATE_MODE;
log_path                    return ZTCF_TOK_LOG_PATH;
giveup_interval             return ZTCF_TOK_GIVEUP_INTERVAL;
goldilocks_listen_port           return ZTCF_TOK_GOLDILOCKS_LISTEN_PORT;
syncher_count               return ZTCF_TOK_SYNCHER_COUNT;
sync_array_size             return ZTCF_TOK_SYNC_ARRAY_SIZE;
update_apply_mode           return ZTCF_TOK_UPDATE_APPLY_MODE;
"("                         return ZTCF_TOK_OPEN_BRACKET;
")"                         return ZTCF_TOK_CLOSE_BRACKET;
"{"                         return ZTCF_TOK_OPEN_BRACE;
"}"                         return ZTCF_TOK_CLOSE_BRACE;
"="                         return ZTCF_TOK_EQUALS;
","                         return ZTCF_TOK_COMMA;
"."                         return ZTCF_TOK_DOT;

\n                          gZtcfConfigureLineNo++;
[ \t\r]+                    /* ignore whitespace */;
#.*                         /* ignore Comment    */;


{STRING}                    {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTCF_TOK_STRING;
                            };
{QUOTED_STRING}             {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTCF_TOK_STRING;
                            };
{IP_STRING}                 {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTCF_TOK_STRING;
                            };
{DIGIT}                     {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTCF_TOK_NUMBER;
                            }
{DOUBLE_QUOTE_STRING}       {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTCF_TOK_STRING;
                            }
                            
%%
        
