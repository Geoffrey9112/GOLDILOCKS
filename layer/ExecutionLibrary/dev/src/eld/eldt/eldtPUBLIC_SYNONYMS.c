/*******************************************************************************
 * eldtPUBLIC_SYNONYMS.c
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
 * @file eldtPUBLIC_SYNONYMS.c
 * @brief DEFINITION_SCHEMA.PUBLIC_SYNONYMS 정의 명세  
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtPUBLIC_SYNONYMS
 * @{
 */
/**
 * @brief DEFINITION_SCHEMA.PUBLIC_SYNONYMS 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescPUBLIC_SYNONYMS[ELDT_Public_Synonyms_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_PUBLIC_SYNONYMS,          /**< Table ID */
        "SYNONYM_ID",                           /**< 컬럼의 이름  */
        ELDT_Public_Synonyms_ColumnOrder_SYNONYM_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SERIAL_NUMBER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "synonym identifier"        
    },
    {
        ELDT_TABLE_ID_PUBLIC_SYNONYMS,          /**< Table ID */
        "SYNONYM_NAME",                         /**< 컬럼의 이름  */
        ELDT_Public_Synonyms_ColumnOrder_SYNONYM_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "synonym name"        
    },
    {
        ELDT_TABLE_ID_PUBLIC_SYNONYMS,          /**< Table ID */
        "CREATOR_ID",                           /**< 컬럼의 이름  */
        ELDT_Public_Synonyms_ColumnOrder_CREATOR_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization identifier who creates the synonym"        
    },    
    {
        ELDT_TABLE_ID_PUBLIC_SYNONYMS,          /**< Table ID */
        "OBJECT_SCHEMA_NAME",                   /**< 컬럼의 이름  */
        ELDT_Public_Synonyms_ColumnOrder_OBJECT_SCHEMA_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "base object identifier"        
    },
    {
        ELDT_TABLE_ID_PUBLIC_SYNONYMS,          /**< Table ID */
        "OBJECT_NAME",                          /**< 컬럼의 이름  */
        ELDT_Public_Synonyms_ColumnOrder_OBJECT_SCHEMA_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "base object identifier"        
    },
    {
        ELDT_TABLE_ID_PUBLIC_SYNONYMS,          /**< Table ID */
        "IS_BUILTIN",                           /**< 컬럼의 이름  */
        ELDT_Public_Synonyms_ColumnOrder_IS_BUILTIN,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "is built-in object or not"        
    },
    {
        ELDT_TABLE_ID_PUBLIC_SYNONYMS,          /**< Table ID */
        "CREATED_TIME",                         /**< 컬럼의 이름  */
        ELDT_Public_Synonyms_ColumnOrder_CREATED_TIME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "created time of the synonym"        
    },
    {
        ELDT_TABLE_ID_PUBLIC_SYNONYMS,           /**< Table ID */
        "MODIFIED_TIME",                         /**< 컬럼의 이름  */
        ELDT_Public_Synonyms_ColumnOrder_MODIFIED_TIME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "last modified time of the synonym"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.PUBLIC_SYNONYMS 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescPUBLIC_SYNONYMS =
{
    ELDT_TABLE_ID_PUBLIC_SYNONYMS,                  /**< Table ID */
    "PUBLIC_SYNONYMS",                              /**< 테이블의 이름  */
    "The PUBLIC SYNONYM table constain one row for each public synonym"
};


/**
 * @brief DEFINITION_SCHEMA.PUBLIC_SYNONYMS 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescPUBLIC_SYNONYMS[ELDT_Public_Synonyms_Const_MAX] =
{
    {
        ELDT_TABLE_ID_PUBLIC_SYNONYMS,               /**< Table ID */
        ELDT_Public_Synonyms_Const_PRIMARY_KEY,      /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Public_Synonyms_ColumnOrder_SYNONYM_ID
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_PUBLIC_SYNONYMS,               /**< Table ID */
        ELDT_Public_Synonyms_Const_UNIQUE_SYNONYM_NAME, /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Public_Synonyms_ColumnOrder_SYNONYM_NAME,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};

/** @} eldtPUBLIC_SYNONYMS */

