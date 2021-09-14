/*******************************************************************************
 * qlneCommon.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlneCommon.c
 * @brief SQL Processor Layer Explain Node Common Functions
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlne
 * @{
 */

qlneExplainNode qlneExplainNodeList[ QLL_PLAN_NODE_TYPE_MAX ] =
{
    qlneExplainSelectStmt,    /* QLL_PLAN_NODE_STMT_SELECT_TYPE */
    qlneExplainInsertStmt,    /* QLL_PLAN_NODE_STMT_INSERT_TYPE */
    qlneExplainUpdateStmt,    /* QLL_PLAN_NODE_STMT_UPDATE_TYPE */
    qlneExplainDeleteStmt,    /* QLL_PLAN_NODE_STMT_DELETE_TYPE */
    NULL,    /* QLL_PLAN_NODE_STMT_MERGE_TYPE */

    qlneExplainQuerySpec,    /* QLL_PLAN_NODE_QUERY_SPEC_TYPE */
    NULL,    /* QLL_PLAN_NODE_INDEX_BUILD_TYPE */
    NULL,    /* QLL_PLAN_NODE_FOR_UPDATE_TYPE */
    NULL,    /* QLL_PLAN_NODE_MERGE_TYPE */
    NULL,    /* QLL_PLAN_NODE_VIEW_TYPE */
    NULL,    /* QLL_PLAN_NODE_VIEW_PUSHED_PRED_TYPE */
    NULL,    /* QLL_PLAN_NODE_FILTER_TYPE */
    NULL,    /* QLL_PLAN_NODE_INLIST_TYPE */

    qlneExplainQuerySet,    /* QLL_PLAN_NODE_QUERY_SET_TYPE */
    qlneExplainSetOP,       /* QLL_PLAN_NODE_UNION_ALL_TYPE */
    qlneExplainSetOP,       /* QLL_PLAN_NODE_UNION_DISTINCT_TYPE */
    qlneExplainSetOP,       /* QLL_PLAN_NODE_EXCEPT_ALL_TYPE */
    qlneExplainSetOP,       /* QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE */
    qlneExplainSetOP,       /* QLL_PLAN_NODE_INTERSECT_ALL_TYPE */
    qlneExplainSetOP,       /* QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE */

    qlneExplainSubQuery,           /* QLL_PLAN_NODE_SUB_QUERY_TYPE */
    qlneExplainSubQueryList,       /* QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE */
    qlneExplainSubQueryFunc,       /* QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE */
    qlneExplainSubQueryFilter,     /* QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE */

    qlneExplainConcat,             /* QLL_PLAN_NODE_CONCAT_TYPE */

    qlneExplainNestedLoopsJoin,    /* QLL_PLAN_NODE_NESTED_LOOPS_TYPE */
    qlneExplainSortMergeJoin,      /* QLL_PLAN_NODE_MERGE_JOIN_TYPE */
    qlneExplainHashJoin,           /* QLL_PLAN_NODE_HASH_JOIN_TYPE */
    NULL,    /* QLL_PLAN_NODE_INDEX_JOIN_TYPE */
    
    NULL,    /* QLL_PLAN_NODE_CONNECT_BY_TYPE */
    NULL,    /* QLL_PLAN_NODE_PSEUDO_COLUMN_TYPE */
    NULL,    /* QLL_PLAN_NODE_COUNT_TYPE */
    NULL,    /* QLL_PLAN_NODE_WINDOW_TYPE */
    NULL,    /* QLL_PLAN_NODE_HASH_TYPE */
    qlneExplainHashAggregation,    /* QLL_PLAN_NODE_HASH_AGGREGATION_TYPE */
    qlneExplainGroup,              /* QLL_PLAN_NODE_GROUP_TYPE */

    qlneExplainTableAccess,        /* QLL_PLAN_NODE_TABLE_ACCESS_TYPE */
    qlneExplainIndexAccess,        /* QLL_PLAN_NODE_INDEX_ACCESS_TYPE */
    qlneExplainRowIdAccess,        /* QLL_PLAN_NODE_ROWID_ACCESS_TYPE */
    NULL,     /* QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE */
    NULL,  /* QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE */
    qlneExplainSortInstantAccess,  /* QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE */
    NULL,  /* QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE */
    qlneExplainGroupHashInstantAccess, /* QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE */
    NULL, /* QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE */
    qlneExplainSortJoinInstantAccess,  /* QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE */
    qlneExplainHashJoinInstantAccess   /* QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE */
};


const stlChar qlneBuiltInFuncName[ KNL_BUILTIN_FUNC_MAX ][ QLNE_BUILT_IN_FUNC_NAME_MAX_SIZE ] = 
{
    /**
     * default operation
     */
    
    "EMPTY",                      /* KNL_BUILTIN_FUNC_INVALID */
    "ASSIGN",                     /* KNL_BUILTIN_FUNC_ASSIGN */
    "RETURN EMPTY",               /* KNL_BUILTIN_FUNC_RETURN_EMPTY */
    "RETURN NULL",                /* KNL_BUILTIN_FUNC_RETURN_NULL */
    "RETURN PASS",                /* KNL_BUILTIN_FUNC_RETURN_PASS */
    
    /**
     * comparision operations
     */
    
    "=",                          /* KNL_BUILTIN_FUNC_IS_EQUAL */
    "<>",                         /* KNL_BUILTIN_FUNC_IS_NOT_EQUAL */
    "<",                          /* KNL_BUILTIN_FUNC_IS_LESS_THAN */
    "<=",                         /* KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL */
    ">",                          /* KNL_BUILTIN_FUNC_IS_GREATER_THAN */
    ">=",                         /* KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL */
    "IS NULL",                    /* KNL_BUILTIN_FUNC_IS_NULL */
    "IS NOT NULL",                /* KNL_BUILTIN_FUNC_IS_NOT_NULL */

    
    /**
     * logical operations
     */

    "AND",                        /* KNL_BUILTIN_FUNC_AND */
    "OR",                         /* KNL_BUILTIN_FUNC_OR */
    "NOT",                        /* KNL_BUILTIN_FUNC_NOT */
    "IS",                         /* KNL_BUILTIN_FUNC_IS */
    "IS NOT",                     /* KNL_BUILTIN_FUNC_IS_NOT */
    "IS UNKNOWN",                 /* KNL_BUILTIN_FUNC_IS_UNKNOWN */
    "IS NOT UNKNOWN",             /* KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN */

    
    /**
     * between
     */

    "BETWEEN",                    /* KNL_BUILTIN_FUNC_BETWEEN */
    "NOT BETWEEN",                /* KNL_BUILTIN_FUNC_NOT_BETWEEN */
    "BETWEEN SYMMETRIC",          /* KNL_BUILTIN_FUNC_BETWEEN_SYMMETRIC */
    "NOT BETWEEN SYMMETRIC",      /* KNL_BUILTIN_FUNC_NOT_BETWEEN_SYMMETRIC */

    
    /**
     * like
     */

    "LIKE",                        /* KNL_BUILTIN_FUNC_LIKE */
    "NOT LIKE",                    /* KNL_BUILTIN_FUNC_NOT_LIKE */
    "",                            /* KNL_BUILTIN_FUNC_LIKE_PATTERN */
    
    /**
     * cast
     */
    
    "CAST",                       /* KNL_BUILTIN_FUNC_CAST */

    
    /**
     * Mathematical ( with Arithmetic )
     */
    
    "+",                          /* KNL_BUILTIN_FUNC_ADD */
    "-",                          /* KNL_BUILTIN_FUNC_SUB */
    "*",                          /* KNL_BUILTIN_FUNC_MUL */
    "/",                          /* KNL_BUILTIN_FUNC_DIV */
    "MOD",                        /* KNL_BUILTIN_FUNC_MOD */
    "",                           /* KNL_BUILTIN_FUNC_POSITIVE */
    "-",                          /* KNL_BUILTIN_FUNC_NEGATIVE */
    "POWER",                      /* KNL_BUILTIN_FUNC_POWER */
    "SQRT",                       /* KNL_BUILTIN_FUNC_SQRT */
    "CBRT",                       /* KNL_BUILTIN_FUNC_CBRT */
    "FACTORIAL",                  /* KNL_BUILTIN_FUNC_FACTORIAL */
    "ABS",                        /* KNL_BUILTIN_FUNC_ABS */
    "CEIL",                       /* KNL_BUILTIN_FUNC_CEIL */
    "DEGREES",                    /* KNL_BUILTIN_FUNC_DEGREES */
    "EXP",                        /* KNL_BUILTIN_FUNC_EXP */
    "FLOOR",                      /* KNL_BUILTIN_FUNC_FLOOR */
    "LN",                         /* KNL_BUILTIN_FUNC_LN */
    "LOG10",                      /* KNL_BUILTIN_FUNC_LOG10 */
    "LOG",                        /* KNL_BUILTIN_FUNC_LOG */ 
    "PI",                         /* KNL_BUILTIN_FUNC_PI */
    "RADIANS",                    /* KNL_BUILTIN_FUNC_RADIANS */
    "RANDOM",                     /* KNL_BUILTIN_FUNC_RANDOM */ 
    "ROUND",                      /* KNL_BUILTIN_FUNC_ROUND */ 
    "SIGN",                       /* KNL_BUILTIN_FUNC_SIGN */
    "TRUNC",                      /* KNL_BUILTIN_FUNC_TRUNC */
    "WIDTH_BUCKET",               /* KNL_BUILTIN_FUNC_WIDTHBUCKET */
    "ACOS",                       /* KNL_BUILTIN_FUNC_ACOS */ 
    "ASIN",                       /* KNL_BUILTIN_FUNC_ASIN */
    "ATAN",                       /* KNL_BUILTIN_FUNC_ATAN */
    "ATAN2",                      /* KNL_BUILTIN_FUNC_ATAN2 */
    "COS",                        /* KNL_BUILTIN_FUNC_COS */
    "COT",                        /* KNL_BUILTIN_FUNC_COT */ 
    "SIN",                        /* KNL_BUILTIN_FUNC_SIN */
    "TAN",                        /* KNL_BUILTIN_FUNC_TAN */


    /**
     * Bitwise
     */
    
    "BITAND",                     /* KNL_BUILTIN_FUNC_BITWISE_AND */
    "BITOR",                      /* KNL_BUILTIN_FUNC_BITWISE_OR */
    "BITXOR",                     /* KNL_BUILTIN_FUNC_BITWISE_XOR */
    "BITNOT",                     /* KNL_BUILTIN_FUNC_BITWISE_NOT */ 
    "SHIFT_LEFT",                 /* KNL_BUILTIN_FUNC_BITWISE_SHIFTLEFT */
    "SHIFT_RIGHT",                /* KNL_BUILTIN_FUNC_BITWISE_SHIFTRIGHT */

    
    /**
     * String
     */
    
    "||",                         /* KNL_BUILTIN_FUNC_CONCATENATE */
    "BIT_LENGTH",                 /* KNL_BUILTIN_FUNC_BITLENGTH */
    "CHAR_LENGTH",                /* KNL_BUILTIN_FUNC_CHARLENGTH */
    "LOWER",                      /* KNL_BUILTIN_FUNC_LOWER */
    "OCTET_LENGTH",               /* KNL_BUILTIN_FUNC_OCTETLENGTH */
    "OVERLAY",                    /* KNL_BUILTIN_FUNC_OVERLAY */ 
    "POSITION",                   /* KNL_BUILTIN_FUNC_POSITION */
    "INSTR",                      /* KNL_BUILTIN_FUNC_INSTR */
    "SUBSTRING",                  /* KNL_BUILTIN_FUNC_SUBSTRING */
    "SUBSTRB",                    /* KNL_BUILTIN_FUNC_SUBSTRB */
    "TRIM",                       /* KNL_BUILTIN_FUNC_TRIM */
    "LTRIM",                      /* KNL_BUILTIN_FUNC_LTRIM */
    "RTRIM",                      /* KNL_BUILTIN_FUNC_RTRIM */    
    "UPPER",                      /* KNL_BUILTIN_FUNC_UPPER */
    "INITCAP",                    /* KNL_BUILTIN_FUNC_INITCAP */
    "LPAD",                       /* KNL_BUILTIN_FUNC_LPAD */
    "RPAD",                       /* KNL_BUILTIN_FUNC_RPAD */  
    "REPEAT",                     /* KNL_BUILTIN_FUNC_REPEAT */
    "REPLACE",                    /* KNL_BUILTIN_FUNC_REPLACE */
    "SPLIT_PART",                 /* KNL_BUILTIN_FUNC_SPLIT_PART */ 
    "TRANSLATE",                  /* KNL_BUILTIN_FUNC_TRANSLATE, */
    "CHR",                        /* KNL_BUILTIN_FUNC_CHR, */    

    
    /**
     * Date & Time
     */

    "ADDDATE",                    /* KNL_BUILTIN_FUNC_ADDDATE */
    "ADDTIME",                    /* KNL_BUILTIN_FUNC_ADDTIME */
    "DATEADD",                    /* KNL_BUILTIN_FUNC_DATEADD */
    "DATE_ADD",                   /* KNL_BUILTIN_FUNC_DATE_ADD */    
    "EXTRACT",                    /* KNL_BUILTIN_FUNC_EXTRACT */
    "DATE_PART",                  /* KNL_BUILTIN_FUNC_DATE_PART */
    "LAST_DAY",                   /* KNL_BUILTIN_FUNC_LAST_DAY */
    "ADD_MONTHS",                 /* KNL_BUILTIN_FUNC_ADD_MONTHS */
    "FROM_UNIXTIME",              /* KNL_BUILTIN_FUNC_FROM_UNIXTIME */
    "DATEDIFF",                   /* KNL_BUILTIN_FUNC_DATEDIFF */        

    
    /**
     * ROWID
     */   

    "ROWID_OBEJCT_ID",            /* KNL_BUILTIN_FUNC_ROWID_OBJECT_ID */
    "ROWID_PAGE_ID",              /* KNL_BUILTIN_FUNC_ROWID_PAGE_ID */ 
    "ROWID_ROW_NUMBER",           /* KNL_BUILTIN_FUNC_ROWID_ROW_NUMBER */
    "ROWID_TABLESPACE_ID",        /* KNL_BUILTIN_FUNC_ROWID_TABLESPACE_ID */

    /**
     * CASE EXPRESSION
     */   

    "CASE",                       /* KNL_BUILTIN_FUNC_CASE */
    "NULLIF",                     /* KNL_BUILTIN_FUNC_NULLIF */
    "COALESCE",                   /* KNL_BUILTIN_FUNC_COALESCE */

    "DECODE",                     /* KNL_BUILTIN_FUNC_DECODE */
    "CASE2",                      /* KNL_BUILTIN_FUNC_CASE2 */    

    /**
     * NVL
     */

    "NVL",                       /* KNL_BUILTIN_FUNC_NVL */
    "NVL2",                      /* KNL_BUILTIN_FUNC_NVL2 */

    /**
     * TO_CHAR
     */

    "TO_CHAR",                   /* KNL_BUILTIN_FUNC_TO_CHAR */
    "",                          /* KNL_BUILTIN_FUNC_TO_CHAR_FORMAT */

    /**
     * TO_DATETIME
     */

    "TO_DATE",                     /* KNL_BUILTIN_FUNC_TO_DATE */
    "TO_TIME",                     /* KNL_BUILTIN_FUNC_TO_TIME */
    "TO_TIME_WITH_TIME_ZONE",      /* KNL_BUILTIN_FUNC_TO_TIME_WITH_TIME_ZONE */
    "TO_TIMESTAMP",                /* KNL_BUILTIN_FUNC_TO_TIMESTAMP */
    "TO_TIMESTAMP_WITH_TIME_ZONE", /* KNL_BUILTIN_FUNC_TO_TIMESTAMP_WITH_TIME_ZONE */
    "",                            /* KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT */

    /**
     * TO_NUMBER
     */

    "TO_NUMBER",                   /* KNL_BUILTIN_FUNC_TO_NUMBER */
    "",                            /* KNL_BUILTIN_FUNC_TO_NUMBER_FORMAT */

    /**
     * TO_NATIVE_REAL
     */
    
    "TO_NATIVE_REAL",              /* KNL_BUILTIN_FUNC_TO_NATIVE_REAL */

    /**
     * TO_NATIVE_DOUBLE
     */
    
    "TO_NATIVE_DOUBLE",            /* KNL_BUILTIN_FUNC_TO_NATIVE_DOUBLE */

    /**
     * general_comparision_function
     */
    
    "GREATEST",                    /* KNL_BUILTIN_FUNC_GREATEST */
    "LEAST",                       /* KNL_BUILTIN_FUNC_LEAST */
    
    
    /**
     * LIST_OPERATION
     */
    
    "=",                          /* KNL_BUILTIN_FUNC_IS_EQUAL_ROW */
    "<>",                         /* KNL_BUILTIN_FUNC_IS_NOT_EQUAL_ROW */
    "<",                          /* KNL_BUILTIN_FUNC_IS_LESS_THAN_ROW */
    "<=",                         /* KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL_ROW */
    ">",                          /* KNL_BUILTIN_FUNC_IS_GREATER_THAN_ROW */
    ">=",                         /* KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW */

      
    /**
     * IN
     */
    
    "IN",                        /* KNL_BUILTIN_FUNC_IN */
    "NOT IN",                    /* KNL_BUILTIN_FUNC_NOT_IN */
    
    /**
     * EXISTS
     */
    
    "EXISTS",                    /* KNL_BUILTIN_FUNC_EXISTS */
    "NOT EXISTS",                /* KNL_BUILTIN_FUNC_NOT_EXISTS */

    /**
     *  LIST OPERATION ( ANY )
     */
    "= ANY",                     /* KNL_BUILTIN_FUNC_EQUAL_ANY */
    "<> ANY",                    /* KNL_BUILTIN_FUNC_NOT_EQUAL_ANY */
    "< ANY",                     /* KNL_BUILTIN_FUNC_LESS_THAN_ANY */
    "<= ANY",                    /* KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY */
    "> ANY",                     /* KNL_BUILTIN_FUNC_GREATER_THAN_ANY  */
    ">= ANY",                    /* KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY */
    
    /**
     *  LIST OPERATION ( ALL )
     */
    "= ALL",                     /* KNL_BUILTIN_FUNC_EQUAL_ALL */
    "<> ALL",                    /* KNL_BUILTIN_FUNC_NOT_EQUAL_ALL */
    "< ALL",                     /* KNL_BUILTIN_FUNC_LESS_THAN_ALL */
    "<= ALL",                    /* KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL */
    "> ALL",                     /* KNL_BUILTIN_FUNC_GREATER_THAN_ALL  */
    ">= ALL",                    /* KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL */
    
    /**
     * DUMP
     */
    
    "DUMP",                        /* KNL_BUILTIN_FUNC_DUMP */

    /**
     * SYSTEM
     */   

    "CURRENT_CATALOG",            /* KNL_BUILTIN_FUNC_CURRENT_CATALOG */
    "VERSION",                    /* KNL_BUILTIN_FUNC_VERSION */


    /**
     * SESSION
     */
    
    "SESSION_ID",                 /* KNL_BUILTIN_FUNC_SESSION_ID */
    "SESSION_SERIAL",             /* KNL_BUILTIN_FUNC_SESSION_SERIAL */
    "USER_ID",                    /* KNL_BUILTIN_FUNC_USER_ID */
    "CURRENT_USER",               /* KNL_BUILTIN_FUNC_CURRENT_USER */
    "SESSION_USER",               /* KNL_BUILTIN_FUNC_SESSION_USER */
    "LOGON_USER",                 /* KNL_BUILTIN_FUNC_LOGON_USER */
    "CURRENT_SCHEMA",             /* KNL_BUILTIN_FUNC_CURRENT_SCHEMA */


    /**
     * Date & Time for Session
     */
        
    "CLOCK_DATE",                 /* KNL_BUILTIN_FUNC_CLOCK_DATE */
    "STATEMENT_DATE",             /* KNL_BUILTIN_FUNC_STATEMENT_DATE */
    "TRANSACTION_DATE",           /* KNL_BUILTIN_FUNC_TRANSACTION_DATE */
    "CLOCK_TIME",                 /* KNL_BUILTIN_FUNC_CLOCK_TIME */
    "STATEMENT_TIME",             /* KNL_BUILTIN_FUNC_STATEMENT_TIME */
    "TRANSACTION_TIME",           /* KNL_BUILTIN_FUNC_TRANSACTION_TIME */
    "CLOCK_TIMESTAMP",            /* KNL_BUILTIN_FUNC_CLOCK_TIMESTAMP */
    "STATEMENT_TIMESTAMP",        /* KNL_BUILTIN_FUNC_STATEMENT_TIMESTAMP */
    "TRANSACTION_TIMESTAMP",      /* KNL_BUILTIN_FUNC_TRANSACTION_TIMESTAMP */
    "CLOCK_LOCALTIME",            /* KNL_BUILTIN_FUNC_CLOCK_LOCALTIME */
    "STATEMENT_LOCALTIME",        /* KNL_BUILTIN_FUNC_STATEMENT_LOCALTIME */
    "TRANSACTION_LOCALTIME",      /* KNL_BUILTIN_FUNC_TRANSACTION_LOCALTIME */
    "CLOCK_LOCALTIMESTAMP",       /* KNL_BUILTIN_FUNC_CLOCK_LOCALTIMESTAMP */
    "STATEMENT_LOCALTIMESTAMP",   /* KNL_BUILTIN_FUNC_STATEMENT_LOCALTIMESTAMP */
    "TRANSACTION_LOCALTIMESTAMP", /* KNL_BUILTIN_FUNC_TRANSACTION_LOCALTIMESTAMP */
    "SYSDATE",                    /* KNL_BUILTIN_FUNC_SYSDATE */
    "SYSTIME",                    /* KNL_BUILTIN_FUNC_SYSTIME */
    "SYSTIMESTAMP",               /* KNL_BUILTIN_FUNC_SYSTIMESTAMP */    
    

    /**
     * Statement SCN
     */
    
    "STATEMENT_VIEW_SCN"          /* KNL_BUILTIN_FUNC_STATEMENT_VIEW_SCN */
};


const stlChar qlneAggrFuncName[ KNL_BUILTIN_AGGREGATION_UNARY_MAX ][ QLNE_BUILT_IN_FUNC_NAME_MAX_SIZE ] =
{
    "INVALID",       /* KNL_BUILTIN_AGGREGATION_UNARY_MIN */

    "SUM",           /* KNL_BUILTIN_AGGREGATION_SUM */
    "COUNT",         /* KNL_BUILTIN_AGGREGATION_COUNT */
    "COUNT(*)",      /* KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK */
    "MIN",           /* KNL_BUILTIN_AGGREGATION_MIN */
    "MAX"            /* KNL_BUILTIN_AGGREGATION_MAX */
};


const stlChar qlneJoinTypeName[ QLV_JOIN_TYPE_MAX ][ QLNE_BUILT_IN_FUNC_NAME_MAX_SIZE ] =
{
    "CROSS JOIN",               /* QLV_JOIN_TYPE_NONE */
    "CROSS JOIN",               /* QLV_JOIN_TYPE_CROSS */
    "INNER JOIN",               /* QLV_JOIN_TYPE_INNER */
    "LEFT OUTER JOIN",          /* QLV_JOIN_TYPE_LEFT_OUTER */
    "RIGHT OUTER JOIN",         /* QLV_JOIN_TYPE_RIGHT_OUTER */
    "FULL OUTER JOIN",          /* QLV_JOIN_TYPE_FULL_OUTER */
    "LEFT SEMI",                /* QLV_JOIN_TYPE_LEFT_SEMI */
    "RIGHT SEMI",               /* QLV_JOIN_TYPE_RIGHT_SEMI */
    "LEFT ANTI SEMI",           /* QLV_JOIN_TYPE_LEFT_ANTI_SEMI */
    "RIGHT ANTI SEMI",          /* QLV_JOIN_TYPE_RIGHT_ANTI_SEMI */
    "LEFT ANTI SEMI NA",        /* QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA */
    "RIGHT ANTI SEMI NA",       /* QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA */
    "INVERTED LEFT SEMI",       /* QLV_JOIN_TYPE_INVERTED_LEFT_SEMI */
    "INVERTED RIGHT SEMI"       /* QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI */
};


/** @} qlne */
