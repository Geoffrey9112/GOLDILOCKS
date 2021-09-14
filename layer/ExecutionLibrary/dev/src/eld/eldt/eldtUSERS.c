/*******************************************************************************
 * eldtUSERS.c
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
 * @file eldtUSERS.c
 * @brief DEFINITION_SCHEMA.USERS 정의 명세  
 */

#include <ell.h>
#include <eldt.h>

/**
 * @brief DEFINITION_SCHEMA.USERS 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescUSERS[ELDT_User_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "AUTH_ID",                              /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_AUTH_ID,          /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "authorization identifier of the user"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "PROFILE_ID",                           /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_PROFILE_ID,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "profile identifier of the user"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "ENCRYPTED_PASSWORD",                   /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_ENCRYPTED_PASSWORD, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "encrypted password of the user"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "DEFAULT_DATA_TABLESPACE_ID",           /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_DEFAULT_DATA_TABLESPACE_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "default data tablespace identifier of the user"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "DEFAULT_TEMP_TABLESPACE_ID",           /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_DEFAULT_TEMP_TABLESPACE_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "default temporary tablespace identifier of the user"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "LOCKED_STATUS",                        /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_LOCKED_STATUS,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "account locked status of the user"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "LOCKED_STATUS_ID",                     /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_LOCKED_STATUS_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,            /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "account locked status identifier"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "LOCKED_TIME",                          /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_LOCKED_TIME,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "account locked time"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "FAILED_LOGIN_ATTEMPTS",                /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_FAILED_LOGIN_ATTEMPTS, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "consecutive failed login attempts count"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "EXPIRY_STATUS",                        /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_EXPIRY_STATUS,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "password expiry status of the user"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "EXPIRY_STATUS_ID",                     /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_EXPIRY_STATUS_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,            /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "expiry status identifier"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "EXPIRY_TIME",                          /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_EXPIRY_TIME,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "password expiry time"        
    },
    {
        ELDT_TABLE_ID_USERS,                    /**< Table ID */
        "PASSWORD_CHANGE_COUNT",                /**< 컬럼의 이름  */
        ELDT_User_ColumnOrder_PASSWORD_CHANGE_COUNT, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "password change count"        
    }
   
};


/**
 * @brief DEFINITION_SCHEMA.USERS 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescUSERS =
{
    ELDT_TABLE_ID_USERS,                  /**< Table ID */
    "USERS",                              /**< 테이블의 이름  */
    "The USERS table has one row for each user identifier. "
    "This table is not declared in SQL standard."
};



/**
 * @brief DEFINITION_SCHEMA.USERS 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescUSERS[ELDT_User_Const_MAX] =
{
    {
        ELDT_TABLE_ID_USERS,                         /**< Table ID */
        ELDT_User_Const_PRIMARY_KEY,                 /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_User_ColumnOrder_AUTH_ID,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};



/**
 * @brief DEFINITION_SCHEMA.USERS 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescUSERS[ELDT_User_Index_MAX] =
{
    {
        ELDT_TABLE_ID_USERS,                         /**< Table ID */
        ELDT_User_Index_DEFAULT_DATA_TABLESPACE_ID,  /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_User_ColumnOrder_DEFAULT_DATA_TABLESPACE_ID,
        }
    },
    {
        ELDT_TABLE_ID_USERS,                         /**< Table ID */
        ELDT_User_Index_DEFAULT_TEMP_TABLESPACE_ID,  /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_User_ColumnOrder_DEFAULT_TEMP_TABLESPACE_ID,
        }
    },
    {
        ELDT_TABLE_ID_USERS,                         /**< Table ID */
        ELDT_User_Index_PROFILE_ID,                  /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_User_ColumnOrder_PROFILE_ID,
        }
    }
};





/** @} eldtUSERS */

