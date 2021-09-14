/*******************************************************************************
 * eldtCATALOG_NAME.h
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


#ifndef _ELDT_CATALOG_NAME_H_
#define _ELDT_CATALOG_NAME_H_ 1

/**
 * @file eldtCATALOG_NAME.h
 * @brief CATALOG_NAME dictionary base table
 */

/**
 * @defgroup eldtCATALOG_NAME CATALOG_NAME table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief default catalog id
 */
#define ELDT_DEFAULT_CATALOG_ID   ( 1 )

/**
 * @brief CATALOG_NAME 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderCATALOG
{
    /*
     * 식별자 영역
     */

    ELDT_Catalog_ColumnOrder_CATALOG_ID,
    
    /*
     * SQL 표준 영역
     */

    ELDT_Catalog_ColumnOrder_CATALOG_NAME,
    
    /*
     * 부가 정보 영역
     */
    ELDT_Catalog_ColumnOrder_CHARACTER_SET_ID,
    ELDT_Catalog_ColumnOrder_CHARACTER_SET_NAME,
    ELDT_Catalog_ColumnOrder_CHAR_LENGTH_UNITS_ID,
    ELDT_Catalog_ColumnOrder_CHAR_LENGTH_UNITS,
    ELDT_Catalog_ColumnOrder_TIME_ZONE_INTERVAL,
    ELDT_Catalog_ColumnOrder_DEFAULT_DATA_TABLESPACE_ID,
    ELDT_Catalog_ColumnOrder_DEFAULT_TEMP_TABLESPACE_ID,
    ELDT_Catalog_ColumnOrder_CREATED_TIME,
    ELDT_Catalog_ColumnOrder_MODIFIED_TIME,
    ELDT_Catalog_ColumnOrder_COMMENTS,
    
    ELDT_Catalog_ColumnOrder_MAX
    
} eldtColumnOrderCATALOG;
    

extern eldtDefinitionTableDesc   gEldtTableDescCATALOG;
extern eldtDefinitionColumnDesc  gEldtColumnDescCATALOG[ELDT_Catalog_ColumnOrder_MAX];


/**
 * @brief CATALOG_NAME 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( CATALOG_NAME )
 *   - 하나의 레코드로 구성된 테이블로 별도의 key constraint가 필요없으나, Cache 의 일관된 관리를 위해. 
 * - 비표준
 *  - 비표준 1 : UNIQUE ( CATALOG_NAME )
 */

typedef enum eldtKeyConstCATALOG_NAME
{
    /*
     * Primary Key 영역
     */

    ELDT_Catalog_Const_PRIMARY_KEY = 0,                  /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    ELDT_Catalog_Const_UNIQUE_CATALOG_NAME,              /**< 비표준 1 */
    
    /*
     * Foreign Key 영역
     */

    
    ELDT_Catalog_Const_MAX
    
} eldtKeyConstCATALOG_NAME;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescCATALOG_NAME[ELDT_Catalog_Const_MAX];

/**
 * @brief CATALOG_NAME 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 * - 별도로 생성할 인덱스가 없음.
 */

typedef enum eldtIndexCATALOG_NAME
{
    ELDT_Catalog_Index_MAX = 0
    
} eldtIndexCATALOG_NAME;


/** @} eldtCATALOG_NAME */



#endif /* _ELDT_CATALOG_NAME_H_ */
