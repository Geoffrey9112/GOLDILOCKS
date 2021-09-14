/*******************************************************************************
 * eldtUSAGE_PRIVILEGES.c
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
 * @file eldtUSAGE_PRIVILEGES.c
 * @brief DEFINITION_SCHEMA.USAGE_PRIVILEGES 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtUSAGE_PRIVILEGES
 * @{
 */


    
/**
 * @brief DEFINITION_SCHEMA.USAGE_PRIVILEGES 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescUSAGE_PRIVILEGES[ELDT_UsagePriv_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,               /**< Table ID */
        "GRANTOR_ID",                                 /**< 컬럼의 이름  */
        ELDT_UsagePriv_ColumnOrder_GRANTOR_ID,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                       /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                    /**< 컬럼의 Nullable 여부 */
        "authorization identifier of the user who granted usage privileges,"
        "on the object of the type identified by OBJECT_TYPE"
    },
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,               /**< Table ID */
        "GRANTEE_ID",                                 /**< 컬럼의 이름  */
        ELDT_UsagePriv_ColumnOrder_GRANTEE_ID,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                       /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                    /**< 컬럼의 Nullable 여부 */
        "authorization identifier of some user or role, or PUBLIC "
        "to indicate all users, to whom the usage privilege being described is granted"
    },
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,               /**< Table ID */
        "SCHEMA_ID",                                  /**< 컬럼의 이름  */
        ELDT_UsagePriv_ColumnOrder_SCHEMA_ID,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                       /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                    /**< 컬럼의 Nullable 여부 */
        "schema identifier of the object of the type identified by OBJECT_TYPE "
        "on which the privilege being described was granted"
    },
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,               /**< Table ID */
        "OBJECT_ID",                                  /**< 컬럼의 이름  */
        ELDT_UsagePriv_ColumnOrder_OBJECT_ID,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                       /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                    /**< 컬럼의 Nullable 여부 */
        "object identifier of the object to which the privilege applies"
    },
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,               /**< Table ID */
        "OBJECT_TYPE",                                /**< 컬럼의 이름  */
        ELDT_UsagePriv_ColumnOrder_OBJECT_TYPE,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                /**< 컬럼의 Nullable 여부 */
        "The values of OBJECT_TYPE have the following meanings:\n"
        "- DOMAIN : unsupported feature\n"
        "- CHARACTER SET : unsupported feature\n"
        "- COLLATION : unsupported feature\n"
        "- TRANSLATION : unsupported feature\n"
        "- SEQUENCE : The object to which the privilege applies is a sequence generator.\n"
    },
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,               /**< Table ID */
        "OBJECT_TYPE_ID",                             /**< 컬럼의 이름  */
        ELDT_UsagePriv_ColumnOrder_OBJECT_TYPE_ID,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                    /**< 컬럼의 Nullable 여부 */
        "OBJECT_TYPE identifier"
    },
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,               /**< Table ID */
        "IS_GRANTABLE",                               /**< 컬럼의 이름  */
        ELDT_UsagePriv_ColumnOrder_IS_GRANTABLE,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                /**< 컬럼의 Nullable 여부 */
        "The values of IS_GRANTABLE have the following meanings:\n"
        "- TRUE : The privilege being described was granted WITH GRANT OPTION "
        "and is thus grantable\n"
        "- FALSE : The privilege being described was not granted WITH GRANT OPTION "
        "and is thus not grantable\n"
    },
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,               /**< Table ID */
        "IS_BUILTIN",                                 /**< 컬럼의 이름  */
        ELDT_UsagePriv_ColumnOrder_IS_BUILTIN,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                /**< 컬럼의 Nullable 여부 */
        "is built-in privilege or not"
    }
};


/**
 * @brief DEFINITION_SCHEMA.USAGE_PRIVILEGES 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescUSAGE_PRIVILEGES =
{
    ELDT_TABLE_ID_USAGE_PRIVILEGES,                  /**< Table ID */
    "USAGE_PRIVILEGES",                              /**< 테이블의 이름  */
    "The USAGE_PRIVILEGES table has one row for each usage privilege descriptor."
};


/**
 * @brief DEFINITION_SCHEMA.USAGE_PRIVILEGES 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescUSAGE_PRIVILEGES[ELDT_UsagePriv_Const_MAX] =
{
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,              /**< Table ID */
        ELDT_UsagePriv_Const_PRIMARY_KEY,            /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        4,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_UsagePriv_ColumnOrder_GRANTOR_ID,
            ELDT_UsagePriv_ColumnOrder_GRANTEE_ID,
            ELDT_UsagePriv_ColumnOrder_OBJECT_ID,
            ELDT_UsagePriv_ColumnOrder_OBJECT_TYPE_ID,  
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/**
 * @brief DEFINITION_SCHEMA.USAGE_PRIVILEGES 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescUSAGE_PRIVILEGES[ELDT_UsagePriv_Index_MAX] =
{
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,              /**< Table ID */
        ELDT_UsagePriv_Index_GRANTOR_ID,             /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_UsagePriv_ColumnOrder_GRANTOR_ID,
        }
    },
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,              /**< Table ID */
        ELDT_UsagePriv_Index_GRANTEE_ID,             /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_UsagePriv_ColumnOrder_GRANTEE_ID,
        }
    },
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,              /**< Table ID */
        ELDT_UsagePriv_Index_SCHEMA_ID,              /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_UsagePriv_ColumnOrder_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_USAGE_PRIVILEGES,              /**< Table ID */
        ELDT_UsagePriv_Index_OBJECT_ID,              /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_UsagePriv_ColumnOrder_OBJECT_ID,
        }
    }
};


/** @} eldtUSAGE_PRIVILEGES */

