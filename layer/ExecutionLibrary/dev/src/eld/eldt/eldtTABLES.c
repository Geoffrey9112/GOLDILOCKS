/*******************************************************************************
 * eldtTABLES.c
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
 * @file eldtTABLES.c
 * @brief DEFINITION_SCHEMA.TABLES 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtTABLES
 * @{
 */


/**
 * @brief DEFINITION_SCHEMA.TABLES 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescTABLES[ELDT_Tables_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_TABLES,                   /**< Table ID */
        "OWNER_ID",                             /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_OWNER_ID,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the table"        
    },
    {
        ELDT_TABLE_ID_TABLES,                   /**< Table ID */
        "SCHEMA_ID",                            /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_SCHEMA_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "schema identifier of the table"        
    },
    {
        ELDT_TABLE_ID_TABLES,                   /**< Table ID */
        "TABLE_ID",                             /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_TABLE_ID,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SERIAL_NUMBER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "table identifier"        
    },
    {
        ELDT_TABLE_ID_TABLES,                   /**< Table ID */
        "TABLESPACE_ID",                        /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_TABLESPACE_ID,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "tablespace identifier of the table"        
    },
    {
        ELDT_TABLE_ID_TABLES,                   /**< Table ID */
        "PHYSICAL_ID",                          /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_PHYSICAL_ID,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "physical identifier of the table"        
    },
    {
        ELDT_TABLE_ID_TABLES,                   /**< Table ID */
        "TABLE_NAME",                           /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_TABLE_NAME,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "table name"        
    },
    {
        ELDT_TABLE_ID_TABLES,                   /**< Table ID */
        "TABLE_TYPE",                           /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_TABLE_TYPE,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "The values of TABLE_TYPE have the following meanings:\n"
        "- BASE TABLE : a persistent base table\n"
        "- VIEW : a viewed table\n"
        "- GLOBAL TEMPORARY : unsupported feature\n"
        "- LOCAL TEMPORARY : unsupported feature\n"
        "- SYSTEM VERSIONED : unsupported feature\n"
        "- SEQUENCE : sequence object\n"
        "- FIXED TABLE : a fixed table\n"
        "- DUMP TABLE : a fixed dump table\n"
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "TABLE_TYPE_ID",                         /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_TABLE_TYPE_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "TABLE_TYPE identifier"
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "SYSTEM_VERSION_START_COLUMN_NAME",      /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_SYSTEM_VERSION_START_COLUMN_NAME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "SYSTEM_VERSION_END_COLUMN_NAME",        /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_SYSTEM_VERSION_END_COLUMN_NAME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "SYSTEM_VERSION_RETENTION_PERIOD",       /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_SYSTEM_VERSION_RETENTION_PERIOD,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "SELF_REFERENCING_COLUMN_NAME",          /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_SELF_REFERENCING_COLUMN_NAME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "REFERENCE_GENERATION",                  /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_REFERENCE_GENERATION,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "USER_DEFINED_TYPE_CATALOG",             /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_USER_DEFINED_TYPE_CATALOG,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "USER_DEFINED_TYPE_SCHEMA",              /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_USER_DEFINED_TYPE_SCHEMA,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "USER_DEFINED_TYPE_NAME",                /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_USER_DEFINED_TYPE_NAME,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "IS_INSERTABLE_INTO",                    /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_IS_INSERTABLE_INTO,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "is insertable into the table"        
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "IS_TYPED",                              /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_IS_TYPED,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "COMMIT_ACTION",                         /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_COMMIT_ACTION,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "IS_SET_SUPPLOG_PK",                     /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_IS_SET_SUPPLOG_PK,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "is set supplemental log data(primary key)"        
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "IS_BUILTIN",                            /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_IS_BUILTIN,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "is built-in object or not"        
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "CREATED_TIME",                          /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_CREATED_TIME,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "created time of the table"        
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "MODIFIED_TIME",                         /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_MODIFIED_TIME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "last modified time of the table"        
    },
    {
        ELDT_TABLE_ID_TABLES,                    /**< Table ID */
        "COMMENTS",                              /**< 컬럼의 이름  */
        ELDT_Tables_ColumnOrder_COMMENTS,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "comments of the table"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.TABLES 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescTABLES =
{
    ELDT_TABLE_ID_TABLES,                  /**< Table ID */
    "TABLES",                              /**< 테이블의 이름  */
    "The TABLES table contains one row for each table including views."
};


/**
 * @brief DEFINITION_SCHEMA.TABLES 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescTABLES[ELDT_Tables_Const_MAX] =
{
    {
        ELDT_TABLE_ID_TABLES,                        /**< Table ID */
        ELDT_Tables_Const_PRIMARY_KEY,               /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Tables_ColumnOrder_SCHEMA_ID,
            ELDT_Tables_ColumnOrder_TABLE_ID,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_TABLES,                        /**< Table ID */
        ELDT_Tables_Const_UNIQUE_SCHEMA_ID_TABLE_NAME,      /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Tables_ColumnOrder_SCHEMA_ID,
            ELDT_Tables_ColumnOrder_TABLE_NAME,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_TABLES,                        /**< Table ID */
        ELDT_Tables_Const_FOREIGN_KEY_SCHEMATA,      /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Tables_ColumnOrder_SCHEMA_ID,
        },
        ELDT_TABLE_ID_SCHEMATA,                      /**< 참조 제약이 참조하는 Table ID */
        ELDT_Schemata_Const_PRIMARY_KEY,             /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};



/**
 * @brief DEFINITION_SCHEMA.TABLES 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescTABLES[ELDT_Tables_Index_MAX] =
{
    {
        ELDT_TABLE_ID_TABLES,                        /**< Table ID */
        ELDT_Tables_Index_OWNER_ID,                  /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Tables_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_TABLES,                        /**< Table ID */
        ELDT_Tables_Index_TABLE_ID,                  /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Tables_ColumnOrder_TABLE_ID,
        }
    },
    {
        ELDT_TABLE_ID_TABLES,                        /**< Table ID */
        ELDT_Tables_Index_TABLESPACE_ID,             /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Tables_ColumnOrder_TABLESPACE_ID,
        }
    },
    {
        ELDT_TABLE_ID_TABLES,                        /**< Table ID */
        ELDT_Tables_Index_PHYSICAL_ID,               /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Tables_ColumnOrder_PHYSICAL_ID,
        }
    },
    {
        ELDT_TABLE_ID_TABLES,                        /**< Table ID */
        ELDT_Tables_Index_TABLE_NAME,                /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Tables_ColumnOrder_TABLE_NAME,
        }
    }
};

/** @} eldtTABLES */

