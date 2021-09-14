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
#include <ztpC.tab.h>
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

#define YY_USER_ACTION                                              \
    {                                                               \
        stlChar * sStr;                                             \
        if(yylineno != ZTP_C_PARAM->mLineNo)                        \
        {                                                           \
            ZTP_C_PARAM->mColumn = 1;                               \
            ZTP_C_PARAM->mLength = 0;                               \
            sStr = ZTP_C_PARAM->mBuffer + ZTP_C_PARAM->mNextLoc     \
                   + yyleng - 1;                                    \
            while(*sStr != '\n')                                    \
            {                                                       \
                ZTP_C_PARAM->mLength++;                             \
                sStr--;                                             \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            ZTP_C_PARAM->mColumn += ZTP_C_PARAM->mLength;           \
            ZTP_C_PARAM->mLength  = yyleng;                         \
        }                                                           \
        ZTP_C_PARAM->mLineNo  = yylineno;                           \
        ZTP_C_PARAM->mCurrLoc = ZTP_C_PARAM->mNextLoc;              \
        ZTP_C_PARAM->mNextLoc += yyleng;                            \
        *yylloc = ZTP_C_PARAM->mCurrLoc;                            \
    }

static const ztpKeyWord *ztpScanCKeyWordLookup(stlChar * aIdentifier);
%}


%option reentrant noyywrap never-interactive nounistd
%option bison-bridge bison-locations
%option case-insensitive
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
char_literal               {CP}?{quote}([^\']|{ES})+{quote}
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
*/
string_literal             ({SP}?{double_quote}([^\"\\]|{ES})*{double_quote})({space}*{SP}?{double_quote}([^\"\\]|{ES})*{double_quote})*

h_string                   <[^\n>]*>
q_string                   {double_quote}[^\n\"]*{double_quote}

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
include_file               {lt}.*{gt}

exec                       [eE][xX][eE][cC]
sql                        [sS][qQ][lL]
begin                      [bB][eE][gG][iI][nN]
end                        [eE][nN][dD]
declare                    [dD][eE][cC][lL][aA][rR][eE]
section                    [sS][eE][cC][tT][iI][oO][nN]
type                       [tT][yY][pP][eE]
is                         [iI][sS]
exec_sql                   {exec}{space}*{sql}{space}*
begin_decl_sec             {begin}{space}*{declare}{space}*{section}{space}*
end_decl_sec               {end}{space}*{declare}{space}*{section}{space}*
sql_type_is                {sql}{space}*{type}{space}*{is}{space}*
/*
  dummy_c_stmt               [\041-\072\074-\176]+
*/
dummy_c_stmt               [^\001-\040\042\047-\052\054\056\072\073\133\135\173\175\177]+

%%
{whitespace} {
    ZTP_C_PARSE_PRINT("WHITESPACE: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    //return ZTP_TOK_C_DUMMY;
}

{exec_sql} {
    ZTP_C_PARSE_PRINT("EXEC_SQL: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_EKW_EXEC_SQL;
}

{comment} {
    ZTP_C_PARSE_PRINT("COMMENT: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    //return ZTP_TOK_C_COMMENT;
}

{string_literal} {
    ZTP_C_PARSE_PRINT("STRING_LITERAL: %s: yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_DQ_LITERAL;
}

{literal} {
    ZTP_C_PARSE_PRINT("LITERAL: %s: yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_LITERAL;
}

{include_file} {
    ZTP_C_PARSE_PRINT("INCLUDE_FILE: %s: yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_INCLUDE_FILE;
}

{semicolon} {
    ZTP_C_PARSE_PRINT("SEMICOLON: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_SEMICOLON;
}

{colon} {
    ZTP_C_PARSE_PRINT("COLON: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_COLON;
}

{assign} {
    ZTP_C_PARSE_PRINT("ASSIGNMENT: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASSIGNMENT;
}

{comma} {
    ZTP_C_PARSE_PRINT("COMMA: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_COMMA;
}

{left_bracket} {
    ZTP_C_PARSE_PRINT("L_BRACKET: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_LBRACKET;
}

{right_bracket} {
    ZTP_C_PARSE_PRINT("R_BRACKET: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_RBRACKET;
}

{left_brace} {
    ZTP_C_PARSE_PRINT("L_BRACE: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_LBRACE;
}

{left_paren} {
    ZTP_C_PARSE_PRINT("L_PAREN: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_LPAREN;
}

{right_paren} {
    ZTP_C_PARSE_PRINT("R_PAREN: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_RPAREN;
}

{right_brace} {
    ZTP_C_PARSE_PRINT("R_BRACE: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_RBRACE;
}


{period} {
    ZTP_C_PARSE_PRINT("[.]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_PERIOD;
}

{asterisk} {
    ZTP_C_PARSE_PRINT("[*]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASTERISK;
}

{ampersand} {
    ZTP_C_PARSE_PRINT("[&]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_AMPERSAND;
}

{plus} {
    ZTP_C_PARSE_PRINT("[+]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_PLUS;
}

{minus} {
    ZTP_C_PARSE_PRINT("[-]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_MINUS;
}

{tilde} {
    ZTP_C_PARSE_PRINT("[~]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_TILDE;
}

{exclamation} {
    ZTP_C_PARSE_PRINT("[!]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_EXCLAMATION;
}

{solidus} {
    ZTP_C_PARSE_PRINT("[/]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_SOLIDUS;
}

{percent} {
    ZTP_C_PARSE_PRINT("[%]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_PERCENT;
}

{lt} {
    ZTP_C_PARSE_PRINT("[<]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_LT;
}

{gt} {
    ZTP_C_PARSE_PRINT("[>]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_GT;
}

{circumflex} {
    ZTP_C_PARSE_PRINT("[^]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_CIRCUMFLEX;
}

{vbar} {
    ZTP_C_PARSE_PRINT("[|]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_VBAR;
}

{question} {
    ZTP_C_PARSE_PRINT("[?]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_QUESTION;
}

{ellipsis} {
    ZTP_C_PARSE_PRINT("[...]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ELLIPSIS;
}

{asterisk_assign} {
    ZTP_C_PARSE_PRINT("*=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASSIGN_MUL;
}

{solidus_assign} {
    ZTP_C_PARSE_PRINT("/=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASSIGN_DIV;
}

{percent_assign} {
    ZTP_C_PARSE_PRINT("%=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASSIGN_MOD;
}

{plus_assign} {
    ZTP_C_PARSE_PRINT("+=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASSIGN_ADD;
}

{minus_assign} {
    ZTP_C_PARSE_PRINT("-=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASSIGN_SUB;
}

{shift_left_assign} {
    ZTP_C_PARSE_PRINT("<<=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASSIGN_LEFT;
}

{shift_right_assign} {
    ZTP_C_PARSE_PRINT(">>=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASSIGN_RIGHT;
}

{ampersand_assign} {
    ZTP_C_PARSE_PRINT("&=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASSIGN_AND;
}

{circumflex_assign} {
    ZTP_C_PARSE_PRINT("^=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASSIGN_XOR;
}

{vbar_assign} {
    ZTP_C_PARSE_PRINT("|=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_ASSIGN_OR;
}

{shift_right} {
    ZTP_C_PARSE_PRINT(">>: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_RIGHT;
}

{shift_left} {
    ZTP_C_PARSE_PRINT("<<: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_LEFT;
}

{double_plus} {
    ZTP_C_PARSE_PRINT("++: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_INC;
}

{double_minus} {
    ZTP_C_PARSE_PRINT("--: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_DEC;
}

{pointer_op} {
    ZTP_C_PARSE_PRINT("->: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_PTR;
}

{double_ampersand} {
    ZTP_C_PARSE_PRINT("&&: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_AND;
}

{double_vbar} {
    ZTP_C_PARSE_PRINT("||: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_OR;
}

{le} {
    ZTP_C_PARSE_PRINT("<=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_LE;
}

{ge} {
    ZTP_C_PARSE_PRINT(">=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_GE;
}

{equal} {
    ZTP_C_PARSE_PRINT("==: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_EQ;
}

{not_equal} {
    ZTP_C_PARSE_PRINT("!=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_OP_NE;
}

{quote} {
    ZTP_C_PARSE_PRINT("': yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_SINGLE_QUOTE;
}

{double_quote} {
    ZTP_C_PARSE_PRINT("\": yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_SYM_DOUBLE_QUOTE;
}

{begin_decl_sec} {
    ZTP_C_PARSE_PRINT("BEGIN_DECL_SEC: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_EKW_BEGIN_DECL_SEC;
}

{end_decl_sec} {
    ZTP_C_PARSE_PRINT("END_DECL_SEC: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_EKW_END_DECL_SEC;
}

{sql_type_is} {
    ZTP_C_PARSE_PRINT("SQL_TYPE_IS: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_EKW_SQL_TYPE_IS;
}

{pp_directive} {
    ZTP_C_PARSE_PRINT("PP_DIRECTIVE: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_C_PP_DIRECTIVE;
}

{pp_elif} {
    ZTP_C_PARSE_PRINT("PP_ELIF: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_C_PP_ELIF;
}

{pp_else} {
    ZTP_C_PARSE_PRINT("PP_ELSE: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_C_PP_ELSE;
}

{pp_endif} {
    ZTP_C_PARSE_PRINT("PP_ENDIF: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_C_PP_ENDIF;
}

{pp_ifdef} {
    ZTP_C_PARSE_PRINT("PP_IFDEF: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_C_PP_IFDEF;
}

{pp_ifndef} {
    ZTP_C_PARSE_PRINT("PP_IFNDEF: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_C_PP_IFNDEF;
}

{pp_if} {
    ZTP_C_PARSE_PRINT("PP_IF: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_C_PP_IF;
}

{pp_include} {
    ZTP_C_PARSE_PRINT("PP_INCLUDE: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_C_PP_INCLUDE;
}

{pp_define} {
    ZTP_C_PARSE_PRINT("PP_DEFINE: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_C_PP_DEFINE;
}

{pp_undef} {
    ZTP_C_PARSE_PRINT("PP_UNDEF: %s\n", yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_C_PP_UNDEF;
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
            ZTP_C_PARSE_PRINT("typedef \"%s\": yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);

            ZTP_C_PARSE_RETURN_STR(yytext);
            return ZTP_TOK_TYPEDEF_NAME;
        }
        else
        {
            ZTP_C_PARSE_PRINT("ID: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);

            ZTP_C_PARSE_RETURN_STR(yytext);
            return ZTP_TOK_ID;
        }
    }
    else
    {
        ZTP_C_PARSE_PRINT("keyword\t: %s\n", sKeyWord->mName);

        ZTP_C_PARSE_RETURN_STR(sKeyWord->mName);
        return sKeyWord->mTokenCode;
    }
}

%%

/*
{dummy_c_stmt} {
    ZTP_C_PARSE_PRINT("DUMMY: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTP_C_PARSE_RETURN_STR(yytext);
    return ZTP_TOK_C_DUMMY;
}
*/

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
    { "auto",  ZTP_TOK_CKW_AUTO, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "break",  ZTP_TOK_CKW_BREAK, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "by",  ZTP_TOK_EKW_BY, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "case",  ZTP_TOK_CKW_CASE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "char",  ZTP_TOK_CKW_CHAR, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "clock_t",  ZTP_TOK_CKW_CLOCK_T, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "const",  ZTP_TOK_CKW_CONST, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "context", ZTP_TOK_EKW_CONTEXT, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "continue", ZTP_TOK_CKW_CONTINUE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "date",  ZTP_TOK_EKW_DATE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "default", ZTP_TOK_CKW_DEFAULT, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "do",  ZTP_TOK_CKW_DO, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "double",  ZTP_TOK_CKW_DOUBLE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "else",  ZTP_TOK_CKW_ELSE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "enum",  ZTP_TOK_CKW_ENUM, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "extern",  ZTP_TOK_CKW_EXTERN, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "file",  ZTP_TOK_CKW_FILE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "float",  ZTP_TOK_CKW_FLOAT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "for",  ZTP_TOK_CKW_FOR, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "found",  ZTP_TOK_EKW_FOUND, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "free",  ZTP_TOK_EKW_FREE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "goto",  ZTP_TOK_CKW_GOTO, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "if",  ZTP_TOK_CKW_IF, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "include",  ZTP_TOK_EKW_INCLUDE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "inline",  ZTP_TOK_CKW_INLINE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "int",  ZTP_TOK_CKW_INT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "key_t",  ZTP_TOK_CKW_KEY_T, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "long",  ZTP_TOK_CKW_LONG, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "not",  ZTP_TOK_EKW_NOT, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "off_t", ZTP_TOK_CKW_OFF_T, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "pid_t", ZTP_TOK_CKW_PID_T, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "pthread_t", ZTP_TOK_CKW_PTHREAD_T, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "register", ZTP_TOK_CKW_REGISTER, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "restrict", ZTP_TOK_CKW_RESTRICT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "return", ZTP_TOK_CKW_RETURN, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "sem_t",  ZTP_TOK_CKW_SEM_T, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "short",  ZTP_TOK_CKW_SHORT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "signed",  ZTP_TOK_CKW_SIGNED, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "size_t",  ZTP_TOK_CKW_SIZE_T, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "sizeof",  ZTP_TOK_CKW_SIZEOF, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "sql_context",  ZTP_TOK_EKW_SQL_CONTEXT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "sqlerror",  ZTP_TOK_EKW_SQLERROR, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "sqllen",  ZTP_TOK_EKW_SQLLEN, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "sqlstate",  ZTP_TOK_EKW_SQLSTATE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "sqlwarning",  ZTP_TOK_EKW_SQLWARNING, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "ssize_t",  ZTP_TOK_CKW_SSIZE_T, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "static",  ZTP_TOK_CKW_STATIC, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "stop",  ZTP_TOK_EKW_STOP, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "struct",  ZTP_TOK_CKW_STRUCT, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "switch",  ZTP_TOK_CKW_SWITCH, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "typedef",  ZTP_TOK_CKW_TYPEDEF, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "union",  ZTP_TOK_CKW_UNION, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "unsigned",  ZTP_TOK_CKW_UNSIGNED, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "use",  ZTP_TOK_EKW_USE, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "varchar",  ZTP_TOK_EKW_VARCHAR, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "void",  ZTP_TOK_CKW_VOID, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "volatile",  ZTP_TOK_CKW_VOLATILE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE },
    { "whenever",  ZTP_TOK_EKW_WHENEVER, ZTP_KEYWORD_CATEGORY_CASE_INSENSITIVE },
    { "while",  ZTP_TOK_CKW_WHILE, ZTP_KEYWORD_CATEGORY_CASE_SENSITIVE }
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
