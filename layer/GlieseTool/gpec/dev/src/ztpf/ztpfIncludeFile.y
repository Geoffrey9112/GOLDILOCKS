/*******************************************************************************
 * ztpfIncludeFile.y
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
#include <ztpfIncludeFileFunc.h>
#include <ztpfIncludeFile.tab.h>

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

%parse-param { ztpIncludeFileParseParam  * aParam  }
%parse-param { void                      * aScanner }
%lex-param   { yyscan_t                  * aScanner }

%token

ZTPF_INC_TOK_PATH_DESC
ZTPF_INC_TOK_SYM_SEMICOLON

%start path_desc
%debug

%%

/********************************************************************
 * Phrase structure grammar
 ********************************************************************/

path_desc
    : ZTPF_INC_TOK_PATH_DESC
    {
        aParam->mPathDesc = $1;
        aParam->mPathLength = aParam->mNextLoc;
        YYACCEPT;
    }
    ;

%%
