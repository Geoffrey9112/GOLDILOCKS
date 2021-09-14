/*******************************************************************************
 * eldtROLE_AUTHORIZATION_DECRIPTORS.c
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
 * @file eldtROLE_AUTHORIZATION_DESCRIPTORS.c
 * @brief DEFINITION_SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtROLE_AUTHORIZATION_DESCRIPTORS
 * @{
 */


    
/**
 * @brief DEFINITION_SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescROLE_AUTHORIZATION_DESCRIPTORS[ELDT_RoleDesc_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS, /**< Table ID */
        "ROLE_ID",                                    /**< 컬럼의 이름  */
        ELDT_RoleDesc_ColumnOrder_ROLE_ID,            /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                       /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                    /**< 컬럼의 Nullable 여부 */
        "the role identifier of some role granted by the grant role statement "
        "or the role name of a role definition"        
    },
    {
        ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS, /**< Table ID */
        "GRANTEE_ID",                                 /**< 컬럼의 이름  */
        ELDT_RoleDesc_ColumnOrder_GRANTEE_ID,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                       /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                    /**< 컬럼의 Nullable 여부 */
        "an authorization identifier, possibly PUBLIC, or role identifier"
        "specified as a grantee contained in a grant role statement, "
        "or the authorization identifier of the current SQLsession "
        "when the role definition is executed."
    },
    {
        ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS, /**< Table ID */
        "GRANTOR_ID",                                 /**< 컬럼의 이름  */
        ELDT_RoleDesc_ColumnOrder_GRANTOR_ID,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                       /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                    /**< 컬럼의 Nullable 여부 */
        "the authorization identifier of the user who granted the role identified "
        "by ROLE_NAME to the user identified by the value of GRANTEE, "
        "or _SYSTEM to indicate that the privileges were granted "
        "to the authorization identifier of the creator of the object "
        "on which the privileges were granted"
    },
    {
        ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS, /**< Table ID */
        "IS_GRANTABLE",                               /**< 컬럼의 이름  */
        ELDT_RoleDesc_ColumnOrder_IS_GRANTABLE,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                /**< 컬럼의 Nullable 여부 */
        "The values of IS_GRANTABLE have the following meanings:\n"
        "- TRUE : The described role is grantable\n"
        "- FALSE : The described role is not grantable\n"
    }
};


/**
 * @brief DEFINITION_SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescROLE_AUTHORIZATION_DESCRIPTORS =
{
    ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,                  /**< Table ID */
    "ROLE_AUTHORIZATION_DESCRIPTORS",                              /**< 테이블의 이름  */
    "The ROLE_AUTHORIZATION_DESCRIPTORS contains a representation "
    "of the role authorization descriptors."
};


/**
 * @brief DEFINITION_SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescROLE_AUTHORIZATION_DESCRIPTORS[ELDT_RoleDesc_Const_MAX] =
{
    {
        ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,/**< Table ID */
        ELDT_RoleDesc_Const_PRIMARY_KEY,             /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        3,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_RoleDesc_ColumnOrder_ROLE_ID,
            ELDT_RoleDesc_ColumnOrder_GRANTEE_ID,
            ELDT_RoleDesc_ColumnOrder_GRANTOR_ID
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/**
 * @brief DEFINITION_SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescROLE_AUTHORIZATION_DESCRIPTORS[ELDT_RoleDesc_Index_MAX] =
{
    {
        ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,/**< Table ID */
        ELDT_RoleDesc_Index_ROLE_ID,                 /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_RoleDesc_ColumnOrder_ROLE_ID,
        }
    },
    {
        ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,/**< Table ID */
        ELDT_RoleDesc_Index_GRANTOR_ID,              /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_RoleDesc_ColumnOrder_GRANTOR_ID,
        }
    },
    {
        ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,/**< Table ID */
        ELDT_RoleDesc_Index_GRANTEE_ID,              /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_RoleDesc_ColumnOrder_GRANTEE_ID,
        }
    }
};


/** @} eldtROLE_AUTHORIZATION_DESCRIPTORS */

