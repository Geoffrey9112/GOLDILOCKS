/*******************************************************************************
 * eldtUSER_PASSWORD_HISTORY.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldtUSER_PASSWORD_HISTORY.c 15987 2015-09-15 09:07:20Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file eldtUSER_PASSWORD_HISTORY.c
 * @brief DEFINITION_SCHEMA.USER_PASSWORD_HISTORY 정의 명세  
 */

#include <ell.h>
#include <eldt.h>

/**
 * @brief DEFINITION_SCHEMA.USER_PASSWORD_HISTORY 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescUSER_PASSWORD_HISTORY[ELDT_PassHist_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_USER_PASSWORD_HISTORY,    /**< Table ID */
        "AUTH_ID",                              /**< 컬럼의 이름  */
        ELDT_PassHist_ColumnOrder_AUTH_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization identifier of the password"        
    },
    {
        ELDT_TABLE_ID_USER_PASSWORD_HISTORY,    /**< Table ID */
        "ENCRYPTED_PASSWORD",                   /**< 컬럼의 이름  */
        ELDT_PassHist_ColumnOrder_ENCRYPTED_PASSWORD, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "encrypted password"        
    },
    {
        ELDT_TABLE_ID_USER_PASSWORD_HISTORY,    /**< Table ID */
        "PASSWORD_CHANGE_NO",                   /**< 컬럼의 이름  */
        ELDT_PassHist_ColumnOrder_PASSWORD_CHANGE_NO, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "password change number"        
    },
    {
        ELDT_TABLE_ID_USER_PASSWORD_HISTORY,    /**< Table ID */
        "PASSWORD_CHANGE_TIME",                 /**< 컬럼의 이름  */
        ELDT_PassHist_ColumnOrder_PASSWORD_CHANGE_TIME,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "password change time"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.USER_PASSWORD_HISTORY 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescUSER_PASSWORD_HISTORY =
{
    ELDT_TABLE_ID_USER_PASSWORD_HISTORY,                  /**< Table ID */
    "USER_PASSWORD_HISTORY",                              /**< 테이블의 이름  */
    "The USER_PASSWORD_HISTORY table has one row for each password change. "
    "This table is not declared in SQL standard."
};



/**
 * @brief DEFINITION_SCHEMA.USER_PASSWORD_HISTORY 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescUSER_PASSWORD_HISTORY[ELDT_PassHist_Const_MAX] =
{
    {
        ELDT_TABLE_ID_USER_PASSWORD_HISTORY,         /**< Table ID */
        ELDT_PassHist_Const_FOREIGN_KEY_USERS,       /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_PassHist_ColumnOrder_AUTH_ID,
        },
        ELDT_TABLE_ID_USERS,              /**< 참조 제약이 참조하는 Table ID */
        ELDT_User_Const_PRIMARY_KEY,      /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};






/** @} eldtUSER_PASSWORD_HISTORY */

