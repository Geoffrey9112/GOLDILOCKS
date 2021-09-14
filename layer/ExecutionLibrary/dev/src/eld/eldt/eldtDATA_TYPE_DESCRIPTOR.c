/*******************************************************************************
 * eldtDATA_TYPE_DESCRIPTOR.c
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
 * @file eldtDATA_TYPE_DESCRIPTOR.c
 * @brief DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR 정의 명세
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtDATA_TYPE_DESCRIPTOR
 * @{
 */

/**
 * @brief DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescDATA_TYPE_DESCRIPTOR[ELDT_DTDesc_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,     /**< Table ID */
        "OWNER_ID",                             /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_OWNER_ID,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "authorization identifier who owns the table of the column"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,     /**< Table ID */
        "SCHEMA_ID",                            /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_SCHEMA_ID,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "schema identifier of the column"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,     /**< Table ID */
        "TABLE_ID",                             /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_TABLE_ID,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "table identifier of the column"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "COLUMN_ID",                                   /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_COLUMN_ID,             /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "column identifier"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "OBJECT_CATALOG",                              /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_OBJECT_CATALOG,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "OBJECT_SCHEMA",                               /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_OBJECT_SCHEMA,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "OBJECT_NAME",                                 /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_OBJECT_NAME,           /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "OBJECT_TYPE",                                 /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_OBJECT_TYPE,           /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "DTD_IDENTIFIER",                              /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_DTD_IDENTIFIER,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,                     /**< 컬럼의 Nullable 여부 */
        "data type descriptor identifier, equal to COLUMN_ID"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "DATA_TYPE",                                   /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_DATA_TYPE,             /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "the standard name of the data type"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "DATA_TYPE_ID",                                /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_DATA_TYPE_ID,          /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,                 /**< 컬럼의 Nullable 여부 */
        "data type identifier"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "CHARACTER_SET_CATALOG",                       /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_CHARACTER_SET_CATALOG, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "CHARACTER_SET_SCHEMA",                        /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_CHARACTER_SET_SCHEMA,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "CHARACTER_SET_NAME",                          /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_CHARACTER_SET_NAME,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "STRING_LENGTH_UNIT",                          /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_STRING_LENGTH_UNIT,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "string length unit (CHARACTERS or OCTETS)"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "STRING_LENGTH_UNIT_ID",                       /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_STRING_LENGTH_UNIT_ID, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "string length unit ID"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "CHARACTER_MAXIMUM_LENGTH",                    /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_CHARACTER_MAXIMUM_LENGTH,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "The maximum length in characters.\n"
        "If the data type being described is a character string type, "
        "the maximum length in characters of the data type being described.\n"
        "If the data type being described is a binary string type, "
        "the maximum length in octets of the data type being described.\n"
        "Otherwise, the values are the null value.\n"
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "CHARACTER_OCTET_LENGTH",                      /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_CHARACTER_MAXIMUM_LENGTH,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "The maximum length in octets.\n"
        "If the data type being described is a character string type, "
        "the maximum length in octets of the data type being described.\n"
        "If the data type being described is a binary string type, "
        "the maximum length in octets of the data type being described.\n"
        "Otherwise, the values are the null value.\n"
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "COLLATION_CATALOG",                           /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_COLLATION_CATALOG,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "COLLATION_SCHEMA",                            /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_COLLATION_SCHEMA,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "COLLATION_NAME",                              /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_COLLATION_NAME,        /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "NUMERIC_PRECISION",                           /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_NUMERIC_PRECISION,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "the numeric precision of the numerical data type"
    },
    { 
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "NUMERIC_PRECISION_RADIX",                     /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_NUMERIC_PRECISION_RADIX,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "the radix ( 2 or 10 ) of the precision of the numerical data type"
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "NUMERIC_SCALE",                               /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_NUMERIC_SCALE,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "the numeric scale of the exact numerical data type"
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "DECLARED_DATA_TYPE",                          /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_DECLARED_DATA_TYPE,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "the data type name that a user declared"
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "DECLARED_NUMERIC_PRECISION",                  /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_DECLARED_NUMERIC_PRECISION,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "The precision value that a user declared.\n"
        "For a character string type, it means the character length. \n"
        "For a binary string type, it means the octet length. \n"
        "For a numerical type, it means the numeric precision. \n"
        "For a datetime or interval type, it means the fractional seconds precision. \n"
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "DECLARED_NUMERIC_SCALE",                      /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_DECLARED_NUMERIC_SCALE,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "The scale value that a user declared.\n"
        "For a numerical type, it means the numeric scale. \n"
        "For a interval type, it means the leading precision. \n"
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "DATETIME_PRECISION",                          /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_DATETIME_PRECISION,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "For a datetime or interval type, the value is the fractional seconds precision"
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "INTERVAL_TYPE",                               /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_INTERVAL_TYPE,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SHORT_DESC,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "For a interval type, the value is in "
        "( 'YEAR', 'MONTH', 'DAY', 'HOUR', 'MINUTE', 'SECOND',"
        " 'YEAR TO MONTH', 'DAY TO HOUR', 'DAY TO MINUTE', 'DAY TO SECOND',"
        " 'HOUR TO MINUTE', 'HOUR TO SECOND', 'MINUTE TO SECOND' ) "
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "INTERVAL_TYPE_ID",                            /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_INTERVAL_TYPE,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "INTERVAL_TYPE identifier"
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "INTERVAL_PRECISION",                          /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_INTERVAL_PRECISION,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "For a interval type, the value is the leading precision"
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "USER_DEFINED_TYPE_CATALOG",                   /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_USER_DEFINED_TYPE_CATALOG,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "USER_DEFINED_TYPE_SCHEMA",                    /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_USER_DEFINED_TYPE_SCHEMA,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "USER_DEFINED_TYPE_NAME",                      /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_USER_DEFINED_TYPE_NAME,     /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "SCOPE_CATALOG",                               /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_SCOPE_CATALOG,         /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "SCOPE_SCHEMA",                                /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_SCOPE_SCHEMA,          /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "SCOPE_NAME",                                  /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_SCOPE_NAME,            /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,            /**< Table ID */
        "MAXIMUM_CARDINALITY",                         /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_MAXIMUM_CARDINALITY,   /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                             /**< 컬럼의 Nullable 여부 */
        "unsupported feature"        
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,     /**< Table ID */
        "PHYSICAL_MAXIMUM_LENGTH",              /**< 컬럼의 이름  */
        ELDT_DTDesc_ColumnOrder_PHYSICAL_MAXIMUM_LENGTH,       /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_BIG_NUMBER,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "maximum physical byte length of the data type defintion"        
    }
};



/**
 * @brief DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescDATA_TYPE_DESCRIPTOR =
{
    ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,         /**< Table ID */
    "DATA_TYPE_DESCRIPTOR",                     /**< 테이블의 이름  */
    "The DATA_TYPE_DESCRIPTOR table has one row for each usage of a column's datatype."
};



/**
 * @brief DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescDATA_TYPE_DESCRIPTOR[ELDT_DTDesc_Const_MAX] =
{
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,          /**< Table ID */
        ELDT_DTDesc_Const_UNIQUE_DTD_IDENTIFIER,     /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_DTDesc_ColumnOrder_DTD_IDENTIFIER,
        },
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,            /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};


/**
 * @brief DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescDATA_TYPE_DESCRIPTOR[ELDT_DTDesc_Index_MAX] =
{
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,          /**< Table ID */
        ELDT_DTDesc_Index_OWNER_ID,                  /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_DTDesc_ColumnOrder_OWNER_ID,
        }
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,          /**< Table ID */
        ELDT_DTDesc_Index_SCHEMA_ID,                 /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_DTDesc_ColumnOrder_SCHEMA_ID,
        }
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,          /**< Table ID */
        ELDT_DTDesc_Index_TABLE_ID,                  /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_DTDesc_ColumnOrder_TABLE_ID,
        }
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,          /**< Table ID */
        ELDT_DTDesc_Index_COLUMN_ID,                 /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_DTDesc_ColumnOrder_COLUMN_ID,
        }
    },
    {
        ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,          /**< Table ID */
        ELDT_DTDesc_Index_DATA_TYPE_ID,              /**< Table 내 Index 번호 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                           /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_DTDesc_ColumnOrder_DATA_TYPE_ID,
        }
    }
};


/** @} eldtDATA_TYPE_DESCRIPTOR */

