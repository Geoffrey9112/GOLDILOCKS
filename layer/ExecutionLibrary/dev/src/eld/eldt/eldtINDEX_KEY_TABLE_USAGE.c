/*******************************************************************************
 * eldtINDEX_KEY_TABLE_USAGE.c
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
 * @file eldtINDEX_KEY_TABLE_USAGE.c
 * @brief DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtINDEX_KEY_TABLE_USAGE
 * @{
 */


    
/**
 * @brief DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescINDEX_KEY_TABLE_USAGE[ELDT_IndexTable_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,           /**< Table ID */
        "INDEX_OWNER_ID",                              /**< 컬럼의 이름  */
        ELDT_IndexTable_ColumnOrder_INDEX_OWNER_ID,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the index"        
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,           /**< Table ID */
        "INDEX_SCHEMA_ID",                             /**< 컬럼의 이름  */
        ELDT_IndexTable_ColumnOrder_INDEX_SCHEMA_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "schema identifier of the index"        
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,           /**< Table ID */
        "INDEX_ID",                                    /**< 컬럼의 이름  */
        ELDT_IndexTable_ColumnOrder_INDEX_ID,          /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "index identifier"
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,           /**< Table ID */
        "TABLE_OWNER_ID",                              /**< 컬럼의 이름  */
        ELDT_IndexTable_ColumnOrder_OWNER_ID,          /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the table of the column "
        "that participates in the index"
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,           /**< Table ID */
        "TABLE_SCHEMA_ID",                             /**< 컬럼의 이름  */
        ELDT_IndexTable_ColumnOrder_SCHEMA_ID,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "schema identifier of the column that participates in the index"
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,           /**< Table ID */
        "TABLE_ID",                                    /**< 컬럼의 이름  */
        ELDT_IndexTable_ColumnOrder_TABLE_ID,          /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "table identifier of the column that participates in the index"
    }
};


/**
 * @brief DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescINDEX_KEY_TABLE_USAGE =
{
    ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,                  /**< Table ID */
    "INDEX_KEY_TABLE_USAGE",                              /**< 테이블의 이름  */
    "The INDEX_KEY_TABLE_USAGE table has one or more rows for each row "
    "in the INDEXES table. " 
    "The rows list the tables that constitute each index. "
    "This table is not declared in SQL standard."
};


/**
 * @brief DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescINDEX_KEY_TABLE_USAGE[ELDT_IndexTable_Const_MAX] =
{
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,         /**< Table ID */
        ELDT_IndexTable_Const_PRIMARY_KEY,           /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        4,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexTable_ColumnOrder_INDEX_SCHEMA_ID,
            ELDT_IndexTable_ColumnOrder_INDEX_ID,
            ELDT_IndexTable_ColumnOrder_SCHEMA_ID,
            ELDT_IndexTable_ColumnOrder_TABLE_ID,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,         /**< Table ID */
        ELDT_IndexTable_Const_FOREIGN_KEY_TABLES,    /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,       /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexTable_ColumnOrder_SCHEMA_ID,
            ELDT_IndexTable_ColumnOrder_TABLE_ID,
        },
        ELDT_TABLE_ID_TABLES,                        /**< 참조 제약이 참조하는 Table ID */
        ELDT_Tables_Const_PRIMARY_KEY,               /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};




/**
 * @brief DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescINDEX_KEY_TABLE_USAGE[ELDT_IndexTable_Index_MAX] =
{
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,        /**< Table ID */
        ELDT_IndexTable_Index_INDEX_OWNER_ID,          /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexTable_ColumnOrder_INDEX_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,        /**< Table ID */
        ELDT_IndexTable_Index_INDEX_SCHEMA_ID,         /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexTable_ColumnOrder_INDEX_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,        /**< Table ID */
        ELDT_IndexTable_Index_INDEX_ID,                /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexTable_ColumnOrder_INDEX_ID,
        }
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,        /**< Table ID */
        ELDT_IndexTable_Index_OWNER_ID,                /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexTable_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,        /**< Table ID */
        ELDT_IndexTable_Index_SCHEMA_ID,               /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexTable_ColumnOrder_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,        /**< Table ID */
        ELDT_IndexTable_Index_TABLE_ID,                /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexTable_ColumnOrder_TABLE_ID,
        }
    }
};



/** @} eldtINDEX_KEY_TABLE_USAGE */

