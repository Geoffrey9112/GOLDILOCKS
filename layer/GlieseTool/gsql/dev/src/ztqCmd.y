/*******************************************************************************
 * ztq.y
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztq.y 1957 2011-09-21 06:16:18Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

%{
#include <stdio.h>
#include <string.h>

#define YYSTYPE void *

#include <stl.h>
#include <dtl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztqDef.h>
#include <ztqCmdParser.h>
    
#if 1
#define ZTQ_PARSE_CHECK( aInstruction )             \
    {                                               \
        aInstruction;                               \
        if( aParam->mErrStatus != STL_SUCCESS )     \
        {                                           \
            YYABORT;                                \
        }                                           \
    }
#else
#define ZTQ_PARSE_CHECK( aInstruction )   ;
#endif

%}

%pure-parser
%parse-param { ztqCmdParseParam * aParam }
%parse-param { void             * aScanner }
%lex-param   { yyscan_t         * aScanner }

%token ZTQ_CMD_TOK_SLASH_ALLOC;
%token ZTQ_CMD_TOK_SLASH_CONNECT;
%token ZTQ_CMD_TOK_SLASH_DESC;
%token ZTQ_CMD_TOK_SLASH_FREE;
%token ZTQ_CMD_TOK_SLASH_IDESC;
%token ZTQ_CMD_TOK_SLASH_EDIT;
%token ZTQ_CMD_TOK_SLASH_EXEC;
%token ZTQ_CMD_TOK_SLASH_DYNAMIC;
%token ZTQ_CMD_TOK_SLASH_HELP;
%token ZTQ_CMD_TOK_SLASH_IMPORT;
%token ZTQ_CMD_TOK_SLASH_PREPARE;
%token ZTQ_CMD_TOK_SLASH_PRINT;
%token ZTQ_CMD_TOK_SLASH_QUIT;
%token ZTQ_CMD_TOK_SLASH_SET;
%token ZTQ_CMD_TOK_SLASH_SPOOL;
%token ZTQ_CMD_TOK_SLASH_VAR;
%token ZTQ_CMD_TOK_SLASH_USE;
%token ZTQ_CMD_TOK_SLASH_HISTORY;
%token ZTQ_CMD_TOK_SLASH_NUMBER;
%token ZTQ_CMD_TOK_SLASH_EXPLAIN;
%token ZTQ_CMD_TOK_SLASH_XA;
%token ZTQ_CMD_TOK_SLASH_SLASH;
%token ZTQ_CMD_TOK_SLASH_STARTUP;
%token ZTQ_CMD_TOK_SLASH_SHUTDOWN;
%token ZTQ_CMD_TOK_SLASH_DDL_DB;
%token ZTQ_CMD_TOK_SLASH_DDL_PROFILE;
%token ZTQ_CMD_TOK_SLASH_DDL_AUTH;
%token ZTQ_CMD_TOK_SLASH_DDL_SPACE;
%token ZTQ_CMD_TOK_SLASH_DDL_SCHEMA;
%token ZTQ_CMD_TOK_SLASH_DDL_PUBLIC_SYNONYM;
%token ZTQ_CMD_TOK_SLASH_DDL_TABLE;
%token ZTQ_CMD_TOK_SLASH_DDL_VIEW;
%token ZTQ_CMD_TOK_SLASH_DDL_SEQUENCE;
%token ZTQ_CMD_TOK_SLASH_DDL_INDEX;
%token ZTQ_CMD_TOK_SLASH_DDL_CONSTRAINT;
%token ZTQ_CMD_TOK_SLASH_DDL_SYNONYM;
%token ZTQ_CMD_TOK_ABORT;
%token ZTQ_CMD_TOK_ADMIN;
%token ZTQ_CMD_TOK_ALTER;
%token ZTQ_CMD_TOK_APPEND;
%token ZTQ_CMD_TOK_AS;
%token ZTQ_CMD_TOK_ASYNC;
%token ZTQ_CMD_TOK_AUTOCOMMIT;
%token ZTQ_CMD_TOK_BIGINT;
%token ZTQ_CMD_TOK_BINARY;
%token ZTQ_CMD_TOK_BOOLEAN;
%token ZTQ_CMD_TOK_BYTE;
%token ZTQ_CMD_TOK_CALLSTACK;
%token ZTQ_CMD_TOK_CHAR;
%token ZTQ_CMD_TOK_CHARACTER;
%token ZTQ_CMD_TOK_CHARACTERS;
%token ZTQ_CMD_TOK_CLOSE;
%token ZTQ_CMD_TOK_COLOR;
%token ZTQ_CMD_TOK_COLSIZE;
%token ZTQ_CMD_TOK_CREATE;
%token ZTQ_CMD_TOK_NUMSIZE;
%token ZTQ_CMD_TOK_COMMENT;
%token ZTQ_CMD_TOK_COMMIT;
%token ZTQ_CMD_TOK_CONSTRAINT;
%token ZTQ_CMD_TOK_DAY;
%token ZTQ_CMD_TOK_DATE;
%token ZTQ_CMD_TOK_DDLSIZE;
%token ZTQ_CMD_TOK_DOUBLE;
%token ZTQ_CMD_TOK_ECHO;
%token ZTQ_CMD_TOK_END;
%token ZTQ_CMD_TOK_ERROR;
%token ZTQ_CMD_TOK_EXEC;
%token ZTQ_CMD_TOK_FAIL;
%token ZTQ_CMD_TOK_FLOAT;
%token ZTQ_CMD_TOK_FORGET;
%token ZTQ_CMD_TOK_GRANT;
%token ZTQ_CMD_TOK_HISTORY;
%token ZTQ_CMD_TOK_HOUR;
%token ZTQ_CMD_TOK_IDENTITY;
%token ZTQ_CMD_TOK_IMMEDIATE;
%token ZTQ_CMD_TOK_INDEX;
%token ZTQ_CMD_TOK_INTEGER;
%token ZTQ_CMD_TOK_INTERVAL;
%token ZTQ_CMD_TOK_JOIN;
%token ZTQ_CMD_TOK_IDENTIFIER;
%token ZTQ_CMD_TOK_LINESIZE;
%token ZTQ_CMD_TOK_LONG;
%token ZTQ_CMD_TOK_MINUTE;
%token ZTQ_CMD_TOK_MIGRATE;
%token ZTQ_CMD_TOK_MONTH;
%token ZTQ_CMD_TOK_MOUNT;
%token ZTQ_CMD_TOK_MULTIPLE;
%token ZTQ_CMD_TOK_NATIVE_BIGINT;
%token ZTQ_CMD_TOK_NATIVE_DOUBLE;
%token ZTQ_CMD_TOK_NATIVE_INTEGER;
%token ZTQ_CMD_TOK_NATIVE_REAL;
%token ZTQ_CMD_TOK_NATIVE_SMALLINT;
%token ZTQ_CMD_TOK_NOMOUNT;
%token ZTQ_CMD_TOK_NORMAL;
%token ZTQ_CMD_TOK_NOWAIT;
%token ZTQ_CMD_TOK_NUMBER;
%token ZTQ_CMD_TOK_NULL;
%token ZTQ_CMD_TOK_NUMBERTYPE;
%token ZTQ_CMD_TOK_NUMERIC;
%token ZTQ_CMD_TOK_OCTETS;
%token ZTQ_CMD_TOK_OFF;
%token ZTQ_CMD_TOK_ON;
%token ZTQ_CMD_TOK_ONEPHASE;
%token ZTQ_CMD_TOK_ONLY;
%token ZTQ_CMD_TOK_OPEN;
%token ZTQ_CMD_TOK_OPERATIONAL;
%token ZTQ_CMD_TOK_OR;
%token ZTQ_CMD_TOK_PAGESIZE;
%token ZTQ_CMD_TOK_PATH;
%token ZTQ_CMD_TOK_PREPARE;
%token ZTQ_CMD_TOK_PLAN;
%token ZTQ_CMD_TOK_PRECISION;
%token ZTQ_CMD_TOK_REAL;
%token ZTQ_CMD_TOK_RECOVER;
%token ZTQ_CMD_TOK_REPLACE;
%token ZTQ_CMD_TOK_RESTART;
%token ZTQ_CMD_TOK_RESULT;
%token ZTQ_CMD_TOK_RESUME;
%token ZTQ_CMD_TOK_ROLLBACK;
%token ZTQ_CMD_TOK_ROWID;
%token ZTQ_CMD_TOK_SCHEMA;
%token ZTQ_CMD_TOK_SECOND;
%token ZTQ_CMD_TOK_SEQUENCE;
%token ZTQ_CMD_TOK_SMALLINT;
%token ZTQ_CMD_TOK_START;
%token ZTQ_CMD_TOK_STRING;
%token ZTQ_CMD_TOK_SUCCESS;
%token ZTQ_CMD_TOK_SUPPLEMENTAL;
%token ZTQ_CMD_TOK_SUSPEND;
%token ZTQ_CMD_TOK_SQL;
%token ZTQ_CMD_TOK_SYSDBA;
%token ZTQ_CMD_TOK_SYNONYM;
%token ZTQ_CMD_TOK_TABLE;
%token ZTQ_CMD_TOK_TIME;
%token ZTQ_CMD_TOK_TIMESTAMP;
%token ZTQ_CMD_TOK_TIMING;
%token ZTQ_CMD_TOK_TRANSACTIONAL;
%token ZTQ_CMD_TOK_TO;
%token ZTQ_CMD_TOK_USER;
%token ZTQ_CMD_TOK_VARBINARY;
%token ZTQ_CMD_TOK_VARCHAR;
%token ZTQ_CMD_TOK_VARYING;
%token ZTQ_CMD_TOK_VERBOSE;
%token ZTQ_CMD_TOK_VERTICAL;
%token ZTQ_CMD_TOK_VIEW;
%token ZTQ_CMD_TOK_WITH;
%token ZTQ_CMD_TOK_YEAR;
%token ZTQ_CMD_TOK_ZONE;
%token ZTQ_CMD_TOK_HOST_ASSIGN;
%token ZTQ_CMD_TOK_HOST_VARIABLE;
%token ZTQ_CMD_TOK_OPEN_PARENTHESES;
%token ZTQ_CMD_TOK_CLOSE_PARENTHESES;
%token ZTQ_CMD_TOK_PERIOD;
%token ZTQ_CMD_TOK_COMMA;
%token ZTQ_CMD_TOK_DOUBLE_QUOTE_STRING;
%token ZTQ_CMD_TOK_DUMMY;

%%
command:
/* empty */                              { aParam->mParseTree = (ztqParseTree*)ZTQ_EMPTY_PARSE_TREE; }
    | set_command_statement              { aParam->mParseTree = (ztqParseTree*)$1; }
    | quit_command_statement             { aParam->mParseTree = (ztqParseTree*)$1; }
    | help_command_statement             { aParam->mParseTree = (ztqParseTree*)$1; }
    | import_command_statement           { aParam->mParseTree = (ztqParseTree*)$1; }
    | print_command_statement            { aParam->mParseTree = (ztqParseTree*)$1; }
    | var_command_statement              { aParam->mParseTree = (ztqParseTree*)$1; }
    | edit_command_statement             { aParam->mParseTree = (ztqParseTree*)$1; }
    | exec_command_statement             { aParam->mParseTree = (ztqParseTree*)$1; }
    | prepare_command_statement          { aParam->mParseTree = (ztqParseTree*)$1; }
    | dynamic_command_statement          { aParam->mParseTree = (ztqParseTree*)$1; }
    | print_history_command_statement    { aParam->mParseTree = (ztqParseTree*)$1; }
    | slash_command_statement            { aParam->mParseTree = (ztqParseTree*)$1; }
    | desc_command_statement             { aParam->mParseTree = (ztqParseTree*)$1; }
    | idesc_command_statement            { aParam->mParseTree = (ztqParseTree*)$1; }
    | explain_plan_statement             { aParam->mParseTree = (ztqParseTree*)$1; }
    | alloc_command_statement            { aParam->mParseTree = (ztqParseTree*)$1; }
    | free_command_statement             { aParam->mParseTree = (ztqParseTree*)$1; }
    | use_command_exec_sql_statement     { aParam->mParseTree = (ztqParseTree*)$1; }
    | use_command_prepare_sql_statement  { aParam->mParseTree = (ztqParseTree*)$1; }
    | use_command_exec_statement         { aParam->mParseTree = (ztqParseTree*)$1; }
    | startup_statement                  { aParam->mParseTree = (ztqParseTree*)$1; }
    | shutdown_statement                 { aParam->mParseTree = (ztqParseTree*)$1; }
    | connect_statement                  { aParam->mParseTree = (ztqParseTree*)$1; }
    | xa_statement                       { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_db_statement                   { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_space_statement                { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_profile_statement              { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_auth_statement                 { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_schema_statement               { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_public_synonym_statement       { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_table_statement                { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_constraint_statement           { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_index_statement                { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_view_statement                 { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_sequence_statement             { aParam->mParseTree = (ztqParseTree*)$1; }
    | ddl_synonym_statement              { aParam->mParseTree = (ztqParseTree*)$1; }
    | spool_command_statement            { aParam->mParseTree = (ztqParseTree*)$1; }
    ;

exec_command_statement:
    ZTQ_CMD_TOK_SLASH_EXEC
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdExecPreparedTree( aParam ) );
    }
    | ZTQ_CMD_TOK_SLASH_EXEC ZTQ_CMD_TOK_HOST_VARIABLE ZTQ_CMD_TOK_HOST_ASSIGN
    {
        ZTQ_PARSE_CHECK( aParam->mParseTree = (ztqParseTree*)ztqMakeCmdExecAssignTree( aParam, (ztqHostVariable*)$2 ) );
        YYABORT;
    }
    | ZTQ_CMD_TOK_SLASH_EXEC ZTQ_CMD_TOK_SQL
    {
        aParam->mSqlPosition = aParam->mCurPosition;
        ZTQ_PARSE_CHECK( aParam->mParseTree = (ztqParseTree*)ztqMakeCmdExecSqlTree( aParam ) );
        YYABORT;
    }
    ;

edit_command_statement:
    ZTQ_CMD_TOK_SLASH_EDIT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdEditTree( aParam, NULL, 0 ) );
    }
    | ZTQ_CMD_TOK_SLASH_EDIT ZTQ_CMD_TOK_STRING
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdEditTree( aParam, (stlChar *)$2, 0 ) );
    }
    | ZTQ_CMD_TOK_SLASH_EDIT ZTQ_CMD_TOK_NUMBER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdEditTree( aParam, NULL, (stlChar *)$2 ) );
    }
    | ZTQ_CMD_TOK_SLASH_EDIT ZTQ_CMD_TOK_HISTORY ZTQ_CMD_TOK_NUMBER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdEditTree( aParam, NULL, (stlChar *)$3 ) );
    }
    ;

prepare_command_statement:
    ZTQ_CMD_TOK_SLASH_PREPARE ZTQ_CMD_TOK_SQL
    {
        aParam->mSqlPosition = aParam->mCurPosition;
        ZTQ_PARSE_CHECK( aParam->mParseTree = (ztqParseTree*)ztqMakeCmdPrepareSqlTree( aParam ) );
        YYABORT;
    }

dynamic_command_statement:
    ZTQ_CMD_TOK_SLASH_DYNAMIC ZTQ_CMD_TOK_SQL ZTQ_CMD_TOK_HOST_VARIABLE
    {
        aParam->mSqlPosition = aParam->mCurPosition;
        ZTQ_PARSE_CHECK( aParam->mParseTree = (ztqParseTree*)ztqMakeCmdDynamicSqlTree( aParam, $3 ) );
        YYABORT;
    }

slash_command_statement:
    ZTQ_CMD_TOK_SLASH_SLASH
    {
        ZTQ_PARSE_CHECK( $$ = (ztqParseTree*)ztqMakeCmdExecHistoryTree( aParam, NULL ) );
    }
    | ZTQ_CMD_TOK_SLASH_NUMBER
    {
        ZTQ_PARSE_CHECK( $$ = (ztqParseTree*)ztqMakeCmdExecHistoryTree( aParam, (stlChar*)$1 ) );
    }
    ;

print_command_statement:
    ZTQ_CMD_TOK_SLASH_PRINT regualar_identifier
    {
        ZTQ_PARSE_CHECK( $$ = (ztqParseTree*)ztqMakeCmdPrintTree( aParam, $2 ) );
    }
    | ZTQ_CMD_TOK_SLASH_PRINT
    {
        ZTQ_PARSE_CHECK( $$ = (ztqParseTree*)ztqMakeCmdPrintTree( aParam, NULL ) );
    }
    ;

var_command_statement:
    ZTQ_CMD_TOK_SLASH_VAR regualar_identifier data_type_clause
    {
        ZTQ_PARSE_CHECK( $$ = (ztqParseTree*)ztqMakeCmdDeclareVarTree( aParam, $2, (ztqDataType*)$3 ) );
    }
    ;

data_type_clause:
    ZTQ_CMD_TOK_BOOLEAN
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_BOOLEAN,
                                                 gDefaultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_NATIVE_SMALLINT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_SMALLINT,
                                                 DTL_NATIVE_SMALLINT_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_SMALLINT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_NUMERIC,
                                                 DTL_DECIMAL_SMALLINT_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_NATIVE_INTEGER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_INTEGER,
                                                 DTL_NATIVE_INTEGER_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_INTEGER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_NUMERIC,
                                                 DTL_DECIMAL_INTEGER_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_NATIVE_BIGINT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_BIGINT,
                                                 DTL_NATIVE_BIGINT_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_BIGINT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_NUMERIC,
                                                 DTL_DECIMAL_BIGINT_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_NATIVE_REAL
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_REAL,
                                                 DTL_NATIVE_REAL_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_REAL
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_FLOAT,
                                                 DTL_DECIMAL_REAL_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_NATIVE_DOUBLE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_DOUBLE,
                                                 DTL_NATIVE_DOUBLE_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_DOUBLE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_FLOAT,
                                                 DTL_DECIMAL_DOUBLE_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_DOUBLE ZTQ_CMD_TOK_PRECISION
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_FLOAT,
                                                 DTL_DECIMAL_DOUBLE_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_FLOAT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_FLOAT,
                                                 DTL_FLOAT_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_FLOAT ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumericType( aParam,
                                                  SQL_FLOAT,
                                                  $3,
                                                  NULL ) );
    }
    | ZTQ_CMD_TOK_NUMERIC 
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumericType( aParam,
                                                  SQL_NUMERIC,
                                                  NULL,
                                                  NULL ) );
    }
    | ZTQ_CMD_TOK_NUMERIC ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumericType( aParam,
                                                  SQL_NUMERIC,
                                                  $3,
                                                  NULL ) );
    }
    | ZTQ_CMD_TOK_NUMERIC ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER 
    ZTQ_CMD_TOK_COMMA ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumericType( aParam,
                                                  SQL_NUMERIC,
                                                  $3,
                                                  $5 ) );
    }
    | ZTQ_CMD_TOK_NUMBERTYPE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumberType( aParam,
                                                 SQL_FLOAT,
                                                 DTL_FLOAT_DEFAULT_PRECISION,
                                                 0 ) );
    }
    | ZTQ_CMD_TOK_NUMBERTYPE ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumericType( aParam,
                                                  SQL_NUMERIC,
                                                  $3,
                                                  NULL ) );
    }
    | ZTQ_CMD_TOK_NUMBERTYPE ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER
    ZTQ_CMD_TOK_COMMA ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeNumericType( aParam,
                                                  SQL_NUMERIC,
                                                  $3,
                                                  $5 ) );
    }
    | ZTQ_CMD_TOK_CHAR
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               NULL ) );
    }
    | ZTQ_CMD_TOK_CHAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_CHAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CHARACTERS ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_CHAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CHAR ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_CHAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_OCTETS ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_OCTETS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_CHAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_BYTE ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_OCTETS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_CHARACTER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               NULL ) );
    }
    | ZTQ_CMD_TOK_CHARACTER ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_CHARACTER ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CHARACTERS ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_CHARACTER ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CHAR ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_CHARACTER ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_OCTETS ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_OCTETS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_CHARACTER ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_BYTE ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_CHAR,
                                               DTL_STRING_LENGTH_UNIT_OCTETS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_VARCHAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARCHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_VARCHAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CHARACTERS ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARCHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_VARCHAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CHAR ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARCHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_VARCHAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_OCTETS ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARCHAR,
                                               DTL_STRING_LENGTH_UNIT_OCTETS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_VARCHAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_BYTE ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARCHAR,
                                               DTL_STRING_LENGTH_UNIT_OCTETS,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_CHARACTER ZTQ_CMD_TOK_VARYING ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARCHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $4 ) );
    }
    | ZTQ_CMD_TOK_CHARACTER ZTQ_CMD_TOK_VARYING ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CHARACTERS ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARCHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $4 ) );
    }
    | ZTQ_CMD_TOK_CHARACTER ZTQ_CMD_TOK_VARYING ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CHAR ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARCHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               $4 ) );
    }
    | ZTQ_CMD_TOK_CHARACTER ZTQ_CMD_TOK_VARYING ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_OCTETS ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARCHAR,
                                               DTL_STRING_LENGTH_UNIT_OCTETS,
                                               $4 ) );
    }
    | ZTQ_CMD_TOK_CHARACTER ZTQ_CMD_TOK_VARYING ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_BYTE ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARCHAR,
                                               DTL_STRING_LENGTH_UNIT_OCTETS,
                                               $4 ) );
    }
    | ZTQ_CMD_TOK_LONG ZTQ_CMD_TOK_CHARACTER ZTQ_CMD_TOK_VARYING
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_LONGVARCHAR,
                                               DTL_STRING_LENGTH_UNIT_CHARACTERS,
                                               NULL ) );
    }
    | ZTQ_CMD_TOK_LONG ZTQ_CMD_TOK_CHAR ZTQ_CMD_TOK_VARYING
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_LONGVARCHAR,
                                               DTL_STRING_LENGTH_UNIT_OCTETS,
                                               NULL ) );
    }
    | ZTQ_CMD_TOK_LONG ZTQ_CMD_TOK_VARCHAR
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_LONGVARCHAR,
                                               DTL_STRING_LENGTH_UNIT_OCTETS,
                                               NULL ) );
    }
    | ZTQ_CMD_TOK_BINARY
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_BINARY,
                                               DTL_STRING_LENGTH_UNIT_NA,
                                               NULL ) );
    }
    | ZTQ_CMD_TOK_BINARY ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_BINARY,
                                               DTL_STRING_LENGTH_UNIT_NA,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_VARBINARY ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARBINARY,
                                               DTL_STRING_LENGTH_UNIT_NA,
                                               $3 ) );
    }
    | ZTQ_CMD_TOK_BINARY ZTQ_CMD_TOK_VARYING ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_VARBINARY,
                                               DTL_STRING_LENGTH_UNIT_NA,
                                               $4 ) );
    }
    | ZTQ_CMD_TOK_LONG ZTQ_CMD_TOK_BINARY ZTQ_CMD_TOK_VARYING
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_LONGVARBINARY,
                                               DTL_STRING_LENGTH_UNIT_NA,
                                               NULL ) );
    }
    | ZTQ_CMD_TOK_LONG ZTQ_CMD_TOK_VARBINARY
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCharType( aParam,
                                               SQL_LONGVARBINARY,
                                               DTL_STRING_LENGTH_UNIT_NA,
                                               NULL ) );
    }
    | ZTQ_CMD_TOK_DATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeTemporalType( aParam,
                                                   SQL_TYPE_TIMESTAMP,
                                                   (YYSTYPE)"0",
                                                   gDefaultDataTypeDef[DTL_TYPE_TIMESTAMP].mArgNum2 ) );
    }
    /* TIME WITH TIME ZONE */
    | ZTQ_CMD_TOK_TIME ZTQ_CMD_TOK_WITH ZTQ_CMD_TOK_TIME ZTQ_CMD_TOK_ZONE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeTemporalType( aParam,
                                                   SQL_TYPE_TIME_WITH_TIMEZONE,
                                                   (YYSTYPE)"6",
                                                   gDefaultDataTypeDef[DTL_TYPE_TIME_WITH_TIME_ZONE].mArgNum2 ) );
    }
    /* TIME( precision ) WITH TIME ZONE */
    | ZTQ_CMD_TOK_TIME ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_WITH ZTQ_CMD_TOK_TIME ZTQ_CMD_TOK_ZONE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeTemporalType( aParam,
                                                   SQL_TYPE_TIME_WITH_TIMEZONE,
                                                   $3,
                                                   gDefaultDataTypeDef[DTL_TYPE_TIME_WITH_TIME_ZONE].mArgNum2 ) );
    }
    /* TIME */
    | ZTQ_CMD_TOK_TIME
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeTemporalType( aParam,
                                                   SQL_TYPE_TIME,
                                                   (YYSTYPE)"6",
                                                   gDefaultDataTypeDef[DTL_TYPE_TIME].mArgNum2 ) );
    }
    /* TIME( precision ) */
    | ZTQ_CMD_TOK_TIME ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeTemporalType( aParam,
                                                   SQL_TYPE_TIME,
                                                   $3,
                                                   gDefaultDataTypeDef[DTL_TYPE_TIME].mArgNum2 ) );
    }
    /* TIMESTAMP WITH TIME ZONE */
    | ZTQ_CMD_TOK_TIMESTAMP ZTQ_CMD_TOK_WITH ZTQ_CMD_TOK_TIME ZTQ_CMD_TOK_ZONE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeTemporalType( aParam,
                                                   SQL_TYPE_TIMESTAMP_WITH_TIMEZONE,
                                                   (YYSTYPE)"6",
                                                   gDefaultDataTypeDef[DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE].mArgNum2 ) );
    }
    /* TIMESTAMP( precision ) WITH TIME ZONE */
    | ZTQ_CMD_TOK_TIMESTAMP ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_WITH ZTQ_CMD_TOK_TIME ZTQ_CMD_TOK_ZONE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeTemporalType( aParam,
                                                   SQL_TYPE_TIMESTAMP_WITH_TIMEZONE,
                                                   $3,
                                                   gDefaultDataTypeDef[DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE].mArgNum2 ) );
    }
    /* TIMESTAMP */
    | ZTQ_CMD_TOK_TIMESTAMP
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeTemporalType( aParam,
                                                   SQL_TYPE_TIMESTAMP,
                                                   (YYSTYPE)"6",
                                                   gDefaultDataTypeDef[DTL_TYPE_TIMESTAMP].mArgNum2 ) );
    }
    /* TIMESTAMP( precision ) */
    | ZTQ_CMD_TOK_TIMESTAMP ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeTemporalType( aParam,
                                                   SQL_TYPE_TIMESTAMP,
                                                   $3,
                                                   gDefaultDataTypeDef[DTL_TYPE_TIMESTAMP].mArgNum2 ) );
    }
    /* INTERVAL YEAR */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_YEAR
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_YEAR,
                                                   (YYSTYPE)"2",
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_YEAR_TO_MONTH].mArgNum2 ) );
    }
    /* INTERVAL YEAR( leading precision ) */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_YEAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_YEAR,
                                                   $4,
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_YEAR_TO_MONTH].mArgNum2 ) );
    }
    /* INTERVAL MONTH */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_MONTH
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_MONTH,
                                                   (YYSTYPE)"2",
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_YEAR_TO_MONTH].mArgNum2 ) );
    }
    /* INTERVAL MONTH( leading precision ) */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_MONTH ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_MONTH,
                                                   $4,
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_YEAR_TO_MONTH].mArgNum2 ) );
    }
    /* INTERVAL YEAR TO MONTH */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_YEAR ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_MONTH
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_YEAR_TO_MONTH,
                                                   (YYSTYPE)"2",
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_YEAR_TO_MONTH].mArgNum2 ) );
    }
    /* INTERVAL YEAR( leading precision ) TO MONTH */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_YEAR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_MONTH
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_YEAR_TO_MONTH,
                                                   $4,
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_YEAR_TO_MONTH].mArgNum2 ) );
    }
    /* INTERVAL DAY */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_DAY
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_DAY,
                                                   (YYSTYPE)"2",
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL DAY( leading precision ) */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_DAY ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_DAY,
                                                   $4,
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL HOUR */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_HOUR
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_HOUR,
                                                   (YYSTYPE)"2",
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL HOUR( leading precision ) */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_HOUR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_HOUR,
                                                   $4,
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL MINUTE */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_MINUTE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_MINUTE,
                                                   (YYSTYPE)"2",
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL MINUTE( leading precision ) */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_MINUTE ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_MINUTE,
                                                   $4,
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL SECOND */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_SECOND
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_SECOND,
                                                         (YYSTYPE)"2",
                                                         (YYSTYPE)"6" ) );
    }
    /* INTERVAL SECOND( leading precision ) */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_SECOND ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_SECOND,
                                                         $4,
                                                         (YYSTYPE)"6" ) );
    }
    /* INTERVAL SECOND( leading precision, fractional second ) */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_SECOND
    ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_COMMA ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_SECOND,
                                                         $4,
                                                         $6 ) );
    }
    /* INTERVAL DAY TO HOUR */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_DAY ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_HOUR
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_DAY_TO_HOUR,
                                                   (YYSTYPE)"2",
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL DAY( leading precision ) TO HOUR */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_DAY ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_HOUR
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_DAY_TO_HOUR,
                                                   $4,
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL DAY TO MINUTE */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_DAY ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_MINUTE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_DAY_TO_MINUTE,
                                                   (YYSTYPE)"2",
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL DAY( leading precision ) TO MINUTE */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_DAY ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_MINUTE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_DAY_TO_MINUTE,
                                                   $4,
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL DAY TO SECOND */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_DAY ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_SECOND
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_DAY_TO_SECOND,
                                                         (YYSTYPE)"2",
                                                         (YYSTYPE)"6" ) );
    }
    /* INTERVAL DAY( leading precision ) TO SECOND */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_DAY ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_SECOND
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_DAY_TO_SECOND,
                                                         $4,
                                                         (YYSTYPE)"6" ) );
    }
    /* INTERVAL DAY( leading precision ) TO SECOND( fractional second ) */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_DAY ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_SECOND ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_DAY_TO_SECOND,
                                                         $4,
                                                         $9 ) );
    }
    /* INTERVAL HOUR TO MINUTE */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_HOUR ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_MINUTE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_HOUR_TO_MINUTE,
                                                   (YYSTYPE)"2",
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL HOUR( leading precision ) TO MINUTE */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_HOUR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_MINUTE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalType( aParam,
                                                   SQL_INTERVAL_HOUR_TO_MINUTE,
                                                   $4,
                                                   gDefaultDataTypeDef[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mArgNum2 ) );
    }
    /* INTERVAL HOUR TO SECOND */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_HOUR ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_SECOND
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_HOUR_TO_SECOND,
                                                         (YYSTYPE)"2",
                                                         (YYSTYPE)"6" ) );
    }
    /* INTERVAL HOUR( leading precision ) TO SECOND */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_HOUR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_SECOND
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_HOUR_TO_SECOND,
                                                         $4,
                                                         (YYSTYPE)"6" ) );
    }
    /* INTERVAL HOUR( leading precision ) TO SECOND( fractional second ) */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_HOUR ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_SECOND ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_HOUR_TO_SECOND,
                                                         $4,
                                                         $9 ) );
    }
    /* INTERVAL MINUTE TO SECOND */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_MINUTE ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_SECOND
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_MINUTE_TO_SECOND,
                                                         (YYSTYPE)"2",
                                                         (YYSTYPE)"6" ) );
    }
    /* INTERVAL MINUTE( leading precision ) TO SECOND */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_MINUTE ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_SECOND
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_MINUTE_TO_SECOND,
                                                         $4,
                                                         (YYSTYPE)"6" ) );
    }
    /* INTERVAL MINUTE( leading precision ) TO SECOND( fractional second ) */
    | ZTQ_CMD_TOK_INTERVAL ZTQ_CMD_TOK_MINUTE ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    ZTQ_CMD_TOK_TO ZTQ_CMD_TOK_SECOND ZTQ_CMD_TOK_OPEN_PARENTHESES ZTQ_CMD_TOK_NUMBER ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIntervalSecondType( aParam,
                                                         SQL_INTERVAL_MINUTE_TO_SECOND,
                                                         $4,
                                                         $9 ) );
    }
    | ZTQ_CMD_TOK_ROWID
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeRowIdType( aParam,
                                                SQL_ROWID,
                                                gDefaultDataTypeDef[DTL_TYPE_ROWID].mArgNum1,
                                                gDefaultDataTypeDef[DTL_TYPE_ROWID].mArgNum2 ) );
    }
    ;

set_command_statement:
    ZTQ_CMD_TOK_SLASH_SET callstack_clause           {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET vertical_clause          {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET timing_clause            {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET verbose_clause           {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET linesize_clause          {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET pagesize_clause          {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET colsize_clause           {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET numsize_clause           {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET ddlsize_clause           {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET history_clause           {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET color_clause             {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET error_clause             {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET autocommit_clause        {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    | ZTQ_CMD_TOK_SLASH_SET result_clause            {  ZTQ_PARSE_CHECK( $$ = $2 );  }
    ;

callstack_clause:
    ZTQ_CMD_TOK_CALLSTACK ZTQ_CMD_TOK_ON
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetCallstackTree( aParam, STL_TRUE ) );
    }
    | ZTQ_CMD_TOK_CALLSTACK ZTQ_CMD_TOK_OFF
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetCallstackTree( aParam, STL_FALSE ) );
    }
    ;

vertical_clause:
    ZTQ_CMD_TOK_VERTICAL ZTQ_CMD_TOK_ON
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetVerticalTree( aParam, STL_TRUE ) );
    }
    | ZTQ_CMD_TOK_VERTICAL ZTQ_CMD_TOK_OFF
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetVerticalTree( aParam, STL_FALSE ) );
    }
    ;

timing_clause:
    ZTQ_CMD_TOK_TIMING ZTQ_CMD_TOK_ON
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetTimingTree( aParam, STL_TRUE ) );
    }
    | ZTQ_CMD_TOK_TIMING ZTQ_CMD_TOK_OFF
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetTimingTree( aParam, STL_FALSE ) );
    }
    ;

verbose_clause:
    ZTQ_CMD_TOK_VERBOSE ZTQ_CMD_TOK_ON
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetVerboseTree( aParam, STL_TRUE ) );
    }
    | ZTQ_CMD_TOK_VERBOSE ZTQ_CMD_TOK_OFF
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetVerboseTree( aParam, STL_FALSE ) );
    }
    ;

color_clause:
    ZTQ_CMD_TOK_COLOR ZTQ_CMD_TOK_ON
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetColorTree( aParam, STL_TRUE ) );
    }
    | ZTQ_CMD_TOK_COLOR ZTQ_CMD_TOK_OFF
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetColorTree( aParam, STL_FALSE ) );
    }
    ;

error_clause:
    ZTQ_CMD_TOK_ERROR ZTQ_CMD_TOK_ON
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetErrorTree( aParam, STL_TRUE ) );
    }
    | ZTQ_CMD_TOK_ERROR ZTQ_CMD_TOK_OFF
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetErrorTree( aParam, STL_FALSE ) );
    }
    ;

linesize_clause:
    ZTQ_CMD_TOK_LINESIZE ZTQ_CMD_TOK_NUMBER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetLinesizeTree( aParam, (stlChar*)$2 ) );
    }
    ;

pagesize_clause:
    ZTQ_CMD_TOK_PAGESIZE ZTQ_CMD_TOK_NUMBER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetPagesizeTree( aParam, (stlChar*)$2 ) );
    }
    ;

colsize_clause:
    ZTQ_CMD_TOK_COLSIZE ZTQ_CMD_TOK_NUMBER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetColsizeTree( aParam, (stlChar*)$2 ) );
    }
    ;

numsize_clause:
    ZTQ_CMD_TOK_NUMSIZE ZTQ_CMD_TOK_NUMBER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetNumsizeTree( aParam, (stlChar*)$2 ) );
    }
    ;

ddlsize_clause:
    ZTQ_CMD_TOK_DDLSIZE ZTQ_CMD_TOK_NUMBER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetDdlsizeTree( aParam, (stlChar*)$2 ) );
    }
    ;

history_clause:
    ZTQ_CMD_TOK_HISTORY ZTQ_CMD_TOK_NUMBER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetHistoryTree( aParam, (stlChar*)$2 ) );
    }
    ;

autocommit_clause:
    ZTQ_CMD_TOK_AUTOCOMMIT ZTQ_CMD_TOK_ON
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetAutocommitTree( aParam, STL_TRUE ) );
    }
    | ZTQ_CMD_TOK_AUTOCOMMIT ZTQ_CMD_TOK_OFF
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetAutocommitTree( aParam, STL_FALSE ) );
    }
    ;

result_clause:
    ZTQ_CMD_TOK_RESULT ZTQ_CMD_TOK_ON
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetResultTree( aParam, STL_TRUE ) );
    }
    | ZTQ_CMD_TOK_RESULT ZTQ_CMD_TOK_OFF
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSetResultTree( aParam, STL_FALSE ) );
    }
    ;

quit_command_statement:
    ZTQ_CMD_TOK_SLASH_QUIT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdQuitTree( aParam ) );
    }
    ;

help_command_statement:
    ZTQ_CMD_TOK_SLASH_HELP
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdHelpTree( aParam ) );
    }
    ;

import_command_statement:
    ZTQ_CMD_TOK_SLASH_IMPORT ZTQ_CMD_TOK_STRING
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdImportTree( aParam, (stlChar*)$2 ) );
    }
    ;

print_history_command_statement:
    ZTQ_CMD_TOK_SLASH_HISTORY
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdPrintHistoryTree( aParam ) );
    }
    ;

desc_command_statement:
    ZTQ_CMD_TOK_SLASH_DESC identifier_chain
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDescTree( aParam, (ztqIdentifier*)$2 ) );
    }
    ;

idesc_command_statement:
    ZTQ_CMD_TOK_SLASH_IDESC identifier_chain
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdIdescTree( aParam, (ztqIdentifier*)$2 ) );
    }
    ;

identifier_chain:
    identifier
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeIdentifier( aParam, $1 ) );
    }
    | identifier ZTQ_CMD_TOK_PERIOD identifier
    {
        ztqIdentifier * sIdentifier1;
        ztqIdentifier * sIdentifier2;
        
        ZTQ_PARSE_CHECK( sIdentifier1 = (ztqIdentifier*)ztqMakeIdentifier( aParam, $1 ) );
        ZTQ_PARSE_CHECK( sIdentifier2 = (ztqIdentifier*)ztqMakeIdentifier( aParam, $3 ) );

        ZTQ_PARSE_CHECK( $$ = ztqAddIdentifier( aParam, sIdentifier1, sIdentifier2 ) );
    }
    ;

regualar_identifier:
    ZTQ_CMD_TOK_IDENTIFIER
    {
        ZTQ_PARSE_CHECK( $$ = $1 );
    }
    | gsql_cmd_keyword
    {
        ZTQ_PARSE_CHECK( $$ = $1 );
    }
    ;

identifier:   
    regualar_identifier
    {
        ZTQ_PARSE_CHECK( $$ = $1 );
    }
    | ZTQ_CMD_TOK_DOUBLE_QUOTE_STRING
    {
        ZTQ_PARSE_CHECK( $$ = $1 );
    }
    ;

explain_plan_statement:
    ZTQ_CMD_TOK_SLASH_EXPLAIN ZTQ_CMD_TOK_PLAN
    {
        aParam->mSqlPosition = aParam->mCurPosition - aParam->mColumnLen;
        ZTQ_PARSE_CHECK( aParam->mParseTree = (ztqParseTree*)ztqMakeCmdExplainPlanTree( aParam, SQL_EXPLAIN_PLAN_ON ) );
        YYABORT;
    }
    | ZTQ_CMD_TOK_SLASH_EXPLAIN ZTQ_CMD_TOK_PLAN ZTQ_CMD_TOK_VERBOSE
    {
        aParam->mSqlPosition = aParam->mCurPosition - aParam->mColumnLen;
        ZTQ_PARSE_CHECK( aParam->mParseTree = (ztqParseTree*)ztqMakeCmdExplainPlanTree( aParam, SQL_EXPLAIN_PLAN_ON_VERBOSE ) );
        YYABORT;
    }
    | ZTQ_CMD_TOK_SLASH_EXPLAIN ZTQ_CMD_TOK_PLAN ZTQ_CMD_TOK_ON
    {
        aParam->mSqlPosition = aParam->mCurPosition;
        ZTQ_PARSE_CHECK( aParam->mParseTree = (ztqParseTree*)ztqMakeCmdExplainPlanTree( aParam, SQL_EXPLAIN_PLAN_ON ) );
        YYABORT;
    }
    | ZTQ_CMD_TOK_SLASH_EXPLAIN ZTQ_CMD_TOK_PLAN ZTQ_CMD_TOK_VERBOSE ZTQ_CMD_TOK_ON 
    {
        aParam->mSqlPosition = aParam->mCurPosition;
        ZTQ_PARSE_CHECK( aParam->mParseTree = (ztqParseTree*)ztqMakeCmdExplainPlanTree( aParam, SQL_EXPLAIN_PLAN_ON_VERBOSE ) );
        YYABORT;
    }
    | ZTQ_CMD_TOK_SLASH_EXPLAIN ZTQ_CMD_TOK_PLAN ZTQ_CMD_TOK_ONLY
    {
        aParam->mSqlPosition = aParam->mCurPosition;
        ZTQ_PARSE_CHECK( aParam->mParseTree = (ztqParseTree*)ztqMakeCmdExplainPlanTree( aParam, SQL_EXPLAIN_PLAN_ONLY ) );
        YYABORT;
    }
    ;

alloc_command_statement:
    ZTQ_CMD_TOK_SLASH_ALLOC identifier
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdAllocStmtTree( aParam, $2 ) );
    }
    ;

free_command_statement:
    ZTQ_CMD_TOK_SLASH_FREE identifier
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdFreeStmtTree( aParam, $2 ) );
    }
    ;

use_command_exec_sql_statement:
    ZTQ_CMD_TOK_SLASH_USE identifier ZTQ_CMD_TOK_EXEC ZTQ_CMD_TOK_SQL
    {
        aParam->mSqlPosition = aParam->mCurPosition;
        ZTQ_PARSE_CHECK(
            aParam->mParseTree = (ztqParseTree*)ztqMakeCmdUseStmtExecSqlTree( aParam, $2 ) );
        YYABORT;
    }
    ;

use_command_prepare_sql_statement:
    ZTQ_CMD_TOK_SLASH_USE identifier ZTQ_CMD_TOK_PREPARE ZTQ_CMD_TOK_SQL
    {
        aParam->mSqlPosition = aParam->mCurPosition;
        ZTQ_PARSE_CHECK(
            aParam->mParseTree = (ztqParseTree*)ztqMakeCmdUseStmtPrepareSqlTree( aParam, $2 ) );
        YYABORT;
    }
    ;

use_command_exec_statement:
    ZTQ_CMD_TOK_SLASH_USE identifier ZTQ_CMD_TOK_EXEC
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdUseStmtExecPreparedTree( aParam, $2 ) );
    }
    ;

startup_statement:
    ZTQ_CMD_TOK_SLASH_STARTUP ZTQ_CMD_TOK_NOMOUNT
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdStartupTree( aParam, ZTQ_STARTUP_NOMOUNT ) );
    }
    | ZTQ_CMD_TOK_SLASH_STARTUP ZTQ_CMD_TOK_MOUNT
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdStartupTree( aParam, ZTQ_STARTUP_MOUNT ) );
    }
    | ZTQ_CMD_TOK_SLASH_STARTUP ZTQ_CMD_TOK_OPEN
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdStartupTree( aParam, ZTQ_STARTUP_OPEN ) );
    }
    | ZTQ_CMD_TOK_SLASH_STARTUP
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdStartupTree( aParam, ZTQ_STARTUP_OPEN ) );
    }
    ;

shutdown_statement:
    ZTQ_CMD_TOK_SLASH_SHUTDOWN ZTQ_CMD_TOK_ABORT
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdShutdownTree( aParam, ZTQ_SHUTDOWN_ABORT ) );
    }
    | ZTQ_CMD_TOK_SLASH_SHUTDOWN ZTQ_CMD_TOK_OPERATIONAL
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdShutdownTree( aParam, ZTQ_SHUTDOWN_OPERATIONAL ) );
    }
    | ZTQ_CMD_TOK_SLASH_SHUTDOWN ZTQ_CMD_TOK_IMMEDIATE
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdShutdownTree( aParam, ZTQ_SHUTDOWN_IMMEDIATE ) );
    }
    | ZTQ_CMD_TOK_SLASH_SHUTDOWN ZTQ_CMD_TOK_TRANSACTIONAL
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdShutdownTree( aParam, ZTQ_SHUTDOWN_TRANSACTIONAL ) );
    }
    | ZTQ_CMD_TOK_SLASH_SHUTDOWN ZTQ_CMD_TOK_NORMAL
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdShutdownTree( aParam, ZTQ_SHUTDOWN_NORMAL ) );
    }
    | ZTQ_CMD_TOK_SLASH_SHUTDOWN
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdShutdownTree( aParam, ZTQ_SHUTDOWN_NORMAL ) );
    }
    ;

connect_statement:
    ZTQ_CMD_TOK_SLASH_CONNECT ZTQ_CMD_TOK_AS ZTQ_CMD_TOK_SYSDBA
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdConnectTree( aParam,
                                        NULL,
                                        NULL,
                                        NULL,
                                        ZTQ_CONNECT_AS_SYSDBA ) );
    }
    | ZTQ_CMD_TOK_SLASH_CONNECT identifier identifier ZTQ_CMD_TOK_AS ZTQ_CMD_TOK_SYSDBA
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdConnectTree( aParam,
                                        $2,
                                        $3,
                                        NULL,
                                        ZTQ_CONNECT_AS_SYSDBA ) );
    }
    | ZTQ_CMD_TOK_SLASH_CONNECT ZTQ_CMD_TOK_AS ZTQ_CMD_TOK_ADMIN
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdConnectTree( aParam,
                                        NULL,
                                        NULL,
                                        NULL,
                                        ZTQ_CONNECT_AS_ADMIN ) );
    }
    | ZTQ_CMD_TOK_SLASH_CONNECT identifier identifier ZTQ_CMD_TOK_AS ZTQ_CMD_TOK_ADMIN
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdConnectTree( aParam,
                                        $2,
                                        $3,
                                        NULL,
                                        ZTQ_CONNECT_AS_ADMIN ) );
    }
    | ZTQ_CMD_TOK_SLASH_CONNECT identifier identifier
    {
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdConnectTree( aParam,
                                        $2,
                                        $3,
                                        NULL,
                                        ZTQ_CONNECT_AS_USER ) );
    }
    | ZTQ_CMD_TOK_SLASH_CONNECT identifier identifier
    ZTQ_CMD_TOK_OPEN_PARENTHESES identifier ZTQ_CMD_TOK_CLOSE_PARENTHESES
    {
        /* for new password test */
        ZTQ_PARSE_CHECK(
            $$ = ztqMakeCmdConnectTree( aParam,
                                        $2,
                                        $3,
                                        $5,
                                        ZTQ_CONNECT_AS_USER ) );
    }
    ;

xa_statement:
    ZTQ_CMD_TOK_SLASH_XA ZTQ_CMD_TOK_OPEN ZTQ_CMD_TOK_STRING xa_flags
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdXaOpenTree( aParam,
                                                    $3,
                                                    $4 ) );
    }
    | ZTQ_CMD_TOK_SLASH_XA ZTQ_CMD_TOK_CLOSE ZTQ_CMD_TOK_STRING xa_flags
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdXaCloseTree( aParam,
                                                     $3,
                                                     $4 ) );
    }
    | ZTQ_CMD_TOK_SLASH_XA ZTQ_CMD_TOK_START xa_xid xa_flags
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdXaStartTree( aParam,
                                                     $3,
                                                     $4 ) );
    }
    | ZTQ_CMD_TOK_SLASH_XA ZTQ_CMD_TOK_END xa_xid xa_flags
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdXaEndTree( aParam,
                                                   $3,
                                                   $4 ) );
    }
    | ZTQ_CMD_TOK_SLASH_XA ZTQ_CMD_TOK_COMMIT xa_xid xa_flags
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdXaCommitTree( aParam,
                                                      $3,
                                                      $4 ) );
    }
    | ZTQ_CMD_TOK_SLASH_XA ZTQ_CMD_TOK_ROLLBACK xa_xid xa_flags
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdXaRollbackTree( aParam,
                                                        $3,
                                                        $4 ) );
    }
    | ZTQ_CMD_TOK_SLASH_XA ZTQ_CMD_TOK_PREPARE xa_xid xa_flags
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdXaPrepareTree( aParam,
                                                       $3,
                                                       $4 ) );
    }
    | ZTQ_CMD_TOK_SLASH_XA ZTQ_CMD_TOK_FORGET xa_xid xa_flags
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdXaForgetTree( aParam,
                                                      $3,
                                                      $4 ) );
    }
    | ZTQ_CMD_TOK_SLASH_XA ZTQ_CMD_TOK_RECOVER ZTQ_CMD_TOK_NUMBER xa_flags
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdXaRecoverTree( aParam,
                                                       $3,
                                                       $4 ) );
    }
    ;

xa_xid:
    ZTQ_CMD_TOK_NUMBER
    {
        ZTQ_PARSE_CHECK( $$ = ztqXaXidFromNumber( aParam, $1 ) );
    }
    | ZTQ_CMD_TOK_STRING
    {
        ZTQ_PARSE_CHECK( $$ = ztqXaXidFromString( aParam, $1 ) );
    }
    ;
        
xa_flags:
    xa_flag
    {
        ZTQ_PARSE_CHECK( $$ = $1 );
    }
    | xa_flag ZTQ_CMD_TOK_OR xa_flag
    {
        ZTQ_PARSE_CHECK( $$ = ztqAddXaFlag( aParam, $1, $3 ) );
    }
    ;
        
xa_flag:
    {
        ZTQ_PARSE_CHECK( $$ = NULL );
    }
    | ZTQ_CMD_TOK_ASYNC
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMASYNC ) );
    }
    | ZTQ_CMD_TOK_ONEPHASE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMONEPHASE ) );
    }
    | ZTQ_CMD_TOK_FAIL
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMFAIL ) );
    }
    | ZTQ_CMD_TOK_NOWAIT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMNOWAIT ) );
    }
    | ZTQ_CMD_TOK_RESUME
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMRESUME ) );
    }
    | ZTQ_CMD_TOK_SUCCESS
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMSUCCESS ) );
    }
    | ZTQ_CMD_TOK_SUSPEND
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMSUSPEND ) );
    }
    | ZTQ_CMD_TOK_START
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMSTARTRSCAN ) );
    }
    | ZTQ_CMD_TOK_END
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMENDRSCAN ) );
    }
    | ZTQ_CMD_TOK_MULTIPLE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMMULTIPLE ) );
    }
    | ZTQ_CMD_TOK_JOIN
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMJOIN ) );
    }
    | ZTQ_CMD_TOK_MIGRATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeXaFlag( aParam, TMMIGRATE ) );
    }
    ;


ddl_db_statement:
    ZTQ_CMD_TOK_SLASH_DDL_DB 
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_DB,
                                                 NULL,
                                                 ZTQ_PRINT_DDL_DB_ALL ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_DB ZTQ_CMD_TOK_GRANT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_DB,
                                                 NULL,
                                                 ZTQ_PRINT_DDL_DB_GRANT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_DB ZTQ_CMD_TOK_COMMENT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_DB,
                                                 NULL,
                                                 ZTQ_PRINT_DDL_DB_COMMENT ) );
    }
    ;

ddl_space_statement:
    ZTQ_CMD_TOK_SLASH_DDL_SPACE identifier
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SPACE,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SPACE_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SPACE identifier ZTQ_CMD_TOK_CREATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SPACE,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SPACE_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SPACE identifier ZTQ_CMD_TOK_ALTER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SPACE,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SPACE_ALTER ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SPACE identifier ZTQ_CMD_TOK_TABLE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SPACE,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SPACE_TABLE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SPACE identifier ZTQ_CMD_TOK_CONSTRAINT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SPACE,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SPACE_CONSTRAINT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SPACE identifier ZTQ_CMD_TOK_INDEX
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SPACE,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SPACE_INDEX ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SPACE identifier ZTQ_CMD_TOK_GRANT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SPACE,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SPACE_GRANT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SPACE identifier ZTQ_CMD_TOK_COMMENT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SPACE,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SPACE_COMMENT ) );
    }
    ;

ddl_profile_statement:
    ZTQ_CMD_TOK_SLASH_DDL_PROFILE identifier
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_PROFILE,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_PROFILE_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_PROFILE identifier ZTQ_CMD_TOK_CREATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_PROFILE,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_PROFILE_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_PROFILE identifier ZTQ_CMD_TOK_COMMENT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_PROFILE,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_PROFILE_COMMENT ) );
    }


ddl_auth_statement:
    ZTQ_CMD_TOK_SLASH_DDL_AUTH identifier
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_AUTH_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_AUTH identifier ZTQ_CMD_TOK_CREATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_AUTH_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_AUTH identifier ZTQ_CMD_TOK_SCHEMA ZTQ_CMD_TOK_PATH
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_AUTH_SCHEMA_PATH ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_AUTH identifier ZTQ_CMD_TOK_SCHEMA
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_AUTH_SCHEMA ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_AUTH identifier ZTQ_CMD_TOK_TABLE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_AUTH_TABLE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_AUTH identifier ZTQ_CMD_TOK_CONSTRAINT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_AUTH_CONSTRAINT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_AUTH identifier ZTQ_CMD_TOK_INDEX
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_AUTH_INDEX ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_AUTH identifier ZTQ_CMD_TOK_VIEW
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_AUTH_VIEW ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_AUTH identifier ZTQ_CMD_TOK_SEQUENCE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_AUTH_SEQUENCE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_AUTH identifier ZTQ_CMD_TOK_COMMENT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_AUTH_COMMENT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_AUTH identifier ZTQ_CMD_TOK_SYNONYM
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_AUTH,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_AUTH_SYNONYM ) );
    }
    ;


ddl_schema_statement:
    ZTQ_CMD_TOK_SLASH_DDL_SCHEMA identifier
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SCHEMA,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SCHEMA_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SCHEMA identifier ZTQ_CMD_TOK_CREATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SCHEMA,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SCHEMA_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SCHEMA identifier ZTQ_CMD_TOK_TABLE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SCHEMA,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SCHEMA_TABLE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SCHEMA identifier ZTQ_CMD_TOK_CONSTRAINT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SCHEMA,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SCHEMA_CONSTRAINT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SCHEMA identifier ZTQ_CMD_TOK_INDEX
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SCHEMA,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SCHEMA_INDEX ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SCHEMA identifier ZTQ_CMD_TOK_VIEW
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SCHEMA,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SCHEMA_VIEW ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SCHEMA identifier ZTQ_CMD_TOK_SEQUENCE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SCHEMA,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SCHEMA_SEQUENCE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SCHEMA identifier ZTQ_CMD_TOK_GRANT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SCHEMA,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SCHEMA_GRANT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SCHEMA identifier ZTQ_CMD_TOK_COMMENT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SCHEMA,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SCHEMA_COMMENT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SCHEMA identifier ZTQ_CMD_TOK_SYNONYM
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SCHEMA,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_SCHEMA_SYNONYM ) );
    }
    ;

ddl_public_synonym_statement:
    ZTQ_CMD_TOK_SLASH_DDL_PUBLIC_SYNONYM identifier
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_PUBLIC_SYNONYM,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_PUBLIC_SYNONYM_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_PUBLIC_SYNONYM identifier ZTQ_CMD_TOK_CREATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_PUBLIC_SYNONYM,
                                                 ztqMakeIdentifier( aParam, $2 ),
                                                 ZTQ_PRINT_DDL_PUBLIC_SYNONYM_CREATE ) );
    }
    ;

ddl_table_statement:
    ZTQ_CMD_TOK_SLASH_DDL_TABLE identifier_chain
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_TABLE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_TABLE_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_TABLE identifier_chain ZTQ_CMD_TOK_CREATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_TABLE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_TABLE_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_TABLE identifier_chain ZTQ_CMD_TOK_CONSTRAINT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_TABLE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_TABLE_CONSTRAINT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_TABLE identifier_chain ZTQ_CMD_TOK_INDEX
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_TABLE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_TABLE_INDEX ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_TABLE identifier_chain ZTQ_CMD_TOK_IDENTITY
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_TABLE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_TABLE_IDENTITY ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_TABLE identifier_chain ZTQ_CMD_TOK_SUPPLEMENTAL
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_TABLE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_TABLE_SUPPLEMENTAL ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_TABLE identifier_chain ZTQ_CMD_TOK_GRANT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_TABLE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_TABLE_GRANT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_TABLE identifier_chain ZTQ_CMD_TOK_COMMENT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_TABLE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_TABLE_COMMENT ) );
    }
    ;

ddl_constraint_statement:
    ZTQ_CMD_TOK_SLASH_DDL_CONSTRAINT identifier_chain
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_CONSTRAINT,
                                                 $2,
                                                 ZTQ_PRINT_DDL_CONSTRAINT_ALTER ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_CONSTRAINT identifier_chain ZTQ_CMD_TOK_ALTER
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_CONSTRAINT,
                                                 $2,
                                                 ZTQ_PRINT_DDL_CONSTRAINT_ALTER ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_CONSTRAINT identifier_chain ZTQ_CMD_TOK_COMMENT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_CONSTRAINT,
                                                 $2,
                                                 ZTQ_PRINT_DDL_CONSTRAINT_COMMENT ) );
    }
    ;


ddl_index_statement:
    ZTQ_CMD_TOK_SLASH_DDL_INDEX identifier_chain
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_INDEX,
                                                 $2,
                                                 ZTQ_PRINT_DDL_INDEX_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_INDEX identifier_chain ZTQ_CMD_TOK_CREATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_INDEX,
                                                 $2,
                                                 ZTQ_PRINT_DDL_INDEX_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_INDEX identifier_chain ZTQ_CMD_TOK_COMMENT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_INDEX,
                                                 $2,
                                                 ZTQ_PRINT_DDL_INDEX_COMMENT ) );
    }
    ;

ddl_view_statement:
    ZTQ_CMD_TOK_SLASH_DDL_VIEW identifier_chain
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_VIEW,
                                                 $2,
                                                 ZTQ_PRINT_DDL_VIEW_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_VIEW identifier_chain ZTQ_CMD_TOK_CREATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_VIEW,
                                                 $2,
                                                 ZTQ_PRINT_DDL_VIEW_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_VIEW identifier_chain ZTQ_CMD_TOK_GRANT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_VIEW,
                                                 $2,
                                                 ZTQ_PRINT_DDL_VIEW_GRANT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_VIEW identifier_chain ZTQ_CMD_TOK_COMMENT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_VIEW,
                                                 $2,
                                                 ZTQ_PRINT_DDL_VIEW_COMMENT ) );
    }
    ;

ddl_sequence_statement:
    ZTQ_CMD_TOK_SLASH_DDL_SEQUENCE identifier_chain
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SEQUENCE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_SEQUENCE_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SEQUENCE identifier_chain ZTQ_CMD_TOK_CREATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SEQUENCE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_SEQUENCE_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SEQUENCE identifier_chain ZTQ_CMD_TOK_RESTART
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SEQUENCE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_SEQUENCE_RESTART ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SEQUENCE identifier_chain ZTQ_CMD_TOK_GRANT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SEQUENCE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_SEQUENCE_GRANT ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SEQUENCE identifier_chain ZTQ_CMD_TOK_COMMENT
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SEQUENCE,
                                                 $2,
                                                 ZTQ_PRINT_DDL_SEQUENCE_COMMENT ) );
    }
    ;

ddl_synonym_statement:
    ZTQ_CMD_TOK_SLASH_DDL_SYNONYM identifier_chain
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SYNONYM,
                                                 $2,
                                                 ZTQ_PRINT_DDL_SYNONYM_CREATE ) );
    }
    |
    ZTQ_CMD_TOK_SLASH_DDL_SYNONYM identifier_chain ZTQ_CMD_TOK_CREATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdDDLTree( aParam,
                                                 ZTQ_COMMAND_TYPE_CMD_DDL_SYNONYM,
                                                 $2,
                                                 ZTQ_PRINT_DDL_SYNONYM_CREATE ) );
    }
    ;

spool_command_statement:
    ZTQ_CMD_TOK_SLASH_SPOOL
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSpoolTree( aParam,
                                                   NULL,
                                                   ZTQ_SPOOL_FLAG_STATUS ) );
    }
    | ZTQ_CMD_TOK_SLASH_SPOOL ZTQ_CMD_TOK_STRING
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSpoolTree( aParam,
                                                   (stlChar *)$2,
                                                   ZTQ_SPOOL_FLAG_BEGIN_REPLACE ) );
    }
    | ZTQ_CMD_TOK_SLASH_SPOOL ZTQ_CMD_TOK_STRING ZTQ_CMD_TOK_APPEND
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSpoolTree( aParam,
                                                   (stlChar *)$2,
                                                   ZTQ_SPOOL_FLAG_BEGIN_APPEND ) );
    }
    | ZTQ_CMD_TOK_SLASH_SPOOL ZTQ_CMD_TOK_STRING ZTQ_CMD_TOK_CREATE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSpoolTree( aParam,
                                                   (stlChar *)$2,
                                                   ZTQ_SPOOL_FLAG_BEGIN_CREATE ) );
    }
    | ZTQ_CMD_TOK_SLASH_SPOOL ZTQ_CMD_TOK_STRING ZTQ_CMD_TOK_REPLACE
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSpoolTree( aParam,
                                                   (stlChar *)$2,
                                                   ZTQ_SPOOL_FLAG_BEGIN_REPLACE ) );
    }
    | ZTQ_CMD_TOK_SLASH_SPOOL ZTQ_CMD_TOK_OFF
    {
        ZTQ_PARSE_CHECK( $$ = ztqMakeCmdSpoolTree( aParam,
                                                   NULL,
                                                   ZTQ_SPOOL_FLAG_END ) );
    }
    ;

gsql_cmd_keyword:
    ZTQ_CMD_TOK_ABORT
    | ZTQ_CMD_TOK_ADMIN
    | ZTQ_CMD_TOK_ALTER
    | ZTQ_CMD_TOK_APPEND
    | ZTQ_CMD_TOK_ASYNC
    | ZTQ_CMD_TOK_AUTOCOMMIT
    | ZTQ_CMD_TOK_BIGINT
    | ZTQ_CMD_TOK_BINARY
    | ZTQ_CMD_TOK_BOOLEAN
    | ZTQ_CMD_TOK_CALLSTACK
    | ZTQ_CMD_TOK_CHAR
    | ZTQ_CMD_TOK_CHARACTER
    | ZTQ_CMD_TOK_CLOSE
    | ZTQ_CMD_TOK_COLOR
    | ZTQ_CMD_TOK_COLSIZE
    | ZTQ_CMD_TOK_CREATE
    | ZTQ_CMD_TOK_NUMSIZE
    | ZTQ_CMD_TOK_COMMENT
    | ZTQ_CMD_TOK_COMMIT
    | ZTQ_CMD_TOK_CONSTRAINT
    | ZTQ_CMD_TOK_DAY
    | ZTQ_CMD_TOK_DATE
    | ZTQ_CMD_TOK_DDLSIZE
    | ZTQ_CMD_TOK_DOUBLE
    | ZTQ_CMD_TOK_ECHO
    | ZTQ_CMD_TOK_END
    | ZTQ_CMD_TOK_ERROR
    | ZTQ_CMD_TOK_EXEC
    | ZTQ_CMD_TOK_FAIL
    | ZTQ_CMD_TOK_FLOAT
    | ZTQ_CMD_TOK_FORGET
    | ZTQ_CMD_TOK_GRANT
    | ZTQ_CMD_TOK_HISTORY
    | ZTQ_CMD_TOK_HOUR
    | ZTQ_CMD_TOK_IDENTITY
    | ZTQ_CMD_TOK_IMMEDIATE
    | ZTQ_CMD_TOK_INDEX
    | ZTQ_CMD_TOK_INTEGER
    | ZTQ_CMD_TOK_INTERVAL
    | ZTQ_CMD_TOK_JOIN
    | ZTQ_CMD_TOK_LINESIZE
    | ZTQ_CMD_TOK_LONG
    | ZTQ_CMD_TOK_MIGRATE
    | ZTQ_CMD_TOK_MINUTE
    | ZTQ_CMD_TOK_MONTH
    | ZTQ_CMD_TOK_MOUNT
    | ZTQ_CMD_TOK_MULTIPLE
    | ZTQ_CMD_TOK_NATIVE_BIGINT
    | ZTQ_CMD_TOK_NATIVE_DOUBLE
    | ZTQ_CMD_TOK_NATIVE_INTEGER
    | ZTQ_CMD_TOK_NATIVE_REAL
    | ZTQ_CMD_TOK_NATIVE_SMALLINT
    | ZTQ_CMD_TOK_NOMOUNT
    | ZTQ_CMD_TOK_NORMAL
    | ZTQ_CMD_TOK_NOWAIT
    | ZTQ_CMD_TOK_NULL
    | ZTQ_CMD_TOK_NUMBERTYPE
    | ZTQ_CMD_TOK_NUMERIC
    | ZTQ_CMD_TOK_OFF
    | ZTQ_CMD_TOK_ON
    | ZTQ_CMD_TOK_ONEPHASE
    | ZTQ_CMD_TOK_ONLY
    | ZTQ_CMD_TOK_OPEN
    | ZTQ_CMD_TOK_OPERATIONAL
    | ZTQ_CMD_TOK_PAGESIZE
    | ZTQ_CMD_TOK_PATH
    | ZTQ_CMD_TOK_PLAN
    | ZTQ_CMD_TOK_PRECISION
    | ZTQ_CMD_TOK_PREPARE
    | ZTQ_CMD_TOK_REAL
    | ZTQ_CMD_TOK_RECOVER
    | ZTQ_CMD_TOK_REPLACE
    | ZTQ_CMD_TOK_RESTART
    | ZTQ_CMD_TOK_RESULT
    | ZTQ_CMD_TOK_RESUME
    | ZTQ_CMD_TOK_ROLLBACK
    | ZTQ_CMD_TOK_ROWID
    | ZTQ_CMD_TOK_SCHEMA
    | ZTQ_CMD_TOK_SECOND
    | ZTQ_CMD_TOK_SEQUENCE
    | ZTQ_CMD_TOK_SMALLINT
    | ZTQ_CMD_TOK_SQL
    | ZTQ_CMD_TOK_START
    | ZTQ_CMD_TOK_SUCCESS
    | ZTQ_CMD_TOK_SUPPLEMENTAL
    | ZTQ_CMD_TOK_SUSPEND
    | ZTQ_CMD_TOK_SYNONYM
    | ZTQ_CMD_TOK_SYSDBA
    | ZTQ_CMD_TOK_TABLE
    | ZTQ_CMD_TOK_TIME
    | ZTQ_CMD_TOK_TIMESTAMP
    | ZTQ_CMD_TOK_TIMING
    | ZTQ_CMD_TOK_TRANSACTIONAL
    | ZTQ_CMD_TOK_TO
    | ZTQ_CMD_TOK_USER
    | ZTQ_CMD_TOK_VARBINARY
    | ZTQ_CMD_TOK_VARCHAR
    | ZTQ_CMD_TOK_VARYING
    | ZTQ_CMD_TOK_VERBOSE
    | ZTQ_CMD_TOK_VERTICAL
    | ZTQ_CMD_TOK_VIEW
    | ZTQ_CMD_TOK_WITH
    | ZTQ_CMD_TOK_YEAR
    | ZTQ_CMD_TOK_ZONE
    {
    }
    ;

%%
