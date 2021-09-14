/*******************************************************************************
 * eldtCOLUMNS.h
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


#ifndef _ELDT_COLUMNS_H_
#define _ELDT_COLUMNS_H_ 1

/**
 * @file eldtCOLUMNS.h
 * @brief COLUMNS dictionary base table
 */

/**
 * @defgroup eldtCOLUMNS COLUMNS table
 * @ingroup eldtCore
 * @{
 */

#define ELDT_COLUMNS_INDEX_TABLE_ID  ( "COLUMNS_INDEX_TABLE_ID" )

/**
 * @brief COLUMNS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderCOLUMNS
{
    /*
     * 식별자 영역
     */
    
    ELDT_Columns_ColumnOrder_OWNER_ID = 0,
    ELDT_Columns_ColumnOrder_SCHEMA_ID,
    ELDT_Columns_ColumnOrder_TABLE_ID,
    ELDT_Columns_ColumnOrder_COLUMN_ID,

    /*
     * SQL 표준 영역
     */

    /*
    ELDT_Columns_ColumnOrder_TABLE_CATALOG,
    ELDT_Columns_ColumnOrder_TABLE_SCHEMA,
    ELDT_Columns_ColumnOrder_TABLE_NAME,
    */
    
    ELDT_Columns_ColumnOrder_COLUMN_NAME,
    ELDT_Columns_ColumnOrder_PHYSICAL_ORDINAL_POSITION,
    ELDT_Columns_ColumnOrder_LOGICAL_ORDINAL_POSITION,
    ELDT_Columns_ColumnOrder_DTD_IDENTIFIER,
    ELDT_Columns_ColumnOrder_DOMAIN_CATALOG,
    ELDT_Columns_ColumnOrder_DOMAIN_SCHEMA,
    ELDT_Columns_ColumnOrder_DOMAIN_NAME,
    ELDT_Columns_ColumnOrder_COLUMN_DEFAULT,
    ELDT_Columns_ColumnOrder_IS_NULLABLE,
    ELDT_Columns_ColumnOrder_IS_SELF_REFERENCING,
    ELDT_Columns_ColumnOrder_IS_IDENTITY,
    ELDT_Columns_ColumnOrder_IDENTITY_GENERATION,
    ELDT_Columns_ColumnOrder_IDENTITY_GENERATION_ID,  /* 비표준 */
    ELDT_Columns_ColumnOrder_IDENTITY_START,
    ELDT_Columns_ColumnOrder_IDENTITY_INCREMENT,
    ELDT_Columns_ColumnOrder_IDENTITY_MAXIMUM,
    ELDT_Columns_ColumnOrder_IDENTITY_MINIMUM,
    ELDT_Columns_ColumnOrder_IDENTITY_CYCLE,
    ELDT_Columns_ColumnOrder_IDENTITY_TABLESPACE_ID,  /* 비표준 */
    ELDT_Columns_ColumnOrder_IDENTITY_PHYSICAL_ID,    /* 비표준 */
    ELDT_Columns_ColumnOrder_IDENTITY_CACHE_SIZE,     /* 비표준 */
    ELDT_Columns_ColumnOrder_IS_GENERATED,
    ELDT_Columns_ColumnOrder_IS_SYSTEM_VERSION_START,
    ELDT_Columns_ColumnOrder_IS_SYSTEM_VERSION_END,
    ELDT_Columns_ColumnOrder_SYSTEM_VERSION_TIMESTAMP_GENERATION,
    ELDT_Columns_ColumnOrder_IS_UPDATABLE,
    
    /* ELDT_Columns_ColumnOrder_TABLE_CATALOG, */
    
    /*
     * 부가 정보 영역
     */

    ELDT_Columns_ColumnOrder_IS_UNUSED,
    ELDT_Columns_ColumnOrder_COMMENTS,

    ELDT_Columns_ColumnOrder_MAX

} eldtColumnOrderCOLUMNS;


extern eldtDefinitionTableDesc   gEldtTableDescCOLUMNS;
extern eldtDefinitionColumnDesc  gEldtColumnDescCOLUMNS[ELDT_Columns_ColumnOrder_MAX];


/**
 * @brief COLUMNS 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME, COLUMN_NAME )
 *   - => PRIMARY KEY (SCHEMA_ID, TABLE_ID, COLUMN_ID)
 *  - SQL 2 : UNIQUE ( TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME, ORDINAL_POSITION )
 *   - => UNIQUE (SCHEMA_ID, TABLE_ID, ORDINAL_POSITION )
 *  - SQL 3 : FOREIGN KEY ( TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME ) REFERENCES TABLES
 *   - => FOREIGN KEY ( SCHEMA_ID, TABLE_ID ) REFERENCES TABLES
 * - 비표준
 *  - 비표준 1 : UNIQUE (SCHEMA_ID, TABLE_ID, COLUMN_NAME)
 */

typedef enum eldtKeyConstCOLUMNS
{
    /*
     * Primary Key 영역
     */

    ELDT_Columns_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    ELDT_Columns_Const_UNIQUE_SCHEMA_ID_TABLE_ID_PHYSICAL_ORDINAL_POSITION, /**< SQL 2 */
    // ELDT_Columns_Const_UNIQUE_SCHEMA_ID_TABLE_ID_LOGICAL_ORDINAL_POSITION,  /**< SQL 2 */
    
    /*
     * Foreign Key 영역
     */

    ELDT_Columns_Const_FOREIGN_KEY_TABLES,          /**< SQL 3 */
    
    ELDT_Columns_Const_MAX
    
} eldtKeyConstCOLUMNS;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescCOLUMNS[ELDT_Columns_Const_MAX];


/**
 * @brief COLUMNS 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexCOLUMNS
{
    ELDT_Columns_Index_OWNER_ID = 0,
    ELDT_Columns_Index_SCHEMA_ID,
    ELDT_Columns_Index_TABLE_ID,
    ELDT_Columns_Index_COLUMN_ID,
    ELDT_Columns_Index_COLUMN_NAME,
    ELDT_Columns_Index_DTD_IDENTIFIER,
    ELDT_Columns_Index_IDENTITY_TABLESPACE_ID,  
    ELDT_Columns_Index_IDENTITY_PHYSICAL_ID,    
    
    ELDT_Columns_Index_MAX
    
} eldtIndexCOLUMNS;

extern eldtDefinitionIndexDesc  gEldtIndexDescCOLUMNS[ELDT_Columns_Index_MAX];



/**
 * @brief DEFINITION_SCHEMA 의 IDENTITY 컬럼의 기본값 
 */
#define ELDT_IDENTITY_VALUE_START        ( 1 )
#define ELDT_IDENTITY_VALUE_INCREMENT    ( 1 )
#define ELDT_IDENTITY_VALUE_MAXIMUM      ( STL_INT64_MAX  )
#define ELDT_IDENTITY_VALUE_MINIMUM      ( 1 )
#define ELDT_IDENTITY_VALUE_CYCLE        ( STL_FALSE )
#define ELDT_IDENTITY_CACHE_SIZE         ( 20 )

/** @} eldtCOLUMNS */



#endif /* _ELDT_COLUMNS_H_ */
