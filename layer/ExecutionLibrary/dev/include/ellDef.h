/*******************************************************************************
 * ellDef.h
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


#ifndef _ELLDEF_H_
#define _ELLDEF_H_ 1

/**
 * @file ellDef.h
 * @brief Execution Layer Definitions
 */



/**
 * @defgroup elExternal Execution Library Layer Routines
 * @{
 */

/**
 * @defgroup ellError Error
 * @{
 */

/**
 * @brief failed to initialize execution library layer
 */
#define ELL_ERRCODE_INITIALIZE                              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 0 )

/**
 * @brief execution library layer is not initialized yet
 */
#define ELL_ERRCODE_INITIALIZED_YET                         \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 1 )

/**
 * @brief \"%s\": schema does not exist.
 */
#define ELL_ERRCODE_INVALID_SCHEMA_NAME                     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 2 )

/**
 * @brief \"%s\": table or view does not exist.
 */
#define ELL_ERRCODE_INVALID_TABLE_NAME                      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 3 )

/**
 * @brief \"%s\": index does not exist.
 */
#define ELL_ERRCODE_INVALID_INDEX_NAME                      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 4 )

/**
 * @brief \"%s\": column does not exist.
 */
#define ELL_ERRCODE_INVALID_COLUMN_NAME                     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 5 )

/**
 * @brief \"%s\": dictionary integrity constraint violation by concurrent DDL execution
 */
#define ELL_ERRCODE_INTEGRITY_VIOLATION                     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 6 )

/**
 * @brief not enough stack space to add entries
 */
#define ELL_ERRCODE_ELK_NOT_ENOUGH_STACK_SPACE              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 7 )

/**
 * @brief table related objects modified while validation
 * Validation 과정에서 발생할 수 있는 아주 특별한 에러이다.
 * 이 에러에 대해서는 정보를 다시 획득해야 한다.
 */
#define ELL_ERRCODE_TABLE_MODIFIED_IN_VALIDATION            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 8 )

/**
 * @brief invalid argument error
 */
#define ELL_ERRCODE_INVALID_ARGUMENT                        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 9 )

/** @brief object modified by concurrent DDL execution */
#define ELL_ERRCODE_INVALID_OBJECT                          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 10 )

/**
 * @brief not implemented feature, in a function %s
 */
#define ELL_ERRCODE_NOT_IMPLEMENTED                         \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 11 )

/**
 * @brief user dropped by other session
 */
#define ELL_ERRCODE_USER_DROPPED_BY_OTHER_SESSION           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 12 )

/**
 * @brief user default schema dropped
 */
#define ELL_ERRCODE_USER_DEFAULT_SCHEMA_DROPPED            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 13 )

/**
 * @brief user default data tablespace dropped
 */
#define ELL_ERRCODE_USER_DEFAULT_DATA_TABLESPACE_DROPPED            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 14 )

/**
 * @brief user default temporary tablespace dropped
 */
#define ELL_ERRCODE_USER_DEFAULT_TEMPORARY_TABLESPACE_DROPPED            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 15 )

/**
 * @brief limit(%d) on the number of named cursor exceeded
 */
#define ELL_ERRCODE_LIMIT_ON_THE_NUMBER_OF_NAMED_CURSOR_EXCEEDED    \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_EXECUTION_LIBRARY, 16 )

#define ELL_MAX_ERROR     17

/** @} ellError */


/**
 * @defgroup ellEnvironment Environment
 * @ingroup elExternal
 * @{
 */

/**
 * @defgroup ellProcEnv Process Environment
 * @ingroup ellEnvironment
 * @{
 */

typedef struct ellLongTypeBlock ellLongTypeBlock;

struct ellLongTypeBlock
{
    knlValueBlockList  * mValueBlockList;
    ellLongTypeBlock   * mNext;
};

/**
 * @brief Execution Library Process Environment
 * - Dictionary Operation 관리 : Region Heap Memory, 가변길이, Operation lifetime
 */
typedef struct ellEnv
{
    smlEnv             mSmlEnv;          /**< Storage Manager Environment */
    ellLongTypeBlock * mLongTypeBlock;   /**< List for Long Type Blocks */
    knlRegionMem       mMemDictOP;       /**< Dictionary DML Operation을 위한 memory */
} ellEnv;

/**
 * @brief Dictionary Operation을 위한 메모리 관리자의 메모리 크기의 단위 
 */

#define ELL_DICT_OP_MEM_INIT_SIZE                    ( 32 * 1024 )
#define ELL_DICT_OP_MEM_NEXT_SIZE                    ( 32 * 1024 )

/**
 * @brief Fixed Table Cache를 위한 메모리 관리자의 메모리 크기의 단위
 * - Fixed Table 이 50 개인 현재 800 K 이상을 사용하고 있음.
 */
#define ELL_DICT_FXTABLE_MEM_INIT_SIZE               ( 1024 * 1024 )
#define ELL_DICT_FXTABLE_MEM_NEXT_SIZE               ( 128 * 1024 )

/**
 * @brief Execution Environment
 */
#define ELL_ENV( aEnv )      ( (ellEnv *)aEnv )

/**
 * @brief Execution Library Environment에서 ErrorStack을 얻는 매크로
 */
#define ELL_ERROR_STACK( aEnv ) KNL_ERROR_STACK( aEnv )


/** @} ellProcEnv */



/** @} ellEnvironment */





/**
 * @defgroup ellTest Test 용 모듈
 * @ingroup elExternal
 * @{
 */

/** @} ellTest */













/**
 * @defgroup ellSqlObject SQL Object
 * @ingroup elExternal
 * @{
 * @remarks
 * - Dictionary 관련 정수형 변수는 다음과 같이 type을 가져간다.
 *  - INTEGER : precision/scale, Code Identifier
 *  - BIGINT  : length, Object Identifier
 * @remarks DDL 수행시 하나의 Statement 로 여러 Dictionary Table들을 변경하는 것이
 * 문제가 없는지 검토해야 한다.  특히, CREATE DATABASE 구문에 대해 여러 테이블들을
 * 하나의 Statement로 생성하는 것은 문제 소지가 없는지 확인해야 한다.
 * 또한, 하나의 트랜잭션으로 여러 테이블을 생성하는 것은 문제가 없는지 확인해야 한다.
 * - 김명근 님과 토론 후 SQL Statement 와 SM Statement 를 1:1 로 가져갈 수 있음을 확인함.
 * - 즉, 위의 검토 사항은 문제없음으로 잠정 결론 내림 
 */

/**
 * @brief Dictionary Object 유형
 */
typedef enum ellObjectType
{
    ELL_OBJECT_NA = 0,

    ELL_OBJECT_COLUMN,                 /**< Column Object */
    ELL_OBJECT_TABLE,                  /**< Table Object */
    ELL_OBJECT_CONSTRAINT,             /**< Table Constraint Object */
    ELL_OBJECT_INDEX,                  /**< Index Object */
    ELL_OBJECT_SEQUENCE,               /**< Sequence Object */
    ELL_OBJECT_SCHEMA,                 /**< Schema Object */
    ELL_OBJECT_TABLESPACE,             /**< Tablespace Object */
    ELL_OBJECT_AUTHORIZATION,          /**< Authorization Object */
    ELL_OBJECT_CATALOG,                /**< Catalog Object */
    ELL_OBJECT_SYNONYM,                /**< Synonym Object */
    ELL_OBJECT_PUBLIC_SYNONYM,         /**< Public Synonym Object */
    ELL_OBJECT_PROFILE,                /**< Profile Object */

    ELL_OBJECT_MAX
} ellObjectType;


#define ELL_DICT_CACHE_VIEW_SCN( aStmt )                                                \
    ( (aStmt->mNeedSnapshotIterator == STL_TRUE) ? aStmt->mScn : SML_MAXIMUM_STABLE_SCN )

/**
 * @brief Dictionary Cache 의 유효성 판단을 위한 자료구조
 */
typedef struct ellVersionInfo
{
    smlTransId        mCreateTransID;     /**< DDL 을 수행한/수행하고 있는 Transcation ID */
    smlScn            mCreateCommitSCN;   /**< Transaction 의 Commit SCN */
    smlTransId        mDropTransID;       /**< DDL 을 수행한/수행하고 있는 Transcation ID */
    smlScn            mDropCommitSCN;     /**< Transaction 의 Commit SCN */
    stlBool           mDropFlag;          /**< Drop 여부 */
    stlInt64          mModifyingCnt;      /**< table related object를 변경하고 있는 트랜잭션의 개수 */
    stlInt64          mTryModifyCnt;      /**< 객체 생성후 table related object 변경 회수 */
    stlInt64          mLastModifiedSCN;   /**< 객체 생성후 table related object 변경 SCN */
    stlInt64          mMemUseCnt;         /**< 홀수: Alloc Memory, 짝수: Free Memory */
} ellVersionInfo;


/**
 * @brief Dictionary Privilege Object 유형
 */
typedef enum ellPrivObject
{
    ELL_PRIV_NA = 0,

    ELL_PRIV_DATABASE,      /**< Database Privilege  */
    ELL_PRIV_SPACE,         /**< Tablespace Privilege  */
    ELL_PRIV_SCHEMA,        /**< Schema Privilege  */
    ELL_PRIV_TABLE,         /**< Table Privilege  */
    ELL_PRIV_USAGE,         /**< Usage Privilege  */
    ELL_PRIV_COLUMN,        /**< Column Privilege */

    ELL_PRIV_OWNER,         /**< Object Owner, is not privilege */
    
    ELL_PRIV_MAX
} ellPrivObject;


/**
 * @brief Dictionary Handle 정보
 */
typedef struct ellDictHandle
{
    /*
     * Object Dictionary 정보
     */

    stlBool           mFullyQualifiedName;  /**< public.t1 or (public.)t1 여부 */
    stlBool           mRecentCheckable;     /**< Recent Checkable */
    ellObjectType     mObjectType;          /**< Dictionary Object Type */
    void            * mObjHashElement;      /**< Hash Element 정보 */

    /*
     * Privilege Dictionary 정보
     */
    
    ellPrivObject     mPrivObject;      /**< Privilege Object */
    void            * mPrivHashElement; /**< Privilege 용 Hash Element */

    /*
     * 공통 정보
     */

    stlInt64          mTryModifyCnt;      /**< 객체를 수정하려는 시도 회수 */
    stlInt64          mLocalModifySeq;    /**< My Transaction에 의한 table related object 변경 횟수 */
    smlScn            mViewSCN;           /**< Handle 설정당시의 View SCN */
    stlInt64          mMemUseCnt;         /**< 홀수: Alloc Memory, 짝수: Free Memory */
} ellDictHandle;




    
/**
 * @brief Dictionary Table 구축 과정인지의 여부
 */
extern stlBool gEllIsMetaBuilding;

/**
 * @brief Dictionary 정보를 위해 유효하지 않는 Object ID
 * @remarks
 * Dictionary 에서 관리하는 논리적 식별자는 양수 또는 음수를 갖는다.
 * - 양수 : SQL 객체의 식별자로 사용
 * - 음수 : Fixed Table, Fixed Column 과 같이 Relation Model 에 속하지 않는 객체의 식별자로 사용
 * 그러나, Tablespace, DataType 등은 해당 layer의 특성에 맞게 고유한 ID 체계를 가지고 있다.
 * 이에 대해서는 별도의 N/A Identifier를 정의한다.
 */
#define ELL_DICT_OBJECT_ID_NA     (  0 )
#define ELL_DICT_TABLESPACE_ID_NA ( -1 )
#define ELL_DICT_DATATYPE_ID_NA   ( -1 )

/**
 * @brief Dictionary 정보를 위해 유효하지 않은 Position
 */
#define ELL_DICT_POSITION_NA ( -1 )

/**
 * @brief Dictionary 정보를 위해 유효하지 않은 stlTime 시간값
 */
#define ELL_DICT_STLTIME_NA      ( 0 )


/**
 * @defgroup ellSqlElement SQL Element
 * @ingroup ellSqlObject
 * @{
 */

/**
 * @brief Name List
 */
typedef struct ellNameList
{
    stlChar            * mName;  /**< Name */
    struct ellNameList * mNext;  /**< Next */
} ellNameList;



/** @defgroup ellBuiltInDataType Built-In DATA TYPE
 * @ingroup ellSqlElement
 * @{
 */

/** @} ellBuiltInDataType */









/**
 * @defgroup ellColumnType Column DATA TYPE
 * @ingroup ellSqlElement
 * @{
 */

/**
 * @brief 별도의 부가정보가 없는 컬럼의 data type descriptor
 */
typedef struct ellEmptyColDepDesc
{
    stlInt64    mDummy;
} ellEmptyColDepDesc;


/**
 * @brief Character String 계열 컬럼의 data type descriptor
 */
typedef struct ellStringColDepDesc
{
    dtlStringLengthUnit  mLengthUnit;       /**< String Length Unit */
    stlInt64             mCharLength;       /**< 유효한 문자 단위의 길이 */
    stlInt64             mOctetLength;      /**< 유효한 Byte 단위의 길이 */
} ellStringColDepDesc;


/**
 * @brief Binary String 계열 컬럼의 data type descriptor
 */
typedef struct ellBinaryColDepDesc
{
    stlInt64    mByteLength;       /**< 유효한 Byte 길이 */
} ellBinaryColDepDesc;

/**
 * @brief Binary Numeric 계열 컬럼의 data type descriptor
 */
typedef struct ellBinaryNumColDepDesc
{
    stlInt32    mBinaryPrec;       /**< 유효한 Binary Precision */
} ellBinaryNumColDepDesc;

/**
 * @brief Decimal Numeric 계열 컬럼의 data type descriptor
 */
typedef struct ellDecimalNumColDepDesc
{
    stlInt32    mRadix;            /**< 유효한 Radix */
    stlInt32    mPrec;             /**< 유효한 Decimal Precision */
    stlInt32    mScale;            /**< 유효한 Scale */
} ellDecimalNumColDepDesc;


/**
 * @brief DataTime 계열 컬럼의 data type descriptor
 */
typedef struct ellDateTimeColDepDesc
{
    stlInt32    mFracSecPrec;   /**< 유효한 Fractional Second Precision */
} ellDateTimeColDepDesc;

/**
 * @brief Interval 컬럼에 종속적인 desciptor
 */
typedef struct ellIntervalColDepDesc
{
    dtlIntervalIndicator mIntervalType;    /**< Interval 유형 */
    stlInt32             mStartPrec;       /**< Start Precision */
    stlInt32             mEndPrec;         /**< End Precision */
} ellIntervalColDepDesc;

/** @} ellColumnType */

/**
 * @defgroup ellColumn COLUMN
 * @ingroup ellSqlElement
 * @{
 */

/**
 * @brief IDENTITY 컬럼의 값 생성 옵션
 */
typedef enum ellIdentityGenOption
{
    ELL_IDENTITY_GENERATION_NA = 0,          /**< 해당사항 없음 */
    
    ELL_IDENTITY_GENERATION_ALWAYS,          /**< ALWAYS 옵션 */
    ELL_IDENTITY_GENERATION_BY_DEFAULT,      /**< BY DEFAULT 옵션 */

    ELL_IDENTITY_GENERATION_MAX
} ellIdentityGenOption;

extern const stlChar * const gEllIdentityGenOptionString[ELL_IDENTITY_GENERATION_MAX];

#define ELL_IDENTITY_GENERATION_DEFAULT       ( ELL_IDENTITY_GENERATION_BY_DEFAULT )


/**
 * @brief 새로운 컬럼 (Dictionary 가 구축되지 않은 컬럼)의 정보
 */
typedef struct ellAddColumnDesc
{
    stlInt64              mTableID;
    stlInt32              mOrdinalPosition; 
    dtlDataType           mDataType;     
    stlInt64              mArgNum1;    
    stlInt64              mArgNum2;    
    dtlStringLengthUnit   mStringLengthUnit; 
    dtlIntervalIndicator  mIntervalIndicator;
    void                * mIdentityHandle;
} ellAddColumnDesc;

/**
 * @brief Column Descriptor
 */
typedef struct ellColumnDesc
{
    /*
     * DEFINITION_SCHEMA.COLUMNS 로부터 획득하는 정보
     */
    stlInt64        mOwnerID;                 /**< Owner ID */
    stlInt64        mSchemaID;                /**< Schema ID */
    stlInt64        mTableID;                 /**< Table ID */
    stlInt64        mColumnID;                /**< Column ID */
    stlChar       * mColumnName;              /**< Column Name */
    stlInt32        mOrdinalPosition;         /**< Column Position in a table */
    stlChar       * mDefaultString;           /**< Column Default Value */
    stlBool         mNullable;                /**< Nullable */
    stlBool         mUpdatable;               /**< Updatable */
    stlBool         mUnused;                  /**< Unused */

    /*
     * 부가적으로 구축하는 정보 
     */
    stlInt64             mIdentityPhyID;      /**< Identity Physical ID */
    void               * mIdentityHandle;     /**< Identity Column Handle */
    ellIdentityGenOption mIdentityGenOption;  /**< Identity GENERATED ALWAYS */
    
    dtlDataType     mDataType;                /**< Data Type */

    /*
     * 컬럼의 Data Type 정보
     */
    union {                                   
        ellEmptyColDepDesc       mEmpty;      /**< BOOL,LONG VARCHAR,LONG VARBINARY,DATE */
        ellStringColDepDesc      mString;     /**< CHAR, VARCHAR */
        ellBinaryColDepDesc      mBinary;     /**< BINARY, VARBINARY */
        ellBinaryNumColDepDesc   mBinaryNum;  /**< SMALLINT, INT, BIGINT, REAL, DOUBLE */
        ellDecimalNumColDepDesc  mDecimalNum; /**< NUMERIC */
        ellDateTimeColDepDesc    mDateTime;   /**< TIME,TIMESTAMP, WITH/WITHOUT TIMEZONE */
        ellIntervalColDepDesc    mInterval;   /**< INTERVAL */
    } mColumnType;
    
} ellColumnDesc;

/** @} ellColumn */

/** @} ellSqlElement */










/**
 * @defgroup ellSchemaObject SQL Schema Object
 * @ingroup ellSqlObject
 * @{
 */


/**
 * @defgroup ellObjectConstraint CONSTRAINT object
 * @ingroup ellSqlSchemaObject
 * @{
 */

/**
 * @brief Table Constraint Type
 * - Not Null 은 SQL표준에 정의한 Table Constraint가 아니며, Column 의 정보이다.
 * - 그러나, Not Null 에 이름을 부여한 경우, DEFINITION_SCHEMA 에서 관리할 적절한 테이블이 없다.
 * - Postgres, Oracle 과 마찬가지로 Not Null 을 Check Constraint 로 관리한다. 
 * - SQL-2008 에서는 다음과 같이 기술하고 있다.
 *  - If C is a column of a base table,
 *    then an indication of whether it is defined as NOT NULL and,
 *    if so, the constraint name of the associated table constraint definition.
 *  - NOTE 41
 *   - This indication and the associated constraint name exist
 *     for definitional purposes only and are not exposed
 *     through the COLUMNS view in the Information Schema.
 */
typedef enum ellTableConstraintType
{
    ELL_TABLE_CONSTRAINT_TYPE_NA = 0,               /**< 해당 사항 없음 */

    ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,          /**< PRIMARY KEY */
    ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,           /**< UNIQUE KEY */
    ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,          /**< FOREIGN KEY */
    ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL,       /**< CHECK NOT NULL */
    ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE,         /**< CHECK CLAUSE */

    ELL_TABLE_CONSTRAINT_TYPE_MAX
} ellTableConstraintType;

extern const stlChar * const gEllTableConstraintTypeString[ELL_TABLE_CONSTRAINT_TYPE_MAX];

/**
 * @brief
 * SQL 표준은 다음과 같다.
 * If [constraint characteristics] is not specified,
 * then INITIALLY IMMEDIATE NOT DEFERRABLE ENFORCED is implicit.
 */
#define ELL_TABLE_CONSTRAINT_DEFERRABLE_DEFAULT              ( STL_FALSE )

#define ELL_TABLE_CONSTRAINT_INITIALLY_DEFERRED_DEFAULT      ( STL_FALSE )

#define ELL_TABLE_CONSTRAINT_ENFORCED_DEFAULT                ( STL_TRUE )

/**
 * @brief Referential Constraint 의 Match Option
 */
typedef enum ellReferentialMatchOption
{
    ELL_REFERENTIAL_MATCH_OPTION_NA = 0,             /**< 해당사항 없음 */
    
    ELL_REFERENTIAL_MATCH_OPTION_SIMPLE,             /**< SIMPLE */
    ELL_REFERENTIAL_MATCH_OPTION_PARTIAL,            /**< PARTIAL */
    ELL_REFERENTIAL_MATCH_OPTION_FULL,               /**< FULL */

    ELL_REFERENTIAL_MATCH_OPTION_MAX
    
} ellReferentialMatchOption;

extern const stlChar * const gEllReferentialMatchOptionString[ELL_REFERENTIAL_MATCH_OPTION_MAX];

#define ELL_REFERENTIAL_MATCH_OPTION_DEFAULT   ( ELL_REFERENTIAL_MATCH_OPTION_SIMPLE )

/**
 * @brief Referential Constraint 의 Update/Delete의 Referential Action Rule
 */
typedef enum ellReferentialActionRule
{
    ELL_REFERENTIAL_ACTION_RULE_NA = 0,                /**< 해당 사항 없음 */
    
    ELL_REFERENTIAL_ACTION_RULE_CASCADE,               /**< CASCADE */
    ELL_REFERENTIAL_ACTION_RULE_SET_NULL,              /**< SET NULL */
    ELL_REFERENTIAL_ACTION_RULE_SET_DEFAULT,           /**< SET DEFAULT */
    ELL_REFERENTIAL_ACTION_RULE_RESTRICT,              /**< RESTRICT */
    ELL_REFERENTIAL_ACTION_RULE_NO_ACTION,             /**< NO ACTION */

    ELL_REFERENTIAL_ACTION_RULE_MAX
    
} ellReferentialActionRule;

extern const stlChar * const gEllReferentialActionRuleString[ELL_REFERENTIAL_ACTION_RULE_MAX];

#define ELL_REFERENTIAL_ACTION_RULE_DEFAULT    ( ELL_REFERENTIAL_ACTION_RULE_NO_ACTION )


/**
 * @brief Primary Key Constraint 에 종속적인 descriptor
 */
typedef struct ellPrimaryKeyDepDesc
{
    stlInt32      mKeyColumnCnt;                                /**< Key Column 개수 */
    ellDictHandle mKeyColumnHandle[DTL_INDEX_COLUMN_MAX_COUNT]; /**< Key Column Handle */
} ellPrimaryKeyDepDesc;


/**
 * @brief Unique Key Constraint 에 종속적인 descriptor
 * @remarks
 * - 별도의 정보를 관리하지 않는다.
 * - Unique 검사 :
 *  - 구문 수행후 table constraint 의 index handle 을 통한 conflict 여부 판단
 */
typedef struct ellUniqueKeyDepDesc
{
    stlInt32      mKeyColumnCnt;                                /**< Key Column 개수 */
    ellDictHandle mKeyColumnHandle[DTL_INDEX_COLUMN_MAX_COUNT]; /**< Key Column Handle */
} ellUniqueKeyDepDesc;


/**
 * @brief Foreign Key Constraint 에 종속적인 descriptor
 */
typedef struct ellForeignKeyDepDesc
{
    stlInt32      mKeyColumnCnt;              /**< Key Column 개수 */
    ellDictHandle mKeyColumnHandle[DTL_INDEX_COLUMN_MAX_COUNT]; 
    stlInt32      mKeyMatchIdxInParent[DTL_INDEX_COLUMN_MAX_COUNT]; /**< 0-base idx */

    ellDictHandle mParentUniqueKeyHandle;     /**< 참조하는 Parent Unique Key Handle */
    
    ellReferentialMatchOption mMatchOption;   /**< Match Option */
    ellReferentialActionRule  mUpdateRule;    /**< Update Referential Action */
    ellReferentialActionRule  mDeleteRule;    /**< Delete Referential Action */
    
} ellForeignKeyDepDesc;


/**
 * @brief Check Not Null Constraint 에 종속적인 descriptor
 */
typedef struct ellCheckNotNullDepDesc
{
    ellDictHandle    mColumnHandle;       /**< Not Null Column 의 Handle */
} ellCheckNotNullDepDesc;

/**
 * @brief Check Clause Constraint 에 종속적인 descriptor
 */
typedef struct ellCheckClauseDepDesc
{
    stlChar *     mCheckClause;    /**< CHECK 구문 */
} ellCheckClauseDepDesc;


/**
 * @brief Table Constraint Descriptor
 */
typedef struct ellTableConstDesc
{
    /*
     * DEFINITION_SCHEMA.TABLE_CONSTRAINTS 로부터 획득하는 정보
     */
    stlInt64                  mConstOwnerID;  /**< Constraint Owner ID */
    stlInt64                  mConstSchemaID; /**< Constraint Schema ID */
    stlInt64                  mConstID;       /**< Constraint ID */
    stlInt64                  mTableOwnerID;  /**< Table 의 Owner ID */
    stlInt64                  mTableSchemaID; /**< Table 의 Schema ID */
    stlInt64                  mTableID;       /**< Table 의 Table ID */
    stlChar                 * mConstName;     /**< Constraint Name */
    ellTableConstraintType    mConstType;     /**< Constraint Type */
    stlBool                   mDeferrable;    /**< DEFERRABLE or NOT DEFERRABLE 여부 */
    stlBool                   mInitDeferred;  /**< INITIALLY DEFERRED or INITIALLY IMMEDIATE 여부 */
    stlBool                   mEnforced;      /**< ENFORCED or NOT ENFORCED 여부 */
    stlBool                   mValidate;      /**< VALIDATE or NOVALIDATE 여부 */

    /*
     * 부가적으로 구축하는 정보 
     */ 

    ellDictHandle             mKeyIndexHandle;  /**< Index Dict Handle for Key Constraint */
    
    /*
     * Constraint Type 별 정보 
     */
    void     * mConstTypeDepDesc; /**<
                                   * Constraint Type 별 정보
                                   * <BR> Primary Key    : ellPrimaryKeyDepDesc
                                   * <BR> Foreign Key    : ellForeignKeyDepDesc
                                   * <BR> Unique Key     : ellUniqueKeyDepDesc
                                   * <BR> Check NOT NULL : ellCheckNotNullDepDesc
                                   * <BR> Check Clause   : ellCheckClauseDepDesc
                                   */
} ellTableConstDesc;


/** @} ellObjectConstraint */








/**
 * @defgroup ellObjectView VIEW object
 * @ingroup ellSqlSchemaObject
 * @{
 */

/**
 * @brief Viewed Table 에 종속적인 descriptor
 */
typedef struct ellViewedTableDepDesc
{
    /*
     * DEFINITION_SCHEMA.VIEWS 로부터 획득하는 정보
     */
    
    stlChar * mViewColumnString;  /**< View 의 Column List 구문 정의 */
    stlChar * mViewQueryString;   /**< View 의 SELECT 구문 정의 */

    // stlBool   mCompiled;          /**< View 의 compile 여부 */
    // stlBool   mAffected;          /**< View 의 affected 여부 */

} ellViewedTableDepDesc;


/** @} ellObjectView */





/**
 * @defgroup ellObjectTable TABLE object
 * @ingroup ellSqlSchemaObject
 * @{
 */

/**
 * @brief 테이블 유형
 */
typedef enum ellTableType
{
    ELL_TABLE_TYPE_NA = 0,            /**< Not Available */
    
    ELL_TABLE_TYPE_BASE_TABLE,        /**< Base Table */
    ELL_TABLE_TYPE_VIEW,              /**< View */

    ELL_TABLE_TYPE_GLOBAL_TEMPORARY,  /**< unsupported feature, global temporary table */
    ELL_TABLE_TYPE_LOCAL_TEMPORARY,   /**< unsupported feature, local temporary table */
    ELL_TABLE_TYPE_SYSTEM_VERSIONED,  /**< unsupported feature, system versioned table */ 

    ELL_TABLE_TYPE_SEQUENCE,          /**< Sequence Object */

    ELL_TABLE_TYPE_FIXED_TABLE,       /**< Fixed Table */
    ELL_TABLE_TYPE_DUMP_TABLE,        /**< Dump Table */

    ELL_TABLE_TYPE_SYNONYM,           /**< Synonym Object */
    
    ELL_TABLE_TYPE_MAX
} ellTableType;

extern const stlChar * const gEllTableTypeString[ELL_TABLE_TYPE_MAX];

    
/**
 * @brief Table 관련 Object
 */
typedef enum ellTableRelatedObject
{
    ELL_TABLE_RELATED_NA = 0,              /**< 해당 사항 없음 */

    ELL_TABLE_RELATED_PRIMARY_KEY,         /**< Primary Key */
    ELL_TABLE_RELATED_UNIQUE_KEY,          /**< Unique Key */
    ELL_TABLE_RELATED_FOREIGN_KEY,         /**< Foreign Key */
    ELL_TABLE_RELATED_CHILD_FOREIGN_KEY,   /**< Child Foreign Key */
    ELL_TABLE_RELATED_UNIQUE_INDEX,        /**< Unique Index */
    ELL_TABLE_RELATED_NON_UNIQUE_INDEX,    /**< Non-Unique Index */
    ELL_TABLE_RELATED_CHECK_NOT_NULL,      /**< Check Not Null Constraint */
    ELL_TABLE_RELATED_CHECK_CLAUSE,        /**< Check Clause Constraint */

    ELL_TABLE_RELATED_MAX
} ellTableRelatedObject;

/**
 * @brief Base Table 에 종속적인 descriptor
 * @remarks
 * 참조 ticket #222
 * 관련 Index, Constraint 등의 정보는 Table Cache 의 Hash Data 로 통합되어 관리된다.
 */
typedef struct ellBaseTableDepDesc
{
    stlInt64       mDummy;   /**< Base Table 에만 종속적인 정보가 있다면 추가하자 */
} ellBaseTableDepDesc;


/**
 * @brief Fixed Table 에 종속적인 descriptor
 */
typedef struct ellFixedTableDepDesc
{
    knlStartupPhase   mStartupPhase;    /**< Startup 단계 이상의 단계에서 사용 가능 */
} ellFixedTableDepDesc;

/**
 * @brief Table Descriptor
 */
typedef struct ellTableDesc
{
    /*
     * DEFINITION_SCHEMA.TABLES 로부터 획득하는 정보
     */
    stlInt64        mOwnerID;                 /**< Owner ID */
    stlInt64        mSchemaID;                /**< Schema ID */
    stlInt64        mTableID;                 /**< Table ID */
    stlInt64        mTablespaceID;            /**< Tablespace ID */
    stlInt64        mPhysicalID;              /**< Table Physical ID */
    stlChar       * mTableName;               /**< Table Name */
    ellTableType    mTableType;               /**< Table Type */
    stlBool         mIsSuppPK;                /**< Supplement Logging 여부 */

    /*
     * 부가적으로 구축하는 정보 
     */ 
    void          * mTableHandle;             /**< Table Handle */
    stlInt32        mColumnCnt;               /**< Column Count */
    ellDictHandle * mColumnDictHandle;        /**< Column Dict Handle Array */

    /*
     * Table Type 별 정보 
     */
    union
    {
        ellBaseTableDepDesc       mBaseTable;    /**< Base Table dependent 정보 */
        ellViewedTableDepDesc     mView;         /**< Viewed Table dependent 정보 */
        ellFixedTableDepDesc      mFixedTable;   /**< Fixed Table dependent 정보 */
    } mTableTypeDesc;
    
} ellTableDesc;

/** @} ellObjectTable */




























/**
 * @defgroup ellObjectSequence SEQUENCE GENERATOR object
 * @ingroup ellSqlSchemaObject
 * @{
 */

/**
 * @brief Sequence Descriptor
 */
typedef struct ellSequenceDesc
{
    /*
     * DEFINITION_SCHEMA.SEQUENCES 로부터 획득하는 정보
     */
    stlInt64        mOwnerID;                 /**< Owner ID */
    stlInt64        mSchemaID;                /**< Schema ID */
    stlInt64        mSequenceID;              /**< Sequence ID */
    stlInt64        mSeqTableID;              /**< Naming Resolution Table ID */
    stlInt64        mTablespaceID;            /**< Tablespace ID */
    stlInt64        mPhysicalID;              /**< Sequence Physical ID */
    stlChar       * mSequenceName;            /**< Sequence Name */

    /*
     * 부가적으로 구축하는 정보
     */ 
    void          * mSequenceHandle;          /**< Sequence Handle */
} ellSequenceDesc;

/** @} ellObjectSequence */













/**
 * @defgroup ellObjectSynonym SYNONYM object
 * @ingroup ellSqlSchemaObject
 * @{
 */

/**
 * @brief Synonym Descriptor
 */
typedef struct ellSynonymDesc
{
    /*
     * DEFINITION_SCHEMA.SYNONYMS 로부터 획득하는 정보
     */
    stlInt64        mOwnerID;                 /**< Owner ID */
    stlInt64        mSchemaID;                /**< Schema ID */
    stlInt64        mSynonymID;               /**< Synonym ID */
    stlInt64        mSynonymTableID;          /**< Naming Resolution Table ID */
    stlChar       * mSynonymName;             /**< Synonym Name */
    stlChar       * mObjectSchemaName;        /**< Object Schema Name */
    stlChar       * mObjectName;              /**< Object Name */
    
} ellSynonymDesc;

/** @} ellObjectSynonym */














/**
 * @defgroup ellObjectPublicSynonym PUBLIC SYNONYM object
 * @ingroup ellSqlNonSchemaObject
 * @{
 */

/**
 * @brief Public Synonym Descriptor
 */
typedef struct ellPublicSynonymDesc
{
    /*
     * DEFINITION_SCHEMA.PUBLIC_SYNONYMS 로부터 획득하는 정보
     */
    
    stlInt64        mSynonymID;               /**< Synonym ID */
    stlChar       * mSynonymName;             /**< Synonym Name */
    stlInt64        mCreatorID;               /**< Creator ID */
    stlChar       * mObjectSchemaName;        /**< Object Schema Name */
    stlChar       * mObjectName;              /**< Object Name */
    
} ellPublicSynonymDesc;

/** @} ellObjectSynonym */
















/**
 * @defgroup ellObjectIndex INDEX object
 * @ingroup ellSqlSchemaObject
 * @{
 */

/**
 * @brief 인덱스 유형
 */
typedef enum ellIndexType
{
    ELL_INDEX_TYPE_NA = 0,            /**< Not Available */
    
    ELL_INDEX_TYPE_BTREE,             /**< B-TREE index */
    ELL_INDEX_TYPE_HASH,              /**< HASH index (unsupported feature) */
    ELL_INDEX_TYPE_BITMAP,            /**< BITMAP index (unsupported feature) */
    ELL_INDEX_TYPE_RTREE,             /**< RTREE index (unsupported feature) */

    ELL_INDEX_TYPE_FUNCTION_BTREE,    /**< Function-based B-TREE (unsupported feature) */
    ELL_INDEX_TYPE_FUNCTION_HASH,     /**< Function-based HASH (unsupported feature) */
    ELL_INDEX_TYPE_FUNCTION_BITMAP,   /**< Function-based BITMAP (unsupported feature) */

    ELL_INDEX_TYPE_MAX
} ellIndexType;

extern const stlChar * const gEllIndexTypeString[ELL_INDEX_TYPE_MAX];

/**
 * @brief Index Column 의 Ordering
 */
typedef enum ellIndexColumnOrdering
{
    ELL_INDEX_COLUMN_DESCENDING  = STL_FALSE,  /**< DESCENDING COLUMN */
    ELL_INDEX_COLUMN_ASCENDING   = STL_TRUE    /**< ASCENDING COLUMN */
} ellIndexColumnOrdering;

extern const stlChar * const gEllIndexColumnOrderString[];

/**
 * @brief Index Column Ordering 의 기본값
 */
#define ELL_INDEX_COLUMN_ORDERING_DEFAULT   ( ELL_INDEX_COLUMN_ASCENDING )

/**
 * @brief Index Column 의 Nulls Ordering
 */
typedef enum ellIndexColumnNullsOrdering
{
    ELL_INDEX_COLUMN_NULLS_LAST   = STL_FALSE,  /**< NULLS LAST */
    ELL_INDEX_COLUMN_NULLS_FIRST  = STL_TRUE    /**< NULLS FIRST */
} ellIndexColumnNullsOrdering;

extern const stlChar * const gEllIndexColumnNullsOrderString[];

/**
 * @brief Index Column 의 Nulls Ordering의 기본값은 NULLS LAST
 */
#define ELL_INDEX_COLUMN_NULLS_ORDERING_DEFAULT ( ELL_INDEX_COLUMN_NULLS_LAST )

/**
 * @brief Index Key Descriptor
 */
typedef struct ellIndexKeyDesc
{
    ellDictHandle               mKeyColumnHandle; /**< Key Column 의 Dictionary Handle */
    ellIndexColumnOrdering      mKeyOrdering;     /**< Key Order ASC/DESC */
    ellIndexColumnNullsOrdering mKeyNullOrdering; /**< Null Order NULLS_FIRST/NULLS_LAST */
} ellIndexKeyDesc;

/**
 * @brief Index Descriptor
 */
typedef struct ellIndexDesc
{
    /*
     * DEFINITION_SCHEMA.INDEXES 로부터 획득하는 정보
     */
    stlInt64        mOwnerID;                 /**< Owner ID */
    stlInt64        mSchemaID;                /**< Schema ID */
    stlInt64        mIndexID;                 /**< Index ID */
    stlInt64        mTablespaceID;            /**< Tablespace ID */
    stlInt64        mPhysicalID;              /**< Index Physical ID */
    stlChar       * mIndexName;               /**< Index Name */
    ellIndexType    mIndexType;               /**< Table Type */
    stlBool         mIsUnique;                /**< Unique Index 여부 */
    stlBool         mInvalid;                 /**< Invalid 여부 */
    stlBool         mByConstraint;            /**< Key Constraint 에 의해 생성되었는지의 여부 */
    stlInt64        mDebugTableID;            /**< Debug 용 Table ID */

    /*
     * 부가적으로 구축하는 정보 
     */ 
    void          * mIndexHandle;             /**< Index Handle */
    stlInt32        mKeyColumnCnt;            /**< Key Column 개수 */
    ellIndexKeyDesc mKeyDesc[DTL_INDEX_COLUMN_MAX_COUNT]; /**< Index Key descriptor */
    
    /*
     * Index Type 별 정보 
     */
    void          * mIndexTypeDesc;   /**<
                                       * Index Type 별 정보 : 무조건 NULL
                                       * <BR> BTREE : 현재는 BTREE 만 있어 별도로 관리하지 않음 
                                       */
} ellIndexDesc;

/** @} ellObjectIndex */

/** @} ellSchemaObject */













/**
 * @defgroup ellNonSchemaObject SQL Non-Schema Object
 * @ingroup ellSqlObject
 * @{
 */





/**
 * @defgroup ellObjectSchema SCHEMA object
 * @ingroup ellSqlNonSchemaObject
 * @{
 */

/**
 * @brief Schema Descriptor
 */
typedef struct ellSchemaDesc
{
    /*
     * DEFINITION_SCHEMA.SCHEMATA 로부터 획득하는 정보
     */
    stlInt64        mOwnerID;                 /**< Owner ID */
    stlInt64        mSchemaID;                /**< Schema ID */
    stlChar       * mSchemaName;              /**< Sequence Name */

    /*
     * 부가적으로 구축하는 정보
     */ 
    
} ellSchemaDesc;

/** @} ellObjectSchema */









/**
 * @defgroup ellObjectAuth AUTHORIZATION (User, Role) object
 * @ingroup ellSqlNonSchemaObject
 * @{
 */

/**
 * @brief AUTHORIZATION 유형
 */
typedef enum ellAuthorizationType
{
    ELL_AUTHORIZATION_TYPE_NA = 0,             /**< Not Available */
    
    ELL_AUTHORIZATION_TYPE_USER,               /**< USER Authorization */
    ELL_AUTHORIZATION_TYPE_ROLE,               /**< ROLE Authorization */
    ELL_AUTHORIZATION_TYPE_SYSTEM,             /**< System Authrization */
    ELL_AUTHORIZATION_TYPE_ALL_USER,           /**< All User Authorization */
    
    ELL_AUTHORIZATION_TYPE_MAX
} ellAuthorizationType;

extern const stlChar * const gEllAuthorizationTypeString[ELL_AUTHORIZATION_TYPE_MAX];

/**
 * @brief Authorization Descriptor
 */
typedef struct ellAuthDesc
{
    /*
     * DEFINITION_SCHEMA.AUTHORIZATIONS 로부터 획득하는 정보
     */
    stlInt64               mAuthID;       /**< Authorization ID */
    stlChar              * mAuthName;     /**< Authorization Name */
    ellAuthorizationType   mAuthType;     /**< Authorization Type */
    stlInt64               mProfileID;    /**< User Profile ID */
    
    /*
     * 부가적으로 구축하는 정보
     */

    stlInt64    mDataSpaceID;    /**< for user, default Data Tablespace ID */
    stlInt64    mTempSpaceID;    /**< for user, default Temp Tablespace ID */
    stlInt32    mSchemaPathCnt;  /**< for user and public, Schema Path 개수 */
    stlInt64  * mSchemaIDArray;  /**< Schema ID Array for Schema Path Count */

    stlInt32    mGrantedRoleCnt; /**< Granted Role 개수, applicable role은 포함되지 않음 */
    stlInt64  * mGrantedRoleIDArray;  /**< Granted Role ID Array for Granted Role Count */
    stlBool   * mIsGrantableArray;    /**< Grantable 에 대한 Array for Granted Role Count */
    
} ellAuthDesc;



/**
 * @brief USER 의 account LOCKED status
 */
typedef enum ellUserLockedStatus
{
    ELL_USER_LOCKED_STATUS_NA = 0,                /**< N/A */
    
    ELL_USER_LOCKED_STATUS_OPEN,                  /**< OPEN */
    ELL_USER_LOCKED_STATUS_LOCKED,                /**< LOCKED */
    ELL_USER_LOCKED_STATUS_LOCKED_TIMED,          /**< LOCKED(TIMED) */

    ELL_USER_LOCKED_STATUS_MAX     
} ellUserLockedStatus;

extern const stlChar * const gEllUserLockedString[ELL_USER_LOCKED_STATUS_MAX];

/**
 * @brief USER 의 Password EXPIRY status
 */
typedef enum ellUserExpiryStatus
{
    ELL_USER_EXPIRY_STATUS_NA = 0,                /**< N/A */
    
    ELL_USER_EXPIRY_STATUS_OPEN,                  /**< OPEN */
    ELL_USER_EXPIRY_STATUS_EXPIRED,               /**< EXPIRED */
    ELL_USER_EXPIRY_STATUS_EXPIRED_GRACE,         /**< EXPIRED(GRACE) */

    ELL_USER_EXPIRY_STATUS_MAX     
} ellUserExpiryStatus;

extern const stlChar * const gEllUserExpiryString[ELL_USER_EXPIRY_STATUS_MAX];


/**
 * @brief User Authentication Information
 */
typedef struct ellAuthenInfo
{
    /* ACCOUNT LOCKED/UNLOCKED */
    ellUserLockedStatus mLockedStatus;         /**< ACCOUNT LOCKED status */
    stlTime             mLockedTime;           /**< locked time (if null, ELL_DICT_STLTIME_NA ) */
    stlInt64            mFailedLoginAttempt;   /**< Consecutive failed login attempt count */
    
    /* PASSWORD EXPIRED */
    ellUserExpiryStatus mExpiryStatus;         /**< PASSWORD EXPIRY status */
    stlTime             mExpiryTime;           /**< expiry time (if null, ELL_DICT_STLTIME_NA ) */
    stlChar             mEncryptPassword[STL_MAX_SQL_IDENTIFIER_LENGTH];  /**< Encrypted Password */
    stlInt64            mPassChangeCount;      /**< Pssword change count */
} ellAuthenInfo;

/** @} ellObjectAuth */





/**
 * @defgroup ellObjectProfile PROFILE object
 * @ingroup ellSqlNonSchemaObject
 * @{
 */

/**
 * @brief PROFILE PASSWORD Parameter 
 */
typedef enum ellProfileParam
{
    ELL_PROFILE_PARAM_NA = 0,                    /**< Not Available */
    
    ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS,     /**< FAILED_LOGIN_ATTEMPTS */
    ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME,        /**< PASSWORD_LOCK_TIME  */
    ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME,        /**< PASSWORD_LIFE_TIME */
    ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME,       /**< PASSWORD_GRACE_TIME */
    ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX,        /**< PASSWORD_REUSE_MAX */
    ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME,       /**< PASSWORD_REUSE_TIME */
    ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION,  /**< PASSWORD_VERIFY_FUNCTION */
    
    ELL_PROFILE_PARAM_MAX
} ellProfileParam;

extern const stlChar * const gEllProfileParamString[ELL_PROFILE_PARAM_MAX];


/**
 * @brief PROFILE LIMIT 상수
 */
#define ELL_PROFILE_LIMIT_UNLIMITED             (0)

#define ELL_PROFILE_LIMIT_STRING_UNLIMITED      ("UNLIMITED")
#define ELL_PROFILE_LIMIT_STRING_DEFAULT        ("DEFAULT")
#define ELL_PROFILE_LIMIT_STRING_NULL           ("NULL")

/**
 * @brief built-in PASSWORD_VERIFY_FUNCTION
 * @todo stored function 이 구현되면 제거되어야 함
 */
#define ELL_PROFILE_VERIFY_KISA                 (-1)
#define ELL_PROFILE_VERIFY_KISA_STRING          ("KISA_VERIFY_FUNCTION")
#define ELL_PROFILE_VERIFY_ORA12                (-2)
#define ELL_PROFILE_VERIFY_ORA12_STRING         ("ORA12C_VERIFY_FUNCTION")
#define ELL_PROFILE_VERIFY_ORA12_STRONG         (-3)
#define ELL_PROFILE_VERIFY_ORA12_STRONG_STRING  ("ORA12C_STRONG_VERIFY_FUNCTION")
#define ELL_PROFILE_VERIFY_ORA11                (-4)
#define ELL_PROFILE_VERIFY_ORA11_STRING         ("VERIFY_FUNCTION_11G")
#define ELL_PROFILE_VERIFY_ORACLE               (-5)
#define ELL_PROFILE_VERIFY_ORACLE_STRING        ("VERIFY_FUNCTION")

#define ELL_PROFILE_VERIFY_MIN                  (-6)

/**
 * @brief Profile Descriptor
 */
typedef struct ellProfileDesc
{
    /*
     * DEFINITION_SCHEMA.PROFILES 로부터 획득하는 정보
     */
    
    stlInt64   mProfileID;       /**< Profile ID */
    stlChar  * mProfileName;     /**< Profile Name */
    
    /*
     * DEFINITION_SCHEMA.PROFILE_PASSWORD_PARAMETER 로부터 획득하는 정보 
     */

    /* FAILED_LOGIN_ATTEMPTS */
    stlBool                    mFailedLoginIsDefault;            
    stlInt64                   mFailedLoginAttempt;

    /* PASSWORD_LOCK_TIME */
    stlBool                    mLockTimeIsDefault; 
    dtlDataValue               mLockTimeNumber;          
    stlChar                    mLockTimeBuffer[DTL_NUMERIC_MAX_SIZE];

    /* PASSWORD_LIFE_TIME */
    stlBool                    mLifeTimeIsDefault; 
    dtlDataValue               mLifeTimeNumber;         
    stlChar                    mLifeTimeBuffer[DTL_NUMERIC_MAX_SIZE];

    /* PASSWORD_GRACE_TIME */
    stlBool                    mGraceTimeIsDefault; 
    dtlDataValue               mGraceTimeNumber;               
    stlChar                    mGraceTimeBuffer[DTL_NUMERIC_MAX_SIZE];
    
    /* PASSWORD_REUSE_MAX */
    stlBool                    mReuseMaxIsDefault;   
    stlInt64                   mReuseMax;            

    /* PASSWORD_REUSE_TIME */
    stlBool                    mReuseTimeIsDefault;   
    dtlDataValue               mReuseTimeNumber;               
    stlChar                    mReuseTimeBuffer[DTL_NUMERIC_MAX_SIZE];
    
    /* PASSWORD_VERIFY_FUNCTION */
    stlBool                    mVerifyFunctionIsDefault;   
    stlInt64                   mVerifyFunctionID;        
    
} ellProfileDesc;


/**
 * @brief User 의 Profile Password Parameter (User Profile + Default Profile)
 */
typedef struct ellUserProfileParam
{
    stlInt64       mFailedLogin;   /**< FAILED_LOGIN_ATTEMPTS */ 
    dtlDataValue * mLockTime;      /**< PASSWORD_LOCK_TIME */
    dtlDataValue * mLifeTime;      /**< PASSWORD_LIFE_TIME */
    dtlDataValue * mGraceTime;     /**< PASSWORD_GRACE_TIME */
    stlInt64       mReuseMax;      /**< PASSWORD_REUSE_MAX */
    dtlDataValue * mReuseTime;     /**< PASSWORD_REUSE_TIME */
    stlInt64       mVerifyFuncID;  /**< PASSWORD_VERIFY_FUNCTION */
} ellUserProfileParam;


/** @} ellObjectProfile */








/**
 * @defgroup ellObjectTablespace TABLESPACE object
 * @ingroup ellSqlNonSchemaObject
 * @{
 */

/**
 * @brief TABLESPACE의 Media 유형
 */
typedef enum ellTablespaceMediaType
{
    ELL_SPACE_MEDIA_TYPE_NA = 0,               /**< Not Available */

    ELL_SPACE_MEDIA_TYPE_DISK,                 /**< DISK tablespace */
    ELL_SPACE_MEDIA_TYPE_MEMORY,               /**< MEMORY tablespace */
    ELL_SPACE_MEDIA_TYPE_SSD,                  /**< Solid-State Disk tablespace */
    
    ELL_SPACE_MEDIA_TYPE_MAX
    
} ellTablespaceMediaType;

extern const stlChar * const gEllTablespaceMediaTypeString[ELL_SPACE_MEDIA_TYPE_MAX];

/**
 * @brief TABLESPACE의 Usage 유형
 */
typedef enum ellTablespaceUsageType
{
    ELL_SPACE_USAGE_TYPE_NA = 0,               /**< Not Available */

    ELL_SPACE_USAGE_TYPE_DICT,                 /**< dictionary data tablespace */
    ELL_SPACE_USAGE_TYPE_UNDO,                 /**< undo data tablespace */
    ELL_SPACE_USAGE_TYPE_DATA,                 /**< user data tablespace */
    ELL_SPACE_USAGE_TYPE_TEMPORARY,            /**< temporary data tablespace */
    
    ELL_SPACE_USAGE_TYPE_MAX
    
} ellTablespaceUsageType;

extern const stlChar * const gEllTablespaceUsageTypeString[ELL_SPACE_USAGE_TYPE_MAX];


/**
 * @brief Tablespace Descriptor
 */
typedef struct ellTablespaceDesc
{
    /*
     * DEFINITION_SCHEMA.TABLESPACES 로부터 획득하는 정보
     */
    stlInt64                 mCreatorID;               /**< Creator ID */
    stlInt64                 mTablespaceID;            /**< Tablespace ID */
    stlChar                * mTablespaceName;          /**< Tablespace Name */
    ellTablespaceMediaType   mMediaType;               /**< Media Type */
    ellTablespaceUsageType   mUsageType;               /**< Usage Type */
    
    /*
     * 부가적으로 구축하는 정보
     * Data File 등의 물리적 정보는 Tablespace ID (Handle과 동일) 로부터 얻을 수 있다.
     */ 
    
} ellTablespaceDesc;

/** @} ellObjectTablespace */









/**
 * @defgroup ellObjectDatabase DATABASE object
 * @ingroup ellSqlNonSchemaObject
 * @{
 */

/**
 * @brief Catalog Descriptor
 */
typedef struct ellCatalogDesc
{
    /*
     * DEFINITION_SCHEMA.CATALOG_NAME 로부터 획득하는 정보
     */

    stlInt64                    mCatalogID;         /**< Catalog ID */
    stlChar                   * mCatalogName;       /**< Catalog Name */
    dtlCharacterSet             mCharacterSetID;    /**< Character Set ID */
    dtlStringLengthUnit         mCharLengthUnit;    /**< char length units */
    dtlIntervalDayToSecondType  mTimeZoneInterval;  /**< Time Zone Interval Value */
    
    /*
     * 부가적으로 구축하는 정보
     */ 
    
} ellCatalogDesc;

/** @} ellObjectDatabase */

/** @} ellNonSchemaObject */

/** @} ellSqlObject */

/**
 * @defgroup ellPrivilege Privilege
 * @ingroup elExternal
 * @{
 */

/**
 * @defgroup ellPrivDatabase Database privilege
 * @ingroup ellPrivilege
 * @{
 */

/**
 * @brief Database privilege 유형
 * @todo Session 관련 권한을 정리해야 함.
 */
typedef enum ellDatabasePrivAction
{
    ELL_DB_PRIV_ACTION_NA = 0,                /**< Not Available */

    ELL_DB_PRIV_ACTION_ADMINISTRATION,        /**< ADMINISTRATION database privilege */
    ELL_DB_PRIV_ACTION_ALTER_DATABASE,        /**< ALTER DATABASE database privilege */
    ELL_DB_PRIV_ACTION_ALTER_SYSTEM,          /**< ALTER SYSTEM database privilege */
    ELL_DB_PRIV_ACTION_ACCESS_CONTROL,        /**< ACCESS CONTROL database privilege */
    ELL_DB_PRIV_ACTION_CREATE_SESSION,        /**< CREATE SESSION database privilege */

    ELL_DB_PRIV_ACTION_CREATE_USER,           /**< CREATE USER database privilege */
    ELL_DB_PRIV_ACTION_ALTER_USER,            /**< ALTER USER database privilege */
    ELL_DB_PRIV_ACTION_DROP_USER,             /**< DROP USER database privilege */

    ELL_DB_PRIV_ACTION_CREATE_ROLE,           /**< CREATE ROLE database privilege */
    ELL_DB_PRIV_ACTION_ALTER_ROLE,            /**< ALTER ROLE database privilege */
    ELL_DB_PRIV_ACTION_DROP_ROLE,             /**< DROP ROLE database privilege */

    ELL_DB_PRIV_ACTION_CREATE_TABLESPACE,     /**< CREATE TABLESPACE database privilege */
    ELL_DB_PRIV_ACTION_ALTER_TABLESPACE,      /**< ALTER TABLESPACE database privilege */
    ELL_DB_PRIV_ACTION_DROP_TABLESPACE,       /**< DROP TABLESPACE database privilege */
    ELL_DB_PRIV_ACTION_USAGE_TABLESPACE,      /**< USAGE TABLESPACE database privilege */

    ELL_DB_PRIV_ACTION_CREATE_SCHEMA,         /**< CREATE SCHEMA database privilege */
    ELL_DB_PRIV_ACTION_ALTER_SCHEMA,          /**< ALTER SCHEMA database privilege */
    ELL_DB_PRIV_ACTION_DROP_SCHEMA,           /**< DROP SCHEMA database privilege */

    ELL_DB_PRIV_ACTION_CREATE_ANY_TABLE,      /**< CREATE ANY TABLE database privilege */
    ELL_DB_PRIV_ACTION_ALTER_ANY_TABLE,       /**< ALTER ANY TABLE database privilege */
    ELL_DB_PRIV_ACTION_DROP_ANY_TABLE,        /**< DROP ANY TABLE database privilege */

    ELL_DB_PRIV_ACTION_SELECT_ANY_TABLE,      /**< SELECT ANY TABLE database privilege */
    ELL_DB_PRIV_ACTION_INSERT_ANY_TABLE,      /**< INSERT ANY TABLE database privilege */
    ELL_DB_PRIV_ACTION_DELETE_ANY_TABLE,      /**< DELETE ANY TABLE database privilege */
    ELL_DB_PRIV_ACTION_UPDATE_ANY_TABLE,      /**< UPDATE ANY TABLE database privilege */
    ELL_DB_PRIV_ACTION_LOCK_ANY_TABLE,        /**< LOCK ANY TABLE database privilege */
    
    ELL_DB_PRIV_ACTION_CREATE_ANY_VIEW,       /**< CREATE ANY VIEW database privilege */
    ELL_DB_PRIV_ACTION_DROP_ANY_VIEW,         /**< DROP ANY VIEW database privilege */

    ELL_DB_PRIV_ACTION_CREATE_ANY_SEQUENCE,   /**< CREATE ANY SEQUENCE database privilege */
    ELL_DB_PRIV_ACTION_ALTER_ANY_SEQUENCE,    /**< ALTER ANY SEQUENCE database privilege */
    ELL_DB_PRIV_ACTION_DROP_ANY_SEQUENCE,     /**< DROP ANY SEQUENCE database privilege */
    ELL_DB_PRIV_ACTION_USAGE_ANY_SEQUENCE,    /**< USAGE ANY SEQUENCE database privilege */

    ELL_DB_PRIV_ACTION_CREATE_ANY_INDEX,      /**< CREATE INDEX database privilege */
    ELL_DB_PRIV_ACTION_ALTER_ANY_INDEX,       /**< ALTER INDEX database privilege */
    ELL_DB_PRIV_ACTION_DROP_ANY_INDEX,        /**< DROP INDEX database privilege */

    ELL_DB_PRIV_ACTION_CREATE_ANY_SYNONYM,    /**< CREATE ANY SYNONYM database privilege */
    ELL_DB_PRIV_ACTION_DROP_ANY_SYNONYM,      /**< DROP ANY SYNONYM database privilege */
    ELL_DB_PRIV_ACTION_CREATE_PUBLIC_SYNONYM, /**< CREATE ANY SYNONYM database privilege */
    ELL_DB_PRIV_ACTION_DROP_PUBLIC_SYNONYM,   /**< DROP ANY SYNONYM database privilege */

    ELL_DB_PRIV_ACTION_CREATE_PROFILE,        /**< CREATE PROFILE database privilege */
    ELL_DB_PRIV_ACTION_ALTER_PROFILE,         /**< ALTER PROFILE database privilege */
    ELL_DB_PRIV_ACTION_DROP_PROFILE,          /**< DROP PROFILE database privilege */
    
    ELL_DB_PRIV_ACTION_MAX
} ellDatabasePrivAction;

extern const stlChar * const gEllDatabasePrivActionString[ELL_DB_PRIV_ACTION_MAX];

/** @} ellPrivDatabase */


/**
 * @defgroup ellPrivTablespace Tablespace privilege
 * @ingroup ellPrivilege
 * @{
 */

/**
 * @brief Tablespace Privilege 유형
 */
typedef enum ellSpacePrivAction
{
    ELL_SPACE_PRIV_ACTION_NA = 0,              /**< Not Available */
    
    ELL_SPACE_PRIV_ACTION_CREATE_OBJECT,       /**< CREATE OBJECT tablespace privilege */
    
    ELL_SPACE_PRIV_ACTION_MAX, 
    
} ellSpacePrivAction;

extern const stlChar * const gEllSpacePrivActionString[ELL_SPACE_PRIV_ACTION_MAX];

/** @} ellPrivTablespace */

/**
 * @defgroup ellPrivSchema Schema privilege
 * @ingroup ellPrivilege
 * @{
 */

/**
 * @brief Schema Privilege 유형
 */
typedef enum ellSchemaPrivAction
{
    ELL_SCHEMA_PRIV_ACTION_NA = 0,              /**< Not Available */
    
    ELL_SCHEMA_PRIV_ACTION_CONTROL_SCHEMA,      /**< CONTROL SCHEMA schema privilege */

    ELL_SCHEMA_PRIV_ACTION_CREATE_TABLE,        /**< CREATE TABLE schema privilege */
    ELL_SCHEMA_PRIV_ACTION_ALTER_TABLE,         /**< ALTER TABLE schema privilege */
    ELL_SCHEMA_PRIV_ACTION_DROP_TABLE,          /**< DROP TABLE schema privilege */

    ELL_SCHEMA_PRIV_ACTION_SELECT_TABLE,        /**< SELECT TABLE schema privilege */
    ELL_SCHEMA_PRIV_ACTION_INSERT_TABLE,        /**< INSERT TABLE schema privilege */
    ELL_SCHEMA_PRIV_ACTION_DELETE_TABLE,        /**< DELETE TABLE schema privilege */
    ELL_SCHEMA_PRIV_ACTION_UPDATE_TABLE,        /**< UPDATE TABLE schema privilege */
    ELL_SCHEMA_PRIV_ACTION_LOCK_TABLE,          /**< LOCK TABLE schema privilege */

    ELL_SCHEMA_PRIV_ACTION_CREATE_VIEW,         /**< CREATE VIEW schema privilege */
    ELL_SCHEMA_PRIV_ACTION_DROP_VIEW,           /**< DROP VIEW schema privilege */

    ELL_SCHEMA_PRIV_ACTION_CREATE_SEQUENCE,     /**< CREATE SEQUENCE schema privilege */
    ELL_SCHEMA_PRIV_ACTION_ALTER_SEQUENCE,      /**< ALTER SEQUENCE schema privilege */
    ELL_SCHEMA_PRIV_ACTION_DROP_SEQUENCE,       /**< DROP SEQUENCE schema privilege */
    ELL_SCHEMA_PRIV_ACTION_USAGE_SEQUENCE,      /**< USAGE SEQUENCE schema privilege */

    ELL_SCHEMA_PRIV_ACTION_CREATE_INDEX,        /**< CREATE INDEX schema privilege */
    ELL_SCHEMA_PRIV_ACTION_ALTER_INDEX,         /**< ALTER INDEX schema privilege */
    ELL_SCHEMA_PRIV_ACTION_DROP_INDEX,          /**< DROP INDEX schema privilege */

    ELL_SCHEMA_PRIV_ACTION_ADD_CONSTRAINT,      /**< ADD CONSTRAINT schema privilege */

    ELL_SCHEMA_PRIV_ACTION_CREATE_SYNONYM,      /**< CREATE SYNONYM schema privilege */
    ELL_SCHEMA_PRIV_ACTION_DROP_SYNONYM,        /**< DROP SYNONYM schema privilege */
    
    ELL_SCHEMA_PRIV_ACTION_MAX, 
    
} ellSchemaPrivAction;

extern const stlChar * const gEllSchemaPrivActionString[ELL_SCHEMA_PRIV_ACTION_MAX];

/** @} ellPrivSchema */

/**
 * @defgroup ellPrivTable Table privilege
 * @ingroup ellPrivilege
 * @{
 */

/**
 * @brief Table Privilege 유형
 */
typedef enum ellTablePrivAction
{
    ELL_TABLE_PRIV_ACTION_NA = 0,              /**< Not Available */
    
    ELL_TABLE_PRIV_ACTION_CONTROL_TABLE,       /**< CONTROL TABLE table privilege */
    
    ELL_TABLE_PRIV_ACTION_SELECT,              /**< SELECT table privilege */
    ELL_TABLE_PRIV_ACTION_INSERT,              /**< INSERT table privilege */
    ELL_TABLE_PRIV_ACTION_UPDATE,              /**< UPDATE table privilege */
    ELL_TABLE_PRIV_ACTION_DELETE,              /**< DELETE table privilege */
    ELL_TABLE_PRIV_ACTION_TRIGGER,             /**< TRIGGER table privilege */
    ELL_TABLE_PRIV_ACTION_REFERENCES,          /**< REFERENCES table privilege */

    ELL_TABLE_PRIV_ACTION_LOCK,                /**< LOCK table privilege */
    ELL_TABLE_PRIV_ACTION_INDEX,               /**< INDEX table privilege */
    ELL_TABLE_PRIV_ACTION_ALTER,               /**< ALTER table privilege */

    
    ELL_TABLE_PRIV_ACTION_MAX, 
    
} ellTablePrivAction;

extern const stlChar * const gEllTablePrivActionString[ELL_TABLE_PRIV_ACTION_MAX];
                                         
/** @} ellPrivTable */


/**
 * @defgroup ellPrivUsage Usage (Sequence) privilege
 * @ingroup ellPrivilege
 * @{
 */

/**
 * @brief Usage Privilege의 대상이 되는 Object 유형 
 */
typedef enum ellUsageObjectType
{
    ELL_USAGE_OBJECT_TYPE_NA = 0,

    ELL_USAGE_OBJECT_TYPE_DOMAIN,        /**< DOMAIN object (unsupported feature)*/
    ELL_USAGE_OBJECT_TYPE_CHARACTER_SET, /**< CHARACTER SET object (unsupported feature) */
    ELL_USAGE_OBJECT_TYPE_COLLATION,     /**< COLLATION object (unsupported feature) */
    ELL_USAGE_OBJECT_TYPE_TRANSLATION,   /**< TRANSLATION object (unsupported feature) */
    ELL_USAGE_OBJECT_TYPE_SEQUENCE,      /**< SEQUENCE object */
    
    ELL_USAGE_OBJECT_TYPE_MAX
} ellUsageObjectType;

extern const stlChar * const  gEllUsageObjectTypeString[ELL_USAGE_OBJECT_TYPE_MAX];

/** @} ellPrivUsage */


/**
 * @defgroup ellPrivColumn Column privilege
 * @ingroup ellPrivilege
 * @{
 */

/**
 * @brief Column Privilege 유형
 */
typedef enum ellColumnPrivAction
{
    ELL_COLUMN_PRIV_ACTION_NA = 0,             /**< Not Available */
    
    ELL_COLUMN_PRIV_ACTION_SELECT,             /**< SELECT column privilege */
    ELL_COLUMN_PRIV_ACTION_INSERT,             /**< INSERT column privilege */
    ELL_COLUMN_PRIV_ACTION_UPDATE,             /**< UPDATE column privilege */
    ELL_COLUMN_PRIV_ACTION_REFERENCES,         /**< REFERENCES column privilege */

    ELL_COLUMN_PRIV_ACTION_MAX,
} ellColumnPrivAction;

extern const stlChar * const  gEllColumnPrivActionString[ELL_COLUMN_PRIV_ACTION_MAX];

/** @} ellPrivColumn */


/**
 * @brief Privilege Descriptor
 */
typedef struct ellPrivDesc
{
    stlInt64             mGrantorID;             /**< Revoke Only Key : Grantor ID */
    stlInt64             mGranteeID;             /**< Key : Grantee ID */
    stlInt64             mObjectID;              /**< Key : Usage Object ID */

    ellPrivObject        mPrivObject;            /**< Not Key: Privilege Object */
    stlBool              mWithGrant;             /**< WITH GRANT OPTION */
    union {                                   
        stlInt32               mAction;          /**< Common Action */
        ellDatabasePrivAction  mDatabaseAction;  /**< DB Privilege Action */
        ellSpacePrivAction     mSpaceAction;     /**< Tablespace Privilege Action */
        ellSchemaPrivAction    mSchemaAction;    /**< Schema Privilege Action */
        ellTablePrivAction     mTableAction;     /**< Table Privilege Action */
        ellColumnPrivAction    mColumnAction;    /**< Column Privilege Action */
        ellUsageObjectType     mObjectType;      /**< Usage Object Type */
    } mPrivAction;                               /**< Key: Privilege Action */
} ellPrivDesc;


/** @} ellPrivilege */


/**
 * @defgroup ellSupportSQL SQL Support API
 * @ingroup elExternal
 * @{
 * @remarks
 * QP 와 Gliese API 의 DML 을 지원하기 위해 공통으로 사용할 Library 를 제공한다.
 * - DML 은 다음과 같은 개념으로 수행된다.
 *  - executeTransation()
 *   - beginTransaction();
 *    - executeDML();
 *    - executeDML();
 *    - executeDML();
 *    - CheckDeferredCollision();    //---- Deferred Constraint 위배 여부 검사 
 *   - endTranscation();
 *  - executeDML()
 *   - while( each row )
 *    - CheckNotNull();
 *    - CheckClause();
 *    - InsertUpdateDelete();        //---- Table Value List 구성 
 *    - while ( primary key )
 *     - UpdatePrimaryKeyIndex();    //---- Index Value List 구성 
 *     - CheckPrimaryKey();          //---- Collision 정보 누적 
 *     - while ( primary key 를 참조하는 child foreign key )
 *      - CheckForeignKey();         //---- Collision 정보 누적 
 *    - while ( unique key )
 *     - UpdateUniqueKeyIndex();     //---- Index Value List 구성 
 *     - CheckUniqueKey();           //---- Collision 정보 누적 
 *     - while ( unique key 를 참조하는 child foreign key )
 *      - CheckForeignKey();         //---- Collision 정보 누적 
 *    - while ( foreign key )
 *     - UpdateForeignKeyIndex();    //---- Index Value List 구성 
 *     - CheckForeignKey();          //---- Collision 정보 누적 
 *    - while ( non-key index )
 *     - UpdateNonKeyIndex();        //---- Index Value List 구성 
 *   - CheckImmediateCollision();    //---- Immediate Constraint 위배 여부 검사 
 */

/**
 * @defgroup ellDataValue DML 의 Data Value 구성 API
 * @ingroup ellSupportSQL
 * @{
 * @remarks
 * Table 에서 모든 컬럼을 읽을 것인가?  필요한 컬럼만 읽을 것인가?  (Push Projection)
 * - 다음과 같은 검색 구문에서 읽어야 할 Table Value List 는 무엇인가?
 *  - SELECT t1.i3 FROM t1 WHERE i2 = ( SELECT c3 FROM t2 WHERE t1.i3 = t2.c1 );
 *   - Read Column List : t1.i3 -> t1.i2 -> t2.c3 -> t1.i3 ->t2.c1
 *   - Table Read Value List : Row 검색시 정렬되어 있어야 하며, 중복이 제거되어야 한다.
 *    - t1 Table Value List : t1.i2 -> t1.i3
 *    - t2 Table Value List : t2.c1 -> t2.c3
 * - 다음과 같은 UPDATE 구문 수행시 읽어야 할 Table Value List 와 Index Value List 는 무엇인가?
 *  - UPDATE t1 SET i3 = i5 WHERE i2 = 1;
 *  - Index : idx1 on ( i1, i3 ), idx2 on ( i2, i4 )
 *   - Read Column List : i5 -> i2
 *   - Write Column List : i3
 *   - Table Read Value List : i1 -> i2 -> i3(old) -> i5
 *    - idx1 의 i3 컬럼이 변경되므로 i1 정보도 읽어야 한다.
 *   - Table Write Value List : i3(new)
 *   - idx1 Index Delete Value list : idx1 에서 Key 를 제거하기 위한 값 정보 
 *    - i1 -> i3(old)
 *   - idx1 Index Insert Value list : idx1 에서 key 를 추가하기 위한 값 정보
 *    - i1 -> i3(new)
 *    
 * - 용어 정의
 *  - Read Column List
 *   - SQL 문장 (명확히는 SFWGH 범위내)에 등장하는 읽기위한 컬럼의 목록
 *  - Write Column List
 *   - SQL 문장에 등장하는 쓰기 위한 컬럼의 목록
 *  - Table Read Value List
 *   - Table 에서 값을 읽기 위한 공간의 목록
 *  - Table Write Value List
 *   - Table 에서 값을 쓰기 위한 공간의 목록
 *  - Index Delete Value List
 *   - Index 에서 제거할 Key 를 위한 공간의 목록
 *   - Table Read Value List 와 공간을 공유함.
 *  - Index Insert Value List
 *   - Index 에서 추가할 Key 를 위한 공간의 목록
 *   - Table Read/Write Value List 와 공간을 공유함.
 *  - 비고
 *   - Filter 는 Table Read Value List 와 공간을 공유해야 함.
 *   - Key Range 에서 사용하는 Index 검색 후 RID 를 통해 Table 을 접근하므로,
 *     Index Insert/Delete Value List 와 무관하다.
 */

/**
 * @defgroup ellTableValueList for TABLE read/write value list
 * @ingroup ellDataValue
 * @{
 */

/** @} ellTableValueList */


/**
 * @defgroup ellIndexValueList for INDEX insert/delete value list
 * @ingroup ellDataValue
 * @{
 */

/** @} ellIndexValueList */

/**
 * @defgroup ellRWColumn Read/Write Column List 관리
 * @ingroup ellDataValue
 * @{
 */

/**
 * @brief Read Column List 또는 Write Column List 를 관리하기 위한 자료구조
 * @remarks
 */
typedef struct ellRWColumnList
{
    stlInt32    mColumnCnt;      /**< Column Count */
    stlRingHead mColumnList;     /**< Column List */
} ellRWColumnList;

/**
 * @brief Read/Write Column List 의 Item
 * @remarks
 */
typedef struct ellRWColumnItem
{
    ellDictHandle * mColumnHandle;  /**< Column Handle */
    stlRingEntry    mLink;          /**< Link */
} ellRWColumnItem;

/** @} ellRWColumn */

/** @} ellDataValue */



/** @} ellSupportSQL */










/**
 * @defgroup ellNodeDef Execution Nodes Definition 
 * @ingroup elExternal
 * @{
 */

typedef enum ellNodeType
{
    ELL_NODE_TYPE_COLUMN    = 0,
    ELL_NODE_TYPE_CONSTANT,

    /**
     * FUNCTION / EXPRESTION / ...
     */
    ELL_NODE_TYPE_EXPR,    
    ELL_NODE_TYPE_FUNCTION_EXPR,
    ELL_NODE_TYPE_COMPARISON_EXPR,
    ELL_NODE_TYPE_BOOL_EXPR,
    ELL_NODE_TYPE_SIMPLE_CASE_EXPR,
    ELL_NODE_TYPE_SEARCHED_CASE_EXPR,
    ELL_NODE_TYPE_COALESCE_EXPR,
    ELL_NODE_TYPE_GREATEST_LEAST_EXPR,
    ELL_NODE_TYPE_NULLIF_EXPR,
    ELL_NODE_TYPE_NVL_EXPR
    
} ellNodeType;


typedef struct ellNode
{
    ellNodeType    mType;
} ellNode;

/** @} ellNodeDef */


/**
 * @defgroup ellFunction Function
 * @ingroup elExternal
 * @{
 */

/**
 * @brief function의 input argument count를 얻는 함수 원형.
 * @remark
 *   <BR> 대부분의 builtin함수는 input argument가 고정적이지만,
 *   <BR> IN, ALL, ANY, (a, b, c)=(d,e,f), greatest, least 등과 같은
 *   <BR> 연산자들은 argument의 값이 가변적이다.
 *   <BR> 이러한 가변적인 요소들은 어떤 형태로든지의 정보로부터 그 argument count를 얻어야 한다.
 *   <BR> ellBuiltInFuncInfo.mArgumentCnt == DTL_FUNC_INPUTARG_VARIABLE_CNT 이면,
 *   <BR> 아래 함수포인터로 실제 input argument count를 계산한다. 
 * @todo  
 *   <BR> Goldilocks는 node정보로부터 정보를 얻을 수 있지만,
 *   <BR> Glises는 ??????? Goldilocks와 같은 방법으로 ?????
 */
typedef stlUInt16 (*ellGetArgumentCntPtr)( ellNode * aNodeType );



extern dtlBuiltInFuncInfo gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_MAX ];

extern knlGetBuiltInListFuncPtr gListFunctionPtr[ KNL_LIST_FUNC_END_ID ][ KNL_LIST_FUNCTION_NA ];

/** @} ellFunction */


/**
 * @defgroup ellSessionFunc Session Function
 * @ingroup elExternal
 * @{
 */


/** @} ellSessionFunc */


/**
 * @defgroup ellPseudoCol Pseudo Column
 * @ingroup elExternal
 * @{
 */

/**
 * @brief Pseudo Function Argument
 */
typedef struct ellPseudoArg
{
    void * mPseudoArg;   /**< 자유로운 형변환이 가능한 Argument */
} ellPseudoArg;
    
/**
 * @brief Pseudo Column Function Pointer
 */
typedef stlStatus (*ellPseudoColFuncPtr) ( smlTransId       aTransID,
                                           smlStatement   * aStmt,
                                           smlRid         * aSmlRid,
                                           stlInt64         aTableID,
                                           stlUInt16        aInputArgumentCnt,
                                           ellPseudoArg   * aInputArgument,
                                           dtlDataValue   * aResultValue,
                                           ellEnv         * aEnv );

/**
 * @brief pseudo column에 대한 정보를 관리하는 구조체
 */
typedef struct ellPseudoColInfo
{
    /*
     * Pseudo Column 정보
     */
    
    knlPseudoCol          mPseudoId;         /**< pseudo column ID */
    dtlIterationTime      mIterationTime;    /**< Iteration Time */
    const stlChar         mName[32];         /**< pseudo column name */

    /*
     * Data Type 정보 (knlValueBlock 참조)
     */
    dtlDataType           mDataType;          /**< data type */
    stlInt64              mArgNum1;           /**< 첫번째 숫자 (TYPE 마다 용도가 다름) */
    stlInt64              mArgNum2;           /**< 두번째 숫자 (TYPE 마다 용도가 다름) */
    dtlStringLengthUnit   mStringLengthUnit;  /**< only for STRING 계열 Type */
    dtlIntervalIndicator  mIntervalIndicator; /**< only for INTERVAL Type */
    
    stlUInt16             mArgumentCntMin;   /**< Argument 최소 개수 */
    stlUInt16             mArgumentCntMax;   /**< Argument 최대 개수 */

    ellPseudoColFuncPtr   mFuncPtr;          /**< Function point 얻기 */

} ellPseudoColInfo;


extern ellPseudoColInfo gPseudoColInfoArr[ KNL_PSEUDO_COL_MAX ];


/** @} ellPseudoCol */



/**
 * @defgroup ellSessionObject Session Object
 * @ingroup ellSqlObject
 * @{
 */


/** @} ellSessionObject */

/**
 * @defgroup ellCursor CURSOR
 * @ingroup ellSessionObject
 * @{
 */


/** @} ellCursor */


/**
 * @defgroup  ellCursorProperty  Cursor Property
 * @ingroup ellCursor
 * @{
 */

#define ELL_CURSOR_DEFAULT_SENSITIVITY        (ELL_CURSOR_SENSITIVITY_INSENSITIVE)

#define ELL_CURSOR_DEFAULT_SCROLLABILITY      (ELL_CURSOR_SCROLLABILITY_NO_SCROLL)

#define ELL_CURSOR_DEFAULT_HOLDABILITY        (ELL_CURSOR_HOLDABILITY_UNSPECIFIED)

#define ELL_CURSOR_DEFAULT_UPDATABILITY       (ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY)

#define ELL_CURSOR_DEFAULT_RETURNABILITY      (ELL_CURSOR_RETURNABILITY_WITHOUT_RETURN)

/** @} ellCursorProperty  */




/**
 * @addtogroup ellCursor
 * @{
 */

/**
 * @brief Cursor Standard Type
 */

#define ELL_CURSOR_DEFAULT_STANDARD_TYPE     (ELL_CURSOR_STANDARD_ISO)

/**
 * @brief Cursor Property
 */
typedef struct ellCursorProperty
{
    ellCursorStandardType   mStandardType;     /**< Cursor Standard Type */

    ellCursorSensitivity    mSensitivity;      /**< Cursor Sensitivity */
    ellCursorScrollability  mScrollability;    /**< Cursor Scrollability */
    ellCursorHoldability    mHoldability;      /**< Cursor Holdability */
    ellCursorUpdatability   mUpdatability;     /**< Cursor Updatability */
    ellCursorReturnability  mReturnability;    /**< Cursor Returnability */
} ellCursorProperty;

/**
 * @brief Cursor Origin Type
 */
typedef enum ellCursorOriginType
{
    ELL_CURSOR_ORIGIN_NA = 0,                     /**< N/A */
    
    ELL_CURSOR_ORIGIN_DBC_CURSOR,                 /**< DBC(ODBC,JDBC) Cursor */
    
    ELL_CURSOR_ORIGIN_STANDING_CURSOR,            /**< Standing Cursor */
    ELL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR,    /**< Declared Dynamic Cursor */
    ELL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR,  /**< Extended Statement Cursor */
    ELL_CURSOR_ORIGIN_RECEIVED_CURSOR,            /**< Received Cursror */

    ELL_CURSOR_ORIGIN_MAX
    
} ellCursorOriginType;


/**
 * @brief Cursor Hash Key
 */
typedef struct ellCursorHashKey
{
    stlChar  * mCursorName;  /**< Cursor Name (hash key) */
    stlInt64   mProcID;      /**< @todo PSM 내에서 declare cursor 가 사용되면 필요함 */
} ellCursorHashKey;


    
/**
 * @brief Cursor Declaration Descriptor
 */
typedef struct ellCursorDeclDesc
{
    stlRingEntry          mLink;             /**< link */
    stlInt32              mSlotID;           /**< Cursor Slot ID */
    ellCursorHashKey      mCursorKey;        /**< Cursor Hash Key */
    
    ellCursorOriginType   mOriginType;       /**< Cursor Origin Type */
    stlChar             * mCursorOrigin;     /**< Cursor Origin 
                                             <BR> STANDING : SQL Text
                                             <BR> ODBC, JDBC: NULL
                                             <BR> DYNAMIC  : statement name
                                             <BR> EXTENDED : prepared statement name
                                             <BR> RECEIVED : specific routine designator */

    ellCursorProperty     mCursorProperty;   /**< Cursor Property */

} ellCursorDeclDesc;


/**
 * @brief Cursor Instance Descriptor
 */
typedef struct ellCursorInstDesc
{
    stlRingEntry            mHashLink;         /**< Hash Item link */
    stlRingEntry            mListLink;         /**< Linked List Item link */
    
    ellCursorHashKey        mCursorKey;        /**< Cursor Hash Key */
    
    ellCursorDeclDesc     * mDeclDesc;         /**< Cursor Declaration Desciptor */
    
    stlBool                 mIsOpen;           /**< Cursor OPENED or CLOSED */

    /*******************************
     * OPEN 구문의 정보 
     *******************************/

    smlStatement          * mCursorStmt;       /**< OPEN 수행시 SM Statement */
    void                  * mCursorDBCStmt;    /**< Cursor DBC Statement */
    void                  * mCursorSQLStmt;    /**< Cursor SQL Statement */
} ellCursorInstDesc;



/** @} ellCursor */



/**
 * @addtogroup ellSessionObject Session Object
 * @{
 */

#define ELL_MAX_NAMED_CURSOR_QUERY_SIZE    (1024)

/**
 * x$named_cursor 를 위한 정보
 */
typedef struct ellNamedCursorSlot
{
    stlBool             mIsOpen;                                          /**< Open 여부 */
    stlChar             mCursorName[STL_MAX_SQL_IDENTIFIER_LENGTH];       /**< Cursor Name */
    stlTime             mOpenTime;                                        /**< Cursor Open Time */
    smlScn              mViewScn;                                         /**< Cursor View SCN */
    ellCursorProperty   mCursorProperty;                                  /**< Cursor Property */
    stlChar             mCursorQuery[ELL_MAX_NAMED_CURSOR_QUERY_SIZE];    /**< Cursor Query */
} ellNamedCursorSlot;
    
/**
 * @brief Session Object Manager
 */
typedef struct ellSessObjectMgr
{
    knlDynamicMem       mMemSessObj;       /**< Session Object Memory 관리자  */
    knlRegionMem        mMemSessHash;      /**< Session Hash Memory 관리자  */

    knlStaticHash     * mHashDeclCursor;   /**< Cursor Declaration Hash */
    knlStaticHash     * mHashInstCursor;   /**< Cursor Instance Hash */

    ellCursorDeclDesc * mLatestDeclCursor;   /**< 가장 최근의 Cursor Declaration Descriptor */
    ellCursorInstDesc * mLatestInstCursor;   /**< 가장 최근의 Cursor Instance Descriptor */
    
    stlRingHead         mOpenCursorList;     /**< Open Cursor List */

    /*
     * Shared 영역에서 관리되어 fixed table 로 조회가 가능함.
     */
    
    knlDynamicMem         mMemCursorSlot;     /**< Cursor Slot 을 위한 Shared Memory 영역의 관리자 */
    stlInt32              mMaxCursorCount;    /**< 최대 Named Cursor 개수 */
    stlInt32              mUsedCursorCount;   /**< 할당된 Named Cursor 개수 */
    stlInt32              mNextCursorSlot;    /**< Next Named Cursor Slot */
    ellNamedCursorSlot ** mCursorSlot;        /**< Cursor Slot Pointer Array */
    
} ellSessObjectMgr;
    
/** @} ellSessionObject */


/**
 * @defgroup ellTemporaryTable TEMPORARY TABLE
 * @ingroup ellSessionObject
 * @{
 */


/** @} ellTemporaryTable */







/**
 * @defgroup ellSessEnv Session Environment
 * @ingroup ellEnvironment
 * @{
 */


/**
 * @brief Execution Library Session Environment
 * - DDL stmt 는 하나의 Session 에서 동시에 수행될 수 없다.
 */
typedef struct ellSessionEnv
{
    smlSessionEnv    mSmlSessionEnv;   /**< Storage Manager Session Environment */

    /*
     * Authorization Information
     */
    
    ellDictHandle    mLoginUser;     /**< Login User */
    ellDictHandle    mSessionUser;   /**< Session User : SET SESSION AUTHORIZATION 명령으로 변경됨 */
    ellDictHandle    mCurrentUser;   /**< Current User : View, PSM 사용시 변경됨 */

    stlBool          mIsSetRole;     /**< Set Role 여부 */
    ellDictHandle    mSessionRole;   /**< Session Role : SET ROLE 명령으로 변경됨 */
    
    stlInt64         mLoginAuthID;   /**< fixed table 동시성 조회를 위해 중복 관리함 */
    stlInt64         mSessionAuthID; /**< fixed table 동시성 조회를 위해 중복 관리함 */

    /*
     * Session Object 
     */

    ellSessObjectMgr mSessObjMgr;      /**< Session Object Manager */
    
    /*
     * DDL Statement
     */
    
    stlTime   mTimeDDL;                 /**< DDL Statement 가 발생한 시간 */
    stlInt64  mStmtTimestamp;           /**< DDL Statement 의 Timestamp */
    void    * mStmtRollbackPendOpList;  /**< Stmt Rollback 시 수행할 Pending OP list */
    void    * mStmtSuccessPendOpList;   /**< Stmt Success  시 수행할 Pending OP list */
    void    * mStmtPreRollbackActionList;   /**< Stmt Success  시 수행할 Pre Action list */

    /*
     * DDL Transaction 
     */
    
    stlBool          mIncludeDDL;               /**< Transaction 내에 DDL 구문이 발생했는지의 여부 */
    stlUInt64        mLocalModifySeq;           /**< Transaction 내에 DDL 구문의 마지막 Sequence */
    stlRingHead      mLocalCache;               /**< Local Cache */
    
    void           * mTxPreRollbackActionList;  /**< Tx Rollback 전 수행할 Action List */
    void           * mTxRollbackPendOpList;     /**< Tx Rollback 시 수행할 Pending OP List */
    void           * mTxCommitPendOpList;       /**< Tx Commit 시 수행할 Pending OP List */
} ellSessionEnv;

/**
 * @brief Execution Library Session Environment 를 얻는다.
 * @param[in]  aEnv Process Environment 포인터
 */
#define ELL_SESS_ENV(aEnv)    ((ellSessionEnv*)(KNL_ENV(aEnv)->mSessionEnv))
#define ELL_SESS_OBJ(aEnv)    ( & (ELL_SESS_ENV(aEnv)->mSessObjMgr) )

/**
 * Session 의 DDL 포함여부
 */
#define ELL_SESS_HAS_DDL(aEnv) (((ellSessionEnv*)(KNL_ENV(aEnv)->mSessionEnv))->mIncludeDDL)

/** @} ellSessEnv */


/**
 * @defgroup ellSharedEntry Shared Entry
 * @ingroup ellEnvironment
 * @{
 */

typedef struct eldcDictHash eldcDictHash;

typedef struct ellCacheEntry
{
    knlDynamicMem  mMemDictCache;       
    knlDynamicMem  mMemDictHashElement; 
    knlDynamicMem  mMemDictHashRelated; 
    knlDynamicMem  mMemDictRelatedTrans;
    knlDynamicMem  mMemPendOP;          

    ellDictHandle  mCatalogHandle;       /**< Catalog Handle */
    
    eldcDictHash ** mObjectCache;         /**< SQL-Object Cache 를 위한 Hash 객체 */
    eldcDictHash ** mPrivCache;           /**< Privilege Cache 를 위한 Hash 객체 */
} ellCacheEntry;
    
/**
 * @brief Execution Library 에서 공유하는 자료 구조 
 * - Dictionary Cache 관리        : Dynamic Shared Memory, 가변길이, DB lifetime
 * - Dictionary Hash Element 관리 : Dynamic Shared Memory, 고정길이, DB lifetime
 * - Dictionary Hash Related 관리 : Dynamic Shared Memory, 고정길이, DB lifetime
 * - Pending Operation 관리       : Dynamic Shared Memory, 가변길이, Transaction lifetime
 */
typedef struct ellSharedEntry
{
    ellCacheEntry * mCurrCacheEntry;                           /**< 현재 단계의 Cache Entry */
    ellCacheEntry   mPhaseCacheEntry[KNL_STARTUP_PHASE_MAX];   /**< Start-Up 단계별 Cache Entry */
    
    void        ** mDictPhysicalHandle;         /**< Dict 들의 table handle */
    void        ** mDictIdentityColumnHandle;   /**< Dict 들의 identity handle */
    
} ellSharedEntry;

/**
 * @brief Shared Memory 상의 Execution Library 를 위한 Memory 시작 공간
 */ 
extern ellSharedEntry * gEllSharedEntry;

/**
 * @brief Cache Build 를 위한 Cache Entry
 */
extern ellCacheEntry * gEllCacheBuildEntry;  

#define ELL_MEM_DICT_CACHE                                  \
    ( ( gEllCacheBuildEntry == NULL )                       \
      ? & gEllSharedEntry->mCurrCacheEntry->mMemDictCache   \
      : & gEllCacheBuildEntry->mMemDictCache )              

#define ELL_MEM_HASH_ELEMENT                                    \
    ( ( gEllCacheBuildEntry == NULL )                           \
      ? & gEllSharedEntry->mCurrCacheEntry->mMemDictHashElement \
      : & gEllCacheBuildEntry->mMemDictHashElement )              

#define ELL_MEM_HASH_RELATED                                    \
    ( ( gEllCacheBuildEntry == NULL )                           \
      ? & gEllSharedEntry->mCurrCacheEntry->mMemDictHashRelated \
      : & gEllCacheBuildEntry->mMemDictHashRelated )                                                       

#define ELL_MEM_RELATED_TRANS                                       \
    ( ( gEllCacheBuildEntry == NULL )                               \
      ? & gEllSharedEntry->mCurrCacheEntry->mMemDictRelatedTrans    \
      : & gEllCacheBuildEntry->mMemDictRelatedTrans )                                                       

#define ELL_MEM_PEND_OP                                 \
    ( gEllCacheBuildEntry == NULL )                     \
    ? & gEllSharedEntry->mCurrCacheEntry->mMemPendOP    \
    : & gEllCacheBuildEntry->mMemPendOP               

#define ELL_OBJECT_CACHE( aObjectCacheNO )                              \
    ( gEllCacheBuildEntry == NULL )                                     \
    ? gEllSharedEntry->mCurrCacheEntry->mObjectCache[aObjectCacheNO]    \
    : gEllCacheBuildEntry->mObjectCache[aObjectCacheNO]            

#define ELL_OBJECT_CACHE_ADDR( aObjectCacheNO )                         \
    ( gEllCacheBuildEntry == NULL )                                     \
    ? & gEllSharedEntry->mCurrCacheEntry->mObjectCache[aObjectCacheNO]  \
    : & gEllCacheBuildEntry->mObjectCache[aObjectCacheNO]            

#define ELL_PRIV_CACHE( aPrivCacheNO )                              \
    ( gEllCacheBuildEntry == NULL )                                 \
    ? gEllSharedEntry->mCurrCacheEntry->mPrivCache[aPrivCacheNO]    \
    : gEllCacheBuildEntry->mPrivCache[aPrivCacheNO]            

#define ELL_PRIV_CACHE_ADDR( aPrivCacheNO )                         \
    ( gEllCacheBuildEntry == NULL )                                 \
    ? & gEllSharedEntry->mCurrCacheEntry->mPrivCache[aPrivCacheNO]  \
    : & gEllCacheBuildEntry->mPrivCache[aPrivCacheNO]            

#define ELL_CATALOG_HANDLE_ADDR()                           \
    ( gEllCacheBuildEntry == NULL )                         \
    ? & gEllSharedEntry->mCurrCacheEntry->mCatalogHandle    \
    : & gEllCacheBuildEntry->mCatalogHandle            

/**
 * @brief Dictionary Cache 를 위한 메모리 관리자의 메모리 크기의 단위
 * - 단위 Dictionary Cache 의 크기가 최소 90 K 에 달한다.
 */

#define ELL_DICT_CACHE_NOMOUNT_MEM_INIT_SIZE               ( 8 * 1024 )
#define ELL_DICT_CACHE_NOMOUNT_MEM_NEXT_SIZE               ( 8 * 1024 )

#define ELL_DICT_CACHE_MOUNT_MEM_INIT_SIZE                 ( 8 * 1024 )
#define ELL_DICT_CACHE_MOUNT_MEM_NEXT_SIZE                 ( 8 * 1024 )

#define ELL_DICT_CACHE_OPEN_MEM_INIT_SIZE                  ( 2 * 1024 * 1024 )
#define ELL_DICT_CACHE_OPEN_MEM_NEXT_SIZE                  ( 128 * 1024 )

/**
 * @brief Dictionary Cache 의 Hash Element 을 위한 메모리 관리자의 메모리 크기의 단위 
 */

#define ELL_DICT_CACHE_NOMOUNT_HASH_ELEMENT_MEM_INIT_SIZE  ( 8 * 1024 )
#define ELL_DICT_CACHE_NOMOUNT_HASH_ELEMENT_MEM_NEXT_SIZE  ( 8 * 1024 )

#define ELL_DICT_CACHE_MOUNT_HASH_ELEMENT_MEM_INIT_SIZE    ( 8 * 1024 )
#define ELL_DICT_CACHE_MOUNT_HASH_ELEMENT_MEM_NEXT_SIZE    ( 8 * 1024 )

#define ELL_DICT_CACHE_OPEN_HASH_ELEMENT_MEM_INIT_SIZE     ( 1024 * 1024 )
#define ELL_DICT_CACHE_OPEN_HASH_ELEMENT_MEM_NEXT_SIZE     ( 128 * 1024 )

/**
 * @brief Dictionary Cache 의 Hash Related Object List 를 위한 메모리 관리자의 메모리 크기의 단위 
 */

#define ELL_DICT_CACHE_NOMOUNT_HASH_RELATED_MEM_INIT_SIZE  ( 8 * 1024 )
#define ELL_DICT_CACHE_NOMOUNT_HASH_RELATED_MEM_NEXT_SIZE  ( 8 * 1024 )

#define ELL_DICT_CACHE_MOUNT_HASH_RELATED_MEM_INIT_SIZE    ( 8 * 1024 )
#define ELL_DICT_CACHE_MOUNT_HASH_RELATED_MEM_NEXT_SIZE    ( 8 * 1024 )

#define ELL_DICT_CACHE_OPEN_HASH_RELATED_MEM_INIT_SIZE     ( 128 * 1024 )
#define ELL_DICT_CACHE_OPEN_HASH_RELATED_MEM_NEXT_SIZE     ( 128 * 1024 )

/**
 * @brief Dictionary Cache 의 Hash Related Object를 수정하는 트랜잭션들을 위한
 *        메모리 관리자의 메모리 크기의 단위 
 */

#define ELL_DICT_CACHE_OPEN_HASH_RELATED_TRANS_MEM_INIT_SIZE  ( 8 * 1024 )
#define ELL_DICT_CACHE_OPEN_HASH_RELATED_TRANS_MEM_NEXT_SIZE  ( 8 * 1024 )


/**
 * @brief Pending Operation을 위한 메모리 관리자의 메모리 크기의 단위 
 */

#define ELL_PEND_OP_NOMOUNT_MEM_INIT_SIZE                  ( 8 * 1024 )
#define ELL_PEND_OP_NOMOUNT_MEM_NEXT_SIZE                  ( 8 * 1024 )

#define ELL_PEND_OP_MOUNT_MEM_INIT_SIZE                    ( 8 * 1024 )
#define ELL_PEND_OP_MOUNT_MEM_NEXT_SIZE                    ( 8 * 1024 )

#define ELL_PEND_OP_OPEN_MEM_INIT_SIZE                     ( 32 * 1024 )
#define ELL_PEND_OP_OPEN_MEM_NEXT_SIZE                     ( 32 * 1024 )


/** @} ellSharedEntry */



/** @} elExternal */





#endif /* _ELLDEF_H_ */
