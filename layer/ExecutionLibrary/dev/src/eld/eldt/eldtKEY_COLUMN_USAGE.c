/*******************************************************************************
 * eldtKEY_COLUMN_USAGE.c
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
 * @file eldtKEY_COLUMN_USAGE.c
 * @brief DEFINITION_SCHEMA.KEY_COLUMN_USAGE 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtKEY_COLUMN_USAGE
 * @{
 */


    
/**
 * @brief DEFINITION_SCHEMA.KEY_COLUMN_USAGE 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescKEY_COLUMN_USAGE[ELDT_KeyColumn_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,                /**< Table ID */
        "CONSTRAINT_OWNER_ID",                         /**< 컬럼의 이름  */
        ELDT_KeyColumn_ColumnOrder_CONSTRAINT_OWNER_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the key constraint"        
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,                /**< Table ID */
        "CONSTRAINT_SCHEMA_ID",                        /**< 컬럼의 이름  */
        ELDT_KeyColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "schema identifier of the key constraint"        
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,                /**< Table ID */
        "CONSTRAINT_ID",                               /**< 컬럼의 이름  */
        ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "key constraint identifier of the constraint being described"
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,                /**< Table ID */
        "TABLE_OWNER_ID",                              /**< 컬럼의 이름  */
        ELDT_KeyColumn_ColumnOrder_OWNER_ID,           /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the table of the column that participates "
        "in the unique, primary key, or foreign key constraint being described."
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,                /**< Table ID */
        "TABLE_SCHEMA_ID",                             /**< 컬럼의 이름  */
        ELDT_KeyColumn_ColumnOrder_SCHEMA_ID,          /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "schema identifier of the column that participates "
        "in the unique, primary key, or foreign key constraint being described."
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,                /**< Table ID */
        "TABLE_ID",                                    /**< 컬럼의 이름  */
        ELDT_KeyColumn_ColumnOrder_TABLE_ID,           /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "table identifier of the column that participates "
        "in the unique, primary key, or foreign key constraint being described."
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,                /**< Table ID */
        "COLUMN_ID",                                   /**< 컬럼의 이름  */
        ELDT_KeyColumn_ColumnOrder_COLUMN_ID,          /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "column identifier of the column that participates "
        "in the unique, primary key, or foreign key constraint being described."
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,                /**< Table ID */
        "ORDINAL_POSITION",                            /**< 컬럼의 이름  */
        ELDT_KeyColumn_ColumnOrder_ORDINAL_POSITION,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "the ordinal position of the specific column "
        "in the constraint being described. "
        "If the constraint described is a key of cardinality 1 (one), "
        "then the value of ORDINAL_POSITION is always 1 (one)."
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,                /**< Table ID */
        "POSITION_IN_UNIQUE_CONSTRAINT",               /**< 컬럼의 이름  */
        ELDT_KeyColumn_ColumnOrder_POSITION_IN_UNIQUE_CONSTRAINT,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "If the constraint being described is a foreign key constraint, "
        "then the value of POSITION_IN_UNIQUE_CONSTRAINT is "
        "the ordinal position of the referenced column corresponding to "
        "the referencing column being described, "
        "in the corresponding unique key constraint. "
        "Otherwise, the value of POSITION_IN_UNIQUE_CONSTRAINT is the null value."
    }
};


/**
 * @brief DEFINITION_SCHEMA.KEY_COLUMN_USAGE 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescKEY_COLUMN_USAGE =
{
    ELDT_TABLE_ID_KEY_COLUMN_USAGE,                  /**< Table ID */
    "KEY_COLUMN_USAGE",                              /**< 테이블의 이름  */
    "The KEY_COLUMN_USAGE table has one or more rows for each row "
    "in the TABLE_CONSTRAINTS table "
    "that has a CONSTRAINT_TYPE of UNIQUE, PRIMARY KEY, or FOREIGN KEY. "
    "The rows list the columns that constitute each unique constraint, "
    "and the referencing columns in each foreign key constraint."
};


/**
 * @brief DEFINITION_SCHEMA.KEY_COLUMN_USAGE 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescKEY_COLUMN_USAGE[ELDT_KeyColumn_Const_MAX] =
{
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,              /**< Table ID */
        ELDT_KeyColumn_Const_PRIMARY_KEY,            /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        3,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KeyColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,
            ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID,
            ELDT_KeyColumn_ColumnOrder_COLUMN_ID,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    /* { */
    /*     ELDT_TABLE_ID_KEY_COLUMN_USAGE,                      /\**< Table ID *\/ */
    /*     ELDT_KeyColumn_Const_UNIQUE_XXXX_POSITION_IN_UNIQUE_CONSTRAINT, /\**< Const 번호 *\/ */
    /*     ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /\**< Table Constraint 유형 *\/ */
    /*     3,                                           /\**< 키를 구성하는 컬럼의 개수 *\/ */
    /*     {                                /\**< 키를 구성하는 컬럼의 테이블내 Ordinal Position *\/ */
    /*         ELDT_KeyColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID, */
    /*         ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID, */
    /*         ELDT_KeyColumn_ColumnOrder_POSITION_IN_UNIQUE_CONSTRAINT, */
    /*     }, */
    /*     ELL_DICT_OBJECT_ID_NA,            /\**< 참조 제약이 참조하는 Table ID *\/ */
    /*     ELL_DICT_OBJECT_ID_NA,            /\**< 참조 제약이 참조하는 Unique의 번호 *\/ */
    /* }, */
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,              /**< Table ID */
        ELDT_KeyColumn_Const_UNIQUE_XXXX_ORDINAL_POSITION, /**< Const 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        3,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KeyColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,
            ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID,
            ELDT_KeyColumn_ColumnOrder_ORDINAL_POSITION,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,              /**< Table ID */
        ELDT_KeyColumn_Const_FOREIGN_KEY_COLUMNS,    /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,       /**< Table Constraint 유형 */
        3,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KeyColumn_ColumnOrder_SCHEMA_ID,
            ELDT_KeyColumn_ColumnOrder_TABLE_ID,
            ELDT_KeyColumn_ColumnOrder_COLUMN_ID,
        },
        ELDT_TABLE_ID_COLUMNS,                       /**< 참조 제약이 참조하는 Table ID */
        ELDT_Columns_Const_PRIMARY_KEY,              /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};



/**
 * @brief DEFINITION_SCHEMA.KEY_COLUMN_USAGE 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescKEY_COLUMN_USAGE[ELDT_KeyColumn_Index_MAX] =
{
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,              /**< Table ID */
        ELDT_KeyColumn_Index_CONSTRAINT_OWNER_ID,    /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KeyColumn_ColumnOrder_CONSTRAINT_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,              /**< Table ID */
        ELDT_KeyColumn_Index_CONSTRAINT_SCHEMA_ID,   /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KeyColumn_ColumnOrder_CONSTRAINT_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,              /**< Table ID */
        ELDT_KeyColumn_Index_CONSTRAINT_ID,          /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID,
        }
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,              /**< Table ID */
        ELDT_KeyColumn_Index_OWNER_ID,               /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KeyColumn_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,              /**< Table ID */
        ELDT_KeyColumn_Index_SCHEMA_ID,              /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KeyColumn_ColumnOrder_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,              /**< Table ID */
        ELDT_KeyColumn_Index_TABLE_ID,               /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KeyColumn_ColumnOrder_TABLE_ID,
        }
    },
    {
        ELDT_TABLE_ID_KEY_COLUMN_USAGE,              /**< Table ID */
        ELDT_KeyColumn_Index_COLUMN_ID,              /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_KeyColumn_ColumnOrder_COLUMN_ID,
        }
    }
};



/** @} eldtKEY_COLUMN_USAGE */

