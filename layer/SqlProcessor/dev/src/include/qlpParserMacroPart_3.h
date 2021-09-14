/*******************************************************************************
 * qlpParserMacroPart_3.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        vId: qlpParserMacroPart_3.h 2754 2011-12-15 02:27:23Z jhkim v
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLPPARSERMACROPART_3_H_
#define _QLPPARSERMACROPART_3_H_ 1

/**
 * @file qlpParserMacroPart_3.h
 * @brief SQL Processor Layer Parser macro Part #3
 */

#include <qlpParse.h>

/**
 * @addtogroup qlpParser
 * @{
 */

/**
 * savepoint_statement
 */

#define STL_PARSER_SAVEPOINT_STATEMENT____( )                               \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeSavepoint( param,                     \
                                                 a1,                        \
                                                 QLP_GET_PTR_VALUE( v2 ),   \
                                                 a2 ) );                    \
    }

/**
 * release_savepoint_statement
 */

#define STL_PARSER_RELEASE_SAVEPOINT_STATEMENT____( )                               \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeReleaseSavepoint( param,                      \
                                                        a1,                         \
                                                        QLP_GET_PTR_VALUE( v3 ),    \
                                                        a3 ) );                     \
    }


/**
 * commit_statement
 */

#define STL_PARSER_COMMIT_STATEMENT____( )              \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCommit( param,    \
                                              a1,       \
                                              NULL,     \
                                              NULL,     \
                                              v3 ) );   \
    }

#define STL_PARSER_COMMIT_STATEMENT____WRITE_MODE( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCommit( param,    \
                                              a1,       \
                                              NULL,     \
                                              v5,       \
                                              v3 ) );   \
    }

#define STL_PARSER_COMMIT_STATEMENT____FORCE( )                                 \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeCommit( param,                            \
                                              a1,                               \
                                              qlpMakeStrConstant( param,        \
                                                                  a4,           \
                                                                  v4 ),         \
                                              NULL,                             \
                                              v5 ) );                           \
    }

#define STL_PARSER_COMMIT_STATEMENT____RELEASE( )       \
    {                                                   \
        STL_PARSER_NOT_SUPPORT( a3 );                   \
    }


/**
 * comment_option
 */

#define STL_PARSER_COMMENT_OPTION____( )                                \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a2, v2 ) );   \
    }


/**
 * commit_write_mode
 */

#define STL_PARSER_COMMIT_WRITE_MODE____NONE( )                                         \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        SML_TRANSACTION_CWM_NONE ) );   \
    }

#define STL_PARSER_COMMIT_WRITE_MODE____NOWAIT( )                                           \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                              \
                                                        a1,                                 \
                                                        SML_TRANSACTION_CWM_NO_WAIT ) );    \
    }

#define STL_PARSER_COMMIT_WRITE_MODE____WAIT( )                                         \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        SML_TRANSACTION_CWM_WAIT ) );   \
    }


/**
 * rollback_statement
 */

#define STL_PARSER_ROLLBACK_STATEMENT____( )            \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeRollback( param,  \
                                                a1,     \
                                                NULL,   \
                                                NULL,   \
                                                NULL,   \
                                                0 ) );  \
    }

#define STL_PARSER_ROLLBACK_STATEMENT____FORCE( )                           \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeRollback( param,                      \
                                                a1,                         \
                                                qlpMakeStrConstant( param,  \
                                                                    a4,     \
                                                                    v4 ),   \
                                                NULL,                       \
                                                NULL,                       \
                                                0 ) );                      \
    }

#define STL_PARSER_ROLLBACK_STATEMENT____FORCE_COMMENT( )                   \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeRollback( param,                      \
                                                a1,                         \
                                                qlpMakeStrConstant( param,  \
                                                                    a4,     \
                                                                    v4 ),   \
                                                qlpMakeStrConstant( param,  \
                                                                    a6,     \
                                                                    v6 ),   \
                                                NULL,                       \
                                                0 ) );                      \
    }

#define STL_PARSER_ROLLBACK_STATEMENT____RELEASE( ) \
    {                                               \
        STL_PARSER_NOT_SUPPORT( a3 );               \
    }

#define STL_PARSER_ROLLBACK_STATEMENT____SAVEPOINT( )                       \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeRollback( param,                      \
                                                a1,                         \
                                                NULL,                       \
                                                NULL,                       \
                                                QLP_GET_PTR_VALUE( v3 ),    \
                                                a3 ) );                     \
    }


/**
 * savepoint_clause
 */

#define STL_PARSER_SAVEPOINT_CLAUSE____( )      \
    {                                           \
        STL_PARSER_CHECK( v0 = v3 );            \
    }


/**
 * savepoint_specifier
 */

#define STL_PARSER_SAVEPOINT_SPECIFIER____( )   \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * savepoint_name
 */

#define STL_PARSER_SAVEPOINT_NAME____( )        \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * alter_system_statement
 */

#define STL_PARSER_ALTER_SYSTEM_STATEMENT____( ) \
    {                                            \
        STL_PARSER_CHECK( v0 = v3 );             \
    }


/**
 * alter_system_clause
 */

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____SET( )                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 =                                                          \
                          qlpMakeAlterSystemSet( param,                                 \
                                                 QLL_STMT_ALTER_SYSTEM_SET_TYPE,        \
                                                 a1,                                    \
                                                 v2 ) );                                \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____RESET( )                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 =                                                          \
                          qlpMakeAlterSystemReset( param,                               \
                                                   QLL_STMT_ALTER_SYSTEM_RESET_TYPE,    \
                                                   a1,                                  \
                                                   v2 ) );                              \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____AGER( )   \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____FLUSH_LOGS( )  \
    {                                                    \
        STL_PARSER_CHECK( v0 = v1 );                     \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____REGISTER_BREAKPOINT( )   \
    {                                                              \
        STL_PARSER_CHECK( v0 = v1 );                               \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____CLEAR_BREAKPOINT( )   \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____REGISTER_SESSION_NAME( ) \
    {                                                              \
        STL_PARSER_CHECK( v0 = v1 );                               \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____CLEAR_SESSION_NAME( ) \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____SWITCH_LOGFILE( )     \
    {                                                           \
        STL_PARSER_CHECK(                                       \
            v0 = qlpMakeAlterSystemSwitchLogfile(               \
                param,                                          \
                a1,                                             \
                QLL_STMT_ALTER_SYSTEM_SWITCH_LOGFILE_TYPE ) );  \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____DATABASE( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____CHECKPOINT( )                 \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemChkpt( param, a1 ) );  \
    }

/**
 * alter_system_flush_log_clause
 */
#define STL_PARSER_ALTER_SYSTEM_FLUSH_LOG_CLAUSE____FLUSH_LOGS( )  \
    {                                                              \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemFlushLog(         \
                              param,                               \
                              a1,                                  \
                              QLP_FLUSH_LOGS ) );                  \
    }

#define STL_PARSER_ALTER_SYSTEM_FLUSH_LOG_CLAUSE____STOP_LOG_FLUSHING( )   \
    {                                                                      \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemFlushLog(                 \
                              param,                                       \
                              a1,                                          \
                              QLP_STOP_FLUSH_LOGS ) );                     \
    }

#define STL_PARSER_ALTER_SYSTEM_FLUSH_LOG_CLAUSE____START_LOG_FLUSHING( )  \
    {                                                                      \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemFlushLog(                 \
                              param,                                       \
                              a1,                                          \
                              QLP_START_FLUSH_LOGS ) );                    \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____CLEANUP_PLAN( )                       \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemCleanupPlan( param, a1 ) );    \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____CLEANUP_SESSION( )                    \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemCleanupSession( param, a1 ) ); \
    }

#define STL_PARSER_ALTER_SYSTEM_CLAUSE____END_SESSION( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_END_SESSION_CLAUSE____DISCONNECT_SESSION( )          \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemDisconnectSession(     \
                              param,                                    \
                              a1,                                       \
                              qlpMakeStrConstant( param,                \
                                                  a3,                   \
                                                  v3 ),                 \
                              qlpMakeStrConstant( param,                \
                                                  a5,                   \
                                                  v5 ),                 \
                              STL_FALSE,                                \
                              v6 ) );                                   \
    }

#define STL_PARSER_END_SESSION_CLAUSE____DISCONNECT_SESSION_POST_TRANSACTION( ) \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemDisconnectSession(             \
                              param,                                            \
                              a1,                                               \
                              qlpMakeStrConstant( param,                        \
                                                  a3,                           \
                                                  v3 ),                         \
                              qlpMakeStrConstant( param,                        \
                                                  a5,                           \
                                                  v5 ),                         \
                              STL_TRUE,                                         \
                              v7 ) );                                           \
    }

#define STL_PARSER_END_SESSION_CLAUSE____KILL_SESSION( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemKillSession(   \
                              param,                            \
                              a1,                               \
                              qlpMakeStrConstant( param,        \
                                                  a3,           \
                                                  v3 ),         \
                              qlpMakeStrConstant( param,        \
                                                  a5,           \
                                                  v5 ) ) );     \
    }

#define STL_PARSER_END_SESSION_OPTION____DEFAULT( )                     \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeNullConstant( param, 0 ) );       \
    }

#define STL_PARSER_END_SESSION_OPTION____IMMEDIATE( )           \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param,       \
                                                   a1,          \
                                                   v1 ) );      \
    }

/**
 * alter_system_set_clause
 */

#define STL_PARSER_ALTER_SYSTEM_SET_CLAUSE____PARAMETER( )    \
    {                                                         \
        STL_PARSER_CHECK( v0 = v1 );                          \
    }


/**
 * set_parameter_clause
 */

#define STL_PARSER_SET_PARAMETER_CLAUSE____SET( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 =                          \
                          qlpMakeSetParameter( param,   \
                                               a1,      \
                                               v1,      \
                                               v3,      \
                                               v4 ) );  \
    }

#define STL_PARSER_SET_PARAMETER_CLAUSE____SET_DEFAULT( )       \
    {                                                           \
        STL_PARSER_CHECK( v0 =                                  \
                          qlpMakeSetParameter( param,           \
                                               a1,              \
                                               v1,              \
                                               NULL,            \
                                               v4 ) );          \
    }


/**
 * parameter_name
 */

#define STL_PARSER_PARAMETER_NAME____( )        \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * parameter_value
 */

#define STL_PARSER_PARAMETER_VALUE____SIGNED_INTEGER( )                         \
    {                                                                           \
        qlpValue  * sValue = v2;                                                \
        stlInt32    sNumLen = stlStrlen( QLP_GET_PTR_VALUE( sValue ) ) + 2;     \
        stlChar   * sStr    = qlpMakeBuffer( param,                             \
                                             sNumLen );                         \
                                                                                \
        sStr[ 0 ] = ((stlChar*)v1)[ 0 ];                                        \
        stlStrcpy( & sStr[ 1 ], QLP_GET_PTR_VALUE( sValue ) );                  \
        sStr[ sNumLen - 1 ] = '\0';                                             \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeNumberConstant( param, a1, sStr ) );      \
    }

#define STL_PARSER_PARAMETER_VALUE____UNSIGNED_INTEGER_BYTE_UNIT( )                     \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeSize( param,                                      \
                                            a1,                                         \
                                            STL_FALSE,                                  \
                                            v2,                                         \
                                            qlpMakeNumberConstant( param, a1, v1 ) ) ); \
    }

#define STL_PARSER_PARAMETER_VALUE____QUOTE_STRING( )           \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param,       \
                                                   a1,          \
                                                   v1 ) );      \
    }

#define STL_PARSER_PARAMETER_VALUE____TRUE( )                           \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

#define STL_PARSER_PARAMETER_VALUE____FALSE( )                          \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_FALSE ) );  \
    }

#define STL_PARSER_PARAMETER_VALUE____YES( )                            \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

#define STL_PARSER_PARAMETER_VALUE____NO( )                             \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_FALSE ) );  \
    }

#define STL_PARSER_PARAMETER_VALUE____ON( )                             \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

#define STL_PARSER_PARAMETER_VALUE____OFF( )                            \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_FALSE ) );  \
    }

#define STL_PARSER_PARAMETER_VALUE____ENABLE( )                         \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }
#define STL_PARSER_PARAMETER_VALUE____DISABLE( )                        \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_FALSE ) );  \
    }


/**
 * alter_system_reset_clause
 */

#define STL_PARSER_ALTER_SYSTEM_RESET_CLAUSE____RESET( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }


/**
 * property_clause
 */

#define STL_PARSER_PROPERTY_CLAUSE____DEFAULT( )                                                    \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeSetParamAttr( param,                                          \
                                                    a0,                                             \
                                                    STL_FALSE,                                      \
                                                    qlpMakeIntParamConstant( param,                 \
                                                                             a0,                    \
                                                                             KNL_PROPERTY_SCOPE_MEMORY ) ) ); \
    }


#define STL_PARSER_PROPERTY_CLAUSE____DEFERRED( )                                                   \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeSetParamAttr( param,                                          \
                                                    a0,                                             \
                                                    STL_TRUE,                                       \
                                                    qlpMakeIntParamConstant( param,                 \
                                                                             a0,                    \
                                                                             KNL_PROPERTY_SCOPE_MEMORY ) ) ); \
    }

#define STL_PARSER_PROPERTY_CLAUSE____SCOPE( )                  \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeSetParamAttr( param,      \
                                                    a0,         \
                                                    STL_FALSE,  \
                                                    v1 ) );     \
    }

#define STL_PARSER_PROPERTY_CLAUSE____DEFERRED_SCOPE( )         \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeSetParamAttr( param,      \
                                                    a0,         \
                                                    STL_TRUE,   \
                                                    v2 ) );     \
    }


/**
 * property_scope
 */

#define STL_PARSER_PROPERTY_SCOPE____MEMORY( )                                          \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a3,                             \
                                                        KNL_PROPERTY_SCOPE_MEMORY ) );  \
    }

#define STL_PARSER_PROPERTY_SCOPE____FILE( )                                            \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a3,                             \
                                                        KNL_PROPERTY_SCOPE_FILE ) );    \
    }

#define STL_PARSER_PROPERTY_SCOPE____BOTH( )                                            \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a3,                             \
                                                        KNL_PROPERTY_SCOPE_BOTH ) );    \
    }

/**
 * alter_system_register_breakpoint_clause
 */

#define STL_PARSER_REGISTER_BREAKPOINT_CLAUSE____( )             \
    {                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemBreakpoint(     \
                              param,                             \
                              a1,                                \
                              KNL_BREAKPOINT_SESSION_TYPE_USER,  \
                              v3,                                \
                              v5,                                \
                              v6 ) );                            \
    }

#define STL_PARSER_REGISTER_SYSTEM_BREAKPOINT_CLAUSE____( )       \
    {                                                             \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemBreakpoint(      \
                              param,                              \
                              a1,                                 \
                              KNL_BREAKPOINT_SESSION_TYPE_SYSTEM, \
                              v3,                                 \
                              qlpMakeStrConstant(param, a5, v5),  \
                              v6 ) );                             \
    }

#define STL_PARSER_BREAKPOINT_OPTION_CLAUSE____SKIP( )                                        \
    {                                                                                         \
        STL_PARSER_CHECK( v0 = qlpMakeBreakpointOption( param,                                \
                                                        a1,                                   \
                                                        qlpMakeStrConstant(param, a2, v2 ),   \
                                                        v3 ) );                               \
    }

#define STL_PARSER_BREAKPOINT_OPTION_CLAUSE____NOSKIP( )            \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeBreakpointOption( param,      \
                                                        a1,         \
                                                        NULL,       \
                                                        v1 ) );     \
    }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____WAKEUP_NONE( )                               \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeBreakpointAction( param,                              \
                                                        a1,                                 \
                                                        KNL_BREAKPOINT_ACTION_WAKEUP,       \
                                                        KNL_BREAKPOINT_POST_ACTION_NONE,    \
                                                        v2 ) );                             \
    }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____WAKEUP_ABORT( )                              \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeBreakpointAction( param,                              \
                                                        a1,                                 \
                                                        KNL_BREAKPOINT_ACTION_WAKEUP,       \
                                                        KNL_BREAKPOINT_POST_ACTION_ABORT,   \
                                                        v2 ) );                             \
    }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____WAKEUP_SESSION_KILL( )                               \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeBreakpointAction( param,                                      \
                                                        a1,                                         \
                                                        KNL_BREAKPOINT_ACTION_WAKEUP,               \
                                                        KNL_BREAKPOINT_POST_ACTION_SESSION_KILL,    \
                                                        v2 ) );                                     \
    }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____WAKEUP_SYSTEM_KILL( )                            \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeBreakpointAction( param,                                  \
                                                        a1,                                     \
                                                        KNL_BREAKPOINT_ACTION_WAKEUP,           \
                                                        KNL_BREAKPOINT_POST_ACTION_SYSTEM_KILL, \
                                                        v2 ) );                                 \
    }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____SLEEP( )                                         \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeBreakpointAction( param,                                  \
                                                        a1,                                     \
                                                        KNL_BREAKPOINT_ACTION_SLEEP,            \
                                                        KNL_BREAKPOINT_POST_ACTION_NONE,        \
                                                        qlpMakeStrConstant(param, a2, v2) ) );  \
    }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____ABORT( )                                     \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeBreakpointAction( param,                              \
                                                        a1,                                 \
                                                        KNL_BREAKPOINT_ACTION_ABORT,        \
                                                        KNL_BREAKPOINT_POST_ACTION_NONE,    \
                                                        NULL ) );                           \
    }

#define STL_PARSER_BREAKPOINT_ACTION_CLAUSE____KILL( )                                      \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeBreakpointAction( param,                              \
                                                        a1,                                 \
                                                        KNL_BREAKPOINT_ACTION_KILL,         \
                                                        KNL_BREAKPOINT_POST_ACTION_NONE,    \
                                                        NULL ) );                           \
    }

/**
 *  alter_system_clear_breakpoint_clause
 */

#define STL_PARSER_CLEAR_BREAKPOINT_CLAUSE____( )               \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemBreakpoint(    \
                              param,                            \
                              a1,                               \
                              KNL_BREAKPOINT_SESSION_TYPE_NONE, \
                              NULL,                             \
                              NULL,                             \
                              NULL ) );                         \
    }

/**
 *  alter_system_register_session_name_clause
 */

#define STL_PARSER_REGISTER_SYSTEM_SESSION_NAME_CLAUSE____( )         \
    {                                                                 \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemSessName(            \
                              param,                                  \
                              a1,                                     \
                              qlpMakeStrConstant(param, a5, v5) ) );  \
    }

#define STL_PARSER_REGISTER_USER_SESSION_NAME_CLAUSE____( )           \
    {                                                                 \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemSessName( param,     \
                                                           a1,        \
                                                           v5 ) );    \
    }

/**
 *  alter_system_clear_session_name_clause
 */

#define STL_PARSER_CLEAR_SESSION_NAME_CLAUSE____( )                    \
    {                                                                  \
        STL_PARSER_CHECK( v0 = qlpMakeAlterSystemSessName( param,      \
                                                           a1,         \
                                                           NULL ) );   \
    }


/**
 * alter_system_ager_clause
 */

#define STL_PARSER_ALTER_SYSTEM_AGER_CLAUSE____STOP( )                  \
    {                                                                   \
        STL_PARSER_CHECK( v0 =                                          \
                          qlpMakeAlterSystemAger( param,                \
                                                  a1,                   \
                                                  QLP_AGER_STOP ) );    \
    }

#define STL_PARSER_ALTER_SYSTEM_AGER_CLAUSE____START( )                 \
    {                                                                   \
        STL_PARSER_CHECK( v0 =                                          \
                          qlpMakeAlterSystemAger( param,                \
                                                  a1,                   \
                                                  QLP_AGER_START ) );   \
    }

#define STL_PARSER_ALTER_SYSTEM_AGER_CLAUSE____LOOPBACK( )                  \
    {                                                                       \
        STL_PARSER_CHECK( v0 =                                              \
                          qlpMakeAlterSystemAger( param,                    \
                                                  a1,                       \
                                                  QLP_AGER_LOOPBACK ) );    \
    }


/**
 * alter_system_database_clause
 */

#define STL_PARSER_ALTER_SYSTEM_DATABASE_CLAUSE____MOUNT( )                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 =                                                          \
                          qlpMakeAlterSystemStartupDatabase( param,                     \
                                                             a1,                        \
                                                             KNL_STARTUP_PHASE_MOUNT,   \
                                                             NULL,                      \
                                                             NULL ) );                  \
    }

#define STL_PARSER_ALTER_SYSTEM_DATABASE_CLAUSE____OPEN( )                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 =                                                          \
                          qlpMakeAlterSystemStartupDatabase( param,                     \
                                                             a1,                        \
                                                             KNL_STARTUP_PHASE_OPEN,    \
                                                             v3,                        \
                                                             v4 ) );                    \
    }

#define STL_PARSER_ALTER_SYSTEM_DATABASE_CLAUSE____CLOSE( )                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 =                                                          \
                          qlpMakeAlterSystemShutdownDatabase( param,                    \
                                                              a1,                       \
                                                              KNL_SHUTDOWN_PHASE_CLOSE, \
                                                              v3 ) );                   \
    }

/**
 * open_database_option
 */

#define STL_PARSER_OPEN_DATABASE____EMPTY()     \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_OPEN_DATABASE____READ_WRITE()                                \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          SML_DATA_ACCESS_MODE_READ_WRITE ) );  \
    }

#define STL_PARSER_OPEN_DATABASE____READ_ONLY()                                 \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          SML_DATA_ACCESS_MODE_READ_ONLY ) );   \
    }

/**
 * open_log_option
 */

#define STL_PARSER_OPEN_LOG____NORESETLOGS()             \
    {                                                    \
        STL_PARSER_CHECK(                                \
            v0 = qlpMakeIntParamConstant(                \
                param,                                   \
                a1,                                      \
                SML_STARTUP_LOG_OPTION_NORESETLOGS ) );  \
    }

#define STL_PARSER_OPEN_LOG____RESETLOGS()               \
    {                                                    \
        STL_PARSER_CHECK(                                \
            v0 = qlpMakeIntParamConstant(                \
                param,                                   \
                a1,                                      \
                SML_STARTUP_LOG_OPTION_RESETLOGS ) );    \
    }

/**
 * close_database_option
 */

#define STL_PARSER_CLOSE_DATABASE____EMPTY()    \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_CLOSE_DATABASE____NORMAL()                           \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeIntParamConstant( param,                        \
                                          a1,                           \
                                          KNL_SHUTDOWN_MODE_NORMAL ) ); \
    }

#define STL_PARSER_CLOSE_DATABASE____TRANSACTIONAL()                            \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          KNL_SHUTDOWN_MODE_TRANSACTIONAL ) );  \
    }

#define STL_PARSER_CLOSE_DATABASE____IMMEDIATE()                            \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            v0 = qlpMakeIntParamConstant( param,                            \
                                          a1,                               \
                                          KNL_SHUTDOWN_MODE_IMMEDIATE ) );  \
    }

#define STL_PARSER_CLOSE_DATABASE____OPERATIONAL()                              \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          KNL_SHUTDOWN_MODE_OPERATIONAL ) );    \
    }

#define STL_PARSER_CLOSE_DATABASE____ABORT()                            \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeIntParamConstant( param,                        \
                                          a1,                           \
                                          KNL_SHUTDOWN_MODE_ABORT ) );  \
    }

/**
 * open_database_option
 */

#define STL_PARSER_OPEN_DATABASE____EMPTY()     \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_OPEN_DATABASE____READ_WRITE()                                \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          SML_DATA_ACCESS_MODE_READ_WRITE ) );  \
    }

#define STL_PARSER_OPEN_DATABASE____READ_ONLY()                                 \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          SML_DATA_ACCESS_MODE_READ_ONLY ) );   \
    }

/**
 * alter_session_statement
 */

#define STL_PARSER_ALTER_SESSION_STATEMENT____( )        \
    {                                                    \
        STL_PARSER_CHECK( v0 = v3 );                     \
    }


/**
 * alter_session_clause
 */

#define STL_PARSER_ALTER_SESSION_CLAUSE____SET( )                               \
    {                                                                           \
        STL_PARSER_CHECK( v0 =                                                  \
                          qlpMakeAlterSession( param,                           \
                                               QLL_STMT_ALTER_SESSION_SET_TYPE, \
                                               a1,                              \
                                               v2,                              \
                                               v4 ) );                          \
    }

#define STL_PARSER_ALTER_SESSION_CLAUSE____SET_DEFAULT( )                       \
    {                                                                           \
        STL_PARSER_CHECK( v0 =                                                  \
                          qlpMakeAlterSession( param,                           \
                                               QLL_STMT_ALTER_SESSION_SET_TYPE, \
                                               a1,                              \
                                               v2,                              \
                                               NULL ) );                        \
    }


/**
 * lock_table_statement : qlpLockTable
 */

#define STL_PARSER_LOCK_TABLE_STATEMENT____( )                  \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeLockTable( param,         \
                                                 a1,            \
                                                 v3,            \
                                                 v5,            \
                                                 a5,            \
                                                 v7 ) );        \
    }


/**
 * table_name_list : qlpList : qlpRelInfo
 */

#define STL_PARSER_TABLE_NAME_LIST____TABLE( )                                          \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
                                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                qlpMakeObjectName( param, v1 ) ) );     \
    }

#define STL_PARSER_TABLE_NAME_LIST____TABLE_LIST( )                                     \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                v1,                                     \
                                                qlpMakeObjectName( param, v3 ) ) );     \
    }


/**
 * lock_mode : qlpValue : integer
 */

#define STL_PARSER_LOCK_MODE____SHARE( )                                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        QLP_LOCK_MODE_SHARE ) );        \
    }

#define STL_PARSER_LOCK_MODE____EXCLUSIVE( )                                            \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        QLP_LOCK_MODE_EXCLUSIVE ) );    \
    }

#define STL_PARSER_LOCK_MODE____ROW_SHARE( )                                            \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        QLP_LOCK_MODE_ROW_SHARE ) );    \
    }

#define STL_PARSER_LOCK_MODE____ROW_EXCLUSIVE( )                                                \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                  \
                                                        a1,                                     \
                                                        QLP_LOCK_MODE_ROW_EXCLUSIVE ) );        \
    }

#define STL_PARSER_LOCK_MODE____SHARE_ROW_EXCLUSIVE( )                                          \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                  \
                                                        a1,                                     \
                                                        QLP_LOCK_MODE_SHARE_ROW_EXCLUSIVE ) );  \
    }


/**
 * wait_clause : qlpValue : float
 */

#define STL_PARSER_WAIT_CLAUSE____WAIT_INFINITY( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = NULL );                  \
    }

#define STL_PARSER_WAIT_CLAUSE____NO_WAIT( )                    \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeNumberConstant( param,    \
                                                      a1,       \
                                                      "0" ) );  \
                                                                \
    }

#define STL_PARSER_WAIT_CLAUSE____WAIT_TIME( )  \
    {                                           \
        STL_PARSER_CHECK( v0 = v2 );            \
    }


/**
 * session_function
 */

#define STL_PARSER_SESSION_FUNCTION____CURRENT_CATALOG( )                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_CURRENT_CATALOG,       \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_SESSION_FUNCTION____VERSION( )                               \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_VERSION,       \
                                                a1,                             \
                                                QLP_GET_LAST_POS(a1) - a1,      \
                                                NULL ) );                       \
    }

#define STL_PARSER_SESSION_FUNCTION____USER( )                                  \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_CURRENT_USER,  \
                                                a1,                             \
                                                QLP_GET_LAST_POS(a1) - a1,      \
                                                NULL ) );                       \
    }

#define STL_PARSER_SESSION_FUNCTION____SESSION_USER( )                          \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_SESSION_USER,  \
                                                a1,                             \
                                                QLP_GET_LAST_POS(a1) - a1,      \
                                                NULL ) );                       \
    }

#define STL_PARSER_SESSION_FUNCTION____LOGON_USER( )                            \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_LOGON_USER,    \
                                                a1,                             \
                                                QLP_GET_LAST_POS(a1) - a1,      \
                                                NULL ) );                       \
    }

#define STL_PARSER_SESSION_FUNCTION____CURRENT_SCHEMA( )                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_CURRENT_SCHEMA,        \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }


#define STL_PARSER_SESSION_FUNCTION____SESSION_ID( )                            \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_SESSION_ID,    \
                                                a1,                             \
                                                QLP_GET_LAST_POS(a1) - a1,      \
                                                NULL ) );                       \
    }

#define STL_PARSER_SESSION_FUNCTION____SESSION_SERIAL( )                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_SESSION_SERIAL,        \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_SESSION_FUNCTION____USER_ID( )                               \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_USER_ID,       \
                                                a1,                             \
                                                QLP_GET_LAST_POS(a1) - a1,      \
                                                NULL ) );                       \
    }


/**
 * AGGREGATE FUNCTION
 */

#define STL_PARSER_AGGREGATE_FUNCTION____UNARY( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

/* Unary Aggregation */
#define STL_PARSER_UNARY_AGGREGATION____SUM( )                                  \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v4 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeAggregation( param,                       \
                                                   a1,                          \
                                                   KNL_BUILTIN_AGGREGATION_SUM, \
                                                   (qlpSetQuantifier)v3,        \
                                                   sList,                       \
                                                   v6 ) );                      \
    }

#define STL_PARSER_UNARY_AGGREGATION____COUNT( )                                    \
    {                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sList,                              \
                                                v4 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeAggregation( param,                           \
                                                   a1,                              \
                                                   KNL_BUILTIN_AGGREGATION_COUNT,   \
                                                   (qlpSetQuantifier)v3,            \
                                                   sList,                           \
                                                   v6 ) );                          \
    }

#define STL_PARSER_UNARY_AGGREGATION____COUNT_ASTERISK( )                                   \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAggregation( param,                                   \
                                                   a1,                                      \
                                                   KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK,  \
                                                   QLP_SET_QUANTIFIER_ALL,                  \
                                                   NULL,                                    \
                                                   v5 ) );                                  \
    }

#define STL_PARSER_UNARY_AGGREGATION____MIN( )                                  \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v4 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeAggregation( param,                       \
                                                   a1,                          \
                                                   KNL_BUILTIN_AGGREGATION_MIN, \
                                                   (qlpSetQuantifier)v3,        \
                                                   sList,                       \
                                                   v6 ) );                      \
    }

#define STL_PARSER_UNARY_AGGREGATION____MAX( )                                  \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v4 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeAggregation( param,                       \
                                                   a1,                          \
                                                   KNL_BUILTIN_AGGREGATION_MAX, \
                                                   (qlpSetQuantifier)v3,        \
                                                   sList,                       \
                                                   v6 ) );                      \
    }

#define STL_PARSER_UNARY_AGGREGATION____AVG( )                                          \
    {                                                                                   \
        qlpList * sArgList1 = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
        qlpList * sArgList2 = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
        qlpList * sFuncList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
        void    * sArg1 = NULL;                                                         \
        void    * sArg2 = NULL;                                                         \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sArgList1,                              \
                                                v4 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( sArg1 = qlpMakeAggregation( param,                            \
                                                      a1,                               \
                                                      KNL_BUILTIN_AGGREGATION_SUM,      \
                                                      (qlpSetQuantifier)v3,             \
                                                      sArgList1,                        \
                                                      v6 ) );                           \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sArgList2,                              \
                                                v4 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( sArg2 = qlpMakeAggregation( param,                            \
                                                      a1,                               \
                                                      KNL_BUILTIN_AGGREGATION_COUNT,    \
                                                      (qlpSetQuantifier)v3,             \
                                                      sArgList2,                        \
                                                      v6 ) );                           \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sFuncList,                              \
                                                sArg1 ) );                              \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sFuncList,                              \
                                                sArg2 ) );                              \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_DIV,                   \
                                                a1,                                     \
                                                QLP_GET_LAST_POS( a6 ) - a1,            \
                                                sFuncList ) );                          \
    }


/**
 * CASE EXPRESSION
 */

/**
 * case_expression ( CASE_ABBREVIATION )
 */

#define STL_PARSER_CASE_ABBREVIATION____( )              \
     {                                                   \
         STL_PARSER_CHECK( v0 = v1 );                    \
     }

/**
 * case_expression ( CASE_SPECIFICATION )
 */

#define STL_PARSER_CASE_SPECIFICATION____( )            \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

/**
 * case_abbreviation ( NULLIF )
 */

#define STL_PARSER_CASE_ABBREVIATION____NULLIF( )                               \
    {                                                                           \
        qlpList     * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeCaseExprForNullif( param,                         \
                                                         a1,                            \
                                                         QLP_GET_LAST_POS( a6 ) - a1,   \
                                                         QLL_NULLIF_EXPR,               \
                                                         KNL_BUILTIN_FUNC_NULLIF,       \
                                                         sList ) );                     \
    }
     

/**
 * case_abbreviation ( COALESCE )
 */
#define STL_PARSER_CASE_ABBREVIATION____COALESCE( )                               \
    {                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakeCaseExprForCoalesce( param,                            \
                                                           a1,                               \
                                                           QLP_GET_LAST_POS( a4 ) - a1,      \
                                                           QLL_COALESCE_EXPR,                \
                                                           KNL_BUILTIN_FUNC_COALESCE,        \
                                                           v3 ) );                           \
    }

/**
 * coalesce_expr_list
 */

#define STL_PARSER_CASE_ABBREVIATION_LIST____COALESCE_EXPR( )           \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
        STL_PARSER_CHECK( v0 = sList );                                 \
    }

#define STL_PARSER_CASE_ABBREVIATION_LIST____COALESCE_EXPR_LIST( )      \
    {                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

/**
 * case_specification 
 */

#define STL_PARSER_CASE_SPECIFICATION____SIMPLE( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_CASE_SPECIFICATION____SEARCHED()     \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

/**
 * simple_case
 */

#define STL_PARSER_CASE_SPECIFICATION____SIMPLE_CASE( )                                             \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeCaseExprForSimpleCase( param,                                 \
                                                             a1,                                    \
                                                             QLP_GET_LAST_POS( a5 ) - a1,           \
                                                             QLL_CASE_SIMPLE_EXPR,                  \
                                                             KNL_BUILTIN_FUNC_CASE,                 \
                                                             v2,                                    \
                                                             v3,                                    \
                                                             v4 ) );                                \
    }

/**
 * searched_case
 */

#define STL_PARSER_CASE_SPECIFICATION____SEARCHED_CASE( )                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCaseExprForSearchedCase( param,                   \
                                                               a1,                              \
                                                               QLP_GET_LAST_POS( a4 ) - a1,     \
                                                               QLL_CASE_SEARCHED_EXPR,          \
                                                               KNL_BUILTIN_FUNC_CASE,           \
                                                               v2,                              \
                                                               v3 ) );                          \
    }

/**
 * simple_when_clause_list
 */

#define STL_PARSER_CASE_SPECIFICATION_LIST____SIMPLE_WHEN_CLAUSE( )     \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }                                                         

#define STL_PARSER_CASE_SPECIFICATION_LIST____SIMPLE_WHEN_CLAUSE_LIST( )  \
    {                                                                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v2 ) );        \
        STL_PARSER_CHECK( v0 = v1 );                                      \
    }
   
/**
 * simple_when_clause
 */

#define STL_PARSER_CASE_SPECIFICATION____SIMPLE_WHEN_CLAUSE( )           \
    {                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeCaseWhenClause( param,             \
                                                      (qllNode *)v2,     \
                                                      v4 ) );            \
    }                                                    

/**
 * searched_when_clause_list
 */

#define STL_PARSER_CASE_SPECIFICATION_LIST____SEARCHED_WHEN_CLAUSE( )   \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }                                                         

#define STL_PARSER_CASE_SPECIFICATION_LIST____SEARCHED_WHEN_CLAUSE_LIST( )  \
    {                                                                       \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v2 ) );          \
        STL_PARSER_CHECK( v0 = v1 );                                        \
    }
    
    
/**
 * searched_when_clause
 */

#define STL_PARSER_CASE_SPECIFICATION____SEARCHED_WHEN_CLAUSE( )       \
    {                                                                  \
        STL_PARSER_CHECK( v0 = qlpMakeCaseWhenClause( param,           \
                                                      v2,              \
                                                      v4 ) );          \
    }                                                  

/**
 * else_clause
 */

#define STL_PARSER_CASE_SPECIFICATION____ELSE_CLAUSE_EMPTY( )          \
    {                                                                  \
        STL_PARSER_CHECK( v0 = qlpMakeNullConstant( param, 0 ) );      \
    }

#define STL_PARSER_CASE_SPECIFICATION____ELSE_CLAUSE( )        \
    {                                                          \
        STL_PARSER_CHECK( v0 = v2 );                           \
    }

/**
 * case_operand
 */

#define STL_PARSER_CASE_SPECIFICATION____CASE_OPERAND_ROW_VALUE( )  \
    {                                                               \
        STL_PARSER_CHECK( v0 = v1 );                                \
    }

#define STL_PARSER_CASE_SPECIFICATION_LIST____CASE_OPERAND_VALUE( )     \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
        STL_PARSER_CHECK( v0 = sList );                                 \
    }

/**
 * when_operand
 */

#define STL_PARSER_CASE_SPECIFICATION____WHEN_OPERAND_ROW_VALUE( ) \
    {                                                              \
        STL_PARSER_CHECK( v0 = v1 );                               \
    }

#define STL_PARSER_CASE_SPECIFICATION_LIST____WHEN_OPERAND_VALUE( )     \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
        STL_PARSER_CHECK( v0 = sList );                                 \
    }

/**
 * result
 */

#define STL_PARSER_CASE_SPECIFICATION____RESULT( )             \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }

/**
 * result_expression
 */

#define STL_PARSER_CASE_SPECIFICATION____RESULT_EXPRESSION( )  \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }



/**
 * SET QUANTIFIER
 */

#define STL_PARSER_SET_QUANTIFIER____DEFAULT( )                         \
    {                                                                   \
        STL_PARSER_CHECK( v0 = (qllNode*)QLP_SET_QUANTIFIER_ALL );      \
    }

#define STL_PARSER_SET_QUANTIFIER____ALL( )                             \
    {                                                                   \
        STL_PARSER_CHECK( v0 = (qllNode*)QLP_SET_QUANTIFIER_ALL );      \
    }

#define STL_PARSER_SET_QUANTIFIER____DISTINCT( )                        \
    {                                                                   \
        STL_PARSER_CHECK( v0 = (qllNode*)QLP_SET_QUANTIFIER_DISTINCT ); \
    }


/**
 * FILTER CLAUSE
 */

#define STL_PARSER_FILTER_CLAUSE____EMPTY( )    \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_FILTER_CLAUSE____FILTER( )   \
    {                                           \
        STL_PARSER_CHECK( v0 = v4 );            \
    }


/**
 * COMMENT ON STATEMENT
 */

#define STL_PARSER_COMMENT_STATEMENT____( )                     \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeCommentOnStmt( param,     \
                                                     a1,        \
                                                     v3,        \
                                                     v5 ) );    \
    }

#define STL_PARSER_COMMENT_OBJECT____DATABASE( )                            \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeCommentObject( param,                 \
                                                     ELL_OBJECT_CATALOG,    \
                                                     a1,                    \
                                                     NULL,                  \
                                                     NULL,                  \
                                                     NULL ) );              \
    }

#define STL_PARSER_COMMENT_OBJECT____PROFILE()                              \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeCommentObject( param,                 \
                                                     ELL_OBJECT_PROFILE,    \
                                                     a1,                    \
                                                     v2,                    \
                                                     NULL,                  \
                                                     NULL ) );              \
    }

#define STL_PARSER_COMMENT_OBJECT____AUTHORIZATION( )                           \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeCommentObject( param,                     \
                                                     ELL_OBJECT_AUTHORIZATION,  \
                                                     a1,                        \
                                                     v2,                        \
                                                     NULL,                      \
                                                     NULL ) );                  \
    }

#define STL_PARSER_COMMENT_OBJECT____TABLESPACE( )                          \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeCommentObject( param,                 \
                                                     ELL_OBJECT_TABLESPACE, \
                                                     a1,                    \
                                                     v2,                    \
                                                     NULL,                  \
                                                     NULL ) );              \
    }

#define STL_PARSER_COMMENT_OBJECT____SCHEMA( )                          \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCommentObject( param,             \
                                                     ELL_OBJECT_SCHEMA, \
                                                     a1,                \
                                                     v2,                \
                                                     NULL,              \
                                                     NULL ) );          \
    }

#define STL_PARSER_COMMENT_OBJECT____TABLE( )                           \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCommentObject( param,             \
                                                     ELL_OBJECT_TABLE,  \
                                                     a1,                \
                                                     NULL,              \
                                                     v2,                \
                                                     NULL ) );          \
    }

#define STL_PARSER_COMMENT_OBJECT____INDEX( )                           \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCommentObject( param,             \
                                                     ELL_OBJECT_INDEX,  \
                                                     a1,                \
                                                     NULL,              \
                                                     v2,                \
                                                     NULL ) );          \
    }

#define STL_PARSER_COMMENT_OBJECT____SEQUENCE( )                            \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeCommentObject( param,                 \
                                                     ELL_OBJECT_SEQUENCE,   \
                                                     a1,                    \
                                                     NULL,                  \
                                                     v2,                    \
                                                     NULL ) );              \
    }

#define STL_PARSER_COMMENT_OBJECT____CONSTRAINT( )                          \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeCommentObject( param,                 \
                                                     ELL_OBJECT_CONSTRAINT, \
                                                     a1,                    \
                                                     NULL,                  \
                                                     v2,                    \
                                                     NULL ) );              \
    }

#define STL_PARSER_COMMENT_OBJECT____COLUMN( )                          \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCommentObject( param,             \
                                                     ELL_OBJECT_COLUMN, \
                                                     a1,                \
                                                     NULL,              \
                                                     NULL,              \
                                                     v2 ) );            \
    }

/*
 * SET CONSTRAINT
 */

#define STL_PARSER_SET_CONSTRAINT____NAME_DEFERRED()                                                \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeSetConstraint( param,                                         \
                                                     a1,                                            \
                                                     QLL_SET_CONSTRAINT_MODE_CONSTRAINT_DEFERRED,   \
                                                     v3 ) );                                        \
    }

#define STL_PARSER_SET_CONSTRAINT____ALL_DEFERRED()                                         \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeSetConstraint( param,                                 \
                                                     a1,                                    \
                                                     QLL_SET_CONSTRAINT_MODE_ALL_DEFERRED,  \
                                                     NULL ) );                              \
    }

#define STL_PARSER_SET_CONSTRAINT____NAME_IMMEDIATE()                                               \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeSetConstraint( param,                                         \
                                                     a1,                                            \
                                                     QLL_SET_CONSTRAINT_MODE_CONSTRAINT_IMMEDIATE,  \
                                                     v3 ) );                                        \
    }

#define STL_PARSER_SET_CONSTRAINT____ALL_IMMEDIATE()                                        \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeSetConstraint( param,                                 \
                                                     a1,                                    \
                                                     QLL_SET_CONSTRAINT_MODE_ALL_IMMEDIATE, \
                                                     NULL ) );                              \
    }



#define STL_PARSER_CONSTRAINT_NAME_LIST()       \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

/*
 * SET TRANSACTION STATEMENT
 */

#define STL_PARSER_SET_TRANSACTION____()                        \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeSetTransaction( param,    \
                                                      a3,       \
                                                      v3 ) );   \
    }

#define STL_PARSER_TRANSACTION_MODE____TRANSACTION_ACCESS_MODE()                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeSetTransactionMode( param,                        \
                                                          a1,                           \
                                                          QLP_TRANSACTION_ATTR_ACCESS,  \
                                                          v1 ) );                       \
    }

#define STL_PARSER_TRANSACTION_MODE____ISOLATION_LEVEL()                                        \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeSetTransactionMode( param,                                \
                                                          a1,                                   \
                                                          QLP_TRANSACTION_ATTR_ISOLATION_LEVEL, \
                                                          v1 ) );                               \
    }

#define STL_PARSER_TRANSACTION_MODE____UNIQUE_INTEGRITY()                                           \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeSetTransactionMode( param,                                    \
                                                          a1,                                       \
                                                          QLP_TRANSACTION_ATTR_UNIQUE_INTEGRITY,    \
                                                          v1 ) );                                   \
    }

#define STL_PARSER_TRANSACTION_ACCESS_MODE____READ_ONLY()                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        SML_STMT_ATTR_READONLY ) );     \
    }

#define STL_PARSER_TRANSACTION_ACCESS_MODE____READ_WRITE()                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        SML_STMT_ATTR_UPDATABLE ) );    \
    }

#define STL_PARSER_ISOLATION_LEVEL____()        \
    {                                           \
        STL_PARSER_CHECK( v0 = v3 );            \
    }

#define STL_PARSER_LEVEL_OF_ISOLATION____READ_COMMITTED()                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        SML_TIL_READ_COMMITTED ) );     \
    }

#define STL_PARSER_LEVEL_OF_ISOLATION____SERIALIZABLE()                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        SML_TIL_SERIALIZABLE ) );       \
    }

#define STL_PARSER_UNIQUE_INTEGRITY____DEFAULT()                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        QLL_UNIQUE_MODE_DEFAULT ) );    \
    }

#define STL_PARSER_UNIQUE_INTEGRITY____DEFERRED()                                       \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        QLL_UNIQUE_MODE_DEFERRED ) );   \
    }

/*
 * SET SESSION CHARACTERISTICS STATEMENT
 */

#define STL_PARSER_SET_SESSION_CHARACTERISTICS____()                            \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeSetSessionCharacteristics( param,         \
                                                                 a6,            \
                                                                 v6 ) );        \
    }

/*
 * SET SESSION AUTHORIZATION
 */

#define STL_PARSER_SET_SESSION_AUTHORIZATION()                  \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeSetSessionAuth( param,    \
                                                      a1,       \
                                                      v4 ) );   \
    }

/*
 * SET TIME ZONE STATEMENT
 */
#define STL_PARSER_SET_TIME_ZONE____LOCAL()                     \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeSetTimeZone( param,       \
                                                   a0,          \
                                                   NULL ) );    \
    }

#define STL_PARSER_SET_TIME_ZONE____GMT_OFFSET()                                                \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeSetTimeZone( param,                                       \
                                                   a0,                                          \
                                                   qlpMakeStrConstant( param, a4, v4 ) ) );     \
    }


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

#define STL_PARSER_DECLARE_CURSOR____ISO_TYPE()                 \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDeclareCursorISO( param,  \
                                                        a1,     \
                                                        v2,     \
                                                        v3,     \
                                                        v4,     \
                                                        v6,     \
                                                        v8 ) ); \
    }

#define STL_PARSER_DECLARE_CURSOR____ODBC_TYPE()                    \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeDeclareCursorODBC( param,     \
                                                         a1,        \
                                                         v2,        \
                                                         v3,        \
                                                         v5,        \
                                                         v7 ) );    \
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

#define STL_PARSER_CURSOR_SENSITIVITY____EMPTY()                            \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            v0 = qlpMakeIntParamConstant( param,                            \
                                          0,                                \
                                          ELL_CURSOR_SENSITIVITY_NA ) );    \
    }

#define STL_PARSER_CURSOR_SENSITIVITY____INSENSITIVE()                              \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeIntParamConstant( param,                                    \
                                          a1,                                       \
                                          ELL_CURSOR_SENSITIVITY_INSENSITIVE ) );   \
    }

#define STL_PARSER_CURSOR_SENSITIVITY____SENSITIVE()                            \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          ELL_CURSOR_SENSITIVITY_SENSITIVE ) ); \
    }

#define STL_PARSER_CURSOR_SENSITIVITY____ASENSITIVE()                               \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeIntParamConstant( param,                                    \
                                          a1,                                       \
                                          ELL_CURSOR_SENSITIVITY_ASENSITIVE ) );    \
    }


/*
 * cursor_scrollability
 */

#define STL_PARSER_CURSOR_SCROLLABILITY____EMPTY()                          \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            v0 = qlpMakeIntParamConstant( param,                            \
                                          0,                                \
                                          ELL_CURSOR_SCROLLABILITY_NA ) );  \
    }

#define STL_PARSER_CURSOR_SCROLLABILITY____NO_SCROLL()                              \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeIntParamConstant( param,                                    \
                                          a1,                                       \
                                          ELL_CURSOR_SCROLLABILITY_NO_SCROLL ) );   \
    }

#define STL_PARSER_CURSOR_SCROLLABILITY____SCROLL()                             \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          ELL_CURSOR_SCROLLABILITY_SCROLL ) );  \
    }

/*
 * cursor odbc type
 */

#define STL_PARSER_CURSOR_ODBC_TYPE____STATIC()                                 \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          ELL_CURSOR_STANDARD_ODBC_STATIC ) );  \
    }

#define STL_PARSER_CURSOR_ODBC_TYPE____KEYSET()                                 \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          ELL_CURSOR_STANDARD_ODBC_KEYSET ) );  \
    }

#define STL_PARSER_CURSOR_ODBC_TYPE____DYNAMIC()                                \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          ELL_CURSOR_STANDARD_ODBC_DYNAMIC ) ); \
    }

/*
 * cursor_holdability
 */

#define STL_PARSER_CURSOR_HOLDABILITY____EMPTY()                                \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          ELL_CURSOR_DEFAULT_HOLDABILITY ) );   \
    }

#define STL_PARSER_CURSOR_HOLDABILITY____WITH_HOLD()                            \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeIntParamConstant( param,                                \
                                          a1,                                   \
                                          ELL_CURSOR_HOLDABILITY_WITH_HOLD ) ); \
    }

#define STL_PARSER_CURSOR_HOLDABILITY____WITHOUT_HOLD()                             \
    {                                                                               \
        STL_PARSER_CHECK(                                                           \
            v0 = qlpMakeIntParamConstant( param,                                    \
                                          a1,                                       \
                                          ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD ) );  \
    }

/*
 * cursor_query
 */

#define STL_PARSER_CURSOR_QUERY____SELECT()     \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_CURSOR_QUERY____INSERT_RETURNING_QUERY() \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_CURSOR_QUERY____UPDATE_RETURNING_QUERY() \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_CURSOR_QUERY____DELETE_RETURNING_QUERY() \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

/*
 * open_statement
 */

#define STL_PARSER_OPEN_CURSOR____()                        \
    {                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeOpenCursor( param,    \
                                                  a1,       \
                                                  v2 ) );   \
    }

#define STL_PARSER_OPEN_DYNAMIC_CURSOR____()    \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a3 );           \
    }

/*
 * close_statement
 */

#define STL_PARSER_CLOSE_CURSOR____()                       \
    {                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeCloseCursor( param,   \
                                                   a1,      \
                                                   v2 ) );  \
    }

/*
 * fetch_statement
 */


#define STL_PARSER_FETCH_CURSOR____WITH_FROM()                  \
    {                                                           \
        qlpIntoClause * sInto = qlpMakeIntoClause( param,       \
                                                   a5,          \
                                                   v6 );        \
        STL_PARSER_CHECK( v0 = sInto );                         \
        STL_PARSER_CHECK( v0 = qlpMakeFetchCursor( param,       \
                                                   a1,          \
                                                   v4,          \
                                                   v2,          \
                                                   sInto ) );   \
    }

#define STL_PARSER_FETCH_CURSOR____WITHOUT_FROM()               \
    {                                                           \
        qlpIntoClause * sInto = qlpMakeIntoClause( param,       \
                                                   a4,          \
                                                   v5 );        \
        STL_PARSER_CHECK( v0 = sInto );                         \
        STL_PARSER_CHECK( v0 = qlpMakeFetchCursor( param,       \
                                                   a1,          \
                                                   v3,          \
                                                   v2,          \
                                                   sInto ) );   \
    }

#define STL_PARSER_FETCH_ORIENTATION____EMPTY()                         \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFetchOrientation( param,          \
                                                        0,              \
                                                        QLL_FETCH_NEXT, \
                                                        NULL ) );       \
    }

#define STL_PARSER_FETCH_ORIENTATION____NEXT()                          \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFetchOrientation( param,          \
                                                        a1,             \
                                                        QLL_FETCH_NEXT, \
                                                        NULL ) );       \
    }

#define STL_PARSER_FETCH_ORIENTATION____PRIOR()                             \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeFetchOrientation( param,              \
                                                        a1,                 \
                                                        QLL_FETCH_PRIOR,    \
                                                        NULL ) );           \
    }

#define STL_PARSER_FETCH_ORIENTATION____FIRST()                             \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeFetchOrientation( param,              \
                                                        a1,                 \
                                                        QLL_FETCH_FIRST,    \
                                                        NULL ) );           \
    }

#define STL_PARSER_FETCH_ORIENTATION____LAST()                          \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFetchOrientation( param,          \
                                                        a1,             \
                                                        QLL_FETCH_LAST, \
                                                        NULL ) );       \
    }

#define STL_PARSER_FETCH_ORIENTATION____CURRENT()                           \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeFetchOrientation( param,              \
                                                        a1,                 \
                                                        QLL_FETCH_CURRENT,  \
                                                        NULL ) );           \
    }

#define STL_PARSER_FETCH_ORIENTATION____ABSOLUTE()                          \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeFetchOrientation( param,              \
                                                        a1,                 \
                                                        QLL_FETCH_ABSOLUTE, \
                                                        v2 ) );             \
    }

#define STL_PARSER_FETCH_ORIENTATION____RELATIVE()                          \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeFetchOrientation( param,              \
                                                        a1,                 \
                                                        QLL_FETCH_RELATIVE, \
                                                        v2 ) );             \
    }

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
        param->mParseTree = v1;                 \
    }

/**
 * phrase_default_expression
 */
#define STL_PARSER_PHRASE____DEFAULT_EXPRESSION()   \
    {                                               \
        param->mParseTree = v1;                     \
    }


/**
 * VIEW view_name ( column_list )
 */
#define STL_PHRASE_VIEWED_TABLE____BASIC()                          \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakePharseViewedTable( param,     \
                                                         a1,        \
                                                         v2,        \
                                                         v3 ) );    \
    }

/**
 * DEFAULT expression
 */
#define STL_PHRASE_DEFAULT_EXPRESSION____BASIC()                                        \
    {                                                                                   \
        qlpValueExpr  * sValueExpr = NULL;                                              \
        STL_PARSER_CHECK( sValueExpr = qlpMakeValueExpr( param,                         \
                                                         a2,                            \
                                                         QLP_GET_LAST_POS( a2 ) - a2,   \
                                                         v2 ) );                        \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakePhraseDefaultExpr( param,                         \
                                                         a1,                            \
                                                         sValueExpr ) );                \
    }

/** @} qlpParser */


#endif /* _QLPPARSERMACROPART_3_H_ */

