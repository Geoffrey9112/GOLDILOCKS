/*******************************************************************************
 * ztpC.l
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
#include <ztpCMin.tab.h>
#include <ztpCParseFunc.h>
#include <ztpMisc.h>
#include <ztpcTypeDef.h>

#ifndef ZTP_PARSER_PRINT
#define ZTP_C_PARSE_PRINT  stlIgnorePrintf
//#define ZTP_C_PARSE_PRINT  stlPrintf
#else
#define ZTP_C_PARSE_PRINT  stlPrintf
#endif

#define ZTP_C_PARAM yyget_extra( yyscanner )

#define ZTP_C_PARSE_RETURN_QUOTE_STR( param, text )
/*
#define ZTP_C_PARSE_RETURN_QUOTE_STR( param, text )                     \
    {                                                                   \
        stlInt32    sCursor  = 0;                                       \
        stlChar   * sCurChar = NULL;                                    \
        stlChar   * sResult  = NULL;                                    \
                                                                        \
        if( (param)->mAllocator( (param)->mContext,                     \
                                 yyleng + 1,                            \
                                 (void**) yylval,                       \
                                 (param)->mErrorStack )                 \
            != STL_SUCCESS )                                            \
        {                                                               \
            (param)->mErrorStatus = STL_FAILURE;                          \
        }                                                               \
        else                                                            \
        {                                                               \
            if( yyleng == 0)                                            \
            {                                                           \
                *yylval = NULL;                                         \
            }                                                           \
            else if( yyleng > 4000 )                                    \
            {                                                           \
                (param)->mErrorStatus = STL_FAILURE;                      \
                                                                        \
                stlPushError( STL_ERROR_LEVEL_ABORT,                    \
                              STL_ERRCODE_PARSER_INPUT_TOO_LONG,        \
                              NULL,                                     \
                              (param)->mErrorStack );                   \
            }                                                           \
            else                                                        \
            {                                                           \
                sCurChar = (stlChar*) text;                             \
                sResult  = (stlChar*) *yylval;                          \
                for( sCursor = 0; sCursor < yyleng ; sCursor++ )        \
                {                                                       \
                    if( ( sCurChar[0] == '\'' ) &&                      \
                        ( sCurChar[1] == '\'' ) )                       \
                    {                                                   \
                        *sResult = '\'';                                \
                        sCurChar++;                                     \
                        yyleng--;                                       \
                    }                                                   \
                    else                                                \
                    {                                                   \
                        *sResult = *sCurChar;                           \
                    }                                                   \
                                                                        \
                    sCurChar++;                                         \
                    sResult++;                                          \
                }                                                       \
                                                                        \
                ((stlChar*)(*yylval))[yyleng] = '\0';                   \
            }                                                           \
            *yylloc = ZTP_C_PARAM->mCurrLoc;                            \
        }                                                               \
    }
*/
#define ZTP_C_PARSE_IGNORE_STR( param )          \
    {                                            \
        *yylloc = ZTP_C_PARAM->mCurrLoc;         \
    }

#define ZTP_C_PARSE_RETURN_STR( aStr )                      \
    {                                                       \
        if(stlAllocRegionMem(ZTP_C_PARAM->mAllocator,       \
                             stlStrlen(aStr) + 1,           \
                             yylval,                        \
                             ZTP_C_PARAM->mErrorStack)      \
           != STL_SUCCESS)                                  \
        {                                                   \
            ZTP_C_PARAM->mErrorStatus = STL_FAILURE;        \
        }                                                   \
        else                                                \
        {                                                   \
            stlMemcpy(*yylval, aStr, stlStrlen(aStr) + 1);  \
        }                                                   \
    }

#define ZTP_C_PARSE_RETURN( aToken, text, loc, len )    \
    {                                                   \
        if( ZTP_C_PARAM->mBypassCToken == STL_FALSE )   \
        {                                               \
            ZTP_C_PARSE_PRINT(                          \
                "%s: loc : %d, leng : %d,"              \
                "line : %d, token : %d\n",              \
                yytext, *yylloc, yyleng,                \
                ZTP_C_PARAM->mLineNo, aToken);          \
            ZTP_C_PARSE_RETURN_STR(text);               \
            return( aToken );                           \
        }                                               \
    }

    /*
#define YY_INPUT( buffer, res, max_size )                               \
    do {                                                                \
        if (ZTP_C_PARAM->mPosition >= ZTP_C_PARAM->mLength)             \
        {                                                               \
            res = YY_NULL;                                              \
        }                                                               \
        else                                                            \
        {                                                               \
            res = ZTP_C_PARAM->mLength - ZTP_C_PARAM->mPosition;        \
            res > (stlInt32)max_size ? res = max_size : 0;              \
            stlMemcpy(buffer,                                           \
                      ZTP_C_PARAM->mBuffer + ZTP_C_PARAM->mPosition,    \
                      res);                                             \
            ZTP_C_PARAM->mPosition += res;                              \
        }                                                               \
    } while (0)
    */

#define YY_INPUT( buffer, res, max_size )                               \
    do {                                                                \
        stlInt32  sNumBytesToRead = max_size;                           \
        stlInt32  sBytesRemain;                                         \
        sBytesRemain = ZTP_C_PARAM->mFileLength - ZTP_C_PARAM->mPosition; \
        if(sNumBytesToRead > sBytesRemain)                              \
        {                                                               \
            sNumBytesToRead = sBytesRemain;                             \
        }                                                               \
        stlMemcpy(buffer,                                               \
                  ZTP_C_PARAM->mBuffer + ZTP_C_PARAM->mPosition,        \
                  sNumBytesToRead);                                     \
        res = sNumBytesToRead;                                          \
        ZTP_C_PARAM->mPosition += sNumBytesToRead;                      \
    } while (0)

#define YY_USER_ACTION                                                  \
    {                                                                   \
        stlChar * sStr;                                                 \
        if(yylineno != ZTP_C_PARAM->mLineNo)                            \
        {                                                               \
            ZTP_C_PARAM->mColumn = 1;                                   \
            ZTP_C_PARAM->mLength = 0;                                   \
            sStr = ZTP_C_PARAM->mBuffer + ZTP_C_PARAM->mNextLoc         \
                + yyleng - 1;                                           \
            while(*sStr != '\n')                                        \
            {                                                           \
                ZTP_C_PARAM->mLength++;                                 \
                sStr--;                                                 \
            }                                                           \
        }                                                               \
        else                                                            \
        {                                                               \
            ZTP_C_PARAM->mColumn += ZTP_C_PARAM->mLength;               \
            ZTP_C_PARAM->mLength  = yyleng;                             \
        }                                                               \
        ZTP_C_PARAM->mLineNo  = yylineno;                               \
        ZTP_C_PARAM->mCurrLoc = ZTP_C_PARAM->mNextLoc;                  \
        ZTP_C_PARAM->mNextLoc += yyleng;                                \
        *yylloc = ZTP_C_PARAM->mCurrLoc;                                \
    }

static const ztpKeyWord *ztpScanCKeyWordLookup(stlChar * aIdentifier);
%}


%option reentrant noyywrap never-interactive nounistd
%option bison-bridge bison-locations
%option nounput
%option yylineno

/* SPECIAL CHARACTER */
comment_introducer         "/*"
comment_terminator         "*/"
simple_comment             "//"[^\n]*\n
block_comment              {comment_introducer}{comment_terminator}|{comment_introducer}([^+])([^*]|(\*+[^*/]))*\**{comment_terminator}

left_bracket               "["
right_bracket              "]"
left_paren                 "("
right_paren                ")"
left_brace                 "{"
right_brace                "}"
period                     "."
pointer_op                 "->"

double_plus                "++"
double_minus               "--"
ampersand                  "&"
asterisk                   "*"
plus                       "+"
minus                      "-"
tilde                      "~"
exclamation                "!"

solidus                    "/"
percent                    "%"
shift_left                 "<<"
shift_right                ">>"
lt                         "<"
gt                         ">"
le                         "<="
ge                         ">="
equal                      "=="
not_equal                  "!="
circumflex                 "^"
vbar                       "|"
double_ampersand           "&&"
double_vbar                "||"

question                   "?"
colon                      ":"
semicolon                  ";"
ellipsis                   "..."

assign                     "="
asterisk_assign            "*="
solidus_assign             "/="
percent_assign             "%="
plus_assign                "+="
minus_assign               "-="
shift_left_assign          "<<="
shift_right_assign         ">>="
ampersand_assign           "&="
circumflex_assign          "^="
vbar_assign                "|="

comma                      ","
sharp                      "#"
double_sharp               "##"

new_line                   "\n"
quote                      "\'"
double_quote               "\""
reverse_solidus            "\\"

/* Tokens */
/* pattern for rules */
space                      [ \t\n\r\v\f]
whitespace                 {space}+
comment                    ({simple_comment}|{block_comment})
letter                     [A-Za-z]
identifier                 [A-Za-z_][A-Za-z_0-9]*
sc                         [A-Z0-9]
sqlstate                   [Ss][Qq][Ll][Ss][Tt][Aa][Tt][Ee]{whitespace}({sc}{sc}|{sc}{sc}{sc}{sc}{sc})


digit                      [0-9]
non_zero_digit             [1-9]
octal_digit                [0-7]
hex_digit                  [0-9A-Fa-f]

literal                    {numeric_literal}|{string_literal}
O                          [0-7]
D                          [0-9]
NZ                         [1-9]
L                          [a-zA-Z_]
A                          [a-zA-Z_0-9]
H                          [a-fA-F0-9]
HP                         (0[xX])
E                          ([Ee][+-]?{D}+)
P                          ([Pp][+-]?{D}+)
FS                         (f|F|l|L)
IS                         (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
CP                         (u|U|L)
SP                         (u8|u|U|L)
ES                         (\\([\'\"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))

hex_literal                {HP}{H}+{IS}?
decimal_literal            {NZ}{D}*{IS}?
octal_literal              "0"{O}*{IS}?
/*
 * Oraginal C는 여기 주석처리된 char_literal을 갖지만,
 * SQL에서 사용되도록 유연성 확보를 위해 아래와 같이 재정의함(\\\n 를 제거)
 * precompiler 에서는 이를 pass 시키지만,
 * C 언어의 영역은 이후의 C compiler에 맡김
char_literal               {CP}?{quote}([^\'\\\n]|{ES})+{quote}
*/
char_literal               {CP}?{quote}([^\\\']|{ES})*{quote}
fixed_literal              ({decimal_literal}|{octal_literal}|{hex_literal}|{char_literal})
float_literal              ({D}+{E}|{D}*"."{D}+{E}?|{D}+"."{E}?|{HP}{H}+{P}|{HP}{H}*"."{H}+{P}|{HP}{H}+"."{P}){FS}?

numeric_literal            {fixed_literal}|{float_literal}
/*
 * Oraginal C는 여기 주석처리된 string_literal을 갖지만,
 * SQL에서 사용되도록 유연성 확보를 위해 아래와 같이 재정의함(\\\n 를 제거)
 * precompiler 에서는 이를 pass 시키지만,
 * C 언어의 영역은 이후의 C compiler에 맡김
string_literal             ({SP}?{double_quote}([^\"\\\n]|{ES})*{double_quote}{space}*)+
string_literal             ({SP}?{double_quote}([^\"\\\n]|{ES})*{double_quote})({space}*{SP}?{double_quote}([^\"\\\n]|{ES})*{double_quote})*
string_literal             ({SP}?{double_quote}([^\"\\]|{ES})*{double_quote})({space}*{SP}?{double_quote}([^\"\\]|{ES})*{double_quote})*
*/

string_literal             {SP}?{double_quote}([^\\\"]|{ES})*{double_quote}

h_string                   <[^\n<>]*>
q_string                   {double_quote}[^\n\"]*{double_quote}

pp_directive               {space}*#[ \t]*(.*\\{space})*.*{new_line}
/*
pp_directive               {space}*#[ \t]*([^deiu][A-Za-z]*|import|error)(.*\\{space})*.*{new_line}
pp_elif                    {space}*#[ \t]*elif(.*\\{space})*.*{new_line}
pp_else                    {space}*#[ \t]*else(.*\\{space})*.*{new_line}
pp_endif                   {space}*#[ \t]*endif(.*\\{space})*.*{new_line}
pp_if                      {space}*#[ \t]*if(.*\\{space})*.*{new_line}
pp_ifdef                   {space}*#[ \t]*ifdef(.*\\{space})*.*{new_line}
pp_ifndef                  {space}*#[ \t]*ifndef(.*\\{space})*.*{new_line}
pp_include                 {space}*#[ \t]*include
pp_define                  {space}*#[ \t]*define(.*\\{space})*.*{new_line}
pp_undef                   {space}*#[ \t]*undef(.*\\{space})*.*{new_line}
*/
include_file               {lt}[^\n<>]*{gt}

exec                       [eE][xX][eE][cC]
sql                        [sS][qQ][lL]
begin                      [bB][eE][gG][iI][nN]
end                        [eE][nN][dD]
declare                    [dD][eE][cC][lL][aA][rR][eE]
section                    [sS][eE][cC][tT][iI][oO][nN]
type                       [tT][yY][pP][eE]
is                         [iI][sS]
exec_sql                   {exec}{space}+{sql}
begin_decl_sec             {begin}{space}+{declare}{space}+{section}
end_decl_sec               {end}{space}+{declare}{space}{section}
sql_type_is                {sql}{space}+{type}{space}+{is}
number                     [nN][uU][mM][bB][eE][rR]
number_lparen              {number}{space}*{left_paren}

not_applicable             ([\000-\010\013-\014\016-\037\140\177\200-\377]+|{quote}|{double_quote}|{reverse_solidus})

%%
{not_applicable} {
    ZTP_C_PARSE_PRINT("not_applicable : %s\n", yytext);
    //ZTP_C_PARSE_RETURN( ZTP_TOK_C_DUMMY, yytext, *yylloc, yyleng );
}

{whitespace} {
    ZTP_C_PARSE_PRINT("WHITESPACE : %s\n", yytext);
    //ZTP_C_PARSE_RETURN( ZTP_TOK_C_DUMMY, yytext, *yylloc, yyleng );
}

{exec_sql} {
    ZTP_C_PARAM->mBypassCToken = STL_FALSE;
    ZTP_C_PARSE_RETURN( ZTP_TOK_EKW_EXEC_SQL, yytext, *yylloc, yyleng );
}

{comment} {
    ZTP_C_PARSE_PRINT(
        "COMMENT:\n%s: loc : %d, leng : %d, line : %d\n",
        yytext, *yylloc, yyleng,
        ZTP_C_PARAM->mLineNo);
    //ZTP_C_PARSE_PRINT("COMMENT : %s\n", yytext);
    //ZTP_C_PARSE_RETURN( ZTP_TOK_C_COMMENT, yytext, *yylloc, yyleng );
}

{string_literal} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_DQ_LITERAL, yytext, *yylloc, yyleng );
}

{literal} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_LITERAL, yytext, *yylloc, yyleng );
}

{include_file} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_INCLUDE_FILE, yytext, *yylloc, yyleng );
}

{semicolon} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_SEMICOLON, yytext, *yylloc, yyleng );
}

{colon} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_COLON, yytext, *yylloc, yyleng );
}

{assign} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASSIGNMENT, yytext, *yylloc, yyleng );
}

{comma} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_COMMA, yytext, *yylloc, yyleng );
}

{left_bracket} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_LBRACKET, yytext, *yylloc, yyleng );
}

{right_bracket} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_RBRACKET, yytext, *yylloc, yyleng );
}

{left_brace} {
    (void)ztpIncSymTabDepth( ZTP_C_PARAM );
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_LBRACE, yytext, *yylloc, yyleng );
}

{left_paren} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_LPAREN, yytext, *yylloc, yyleng );
}

{right_paren} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_RPAREN, yytext, *yylloc, yyleng );
}

{right_brace} {
    (void)ztpDecSymTabDepth( ZTP_C_PARAM );
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_RBRACE, yytext, *yylloc, yyleng );
}

{period} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_PERIOD, yytext, *yylloc, yyleng );
}

{asterisk} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASTERISK, yytext, *yylloc, yyleng );
}

{ampersand} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_AMPERSAND, yytext, *yylloc, yyleng );
}

{plus} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_PLUS, yytext, *yylloc, yyleng );
}

{minus} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_MINUS, yytext, *yylloc, yyleng );
}

{tilde} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_TILDE, yytext, *yylloc, yyleng );
}

{exclamation} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_EXCLAMATION, yytext, *yylloc, yyleng );
}

{solidus} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_SOLIDUS, yytext, *yylloc, yyleng );
}

{percent} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_PERCENT, yytext, *yylloc, yyleng );
}

{lt} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_LT, yytext, *yylloc, yyleng );
}

{gt} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_GT, yytext, *yylloc, yyleng );
}

{circumflex} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_CIRCUMFLEX, yytext, *yylloc, yyleng );
}

{vbar} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_VBAR, yytext, *yylloc, yyleng );
}

{question} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_QUESTION, yytext, *yylloc, yyleng );
}

{ellipsis} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ELLIPSIS, yytext, *yylloc, yyleng );
}

{asterisk_assign} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASSIGN_MUL, yytext, *yylloc, yyleng );
}

{solidus_assign} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASSIGN_DIV, yytext, *yylloc, yyleng );
}

{percent_assign} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASSIGN_MOD, yytext, *yylloc, yyleng );
}

{plus_assign} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASSIGN_ADD, yytext, *yylloc, yyleng );
}

{minus_assign} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASSIGN_SUB, yytext, *yylloc, yyleng );
}

{shift_left_assign} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASSIGN_LEFT, yytext, *yylloc, yyleng );
}

{shift_right_assign} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASSIGN_RIGHT, yytext, *yylloc, yyleng );
}

{ampersand_assign} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASSIGN_AND, yytext, *yylloc, yyleng );
}

{circumflex_assign} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASSIGN_XOR, yytext, *yylloc, yyleng );
}

{vbar_assign} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_ASSIGN_OR, yytext, *yylloc, yyleng );
}

{shift_right} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_RIGHT, yytext, *yylloc, yyleng );
}

{shift_left} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_LEFT, yytext, *yylloc, yyleng );
}

{double_plus} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_INC, yytext, *yylloc, yyleng );
}

{double_minus} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_DEC, yytext, *yylloc, yyleng );
}

{pointer_op} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_PTR, yytext, *yylloc, yyleng );
}

{double_ampersand} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_AND, yytext, *yylloc, yyleng );
}

{double_vbar} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_OR, yytext, *yylloc, yyleng );
}

{le} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_LE, yytext, *yylloc, yyleng );
}

{ge} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_GE, yytext, *yylloc, yyleng );
}

{equal} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_EQ, yytext, *yylloc, yyleng );
}

{not_equal} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SYM_OP_NE, yytext, *yylloc, yyleng );
}

{begin_decl_sec} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_EKW_BEGIN_DECL_SEC, yytext, *yylloc, yyleng );
}

{end_decl_sec} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_EKW_END_DECL_SEC, yytext, *yylloc, yyleng );
}

{sql_type_is} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_EKW_SQL_TYPE_IS, yytext, *yylloc, yyleng );
}

{number_lparen} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_EKW_NUMBER_LPAREN, yytext, *yylloc, yyleng );
}

{pp_directive} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_C_PP_DIRECTIVE, yytext, *yylloc, yyleng );
}

{sqlstate} {
    ZTP_C_PARSE_RETURN( ZTP_TOK_SQLSTATE, yytext, *yylloc, yyleng );
}

{identifier} {
    const ztpKeyWord *sKeyWord;
    ztpCVariable     *sTypeDefSymbol = NULL;

    //stlToupperString( yytext, yytext );

    sKeyWord = ztpScanCKeyWordLookup( yytext );
    if( sKeyWord == NULL )
    {
        ztpcFindTypeDefInSymTab( yytext,
                                 &sTypeDefSymbol );
        if( sTypeDefSymbol != NULL )
        {
            ZTP_C_PARSE_RETURN( ZTP_TOK_TYPEDEF_NAME, yytext, *yylloc, yyleng );
        }
        else
        {
            ZTP_C_PARSE_RETURN( ZTP_TOK_ID, yytext, *yylloc, yyleng );
        }
    }
    else
    {
        ZTP_C_PARSE_RETURN( sKeyWord->mTokenCode, yytext, *yylloc, yyleng );
    }
}

%%

/*
 * Keyword List는 반드시, 이름에 따라서 soft되어 있어야만 한다.
 * 그렇지 않으면, 검색에 실패하는 경우가 발생한다.
 */
static const ztpKeyWord ztpKeyWordList[] = {
    { "__func__",  ZTP_TOK_CKW___FUNC__, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "_alignof",  ZTP_TOK_CKW__ALIGNOF, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "_bool",  ZTP_TOK_CKW__BOOL, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "_complex",  ZTP_TOK_CKW__COMPLEX, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "_imaginary",  ZTP_TOK_CKW__IMAGINARY, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "allocate",  ZTP_TOK_EKW_ALLOCATE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "at",  ZTP_TOK_EKW_AT, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "atomic",  ZTP_TOK_EKW_ATOMIC, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "auto",  ZTP_TOK_CKW_AUTO, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "autocommit",  ZTP_TOK_EKW_AUTOCOMMIT, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "binary",  ZTP_TOK_EKW_BINARY, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "boolean",  ZTP_TOK_EKW_BOOLEAN, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "char",  ZTP_TOK_CKW_CHAR, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "const",  ZTP_TOK_CKW_CONST, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "context", ZTP_TOK_EKW_CONTEXT, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "continue", ZTP_TOK_CKW_CONTINUE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "date",  ZTP_TOK_EKW_DATE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "day",  ZTP_TOK_EKW_DAY, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "default", ZTP_TOK_CKW_DEFAULT, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "do",  ZTP_TOK_CKW_DO, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "double",  ZTP_TOK_CKW_DOUBLE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "enum",  ZTP_TOK_CKW_ENUM, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "extern",  ZTP_TOK_CKW_EXTERN, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "float",  ZTP_TOK_CKW_FLOAT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "for",  ZTP_TOK_CKW_FOR, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "found",  ZTP_TOK_EKW_FOUND, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "free",  ZTP_TOK_EKW_FREE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "goto",  ZTP_TOK_CKW_GOTO, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "hour",  ZTP_TOK_EKW_HOUR, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "include",  ZTP_TOK_EKW_INCLUDE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "inline",  ZTP_TOK_CKW_INLINE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "int",  ZTP_TOK_CKW_INT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "interval",  ZTP_TOK_EKW_INTERVAL, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "long",  ZTP_TOK_CKW_LONG, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "longvarbinary",  ZTP_TOK_EKW_LONGVARBINARY, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "longvarchar",  ZTP_TOK_EKW_LONGVARCHAR, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "minute",  ZTP_TOK_EKW_MINUTE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "month",  ZTP_TOK_EKW_MONTH, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "not",  ZTP_TOK_EKW_NOT, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "number",  ZTP_TOK_EKW_NUMBER, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "off",  ZTP_TOK_EKW_OFF, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "on",  ZTP_TOK_EKW_ON, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "option",  ZTP_TOK_EKW_OPTION, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "register", ZTP_TOK_CKW_REGISTER, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "restrict", ZTP_TOK_CKW_RESTRICT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "second",  ZTP_TOK_EKW_SECOND, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "short",  ZTP_TOK_CKW_SHORT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "signed",  ZTP_TOK_CKW_SIGNED, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "sizeof",  ZTP_TOK_CKW_SIZEOF, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "sql_context",  ZTP_TOK_EKW_SQL_CONTEXT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "sqlerror",  ZTP_TOK_EKW_SQLERROR, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "sqlwarning",  ZTP_TOK_EKW_SQLWARNING, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "static",  ZTP_TOK_CKW_STATIC, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "stop",  ZTP_TOK_EKW_STOP, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "struct",  ZTP_TOK_CKW_STRUCT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "time",  ZTP_TOK_EKW_TIME, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "timestamp",  ZTP_TOK_EKW_TIMESTAMP, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "timezone",  ZTP_TOK_EKW_TIMEZONE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "to",  ZTP_TOK_EKW_TO, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "typedef",  ZTP_TOK_CKW_TYPEDEF, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "union",  ZTP_TOK_CKW_UNION, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "unsigned",  ZTP_TOK_CKW_UNSIGNED, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "use",  ZTP_TOK_EKW_USE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "varbinary",  ZTP_TOK_EKW_VARBINARY, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "varchar",  ZTP_TOK_EKW_VARCHAR, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "void",  ZTP_TOK_CKW_VOID, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "volatile",  ZTP_TOK_CKW_VOLATILE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "whenever",  ZTP_TOK_EKW_WHENEVER, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "while",  ZTP_TOK_CKW_WHILE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "with",  ZTP_TOK_EKW_WITH, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "year",  ZTP_TOK_EKW_YEAR, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
};

static const ztpKeyWord *ztpScanCKeyWordLookup(stlChar * aIdentifier)
{
    const ztpKeyWord  *sLow;
    const ztpKeyWord  *sHigh;
    const ztpKeyWord  *sMiddle;
    const ztpKeyWord  *sResultKeyWord;
    stlChar            sIdentifier[ZTP_KEY_WORD_MAX_LENGTH];
    stlInt32           sLength;
    stlInt32           sDifference;

    /*
     * check identifier length
     */

    sLength = 0;
    while(aIdentifier[sLength] != 0)
    {
        sLength++;
    }

    if(sLength >= ZTP_KEY_WORD_MAX_LENGTH)
    {
        sResultKeyWord = NULL;
    }
    else
    {
        /*
         * convert identifier to lower case
         */
        stlTolowerString(aIdentifier, sIdentifier);
        sIdentifier[sLength] = '\0';

        /*
         * scan keyword
         */
        sLow           = ztpKeyWordList;
        sHigh          = & ztpKeyWordList[ZTP_KEY_WORD_COUNT(ztpKeyWordList) - 1];
        sResultKeyWord = NULL;

        while(sLow <= sHigh)
        {
            sMiddle = sLow + ((sHigh - sLow) / 2);

            sDifference = stlStrcmp(sMiddle->mName, sIdentifier);

            if(sDifference == 0)
            {
                sResultKeyWord = sMiddle;
                break;
            }
            else if(sDifference < 0)
            {
                sLow = sMiddle + 1;
            }
            else
            {
                sHigh = sMiddle - 1;
            }
        }
    }

    /*
     * output 설정
     */

    return sResultKeyWord;
}
