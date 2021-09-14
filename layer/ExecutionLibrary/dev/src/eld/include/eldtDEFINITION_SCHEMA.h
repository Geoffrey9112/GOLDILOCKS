/*******************************************************************************
 * eldtDefinitionSchema.h
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


#ifndef _ELDT_DEFINITION_SCHEMA_H_
#define _ELDT_DEFINITION_SCHEMA_H_ 1

/**
 * @file eldtDEFINITION_SCHEMA.h
 * @brief Dictionary Table 관리를 위한 DEFINITION_SCHEMA 정의 
 */

   
/**
 * @ingroup eldtSCHEMA
 * @{
 */


/**
 * @brief DEFINITION_SCHEMA 를 위한 Domain 번호 
 */
typedef enum eldtDefinitionDomainNO
{
    ELDT_DOMAIN_NA = 0,             /** N/A */
    
    ELDT_DOMAIN_SQL_IDENTIFIER,     /**< SQL 표준, Object Name, VARCHAR(128) */

    /*
     * SQL 표준 CHARACTER_DATA 에 해당하며, 아래의 세가지 Domain 으로 구분함
     */
    
    ELDT_DOMAIN_SHORT_DESC,         /**< 비표준, Short Description, VARCHAR(32) */
    ELDT_DOMAIN_LONG_DESC,          /**< 비표준, Long Description,  VARCHAR(1024) */
    ELDT_DOMAIN_SQL_SYNTAX,         /**< 비표준, SQL Syntax,        LONG VARCHAR */

    ELDT_DOMAIN_CARDINAL_NUMBER,    /**< SQL 표준, INTEGER */
    ELDT_DOMAIN_BIG_NUMBER,         /**< 비표준, BIGINT  */
    ELDT_DOMAIN_SERIAL_NUMBER,      /**< 비표준, Object ID, BIGINT SEQUENCE */
    
    ELDT_DOMAIN_YES_OR_NO,          /**< SQL 표준, yes or no, BOOLEAN */
    
    ELDT_DOMAIN_TIME_STAMP,         /**< SQL 표준, TIMESTAMP(2) */
    ELDT_DOMAIN_INTERVAL,           /**< 비표준, INTERVAL HOUR TO MINUTE */
    ELDT_DOMAIN_FLOATING_NUMBER,    /**< 비표준, NUMBER(38,N/A) */

    ELDT_DOMAIN_MAX
    
} eldtDefinitionDomainNO;

/**
 * @brief DEFINITION_SCHEMA 에 속하는 Dictionary Table 들의 ID
 */
typedef enum eldtDefinitionTableID
{
    /*
     * SQL 표준 core tables
     */

    ELDT_TABLE_ID_NA = 0,
    
    ELDT_TABLE_ID_TABLES,
    ELDT_TABLE_ID_COLUMNS,
    ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,

    /*
     * SQL 표준 tables
     */
    
    ELDT_TABLE_ID_AUTHORIZATIONS,
    ELDT_TABLE_ID_CATALOG_NAME,
    ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
    ELDT_TABLE_ID_CHECK_CONSTRAINTS,
    ELDT_TABLE_ID_CHECK_TABLE_USAGE,
    ELDT_TABLE_ID_COLUMN_PRIVILEGES,
    ELDT_TABLE_ID_KEY_COLUMN_USAGE,
    ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
    ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
    ELDT_TABLE_ID_SCHEMATA,
    ELDT_TABLE_ID_SEQUENCES,
    ELDT_TABLE_ID_SQL_CONFORMANCE,
    ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,
    ELDT_TABLE_ID_SQL_SIZING,
    ELDT_TABLE_ID_TABLE_CONSTRAINTS,
    ELDT_TABLE_ID_TABLE_PRIVILEGES,
    ELDT_TABLE_ID_USAGE_PRIVILEGES,
    ELDT_TABLE_ID_VIEW_TABLE_USAGE,
    ELDT_TABLE_ID_VIEWS,

    /*
     * 비표준 tables
     */
    
    ELDT_TABLE_ID_TYPE_INFO,
    ELDT_TABLE_ID_DATABASE_PRIVILEGES,
    ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,
    ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,
    ELDT_TABLE_ID_INDEXES,
    ELDT_TABLE_ID_PENDING_TRANS,
    ELDT_TABLE_ID_SCHEMA_PRIVILEGES,
    ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
    ELDT_TABLE_ID_TABLESPACES,
    ELDT_TABLE_ID_USER_SCHEMA_PATH,
    ELDT_TABLE_ID_USERS,
    ELDT_TABLE_ID_SYNONYMS,
    ELDT_TABLE_ID_PUBLIC_SYNONYMS,
    ELDT_TABLE_ID_PROFILES,
    ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
    ELDT_TABLE_ID_PROFILE_KERNEL_PARAMETER,
    ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
    
    ELDT_TABLE_ID_MAX
} eldtDefinitionTableID;

extern stlInt64 gEldtIdentityValue[ELDT_TABLE_ID_MAX];

/**
 * @brief DEFINITION_SCHEMA의 Domain Descriptor
 */
typedef struct eldtDefinitionDomainDesc
{
    eldtDefinitionDomainNO        mDomainNO;     /**< Domain Number */
    dtlDataType                   mDataTypeID;   /**< Data Type ID */
    stlChar                     * mDataTypeName; /**< Data Type Name */
    stlBool                       mIsIdentity;   /**< 컬럼의 IDENTITY 여부 */
    stlInt64                      mLength;       /**< String Length */
    stlInt32                      mPrecision;    /**< Precision */
} eldtDefinitionDomainDesc;

/**
 * @brief DEFINITION_SCHEMA의 컬럼의 nullable 정보 
 */
typedef enum eldtDefinitionNullable
{
    ELDT_NULLABLE_YES = 0,               /* NULLABLE */
    ELDT_NULLABLE_COLUMN_NOT_NULL,       /* 컬럼에 정의된 NOT NULL */
    ELDT_NULLABLE_PK_NOT_NULL            /* Primary Key 에 포함된 NOT NULL */
} eldtDefinitionNullable;
    
/**
 * @brief DEFINITION_SCHEMA의 Column Descriptor
 */
typedef struct eldtDefinitionColumnDesc
{
    eldtDefinitionTableID         mTableID;          /**< Table ID */
    stlChar                     * mColumnName;       /**< 컬럼의 이름  */
    stlInt32                      mOrdinalPosition;  /**< 컬럼의 순서 위치 */
    eldtDefinitionDomainNO        mDomainNO;         /**< 컬럼의 Domain Number */
    eldtDefinitionNullable        mNullable;         /**< 컬럼의 Nullable 여부 */
    stlChar                     * mComment;          /**< 컬럼의 주석 */
} eldtDefinitionColumnDesc;

#define ELDT_DOMAIN_PRECISION_SHORT_DESC       (32)
#define ELDT_DOMAIN_PRECISION_SQL_IDENTIFIER   (128)
#define ELDT_DOMAIN_PRECISION_LONG_DESC        (1024)
#define ELDT_DOMAIN_PRECISION_SQL_SYNTAX       (DTL_LONGVARCHAR_MAX_PRECISION)
#define ELDT_DOMAIN_PRECISION_TIME_STAMP       (2)
#define ELDT_DOMAIN_PRECISION_FLOATING_NUMBER  (DTL_NUMERIC_MAX_PRECISION)

/**
 * @brief INTERVAL HOUR(2) TO MINUTE(2) 를 위한 상수
 */
#define ELDT_DOMAIN_INTERVAL_INDICATOR         (DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE)
#define ELDT_DOMAIN_INTERVAL_START_PRECISION   (2)   


/**
 * @brief DEFINITION_SCHEMA에 속하는 모든 Column Descriptor Array
 */

typedef struct eldtDefinitionColumnDescArray
{
    stlInt32                   mDictColumnCnt;   /**< Column Descriptor의 개수 */
    eldtDefinitionColumnDesc * mDictColumnDesc;  /**< Column Descriptor */
} eldtDefinitionColumnDescArray;

extern eldtDefinitionColumnDescArray gEldtColumnDescSCHEMA[ELDT_TABLE_ID_MAX];

/**
 * @brief DEFINITION_SCHEMA의 Table Descriptor
 */

typedef struct eldtDefinitionTableDesc
{
    eldtDefinitionTableID         mTableID;          /**< Table ID */
    stlChar                     * mTableName;        /**< 테이블의 이름  */
    stlChar                     * mComment;          /**< 테이블의 주석 */
} eldtDefinitionTableDesc;

extern eldtDefinitionTableDesc * gEldtTableDescSCHEMA[ELDT_TABLE_ID_MAX];


/**
 * @brief DEFINITION_SCHEMA 의 Key를 구성하는 컬럼의 최대 개수
 */
#define ELDT_KEY_COLUMN_COUNT_MAXIMUM     ( 8 )

/**
 * @brief DEFINITION_SCHEMA의 Key Constraint Descriptor
 */ 
typedef struct eldtDefinitionKeyConstDesc
{
    eldtDefinitionTableID         mTableID;          /**< Table ID */
    stlInt32                      mConstNum;         /**< Table 내 Constraint 번호 */
    ellTableConstraintType        mConstType;        /**< Table Constraint 유형 */
    stlInt32                      mKeyCount;         /**< 키를 구성하는 컬럼의 개수 */
    stlInt32                      mColOrder[ELDT_KEY_COLUMN_COUNT_MAXIMUM];
                                  /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
    eldtDefinitionTableID         mRefTableID;       /**< 참조 제약이 참조하는 Table ID */
    stlInt32                      mRefConstNum;      /**< 참조 제약이 참조하는 Unique의 번호 */
} eldtDefinitionKeyConstDesc;

/**
 * @brief DEFINITION_SCHEMA에 속하는 모든 Table Constraint Descriptor Array
 */

typedef struct eldtDefinitionKeyConstDescArray
{
    stlInt32                     mDictConstCnt;   /**< Constraint Descriptor의 개수 */
    eldtDefinitionKeyConstDesc * mDictConstDesc;  /**< Constraint Descriptor */
} eldtDefinitionKeyConstDescArray;

extern eldtDefinitionKeyConstDescArray gEldtKeyConstDescSCHEMA[ELDT_TABLE_ID_MAX];


/**
 * @brief DEFINITION_SCHEMA 의 Index Descriptor
 */

typedef struct eldtDefinitionIndexDesc
{
    eldtDefinitionTableID         mTableID;          /**< Table ID */
    stlInt32                      mIndexNum;         /**< Table 내 Index 번호 */
    stlInt32                      mKeyCount;         /**< 키를 구성하는 컬럼의 개수 */
    stlInt32                      mColOrder[ELDT_KEY_COLUMN_COUNT_MAXIMUM];
} eldtDefinitionIndexDesc;


/**
 * @brief DEFINITION_SCHEMA에 속하는 모든 Index Descriptor Array
 */

typedef struct eldtDefinitionIndexDescArray
{
    stlInt32                  mDictIndexCnt;   /**< Index Descriptor의 개수 */
    eldtDefinitionIndexDesc * mDictIndexDesc;  /**< Index Descriptor */
} eldtDefinitionIndexDescArray;

extern eldtDefinitionIndexDescArray gEldtIndexDescSCHEMA[ELDT_TABLE_ID_MAX];



/**
 * @brief DEFINITION_SCHEMA 에 정의된 모든 Index 의 물리적 정보
 */
typedef struct eldtDefinitionPhysicalIndexInfo
{
    void   * mIndexHandle;  /**< 생성한 인덱스의 Handle */
    stlBool  mIsKeyConst;   /**< 인덱스가 Constraint, 또는 Index 로부터 생성 여부 */
    void   * mDescInfo;     /**< eldtDefinitionKeyConstDesc or eldtDefinitionIndexDesc */
} eldtDefinitionPhysicalIndexInfo;

/**
 * @brief DEFINITION_SCHEMA 의 최대 인덱스 개수
 * 오류가 발생한다면 이 값을 변경시켜야 함.
 */
#define ELDT_MAX_INDEX_COUNT    (1024)
extern stlInt32 gEldtIndexCnt;
extern eldtDefinitionPhysicalIndexInfo gEldtIndexInfo[ELDT_MAX_INDEX_COUNT];









stlStatus eldtBuildDEFINITION_SCHEMA( smlTransId            aTransID,
                                      smlStatement        * aStmt,
                                      stlChar             * aDatabaseName,
                                      stlChar             * aDatabaseComment,
                                      stlInt64              aFirstDictPhysicalID,
                                      void                * aFirstDictPhysicalHandle,
                                      smlTbsId              aSystemTbsID,
                                      smlTbsId              aDefaultUserTbsID,
                                      stlChar             * aDefaultUserTbsName,
                                      smlTbsId              aDefaultUndoTbsID,
                                      stlChar             * aDefaultUndoTbsName,
                                      smlTbsId              aDefaultTempTbsID,
                                      stlChar             * aDefaultTempTbsName,
                                      dtlCharacterSet       aCharacterSetID,
                                      dtlStringLengthUnit   aCharLengthUnit,
                                      stlChar             * aEncryptedSysPassword,
                                      stlChar             * aTimeZoneIntervalString,
                                      ellEnv              * aEnv );

stlStatus eldtCreateDictionaryTables( smlTransId     aTransID,
                                      smlStatement * aStmt,
                                      stlInt64       aFirstDictPhysicalID,
                                      void         * aFirstDictPhysicalHandle,
                                      smlTbsId       aSystemTbsID,
                                      ellEnv       * aEnv );

stlStatus eldtInsertDictionaryColumns( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       smlTbsId       aSystemTbsID,
                                       ellEnv       * aEnv );
                                                 
                            
stlStatus eldtInsertColumnDomainDesc( smlTransId                 aTransID,
                                      smlStatement             * aStmt,
                                      stlInt64                   aTableID,
                                      stlInt64                   aColumnID,
                                      eldtDefinitionDomainDesc * aDomainDesc,
                                      ellEnv                   * aEnv );

stlStatus eldtInsertKeyConstraints( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    stlInt64       aTablespaceID,
                                    ellEnv       * aEnv );


stlStatus eldtInsertKeyDesc( smlTransId                     aTransID,
                             smlStatement                 * aStmt,
                             stlInt64                       aConstID,
                             eldtDefinitionKeyConstDesc   * aConstDesc,
                             ellEnv                       * aEnv );

stlStatus eldtInsertForeignKey( smlTransId                     aTransID,
                                smlStatement                 * aStmt,
                                stlInt64                       aConstID,
                                eldtDefinitionKeyConstDesc   * aConstDesc,
                                ellEnv                       * aEnv );

stlStatus eldtInsertNotNullColumn( smlTransId                     aTransID,
                                   smlStatement                 * aStmt,
                                   stlInt64                       aColumnID,
                                   eldtDefinitionColumnDesc     * aColumnDesc,
                                   ellEnv                       * aEnv );

stlStatus eldtInsertKeyIndex( smlTransId                     aTransID,
                              smlStatement                 * aStmt,
                              eldtDefinitionKeyConstDesc   * aConstDesc,
                              stlChar                      * aKeyConstraintName,
                              stlInt64                     * aIndexID,
                              stlInt64                       aTablespaceID,
                              ellEnv                       * aEnv );

stlStatus eldtInsertIndexes( smlTransId     aTransID,
                             smlStatement * aStmt,
                             stlInt64       aTablespaceID,
                             ellEnv       * aEnv );

stlStatus eldtSetKeyConstraintName( stlChar                    * aConstNameBuffer,
                                    stlInt32                     aBufferSize,
                                    eldtDefinitionKeyConstDesc * aConstDesc,
                                    ellEnv                     * aEnv );

stlStatus eldtSetIndexName( stlChar                    * aIndexNameBuffer,
                            stlInt32                     aBufferSize,
                            stlChar                    * aKeyConstraintName,
                            eldtDefinitionTableID        mTableID,
                            stlInt32                     aKeyCount,
                            stlInt32                   * aKeyColumnArray,
                            ellEnv                     * aEnv );

/*
 * DEFINITION_SCHEMA 의 자체 descriptor 정보 획득 루틴
 */

stlStatus eldtGetDictTableColumnCnt( eldtDefinitionTableID      aDictTableID,
                                     stlInt32                 * aColumnCnt,
                                     ellEnv                   * aEnv );

stlStatus eldtGetDictColumnDescArray( eldtDefinitionTableID       aDictTableID,
                                      eldtDefinitionColumnDesc ** aDictColumnDesc,
                                      ellEnv                    * aEnv );

eldtDefinitionDomainDesc * eldtGetDictDomainDesc( eldtDefinitionDomainNO aDomainNO );

stlStatus eldtGetDictColumnID( eldtDefinitionTableID  aDictTableID,
                               stlInt32               aDictColumnOrder,
                               stlInt64             * aCalcColumnID,
                               ellEnv               * aEnv );

stlStatus eldtGetDictKeyConstID( eldtDefinitionTableID  aDictTableID,
                                 stlInt32               aDictConstNum,
                                 stlInt64             * aCalcConstID,
                                 ellEnv               * aEnv );


/** @} eldtSCHEMA */

#endif /* _ELDT_DEFINITION_SCHEMA_H_ */
