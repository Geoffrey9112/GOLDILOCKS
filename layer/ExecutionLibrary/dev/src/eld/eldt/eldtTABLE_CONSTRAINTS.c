/*******************************************************************************
 * eldtTABLE_CONSTRAINTS.c
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
 * @file eldtTABLE_CONSTRAINTS.c
 * @brief DEFINITION_SCHEMA.TABLE_CONSTRAINTS 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtTABLE_CONSTRAINTS
 * @{
 */


/**
 * @brief DEFINITION_SCHEMA.TABLE_CONSTRAINTS 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescTABLE_CONSTRAINTS[ELDT_TableConst_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "CONSTRAINT_OWNER_ID",                  /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_CONSTRAINT_OWNER_ID,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the constraint"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "CONSTRAINT_SCHEMA_ID",                 /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_CONSTRAINT_SCHEMA_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "schema identifier of the constraint being described"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "CONSTRAINT_ID",                        /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SERIAL_NUMBER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "constraint identifier"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "TABLE_OWNER_ID",                       /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_OWNER_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the table to "
        "which the table constraint being described applies"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "TABLE_SCHEMA_ID",                      /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_SCHEMA_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "schema identifier of the table to "
        "which the table constraint being described applies"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "TABLE_ID",                             /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_TABLE_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "table identifier of the table to "
        "which the table constraint being described applies"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "CONSTRAINT_NAME",                      /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_CONSTRAINT_NAME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "constraint name"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "CONSTRAINT_TYPE",                      /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_CONSTRAINT_TYPE,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "The values of CONSTRAINT_TYPE have the following meanings:\n"
        "- PRIMARY KEY : The constraint being described is a primary key constraint.\n"
        "- UNIQUE : The constraint being described is a unique constraint.\n"
        "- FOREIGN KEY : The constraint being described is a foreign key constraint.\n"
        "- NOT NULL : The constraint being described is a NOT NULL column.\n"
        "- CHECK : The constraint being described is a check constraint.\n"
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "CONSTRAINT_TYPE_ID",                   /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_CONSTRAINT_TYPE_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,            /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "CONSTRAINT_TYPE identifier"
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "IS_DEFERRABLE",                        /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_IS_DEFERRABLE,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "The values of IS_DEFERRABLE have the following meanings:\n"
        "- TRUE : The table constraint is deferrable.\n"
        "- FALSE : The table constraint is not deferrable.\n"
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "INITIALLY_DEFERRED",                   /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_INITIALLY_DEFERRED,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "The values of INITIALLY_DEFERRED have the following meanings:\n"
        "- TRUE : The table constraint is initially deferred.\n"
        "- FALSE : The table constraint is initially immediate.\n"
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "ENFORCED",                             /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_ENFORCED,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "The values of ENFORCED have the following meanings:\n"
        "- TRUE : The table constraint is enforced.\n"
        "- FALSE : The table constraint is not enforced.\n"
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "ASSOCIATED_INDEX_ID",                  /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_ASSOCIATED_INDEX_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "index identifier if the constraint is associated with a index, "
        "null if the constraint is not associated with a index"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "VALIDATE",                             /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_VALIDATE,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "indicates whether the table constraint is VALIDATE or NOVALIDATE"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "IS_BUILTIN",                           /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_IS_BUILTIN, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "is built-in object or not"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "CREATED_TIME",                         /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_CREATED_TIME,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "created time of the constraint"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "MODIFIED_TIME",                        /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_MODIFIED_TIME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "last modified time of the constraint"        
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,        /**< Table ID */
        "COMMENTS",                             /**< 컬럼의 이름  */
        ELDT_TableConst_ColumnOrder_COMMENTS,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "comments of the constraint"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.TABLE_CONSTRAINTS 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescTABLE_CONSTRAINTS =
{
    ELDT_TABLE_ID_TABLE_CONSTRAINTS,                  /**< Table ID */
    "TABLE_CONSTRAINTS",                              /**< 테이블의 이름  */
    "The TABLE_CONSTRAINTS table has one row for each table constraint "
    "associated with a table. "
};


/**
 * @brief DEFINITION_SCHEMA.TABLE_CONSTRAINTS 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescTABLE_CONSTRAINTS[ELDT_TableConst_Const_MAX] =
{
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,             /**< Table ID */
        ELDT_TableConst_Const_PRIMARY_KEY,           /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TableConst_ColumnOrder_CONSTRAINT_SCHEMA_ID,
            ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,             /**< Table ID */
        ELDT_TableConst_Const_UNIQUE_CONSTRAINT_SCHEMA_ID_CONSTRAINT_NAME, /**< Const 번호*/
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TableConst_ColumnOrder_CONSTRAINT_SCHEMA_ID,
            ELDT_TableConst_ColumnOrder_CONSTRAINT_NAME,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};



/**
 * @brief DEFINITION_SCHEMA.TABLE_CONSTRAINTS 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescTABLE_CONSTRAINTS[ELDT_TableConst_Index_MAX] =
{
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,             /**< Table ID */
        ELDT_TableConst_Index_CONSTRAINT_OWNER_ID,   /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TableConst_ColumnOrder_CONSTRAINT_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,             /**< Table ID */
        ELDT_TableConst_Index_CONSTRAINT_SCHEMA_ID,  /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TableConst_ColumnOrder_CONSTRAINT_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,             /**< Table ID */
        ELDT_TableConst_Index_CONSTRAINT_ID,         /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
        }
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,             /**< Table ID */
        ELDT_TableConst_Index_OWNER_ID,             /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TableConst_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,             /**< Table ID */
        ELDT_TableConst_Index_SCHEMA_ID,             /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TableConst_ColumnOrder_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,             /**< Table ID */
        ELDT_TableConst_Index_TABLE_ID,              /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TableConst_ColumnOrder_TABLE_ID,
        }
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,             /**< Table ID */
        ELDT_TableConst_Index_CONSTRAINT_NAME,       /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TableConst_ColumnOrder_CONSTRAINT_NAME,
        }
    },
    {
        ELDT_TABLE_ID_TABLE_CONSTRAINTS,             /**< Table ID */
        ELDT_TableConst_Index_ASSOCIATED_INDEX_ID,   /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TableConst_ColumnOrder_ASSOCIATED_INDEX_ID,
        }
    }
};


/** @} eldtTABLE_CONSTRAINTS */

