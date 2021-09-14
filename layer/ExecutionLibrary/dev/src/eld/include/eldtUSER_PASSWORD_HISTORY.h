/*******************************************************************************
 * eldtUSER_PASSWORD_HISTORY.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldtUSER_PASSWORD_HISTORY.h 15987 2015-09-15 09:07:20Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ELDT_USER_PASSWORD_HISTORY_H_
#define _ELDT_USER_PASSWORD_HISTORY_H_ 1

/**
 * @file eldtUSER_PASSWORD_HISTORY.h
 * @brief USER_PASSWORD_HISTORY dictionary base table
 */

/**
 * @defgroup eldtUSER_PASSWORD_HISTORY USER_PASSWORD_HISTORY table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief USER_PASSWORD_HISTORY 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderUSER_PASSWORD_HISTORY
{
    /*
     * 식별자 영역
     */
    
    ELDT_PassHist_ColumnOrder_AUTH_ID = 0,

    /*
     * SQL 표준 영역
     */

    
    /*
     * 부가 정보 영역
     */

    ELDT_PassHist_ColumnOrder_ENCRYPTED_PASSWORD,
    ELDT_PassHist_ColumnOrder_PASSWORD_CHANGE_NO,
    ELDT_PassHist_ColumnOrder_PASSWORD_CHANGE_TIME,
    
    ELDT_PassHist_ColumnOrder_MAX

} eldtColumnOrderUSER_PASSWORD_HISTORY;


extern eldtDefinitionTableDesc   gEldtTableDescUSER_PASSWORD_HISTORY;
extern eldtDefinitionColumnDesc  gEldtColumnDescUSER_PASSWORD_HISTORY[ELDT_PassHist_ColumnOrder_MAX];


/**
 * @brief USER_PASSWORD_HISTORY 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 * - 비표준
 *  - 비표준 1 : FOREIGN KEY ( AUTH_ID ) 
 */

typedef enum eldtKeyConstUSER_PASSWORD_HISTORY
{
    /*
     * Primary Key 영역
     */

    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    ELDT_PassHist_Const_FOREIGN_KEY_USERS = 0,                  /**< 비표준 1 */
    
    
    ELDT_PassHist_Const_MAX
    
} eldtKeyConstUSER_PASSWORD_HISTORY;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescUSER_PASSWORD_HISTORY[ELDT_PassHist_Const_MAX];




/**
 * @brief USER_PASSWORD_HISTORY 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexUSER_PASSWORD_HISTORY
{
    ELDT_PassHist_Index_MAX = 0
    
} eldtIndexUSER_PASSWORD_HISTORY;






/** @} eldtUSER_PASSWORD_HISTORY */



#endif /* _ELDT_USER_PASSWORD_HISTORY_H_ */
