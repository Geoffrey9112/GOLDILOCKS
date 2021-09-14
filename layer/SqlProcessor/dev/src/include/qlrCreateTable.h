/*******************************************************************************
 * qlrCreateTable.h
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

#ifndef _QLR_CREATE_TABLE_H_
#define _QLR_CREATE_TABLE_H_ 1

/**
 * @file qlrCreateTable.h
 * @brief CREATE TABLE
 */


/****************************************************
 * CREATE TABLE Nodes
 ****************************************************/

typedef struct qlrInitColumn              qlrInitColumn;
typedef struct qlrInitConst               qlrInitConst;
typedef struct qlrInitCreateTable         qlrInitCreateTable;
typedef struct qlrCodeCreateTable         qlrCodeCreateTable;
typedef struct qlrDataCreateTable         qlrDataCreateTable;

/**
 * @brief Init Plan: Column Definition List 
 */
struct qlrInitColumn
{
    /* Column 정보 */
    
    stlChar            * mColumnName;        /**< Column Name */
    stlChar            * mColumnDefault;     /**< Default Clause */
    stlBool              mNullDefault;       /**< Default NULL 여부 */
    qlvInitExpression  * mInitDefault;       /**< DEFAULT init expression */
    stlInt32             mPhysicalOrdinalPosition;   /**< Physical Ordinal Position */
    stlInt32             mLogicalOrdinalPosition;    /**< Logical Ordinal Position */
    stlBool              mIsNullable;        /**< Nullable */
    stlBool              mIsIdentity;        /**< Identity Column 여부 */
    stlInt16             mColLength;         /**< Column Buffer Length : columnar table only */
    ellIdentityGenOption mIdentityOption;    /**< Identity Generation Option */
    smlSequenceAttr      mIdentityAttr;      /**< Identity Attribute */

    /* Column Type 정보 */
    qlvInitTypeDef       mTypeDef;

    struct qlrInitColumn  * mNext;
};

/**
 * @brief Init Plan: Table Constraint List
 */
struct qlrInitConst
{
    /* Table Constraint 공통 정보 */
    
    ellTableConstraintType    mConstType;          /**< 제약조건 유형 */
    stlBool                   mDeferrable;         /**< Deferrable 여부 */
    stlBool                   mInitDeferred;       /**< Initially Deferred */
    stlBool                   mEnforced;           /**< Enforced(Enable) 여부 */

    stlInt32                  mColumnCnt;          /**< Constraint가 걸린 컬럼 개수 */
    stlInt32                  mColumnIdx[DTL_INDEX_COLUMN_MAX_COUNT];     /**< for non-exist column */
    ellDictHandle             mColumnHandle[DTL_INDEX_COLUMN_MAX_COUNT];  /**< for exist column */

    ellDictHandle             mConstSchemaHandle;  /**< 제약조건의 Schema Handle */
    stlChar                 * mConstName;          /**< 제약조건의 이름 */
    
    /* only for key constraints */
    
    /* ellDictHandle          mIndexSchemaHandle;   Key 인덱스의 Schema Handle 은 Key 와 동일함 */
    ellDictHandle             mIndexSpaceHandle;   /**< 인덱스의 Tablespace Handle */
    stlChar                 * mIndexName;          /**< 제약조건에 대응하는 인덱스 이름 */
    smlIndexAttr              mIndexAttr;          /**< 인덱스 Physical Attribute */
    stlUInt16                 mIndexParallel;      /**< 인덱스 Parallel Factor */
    stlUInt8                  mIndexKeyFlag[DTL_INDEX_COLUMN_MAX_COUNT];


    /* only for check not null, check clause */
    
    stlChar                 * mCheckClause;

    /* only for Foreign Key */
    
    ellDictHandle             mRefColumnHandle[DTL_INDEX_COLUMN_MAX_COUNT];
    
    ellReferentialMatchOption mMatchOption;   /**< Match Option */
    ellReferentialActionRule  mUpdateRule;    /**< Update Referential Action */
    ellReferentialActionRule  mDeleteRule;    /**< Delete Referential Action */

    struct qlrInitConst  * mNext;
};


/**
 * @brief Init Plan: Table Constraint List
 */
typedef struct qlrInitTableConstraint
{
    ellObjectList    * mConstSchemaList;   /**< Constraint Schema Handle List */
    ellObjectList    * mConstSpaceList;    /**< Constraint Tablespace Handle List */
    
    qlrInitConst     * mPrimaryKey;        /**< Primary Key Constraint List */
    qlrInitConst     * mUniqueKey;         /**< Unique Key Constraint List */
    qlrInitConst     * mForeignKey;        /**< Foreign Key Constraint List */
    qlrInitConst     * mCheckNotNull;      /**< Check Not Null Constraint List */
    qlrInitConst     * mCheckClause;       /**< Check Clause Constraint List */
} qlrInitTableConstraint;


/**
 * @brief CREATE TABLE 을 위한 Init Plan
 */
struct qlrInitCreateTable
{
    qlvInitPlan        mCommonInit;        /**< Common Init Plan */
    
    ellDictHandle      mSpaceHandle;       /**< Tablespace Handle */
    ellDictHandle      mSchemaHandle;      /**< Schema Handle */
    stlChar          * mTableName;         /**< Table Name */
    smlRelationType    mTableType;         /**< Table Type */
    stlInt32           mColCount;          /**< Column Count */
    stlInt16         * mColLengthArray;    /**< Column Length Array - columnar table only */

    stlBool            mHasIdentity;       /**< Identity Column 존재 여부 */

    qlrInitColumn    * mColumnList;        /**< Column Definition List */

    qlrInitTableConstraint mAllConstraint; /**< All Table Constraints */

    smlTableAttr       mPhysicalAttr;      /**< Table Physical Attribute */

    qlvInitNode      * mSubQueryInitNode;  /**< AS Subquery */
    stlBool            mWithData;          /**< AS Subquery 일 때에만 사용 */
    ellObjectList    * mUnderlyingRelationList;  /**< Subquery Relations */
};


/**
 * @brief CREATE TABLE 을 위한 Data Plan
 */

struct qlrDataCreateTable
{
    /*
     * Table 상위 객체
     */

    stlInt64              mTableOwnerID;
    stlInt64              mTableSchemaID;
    stlInt64              mTableSpaceID;

    /*
     * Table 객체
     */

    stlInt64              mTableID;
    stlInt64              mTablePhyID;
    void                * mTablePhyHandle;

    /*
     * Table Element 객체 
     */

    stlInt32              mColumnCount;
    stlInt64            * mColumnID;
    ellAddColumnDesc    * mAddColumnDesc;

    
    /*
     * For CREATE TABLE AS SELECT 
     */
    qllDataArea         * mDataArea; /* INSERT SELECT를 위한 Data Area */
    smlRowBlock         * mRowBlock; /* INSERT 할 Row 관련 정보를 저장할 Block */

};



stlStatus qlrValidateCreateTable( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qlrOptCodeCreateTable( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlrOptDataCreateTable( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlrExecuteCreateTable( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );


stlStatus qlrExecuteAddColumnDesc( smlTransId           aTransID,
                                   smlStatement       * aStmt,
                                   stlInt64             aTableOwnerID,
                                   stlInt64             aTableSchemaID,
                                   stlInt64             aTableID,
                                   ellTableType         aTableType,
                                   qlrInitColumn      * aCodeColumn,
                                   ellAddColumnDesc   * aAddColumnDesc,
                                   stlInt64           * aColumnID,
                                   qllEnv             * aEnv );

stlStatus qlrExecuteAddKeyIndex( smlTransId         aTransID,
                                 smlStatement     * aStmt,
                                 qllDBCStmt       * aDBCStmt,
                                 stlInt64           aTableOwnerID,
                                 stlInt64           aTableSchemaID,
                                 stlInt64           aTableID,
                                 void             * aTablePhyHandle,
                                 qlrInitConst     * aCodeConst,
                                 stlInt64         * aKeyColumnID,
                                 stlInt32         * aKeyColumnPos,
                                 ellAddColumnDesc * aNewColumnDesc,
                                 stlInt64         * aIndexID,
                                 qllEnv           * aEnv );

stlStatus qlrExecuteAddKeyConst( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 stlInt64        aTableOwnerID,
                                 stlInt64        aTableSchemaID,
                                 stlInt64        aTableID,
                                 qlrInitConst  * aCodeConst,
                                 stlInt64        aKeyIndexID,
                                 stlInt64      * aKeyColumnID,
                                 qllEnv        * aEnv );

stlStatus qlrExecuteAddCheckConst( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   stlInt64        aTableOwnerID,
                                   stlInt64        aTableSchemaID,
                                   stlInt64        aTableID,
                                   qlrInitConst  * aCodeConst,
                                   stlInt64        aCheckColumnID,
                                   qllEnv        * aEnv );

/*
 * Validation 보조 함수
 */

stlStatus qlrValidateCreateTableCommon( smlTransId            aTransID,
                                        qllDBCStmt          * aDBCStmt,
                                        qllStatement        * aSQLStmt,
                                        stlChar             * aSQLString,
                                        qlpTableDef         * aParseTree,
                                        qlrInitCreateTable ** aInitPlan,
                                        qlvInitExprList    ** aQueryExprList,
                                        qllEnv              * aEnv );

stlStatus qlrAddColumnDefinition( smlTransId                aTransID,
                                  qllDBCStmt              * aDBCStmt,
                                  qllStatement            * aSQLStmt,
                                  stlChar                 * aSQLString,
                                  qlvInitExprList         * aDefaultExprList,
                                  ellDictHandle           * aSchemaHandle,
                                  ellDictHandle           * aTableHandle,
                                  smlRelationType           aRelationType,
                                  stlChar                 * aTableName,
                                  qlpColumnDef            * aColumnDef,
                                  stlBool                 * aHasIdentity,
                                  qlrInitColumn          ** aInitColumnList,
                                  qlrInitTableConstraint  * aAllConstraint,
                                  qllEnv                  * aEnv );

stlStatus qlrMakeDefaultNullNode( qllDBCStmt  * aDBCStmt, 
                                  qlpValue   ** aDefaultNullNode,
                                  qllEnv      * aEnv );

stlStatus qlrAddInlineConstDefinition( smlTransId               aTransID,
                                       qllDBCStmt             * aDBCStmt,
                                       qllStatement           * aSQLStmt,
                                       stlChar                * aSQLString,
                                       ellDictHandle          * aSchemaHandle,
                                       ellDictHandle          * aTableHandle,
                                       stlChar                * aTableName,
                                       qlrInitColumn          * aInitColumnList,
                                       qlrInitTableConstraint * aAllConstraint,
                                       qlrInitColumn          * aCodeColumn,
                                       qlpList                * aConstList,
                                       qllEnv                 * aEnv );
                            
stlStatus qlrAddOutlineConstDefinition( smlTransId               aTransID,
                                        qllDBCStmt             * aDBCStmt,
                                        qllStatement           * aSQLStmt,
                                        stlChar                * aSQLString,
                                        ellDictHandle          * aSchemaHandle,
                                        ellDictHandle          * aTableHandle,
                                        stlChar                * aTableName,
                                        qlrInitColumn          * aInitColumnList,
                                        qlrInitTableConstraint * aAllConstraint,
                                        qlpList                * aConstList,
                                        qllEnv                 * aEnv );


stlStatus qlrSetConstCharacteristics( stlChar       * aSQLString,
                                      qlpConstraint * aParseConst,
                                      qlrInitConst  * aInitConst,
                                      qllEnv        * aEnv );

void qlrSetRefineColumnType( qlvInitTypeDef * sSrcType,
                             qlvInitTypeDef * sDstType );



/*
 * Execution 보조 함수
 */



stlStatus qlrValidateCheckNotNull( smlTransId               aTransID,
                                   qllDBCStmt             * aDBCStmt,
                                   qllStatement           * aSQLStmt,
                                   stlChar                * aSQLString,
                                   ellDictHandle          * aSchemaHandle,
                                   stlChar                * aTableName,
                                   qlrInitColumn          * aInitColumnList,
                                   qlrInitTableConstraint * aAllConstraint,
                                   qlrInitConst           * aCodeConst,
                                   qlpConstraint          * aConstDef,
                                   qllEnv                 * aEnv );

stlStatus qlrValidateUniquePrimaryKey( smlTransId               aTransID,
                                       qllDBCStmt             * aDBCStmt,
                                       qllStatement           * aSQLStmt,
                                       stlBool                  aIsPrimary,
                                       stlChar                * aSQLString,
                                       ellDictHandle          * aSchemaHandle,
                                       ellDictHandle          * aTableHandle,
                                       stlChar                * aTableName,
                                       qlrInitColumn          * aInitColumnList,
                                       qlrInitTableConstraint * aAllConstraint,
                                       qlrInitConst           * aCodeConst,
                                       qlpConstraint          * aConstDef,
                                       qllEnv                 * aEnv );

stlStatus qlrCheckSameConstNameInTableDef( qlrInitTableConstraint * aAllConstraint,
                                           ellDictHandle          * aSchemaHandle,
                                           stlChar                * aNewName,
                                           stlBool                * aObjectExist,
                                           qllEnv                 * aEnv );

stlStatus qlrCheckSameIndexNameInTableDef( qlrInitTableConstraint * aAllConstraint,
                                           ellDictHandle          * aSchemaHandle,
                                           stlChar                * aNewName,
                                           stlBool                * aObjectExist,
                                           qllEnv                 * aEnv );

stlStatus qlrCheckSameUniqueKeyInTableDef( qlrInitTableConstraint * aAllConstraint,
                                           qlrInitConst           * aCodeConst,
                                           stlBool                * aObjectExist,
                                           qllEnv                 * aEnv );

stlStatus qlrValidateSegmentAttr( stlChar         * aSQLString,
                                  smlTbsId          aSpacePhyID,
                                  qlpSize         * aSegInitialSize,
                                  qlpSize         * aSegNextSize,
                                  qlpSize         * aSegMinSize,
                                  qlpSize         * aSegMaxSize,
                                  smlSegmentAttr  * aSegAttr,
                                  qllEnv          * aEnv );

stlStatus qlrExecuteCreateTableCommon( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );


#endif /* _QLR_CREATE_TABLE_H_ */
