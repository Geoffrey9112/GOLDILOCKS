/*******************************************************************************
 * eldtUSER_SCHEMA_PATH.h
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


#ifndef _ELDT_USER_SCHEMA_PATH_H_
#define _ELDT_USER_SCHEMA_PATH_H_ 1

/**
 * @file eldtUSER_SCHEMA_PATH.h
 * @brief USER_SCHEMA_PATH dictionary base table
 */

/**
 * @defgroup eldtUSER_SCHEMA_PATH USER_SCHEMA_PATH table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief USER_SCHEMA_PATH 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderUSER_SCHEMA_PATH
{
    /*
     * 식별자 영역
     */

    ELDT_UserPath_ColumnOrder_AUTH_ID = 0,
    ELDT_UserPath_ColumnOrder_SCHEMA_ID,
    
    /*
     * SQL 표준 영역
     */

    /*
     * 부가 정보 영역
     */
    ELDT_UserPath_ColumnOrder_SEARCH_ORDER,
    
    ELDT_UserPath_ColumnOrder_MAX
    
} eldtColumnOrderUSER_SCHEMA_PATH;
    

extern eldtDefinitionTableDesc   gEldtTableDescUSER_SCHEMA_PATH;
extern eldtDefinitionColumnDesc  gEldtColumnDescUSER_SCHEMA_PATH[ELDT_UserPath_ColumnOrder_MAX];


/**
 * @brief USER_SCHEMA_PATH 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 * - 비표준
 *  - 비표준 1 : PRIMARY KEY ( AUTH_ID, SCHEMA_ID, SEARCH_ORDER ) 
 */

typedef enum eldtKeyConstUSER_SCHEMA_PATH
{
    /*
     * Primary Key 영역
     */

    ELDT_UserPath_Const_PRIMARY_KEY = 0,                  /**< 비표준 1 */
    
    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    ELDT_UserPath_Const_MAX
    
} eldtKeyConstUSER_SCHEMA_PATH;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescUSER_SCHEMA_PATH[ELDT_UserPath_Const_MAX];




/**
 * @brief USER_SCHEMA_PATH 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexUSER_SCHEMA_PATH
{
    ELDT_UserPath_Index_AUTH_ID = 0,
    ELDT_UserPath_Index_SCHEMA_ID,

    
    ELDT_UserPath_Index_MAX
    
} eldtIndexUSER_SCHEMA_PATH;

extern eldtDefinitionIndexDesc  gEldtIndexDescUSER_SCHEMA_PATH[ELDT_UserPath_Index_MAX];





/** @} eldtUSER_SCHEMA_PATH */



#endif /* _ELDT_USER_SCHEMA_PATH_H_ */
