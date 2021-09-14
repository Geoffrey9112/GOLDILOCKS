/*******************************************************************************
 * eldtVIEW_TABLE_USAGE.h
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


#ifndef _ELDT_VIEW_TABLE_USAGE_H_
#define _ELDT_VIEW_TABLE_USAGE_H_ 1

/**
 * @file eldtVIEW_TABLE_USAGE.h
 * @brief VIEW_TABLE_USAGE dictionary base table
 */

/**
 * @defgroup eldtVIEW_TABLE_USAGE VIEW_TABLE_USAGE table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief VIEW_TABLE_USAGE 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderVIEW_TABLE_USAGE
{
    /*
     * 식별자 영역
     */

    ELDT_ViewTable_ColumnOrder_VIEW_OWNER_ID,
    ELDT_ViewTable_ColumnOrder_VIEW_SCHEMA_ID,
    ELDT_ViewTable_ColumnOrder_VIEW_ID,

    ELDT_ViewTable_ColumnOrder_OWNER_ID,
    ELDT_ViewTable_ColumnOrder_SCHEMA_ID,
    ELDT_ViewTable_ColumnOrder_TABLE_ID,
    
    /*
     * SQL 표준 영역
     */

    /*
    ELDT_ViewTable_ColumnOrder_VIEW_CATALOG,
    ELDT_ViewTable_ColumnOrder_VIEW_SCHEMA,
    ELDT_ViewTable_ColumnOrder_VIEW_NAME,
    ELDT_ViewTable_ColumnOrder_TABLE_CATALOG,
    ELDT_ViewTable_ColumnOrder_TABLE_SCHEMA,
    ELDT_ViewTable_ColumnOrder_TABLE_NAME,
    */
        
    /*
     * 부가 정보 영역
     */
    
    ELDT_ViewTable_ColumnOrder_MAX
    
} eldtColumnOrderVIEW_TABLE_USAGE;
    

extern eldtDefinitionTableDesc   gEldtTableDescVIEW_TABLE_USAGE;
extern eldtDefinitionColumnDesc  gEldtColumnDescVIEW_TABLE_USAGE[ELDT_ViewTable_ColumnOrder_MAX];


/**
 * @brief VIEW_TABLE_USAGE 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( VIEW_CATALOG, VIEW_SCHEMA, VIEW_NAME, TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME )
 *   - => PRIMARY KEY ( VIEW_SCHEMA_ID, VIEW_ID, SCHEMA_ID, TABLE_ID )
 *  - SQL 2 : FOREIGN KEY ( VIEW_CATALOG, VIEW_SCHEMA, VIEW_NAME ) REFERENCES VIEWS
 *   - => FOREIGN KEY ( VIEW_SCHEMA_ID, VIEW_ID ) REFERENCES VIEWS
 * - 비표준
 *  - VIEW_TABLE_USAGE 는 compiled view 에 대해서만 유효하며, loose consitency 정책을 사용하므로
 *  - TABLE_ID 등에 foreign key 등의 제약조건을 설정하지 않는다.
 */

typedef enum eldtKeyConstVIEW_TABLE_USAGE
{
    /*
     * Primary Key 영역
     */

    ELDT_ViewTable_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    /*
     * Foreign Key 영역
     */

    ELDT_ViewTable_Const_FOREIGN_KEY_VIEWS,  /**< SQL 2 */
    
    ELDT_ViewTable_Const_MAX
    
} eldtKeyConstVIEW_TABLE_USAGE;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescVIEW_TABLE_USAGE[ELDT_ViewTable_Const_MAX];


/**
 * @brief VIEW_TABLE_USAGE 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexVIEW_TABLE_USAGE
{
    ELDT_ViewTable_Index_VIEW_OWNER_ID = 0,
    ELDT_ViewTable_Index_VIEW_SCHEMA_ID,
    ELDT_ViewTable_Index_VIEW_ID,
    
    ELDT_ViewTable_Index_OWNER_ID,
    ELDT_ViewTable_Index_SCHEMA_ID,
    ELDT_ViewTable_Index_TABLE_ID,
    
    ELDT_ViewTable_Index_MAX
    
} eldtIndexVIEW_TABLE_USAGE;

extern eldtDefinitionIndexDesc  gEldtIndexDescVIEW_TABLE_USAGE[ELDT_ViewTable_Index_MAX];



/** @} eldtVIEW_TABLE_USAGE */



#endif /* _ELDT_VIEW_TABLE_USAGE_H_ */
