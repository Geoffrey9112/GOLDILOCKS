/*******************************************************************************
 * eldtTYPE_INFO.c
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
 * @file eldtTYPE_INFO.c
 * @brief TYPE_INFO dictionary table 
 */

#include <ell.h>

#include <eldt.h>

/**
 * @addtogroup eldtTYPE_INFO
 * @internal
 * @{
 */

/**
 * @brief DEFINITION_SCHEMA.TYPE_INFO 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescTYPE_INFO[ELDT_TypeInfo_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_TYPE_INFO,             /**< Table ID */
        "TYPE_NAME",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_TYPE_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,          /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "the name of the data type"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                  /**< Table ID */
        "ODBC_DATA_TYPE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_ODBC_DATA_TYPE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,            /**< 컬럼의 Nullable 여부 */
        "concise data type"        
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                  /**< Table ID */
        "JDBC_DATA_TYPE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_JDBC_DATA_TYPE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,            /**< 컬럼의 Nullable 여부 */
        "the JDBC (SQL) data type from java.sql.Types that corresponds to "
        "the local data type in column TYPE_NAME"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,               /**< Table ID */
        "COLUMN_SIZE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_COLUMN_SIZE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,           /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                     /**< 컬럼의 Nullable 여부 */
        "The maximum column size that the server supports for this data type"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                  /**< Table ID */
        "LITERAL_PREFIX",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_LITERAL_PREFIX, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,               /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                        /**< 컬럼의 Nullable 여부 */
        "the prefix used to quote a literal"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                  /**< Table ID */
        "LITERAL_SUFFIX",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_LITERAL_SUFFIX, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,               /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                        /**< 컬럼의 Nullable 여부 */
        "the suffix  used to quote a literal"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                 /**< Table ID */
        "CREATE_PARAMS",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_CREATE_PARAMS, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_SYNTAX,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "the parameters used in creating the type"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,            /**< Table ID */
        "NULLABLE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_NULLABLE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,        /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,      /**< 컬럼의 Nullable 여부 */
        "whether a column can be NULL"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                  /**< Table ID */
        "CASE_SENSITIVE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_CASE_SENSITIVE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                    /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,            /**< 컬럼의 Nullable 여부 */
        "the type is case sensitive"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,              /**< Table ID */
        "SEARCHABLE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_SEARCHABLE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,          /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,        /**< 컬럼의 Nullable 여부 */
        "whether it is possible to use a WHERE clause based on this type"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                      /**< Table ID */
        "UNSIGNED_ATTRIBUTE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_UNSIGNED_ATTRIBUTE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                        /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                            /**< 컬럼의 Nullable 여부 */
        "the type is unsigned"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                    /**< Table ID */
        "FIXED_PREC_SCALE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_FIXED_PREC_SCALE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                      /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "whether the data type has predefined fixed precision and scale"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                     /**< Table ID */
        "AUTO_UNIQUE_VALUE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_AUTO_UNIQUE_VALUE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                       /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                           /**< 컬럼의 Nullable 여부 */
        "whether the data type is autoincrementing"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                   /**< Table ID */
        "LOCAL_TYPE_NAME",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_LOCAL_TYPE_NAME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,                /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                         /**< 컬럼의 Nullable 여부 */
        "localized version of the data name of the data type"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                 /**< Table ID */
        "MINIMUM_SCALE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_MINIMUM_SCALE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "the minimum scale"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                 /**< Table ID */
        "MAXIMUM_SCALE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_MAXIMUM_SCALE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                       /**< 컬럼의 Nullable 여부 */
        "the maximum scale"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                 /**< Table ID */
        "SQL_DATA_TYPE",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_SQL_DATA_TYPE, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "SQL data type"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                    /**< Table ID */
        "SQL_DATETIME_SUB",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_SQL_DATETIME_SUB, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                          /**< 컬럼의 Nullable 여부 */
        "the subcode for the specific interval or datetime data type"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                  /**< Table ID */
        "NUM_PREC_RADIX",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_NUM_PREC_RADIX, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                        /**< 컬럼의 Nullable 여부 */
        "the radix of precision"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                      /**< Table ID */
        "INTERVAL_PRECISION",                         /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_INTERVAL_PRECISION, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_CARDINAL_NUMBER,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                            /**< 컬럼의 Nullable 여부 */
        "the value of the interval leading precision"
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,                /**< Table ID */
        "IS_SUPPORTED",                          /**< 컬럼의 이름  */
        ELDT_TypeInfo_ColumnOrder_IS_SUPPORTED, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                   /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,           /**< 컬럼의 Nullable 여부 */
        "is supported"
    }
};


/**
 * @brief DEFINITION_SCHEMA.TYPE_INFO 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescTYPE_INFO =
{
    ELDT_TABLE_ID_TYPE_INFO, /**< Table ID */
    "TYPE_INFO",             /**< 테이블의 이름  */
    "The TYPE_INFO table contains one row for each built-in data types. "
    "This table is not declared in SQL standard."
};

/**
 * @brief DEFINITION_SCHEMA.TYPE_INFO 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescTYPE_INFO[ELDT_TypeInfo_Const_MAX] =
{
    {
        ELDT_TABLE_ID_TYPE_INFO,               /**< Table ID */
        ELDT_TypeInfo_Const_PRIMARY_KEY,       /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY, /**< Table Constraint 유형 */
        1,                                     /**< 키를 구성하는 컬럼의 개수 */
        {                                  /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TypeInfo_ColumnOrder_TYPE_NAME,
        },
        ELL_DICT_OBJECT_ID_NA,                  /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                  /**< 참조 제약이 참조하는 Unique의 번호 */
    },
};

/**
 * @brief DEFINITION_SCHEMA.TYPE_INFO 의 부가적 INDEX
 */
eldtDefinitionIndexDesc  gEldtIndexDescTYPE_INFO[ELDT_TypeInfo_Index_MAX] =
{
    {
        ELDT_TABLE_ID_TYPE_INFO,           /**< Table ID */
        ELDT_TypeInfo_Index_ODBC_DATA_TYPE,/**< Table 내 Index 번호 */
        1,                                 /**< 키를 구성하는 컬럼의 개수 */
        {                                  /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TypeInfo_ColumnOrder_ODBC_DATA_TYPE,
        }
    },
    {
        ELDT_TABLE_ID_TYPE_INFO,           /**< Table ID */
        ELDT_TypeInfo_Index_JDBC_DATA_TYPE,/**< Table 내 Index 번호 */
        1,                                 /**< 키를 구성하는 컬럼의 개수 */
        {                                  /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_TypeInfo_ColumnOrder_JDBC_DATA_TYPE,
        }
    }
};




/**
 * @brief Built-In Data Type 명세를 추가한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertBuiltInTypeInfoDesc( smlTransId     aTransID,
                                         smlStatement * aStmt,
                                         ellEnv       * aEnv )
{
    dtlDataType sType = DTL_TYPE_MAX;
    stlInt32    i = 0;

    stlChar * sTypeName     = NULL;
    stlInt32  sODBCDataType = 0;
    stlInt32  sJDBCDataType = 0;

    stlInt32  sColumnSize = DTL_PRECISION_NA;

    stlChar * sLiteralPrefix = NULL;
    stlChar * sLiteralSuffix = NULL;
    stlChar * sCreateParams  = NULL;

    stlInt32  sNullable      = 1; /* SQL_NULLABLE */
    stlBool   sCaseSensitive = STL_FALSE;

    dtlSearchable                 sSearchable        = DTL_SEARCHABLE_PRED_NONE;
    eldtTypeInfoUnsignedAttribute sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
    stlBool                       sFixedPrecScale    = STL_FALSE;
    eldtTypeInfoAutoUniqueValue   sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;

    stlInt32  sMinimumScale = DTL_SCALE_NA;
    stlInt32  sMaximumScale = DTL_SCALE_NA;

    stlInt32  sSQLDataType                     = 0;
    eldtTypeInfoSqlDateTimeSub sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;
    stlInt32  sIntervalPrecision               = DTL_PRECISION_NA;

    dtlNumericPrecRadix sNumPrecRadix = DTL_NUMERIC_PREC_RADIX_NA;
    
    stlBool sSupportedFeature = STL_FALSE;

    dtlDataTypeDefinition * sDataTypeDefinition;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    for ( sType = 0; sType < DTL_TYPE_MAX; sType++ )
    {
        sDataTypeDefinition = &gDataTypeDefinition[sType];
        
        sTypeName          = sDataTypeDefinition->mSqlNormalTypeName;
        sODBCDataType      = sDataTypeDefinition->mODBCDataTypeID;
        sSQLDataType       = sDataTypeDefinition->mODBCDataTypeID;
        sJDBCDataType      = sDataTypeDefinition->mJDBCDataTypeID;
        sSearchable        = sDataTypeDefinition->mSearchable;
        sMinimumScale      = sDataTypeDefinition->mMinNumericScale;
        sMaximumScale      = sDataTypeDefinition->mMaxNumericScale;
        sNumPrecRadix      = sDataTypeDefinition->mDefaultNumericPrecRadix;
        sIntervalPrecision = sDataTypeDefinition->mMaxIntervalPrec;
        sSupportedFeature  = sDataTypeDefinition->mSupportedFeature;
        
        switch( sType )
        {
            case DTL_TYPE_FLOAT :
                sLiteralPrefix     = NULL;
                sLiteralSuffix     = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_FALSE;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                sColumnSize     = sDataTypeDefinition->mMaxNumericPrec;
                sCreateParams   = "PRECISION";
                sFixedPrecScale = STL_FALSE;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_NUMBER :
                sLiteralPrefix     = NULL;
                sLiteralSuffix     = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_FALSE;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                sColumnSize     = sDataTypeDefinition->mMaxNumericPrec;
                sCreateParams   = "PRECISION,SCALE";
                sFixedPrecScale = STL_FALSE;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_INTERVAL_YEAR_TO_MONTH_TYPE_ID :
                for( i = 0; i < DTL_INTERVAL_YEAR_TO_MONTH_TYPE_COUNT; i++ )
                {
                    sDataTypeDefinition = &gIntervalYearToMonthDataTypeDefinition[i];

                    sTypeName          = sDataTypeDefinition->mSqlNormalTypeName;
                    sODBCDataType      = sDataTypeDefinition->mODBCDataTypeID;
                    sSQLDataType       = 10; /* SQL_INTERVAL */
                    sJDBCDataType      = sDataTypeDefinition->mJDBCDataTypeID;
                    sSearchable        = sDataTypeDefinition->mSearchable;
                    sNumPrecRadix      = sDataTypeDefinition->mDefaultNumericPrecRadix;
                    sMinimumScale      = sDataTypeDefinition->mMinFractionalSecondPrec;
                    sMaximumScale      = sDataTypeDefinition->mMaxFractionalSecondPrec;
                    sIntervalPrecision = sDataTypeDefinition->mMaxIntervalPrec;
                    sSupportedFeature  = sDataTypeDefinition->mSupportedFeature;
                    
                    sLiteralPrefix     = "'";
                    sLiteralSuffix     = "'";
                    sCaseSensitive     = STL_FALSE;
                    sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                    sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                    sCreateParams      = NULL;
                    sFixedPrecScale    = STL_FALSE;

                    switch( i )
                    {
                        case 0 :
                            /* INTERVAL YEAR */
                            sColumnSize     = 6;
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_YEAR;
                            break;
                        case 1 :
                            /* INTERVAL MONTH */
                            sColumnSize     = 6;
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_MONTH;
                            break;
                        case 2 :
                            /* INTERVAL YEAR TO MONTH */
                            sColumnSize     = 9;
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_YEAR_TO_MONTH;
                            break;
                        default :
                            STL_ASSERT( STL_FALSE );
                            break;
                    }

                    STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                       aStmt,
                                                       sTypeName,
                                                       sODBCDataType,
                                                       sJDBCDataType,
                                                       sColumnSize,
                                                       sLiteralPrefix,
                                                       sLiteralSuffix,
                                                       sCreateParams,
                                                       sNullable,
                                                       sCaseSensitive,
                                                       sSearchable,
                                                       sUnsignedAttribute,
                                                       sFixedPrecScale,
                                                       sAutoUniqueValue,
                                                       sTypeName,
                                                       sMinimumScale,
                                                       sMaximumScale,
                                                       sSQLDataType,
                                                       sSQLDateTimeSub,
                                                       sNumPrecRadix,
                                                       sIntervalPrecision,
                                                       sSupportedFeature,
                                                       aEnv ) == STL_SUCCESS );
                }
                break;
            case DTL_INTERVAL_DAY_TO_SECOND_TYPE_ID :
                for( i = 0; i < DTL_INTERVAL_DAY_TO_SECOND_TYPE_COUNT; i++ )
                {
                    sDataTypeDefinition = &gIntervalDayToSecondDataTypeDefinition[i];

                    sTypeName          = sDataTypeDefinition->mSqlNormalTypeName;
                    sODBCDataType      = sDataTypeDefinition->mODBCDataTypeID;
                    sSQLDataType       = 10; /* SQL_INTERVAL */
                    sJDBCDataType      = sDataTypeDefinition->mJDBCDataTypeID;
                    sSearchable        = sDataTypeDefinition->mSearchable;
                    sNumPrecRadix      = sDataTypeDefinition->mDefaultNumericPrecRadix;
                    sMinimumScale      = sDataTypeDefinition->mMinFractionalSecondPrec;
                    sMaximumScale      = sDataTypeDefinition->mMaxFractionalSecondPrec;
                    sIntervalPrecision = sDataTypeDefinition->mMaxIntervalPrec;
                    sSupportedFeature  = sDataTypeDefinition->mSupportedFeature;
                    
                    sLiteralPrefix     = "'";
                    sLiteralSuffix     = "'";
                    sCaseSensitive     = STL_FALSE;
                    sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                    sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                    sFixedPrecScale    = STL_FALSE;

                    switch( i )
                    {
                        case 0 :
                            /* INTERVAL DAY */
                            sColumnSize     = 6;
                            sCreateParams   = NULL;
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_DAY;
                            break;
                        case 1 :
                            /* INTERVAL HOUR */
                            sColumnSize     = 6;
                            sCreateParams   = NULL;
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_HOUR;
                            break;
                        case 2 :
                            /* INTERVAL MINUTE */
                            sColumnSize     = 6;
                            sCreateParams   = NULL;
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_MINUTE;
                            break;
                        case 3 :
                            /* INTERVAL SECOND */
                            sColumnSize     = 13;
                            sCreateParams   = "PRECISION";
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_SECOND;
                            break;
                        case 4 :
                            /* INTERVAL DAY TO HOUR */
                            sColumnSize     = 9;
                            sCreateParams   = NULL;
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_DAY_TO_HOUR;
                            break;
                        case 5 :
                            /* INTERVAL DAY TO MINUTE */
                            sColumnSize     = 12;
                            sCreateParams   = NULL;
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_DAY_TO_MINUTE;
                            break;
                        case 6 :
                            /* INTERVAL DAY TO SECOND */
                            sColumnSize     = 22;
                            sCreateParams   = "PRECISION";
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_DAY_TO_SECOND;
                            break;
                        case 7 :
                            /* INTERVAL HOUR TO MINUTE */
                            sColumnSize     = 9;
                            sCreateParams   = NULL;
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_HOUR_TO_MINUTE;
                            break;
                        case 8 :
                            /* INTERVAL HOUR TO SECOND */
                            sColumnSize     = 19;
                            sCreateParams   = "PRECISION";
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_HOUR_TO_SECOND;
                            break;
                        case 9 :
                            /* INTERVAL MINUTE TO SECOND */
                            sColumnSize     = 16;
                            sCreateParams   = "PRECISION";
                            sSQLDateTimeSub = ELDT_TypeInfo_SQL_DATETIME_SUB_MINUTE_TO_SECOND;
                            break;
                        default :
                            STL_ASSERT( STL_FALSE );
                            break;
                    }

                    STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                       aStmt,
                                                       sTypeName,
                                                       sODBCDataType,
                                                       sJDBCDataType,
                                                       sColumnSize,
                                                       sLiteralPrefix,
                                                       sLiteralSuffix,
                                                       sCreateParams,
                                                       sNullable,
                                                       sCaseSensitive,
                                                       sSearchable,
                                                       sUnsignedAttribute,
                                                       sFixedPrecScale,
                                                       sAutoUniqueValue,
                                                       sTypeName,
                                                       sMinimumScale,
                                                       sMaximumScale,
                                                       sSQLDataType,
                                                       sSQLDateTimeSub,
                                                       sNumPrecRadix,
                                                       sIntervalPrecision,
                                                       sSupportedFeature,
                                                       aEnv ) == STL_SUCCESS );
                }
                break;
            case DTL_TYPE_BOOLEAN :
                sColumnSize        = 5;
                sLiteralPrefix     = NULL;
                sLiteralSuffix     = NULL;
                sCreateParams      = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_NATIVE_SMALLINT :
                sColumnSize        = 5;
                sLiteralPrefix     = NULL;
                sLiteralSuffix     = NULL;
                sCreateParams      = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_FALSE;
                sFixedPrecScale    = STL_TRUE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_FALSE;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_NATIVE_INTEGER :
                sColumnSize        = 10;
                sLiteralPrefix     = NULL;
                sLiteralSuffix     = NULL;
                sCreateParams      = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_FALSE;
                sFixedPrecScale    = STL_TRUE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_FALSE;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_NATIVE_BIGINT :
                sColumnSize        = 19;
                sLiteralPrefix     = NULL;
                sLiteralSuffix     = NULL;
                sCreateParams      = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_FALSE;
                sFixedPrecScale    = STL_TRUE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_FALSE;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_NATIVE_REAL :
                sColumnSize        = 7;
                sLiteralPrefix     = NULL;
                sLiteralSuffix     = NULL;
                sCreateParams      = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_FALSE;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_FALSE;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_NATIVE_DOUBLE :
                sColumnSize        = 15;
                sLiteralPrefix     = NULL;
                sLiteralSuffix     = NULL;
                sCreateParams      = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_FALSE;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_FALSE;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_DECIMAL :
                sColumnSize        = DTL_PRECISION_NA;
                sLiteralPrefix     = NULL;
                sLiteralSuffix     = NULL;
                sCreateParams      = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_FALSE;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_FALSE;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_CHAR :
            case DTL_TYPE_VARCHAR :
            case DTL_TYPE_LONGVARCHAR :
                sColumnSize        = sDataTypeDefinition->mMaxStringLength;
                sLiteralPrefix     = "'";
                sLiteralSuffix     = "'";
                sCreateParams      = "LENGTH";
                sCaseSensitive     = STL_TRUE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_CLOB :
                if( sDataTypeDefinition->mMaxStringLength == DTL_LENGTH_NA )
                {
                    sColumnSize = DTL_PRECISION_NA;
                }
                else
                {
                    if( sDataTypeDefinition->mMaxStringLength > STL_INT32_MAX )
                    {
                        sColumnSize = STL_INT32_MAX;
                    }
                    else
                    {
                        sColumnSize = sDataTypeDefinition->mMaxStringLength;
                    }
                }

                sLiteralPrefix     = "'";
                sLiteralSuffix     = "'";
                sCreateParams      = NULL;
                sCaseSensitive     = STL_TRUE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_BINARY :
            case DTL_TYPE_VARBINARY :
            case DTL_TYPE_LONGVARBINARY :
                sColumnSize        = sDataTypeDefinition->mMaxStringLength;
                sLiteralPrefix     = "X'";
                sLiteralSuffix     = "'";
                sCreateParams      = "LENGTH";
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_BLOB :
                if( sDataTypeDefinition->mMaxStringLength == DTL_LENGTH_NA )
                {
                    sColumnSize = DTL_PRECISION_NA;
                }
                else
                {
                    if( sDataTypeDefinition->mMaxStringLength > STL_INT32_MAX )
                    {
                        sColumnSize = STL_INT32_MAX;
                    }
                    else
                    {
                        sColumnSize = sDataTypeDefinition->mMaxStringLength;
                    }
                }

                sLiteralPrefix     = "X'";
                sLiteralSuffix     = "'";
                sCreateParams      = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_DATE :
                sColumnSize        = 10;
                sLiteralPrefix     = "'";
                sLiteralSuffix     = "'";
                sCreateParams      = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                sSQLDataType       = 9; /* SQL_DATETIME */
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_DATE;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_TIME :
                sColumnSize        = 15;
                sLiteralPrefix     = "'";
                sLiteralSuffix     = "'";
                sCreateParams      = "PRECISION";
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                sMinimumScale      = sDataTypeDefinition->mMinFractionalSecondPrec;
                sMaximumScale      = sDataTypeDefinition->mMaxFractionalSecondPrec;
                sSQLDataType       = 9; /* SQL_DATETIME */
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_TIME;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_TIMESTAMP :
                sColumnSize        = 26;
                sLiteralPrefix     = "'";
                sLiteralSuffix     = "'";
                sCreateParams      = "PRECISION";
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                sMinimumScale      = sDataTypeDefinition->mMinFractionalSecondPrec;
                sMaximumScale      = sDataTypeDefinition->mMaxFractionalSecondPrec;
                sSQLDataType       = 9; /* SQL_DATETIME */
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_TIMESTAMP;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_TIME_WITH_TIME_ZONE :
                sColumnSize        = 22;
                sLiteralPrefix     = "'";
                sLiteralSuffix     = "'";
                sCreateParams      = "PRECISION";
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                sMinimumScale      = sDataTypeDefinition->mMinFractionalSecondPrec;
                sMaximumScale      = sDataTypeDefinition->mMaxFractionalSecondPrec;
                sSQLDataType       = 9; /* SQL_DATETIME */
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_TIME_WITH_TIMEZONE;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE :
                sColumnSize        = 33;
                sLiteralPrefix     = "'";
                sLiteralSuffix     = "'";
                sCreateParams      = "PRECISION";
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                sMinimumScale      = sDataTypeDefinition->mMinFractionalSecondPrec;
                sMaximumScale      = sDataTypeDefinition->mMaxFractionalSecondPrec;
                sSQLDataType       = 9; /* SQL_DATETIME */
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_TIMESTAMP_WITH_TIMEZONE;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            case DTL_TYPE_ROWID :
                sColumnSize        = 23;
                sLiteralPrefix     = "'";
                sLiteralSuffix     = "'";
                sCreateParams      = NULL;
                sCaseSensitive     = STL_FALSE;
                sUnsignedAttribute = ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA;
                sFixedPrecScale    = STL_FALSE;
                sAutoUniqueValue   = ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA;
                sSQLDateTimeSub    = ELDT_TypeInfo_SQL_DATETIME_SUB_NA;

                STL_TRY( ellInsertBuiltInTypeDesc( aTransID,
                                                   aStmt,
                                                   sTypeName,
                                                   sODBCDataType,
                                                   sJDBCDataType,
                                                   sColumnSize,
                                                   sLiteralPrefix,
                                                   sLiteralSuffix,
                                                   sCreateParams,
                                                   sNullable,
                                                   sCaseSensitive,
                                                   sSearchable,
                                                   sUnsignedAttribute,
                                                   sFixedPrecScale,
                                                   sAutoUniqueValue,
                                                   sTypeName,
                                                   sMinimumScale,
                                                   sMaximumScale,
                                                   sSQLDataType,
                                                   sSQLDateTimeSub,
                                                   sNumPrecRadix,
                                                   sIntervalPrecision,
                                                   sSupportedFeature,
                                                   aEnv ) == STL_SUCCESS );
                break;
            default :
                STL_ASSERT( STL_FALSE );
                break;
        }
    }
        
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} eldtTYPE_INFO */
