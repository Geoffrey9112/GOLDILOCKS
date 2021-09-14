/*******************************************************************************
 * eldtCOLUMNS.c
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
 * @file eldtCOLUMNS.c
 * @brief DEFINITION_SCHEMA.COLUMNS 정의 명세  
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtCOLUMNS
 * @{
 */

/**
 * @brief DEFINITION_SCHEMA.COLUMNS 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescCOLUMNS[ELDT_Columns_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "OWNER_ID",                             /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_OWNER_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the table of the column"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "SCHEMA_ID",                            /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_SCHEMA_ID,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "schema identifier of the column"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "TABLE_ID",                             /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_TABLE_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "table identifier of the column"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "COLUMN_ID",                            /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_COLUMN_ID,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SERIAL_NUMBER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "column identifier"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "COLUMN_NAME",                          /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_COLUMN_NAME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "column name"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "PHYSICAL_ORDINAL_POSITION",            /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_PHYSICAL_ORDINAL_POSITION,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,            /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "the physical ordinal position (> 0) of the column in the table including unused columns"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "LOGICAL_ORDINAL_POSITION",            /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_LOGICAL_ORDINAL_POSITION,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,            /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "the logical ordinal position (> 0) of the column in the table without unused columns"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "DTD_IDENTIFIER",                       /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_DTD_IDENTIFIER,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "data type descriptor identifier, equal to COLUMN_ID"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "DOMAIN_CATALOG",                       /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_DOMAIN_CATALOG,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "DOMAIN_SCHEMA",                        /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_DOMAIN_SCHEMA, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "DOMAIN_NAME",                          /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_DOMAIN_NAME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "COLUMN_DEFAULT",                       /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_COLUMN_DEFAULT,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_SYNTAX,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "the default for the column"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IS_NULLABLE",                          /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IS_NULLABLE,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "is nullable of the column"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IS_SELF_REFERENCING",                  /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IS_SELF_REFERENCING,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IS_IDENTITY",                          /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IS_IDENTITY,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "is identity column"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IDENTITY_GENERATION",                  /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IDENTITY_GENERATION,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "The values of IDENTITY_GENERATION have the following meanings:\n"
        "- ALWAYS : values are always generated.\n"
        "- BY DEFAULT : values are generated by default.\n"
        "- null : The column is not an identity column.\n"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IDENTITY_GENERATION_ID",               /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IDENTITY_GENERATION_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,            /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "IDENTITY_GENERATION identifier"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IDENTITY_START",                       /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IDENTITY_START,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "the start value of the identity column"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IDENTITY_INCREMENT",                   /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IDENTITY_INCREMENT,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "the increment of the identity column"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IDENTITY_MAXIMUM",                     /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IDENTITY_MAXIMUM,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "the maximum value of the identity column"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IDENTITY_MINIMUM",                     /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IDENTITY_MINIMUM,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "the minimum value of the identity column"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IDENTITY_CYCLE",                       /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IDENTITY_CYCLE,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "The values of IDENTITY_CYCLE have the following meanings:\n"
        "- TRUE : The cycle option of the column is CYCLE.\n"
        "- FALSE : The cycle option of the column is NO CYCLE.\n"
        "- null : The column is not an identity column.\n"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IDENTITY_TABLESPACE_ID",               /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IDENTITY_TABLESPACE_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "tablespace identifier of the identity column"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IDENTITY_PHYSICAL_ID",                 /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IDENTITY_PHYSICAL_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "physical identifier of the identity column"        
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IDENTITY_CACHE_SIZE",                  /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IDENTITY_CACHE_SIZE,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "number of sequence numbers to cache for identity column"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IS_GENERATED",                         /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IS_GENERATED,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "unsupported feature"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IS_SYSTEM_VERSION_START",              /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IS_SYSTEM_VERSION_START,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "unsupported feature"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IS_SYSTEM_VERSION_END",                /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IS_SYSTEM_VERSION_END,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "unsupported feature"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "SYSTEM_VERSION_TIMESTAMP_GENERATION",  /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_SYSTEM_VERSION_TIMESTAMP_GENERATION, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "unsupported feature"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IS_UPDATABLE",                         /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IS_UPDATABLE,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "is updatable column"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "IS_UNUSED",                            /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_IS_UNUSED,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "is unused column"
    },
    {
        ELDT_TABLE_ID_COLUMNS,                  /**< Table ID */
        "COMMENTS",                             /**< 컬럼의 이름  */
        ELDT_Columns_ColumnOrder_COMMENTS,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "comments of the column"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.COLUMNS 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescCOLUMNS =
{
    ELDT_TABLE_ID_COLUMNS,                 /**< Table ID */
    "COLUMNS",                             /**< 테이블의 이름  */
    "The COLUMNS table has one row for each column."
};


/**
 * @brief DEFINITION_SCHEMA.COLUMNS 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescCOLUMNS[ELDT_Columns_Const_MAX] =
{
    {
        ELDT_TABLE_ID_COLUMNS,                       /**< Table ID */
        ELDT_Columns_Const_PRIMARY_KEY,              /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        3,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Columns_ColumnOrder_SCHEMA_ID,
            ELDT_Columns_ColumnOrder_TABLE_ID,
            ELDT_Columns_ColumnOrder_COLUMN_ID,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_COLUMNS,                       /**< Table ID */
        ELDT_Columns_Const_UNIQUE_SCHEMA_ID_TABLE_ID_PHYSICAL_ORDINAL_POSITION, /**< Const 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        3,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Columns_ColumnOrder_SCHEMA_ID,
            ELDT_Columns_ColumnOrder_TABLE_ID,
            ELDT_Columns_ColumnOrder_PHYSICAL_ORDINAL_POSITION,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    /* { */
    /*     ELDT_TABLE_ID_COLUMNS,                       /\**< Table ID *\/ */
    /*     ELDT_Columns_Const_UNIQUE_SCHEMA_ID_TABLE_ID_LOGICAL_ORDINAL_POSITION, /\**< Const 번호 *\/ */
    /*     ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /\**< Table Constraint 유형 *\/ */
    /*     3,                                           /\**< 키를 구성하는 컬럼의 개수 *\/ */
    /*     {                                /\**< 키를 구성하는 컬럼의 테이블내 Ordinal Position *\/ */
    /*         ELDT_Columns_ColumnOrder_SCHEMA_ID, */
    /*         ELDT_Columns_ColumnOrder_TABLE_ID, */
    /*         ELDT_Columns_ColumnOrder_LOGICAL_ORDINAL_POSITION, */
    /*     }, */
    /*     ELL_DICT_OBJECT_ID_NA,            /\**< 참조 제약이 참조하는 Table ID *\/ */
    /*     ELL_DICT_OBJECT_ID_NA,            /\**< 참조 제약이 참조하는 Unique의 번호 *\/ */
    /* }, */
    {
        ELDT_TABLE_ID_COLUMNS,                       /**< Table ID */
        ELDT_Columns_Const_FOREIGN_KEY_TABLES,       /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,       /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Columns_ColumnOrder_SCHEMA_ID,
            ELDT_Columns_ColumnOrder_TABLE_ID,
        },
        ELDT_TABLE_ID_TABLES,                        /**< 참조 제약이 참조하는 Table ID */
        ELDT_Tables_Const_PRIMARY_KEY,               /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/**
 * @brief DEFINITION_SCHEMA.COLUMNS 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescCOLUMNS[ELDT_Columns_Index_MAX] =
{
    {
        ELDT_TABLE_ID_COLUMNS,                       /**< Table ID */
        ELDT_Columns_Index_OWNER_ID,                 /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Columns_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_COLUMNS,                       /**< Table ID */
        ELDT_Columns_Index_SCHEMA_ID,                /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Columns_ColumnOrder_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_COLUMNS,                       /**< Table ID */
        ELDT_Columns_Index_TABLE_ID,                 /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Columns_ColumnOrder_TABLE_ID,
        }
    },
    {
        ELDT_TABLE_ID_COLUMNS,                       /**< Table ID */
        ELDT_Columns_Index_COLUMN_ID,                /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Columns_ColumnOrder_COLUMN_ID,
        }
    },
    {
        ELDT_TABLE_ID_COLUMNS,                       /**< Table ID */
        ELDT_Columns_Index_COLUMN_NAME,              /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Columns_ColumnOrder_COLUMN_NAME,
        }
    },
    {
        ELDT_TABLE_ID_COLUMNS,                       /**< Table ID */
        ELDT_Columns_Index_DTD_IDENTIFIER,           /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Columns_ColumnOrder_DTD_IDENTIFIER,
        }
    },
    {
        ELDT_TABLE_ID_COLUMNS,                       /**< Table ID */
        ELDT_Columns_Index_IDENTITY_TABLESPACE_ID,   /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Columns_ColumnOrder_IDENTITY_TABLESPACE_ID,
        }
    },
    {
        ELDT_TABLE_ID_COLUMNS,                       /**< Table ID */
        ELDT_Columns_Index_IDENTITY_PHYSICAL_ID,     /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Columns_ColumnOrder_IDENTITY_PHYSICAL_ID,
        }
    }
};


/** @} eldtCOLUMNS */

