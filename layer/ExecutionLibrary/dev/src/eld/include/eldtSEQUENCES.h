/*******************************************************************************
 * eldtSEQUENCES.h
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


#ifndef _ELDT_SEQUENCES_H_
#define _ELDT_SEQUENCES_H_ 1

/**
 * @file eldtSEQUENCES.h
 * @brief SEQUENCES dictionary base table
 */

/**
 * @defgroup eldtSEQUENCES SEQUENCES table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief SEQUENCES 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderSEQUENCES
{
    /*
     * 식별자 영역
     */
    
    ELDT_Sequences_ColumnOrder_OWNER_ID = 0,
    ELDT_Sequences_ColumnOrder_SCHEMA_ID,
    ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
    ELDT_Sequences_ColumnOrder_SEQUENCE_TABLE_ID,
    ELDT_Sequences_ColumnOrder_TABLESPACE_ID,
    ELDT_Sequences_ColumnOrder_PHYSICAL_ID,

    /*
     * SQL 표준 영역
     */

    /*
    ELDT_Sequences_ColumnOrder_SEQUENCE_CATALOG,
    ELDT_Sequences_ColumnOrder_SEQUENCE_SCHEMA,
    */
    ELDT_Sequences_ColumnOrder_SEQUENCE_NAME,
    ELDT_Sequences_ColumnOrder_DTD_IDENTIFIER,
    ELDT_Sequences_ColumnOrder_START_VALUE,
    ELDT_Sequences_ColumnOrder_MINIMUM_VALUE,
    ELDT_Sequences_ColumnOrder_MAXIMUM_VALUE,
    ELDT_Sequences_ColumnOrder_INCREMENT,
    ELDT_Sequences_ColumnOrder_CYCLE_OPTION,
    
    /*
     * 부가 정보 영역
     */
    
    ELDT_Sequences_ColumnOrder_CACHE_SIZE,
    ELDT_Sequences_ColumnOrder_IS_BUILTIN,

    ELDT_Sequences_ColumnOrder_CREATED_TIME,
    ELDT_Sequences_ColumnOrder_MODIFIED_TIME,
    ELDT_Sequences_ColumnOrder_COMMENTS,

    ELDT_Sequences_ColumnOrder_MAX

} eldtColumnOrderSEQUENCES;


extern eldtDefinitionTableDesc   gEldtTableDescSEQUENCES;
extern eldtDefinitionColumnDesc  gEldtColumnDescSEQUENCES[ELDT_Sequences_ColumnOrder_MAX];


/**
 * @brief SEQUENCES 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY (SEQUENCE_CATALOG, SEQUENCE_SCHEMA, SEQUENCE_NAME)
 *   - => PRIMARY KEY (SCHEMA_ID, SEQUENCE_ID)
 *  - SQL 2 : FOREIGN KEY ( SEQUENCE_CATALOG, SEQUENCE_SCHEMA ) REFERENCES SCHEMATA
 *   - => FOREIGN KEY ( SCHEMA_ID ) REFERENCES SCHEMATA
 * - 비표준
 *  - 비표준 1 : UNIQUE (SCHEMA_ID, SEQUENCE_NAME)
 */

typedef enum eldtKeyConstSEQUENCES
{
    /*
     * Primary Key 영역
     */

    ELDT_Sequences_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    ELDT_Sequences_Const_UNIQUE_SCHEMA_ID_SEQUENCE_NAME,          /**< 비표준 1 */
    
    /*
     * Foreign Key 영역
     */

    ELDT_Sequences_Const_FOREIGN_KEY_SCHEMATA,       /**< SQL 2 */
    
    ELDT_Sequences_Const_MAX
    
} eldtKeyConstSEQUENCES;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescSEQUENCES[ELDT_Sequences_Const_MAX];


/**
 * @brief SEQUENCES 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexSEQUENCES
{
    ELDT_Sequences_Index_OWNER_ID = 0,
    /*
     * ELDT_Sequences_Index_SCHEMA_ID, Foreign Key 가 존재함
     */
    ELDT_Sequences_Index_SEQUENCE_ID,
    ELDT_Sequences_Index_TABLESPACE_ID,
    ELDT_Sequences_Index_PHYSICAL_ID,
    ELDT_Sequences_Index_SEQUENCE_NAME,
    
    ELDT_Sequences_Index_MAX
    
} eldtIndexSEQUENCES;

extern eldtDefinitionIndexDesc  gEldtIndexDescSEQUENCES[ELDT_Sequences_Index_MAX];

/** @} eldtSEQUENCES */



#endif /* _ELDT_SEQUENCES_H_ */
