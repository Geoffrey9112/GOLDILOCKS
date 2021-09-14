/*******************************************************************************
 * eldtPUBLIC_SYNONYMS.h
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


#ifndef _ELDT_PUBLIC_SYNONYMS_H_
#define _ELDT_PUBLIC_SYNONYMS_H_ 1

/**
 * @file eldtPUBLIC_SYNONYMS.h
 * @brief PUBLIC SYNONYMS dictionary base table
 */

/**
 * @defgroup eldtPUBLIC_SYNONYMS PUBLIC SYNONYMS table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief PUBLIC SYNONYMS 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderPUBLIC_SYNONYMS
{
    /*
     * 식별자 영역
     */

    ELDT_Public_Synonyms_ColumnOrder_SYNONYM_ID = 0,

    /*
     * 부가 정보 영역
     */

    ELDT_Public_Synonyms_ColumnOrder_SYNONYM_NAME,
    ELDT_Public_Synonyms_ColumnOrder_CREATOR_ID,
    ELDT_Public_Synonyms_ColumnOrder_OBJECT_SCHEMA_NAME,
    ELDT_Public_Synonyms_ColumnOrder_OBJECT_NAME,
    ELDT_Public_Synonyms_ColumnOrder_IS_BUILTIN,
    ELDT_Public_Synonyms_ColumnOrder_CREATED_TIME,
    ELDT_Public_Synonyms_ColumnOrder_MODIFIED_TIME,

    ELDT_Public_Synonyms_ColumnOrder_MAX

} ELDTColumnOrderPUBLIC_SYNONYMS;

extern eldtDefinitionTableDesc   gEldtTableDescPUBLIC_SYNONYMS;
extern eldtDefinitionColumnDesc  gEldtColumnDescPUBLIC_SYNONYMS[ELDT_Public_Synonyms_ColumnOrder_MAX];

/**
 * @brief PUBLIC SYNONYMS 테이블의 KEY 제약 조건
 * - PRIMARY KEY ( SYNONYM_ID )
 * - UNIQUE ( SYNONYM_NAME )
 */

typedef enum eldtKeyConstPUBLIC_SYNONYMS
{
    /*
     * Primary Key 영역
     */

    ELDT_Public_Synonyms_Const_PRIMARY_KEY = 0,

    /*
     * Unique 영역
     */

    ELDT_Public_Synonyms_Const_UNIQUE_SYNONYM_NAME,

    
    ELDT_Public_Synonyms_Const_MAX
    
} eldtKeyConstPUBLIC_SYNONYMS;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescPUBLIC_SYNONYMS[ELDT_Public_Synonyms_Const_MAX];


/** @} eldtPUBLIC_SYNONYMS */


#endif /* _ELDT_PUBLIC_SYNONYMS_H_ */
