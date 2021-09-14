/*******************************************************************************
 * ztdCmd.l
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

%{
#include <stl.h>
#include <goldilocks.h>
#include <ztdDef.h>
#include <ztdCmdParser.h>
#include <ztdCmd.tab.h>

#ifndef ZTD_PARSER_PRINT
#define ZTD_C_PARSE_PRINT stlIgnorePrintf
#else
#define ZTD_C_PARSE_PRINT stlPrintf
#endif

const ztdCmdKeyWord * ztdCmdScanKeyWordLookup( stlChar * aString );

#define ZTD_CMD_PARAM yyget_extra( yyscanner )

#define ZTD_CMD_PARSE_RETURN_QUOTE_STR( aStr )                                  \
    {                                                                           \
        stlInt32   sCursor  = 0;                                                \
        stlChar  * sCurChar = NULL;                                             \
        stlChar  * sResult  = NULL;                                             \
                                                                                \
        if( stlAllocRegionMem( ZTD_CMD_PARAM->mAllocator,                       \
                               yyleng + 1,                                      \
                               yylval,                                          \
                               ZTD_CMD_PARAM->mErrorStack ) != STL_SUCCESS )    \
        {                                                                       \
            ZTD_CMD_PARAM->mErrStatus = STL_FAILURE;                            \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            if( yyleng == 0 )                                                   \
            {                                                                   \
                ((stlChar*)(*yylval))[0] = '\0';                                \
            }                                                                   \
            else                                                                \
            {                                                                   \
                sCurChar = (stlChar*) aStr;                                     \
                sResult  = (stlChar*) *yylval;                                  \
                                                                                \
                for( sCursor = 0; sCursor < yyleng; sCursor++ )                 \
                {                                                               \
                    if( ( sCurChar[0] == '\'' ) &&                              \
                        ( sCurChar[1] == '\'' ) )                               \
                    {                                                           \
                        *sResult = '\'';                                        \
                        sCurChar++;                                             \
                        yyleng--;                                               \
                    }                                                           \
                    else                                                        \
                    {                                                           \
                        *sResult = *sCurChar;                                   \
                    }                                                           \
                                                                                \
                    sResult++;                                                  \
                    sCurChar++;                                                 \
                }                                                               \
                                                                                \
                ((stlChar*)(*yylval))[yyleng] = '\0';                           \
            }                                                                   \
            *yylloc = ZTD_CMD_PARAM->mCurPosition;                              \
        }                                                                       \
    }

#define ZTD_CMD_PARSE_RETURN_DOUBLE_QUOTE_STR( aStr )                           \
    {                                                                           \
        stlInt32   sCursor  = 0;                                                \
        stlChar  * sCurChar = NULL;                                             \
        stlChar  * sResult  = NULL;                                             \
                                                                                \
        if( stlAllocRegionMem( ZTD_CMD_PARAM->mAllocator,                       \
                               yyleng + 1,                                      \
                               yylval,                                          \
                               ZTD_CMD_PARAM->mErrorStack ) != STL_SUCCESS )    \
        {                                                                       \
            ZTD_CMD_PARAM->mErrStatus = STL_FAILURE;                            \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            if( yyleng == 0 )                                                   \
            {                                                                   \
                ((stlChar*)(*yylval))[0] = '\0';                                \
            }                                                                   \
            else                                                                \
            {                                                                   \
                sCurChar = (stlChar*) aStr;                                     \
                sResult  = (stlChar*) *yylval;                                  \
                                                                                \
                for( sCursor = 0; sCursor < yyleng; sCursor++ )                 \
                {                                                               \
                    if( ( sCurChar[0] == '\"' ) &&                              \
                        ( sCurChar[1] == '\"' ) )                               \
                    {                                                           \
                        *sResult = '\"';                                        \
                        sCurChar++;                                             \
                        yyleng--;                                               \
                    }                                                           \
                    else                                                        \
                    {                                                           \
                        *sResult = *sCurChar;                                   \
                    }                                                           \
                                                                                \
                    sResult++;                                                  \
                    sCurChar++;                                                 \
                }                                                               \
                                                                                \
                ((stlChar*)(*yylval))[yyleng] = '\0';                           \
            }                                                                   \
            *yylloc = ZTD_CMD_PARAM->mCurPosition;                              \
        }                                                                       \
    }
    
#define ZTD_CMD_PARSE_RETURN_STRING( aStr )                                     \
    {                                                                           \
        if( stlAllocRegionMem( ZTD_CMD_PARAM->mAllocator,                       \
                               yyleng + 1,                                      \
                               yylval,                                          \
                               ZTD_CMD_PARAM->mErrorStack ) != STL_SUCCESS )    \
        {                                                                       \
            ZTD_CMD_PARAM->mErrStatus = STL_FAILURE;                            \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            stlMemcpy( *yylval, aStr, yyleng );                                 \
            ((stlChar*)(*yylval))[yyleng] = '\0';                               \
            *yylloc = ZTD_CMD_PARAM->mCurPosition;                              \
        }                                                                       \
    }                                                                        


#define YY_INPUT( aBuffer, aRes, aMaxSize )                                     \
    do                                                                          \
    {                                                                           \
        if( ZTD_CMD_PARAM->mPosition >= ZTD_CMD_PARAM->mLength )                \
        {                                                                       \
            (aRes) = YY_NULL;                                                   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            (aRes) = ZTD_CMD_PARAM->mLength - ZTD_CMD_PARAM->mPosition;         \
            (aRes) > (stlInt32)(aMaxSize) ? (aRes) = (aMaxSize) : 0;            \
            stlMemcpy( (aBuffer),                                               \
                       ZTD_CMD_PARAM->mBuffer + ZTD_CMD_PARAM->mPosition,       \
                       (aRes) );                                                \
            ZTD_CMD_PARAM->mPosition += aRes;                                   \
        }                                                                       \
    } while( 0 )

#define YY_USER_ACTION                                                          \
    {                                                                           \
        stlChar * sStr;                                                         \
                                                                                \
        if( yylineno != ZTD_CMD_PARAM->mLineNo )                                \
        {                                                                       \
            ZTD_CMD_PARAM->mColumn = 1;                                         \
            ZTD_CMD_PARAM->mColumnLen = 0;                                      \
            sStr = ZTD_CMD_PARAM->mBuffer + ZTD_CMD_PARAM->mNextPosition        \
                + yyleng - 1;                                                   \
            while( *sStr != '\n' )                                              \
            {                                                                   \
                ZTD_CMD_PARAM->mColumnLen++;                                    \
                sStr--;                                                         \
            }                                                                   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            ZTD_CMD_PARAM->mColumn    += ZTD_CMD_PARAM->mColumnLen;             \
            ZTD_CMD_PARAM->mColumnLen = yyleng;                                 \
        }                                                                       \
                                                                                \
        *yylloc = ZTD_CMD_PARAM->mCurPosition;                                  \
        ZTD_CMD_PARAM->mLineNo  = yylineno;                                     \
        ZTD_CMD_PARAM->mCurPosition = ZTD_CMD_PARAM->mNextPosition;             \
        ZTD_CMD_PARAM->mNextPosition += yyleng;                                 \
    }
    
%}

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge bison-locations
%option case-insensitive
%option nounput
%option yylineno

digits                [0-9]*
letter                [A-Za-z_]
letters               [A-Za-z_0-9\$]
string                {letter}{letters}*

quote_symbol          "\'\'"
double_quote_symbol   "\"\""

quote                 "\'"
double_quote          "\""
period                "."

quote_string          {quote}([^\'\n\r]|{quote_symbol})+{quote}
double_quote_string   {double_quote}([^\"\n\r]|{double_quote_symbol})+{double_quote}

white_space           [ \t\n\r\v\f]+

/**
 * unused symbol: !"#$%&'()*+`-./:;<=>?@[\]\^_.{|}~
 * Lex에서 식별되지 않는 토큰(문자 또는 문자열)은 에러 처리를 할때 정확히 할수가 없다.
 * example) 'TABLE *' 라고 명시하고 '*'를 에러 처리하려면 *에 해당하는 토큰이 정의되어 있어야 한다.
 */
unused_symbol         [\041-\057\072-\100\133-\140\173-\177]

%%

{period} {
    ZTD_C_PARSE_PRINT( "PERIOD: yylloc : %d, leng : %d, \"%s\"\n",
                       *yylloc, yyleng, yytext );

    return ZTD_TOK_PERIOD;
}

{digits} {
    ZTD_C_PARSE_PRINT( "DIGITS: yylloc : %d, leng : %d, \"%s\"\n",
                       *yylloc, yyleng, yytext );

    ZTD_CMD_PARSE_RETURN_STRING( yytext );
    return ZTD_TOK_DIGITS;
}

{string} {
    const ztdCmdKeyWord * sKeyWord;

    sKeyWord = ztdCmdScanKeyWordLookup( yytext );

    if( sKeyWord == NULL )
    {
        ZTD_CMD_PARSE_RETURN_STRING( yytext );
        return ZTD_TOK_STRING;
    }
    else
    {
        return sKeyWord->mTokenCode;
    }
}

{quote_string} {
    ZTD_C_PARSE_PRINT( "QUOTE STRING: yylloc : %d, leng : %d, \"%s\"\n",
                       *yylloc, yyleng, yytext );
    yyleng -= 2;

    ZTD_CMD_PARSE_RETURN_QUOTE_STR( yytext + 1 );

    return ZTD_TOK_QUOTE_STRING;
}

{double_quote_string} {
    ZTD_C_PARSE_PRINT( "DOUBLE QUOTE STRING: yylloc : %d, leng : %d, \"%s\"\n",
                       *yylloc, yyleng, yytext );
    yyleng -= 2;

    ZTD_CMD_PARSE_RETURN_DOUBLE_QUOTE_STR( yytext + 1);

    return ZTD_TOK_DOUBLE_QUOTE_STRING;
}

{white_space} {
    ZTD_C_PARSE_PRINT( "WHITE_SPACE: yylloc : %d, leng : %d, \"%s\"\n",
                       *yylloc, yyleng, yytext );
}

{unused_symbol} {
    ZTD_C_PARSE_PRINT( "UNUSED_STRING yylloc : %d, leng : %d, \"%s\"\n",
                       *yylloc, yyleng, yytext );
    
    return ZTD_TOK_UNUSED_SYMBOL;
}

%%

const ztdCmdKeyWord ztdCmdKeyWordList[] =
{
    { "AND",                ZTD_TOK_AND                },
    { "BY",                 ZTD_TOK_BY                 },
    { "CHARACTERSET",       ZTD_TOK_CHARACTERSET       },
    { "ENCLOSED",           ZTD_TOK_ENCLOSED           },
    { "FIELDS",             ZTD_TOK_FIELDS             },
    { "OPTIONALLY",         ZTD_TOK_OPTIONALLY         },
    { "TABLE",              ZTD_TOK_TABLE              },
    { "TERMINATED",         ZTD_TOK_TERMINATED         }
};

const stlInt16  ztdCmdKeyWordCount = ZTD_CMD_KEY_WORD_COUNT( ztdCmdKeyWordList );


/**
 * @brief keyworkd 찾기
 * @param[in]   aString    문자열
 *
 * @return      keyword 정보
 */ 
const ztdCmdKeyWord * ztdCmdScanKeyWordLookup( stlChar * aString )
{
    const ztdCmdKeyWord * sLow;
    const ztdCmdKeyWord * sMiddle;
    const ztdCmdKeyWord * sHigh;
    const ztdCmdKeyWord * sResult;
    stlInt32              sDifference;

    /**
     * scan keyword
     */ 

    sLow    = ztdCmdKeyWordList;
    sHigh   = &ztdCmdKeyWordList[ ztdCmdKeyWordCount - 1 ];
    sResult = NULL;

    while( sLow <= sHigh )
    {
        sMiddle = sLow + ( ( sHigh - sLow ) / 2 );

        sDifference = stlStrcasecmp( sMiddle->mName, aString );

        if( sDifference == 0 )
        {
            sResult = sMiddle;
            break;
        }
        else if( sDifference < 0 )
        {
            sLow = sMiddle + 1;
        }
        else
        {
            sHigh = sMiddle - 1;
        }
    }

    return sResult;
}
