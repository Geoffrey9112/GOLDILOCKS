/*******************************************************************************
 * ztppCalc.y
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

#include <stl.h>
#include <dtl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztpDef.h>
#include <ztpMisc.h>
#include <ztppCommon.h>
#include <ztppCalcFunc.h>
#include <ztppFunc.h>
#include <ztppEval.h>
#include <ztppCalc.tab.h>

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
#define ZTPP_PARSE_CHECK( aInstruction )                \
    {                                                   \
        aInstruction;                                   \
        if( aParam->mErrorStatus != STL_SUCCESS )       \
        {                                               \
            YYABORT;                                    \
        }                                               \
    }
#else
#define ZTPP_PARSE_CHECK( aInstruction )   ;
#endif

%}

%pure-parser
%locations

%parse-param { ztpCalcParseParam  * aParam  }
%parse-param { void               * aScanner }
%lex-param   { yyscan_t           * aScanner }

%token
ZTPP_TOK_DEFINED
ZTPP_TOK_LITERAL
ZTPP_TOK_ID
ZTPP_TOK_DEFINED_MACRO
ZTPP_TOK_H_STRING
ZTPP_TOK_Q_STRING

ZTPP_TOK_SYM_LPAREN
ZTPP_TOK_SYM_RPAREN
ZTPP_TOK_SYM_LBRACE
ZTPP_TOK_SYM_RBRACE
ZTPP_TOK_SYM_LBRACKET
ZTPP_TOK_SYM_RBRACKET

ZTPP_TOK_SYM_SHARP
ZTPP_TOK_SYM_DOUBLE_SHARP
ZTPP_TOK_SYM_NEW_LINE
ZTPP_TOK_SYM_ELLIPSIS
ZTPP_TOK_SYM_COMMA
ZTPP_TOK_SYM_QUESTION
ZTPP_TOK_SYM_COLON
ZTPP_TOK_SYM_AMPERSAND
ZTPP_TOK_SYM_ASTERISK
ZTPP_TOK_SYM_PLUS
ZTPP_TOK_SYM_MINUS
ZTPP_TOK_SYM_TILDE
ZTPP_TOK_SYM_EXCLAMATION
ZTPP_TOK_SYM_CIRCUMFLEX
ZTPP_TOK_SYM_SOLIDUS
ZTPP_TOK_SYM_PERCENT
ZTPP_TOK_SYM_SL
ZTPP_TOK_SYM_SR
ZTPP_TOK_SYM_LT
ZTPP_TOK_SYM_GT
ZTPP_TOK_SYM_LE
ZTPP_TOK_SYM_GE
ZTPP_TOK_SYM_EQ
ZTPP_TOK_SYM_NE
ZTPP_TOK_SYM_VBAR
ZTPP_TOK_SYM_AND
ZTPP_TOK_SYM_OR
ZTPP_TOK_SYM_PERIOD
ZTPP_TOK_SYM_PTR
ZTPP_TOK_SYM_INC
ZTPP_TOK_SYM_DEC

ZTPP_TOK_SYM_SEMICOLON
ZTPP_TOK_SYM_ASSIGNMENT
ZTPP_TOK_SYM_ASTERISK_ASSIGN
ZTPP_TOK_SYM_SOLIDUS_ASSIGN
ZTPP_TOK_SYM_PERCENT_ASSIGN
ZTPP_TOK_SYM_PLUS_ASSIGN
ZTPP_TOK_SYM_MINUS_ASSIGN
ZTPP_TOK_SYM_SR_ASSIGN
ZTPP_TOK_SYM_SL_ASSIGN
ZTPP_TOK_SYM_AMPERSAND_ASSIGN
ZTPP_TOK_SYM_CIRCUMFLEX_ASSIGN
ZTPP_TOK_SYM_VBAR_ASSIGN

%start expression_result
%debug

%%

/********************************************************************
 * Phrase structure grammar
 ********************************************************************/

expression_result
    : constant_expression
    {
        ztpCalcResult  *sCalcResult = (ztpCalcResult *)$1;

        aParam->mResult = sCalcResult->mValue;
    }
    ;

constant_expression
    : conditional_expression
    {
        $$ = $1;
    }
    ;

primary_expression
    : ZTPP_TOK_ID
    {
        /*
         * expression 에서 ID 가 올 수 있는 경우는, Macro 사용 뿐이다.
         * 따라서, 주어진 이름에 대하여 Macro를 찾아서,
         * Macro를 치환하고 수행한 결과를 반환한다.
         * Macro 를 찾지 못 했을 경우에는, 0(FALSE) 을 반환한다.
         */
        stlInt64   sResult;

        sResult = ztppEvalObjectLikeMacro( aParam->mAllocator,
                                           aParam->mErrorStack,
                                           $1 );

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | ZTPP_TOK_ID ZTPP_TOK_SYM_LPAREN ZTPP_TOK_SYM_RPAREN
    {
        /*
         * - funcation call 형식의 macro invocation
         * expression 에서 ID 가 올 수 있는 경우는, Macro 사용 뿐이다.
         * 따라서, 주어진 이름에 대하여 Macro를 찾아서,
         * Macro를 치환하고 수행한 결과를 반환한다.
         * Macro 를 찾지 못 했을 경우에는, 0(FALSE) 을 반환한다.
         */
        stlInt64   sResult;

        sResult = ztppEvalFunctionLikeMacro( aParam->mAllocator,
                                             aParam->mErrorStack,
                                             $1,
                                             NULL );

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | ZTPP_TOK_ID ZTPP_TOK_SYM_LPAREN argument_expression_list ZTPP_TOK_SYM_RPAREN
    {
        /*
         * - funcation call 형식의 macro invocation
         * expression 에서 ID 가 올 수 있는 경우는, Macro 사용 뿐이다.
         * 따라서, 주어진 이름에 대하여 Macro를 찾아서,
         * Macro를 치환하고 수행한 결과를 반환한다.
         * Macro 를 찾지 못 했을 경우에는, 0(FALSE) 을 반환한다.
         */
        stlInt64   sResult;

        sResult = ztppEvalFunctionLikeMacro( aParam->mAllocator,
                                             aParam->mErrorStack,
                                             $1,
                                             $3 );

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | ZTPP_TOK_LITERAL
    {
        stlInt64   sResult;

        ztppConvertLiteral( $1,
                            &sResult,
                            aParam->mErrorStack );


        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | ZTPP_TOK_SYM_LPAREN constant_expression ZTPP_TOK_SYM_RPAREN
    {
        $$ = $2;
    }
    | ZTPP_TOK_DEFINED ZTPP_TOK_SYM_LPAREN ZTPP_TOK_ID ZTPP_TOK_SYM_RPAREN
    {
        /*
         * Macro 가 정의된 경우를 판단
         */
        stlStatus        sResult;
        ztpMacroSymbol  *sMacroSymbol;

        sResult = ztppFindMacro( $3,
                                 &sMacroSymbol,
                                 aParam->mErrorStack );

        if( sMacroSymbol != NULL )
        {
            /*
             * Macro 가 정의된 경우
             */
            sResult = STL_TRUE;
        }
        else
        {
            /*
             * Macro 가 정의되지 않은 경우
             */
            sResult = STL_FALSE;
        }


        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | ZTPP_TOK_DEFINED ZTPP_TOK_ID
    {
        /*
         * Macro 가 정의된 경우를 판단
         */
        stlStatus        sResult;
        ztpMacroSymbol  *sMacroSymbol;

        sResult = ztppFindMacro( $2,
                                 &sMacroSymbol,
                                 aParam->mErrorStack );

        if( sMacroSymbol != NULL )
        {
            /*
             * Macro 가 정의된 경우
             */
            sResult = STL_TRUE;
        }
        else
        {
            /*
             * Macro 가 정의되지 않은 경우
             */
            sResult = STL_FALSE;
        }

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

postfix_expression
    : primary_expression
    {
        $$ = $1;
    }
    ;

argument_expression_list
    : constant_expression
    {
        ztpCalcResult *sCalcResult = (ztpCalcResult *)$1;

        ZTPP_PARSE_CHECK( $$ = ztppCalcParamList( aParam,
                                                  NULL,
                                                  sCalcResult->mValue ) );
    }
    | constant_expression ZTPP_TOK_SYM_COMMA argument_expression_list
    {
        ztpCalcResult *sCalcResult = (ztpCalcResult *)$1;

        ZTPP_PARSE_CHECK( $$ = ztppCalcParamList( aParam,
                                                  $3,
                                                  sCalcResult->mValue ) );
    }
    ;

unary_expression
    : postfix_expression
    {
        $$ = $1;
    }
    | ZTPP_TOK_SYM_PLUS unary_expression
    {
        $$ = $2;
    }
    | ZTPP_TOK_SYM_MINUS unary_expression
    {
        stlInt64       sOperand;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult = (ztpCalcResult *)$2;

        sOperand = sCalcResult->mValue;
        sResult = -sOperand;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | ZTPP_TOK_SYM_TILDE unary_expression
    {
        stlInt64       sOperand;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult = (ztpCalcResult *)$2;

        sOperand = sCalcResult->mValue;
        sResult = ~sOperand;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | ZTPP_TOK_SYM_EXCLAMATION unary_expression
    {
        stlInt64       sOperand;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult = (ztpCalcResult *)$2;

        sOperand = sCalcResult->mValue;
        sResult = !sOperand;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

multiplicative_expression
    : unary_expression
    {
        $$ = $1;
    }
    | multiplicative_expression ZTPP_TOK_SYM_ASTERISK unary_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 * sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | multiplicative_expression ZTPP_TOK_SYM_SOLIDUS unary_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 / sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | multiplicative_expression ZTPP_TOK_SYM_PERCENT unary_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 % sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

additive_expression
    : multiplicative_expression
    {
        $$ = $1;
    }
    | additive_expression ZTPP_TOK_SYM_PLUS multiplicative_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 + sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | additive_expression ZTPP_TOK_SYM_MINUS multiplicative_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 - sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

shift_expression
    : additive_expression
    {
        $$ = $1;
    }
    | shift_expression ZTPP_TOK_SYM_SL additive_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 << sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | shift_expression ZTPP_TOK_SYM_SR additive_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 >> sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

relational_expression
    : shift_expression
    {
        $$ = $1;
    }
    | relational_expression ZTPP_TOK_SYM_LT shift_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 < sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | relational_expression ZTPP_TOK_SYM_GT shift_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 > sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | relational_expression ZTPP_TOK_SYM_LE shift_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 <= sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | relational_expression ZTPP_TOK_SYM_GE shift_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 >= sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

equality_expression
    : relational_expression
    {
        $$ = $1;
    }
    | equality_expression ZTPP_TOK_SYM_EQ relational_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 == sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    | equality_expression ZTPP_TOK_SYM_NE relational_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 != sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

and_expression
    : equality_expression
    {
        $$ = $1;
    }
    | and_expression ZTPP_TOK_SYM_AMPERSAND equality_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 & sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

exclusive_or_expression
    : and_expression
    {
        $$ = $1;
    }
    | exclusive_or_expression ZTPP_TOK_SYM_CIRCUMFLEX and_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 ^ sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

inclusive_or_expression
    : exclusive_or_expression
    {
        $$ = $1;
    }
    | inclusive_or_expression ZTPP_TOK_SYM_VBAR exclusive_or_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 | sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

logical_and_expression
    : inclusive_or_expression
    {
        $$ = $1;
    }
    | logical_and_expression ZTPP_TOK_SYM_AND inclusive_or_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 && sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

logical_or_expression
    : logical_and_expression
    {
        $$ = $1;
    }
    | logical_or_expression ZTPP_TOK_SYM_OR logical_and_expression
    {
        stlInt64       sOperand1;
        stlInt64       sOperand2;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = (ztpCalcResult *)$3;

        sOperand1 = sCalcResult1->mValue;
        sOperand2 = sCalcResult2->mValue;

        sResult = sOperand1 || sOperand2;

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

conditional_expression
    : logical_or_expression
    {
        $$ = $1;
    }
    | logical_or_expression ZTPP_TOK_SYM_QUESTION constant_expression ZTPP_TOK_SYM_COLON conditional_expression
    {
        stlInt64       sOperand;
        stlInt64       sResult;
        ztpCalcResult *sCalcResult1 = (ztpCalcResult *)$1;
        ztpCalcResult *sCalcResult2 = NULL;

        sOperand = sCalcResult1->mValue;
        if( sOperand != 0 )
        {
            sCalcResult2 = (ztpCalcResult *)$3;
            sResult = sCalcResult2->mValue;
        }
        else
        {
            sCalcResult2 = (ztpCalcResult *)$5;
            sResult = sCalcResult2->mValue;
        }

        ZTPP_PARSE_CHECK( $$ = ztppMakeCalcResult( aParam,
                                                   sResult ) );
    }
    ;

%%
