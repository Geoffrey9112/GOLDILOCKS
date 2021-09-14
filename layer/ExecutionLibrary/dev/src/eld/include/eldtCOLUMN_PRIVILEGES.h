/*******************************************************************************
 * eldtCOLUMN_PRIVILEGES.h
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


#ifndef _ELDT_COLUMN_PRIVILEGES_H_
#define _ELDT_COLUMN_PRIVILEGES_H_ 1

/**
 * @file eldtCOLUMN_PRIVILEGES.h
 * @brief COLUMN_PRIVILEGES dictionary base table
 */

/**
 * @defgroup eldtCOLUMN_PRIVILEGES COLUMN_PRIVILEGES table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief COLUMN_PRIVILEGES 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderCOLUMN_PRIVILEGES
{
    /*
     * 식별자 영역
     */

    ELDT_ColumnPriv_ColumnOrder_GRANTOR_ID = 0,
    ELDT_ColumnPriv_ColumnOrder_GRANTEE_ID,
    ELDT_ColumnPriv_ColumnOrder_SCHEMA_ID,
    ELDT_ColumnPriv_ColumnOrder_TABLE_ID,
    ELDT_ColumnPriv_ColumnOrder_COLUMN_ID,
    
    /*
     * SQL 표준 영역
     */

    /*
    ELDT_ColumnPriv_ColumnOrder_GRANTOR,
    ELDT_ColumnPriv_ColumnOrder_GRANTEE,
    ELDT_ColumnPriv_ColumnOrder_TABLE_CATALOG,
    ELDT_ColumnPriv_ColumnOrder_TABLE_SCHEMA,
    ELDT_ColumnPriv_ColumnOrder_TABLE_NAME,
    ELDT_ColumnPriv_ColumnOrder_COLUMN_NAME,
    */
    
    ELDT_ColumnPriv_ColumnOrder_PRIVILEGE_TYPE,
    ELDT_ColumnPriv_ColumnOrder_PRIVILEGE_TYPE_ID,   /* 비표준 */
    ELDT_ColumnPriv_ColumnOrder_IS_GRANTABLE,
    ELDT_ColumnPriv_ColumnOrder_IS_BUILTIN,
    
    /*
     * 부가 정보 영역
     */
    
    ELDT_ColumnPriv_ColumnOrder_MAX
    
} eldtColumnOrderCOLUMN_PRIVILEGES;
    

extern eldtDefinitionTableDesc   gEldtTableDescCOLUMN_PRIVILEGES;
extern eldtDefinitionColumnDesc  gEldtColumnDescCOLUMN_PRIVILEGES[ELDT_ColumnPriv_ColumnOrder_MAX];


/**
 * @brief COLUMN_PRIVILEGES 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( GRANTOR, GRANTEE, TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME, PRIVILEGE_TYPE, COLUMN_NAME )
 *   - => : PRIMARY KEY ( GRANTOR_ID, GRANTEE_ID, SCHEMA_ID, TABLE_ID, PRIVILEGE_TYPE, COLUMN_ID )
 *  - SQL 2 : FOREIGN KEY ( TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME, COLUMN_NAME ) REFERENCES COLUMNS,
 *   - => : FOREIGN KEY ( SCHEMA_ID, TABLE_ID, COLUMN_ID ) REFERENCES COLUMNS
 * - 비표준
 */

typedef enum eldtKeyConstCOLUMN_PRIVILEGES
{
    /*
     * Primary Key 영역
     */

    ELDT_ColumnPriv_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    ELDT_ColumnPriv_Const_FOREIGN_KEY_COLUMNS,          /**< SQL 2 */
    
    ELDT_ColumnPriv_Const_MAX
    
} eldtKeyConstCOLUMN_PRIVILEGES;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescCOLUMN_PRIVILEGES[ELDT_ColumnPriv_Const_MAX];


/**
 * @brief COLUMN_PRIVILEGES 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexCOLUMN_PRIVILEGES
{
    ELDT_ColumnPriv_Index_GRANTOR_ID = 0,
    ELDT_ColumnPriv_Index_GRANTEE_ID,
    ELDT_ColumnPriv_Index_SCHEMA_ID,
    ELDT_ColumnPriv_Index_TABLE_ID,
    ELDT_ColumnPriv_Index_COLUMN_ID,

    ELDT_ColumnPriv_Index_MAX
    
} eldtIndexCOLUMN_PRIVILEGES;

extern eldtDefinitionIndexDesc  gEldtIndexDescCOLUMN_PRIVILEGES[ELDT_ColumnPriv_Index_MAX];


/** @} eldtCOLUMN_PRIVILEGES */



#endif /* _ELDT_COLUMN_PRIVILEGES_H_ */
