/*******************************************************************************
 * eldtCATALOG_NAME.c
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
 * @file eldtCATALOG_NAME.c
 * @brief DEFINITION_SCHEMA.CATALOG_NAME 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtCATALOG_NAME
 * @{
 */


    
/**
 * @brief DEFINITION_SCHEMA.CATALOG_NAME 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescCATALOG[ELDT_Catalog_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_CATALOG_NAME,             /**< Table ID */
        "CATALOG_ID",                           /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_CATALOG_ID,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain, Sequence 로 생성하지 않음 */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "catalog(database) identifier"        
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,              /**< Table ID */
        "CATALOG_NAME",                          /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_CATALOG_NAME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "name of the catalog(database) that are described by DEFINITION_SCHEMA"
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,              /**< Table ID */
        "CHARACTER_SET_ID",                      /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_CHARACTER_SET_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "character set identifier"        
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,              /**< Table ID */
        "CHARACTER_SET_NAME",                    /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_CHARACTER_SET_NAME,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "character set name of the catalog(database)"        
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,              /**< Table ID */
        "CHAR_LENGTH_UNITS_ID",                  /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_CHAR_LENGTH_UNITS_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "char length units identifier"        
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,              /**< Table ID */
        "CHAR_LENGTH_UNITS",                     /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_CHAR_LENGTH_UNITS,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "char length units of the catalog(database)"        
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,              /**< Table ID */
        "TIME_ZONE_INTERVAL",                    /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_TIME_ZONE_INTERVAL,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_INTERVAL,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "time zone of the catalog(database)"        
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,             /**< Table ID */
        "DEFAULT_DATA_TABLESPACE_ID",           /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_DEFAULT_DATA_TABLESPACE_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "default data tablespace identifier of the catalog(database)"        
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,             /**< Table ID */
        "DEFAULT_TEMP_TABLESPACE_ID",           /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_DEFAULT_TEMP_TABLESPACE_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "default temporary tablespace identifier of catalog(database)"        
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,              /**< Table ID */
        "CREATED_TIME",                          /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_CREATED_TIME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "created time of the catalog(database)"        
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,              /**< Table ID */
        "MODIFIED_TIME",                         /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_MODIFIED_TIME,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "last modified time of the catalog(database)"        
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,              /**< Table ID */
        "COMMENTS",                              /**< 컬럼의 이름  */
        ELDT_Catalog_ColumnOrder_COMMENTS,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "comments of the catalog(database)"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.CATALOG_NAME 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescCATALOG =
{
    ELDT_TABLE_ID_CATALOG_NAME,                  /**< Table ID */
    "CATALOG_NAME",                              /**< 테이블의 이름  */
    "The CATALOG_NAME table identifies the catalog that is described by DEFINITION_SCHEMA."
};


/**
 * @brief DEFINITION_SCHEMA.CATALOG_NAME 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescCATALOG_NAME[ELDT_Catalog_Const_MAX] =
{
    {
        ELDT_TABLE_ID_CATALOG_NAME,                  /**< Table ID */
        ELDT_Catalog_Const_PRIMARY_KEY,              /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Catalog_ColumnOrder_CATALOG_ID,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_CATALOG_NAME,                  /**< Table ID */
        ELDT_Catalog_Const_UNIQUE_CATALOG_NAME,      /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Catalog_ColumnOrder_CATALOG_NAME,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/** @} eldtCATALOG_NAME */

