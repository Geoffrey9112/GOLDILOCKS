/*******************************************************************************
 * eldtCHECK_COLUMN_USAGE.c
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
 * @file eldtCHECK_COLUMN_USAGE.c
 * @brief DEFINITION_SCHEMA.CHECK_COLUMN_USAGE 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtCHECK_COLUMN_USAGE
 * @{
 */


    
/**
 * @brief DEFINITION_SCHEMA.CHECK_COLUMN_USAGE 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescCHECK_COLUMN_USAGE[ELDT_CheckColumn_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,              /**< Table ID */
        "CONSTRAINT_OWNER_ID",                         /**< 컬럼의 이름  */
        ELDT_CheckColumn_ColumnOrder_CONSTRAINT_OWNER_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the check constraint"        
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,              /**< Table ID */
        "CONSTRAINT_SCHEMA_ID",                        /**< 컬럼의 이름  */
        ELDT_CheckColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "schema identifier of the check constraint"        
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,              /**< Table ID */
        "CONSTRAINT_ID",                               /**< 컬럼의 이름  */
        ELDT_CheckColumn_ColumnOrder_CONSTRAINT_ID,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "check constraint identifier of the constraint being described"
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,              /**< Table ID */
        "TABLE_OWNER_ID",                              /**< 컬럼의 이름  */
        ELDT_CheckColumn_ColumnOrder_OWNER_ID,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the table of a column identified by a column reference "
        "explicitly or implicitly contained in the search condition "
        "of the constraint being described"
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,              /**< Table ID */
        "TABLE_SCHEMA_ID",                             /**< 컬럼의 이름  */
        ELDT_CheckColumn_ColumnOrder_SCHEMA_ID,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "schema identifier of a column identified by a column reference "
        "explicitly or implicitly contained in the search condition "
        "of the constraint being described"
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,              /**< Table ID */
        "TABLE_ID",                                    /**< 컬럼의 이름  */
        ELDT_CheckColumn_ColumnOrder_TABLE_ID,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "table identifier of a column identified by a column reference "
        "explicitly or implicitly contained in the search condition "
        "of the constraint being described"
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,              /**< Table ID */
        "COLUMN_ID",                                   /**< 컬럼의 이름  */
        ELDT_CheckColumn_ColumnOrder_COLUMN_ID,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "column identifier of a column identified by a column reference "
        "explicitly or implicitly contained in the search condition "
        "of the constraint being described"
    }
};


/**
 * @brief DEFINITION_SCHEMA.CHECK_COLUMN_USAGE 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescCHECK_COLUMN_USAGE =
{
    ELDT_TABLE_ID_CHECK_COLUMN_USAGE,                  /**< Table ID */
    "CHECK_COLUMN_USAGE",                              /**< 테이블의 이름  */
    "The CHECK_COLUMN_USAGE table has one row for each column "
    "identified by a column reference contained in the search condition "
    "of a check constraint, domain constraint, or assertion."
};



/**
 * @brief DEFINITION_SCHEMA.CHECK_COLUMN_USAGE 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescCHECK_COLUMN_USAGE[ELDT_CheckColumn_Const_MAX] =
{
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,            /**< Table ID */
        ELDT_CheckColumn_Const_PRIMARY_KEY,          /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        5,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,
            ELDT_CheckColumn_ColumnOrder_CONSTRAINT_ID,
            ELDT_CheckColumn_ColumnOrder_SCHEMA_ID,
            ELDT_CheckColumn_ColumnOrder_TABLE_ID,
            ELDT_CheckColumn_ColumnOrder_COLUMN_ID,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,            /**< Table ID */
        ELDT_CheckColumn_Const_FOREIGN_KEY_CHECK_TABLE_USAGE,  /**< Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,       /**< Table Constraint 유형 */
        4,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,
            ELDT_CheckColumn_ColumnOrder_CONSTRAINT_ID,
            ELDT_CheckColumn_ColumnOrder_SCHEMA_ID,
            ELDT_CheckColumn_ColumnOrder_TABLE_ID,
        },
        ELDT_TABLE_ID_CHECK_TABLE_USAGE,             /**< 참조 제약이 참조하는 Table ID */
        ELDT_CheckTable_Const_PRIMARY_KEY,           /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};



/**
 * @brief DEFINITION_SCHEMA.CHECK_COLUMN_USAGE 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescCHECK_COLUMN_USAGE[ELDT_CheckColumn_Index_MAX] =
{
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,            /**< Table ID */
        ELDT_CheckColumn_Index_CONSTRAINT_OWNER_ID,  /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckColumn_ColumnOrder_CONSTRAINT_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,            /**< Table ID */
        ELDT_CheckColumn_Index_CONSTRAINT_SCHEMA_ID, /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,            /**< Table ID */
        ELDT_CheckColumn_Index_CONSTRAINT_ID,        /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckColumn_ColumnOrder_CONSTRAINT_ID,
        }
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,            /**< Table ID */
        ELDT_CheckColumn_Index_OWNER_ID,             /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckColumn_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,            /**< Table ID */
        ELDT_CheckColumn_Index_SCHEMA_ID,            /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckColumn_ColumnOrder_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,            /**< Table ID */
        ELDT_CheckColumn_Index_TABLE_ID,             /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckColumn_ColumnOrder_TABLE_ID,
        }
    },
    {
        ELDT_TABLE_ID_CHECK_COLUMN_USAGE,            /**< Table ID */
        ELDT_CheckColumn_Index_COLUMN_ID,            /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckColumn_ColumnOrder_COLUMN_ID,
        }
    }
};



/** @} eldtCHECK_COLUMN_USAGE */

