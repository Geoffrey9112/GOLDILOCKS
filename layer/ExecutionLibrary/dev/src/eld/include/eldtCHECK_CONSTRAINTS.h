/*******************************************************************************
 * eldtCHECK_CONSTRAINTS.h
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


#ifndef _ELDT_CHECK_CONSTRAINTS_H_
#define _ELDT_CHECK_CONSTRAINTS_H_ 1

/**
 * @file eldtCHECK_CONSTRAINTS.h
 * @brief CHECK_CONSTRAINTS dictionary base table
 */

/**
 * @defgroup eldtCHECK_CONSTRAINTS CHECK_CONSTRAINTS table
 * @ingroup eldtStandard
 * @{
 */

#define ELDT_CHECK_CONSTRAINTS_INDEX_CONSTRAINT_ID  ( "CHECK_CONSTRAINTS_INDEX_CONSTRAINT_ID" )

/**
 * @brief CHECK_CONSTRAINTS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderCHECK_CONSTRAINTS
{
    /*
     * 식별자 영역
     */

    ELDT_CheckConst_ColumnOrder_CONSTRAINT_OWNER_ID = 0,
    ELDT_CheckConst_ColumnOrder_CONSTRAINT_SCHEMA_ID,
    ELDT_CheckConst_ColumnOrder_CONSTRAINT_ID,
    
    /*
     * SQL 표준 영역
     */

    /*
    ELDT_CheckConst_ColumnOrder_CONSTRAINT_CATALOG, 
    ELDT_CheckConst_ColumnOrder_CONSTRAINT_SCHEMA,
    ELDT_CheckConst_ColumnOrder_CONSTRAINT_NAME,
    */
    ELDT_CheckConst_ColumnOrder_CHECK_CLAUSE,
    
    /*
     * 부가 정보 영역
     */
    
    ELDT_CheckConst_ColumnOrder_MAX
    
} eldtColumnOrderCHECK_CONSTRAINTS;
    

extern eldtDefinitionTableDesc   gEldtTableDescCHECK_CONSTRAINTS;
extern eldtDefinitionColumnDesc  gEldtColumnDescCHECK_CONSTRAINTS[ELDT_CheckConst_ColumnOrder_MAX];


/**
 * @brief CHECK_CONSTRAINTS 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( CONSTRAINT_CATALOG, CONSTRAINT_SCHEMA, CONSTRAINT_NAME )
 *   - => PRIMARY KEY ( CONSTRAINT_SCHEMA_ID, CONSTRAINT_ID )
 * - 비표준
 *  
 * - TABLE_CONSTRAINTS, VIEWS 테이블에 종속적인 table 들은 상위 table들을 reference 해서는 안된다.
 *  - (CONSTRAINT_OWNER_ID), (CONSTRAINT_SCHEMA_ID)
 * - CONSTRAINT_ID 는 ASSERTIONS, TABLE_CONSTRAINTS, DOMAIN_CONSTRAINTS 중 하나이므로 foreign key 관계를 가질 수 없다.
 */

typedef enum eldtKeyConstCHECK_CONSTRAINTS
{
    /*
     * Primary Key 영역
     */

    ELDT_CheckConst_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    /*
     * Foreign Key 영역
     */

    ELDT_CheckConst_Const_MAX
    
} eldtKeyConstCHECK_CONSTRAINTS;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescCHECK_CONSTRAINTS[ELDT_CheckConst_Const_MAX];


/**
 * @brief CHECK_CONSTRAINTS 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexCHECK_CONSTRAINTS
{
    ELDT_CheckConst_Index_CONSTRAINT_OWNER_ID = 0,
    ELDT_CheckConst_Index_CONSTRAINT_SCHEMA_ID,
    ELDT_CheckConst_Index_CONSTRAINT_ID,
    
    ELDT_CheckConst_Index_MAX
    
} eldtIndexCHECK_CONSTRAINTS;

extern eldtDefinitionIndexDesc  gEldtIndexDescCHECK_CONSTRAINTS[ELDT_CheckConst_Index_MAX];


/** @} eldtCHECK_CONSTRAINTS */



#endif /* _ELDT_CHECK_CONSTRAINTS_H_ */
