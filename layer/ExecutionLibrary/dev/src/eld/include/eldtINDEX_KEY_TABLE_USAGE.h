/*******************************************************************************
 * eldtINDEX_KEY_TABLE_USAGE.h
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


#ifndef _ELDT_INDEX_KEY_TABLE_USAGE_H_
#define _ELDT_INDEX_KEY_TABLE_USAGE_H_ 1

/**
 * @file eldtINDEX_KEY_TABLE_USAGE.h
 * @brief INDEX_KEY_TABLE_USAGE dictionary base table
 */

/**
 * @defgroup eldtINDEX_KEY_TABLE_USAGE INDEX_KEY_TABLE_USAGE table
 * @ingroup eldtNonStandard
 * @{
 */


/**
 * @brief INDEX_KEY_TABLE_USAGE 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderINDEX_KEY_TABLE_USAGE
{
    /*
     * 식별자 영역
     */

    ELDT_IndexTable_ColumnOrder_INDEX_OWNER_ID,
    ELDT_IndexTable_ColumnOrder_INDEX_SCHEMA_ID,
    ELDT_IndexTable_ColumnOrder_INDEX_ID,

    ELDT_IndexTable_ColumnOrder_OWNER_ID,
    ELDT_IndexTable_ColumnOrder_SCHEMA_ID,
    ELDT_IndexTable_ColumnOrder_TABLE_ID,
    
    /*
     * SQL 표준 영역
     */

    
    /*
     * 부가 정보 영역
     */
    
    ELDT_IndexTable_ColumnOrder_MAX
    
} eldtColumnOrderINDEX_KEY_TABLE_USAGE;
    

extern eldtDefinitionTableDesc   gEldtTableDescINDEX_KEY_TABLE_USAGE;
extern eldtDefinitionColumnDesc  gEldtColumnDescINDEX_KEY_TABLE_USAGE[ELDT_IndexTable_ColumnOrder_MAX];


/**
 * @brief INDEX_KEY_TABLE_USAGE 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - 없음 
 * - 비표준
 *  - 비표준 1 : PRIMARY KEY (INDEX_SCHEMA_ID, INDEX_ID, SCHEMA_ID, TABLE_ID)
 *  - 비표준 2 : FOREIGN KEY ( SCHEMA_ID, TABLE_ID ) REFERENCES TABLES
 */

typedef enum eldtKeyConstINDEX_KEY_TABLE_USAGE
{
    /*
     * Primary Key 영역
     */

    ELDT_IndexTable_Const_PRIMARY_KEY = 0,               /**< 비표준 1 */
    
    /*
     * Unique 영역
     */

    /*
     * Foreign Key 영역
     */

    ELDT_IndexTable_Const_FOREIGN_KEY_TABLES,            /**< 비표준 2 */
    
    ELDT_IndexTable_Const_MAX
    
} eldtKeyConstINDEX_KEY_TABLE_USAGE;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescINDEX_KEY_TABLE_USAGE[ELDT_IndexTable_Const_MAX];



/**
 * @brief INDEX_KEY_TABLE_USAGE 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexINDEX_KEY_TABLE_USAGE
{
    ELDT_IndexTable_Index_INDEX_OWNER_ID = 0,
    ELDT_IndexTable_Index_INDEX_SCHEMA_ID,
    ELDT_IndexTable_Index_INDEX_ID,
    
    ELDT_IndexTable_Index_OWNER_ID,
    ELDT_IndexTable_Index_SCHEMA_ID,
    ELDT_IndexTable_Index_TABLE_ID,
    
    ELDT_IndexTable_Index_MAX
    
} eldtIndexINDEX_KEY_TABLE_USAGE;

extern eldtDefinitionIndexDesc  gEldtIndexDescINDEX_KEY_TABLE_USAGE[ELDT_IndexTable_Index_MAX];




/** @} eldtINDEX_INDEX_KEY_TABLE_USAGE */



#endif /* _ELDT_INDEX_INDEX_KEY_TABLE_USAGE_H_ */
