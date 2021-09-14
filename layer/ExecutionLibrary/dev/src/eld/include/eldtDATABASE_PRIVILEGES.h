/*******************************************************************************
 * eldtDATABASE_PRIVILEGES.h
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


#ifndef _ELDT_DATABASE_PRIVILEGES_H_
#define _ELDT_DATABASE_PRIVILEGES_H_ 1

/**
 * @file eldtDATABASE_PRIVILEGES.h
 * @brief DATABASE_PRIVILEGES dictionary base table
 */

/**
 * @defgroup eldtDATABASE_PRIVILEGES DATABASE_PRIVILEGES table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief DATABASE_PRIVILEGES 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderDATABASE_PRIVILEGES
{
    /*
     * 식별자 영역
     */

    ELDT_DBPriv_ColumnOrder_GRANTOR_ID = 0,
    ELDT_DBPriv_ColumnOrder_GRANTEE_ID,
    
    /*
     * SQL 표준 영역
     */

    /*
     * 부가 정보 영역
     */

    ELDT_DBPriv_ColumnOrder_PRIVILEGE_TYPE,
    ELDT_DBPriv_ColumnOrder_PRIVILEGE_TYPE_ID, 
    ELDT_DBPriv_ColumnOrder_IS_GRANTABLE,

    ELDT_DBPriv_ColumnOrder_IS_BUILTIN,
    
    ELDT_DBPriv_ColumnOrder_MAX
    
} eldtColumnOrderDATABASE_PRIVILEGES;
    

extern eldtDefinitionTableDesc   gEldtTableDescDATABASE_PRIVILEGES;
extern eldtDefinitionColumnDesc  gEldtColumnDescDATABASE_PRIVILEGES[ELDT_DBPriv_ColumnOrder_MAX];



/**
 * @brief DATABASE_PRIVILEGES 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 * - 비표준
 *  - 비표준 1 : PRIMARY KEY ( GRANTOR_ID, GRANTEE_ID, PRIVILEGE_TYPE_ID ) 
 */

typedef enum eldtKeyConstDATABASE_PRIVILEGES
{
    /*
     * Primary Key 영역
     */

    ELDT_DBPriv_Const_PRIMARY_KEY = 0,                  /**< 비표준 1 */
    
    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    ELDT_DBPriv_Const_MAX
    
} eldtKeyConstDATABASE_PRIVILEGES;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescDATABASE_PRIVILEGES[ELDT_DBPriv_Const_MAX];


/**
 * @brief DATABASE_PRIVILEGES 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexDATABASE_PRIVILEGES
{
    ELDT_DBPriv_Index_GRANTOR_ID = 0,
    ELDT_DBPriv_Index_GRANTEE_ID,

    ELDT_DBPriv_Index_MAX
    
} eldtIndexDATABASE_PRIVILEGES;

extern eldtDefinitionIndexDesc  gEldtIndexDescDATABASE_PRIVILEGES[ELDT_DBPriv_Index_MAX];



/** @} eldtDATABASE_PRIVILEGES */



#endif /* _ELDT_DATABASE_PRIVILEGES_H_ */
