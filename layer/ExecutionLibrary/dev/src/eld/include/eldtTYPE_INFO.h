/*******************************************************************************
 * eldtTYPE_INFO.h
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


#ifndef _ELDT_TYPE_INFO_H_
#define _ELDT_TYPE_INFO_H_ 1

/**
 * @file eldtTYPE_INFO.h
 * @brief TYPE_INFO dictionary base table
 */

/**
 * @defgroup eldtTYPE_INFO TYPE_INFO table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief TYPE_INFO 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderTYPE_INFO
{
    /*
     * 식별자 영역
     */

    ELDT_TypeInfo_ColumnOrder_TYPE_NAME = 0,      /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_ODBC_DATA_TYPE,     /* ODBC */
    ELDT_TypeInfo_ColumnOrder_JDBC_DATA_TYPE,     /* JDBC */
    ELDT_TypeInfo_ColumnOrder_COLUMN_SIZE,        /* ODBC */
    ELDT_TypeInfo_ColumnOrder_LITERAL_PREFIX,     /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_LITERAL_SUFFIX,     /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_CREATE_PARAMS,      /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_NULLABLE,           /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_CASE_SENSITIVE,     /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_SEARCHABLE,         /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_UNSIGNED_ATTRIBUTE, /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_FIXED_PREC_SCALE,   /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_AUTO_UNIQUE_VALUE,  /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_LOCAL_TYPE_NAME,    /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_MINIMUM_SCALE,      /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_MAXIMUM_SCALE,      /* ODBC, JDBC */
    ELDT_TypeInfo_ColumnOrder_SQL_DATA_TYPE,      /* ODBC */
    ELDT_TypeInfo_ColumnOrder_SQL_DATETIME_SUB,   /* ODBC */
    ELDT_TypeInfo_ColumnOrder_NUM_PREC_RADIX,     /* ODBC */
    ELDT_TypeInfo_ColumnOrder_INTERVAL_PRECISION, /* ODBC */
    
    ELDT_TypeInfo_ColumnOrder_IS_SUPPORTED,
    
    ELDT_TypeInfo_ColumnOrder_MAX
    
} eldtColumnOrderTYPE_INFO;
    

extern eldtDefinitionTableDesc   gEldtTableDescTYPE_INFO;
extern eldtDefinitionColumnDesc  gEldtColumnDescTYPE_INFO[ELDT_TypeInfo_ColumnOrder_MAX];


/**
 * @brief TYPE_INFO 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 * - SQL 표준
 *  - SQL 1 : PRIMARY KEY ( TYPE_NAME )
 *   - => : PRIMARY KEY ( TYPE_NAME )
 * - 비표준
 *   - 별도의 key constraint가 없다. 
 */

typedef enum eldtKeyConstTYPE_INFO
{
    /*
     * Primary Key 영역
     */

    ELDT_TypeInfo_Const_PRIMARY_KEY = 0,

    /*
     * Unique 영역
     */

    /*
     * Foreign Key 영역
     */
    
    ELDT_TypeInfo_Const_MAX
    
} eldtKeyConstTYPE_INFO;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescTYPE_INFO[ELDT_TypeInfo_Const_MAX];

/**
 * @brief TYPE_INFO 테이블의 부가적 INDEX
 * - identifier 에 해당하는 컬럼에 인덱스를 생성한다.
 */

typedef enum eldtIndexTYPE_INFO
{
    ELDT_TypeInfo_Index_ODBC_DATA_TYPE = 0,
    ELDT_TypeInfo_Index_JDBC_DATA_TYPE,
    
    ELDT_TypeInfo_Index_MAX
    
} eldtIndexTYPE_INFO;

extern eldtDefinitionIndexDesc  gEldtIndexDescTYPE_INFO[ELDT_TypeInfo_Index_MAX];

typedef enum eldtTypeInfoUnsignedAttribute
{
    ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_NA = 0, /**< N/A */
    
    ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_TRUE,   /**< unsigned */
    ELDT_TypeInfo_UNSIGNED_ATTRIBUTE_FALSE   /**< signed */
} eldtTypeInfoUnsignedAttribute;

typedef enum eldtTypeInfoAutoUniqueValue
{
    ELDT_TypeInfo_AUTO_UNIQUE_VALUE_NA = 0, /**< N/A */

    ELDT_TypeInfo_AUTO_UNIQUE_VALUE_TRUE,   /**< autoincrementing */
    ELDT_TypeInfo_AUTO_UNIQUE_VALUE_FALSE   /**< not autoincrementing */
} eldtTypeInfoAutoUniqueValue;

typedef enum eldtTypeInfoSqlDateTimeSub
{
    ELDT_TypeInfo_SQL_DATETIME_SUB_NA = 0,

    /* datetime type */
    ELDT_TypeInfo_SQL_DATETIME_SUB_DATE = 1,
    ELDT_TypeInfo_SQL_DATETIME_SUB_TIME,
    ELDT_TypeInfo_SQL_DATETIME_SUB_TIMESTAMP,
    ELDT_TypeInfo_SQL_DATETIME_SUB_TIME_WITH_TIMEZONE,
    ELDT_TypeInfo_SQL_DATETIME_SUB_TIMESTAMP_WITH_TIMEZONE,

    /* interval type */
    ELDT_TypeInfo_SQL_DATETIME_SUB_YEAR = 1,
    ELDT_TypeInfo_SQL_DATETIME_SUB_MONTH,
    ELDT_TypeInfo_SQL_DATETIME_SUB_DAY,
    ELDT_TypeInfo_SQL_DATETIME_SUB_HOUR,
    ELDT_TypeInfo_SQL_DATETIME_SUB_MINUTE,
    ELDT_TypeInfo_SQL_DATETIME_SUB_SECOND,
    ELDT_TypeInfo_SQL_DATETIME_SUB_YEAR_TO_MONTH,
    ELDT_TypeInfo_SQL_DATETIME_SUB_DAY_TO_HOUR,
    ELDT_TypeInfo_SQL_DATETIME_SUB_DAY_TO_MINUTE,
    ELDT_TypeInfo_SQL_DATETIME_SUB_DAY_TO_SECOND,
    ELDT_TypeInfo_SQL_DATETIME_SUB_HOUR_TO_MINUTE,
    ELDT_TypeInfo_SQL_DATETIME_SUB_HOUR_TO_SECOND,
    ELDT_TypeInfo_SQL_DATETIME_SUB_MINUTE_TO_SECOND
} eldtTypeInfoSqlDateTimeSub;

/**
 * @brief SCHEMA.TYPE_INFO 테이블의 레코드를 구성하기 위한 자료구조
 * @remarks
 * Data Type layer의 dtlDataTypeDefinition 자료구조를 변형해서 사용한다.
 */

typedef struct eldtTypeInfoRecord
{
    stlChar  * mTypeName;          /**< 데이터 타입 이름 */
    stlInt32   mODBCDataType;      /**< ODBC : data type */
    stlInt32   mJDBCDataType;      /**< JDBC : data type */
    stlInt32   mColumnSize;        /**< ODBC : column size */
    stlChar  * mLiteralPrefix;     /**< prefix used to quote a literal */
    stlChar  * mLiteralSuffix;     /**< suffix used to quote a literal */
    stlChar  * mCreateParams;      /**< parameters used in creating the type */    
    stlBool    mNullable;          /**< whether a column can be NULL */
    stlBool    mCaseSensitive;     /**< case sensitive */
    stlInt32   mSearchable;        /**< whether it is possible to use a WHERE clause based on this type */
    stlBool    mUnsignedAttribute; /**< unsigned */
    stlBool    mFixedPrecScale;    /**< exact numeric with a fixed precision and scale */
    stlBool    mAutoUniqueValue;   /**< auto-increment value */
    stlChar  * mLocalTypeName;     /**< localized version of the type name */
    stlInt32   mMinimumScale;      /**< minimum scale */
    stlInt32   mMaximumScale;      /**< maximum scale */
    stlInt32   mSQLDataType;       /**< SQL data type */
    stlInt32   mSQLDateTimeSub;    /**< SQL date/time type subcodes */
    stlInt32   mNumPrecRadix;      /**< radix */
    stlInt32   mIntervalPrecision; /**< interval leading precision */
    stlBool    mSupportedFeature;  /**< 지원 여부 */
} eldtTypeInfoRecord;

stlStatus eldtInsertBuiltInTypeInfoDesc( smlTransId     aTransID,
                                         smlStatement * aStmt,
                                         ellEnv       * aEnv );

/** @} eldtTYPE_INFO */



#endif /* _ELDT_TYPE_INFO_H_ */
