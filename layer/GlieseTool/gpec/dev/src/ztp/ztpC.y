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
#include <ztpDef.h>
#include <ztpCParseFunc.h>
#include <ztpConvertExecSql.h>
#include <ztpdType.h>
#include <ztpdConvert.h>
#include <ztpMisc.h>
#include <ztpC.tab.h>

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
ZTP_TOK_C_DUMMY
ZTP_TOK_C_PP_DIRECTIVE

ZTP_TOK_C_PP_ELIF
ZTP_TOK_C_PP_ELSE
ZTP_TOK_C_PP_ENDIF
ZTP_TOK_C_PP_IF
ZTP_TOK_C_PP_IFDEF
ZTP_TOK_C_PP_IFNDEF
ZTP_TOK_C_PP_INCLUDE
ZTP_TOK_C_PP_DEFINE
ZTP_TOK_C_PP_UNDEF

ZTP_TOK_CKW___FUNC__
ZTP_TOK_CKW__ALIGNOF
ZTP_TOK_CKW__BOOL
ZTP_TOK_CKW__COMPLEX
ZTP_TOK_CKW__IMAGINARY
ZTP_TOK_CKW_AUTO
ZTP_TOK_CKW_BREAK
ZTP_TOK_CKW_CASE
ZTP_TOK_CKW_CHAR
ZTP_TOK_CKW_CONST
ZTP_TOK_CKW_CONTINUE
ZTP_TOK_CKW_DEFAULT
ZTP_TOK_CKW_DO
ZTP_TOK_CKW_DOUBLE
ZTP_TOK_CKW_ELSE
ZTP_TOK_CKW_ENUM
ZTP_TOK_CKW_EXTERN
ZTP_TOK_CKW_FLOAT
ZTP_TOK_CKW_FOR
ZTP_TOK_CKW_GOTO
ZTP_TOK_CKW_IF
ZTP_TOK_CKW_INLINE
ZTP_TOK_CKW_INT
ZTP_TOK_CKW_LONG
ZTP_TOK_CKW_REGISTER
ZTP_TOK_CKW_RESTRICT
ZTP_TOK_CKW_RETURN
ZTP_TOK_CKW_SHORT
ZTP_TOK_CKW_SIGNED
ZTP_TOK_CKW_SIZEOF
ZTP_TOK_CKW_STATIC
ZTP_TOK_CKW_STRUCT
ZTP_TOK_CKW_SWITCH
ZTP_TOK_CKW_TYPEDEF
ZTP_TOK_CKW_UNION
ZTP_TOK_CKW_UNSIGNED
ZTP_TOK_CKW_VOID
ZTP_TOK_CKW_VOLATILE
ZTP_TOK_CKW_WHILE

/* 임시 코드 : 삭제해야 함 */
ZTP_TOK_CKW_FILE
ZTP_TOK_CKW_CLOCK_T
ZTP_TOK_CKW_KEY_T
ZTP_TOK_CKW_SEM_T
ZTP_TOK_CKW_SIZE_T
ZTP_TOK_CKW_SSIZE_T
ZTP_TOK_CKW_OFF_T
ZTP_TOK_CKW_PID_T
ZTP_TOK_CKW_PTHREAD_T
ZTP_TOK_EKW_SQLLEN
/***********************/

ZTP_TOK_EKW_ALLOCATE
ZTP_TOK_EKW_BEGIN_DECL_SEC
ZTP_TOK_EKW_BY
ZTP_TOK_EKW_CONTEXT
ZTP_TOK_EKW_DATE
ZTP_TOK_EKW_END_DECL_SEC
ZTP_TOK_EKW_EXEC_SQL
ZTP_TOK_EKW_FOUND
ZTP_TOK_EKW_FREE
ZTP_TOK_EKW_INCLUDE
ZTP_TOK_EKW_NOT
ZTP_TOK_EKW_SQL_CONTEXT
ZTP_TOK_EKW_SQL_TYPE_IS
ZTP_TOK_EKW_SQLERROR
ZTP_TOK_EKW_SQLSTATE
ZTP_TOK_EKW_SQLWARNING
ZTP_TOK_EKW_STOP
ZTP_TOK_EKW_USE
ZTP_TOK_EKW_VARCHAR
ZTP_TOK_EKW_WHENEVER

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

ZTP_TOK_SYM_SINGLE_QUOTE
ZTP_TOK_SYM_DOUBLE_QUOTE

ZTP_TOK_ID
ZTP_TOK_TYPEDEF_NAME
ZTP_TOK_LITERAL
ZTP_TOK_DQ_LITERAL
ZTP_TOK_INCLUDE_FILE

%start translation_unit
%nonassoc ZTP_TOK_CKW_IF
%nonassoc ZTP_TOK_CKW_ELSE
%debug
%%

/********************************************************************
 * Phrase structure grammar
 ********************************************************************/

translation_unit
    :
    {
        aParam->mCCodeEndLoc = aParam->mFileLength;
        ZTP_PARSE_CHECK(ztpBypassCCode(aParam));
    }
    | external_declaration translation_unit
    ;

external_declaration
    : c_function_definition
    | c_declaration
    {
        aParam->mCCodeEndLoc = @1;
        ZTP_PARSE_CHECK(ztpBypassCCode(aParam));

        ztpProcessCDeclaration( aParam,
                                $1,
                                STL_FALSE );

        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    | exec_sql_statement
    | c_pp_directive
    | ZTP_TOK_C_COMMENT
    ;

c_function_definition
    : c_function_header c_function_body
    ;

c_function_header
    : c_declaration_specifiers c_declarator c_declaration_list
    {
        /*
        aParam->mDeclarationSpecifiers = 0;
        */
        aParam->mCDeclarator = NULL;
    }
    | c_declaration_specifiers c_declarator
    {
        /*
        aParam->mDeclarationSpecifiers = 0;
        */
        aParam->mCDeclarator = NULL;
    }
    ;

c_function_body
    : c_compound_statement
    ;

c_compound_statement
    : ZTP_TOK_SYM_LBRACE ZTP_TOK_SYM_RBRACE
    | ZTP_TOK_SYM_LBRACE
    {
        ZTP_PARSE_CHECK(ztpIncSymTabDepth(aParam));
    }
    c_block_item_list
    ZTP_TOK_SYM_RBRACE
    {
        ZTP_PARSE_CHECK(ztpDecSymTabDepth(aParam));
    }
    ;

c_block_item_list
    : c_block_item
    | c_block_item c_block_item_list
    ;

c_block_item
    : c_declaration
    {
        aParam->mCCodeEndLoc = @1;
        ZTP_PARSE_CHECK(ztpBypassCCode(aParam));

        ztpProcessCDeclaration( aParam,
                                $1,
                                STL_FALSE );

        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    | c_statement
    | c_pp_directive
    ;

c_statement
    : c_labeled_statement
    | c_compound_statement
    | c_expression_statement
    | c_selection_statement
    | c_iteration_statement
    | c_jump_statement
    | exec_sql_statement
    ;

c_labeled_statement
    : ZTP_TOK_ID ZTP_TOK_SYM_COLON c_statement
    | ZTP_TOK_CKW_CASE c_constant_expression ZTP_TOK_SYM_COLON c_statement
    | ZTP_TOK_CKW_DEFAULT ZTP_TOK_SYM_COLON c_statement
    ;

c_expression_statement
    : c_expression ZTP_TOK_SYM_SEMICOLON
    | ZTP_TOK_SYM_SEMICOLON
    ;

c_selection_statement
    : ZTP_TOK_CKW_IF ZTP_TOK_SYM_LPAREN c_expression ZTP_TOK_SYM_RPAREN c_statement %prec ZTP_TOK_CKW_IF
    | ZTP_TOK_CKW_IF ZTP_TOK_SYM_LPAREN c_expression ZTP_TOK_SYM_RPAREN c_statement ZTP_TOK_CKW_ELSE c_statement
    | ZTP_TOK_CKW_SWITCH ZTP_TOK_SYM_LPAREN c_expression ZTP_TOK_SYM_RPAREN c_statement
    ;

c_iteration_statement
    : ZTP_TOK_CKW_WHILE ZTP_TOK_SYM_LPAREN c_expression ZTP_TOK_SYM_RPAREN c_statement
    | ZTP_TOK_CKW_DO c_statement ZTP_TOK_CKW_WHILE ZTP_TOK_SYM_LPAREN c_expression ZTP_TOK_SYM_RPAREN ZTP_TOK_SYM_SEMICOLON
    | ZTP_TOK_CKW_FOR ZTP_TOK_SYM_LPAREN c_expression_statement c_expression_statement ZTP_TOK_SYM_RPAREN c_statement
    | ZTP_TOK_CKW_FOR ZTP_TOK_SYM_LPAREN c_expression_statement c_expression_statement c_expression ZTP_TOK_SYM_RPAREN c_statement
    | ZTP_TOK_CKW_FOR ZTP_TOK_SYM_LPAREN c_declaration c_expression_statement ZTP_TOK_SYM_RPAREN c_statement
    {
        ztpProcessCDeclaration( aParam,
                                $3,
                                STL_TRUE);
    }
    | ZTP_TOK_CKW_FOR ZTP_TOK_SYM_LPAREN c_declaration c_expression_statement c_expression ZTP_TOK_SYM_RPAREN c_statement
    {
        ztpProcessCDeclaration( aParam,
                                $3,
                                STL_TRUE);
    }
    ;

c_jump_statement
    : ZTP_TOK_CKW_GOTO ZTP_TOK_ID ZTP_TOK_SYM_SEMICOLON
    | ZTP_TOK_CKW_CONTINUE ZTP_TOK_SYM_SEMICOLON
    | ZTP_TOK_CKW_BREAK ZTP_TOK_SYM_SEMICOLON
    | ZTP_TOK_CKW_RETURN ZTP_TOK_SYM_SEMICOLON
    | ZTP_TOK_CKW_RETURN c_expression ZTP_TOK_SYM_SEMICOLON
    ;


exec_sql_statement
    : ZTP_TOK_EKW_EXEC_SQL sql_stmt ZTP_TOK_SYM_SEMICOLON
    {
        stlInt32   sEndPos;

        aParam->mCCodeEndLoc = @1;
        ZTP_PARSE_CHECK(ztpBypassCCode(aParam));

        /*
        ZTP_PARSE_CHECK(ztpPrecompSourceLine(aParam, aParam->mLineNo,
                                             aParam->mInFileName, aParam->mOutFileName));
        */
        /* EXEC SQL로 지정된 구문을 C code로 변환 */
        if($2 != NULL)
        {
            if($2 == ZTP_EMPTY_PARSE_TREE)
            {
                /* Gliese common parser (SQL) used */
                if ( ztpMakeExecSqlStmtToC(aParam, @2, &sEndPos) != NULL )
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

            /* EXEC SQL 구문을 주석 처리하여 출력 */
            ZTP_PARSE_CHECK(ztpSendStringToOutFile(aParam, "/* "));
            ZTP_PARSE_CHECK(ztpBypassCCodeByLoc(aParam, @1, sEndPos));
            ZTP_PARSE_CHECK(ztpSendStringToOutFile(aParam, " */\n"));

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

        yyclearin;
        while( aParam->mNextLoc <= sEndPos )
        {
            YYLEX;
        }

        ZTP_PARSE_CHECK(ztpPrecompSourceLine(aParam, aParam->mLineNo,
                                             aParam->mInFileName, aParam->mOutFileName));

        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    ;

sql_stmt
    : sql_token sql_stmt
    {
        $$ = ZTP_EMPTY_PARSE_TREE;
    }
    | embedded_sql_declare_section
    {
        $$ = $1;
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
    | /* empty */
    ;

embedded_sql_declare_section
    : ZTP_TOK_EKW_BEGIN_DECL_SEC
    {
        $$ = NULL;
    }
    | ZTP_TOK_EKW_END_DECL_SEC
    {
        $$ = NULL;
    }
    ;

c_declaration_list
    : c_declaration c_declaration_list
    {
        aParam->mCCodeEndLoc = @1;
        ZTP_PARSE_CHECK(ztpBypassCCode(aParam));

        ztpProcessCDeclaration( aParam,
                                $1,
                                STL_FALSE );

        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    | c_declaration
    {
        aParam->mCCodeEndLoc = @1;
        ZTP_PARSE_CHECK(ztpBypassCCode(aParam));

        ztpProcessCDeclaration( aParam,
                                $1,
                                STL_FALSE );

        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    ;

c_declaration
    : c_declaration_specifiers ZTP_TOK_SYM_SEMICOLON
    {
        $$ = NULL;
    }
    | c_declaration_specifiers c_init_declarator_list ZTP_TOK_SYM_SEMICOLON
    {
        /*
        ztpDeclSpec  *sDeclSpec = (ztpDeclSpec *)$1;
        stlInt64      sFlag     = sDeclSpec->mDeclarationSpecifiers;

        ZTP_PARSE_CHECK( $$ = ztpMakeCDeclaration( aParam,
                                                   @1,
                                                   aParam->mNextLoc,
                                                   aParam->mLineNo,
                                                   sDeclSpec,
                                                   ztpdGetCDataType(sFlag),
                                                   $2 ) );
        */
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
    }
    ;

c_declaration_specifiers_desc
    : c_storage_class_specifier
    {
        $$ = $1;
    }
    | c_storage_class_specifier c_declaration_specifiers_desc
    {
        $$ = $1;
    }
    | c_type_qualifier
    {
        $$ = $1;
    }
    | c_type_qualifier c_declaration_specifiers_desc
    {
        $$ = $1;
    }
    | c_function_specifier
    {
        $$ = $1;
    }
    | c_function_specifier c_declaration_specifiers_desc
    {
        $$ = $1;
    }
    | c_type_specifier
    {
        $$ = $1;
    }
    | c_type_specifier c_declaration_specifiers_desc
    {
        $$ = $1;
    }
    ;

c_storage_class_specifier
    : ZTP_TOK_CKW_AUTO
    {
        $$ = NULL;
    }
    | ZTP_TOK_CKW_EXTERN
    {
        $$ = NULL;
    }
    | ZTP_TOK_CKW_STATIC
    {
        $$ = NULL;
    }
    | ZTP_TOK_CKW_REGISTER
    {
        $$ = NULL;
    }
    | ZTP_TOK_CKW_TYPEDEF
    {
        /*
        ZTP_SET_BIT( aParam->mDeclarationSpecifiers, ZTP_SPECIFIER_BIT_TYPEDEF );
        $$ = NULL;
        */
    }
    ;

c_type_qualifier
    : ZTP_TOK_CKW_CONST
    {
        $$ = NULL;
    }
    | ZTP_TOK_CKW_RESTRICT
    {
        $$ = NULL;
    }
    | ZTP_TOK_CKW_VOLATILE
    {
        $$ = NULL;
    }
    ;

c_function_specifier
    : ZTP_TOK_CKW_INLINE
    {
        $$ = NULL;
    }
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
        ZTP_PARSE_CHECK( $$ = ztpMakeCParameterList( aParam,
                                                     NULL,
                                                     $1 ) );
    }
    | c_parameter_list ZTP_TOK_SYM_COMMA c_parameter_declaration
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCParameterList( aParam,
                                                     $1,
                                                     $3 ) );
    }
    ;

c_parameter_declaration
    : c_declaration_specifiers c_declarator
    {
        /*
        ztpCDeclaration        *sCDeclaration;
        ztpCDeclaratorList *sInitDeclaratorList;

        aParam->mCDeclarator = NULL;

        ZTP_PARSE_CHECK(sInitDeclaratorList = ztpMakeCDeclaratorList(aParam, $2));

        ZTP_PARSE_CHECK(sCDeclaration = ztpMakeCDeclaration(aParam, @1, aParam->mNextLoc, aParam->mLineNo, ztpdGetCDataType(aParam->mDeclarationSpecifiers), sInitDeclaratorList));

        aParam->mCDeclaratorList = NULL;

        ztpAddHostVariableToSymTab(aParam, sCDeclaration, STL_TRUE);
        aParam->mDeclarationSpecifiers = 0;
        */
        /*
        ZTP_PARSE_CHECK( $$ = ztpMakeCParamDecl( aParam,
                                                 aParam->mDeclarationSpecifiers,
                                                 $2 ) );
        aParam->mDeclarationSpecifiers = 0;
        aParam->mCDeclarator = NULL;
        */
    }
    | c_declaration_specifiers
    {
        /*
        aParam->mDeclarationSpecifiers = 0;
        */
        aParam->mCDeclarator = NULL;
        $$ = NULL;
    }
    | c_declaration_specifiers c_abstract_declarator
    {
        /*
        aParam->mDeclarationSpecifiers = 0;
        */
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
        ZTP_PARSE_CHECK( $$ = ztpMakeCIdentifierList(aParam, NULL, $1) );
    }
    | c_identifier ZTP_TOK_SYM_COMMA c_identifier_list
    {
        ZTP_PARSE_CHECK( $$ = ztpMakeCIdentifierList(aParam, $3, $1) );
    }
    ;

c_abstract_declarator
    : c_pointer
    | c_direct_abstract_declarator
    | c_pointer c_direct_abstract_declarator
    ;

c_direct_abstract_declarator
    : ZTP_TOK_SYM_LPAREN c_abstract_declarator ZTP_TOK_SYM_RPAREN
    | ZTP_TOK_SYM_LBRACKET ZTP_TOK_SYM_RBRACKET
    | ZTP_TOK_SYM_LBRACKET c_assignment_expression ZTP_TOK_SYM_RBRACKET
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
    | ZTP_TOK_CKW_SHORT
    | ZTP_TOK_CKW_INT
    | ZTP_TOK_CKW_LONG
    | ZTP_TOK_CKW_FLOAT
    | ZTP_TOK_CKW_DOUBLE
    | ZTP_TOK_CKW_SIGNED
    | ZTP_TOK_CKW_UNSIGNED
    | c_derived_type_specifier
    {
        $$ = $1;
    }
    ;

c_derived_type_specifier
    : ZTP_TOK_EKW_VARCHAR
    | ZTP_TOK_EKW_DATE
    | ZTP_TOK_EKW_SQL_CONTEXT
    | c_struct_or_union_specifier
    | c_enum_specifier
    | ZTP_TOK_TYPEDEF_NAME
    | ZTP_TOK_CKW_VOID
    | ZTP_TOK_CKW__BOOL
    | ZTP_TOK_CKW__COMPLEX
    | ZTP_TOK_CKW__IMAGINARY
    /* Exception : 임시 코드, 추후에 제거해야만 함 */
    | ZTP_TOK_CKW_FILE
    | ZTP_TOK_CKW_CLOCK_T
    | ZTP_TOK_CKW_KEY_T
    | ZTP_TOK_CKW_SEM_T
    | ZTP_TOK_CKW_SIZE_T
    | ZTP_TOK_CKW_SSIZE_T
    | ZTP_TOK_CKW_OFF_T
    | ZTP_TOK_CKW_PID_T
    | ZTP_TOK_CKW_PTHREAD_T
    | ZTP_TOK_EKW_SQLLEN
    ;

c_struct_or_union_specifier
    : c_struct_or_union c_tag_name
    | c_struct_or_union ZTP_TOK_SYM_LBRACE c_struct_declaration_list ZTP_TOK_SYM_RBRACE
    | c_struct_or_union c_tag_name ZTP_TOK_SYM_LBRACE c_struct_declaration_list ZTP_TOK_SYM_RBRACE
    ;

c_struct_or_union
    : ZTP_TOK_CKW_STRUCT
    | ZTP_TOK_CKW_UNION
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
    | c_struct_declaration c_struct_declaration_list
    ;

c_struct_declaration
    : c_specifier_qualifier_list ZTP_TOK_SYM_SEMICOLON
    | c_specifier_qualifier_list c_struct_declarator_list ZTP_TOK_SYM_SEMICOLON
    ;

c_specifier_qualifier_list
    : c_type_specifier c_specifier_qualifier_list
    | c_type_specifier
    | c_type_qualifier c_specifier_qualifier_list
    | c_type_qualifier
    ;

c_struct_declarator_list
    : c_struct_declarator
    | c_struct_declarator_list ZTP_TOK_SYM_COMMA c_struct_declarator
    ;

c_struct_declarator
    : c_declarator
    {
        aParam->mCDeclarator = NULL;
    }
    | ZTP_TOK_SYM_COLON c_constant_expression
    | c_declarator ZTP_TOK_SYM_COLON c_constant_expression
    {
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
        ZTP_PARSE_CHECK($$ = ztpMakeExecSqlInclude(aParam, aParam->mTempBuffer));
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
    | ZTP_TOK_EKW_SQLSTATE ZTP_TOK_C_DUMMY
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExceptionCondition(aParam,
                                                       ZTP_EXCEPTION_COND_SQLSTATE,
                                                       $2));
    }
    | ZTP_TOK_EKW_SQLSTATE ZTP_TOK_ID
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExceptionCondition(aParam,
                                                       ZTP_EXCEPTION_COND_SQLSTATE,
                                                       $2));
    }
    ;

exception_action
    : ZTP_TOK_CKW_CONTINUE
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExceptionAction(aParam,
                                                    ZTP_EXCEPTION_ACT_CONTINUE,
                                                    ""));
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
                                                    ""));
    }
    | ZTP_TOK_CKW_DO token_stream
    {
        ZTP_PARSE_CHECK($$ = ztpMakeExceptionAction(aParam,
                                                    ZTP_EXCEPTION_ACT_DO,
                                                    aParam->mTempBuffer));
    }
    ;

token_stream
    : token_stream_element token_stream
    {
        ZTP_PARSE_CHECK($$ = ztpMakeTokenStream(aParam,
                                                @1,
                                                @2,
                                                STL_FALSE));
    }
    | token_stream_element
    {
        ZTP_PARSE_CHECK($$ = ztpMakeTokenStream(aParam,
                                                @1,
                                                @1 + (stlInt32)stlStrlen($1),
                                                STL_TRUE));
    }
    ;

token_stream_element
    : c_identifier_or_keyword
    | ZTP_TOK_C_DUMMY
    | ZTP_TOK_C_COMMENT
    | ZTP_TOK_SYM_LBRACKET
    | ZTP_TOK_SYM_RBRACKET
    | ZTP_TOK_SYM_LBRACE
    | ZTP_TOK_SYM_RBRACE
    | ZTP_TOK_DQ_LITERAL
    | ZTP_TOK_SYM_COMMA
    ;

c_primary_expression
    : c_identifier_or_keyword
    | ZTP_TOK_C_DUMMY
    | c_constant
    | c_string
    | ZTP_TOK_SYM_LPAREN c_expression ZTP_TOK_SYM_RPAREN
    ;

c_constant
    : ZTP_TOK_LITERAL
    ;

c_string
    : ZTP_TOK_DQ_LITERAL
    | ZTP_TOK_CKW___FUNC__
    ;

c_identifier_or_keyword
    : ZTP_TOK_ID
    | ZTP_TOK_EKW_SQLSTATE
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
        ZTP_PARSE_CHECK($$ = ztpMakeSqlContextAction(aParam,
                                                     ZTP_PARSETREE_TYPE_CONTEXT_ALLOCATE,
                                                     ZTP_CONTEXT_ALLOCATE,
                                                     aParam->mTempBuffer));
    }
    | ZTP_TOK_EKW_CONTEXT ZTP_TOK_EKW_FREE host_variable_desc
    {
        ZTP_PARSE_CHECK($$ = ztpMakeSqlContextAction(aParam,
                                                     ZTP_PARSETREE_TYPE_CONTEXT_FREE,
                                                     ZTP_CONTEXT_FREE,
                                                     aParam->mTempBuffer));
    }
    | ZTP_TOK_EKW_CONTEXT ZTP_TOK_EKW_USE host_variable_desc
    {
        ZTP_PARSE_CHECK($$ = ztpMakeSqlContextAction(aParam,
                                                     ZTP_PARSETREE_TYPE_CONTEXT_USE,
                                                     ZTP_CONTEXT_USE,
                                                     aParam->mTempBuffer));
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
    : ZTP_TOK_SYM_COLON token_stream
    ;

sql_token
    : ZTP_TOK_C_DUMMY
    | ZTP_TOK_C_COMMENT
    | ZTP_TOK_ID
    | ZTP_TOK_CKW_CASE
    | ZTP_TOK_CKW_CHAR
    | ZTP_TOK_CKW_CONTINUE
    | ZTP_TOK_CKW_DEFAULT
    | ZTP_TOK_CKW_DOUBLE
    | ZTP_TOK_CKW_ELSE
    | ZTP_TOK_CKW_FILE
    | ZTP_TOK_CKW_FLOAT
    | ZTP_TOK_CKW_FOR
    | ZTP_TOK_CKW_GOTO
    | ZTP_TOK_CKW_IF
    | ZTP_TOK_CKW_INT
    | ZTP_TOK_CKW_LONG
    | ZTP_TOK_CKW_REGISTER
    | ZTP_TOK_CKW_RESTRICT
    | ZTP_TOK_CKW_RETURN
    | ZTP_TOK_CKW_STATIC
    | ZTP_TOK_CKW_SWITCH
    | ZTP_TOK_CKW_UNION
    | ZTP_TOK_LITERAL
    | ZTP_TOK_DQ_LITERAL
    | ZTP_TOK_EKW_ALLOCATE
    | ZTP_TOK_EKW_BY
    | ZTP_TOK_EKW_DATE
    | ZTP_TOK_EKW_FOUND
    | ZTP_TOK_EKW_FREE
    | ZTP_TOK_EKW_NOT
    | ZTP_TOK_EKW_SQL_CONTEXT
    | ZTP_TOK_EKW_SQLERROR
    | ZTP_TOK_EKW_SQLLEN
    | ZTP_TOK_EKW_SQLSTATE
    | ZTP_TOK_EKW_SQLWARNING
    | ZTP_TOK_EKW_STOP
    | ZTP_TOK_EKW_USE
    | ZTP_TOK_EKW_VARCHAR
    | ZTP_TOK_SYM_ASSIGNMENT
    | ZTP_TOK_SYM_COLON
    | ZTP_TOK_SYM_COMMA
    | ZTP_TOK_SYM_LPAREN
    | ZTP_TOK_SYM_RPAREN
    | ZTP_TOK_SYM_LBRACKET
    | ZTP_TOK_SYM_RBRACKET
    | ZTP_TOK_SYM_OP_LT
    | ZTP_TOK_SYM_OP_GT
    | ZTP_TOK_SYM_OP_LE
    | ZTP_TOK_SYM_OP_GE
    | ZTP_TOK_SYM_OP_OR
    | ZTP_TOK_SYM_PLUS
    | ZTP_TOK_SYM_MINUS
    | ZTP_TOK_SYM_ASTERISK
    | ZTP_TOK_SYM_PERIOD
    | ZTP_TOK_SYM_SOLIDUS
    | ZTP_TOK_SYM_SINGLE_QUOTE
    | ZTP_TOK_SYM_DOUBLE_QUOTE
    ;

filename
    : filename_element filename
    {
        ZTP_PARSE_CHECK($$ = ztpMakeTokenStream(aParam,
                                                @1,
                                                @2,
                                                STL_FALSE));
    }
    | filename_element
    {
        ZTP_PARSE_CHECK($$ = ztpMakeTokenStream(aParam,
                                                @1,
                                                @1 + (stlInt32)stlStrlen($1),
                                                STL_TRUE));
    }
    ;

filename_element
    : ZTP_TOK_ID
    | ZTP_TOK_C_DUMMY
    | ZTP_TOK_SYM_PERIOD
    ;

c_pp_directive
    : ZTP_TOK_C_PP_DIRECTIVE
    | ZTP_TOK_C_PP_ELIF
    {
        stlInt32  sToken;

        sToken = ztpGetNextToken;
        while( sToken != ZTP_TOK_C_PP_ENDIF )
        {
            sToken = ztpGetNextToken;
        }
    }
    | ZTP_TOK_C_PP_ELSE
    | ZTP_TOK_C_PP_ENDIF
    | ZTP_TOK_C_PP_IF
    {
        stlInt32  sToken;

        sToken = ztpGetNextToken;
        while( sToken != ZTP_TOK_C_PP_ENDIF )
        {
            sToken = ztpGetNextToken;
        }
    }
    | ZTP_TOK_C_PP_IFDEF
    {
        stlInt32  sToken;

        sToken = ztpGetNextToken;
        while( sToken != ZTP_TOK_C_PP_ENDIF )
        {
            sToken = ztpGetNextToken;
        }
    }
    | ZTP_TOK_C_PP_IFNDEF
    {
        stlInt32  sToken;

        sToken = ztpGetNextToken;
        while( sToken != ZTP_TOK_C_PP_ENDIF )
        {
            sToken = ztpGetNextToken;
        }
    }
    | ZTP_TOK_C_PP_INCLUDE ZTP_TOK_INCLUDE_FILE
    {
        if( ztpProcessCInclude(aParam, $2) != STL_SUCCESS )
        {
            YYABORT;
        }
    }
    | ZTP_TOK_C_PP_INCLUDE ZTP_TOK_DQ_LITERAL
    {
        if( ztpProcessCInclude(aParam, $2) != STL_SUCCESS )
        {
            YYABORT;
        }
    }
    | ZTP_TOK_C_PP_DEFINE
    | ZTP_TOK_C_PP_UNDEF
    ;

%%
