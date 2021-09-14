/*******************************************************************************
 * eldtPROFILE_KERNEL_PARAMETER.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldtPROFILE_KERNEL_PARAMETER.c 14729 2015-04-06 03:16:14Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file eldtPROFILE_KERNEL_PARAMETER.c
 * @brief DEFINITION_SCHEMA.PROFILE_KERNEL_PARAMETER 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtPROFILE_KERNEL_PARAMETER
 * @{
 */

    
/**
 * @brief DEFINITION_SCHEMA.PROFILE_KERNEL_PARAMETER 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescPROFILE_KERNEL_PARAMETER[ELDT_KernelParam_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_PROFILE_KERNEL_PARAMETER,    /**< Table ID */
        "PROFILE_ID",                              /**< 컬럼의 이름  */
        ELDT_KernelParam_ColumnOrder_PROFILE_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "profile identifier"        
    },
    {
        ELDT_TABLE_ID_PROFILE_KERNEL_PARAMETER,    /**< Table ID */
        "PARAMETER_ID",                            /**< 컬럼의 이름  */
        ELDT_KernelParam_ColumnOrder_PARAMETER_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,               /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "password parameter identifier"        
    },
    {
        ELDT_TABLE_ID_PROFILE_KERNEL_PARAMETER,    /**< Table ID */
        "PARAMETER_NAME",                          /**< 컬럼의 이름  */
        ELDT_KernelParam_ColumnOrder_PARAMETER_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,             /**< 컬럼의 Nullable 여부 */
        "password parameter name"        
    },
    {
        ELDT_TABLE_ID_PROFILE_KERNEL_PARAMETER,      /**< Table ID */
        "IS_DEFAULT",                                /**< 컬럼의 이름  */
        ELDT_KernelParam_ColumnOrder_IS_DEFAULT,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                       /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,               /**< 컬럼의 Nullable 여부 */
        "password parameter value is default or not"        
    },
    {
        ELDT_TABLE_ID_PROFILE_KERNEL_PARAMETER,      /**< Table ID */
        "PARAMETER_VALUE",                           /**< 컬럼의 이름  */
        ELDT_KernelParam_ColumnOrder_PARAMETER_VALUE,/**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_FLOATING_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                           /**< 컬럼의 Nullable 여부 */
        "password parameter value"        
    },
    {
        ELDT_TABLE_ID_PROFILE_KERNEL_PARAMETER,      /**< Table ID */
        "PARAMETER_STRING",                          /**< 컬럼의 이름  */
        ELDT_KernelParam_ColumnOrder_PARAMETER_STRING, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_SYNTAX,                      /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,               /**< 컬럼의 Nullable 여부 */
        "password parameter value string"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.PROFILE_KERNEL_PARAMETER 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescPROFILE_KERNEL_PARAMETER =
{
    ELDT_TABLE_ID_PROFILE_KERNEL_PARAMETER,                  /**< Table ID */
    "PROFILE_KERNEL_PARAMETER",                              /**< 테이블의 이름  */
    "The PROFILE_KERNEL_PARAMETER table has one row for each password parameter of a profile "
};


/**
 * @brief DEFINITION_SCHEMA.PROFILE_KERNEL_PARAMETER 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescPROFILE_KERNEL_PARAMETER[ELDT_KernelParam_Const_MAX] =
{
    {
        ELDT_TABLE_ID_PROFILE_KERNEL_PARAMETER,      /**< Table ID */
        ELDT_KernelParam_Const_PRIMARY_KEY,          /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                               /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KernelParam_ColumnOrder_PROFILE_ID,
            ELDT_KernelParam_ColumnOrder_PARAMETER_ID,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_PROFILE_KERNEL_PARAMETER,      /**< Table ID */
        ELDT_KernelParam_Const_FOREIGN_KEY_PROFILES, /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KernelParam_ColumnOrder_PROFILE_ID,
        },
        ELDT_TABLE_ID_PROFILES,                      /**< 참조 제약이 참조하는 Table ID */
        ELDT_Profile_Const_PRIMARY_KEY,              /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};





/** @} eldtPROFILE_KERNEL_PARAMETER */

