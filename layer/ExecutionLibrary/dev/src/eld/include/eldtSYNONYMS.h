/*******************************************************************************
 * eldtSYNONYMS.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: 
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ELDT_SYNONYMS_H_
#define _ELDT_SYNONYMS_H_ 1

/**
 * @file eldtSYNONYMS.h
 * @brief SYNONYMS dictionary base table
 */

/**
 * @defgroup eldtSYNONYMS SYNONYMS table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief SYNONYMS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderSYNONYMS
{
    /*
     * 식별자 영역
     */

    ELDT_Synonyms_ColumnOrder_OWNER_ID = 0,
    ELDT_Synonyms_ColumnOrder_SCHEMA_ID,
    ELDT_Synonyms_ColumnOrder_SYNONYM_ID,
    ELDT_Synonyms_ColumnOrder_SYNONYM_TABLE_ID,

    /*
     * 부가 정보 영역
     */

    ELDT_Synonyms_ColumnOrder_SYNONYM_NAME,
    ELDT_Synonyms_ColumnOrder_OBJECT_SCHEMA_NAME,
    ELDT_Synonyms_ColumnOrder_OBJECT_NAME,
    ELDT_Synonyms_ColumnOrder_IS_BUILTIN,
    ELDT_Synonyms_ColumnOrder_CREATED_TIME,
    ELDT_Synonyms_ColumnOrder_MODIFIED_TIME,

    ELDT_Synonyms_ColumnOrder_MAX

} ELDTColumnOrderSYNONYMS;

extern eldtDefinitionTableDesc   gEldtTableDescSYNONYMS;
extern eldtDefinitionColumnDesc  gEldtColumnDescSYNONYMS[ELDT_Synonyms_ColumnOrder_MAX];

/**
 * @brief SYNONYMS 테이블의 KEY 제약 조건
 * - PRIMARY KEY (  SCHEMA_ID, SYNONYM_ID )
 * - UNIQUE ( SCHEMA_ID, SYNONYM_NAME )
 * - FOREIGN KEY ( SCHEMA_ID ) REFERENCES SCHEMATA
 */

typedef enum eldtKeyConstSYNONYMS
{
    /*
     * Primary Key 영역
     */

    ELDT_Synonyms_Const_PRIMARY_KEY = 0,

    /*
     * Unique 영역
     */

    ELDT_Synonyms_Const_UNIQUE_SCHEMA_ID_SYNONYM_NAME,

    /*
     * Foreign Key 영역
     */

    ELDT_Synonyms_Const_FOREIGN_KEY_SCHEMATA,
    
    ELDT_Synonyms_Const_MAX
    
} eldtKeyConstSYNONYMS;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescSYNONYMS[ELDT_Synonyms_Const_MAX];

/**
 * @brief SYNONYMS 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexSYNONYMS
{
    ELDT_Synonyms_Index_OWNER_ID = 0,
    ELDT_Synonyms_Index_SYNONYM_ID,
    
    ELDT_Synonyms_Index_MAX
    
} eldtIndexSYNONYMS;

extern eldtDefinitionIndexDesc  gEldtIndexDescSYNONYMS[ELDT_Synonyms_Index_MAX];

/** @} eldtSYNONYMS */


#endif /* _ELDT_SYNONYMS_H_ */
