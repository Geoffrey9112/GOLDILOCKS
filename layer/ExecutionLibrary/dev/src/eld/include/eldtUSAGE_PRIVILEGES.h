/*******************************************************************************
 * eldtUSAGE_PRIVILEGES.h
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


#ifndef _ELDT_USAGE_PRIVILEGES_H_
#define _ELDT_USAGE_PRIVILEGES_H_ 1

/**
 * @file eldtUSAGE_PRIVILEGES.h
 * @brief USAGE_PRIVILEGES dictionary base table
 */

/**
 * @defgroup eldtUSAGE_PRIVILEGES USAGE_PRIVILEGES table
 * @ingroup eldtStandard
 * @{
 */

/**
 * @brief USAGE_PRIVILEGES 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderUSAGE_PRIVILEGES
{
    /*
     * 식별자 영역
     */

    ELDT_UsagePriv_ColumnOrder_GRANTOR_ID = 0,
    ELDT_UsagePriv_ColumnOrder_GRANTEE_ID,
    ELDT_UsagePriv_ColumnOrder_SCHEMA_ID,
    ELDT_UsagePriv_ColumnOrder_OBJECT_ID,
    
    /*
     * SQL 표준 영역
     */

    /*
    ELDT_UsagePriv_ColumnOrder_GRANTOR,
    ELDT_UsagePriv_ColumnOrder_GRANTEE,
    ELDT_UsagePriv_ColumnOrder_OBJECT_CATALOG,
    ELDT_UsagePriv_ColumnOrder_OBJECT_SCHEMA,
    ELDT_UsagePriv_ColumnOrder_OBJECT_NAME,
    */
    
    ELDT_UsagePriv_ColumnOrder_OBJECT_TYPE,
    ELDT_UsagePriv_ColumnOrder_OBJECT_TYPE_ID,       /* 비표준 */
    ELDT_UsagePriv_ColumnOrder_IS_GRANTABLE,
    ELDT_UsagePriv_ColumnOrder_IS_BUILTIN,
    
    /*
     * 부가 정보 영역
     */
    
    ELDT_UsagePriv_ColumnOrder_MAX
    
} eldtColumnOrderUSAGE_PRIVILEGES;
    

extern eldtDefinitionTableDesc   gEldtTableDescUSAGE_PRIVILEGES;
extern eldtDefinitionColumnDesc  gEldtColumnDescUSAGE_PRIVILEGES[ELDT_UsagePriv_ColumnOrder_MAX];


/**
 * @brief USAGE_PRIVILEGES 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( GRANTOR, GRANTEE, OBJECT_CATALOG, OBJECT_SCHEMA, OBJECT_NAME, OBJECT_TYPE )
 *   - => : PRIMARY KEY ( GRANTOR_ID, GRANTEE_ID, SCHEMA_ID, OBJECT_ID, OBJECT_TYPE_ID )
 * - 비표준
 */

typedef enum eldtKeyConstUSAGE_PRIVILEGES
{
    /*
     * Primary Key 영역
     */

    ELDT_UsagePriv_Const_PRIMARY_KEY = 0,             /**< SQL 1 */
    
    /*
     * Unique 영역
     */

    
    /*
     * Foreign Key 영역
     */

    ELDT_UsagePriv_Const_MAX
    
} eldtKeyConstUSAGE_PRIVILEGES;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescUSAGE_PRIVILEGES[ELDT_UsagePriv_Const_MAX];


/**
 * @brief USAGE_PRIVILEGES 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexUSAGE_PRIVILEGES
{
    ELDT_UsagePriv_Index_GRANTOR_ID = 0,
    ELDT_UsagePriv_Index_GRANTEE_ID,
    ELDT_UsagePriv_Index_SCHEMA_ID,
    ELDT_UsagePriv_Index_OBJECT_ID,

    ELDT_UsagePriv_Index_MAX
    
} eldtIndexUSAGE_PRIVILEGES;

extern eldtDefinitionIndexDesc  gEldtIndexDescUSAGE_PRIVILEGES[ELDT_UsagePriv_Index_MAX];



/** @} eldtUSAGE_PRIVILEGES */



#endif /* _ELDT_USAGE_PRIVILEGES_H_ */
