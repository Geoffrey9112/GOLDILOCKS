/*******************************************************************************
 * ztppGram.y
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
#include <ztppFunc.h>
#include <ztppEval.h>
#include <ztppCalcFunc.h>
#include <ztppExpandFile.h>
#include <ztpConvertExecSql.h>
#include <ztpPP.tab.h>

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

%parse-param { ztpPPParseParam  * aParam  }
%parse-param { void            * aScanner }
%lex-param   { yyscan_t        * aScanner }

%token
ZTPP_TOK_DEFINED
ZTPP_TOK_LITERAL
ZTPP_TOK_ID
ZTPP_TOK_DEFINED_MACRO
ZTPP_TOK_H_STRING
ZTPP_TOK_Q_STRING

ZTPP_TOK_PP_ELIF
ZTPP_TOK_PP_ELSE
ZTPP_TOK_PP_ENDIF
ZTPP_TOK_PP_IF
ZTPP_TOK_PP_IFDEF
ZTPP_TOK_PP_IFNDEF
ZTPP_TOK_PP_INCLUDE
ZTPP_TOK_PP_DEFINE
ZTPP_TOK_PP_DEFINE_FUNCTION_MACRO
ZTPP_TOK_PP_UNDEF
ZTPP_TOK_PP_LINE
ZTPP_TOK_PP_ERROR
ZTPP_TOK_PP_PRAGMA
ZTPP_TOK_PP_NULL

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

%start preprocessing_file
%debug

%%

/********************************************************************
 * Phrase structure grammar
 ********************************************************************/

identifier
    : ZTPP_TOK_ID
    {
        $$ = $1;
    }
    ;

identifier_list
    : identifier
    {
        $$ = ztppMakeArgNameList( aParam, $1, NULL );
    }
    | identifier ZTPP_TOK_SYM_COMMA identifier_list
    {
        $$ = ztppMakeArgNameList( aParam, $1, $3 );
    }
    ;

preprocessing_file
    : group
    |
    {
        aParam->mCCodeEndLoc = aParam->mFileLength;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
    }
    ;

group
    : group_part
    | group group_part
    ;

group_part
    : if_section
    {
    }
    | control_line
    {
    }
    | text_line
    {
        aParam->mCCodeEndLoc = aParam->mNextLoc;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    | ZTPP_TOK_PP_NULL non_directive
    {
    }
    ;

if_section
    : if_group elif_groups else_group endif_line
    | if_group elif_groups endif_line
    | if_group else_group endif_line
    | if_group endif_line
    ;

if_group
    : if_condition group
    {
    }
    | if_condition
    {
    }
    ;

if_condition
    : ZTPP_TOK_PP_IF constant_expression new_line
    {
        stlInt64  sExprResult = 0;
        stlBool   sCondition = STL_FALSE;
        stlChar  *sStartPtr;

        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;

        /*
         * Debug
         */
        sStartPtr = aParam->mBuffer + @2;

        if( stlStrncmp( sStartPtr, "__WORDSIZE ==", 13 ) == 0 )
        {
            sExprResult = 0;
        }

        /*
         * #if 가 출현하면 If Level을 1 증가시킨다.
         * 이는 #else/#endif 와 #if 의 결합 관계를 나타낸다.
         */
        aParam->mIfLevel ++;

        aParam->mOccurConditionTrue = STL_FALSE;

        /*
         * Evaluate Constant Expression
         */
        sExprResult = ztppEvalConstExpr( aParam->mAllocator,
                                         aParam->mErrorStack,
                                         $2 );

        if( sExprResult != 0 )
        {
            sCondition = STL_TRUE;
            aParam->mOccurConditionTrue = STL_TRUE;
        }
        else
        {
            sCondition = STL_FALSE;
        }

        /*
         * 현재 #if group의 끝까지 진행한다.
         */
        if( sCondition == STL_FALSE )
        {
            ztppGoAndIgnoreGroup( aParam, aScanner );
        }

        /*
         * #if expression 문장을 skip 한다.
         */
        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    | ZTPP_TOK_PP_IFDEF identifier new_line
    {
        ztpMacroSymbol  *sMacroSymbol;

        /*
         * #if 가 출현하면 If Level을 1 증가시킨다.
         * 이는 #else/#endif 와 #if 의 결합 관계를 나타낸다.
         */
        aParam->mIfLevel ++;

        aParam->mOccurConditionTrue = STL_FALSE;

        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));

        (void)ztppFindMacro( $2,
                             &sMacroSymbol,
                             aParam->mErrorStack );

        if( sMacroSymbol != NULL )
        {
            /*
             * 이미 정의된 Macro가 존재하는 경우
             * condition이 TRUE 이다.
             */
            aParam->mOccurConditionTrue = STL_TRUE;
            $$ = (YYSTYPE)STL_TRUE;
        }
        else
        {
            /*
             * 이미 정의된 Macro가 존재하지 않는 경우
             * condition이 FALSE 이다.
             */
            $$ = (YYSTYPE)STL_FALSE;

            /*
             * 현재 #if group의 끝까지 진행한다.
             */
            ztppGoAndIgnoreGroup( aParam, aScanner );
        }

        /*
         * #ifdef identifier 문장을 skip 한다.
         */
        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    | ZTPP_TOK_PP_IFNDEF identifier new_line
    {
        ztpMacroSymbol  *sMacroSymbol;

        /*
         * #if 가 출현하면 If Level을 1 증가시킨다.
         * 이는 #else/#endif 와 #if 의 결합 관계를 나타낸다.
         */
        aParam->mIfLevel ++;

        aParam->mOccurConditionTrue = STL_FALSE;

        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));

        (void)ztppFindMacro( $2,
                             &sMacroSymbol,
                             aParam->mErrorStack );

        if( sMacroSymbol != NULL )
        {
            /*
             * 이미 정의된 Macro가 존재하는 경우
             * condition이 FALSE 이다.
             */

            $$ = (YYSTYPE)STL_FALSE;

            /*
             * 현재 #if group의 끝까지 진행한다.
             */
            ztppGoAndIgnoreGroup( aParam, aScanner );
        }
        else
        {
            /*
             * 이미 정의된 Macro가 존재하지 않는 경우
             * condition이 TRUE 이다.
             */
            aParam->mOccurConditionTrue = STL_TRUE;
            $$ = (YYSTYPE)STL_TRUE;
        }

        /*
         * #ifndef identifier 문장을 skip 한다.
         */
        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    ;

elif_groups
    : elif_group
    | elif_groups elif_group
    ;

elif_group
    : elif_condition group
    {
    }
    | elif_condition
    {
    }
    ;

elif_condition
    : ZTPP_TOK_PP_ELIF constant_expression new_line
    {
        stlInt64  sExprResult = 0;
        stlBool   sCondition = STL_FALSE;

        if( aParam->mOccurConditionTrue == STL_TRUE )
        {
            sCondition = STL_FALSE;
        }
        else
        {
            sExprResult = ztppEvalConstExpr( aParam->mAllocator,
                                             aParam->mErrorStack,
                                             $2 );

            if( sExprResult != 0 )
            {
                sCondition = STL_TRUE;
                aParam->mOccurConditionTrue = STL_TRUE;
            }
            else
            {
                sCondition = STL_FALSE;
            }
        }

        /*
         * 현재 #if group의 끝까지 진행한다.
         */
        if( sCondition == STL_FALSE )
        {
            ztppGoAndIgnoreGroup( aParam, aScanner );
        }

        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    ;

else_group
    : else_phrase group
    {
    }
    | else_phrase
    {
    }
    ;

else_phrase
    : ZTPP_TOK_PP_ELSE new_line
    {
        /*
         * 현재까지의 if group에서 한번도 TRUE가 아니었을때
         * else group 을 expand 한다.
         */
        if(aParam->mOccurConditionTrue == STL_TRUE)
        {
            /*
             * 기존에 True 가 한번이라도 발생한 경우이므로,
             * else 파트를 skip 한다.
             */

            /*
             * 현재 #if group의 끝까지 진행한다.
             */
            ztppGoAndIgnoreGroup( aParam, aScanner );
        }
        else
        {
            /*
             * 기존에 True 가 한번도 발생하지 않았으므로,
             * else 파트를 expand 한다.
             */
        }

        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    ;

endif_line
    : ZTPP_TOK_PP_ENDIF new_line
    {
        /*
         * 현재까지의 if_group이 expand 가 skip 상태가 아닐때에만,
         * 코드를 print 한다.
         */
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;

        /*
         * #endif 가 출현하면 If Level을 1 감소시킨다.
         * 현재 #endif 와 짝이 되는 #if 가 종료되었음을 나타낸다.
         */
        aParam->mIfLevel --;

        /*
         * #endif 이후에 #if level이 하나 감소하게 되면,
         * 무조건 True 상태였던 group이므로
         * mOccurConditionTrue를 초기화(TRUE) 해 준다.
         */
        aParam->mOccurConditionTrue = STL_TRUE;
    }
    ;

control_line
    : include_header_file
    | define_simple_macro
    | define_function_like_macro
    | undef_macro
    | other_control_line
    {
        aParam->mCCodeEndLoc = aParam->mNextLoc;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    ;

other_control_line
    : ZTPP_TOK_PP_LINE pp_tokens new_line
    | ZTPP_TOK_PP_ERROR pp_tokens new_line
    | ZTPP_TOK_PP_ERROR new_line
    | ZTPP_TOK_PP_PRAGMA pp_tokens new_line
    | ZTPP_TOK_PP_PRAGMA new_line
    | ZTPP_TOK_PP_NULL new_line
    ;

text_line
    : text_line_pp_tokens new_line
    {
    }
    | new_line
    ;

text_line_pp_tokens
    : text_line_preprocessing_token
    {
    }
    | text_line_preprocessing_token text_line_pp_tokens
    {
    }
    ;

text_line_preprocessing_token
    : header_name
    {
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;

        ztppWritePPToken( aParam, $1 );
    }
    | ZTPP_TOK_ID
    {
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));

        ztppWritePPMacroToken( aParam, $1 );
        aParam->mCCodeStartLoc = aParam->mNextLoc;
    }
    | ZTPP_TOK_LITERAL
    {
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;

        ztppWritePPToken( aParam, $1 );
    }
    | ZTPP_TOK_DEFINED
    {
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;

        ztppWritePPToken( aParam, $1 );
    }
    | punctuator
    {
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;

        ztppWritePPToken( aParam, $1 );
    }
    ;

non_directive
    : pp_tokens new_line
    ;

replacement_list
    : pp_tokens
    {
        $$ = (YYSTYPE)ztppMakeStringByPPTokens( aParam );
    }
    |
    {
        $$ = (YYSTYPE)NULL;
    }
    ;

pp_tokens
    : preprocessing_token
    {
        ZTPP_PARSE_CHECK($$ = ztppMakePPTokens( aParam,
                                                @1,
                                                @1 + (stlInt32)stlStrlen($1),
                                                STL_TRUE ));
    }
    | preprocessing_token pp_tokens
    {
        ZTPP_PARSE_CHECK($$ = ztppMakePPTokens(aParam,
                                               @1,
                                               @2,
                                               STL_FALSE));
    }
    ;

preprocessing_token
    : header_name
    | ZTPP_TOK_ID
    | ZTPP_TOK_LITERAL
    | ZTPP_TOK_DEFINED
    | punctuator
    ;

header_name
    : ZTPP_TOK_H_STRING
    {
        $$ = $1;
    }
    | ZTPP_TOK_Q_STRING
    {
        $$ = $1;
    }
    ;

include_header_file
    : ZTPP_TOK_PP_INCLUDE header_name new_line
    {
        stlChar  sLineBuffer[ZTP_LINE_BUF_SIZE];

        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));

        stlSnprintf( sLineBuffer, ZTP_LINE_BUF_SIZE,
                    "\n/*\n#include %s \n*/\n", $2 );
        ztppSendStringToOutFile(aParam, sLineBuffer);

        aParam->mCCodeStartLoc = aParam->mNextLoc;

        ZTPP_PARSE_CHECK($$ = (YYSTYPE)ztppMakeIncludeHeaderFile( aParam, $2 ));
    }
    ;

punctuator
    : ZTPP_TOK_SYM_LBRACKET
    | ZTPP_TOK_SYM_RBRACKET
    | ZTPP_TOK_SYM_LPAREN
    | ZTPP_TOK_SYM_RPAREN
    | ZTPP_TOK_SYM_LBRACE
    | ZTPP_TOK_SYM_RBRACE
    | ZTPP_TOK_SYM_PERIOD
    | ZTPP_TOK_SYM_PTR
    | ZTPP_TOK_SYM_INC
    | ZTPP_TOK_SYM_DEC
    | ZTPP_TOK_SYM_AMPERSAND
    | ZTPP_TOK_SYM_ASTERISK
    | ZTPP_TOK_SYM_PLUS
    | ZTPP_TOK_SYM_MINUS
    | ZTPP_TOK_SYM_TILDE
    | ZTPP_TOK_SYM_EXCLAMATION
    | ZTPP_TOK_SYM_SOLIDUS
    | ZTPP_TOK_SYM_PERCENT
    | ZTPP_TOK_SYM_SL
    | ZTPP_TOK_SYM_SR
    | ZTPP_TOK_SYM_LT
    | ZTPP_TOK_SYM_GT
    | ZTPP_TOK_SYM_LE
    | ZTPP_TOK_SYM_GE
    | ZTPP_TOK_SYM_EQ
    | ZTPP_TOK_SYM_NE
    | ZTPP_TOK_SYM_CIRCUMFLEX
    | ZTPP_TOK_SYM_VBAR
    | ZTPP_TOK_SYM_AND
    | ZTPP_TOK_SYM_OR
    | ZTPP_TOK_SYM_QUESTION
    | ZTPP_TOK_SYM_COLON
    | ZTPP_TOK_SYM_SEMICOLON
    | ZTPP_TOK_SYM_ELLIPSIS
    | ZTPP_TOK_SYM_ASSIGNMENT
    | ZTPP_TOK_SYM_ASTERISK_ASSIGN
    | ZTPP_TOK_SYM_SOLIDUS_ASSIGN
    | ZTPP_TOK_SYM_PERCENT_ASSIGN
    | ZTPP_TOK_SYM_PLUS_ASSIGN
    | ZTPP_TOK_SYM_MINUS_ASSIGN
    | ZTPP_TOK_SYM_SR_ASSIGN
    | ZTPP_TOK_SYM_SL_ASSIGN
    | ZTPP_TOK_SYM_AMPERSAND_ASSIGN
    | ZTPP_TOK_SYM_CIRCUMFLEX_ASSIGN
    | ZTPP_TOK_SYM_VBAR_ASSIGN
    | ZTPP_TOK_SYM_COMMA
    | ZTPP_TOK_SYM_SHARP
    | ZTPP_TOK_SYM_DOUBLE_SHARP
    ;

new_line
    : ZTPP_TOK_SYM_NEW_LINE
    {
        $$ = $1;
    }
    ;

define_simple_macro
    : ZTPP_TOK_PP_DEFINE identifier replacement_list new_line
    {
        /*
         * 현재 파트가 expand 상태일 때에만, Code를 exand 한다.
         */
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;

        $$ = (YYSTYPE)ztppMakeMacro( aParam, $2, -1, STL_FALSE, NULL, $3 );
    }
    ;

define_function_like_macro
    : ZTPP_TOK_PP_DEFINE_FUNCTION_MACRO identifier_list ZTPP_TOK_SYM_RPAREN replacement_list new_line
    {
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;

        $$ = (YYSTYPE)ztppMakeFunctionLikeMacro( aParam, $1, STL_FALSE, $2, $4 );
    }
    | ZTPP_TOK_PP_DEFINE_FUNCTION_MACRO ZTPP_TOK_SYM_RPAREN replacement_list new_line
    {
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;

        $$ = (YYSTYPE)ztppMakeFunctionLikeMacro( aParam, $1, STL_FALSE, NULL, $3 );
    }
/*
    | ZTPP_TOK_PP_DEFINE_FUNCTION_MACRO ZTPP_TOK_SYM_ELLIPSIS ZTPP_TOK_SYM_RPAREN replacement_list new_line
    {
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        //aParam->mCCodeStartLoc = aParam->mNextLoc;

        $$ = (YYSTYPE)ztppMakeFunctionLikeMacro( aParam, $1, STL_TRUE, NULL, $4 );
    }
    | ZTPP_TOK_PP_DEFINE_FUNCTION_MACRO identifier_list ZTPP_TOK_SYM_COMMA ZTPP_TOK_SYM_ELLIPSIS ZTPP_TOK_SYM_RPAREN replacement_list new_line
    {
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        //aParam->mCCodeStartLoc = aParam->mNextLoc;

        $$ = (YYSTYPE)ztppMakeFunctionLikeMacro( aParam, $1, STL_TRUE, $2, $4 );
    }
*/
    ;

undef_macro
    : ZTPP_TOK_PP_UNDEF identifier new_line
    {
        aParam->mCCodeEndLoc = @1;
        ZTPP_PARSE_CHECK(ztppBypassCCode(aParam));
        aParam->mCCodeStartLoc = aParam->mNextLoc;

        $$ = (YYSTYPE)ztppProcessUndefMacro( aParam, $2 );
    }
    ;

primary_expression
    : ZTPP_TOK_ID
    {
        /*
         * - Object-like macro
         */
        ZTPP_PARSE_CHECK( $$ = ztppMakePPPrimaryExpr( aParam,
                                                      ZTP_TERMINAL_TYPE_OBJECT_LIKE_MACRO,
                                                      0,
                                                      NULL,
                                                      $1,
                                                      NULL ) );
    }
    | ZTPP_TOK_ID ZTPP_TOK_SYM_LPAREN ZTPP_TOK_SYM_RPAREN
    {
        /*
         * - Function-like macro
         */
        ZTPP_PARSE_CHECK( $$ = ztppMakePPPrimaryExpr( aParam,
                                                      ZTP_TERMINAL_TYPE_FUNCTION_LIKE_MACRO,
                                                      0,
                                                      NULL,
                                                      $1,
                                                      NULL ) );
    }
    | ZTPP_TOK_ID ZTPP_TOK_SYM_LPAREN argument_expression_list ZTPP_TOK_SYM_RPAREN
    {
        /*
         * - Function-like macro
         */
        ZTPP_PARSE_CHECK( $$ = ztppMakePPPrimaryExpr( aParam,
                                                      ZTP_TERMINAL_TYPE_FUNCTION_LIKE_MACRO,
                                                      0,
                                                      NULL,
                                                      $1,
                                                      $3 ) );
    }
    | ZTPP_TOK_LITERAL
    {
        stlInt64   sResult;

        ztppConvertLiteral( $1,
                            &sResult,
                            aParam->mErrorStack );

        ZTPP_PARSE_CHECK( $$ = ztppMakePPPrimaryExpr( aParam,
                                                      ZTP_TERMINAL_TYPE_VALUE,
                                                      sResult,
                                                      NULL,
                                                      NULL,
                                                      NULL ) );
    }
    | ZTPP_TOK_SYM_LPAREN constant_expression ZTPP_TOK_SYM_RPAREN
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPPrimaryExpr( aParam,
                                                      ZTP_TERMINAL_TYPE_EXPRESSION,
                                                      0,
                                                      $2,
                                                      NULL,
                                                      NULL ) );
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

        ZTPP_PARSE_CHECK( $$ = ztppMakePPPrimaryExpr( aParam,
                                                      ZTP_TERMINAL_TYPE_VALUE,
                                                      sResult,
                                                      NULL,
                                                      NULL,
                                                      NULL ) );
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

        ZTPP_PARSE_CHECK( $$ = ztppMakePPPrimaryExpr( aParam,
                                                      ZTP_TERMINAL_TYPE_VALUE,
                                                      sResult,
                                                      NULL,
                                                      NULL,
                                                      NULL ) );
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
        ZTPP_PARSE_CHECK( $$ = ztppMakeParameterList( aParam,
                                                      NULL,
                                                      $1 ) );
    }
    | constant_expression ZTPP_TOK_SYM_COMMA argument_expression_list
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakeParameterList( aParam,
                                                      $3,
                                                      $1 ) );
    }
    ;

unary_expression
    : postfix_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPUnaryExpr( aParam,
                                                    NULL,
                                                    $1,
                                                    ZTP_PPOP_NONE ) );
    }
    | ZTPP_TOK_SYM_PLUS unary_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPUnaryExpr( aParam,
                                                    $2,
                                                    NULL,
                                                    ZTP_PPOP_PLUS ) );
    }
    | ZTPP_TOK_SYM_MINUS unary_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPUnaryExpr( aParam,
                                                    $2,
                                                    NULL,
                                                    ZTP_PPOP_MINUS ) );
    }
    | ZTPP_TOK_SYM_TILDE unary_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPUnaryExpr( aParam,
                                                    $2,
                                                    NULL,
                                                    ZTP_PPOP_TILDE ) );
    }
    | ZTPP_TOK_SYM_EXCLAMATION unary_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPUnaryExpr( aParam,
                                                    $2,
                                                    NULL,
                                                    ZTP_PPOP_NOT ) );
    }
    ;

multiplicative_expression
    : unary_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPMulExpr( aParam,
                                                  NULL,
                                                  $1,
                                                  ZTP_PPOP_NONE ) );
    }
    | multiplicative_expression ZTPP_TOK_SYM_ASTERISK unary_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPMulExpr( aParam,
                                                  $1,
                                                  $3,
                                                  ZTP_PPOP_MUL ) );
    }
    | multiplicative_expression ZTPP_TOK_SYM_SOLIDUS unary_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPMulExpr( aParam,
                                                  $1,
                                                  $3,
                                                  ZTP_PPOP_DIV ) );
    }
    | multiplicative_expression ZTPP_TOK_SYM_PERCENT unary_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPMulExpr( aParam,
                                                  $1,
                                                  $3,
                                                  ZTP_PPOP_MOD ) );
    }
    ;

additive_expression
    : multiplicative_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPAddExpr( aParam,
                                                  NULL,
                                                  $1,
                                                  ZTP_PPOP_NONE ) );
    }
    | additive_expression ZTPP_TOK_SYM_PLUS multiplicative_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPAddExpr( aParam,
                                                  $1,
                                                  $3,
                                                  ZTP_PPOP_ADD ) );
    }
    | additive_expression ZTPP_TOK_SYM_MINUS multiplicative_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPAddExpr( aParam,
                                                  $1,
                                                  $3,
                                                  ZTP_PPOP_SUB ) );
    }
    ;

shift_expression
    : additive_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPShiftExpr( aParam,
                                                    NULL,
                                                    $1,
                                                    ZTP_PPOP_NONE ) );
    }
    | shift_expression ZTPP_TOK_SYM_SL additive_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPShiftExpr( aParam,
                                                    $1,
                                                    $3,
                                                    ZTP_PPOP_SL ) );
    }
    | shift_expression ZTPP_TOK_SYM_SR additive_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPShiftExpr( aParam,
                                                    $1,
                                                    $3,
                                                    ZTP_PPOP_SR ) );
    }
    ;

relational_expression
    : shift_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPRelationalExpr( aParam,
                                                         NULL,
                                                         $1,
                                                         ZTP_PPOP_NONE ) );
    }
    | relational_expression ZTPP_TOK_SYM_LT shift_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPRelationalExpr( aParam,
                                                         $1,
                                                         $3,
                                                         ZTP_PPOP_LT ) );
    }
    | relational_expression ZTPP_TOK_SYM_GT shift_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPRelationalExpr( aParam,
                                                         $1,
                                                         $3,
                                                         ZTP_PPOP_GT ) );
    }
    | relational_expression ZTPP_TOK_SYM_LE shift_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPRelationalExpr( aParam,
                                                         $1,
                                                         $3,
                                                         ZTP_PPOP_LE ) );
    }
    | relational_expression ZTPP_TOK_SYM_GE shift_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPRelationalExpr( aParam,
                                                         $1,
                                                         $3,
                                                         ZTP_PPOP_GE ) );
    }
    ;

equality_expression
    : relational_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPEqualExpr( aParam,
                                                    NULL,
                                                    $1,
                                                    ZTP_PPOP_NONE ) );
    }
    | equality_expression ZTPP_TOK_SYM_EQ relational_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPEqualExpr( aParam,
                                                    $1,
                                                    $3,
                                                    ZTP_PPOP_EQ ) );
    }
    | equality_expression ZTPP_TOK_SYM_NE relational_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPEqualExpr( aParam,
                                                    $1,
                                                    $3,
                                                    ZTP_PPOP_NE ) );
    }
    ;

and_expression
    : equality_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPBitAndExpr( aParam,
                                                     NULL,
                                                     $1,
                                                     ZTP_PPOP_NONE ) );
    }
    | and_expression ZTPP_TOK_SYM_AMPERSAND equality_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPBitAndExpr( aParam,
                                                     $1,
                                                     $3,
                                                     ZTP_PPOP_BIT_AND ) );
    }
    ;

exclusive_or_expression
    : and_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPBitXorExpr( aParam,
                                                     NULL,
                                                     $1,
                                                     ZTP_PPOP_NONE ) );
    }
    | exclusive_or_expression ZTPP_TOK_SYM_CIRCUMFLEX and_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPBitXorExpr( aParam,
                                                     $1,
                                                     $3,
                                                     ZTP_PPOP_BIT_XOR ) );
    }
    ;

inclusive_or_expression
    : exclusive_or_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPBitOrExpr( aParam,
                                                    NULL,
                                                    $1,
                                                    ZTP_PPOP_NONE ) );
    }
    | inclusive_or_expression ZTPP_TOK_SYM_VBAR exclusive_or_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPBitOrExpr( aParam,
                                                    $1,
                                                    $3,
                                                    ZTP_PPOP_BIT_OR ) );
    }
    ;

logical_and_expression
    : inclusive_or_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPAndExpr( aParam,
                                                  NULL,
                                                  $1,
                                                  ZTP_PPOP_NONE ) );
    }
    | logical_and_expression ZTPP_TOK_SYM_AND inclusive_or_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPAndExpr( aParam,
                                                  $1,
                                                  $3,
                                                  ZTP_PPOP_AND ) );
    }
    ;

logical_or_expression
    : logical_and_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPOrExpr( aParam,
                                                 NULL,
                                                 $1,
                                                 ZTP_PPOP_NONE ) );
    }
    | logical_or_expression ZTPP_TOK_SYM_OR logical_and_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPOrExpr( aParam,
                                                 $1,
                                                 $3,
                                                 ZTP_PPOP_OR ) );
    }
    ;

conditional_expression
    : logical_or_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPCondExpr( aParam,
                                                   $1,
                                                   NULL,
                                                   NULL ) );
    }
    | logical_or_expression ZTPP_TOK_SYM_QUESTION constant_expression ZTPP_TOK_SYM_COLON conditional_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPCondExpr( aParam,
                                                   $1,
                                                   $3,
                                                   $5 ) );
    }
    ;

constant_expression
    : conditional_expression
    {
        ZTPP_PARSE_CHECK( $$ = ztppMakePPConstExpr( aParam,
                                                    $1 ) );
    }
    ;

%%


void ztppGoAndIgnoreGroup( ztpPPParseParam  *aParam,
                           void             *aScanner )
{
    YYSTYPE   yylval;
    YYLTYPE   yylloc;
    stlInt32  sToken;
    stlInt32  sIfLevel = 1;

    /*
     * sIfLevel은 현재 #if 가 몇단계 중첩인지를 나타낸다.
     * 기본적으로 현재 #if group에 속해 있으므로, 1에서 시작한다.
     * 현재 sIfLevel 이 Initial level 과 같으면
     * #if, #ifdef, #ifndef 가 한번도 발생하지 않은 것이므로,
     * #elif, #else, #endif 의 경우, 현재 group 종료로 판단한다.
     * sIfLevel 이 Initial level 보다 높으면,
     * 현재 skip하도록 token을 소비하는 과정에서
     * 새로운 #if, #ifdef, #ifndef 가 발생한 것이므로
     * #elif, #else 는 무시하고, #endif 일때 sIfLevel을 감소한다.
     *
     * 탈출 조건
     *   #elif, #endif : initial level과 같은 level 이면,
     *                   최초 진입한 #if group 과 같은 level이므로,
     *                   sIfLevel == iniitial level 이면 탈출한다.
     *
     *   #endif        : 처음에 #if 상태에서 시작한 것이므로,
     *                   #endif 후에 initial level - 1 이 되어야,
     *                   최초 진입한 #if group가 같은 level 이 된다.
     *                   그러므로,
     *                   sIfLevel == initial level - 1
     *                   이면 탈출한다.
     */
    sToken = YYLEX;
    while( STL_TRUE )
    {
        switch( sToken )
        {
            case ZTPP_TOK_PP_ENDIF:
                sIfLevel --;
                if( sIfLevel <= 0 )
                {
                    STL_THROW( RAMP_EXIT );
                }
                break;
            case ZTPP_TOK_PP_ELIF:
            case ZTPP_TOK_PP_ELSE:
                if( sIfLevel <= 1 )
                {
                    STL_THROW( RAMP_EXIT );
                }
                break;
            case ZTPP_TOK_PP_IF:
            case ZTPP_TOK_PP_IFDEF:
            case ZTPP_TOK_PP_IFNDEF:
                sIfLevel ++;
            default:
                break;
        }

        sToken = YYLEX;
    }

    STL_RAMP( RAMP_EXIT );

    ztppGoPrevToken( aParam );
}

stlInt32 ztppGetNextToken( ztpPPParseParam  *aParam,
                           void             *aScanner )
{
    YYSTYPE   yylval;
    YYLTYPE   yylloc;

    return YYLEX;
}

stlBool  ztppIsLParen( stlInt32   aToken )
{
    return aToken == ZTPP_TOK_SYM_LPAREN ? STL_TRUE : STL_FALSE;
}

stlBool  ztppIsRParen( stlInt32   aToken )
{
    return aToken == ZTPP_TOK_SYM_RPAREN ? STL_TRUE : STL_FALSE;
}
