/*******************************************************************************
 * eldtUSERS.h
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


#ifndef _ELDT_USERS_H_
#define _ELDT_USERS_H_ 1

/**
 * @file eldtUSERS.h
 * @brief USERS dictionary base table
 */

/**
 * @defgroup eldtUSERS USERS table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief USERS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderUSERS
{
    /*
     * 식별자 영역
     */
    
    ELDT_User_ColumnOrder_AUTH_ID = 0,
    ELDT_User_ColumnOrder_PROFILE_ID,

    /*
     * SQL 표준 영역
     */

    
    /*
     * 부가 정보 영역
     */

    ELDT_User_ColumnOrder_ENCRYPTED_PASSWORD,
    ELDT_User_ColumnOrder_DEFAULT_DATA_TABLESPACE_ID,
    ELDT_User_ColumnOrder_DEFAULT_TEMP_TABLESPACE_ID,

    ELDT_User_ColumnOrder_LOCKED_STATUS,
    ELDT_User_ColumnOrder_LOCKED_STATUS_ID,
    ELDT_User_ColumnOrder_LOCKED_TIME,
    ELDT_User_ColumnOrder_FAILED_LOGIN_ATTEMPTS,
    
    ELDT_User_ColumnOrder_EXPIRY_STATUS,
    ELDT_User_ColumnOrder_EXPIRY_STATUS_ID,
    ELDT_User_ColumnOrder_EXPIRY_TIME,
    ELDT_User_ColumnOrder_PASSWORD_CHANGE_COUNT,
    
    ELDT_User_ColumnOrder_MAX

} eldtColumnOrderUSERS;


extern eldtDefinitionTableDesc   gEldtTableDescUSERS;
extern eldtDefinitionColumnDesc  gEldtColumnDescUSERS[ELDT_User_ColumnOrder_MAX];


/**
 * @brief USERS 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 * - 비표준
 *  - 비표준 1 : PRIMARY KEY ( AUTH_ID ) 
 */

typedef enum eldtKeyConstUSERS
{
    /*
     * Primary Key 영역
     */

    ELDT_User_Const_PRIMARY_KEY = 0,                  /**< 비표준 1 */
    
    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    ELDT_User_Const_MAX
    
} eldtKeyConstUSERS;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescUSERS[ELDT_User_Const_MAX];




/**
 * @brief USERS 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexUSERS
{
    /* ELDT_User_Index_AUTH_ID = 0,  Primary Key 로 존재함 */
    ELDT_User_Index_DEFAULT_DATA_TABLESPACE_ID = 0,
    ELDT_User_Index_DEFAULT_TEMP_TABLESPACE_ID,
    ELDT_User_Index_PROFILE_ID,
    
    ELDT_User_Index_MAX
    
} eldtIndexUSERS;

extern eldtDefinitionIndexDesc  gEldtIndexDescUSERS[ELDT_User_Index_MAX];





/** @} eldtUSERS */



#endif /* _ELDT_USERS_H_ */
