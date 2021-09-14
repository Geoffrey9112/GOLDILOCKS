/*******************************************************************************
 * eldtDATA_TYPE_DESCRIPTOR.h
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


#ifndef _ELDT_DATA_TYPE_DESCRIPTOR_H_
#define _ELDT_DATA_TYPE_DESCRIPTOR_H_ 1

/**
 * @file eldtDATA_TYPE_DESCRIPTOR.h
 * @brief DATA_TYPE_DESCRIPTOR dictionary base table
 */

/**
 * @defgroup eldtDATA_TYPE_DESCRIPTOR DATA_TYPE_DESCRIPTOR table
 * @ingroup eldtCore
 * @{
 */

#define ELDT_DTD_IDENTIFIER_INDEX_NAME  ( "DATA_TYPE_DESCRIPTOR_UNIQUE_DTD_IDENTIFIER_INDEX" )

/**
 * @brief DATA_TYPE_DESCRIPTORS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderDATA_TYPE_DESCRIPTOR
{
    /*
     * 식별자 영역
     */
    
    ELDT_DTDesc_ColumnOrder_OWNER_ID = 0,
    ELDT_DTDesc_ColumnOrder_SCHEMA_ID,
    ELDT_DTDesc_ColumnOrder_TABLE_ID,
    ELDT_DTDesc_ColumnOrder_COLUMN_ID,

    /*
     * SQL 표준 영역
     */

    ELDT_DTDesc_ColumnOrder_OBJECT_CATALOG,
    ELDT_DTDesc_ColumnOrder_OBJECT_SCHEMA,
    ELDT_DTDesc_ColumnOrder_OBJECT_NAME,
    ELDT_DTDesc_ColumnOrder_OBJECT_TYPE,
    ELDT_DTDesc_ColumnOrder_DTD_IDENTIFIER, 
    ELDT_DTDesc_ColumnOrder_DATA_TYPE,
    ELDT_DTDesc_ColumnOrder_DATA_TYPE_ID,              /* 비표준 */
    ELDT_DTDesc_ColumnOrder_CHARACTER_SET_CATALOG,
    ELDT_DTDesc_ColumnOrder_CHARACTER_SET_SCHEMA,
    ELDT_DTDesc_ColumnOrder_CHARACTER_SET_NAME,
    ELDT_DTDesc_ColumnOrder_STRING_LENGTH_UNIT,        /* 비표준 */
    ELDT_DTDesc_ColumnOrder_STRING_LENGTH_UNIT_ID,     /* 비표준 */
    ELDT_DTDesc_ColumnOrder_CHARACTER_MAXIMUM_LENGTH,
    ELDT_DTDesc_ColumnOrder_CHARACTER_OCTET_LENGTH,
    ELDT_DTDesc_ColumnOrder_COLLATION_CATALOG,
    ELDT_DTDesc_ColumnOrder_COLLATION_SCHEMA,
    ELDT_DTDesc_ColumnOrder_COLLATION_NAME,
    ELDT_DTDesc_ColumnOrder_NUMERIC_PRECISION,
    ELDT_DTDesc_ColumnOrder_NUMERIC_PRECISION_RADIX,
    ELDT_DTDesc_ColumnOrder_NUMERIC_SCALE,
    ELDT_DTDesc_ColumnOrder_DECLARED_DATA_TYPE,
    ELDT_DTDesc_ColumnOrder_DECLARED_NUMERIC_PRECISION,
    ELDT_DTDesc_ColumnOrder_DECLARED_NUMERIC_SCALE,
    ELDT_DTDesc_ColumnOrder_DATETIME_PRECISION,
    ELDT_DTDesc_ColumnOrder_INTERVAL_TYPE,
    ELDT_DTDesc_ColumnOrder_INTERVAL_TYPE_ID,          /* 비표준 */
    ELDT_DTDesc_ColumnOrder_INTERVAL_PRECISION,
    ELDT_DTDesc_ColumnOrder_USER_DEFINED_TYPE_CATALOG,
    ELDT_DTDesc_ColumnOrder_USER_DEFINED_TYPE_SCHEMA,
    ELDT_DTDesc_ColumnOrder_USER_DEFINED_TYPE_NAME,
    ELDT_DTDesc_ColumnOrder_SCOPE_CATALOG,
    ELDT_DTDesc_ColumnOrder_SCOPE_SCHEMA,
    ELDT_DTDesc_ColumnOrder_SCOPE_NAME,
    ELDT_DTDesc_ColumnOrder_MAXIMUM_CARDINALITY,
    ELDT_DTDesc_ColumnOrder_PHYSICAL_MAXIMUM_LENGTH,
    
    /*
     * 부가 정보 영역
     */

    ELDT_DTDesc_ColumnOrder_MAX

} eldtColumnOrderDATA_TYPE_DESCRIPTOR;


extern eldtDefinitionTableDesc   gEldtTableDescDATA_TYPE_DESCRIPTOR;
extern eldtDefinitionColumnDesc  gEldtColumnDescDATA_TYPE_DESCRIPTOR[ELDT_DTDesc_ColumnOrder_MAX];


/**
 * @brief DATA_TYPE_DESCRIPTOR 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( OBJECT_CATALOG, OBJECT_SCHEMA, OBJECT_NAME, OBJECT_TYPE, DTD_IDENTIFIER )
 *   - => 생성 안함 
 *  - SQL 2 : FOREIGN KEY ( USER_DEFINED_TYPE_CATALOG, USER_DEFINED_TYPE_SCHEMA ) REFERENCES SCHEMATA
 *   - => 생성 안함 
 * - 비표준
 *  - 비표준 3 : UNIQUE ( DTD_IDENTIFIER )
 */

typedef enum eldtKeyConstDATA_TYPE_DESCRIPTOR
{
    /*
     * Primary Key 영역
     */

    /*
     * Unique 영역
     */

    ELDT_DTDesc_Const_UNIQUE_DTD_IDENTIFIER = 0,   /**< 비표준 3 */
    
    /*
     * Foreign Key 영역
     */

    
    ELDT_DTDesc_Const_MAX
    
} eldtKeyConstDATA_TYPE_DESCRIPTOR;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescDATA_TYPE_DESCRIPTOR[ELDT_DTDesc_Const_MAX];


/**
 * @brief DATA_TYPE_DESCRIPTOR 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexDATA_TYPE_DESCRIPTOR
{
    ELDT_DTDesc_Index_OWNER_ID = 0,
    ELDT_DTDesc_Index_SCHEMA_ID,
    ELDT_DTDesc_Index_TABLE_ID,
    ELDT_DTDesc_Index_COLUMN_ID,
    
    /*
     * ELDT_DTDesc_Index_DTD_IDENTIFIER, Unique Key 가 있음. 
     */

    ELDT_DTDesc_Index_DATA_TYPE_ID,     
    
    ELDT_DTDesc_Index_MAX
    
} eldtIndexDATA_TYPE_DESCRIPTOR;

extern eldtDefinitionIndexDesc  gEldtIndexDescDATA_TYPE_DESCRIPTOR[ELDT_DTDesc_Index_MAX];




/** @} eldtDATA_TYPE_DESCRIPTOR */



#endif /* _ELDT_DATA_TYPE_DESCRIPTOR_H_ */
