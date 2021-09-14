/*******************************************************************************
 * dtlDataType.c
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
 * @file dtlDataType.c
 * @brief DataType Layer 데이타 타입관련 redirect 함수 정의
 */

#include <dtl.h>
#include <dtDef.h>

/**
 * @addtogroup dtlDataTypeDefinition
 * @{
 */

static const dtlDataType dtlStaticTypes[ DTL_STATIC_TYPE_COUNT ] =
{
    DTL_TYPE_BOOLEAN,
    DTL_TYPE_NATIVE_SMALLINT,
    DTL_TYPE_NATIVE_INTEGER,
    DTL_TYPE_NATIVE_BIGINT,
    
    DTL_TYPE_NATIVE_REAL,
    DTL_TYPE_NATIVE_DOUBLE,

    DTL_TYPE_ROWID
};

/**
 * @brief Data Type Full Name
 */
const stlChar dtlDataTypeName[DTL_TYPE_MAX][DTL_DATA_TYPE_NAME_MAX_SIZE] =
{
    "BOOLEAN",
    "NATIVE_SMALLINT",
    "NATIVE_INTEGER",
    "NATIVE_BIGINT",

    "NATIVE_REAL",
    "NATIVE_DOUBLE",

    "FLOAT",    
    "NUMBER",
    "DECIMAL",
    
    "CHARACTER",
    "CHARACTER VARYING",
    "CHARACTER LONG VARYING",
    "CHARACTER LARGE OBJECT",

    "BINARY",
    "BINARY VARYING",
    "BINARY LONG VARYING",
    "BINARY LARGE OBJECT",

    "DATE",
    "TIME WITHOUT TIME ZONE",
    "TIMESTAMP WITHOUT TIME ZONE",
    "TIME WITH TIME ZONE",
    "TIMESTAMP WITH TIME ZONE",

    "INTERVAL",   // DTL_TYPE_INTERVAL_YEAR_TO_MONTH
    "INTERVAL",   // DTL_TYPE_INTERVAL_DAY_TO_SECOND

    "ROWID"       // ROWID
};


/**
 * @brief Data Type Name for Dump Function
 */
const stlChar dtlDataTypeNameForDumpFunc[ DTL_TYPE_MAX ][ DTL_DATA_TYPE_NAME_FOR_DUMP_FUNC_MAX_SIZE ] =
{
    "BOOLEAN",
    "NATIVE_SMALLINT",
    "NATIVE_INTEGER",
    "NATIVE_BIGINT",

    "NATIVE_REAL",
    "NATIVE_DOUBLE",

    "FLOAT",
    "NUMBER",
    "DECIMAL",
    
    "CHAR",
    "VARCHAR",
    "LONG VARCHAR",
    "CLOB",

    "BINARY",
    "VARBINARY",
    "LONG VARBINARY",
    "BLOB",

    "DATE",
    "TIME",
    "TIMESTAMP",
    "TIME_TZ",
    "TIMESTAMP_TZ",

    "INTERVAL_YM",   // DTL_TYPE_INTERVAL_YEAR_TO_MONTH
    "INTERVAL_DS",   // DTL_TYPE_INTERVAL_DAY_TO_SECOND

    "ROWID"       // ROWID
};

/** @} dtlDataTypeDefinition */


/**
 * @addtogroup dtlDataTypeInformation
 * @{
 */

stlChar * gDtlLengthUnitString[DTL_STRING_LENGTH_UNIT_MAX] =
{
    "N/A",         /* DTL_STRING_LENGTH_UNIT_NOT_AVAILABLE */
    "CHARACTERS",  /* DTL_STRING_LENGTH_UNIT_CHARACTERS */
    "OCTETS"       /* DTL_STRING_LENGTH_UNIT_OCTETS */
};


/**
 * @brief CHARACTER SET 이름과 ID MAP
 */
const dtlLengthUnitMap gDtlLengthUnitMap[] =
{
    { "OCTETS",         DTL_STRING_LENGTH_UNIT_OCTETS },
    { "BYTE",           DTL_STRING_LENGTH_UNIT_OCTETS },
    { "CHARACTERS",     DTL_STRING_LENGTH_UNIT_CHARACTERS },
    { "CHAR",           DTL_STRING_LENGTH_UNIT_CHARACTERS },
    { NULL,             DTL_STRING_LENGTH_UNIT_NA   },
};

stlChar * gDtlIntervalIndicatorString[DTL_INTERVAL_INDICATOR_MAX] =
{
    "N/A",
    "YEAR",
    "MONTH",
    "DAY",
    "HOUR",
    "MINUTE",
    "SECOND",
    "YEAR TO MONTH",
    "DAY TO HOUR",
    "DAY TO MINUTE",
    "DAY TO SECOND",
    "HOUR TO MINUTE",
    "HOUR TO SECOND",
    "MINUTE TO SECOND"
};


stlChar * gDtlIntervalIndicatorStringForDump[DTL_INTERVAL_INDICATOR_MAX] =
{
    "N/A",
    "YY",
    "MM",
    "DD",
    "HH",
    "MI",
    "SS",
    "YM",
    "DH",
    "DM",
    "DS",
    "HM",
    "HS",
    "MS"
};

    
dtlDataTypeDefinition gIntervalYearToMonthDataTypeDefinition[DTL_INTERVAL_YEAR_TO_MONTH_TYPE_COUNT] =
{
    /* INTERVAL YEAR 타입 */
    {
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,          /**< 내부 Data Type ID */
        101,                                      /**< ODBC Data Type ID */
        101,                                      /**< JDBC Data Type ID */
        
        "INTERVAL YEAR",                          /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,                         /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    },

    /* INTERVAL MONTH 타입 */
    {
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,          /**< 내부 Data Type ID */
        102,                                      /**< ODBC Data Type ID */
        102,                                      /**< JDBC Data Type ID */
        
        "INTERVAL MONTH",                          /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,                         /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    },

    /* INTERVAL YEAR TO MONTH 타입 */
    {
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,          /**< 내부 Data Type ID */
        107,                                      /**< ODBC Data Type ID */
        107,                                      /**< JDBC Data Type ID */
        
        "INTERVAL YEAR TO MONTH",                 /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,                         /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    }
};

    
dtlDataTypeDefinition gIntervalDayToSecondDataTypeDefinition[DTL_INTERVAL_DAY_TO_SECOND_TYPE_COUNT] =
{
    /* INTERVAL DAY 타입 */
    {
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,          /**< 내부 Data Type ID */
        103,                                      /**< ODBC Data Type ID */
        103,                                      /**< JDBC Data Type ID */
        
        "INTERVAL DAY",                           /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,                         /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    },

    /* INTERVAL HOUR 타입 */
    {
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,          /**< 내부 Data Type ID */
        104,                                      /**< ODBC Data Type ID */
        104,                                      /**< JDBC Data Type ID */
        
        "INTERVAL HOUR",                          /**< SQL 표준 Data Type Name */

        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,                         /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    },

    /* INTERVAL MINUTE 타입 */
    {
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,          /**< 내부 Data Type ID */
        105,                                      /**< ODBC Data Type ID */
        105,                                      /**< JDBC Data Type ID */
        
        "INTERVAL MINUTE",                        /**< SQL 표준 Data Type Name */

        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,                         /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    },

    /* INTERVAL SECOND 타입 */
    {
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,          /**< 내부 Data Type ID */
        106,                                      /**< ODBC Data Type ID */
        106,                                      /**< JDBC Data Type ID */
        
        "INTERVAL SECOND",                        /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,/**< Fractional Second Max Precision */
        DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION,/**< Fractional Second Min Precision */
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,/**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    },

    /* INTERVAL DAY TO HOUR 타입 */
    {
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,          /**< 내부 Data Type ID */
        108,                                      /**< ODBC Data Type ID */
        108,                                      /**< JDBC Data Type ID */
        
        "INTERVAL DAY TO HOUR",                   /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,                         /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    },

    /* INTERVAL DAY TO MINUTE 타입 */
    {
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,          /**< 내부 Data Type ID */
        109,                                      /**< ODBC Data Type ID */
        109,                                      /**< JDBC Data Type ID */
        
        "INTERVAL DAY TO MINUTE",                 /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,                         /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    },

    /* INTERVAL DAY TO SECOND 타입 */
    {
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,          /**< 내부 Data Type ID */
        110,                                      /**< ODBC Data Type ID */
        110,                                      /**< JDBC Data Type ID */
        
        "INTERVAL DAY TO SECOND",                 /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,/**< Fractional Second Max Precision */
        DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION,/**< Fractional Second Min Precision */
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,/**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    },

    /* INTERVAL HOUR TO MINUTE 타입 */
    {
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,          /**< 내부 Data Type ID */
        111,                                      /**< ODBC Data Type ID */
        111,                                      /**< JDBC Data Type ID */
        
        "INTERVAL HOUR TO MINUTE",                /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,                         /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,                         /**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    },

    /* INTERVAL HOUR TO SECOND 타입 */
    {
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,          /**< 내부 Data Type ID */
        112,                                      /**< ODBC Data Type ID */
        112,                                      /**< JDBC Data Type ID */
        
        "INTERVAL HOUR TO SECOND",                /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,/**< Fractional Second Max Precision */
        DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION,/**< Fractional Second Min Precision */
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,/**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    },

    /* INTERVAL MINUTE TO SECOND 타입 */
    {
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,          /**< 내부 Data Type ID */
        113,                                      /**< ODBC Data Type ID */
        113,                                      /**< JDBC Data Type ID */
        
        "INTERVAL MINUTE TO SECOND",              /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                            /**< String Maximum Length */
        DTL_LENGTH_NA,                            /**< String Minimum Length */
        DTL_LENGTH_NA,                            /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE,           /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,                /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,                         /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,                         /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                             /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                             /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                             /**< Numeric Default Scale */
        
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,/**< Fractional Second Max Precision */
        DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION,/**< Fractional Second Min Precision */
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,/**< Fractional Second Default */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION, /**< Interval Max Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Min Prec */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION, /**< Interval Def Prec */
        
        STL_TRUE                                  /**< 지원 여부 */
    }
};

    
dtlDataTypeDefinition gDataTypeDefinition[DTL_TYPE_MAX] =
{
    /* BOOLEAN 타입 */
    {
        DTL_TYPE_BOOLEAN,              /**< 내부 Data Type ID */
        16,                            /**< ODBC Data Type ID */
        16,                            /**< JDBC Data Type ID */
        
        "BOOLEAN",                     /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* NATIVE_SMALLINT 타입 */
    {
        DTL_TYPE_NATIVE_SMALLINT,      /**< 내부 Data Type ID */
        5,                             /**< ODBC Data Type ID */
        5,                             /**< JDBC Data Type ID */
        
        "NATIVE_SMALLINT",             /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_BINARY, /**< Numeric의 기본 Radix */
        
        DTL_NATIVE_SMALLINT_DEFAULT_PRECISION, /**< Numeric Maximum Precision */
        DTL_NATIVE_SMALLINT_DEFAULT_PRECISION, /**< Numeric Minimum Precision */
        DTL_NATIVE_SMALLINT_DEFAULT_PRECISION, /**< Numeric Default Precision */
        
        0,                             /**< Numeric Maximum Scale */
        0,                             /**< Numeric Minimum Scale */
        0,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* NATIVE_INTEGER 타입 */
    {
        DTL_TYPE_NATIVE_INTEGER,       /**< 내부 Data Type ID */
        4,                             /**< ODBC Data Type ID */
        4,                             /**< JDBC Data Type ID */
        
        "NATIVE_INTEGER",              /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_BINARY, /**< Numeric의 기본 Radix */
        
        DTL_NATIVE_INTEGER_DEFAULT_PRECISION, /**< Numeric Maximum Precision */
        DTL_NATIVE_INTEGER_DEFAULT_PRECISION, /**< Numeric Minimum Precision */
        DTL_NATIVE_INTEGER_DEFAULT_PRECISION, /**< Numeric Default Precision */
        
        0,                             /**< Numeric Maximum Scale */
        0,                             /**< Numeric Minimum Scale */
        0,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* NATIVE_BIGINT 타입 */
    {
        DTL_TYPE_NATIVE_BIGINT,        /**< 내부 Data Type ID */
        -5,                            /**< ODBC Data Type ID */
        -5,                            /**< JDBC Data Type ID */
        
        "NATIVE_BIGINT",               /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_BINARY, /**< Numeric의 기본 Radix */
        
        DTL_NATIVE_BIGINT_DEFAULT_PRECISION,  /**< Numeric Maximum Precision */
        DTL_NATIVE_BIGINT_DEFAULT_PRECISION,  /**< Numeric Minimum Precision */
        DTL_NATIVE_BIGINT_DEFAULT_PRECISION,  /**< Numeric Default Precision */
        
        0,                             /**< Numeric Maximum Scale */
        0,                             /**< Numeric Minimum Scale */
        0,                             /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* NATIVE_REAL 타입 */
    {
        DTL_TYPE_NATIVE_REAL,          /**< 내부 Data Type ID */
        7,                             /**< ODBC Data Type ID */
        7,                             /**< JDBC Data Type ID */
        
        "NATIVE_REAL",                 /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_BINARY, /**< Numeric의 기본 Radix */
        
        DTL_NATIVE_REAL_DEFAULT_PRECISION,    /**< Numeric Maximum Precision */
        DTL_NATIVE_REAL_DEFAULT_PRECISION,    /**< Numeric Minimum Precision */
        DTL_NATIVE_REAL_DEFAULT_PRECISION,    /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* NATIVE_DOUBLE 타입 */
    {
        DTL_TYPE_NATIVE_DOUBLE,        /**< 내부 Data Type ID */
        8,                             /**< ODBC Data Type ID */
        8,                             /**< JDBC Data Type ID */
        
        "NATIVE_DOUBLE",               /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_BINARY, /**< Numeric의 기본 Radix */
        
        DTL_NATIVE_DOUBLE_DEFAULT_PRECISION,  /**< Numeric Maximum Precision */
        DTL_NATIVE_DOUBLE_DEFAULT_PRECISION,  /**< Numeric Minimum Precision */
        DTL_NATIVE_DOUBLE_DEFAULT_PRECISION,  /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* FLOAT 타입 */
    {
        DTL_TYPE_FLOAT,                /**< 내부 Data Type ID */
        6,                             /**< ODBC Data Type ID */
        6,                             /**< JDBC Data Type ID */
        
        "FLOAT",                       /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_BINARY, /**< Numeric의 기본 Radix */
        
        DTL_FLOAT_MAX_PRECISION,       /**< Numeric Maximum Precision */
        DTL_FLOAT_MIN_PRECISION,       /**< Numeric Minimum Precision */
        DTL_FLOAT_DEFAULT_PRECISION,   /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* NUMBER 타입 */
    {
        DTL_TYPE_NUMBER,               /**< 내부 Data Type ID */
        2,                             /**< ODBC Data Type ID */
        2,                             /**< JDBC Data Type ID */
        
        "NUMBER",                      /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_DECIMAL, /**< Numeric의 기본 Radix */
        
        DTL_NUMERIC_MAX_PRECISION,     /**< Numeric Maximum Precision */
        DTL_NUMERIC_MIN_PRECISION,     /**< Numeric Minimum Precision */
        DTL_NUMERIC_DEFAULT_PRECISION, /**< Numeric Default Precision */
        
        DTL_NUMERIC_MAX_SCALE,         /**< Numeric Maximum Scale */
        DTL_NUMERIC_MIN_SCALE,         /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* unsupported feature, DECIMAL 타입 */
    {
        DTL_TYPE_DECIMAL,              /**< 내부 Data Type ID */
        3,                             /**< ODBC Data Type ID */
        3,                             /**< JDBC Data Type ID */
        
        "DECIMAL",                     /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_FALSE                      /**< 지원 여부 */
    },
    /* CHAR 타입 */
    {
        DTL_TYPE_CHAR,                 /**< 내부 Data Type ID */
        1,                             /**< ODBC Data Type ID */
        1,                             /**< JDBC Data Type ID */
        
        "CHARACTER",                   /**< SQL 표준 Data Type Name */
        
        DTL_CHAR_MAX_PRECISION,        /**< String Maximum Length */
        1,                             /**< String Minimum Length */
        1,                             /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* VARCHAR 타입 */
    {
        DTL_TYPE_VARCHAR,              /**< 내부 Data Type ID */
        12,                            /**< ODBC Data Type ID */
        12,                            /**< JDBC Data Type ID */
        
        "CHARACTER VARYING",           /**< SQL 표준 Data Type Name */
        
        DTL_VARCHAR_MAX_PRECISION,     /**< String Maximum Length */
        1,                             /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* LONG VARCHAR 타입 */
    {
        DTL_TYPE_LONGVARCHAR,          /**< 내부 Data Type ID */
        -1,                            /**< ODBC Data Type ID */
        -1,                            /**< JDBC Data Type ID */
        
        "CHARACTER LONG VARYING",      /**< SQL 표준 Data Type Name */
        
        DTL_LONGVARCHAR_MAX_PRECISION,     /**< String Maximum Length */
        1,                                 /**< String Minimum Length */
        DTL_LONGVARCHAR_MAX_PRECISION,     /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE,    /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_FALSE                      /**< 지원 여부 */
    },
    /* CLOB 타입 */
    {
        DTL_TYPE_CLOB,                 /**< 내부 Data Type ID */
        40,                            /**< ODBC Data Type ID */
        2005,                          /**< JDBC Data Type ID */
        
        "CHARACTER LARGE OBJECT",      /**< SQL 표준 Data Type Name */

        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */

        DTL_SEARCHABLE_PRED_NONE,      /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_FALSE                      /**< 지원 여부 */
    },
    /* BINARY 타입 */
    {
        DTL_TYPE_BINARY,               /**< 내부 Data Type ID */
        -2,                            /**< ODBC Data Type ID */
        -2,                            /**< JDBC Data Type ID */
        
        "BINARY",                      /**< SQL 표준 Data Type Name */
        
        DTL_BINARY_MAX_PRECISION,      /**< String Maximum Length */
        1,                             /**< String Minimum Length */
        1,                             /**< String Default Length */

        DTL_SEARCHABLE_PRED_BASIC,     /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* VARBINARY 타입 */
    {
        DTL_TYPE_VARBINARY,            /**< 내부 Data Type ID */
        -3,                            /**< ODBC Data Type ID */
        -3,                            /**< JDBC Data Type ID */
        
        "BINARY VARYING",              /**< SQL 표준 Data Type Name */
        
        DTL_VARBINARY_MAX_PRECISION,   /**< String Maximum Length */
        1,                             /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_BASIC    , /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* BINARY LONG VARYING 타입 */
    {
        DTL_TYPE_LONGVARBINARY,        /**< 내부 Data Type ID */
        -4,                            /**< ODBC Data Type ID */
        -4,                            /**< JDBC Data Type ID */
        
        "BINARY LONG VARYING",         /**< SQL 표준 Data Type Name */
        
        DTL_LONGVARBINARY_MAX_PRECISION,      /**< String Maximum Length */
        1,                             /**< String Minimum Length */
        DTL_LONGVARBINARY_MAX_PRECISION,      /**< String Default Length */

        DTL_SEARCHABLE_PRED_BASIC,     /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_FALSE                      /**< 지원 여부 */
    },
    /* BLOB 타입 */
    {
        DTL_TYPE_BLOB,                 /**< 내부 Data Type ID */
        30,                            /**< ODBC Data Type ID */
        2004,                          /**< JDBC Data Type ID */
        
        "BINARY LARGE OBJECT",         /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */

        DTL_SEARCHABLE_PRED_NONE,      /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_FALSE                      /**< 지원 여부 */
    },
    /* DATE 타입 */
    {
        DTL_TYPE_DATE,                 /**< 내부 Data Type ID */
        91,                            /**< ODBC Data Type ID */
        91,                            /**< JDBC Data Type ID */
        
        "DATE",                        /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */

        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* TIME 타입 */
    {
        DTL_TYPE_TIME,                 /**< 내부 Data Type ID */
        92,                            /**< ODBC Data Type ID */
        92,                            /**< JDBC Data Type ID */
        
        "TIME WITHOUT TIME ZONE",      /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_TIME_MAX_PRECISION,        /**< Fractional Second Max Precision (6) */
        DTL_TIME_MIN_PRECISION,        /**< Fractional Second Min Precision (0) */
        DTL_TIME_MAX_PRECISION,        /**< Fractional Second Default (6) */

        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* TIMESTAMP 타입 */
    {
        DTL_TYPE_TIMESTAMP,            /**< 내부 Data Type ID */
        93,                            /**< ODBC Data Type ID */
        93,                            /**< JDBC Data Type ID */
        
        "TIMESTAMP WITHOUT TIME ZONE",  /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_TIMESTAMP_MAX_PRECISION,   /**< Fractional Second Max Precision (6) */
        DTL_TIMESTAMP_MIN_PRECISION,   /**< Fractional Second Min Precision (0) */
        DTL_TIMESTAMP_MAX_PRECISION,   /**< Fractional Second Default (6) */

        DTL_PRECISION_NA,             /**< Interval Max Prec */
        DTL_PRECISION_NA,             /**< Interval Min Prec */
        DTL_PRECISION_NA,             /**< Interval Def Prec */
        
        STL_TRUE                      /**< 지원 여부 */
    },
    /* TIME WITH TIME ZONE*/
    {
        DTL_TYPE_TIME_WITH_TIME_ZONE,  /**< 내부 Data Type ID */
        94,                            /**< ODBC Data Type ID */
        92,                            /**< JDBC Data Type ID */
        
        "TIME WITH TIME ZONE",         /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_TIMETZ_MAX_PRECISION,      /**< Fractional Second Max Precision (6) */
        DTL_TIMETZ_MIN_PRECISION,      /**< Fractional Second Min Precision (0) */
        DTL_TIMETZ_MAX_PRECISION,      /**< Fractional Second Default (6) */

        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* TIMESTAMP 타입 */
    {
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, /**< 내부 Data Type ID */
        95,                            /**< ODBC Data Type ID */
        93,                            /**< JDBC Data Type ID */
        
        "TIMESTAMP WITH TIME ZONE",    /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_TIMESTAMPTZ_MAX_PRECISION, /**< Fractional Second Max Precision (6) */
        DTL_TIMESTAMPTZ_MIN_PRECISION, /**< Fractional Second Min Precision (0) */
        DTL_TIMESTAMPTZ_MAX_PRECISION, /**< Fractional Second Default (6) */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    },
    /* INTERVAL YEAR TO MONTH 타입 */
    {
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH, /**< 내부 Data Type ID */
        100,                           /**< ODBC Data Type ID */
        10,                            /**< JDBC Data Type ID */
        
        "INTERVAL YEAR TO MONTH",      /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,  /**< Fractional Second Max Precision(6) */
        DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION,  /**< Fractional Second Min Precision(0) */
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,  /**< Fractional Second Default(6) */

        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,      /**< Interval Max Prec(6) */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,      /**< Interval Min Prec(2) */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,      /**< Interval Def Prec(2) */
        
        STL_TRUE                      /**< 지원 여부 */
    },
    /* INTERVAL DAY TO SECOND 타입 */
    {
        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /**< 내부 Data Type ID */
        100,                           /**< ODBC Data Type ID */
        10,                            /**< JDBC Data Type ID */
        
        "INTERVAL DAY TO SECOND",      /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */
        
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,  /**< Fractional Second Max Precision(6) */
        DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION,  /**< Fractional Second Min Precision(0) */
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,  /**< Fractional Second Default(6) */

        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,      /**< Interval Max Prec(6) */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,      /**< Interval Min Prec(2) */
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,      /**< Interval Def Prec(2) */
        
        STL_TRUE                      /**< 지원 여부 */
    },
    /* ROWID 타입 */
    {
        DTL_TYPE_ROWID,                /**< 내부 Data Type ID */
        -101,                          /**< ODBC Data Type ID */ 
        -8,                            /**< JDBC Data Type ID */
        
        "ROWID",                       /**< SQL 표준 Data Type Name */
        
        DTL_LENGTH_NA,                 /**< String Maximum Length */
        DTL_LENGTH_NA,                 /**< String Minimum Length */
        DTL_LENGTH_NA,                 /**< String Default Length */
        
        DTL_SEARCHABLE_PRED_SEARCHABLE, /**< 검색 가능 여부 */
        
        DTL_NUMERIC_PREC_RADIX_NA,     /**< Numeric의 기본 Radix */
        
        DTL_PRECISION_NA,              /**< Numeric Maximum Precision */
        DTL_PRECISION_NA,              /**< Numeric Minimum Precision */
        DTL_PRECISION_NA,              /**< Numeric Default Precision */
        
        DTL_SCALE_NA,                  /**< Numeric Maximum Scale */
        DTL_SCALE_NA,                  /**< Numeric Minimum Scale */
        DTL_SCALE_NA,                  /**< Numeric Default Scale */

        DTL_PRECISION_NA,              /**< Fractional Second Max Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Min Precision */
        DTL_PRECISION_NA,              /**< Fractional Second Default */
        
        DTL_PRECISION_NA,              /**< Interval Max Prec */
        DTL_PRECISION_NA,              /**< Interval Min Prec */
        DTL_PRECISION_NA,              /**< Interval Def Prec */
        
        STL_TRUE                       /**< 지원 여부 */
    }       
};

const dtlDataTypeDefInfo gDefaultDataTypeDef[ DTL_TYPE_MAX ] =
{
    /* BOOLEAN */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_BOOLEAN_SIZE,               /**< value buffer size */
        DTL_BOOLEAN_STRING_SIZE,        /**< string buffer size */
    },

    /* NATIVE_SMALLINT */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NATIVE_SMALLINT_SIZE,              /**< value buffer size */
        DTL_NATIVE_SMALLINT_STRING_SIZE,       /**< string buffer size */
    },

    /* NATIVE_INTEGER */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NATIVE_INTEGER_SIZE,               /**< value buffer size */
        DTL_NATIVE_INTEGER_STRING_SIZE,        /**< string buffer size */
    },

    /* NATIVE_BIGINT */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NATIVE_BIGINT_SIZE,                /**< value buffer size */
        DTL_NATIVE_BIGINT_STRING_SIZE,         /**< string buffer size */
    },

    
    /* NATIVE_REAL */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NATIVE_REAL_SIZE,                  /**< value buffer size */
        DTL_NATIVE_REAL_STRING_SIZE,           /**< string buffer size */
    },

    /* NATIVE_DOUBLE */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NATIVE_DOUBLE_SIZE,                /**< value buffer size */
        DTL_NATIVE_DOUBLE_STRING_SIZE,         /**< string buffer size */
    },

    /* FLOAT */
    {
        DTL_NUMERIC_DEFAULT_PRECISION,  /**< precision */
        DTL_NUMERIC_DEFAULT_SCALE,      /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NUMERIC_MAX_SIZE,           /**< value buffer size */
        DTL_NUMERIC_STRING_MAX_SIZE,    /**< string buffer size */
    },
        
    /* NUMBER */
    {
        DTL_NUMERIC_DEFAULT_PRECISION,  /**< precision */
        DTL_NUMERIC_DEFAULT_SCALE,      /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NUMERIC_MAX_SIZE,           /**< value buffer size */
        DTL_NUMERIC_STRING_MAX_SIZE,    /**< string buffer size */
    },

    /* DECIMAL */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_LENGTH_NA,                  /**< value buffer size */
        DTL_LENGTH_NA,                  /**< string buffer size */
    },


    /* CHAR */
    {
        DTL_CHAR_MAX_PRECISION,           /**< precision */
        DTL_SCALE_NA,                     /**< scale */
        DTL_STRING_LENGTH_UNIT_CHARACTERS,    /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,        /**< interval indicator */
        
        DTL_CHAR_MAX_BUFFER_SIZE,  /**< value buffer size */
        DTL_CHAR_MAX_CHAR_STRING_SIZE,  /**< string buffer size */
    },

    /* VARCHAR */
    {
        DTL_VARCHAR_MAX_PRECISION,          /**< precision */
        DTL_SCALE_NA,                       /**< scale */
        DTL_STRING_LENGTH_UNIT_CHARACTERS,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,          /**< interval indicator */
        
        DTL_VARCHAR_MAX_BUFFER_SIZE, /**< value buffer size */
        DTL_VARCHAR_MAX_CHAR_STRING_SIZE, /**< string buffer size */
    },

    /* LONGVARCHAR */
    {
        DTL_LONGVARCHAR_MAX_PRECISION,          /**< precision */
        DTL_SCALE_NA,                           /**< scale */
        DTL_STRING_LENGTH_UNIT_OCTETS,          /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,              /**< interval indicator */
        
        DTL_LONGVARYING_INIT_STRING_SIZE,       /**< value buffer size */
        DTL_LONGVARYING_INIT_STRING_SIZE,       /**< string buffer size */
    },

    /* CLOB */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_LENGTH_NA,                  /**< value buffer size */
        DTL_LENGTH_NA,                  /**< string buffer size */
    },


    /* BINARY */
    {
        DTL_BINARY_MAX_PRECISION,       /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_BINARY_STRING_SIZE,         /**< value buffer size */
        DTL_BINARY_STRING_SIZE,         /**< string buffer size */
    },

    /* VARBINARY */
    {
        DTL_VARBINARY_MAX_PRECISION,       /**< precision */
        DTL_SCALE_NA,                      /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,         /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,         /**< interval indicator */
        
        DTL_VARBINARY_STRING_SIZE,         /**< value buffer size */
        DTL_VARBINARY_STRING_SIZE,         /**< string buffer size */
    },

    /* LONGVARBINARY */
    {
        DTL_LONGVARBINARY_MAX_PRECISION,   /**< precision */
        DTL_SCALE_NA,                      /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,         /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,         /**< interval indicator */
        
        DTL_LONGVARYING_INIT_STRING_SIZE,  /**< value buffer size */
        DTL_LONGVARYING_INIT_STRING_SIZE,  /**< string buffer size */
    },

    /* BLOB */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_LENGTH_NA,                  /**< value buffer size */
        DTL_LENGTH_NA,                  /**< string buffer size */
    },


    /* DATE */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_DATE_SIZE,                  /**< value buffer size */
        DTL_DATE_STRING_SIZE + 1,       /**< string buffer size */
    },

    /* TIME WITHOUT TIME ZONE */
    {
        DTL_TIME_MAX_PRECISION,         /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_TIME_SIZE,                  /**< value buffer size */
        DTL_TIME_STRING_SIZE + 1,       /**< string buffer size */
    },

    /* TIMESTAMP WITHOUT TIME ZONE */
    {
        DTL_TIMESTAMP_MAX_PRECISION,    /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_TIMESTAMP_SIZE,             /**< value buffer size */
        DTL_TIMESTAMP_STRING_SIZE + 1,  /**< string buffer size */
    },

    /* TIME WITH TIME ZONE */
    {
        DTL_TIMETZ_MAX_PRECISION,       /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_TIMETZ_SIZE,                /**< value buffer size */
        DTL_TIMETZ_STRING_SIZE + 1,     /**< string buffer size */
    },

    /* TIMESTAMP WITH TIME ZONE */
    {
        DTL_TIMESTAMPTZ_MAX_PRECISION,  /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_TIMESTAMPTZ_SIZE,           /**< value buffer size */
        DTL_TIMESTAMPTZ_STRING_SIZE + 1, /**< string buffer size */
    },

    /* INTERVAL YEAR TO MONTH */
    {
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,         /**< precision */
        DTL_SCALE_NA,                                     /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,                        /**< string length unit */
        DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,             /**< interval indicator */
        
        DTL_INTERVAL_YEAR_TO_MONTH_SIZE,                  /**< value buffer size */
        DTL_INTERVAL_YEAR_TO_MONTH_MAX_STRING_SIZE + 1,   /**< string buffer size */
    },

    /* INTERVAL DAY TO SECOND */
    {
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,         /**< precision */
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,     /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,                        /**< string length unit */
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,             /**< interval indicator */
        
        DTL_INTERVAL_DAY_TO_SECOND_SIZE,                  /**< value buffer size */
        DTL_INTERVAL_DAY_TO_SECOND_MAX_STRING_SIZE + 1,   /**< string buffer size */
    },

    /* ROWID */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_ROWID_SIZE,                 /**< value buffer size */
        DTL_ROWID_STRING_SIZE,          /**< string buffer size */
    }       
};


const dtlDataTypeDefInfo gResultDataTypeDef[ DTL_TYPE_MAX ] =
{
    /* BOOLEAN */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_BOOLEAN_SIZE,               /**< value buffer size */
        DTL_BOOLEAN_STRING_SIZE,        /**< string buffer size */
    },

    /* NATIVE_SMALLINT */
    {
        DTL_NATIVE_SMALLINT_DEFAULT_PRECISION, /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NATIVE_SMALLINT_SIZE,              /**< value buffer size */
        DTL_NATIVE_SMALLINT_STRING_SIZE,       /**< string buffer size */
    },

    /* NATIVE_INTEGER */
    {
        DTL_NATIVE_INTEGER_DEFAULT_PRECISION,  /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NATIVE_INTEGER_SIZE,               /**< value buffer size */
        DTL_NATIVE_INTEGER_STRING_SIZE,        /**< string buffer size */
    },

    /* NATIVE_BIGINT */
    {
        DTL_NATIVE_BIGINT_DEFAULT_PRECISION,   /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NATIVE_BIGINT_SIZE,                /**< value buffer size */
        DTL_NATIVE_BIGINT_STRING_SIZE,         /**< string buffer size */
    },

    
    /* NATIVE_REAL */
    {
        DTL_NATIVE_REAL_DEFAULT_PRECISION,     /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NATIVE_REAL_SIZE,                  /**< value buffer size */
        DTL_NATIVE_REAL_STRING_SIZE,           /**< string buffer size */
    },

    /* NATIVE_DOUBLE */
    {
        DTL_NATIVE_DOUBLE_DEFAULT_PRECISION,   /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NATIVE_DOUBLE_SIZE,                /**< value buffer size */
        DTL_NATIVE_DOUBLE_STRING_SIZE,         /**< string buffer size */
    },

    /* FLOAT */
    {
        DTL_FLOAT_MAX_PRECISION,        /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NUMERIC_MAX_SIZE,           /**< value buffer size */
        DTL_NUMERIC_STRING_MAX_SIZE,    /**< string buffer size */
    },
    
    /* NUMBER */
    {
        DTL_NUMERIC_MAX_PRECISION,      /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_NUMERIC_MAX_SIZE,           /**< value buffer size */
        DTL_NUMERIC_STRING_MAX_SIZE,    /**< string buffer size */
    },

    /* DECIMAL */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_LENGTH_NA,                  /**< value buffer size */
        DTL_LENGTH_NA,                  /**< string buffer size */
    },


    /* CHAR */
    {
        DTL_CHAR_MAX_PRECISION,         /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_CHARACTERS,  /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_CHAR_MAX_BUFFER_SIZE,       /**< value buffer size */
        DTL_CHAR_MAX_BUFFER_SIZE,       /**< string buffer size */
    },

    /* VARCHAR */
    {
        DTL_VARCHAR_MAX_PRECISION,      /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_CHARACTERS,  /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_VARCHAR_MAX_BUFFER_SIZE,    /**< value buffer size */
        DTL_VARCHAR_MAX_BUFFER_SIZE,    /**< string buffer size */
    },

    /* LONGVARCHAR */
    {
        DTL_LONGVARCHAR_MAX_PRECISION,    /**< precision */
        DTL_SCALE_NA,                     /**< scale */
        DTL_STRING_LENGTH_UNIT_OCTETS,    /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,        /**< interval indicator */
        
        DTL_LONGVARYING_INIT_STRING_SIZE,  /**< value buffer size */
        DTL_LONGVARYING_INIT_STRING_SIZE,  /**< string buffer size */
    },

    /* CLOB */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_LENGTH_NA,                  /**< value buffer size */
        DTL_LENGTH_NA,                  /**< string buffer size */
    },


    /* BINARY */
    {
        DTL_BINARY_MAX_PRECISION,       /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_BINARY_STRING_SIZE,         /**< value buffer size */
        DTL_BINARY_STRING_SIZE,         /**< string buffer size */
    },

    /* VARBINARY */
    {
        DTL_VARBINARY_MAX_PRECISION,   /**< precision */
        DTL_SCALE_NA,                  /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,     /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,     /**< interval indicator */
        
        DTL_VARBINARY_STRING_SIZE,     /**< value buffer size */
        DTL_VARBINARY_STRING_SIZE,     /**< string buffer size */
    },

    /* LONGVARBINARY */
    {
        DTL_LONGVARBINARY_MAX_PRECISION,   /**< precision */
        DTL_SCALE_NA,                      /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,         /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,         /**< interval indicator */
        
        DTL_LONGVARYING_INIT_STRING_SIZE,     /**< value buffer size */
        DTL_LONGVARYING_INIT_STRING_SIZE,     /**< string buffer size */
    },

    /* BLOB */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_LENGTH_NA,                  /**< value buffer size */
        DTL_LENGTH_NA,                  /**< string buffer size */
    },


    /* DATE */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_DATE_SIZE,                  /**< value buffer size */
        DTL_DATE_STRING_SIZE + 1,       /**< string buffer size */
    },

    /* TIME WITHOUT TIME ZONE */
    {
        DTL_TIME_MAX_PRECISION,         /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_TIME_SIZE,                  /**< value buffer size */
        DTL_TIME_STRING_SIZE + 1,       /**< string buffer size */
    },

    /* TIMESTAMP WITHOUT TIME ZONE */
    {
        DTL_TIMESTAMP_MAX_PRECISION,    /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_TIMESTAMP_SIZE,             /**< value buffer size */
        DTL_TIMESTAMP_STRING_SIZE + 1,  /**< string buffer size */
    },

    /* TIME WITH TIME ZONE */
    {
        DTL_TIMETZ_MAX_PRECISION,       /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_TIMETZ_SIZE,                /**< value buffer size */
        DTL_TIMETZ_STRING_SIZE + 1,     /**< string buffer size */
    },

    /* TIMESTAMP WITH TIME ZONE */
    {
        DTL_TIMESTAMPTZ_MAX_PRECISION,  /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_TIMESTAMPTZ_SIZE,           /**< value buffer size */
        DTL_TIMESTAMPTZ_STRING_SIZE + 1, /**< string buffer size */
    },

    /* INTERVAL YEAR TO MONTH */
    {
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,         /**< precision */
        DTL_SCALE_NA,                                     /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,                        /**< string length unit */
        DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH,             /**< interval indicator */
        
        DTL_INTERVAL_YEAR_TO_MONTH_SIZE,                  /**< value buffer size */
        DTL_INTERVAL_YEAR_TO_MONTH_MAX_STRING_SIZE + 1,   /**< string buffer size */
    },

    /* INTERVAL DAY TO SECOND */
    {
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,         /**< precision */
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,     /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,                        /**< string length unit */
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND,             /**< interval indicator */
        
        DTL_INTERVAL_DAY_TO_SECOND_SIZE,                  /**< value buffer size */
        DTL_INTERVAL_DAY_TO_SECOND_MAX_STRING_SIZE + 1,   /**< string buffer size */
    },

    /* ROWID */
    {
        DTL_PRECISION_NA,               /**< precision */
        DTL_SCALE_NA,                   /**< scale */
        DTL_STRING_LENGTH_UNIT_NA,      /**< string length unit */
        DTL_INTERVAL_INDICATOR_NA,      /**< interval indicator */
        
        DTL_ROWID_SIZE,                 /**< value buffer size */
        DTL_ROWID_STRING_SIZE,          /**< string buffer size */
    }           
};



const dtlDataTypeInfoDomain gDataTypeInfoDomain[ DTL_TYPE_MAX ] =
{
    /* BOOLEAN */
    {
        DTL_PRECISION_NA,            /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_PRECISION_NA,            /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    /* NATIVE_SMALLINT */
    {
        DTL_PRECISION_NA,            /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_PRECISION_NA,            /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    /* NATIVE_INTEGER */
    {
        DTL_PRECISION_NA,            /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_PRECISION_NA,            /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    /* NATIVE_BIGINT */
    {
        DTL_PRECISION_NA,            /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_PRECISION_NA,            /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    
    /* NATIVE_REAL */
    {
        DTL_PRECISION_NA,            /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_PRECISION_NA,            /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    /* NATIVE_DOUBLE */
    {
        DTL_PRECISION_NA,            /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_PRECISION_NA,            /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    /* FLOAT */
    {
        DTL_FLOAT_MIN_PRECISION,     /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_FLOAT_MAX_PRECISION,     /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : max */
    },
    
    /* NUMBER */
    {
        DTL_NUMERIC_MIN_PRECISION,   /**< precision : min */
        DTL_NUMERIC_MIN_SCALE,       /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_NUMERIC_MAX_PRECISION,   /**< precision : max */
        DTL_NUMERIC_MAX_SCALE,       /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : max */
    },

    /* DECIMAL */
    {
        DTL_PRECISION_NA,            /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_PRECISION_NA,            /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },


    /* CHAR */
    {
        DTL_CHAR_MIN_PRECISION,          /**< precision : min */
        DTL_SCALE_NA,                    /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA + 1,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,       /**< interval indicator : min */
        
        DTL_CHAR_MAX_PRECISION,          /**< precision : max */
        DTL_SCALE_NA,                    /**< scale : max */
        DTL_STRING_LENGTH_UNIT_MAX - 1,  /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA        /**< interval indicator : max */
    },

    /* VARCHAR */
    {
        DTL_VARCHAR_MIN_PRECISION,       /**< precision : min */
        DTL_SCALE_NA,                    /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA + 1,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,       /**< interval indicator : min */
        
        DTL_VARCHAR_MAX_PRECISION,       /**< precision : max */
        DTL_SCALE_NA,                    /**< scale : max */
        DTL_STRING_LENGTH_UNIT_MAX - 1,  /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA        /**< interval indicator : max */
    },

    /* LONGVARCHAR */
    {
        DTL_LONGVARCHAR_MAX_PRECISION,   /**< precision : min */
        DTL_SCALE_NA,                    /**< scale : min */
        DTL_STRING_LENGTH_UNIT_OCTETS,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,       /**< interval indicator : min */
        
        DTL_LONGVARCHAR_MAX_PRECISION,   /**< precision : max */
        DTL_SCALE_NA,                    /**< scale : max */
        DTL_STRING_LENGTH_UNIT_OCTETS,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA        /**< interval indicator : max */
    },

    /* CLOB */
    {
        DTL_PRECISION_NA,            /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_PRECISION_NA,            /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },


    /* BINARY */
    {
        DTL_BINARY_MIN_PRECISION,    /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_BINARY_MAX_PRECISION,    /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    /* VARBINARY */
    {
        DTL_VARBINARY_MIN_PRECISION, /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */

        DTL_VARBINARY_MAX_PRECISION, /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    /* LONGVARBINARY */
    {
        DTL_LONGVARBINARY_MAX_PRECISION,   /**< precision : min */
        DTL_SCALE_NA,                      /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,         /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,         /**< interval indicator : min */
        
        DTL_LONGVARBINARY_MAX_PRECISION,   /**< precision : max */
        DTL_SCALE_NA,                      /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,         /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA          /**< interval indicator : max */
    },

    /* BLOB */
    {
        DTL_PRECISION_NA,            /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_PRECISION_NA,            /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },


    /* DATE */
    {
        DTL_PRECISION_NA,            /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_PRECISION_NA,            /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    /* TIME WITHOUT TIME ZONE */
    {
        DTL_TIME_MIN_PRECISION,      /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_TIME_MAX_PRECISION,      /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    /* TIMESTAMP WITHOUT TIME ZONE */
    {
        DTL_TIMESTAMP_MIN_PRECISION, /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_TIMESTAMP_MAX_PRECISION, /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    /* TIME WITH TIME ZONE */
    {
        DTL_TIMETZ_MIN_PRECISION,    /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_TIMETZ_MAX_PRECISION,    /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    },

    /* TIMESTAMP WITH TIME ZONE */
    {
        DTL_TIMESTAMPTZ_MIN_PRECISION,   /**< precision : min */
        DTL_SCALE_NA,                    /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,       /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,       /**< interval indicator : min */
        
        DTL_TIMESTAMPTZ_MAX_PRECISION,   /**< precision : max */
        DTL_SCALE_NA,                    /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,       /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA        /**< interval indicator : max */
    },

    /* INTERVAL YEAR TO MONTH */
    {
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,       /**< precision : min */
        DTL_SCALE_NA,                                   /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,                      /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA + 1,                  /**< interval indicator : min */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,       /**< precision : max */
        DTL_SCALE_NA,                                   /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,                      /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_MAX - 1,                 /**< interval indicator : max */
    },

    /* INTERVAL DAY TO SECOND */
    {
        DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,       /**< precision : min */
        DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION,   /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,                      /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA + 1,                  /**< interval indicator : min */
        
        DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION,       /**< precision : max */
        DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION,   /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,                      /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_MAX - 1,                 /**< interval indicator : max */
    },

    /* ROWID */
    {
        DTL_PRECISION_NA,            /**< precision : min */
        DTL_SCALE_NA,                /**< scale : min */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : min */
        DTL_INTERVAL_INDICATOR_NA,   /**< interval indicator : min */
        
        DTL_PRECISION_NA,            /**< precision : max */
        DTL_SCALE_NA,                /**< scale : max */
        DTL_STRING_LENGTH_UNIT_NA,   /**< string length unit : max */
        DTL_INTERVAL_INDICATOR_NA    /**< interval indicator : max */
    }
};


const stlInt32 gDtlCompValueIgnoreSize[ DTL_TYPE_MAX ] =
{
    0,                              /**< BOOLEAN */
    0,                              /**< NATIVE_SMALLINT */
    0,                              /**< NATIVE_INTEGER */
    0,                              /**< NATIVE_BIGINT */

    0,                              /**< NATIVE_REAL */
    0,                              /**< NATIVE_DOUBLE */

    0,                              /**< FLOAT */
    0,                              /**< NUMBER */
    0,                              /**< unsupported feature, DECIMAL */
    
    0,                              /**< CHARACTER */
    0,                              /**< CHARACTER VARYING */
    0,                              /**< CHARACTER LONG VARYING */
    0,                              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
    0,                              /**< BINARY */
    0,                              /**< BINARY VARYING */
    0,                              /**< BINARY LONG VARYING */
    0,                              /**< unsupported feature, BINARY LARGE OBJECT */

    0,                              /**< DATE */
    0,                              /**< TIME WITHOUT TIME ZONE */
    0,                              /**< TIMESTAMP WITHOUT TIME ZONE */
    STL_SIZEOF( stlInt32 ),         /**< TIME WITH TIME ZONE */
    STL_SIZEOF( stlInt32 ),         /**< TIMESTAMP WITH TIME ZONE */

    STL_SIZEOF( stlInt32 ),         /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    STL_SIZEOF( stlInt32 ),         /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

    0                               /**< DTL_TYPE_ROWID */
};


const stlUInt16 dtlNumericDigitSize[ DTL_NUMERIC_MAX_PRECISION + 1 ] =
{
    1,   /* precision : 0  */
    1,   /* precision : 1  */
    2,   /* precision : 2  */
    2,   /* precision : 3  */
    3,   /* precision : 4  */
    3,   /* precision : 5  */
    4,   /* precision : 6  */
    4,   /* precision : 7  */
    5,   /* precision : 8  */
    5,   /* precision : 9  */
    
    6,   /* precision : 10 */
    6,   /* precision : 11 */
    7,   /* precision : 12 */
    7,   /* precision : 13 */
    8,   /* precision : 14 */
    8,   /* precision : 15 */
    9,   /* precision : 16 */
    9,   /* precision : 17 */
    10,  /* precision : 18 */
    10,  /* precision : 19 */
    
    11,  /* precision : 20 */
    11,  /* precision : 21 */
    12,  /* precision : 22 */
    12,  /* precision : 23 */
    13,  /* precision : 24 */
    13,  /* precision : 25 */
    14,  /* precision : 26 */
    14,  /* precision : 27 */
    15,  /* precision : 28 */
    15,  /* precision : 29 */

    16,  /* precision : 30 */
    16,  /* precision : 31 */
    17,  /* precision : 32 */
    17,  /* precision : 33 */
    18,  /* precision : 34 */
    18,  /* precision : 35 */
    19,  /* precision : 36 */
    19,  /* precision : 37 */
    20   /* precision : 38 */
};

const stlUInt16 dtlBinaryToDecimalPrecision[ DTL_FLOAT_MAX_PRECISION + 1 ] =
{
    0,   /* precision : 0  */
    1,   /* precision : 1  */
    1,   /* precision : 2  */
    1,   /* precision : 3  */
    2,   /* precision : 4  */
    2,   /* precision : 5  */
    2,   /* precision : 6  */
    3,   /* precision : 7  */
    3,   /* precision : 8  */
    3,   /* precision : 9  */

    4,   /* precision : 10 */
    4,   /* precision : 11 */
    4,   /* precision : 12 */
    4,   /* precision : 13 */
    5,   /* precision : 14 */
    5,   /* precision : 15 */
    5,   /* precision : 16 */
    6,   /* precision : 17 */
    6,   /* precision : 18 */
    6,   /* precision : 19 */
    
    7,   /* precision : 20 */
    7,   /* precision : 21 */
    7,   /* precision : 22 */
    7,   /* precision : 23 */
    8,   /* precision : 24 */
    8,   /* precision : 25 */
    8,   /* precision : 26 */
    9,   /* precision : 27 */
    9,   /* precision : 28 */
    9,   /* precision : 29 */

    10,  /* precision : 30 */
    10,  /* precision : 31 */
    10,  /* precision : 32 */
    10,  /* precision : 33 */
    11,  /* precision : 34 */
    11,  /* precision : 35 */
    11,  /* precision : 36 */
    12,  /* precision : 37 */
    12,  /* precision : 38 */
    12,  /* precision : 39 */

    13,  /* precision : 40 */
    13,  /* precision : 41 */
    13,  /* precision : 42 */
    13,  /* precision : 43 */
    14,  /* precision : 44 */
    14,  /* precision : 45 */
    14,  /* precision : 46 */
    15,  /* precision : 47 */
    15,  /* precision : 48 */
    15,  /* precision : 49 */

    16,  /* precision : 50 */
    16,  /* precision : 51 */
    16,  /* precision : 52 */
    16,  /* precision : 53 */
    17,  /* precision : 54 */
    17,  /* precision : 55 */
    17,  /* precision : 56 */
    18,  /* precision : 57 */
    18,  /* precision : 58 */
    18,  /* precision : 59 */

    19,  /* precision : 60 */
    19,  /* precision : 61 */
    19,  /* precision : 62 */
    19,  /* precision : 63 */
    20,  /* precision : 64 */
    20,  /* precision : 65 */
    20,  /* precision : 66 */
    21,  /* precision : 67 */
    21,  /* precision : 68 */
    21,  /* precision : 69 */

    22,  /* precision : 70 */
    22,  /* precision : 71 */
    22,  /* precision : 72 */
    22,  /* precision : 73 */
    23,  /* precision : 74 */
    23,  /* precision : 75 */
    23,  /* precision : 76 */
    24,  /* precision : 77 */
    24,  /* precision : 78 */
    24,  /* precision : 79 */

    25,  /* precision : 80 */
    25,  /* precision : 81 */
    25,  /* precision : 82 */
    25,  /* precision : 83 */
    26,  /* precision : 84 */
    26,  /* precision : 85 */
    26,  /* precision : 86 */
    27,  /* precision : 87 */
    27,  /* precision : 88 */
    27,  /* precision : 89 */

    28,  /* precision : 90 */
    28,  /* precision : 91 */
    28,  /* precision : 92 */
    28,  /* precision : 93 */
    29,  /* precision : 94 */
    29,  /* precision : 95 */
    29,  /* precision : 96 */
    30,  /* precision : 97 */
    30,  /* precision : 98 */
    30,  /* precision : 99 */

    31,  /* precision : 100 */
    31,  /* precision : 101 */
    31,  /* precision : 102 */
    31,  /* precision : 103 */
    32,  /* precision : 104 */
    32,  /* precision : 105 */
    32,  /* precision : 106 */
    33,  /* precision : 107 */
    33,  /* precision : 108 */
    33,  /* precision : 109 */

    34,  /* precision : 110 */
    34,  /* precision : 111 */
    34,  /* precision : 112 */
    34,  /* precision : 113 */
    35,  /* precision : 114 */
    35,  /* precision : 115 */
    35,  /* precision : 116 */
    36,  /* precision : 117 */
    36,  /* precision : 118 */
    36,  /* precision : 119 */
    
    37,  /* precision : 120 */
    37,  /* precision : 121 */
    37,  /* precision : 122 */
    37,  /* precision : 123 */
    38,  /* precision : 124 */
    38,  /* precision : 125 */
    38   /* precision : 126 */
};

const stlUInt16 dtlDecimalToBinaryPrecision[ DTL_NUMERIC_MAX_PRECISION + 1] =
{
    0,    /* precision : 0  */
    3,    /* precision : 1  */
    6,    /* precision : 2  */
    9,    /* precision : 3  */
    13,   /* precision : 4  */
    16,   /* precision : 5  */
    19,   /* precision : 6  */
    23,   /* precision : 7  */
    26,   /* precision : 8  */
    29,   /* precision : 9  */

    33,   /* precision : 10 */
    36,   /* precision : 11 */
    39,   /* precision : 12 */
    43,   /* precision : 13 */
    46,   /* precision : 14 */
    49,   /* precision : 15 */
    53,   /* precision : 16 */
    56,   /* precision : 17 */
    59,   /* precision : 18 */
    63,   /* precision : 19 */
    
    66,   /* precision : 20 */
    69,   /* precision : 21 */
    73,   /* precision : 22 */
    76,   /* precision : 23 */
    79,   /* precision : 24 */
    83,   /* precision : 25 */
    86,   /* precision : 26 */
    89,   /* precision : 27 */
    93,   /* precision : 28 */
    96,   /* precision : 29 */

    99,  /* precision : 30 */
    103, /* precision : 31 */
    106, /* precision : 32 */
    109, /* precision : 33 */
    113, /* precision : 34 */
    116, /* precision : 35 */
    119, /* precision : 36 */
    123, /* precision : 37 */
    126  /* precision : 38 */
};


/**
 * @brief 각 data type의 max precision을 고려한 최대 버퍼 크기
 * @note Long Varying type은 최대 버퍼사이즈를 사용해서는 안된다.
 */
const stlUInt32 dtlDataTypeMaxBufferSize[ DTL_TYPE_MAX ] =
{
    DTL_BOOLEAN_SIZE,
    DTL_NATIVE_SMALLINT_SIZE,
    DTL_NATIVE_INTEGER_SIZE,
    DTL_NATIVE_BIGINT_SIZE,

    DTL_NATIVE_REAL_SIZE,
    DTL_NATIVE_DOUBLE_SIZE,

    DTL_NUMERIC_MAX_SIZE,
    DTL_NUMERIC_MAX_SIZE,
    0,                            /**< unsupported feature, DECIMAL */

    DTL_CHAR_MAX_BUFFER_SIZE,
    DTL_VARCHAR_MAX_BUFFER_SIZE,
    DTL_NO_TOTAL_SIZE,
    0,                            /**< unsupported feature, CHARACTER LARGE OBJECT */

    DTL_BINARY_STRING_SIZE,
    DTL_VARBINARY_STRING_SIZE,
    DTL_NO_TOTAL_SIZE,
    0,                            /**< unsupported feature, BINARY LARGE OBJECT */

    DTL_DATE_SIZE,
    DTL_TIME_SIZE,
    DTL_TIMESTAMP_SIZE,
    DTL_TIMETZ_SIZE,
    DTL_TIMESTAMPTZ_SIZE,
    
    DTL_INTERVAL_YEAR_TO_MONTH_SIZE,
    DTL_INTERVAL_DAY_TO_SECOND_SIZE,

    DTL_ROWID_SIZE
};

/**
 * @brief Comparison 연산시 Function Argument 의 최소한의 Type Conversion 을 위한 Array
 */
const dtlDataType dtlComparisonFuncArgType[ DTL_TYPE_MAX ][ DTL_TYPE_MAX ] =
{
    /* BOOLEAN */
    {
        DTL_TYPE_BOOLEAN, /**< BOOLEAN */
        DTL_TYPE_NA,     /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,     /**< NATIVE_INTEGER */
        DTL_TYPE_NA,     /**< NATIVE_BIGINT */

        DTL_TYPE_NA,     /**< NATIVE_REAL */
        DTL_TYPE_NA,     /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,     /**< FLOAT */
        DTL_TYPE_NA,     /**< NUMBER */
        DTL_TYPE_NA,     /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_BOOLEAN, /**< CHARACTER */
        DTL_TYPE_BOOLEAN, /**< CHARACTER VARYING */
        DTL_TYPE_BOOLEAN, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,     /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,     /**< BINARY */
        DTL_TYPE_NA,     /**< BINARY VARYING */
        DTL_TYPE_NA,     /**< BINARY LONG VARYING */
        DTL_TYPE_NA,     /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,     /**< DATE */
        DTL_TYPE_NA,     /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,     /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,     /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,     /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,     /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,     /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,     /**< DTL_TYPE_ROWID */
    },

    /* NATIVE_SMALLINT */
    {
        DTL_TYPE_NA, /**< BOOLEAN */
        DTL_TYPE_NATIVE_SMALLINT, /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,  /**< NATIVE_INTEGER */
        DTL_TYPE_NA,   /**< NATIVE_BIGINT */

        DTL_TYPE_NA,     /**< NATIVE_REAL */
        DTL_TYPE_NA,   /**< NATIVE_DOUBLE */

        DTL_TYPE_NA, /**< FLOAT */
        DTL_TYPE_NA, /**< NUMBER */
        DTL_TYPE_NA, /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NA, /**< CHARACTER */
        DTL_TYPE_NA, /**< CHARACTER VARYING */
        DTL_TYPE_NA, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA, /**< BINARY */
        DTL_TYPE_NA, /**< BINARY VARYING */
        DTL_TYPE_NA, /**< BINARY LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA, /**< DATE */
        DTL_TYPE_NA, /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA, /**< DTL_TYPE_ROWID */
    },

    /* NATIVE_INTEGER */
    {
        DTL_TYPE_NA, /**< BOOLEAN */
        DTL_TYPE_NATIVE_SMALLINT, /**< NATIVE_SMALLINT */
        DTL_TYPE_NATIVE_INTEGER, /**< NATIVE_INTEGER */
        DTL_TYPE_NA, /**< NATIVE_BIGINT */

        DTL_TYPE_NA, /**< NATIVE_REAL */
        DTL_TYPE_NA, /**< NATIVE_DOUBLE */

        DTL_TYPE_NA, /**< FLOAT */
        DTL_TYPE_NA, /**< NUMBER */
        DTL_TYPE_NA, /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NA, /**< CHARACTER */
        DTL_TYPE_NA, /**< CHARACTER VARYING */
        DTL_TYPE_NA, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA, /**< BINARY */
        DTL_TYPE_NA, /**< BINARY VARYING */
        DTL_TYPE_NA, /**< BINARY LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA, /**< DATE */
        DTL_TYPE_NA, /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA, /**< DTL_TYPE_ROWID */
    },

    /* NATIVE_BIGINT */
    {
        DTL_TYPE_NA,              /**< BOOLEAN */
        DTL_TYPE_NATIVE_SMALLINT,  /**< NATIVE_SMALLINT */
        DTL_TYPE_NATIVE_INTEGER,   /**< NATIVE_INTEGER */
        DTL_TYPE_NATIVE_BIGINT,    /**< NATIVE_BIGINT */

        DTL_TYPE_NA,              /**< NATIVE_REAL */
        DTL_TYPE_NA,              /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,              /**< FLOAT */
        DTL_TYPE_NA,              /**< NUMBER */
        DTL_TYPE_NA,              /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NA,           /**< CHARACTER */
        DTL_TYPE_NA,           /**< CHARACTER VARYING */
        DTL_TYPE_NA,           /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,              /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,              /**< BINARY */
        DTL_TYPE_NA,              /**< BINARY VARYING */
        DTL_TYPE_NA,              /**< BINARY LONG VARYING */
        DTL_TYPE_NA,              /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,              /**< DATE */
        DTL_TYPE_NA,              /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,              /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,              /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,              /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,    /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,    /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA               /**< DTL_TYPE_ROWID */
    },

    /* NATIVE_REAL */
    {
        DTL_TYPE_NA, /**< BOOLEAN */
        DTL_TYPE_NATIVE_SMALLINT, /**< NATIVE_SMALLINT */
        DTL_TYPE_NA, /**< NATIVE_INTEGER */
        DTL_TYPE_NA, /**< NATIVE_BIGINT */

        DTL_TYPE_NATIVE_REAL, /**< NATIVE_REAL */
        DTL_TYPE_NA, /**< NATIVE_DOUBLE */

        DTL_TYPE_NA, /**< FLOAT */
        DTL_TYPE_NA, /**< NUMBER */
        DTL_TYPE_NA, /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NA, /**< CHARACTER */
        DTL_TYPE_NA, /**< CHARACTER VARYING */
        DTL_TYPE_NA, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA, /**< BINARY */
        DTL_TYPE_NA, /**< BINARY VARYING */
        DTL_TYPE_NA, /**< BINARY LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA, /**< DATE */
        DTL_TYPE_NA, /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA, /**< DTL_TYPE_ROWID */
    },

    /* NATIVE_DOUBLE */
    {
        DTL_TYPE_NA,             /**< BOOLEAN */
        DTL_TYPE_NATIVE_SMALLINT, /**< NATIVE_SMALLINT */
        DTL_TYPE_NATIVE_INTEGER,  /**< NATIVE_INTEGER */
        DTL_TYPE_NATIVE_BIGINT,   /**< NATIVE_BIGINT */

        DTL_TYPE_NATIVE_REAL,     /**< NATIVE_REAL */
        DTL_TYPE_NATIVE_DOUBLE,   /**< NATIVE_DOUBLE */

        DTL_TYPE_FLOAT,           /**< FLOAT */
        DTL_TYPE_NUMBER,          /**< NUMBER */
        DTL_TYPE_NA,             /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NUMBER,          /**< CHARACTER */
        DTL_TYPE_NUMBER,          /**< CHARACTER VARYING */
        DTL_TYPE_NUMBER,          /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,             /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,             /**< BINARY */
        DTL_TYPE_NA,             /**< BINARY VARYING */
        DTL_TYPE_NA,             /**< BINARY LONG VARYING */
        DTL_TYPE_NA,             /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,             /**< DATE */
        DTL_TYPE_NA,             /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,             /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,             /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,             /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,             /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,             /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,             /**< DTL_TYPE_ROWID */
    },

    /* FLOAT */
    {
        DTL_TYPE_NA, /**< BOOLEAN */
        DTL_TYPE_NA, /**< NATIVE_SMALLINT */
        DTL_TYPE_NA, /**< NATIVE_INTEGER */
        DTL_TYPE_NA, /**< NATIVE_BIGINT */

        DTL_TYPE_NA, /**< NATIVE_REAL */
        DTL_TYPE_NA, /**< NATIVE_DOUBLE */

        DTL_TYPE_FLOAT, /**< FLOAT */
        DTL_TYPE_NA, /**< NUMBER */
        DTL_TYPE_NA, /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NA, /**< CHARACTER */
        DTL_TYPE_NA, /**< CHARACTER VARYING */
        DTL_TYPE_NA, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA, /**< BINARY */
        DTL_TYPE_NA, /**< BINARY VARYING */
        DTL_TYPE_NA, /**< BINARY LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA, /**< DATE */
        DTL_TYPE_NA, /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA, /**< DTL_TYPE_ROWID */
    },

    /* NUMBER */
    {
        DTL_TYPE_NA,              /**< BOOLEAN */
        DTL_TYPE_NATIVE_SMALLINT,  /**< NATIVE_SMALLINT */
        DTL_TYPE_NATIVE_INTEGER,   /**< NATIVE_INTEGER */
        DTL_TYPE_NATIVE_BIGINT,    /**< NATIVE_BIGINT */

        DTL_TYPE_NA,    /**< NATIVE_REAL */
        DTL_TYPE_NA,    /**< NATIVE_DOUBLE */

        DTL_TYPE_FLOAT,  /**< FLOAT */
        DTL_TYPE_NUMBER, /**< NUMBER */
        DTL_TYPE_NA,    /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NUMBER, /**< CHARACTER */
        DTL_TYPE_NUMBER, /**< CHARACTER VARYING */
        DTL_TYPE_NUMBER, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,    /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,    /**< BINARY */
        DTL_TYPE_NA,    /**< BINARY VARYING */
        DTL_TYPE_NA,    /**< BINARY LONG VARYING */
        DTL_TYPE_NA,    /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,    /**< DATE */
        DTL_TYPE_NA,    /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,    /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,    /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,    /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,    /**< DTL_TYPE_ROWID */
    },

    /* DECIMAL */
    {
        DTL_TYPE_NA, /**< BOOLEAN */
        DTL_TYPE_NA, /**< NATIVE_SMALLINT */
        DTL_TYPE_NA, /**< NATIVE_INTEGER */
        DTL_TYPE_NA, /**< NATIVE_BIGINT */

        DTL_TYPE_NA, /**< NATIVE_REAL */
        DTL_TYPE_NA, /**< NATIVE_DOUBLE */

        DTL_TYPE_NA, /**< FLOAT */
        DTL_TYPE_NA, /**< NUMBER */
        DTL_TYPE_NA, /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NA, /**< CHARACTER */
        DTL_TYPE_NA, /**< CHARACTER VARYING */
        DTL_TYPE_NA, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA, /**< BINARY */
        DTL_TYPE_NA, /**< BINARY VARYING */
        DTL_TYPE_NA, /**< BINARY LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA, /**< DATE */
        DTL_TYPE_NA, /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA, /**< DTL_TYPE_ROWID */
    },

    /* CHAR */
    {
        DTL_TYPE_NA, /**< BOOLEAN */
        DTL_TYPE_NA, /**< NATIVE_SMALLINT */
        DTL_TYPE_NA, /**< NATIVE_INTEGER */
        DTL_TYPE_NA, /**< NATIVE_BIGINT */

        DTL_TYPE_NA, /**< NATIVE_REAL */
        DTL_TYPE_NA, /**< NATIVE_DOUBLE */

        DTL_TYPE_NA, /**< FLOAT */
        DTL_TYPE_NA, /**< NUMBER */
        DTL_TYPE_NA, /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_CHAR, /**< CHARACTER */
        DTL_TYPE_NA, /**< CHARACTER VARYING */
        DTL_TYPE_NA, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA, /**< BINARY */
        DTL_TYPE_NA, /**< BINARY VARYING */
        DTL_TYPE_NA, /**< BINARY LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA, /**< DATE */
        DTL_TYPE_NA, /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA, /**< DTL_TYPE_ROWID */
    },

    /* VARCHAR */
    {
        DTL_TYPE_NA,     /**< BOOLEAN */
        DTL_TYPE_NA,     /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,     /**< NATIVE_INTEGER */
        DTL_TYPE_NA,     /**< NATIVE_BIGINT */

        DTL_TYPE_NA,     /**< NATIVE_REAL */
        DTL_TYPE_NA,     /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,     /**< FLOAT */
        DTL_TYPE_NA,     /**< NUMBER */
        DTL_TYPE_NA,         /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_CHAR,        /**< CHARACTER */
        DTL_TYPE_VARCHAR,     /**< CHARACTER VARYING */
        DTL_TYPE_NA, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,         /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,         /**< BINARY */
        DTL_TYPE_NA,         /**< BINARY VARYING */
        DTL_TYPE_NA,         /**< BINARY LONG VARYING */
        DTL_TYPE_NA,         /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,     /**< DATE */
        DTL_TYPE_NA,     /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,     /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,     /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,     /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,     /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,     /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,     /**< DTL_TYPE_ROWID */
    },

    /* LONGVARCHAR */
    {
        DTL_TYPE_NA,     /**< BOOLEAN */
        DTL_TYPE_NA,     /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,     /**< NATIVE_INTEGER */
        DTL_TYPE_NA,     /**< NATIVE_BIGINT */

        DTL_TYPE_NA,     /**< NATIVE_REAL */
        DTL_TYPE_NA,     /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,     /**< FLOAT */
        DTL_TYPE_NA,     /**< NUMBER */
        DTL_TYPE_NA,         /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_CHAR,        /**< CHARACTER */
        DTL_TYPE_VARCHAR,     /**< CHARACTER VARYING */
        DTL_TYPE_LONGVARCHAR, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,         /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,         /**< BINARY */
        DTL_TYPE_NA,         /**< BINARY VARYING */
        DTL_TYPE_NA,         /**< BINARY LONG VARYING */
        DTL_TYPE_NA,         /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,     /**< DATE */
        DTL_TYPE_NA,     /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,     /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,     /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,     /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,     /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,     /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,     /**< DTL_TYPE_ROWID */
    },

    /* CLOB */
    {
        DTL_TYPE_NA, /**< BOOLEAN */
        DTL_TYPE_NA, /**< NATIVE_SMALLINT */
        DTL_TYPE_NA, /**< NATIVE_INTEGER */
        DTL_TYPE_NA, /**< NATIVE_BIGINT */

        DTL_TYPE_NA, /**< NATIVE_REAL */
        DTL_TYPE_NA, /**< NATIVE_DOUBLE */

        DTL_TYPE_NA, /**< FLOAT */
        DTL_TYPE_NA, /**< NUMBER */
        DTL_TYPE_NA, /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NA, /**< CHARACTER */
        DTL_TYPE_NA, /**< CHARACTER VARYING */
        DTL_TYPE_NA, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA, /**< BINARY */
        DTL_TYPE_NA, /**< BINARY VARYING */
        DTL_TYPE_NA, /**< BINARY LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA, /**< DATE */
        DTL_TYPE_NA, /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA, /**< DTL_TYPE_ROWID */
    },

    /* BINARY */
    {
        DTL_TYPE_NA,           /**< BOOLEAN */
        DTL_TYPE_NA,           /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,           /**< NATIVE_INTEGER */
        DTL_TYPE_NA,           /**< NATIVE_BIGINT */

        DTL_TYPE_NA,           /**< NATIVE_REAL */
        DTL_TYPE_NA,           /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,           /**< FLOAT */
        DTL_TYPE_NA,           /**< NUMBER */
        DTL_TYPE_NA,           /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NA,           /**< CHARACTER */
        DTL_TYPE_NA,           /**< CHARACTER VARYING */
        DTL_TYPE_NA,           /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,           /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_BINARY,       /**< BINARY */
        DTL_TYPE_NA,           /**< BINARY VARYING */
        DTL_TYPE_NA,           /**< BINARY LONG VARYING */
        DTL_TYPE_NA,           /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,           /**< DATE */
        DTL_TYPE_NA,           /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,           /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,           /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,           /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,           /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,           /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,           /**< DTL_TYPE_ROWID */
    },

    /* VARBINARY */
    {
        DTL_TYPE_NA,           /**< BOOLEAN */
        DTL_TYPE_NA,           /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,           /**< NATIVE_INTEGER */
        DTL_TYPE_NA,           /**< NATIVE_BIGINT */

        DTL_TYPE_NA,           /**< NATIVE_REAL */
        DTL_TYPE_NA,           /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,           /**< FLOAT */
        DTL_TYPE_NA,           /**< NUMBER */
        DTL_TYPE_NA,           /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NA,           /**< CHARACTER */
        DTL_TYPE_NA,           /**< CHARACTER VARYING */
        DTL_TYPE_NA,           /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,           /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_BINARY,       /**< BINARY */
        DTL_TYPE_VARBINARY,    /**< BINARY VARYING */
        DTL_TYPE_NA,           /**< BINARY LONG VARYING */
        DTL_TYPE_NA,           /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,           /**< DATE */
        DTL_TYPE_NA,           /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,           /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,           /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,           /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,           /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,           /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,           /**< DTL_TYPE_ROWID */
    },

    /* LONGVARBINARY */
    {
        DTL_TYPE_NA,           /**< BOOLEAN */
        DTL_TYPE_NA,           /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,           /**< NATIVE_INTEGER */
        DTL_TYPE_NA,           /**< NATIVE_BIGINT */

        DTL_TYPE_NA,           /**< NATIVE_REAL */
        DTL_TYPE_NA,           /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,           /**< FLOAT */
        DTL_TYPE_NA,           /**< NUMBER */
        DTL_TYPE_NA,           /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NA,           /**< CHARACTER */
        DTL_TYPE_NA,           /**< CHARACTER VARYING */
        DTL_TYPE_NA,           /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,           /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_BINARY,        /**< BINARY */
        DTL_TYPE_VARBINARY,     /**< BINARY VARYING */
        DTL_TYPE_LONGVARBINARY, /**< BINARY LONG VARYING */
        DTL_TYPE_NA,           /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,           /**< DATE */
        DTL_TYPE_NA,           /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,           /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,           /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,           /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,           /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,           /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,           /**< DTL_TYPE_ROWID */
    },

    /* BLOB */
    {
        DTL_TYPE_NA, /**< BOOLEAN */
        DTL_TYPE_NA, /**< NATIVE_SMALLINT */
        DTL_TYPE_NA, /**< NATIVE_INTEGER */
        DTL_TYPE_NA, /**< NATIVE_BIGINT */

        DTL_TYPE_NA, /**< NATIVE_REAL */
        DTL_TYPE_NA, /**< NATIVE_DOUBLE */

        DTL_TYPE_NA, /**< FLOAT */
        DTL_TYPE_NA, /**< NUMBER */
        DTL_TYPE_NA, /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_NA, /**< CHARACTER */
        DTL_TYPE_NA, /**< CHARACTER VARYING */
        DTL_TYPE_NA, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA, /**< BINARY */
        DTL_TYPE_NA, /**< BINARY VARYING */
        DTL_TYPE_NA, /**< BINARY LONG VARYING */
        DTL_TYPE_NA, /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA, /**< DATE */
        DTL_TYPE_NA, /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA, /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA, /**< DTL_TYPE_ROWID */
    },

    /* DATE */
    {
        DTL_TYPE_NA,  /**< BOOLEAN */
        DTL_TYPE_NA,  /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,  /**< NATIVE_INTEGER */
        DTL_TYPE_NA,  /**< NATIVE_BIGINT */

        DTL_TYPE_NA,  /**< NATIVE_REAL */
        DTL_TYPE_NA,  /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,  /**< FLOAT */
        DTL_TYPE_NA,  /**< NUMBER */
        DTL_TYPE_NA,  /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_DATE, /**< CHARACTER */
        DTL_TYPE_DATE, /**< CHARACTER VARYING */
        DTL_TYPE_DATE, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,  /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,  /**< BINARY */
        DTL_TYPE_NA,  /**< BINARY VARYING */
        DTL_TYPE_NA,  /**< BINARY LONG VARYING */
        DTL_TYPE_NA,  /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_DATE, /**< DATE */
        DTL_TYPE_NA,  /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,  /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA, /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,  /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,  /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,  /**< DTL_TYPE_ROWID */
    },

    /* TIME */
    {
        DTL_TYPE_NA,  /**< BOOLEAN */
        DTL_TYPE_NA,  /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,  /**< NATIVE_INTEGER */
        DTL_TYPE_NA,  /**< NATIVE_BIGINT */

        DTL_TYPE_NA,  /**< NATIVE_REAL */
        DTL_TYPE_NA,  /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,  /**< FLOAT */
        DTL_TYPE_NA,  /**< NUMBER */
        DTL_TYPE_NA,  /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_TIME, /**< CHARACTER */
        DTL_TYPE_TIME, /**< CHARACTER VARYING */
        DTL_TYPE_TIME, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,  /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,  /**< BINARY */
        DTL_TYPE_NA,  /**< BINARY VARYING */
        DTL_TYPE_NA,  /**< BINARY LONG VARYING */
        DTL_TYPE_NA,  /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,  /**< DATE */
        DTL_TYPE_TIME, /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,  /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,  /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,  /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,  /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,  /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,  /**< DTL_TYPE_ROWID */
    },

    /* TIMESTAMP */
    {
        DTL_TYPE_NA,       /**< BOOLEAN */
        DTL_TYPE_NA,       /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,       /**< NATIVE_INTEGER */
        DTL_TYPE_NA,       /**< NATIVE_BIGINT */

        DTL_TYPE_NA,       /**< NATIVE_REAL */
        DTL_TYPE_NA,       /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,       /**< FLOAT */
        DTL_TYPE_NA,       /**< NUMBER */
        DTL_TYPE_NA,       /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_TIMESTAMP, /**< CHARACTER */
        DTL_TYPE_TIMESTAMP, /**< CHARACTER VARYING */
        DTL_TYPE_TIMESTAMP, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,       /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,       /**< BINARY */
        DTL_TYPE_NA,       /**< BINARY VARYING */
        DTL_TYPE_NA,       /**< BINARY LONG VARYING */
        DTL_TYPE_NA,       /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_TIMESTAMP, /**< DATE */
        DTL_TYPE_NA,       /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_TIMESTAMP, /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,       /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,       /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,       /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,       /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,       /**< DTL_TYPE_ROWID */
    },

    /* TIME WITH TIME ZONE */
    {
        DTL_TYPE_NA,                 /**< BOOLEAN */
        DTL_TYPE_NA,                 /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,                 /**< NATIVE_INTEGER */
        DTL_TYPE_NA,                 /**< NATIVE_BIGINT */

        DTL_TYPE_NA,                 /**< NATIVE_REAL */
        DTL_TYPE_NA,                 /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,                 /**< FLOAT */
        DTL_TYPE_NA,                 /**< NUMBER */
        DTL_TYPE_NA,                 /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_TIME_WITH_TIME_ZONE, /**< CHARACTER */
        DTL_TYPE_TIME_WITH_TIME_ZONE, /**< CHARACTER VARYING */
        DTL_TYPE_TIME_WITH_TIME_ZONE, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,                 /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,                 /**< BINARY */
        DTL_TYPE_NA,                 /**< BINARY VARYING */
        DTL_TYPE_NA,                 /**< BINARY LONG VARYING */
        DTL_TYPE_NA,                 /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,                 /**< DATE */
        DTL_TYPE_TIME_WITH_TIME_ZONE, /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,                 /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_TIME_WITH_TIME_ZONE, /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,                 /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,                 /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,                 /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,                 /**< DTL_TYPE_ROWID */
    },

    /* TIMESTAMP WITH TIME ZONE */
    {
        DTL_TYPE_NA,                      /**< BOOLEAN */
        DTL_TYPE_NA,                      /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,                      /**< NATIVE_INTEGER */
        DTL_TYPE_NA,                      /**< NATIVE_BIGINT */

        DTL_TYPE_NA,                      /**< NATIVE_REAL */
        DTL_TYPE_NA,                      /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,                      /**< FLOAT */
        DTL_TYPE_NA,                      /**< NUMBER */
        DTL_TYPE_NA,                      /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, /**< CHARACTER */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, /**< CHARACTER VARYING */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,                      /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,                      /**< BINARY */
        DTL_TYPE_NA,                      /**< BINARY VARYING */
        DTL_TYPE_NA,                      /**< BINARY LONG VARYING */
        DTL_TYPE_NA,                      /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, /**< DATE */
        DTL_TYPE_NA,                      /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,                      /**< TIME WITH TIME ZONE */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,                      /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,                      /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,                      /**< DTL_TYPE_ROWID */
    },

    /* INTERVAL_YEAR_TO_MONTH */
    {
        DTL_TYPE_NA,                    /**< BOOLEAN */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH, /**< NATIVE_SMALLINT */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH, /**< NATIVE_INTEGER */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH, /**< NATIVE_BIGINT */

        DTL_TYPE_NA,                    /**< NATIVE_REAL */
        DTL_TYPE_NA,                    /**< NATIVE_DOUBLE */

        DTL_TYPE_INTERVAL_YEAR_TO_MONTH, /**< FLOAT */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH, /**< NUMBER */
        DTL_TYPE_NA,                    /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH, /**< CHARACTER */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH, /**< CHARACTER VARYING */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,                    /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,                    /**< BINARY */
        DTL_TYPE_NA,                    /**< BINARY VARYING */
        DTL_TYPE_NA,                    /**< BINARY LONG VARYING */
        DTL_TYPE_NA,                    /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,                    /**< DATE */
        DTL_TYPE_NA,                    /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,                    /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,                    /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,                    /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_INTERVAL_YEAR_TO_MONTH, /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,                    /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,                    /**< DTL_TYPE_ROWID */
    },

    /* INTERVAL_DAY_TO_SECOND */
    {
        DTL_TYPE_NA,                    /**< BOOLEAN */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /**< NATIVE_SMALLINT */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /**< NATIVE_INTEGER */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /**< NATIVE_BIGINT */

        DTL_TYPE_NA,                    /**< NATIVE_REAL */
        DTL_TYPE_NA,                    /**< NATIVE_DOUBLE */

        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /**< FLOAT */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /**< NUMBER */
        DTL_TYPE_NA,                    /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /**< CHARACTER */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /**< CHARACTER VARYING */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,                    /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,                    /**< BINARY */
        DTL_TYPE_NA,                    /**< BINARY VARYING */
        DTL_TYPE_NA,                    /**< BINARY LONG VARYING */
        DTL_TYPE_NA,                    /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,                    /**< DATE */
        DTL_TYPE_NA,                    /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,                    /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,                    /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,                    /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,                    /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_NA,                    /**< DTL_TYPE_ROWID */
    },

    /* ROW ID */
    {
        DTL_TYPE_NA,   /**< BOOLEAN */
        DTL_TYPE_NA,   /**< NATIVE_SMALLINT */
        DTL_TYPE_NA,   /**< NATIVE_INTEGER */
        DTL_TYPE_NA,   /**< NATIVE_BIGINT */

        DTL_TYPE_NA,   /**< NATIVE_REAL */
        DTL_TYPE_NA,   /**< NATIVE_DOUBLE */

        DTL_TYPE_NA,   /**< FLOAT */
        DTL_TYPE_NA,   /**< NUMBER */
        DTL_TYPE_NA,   /**< unsupported feature, DECIMAL */
    
        DTL_TYPE_ROWID, /**< CHARACTER */
        DTL_TYPE_ROWID, /**< CHARACTER VARYING */
        DTL_TYPE_ROWID, /**< CHARACTER LONG VARYING */
        DTL_TYPE_NA,   /**< unsupported feature, CHARACTER LARGE OBJECT */
    
        DTL_TYPE_NA,   /**< BINARY */
        DTL_TYPE_NA,   /**< BINARY VARYING */
        DTL_TYPE_NA,   /**< BINARY LONG VARYING */
        DTL_TYPE_NA,   /**< unsupported feature, BINARY LARGE OBJECT */

        DTL_TYPE_NA,   /**< DATE */
        DTL_TYPE_NA,   /**< TIME WITHOUT TIME ZONE */
        DTL_TYPE_NA,   /**< TIMESTAMP WITHOUT TIME ZONE */
        DTL_TYPE_NA,   /**< TIME WITH TIME ZONE */
        DTL_TYPE_NA,   /**< TIMESTAMP WITH TIME ZONE */

        DTL_TYPE_NA,   /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        DTL_TYPE_NA,   /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

        DTL_TYPE_ROWID, /**< DTL_TYPE_ROWID */
    },

};


/**
 * @brief Comparison Result DataType Combination ( greatest(), least() ... )
 */
const dtlDataType dtlComparisonResultTypeCombination[DTL_TYPE_MAX][DTL_TYPE_MAX] =
{
    /* DTL_TYPE_BOOLEAN */
    {
        DTL_TYPE_BOOLEAN, /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_BOOLEAN,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_BOOLEAN,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_BOOLEAN,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NATIVE_SMALLINT,   */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NATIVE_SMALLINT, /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NATIVE_INTEGER,  /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NATIVE_BIGINT,   /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,   /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,   /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NUMBER,        /* DTL_TYPE_FLOAT,           */
        DTL_TYPE_NUMBER,        /* DTL_TYPE_NUMBER,          */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NUMBER,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NATIVE_INTEGER,    */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NATIVE_INTEGER,   /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NATIVE_INTEGER,   /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NATIVE_BIGINT,    /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NUMBER,        /* DTL_TYPE_FLOAT,           */
        DTL_TYPE_NUMBER,        /* DTL_TYPE_NUMBER,          */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NUMBER,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NATIVE_BIGINT,     */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NATIVE_BIGINT,  /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NATIVE_BIGINT,  /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NATIVE_BIGINT,  /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NUMBER,        /* DTL_TYPE_FLOAT,           */
        DTL_TYPE_NUMBER,        /* DTL_TYPE_NUMBER,          */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NUMBER,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NATIVE_REAL,       */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_REAL,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_FLOAT              */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NUMBER ,           */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NATIVE_DOUBLE,     */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_FLOAT */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_FLOAT,   /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NUMBER,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,          /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NUMBER */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NUMBER,  /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NUMBER,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_DECIMAL  */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */        
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_CHAR */
    {
        DTL_TYPE_BOOLEAN,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NUMBER,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_CHAR,         /* DTL_TYPE_CHAR,           */
        DTL_TYPE_VARCHAR,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_LONGVARCHAR,           /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,           /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_DATE,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_TIME,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_TIMESTAMP,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_TIME_WITH_TIME_ZONE,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_ROWID       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_VARCHAR */
    {
        DTL_TYPE_BOOLEAN,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NUMBER,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_VARCHAR,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_VARCHAR,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_LONGVARCHAR,           /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,           /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_DATE,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_TIME,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_TIMESTAMP,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_TIME_WITH_TIME_ZONE,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_ROWID       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_LONGVARCHAR */
    {
        DTL_TYPE_BOOLEAN,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NUMBER,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NUMBER,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_LONGVARCHAR,           /* DTL_TYPE_CHAR,           */
        DTL_TYPE_LONGVARCHAR,           /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_LONGVARCHAR,  /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,           /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_DATE,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_TIME,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_TIMESTAMP,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_TIME_WITH_TIME_ZONE,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_ROWID       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_CLOB */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_BINARY */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_BINARY,         /* DTL_TYPE_BINARY,         */
        DTL_TYPE_VARBINARY,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_LONGVARBINARY,             /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,             /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_VARBINARY */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_VARBINARY,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_VARBINARY,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_LONGVARBINARY,  /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,             /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_LONGVARBINARY */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_LONGVARBINARY,              /* DTL_TYPE_BINARY,         */
        DTL_TYPE_LONGVARBINARY,              /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_LONGVARBINARY,   /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,              /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_BLOB */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_DATE */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_DATE,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_DATE,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_DATE,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_DATE,                      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME,                      */
        DTL_TYPE_TIMESTAMP,                 /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_TIME */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_TIME,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_TIME,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_TIME,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,                    /* DTL_TYPE_DATE,                      */
        DTL_TYPE_TIME,                  /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,                    /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_TIME_WITH_TIME_ZONE,   /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_TIMESTAMP */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_TIMESTAMP,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_TIMESTAMP,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_TIMESTAMP,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_TIMESTAMP,                 /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME,                      */
        DTL_TYPE_TIMESTAMP,                 /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_TIME_WITH_TIME_ZONE,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_TIME_WITH_TIME_ZONE,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_TIME_WITH_TIME_ZONE,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,                    /* DTL_TYPE_DATE,                      */
        DTL_TYPE_TIME_WITH_TIME_ZONE,   /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,                    /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_TIME_WITH_TIME_ZONE,   /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME,                      */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,  /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,                      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_ROWID */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_ROWID,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_ROWID,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_ROWID,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_ROWID      /* DTL_TYPE_ROWID,                     */
    }
};


/**
 * @brief 표준문서 9.3 Result of data type combinations 참조.
 */
const dtlDataType dtlResultTypeCombination[DTL_TYPE_MAX][DTL_TYPE_MAX] =
{
    /* DTL_TYPE_BOOLEAN */
    {
        DTL_TYPE_BOOLEAN, /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NATIVE_SMALLINT,   */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NATIVE_SMALLINT, /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NATIVE_INTEGER,  /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NATIVE_BIGINT,   /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,   /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,   /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NUMBER,        /* DTL_TYPE_FLOAT,           */
        DTL_TYPE_NUMBER,        /* DTL_TYPE_NUMBER,          */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NATIVE_INTEGER,    */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NATIVE_INTEGER,   /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NATIVE_INTEGER,   /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NATIVE_BIGINT,    /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NUMBER,        /* DTL_TYPE_FLOAT,           */
        DTL_TYPE_NUMBER,        /* DTL_TYPE_NUMBER,          */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NATIVE_BIGINT,     */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NATIVE_BIGINT,  /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NATIVE_BIGINT,  /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NATIVE_BIGINT,  /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NUMBER,        /* DTL_TYPE_FLOAT,           */
        DTL_TYPE_NUMBER,        /* DTL_TYPE_NUMBER,          */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NATIVE_REAL,       */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_REAL,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_FLOAT              */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NUMBER ,           */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NATIVE_DOUBLE,     */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_FLOAT */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_FLOAT,   /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_NUMBER */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NATIVE_DOUBLE,    /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NUMBER,  /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NUMBER,  /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_DECIMAL  */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */        
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_CHAR */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_CHAR,         /* DTL_TYPE_CHAR,           */
        DTL_TYPE_VARCHAR,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_LONGVARCHAR,  /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,           /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_VARCHAR */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_VARCHAR,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_VARCHAR,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_LONGVARCHAR,  /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,           /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_LONGVARCHAR */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_LONGVARCHAR,  /* DTL_TYPE_CHAR,           */
        DTL_TYPE_LONGVARCHAR,  /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_LONGVARCHAR,  /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,           /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_CLOB */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_BINARY */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_BINARY,         /* DTL_TYPE_BINARY,         */
        DTL_TYPE_VARBINARY,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_LONGVARBINARY,  /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,             /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_VARBINARY */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_VARBINARY,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_VARBINARY,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_LONGVARBINARY,  /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,             /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_LONGVARBINARY */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_LONGVARBINARY,   /* DTL_TYPE_BINARY,         */
        DTL_TYPE_LONGVARBINARY,   /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_LONGVARBINARY,   /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,              /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_BLOB */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_DATE */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_DATE,                      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME,                      */
        DTL_TYPE_TIMESTAMP,                 /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_TIME */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,                    /* DTL_TYPE_DATE,                      */
        DTL_TYPE_TIME,                  /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,                    /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_TIME_WITH_TIME_ZONE,   /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_TIMESTAMP */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_TIMESTAMP,                 /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME,                      */
        DTL_TYPE_TIMESTAMP,                 /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,                    /* DTL_TYPE_DATE,                      */
        DTL_TYPE_TIME_WITH_TIME_ZONE,   /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,                    /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_TIME_WITH_TIME_ZONE,   /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME,                      */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,                        /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_INTERVAL_YEAR_TO_MONTH,  /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,                      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_INTERVAL_DAY_TO_SECOND, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_NA       /* DTL_TYPE_ROWID,                     */
    },
    /* DTL_TYPE_ROWID */
    {
        DTL_TYPE_NA,      /* DTL_TYPE_BOOLEAN, */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_SMALLINT,   */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_INTEGER,    */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_BIGINT,     */
        
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_REAL,       */
        DTL_TYPE_NA,      /* DTL_TYPE_NATIVE_DOUBLE,     */

        DTL_TYPE_NA,      /* DTL_TYPE_FLOAT,             */
        DTL_TYPE_NA,      /* DTL_TYPE_NUMBER,            */
        DTL_TYPE_NA,      /* DTL_TYPE_DECIMAL,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_CHAR,           */
        DTL_TYPE_NA,      /* DTL_TYPE_VARCHAR,        */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARCHAR,    */
        DTL_TYPE_NA,      /* DTL_TYPE_CLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_BINARY,         */
        DTL_TYPE_NA,      /* DTL_TYPE_VARBINARY,      */
        DTL_TYPE_NA,      /* DTL_TYPE_LONGVARBINARY,  */
        DTL_TYPE_NA,      /* DTL_TYPE_BLOB,           */
        
        DTL_TYPE_NA,      /* DTL_TYPE_DATE,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME,                      */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP,                 */
        DTL_TYPE_NA,      /* DTL_TYPE_TIME_WITH_TIME_ZONE,       */
        DTL_TYPE_NA,      /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  */
        
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH,    */
        DTL_TYPE_NA,      /* DTL_TYPE_INTERVAL_DAY_TO_SECOND,    */
        
        DTL_TYPE_ROWID      /* DTL_TYPE_ROWID,                     */
    }
};


/**
 * @brief 두개의 interval indicator의 범위가 포함되는 interval indicator 반환
 *        ( 현재 interval 간의 add, interval 간의 minus 연산에서 사용됨. )
 */

const dtlIntervalIndicator
dtlIntervalIndicatorContainArgIndicator[DTL_INTERVAL_INDICATOR_MAX][DTL_INTERVAL_INDICATOR_MAX] =
{
    /* DTL_INTERVAL_INDICATOR_NA */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_NA  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },
    /* DTL_INTERVAL_INDICATOR_YEAR */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_YEAR, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_NA  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },    
    /* DTL_INTERVAL_INDICATOR_MONTH */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_MONTH, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_NA  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },    
    /* DTL_INTERVAL_INDICATOR_DAY */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_DAY, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_DAY_TO_HOUR, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_DAY_TO_HOUR, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },
    /* DTL_INTERVAL_INDICATOR_HOUR */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_DAY_TO_HOUR, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_HOUR, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_DAY_TO_HOUR, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },
    /* DTL_INTERVAL_INDICATOR_MINUTE */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_MINUTE, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },
    /* DTL_INTERVAL_INDICATOR_SECOND */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_SECOND, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },
    /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_NA  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },
    /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_DAY_TO_HOUR, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_DAY_TO_HOUR, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_DAY_TO_HOUR, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },
    /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },
    /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },
    /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },
    /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND , /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    },    
    /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */
    {
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_NA */
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR */             
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_MONTH */            
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY */              
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR */             
        DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND, /* DTL_INTERVAL_INDICATOR_MINUTE */           
        DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND, /* DTL_INTERVAL_INDICATOR_SECOND */           
        DTL_INTERVAL_INDICATOR_NA, /* DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_HOUR */      
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE */    
        DTL_INTERVAL_INDICATOR_DAY_TO_SECOND, /* DTL_INTERVAL_INDICATOR_DAY_TO_SECOND */    
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE */   
        DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND, /* DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND */   
        DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND  /* DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND */ 
    }
};

const dtlGroup dtlConversionGroup[DTL_GROUP_MAX][DTL_GROUP_MAX] =
{
    /* DTL_GROUP_BOOLEAN */
    { DTL_GROUP_BOOLEAN,                   /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_NUMBER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_REAL */    
      DTL_GROUP_BOOLEAN,                   /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_BOOLEAN,                   /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */
    
    /* DTL_GROUP_BINARY_INTEGER */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_BINARY_INTEGER,            /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_NUMBER,                    /* DTL_GROUP_NUMBER */
      DTL_GROUP_BINARY_REAL,               /* DTL_GROUP_BINARY_REAL */            
      DTL_GROUP_NUMBER,                    /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_NUMBER,                    /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_INTERVAL_YEAR_TO_MONTH,    /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_INTERVAL_DAY_TO_SECOND,    /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */

    /* DTL_GROUP_NUMBER */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_NUMBER,                    /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_NUMBER,                    /* DTL_GROUP_NUMBER */
      DTL_GROUP_BINARY_REAL,               /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_NUMBER,                    /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_NUMBER,                    /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_INTERVAL_YEAR_TO_MONTH,    /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_INTERVAL_DAY_TO_SECOND,    /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */    

    /* DTL_GROUP_BINARY_REAL */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_BINARY_REAL,               /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_BINARY_REAL,               /* DTL_GROUP_NUMBER */
      DTL_GROUP_BINARY_REAL,               /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_BINARY_REAL,               /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_BINARY_REAL,               /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */

    /* DTL_GROUP_CHAR_STRING */
    { DTL_GROUP_BOOLEAN,                   /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_NUMBER,                    /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_NUMBER,                    /* DTL_GROUP_NUMBER */
      DTL_GROUP_BINARY_REAL,               /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_CHAR_STRING,               /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_LONGVARCHAR_STRING,        /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_BINARY_STRING,             /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_LONGVARBINARY_STRING,      /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_DATE,                      /* DTL_GROUP_DATE */
      DTL_GROUP_TIME,                      /* DTL_GROUP_TIME */
      DTL_GROUP_TIMESTAMP,                 /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_TIME_WITH_TIME_ZONE,       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_INTERVAL_YEAR_TO_MONTH,    /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_INTERVAL_DAY_TO_SECOND,    /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_ROWID },                   /* DTL_GROUP_ROWID */      

    /* DTL_GROUP_LONGVARCHAR_STRING */
    { DTL_GROUP_BOOLEAN,                   /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_NUMBER,                    /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_NUMBER,                    /* DTL_GROUP_NUMBER */
      DTL_GROUP_BINARY_REAL,               /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_LONGVARCHAR_STRING,        /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_LONGVARCHAR_STRING,        /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_DATE,                      /* DTL_GROUP_DATE */
      DTL_GROUP_TIME,                      /* DTL_GROUP_TIME */
      DTL_GROUP_TIMESTAMP,                 /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_TIME_WITH_TIME_ZONE,       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_INTERVAL_YEAR_TO_MONTH,    /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_INTERVAL_DAY_TO_SECOND,    /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_ROWID },                   /* DTL_GROUP_ROWID */      

    /* DTL_GROUP_BINARY_STRING */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_NUMBER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_BINARY_STRING,             /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_BINARY_STRING,             /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_LONGVARBINARY_STRING,      /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */      

    /* DTL_GROUP_LONGVARBINARY_STRING */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_NUMBER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_LONGVARBINARY_STRING,      /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_LONGVARBINARY_STRING,      /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_LONGVARBINARY_STRING,      /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */      

    /* DTL_GROUP_DATE */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_NUMBER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_DATE,                      /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_DATE,                      /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_DATE,                      /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_TIMESTAMP,                 /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */      

    /* DTL_GROUP_TIME */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_NUMBER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_TIME,                      /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_TIME,                      /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_DATE */
      DTL_GROUP_TIME,                      /* DTL_GROUP_TIME */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_TIME_WITH_TIME_ZONE,       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */      

    /* DTL_GROUP_TIMESTAMP */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_NUMBER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_TIMESTAMP,                 /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_TIMESTAMP,                 /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_TIMESTAMP,                 /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_TIMESTAMP,                 /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */             

    /* DTL_GROUP_TIME_WITH_TIME_ZONE */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_NUMBER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_TIME_WITH_TIME_ZONE,       /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_TIME_WITH_TIME_ZONE,       /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_DATE */
      DTL_GROUP_TIME_WITH_TIME_ZONE,       /* DTL_GROUP_TIME */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_TIME_WITH_TIME_ZONE,       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */            

    /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_NUMBER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,  /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */            

    /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_INTERVAL_YEAR_TO_MONTH,    /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_INTERVAL_YEAR_TO_MONTH,    /* DTL_GROUP_NUMBER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_INTERVAL_YEAR_TO_MONTH,    /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_INTERVAL_YEAR_TO_MONTH,    /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_INTERVAL_YEAR_TO_MONTH,    /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */            

    /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_INTERVAL_DAY_TO_SECOND,    /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_INTERVAL_DAY_TO_SECOND,    /* DTL_GROUP_NUMBER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_INTERVAL_DAY_TO_SECOND,    /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_INTERVAL_DAY_TO_SECOND,    /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_INTERVAL_DAY_TO_SECOND,    /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_MAX },                     /* DTL_GROUP_ROWID */

    /* DTL_GROUP_ROWID */
    { DTL_GROUP_MAX,                       /* DTL_GROUP_BOOLEAN */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_INTEGER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_NUMBER */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_REAL */
      DTL_GROUP_ROWID,                     /* DTL_GROUP_CHAR_STRING */
      DTL_GROUP_ROWID,                     /* DTL_GROUP_LONGVARCHAR_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_BINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_LONGVARBINARY_STRING */
      DTL_GROUP_MAX,                       /* DTL_GROUP_DATE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIME_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_YEAR_TO_MONTH */
      DTL_GROUP_MAX,                       /* DTL_GROUP_INTERVAL_DAY_TO_SECOND */
      DTL_GROUP_ROWID }                    /* DTL_GROUP_ROWID */            
    
};


/** @} dtlDataTypeInformation */

const dtlDateToken dtlDateTokenTbl[] =
{
    /* mToken, mType, mValue */
    { DTL_EARLY,       DTL_RESERV,     DTL_DTK_EARLY }, /* "-infinity" reserved for "early time"*/
    { DTL_DA_D,        DTL_ADBC,       DTL_AD },          /* "ad" for years > 0 */
    { "allballs",      DTL_RESERV,     DTL_DTK_ZULU },    /* 00:00:00 */
    { "am",            DTL_AMPM,       DTL_AM },
    { "apr",           DTL_MONTH,      4 },
    { "april",         DTL_MONTH,      4 },
    { "at",            DTL_IGNORE_DTF, 0 },	              /* "at" (throwaway) */
    { "aug",           DTL_MONTH,      8 },
    { "august",        DTL_MONTH,      8 },
    { DTL_DB_C,        DTL_ADBC,       DTL_BC },          /* "bc" for years <= 0 */
    { "d",             DTL_UNITS,      DTL_DTK_DAY },     /* "day of month" for ISO input */
    { "dec",           DTL_MONTH,      12 },
    { "december",      DTL_MONTH,      12 },
    { "dow",           DTL_RESERV,     DTL_DTK_DOW },     /* day of week */
    { "doy",           DTL_RESERV,     DTL_DTK_DOY },     /* day of year */
    { "dst",           DTL_DTZMOD,     6 },
    { DTL_EPOCH,       DTL_RESERV,     DTL_DTK_EPOCH }, /*"epoch" reserved for system epoch time*/
    { "feb",           DTL_MONTH,      2 },
    { "february",      DTL_MONTH,      2 },
    { "fri",           DTL_DOW,        5 },
    { "friday",        DTL_DOW,        5 },
    { "h",             DTL_UNITS,      DTL_DTK_HOUR },    /* "hour" */
    { DTL_LATE,        DTL_RESERV,     DTL_DTK_LATE },    /* "infinity" reserved for "late time" */
    { "isodow",        DTL_RESERV,     DTL_DTK_ISODOW },  /* ISO day of week, Sunday == 7 */
    { "isoyear",       DTL_UNITS,      DTL_DTK_ISOYEAR }, /* year in terms of the ISO week date */
    { "j",             DTL_UNITS,      DTL_DTK_JULIAN },
    { "jan",           DTL_MONTH,      1 },
    { "january",       DTL_MONTH,      1 },
    { "jd",            DTL_UNITS,      DTL_DTK_JULIAN },
    { "jul",           DTL_MONTH,      7 },
    { "julian",        DTL_UNITS,      DTL_DTK_JULIAN },
    { "july",          DTL_MONTH,      7 },
    { "jun",           DTL_MONTH,      6 },
    { "june",          DTL_MONTH,      6 },
    { "m",             DTL_UNITS,      DTL_DTK_MONTH },   /* "month" for ISO input */
    { "mar",           DTL_MONTH,      3 },
    { "march",         DTL_MONTH,      3 },
    { "may",           DTL_MONTH,      5 },
    { "mm",            DTL_UNITS,      DTL_DTK_MINUTE },  /* "minute" for ISO input */
    { "mon",           DTL_DOW,        1 },
    { "monday",        DTL_DOW,        1 },
    { "nov",           DTL_MONTH,      11 },
    { "november",      DTL_MONTH,      11 },
    { DTL_NOW,         DTL_RESERV,     DTL_DTK_NOW },     /* current transaction time */
    { "oct",           DTL_MONTH,      10 },
    { "october",       DTL_MONTH,      10 },
    { "on",            DTL_IGNORE_DTF, 0 },               /* "on" (throwaway) */
    { "pm",            DTL_AMPM,       DTL_PM },
    { "s",             DTL_UNITS,      DTL_DTK_SECOND },  /* "seconds" for ISO input */
    { "sat",           DTL_DOW,        6 },
    { "saturday",      DTL_DOW,        6 },
    { "sep",           DTL_MONTH,      9 },
    { "sept",          DTL_MONTH,      9 },
    { "september",     DTL_MONTH,      9 },
    { "sun",           DTL_DOW,        0 },
    { "sunday",        DTL_DOW,        0 },
    { "t",             DTL_ISOTIME,    DTL_DTK_TIME },    /* Filler for ISO time fields */
    { "thu",           DTL_DOW,        4 },
    { "thur",          DTL_DOW,        4 },
    { "thurs",         DTL_DOW,        4 },
    { "thursday",      DTL_DOW,        4 },
    { DTL_TODAY,       DTL_RESERV,     DTL_DTK_TODAY },    /* midnight */
    { DTL_TOMORROW,    DTL_RESERV,     DTL_DTK_TOMORROW }, /* tomorrow midnight */
    { "tue",           DTL_DOW,        2 },
    { "tues",          DTL_DOW,        2 },
    { "tuesday",       DTL_DOW,        2 },
    { "wed",           DTL_DOW,        3 },
    { "wednesday",     DTL_DOW,        3 },
    { "weds",          DTL_DOW,        3 },
    { "y",             DTL_UNITS,      DTL_DTK_YEAR },     /* "year" for ISO input */
    { DTL_YESTERDAY,   DTL_RESERV,     DTL_DTK_YESTERDAY } /* yesterday midnight */
};

const stlInt32 dtlDateTokenTblSize =
    ( STL_SIZEOF(dtlDateTokenTbl) / STL_SIZEOF(dtlDateTokenTbl[0]) );

const dtlDateToken * dtlDateTokenCache[DTL_MAX_DATE_FIELDS] = {NULL};

const dtlDateToken dtlDateDeltaTokenTbl[] = {
    /* text, token, lexval */
    { "@",             DTL_IGNORE_DTF, 0 },                  /* postgres relative prefix */
    { DTL_DAGO,        DTL_AGO,        0 },               /* "ago" indicates negative time offset */
    { "c",             DTL_UNITS,      DTL_DTK_CENTURY },    /* "century" relative */
    { "cent",          DTL_UNITS,      DTL_DTK_CENTURY },    /* "century" relative */
    { "centuries",     DTL_UNITS,      DTL_DTK_CENTURY },    /* "centuries" relative */
    { DTL_DCENTURY,    DTL_UNITS,      DTL_DTK_CENTURY },    /* "century" relative */
    { "d",             DTL_UNITS,      DTL_DTK_DAY },        /* "day" relative */
    { DTL_DDAY,        DTL_UNITS,      DTL_DTK_DAY },        /* "day" relative */
    { "days",          DTL_UNITS,      DTL_DTK_DAY },        /* "days" relative */
    { "dec",           DTL_UNITS,      DTL_DTK_DECADE },     /* "decade" relative */
    { DTL_DDECADE,     DTL_UNITS,      DTL_DTK_DECADE },     /* "decade" relative */
    { "decades",       DTL_UNITS,      DTL_DTK_DECADE },     /* "decades" relative */
    { "decs",          DTL_UNITS,      DTL_DTK_DECADE },     /* "decades" relative */
    { "h",             DTL_UNITS,      DTL_DTK_HOUR },       /* "hour" relative */
    { DTL_DHOUR,       DTL_UNITS,      DTL_DTK_HOUR },       /* "hour" relative */
    { "hours",         DTL_UNITS,      DTL_DTK_HOUR },       /* "hours" relative */
    { "hr",            DTL_UNITS,      DTL_DTK_HOUR },       /* "hour" relative */
    { "hrs",           DTL_UNITS,      DTL_DTK_HOUR },       /* "hours" relative */
    { "m",             DTL_UNITS,      DTL_DTK_MINUTE },     /* "minute" relative */
    { "microsecond",   DTL_UNITS,      DTL_DTK_MICROSEC },   /* "microsecond" relative */
    { "mil",           DTL_UNITS,      DTL_DTK_MILLENNIUM }, /* "millennium" relative */
    { "millennia",     DTL_UNITS,      DTL_DTK_MILLENNIUM }, /* "millennia" relative */
    { DTL_DMILLENNIUM, DTL_UNITS,      DTL_DTK_MILLENNIUM }, /* "millennium" relative */
    { "millisecond",   DTL_UNITS,      DTL_DTK_MILLISEC },   /* relative */
    { "mils",          DTL_UNITS,      DTL_DTK_MILLENNIUM }, /* "millennia" relative */
    { "min",           DTL_UNITS,      DTL_DTK_MINUTE },     /* "minute" relative */
    { "mins",          DTL_UNITS,      DTL_DTK_MINUTE },     /* "minutes" relative */
    { DTL_DMINUTE,     DTL_UNITS,      DTL_DTK_MINUTE },     /* "minute" relative */
    { "minutes",       DTL_UNITS,      DTL_DTK_MINUTE },     /* "minutes" relative */
    { "mon",           DTL_UNITS,      DTL_DTK_MONTH },      /* "months" relative */
    { "mons",          DTL_UNITS,      DTL_DTK_MONTH },      /* "months" relative */
    { DTL_DMONTH,      DTL_UNITS,      DTL_DTK_MONTH },      /* "month" relative */
    { "months",        DTL_UNITS,      DTL_DTK_MONTH },
    { "ms",            DTL_UNITS,      DTL_DTK_MILLISEC },
    { "msec",          DTL_UNITS,      DTL_DTK_MILLISEC },
    { DTL_DMILLISEC,   DTL_UNITS,      DTL_DTK_MILLISEC },
    { "mseconds",      DTL_UNITS,      DTL_DTK_MILLISEC },
    { "msecs",         DTL_UNITS,      DTL_DTK_MILLISEC },
    { "qtr",           DTL_UNITS,      DTL_DTK_QUARTER },    /* "quarter" relative */
    { DTL_DQUARTER,    DTL_UNITS,      DTL_DTK_QUARTER },    /* "quarter" relative */
    { "s",             DTL_UNITS,      DTL_DTK_SECOND },
    { "sec",           DTL_UNITS,      DTL_DTK_SECOND },
    { DTL_DSECOND,     DTL_UNITS,      DTL_DTK_SECOND },
    { "seconds",       DTL_UNITS,      DTL_DTK_SECOND },
    { "secs",          DTL_UNITS,      DTL_DTK_SECOND },
    { DTL_DTIMEZONE,   DTL_UNITS,      DTL_DTK_TZ },        /* "timezone" time offset */
    { "timezone_h",    DTL_UNITS,      DTL_DTK_TZ_HOUR },    /* timezone hour units */
    { "timezone_m",    DTL_UNITS,      DTL_DTK_TZ_MINUTE },  /* timezone minutes units */
    { "us",            DTL_UNITS,      DTL_DTK_MICROSEC },   /* "microsecond" relative */
    { "usec",          DTL_UNITS,      DTL_DTK_MICROSEC },   /* "microsecond" relative */
    { DTL_DMICROSEC,   DTL_UNITS,      DTL_DTK_MICROSEC },   /* "microsecond" relative */
    { "useconds",      DTL_UNITS,      DTL_DTK_MICROSEC },   /* "microseconds" relative */
    { "usecs",         DTL_UNITS,      DTL_DTK_MICROSEC },   /* "microseconds" relative */
    { "w",             DTL_UNITS,      DTL_DTK_WEEK },       /* "week" relative */
    { DTL_DWEEK,       DTL_UNITS,      DTL_DTK_WEEK },       /* "week" relative */
    { "weeks",         DTL_UNITS,      DTL_DTK_WEEK },       /* "weeks" relative */
    { "y",             DTL_UNITS,      DTL_DTK_YEAR },       /* "year" relative */
    { DTL_DYEAR,       DTL_UNITS,      DTL_DTK_YEAR },       /* "year" relative */
    { "years",         DTL_UNITS,      DTL_DTK_YEAR },       /* "years" relative */
    { "yr",            DTL_UNITS,      DTL_DTK_YEAR },       /* "year" relative */
    { "yrs",           DTL_UNITS,      DTL_DTK_YEAR }        /* "years" relative */
};

const stlInt32 dtlDateDeltaTokenTblSize = 
    ( STL_SIZEOF(dtlDateDeltaTokenTbl) / STL_SIZEOF(dtlDateDeltaTokenTbl[0]) );

const dtlDateToken * dtlDateDeltaCache[DTL_MAX_DATE_FIELDS] = {NULL};

/**
 * 윤년 체크 포함
 */
const stlInt32 dtlDayTab[2][13] =
{
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0}
};

const stlUInt16 dtlSumDayOfYear[2][13] =
{
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};


/**
 * @brief Char Legnth Unit 이름에 해당하는 dtlStringLengthUnit 반환
 * @param[in]  aName              char length unit 이름
 * @param[out] aCharLengthUnit    Name 해당하는 dtlStringLengthUnit
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtlGetCharLengthUnit( stlChar             * aName,
                                dtlStringLengthUnit * aCharLengthUnit,
                                stlErrorStack       * aErrorStack )
{
    const dtlLengthUnitMap * sMap;
    dtlStringLengthUnit      sLengthUnit = DTL_STRING_LENGTH_UNIT_NA;

    STL_DASSERT( aName != NULL );
    STL_DASSERT( aCharLengthUnit != NULL );
    
    for( sMap = gDtlLengthUnitMap; sMap->mName != NULL; sMap++ )
    {
        if( stlStrcasecmp( aName, sMap->mName ) == 0 )
        {
            sLengthUnit = sMap->mLengthUnit;
            break;
        }
    }

    STL_TRY_THROW( (sLengthUnit == DTL_STRING_LENGTH_UNIT_OCTETS) ||
                   (sLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS),
                   RAMP_ERR_INVALID_LENGTH_UNIT_NAME );

    *aCharLengthUnit = sLengthUnit;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_LENGTH_UNIT_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHAR_LENGTH_UNITS,
                      NULL,
                      aErrorStack,
                      aName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 고정 길이형 데이타 타입인지 판단
 * @param[in]     aType        DB Type
 * @param[out]    aIsStatic    고정 길이 타입 여부 
 * @param[out]    aErrorStack  에러 스택
 */
stlStatus dtlIsStaticType( dtlDataType       aType,
                           stlBool         * aIsStatic,
                           stlErrorStack   * aErrorStack )
{
    stlInt32   i = 0;
    stlBool    sIsStatic = STL_FALSE;

    for( i = 0 ; i < DTL_STATIC_TYPE_COUNT ; i++ )
    {
        if( dtlStaticTypes[i] == aType )
        {
            sIsStatic = STL_TRUE;
            break;
        }
        else
        {
            /* Do Nothing */
        }
    }

    *aIsStatic = sIsStatic;
        
    return STL_SUCCESS;        
}


/**
 * @brief DB Type 에 따른 출력에 필요한 길이값 계산
 * @param[in]  aType       DB Type
 * @param[in]  aPrecision  Precision
 * @param[in]  aScale      Scale
 * @param[in]  aIntervalIndicator   interval indicator
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @remarks
 */
stlInt32  dtlGetDisplaySize( dtlDataType            aType,
                             stlInt64               aPrecision,
                             stlInt64               aScale,
                             dtlIntervalIndicator   aIntervalIndicator,
                             dtlFuncVector        * aVectorFunc,
                             void                 * aVectorArg )
{
    stlInt32 sDisplaySize = 0;

    switch( aType )
    {
        case DTL_TYPE_BOOLEAN:
            sDisplaySize = DTL_BOOLEAN_DISPLAY_SIZE;
            break;
        case DTL_TYPE_NATIVE_SMALLINT:
            sDisplaySize = DTL_NATIVE_SMALLINT_DISPLAY_SIZE;
            break;
        case DTL_TYPE_NATIVE_INTEGER:
            sDisplaySize = DTL_NATIVE_INTEGER_DISPLAY_SIZE;
            break;
        case DTL_TYPE_NATIVE_BIGINT:
            sDisplaySize = DTL_NATIVE_BIGINT_DISPLAY_SIZE;
            break;

        case DTL_TYPE_NATIVE_REAL:
            sDisplaySize = DTL_NATIVE_REAL_DISPLAY_SIZE;
            break;
        case DTL_TYPE_NATIVE_DOUBLE:
            sDisplaySize = DTL_NATIVE_DOUBLE_DISPLAY_SIZE;
            break;

        case DTL_TYPE_FLOAT:
            sDisplaySize = DTL_FLOAT_DISPLAY_SIZE(aPrecision);
            break;
        case DTL_TYPE_NUMBER:
            sDisplaySize = DTL_NUMERIC_DISPLAY_SIZE(aPrecision);
            break;
        case DTL_TYPE_DECIMAL:
            /** @note NOT IMPLEMENTED */
            sDisplaySize = 0;
            break;
    
        case DTL_TYPE_CHAR:
            sDisplaySize = DTL_CHAR_DISPLAY_SIZE(aPrecision, aVectorFunc, aVectorArg);
            break;
        case DTL_TYPE_VARCHAR:
            sDisplaySize = DTL_VARCHAR_DISPLAY_SIZE(aPrecision, aVectorFunc, aVectorArg);
            break;
        case DTL_TYPE_LONGVARCHAR:
            sDisplaySize = DTL_NO_TOTAL_SIZE;
            break;
        case DTL_TYPE_CLOB:
            /** @note NOT IMPLEMENTED */
            sDisplaySize = 0;
            break;
    
        case DTL_TYPE_BINARY:
            sDisplaySize = DTL_BINARY_DISPLAY_SIZE(aPrecision);
            break;
        case DTL_TYPE_VARBINARY:
            sDisplaySize = DTL_VARBINARY_DISPLAY_SIZE(aPrecision);
            break;
        case DTL_TYPE_LONGVARBINARY:
            sDisplaySize = DTL_NO_TOTAL_SIZE;
            break;
        case DTL_TYPE_BLOB:
            /** @note NOT IMPLEMENTED */
            sDisplaySize = 0;
            break;

        case DTL_TYPE_DATE:
            sDisplaySize = (aVectorFunc->mGetDateFormatInfoFunc( aVectorArg ))->mToCharMaxResultLen;
            break;
        case DTL_TYPE_TIME:
            sDisplaySize = (aVectorFunc->mGetTimeFormatInfoFunc( aVectorArg ))->mToCharMaxResultLen;
            break;
        case DTL_TYPE_TIMESTAMP:
            sDisplaySize = (aVectorFunc->mGetTimestampFormatInfoFunc( aVectorArg ))->mToCharMaxResultLen;
            break;
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            sDisplaySize = (aVectorFunc->mGetTimeWithTimeZoneFormatInfoFunc( aVectorArg ))->mToCharMaxResultLen;
            break;
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            sDisplaySize = (aVectorFunc->mGetTimestampWithTimeZoneFormatInfoFunc( aVectorArg ))->mToCharMaxResultLen;
            break;

        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            sDisplaySize = DTL_INTERVAL_YEAR_TO_MONTH_DISPLAY_SIZE( aPrecision );
            break;
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            sDisplaySize = DTL_INTERVAL_DAY_TO_SECOND_DISPLAY_SIZE( aPrecision, aScale, aIntervalIndicator );
            break;
        case DTL_TYPE_ROWID:
            sDisplaySize = DTL_ROWID_DISPLAY_SIZE;
            break;
        default:
            sDisplaySize = 0;
            break;
    }
    
    return sDisplaySize;
}


/*****************************************************************
 * Wrapping Functions (for Conversion)
 *****************************************************************/


/**
 * Set Value From String
 */

stlStatus dtlBooleanSetValueFromString( stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        dtlIntervalIndicator  aIntervalIndicator,
                                        stlInt64              aAvailableSize,
                                        dtlDataValue        * aValue,
                                        stlBool             * aSuccessWithInfo,
                                        dtlFuncVector       * aSourceVectorFunc,
                                        void                * aSourceVectorArg,
                                        dtlFuncVector       * aDestVectorFunc,
                                        void                * aDestVectorArg,
                                        stlErrorStack       * aErrorStack )
{
    return dtdBooleanSetValueFromString( aString,
                                         aStringLength,
                                         aPrecision,
                                         aScale,
                                         aStringUnit,
                                         aIntervalIndicator,
                                         aAvailableSize,
                                         aValue,
                                         aSuccessWithInfo,
                                         aSourceVectorFunc,
                                         aSourceVectorArg,
                                         aDestVectorFunc,
                                         aDestVectorArg,
                                         aErrorStack );
}

stlStatus dtlSmallIntSetValueFromString( stlChar             * aString,
                                         stlInt64              aStringLength,
                                         stlInt64              aPrecision,
                                         stlInt64              aScale,
                                         dtlStringLengthUnit   aStringUnit,
                                         dtlIntervalIndicator  aIntervalIndicator,
                                         stlInt64              aAvailableSize,
                                         dtlDataValue        * aValue,
                                         stlBool             * aSuccessWithInfo,
                                         dtlFuncVector       * aSourceVectorFunc,
                                         void                * aSourceVectorArg,
                                         dtlFuncVector       * aDestVectorFunc,
                                         void                * aDestVectorArg,
                                         stlErrorStack       * aErrorStack )
{
    return dtdSmallIntSetValueFromString( aString,
                                          aStringLength,
                                          aPrecision,
                                          aScale,
                                          aStringUnit,
                                          aIntervalIndicator,
                                          aAvailableSize,
                                          aValue,
                                          aSuccessWithInfo,
                                          aSourceVectorFunc,
                                          aSourceVectorArg,
                                          aDestVectorFunc,
                                          aDestVectorArg,
                                          aErrorStack );
}


stlStatus dtlIntegerSetValueFromString( stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        dtlIntervalIndicator  aIntervalIndicator,
                                        stlInt64              aAvailableSize,
                                        dtlDataValue        * aValue,
                                        stlBool             * aSuccessWithInfo,
                                        dtlFuncVector       * aSourceVectorFunc,
                                        void                * aSourceVectorArg,
                                        dtlFuncVector       * aDestVectorFunc,
                                        void                * aDestVectorArg,
                                        stlErrorStack       * aErrorStack )
{
    return dtdIntegerSetValueFromString( aString,
                                         aStringLength,
                                         aPrecision,
                                         aScale,
                                         aStringUnit,
                                         aIntervalIndicator,
                                         aAvailableSize,
                                         aValue,
                                         aSuccessWithInfo,
                                         aSourceVectorFunc,
                                         aSourceVectorArg,
                                         aDestVectorFunc,
                                         aDestVectorArg,
                                         aErrorStack );
}


stlStatus dtlBigIntSetValueFromString( stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       dtlIntervalIndicator  aIntervalIndicator,
                                       stlInt64              aAvailableSize,
                                       dtlDataValue        * aValue,
                                       stlBool             * aSuccessWithInfo,
                                       dtlFuncVector       * aSourceVectorFunc,
                                       void                * aSourceVectorArg,
                                       dtlFuncVector       * aDestVectorFunc,
                                       void                * aDestVectorArg,
                                       stlErrorStack       * aErrorStack )
{
    return dtdBigIntSetValueFromString( aString,
                                        aStringLength,
                                        aPrecision,
                                        aScale,
                                        aStringUnit,
                                        aIntervalIndicator,
                                        aAvailableSize,
                                        aValue,
                                        aSuccessWithInfo,
                                        aSourceVectorFunc,
                                        aSourceVectorArg,
                                        aDestVectorFunc,
                                        aDestVectorArg,
                                        aErrorStack );
}


stlStatus dtlRealSetValueFromString( stlChar             * aString,
                                     stlInt64              aStringLength,
                                     stlInt64              aPrecision,
                                     stlInt64              aScale,
                                     dtlStringLengthUnit   aStringUnit,
                                     dtlIntervalIndicator  aIntervalIndicator,
                                     stlInt64              aAvailableSize,
                                     dtlDataValue        * aValue,
                                     stlBool             * aSuccessWithInfo,
                                     dtlFuncVector       * aSourceVectorFunc,
                                     void                * aSourceVectorArg,
                                     dtlFuncVector       * aDestVectorFunc,
                                     void                * aDestVectorArg,
                                     stlErrorStack       * aErrorStack )
{
    return dtdRealSetValueFromString( aString,
                                      aStringLength,
                                      aPrecision,
                                      aScale,
                                      aStringUnit,
                                      aIntervalIndicator,
                                      aAvailableSize,
                                      aValue,
                                      aSuccessWithInfo,
                                      aSourceVectorFunc,
                                      aSourceVectorArg,
                                      aDestVectorFunc,
                                      aDestVectorArg,
                                      aErrorStack );
}


stlStatus dtlDoubleSetValueFromString( stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       dtlIntervalIndicator  aIntervalIndicator,
                                       stlInt64              aAvailableSize,
                                       dtlDataValue        * aValue,
                                       stlBool             * aSuccessWithInfo,
                                       dtlFuncVector       * aSourceVectorFunc,
                                       void                * aSourceVectorArg,
                                       dtlFuncVector       * aDestVectorFunc,
                                       void                * aDestVectorArg,
                                       stlErrorStack       * aErrorStack )
{
    return dtdDoubleSetValueFromString( aString,
                                        aStringLength,
                                        aPrecision,
                                        aScale,
                                        aStringUnit,
                                        aIntervalIndicator,
                                        aAvailableSize,
                                        aValue,
                                        aSuccessWithInfo,
                                        aSourceVectorFunc,
                                        aSourceVectorArg,
                                        aDestVectorFunc,
                                        aDestVectorArg,
                                        aErrorStack );
}


stlStatus dtlNumericSetValueFromString( stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        dtlIntervalIndicator  aIntervalIndicator,
                                        stlInt64              aAvailableSize,
                                        dtlDataValue        * aValue,
                                        stlBool             * aSuccessWithInfo,
                                        dtlFuncVector       * aSourceVectorFunc,
                                        void                * aSourceVectorArg,
                                        dtlFuncVector       * aDestVectorFunc,
                                        void                * aDestVectorArg,
                                        stlErrorStack       * aErrorStack )
{
    return dtdNumericSetValueFromString( aString,
                                         aStringLength,
                                         aPrecision,
                                         aScale,
                                         aStringUnit,
                                         aIntervalIndicator,
                                         aAvailableSize,
                                         aValue,
                                         aSuccessWithInfo,
                                         aSourceVectorFunc,
                                         aSourceVectorArg,
                                         aDestVectorFunc,
                                         aDestVectorArg,
                                         aErrorStack );
}


stlStatus dtlFloatSetValueFromString( stlChar             * aString,
                                      stlInt64              aStringLength,
                                      stlInt64              aPrecision,
                                      stlInt64              aScale,
                                      dtlStringLengthUnit   aStringUnit,
                                      dtlIntervalIndicator  aIntervalIndicator,
                                      stlInt64              aAvailableSize,
                                      dtlDataValue        * aValue,
                                      stlBool             * aSuccessWithInfo,
                                      dtlFuncVector       * aSourceVectorFunc,
                                      void                * aSourceVectorArg,
                                      dtlFuncVector       * aDestVectorFunc,
                                      void                * aDestVectorArg,
                                      stlErrorStack       * aErrorStack )
{
    return dtdFloatSetValueFromString( aString,
                                       aStringLength,
                                       aPrecision,
                                       aScale,
                                       aStringUnit,
                                       aIntervalIndicator,
                                       aAvailableSize,
                                       aValue,
                                       aSuccessWithInfo,
                                       aSourceVectorFunc,
                                       aSourceVectorArg,
                                       aDestVectorFunc,
                                       aDestVectorArg,
                                       aErrorStack );
}


stlStatus dtlCharSetValueFromString( stlChar             * aString,
                                     stlInt64              aStringLength,
                                     stlInt64              aPrecision,
                                     stlInt64              aScale,
                                     dtlStringLengthUnit   aStringUnit,
                                     dtlIntervalIndicator  aIntervalIndicator,
                                     stlInt64              aAvailableSize,
                                     dtlDataValue        * aValue,
                                     stlBool             * aSuccessWithInfo,
                                     dtlFuncVector       * aSourceVectorFunc,
                                     void                * aSourceVectorArg,
                                     dtlFuncVector       * aDestVectorFunc,
                                     void                * aDestVectorArg,
                                     stlErrorStack       * aErrorStack )
{
    return dtdCharSetValueFromString( aString,
                                      aStringLength,
                                      aPrecision,
                                      aScale,
                                      aStringUnit,
                                      aIntervalIndicator,
                                      aAvailableSize,
                                      aValue,
                                      aSuccessWithInfo,
                                      aSourceVectorFunc,
                                      aSourceVectorArg,
                                      aDestVectorFunc,
                                      aDestVectorArg,
                                      aErrorStack );
}


stlStatus dtlCharSetValueFromStringWithoutPaddNull( stlChar             * aString,
                                                    stlInt64              aStringLength,
                                                    stlInt64              aPrecision,
                                                    stlInt64              aScale,
                                                    dtlStringLengthUnit   aStringUnit,
                                                    dtlIntervalIndicator  aIntervalIndicator,
                                                    stlInt64              aAvailableSize,
                                                    dtlDataValue        * aValue,
                                                    stlBool             * aSuccessWithInfo,
                                                    dtlFuncVector       * aSourceVectorFunc,
                                                    void                * aSourceVectorArg,
                                                    dtlFuncVector       * aDestVectorFunc,
                                                    void                * aDestVectorArg,
                                                    stlErrorStack       * aErrorStack )
{
    return dtdCharSetValueFromStringWithoutPaddNull( aString,
                                                     aStringLength,
                                                     aPrecision,
                                                     aScale,
                                                     aStringUnit,
                                                     aIntervalIndicator,
                                                     aAvailableSize,
                                                     aValue,
                                                     aSuccessWithInfo,
                                                     aSourceVectorFunc,
                                                     aSourceVectorArg,
                                                     aDestVectorFunc,
                                                     aDestVectorArg,
                                                     aErrorStack );
}

stlStatus dtlBinarySetValueFromStringWithoutPaddNull( stlChar             * aString,
                                                      stlInt64              aStringLength,
                                                      stlInt64              aPrecision,
                                                      stlInt64              aScale,
                                                      dtlStringLengthUnit   aStringUnit,
                                                      dtlIntervalIndicator  aIntervalIndicator,
                                                      stlInt64              aAvailableSize,
                                                      dtlDataValue        * aValue,
                                                      stlBool             * aSuccessWithInfo,
                                                      dtlFuncVector       * aSourceVectorFunc,
                                                      void                * aSourceVectorArg,
                                                      dtlFuncVector       * aDestVectorFunc,
                                                      void                * aDestVectorArg,
                                                      stlErrorStack       * aErrorStack )
{
    return dtdBinarySetValueFromStringWithoutPaddNull( aString,
                                                       aStringLength,
                                                       aPrecision,
                                                       aScale,
                                                       aStringUnit,
                                                       aIntervalIndicator,
                                                       aAvailableSize,
                                                       aValue,
                                                       aSuccessWithInfo,
                                                       aSourceVectorFunc,
                                                       aSourceVectorArg,
                                                       aDestVectorFunc,
                                                       aDestVectorArg,
                                                       aErrorStack );
}

stlStatus dtlVarcharSetValueFromString( stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        dtlIntervalIndicator  aIntervalIndicator,
                                        stlInt64              aAvailableSize,
                                        dtlDataValue        * aValue,
                                        stlBool             * aSuccessWithInfo,
                                        dtlFuncVector       * aSourceVectorFunc,
                                        void                * aSourceVectorArg,
                                        dtlFuncVector       * aDestVectorFunc,
                                        void                * aDestVectorArg,
                                        stlErrorStack       * aErrorStack )
{
    return dtdVarcharSetValueFromString( aString,
                                         aStringLength,
                                         aPrecision,
                                         aScale,
                                         aStringUnit,
                                         aIntervalIndicator,
                                         aAvailableSize,
                                         aValue,
                                         aSuccessWithInfo,
                                         aSourceVectorFunc,
                                         aSourceVectorArg,
                                         aDestVectorFunc,
                                         aDestVectorArg,
                                         aErrorStack );
}


stlStatus dtlLongvarcharSetValueFromString( stlChar             * aString,
                                            stlInt64              aStringLength,
                                            stlInt64              aPrecision,
                                            stlInt64              aScale,
                                            dtlStringLengthUnit   aStringUnit,
                                            dtlIntervalIndicator  aIntervalIndicator,
                                            stlInt64              aAvailableSize,
                                            dtlDataValue        * aValue,
                                            stlBool             * aSuccessWithInfo,
                                            dtlFuncVector       * aSourceVectorFunc,
                                            void                * aSourceVectorArg,
                                            dtlFuncVector       * aDestVectorFunc,
                                            void                * aDestVectorArg,
                                            stlErrorStack       * aErrorStack )
{
    return dtdLongvarcharSetValueFromString( aString,
                                             aStringLength,
                                             aPrecision,
                                             aScale,
                                             aStringUnit,
                                             aIntervalIndicator,
                                             aAvailableSize,
                                             aValue,
                                             aSuccessWithInfo,
                                             aSourceVectorFunc,
                                             aSourceVectorArg,
                                             aDestVectorFunc,
                                             aDestVectorArg,
                                             aErrorStack );
}


stlStatus dtlBinarySetValueFromString( stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       dtlIntervalIndicator  aIntervalIndicator,
                                       stlInt64              aAvailableSize,
                                       dtlDataValue        * aValue,
                                       stlBool             * aSuccessWithInfo,
                                       dtlFuncVector       * aSourceVectorFunc,
                                       void                * aSourceVectorArg,
                                       dtlFuncVector       * aDestVectorFunc,
                                       void                * aDestVectorArg,
                                       stlErrorStack       * aErrorStack )
{
    return dtdBinarySetValueFromString( aString,
                                        aStringLength,
                                        aPrecision,
                                        aScale,
                                        aStringUnit,
                                        aIntervalIndicator,
                                        aAvailableSize,
                                        aValue,
                                        aSuccessWithInfo,
                                        aSourceVectorFunc,
                                        aSourceVectorArg,
                                        aDestVectorFunc,
                                        aDestVectorArg,
                                        aErrorStack );
}


stlStatus dtlVarbinarySetValueFromString( stlChar             * aString,
                                          stlInt64              aStringLength,
                                          stlInt64              aPrecision,
                                          stlInt64              aScale,
                                          dtlStringLengthUnit   aStringUnit,
                                          dtlIntervalIndicator  aIntervalIndicator,
                                          stlInt64              aAvailableSize,
                                          dtlDataValue        * aValue,
                                          stlBool             * aSuccessWithInfo,
                                          dtlFuncVector       * aSourceVectorFunc,
                                          void                * aSourceVectorArg,
                                          dtlFuncVector       * aDestVectorFunc,
                                          void                * aDestVectorArg,
                                          stlErrorStack       * aErrorStack )
{
    return dtdVarbinarySetValueFromString( aString,
                                           aStringLength,
                                           aPrecision,
                                           aScale,
                                           aStringUnit,
                                           aIntervalIndicator,
                                           aAvailableSize,
                                           aValue,
                                           aSuccessWithInfo,
                                           aSourceVectorFunc,
                                           aSourceVectorArg,
                                           aDestVectorFunc,
                                           aDestVectorArg,
                                           aErrorStack );
}


stlStatus dtlLongvarbinarySetValueFromString( stlChar             * aString,
                                              stlInt64              aStringLength,
                                              stlInt64              aPrecision,
                                              stlInt64              aScale,
                                              dtlStringLengthUnit   aStringUnit,
                                              dtlIntervalIndicator  aIntervalIndicator,
                                              stlInt64              aAvailableSize,
                                              dtlDataValue        * aValue,
                                              stlBool             * aSuccessWithInfo,
                                              dtlFuncVector       * aSourceVectorFunc,
                                              void                * aSourceVectorArg,
                                              dtlFuncVector       * aDestVectorFunc,
                                              void                * aDestVectorArg,
                                              stlErrorStack       * aErrorStack )
{
    return dtdLongvarbinarySetValueFromString( aString,
                                               aStringLength,
                                               aPrecision,
                                               aScale,
                                               aStringUnit,
                                               aIntervalIndicator,
                                               aAvailableSize,
                                               aValue,
                                               aSuccessWithInfo,
                                               aSourceVectorFunc,
                                               aSourceVectorArg,
                                               aDestVectorFunc,
                                               aDestVectorArg,
                                               aErrorStack );
}


stlStatus dtlDateSetValueFromString( stlChar             * aString,
                                     stlInt64              aStringLength,
                                     stlInt64              aPrecision,
                                     stlInt64              aScale,
                                     dtlStringLengthUnit   aStringUnit,
                                     dtlIntervalIndicator  aIntervalIndicator,
                                     stlInt64              aAvailableSize,
                                     dtlDataValue        * aValue,
                                     stlBool             * aSuccessWithInfo,
                                     dtlFuncVector       * aSourceVectorFunc,
                                     void                * aSourceVectorArg,
                                     dtlFuncVector       * aDestVectorFunc,
                                     void                * aDestVectorArg,
                                     stlErrorStack       * aErrorStack )
{
    return dtdDateSetValueFromString( aString,
                                      aStringLength,
                                      aPrecision,
                                      aScale,
                                      aStringUnit,
                                      aIntervalIndicator,
                                      aAvailableSize,
                                      aValue,
                                      aSuccessWithInfo,
                                      aSourceVectorFunc,
                                      aSourceVectorArg,
                                      aDestVectorFunc,
                                      aDestVectorArg,
                                      aErrorStack );
}


stlStatus dtlTimeSetValueFromString( stlChar             * aString,
                                     stlInt64              aStringLength,
                                     stlInt64              aPrecision,
                                     stlInt64              aScale,
                                     dtlStringLengthUnit   aStringUnit,
                                     dtlIntervalIndicator  aIntervalIndicator,
                                     stlInt64              aAvailableSize,
                                     dtlDataValue        * aValue,
                                     stlBool             * aSuccessWithInfo,
                                     dtlFuncVector       * aSourceVectorFunc,
                                     void                * aSourceVectorArg,
                                     dtlFuncVector       * aDestVectorFunc,
                                     void                * aDestVectorArg,
                                     stlErrorStack       * aErrorStack )
{
    return dtdTimeSetValueFromString( aString,
                                      aStringLength,
                                      aPrecision,
                                      aScale,
                                      aStringUnit,
                                      aIntervalIndicator,
                                      aAvailableSize,
                                      aValue,
                                      aSuccessWithInfo,
                                      aSourceVectorFunc,
                                      aSourceVectorArg,
                                      aDestVectorFunc,
                                      aDestVectorArg,
                                      aErrorStack );
}


stlStatus dtlTimestampSetValueFromString( stlChar             * aString,
                                          stlInt64              aStringLength,
                                          stlInt64              aPrecision,
                                          stlInt64              aScale,
                                          dtlStringLengthUnit   aStringUnit,
                                          dtlIntervalIndicator  aIntervalIndicator,
                                          stlInt64              aAvailableSize,
                                          dtlDataValue        * aValue,
                                          stlBool             * aSuccessWithInfo,
                                          dtlFuncVector       * aSourceVectorFunc,
                                          void                * aSourceVectorArg,
                                          dtlFuncVector       * aDestVectorFunc,
                                          void                * aDestVectorArg,
                                          stlErrorStack       * aErrorStack )
{
    return dtdTimestampSetValueFromString( aString,
                                           aStringLength,
                                           aPrecision,
                                           aScale,
                                           aStringUnit,
                                           aIntervalIndicator,
                                           aAvailableSize,
                                           aValue,
                                           aSuccessWithInfo,
                                           aSourceVectorFunc,
                                           aSourceVectorArg,
                                           aDestVectorFunc,
                                           aDestVectorArg,
                                           aErrorStack );
}


stlStatus dtlTimeTzSetValueFromString( stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       dtlIntervalIndicator  aIntervalIndicator,
                                       stlInt64              aAvailableSize,
                                       dtlDataValue        * aValue,
                                       stlBool             * aSuccessWithInfo,
                                       dtlFuncVector       * aSourceVectorFunc,
                                       void                * aSourceVectorArg,
                                       dtlFuncVector       * aDestVectorFunc,
                                       void                * aDestVectorArg,
                                       stlErrorStack       * aErrorStack )
{
    return dtdTimeTzSetValueFromString( aString,
                                        aStringLength,
                                        aPrecision,
                                        aScale,
                                        aStringUnit,
                                        aIntervalIndicator,
                                        aAvailableSize,
                                        aValue,
                                        aSuccessWithInfo,
                                        aSourceVectorFunc,
                                        aSourceVectorArg,
                                        aDestVectorFunc,
                                        aDestVectorArg,
                                        aErrorStack );
}


stlStatus dtlTimestampTzSetValueFromString( stlChar             * aString,
                                            stlInt64              aStringLength,
                                            stlInt64              aPrecision,
                                            stlInt64              aScale,
                                            dtlStringLengthUnit   aStringUnit,
                                            dtlIntervalIndicator  aIntervalIndicator,
                                            stlInt64              aAvailableSize,
                                            dtlDataValue        * aValue,
                                            stlBool             * aSuccessWithInfo,
                                            dtlFuncVector       * aSourceVectorFunc,
                                            void                * aSourceVectorArg,
                                            dtlFuncVector       * aDestVectorFunc,
                                            void                * aDestVectorArg,
                                            stlErrorStack       * aErrorStack )
{
    return dtdTimestampTzSetValueFromString( aString,
                                             aStringLength,
                                             aPrecision,
                                             aScale,
                                             aStringUnit,
                                             aIntervalIndicator,
                                             aAvailableSize,
                                             aValue,
                                             aSuccessWithInfo,
                                             aSourceVectorFunc,
                                             aSourceVectorArg,
                                             aDestVectorFunc,
                                             aDestVectorArg,
                                             aErrorStack );
}


stlStatus dtlIntervalYearToMonthSetValueFromString( stlChar             * aString,
                                                    stlInt64              aStringLength,
                                                    stlInt64              aPrecision,
                                                    stlInt64              aScale,
                                                    dtlStringLengthUnit   aStringUnit,
                                                    dtlIntervalIndicator  aIntervalIndicator,
                                                    stlInt64              aAvailableSize,
                                                    dtlDataValue        * aValue,
                                                    stlBool             * aSuccessWithInfo,
                                                    dtlFuncVector       * aSourceVectorFunc,
                                                    void                * aSourceVectorArg,
                                                    dtlFuncVector       * aDestVectorFunc,
                                                    void                * aDestVectorArg,
                                                    stlErrorStack       * aErrorStack )
{
    return dtdIntervalYearToMonthSetValueFromString( aString,
                                                     aStringLength,
                                                     aPrecision,
                                                     aScale,
                                                     aStringUnit,
                                                     aIntervalIndicator,
                                                     aAvailableSize,
                                                     aValue,
                                                     aSuccessWithInfo,
                                                     aSourceVectorFunc,
                                                     aSourceVectorArg,
                                                     aDestVectorFunc,
                                                     aDestVectorArg,
                                                     aErrorStack );
}


stlStatus dtlIntervalDayToSecondSetValueFromString( stlChar             * aString,
                                                    stlInt64              aStringLength,
                                                    stlInt64              aPrecision,
                                                    stlInt64              aScale,
                                                    dtlStringLengthUnit   aStringUnit,
                                                    dtlIntervalIndicator  aIntervalIndicator,
                                                    stlInt64              aAvailableSize,
                                                    dtlDataValue        * aValue,
                                                    stlBool             * aSuccessWithInfo,
                                                    dtlFuncVector       * aSourceVectorFunc,
                                                    void                * aSourceVectorArg,
                                                    dtlFuncVector       * aDestVectorFunc,
                                                    void                * aDestVectorArg,
                                                    stlErrorStack       * aErrorStack )
{
    return dtdIntervalDayToSecondSetValueFromString( aString,
                                                     aStringLength,
                                                     aPrecision,
                                                     aScale,
                                                     aStringUnit,
                                                     aIntervalIndicator,
                                                     aAvailableSize,
                                                     aValue,
                                                     aSuccessWithInfo,
                                                     aSourceVectorFunc,
                                                     aSourceVectorArg,
                                                     aDestVectorFunc,
                                                     aDestVectorArg,
                                                     aErrorStack );
}


stlStatus dtlRowIdSetValueFromString( stlChar             * aString,
                                      stlInt64              aStringLength,
                                      stlInt64              aPrecision,
                                      stlInt64              aScale,
                                      dtlStringLengthUnit   aStringUnit,
                                      dtlIntervalIndicator  aIntervalIndicator,
                                      stlInt64              aAvailableSize,
                                      dtlDataValue        * aValue,
                                      stlBool             * aSuccessWithInfo,
                                      dtlFuncVector       * aSourceVectorFunc,
                                      void                * aSourceVectorArg,
                                      dtlFuncVector       * aDestVectorFunc,
                                      void                * aDestVectorArg,
                                      stlErrorStack       * aErrorStack )
{
    return dtdRowIdSetValueFromString( aString,
                                       aStringLength,
                                       aPrecision,
                                       aScale,
                                       aStringUnit,
                                       aIntervalIndicator,
                                       aAvailableSize,
                                       aValue,
                                       aSuccessWithInfo,
                                       aSourceVectorFunc,
                                       aSourceVectorArg,
                                       aDestVectorFunc,
                                       aDestVectorArg,
                                       aErrorStack );
}


/**
 * Set Value From Integer
 */

stlStatus dtlBooleanSetValueFromInteger( stlInt64               aInteger,
                                         stlInt64               aPrecision,
                                         stlInt64               aScale,
                                         dtlStringLengthUnit    aStringUnit,
                                         dtlIntervalIndicator   aIntervalIndicator,
                                         stlInt64               aAvailableSize,
                                         dtlDataValue         * aValue,
                                         stlBool              * aSuccessWithInfo,
                                         dtlFuncVector        * aVectorFunc,
                                         void                 * aVectorArg,
                                         stlErrorStack        * aErrorStack )
{
    return dtdBooleanSetValueFromInteger( aInteger,
                                          aPrecision,
                                          aScale,
                                          aStringUnit,
                                          aIntervalIndicator,
                                          aAvailableSize,
                                          aValue,
                                          aSuccessWithInfo,
                                          aVectorFunc,
                                          aVectorArg,
                                          aErrorStack );
}


stlStatus dtlSmallIntSetValueFromInteger( stlInt64               aInteger,
                                          stlInt64               aPrecision,
                                          stlInt64               aScale,
                                          dtlStringLengthUnit    aStringUnit,
                                          dtlIntervalIndicator   aIntervalIndicator,
                                          stlInt64               aAvailableSize,
                                          dtlDataValue         * aValue,
                                          stlBool              * aSuccessWithInfo,
                                          dtlFuncVector        * aVectorFunc,
                                          void                 * aVectorArg,
                                          stlErrorStack        * aErrorStack )
{
    return dtdSmallIntSetValueFromInteger( aInteger,
                                           aPrecision,
                                           aScale,
                                           aStringUnit,
                                           aIntervalIndicator,
                                           aAvailableSize,
                                           aValue,
                                           aSuccessWithInfo,
                                           aVectorFunc,
                                           aVectorArg,
                                           aErrorStack );
}


stlStatus dtlIntegerSetValueFromInteger( stlInt64               aInteger,
                                         stlInt64               aPrecision,
                                         stlInt64               aScale,
                                         dtlStringLengthUnit    aStringUnit,
                                         dtlIntervalIndicator   aIntervalIndicator,
                                         stlInt64               aAvailableSize,
                                         dtlDataValue         * aValue,
                                         stlBool              * aSuccessWithInfo,
                                         dtlFuncVector        * aVectorFunc,
                                         void                 * aVectorArg,
                                         stlErrorStack        * aErrorStack )
{
    /* integer value로부터 Integer 타입의 value 구성 */
    dtlIntegerType  * sDstValue;

    DTL_CHECK_TYPE( DTL_TYPE_NATIVE_INTEGER, aValue, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_NATIVE_INTEGER_SIZE, aErrorStack );
    
    /* integer value로부터 Integer 타입의 value 구성 */
    STL_TRY_THROW( (aInteger >= DTL_NATIVE_INTEGER_MIN) &&
                   (aInteger <= DTL_NATIVE_INTEGER_MAX),
                   ERROR_OUT_OF_RANGE );

    sDstValue  = (dtlIntegerType *) aValue->mValue;
    *sDstValue = (dtlIntegerType) aInteger;

    aValue->mLength = DTL_NATIVE_INTEGER_SIZE;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
    /* return dtdIntegerSetValueFromInteger( aInteger, */
    /*                                       aPrecision, */
    /*                                       aScale, */
    /*                                       aStringUnit, */
    /*                                       aIntervalIndicator, */
    /*                                       aAvailableSize, */
    /*                                       aValue, */
    /*                                       aSuccessWithInfo, */
    /*                                       aVectorFunc, */
    /*                                       aVectorArg, */
    /*                                       aErrorStack ); */
}


stlStatus dtlBigIntSetValueFromInteger( stlInt64               aInteger,
                                        stlInt64               aPrecision,
                                        stlInt64               aScale,
                                        dtlStringLengthUnit    aStringUnit,
                                        dtlIntervalIndicator   aIntervalIndicator,
                                        stlInt64               aAvailableSize,
                                        dtlDataValue         * aValue,
                                        stlBool              * aSuccessWithInfo,
                                        dtlFuncVector        * aVectorFunc,
                                        void                 * aVectorArg,
                                        stlErrorStack        * aErrorStack )
{
    return dtdBigIntSetValueFromInteger( aInteger,
                                         aPrecision,
                                         aScale,
                                         aStringUnit,
                                         aIntervalIndicator,
                                         aAvailableSize,
                                         aValue,
                                         aSuccessWithInfo,
                                         aVectorFunc,
                                         aVectorArg,
                                         aErrorStack );
}


stlStatus dtlRealSetValueFromInteger( stlInt64               aInteger,
                                      stlInt64               aPrecision,
                                      stlInt64               aScale,
                                      dtlStringLengthUnit    aStringUnit,
                                      dtlIntervalIndicator   aIntervalIndicator,
                                      stlInt64               aAvailableSize,
                                      dtlDataValue         * aValue,
                                      stlBool              * aSuccessWithInfo,
                                      dtlFuncVector        * aVectorFunc,
                                      void                 * aVectorArg,
                                      stlErrorStack        * aErrorStack )
{
    return dtdRealSetValueFromInteger( aInteger,
                                       aPrecision,
                                       aScale,
                                       aStringUnit,
                                       aIntervalIndicator,
                                       aAvailableSize,
                                       aValue,
                                       aSuccessWithInfo,
                                       aVectorFunc,
                                       aVectorArg,
                                       aErrorStack );
}

stlStatus dtlDoubleSetValueFromInteger( stlInt64               aInteger,
                                        stlInt64               aPrecision,
                                        stlInt64               aScale,
                                        dtlStringLengthUnit    aStringUnit,
                                        dtlIntervalIndicator   aIntervalIndicator,
                                        stlInt64               aAvailableSize,
                                        dtlDataValue         * aValue,
                                        stlBool              * aSuccessWithInfo,
                                        dtlFuncVector        * aVectorFunc,
                                        void                 * aVectorArg,
                                        stlErrorStack        * aErrorStack )
{
    return dtdDoubleSetValueFromInteger( aInteger,
                                         aPrecision,
                                         aScale,
                                         aStringUnit,
                                         aIntervalIndicator,
                                         aAvailableSize,
                                         aValue,
                                         aSuccessWithInfo,
                                         aVectorFunc,
                                         aVectorArg,
                                         aErrorStack );
}


stlStatus dtlNumericSetValueFromInteger( stlInt64               aInteger,
                                         stlInt64               aPrecision,
                                         stlInt64               aScale,
                                         dtlStringLengthUnit    aStringUnit,
                                         dtlIntervalIndicator   aIntervalIndicator,
                                         stlInt64               aAvailableSize,
                                         dtlDataValue         * aValue,
                                         stlBool              * aSuccessWithInfo,
                                         dtlFuncVector        * aVectorFunc,
                                         void                 * aVectorArg,
                                         stlErrorStack        * aErrorStack )
{
    return dtdNumericSetValueFromInteger( aInteger,
                                          aPrecision,
                                          aScale,
                                          aStringUnit,
                                          aIntervalIndicator,
                                          aAvailableSize,
                                          aValue,
                                          aSuccessWithInfo,
                                          aVectorFunc,
                                          aVectorArg,
                                          aErrorStack );
}


stlStatus dtlFloatSetValueFromInteger( stlInt64               aInteger,
                                       stlInt64               aPrecision,
                                       stlInt64               aScale,
                                       dtlStringLengthUnit    aStringUnit,
                                       dtlIntervalIndicator   aIntervalIndicator,
                                       stlInt64               aAvailableSize,
                                       dtlDataValue         * aValue,
                                       stlBool              * aSuccessWithInfo,
                                       dtlFuncVector        * aVectorFunc,
                                       void                 * aVectorArg,
                                       stlErrorStack        * aErrorStack )
{
    return dtdFloatSetValueFromInteger( aInteger,
                                        aPrecision,
                                        aScale,
                                        aStringUnit,
                                        aIntervalIndicator,
                                        aAvailableSize,
                                        aValue,
                                        aSuccessWithInfo,
                                        aVectorFunc,
                                        aVectorArg,
                                        aErrorStack );
}


stlStatus dtlCharSetValueFromInteger( stlInt64               aInteger,
                                      stlInt64               aPrecision,
                                      stlInt64               aScale,
                                      dtlStringLengthUnit    aStringUnit,
                                      dtlIntervalIndicator   aIntervalIndicator,
                                      stlInt64               aAvailableSize,
                                      dtlDataValue         * aValue,
                                      stlBool              * aSuccessWithInfo,
                                      dtlFuncVector        * aVectorFunc,
                                      void                 * aVectorArg,
                                      stlErrorStack        * aErrorStack )
{
    return dtdCharSetValueFromInteger( aInteger,
                                       aPrecision,
                                       aScale,
                                       aStringUnit,
                                       aIntervalIndicator,
                                       aAvailableSize,
                                       aValue,
                                       aSuccessWithInfo,
                                       aVectorFunc,
                                       aVectorArg,
                                       aErrorStack );
}


stlStatus dtlCharSetValueFromIntegerWithoutPaddNull( stlInt64               aInteger,
                                                     stlInt64               aPrecision,
                                                     stlInt64               aScale,
                                                     dtlStringLengthUnit    aStringUnit,
                                                     dtlIntervalIndicator   aIntervalIndicator,
                                                     stlInt64               aAvailableSize,
                                                     dtlDataValue         * aValue,
                                                     stlBool              * aSuccessWithInfo,
                                                     dtlFuncVector        * aVectorFunc,
                                                     void                 * aVectorArg,
                                                     stlErrorStack        * aErrorStack )
{
    return dtdCharSetValueFromIntegerWithoutPaddNull( aInteger,
                                                      aPrecision,
                                                      aScale,
                                                      aStringUnit,
                                                      aIntervalIndicator,
                                                      aAvailableSize,
                                                      aValue,
                                                      aSuccessWithInfo,
                                                      aVectorFunc,
                                                      aVectorArg,
                                                      aErrorStack );
}


stlStatus dtlVarcharSetValueFromInteger( stlInt64               aInteger,
                                         stlInt64               aPrecision,
                                         stlInt64               aScale,
                                         dtlStringLengthUnit    aStringUnit,
                                         dtlIntervalIndicator   aIntervalIndicator,
                                         stlInt64               aAvailableSize,
                                         dtlDataValue         * aValue,
                                         stlBool              * aSuccessWithInfo,
                                         dtlFuncVector        * aVectorFunc,
                                         void                 * aVectorArg,
                                         stlErrorStack        * aErrorStack )
{
    return dtdVarcharSetValueFromInteger( aInteger,
                                          aPrecision,
                                          aScale,
                                          aStringUnit,
                                          aIntervalIndicator,
                                          aAvailableSize,
                                          aValue,
                                          aSuccessWithInfo,
                                          aVectorFunc,
                                          aVectorArg,
                                          aErrorStack );
}


stlStatus dtlLongvarcharSetValueFromInteger( stlInt64               aInteger,
                                             stlInt64               aPrecision,
                                             stlInt64               aScale,
                                             dtlStringLengthUnit    aStringUnit,
                                             dtlIntervalIndicator   aIntervalIndicator,
                                             stlInt64               aAvailableSize,
                                             dtlDataValue         * aValue,
                                             stlBool              * aSuccessWithInfo,
                                             dtlFuncVector        * aVectorFunc,
                                             void                 * aVectorArg,
                                             stlErrorStack        * aErrorStack )
{
    return dtdLongvarcharSetValueFromInteger( aInteger,
                                              aPrecision,
                                              aScale,
                                              aStringUnit,
                                              aIntervalIndicator,
                                              aAvailableSize,
                                              aValue,
                                              aSuccessWithInfo,
                                              aVectorFunc,
                                              aVectorArg,
                                              aErrorStack );
}


/* stlStatus dtlBinarySetValueFromInteger( stlInt64               aInteger, */
/*                                         stlInt64               aPrecision, */
/*                                         stlInt64               aScale, */
/*                                         dtlStringLengthUnit    aStringUnit, */
/*                                         dtlIntervalIndicator   aIntervalIndicator, */
/*                                         stlInt64               aAvailableSize, */
/*                                         dtlDataValue         * aValue, */
/*                                         stlBool              * aSuccessWithInfo, */
/*                                         dtlFuncVector        * aVectorFunc, */
/*                                         void                 * aVectorArg, */
/*                                         stlErrorStack        * aErrorStack ) */
/* { */
/*     return dtdBinarySetValueFromInteger( aInteger, */
/*                                          aPrecision, */
/*                                          aScale, */
/*                                          aStringUnit, */
/*                                          aIntervalIndicator, */
/*                                          aAvailableSize, */
/*                                          aValue, */
/*                                          aSuccessWithInfo, */
/*                                          aVectorFunc, */
/*                                          aVectorArg, */
/*                                          aErrorStack ); */
/* } */


/* stlStatus dtlVarbinarySetValueFromInteger( stlInt64               aInteger, */
/*                                            stlInt64               aPrecision, */
/*                                            stlInt64               aScale, */
/*                                            dtlStringLengthUnit    aStringUnit, */
/*                                            dtlIntervalIndicator   aIntervalIndicator, */
/*                                            stlInt64               aAvailableSize, */
/*                                            dtlDataValue         * aValue, */
/*                                            stlBool              * aSuccessWithInfo, */
/*                                            dtlFuncVector        * aVectorFunc, */
/*                                            void                 * aVectorArg, */
/*                                            stlErrorStack        * aErrorStack ) */
/* { */
/*     return dtdVarbinarySetValueFromInteger( aInteger, */
/*                                             aPrecision, */
/*                                             aScale, */
/*                                             aStringUnit, */
/*                                             aIntervalIndicator, */
/*                                             aAvailableSize, */
/*                                             aValue, */
/*                                             aSuccessWithInfo, */
/*                                             aVectorFunc, */
/*                                             aVectorArg, */
/*                                             aErrorStack ); */
/* } */


/* stlStatus dtlLongvarbinarySetValueFromInteger( stlInt64               aInteger, */
/*                                                stlInt64               aPrecision, */
/*                                                stlInt64               aScale, */
/*                                                dtlStringLengthUnit    aStringUnit, */
/*                                                dtlIntervalIndicator   aIntervalIndicator, */
/*                                                stlInt64               aAvailableSize, */
/*                                                dtlDataValue         * aValue, */
/*                                                stlBool              * aSuccessWithInfo, */
/*                                                dtlFuncVector        * aVectorFunc, */
/*                                                void                 * aVectorArg, */
/*                                                stlErrorStack        * aErrorStack ) */
/* { */
/*     return dtdLongvarbinarySetValueFromInteger( aInteger, */
/*                                                 aPrecision, */
/*                                                 aScale, */
/*                                                 aStringUnit, */
/*                                                 aIntervalIndicator, */
/*                                                 aAvailableSize, */
/*                                                 aValue, */
/*                                                 aSuccessWithInfo, */
/*                                                 aVectorFunc, */
/*                                                 aVectorArg, */
/*                                                 aErrorStack ); */
/* } */


stlStatus dtlIntervalYearToMonthSetValueFromInteger( stlInt64               aInteger,
                                                     stlInt64               aPrecision,
                                                     stlInt64               aScale,
                                                     dtlStringLengthUnit    aStringUnit,
                                                     dtlIntervalIndicator   aIntervalIndicator,
                                                     stlInt64               aAvailableSize,
                                                     dtlDataValue         * aValue,
                                                     stlBool              * aSuccessWithInfo,
                                                     dtlFuncVector        * aVectorFunc,
                                                     void                 * aVectorArg,
                                                     stlErrorStack        * aErrorStack )
{
    return dtdIntervalYearToMonthSetValueFromInteger( aInteger,
                                                      aPrecision,
                                                      aScale,
                                                      aStringUnit,
                                                      aIntervalIndicator,
                                                      aAvailableSize,
                                                      aValue,
                                                      aSuccessWithInfo,
                                                      aVectorFunc,
                                                      aVectorArg,
                                                      aErrorStack );
}


stlStatus dtlIntervalDayToSecondSetValueFromInteger( stlInt64               aInteger,
                                                     stlInt64               aPrecision,
                                                     stlInt64               aScale,
                                                     dtlStringLengthUnit    aStringUnit,
                                                     dtlIntervalIndicator   aIntervalIndicator,
                                                     stlInt64               aAvailableSize,
                                                     dtlDataValue         * aValue,
                                                     stlBool              * aSuccessWithInfo,
                                                     dtlFuncVector        * aVectorFunc,
                                                     void                 * aVectorArg,
                                                     stlErrorStack        * aErrorStack )
{
    return dtdIntervalDayToSecondSetValueFromInteger( aInteger,
                                                      aPrecision,
                                                      aScale,
                                                      aStringUnit,
                                                      aIntervalIndicator,
                                                      aAvailableSize,
                                                      aValue,
                                                      aSuccessWithInfo,
                                                      aVectorFunc,
                                                      aVectorArg,
                                                      aErrorStack );
}


/**
 * Set Value From Real
 */

stlStatus dtlSmallIntSetValueFromReal( stlFloat64             aReal,
                                       stlInt64               aPrecision,
                                       stlInt64               aScale,
                                       dtlStringLengthUnit    aStringUnit,
                                       dtlIntervalIndicator   aIntervalIndicator,
                                       stlInt64               aAvailableSize,
                                       dtlDataValue         * aValue,
                                       stlBool              * aSuccessWithInfo,
                                       dtlFuncVector        * aVectorFunc,
                                       void                 * aVectorArg,
                                       stlErrorStack        * aErrorStack )
{
    return dtdSmallIntSetValueFromReal( aReal,
                                        aPrecision,
                                        aScale,
                                        aStringUnit,
                                        aIntervalIndicator,
                                        aAvailableSize,
                                        aValue,
                                        aSuccessWithInfo,
                                        aVectorFunc,
                                        aVectorArg,
                                        aErrorStack );
}


stlStatus dtlIntegerSetValueFromReal( stlFloat64             aReal,
                                      stlInt64               aPrecision,
                                      stlInt64               aScale,
                                      dtlStringLengthUnit    aStringUnit,
                                      dtlIntervalIndicator   aIntervalIndicator,
                                      stlInt64               aAvailableSize,
                                      dtlDataValue         * aValue,
                                      stlBool              * aSuccessWithInfo,
                                      dtlFuncVector        * aVectorFunc,
                                      void                 * aVectorArg,
                                      stlErrorStack        * aErrorStack )
{
    return dtdIntegerSetValueFromReal( aReal,
                                       aPrecision,
                                       aScale,
                                       aStringUnit,
                                       aIntervalIndicator,
                                       aAvailableSize,
                                       aValue,
                                       aSuccessWithInfo,
                                       aVectorFunc,
                                       aVectorArg,
                                       aErrorStack );
}


stlStatus dtlBigIntSetValueFromReal( stlFloat64             aReal,
                                     stlInt64               aPrecision,
                                     stlInt64               aScale,
                                     dtlStringLengthUnit    aStringUnit,
                                     dtlIntervalIndicator   aIntervalIndicator,
                                     stlInt64               aAvailableSize,
                                     dtlDataValue         * aValue,
                                     stlBool              * aSuccessWithInfo,
                                     dtlFuncVector        * aVectorFunc,
                                     void                 * aVectorArg,
                                     stlErrorStack        * aErrorStack )
{
    return dtdBigIntSetValueFromReal( aReal,
                                      aPrecision,
                                      aScale,
                                      aStringUnit,
                                      aIntervalIndicator,
                                      aAvailableSize,
                                      aValue,
                                      aSuccessWithInfo,
                                      aVectorFunc,
                                      aVectorArg,
                                      aErrorStack );
}


stlStatus dtlRealSetValueFromReal( stlFloat64             aReal,
                                   stlInt64               aPrecision,
                                   stlInt64               aScale,
                                   dtlStringLengthUnit    aStringUnit,
                                   dtlIntervalIndicator   aIntervalIndicator,
                                   stlInt64               aAvailableSize,
                                   dtlDataValue         * aValue,
                                   stlBool              * aSuccessWithInfo,
                                   dtlFuncVector        * aVectorFunc,
                                   void                 * aVectorArg,
                                   stlErrorStack        * aErrorStack )
{
    return dtdRealSetValueFromReal( aReal,
                                    aPrecision,
                                    aScale,
                                    aStringUnit,
                                    aIntervalIndicator,
                                    aAvailableSize,
                                    aValue,
                                    aSuccessWithInfo,
                                    aVectorFunc,
                                    aVectorArg,
                                    aErrorStack );
}

stlStatus dtlDoubleSetValueFromReal( stlFloat64             aReal,
                                     stlInt64               aPrecision,
                                     stlInt64               aScale,
                                     dtlStringLengthUnit    aStringUnit,
                                     dtlIntervalIndicator   aIntervalIndicator,
                                     stlInt64               aAvailableSize,
                                     dtlDataValue         * aValue,
                                     stlBool              * aSuccessWithInfo,
                                     dtlFuncVector        * aVectorFunc,
                                     void                 * aVectorArg,
                                     stlErrorStack        * aErrorStack )
{
    return dtdDoubleSetValueFromReal( aReal,
                                      aPrecision,
                                      aScale,
                                      aStringUnit,
                                      aIntervalIndicator,
                                      aAvailableSize,
                                      aValue,
                                      aSuccessWithInfo,
                                      aVectorFunc,
                                      aVectorArg,
                                      aErrorStack );
}


stlStatus dtlNumericSetValueFromReal( stlFloat64             aReal,
                                      stlInt64               aPrecision,
                                      stlInt64               aScale,
                                      dtlStringLengthUnit    aStringUnit,
                                      dtlIntervalIndicator   aIntervalIndicator,
                                      stlInt64               aAvailableSize,
                                      dtlDataValue         * aValue,
                                      stlBool              * aSuccessWithInfo,
                                      dtlFuncVector        * aVectorFunc,
                                      void                 * aVectorArg,
                                      stlErrorStack        * aErrorStack )
{
    return dtdNumericSetValueFromReal( aReal,
                                       aPrecision,
                                       aScale,
                                       aStringUnit,
                                       aIntervalIndicator,
                                       aAvailableSize,
                                       aValue,
                                       aSuccessWithInfo,
                                       aVectorFunc,
                                       aVectorArg,
                                       aErrorStack );
}


stlStatus dtlFloatSetValueFromReal( stlFloat64             aReal,
                                    stlInt64               aPrecision,
                                    stlInt64               aScale,
                                    dtlStringLengthUnit    aStringUnit,
                                    dtlIntervalIndicator   aIntervalIndicator,
                                    stlInt64               aAvailableSize,
                                    dtlDataValue         * aValue,
                                    stlBool              * aSuccessWithInfo,
                                    dtlFuncVector        * aVectorFunc,
                                    void                 * aVectorArg,
                                    stlErrorStack        * aErrorStack )
{
    return dtdFloatSetValueFromReal( aReal,
                                     aPrecision,
                                     aScale,
                                     aStringUnit,
                                     aIntervalIndicator,
                                     aAvailableSize,
                                     aValue,
                                     aSuccessWithInfo,
                                     aVectorFunc,
                                     aVectorArg,
                                     aErrorStack );
}


stlStatus dtlCharSetValueFromReal( stlFloat64             aReal,
                                   stlInt64               aPrecision,
                                   stlInt64               aScale,
                                   dtlStringLengthUnit    aStringUnit,
                                   dtlIntervalIndicator   aIntervalIndicator,
                                   stlInt64               aAvailableSize,
                                   dtlDataValue         * aValue,
                                   stlBool              * aSuccessWithInfo,
                                   dtlFuncVector        * aVectorFunc,
                                   void                 * aVectorArg,
                                   stlErrorStack        * aErrorStack )
{
    return dtdCharSetValueFromReal( aReal,
                                    aPrecision,
                                    aScale,
                                    aStringUnit,
                                    aIntervalIndicator,
                                    aAvailableSize,
                                    aValue,
                                    aSuccessWithInfo,
                                    aVectorFunc,
                                    aVectorArg,
                                    aErrorStack );
}


stlStatus dtlCharSetValueFromRealWithoutPaddNull( stlFloat64             aReal,
                                                  stlInt64               aPrecision,
                                                  stlInt64               aScale,
                                                  dtlStringLengthUnit    aStringUnit,
                                                  dtlIntervalIndicator   aIntervalIndicator,
                                                  stlInt64               aAvailableSize,
                                                  dtlDataValue         * aValue,
                                                  stlBool              * aSuccessWithInfo,
                                                  dtlFuncVector        * aVectorFunc,
                                                  void                 * aVectorArg,
                                                  stlErrorStack        * aErrorStack )
{
    return dtdCharSetValueFromRealWithoutPaddNull( aReal,
                                                   aPrecision,
                                                   aScale,
                                                   aStringUnit,
                                                   aIntervalIndicator,
                                                   aAvailableSize,
                                                   aValue,
                                                   aSuccessWithInfo,
                                                   aVectorFunc,
                                                   aVectorArg,
                                                   aErrorStack );
}


stlStatus dtlVarcharSetValueFromReal( stlFloat64             aReal,
                                      stlInt64               aPrecision,
                                      stlInt64               aScale,
                                      dtlStringLengthUnit    aStringUnit,
                                      dtlIntervalIndicator   aIntervalIndicator,
                                      stlInt64               aAvailableSize,
                                      dtlDataValue         * aValue,
                                      stlBool              * aSuccessWithInfo,
                                      dtlFuncVector        * aVectorFunc,
                                      void                 * aVectorArg,
                                      stlErrorStack        * aErrorStack )
{
    return dtdVarcharSetValueFromReal( aReal,
                                       aPrecision,
                                       aScale,
                                       aStringUnit,
                                       aIntervalIndicator,
                                       aAvailableSize,
                                       aValue,
                                       aSuccessWithInfo,
                                       aVectorFunc,
                                       aVectorArg,
                                       aErrorStack );
}


stlStatus dtlLongvarcharSetValueFromReal( stlFloat64             aReal,
                                          stlInt64               aPrecision,
                                          stlInt64               aScale,
                                          dtlStringLengthUnit    aStringUnit,
                                          dtlIntervalIndicator   aIntervalIndicator,
                                          stlInt64               aAvailableSize,
                                          dtlDataValue         * aValue,
                                          stlBool              * aSuccessWithInfo,
                                          dtlFuncVector        * aVectorFunc,
                                          void                 * aVectorArg,
                                          stlErrorStack        * aErrorStack )
{
    return dtdLongvarcharSetValueFromReal( aReal,
                                           aPrecision,
                                           aScale,
                                           aStringUnit,
                                           aIntervalIndicator,
                                           aAvailableSize,
                                           aValue,
                                           aSuccessWithInfo,
                                           aVectorFunc,
                                           aVectorArg,
                                           aErrorStack );
}


/* stlStatus dtlBinarySetValueFromReal( stlFloat64             aReal, */
/*                                      stlInt64               aPrecision, */
/*                                      stlInt64               aScale, */
/*                                      dtlStringLengthUnit    aStringUnit, */
/*                                      dtlIntervalIndicator   aIntervalIndicator, */
/*                                      stlInt64               aAvailableSize, */
/*                                      dtlDataValue         * aValue, */
/*                                      stlBool              * aSuccessWithInfo, */
/*                                      dtlFuncVector        * aVectorFunc, */
/*                                      void                 * aVectorArg, */
/*                                      stlErrorStack        * aErrorStack ) */
/* { */
/*     return dtdBinarySetValueFromReal( aReal, */
/*                                       aPrecision, */
/*                                       aScale, */
/*                                       aStringUnit, */
/*                                       aIntervalIndicator, */
/*                                       aAvailableSize, */
/*                                       aValue, */
/*                                       aSuccessWithInfo, */
/*                                       aVectorFunc, */
/*                                       aVectorArg, */
/*                                       aErrorStack ); */
/* } */


/* stlStatus dtlVarbinarySetValueFromReal( stlFloat64             aReal, */
/*                                         stlInt64               aPrecision, */
/*                                         stlInt64               aScale, */
/*                                         dtlStringLengthUnit    aStringUnit, */
/*                                         dtlIntervalIndicator   aIntervalIndicator, */
/*                                         stlInt64               aAvailableSize, */
/*                                         dtlDataValue         * aValue, */
/*                                         stlBool              * aSuccessWithInfo, */
/*                                         dtlFuncVector        * aVectorFunc, */
/*                                         void                 * aVectorArg, */
/*                                         stlErrorStack        * aErrorStack ) */
/* { */
/*     return dtdVarbinarySetValueFromReal( aReal, */
/*                                          aPrecision, */
/*                                          aScale, */
/*                                          aStringUnit, */
/*                                          aIntervalIndicator, */
/*                                          aAvailableSize, */
/*                                          aValue, */
/*                                          aSuccessWithInfo, */
/*                                          aVectorFunc, */
/*                                          aVectorArg, */
/*                                          aErrorStack ); */
/* } */


/* stlStatus dtlLongvarbinarySetValueFromReal( stlFloat64             aReal, */
/*                                             stlInt64               aPrecision, */
/*                                             stlInt64               aScale, */
/*                                             dtlStringLengthUnit    aStringUnit, */
/*                                             dtlIntervalIndicator   aIntervalIndicator, */
/*                                             stlInt64               aAvailableSize, */
/*                                             dtlDataValue         * aValue, */
/*                                             stlBool              * aSuccessWithInfo, */
/*                                             dtlFuncVector        * aVectorFunc, */
/*                                             void                 * aVectorArg, */
/*                                             stlErrorStack        * aErrorStack ) */
/* { */
/*     return dtdLongvarbinarySetValueFromReal( aReal, */
/*                                              aPrecision, */
/*                                              aScale, */
/*                                              aStringUnit, */
/*                                              aIntervalIndicator, */
/*                                              aAvailableSize, */
/*                                              aValue, */
/*                                              aSuccessWithInfo, */
/*                                              aVectorFunc, */
/*                                              aVectorArg, */
/*                                              aErrorStack ); */
/* } */


/**
 * Set Value From WCharString
 */

stlStatus dtlBooleanSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                             void                * aString,
                                             stlInt64              aStringLength,
                                             stlInt64              aPrecision,
                                             stlInt64              aScale,
                                             dtlStringLengthUnit   aStringUnit,
                                             dtlIntervalIndicator  aIntervalIndicator,
                                             stlInt64              aAvailableSize,
                                             dtlDataValue        * aValue,
                                             stlBool             * aSuccessWithInfo,
                                             dtlFuncVector       * aVectorFunc,
                                             void                * aVectorArg,
                                             stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_BOOLEAN ][ aUnicodeEncoding ]( 
               aString, 
               aStringLength,
               aPrecision,
               aScale,
               aStringUnit,
               aIntervalIndicator,
               aAvailableSize,
               aValue,
               aSuccessWithInfo,
               aVectorFunc,
               aVectorArg,
               aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlNumericSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                             void                * aString,
                                             stlInt64              aStringLength,
                                             stlInt64              aPrecision,
                                             stlInt64              aScale,
                                             dtlStringLengthUnit   aStringUnit,
                                             dtlIntervalIndicator  aIntervalIndicator,
                                             stlInt64              aAvailableSize,
                                             dtlDataValue        * aValue,
                                             stlBool             * aSuccessWithInfo,
                                             dtlFuncVector       * aVectorFunc,
                                             void                * aVectorArg,
                                             stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_NUMBER ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlFloatSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                           void                * aString,
                                           stlInt64              aStringLength,
                                           stlInt64              aPrecision,
                                           stlInt64              aScale,
                                           dtlStringLengthUnit   aStringUnit,
                                           dtlIntervalIndicator  aIntervalIndicator,
                                           stlInt64              aAvailableSize,
                                           dtlDataValue        * aValue,
                                           stlBool             * aSuccessWithInfo,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_FLOAT ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlCharSetValueFromWCharStringWithoutPaddNull( dtlUnicodeEncoding    aUnicodeEncoding,
                                                         void                * aString,
                                                         stlInt64              aStringLength,
                                                         stlInt64              aPrecision,
                                                         stlInt64              aScale,
                                                         dtlStringLengthUnit   aStringUnit,
                                                         dtlIntervalIndicator  aIntervalIndicator,
                                                         stlInt64              aAvailableSize,
                                                         dtlDataValue        * aValue,
                                                         stlBool             * aSuccessWithInfo,
                                                         dtlFuncVector       * aVectorFunc,
                                                         void                * aVectorArg,
                                                         stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_VARCHAR ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlVarcharSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                             void                * aString,
                                             stlInt64              aStringLength,
                                             stlInt64              aPrecision,
                                             stlInt64              aScale,
                                             dtlStringLengthUnit   aStringUnit,
                                             dtlIntervalIndicator  aIntervalIndicator,
                                             stlInt64              aAvailableSize,
                                             dtlDataValue        * aValue,
                                             stlBool             * aSuccessWithInfo,
                                             dtlFuncVector       * aVectorFunc,
                                             void                * aVectorArg,
                                             stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_VARCHAR ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlLongvarcharSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                                 void                * aString,
                                                 stlInt64              aStringLength,
                                                 stlInt64              aPrecision,
                                                 stlInt64              aScale,
                                                 dtlStringLengthUnit   aStringUnit,
                                                 dtlIntervalIndicator  aIntervalIndicator,
                                                 stlInt64              aAvailableSize,
                                                 dtlDataValue        * aValue,
                                                 stlBool             * aSuccessWithInfo,
                                                 dtlFuncVector       * aVectorFunc,
                                                 void                * aVectorArg,
                                                 stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_LONGVARCHAR ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlBinarySetValueFromWCharStringWithoutPaddNull( dtlUnicodeEncoding    aUnicodeEncoding,
                                                           void                * aString,
                                                           stlInt64              aStringLength,
                                                           stlInt64              aPrecision,
                                                           stlInt64              aScale,
                                                           dtlStringLengthUnit   aStringUnit,
                                                           dtlIntervalIndicator  aIntervalIndicator,
                                                           stlInt64              aAvailableSize,
                                                           dtlDataValue        * aValue,
                                                           stlBool             * aSuccessWithInfo,
                                                           dtlFuncVector       * aVectorFunc,
                                                           void                * aVectorArg,
                                                           stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_VARBINARY ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlVarbinarySetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                               void                * aString,
                                               stlInt64              aStringLength,
                                               stlInt64              aPrecision,
                                               stlInt64              aScale,
                                               dtlStringLengthUnit   aStringUnit,
                                               dtlIntervalIndicator  aIntervalIndicator,
                                               stlInt64              aAvailableSize,
                                               dtlDataValue        * aValue,
                                               stlBool             * aSuccessWithInfo,
                                               dtlFuncVector       * aVectorFunc,
                                               void                * aVectorArg,
                                               stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_VARBINARY ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlLongvarbinarySetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                                   void                * aString,
                                                   stlInt64              aStringLength,
                                                   stlInt64              aPrecision,
                                                   stlInt64              aScale,
                                                   dtlStringLengthUnit   aStringUnit,
                                                   dtlIntervalIndicator  aIntervalIndicator,
                                                   stlInt64              aAvailableSize,
                                                   dtlDataValue        * aValue,
                                                   stlBool             * aSuccessWithInfo,
                                                   dtlFuncVector       * aVectorFunc,
                                                   void                * aVectorArg,
                                                   stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_LONGVARBINARY ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlDateSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                          void                * aString,
                                          stlInt64              aStringLength,
                                          stlInt64              aPrecision,
                                          stlInt64              aScale,
                                          dtlStringLengthUnit   aStringUnit,
                                          dtlIntervalIndicator  aIntervalIndicator,
                                          stlInt64              aAvailableSize,
                                          dtlDataValue        * aValue,
                                          stlBool             * aSuccessWithInfo,
                                          dtlFuncVector       * aVectorFunc,
                                          void                * aVectorArg,
                                          stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_DATE ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlTimeSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                          void                * aString,
                                          stlInt64              aStringLength,
                                          stlInt64              aPrecision,
                                          stlInt64              aScale,
                                          dtlStringLengthUnit   aStringUnit,
                                          dtlIntervalIndicator  aIntervalIndicator,
                                          stlInt64              aAvailableSize,
                                          dtlDataValue        * aValue,
                                          stlBool             * aSuccessWithInfo,
                                          dtlFuncVector       * aVectorFunc,
                                          void                * aVectorArg,
                                          stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_TIME ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlTimestampSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                               void                * aString,
                                               stlInt64              aStringLength,
                                               stlInt64              aPrecision,
                                               stlInt64              aScale,
                                               dtlStringLengthUnit   aStringUnit,
                                               dtlIntervalIndicator  aIntervalIndicator,
                                               stlInt64              aAvailableSize,
                                               dtlDataValue        * aValue,
                                               stlBool             * aSuccessWithInfo,
                                               dtlFuncVector       * aVectorFunc,
                                               void                * aVectorArg,
                                               stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_TIMESTAMP ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlTimeTzSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                            void                * aString,
                                            stlInt64              aStringLength,
                                            stlInt64              aPrecision,
                                            stlInt64              aScale,
                                            dtlStringLengthUnit   aStringUnit,
                                            dtlIntervalIndicator  aIntervalIndicator,
                                            stlInt64              aAvailableSize,
                                            dtlDataValue        * aValue,
                                            stlBool             * aSuccessWithInfo,
                                            dtlFuncVector       * aVectorFunc,
                                            void                * aVectorArg,
                                            stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_TIME_WITH_TIME_ZONE ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlTimestampTzSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                                 void                * aString,
                                                 stlInt64              aStringLength,
                                                 stlInt64              aPrecision,
                                                 stlInt64              aScale,
                                                 dtlStringLengthUnit   aStringUnit,
                                                 dtlIntervalIndicator  aIntervalIndicator,
                                                 stlInt64              aAvailableSize,
                                                 dtlDataValue        * aValue,
                                                 stlBool             * aSuccessWithInfo,
                                                 dtlFuncVector       * aVectorFunc,
                                                 void                * aVectorArg,
                                                 stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlIntervalYearToMonthSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                                         void                * aString,
                                                         stlInt64              aStringLength,
                                                         stlInt64              aPrecision,
                                                         stlInt64              aScale,
                                                         dtlStringLengthUnit   aStringUnit,
                                                         dtlIntervalIndicator  aIntervalIndicator,
                                                         stlInt64              aAvailableSize,
                                                         dtlDataValue        * aValue,
                                                         stlBool             * aSuccessWithInfo,
                                                         dtlFuncVector       * aVectorFunc,
                                                         void                * aVectorArg,
                                                         stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_INTERVAL_YEAR_TO_MONTH ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlIntervalDayToSecondSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                                         void                * aString,
                                                         stlInt64              aStringLength,
                                                         stlInt64              aPrecision,
                                                         stlInt64              aScale,
                                                         dtlStringLengthUnit   aStringUnit,
                                                         dtlIntervalIndicator  aIntervalIndicator,
                                                         stlInt64              aAvailableSize,
                                                         dtlDataValue        * aValue,
                                                         stlBool             * aSuccessWithInfo,
                                                         dtlFuncVector       * aVectorFunc,
                                                         void                * aVectorArg,
                                                         stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_INTERVAL_DAY_TO_SECOND ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtlRowIdSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                           void                * aString,
                                           stlInt64              aStringLength,
                                           stlInt64              aPrecision,
                                           stlInt64              aScale,
                                           dtlStringLengthUnit   aStringUnit,
                                           dtlIntervalIndicator  aIntervalIndicator,
                                           stlInt64              aAvailableSize,
                                           dtlDataValue        * aValue,
                                           stlBool             * aSuccessWithInfo,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( (aUnicodeEncoding >= 0) && 
                        (aUnicodeEncoding < DTL_UNICODE_MAX),
                        aErrorStack );

    STL_TRY( dtdSetValueFromWCharStringFuncList[ DTL_TYPE_ROWID ][ aUnicodeEncoding ]( 
                aString, 
                aStringLength,
                aPrecision,
                aScale,
                aStringUnit,
                aIntervalIndicator,
                aAvailableSize,
                aValue,
                aSuccessWithInfo,
                aVectorFunc,
                aVectorArg,
                aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Key Compare 가능한 Type인지 확인
 * @param[in]  aType         Data Type
 * 
 * @return Key Compare가 유효한지 여부
 */
stlBool dtlCheckKeyCompareType( dtlDataType  aType )
{
    switch( aType )
    {
        case DTL_TYPE_LONGVARCHAR :
        case DTL_TYPE_CLOB :
        case DTL_TYPE_LONGVARBINARY :
        case DTL_TYPE_BLOB :
            {
                return STL_FALSE;
            }
        default:
            {
                STL_DASSERT( aType < DTL_TYPE_MAX );
                return STL_TRUE;
            }
    }
}


/**
 * @brief Comparison Result Type Combination Rule 에 따른 Result Type 및 Def Info 결정
 * @param[in]  aDataTypeArrayCount    arg count
 * @param[in]  aDataTypeArray         function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray  aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[out] aResultType            Result DataType
 * @param[out] aResultDefInfo         Result DataType Def Info
 * @param[in]  aErrorStack            environment (stlErrorStack)
 * @remarks
 * - greatest(), least(), nvl(), nvl2(), case2(), decode() 에서 사용한다.
 */
stlStatus dtlGetResultTypeForComparisonRule( stlUInt16               aDataTypeArrayCount,
                                             dtlDataType           * aDataTypeArray,
                                             dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                             dtlDataType           * aResultType,
                                             dtlDataTypeDefInfo    * aResultDefInfo,
                                             stlErrorStack         * aErrorStack )
{
    stlInt32                sIndex         = 0;
    dtlGroup                sArgGroup      = DTL_GROUP_MAX;
    dtlIntervalIndicator    sTmpIndicator  = DTL_INTERVAL_INDICATOR_MAX;
    dtlDataType             sTmpType       = DTL_TYPE_MAX;
    stlChar                 sIntervalDataTypeString[ DTL_DATA_TYPE_NAME_MAX_SIZE ];

    STL_PARAM_VALIDATE( aDataTypeArray != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aDataTypeDefInfoArray != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aResultType != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aResultDefInfo != NULL, aErrorStack );

    /**
     * init result data type definition info
     */
    DTL_INIT_DATA_TYPE_DEF_INFO( aResultDefInfo );


    /**
    * set result data type definition info
    */
    switch( aDataTypeArray[ 0 ] )
    {
        case DTL_TYPE_BOOLEAN:
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
        case DTL_TYPE_FLOAT:
        case DTL_TYPE_NUMBER:
        case DTL_TYPE_CHAR:
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_LONGVARCHAR:
        case DTL_TYPE_BINARY:
        case DTL_TYPE_VARBINARY:
        case DTL_TYPE_LONGVARBINARY:
        case DTL_TYPE_DATE:
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
        case DTL_TYPE_ROWID:

            *aResultType = aDataTypeArray[ 0 ];
            DTL_COPY_DATA_TYPE_DEF_INFO( aResultDefInfo,
                                         & aDataTypeDefInfoArray[ 0 ] );
            
            for( sIndex = 1; sIndex < aDataTypeArrayCount; sIndex++ )
            {
                /* 오류시 Expected DataType 출력용 */
                sTmpType = *aResultType;
                sTmpIndicator = aResultDefInfo->mIntervalIndicator;
                
                dtlComparisonResultType( aResultType,
                                         aResultDefInfo,
                                         *aResultType,
                                         aResultDefInfo,
                                         aDataTypeArray[ sIndex ],
                                         & aDataTypeDefInfoArray[ sIndex ] );

                STL_TRY_THROW( *aResultType != DTL_TYPE_NA, ERROR_INCONSISTENT_DATATYPES ); 
            }

            /**
             * aResultType , aResultDefInfo  결정 후
             * aDataTypeArray 가 aFuncResultDataTypeDefInfo 로 변환 가능 여부를 확인,
             * ex ) greatest( interval_year , integer , interval_month )
             *     -> interval_year , interval_month ( O )
             *     -> interval_year_to_month ( O )
             *     -> 이경우 interval_year_to_month , integer 는 변환 가능하지 않음 ( X )
             */

            sTmpType = *aResultType;
            sTmpIndicator = aResultDefInfo->mIntervalIndicator;
            for( sIndex = 0; sIndex < aDataTypeArrayCount; sIndex++ )
            {
                STL_TRY_THROW(
                    dtlIsApplicableCast( *aResultType,
                                         aResultDefInfo->mIntervalIndicator,
                                         aDataTypeArray[ sIndex ],
                                         aDataTypeDefInfoArray[ sIndex ].mIntervalIndicator )
                    == STL_TRUE,
                    ERROR_INCONSISTENT_DATATYPES );


            }
            break;
            
        case DTL_TYPE_DECIMAL:
        case DTL_TYPE_CLOB:
        case DTL_TYPE_BLOB:
                                    
        default:
            STL_THROW( ERROR_INCONSISTENT_DATATYPE_FUNCTION_PARAMETERS );

            break;
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INCONSISTENT_DATATYPE_FUNCTION_PARAMETERS )
    {
        /* "inconsistent datatype of function parameters: %s" */
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INCONSISTENT_DATATYPE_FUNCTION_PARAMETERS,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aDataTypeArray[ 0 ] ] );
    }

    STL_CATCH( ERROR_INCONSISTENT_DATATYPES )
    {
        /* "inconsistent datatypes: argument[%d] expected %s" */

        sArgGroup = dtlDataTypeGroup[ sTmpType ];
        switch( sArgGroup )
        {
            case DTL_GROUP_INTERVAL_YEAR_TO_MONTH:
            case DTL_GROUP_INTERVAL_DAY_TO_SECOND:

                stlSnprintf( sIntervalDataTypeString,
                             DTL_DATA_TYPE_NAME_MAX_SIZE,
                             "INTERVAL %s",
                             gDtlIntervalIndicatorString[ sTmpIndicator ] );

                stlPushError( STL_ERROR_LEVEL_ABORT,
                              DTL_ERRCODE_INCONSISTENT_DATATYPES,
                              NULL,
                              aErrorStack,
                              sIndex + 1,
                              sIntervalDataTypeString );
                break;

            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              DTL_ERRCODE_INCONSISTENT_DATATYPES,
                              NULL,
                              aErrorStack,
                              sIndex +1 ,
                              dtlDataTypeName[ sTmpType ] );
                break;
        }
    }

    STL_FINISH;
               
    return STL_FAILURE;    
}


/**
 * @brief Comparison Result Type Combination Rule 에 따른 Result Type 및 Def Info 결정
 * @param[out] aResultType    Result DataType
 * @param[out] aResultDefInfo Result DataType Def Info
 * @param[in]  aLeftType      Left DataType
 * @param[in]  aLeftDefInfo   Left DataType Def Info
 * @param[in]  aRightType     Right DataType
 * @param[in]  aRightDefInfo  Right DataType Def Info
 * @remarks
 * 비교 연산 함수에서 사용된다.
 * - greatest(), least()
 */
void dtlComparisonResultType( dtlDataType        * aResultType,
                              dtlDataTypeDefInfo * aResultDefInfo,
                              dtlDataType          aLeftType,
                              dtlDataTypeDefInfo * aLeftDefInfo,
                              dtlDataType          aRightType,
                              dtlDataTypeDefInfo * aRightDefInfo )
{
    dtlDataTypeDefInfo    sResultDefInfo;
    
    stlInt64             sLeftArgNum1 = 0;           
    stlInt64             sLeftArgNum2 = 0;           
    
    stlInt64             sRightArgNum1 = 0;           
    stlInt64             sRightArgNum2 = 0;

    STL_DASSERT( aResultType != NULL );
    STL_DASSERT( aResultDefInfo != NULL );
    STL_DASSERT( aLeftDefInfo != NULL );
    STL_DASSERT( aRightDefInfo != NULL );

    stlMemset( & sResultDefInfo, 0x00, STL_SIZEOF( dtlDataTypeDefInfo ) );

    sResultDefInfo.mArgNum1 = DTL_PRECISION_NA;
    sResultDefInfo.mArgNum2 = DTL_SCALE_NA;
    sResultDefInfo.mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
    sResultDefInfo.mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;


    *aResultType = dtlComparisonResultTypeCombination[ aLeftType ][ aRightType ];
    STL_TRY_THROW( *aResultType != DTL_TYPE_NA, RAMP_FINISH );

    /* casting 가능 여부 */
    STL_TRY_THROW( dtlIsApplicableCast( aLeftType,
                                        aLeftDefInfo->mIntervalIndicator,
                                        aRightType,
                                        aRightDefInfo->mIntervalIndicator ) == STL_TRUE,
                   RAMP_ERROR_INCONSISTENT_DATATYPES );

    switch( *aResultType )
    {
        case DTL_TYPE_BOOLEAN:
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
        case DTL_TYPE_LONGVARCHAR:
        case DTL_TYPE_LONGVARBINARY:
        case DTL_TYPE_FLOAT:
        case DTL_TYPE_NUMBER:
        case DTL_TYPE_DATE:
        case DTL_TYPE_ROWID:

            /* precison, scale, fraction 등을 고려하지 않는 result type */
            DTL_COPY_DATA_TYPE_DEF_INFO(
                & sResultDefInfo,
                & gResultDataTypeDef[ *aResultType ] );
            break;

        case DTL_TYPE_CHAR:
            sResultDefInfo.mArgNum1 =
                STL_MAX( aLeftDefInfo->mArgNum1, aRightDefInfo->mArgNum1 );

            if( (aLeftDefInfo->mArgNum1 == aRightDefInfo->mArgNum1) &&
                (aLeftDefInfo->mStringLengthUnit == aRightDefInfo->mStringLengthUnit) )
            {
                *aResultType = DTL_TYPE_CHAR;
            }
            else
            {
                *aResultType = DTL_TYPE_VARCHAR;
            }

            if ( (aLeftDefInfo->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS) ||
                 (aRightDefInfo->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS) )
            {
                sResultDefInfo.mStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
            }
            else
            {
                sResultDefInfo.mStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
            }
            
            break;
        case DTL_TYPE_VARCHAR:
            /* varchar-char
             * varchar-varchar */
            sResultDefInfo.mArgNum1 = STL_MAX( aLeftDefInfo->mArgNum1, aRightDefInfo->mArgNum1 );
            
            if ( (aLeftDefInfo->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS) ||
                 (aRightDefInfo->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS) )
            {
                sResultDefInfo.mStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
            }
            else
            {
                sResultDefInfo.mStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
            }
            break;

        case DTL_TYPE_BINARY:
            /* binary-binary */
            sResultDefInfo.mArgNum1 =
                STL_MAX( aLeftDefInfo->mArgNum1, aRightDefInfo->mArgNum1 );
            
            if( aLeftDefInfo->mArgNum1 == aRightDefInfo->mArgNum1 )
            {
                *aResultType = DTL_TYPE_BINARY;
            }
            else
            {
                *aResultType = DTL_TYPE_VARBINARY;
            }
            break;
            
        case DTL_TYPE_VARBINARY:
             /* varbinary-binary
              * varbinary-varbinary */
            sResultDefInfo.mArgNum1 = STL_MAX( aLeftDefInfo->mArgNum1, aRightDefInfo->mArgNum1 );
            break;

        case DTL_TYPE_TIME:
            /* time-time
             * time-char */

            switch( aLeftType )
            {
                case DTL_TYPE_TIME:
                    sLeftArgNum1 = aLeftDefInfo->mArgNum1;
                    break;

                default:
                    break;
            }

            switch( aRightType )
            {
                case DTL_TYPE_TIME:
                    sRightArgNum1 = aRightDefInfo->mArgNum1;
                    break;

                default:
                    break;
            }

            sResultDefInfo.mArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );
            break;

        case DTL_TYPE_TIMESTAMP:
            /* timestamp-char
             * timestamp-varchar
             * timestamp-longvarchar
             * timestamp-date
             * timestamp-timestamp */
            
            switch( aLeftType )
            {
                case DTL_TYPE_TIMESTAMP:
                    sLeftArgNum1 = aLeftDefInfo->mArgNum1;
                    break;

                default:
                    break;
            }

            switch( aRightType )
            {
                case DTL_TYPE_TIMESTAMP:
                    sRightArgNum1 = aRightDefInfo->mArgNum1;
                    break;

                default:
                    break;
            }

            sResultDefInfo.mArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );
            break;

        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            /* time_tz-char
             * time_tz-varchar
             * time_tz-longvarchar
             * time_tz-time
             * time_tz-time-tz */

            switch( aLeftType )
            {
                case DTL_TYPE_TIME:
                case DTL_TYPE_TIME_WITH_TIME_ZONE:
                    sLeftArgNum1 = aLeftDefInfo->mArgNum1;
                    break;

                default:
                    break;
            }

            switch( aRightType )
            {
                case DTL_TYPE_TIME:
                case DTL_TYPE_TIME_WITH_TIME_ZONE:
                    sRightArgNum1 = aRightDefInfo->mArgNum1;
                    break;

                default:
                    break;
            }

            sResultDefInfo.mArgNum1 = STL_MAX( sLeftArgNum1 , sRightArgNum1 );
            break;

        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            /* timestamp_tz-char
             * timestamp_tz-varchar
             * timestamp_tz-longvarchar
             * timestamp_tz-date,
             * timestamp_tz-timestamp,
             * timestamp_tz-timestamp_tz */

            switch( aLeftType )
            {
                case DTL_TYPE_TIMESTAMP:
                case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                    sLeftArgNum1 = aLeftDefInfo->mArgNum1;
                    break;
                    
                default:
                    break;
            }

            switch( aRightType )
            {
                case DTL_TYPE_TIMESTAMP:
                case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                    sRightArgNum1 = aRightDefInfo->mArgNum1;
                    break;
                    
                default:
                    break;
            }

            sResultDefInfo.mArgNum1 = STL_MAX( sLeftArgNum1 , sRightArgNum1 );
            break;

        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:

            /* number, char, native <-> interval */
            if( aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_NA )
            {
                DTL_COPY_DATA_TYPE_DEF_INFO(
                    & sResultDefInfo,
                    aRightDefInfo );
                break;
            }
            /* interval <-> number, char, native */
            else if( aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_NA )
            {
                DTL_COPY_DATA_TYPE_DEF_INFO(
                    & sResultDefInfo,
                    aLeftDefInfo );
                break;
            }
            else
            {
                /* do nothing */
            }
            
            sResultDefInfo.mIntervalIndicator =
                dtlIntervalIndicatorContainArgIndicator
                [ aLeftDefInfo->mIntervalIndicator ][ aRightDefInfo->mIntervalIndicator ];

            switch( sResultDefInfo.mIntervalIndicator )
            {
                case DTL_INTERVAL_INDICATOR_YEAR:
                case DTL_INTERVAL_INDICATOR_MONTH:
                    sResultDefInfo.mArgNum1 =
                        STL_MAX( aLeftDefInfo->mArgNum1, aRightDefInfo->mArgNum1 );
                    break;

                case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                    switch( aLeftDefInfo->mIntervalIndicator )
                    {
                        case DTL_INTERVAL_INDICATOR_YEAR:
                        case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                            sLeftArgNum1 = aLeftDefInfo->mArgNum1;
                            break;

                        default:
                            sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                            break;
                    }

                    switch( aRightDefInfo->mIntervalIndicator )
                    {
                        case DTL_INTERVAL_INDICATOR_YEAR:
                        case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                            sRightArgNum1 = aRightDefInfo->mArgNum1;
                            break;

                        default:
                            sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                            break;
                    }

                    sResultDefInfo.mArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );
                    break;

                default:
                    STL_DASSERT(0);
                    break;
            }
            break;

        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:

            if( aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_NA )
            {
                DTL_COPY_DATA_TYPE_DEF_INFO(
                    & sResultDefInfo,
                    aRightDefInfo );
                break;
            }
            else if( aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_NA )
            {
                DTL_COPY_DATA_TYPE_DEF_INFO(
                    & sResultDefInfo,
                    aLeftDefInfo );
                break;
            }
            else
            {
                /* do nothing */
            }
            
            sResultDefInfo.mIntervalIndicator =
                dtlIntervalIndicatorContainArgIndicator
                [ aLeftDefInfo->mIntervalIndicator ][ aRightDefInfo->mIntervalIndicator ];

            switch( sResultDefInfo.mIntervalIndicator )
            {
                case DTL_INTERVAL_INDICATOR_DAY:
                case DTL_INTERVAL_INDICATOR_HOUR:
                case DTL_INTERVAL_INDICATOR_MINUTE:
                    sResultDefInfo.mArgNum1 =
                        STL_MAX( aLeftDefInfo->mArgNum1, aRightDefInfo->mArgNum1 );
                    break;

                case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                    if( (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) )
                    {
                        sLeftArgNum1 = aLeftDefInfo->mArgNum1;
                    }
                    else
                    {
                        sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                    }

                    if( (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) )
                    {
                        sRightArgNum1 = aRightDefInfo->mArgNum1;
                    }
                    else
                    {
                        sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                    }
                            
                    sResultDefInfo.mArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );
                    break;

                case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                    if( (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
                    {
                        sLeftArgNum1 = aLeftDefInfo->mArgNum1;
                    }
                    else
                    {
                        sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                    }

                    if( (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
                    {
                        sRightArgNum1 = aRightDefInfo->mArgNum1;
                    }
                    else
                    {
                        sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                    }
                            
                    sResultDefInfo.mArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );
                    break;

                case DTL_INTERVAL_INDICATOR_SECOND:
                    sResultDefInfo.mArgNum1 = STL_MAX( aLeftDefInfo->mArgNum1, aRightDefInfo->mArgNum1 );
                    sResultDefInfo.mArgNum2 = STL_MAX( aLeftDefInfo->mArgNum2, aRightDefInfo->mArgNum2 );
                    break;

                case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                    if( (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) )
                    {
                        sLeftArgNum1 = aLeftDefInfo->mArgNum1;
                    }
                    else
                    {
                        sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                    }

                    if( (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) )
                    {
                        sRightArgNum1 = aRightDefInfo->mArgNum1;
                    }
                    else
                    {
                        sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                    }
                            
                    sResultDefInfo.mArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );

                    if( (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                    {
                        sLeftArgNum2 = aLeftDefInfo->mArgNum2;
                    }
                    else
                    {
                        sLeftArgNum2 = 0;
                    }

                    if( (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                    {
                        sRightArgNum2 = aRightDefInfo->mArgNum2;
                    }
                    else
                    {
                        sRightArgNum2 = 0;
                    }

                    sResultDefInfo.mArgNum2 = STL_MAX( sLeftArgNum2, sRightArgNum2 );
                            
                    break;

                case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                    if( (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) )
                    {
                        sLeftArgNum1 = aLeftDefInfo->mArgNum1;
                    }
                    else
                    {
                        sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                    }

                    if( (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) )
                    {
                        sRightArgNum1 = aRightDefInfo->mArgNum1;
                    }
                    else
                    {
                        sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                    }
                            
                    sResultDefInfo.mArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );

                    if( (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                    {
                        sLeftArgNum2 = aLeftDefInfo->mArgNum2;
                    }
                    else
                    {
                        sLeftArgNum2 = 0;
                    }

                    if( (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                    {
                        sRightArgNum2 = aRightDefInfo->mArgNum2;
                    }
                    else
                    {
                        sRightArgNum2 = 0;
                    }

                    sResultDefInfo.mArgNum2 = STL_MAX( sLeftArgNum2, sRightArgNum2 );
                            
                    break;
                            
                case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                    if( (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                    {
                        sLeftArgNum1 = aLeftDefInfo->mArgNum1;
                    }
                    else
                    {
                        sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                    }

                    if( (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                    {
                        sRightArgNum1 = aRightDefInfo->mArgNum1;
                    }
                    else
                    {
                        sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                    }
                            
                    sResultDefInfo.mArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );

                    if( (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                        (aLeftDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                    {
                        sLeftArgNum2 = aLeftDefInfo->mArgNum2;
                    }
                    else
                    {
                        sLeftArgNum2 = 0;
                    }

                    if( (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                        (aRightDefInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                    {
                        sRightArgNum2 = aRightDefInfo->mArgNum2;
                    }
                    else
                    {
                        sRightArgNum2 = 0;
                    }

                    sResultDefInfo.mArgNum2 = STL_MAX( sLeftArgNum2, sRightArgNum2 );
                            
                    break;

                default:
                    break;
            }
            break;
            
        default:
            break;

    }

    STL_THROW( RAMP_FINISH );

    STL_RAMP( RAMP_ERROR_INCONSISTENT_DATATYPES );

    *aResultType = DTL_TYPE_NA;

    STL_RAMP( RAMP_FINISH );

    DTL_COPY_DATA_TYPE_DEF_INFO( aResultDefInfo, & sResultDefInfo );
    
    
}


/**
 * @brief Data Type에 따른 Data Type 속성 검사
 * @param[in]  aType               Data Type
 * @param[in]  aPrecision          Precision
 * @param[in]  aScale              Scale
 * @param[in]  aStringLengthUnit   String Length Unit
 * @param[in]  aIntervalIndicator  INTERVAL 타입의 Indicator
 * @param[out] aErrorStack         에러 스택
 */
stlStatus dtlVaildateDataTypeInfo( dtlDataType             aType,
                                   stlInt64                aPrecision,
                                   stlInt64                aScale,
                                   dtlStringLengthUnit     aStringLengthUnit,
                                   dtlIntervalIndicator    aIntervalIndicator,
                                   stlErrorStack         * aErrorStack )
{

    /**
     * Validate Data Type
     */
    
    STL_TRY_THROW( ( aType >= 0 ) && ( aType < DTL_TYPE_MAX ), RAMP_ERR_DATA_TYPE_INVALID );


    /**
     * Data Type 속성 검사
     */

    switch( aType )
    {
        case DTL_TYPE_BOOLEAN :
        case DTL_TYPE_NATIVE_SMALLINT :
        case DTL_TYPE_NATIVE_INTEGER :
        case DTL_TYPE_NATIVE_BIGINT :
        case DTL_TYPE_NATIVE_REAL :
        case DTL_TYPE_NATIVE_DOUBLE :
        case DTL_TYPE_ROWID :
        case DTL_TYPE_LONGVARCHAR :
        case DTL_TYPE_LONGVARBINARY :
        case DTL_TYPE_DATE :
            {
                /* Do Nothing */
                break;
            }
            
        case DTL_TYPE_FLOAT :
            {
                STL_TRY_THROW( ( aPrecision >= gDataTypeInfoDomain[ aType ].mMinArgNum1 ) &&
                               ( aPrecision <= gDataTypeInfoDomain[ aType ].mMaxArgNum1 ),
                               RAMP_ERR_PRECISION_INVALID );

                break;
            }

        case DTL_TYPE_NUMBER :
            {
                STL_TRY_THROW( ( aPrecision >= gDataTypeInfoDomain[ aType ].mMinArgNum1 ) &&
                               ( aPrecision <= gDataTypeInfoDomain[ aType ].mMaxArgNum1 ),
                               RAMP_ERR_PRECISION_INVALID );

                STL_TRY_THROW( ( ( ( aScale >= gDataTypeInfoDomain[ aType ].mMinArgNum2 ) &&
                                   ( aScale <= gDataTypeInfoDomain[ aType ].mMaxArgNum2 ) ) ||
                                 ( aScale == DTL_SCALE_NA ) ),
                               RAMP_ERR_SCALE_INVALID );

                break;
            }
    
        case DTL_TYPE_CHAR :
        case DTL_TYPE_VARCHAR :
            {
                STL_TRY_THROW( ( aPrecision >= gDataTypeInfoDomain[ aType ].mMinArgNum1 ) &&
                               ( aPrecision <= gDataTypeInfoDomain[ aType ].mMaxArgNum1 ),
                               RAMP_ERR_PRECISION_INVALID );

                STL_TRY_THROW( ( aStringLengthUnit >= gDataTypeInfoDomain[ aType ].mMinStringLengthUnit ) &&
                               ( aStringLengthUnit <= gDataTypeInfoDomain[ aType ].mMaxStringLengthUnit ),
                               RAMP_ERR_CHARACTER_LENGTH_UNIT_INVALID );

                break;
            }

        case DTL_TYPE_BINARY :
        case DTL_TYPE_VARBINARY :
            {
                STL_TRY_THROW( ( aPrecision >= gDataTypeInfoDomain[ aType ].mMinArgNum1 ) &&
                               ( aPrecision <= gDataTypeInfoDomain[ aType ].mMaxArgNum1 ),
                               RAMP_ERR_PRECISION_INVALID );
                
                break;
            }
            
        case DTL_TYPE_TIME :
        case DTL_TYPE_TIMESTAMP :
        case DTL_TYPE_TIME_WITH_TIME_ZONE :
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE :
            {
                STL_TRY_THROW( ( aPrecision >= gDataTypeInfoDomain[ aType ].mMinArgNum1 ) &&
                               ( aPrecision <= gDataTypeInfoDomain[ aType ].mMaxArgNum1 ),
                               RAMP_ERR_PRECISION_INVALID );

                break;
            }

        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH :
            {
                STL_TRY_THROW( ( aPrecision >= gDataTypeInfoDomain[ aType ].mMinArgNum1 ) &&
                               ( aPrecision <= gDataTypeInfoDomain[ aType ].mMaxArgNum1 ),
                               RAMP_ERR_LEADING_PRECISION_INVALID );

                STL_TRY_THROW( ( aIntervalIndicator >= gDataTypeInfoDomain[ aType ].mMinIntervalIndicator ) &&
                               ( aIntervalIndicator <= gDataTypeInfoDomain[ aType ].mMaxIntervalIndicator ),
                               RAMP_ERR_INTERVAL_INDICATOR_INVALID );

                break;
            }
            
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND :
            {
                STL_TRY_THROW( ( aPrecision >= gDataTypeInfoDomain[ aType ].mMinArgNum1 ) &&
                               ( aPrecision <= gDataTypeInfoDomain[ aType ].mMaxArgNum1 ),
                               RAMP_ERR_LEADING_PRECISION_INVALID );

                STL_TRY_THROW( ( ( ( aScale >= gDataTypeInfoDomain[ aType ].mMinArgNum2 ) &&
                                   ( aScale <= gDataTypeInfoDomain[ aType ].mMaxArgNum2 ) ) ||
                                 ( aScale == DTL_SCALE_NA ) ),
                               RAMP_ERR_FRACTIONAL_SECOND_PRECISION_INVALID );

                STL_TRY_THROW( ( aIntervalIndicator >= gDataTypeInfoDomain[ aType ].mMinIntervalIndicator ) &&
                               ( aIntervalIndicator <= gDataTypeInfoDomain[ aType ].mMaxIntervalIndicator ),
                               RAMP_ERR_INTERVAL_INDICATOR_INVALID );

                break;
            }

        case DTL_TYPE_DECIMAL :
        case DTL_TYPE_CLOB :
        case DTL_TYPE_BLOB :
            {
                STL_THROW( RAMP_ERR_NOT_SUPPORTED_DATA_TYPE );
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORTED_DATA_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NOT_SUPPORTED_DATA_TYPE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aType ] );
    }

    STL_CATCH( RAMP_ERR_DATA_TYPE_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATA_TYPE_INVALID,
                      NULL,
                      aErrorStack,
                      aType,
                      0,
                      DTL_TYPE_MAX - 1 );
    }

    STL_CATCH( RAMP_ERR_PRECISION_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_PRECISION_INVALID,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aType ],
                      aPrecision,
                      gDataTypeInfoDomain[ aType ].mMinArgNum1,
                      gDataTypeInfoDomain[ aType ].mMaxArgNum1 );
    }

    STL_CATCH( RAMP_ERR_LEADING_PRECISION_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_LEADING_PRECISION_INVALID,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aType ],
                      aPrecision,
                      gDataTypeInfoDomain[ aType ].mMinArgNum1,
                      gDataTypeInfoDomain[ aType ].mMaxArgNum1 );
    }

    STL_CATCH( RAMP_ERR_SCALE_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SCALE_INVALID,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aType ],
                      aScale,
                      gDataTypeInfoDomain[ aType ].mMinArgNum2,
                      gDataTypeInfoDomain[ aType ].mMaxArgNum2 );
    }

    STL_CATCH( RAMP_ERR_FRACTIONAL_SECOND_PRECISION_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_FRACTIONAL_SECOND_PRECISION_INVALID,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aType ],
                      aScale,
                      gDataTypeInfoDomain[ aType ].mMinArgNum2,
                      gDataTypeInfoDomain[ aType ].mMaxArgNum2 );
    }

    STL_CATCH( RAMP_ERR_CHARACTER_LENGTH_UNIT_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_CHARACTER_LENGTH_UNIT_INVALID,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aType ],
                      aStringLengthUnit,
                      gDataTypeInfoDomain[ aType ].mMinStringLengthUnit,
                      gDataTypeInfoDomain[ aType ].mMaxStringLengthUnit );
    }

    STL_CATCH( RAMP_ERR_INTERVAL_INDICATOR_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERVAL_INDICATOR_INVALID,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aType ],
                      aIntervalIndicator,
                      gDataTypeInfoDomain[ aType ].mMinIntervalIndicator,
                      gDataTypeInfoDomain[ aType ].mMaxIntervalIndicator );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */

