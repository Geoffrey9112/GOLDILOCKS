/*******************************************************************************
 * eldtKEY_COLUMN_USAGE.h
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


#ifndef _ELDT_KEY_COLUMN_USAGE_H_
#define _ELDT_KEY_COLUMN_USAGE_H_ 1

/**
 * @file eldtKEY_COLUMN_USAGE.h
 * @brief KEY_COLUMN_USAGE dictionary base table
 */

/**
 * @defgroup eldtKEY_COLUMN_USAGE KEY_COLUMN_USAGE table
 * @ingroup eldtStandard
 * @{
 */

#define ELDT_KEY_COLUMN_USAGE_INDEX_CONSTRAINT_ID  ( "KEY_COLUMN_USAGE_INDEX_CONSTRAINT_ID" )

/**
 * @brief KEY_COLUMN_USAGE 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderKEY_COLUMN_USAGE
{
    /*
     * 식별자 영역
     */

    ELDT_KeyColumn_ColumnOrder_CONSTRAINT_OWNER_ID,
    ELDT_KeyColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,
    ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID,

    ELDT_KeyColumn_ColumnOrder_OWNER_ID,
    ELDT_KeyColumn_ColumnOrder_SCHEMA_ID,
    ELDT_KeyColumn_ColumnOrder_TABLE_ID,
    ELDT_KeyColumn_ColumnOrder_COLUMN_ID,
    
    /*
     * SQL 표준 영역
     */

    /*
    ELDT_KeyColumn_ColumnOrder_CONSTRAINT_CATALOG,
    ELDT_KeyColumn_ColumnOrder_CONSTRAINT_SCHEMA,
    ELDT_KeyColumn_ColumnOrder_CONSTRAINT_NAME,
    ELDT_KeyColumn_ColumnOrder_TABLE_CATALOG,
    ELDT_KeyColumn_ColumnOrder_TABLE_SCHEMA,
    ELDT_KeyColumn_ColumnOrder_TABLE_NAME,
    ELDT_KeyColumn_ColumnOrder_COLUMN_NAME,
    */
    
    ELDT_KeyColumn_ColumnOrder_ORDINAL_POSITION,
    ELDT_KeyColumn_ColumnOrder_POSITION_IN_UNIQUE_CONSTRAINT,
    
    /*
     * 부가 정보 영역
     */
    
    ELDT_KeyColumn_ColumnOrder_MAX
    
} eldtColumnOrderKEY_COLUMN_USAGE;
    

extern eldtDefinitionTableDesc   gEldtTableDescKEY_COLUMN_USAGE;
extern eldtDefinitionColumnDesc  gEldtColumnDescKEY_COLUMN_USAGE[ELDT_KeyColumn_ColumnOrder_MAX];



/**
 * @brief KEY_COLUMN_USAGE 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( CONSTRAINT_CATALOG, CONSTRAINT_SCHEMA, CONSTRAINT_NAME, COLUMN_NAME )
 *   - => PRIMARY KEY ( CONSTRAINT_SCHEMA_ID, CONSTRAINT_ID, COLUMN_ID )
 *  - SQL 2 : UNIQUE ( CONSTRAINT_CATALOG, CONSTRAINT_SCHEMA, CONSTRAINT_NAME, POSITION_IN_UNIQUE_CONSTRAINT )
 *   - => UNIQUE ( CONSTRAINT_SCHEMA_ID, CONSTRAINT_ID, POSITION_IN_UNIQUE_CONSTRAINT )
 *  - SQL 3 : UNIQUE ( CONSTRAINT_CATALOG, CONSTRAINT_SCHEMA, CONSTRAINT_NAME, ORDINAL_POSITION ),
 *   - => UNIQUE ( CONSTRAINT_SCHEMA_ID, CONSTRAINT_ID, ORDINAL_POSITION )
 *  - SQL 4 : FOREIGN KEY ( TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME, COLUMN_NAME ) REFERENCES COLUMNS
 *   - => FOREIGN KEY ( SCHEMA_ID, TABLE_ID, COLUMN_ID ) REFERENCES COLUMNS
 * - 비표준
 */

typedef enum eldtKeyConstKEY_COLUMN_USAGE
{
    /*
     * Primary Key 영역
     */

    ELDT_KeyColumn_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    // ELDT_KeyColumn_Const_UNIQUE_XXXX_POSITION_IN_UNIQUE_CONSTRAINT,  /**< SQL 2 */
    ELDT_KeyColumn_Const_UNIQUE_XXXX_ORDINAL_POSITION,               /**< SQL 3 */
    
    /*
     * Foreign Key 영역
     */

    ELDT_KeyColumn_Const_FOREIGN_KEY_COLUMNS,         /**< SQL 4 */
    
    ELDT_KeyColumn_Const_MAX
    
} eldtKeyConstKEY_COLUMN_USAGE;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescKEY_COLUMN_USAGE[ELDT_KeyColumn_Const_MAX];



/**
 * @brief KEY_COLUMN_USAGE 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexKEY_COLUMN_USAGE
{
    ELDT_KeyColumn_Index_CONSTRAINT_OWNER_ID = 0,
    ELDT_KeyColumn_Index_CONSTRAINT_SCHEMA_ID,
    ELDT_KeyColumn_Index_CONSTRAINT_ID,
    
    ELDT_KeyColumn_Index_OWNER_ID,
    ELDT_KeyColumn_Index_SCHEMA_ID,
    ELDT_KeyColumn_Index_TABLE_ID,
    ELDT_KeyColumn_Index_COLUMN_ID,

    ELDT_KeyColumn_Index_MAX
    
} eldtIndexKEY_COLUMN_USAGE;

extern eldtDefinitionIndexDesc  gEldtIndexDescKEY_COLUMN_USAGE[ELDT_KeyColumn_Index_MAX];




/** @} eldtKEY_COLUMN_USAGE */



#endif /* _ELDT_KEY_COLUMN_USAGE_H_ */
