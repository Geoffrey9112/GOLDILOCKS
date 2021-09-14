/*******************************************************************************
 * eldtREFERENTIAL_CONSTRAINTS.c
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
 * @file eldtREFERENTIAL_CONSTRAINTS.c
 * @brief DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtREFERENTIAL_CONSTRAINTS
 * @{
 */


    
/**
 * @brief DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescREFERENTIAL_CONSTRAINTS[ELDT_Reference_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "CONSTRAINT_OWNER_ID",                          /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_CONSTRAINT_OWNER_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the referential constraint"        
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "CONSTRAINT_SCHEMA_ID",                         /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_CONSTRAINT_SCHEMA_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                      /**< 컬럼의 Nullable 여부 */
        "schema identifier of the referential constraint"        
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "CONSTRAINT_ID",                                /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_CONSTRAINT_ID,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                      /**< 컬럼의 Nullable 여부 */
        "referential constraint identifier"
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "UNIQUE_CONSTRAINT_OWNER_ID",                   /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_OWNER_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the unique or primary key constraint "
        "applied to the referenced column list being described"
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "UNIQUE_CONSTRAINT_SCHEMA_ID",                  /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_SCHEMA_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "schema identifier of the unique or primary key constraint "
        "applied to the referenced column list being described"
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "UNIQUE_CONSTRAINT_ID",                         /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "constraint identifier of the unique or primary key constraint "
        "applied to the referenced column list being described"
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "MATCH_OPTION",                                 /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_MATCH_OPTION,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "The values of MATCH_OPTION have the following meanings:\n"
        "- SIMPLE : A match type of SIMPLE was specified\n"
        "- PARTIAL : A match type of PARTIAL was specified\n"
        "- FULL : A match type of FULL was specified\n"
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "MATCH_OPTION_ID",                              /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_MATCH_OPTION_ID,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "MATCH_OPTION identifier"
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "UPDATE_RULE",                                  /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_UPDATE_RULE,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "The values of UPDATE_RULE have the following meanings "
        "for a referential constraint that has an update rule:\n"
        "- NO ACTION : A referential action of NO ACTION was specified.\n"
        "- RESTRICT : A referential action of RESTRICT was specified.\n"
        "- CASCADE : A referential action of CASCADE was specified.\n"
        "- SET NULL : A referential action of SET NULL was specified.\n"
        "- SET DEFAULT : A referential action of SET DEFAULT was specified.\n"
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "UPDATE_RULE_ID",                               /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_UPDATE_RULE_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "UPDATE_RULE identifier"
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "DELETE_RULE",                                  /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_DELETE_RULE,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                         /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "The values of DEELTE_RULE have the following meanings "
        "for a referential constraint that has an delete rule:\n"
        "- NO ACTION : A referential action of NO ACTION was specified.\n"
        "- RESTRICT : A referential action of RESTRICT was specified.\n"
        "- CASCADE : A referential action of CASCADE was specified.\n"
        "- SET NULL : A referential action of SET NULL was specified.\n"
        "- SET DEFAULT : A referential action of SET DEFAULT was specified.\n"
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,          /**< Table ID */
        "DELETE_RULE_ID",                               /**< 컬럼의 이름  */
        ELDT_Reference_ColumnOrder_DELETE_RULE_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                  /**< 컬럼의 Nullable 여부 */
        "DELETE_RULE identifier"
    }
};


/**
 * @brief DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescREFERENTIAL_CONSTRAINTS =
{
    ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,                  /**< Table ID */
    "REFERENTIAL_CONSTRAINTS",                              /**< 테이블의 이름  */
    "The REFERENTIAL_CONSTRAINTS table has one row for each row "
    "in the TABLE_CONSTRAINTS table "
    "that has a CONSTRAINT_TYPE value of FOREIGN KEY."
};


/**
 * @brief DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescREFERENTIAL_CONSTRAINTS[ELDT_Reference_Const_MAX] =
{
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,                      /**< Table ID */
        ELDT_Reference_Const_PRIMARY_KEY,             /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Reference_ColumnOrder_CONSTRAINT_SCHEMA_ID,
            ELDT_Reference_ColumnOrder_CONSTRAINT_ID,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    },
};



/**
 * @brief DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescREFERENTIAL_CONSTRAINTS[ELDT_Reference_Index_MAX] =
{
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,       /**< Table ID */
        ELDT_Reference_Index_CONSTRAINT_OWNER_ID,    /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Reference_ColumnOrder_CONSTRAINT_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,       /**< Table ID */
        ELDT_Reference_Index_CONSTRAINT_SCHEMA_ID,   /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Reference_ColumnOrder_CONSTRAINT_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,       /**< Table ID */
        ELDT_Reference_Index_CONSTRAINT_ID,          /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Reference_ColumnOrder_CONSTRAINT_ID,
        }
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,       /**< Table ID */
        ELDT_Reference_Index_UNIQUE_CONSTRAINT_OWNER_ID,    /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,       /**< Table ID */
        ELDT_Reference_Index_UNIQUE_CONSTRAINT_SCHEMA_ID,   /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,       /**< Table ID */
        ELDT_Reference_Index_UNIQUE_CONSTRAINT_ID,   /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_ID,
        }
    }
};

/** @} eldtREFERENTIAL_CONSTRAINTS */

