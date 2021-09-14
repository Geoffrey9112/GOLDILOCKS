/*******************************************************************************
 * eldtSCHEMATA.h
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


#ifndef _ELDT_SCHEMATA_H_
#define _ELDT_SCHEMATA_H_ 1

/**
 * @file eldtSCHEMATA.h
 * @brief SCHEMATA dictionary base table
 */

/**
 * @defgroup eldtSCHEMATA SCHEMATA table
 * @ingroup eldtStandard
 * @{
 */


/**
 * @brief SCHEMATA 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderSCHEMATA
{
    /*
     * 식별자 영역
     */

    ELDT_Schemata_ColumnOrder_OWNER_ID = 0,
    ELDT_Schemata_ColumnOrder_SCHEMA_ID,

    /*
     * SQL 표준 영역
     */

    /*
    ELDT_Schemata_ColumnOrder_CATALOG_NAME,
    ELDT_Schemata_ColumnOrder_SCHEMA_OWNER,
    */
    
    ELDT_Schemata_ColumnOrder_SCHEMA_NAME,
    ELDT_Schemata_ColumnOrder_DEFAULT_CHARACTER_SET_CATALOG,
    ELDT_Schemata_ColumnOrder_DEFAULT_CHARACTER_SET_SCHEMA,
    ELDT_Schemata_ColumnOrder_DEFAULT_CHARACTER_SET_NAME,
    ELDT_Schemata_ColumnOrder_SQL_PATH,
    
    /*
     * 부가 정보 영역
     */
    ELDT_Schemata_ColumnOrder_IS_BUILTIN,
    ELDT_Schemata_ColumnOrder_CREATED_TIME,
    ELDT_Schemata_ColumnOrder_MODIFIED_TIME,
    ELDT_Schemata_ColumnOrder_COMMENTS,
    
    ELDT_Schemata_ColumnOrder_MAX
    
} eldtColumnOrderSCHEMATA;
    

extern eldtDefinitionTableDesc   gEldtTableDescSCHEMATA;
extern eldtDefinitionColumnDesc  gEldtColumnDescSCHEMATA[ELDT_Schemata_ColumnOrder_MAX];


/**
 * @brief SCHEMATA 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( CATALOG_NAME, SCHEMA_NAME )
 *   - => PRIMARY KEY (SCHEMA_ID)
 *  - SQL 2 : FOREIGN KEY ( SCHEMA_OWNER ) REFERENCES AUTHORIZATIONS
 *   - => FOREIGN KEY (OWNER_ID) REFERENCES AUTHORIZATIONS
 *  - SQL 3 : FOREIGN KEY ( CATALOG_NAME ) REFERENCES CATALOG_NAME
 *   - => 생성하지 않음 
 * - 비표준
 *  - 비표준 1 : UNIQUE (SCHEMA_NAME) 
 */

typedef enum eldtKeyConstSCHEMATA
{
    /*
     * Primary Key 영역
     */

    ELDT_Schemata_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    ELDT_Schemata_Const_UNIQUE_SCHEMA_NAME,          /**< 비표준 1 */
    
    /*
     * Foreign Key 영역
     */

    ELDT_Schemata_Const_FOREIGN_KEY_AUTHORIZATIONS,  /**< SQL 2 */
    
    ELDT_Schemata_Const_MAX
    
} eldtKeyConstSCHEMATA;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescSCHEMATA[ELDT_Schemata_Const_MAX];


/**
 * @brief SCHEMATA 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexSCHEMATA
{
    ELDT_Schemata_Index_OWNER_ID = 0,
    
    ELDT_Schemata_Index_MAX
    
} eldtIndexSCHEMATA;

extern eldtDefinitionIndexDesc  gEldtIndexDescSCHEMATA[ELDT_Schemata_Index_MAX];




/**
 * @brief DEFINITION_SCHEMA 의 built-in SCHEMA ID
 */

typedef enum eldtBuiltInSchemaID
{
    ELDT_SCHEMA_ID_NA = 0,                   /**< Not Available */

    ELDT_SCHEMA_ID_DEFINITION_SCHEMA,              /**< DEFINITION_SCHEMA schema */
    ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA,             /**< FIXED_TABLE_SCHEMA schema */
    ELDT_SCHEMA_ID_DICTIONARY_SCHEMA,              /**< DICIONARY_SCHEMA schema */
    ELDT_SCHEMA_ID_INFORMATION_SCHEMA,             /**< INFORMATION_SCHEMA schema */
    ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA,        /**< PERFORMANCE_SCHEMA schema */
    ELDT_SCHEMA_ID_PUBLIC,                         /**< PUBLIC schema */

    ELDT_SCHEMA_ID_MAX
} eldtBuiltInSchemaID;

extern stlChar * gEldtBuiltInSchemaString[ELDT_SCHEMA_ID_MAX];


/**
 * @brief DEFINITION_SCHEMA의 built-in Schema Descriptor
 */
typedef struct eldtBuiltInSchemaDesc
{
    eldtBuiltInSchemaID      mSchemaID;         /**< Schema ID */
    stlChar                * mSchemaComment;    /**< Schema 의 주석 */
} eldtBuiltInSchemaDesc;

extern eldtBuiltInSchemaDesc gEldtBuiltInSchemaFIXED_TABLE_SCHEMA;
extern eldtBuiltInSchemaDesc gEldtBuiltInSchemaPERFORMANCE_VIEW_SCHEMA;

stlStatus eldtInsertBuiltInSchemaDesc( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       ellEnv       * aEnv );

/** @} eldtSCHEMATA */


#endif /* _ELDT_SCHEMATA_H_ */
