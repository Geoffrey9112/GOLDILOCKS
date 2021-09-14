/*******************************************************************************
 * ztpfIncludeFile.l
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
#include <ztpfIncludeFileFunc.h>
#include <ztpMisc.h>

#include <ztpfIncludeFile.tab.h>

#ifndef ZTP_PARSER_PRINT
#define ZTPF_PARSE_PRINT  stlIgnorePrintf
//#define ZTPF_PARSE_PRINT  stlPrintf
#else
#define ZTPF_PARSE_PRINT  stlPrintf
#endif

#define PARAM yyget_extra( yyscanner )

#define ZTPF_PARSE_IGNORE_STR( param )          \
    {                                           \
        *yylloc = PARAM->mCurrLoc;              \
    }

#define ZTPF_PARSE_RETURN_STR( aStr )                       \
    {                                                       \
        if(stlAllocRegionMem(PARAM->mAllocator,             \
                             stlStrlen(aStr) + 1,           \
                             yylval,                        \
                             PARAM->mErrorStack)            \
           != STL_SUCCESS)                                  \
        {                                                   \
            PARAM->mErrorStatus = STL_FAILURE;              \
        }                                                   \
        else                                                \
        {                                                   \
            stlMemcpy(*yylval, aStr, stlStrlen(aStr) + 1);  \
        }                                                   \
    }

#define YY_INPUT( buffer, res, max_size )                       \
    do {                                                        \
        stlInt32  sNumBytesToRead = max_size;                   \
        stlInt32  sBytesRemain;                                 \
        sBytesRemain = PARAM->mPathLength - PARAM->mPosition;   \
        if(sNumBytesToRead > sBytesRemain)                      \
        {                                                       \
            sNumBytesToRead = sBytesRemain;                     \
        }                                                       \
        stlMemcpy(buffer,                                       \
                  PARAM->mBuffer + PARAM->mPosition,            \
                  sNumBytesToRead);                             \
        res = sNumBytesToRead;                                  \
        PARAM->mPosition += sNumBytesToRead;                    \
    } while (0)

#define YY_USER_ACTION                              \
    {                                               \
        stlChar * sStr;                             \
        if(yylineno != PARAM->mLineNo)              \
        {                                           \
            PARAM->mColumn = 1;                     \
            PARAM->mLength = 0;                     \
            sStr = PARAM->mBuffer + PARAM->mNextLoc \
                + yyleng - 1;                       \
            while(*sStr != '\n')                    \
            {                                       \
                PARAM->mLength++;                   \
                sStr--;                             \
            }                                       \
            PARAM->mLength++;                       \
        }                                           \
        else                                        \
        {                                           \
            PARAM->mColumn += PARAM->mLength;       \
            PARAM->mLength  = yyleng;               \
        }                                           \
        PARAM->mLineNo  = yylineno;                 \
        PARAM->mCurrLoc = PARAM->mNextLoc;          \
        PARAM->mNextLoc += yyleng;                  \
        *yylloc = PARAM->mCurrLoc;                  \
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
semicolon                  ";"

/* Tokens */
/* pattern for rules */
path_desc                  [^\; \t\n\r\v\f]*

%%

{whitespace} {
    ZTPF_PARSE_PRINT("WHITESPACE : %s\n", yytext);
}

{semicolon} {
    ZTPF_PARSE_PRINT("SEMICOLON: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPF_PARSE_RETURN_STR(yytext);
    return ZTPF_INC_TOK_SYM_SEMICOLON;
}

{path_desc} {
    ZTPF_PARSE_PRINT("PATH_DESC: %s: yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);
    ZTPF_PARSE_RETURN_STR(yytext);
    return ZTPF_INC_TOK_PATH_DESC;
}

%%

