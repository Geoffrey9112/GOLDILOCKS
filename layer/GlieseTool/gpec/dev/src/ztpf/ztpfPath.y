/*******************************************************************************
 * ztpfPath.y
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    reference : http://c.comsci.us/syntax/index.html
 *    reference : ISO/IEC 9899:1999
 *
 ******************************************************************************/

%{
#include <stdio.h>
#include <string.h>

#include <dtl.h>
#include <ztpDef.h>
#include <ztpfPathFunc.h>
#include <ztpfPath.tab.h>

#define YYSTYPE void *
#define YYLTYPE stlInt32
#define YYLLOC_DEFAULT(Current, Rhs, N)         \
    do {                                        \
        if (N)                                  \
            (Current) = (Rhs)[1];               \
        else                                    \
            (Current) = (Rhs)[0];               \
    } while (0)


#if 1
#define ZTPF_PARSE_CHECK( aInstruction )                \
    {                                                   \
        aInstruction;                                   \
        if( aParam->mErrorStatus != STL_SUCCESS )       \
        {                                               \
            YYABORT;                                    \
        }                                               \
    }
#else
#define ZTPF_PARSE_CHECK( aInstruction )   ;
#endif

%}

%pure-parser
%locations

%parse-param { ztpPathParseParam  * aParam  }
%parse-param { void               * aScanner }
%lex-param   { yyscan_t           * aScanner }

%token

ZTPF_TOK_PATH_DESC
ZTPF_TOK_SYM_RPAREN
ZTPF_TOK_SYM_SEMICOLON

%start start_text
%debug

%%

/********************************************************************
 * Phrase structure grammar
 ********************************************************************/

start_text
    : path_desc_list
    {
        aParam->mPathLength = aParam->mCurrLoc;
        YYACCEPT;
    }
    ;

path_desc_list
    : path_desc
    | path_desc ZTPF_TOK_SYM_SEMICOLON path_desc_list
    ;

path_desc
    : ZTPF_TOK_PATH_DESC
    {
        if( ztpfMakeIncludePath( aParam, $1 ) != STL_SUCCESS )
        {
            YYABORT;
        }
    }
    ;

%%
