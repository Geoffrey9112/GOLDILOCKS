/*******************************************************************************
 * qlpParserMacroPart_2.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        vId: qlpParserMacroPart_2.h 2754 2011-12-15 02:27:23Z jhkim v
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLPPARSERMACROPART_2_H_
#define _QLPPARSERMACROPART_2_H_ 1

/**
 * @file qlpParserMacroPart_2.h
 * @brief SQL Processor Layer Parser macro Part #2
 */

#include <qlpParse.h>

/**
 * @addtogroup qlpParser
 * @{
 */


/**
 * VIEW DEFINITION
 */

#define STL_PARSER_VIEW_DEFINITION____BASIC()                           \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeViewDef( param,                                 \
                                 a1,                                    \
                                 (v2 == NULL) ? STL_FALSE : STL_TRUE,   \
                                 (v3 == NULL) ? STL_FALSE : STL_TRUE,   \
                                 v5,                                    \
                                 v6,                                    \
                                 v8,                                    \
                                 a4,                                    \
                                 a7 - a4,                               \
                                 a8,                                    \
                                 QLP_GET_LAST_POS( a8 ) - a8 ) );       \
    }

#define STL_PARSER_OR_REPLACE_OPTION____NO_REPLACE() \
    {                                                \
        STL_PARSER_CHECK( v0 = NULL );               \
    }

#define STL_PARSER_OR_REPLACE_OPTION____REPLACE()                       \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

#define STL_PARSER_FORCE_OPTION____NO_FORCE()   \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_FORCE_OPTION____FORCE()                              \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

#define STL_PARSER_VIEW_COLUMN_LIST____EMPTY()  \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_VIEW_COLUMN_LIST____NOT_EMPTY()  \
    {                                               \
        STL_PARSER_CHECK( v0 = v2 );                \
    }

/**
 * DROP VIEW
 */

#define STL_PARSER_DROP_VIEW_STATEMENT()                                \
    {                                                                   \
        STL_PARSER_CHECK(                                               \
            v0 = qlpMakeDropView( param,                                \
                                  a1,                                   \
                                  (v3 == NULL) ? STL_FALSE : STL_TRUE,  \
                                  v4 ) );                               \
    }

/**
 * ALTER VIEW
 */

#define STL_PARSER_ALTER_VIEW_STATEMENT()       \
    {                                           \
        STL_PARSER_CHECK(                       \
            v0 = qlpMakeAlterView( param,       \
                                   a1,          \
                                   v3,          \
                                   v4 ) );      \
    }

#define STL_PARSER_ALTER_VIEW_ACTION____COMPILE()                                           \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                              \
                                                        a1,                                 \
                                                        QLP_ALTER_VIEW_ACTION_COMPILE ) );  \
    }

/**
 * sequence_definition
 */

#define STL_PARSER_SEQUENCE_DEFINITION____NO_OPTION()           \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeSequenceDef( param,       \
                                                   a1,          \
                                                   v3,          \
                                                   NULL ) );    \
    }

#define STL_PARSER_SEQUENCE_DEFINITION____WITH_OPTION()                         \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeSequenceDef( param,                                     \
                                     a1,                                        \
                                     v3,                                        \
                                     qlpMakeSequenceOption( param, v4 ) ) );    \
    }


/**
 * drop_sequence_statement
 */

#define STL_PARSER_DROP_SEQUENCE_STATEMENT()                                \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            v0 = qlpMakeDropSequence( param,                                \
                                      a1,                                   \
                                      (v3 == NULL) ? STL_FALSE : STL_TRUE,  \
                                      v4 ) );                               \
    }

/**
 * alter_sequence_statement
 */

#define STL_PARSER_ALTER_SEQUENCE_STATEMENT()                                   \
    {                                                                           \
        STL_PARSER_CHECK(                                                       \
            v0 = qlpMakeAlterSequence( param,                                   \
                                       a1,                                      \
                                       v3,                                      \
                                       qlpMakeSequenceOption( param, v4 ) ) );  \
    }

#define STL_PARSER_ALTER_SEQUENCE_GENERATOR_OPTIONS____OPTION()         \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
                                                                        \
    }

#define STL_PARSER_ALTER_SEQUENCE_GENERATOR_OPTIONS____OPTION_LIST()    \
    {                                                                   \
        STL_PARSER_CHECK( qlpCheckDuplicateOptions( param, v1, v2 ) );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v2 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


#define STL_PARSER_ALTER_SEQUENCE_GENERATOR_OPTION____RESTART() \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_ALTER_SEQUENCE_GENERATOR_OPTION____BASIC()   \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_ALTER_SEQUENCE_RESTART____RESTART_NO_VALUE()                     \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                               \
                                               a1,                                  \
                                               QLP_SEQUENCE_OPTION_RESTART_WITH,    \
                                               NULL ) );                            \
    }

#define STL_PARSER_ALTER_SEQUENCE_RESTART____RESTRAT_WITH_VALUE()                   \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                               \
                                               a1,                                  \
                                               QLP_SEQUENCE_OPTION_RESTART_WITH,    \
                                               (qllNode *) v3 ) );                  \
    }

/**
 * synonym_definition
 */

#define STL_PARSER_SYNONYM_DEFINITION____PRIVATE()                                     \
    {                                                                                  \
        STL_PARSER_CHECK( v0 = qlpMakeSynonymDef( param,                               \
                                                  a1,                                  \
                                                  (v2 == NULL) ? STL_FALSE : STL_TRUE, \
                                                  STL_FALSE,                           \
                                                  v4,                                  \
                                                  v6 ) );                              \
    }

#define STL_PARSER_SYNONYM_DEFINITION____PUBLIC()                                      \
    {                                                                                  \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                 \
                                                sList,                                 \
                                                v5 ) );                                \
                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeSynonymDef( param,                               \
                                                  a1,                                  \
                                                  (v2 == NULL) ? STL_FALSE : STL_TRUE, \
                                                  STL_TRUE,                            \
                                                  sList,                               \
                                                  v7 ) );                              \
    }

/**
 * drop_synonym_statement
 */

#define STL_PARSER_DROP_SYNONYM_STATEMENT____PRIVATE()                        \
    {                                                                         \
        STL_PARSER_CHECK(                                                     \
            v0 = qlpMakeDropSynonym( param,                                   \
                                     a1,                                      \
                                     STL_FALSE,                               \
                                     (v3 == NULL) ? STL_FALSE : STL_TRUE,     \
                                     v4 ) );                                  \
    }

#define STL_PARSER_DROP_SYNONYM_STATEMENT____PUBLIC()                         \
    {                                                                         \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                        \
                                                sList,                        \
                                                v5 ) );                       \
        STL_PARSER_CHECK(                                                     \
            v0 = qlpMakeDropSynonym( param,                                   \
                                     a1,                                      \
                                     STL_TRUE,                                \
                                     (v4 == NULL) ? STL_FALSE : STL_TRUE,     \
                                     sList ) );                               \
    }

/**
 * index_definition : qllNode
 */

#define STL_PARSER_INDEX_DEFINITION____( )                                          \
    {                                                                               \
        qlpIndexDef  * sNode = qlpMakeIndexDef( param,                              \
                                                a1,                                 \
                                                (stlBool) QLP_GET_INT_VALUE( v2 ),  \
                                                a2,                                 \
                                                STL_FALSE,                          \
                                                STL_FALSE,                          \
                                                v4,                                 \
                                                v6,                                 \
                                                a6,                                 \
                                                v8,                                 \
                                                NULL,                               \
                                                v10 );                              \
        STL_PARSER_CHECK( v0 = (qllNode*) sNode );                                  \
    }

#define STL_PARSER_INDEX_DEFINITION____ATTR_TBS_NAME( )                                 \
    {                                                                                   \
        qlpIndexDef  * sNode = qlpMakeIndexDef( param,                                  \
                                                a1,                                     \
                                                (stlBool) QLP_GET_INT_VALUE( v2 ),      \
                                                a2,                                     \
                                                STL_FALSE,                              \
                                                STL_FALSE,                              \
                                                v4,                                     \
                                                v6,                                     \
                                                a6,                                     \
                                                v8,                                     \
                                                v10,                                    \
                                                v11 );                                  \
        STL_PARSER_CHECK( v0 = (qllNode*) sNode );                                      \
    }

/**
 * index_attribute_list
 */

#define STL_PARSER_INDEX_ATTRIBUTE_LIST____ATTR()                       \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_INDEX_ATTRIBUTE_LIST____ATTR_LIST()                  \
    {                                                                   \
        STL_PARSER_CHECK( qlpCheckDuplicateOptions( param, v1, v2 ) );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v2 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * index_attribute_types
 */

#define STL_PARSER_INDEX_ATTRIBUTE_TYPES____SEGMENT( )                          \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                           \
                                               a1,                              \
                                               QLP_OBJECT_ATTR_SEGMENT_CLAUSE,  \
                                               v1 ) );                          \
    }

#define STL_PARSER_INDEX_ATTRIBUTE_TYPES____PHYSICAL( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_INDEX_ATTRIBUTE_TYPES____LOGGING( )                  \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDefElem( param,                   \
                                               a1,                      \
                                               QLP_OBJECT_ATTR_LOGGING, \
                                               v1 ) );                  \
    }

#define STL_PARSER_INDEX_ATTRIBUTE_TYPES____PARALLEL( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }


/**
 * index_physical_attribute_type
 */

#define STL_PARSER_INDEX_PHYSICAL_ATTRIBUTE_TYPE____INITRANS( )                         \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            v0 = qlpMakeDefElem( param,                                                 \
                                 a1,                                                    \
                                 QLP_OBJECT_ATTR_PHYSICAL_INITRANS,                     \
                                 (qllNode *) qlpMakeStrConstant(param, a2, v2 ) ) );    \
    }


#define STL_PARSER_INDEX_PHYSICAL_ATTRIBUTE_TYPE____MAXTRANS( )                         \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            v0 = qlpMakeDefElem( param,                                                 \
                                 a1,                                                    \
                                 QLP_OBJECT_ATTR_PHYSICAL_MAXTRANS,                     \
                                 (qllNode *) qlpMakeStrConstant(param, a2, v2 ) ) );    \
    }

#define STL_PARSER_INDEX_PHYSICAL_ATTRIBUTE_TYPE____PCTFREE( )                          \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            v0 = qlpMakeDefElem( param,                                                 \
                                 a1,                                                    \
                                 QLP_OBJECT_ATTR_PHYSICAL_PCTFREE,                      \
                                 (qllNode *) qlpMakeStrConstant(param, a2, v2 ) ) );    \
    }

/**
 * index_logging_attribute_type:
 */

#define STL_PARSER_INDEX_LOGGING_ATTRIBUTE_TYPE____LOGGING( )           \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

#define STL_PARSER_INDEX_LOGGING_ATTRIBUTE_TYPE____NOLOGGING( ) \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_FALSE ) );  \
    }

/**
 * index_parallel_attribute_type
 */

#define STL_PARSER_INDEX_PARALLEL_ATTRIBUTE_TYPE____NOPARALLEL( )                       \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            v0 = qlpMakeDefElem( param,                                                 \
                                 a1,                                                    \
                                 QLP_OBJECT_ATTR_PARALLEL,                              \
                                 (qllNode *) qlpMakeStrConstant(param, a1, "1" ) ) );   \
    }

#define STL_PARSER_INDEX_PARALLEL_ATTRIBUTE_TYPE____PARALLEL_EMPTY( )                   \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            v0 = qlpMakeDefElem( param,                                                 \
                                 a1,                                                    \
                                 QLP_OBJECT_ATTR_PARALLEL,                              \
                                 (qllNode *) qlpMakeStrConstant(param, a1, "0" ) ) );   \
    }

#define STL_PARSER_INDEX_PARALLEL_ATTRIBUTE_TYPE____PARALLEL_INTEGER( )                 \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            v0 = qlpMakeDefElem( param,                                                 \
                                 a1,                                                    \
                                 QLP_OBJECT_ATTR_PARALLEL,                              \
                                 (qllNode *) qlpMakeStrConstant(param, a2, v2 ) ) );    \
    }

/**
 * index_unique_clause : qlpValue : integer
 */

#define STL_PARSER_INDEX_UNIQUE_CLAUSE____NOT_UNIQUE( )                 \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_FALSE ) );  \
    }

#define STL_PARSER_INDEX_UNIQUE_CLAUSE____UNIQUE( )                     \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }


/**
 * index_column_list : qlpList
 */

#define STL_PARSER_INDEX_COLUMN_LIST____COLUMN( )                       \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_INDEX_COLUMN_LIST____COLUMN_LIST( )                  \
    {                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * index_column_clause : qlpIndexElement
 */

#define STL_PARSER_INDEX_COLUMN_CLAUSE____NAME( )               \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIndexElement( param,      \
                                                    a1,         \
                                                    v1,         \
                                                    NULL,       \
                                                    NULL,       \
                                                    NULL ) );   \
    }

#define STL_PARSER_INDEX_COLUMN_CLAUSE____NAME_SORT( )          \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIndexElement( param,      \
                                                    a1,         \
                                                    v1,         \
                                                    v2,         \
                                                    NULL,       \
                                                    NULL ) );   \
    }

#define STL_PARSER_INDEX_COLUMN_CLAUSE____NAME_NULLS( )         \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIndexElement( param,      \
                                                    a1,         \
                                                    v1,         \
                                                    NULL,       \
                                                    v2,         \
                                                    NULL ) );   \
    }

#define STL_PARSER_INDEX_COLUMN_CLAUSE____NAME_SORT_NULLS( )    \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIndexElement( param,      \
                                                    a1,         \
                                                    v1,         \
                                                    v2,         \
                                                    v3,         \
                                                    NULL ) );   \
    }


/**
 * sort_order_clause
 */

#define STL_PARSER_SORT_ORDER_CLAUSE____ASC( )                  \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                a1,              \
                                                STL_TRUE ) );   \
    }

#define STL_PARSER_SORT_ORDER_CLAUSE____DESC( )                 \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                a1,              \
                                                STL_FALSE ) );  \
    }


/**
 * nulls_order_clause
 */

#define STL_PARSER_NULLS_ORDER_CLAUSE____FIRST( )               \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                a1,              \
                                                STL_TRUE ) );  \
    }

#define STL_PARSER_NULLS_ORDER_CLAUSE____LAST( )                        \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_FALSE ) );  \
    }


/**
 * drop_if_exists_option
 */

#define STL_PARSER_DROP_IF_EXISTS____EMPTY()    \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_DROP_IF_EXISTS____EXISTS()                           \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

/**
 * drop_index_statement : qlpDropIndex
 */

#define STL_PARSER_DROP_INDEX_STATEMENT()                                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDropIndex( param,                                 \
                                                 a1,                                    \
                                                 (v3 == NULL) ? STL_FALSE : STL_TRUE,   \
                                                 v4 ) );                                \
    }


/**
 * drop_table_statement : qlpDropTable
 */

#define STL_PARSER_DROP_TABLE_STATEMENT( )                                  \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            v0 = qlpMakeDropTable( param,                                   \
                                   a1,                                      \
                                   (v3 == NULL) ? STL_FALSE : STL_TRUE,     \
                                   v4,                                      \
                                   (v5 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }


/**
 * drop_behavior
 */

#define STL_PARSER_DROP_BEHAVIOR____EMPTY()     \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_DROP_BEHAVIOR____CASCADE()                           \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }


/**
 * ALTER TABLE STATEMENT
 */

#define STL_PARSER_ALTER_TABLE____ADD_COLUMN()  \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_ALTER_TABLE____DROP_COLUMN() \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }
#define STL_PARSER_ALTER_TABLE____ALTER_COLUMN()    \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }
#define STL_PARSER_ALTER_TABLE____RENAME_COLUMN()   \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_ALTER_TABLE____ADD_CONSTRAINT()  \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_ALTER_TABLE____DROP_CONSTRAINT() \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_ALTER_TABLE____ALTER_CONSTRAINT()    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }
/*
#define STL_PARSER_ALTER_TABLE____RENAME_CONSTRAINT() \
    { }
*/
#define STL_PARSER_ALTER_TABLE____ALTER_PHYSICAL() \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_ALTER_TABLE____ADD_SUPPLEMENTAL_LOG() \
    { }
#define STL_PARSER_ALTER_TABLE____DROP_SUPPLEMENTAL_LOG() \
    { }
#define STL_PARSER_ALTER_TABLE____RENAME_TABLE()    \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }


/*
 * alter_table_add_column_definition
 */

#define STL_PARSER_ADD_ONE_COLUMN____WITH_COLUMN() \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v6 ) );   \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAddColumn( param,       \
                                                           a1,          \
                                                           v3,          \
                                                           sList ) );   \
    }

#define STL_PARSER_ADD_ONE_COLUMN____WITHOUT_COLUMN()                   \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v5 ) );   \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAddColumn( param,       \
                                                           a1,          \
                                                           v3,          \
                                                           sList ) );   \
    }

#define STL_PARSER_ADD_MULTI_COLUMN____WITH_COLUMN()                \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAddColumn( param,   \
                                                           a1,      \
                                                           v3,      \
                                                           v7 ) );  \
    }

#define STL_PARSER_ADD_MULTI_COLUMN____WITHOUT_COLUMN()             \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAddColumn( param,   \
                                                           a1,      \
                                                           v3,      \
                                                           v6 ) );  \
    }

/*
 * add_column_definition_list
 */

#define STL_PARSER_ADD_COLUMN_DEFINITION____ELEMENT()                   \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
        STL_PARSER_CHECK( v0 = sList );                                 \
    }

#define STL_PARSER_ADD_COLUMN_DEFINITION____LIST()                  \
    {                                                               \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );  \
        STL_PARSER_CHECK( v0 = v1 );                                \
    }

/*
 * alter_table_drop_column_definition
 */

#define STL_PARSER_SET_UNUSED_ONE_COLUMN____WITH_COLUMN()                                           \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v7 ) );                               \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableDropColumn( param,                                  \
                                                            a1,                                     \
                                                            v3,                                     \
                                                            a4,                                     \
                                                            QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN, \
                                                            sList,                                  \
                                                            (v8 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }


#define STL_PARSER_SET_UNUSED_ONE_COLUMN____WITHOUT_COLUMN()                                        \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v6 ) );                               \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableDropColumn( param,                                  \
                                                            a1,                                     \
                                                            v3,                                     \
                                                            a4,                                     \
                                                            QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN, \
                                                            sList,                                  \
                                                            (v7 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

#define STL_PARSER_SET_UNUSED_MULTI_COLUMN____WITH_COLUMN()                                         \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableDropColumn( param,                                  \
                                                            a1,                                     \
                                                            v3,                                     \
                                                            a4,                                     \
                                                            QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN, \
                                                            v8,                                     \
                                                            (v10 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

#define STL_PARSER_SET_UNUSED_MULTI_COLUMN____WITHOUT_COLUMN()                                      \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableDropColumn( param,                                  \
                                                            a1,                                     \
                                                            v3,                                     \
                                                            a4,                                     \
                                                            QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN, \
                                                            v7,                                     \
                                                            (v9 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

#define STL_PARSER_DROP_ONE_COLUMN____WITH_COLUMN()                                                 \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v6 ) );                               \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableDropColumn( param,                                  \
                                                            a1,                                     \
                                                            v3,                                     \
                                                            a4,                                     \
                                                            QLP_DROP_COLUMN_ACTION_DROP_COLUMN,     \
                                                            sList,                                  \
                                                            (v7 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

#define STL_PARSER_DROP_ONE_COLUMN____WITHOUT_COLUMN()                                              \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v5 ) );                               \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableDropColumn( param,                                  \
                                                            a1,                                     \
                                                            v3,                                     \
                                                            a4,                                     \
                                                            QLP_DROP_COLUMN_ACTION_DROP_COLUMN,     \
                                                            sList,                                  \
                                                            (v6 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

#define STL_PARSER_DROP_MULTI_COLUMN____WITH_COLUMN()                                               \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableDropColumn( param,                                  \
                                                            a1,                                     \
                                                            v3,                                     \
                                                            a4,                                     \
                                                            QLP_DROP_COLUMN_ACTION_DROP_COLUMN,     \
                                                            v7,                                     \
                                                            (v9 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

#define STL_PARSER_DROP_MULTI_COLUMN____WITHOUT_COLUMN()                                            \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableDropColumn( param,                                  \
                                                            a1,                                     \
                                                            v3,                                     \
                                                            a4,                                     \
                                                            QLP_DROP_COLUMN_ACTION_DROP_COLUMN,     \
                                                            v6,                                     \
                                                            (v8 == NULL) ? STL_FALSE : STL_TRUE ) ); \
    }

#define STL_PARSER_DROP_UNUSED_COLUMNS____()                                                        \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableDropColumn( param,                                  \
                                                            a1,                                     \
                                                            v3,                                     \
                                                            a4,                                     \
                                                            QLP_DROP_COLUMN_ACTION_DROP_UNUSED_COLUMNS, \
                                                            NULL,                                   \
                                                            STL_FALSE ) );                          \
    }

/**
 * alter_table_alter_column_definition
 */

#define STL_PARSER_ALTER_COLUMN_SET_DEFAULT_CLAUSE____WITH_COLUMN()                         \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                         \
                                                             a1,                            \
                                                             v3,                            \
                                                             v6,                            \
                                                             a7,                            \
                                                             QLP_ALTER_COLUMN_SET_DEFAULT,  \
                                                             NULL,                          \
                                                             NULL,                          \
                                                             v8 ) );                        \
    }

#define STL_PARSER_ALTER_COLUMN_SET_DEFAULT_CLAUSE____WITHOUT_COLUMN()                      \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                         \
                                                             a1,                            \
                                                             v3,                            \
                                                             v5,                            \
                                                             a6,                            \
                                                             QLP_ALTER_COLUMN_SET_DEFAULT,  \
                                                             NULL,                          \
                                                             NULL,                          \
                                                             v7 ) );                        \
    }

#define STL_PARSER_ALTER_COLUMN_DROP_DEFAULT_CLAUSE____WITH_COLUMN()                        \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                         \
                                                             a1,                            \
                                                             v3,                            \
                                                             v6,                            \
                                                             a7,                            \
                                                             QLP_ALTER_COLUMN_DROP_DEFAULT, \
                                                             NULL,                          \
                                                             NULL,                          \
                                                             NULL ) );                      \
    }

#define STL_PARSER_ALTER_COLUMN_DROP_DEFAULT_CLAUSE____WITHOUT_COLUMN()  \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                         \
                                                             a1,                            \
                                                             v3,                            \
                                                             v5,                            \
                                                             a6,                            \
                                                             QLP_ALTER_COLUMN_DROP_DEFAULT, \
                                                             NULL,                          \
                                                             NULL,                          \
                                                             NULL ) );                      \
    }

#define STL_PARSER_ALTER_COLUMN_SET_NOT_NULL_CLAUSE____WITH_COLUMN()                        \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                         \
                                                             a1,                            \
                                                             v3,                            \
                                                             v6,                            \
                                                             a7,                            \
                                                             QLP_ALTER_COLUMN_SET_NOT_NULL, \
                                                             NULL,                          \
                                                             NULL,                          \
                                                             NULL ) );                      \
    }

#define STL_PARSER_ALTER_COLUMN_SET_NOT_NULL_CLAUSE____WITHOUT_COLUMN()                     \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                         \
                                                             a1,                            \
                                                             v3,                            \
                                                             v5,                            \
                                                             a6,                            \
                                                             QLP_ALTER_COLUMN_SET_NOT_NULL, \
                                                             NULL,                          \
                                                             NULL,                          \
                                                             NULL ) );                      \
    }

#define STL_PARSER_ALTER_COLUMN_SET_CONSTRAINT_NOT_NULL_CLAUSE____WITH_COLUMN()             \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                         \
                                                             a1,                            \
                                                             v3,                            \
                                                             v6,                            \
                                                             a7,                            \
                                                             QLP_ALTER_COLUMN_SET_NOT_NULL, \
                                                             v8,                            \
                                                             NULL,                          \
                                                             NULL ) );                      \
    }

#define STL_PARSER_ALTER_COLUMN_SET_CONSTRAINT_NOT_NULL_CLAUSE____WITHOUT_COLUMN()          \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                         \
                                                             a1,                            \
                                                             v3,                            \
                                                             v5,                            \
                                                             a6,                            \
                                                             QLP_ALTER_COLUMN_SET_NOT_NULL, \
                                                             v7,                            \
                                                             NULL,                          \
                                                             NULL ) );                      \
    }

#define STL_PARSER_ALTER_COLUMN_SET_NOT_NULL_CLAUSE____WITH_COLUMN_WITH_CONST_ATTR()        \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                         \
                                                             a1,                            \
                                                             v3,                            \
                                                             v6,                            \
                                                             a7,                            \
                                                             QLP_ALTER_COLUMN_SET_NOT_NULL, \
                                                             NULL,                          \
                                                             v10,                           \
                                                             NULL ) );                      \
    }

#define STL_PARSER_ALTER_COLUMN_SET_NOT_NULL_CLAUSE____WITHOUT_COLUMN_WITH_CONST_ATTR()     \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                         \
                                                             a1,                            \
                                                             v3,                            \
                                                             v5,                            \
                                                             a6,                            \
                                                             QLP_ALTER_COLUMN_SET_NOT_NULL, \
                                                             NULL,                          \
                                                             v9,                            \
                                                             NULL ) );                      \
    }

#define STL_PARSER_ALTER_COLUMN_SET_CONSTRAINT_NOT_NULL_CLAUSE____WITH_COLUMN_WITH_CONST_ATTR() \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                             \
                                                             a1,                                \
                                                             v3,                                \
                                                             v6,                                \
                                                             a7,                                \
                                                             QLP_ALTER_COLUMN_SET_NOT_NULL,     \
                                                             v8,                                \
                                                             v11,                               \
                                                             NULL ) );                          \
    }

#define STL_PARSER_ALTER_COLUMN_SET_CONSTRAINT_NOT_NULL_CLAUSE____WITHOUT_COLUMN_WITH_CONST_ATTR()  \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                                 \
                                                             a1,                                    \
                                                             v3,                                    \
                                                             v5,                                    \
                                                             a6,                                    \
                                                             QLP_ALTER_COLUMN_SET_NOT_NULL,         \
                                                             v7,                                    \
                                                             v10,                                   \
                                                             NULL ) );                              \
    }

#define STL_PARSER_ALTER_COLUMN_DROP_NOT_NULL_CLAUSE____WITH_COLUMN()                           \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                             \
                                                             a1,                                \
                                                             v3,                                \
                                                             v6,                                \
                                                             a7,                                \
                                                             QLP_ALTER_COLUMN_DROP_NOT_NULL,    \
                                                             NULL,                              \
                                                             NULL,                              \
                                                             NULL ) );                          \
    }

#define STL_PARSER_ALTER_COLUMN_DROP_NOT_NULL_CLAUSE____WITHOUT_COLUMN()                        \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                             \
                                                             a1,                                \
                                                             v3,                                \
                                                             v5,                                \
                                                             a6,                                \
                                                             QLP_ALTER_COLUMN_DROP_NOT_NULL,    \
                                                             NULL,                              \
                                                             NULL,                              \
                                                             NULL ) );                          \
    }

#define STL_PARSER_ALTER_COLUMN_ALTER_DATA_TYPE_CLAUSE____WITH_COLUMN()                         \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                             \
                                                             a1,                                \
                                                             v3,                                \
                                                             v6,                                \
                                                             a7,                                \
                                                             QLP_ALTER_COLUMN_SET_DATA_TYPE,    \
                                                             NULL,                              \
                                                             NULL,                              \
                                                             v10 ) );                           \
    }

#define STL_PARSER_ALTER_COLUMN_ALTER_DATA_TYPE_CLAUSE____WITHOUT_COLUMN()                      \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                             \
                                                             a1,                                \
                                                             v3,                                \
                                                             v5,                                \
                                                             a6,                                \
                                                             QLP_ALTER_COLUMN_SET_DATA_TYPE,    \
                                                             NULL,                              \
                                                             NULL,                              \
                                                             v9 ) );                            \
    }

#define STL_PARSER_ALTER_COLUMN_ALTER_IDENTITY_SPEC____WITH_COLUMN()                                \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                                 \
                                                             a1,                                    \
                                                             v3,                                    \
                                                             v6,                                    \
                                                             a7,                                    \
                                                             QLP_ALTER_COLUMN_ALTER_IDENTITY_SPEC,  \
                                                             NULL,                                  \
                                                             NULL,                                  \
                                                             v7 ) );                                \
    }

#define STL_PARSER_ALTER_COLUMN_ALTER_IDENTITY_SPEC____WITHOUT_COLUMN()                             \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                                 \
                                                             a1,                                    \
                                                             v3,                                    \
                                                             v5,                                    \
                                                             a6,                                    \
                                                             QLP_ALTER_COLUMN_ALTER_IDENTITY_SPEC,  \
                                                             NULL,                                  \
                                                             NULL,                                  \
                                                             v6 ) );                                \
    }

#define STL_PARSER_ALTER_COLUMN_DROP_IDENTITY____WITH_COLUMN()                                  \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                             \
                                                             a1,                                \
                                                             v3,                                \
                                                             v6,                                \
                                                             a7,                                \
                                                             QLP_ALTER_COLUMN_DROP_IDENTITY,    \
                                                             NULL,                              \
                                                             NULL,                              \
                                                             NULL ) );                          \
    }

#define STL_PARSER_ALTER_COLUMN_DROP_IDENTITY____WITHOUT_COLUMN()                               \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAlterColumn( param,                             \
                                                             a1,                                \
                                                             v3,                                \
                                                             v5,                                \
                                                             a6,                                \
                                                             QLP_ALTER_COLUMN_DROP_IDENTITY,    \
                                                             NULL,                              \
                                                             NULL,                              \
                                                             NULL ) );                          \
    }

/*
 * alter_identity_column_specification:
 */

#define STL_PARSER_ALTER_IDENTITY____GENERATION_ALWAYS()                        \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeAlterIdentitySpec( param,                 \
                                                         QLP_IDENTITY_ALWAYS,   \
                                                         NULL ) );              \
    }

#define STL_PARSER_ALTER_IDENTITY____GENERATION_DEFAULT()                           \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterIdentitySpec( param,                     \
                                                         QLP_IDENTITY_BY_DEFAULT,   \
                                                         NULL ) );                  \
    }

#define STL_PARSER_ALTER_IDENTITY____GENERATION_ALWAYS_AND_OPTIONS()                                \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterIdentitySpec( param,                                     \
                                                         QLP_IDENTITY_ALWAYS,                       \
                                                         qlpMakeSequenceOption( param, v4 ) ) );    \
    }

#define STL_PARSER_ALTER_IDENTITY____GENERATION_DEFAULT_AND_OPTIONS()                               \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterIdentitySpec( param,                                     \
                                                         QLP_IDENTITY_BY_DEFAULT,                   \
                                                         qlpMakeSequenceOption( param, v5 ) ) );    \
    }

#define STL_PARSER_ALTER_IDENTITY____OPTIONS()                                                      \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeAlterIdentitySpec( param,                                     \
                                                         QLP_IDENTITY_INVALID,                      \
                                                         qlpMakeSequenceOption( param, v1 ) ) );    \
    }

/*
 * alter_identity_column_options
 */

#define STL_PARSER_ALTER_IDENTITY_COLUMN_OPTIONS____OPTION()            \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
                                                                        \
    }

#define STL_PARSER_ALTER_IDENTITY_COLUMN_OPTIONS____OPTION_LIST() \
    {                                                                   \
        STL_PARSER_CHECK( qlpCheckDuplicateOptions( param, v1, v2 ) );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v2 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

/*
 * alter_identity_column_option
 */

#define STL_PARSER_ALTER_IDENTITY_COLUMN_OPTION____RESTART()    \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_ALTER_IDENTITY_COLUMN_OPTION____BASIC_OPTION_WITH_SET()  \
    {                                                                       \
        STL_PARSER_CHECK( v0 = v2 );                                        \
    }

#define STL_PARSER_ALTER_IDENTITY_COLUMN_OPTION____BASIC_OPTION()   \
    {                                                               \
        STL_PARSER_CHECK( v0 = v1 );                                \
    }

/*
 * alter_table_physical_attribute_clause
 */

#define STL_PARSER_ALTER_TABLE_PHYSICAL_ATTR_CLAUSE()                   \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTablePhysicalAttr( param,    \
                                                              a1,       \
                                                              v3,       \
                                                              v4 ) );   \
    }

/**
 * alter_table_rename_column_clause
 */

#define STL_PARSER_RENAME_COLUMN_CLAUSE()                               \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableRenameColumn( param,    \
                                                              a1,       \
                                                              v3,       \
                                                              v6,       \
                                                              v8 ) );   \
    }

/**
 * alter_table_add_constraint_definition
 */

#define STL_PARSER_ADD_CONSTRAINT_CLAUSE()                              \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableAddConstraint( param,   \
                                                               a1,      \
                                                               v3,      \
                                                               v5 ) );  \
    }

/**
 * alter_table_drop_constraint_definition
 */

#define STL_PARSER_DROP_CONSTRAINT_CLAUSE()                                             \
    {                                                                                   \
        STL_PARSER_CHECK(                                                               \
            v0 =  qlpMakeAlterTableDropConstraint( param,                               \
                                                   a1,                                  \
                                                   v3,                                  \
                                                   v5,                                  \
                                                   (v6 == NULL) ? STL_FALSE:STL_TRUE,   \
                                                   (v7 == NULL) ? STL_FALSE:STL_TRUE )  \
            );                                                                          \
    }

/**
 * alter_table_alter_constraint_definition
 */

#define STL_PARSER_ALTER_CONSTRAINT_CLAUSE()                \
    {                                                       \
        STL_PARSER_CHECK(                                   \
            v0 = qlpMakeAlterTableAlterConstraint( param,   \
                                                   a1,      \
                                                   v3,      \
                                                   v5,      \
                                                   v6 )     \
            );                                              \
    }


/**
 * drop_constraint_object
 */

#define STL_PARSER_DROP_CONSTRAINT_OBJECT____CONSTRAINT_NAME()              \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeDropConstObject( param,               \
                                                       a1,                  \
                                                       QLP_DROP_CONST_NAME, \
                                                       v2,                  \
                                                       NULL ) );            \
    }

#define STL_PARSER_DROP_CONSTRAINT_OBJECT____PRIMARY_KEY()                          \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeDropConstObject( param,                       \
                                                       a1,                          \
                                                       QLP_DROP_CONST_PRIMARY_KEY,  \
                                                       NULL,                        \
                                                       NULL ) );                    \
    }

#define STL_PARSER_DROP_CONSTRAINT_OBJECT____UNIQUE()                           \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDropConstObject( param,                   \
                                                       a1,                      \
                                                       QLP_DROP_CONST_UNIQUE,   \
                                                       NULL,                    \
                                                       v3 ) );                  \
    }


/**
 * drop_key_index_option
 */

#define STL_PARSER_DROP_KEY_INDEX_OPTION____EMPTY() \
    {                                               \
        STL_PARSER_CHECK( v0 = NULL );              \
    }

#define STL_PARSER_DROP_KEY_INDEX_OPTION____KEEP_INDEX() \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

#define STL_PARSER_DROP_KEY_INDEX_OPTION____DROP_INDEX() \
    {                                               \
        STL_PARSER_CHECK( v0 = NULL );              \
    }

/*
 * alter_table_add_supplemental_log_clause
 */

#define STL_PARSER_ADD_TABLE_SUPPLEMENTAL_LOG_CLAUSE()                  \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableSuppLog( param,         \
                                                         a1,            \
                                                         v3,            \
                                                         STL_TRUE ) );  \
    }

/*
 * alter_table_drop_supplemental_log_clause
 */

#define STL_PARSER_DROP_TABLE_SUPPLEMENTAL_LOG_CLAUSE()                 \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableSuppLog( param,         \
                                                         a1,            \
                                                         v3,            \
                                                         STL_FALSE ) ); \
    }

/**
 * alter_table_rename_table_clause
 */

#define STL_PARSER_RENAME_TABLE_CLAUSE()                                \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterTableRenameTable( param,     \
                                                             a1,        \
                                                             v3,        \
                                                             v6 ) );    \
    }



/**
 * alter_index_statement
 */

#define STL_PARSER_ALTER_INDEX____ALTER_PHYSICAL()  \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

/**
 * alter_index_physical_attribute_clause
 */

#define STL_PARSER_INDEX_PHYSICAL_ATTR_CLAUSE()                         \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlterIndexPhysicalAttr( param,    \
                                                              a1,       \
                                                              v3,       \
                                                              v4 ) );   \
    }



/**
 * insert_statement : qlpInsert
 */

#define STL_PARSER_INSERT_STATEMENT____( )              \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeInsert( param,    \
                                              a1,       \
                                              v3,       \
                                              v4,       \
                                              v5 ) );   \
    }


/**
 * insert_columns_and_source : qlpInsertSource
 */

#define STL_PARSER_INSERT_COLUMNS_AND_SOURCE____CONSTRUCTOR( )  \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_INSERT_COLUMNS_AND_SOURCE____DEFAULT( )      \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_INSERT_COLUMNS_AND_SOURCE____SUBQUERY( )     \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }


/**
 * from_constructor : qlpInsertSource
 */

#define STL_PARSER_FROM_CONSTRUCTOR____ALL_COLUMNS( )           \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeInsertSource( param,      \
                                                    a1,         \
                                                    STL_TRUE,   \
                                                    STL_FALSE,  \
                                                    NULL,       \
                                                    v1,         \
                                                    NULL) );    \
    }

#define STL_PARSER_FROM_CONSTRUCTOR____COLUMNS( )               \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeInsertSource( param,      \
                                                    a1,         \
                                                    STL_FALSE,  \
                                                    STL_FALSE,  \
                                                    v2,         \
                                                    v4,         \
                                                    NULL ) );   \
    }


/**
 * column_name_list : qlpList
 */

#define STL_PARSER_COLUMN_NAME_LIST____COLUMN( )                        \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );  \
    }

#define STL_PARSER_COLUMN_NAME_LIST____COLUMN_LIST( )                   \
    {                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * contextually_typed_table_value_constructor : qlpList
 */

#define STL_PARSER_CONTEXTUALLY_TYPED_TABLE_VALUE_CONSTRUCTOR____( )    \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v2 );                                    \
    }


/**
 * contextually_typed_row_value_expression_list : qlpList
 */
#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_EXPRESSION_LIST____EXPR( )      \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( v0 = sList );                                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );           \
    }

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_EXPRESSION_LIST____EXPR_LIST( ) \
    {                                                                           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );              \
        STL_PARSER_CHECK( v0 = v1 );                                            \
    }

/**
 * contextually_typed_row_value_expression : qllNode
 */

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_EXPRESSION____ONE_ROW_EXPR( )   \
    {                                                                           \
        STL_PARSER_CHECK( v0 = v2 );                                            \
    }


/*
#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_EXPRESSION____VALUE( )          \
    {                                                                           \
        STL_PARSER_CHECK( v0 = v1 );                                            \
    }
#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_EXPRESSION____VALUE_LIST( )     \
    {                                                                           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v2, v4 ) );              \
        STL_PARSER_CHECK( v0 = v2 );                                            \
    }

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_EXPRESSION____ROW_TYPED_VALUE( )        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }
*/

/**
 * contextually_typed_row_value_constructor_element_2 : qlpList (not support)
 */

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_CONSTRUCTOR_ELEMENT_2____ROW_VALUE( )   \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
                                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );                   \
    }

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_CONSTRUCTOR_ELEMENT_2____ROW_VALUE_LIST( )      \
    {                                                                                           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );                              \
        STL_PARSER_CHECK( v0 = v1 );                                                            \
    }


/**
 * contextually_typed_row_value_constructor_element : qllNode
 */

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_CONSTRUCTOR_ELEMENT____VALUE_EXPR( )    \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeValueExpr( param,                                 \
                                                 a1,                                    \
                                                 QLP_GET_LAST_POS( a1 ) - a1,           \
                                                 (qllNode*) v1 ) );                     \
    }

#define STL_PARSER_CONTEXTUALLY_TYPED_ROW_VALUE_CONSTRUCTOR_ELEMENT____TYPED_VALUE( )   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeValueExpr( param,                                 \
                                                 a1,                                    \
                                                 QLP_GET_LAST_POS( a1 ) - a1,           \
                                                 (qllNode*) v1 ) );                     \
    }


/**
 * from_default : qlpInsertSource
 */

#define STL_PARSER_FROM_DEFAULT____( )                          \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeInsertSource( param,      \
                                                    a1,         \
                                                    STL_TRUE,   \
                                                    STL_TRUE,   \
                                                    NULL,       \
                                                    NULL,       \
                                                    NULL ) );   \
    }

/**
 * from_subquery : qlpInsertSource
 */

#define STL_PARSER_FROM_SUBQUERY____QUERY_EXPR_ALL_COLUMNS()    \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeInsertSource( param,      \
                                                    a1,         \
                                                    STL_TRUE,   \
                                                    STL_FALSE,  \
                                                    NULL,       \
                                                    NULL,       \
                                                    v1) );      \
    }

#define STL_PARSER_FROM_SUBQUERY____QUERY_EXPR_COLUMNS()        \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeInsertSource( param,      \
                                                    a1,         \
                                                    STL_FALSE,  \
                                                    STL_FALSE,  \
                                                    v2,         \
                                                    NULL,       \
                                                    v4 ) );     \
    }


/**
 * update_statement : qlpUpdate
 */

#define STL_PARSER_UPDATE_STATEMENT____NO_WHERE( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeUpdate( param,    \
                                              a1,       \
                                              v3,       \
                                              v2,       \
                                              v5,       \
                                              NULL,     \
                                              v6,       \
                                              v7,       \
                                              v8 ) );   \
    }

#define STL_PARSER_UPDATE_STATEMENT____WHERE( )         \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeUpdate( param,    \
                                              a1,       \
                                              v3,       \
                                              v2,       \
                                              v5,       \
                                              v7,       \
                                              v8,       \
                                              v9,       \
                                              v10 ) );  \
    }

/**
 * update_statement : positioned
 */

#define STL_PARSER_UPDATE_STATEMENT_POSITIONED____CURSOR( )             \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeUpdatePositionedCursor( param,    \
                                                              a1,       \
                                                              v2,       \
                                                              v3,       \
                                                              v5,       \
                                                              v9 ) );   \
    }

/**
 * set_clause_list : qlpList : qlpTarget
 */

#define STL_PARSER_SET_CLAUSE_LIST____SET_CLAUSE( ) \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_SET_CLAUSE_LIST____SET_CLAUSE_LIST( )                        \
    {                                                                           \
        STL_PARSER_CHECK( qlpAddListToList( (qlpList *)v1, (qlpList *)v3 ) );   \
        STL_PARSER_CHECK( v0 = v1 );                                            \
    }


/**
 * set_clause : qlpTarget
 */

#define STL_PARSER_SET_CLAUSE____MULTIPLE_COLUMN( )     \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_SET_CLAUSE____ASSIGN( )                                          \
    {                                                                               \
        qlpValueExpr  * sExpr = NULL;                                               \
        qlpTarget     * sTarget = NULL;                                             \
        qlpList       * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );        \
                                                                                    \
        STL_PARSER_CHECK( sExpr = qlpMakeValueExpr( param,                          \
                                                    a3,                             \
                                                    QLP_GET_LAST_POS( a3 ) - a3,    \
                                                    (qllNode*) v3 ) );              \
                                                                                    \
        STL_PARSER_CHECK( sTarget = qlpMakeTarget( param,                           \
                                              a1,                                   \
                                              NULL,                                 \
                                              a1,                                   \
                                              NULL,                                 \
                                              (qllNode*) sExpr,                     \
                                              v1 ) );                               \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, sTarget ) );          \
        STL_PARSER_CHECK( v0 = sList );                                             \
    }


/**
 * set_target : qlpValue : string
 */

#define STL_PARSER_SET_TARGET____UPDATE_TARGET( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_SET_TARGET____MUTATED_SET( ) \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a1 );           \
    }


/**
 * multiple_column_assignment : qlpMultipleSetColumn
 */

#define STL_PARSER_MULTIPLE_COLUMN_ASSIGNMENT____( )                \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeMultipleSetColumn( param,     \
                                                         a1,        \
                                                         v1,        \
                                                         v3 ) );    \
    }


/**
 * set_target_list : qlpList
 */

#define STL_PARSER_SET_TARGET_LIST____( )                               \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v2 );                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v2, v4 ) );      \
    }


/**
 * set_target_list_2 : qlpList
 */

#define STL_PARSER_SET_TARGET_LIST_2____TARGET( )                       \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_SET_TARGET_LIST_2____TARGET_LIST( )                  \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );      \
    }


/**
 * assigned_row : qllNode
 */

#define STL_PARSER_ASSIGNED_ROW____( )          \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_ASSIGNED_ROW____ONE_SUBQUERY( )                              \
    {                                                                           \
        qlpList         * sList         = NULL;                                 \
        qlpValueExpr    * sValueExpr    = NULL;                                 \
                                                                                \
        STL_PARSER_CHECK( sValueExpr = qlpMakeValueExpr( param,                 \
                                                         a1,                    \
                                                         QLP_GET_LAST_POS( a1 ) - a1,   \
                                                         (qllNode*) v1 ) );     \
                                                                                \
        sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                    \
        STL_PARSER_CHECK( v0 = sList );                                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, sValueExpr ) );   \
    }


/**
 * update_target : qlpValue : string
 */

#define STL_PARSER_UPDATE_TARGET____NAME( )                             \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeColumnRef( param,                 \
                                                 a1,                    \
                                                 v1,                    \
                                                 STL_FALSE ) );         \
    }

#define STL_PARSER_UPDATE_TARGET____PAREN_NAME( )                       \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeColumnRef( param,                 \
                                                 a2,                    \
                                                 v2,                    \
                                                 STL_FALSE ) );         \
    }


/**
 * mutated_set_clause
 */

#define STL_PARSER_MUTATED_SET_CLAUSE____( )    \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a2 );           \
    }


/**
 * mutated_target
 */

#define STL_PARSER_MUTATED_TARGET____COLUMN( )  \
    {                                           \
        STL_PARSER_NOT_SUPPORT( a1 );           \
    }

#define STL_PARSER_MUTATED_TARGET____MUTATED_SET( ) \
    {                                               \
        STL_PARSER_NOT_SUPPORT( a1 );               \
    }


/**
 * update_source : qllNode
 */

#define STL_PARSER_UPDATE_SOURCE____VALUE_EXPR( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_UPDATE_SOURCE____TYPED_VALUE( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }


/**
 * contextually_typed_value_specification : qllNode
 */

#define STL_PARSER_CONTEXTUALLY_TYPED_VALUE_SPECIFICATION____IMPLICITLY_TYPED_VALUE( )  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeNullConstant( param, a1 ) );                      \
    }

#define STL_PARSER_CONTEXTUALLY_TYPED_VALUE_SPECIFICATION____DEFAULT( ) \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDefault( param,                   \
                                               a1,                      \
                                               STL_FALSE,               \
                                               a1 ) );                  \
    }


/**
 * simple_value_specification
 */

#define STL_PARSER_SIMPLE_VALUE_SPECIFICATION____LITERAL( ) \
    {                                                       \
        v0 = v1;                                            \
    }

#define STL_PARSER_SIMPLE_VALUE_SPECIFICATION____HOST_PARAMETER( )                  \
    {                                                                               \
        qlpList  * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );             \
        STL_PARSER_CHECK( qlpAddPtrValueToList(                                     \
                              param,                                                \
                              sList,                                                \
                              qlpMakeStrConstant( param, a1, v1 ) ) );              \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v2 ) );               \
                                                                                    \
        STL_PARSER_CHECK( v0 = (qllNode *) qlpMakeParameter( param,                 \
                                                             a1,                    \
                                                             QLP_PARAM_TYPE_HOST,   \
                                                             KNL_BIND_TYPE_IN,      \
                                                             a1,                    \
                                                             sList ) );             \
    }

#define STL_PARSER_SIMPLE_VALUE_SPECIFICATION____DYNAMIC_PARAMETER( )                   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = (qllNode *) qlpMakeParameter( param,                     \
                                                             a1,                        \
                                                             QLP_PARAM_TYPE_DYNAMIC,    \
                                                             KNL_BIND_TYPE_IN,          \
                                                             a1,                        \
                                                             NULL ) );                  \
    }

#define STL_PARSER_SIMPLE_VALUE_SPECIFICATION____SQL_PARAMETER( )   \
    {                                                               \
        STL_PARSER_NOT_SUPPORT( a1 );                               \
    }


/**
 * literal
 */

#define STL_PARSER_LITERAL____SIGNED_NUMERIC( ) \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_LITERAL____GENERAL( )        \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * signed_numeric_literal
 */

#define STL_PARSER_SIGNED_NUMERIC_LITERAL____UNSIGNED_NUMERIC( ) \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_SIGNED_NUMERIC_LITERAL____SIGNED_NUMERIC( )                  \
    {                                                                           \
        qlpValue  * sValue = ( (qlpConstantValue*) v2 )->mValue;                \
        stlInt32    sNumLen = stlStrlen( QLP_GET_PTR_VALUE( sValue ) ) + 2;     \
        stlChar   * sStr    = qlpMakeBuffer( param,                             \
                                             sNumLen );                         \
        qlpValue  * sValueNode = NULL;                                          \
                                                                                \
        sStr[ 0 ] = ((stlChar*)v1)[ 0 ];                                        \
        stlStrcpy( & sStr[ 1 ], QLP_GET_PTR_VALUE( sValue ) );                  \
        sStr[ sNumLen - 1 ] = '\0';                                             \
                                                                                \
        STL_PARSER_CHECK( sValueNode = qlpMakeNumberConstant( param, a1, sStr ) );  \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param, a1, sValueNode ) );      \
    }

/**
 * host_variable
 */

#define STL_PARSER_HOST_VARIABLE____( )                         \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeHostVariable( param ) );  \
    }


/**
 * delete_statement
 */

#define STL_PARSER_DELETE_STATEMENT____NO_WHERE( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDelete( param,    \
                                              a1,       \
                                              v4,       \
                                              v2,       \
                                              NULL,     \
                                              v5,       \
                                              v6,       \
                                              v7 ) );   \
    }

#define STL_PARSER_DELETE_STATEMENT____WHERE( )         \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDelete( param,    \
                                              a1,       \
                                              v4,       \
                                              v2,       \
                                              v6,       \
                                              v7,       \
                                              v8,       \
                                              v9 ) );   \
    }

#define STL_PARSER_DELETE_STATEMENT____NO_FROM_NO_WHERE( )      \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDelete( param,            \
                                              a1,               \
                                              v3,               \
                                              v2,               \
                                              NULL,             \
                                              v4,               \
                                              v5,               \
                                              v6 ) );           \
    }

#define STL_PARSER_DELETE_STATEMENT____NO_FROM_WHERE( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDelete( param,    \
                                              a1,       \
                                              v3,       \
                                              v2,       \
                                              v5,       \
                                              v6,       \
                                              v7,       \
                                              v8 ) );   \
    }

/**
 * delete_statement : positioned
 */

#define STL_PARSER_DELETE_STATEMENT_POSITIONED____CURSOR( )             \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDeletePositionedCursor( param,    \
                                                              a1,       \
                                                              v2,       \
                                                              v4,       \
                                                              v8 ) );   \
    }

#define STL_PARSER_DELETE_STATEMENT_POSITIONED____NO_FROM_CURSOR( )     \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDeletePositionedCursor( param,    \
                                                              a1,       \
                                                              v2,       \
                                                              v3,       \
                                                              v7 ) );   \
    }

/**
 * insert or delete returning
 */


#define STL_PARSER_INSERT_OR_DELETE_RETURNING_CLAUSE____EMPTY( )    \
    {                                                               \
        STL_PARSER_CHECK( v0 = NULL );                              \
    }

#define STL_PARSER_INSERT_OR_DELETE_RETURNING_CLAUSE____RETURNING( )    \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeInsDelReturn( param,              \
                                                    a1,                 \
                                                    v2,                 \
                                                    v3 ) );             \
    }

/**
 * update_returning_clause
 */

#define STL_PARSER_UPDATE_RETURNING_CLAUSE____EMPTY( )  \
    {                                                   \
        STL_PARSER_CHECK( v0 = NULL );                  \
    }

#define STL_PARSER_UPDATE_RETURNING_CLAUSE____RETURNING( )      \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeUpdateReturn( param,      \
                                                    a1,         \
                                                    STL_TRUE,   \
                                                    v2,         \
                                                    v3 ) );     \
    }

#define STL_PARSER_UPDATE_RETURNING_CLAUSE____RETURNING_NEW( )  \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeUpdateReturn( param,      \
                                                    a1,         \
                                                    STL_TRUE,   \
                                                    v3,         \
                                                    v4 ) );     \
    }

#define STL_PARSER_UPDATE_RETURNING_CLAUSE____RETURNING_OLD( )  \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeUpdateReturn( param,      \
                                                    a1,         \
                                                    STL_FALSE,  \
                                                    v3,         \
                                                    v4 ) );     \
    }

/**
 * select_statement : qlpSelect
 */

#define STL_PARSER_SELECT_STATEMENT____QUERY_EXPRESSION()   \
    {                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeSelect( param,        \
                                              a1,           \
                                              v1,           \
                                              v2 ) );       \
    }

/**
 * updatabilty_clause:
 */

#define STL_PARSER_UPDATABILITY____EMPTY()      \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_UPDATABILITY____FOR_READ_ONLY()                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeUpdatableClause( param,                           \
                                                       a1,                              \
                                                       QLP_UPDATABILITY_FOR_READ_ONLY,  \
                                                       NULL,                            \
                                                       NULL ) );                        \
    }

#define STL_PARSER_UPDATABILITY____FOR_UPDATE()                                     \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeUpdatableClause( param,                       \
                                                       a1,                          \
                                                       QLP_UPDATABILITY_FOR_UPDATE, \
                                                       NULL,                        \
                                                       v3 ) );                      \
    }

#define STL_PARSER_UPDATABILITY____FOR_UPDATE_OF()                                  \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeUpdatableClause( param,                       \
                                                       a1,                          \
                                                       QLP_UPDATABILITY_FOR_UPDATE, \
                                                       v4,                          \
                                                       v5 ) );                      \
    }

/**
 * for_update_lock_wait_mode
 */

#define STL_PARSER_FOR_UPDATE_LOCK_WAIT_MODE____EMPTY() \
    {                                                   \
        STL_PARSER_CHECK( v0 = NULL );                  \
    }

#define STL_PARSER_FOR_UPDATE_LOCK_WAIT_MODE____WAIT()                              \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeForUpdateLockWait( param,                     \
                                                         a1,                        \
                                                         QLP_FOR_UPDATE_LOCK_WAIT,  \
                                                         NULL ) );                  \
    }

#define STL_PARSER_FOR_UPDATE_LOCK_WAIT_MODE____WAIT_SECOND()                       \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeForUpdateLockWait( param,                     \
                                                         a1,                        \
                                                         QLP_FOR_UPDATE_LOCK_WAIT,  \
                                                         v2 ) );                    \
    }

#define STL_PARSER_FOR_UPDATE_LOCK_WAIT_MODE____NOWAIT()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeForUpdateLockWait( param,                         \
                                                         a1,                            \
                                                         QLP_FOR_UPDATE_LOCK_NOWAIT,    \
                                                         NULL ) );                      \
    }

#define STL_PARSER_FOR_UPDATE_LOCK_WAIT_MODE____SKIP_LOCKED()               \
    {                                                                       \
        STL_PARSER_CHECK(                                                   \
            v0 = qlpMakeForUpdateLockWait( param,                           \
                                           a1,                              \
                                           QLP_FOR_UPDATE_LOCK_SKIP_LOCKED, \
                                           NULL ) );                        \
    }

/**
 * for_update_column_list
 */

#define STL_PARSER_FOR_UPDATE_COLUMN_LIST____COLUMN()                       \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        qllNode * sColumn = (qllNode *) qlpMakeColumnRef( param,            \
                                                          a1,               \
                                                          v1,               \
                                                          STL_FALSE );      \
                                                                            \
        STL_PARSER_CHECK( v0 = sList );                                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, sColumn ) );  \
    }

#define STL_PARSER_FOR_UPDATE_COLUMN_LIST____COLUMN_LIST()              \
    {                                                                   \
        qllNode * sColumn = (qllNode *) qlpMakeColumnRef( param,        \
                                                          a3,           \
                                                          v3,           \
                                                          STL_FALSE );  \
        STL_PARSER_CHECK( v0 = v1 );                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, sColumn ) ); \
    }

/**
 * select_into_statement : qlpSelectInto
 */

#define STL_PARSER_SELECT_INTO_STATEMENT____()                                          \
    {                                                                                   \
        qllNode         * sQueryNode;                                                   \
        qlpIntoClause   * sInto;                                                        \
                                                                                        \
        STL_PARSER_CHECK( sQueryNode = qlpMakeQuerySpecNode( param,                     \
                                                             a1,                        \
                                                             v2,                        \
                                                             QLP_SET_QUANTIFIER_ALL,    \
                                                             v3,                        \
                                                             v6,                        \
                                                             v7,                        \
                                                             v8,                        \
                                                             v9,                        \
                                                             v10 ) );                   \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpAddQueryNodeOption( param,                            \
                                                      a1,                               \
                                                      sQueryNode,                       \
                                                      NULL,                             \
                                                      v11,                              \
                                                      a11,                              \
                                                      v12,                              \
                                                      v13 ) );                          \
                                                                                        \
        STL_PARSER_CHECK( sInto = qlpMakeIntoClause( param,                             \
                                                     a5,                                \
                                                     v5 ) );                            \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeSelectInto( param,                                \
                                                  a1,                                   \
                                                  sQueryNode,                           \
                                                  sInto,                                \
                                                  v14 ) );                              \
    }

#define STL_PARSER_SELECT_INTO_STATEMENT____ALL()                                       \
    {                                                                                   \
        qllNode         * sQueryNode;                                                   \
        qlpIntoClause   * sInto;                                                        \
                                                                                        \
        STL_PARSER_CHECK( sQueryNode = qlpMakeQuerySpecNode( param,                     \
                                                             a1,                        \
                                                             v2,                        \
                                                             QLP_SET_QUANTIFIER_ALL,    \
                                                             v4,                        \
                                                             v7,                        \
                                                             v8,                        \
                                                             v9,                        \
                                                             v10,                       \
                                                             v11 ) );                   \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpAddQueryNodeOption( param,                            \
                                                      a1,                               \
                                                      sQueryNode,                       \
                                                      NULL,                             \
                                                      v12,                              \
                                                      a12,                              \
                                                      v13,                              \
                                                      v14 ) );                          \
                                                                                        \
        STL_PARSER_CHECK( sInto = qlpMakeIntoClause( param,                             \
                                                     a6,                                \
                                                     v6 ) );                            \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeSelectInto( param,                                \
                                                  a1,                                   \
                                                  sQueryNode,                           \
                                                  sInto,                                \
                                                  v15 ) );                              \
    }

#define STL_PARSER_SELECT_INTO_STATEMENT____DISTINCT()                          \
    {                                                                           \
        qllNode         * sQueryNode;                                           \
        qlpIntoClause   * sInto;                                                \
                                                                                \
        STL_PARSER_CHECK( sQueryNode =                                          \
                          qlpMakeQuerySpecNode( param,                          \
                                                a1,                             \
                                                v2,                             \
                                                QLP_SET_QUANTIFIER_DISTINCT,    \
                                                v4,                             \
                                                v7,                             \
                                                v8,                             \
                                                v9,                             \
                                                v10,                            \
                                                v11 ) );                        \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpAddQueryNodeOption( param,                    \
                                                      a1,                       \
                                                      sQueryNode,               \
                                                      NULL,                     \
                                                      v12,                      \
                                                      a12,                      \
                                                      v13,                      \
                                                      v14 ) );                  \
                                                                                \
        STL_PARSER_CHECK( sInto = qlpMakeIntoClause( param,                     \
                                                     a6,                        \
                                                     v6 ) );                    \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeSelectInto( param,                        \
                                                  a1,                           \
                                                  sQueryNode,                   \
                                                  sInto,                        \
                                                  v15 ) );                      \
    }

/**
 * query_expression : qllNode
 */

#define STL_PARSER_QUERY_EXPRESSION____NO_WITH_CLAUSE( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpAddQueryNodeOption( param,    \
                                                      a1,       \
                                                      v1,       \
                                                      NULL,     \
                                                      v2,       \
                                                      a2,       \
                                                      v3,       \
                                                      v4 ) );   \
    }

#define STL_PARSER_QUERY_EXPRESSION____WITH_CLAUSE( )           \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpAddQueryNodeOption( param,    \
                                                      a1,       \
                                                      v2,       \
                                                      v1,       \
                                                      v3,       \
                                                      a3,       \
                                                      v4,       \
                                                      v5 ) );   \
    }


/**
 * query_expression_body : qllNode
 */

#define STL_PARSER_QUERY_EXPRESSION_BODY____()  \
    {                                           \
        v0 = v1;                                \
    }

#define STL_PARSER_QUERY_EXPRESSION_BODY____UNION_DEFAULT()                         \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySetNode( param,                          \
                                                    a2,                             \
                                                    QLP_SET_TYPE_UNION,             \
                                                    QLP_SET_QUANTIFIER_DISTINCT,    \
                                                    v1,                             \
                                                    v4,                             \
                                                    v3 ) );                         \
    }

#define STL_PARSER_QUERY_EXPRESSION_BODY____UNION_ALL()                             \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySetNode( param,                          \
                                                    a2,                             \
                                                    QLP_SET_TYPE_UNION,             \
                                                    QLP_SET_QUANTIFIER_ALL,         \
                                                    v1,                             \
                                                    v5,                             \
                                                    v4 ) );                         \
    }

#define STL_PARSER_QUERY_EXPRESSION_BODY____UNION_DISTINCT()                        \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySetNode( param,                          \
                                                    a2,                             \
                                                    QLP_SET_TYPE_UNION,             \
                                                    QLP_SET_QUANTIFIER_DISTINCT,    \
                                                    v1,                             \
                                                    v5,                             \
                                                    v4 ) );                         \
    }

#define STL_PARSER_QUERY_EXPRESSION_BODY____EXCEPT_DEFAULT()                        \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySetNode( param,                          \
                                                    a2,                             \
                                                    QLP_SET_TYPE_EXCEPT,            \
                                                    QLP_SET_QUANTIFIER_DISTINCT,    \
                                                    v1,                             \
                                                    v4,                             \
                                                    v3 ) );                         \
    }

#define STL_PARSER_QUERY_EXPRESSION_BODY____EXCEPT_ALL()                            \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySetNode( param,                          \
                                                    a2,                             \
                                                    QLP_SET_TYPE_EXCEPT,            \
                                                    QLP_SET_QUANTIFIER_ALL,         \
                                                    v1,                             \
                                                    v5,                             \
                                                    v4 ) );                         \
    }

#define STL_PARSER_QUERY_EXPRESSION_BODY____EXCEPT_DISTINCT()                       \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySetNode( param,                          \
                                                    a2,                             \
                                                    QLP_SET_TYPE_EXCEPT,            \
                                                    QLP_SET_QUANTIFIER_DISTINCT,    \
                                                    v1,                             \
                                                    v5,                             \
                                                    v4 ) );                         \
    }


/**
 * query_term : qllNode
 */

#define STL_PARSER_QUERY_TERM____() \
    {                               \
        v0 = v1;                    \
    }

#define STL_PARSER_QUERY_TERM____INTERSECT_DEFAULT()                                \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySetNode( param,                          \
                                                    a2,                             \
                                                    QLP_SET_TYPE_INTERSECT,         \
                                                    QLP_SET_QUANTIFIER_DISTINCT,    \
                                                    v1,                             \
                                                    v4,                             \
                                                    v3 ) );                         \
    }

#define STL_PARSER_QUERY_TERM____INTERSECT_ALL()                                    \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySetNode( param,                          \
                                                    a2,                             \
                                                    QLP_SET_TYPE_INTERSECT,         \
                                                    QLP_SET_QUANTIFIER_ALL,         \
                                                    v1,                             \
                                                    v5,                             \
                                                    v4 ) );                         \
    }

#define STL_PARSER_QUERY_TERM____INTERSECT_DISTINCT()                               \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySetNode( param,                          \
                                                    a2,                             \
                                                    QLP_SET_TYPE_INTERSECT,         \
                                                    QLP_SET_QUANTIFIER_DISTINCT,    \
                                                    v1,                             \
                                                    v5,                             \
                                                    v4 ) );                         \
    }


/**
 * query_primary : qllNode
 */

#define STL_PARSER_QUERY_PRIMARY____QUERY_SPEC()                                    \
    {                                                                               \
        v0 = v1;                                                                    \
    }

#define STL_PARSER_QUERY_PRIMARY____PARENTHESIZED_QUERY_EXPRESSION_NO_WITH_CLAUSE() \
    {                                                                               \
        v0 = v1;                                                                    \
    }


/**
 * parenthesized_query_expression_no_with_clause_head : qllNode
 */

#define STL_PARSER_PARENTHESIZED_QUERY_EXPRESSION_NO_WITH_CLAUSE_HEAD____()             \
    {                                                                                   \
        qllNode  * sNode;                                                               \
        STL_PARSER_CHECK( sNode = qlpAddQueryNodeOption( param, a1,                     \
                                                         v2, NULL, v3, a3, v4, v5 ) );  \
        STL_PARSER_CHECK( v0 = qlpMakeSubTableNode( param, a2, sNode, NULL, NULL ) );   \
    }


/**
 * parenthesized_query_expression_no_with_clause : qllNode
 */

#define STL_PARSER_PARENTHESIZED_QUERY_EXPRESSION_NO_WITH_CLAUSE____()  \
    {                                                                   \
        v0 = v1;                                                        \
    }


/**
 * parenthesized_query_expression_with_clause : qllNode
 */

#define STL_PARSER_PARENTHESIZED_QUERY_EXPRESSION_WITH_CLAUSE____()                     \
    {                                                                                   \
        qllNode  * sNode;                                                               \
        STL_PARSER_CHECK( sNode = qlpAddQueryNodeOption( param, a1,                     \
                                                         v3, v2, v4, a4, v5, v6 ) );    \
        STL_PARSER_CHECK( v0 = qlpMakeSubTableNode( param, a2, sNode, NULL, NULL ) );   \
    }


/**
 * corresponding_spec
 */

#define STL_PARSER_CORRESPONDING_SPEC____EMPTY()    \
    {                                               \
        v0 = NULL;                                  \
    }


/**
 * query_specification : qllNode
 */

#define STL_PARSER_QUERY_SPECIFICATION____()                                        \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySpecNode( param,                         \
                                                     a1,                            \
                                                     v2,                            \
                                                     QLP_SET_QUANTIFIER_ALL,        \
                                                     v3,                            \
                                                     v4,                            \
                                                     v5,                            \
                                                     v6,                            \
                                                     v7,                            \
                                                     v8 ) );                        \
    }

#define STL_PARSER_QUERY_SPECIFICATION____ALL()                                     \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySpecNode( param,                         \
                                                     a1,                            \
                                                     v2,                            \
                                                     QLP_SET_QUANTIFIER_ALL,        \
                                                     v4,                            \
                                                     v5,                            \
                                                     v6,                            \
                                                     v7,                            \
                                                     v8,                            \
                                                     v9 ) );                        \
    }

#define STL_PARSER_QUERY_SPECIFICATION____DISTINCT()                                \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeQuerySpecNode( param,                         \
                                                     a1,                            \
                                                     v2,                            \
                                                     QLP_SET_QUANTIFIER_DISTINCT,   \
                                                     v4,                            \
                                                     v5,                            \
                                                     v6,                            \
                                                     v7,                            \
                                                     v8,                            \
                                                     v9 ) );                        \
    }


/**
 * from_clause : qllNode
 */

#define STL_PARSER_FROM_CLAUSE____()    \
    {                                   \
        v0 = v2;                        \
    }


/**
 * where_clause : qlpList
 */

#define STL_PARSER_WHERE_CLAUSE____EMPTY()  \
    {                                       \
        v0 = NULL;                          \
    }

#define STL_PARSER_WHERE_CLAUSE____WHERE()  \
    {                                       \
        v0 = v2;                            \
    }


/**
 * table_reference_list : qllNode
 */

#define STL_PARSER_TABLE_REFERENCE_LIST____SINGLE( )    \
    {                                                   \
        v0 = v1;                                        \
    }

#define STL_PARSER_TABLE_REFERENCE_LIST____LIST( )                          \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTableNode( param, a1, STL_FALSE,  \
                                                     QLP_JOIN_TYPE_NONE,    \
                                                     v1, v3, NULL ) );      \
    }


/**
 * table_reference : qllNode
 */

#define STL_PARSER_TABLE_REFERENCE____TABLE_FACTOR()    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_TABLE_REFERENCE____JOINED_TABLE()    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }


/**
 * table_factor : qllNode
 */

#define STL_PARSER_TABLE_FACTOR____()   \
    {                                   \
        STL_PARSER_CHECK( v0 = v1 );    \
    }


/**
 * joined_table : qllNode
 */

#define STL_PARSER_JOINED_TABLE____CROSS_JOIN() \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_JOINED_TABLE____QUALIFIED_JOIN() \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_JOINED_TABLE____NATURAL_JOIN()   \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_JOINED_TABLE____ESC_JOIN()       \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }


/**
 * cross_join : qllNode
 */

#define STL_PARSER_CROSS_JOIN____()                                         \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTableNode( param, a1, STL_FALSE,  \
                                                     QLP_JOIN_TYPE_CROSS,   \
                                                     v1, v4, NULL ) );      \
    }

/**
 * qualified_join : qlpTableRef
 */

#define STL_PARSER_QUALIFIED_JOIN____WITHOUT_JOIN_TYPE()                    \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTableNode( param, a1, STL_FALSE,  \
                                                     QLP_JOIN_TYPE_INNER,   \
                                                     v1, v3, v4 ) );        \
    }

#define STL_PARSER_QUALIFIED_JOIN____WITH_JOIN_TYPE()                       \
    {                                                                       \
        qlpJoinType  sJoinType = ((qlpJoinTypeInfo*)v2)->mJoinType;         \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTableNode( param, a1, STL_FALSE,  \
                                                     sJoinType,             \
                                                     v1, v4, v5 ) );        \
    }


/**
 * natural_join : qllNode
 */

#define STL_PARSER_NATURAL_JOIN____WITHOUT_JOIN_TYPE()                      \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTableNode( param, a1, STL_TRUE,   \
                                                     QLP_JOIN_TYPE_INNER,   \
                                                     v1, v4, NULL ) );      \
    }

#define STL_PARSER_NATURAL_JOIN____WITH_JOIN_TYPE()                         \
    {                                                                       \
        qlpJoinType  sJoinType = ((qlpJoinTypeInfo*)v3)->mJoinType;         \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTableNode( param, a1, STL_TRUE,   \
                                                     sJoinType,             \
                                                     v1, v5, NULL ) );      \
    }

/**
 * escape_sequence_join:   // qlpTableInfo
 */

#define STL_PARSER_ESCAPE_JOIN____OUTER_JOIN()                 \
    {                                                          \
        STL_PARSER_CHECK( v0 = v2 );                           \
    }


/**
 * escape_sequence_outer_join:    // qlpTableInfo
 */

#define STL_PARSER_ESCAPE_OUTER_JOIN____()                                  \
    {                                                                       \
        qlpJoinType  sJoinType = ((qlpJoinTypeInfo*)v3)->mJoinType;         \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTableNode( param, a2, STL_FALSE,  \
                                                     sJoinType,             \
                                                     v2, v6, v7 ) );        \
    }

/**
 * escape_sequence_outer_target:   // qlpTableInfo
 */

#define STL_PARSER_ESCAPE_OUTER_TARGET____TABLE_REF()          \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }

#define STL_PARSER_ESCAPE_OUTER_TARGET____OUTER_JOIN()         \
    {                                                                       \
        qlpJoinType  sJoinType = ((qlpJoinTypeInfo*)v2)->mJoinType;         \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTableNode( param, a1, STL_FALSE,  \
                                                     sJoinType,             \
                                                     v1, v5, v6 ) );        \
    }


/**
 * join_specification : qlpJoinSpecification
 */

#define STL_PARSER_JOIN_SPECIFICATION____JOIN_CONDITION()                           \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeJoinSpecification( param, a1, v1, NULL ) );   \
    }

#define STL_PARSER_JOIN_SPECIFICATION____NAMED_COLUMNS()                            \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeJoinSpecification( param, a1, NULL, v1 ) );   \
    }


/**
 * join_condition : qlpList
 */

#define STL_PARSER_JOIN_CONDITION____()         \
    {                                           \
        STL_PARSER_CHECK( v0 = v2 );            \
    }


/**
 * search_condition : qlpList
 */

#define STL_PARSER_SEARCH_CONDITION____()       \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * named_columns_join : qlpList
 */

#define STL_PARSER_NAMED_COLUMNS_JOIN____()     \
    {                                           \
        STL_PARSER_CHECK( v0 = v3 );            \
    }


/**
 * join_column_list : qlpList
 */

#define STL_PARSER_JOIN_COLUMN_LIST____()       \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * join_type : qlpJoinTypeInfo
 */

#define STL_PARSER_JOIN_TYPE____INNER()                                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTypeInfo( param, a1, QLP_JOIN_TYPE_INNER ) ); \
    }

#define STL_PARSER_JOIN_TYPE____WITHOUT_OUTER_KEYWORD() \
    {                                                   \
        v0 = v1;                                        \
    }

#define STL_PARSER_JOIN_TYPE____WITH_OUTER_KEYWORD()    \
    {                                                   \
        v0 = v1;                                        \
    }

#define STL_PARSER_JOIN_TYPE____SEMI()          \
    {                                           \
        v0 = v1;                                \
    }

#define STL_PARSER_JOIN_TYPE____ANTI_SEMI()     \
    {                                           \
        v0 = v1;                                \
    }


/**
 * outer_join_type : qlpJoinTypeInfo
 */

#define STL_PARSER_OUTER_JOIN_TYPE____LEFT()                                                    \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTypeInfo( param, a1, QLP_JOIN_TYPE_LEFT_OUTER ) );    \
    }

#define STL_PARSER_OUTER_JOIN_TYPE____RIGHT()                                                   \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTypeInfo( param, a1, QLP_JOIN_TYPE_RIGHT_OUTER ) );   \
    }

#define STL_PARSER_OUTER_JOIN_TYPE____FULL()                                                    \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTypeInfo( param, a1, QLP_JOIN_TYPE_FULL_OUTER ) );    \
    }


/**
 * semi_join_type : qlpJoinTypeInfo
 */

#define STL_PARSER_SEMI_JOIN_TYPE____LEFT()                                                     \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTypeInfo( param, a1, QLP_JOIN_TYPE_LEFT_SEMI ) );     \
    }

#define STL_PARSER_SEMI_JOIN_TYPE____RIGHT()                                                    \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTypeInfo( param, a1, QLP_JOIN_TYPE_RIGHT_SEMI ) );    \
    }


/**
 * anti_semi_join_type : qlpJoinTypeInfo
 */

#define STL_PARSER_ANTI_SEMI_JOIN_TYPE____LEFT()                                                    \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTypeInfo( param, a1, QLP_JOIN_TYPE_LEFT_ANTI_SEMI ) );    \
    }

#define STL_PARSER_ANTI_SEMI_JOIN_TYPE____RIGHT()                                                   \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeJoinTypeInfo( param, a1, QLP_JOIN_TYPE_RIGHT_ANTI_SEMI ) );   \
    }


/**
 * table_primary_element : qllNode
 */

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____NAME( )                                 \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeBaseTableNode( param, a1, v1, NULL, NULL ) ); \
    }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____NAME_ALIAS_WITH_AS( )                   \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeBaseTableNode( param, a1, v1, NULL, v3 ) );   \
    }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____NAME_ALIAS_WITH_BLANK( )                \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeBaseTableNode( param, a1, v1, NULL, v2 ) );   \
    }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DERIVED_TABLE( )                        \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeSubTableNode( param, a1, v1, NULL, NULL ) );  \
    }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DERIVED_TABLE_ALIAS_WITH_AS( )          \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeSubTableNode( param, a1, v1, v3, v4 ) );      \
    }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DERIVED_TABLE_ALIAS_WITH_BLANK( )       \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeSubTableNode( param, a1, v1, v2, v3 ) );      \
    }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____JOIN_TABLE( )                           \
    {                                                                               \
        v0 = v1;                                                                    \
    }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DUMP_TABLE( )                           \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeBaseTableNode( param, a1, v1,                 \
                                                     qlpMakeStrConstant( param,     \
                                                                         a3,        \
                                                                         v3 ),      \
                                                     NULL ) );                      \
    }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DUMP_TABLE_ALIAS_WITH_AS( )             \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeBaseTableNode( param, a1, v1,                 \
                                                     qlpMakeStrConstant( param,     \
                                                                         a3,        \
                                                                         v3 ),      \
                                                     v6 ) );                        \
    }

#define STL_PARSER_TABLE_PRIMARY_ELEMENT____DUMP_TABLE_ALIAS_WITH_BLANK( )          \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeBaseTableNode( param, a1, v1,                 \
                                                     qlpMakeStrConstant( param,     \
                                                                         a3,        \
                                                                         v3 ),      \
                                                     v5 ) );                        \
    }


/**
 * derived_column_list : qllList
 */

#define STL_PARSER_DERIVED_COLUMN_LIST____()    \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_DERIVED_COLUMN_LIST____COLUMN_LIST() \
    {                                                   \
        STL_PARSER_CHECK( v0 = v2 );                    \
    }


/**
 * derived_table : qllNode
 */

#define STL_PARSER_DERIVED_TABLE____()  \
    {                                   \
        v0 = v1;                        \
    }


/**
 * table_subquery : qllNode
 */

#define STL_PARSER_TABLE_SUBQUERY____() \
    {                                   \
        v0 = v1;                        \
    }


/**
 * subquery : qllNode
 */

#define STL_PARSER_SUBQUERY____NO_WITH_CLAUSE() \
    {                                           \
        v0 = v1;                                \
    }

#define STL_PARSER_SUBQUERY____WITH_CLAUSE()    \
    {                                           \
        v0 = v1;                                \
    }


/**
 * parenthesized_joined_table : qlpTableRef
 */

#define STL_PARSER_PARENTHESIZED_JOINED_TABLE____() \
    {                                               \
        v0 = v2;                                    \
    }

#define STL_PARSER_PARENTHESIZED_JOINED_TABLE____RECURSIVE_PAREN()  \
    {                                                               \
        v0 = v2;                                                    \
    }


/**
 * group_by_clause
 */

#define STL_PARSER_GROUP_BY_CLAUSE____EMPTY()   \
    {                                           \
        v0 = NULL;                              \
    }

#define STL_PARSER_GROUP_BY_CLAUSE____ELEMENT_LIST()                    \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeGroupBy( param, a1, v3 ) );       \
    }


/**
 * grouping_element_list
 */
    
#define STL_PARSER_GROUPING_ELEMENT_LIST____GROUPING_ELEMENT()          \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
    }

#define STL_PARSER_GROUPING_ELEMENT_LIST____GROUPING_ELEMENT_LIST()     \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                v1,                     \
                                                v3 ) );                 \
    }


/**
 * grouping_element
 */
    
#define STL_PARSER_GROUPING_ELEMENT____ORDINARY_GROUPING_SET()                  \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeGroupingElem( param,                      \
                                                    a1,                         \
                                                    QLP_GROUPING_TYPE_ORDINARY, \
                                                    v1 ) );                     \
    }

#define STL_PARSER_GROUPING_ELEMENT____EMPTY_GROUPING_SET()     \
    {                                                           \
        STL_PARSER_CHECK( v0 = NULL );                          \
    }


/**
 * ordinary_grouping_set
 */
    
#define STL_PARSER_ORDINARY_GROUPING_SET____GROUPING_COLUMN_REFERENCE() \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
    }

#define STL_PARSER_ORDINARY_GROUPING_SET____GROUPING_COLUMN_REFERENCE_LIST()    \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( v0 = sList );                                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v2 ) );           \
                                                                                \
        STL_PARSER_CHECK( qlpAddListToList( (qlpList *)v0, (qlpList *)v4 ) );   \
    }


/**
 * grouping_column_reference
 */
    
#define STL_PARSER_GROUPING_COLUMN_REFERENCE____VALUE_EXPRESSION()      \
    {                                                                   \
        v0 = v1;                                                        \
    }


/**
 * grouping_column_reference_list
 */
    
#define STL_PARSER_GROUPING_COLUMN_REFERENCE_LIST____GROUPING_COLUMN_REFERENCE()        \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
                                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v1 ) );                                 \
    }

#define STL_PARSER_GROUPING_COLUMN_REFERENCE_LIST____GROUPING_COLUMN_REFERENCE_LIST()   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                v1,                                     \
                                                v3 ) );                                 \
    }


/**
 * empty_grouping_set
 */

#define STL_PARSER_EMPTY_GROUPING_SET____EMPTY()        \
    {                                                   \
        v0 = NULL;                                      \
    }


/**
 * having_clause
 */

#define STL_PARSER_HAVING_CLAUSE____EMPTY()     \
    {                                           \
        v0 = NULL;                              \
    }

#define STL_PARSER_HAVING_CLAUSE____SEARCH_CONDITION()  \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHaving( param,    \
                                              a1,       \
                                              v2 ) );   \
    }


/**
 * window_clause
 */

#define STL_PARSER_WINDOW_CLAUSE____EMPTY()     \
    {                                           \
        v0 = NULL;                              \
    }


/**
 * order_by_clause
 */

#define STL_PARSER_ORDER_BY_CLAUSE____EMPTY()   \
    {                                           \
        v0 = NULL;                              \
    }

#define STL_PARSER_ORDER_BY_CLAUSE____SORT()    \
    {                                           \
        STL_PARSER_CHECK( v0 = v3 );            \
    }


/**
 * sort_specification_list
 */

#define STL_PARSER_SORT_SPECIFICATION_LIST____SORT_SPEC()               \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_SORT_SPECIFICATION_LIST____SORT_SPEC_LIST()          \
    {                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * sort_specification
 */

#define STL_PARSER_SORT_SPECIFICATION____KEY()                                      \
    {                                                                               \
        qlpValueExpr  * sExpr = NULL;                                               \
        STL_PARSER_CHECK( sExpr = qlpMakeValueExpr( param,                          \
                                                    a1,                             \
                                                    QLP_GET_LAST_POS( a1 ) - a1,    \
                                                    (qllNode*) v1 ) );              \
        STL_PARSER_CHECK( v0 = qlpMakeOrderBy( param,                               \
                                               a1,                                  \
                                               (qllNode*)sExpr,                     \
                                               NULL,                                \
                                               NULL ) );                            \
    }

#define STL_PARSER_SORT_SPECIFICATION____KEY_SORT()                                 \
    {                                                                               \
        qlpValueExpr  * sExpr = NULL;                                               \
        STL_PARSER_CHECK( sExpr = qlpMakeValueExpr( param,                          \
                                                    a1,                             \
                                                    a2 - a1,                        \
                                                    (qllNode*) v1 ) );              \
        STL_PARSER_CHECK( v0 = qlpMakeOrderBy( param,                               \
                                                  a1,                               \
                                                  (qllNode*)sExpr,                  \
                                                  v2,                               \
                                                  NULL ) );                         \
    }

#define STL_PARSER_SORT_SPECIFICATION____KEY_NULLS()                                \
    {                                                                               \
        qlpValueExpr  * sExpr = NULL;                                               \
        STL_PARSER_CHECK( sExpr = qlpMakeValueExpr( param,                          \
                                                    a1,                             \
                                                    a2 - a1,                        \
                                                    (qllNode*) v1 ) );              \
        STL_PARSER_CHECK( v0 = qlpMakeOrderBy( param,                               \
                                               a1,                                  \
                                               (qllNode*)sExpr,                     \
                                               NULL,                                \
                                               v2 ) );                              \
    }

#define STL_PARSER_SORT_SPECIFICATION____KEY_SORT_NULLS()                           \
    {                                                                               \
        qlpValueExpr  * sExpr = NULL;                                               \
        STL_PARSER_CHECK( sExpr = qlpMakeValueExpr( param,                          \
                                                    a1,                             \
                                                    a2 - a1,                        \
                                                    (qllNode*) v1 ) );              \
        STL_PARSER_CHECK( v0 = qlpMakeOrderBy( param,                               \
                                               a1,                                  \
                                               (qllNode*)sExpr,                     \
                                               v2,                                  \
                                               v3 ) );                              \
    }


/**
 * select_target_clause : qlpIntoClause
 */

#define STL_PARSER_SELECT_TARGET_CLAUSE____EMPTY( )     \
    {                                                   \
        STL_PARSER_CHECK( v0 = NULL );                  \
    }

#define STL_PARSER_SELECT_TARGET_CLAUSE____TARGETS( )           \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntoClause( param,        \
                                                  a1,           \
                                                  v2 ) );       \
    }


/**
 * select_target_list : qlpList : qlpTarget
 */

#define STL_PARSER_SELECT_TARGET_LIST____TARGET( )                      \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }

#define STL_PARSER_SELECT_TARGET_LIST____TARGET_LIST( )                 \
    {                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * target_specification : qlpParameter
 */

#define STL_PARSER_TARGET_SPECIFICATION____HOST_PARAMETER( )                        \
    {                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
        STL_PARSER_CHECK(                                                           \
            qlpAddPtrValueToList( param,                                            \
                                  sList,                                            \
                                  qlpMakeStrConstant( param, a1, v1 ) ) );          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v2 ) );               \
                                                                                    \
        STL_PARSER_CHECK( v0 = (qllNode *) qlpMakeParameter( param,                 \
                                                             a1,                    \
                                                             QLP_PARAM_TYPE_HOST,   \
                                                             KNL_BIND_TYPE_OUT,     \
                                                             a1,                    \
                                                             sList ) );             \
    }

#define STL_PARSER_TARGET_SPECIFICATION____DYNAMIC_PARAMETER( )                         \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = (qllNode *) qlpMakeParameter( param,                     \
                                                             a1,                        \
                                                             QLP_PARAM_TYPE_DYNAMIC,    \
                                                             KNL_BIND_TYPE_OUT,         \
                                                             a1,                        \
                                                             NULL ) );                  \
    }

/**
 * select_list : qllNode ( qlpList )
 */

#define STL_PARSER_SELECT_LIST____SUBLIST( )    \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_SELECT_LIST____ALL( )                                                \
    {                                                                                   \
        qlpList  * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                 \
        STL_PARSER_CHECK( v0 = (qllNode*) sList );                                      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                qlpMakeTargetAsterisk( param,           \
                                                                       a1,              \
                                                                       "*",             \
                                                                       a1,              \
                                                                       NULL,            \
                                                                       NULL,            \
                                                                       NULL,            \
                                                                       NULL ) ) );      \
    }


/**
 * select_sublist : qlpList : qlpTarget
 */

#define STL_PARSER_SELECT_SUBLIST____SUBLIST2( )                        \
    {                                                                   \
        qlpList  * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        STL_PARSER_CHECK( v0 = (qllNode*) sList );                      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
    }

#define STL_PARSER_SELECT_SUBLIST____SUBLIST2_LIST( )           \
    {                                                           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,          \
                                                v1,             \
                                                v3 ) );         \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }


/**
 * select_sublist2 : qlpTarget
 */

#define STL_PARSER_SELECT_SUBLIST2____DERIVED_COLUMN( )         \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_SELECT_SUBLIST2____IDENTIFIER_ASTERISK( )                        \
    {                                                                               \
        stlInt32        sNameLen    = QLP_GET_LAST_POS( a1 ) - a1;                  \
        stlChar       * sBuffer     = NULL;                                         \
                                                                                    \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param,                           \
                                                   sNameLen + 1 ) );                \
        stlMemcpy( sBuffer, & param->mBuffer[a1], sNameLen );                       \
        sBuffer[sNameLen] = '\0';                                                   \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeTargetAsterisk( param,                        \
                                                      a1,                           \
                                                      sBuffer,                      \
                                                      a1,                           \
                                                      NULL,                         \
                                                      v1,                           \
                                                      NULL,                         \
                                                      NULL ) );                     \
    }

#define STL_PARSER_SELECT_SUBLIST2____VALUE_EXPRESSION_ASTERISK( )                      \
    {                                                                                   \
        stlInt32        sNameLen = QLP_GET_LAST_POS( a1 ) - a1;                         \
        stlChar       * sBuffer  = NULL;                                                \
        qlpValueExpr  * sExpr    = NULL;                                                \
                                                                                        \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param,                               \
                                                   sNameLen + 1 ) );                    \
        stlMemcpy( sBuffer, & param->mBuffer[a1], sNameLen );                           \
        sBuffer[sNameLen] = '\0';                                                       \
                                                                                        \
        STL_PARSER_CHECK( sExpr = qlpMakeValueExpr( param,                              \
                                                    a1,                                 \
                                                    QLP_GET_LAST_POS( a1 ) - a1,        \
                                                    (qllNode*) v1 ) );                  \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeTargetAsterisk( param,                            \
                                                      a1,                               \
                                                      sBuffer,                          \
                                                      a1,                               \
                                                      NULL,                             \
                                                      NULL,                             \
                                                      NULL,                             \
                                                      (qllNode*)sExpr ) );              \
    }

#define STL_PARSER_SELECT_SUBLIST2____VALUE_EXPRESSION_ASTERISK_WITH_AS( )              \
    {                                                                                   \
        stlInt32        sNameLen = QLP_GET_LAST_POS( a1 ) - a1;                         \
        stlChar       * sBuffer  = NULL;                                                \
        qlpValueExpr  * sExpr    = NULL;                                                \
                                                                                        \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param,                               \
                                                   sNameLen + 1 ) );                    \
        stlMemcpy( sBuffer, & param->mBuffer[a1], sNameLen );                           \
        sBuffer[sNameLen] = '\0';                                                       \
                                                                                        \
        STL_PARSER_CHECK( sExpr = qlpMakeValueExpr( param,                              \
                                                    a1,                                 \
                                                    QLP_GET_LAST_POS( a1 ) - a1,        \
                                                    (qllNode*) v1 ) );                  \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeTargetAsterisk( param,                            \
                                                      a1,                               \
                                                      sBuffer,                          \
                                                      a1,                               \
                                                      NULL,                             \
                                                      NULL,                             \
                                                      v6,                               \
                                                      (qllNode*)sExpr ) );              \
    }


/**
 * derived_column
 */

#define STL_PARSER_DERIVED_COLUMN____VALUE( )                                           \
    {                                                                                   \
        stlInt32        sNameLen = QLP_GET_LAST_POS( a1 ) - a1;                         \
        stlChar       * sBuffer  = NULL;                                                \
        qlpValueExpr  * sExpr    = NULL;                                                \
                                                                                        \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param,                               \
                                                   sNameLen + 1 ) );                    \
        stlMemcpy( sBuffer, & param->mBuffer[a1], sNameLen );                           \
        sBuffer[sNameLen] = '\0';                                                       \
                                                                                        \
        STL_PARSER_CHECK( sExpr = qlpMakeValueExpr( param,                              \
                                                    a1,                                 \
                                                    QLP_GET_LAST_POS( a1 ) - a1,        \
                                                    (qllNode*) v1 ) );                  \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeTarget( param,                                    \
                                              a1,                                       \
                                              sBuffer,                                  \
                                              a1,                                       \
                                              NULL,                                     \
                                              (qllNode*)sExpr,                          \
                                              NULL ) );                                 \
    }

#define STL_PARSER_DERIVED_COLUMN____VALUE_ALIAS_WITH_AS( )             \
    {                                                                   \
        qlpValueExpr  * sExpr = NULL;                                   \
        STL_PARSER_CHECK( sExpr = qlpMakeValueExpr( param,              \
                                                    a1,                 \
                                                    a2 - a1,            \
                                                    (qllNode*) v1 ) );  \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeTarget( param,                    \
                                              a1,                       \
                                              QLP_GET_PTR_VALUE( v3 ),  \
                                              a3,                       \
                                              NULL,                     \
                                              (qllNode*) sExpr,         \
                                              NULL ) );                 \
    }

#define STL_PARSER_DERIVED_COLUMN____VALUE_ALIAS_WITH_BLANK( )          \
    {                                                                   \
        qlpValueExpr  * sExpr = NULL;                                   \
        STL_PARSER_CHECK( sExpr = qlpMakeValueExpr( param,              \
                                                    a1,                 \
                                                    a2 - a1,            \
                                                    (qllNode*) v1 ) );  \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeTarget( param,                    \
                                              a1,                       \
                                              QLP_GET_PTR_VALUE( v2 ),  \
                                              a2,                       \
                                              NULL,                     \
                                              (qllNode*) sExpr,         \
                                              NULL ) );                 \
    }


/**
 * hint_clause
 */

#define STL_PARSER_HINT_CLAUSE____EMPTY( )      \
    {                                           \
        param->mErrSkip = STL_FALSE;            \
        param->mErrClear = STL_FALSE;           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_HINT_CLAUSE____LIST( )       \
    {                                           \
        param->mErrSkip = STL_FALSE;            \
        param->mErrClear = STL_FALSE;           \
        STL_PARSER_CHECK( v0 = v2 );            \
    }


/**
 * hint_list
 */

#define STL_PARSER_HINT_LIST____()                              \
    {                                                           \
        qlpList           * sList = NULL;                       \
                                                                \
        sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );    \
        STL_PARSER_CHECK( v0 = sList );                         \
        if( v1 != NULL )                                        \
        {                                                       \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,      \
                                                    sList,      \
                                                    v1 ) );     \
        }                                                       \
        else                                                    \
        {                                                       \
            if( param->mErrClear == STL_TRUE )                  \
            {                                                   \
                yyclearin;                                      \
            }                                                   \
            param->mErrClear = STL_FALSE;                       \
        }                                                       \
    }

#define STL_PARSER_HINT_LIST____LIST()                          \
    {                                                           \
        if( v2 != NULL )                                        \
        {                                                       \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,      \
                                                    v1,         \
                                                    v2 ) );     \
        }                                                       \
        else                                                    \
        {                                                       \
            if( param->mErrClear == STL_TRUE )                  \
            {                                                   \
                yyclearin;                                      \
            }                                                   \
            param->mErrClear = STL_FALSE;                       \
        }                                                       \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_HINT_LIST____COMMA_LIST()                    \
    {                                                           \
        if( v3 != NULL )                                        \
        {                                                       \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,      \
                                                    v1,         \
                                                    v3 ) );     \
        }                                                       \
        param->mErrClear = STL_FALSE;                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }


/**
 * hint_element
 */

#define STL_PARSER_HINT_ELEMENT____ACCESS_PATH_HINTS()  \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_HINT_ELEMENT____JOIN_HINTS() \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_HINT_ELEMENT____QUERY_TRANSFORMATION_HINTS() \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_HINT_ELEMENT____OTHER_HINTS()        \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_HINT_ELEMENT____ERROR()                                              \
    {                                                                                   \
        stlBool  sEnableHintErr = STL_FALSE;                                            \
                                                                                        \
        if( STL_PARSER_CHAR != YYEOF )                                                  \
        {                                                                               \
            /* Hint Error Property에 따라 수행 */                                       \
            QLP_GET_HINT_ERROR_FROM_PROPERTY( &sEnableHintErr );                        \
                                                                                        \
            if( sEnableHintErr == STL_FALSE )                                           \
            {                                                                           \
                /* Hint Error 발생시키지 않음 */                                        \
                param->mErrStatus = STL_SUCCESS;                                        \
                param->mErrSkip = STL_TRUE;                                             \
                param->mErrClear = STL_TRUE;                                            \
                param->mHasHintErr = STL_TRUE;                                          \
                yyerrok;                                                                \
                STL_PARSER_CHECK( v0 = NULL );                                          \
            }                                                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            param->mErrStatus = STL_FAILURE;                                            \
        }                                                                               \
    }


/**
 * access_path_hints
 */

#define STL_PARSER_ACCESS_PATH_HINTS____ERROR()                                         \
    {                                                                                   \
        stlBool  sEnableHintErr = STL_FALSE;                                            \
                                                                                        \
        /* Hint Error Property에 따라 수행 */                                           \
        QLP_GET_HINT_ERROR_FROM_PROPERTY( &sEnableHintErr );                            \
                                                                                        \
        if( sEnableHintErr == STL_FALSE )                                               \
        {                                                                               \
            /* Hint Error 발생시키지 않음 */                                            \
            param->mErrStatus = STL_SUCCESS;                                            \
            param->mErrSkip = STL_TRUE;                                                 \
            param->mErrClear = STL_FALSE;                                               \
            param->mHasHintErr = STL_TRUE;                                              \
            yyerrok;                                                                    \
            STL_PARSER_CHECK( v0 = NULL );                                              \
        }                                                                               \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____FULL()                                          \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintTable( param,                                 \
                                                 a1,                                    \
                                                 QLP_TABLE_HINT_FULL,                   \
                                                 v3 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____HASH()                                          \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintTable( param,                                 \
                                                 a1,                                    \
                                                 QLP_TABLE_HINT_HASH,                   \
                                                 v3 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX()                                         \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX,                  \
                                                 v3,                                    \
                                                 v4 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_ONLY_TABLE_NAME()                         \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX,                  \
                                                 v3,                                    \
                                                 NULL ) );                              \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_WITH_COMMA()                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX,                  \
                                                 v3,                                    \
                                                 v5 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____NO_INDEX()                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_NO_INDEX,               \
                                                 v3,                                    \
                                                 v4 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____NO_INDEX_ONLY_TABLE_NAME()                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_NO_INDEX,               \
                                                 v3,                                    \
                                                 NULL ) );                              \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____NO_INDEX_WITH_COMMA()                           \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_NO_INDEX,               \
                                                 v3,                                    \
                                                 v5 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_ASC()                                     \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX_ASC,              \
                                                 v3,                                    \
                                                 v4 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_ASC_ONLY_TABLE_NAME()                     \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX_ASC,              \
                                                 v3,                                    \
                                                 NULL ) );                              \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_ASC_WITH_COMMA()                          \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX_ASC,              \
                                                 v3,                                    \
                                                 v5 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_DESC()                                    \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX_DESC,             \
                                                 v3,                                    \
                                                 v4 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_DESC_ONLY_TABLE_NAME()                    \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX_DESC,             \
                                                 v3,                                    \
                                                 NULL ) );                              \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_DESC_WITH_COMMA()                         \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX_DESC,             \
                                                 v3,                                    \
                                                 v5 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____ROWID()                                         \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintTable( param,                                 \
                                                 a1,                                    \
                                                 QLP_TABLE_HINT_ROWID,                  \
                                                 v3 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_COMBINE()                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX_COMBINE,          \
                                                 v3,                                    \
                                                 v4 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_COMBINE_ONLY_TABLE_NAME()                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX_COMBINE,          \
                                                 v3,                                    \
                                                 NULL ) );                              \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____INDEX_COMBINE_WITH_COMMA()                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_INDEX_COMBINE,          \
                                                 v3,                                    \
                                                 v5 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____IN_KEY_RANGE()                                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_IN_KEY_RANGE,           \
                                                 v3,                                    \
                                                 v4 ) );                                \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____IN_KEY_RANGE_ONLY_TABLE_NAME()                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_IN_KEY_RANGE,           \
                                                 v3,                                    \
                                                 NULL ) );                              \
    }

#define STL_PARSER_ACCESS_PATH_HINTS____IN_KEY_RANGE_WITH_COMMA()                       \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintIndex( param,                                 \
                                                 a1,                                    \
                                                 QLP_INDEX_HINT_IN_KEY_RANGE,           \
                                                 v3,                                    \
                                                 v5 ) );                                \
    }


/**
 * join_hints
 */

#define STL_PARSER_JOIN_HINTS____ORDERED()                                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintJoinOrder( param,                             \
                                                     a1,                                \
                                                     QLP_JOIN_ORDER_HINT_ORDERED,       \
                                                     NULL ) );                          \
    }

#define STL_PARSER_JOIN_HINTS____ORDERING()                                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintJoinOrder( param,                             \
                                                     a1,                                \
                                                     QLP_JOIN_ORDER_HINT_ORDERING,      \
                                                     v3 ) );                            \
    }

#define STL_PARSER_JOIN_HINTS____LEADING()                                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintJoinOrder( param,                             \
                                                     a1,                                \
                                                     QLP_JOIN_ORDER_HINT_LEADING,       \
                                                     v3 ) );                            \
    }

#define STL_PARSER_JOIN_HINTS____USE_HASH()                                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintJoinOper( param,                              \
                                                    a1,                                 \
                                                    QLP_JOIN_OPER_HINT_HASH,            \
                                                    STL_FALSE,                          \
                                                    v3 ) );                             \
    }

#define STL_PARSER_JOIN_HINTS____NO_USE_HASH()                                          \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintJoinOper( param,                              \
                                                    a1,                                 \
                                                    QLP_JOIN_OPER_HINT_HASH,            \
                                                    STL_TRUE,                           \
                                                    v3 ) );                             \
    }

#define STL_PARSER_JOIN_HINTS____USE_INL()                                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintJoinOper( param,                              \
                                                    a1,                                 \
                                                    QLP_JOIN_OPER_HINT_INL,             \
                                                    STL_FALSE,                          \
                                                    v3 ) );                             \
    }

#define STL_PARSER_JOIN_HINTS____NO_USE_INL()                                           \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintJoinOper( param,                              \
                                                    a1,                                 \
                                                    QLP_JOIN_OPER_HINT_INL,             \
                                                    STL_TRUE,                           \
                                                    v3 ) );                             \
    }

#define STL_PARSER_JOIN_HINTS____USE_MERGE()                                            \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintJoinOper( param,                              \
                                                    a1,                                 \
                                                    QLP_JOIN_OPER_HINT_MERGE,           \
                                                    STL_FALSE,                          \
                                                    v3 ) );                             \
    }

#define STL_PARSER_JOIN_HINTS____NO_USE_MERGE()                                         \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintJoinOper( param,                              \
                                                    a1,                                 \
                                                    QLP_JOIN_OPER_HINT_MERGE,           \
                                                    STL_TRUE,                           \
                                                    v3 ) );                             \
    }

#define STL_PARSER_JOIN_HINTS____USE_NL()                                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintJoinOper( param,                              \
                                                    a1,                                 \
                                                    QLP_JOIN_OPER_HINT_NL,              \
                                                    STL_FALSE,                          \
                                                    v3 ) );                             \
    }

#define STL_PARSER_JOIN_HINTS____NO_USE_NL()                                            \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintJoinOper( param,                              \
                                                    a1,                                 \
                                                    QLP_JOIN_OPER_HINT_NL,              \
                                                    STL_TRUE,                           \
                                                    v3 ) );                             \
    }


/**
 * query_transformation_hints
 */

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____UNNEST()                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintQueryTransform(                               \
                                   param,                                               \
                                   a1,                                                  \
                                   QLP_QUERY_TRANSFORM_HINT_UNNEST ) );                 \
    }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____NO_UNNEST()                            \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintQueryTransform(                               \
                                   param,                                               \
                                   a1,                                                  \
                                   QLP_QUERY_TRANSFORM_HINT_NO_UNNEST ) );              \
    }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____NL_ISJ()                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintQueryTransform(                               \
                                   param,                                               \
                                   a1,                                                  \
                                   QLP_QUERY_TRANSFORM_HINT_NL_ISJ ) );                 \
    }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____NL_SJ()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintQueryTransform(                               \
                                   param,                                               \
                                   a1,                                                  \
                                   QLP_QUERY_TRANSFORM_HINT_NL_SJ ) );                  \
    }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____NL_AJ()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintQueryTransform(                               \
                                   param,                                               \
                                   a1,                                                  \
                                   QLP_QUERY_TRANSFORM_HINT_NL_AJ ) );                  \
    }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____MERGE_SJ()                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintQueryTransform(                               \
                                   param,                                               \
                                   a1,                                                  \
                                   QLP_QUERY_TRANSFORM_HINT_MERGE_SJ ) );               \
    }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____MERGE_AJ()                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintQueryTransform(                               \
                                   param,                                               \
                                   a1,                                                  \
                                   QLP_QUERY_TRANSFORM_HINT_MERGE_AJ ) );               \
    }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____HASH_ISJ()                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintQueryTransform(                               \
                                   param,                                               \
                                   a1,                                                  \
                                   QLP_QUERY_TRANSFORM_HINT_HASH_ISJ ) );               \
    }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____HASH_SJ()                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintQueryTransform(                               \
                                   param,                                               \
                                   a1,                                                  \
                                   QLP_QUERY_TRANSFORM_HINT_HASH_SJ ) );                \
    }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____HASH_AJ()                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintQueryTransform(                               \
                                   param,                                               \
                                   a1,                                                  \
                                   QLP_QUERY_TRANSFORM_HINT_HASH_AJ ) );                \
    }

#define STL_PARSER_QUERY_TRANSFORMATION_HINTS____NO_QUERY_TRANSFORMATION()              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintQueryTransform(                               \
                                   param,                                               \
                                   a1,                                                  \
                                   QLP_QUERY_TRANSFORM_HINT_NO_QUERY_TRANSFORMATION ) );\
    }


/**
 * other_hints
 */

#define STL_PARSER_OTHER_HINTS____PUSH_PRED()                                           \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintOther( param,                                 \
                                                 a1,                                    \
                                                 STL_FALSE,                             \
                                                 NULL,                                  \
                                                 QLP_OTHER_HINT_PUSH_PRED ) );          \
    }

#define STL_PARSER_OTHER_HINTS____PUSH_PRED_WITH_TABLE()                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintOther( param,                                 \
                                                 a1,                                    \
                                                 STL_FALSE,                             \
                                                 v3,                                    \
                                                 QLP_OTHER_HINT_PUSH_PRED ) );          \
    }

#define STL_PARSER_OTHER_HINTS____NO_PUSH_PRED()                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintOther( param,                                 \
                                                 a1,                                    \
                                                 STL_TRUE,                              \
                                                 NULL,                                  \
                                                 QLP_OTHER_HINT_PUSH_PRED ) );          \
    }

#define STL_PARSER_OTHER_HINTS____NO_PUSH_PRED_WITH_TABLE()                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintOther( param,                                 \
                                                 a1,                                    \
                                                 STL_TRUE,                              \
                                                 v3,                                    \
                                                 QLP_OTHER_HINT_PUSH_PRED ) );          \
    }

#define STL_PARSER_OTHER_HINTS____PUSH_SUBQ()                                           \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintOther( param,                                 \
                                                 a1,                                    \
                                                 STL_FALSE,                             \
                                                 NULL,                                  \
                                                 QLP_OTHER_HINT_PUSH_SUBQ ) );          \
    }

#define STL_PARSER_OTHER_HINTS____NO_PUSH_SUBQ()                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintOther( param,                                 \
                                                 a1,                                    \
                                                 STL_TRUE,                              \
                                                 NULL,                                  \
                                                 QLP_OTHER_HINT_PUSH_SUBQ) );           \
    }

#define STL_PARSER_OTHER_HINTS____USE_DISTINCT_HASH()                                   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintOther( param,                                 \
                                                 a1,                                    \
                                                 STL_FALSE,                             \
                                                 NULL,                                  \
                                                 QLP_OTHER_HINT_USE_DISTINCT_HASH) );   \
    }

#define STL_PARSER_OTHER_HINTS____USE_GROUP_HASH()                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeHintOther( param,                                 \
                                                 a1,                                    \
                                                 STL_FALSE,                             \
                                                 NULL,                                  \
                                                 QLP_OTHER_HINT_USE_GROUP_HASH) );      \
    }


/**
 * table_identifier_chain_list : qlpList : qlpRelInfo
 */

#define STL_PARSER_TABLE_IDENTIFIER_CHAIN_LIST____()                                    \
    {                                                                                   \
        qlpList         * sList         = NULL;                                         \
                                                                                        \
        sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                            \
        STL_PARSER_CHECK( v0 = sList );                                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v1 ) );                                 \
    }

#define STL_PARSER_TABLE_IDENTIFIER_CHAIN_LIST____LIST()                                \
    {                                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                v1,                                     \
                                                v2 ) );                                 \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }

#define STL_PARSER_TABLE_IDENTIFIER_CHAIN_LIST____COMMA_LIST()                          \
    {                                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                v1,                                     \
                                                v3 ) );                                 \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }


/**
 * table_identifier_chain : qlpObjectName
 */

#define STL_PARSER_TABLE_IDENTIFIER_CHAIN____()                                         \
    {                                                                                   \
        qlpObjectName * sObjectName = NULL;                                             \
                                                                                        \
        STL_PARSER_CHECK( sObjectName = qlpMakeObjectName( param, v1 ) );               \
        STL_PARSER_CHECK( v0 = sObjectName );                                           \
    }


/**
 * ordering_table_identifier_chain_list : qlpList
 */

#define STL_PARSER_ORDERING_TABLE_IDENTIFIER_CHAIN_LIST____()                           \
    {                                                                                   \
        qlpList         * sList         = NULL;                                         \
                                                                                        \
        sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                            \
        STL_PARSER_CHECK( v0 = sList );                                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v1 ) );                                 \
    }

#define STL_PARSER_ORDERING_TABLE_IDENTIFIER_CHAIN_LIST____COMMA_LIST()                 \
    {                                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                v1,                                     \
                                                v3 ) );                                 \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }


/**
 * ordering_table_identifier_chain : qlpOrdering
 */

#define STL_PARSER_ORDERING_TABLE_IDENTIFIER_CHAIN____()                                \
    {                                                                                   \
        qlpObjectName   * sObjectName   = NULL;                                         \
        qlpOrdering     * sOrdering     = NULL;                                         \
                                                                                        \
        STL_PARSER_CHECK( sObjectName = qlpMakeObjectName( param, v1 ) );               \
        STL_PARSER_CHECK( sOrdering = qlpMakeOrdering( param,                           \
                                                       a1,                              \
                                                       sObjectName,                     \
                                                       STL_FALSE,                       \
                                                       STL_FALSE ) );                   \
        STL_PARSER_CHECK( v0 = sOrdering );                                             \
    }

#define STL_PARSER_ORDERING_TABLE_IDENTIFIER_CHAIN____LEFT()                            \
    {                                                                                   \
        qlpObjectName   * sObjectName   = NULL;                                         \
        qlpOrdering     * sOrdering     = NULL;                                         \
                                                                                        \
        STL_PARSER_CHECK( sObjectName = qlpMakeObjectName( param, v1 ) );               \
        STL_PARSER_CHECK( sOrdering = qlpMakeOrdering( param,                           \
                                                       a1,                              \
                                                       sObjectName,                     \
                                                       STL_TRUE,                        \
                                                       STL_TRUE ) );                    \
        STL_PARSER_CHECK( v0 = sOrdering );                                             \
    }

#define STL_PARSER_ORDERING_TABLE_IDENTIFIER_CHAIN____RIGHT()                           \
    {                                                                                   \
        qlpObjectName   * sObjectName   = NULL;                                         \
        qlpOrdering     * sOrdering     = NULL;                                         \
                                                                                        \
        STL_PARSER_CHECK( sObjectName = qlpMakeObjectName( param, v1 ) );               \
        STL_PARSER_CHECK( sOrdering = qlpMakeOrdering( param,                           \
                                                       a1,                              \
                                                       sObjectName,                     \
                                                       STL_TRUE,                        \
                                                       STL_FALSE ) );                   \
        STL_PARSER_CHECK( v0 = sOrdering );                                             \
    }


/**
 * index_identifier_chain_list : qlpList : qlpIndexInfo
 */

#define STL_PARSER_INDEX_IDENTIFIER_CHAIN_LIST____()                                    \
    {                                                                                   \
        qlpList         * sList         = NULL;                                         \
                                                                                        \
        sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                            \
        STL_PARSER_CHECK( v0 = sList );                                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v1 ) );                                 \
    }

#define STL_PARSER_INDEX_IDENTIFIER_CHAIN_LIST____LIST()                                \
    {                                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                v1,                                     \
                                                v2 ) );                                 \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }

#define STL_PARSER_INDEX_IDENTIFIER_CHAIN_LIST____COMMA_LIST()                          \
    {                                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                v1,                                     \
                                                v3 ) );                                 \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }


/**
 * index_identifier_chain : qlpIndexInfo
 */

#define STL_PARSER_INDEX_IDENTIFIER_CHAIN____()                                         \
    {                                                                                   \
        qlpIndexScan      * sIndexScan = NULL;                                          \
                                                                                        \
        STL_PARSER_CHECK( sIndexScan = qlpMakeIndexScan( param,                         \
                                                         a1,                            \
                                                         v1 ) );                        \
        STL_PARSER_CHECK( v0 = sIndexScan );                                            \
    }


/**
 * result skip and limit
 *
 * OFFSET .. FETCH ..
 * OFFSET .. LIMIT ..
 */

#define STL_PARSER_RESULT_OFFSET_CLAUSE____EMPTY()  \
    {                                               \
        STL_PARSER_CHECK( v0 = NULL );              \
    }

#define STL_PARSER_RESULT_OFFSET_CLAUSE____COUNT()  \
    {                                               \
        STL_PARSER_CHECK( v0 = v2 );                \
    }

#define STL_PARSER_FETCH_LIMIT_CLAUSE____EMPTY()    \
    {                                               \
        STL_PARSER_CHECK( v0 = NULL );              \
    }

#define STL_PARSER_FETCH_LIMIT_CLAUSE____COUNT()    \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_FETCH_FIRST_CLAUSE____COUNT()    \
    {                                               \
        STL_PARSER_CHECK( v0 = v3 );                \
    }

#define STL_PARSER_FETCH_FIRST_CLAUSE____EMPTY()                \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeNumberConstant( param,    \
                                                      a1,       \
                                                      "1" ) );  \
    }

#define STL_PARSER_LIMIT_CLAUSE____COUNT()      \
    {                                           \
        STL_PARSER_CHECK( v0 = v2 );            \
    }

#define STL_PARSER_LIMIT_CLAUSE____ALL()        \
    {                                           \
        STL_PARSER_CHECK( v0 = NULL );          \
    }

#define STL_PARSER_LIMIT_CLAUSE____SKIP_LIMIT()                         \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v2 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v4 ) );                 \
        STL_PARSER_CHECK( v0 = sList );                                 \
    }



/**
 * value_expression
 */

#define STL_PARSER_VALUE_EXPRESSION____( )      \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * value_expression_primary
 */

#define STL_PARSER_VALUE_EXPRESSION_PRIMARY____OR( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }


/**
 * parenthesized_value_expression
 */

#define STL_PARSER_PARENTHESIZED_VALUE_EXPRESSION____EXPRESSION( )      \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v2 );                                    \
    }

#define STL_PARSER_PARENTHESIZED_VALUE_EXPRESSION____QUERY( )                           \
    {                                                                                   \
        qllNode  * sNode;                                                               \
        STL_PARSER_CHECK( sNode = qlpAddQueryNodeOption( param, a1,                     \
                                                         v2, NULL, v3, a3, v4, v5 ) );  \
        STL_PARSER_CHECK( v0 = qlpMakeSubTableNode( param, a2, sNode, NULL, NULL ) );   \
    }


/**
 * value_OR_term
 */

#define STL_PARSER_VALUE_OR_TERM____SINGLE( )   \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_OR_TERM____OR( )                                                       \
    {                                                                                           \
        qlpList * sList;                                                                        \
        if( (QLL_NODE_TYPE( v1 ) != QLL_FUNCTION_TYPE) ||                                       \
            (((qlpFunction*)(v1))->mFuncId != KNL_BUILTIN_FUNC_OR ) )                           \
        {                                                                                       \
            if( (QLL_NODE_TYPE( v3 ) == QLL_FUNCTION_TYPE) &&                                   \
                (((qlpFunction*)(v3))->mFuncId == KNL_BUILTIN_FUNC_OR ) )                       \
            {                                                                                   \
                sList = qlpGetFunctionArgList( v3 );                                            \
                STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );                   \
                STL_PARSER_CHECK( v0 = v3 );                                                    \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                            \
                STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                        sList,                                  \
                                                        v1 ) );                                 \
                STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                        sList,                                  \
                                                        v3 ) );                                 \
                                                                                                \
                STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                        KNL_BUILTIN_FUNC_OR,                    \
                                                        a1,                                     \
                                                        QLP_GET_LAST_POS( a3 ) - a1 ,           \
                                                        sList ) );                              \
            }                                                                                   \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            if( (QLL_NODE_TYPE( v3 ) == QLL_FUNCTION_TYPE) &&                                   \
                (((qlpFunction*)(v3))->mFuncId == KNL_BUILTIN_FUNC_OR ) )                       \
            {                                                                                   \
                sList = qlpGetFunctionArgList( v1 );                                            \
                STL_PARSER_CHECK( qlpAddListToList( sList, qlpGetFunctionArgList( v3 ) ) );     \
                STL_PARSER_CHECK( v0 = v1 );                                                    \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                sList = qlpGetFunctionArgList( v1 );                                            \
                STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v3 ) );                   \
                STL_PARSER_CHECK( v0 = v1 );                                                    \
            }                                                                                   \
        }                                                                                       \
    }


/**
 * value_AND_term
 */

#define STL_PARSER_VALUE_AND_TERM____SINGLE( )  \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_AND_TERM____AND( )                                                     \
    {                                                                                           \
        qlpList * sList;                                                                        \
        if( (QLL_NODE_TYPE( v1 ) != QLL_FUNCTION_TYPE) ||                                       \
            (((qlpFunction*)(v1))->mFuncId != KNL_BUILTIN_FUNC_AND ) )                          \
        {                                                                                       \
            if( (QLL_NODE_TYPE( v3 ) == QLL_FUNCTION_TYPE) &&                                   \
                (((qlpFunction*)(v3))->mFuncId == KNL_BUILTIN_FUNC_AND ) )                      \
            {                                                                                   \
                sList = qlpGetFunctionArgList( v3 );                                            \
                STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );                   \
                STL_PARSER_CHECK( v0 = v3 );                                                    \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                            \
                STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                        sList,                                  \
                                                        v1 ) );                                 \
                STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                        sList,                                  \
                                                        v3 ) );                                 \
                                                                                                \
                STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                        KNL_BUILTIN_FUNC_AND,                   \
                                                        a1,                                     \
                                                        QLP_GET_LAST_POS( a3 ) - a1 ,           \
                                                        sList ) );                              \
            }                                                                                   \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            if( (QLL_NODE_TYPE( v3 ) == QLL_FUNCTION_TYPE) &&                                   \
                (((qlpFunction*)(v3))->mFuncId == KNL_BUILTIN_FUNC_AND ) )                      \
            {                                                                                   \
                sList = qlpGetFunctionArgList( v1 );                                            \
                STL_PARSER_CHECK( qlpAddListToList( sList, qlpGetFunctionArgList( v3 ) ) );     \
                STL_PARSER_CHECK( v0 = v1 );                                                    \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                sList = qlpGetFunctionArgList( v1 );                                            \
                STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v3 ) );                   \
                STL_PARSER_CHECK( v0 = v1 );                                                    \
            }                                                                                   \
        }                                                                                       \
    }


/**
 * value_NOT_term
 */

#define STL_PARSER_VALUE_NOT_TERM____SINGLE( )  \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_NOT_TERM____NOT( )                                                     \
    {                                                                                           \
        if( qlpIsExistsFunc( (void *) v2 ) == STL_TRUE )                                        \
        {                                                                                       \
            ((qlpListFunc *) v2)->mFuncId = KNL_BUILTIN_FUNC_NOT_EXISTS;                        \
            ((qlpListFunc *) v2)->mNodePos = a1;                                                \
            ((qlpListFunc *) v2)->mNodeLen = QLP_GET_LAST_POS( a2 ) - a1;                       \
            ((qlpListCol *) ((qlpListFunc *) v2)->mArgs->mHead->mData.mPtrValue )->mFuncId =    \
                KNL_BUILTIN_FUNC_NOT_EXISTS;                                                    \
                                                                                                \
            STL_PARSER_CHECK( v0 = v2 );                                                        \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                      \
                                                                                                \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v2 ) );                                     \
                                                                                                \
            STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                      \
                                                    KNL_BUILTIN_FUNC_NOT,                       \
                                                    a1,                                         \
                                                    QLP_GET_LAST_POS( a2 ) - a1 ,               \
                                                    sList ) );                                  \
        }                                                                                       \
    }


/**
 * value_test
 */

#define STL_PARSER_VALUE_TEST____PREDICATE( )   \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_TEST____COMP( )        \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * value_predicate
 */

#define STL_PARSER_VALUE_PREDICATE____PREDICATE( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_VALUE_PREDICATE____BETWEEN( )        \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_VALUE_PREDICATE____LIKE( )   \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_PREDICATE____IN( )     \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_PREDICATE____EXISTS( ) \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_PREDICATE____QUANTIFIED_COMP( )    \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }


/**
 * value_predicate_2
 */

#define STL_PARSER_VALUE_PREDICATE_2____PREDICATE( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_VALUE_PREDICATE_2____CONCAT( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }


/**
 * value_predicate_3
 */

#define STL_PARSER_VALUE_PREDICATE_3____PREDICATE( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_VALUE_PREDICATE_3____IS_TRUTH( )                                             \
    {                                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                          \
                                                                                                \
        if( ( ((qllNode *)(v3))->mType == QLL_BNF_CONSTANT_TYPE ) &&                            \
            ( ((qlpConstantValue *)(v3))->mValue->mType == QLL_BNF_NULL_CONSTANT_TYPE ) )       \
        {                                                                                       \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v1 ) );                                     \
                                                                                                \
            STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                      \
                                                    KNL_BUILTIN_FUNC_IS_UNKNOWN,                \
                                                    a1,                                         \
                                                    QLP_GET_LAST_POS( a3 ) - a1 ,               \
                                                    sList ) );                                  \
        }                                                                                       \
        else {                                                                                  \
                                                                                                \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v1 ) );                                     \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v3 ) );                                     \
                                                                                                \
            STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                      \
                                                    KNL_BUILTIN_FUNC_IS,                        \
                                                    a1,                                         \
                                                    QLP_GET_LAST_POS( a3 ) - a1 ,               \
                                                    sList ) );                                  \
        }                                                                                       \
    }

#define STL_PARSER_VALUE_PREDICATE_3____IS_NOT_TRUTH( )                                         \
    {                                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                          \
                                                                                                \
        if( ( ((qllNode *)(v4))->mType == QLL_BNF_CONSTANT_TYPE ) &&                            \
            ( ((qlpConstantValue *)(v4))->mValue->mType == QLL_BNF_NULL_CONSTANT_TYPE ) )       \
        {                                                                                       \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v1 ) );                                     \
                                                                                                \
            STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                      \
                                                    KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN,            \
                                                    a1,                                         \
                                                    QLP_GET_LAST_POS( a4 ) - a1 ,               \
                                                    sList ) );                                  \
        }                                                                                       \
        else {                                                                                  \
                                                                                                \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v1 ) );                                     \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v4 ) );                                     \
                                                                                                \
            STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                      \
                                                    KNL_BUILTIN_FUNC_IS_NOT,                    \
                                                    a1,                                         \
                                                    QLP_GET_LAST_POS( a4 ) - a1 ,               \
                                                    sList ) );                                  \
        }                                                                                       \
    }

#define STL_PARSER_VALUE_PREDICATE_3____IS_NULL( )                              \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_IS_NULL,       \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a3 ) - a1 ,   \
                                                sList ) );                      \
    }

#define STL_PARSER_VALUE_PREDICATE_3____IS_NOT_NULL( )                          \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_IS_NOT_NULL,   \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a4 ) - a1 ,   \
                                                sList ) );                      \
    }


/**
 * value_predicate_4
 */

#define STL_PARSER_VALUE_PREDICATE_4____TERM( ) \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_PREDICATE_4____ADD( )                                  \
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
                                                KNL_BUILTIN_FUNC_ADD,           \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a3 ) - a1,    \
                                                sList ) );                      \
    }

#define STL_PARSER_VALUE_PREDICATE_4____SUB( )                                  \
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
                                                KNL_BUILTIN_FUNC_SUB,           \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a3 ) - a1,    \
                                                sList ) );                      \
    }


/**
 * value_term
 */

#define STL_PARSER_VALUE_TERM____FACTOR( )      \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_TERM____MULTIPLE( )                                    \
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
                                                KNL_BUILTIN_FUNC_MUL,           \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a3 ) - a1,    \
                                                sList ) );                      \
    }

#define STL_PARSER_VALUE_TERM____DIVIDE( )                                      \
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
 * sign
 */

#define STL_PARSER_SIGN____PLUS( )              \
    {                                           \
        STL_PARSER_CHECK( v0 = "+" );           \
    }


#define STL_PARSER_SIGN____MINUS( )             \
    {                                           \
        STL_PARSER_CHECK( v0 = "-" );           \
    }


/**
 * value_factor
 */

#define STL_PARSER_VALUE_FACTOR____FACTOR( )    \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_FACTOR____POSITIVE( )                                  \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v2 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_POSITIVE,      \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a2 ) - a1,    \
                                                sList ) );                      \
    }

#define STL_PARSER_VALUE_FACTOR____NEGATIVE( )                                  \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v2 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_NEGATIVE,      \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a2 ) - a1,    \
                                                sList ) );                      \
    }


/**
 * value_factor_2
 */

#define STL_PARSER_VALUE_FACTOR_2____FACTOR( )  \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_FACTOR_2____TIME_ZONE( )   \
    {                                               \
        STL_PARSER_NOT_SUPPORT( a2 );               \
    }


/**
 * value_factor_3
 */

#define STL_PARSER_VALUE_FACTOR_3____NON_PAREN_VALUE( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_VALUE_FACTOR_3____PAREN_VALUE( )     \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_VALUE_FACTOR_3____CAST( )    \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_VALUE_FACTOR_3____NUMERIC_FUNCTION( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_VALUE_FACTOR_3____STRING_FUNCTION( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_VALUE_FACTOR_3____DATETIME_FUNCTION( )       \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_VALUE_FACTOR_3____NVL_FUNCTION( )            \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_VALUE_FACTOR_3____DUMP_FUNCTION( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_VALUE_FACTOR_3____DECODE_FUNCTION( )   \
    {                                                     \
        STL_PARSER_CHECK( v0 = v1 );                      \
    }

#define STL_PARSER_VALUE_FACTOR_3____CASE2_FUNCTION( )   \
    {                                                     \
        STL_PARSER_CHECK( v0 = v1 );                      \
    }

#define STL_PARSER_VALUE_FACTOR_3____GENERAL_COMPARISON_FUNCTION( )   \
    {                                                                 \
        STL_PARSER_CHECK( v0 = v1 );                                  \
    }


/**
 * nonparenthesized_value_expression_primary
 */

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____UNSIGNED_VALUE( )       \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____SEQUENCE_FUNCTION( )    \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____SESSION_FUNCTION( )     \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____AGGREGATE_FUNCTION( )   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____CASE_EXPRESSION( )      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                                    \
    }


/** @} qlpParser */


#endif /* _QLPPARSERMACROPART_2_H_ */
