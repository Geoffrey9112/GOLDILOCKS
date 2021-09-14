/*******************************************************************************
 * eldtINDEXES.h
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


#ifndef _ELDT_INDEXES_H_
#define _ELDT_INDEXES_H_ 1

/**
 * @file eldtINDEXES.h
 * @brief INDEXES dictionary base table
 */

/**
 * @defgroup eldtINDEXES INDEXES table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief INDEXES 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderINDEXES
{
    /*
     * 식별자 영역
     */
    
    ELDT_Indexes_ColumnOrder_OWNER_ID = 0,
    ELDT_Indexes_ColumnOrder_SCHEMA_ID,
    ELDT_Indexes_ColumnOrder_INDEX_ID,
    ELDT_Indexes_ColumnOrder_TABLESPACE_ID,
    ELDT_Indexes_ColumnOrder_PHYSICAL_ID,

    /*
     * SQL 표준 영역
     */

    
    /*
     * 부가 정보 영역
     */

    ELDT_Indexes_ColumnOrder_INDEX_NAME,
    ELDT_Indexes_ColumnOrder_INDEX_TYPE,
    ELDT_Indexes_ColumnOrder_INDEX_TYPE_ID,
    ELDT_Indexes_ColumnOrder_IS_UNIQUE,
    ELDT_Indexes_ColumnOrder_INVALID,
    ELDT_Indexes_ColumnOrder_BY_CONSTRAINT,
    ELDT_Indexes_ColumnOrder_IS_BUILTIN,
    
    ELDT_Indexes_ColumnOrder_CREATED_TIME,
    ELDT_Indexes_ColumnOrder_MODIFIED_TIME,
    ELDT_Indexes_ColumnOrder_COMMENTS,

    ELDT_Indexes_ColumnOrder_MAX

} eldtColumnOrderINDEXES;


extern eldtDefinitionTableDesc   gEldtTableDescINDEXES;
extern eldtDefinitionColumnDesc  gEldtColumnDescINDEXES[ELDT_Indexes_ColumnOrder_MAX];


/** @} eldtINDEXES */


/**
 * @brief INDEXES 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - 없음 
 * - 비표준
 *  - 비표준 1 : PRIMARY KEY (SCHEMA_ID, INDEX_ID) 
 *  - 비표준 2 : UNIQUE (SCHEMA_ID, INDEX_NAME) 
 */

typedef enum eldtKeyConstINDEXES
{
    /*
     * Primary Key 영역
     */

    ELDT_Indexes_Const_PRIMARY_KEY = 0,               /**< 비표준 1 */
    
    /*
     * Unique 영역
     */

    ELDT_Indexes_Const_UNIQUE_SCHEMA_ID_INDEX_NAME,   /**< 비표준 2 */
    
    /*
     * Foreign Key 영역
     */

    
    ELDT_Indexes_Const_MAX
    
} eldtKeyConstINDEXES;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescINDEXES[ELDT_Indexes_Const_MAX];



/**
 * @brief INDEXES 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexINDEXES
{
    ELDT_Indexes_Index_OWNER_ID = 0,
    ELDT_Indexes_Index_SCHEMA_ID, 
    ELDT_Indexes_Index_INDEX_ID,
    ELDT_Indexes_Index_TABLESPACE_ID,
    ELDT_Indexes_Index_PHYSICAL_ID,
    ELDT_Indexes_Index_INDEX_NAME,
    
    ELDT_Indexes_Index_MAX
    
} eldtIndexINDEXES;

extern eldtDefinitionIndexDesc  gEldtIndexDescINDEXES[ELDT_Indexes_Index_MAX];



#endif /* _ELDT_INDEXES_H_ */
