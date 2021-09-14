/*******************************************************************************
 * eldtUSER_SCHEMA_PATH.c
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
 * @file eldtUSER_SCHEMA_PATH.c
 * @brief DEFINITION_SCHEMA.USER_SCHEMA_PATH 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtUSER_SCHEMA_PATH
 * @{
 */


    
/**
 * @brief DEFINITION_SCHEMA.USER_SCHEMA_PATH 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescUSER_SCHEMA_PATH[ELDT_UserPath_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_USER_SCHEMA_PATH,                /**< Table ID */
        "AUTH_ID",                                     /**< 컬럼의 이름  */
        ELDT_UserPath_ColumnOrder_AUTH_ID,             /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "authorization identifier of the user or PUBLIC(all users)"        
    },
    {
        ELDT_TABLE_ID_USER_SCHEMA_PATH,                /**< Table ID */
        "SCHEMA_ID",                                   /**< 컬럼의 이름  */
        ELDT_UserPath_ColumnOrder_SCHEMA_ID,           /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "schema identifier"        
    },
    {
        ELDT_TABLE_ID_USER_SCHEMA_PATH,                /**< Table ID */
        "SEARCH_ORDER",                                /**< 컬럼의 이름  */
        ELDT_UserPath_ColumnOrder_SEARCH_ORDER,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "the search order of schema identifier for schema name resolution of "
        "unqualified SQL-Schema object, such as table name without schema name"
    }
};


/**
 * @brief DEFINITION_SCHEMA.USER_SCHEMA_PATH 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescUSER_SCHEMA_PATH =
{
    ELDT_TABLE_ID_USER_SCHEMA_PATH,                  /**< Table ID */
    "USER_SCHEMA_PATH",                              /**< 테이블의 이름  */
    "The USER_SCHEMA_PATH table has one or more rows for each user. "
    "The rows list the search order of schema identifier for unqualified SQL-schema object. "
    "This table is not declared in SQL standard."
};


/**
 * @brief DEFINITION_SCHEMA.USER_SCHEMA_PATH 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescUSER_SCHEMA_PATH[ELDT_UserPath_Const_MAX] =
{
    {
        ELDT_TABLE_ID_USER_SCHEMA_PATH,              /**< Table ID */
        ELDT_UserPath_Const_PRIMARY_KEY,             /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        3,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_UserPath_ColumnOrder_AUTH_ID,
            ELDT_UserPath_ColumnOrder_SCHEMA_ID,
            ELDT_UserPath_ColumnOrder_SEARCH_ORDER,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};




/**
 * @brief DEFINITION_SCHEMA.USER_SCHEMA_PATH 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescUSER_SCHEMA_PATH[ELDT_UserPath_Index_MAX] =
{
    {
        ELDT_TABLE_ID_USER_SCHEMA_PATH,              /**< Table ID */
        ELDT_UserPath_Index_AUTH_ID,                 /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_UserPath_ColumnOrder_AUTH_ID,
        }
    },
    {
        ELDT_TABLE_ID_USER_SCHEMA_PATH,              /**< Table ID */
        ELDT_UserPath_Index_SCHEMA_ID,               /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_UserPath_ColumnOrder_SCHEMA_ID,
        }
    }
};





/** @} eldtUSER_SCHEMA_PATH */

