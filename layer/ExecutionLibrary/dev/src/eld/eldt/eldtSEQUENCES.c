/*******************************************************************************
 * eldtSEQUENCES.c
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
 * @file eldtSEQUENCES.c
 * @brief DEFINITION_SCHEMA.SEQUENCES 정의 명세  
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtSEQUENCES
 * @{
 */

/**
 * @brief DEFINITION_SCHEMA.SEQUENCES 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescSEQUENCES[ELDT_Sequences_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "OWNER_ID",                             /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_OWNER_ID,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the table of the sequence generator"        
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "SCHEMA_ID",                            /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_SCHEMA_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "schema identifier of the sequence generator"        
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "SEQUENCE_ID",                          /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_SEQUENCE_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SERIAL_NUMBER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "sequence generator identifier"        
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "SEQUENCE_TABLE_ID",                    /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_SEQUENCE_TABLE_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "table id of sequence for naming resolution"        
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "TABLESPACE_ID",                        /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_TABLESPACE_ID,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "tablespace identifier of the seqence generator"        
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "PHYSICAL_ID",                          /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_PHYSICAL_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "physical identifier of the sequence generator"        
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "SEQUENCE_NAME",                        /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_SEQUENCE_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "sequence generator name"        
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "DTD_IDENTIFIER",                       /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_DTD_IDENTIFIER,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "START_VALUE",                          /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_START_VALUE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "the start value of the sequence generator"
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "MINIMUM_VALUE",                        /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_MINIMUM_VALUE,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "the minimum value of the sequence generator"
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "MAXIMUM_VALUE",                        /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_MAXIMUM_VALUE,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "the maximum value of the sequence generator"
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "INCREMENT",                            /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_INCREMENT,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "the increment of the sequence generator"
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "CYCLE_OPTION",                         /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_CYCLE_OPTION,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "The values of CYCLE_OPTION have the following meanings:\n"
        "- TRUE : The cycle option of the sequence generator is CYCLE.\n"
        "- FALSE : The cycle option of the sequence generator is NO CYCLE.\n"
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "CACHE_SIZE",                           /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_CACHE_SIZE,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,            /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "number of sequence numbers to cache"
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                /**< Table ID */
        "IS_BUILTIN",                           /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_IS_BUILTIN,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "is built-in object or not"        
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                 /**< Table ID */
        "CREATED_TIME",                          /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_CREATED_TIME,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "created time of the sequence generator"        
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                 /**< Table ID */
        "MODIFIED_TIME",                         /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_MODIFIED_TIME,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "last modified time of the sequence generator"        
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                 /**< Table ID */
        "COMMENTS",                              /**< 컬럼의 이름  */
        ELDT_Sequences_ColumnOrder_COMMENTS,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "comments of the sequence generator"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.SEQUENCES 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescSEQUENCES =
{
    ELDT_TABLE_ID_SEQUENCES,                 /**< Table ID */
    "SEQUENCES",                             /**< 테이블의 이름  */
    "The SEQUENCES base table has one row for each external sequence generator."
};


/**
 * @brief DEFINITION_SCHEMA.SEQUENCES 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescSEQUENCES[ELDT_Sequences_Const_MAX] =
{
    {
        ELDT_TABLE_ID_SEQUENCES,                     /**< Table ID */
        ELDT_Sequences_Const_PRIMARY_KEY,            /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Sequences_ColumnOrder_SCHEMA_ID,
            ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                     /**< Table ID */
        ELDT_Sequences_Const_UNIQUE_SCHEMA_ID_SEQUENCE_NAME, /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        2,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Sequences_ColumnOrder_SCHEMA_ID,
            ELDT_Sequences_ColumnOrder_SEQUENCE_NAME,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                     /**< Table ID */
        ELDT_Sequences_Const_FOREIGN_KEY_SCHEMATA,   /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Sequences_ColumnOrder_SCHEMA_ID,
        },
        ELDT_TABLE_ID_SCHEMATA,                      /**< 참조 제약이 참조하는 Table ID */
        ELDT_Schemata_Const_PRIMARY_KEY,             /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/**
 * @brief DEFINITION_SCHEMA.SEQUENCES 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescSEQUENCES[ELDT_Sequences_Index_MAX] =
{
    {
        ELDT_TABLE_ID_SEQUENCES,                     /**< Table ID */
        ELDT_Sequences_Index_OWNER_ID,               /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Sequences_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                     /**< Table ID */
        ELDT_Sequences_Index_SEQUENCE_ID,            /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
        }
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                     /**< Table ID */
        ELDT_Sequences_Index_TABLESPACE_ID,          /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Sequences_ColumnOrder_TABLESPACE_ID,
        }
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                     /**< Table ID */
        ELDT_Sequences_Index_PHYSICAL_ID,            /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Sequences_ColumnOrder_PHYSICAL_ID,
        }
    },
    {
        ELDT_TABLE_ID_SEQUENCES,                     /**< Table ID */
        ELDT_Sequences_Index_SEQUENCE_NAME,          /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Sequences_ColumnOrder_SEQUENCE_NAME,
        }
    }
};

/** @} eldtSEQUENCES */

