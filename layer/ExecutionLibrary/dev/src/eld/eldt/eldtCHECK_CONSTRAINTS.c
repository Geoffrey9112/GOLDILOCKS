/*******************************************************************************
 * eldtCHECK_CONSTRAINTS.c
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


/**
 * @file eldtCHECK_CONSTRAINTS.c
 * @brief DEFINITION_SCHEMA.CHECK_CONSTRAINTS 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtCHECK_CONSTRAINTS
 * @{
 */


    
/**
 * @brief DEFINITION_SCHEMA.CHECK_CONSTRAINTS 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescCHECK_CONSTRAINTS[ELDT_CheckConst_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_CHECK_CONSTRAINTS,                /**< Table ID */
        "CONSTRAINT_OWNER_ID",                          /**< 컬럼의 이름  */
        ELDT_CheckConst_ColumnOrder_CONSTRAINT_OWNER_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the check constraint"        
    },
    {
        ELDT_TABLE_ID_CHECK_CONSTRAINTS,                /**< Table ID */
        "CONSTRAINT_SCHEMA_ID",                         /**< 컬럼의 이름  */
        ELDT_CheckConst_ColumnOrder_CONSTRAINT_SCHEMA_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                      /**< 컬럼의 Nullable 여부 */
        "schema identifier of the check constraint"        
    },
    {
        ELDT_TABLE_ID_CHECK_CONSTRAINTS,                /**< Table ID */
        "CONSTRAINT_ID",                                /**< 컬럼의 이름  */
        ELDT_CheckConst_ColumnOrder_CONSTRAINT_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                      /**< 컬럼의 Nullable 여부 */
        "check constraint identifier"
    },
    {
        ELDT_TABLE_ID_CHECK_CONSTRAINTS,                /**< Table ID */
        "CHECK_CLAUSE",                                 /**< 컬럼의 이름  */
        ELDT_CheckConst_ColumnOrder_CHECK_CLAUSE,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_SYNTAX,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                              /**< 컬럼의 Nullable 여부 */
        "the character representation of the search condition contained "
        "in the check constraint definition, domain constraint, or assertion definition "
        "that defined the check constraint being described can be represented "
        "without truncation.  if null, it represents NOT NULL check constraint"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.CHECK_CONSTRAINTS 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescCHECK_CONSTRAINTS =
{
    ELDT_TABLE_ID_CHECK_CONSTRAINTS,                  /**< Table ID */
    "CHECK_CONSTRAINTS",                              /**< 테이블의 이름  */
    "The CHECK_CONSTRAINTS table has one row for each domain constraint, "
    "table check constraint, and assertion."
};


/**
 * @brief DEFINITION_SCHEMA.CHECK_CONSTRAINTS 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescCHECK_CONSTRAINTS[ELDT_CheckConst_Const_MAX] =
{
    {
        ELDT_TABLE_ID_CHECK_CONSTRAINTS,             /**< Table ID */
        ELDT_CheckConst_Const_PRIMARY_KEY,           /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                  /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckConst_ColumnOrder_CONSTRAINT_SCHEMA_ID,
            ELDT_CheckConst_ColumnOrder_CONSTRAINT_ID
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};



/**
 * @brief DEFINITION_SCHEMA.CHECK_CONSTRAINTS 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescCHECK_CONSTRAINTS[ELDT_CheckConst_Index_MAX] =
{
    {
        ELDT_TABLE_ID_CHECK_CONSTRAINTS,             /**< Table ID */
        ELDT_CheckConst_Index_CONSTRAINT_OWNER_ID,   /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckConst_ColumnOrder_CONSTRAINT_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_CHECK_CONSTRAINTS,             /**< Table ID */
        ELDT_CheckConst_Index_CONSTRAINT_SCHEMA_ID,  /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckConst_ColumnOrder_CONSTRAINT_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_CHECK_CONSTRAINTS,             /**< Table ID */
        ELDT_CheckConst_Index_CONSTRAINT_ID,         /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_CheckConst_ColumnOrder_CONSTRAINT_ID,
        }
    }
};

/** @} eldtCHECK_CONSTRAINTS */

