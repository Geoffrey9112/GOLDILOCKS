/*******************************************************************************
 * eldtSQL_SIZING.c
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
 * @file eldtSQL_SIZING.c
 * @brief SQL_SIZING dictionary table 
 */

#include <ell.h>

#include <eldt.h>

/**
 * @addtogroup eldtSQL_SIZING
 * @internal
 * @{
 */

eldtSqlSizingRecord gEldtSqlSizingRecord[] =
{
    {
        34,
        "MAXIMUM CATALOG NAME LENGTH",
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        "Length in characters"            
    },
    {
        30,
        "MAXIMUM COLUMN NAME LENGTH",
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        "Length in characters"            
    },
    {
        97,
        "MAXIMUM COLUMNS IN GROUP BY",
        32768,
        NULL
    },
    {
        99,
        "MAXIMUM COLUMNS IN ORDER BY",
        32768,
        NULL
    },
    {
        100,
        "MAXIMUM COLUMNS IN SELECT",
        ELDT_SqlSizing_SUPPORTED_VALUE_UNLIMITED,
        "Max number of expressions in <select list>"
    },
    {
        101,
        "MAXIMUM COLUMNS IN TABLE",
        ELDT_SqlSizing_SUPPORTED_VALUE_UNLIMITED,
        NULL
    },
    {
        1,
        "MAXIMUM CONCURRENT ACTIVITIES",
        ELDT_SqlSizing_SUPPORTED_VALUE_UNLIMITED,
        "Max number of SQL-statements currently active"        
    },
    {
        31,
        "MAXIMUM CURSOR NAME LENGTH",
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        "Length in characters"
    },
    {
        0,
        "MAXIMUM DRIVER CONNECTIONS",
        ELDT_SqlSizing_SUPPORTED_VALUE_UNLIMITED,
        "Max number of SQL-connections currently established"
    },
    {
        10005,
        "MAXIMUM IDENTIFIER LENGTH",
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        "Length in characters;\n"
        "If different for some objects, set to smallest max"
    },
    {
        32,
        "MAXIMUM SCHEMA NAME LENGTH",
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        "Length in characters"
    },
    {
        20000,
        "MAXIMUM STATEMENT OCTETS",
        ELDT_SqlSizing_SUPPORTED_VALUE_UNLIMITED,
        "Max length in octets of <SQL statement variable>"
    },
    {
        20001,
        "MAXIMUM STATEMENT OCTETS DATA",
        ELDT_SqlSizing_SUPPORTED_VALUE_UNLIMITED,
        "Max length in octets of <SQL data statement>"
    },
    {
        20002,
        "MAXIMUM STATEMENT OCTETS SCHEMA",
        ELDT_SqlSizing_SUPPORTED_VALUE_UNLIMITED,
        "Max length in octets of <schema definition>"
    },
    {
        35,
        "MAXIMUM TABLE NAME LENGTH",
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        "Max length in chars of low order table name part"
    },
    {
        106,
        "MAXIMUM TABLES IN SELECT",
        ELDT_SqlSizing_SUPPORTED_VALUE_UNLIMITED,
        "Max number of table names in FROM clause"
    },
    {
        107,
        "MAXIMUM USER NAME LENGTH",
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        "Length in characters for a <user identifier> of an SQL-session"
    },
    {
        25000,
        "MAXIMUM CURRENT DEFAULT TRANSFORM GROUP LENGTH",
        ELDT_SqlSizing_SUPPORTED_VALUE_NA,
        "Length in characters for a <user identifier> of an SQL-session"
    },
    {
        25001,
        "MAXIMUM CURRENT TRANSFORM GROUP LENGTH",
        ELDT_SqlSizing_SUPPORTED_VALUE_NA,
        "Length in characters"
    },
    {
        25002,
        "MAXIMUM CURRENT PATH LENGTH",
        ELDT_SqlSizing_SUPPORTED_VALUE_NA,
        "Length in characters"
    },
    {
        25003,
        "MAXIMUM CURRENT ROLE LENGTH",
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        "Length in characters"
    },
    {
        25004,
        "MAXIMUM SESSION USER LENGTH",
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        "Length in characters"
    },
    {
        25005,
        "MAXIMUM SYSTEM USER LENGTH",
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        "Length in characters"
    },

    
    {
        STL_INT32_MIN,
        NULL,
        ELDT_SqlSizing_SUPPORTED_VALUE_NA,
        NULL
    }
};

/**
 * @brief DEFINITION_SCHEMA.SQL_SIZING 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescSQL_SIZING[ELDT_SqlSizing_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_SQL_SIZING,                   /**< Table ID */
        "SIZING_ID",                                /**< 컬럼의 이름  */
        ELDT_SqlSizing_ColumnOrder_SIZING_ID,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "identifier of the sizing item"
    },
    {
        ELDT_TABLE_ID_SQL_SIZING,                   /**< Table ID */
        "SIZING_NAME",                              /**< 컬럼의 이름  */
        ELDT_SqlSizing_ColumnOrder_SIZING_NAME,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                       /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "descriptive name of the sizing item"        
    },
    {
        ELDT_TABLE_ID_SQL_SIZING,                   /**< Table ID */
        "SUPPORTED_VALUE",                          /**< 컬럼의 이름  */
        ELDT_SqlSizing_ColumnOrder_SUPPORTED_VALUE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                          /**< 컬럼의 Nullable 여부 */
        "value of the sizing item, or 0 if the size is unlimited or cannot be determined, "
        "or null if the features for which the sizing item is applicable are not supported"
    },
    {
        ELDT_TABLE_ID_SQL_SIZING,                   /**< Table ID */
        "COMMENTS",                                 /**< 컬럼의 이름  */
        ELDT_SqlSizing_ColumnOrder_COMMENTS,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                      /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                          /**< 컬럼의 Nullable 여부 */
        "possibly a comment pertaining to the sizing item"
    }
};


/**
 * @brief DEFINITION_SCHEMA.SQL_SIZING 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescSQL_SIZING =
{
    ELDT_TABLE_ID_SQL_SIZING, /**< Table ID */
    "SQL_SIZING",             /**< 테이블의 이름  */
    "The SQL_SIZING base table has one row for each sizing item defined in ISO/IEC 9075."
};

/**
 * @brief DEFINITION_SCHEMA.SQL_SIZING 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescSQL_SIZING[ELDT_SqlSizing_Const_MAX] =
{
    {
        ELDT_TABLE_ID_SQL_SIZING,                /**< Table ID */
        ELDT_SqlSizing_Const_PRIMARY_KEY,        /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,   /**< Table Constraint 유형 */
        1,                                       /**< 키를 구성하는 컬럼의 개수 */
        {                                  /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_SqlSizing_ColumnOrder_SIZING_ID
        },
        ELL_DICT_OBJECT_ID_NA,                   /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                   /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_SQL_SIZING,                /**< Table ID */
        ELDT_SqlSizing_Const_UNIQUE_SIZING_NAME, /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,    /**< Table Constraint 유형 */
        1,                                       /**< 키를 구성하는 컬럼의 개수 */
        {                                  /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_SqlSizing_ColumnOrder_SIZING_NAME
        },
        ELL_DICT_OBJECT_ID_NA,                   /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                   /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/**
 * @brief sizing item 명세를 추가한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertSqlSizingDesc( smlTransId     aTransID,
                                   smlStatement * aStmt,
                                   ellEnv       * aEnv )
{
    eldtSqlSizingRecord * sRecord = NULL;
    stlInt32              i       = -1;

    while( STL_TRUE )
    {
        i++;

        sRecord = &gEldtSqlSizingRecord[i];
        
        if( sRecord->mName == NULL )
        {
            break;
        }

        STL_TRY( ellInsertSqlSizingDesc( aTransID,
                                         aStmt,
                                         sRecord->mID,
                                         sRecord->mName,
                                         sRecord->mSupportedValue,
                                         sRecord->mComments,
                                         aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} eldtSQL_SIZING */
