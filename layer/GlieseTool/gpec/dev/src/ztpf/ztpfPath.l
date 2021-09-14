/*******************************************************************************
 * ztpfPath.l
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *  reference : http://c.comsci.us/etymology/lexicon.html
 *  reference : ISO/IEC 9899:1999
 *
 ******************************************************************************/

%{
#include <stl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztpDef.h>
#include <ztpfPath.tab.h>
#include <ztpfPathFunc.h>
#include <ztpMisc.h>

#ifndef ZTP_PARSER_PRINT
#define ZTPF_PARSE_PRINT  stlIgnorePrintf
//#define ZTPF_PARSE_PRINT  stlPrintf
#else
#define ZTPF_PARSE_PRINT  stlPrintf
#endif

#define ZTPF_PATH_PARAM yyget_extra( yyscanner )

#define ZTPF_PARSE_IGNORE_STR( param )          \
    {                                           \
        *yylloc = ZTPF_PATH_PARAM->mCurrLoc;    \
    }

#define ZTPF_PARSE_RETURN_STR( aStr )                       \
    {                                                       \
        if(stlAllocRegionMem(ZTPF_PATH_PARAM->mAllocator,   \
                             stlStrlen(aStr) + 1,           \
                             yylval,                        \
                             ZTPF_PATH_PARAM->mErrorStack)  \
           != STL_SUCCESS)                                  \
        {                                                   \
            ZTPF_PATH_PARAM->mErrorStatus = STL_FAILURE;    \
        }                                                   \
        else                                                \
        {                                                   \
            stlMemcpy(*yylval, aStr, stlStrlen(aStr) + 1);  \
        }                                                   \
    }

#define YY_INPUT( buffer, res, max_size )                               \
    do {                                                                \
        stlInt32  sNumBytesToRead = max_size;                           \
        stlInt32  sBytesRemain;                                         \
        sBytesRemain = ZTPF_PATH_PARAM->mPathLength - ZTPF_PATH_PARAM->mPosition; \
        if(sNumBytesToRead > sBytesRemain)                              \
        {                                                               \
            sNumBytesToRead = sBytesRemain;                             \
        }                                                               \
        stlMemcpy(buffer,                                               \
                  ZTPF_PATH_PARAM->mBuffer + ZTPF_PATH_PARAM->mPosition, \
                  sNumBytesToRead);                                     \
        res = sNumBytesToRead;                                          \
        ZTPF_PATH_PARAM->mPosition += sNumBytesToRead;                  \
    } while (0)

#define YY_USER_ACTION                                                  \
    {                                                                   \
        stlChar * sStr;                                                 \
        if(yylineno != ZTPF_PATH_PARAM->mLineNo)                        \
        {                                                               \
            ZTPF_PATH_PARAM->mColumn = 1;                               \
            ZTPF_PATH_PARAM->mLength = 0;                               \
            sStr = ZTPF_PATH_PARAM->mBuffer + ZTPF_PATH_PARAM->mNextLoc \
                + yyleng - 1;                                           \
            while(*sStr != '\n')                                        \
            {                                                           \
                ZTPF_PATH_PARAM->mLength++;                             \
                sStr--;                                                 \
            }                                                           \
            ZTPF_PATH_PARAM->mLength++;                                 \
        }                                                               \
        else                                                            \
        {                                                               \
            ZTPF_PATH_PARAM->mColumn += ZTPF_PATH_PARAM->mLength;       \
            ZTPF_PATH_PARAM->mLength  = yyleng;                         \
        }                                                               \
        ZTPF_PATH_PARAM->mLineNo  = yylineno;                           \
        ZTPF_PATH_PARAM->mCurrLoc = ZTPF_PATH_PARAM->mNextLoc;          \
        ZTPF_PATH_PARAM->mNextLoc += yyleng;                            \
        *yylloc = ZTPF_PATH_PARAM->mCurrLoc;                            \
    }
%}


%option reentrant noyywrap never-interactive nounistd
%option bison-bridge bison-locations
%option case-insensitive
%option nounput
%option yylineno

/* SPECIAL CHARACTER */
space                      [ \t\n\r\v\f]
whitespace                 {space}+
right_paren                ")"
semicolon                  ";"

/* Tokens */
/* pattern for rules */
path_desc                  [^\); \t\n\r\v\f]*

%%

{whitespace} {
    ZTPF_PARSE_PRINT("WHITESPACE : %s\n", yytext);
}

{right_paren} {
    ZTPF_PARSE_PRINT("[)]: %s: yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);
    ZTPF_PARSE_RETURN_STR(yytext);
    return ZTPF_TOK_SYM_RPAREN;
}

{semicolon} {
    ZTPF_PARSE_PRINT("SEMICOLON: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPF_PARSE_RETURN_STR(yytext);
    return ZTPF_TOK_SYM_SEMICOLON;
}

{path_desc} {
    ZTPF_PARSE_PRINT("PATH_DESC: %s: yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);
    ZTPF_PARSE_RETURN_STR(yytext);
    return ZTPF_TOK_PATH_DESC;
}

%%

