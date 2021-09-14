/*******************************************************************************
 * ztpC.y
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
#include <goldilocks.h>
#include <zlplDef.h>
#include <ztpDef.h>
#include <ztpConvertExecSql.h>
#include <ztpMisc.h>
#include <ztpcTypeDef.h>
#include <ztpdConvert.h>
#include <ztpdType.h>
#include <ztpCParseFunc.h>
#include <ztpwPrecomp.h>

/*
 * *.tab.h 는 마지막에 include 되어야 한다.
 * 그렇지 않으면 build 중에 error 발생(YYLTYPE)
 */
#include <ztpCMin.tab.h>

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
#define ZTP_PARSE_CHECK( aInstruction )             \
    {                                               \
        aInstruction;                               \
        if( aParam->mErrorStatus != STL_SUCCESS )   \
        {                                           \
            YYABORT;                                \
        }                                           \
    }
#else
#define ZTP_PARSE_CHECK( aInstruction )   ;
#endif

#define ztpGetNextToken       YYLEX
%}

%pure-parser
%locations

%parse-param { ztpCParseParam  * aParam   }
%parse-param { void            * aScanner }
%lex-param   { yyscan_t        * aScanner }

%token
ZTP_TOK_C_COMMENT
ZTP_TOK_C_PP_DIRECTIVE

ZTP_TOK_CKW___FUNC__
ZTP_TOK_CKW__ALIGNOF
ZTP_TOK_CKW__BOOL
ZTP_TOK_CKW__COMPLEX
ZTP_TOK_CKW__IMAGINARY
ZTP_TOK_CKW_AUTO
ZTP_TOK_CKW_CHAR
ZTP_TOK_CKW_CONST
ZTP_TOK_CKW_CONTINUE
ZTP_TOK_CKW_DEFAULT
ZTP_TOK_CKW_DO
ZTP_TOK_CKW_DOUBLE
ZTP_TOK_CKW_ENUM
ZTP_TOK_CKW_EXTERN
ZTP_TOK_CKW_FLOAT
ZTP_TOK_CKW_FOR
ZTP_TOK_CKW_GOTO
ZTP_TOK_CKW_INLINE
ZTP_TOK_CKW_INT
ZTP_TOK_CKW_LONG
ZTP_TOK_CKW_REGISTER
ZTP_TOK_CKW_RESTRICT
ZTP_TOK_CKW_SHORT
ZTP_TOK_CKW_SIGNED
ZTP_TOK_CKW_SIZEOF
ZTP_TOK_CKW_STATIC
ZTP_TOK_CKW_STRUCT
ZTP_TOK_CKW_TYPEDEF
ZTP_TOK_CKW_UNION
ZTP_TOK_CKW_UNSIGNED
ZTP_TOK_CKW_VOID
ZTP_TOK_CKW_VOLATILE
ZTP_TOK_CKW_WHILE

ZTP_TOK_EKW_ALLOCATE
ZTP_TOK_EKW_AUTOCOMMIT
ZTP_TOK_EKW_AT
ZTP_TOK_EKW_ATOMIC
ZTP_TOK_EKW_BEGIN_DECL_SEC
ZTP_TOK_EKW_BINARY
ZTP_TOK_EKW_BOOLEAN
ZTP_TOK_EKW_CONTEXT
ZTP_TOK_EKW_DATE
ZTP_TOK_EKW_DAY
ZTP_TOK_EKW_END_DECL_SEC
ZTP_TOK_EKW_EXEC_SQL
ZTP_TOK_EKW_FOUND
ZTP_TOK_EKW_FREE
ZTP_TOK_EKW_HOUR
ZTP_TOK_EKW_INCLUDE
ZTP_TOK_EKW_INTERVAL
ZTP_TOK_EKW_LONGVARBINARY
ZTP_TOK_EKW_LONGVARCHAR
ZTP_TOK_EKW_MINUTE
ZTP_TOK_EKW_MONTH
ZTP_TOK_EKW_NOT
ZTP_TOK_EKW_NUMBER
ZTP_TOK_EKW_NUMBER_LPAREN
ZTP_TOK_EKW_OFF
ZTP_TOK_EKW_ON
ZTP_TOK_EKW_OPTION
ZTP_TOK_EKW_SECOND
ZTP_TOK_EKW_SQL_CONTEXT
ZTP_TOK_EKW_SQL_TYPE_IS
ZTP_TOK_EKW_SQLERROR
ZTP_TOK_EKW_SQLWARNING
ZTP_TOK_EKW_STOP
ZTP_TOK_EKW_TIME
ZTP_TOK_EKW_TIMESTAMP
ZTP_TOK_EKW_TIMEZONE
ZTP_TOK_EKW_TO
ZTP_TOK_EKW_USE
ZTP_TOK_EKW_VARBINARY
ZTP_TOK_EKW_VARCHAR
ZTP_TOK_EKW_WHENEVER
ZTP_TOK_EKW_WITH
ZTP_TOK_EKW_YEAR

ZTP_TOK_SYM_ASSIGN_MUL
ZTP_TOK_SYM_ASSIGN_DIV
ZTP_TOK_SYM_ASSIGN_MOD
ZTP_TOK_SYM_ASSIGN_ADD
ZTP_TOK_SYM_ASSIGN_SUB
ZTP_TOK_SYM_ASSIGN_LEFT
ZTP_TOK_SYM_ASSIGN_RIGHT
ZTP_TOK_SYM_ASSIGN_AND
ZTP_TOK_SYM_ASSIGN_XOR
ZTP_TOK_SYM_ASSIGN_OR
ZTP_TOK_SYM_ASSIGNMENT

ZTP_TOK_SYM_OP_RIGHT
ZTP_TOK_SYM_OP_LEFT
ZTP_TOK_SYM_OP_INC
ZTP_TOK_SYM_OP_DEC
ZTP_TOK_SYM_OP_PTR
ZTP_TOK_SYM_OP_AND
ZTP_TOK_SYM_OP_OR
ZTP_TOK_SYM_OP_LE
ZTP_TOK_SYM_OP_GE
ZTP_TOK_SYM_OP_EQ
ZTP_TOK_SYM_OP_NE
ZTP_TOK_SYM_OP_LT
ZTP_TOK_SYM_OP_GT

ZTP_TOK_SYM_ASTERISK
ZTP_TOK_SYM_AMPERSAND
ZTP_TOK_SYM_PLUS
ZTP_TOK_SYM_MINUS
ZTP_TOK_SYM_TILDE
ZTP_TOK_SYM_EXCLAMATION
ZTP_TOK_SYM_SOLIDUS
ZTP_TOK_SYM_PERCENT
ZTP_TOK_SYM_CIRCUMFLEX
ZTP_TOK_SYM_VBAR
ZTP_TOK_SYM_QUESTION

ZTP_TOK_SYM_COLON
ZTP_TOK_SYM_COMMA
ZTP_TOK_SYM_ELLIPSIS
ZTP_TOK_SYM_LBRACE
ZTP_TOK_SYM_LBRACKET
ZTP_TOK_SYM_LPAREN

ZTP_TOK_SYM_RBRACE
ZTP_TOK_SYM_RBRACKET
ZTP_TOK_SYM_RPAREN
ZTP_TOK_SYM_PERIOD
ZTP_TOK_SYM_SEMICOLON

ZTP_TOK_DQ_LITERAL
ZTP_TOK_ID
ZTP_TOK_INCLUDE_FILE
ZTP_TOK_LITERAL
ZTP_TOK_SQLSTATE
ZTP_TOK_TYPEDEF_NAME

%start gc_text
%debug
%%

/********************************************************************
 * Phrase structure grammar
 ********************************************************************/
gc_text
    : gc_statement gc_text
    | /* empty */
    {
        aParam->mCCodeEndLoc = aParam->mFileLength;
        ZTP_PARSE_CHECK(ztpwBypassCCode(aParam));
    }
    ;

gc_statement
    : exec_sql_statement
    | c_declaration
    {
        ztpProcessCDeclaration( aParam,
                                $1,
                                STL_FALSE );
        aParam->mIsTypedef = STL_FALSE;
    }
    ;

exec_sql_statement
    : ZTP_TOK_EKW_EXEC_SQL embedded_conn_string atomic_desc embedded_for_clause
    {
        stlInt32   sSqlStartPos;
        stlInt32   sEndPos;

        aParam->mCCodeEndLoc = @1;
        ZTP_PARSE_CHECK(ztpwBypassCCode(aParam));

        aParam->mConnStr = $2;

        if( $3 != NULL )
        {
            aParam->mIsAtomic = STL_TRUE;
        }
        else
        {
            aParam->mIsAtomic = STL_FALSE;
        }

        aParam->mIterationValue = $4;
        if( $4 != NULL )
        {
            sSqlStartPos = aParam->mNextLoc;
        }
        else
        {
            sSqlStartPos = aParam->mCurrLoc;
        }

        /* EXEC SQL로 지정된 구문을 C code로 변환 */
        /* Gliese common parser (SQL) used */
        if ( ztpMakeExecSqlStmtToC(aParam, sSqlStartPos, &sEndPos) != NULL )
        {
            /* success */
            /* EXEC SQL 구문을 주석 처리하여 출력 */
            ZTP_PARSE_CHECK(ztpwSendStringToOutFile(aParam, "/* "));
            ZTP_PARSE_CHECK(ztpwBypassCCodeByLoc(aParam, @1, sEndPos));
            ZTP_PARSE_CHECK(ztpwSendStringToOutFile(aParam, " */\n"));

            if( gCvtSQLFunc[aParam->mParseTree->mParseTreeType](aParam) == STL_SUCCESS )
            {
                /* success */
            }
            else
            {
                /**
                 * SQL syntax error 인 경우
                 * - error 를 출력하고 계속 진행한다.
                 */

                ZTP_PARSE_CHECK( ztpSetSqlPrecompileError( aParam ) );
            }
        }
        else
        {
            /**
             * SQL syntax error 인 경우
             * - error 를 출력하고 계속 진행한다.
             */

            ZTP_PARSE_CHECK( ztpSetSqlPrecompileError( aParam ) );

            sEndPos = aParam->mNextLoc;
        }

        yyclearin;
        while( aParam->mNextLoc <= sEndPos )
        {
            YYLEX;
        }

        ZTP_PARSE_CHECK(ztpwPrecompSourceLine(aParam, aParam->mLineNo,
                                              aParam->mInFileName, aParam->mOutFileName));

        aParam->mCCodeStartLoc = aParam->mNextLoc;
        aParam->mBypassCToken = STL_TRUE;
    }
    | ZTP_TOK_EKW_EXEC_SQL embedded_specific_statement ZTP_TOK_SYM_SEMICOLON
    {
        aParam->mCCodeEndLoc = @1;
        ZTP_PARSE_CHECK(ztpwBypassCCode(aParam));

        /* EXEC SQL 구문을 주석 처리하여 출력 */
        ZTP_PARSE_CHECK(ztpwSendStringToOutFile(aParam, "/* "));
        ZTP_PARSE_CHECK(ztpwBypassCCodeByLoc(aParam, @1, aParam->mNextLoc));
        ZTP_PARSE_CHECK(ztpwSendStringToOutFile(aParam, " */\n"));

        if( aParam->mInnerDeclSec != STL_TRUE )
        {
            aParam->mBypassCToken = STL_TRUE;
        }

        /* EXEC SQL로 지정된 구문을 C code로 변환 */
        if( $2 != NULL )
        {
            if( gCvtSQLFunc[aParam->mParseTree->mParseTreeType](aParam) == STL_SUCCESS )
            {
                /* success */
            }
            else
            {
                /**
                 * SQL syntax error 인 경우
                 * - error 를 출력하고 계속 진행한다.
                 */

                ZTP_PARSE_CHECK( ztpSetSqlPrecompileError( aParam ) );
            }
        }

        ZTP_PARSE_CHECK(ztpwPrecompSourceLine(aParam, aParam->mLineNo,
                                              aParam->mInFileName, aParam->mOutFileName));

        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    ;

embedded_conn_string
    : ZTP_TOK_EKW_AT host_variable_desc
    {
        ztpHostVariable  *sHostVariable = $2;
        $$ = sHostVariable->mHostVarString;
    }
    | ZTP_TOK_EKW_AT ZTP_TOK_ID
    {
        ZTP_PARSE_CHECK($$ = ztpMakeStaticConnStringInC(aParam, $2));
    }
    | /* empty */
    {
        $$ = NULL;
    }
    ;

atomic_desc
    : ZTP_TOK_EKW_ATOMIC
    {
        $$ = $1;
    }
    | /* empty */
    {
        $$ = NULL;
    }
    ;

embedded_for_clause
    : ZTP_TOK_CKW_FOR host_variable_desc
    {
        ztpHostVariable  *sHostVariable = $2;
        $$ = sHostVariable->mHostVarString;
    }
    | ZTP_TOK_CKW_FOR ZTP_TOK_LITERAL
    {
        $$ = $2;
    }
    | /* empty */
    {
        $$ = NULL;
    }
    ;

embedded_specific_statement
    : embedded_sql_declare_section
    {
        $$ = NULL;
    }
    | embedded_sql_include_statement
    {
        $$ = $1;
    }
    | embedded_sql_exception_statement
    {
        $$ = $1;
    }
    | embedded_context_statement
    {
        $$ = $1;
    }
    | embedded_autocommit_statement
    {
        $$ = $1;
    }
    | embedded_option_statement
    {
        $$ = NULL;
    }
    | /* empty */
    {
        $$ = NULL;
    }
    ;

embedded_sql_declare_section
    : ZTP_TOK_EKW_BEGIN_DECL_SEC
    {
        aParam->mInnerDeclSec = STL_TRUE;
        aParam->mBypassCToken = STL_FALSE;
    }
    | ZTP_TOK_EKW_END_DECL_SEC
    {
        aParam->mInnerDeclSec = STL_FALSE;
    }
    ;

c_declaration
    : c_declaration_specifiers ZTP_TOK_SYM_SEMICOLON
    {
        $$ = NULL;
    }
    | c_declaration_specifiers c_init_declarator_list ZTP_TOK_SYM_SEMICOLON
    {
        zlplCDataType          sDataType = ZLPL_C_DATATYPE_UNKNOWN;
        ztpTypeSpecifierList  *sTypeSpecifierList = (ztpTypeSpecifierList *)$1;
        ztpCVariable          *sCVariable;
        stlChar               *sPrecision;
        stlChar               *sScale;

        ZTP_PARSE_CHECK( ztpdGetCDataType( aParam,
                                           sTypeSpecifierList,
                                           &sDataType,
                                           &sPrecision,
                                           &sScale,
                                           &sCVariable ) );

        ZTP_PARSE_CHECK( $$ = ztpMakeCDeclaration( aParam,
                                                   @1,
                                                   aParam->mNextLoc,
                                                   aParam->mLineNo,
                                                   sCVariable,
                                                   sDataType,
                                                   sPrecision,
                                                   sScale,
                                                   $2 ) );
    }
    ;

c_declaration_specifiers
    : c_declaration_specifiers_desc
    {
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeDeclSpec( aParam,
                                               aParam->mDeclarationSpecifiers,
                                               $1 ) );

        aParam->mDeclarationSpecifiers = 0;
        */
        $$ = $1;
    }
    ;

c_declaration_specifiers_desc
    : c_storage_class_specifier
    {
        $$ = NULL;
    }
    | c_storage_class_specifier c_declaration_specifiers_desc
    {
        $$ = $2;
    }
    | c_type_qualifier
    {
        $$ = NULL;
    }
    | c_type_qualifier c_declaration_specifiers_desc
    {
        $$ = $2;
    }
    | c_function_specifier
    {
        $$ = NULL;
    }
    | c_function_specifier c_declaration_specifiers_desc
    {
        $$ = $2;
    }
    | c_type_specifier
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierList( aParam,
                                                        NULL,
                                                        $1) );
    }
    | c_type_specifier c_declaration_specifiers_desc
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierList( aParam,
                                                        $2,
                                                        $1) );
    }
    ;

c_storage_class_specifier
    : ZTP_TOK_CKW_AUTO
    | ZTP_TOK_CKW_EXTERN
    | ZTP_TOK_CKW_STATIC
    | ZTP_TOK_CKW_REGISTER
    | ZTP_TOK_CKW_TYPEDEF
    {
        aParam->mIsTypedef = STL_TRUE;
    }
    ;

c_type_qualifier
    : ZTP_TOK_CKW_CONST
    | ZTP_TOK_CKW_RESTRICT
    | ZTP_TOK_CKW_VOLATILE
    ;

c_function_specifier
    : ZTP_TOK_CKW_INLINE
    ;

c_init_declarator_list
    : c_init_declarator
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCDeclaratorList( aParam,
                                                      $1,
                                                      NULL ) );
    }
    | c_init_declarator ZTP_TOK_SYM_COMMA c_init_declarator_list
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCDeclaratorList( aParam,
                                                      $1,
                                                      $3) );
    }
    ;

c_init_declarator
    : c_declarator
    {
        $$ = $1;
        aParam->mCDeclarator = NULL;
    }
    | c_declarator ZTP_TOK_SYM_ASSIGNMENT c_initializer /* token_stream */
    {
        $$ = $1;
        aParam->mCDeclarator = NULL;
    }
    ;

c_declarator
    : c_direct_declarator
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCDeclarator(aParam, $1, STL_FALSE) );
    }
    | c_pointer c_direct_declarator
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCDeclarator(aParam, $2, STL_TRUE) );
    }
    ;

c_direct_declarator
    : c_identifier
    {
        /* Declare Identifier */
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        $1,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        NULL ) );
    }
    | ZTP_TOK_SYM_LPAREN c_declarator ZTP_TOK_SYM_RPAREN
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        $2,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        NULL ) );
        aParam->mCDeclarator = NULL;
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET ZTP_TOK_SYM_RBRACKET
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        "",
                                                        NULL,
                                                        NULL ) );
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET c_assignment_expression ZTP_TOK_SYM_RBRACKET

    {
        /*
         * Declare array
         */
        stlChar  *sAssignExpr;

        sAssignExpr = ztpMakeCAssignmentExpression( aParam,
                                                    @3,
                                                    @4 );

        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        sAssignExpr,
                                                        NULL,
                                                        NULL ) );
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET c_type_qualifier_list ZTP_TOK_SYM_RBRACKET
    {
        /* parameter list */
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        NULL,
                                                        NULL,
                                                        NULL ) );
        */
        $$ = $1;
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET c_type_qualifier_list c_assignment_expression ZTP_TOK_SYM_RBRACKET
    {
        /*
         * Declare array
         */
        stlChar  *sAssignExpr;

        sAssignExpr = ztpMakeCAssignmentExpression( aParam,
                                                    @3,
                                                    @5 );

        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        sAssignExpr,
                                                        NULL,
                                                        NULL ) );
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET ZTP_TOK_CKW_STATIC ZTP_TOK_SYM_RBRACKET
    {
        /* parameter list */
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        NULL,
                                                        NULL,
                                                        NULL ) );
        */
        $$ = $1;
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET ZTP_TOK_CKW_STATIC c_assignment_expression ZTP_TOK_SYM_RBRACKET
    {
        /*
         * Declare array
         */
        stlChar  *sAssignExpr;

        sAssignExpr = ztpMakeCAssignmentExpression( aParam,
                                                    @3,
                                                    @5 );

        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        sAssignExpr,
                                                        NULL,
                                                        NULL ) );
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET ZTP_TOK_CKW_STATIC c_type_qualifier_list ZTP_TOK_SYM_RBRACKET
    {
        /* parameter list */
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        NULL,
                                                        NULL,
                                                        NULL ) );
        */
        $$ = $1;
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET ZTP_TOK_CKW_STATIC c_type_qualifier_list c_assignment_expression ZTP_TOK_SYM_RBRACKET
    {
        /*
         * Declare array
         */
        stlChar  *sAssignExpr;

        sAssignExpr = ztpMakeCAssignmentExpression( aParam,
                                                    @3,
                                                    @6 );

        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        sAssignExpr,
                                                        NULL,
                                                        NULL ) );
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET c_type_qualifier_list ZTP_TOK_CKW_STATIC ZTP_TOK_SYM_RBRACKET
    {
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        NULL,
                                                        NULL,
                                                        NULL ) );
        */
        $$ = $1;
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET c_type_qualifier_list ZTP_TOK_CKW_STATIC c_assignment_expression ZTP_TOK_SYM_RBRACKET
    {
        /* function prototype only */
        stlChar  *sAssignExpr;

        sAssignExpr = ztpMakeCAssignmentExpression( aParam,
                                                    @3,
                                                    @6 );

        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        sAssignExpr,
                                                        NULL,
                                                        NULL ) );
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET ZTP_TOK_SYM_ASTERISK ZTP_TOK_SYM_RBRACKET
    {
        // function prototype only
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        NULL,
                                                        NULL,
                                                        NULL ) );
        */
        $$ = $1;
    }
    | c_direct_declarator ZTP_TOK_SYM_LBRACKET c_type_qualifier_list ZTP_TOK_SYM_ASTERISK ZTP_TOK_SYM_RBRACKET
    {
        // function prototype only
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        NULL,
                                                        NULL,
                                                        NULL ) );
    }
    | c_direct_declarator ZTP_TOK_SYM_LPAREN ZTP_TOK_SYM_RPAREN
    {
        /* function only */
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        NULL,
                                                        NULL,
                                                        NULL ) );
        */
        $$ = $1;
    }
    | c_direct_declarator ZTP_TOK_SYM_LPAREN c_identifier_list ZTP_TOK_SYM_RPAREN
    {
        /* function only */
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        NULL,
                                                        $3,
                                                        NULL ) );
    }
    | c_direct_declarator ZTP_TOK_SYM_LPAREN c_parameter_type_list ZTP_TOK_SYM_RPAREN
    {
        /* function only */
        ZTP_PARSE_CHECK( $$ = ztpMakeCDirectDeclarator( aParam,
                                                        NULL,
                                                        NULL,
                                                        $1,
                                                        NULL,
                                                        NULL,
                                                        $3 ) );
    }
    ;

c_pointer
    : ZTP_TOK_SYM_ASTERISK
    | ZTP_TOK_SYM_ASTERISK c_type_qualifier
    | ZTP_TOK_SYM_ASTERISK c_pointer
    | ZTP_TOK_SYM_ASTERISK c_type_qualifier c_pointer
    ;

c_type_qualifier_list
    : c_type_qualifier
    | c_type_qualifier c_type_qualifier_list
    ;

c_parameter_type_list
    : c_parameter_list
    {
        $$ = $1;
    }
    | c_parameter_list ZTP_TOK_SYM_COMMA ZTP_TOK_SYM_ELLIPSIS
    {
        $$ = $1;
    }
    ;

c_parameter_list
    : c_parameter_declaration
    {
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCParameterList( aParam,
                                                     NULL,
                                                     $1 ) );
        */
    }
    | c_parameter_list ZTP_TOK_SYM_COMMA c_parameter_declaration
    {
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCParameterList( aParam,
                                                     $1,
                                                     $3 ) );
        */
    }
    ;

c_parameter_declaration
    : c_declaration_specifiers c_declarator
    {
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCParamDecl( aParam,
                                                 aParam->mDeclarationSpecifiers,
                                                 $2 ) );
        */
        aParam->mCDeclarator = NULL;
    }
    | c_declaration_specifiers
    {
        aParam->mCDeclarator = NULL;
        $$ = NULL;
    }
    | c_declaration_specifiers c_abstract_declarator
    {
        aParam->mCDeclarator = NULL;
        $$ = NULL;
    }
    ;

c_identifier
    : ZTP_TOK_ID
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCIdentifier(aParam, $1) );
    }
    ;

c_identifier_list
    : c_identifier
    {
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCIdentifierList(aParam, NULL, $1) );
        */
    }
    | c_identifier ZTP_TOK_SYM_COMMA c_identifier_list
    {
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCIdentifierList(aParam, $3, $1) );
        */
    }
    ;

c_abstract_declarator
    : c_pointer
    | c_direct_abstract_declarator
    | c_pointer c_direct_abstract_declarator
    ;

c_direct_abstract_declarator
    : ZTP_TOK_SYM_LBRACKET ZTP_TOK_SYM_RBRACKET
    | ZTP_TOK_SYM_LBRACKET c_assignment_expression ZTP_TOK_SYM_RBRACKET
    | ZTP_TOK_SYM_LPAREN c_abstract_declarator ZTP_TOK_SYM_RPAREN
    | c_direct_abstract_declarator ZTP_TOK_SYM_LBRACKET ZTP_TOK_SYM_RBRACKET
    | c_direct_abstract_declarator ZTP_TOK_SYM_LBRACKET c_assignment_expression ZTP_TOK_SYM_RBRACKET
    | ZTP_TOK_SYM_LBRACKET ZTP_TOK_SYM_ASTERISK ZTP_TOK_SYM_RBRACKET
    | c_direct_abstract_declarator ZTP_TOK_SYM_LBRACKET ZTP_TOK_SYM_ASTERISK ZTP_TOK_SYM_RBRACKET
    | ZTP_TOK_SYM_LPAREN c_parameter_type_list ZTP_TOK_SYM_RPAREN
    | c_direct_abstract_declarator ZTP_TOK_SYM_LPAREN c_parameter_type_list ZTP_TOK_SYM_RPAREN
    | ZTP_TOK_SYM_LPAREN ZTP_TOK_SYM_RPAREN
    | c_direct_abstract_declarator ZTP_TOK_SYM_LPAREN ZTP_TOK_SYM_RPAREN
    ;

c_initializer
    : ZTP_TOK_SYM_LBRACE c_initializer_list ZTP_TOK_SYM_RBRACE
    | ZTP_TOK_SYM_LBRACE c_initializer_list ZTP_TOK_SYM_COMMA ZTP_TOK_SYM_RBRACE
    | c_assignment_expression
    ;

c_initializer_list
    : c_designation c_initializer
    | c_initializer
    | c_initializer_list ZTP_TOK_SYM_COMMA c_designation c_initializer
    | c_initializer_list ZTP_TOK_SYM_COMMA c_initializer
    ;

c_designation
    : c_designator_list ZTP_TOK_SYM_ASSIGNMENT
    ;

c_designator_list
    : c_designator
    | c_designator_list c_designator
    ;

c_designator
    : ZTP_TOK_SYM_LBRACKET c_constant_expression ZTP_TOK_SYM_RBRACKET
    | ZTP_TOK_SYM_PERIOD c_identifier_or_keyword
    ;

c_type_specifier
    : ZTP_TOK_CKW_CHAR
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_CHAR,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_CKW_SHORT
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_SHORT,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_CKW_INT
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INT,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_CKW_LONG
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_LONG,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_CKW_FLOAT
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_FLOAT,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_CKW_DOUBLE
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_DOUBLE,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_CKW_SIGNED
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_SIGNED,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_CKW_UNSIGNED
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_UNSIGNED,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | c_derived_type_specifier
    {
        /*
        ZTP_SET_BIT( aParam->mDeclarationSpecifiers, ZTP_SPECIFIER_BIT_DERIVED );
        */
        $$ = $1;
    }
    ;

c_derived_type_specifier
    : ZTP_TOK_EKW_VARCHAR
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_VARCHAR,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_LONGVARCHAR
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_LONGVARCHAR,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_BINARY
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_BINARY,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_VARBINARY
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_VARBINARY,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_LONGVARBINARY
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_LONGVARBINARY,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | number_type
    {
        $$ = $1;
    }
    | ZTP_TOK_EKW_BOOLEAN
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_BOOLEAN,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | date_time_type
    {
        $$ = $1;
    }
    | ZTP_TOK_EKW_INTERVAL interval_type
    {
        $$ = $2;
    }
    | ZTP_TOK_EKW_SQL_CONTEXT
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_SQLCONTEXT,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | c_struct_or_union_specifier
    {
        ztpCVariable  *sCVariable;

        sCVariable = ztpProcessCStructSpecifier( aParam,
                                                 aParam->mIsTypedef,
                                                 $1 );

        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_STRUCT,
                                                        NULL,
                                                        NULL,
                                                        sCVariable,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | c_enum_specifier
    {
        $$ = NULL;
    }
    | ZTP_TOK_TYPEDEF_NAME
    {
        ztpCVariable  *sTypeDefSymbol = NULL;

        ztpcFindTypeDefInSymTab( $1,
                                 &sTypeDefSymbol );

        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_TYPEDEF,
                                                        NULL,
                                                        NULL,
                                                        sTypeDefSymbol,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_CKW_VOID
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_UNKNOWN,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_CKW__BOOL
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_UNKNOWN,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_CKW__COMPLEX
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_UNKNOWN,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_CKW__IMAGINARY
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_UNKNOWN,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    ;

number_type
    : ZTP_TOK_EKW_NUMBER
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_NUMBER,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_NUMBER_LPAREN c_constant ZTP_TOK_SYM_RPAREN
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_NUMBER,
                                                        $2,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_NUMBER_LPAREN c_constant ZTP_TOK_SYM_COMMA c_constant ZTP_TOK_SYM_RPAREN
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_NUMBER,
                                                        $2,
                                                        $4,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    ;

date_time_type
    : ZTP_TOK_EKW_DATE
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_DATE,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_TIME
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_TIME,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_TIME ZTP_TOK_EKW_WITH ZTP_TOK_EKW_TIMEZONE
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_TIMETZ,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_TIMESTAMP
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_TIMESTAMP,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_TIMESTAMP ZTP_TOK_EKW_WITH ZTP_TOK_EKW_TIMEZONE
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_TIMESTAMPTZ,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    ;

interval_type
    : ZTP_TOK_EKW_YEAR
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_YEAR,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_MONTH
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_MONTH,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_DAY
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_DAY,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_HOUR
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_HOUR,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_MINUTE
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_MINUTE,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_SECOND
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_SECOND,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_YEAR ZTP_TOK_EKW_TO ZTP_TOK_EKW_MONTH
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_YEAR_TO_MONTH,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_DAY ZTP_TOK_EKW_TO ZTP_TOK_EKW_HOUR
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_DAY_TO_HOUR,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_DAY ZTP_TOK_EKW_TO ZTP_TOK_EKW_MINUTE
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_DAY_TO_MINUTE,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_DAY ZTP_TOK_EKW_TO ZTP_TOK_EKW_SECOND
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_DAY_TO_SECOND,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_HOUR ZTP_TOK_EKW_TO ZTP_TOK_EKW_MINUTE
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_HOUR_TO_MINUTE,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_HOUR ZTP_TOK_EKW_TO ZTP_TOK_EKW_SECOND
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_HOUR_TO_SECOND,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    | ZTP_TOK_EKW_MINUTE ZTP_TOK_EKW_TO ZTP_TOK_EKW_SECOND
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierNode( aParam,
                                                        ZTP_SPECIFIER_INTERVAL_MINUTE_TO_SECOND,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        @1,
                                                        aParam->mLength ) );
    }
    ;

c_struct_or_union_specifier
    : c_struct_or_union c_tag_name
    {
        /*
         * struct name만 정의
         */
        ZTP_PARSE_CHECK( $$ = ztpMakeCStructSpecifier( aParam,
                                                       $2,
                                                       NULL ) );
    }
    | c_struct_or_union ZTP_TOK_SYM_LBRACE c_struct_declaration_list ZTP_TOK_SYM_RBRACE
    {
        /*
         * tag name 생략: typedef에서만 사용
         */
        ZTP_PARSE_CHECK( $$ = ztpMakeCStructSpecifier( aParam,
                                                       NULL,
                                                       $3 ) );
    }
    | c_struct_or_union c_tag_name ZTP_TOK_SYM_LBRACE c_struct_declaration_list ZTP_TOK_SYM_RBRACE
    {
        /*
         * tag name과 body를 모두 가짐
         * struct 정의에 이용되거나, typedef 모두에 사용될 수 있음
         */
        ZTP_PARSE_CHECK( $$ = ztpMakeCStructSpecifier( aParam,
                                                       $2,
                                                       $4 ) );
    }
    ;

c_struct_or_union
    : ZTP_TOK_CKW_STRUCT
    {
        /*
         * struct나 union이 사용될 때는, 내부에 member를 서술하면서,
         * c_struct_decaration 에서 declaration specifier가 초기화된다.
         * 따라서, 여기서 typedef를 먼저 check 해 놓도록 한다.
         */
        /*
        aParam->mIsTypedef = ZTP_IS_SET_BIT( aParam->mDeclarationSpecifiers,
                                             ZTP_SPECIFIER_BIT_TYPEDEF );
        */
    }
    | ZTP_TOK_CKW_UNION
    {
        /*
         * struct나 union이 사용될 때는, 내부에 member를 서술하면서,
         * c_struct_decaration 에서 declaration specifier가 초기화된다.
         * 따라서, 여기서 typedef를 먼저 check 해 놓도록 한다.
         */
        /*
        aParam->mIsTypedef = ZTP_IS_SET_BIT( aParam->mDeclarationSpecifiers,
                                             ZTP_SPECIFIER_BIT_TYPEDEF );
        */
    }
    ;

c_tag_name
    : ZTP_TOK_ID
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCIdentifier(aParam, $1) );
    }
    | ZTP_TOK_TYPEDEF_NAME
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCIdentifier(aParam, $1) );
    }
    ;

c_struct_declaration_list
    : c_struct_declaration
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCDeclarationList( aParam,
                                                       $1,
                                                       NULL) );
    }
    | c_struct_declaration c_struct_declaration_list
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCDeclarationList( aParam,
                                                       $1,
                                                       $2) );
    }
    ;

c_struct_declaration
    : c_specifier_qualifier_list ZTP_TOK_SYM_SEMICOLON
    {
        $$ = NULL;
    }
    | c_specifier_qualifier_list c_struct_declarator_list ZTP_TOK_SYM_SEMICOLON
    {
        zlplCDataType          sDataType = ZLPL_C_DATATYPE_UNKNOWN;
        ztpTypeSpecifierList  *sTypeSpecifierList = (ztpTypeSpecifierList *)$1;
        ztpCVariable          *sCVariable;
        stlChar               *sPrecision;
        stlChar               *sScale;

        ZTP_PARSE_CHECK( ztpdGetCDataType( aParam,
                                           sTypeSpecifierList,
                                           &sDataType,
                                           &sPrecision,
                                           &sScale,
                                           &sCVariable ) );

        ZTP_PARSE_CHECK( $$ = ztpMakeCDeclaration( aParam,
                                                   @1,
                                                   aParam->mNextLoc,
                                                   aParam->mLineNo,
                                                   sCVariable,
                                                   sDataType,
                                                   sPrecision,
                                                   sScale,
                                                   $2 ) );

    }
    ;

c_specifier_qualifier_list
    : c_specifier_qualifier_list_desc
    {
        $$ = $1;
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeDeclSpec( aParam,
                                               aParam->mDeclarationSpecifiers,
                                               $1 ) );

        aParam->mDeclarationSpecifiers = 0;
        */
    }
    ;

c_specifier_qualifier_list_desc
    : c_type_specifier c_specifier_qualifier_list_desc
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierList( aParam,
                                                        $2,
                                                        $1) );
    }
    | c_type_specifier
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeTypeSpecifierList( aParam,
                                                        NULL,
                                                        $1) );
    }
    | c_type_qualifier c_specifier_qualifier_list_desc
    {
        $$ = $2;
    }
    | c_type_qualifier
    {
        $$ = NULL;
    }
    ;

c_struct_declarator_list
    : c_struct_declarator
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCDeclaratorList( aParam,
                                                      $1,
                                                      NULL) );
    }
    | c_struct_declarator ZTP_TOK_SYM_COMMA c_struct_declarator_list
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCDeclaratorList( aParam,
                                                      $1,
                                                      $3) );
    }
    ;

c_struct_declarator
    : c_declarator
    {
        $$ = $1;
        aParam->mCDeclarator = NULL;
    }
    | ZTP_TOK_SYM_COLON c_constant_expression
    | c_declarator ZTP_TOK_SYM_COLON c_constant_expression
    {
        $$ = $1;
        aParam->mCDeclarator = NULL;
    }
    ;

c_enum_specifier
    : ZTP_TOK_CKW_ENUM c_tag_name
    | ZTP_TOK_CKW_ENUM ZTP_TOK_SYM_LBRACE c_enumerator_list ZTP_TOK_SYM_RBRACE
    | ZTP_TOK_CKW_ENUM ZTP_TOK_SYM_LBRACE c_enumerator_list ZTP_TOK_SYM_COMMA ZTP_TOK_SYM_RBRACE
    | ZTP_TOK_CKW_ENUM c_tag_name ZTP_TOK_SYM_LBRACE c_enumerator_list ZTP_TOK_SYM_RBRACE
    | ZTP_TOK_CKW_ENUM c_tag_name ZTP_TOK_SYM_LBRACE c_enumerator_list ZTP_TOK_SYM_COMMA ZTP_TOK_SYM_RBRACE
    ;

c_enumerator_list
    : c_enumerator
    | c_enumerator_list ZTP_TOK_SYM_COMMA c_enumerator
    ;

c_enumerator
    : c_enumeration_constant
    | c_enumeration_constant ZTP_TOK_SYM_ASSIGNMENT c_constant_expression
    ;

c_enumeration_constant
    : c_identifier
    ;





embedded_sql_include_statement
    : ZTP_TOK_EKW_INCLUDE filename
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExecSqlInclude(aParam, $2));
    }
    ;

embedded_sql_exception_statement
    : ZTP_TOK_EKW_WHENEVER exception_condition exception_action
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExecSqlException(aParam, $2, $3));
    }
    ;

exception_condition
    : ZTP_TOK_EKW_SQLERROR
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExceptionCondition(aParam,
                                                       ZTP_EXCEPTION_COND_SQLERROR,
                                                       ""));
    }
    | ZTP_TOK_EKW_SQLWARNING
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExceptionCondition(aParam,
                                                       ZTP_EXCEPTION_COND_SQLWARNING,
                                                       ""));
    }
    | ZTP_TOK_EKW_NOT ZTP_TOK_EKW_FOUND
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExceptionCondition(aParam,
                                                       ZTP_EXCEPTION_COND_NOT_FOUND,
                                                       ""));
    }
    | ZTP_TOK_SQLSTATE
    {
        char   *sStateCode = (stlChar*)$1 + aParam->mLength - 1;
        while( stlIsspace( *sStateCode ) == STL_FALSE )
        {
            sStateCode --;
        }
        sStateCode ++;

        ZTP_PARSE_CHECK($$ = ztpMakeExceptionCondition(aParam,
                                                       ZTP_EXCEPTION_COND_SQLSTATE,
                                                       sStateCode));
    }
    ;

exception_action
    : ZTP_TOK_CKW_CONTINUE
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExceptionAction(aParam,
                                                    ZTP_EXCEPTION_ACT_CONTINUE,
                                                    NULL));
    }
    | ZTP_TOK_CKW_GOTO ZTP_TOK_ID
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExceptionAction(aParam,
                                                    ZTP_EXCEPTION_ACT_GOTO,
                                                    $2));
    }
    | ZTP_TOK_EKW_STOP
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExceptionAction(aParam,
                                                    ZTP_EXCEPTION_ACT_STOP,
                                                    NULL));
    }
    | ZTP_TOK_CKW_DO c_assignment_expression
    {
        stlChar  *sAssignExpr;

        sAssignExpr = ztpMakeCAssignmentExpression( aParam,
                                                    @2,
                                                    aParam->mCurrLoc );

        ZTP_PARSE_CHECK($$ = ztpMakeExceptionAction( aParam,
                                                     ZTP_EXCEPTION_ACT_DO,
                                                     sAssignExpr ));
    }
    ;

c_primary_expression
    : c_identifier_or_keyword
    | c_constant
    | c_string
    | ZTP_TOK_SYM_LPAREN c_expression ZTP_TOK_SYM_RPAREN
    ;

c_constant
    : ZTP_TOK_LITERAL
    {
        $$ = $1;
    }
    ;

c_string
    : ZTP_TOK_DQ_LITERAL
    | ZTP_TOK_CKW___FUNC__
    ;

c_identifier_or_keyword
    : ZTP_TOK_ID
    | ZTP_TOK_EKW_FREE
    ;

c_postfix_expression
    : c_primary_expression
    | c_postfix_expression ZTP_TOK_SYM_LBRACKET c_expression ZTP_TOK_SYM_RBRACKET
    | c_postfix_expression ZTP_TOK_SYM_LPAREN ZTP_TOK_SYM_RPAREN
    | c_postfix_expression ZTP_TOK_SYM_LPAREN c_argument_expression_list ZTP_TOK_SYM_RPAREN
    | c_postfix_expression ZTP_TOK_SYM_PERIOD c_identifier_or_keyword
    | c_postfix_expression ZTP_TOK_SYM_OP_PTR c_identifier_or_keyword
    | c_postfix_expression ZTP_TOK_SYM_OP_INC
    | c_postfix_expression ZTP_TOK_SYM_OP_DEC
    | ZTP_TOK_SYM_LPAREN c_type_name ZTP_TOK_SYM_RPAREN ZTP_TOK_SYM_LBRACE c_initializer_list ZTP_TOK_SYM_RBRACE
    | ZTP_TOK_SYM_LPAREN c_type_name ZTP_TOK_SYM_RPAREN ZTP_TOK_SYM_LBRACE c_initializer_list ZTP_TOK_SYM_COMMA ZTP_TOK_SYM_RBRACE
    ;

c_argument_expression_list
    : c_assignment_expression
    | c_argument_expression_list ZTP_TOK_SYM_COMMA c_assignment_expression
    ;

c_unary_expression
    : c_postfix_expression
    | ZTP_TOK_SYM_OP_INC c_unary_expression
    | ZTP_TOK_SYM_OP_DEC c_unary_expression
    | c_unary_operator c_cast_expression
    | ZTP_TOK_CKW_SIZEOF c_unary_expression
    | ZTP_TOK_CKW_SIZEOF ZTP_TOK_SYM_LPAREN c_type_name ZTP_TOK_SYM_RPAREN
    | ZTP_TOK_CKW__ALIGNOF ZTP_TOK_SYM_LPAREN c_type_name ZTP_TOK_SYM_RPAREN
    ;

c_unary_operator
    : ZTP_TOK_SYM_AMPERSAND
    | ZTP_TOK_SYM_ASTERISK
    | ZTP_TOK_SYM_PLUS
    | ZTP_TOK_SYM_MINUS
    | ZTP_TOK_SYM_TILDE
    | ZTP_TOK_SYM_EXCLAMATION
    ;

c_cast_expression
    : c_unary_expression
    | ZTP_TOK_SYM_LPAREN c_type_name ZTP_TOK_SYM_RPAREN c_cast_expression
    ;

c_multiplicative_expression
    : c_cast_expression
    | c_multiplicative_expression ZTP_TOK_SYM_ASTERISK c_cast_expression
    | c_multiplicative_expression ZTP_TOK_SYM_SOLIDUS c_cast_expression
    | c_multiplicative_expression ZTP_TOK_SYM_PERCENT c_cast_expression
    ;

c_additive_expression
    : c_multiplicative_expression
    | c_additive_expression ZTP_TOK_SYM_PLUS c_multiplicative_expression
    | c_additive_expression ZTP_TOK_SYM_MINUS c_multiplicative_expression
    ;

c_shift_expression
    : c_additive_expression
    | c_shift_expression ZTP_TOK_SYM_OP_LEFT c_additive_expression
    | c_shift_expression ZTP_TOK_SYM_OP_RIGHT c_additive_expression
    ;

c_relational_expression
    : c_shift_expression
    | c_relational_expression ZTP_TOK_SYM_OP_LT c_shift_expression
    | c_relational_expression ZTP_TOK_SYM_OP_GT c_shift_expression
    | c_relational_expression ZTP_TOK_SYM_OP_LE c_shift_expression
    | c_relational_expression ZTP_TOK_SYM_OP_GE c_shift_expression
    ;

c_equality_expression
    : c_relational_expression
    | c_equality_expression ZTP_TOK_SYM_OP_EQ c_relational_expression
    | c_equality_expression ZTP_TOK_SYM_OP_NE c_relational_expression
    ;

c_and_expression
    : c_equality_expression
    | c_and_expression ZTP_TOK_SYM_AMPERSAND c_equality_expression
    ;

c_exclusive_or_expression
    : c_and_expression
    | c_exclusive_or_expression ZTP_TOK_SYM_CIRCUMFLEX c_and_expression
    ;

c_inclusive_or_expression
    : c_exclusive_or_expression
    | c_inclusive_or_expression ZTP_TOK_SYM_VBAR c_exclusive_or_expression
    ;

c_logical_and_expression
    : c_inclusive_or_expression
    | c_logical_and_expression ZTP_TOK_SYM_OP_AND c_inclusive_or_expression
    ;

c_logical_or_expression
    : c_logical_and_expression
    | c_logical_or_expression ZTP_TOK_SYM_OP_OR c_logical_and_expression
    ;

c_conditional_expression
    : c_logical_or_expression
    | c_logical_or_expression ZTP_TOK_SYM_QUESTION c_expression ZTP_TOK_SYM_COLON c_conditional_expression
    ;

c_assignment_expression
    : c_conditional_expression
    | c_unary_expression c_assignment_operator c_assignment_expression
    ;

c_assignment_operator
    : ZTP_TOK_SYM_ASSIGNMENT
    | ZTP_TOK_SYM_ASSIGN_MUL
    | ZTP_TOK_SYM_ASSIGN_DIV
    | ZTP_TOK_SYM_ASSIGN_MOD
    | ZTP_TOK_SYM_ASSIGN_ADD
    | ZTP_TOK_SYM_ASSIGN_SUB
    | ZTP_TOK_SYM_ASSIGN_LEFT
    | ZTP_TOK_SYM_ASSIGN_RIGHT
    | ZTP_TOK_SYM_ASSIGN_AND
    | ZTP_TOK_SYM_ASSIGN_XOR
    | ZTP_TOK_SYM_ASSIGN_OR
    ;

c_constant_expression
    : c_conditional_expression
    ;

c_expression
    : c_assignment_expression
    | c_expression ZTP_TOK_SYM_COMMA c_assignment_expression
    ;

c_type_name
    : c_specifier_qualifier_list c_abstract_declarator
    | c_specifier_qualifier_list
    ;

embedded_context_statement
    : ZTP_TOK_EKW_CONTEXT ZTP_TOK_EKW_ALLOCATE host_variable_desc
    {
        ztpHostVariable  *sHostVariable = $3;
        ZTP_PARSE_CHECK($$ = ztpMakeSqlContextAction(aParam,
                                                     ZTP_PARSETREE_TYPE_CONTEXT_ALLOCATE,
                                                     ZTP_CONTEXT_ALLOCATE,
                                                     sHostVariable->mHostVarString));
    }
    | ZTP_TOK_EKW_CONTEXT ZTP_TOK_EKW_FREE host_variable_desc
    {
        ztpHostVariable  *sHostVariable = $3;
        ZTP_PARSE_CHECK($$ = ztpMakeSqlContextAction(aParam,
                                                     ZTP_PARSETREE_TYPE_CONTEXT_FREE,
                                                     ZTP_CONTEXT_FREE,
                                                     sHostVariable->mHostVarString));
    }
    | ZTP_TOK_EKW_CONTEXT ZTP_TOK_EKW_USE host_variable_desc
    {
        ztpHostVariable  *sHostVariable = $3;
        ZTP_PARSE_CHECK($$ = ztpMakeSqlContextAction(aParam,
                                                     ZTP_PARSETREE_TYPE_CONTEXT_USE,
                                                     ZTP_CONTEXT_USE,
                                                     sHostVariable->mHostVarString));
    }
    | ZTP_TOK_EKW_CONTEXT ZTP_TOK_EKW_USE ZTP_TOK_CKW_DEFAULT
    {
        ZTP_PARSE_CHECK($$ = ztpMakeSqlContextAction(aParam,
                                                     ZTP_PARSETREE_TYPE_CONTEXT_USE_DEFAULT,
                                                     ZTP_CONTEXT_USE_DEFAULT,
                                                     ""));
    }
    ;

host_variable_desc
    : ZTP_TOK_SYM_COLON /* token_stream */
    {
        stlInt32  sEndPos = 0;
        ZTP_PARSE_CHECK( $$ = ztpMakeHostVariableInC( aParam,
                                                      @1,
                                                      &sEndPos) );
        yyclearin;
        while( aParam->mNextLoc < sEndPos )
        {
            YYLEX;
        }
    }
    ;

embedded_autocommit_statement
    : embedded_conn_string ZTP_TOK_EKW_AUTOCOMMIT ZTP_TOK_EKW_ON
    {
        aParam->mConnStr = $1;
        ZTP_PARSE_CHECK($$ = ztpMakeAutoCommit( aParam,
                                                STL_TRUE ) );
    }
    | embedded_conn_string ZTP_TOK_EKW_AUTOCOMMIT ZTP_TOK_EKW_OFF
    {
        aParam->mConnStr = $1;
        ZTP_PARSE_CHECK($$ = ztpMakeAutoCommit( aParam,
                                                STL_FALSE ) );
    }
    ;

embedded_option_statement
    : ZTP_TOK_EKW_OPTION ZTP_TOK_SYM_LPAREN option_desc ZTP_TOK_SYM_RPAREN
    ;

option_desc
    : ZTP_TOK_EKW_INCLUDE ZTP_TOK_SYM_ASSIGNMENT include_dir_list
    ;

filename
    : /* empty */
    {
        stlInt32  sEndPos = 0;

        ZTP_PARSE_CHECK($$ = ztpMakeIncludeFile( aParam,
                                                 aParam->mNextLoc,
                                                 &sEndPos ) );

        yyclearin;
        while( aParam->mNextLoc < sEndPos )
        {
            YYLEX;
        }
    }
    ;

include_dir_list
    : /* empty */
    {
        stlInt32  sEndPos = 0;

        ZTP_PARSE_CHECK($$ = ztpMakePathList( aParam,
                                              aParam->mNextLoc,
                                              &sEndPos ) );

        yyclearin;
        while( aParam->mNextLoc < sEndPos )
        {
            YYLEX;
        }
    }
    ;

%%
