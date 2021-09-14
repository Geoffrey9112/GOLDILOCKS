/*******************************************************************************
 * ztrcConfigureParser.l
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
#include <ztrcConfigure.tab.h>

#define ZTR_CONF_PARAM yyget_extra( yyscanner )

#define YY_INPUT( aBuffer, aRes, aMaxSize )                                 \
    do                                                                      \
    {                                                                       \
        if( ZTR_CONF_PARAM->mPosition >= ZTR_CONF_PARAM->mLength )          \
        {                                                                   \
            aRes = YY_NULL;                                                 \
        }                                                                   \
        else                                                                \
        {                                                                   \
            aRes = ZTR_CONF_PARAM->mLength - ZTR_CONF_PARAM->mPosition;     \
            aRes > (stlInt32)aMaxSize ? aRes = aMaxSize : 0;                \
            stlMemcpy( aBuffer,                                             \
                       ZTR_CONF_PARAM->mBuffer + ZTR_CONF_PARAM->mPosition, \
                       aRes );                                              \
            ZTR_CONF_PARAM->mPosition += aRes;                              \
        }                                                                   \
    } while( 0 )
    
stlInt32       gZtrcConfigureLineNo = 1;

%}

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge
%option case-insensitive
%option nounput noinput

DIGIT               [0-9]*
LETTER              [A-Za-z_]
LETTER_OR_DIGIT     [A-Za-z_0-9]
STRING              {LETTER}{LETTER_OR_DIGIT}*
QUOTED_STRING       \'([^'\\\n]|\\.|\'\')*\'
QUALIFIED_STRING    {STRING}"."{STRING}
IP_STRING           {DIGIT}"."{DIGIT}"."{DIGIT}"."{DIGIT}

%%
user_id                     return ZTRC_TOK_USER_ID;
user_pw                     return ZTRC_TOK_USER_PW;
port                        return ZTRC_TOK_PORT;
host_ip                     return ZTRC_TOK_HOST_IP;
host_port                   return ZTRC_TOK_HOST_PORT;
dsn                         return ZTRC_TOK_DSN;
master_ip                   return ZTRC_TOK_MASTER_IP;
log_path                    return ZTRC_TOK_LOG_PATH;
"="                         return ZTRC_TOK_EQUALS;
","                         return ZTRC_TOK_COMMA;

\n                          gZtrcConfigureLineNo++;
[ \t\r]+                    /* ignore whitespace */;
#.*                         /* ignore Comment    */;

{STRING}                    {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTRC_TOK_STRING;
                            };
{QUALIFIED_STRING}          {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTRC_TOK_STRING;
                            };
{QUOTED_STRING}             {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTRC_TOK_STRING;
                            };
{IP_STRING}                 {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTRC_TOK_STRING;
                            };
{DIGIT}                     {
                                stlStrncpy( yylval->str_val, yytext, stlStrlen( yytext ) + 1);
                                return ZTRC_TOK_NUMBER;
                            }
                            
%%
        
