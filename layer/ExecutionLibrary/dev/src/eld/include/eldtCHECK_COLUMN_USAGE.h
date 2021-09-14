/*******************************************************************************
 * eldtCHECK_COLUMN_USAGE.h
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


#ifndef _ELDT_CHECK_COLUMN_USAGE_H_
#define _ELDT_CHECK_COLUMN_USAGE_H_ 1

/**
 * @file eldtCHECK_COLUMN_USAGE.h
 * @brief CHECK_COLUMN_USAGE dictionary base table
 */

/**
 * @defgroup eldtCHECK_COLUMN_USAGE CHECK_COLUMN_USAGE table
 * @ingroup eldtStandard
 * @{
 */

#define ELDT_CHECK_COLUMN_USAGE_INDEX_CONSTRAINT_ID  ( "CHECK_COLUMN_USAGE_INDEX_CONSTRAINT_ID" )

/**
 * @brief CHECK_COLUMN_USAGE 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderCHECK_COLUMN_USAGE
{
    /*
     * 식별자 영역
     */

    ELDT_CheckColumn_ColumnOrder_CONSTRAINT_OWNER_ID = 0,
    ELDT_CheckColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,
    ELDT_CheckColumn_ColumnOrder_CONSTRAINT_ID,
    
    ELDT_CheckColumn_ColumnOrder_OWNER_ID,
    ELDT_CheckColumn_ColumnOrder_SCHEMA_ID,
    ELDT_CheckColumn_ColumnOrder_TABLE_ID,
    ELDT_CheckColumn_ColumnOrder_COLUMN_ID,
    
    /*
     * SQL 표준 영역
     */

    /*
    ELDT_CheckColumn_ColumnOrder_CONSTRAINT_CATALOG,
    ELDT_CheckColumn_ColumnOrder_CONSTRAINT_SCHEMA,
    ELDT_CheckColumn_ColumnOrder_CONSTRAINT_NAME,
    ELDT_CheckColumn_ColumnOrder_TABLE_CATALOG,
    ELDT_CheckColumn_ColumnOrder_TABLE_SCHEMA,
    ELDT_CheckColumn_ColumnOrder_TABLE_NAME,
    ELDT_CheckColumn_ColumnOrder_COLUMN_NAME,
    */
    
    /*
     * 부가 정보 영역
     */
    
    ELDT_CheckColumn_ColumnOrder_MAX
    
} eldtColumnOrderCHECK_COLUMN_USAGE;
    

extern eldtDefinitionTableDesc   gEldtTableDescCHECK_COLUMN_USAGE;
extern eldtDefinitionColumnDesc  gEldtColumnDescCHECK_COLUMN_USAGE[ELDT_CheckColumn_ColumnOrder_MAX];


/**
 * @brief CHECK_COLUMN_USAGE 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( CONSTRAINT_CATALOG, CONSTRAINT_SCHEMA, CONSTRAINT_NAME, TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME, COLUMN_NAME ),
 *   - => PRIMARY KEY (CONSTRAINT_SCHEMA_ID, CONSTRAINT_ID, SCHEMA_ID, TABLE_ID, COLUMN_ID)
 *  - SQL 2 : FOREIGN KEY ( CONSTRAINT_CATALOG, CONSTRAINT_SCHEMA, CONSTRAINT_NAME, TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME) REFERENCES CHECK_TABLE_USAGE,
 *   - => FOREIGN KEY ( CONSTRAINT_SCHEMA_ID, CONSTRAINT_ID, SCHEMA_ID, TABLE_ID ) REFERENCES CHECK_TABLE_USAGE
 * - 비표준
 */

typedef enum eldtKeyConstCHECK_COLUMN_USAGE
{
    /*
     * Primary Key 영역
     */

    ELDT_CheckColumn_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    /*
     * Foreign Key 영역
     */

    ELDT_CheckColumn_Const_FOREIGN_KEY_CHECK_TABLE_USAGE,  /**< SQL 2 */
    
    ELDT_CheckColumn_Const_MAX
    
} eldtKeyConstCHECK_COLUMN_USAGE;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescCHECK_COLUMN_USAGE[ELDT_CheckColumn_Const_MAX];


/**
 * @brief CHECK_COLUMN_USAGE 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexCHECK_COLUMN_USAGE
{
    ELDT_CheckColumn_Index_CONSTRAINT_OWNER_ID = 0,
    ELDT_CheckColumn_Index_CONSTRAINT_SCHEMA_ID,
    ELDT_CheckColumn_Index_CONSTRAINT_ID,
    
    ELDT_CheckColumn_Index_OWNER_ID,
    ELDT_CheckColumn_Index_SCHEMA_ID,
    ELDT_CheckColumn_Index_TABLE_ID,
    ELDT_CheckColumn_Index_COLUMN_ID,
    
    ELDT_CheckColumn_Index_MAX
    
} eldtIndexCHECK_COLUMN_USAGE;

extern eldtDefinitionIndexDesc  gEldtIndexDescCHECK_COLUMN_USAGE[ELDT_CheckColumn_Index_MAX];




/** @} eldtCHECK_COLUMN_USAGE */



#endif /* _ELDT_CHECK_COLUMN_USAGE_H_ */
