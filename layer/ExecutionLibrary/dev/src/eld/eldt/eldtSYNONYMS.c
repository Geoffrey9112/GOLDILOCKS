/*******************************************************************************
 * eldtSYNONYMS.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: 
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file eldtSYNONYMS.c
 * @brief DEFINITION_SCHEMA.SYNONYMS 정의 명세  
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtSYNONYMS
 * @{
 */
/**
 * @brief DEFINITION_SCHEMA.SYNONYMS 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescSYNONYMS[ELDT_Synonyms_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_SYNONYMS,                 /**< Table ID */
        "OWNER_ID",                             /**< 컬럼의 이름  */
        ELDT_Synonyms_ColumnOrder_OWNER_ID,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the table of the synonym"        
    },
    {
        ELDT_TABLE_ID_SYNONYMS,                 /**< Table ID */
        "SCHEMA_ID",                            /**< 컬럼의 이름  */
        ELDT_Synonyms_ColumnOrder_SCHEMA_ID,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "schema identifier of the synonym"        
    },
    {
        ELDT_TABLE_ID_SYNONYMS,                 /**< Table ID */
        "SYNONYM_ID",                           /**< 컬럼의 이름  */
        ELDT_Synonyms_ColumnOrder_SYNONYM_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SERIAL_NUMBER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "synonym identifier"        
    },
    {
        ELDT_TABLE_ID_SYNONYMS,                 /**< Table ID */
        "SYNONYM_TABLE_ID",                     /**< 컬럼의 이름  */
        ELDT_Synonyms_ColumnOrder_SYNONYM_TABLE_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "table id of the synonym for naming resolution"
    },
    {
        ELDT_TABLE_ID_SYNONYMS,                 /**< Table ID */
        "SYNONYM_NAME",                         /**< 컬럼의 이름  */
        ELDT_Synonyms_ColumnOrder_SYNONYM_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "synonym name"        
    }, 
    {
        ELDT_TABLE_ID_SYNONYMS,                 /**< Table ID */
        "OBJECT_SCHEMA_NAME",                   /**< 컬럼의 이름  */
        ELDT_Synonyms_ColumnOrder_OBJECT_SCHEMA_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "schema name of the base object"        
    },
    {
        ELDT_TABLE_ID_SYNONYMS,                 /**< Table ID */
        "OBJECT_NAME",                          /**< 컬럼의 이름  */
        ELDT_Synonyms_ColumnOrder_OBJECT_SCHEMA_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "name of the base object"        
    },
    {
        ELDT_TABLE_ID_SYNONYMS,                 /**< Table ID */
        "IS_BUILTIN",                           /**< 컬럼의 이름  */
        ELDT_Synonyms_ColumnOrder_IS_BUILTIN,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "is built-in object or not"        
    },
    {
        ELDT_TABLE_ID_SYNONYMS,                 /**< Table ID */
        "CREATED_TIME",                         /**< 컬럼의 이름  */
        ELDT_Synonyms_ColumnOrder_CREATED_TIME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "created time of the synonym"        
    },
    {
        ELDT_TABLE_ID_SYNONYMS,                  /**< Table ID */
        "MODIFIED_TIME",                         /**< 컬럼의 이름  */
        ELDT_Synonyms_ColumnOrder_MODIFIED_TIME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "last modified time of the synonym"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.SYNONYMS 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescSYNONYMS =
{
    ELDT_TABLE_ID_SYNONYMS,                  /**< Table ID */
    "SYNONYMS",                              /**< 테이블의 이름  */
    "The SYNONYM table constain one row for each synonym"
};


/**
 * @brief DEFINITION_SCHEMA.SYNONYMS 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescSYNONYMS[ELDT_Synonyms_Const_MAX] =
{
    {
        ELDT_TABLE_ID_SYNONYMS,                      /**< Table ID */
        ELDT_Synonyms_Const_PRIMARY_KEY,             /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Synonyms_ColumnOrder_SCHEMA_ID,
            ELDT_Synonyms_ColumnOrder_SYNONYM_ID
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_SYNONYMS,                     /**< Table ID */
        ELDT_Synonyms_Const_UNIQUE_SCHEMA_ID_SYNONYM_NAME, /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Synonyms_ColumnOrder_SCHEMA_ID,
            ELDT_Synonyms_ColumnOrder_SYNONYM_NAME,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_SYNONYMS,                     /**< Table ID */
        ELDT_Synonyms_Const_FOREIGN_KEY_SCHEMATA,   /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,      /**< Table Constraint 유형 */
        1,                                          /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Synonyms_ColumnOrder_SCHEMA_ID,
        },
        ELDT_TABLE_ID_SCHEMATA,                     /**< 참조 제약이 참조하는 Table ID */
        ELDT_Schemata_Const_PRIMARY_KEY,            /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/**
 * @brief DEFINITION_SCHEMA.SYNONYMS 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescSYNONYMS[ELDT_Synonyms_Index_MAX] =
{
    {
        ELDT_TABLE_ID_SYNONYMS,                     /**< Table ID */
        ELDT_Synonyms_Index_OWNER_ID  ,             /**< Table 내 Index 번호 */
        1,                                          /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Synonyms_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_SYNONYMS,                     /**< Table ID */
        ELDT_Synonyms_Index_SYNONYM_ID  ,          /**< Table 내 Index 번호 */
        1,                                          /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Synonyms_ColumnOrder_SYNONYM_ID,
        }
    }
};

/** @} eldtSYNONYMS */

