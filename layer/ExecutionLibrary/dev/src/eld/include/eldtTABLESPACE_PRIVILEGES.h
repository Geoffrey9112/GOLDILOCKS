/*******************************************************************************
 * eldtTABLESPACE_PRIVILEGES.h
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


#ifndef _ELDT_TABLESPACE_PRIVILEGES_H_
#define _ELDT_TABLESPACE_PRIVILEGES_H_ 1

/**
 * @file eldtTABLESPACE_PRIVILEGES.h
 * @brief TABLESPACE_PRIVILEGES dictionary base table
 */

/**
 * @defgroup eldtTABLESPACE_PRIVILEGES TABLESPACE_PRIVILEGES table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief TABLESPACE_PRIVILEGES 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderTABLESPACE_PRIVILEGES
{
    /*
     * 식별자 영역
     */

    ELDT_SpacePriv_ColumnOrder_GRANTOR_ID = 0,
    ELDT_SpacePriv_ColumnOrder_GRANTEE_ID,
    ELDT_SpacePriv_ColumnOrder_TABLESPACE_ID,
    
    /*
     * SQL 표준 영역
     */
    
    /*
     * 부가 정보 영역
     */

    ELDT_SpacePriv_ColumnOrder_PRIVILEGE_TYPE,
    ELDT_SpacePriv_ColumnOrder_PRIVILEGE_TYPE_ID,  
    ELDT_SpacePriv_ColumnOrder_IS_GRANTABLE,
    ELDT_SpacePriv_ColumnOrder_IS_BUILTIN,
    
    ELDT_SpacePriv_ColumnOrder_MAX
    
} eldtColumnOrderTABLESPACE_PRIVILEGES;
    

extern eldtDefinitionTableDesc   gEldtTableDescTABLESPACE_PRIVILEGES;
extern eldtDefinitionColumnDesc  gEldtColumnDescTABLESPACE_PRIVILEGES[ELDT_SpacePriv_ColumnOrder_MAX];



/**
 * @brief TABLESPACE_PRIVILEGES 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 * - 비표준
 *  - 비표준 1 : PRIMARY KEY ( GRANTOR_ID, GRANTEE_ID, TABLESPACE_ID, PRIVILEGE_TYPE_ID ) 
 */

typedef enum eldtKeyConstTABLESPACE_PRIVILEGES
{
    /*
     * Primary Key 영역
     */

    ELDT_SpacePriv_Const_PRIMARY_KEY = 0,                  /**< 비표준 1 */
    
    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    ELDT_SpacePriv_Const_MAX
    
} eldtKeyConstTABLESPACE_PRIVILEGES;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescTABLESPACE_PRIVILEGES[ELDT_SpacePriv_Const_MAX];



/**
 * @brief TABLESPACE_PRIVILEGES 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexTABLESPACE_PRIVILEGES
{
    ELDT_SpacePriv_Index_GRANTOR_ID = 0,
    ELDT_SpacePriv_Index_GRANTEE_ID,
    ELDT_SpacePriv_Index_TABLESPACE_ID,

    ELDT_SpacePriv_Index_MAX
    
} eldtIndexTABLESPACE_PRIVILEGES;

extern eldtDefinitionIndexDesc  gEldtIndexDescTABLESPACE_PRIVILEGES[ELDT_SpacePriv_Index_MAX];





/** @} eldtTABLESPACE_PRIVILEGES */



#endif /* _ELDT_TABLESPACE_PRIVILEGES_H_ */
