/*******************************************************************************
 * ztqSqlMacro.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        vId: ztqSqlMacro.h 2754 2011-12-15 02:27:23Z jhkim v
 *
 * NOTES
 *
 *
 ******************************************************************************/

#include <stl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztqDef.h>
#include <ztqSqlParser.h>

/******************************************************************************* 
 * CLAUSES
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

#define STL_PARSER_COMMAND____EMPTY( )              \
    { param->mParseTree = ZTQ_EMPTY_PARSE_TREE; }

#define STL_PARSER_COMMAND____ALTER_DATABASE( )                                 \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_ALTER,           \
                                                      ZTQ_OBJECT_DATABASE,      \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____USER_DEFINITION( )                        \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            param->mParseTree = ztqMakeBypassSqlTree( param,            \
                                                      ZTQ_STMT_CREATE,  \
                                                      ZTQ_OBJECT_USER,  \
                                                      NULL ) );         \
    }

#define STL_PARSER_COMMAND____DROP_USER( )                              \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            param->mParseTree = ztqMakeBypassSqlTree( param,            \
                                                      ZTQ_STMT_DROP,    \
                                                      ZTQ_OBJECT_USER,  \
                                                      NULL ) );         \
    }

#define STL_PARSER_COMMAND____ALTER_USER( )                             \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            param->mParseTree = ztqMakeBypassSqlTree( param,            \
                                                      ZTQ_STMT_ALTER,   \
                                                      ZTQ_OBJECT_USER,  \
                                                      NULL ) );         \
    }

#define STL_PARSER_COMMAND____PROFILE_DEFINITION( )                        \
    {                                                                      \
        STL_PARSER_CHECK(                                                  \
            param->mParseTree = ztqMakeBypassSqlTree( param,               \
                                                      ZTQ_STMT_CREATE,     \
                                                      ZTQ_OBJECT_PROFILE,  \
                                                      NULL ) );            \
    }

#define STL_PARSER_COMMAND____DROP_PROFILE( )                              \
    {                                                                      \
        STL_PARSER_CHECK(                                                  \
            param->mParseTree = ztqMakeBypassSqlTree( param,               \
                                                      ZTQ_STMT_DROP,       \
                                                      ZTQ_OBJECT_PROFILE,  \
                                                      NULL ) );            \
    }

#define STL_PARSER_COMMAND____ALTER_PROFILE( )                             \
    {                                                                      \
        STL_PARSER_CHECK(                                                  \
            param->mParseTree = ztqMakeBypassSqlTree( param,               \
                                                      ZTQ_STMT_ALTER,      \
                                                      ZTQ_OBJECT_PROFILE,  \
                                                      NULL ) );            \
    }


#define STL_PARSER_COMMAND____GRANT_PRIVILEGE( )                            \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_GRANT,       \
                                                      ZTQ_OBJECT_UNKNOWN,   \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____REVOKE_PRIVILEGE( )                           \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_REVOKE,      \
                                                      ZTQ_OBJECT_UNKNOWN,   \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____TABLESPACE_DEFINITION( )                          \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_CREATE,          \
                                                      ZTQ_OBJECT_TABLESPACE,    \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____ALTER_TABLESPACE( )                                   \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            param->mParseTree = ztqMakeBypassSqlTree( param,                        \
                                                      ZTQ_STMT_ALTER,               \
                                                      ZTQ_OBJECT_TABLESPACE,        \
                                                      NULL ) );                     \
    }

#define STL_PARSER_COMMAND____DROP_TABLESPACE( )                                    \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            param->mParseTree = ztqMakeBypassSqlTree( param,                        \
                                                      ZTQ_STMT_DROP,                \
                                                      ZTQ_OBJECT_TABLESPACE,        \
                                                      NULL ) );                     \
    }

#define STL_PARSER_COMMAND____SCHEMA_DEFINITION( )                          \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_CREATE,      \
                                                      ZTQ_OBJECT_SCHEMA,    \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____DROP_SCHEMA( )                                \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_DROP,        \
                                                      ZTQ_OBJECT_SCHEMA,    \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____TABLE_DEFINITION( )                           \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_CREATE,      \
                                                      ZTQ_OBJECT_TABLE,     \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____DROP_TABLE( )                                 \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_DROP,        \
                                                      ZTQ_OBJECT_TABLE,     \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____ALTER_TABLE( )                            \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            param->mParseTree = ztqMakeBypassSqlTree( param,            \
                                                      ZTQ_STMT_ALTER,   \
                                                      ZTQ_OBJECT_TABLE, \
                                                      NULL ) );         \
    }

#define STL_PARSER_COMMAND____VIEW_DEFINITION( )                        \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            param->mParseTree = ztqMakeBypassSqlTree( param,            \
                                                      ZTQ_STMT_CREATE,  \
                                                      ZTQ_OBJECT_VIEW,  \
                                                      NULL ) );         \
    }

#define STL_PARSER_COMMAND____DROP_VIEW( )                              \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            param->mParseTree = ztqMakeBypassSqlTree( param,            \
                                                      ZTQ_STMT_DROP,    \
                                                      ZTQ_OBJECT_VIEW,  \
                                                      NULL ) );         \
    }

#define STL_PARSER_COMMAND____ALTER_VIEW( )                             \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            param->mParseTree = ztqMakeBypassSqlTree( param,            \
                                                      ZTQ_STMT_ALTER,   \
                                                      ZTQ_OBJECT_VIEW,  \
                                                      NULL ) );         \
    }

#define STL_PARSER_COMMAND____SEQUENCE_DEFINITION( )                            \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_CREATE,          \
                                                      ZTQ_OBJECT_SEQUENCE,      \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____DROP_SEQUENCE( )                                  \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_DROP,            \
                                                      ZTQ_OBJECT_SEQUENCE,      \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____ALTER_SEQUENCE()                              \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_ALTER,       \
                                                      ZTQ_OBJECT_SEQUENCE,  \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____SYNONYM_DEFINITION( )                             \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_CREATE,          \
                                                      ZTQ_OBJECT_SYNONYM,       \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____DROP_SYNONYM( )                                   \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_DROP,            \
                                                      ZTQ_OBJECT_SYNONYM,       \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____INDEX_DEFINITION( )                           \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_CREATE,      \
                                                      ZTQ_OBJECT_INDEX,     \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____DROP_INDEX( )                                 \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_DROP,        \
                                                      ZTQ_OBJECT_INDEX,     \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____ALTER_INDEX( )                            \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            param->mParseTree = ztqMakeBypassSqlTree( param,            \
                                                      ZTQ_STMT_ALTER,   \
                                                      ZTQ_OBJECT_INDEX, \
                                                      NULL ) );         \
    }

#define STL_PARSER_COMMAND____TRUNCATE_TABLE( )                             \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_TRUNCATE,    \
                                                      ZTQ_OBJECT_TABLE,     \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____INSERT( )                                     \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_INSERT,      \
                                                      ZTQ_OBJECT_TABLE,     \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____UPDATE( )                                     \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_UPDATE,      \
                                                      ZTQ_OBJECT_TABLE,     \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____UPDATE_POSITIONED( )                      \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            param->mParseTree = ztqMakeBypassSqlTree( param,            \
                                                      ZTQ_STMT_UPDATE,  \
                                                      ZTQ_OBJECT_TABLE, \
                                                      NULL ) );         \
    }

#define STL_PARSER_COMMAND____DELETE( )                                     \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_DELETE,      \
                                                      ZTQ_OBJECT_TABLE,     \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____DELETE_POSITIONED( )                      \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            param->mParseTree = ztqMakeBypassSqlTree( param,            \
                                                      ZTQ_STMT_DELETE,  \
                                                      ZTQ_OBJECT_TABLE, \
                                                      NULL ) );         \
    }

#define STL_PARSER_COMMAND____SELECT( )                                     \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_SELECT,      \
                                                      ZTQ_OBJECT_TABLE,     \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____SELECT_INTO( )                                \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_SELECT_INTO, \
                                                      ZTQ_OBJECT_TABLE,     \
                                                      NULL ) );             \
    }


#define STL_PARSER_COMMAND____QUERY_EXPRESSION( )                               \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_QUERY_EXPR,      \
                                                      ZTQ_OBJECT_TABLE,         \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____SAVEPOINT( )                                      \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_CREATE,          \
                                                      ZTQ_OBJECT_SAVEPOINT,     \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____RELEASE_SAVEPOINT( )                          \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_DROP,        \
                                                      ZTQ_OBJECT_SAVEPOINT, \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____COMMIT( )                                         \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_COMMIT,          \
                                                      ZTQ_OBJECT_UNKNOWN,   \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____ROLLBACK( )       \
    { param->mParseTree = v1; }

#define STL_PARSER_COMMAND____ALTER_SYSTEM( )                                   \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_ALTER,           \
                                                      ZTQ_OBJECT_SYSTEM,        \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____ALTER_SESSION( )                                  \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_ALTER,           \
                                                      ZTQ_OBJECT_SESSION,       \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____LOCK_TABLE()                                  \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_LOCK,        \
                                                      ZTQ_OBJECT_TABLE,     \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____COMMENT()                                         \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_COMMENT,         \
                                                      ZTQ_OBJECT_UNKNOWN,       \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____SET_CONSTRAINT_MODE()                             \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_SET,             \
                                                      ZTQ_OBJECT_CONSTRAINTS,   \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____SET_TRANSACTION()                                         \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            param->mParseTree = ztqMakeBypassSqlTree( param,                            \
                                                      ZTQ_STMT_SET,                     \
                                                      ZTQ_OBJECT_TRANSACTION,           \
                                                      NULL ) );                         \
    }

#define STL_PARSER_COMMAND____SET_SESSION_CHARACTERISTICS()                     \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_SET,             \
                                                      ZTQ_OBJECT_SESSION,       \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____SET_SESSION_AUTH()                            \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_SET,         \
                                                      ZTQ_OBJECT_SESSION,   \
                                                      NULL ) );             \
    }

#define STL_PARSER_COMMAND____SET_TIME_ZONE()                                   \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_SET,             \
                                                      ZTQ_OBJECT_SESSION,       \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____DECLARE_STATEMENT()   \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_COMMAND____DECLARE_CURSOR()                                  \
    {                                                                           \
        STL_PARSER_CHECK( v0 = v1 );                                            \
    }

#define STL_PARSER_COMMAND____OPEN_CURSOR()     \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_COMMAND____CLOSE_CURSOR()                                    \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_CLOSE_CURSOR,    \
                                                      ZTQ_OBJECT_UNKNOWN,       \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____FETCH_CURSOR()                                    \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_FETCH_CURSOR,    \
                                                      ZTQ_OBJECT_UNKNOWN,       \
                                                      NULL ) );                 \
    }

#define STL_PARSER_COMMAND____CONNECTION()          \
    {                                               \
        param->mParseTree = ZTQ_EMPTY_PARSE_TREE;   \
    }   


/**
 * user_definition
 */

#define STL_PARSE_USER_DEFINTION____WITH_SCHEMA_EMPTY() \
    {  }
#define STL_PARSE_USER_DEFINTION____WITH_SCHEMA_NAME() \
    {  }
#define STL_PARSE_USER_DEFINTION____WITHOUT_SCHEMA_NAME() \
    {  }
#define STL_PARSE_SENSITIVE_PASSWORD____REGULAR() \
    {  }
#define STL_PARSE_SENSITIVE_PASSWORD____DELIMITED() \
    {  }
#define STL_PARSE_USER_PROFILE____NULL() \
    {  }
#define STL_PARSE_USER_PROFILE____NAME() \
    {  }
#define STL_PARSE_USER_PROFILE____DEFAULT() \
    {  }
#define STL_PARSE_PASSWORD_EXPIRE____EMPTY() \
    {  }
#define STL_PARSE_PASSWORD_EXPIRE____EXIST() \
    {  }
#define STL_PARSE_ACCOUNT____UNLOCK() \
    {  }
#define STL_PARSE_ACCOUNT____LOCK() \
    {  }
#define STL_PARSE_DEFAULT_TABLESPACE____EMPTY() \
    {  }
#define STL_PARSE_DEFAULT_TABLESPACE____NAME() \
    {  }
#define STL_PARSE_TEMPORARY_TABLESPACE____EMPTY() \
    {  }
#define STL_PARSE_TEMPORARY_TABLESPACE____NAME() \
    {  }

/**
 * drop_user_statement
 */

#define STL_PARSER_DROP_USER_STATEMENT() \
    {  }

/*
 * alter_user_statement
 */

#define STL_PARSER_ALTER_USER_STATEMENT____PASSWORD_ONLY()  \
    {  }
#define STL_PARSER_ALTER_USER_STATEMENT____PASSWORD_REPLACE() \
    {  }
#define STL_PARSER_ALTER_USER_STATEMENT____ALTER_PROFILE() \
    {  }
#define STL_PARSER_ALTER_USER_STATEMENT____PASSWORD_EXPIRE() \
    {  }
#define STL_PARSER_ALTER_USER_STATEMENT____ACCOUNT_LOCK() \
    {  }
#define STL_PARSER_ALTER_USER_STATEMENT____ACCOUNT_UNLOCK() \
    {  }
#define STL_PARSER_ALTER_USER_STATEMENT____DEFAULT_SPACE() \
    {  }
#define STL_PARSER_ALTER_USER_STATEMENT____TEMPORARY_SPACE() \
    {  }
#define STL_PARSER_ALTER_USER_STATEMENT____SCHEMA_PATH() \
    {  }
#define STL_PARSER_SCHEMA_NAME_LIST____SCHEMA_NAME() \
    {  }
#define STL_PARSER_SCHEMA_NAME_LIST____SCHEMA_NAME_LIST() \
    {  }
#define STL_PARSER_SCHEMA_NAME____IDENTIFIER() \
    {  }
#define STL_PARSER_SCHEMA_NAME____CURRENT_PATH() \
    {  }

/**
 * profile_definition
 */
        
#define STL_PARSER_PROFILE_DEFINITION____() \
    {  }

/**
 * password_parameter_list
 */

#define STL_PARSER_PASSWORD_PARAMETERS____PARAMETER() \
    {  }

#define STL_PARSER_PASSWORD_PARAMETERS____PARAMETER_LIST() \
    {  }

/**
 * password_parameter
 */

#define STL_PARSER_PASSWORD_PARAMETER____FAILED_LOGIN_ATTEMPTS() \
    {  }

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_LIFE_TIME()    \
    {  }

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_LOCK_TIME()    \
    {  }

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_GRACE_TIME()   \
    {  }

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_REUSE_MAX()   \
    {  } 

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_REUSE_TIME()  \
    {  } 

#define STL_PARSER_PASSWORD_PARAMETER____PASSWORD_VERIFY_FUNCTION()  \
    {  } 


/**
 * failed_login_attempts
 */

#define STL_PARSER_FAILED_LOGIN_ATTEMPTS____UNSIGNED_INTEGER() \
    {  }

#define STL_PARSER_FAILED_LOGIN_ATTEMPTS____UNLIMITED() \
    {  }

#define STL_PARSER_FAILED_LOGIN_ATTEMPTS____DEFAULT() \
    {  }


/**
 * password_life_time
 */

#define STL_PARSER_PASSWORD_LIFE_TIME____INTERVAL_VALUE() \
    {  }

#define STL_PARSER_PASSWORD_LIFE_TIME____UNLIMITED() \
    {  }

#define STL_PARSER_PASSWORD_LIFE_TIME____DEFAULT() \
    {  }

/**
 * password_lock_time
 */

#define STL_PARSER_PASSWORD_LOCK_TIME____INTERVAL_VALUE() \
    {  }

#define STL_PARSER_PASSWORD_LOCK_TIME____UNLIMITED() \
    {  }

#define STL_PARSER_PASSWORD_LOCK_TIME____DEFAULT() \
    {  }


/**
 * password_grace_time
 */

#define STL_PARSER_PASSWORD_GRACE_TIME____INTERVAL_VALUE() \
    {  }

#define STL_PARSER_PASSWORD_GRACE_TIME____UNLIMITED() \
    {  }

#define STL_PARSER_PASSWORD_GRACE_TIME____DEFAULT() \
    {  }

/**
 * password_reuse_max
 */

#define STL_PARSER_PASSWORD_REUSE_MAX____UNSIGNED_INTEGER() \
    {  }

#define STL_PARSER_PASSWORD_REUSE_MAX____UNLIMITED() \
    {  }

#define STL_PARSER_PASSWORD_REUSE_MAX____DEFAULT() \
    {  }

/**
 * password_reuse_time
 */

#define STL_PARSER_PASSWORD_REUSE_TIME____INTERVAL_VALUE() \
    {  }

#define STL_PARSER_PASSWORD_REUSE_TIME____UNLIMITED() \
    {  }

#define STL_PARSER_PASSWORD_REUSE_TIME____DEFAULT() \
    {  }

/**
 * password_verify_function
 */

#define STL_PARSER_PASSWORD_VERIFY_FUNCTION____VERIFY_POLICY() \
    {  }

#define STL_PARSER_PASSWORD_VERIFY_FUNCTION____NULL() \
    {  }

#define STL_PARSER_PASSWORD_VERIFY_FUNCTION____DEFAULT() \
    {  }


/**
 * password_parameter_number_interval
 */

#define STL_PARSER_PASSWORD_PARAMTER_NUMBER_INTERVAL____NUMBER( )   \
    {  }

#define STL_PARSER_PASSWORD_PARAMTER_NUMBER_INTERVAL____DIVIDE( )   \
    {  }

/**
 * drop_profile_statement
 */

#define STL_PARSER_DROP_PROFILE_STATEMENT____() \
    {  }

#define STL_PARSER_DROP_PROFILE_STATEMENT____CASCADE() \
    {  }

/**
 * alter_profile_statement
 */

        
#define STL_PARSER_ALTER_PROFILE_STATEMENT____USER_PROFILE() \
    {  }            
#define STL_PARSER_ALTER_PROFILE_STATEMENT____DEFAULT_PROFILE() \
    {  }            


/**
 * grant_privilege_statement
 */

#define STL_PARSER_GRANT_PRIVILEGE____WITHOUT_GRANT()   \
    {  }
#define STL_PARSER_GRANT_PRIVILEGE____WITH_GRANT()  \
    {  }

#define STL_PARSER_GRANT_PRIVILEGE____WITHOUT_SCHEMA_PRIVILEGE_WITHOUT_GRANT()  \
    {  }
#define STL_PARSER_GRANT_PRIVILEGE____WITHOUT_SCHEMA_PRIVILEGE_WITH_GRANT()  \
    {  }

/*
 * REVOKE PRIVILEGE
 */

#define STL_PARSER_REVOKE_PRIVILEGE____WITHOUT_GRANT() \
    { }
#define STL_PARSER_REVOKE_PRIVILEGE____WITH_GRANT() \
    { }
#define STL_PARSER_REVOKE_PRIVILEGE____WITHOUT_SCHEMA_PRIVILEGE_WITHOUT_GRANT() \
    { }
#define STL_PARSER_REVOKE_PRIVILEGE____WITHOUT_SCHEMA_PRIVILEGE_WITH_GRANT() \
    { }

#define STL_PARSER_REVOKE_BEHAVIOR____EMPTY() \
   {  }
#define STL_PARSER_REVOKE_BEHAVIOR____RESTRICT() \
   {  }
#define STL_PARSER_REVOKE_BEHAVIOR____CASCADE() \
   {  }
#define STL_PARSER_REVOKE_BEHAVIOR____CASCADE_CONSTRAINTS() \
   {  }

#define STL_PARSER_GRANTEE_LIST____GRANTEE()    \
    {  }
#define STL_PARSER_GRANTEE_LIST____GRANTEE_LIST()   \
    {  }
#define STL_PARSER_GRANTEE_NAME()               \
    {  }

#define STL_PARSER_PRIVILEGE____DATABASE()      \
    {  }
#define STL_PARSER_PRIVILEGE____TABLESPACE()    \
    {  }
#define STL_PARSER_PRIVILEGE____SCHEMA()        \
    {  }
#define STL_PARSER_PRIVILEGE____TABLE()         \
    {  }
#define STL_PARSER_PRIVILEGE____SEQUENCE()      \
    {  }

#define STL_PARSER_PRIVILEGE_ACTION_LIST____ACTION()    \
    {  }
#define STL_PARSER_PRIVILEGE_ACTION_LIST____ACTION_LIST()   \
    {  }

/**
 * database_privilege
 */

#define STL_PARSER_DATABASE_PRIVILEGE____ACTION_LIST()  \
    {  }
#define STL_PARSER_DATABASE_PRIVILEGE____ALL_PRIVILEGES() \
    {  }
#define STL_PARSER_DATABASE_ACTION____ADMINISTRATION()  \
    {  }
#define STL_PARSER_DATABASE_ACTION____ALTER_DATABASE()  \
    {  }
#define STL_PARSER_DATABASE_ACTION____ALTER_SYSTEM()    \
    {  }
#define STL_PARSER_DATABASE_ACTION____ACCESS_CONTROL()  \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_SESSION()  \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_USER() \
    {  }
#define STL_PARSER_DATABASE_ACTION____ALTER_USER()  \
    {  }
#define STL_PARSER_DATABASE_ACTION____DROP_USER()   \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_PROFILE() \
    {  }
#define STL_PARSER_DATABASE_ACTION____ALTER_PROFILE()  \
    {  }
#define STL_PARSER_DATABASE_ACTION____DROP_PROFILE()   \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_ROLE() \
    {  }
#define STL_PARSER_DATABASE_ACTION____ALTER_ROLE()  \
    {  }
#define STL_PARSER_DATABASE_ACTION____DROP_ROLE()   \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_TABLESPACE()   \
    {  }
#define STL_PARSER_DATABASE_ACTION____ALTER_TABLESPACE()    \
    {  }
#define STL_PARSER_DATABASE_ACTION____DROP_TABLESPACE() \
    {  }
#define STL_PARSER_DATABASE_ACTION____USAGE_TABLESPACE()    \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_SCHEMA()   \
    {  }
#define STL_PARSER_DATABASE_ACTION____ALTER_SCHEMA() \
    {  }
#define STL_PARSER_DATABASE_ACTION____DROP_SCHEMA() \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_ANY_TABLE()    \
    {  }
#define STL_PARSER_DATABASE_ACTION____ALTER_ANY_TABLE() \
    {  }
#define STL_PARSER_DATABASE_ACTION____DROP_ANY_TABLE()  \
    {  }
#define STL_PARSER_DATABASE_ACTION____SELECT_ANY_TABLE()    \
    {  }
#define STL_PARSER_DATABASE_ACTION____INSERT_ANY_TABLE()    \
    {  }
#define STL_PARSER_DATABASE_ACTION____DELETE_ANY_TABLE()    \
    {  }
#define STL_PARSER_DATABASE_ACTION____UPDATE_ANY_TABLE()    \
    {  }
#define STL_PARSER_DATABASE_ACTION____LOCK_ANY_TABLE()  \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_ANY_VIEW() \
    {  }
#define STL_PARSER_DATABASE_ACTION____DROP_ANY_VIEW()   \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_ANY_SEQUENCE() \
    {  }
#define STL_PARSER_DATABASE_ACTION____DROP_ANY_SEQUENCE()   \
    {  }
#define STL_PARSER_DATABASE_ACTION____ALTER_ANY_SEQUENCE()   \
    {  }
#define STL_PARSER_DATABASE_ACTION____USAGE_ANY_SEQUENCE()  \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_ANY_INDEX()    \
    {  }
#define STL_PARSER_DATABASE_ACTION____ALTER_ANY_INDEX() \
    {  }
#define STL_PARSER_DATABASE_ACTION____DROP_ANY_INDEX()  \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_ANY_SYNONYM()    \
    {  }
#define STL_PARSER_DATABASE_ACTION____DROP_ANY_SYNONYM()      \
    {  }
#define STL_PARSER_DATABASE_ACTION____CREATE_PUBLIC_SYNONYM() \
    {  }
#define STL_PARSER_DATABASE_ACTION____DROP_PUBLIC_SYNONYM()   \
    {  }

/**
 * tablespace_privilege
 */

#define STL_PARSER_TABLESPACE_PRIVILEGE____ACTION_LIST()       \
    {  }
#define STL_PARSER_TABLESPACE_PRIVILEGE____ALL()       \
    {  }
#define STL_PARSER_TABLESPACE_PRIVILEGE____ALL_PRIVILEGES()       \
    {  }
#define STL_PARSER_TABLESPACE_ACTION____CREATE_OBJECT() \
    {  }

/**
 * schema_privilege
 */

#define STL_PARSER_SCHEMA_PRIVILEGE____WITH_SCHEMA()    \
    {  }
#define STL_PARSER_SCHEMA_PRIVILEGE____WITHOUT_SCHEMA() \
    {  }
#define STL_PARSER_SCHEMA_PRIVILEGE____ALL() \
    {  }
#define STL_PARSER_SCHEMA_PRIVILEGE____ALL_PRIVILEGES() \
    {  }
#define STL_PARSER_SCHEMA_ACTION____CONTROL_SCHEMA()    \
    {  }
#define STL_PARSER_SCHEMA_ACTION____CREATE_TABLE()   \
    {  }
#define STL_PARSER_SCHEMA_ACTION____ALTER_TABLE()   \
    {  }
#define STL_PARSER_SCHEMA_ACTION____DROP_TABLE()    \
    {  }
#define STL_PARSER_SCHEMA_ACTION____SELECT_TABLE()  \
    {  }
#define STL_PARSER_SCHEMA_ACTION____INSERT_TABLE()  \
    {  }
#define STL_PARSER_SCHEMA_ACTION____DELETE_TABLE()  \
    {  }
#define STL_PARSER_SCHEMA_ACTION____UPDATE_TABLE()  \
    {  }
#define STL_PARSER_SCHEMA_ACTION____LOCK_TABLE()    \
    {  }
#define STL_PARSER_SCHEMA_ACTION____CREATE_VIEW()   \
    {  }
#define STL_PARSER_SCHEMA_ACTION____DROP_VIEW() \
    {  }
#define STL_PARSER_SCHEMA_ACTION____CREATE_SEQUENCE()   \
    {  }
#define STL_PARSER_SCHEMA_ACTION____ALTER_SEQUENCE()    \
    {  }
#define STL_PARSER_SCHEMA_ACTION____DROP_SEQUENCE() \
    {  }
#define STL_PARSER_SCHEMA_ACTION____USAGE_SEQUENCE()    \
    {  }
#define STL_PARSER_SCHEMA_ACTION____CREATE_INDEX()  \
    {  }
#define STL_PARSER_SCHEMA_ACTION____ALTER_INDEX()   \
    {  }
#define STL_PARSER_SCHEMA_ACTION____DROP_INDEX()    \
    {  }
#define STL_PARSER_SCHEMA_ACTION____ADD_CONSTRAINT()    \
    {  }
#define STL_PARSER_SCHEMA_ACTION____CREATE_SYNONYM()   \
    {  }
#define STL_PARSER_SCHEMA_ACTION____DROP_SYNONYM()     \
    {  }


/**
 * table_privilege
 */

#define STL_PARSER_TABLE_PRIVILEGE____ON_TABLE()    \
    {  }
#define STL_PARSER_TABLE_PRIVILEGE____ON()      \
    {  }
#define STL_PARSER_TABLE_PRIVILEGE____ALL_ON_TABLE() \
    {  }
#define STL_PARSER_TABLE_PRIVILEGE____ALL_PRIVILEGES_ON_TABLE() \
    {  }
#define STL_PARSER_TABLE_PRIVILEGE____ALL_ON() \
    {  }
#define STL_PARSER_TABLE_PRIVILEGE____ALL_PRIVILEGES_ON() \
    {  }
#define STL_PARSER_TABLE_ACTION____CONTROL_TABLE()  \
    {  }
#define STL_PARSER_TABLE_ACTION____SELECT()     \
    {  }
#define STL_PARSER_TABLE_ACTION____INSERT()     \
    {  }
#define STL_PARSER_TABLE_ACTION____UPDATE()     \
    {  }
#define STL_PARSER_TABLE_ACTION____DELETE()     \
    {  }
#define STL_PARSER_TABLE_ACTION____TRIGGER()    \
    {  }
#define STL_PARSER_TABLE_ACTION____REFERENCES() \
    {  }
#define STL_PARSER_TABLE_ACTION____LOCK()       \
    {  }
#define STL_PARSER_TABLE_ACTION____INDEX()      \
    {  }
#define STL_PARSER_TABLE_ACTION____ALTER()      \
    {  }

/**
 * column_privilege
 */

#define STL_PARSER_COLUMN_ACTION____SELECT()    \
    {  }
#define STL_PARSER_COLUMN_ACTION____INSERT()    \
    {  }
#define STL_PARSER_COLUMN_ACTION____UPDATE()    \
    {  }
#define STL_PARSER_COLUMN_ACTION____REFERENCES()    \
    {  }

/**
 * sequnce_privilege
 */

#define STL_PARSER_SEQUENCE_PRIVILEGE____ACTION_LIST()         \
    {  }
#define STL_PARSER_SEQUENCE_PRIVILEGE____ALL()         \
    {  }
#define STL_PARSER_SEQUENCE_PRIVILEGE____ALL_PRIVILEGES()         \
    {  }
#define STL_PARSER_USAGE_ACTION____SEQUENCE()      \
    {  }




/**
 * tablespace_definition
 */

#define STL_PARSER_TABLESPACE_DEFINITION( )     \
    { }

#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_DATA_DATAFILE( ) \
    { }

#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_DATA_DATAFILE_ATTR( )    \
    { }

#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_MEMORY( )    \
    { }

#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_MEMORY_ATTR( )    \
    { }

#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_UNDO_DATAFILE( ) \
    { }

#define STL_PARSER_TABLESPACE_DEFINITION____MEMORY_UNDO_DATAFILE_ATTR( )    \
    { }

#define STL_PARSER_TABLESPACE_DEFINITION____DISK_DATA_DATAFILES( )  \
    { }

#define STL_PARSER_TABLESPACE_DEFINITION____DISK_DATA_DATAFILES_ATTR( ) \
    { }

#define STL_PARSER_TABLESPACE_DEFINITION____DISK_TEMPORARY_DATAFILES( ) \
    { }                                                                 



/**
 * tablespace_clause
 */

#define STL_PARSER_TABLESPACE_CLAUSE____( )   \
    { }


/**
 * permanent_tablespace_options
 */

#define STL_PARSER_PERMANENT_TABLESPACE_OPTIONS____ATTR( )   \
    { }

#define STL_PARSER_PERMANENT_TABLESPACE_OPTIONS____ATTR_LIST( )     \
    { }


/**
 * permanent_tablespace_option
 */

#define STL_PARSER_PERMANENT_TABLESPACE_OPTION____LOGGING( )   \
    { }

#define STL_PARSER_PERMANENT_TABLESPACE_OPTION____NO_LOGGING( )   \
    { }

#define STL_PARSER_PERMANENT_TABLESPACE_OPTION____ONLINE( )     \
    { }

#define STL_PARSER_PERMANENT_TABLESPACE_OPTION____OFFLINE( )    \
    { }

#define STL_PARSER_PERMANENT_TABLESPACE_OPTION____EXTSIZE( )    \
    { }


/**
 * temporary_tablespace_options : qlpTablespaceAttr
 */

#define STL_PARSER_TEMPORARY_TABLESPACE_OPTIONS____ATTR( )  \
    { }

#define STL_PARSER_TEMPORARY_TABLESPACE_OPTIONS____ATTR_LIST( ) \
    { }


/**
 * temporary_tablespace_option : qlpList
 */

#define STL_PARSER_TEMPORARY_TABLESPACE_OPTION____EXTSIZE( ) \
    { }


/**
 * size_clause
 */

#define STL_PARSER_SIZE_CLAUSE____( )   \
    { }


/**
 * byte_unit
 */

#define STL_PARSER_BYTE_UNIT____EMPTY( )        \
    { }

#define STL_PARSER_BYTE_UNIT____BYTE( )         \
    { }

#define STL_PARSER_BYTE_UNIT____KILO( )         \
    { }

#define STL_PARSER_BYTE_UNIT____MEGA( )         \
    { }

#define STL_PARSER_BYTE_UNIT____GIGA( )         \
    { }

#define STL_PARSER_BYTE_UNIT____TERA( )         \
    { }


/**
 * memory_file_specifications
 */

#define STL_PARSER_MEMORY_FILE_SPECIFICATIONS____SPEC( )   \
    { }

#define STL_PARSER_MEMORY_FILE_SPECIFICATIONS____SPEC_LIST( )   \
    { }


/**
 * memory_file_specification
 */

#define STL_PARSER_MEMORY_FILE_SPECIFICATION____NAME_SIZE( )   \
    { }

#define STL_PARSER_MEMORY_FILE_SPECIFICATION____NAME_SIZE_REUSE( )   \
    { }

#define STL_PARSER_MEMORY_FILE_SPECIFICATION____NAME_REUSE( )   \
    { }


/**
 * disk_file_specifications
 */

#define STL_PARSER_DISK_FILE_SPECIFICATIONS____SPEC( )   \
    { }

#define STL_PARSER_DISK_FILE_SPECIFICATIONS____SPEC_LIST( )   \
    { }


/**
 * disk_file_specification
 */

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_SIZE( )   \
    { }

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_SIZE_REUSE( )   \
    { }

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_SIZE_REUSE_AUTOEXTEND( )   \
    { }

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_SIZE_AUTOEXTEND( )   \
    { }

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_REUSE( )   \
    { }

#define STL_PARSER_DISK_FILE_SPECIFICATION____NAME_REUSE_AUTOEXTEND( )   \
    { }


/**
 * autoextend_clause
 */

#define STL_PARSER_AUTOEXTEND_CLAUSE____ON( )   \
    { }

#define STL_PARSER_AUTOEXTEND_CLAUSE____OFF( )   \
    { }

#define STL_PARSER_AUTOEXTEND_CLAUSE____ON_NEXT( )   \
    { }

#define STL_PARSER_AUTOEXTEND_CLAUSE____ON_MAXSIZE( )   \
    { }

#define STL_PARSER_AUTOEXTEND_CLAUSE____ON_NEXT_MAXSIZE( )   \
    { }


/**
 * maxsize_clause
 */

#define STL_PARSER_MAXSIZE_CLAUSE____UNLIMITED( )   \
    { }

#define STL_PARSER_MAXSIZE_CLAUSE____SIZE( )   \
    { }

/**
 * alter database statement
 */

#define STL_PARSER_ALTER_DATABASE_STATEMENT____( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____LOGFILE( )      \
    { }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____BACKUP( )       \
    { }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____ARCHIVELOG( )   \
    { }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____REGISTER( )     \
    { }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____RECOVER( )      \
    { }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____RESTORE( )      \
    { }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____DELETE_BACKUP_LIST( )  \
    { }

#define STL_PARSE_ALTER_DATABASE_CLAUSE____CLEAR_PASSWORD_HISTORY() \
    { }

#define STL_PARSE_ALTER_DATABASE_DELETE_BACKUP_LIST_CLAUSE____( )  \
    { }

#define STL_PARSE_ALTER_DATABASE_CLEAR_PASSWORD_HISTORY_CLAUSE____() \
    { }

#define STL_PARSE_DELETE_BACKUP_LIST_OPTION____OBSOLETE( )  \
    { }

#define STL_PARSE_DELETE_BACKUP_LIST_OPTION____ALL( )  \
    { }

/**
 * including_backup_file
 */

#define STL_PARSE_INCLUDING_BACKUP_FILE_OPTION____EMPTY( ) \
    { }

#define STL_PARSE_INCLUDING_BACKUP_FILE_OPTION____INCLUDING_BACKUP_FILE( ) \
    { }

#define STL_PARSER_ALTER_DATABASE_DELETE_BACKUP_LIST_CLAUSE( )   \
    { }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____ADD_GROUP( )  \
    { }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____ADD_GROUP_REUSE( ) \
    { }

#define ALTER_DATABASE_LOG_FILE____RENAME_LOGFILE( )    \
    { }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____ADD_MEMBER( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____DROP_GROUP( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____DROP_MEMBER( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_LOGFILE_CLAUSE____RENAME( )  \
    { }

#define STL_PARSE_ALTER_DATABASE_ARCHIVELOG_CLAUSE____( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_NOARCHIVELOG_CLAUSE____( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_BACKUP_CLAUSE____( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_BACKUP_INCREMENTAL_CLAUSE____( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_BACKUP_CONTROLFILE_CLAUSE____( )   \
    { }

#define STL_PARSE_INCREMENTAL_BACKUP_OPTION____NONE( ) \
    { }

#define STL_PARSE_INCREMENTAL_BACKUP_OPTION____( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_REGISTER_CLAUSE____IRRECOVERABLE( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_RECOVER_CLAUSE____DATABASE( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_RECOVER_CLAUSE____TABLESPACE( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_RESTORE_CLAUSE____DATABASE( ) \
    { }

#define STL_PARSE_ALTER_DATABASE_RESTORE_CLAUSE____TABLESPACE( ) \
    { }

#define STL_PARSE_MEDIA_RECOVERY_OPTION____FULL()       \
    { }

#define STL_PARSE_MEDIA_RECOVERY_OPTION____INCOMPLETE_RECOVERY_INTERACTIVE()    \
    { }

#define STL_PARSE_MEDIA_RECOVERY_OPTION____INCOMPLETE_RECOVERY_CHANGE() \
    { }

#define STL_PARSE_MEDIA_RECOVERY_OPTION____INCOMPLETE_RECOVERY_TIME()   \
    { }

#define STL_PARSE_INCOMPLETE_RECOVERY_OPTION____BEGIN() \
    { }

#define STL_PARSE_INCOMPLETE_RECOVERY_OPTION____END() \
    { }

#define STL_PARSE_INCOMPLETE_RECOVERY_OPTION____MANUAL() \
    { }

#define STL_PARSE_INCOMPLETE_RECOVERY_OPTION____AUTO() \
    { }

#define STL_PARSE_INCOMPLETE_RECOVERY_OPTION____SUGGESTION() \
    { }

#define STL_PARSE_USING_CONTROLFILE_OPTION____USING_RECENT_CONTROLFILE() \
    { }

#define STL_PARSE_USING_CONTROLFILE_OPTION____USING_BACKUP_CONTROLFILE() \
    { }

#define STL_PARSE_ALTER_DATABASE_RESTORE_CONTROLFILE_CLAUSE____( )      \
    { }

#define STL_PARSE_RESTORE_UNTIL_OPTION____NONE() \
    { }

#define STL_PARSE_RESTORE_UNTIL_OPTION____CHANGE() \
    { }

#define STL_PARSE_RESTORE_UNTIL_OPTION____TIME() \
    { }

#define STL_PARSE_ALTER_DATABASE_RECOVER_CLAUSE____DATAFILE()   \
    { }

#define STL_PARSE_RECOVER_DATAFILE____DATAFILE()        \
    { }

#define STL_PARSE_RECOVER_DATAFILE____DATAFILE_LIST()   \
    { }

#define STL_PARSE_RECOVER_DATAFILE____TARGET()  \
    { }

#define STL_PARSE_RECOVER_CORRUPTION_OPTION____NONE()   \
    { }

#define STL_PARSE_RECOVER_CORRUPTION_OPTION____()       \
    { }

#define STL_PARSE_RECOVER_USING_OPTION____NONE()        \
    { }

#define STL_PARSE_RECOVER_USING_OPTION____()    \
    { }

#define STL_PARSER_LOG_FILE____STRING( ) \
    { }

#define STL_PARSER_LOG_FILE____STRING_LIST( ) \
    { }

#define STL_PARSER_LOG_FILE____NAME( ) \
    { }

#define STL_PARSER_LOG_FILE____NAME_REUSE( ) \
    { }

#define STL_PARSER_LOG_FILE_LIST____STRING( ) \
    { }

#define STL_PARSER_LOG_FILE_LIST____STRING_LIST( ) \
    { }

#define STL_PARSER_LOG_GROUP____NAME_SIZE( ) \
    { }

#define STL_PARSER_SEGMENT_ID_LIST____INTEGER( )    \
    { }

#define STL_PARSER_SEGMENT_ID_LIST____INTEGER_LIST( )    \
    { }


/**
 * alter tablespace statement
 */

#define STL_PARSE_ALTER_TABLESPACE( )  \
    { }

#define ALTER_TABLESPACE_ADD____ADD_MEMORY( )  \
    { }                                                                         

#define ALTER_TABLESPACE_ADD____ADD_DATAFILE( ) \
    { }                                                                         

#define ALTER_TABLESPACE_ADD____ADD_TEMPFILE( ) \
    { }                                                                         

#define ALTER_TABLESPACE_RENAME____( )          \
    { }

#define ALTER_TABLESPACE_BACKUP_CLAUSE____( ) \
    { }

#define ALTER_TABLESPACE_BACKUP_INCREMENTAL_CLAUSE____( ) \
    { }

#define ALTER_TABLESPACE_DROP____DROP_MEMORY( )  \
    { }                                                                         

#define ALTER_TABLESPACE_DROP____DROP_DATAFILE( ) \
    { }                                                                         

#define ALTER_TABLESPACE_DROP____DROP_TEMPFILE( ) \
    { }                                                                         

#define ALTER_TABLESPACE_ONOFF____ONLINE( )     \
    { }

#define ALTER_TABLESPACE_ONOFF____OFFLINE_EMPTY( )     \
    { }

#define ALTER_TABLESPACE_ONOFF____OFFLINE_NORMAL( )     \
    { }

#define ALTER_TABLESPACE_ONOFF____OFFLINE_IMMEDIATE( )     \
    { }

#define ALTER_TABLESPACE_RENAME____RENAME_MEMORY( ) \
    { }                                                                         

#define ALTER_TABLESPACE_RENAME____RENAME_DATAFILE( ) \
    { }                                                                         

#define ALTER_TABLESPACE_RENAME____RENAME_TEMPFILE( ) \
    { }                                                                         

#define STL_PARSER_RENAME_FILE_LIST____STRING( ) \
    { }                                                                         

#define STL_PARSER_RENAME_FILE_LIST____STRING_LIST( ) \
    { }                                                                         


/**
 * drop tablespace statement
 */

#define STL_PARSER_DROP_TABLESPACE_STATEMENT()   \
    { }

/**
 * including_contents_option
 */

#define STL_PARSER_INCLUDING_CONTENTS_OPTION____EMPTY()  \
    { }

#define STL_PARSER_INCLUDING_CONTENTS_OPTION____INCLUDING_CONTENTS()  \
    { }

/**
 * drop_datafile_option
 */

#define STL_PARSER_DROP_DATAFILE_OPTION____EMPTY()  \
    { }

#define STL_PARSER_DROP_DATAFILE_OPTION____AND_DATAFILES()  \
    { }

#define STL_PARSER_DROP_DATAFILE_OPTION____KEEP_DATAFILES()  \
    { }





/*
 * schema_definition:
 */

#define STL_PARSER_SCHEMA_DEFINITION_WITHOUT_DDL____SCHEMA_ONLY()  \
    {  }
#define STL_PARSER_SCHEMA_DEFINITION_WITHOUT_DDL____OWNER_ONLY()  \
    {  }
#define STL_PARSER_SCHEMA_DEFINITION_WITHOUT_DDL____SCHEMA_AND_OWNER()  \
    {  }
#define STL_PARSER_SCHEMA_DEFINITION_WITH_DDL____SCHEMA_ONLY()  \
    {  }
#define STL_PARSER_SCHEMA_DEFINITION_WITH_DDL____OWNER_ONLY()  \
    {  }
#define STL_PARSER_SCHEMA_DEFINITION_WITH_DDL____SCHEMA_AND_OWNER()  \
    {  }



/*
 * schema_element_list:
 */

#define STL_PARSER_SCHEMA_ELEMENT_LIST____DDL()  \
    {  }
#define STL_PARSER_SCHEMA_ELEMENT_LIST____DDL_LIST()  \
    {  }

/*
 *schema_element:  
 */

#define STL_PARSER_SCHEMA_ELEMENT____TABLE_DEFINITION() \
    {  }
#define STL_PARSER_SCHEMA_ELEMENT____VIEW_DEFINITION()  \
    {  }
#define STL_PARSER_SCHEMA_ELEMENT____INDEX_DEFINITION() \
    {  }
#define STL_PARSER_SCHEMA_ELEMENT____SEQUENCE_DEFINITION() \
    {  }
#define STL_PARSER_SCHEMA_ELEMENT____GRANT_PRIV_STATEMENT() \
    {  }
#define STL_PARSER_SCHEMA_ELEMENT____COMMENT_STATEMENT() \
    {  }


/*
 * drop_schema_statement:
 */

#define STL_PARSER_DROP_SCHEMA_STATEMENT() \
    { }


/**
 * table definition
 */

#define STL_PARSER_TABLE_DEFINITION____SCOPE_NAME_CONTENTS( )          \
    { }

#define STL_PARSER_TABLE_DEFINITION____SCOPE_NAME_CONTENTS_RELATION( ) \
    { }

#define STL_PARSER_TABLE_DEFINITION____SCOPE_NAME_COLUMNLIST_SUBQUERY() \
    { }

#define STL_PARSER_TABLE_DEFINITION____SCOPE_NAME_COLUMNLIST_RELATION_SUBQUERY() \
    { }


/**
 * as_subquery_clause option
 */

#define STL_PARSER_WITH_DATA_OPTION____NO_DATA() \
    { }

#define STL_PARSER_WITH_DATA_OPTION____DATA()    \
    { }

/**
 * relation_attribute_types
 */

#define STL_PARSER_RELATION_ATTRIBUTE_TYPES____LIST()   \
    { }

#define STL_PARSER_RELATION_ATTRIBUTE_TYPES____TABLESPACE() \
    { }

#define STL_PARSER_RELATION_ATTRIBUTE_TYPES____LIST_TABLESPACE()    \
    { }

#define STL_PARSER_RELATION_ATTRIBUTE_TYPES____COLUMNAR() \
    { }

#define STL_PARSER_RELATION_ATTRIBUTE_TYPES____COLUMNAR_TABLESPACE() \
    { }

/**
 * relation_attribute_list
 */

#define STL_PARSER_RELATION_ATTRIBUTE_LIST____ATTR()    \
    { }

#define STL_PARSER_RELATION_ATTRIBUTE_LIST____ATTR_LIST()   \
    { }

/**
 * relation_columnar_option
 */

#define STL_PARSER_RELATION_ATTRIBUTE_COLUMNAR____WITH_COLUMNAR_OPTIONS() \
    { }

#define STL_PARSER_RELATION_ATTRIBUTE_COLUMNAR____WITHOUT_COLUMNAR_OPTIONS() \
    { }

#define STL_PARSER_RELATION_ATTRIBUTE_COLUMNAR____WITH_COLUMNAR_OPTIONS_SEGMENT()       \
    { }

#define STL_PARSER_RELATION_ATTRIBUTE_COLUMNAR____WITHOUT_COLUMNAR_OPTIONS_SEGMENT()    \
    { }

/**
 * relation_attribute
 */

#define STL_PARSER_RELATION_ATTRIBUTE_TYPE____SEGMENT() \
    { }

#define STL_PARSER_RELATION_ATTRIBUTE_TYPE____PHYSICAL()    \
    { }


/**
 * tablespace_attribute_types
 */

#define STL_PARSER_TABLESPACE_ATTRIBUTE_TYPES____() \
    { }

/**
 * segment_attribute_types
 */

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPES____LIST()    \
    { }


/**
 * segment_attribute_type_list
 */

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE_LIST____ATTR()    \
    { }

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE_LIST____ATTR_LIST()   \
    { }


/**
 * segment_attribute_type
 */

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE____INITIAL()  \
    { }

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE____NEXT() \
    { }

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE____MINSIZE()   \
    { }

#define STL_PARSER_SEGMENT_ATTRIBUTE_TYPE____MAXSIZE()   \
    { }



/**
 * physical_attribute_type
 */

#define STL_PARSER_PHYSICAL_ATTRIBUTE_TYPE____PCTFREE( )        \
    { }

#define STL_PARSER_PHYSICAL_ATTRIBUTE_TYPE____PCTUSED( )        \
    { }

#define STL_PARSER_PHYSICAL_ATTRIBUTE_TYPE____INITRANS( )       \
    { }

#define STL_PARSER_PHYSICAL_ATTRIBUTE_TYPE____MAXTRANS( )       \
    { }


/**
 * table_scope
 */
#define STL_PARSER_TABLE_SCOPE____NON_TEMP( )   \
    { }

#define STL_PARSER_TABLE_SCOPE____GLOBAL_TEMP( )        \
    { }

#define STL_PARSER_TABLE_SCOPE____LOCAL_TEMP( ) \
    { }


/**
 * table_contents_source
 */

#define STL_PARSER_TABLE_CONTENTS_SOURCE____ELEMENTS( ) \
    { }


/**
 * table_element_list
 */

#define STL_PARSER_TABLE_ELEMENT_LIST____( )    \
    { }


/**
 * table_element_2
 */

#define STL_PARSER_TABLE_ELEMENT_2____ELEMENT( )        \
    { }

#define STL_PARSER_TABLE_ELEMENT_2____LIST( )    \
    { }


/**
 * table_element
 */

#define STL_PARSER_TABLE_ELEMENT____COLUMN_DEF( )       \
    { }

#define STL_PARSER_TABLE_ELEMENT____CONSTRAINT_DEF( )   \
    { }


/**
 * table_constraint_definition : qlpList
 */

#define STL_PARSER_TABLE_CONSTRAINT_DEFINITION____CONSTRAINT( )       \
    { }

/**
 * table_constraint_element
 */

#define STL_PARSER_TABLE_CONSTRAINT_ELEMENT____DEF( )        \
    { }    

#define STL_PARSER_TABLE_CONSTRAINT_ELEMENT____NAME_DEF( )   \
    { }    

#define STL_PARSER_TABLE_CONSTRAINT_ELEMENT____DEF_WITH_CONSTARINT_CHARACTERISTICS( )   \
    { }    

#define STL_PARSER_TABLE_CONSTRAINT_ELEMENT____NAME_DEF_WITH_CONSTARINT_CHARACTERISTICS( )  \
    { }    

/**
 * constraint_name_definition
 */

#define STL_PARSER_CONSTRAINT_NAME_DEFINITION____( )    \
    { }


/**
 * constraint_name
 */

#define STL_PARSER_CONSTRAINT_NAME____( )       \
    { }


/**
 * constraint_characteristics
 */

#define STL_PARSER_CONSTRAINT_CHARACTERISTIC____FIRST() \
    { }

#define STL_PARSER_CONSTRAINT_CHARACTERISTIC____NEXT()  \
    { }

#define STL_PARSER_CONSTRAINT_CHARACTERISTICS____CHECKTIME( )   \
    { }

#define STL_PARSER_CONSTRAINT_CHARACTERISTICS____DEFERRABLE( )  \
    { }

#define STL_PARSER_CONSTRAINT_CHARACTERISTICS____NOT_DEFERRABLE( )      \
    { }

/*
#define STL_PARSER_CONSTRAINT_CHARACTERISTICS____ENFORCEMENT( ) \
    { }
*/

/**
 * constraint_check_time
 */

#define STL_PARSER_CONSTRAINT_CHECK_TIME____INIT_DEFERRED( )    \
    { }

#define STL_PARSER_CONSTRAINT_CHECK_TIME____INIT_IMMEDIATE( )   \
    { }


/**
 * constraint_enforcement
 */

/*
#define STL_PARSER_CONSTRAINT_ENFORCEMENT____ENFORCED( )    \
    { }

#define STL_PARSER_CONSTRAINT_ENFORCEMENT____NOT_ENFORCED( )    \
    { }
*/

/**
 * table_constraint
 */

#define STL_PARSER_TABLE_CONSTRAINT____UNIQUE( )        \
    { }

#define STL_PARSER_TABLE_CONSTRAINT____FK( )    \
    { }

#define STL_PARSER_TABLE_CONSTRAINT____CHECK( ) \
    { }


/**
 * unique_constraint_definition
 */

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____UNIQUE( )        \
    { }

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____UNIQUE_INDEX_OPTION( )        \
    { }

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____UNIQUE_INDEX_ATTR_OPTION( )        \
    { }

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____PK( )    \
    { }

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____PK_INDEX_OPTION( )    \
    { }

#define STL_PARSER_UNIQUE_CONSTRAINT_DEFINITION____PK_INDEX_ATTR_OPTION( )    \
    { }


/**
 * index_tablespace_name
 */

#define STL_PARSER_INDEX_TABLESPACE_NAME____DEFAULT_TBS( )      \
    { }

#define STL_PARSER_INDEX_TABLESPACE_NAME____TBS_NAME( ) \
    { }


/**
 * key_column_list
 */

#define STL_PARSER_KEY_COLUMN_LIST____( )    \
    { }

/**
 * unique_column_list
 */

#define STL_PARSER_UNIQUE_COLUMN_LIST____( )    \
    { }


/**
 * referential_constraint_definition
 */

#define STL_PARSER_REFERENTIAL_CONSTRAINT_DEFINITION____( )     \
    { }

#define STL_PARSER_REFERENTIAL_CONSTRAINT_DEFINITION____INDEX_OPTION( )  \
    { }

#define STL_PARSER_REFERENTIAL_CONSTRAINT_DEFINITION____INDEX_ATTR_OPTION( )  \
    { }

/**
 * reference_column_list
 */

#define STL_PARSER_REFERENCE_COLUMN_LIST____( ) \
    { }


/**
 * column_definition
 */

#define STL_PARSER_COLUMN_DEFINITION____NAME_TYPE( )    \
    { }

#define STL_PARSER_COLUMN_DEFINITION____NAME_TYPE_DEFINITION( ) \
    { }

#define STL_PARSER_COLUMN_DEFINITION____NAME_TYPE_CONSTRAINT( ) \
    { }   

#define STL_PARSER_COLUMN_DEFINITION____NAME_TYPE_CONSTRAINT_DEFINITION( )      \
    { }   

#define STL_PARSER_COLUMN_DEFINITION____NAME_TYPE_DEFINITION_CONSTRAINT( )    \
    { }   


/**
 * date_type_or_domain_name
 */

#define STL_PARSER_DATE_TYPE_OR_DOMAIN_NAME____TYPE( )  \
    { }


/**
 * column_definition_second_clause
 */

#define STL_PARSER_COLUMN_DEFINITION_SECOND_CLAUSE____DEFAULT( )        \
    { }

#define STL_PARSER_COLUMN_DEFINITION_SECOND_CLAUSE____IDENTITY( )       \
    { }

#define STL_PARSER_COLUMN_DEFINITION_SECOND_CLAUSE____GENERATION( )     \
    { }


/**
 * column_constraint_definitions
 */

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITIONS____CONSTRAINT( )        \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITIONS____CONSTRAINT_LIST( )        \
    { }


/**
 * column_constraint_definition
 */

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITION____CONSTRAINT( )        \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITION____NAME_CONSTRAINT( )   \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITION____CONSTRAINT_WITH_CONSTARINT_CHARACTERISTICS( )    \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT_DEFINITION____NAME_CONSTRAINT_WITH_CONSTARINT_CHARACTERISTICS( ) \
    { }

/**
 * column_constraint
 */

#define STL_PARSER_COLUMN_CONSTRAINT____NULL( )     \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT____NOT_NULL( )     \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT____PRIMARY_KEY( )        \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT____PRIMARY_KEY_INDEX_OPTION( )        \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT____PRIMARY_KEY_INDEX_ATTR_OPTION( )        \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT____UNIQUE( )        \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT____UNIQUE_INDEX_OPTION( )        \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT____UNIQUE_INDEX_ATTR_OPTION( )        \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT____FK( )   \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT____FK_INDEX( )   \
    { }

#define STL_PARSER_COLUMN_CONSTRAINT____CHECK( )        \
    { }


/**
 * references_specification
 */

#define STL_PARSER_REFERENCES_SPECIFICATION____TABLE_COLUMNS( ) \
    { }


/**
 * referenced_columns
 */

#define STL_PARSER_REFERENCED_COLUMNS____EMPTY( )       \
    { }

#define STL_PARSER_REFERENCED_COLUMNS____COLUMNS( )     \
    { }


/**
 * match_type
 */

#define STL_PARSER_MATCH_TYPE____FULL( )        \
    { }

#define STL_PARSER_MATCH_TYPE____PARTIAL( )     \
    { }

#define STL_PARSER_MATCH_TYPE____SIMPLE( )      \
    { }


/**
 * referential_triggered_action
 */

#define STL_PARSER_REFERENTIAL_TRIGGERED_ACTION____UPDATE( )    \
    { }

#define STL_PARSER_REFERENTIAL_TRIGGERED_ACTION____UPDATE_DELETE( )     \
    { }

#define STL_PARSER_REFERENTIAL_TRIGGERED_ACTION____DELETE( )    \
    { }

#define STL_PARSER_REFERENTIAL_TRIGGERED_ACTION____DELETE_UPDATE( )     \
    { }


/**
 * update_rule
 */

#define STL_PARSER_UPDATE_RULE____( )           \
    { }


/**
 * delete_rule
 */

#define STL_PARSER_DELETE_RULE____( )           \
    { }


/**
 * referential_action
 */

#define STL_PARSER_REFERENTIAL_ACTION____CASCADE( )     \
    { }

#define STL_PARSER_REFERENTIAL_ACTION____SET_NULL( )    \
    { }

#define STL_PARSER_REFERENTIAL_ACTION____SET_DEFAULT( ) \
    { }

#define STL_PARSER_REFERENTIAL_ACTION____RESTRICT( )    \
    { }

#define STL_PARSER_REFERENTIAL_ACTION____NO_ACTION( )   \
    { }


/**
 * check_constraint_definition
 */

#define STL_PARSER_CHECK_CONSTRAINT_DEFINITION____( )   \
    { }


/**
 * schema_qualified_name
 */

#define STL_PARSER_SCHEMA_QUALIFIED_NAME____( ) \
    { }


/**
 * default_clause
 */

#define STL_PARSER_DEFAULT_CLAUSE____( )        \
    { }


/**
 * default_option
 */

#define STL_PARSER_DEFAULT_OPTION____VALUE_EXPR( )      \
    { }


/**
 * identity_column_specification
 */

#define STL_PARSER_IDENTITY_COLUMN_SPECIFICATION____ALWAYS( )   \
    { }

#define STL_PARSER_IDENTITY_COLUMN_SPECIFICATION____BY_DEFAULT( )       \
    { }

#define STL_PARSER_IDENTITY_COLUMN_SPECIFICATION____ALWAYS_SEQUENCE( )  \
    { }

#define STL_PARSER_IDENTITY_COLUMN_SPECIFICATION____BY_DEFAULT_SEQUENCE( )      \
    { }


/**
 * generation_clause
 */

#define STL_PARSER_GENERATION_CLAUSE____( )     \
    { }


/**
 * common_sequence_generator_options
 */

#define STL_PARSER_COMMON_SEQUENCE_GENERATOR_OPTIONS____OPTION( )       \
    { }

#define STL_PARSER_COMMON_SEQUENCE_GENERATOR_OPTIONS____OPTION_LIST( )   \
    { }


/**
 * common_sequence_generator_option
 */

#define STL_PARSER_COMMON_SEQUENCE_GENERATOR_OPTION____START_WITH( )    \
    { }

#define STL_PARSER_COMMON_SEQUENCE_GENERATOR_OPTION____SEQUENCE( )      \
    { }


/**
 * sequence_generator_start_with_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_START_WITH_OPTION____( )  \
    { }


/**
 * sequence_generator_start_value
 */

#define STL_PARSER_SEQUENCE_GENERATOR_START_VALUE____( )        \
    { }


/**
 * basic_sequence_generator_option
 */

#define STL_PARSER_BASIC_SEQUENCE_GENERATOR_OPTION____INCREMENT( )      \
    { }

#define STL_PARSER_BASIC_SEQUENCE_GENERATOR_OPTION____MAXVALUE( )       \
    { }

#define STL_PARSER_BASIC_SEQUENCE_GENERATOR_OPTION____MINVALUE( )       \
    { }

#define STL_PARSER_BASIC_SEQUENCE_GENERATOR_OPTION____CYCLE( )  \
    { }

#define STL_PARSER_BASIC_SEQUENCE_GENERATOR_OPTION____CACHE( )  \
    { }


/**
 * sequence_generator_increment_by_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_INCREMENT_BY_OPTION____( )        \
    { }

#define STL_PARSER_SEQUENCE_GENERATOR_INCREMENT____( )  \
    { }


/**
 * sequence_generator_maxvalue_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_MAXVALUE_OPTION____MAXVALUE( )    \
    { }
            
#define STL_PARSER_SEQUENCE_GENERATOR_MAXVALUE_OPTION____NO_MAXVALUE( ) \
    { }

#define STL_PARSER_SEQUENCE_GENERATOR_MAXVALUE_OPTION____NOMAXVALUE( ) \
    { }


/**
 * sequence_generator_max_value
 */

#define STL_PARSER_SEQUENCE_GENERATOR_MAX_VALUE____( )  \
    { }


/**
 * sequence_generator_minvalue_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_MINVALUE_OPTION____MINVALUE( )    \
    { }

#define STL_PARSER_SEQUENCE_GENERATOR_MINVALUE_OPTION____NO_MINVALUE( ) \
    { }

#define STL_PARSER_SEQUENCE_GENERATOR_MINVALUE_OPTION____NOMINVALUE( ) \
    { }


/**
 * sequence_generator_min_value
 */

#define STL_PARSER_SEQUENCE_GENERATOR_MIN_VALUE____( )  \
    { }


/**
 * sequence_generator_cycle_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_CYCLE_OPTION____CYCLE( )  \
    { }

#define STL_PARSER_SEQUENCE_GENERATOR_CYCLE_OPTION____NO_CYCLE( )       \
    { }

#define STL_PARSER_SEQUENCE_GENERATOR_CYCLE_OPTION____NOCYCLE( )        \
    { }


/**
 * sequence_generator_cache_option
 */

#define STL_PARSER_SEQUENCE_GENERATOR_CACHE_OPTION____CACHE( )  \
    { }

#define STL_PARSER_SEQUENCE_GENERATOR_CACHE_OPTION____NO_CACHE( )       \
    { }

#define STL_PARSER_SEQUENCE_GENERATOR_CACHE_OPTION____NOCACHE( )        \
    { }


/**
 * sequence_generator_cache_value
 */

#define STL_PARSER_SEQUENCE_GENERATOR_CACHE_VALUE____( )        \
    { }


/**
 * truncate_table_statement
 */

#define STL_PARSER_TRUNCATE_TABLE_STATEMENT()       \
    { }


/**
 * identity_column_restart_option
 */

#define STL_PARSER_IDENTITY_COLUMN_RESTART_OPTION____EMPTY()    \
    { }

#define STL_PARSER_IDENTITY_COLUMN_RESTART_OPTION____CONTINUE() \
    { }

#define STL_PARSER_IDENTITY_COLUMN_RESTART_OPTION____RESTART()  \
    { }


/*
 * drop_storage_option
 */

#define STL_PARSER_DROP_STORAGE_OPTION____EMPTY()   \
    { }

#define STL_PARSER_DROP_STORAGE_OPTION____DROP()  \
    { }

#define STL_PARSER_DROP_STORAGE_OPTION____DROP_ALL()  \
    { }

#define STL_PARSER_DROP_STORAGE_OPTION____REUSE()  \
    { }


/**
 * VIEW DEFINITION
 */

#define STL_PARSER_VIEW_DEFINITION____BASIC() \
    {  }

#define STL_PARSER_OR_REPLACE_OPTION____NO_REPLACE() \
    {  }

#define STL_PARSER_OR_REPLACE_OPTION____REPLACE() \
    {  }

#define STL_PARSER_FORCE_OPTION____NO_FORCE() \
    {  }

#define STL_PARSER_FORCE_OPTION____FORCE() \
    {  }

#define STL_PARSER_VIEW_COLUMN_LIST____EMPTY() \
    {  }

#define STL_PARSER_VIEW_COLUMN_LIST____NOT_EMPTY() \
    {  }

/**
 * DROP VIEW
 */

#define STL_PARSER_DROP_VIEW_STATEMENT() \
    {  }

/**
 * ALTER VIEW
 */

#define STL_PARSER_ALTER_VIEW_STATEMENT() \
    {  }

#define STL_PARSER_ALTER_VIEW_ACTION____COMPILE() \
    {  }


/**
 * sequence_definition
 */

#define STL_PARSER_SEQUENCE_DEFINITION____NO_OPTION()    \
    { }

#define STL_PARSER_SEQUENCE_DEFINITION____WITH_OPTION()    \
    { }


/**
 * drop_sequence_statement
 */

#define STL_PARSER_DROP_SEQUENCE_STATEMENT()  \
    { }

/**
 * alter_sequence_statement
 */

#define STL_PARSER_ALTER_SEQUENCE_STATEMENT() \
    { }

#define STL_PARSER_ALTER_SEQUENCE_GENERATOR_OPTIONS____OPTION() \
    { }
#define STL_PARSER_ALTER_SEQUENCE_GENERATOR_OPTIONS____OPTION_LIST() \
    { }

#define STL_PARSER_ALTER_SEQUENCE_GENERATOR_OPTION____RESTART() \
    { }
#define STL_PARSER_ALTER_SEQUENCE_GENERATOR_OPTION____BASIC() \
    { }

#define STL_PARSER_ALTER_SEQUENCE_RESTART____RESTART_NO_VALUE() \
    { }
#define STL_PARSER_ALTER_SEQUENCE_RESTART____RESTRAT_WITH_VALUE() \
    { }

/**
 * synonym_definition
 */

#define STL_PARSER_SYNONYM_DEFINITION____PRIVATE()    \
    { }

#define STL_PARSER_SYNONYM_DEFINITION____PUBLIC()     \
    { }


/**
 * drop_synonym_statement
 */

#define STL_PARSER_DROP_SYNONYM_STATEMENT____PRIVATE()  \
    { }
#define STL_PARSER_DROP_SYNONYM_STATEMENT____PUBLIC()  \
    { }

/**
 * index_definition
 */

#define STL_PARSER_INDEX_DEFINITION____( ) \
    { }

#define STL_PARSER_INDEX_DEFINITION____ATTR_TBS_NAME( )  \
    { }

/**
 * index_attribute_list
 */

#define STL_PARSER_INDEX_ATTRIBUTE_LIST____ATTR()   \
    { }

#define STL_PARSER_INDEX_ATTRIBUTE_LIST____ATTR_LIST()  \
    { }


/**
 * index_attribute_types
 */

#define STL_PARSER_INDEX_ATTRIBUTE_TYPES____SEGMENT( )  \
    { }

#define STL_PARSER_INDEX_ATTRIBUTE_TYPES____PHYSICAL( ) \
    { }

#define STL_PARSER_INDEX_ATTRIBUTE_TYPES____LOGGING( ) \
    { }

#define STL_PARSER_INDEX_ATTRIBUTE_TYPES____PARALLEL( ) \
    { }


/**
 * index_physical_attribute_type
 */

#define STL_PARSER_INDEX_PHYSICAL_ATTRIBUTE_TYPE____INITRANS( ) \
    { }

#define STL_PARSER_INDEX_PHYSICAL_ATTRIBUTE_TYPE____MAXTRANS( ) \
    { }

#define STL_PARSER_INDEX_PHYSICAL_ATTRIBUTE_TYPE____PCTFREE( ) \
    { }

/**
 * index_logging_attribute_type:
 */

#define STL_PARSER_INDEX_LOGGING_ATTRIBUTE_TYPE____LOGGING( ) \
    { }

#define STL_PARSER_INDEX_LOGGING_ATTRIBUTE_TYPE____NOLOGGING( ) \
    { }


/**
 * index_parallel_attribute_type
 */

#define STL_PARSER_INDEX_PARALLEL_ATTRIBUTE_TYPE____NOPARALLEL( ) \
    { }

#define STL_PARSER_INDEX_PARALLEL_ATTRIBUTE_TYPE____PARALLEL_EMPTY( ) \
    { }

#define STL_PARSER_INDEX_PARALLEL_ATTRIBUTE_TYPE____PARALLEL_INTEGER( ) \
    { }


/**
 * index_unique_clause
 */

#define STL_PARSER_INDEX_UNIQUE_CLAUSE____NOT_UNIQUE( ) \
    { }

#define STL_PARSER_INDEX_UNIQUE_CLAUSE____UNIQUE( ) \
    { }


/**
 * index_column_list
 */

#define STL_PARSER_INDEX_COLUMN_LIST____COLUMN( )       \
    { }

#define STL_PARSER_INDEX_COLUMN_LIST____COLUMN_LIST( )  \
    { }


/**
 * index_column_clause
 */

#define STL_PARSER_INDEX_COLUMN_CLAUSE____NAME( )       \
    { }

#define STL_PARSER_INDEX_COLUMN_CLAUSE____NAME_SORT( )  \
    { }

#define STL_PARSER_INDEX_COLUMN_CLAUSE____NAME_NULLS( ) \
    { }

#define STL_PARSER_INDEX_COLUMN_CLAUSE____NAME_SORT_NULLS( )    \
    { }


/**
 * sort_order_clause
 */

#define STL_PARSER_SORT_ORDER_CLAUSE____ASC( )  \
    { }

#define STL_PARSER_SORT_ORDER_CLAUSE____DESC( ) \
    { }


/**
 * nulls_order_clause
 */

#define STL_PARSER_NULLS_ORDER_CLAUSE____FIRST( )       \
    { }

#define STL_PARSER_NULLS_ORDER_CLAUSE____LAST( )        \
    { }


/**
 * drop_if_exists_option
 */

#define STL_PARSER_DROP_IF_EXISTS____EMPTY() \
    { }
    
#define STL_PARSER_DROP_IF_EXISTS____EXISTS() \
    { }                                       


/**
 * drop_index_statement
 */

#define STL_PARSER_DROP_INDEX_STATEMENT()  \
    { }



/**
 * drop_table_statement
 */

#define STL_PARSER_DROP_TABLE_STATEMENT( )   \
    { }



/**
 * drop_behavior
 */

#define STL_PARSER_DROP_BEHAVIOR____EMPTY() \
    { }

#define STL_PARSER_DROP_BEHAVIOR____CASCADE() \
    { }


/**
 * ALTER TABLE STATEMENT 
 */

#define STL_PARSER_ALTER_TABLE____ADD_COLUMN()  \
    { }
#define STL_PARSER_ALTER_TABLE____DROP_COLUMN() \
    { }
#define STL_PARSER_ALTER_TABLE____ALTER_COLUMN() \
    { }
#define STL_PARSER_ALTER_TABLE____RENAME_COLUMN() \
    { }
#define STL_PARSER_ALTER_TABLE____ADD_CONSTRAINT() \
    { }
#define STL_PARSER_ALTER_TABLE____DROP_CONSTRAINT() \
    { }
#define STL_PARSER_ALTER_TABLE____ALTER_CONSTRAINT() \
    { }
/*
#define STL_PARSER_ALTER_TABLE____RENAME_CONSTRAINT() \
    { }
*/
#define STL_PARSER_ALTER_TABLE____ALTER_PHYSICAL() \
    { }
#define STL_PARSER_ALTER_TABLE____ADD_SUPPLEMENTAL_LOG() \
    { }
#define STL_PARSER_ALTER_TABLE____DROP_SUPPLEMENTAL_LOG() \
    { }
#define STL_PARSER_ALTER_TABLE____RENAME_TABLE() \
    { }



/*
 * alter_table_add_column_definition
 */

#define STL_PARSER_ADD_ONE_COLUMN____WITH_COLUMN() \
    { }
#define STL_PARSER_ADD_ONE_COLUMN____WITHOUT_COLUMN() \
    { }
#define STL_PARSER_ADD_MULTI_COLUMN____WITH_COLUMN() \
    { }
#define STL_PARSER_ADD_MULTI_COLUMN____WITHOUT_COLUMN() \
    { }

/*
 * add_column_definition_list
 */
#define STL_PARSER_ADD_COLUMN_DEFINITION____ELEMENT() \
    { }
#define STL_PARSER_ADD_COLUMN_DEFINITION____LIST() \
    { }


/*
 * alter_table_drop_column_definition
 */

#define STL_PARSER_SET_UNUSED_ONE_COLUMN____WITH_COLUMN() \
    {  }

#define STL_PARSER_SET_UNUSED_ONE_COLUMN____WITHOUT_COLUMN() \
    {  }

#define STL_PARSER_SET_UNUSED_MULTI_COLUMN____WITH_COLUMN() \
    {  }

#define STL_PARSER_SET_UNUSED_MULTI_COLUMN____WITHOUT_COLUMN() \
    {  }

#define STL_PARSER_DROP_ONE_COLUMN____WITH_COLUMN()  \
    {  }

#define STL_PARSER_DROP_ONE_COLUMN____WITHOUT_COLUMN()  \
    {  }

#define STL_PARSER_DROP_MULTI_COLUMN____WITH_COLUMN()  \
    {  }

#define STL_PARSER_DROP_MULTI_COLUMN____WITHOUT_COLUMN()  \
    {  }

#define STL_PARSER_DROP_UNUSED_COLUMNS____()  \
    {  }

/**
 * alter_table_alter_column_definition
 */

#define STL_PARSER_ALTER_COLUMN_SET_DEFAULT_CLAUSE____WITH_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_SET_DEFAULT_CLAUSE____WITHOUT_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_DROP_DEFAULT_CLAUSE____WITH_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_DROP_DEFAULT_CLAUSE____WITHOUT_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_SET_NOT_NULL_CLAUSE____WITH_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_SET_NOT_NULL_CLAUSE____WITHOUT_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_SET_CONSTRAINT_NOT_NULL_CLAUSE____WITH_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_SET_CONSTRAINT_NOT_NULL_CLAUSE____WITHOUT_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_SET_NOT_NULL_CLAUSE____WITH_COLUMN_WITH_CONST_ATTR()   \
    {  }
#define STL_PARSER_ALTER_COLUMN_SET_NOT_NULL_CLAUSE____WITHOUT_COLUMN_WITH_CONST_ATTR()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_SET_CONSTRAINT_NOT_NULL_CLAUSE____WITH_COLUMN_WITH_CONST_ATTR()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_SET_CONSTRAINT_NOT_NULL_CLAUSE____WITHOUT_COLUMN_WITH_CONST_ATTR()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_DROP_NOT_NULL_CLAUSE____WITH_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_DROP_NOT_NULL_CLAUSE____WITHOUT_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_ALTER_DATA_TYPE_CLAUSE____WITH_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_ALTER_DATA_TYPE_CLAUSE____WITHOUT_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_ALTER_IDENTITY_SPEC____WITH_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_ALTER_IDENTITY_SPEC____WITHOUT_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_DROP_IDENTITY____WITH_COLUMN()  \
    {  }
#define STL_PARSER_ALTER_COLUMN_DROP_IDENTITY____WITHOUT_COLUMN()  \
    {  }

/*
 * alter_identity_column_specification:
 */

#define STL_PARSER_ALTER_IDENTITY____GENERATION_ALWAYS()  \
   {  }
#define STL_PARSER_ALTER_IDENTITY____GENERATION_DEFAULT()  \
   {  }
#define STL_PARSER_ALTER_IDENTITY____GENERATION_ALWAYS_AND_OPTIONS()  \
   {  }
#define STL_PARSER_ALTER_IDENTITY____GENERATION_DEFAULT_AND_OPTIONS()  \
   {  }
#define STL_PARSER_ALTER_IDENTITY____OPTIONS()  \
   {  }

/*
 * alter_identity_column_options
 */

#define STL_PARSER_ALTER_IDENTITY_COLUMN_OPTIONS____OPTION() \
   {  }
#define STL_PARSER_ALTER_IDENTITY_COLUMN_OPTIONS____OPTION_LIST() \
   {  }

/*
 * alter_identity_column_option
 */

#define STL_PARSER_ALTER_IDENTITY_COLUMN_OPTION____RESTART() \
   {  }
#define STL_PARSER_ALTER_IDENTITY_COLUMN_OPTION____BASIC_OPTION_WITH_SET() \
   {  }
#define STL_PARSER_ALTER_IDENTITY_COLUMN_OPTION____BASIC_OPTION() \
   {  }

/*
 * alter_table_physical_attribute_clause
 */

#define STL_PARSER_ALTER_TABLE_PHYSICAL_ATTR_CLAUSE()   \
    { }

/**
 * alter_table_rename_column_clause
 */

#define STL_PARSER_RENAME_COLUMN_CLAUSE() \
    { }

/**
 * alter_table_add_constraint_definition
 */

#define STL_PARSER_ADD_CONSTRAINT_CLAUSE()  \
    {  }

/**
 * alter_table_drop_constraint_definition
 */

#define STL_PARSER_DROP_CONSTRAINT_CLAUSE()  \
    {  }

/**
 * alter_table_alter_constraint_definition
 */

#define STL_PARSER_ALTER_CONSTRAINT_CLAUSE() \
    {  }


/**
 * drop_constraint_object
 */

#define STL_PARSER_DROP_CONSTRAINT_OBJECT____CONSTRAINT_NAME() \
    {  }

#define STL_PARSER_DROP_CONSTRAINT_OBJECT____PRIMARY_KEY() \
    {  }

#define STL_PARSER_DROP_CONSTRAINT_OBJECT____UNIQUE() \
    {  }
    

/**
 * drop_key_index_option
 */

#define STL_PARSER_DROP_KEY_INDEX_OPTION____EMPTY() \
    {  }

#define STL_PARSER_DROP_KEY_INDEX_OPTION____KEEP_INDEX() \
    {  }

#define STL_PARSER_DROP_KEY_INDEX_OPTION____DROP_INDEX() \
    {  }

/*
 * alter_table_add_supplemental_log_clause
 */

#define STL_PARSER_ADD_TABLE_SUPPLEMENTAL_LOG_CLAUSE() \
    { }

/*
 * alter_table_drop_supplemental_log_clause
 */

#define STL_PARSER_DROP_TABLE_SUPPLEMENTAL_LOG_CLAUSE() \
   { }

/**
 * alter_table_rename_table_clause
 */

#define STL_PARSER_RENAME_TABLE_CLAUSE() \
    { }


/**
 * alter_index_statement
 */

#define STL_PARSER_ALTER_INDEX____ALTER_PHYSICAL()  \
    { }

/**
 * alter_index_physical_attribute_clause
 */

#define STL_PARSER_INDEX_PHYSICAL_ATTR_CLAUSE()  \
    { }


/**
 * insert_statement
 */

#define STL_PARSER_INSERT_STATEMENT____( )                                      \
    { }


/**
 * insert_columns_and_source
 */

#define STL_PARSER_INSERT_COLUMNS_AND_SOURCE____CONSTRUCTOR( )  \
    { }

#define STL_PARSER_INSERT_COLUMNS_AND_SOURCE____DEFAULT( )      \
    { }

#define STL_PARSER_INSERT_COLUMNS_AND_SOURCE____SUBQUERY( )      \
    { }


/**
 * from_constructor
 */

#define STL_PARSER_FROM_CONSTRUCTOR____ALL_COLUMNS( )    \
    { }

#define STL_PARSER_FROM_CONSTRUCTOR____COLUMNS( )       \
    { }


/**
 * column_name_list
 */

#define STL_PARSER_COLUMN_NAME_LIST____COLUMN( )        \
    { }

#define STL_PARSER_COLUMN_NAME_LIST____COLUMN_LIST( )    \
    { }


/**
 * contextually_typed_table_value_constructor
 */

#define STL_PARSER_CONTEXTUALLY_TYPED_TABLE_VALUE_CONSTRUCTOR____( )    \
    { }


/**
 * contextually_typed_row_value_expression_list
 */

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_EXPRESSION_LIST____EXPR( )      \
    { }

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_EXPRESSION_LIST____EXPR_LIST( ) \
    { }


/**
 * contextually_typed_row_value_expression
 */

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_EXPRESSION____ONE_ROW_EXPR( )   \
    { }


/**
 * contextually_typed_row_value_constructor_element_2
 */

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_CONSTRUCTOR_ELEMENT_2____ROW_VALUE( )   \
    { }

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_CONSTRUCTOR_ELEMENT_2____ROW_VALUE_LIST( )      \
    { }


/**
 * contextually_typed_row_value_constructor_element
 */

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_CONSTRUCTOR_ELEMENT____VALUE_EXPR( )    \
    { }

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_CONSTRUCTOR_ELEMENT____TYPED_VALUE( )   \
    { }


/**
 * from_default
 */

#define STL_PARSER_FROM_DEFAULT____( )          \
    { }


/**
 * from_subquery : qlpInsertSource
 */

#define STL_PARSER_FROM_SUBQUERY____QUERY_EXPR_ALL_COLUMNS()    \
    { }

#define STL_PARSER_FROM_SUBQUERY____QUERY_EXPR_COLUMNS()        \
    { }                                                         

/**
 * update_statement
 */

#define STL_PARSER_UPDATE_STATEMENT____NO_WHERE( )                              \
    { }

#define STL_PARSER_UPDATE_STATEMENT____WHERE( )                                 \
    { }

/**
 * update_statement: positioned
 */

#define STL_PARSER_UPDATE_STATEMENT_POSITIONED____CURSOR( )  \
    { }

/**
 * set_clause_list
 */

#define STL_PARSER_SET_CLAUSE_LIST____SET_CLAUSE( )     \
    { }

#define STL_PARSER_SET_CLAUSE_LIST____SET_CLAUSE_LIST( )        \
    { }


/**
 * set_clause
 */

#define STL_PARSER_SET_CLAUSE____MULTIPLE_COLUMN( )     \
    { }

#define STL_PARSER_SET_CLAUSE____ASSIGN( )      \
    { }


/**
 * set_target
 */

#define STL_PARSER_SET_TARGET____UPDATE_TARGET( )       \
    { }

#define STL_PARSER_SET_TARGET____MUTATED_SET( ) \
    { }


/**
 * multiple_column_assignment
 */

#define STL_PARSER_MULTIPLE_COLUMN_ASSIGNMENT____( )    \
    { }


/**
 * set_target_list
 */

#define STL_PARSER_SET_TARGET_LIST____( )       \
    { }


/**
 * set_target_list_2
 */

#define STL_PARSER_SET_TARGET_LIST_2____TARGET( )       \
    { }

#define STL_PARSER_SET_TARGET_LIST_2____TARGET_LIST( )  \
    { }


/**
 * assigned_row
 */

#define STL_PARSER_ASSIGNED_ROW____( )          \
    { }

#define STL_PARSER_ASSIGNED_ROW____ONE_SUBQUERY( )                              \
    { }


/**
 * update_target
 */

#define STL_PARSER_UPDATE_TARGET____NAME( )     \
    { }

#define STL_PARSER_UPDATE_TARGET____PAREN_NAME( )       \
    { }


/**
 * mutated_set_clause
 */

#define STL_PARSER_MUTATED_SET_CLAUSE____( )    \
    { }


/**
 * mutated_target
 */

#define STL_PARSER_MUTATED_TARGET____COLUMN( )  \
    { }

#define STL_PARSER_MUTATED_TARGET____MUTATED_SET( )     \
    { }


/**
 * update_source
 */

#define STL_PARSER_UPDATE_SOURCE____VALUE_EXPR( )       \
    { }

#define STL_PARSER_UPDATE_SOURCE____TYPED_VALUE( )      \
    { }

/**
 * contextually_typed_value_specification
 */

#define STL_PARSER_CONTEXTUALLY_TYPED_VALUE_SPECIFICATION____IMPLICITLY_TYPED_VALUE( )  \
    { }

#define STL_PARSER_CONTEXTUALLY_TYPED_VALUE_SPECIFICATION____DEFAULT( ) \
    { }


/**
 * simple_value_specification
 */

#define STL_PARSER_SIMPLE_VALUE_SPECIFICATION____LITERAL( )     \
    { }

#define STL_PARSER_SIMPLE_VALUE_SPECIFICATION____HOST_PARAMETER( )  \
    {                                                               \
        STL_PARSER_CHECK( v0 = ztqAddInHostVariable( param,         \
                                                     v1,            \
                                                     NULL ) );      \
    }

#define STL_PARSER_SIMPLE_VALUE_SPECIFICATION____DYNAMIC_PARAMETER( )   \
    {                                                                   \
        STL_PARSER_ERROR( NULL, param, param->mScanner, NULL );         \
        YYABORT;                                                        \
    }

#define STL_PARSER_SIMPLE_VALUE_SPECIFICATION____SQL_PARAMETER( )       \
    { }


/**
 * literal
 */

#define STL_PARSER_LITERAL____SIGNED_NUMERIC( ) \
    { }

#define STL_PARSER_LITERAL____GENERAL( )        \
    { }


/**
 * signed_numeric_literal
 */

#define STL_PARSER_SIGNED_NUMERIC_LITERAL____UNSIGNED_NUMERIC( )    \
    { }

#define STL_PARSER_SIGNED_NUMERIC_LITERAL____SIGNED_NUMERIC( ) \
    { }

/**
 * host_variable
 */

#define STL_PARSER_HOST_VARIABLE____( )                         \
    {                                                           \
        STL_PARSER_CHECK( v0 = ztqMakeHostVariable( param ) );  \
    }


/**
 * delete_statement
 */

#define STL_PARSER_DELETE_STATEMENT____NO_WHERE( )      \
    { }

#define STL_PARSER_DELETE_STATEMENT____WHERE( ) \
    { }

#define STL_PARSER_DELETE_STATEMENT____NO_FROM_NO_WHERE( )      \
    { }

#define STL_PARSER_DELETE_STATEMENT____NO_FROM_WHERE( ) \
    { }


/**
 * delete_statement: positioned
 */

#define STL_PARSER_DELETE_STATEMENT_POSITIONED____CURSOR( )  \
    { }

#define STL_PARSER_DELETE_STATEMENT_POSITIONED____NO_FROM_CURSOR( )     \
    { }


/**
 * insert or delete returning
 */


#define STL_PARSER_INSERT_OR_DELETE_RETURNING_CLAUSE____EMPTY( )  \
    { }

#define STL_PARSER_INSERT_OR_DELETE_RETURNING_CLAUSE____RETURNING( ) \
    { }


/**
 * update_returning_clause
 */

#define STL_PARSER_UPDATE_RETURNING_CLAUSE____EMPTY( )  \
    { }

#define STL_PARSER_UPDATE_RETURNING_CLAUSE____RETURNING( ) \
    { }

#define STL_PARSER_UPDATE_RETURNING_CLAUSE____RETURNING_NEW( ) \
    { }

#define STL_PARSER_UPDATE_RETURNING_CLAUSE____RETURNING_OLD( ) \
    { }

/**
 * select_statement
 */

#define STL_PARSER_SELECT_STATEMENT____QUERY_EXPRESSION()   \
    { }


/**
 * updatabilty_clause:
 */

#define STL_PARSER_UPDATABILITY____EMPTY() \
   {  }

#define STL_PARSER_UPDATABILITY____FOR_READ_ONLY() \
   {  }

#define STL_PARSER_UPDATABILITY____FOR_UPDATE() \
   {  }

#define STL_PARSER_UPDATABILITY____FOR_UPDATE_OF() \
   {  }

/**
 * for_update_lock_wait_mode
 */

#define STL_PARSER_FOR_UPDATE_LOCK_WAIT_MODE____EMPTY()    \
    {  }                                        

#define STL_PARSER_FOR_UPDATE_LOCK_WAIT_MODE____WAIT() \
   {  }

#define STL_PARSER_FOR_UPDATE_LOCK_WAIT_MODE____WAIT_SECOND() \
   {  }

#define STL_PARSER_FOR_UPDATE_LOCK_WAIT_MODE____NOWAIT() \
   {  }

#define STL_PARSER_FOR_UPDATE_LOCK_WAIT_MODE____SKIP_LOCKED() \
   {  }

/**
 * for_update_column_list
 */

#define STL_PARSER_FOR_UPDATE_COLUMN_LIST____COLUMN() \
   {  }

#define STL_PARSER_FOR_UPDATE_COLUMN_LIST____COLUMN_LIST() \
   {  }

/**
 * select_into_statement
 */

#define STL_PARSER_SELECT_INTO_STATEMENT____()              \
    { }

#define STL_PARSER_SELECT_INTO_STATEMENT____ALL()           \
    { }

#define STL_PARSER_SELECT_INTO_STATEMENT____DISTINCT()      \
    { }


/**
 * query_expression
 */

#define STL_PARSER_QUERY_EXPRESSION____NO_WITH_CLAUSE( )        \
    { }

#define STL_PARSER_QUERY_EXPRESSION____WITH_CLAUSE( )           \
    { }
 

/**
 * query_expression_body
 */
 
#define STL_PARSER_QUERY_EXPRESSION_BODY____()  \
    { }
 
#define STL_PARSER_QUERY_EXPRESSION_BODY____UNION_DEFAULT()                         \
    { }
 
#define STL_PARSER_QUERY_EXPRESSION_BODY____UNION_ALL()                             \
    { }
 
#define STL_PARSER_QUERY_EXPRESSION_BODY____UNION_DISTINCT()                        \
    { }

#define STL_PARSER_QUERY_EXPRESSION_BODY____EXCEPT_DEFAULT()                        \
    { }

#define STL_PARSER_QUERY_EXPRESSION_BODY____EXCEPT_ALL()                            \
    { }

#define STL_PARSER_QUERY_EXPRESSION_BODY____EXCEPT_DISTINCT()                       \
    { }


/**
 * query_term
 */
 
#define STL_PARSER_QUERY_TERM____() \
    { }

#define STL_PARSER_QUERY_TERM____INTERSECT_DEFAULT()                                \
    { }

#define STL_PARSER_QUERY_TERM____INTERSECT_ALL()                                    \
    { }

#define STL_PARSER_QUERY_TERM____INTERSECT_DISTINCT()                               \
    { }


/**
 * query_primary
 */

#define STL_PARSER_QUERY_PRIMARY____QUERY_SPEC()                                        \
    { }

#define STL_PARSER_QUERY_PRIMARY____PARENTHESIZED_QUERY_EXPRESSION_NO_WITH_CLAUSE() \
    { }


/**
 * parenthesized_query_expression_no_with_clause
 */

#define STL_PARSER_PARENTHESIZED_QUERY_EXPRESSION_NO_WITH_CLAUSE____()              \
    { }


/**
 * parenthesized_query_expression_no_with_clause_head
 */

#define STL_PARSER_PARENTHESIZED_QUERY_EXPRESSION_NO_WITH_CLAUSE_HEAD____()              \
    { }


/**
 * parenthesized_query_expression_with_clause
 */

#define STL_PARSER_PARENTHESIZED_QUERY_EXPRESSION_WITH_CLAUSE____()                 \
    { }


/**
 * corresponding_spec
 */

#define STL_PARSER_CORRESPONDING_SPEC____EMPTY()    \
    { }


/**
 * query_specification
 */

#define STL_PARSER_QUERY_SPECIFICATION____()                \
    { }

#define STL_PARSER_QUERY_SPECIFICATION____ALL()                             \
    { }

#define STL_PARSER_QUERY_SPECIFICATION____DISTINCT()                            \
    { }


/**
 * from_clause
 */

#define STL_PARSER_FROM_CLAUSE____()    \
    { }


/**
 * where_clause
 */

#define STL_PARSER_WHERE_CLAUSE____EMPTY()  \
    { }

#define STL_PARSER_WHERE_CLAUSE____WHERE()  \
    { }


/**
 * table_reference_list
 */

#define STL_PARSER_TABLE_REFERENCE_LIST____SINGLE( )    \
    { }

#define STL_PARSER_TABLE_REFERENCE_LIST____LIST( )                                      \
    { }


/**
 * table_reference
 */

#define STL_PARSER_TABLE_REFERENCE____TABLE_FACTOR()    \
    { }

#define STL_PARSER_TABLE_REFERENCE____JOINED_TABLE()    \
    { }


/**
 * table_factor
 */

#define STL_PARSER_TABLE_FACTOR____()   \
    { }


/**
 * joined_table
 */

#define STL_PARSER_JOINED_TABLE____CROSS_JOIN() \
    { }
 
#define STL_PARSER_JOINED_TABLE____QUALIFIED_JOIN() \
    { }

#define STL_PARSER_JOINED_TABLE____NATURAL_JOIN()   \
    { }

#define STL_PARSER_JOINED_TABLE____ESC_JOIN()       \
    { }


/**
 * cross_join
 */

#define STL_PARSER_CROSS_JOIN____()                                                     \
    { }

/**
 * qualified_join
 */

#define STL_PARSER_QUALIFIED_JOIN____WITHOUT_JOIN_TYPE()                                \
    { }

#define STL_PARSER_QUALIFIED_JOIN____WITH_JOIN_TYPE()                                           \
    { }


/**
 * natural_join
 */

#define STL_PARSER_NATURAL_JOIN____WITHOUT_JOIN_TYPE()                                  \
    { }

#define STL_PARSER_NATURAL_JOIN____WITH_JOIN_TYPE()                                     \
    { }


/**
 * escape_join
 */

#define STL_PARSER_ESCAPE_JOIN____OUTER_JOIN()                 \
    { }

#define STL_PARSER_ESCAPE_OUTER_JOIN____()                     \
    { }

#define STL_PARSER_ESCAPE_OUTER_TARGET____TABLE_REF()          \
    { }

#define STL_PARSER_ESCAPE_OUTER_TARGET____OUTER_JOIN()         \
    { }

/**
 * join_specification
 */

#define STL_PARSER_JOIN_SPECIFICATION____JOIN_CONDITION()                           \
    { }

#define STL_PARSER_JOIN_SPECIFICATION____NAMED_COLUMNS()                            \
    { }


/**
 * join_condition
 */

#define STL_PARSER_JOIN_CONDITION____() \
    { }


/**
 * search_condition
 */

#define STL_PARSER_SEARCH_CONDITION____()   \
    { }


/**
 * named_columns_join
 */

#define STL_PARSER_NAMED_COLUMNS_JOIN____() \
    { }


/**
 * join_column_list
 */

#define STL_PARSER_JOIN_COLUMN_LIST____()   \
    { }


/**
 * join_type
 */

#define STL_PARSER_JOIN_TYPE____INNER()                                                 \
    { }

#define STL_PARSER_JOIN_TYPE____WITHOUT_OUTER_KEYWORD() \
    { }
 
#define STL_PARSER_JOIN_TYPE____WITH_OUTER_KEYWORD()    \
    { }

#define STL_PARSER_JOIN_TYPE____SEMI()          \
    { }

#define STL_PARSER_JOIN_TYPE____ANTI_SEMI()     \
    { }
 
 
/**
 * outer_join_type
 */
 
#define STL_PARSER_OUTER_JOIN_TYPE____LEFT()                                                    \
    { }

#define STL_PARSER_OUTER_JOIN_TYPE____RIGHT()                                                   \
    { }

#define STL_PARSER_OUTER_JOIN_TYPE____FULL()                                                    \
    { }


/**
 * semi_join_type
 */
 
#define STL_PARSER_SEMI_JOIN_TYPE____LEFT()     \
    { }

#define STL_PARSER_SEMI_JOIN_TYPE____RIGHT()    \
    { }


/**
 * anti_semi_join_type
 */
 
#define STL_PARSER_ANTI_SEMI_JOIN_TYPE____LEFT()        \
    { }

#define STL_PARSER_ANTI_SEMI_JOIN_TYPE____RIGHT()       \
    { }


/**
 * table_primary_element
 */

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____NAME( )                                             \
    { }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____NAME_ALIAS_WITH_AS( )                               \
    { }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____NAME_ALIAS_WITH_BLANK( )                            \
    { }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DERIVED_TABLE( )                                    \
    { }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DERIVED_TABLE_ALIAS_WITH_AS( )                      \
    { }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DERIVED_TABLE_ALIAS_WITH_BLANK( )                   \
    { }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____JOIN_TABLE( )                       \
    { }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DUMP_TABLE( )                                       \
    { }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DUMP_TABLE_ALIAS_WITH_AS( )                         \
    { }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DUMP_TABLE_ALIAS_WITH_BLANK( )                      \
    { }


/**
 * derived_column_list : qllList
 */

#define STL_PARSER_DERIVED_COLUMN_LIST____()    \
    { }

#define STL_PARSER_DERIVED_COLUMN_LIST____COLUMN_LIST() \
    { }


/**
 * derived_table
 */

#define STL_PARSER_DERIVED_TABLE____()  \
    { }


/**
 * table_subquery
 */

#define STL_PARSER_TABLE_SUBQUERY____() \
    { }


/**
 * subquery
 */

#define STL_PARSER_SUBQUERY____NO_WITH_CLAUSE() \
    { }

#define STL_PARSER_SUBQUERY____WITH_CLAUSE()    \
    { }


/**
 * parenthesized_joined_table
 */

#define STL_PARSER_PARENTHESIZED_JOINED_TABLE____() \
    { }

#define STL_PARSER_PARENTHESIZED_JOINED_TABLE____RECURSIVE_PAREN()  \
    { }


/**
 * group_by_clause
 */

#define STL_PARSER_GROUP_BY_CLAUSE____EMPTY()   \
    { }

#define STL_PARSER_GROUP_BY_CLAUSE____ELEMENT_LIST()    \
    { }


/**
 * grouping_element_list
 */
    
#define STL_PARSER_GROUPING_ELEMENT_LIST____GROUPING_ELEMENT()  \
    { }

#define STL_PARSER_GROUPING_ELEMENT_LIST____GROUPING_ELEMENT_LIST()     \
    { }


/**
 * grouping_element
 */
    
#define STL_PARSER_GROUPING_ELEMENT____ORDINARY_GROUPING_SET()  \
    { }

#define STL_PARSER_GROUPING_ELEMENT____EMPTY_GROUPING_SET()  \
    { }


/**
 * ordinary_grouping_set
 */
    
#define STL_PARSER_ORDINARY_GROUPING_SET____GROUPING_COLUMN_REFERENCE() \
    { }

#define STL_PARSER_ORDINARY_GROUPING_SET____GROUPING_COLUMN_REFERENCE_LIST()    \
    { }


/**
 * grouping_column_reference
 */
    
#define STL_PARSER_GROUPING_COLUMN_REFERENCE____VALUE_EXPRESSION()      \
    { }


/**
 * grouping_column_reference_list
 */
    
#define STL_PARSER_GROUPING_COLUMN_REFERENCE_LIST____GROUPING_COLUMN_REFERENCE()        \
    { }

#define STL_PARSER_GROUPING_COLUMN_REFERENCE_LIST____GROUPING_COLUMN_REFERENCE_LIST()   \
    { }


/**
 * empty_grouping_set
 */

#define STL_PARSER_EMPTY_GROUPING_SET____EMPTY()        \
    { }


/**
 * having_clause
 */

#define STL_PARSER_HAVING_CLAUSE____EMPTY()     \
    { }

#define STL_PARSER_HAVING_CLAUSE____SEARCH_CONDITION()  \
    { }


/**
 * window_clause
 */

#define STL_PARSER_WINDOW_CLAUSE____EMPTY()     \
    { }


/**
 * order_by_clause
 */

#define STL_PARSER_ORDER_BY_CLAUSE____EMPTY()   \
    { }

#define STL_PARSER_ORDER_BY_CLAUSE____SORT()    \
    { }


/**
 * sort_specification_list
 */

#define STL_PARSER_SORT_SPECIFICATION_LIST____SORT_SPEC()               \
    { }

#define STL_PARSER_SORT_SPECIFICATION_LIST____SORT_SPEC_LIST()          \
    { }


/**
 * sort_specification
 */

#define STL_PARSER_SORT_SPECIFICATION____KEY()              \
    { }

#define STL_PARSER_SORT_SPECIFICATION____KEY_SORT()         \
    { }

#define STL_PARSER_SORT_SPECIFICATION____KEY_NULLS()        \
    { }

#define STL_PARSER_SORT_SPECIFICATION____KEY_SORT_NULLS()   \
    { }


/**
 * select_target_clause
 */

#define STL_PARSER_SELECT_TARGET_CLAUSE____EMPTY( ) \
    { }                                             

#define STL_PARSER_SELECT_TARGET_CLAUSE____TARGETS( )   \
    { }                                             

    
/**
 * select_target_list
 */

#define STL_PARSER_SELECT_TARGET_LIST____TARGET( )      \
    { }                                             

#define STL_PARSER_SELECT_TARGET_LIST____TARGET_LIST( ) \
    { }                                             


/**
 * target_specification
 */

#define STL_PARSER_TARGET_SPECIFICATION____HOST_PARAMETER( )    \
    {                                                           \
        STL_PARSER_CHECK( v0 = ztqAddOutHostVariable( param,    \
                                                      v1,       \
                                                      v2 ) );   \
    }

#define STL_PARSER_TARGET_SPECIFICATION____DYNAMIC_PARAMETER( ) \
    {                                                           \
        STL_PARSER_ERROR( NULL, param, param->mScanner, NULL ); \
        YYABORT;                                                \
    }


/**
 * select_list
 */

#define STL_PARSER_SELECT_LIST____SUBLIST( )    \
    { }

#define STL_PARSER_SELECT_LIST____ALL( )        \
    { }


/**
 * select_sublist
 */

#define STL_PARSER_SELECT_SUBLIST____SUBLIST2( )                        \
    { }

#define STL_PARSER_SELECT_SUBLIST____SUBLIST2_LIST( )           \
    { }


/**
 * select_sublist2
 */

#define STL_PARSER_SELECT_SUBLIST2____DERIVED_COLUMN( )         \
    { }

#define STL_PARSER_SELECT_SUBLIST2____IDENTIFIER_ASTERISK( )            \
    { }

#define STL_PARSER_SELECT_SUBLIST2____VALUE_EXPRESSION_ASTERISK( )                      \
    { }

#define STL_PARSER_SELECT_SUBLIST2____VALUE_EXPRESSION_ASTERISK_WITH_AS( )              \
    { }


/**
 * derived_column
 */

#define STL_PARSER_DERIVED_COLUMN____VALUE( )   \
    { }

#define STL_PARSER_DERIVED_COLUMN____VALUE_ALIAS_WITH_AS( )      \
    { }

#define STL_PARSER_DERIVED_COLUMN____VALUE_ALIAS_WITH_BLANK( )      \
    { }


/**
 * hint_clause
 */

#define STL_PARSER_HINT_CLAUSE____EMPTY( )      \
    {                                           \
        param->mErrSkip = STL_FALSE;            \
        param->mErrClear = STL_FALSE;           \
    }

#define STL_PARSER_HINT_CLAUSE____LIST( )       \
    {                                           \
        param->mErrSkip = STL_FALSE;            \
        param->mErrClear = STL_FALSE;           \
    }


/**
 * hint_list
 */

#define STL_PARSER_HINT_LIST____()              \
    {                                           \
        if( param->mErrClear == STL_TRUE )      \
        {                                       \
            yyclearin;                          \
            param->mErrClear = STL_FALSE;       \
        }                                       \
    }

#define STL_PARSER_HINT_LIST____LIST()          \
    {                                           \
        if( param->mErrClear == STL_TRUE )      \
        {                                       \
            yyclearin;                          \
            param->mErrClear = STL_FALSE;       \
        }                                       \
    }

#define STL_PARSER_HINT_LIST____COMMA_LIST()    \
    {                                           \
        param->mErrClear = STL_FALSE;           \
    }


/**
 * hint_element
 */

#define STL_PARSER_HINT_ELEMENT____ACCESS_PATH_HINTS()  \
    { }

#define STL_PARSER_HINT_ELEMENT____JOIN_HINTS() \
    { }

#define STL_PARSER_HINT_ELEMENT____QUERY_TRANSFORMATION_HINTS() \
    { }

#define STL_PARSER_HINT_ELEMENT____OTHER_HINTS()        \
    { }

#define STL_PARSER_HINT_ELEMENT____ERROR()      \
    {                                           \
        if( STL_PARSER_CHAR != YYEOF )          \
        {                                       \
            param->mErrStatus = STL_SUCCESS;    \
            param->mErrSkip = STL_TRUE;         \
            param->mErrClear = STL_TRUE;        \
            yyerrok;                            \
        }                                       \
        else                                    \
        {                                       \
            param->mErrStatus = STL_FAILURE;    \
        }                                       \
    }


/**
 * access_path_hints
 */

#define STL_PARSER_ACCESS_PATH_HINTS____ERROR() \
    {                                           \
        param->mErrStatus = STL_SUCCESS;        \
        param->mErrSkip = STL_TRUE;             \
        param->mErrClear = STL_FALSE;           \
        yyerrok;                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____FULL()  \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____HASH()                                          \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX()                                         \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_ONLY_TABLE_NAME()                         \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_WITH_COMMA()                              \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____NO_INDEX()                                      \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____NO_INDEX_ONLY_TABLE_NAME()                      \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____NO_INDEX_WITH_COMMA()                           \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_ASC()                                     \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_ASC_ONLY_TABLE_NAME()                     \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_ASC_WITH_COMMA()                          \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_DESC()                                    \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_DESC_ONLY_TABLE_NAME()                    \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_DESC_WITH_COMMA()                         \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____ROWID()                                         \
    { }                                                                                 

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_COMBINE()                                 \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_COMBINE_ONLY_TABLE_NAME()                 \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_COMBINE_WITH_COMMA()                      \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____IN_KEY_RANGE()                                  \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____IN_KEY_RANGE_ONLY_TABLE_NAME()                  \
    { }

#define STL_PARSER_ACCESS_PATH_HINTS____IN_KEY_RANGE_WITH_COMMA()                       \
    { }


/**
 * join_hints
 */

#define STL_PARSER_JOIN_HINTS____ORDERED()                                              \
    { }

#define STL_PARSER_JOIN_HINTS____ORDERING()                                             \
    { }

#define STL_PARSER_JOIN_HINTS____LEADING()                                              \
    { }

#define STL_PARSER_JOIN_HINTS____USE_HASH()                                             \
    { }

#define STL_PARSER_JOIN_HINTS____NO_USE_HASH()                                          \
    { }

#define STL_PARSER_JOIN_HINTS____USE_INL()                                              \
    { }

#define STL_PARSER_JOIN_HINTS____NO_USE_INL()                                           \
    { }

#define STL_PARSER_JOIN_HINTS____USE_MERGE()                                            \
    { }

#define STL_PARSER_JOIN_HINTS____NO_USE_MERGE()                                         \
    { }

#define STL_PARSER_JOIN_HINTS____USE_NL()                                               \
    { }

#define STL_PARSER_JOIN_HINTS____NO_USE_NL()                                            \
    { }


/**
 * query_transformation_hints
 */

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____UNNEST()                               \
    { }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____NO_UNNEST()                            \
    { }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____NL_ISJ()                               \
    { }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____NL_SJ()                                \
    { }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____NL_AJ()                                \
    { }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____MERGE_SJ()                             \
    { }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____MERGE_AJ()                             \
    { }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____HASH_ISJ()                             \
    { }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____HASH_SJ()                              \
    { }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____HASH_AJ()                              \
    { }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____NO_QUERY_TRANSFORMATION()              \
    { }


/**
 * other_hints
 */

#define STL_PARSER_OTHER_HINTS____PUSH_PRED()                                           \
    { }

#define STL_PARSER_OTHER_HINTS____PUSH_PRED_WITH_TABLE()                                \
    { }

#define STL_PARSER_OTHER_HINTS____NO_PUSH_PRED()                                        \
    { }

#define STL_PARSER_OTHER_HINTS____NO_PUSH_PRED_WITH_TABLE()                             \
    { }

#define STL_PARSER_OTHER_HINTS____PUSH_SUBQ()                                           \
    { }

#define STL_PARSER_OTHER_HINTS____NO_PUSH_SUBQ()                                        \
    { }

#define STL_PARSER_OTHER_HINTS____USE_DISTINCT_HASH()                                   \
    { }

#define STL_PARSER_OTHER_HINTS____USE_GROUP_HASH()                                      \
    { }


/**
 * table_identifier_chain_list : qlpList : qlpRelInfo
 */

#define STL_PARSER_TABLE_IDENTIFIER_CHAIN_LIST____()                                    \
    { }

#define STL_PARSER_TABLE_IDENTIFIER_CHAIN_LIST____LIST()                                \
    { }

#define STL_PARSER_TABLE_IDENTIFIER_CHAIN_LIST____COMMA_LIST()                          \
    { }


/**
 * table_identifier_chain : qlpRelInfo
 */

#define STL_PARSER_TABLE_IDENTIFIER_CHAIN____()                                         \
    { }


/**
 * ordering_table_identifier_chain_list : qlpList
 */

#define STL_PARSER_ORDERING_TABLE_IDENTIFIER_CHAIN_LIST____()                           \
    { }

#define STL_PARSER_ORDERING_TABLE_IDENTIFIER_CHAIN_LIST____COMMA_LIST()                 \
    { }


/**
 * ordering_table_identifier_chain : qlpOrdering
 */

#define STL_PARSER_ORDERING_TABLE_IDENTIFIER_CHAIN____()                                \
    { }

#define STL_PARSER_ORDERING_TABLE_IDENTIFIER_CHAIN____LEFT()                            \
    { }

#define STL_PARSER_ORDERING_TABLE_IDENTIFIER_CHAIN____RIGHT()                           \
    { }


/**
 * index_identifier_chain_list : qlpList : qlpIndexInfo
 */

#define STL_PARSER_INDEX_IDENTIFIER_CHAIN_LIST____()                                    \
    { }

#define STL_PARSER_INDEX_IDENTIFIER_CHAIN_LIST____LIST()                                \
    { }

#define STL_PARSER_INDEX_IDENTIFIER_CHAIN_LIST____COMMA_LIST()                          \
    { }


/**
 * index_identifier_chain : qlpIndexInfo
 */

#define STL_PARSER_INDEX_IDENTIFIER_CHAIN____()                                         \
    { }


/**
 * result skip and limit
 * 
 * OFFSET .. FETCH ..
 * OFFSET .. LIMIT ..
 */

#define STL_PARSER_RESULT_OFFSET_CLAUSE____EMPTY()   \
    { }                                              

#define STL_PARSER_RESULT_OFFSET_CLAUSE____COUNT()   \
    { }                                              

#define STL_PARSER_FETCH_LIMIT_CLAUSE____EMPTY()   \
    { }                                              

#define STL_PARSER_FETCH_LIMIT_CLAUSE____COUNT()   \
    { }                                              

#define STL_PARSER_FETCH_FIRST_CLAUSE____COUNT()   \
    { }                                              

#define STL_PARSER_FETCH_FIRST_CLAUSE____EMPTY()   \
    { }                                              

#define STL_PARSER_LIMIT_CLAUSE____COUNT()   \
    { }                                              

#define STL_PARSER_LIMIT_CLAUSE____ALL()   \
    { }                                              

#define STL_PARSER_LIMIT_CLAUSE____SKIP_LIMIT() \
    { }                                              


/**
 * value_expression
 */

#define STL_PARSER_VALUE_EXPRESSION____( )    \
    { }


/**
 * value_expression_primary
 */

#define STL_PARSER_VALUE_EXPRESSION_PRIMARY____OR( )    \
    { }


/**
 * parenthesized_value_expression
 */

#define STL_PARSER_PARENTHESIZED_VALUE_EXPRESSION____EXPRESSION( )      \
    { }

#define STL_PARSER_PARENTHESIZED_VALUE_EXPRESSION____QUERY( )   \
    { }


/**
 * value_OR_term
 */

#define STL_PARSER_VALUE_OR_TERM____SINGLE( )   \
    { }

#define STL_PARSER_VALUE_OR_TERM____OR( )       \
    { }


/**
 * value_AND_term
 */

#define STL_PARSER_VALUE_AND_TERM____SINGLE( )  \
    { }

#define STL_PARSER_VALUE_AND_TERM____AND( )     \
    { }


/**
 * value_NOT_term
 */

#define STL_PARSER_VALUE_NOT_TERM____SINGLE( )  \
    { }

#define STL_PARSER_VALUE_NOT_TERM____NOT( )     \
    { }


/**
 * value_test
 */

#define STL_PARSER_VALUE_TEST____PREDICATE( )   \
    { }

#define STL_PARSER_VALUE_TEST____COMP( )        \
    { }


/**
 * value_predicate
 */

#define STL_PARSER_VALUE_PREDICATE____PREDICATE( )      \
    { }

#define STL_PARSER_VALUE_PREDICATE____BETWEEN( )        \
    { }

#define STL_PARSER_VALUE_PREDICATE____LIKE( )   \
    { }

#define STL_PARSER_VALUE_PREDICATE____IN( )   \
    { }

#define STL_PARSER_VALUE_PREDICATE____EXISTS( ) \
    { }

#define STL_PARSER_VALUE_PREDICATE____QUANTIFIED_COMP( )        \
    { }



/**
 * value_predicate_2
 */

#define STL_PARSER_VALUE_PREDICATE_2____PREDICATE( )    \
    { }

#define STL_PARSER_VALUE_PREDICATE_2____CONCAT( )       \
    { }


/**
 * value_predicate_3
 */

#define STL_PARSER_VALUE_PREDICATE_3____PREDICATE( )    \
    { }

#define STL_PARSER_VALUE_PREDICATE_3____IS_TRUTH( )     \
    { }

#define STL_PARSER_VALUE_PREDICATE_3____IS_NOT_TRUTH( ) \
    { }

#define STL_PARSER_VALUE_PREDICATE_3____IS_NULL( )      \
    { }

#define STL_PARSER_VALUE_PREDICATE_3____IS_NOT_NULL( )  \
    { }


/**
 * value_predicate_4
 */

#define STL_PARSER_VALUE_PREDICATE_4____TERM( ) \
    { }

#define STL_PARSER_VALUE_PREDICATE_4____ADD( )  \
    { }

#define STL_PARSER_VALUE_PREDICATE_4____SUB( )  \
    { }


/**
 * value_term
 */

#define STL_PARSER_VALUE_TERM____FACTOR( )      \
    { }

#define STL_PARSER_VALUE_TERM____MULTIPLE( )    \
    { }

#define STL_PARSER_VALUE_TERM____DIVIDE( )      \
    { }


/**
 * sign
 */

#define STL_PARSER_SIGN____PLUS( )              \
    { }


#define STL_PARSER_SIGN____MINUS( )             \
    { }


/**
 * value_factor
 */

#define STL_PARSER_VALUE_FACTOR____FACTOR( )    \
    { }

#define STL_PARSER_VALUE_FACTOR____POSITIVE( )  \
    { }

#define STL_PARSER_VALUE_FACTOR____NEGATIVE( )  \
    { }


/**
 * value_factor_2
 */

#define STL_PARSER_VALUE_FACTOR_2____FACTOR( )  \
    { }

#define STL_PARSER_VALUE_FACTOR_2____TIME_ZONE( )  \
    { }


/**
 * value_factor_3
 */

#define STL_PARSER_VALUE_FACTOR_3____NON_PAREN_VALUE( ) \
    { }

#define STL_PARSER_VALUE_FACTOR_3____PAREN_VALUE( )     \
    { }

#define STL_PARSER_VALUE_FACTOR_3____CAST( )    \
    { }

#define STL_PARSER_VALUE_FACTOR_3____NUMERIC_FUNCTION( )        \
    { }

#define STL_PARSER_VALUE_FACTOR_3____STRING_FUNCTION( ) \
    { }

#define STL_PARSER_VALUE_FACTOR_3____DATETIME_FUNCTION( )       \
    { }

#define STL_PARSER_VALUE_FACTOR_3____NVL_FUNCTION( )            \
    { }

#define STL_PARSER_VALUE_FACTOR_3____NVL2_FUNCTION( )           \
    { }

#define STL_PARSER_VALUE_FACTOR_3____DUMP_FUNCTION( )   \
    { }

#define STL_PARSER_VALUE_FACTOR_3____DECODE_FUNCTION( )   \
    { }

#define STL_PARSER_VALUE_FACTOR_3____CASE2_FUNCTION( )   \
    { }

#define STL_PARSER_VALUE_FACTOR_3____GENERAL_COMPARISON_FUNCTION( )   \
    { }

/**
 * nonparenthesized_value_expression_primary
 */

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____UNSIGNED_VALUE( )       \
    { }

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____SEQUENCE_FUNCTION( )    \
    { }

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____SESSION_FUNCTION( )        \
    { }

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____AGGREGATE_FUNCTION( )   \
    { }

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____CASE_EXPRESSION( )      \
    { }


/**
 * sequence_function : qlpFunction
 */

#define STL_PARSER_SEQUENCE_FUNCTION____CURRVAL( )      \
    { }

#define STL_PARSER_SEQUENCE_FUNCTION____NEXTVAL( )      \
    { }

#define STL_PARSER_SEQUENCE_FUNCTION____NEXT_VALUE_FOR( )  \
    { }

/**
 * numeric_value_function
 */

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ABSOLUTE( )        \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____POSITIVE( )  \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____NEGATIVE( )  \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ADDITION( )        \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SUBTRACTION( )     \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____MULTIPLICATION( )  \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____DIVISION( )    \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____DEGREES( ) \
    { }                                                 

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____FACTORIAL( ) \
    { }                                                 

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____LOG( ) \
    { }                                                 

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____PI( ) \
    { }                                                 

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____RADIANS( ) \
    { }                                                 

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____RANDOM( ) \
    { }                                                 

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ROUND( ) \
    { }                                                 

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SIGN( ) \
    { }                                                 

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____TRUNC( ) \
    { }                                                 

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____MODULUS( ) \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____NATURAL_LOGARITHM( )       \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____EXPONENTIAL( )     \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____POWER( )   \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____CUBE_ROOT( )   \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SQUARE_ROOT( )     \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____FLOOR( )   \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____CEILING( ) \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____WIDTH_BUCKET( )    \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ACOS( ) \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ASIN( ) \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ATAN( ) \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ATAN2( ) \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____COS( ) \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____COT( ) \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SIN( ) \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____TAN( ) \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____BITAND( )  \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____BITNOT( )  \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____BITOR( )   \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____BITXOR( )  \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SHIFT_LEFT( )  \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SHIFT_RIGHT( ) \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____BIT_LENGTH( )  \
    { }                                                     

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____CHAR_LENGTH( )     \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____OCTET_LENGTH( )    \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____POSITION( )        \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____INSTR( )    \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____EXTRACT( )    \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ROWID_OBJECT_ID()  \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ROWID_PAGE_ID()    \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ROWID_ROW_NUMBER() \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ROWID_TABLESPACE_ID()      \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____TO_NUMBER()              \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____TO_NATIVE_REAL()              \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____TO_NATIVE_DOUBLE()              \
    { }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____STATEMENT_VIEW_SCN()     \
    { }

/**
 * absolute_value_expression
 */

#define STL_PARSER_ABSOLUTE_FUNCTION____( )     \
    { }

/**
 * positive_function
 */
#define STL_PARSER_POSITIVE_FUNCTION____( )                         \
    { }

/**
 * negative_function
 */
#define STL_PARSER_NEGATIVE_FUNCTION____( )                         \
    {  }

/**
 * addition_function
 */
#define STL_PARSER_ADDITION_FUNCTION____( )     \
    { }

/**
 * subtraction_function
 */
#define STL_PARSER_SUBTRACTION_FUNCTION____( )     \
    { }

/**
 * multiplication_function
 */
#define STL_PARSER_MULTIPLICATION_FUNCTION____( )     \
    { }

/**
 * division_function
 */
#define STL_PARSER_DIVISION_FUNCTION____( )     \
    { }

/**
 * degrees_function
 */
#define STL_PARSER_DEGREES_FUNCTION____( )      \
    { }

/**
 * factorial_function
 */
#define STL_PARSER_FACTORIAL_FUNCTION____( )      \
    { }

/**
 * log10_value_function
 */
#define STL_PARSER_LOG10_FUNCTION____( )        \
    { }

/**
 * log_value_function
 */
#define STL_PARSER_LOG_FUNCTION____( )          \
    { }

/**
 * pi_value_function
 */
#define STL_PARSER_PI_FUNCTION____( )          \
    { }

/**
 * radians_function
 */
#define STL_PARSER_RADIANS_FUNCTION____( )      \
    { }

/**
 * random_function
 */
#define STL_PARSER_RANDOM_FUNCTION____( )      \
    { }

/**
 * round_integer_function
 */
#define STL_PARSER_ROUND_INTEGER_FUNCTION____( )    \
    { }

/**
 * round_scale_function
 */
#define STL_PARSER_ROUND_SCALE_FUNCTION____( )  \
    { } 

/**
 * sign_function
 */
#define STL_PARSER_SIGN_FUNCTION____( )    \
    { }

/**
 * trunc_integer_function
 */
#define STL_PARSER_TRUNC_INTEGER_FUNCTION____( )    \
    { }

/**
 * trunc_scale_function
 */
#define STL_PARSER_TRUNC_SCALE_FUNCTION____( )  \
    { } 

/**
 * modulus_function
 */
#define STL_PARSER_MODULUS_FUNCTION____( )    \
    { }


/**
 * natural_logarithm
 */

#define STL_PARSER_NATURAL_LOGARITHM____( )     \
    { }


/**
 * exponential_function
 */

#define STL_PARSER_EXPONENTIAL_FUNCTION____( )  \
    { }


/**
 * power_function
 */

#define STL_PARSER_POWER_FUNCTION____( )        \
    { }

/**
 * cube_root
 */

#define STL_PARSER_CUBE_ROOT____( )           \
    { }

/**
 * square_root
 */

#define STL_PARSER_SQUARE_ROOT____( )           \
    { }


/**
 * floor_function
 */

#define STL_PARSER_FLOOR_FUNCTION____( )        \
    { }


/**
 * ceiling_function
 */

#define STL_PARSER_CEILING_FUNCTION____( )      \
    { }

/**
 * width_bucket_function
 */
#define STL_PARSER_WIDTH_BUCKET_FUNCTION____( ) \
    { }

/**
 * acos_function
 */
#define STL_PARSER_ACOS_FUNCTION____( )         \
    { }

/**
 * asin_function
 */
#define STL_PARSER_ASIN_FUNCTION____( )         \
    { }

/**
 * atan_function
 */
#define STL_PARSER_ATAN_FUNCTION____( )         \
    { }

/**
 * atan2_function
 */
#define STL_PARSER_ATAN2_FUNCTION____( )        \
    { }

/**
 * cos_function
 */
#define STL_PARSER_COS_FUNCTION____( )          \
    { }

/**
 * cot_function
 */
#define STL_PARSER_COT_FUNCTION____( )          \
    { }

/**
 * sin_function
 */
#define STL_PARSER_SIN_FUNCTION____( )          \
    { }

/**
 * tan_function
 */
#define STL_PARSER_TAN_FUNCTION____( )          \
    { }

/**
 * bitand_function
 */
#define STL_PARSER_BITAND_FUNCTION____( )       \
    { }

/**
 * bitnot_function
 */
#define STL_PARSER_BITNOT_FUNCTION____( )       \
    { }

/**
 * bitor_function
 */
#define STL_PARSER_BITOR_FUNCTION____( )        \
    { }

/**
 * bitxor_function
 */
#define STL_PARSER_BITXOR_FUNCTION____( )       \
    { }

/**
 * shift_left_function
 */
#define STL_PARSER_SHIFT_LEFT_FUNCTION____( )   \
    { }

/**
 * shift_right_function
 */
#define STL_PARSER_SHIFT_RIGHT_FUNCTION____( )  \
    { }

/**
 * bit_length_function
 */
#define STL_PARSER_BIT_LENGTH_FUNCTION____( )   \
    { }

/**
 * char_length_function
 */
#define STL_PARSER_CHAR_LENGTH_FUNCTION____( )  \
    { }

/**
 * octet_length_function
 */
#define STL_PARSER_OCTET_LENGTH_FUNCTION____( ) \
    { }

/**
 * position_function
 */
#define STL_PARSER_POSITION_FUNCTION____( )     \
    { }

/**
 * instr_function
 */
#define STL_PARSER_INSTR_FUNCTION____DEFAULT( )         \
    { }

#define STL_PARSER_INSTR_FUNCTION____POSITION( )        \
    { }

#define STL_PARSER_INSTR_FUNCTION____OCCURRENCE( )      \
    { }

/**
 * rowid_object_id_value_function
 */
#define STL_PARSER_ROWID_OBJECT_ID_FUNCTION____( )      \
    { }

/**
 * rowid_page_id_value_function
 */
#define STL_PARSER_ROWID_PAGE_ID_FUNCTION____( )        \
    { }

/**
 * rowid_row_number_value_function
 */
#define STL_PARSER_ROWID_ROW_NUMBER_FUNCTION____( )     \
    { }

/**
 * rowid_tablespace_id_value_function
 */ 
#define STL_PARSER_ROWID_TABLESPACE_ID_FUNCTION____( )  \
    { }

/**
 * to_number_value_function
 */
#define STL_PARSER_TO_NUMBER_FUNCTION____DEFAULT( )     \
    { }

#define STL_PARSER_TO_NUMBER_FUNCTION____( )            \
    { }

/**
 * to_native_real_value_function
 */
#define STL_PARSER_TO_NATIVE_REAL_FUNCTION____DEFAULT( )     \
    { }

#define STL_PARSER_TO_NATIVE_REAL_FUNCTION____( )            \
    { }

/**
 * to_native_double_value_function
 */
#define STL_PARSER_TO_NATIVE_DOUBLE_FUNCTION____DEFAULT( )     \
    { }

#define STL_PARSER_TO_NATIVE_DOUBLE_FUNCTION____( )            \
    { }

/**
 * statement_view_scn_function
 */
#define STL_PARSER_STATEMENT_VIEW_SCN_FUNCTION____( )          \
    { }

/**
 * extract_function
 */
#define STL_PARSER_EXTRACT_FUNCTION____( )                                 \
    { }

/**
 * date_part_function
 */
#define STL_PARSER_DATE_PART_FUNCTION____( )                                 \
    { }

/**
 * date_part_field
 */
#define STL_PARSER_EXTRACT_FIELD____DATE_FIELD_STRING( )  \
    { }

/**
 * extract_field
 */
#define STL_PARSER_EXTRACT_FIELD____PRIMARY_DATETIME_FIELD( )      \
    { }

#define STL_PARSER_EXTRACT_FIELD____TIME_ZONE_FIELD( )     \
    { }

/**
 * time_zone_field
 */
#define STL_PARSER_TIME_ZONE_FIELD____TIME_ZONE_HOUR( )      \
    { }

#define STL_PARSER_TIME_ZONE_FIELD____TIME_ZONE_MINUTE( )     \
    { }

/**
 * string_value_function
 */

#define STL_PARSER_STRING_VALUE_FUNCTION____( ) \
    { }


/**
 * character_value_function
 */

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____SUBSTRING( )     \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____SUBSTRB( )       \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____FOLD( )  \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____CONCATENATE( )  \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____TRIM( )  \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____OVERLAY( )       \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____PAD( )   \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____REPEAT( )   \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____REPLACE( )   \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____SPLITPART( ) \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____TRANSLATE( ) \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____TO_CHAR( )   \
    { }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____CHR( )       \
    { }

/**
 * character_substring_function
 */

#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____FROM( )      \
    { }

#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____FROM_FOR( )  \
    { }

#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____FROM_USING( )        \
    { }

#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____FROM_FOR_USING( )    \
    { }

#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____COMMA( )  \
    { }

#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____SINGLE_COMMA( )    \
    { }


/**
 * char_length_units
 */

#define STL_PARSER_CHAR_LENGTH_UNITS____CHARACTER( )    \
    { }

#define STL_PARSER_CHAR_LENGTH_UNITS____OCTET( )        \
    { }

/**
 * character_substrb_function : qlpFunction
 */

#define STL_PARSER_CHARACTER_SUBSTRB_FUNCTION____SINGLE_COMMA( ) \
    { }

#define STL_PARSER_CHARACTER_SUBSTRB_FUNCTION____COMMA( )        \
    { }

/**
 * fold
 */

#define STL_PARSER_FOLD____UPPER( )             \
    { }

#define STL_PARSER_FOLD____LOWER( )             \
    { }

#define STL_PARSER_FOLD____INITCAP( )             \
    { }

/**
 * concatenate_function
 */
#define STL_PARSER_CONCATENATE_FUNCTION____( )  \
    { }

/**
 * trim_function
 */

#define STL_PARSER_TRIM_FUNCTION____( )         \
    { }

/**
 * ltrim_function
 */
#define STL_PARSER_LTRIM_FUNCTION____DEFAULT( ) \
    { }

#define STL_PARSER_LTRIM_FUNCTION____( )        \
    { } 

/**
 * rtrim_function
 */
#define STL_PARSER_RTRIM_FUNCTION____DEFAULT( ) \
    { } 

#define STL_PARSER_RTRIM_FUNCTION____( )        \
    { }

/**
 * trim_operands
 */

#define STL_PARSER_TRIM_OPERANDS____EXPR( )     \
    { }

#define STL_PARSER_TRIM_OPERANDS____TRIM_FROM_EXPR( )   \
    { }

#define STL_PARSER_TRIM_OPERANDS____EXPR_FROM_EXPR( )   \
    { }

#define STL_PARSER_TRIM_OPERANDS____TRIM_EXPR_FROM_EXPR( )      \
    { }


/**
 * trim_specification
 */

#define STL_PARSER_TRIM_SPECIFICATION____LEADING( )     \
    { }

#define STL_PARSER_TRIM_SPECIFICATION____TRAILING( )    \
    { }

#define STL_PARSER_TRIM_SPECIFICATION____BOTH( )        \
    { }


/**
 * character_overlay_function
 */

#define STL_PARSER_CHARACTER_OVERLAY_FUNCTION____FROM( )        \
    { }

#define STL_PARSER_CHARACTER_OVERLAY_FUNCTION____FROM_FOR( )    \
    { }

/*
#define STL_PARSER_CHARACTER_OVERLAY_FUNCTION____FROM_USING( )  \
    { }
*/

/*
#define STL_PARSER_CHARACTER_OVERLAY_FUNCTION____FROM_FOR_USING( )      \
    { }
*/


/**
 * lpad_function
 */

#define STL_PARSER_LPAD_FUNCTION____BOTH( )         \
    { } 

#define STL_PARSER_LPAD_FUNCTION____BASIC( )         \
    { } 

/**
 * rpad_function
 */

#define STL_PARSER_RPAD_FUNCTION____BOTH( )        \
    { }

#define STL_PARSER_RPAD_FUNCTION____BASIC( )        \
    { }

/**
 * repeat_function
 */

#define STL_PARSER_REPEAT_FUNCTION____( )        \
    { }

/**
 * replace_function
 */

#define STL_PARSER_REPLACE_FUNCTION____BOTH( )        \
    { }

#define STL_PARSER_REPLACE_FUNCTION____BASIC( )        \
    { }

/**
 * split_part_function
 */

#define STL_PARSER_SPLITPART_FUNCTION____( )    \
    { }


/**
 * translate_function
 */

#define STL_PARSER_TRANSLATE_FUNCTION____( )    \
    { }

/**
 * to_char_function
 */
#define STL_PARSER_TO_CHAR_FUNCTION____DEFAULT( )  \
    { }

#define STL_PARSER_TO_CHAR_FUNCTION____( )         \
    { }

/**
 * chr_function
 */
#define STL_PARSER_CHR_FUNCTION____( )             \
    { }

/**
 * time_zone
 */

#define STL_PARSER_TIME_ZONE____( )             \
    { }


/**
 * time_zone_specifier
 */

#define STL_PARSER_TIME_ZONE_SPECIFIER____LOCAL( )      \
    { }

#define STL_PARSER_TIME_ZONE_SPECIFIER____TIME_ZONE( )  \
    { }


/**
 * interval_qualifier
 */

#define STL_PARSER_INTERVAL_QUALIFIER____START_TO_END( )        \
    { }

#define STL_PARSER_INTERVAL_QUALIFIER____SINGLE_DATETIME( )     \
    { }


/**
 * start_field
 */

#define STL_PARSER_START_FIELD____NON_SECOND( ) \
    { }

#define STL_PARSER_START_FIELD____NON_SECOND_INTERVAL( )        \
    { }


/**
 * end_field
 */

#define STL_PARSER_END_FIELD____NON_SECOND( )   \
    { }

#define STL_PARSER_END_FIELD____TO_SECOND( )    \
    { }

#define STL_PARSER_END_FIELD____TO_SECOND_INTERVAL( )   \
    { }


/**
 * single_datetime_field
 */

#define STL_PARSER_SINGLE_DATETIME_FIELD____NON_SECOND( )       \
    { }

#define STL_PARSER_SINGLE_DATETIME_FIELD____NON_SECOND_INTERVAL( )      \
    { }

#define STL_PARSER_SINGLE_DATETIME_FIELD____SECOND( )   \
    { }

#define STL_PARSER_SINGLE_DATETIME_FIELD____SECOND_INTERVAL( )  \
    { }

#define STL_PARSER_SINGLE_DATETIME_FIELD____SECOND_INTERVAL_INTERVAL( ) \
    { }


/**
 * primary_datetime_field
 */

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD( ) \
    { }

#define STL_PARSER_PRIMARY_DATETIME_FIELD____SECOND( ) \
    { }

/**
 * non_second_primary_datetime_field
 */

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD____YEAR( ) \
    { }

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD____MONTH( )        \
    { }

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD____DAY( )  \
    { }

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD____HOUR( ) \
    { }

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD____MINUTE( )       \
    { }


/**
 * interval_fractional_seconds_precision
 */

#define STL_PARSER_INTERVAL_FRACTIONAL_SECONDS_PRECISION____( ) \
    { }


/**
 * interval_leading_field_precision
 */

#define STL_PARSER_INTERVAL_LEADING_FIELD_PRECISION____( )      \
    { }


/**
 * datetime_value_function
 */

#define STL_PARSER_DATETIME_VALUE_FUNCTION____SYSDATE( )        \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____SYSTIME( )        \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____SYSTIMESTAMP( )   \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____CLOCK_DATE( )   \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____STATEMENT_DATE( )   \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TRANSACTION_DATE( )   \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____CLOCK_TIME( )   \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____STATEMENT_TIME( )   \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TRANSACTION_TIME( )   \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____CLOCK_TIMESTAMP( )  \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____STATEMENT_TIMESTAMP( )  \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TRANSACTION_TIMESTAMP( )  \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____CLOCK_LOCALTIME( )  \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____STATEMENT_LOCALTIME( )  \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TRANSACTION_LOCALTIME( )  \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____CLOCK_LOCALTIMESTAMP( )  \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____STATEMENT_LOCALTIMESTAMP( )  \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TRANSACTION_LOCALTIMESTAMP( )  \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____ADDDATE( )    \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____ADDTIME( )    \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____DATEADD( )    \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____DATE_ADD( )   \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TO_DATE( )    \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TO_TIME( )    \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TO_TIME_WITH_TIME_ZONE( )    \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TO_TIMESTAMP( )    \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TO_TIMESTAMP_WITH_TIME_ZONE( )    \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____LAST_DAY( )                    \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____ADD_MONTHS( )                  \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____FROM_UNIXTIME( )   \
    { }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____DATEDIFF()         \
    { }

/**
 * time_precision
 */

#define STL_PARSER_TIME_PRECISION____( )        \
    { }


/**
 * timestamp_precision
 */

#define STL_PARSER_TIMESTAMP_PRECISION____( )   \
    { }


/**
 * time_fractional_seconds_precision
 */

#define STL_PARSER_TIME_FRACTIONAL_SECONDS_PRECISION____( )     \
    { }


/**
 * adddate_function
 */
#define STL_PARSER_ADDDATE_FUNCTION____( )                                 \
    { }

/**
 * addtime_function
 */
#define STL_PARSER_ADDTIME_FUNCTION____( )                                 \
    { }

/**
 * dateadd_function
 */
#define STL_PARSER_DATETIME_FUNCTION____DATEADD( )                         \
    { }

/**
 * datetime_part
 */
#define STL_PARSER_DATETIME_PART____DATETIME_PART( )                       \
    { }

/**
 * datetime_part_field
 */
#define STL_PARSER_DATETIME_PART____PRIMARY_DATETIME_FIELD( )              \
    { }

#define STL_PARSER_DATETIME_PART____QUARTER( )                             \
    { }

#define STL_PARSER_DATETIME_PART____DAYOFYEAR( )                           \
    { }

#define STL_PARSER_DATETIME_PART____WEEK( )                                \
    { }

#define STL_PARSER_DATETIME_PART____WEEKDAY( )                             \
    { }

#define STL_PARSER_DATETIME_PART____MILLISECOND( )                         \
    { }

#define STL_PARSER_DATETIME_PART____MICROSECOND( )                         \
    { }

/**
 * date_add_function
 */
#define STL_PARSER_DATE_ADD_FUNCTION____( )                                \
    { }

/**
 * to_date_function
 */
#define STL_PARSER_TO_DATE_FUNCTION____DEFAULT( )                          \
    { }

#define STL_PARSER_TO_DATE_FUNCTION____( )                                 \
    { }

/**
 * to_time_function
 */
#define STL_PARSER_TO_TIME_FUNCTION____DEFAULT( )                          \
    { }

#define STL_PARSER_TO_TIME_FUNCTION____( )                                 \
    { }

/**
 * to_time_with_time_zone_function
 */
#define STL_PARSER_TO_TIME_WITH_TIME_ZONE_FUNCTION____DEFAULT( )           \
    { }

#define STL_PARSER_TO_TIME_WITH_TIME_ZONE_FUNCTION____( )                  \
    { }

/**
 * to_timestamp_function
 */
#define STL_PARSER_TO_TIMESTAMP_FUNCTION____DEFAULT( )                          \
    { }

#define STL_PARSER_TO_TIMESTAMP_FUNCTION____( )                                 \
    { }

/**
 * to_timestamp_with_time_zone_function
 */
#define STL_PARSER_TO_TIMESTAMP_WITH_TIME_ZONE_FUNCTION____DEFAULT( )           \
    { }

#define STL_PARSER_TO_TIMESTAMP_WITH_TIME_ZONE_FUNCTION____( )                  \
    { }

/**
 * last_day_function
 */
#define STL_PARSER_LAST_DAY_FUNCTION____( )    \
    { }

/**
 * add_months_function
 */
#define STL_PARSER_ADD_MONTHS_FUNCTION____( )  \
    { }

/**
 * from_unixtime_function
 */
#define STL_PARSER_FROM_UNIXTIME_FUNCTION____( )        \
    { } 

/**
 * datediff_function
 */
#define STL_PARSER_DATEDIFF_FUNCTION____( )             \
    { }

/**
 * nvl_function
 */
#define STL_PARSER_NVL_FUNCTION____( )                                      \
    { }

#define STL_PARSER_NVL2_FUNCTION____( )                                     \
    { }

/**
 * dump_function
 */
#define STL_PARSER_DUMP_FUNCTION____( )         \
    { }

/**
 * decode_expr_list
 */

#define STL_PARSER_DECODE_LIST____DECODE_FUNC( )       \
    { }

#define STL_PARSER_DECODE_LIST____DECODE_FUNC_LIST( )  \
    { }

/**
 * decode_function
 */

#define STL_PARSER_DECODE_FUNCTION____( )       \
    { }

/**
 * case2_expr_list
 */

#define STL_PARSER_CASE2_LIST____CASE2_FUNC( )       \
    { }

#define STL_PARSER_CASE2_LIST____CASE2_FUNC_LIST( )  \
    { }

/**
 * case2_function
 */

#define STL_PARSER_CASE2_FUNCTION____( )       \
    { }

/**
 * general_comparison_function
 */

#define STL_PARSER_GENERAL_COMPARISON_FUNCTION____GREATEST_FUNC( )  \
    { }

#define STL_PARSER_GENERAL_COMPARISON_FUNCTION____LEAST_FUNC( )     \
    { }

/**
 * general_comparison_function:
 */

#define STL_PARSER_GENERAL_COMPARISON_FUNCTION_ARG_LIST____GENERAL_COMPARISON_FUNC( )   \
    { }

#define STL_PARSER_GENERAL_COMPARISON_FUNCTION_ARG_LIST____GENERAL_COMPARISON_FUNC_LIST( ) \
    { }

/**
 * boolean_value_expression
 */

#define STL_PARSER_BOOLEAN_VALUE_EXPRESSION____SINGLE( )        \
    { }

#define STL_PARSER_BOOLEAN_VALUE_EXPRESSION____OR( )    \
    { }


/**
 * boolean_term
 */

#define STL_PARSER_BOOLEAN_TERM____BOOLEAN( )   \
    { }

#define STL_PARSER_BOOLEAN_TERM____BOOLEAN_AND_BOOLEAN( )       \
    { }


/**
 * boolean_comparison_predicate : qllNode
 */

#define STL_PARSER_BOOLEAN_COMPARISON_PREDICATE____VALUE()      \
    { }

#define STL_PARSER_BOOLEAN_COMPARISON_PREDICATE____FUNCTION()   \
    { }


/**
 * boolean_factor
 */

#define STL_PARSER_BOOLEAN_FACTOR____BOOLEAN( ) \
    { }

#define STL_PARSER_BOOLEAN_FACTOR____NOT_BOOLEAN( )     \
    { }


/**
 * boolean_test
 */
#define STL_PARSER_BOOLEAN_TEST____BOOLEAN( )   \
    {                                           \
    }
                                                    
#define STL_PARSER_BOOLEAN_TEST____BOOLEAN_IS_TRUTH( )  \
    {                                                   \
    }

#define STL_PARSER_BOOLEAN_TEST____BOOLEAN_IS_NOT_TRUTH( )      \
    {                                                           \
    }


/**
 * boolean_TEST
 */

#define STL_PARSER_BOOLEAN_TEST____BOOLEAN( )  \
    { }

#define STL_PARSER_BOOLEAN_TEST____BOOLEAN_IS_TRUTH( ) \
    { }

#define STL_PARSER_BOOLEAN_TEST____BOOLEAN_IS_NOT_TRUTH( )     \
    { }

#define STL_PARSER_BOOLEAN_TEST____BOOLEAN_IS_NULL( ) \
    { }

#define STL_PARSER_BOOLEAN_TEST____BOOLEAN_IS_NOT_NULL( )     \
    { }


/**
 * truth_value
 */

#define STL_PARSER_TRUTH_VALUE____TRUE( )       \
    { }

#define STL_PARSER_TRUTH_VALUE____FALSE( )      \
    { }

#define STL_PARSER_TRUTH_VALUE____UNKNOWN( )    \
    { }


/**
 * boolean_primary
 */

#define STL_PARSER_BOOLEAN_PRIMARY____LITERAL( )      \
    { }

#define STL_PARSER_BOOLEAN_PRIMARY____PREDICATE( )      \
    { }

#define STL_PARSER_BOOLEAN_PRIMARY____BOOLEAN_PREDICATE( )      \
    { }


/**
 * boolean_predicand
 */

#define STL_PARSER_BOOLEAN_PREDICAND____( )     \
    { }


/**
 * parenthesized_boolean_value_expression
 */

#define STL_PARSER_PARENTHESIZED_BOOLEAN_VALUE_EXPRESSION____( )        \
    { }


/**
 * nonparenthesized_value_expression_primary
 */

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____VALUE( )        \
    { }

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____CAST( ) \
    { }


/**
 * unsigned_value_specification
 */

#define STL_PARSER_UNSIGNED_VALUE_SPECIFICATION____LITERAL( )   \
    { }

#define STL_PARSER_UNSIGNED_VALUE_SPECIFICATION____VALUE( )     \
    { }


/**
 * unsigned_literal
 */

#define STL_PARSER_UNSIGNED_LITERAL____NUMERIC_LITERAL( )       \
    { }

#define STL_PARSER_UNSIGNED_LITERAL____GENERAL_LITERAL( )       \
    { }


/**
 * unsigned_numeric_literal
 */

#define STL_PARSER_UNSIGNED_NUMERIC_LITERAL____EXACT( ) \
    { }

#define STL_PARSER_UNSIGNED_NUMERIC_LITERAL____APPROXIMATE( )   \
    { }


/**
 * exact_numeric_literal
 */

#define STL_PARSER_EXACT_NUMERIC_LITERAL____UNSIGNED_INTEGER( ) \
    { }

#define STL_PARSER_EXACT_NUMERIC_LITERAL____EXACT_NUMERIC( )    \
    { }


/**
 * approximate_numeric_literal
 */

#define STL_PARSER_APPROXIMATE_NUMERIC_LITERAL____( )   \
    { }


/**
 * signed_integer
 */

#define STL_PARSER_SIGNED_INTEGER____UNSIGNED_INTERGER( )       \
    { }

#define STL_PARSER_SIGNED_INTEGER____SIGNED_INTERGER( )       \
    { }


/**
 * unsigned_integer
 */

#define STL_PARSER_UNSIGNED_INTEGER____( )      \
    { }


/**
 * general_literal
 */

#define STL_PARSER_GENERAL_LITERAL____CHARACTER( )      \
    { }

#define STL_PARSER_GENERAL_LITERAL____BINARY( ) \
    { }

#define STL_PARSER_GENERAL_LITERAL____DATETIME( )       \
    { }

#define STL_PARSER_GENERAL_LITERAL____INTERVAL( )       \
    { }

#define STL_PARSER_GENERAL_LITERAL____BOOLEAN( )        \
    { }

#define STL_PARSER_GENERAL_LITERAL____NULL( )        \
    { }


/**
 * character_string_literal
 */

#define STL_PARSER_CHARACTER_STRING_LITERAL____STRING( )        \
    { }

#define STL_PARSER_CHARACTER_STRING_LITERAL____INTRODUCER_STRING( )     \
    { }


/**
 * quote_character_representation_list
 */

#define STL_PARSER_QUOTE_CHARACTER_REPRESENTATION_LIST____STRING( )     \
    { }

#define STL_PARSER_QUOTE_CHARACTER_REPRESENTATION_LIST____STRING_LIST( )        \
    { }


/**
 * quote_character_representation
 */

#define STL_PARSER_QUOTE_CHARACTER_REPRESENTATION____STRING( )  \
    { }


/**
 * separator
 */

#define STL_PARSER_SEPARATOR____( )             \
    { }


/**
 * separator_2
 */

#define STL_PARSER_SEPARATOR_2____( )           \
    { }


/**
 * introducer
 */

#define STL_PARSER_INTRODUCER____( )            \
    { }


/**
 * character_set_specification
 */

#define STL_PARSER_CHARACTER_SET_SPECIFICATION____( )   \
    { }


/**
 * standard_character_set_name
 */

#define STL_PARSER_STANDARD_CHARACTER_SET_NAME____( )   \
    { }


/**
 * character_set_name
 */

#define STL_PARSER_CHARACTER_SET_NAME____( )    \
    { }


/**
 * SQL_language_identifier
 */

#define STL_PARSER_SQL_LANGUAGE_IDENTIFIER____( )       \
    { }


/**
 * column_name
 */

#define STL_PARSER_COLUMN_NAME____( )           \
    { }


/**
 * identifier_chain
 */

#define STL_PARSER_IDENTIFIER_CHAIN____IDENTIFIER( )    \
    { }

#define STL_PARSER_IDENTIFIER_CHAIN____IDENTIFIER_LIST( )        \
    { }

/**
 * rowid_identifier_chain
 */

#define STL_PARSER_IDENTIFIER_CHAIN____ROWID_IDENTIFIER( )       \
    { }

#define STL_PARSER_IDENTIFIER_CHAIN____ROWID_IDENTIFIER_LIST( )   \
    { }

/**
 * rowid_pseudo_column
 */

#define STL_PARSER_REGUALAR_IDENTIFIER____ROWID_IDENTIFIER( )  \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }

/**
 * identifier
 */

#define STL_PARSER_IDENTIFIER____( )            \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * actual_identifier
 */

#define STL_PARSER_ACTUAL_IDENTIFIER____REGULAR( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1);                     \
    }

#define STL_PARSER_ACTUAL_IDENTIFIER____DELIMITED( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1);                     \
    }


/**
 * regualar_identifier
 */

#define STL_PARSER_REGUALAR_IDENTIFIER____IDENTIFIER( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_REGUALAR_IDENTIFIER____NON_RESERVED_WORD( )  \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_REGUALAR_IDENTIFIER____QSQL_KEYWORD( )   \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

/**
 * delimited_identifier
 */

#define STL_PARSER_DELIMITED_IDENTIFIER____( )  \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }



/**
 * cast_specification
 */

#define STL_PARSER_CAST_SPECIFICATION____( )  \
    { }


/**
 * cast_operand
 */

#define STL_PARSER_CAST_OPERAND____VALUE_EXPR( )        \
    { }

#define STL_PARSER_CAST_OPERAND____IMPLICITLY_TYPED_VALUE( )    \
    { }


/**
 * cast_target
 */

#define STL_PARSER_CAST_TARGET____( )           \
    { }


/**
 * implicitly_typed_value_specification
 */

#define STL_PARSER_IMPLICITLY_TYPED_VALUE_SPECIFICATION____NULL( )      \
    { }

#define STL_PARSER_IMPLICITLY_TYPED_VALUE_SPECIFICATION____PAREN_NULL( )        \
    { }


/**
 * data_type
 */

#define STL_PARSER_DATA_TYPE____( )             \
    { }


/**
 * predefined_type
 */

#define STL_PARSER_PREDEFINED_TYPE____CHARACTER( )      \
    { }

#define STL_PARSER_PREDEFINED_TYPE____CHARACTER_CHARACTER_SET( )        \
    { }

#define STL_PARSER_PREDEFINED_TYPE____NATIONAL_CHARACTER( )     \
    { }

#define STL_PARSER_PREDEFINED_TYPE____BINARY( ) \
    { }

#define STL_PARSER_PREDEFINED_TYPE____NUMERIC( )        \
    { }

#define STL_PARSER_PREDEFINED_TYPE____BOOLEAN( )        \
    { }

#define STL_PARSER_PREDEFINED_TYPE____DATETIME( )       \
    { }

#define STL_PARSER_PREDEFINED_TYPE____INTERVAL( )       \
    { }

#define STL_PARSER_PREDEFINED_TYPE____ROWID( )       \
    { }


/**
 * character_string_type
 */

#define STL_PARSER_CHARACTER_STRING_TYPE____CHAR( )     \
    { }

#define STL_PARSER_CHARACTER_STRING_TYPE____CHAR_LENGTH( )      \
    { }

#define STL_PARSER_CHARACTER_STRING_TYPE____VARCHAR_NON_PRECISION( )    \
    { }

#define STL_PARSER_CHARACTER_STRING_TYPE____CHAR_VARYING_NON_PRECISION( )   \
    { }

#define STL_PARSER_CHARACTER_STRING_TYPE____CHAR_VARYING_LENGTH( )      \
    { }

#define STL_PARSER_CHARACTER_STRING_TYPE____VARCHAR_LENGTH( )   \
    { }

#define STL_PARSER_CHARACTER_STRING_TYPE____LONG_CHAR_VARYING_LENGTH( ) \
    { }

#define STL_PARSER_CHARACTER_STRING_TYPE____LONG_VARCHAR_LENGTH( )      \
    { }



/**
 * character_length
 */

#define STL_PARSER_CHARACTER_LENGTH____LENGTH( )        \
    { }

#define STL_PARSER_CHARACTER_LENGTH____LENGTH_UNIT( )   \
    { }


/**
 * national_character_string_type
 */

#define STL_PARSER_NATIONAL_CHARACTER_STRING_TYPE____( )        \
    { }


/**
 * binary_string_type
 */

#define STL_PARSER_BINARY_STRING_TYPE____BINARY( )      \
    { }

#define STL_PARSER_BINARY_STRING_TYPE____BINARY_LENGTH( )       \
    { }

#define STL_PARSER_BINARY_STRING_TYPE____VARBINARY_NON_PRECISION( )         \
    { }

#define STL_PARSER_BINARY_STRING_TYPE____BINARY_VARYING_NON_PRECISION( )    \
    { }

#define STL_PARSER_BINARY_STRING_TYPE____BINARY_VARYING_LENGTH( )       \
    { }

#define STL_PARSER_BINARY_STRING_TYPE____VARBINARY_LENGTH( )    \
    { }

#define STL_PARSER_BINARY_STRING_TYPE____LONG_VARBINARY_LENGTH( )       \
    { }

#define STL_PARSER_BINARY_STRING_TYPE____LONG_BINARY_VARYING_LENGTH( )  \
    { }


/**
 * numeric_type
 */

#define STL_PARSER_NUMERIC_TYPE____EXACT( )     \
    { }

#define STL_PARSER_NUMERIC_TYPE____APPROXIMATE( )       \
    { }

#define STL_PARSER_NUMERIC_TYPE____NATIVE( )       \
    { }


/**
 * exact_numeric_type
 */

#define STL_PARSER_EXACT_NUMERIC_TYPE____NUMERIC( )     \
    { }

#define STL_PARSER_EXACT_NUMERIC_TYPE____NUMBER( )     \
    { }

#define STL_PARSER_EXACT_NUMERIC_TYPE____DECIMAL_SMALLINT( )    \
    { }

#define STL_PARSER_EXACT_NUMERIC_TYPE____DECIMAL_INTEGER( )     \
    { }

#define STL_PARSER_EXACT_NUMERIC_TYPE____DECIMAL_BIGINT( )      \
    { }

#define STL_PARSER_EXACT_NUMERIC_TYPE____NUMERIC_PRECISION( )   \
    { }

#define STL_PARSER_EXACT_NUMERIC_TYPE____NUMERIC_PRECISION_SCALE( )     \
    { }


/**
 * precision
 */

#define STL_PARSER_PRECISION____( )             \
    { }


/**
 * scale
 */

#define STL_PARSER_SCALE____( )                 \
    { }


/**
 * approximate_numeric_type
 */

#define STL_PARSER_APPROXIMATE_NUMERIC_TYPE____FLOAT() \
    { }

#define STL_PARSER_APPROXIMATE_NUMERIC_TYPE____FLOAT_PRECISION() \
    { }

#define STL_PARSER_APPROXIMATE_NUMERIC_TYPE____DECIMAL_REAL( )  \
    { }

#define STL_PARSER_APPROXIMATE_NUMERIC_TYPE____DECIMAL_DOUBLE_PRECISION( )        \
    { }

#define STL_PARSER_APPROXIMATE_NUMERIC_TYPE____DECIMAL_DOUBLE( )        \
    { }

/**
 * native_numeric_type
 */

#define STL_PARSER_NATIVE_NUMERIC_TYPE____NATIVE_SMALLINT() \
    { }

#define STL_PARSER_NATIVE_NUMERIC_TYPE____NATIVE_INTEGER() \
    { }

#define STL_PARSER_NATIVE_NUMERIC_TYPE____NATIVE_BIGINT() \
    { }

#define STL_PARSER_NATIVE_NUMERIC_TYPE____NATIVE_REAL() \
    { }

#define STL_PARSER_NATIVE_NUMERIC_TYPE____NATIVE_DOUBLE() \
    { }

/**
 * length
 */

#define STL_PARSER_LENGTH____( )                \
    { }


/**
 * boolean_type
 */

#define STL_PARSER_BOOLEAN_TYPE____( )          \
    { }


/**
 * datetime_type
 */

#define STL_PARSER_DATETIME_TYPE____DATE( )     \
    { }

#define STL_PARSER_DATETIME_TYPE____TIME( )     \
    { }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP( )        \
    { }

#define STL_PARSER_DATETIME_TYPE____TIME_PRECISION( )   \
    { }

#define STL_PARSER_DATETIME_TYPE____TIME_WITH( )        \
    { }

#define STL_PARSER_DATETIME_TYPE____TIME_WITHOUT( )        \
    { }

#define STL_PARSER_DATETIME_TYPE____TIME_PRECISION_WITH( )      \
    { }

#define STL_PARSER_DATETIME_TYPE____TIME_PRECISION_WITHOUT( )      \
    { }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP_PRECISION( )      \
    { }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP_WITH( )   \
    { }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP_WITHOUT( )   \
    { }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP_PRECISION_WITH( ) \
    { }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP_PRECISION_WITHOUT( ) \
    { }


/**
 * interval_type
 */

#define STL_PARSER_INTERVAL_TYPE____( )         \
    { }

/**
 * rowid_type
 */

#define STL_PARSER_ROWID_TYPE____( )            \
    { }

/**
 * binary_string_literal
 */

#define STL_PARSER_BINARY_STRING_LITERAL____( ) \
    { }


/**
 * datetime_literal
 */

#define STL_PARSER_DATETIME_LITERAL____DATE( )  \
    { }

#define STL_PARSER_DATETIME_LITERAL____TIME( )  \
    { }

#define STL_PARSER_DATETIME_LITERAL____TIMESTAMP( )     \
    { }


/**
 * date_literal
 */

#define STL_PARSER_DATE_LITERAL____( )          \
    { }

#define STL_PARSER_DATE_LITERAL____ESCAPE_SEQUENCE_DATE( )                       \
    { }


/**
 * time_literal
 */

#define STL_PARSER_TIME_LITERAL____( )          \
    { }

#define STL_PARSER_TIME_LITERAL____ESCAPE_SEQUENCE_TIME( )                       \
    { }

#define STL_PARSER_TIME_WITH_LITERAL____( )          \
    { }

/**
 * timestamp_literal
 */

#define STL_PARSER_TIMESTAMP_LITERAL____( )     \
    { }

#define STL_PARSER_TIMESTAMP_LITERAL____ESCAPE_SEQUENCE_TIMESTAMP( )               \
    { }

#define STL_PARSER_TIMESTAMP_WITH_LITERAL____( )     \
    { }


/**
 * interval_literal
 */

#define STL_PARSER_INTERVAL_LITERAL____INTERVAL( )      \
    { }

#define STL_PARSER_INTERVAL_LITERAL____SIGN_INTERVAL( ) \
    { }


/**
 * interval_string
 */

#define STL_PARSER_INTERVAL_STRING____( )       \
    { }


/**
 * unquoted_interval_string
 */

#define STL_PARSER_UNQUOTED_INTERVAL_STRING____YEAR_MONTH( )    \
    { }

#define STL_PARSER_UNQUOTED_INTERVAL_STRING____DAY_TIME( )      \
    { }

#define STL_PARSER_UNQUOTED_INTERVAL_STRING____SIGN_YEAR_MONTH( )       \
    { }

#define STL_PARSER_UNQUOTED_INTERVAL_STRING____SIGN_DAY_TIME( ) \
    { }


/**
 * year_month_literal
 */

#define STL_PARSER_YEAR_MONTH_LITERAL____YEAR_MONTH( )  \
    { }


/**
 * day_time_literal
 */

#define STL_PARSER_DAY_TIME_LITERAL____DAY_TIME( )      \
    { }

#define STL_PARSER_DAY_TIME_LITERAL____TIME( )  \
    { }


/**
 * day_time_interval
 */

#define STL_PARSER_DAY_TIME_INTERVAL____DAY( )  \
    { }

#define STL_PARSER_DAY_TIME_INTERVAL____DAY_HOUR( )     \
    { }

#define STL_PARSER_DAY_TIME_INTERVAL____DAY_HOUR_MINUTE( )      \
    { }

#define STL_PARSER_DAY_TIME_INTERVAL____DAY_HOUR_MINUTE_SECOND( )       \
    { }


/**
 * time_interval
 */

#define STL_PARSER_TIME_INTERVAL____HOUR_MINUTE( )      \
    { }

#define STL_PARSER_TIME_INTERVAL____HOUR_MINUTE_SECOND( )       \
    { }


/**
 * seconds_value
 */

#define STL_PARSER_SECONDS_VALUE____INTEGER( )  \
    { }

#define STL_PARSER_SECONDS_VALUE____INTEGER_FRACTION( ) \
    { }


/**
 * seconds_integer_value
 */

#define STL_PARSER_SECONDS_INTEGER_VALUE____( ) \
    { }


/**
 * seconds_fraction
 */

#define STL_PARSER_SECONDS_FRACTION____( )      \
    { }


/**
 * datetime_value
 */

#define STL_PARSER_DATETIME_VALUE____( )        \
    { }


/**
 * boolean_literal
 */

#define STL_PARSER_BOOLEAN_LITERAL____TRUE( )   \
    { }

#define STL_PARSER_BOOLEAN_LITERAL____FALSE( )  \
    { }

#define STL_PARSER_BOOLEAN_LITERAL____UNKNOWN( )        \
    { }


/**
 * row_value_predicand
 */
#define STL_PARSER_ROW_VALUE_PREDICAND____CONSTRUCTOR_VALUE( )  \
    { }

#define STL_PARSER_ROW_VALUE_PREDICAND____CONSTRUCTOR_NONPARENT_VALUE( )  \
    { }


/**
 * row_value_constructor_predicate
 */
#define STL_PARSER_ROW_VALUE_CONSTRUCTOR____( ) \
    { }

#define STL_PARSER_ROW_VALUE_CONSTRUCTOR____EXPLICIT_VALUE( )   \
    { }

/**
 * explicit_row_value_constructor
 */

#define STL_PARSER_ROW_VALUE____CONSTRUCTOR( )                \
    { }


/**
 * row_value_constructor_element_list
 */

#define STL_PARSER_ROW_VALUE_CONSTRUCTOR_LIST____ELEMENT( )       \
    { }

#define STL_PARSER_ROW_VALUE_CONSTRUCTOR_LIST____ELEMENT_LIST( )  \
    { }


/**
 * row_value_constructor_element
 */

#define STL_PARSER_ROW_VALUE_CONSTRUCTOR____ELEMENT( )        \
    { }


/**
 * general_value_specification
 */

#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____HOST_PARAMETER( ) \
    {                                                               \
        STL_PARSER_CHECK( v0 = ztqAddInHostVariable( param,         \
                                                     v1,            \
                                                     v2 ) );        \
    }

#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____COLUMN( )      \
    { }

#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____COLUMN_WITH_OUTER_JOIN_OPERATOR( )    \
    { }

#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____ROWID_COLUMN( )       \
    { }                                                                 

#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____ROWID_COLUMN_WITH_OUTER_JOIN_OPERATOR( )  \
    { }

#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____DYNAMIC_PARAMETER( )  \
    {                                                                   \
        STL_PARSER_ERROR( NULL, param, param->mScanner, NULL );         \
        YYABORT;                                                        \
    }


/**
 * indicator_paramter
 */

#define STL_PARSER_INDICATOR_PARAMTER____NO_INDICATOR( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = NULL );                          \
    }

#define STL_PARSER_INDICATOR_PARAMTER____EMPTY_INDICATOR( )         \
    {                                                               \
        STL_PARSER_CHECK( v0 = ztqMakeHostIndicator( param, v1 ) ); \
    }

#define STL_PARSER_INDICATOR_PARAMTER____INDICATOR( )               \
    {                                                               \
        STL_PARSER_CHECK( v0 = ztqMakeHostIndicator( param, v2 ) ); \
    }


/**
 * predicate
 */

#define STL_PARSER_PREDICATE____COMPARISON( )   \
    { }

#define STL_PARSER_PREDICATE____BETWEEN( )      \
    { }

#define STL_PARSER_PREDICATE____LIKE( )         \
    { }

#define STL_PARSER_PREDICATE____IN( )           \
    { }

#define STL_PARSER_PREDICATE____NULL( )         \
    { }


/**
 * comparison_predicate
 */

#define STL_PARSER_COMPARISON_PREDICATE____VALUE_VALUE( )       \
    { }

#define STL_PARSER_COMPARISON_PREDICATE____VALUE_ROW( ) \
    { }

#define STL_PARSER_COMPARISON_PREDICATE____ROW_VALUE( ) \
    { }

#define STL_PARSER_COMPARISON_PREDICATE____ROW_ROW( )   \
    { }


/**
 * comparison_predicate_part_2
 */

#define STL_PARSER_COMP_PREDICATE_PART_2____( )  \
    { }


/**
 * row_predicate_value
 */

#define STL_PARSER_ROW_PREDICATE____PREDICATE_VALUE( )  \
    { }


/**
 * comp_op
 */

#define STL_PARSER_COMP_OP____EQUAL( )          \
    { }

#define STL_PARSER_COMP_OP____NOT_EQUAL( )      \
    { }

#define STL_PARSER_COMP_OP____LESS_THAN( )      \
    { }

#define STL_PARSER_COMP_OP____LESS_THAN_EQUAL( )        \
    { }

#define STL_PARSER_COMP_OP____GREATER_THAN( )   \
    { }

#define STL_PARSER_COMP_OP____GREATER_THAN_EQUAL( )     \
    { }


/**
 * between_predicate
 */

#define STL_PARSER_BETWEEN_PREDICATE____( )     \
    { }

#define STL_PARSER_NOT_BETWEEN_PREDICATE____( )     \
    { }

#define STL_PARSER_BETWEEN_PREDICATE____REWRITE_ASYM() \
    { }

#define STL_PARSER_NOT_BETWEEN_PREDICATE____REWRITE_ASYM()  \
    { }

#define STL_PARSER_BETWEEN_REWRITE_ASYMETRIC_PREDICATE____WITHOUT_ASYM() \
    { }

#define STL_PARSER_BETWEEN_REWRITE_ASYMETRIC_PREDICATE____WITH_ASYM()   \
    { }

/**
 * between_predicate_part_2
 */

#define STL_PARSER_BETWEEN_PREDICATE_PART_2____BETWEEN( )       \
    { }


/**
 * like_predicate
 */

#define STL_PARSER_LIKE_PREDICATE____( )        \
    { }


/**
 * character_like_predicate
 */

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____( )      \
    { }


/**
 * character_like_predicate_part_2
 */

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____LIKE( )  \
    { }

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____NOT_LIKE( )      \
    { }

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____LIKE_ESCAPE( )   \
    { }

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____NOT_LIKE_ESCAPE( )       \
    { }

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____LIKE_ESCAPE_SEQUENCE( )   \
    { }

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____NOT_LIKE_ESCAPE_SEQUENCE( )       \
    { }


/**
 * in_predicate
 */
#define STL_PARSER_IN_PREDICATE____( )          \
    { }

#define STL_PARSER_IN_PREDICATE____LIST( )          \
    { }

/**
 * in_predicate_part_2
 */
#define STL_PARSER_IN_PREDICATE____IN( )        \
    { }

#define STL_PARSER_IN_PREDICATE____NOT_IN( )    \
    { }

/**
 * in_predicate_value
 */
#define STL_PARSER_IN_PREDICATE____PREDICATE_VALUE( )   \
    { }

#define STL_PARSER_IN_PREDICATE____TABLE_SUBQUERY( )    \
    { }

#define STL_PARSER_IN_PREDICATE____TABLE_SUBQUERY_PREDICATE_VALUE( )    \
    { }

/**
 * in_value_list
 */

#define STL_PARSER_IN_PREDICATE____IN_SINGLE( )    \
    { }

#define STL_PARSER_IN_PREDICATE____IN_LIST( )  \
    { }

/**
 * row_list
 */

#define STL_PARSER_IN_PREDICATE____ROW_SINGLE( )    \
    { }

#define STL_PARSER_IN_PREDICATE____ROW_LIST( )  \
    { }


/**
 * exists_predicate
 */

#define STL_PARSER_EXISTS_PREDICATE____( )      \
    { }


/**
 * quantified_comparison_predicate
 */
#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____( ) \
    { }

#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____LIST( ) \
    { }


/**
 * quantified_comparison_predicate_part_2
 */
#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____PART_2( )   \
    { }

/**
 * quantifier
 */
#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____QUANTIFIER_ALL( )   \
    { }

#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____QUANTIFIER_SOME( )  \
    { }

/**
 * all
 */
#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____ALL( )  \
    { }

/**
 * some
 */
#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____SOME( ) \
    { }

#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____ANY( )  \
    { }

/**
 * concatenation
 */

#define STL_PARSER_CONCATENATION____SINGLE( )   \
    { }

#define STL_PARSER_CONCATENATION____LIST( )     \
    { }


/**
 * null_predicate
 */

#define STL_PARSER_NULL_PREDICATE____IS_NULL( )         \
    { }

#define STL_PARSER_NULL_PREDICATE____IS_NOT_NULL( )         \
    { }


/**
 * table_primary
 */

#define STL_PARSER_TABLE_PRIMARY____NAME( )     \
    { }

#define STL_PARSER_TABLE_PRIMARY____NAME_ALIAS_WITH_AS( )       \
    { }

#define STL_PARSER_TABLE_PRIMARY____NAME_ALIAS_WITH_BLANK( )    \
    { }


/**
 * correlation_name
 */

#define STL_PARSER_CORRELATION_NAME____( )      \
    { }


/**
 * savepoint_statement
 */

#define STL_PARSER_SAVEPOINT_STATEMENT____( )       \
    { }

/**
 * release_savepoint_statement
 */

#define STL_PARSER_RELEASE_SAVEPOINT_STATEMENT____( )       \
    { }


/**
 * commit_statement
 */

#define STL_PARSER_COMMIT_STATEMENT____( )      \
    { }

#define STL_PARSER_COMMIT_STATEMENT____WRITE_MODE( )    \
    { }

#define STL_PARSER_COMMIT_STATEMENT____FORCE( ) \
    { }

#define STL_PARSER_COMMIT_STATEMENT____RELEASE( )   \
    {                                               \
        STL_PARSER_NOT_SUPPORT( a3 );               \
    }


/**
 * comment_option
 */

#define STL_PARSER_COMMENT_OPTION____( )        \
    { }


/**
 * commit_write_mode
 */

#define STL_PARSER_COMMIT_WRITE_MODE____NONE( ) \
    { }

#define STL_PARSER_COMMIT_WRITE_MODE____NOWAIT( )   \
    { }

#define STL_PARSER_COMMIT_WRITE_MODE____WAIT( ) \
    { }


/**
 * rollback_statement
 */

#define STL_PARSER_ROLLBACK_STATEMENT____( )                                \
    {                                                                       \
        STL_PARSER_CHECK( v0 = ztqMakeBypassSqlTree( param,                 \
                                                     ZTQ_STMT_ROLLBACK,     \
                                                     ZTQ_OBJECT_UNKNOWN,    \
                                                     NULL ) );              \
    }

#define STL_PARSER_ROLLBACK_STATEMENT____FORCE( )                           \
    {                                                                       \
        STL_PARSER_CHECK( v0 = ztqMakeBypassSqlTree( param,                 \
                                                     ZTQ_STMT_ROLLBACK,     \
                                                     ZTQ_OBJECT_UNKNOWN,    \
                                                     NULL ) );              \
    }

#define STL_PARSER_ROLLBACK_STATEMENT____FORCE_COMMENT( )                   \
    {                                                                       \
        STL_PARSER_CHECK( v0 = ztqMakeBypassSqlTree( param,                 \
                                                     ZTQ_STMT_ROLLBACK,     \
                                                     ZTQ_OBJECT_UNKNOWN,    \
                                                     NULL ) );              \
    }

#define STL_PARSER_ROLLBACK_STATEMENT____RELEASE( ) \
    {                                               \
        STL_PARSER_NOT_SUPPORT( a3 );               \
    }

#define STL_PARSER_ROLLBACK_STATEMENT____SAVEPOINT( )                       \
    {                                                                       \
        STL_PARSER_CHECK( v0 = ztqMakeBypassSqlTree( param,                 \
                                                     ZTQ_STMT_ROLLBACK,     \
                                                     ZTQ_OBJECT_UNKNOWN,    \
                                                     NULL ) );              \
    }


/**
 * savepoint_clause
 */

#define STL_PARSER_SAVEPOINT_CLAUSE____( )      \
    { }


/**
 * savepoint_specifier
 */

#define STL_PARSER_SAVEPOINT_SPECIFIER____( )   \
    { }


/**
 * savepoint_name
 */

#define STL_PARSER_SAVEPOINT_NAME____( )        \
    { }


/**
 * alter_system_statement
 */

#define STL_PARSER_ALTER_SYSTEM_STATEMENT____( )    \
    { }


/**
 * alter_system_clause
 */

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____SET( )        \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____RESET( )      \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____AGER( )       \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____DATABASE( )    \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____SWITCH_LOGFILE( ) \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____CHECKPOINT( ) \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____FLUSH_LOGS( ) \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____CLEANUP_PLAN( ) \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____CLEANUP_SESSION( ) \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____DISCONNECT_SESSION( ) \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____END_SESSION( )        \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____REGISTER_BREAKPOINT( )     \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____CLEAR_BREAKPOINT( )        \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____REGISTER_SESSION_NAME( )   \
    { }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____CLEAR_SESSION_NAME( )      \
    { }

#define STL_PARSER_END_SESSION_CLAUSE____DISCONNECT_SESSION( )  \
    { }

#define STL_PARSER_END_SESSION_CLAUSE____DISCONNECT_SESSION_POST_TRANSACTION( ) \
    { }

#define STL_PARSER_END_SESSION_CLAUSE____KILL_SESSION( )        \
    { }

#define STL_PARSER_END_SESSION_OPTION____DEFAULT( )     \
    { }

#define STL_PARSER_END_SESSION_OPTION____IMMEDIATE( )   \
    { }

/**
 * alter_system_set_clause
 */

#define STL_PARSER_ALTER_SYSTEM_SET_CLAUSE____PARAMETER( )      \
    { }


/**
 * set_parameter_clause
 */

#define STL_PARSER_SET_PARAMETER_CLAUSE____SET( )       \
    { }

#define STL_PARSER_SET_PARAMETER_CLAUSE____SET_DEFAULT( )       \
    { }


/**
 * parameter_name
 */

#define STL_PARSER_PARAMETER_NAME____( )        \
    { }


/**
 * parameter_value
 */

#define STL_PARSER_PARAMETER_VALUE____SIGNED_INTEGER( ) \
    { }

#define STL_PARSER_PARAMETER_VALUE____UNSIGNED_INTEGER_BYTE_UNIT( )     \
    { }

#define STL_PARSER_PARAMETER_VALUE____QUOTE_STRING( )   \
    { }

#define STL_PARSER_PARAMETER_VALUE____TRUE( )   \
    { }

#define STL_PARSER_PARAMETER_VALUE____FALSE( )  \
    { }

#define STL_PARSER_PARAMETER_VALUE____YES( )    \
    { }

#define STL_PARSER_PARAMETER_VALUE____NO( )     \
    { }

#define STL_PARSER_PARAMETER_VALUE____ON( )     \
    { }

#define STL_PARSER_PARAMETER_VALUE____OFF( )    \
    { }

#define STL_PARSER_PARAMETER_VALUE____ENABLE( ) \
    { }

#define STL_PARSER_PARAMETER_VALUE____DISABLE( )        \
    { }


/**
 * alter_system_reset_clause
 */

#define STL_PARSER_ALTER_SYSTEM_RESET_CLAUSE____RESET( )        \
    { }


/**
 * property_clause
 */

#define STL_PARSER_PROPERTY_CLAUSE____DEFAULT( )        \
    { }

#define STL_PARSER_PROPERTY_CLAUSE____DEFERRED( )       \
    { }

#define STL_PARSER_PROPERTY_CLAUSE____SCOPE( )  \
    { }

#define STL_PARSER_PROPERTY_CLAUSE____DEFERRED_SCOPE( ) \
    { }

/**
 * property_scope
 */

#define STL_PARSER_PROPERTY_SCOPE____MEMORY( )  \
    { }

#define STL_PARSER_PROPERTY_SCOPE____FILE( )    \
    { }

#define STL_PARSER_PROPERTY_SCOPE____BOTH( )    \
    { }

/**
 *  * alter_system_register_breakpoint_clause
 *   */

#define STL_PARSER_REGISTER_BREAKPOINT_CLAUSE____( )        \
        { }

#define STL_PARSER_REGISTER_SYSTEM_BREAKPOINT_CLAUSE____( )   \
        { }

#define STL_PARSER_BREAKPOINT_OPTION_CLAUSE____SKIP( )      \
        { }

#define STL_PARSER_BREAKPOINT_OPTION_CLAUSE____NOSKIP( )    \
        { }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____WAKEUP_NONE( )   \
        { }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____WAKEUP_ABORT( )  \
        { }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____WAKEUP_SESSION_KILL( )   \
        { }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____WAKEUP_SYSTEM_KILL( )   \
        { }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____SLEEP( )     \
        { }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____ABORT( )     \
        { }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____KILL( )      \
        { }

/**
 *  * alter_system_clear_breakpoint_clause
 *   */

#define STL_PARSER_CLEAR_BREAKPOINT_CLAUSE____( )          \
        { }

/**
 *  * alter_system_register_sesion_name_clause
 *   */

#define STL_PARSER_REGISTER_SYSTEM_SESSION_NAME_CLAUSE____( )     \
        { }

#define STL_PARSER_REGISTER_USER_SESSION_NAME_CLAUSE____( )     \
        { }

/**
 *  * alter_system_clear_session_name_clause
 *   */

#define STL_PARSER_CLEAR_SESSION_NAME_CLAUSE____( )          \
        { }


/**
 * alter_system_ager_clause
 */

#define STL_PARSER_ALTER_SYSTEM_AGER_CLAUSE____STOP( )  \
    { }

#define STL_PARSER_ALTER_SYSTEM_AGER_CLAUSE____START( ) \
    { }

#define STL_PARSER_ALTER_SYSTEM_AGER_CLAUSE____LOOPBACK( ) \
    { }

/**
 * alter_system_flush_log_clause
 */

#define STL_PARSER_ALTER_SYSTEM_FLUSH_LOG_CLAUSE____FLUSH_LOGS( ) \
    { }

#define STL_PARSER_ALTER_SYSTEM_FLUSH_LOG_CLAUSE____STOP_LOG_FLUSHING( )  \
    { }

#define STL_PARSER_ALTER_SYSTEM_FLUSH_LOG_CLAUSE____START_LOG_FLUSHING( ) \
    { }

/**
 * alter_system_startup_clause
 */

#define STL_PARSER_ALTER_SYSTEM_DATABASE_CLAUSE____MOUNT( )  \
    { }

#define STL_PARSER_ALTER_SYSTEM_DATABASE_CLAUSE____OPEN( )   \
    { }

#define STL_PARSER_ALTER_SYSTEM_DATABASE_CLAUSE____CLOSE( )  \
    { }


/**
 * open_database_option
 */

#define STL_PARSER_OPEN_DATABASE____EMPTY()         \
    { }

#define STL_PARSER_OPEN_DATABASE____READ_WRITE()    \
    { }

#define STL_PARSER_OPEN_DATABASE____READ_ONLY()     \
    { }

/**
 * open_log_option
 */

#define STL_PARSER_OPEN_LOG____NORESETLOGS()         \
    { }

#define STL_PARSER_OPEN_LOG____RESETLOGS()    \
    { }

/**
 * close_database_option
 */

#define STL_PARSER_CLOSE_DATABASE____EMPTY()            \
    { }

#define STL_PARSER_CLOSE_DATABASE____NORMAL()           \
    { }

#define STL_PARSER_CLOSE_DATABASE____TRANSACTIONAL()    \
    { }

#define STL_PARSER_CLOSE_DATABASE____IMMEDIATE()        \
    { }

#define STL_PARSER_CLOSE_DATABASE____OPERATIONAL()      \
    { }

#define STL_PARSER_CLOSE_DATABASE____ABORT()            \
    { }


/**
 * alter_session_statement
 */

#define STL_PARSER_ALTER_SESSION_STATEMENT____( )                               \
    { }


/**
 * alter_session_clause
 */

#define STL_PARSER_ALTER_SESSION_CLAUSE____SET( )  \
    { }

#define STL_PARSER_ALTER_SESSION_CLAUSE____SET_DEFAULT( )       \
    { }


/**
 * lock_table_statement
 */

#define STL_PARSER_LOCK_TABLE_STATEMENT____( )  \
    { }


/**
 * table_name_list
 */

#define STL_PARSER_TABLE_NAME_LIST____TABLE( )  \
    { }

#define STL_PARSER_TABLE_NAME_LIST____TABLE_LIST( )     \
    { }


/**
 * lock_mode
 */

#define STL_PARSER_LOCK_MODE____SHARE( )        \
    { }

#define STL_PARSER_LOCK_MODE____EXCLUSIVE( )    \
    { }

#define STL_PARSER_LOCK_MODE____ROW_SHARE( )    \
    { }

#define STL_PARSER_LOCK_MODE____ROW_EXCLUSIVE( )        \
    { }

#define STL_PARSER_LOCK_MODE____SHARE_ROW_EXCLUSIVE( )  \
    { }


/**
 * wait_clause
 */

#define STL_PARSER_WAIT_CLAUSE____WAIT_INFINITY( )      \
    { }

#define STL_PARSER_WAIT_CLAUSE____NO_WAIT( )    \
    { }

#define STL_PARSER_WAIT_CLAUSE____WAIT_TIME( )  \
    { }


/**
 * session_function
 */

#define STL_PARSER_SESSION_FUNCTION____CURRENT_CATALOG( )  \
    { }

#define STL_PARSER_SESSION_FUNCTION____VERSION( )  \
    { }

#define STL_PARSER_SESSION_FUNCTION____USER( )     \
    { }

#define STL_PARSER_SESSION_FUNCTION____SESSION_USER( )     \
    { }

#define STL_PARSER_SESSION_FUNCTION____LOGON_USER( )     \
    { }

#define STL_PARSER_SESSION_FUNCTION____CURRENT_SCHEMA( )   \
    { }

#define STL_PARSER_SESSION_FUNCTION____SESSION_ID( )  \
    { }

#define STL_PARSER_SESSION_FUNCTION____SESSION_SERIAL( )  \
    { }

#define STL_PARSER_SESSION_FUNCTION____USER_ID( )  \
    { }


/**
 * AGGREGATE FUNCTION
 */

#define STL_PARSER_AGGREGATE_FUNCTION____UNARY( )       \
    { }

/* Unary Aggregation */
#define STL_PARSER_UNARY_AGGREGATION____SUM( )                                  \
    { }

#define STL_PARSER_UNARY_AGGREGATION____COUNT( )                                \
    { }

#define STL_PARSER_UNARY_AGGREGATION____COUNT_ASTERISK( )                       \
    { }

#define STL_PARSER_UNARY_AGGREGATION____MIN( )                                  \
    { }

#define STL_PARSER_UNARY_AGGREGATION____MAX( )                                  \
    { }

#define STL_PARSER_UNARY_AGGREGATION____AVG( )                                  \
    { }

/**
 * CASE EXPRESSION
 */

#define STL_PARSER_CASE_ABBREVIATION____( )                           \
    { }

#define STL_PARSER_CASE_SPECIFICATION____( )                          \
    { }

#define STL_PARSER_CASE_ABBREVIATION____NULLIF( )                     \
    { }

#define STL_PARSER_CASE_ABBREVIATION____COALESCE( )                   \
    { }

#define STL_PARSER_CASE_ABBREVIATION_LIST____COALESCE_EXPR( )         \
    { }

#define STL_PARSER_CASE_ABBREVIATION_LIST____COALESCE_EXPR_LIST( )    \
    { }

#define STL_PARSER_CASE_SPECIFICATION____SIMPLE( )                    \
    { }

#define STL_PARSER_CASE_SPECIFICATION____SEARCHED( )                  \
    { }

#define STL_PARSER_CASE_SPECIFICATION____SIMPLE_CASE( )               \
    { } 

#define STL_PARSER_CASE_SPECIFICATION____SEARCHED_CASE( )             \
    { } 

#define STL_PARSER_CASE_SPECIFICATION_LIST____SIMPLE_WHEN_CLAUSE( )   \
    { }

#define STL_PARSER_CASE_SPECIFICATION_LIST____SIMPLE_WHEN_CLAUSE_LIST( )        \
    { }

#define STL_PARSER_CASE_SPECIFICATION____SIMPLE_WHEN_CLAUSE( )        \
    { }

#define STL_PARSER_CASE_SPECIFICATION_LIST____SEARCHED_WHEN_CLAUSE( ) \
    { }

#define STL_PARSER_CASE_SPECIFICATION_LIST____SEARCHED_WHEN_CLAUSE_LIST( )      \
    { }

#define STL_PARSER_CASE_SPECIFICATION____SEARCHED_WHEN_CLAUSE( )      \
    { }

#define STL_PARSER_CASE_SPECIFICATION____ELSE_CLAUSE_EMPTY( )         \
    { }

#define STL_PARSER_CASE_SPECIFICATION____ELSE_CLAUSE( )               \
    { }

#define STL_PARSER_CASE_SPECIFICATION____CASE_OPERAND_ROW_VALUE( )    \
    { }                                                       

#define STL_PARSER_CASE_SPECIFICATION_LIST____CASE_OPERAND_VALUE( )   \
    { }

#define STL_PARSER_CASE_SPECIFICATION____WHEN_OPERAND_ROW_VALUE( )    \
    { }

#define STL_PARSER_CASE_SPECIFICATION_LIST____WHEN_OPERAND_VALUE( )   \
    { }

#define STL_PARSER_CASE_SPECIFICATION____RESULT( )                    \
    { }

#define STL_PARSER_CASE_SPECIFICATION____RESULT_EXPRESSION( )         \
    { }


/**
 * SET QUANTIFIER
 */

#define STL_PARSER_SET_QUANTIFIER____DEFAULT( )                         \
    { }

#define STL_PARSER_SET_QUANTIFIER____ALL( )                             \
    { }

#define STL_PARSER_SET_QUANTIFIER____DISTINCT( )                        \
    { }


/**
 * FILTER CLAUSE
 */

#define STL_PARSER_FILTER_CLAUSE____EMPTY( )    \
    { }

#define STL_PARSER_FILTER_CLAUSE____FILTER( )   \
    { }


/**
 * COMMENT ON STATEMENT
 */

#define STL_PARSER_COMMENT_STATEMENT____( )     \
    { }

#define STL_PARSER_COMMENT_OBJECT____DATABASE( )    \
    { }

#define STL_PARSER_COMMENT_OBJECT____PROFILE() \
    { }

#define STL_PARSER_COMMENT_OBJECT____AUTHORIZATION( )   \
    { }

#define STL_PARSER_COMMENT_OBJECT____TABLESPACE( )  \
    { }

#define STL_PARSER_COMMENT_OBJECT____SCHEMA( )  \
    { }

#define STL_PARSER_COMMENT_OBJECT____TABLE( )   \
    { }

#define STL_PARSER_COMMENT_OBJECT____INDEX( )   \
    { }

#define STL_PARSER_COMMENT_OBJECT____SEQUENCE( )    \
    { }

#define STL_PARSER_COMMENT_OBJECT____CONSTRAINT( )  \
    { }

#define STL_PARSER_COMMENT_OBJECT____COLUMN( )  \
    { }


/*
 * SET CONSTRAINT
 */

#define STL_PARSER_SET_CONSTRAINT____NAME_DEFERRED()    \
    { }

#define STL_PARSER_SET_CONSTRAINT____ALL_DEFERRED() \
    { }

#define STL_PARSER_SET_CONSTRAINT____NAME_IMMEDIATE()   \
    { }

#define STL_PARSER_SET_CONSTRAINT____ALL_IMMEDIATE()    \
    { }


#define STL_PARSER_CONSTRAINT_NAME_LIST()       \
    { }

/*
 * SET TRANSACTION STATEMENT
 */

#define STL_PARSER_SET_TRANSACTION____()        \
    { }

#define STL_PARSER_TRANSACTION_MODE____TRANSACTION_ACCESS_MODE()        \
    { }

#define STL_PARSER_TRANSACTION_MODE____ISOLATION_LEVEL()        \
    { }

#define STL_PARSER_TRANSACTION_MODE____UNIQUE_INTEGRITY()   \
    { }

#define STL_PARSER_TRANSACTION_ACCESS_MODE____READ_ONLY()       \
    { }

#define STL_PARSER_TRANSACTION_ACCESS_MODE____READ_WRITE()      \
    { }

#define STL_PARSER_ISOLATION_LEVEL____()        \
    { }

#define STL_PARSER_LEVEL_OF_ISOLATION____READ_COMMITTED()       \
    { }

#define STL_PARSER_LEVEL_OF_ISOLATION____SERIALIZABLE() \
    { }

#define STL_PARSER_UNIQUE_INTEGRITY____DEFAULT()    \
    { }

#define STL_PARSER_UNIQUE_INTEGRITY____DEFERRED()   \
    { }

/*
 * SET SESSION CHARACTERISTICS STATEMENT
 */

#define STL_PARSER_SET_SESSION_CHARACTERISTICS____()    \
    { }


/*
 * SET SESSION AUTHORIZATION
 */

#define STL_PARSER_SET_SESSION_AUTHORIZATION() \
    { }

/*
 * SET TIME ZONE STATEMENT
 */
#define STL_PARSER_SET_TIME_ZONE____LOCAL()     \
    { }

#define STL_PARSER_SET_TIME_ZONE____GMT_OFFSET()        \
    { }

/*
 * DECLARE .. STATEMENT
 */
#define STL_PARSER_DECLARE_STATEMENT____()      \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a3 );           \
    }

/*
 * DECLARE .. CURSOR
 */

#define STL_PARSER_DECLARE_CURSOR____ISO_TYPE()                                 \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_DECLARE_CURSOR,  \
                                                      ZTQ_OBJECT_UNKNOWN,       \
                                                      (stlChar*)v2 ) );         \
    }

#define STL_PARSER_DECLARE_CURSOR____ODBC_TYPE()                                \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            param->mParseTree = ztqMakeBypassSqlTree( param,                    \
                                                      ZTQ_STMT_DECLARE_CURSOR,  \
                                                      ZTQ_OBJECT_UNKNOWN,       \
                                                      (stlChar*)v2 ) );         \
    }

#define STL_PARSER_DYNAMIC_CURSOR____ISO_TYPE() \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a8 );           \
    }

#define STL_PARSER_DYNAMIC_CURSOR____ODBC_TYPE()    \
    {                                               \
        STL_PARSER_NOT_SUPPORT( a7 );               \
    }

/*
 * cursor_sensitivity
 */

#define STL_PARSER_CURSOR_SENSITIVITY____EMPTY()   \
    { }

#define STL_PARSER_CURSOR_SENSITIVITY____INSENSITIVE()  \
    { }

#define STL_PARSER_CURSOR_SENSITIVITY____SENSITIVE()  \
    { }

#define STL_PARSER_CURSOR_SENSITIVITY____ASENSITIVE()  \
    { }


/*
 * cursor_scrollability
 */

#define STL_PARSER_CURSOR_SCROLLABILITY____EMPTY()  \
    { }

#define STL_PARSER_CURSOR_SCROLLABILITY____NO_SCROLL()  \
    { }

#define STL_PARSER_CURSOR_SCROLLABILITY____SCROLL()         \
    { }

/*
 * cursor odbc type
 */

#define STL_PARSER_CURSOR_ODBC_TYPE____STATIC() \
    { }

#define STL_PARSER_CURSOR_ODBC_TYPE____KEYSET() \
    { }

#define STL_PARSER_CURSOR_ODBC_TYPE____DYNAMIC() \
    { }

/*
 * cursor_holdability
 */

#define STL_PARSER_CURSOR_HOLDABILITY____EMPTY()  \
    { }

#define STL_PARSER_CURSOR_HOLDABILITY____WITH_HOLD()  \
    { }

#define STL_PARSER_CURSOR_HOLDABILITY____WITHOUT_HOLD()  \
    { }

/*
 * cursor_query
 */

#define STL_PARSER_CURSOR_QUERY____SELECT() \
    { }

#define STL_PARSER_CURSOR_QUERY____INSERT_RETURNING_QUERY()  \
    { }

#define STL_PARSER_CURSOR_QUERY____UPDATE_RETURNING_QUERY()  \
    { }

#define STL_PARSER_CURSOR_QUERY____DELETE_RETURNING_QUERY()  \
    { }

/*
 * open_statement
 */

#define STL_PARSER_OPEN_CURSOR____()                                        \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            param->mParseTree = ztqMakeBypassSqlTree( param,                \
                                                      ZTQ_STMT_OPEN_CURSOR, \
                                                      ZTQ_OBJECT_UNKNOWN,   \
                                                      (stlChar*)v2 ) );     \
        ztqSetHostVar4OpenCursor( (ztqBypassTree*)param->mParseTree, v2 );  \
    }

#define STL_PARSER_OPEN_DYNAMIC_CURSOR____()    \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a3 );           \
    }

/*
 * close_statement
 */

#define STL_PARSER_CLOSE_CURSOR____()           \
    { }                                                      

/*
 * fetch_statement
 */


#define STL_PARSER_FETCH_CURSOR____WITH_FROM() \
    {  }

#define STL_PARSER_FETCH_CURSOR____WITHOUT_FROM() \
    {  }

#define STL_PARSER_FETCH_ORIENTATION____EMPTY()  \
    {  }

#define STL_PARSER_FETCH_ORIENTATION____NEXT()  \
    {  }

#define STL_PARSER_FETCH_ORIENTATION____PRIOR()  \
    {  }

#define STL_PARSER_FETCH_ORIENTATION____FIRST()  \
    {  }

#define STL_PARSER_FETCH_ORIENTATION____LAST()  \
    {  }

#define STL_PARSER_FETCH_ORIENTATION____CURRENT()  \
    {  }

#define STL_PARSER_FETCH_ORIENTATION____ABSOLUTE()  \
    {  }

#define STL_PARSER_FETCH_ORIENTATION____RELATIVE() \
    {  }

/*
 * connection_statement
 */

#define STL_PARSER_CONNECTION____CONNECT()      \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a1 );           \
    }

#define STL_PARSER_CONNECTION____DISCONNECT()   \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a1 );           \
    }

#define STL_PARSER_CONNECT____EMPTY()           \
    { }

#define STL_PARSER_CONNECT____USING()           \
    { }

#define STL_PARSER_CONNECT____AT_USING()        \
    { }

#define STL_PARSER_USER_PWD____USER_PASSWORD_ALL()  \
    { }

#define STL_PARSER_USER_PWD____USER_PASSWORD_SEP()  \
    { }

#define STL_PARSER_CONN_STRING_DESC____HOST_VARIABLE()  \
    { }

#define STL_PARSER_CONN_STRING_DESC____IDENTIFIER() \
    { }

#define STL_PARSER_DISCONNECT____EMPTY()        \
    { }

#define STL_PARSER_DISCONNECT____CONN_OBJECT()  \
    { }

#define STL_PARSER_DISCONNECT____ALL()          \
    { }


/***************************************************************************************
 * DYNAMIC SQL COMMAND - only available in Embedded-SQL, PSM
 * - Embedded SQL, PSM 등에서만 사용가능한 Dynamic SQL
 * - ex) EXECUTE IMMEDIATE
 ***************************************************************************************/

/**
 * dynamic_sql_command
 */

#define STL_PARSER_DYNAMIC_COMMAND____EXECUTE_IMMEDIATE()   \
    {                                                       \
        STL_PARSER_NOT_SUPPORT( a1 );                       \
    }

#define STL_PARSER_DYNAMIC_COMMAND____PREPARE() \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a1 );           \
    }

#define STL_PARSER_DYNAMIC_COMMAND____EXECUTE() \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a1 );           \
    }

/**
 * EXECUTE IMMEDIATE
 */

#define STL_PARSER_EXECUTE_IMMEDIATE____HOSTVAR() \
    { }
#define STL_PARSER_EXECUTE_IMMEDIATE____SINGLE_QUOTE() \
    { }
#define STL_PARSER_EXECUTE_IMMEDIATE____DOUBLE_QUOTE() \
    { }
#define STL_PARSER_EXECUTE_IMMEDIATE____SQL_COMMAND() \
    { }

/**
 * PREPARE
 */

#define STL_PARSER_PREPARE____HOSTVAR() \
    { }
#define STL_PARSER_PREPARE____SINGLE_QUOTE() \
    { }
#define STL_PARSER_PREPARE____DOUBLE_QUOTE() \
    { }
#define STL_PARSER_PREPARE____SQL_COMMAND() \
    { }

/**
 * EXECUTE
 */

#define STL_PARSER_EXECUTE____BASIC() \
    { }

#define STL_PARSER_EXECUTE____EMPTY() \
    { }
#define STL_PARSER_EXECUTE____USING() \
    { }
#define STL_PARSER_EXECUTE____INTO() \
    { }
#define STL_PARSER_EXECUTE____USING_INTO() \
    { }
#define STL_PARSER_EXECUTE____INTO_USING() \
    { }

#define STL_PARSER_USING_PARAMETER_CLAUSE____ARGUMENTS() \
    { }
#define STL_PARSER_USING_PARAMETER_CLAUSE____DESCRIPTOR() \
    { }
#define STL_PARSER_INTO_PARAMETER_CLAUSE____ARGUMENTS() \
    { }
#define STL_PARSER_INTO_PARAMETER_CLAUSE____DESCRIPTOR() \
    { }

#define STL_PARSER_DESCRIPTOR_CLAUSE____HOST_VAR() \
    { }
#define STL_PARSER_DESCRIPTOR_CLAUSE____SQL_HOST_VAR() \
    { }
#define STL_PARSER_DESCRIPTOR_CLUASE____SINGLE_QUOTE() \
    { }
#define STL_PARSER_DESCRIPTOR_CLUASE____SQL_SINGLE_QUOTE() \
    { }

/***************************************************************************************
 * PHRASE SYNTAX - syntax replacement
 * - Syntax Replacement 를 위해 QP 내부적으로만 사용하는 기능임.
 * - ex) viewed table : VIEW v1(c1, c2) 의 대체
 * - ex) check constraint : CHECK c1 > 0 의 대체
 * - ex) default value : DEFAULT systimestamp
 ***************************************************************************************/

/**
 * phrase_viewed_table
 */
#define STL_PARSER_PHRASE____VIEWED_TABLE()     \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a1 );           \
    }

/**
 * phrase_default_expression
 */
#define STL_PARSER_PHRASE____DEFAULT_EXPRESSION()   \
    {                                               \
        STL_PARSER_NOT_SUPPORT( a1 );               \
    }

/**
 * VIEW view_name ( column_list )
 */
#define STL_PHRASE_VIEWED_TABLE____BASIC()      \
    { }

/**
 * DEFAULT expression
 */
#define STL_PHRASE_DEFAULT_EXPRESSION____BASIC() \
    { }

