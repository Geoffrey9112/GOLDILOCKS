/*******************************************************************************
 * eldtREFERENTIAL_CONSTRAINTS.h
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


#ifndef _ELDT_REFERENTIAL_CONSTRAINTS_H_
#define _ELDT_REFERENTIAL_CONSTRAINTS_H_ 1

/**
 * @file eldtREFERENTIAL_CONSTRAINTS.h
 * @brief REFERENTIAL_CONSTRAINTS dictionary base table
 */

/**
 * @defgroup eldtREFERENTIAL_CONSTRAINTS REFERENTIAL_CONSTRAINTS table
 * @ingroup eldtStandard
 * @{
 */

#define ELDT_REFERENTIAL_CONSTRAINTS_INDEX_CONSTRAINT_ID  ( "REFERENTIAL_CONSTRAINTS_INDEX_CONSTRAINT_ID" )

/**
 * @brief REFERENTIAL_CONSTRAINTS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderREFERENTIAL_CONSTRAINTS
{
    /*
     * 식별자 영역
     */

    ELDT_Reference_ColumnOrder_CONSTRAINT_OWNER_ID = 0,
    ELDT_Reference_ColumnOrder_CONSTRAINT_SCHEMA_ID,
    ELDT_Reference_ColumnOrder_CONSTRAINT_ID,
    ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_OWNER_ID,
    ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_SCHEMA_ID,
    ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_ID,
    
    
    /*
     * SQL 표준 영역
     */

    /*
    ELDT_Reference_ColumnOrder_CONSTRAINT_CATALOG,
    ELDT_Reference_ColumnOrder_CONSTRAINT_SCHEMA,
    ELDT_Reference_ColumnOrder_CONSTRAINT_NAME,
    ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_CATALOG,
    ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_SCHEMA,
    ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_NAME,
    */
    
    ELDT_Reference_ColumnOrder_MATCH_OPTION,
    ELDT_Reference_ColumnOrder_MATCH_OPTION_ID,  /* 비표준 */
    ELDT_Reference_ColumnOrder_UPDATE_RULE,
    ELDT_Reference_ColumnOrder_UPDATE_RULE_ID,   /* 비표준 */
    ELDT_Reference_ColumnOrder_DELETE_RULE,
    ELDT_Reference_ColumnOrder_DELETE_RULE_ID,   /* 비표준 */

    /*
     * 부가 정보 영역
     */

    ELDT_Reference_ColumnOrder_MAX
    
} eldtColumnOrderREFERENTIAL_CONSTRAINTS;
    

extern eldtDefinitionTableDesc   gEldtTableDescREFERENTIAL_CONSTRAINTS;
extern eldtDefinitionColumnDesc  gEldtColumnDescREFERENTIAL_CONSTRAINTS[ELDT_Reference_ColumnOrder_MAX];


/**
 * @brief REFERENTIAL_CONSTRAINTS 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( CONSTRAINT_CATALOG, CONSTRAINT_SCHEMA, CONSTRAINT_NAME )
 *   - => PRIMARY KEY ( CONSTRAINT_SCHEMA_ID, CONSTRAINT_ID )
 * - 비표준
 */

typedef enum eldtKeyConstREFERENTIAL_CONSTRAINTS
{
    /*
     * Primary Key 영역
     */

    ELDT_Reference_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    
    ELDT_Reference_Const_MAX
    
} eldtKeyConstREFERENTIAL_CONSTRAINTS;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescREFERENTIAL_CONSTRAINTS[ELDT_Reference_Const_MAX];


/**
 * @brief REFERENTIAL_CONSTRAINTS 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexREFERENTIAL_CONSTRAINTS
{
    ELDT_Reference_Index_CONSTRAINT_OWNER_ID = 0,
    ELDT_Reference_Index_CONSTRAINT_SCHEMA_ID,
    ELDT_Reference_Index_CONSTRAINT_ID,
    ELDT_Reference_Index_UNIQUE_CONSTRAINT_OWNER_ID,
    ELDT_Reference_Index_UNIQUE_CONSTRAINT_SCHEMA_ID,
    ELDT_Reference_Index_UNIQUE_CONSTRAINT_ID,
    
    ELDT_Reference_Index_MAX
    
} eldtIndexREFERENTIAL_CONSTRAINTS;

extern eldtDefinitionIndexDesc  gEldtIndexDescREFERENTIAL_CONSTRAINTS[ELDT_Reference_Index_MAX];



/** @} eldtREFERENTIAL_CONSTRAINTS */



#endif /* _ELDT_REFERENTIAL_CONSTRAINTS_H_ */
