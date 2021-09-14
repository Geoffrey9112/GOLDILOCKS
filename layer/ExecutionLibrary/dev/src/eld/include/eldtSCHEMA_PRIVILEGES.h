/*******************************************************************************
 * eldtSCHEMA_PRIVILEGES.h
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


#ifndef _ELDT_SCHEMA_PRIVILEGES_H_
#define _ELDT_SCHEMA_PRIVILEGES_H_ 1

/**
 * @file eldtSCHEMA_PRIVILEGES.h
 * @brief SCHEMA_PRIVILEGES dictionary base table
 */

/**
 * @defgroup eldtSCHEMA_PRIVILEGES SCHEMA_PRIVILEGES table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief SCHEMA_PRIVILEGES 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderSCHEMA_PRIVILEGES
{
    /*
     * 식별자 영역
     */

    ELDT_SchemaPriv_ColumnOrder_GRANTOR_ID = 0,
    ELDT_SchemaPriv_ColumnOrder_GRANTEE_ID,
    ELDT_SchemaPriv_ColumnOrder_SCHEMA_ID,
    
    /*
     * SQL 표준 영역
     */
    
    /*
     * 부가 정보 영역
     */

    ELDT_SchemaPriv_ColumnOrder_PRIVILEGE_TYPE,
    ELDT_SchemaPriv_ColumnOrder_PRIVILEGE_TYPE_ID,  
    ELDT_SchemaPriv_ColumnOrder_IS_GRANTABLE,
    ELDT_SchemaPriv_ColumnOrder_IS_BUILTIN,
    
    ELDT_SchemaPriv_ColumnOrder_MAX
    
} eldtColumnOrderSCHEMA_PRIVILEGES;
    

extern eldtDefinitionTableDesc   gEldtTableDescSCHEMA_PRIVILEGES;
extern eldtDefinitionColumnDesc  gEldtColumnDescSCHEMA_PRIVILEGES[ELDT_SchemaPriv_ColumnOrder_MAX];


/**
 * @brief SCHEMA_PRIVILEGES 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 * - 비표준
 *  - 비표준 1 :PRIMARY KEY (GRANTOR_ID, GRANTEE_ID, SCHEMA_ID, PRIVILEGE_TYPE_ID) 
 */

typedef enum eldtKeyConstSCHEMA_PRIVILEGES
{
    /*
     * Primary Key 영역
     */

    ELDT_SchemaPriv_Const_PRIMARY_KEY = 0,             /**< 비표준 1 */
    
    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    
    ELDT_SchemaPriv_Const_MAX
    
} eldtKeyConstSCHEMA_PRIVILEGES;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescSCHEMA_PRIVILEGES[ELDT_SchemaPriv_Const_MAX];


/**
 * @brief SCHEMA_PRIVILEGES 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexSCHEMA_PRIVILEGES
{
    ELDT_SchemaPriv_Index_GRANTOR_ID = 0,
    ELDT_SchemaPriv_Index_GRANTEE_ID,
    ELDT_SchemaPriv_Index_SCHEMA_ID,

    ELDT_SchemaPriv_Index_MAX
    
} eldtIndexSCHEMA_PRIVILEGES;

extern eldtDefinitionIndexDesc  gEldtIndexDescSCHEMA_PRIVILEGES[ELDT_SchemaPriv_Index_MAX];


/** @} eldtSCHEMA_PRIVILEGES */



#endif /* _ELDT_SCHEMA_PRIVILEGES_H_ */
