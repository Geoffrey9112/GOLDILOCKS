/*******************************************************************************
 * ztq.l
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztq.l 1957 2011-09-21 06:16:18Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

%{
#include <stl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztqDef.h>
#include <ztqCmd.tab.h>
#include <ztqCmdParser.h>

const ztqCmdKeyWord * ztqCmdScanKeyWordLookup( stlChar * aIdentifier );

#define ZTQ_CMD_PARAM yyget_extra( yyscanner )
#define ZTQ_CMD_PARSE_RETURN_STR( aStr )                                        \
    {                                                                           \
        if( stlAllocRegionMem( ZTQ_CMD_PARAM->mAllocator,                       \
                               stlStrlen(aStr) + 1,                             \
                               yylval,                                          \
                               ZTQ_CMD_PARAM->mErrorStack ) != STL_SUCCESS )    \
        {                                                                       \
            ZTQ_CMD_PARAM->mErrStatus = STL_FAILURE;                            \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            stlMemcpy( *yylval, aStr, stlStrlen(aStr) + 1 );                    \
        }                                                                       \
    }

#define YY_INPUT( aBuffer, aRes, aMaxSize )                                 \
    do                                                                      \
    {                                                                       \
        if( ZTQ_CMD_PARAM->mPosition >= ZTQ_CMD_PARAM->mLength )            \
        {                                                                   \
            aRes = YY_NULL;                                                 \
        }                                                                   \
        else                                                                \
        {                                                                   \
            aRes = ZTQ_CMD_PARAM->mLength - ZTQ_CMD_PARAM->mPosition;       \
            aRes > (stlInt32)aMaxSize ? aRes = aMaxSize : 0;                \
            stlMemcpy( aBuffer,                                             \
                       ZTQ_CMD_PARAM->mBuffer + ZTQ_CMD_PARAM->mPosition,   \
                       aRes );                                              \
            ZTQ_CMD_PARAM->mPosition += aRes;                               \
        }                                                                   \
    } while( 0 )

#define YY_USER_ACTION                                              \
    {                                                               \
        if( yylineno != ZTQ_CMD_PARAM->mLineNo )                    \
        {                                                           \
            ZTQ_CMD_PARAM->mColumn = 0;                             \
        }                                                           \
        else                                                        \
        {                                                           \
            ZTQ_CMD_PARAM->mColumn += ZTQ_CMD_PARAM->mColumnLen;    \
        }                                                           \
        ZTQ_CMD_PARAM->mLineNo = yylineno;                          \
        ZTQ_CMD_PARAM->mColumnLen = yyleng;                         \
        ZTQ_CMD_PARAM->mCurPosition += yyleng;                      \
    }
%}


%option reentrant noyywrap never-interactive nounistd
%option bison-bridge
%option case-insensitive
%option nounput
%option yylineno

%%
\\alloc                     return ZTQ_CMD_TOK_SLASH_ALLOC;
\\connect                   return ZTQ_CMD_TOK_SLASH_CONNECT;
\\desc                      return ZTQ_CMD_TOK_SLASH_DESC;
\\free                      return ZTQ_CMD_TOK_SLASH_FREE;
\\idesc                     return ZTQ_CMD_TOK_SLASH_IDESC;
\\ed                        return ZTQ_CMD_TOK_SLASH_EDIT;
\\edit                      return ZTQ_CMD_TOK_SLASH_EDIT;
\\exec                      return ZTQ_CMD_TOK_SLASH_EXEC;
\\execute                   return ZTQ_CMD_TOK_SLASH_EXEC;
\\dynamic                   return ZTQ_CMD_TOK_SLASH_DYNAMIC;
\\help                      return ZTQ_CMD_TOK_SLASH_HELP;
\\history                   return ZTQ_CMD_TOK_SLASH_HISTORY;
\\hi                        return ZTQ_CMD_TOK_SLASH_HISTORY;
\\i                         return ZTQ_CMD_TOK_SLASH_IMPORT;
\\import                    return ZTQ_CMD_TOK_SLASH_IMPORT;
\\q                         return ZTQ_CMD_TOK_SLASH_QUIT;
\\prepare                   return ZTQ_CMD_TOK_SLASH_PREPARE;
\\print                     return ZTQ_CMD_TOK_SLASH_PRINT;
\\quit                      return ZTQ_CMD_TOK_SLASH_QUIT;
\\set                       return ZTQ_CMD_TOK_SLASH_SET;
\\var                       return ZTQ_CMD_TOK_SLASH_VAR;
\\use                       return ZTQ_CMD_TOK_SLASH_USE;
\\explain                   return ZTQ_CMD_TOK_SLASH_EXPLAIN;
\\startup                   return ZTQ_CMD_TOK_SLASH_STARTUP;
\\shutdown                  return ZTQ_CMD_TOK_SLASH_SHUTDOWN;
\\spo                       return ZTQ_CMD_TOK_SLASH_SPOOL;
\\spool                     return ZTQ_CMD_TOK_SLASH_SPOOL;
\\xa                        return ZTQ_CMD_TOK_SLASH_XA;
\\ddl_db                    return ZTQ_CMD_TOK_SLASH_DDL_DB;
\\ddl_tablespace            return ZTQ_CMD_TOK_SLASH_DDL_SPACE;
\\ddl_profile               return ZTQ_CMD_TOK_SLASH_DDL_PROFILE;
\\ddl_auth                  return ZTQ_CMD_TOK_SLASH_DDL_AUTH;
\\ddl_schema                return ZTQ_CMD_TOK_SLASH_DDL_SCHEMA;
\\ddl_public_synonym        return ZTQ_CMD_TOK_SLASH_DDL_PUBLIC_SYNONYM;
\\ddl_table                 return ZTQ_CMD_TOK_SLASH_DDL_TABLE;
\\ddl_view                  return ZTQ_CMD_TOK_SLASH_DDL_VIEW;
\\ddl_sequence              return ZTQ_CMD_TOK_SLASH_DDL_SEQUENCE;
\\ddl_index                 return ZTQ_CMD_TOK_SLASH_DDL_INDEX;
\\ddl_constraint            return ZTQ_CMD_TOK_SLASH_DDL_CONSTRAINT;
\\ddl_synonym               return ZTQ_CMD_TOK_SLASH_DDL_SYNONYM;
\\\\                        return ZTQ_CMD_TOK_SLASH_SLASH;
\\[0-9]+                    {
                                ZTQ_CMD_PARSE_RETURN_STR(yytext + 1);
                                return ZTQ_CMD_TOK_SLASH_NUMBER;
                            }
as                          return ZTQ_CMD_TOK_AS;
"("                         return ZTQ_CMD_TOK_OPEN_PARENTHESES;
")"                         return ZTQ_CMD_TOK_CLOSE_PARENTHESES;
"."                         return ZTQ_CMD_TOK_PERIOD;
","                         return ZTQ_CMD_TOK_COMMA;
"|"                         return ZTQ_CMD_TOK_OR;
:=                          {
                                ZTQ_CMD_PARAM->mSqlPosition = ZTQ_CMD_PARAM->mCurPosition;
                                return ZTQ_CMD_TOK_HOST_ASSIGN;
                            }
[\n\r]+                     /* ignore end of line */;
[ \t]+                      /* ignore whitespace */;
:[a-zA-Z_][a-zA-Z_0-9$]*    {
                                ZTQ_CMD_PARSE_RETURN_STR(yytext + 1);
                                return ZTQ_CMD_TOK_HOST_VARIABLE;
                            }
[+\-]?[0-9][0-9]*           {
                                ZTQ_CMD_PARSE_RETURN_STR(yytext);
                                return ZTQ_CMD_TOK_NUMBER;
                            }
[a-zA-Z_][a-zA-Z_0-9$]*     {
                                const ztqCmdKeyWord * sKeyWord;

                                sKeyWord = ztqCmdScanKeyWordLookup( yytext );
                                if( sKeyWord == NULL )
                                {
                                    ZTQ_CMD_PARSE_RETURN_STR( yytext );
                                    return ZTQ_CMD_TOK_IDENTIFIER;
                                }
                                else
                                {
                                    ZTQ_CMD_PARSE_RETURN_STR( sKeyWord->mName );
                                    return sKeyWord->mTokenCode;
                                }
                            }
"\""[^"]*"\""               { 
                                ZTQ_CMD_PARSE_RETURN_STR(yytext);
                                return ZTQ_CMD_TOK_DOUBLE_QUOTE_STRING;
                            }
"'"[^']*"'"                 {
                                yytext[stlStrlen(yytext) - 1] = '\0'; 
                                ZTQ_CMD_PARSE_RETURN_STR(yytext + 1);
                                return ZTQ_CMD_TOK_STRING;
                            }
[\041-\047\052-\053\055\057\072-\100\133-\140\173-\176][\041\043-\046\052-\053\055\057\072-\100\133-\140\173-\176]*   {
                                ZTQ_CMD_PARSE_RETURN_STR(yytext);
                                return ZTQ_CMD_TOK_DUMMY;
                            }

%%

const ztqCmdKeyWord ztqCmdKeyWordList[] = 
{
    { "ABORT",           ZTQ_CMD_TOK_ABORT           },
    { "ADMIN",           ZTQ_CMD_TOK_ADMIN           },
    { "ALTER",           ZTQ_CMD_TOK_ALTER           },
    { "APPEND",          ZTQ_CMD_TOK_APPEND          },
    { "ASYNC",           ZTQ_CMD_TOK_ASYNC           },
    { "AUTOCOMMIT",      ZTQ_CMD_TOK_AUTOCOMMIT      },
    { "BIGINT",          ZTQ_CMD_TOK_BIGINT          },
    { "BINARY",          ZTQ_CMD_TOK_BINARY          },
    { "BOOLEAN",         ZTQ_CMD_TOK_BOOLEAN         },
    { "BYTE",            ZTQ_CMD_TOK_BYTE            },
    { "CALLSTACK",       ZTQ_CMD_TOK_CALLSTACK       },
    { "CHAR",            ZTQ_CMD_TOK_CHAR            },
    { "CHARACTER",       ZTQ_CMD_TOK_CHARACTER       },
    { "CHARACTERS",      ZTQ_CMD_TOK_CHARACTERS      },
    { "CLOSE",           ZTQ_CMD_TOK_CLOSE           },
    { "COLOR",           ZTQ_CMD_TOK_COLOR           },
    { "COLSIZE",         ZTQ_CMD_TOK_COLSIZE         },
    { "COMMENT",         ZTQ_CMD_TOK_COMMENT         },
    { "COMMIT",          ZTQ_CMD_TOK_COMMIT          },
    { "CONSTRAINT",      ZTQ_CMD_TOK_CONSTRAINT      },
    { "CREATE",          ZTQ_CMD_TOK_CREATE          },
    { "DATE",            ZTQ_CMD_TOK_DATE            },
    { "DAY",             ZTQ_CMD_TOK_DAY             },
    { "DDLSIZE",         ZTQ_CMD_TOK_DDLSIZE         },
    { "DOUBLE",          ZTQ_CMD_TOK_DOUBLE          },
    { "ECHO",            ZTQ_CMD_TOK_ECHO            },
    { "END",             ZTQ_CMD_TOK_END             },
    { "ERROR",           ZTQ_CMD_TOK_ERROR           },
    { "EXEC",            ZTQ_CMD_TOK_EXEC            },
    { "FAIL",            ZTQ_CMD_TOK_FAIL            },
    { "FLOAT",           ZTQ_CMD_TOK_FLOAT           },
    { "FORGET",          ZTQ_CMD_TOK_FORGET          },
    { "GRANT",           ZTQ_CMD_TOK_GRANT           },
    { "HISTORY",         ZTQ_CMD_TOK_HISTORY         },
    { "HOUR",            ZTQ_CMD_TOK_HOUR            },
    { "IDENTITY",        ZTQ_CMD_TOK_IDENTITY        },
    { "IMMEDIATE",       ZTQ_CMD_TOK_IMMEDIATE       },
    { "INDEX",           ZTQ_CMD_TOK_INDEX           },
    { "INTEGER",         ZTQ_CMD_TOK_INTEGER         },
    { "INTERVAL",        ZTQ_CMD_TOK_INTERVAL        },
    { "JOIN",            ZTQ_CMD_TOK_JOIN            },
    { "LINESIZE",        ZTQ_CMD_TOK_LINESIZE        },
    { "LONG",            ZTQ_CMD_TOK_LONG            },
    { "MIGRATE",         ZTQ_CMD_TOK_MIGRATE         },
    { "MINUTE",          ZTQ_CMD_TOK_MINUTE          },
    { "MONTH",           ZTQ_CMD_TOK_MONTH           },
    { "MOUNT",           ZTQ_CMD_TOK_MOUNT           },
    { "MULTIPLE",        ZTQ_CMD_TOK_MULTIPLE        },
    { "NATIVE_BIGINT",   ZTQ_CMD_TOK_NATIVE_BIGINT   },
    { "NATIVE_DOUBLE",   ZTQ_CMD_TOK_NATIVE_DOUBLE   },
    { "NATIVE_INTEGER",  ZTQ_CMD_TOK_NATIVE_INTEGER  },
    { "NATIVE_REAL",     ZTQ_CMD_TOK_NATIVE_REAL     },
    { "NATIVE_SMALLINT", ZTQ_CMD_TOK_NATIVE_SMALLINT },
    { "NOMOUNT",         ZTQ_CMD_TOK_NOMOUNT         },
    { "NORMAL",          ZTQ_CMD_TOK_NORMAL          },
    { "NOWAIT",          ZTQ_CMD_TOK_NOWAIT          },
    { "NULL",            ZTQ_CMD_TOK_NULL            },
    { "NUMBER",          ZTQ_CMD_TOK_NUMBERTYPE      },
    { "NUMERIC",         ZTQ_CMD_TOK_NUMERIC         },
    { "NUMSIZE",         ZTQ_CMD_TOK_NUMSIZE         },
    { "OCTETS",          ZTQ_CMD_TOK_OCTETS          },
    { "OFF",             ZTQ_CMD_TOK_OFF             },
    { "ON",              ZTQ_CMD_TOK_ON              },
    { "ONEPHASE",        ZTQ_CMD_TOK_ONEPHASE        },
    { "ONLY",            ZTQ_CMD_TOK_ONLY            },
    { "OPEN",            ZTQ_CMD_TOK_OPEN            },
    { "OPERATIONAL",     ZTQ_CMD_TOK_OPERATIONAL     },
    { "PAGESIZE",        ZTQ_CMD_TOK_PAGESIZE        },
    { "PATH",            ZTQ_CMD_TOK_PATH            },
    { "PLAN",            ZTQ_CMD_TOK_PLAN            },
    { "PRECISION",       ZTQ_CMD_TOK_PRECISION       },
    { "PREPARE",         ZTQ_CMD_TOK_PREPARE         },
    { "REAL",            ZTQ_CMD_TOK_REAL            },
    { "RECOVER",         ZTQ_CMD_TOK_RECOVER         },
    { "REPLACE",         ZTQ_CMD_TOK_REPLACE         },
    { "RESTART",         ZTQ_CMD_TOK_RESTART         },
    { "RESULT",          ZTQ_CMD_TOK_RESULT          },
    { "RESUME",          ZTQ_CMD_TOK_RESUME          },
    { "ROLLBACK",        ZTQ_CMD_TOK_ROLLBACK        },
    { "ROWID",           ZTQ_CMD_TOK_ROWID           },
    { "SCHEMA",          ZTQ_CMD_TOK_SCHEMA          },
    { "SECOND",          ZTQ_CMD_TOK_SECOND          },
    { "SEQUENCE",        ZTQ_CMD_TOK_SEQUENCE        },
    { "SMALLINT",        ZTQ_CMD_TOK_SMALLINT        },
    { "SQL",             ZTQ_CMD_TOK_SQL             },
    { "START",           ZTQ_CMD_TOK_START           },
    { "SUCCESS",         ZTQ_CMD_TOK_SUCCESS         },
    { "SUPPLEMENTAL",    ZTQ_CMD_TOK_SUPPLEMENTAL    },
    { "SUSPEND",         ZTQ_CMD_TOK_SUSPEND         },
    { "SYNONYM",         ZTQ_CMD_TOK_SYNONYM         },
    { "SYSDBA",          ZTQ_CMD_TOK_SYSDBA          },
    { "TABLE",           ZTQ_CMD_TOK_TABLE           },
    { "TIME",            ZTQ_CMD_TOK_TIME            },
    { "TIMESTAMP",       ZTQ_CMD_TOK_TIMESTAMP       },
    { "TIMING",          ZTQ_CMD_TOK_TIMING          },
    { "TO",              ZTQ_CMD_TOK_TO              },
    { "TRANSACTIONAL",   ZTQ_CMD_TOK_TRANSACTIONAL   },
    { "USER",            ZTQ_CMD_TOK_USER            },
    { "VARBINARY",       ZTQ_CMD_TOK_VARBINARY       },
    { "VARCHAR",         ZTQ_CMD_TOK_VARCHAR         },
    { "VARYING",         ZTQ_CMD_TOK_VARYING         },
    { "VERBOSE",         ZTQ_CMD_TOK_VERBOSE         },
    { "VERTICAL",        ZTQ_CMD_TOK_VERTICAL        },
    { "VIEW",            ZTQ_CMD_TOK_VIEW            },
    { "WITH",            ZTQ_CMD_TOK_WITH            },
    { "YEAR",            ZTQ_CMD_TOK_YEAR            },
    { "ZONE",            ZTQ_CMD_TOK_ZONE            }
};

const stlInt16  ztqCmdKeyWordCount = ZTQ_CMD_KEY_WORD_COUNT( ztqCmdKeyWordList );

/*
 * @brief keyword 찾기
 * @param[in,out]   aIdentifier    문자열
 *
 * @return keyword 정보
 */
const ztqCmdKeyWord * ztqCmdScanKeyWordLookup( stlChar * aIdentifier )
{
    const ztqCmdKeyWord  *  sLow;
    const ztqCmdKeyWord  *  sHigh;
    const ztqCmdKeyWord  *  sMiddle;
    const ztqCmdKeyWord  *  sResultKeyWord;
    stlInt32                sDifference;

    
    /*
     * scan keyword
     */

    sLow           = ztqCmdKeyWordList;
    sHigh          = & ztqCmdKeyWordList[ ztqCmdKeyWordCount - 1 ];
    sResultKeyWord = NULL;
            
    while( sLow <= sHigh )
    {
        sMiddle = sLow + ( ( sHigh - sLow ) / 2 );
        
        sDifference = stlStrcasecmp( sMiddle->mName, aIdentifier );
        
        if( sDifference == 0 )
        {
            sResultKeyWord = sMiddle;
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

    
    /*
     * output 설정
     */

    return sResultKeyWord;
}
