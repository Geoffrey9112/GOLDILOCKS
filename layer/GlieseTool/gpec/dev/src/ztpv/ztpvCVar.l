/*******************************************************************************
 * ztpvCVar.l
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
#include <ztpvCVar.tab.h>
#include <ztpvCVarFunc.h>
#include <ztpMisc.h>

#ifndef ZTP_PARSER_PRINT
#define ZTPV_PARSE_PRINT  stlIgnorePrintf
//#define ZTPV_PARSE_PRINT  stlPrintf
#else
#define ZTPV_PARSE_PRINT  stlPrintf
#endif

#define ZTPV_CVAR_PARAM yyget_extra( yyscanner )

#define ZTPV_PARSE_IGNORE_STR( param )          \
    {                                           \
        *yylloc = ZTPV_CVAR_PARAM->mCurrLoc;    \
    }

#define ZTPV_PARSE_RETURN_STR( aStr )                       \
    {                                                       \
        if(stlAllocRegionMem(ZTPV_CVAR_PARAM->mAllocator,   \
                             stlStrlen(aStr) + 1,           \
                             yylval,                        \
                             ZTPV_CVAR_PARAM->mErrorStack)  \
           != STL_SUCCESS)                                  \
        {                                                   \
            ZTPV_CVAR_PARAM->mErrorStatus = STL_FAILURE;    \
        }                                                   \
        else                                                \
        {                                                   \
            stlMemcpy(*yylval, aStr, stlStrlen(aStr) + 1);  \
        }                                                   \
    }

#define YY_INPUT( buffer, res, max_size )                               \
    do {                                                                \
        stlInt32  sNumBytesToRead = max_size;                           \
        stlInt32  sBytesRemain;                                         \
        sBytesRemain = ZTPV_CVAR_PARAM->mCHostVarLength - ZTPV_CVAR_PARAM->mPosition; \
        if(sNumBytesToRead > sBytesRemain)                              \
        {                                                               \
            sNumBytesToRead = sBytesRemain;                             \
        }                                                               \
        stlMemcpy(buffer,                                               \
                  ZTPV_CVAR_PARAM->mBuffer + ZTPV_CVAR_PARAM->mPosition, \
                  sNumBytesToRead);                                     \
        res = sNumBytesToRead;                                          \
        ZTPV_CVAR_PARAM->mPosition += sNumBytesToRead;                  \
    } while (0)

#define YY_USER_ACTION                                                  \
    {                                                                   \
        stlChar * sStr;                                                 \
        if(yylineno != ZTPV_CVAR_PARAM->mLineNo)                        \
        {                                                               \
            ZTPV_CVAR_PARAM->mColumn = 1;                               \
            ZTPV_CVAR_PARAM->mLength = 0;                               \
            sStr = ZTPV_CVAR_PARAM->mBuffer + ZTPV_CVAR_PARAM->mNextLoc \
                + yyleng - 1;                                           \
            while(*sStr != '\n')                                        \
            {                                                           \
                ZTPV_CVAR_PARAM->mLength++;                             \
                sStr--;                                                 \
            }                                                           \
            ZTPV_CVAR_PARAM->mLength++;                                 \
        }                                                               \
        else                                                            \
        {                                                               \
            ZTPV_CVAR_PARAM->mColumn += ZTPV_CVAR_PARAM->mLength;       \
            ZTPV_CVAR_PARAM->mLength  = yyleng;                         \
        }                                                               \
        ZTPV_CVAR_PARAM->mLineNo  = yylineno;                           \
        ZTPV_CVAR_PARAM->mCurrLoc = ZTPV_CVAR_PARAM->mNextLoc;          \
        ZTPV_CVAR_PARAM->mNextLoc += yyleng;                            \
        *yylloc = ZTPV_CVAR_PARAM->mCurrLoc;                            \
    }
%}


%option reentrant noyywrap never-interactive nounistd
%option bison-bridge bison-locations
%option nounput
%option yylineno

/* SPECIAL CHARACTER */
comment_introducer         "/*"
comment_terminator         "*/"
c_simple_comment           "//"[^\n]*\n
sql_line_comment           "--"[^\n]*\n
block_comment              {comment_introducer}{comment_terminator}|{comment_introducer}([^+*])([^*]|(\*+[^*/]))*\**{comment_terminator}

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

/*
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
*/
assign_op                  ("="|"*="|"/="|"%="|"+="|"-="|"<<="|">>="|"&="|"^="|"|=")

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
whitespace                 ({space}|\\\n)+
comment                    ({c_simple_comment}|{sql_line_comment}|{block_comment})
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
char_literal               {CP}?{quote}([^\'\\\n]|{ES})+{quote}

fixed_literal              ({decimal_literal}|{octal_literal}|{hex_literal}|{char_literal})
float_literal              ({D}+{E}|{D}*"."{D}+{E}?|{D}+"."{E}?|{HP}{H}+{P}|{HP}{H}*"."{H}+{P}|{HP}{H}+"."{P}){FS}?

numeric_literal            {fixed_literal}|{float_literal}
/*
string_literal             ({SP}?{double_quote}([^\"\\\n]|{ES})*{double_quote}{space}*)+
*/
string_literal             ({SP}?{double_quote}([^\"\\\n]|{ES})*{double_quote})({space}*{SP}?{double_quote}([^\"\\\n]|{ES})*{double_quote})*

not_applicable             ([\000-\010\013-\014\016-\037\134\140\177\200-\377]+|{assign_op}|{quote}|{double_quote})

%%

{whitespace} {
    ZTPV_PARSE_PRINT("WHITESPACE: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    //return ZTP_TOK_C_DUMMY;
}

{not_applicable} {
    ZTPV_PARSE_PRINT("NOT_APPLICABLE: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_NOT_APPLICABLE;
}

{comment} {
    stlInt32   i;

    ZTPV_PARSE_PRINT("COMMENT: %s\n", yytext);
    ZTPV_PARSE_RETURN_STR(yytext);

    for( i = ZTPV_CVAR_PARAM->mCurrLoc; i < ZTPV_CVAR_PARAM->mNextLoc; i ++ )
    {
        ZTPV_CVAR_PARAM->mBuffer[i] = ' ';
    }
    //return ZTP_TOK_C_COMMENT;
}

{literal} {
    ZTPV_PARSE_PRINT("LITERAL: %s: yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_LITERAL;
}

{comma} {
    ZTPV_PARSE_PRINT("[,]: %s: yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_COMMA;
}

{left_paren} {
    ZTPV_PARSE_PRINT("[(]: %s: yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_LPAREN;
}

{right_paren} {
    ZTPV_PARSE_PRINT("[)]: %s: yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_RPAREN;
}

{left_bracket} {
    ZTPV_PARSE_PRINT("L_BRACKET: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_LBRACKET;
}

{right_bracket} {
    ZTPV_PARSE_PRINT("R_BRACKET: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_RBRACKET;
}

{period} {
    ZTPV_PARSE_PRINT("[.]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_PERIOD;
}

{asterisk} {
    ZTPV_PARSE_PRINT("[*]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_ASTERISK;
}

{ampersand} {
    ZTPV_PARSE_PRINT("[&]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_AMPERSAND;
}

{plus} {
    ZTPV_PARSE_PRINT("[+]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_PLUS;
}

{minus} {
    ZTPV_PARSE_PRINT("[-]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_MINUS;
}

{tilde} {
    ZTPV_PARSE_PRINT("[~]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_TILDE;
}

{exclamation} {
    ZTPV_PARSE_PRINT("[!]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_EXCLAMATION;
}

{solidus} {
    ZTPV_PARSE_PRINT("[/]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_SOLIDUS;
}

{percent} {
    ZTPV_PARSE_PRINT("[%]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_PERCENT;
}

{lt} {
    ZTPV_PARSE_PRINT("[<]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_LT;
}

{gt} {
    ZTPV_PARSE_PRINT("[>]: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_GT;
}

{circumflex} {
    ZTPV_PARSE_PRINT("CIRCUMFLEX: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_CIRCUMFLEX;
}

{vbar} {
    ZTPV_PARSE_PRINT("VBAR: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_VBAR;
}

{question} {
    ZTPV_PARSE_PRINT("QUESTION: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_QUESTION;
}

{semicolon} {
    ZTPV_PARSE_PRINT("SEMICOLON: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_SEMICOLON;
}

{colon} {
    ZTPV_PARSE_PRINT("COLON: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_COLON;
}

{shift_right} {
    ZTPV_PARSE_PRINT(">>: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_SR;
}

{shift_left} {
    ZTPV_PARSE_PRINT("<<: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_SL;
}

{double_plus} {
    ZTPV_PARSE_PRINT("++: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_INC;
}

{double_minus} {
    ZTPV_PARSE_PRINT("--: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_DEC;
}

{pointer_op} {
    ZTPV_PARSE_PRINT("->: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_PTR;
}

{double_ampersand} {
    ZTPV_PARSE_PRINT("&&: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_AND;
}

{double_vbar} {
    ZTPV_PARSE_PRINT("||: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_OR;
}

{le} {
    ZTPV_PARSE_PRINT("<=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_LE;
}

{ge} {
    ZTPV_PARSE_PRINT(">=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_GE;
}

{equal} {
    ZTPV_PARSE_PRINT("==: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_EQ;
}

{not_equal} {
    ZTPV_PARSE_PRINT("!=: yylloc : %d, leng : %d, \"%s\"\n", *yylloc, yyleng, yytext);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_SYM_NE;
}

{identifier} {
    ZTPV_PARSE_PRINT("IDENTIFER: %s: yylloc : %d, leng : %d\n", yytext, *yylloc, yyleng);
    ZTPV_PARSE_RETURN_STR(yytext);
    return ZTPV_TOK_ID;
}

%%

