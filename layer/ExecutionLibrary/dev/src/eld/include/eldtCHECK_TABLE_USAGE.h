/*******************************************************************************
 * eldtCHECK_TABLE_USAGE.h
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


#ifndef _ELDT_CHECK_TABLE_USAGE_H_
#define _ELDT_CHECK_TABLE_USAGE_H_ 1

/**
 * @file eldtCHECK_TABLE_USAGE.h
 * @brief CHECK_TABLE_USAGE dictionary base table
 */

/**
 * @defgroup eldtCHECK_TABLE_USAGE CHECK_TABLE_USAGE table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief CHECK_TABLE_USAGE 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderCHECK_TABLE_USAGE
{
    /*
     * 식별자 영역
     */

    ELDT_CheckTable_ColumnOrder_CONSTRAINT_OWNER_ID = 0,
    ELDT_CheckTable_ColumnOrder_CONSTRAINT_SCHEMA_ID,
    ELDT_CheckTable_ColumnOrder_CONSTRAINT_ID,
    
    ELDT_CheckTable_ColumnOrder_OWNER_ID,
    ELDT_CheckTable_ColumnOrder_SCHEMA_ID,
    ELDT_CheckTable_ColumnOrder_TABLE_ID,
    
    /*
     * SQL 표준 영역
     */

    /*
    ELDT_CheckTable_ColumnOrder_CONSTRAINT_CATALOG,
    ELDT_CheckTable_ColumnOrder_CONSTRAINT_SCHEMA,
    ELDT_CheckTable_ColumnOrder_CONSTRAINT_NAME,
    ELDT_CheckTable_ColumnOrder_TABLE_CATALOG,
    ELDT_CheckTable_ColumnOrder_TABLE_SCHEMA,
    ELDT_CheckTable_ColumnOrder_TABLE_NAME,
    */
    
    /*
     * 부가 정보 영역
     */
    
    ELDT_CheckTable_ColumnOrder_MAX
    
} eldtColumnOrderCHECK_TABLE_USAGE;
    

extern eldtDefinitionTableDesc   gEldtTableDescCHECK_TABLE_USAGE;
extern eldtDefinitionColumnDesc  gEldtColumnDescCHECK_TABLE_USAGE[ELDT_CheckTable_ColumnOrder_MAX];


/**
 * @brief CHECK_TABLE_USAGE 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( CONSTRAINT_CATALOG, CONSTRAINT_SCHEMA, CONSTRAINT_NAME, TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME )
 *   - => PRIMARY KEY (CONSTRAINT_SCHEMA_ID, CONSTRAINT_ID, SCHEMA_ID, TABLE_ID)
 *  - SQL 2 : FOREIGN KEY ( CONSTRAINT_CATALOG, CONSTRAINT_SCHEMA, CONSTRAINT_NAME ) REFERENCES CHECK_CONSTRAINTS
 *   - => FOREIGN KEY (CONSTRAINT_SCHEMA_ID, CONSTRAINT_ID) REFERENCES CHECK_CONSTRAINTS
 * - 비표준
 */

typedef enum eldtKeyConstCHECK_TABLE_USAGE
{
    /*
     * Primary Key 영역
     */

    ELDT_CheckTable_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    /*
     * Foreign Key 영역
     */

    ELDT_CheckTable_Const_FOREIGN_KEY_CHECK_CONSTRAINTS,  /**< SQL 2 */
    
    ELDT_CheckTable_Const_MAX
    
} eldtKeyConstCHECK_TABLE_USAGE;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescCHECK_TABLE_USAGE[ELDT_CheckTable_Const_MAX];


/**
 * @brief CHECK_TABLE_USAGE 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexCHECK_TABLE_USAGE
{
    ELDT_CheckTable_Index_CONSTRAINT_OWNER_ID = 0,
    ELDT_CheckTable_Index_CONSTRAINT_SCHEMA_ID,
    ELDT_CheckTable_Index_CONSTRAINT_ID,
    
    ELDT_CheckTable_Index_OWNER_ID,
    ELDT_CheckTable_Index_SCHEMA_ID,
    ELDT_CheckTable_Index_TABLE_ID,
    
    ELDT_CheckTable_Index_MAX
    
} eldtIndexCHECK_TABLE_USAGE;

extern eldtDefinitionIndexDesc  gEldtIndexDescCHECK_TABLE_USAGE[ELDT_CheckTable_Index_MAX];

/** @} eldtCHECK_TABLE_USAGE */



#endif /* _ELDT_CHECK_TABLE_USAGE_H_ */
