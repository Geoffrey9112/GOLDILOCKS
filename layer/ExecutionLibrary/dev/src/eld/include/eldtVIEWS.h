/*******************************************************************************
 * eldtVIEWS.h
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


#ifndef _ELDT_VIEWS_H_
#define _ELDT_VIEWS_H_ 1

/**
 * @file eldtVIEWS.h
 * @brief VIEWS dictionary base table
 */

/**
 * @defgroup eldtVIEWS VIEWS table
 * @ingroup eldtStandard
 * @{
 */

#define ELDT_VIEWS_INDEX_TABLE_ID  ( "VIEWS_INDEX_TABLE_ID" )

/**
 * @brief VIEWS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderVIEWS
{
    /*
     * 식별자 영역
     */
    
    ELDT_Views_ColumnOrder_OWNER_ID = 0,
    ELDT_Views_ColumnOrder_SCHEMA_ID,
    ELDT_Views_ColumnOrder_TABLE_ID,

    /*
     * SQL 표준 영역
     */

    /*
    ELDT_Views_ColumnOrder_TABLE_CATALOG,
    ELDT_Views_ColumnOrder_TABLE_SCHEMA,
    ELDT_Views_ColumnOrder_TABLE_NAME,
    */
    ELDT_Views_ColumnOrder_VIEW_COLUMNS,        /* 비표준  */
    ELDT_Views_ColumnOrder_VIEW_DEFINITION,
    ELDT_Views_ColumnOrder_CHECK_OPTION,
    ELDT_Views_ColumnOrder_IS_COMPILED,         /* 비표준 */
    ELDT_Views_ColumnOrder_IS_AFFECTED,         /* 비표준 */
    ELDT_Views_ColumnOrder_IS_UPDATABLE,
    ELDT_Views_ColumnOrder_IS_TRIGGER_UPDATABLE,
    ELDT_Views_ColumnOrder_IS_TRIGGER_DELETABLE,
    ELDT_Views_ColumnOrder_IS_TRIGGER_INSERTABLE_INTO,
    
    /*
     * 부가 정보 영역
     */

    
    ELDT_Views_ColumnOrder_MAX

} eldtColumnOrderVIEWS;


extern eldtDefinitionTableDesc   gEldtTableDescVIEWS;
extern eldtDefinitionColumnDesc  gEldtColumnDescVIEWS[ELDT_Views_ColumnOrder_MAX];


/** @} eldtVIEWS */


/**
 * @brief VIEWS 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME )
 *   - => PRIMARY KEY ( SCHEMA_ID, TABLE_ID )
 * - 비표준
 */

typedef enum eldtKeyConstVIEWS
{
    /*
     * Primary Key 영역
     */

    ELDT_Views_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    
    ELDT_Views_Const_MAX
    
} eldtKeyConstVIEWS;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescVIEWS[ELDT_Views_Const_MAX];



/**
 * @brief VIEWS 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexVIEWS
{
    ELDT_Views_Index_OWNER_ID = 0,
    ELDT_Views_Index_SCHEMA_ID,
    ELDT_Views_Index_TABLE_ID,
    
    ELDT_Views_Index_MAX
    
} eldtIndexVIEWS;

extern eldtDefinitionIndexDesc  gEldtIndexDescVIEWS[ELDT_Views_Index_MAX];


#endif /* _ELDT_VIEWS_H_ */
