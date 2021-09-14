/*******************************************************************************
 * eldtINDEX_KEY_COLUMN_USAGE.c
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
 * @file eldtINDEX_KEY_COLUMN_USAGE.c
 * @brief DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtINDEX_KEY_COLUMN_USAGE
 * @{
 */


    
/**
 * @brief DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescINDEX_KEY_COLUMN_USAGE[ELDT_IndexKey_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,          /**< Table ID */
        "INDEX_OWNER_ID",                              /**< 컬럼의 이름  */
        ELDT_IndexKey_ColumnOrder_INDEX_OWNER_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the index"        
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,          /**< Table ID */
        "INDEX_SCHEMA_ID",                             /**< 컬럼의 이름  */
        ELDT_IndexKey_ColumnOrder_INDEX_SCHEMA_ID,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "schema identifier of the index"        
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,          /**< Table ID */
        "INDEX_ID",                                    /**< 컬럼의 이름  */
        ELDT_IndexKey_ColumnOrder_INDEX_ID,            /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "index identifier"
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,          /**< Table ID */
        "TABLE_OWNER_ID",                              /**< 컬럼의 이름  */
        ELDT_IndexKey_ColumnOrder_OWNER_ID,            /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the table of the column "
        "that participates in the index"
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,          /**< Table ID */
        "TABLE_SCHEMA_ID",                             /**< 컬럼의 이름  */
        ELDT_IndexKey_ColumnOrder_SCHEMA_ID,           /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "schema identifier of the column that participates in the index"
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,          /**< Table ID */
        "TABLE_ID",                                    /**< 컬럼의 이름  */
        ELDT_IndexKey_ColumnOrder_TABLE_ID,            /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "table identifier of the column that participates in the index"
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,          /**< Table ID */
        "COLUMN_ID",                                   /**< 컬럼의 이름  */
        ELDT_IndexKey_ColumnOrder_COLUMN_ID,           /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "column identifier of the column that participates in the index"
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,          /**< Table ID */
        "ORDINAL_POSITION",                            /**< 컬럼의 이름  */
        ELDT_IndexKey_ColumnOrder_ORDINAL_POSITION,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "the ordinal position of the specific column in the index described. "
        "If the index described is a key of cardinality 1 (one), "
        "then the value of ORDINAL_POSITION is always 1 (one)."
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,          /**< Table ID */
        "IS_ASCENDING_ORDER",                          /**< 컬럼의 이름  */
        ELDT_IndexKey_ColumnOrder_IS_ASCENDING_ORDER,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "The values of IS_ASCENDING_ORDER have the following meanings:\n"
        "- TRUE : The index key column being described is sorted in ASCENDING order\n"
        "- FALSE : The index key column being described is sorted in DESCENDING order\n"
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,          /**< Table ID */
        "IS_NULLS_FIRST",                              /**< 컬럼의 이름  */
        ELDT_IndexKey_ColumnOrder_IS_NULLS_FIRST,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "The values of IS_NULLS_FIRST have the following meanings:\n"
        "- TRUE : The null values of the key column are sorted before non-null values\n"
        "- FALSE : The null values of the key column are sorted after non-null values\n"
    }
};


/**
 * @brief DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescINDEX_KEY_COLUMN_USAGE =
{
    ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,                  /**< Table ID */
    "INDEX_KEY_COLUMN_USAGE",                              /**< 테이블의 이름  */
    "The INDEX_KEY_COLUMN_USAGE table has one or more rows for each row "
    "in the INDEXES table. " 
    "The rows list the columns that constitute each index. "
    "This table is not declared in SQL standard."
};


/**
 * @brief DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescINDEX_KEY_COLUMN_USAGE[ELDT_IndexKey_Const_MAX] =
{
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,        /**< Table ID */
        ELDT_IndexKey_Const_PRIMARY_KEY,             /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        5,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexKey_ColumnOrder_INDEX_SCHEMA_ID,
            ELDT_IndexKey_ColumnOrder_INDEX_ID,
            ELDT_IndexKey_ColumnOrder_SCHEMA_ID,
            ELDT_IndexKey_ColumnOrder_TABLE_ID,
            ELDT_IndexKey_ColumnOrder_COLUMN_ID,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,        /**< Table ID */
        ELDT_IndexKey_Const_UNIQUE_XXXX_ORDINAL_POSITION, /**< Const 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        3,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexKey_ColumnOrder_INDEX_SCHEMA_ID,
            ELDT_IndexKey_ColumnOrder_INDEX_ID,
            ELDT_IndexKey_ColumnOrder_ORDINAL_POSITION,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,        /**< Table ID */
        ELDT_IndexKey_Const_FOREIGN_KEY_COLUMNS,     /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,       /**< Table Constraint 유형 */
        3,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexKey_ColumnOrder_SCHEMA_ID,
            ELDT_IndexKey_ColumnOrder_TABLE_ID,
            ELDT_IndexKey_ColumnOrder_COLUMN_ID,
        },
        ELDT_TABLE_ID_COLUMNS,                       /**< 참조 제약이 참조하는 Table ID */
        ELDT_Columns_Const_PRIMARY_KEY,              /**< 참조 제약이 참조하는 Unique의 번호 */
    }
    
};




/**
 * @brief DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescINDEX_KEY_COLUMN_USAGE[ELDT_IndexKey_Index_MAX] =
{
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,        /**< Table ID */
        ELDT_IndexKey_Index_INDEX_OWNER_ID,          /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexKey_ColumnOrder_INDEX_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,        /**< Table ID */
        ELDT_IndexKey_Index_INDEX_SCHEMA_ID,         /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexKey_ColumnOrder_INDEX_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,        /**< Table ID */
        ELDT_IndexKey_Index_INDEX_ID,                /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexKey_ColumnOrder_INDEX_ID,
        }
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,        /**< Table ID */
        ELDT_IndexKey_Index_OWNER_ID,                /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexKey_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,        /**< Table ID */
        ELDT_IndexKey_Index_SCHEMA_ID,               /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexKey_ColumnOrder_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,        /**< Table ID */
        ELDT_IndexKey_Index_TABLE_ID,                /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexKey_ColumnOrder_TABLE_ID,
        }
    },
    {
        ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,        /**< Table ID */
        ELDT_IndexKey_Index_COLUMN_ID,               /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_IndexKey_ColumnOrder_COLUMN_ID,
        }
    }
};



/** @} eldtINDEX_KEY_COLUMN_USAGE */

