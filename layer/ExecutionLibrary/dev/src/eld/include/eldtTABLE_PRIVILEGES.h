/*******************************************************************************
 * eldtTABLE_PRIVILEGES.h
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


#ifndef _ELDT_TABLE_PRIVILEGES_H_
#define _ELDT_TABLE_PRIVILEGES_H_ 1

/**
 * @file eldtTABLE_PRIVILEGES.h
 * @brief TABLE_PRIVILEGES dictionary base table
 */

/**
 * @defgroup eldtTABLE_PRIVILEGES TABLE_PRIVILEGES table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief TABLE_PRIVILEGES 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderTABLE_PRIVILEGES
{
    /*
     * 식별자 영역
     */

    ELDT_TablePriv_ColumnOrder_GRANTOR_ID = 0,
    ELDT_TablePriv_ColumnOrder_GRANTEE_ID,
    ELDT_TablePriv_ColumnOrder_SCHEMA_ID,
    ELDT_TablePriv_ColumnOrder_TABLE_ID,
    
    /*
     * SQL 표준 영역
     */

    /*
    ELDT_TablePriv_ColumnOrder_GRANTOR,
    ELDT_TablePriv_ColumnOrder_GRANTEE,
    ELDT_TablePriv_ColumnOrder_TABLE_CATALOG,
    ELDT_TablePriv_ColumnOrder_TABLE_SCHEMA,
    ELDT_TablePriv_ColumnOrder_TABLE_NAME,
    */
    ELDT_TablePriv_ColumnOrder_PRIVILEGE_TYPE,
    ELDT_TablePriv_ColumnOrder_PRIVILEGE_TYPE_ID,     /* 비표준 */
    ELDT_TablePriv_ColumnOrder_IS_GRANTABLE,
    ELDT_TablePriv_ColumnOrder_WITH_HIERARCHY,
    ELDT_TablePriv_ColumnOrder_IS_BUILTIN,
    
    
    /*
     * 부가 정보 영역
     */
    
    ELDT_TablePriv_ColumnOrder_MAX
    
} eldtColumnOrderTABLE_PRIVILEGES;
    

extern eldtDefinitionTableDesc   gEldtTableDescTABLE_PRIVILEGES;
extern eldtDefinitionColumnDesc  gEldtColumnDescTABLE_PRIVILEGES[ELDT_TablePriv_ColumnOrder_MAX];



/**
 * @brief TABLE_PRIVILEGES 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( GRANTOR, GRANTEE, TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME, PRIVILEGE_TYPE )
 *   - => : PRIMARY KEY ( GRANTOR_ID, GRANTEE_ID, SCHEMA_ID, TABLE_ID, PRIVILEGE_TYPE )
 *  - SQL 2 : FOREIGN KEY ( TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME ) REFERENCES TABLES,
 *   - => : FOREIGN KEY ( SCHEMA_ID, TABLE_ID ) REFERENCES TABLES
 * - 비표준
 */

typedef enum eldtKeyConstTABLE_PRIVILEGES
{
    /*
     * Primary Key 영역
     */

    ELDT_TablePriv_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    ELDT_TablePriv_Const_FOREIGN_KEY_TABLES,          /**< SQL 2 */
    
    ELDT_TablePriv_Const_MAX
    
} eldtKeyConstTABLE_PRIVILEGES;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescTABLE_PRIVILEGES[ELDT_TablePriv_Const_MAX];


/**
 * @brief TABLE_PRIVILEGES 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexTABLE_PRIVILEGES
{
    ELDT_TablePriv_Index_GRANTOR_ID = 0,
    ELDT_TablePriv_Index_GRANTEE_ID,
    ELDT_TablePriv_Index_SCHEMA_ID,
    ELDT_TablePriv_Index_TABLE_ID,

    ELDT_TablePriv_Index_MAX
    
} eldtIndexTABLE_PRIVILEGES;

extern eldtDefinitionIndexDesc  gEldtIndexDescTABLE_PRIVILEGES[ELDT_TablePriv_Index_MAX];


/** @} eldtTABLE_PRIVILEGES */



#endif /* _ELDT_TABLE_PRIVILEGES_H_ */
