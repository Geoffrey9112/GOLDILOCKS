/*******************************************************************************
 * eldtTABLES.h
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


#ifndef _ELDT_TABLES_H_
#define _ELDT_TABLES_H_ 1

/**
 * @file eldtTABLES.h
 * @brief TABLES dictionary base table
 */

/**
 * @defgroup eldtTABLES TABLES table
 * @ingroup eldtCore
 * @{
 */

/**
 * @brief TABLES 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderTABLES
{
    /*
     * 식별자 영역
     */
    
    ELDT_Tables_ColumnOrder_OWNER_ID = 0,
    ELDT_Tables_ColumnOrder_SCHEMA_ID,
    ELDT_Tables_ColumnOrder_TABLE_ID,
    ELDT_Tables_ColumnOrder_TABLESPACE_ID,
    ELDT_Tables_ColumnOrder_PHYSICAL_ID,

    /*
     * SQL 표준 영역
     */

    /*
    ELDT_Tables_ColumnOrder_TABLE_CATALOG, 
    ELDT_Tables_ColumnOrder_TABLE_SCHEMA,
    */
    ELDT_Tables_ColumnOrder_TABLE_NAME,
    ELDT_Tables_ColumnOrder_TABLE_TYPE,
    ELDT_Tables_ColumnOrder_TABLE_TYPE_ID,   /* 비표준 */
    ELDT_Tables_ColumnOrder_SYSTEM_VERSION_START_COLUMN_NAME,
    ELDT_Tables_ColumnOrder_SYSTEM_VERSION_END_COLUMN_NAME,
    ELDT_Tables_ColumnOrder_SYSTEM_VERSION_RETENTION_PERIOD,
    ELDT_Tables_ColumnOrder_SELF_REFERENCING_COLUMN_NAME,
    ELDT_Tables_ColumnOrder_REFERENCE_GENERATION,
    ELDT_Tables_ColumnOrder_USER_DEFINED_TYPE_CATALOG, 
    ELDT_Tables_ColumnOrder_USER_DEFINED_TYPE_SCHEMA,
    ELDT_Tables_ColumnOrder_USER_DEFINED_TYPE_NAME,
    ELDT_Tables_ColumnOrder_IS_INSERTABLE_INTO,
    ELDT_Tables_ColumnOrder_IS_TYPED,
    ELDT_Tables_ColumnOrder_COMMIT_ACTION,
    
    /*
     * 부가 정보 영역
     */

    ELDT_Tables_ColumnOrder_IS_SET_SUPPLOG_PK,
    ELDT_Tables_ColumnOrder_IS_BUILTIN,
    
    ELDT_Tables_ColumnOrder_CREATED_TIME,
    ELDT_Tables_ColumnOrder_MODIFIED_TIME,
    ELDT_Tables_ColumnOrder_COMMENTS,

    ELDT_Tables_ColumnOrder_MAX

} eldtColumnOrderTABLES;


extern eldtDefinitionTableDesc   gEldtTableDescTABLES;
extern eldtDefinitionColumnDesc  gEldtColumnDescTABLES[ELDT_Tables_ColumnOrder_MAX];


/**
 * @brief TABLES 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME )
 *   - => PRIMARY KEY (SCHEMA_ID, TABLE_ID)
 *  - SQL 2 : FOREIGN KEY ( TABLE_CATALOG, TABLE_SCHEMA ) REFERENCES SCHEMATA,
 *   - => FOREIGN KEY (SCHEMA_ID) REFERENCES SCHEMATA
 *  - SQL 3 : FOREIGN KEY ( USER_DEFINED_TYPE_CATALOG, USER_DEFINED_TYPE_SCHEMA, USER_DEFINED_TYPE_NAME ) REFERENCES USER_DEFINED_TYPES MATCH FULL,
 *   - => 생성하지 않음 
 * - 비표준
 *  - 비표준 1 : UNIQUE (SCHEMA_ID, TABLE_NAME)
 */

typedef enum eldtKeyConstTABLES
{
    /*
     * Primary Key 영역
     */

    ELDT_Tables_Const_PRIMARY_KEY = 0,                /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    ELDT_Tables_Const_UNIQUE_SCHEMA_ID_TABLE_NAME,   /**< 비표준 1 */
    
    /*
     * Foreign Key 영역
     */

    ELDT_Tables_Const_FOREIGN_KEY_SCHEMATA,          /**< SQL 2 */
    
    ELDT_Tables_Const_MAX
    
} eldtKeyConstTABLES;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescTABLES[ELDT_Tables_Const_MAX];

/**
 * @brief TABLES 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexTABLES
{
    ELDT_Tables_Index_OWNER_ID = 0,
    /*
     * ELDT_Tables_Index_SCHEMA_ID, Foreign Key 가 존재함
     */
    ELDT_Tables_Index_TABLE_ID,
    ELDT_Tables_Index_TABLESPACE_ID,
    ELDT_Tables_Index_PHYSICAL_ID,
    ELDT_Tables_Index_TABLE_NAME,
    
    ELDT_Tables_Index_MAX
    
} eldtIndexTABLES;

extern eldtDefinitionIndexDesc  gEldtIndexDescTABLES[ELDT_Tables_Index_MAX];


/** @} eldtTABLES */



#endif /* _ELDT_TABLES_H_ */
