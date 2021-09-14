/*******************************************************************************
 * eldtTABLE_CONSTRAINTS.h
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


#ifndef _ELDT_TABLE_CONSTRAINTS_H_
#define _ELDT_TABLE_CONSTRAINTS_H_ 1

/**
 * @file eldtTABLE_CONSTRAINTS.h
 * @brief TABLE_CONSTRAINTS dictionary base table
 */

/**
 * @defgroup eldtTABLE_CONSTRAINTS TABLE_CONSTRAINTS table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief TABLE_CONSTRAINTS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderTABLE_CONSTRAINTS
{
    /*
     * 식별자 영역
     */
    
    ELDT_TableConst_ColumnOrder_CONSTRAINT_OWNER_ID = 0,
    ELDT_TableConst_ColumnOrder_CONSTRAINT_SCHEMA_ID,
    ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
    ELDT_TableConst_ColumnOrder_OWNER_ID,
    ELDT_TableConst_ColumnOrder_SCHEMA_ID,
    ELDT_TableConst_ColumnOrder_TABLE_ID,

    /*
     * SQL 표준 영역
     */

    /*
    ELDT_TableConst_ColumnOrder_CONSTRAINT_CATALOG,
    ELDT_TableConst_ColumnOrder_CONSTRAINT_SCHEMA,
    */
    ELDT_TableConst_ColumnOrder_CONSTRAINT_NAME,
    ELDT_TableConst_ColumnOrder_CONSTRAINT_TYPE,
    ELDT_TableConst_ColumnOrder_CONSTRAINT_TYPE_ID,  /* 비표준 */

    /*
    ELDT_TableConst_ColumnOrder_TABLE_CATALOG ,
    ELDT_TableConst_ColumnOrder_TABLE_SCHEMA,
    ELDT_TableConst_ColumnOrder_TABLE_NAME,
    */
    
    ELDT_TableConst_ColumnOrder_IS_DEFERRABLE,
    ELDT_TableConst_ColumnOrder_INITIALLY_DEFERRED,
    ELDT_TableConst_ColumnOrder_ENFORCED,
    
    /*
     * 부가 정보 영역
     */

    ELDT_TableConst_ColumnOrder_ASSOCIATED_INDEX_ID,
    ELDT_TableConst_ColumnOrder_VALIDATE,
    ELDT_TableConst_ColumnOrder_IS_BUILTIN,
    
    ELDT_TableConst_ColumnOrder_CREATED_TIME,
    ELDT_TableConst_ColumnOrder_MODIFIED_TIME,
    ELDT_TableConst_ColumnOrder_COMMENTS,

    ELDT_TableConst_ColumnOrder_MAX

} eldtColumnOrderTABLE_CONSTRAINTS;


extern eldtDefinitionTableDesc   gEldtTableDescTABLE_CONSTRAINTS;
extern eldtDefinitionColumnDesc  gEldtColumnDescTABLE_CONSTRAINTS[ELDT_TableConst_ColumnOrder_MAX];


/**
 * @brief TABLE_CONSTRAINTS 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( CONSTRAINT_CATALOG, CONSTRAINT_SCHEMA, CONSTRAINT_NAME )
 *   - => PRIMARY KEY (CONSTRAINT_SCHEMA_ID, CONSTRAINT_ID)
 * - 비표준
 *  - 비표준 1 : UNIQUE (CONSTRAINT_SCHEMA_ID, CONSTRAINT_NAME)
 */

typedef enum eldtKeyConstTABLE_CONSTRAINTS
{
    /*
     * Primary Key 영역
     */

    ELDT_TableConst_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    ELDT_TableConst_Const_UNIQUE_CONSTRAINT_SCHEMA_ID_CONSTRAINT_NAME,   /**< 비표준 1 */
    
    /*
     * Foreign Key 영역
     */
    
    ELDT_TableConst_Const_MAX
    
} eldtKeyConstTABLE_CONSTRAINTS;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescTABLE_CONSTRAINTS[ELDT_TableConst_Const_MAX];



/**
 * @brief TABLE_CONSTRAINTS 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexTABLE_CONSTRAINTS
{
    ELDT_TableConst_Index_CONSTRAINT_OWNER_ID = 0,
    ELDT_TableConst_Index_CONSTRAINT_SCHEMA_ID,
    ELDT_TableConst_Index_CONSTRAINT_ID,
    ELDT_TableConst_Index_OWNER_ID,
    ELDT_TableConst_Index_SCHEMA_ID,
    ELDT_TableConst_Index_TABLE_ID,
    ELDT_TableConst_Index_CONSTRAINT_NAME,
    ELDT_TableConst_Index_ASSOCIATED_INDEX_ID,
    
    ELDT_TableConst_Index_MAX
    
} eldtIndexTABLE_CONSTRAINTS;

extern eldtDefinitionIndexDesc  gEldtIndexDescTABLE_CONSTRAINTS[ELDT_TableConst_Index_MAX];

/** @} eldtTABLE_CONSTRAINTS */



#endif /* _ELDT_TABLE_CONSTRAINTS_H_ */
