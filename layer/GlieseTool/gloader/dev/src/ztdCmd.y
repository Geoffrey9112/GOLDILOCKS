/*******************************************************************************
 * ztdCmd.y
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztdCmd.y
 *
 * NOTES
 *    
 *
 ******************************************************************************/

%{
#include <stdio.h>
#include <string.h>
    
#include <stl.h>  
#include <goldilocks.h>
#include <ztdDef.h>
#include <ztdCmdParser.h>

#define ZTD_PARSE_CHECK( aRet )                 \
    {                                           \
        (aRet);                                 \
        if( aParam->mErrStatus != STL_SUCCESS ) \
        {                                       \
            YYABORT;                            \
        }                                       \
    }

#define ZTD_PARSE_COPY_CHAR( aDesc, aSrc )      \
    {                                           \
        (aDesc) = ((stlChar)(aSrc));            \
    }
    
#define ZTD_PARSE_COPY_STR( aDesc, aSrc )                               \
    {                                                                   \
        if( stlStrlen(aSrc) > 0 )                                       \
        {                                                               \
            stlStrncpy( (aDesc), (aSrc), stlStrlen( (aSrc) ) + 1 );     \
        }                                                               \
        else                                                            \
        {                                                               \
            aDesc[0] = '\0';                                            \
        }                                                               \
    }

#define YYLLOC_DEFAULT(Current, Rhs, N)         \
    do {                                        \
        if (N)                                  \
            (Current) = (Rhs)[1];               \
        else                                    \
            (Current) = (Rhs)[0];               \
    } while (0)


%}

%pure-parser
%locations

%parse-param { ztdCmdParseParam * aParam }
%parse-param { void             * aScanner }
%lex-param   { yyscan_t         * aScanner }

%token ZTD_TOK_AND;
%token ZTD_TOK_BY;
%token ZTD_TOK_CHARACTERSET;
%token ZTD_TOK_ENCLOSED;
%token ZTD_TOK_FIELDS;
%token ZTD_TOK_OPTIONALLY;
%token ZTD_TOK_TABLE;
%token ZTD_TOK_TERMINATED;
%token ZTD_TOK_PERIOD;

%token ZTD_TOK_DIGITS;
%token ZTD_TOK_STRING;
%token ZTD_TOK_QUOTE_STRING;
%token ZTD_TOK_DOUBLE_QUOTE_STRING;

%token ZTD_TOK_UNUSED_SYMBOL;

%%

start:
    control_clauses ;

control_clauses:
    control_clauses control_clause
    | control_clause
    ;


control_clause:
    table_clause
    | delimiter_clause
    | qualifier_clause
    | characterset_clause
    ;

/** character set  */
characterset_clause:
    ZTD_TOK_CHARACTERSET  ZTD_TOK_STRING
    {
        ZTD_PARSE_COPY_STR( aParam->mControlInfo->mCharacterSet, $2 );
    };

/** table name  */
table_clause:
    ZTD_TOK_TABLE table_identifier 
    {
        ztdCmdTableName * sTableName = (ztdCmdTableName *) $2;

        ZTD_PARSE_COPY_STR( aParam->mControlInfo->mSchema, sTableName->mSchema );
        ZTD_PARSE_COPY_STR( aParam->mControlInfo->mTable, sTableName->mTable );
    };


table_identifier:
    actual_identifier
    {
        ztdCmdTableName * sTableName;

        ZTD_PARSE_CHECK( sTableName = (ztdCmdTableName*)
                         ztdCmdAllocBuffer( aParam,
                                            STL_SIZEOF( ztdCmdTableName ) ) );
        
        ZTD_PARSE_COPY_STR( sTableName->mSchema, "" );

        ZTD_PARSE_COPY_STR( sTableName->mTable, $1 );

        $$ = sTableName;
    }
    |
    actual_identifier ZTD_TOK_PERIOD actual_identifier
    {
        ztdCmdTableName * sTableName;

        ZTD_PARSE_CHECK( sTableName = (ztdCmdTableName*)
                         ztdCmdAllocBuffer( aParam,
                                            STL_SIZEOF( ztdCmdTableName ) ) );
        
        ZTD_PARSE_COPY_STR( sTableName->mSchema, $1 );

        ZTD_PARSE_COPY_STR( sTableName->mTable, $3 );

        $$ = sTableName;
    };

actual_identifier:
    ZTD_TOK_STRING
    {
        $$ = $1;
    }
    | ZTD_TOK_QUOTE_STRING
    {
        $$ = $1;
    }
    | ZTD_TOK_DOUBLE_QUOTE_STRING
    {
        $$ = $1;
    };


/** field terminated by  */
delimiter_clause:
    ZTD_TOK_FIELDS  ZTD_TOK_TERMINATED  ZTD_TOK_BY  seperator
    {
        aParam->mControlInfo->mDelimiter = $4;
    };


/** optionally enclosed by */
qualifier_clause:
    ZTD_TOK_OPTIONALLY  ZTD_TOK_ENCLOSED  ZTD_TOK_BY  qualifier
    {
        ztdCmdQualifier * sQualifier = (ztdCmdQualifier*)$4;

        ZTD_PARSE_COPY_CHAR( aParam->mControlInfo->mOpQualifier,
                             sQualifier->mOpQualifier );

        ZTD_PARSE_COPY_CHAR( aParam->mControlInfo->mClQualifier,
                             sQualifier->mClQualifier );
    };

qualifier:
    seperator
    {
        ztdCmdQualifier * sQualifier;
        
        ZTD_PARSE_CHECK( sQualifier = (ztdCmdQualifier*)
                         ztdCmdAllocBuffer( aParam,
                                            STL_SIZEOF(ztdCmdQualifier) ) );
        
        ZTD_PARSE_CHECK( ztdCmdCheckQualifier( aParam, $1 ) );

        ZTD_PARSE_COPY_CHAR( sQualifier->mOpQualifier, *((stlChar*)$1) );
        ZTD_PARSE_COPY_CHAR( sQualifier->mClQualifier, *((stlChar*)$1) );

        $$ = sQualifier;
    }
    | seperator ZTD_TOK_AND seperator
    {
        ztdCmdQualifier * sQualifier;
        
        ZTD_PARSE_CHECK( sQualifier = (ztdCmdQualifier*)
                         ztdCmdAllocBuffer( aParam,
                                            STL_SIZEOF(ztdCmdQualifier) ) );
        
        
        ZTD_PARSE_CHECK( ztdCmdCheckQualifier( aParam, $1 ) );
        ZTD_PARSE_CHECK( ztdCmdCheckQualifier( aParam, $3 ) );

        ZTD_PARSE_COPY_CHAR( sQualifier->mOpQualifier, *((stlChar*)$1) );
        ZTD_PARSE_COPY_CHAR( sQualifier->mClQualifier, *((stlChar*)$3) );

        $$ = sQualifier;
    };

seperator:
    ZTD_TOK_QUOTE_STRING
    {
        $$ = $1;
    }
    | ZTD_TOK_DOUBLE_QUOTE_STRING
    {
        $$ = $1;
    };

%%
