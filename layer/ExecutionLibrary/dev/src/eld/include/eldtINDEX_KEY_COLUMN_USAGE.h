/*******************************************************************************
 * eldtINDEX_KEY_COLUMN_USAGE.h
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


#ifndef _ELDT_INDEX_KEY_COLUMN_USAGE_H_
#define _ELDT_INDEX_KEY_COLUMN_USAGE_H_ 1

/**
 * @file eldtINDEX_KEY_COLUMN_USAGE.h
 * @brief INDEX_KEY_COLUMN_USAGE dictionary base table
 */

/**
 * @defgroup eldtINDEX_KEY_COLUMN_USAGE INDEX_KEY_COLUMN_USAGE table
 * @ingroup eldtNonStandard
 * @{
 */

#define ELDT_INDEX_KEY_COLUMN_USAGE_INDEX_INDEX_ID  ( "INDEX_KEY_COLUMN_USAGE_INDEX_INDEX_ID" )

/**
 * @brief INDEX_KEY_COLUMN_USAGE 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderINDEX_KEY_COLUMN_USAGE
{
    /*
     * 식별자 영역
     */

    ELDT_IndexKey_ColumnOrder_INDEX_OWNER_ID,
    ELDT_IndexKey_ColumnOrder_INDEX_SCHEMA_ID,
    ELDT_IndexKey_ColumnOrder_INDEX_ID,

    ELDT_IndexKey_ColumnOrder_OWNER_ID,
    ELDT_IndexKey_ColumnOrder_SCHEMA_ID,
    ELDT_IndexKey_ColumnOrder_TABLE_ID,
    ELDT_IndexKey_ColumnOrder_COLUMN_ID,
    
    /*
     * SQL 표준 영역
     */

    
    /*
     * 부가 정보 영역
     */
    
    ELDT_IndexKey_ColumnOrder_ORDINAL_POSITION,
    ELDT_IndexKey_ColumnOrder_IS_ASCENDING_ORDER,
    ELDT_IndexKey_ColumnOrder_IS_NULLS_FIRST,
    
    ELDT_IndexKey_ColumnOrder_MAX
    
} eldtColumnOrderINDEX_KEY_COLUMN_USAGE;
    

extern eldtDefinitionTableDesc   gEldtTableDescINDEX_KEY_COLUMN_USAGE;
extern eldtDefinitionColumnDesc  gEldtColumnDescINDEX_KEY_COLUMN_USAGE[ELDT_IndexKey_ColumnOrder_MAX];


/**
 * @brief INDEX_KEY_COLUMN_USAGE 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - 없음 
 * - 비표준
 *  - 비표준 1 : PRIMARY KEY (INDEX_SCHEMA_ID, INDEX_ID, SCHEMA_ID, TABLE_ID, COLUMN_ID)
 *  - 비표준 2 : UNIQUE (INDEX_SCHEMA_ID, INDEX_ID, ORDINAL_POSITION )
 *  - 비표준 3 : FOREIGN KEY ( SCHEMA_ID, TABLE_ID, COLUMN_ID ) REFERENCES COLUMNS
 */

typedef enum eldtKeyConstINDEX_KEY_COLUMN_USAGE
{
    /*
     * Primary Key 영역
     */

    ELDT_IndexKey_Const_PRIMARY_KEY = 0,               /**< 비표준 1 */
    
    /*
     * Unique 영역
     */

    ELDT_IndexKey_Const_UNIQUE_XXXX_ORDINAL_POSITION,  /**< 비표준 2 */
    
    /*
     * Foreign Key 영역
     */

    ELDT_IndexKey_Const_FOREIGN_KEY_COLUMNS,           /**< 비표준  */
    
    ELDT_IndexKey_Const_MAX
    
} eldtKeyConstINDEX_KEY_COLUMN_USAGE;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescINDEX_KEY_COLUMN_USAGE[ELDT_IndexKey_Const_MAX];



/**
 * @brief INDEX_KEY_COLUMN_USAGE 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexINDEX_KEY_COLUMN_USAGE
{
    ELDT_IndexKey_Index_INDEX_OWNER_ID = 0,
    ELDT_IndexKey_Index_INDEX_SCHEMA_ID,
    ELDT_IndexKey_Index_INDEX_ID,
    
    ELDT_IndexKey_Index_OWNER_ID,
    ELDT_IndexKey_Index_SCHEMA_ID,
    ELDT_IndexKey_Index_TABLE_ID,
    ELDT_IndexKey_Index_COLUMN_ID,
    
    ELDT_IndexKey_Index_MAX
    
} eldtIndexINDEX_KEY_COLUMN_USAGE;

extern eldtDefinitionIndexDesc  gEldtIndexDescINDEX_KEY_COLUMN_USAGE[ELDT_IndexKey_Index_MAX];




/** @} eldtINDEX_INDEX_KEY_COLUMN_USAGE */



#endif /* _ELDT_INDEX_INDEX_KEY_COLUMN_USAGE_H_ */
