/*******************************************************************************
 * qlpParserMacroPart_1.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        vId: qlpParserMacroPart_1.h 2754 2011-12-15 02:27:23Z jhkim v
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLPPARSERMACROPART_1_H_
#define _QLPPARSERMACROPART_1_H_ 1

/**
 * @file qlpParserMacroPart_1.h
 * @brief SQL Processor Layer Parser macro Part #1
 */

#include <qlpParse.h>

/**
 * @addtogroup qlpParser
 * @{
 */


/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

stlStatus qlpAllocator( void            * aContext,
                        stlInt32          aAllocSize,
                        void           ** aAddr,
                        stlErrorStack   * aErrorStack );

stlChar  *qlpMakeHostVariable( stlParseParam * aParam );


#define QLP_GET_LAST_POS( loc ) ( loc >= param->mCurrLoc ? param->mNextLoc :                    \
                                  ( YYCHAR == YYEOF ? param->mNextLoc : param->mCurrLoc ) )

#ifdef STL_DEBUG
#define QLP_GET_HINT_ERROR_FROM_PROPERTY( _aEnableHintErr )                         \
{                                                                                   \
    STL_DASSERT( knlGetPropertyValueByID(                                           \
                     KNL_PROPERTY_HINT_ERROR,                                       \
                     (_aEnableHintErr),                                             \
                     KNL_ENV( QLP_PARSE_GET_ENV( param ) ) ) == STL_SUCCESS );      \
}
#else
#define QLP_GET_HINT_ERROR_FROM_PROPERTY( _aEnableHintErr )                         \
{                                                                                   \
    knlGetPropertyValueByID(                                                        \
        KNL_PROPERTY_HINT_ERROR,                                                    \
        (_aEnableHintErr),                                                          \
        KNL_ENV( QLP_PARSE_GET_ENV( param ) ) );                                    \
}
#endif

/*******************************************************************************
 * CODES
 ******************************************************************************/

/**
 * start_text
 */

#define STL_PARSER_START_TEXT____COMMAND( )     \
    {                                           \
    }                                           \

#define STL_PARSER_START_TEXT____COMMAND_TERMINATOR( )  \
    {                                                   \
    }                                                   \

/**
 * command
 */

#define STL_PARSER_COMMAND____EMPTY( )   \
    {                                    \
        param->mParseTree = NULL;        \
    }

#define STL_PARSER_COMMAND____ALTER_DATABASE( )   \
    {                                             \
        param->mParseTree = v1;                   \
    }

#define STL_PARSER_COMMAND____USER_DEFINITION( )    \
    {                                               \
        param->mParseTree = v1;                     \
    }

#define STL_PARSER_COMMAND____DROP_USER( )      \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____ALTER_USER( )     \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____PROFILE_DEFINITION( )    \
    {                                                  \
        param->mParseTree = v1;                        \
    }

#define STL_PARSER_COMMAND____DROP_PROFILE( )          \
    {                                                  \
        param->mParseTree = v1;                        \
    }

#define STL_PARSER_COMMAND____ALTER_PROFILE( )         \
    {                                                  \
        param->mParseTree = v1;                        \
    }

#define STL_PARSER_COMMAND____GRANT_PRIVILEGE( )    \
    {                                               \
        param->mParseTree = v1;                     \
    }

#define STL_PARSER_COMMAND____REVOKE_PRIVILEGE( )   \
    {                                               \
        param->mParseTree = v1;                     \
    }

#define STL_PARSER_COMMAND____TABLESPACE_DEFINITION( )  \
    {                                                   \
        param->mParseTree = v1;                         \
    }

#define STL_PARSER_COMMAND____ALTER_TABLESPACE( )   \
    {                                               \
        param->mParseTree = v1;                     \
    }

#define STL_PARSER_COMMAND____DROP_TABLESPACE( )        \
    {                                                   \
        param->mParseTree = v1;                         \
    }

#define STL_PARSER_COMMAND____SCHEMA_DEFINITION( )      \
    {                                                   \
        param->mParseTree = v1;                         \
    }

#define STL_PARSER_COMMAND____DROP_SCHEMA( )    \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____TABLE_DEFINITION( )       \
    {                                                   \
        param->mParseTree = v1;                         \
    }

#define STL_PARSER_COMMAND____DROP_TABLE( )     \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____ALTER_TABLE( )    \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____VIEW_DEFINITION( )    \
    {                                               \
        param->mParseTree = v1;                     \
    }

#define STL_PARSER_COMMAND____DROP_VIEW( )      \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____ALTER_VIEW( )     \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____SEQUENCE_DEFINITION( )    \
    {                                                   \
        param->mParseTree = v1;                         \
    }

#define STL_PARSER_COMMAND____DROP_SEQUENCE( )  \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____ALTER_SEQUENCE()  \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____SYNONYM_DEFINITION( )    \
    {                                                  \
        param->mParseTree = v1;                        \
    }

#define STL_PARSER_COMMAND____DROP_SYNONYM( )  \
    {                                          \
        param->mParseTree = v1;                \
    }

#define STL_PARSER_COMMAND____INDEX_DEFINITION( )       \
    {                                                   \
        param->mParseTree = v1;                         \
    }

#define STL_PARSER_COMMAND____DROP_INDEX( )     \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____ALTER_INDEX( )    \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____TRUNCATE_TABLE( ) \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____INSERT( )         \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____UPDATE( )         \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____UPDATE_POSITIONED( )  \
    {                                               \
        param->mParseTree = v1;                     \
    }

#define STL_PARSER_COMMAND____DELETE( )         \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____DELETE_POSITIONED( )  \
    {                                               \
        param->mParseTree = v1;                     \
    }

#define STL_PARSER_COMMAND____SELECT( )         \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____SELECT_INTO( )    \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____SAVEPOINT( )      \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____RELEASE_SAVEPOINT( )  \
    {                                               \
        param->mParseTree = v1;                     \
    }

#define STL_PARSER_COMMAND____COMMIT( )         \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____ROLLBACK( )       \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____ALTER_SYSTEM( )   \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____ALTER_SESSION( )  \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____LOCK_TABLE()      \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____COMMENT()         \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____SET_CONSTRAINT_MODE() \
    {                                               \
        param->mParseTree = v1;                     \
    }

#define STL_PARSER_COMMAND____SET_TRANSACTION() \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____SET_SESSION_CHARACTERISTICS()     \
    {                                                           \
        param->mParseTree = v1;                                 \
    }

#define STL_PARSER_COMMAND____SET_SESSION_AUTH()    \
    {                                               \
        param->mParseTree = v1;                     \
    }

#define STL_PARSER_COMMAND____SET_TIME_ZONE()   \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____DECLARE_STATEMENT()   \
    {                                               \
        param->mParseTree = v1;                     \
    }

#define STL_PARSER_COMMAND____DECLARE_CURSOR()  \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____OPEN_CURSOR()     \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____CLOSE_CURSOR()    \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____FETCH_CURSOR()    \
    {                                           \
        param->mParseTree = v1;                 \
    }

#define STL_PARSER_COMMAND____CONNECTION()      \
    {                                           \
        param->mParseTree = v1;                 \
    }

/**
 * user_definition
 */

#define STL_PARSE_USER_DEFINTION____WITH_SCHEMA_EMPTY()                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeUserDef( param,                                   \
                                               a1,                                      \
                                               v3,                                      \
                                               v6,                                      \
                                               v10,                                     \
                                               v11,                                     \
                                               v3,                                      \
                                               v7,                                      \
                                               (v8 == NULL) ? STL_FALSE : STL_TRUE,     \
                                               (v9 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

#define STL_PARSE_USER_DEFINTION____WITH_SCHEMA_NAME()                                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeUserDef( param,                                   \
                                               a1,                                      \
                                               v3,                                      \
                                               v6,                                      \
                                               v10,                                     \
                                               v11,                                     \
                                               v14,                                     \
                                               v7,                                      \
                                               (v8 == NULL) ? STL_FALSE : STL_TRUE,     \
                                               (v9 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

#define STL_PARSE_USER_DEFINTION____WITHOUT_SCHEMA_NAME()                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeUserDef( param,                                   \
                                               a1,                                      \
                                               v3,                                      \
                                               v6,                                      \
                                               v10,                                     \
                                               v11,                                     \
                                               NULL,                                    \
                                               v7,                                      \
                                               (v8 == NULL) ? STL_FALSE : STL_TRUE,     \
                                               (v9 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

#define STL_PARSE_SENSITIVE_PASSWORD____REGULAR()                       \
    {                                                                   \
        STL_PARSER_CHECK( qlpCheckIdentifierLength( param, v1 ) );      \
        STL_PARSER_CHECK( v0 = qlpMakeSensitiveString( param, v1 ) );   \
    }
#define STL_PARSE_SENSITIVE_PASSWORD____DELIMITED() \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSE_USER_PROFILE____NULL() \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSE_USER_PROFILE____NAME() \
    {                                           \
        STL_PARSER_CHECK( v0 = v2 );            \
    }

#define STL_PARSE_USER_PROFILE____DEFAULT()                                     \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a2, "DEFAULT" ) );    \
    }

#define STL_PARSE_PASSWORD_EXPIRE____EMPTY()    \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSE_PASSWORD_EXPIRE____EXIST()                                        \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, STL_TRUE )  );   \
    }
#define STL_PARSE_ACCOUNT____UNLOCK()           \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }
#define STL_PARSE_ACCOUNT____LOCK()                                                 \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, STL_TRUE )  );   \
    }

#define STL_PARSE_DEFAULT_TABLESPACE____EMPTY() \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSE_DEFAULT_TABLESPACE____NAME()  \
    {                                           \
        STL_PARSER_CHECK( v0 = v3 );            \
    }

#define STL_PARSE_TEMPORARY_TABLESPACE____EMPTY()   \
    {                                               \
        STL_PARSER_CHECK( v0 = NULL );              \
    }

#define STL_PARSE_TEMPORARY_TABLESPACE____NAME() \
    {                                           \
        STL_PARSER_CHECK( v0 = v3 );            \
    }


/**
 * drop_user_statement
 */

#define STL_PARSER_DROP_USER_STATEMENT()                                                    \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeDropUser( param,                                      \
                                                a1,                                         \
                                                (v3 == NULL) ? STL_FALSE : STL_TRUE,        \
                                                v4,                                         \
                                                (v5 == NULL) ? STL_FALSE : STL_TRUE ) );    \
    }


/*
 * alter_user_statement
 */

#define STL_PARSER_ALTER_USER_STATEMENT____PASSWORD_ONLY()                          \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterUser( param,                             \
                                                 a1,                                \
                                                 QLP_ALTER_USER_ACTION_PASSWORD,    \
                                                 v3,                                \
                                                 v6,                                \
                                                 NULL,                              \
                                                 NULL,                              \
                                                 NULL,                              \
                                                 NULL ) );                          \
    }
#define STL_PARSER_ALTER_USER_STATEMENT____PASSWORD_REPLACE()                       \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterUser( param,                             \
                                                 a1,                                \
                                                 QLP_ALTER_USER_ACTION_PASSWORD,    \
                                                 v3,                                \
                                                 v6,                                \
                                                 v8,                                \
                                                 NULL,                              \
                                                 NULL,                              \
                                                 NULL ) );                          \
    }

#define STL_PARSER_ALTER_USER_STATEMENT____ALTER_PROFILE()                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterUser( param,                                 \
                                                 a1,                                    \
                                                 QLP_ALTER_USER_ACTION_ALTER_PROFILE,   \
                                                 v3,                                    \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 v4 ) );                                \
    }

#define STL_PARSER_ALTER_USER_STATEMENT____PASSWORD_EXPIRE()                            \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterUser( param,                                 \
                                                 a1,                                    \
                                                 QLP_ALTER_USER_ACTION_PASSWORD_EXPIRE, \
                                                 v3,                                    \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 NULL ) );                              \
    }

#define STL_PARSER_ALTER_USER_STATEMENT____ACCOUNT_LOCK()                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterUser( param,                                 \
                                                 a1,                                    \
                                                 QLP_ALTER_USER_ACTION_ACCOUNT_LOCK,    \
                                                 v3,                                    \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 NULL ) );                              \
    }
#define STL_PARSER_ALTER_USER_STATEMENT____ACCOUNT_UNLOCK()                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterUser( param,                                 \
                                                 a1,                                    \
                                                 QLP_ALTER_USER_ACTION_ACCOUNT_UNLOCK,  \
                                                 v3,                                    \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 NULL ) );                              \
    }

#define STL_PARSER_ALTER_USER_STATEMENT____DEFAULT_SPACE()                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterUser( param,                                 \
                                                 a1,                                    \
                                                 QLP_ALTER_USER_ACTION_DEFAULT_SPACE,   \
                                                 v3,                                    \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 v6,                                    \
                                                 NULL,                                  \
                                                 NULL ) );                              \
    }

#define STL_PARSER_ALTER_USER_STATEMENT____TEMPORARY_SPACE()                            \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterUser( param,                                 \
                                                 a1,                                    \
                                                 QLP_ALTER_USER_ACTION_TEMPORARY_SPACE, \
                                                 v3,                                    \
                                                 NULL,                                  \
                                                 NULL,                                  \
                                                 v6,                                    \
                                                 NULL,                                  \
                                                 NULL ) );                              \
    }

#define STL_PARSER_ALTER_USER_STATEMENT____SCHEMA_PATH()                            \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterUser( param,                             \
                                                 a1,                                \
                                                 QLP_ALTER_USER_ACTION_SCHEMA_PATH, \
                                                 v3,                                \
                                                 NULL,                              \
                                                 NULL,                              \
                                                 NULL,                              \
                                                 v7,                                \
                                                 NULL ) );                          \
    }

#define STL_PARSER_SCHEMA_NAME_LIST____SCHEMA_NAME()                    \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_SCHEMA_NAME_LIST____SCHEMA_NAME_LIST()           \
    {                                                               \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );  \
        STL_PARSER_CHECK( v0 = v1 );                                \
    }

#define STL_PARSER_SCHEMA_NAME____IDENTIFIER()  \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_SCHEMA_NAME____CURRENT_PATH()                                                \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a1, QLP_ALTER_USER_CURRENT_PATH ) );  \
    }

/**
 * profile_definition
 */
        
#define STL_PARSER_PROFILE_DEFINITION____()                \
    {                                                      \
        STL_PARSER_CHECK( v0 = qlpMakeProfileDef( param,   \
                                                  a1,      \
                                                  v3,      \
                                                  v5 ) );  \
    }


/**
 * password_parameter_list
 */

#define STL_PARSER_PASSWORD_PARAMETERS____PARAMETER()                   \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_PASSWORD_PARAMETERS____PARAMETER_LIST()                       \
    {                                                                            \
        STL_PARSER_CHECK( qlpCheckDuplicatePasswordParameter( param, v1, v2 ) ); \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v2 ) );               \
        STL_PARSER_CHECK( v0 = v1 );                                             \
    }

/**
 * password_parameter
 */

#define STL_PARSER_PASSWORD_PARAMETER____FAILED_LOGIN_ATTEMPTS() \
    {                                                            \
        STL_PARSER_CHECK( v0 = v1 );                             \
    }                                                            \

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_LIFE_TIME()   \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }                                                           \

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_LOCK_TIME()   \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }                                                           \

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_GRACE_TIME()  \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }                                                           \

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_REUSE_MAX()   \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }    

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_REUSE_TIME()  \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }    

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_VERIFY_FUNCTION()  \
    {                                                                \
        STL_PARSER_CHECK( v0 = v1 );                                 \
    }    


/**
 * failed_login_attempts
 */

#define STL_PARSER_FAILED_LOGIN_ATTEMPTS____UNSIGNED_INTEGER()                                    \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS, \
                                                         STL_FALSE,                               \
                                                         STL_FALSE,                               \
                                                         v2,                                      \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_FAILED_LOGIN_ATTEMPTS____UNLIMITED()                                           \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS, \
                                                         STL_FALSE,                               \
                                                         STL_TRUE,                                \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_FAILED_LOGIN_ATTEMPTS____DEFAULT()                                             \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS, \
                                                         STL_TRUE,                                \
                                                         STL_FALSE,                               \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }


/**
 * password_life_time
 */

#define STL_PARSER_PASSWORD_LIFE_TIME____INTERVAL_VALUE()                                         \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME,    \
                                                         STL_FALSE,                               \
                                                         STL_FALSE,                               \
                                                         v2,                                      \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_LIFE_TIME____UNLIMITED()                                              \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME,    \
                                                         STL_FALSE,                               \
                                                         STL_TRUE,                                \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_LIFE_TIME____DEFAULT()                                                \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME,    \
                                                         STL_TRUE,                                \
                                                         STL_FALSE,                               \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

/**
 * password_lock_time
 */

#define STL_PARSER_PASSWORD_LOCK_TIME____INTERVAL_VALUE()                                         \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME,    \
                                                         STL_FALSE,                               \
                                                         STL_FALSE,                               \
                                                         v2,                                      \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_LOCK_TIME____UNLIMITED()                                              \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME,    \
                                                         STL_FALSE,                               \
                                                         STL_TRUE,                                \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_LOCK_TIME____DEFAULT()                                                \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME,    \
                                                         STL_TRUE,                                \
                                                         STL_FALSE,                               \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }


/**
 * password_grace_time
 */

#define STL_PARSER_PASSWORD_GRACE_TIME____INTERVAL_VALUE()                                        \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME,   \
                                                         STL_FALSE,                               \
                                                         STL_FALSE,                               \
                                                         v2,                                      \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_GRACE_TIME____UNLIMITED()                                             \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME,   \
                                                         STL_FALSE,                               \
                                                         STL_TRUE,                                \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_GRACE_TIME____DEFAULT()                                               \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME,   \
                                                         STL_TRUE,                                \
                                                         STL_FALSE,                               \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

/**
 * password_reuse_max
 */

#define STL_PARSER_PASSWORD_REUSE_MAX____UNSIGNED_INTEGER()                                       \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX,    \
                                                         STL_FALSE,                               \
                                                         STL_FALSE,                               \
                                                         v2,                                      \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_REUSE_MAX____UNLIMITED()                                              \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX,    \
                                                         STL_FALSE,                               \
                                                         STL_TRUE,                                \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_REUSE_MAX____DEFAULT()                                                \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX,    \
                                                         STL_TRUE,                                \
                                                         STL_FALSE,                               \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

/**
 * password_reuse_time
 */

#define STL_PARSER_PASSWORD_REUSE_TIME____INTERVAL_VALUE()                                        \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME,   \
                                                         STL_FALSE,                               \
                                                         STL_FALSE,                               \
                                                         v2,                                      \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_REUSE_TIME____UNLIMITED()                                             \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME,   \
                                                         STL_FALSE,                               \
                                                         STL_TRUE,                                \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_REUSE_TIME____DEFAULT()                                               \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME,   \
                                                         STL_TRUE,                                \
                                                         STL_FALSE,                               \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

/**
 * password_verify_function
 */

#define STL_PARSER_PASSWORD_VERIFY_FUNCTION____VERIFY_POLICY()                                    \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION,\
                                                         STL_FALSE,                               \
                                                         STL_FALSE,                               \
                                                         v2,                                      \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_VERIFY_FUNCTION____NULL()                                             \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION,\
                                                         STL_FALSE,                               \
                                                         STL_FALSE,                               \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }

#define STL_PARSER_PASSWORD_VERIFY_FUNCTION____DEFAULT()                                          \
    {                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakePasswordParameter( param,                                   \
                                                         a1,                                      \
                                                         ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION,\
                                                         STL_TRUE,                                \
                                                         STL_FALSE,                               \
                                                         NULL,                                    \
                                                         QLP_GET_LAST_POS( a2 ) - a2 ) );         \
    }


/**
 * password_parameter_number_interval
 */

#define STL_PARSER_PASSWORD_PARAMTER_NUMBER_INTERVAL____NUMBER( )   \
    {                                                               \
        STL_PARSER_CHECK( v0 = v1 );                                \
    }

#define STL_PARSER_PASSWORD_PARAMTER_NUMBER_INTERVAL____DIVIDE( )               \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_DIV,           \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a3 ) - a1,    \
                                                sList ) );                      \
    }

/**
 * drop_profile_statement
 */

#define STL_PARSER_DROP_PROFILE_STATEMENT____()                                             \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeDropProfile( param,                                   \
                                                   a1,                                      \
                                                   (v3 == NULL) ? STL_FALSE : STL_TRUE,     \
                                                   v4,                                      \
                                                   STL_FALSE) );                            \
    }

#define STL_PARSER_DROP_PROFILE_STATEMENT____CASCADE()                                      \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeDropProfile( param,                                   \
                                                   a1,                                      \
                                                   (v3 == NULL) ? STL_FALSE : STL_TRUE,     \
                                                   v4,                                      \
                                                   STL_TRUE ) );                            \
    }
    
/**
 * alter_profile_statement
 */

        
#define STL_PARSER_ALTER_PROFILE_STATEMENT____USER_PROFILE()    \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterProfile( param,      \
                                                    a1,         \
                                                    v3,         \
                                                    v5 ) );     \
    }

#define STL_PARSER_ALTER_PROFILE_STATEMENT____DEFAULT_PROFILE()                                 \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterProfile( param,                                      \
                                                    a1,                                         \
                                                    qlpMakeStrConstant( param, a3, "DEFAULT" ), \
                                                    v5 ) );                                     \
    }            

        

/**
 * grant_privilege_statement
 */

#define STL_PARSER_GRANT_PRIVILEGE____WITHOUT_GRANT()           \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeGrantPriv( param,         \
                                                 a1,            \
                                                 v2,            \
                                                 v4,            \
                                                 STL_FALSE ) ); \
    }

#define STL_PARSER_GRANT_PRIVILEGE____WITH_GRANT()              \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeGrantPriv( param,         \
                                                 a1,            \
                                                 v2,            \
                                                 v4,            \
                                                 STL_TRUE ) );  \
    }

#define STL_PARSER_GRANT_PRIVILEGE____WITHOUT_SCHEMA_PRIVILEGE_WITHOUT_GRANT()  \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( v0 = sList );                                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v4 ) );           \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeGrantPriv( param,                         \
                                                 a1,                            \
                                                 v2,                            \
                                                 sList,                         \
                                                 STL_FALSE ) );                 \
    }

#define STL_PARSER_GRANT_PRIVILEGE____WITHOUT_SCHEMA_PRIVILEGE_WITH_GRANT() \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
                                                                            \
        STL_PARSER_CHECK( v0 = sList );                                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v4 ) );       \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeGrantPriv( param,                     \
                                                 a1,                        \
                                                 v2,                        \
                                                 sList,                     \
                                                 STL_TRUE ) );              \
    }


/*
 * REVOKE PRIVILEGE
 */

#define STL_PARSER_REVOKE_PRIVILEGE____WITHOUT_GRANT()          \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeRevokePriv( param,        \
                                                  a1,           \
                                                  v2,           \
                                                  v4,           \
                                                  STL_FALSE,    \
                                                  v5 ) );       \
    }

#define STL_PARSER_REVOKE_PRIVILEGE____WITH_GRANT()         \
    {                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeRevokePriv( param,    \
                                                  a1,       \
                                                  v5,       \
                                                  v7,       \
                                                  STL_TRUE, \
                                                  v8 ) );   \
    }

#define STL_PARSER_REVOKE_PRIVILEGE____WITHOUT_SCHEMA_PRIVILEGE_WITHOUT_GRANT() \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( v0 = sList );                                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v4 ) );           \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeRevokePriv( param,                        \
                                                  a1,                           \
                                                  v2,                           \
                                                  sList,                        \
                                                  STL_FALSE,                    \
                                                  v5 ) );                       \
    }

#define STL_PARSER_REVOKE_PRIVILEGE____WITHOUT_SCHEMA_PRIVILEGE_WITH_GRANT()    \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( v0 = sList );                                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v7 ) );           \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeRevokePriv( param,                        \
                                                  a1,                           \
                                                  v5,                           \
                                                  sList,                        \
                                                  STL_TRUE,                     \
                                                  v8 ) );                       \
    }

#define STL_PARSER_REVOKE_BEHAVIOR____EMPTY()                                   \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                  \
                                                        a1,                     \
                                                        QLP_REVOKE_CASCADE ) ); \
    }

#define STL_PARSER_REVOKE_BEHAVIOR____RESTRICT()                                    \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                      \
                                                        a1,                         \
                                                        QLP_REVOKE_RESTRICT ) );    \
    }

#define STL_PARSER_REVOKE_BEHAVIOR____CASCADE()                                 \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                  \
                                                        a1,                     \
                                                        QLP_REVOKE_CASCADE ) ); \
    }

#define STL_PARSER_REVOKE_BEHAVIOR____CASCADE_CONSTRAINTS()                                 \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                              \
                                                        a1,                                 \
                                                        QLP_REVOKE_CASCADE_CONSTRAINTS ) ); \
    }

#define STL_PARSER_GRANTEE_LIST____GRANTEE()                            \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_GRANTEE_LIST____GRANTEE_LIST()                       \
    {                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_GRANTEE_NAME()               \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }




#define STL_PARSER_PRIVILEGE____DATABASE()      \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }
#define STL_PARSER_PRIVILEGE____TABLESPACE()    \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }
#define STL_PARSER_PRIVILEGE____SCHEMA()        \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }
#define STL_PARSER_PRIVILEGE____TABLE()         \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }
#define STL_PARSER_PRIVILEGE____SEQUENCE()      \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }



#define STL_PARSER_PRIVILEGE_ACTION_LIST____ACTION()    \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }
#define STL_PARSER_PRIVILEGE_ACTION_LIST____ACTION_LIST()   \
    {                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * database_privilege
 */

#define STL_PARSER_DATABASE_PRIVILEGE____ACTION_LIST()                      \
    {                                                                       \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,                \
                                                      a1,                   \
                                                      ELL_PRIV_DATABASE,    \
                                                      NULL,                 \
                                                      NULL );               \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                     \
                                                 a1,                        \
                                                 sPrivObj,                  \
                                                 v1 ) );                    \
    }

#define STL_PARSER_DATABASE_PRIVILEGE____ALL_PRIVILEGES()                   \
    {                                                                       \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,                \
                                                      a1,                   \
                                                      ELL_PRIV_DATABASE,    \
                                                      NULL,                 \
                                                      NULL );               \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                     \
                                                 a1,                        \
                                                 sPrivObj,                  \
                                                 NULL ) );                  \
    }




#define STL_PARSER_DATABASE_ACTION____ADMINISTRATION()                                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_ADMINISTRATION,    \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____ALTER_DATABASE()                                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_ALTER_DATABASE,    \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____ALTER_SYSTEM()                                \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_DATABASE,                \
                                                  ELL_DB_PRIV_ACTION_ALTER_SYSTEM,  \
                                                  NULL ) );                         \
    }
#define STL_PARSER_DATABASE_ACTION____ACCESS_CONTROL()                                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_ACCESS_CONTROL,    \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_SESSION()                                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_CREATE_SESSION,    \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_USER()                                 \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_DATABASE,                \
                                                  ELL_DB_PRIV_ACTION_CREATE_USER,   \
                                                  NULL ) );                         \
    }
#define STL_PARSER_DATABASE_ACTION____ALTER_USER()                                  \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_DATABASE,                \
                                                  ELL_DB_PRIV_ACTION_ALTER_USER,    \
                                                  NULL ) );                         \
    }
#define STL_PARSER_DATABASE_ACTION____DROP_USER()                               \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                        \
                                                  a1,                           \
                                                  ELL_PRIV_DATABASE,            \
                                                  ELL_DB_PRIV_ACTION_DROP_USER, \
                                                  NULL ) );                     \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_PROFILE()                               \
    {                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                             \
                                                  a1,                                \
                                                  ELL_PRIV_DATABASE,                 \
                                                  ELL_DB_PRIV_ACTION_CREATE_PROFILE, \
                                                  NULL ) );                          \
    }
#define STL_PARSER_DATABASE_ACTION____ALTER_PROFILE()                               \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_DATABASE,                \
                                                  ELL_DB_PRIV_ACTION_ALTER_PROFILE, \
                                                  NULL ) );                         \
    }
#define STL_PARSER_DATABASE_ACTION____DROP_PROFILE()                               \
    {                                                                              \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                           \
                                                  a1,                              \
                                                  ELL_PRIV_DATABASE,               \
                                                  ELL_DB_PRIV_ACTION_DROP_PROFILE, \
                                                  NULL ) );                        \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_ROLE()                                 \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_DATABASE,                \
                                                  ELL_DB_PRIV_ACTION_CREATE_ROLE,   \
                                                  NULL ) );                         \
    }
#define STL_PARSER_DATABASE_ACTION____ALTER_ROLE()                                  \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_DATABASE,                \
                                                  ELL_DB_PRIV_ACTION_ALTER_ROLE,    \
                                                  NULL ) );                         \
    }
#define STL_PARSER_DATABASE_ACTION____DROP_ROLE()                               \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                        \
                                                  a1,                           \
                                                  ELL_PRIV_DATABASE,            \
                                                  ELL_DB_PRIV_ACTION_DROP_ROLE, \
                                                  NULL ) );                     \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_TABLESPACE()                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_CREATE_TABLESPACE, \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____ALTER_TABLESPACE()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_ALTER_TABLESPACE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____DROP_TABLESPACE()                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_DROP_TABLESPACE,   \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____USAGE_TABLESPACE()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_USAGE_TABLESPACE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_SCHEMA()                               \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_DATABASE,                \
                                                  ELL_DB_PRIV_ACTION_CREATE_SCHEMA, \
                                                  NULL ) );                         \
    }
#define STL_PARSER_DATABASE_ACTION____ALTER_SCHEMA()                                \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_DATABASE,                \
                                                  ELL_DB_PRIV_ACTION_ALTER_SCHEMA,  \
                                                  NULL ) );                         \
    }
#define STL_PARSER_DATABASE_ACTION____DROP_SCHEMA()                                 \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_DATABASE,                \
                                                  ELL_DB_PRIV_ACTION_DROP_SCHEMA,   \
                                                  NULL ) );                         \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_ANY_TABLE()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_CREATE_ANY_TABLE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____ALTER_ANY_TABLE()                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_ALTER_ANY_TABLE,   \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____DROP_ANY_TABLE()                                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_DROP_ANY_TABLE,    \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____SELECT_ANY_TABLE()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_SELECT_ANY_TABLE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____INSERT_ANY_TABLE()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_INSERT_ANY_TABLE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____DELETE_ANY_TABLE()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_DELETE_ANY_TABLE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____UPDATE_ANY_TABLE()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_UPDATE_ANY_TABLE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____LOCK_ANY_TABLE()                                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_LOCK_ANY_TABLE,    \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_ANY_VIEW()                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_CREATE_ANY_VIEW,   \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____DROP_ANY_VIEW()                               \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_DATABASE,                \
                                                  ELL_DB_PRIV_ACTION_DROP_ANY_VIEW, \
                                                  NULL ) );                         \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_ANY_SEQUENCE()                                 \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_DATABASE,                        \
                                                  ELL_DB_PRIV_ACTION_CREATE_ANY_SEQUENCE,   \
                                                  NULL ) );                                 \
    }
#define STL_PARSER_DATABASE_ACTION____DROP_ANY_SEQUENCE()                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_DROP_ANY_SEQUENCE, \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____ALTER_ANY_SEQUENCE()                                  \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_DATABASE,                        \
                                                  ELL_DB_PRIV_ACTION_ALTER_ANY_SEQUENCE,    \
                                                  NULL ) );                                 \
    }
#define STL_PARSER_DATABASE_ACTION____USAGE_ANY_SEQUENCE()                                  \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_DATABASE,                        \
                                                  ELL_DB_PRIV_ACTION_USAGE_ANY_SEQUENCE,    \
                                                  NULL ) );                                 \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_ANY_INDEX()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_CREATE_ANY_INDEX,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____ALTER_ANY_INDEX()                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_ALTER_ANY_INDEX,   \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____DROP_ANY_INDEX()                                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_DROP_ANY_INDEX,    \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_ANY_SYNONYM()                                  \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_DATABASE,                        \
                                                  ELL_DB_PRIV_ACTION_CREATE_ANY_SYNONYM,    \
                                                  NULL ) );                                 \
    }
#define STL_PARSER_DATABASE_ACTION____DROP_ANY_SYNONYM()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_DATABASE,                    \
                                                  ELL_DB_PRIV_ACTION_DROP_ANY_SYNONYM,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_DATABASE_ACTION____CREATE_PUBLIC_SYNONYM()                               \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_DATABASE,                        \
                                                  ELL_DB_PRIV_ACTION_CREATE_PUBLIC_SYNONYM, \
                                                  NULL ) );                                 \
    }
#define STL_PARSER_DATABASE_ACTION____DROP_PUBLIC_SYNONYM()                               \
    {                                                                                     \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                  \
                                                  a1,                                     \
                                                  ELL_PRIV_DATABASE,                      \
                                                  ELL_DB_PRIV_ACTION_DROP_PUBLIC_SYNONYM, \
                                                  NULL ) );                               \
    }

/**
 * tablespace_privilege
 */

#define STL_PARSER_TABLESPACE_PRIVILEGE____ACTION_LIST()                \
    {                                                                   \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_SPACE,   \
                                                      v4,               \
                                                      NULL );           \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 v1 ) );                \
    }

#define STL_PARSER_TABLESPACE_PRIVILEGE____ALL()                        \
    {                                                                   \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_SPACE,   \
                                                      v4,               \
                                                      NULL );           \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 NULL ) );              \
    }

#define STL_PARSER_TABLESPACE_PRIVILEGE____ALL_PRIVILEGES()             \
    {                                                                   \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_SPACE,   \
                                                      v5,               \
                                                      NULL );           \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 NULL ) );              \
    }

#define STL_PARSER_TABLESPACE_ACTION____CREATE_OBJECT()                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SPACE,                       \
                                                  ELL_SPACE_PRIV_ACTION_CREATE_OBJECT,  \
                                                  NULL ) );                             \
    }

/**
 * schema_privilege
 */

#define STL_PARSER_SCHEMA_PRIVILEGE____WITH_SCHEMA()                    \
    {                                                                   \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_SCHEMA,  \
                                                      v4,               \
                                                      NULL );           \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 v1 ) );                \
    }

#define STL_PARSER_SCHEMA_PRIVILEGE____WITHOUT_SCHEMA()                 \
    {                                                                   \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_SCHEMA,  \
                                                      NULL,             \
                                                      NULL );           \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 v1 ) );                \
    }

#define STL_PARSER_SCHEMA_PRIVILEGE____ALL()                            \
    {                                                                   \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_SCHEMA,  \
                                                      v4,               \
                                                      NULL );           \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 NULL ) );              \
    }

#define STL_PARSER_SCHEMA_PRIVILEGE____ALL_PRIVILEGES()                 \
    {                                                                   \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_SCHEMA,  \
                                                      v5,               \
                                                      NULL );           \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 NULL ) );              \
    }

#define STL_PARSER_SCHEMA_ACTION____CONTROL_SCHEMA()                                        \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_SCHEMA,                          \
                                                  ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA,    \
                                                  NULL ) );                                 \
    }
#define STL_PARSER_SCHEMA_ACTION____CREATE_TABLE()                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_CREATE_TABLE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____ALTER_TABLE()                                       \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_ALTER_TABLE,   \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____DROP_TABLE()                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_DROP_TABLE,    \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____SELECT_TABLE()                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_SELECT_TABLE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____INSERT_TABLE()                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_INSERT_TABLE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____DELETE_TABLE()                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_DELETE_TABLE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____UPDATE_TABLE()                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_UPDATE_TABLE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____LOCK_TABLE()                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_LOCK_TABLE,    \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____CREATE_VIEW()                                       \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_CREATE_VIEW,   \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____DROP_VIEW()                                     \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_SCHEMA,                  \
                                                  ELL_SCHEMA_PRIV_ACTION_DROP_VIEW, \
                                                  NULL ) );                         \
    }
#define STL_PARSER_SCHEMA_ACTION____CREATE_SEQUENCE()                                       \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_SCHEMA,                          \
                                                  ELL_SCHEMA_PRIV_ACTION_CREATE_SEQUENCE,   \
                                                  NULL ) );                                 \
    }
#define STL_PARSER_SCHEMA_ACTION____ALTER_SEQUENCE()                                        \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_SCHEMA,                          \
                                                  ELL_SCHEMA_PRIV_ACTION_ALTER_SEQUENCE,    \
                                                  NULL ) );                                 \
    }
#define STL_PARSER_SCHEMA_ACTION____DROP_SEQUENCE()                                     \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_DROP_SEQUENCE, \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____USAGE_SEQUENCE()                                        \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_SCHEMA,                          \
                                                  ELL_SCHEMA_PRIV_ACTION_USAGE_SEQUENCE,    \
                                                  NULL ) );                                 \
    }
#define STL_PARSER_SCHEMA_ACTION____CREATE_INDEX()                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_CREATE_INDEX,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____ALTER_INDEX()                                       \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_ALTER_INDEX,   \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____DROP_INDEX()                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_SCHEMA,                      \
                                                  ELL_SCHEMA_PRIV_ACTION_DROP_INDEX,    \
                                                  NULL ) );                             \
    }
#define STL_PARSER_SCHEMA_ACTION____ADD_CONSTRAINT()                                        \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_SCHEMA,                          \
                                                  ELL_SCHEMA_PRIV_ACTION_ADD_CONSTRAINT,    \
                                                  NULL ) );                                 \
    }
#define STL_PARSER_SCHEMA_ACTION____CREATE_SYNONYM()                                        \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_SCHEMA,                          \
                                                  ELL_SCHEMA_PRIV_ACTION_CREATE_SYNONYM,    \
                                                  NULL ) );                                 \
    }
#define STL_PARSER_SCHEMA_ACTION____DROP_SYNONYM()                                          \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                    \
                                                  a1,                                       \
                                                  ELL_PRIV_SCHEMA,                          \
                                                  ELL_SCHEMA_PRIV_ACTION_DROP_SYNONYM,      \
                                                  NULL ) );                                 \
    }        
        
/**
 * table_privilege
 */

#define STL_PARSER_TABLE_PRIVILEGE____ON_TABLE()                        \
    {                                                                   \
        qlpObjectName * sObjName = qlpMakeObjectName( param, v4 );      \
                                                                        \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_TABLE,   \
                                                      NULL,             \
                                                      sObjName );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 v1 ) );                \
    }

#define STL_PARSER_TABLE_PRIVILEGE____ON()                              \
    {                                                                   \
        qlpObjectName * sObjName = qlpMakeObjectName( param, v3 );      \
                                                                        \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_TABLE,   \
                                                      NULL,             \
                                                      sObjName );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 v1 ) );                \
    }

#define STL_PARSER_TABLE_PRIVILEGE____ALL_ON_TABLE()                    \
    {                                                                   \
        qlpObjectName * sObjName = qlpMakeObjectName( param, v4 );      \
                                                                        \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_TABLE,   \
                                                      NULL,             \
                                                      sObjName );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 NULL ) );              \
    }

#define STL_PARSER_TABLE_PRIVILEGE____ALL_PRIVILEGES_ON_TABLE()         \
    {                                                                   \
        qlpObjectName * sObjName = qlpMakeObjectName( param, v5 );      \
                                                                        \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_TABLE,   \
                                                      NULL,             \
                                                      sObjName );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 NULL ) );              \
    }

#define STL_PARSER_TABLE_PRIVILEGE____ALL_ON()                          \
    {                                                                   \
        qlpObjectName * sObjName = qlpMakeObjectName( param, v3 );      \
                                                                        \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_TABLE,   \
                                                      NULL,             \
                                                      sObjName );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 NULL ) );              \
    }

#define STL_PARSER_TABLE_PRIVILEGE____ALL_PRIVILEGES_ON()               \
    {                                                                   \
        qlpObjectName * sObjName = qlpMakeObjectName( param, v4 );      \
                                                                        \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_TABLE,   \
                                                      NULL,             \
                                                      sObjName );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 NULL ) );              \
    }

#define STL_PARSER_TABLE_ACTION____CONTROL_TABLE()                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_TABLE,                       \
                                                  ELL_TABLE_PRIV_ACTION_CONTROL_TABLE,  \
                                                  NULL ) );                             \
    }
#define STL_PARSER_TABLE_ACTION____SELECT()                                     \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                        \
                                                  a1,                           \
                                                  ELL_PRIV_TABLE,               \
                                                  ELL_TABLE_PRIV_ACTION_SELECT, \
                                                  NULL ) );                     \
    }
#define STL_PARSER_TABLE_ACTION____INSERT()                                     \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                        \
                                                  a1,                           \
                                                  ELL_PRIV_TABLE,               \
                                                  ELL_TABLE_PRIV_ACTION_INSERT, \
                                                  NULL ) );                     \
    }
#define STL_PARSER_TABLE_ACTION____UPDATE()                                     \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                        \
                                                  a1,                           \
                                                  ELL_PRIV_TABLE,               \
                                                  ELL_TABLE_PRIV_ACTION_UPDATE, \
                                                  NULL ) );                     \
    }
#define STL_PARSER_TABLE_ACTION____DELETE()                                     \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                        \
                                                  a1,                           \
                                                  ELL_PRIV_TABLE,               \
                                                  ELL_TABLE_PRIV_ACTION_DELETE, \
                                                  NULL ) );                     \
    }
#define STL_PARSER_TABLE_ACTION____TRIGGER()                                        \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_TABLE,                   \
                                                  ELL_TABLE_PRIV_ACTION_TRIGGER,    \
                                                  NULL ) );                         \
    }
#define STL_PARSER_TABLE_ACTION____REFERENCES()                                     \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_TABLE,                   \
                                                  ELL_TABLE_PRIV_ACTION_REFERENCES, \
                                                  NULL ) );                         \
    }
#define STL_PARSER_TABLE_ACTION____LOCK()                                       \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                        \
                                                  a1,                           \
                                                  ELL_PRIV_TABLE,               \
                                                  ELL_TABLE_PRIV_ACTION_LOCK,   \
                                                  NULL ) );                     \
    }
#define STL_PARSER_TABLE_ACTION____INDEX()                                      \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                        \
                                                  a1,                           \
                                                  ELL_PRIV_TABLE,               \
                                                  ELL_TABLE_PRIV_ACTION_INDEX,  \
                                                  NULL ) );                     \
    }
#define STL_PARSER_TABLE_ACTION____ALTER()                                      \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                        \
                                                  a1,                           \
                                                  ELL_PRIV_TABLE,               \
                                                  ELL_TABLE_PRIV_ACTION_ALTER,  \
                                                  NULL ) );                     \
    }

/**
 * column_privilege
 */

#define STL_PARSER_COLUMN_ACTION____SELECT()                                        \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_COLUMN,                  \
                                                  ELL_COLUMN_PRIV_ACTION_SELECT,    \
                                                  v3 ) );                           \
    }
#define STL_PARSER_COLUMN_ACTION____INSERT()                                        \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_COLUMN,                  \
                                                  ELL_COLUMN_PRIV_ACTION_INSERT,    \
                                                  v3 ) );                           \
    }
#define STL_PARSER_COLUMN_ACTION____UPDATE()                                        \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_COLUMN,                  \
                                                  ELL_COLUMN_PRIV_ACTION_UPDATE,    \
                                                  v3 ) );                           \
    }
#define STL_PARSER_COLUMN_ACTION____REFERENCES()                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                                \
                                                  a1,                                   \
                                                  ELL_PRIV_COLUMN,                      \
                                                  ELL_COLUMN_PRIV_ACTION_REFERENCES,    \
                                                  v3 ) );                               \
    }

/**
 * sequnce_privilege
 */

#define STL_PARSER_SEQUENCE_PRIVILEGE____ACTION_LIST()                  \
    {                                                                   \
        qlpObjectName * sObjName = qlpMakeObjectName( param, v4 );      \
                                                                        \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_USAGE,   \
                                                      NULL,             \
                                                      sObjName );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 v1 ) );                \
    }

#define STL_PARSER_SEQUENCE_PRIVILEGE____ALL()                          \
    {                                                                   \
        qlpObjectName * sObjName = qlpMakeObjectName( param, v4 );      \
                                                                        \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_USAGE,   \
                                                      NULL,             \
                                                      sObjName );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 NULL ) );              \
    }

#define STL_PARSER_SEQUENCE_PRIVILEGE____ALL_PRIVILEGES()               \
    {                                                                   \
        qlpObjectName * sObjName = qlpMakeObjectName( param, v5 );      \
                                                                        \
        qlpPrivObject * sPrivObj = qlpMakePrivObject( param,            \
                                                      a1,               \
                                                      ELL_PRIV_USAGE,   \
                                                      NULL,             \
                                                      sObjName );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePrivilege( param,                 \
                                                 a1,                    \
                                                 sPrivObj,              \
                                                 NULL ) );              \
    }

#define STL_PARSER_USAGE_ACTION____SEQUENCE()      \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePrivAction( param,                            \
                                                  a1,                               \
                                                  ELL_PRIV_USAGE,                   \
                                                  ELL_USAGE_OBJECT_TYPE_SEQUENCE,   \
                                                  v3 ) );                           \
    }




/**
 * tablespace_definition : qlpTablespaceDef
 */

#define STL_PARSER_TABLESPACE_DEFINITION( )     \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_DATA_DATAFILE( ) \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            v0 = qlpMakeTablespaceDef( param,                               \
                                       a1,                                  \
                                       QLL_STMT_CREATE_TABLESPACE_TYPE,     \
                                       QLP_SPACE_MEDIA_TYPE_MEMORY,         \
                                       QLP_SPACE_USAGE_TYPE_DATA,           \
                                       v4,                                  \
                                       v6,                                  \
                                       NULL ) );                            \
    }

#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_DATA_DATAFILE_ATTR( )    \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeTablespaceDef( param,                                       \
                                       a1,                                          \
                                       QLL_STMT_CREATE_TABLESPACE_TYPE,             \
                                       QLP_SPACE_MEDIA_TYPE_MEMORY,                 \
                                       QLP_SPACE_USAGE_TYPE_DATA,                   \
                                       v4,                                          \
                                       v6,                                          \
                                       v7 ) );                                      \
    }

#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_MEMORY( )            \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeTablespaceDef( param,                           \
                                       a1,                              \
                                       QLL_STMT_CREATE_TABLESPACE_TYPE, \
                                       QLP_SPACE_MEDIA_TYPE_MEMORY,     \
                                       QLP_SPACE_USAGE_TYPE_TEMP,       \
                                       v4,                              \
                                       v6,                              \
                                       NULL ) );                        \
    }


#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_MEMORY_ATTR( )       \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeTablespaceDef( param,                           \
                                       a1,                              \
                                       QLL_STMT_CREATE_TABLESPACE_TYPE, \
                                       QLP_SPACE_MEDIA_TYPE_MEMORY,     \
                                       QLP_SPACE_USAGE_TYPE_TEMP,       \
                                       v4,                              \
                                       v6,                              \
                                       v7 ) );                          \
    }


#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_UNDO_DATAFILE( ) \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            v0 = qlpMakeTablespaceDef( param,                               \
                                       a1,                                  \
                                       QLL_STMT_CREATE_TABLESPACE_TYPE,     \
                                       QLP_SPACE_MEDIA_TYPE_MEMORY,         \
                                       QLP_SPACE_USAGE_TYPE_UNDO,           \
                                       v4,                                  \
                                       v6,                                  \
                                       NULL ) );                            \
    }

#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_UNDO_DATAFILE_ATTR( )    \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            v0 = qlpMakeTablespaceDef( param,                               \
                                       a1,                                  \
                                       QLL_STMT_CREATE_TABLESPACE_TYPE,     \
                                       QLP_SPACE_MEDIA_TYPE_MEMORY,         \
                                       QLP_SPACE_USAGE_TYPE_UNDO,           \
                                       v4,                                  \
                                       v6,                                  \
                                       v7 ) );                              \
    }

#define STL_PARSER_TABLESPACE_DEFINITION____DISK_DATA_DATAFILES( )      \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeTablespaceDef( param,                           \
                                       a1,                              \
                                       QLL_STMT_CREATE_TABLESPACE_TYPE, \
                                       QLP_SPACE_MEDIA_TYPE_DISK,       \
                                       QLP_SPACE_USAGE_TYPE_DATA,       \
                                       v4,                              \
                                       v6,                              \
                                       NULL ) );                        \
    }

#define STL_PARSER_TABLESPACE_DEFINITION____DISK_DATA_DATAFILES_ATTR( ) \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeTablespaceDef( param,                           \
                                       a1,                              \
                                       QLL_STMT_CREATE_TABLESPACE_TYPE, \
                                       QLP_SPACE_MEDIA_TYPE_DISK,       \
                                       QLP_SPACE_USAGE_TYPE_DATA,       \
                                       v4,                              \
                                       v6,                              \
                                       v7 ) );                          \
    }


#define STL_PARSER_TABLESPACE_DEFINITION____DISK_TEMPORARY_DATAFILES( ) \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeTablespaceDef( param,                           \
                                       a1,                              \
                                       QLL_STMT_CREATE_TABLESPACE_TYPE, \
                                       QLP_SPACE_MEDIA_TYPE_DISK,       \
                                       QLP_SPACE_USAGE_TYPE_TEMP,       \
                                       v4,                              \
                                       v6,                              \
                                       NULL ) );                        \
    }





/**
 * tablespace_clause : qlpTablespaceAttr
 */

#define STL_PARSER_TABLESPACE_CLAUSE____( )     \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * permanent_tablespace_options : qlpTablespaceAttr
 */

#define STL_PARSER_PERMANENT_TABLESPACE_OPTIONS____ATTR( )      \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeTablespaceAttr( param,    \
                                                      0 ) );    \
        STL_PARSER_CHECK( qlpSetTablespaceAttr( param,          \
                                                v0,             \
                                                v1 ) );         \
    }

#define STL_PARSER_PERMANENT_TABLESPACE_OPTIONS____ATTR_LIST( ) \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
        STL_PARSER_CHECK( qlpSetTablespaceAttr( param,          \
                                                v1,             \
                                                v2 ) );         \
    }


/**
 * permanent_tablespace_option : qlpList
 */

#define STL_PARSER_PERMANENT_TABLESPACE_OPTION____LOGGING( )                                        \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( v0 = sList );                                                             \
        STL_PARSER_CHECK(                                                                           \
            qlpAddPtrValueToList( param,                                                            \
                                  sList,                                                            \
                                  qlpMakeIntParamConstant( param,                                   \
                                                           a1,                                      \
                                                           QLP_TABLESPACE_ATTR_LOGGING ) ) );       \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                qlpMakeIntParamConstant( param, a1, STL_TRUE ) ) ); \
    }

#define STL_PARSER_PERMANENT_TABLESPACE_OPTION____NO_LOGGING( )                                     \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( v0 = sList );                                                             \
        STL_PARSER_CHECK(                                                                           \
            qlpAddPtrValueToList( param,                                                            \
                                  sList,                                                            \
                                  qlpMakeIntParamConstant( param,                                   \
                                                           a1,                                      \
                                                           QLP_TABLESPACE_ATTR_LOGGING ) ) );       \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                qlpMakeIntParamConstant( param, a1, STL_FALSE ) ) ); \
    }

#define STL_PARSER_PERMANENT_TABLESPACE_OPTION____ONLINE( )                                         \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( v0 = sList );                                                             \
        STL_PARSER_CHECK(                                                                           \
            qlpAddPtrValueToList( param,                                                            \
                                  sList,                                                            \
                                  qlpMakeIntParamConstant( param,                                   \
                                                           a1,                                      \
                                                           QLP_TABLESPACE_ATTR_ONLINE ) ) );        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                qlpMakeIntParamConstant( param, a1, STL_TRUE ) ) ); \
    }

#define STL_PARSER_PERMANENT_TABLESPACE_OPTION____OFFLINE( )                                        \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( v0 = sList );                                                             \
        STL_PARSER_CHECK(                                                                           \
            qlpAddPtrValueToList( param,                                                            \
                                  sList,                                                            \
                                  qlpMakeIntParamConstant( param,                                   \
                                                           a1,                                      \
                                                           QLP_TABLESPACE_ATTR_ONLINE ) ) );        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                qlpMakeIntParamConstant( param, a1, STL_FALSE ) ) ); \
    }

#define STL_PARSER_PERMANENT_TABLESPACE_OPTION____EXTSIZE( )                                        \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( v0 = sList );                                                             \
        STL_PARSER_CHECK(                                                                           \
            qlpAddPtrValueToList( param,                                                            \
                                  sList,                                                            \
                                  qlpMakeIntParamConstant( param,                                   \
                                                           a1,                                      \
                                                           QLP_TABLESPACE_ATTR_EXTSIZE ) ) );       \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v2 ) );                               \
    }


/**
 * temporary_tablespace_options : qlpTablespaceAttr
 */

#define STL_PARSER_TEMPORARY_TABLESPACE_OPTIONS____ATTR( )      \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeTablespaceAttr( param,    \
                                                      0 ) );    \
        STL_PARSER_CHECK( qlpSetTablespaceAttr( param,          \
                                                v0,             \
                                                v1 ) );         \
    }

#define STL_PARSER_TEMPORARY_TABLESPACE_OPTIONS____ATTR_LIST( ) \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
        STL_PARSER_CHECK( qlpSetTablespaceAttr( param,          \
                                                v1,             \
                                                v2 ) );         \
    }


/**
 * temporary_tablespace_option : qlpList
 */

#define STL_PARSER_TEMPORARY_TABLESPACE_OPTION____EXTSIZE( )                                        \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( v0 = sList );                                                             \
        STL_PARSER_CHECK(                                                                           \
            qlpAddPtrValueToList( param,                                                            \
                                  sList,                                                            \
                                  qlpMakeIntParamConstant( param,                                   \
                                                           a1,                                      \
                                                           QLP_TABLESPACE_ATTR_EXTSIZE ) ) );       \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v2 ) );                               \
    }


/**
 * size_clause : qlpSize
 */

#define STL_PARSER_SIZE_CLAUSE____( )                                                   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeSize( param,                                      \
                                            a1,                                         \
                                            STL_FALSE,                                  \
                                            v2,                                         \
                                            qlpMakeStrConstant( param, a1, v1 ) ) );    \
    }


/**
 * byte_unit : qlpValue : integer
 */

#define STL_PARSER_BYTE_UNIT____EMPTY( )                                                        \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_SIZE_UNIT_INVALID ) );   \
    }

#define STL_PARSER_BYTE_UNIT____BYTE( )                                                         \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_SIZE_UNIT_BYTE ) );      \
    }

#define STL_PARSER_BYTE_UNIT____KILO( )                                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_SIZE_UNIT_KILO ) );      \
    }

#define STL_PARSER_BYTE_UNIT____MEGA( )                                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_SIZE_UNIT_MEGA ) );      \
    }

#define STL_PARSER_BYTE_UNIT____GIGA( )                                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_SIZE_UNIT_GIGA ) );      \
    }

#define STL_PARSER_BYTE_UNIT____TERA( )                                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_SIZE_UNIT_TERA ) );      \
    }


/**
 * memory_file_specifications : qlpList
 */

#define STL_PARSER_MEMORY_FILE_SPECIFICATIONS____SPEC( )               \
    {                                                                  \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
                                                                       \
        STL_PARSER_CHECK( v0 = sList );                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                 \
                                                sList,                 \
                                                v1 ) );                \
    }

#define STL_PARSER_MEMORY_FILE_SPECIFICATIONS____SPEC_LIST( )          \
    {                                                                  \
        STL_PARSER_CHECK( v0 = v1 );                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                 \
                                                v1,                    \
                                                v3 ) );                \
    }


/**
 * memory_file_specification : qlpDatafile
 */

#define STL_PARSER_MEMORY_FILE_SPECIFICATION____NAME_SIZE( )                            \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDatafile( param,                                  \
                                                a1,                                     \
                                                qlpMakeStrConstant( param, a1, v1 ),    \
                                                v3,                                     \
                                                NULL,                                   \
                                                NULL ) );                               \
    }

#define STL_PARSER_MEMORY_FILE_SPECIFICATION____NAME_SIZE_REUSE( )                              \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDatafile( param,                                          \
                                                a1,                                             \
                                                qlpMakeStrConstant( param, a1, v1 ),            \
                                                v3,                                             \
                                                qlpMakeIntParamConstant( param, a4, STL_TRUE ), \
                                                NULL ) );                                       \
    }


#define STL_PARSER_MEMORY_FILE_SPECIFICATION____NAME_REUSE( )                                   \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDatafile( param,                                          \
                                                a1,                                             \
                                                qlpMakeStrConstant( param, a1, v1 ),            \
                                                NULL,                                           \
                                                qlpMakeIntParamConstant( param, a2, STL_TRUE ), \
                                                NULL ) );                                       \
    }


/**
 * disk_file_specifications : qlpList
 */

#define STL_PARSER_DISK_FILE_SPECIFICATIONS____SPEC( )                  \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
    }

#define STL_PARSER_DISK_FILE_SPECIFICATIONS____SPEC_LIST( )     \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,          \
                                                v1,             \
                                                v3 ) );         \
    }


/**
 * disk_file_specification : qlpDatafile
 */

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_SIZE( )                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDatafile( param,                                  \
                                                a1,                                     \
                                                qlpMakeStrConstant( param, a1, v1 ),    \
                                                v3,                                     \
                                                NULL,                                   \
                                                NULL ) );                               \
    }

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_SIZE_REUSE( )                                \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDatafile( param,                                          \
                                                a1,                                             \
                                                qlpMakeStrConstant( param, a1, v1 ),            \
                                                v3,                                             \
                                                qlpMakeIntParamConstant( param, a4, STL_TRUE ), \
                                                NULL ) );                                       \
    }

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_SIZE_REUSE_AUTOEXTEND( )                     \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDatafile( param,                                          \
                                                a1,                                             \
                                                qlpMakeStrConstant( param, a1, v1 ),            \
                                                v3,                                             \
                                                qlpMakeIntParamConstant( param, a4, STL_TRUE ), \
                                                v5 ) );                                         \
    }

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_SIZE_AUTOEXTEND( )                   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDatafile( param,                                  \
                                                a1,                                     \
                                                qlpMakeStrConstant( param, a1, v1 ),    \
                                                v3,                                     \
                                                NULL,                                   \
                                                v4 ) );                                 \
    }

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_REUSE( )                                     \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDatafile( param,                                          \
                                                a1,                                             \
                                                qlpMakeStrConstant( param, a1, v1 ),            \
                                                NULL,                                           \
                                                qlpMakeIntParamConstant( param, a2, STL_TRUE ), \
                                                NULL ) );                                       \
    }

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_REUSE_AUTOEXTEND( )                          \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDatafile( param,                                          \
                                                a1,                                             \
                                                qlpMakeStrConstant( param, a1, v1 ),            \
                                                NULL,                                           \
                                                qlpMakeIntParamConstant( param, a2, STL_TRUE ), \
                                                v3 ) );                                         \
    }



/**
 * autoextend_clause : qlpAutoExtend
 */

#define STL_PARSER_AUTOEXTEND_CLAUSE____ON( )                                           \
    {                                                                                   \
        STL_PARSER_CHECK( v0 =                                                          \
                          qlpMakeAutoExtend( param,                                     \
                                             a1,                                        \
                                             qlpMakeIntParamConstant( param, a2, STL_TRUE ),    \
                                             NULL,                                      \
                                             NULL ) );                                  \
    }

#define STL_PARSER_AUTOEXTEND_CLAUSE____OFF( )                                          \
    {                                                                                   \
        STL_PARSER_CHECK( v0 =                                                          \
                          qlpMakeAutoExtend( param,                                     \
                                             a1,                                        \
                                             qlpMakeIntParamConstant( param, a2, STL_FALSE ),   \
                                             NULL,                                      \
                                             NULL ) );                                  \
    }

#define STL_PARSER_AUTOEXTEND_CLAUSE____ON_NEXT( )                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 =                                                          \
                          qlpMakeAutoExtend( param,                                     \
                                             a1,                                        \
                                             qlpMakeIntParamConstant( param, a2, STL_TRUE ),    \
                                             v4,                                        \
                                             NULL ) );                                  \
    }

#define STL_PARSER_AUTOEXTEND_CLAUSE____ON_MAXSIZE( )                                   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 =                                                          \
                          qlpMakeAutoExtend( param,                                     \
                                             a1,                                        \
                                             qlpMakeIntParamConstant( param, a2, STL_TRUE ),    \
                                             NULL,                                      \
                                             v3 ) );                                    \
    }

#define STL_PARSER_AUTOEXTEND_CLAUSE____ON_NEXT_MAXSIZE( )                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 =                                                          \
                          qlpMakeAutoExtend( param,                                     \
                                             a1,                                        \
                                             qlpMakeIntParamConstant( param, a2, STL_TRUE ),    \
                                             v3,                                        \
                                             v4 ) );                                    \
    }


/**
 * maxsize_clause : qlpSize
 */

#define STL_PARSER_MAXSIZE_CLAUSE____UNLIMITED( )                       \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeSize( param,                      \
                                            a1,                         \
                                            STL_TRUE,                   \
                                            QLP_SIZE_UNIT_INVALID,      \
                                            NULL ) );                   \
    }

#define STL_PARSER_MAXSIZE_CLAUSE____SIZE( )   \
    {                                          \
        STL_PARSER_CHECK( v0 = v2 );           \
    }

/**
 * alter database statement
 */

#define STL_PARSER_ALTER_DATABASE_STATEMENT____( ) \
    {                                              \
        STL_PARSER_CHECK( v0 = v3 );               \
    }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____LOGFILE( )      \
    {                                                      \
        STL_PARSER_CHECK( v0 = v1 );                       \
    }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____BACKUP( )       \
    {                                                      \
        STL_PARSER_CHECK( v0 = v1 );                       \
    }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____ARCHIVELOG( )   \
    {                                                      \
        STL_PARSER_CHECK( v0 = v1 );                       \
    }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____REGISTER( )     \
    {                                                      \
        STL_PARSER_CHECK( v0 = v1 );                       \
    }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____RECOVER( )      \
    {                                                      \
        STL_PARSER_CHECK( v0 = v1 );                       \
    }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____DELETE_BACKUP_LIST( )  \
    {                                                             \
        STL_PARSER_CHECK( v0 = v1 );                              \
    }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____CLEAR_PASSWORD_HISTORY() \
    {                                                               \
        STL_PARSER_CHECK( v0 = v1 );                                \
    }

#define STL_PARSE_ALTER_DATABASE_DELETE_BACKUP_LIST_CLAUSE____( )   \
    {                                                               \
        STL_PARSER_CHECK(                                           \
            v0 = qlpMakeDeleteBackupList(                           \
                param,                                              \
                a1,                                                 \
                QLL_STMT_ALTER_DATABASE_DELETE_BACKUP_LIST_TYPE,    \
                v2,                                                 \
                (v5 == NULL) ? STL_FALSE : STL_TRUE ) );            \
    }

#define STL_PARSE_ALTER_DATABASE_CLEAR_PASSWORD_HISTORY_CLAUSE____()    \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeAlterDatabaseClearPassHistory( param,           \
                                                       a1 ) );          \
    }

#define STL_PARSE_DELETE_BACKUP_LIST_OPTION____OBSOLETE( )              \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a1, v1 ) );   \
    }

#define STL_PARSE_DELETE_BACKUP_LIST_OPTION____ALL( )                   \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a1, v1 ) );   \
    }

/**
 * including_backup_file
 */

#define STL_PARSE_INCLUDING_BACKUP_FILE_OPTION____EMPTY( ) \
    {                                                      \
        STL_PARSER_CHECK( v0 = NULL );                     \
    }

#define STL_PARSE_INCLUDING_BACKUP_FILE_OPTION____INCLUDING_BACKUP_FILE( )  \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,              \
                                                        a1,                 \
                                                        STL_TRUE ) );       \
    }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____RESTORE( )      \
    {                                                      \
        STL_PARSER_CHECK( v0 = v1 );                       \
    }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____ADD_GROUP( )  \
    {                                                            \
        STL_PARSER_CHECK(                                        \
            v0 = qlpMakeAlterDatabaseAddLogfileGroup(            \
                param,                                           \
                a1,                                              \
                QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_GROUP_TYPE,  \
                v3,                                              \
                v5,                                              \
                v8,                                              \
                NULL ) );                                        \
    }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____ADD_GROUP_REUSE( ) \
    {                                                            \
        STL_PARSER_CHECK(                                        \
            v0 = qlpMakeAlterDatabaseAddLogfileGroup(            \
                param,                                           \
                a1,                                              \
                QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_GROUP_TYPE,  \
                v3,                                              \
                v5,                                              \
                v8,                                              \
                qlpMakeIntParamConstant( param,                  \
                                         a9,                     \
                                         STL_TRUE ) ) );         \
    }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____ADD_MEMBER( ) \
    {                                                            \
        STL_PARSER_CHECK(                                        \
            v0 = qlpMakeAlterDatabaseAddLogfileMember(           \
                param,                                           \
                a1,                                              \
                QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_MEMBER_TYPE, \
                v4,                                              \
                v6 ) );                                          \
    }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____DROP_GROUP( ) \
    {                                                            \
        STL_PARSER_CHECK(                                        \
            v0 = qlpMakeAlterDatabaseDropLogfileGroup(           \
                param,                                           \
                a1,                                              \
                QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_GROUP_TYPE, \
                v3 ) );                                          \
    }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____DROP_MEMBER( ) \
    {                                                             \
        STL_PARSER_CHECK(                                         \
            v0 = qlpMakeAlterDatabaseDropLogfileMember(           \
                param,                                            \
                a1,                                               \
                QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_MEMBER_TYPE, \
                v4 ) );                                           \
    }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____RENAME( )  \
    {                                                         \
        STL_PARSER_CHECK(                                     \
            v0 = qlpMakeAlterDatabaseRenameLogfile(           \
                param,                                        \
                a1,                                           \
                QLL_STMT_ALTER_DATABASE_RENAME_LOGFILE_TYPE,  \
                v3,                                           \
                v5 ) );                                       \
    }

#define STL_PARSE_ALTER_DATABASE_ARCHIVELOG_CLAUSE____( )           \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterDatabaseArchivelogMode(  \
                              param,                                \
                              a1,                                   \
                              SML_ARCHIVELOG_MODE ) );              \
    }

#define STL_PARSE_ALTER_DATABASE_NOARCHIVELOG_CLAUSE____( )         \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterDatabaseArchivelogMode(  \
                              param,                                \
                              a1,                                   \
                              SML_NOARCHIVELOG_MODE ) );            \
    }

#define STL_PARSE_ALTER_DATABASE_BACKUP_CLAUSE____( )                    \
    {                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeAlterDatabaseBackup(               \
                              param,                                     \
                              a1,                                        \
                              QLL_STMT_ALTER_DATABASE_BACKUP_TYPE,       \
                              qlpMakeStrConstant(                        \
                                  param,                                 \
                                  a1,                                    \
                                  v1 ),                                  \
                              SML_BACKUP_TYPE_FULL,                      \
                              NULL ) );                                  \
    }

#define STL_PARSE_ALTER_DATABASE_BACKUP_INCREMENTAL_CLAUSE____( )                    \
    {                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeAlterDatabaseBackup(                           \
                              param,                                                 \
                              a1,                                                    \
                              QLL_STMT_ALTER_DATABASE_BACKUP_INCREMENTAL_TYPE,       \
                              NULL,                                                  \
                              SML_BACKUP_TYPE_INCREMENTAL,                           \
                              v3 ) );                                                \
    }

#define STL_PARSE_ALTER_DATABASE_BACKUP_CONTROLFILE_CLAUSE____( )   \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeBackupCtrlfile(               \
                              param,                                \
                              a1,                                   \
                              qlpMakeStrConstant(param,             \
                                                 a4,                \
                                                 v4) ) );           \
    }

#define STL_PARSE_INCREMENTAL_BACKUP_OPTION____NONE( )                \
    {                                                                 \
        STL_PARSER_CHECK( v0 = qlpMakeIncrementalBackupOption(        \
                              param,                                  \
                              qlpMakeStrConstant( param, a2, v2 ),    \
                              NULL ) );                               \
    }

#define STL_PARSE_INCREMENTAL_BACKUP_OPTION____( )                        \
    {                                                                     \
        STL_PARSER_CHECK( v0 = qlpMakeIncrementalBackupOption(            \
                              param,                                      \
                              qlpMakeStrConstant( param, a2, v2 ),        \
                              qlpMakeStrConstant( param, a3, v3 ) ) );    \
    }

#define STL_PARSE_ALTER_DATABASE_REGISTER_CLAUSE____IRRECOVERABLE( )        \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterDatabaseIrrecoverable(           \
                              param,                                        \
                              a1,                                           \
                              v4 ) );                                       \
    }

#define STL_PARSE_ALTER_DATABASE_RECOVER_CLAUSE____DATABASE( )              \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterDatabaseRecover(                 \
                              param,                                        \
                              a1,                                           \
                              v1 ) );                                       \
    }

#define STL_PARSE_ALTER_DATABASE_RECOVER_CLAUSE____TABLESPACE( )            \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterDatabaseRecoverTablespace(       \
                              param,                                        \
                              a1,                                           \
                              v3 ) );                                       \
    }

#define STL_PARSE_ALTER_DATABASE_RESTORE_CLAUSE____DATABASE( )                \
    {                                                                         \
        STL_PARSER_CHECK( v0 = qlpMakeAlterDatabaseRestore(                   \
                              param,                                          \
                              a1,                                             \
                              QLL_STMT_ALTER_DATABASE_RESTORE_DATABASE_TYPE,  \
                              v2 ) );                                         \
    }

#define STL_PARSE_ALTER_DATABASE_RESTORE_CLAUSE____TABLESPACE( )               \
    {                                                                          \
        STL_PARSER_CHECK( v0 = qlpMakeAlterDatabaseRestoreTablespace(          \
                              param,                                           \
                              a1,                                              \
                              QLL_STMT_ALTER_DATABASE_RESTORE_TABLESPACE_TYPE, \
                              v3 ) );                                          \
    }

#define STL_PARSE_MEDIA_RECOVERY_OPTION____FULL()       \
    {                                                   \
        STL_PARSER_CHECK( v0 = NULL );                  \
    }

#define STL_PARSE_MEDIA_RECOVERY_OPTION____INCOMPLETE_RECOVERY_INTERACTIVE()    \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeImrOption(                                \
                              param,                                            \
                              SML_IMR_OPTION_INTERACTIVE,                       \
                              SML_IMR_CONDITION_NONE,                           \
                              v1,                                               \
                              (v2 != NULL)? STL_TRUE : STL_FALSE ) );           \
    }

#define STL_PARSE_MEDIA_RECOVERY_OPTION____INCOMPLETE_RECOVERY_CHANGE() \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeImrOption(                        \
                              param,                                    \
                              SML_IMR_OPTION_BATCH,                     \
                              SML_IMR_CONDITION_CHANGE,                 \
                              qlpMakeStrConstant( param, a4, v4 ),      \
                              (v5 != NULL)? STL_TRUE : STL_FALSE ) );   \
    }

#define STL_PARSE_MEDIA_RECOVERY_OPTION____INCOMPLETE_RECOVERY_TIME()   \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeImrOption(                        \
                              param,                                    \
                              SML_IMR_OPTION_BATCH,                     \
                              SML_IMR_CONDITION_TIME,                   \
                              qlpMakeStrConstant( param, a4, v4 ),      \
                              (v5 != NULL)? STL_TRUE : STL_FALSE ) );   \
    }

#define STL_PARSE_INCOMPLETE_RECOVERY_OPTION____BEGIN()         \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeInteractiveImrCondition(  \
                              param,                            \
                              SML_IMR_CONDITION_BEGIN,          \
                              NULL ) );                         \
    }

#define STL_PARSE_INCOMPLETE_RECOVERY_OPTION____END()           \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeInteractiveImrCondition(  \
                              param,                            \
                              SML_IMR_CONDITION_END,            \
                              NULL ) );                         \
    }

#define STL_PARSE_INCOMPLETE_RECOVERY_OPTION____MANUAL()                \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeInteractiveImrCondition(          \
                              param,                                    \
                              SML_IMR_CONDITION_MANUAL,                 \
                              qlpMakeStrConstant( param, a2, v2 ) ) );  \
    }

#define STL_PARSE_INCOMPLETE_RECOVERY_OPTION____AUTO()                  \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeInteractiveImrCondition(          \
                              param,                                    \
                              SML_IMR_CONDITION_AUTO,                   \
                              NULL ) );                                 \
    }

#define STL_PARSE_INCOMPLETE_RECOVERY_OPTION____SUGGESTION()    \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeInteractiveImrCondition(  \
                              param,                            \
                              SML_IMR_CONDITION_SUGGESTION,     \
                              NULL ) );                         \
    }

#define STL_PARSE_USING_CONTROLFILE_OPTION____USING_RECENT_CONTROLFILE()          \
    {                                                                             \
        STL_PARSER_CHECK( v0 = NULL );                                            \
    }

#define STL_PARSE_USING_CONTROLFILE_OPTION____USING_BACKUP_CONTROLFILE()          \
    {                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, STL_TRUE ) );  \
    }

#define STL_PARSE_RESTORE_UNTIL_OPTION____NONE()        \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeUntilOption(      \
                              param,                    \
                              SML_IMR_CONDITION_NONE,   \
                              NULL ) );                 \
    }

#define STL_PARSE_RESTORE_UNTIL_OPTION____CHANGE()                      \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeUntilOption(                      \
                              param,                                    \
                              SML_IMR_CONDITION_CHANGE,                 \
                              qlpMakeStrConstant( param, a3, v3 ) ) );  \
    }

#define STL_PARSE_RESTORE_UNTIL_OPTION____TIME()                        \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeUntilOption(                      \
                              param,                                    \
                              SML_IMR_CONDITION_TIME,                   \
                              qlpMakeStrConstant( param, a3, v3 ) ) );  \
    }

#define STL_PARSE_ALTER_DATABASE_RESTORE_CONTROLFILE_CLAUSE____()       \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeRestoreCtrlfile(                  \
                              param,                                    \
                              a1,                                       \
                              qlpMakeStrConstant(param,                 \
                                                 a4,                    \
                                                 v4 ) ) );              \
    }

/**
 * log_file : qlpList
 */

#define STL_PARSER_LOG_FILE____STRING( )                                \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
    }

#define STL_PARSER_LOG_FILE____STRING_LIST( )                   \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,          \
                                                v1,             \
                                                v3 ) );         \
    }

#define STL_PARSER_LOG_FILE____NAME( )                                                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDatafile( param,                                  \
                                                a1,                                     \
                                                qlpMakeStrConstant( param, a1, v1 ),    \
                                                NULL,                                   \
                                                NULL,                                   \
                                                NULL ) );                               \
    }

#define STL_PARSER_LOG_FILE____NAME_REUSE( )                                   \
    {                                                                          \
        STL_PARSER_CHECK( v0 = qlpMakeDatafile(                                \
                              param,                                           \
                              a1,                                              \
                              qlpMakeStrConstant( param, a1, v1 ),             \
                              NULL,                                            \
                              qlpMakeIntParamConstant( param, a2, STL_TRUE ),  \
                              NULL ) );                                        \
    }

#define STL_PARSER_LOG_FILE_LIST____STRING( )                                           \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
                                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                qlpMakeStrConstant(param, a1, v1) ) );  \
    }

#define STL_PARSER_LOG_FILE_LIST____STRING_LIST( )                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                v1,                                     \
                                                qlpMakeStrConstant(param, a3, v3) ) );  \
    }

/**
 * log_group : qlpList
 */

#define STL_PARSER_LOG_GROUP____NAME_SIZE( )               \
    {                                                      \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param,  \
                                                   a2,     \
                                                   v2 ) ); \
    }

/**
 * segment_id_list : qlpList
 */

#define STL_PARSER_SEGMENT_ID_LIST____INTEGER( )                        \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
    }

#define STL_PARSER_SEGMENT_ID_LIST____INTEGER_LIST( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,  \
                                                v1,     \
                                                v3 ) ); \
    }

/**
 * datafile_recovery_clause
 */
#define STL_PARSE_ALTER_DATABASE_RECOVER_CLAUSE____DATAFILE()           \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterDatabaseRecoverDatafile(     \
                              param,                                    \
                              a3,                                       \
                              v3 ) );                                   \
    }

/**
 * datafile_recovery_objects
 */
#define STL_PARSE_RECOVER_DATAFILE____DATAFILE()                        \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
    }

#define STL_PARSE_RECOVER_DATAFILE____DATAFILE_LIST()           \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,          \
                                                v1,             \
                                                v3 ) );         \
    }

/**
 * datafile_recovery_object
 */
#define STL_PARSE_RECOVER_DATAFILE____TARGET()                          \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeRecoverDatafileTarget(            \
                              param,                                    \
                              qlpMakeStrConstant( param, a1, v1 ),      \
                              v2,                                       \
                              (v3 == NULL) ? STL_FALSE : STL_TRUE ) );  \
    }

/**
 * recovery_using_backup_option: qlpValue
 */

#define STL_PARSE_RECOVER_USING_OPTION____NONE( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = NULL );                  \
    }

#define STL_PARSE_RECOVER_USING_OPTION____( )                   \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param,       \
                                                   a1,          \
                                                   v3 ) );      \
    }

/**
 * recovery_corruption_option: stlBool
 */
#define STL_PARSE_RECOVER_CORRUPTION_OPTION____NONE()\
    {                                                \
        STL_PARSER_CHECK( v0 = NULL );               \
    }

#define STL_PARSE_RECOVER_CORRUPTION_OPTION____()                       \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

/**
 * alter tablespace statement
 */

#define STL_PARSE_ALTER_TABLESPACE( )           \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


#define ALTER_TABLESPACE_ADD____ADD_MEMORY( )                                   \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeAlterSpaceAddFile( param,                               \
                                           a1,                                  \
                                           QLL_STMT_ALTER_TABLESPACE_ADD_TYPE,  \
                                           v3,                                  \
                                           a5,                                  \
                                           QLP_SPACE_MEDIA_TYPE_MEMORY,         \
                                           QLP_SPACE_USAGE_TYPE_TEMP,           \
                                           v6 ) );                              \
    }

#define ALTER_TABLESPACE_ADD____ADD_DATAFILE( )                                 \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeAlterSpaceAddFile( param,                               \
                                           a1,                                  \
                                           QLL_STMT_ALTER_TABLESPACE_ADD_TYPE,  \
                                           v3,                                  \
                                           a5,                                  \
                                           QLP_SPACE_MEDIA_TYPE_NA,             \
                                           QLP_SPACE_USAGE_TYPE_DATA,           \
                                           v6 ) );                              \
    }

#define ALTER_TABLESPACE_ADD____ADD_TEMPFILE( )                                 \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeAlterSpaceAddFile( param,                               \
                                           a1,                                  \
                                           QLL_STMT_ALTER_TABLESPACE_ADD_TYPE,  \
                                           v3,                                  \
                                           a5,                                  \
                                           QLP_SPACE_MEDIA_TYPE_DISK,           \
                                           QLP_SPACE_USAGE_TYPE_TEMP,           \
                                           v6 ) );                              \
    }

#define ALTER_TABLESPACE_RENAME____( )                                              \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeAlterSpaceRename( param,                                    \
                                          a1,                                       \
                                          QLL_STMT_ALTER_TABLESPACE_RENAME_TYPE,    \
                                          v3,                                       \
                                          v6 ) );                                   \
    }

#define ALTER_TABLESPACE_BACKUP_CLAUSE____( )                             \
    {                                                                     \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTablespaceBackup(              \
                              param,                                      \
                              a1,                                         \
                              QLL_STMT_ALTER_TABLESPACE_BACKUP_TYPE,      \
                              v3,                                         \
                              qlpMakeStrConstant(                         \
                                  param,                                  \
                                  a4,                                     \
                                  v4 ),                                   \
                              SML_BACKUP_TYPE_FULL,                       \
                              NULL ) );                                   \
    }

#define ALTER_TABLESPACE_BACKUP_INCREMENTAL_CLAUSE____( )                             \
    {                                                                                 \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTablespaceBackup(                          \
                              param,                                                  \
                              a1,                                                     \
                              QLL_STMT_ALTER_TABLESPACE_BACKUP_INCREMENTAL_TYPE,      \
                              v3,                                                     \
                              NULL,                                                   \
                              SML_BACKUP_TYPE_INCREMENTAL,                            \
                              v6 ) );                                                 \
    }

#define ALTER_TABLESPACE_DROP____DROP_MEMORY( )                                     \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeAlterSpaceDropFile( param,                                  \
                                            a1,                                     \
                                            QLL_STMT_ALTER_TABLESPACE_DROP_TYPE,    \
                                            v3,                                     \
                                            a5,                                     \
                                            QLP_SPACE_MEDIA_TYPE_MEMORY,            \
                                            QLP_SPACE_USAGE_TYPE_TEMP,              \
                                            qlpMakeStrConstant(param, a6, v6) ) );  \
    }

#define ALTER_TABLESPACE_DROP____DROP_DATAFILE( )                                   \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeAlterSpaceDropFile( param,                                  \
                                            a1,                                     \
                                            QLL_STMT_ALTER_TABLESPACE_DROP_TYPE,    \
                                            v3,                                     \
                                            a5,                                     \
                                            QLP_SPACE_MEDIA_TYPE_NA,                \
                                            QLP_SPACE_USAGE_TYPE_DATA,              \
                                            qlpMakeStrConstant(param, a6, v6) ) );  \
    }

#define ALTER_TABLESPACE_DROP____DROP_TEMPFILE( )                                   \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeAlterSpaceDropFile( param,                                  \
                                            a1,                                     \
                                            QLL_STMT_ALTER_TABLESPACE_DROP_TYPE,    \
                                            v3,                                     \
                                            a5,                                     \
                                            QLP_SPACE_MEDIA_TYPE_DISK,              \
                                            QLP_SPACE_USAGE_TYPE_TEMP,              \
                                            qlpMakeStrConstant(param, a6, v6) ) );  \
    }

#define ALTER_TABLESPACE_ONOFF____ONLINE( )                                         \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeAlterSpaceOnline( param,                                    \
                                          a1,                                       \
                                          QLL_STMT_ALTER_TABLESPACE_ONLINE_TYPE,    \
                                          v3 ) );                                   \
    }

#define ALTER_TABLESPACE_ONOFF____OFFLINE_EMPTY( )                                  \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeAlterSpaceOffline( param,                                   \
                                           a1,                                      \
                                           QLL_STMT_ALTER_TABLESPACE_OFFLINE_TYPE,  \
                                           v3,                                      \
                                           NULL ) );                                \
    }

#define ALTER_TABLESPACE_ONOFF____OFFLINE_NORMAL( )                                                 \
    {                                                                                               \
        STL_PARSER_CHECK(                                                                           \
            v0 = qlpMakeAlterSpaceOffline( param,                                                   \
                                           a1,                                                      \
                                           QLL_STMT_ALTER_TABLESPACE_OFFLINE_TYPE,                  \
                                           v3,                                                      \
                                           qlpMakeIntParamConstant(param,                           \
                                                                   a5,                              \
                                                                   SML_OFFLINE_BEHAVIOR_NORMAL) ) ); \
    }

#define ALTER_TABLESPACE_ONOFF____OFFLINE_IMMEDIATE( )                                              \
    {                                                                                               \
        STL_PARSER_CHECK(                                                                           \
            v0 = qlpMakeAlterSpaceOffline( param,                                                   \
                                           a1,                                                      \
                                           QLL_STMT_ALTER_TABLESPACE_OFFLINE_TYPE,                  \
                                           v3,                                                      \
                                           qlpMakeIntParamConstant(param,                           \
                                                                   a5,                              \
                                                                   SML_OFFLINE_BEHAVIOR_IMMEDIATE) ) ); \
    }

#define ALTER_TABLESPACE_RENAME____RENAME_MEMORY( )                                         \
    {                                                                                       \
        STL_PARSER_CHECK(                                                                   \
            v0 = qlpMakeAlterSpaceRenameFile( param,                                        \
                                              a1,                                           \
                                              QLL_STMT_ALTER_TABLESPACE_RENAME_FILE_TYPE,   \
                                              v3,                                           \
                                              a5,                                           \
                                              QLP_SPACE_MEDIA_TYPE_MEMORY,                  \
                                              QLP_SPACE_USAGE_TYPE_TEMP,                    \
                                              v6,                                           \
                                              v8 ) );                                       \
    }

#define ALTER_TABLESPACE_RENAME____RENAME_DATAFILE( )                                       \
    {                                                                                       \
        STL_PARSER_CHECK(                                                                   \
            v0 = qlpMakeAlterSpaceRenameFile( param,                                        \
                                              a1,                                           \
                                              QLL_STMT_ALTER_TABLESPACE_RENAME_FILE_TYPE,   \
                                              v3,                                           \
                                              a5,                                           \
                                              QLP_SPACE_MEDIA_TYPE_NA,                      \
                                              QLP_SPACE_USAGE_TYPE_DATA,                    \
                                              v6,                                           \
                                              v8 ) );                                       \
    }

#define ALTER_TABLESPACE_RENAME____RENAME_TEMPFILE( )                                       \
    {                                                                                       \
        STL_PARSER_CHECK(                                                                   \
            v0 = qlpMakeAlterSpaceRenameFile( param,                                        \
                                              a1,                                           \
                                              QLL_STMT_ALTER_TABLESPACE_RENAME_FILE_TYPE,   \
                                              v3,                                           \
                                              a5,                                           \
                                              QLP_SPACE_MEDIA_TYPE_DISK,                    \
                                              QLP_SPACE_USAGE_TYPE_TEMP,                    \
                                              v6,                                           \
                                              v8 ) );                                       \
    }

/**
 * rename_file_list : qlpList
 */

#define STL_PARSER_RENAME_FILE_LIST____STRING( )                                        \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
                                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                qlpMakeStrConstant(param, a1, v1) ) );  \
    }

#define STL_PARSER_RENAME_FILE_LIST____STRING_LIST( )                                   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                v1,                                     \
                                                qlpMakeStrConstant(param, a3, v3) ) );  \
    }

/**
 * drop tablespace statement : qlpDropTablespace
 */

#define STL_PARSER_DROP_TABLESPACE_STATEMENT()                                  \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeDropTablespace( param,                                  \
                                        a1,                                     \
                                        (v3 == NULL) ? STL_FALSE : STL_TRUE,    \
                                        v4,                                     \
                                        v5,                                     \
                                        v6,                                     \
                                        v7 ) );                                 \
    }


/**
 * including_contents_option
 */

#define STL_PARSER_INCLUDING_CONTENTS_OPTION____EMPTY() \
    {                                                   \
        STL_PARSER_CHECK( v0 = NULL );                  \
    }

#define STL_PARSER_INCLUDING_CONTENTS_OPTION____INCLUDING_CONTENTS()    \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

/**
 * drop_datafile_option
 */

#define STL_PARSER_DROP_DATAFILE_OPTION____EMPTY()  \
    {                                               \
        STL_PARSER_CHECK( v0 = NULL );              \
    }

#define STL_PARSER_DROP_DATAFILE_OPTION____AND_DATAFILES()                          \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                      \
                                                        a1,                         \
                                                        QLP_DROP_DATAFILES_AND ) ); \
    }

#define STL_PARSER_DROP_DATAFILE_OPTION____KEEP_DATAFILES()                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        QLP_DROP_DATAFILES_KEEP ) );    \
    }



/*
 * schema_definition:
 */

#define STL_PARSER_SCHEMA_DEFINITION_WITHOUT_DDL____SCHEMA_ONLY()   \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeSchemaDef( param,             \
                                                 a1,                \
                                                 v3,                \
                                                 NULL,              \
                                                 NULL ) );          \
    }

#define STL_PARSER_SCHEMA_DEFINITION_WITHOUT_DDL____OWNER_ONLY()    \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeSchemaDef( param,             \
                                                 a1,                \
                                                 NULL,              \
                                                 v4,                \
                                                 NULL ) );          \
    }

#define STL_PARSER_SCHEMA_DEFINITION_WITHOUT_DDL____SCHEMA_AND_OWNER()  \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeSchemaDef( param,                 \
                                                 a1,                    \
                                                 v3,                    \
                                                 v5,                    \
                                                 NULL ) );              \
    }

#define STL_PARSER_SCHEMA_DEFINITION_WITH_DDL____SCHEMA_ONLY()  \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeSchemaDef( param,         \
                                                 a1,            \
                                                 v3,            \
                                                 NULL,          \
                                                 v4 ) );        \
    }

#define STL_PARSER_SCHEMA_DEFINITION_WITH_DDL____OWNER_ONLY()   \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeSchemaDef( param,         \
                                                 a1,            \
                                                 NULL,          \
                                                 v4,            \
                                                 v5 ) );        \
    }

#define STL_PARSER_SCHEMA_DEFINITION_WITH_DDL____SCHEMA_AND_OWNER() \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeSchemaDef( param,             \
                                                 a1,                \
                                                 v3,                \
                                                 v5,                \
                                                 v6 ) );            \
    }


/*
 * schema_element_list:
 */

#define STL_PARSER_SCHEMA_ELEMENT_LIST____DDL()                         \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_SCHEMA_ELEMENT_LIST____DDL_LIST()                \
    {                                                               \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v2 ) );  \
        STL_PARSER_CHECK( v0 = v1 );                                \
    }


/*
 * schema_element:  
 */

#define STL_PARSER_SCHEMA_ELEMENT____TABLE_DEFINITION() \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_SCHEMA_ELEMENT____VIEW_DEFINITION()  \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }
    
#define STL_PARSER_SCHEMA_ELEMENT____INDEX_DEFINITION() \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }
#define STL_PARSER_SCHEMA_ELEMENT____SEQUENCE_DEFINITION()  \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }
#define STL_PARSER_SCHEMA_ELEMENT____GRANT_PRIV_STATEMENT() \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }
#define STL_PARSER_SCHEMA_ELEMENT____COMMENT_STATEMENT()    \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }


/*
 * drop_schema_statement:
 */

#define STL_PARSER_DROP_SCHEMA_STATEMENT()                                                  \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeDropSchema( param,                                    \
                                                  a1,                                       \
                                                  (v3 == NULL) ? STL_FALSE : STL_TRUE,      \
                                                  v4,                                       \
                                                  (v5 == NULL) ? STL_FALSE : STL_TRUE ) );  \
    }


/**
 * table definition
 */

#define STL_PARSER_TABLE_DEFINITION____SCOPE_NAME_CONTENTS( )   \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeTableDef( param,          \
                                                a1,             \
                                                v4,             \
                                                a4,             \
                                                v5,             \
                                                NULL,           \
                                                NULL,           \
                                                NULL,           \
                                                STL_FALSE ) );  \
    }

#define STL_PARSER_TABLE_DEFINITION____SCOPE_NAME_CONTENTS_RELATION()   \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeTableDef( param,                  \
                                                a1,                     \
                                                v4,                     \
                                                a4,                     \
                                                v5,                     \
                                                v6,                     \
                                                NULL,                   \
                                                NULL,                   \
                                                STL_FALSE) );           \
    }

#define STL_PARSER_TABLE_DEFINITION____SCOPE_NAME_COLUMNLIST_SUBQUERY()                  \
    {                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeTableDef( param,                                   \
                                                a1,                                      \
                                                v4,                                      \
                                                a4,                                      \
                                                NULL,                                    \
                                                NULL,                                    \
                                                v5,                                      \
                                                v7,                                      \
                                                (v8 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

#define STL_PARSER_TABLE_DEFINITION____SCOPE_NAME_COLUMNLIST_RELATION_SUBQUERY()         \
    {                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeTableDef( param,                                   \
                                                a1,                                      \
                                                v4,                                      \
                                                a4,                                      \
                                                NULL,                                    \
                                                v6,                                      \
                                                v5,                                      \
                                                v8,                                      \
                                                (v9 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

/**
 * as_subquery_clause
 */

#define STL_PARSER_WITH_DATA_OPTION____NO_DATA() \
    {                                            \
        STL_PARSER_CHECK( v0 = NULL );           \
    }

#define STL_PARSER_WITH_DATA_OPTION____DATA()                           \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }


/**
 * relation_attribute_types
 */

#define STL_PARSER_RELATION_ATTRIBUTE_TYPES____LIST()   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_RELATION_ATTRIBUTE_TYPES____TABLESPACE() \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_RELATION_ATTRIBUTE_TYPES____LIST_TABLESPACE()                \
    {                                                                           \
        STL_PARSER_CHECK( qlpCheckDuplicateOptions(                             \
                              param,                                            \
                              v1,                                               \
                              (((qlpList *)v2)->mHead)->mData.mPtrValue ) );    \
        STL_PARSER_CHECK( qlpAddPtrValueToList(                                 \
                              param,                                            \
                              v1,                                               \
                              (((qlpList *)v2)->mHead)->mData.mPtrValue) );     \
        STL_PARSER_CHECK( v0 = v1 );                                            \
    }

#define STL_PARSER_RELATION_ATTRIBUTE_TYPES____COLUMNAR()               \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_RELATION_ATTRIBUTE_TYPES____COLUMNAR_TABLESPACE()            \
    {                                                                           \
        STL_PARSER_CHECK( qlpAddPtrValueToList(                                 \
                              param,                                            \
                              v1,                                               \
                              (((qlpList *)v2)->mHead)->mData.mPtrValue) );     \
        STL_PARSER_CHECK( v0 = v1 );                                            \
    }

/**
 * relation_attribute_list
 */

#define STL_PARSER_RELATION_ATTRIBUTE_LIST____ATTR()                    \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_RELATION_ATTRIBUTE_LIST____ATTR_LIST()               \
    {                                                                   \
        STL_PARSER_CHECK( qlpCheckDuplicateOptions( param, v1, v2 ) );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v2 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

/**
 * relation_columnar_option
 */

#define STL_PARSER_RELATION_ATTRIBUTE_COLUMNAR____WITH_COLUMNAR_OPTIONS()       \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        qlpDefElem * sDefElem = qlpMakeDefElem(                                 \
            param,                                                              \
            a1,                                                                 \
            QLP_OBJECT_ATTR_COLUMNAR_OPTION,                                    \
            (qllNode *)qlpMakeIntParamConstant(                                 \
                param,                                                          \
                a1,                                                             \
                QLP_TABLE_DEFINITION_TYPE_COLUMNAR ) );                         \
        STL_PARSER_CHECK( v0 = sList );                                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, sDefElem ) );     \
    }

#define STL_PARSER_RELATION_ATTRIBUTE_COLUMNAR____WITHOUT_COLUMNAR_OPTIONS()    \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        qlpDefElem * sDefElem = qlpMakeDefElem(                                 \
            param,                                                              \
            a1,                                                                 \
            QLP_OBJECT_ATTR_COLUMNAR_OPTION,                                    \
            (qllNode *)qlpMakeIntParamConstant(                                 \
                param,                                                          \
                a1,                                                             \
                QLP_TABLE_DEFINITION_TYPE_ROW ) );                              \
        STL_PARSER_CHECK( v0 = sList );                                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, sDefElem ) );     \
    }

#define STL_PARSER_RELATION_ATTRIBUTE_COLUMNAR____WITH_COLUMNAR_OPTIONS_SEGMENT()       \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
        qlpDefElem * sDefElem = qlpMakeDefElem(                                         \
            param,                                                                      \
            a1,                                                                         \
            QLP_OBJECT_ATTR_COLUMNAR_OPTION,                                            \
            (qllNode *)qlpMakeIntParamConstant(                                         \
                param,                                                                  \
                a1,                                                                     \
                QLP_TABLE_DEFINITION_TYPE_COLUMNAR ) );                                 \
        STL_PARSER_CHECK( v0 = sList );                                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, sDefElem ) );             \
        qlpDefElem * sDefSeg = qlpMakeDefElem( param,                                   \
                                               a4,                                      \
                                               QLP_OBJECT_ATTR_SEGMENT_CLAUSE,          \
                                               v4 );                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, sDefSeg ) );              \
    }

#define STL_PARSER_RELATION_ATTRIBUTE_COLUMNAR____WITHOUT_COLUMNAR_OPTIONS_SEGMENT()    \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
        qlpDefElem * sDefElem = qlpMakeDefElem(                                         \
            param,                                                                      \
            a1,                                                                         \
            QLP_OBJECT_ATTR_COLUMNAR_OPTION,                                            \
            (qllNode *)qlpMakeIntParamConstant(                                         \
                param,                                                                  \
                a1,                                                                     \
                QLP_TABLE_DEFINITION_TYPE_ROW ) );                                      \
        STL_PARSER_CHECK( v0 = sList );                                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, sDefElem ) );             \
        qlpDefElem * sDefSeg = qlpMakeDefElem( param,                                   \
                                               a4,                                      \
                                               QLP_OBJECT_ATTR_SEGMENT_CLAUSE,          \
                                               v4 );                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, sDefSeg ) );              \
    }

/**
 * relation_attribute
 */

#define STL_PARSER_RELATION_ATTRIBUTE_TYPE____SEGMENT()                         \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_OBJECT_ATTR_SEGMENT_CLAUSE,  \
                                               v1 ) );                          \
    }

#define STL_PARSER_RELATION_ATTRIBUTE_TYPE____PHYSICAL()    \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }


/**
 * tablespace_attribute_types
 */

#define STL_PARSER_TABLESPACE_ATTRIBUTE_TYPES____()                         \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        qlpDefElem * sDefElem = qlpMakeDefElem( param,                      \
                                                a1,                         \
                                                QLP_OBJECT_ATTR_TABLESPACE, \
                                                v2 );                       \
                                                                            \
        STL_PARSER_CHECK( v0 = sList );                                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, sDefElem ) ); \
    }

/**
 * segment_attribute_types
 */

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPES____LIST()    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v3 );                    \
    }


/**
 * segment_attribute_type_list
 */

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE_LIST____ATTR()                \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE_LIST____ATTR_LIST()           \
    {                                                                   \
        STL_PARSER_CHECK( qlpCheckDuplicateOptions( param, v1, v2 ) );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v2 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * segment_attribute_type
 */

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE____INITIAL()                          \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_OBJECT_ATTR_SEGMENT_INITIAL, \
                                               (qllNode *) v2 ) );              \
    }

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE____NEXT()                             \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_OBJECT_ATTR_SEGMENT_NEXT,    \
                                               (qllNode *) v2 ) );              \
    }

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE____MINSIZE()                          \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_OBJECT_ATTR_SEGMENT_MINSIZE, \
                                               (qllNode *) v2 ) );              \
    }

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE____MAXSIZE()                          \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_OBJECT_ATTR_SEGMENT_MAXSIZE, \
                                               (qllNode *) v2 ) );              \
    }



/**
 * physical_attribute_type
 */

#define STL_PARSER_PHYSICAL_ATTRIBUTE_TYPE____PCTFREE( )                                \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            v0 = qlpMakeDefElem( param,                                                 \
                                 a1,                                                    \
                                 QLP_OBJECT_ATTR_PHYSICAL_PCTFREE,                      \
                                 (qllNode *) qlpMakeStrConstant(param, a2, v2 ) ) );    \
    }

#define STL_PARSER_PHYSICAL_ATTRIBUTE_TYPE____PCTUSED( )                                \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            v0 = qlpMakeDefElem( param,                                                 \
                                 a1,                                                    \
                                 QLP_OBJECT_ATTR_PHYSICAL_PCTUSED,                      \
                                 (qllNode *) qlpMakeStrConstant(param, a2, v2 ) ) );    \
    }

#define STL_PARSER_PHYSICAL_ATTRIBUTE_TYPE____INITRANS( )                               \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            v0 = qlpMakeDefElem( param,                                                 \
                                 a1,                                                    \
                                 QLP_OBJECT_ATTR_PHYSICAL_INITRANS,                     \
                                 (qllNode *) qlpMakeStrConstant(param, a2, v2 ) ) );    \
    }

#define STL_PARSER_PHYSICAL_ATTRIBUTE_TYPE____MAXTRANS( )                               \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            v0 = qlpMakeDefElem( param,                                                 \
                                 a1,                                                    \
                                 QLP_OBJECT_ATTR_PHYSICAL_MAXTRANS,                     \
                                 (qllNode *) qlpMakeStrConstant(param, a2, v2 ) ) );    \
    }

/**
 * table_scope
 */
#define STL_PARSER_TABLE_SCOPE____NON_TEMP( )                                       \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                      \
                                                        a0,                         \
                                                        QLP_TABLE_SCOPE_EMPTY ) );  \
    }

#define STL_PARSER_TABLE_SCOPE____GLOBAL_TEMP( )                                                \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                  \
                                                        a1,                                     \
                                                        QLP_TABLE_SCOPE_GLOBAL_TEMPORARY ) );   \
        STL_PARSER_NOT_SUPPORT( a1 );                                                           \
    }

#define STL_PARSER_TABLE_SCOPE____LOCAL_TEMP( )                                                 \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                  \
                                                        a1,                                     \
                                                        QLP_TABLE_SCOPE_LOCAL_TEMPORARY ) );    \
        STL_PARSER_NOT_SUPPORT( a1 );                                                           \
    }


/**
 * table_contents_source
 */

#define STL_PARSER_TABLE_CONTENTS_SOURCE____ELEMENTS( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }


/**
 * table_element_list
 */

#define STL_PARSER_TABLE_ELEMENT_LIST____( )    \
    {                                           \
        STL_PARSER_CHECK( v0 = v2 );            \
    }


/**
 * table_element_2
 */

#define STL_PARSER_TABLE_ELEMENT_2____ELEMENT( )                        \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );  \
    }

#define STL_PARSER_TABLE_ELEMENT_2____LIST( )                           \
    {                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );     \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * table_element
 */

#define STL_PARSER_TABLE_ELEMENT____COLUMN_DEF( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = (qllNode*) v1 );         \
    }

#define STL_PARSER_TABLE_ELEMENT____CONSTRAINT_DEF( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = (qllNode*) v1 );         \
    }


/**
 * table_constraint_definition : qlpList
 */

#define STL_PARSER_TABLE_CONSTRAINT_DEFINITION____CONSTRAINT( )                 \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( qlpCheckNodeType( param, v1, QLL_CONSTRAINT_TYPE ) ); \
        STL_PARSER_CHECK( v0 = sList );                                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );           \
    }

/**
 * table_constraint_element
 */

#define STL_PARSER_TABLE_CONSTRAINT_ELEMENT____DEF( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }                                                   

#define STL_PARSER_TABLE_CONSTRAINT_ELEMENT____NAME_DEF( )              \
    {                                                                   \
        ((qlpConstraint*)v2)->mName = qlpMakeObjectName( param, v1);    \
        STL_PARSER_CHECK( v0 = v2 );                                    \
    }                                                                   

#define STL_PARSER_TABLE_CONSTRAINT_ELEMENT____DEF_WITH_CONSTARINT_CHARACTERISTICS( )   \
    {                                                                                   \
        STL_PARSER_CHECK( qlpAddConstraintCharacteristic( param,                        \
                                                          (qlpConstraint *) v1,         \
                                                          (qlpConstraintAttr *) v2 ) ); \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }                                                   

#define STL_PARSER_TABLE_CONSTRAINT_ELEMENT____NAME_DEF_WITH_CONSTARINT_CHARACTERISTICS( )  \
    {                                                                                       \
        ((qlpConstraint*)v2)->mName = qlpMakeObjectName( param, v1);                        \
        STL_PARSER_CHECK( qlpAddConstraintCharacteristic( param,                            \
                                                          (qlpConstraint *) v2,             \
                                                          (qlpConstraintAttr *) v3 ) );     \
        STL_PARSER_CHECK( v0 = v2 );                                                        \
    }                                                                   


    
#define STL_PARSER_TABLE_CONSTRAINT_ELEMENT____CONSTRAINT_CHARACTERISTICS( )    \
    {                                                                           \
        STL_PARSER_CHECK( v0 = v1 );                                            \
    }


/**
 * constraint_name_definition
 */

#define STL_PARSER_CONSTRAINT_NAME_DEFINITION____( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v2 );                    \
    }


/**
 * constraint_name
 */

#define STL_PARSER_CONSTRAINT_NAME____( )       \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * constraint characteristics
 */

#define STL_PARSER_CONSTRAINT_CHARACTERISTIC____FIRST() \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }


#define STL_PARSER_CONSTRAINT_CHARACTERISTIC____NEXT()          \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpAppendConstraintAttr( param,  \
                                                        v1,     \
                                                        v2 ) ); \
    }

#define STL_PARSER_CONSTRAINT_CHARACTERISTICS____CHECKTIME( )   \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeConstraintAttr( param,    \
                                                      a1,       \
                                                      v1,       \
                                                      NULL,     \
                                                      NULL ) ); \
    }

#define STL_PARSER_CONSTRAINT_CHARACTERISTICS____DEFERRABLE( )                                      \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeConstraintAttr( param,                                        \
                                                      a1,                                           \
                                                      NULL,                                         \
                                                      qlpMakeIntParamConstant( param, a1, STL_TRUE ), \
                                                      NULL ) );                                     \
    }

#define STL_PARSER_CONSTRAINT_CHARACTERISTICS____NOT_DEFERRABLE( )                                  \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeConstraintAttr( param,                                        \
                                                      a1,                                           \
                                                      NULL,                                         \
                                                      qlpMakeIntParamConstant( param, a1, STL_FALSE ), \
                                                      NULL ) );                                     \
    }

/*
#define STL_PARSER_CONSTRAINT_CHARACTERISTICS____ENFORCEMENT( ) \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }
*/

/**
 * constraint_check_time
 */

#define STL_PARSER_CONSTRAINT_CHECK_TIME____INIT_DEFERRED( )            \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

#define STL_PARSER_CONSTRAINT_CHECK_TIME____INIT_IMMEDIATE( )           \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_FALSE ) );  \
    }


/**
 * constraint_enforcement
 */

/*
#define STL_PARSER_CONSTRAINT_ENFORCEMENT____ENFORCED( )                                            \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeConstraintAttr( param,                                        \
                                                      a1,                                           \
                                                      NULL,                                         \
                                                      NULL,                                         \
                                                      qlpMakeIntParamConstant( param, a1, STL_TRUE ) ) ); \
    }

#define STL_PARSER_CONSTRAINT_ENFORCEMENT____NOT_ENFORCED( )                                        \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeConstraintAttr( param,                                        \
                                                      a1,                                           \
                                                      NULL,                                         \
                                                      NULL,                                         \
                                                      qlpMakeIntParamConstant( param, a1, STL_FALSE ) ) ); \
    }
*/


/**
 * table_constraint
 */

#define STL_PARSER_TABLE_CONSTRAINT____UNIQUE( )        \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_TABLE_CONSTRAINT____FK( )    \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_TABLE_CONSTRAINT____CHECK( ) \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * unique_constraint_definition
 */

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____UNIQUE( )    \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeUniqueConst( param,       \
                                                   a1,          \
                                                   a1,          \
                                                   v3,          \
                                                   NULL,        \
                                                   NULL,        \
                                                   NULL ) );    \
    }

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____UNIQUE_INDEX_OPTION( )   \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeUniqueConst( param,                   \
                                                   a1,                      \
                                                   a1,                      \
                                                   v3,                      \
                                                   v6,                      \
                                                   NULL,                    \
                                                   v7 ) );                  \
    }

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____UNIQUE_INDEX_ATTR_OPTION( )  \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeUniqueConst( param,                       \
                                                   a1,                          \
                                                   a1,                          \
                                                   v3,                          \
                                                   v6,                          \
                                                   v7,                          \
                                                   v8 ) );                      \
    }

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____PK( )    \
    {                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePKConst( param,       \
                                               a1,          \
                                               a1,          \
                                               v4,          \
                                               NULL,        \
                                               NULL,        \
                                               NULL ) );    \
    }

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____PK_INDEX_OPTION( )   \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakePKConst( param,                   \
                                               a1,                      \
                                               a1,                      \
                                               v4,                      \
                                               v7,                      \
                                               NULL,                    \
                                               v8 ) );                  \
    }

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____PK_INDEX_ATTR_OPTION( )  \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePKConst( param,                       \
                                               a1,                          \
                                               a1,                          \
                                               v4,                          \
                                               v7,                          \
                                               v8,                          \
                                               v9 ) );                      \
    }


/**
 * index_tablespace_name
 */

#define STL_PARSER_INDEX_TABLESPACE_NAME____DEFAULT_TBS( )      \
    {                                                           \
        STL_PARSER_CHECK( v0 = NULL );                          \
    }

#define STL_PARSER_INDEX_TABLESPACE_NAME____TBS_NAME( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v2 );                    \
    }

/**
 * key_column_list
 */

#define STL_PARSER_KEY_COLUMN_LIST____( )       \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

/**
 * unique_column_list
 */

#define STL_PARSER_UNIQUE_COLUMN_LIST____( )    \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * referential_constraint_definition
 */

#define STL_PARSER_REFERENTIAL_CONSTRAINT_DEFINITION____( )     \
    {                                                           \
        qlpConstraint  * sConstr = (qlpConstraint *) v6;        \
        sConstr->mFKFields = v4;                                \
        STL_PARSER_CHECK( v0 = sConstr );                       \
    }

#define STL_PARSER_REFERENTIAL_CONSTRAINT_DEFINITION____INDEX_OPTION( )  \
    { }

#define STL_PARSER_REFERENTIAL_CONSTRAINT_DEFINITION____INDEX_ATTR_OPTION( )  \
    { }

/**
 * reference_column_list
 */

#define STL_PARSER_REFERENCE_COLUMN_LIST____( ) \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * column_definition
 */

#define STL_PARSER_COLUMN_DEFINITION____NAME_TYPE( )                            \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeColumnDef( param,                         \
                                                 a1,                            \
                                                 QLP_GET_PTR_VALUE( v1 ),       \
                                                 a1,                            \
                                                 v2,                            \
                                                 NULL,                          \
                                                 NULL ) );                      \
    }

#define STL_PARSER_COLUMN_DEFINITION____NAME_TYPE_DEFINITION( )                 \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeColumnDef( param,                         \
                                                 a1,                            \
                                                 QLP_GET_PTR_VALUE( v1 ),       \
                                                 a1,                            \
                                                 v2,                            \
                                                 NULL,                          \
                                                 v3 ) );                        \
    }

#define STL_PARSER_COLUMN_DEFINITION____NAME_TYPE_CONSTRAINT( )                                     \
    {                                                                                               \
        qlpListElement * sListElem  = NULL;                                                         \
        qlpConstraint * sConstr     = NULL;                                                         \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeColumnDef( param,                                             \
                                                 a1,                                                \
                                                 QLP_GET_PTR_VALUE( v1 ),                           \
                                                 a1,                                                \
                                                 v2,                                                \
                                                 v3,                                                \
                                                 NULL ) );                                          \
                                                                                                    \
        QLP_LIST_FOR_EACH( (qlpList*)v3, sListElem )                                                \
        {                                                                                           \
            sConstr = (qlpConstraint *) QLP_LIST_GET_POINTER_VALUE( sListElem );                    \
                                                                                                    \
            if( ( sConstr->mConstType == QLP_CONSTRAINT_UNIQUE ) ||                                 \
                ( sConstr->mConstType == QLP_CONSTRAINT_PRIMARY ) )                                 \
            {                                                                                       \
                STL_PARSER_CHECK( sConstr->mUniqueFields = qlpMakeList( param,                      \
                                                                        QLL_POINTER_LIST_TYPE ) );  \
                                                                                                    \
                STL_PARSER_CHECK( qlpAddPtrValueToList( param, sConstr->mUniqueFields, v1 ) );      \
            }                                                                                       \
            else if( sConstr->mConstType == QLP_CONSTRAINT_FOREIGN )                                \
            {                                                                                       \
                STL_PARSER_CHECK( sConstr->mFKFields = qlpMakeList( param,                          \
                                                                    QLL_POINTER_LIST_TYPE ) );      \
                                                                                                    \
                STL_PARSER_CHECK( qlpAddPtrValueToList( param, sConstr->mFKFields, v1 ) );          \
            }                                                                                       \
        }                                                                                           \
    }

#define STL_PARSER_COLUMN_DEFINITION____NAME_TYPE_CONSTRAINT_DEFINITION( )              \
    {                                                                                   \
        qlpListElement * sListElem  = NULL;                                             \
        qlpConstraint * sConstr     = NULL;                                             \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeColumnDef( param,                                 \
                                                 a1,                                    \
                                                 (stlChar*)QLP_GET_PTR_VALUE( v1 ),     \
                                                 a1,                                    \
                                                 v2,                                    \
                                                 v3,                                    \
                                                 v4 ) );                                \
                                                                                        \
        QLP_LIST_FOR_EACH( (qlpList*)v3, sListElem )                                    \
        {                                                                               \
            sConstr = (qlpConstraint *) QLP_LIST_GET_POINTER_VALUE( sListElem );        \
                                                                                        \
            if( ( sConstr->mConstType == QLP_CONSTRAINT_UNIQUE ) ||                     \
                ( sConstr->mConstType == QLP_CONSTRAINT_PRIMARY ) )                     \
            {                                                                           \
                STL_PARSER_CHECK(                                                       \
                    sConstr->mUniqueFields = qlpMakeList( param,                        \
                                                          QLL_POINTER_LIST_TYPE ) );    \
                                                                                        \
                STL_PARSER_CHECK(                                                       \
                    qlpAddPtrValueToList( param, sConstr->mUniqueFields, v1 ) );        \
            }                                                                           \
            else if( sConstr->mConstType == QLP_CONSTRAINT_FOREIGN )                    \
            {                                                                           \
                STL_PARSER_CHECK(                                                       \
                    sConstr->mFKFields = qlpMakeList( param,                            \
                                                      QLL_POINTER_LIST_TYPE ) );        \
                                                                                        \
                STL_PARSER_CHECK(                                                       \
                    qlpAddPtrValueToList( param, sConstr->mFKFields, v1 ) );            \
            }                                                                           \
        }                                                                               \
    }

#define STL_PARSER_COLUMN_DEFINITION____NAME_TYPE_DEFINITION_CONSTRAINT( )              \
    {                                                                                   \
        qlpListElement * sListElem  = NULL;                                             \
        qlpConstraint * sConstr     = NULL;                                             \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeColumnDef( param,                                 \
                                                 a1,                                    \
                                                 (stlChar*)QLP_GET_PTR_VALUE( v1 ),     \
                                                 a1,                                    \
                                                 v2,                                    \
                                                 v4,                                    \
                                                 v3 ) );                                \
                                                                                        \
        QLP_LIST_FOR_EACH( (qlpList*)v4, sListElem )                                    \
        {                                                                               \
            sConstr = (qlpConstraint *) QLP_LIST_GET_POINTER_VALUE( sListElem );        \
                                                                                        \
            if( ( sConstr->mConstType == QLP_CONSTRAINT_UNIQUE ) ||                     \
                ( sConstr->mConstType == QLP_CONSTRAINT_PRIMARY ) )                     \
            {                                                                           \
                STL_PARSER_CHECK(                                                       \
                    sConstr->mUniqueFields = qlpMakeList( param,                        \
                                                          QLL_POINTER_LIST_TYPE ) );    \
                                                                                        \
                STL_PARSER_CHECK(                                                       \
                    qlpAddPtrValueToList( param, sConstr->mUniqueFields, v1 ) );        \
            }                                                                           \
            else if( sConstr->mConstType == QLP_CONSTRAINT_FOREIGN )                    \
            {                                                                           \
                STL_PARSER_CHECK(                                                       \
                    sConstr->mFKFields = qlpMakeList( param,                            \
                                                      QLL_POINTER_LIST_TYPE ) );        \
                                                                                        \
                STL_PARSER_CHECK(                                                       \
                    qlpAddPtrValueToList( param, sConstr->mFKFields, v1 ) );            \
            }                                                                           \
        }                                                                               \
    }

/**
 * date_type_or_domain_name
 */

#define STL_PARSER_DATE_TYPE_OR_DOMAIN_NAME____TYPE( )  \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }


/**
 * column_definition_second_clause : qlpColDefSecond
 */

#define STL_PARSER_COLUMN_DEFINITION_SECOND_CLAUSE____DEFAULT( )        \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_COLUMN_DEFINITION_SECOND_CLAUSE____IDENTITY( )       \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_COLUMN_DEFINITION_SECOND_CLAUSE____GENERATION( )     \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * column_constraint_definitions : qlpList
 */

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITIONS____CONSTRAINT( )       \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( qlpCheckNodeType( param, v1, QLL_CONSTRAINT_TYPE ) );   \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITIONS____CONSTRAINT_LIST( )                  \
    {                                                                                   \
        qlpListElement * sListElem = (qlpListElement *) QLP_LIST_GET_LAST( (qlpList *)v1 ); \
        qllNode * sPrevNode = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sListElem );      \
        qllNode * sAddNode = (qllNode *) v2;                                            \
                                                                                        \
        STL_PARSER_CHECK( qlpCheckNodeType( param, sPrevNode, QLL_CONSTRAINT_TYPE ) );  \
        if( sAddNode->mType == QLL_CONSTRAINT_TYPE )                                    \
        {                                                                               \
            STL_PARSER_CHECK( qlpCheckDuplicateConstraints( param, v1, v2 ) );          \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v2 ) );                  \
            STL_PARSER_CHECK( v0 = v1 );                                                \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            STL_PARSER_CHECK( qlpCheckNodeType( param, sAddNode, QLL_CONSTRAINT_ATTR__TYPE ) ); \
            STL_PARSER_CHECK( qlpAddConstraintCharacteristic( param,                        \
                                                              (qlpConstraint *)sPrevNode,   \
                                                              (qlpConstraintAttr *)sAddNode ) ); \
        }                                                                                   \
    }


/**
 * column_constraint_definition : qlpConstraintAttr
 */

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITION____CONSTRAINT( )        \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITION____NAME_CONSTRAINT( )   \
    {                                                                   \
        ((qlpConstraint*)v2)->mName = qlpMakeObjectName( param, v1 );   \
        STL_PARSER_CHECK( v0 = v2 );                                    \
    }

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITION____CONSTRAINT_WITH_CONSTARINT_CHARACTERISTICS( )    \
    {                                                                                               \
        STL_PARSER_CHECK( qlpAddConstraintCharacteristic( param,                                    \
                                                          (qlpConstraint *) v1,                     \
                                                          (qlpConstraintAttr *) v2 ) );             \
        STL_PARSER_CHECK( v0 = v1 );                                                                \
    }

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITION____NAME_CONSTRAINT_WITH_CONSTARINT_CHARACTERISTICS( ) \
    {                                                                                               \
        ((qlpConstraint*)v2)->mName = qlpMakeObjectName( param, v1 );                               \
        STL_PARSER_CHECK( qlpAddConstraintCharacteristic( param,                                    \
                                                          (qlpConstraint *) v2,                     \
                                                          (qlpConstraintAttr *) v3 ) );             \
        STL_PARSER_CHECK( v0 = v2 );                                                                \
    }



/**
 * column_constraint
 */

#define STL_PARSER_COLUMN_CONSTRAINT____NULL( )             \
    {                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeNullConst( param,     \
                                                 a1,        \
                                                 a1 ) );    \
    }

#define STL_PARSER_COLUMN_CONSTRAINT____NOT_NULL( )         \
    {                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeNotNullConst( param,  \
                                                    a1,     \
                                                    a1 ) ); \
    }

#define STL_PARSER_COLUMN_CONSTRAINT____PRIMARY_KEY( )      \
    {                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePKConst( param,       \
                                               a1,          \
                                               a1,          \
                                               NULL,        \
                                               NULL,        \
                                               NULL,        \
                                               NULL ) );    \
    }

#define STL_PARSER_COLUMN_CONSTRAINT____PRIMARY_KEY_INDEX_OPTION( ) \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePKConst( param,               \
                                               a1,                  \
                                               a1,                  \
                                               NULL,                \
                                               v4,                  \
                                               NULL,                \
                                               v5 ) );              \
    }

#define STL_PARSER_COLUMN_CONSTRAINT____PRIMARY_KEY_INDEX_ATTR_OPTION( )    \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakePKConst( param,                       \
                                               a1,                          \
                                               a1,                          \
                                               NULL,                        \
                                               v4,                          \
                                               v5,                          \
                                               v6 ) );                      \
    }

#define STL_PARSER_COLUMN_CONSTRAINT____UNIQUE( )               \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeUniqueConst( param,       \
                                                   a1,          \
                                                   a1,          \
                                                   NULL,        \
                                                   NULL,        \
                                                   NULL,        \
                                                   NULL ) );    \
    }

#define STL_PARSER_COLUMN_CONSTRAINT____UNIQUE_INDEX_OPTION( )  \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeUniqueConst( param,       \
                                                   a1,          \
                                                   a1,          \
                                                   NULL,        \
                                                   v3,          \
                                                   NULL,        \
                                                   v4 ) );      \
    }

#define STL_PARSER_COLUMN_CONSTRAINT____UNIQUE_INDEX_ATTR_OPTION( ) \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeUniqueConst( param,           \
                                                   a1,              \
                                                   a1,              \
                                                   NULL,            \
                                                   v3,              \
                                                   v4,              \
                                                   v5 ) );          \
    }

#define STL_PARSER_COLUMN_CONSTRAINT____FK( )   \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_COLUMN_CONSTRAINT____FK_INDEX( ) \
    {                                               \
        ((qlpConstraint*)v1)->mIndexName = v3;      \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_COLUMN_CONSTRAINT____CHECK( )    \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }


/**
 * references_specification
 */

#define STL_PARSER_REFERENCES_SPECIFICATION____TABLE_COLUMNS( )         \
    {                                                                   \
        qlpRelInfo  * sRelInfo = NULL;                                  \
                                                                        \
        STL_PARSER_CHECK( sRelInfo = qlpMakeRelInfo( param,             \
                                                     a2,                \
                                                     v2,                \
                                                     NULL ) );          \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFKConst( param,                   \
                                               a1,                      \
                                               a1,                      \
                                               sRelInfo,                \
                                               v3,                      \
                                               NULL,                    \
                                               QLP_FK_MATCH_SIMPLE,     \
                                               0,                       \
                                               QLP_FK_ACTION_NO_ACTION, \
                                               0,                       \
                                               QLP_FK_ACTION_NO_ACTION, \
                                               0 ) );                   \
    }


/**
 * referenced_columns
 */

#define STL_PARSER_REFERENCED_COLUMNS____EMPTY( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = NULL );                  \
    }

#define STL_PARSER_REFERENCED_COLUMNS____COLUMNS( )      \
    {                                                           \
        STL_PARSER_CHECK( v0 = v2 );                            \
    }


/**
 * match_type
 */

#define STL_PARSER_MATCH_TYPE____FULL( )                                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_FK_MATCH_FULL ) );        \
    }

#define STL_PARSER_MATCH_TYPE____PARTIAL( )                                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_FK_MATCH_PARTIAL ) );     \
    }

#define STL_PARSER_MATCH_TYPE____SIMPLE( )                                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_FK_MATCH_SIMPLE ) );      \
    }


/**
 * referential_triggered_action
 */

#define STL_PARSER_REFERENTIAL_TRIGGERED_ACTION____UPDATE( )            \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeRefTriggerAction( param,          \
                                                        a1,             \
                                                        v1,             \
                                                        NULL ) );       \
    }

#define STL_PARSER_REFERENTIAL_TRIGGERED_ACTION____UPDATE_DELETE( )     \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeRefTriggerAction( param,          \
                                                        a1,             \
                                                        v1,             \
                                                        v2 ) );         \
    }

#define STL_PARSER_REFERENTIAL_TRIGGERED_ACTION____DELETE( )    \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeRefTriggerAction( param,  \
                                                        a1,     \
                                                        NULL,   \
                                                        v1 ) ); \
    }

#define STL_PARSER_REFERENTIAL_TRIGGERED_ACTION____DELETE_UPDATE( )     \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeRefTriggerAction( param,          \
                                                        a1,             \
                                                        v2,             \
                                                        v1 ) );         \
    }


/**
 * update_rule
 */

#define STL_PARSER_UPDATE_RULE____( )    \
    {                                           \
        STL_PARSER_CHECK( v0 = v2 );            \
    }


/**
 * delete_rule
 */

#define STL_PARSER_DELETE_RULE____( )    \
    {                                           \
        STL_PARSER_CHECK( v0 = v2 );            \
    }


/**
 * referential_action
 */

#define STL_PARSER_REFERENTIAL_ACTION____CASCADE( )                                     \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_FK_ACTION_CASCADE ) );   \
    }

#define STL_PARSER_REFERENTIAL_ACTION____SET_NULL( )                                    \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_FK_ACTION_SET_NULL ) );  \
    }

#define STL_PARSER_REFERENTIAL_ACTION____SET_DEFAULT( )                                         \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_FK_ACTION_SET_DEFAULT ) );       \
    }

#define STL_PARSER_REFERENTIAL_ACTION____RESTRICT( )                                    \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_FK_ACTION_RESTRICT ) );  \
    }

#define STL_PARSER_REFERENTIAL_ACTION____NO_ACTION( )                                   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param, a1, QLP_FK_ACTION_NO_ACTION ) ); \
    }


/**
 * check_constraint_definition
 */

#define STL_PARSER_CHECK_CONSTRAINT_DEFINITION____( )                   \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCheckConst( param,                \
                                                  a1,                   \
                                                  a1,                   \
                                                  (qllNode *) v3 ) );   \
    }


/**
 * schema_qualified_name
 */

#define STL_PARSER_SCHEMA_QUALIFIED_NAME____( )  \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }


/**
 * default_clause : qlpColDefSecond
 */

#define STL_PARSER_DEFAULT_CLAUSE____( )                                                \
    {                                                                                   \
        qlpValueExpr  * sValueExpr = NULL;                                              \
        STL_PARSER_CHECK( sValueExpr = qlpMakeValueExpr( param,                         \
                                                         a2,                            \
                                                         QLP_GET_LAST_POS( a2 ) - a2,   \
                                                         v2 ) );                        \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeColDefSecond( param,                              \
                                                    a1,                                 \
                                                    (qllNode*)sValueExpr,               \
                                                    QLP_IDENTITY_INVALID,               \
                                                    0,                                  \
                                                    NULL,                               \
                                                    NULL ) );                           \
    }


/**
 * default_option : qllNode
 */

#define STL_PARSER_DEFAULT_OPTION____VALUE_EXPR( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = (qllNode *) v1 );        \
    }

/**
 * identity_column_specification : qlpColDefSecond
 */

#define STL_PARSER_IDENTITY_COLUMN_SPECIFICATION____ALWAYS( )                   \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeColDefSecond( param,                      \
                                                    a1,                         \
                                                    NULL,                       \
                                                    QLP_IDENTITY_ALWAYS,        \
                                                    a2,                         \
                                                    NULL,                       \
                                                    NULL ) );                   \
    }

#define STL_PARSER_IDENTITY_COLUMN_SPECIFICATION____BY_DEFAULT( )               \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeColDefSecond( param,                      \
                                                    a1,                         \
                                                    NULL,                       \
                                                    QLP_IDENTITY_BY_DEFAULT,    \
                                                    a2,                         \
                                                    NULL,                       \
                                                    NULL ) );                   \
    }

#define STL_PARSER_IDENTITY_COLUMN_SPECIFICATION____ALWAYS_SEQUENCE( )                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeColDefSecond( param,                              \
                                                    a1,                                 \
                                                    NULL,                               \
                                                    QLP_IDENTITY_ALWAYS,                \
                                                    a2,                                 \
                                                    qlpMakeSequenceOption( param, v6 ), \
                                                    NULL ) );                           \
    }

#define STL_PARSER_IDENTITY_COLUMN_SPECIFICATION____BY_DEFAULT_SEQUENCE( )              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeColDefSecond( param,                              \
                                                    a1,                                 \
                                                    NULL,                               \
                                                    QLP_IDENTITY_BY_DEFAULT,            \
                                                    a2,                                 \
                                                    qlpMakeSequenceOption( param, v7 ), \
                                                    NULL ) );                           \
    }


/**
 * generation_clause : qlpColDefSecond
 */

#define STL_PARSER_GENERATION_CLAUSE____( )                                     \
    {                                                                           \
        qlpValueExpr  * sValueExpr = NULL;                                      \
        STL_PARSER_CHECK( sValueExpr = qlpMakeValueExpr( param,                 \
                                                         a5,                    \
                                                         a6 - a5,               \
                                                         v5 ) );                \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeColDefSecond( param,                      \
                                                    a1,                         \
                                                    NULL,                       \
                                                    QLP_IDENTITY_INVALID,       \
                                                    0,                          \
                                                    NULL,                       \
                                                    (qllNode*) sValueExpr ) );  \
    }


/**
 * common_sequence_generator_options : qlpList : qlpDefElem
 */

#define STL_PARSER_COMMON_SEQUENCE_GENERATOR_OPTIONS____OPTION( )       \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
                                                                        \
    }

#define STL_PARSER_COMMON_SEQUENCE_GENERATOR_OPTIONS____OPTION_LIST( )  \
    {                                                                   \
        STL_PARSER_CHECK( qlpCheckDuplicateOptions( param, v1, v2 ) );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v2 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * common_sequence_generator_option
 */

#define STL_PARSER_COMMON_SEQUENCE_GENERATOR_OPTION____START_WITH( )    \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_COMMON_SEQUENCE_GENERATOR_OPTION____SEQUENCE( )      \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * sequence_generator_start_with_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_START_WITH_OPTION____( )           \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                                 \
                                               QLP_SEQUENCE_OPTION_START_WITH,     \
                                               (qllNode *) v3 ) );              \
    }


/**
 * sequence_generator_start_value : qlpValue : float
 */

#define STL_PARSER_SEQUENCE_GENERATOR_START_VALUE____( )                \
    {                                                                   \
        STL_PARSER_CHECK( v0 = ((qlpConstantValue*)v1)->mValue );       \
    }


/**
 * basic_sequence_generator_option
 */

#define STL_PARSER_BASIC_SEQUENCE_GENERATOR_OPTION____INCREMENT( )       \
    {                                                                           \
        STL_PARSER_CHECK( v0 = v1 );                                            \
    }

#define STL_PARSER_BASIC_SEQUENCE_GENERATOR_OPTION____MAXVALUE( )        \
    {                                                                           \
        STL_PARSER_CHECK( v0 = v1 );                                            \
    }

#define STL_PARSER_BASIC_SEQUENCE_GENERATOR_OPTION____MINVALUE( )        \
    {                                                                           \
        STL_PARSER_CHECK( v0 = v1 );                                            \
    }

#define STL_PARSER_BASIC_SEQUENCE_GENERATOR_OPTION____CYCLE( )   \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_BASIC_SEQUENCE_GENERATOR_OPTION____CACHE( )   \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * sequence_generator_increment_by_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_INCREMENT_BY_OPTION____( )                \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_SEQUENCE_OPTION_INCREMENT,   \
                                               (qllNode *) v3 ) );              \
    }


/**
 * sequence_generator_increment
 */

#define STL_PARSER_SEQUENCE_GENERATOR_INCREMENT____( )                  \
    {                                                                   \
        STL_PARSER_CHECK( v0 = ((qlpConstantValue*)v1)->mValue );       \
    }


/**
 * sequence_generator_maxvalue_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_MAXVALUE_OPTION____MAXVALUE( )            \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_SEQUENCE_OPTION_MAXVALUE,    \
                                               (qllNode *) v2 ) );              \
    }

#define STL_PARSER_SEQUENCE_GENERATOR_MAXVALUE_OPTION____NO_MAXVALUE( )         \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_SEQUENCE_OPTION_MAXVALUE,    \
                                               NULL ) );                        \
    }

#define STL_PARSER_SEQUENCE_GENERATOR_MAXVALUE_OPTION____NOMAXVALUE( )          \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_SEQUENCE_OPTION_MAXVALUE,    \
                                               NULL ) );                        \
    }


/**
 * sequence_generator_max_value
 */

#define STL_PARSER_SEQUENCE_GENERATOR_MAX_VALUE____( )                  \
    {                                                                   \
        STL_PARSER_CHECK( v0 = ((qlpConstantValue*)v1)->mValue );       \
    }


/**
 * sequence_generator_minvalue_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_MINVALUE_OPTION____MINVALUE( )            \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_SEQUENCE_OPTION_MINVALUE,    \
                                               (qllNode *) v2 ) );              \
    }

#define STL_PARSER_SEQUENCE_GENERATOR_MINVALUE_OPTION____NO_MINVALUE( )         \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_SEQUENCE_OPTION_MINVALUE,    \
                                               NULL ) );                        \
    }

#define STL_PARSER_SEQUENCE_GENERATOR_MINVALUE_OPTION____NOMINVALUE( )          \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_SEQUENCE_OPTION_MINVALUE,    \
                                               NULL ) );                        \
    }



/**
 * sequence_generator_min_value
 */

#define STL_PARSER_SEQUENCE_GENERATOR_MIN_VALUE____( )                  \
    {                                                                   \
        STL_PARSER_CHECK( v0 = ((qlpConstantValue*)v1)->mValue );       \
    }


/**
 * sequence_generator_cycle_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_CYCLE_OPTION____CYCLE( )                                  \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                                           \
                                               a1,                                              \
                                               QLP_SEQUENCE_OPTION_CYCLE,                       \
                                               (qllNode *) qlpMakeIntParamConstant( param,              \
                                                                            a1,                  \
                                                                            STL_TRUE ) ) );     \
    }

#define STL_PARSER_SEQUENCE_GENERATOR_CYCLE_OPTION____NO_CYCLE( )                               \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                                           \
                                               a1,                                              \
                                               QLP_SEQUENCE_OPTION_CYCLE,                       \
                                               (qllNode *) qlpMakeIntParamConstant( param,              \
                                                                            a1,                  \
                                                                            STL_FALSE ) ) );    \
    }

#define STL_PARSER_SEQUENCE_GENERATOR_CYCLE_OPTION____NOCYCLE( )                                \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                                           \
                                               a1,                                              \
                                               QLP_SEQUENCE_OPTION_CYCLE,                       \
                                               (qllNode *) qlpMakeIntParamConstant( param,              \
                                                                            a1,                 \
                                                                            STL_FALSE ) ) );    \
    }


/**
 * sequence_generator_cache_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_CACHE_OPTION____CACHE( )                  \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_SEQUENCE_OPTION_CACHE,       \
                                               (qllNode *) v2 ) );              \
    }

#define STL_PARSER_SEQUENCE_GENERATOR_CACHE_OPTION____NO_CACHE( )                                   \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                                               \
                                               a1,                                                   \
                                               QLP_SEQUENCE_OPTION_CACHE,                        \
                                               (qllNode *) qlpMakeIntParamConstant( param, a1, STL_TRUE ) ) ); \
    }

#define STL_PARSER_SEQUENCE_GENERATOR_CACHE_OPTION____NOCACHE( )                                    \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                                               \
                                               a1,                                                  \
                                               QLP_SEQUENCE_OPTION_CACHE,                        \
                                               (qllNode *) qlpMakeIntParamConstant( param, a1, STL_TRUE ) ) ); \
    }


/**
 * sequence_generator_cache_value
 */

#define STL_PARSER_SEQUENCE_GENERATOR_CACHE_VALUE____( )                \
    {                                                                   \
        STL_PARSER_CHECK( v0 = ((qlpConstantValue*)v1)->mValue );       \
    }


/**
 * truncate_table_statement
 */

#define STL_PARSER_TRUNCATE_TABLE_STATEMENT()                   \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeTruncateTable( param,     \
                                                     a1,        \
                                                     v3,        \
                                                     v4,        \
                                                     v5 ) );    \
    }



/**
 * identity_column_restart_option
 */

#define STL_PARSER_IDENTITY_COLUMN_RESTART_OPTION____EMPTY()    \
    {                                                           \
        STL_PARSER_CHECK( v0 = NULL );                          \
    }

#define STL_PARSER_IDENTITY_COLUMN_RESTART_OPTION____CONTINUE()                     \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeIntParamConstant( param,                                    \
                                          a1,                                       \
                                          QLP_IDENTITY_RESTART_OPTION_CONTINUE ) ); \
    }

#define STL_PARSER_IDENTITY_COLUMN_RESTART_OPTION____RESTART()                      \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeIntParamConstant( param,                                    \
                                          a1,                                       \
                                          QLP_IDENTITY_RESTART_OPTION_RESTART ) );  \
    }


/*
 * drop_storage_option
 */

#define STL_PARSER_DROP_STORAGE_OPTION____EMPTY()   \
    {                                               \
        STL_PARSER_CHECK( v0 = NULL );              \
    }

#define STL_PARSER_DROP_STORAGE_OPTION____DROP()                        \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeIntParamConstant( param,                        \
                                          a1,                           \
                                          QLP_DROP_STORAGE_DROP ) );    \
    }

#define STL_PARSER_DROP_STORAGE_OPTION____DROP_ALL()                        \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            v0 = qlpMakeIntParamConstant( param,                            \
                                          a1,                               \
                                          QLP_DROP_STORAGE_DROP_ALL ) );    \
    }

#define STL_PARSER_DROP_STORAGE_OPTION____REUSE()                       \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeIntParamConstant( param,                        \
                                          a1,                           \
                                          QLP_DROP_STORAGE_REUSE ) );   \
    }


/** @} qlpParser */


#endif /* _QLPPARSERMACROPART_1_H_ */
