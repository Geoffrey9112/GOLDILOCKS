/*******************************************************************************
 * eldtVIEW_TABLE_USAGE.c
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
 * @file eldtVIEW_TABLE_USAGE.c
 * @brief DEFINITION_SCHEMA.VIEW_TABLE_USAGE 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtVIEW_TABLE_USAGE
 * @{
 */


    
/**
 * @brief DEFINITION_SCHEMA.VIEW_TABLE_USAGE 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescVIEW_TABLE_USAGE[ELDT_ViewTable_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,                /**< Table ID */
        "VIEW_OWNER_ID",                               /**< 컬럼의 이름  */
        ELDT_ViewTable_ColumnOrder_VIEW_OWNER_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the view"        
    },
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,                /**< Table ID */
        "VIEW_SCHEMA_ID",                              /**< 컬럼의 이름  */
        ELDT_ViewTable_ColumnOrder_VIEW_SCHEMA_ID,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "schema identifier of the view"        
    },
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,                /**< Table ID */
        "VIEW_ID",                                     /**< 컬럼의 이름  */
        ELDT_ViewTable_ColumnOrder_VIEW_ID,            /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "view identifier"
    },
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,                /**< Table ID */
        "TABLE_OWNER_ID",                              /**< 컬럼의 이름  */
        ELDT_ViewTable_ColumnOrder_OWNER_ID,           /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the table of a table "
        "that is explicitly or implicitly referenced in the original query expression "
        " of the compiled view being described."
    },
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,                /**< Table ID */
        "TABLE_SCHEMA_ID",                             /**< 컬럼의 이름  */
        ELDT_ViewTable_ColumnOrder_SCHEMA_ID,          /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "schema identifier of a table "
        "that is explicitly or implicitly referenced in the original query expression "
        " of the compiled view being described."
    },
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,                /**< Table ID */
        "TABLE_ID",                                    /**< 컬럼의 이름  */
        ELDT_ViewTable_ColumnOrder_TABLE_ID,           /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "table identifier of a table "
        "that is explicitly or implicitly referenced in the original query expression "
        " of the compiled view being described."
    }
};


/**
 * @brief DEFINITION_SCHEMA.VIEW_TABLE_USAGE 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescVIEW_TABLE_USAGE =
{
    ELDT_TABLE_ID_VIEW_TABLE_USAGE,                  /**< Table ID */
    "VIEW_TABLE_USAGE",                              /**< 테이블의 이름  */
    "The VIEW_TABLE_USAGE table has one row for each table or view "
    "that is explicitly or implicitly referenced "
    "in the original query expression of the compiled view."
};


/**
 * @brief DEFINITION_SCHEMA.VIEW_TABLE_USAGE 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescVIEW_TABLE_USAGE[ELDT_ViewTable_Const_MAX] =
{
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,              /**< Table ID */
        ELDT_ViewTable_Const_PRIMARY_KEY,            /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        4,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_ViewTable_ColumnOrder_VIEW_SCHEMA_ID,
            ELDT_ViewTable_ColumnOrder_VIEW_ID,
            ELDT_ViewTable_ColumnOrder_SCHEMA_ID,
            ELDT_ViewTable_ColumnOrder_TABLE_ID,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,              /**< Table ID */
        ELDT_ViewTable_Const_FOREIGN_KEY_VIEWS,      /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,       /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_ViewTable_ColumnOrder_VIEW_SCHEMA_ID,
            ELDT_ViewTable_ColumnOrder_VIEW_ID,
        },
        ELDT_TABLE_ID_VIEWS,                         /**< 참조 제약이 참조하는 Table ID */
        ELDT_Views_Const_PRIMARY_KEY,                /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};



/**
 * @brief DEFINITION_SCHEMA.VIEW_TABLE_USAGE 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescVIEW_TABLE_USAGE[ELDT_ViewTable_Index_MAX] =
{
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,              /**< Table ID */
        ELDT_ViewTable_Index_VIEW_OWNER_ID,          /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_ViewTable_ColumnOrder_VIEW_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,              /**< Table ID */
        ELDT_ViewTable_Index_VIEW_SCHEMA_ID,         /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_ViewTable_ColumnOrder_VIEW_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,              /**< Table ID */
        ELDT_ViewTable_Index_VIEW_ID,                /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_ViewTable_ColumnOrder_VIEW_ID,
        }
    },
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,              /**< Table ID */
        ELDT_ViewTable_Index_OWNER_ID,               /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_ViewTable_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,              /**< Table ID */
        ELDT_ViewTable_Index_SCHEMA_ID,              /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_ViewTable_ColumnOrder_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_VIEW_TABLE_USAGE,              /**< Table ID */
        ELDT_ViewTable_Index_TABLE_ID,               /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_ViewTable_ColumnOrder_TABLE_ID,
        }
    }
};


/** @} eldtVIEW_TABLE_USAGE */

