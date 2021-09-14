/*******************************************************************************
 * qlnxNode.h
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

#ifndef _QLNXNODE_H_
#define _QLNXNODE_H_ 1

/**
 * @file qlnxNode.h
 * @brief SQL Processor Layer Execution Node
 */

#include <qlDef.h>

/**
 * @addtogroup qlnx
 * @{
 */

struct qlnxRecordStat
{
    stlInt64        mTotalSize;
    stlInt64        mMaxSize;
    stlInt64        mMinSize;
    stlInt64        mTotalCount;
};

/**
 * @brief COMMON INFO (for Join)
 * 현재 Query Set, Query Specification 이하에서만 사용
 */

struct qlnxCommonInfo
{
    knlValueBlockList   * mResultColBlock;
    qlnxRowBlockList    * mRowBlockList;

    knlValueBlockList   * mOuterColBlock;        /**< Outer Column List */
    knlValueBlockList   * mOuterOrgColBlock;     /**< Original Block for Outer Column List */
    
    stlUInt64             mFetchRowCnt;
    stlTime               mBuildTime;
    stlTime               mFetchTime;

    qlnxRecordStat        mFetchRecordStat;

    stlInt32              mFetchCallCount;
};


/***************************************************
 * STATEMENT Nodes
 ***************************************************/ 

/**
 * @brief SELECT STATEMENT Execution Node
 */
struct qlnxSelectStmt
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */
    
    /* Value Block 관련 */
    knlValueBlockList   * mStmtPseudoColBlock;    /**< Pseudo Column List
                                                   *   : Iteration Time이 Statement 단위인 Pseudo Column */

    /* OFFSET .. LIMIT 절 관련 */
    stlInt64              mRemainSkipCnt;         /**< Remain Skip(Offset) Count */
    stlInt64              mRemainFetchCnt;        /**< Remain Fetch Count */
};


/**
 * @brief INSERT STATEMENT Execution Node
 */
struct qlnxInsertStmt
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */
    
    /* Pseudo Column 관련 */
    knlValueBlockList      * mTotalPseudoColBlock;   /**< 전체 Pseudo Column List */
    knlValueBlockList      * mQueryPseudoColBlock;   /**< Query단위 Pseudo Column List */

    /* RowId Column 관련 */    
    knlValueBlockList      * mRowIdColBlock;         /**< RowId Column Block List */

    /* Insert Values 관련 */
    smlRowBlock              mRowBlock;              /**< Row 관련 정보를 저장할 Block */

    knlValueBlockList      * mReturnOuterColBlock;
    knlValueBlockList      * mReturnOuterOrgColBlock;
    
    /* Insert에 영향을 받는 Object 관련 */
    qlxExecInsertRelObject   mRelObject;             /**< Insert에 영향을 받는 Object List */
};


/**
 * @brief UPDATE STATEMENT Execution Node
 */
struct qlnxUpdateStmt
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */
    
    /* Pseudo Column 관련 */
    knlValueBlockList      * mTotalPseudoColBlock;   /**< 전체 Pseudo Column List */
    knlValueBlockList      * mQueryPseudoColBlock;   /**< Query단위 Pseudo Column List */

    /* RowId Column 관련 */
    knlValueBlockList      * mRowIdColBlock;         /**< Write Table RowId Column Block List */

    /* Update 대상 Table 관련 */
    knlValueBlockList      * mReadColBlock;          /**< Update Table에 대한 Read Column List */
    knlValueBlockList      * mWriteColBlock;         /**< Update Table에 대한 Write Column List */

    /* Update Values 관련 */
    smlRowBlock            * mRowBlock;              /**< Row 관련 정보를 저장할 Block */

    knlValueBlockList      * mSetOuterColBlock;
    knlValueBlockList      * mSetOuterOrgColBlock;

    knlValueBlockList      * mReturnOuterColBlock;
    knlValueBlockList      * mReturnOuterOrgColBlock;

    /* Update에 영향을 받는 Object 관련 */
    qlxExecUpdateRelObject   mRelObject;             /**< Update에 영향을 받는 Object List */

    /* Update시 conflict 처리를 위한 Fetch Record 정보 관련 */
    stlBool                  mVersionConflict;       /**< 구문 수행중 Version Conflict 발생 여부 */
    knlPhysicalFilter      * mPhysicalWhereFilter;   /**< Physical Table Filter */
    smlFetchRecordInfo       mFetchRecordInfo;       /**< Fetch Record 정보 */

    /* Update 성공 여부 */
    stlBool                * mIsUpdated;             /**< 각 row에 대한 update 성공 여부 */
};


/**
 * @brief DELETE STATEMENT Execution Node
 */
struct qlnxDeleteStmt
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */
    
    /* Pseudo Column 관련 */
    knlValueBlockList      * mTotalPseudoColBlock;   /**< 전체 Pseudo Column List */
    knlValueBlockList      * mQueryPseudoColBlock;   /**< Query단위 Pseudo Column List */

    /* Delete 대상 Table 관련 */
    knlValueBlockList      * mReadColBlock;          /**< Delete Table에 대한 Read Column List */

    knlValueBlockList      * mRowIdColBlock;         /**< Delete Table에 대한 RowId Column List */
    

    /* Delete Values 관련 */
    smlRowBlock            * mRowBlock;              /**< Row 관련 정보를 저장할 Block */

    knlValueBlockList      * mReturnOuterColBlock;
    knlValueBlockList      * mReturnOuterOrgColBlock;

    /* Delete에 영향을 받는 Object 관련 */
    qlxExecDeleteRelObject   mRelObject;             /**< Delete에 영향을 받는 Object List */

    /* Delete시 conflict 처리를 위한 Fetch Record 정보 관련 */
    stlBool                  mVersionConflict;       /**< 구문 수행중 Version Conflict 발생 여부 */
    knlPhysicalFilter      * mPhysicalWhereFilter;   /**< Physical Table Filter */
    smlFetchRecordInfo       mFetchRecordInfo;       /**< Fetch Record 정보 */

    /* Delete 성공 여부 */
    stlBool                * mIsDeleted;             /**< 각 row에 대한 delete 성공 여부 */
};


/***************************************************
 * QUERY EXPRESSION Nodes
 ***************************************************/ 

/**
 * @brief QUERY SPECIFICATION Execution Node
 */
struct qlnxQuerySpec
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */

    /* Value Block 관련 */
    knlValueBlockList   * mTargetColBlock;      /**< Target Column 공간 */
    knlValueBlockList   * mPseudoColBlock;      /**< Pseudo Column 공간
                                                 *   : Iteration Time이 FETCH 또는 EXPRESSION 단위인 Pseudo Column */

    /* OFFSET .. LIMIT 절 관련 */
    stlInt64              mRemainSkipCnt;        /**< Remain Skip(Offset) Count */
    stlInt64              mRemainFetchCnt;       /**< Remain Fetch Count */
};


/**
 * @brief SUB-QUERY Execution Node
 */
struct qlnxSubQuery
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */

    stlBool               mHasFalseFilter;      /**< 결과값으로 항상 FALSE 결과를 반환하는지 여부 */

    /* Value Block 관련 */
    knlValueBlockList   * mTargetBlock;         /**< Target Column 공간 */

    knlExprEvalInfo     * mFilterExprEvalInfo;  /**< Where Predicate의 Evaluate Info */
};


/**
 * @brief SUB-QUERY Execution Node
 */
struct qlnxSubQueryList
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */

    /* Value Block 관련 */
    knlValueBlockList  ** mTargetBlockList;     /**< Sub Query 별 Target Column 공간 */
    knlValueBlockList  ** mRefColBlockList;     /**< Sub Query 별 Reference Column 공간 */

    stlInt32              mSubQueryCnt;         /**< Sub Query 개수 */
    stlBool               mIsFirstExec;         /**< First Execution인지 여부 */
};


/**
 * @brief SUB-QUERY FUNCTION Execution Node
 */
struct qlnxSubQueryFunc
{
    qlnxCommonInfo               mCommonInfo;              /**< Common Info (반드시 처음에 위치해야 함) */

    /* Func 관련 */
    knlValueBlockList          * mLeftBlockList;           /**< Left Operand Expression 공간 */
    knlValueBlockList          * mRightBlockList;          /**< Right Operand Expression 공간 */
    knlValueBlockList          * mBlockListForCast;        /**< Cast 수행을 위한 Value Block List 공간 */

    knlCastFuncInfo           ** mRightCastInfo;           /**< Right Operand Type Cast Info */

    knlValueBlockList          * mResultBlock;             /**< Sub Query Function Result Value Block : Reference Column */
    qlnxSubQueryFuncPtr          mSubQueryFuncPtr;         /**< Sub Query Function의 Function Pointer */

    stlInt32                     mValueBlockCnt;           /**< Value Block Count */
    stlBool                      mIsFirstExec;             /**< First Execution인지 여부 */
    stlBool                      mNeedMaterialize;         /**< Materialization 여부 */
    stlBool                      mHasNull;                 /**< Right Operand Expression가 NULL value를 가지는지 여부 */
    stlBool                      mNeedResNullCheck;        /**< List Function 결과가 NULL인지 체크할지 여부 */
    stlBool                      mIsResReverse;            /**< 결과 반전 여부 */
    stlBool                      mIsOrdering;              /**<  */

    /* Row 관련 */
    stlBool                      mIsCheckNext[3];          /**< Compare 결과에 따른 다음 Value Expression Check 여부 */
    stlBool                      mResult[3];               /**< Compare 결과에 따른 반환 값 */
    dtlPhysicalCompareFuncPtr  * mCompFunc;                /**< Compare Function : Array */
    
    /* N/A 형 관련 : Materialize */
    qlnxListFuncInstantFetchInfo    * mListFuncInstantFetchInfo;    /**< Instant를 이용한 Sub Query Function Fetch Info */
    qlnxListFuncArrayFetchInfo      * mListFuncArrayFetchInfo;      /**< Array를 이용한 Sub Query Function Fetch Info */
};


/**
 * @brief SUB-QUERY FILTER Execution Node
 */
struct qlnxSubQueryFilter
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */

    knlExprEvalInfo    ** mFilterExprEvalInfo;  /**< SubQuery Predicate의 Evaluate Info : Array */
};


/**
 * @brief VIEW Execution Node
 */
struct qlnxView
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */

    // Nothing
};

/***************************************************
 * QUERY SET nodes
 ***************************************************/ 

/**
 * @brief QUERY SET execution node
 */
struct qlnxQuerySet
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info */

    /* Value Block 관련 */
    knlValueBlockList   * mTargetColBlock;      /**< Target Column 공간 */

    
    knlValueBlockList   * mSetColumnBlock;      /**< Set Column 공간 */
    knlValueBlockList   * mOriginColumnBlock;   /**< Original Set Column Block  */
    
    /* OFFSET .. LIMIT 절 관련 */
    stlInt64              mRemainSkipCnt;        /**< Remain Skip(Offset) Count */
    stlInt64              mRemainFetchCnt;       /**< Remain Fetch Count */
};


/**
 * @brief Set Operation execution node
 */
struct qlnxSetOP
{
    qlnxCommonInfo        mCommonInfo;              /**< Common Info */

    /* Value Block 관련 */
    knlValueBlockList   * mColumnBlock;             /**< Column Block 공간 */
    
    /* materialized SET */
    qlnInstantTable       mInstantTable;            /**< Instant Table */
    smlRowBlock           mHashRowBlock;            /**< Hash에 Insert시 사용될 RowBlock 정보 */
    smlFetchInfo          mFetchInfo;               /**< Fetch Information */

    /* for INTERSECT */
    qlnInstantTable       mInstantA;                /**< Instant Table A */
    qlnInstantTable       mInstantB;                /**< Instant Table B */
    qlnInstantTable     * mSrcInstant;              /**< Source Instant */
    qlnInstantTable     * mDstInstant;              /**< Destin Instant */
    
    
    /* run-time */
    stlInt32              mFetchChildIdx;           /**< Fetch 중인 Child */
};


/***************************************************
 * ORDER BY, GROUP BY Nodes
 ***************************************************/ 

/**
 * @brief HASH Execution Node
 */
struct qlnxHash
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */

    /* Scan 관련 */
    void                * mHashTable;           /**< Hash Table : 미정 */
    void                * mHashProperty;        /**< Hash Function : 미정 */
    smlRowBlock           mRowBlock;            /**< Row Block */

    /* Value Block List 관련 */
    knlValueBlockList   * mReadColBlock;        /**< 읽을 Column 공간 */
};


/**
 * @brief AGGREGATION Execution Node
 */
struct qlnxAggregation
{
    qlnxCommonInfo          mCommonInfo;         /**< Common Info (반드시 처음에 위치해야 함) */

    /* Scan 관련 */
    stlBool                 mIsNeedBuild;        /**< 첫번째 Execute인지 여부 */
    smlRowBlock             mRowBlock;           /**< Row Block */

    /* Aggregation 관련 */
    qlnfAggrExecInfo      * mAggrExecInfo;       /**< Aggregation 수행 정보 : array */
    qlnfAggrDistExecInfo  * mAggrDistExecInfo;   /**< Aggregation Distinct 수행 정보 : array */
};


/**
 * @brief GROUP Execution Node
 */
struct qlnxGroup
{
    /* Common Info */
    qlnxCommonInfo           mCommonInfo;              /**< Common Info (반드시 처음에 위치해야 함) */

    /* Scan 관련 */
    stlBool                  mHasFalseFilter;          /**< 결과값으로 항상 FALSE 결과를 반환하는지 여부 */
    stlBool                  mHasAggr;                 /**< Aggregation Function 포함 여부 */
    
    stlBool                  mIsFirstExecute;          /**< 첫번째 Execute인지 여부 */
    stlBool                  mIsExistGroup;            /**< Group이 존재하는지 여부 */
    stlBool                  mIsNeedFetch;             /**< child node에 대한 fetch 진행 여부 */
    stlBool                  mEOF;                     /**< Group node에 대한 EOF */
    stlBool                  mIsSwitched;              /**< Column 값 설정을 위한 임시 Column의 Value Block 과의 switch 진행 여부 */
    stlInt64                 mReadBlockCnt;            /**< child node에서 fetch한 record 개수 */
    stlUInt16                mCurrBlockIdx;            /**< child node에서 fetch한 block 내 record를 읽을 위치 */

    /* Group 관련 */
    stlInt32                 mGroupColCount;           /**< Group Column Count */
    dtlDataValue           * mGroupColValue;           /**< Group 대표값들 : array */

    /* Column 관련 */
    knlValueBlockList      * mKeyColBlock;             /**< Key Column Block List */
    knlValueBlockList      * mRecColBlock;             /**< Record Column Block List */
    knlValueBlockList      * mReadColBlock;            /**< Read Column Block List */
    knlValueBlockList      * mTotalColBlock;           /**< Total Column Block List */
    knlValueBlockList      * mSwitchBlock;             /**< Switch Total Column Block List */
    knlValueBlockList      * mSwitchRecBlock;          /**< Switch Record Column Block List */

    /* Aggregation 관련 */
    qlnfAggrExecInfo       * mAggrExecInfo;            /**< Aggregation 수행 정보 */
    qlnfAggrDistExecInfo   * mAggrDistExecInfo;        /**< Aggregation Distinct 수행 정보 */

    /* HAVING 관련 */
    knlExprEvalInfo        * mFilterExprEvalInfo;      /**< Filter Predicate의 Evaluate Info */

    /* Nested Aggregation을 위한 Instant 관련 */
    smlRowBlock              mRowBlock;                /**< Hash Instant Table을 위한 Row Block */
};


/***************************************************
 * SCAN Nodes
 ***************************************************/ 

/**
 * @brief TABLE ACCESS Execution Node
 */
struct qlnxTableAccess
{
    /* Common Info */
    qlnxCommonInfo          mCommonInfo;           /**< Common Info (반드시 처음에 위치해야 함) */

    /* Scan 관련 */
    stlBool                 mIsFirstExecute;       /**< 첫번째 Execute인지 여부 */
    stlBool                 mParentIsQuerySpec;    /**< Parent Node가 Query Spec Node인지 여부 :
                                                   * Record가 존재하지 않는 경우 Aggregation 결과 설정 정보 */
    stlBool                 mHasFalseFilter;       /**< 결과값으로 항상 FALSE 결과를 반환하는지 여부 */
    void                  * mIterator;             /**< SM Iterator */
    smlIteratorProperty     mIteratorProperty;     /**< Iterator Property */
    smlFetchInfo            mFetchInfo;            /**< Fetch Info : Fetch Aggregation 정보 포함 */

    void                  * mPhysicalHandle;       /**< Table Physical Handle */
    
    /* Value Block List 관련 */ 
    knlValueBlockList     * mReadColBlock;         /**< Read Table Column Block List */

    /* RowId Value Block List */
    knlValueBlockList     * mRowIdColBlock;        /**< RowId Column Block List */

    /* Read Mode 관련 */
    smlTransReadMode        mTransReadMode;        /**< Transaction Read Mode */
    smlStmtReadMode         mStmtReadMode;         /**< Statement Read Mode */

    /* Aggregation 관련 */
    qlnfAggrExecInfo      * mAggrExecInfo;         /**< Aggregation 수행 정보 */
    qlnfAggrDistExecInfo  * mAggrDistExecInfo;     /**< Aggregation Distinct 수행 정보 */

    /* Prepare Filter 관련 */
    knlExprEvalInfo       * mPreFilterEvalInfo;    /**< Prepare Filter의 Evaluate Info */
};


/**
 * @brief INDEX ACCESS Execution Node
 */
struct qlnxIndexAccess
{
    /* Common Info */
    qlnxCommonInfo          mCommonInfo;           /**< Common Info (반드시 처음에 위치해야 함) */

    /* Scan 관련 */
    stlBool                 mIsFirstExecute;       /**< 첫번째 Execute인지 여부 */
    stlBool                 mParentIsQuerySpec;    /**< Parent Node가 Query Spec Node인지 여부 :
                                                    * Record가 존재하지 않는 경우 Aggregation 결과 설정 정보 */
    stlBool                 mHasFalseFilter;       /**< 결과값으로 항상 FALSE 결과를 반환하는지 여부 */
    void                  * mIterator;             /**< SM Iterator */
    smlIteratorProperty     mIteratorProperty;     /**< Iterator Property */
    smlFetchInfo            mFetchInfo;            /**< Fetch Info : Fetch Aggregation 정보 포함 */

    /* Value Block List 관련 */ 
    knlValueBlockList     * mIndexReadColBlock;    /**< Read Index Column Block List */
    knlValueBlockList     * mTableReadColBlock;    /**< Read Table Column Block List */
    knlValueBlockList     * mTotalReadColBlock;    /**< Total Read Table Column Block List */

    /* RowId Value Block List */
    knlValueBlockList     * mRowIdColBlock;        /**< RowId Column Block List */    

    /* Range 관련 */ 
    knlCompareList        * mMinRangeCompList;     /**< Min Range Compare Array */
    knlCompareList        * mMaxRangeCompList;     /**< Max Range Compare Array */
    stlInt32                mMinRangeTotalCnt;     /**< Total Min Range Count */
    stlInt32                mMaxRangeTotalCnt;     /**< Total Max Range Count */

    /* Read Mode 관련 */
    smlTransReadMode        mTransReadMode;        /**< Transaction Read Mode */
    smlStmtReadMode         mStmtReadMode;         /**< Statement Read Mode */

    /* Aggregation 관련 */
    qlnfAggrExecInfo      * mAggrExecInfo;         /**< Aggregation 수행 정보 */
    qlnfAggrDistExecInfo  * mAggrDistExecInfo;     /**< Aggregation Distinct 수행 정보 */

    /* Prepare Range 관련 */
    knlExprEvalInfo       * mPreRangeEvalInfo;     /**< Prepare Range의 Evaluate Info */
    knlExprEvalInfo       * mPreInKeyRangeEvalInfo;/**< Prepare IN Key Range의 Evaluate Info */

    knlExprEvalInfo       * mPreKeyFilterEvalInfo; /**< Prepare Key Filter의 Evaluate Info */
    knlExprEvalInfo       * mPreFilterEvalInfo;    /**< Prepare Filter의 Evaluate Info */

    /* IN Key Range 관련 */
    qlnxInKeyRangeFetchInfo  * mInKeyRange;        /**< IN Key Range Fetch Info */
};

/**
 * @brief ROWID ACCESS Execution Node
 */
struct qlnxRowIdAccess
{
    /* Common Info */
    qlnxCommonInfo         mCommonInfo;           /**< Common Info (반드시 처음에 위치해야 함) */

    /* Scan 관련 */
    stlBool                mIsFirstExecute;       /**< 첫번째 Execute인지 여부 */
    stlBool                mHasFalseFilter;       /**< 결과값으로 항상 FALSE 결과를 반환하는지 여부 */
    smlFetchInfo           mFetchInfo;            /**< Fetch Info */
    
    /* Value Block List 관련 */ 
    knlValueBlockList    * mReadColBlock;         /**< Read Column Block List */

    /* RowId Value Block List */
    knlValueBlockList    * mRowIdColBlock;        /**< RowId Column Block List */

    /* RowIdScan 관련
     * 예) ROWID = 'AAAA24AAAAAAACAAAAAiAAC' 에서
     * 'AAAA24AAAAAAACAAAAAiAAC' 를 보고
     * sm에서 레코드를 가져오기 위해 TablePhysicalID , smlRid 정보를 구한다.
     */
    qlvInitExpression    * mRowIdScanValueExpr;   /**< 검색할 RowId Value 정보 */
    
    smlRid               * mRidForRowIdScan;      /**< 검색할 RowId Value의 smlRid 정보 */
    stlInt64               mTableLogicalIDForRowIdScan;  /**< 검색할 RowId Value의 TableLogicalID */

    /* Read Mode 관련 */
    smlTransReadMode       mTransReadMode;        /**< Transaction Read Mode */
    smlStmtReadMode        mStmtReadMode;         /**< Statement Read Mode */

    /* Aggregation 관련 */
    qlnfAggrExecInfo     * mAggrExecInfo;         /**< Aggregation 수행 정보 */

    /* Prepare Filter 관련 */
    knlExprEvalInfo      * mPreFilterEvalInfo;    /**< Prepare Filter의 Evaluate Info */
};


/**
 * @brief FAST DUAL Execution Node
 */
struct qlnxFastDualAccess
{
    /* Common Info */
    qlnxCommonInfo         mCommonInfo;           /**< Common Info (반드시 처음에 위치해야 함) */

    // Nothing
};


/**
 * Row Block Item
 */

struct qlnxRowBlockItem
{
    qllOptimizationNode * mLeafOptNode;             /**< Base Table의 Optimization Node */
    smlRowBlock         * mRowBlock;                /**< Row Block */
    smlRowBlock         * mOrgRowBlock;             /**< Base Table의 Row Block */
    qlnxRowBlockItem    * mNext;
    stlBool               mIsLeftTableRowBlock;     /**< Left Table의 Row Block인지 여부 */
    stlChar               mPadding[7];              /**< for 8 bytes align */
};


/**
 * Row Block List
 */

struct qlnxRowBlockList
{
    qlnxRowBlockItem    * mHead;
    qlnxRowBlockItem    * mTail;
    stlInt32              mCount;
    stlChar               mPadding[4];      /**< for 8 bytes align */
};


/**
 * Cast Info
 */
struct qlnxCastInfo
{
    dtlBuiltInFuncPtr    mFuncPtr;
    dtlValueEntry        mInputArgument[ DTL_CAST_INPUT_ARG_CNT ];
    knlValueBlockList  * mInValueBlock;
    knlValueBlockList  * mOutValueBlock;
};


/**
 * @brief Instant Execution Node
 */
struct qlnxInstant
{
    /* Common Info */
    qlnxCommonInfo             mCommonInfo;         /**< Common Info (반드시 처음에 위치해야 함) */
    stlBool                    mHasAggr;            /**< Aggregation Function 포함 여부 */
    stlBool                    mHasNestedAggr;      /**< Nested Aggregation Function 포함 여부 */
    stlBool                    mHasFalseFilter;     /**< 항상 False 값을 가지는 검색 조건을 가지고 있는지 여부 */
    stlBool                    mNeedMaterialize;    /**< Materialization 여부 */
    stlBool                    mNeedRebuild;        /**< Rebuild 필요 여부 */
    stlBool                    mIsFirstExecute;     /**< 첫번째 Execute인지 여부 */


    /* Table Scan 관련 */
    qlnInstantTable            mInstantTable;       /**< Instant Table Info */
    smlFetchInfo               mFetchInfo;          /**< Fetch Info : Aggr Fetch 정보도 포함 */
    smlMergeRecordInfo       * mMergeRecord;        /**< Merge Record Info */

    knlValueBlockList        * mLeftConstBlockList;
    knlDataValueList         * mLeftConstValueList;
    
    /* Column 관련 */
    knlValueBlockList        * mKeyColBlock;        /**< Key Column Block List */
    knlValueBlockList        * mRecColBlock;        /**< Record Column Block List */
    knlValueBlockList        * mReadColBlock;       /**< Read Column Block List */
    knlValueBlockList        * mInsertColBlock;     /**< Insert Column Block List */

    knlValueBlockList        * mIndexReadColBlock;  /**< Index Read Column Block List */
    knlValueBlockList        * mTableReadColBlock;  /**< Table Read Column Block List */
    
    /* Aggregation 관련 */
    qlnfAggrExecInfo         * mAggrExecInfo;       /**< Aggregation 수행 정보 */
    qlnfAggrDistExecInfo     * mAggrDistExecInfo;   /**< Aggregation Distinct 수행 정보 */

    /* Nested Aggregation 관련 */
    qlnfAggrExecInfo         * mNestedAggrExecInfo;     /**< Nested Aggregation 수행 정보 */
    qlnfAggrDistExecInfo     * mNestedAggrDistExecInfo; /**< Nested Aggregation Distinct 수행 정보 */

    /* Conflict Check Info 관련 */
    qlnxConflictCheckInfo    * mConflictCheckInfo;      /**< Grouping Aggregation Distinct를 위한 Conflict Check 정보 */

    /* Prepare Range & Hash Key 관련 */
    knlExprEvalInfo          * mPreRangeEvalInfo;       /**< Prepare Range/Hash-Key의 Evaluate Info */

    /* Prepare Filter 관련 */
    knlExprEvalInfo          * mPreKeyFilterEvalInfo;   /**< Prepare Key Filter의 Evaluate Info */
    knlExprEvalInfo          * mPreFilterEvalInfo;      /**< Prepare Filter의 Evaluate Info */
};


/**
 * @brief aggregation에 대한 Execution 정보 관리하는 구조체
 */
struct qlnxConflictCheckInfo
{
    /* Input Argument 관련 */
    stlInt32                 mInsertColCnt;       /**< insert column 개수 */
    stlInt32                 mConflictColCnt;     /**< conflict column 개수 */

    /* Value Block 관련 */
    knlValueBlockList      * mInsertColList;      /**< insert column value block list */
    knlValueBlockList      * mConflictColList;    /**< conflict column value block list */
    knlValueBlockList      * mConflictResult;     /**< conflict result value block list */

    /* instant table 관련 */
    qlnInstantTable          mInstantTable;       /**< Instant Table Info */
};


/**
 * @brief Sub Query Func에 대한 Instant를 이용한 Fetch 정보 관리하는 구조체
 */
struct qlnxListFuncInstantFetchInfo
{
    /* Fetch Info : Data Opt 시에 설정 */
    smlFetchInfo               * mFetchInfo;        /**< Fetch Info : Range & Read Column List */
    stlUInt8                   * mSortKeyFlags;     /**< Sort Instant Table의 Key Flags */
    smlRowBlock                  mSortRowBlock;     /**< Sort Instant Table을 위한 Row Block */

    knlCompareList             * mMinRange;         /**< Min Range Conditions : Array */
    knlCompareList             * mMaxRange;         /**< Max Range Conditions : Array */

    /* Instant Table 관련 : Execute 시에 설정 */
    qlnInstantTable            * mInstantTable;     /**< Right Operand Materialize를 위한 Sort Instant Table */
};


/**
 * @brief Sub Query Func에 대한 Array를 이용한 Fetch 정보 관리하는 구조체
 */
struct qlnxListFuncArrayFetchInfo
{
    /* Fetch Info : Data Opt 시에 설정 */
    stlBool                       mIsNeedBuild;             /**< Build 여부 */
    stlInt32                      mRowColRecordCnt;         /**< Row Column Block List 을 구성하는 Record 개수 */
    knlValueBlockList           * mRowColBlock;             /**< Row Column Block List Array */
    stlBool                     * mIsUsedRowCol;            /**< Row Column Block의 n번째 data value 구성 여부 */

    /* Build 관련 */
    stlBool                      mIsCheckNextForBuild[3];   /**< Compare 결과에 따른 다음 Value Expression Check 여부 */
    stlBool                      mResultForBuild[3];        /**< Compare 결과에 따른 반환 값 */
    dtlPhysicalCompareFuncPtr  * mCompFuncForBuild;         /**< Build시 사용할 Compare Function : Array */
};


/**
 * @brief IN Key Range에 대한 Fetch 정보 관리하는 구조체
 */
struct qlnxInKeyRangeFetchInfo
{
    stlInt32                mRecCnt;           /**< Record Count */
    stlBool                 mNeedFetchKeyRec;  /**< Key Range Record 필요한지 여부 */
    knlValueBlockList     * mValueBlock;       /**< Constant Value Block List */
    knlValueBlockList     * mListColBlock;     /**< List Column Block List */
    knlValueBlockList    ** mInsertColBlock;   /**< Column Block List For IN Key Range */
    qlnInstantTable         mInstantTable;     /**< Sort Instant Table */
    smlFetchInfo            mFetchInfo;        /**< Fetch Info : Read Column List */
    stlUInt8              * mSortKeyFlags;     /**< Sort Instant Table의 Key Flags */
    smlRowBlock             mRowBlock;         /**< Sort Instant Table을 위한 Row Block */
};


/***************************************************
 * JOIN Nodes
 ***************************************************/ 

/**
 * Cache Block Info
 */
struct qlnxCacheBlockInfo
{
    stlInt64    mCacheBlockCnt;     /**< 이미 읽어 Value Block에 존재하는 Block Count */
    stlInt64    mCurrBlockIdx;      /**< Block Cache에서 이미 읽은 개수 */
    stlInt16    mNodeIdx;           /**< Child Node Index */
    stlBool     mEOF;               /**< Child Node의 EOF 여부 */
    stlBool     mNeedScan;          /**< Child Node의 Scan을 해야 할지 여부 */
    stlChar     mPadding[3];        /**< for 8 bytes align */
};


#define QLNX_RESET_CACHE_BLOCK_INFO( aCacheBlockInfo )  \
{                                                       \
    (aCacheBlockInfo)->mCacheBlockCnt = 0;              \
    (aCacheBlockInfo)->mCurrBlockIdx = -1;              \
    (aCacheBlockInfo)->mEOF = STL_FALSE;                \
    (aCacheBlockInfo)->mNeedScan = STL_TRUE;            \
}

#define QLNX_SET_END_OF_TABLE_FETCH( aCacheBlockInfo )  \
{                                                       \
    (aCacheBlockInfo)->mCacheBlockCnt = 0;              \
    (aCacheBlockInfo)->mCurrBlockIdx = -1;              \
    (aCacheBlockInfo)->mEOF = STL_TRUE;                 \
    (aCacheBlockInfo)->mNeedScan = STL_TRUE;            \
}

#define QLNX_IS_END_OF_TABLE_FETCH( aCacheBlockInfo )   \
    ( ((aCacheBlockInfo)->mEOF == STL_TRUE) &&          \
      ((aCacheBlockInfo)->mCurrBlockIdx + 1 >= (aCacheBlockInfo)->mCacheBlockCnt) )

#define QLNX_NEED_NEXT_FETCH( aCacheBlockInfo ) \
    ( (aCacheBlockInfo)->mCurrBlockIdx + 1 >= (aCacheBlockInfo)->mCacheBlockCnt )


/**
 * Value Block Item
 */
struct qlnxValueBlockItem
{
    knlCastFuncInfo     * mCastFuncInfo;

    knlValueBlock       * mSrcValueBlock;
    knlValueBlock       * mDstValueBlock;
    stlInt64            * mSrcBlockIdx;
    stlInt64            * mDstBlockIdx;
    stlInt64              mDataValueSize;
    qlnxValueBlockItem  * mNext;
};


/**
 * Value Block List
 */
struct qlnxValueBlockList
{
    qlnxValueBlockItem  * mHead;
    qlnxValueBlockItem  * mTail;
    stlInt32              mCount;
    stlChar               mPadding[4];      /**< for 8 bytes align */
};

#define QLNX_SET_VALUE_BLOCK( _aValueBlockList, _aEnv )                             \
{                                                                                   \
    qlnxValueBlockItem  * _sValueBlockItem  = NULL;                                 \
                                                                                    \
    _sValueBlockItem = (_aValueBlockList)->mHead;                                   \
    while( _sValueBlockItem != NULL )                                               \
    {                                                                               \
        if( (_sValueBlockItem->mCastFuncInfo != NULL) &&                            \
            !DTL_IS_NULL( KNL_GET_VALUE_BLOCK_DATA_VALUE(                           \
                              _sValueBlockItem->mSrcValueBlock,                     \
                              *(_sValueBlockItem->mSrcBlockIdx) ) ) )               \
        {                                                                           \
            _sValueBlockItem->mCastFuncInfo->mArgs[0].mValue.mDataValue =           \
                KNL_GET_VALUE_BLOCK_DATA_VALUE(                                     \
                    _sValueBlockItem->mSrcValueBlock,                               \
                    *(_sValueBlockItem->mSrcBlockIdx) );                            \
            STL_TRY( _sValueBlockItem->mCastFuncInfo->mCastFuncPtr(                 \
                         DTL_CAST_INPUT_ARG_CNT,                                    \
                         _sValueBlockItem->mCastFuncInfo->mArgs,                    \
                         (void *)KNL_GET_VALUE_BLOCK_DATA_VALUE(                    \
                                     _sValueBlockItem->mDstValueBlock,              \
                                     *(_sValueBlockItem->mDstBlockIdx) ),           \
                         (void *)(_sValueBlockItem->mCastFuncInfo->mSrcTypeInfo),   \
                         KNL_DT_VECTOR( _aEnv ),                                    \
                         (_aEnv),                                                   \
                         KNL_ERROR_STACK( _aEnv ) )                                 \
                     == STL_SUCCESS );                                              \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            STL_TRY( knlCopyDataValue(                                              \
                         KNL_GET_VALUE_BLOCK_DATA_VALUE(                            \
                             _sValueBlockItem->mSrcValueBlock,                      \
                             *(_sValueBlockItem->mSrcBlockIdx) ),                   \
                         KNL_GET_VALUE_BLOCK_DATA_VALUE(                            \
                             _sValueBlockItem->mDstValueBlock,                      \
                             *(_sValueBlockItem->mDstBlockIdx) ),                   \
                         KNL_ENV(_aEnv) )                                           \
                     == STL_SUCCESS );                                              \
        }                                                                           \
                                                                                    \
        _sValueBlockItem = _sValueBlockItem->mNext;                                 \
    }                                                                               \
}

#define QLNX_SET_OUTER_VALUE_BLOCK( _aValueBlockList, _aOuterBlockIdxPtr, _aEnv )       \
{                                                                                       \
    qlnxValueBlockItem  * _sValueBlockItem  = NULL;                                     \
                                                                                        \
    _sValueBlockItem = (_aValueBlockList)->mHead;                                       \
    while( _sValueBlockItem != NULL )                                                   \
    {                                                                                   \
        if( _sValueBlockItem->mSrcBlockIdx == (_aOuterBlockIdxPtr) )                    \
        {                                                                               \
            if( (_sValueBlockItem->mCastFuncInfo != NULL) &&                            \
                !DTL_IS_NULL( KNL_GET_VALUE_BLOCK_DATA_VALUE(                           \
                                  _sValueBlockItem->mSrcValueBlock,                     \
                                  *(_sValueBlockItem->mSrcBlockIdx) ) ) )               \
            {                                                                           \
                _sValueBlockItem->mCastFuncInfo->mArgs[0].mValue.mDataValue =           \
                KNL_GET_VALUE_BLOCK_DATA_VALUE(                                         \
                    _sValueBlockItem->mSrcValueBlock,                                   \
                    *(_sValueBlockItem->mSrcBlockIdx) );                                \
                STL_TRY( _sValueBlockItem->mCastFuncInfo->mCastFuncPtr(                 \
                             DTL_CAST_INPUT_ARG_CNT,                                    \
                             _sValueBlockItem->mCastFuncInfo->mArgs,                    \
                             (void *)KNL_GET_VALUE_BLOCK_DATA_VALUE(                    \
                                         _sValueBlockItem->mDstValueBlock,              \
                                         *(_sValueBlockItem->mDstBlockIdx) ),           \
                             (void *)(_sValueBlockItem->mCastFuncInfo->mSrcTypeInfo),   \
                             KNL_DT_VECTOR( _aEnv ),                                    \
                             (_aEnv),                                                   \
                             KNL_ERROR_STACK( _aEnv ) )                                 \
                         == STL_SUCCESS );                                              \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                STL_TRY( knlCopyDataValue(                                              \
                             KNL_GET_VALUE_BLOCK_DATA_VALUE(                            \
                                 _sValueBlockItem->mSrcValueBlock,                      \
                                 *(_sValueBlockItem->mSrcBlockIdx) ),                   \
                             KNL_GET_VALUE_BLOCK_DATA_VALUE(                            \
                                 _sValueBlockItem->mDstValueBlock,                      \
                                 *(_sValueBlockItem->mDstBlockIdx) ),                   \
                             KNL_ENV(_aEnv) )                                           \
                         == STL_SUCCESS );                                              \
            }                                                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            DTL_SET_NULL(                                                               \
                KNL_GET_VALUE_BLOCK_DATA_VALUE(                                         \
                    _sValueBlockItem->mDstValueBlock,                                   \
                    *(_sValueBlockItem->mDstBlockIdx) ) );                              \
        }                                                                               \
                                                                                        \
        _sValueBlockItem = _sValueBlockItem->mNext;                                     \
    }                                                                                   \
}


#define QLNX_SET_ROW_BLOCK( _aRowBlockList, _aCurrBlockIdx, _aOuterBlockIdx, _aInnerBlockIdx )  \
{                                                                                               \
    qlnxRowBlockItem    * _sRowBlockItem    = NULL;                                             \
    stlInt64              _sTempBlockIdx    = 0;                                                \
                                                                                                \
    _sRowBlockItem = (_aRowBlockList)->mHead;                                                   \
    while( _sRowBlockItem != NULL )                                                             \
    {                                                                                           \
        /* 원본 블럭의 idx 가져오기 */                                                          \
        _sTempBlockIdx = ( _sRowBlockItem->mIsLeftTableRowBlock == STL_TRUE                     \
                           ? (_aOuterBlockIdx) : (_aInnerBlockIdx) );                           \
                                                                                                \
        /* Scn Value 복사 */                                                                    \
        SML_SET_SCN_VALUE(                                                                      \
            _sRowBlockItem->mRowBlock,                                                          \
            (_aCurrBlockIdx),                                                                   \
            SML_GET_SCN_VALUE(                                                                  \
                _sRowBlockItem->mOrgRowBlock,                                                   \
                _sTempBlockIdx ) );                                                             \
                                                                                                \
        /* Rid Value 복사 */                                                                    \
        SML_SET_RID_VALUE(                                                                      \
            _sRowBlockItem->mRowBlock,                                                          \
            (_aCurrBlockIdx),                                                                   \
            SML_GET_RID_VALUE(                                                                  \
                _sRowBlockItem->mOrgRowBlock,                                                   \
                _sTempBlockIdx ) );                                                             \
                                                                                                \
        _sRowBlockItem = _sRowBlockItem->mNext;                                                 \
    }                                                                                           \
}

#define QLNX_SET_USED_BLOCK_SIZE_ON_ROW_BLOCK( _aRowBlockList, _aUsedBlockSize )    \
{                                                                                   \
    qlnxRowBlockItem    * _sRowBlockItem    = NULL;                                 \
                                                                                    \
    _sRowBlockItem = (_aRowBlockList)->mHead;                                       \
    while( _sRowBlockItem != NULL )                                                 \
    {                                                                               \
        SML_SET_USED_BLOCK_SIZE(                                                    \
            _sRowBlockItem->mRowBlock,                                              \
            (_aUsedBlockSize) );                                                    \
                                                                                    \
        _sRowBlockItem = _sRowBlockItem->mNext;                                     \
    }                                                                               \
}

#define QLNX_SET_OUTER_COLUMN_DATA_VALUE_INFO( _aOuterColumnBlockList ) \
{                                                                       \
    qlnxOuterColumnBlockItem    * _sOuterColumnBlockItem    = NULL;     \
                                                                        \
    STL_DASSERT( (_aOuterColumnBlockList) != NULL );                    \
    _sOuterColumnBlockItem = (_aOuterColumnBlockList)->mHead;           \
    while( _sOuterColumnBlockItem != NULL )                             \
    {                                                                   \
        if( _sOuterColumnBlockItem->mCast != NULL )                     \
        {                                                               \
            *(_sOuterColumnBlockItem->mDstDataValue) =                  \
            _sOuterColumnBlockItem->mCast->mCastResultBuf;              \
        }                                                               \
                                                                        \
        _sOuterColumnBlockItem = _sOuterColumnBlockItem->mNext;         \
    }                                                                   \
}

#define QLNX_SET_OUTER_COLUMN_BLOCK( _aOuterColumnBlockItem, _aEnv )                            \
    {                                                                                           \
        dtlDataValue    * _sTmpDataValue    = NULL;                                             \
                                                                                                \
        if( (_aOuterColumnBlockItem)->mCast == NULL )                                           \
        {                                                                                       \
            _sTmpDataValue =                                                                    \
                KNL_GET_VALUE_BLOCK_DATA_VALUE(                                                 \
                    (_aOuterColumnBlockItem)->mSrcValueBlock,                                   \
                    *((_aOuterColumnBlockItem)->mSrcBlockIdx) );                                \
                                                                                                \
            (_aOuterColumnBlockItem)->mDstDataValue->mValue = _sTmpDataValue->mValue;           \
            (_aOuterColumnBlockItem)->mDstDataValue->mLength = _sTmpDataValue->mLength;         \
            (_aOuterColumnBlockItem)->mDstDataValue->mBufferSize = _sTmpDataValue->mBufferSize; \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            (_aOuterColumnBlockItem)->mCast->mArgs[0].mValue.mDataValue =                       \
                KNL_GET_VALUE_BLOCK_DATA_VALUE(                                                 \
                    (_aOuterColumnBlockItem)->mSrcValueBlock,                                   \
                    *((_aOuterColumnBlockItem)->mSrcBlockIdx) );                                \
                                                                                                \
            (_aOuterColumnBlockItem)->mDstDataValue->mBufferSize =                              \
                (_aOuterColumnBlockItem)->mCast->mCastResultBuf.mBufferSize;                    \
                                                                                                \
            if( (_aOuterColumnBlockItem)->mCast->mArgs[0].mValue.mDataValue->mLength == 0 )     \
            {                                                                                   \
                (_aOuterColumnBlockItem)->mDstDataValue->mType =                                \
                    (_aOuterColumnBlockItem)->mCast->mCastResultBuf.mType;                      \
                (_aOuterColumnBlockItem)->mDstDataValue->mValue =                               \
                    (_aOuterColumnBlockItem)->mCast->mCastResultBuf.mValue;                     \
                (_aOuterColumnBlockItem)->mDstDataValue->mLength = 0;                           \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                STL_TRY( (_aOuterColumnBlockItem)->mCast->mCastFuncPtr(                         \
                             DTL_CAST_INPUT_ARG_CNT,                                            \
                             (_aOuterColumnBlockItem)->mCast->mArgs,                            \
                             (void *) & (_aOuterColumnBlockItem)->mCast->mCastResultBuf,        \
                             (void *)((_aOuterColumnBlockItem)->mCast->mSrcTypeInfo),           \
                             KNL_DT_VECTOR( (_aEnv) ),                                          \
                             (_aEnv),                                                           \
                             KNL_ERROR_STACK( (_aEnv) ) )                                       \
                         == STL_SUCCESS );                                                      \
                                                                                                \
                (_aOuterColumnBlockItem)->mDstDataValue->mType =                                \
                    (_aOuterColumnBlockItem)->mCast->mCastResultBuf.mType;                      \
                (_aOuterColumnBlockItem)->mDstDataValue->mValue =                               \
                    (_aOuterColumnBlockItem)->mCast->mCastResultBuf.mValue;                     \
                (_aOuterColumnBlockItem)->mDstDataValue->mLength =                              \
                    (_aOuterColumnBlockItem)->mCast->mCastResultBuf.mLength;                    \
            }                                                                                   \
        }                                                                                       \
    }

#define QLNX_SET_OUTER_COLUMN_BLOCK_LIST( _aOuterColumnBlockItem, _aEnv )               \
{                                                                                       \
    while( (_aOuterColumnBlockItem) != NULL )                                           \
    {                                                                                   \
        QLNX_SET_OUTER_COLUMN_BLOCK( (_aOuterColumnBlockItem), (_aEnv) );               \
                                                                                        \
        (_aOuterColumnBlockItem) = (_aOuterColumnBlockItem)->mNext;                     \
    }                                                                                   \
}

#define QLNX_BACKUP_OUTER_COLUMN_DATA_VALUE_INFO( _aOuterColumnBlockItem )              \
{                                                                                       \
    (_aOuterColumnBlockItem)->mDstDataValueOrgInfo.mType =                              \
        (_aOuterColumnBlockItem)->mDstDataValue->mType;                                 \
    (_aOuterColumnBlockItem)->mDstDataValueOrgInfo.mBufferSize =                        \
        (_aOuterColumnBlockItem)->mDstDataValue->mBufferSize;                           \
    (_aOuterColumnBlockItem)->mDstDataValueOrgInfo.mLength =                            \
        (_aOuterColumnBlockItem)->mDstDataValue->mLength;                               \
    (_aOuterColumnBlockItem)->mDstDataValueOrgInfo.mValue =                             \
        (_aOuterColumnBlockItem)->mDstDataValue->mValue;                                \
}

#define QLNX_REVERT_DATA_VALUE_INFO_OF_OUTER_COLUMN_LIST( _aOuterColumnBlockItem )      \
{                                                                                       \
    while( (_aOuterColumnBlockItem) != NULL )                                           \
    {                                                                                   \
        (_aOuterColumnBlockItem)->mDstDataValue->mType =                                \
            (_aOuterColumnBlockItem)->mDstDataValueOrgInfo.mType;                       \
        (_aOuterColumnBlockItem)->mDstDataValue->mBufferSize =                          \
            (_aOuterColumnBlockItem)->mDstDataValueOrgInfo.mBufferSize;                 \
        (_aOuterColumnBlockItem)->mDstDataValue->mLength =                              \
            (_aOuterColumnBlockItem)->mDstDataValueOrgInfo.mLength;                     \
        (_aOuterColumnBlockItem)->mDstDataValue->mValue =                               \
            (_aOuterColumnBlockItem)->mDstDataValueOrgInfo.mValue;                      \
                                                                                        \
        (_aOuterColumnBlockItem) = (_aOuterColumnBlockItem)->mNext;                     \
    }                                                                                   \
}

#define QLNX_SWAP_DATA_VALUE_ARRAY( _aValueBlockList1,          \
                                    _aValueBlockList2,          \
                                    _aDataValueArr )            \
    {                                                           \
        (_aDataValueArr) =                                      \
            (_aValueBlockList1)->mValueBlock->mDataValue;       \
        (_aValueBlockList1)->mValueBlock->mDataValue =          \
            (_aValueBlockList2)->mValueBlock->mDataValue;       \
        (_aValueBlockList2)->mValueBlock->mDataValue =          \
            (_aDataValueArr);                                   \
    }


/**
 * Outer Column Value Block Item
 */
struct qlnxOuterColumnBlockItem
{
    knlValueBlock               * mSrcValueBlock;
    dtlDataValue                * mDstDataValue;
    stlInt64                    * mSrcBlockIdx;
    knlCastFuncInfo             * mCast;
    dtlDataValue                  mDstDataValueOrgInfo;
    qlnxOuterColumnBlockItem    * mNext;
};


/**
 * Outer Column Value Block List
 */
struct qlnxOuterColumnBlockList
{
    qlnxOuterColumnBlockItem    * mHead;
    qlnxOuterColumnBlockItem    * mTail;
    stlInt32                      mCount;
    stlChar                       mPadding[4];      /**< for 8 bytes align */
};


/**
 * @brief NESTED LOOPS JOIN Execution Node
 */
struct qlnxNestedLoopsJoin
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */

    /* Cache Block Info 관련 */
    qlnxCacheBlockInfo    mInnerCacheBlockInfo;     /**< Inner Table의 Cache Block Info */
    qlnxCacheBlockInfo    mOuterCacheBlockInfo;     /**< Outer Table의 Cache Block Info */
    stlInt64              mCurrBlockIdx;            /**< Join Result의 결과가 쓰여질 Block Idx */

    /* Row Block List 관련 */
    qlnxRowBlockList      mRowBlockList;            /**< Row Block List */

    /* Value Block List 관련 */
    knlValueBlockList   * mJoinedColBlock;          /**< JOIN 결과를 저장할 Column 공간 */
    qlnxValueBlockList    mValueBlockList;          /**< Value Block의 연결 정보 관련 */

    /* Outer Column 관련 */
    qlnxOuterColumnBlockList  mLeftOuterColumnBlockList;    /**< Left Outer Column Value Block의 연결 정보 */
    qlnxOuterColumnBlockList  mRightOuterColumnBlockList;   /**< Right Outer Column Value Block의 연결 정보 */

    qlnxOuterColumnBlockList  mWhereOuterColumnBlockList;   /**< Where Filter Outer Column Value Block의 연결 정보 */

    /* Evaluate Info 관련 */
    knlExprEvalInfo       mWhereExprEvalInfo;       /**< Where Predicate의 Evaluate Info */
    knlExprEvalInfo       mJoinExprEvalInfo;        /**< Join Predicate의 Evaluate Info */
    knlExprEvalInfo    ** mSubQueryFilterExprEvalInfo;  /**< SubQuery Predicate의 Evaluate Info : Array */
    stlBool               mHasJoinCondition;        /**< Join Condition 존재 여부 */
    stlBool               mHasWhereCondition;       /**< Where Condition 존재 여부 */
    stlBool               mIsFirstFetch;            /**< 처음 Fetch를 하는 것인지 여부 */
    stlBool               mHasFalseFilter;          /**< 결과값으로 항상 FALSE 결과를 반환하는지 여부 */
    stlBool               mIsEOF;                   /**< 더이상 Fetch할 것이 없는지 여부 */
    stlBool               mIsSwitched;              /**< Inner와 Outer의 Switching 여부 */
    stlChar               mPadding[2];              /**< for 8 bytes align */

    qlvJoinType           mJoinType;                /**< Join Type */
};


/**
 * @brief Sort Key Compare Item
 */
struct qlnxSortKeyCompareItem
{
    stlBool                       mNeedOuterCast;
    stlBool                       mNeedInnerCast;
    knlCastFuncInfo             * mOuterCastFuncInfo;
    knlCastFuncInfo             * mInnerCastFuncInfo;
    knlValueBlock               * mOuterValueBlock; /**< Outer Value Block */
    knlValueBlock               * mInnerValueBlock; /**< Inner Value Block */
    dtlPhysicalCompareFuncPtr     mCompFunc;        /**< Compare Function */
    dtlPhysicalCompareFuncPtr     mOuterValueCompFunc;  /**< Outer Value Compare Function */
    qlnxSortKeyCompareItem      * mNext;
};


/**
 * @brief Sort Key Compare List
 */
struct qlnxSortKeyCompareList
{
    stlInt32                  mCount;
    qlnxSortKeyCompareItem  * mHead;
    qlnxSortKeyCompareItem  * mTail;
};

#define QLNX_COMPARE_SORT_KEY( _aSortKeyCompareList,                                                \
                               _aOuterCurrBlockIdx,                                                 \
                               _aInnerCurrBlockIdx,                                                 \
                               _aCompareResult,                                                     \
                               _aEnv )                                                              \
{                                                                                                   \
    qlnxSortKeyCompareItem  * _sSortKeyCompareItem  = NULL;                                         \
    dtlDataValue            * _sOuterDataValue      = NULL;                                         \
    dtlDataValue            * _sInnerDataValue      = NULL;                                         \
                                                                                                    \
    _sSortKeyCompareItem = (_aSortKeyCompareList)->mHead;                                           \
    while( _sSortKeyCompareItem != NULL )                                                           \
    {                                                                                               \
        /* Outer Value 셋팅 */                                                                      \
        if( DTL_IS_NULL(                                                                            \
                KNL_GET_VALUE_BLOCK_DATA_VALUE(                                                     \
                    _sSortKeyCompareItem->mOuterValueBlock,                                         \
                    (_aOuterCurrBlockIdx) ) ) )                                                     \
        {                                                                                           \
            /* Null Data일 경우 Skip */                                                             \
            (_aCompareResult) = DTL_COMPARISON_LESS;                                                \
            break;                                                                                  \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            if( _sSortKeyCompareItem->mNeedOuterCast == STL_TRUE )                                  \
            {                                                                                       \
                _sSortKeyCompareItem->mOuterCastFuncInfo->mArgs[0].mValue.mDataValue =              \
                    KNL_GET_VALUE_BLOCK_DATA_VALUE(                                                 \
                        _sSortKeyCompareItem->mOuterValueBlock,                                     \
                        (_aOuterCurrBlockIdx) );                                                    \
                                                                                                    \
                STL_TRY( _sSortKeyCompareItem->mOuterCastFuncInfo->mCastFuncPtr(                    \
                             DTL_CAST_INPUT_ARG_CNT,                                                \
                             _sSortKeyCompareItem->mOuterCastFuncInfo->mArgs,                       \
                             (void *) & _sSortKeyCompareItem->mOuterCastFuncInfo->mCastResultBuf,   \
                             (void *)(_sSortKeyCompareItem->mOuterCastFuncInfo->mSrcTypeInfo),      \
                             KNL_DT_VECTOR( _aEnv ),                                                \
                             (_aEnv),                                                               \
                             KNL_ERROR_STACK( _aEnv ) )                                             \
                         == STL_SUCCESS );                                                          \
                _sOuterDataValue = &(_sSortKeyCompareItem->mOuterCastFuncInfo->mCastResultBuf);     \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                _sOuterDataValue =                                                                  \
                    KNL_GET_VALUE_BLOCK_DATA_VALUE(                                                 \
                        _sSortKeyCompareItem->mOuterValueBlock,                                     \
                        (_aOuterCurrBlockIdx) );                                                    \
            }                                                                                       \
        }                                                                                           \
                                                                                                    \
        /* Inner Value 셋팅 */                                                                      \
        if( DTL_IS_NULL(                                                                            \
                KNL_GET_VALUE_BLOCK_DATA_VALUE(                                                     \
                    _sSortKeyCompareItem->mInnerValueBlock,                                         \
                    (_aInnerCurrBlockIdx) ) ) )                                                     \
        {                                                                                           \
            /* Null Data일 경우 Skip */                                                             \
            (_aCompareResult) = DTL_COMPARISON_GREATER;                                             \
            break;                                                                                  \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            if( _sSortKeyCompareItem->mNeedInnerCast == STL_TRUE )                                  \
            {                                                                                       \
                _sSortKeyCompareItem->mInnerCastFuncInfo->mArgs[0].mValue.mDataValue =              \
                    KNL_GET_VALUE_BLOCK_DATA_VALUE(                                                 \
                        _sSortKeyCompareItem->mInnerValueBlock,                                     \
                        (_aInnerCurrBlockIdx) );                                                    \
                                                                                                    \
                STL_TRY( _sSortKeyCompareItem->mInnerCastFuncInfo->mCastFuncPtr(                    \
                             DTL_CAST_INPUT_ARG_CNT,                                                \
                             _sSortKeyCompareItem->mInnerCastFuncInfo->mArgs,                       \
                             (void *) & _sSortKeyCompareItem->mInnerCastFuncInfo->mCastResultBuf,   \
                             (void *)(_sSortKeyCompareItem->mInnerCastFuncInfo->mSrcTypeInfo),      \
                             KNL_DT_VECTOR( _aEnv ),                                                \
                             (_aEnv),                                                               \
                             KNL_ERROR_STACK( _aEnv ) )                                             \
                         == STL_SUCCESS );                                                          \
                _sInnerDataValue = &(_sSortKeyCompareItem->mInnerCastFuncInfo->mCastResultBuf);     \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                _sInnerDataValue =                                                                  \
                    KNL_GET_VALUE_BLOCK_DATA_VALUE(                                                 \
                        _sSortKeyCompareItem->mInnerValueBlock,                                     \
                        (_aInnerCurrBlockIdx) );                                                    \
            }                                                                                       \
        }                                                                                           \
                                                                                                    \
        (_aCompareResult) =                                                                         \
            _sSortKeyCompareItem->mCompFunc( _sOuterDataValue->mValue,                              \
                                             _sOuterDataValue->mLength,                             \
                                             _sInnerDataValue->mValue,                              \
                                             _sInnerDataValue->mLength );                           \
                                                                                                    \
        if( (_aCompareResult) != DTL_COMPARISON_EQUAL )                                             \
        {                                                                                           \
            break;                                                                                  \
        }                                                                                           \
                                                                                                    \
        _sSortKeyCompareItem = _sSortKeyCompareItem->mNext;                                         \
    }                                                                                               \
}

#define QLNX_COMPARE_ONLY_OUTER_SORT_KEY( _aSortKeyCompareList,                         \
                                          _aOuterCurrBlockIdx,                          \
                                          _aHasNullValue,                               \
                                          _aCompareResult )                             \
{                                                                                       \
    qlnxSortKeyCompareItem  * _sSortKeyCompareItem  = NULL;                             \
    dtlDataValue            * _sOuterDataValue      = NULL;                             \
    dtlDataValue            * _sPrevOuterDataValue  = NULL;                             \
                                                                                        \
    (_aHasNullValue) = STL_FALSE;                                                       \
    _sSortKeyCompareItem = (_aSortKeyCompareList)->mHead;                               \
    while( _sSortKeyCompareItem != NULL )                                               \
    {                                                                                   \
        /* Prev Outer Value 셋팅 */                                                     \
        _sPrevOuterDataValue =                                                          \
            KNL_GET_VALUE_BLOCK_DATA_VALUE(                                             \
                _sSortKeyCompareItem->mOuterValueBlock,                                 \
                (_aOuterCurrBlockIdx)-1 );                                              \
                                                                                        \
        /* Outer Value 셋팅 */                                                          \
        _sOuterDataValue =                                                              \
            KNL_GET_VALUE_BLOCK_DATA_VALUE(                                             \
                _sSortKeyCompareItem->mOuterValueBlock,                                 \
                (_aOuterCurrBlockIdx) );                                                \
                                                                                        \
        if( DTL_IS_NULL( _sOuterDataValue ) )                                           \
        {                                                                               \
            /* Null Data일 경우 Skip */                                                 \
            (_aHasNullValue) = STL_TRUE;                                                \
            break;                                                                      \
        }                                                                               \
                                                                                        \
        (_aCompareResult) =                                                             \
            _sSortKeyCompareItem->mOuterValueCompFunc( _sPrevOuterDataValue->mValue,    \
                                                       _sPrevOuterDataValue->mLength,   \
                                                       _sOuterDataValue->mValue,        \
                                                       _sOuterDataValue->mLength );     \
                                                                                        \
        if( (_aCompareResult) != DTL_COMPARISON_EQUAL )                                 \
        {                                                                               \
            break;                                                                      \
        }                                                                               \
                                                                                        \
        _sSortKeyCompareItem = _sSortKeyCompareItem->mNext;                             \
    }                                                                                   \
}

#define QLNX_SET_LOCAL_FETCH_INFO( _aLocalFetchInfo, _aParentFetchInfo )        \
    {                                                                           \
        (_aLocalFetchInfo)->mTransID  = (_aParentFetchInfo)->mTransID;          \
        (_aLocalFetchInfo)->mStmt     = (_aParentFetchInfo)->mStmt;             \
        (_aLocalFetchInfo)->mDBCStmt  = (_aParentFetchInfo)->mDBCStmt;          \
        (_aLocalFetchInfo)->mSQLStmt  = (_aParentFetchInfo)->mSQLStmt;          \
        (_aLocalFetchInfo)->mDataArea = (_aParentFetchInfo)->mDataArea;         \
        (_aLocalFetchInfo)->mOptNode  = (_aParentFetchInfo)->mOptNode;          \
        (_aLocalFetchInfo)->mStartIdx = (_aParentFetchInfo)->mStartIdx;         \
        (_aLocalFetchInfo)->mSkipCnt  = 0;                                      \
        (_aLocalFetchInfo)->mFetchCnt = QLL_FETCH_COUNT_MAX;                    \
    }


/**
 * @brief SORT MERGE JOIN Execution Node
 */
struct qlnxSortMergeJoin
{
    qlnxCommonInfo        mCommonInfo;              /**< Common Info (반드시 처음에 위치해야 함) */

    /* Cache Block Info 관련 */
    qlnxCacheBlockInfo    mInnerCacheBlockInfo;     /**< Inner Table의 Cache Block Info */
    qlnxCacheBlockInfo    mOuterCacheBlockInfo;     /**< Outer Table의 Cache Block Info */
    stlInt64              mCurrBlockIdx;            /**< Join Result의 결과가 쓰여질 Block Idx */

    /* Row Block List 관련 */
    qlnxRowBlockList      mRowBlockList;            /**< Row Block List */

    /* Value Block List 관련 */
    knlValueBlockList   * mJoinedColBlock;          /**< JOIN 결과를 저장할 Column 공간 */
    qlnxValueBlockList    mValueBlockList;          /**< Value Block의 연결 정보 관련 */

    /* Sort Instant 관련 */
    // qlnxSortInstant       mInnerSortInstant;        /**< Inner Sort Instant */
    // qlnxSortInstant       mOuterSortInstant;        /**< Outer Sort Instant */

    /* Outer Column 관련 */
    qlnxOuterColumnBlockList  mLeftGroupOuterColumnBlockList;   /**< Left Group Outer Column Value Block의 연결 정보 */
    qlnxOuterColumnBlockList  mLeftOuterColumnBlockList;    /**< Left Outer Column Value Block의 연결 정보 */
    qlnxOuterColumnBlockList  mRightOuterColumnBlockList;   /**< Right Outer Column Value Block의 연결 정보 */
    qlnxSortKeyCompareList    mSortKeyCompareList;          /**< Equal Join 처리시 Compare할 정보 */

    /* Evaluate Info 관련 */
    knlExprEvalInfo       mWhereExprEvalInfo;       /**< Where Predicate의 Evaluate Info */
    knlExprEvalInfo       mJoinExprEvalInfo;        /**< Join Predicate의 Evaluate Info */
    stlBool               mHasJoinCondition;        /**< Join Condition 존재 여부 */
    stlBool               mHasWhereCondition;       /**< Where Condition 존재 여부 */
    stlBool               mIsFirstFetch;            /**< 처음 Fetch를 하는 것인지 여부 */
    stlBool               mHasFalseFilter;          /**< 결과값으로 항상 FALSE 결과를 반환하는지 여부 */
    stlBool               mIsEOF;                   /**< 더이상 Fetch할 것이 없는지 여부 */
    stlBool               mIsGroupEOF;              /**< 더이상 Group Fetch할 것이 없는지 여부 */

    /* Outer Join을 위해 필요한 정보 */
    stlBool               mIsLeftMatched;           /**< Left Record가 Match되었었는지 여부 */
    stlBool               mIsRightMatched;          /**< Right Record가 Match되었었는지 여부 */
    stlBool               mIsSetAntiOuter;          /**< Full Outer Join에서 Right Node에 남은 데이터 처리를 위해 Reset이 되었는지 여부 */

    /* Set Flag Value Block 관련 */
    knlValueBlockList   * mSetRightFlagBlock;       /**< Right Node의 Flag를 Setting하기 위한 공간 */

    qlvJoinType           mJoinType;                /**< Join Type */

    void                * mFetchFunc;               /**< Fetch Function Pointer */
};

/**
 * @brief HASH JOIN Execution Node
 */
struct qlnxHashJoin
{
    qlnxCommonInfo        mCommonInfo;              /**< Common Info (반드시 처음에 위치해야 함) */

    /* Cache Block Info 관련 */
    qlnxCacheBlockInfo    mInnerCacheBlockInfo;     /**< Inner Table의 Cache Block Info */
    qlnxCacheBlockInfo    mOuterCacheBlockInfo;     /**< Outer Table의 Cache Block Info */
    stlInt64              mCurrBlockIdx;            /**< Join Result의 결과가 쓰여질 Block Idx */

    /* Row Block List 관련 */
    qlnxRowBlockList      mRowBlockList;            /**< Row Block List */

    /* Value Block List 관련 */
    knlValueBlockList   * mJoinedColBlock;          /**< JOIN 결과를 저장할 Column 공간 */
    qlnxValueBlockList    mValueBlockList;          /**< Value Block의 연결 정보 관련 */

    /* Sort Instant 관련 */
    // qlnxSortInstant       mInnerSortInstant;        /**< Inner Sort Instant */
    // qlnxSortInstant       mOuterSortInstant;        /**< Outer Sort Instant */

    /* Outer Column 관련 */
    qlnxOuterColumnBlockList  mLeftOuterColumnBlockList;    /**< Left Outer Column Value Block의 연결 정보 */
    qlnxOuterColumnBlockList  mRightOuterColumnBlockList;   /**< Right Outer Column Value Block의 연결 정보 */

    /* Evaluate Info 관련 */
    knlExprEvalInfo       mWhereExprEvalInfo;       /**< Where Predicate의 Evaluate Info */
    knlExprEvalInfo       mJoinExprEvalInfo;        /**< Join Predicate의 Evaluate Info */
    stlBool               mHasJoinCondition;        /**< Join Condition 존재 여부 */
    stlBool               mHasWhereCondition;       /**< Where Condition 존재 여부 */
    stlBool               mIsFirstFetch;            /**< 처음 Fetch를 하는 것인지 여부 */
    stlBool               mHasFalseFilter;          /**< 결과값으로 항상 FALSE 결과를 반환하는지 여부 */
    stlBool               mIsEOF;                   /**< 더이상 Fetch할 것이 없는지 여부 */
    stlBool               mIsSwitched;              /**< Inner와 Outer의 Switching 여부 */
    stlChar               mPadding[2];              /**< for 8 bytes align */

    qlvJoinType           mJoinType;                /**< Join Type */

    smlBlockJoinFetchInfo mBlockJoinFetchInfo;      /**< Block Join Fetch Information */
};


/***************************************************
 * SET Nodes
 ***************************************************/ 

/**
 * @brief SET UNION Execution Node
 */
struct qlnxSetUnion
{
    qlnxCommonInfo        mCommonInfo;          /**< Common Info (반드시 처음에 위치해야 함) */

    /* Value Block List 관련 */
    knlValueBlockList   * mUnionColBlock;         /**< UNION 결과를 저장할 Column 공간 */
};


/**
 * @brief Concat Execution Node
 */
struct qlnxConcat
{
    qlnxCommonInfo        mCommonInfo;              /**< Common Info (반드시 처음에 위치해야 함) */

    /* Cache Block Info 관련 */
    stlInt32              mChildCount;              /**< Child Node의 개수 */
    stlInt32              mCurChildSeq;             /**< 현재 수행중인 Child Node의 Sequence No */
    knlValueBlockList   * mCurChildColBlock;        /**< 현재 수행중인 Child Node의 ValueBlockList */
    smlRowBlock         * mCurChildRowBlock;        /**< 현재 수행중인 Child Node의 Row Block */

    /* Row Block List 관련 */
    qlnxRowBlockList      mRowBlockList;            /**< Row Block List */

    /* Value Block List 관련 */
    knlValueBlockList   * mOutputValueBlock;        /**< child에서 읽어들일 Column 공간 */
    stlInt16              mKeyColCount;
    knlValueBlockList   * mKeyValueBlock;           /**< Hash에 Insert해 볼 Column 공간 */
    stlInt64              mCurInputBlockIdx;        /**< 현재 처리할 child Block Idx */
    stlInt64              mCurInputBlockCnt;        /**< 현재 Fetch한 child Block의 개수 */

    /* Outer Column 관련 */
    knlValueBlockList   * mOuterColBlock;           /**< Outer Column Value Block List */
    knlValueBlockList   * mOuterOrgColBlock;        /**< Original Value Block List for Outer Columns */

    /* Hash Instant Index 생성 */
    qlnInstantTable       mInstantTable;            /**< Instant Table Info */
    smlRowBlock           mHashRowBlock;            /**< Hash에 Insert시 사용될 RowBlock 정보 */
    knlValueBlockList   * mDupFlagBlockList;        /**< child에서 읽어들일 Column 공간 */
    smlIndexAttr          mHashTableAttr;           /**< Hash Instant Table 생성 Attribute */


    /* Evaluate Info 관련 */
    stlBool               mIsFirstFetch;            /**< 처음 Fetch를 하는 것인지 여부 */
    stlBool               mIsEOF;                   /**< 더이상 Fetch할 것이 없는지 여부 */
    stlBool               mIsCurChildEOF;           /**< 현재 Child Node가 EOF인지 여부 */
    stlChar               mPadding[5];              /**< for 8 bytes align */
};



/****************************************************
 * Functions
 ****************************************************/ 

/* SELECT STATEMENT */

/* INSERT STATEMENT */

/* DELETE STATEMENT */

/* UPDATE STATEMENT */

/* TRUNCATE TABLE STATEMENT */

/* MERGE TABLE STATEMENT */

/* Misc */



typedef stlStatus (*qlnxInitializePtr) ( smlTransId              aTransID,
                                         smlStatement          * aStmt,
                                         qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv );

typedef stlStatus (*qlnxExecutePtr) ( smlTransId              aTransID,
                                      smlStatement          * aStmt,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv );

typedef stlStatus (*qlnxFetchPtr) ( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                    stlInt64              * aUsedBlockCnt,
                                    stlBool               * aEOF,
                                    qllEnv                * aEnv );

typedef stlStatus (*qlnxFinalizePtr) ( qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv );


/**
 * @brief Execution Node Function Pointer Structure
 */
struct qlnxFunctionPtr
{
    qlnxInitializePtr     mInitialize;
    qlnxExecutePtr        mExecute;
    qlnxFetchPtr          mFetch;
    qlnxFinalizePtr       mFinalize;
};


extern qlnxFunctionPtr qlnxNodeFunctions[ QLL_PLAN_NODE_TYPE_MAX ];


/**
 * @brief 하위 노드 Fetch 를 위한 Fetch 정보
 */
struct qlnxFetchNodeInfo
{
    /* 변하지 않는 정보 (IN) */
    smlTransId              mTransID;
    smlStatement          * mStmt;
    qllDBCStmt            * mDBCStmt;
    qllStatement          * mSQLStmt;
    qllDataArea           * mDataArea;

    /* Node마다 설정이 필요한 정보 (IN) */
    qllOptimizationNode   * mOptNode;
    stlInt32                mStartIdx;

    /* Node마다 설정이 필요한 정보 (IN/OUT) */
    stlInt64                mSkipCnt;
    stlInt64                mFetchCnt;
};


/*********************************************************
 * NA Functions
 ********************************************************/

stlStatus qlnxInitializeNA( smlTransId              aTransID,
                            smlStatement          * aStmt,
                            qllDBCStmt            * aDBCStmt,
                            qllStatement          * aSQLStmt,
                            qllOptimizationNode   * aOptNode,
                            qllDataArea           * aDataArea,
                            qllEnv                * aEnv );

stlStatus qlnxExecuteNA( smlTransId              aTransID,
                         smlStatement          * aStmt,
                         qllDBCStmt            * aDBCStmt,
                         qllStatement          * aSQLStmt,
                         qllOptimizationNode   * aOptNode,
                         qllDataArea           * aDataArea,
                         qllEnv                * aEnv );

stlStatus qlnxFetchNA( qlnxFetchNodeInfo     * aFetchNodeInfo,
                       stlInt64              * aUsedBlockCnt,
                       stlBool               * aEOF,
                       qllEnv                * aEnv );

stlStatus qlnxFinalizeNA( qllOptimizationNode   * aOptNode,
                          qllDataArea           * aDataArea,
                          qllEnv                * aEnv );



/*********************************************************
 * SELECT STATEMENT
 ********************************************************/

stlStatus qlnxInitializeSelectStmt( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv );

stlStatus qlnxExecuteSelectStmt( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv );

stlStatus qlnxFetchSelectStmt( qlnxFetchNodeInfo     * aFetchNodeInfo,
                               stlInt64              * aUsedBlockCnt,
                               stlBool               * aEOF,
                               qllEnv                * aEnv );

stlStatus qlnxFinalizeSelectStmt( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );


/*********************************************************
 * INSERT STATEMENT
 ********************************************************/

stlStatus qlnxInitializeInsertStmt( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv );

stlStatus qlnxExecuteInsertStmt( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv );

stlStatus qlnxExecuteInsertValues( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv );

stlStatus qlnxExecuteInsertSelect( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv );

#if 0
stlStatus qlnxFetchInsertStmt( qlnxFetchNodeInfo     * aFetchNodeInfo,
                               stlInt64              * aUsedBlockCnt,
                               stlBool               * aEOF,
                               qllEnv                * aEnv );
#endif

stlStatus qlnxFinalizeInsertStmt( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );


/*********************************************************
 * UPDATE STATEMENT
 ********************************************************/

stlStatus qlnxInitializeUpdateStmt( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv );

stlStatus qlnxExecuteUpdateStmt( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv );

#if 0
stlStatus qlnxFetchUpdateStmt( qlnxFetchNodeInfo     * aFetchNodeInfo,
                               stlInt64              * aUsedBlockCnt,
                               stlBool               * aEOF,
                               qllEnv                * aEnv );
#endif

stlStatus qlnxFinalizeUpdateStmt( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );


/*********************************************************
 * DELETE STATEMENT
 ********************************************************/

stlStatus qlnxInitializeDeleteStmt( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv );

stlStatus qlnxExecuteDeleteStmt( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv );

#if 0
stlStatus qlnxFetchDeleteStmt( qlnxFetchNodeInfo     * aFetchNodeInfo,
                               stlInt64              * aUsedBlockCnt,
                               stlBool               * aEOF,
                               qllEnv                * aEnv );
#endif

stlStatus qlnxFinalizeDeleteStmt( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );


/*********************************************************
 * QUERY SPEC
 ********************************************************/

stlStatus qlnxInitializeQuerySpec( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv );

stlStatus qlnxExecuteQuerySpec( smlTransId              aTransID,
                                smlStatement          * aStmt,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv );

stlStatus qlnxFetchQuerySpec( qlnxFetchNodeInfo     * aFetchNodeInfo,
                              stlInt64              * aUsedBlockCnt,
                              stlBool               * aEOF,
                              qllEnv                * aEnv );

stlStatus qlnxFinalizeQuerySpec( qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv );


stlStatus qlnxGetBindedValue4FETCH( qllDBCStmt        * aDBCStmt,
                                    qllStatement      * aSQLStmt,
                                    stlBool             aIsFetch,   
                                    qlvInitExpression * aFetchExpr,
                                    stlInt64          * aBindValue,
                                    qllEnv            * aEnv );

/*********************************************************
 * QUERY SET
 ********************************************************/


stlStatus qlnxInitializeQuerySet( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );

stlStatus qlnxExecuteQuerySet( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               qllEnv                * aEnv );

stlStatus qlnxFetchQuerySet( qlnxFetchNodeInfo     * aFetchNodeInfo,
                             stlInt64              * aUsedBlockCnt,
                             stlBool               * aEOF,
                             qllEnv                * aEnv );

stlStatus qlnxFinalizeQuerySet( qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv );


stlStatus qlnxRevertQuerySetTarget( qllDataArea         * aDataArea,
                                    qllOptimizationNode * aOptNode,
                                    qllEnv              * aEnv );
                                    
                               



/*********************************************************
 * UNION ALL
 ********************************************************/

stlStatus qlnxInitializeUnionAll( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );

stlStatus qlnxExecuteUnionAll( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               qllEnv                * aEnv );

stlStatus qlnxFetchUnionAll( qlnxFetchNodeInfo     * aFetchNodeInfo,
                             stlInt64              * aUsedBlockCnt,
                             stlBool               * aEOF,
                             qllEnv                * aEnv );

stlStatus qlnxFinalizeUnionAll( qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv );

/*********************************************************
 * UNION DISTINCT
 ********************************************************/

stlStatus qlnxInitializeUnionDistinct( smlTransId              aTransID,
                                       smlStatement          * aStmt,
                                       qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv );

stlStatus qlnxExecuteUnionDistinct( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv );

stlStatus qlnxFetchUnionDistinct( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                  stlInt64              * aUsedBlockCnt,
                                  stlBool               * aEOF,
                                  qllEnv                * aEnv );

stlStatus qlnxFinalizeUnionDistinct( qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv );


/*********************************************************
 * EXCEPT (ALL/DISTINCT)
 ********************************************************/

stlStatus qlnxInitializeExcept( smlTransId              aTransID,
                                smlStatement          * aStmt,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv );

stlStatus qlnxExecuteExcept( smlTransId              aTransID,
                             smlStatement          * aStmt,
                             qllDBCStmt            * aDBCStmt,
                             qllStatement          * aSQLStmt,
                             qllOptimizationNode   * aOptNode,
                             qllDataArea           * aDataArea,
                             qllEnv                * aEnv );

stlStatus qlnxFetchExcept( qlnxFetchNodeInfo     * aFetchNodeInfo,
                           stlInt64              * aUsedBlockCnt,
                           stlBool               * aEOF,
                           qllEnv                * aEnv );

stlStatus qlnxFinalizeExcept( qllOptimizationNode   * aOptNode,
                              qllDataArea           * aDataArea,
                              qllEnv                * aEnv );

/*********************************************************
 * INTERSECT (ALL/DISTINCT)
 ********************************************************/

stlStatus qlnxInitializeIntersect( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv );

stlStatus qlnxExecuteIntersect( smlTransId              aTransID,
                                smlStatement          * aStmt,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv );

stlStatus qlnxFetchIntersect( qlnxFetchNodeInfo     * aFetchNodeInfo,
                              stlInt64              * aUsedBlockCnt,
                              stlBool               * aEOF,
                              qllEnv                * aEnv );

stlStatus qlnxFinalizeIntersect( qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv );


/*********************************************************
 * SUBQUERY
 ********************************************************/
stlStatus qlnxInitializeSubQuery( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );

stlStatus qlnxExecuteSubQuery( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               qllEnv                * aEnv );

stlStatus qlnxFetchSubQuery( qlnxFetchNodeInfo     * aFetchNodeInfo,
                             stlInt64              * aUsedBlockCnt,
                             stlBool               * aEOF,
                             qllEnv                * aEnv );

stlStatus qlnxFinalizeSubQuery( qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv );


/*********************************************************
 * SUBQUERY LIST
 ********************************************************/
stlStatus qlnxInitializeSubQueryList( smlTransId              aTransID,
                                      smlStatement          * aStmt,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv );

stlStatus qlnxExecuteSubQueryList( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv );

stlStatus qlnxFetchSubQueryList( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                 stlInt64              * aUsedBlockCnt,
                                 stlBool               * aEOF,
                                 qllEnv                * aEnv );

stlStatus qlnxFinalizeSubQueryList( qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv );


/*********************************************************
 * SUBQUERY FUNC
 ********************************************************/
stlStatus qlnxInitializeSubQueryFunc( smlTransId              aTransID,
                                      smlStatement          * aStmt,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv );

stlStatus qlnxExecuteSubQueryFunc( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv );

stlStatus qlnxFetchSubQueryFunc( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                 stlInt64              * aUsedBlockCnt,
                                 stlBool               * aEOF,
                                 qllEnv                * aEnv );

stlStatus qlnxFinalizeSubQueryFunc( qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv );


/*********************************************************
 * SUBQUERY FILTER
 ********************************************************/
stlStatus qlnxInitializeSubQueryFilter( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv );

stlStatus qlnxExecuteSubQueryFilter( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv );

stlStatus qlnxFetchSubQueryFilter( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                   stlInt64              * aUsedBlockCnt,
                                   stlBool               * aEOF,
                                   qllEnv                * aEnv );

stlStatus qlnxFinalizeSubQueryFilter( qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv );


/*********************************************************
 * CONCAT
 ********************************************************/
stlStatus qlnxInitializeConcat( smlTransId              aTransID,
                                smlStatement          * aStmt,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv );

stlStatus qlnxExecuteConcat( smlTransId              aTransID,
                             smlStatement          * aStmt,
                             qllDBCStmt            * aDBCStmt,
                             qllStatement          * aSQLStmt,
                             qllOptimizationNode   * aOptNode,
                             qllDataArea           * aDataArea,
                             qllEnv                * aEnv );

stlStatus qlnxFetchConcat( qlnxFetchNodeInfo     * aFetchNodeInfo,
                           stlInt64              * aUsedBlockCnt,
                           stlBool               * aEOF,
                           qllEnv                * aEnv );

stlStatus qlnxFinalizeConcat( qllOptimizationNode   * aOptNode,
                              qllDataArea           * aDataArea,
                              qllEnv                * aEnv );


/*********************************************************
 * NESTED LOOPS JOIN
 ********************************************************/

stlStatus qlnxInitializeNestedLoopsJoin( smlTransId              aTransID,
                                         smlStatement          * aStmt,
                                         qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv );

stlStatus qlnxExecuteNestedLoopsJoin( smlTransId              aTransID,
                                      smlStatement          * aStmt,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv );

stlStatus qlnxFetchNestedLoopsJoin( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                    stlInt64              * aUsedBlockCnt,
                                    stlBool               * aEOF,
                                    qllEnv                * aEnv );

stlStatus qlnxFinalizeNestedLoopsJoin( qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv );

typedef stlStatus (*qlnxNestedJoinFetchFuncPtr) ( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                  stlInt64              * aUsedBlockCnt,
                                                  stlBool               * aEOF,
                                                  qllEnv                * aEnv );

extern const qlnxNestedJoinFetchFuncPtr qlnxNestedJoinFetchFuncList[ QLV_JOIN_TYPE_MAX ];

stlStatus qlnxFetchNestedLoopsJoin_Inner( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                          stlInt64              * aUsedBlockCnt,
                                          stlBool               * aEOF,
                                          qllEnv                * aEnv );

stlStatus qlnxFetchNestedLoopsJoin_Outer( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                          stlInt64              * aUsedBlockCnt,
                                          stlBool               * aEOF,
                                          qllEnv                * aEnv );

stlStatus qlnxFetchNestedLoopsJoin_Semi( qlnxFetchNodeInfo  * aFetchNodeInfo,
                                         stlInt64           * aUsedBlockCnt,
                                         stlBool            * aEOF,
                                         qllEnv             * aEnv );

stlStatus qlnxFetchNestedLoopsJoin_AntiSemi( qlnxFetchNodeInfo  * aFetchNodeInfo,
                                             stlInt64           * aUsedBlockCnt,
                                             stlBool            * aEOF,
                                             qllEnv             * aEnv );


/*********************************************************
 * SORT MERGE JOIN
 ********************************************************/

stlStatus qlnxInitializeSortMergeJoin( smlTransId              aTransID,
                                       smlStatement          * aStmt,
                                       qllDBCStmt            * aDBCStmt,
                                       qllStatement          * aSQLStmt,
                                       qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv );

stlStatus qlnxExecuteSortMergeJoin( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv );

stlStatus qlnxFetchSortMergeJoin( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                  stlInt64              * aUsedBlockCnt,
                                  stlBool               * aEOF,
                                  qllEnv                * aEnv );

stlStatus qlnxFinalizeSortMergeJoin( qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv );

typedef stlStatus (*qlnxMergeJoinFetchFuncPtr) ( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                 stlInt64              * aUsedBlockCnt,
                                                 stlBool               * aEOF,
                                                 qllEnv                * aEnv );

stlStatus qlnxFetchSortMergeJoin_Instant_Inner( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                stlInt64              * aUsedBlockCnt,
                                                stlBool               * aEOF,
                                                qllEnv                * aEnv );

stlStatus qlnxFetchSortMergeJoin_Instant_Outer( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                stlInt64              * aUsedBlockCnt,
                                                stlBool               * aEOF,
                                                qllEnv                * aEnv );

stlStatus qlnxFetchSortMergeJoin_Instant_AntiOuter( qlnxFetchNodeInfo * aFetchNodeInfo,
                                                    stlInt64          * aUsedBlockCnt,
                                                    stlBool           * aEOF,
                                                    qllEnv            * aEnv );

stlStatus qlnxFetchSortMergeJoin_Instant_Semi( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                               stlInt64              * aUsedBlockCnt,
                                               stlBool               * aEOF,
                                               qllEnv                * aEnv );

stlStatus qlnxFetchSortMergeJoin_Instant_AntiSemi( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                   stlInt64              * aUsedBlockCnt,
                                                   stlBool               * aEOF,
                                                   qllEnv                * aEnv );

stlStatus qlnxFetchSortMergeJoin_Index_Inner( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                              stlInt64              * aUsedBlockCnt,
                                              stlBool               * aEOF,
                                              qllEnv                * aEnv );

stlStatus qlnxFetchSortMergeJoin_Index_Outer( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                              stlInt64              * aUsedBlockCnt,
                                              stlBool               * aEOF,
                                              qllEnv                * aEnv );

stlStatus qlnxFetchSortMergeJoin_Index_Semi( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                             stlInt64              * aUsedBlockCnt,
                                             stlBool               * aEOF,
                                             qllEnv                * aEnv );

stlStatus qlnxFetchSortMergeJoin_Index_AntiSemi( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                 stlInt64              * aUsedBlockCnt,
                                                 stlBool               * aEOF,
                                                 qllEnv                * aEnv );


/*********************************************************
 * HASH JOIN
 ********************************************************/

stlStatus qlnxInitializeHashJoin( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );

stlStatus qlnxExecuteHashJoin( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               qllEnv                * aEnv );

stlStatus qlnxFetchHashJoin( qlnxFetchNodeInfo     * aFetchNodeInfo,
                             stlInt64              * aUsedBlockCnt,
                             stlBool               * aEOF,
                             qllEnv                * aEnv );

stlStatus qlnxFinalizeHashJoin( qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv );


/*********************************************************
 * TABLE ACCESS
 ********************************************************/

stlStatus qlnxInitializeTableAccess( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv );

stlStatus qlnxExecuteTableAccess( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );

stlStatus qlnxFetchTableAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                stlInt64              * aUsedBlockCnt,
                                stlBool               * aEOF,
                                qllEnv                * aEnv );

stlStatus qlnxFinalizeTableAccess( qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv );


/*********************************************************
 * INDEX ACCESS
 ********************************************************/

stlStatus qlnxInitializeIndexAccess( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv );

stlStatus qlnxExecuteIndexAccess( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );

stlStatus qlnxFetchIndexAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                stlInt64              * aUsedBlockCnt,
                                stlBool               * aEOF,
                                qllEnv                * aEnv );

stlStatus qlnxFinalizeIndexAccess( qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv );

void qlnxGetRangePatternValue( dtlDataValue    * aConstValue,
                               knlCompareList  * aCompList );

stlStatus qlnxRefineIndexFetchInfo( ellDictHandle     * aIndexHandle,
                                    qloIndexScanInfo  * aIndexScanInfo,
                                    smlFetchInfo      * aFetchInfo,
                                    qlnxIndexAccess   * aExeIndexAccess,
                                    qllEnv            * aEnv );

stlStatus qlnxFetchIndexAccessInKeyRange( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                          stlInt64              * aUsedBlockCnt,
                                          stlBool               * aEOF,
                                          qllEnv                * aEnv );

stlStatus qlnxFetchIndexAccessInKeyRangeAggrDist( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                  stlInt64              * aUsedBlockCnt,
                                                  stlBool               * aEOF,
                                                  qllEnv                * aEnv );


/*********************************************************
 * ROWID ACCESS
 ********************************************************/

stlStatus qlnxInitializeRowIdAccess( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv );

stlStatus qlnxExecuteRowIdAccess( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv );

stlStatus qlnxFetchRowIdAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                stlInt64              * aUsedBlockCnt,
                                stlBool               * aEOF,
                                qllEnv                * aEnv );

stlStatus qlnxFinalizeRowIdAccess( qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv );


/*********************************************************
 * FAST DUAL
 ********************************************************/

#if 0
stlStatus qlnxInitializeFastDualAccess( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv );

stlStatus qlnxExecuteFastDualAccess( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv );

stlStatus qlnxFetchFastDualAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                   stlInt64              * aUsedBlockCnt,
                                   stlBool               * aEOF,
                                   qllEnv                * aEnv );

stlStatus qlnxFinalizeFastDualAccess( qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv );
#endif

/*********************************************************
 * INSTANT 공통
 ********************************************************/

stlStatus qlnxDropInstantTable( qlnInstantTable  * aInstantTable,
                                qllEnv           * aEnv );

stlStatus qlnxTruncateInstantTable( smlStatement    * aStmt,
                                    qlnInstantTable * aInstantTable,
                                    qllEnv          * aEnv );

/*********************************************************
 * FLAT INSTANT
 ********************************************************/

#if 0
stlStatus qlnxInitializeFlatInstantAccess( smlTransId              aTransID,
                                           smlStatement          * aStmt,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           qllEnv                * aEnv );

stlStatus qlnxExecuteFlatInstantAccess( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv );

stlStatus qlnxFetchFlatInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                      stlInt64              * aUsedBlockCnt,
                                      stlBool               * aEOF,
                                      qllEnv                * aEnv );

stlStatus qlnxFinalizeFlatInstantAccess( qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv );
#endif

stlStatus qlnxCreateFlatInstantTable( smlTransId               aTransID,
                                      smlStatement           * aStmt,
                                      qlnInstantTable        * aInstantTable,
                                      stlBool                  aIsScrollableInstant,
                                      qllEnv                 * aEnv );


/*********************************************************
 * SORT INSTANT
 ********************************************************/

stlStatus qlnxInitializeSortInstantAccess( smlTransId              aTransID,
                                           smlStatement          * aStmt,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           qllEnv                * aEnv );

stlStatus qlnxExecuteSortInstantAccess( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv );

stlStatus qlnxFetchSortInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                      stlInt64              * aUsedBlockCnt,
                                      stlBool               * aEOF,
                                      qllEnv                * aEnv );

stlStatus qlnxFinalizeSortInstantAccess( qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv );


stlStatus qlnxCreateSortInstantTable( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      qlnInstantTable    * aInstantTable,
                                      void               * aBaseTableHandle,
                                      stlUInt16            aKeyColCount,
                                      knlValueBlockList  * aKeyColList,
                                      stlUInt8           * aKeyColFlags,
                                      stlBool              aIsTopDown,
                                      stlBool              aIsVolatile,
                                      stlBool              aIsLeafOnly,
                                      stlBool              aIsDistinct,
                                      stlBool              aNullExcluded,
                                      qllEnv             * aEnv );


/*********************************************************
 * HASH INSTANT
 ********************************************************/

#if 0
stlStatus qlnxInitializeHashInstantAccess( smlTransId              aTransID,
                                           smlStatement          * aStmt,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           qllEnv                * aEnv );

stlStatus qlnxExecuteHashInstantAccess( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv );

stlStatus qlnxFetchHashInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                      stlInt64              * aUsedBlockCnt,
                                      stlBool               * aEOF,
                                      qllEnv                * aEnv );

stlStatus qlnxFinalizeHashInstantAccess( qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv );
#endif

stlStatus qlnxCreateHashInstantTable( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      qlnInstantTable    * aInstantTable,
                                      void               * aBaseTableHandle,
                                      stlUInt16            aKeyColCount,
                                      knlValueBlockList  * aColList,
                                      smlIndexAttr       * aHashTableAttr,
                                      qllEnv             * aEnv );


/*********************************************************
 * GROUP HASH INSTANT
 ********************************************************/

stlStatus qlnxInitializeGroupHashInstantAccess( smlTransId              aTransID,
                                                smlStatement          * aStmt,
                                                qllDBCStmt            * aDBCStmt,
                                                qllStatement          * aSQLStmt,
                                                qllOptimizationNode   * aOptNode,
                                                qllDataArea           * aDataArea,
                                                qllEnv                * aEnv );

stlStatus qlnxExecuteGroupHashInstantAccess( smlTransId              aTransID,
                                             smlStatement          * aStmt,
                                             qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             qllEnv                * aEnv );

stlStatus qlnxFetchGroupHashInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                           stlInt64              * aUsedBlockCnt,
                                           stlBool               * aEOF,
                                           qllEnv                * aEnv );

stlStatus qlnxFinalizeGroupHashInstantAccess( qllOptimizationNode   * aOptNode,
                                              qllDataArea           * aDataArea,
                                              qllEnv                * aEnv );

stlStatus qlnxCreateGroupHashInstantTable( smlTransId           aTransID,
                                           smlStatement       * aStmt,
                                           qlnInstantTable    * aInstantTable,
                                           stlBool              aSkipNullFlag,
                                           stlUInt16            aKeyColCount,
                                           knlValueBlockList  * aColList,
                                           qllEnv             * aEnv );

stlStatus qlnxFetchAggrDistGroupHashInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                                   stlInt64              * aUsedBlockCnt,
                                                   stlBool               * aEOF,
                                                   qllEnv                * aEnv );

/*********************************************************
 * GROUP SORT INSTANT
 ********************************************************/

#if 0
stlStatus qlnxInitializeGroupSortInstantAccess( smlTransId              aTransID,
                                                smlStatement          * aStmt,
                                                qllDBCStmt            * aDBCStmt,
                                                qllStatement          * aSQLStmt,
                                                qllOptimizationNode   * aOptNode,
                                                qllDataArea           * aDataArea,
                                                qllEnv                * aEnv );

stlStatus qlnxExecuteGroupSortInstantAccess( smlTransId              aTransID,
                                             smlStatement          * aStmt,
                                             qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             qllEnv                * aEnv );

stlStatus qlnxFetchGroupSortInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                           stlInt64              * aUsedBlockCnt,
                                           stlBool               * aEOF,
                                           qllEnv                * aEnv );

stlStatus qlnxFinalizeGroupSortInstantAccess( qllOptimizationNode   * aOptNode,
                                              qllDataArea           * aDataArea,
                                              qllEnv                * aEnv );

stlStatus qlnxCreateGroupSortInstantTable( smlTransId           aTransID,
                                           smlStatement       * aStmt,
                                           qlnInstantTable    * aInstantTable,
                                           void               * aBaseTableHandle,
                                           stlUInt16            aKeyColCount,
                                           knlValueBlockList  * aKeyColList,
                                           stlUInt8           * aKeyColFlags,
                                           stlBool              aIsTopDown,
                                           stlBool              aIsVolatile,
                                           stlBool              aIsLeafOnly,
                                           stlBool              aIsUnique,
                                           stlBool              aNullExcluded,
                                           qllEnv             * aEnv );
#endif


/*********************************************************
 * AGGREGATION
 ********************************************************/

stlStatus qlnxInitializeHashAggregation( smlTransId              aTransID,
                                         smlStatement          * aStmt,
                                         qllDBCStmt            * aDBCStmt,
                                         qllStatement          * aSQLStmt,
                                         qllOptimizationNode   * aOptNode,
                                         qllDataArea           * aDataArea,
                                         qllEnv                * aEnv );

stlStatus qlnxExecuteHashAggregation( smlTransId              aTransID,
                                      smlStatement          * aStmt,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv );

stlStatus qlnxFetchHashAggregation( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                    stlInt64              * aUsedBlockCnt,
                                    stlBool               * aEOF,
                                    qllEnv                * aEnv );

stlStatus qlnxFinalizeHashAggregation( qllOptimizationNode   * aOptNode,
                                       qllDataArea           * aDataArea,
                                       qllEnv                * aEnv );

stlStatus qlnxInitializeAggrDist( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  stlInt32                aAggrDistCnt,
                                  qlnfAggrDistExecInfo  * aAggrDistInfo,
                                  qllEnv                * aEnv );

stlStatus qlnxFetchAggrDist( smlStatement          * aStmt,
                             void                  * aIterator,
                             smlFetchInfo          * aFetchInfo,
                             stlInt32                aAggrCnt,
                             stlInt32                aAggrDistCnt,
                             qlnfAggrExecInfo      * aAggrInfo,
                             qlnfAggrDistExecInfo  * aAggrDistInfo,
                             stlInt64              * aUsedBlockCnt,
                             stlBool               * aEOF,
                             qllEnv                * aEnv );

stlStatus qlnxFinalizeAggrDist( stlInt32                aAggrDistCnt,
                                qlnfAggrDistExecInfo  * aAggrDistInfo,
                                qllEnv                * aEnv );


stlStatus qlnxInitializeAggrConflictCheck( smlTransId              aTransID,
                                           smlStatement          * aStmt,
                                           stlInt32                aAggrDistCnt,
                                           qlnxConflictCheckInfo * aConflictCheckInfo,
                                           qllEnv                * aEnv );

stlStatus qlnxFinalizeAggrConflictCheck( stlInt32                aAggrDistCnt,
                                         qlnxConflictCheckInfo * aConflictCheckInfo,
                                         qllEnv                * aEnv );


stlStatus qlnxInitAggrFunc( smlAggrFuncInfo   * aAggrExecInfo,
                            qllEnv            * aEnv );


/*********************************************************
 * GROUP HASH INSTANT
 ********************************************************/

stlStatus qlnxInitializeGroup( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               qllEnv                * aEnv );

stlStatus qlnxExecuteGroup( smlTransId              aTransID,
                            smlStatement          * aStmt,
                            qllDBCStmt            * aDBCStmt,
                            qllStatement          * aSQLStmt,
                            qllOptimizationNode   * aOptNode,
                            qllDataArea           * aDataArea,
                            qllEnv                * aEnv );

stlStatus qlnxFetchGroup( qlnxFetchNodeInfo     * aFetchNodeInfo,
                          stlInt64              * aUsedBlockCnt,
                          stlBool               * aEOF,
                          qllEnv                * aEnv );

stlStatus qlnxFinalizeGroup( qllOptimizationNode   * aOptNode,
                             qllDataArea           * aDataArea,
                             qllEnv                * aEnv );

stlStatus qlnxGroupBuildAggrExecInfo( stlInt32            aAggrCnt,
                                      qlnfAggrExecInfo  * aAggrExecInfo,
                                      stlInt32            aStartIdx,
                                      stlInt32            aGroupRecCnt,
                                      qllEnv            * aEnv );

stlStatus qlnxGroupBuildAggrDistExecInfo( smlStatement          * aStmt,
                                          stlInt32                aAggrDistCnt,
                                          qlnfAggrDistExecInfo  * aAggrDistExecInfo,
                                          stlInt32                aStartIdx,
                                          stlInt32                aGroupRecCnt,
                                          knlValueBlockList     * aDupCheckBlock,
                                          smlRowBlock           * aRowBlock,
                                          qllEnv                * aEnv );


/*********************************************************
 * SORT JOIN INSTANT
 ********************************************************/

stlStatus qlnxInitializeSortJoinInstantAccess( smlTransId              aTransID,
                                               smlStatement          * aStmt,
                                               qllDBCStmt            * aDBCStmt,
                                               qllStatement          * aSQLStmt,
                                               qllOptimizationNode   * aOptNode,
                                               qllDataArea           * aDataArea,
                                               qllEnv                * aEnv );

stlStatus qlnxExecuteSortJoinInstantAccess( smlTransId              aTransID,
                                            smlStatement          * aStmt,
                                            qllDBCStmt            * aDBCStmt,
                                            qllStatement          * aSQLStmt,
                                            qllOptimizationNode   * aOptNode,
                                            qllDataArea           * aDataArea,
                                            qllEnv                * aEnv );

stlStatus qlnxGroupInitSortJoinInstantAccess( smlTransId              aTransID,
                                              smlStatement          * aStmt,
                                              qllDBCStmt            * aDBCStmt,
                                              qllStatement          * aSQLStmt,
                                              qllOptimizationNode   * aOptNode,
                                              qllDataArea           * aDataArea,
                                              stlBool               * aEOF,
                                              qllEnv                * aEnv );

stlStatus qlnxGroupNextSortJoinInstantAccess( smlTransId              aTransID,
                                              smlStatement          * aStmt,
                                              qllDBCStmt            * aDBCStmt,
                                              qllStatement          * aSQLStmt,
                                              qllOptimizationNode   * aOptNode,
                                              qllDataArea           * aDataArea,
                                              stlBool               * aEOF,
                                              qllEnv                * aEnv );

stlStatus qlnxGroupResetSortJoinInstantAccess( smlTransId             aTransID,
                                               smlStatement         * aStmt,
                                               qllDBCStmt           * aDBCStmt,
                                               qllStatement         * aSQLStmt,
                                               qllOptimizationNode  * aOptNode,
                                               qllDataArea          * aDataArea,
                                               stlBool              * aEOF,
                                               qllEnv               * aEnv );


stlStatus qlnxSetFlagSortJoinInstantAccess( smlStatement        * aStmt,
                                            qllOptimizationNode * aOptNode,
                                            qllDataArea         * aDataArea,
                                            knlValueBlockList   * aSetFlagBlock,
                                            qllEnv              * aEnv );


stlStatus qlnxSetFetchRowFlagSortJoinInstantAccess( smlStatement        * aStmt,
                                                    qllOptimizationNode * aOptNode,
                                                    qllDataArea         * aDataArea,
                                                    stlInt8               aRowFlag,
                                                    qllEnv              * aEnv );


stlStatus qlnxFetchSortJoinInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                          stlInt64              * aUsedBlockCnt,
                                          stlBool               * aEOF,
                                          qllEnv                * aEnv );

stlStatus qlnxFinalizeSortJoinInstantAccess( qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             qllEnv                * aEnv );


/*********************************************************
 * HASH JOIN INSTANT
 ********************************************************/

stlStatus qlnxInitializeHashJoinInstantAccess( smlTransId              aTransID,
                                               smlStatement          * aStmt,
                                               qllDBCStmt            * aDBCStmt,
                                               qllStatement          * aSQLStmt,
                                               qllOptimizationNode   * aOptNode,
                                               qllDataArea           * aDataArea,
                                               qllEnv                * aEnv );

stlStatus qlnxExecuteHashJoinInstantAccess( smlTransId              aTransID,
                                            smlStatement          * aStmt,
                                            qllDBCStmt            * aDBCStmt,
                                            qllStatement          * aSQLStmt,
                                            qllOptimizationNode   * aOptNode,
                                            qllDataArea           * aDataArea,
                                            qllEnv                * aEnv );

stlStatus qlnxFetchHashJoinInstantAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                          stlInt64              * aUsedBlockCnt,
                                          stlBool               * aEOF,
                                          qllEnv                * aEnv );

stlStatus qlnxFinalizeHashJoinInstantAccess( qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             qllEnv                * aEnv );

stlStatus qlnxCreateHashJoinInstantIndex( smlTransId           aTransID,
                                          smlStatement       * aStmt,
                                          qlnInstantTable    * aInstantTable,
                                          stlUInt16            aKeyColCount,
                                          knlValueBlockList  * aColList,
                                          smlIndexAttr       * aHashTableAttr,
                                          qllEnv             * aEnv );


/*********************************************************
 * Sub Query Functions
 ********************************************************/

stlStatus qlnxSubQueryFuncExists( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  stlInt32                aIdx,
                                  qllEnv                * aEnv );
    
stlStatus qlnxSubQueryFuncExistsWithInstant( smlTransId              aTransID,
                                             smlStatement          * aStmt,
                                             qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             stlInt32                aIdx,
                                             qllEnv                * aEnv );

stlStatus qlnxSubQueryFuncCompAnyOrdering( smlTransId              aTransID,
                                           smlStatement          * aStmt,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           stlInt32                aIdx,
                                           qllEnv                * aEnv );

stlStatus qlnxSubQueryFuncCompAnyWithArrayOrdering( smlTransId              aTransID,
                                                    smlStatement          * aStmt,
                                                    qllDBCStmt            * aDBCStmt,
                                                    qllStatement          * aSQLStmt,
                                                    qllOptimizationNode   * aOptNode,
                                                    qllDataArea           * aDataArea,
                                                    stlInt32                aIdx,
                                                    qllEnv                * aEnv );

stlStatus qlnxSubQueryFuncCompAnyExact( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        stlInt32                aIdx,
                                        qllEnv                * aEnv );

stlStatus qlnxSubQueryFuncCompAnyWithInstantExact( smlTransId              aTransID,
                                                   smlStatement          * aStmt,
                                                   qllDBCStmt            * aDBCStmt,
                                                   qllStatement          * aSQLStmt,
                                                   qllOptimizationNode   * aOptNode,
                                                   qllDataArea           * aDataArea,
                                                   stlInt32                aIdx,
                                                   qllEnv                * aEnv );

void qlnxRebuildSubQueryFuncFetchInfo( qlnxListFuncInstantFetchInfo * aListFuncInstantFetchInfo,
                                       knlValueBlockList            * aReadColList,
                                       stlInt32                       aKeyColCnt,
                                       stlInt64                       aFetchCnt );

stlStatus qlnxAddMeetBlockIntoInstantTable( qlnInstantTable   * aInstantTable,
                                            smlRowBlock       * aRowBlock,
                                            knlValueBlockList * aStoreBlockList,
                                            stlBool           * aMeetArray,
                                            stlInt64            aSkipCnt,
                                            stlInt64            aUsedCnt,
                                            qllEnv            * aEnv );

stlStatus qlnxExecuteReturnExprBlock( qllDataArea       * aDataArea,
                                      stlBool             aIsReturnQuery,
                                      knlValueBlockList * aReturnBlockList,
                                      stlInt64            aBlockIdx,
                                      stlInt32            aReturnExprCnt,
                                      knlExprStack      * aReturnExprStack,
                                      qllEnv            * aEnv );


/*********************************************************
 * Common
 ********************************************************/

stlStatus qlxEvaluateConstExpr( knlExprStack        * aExprStack,
                                qllDataArea         * aDataArea,
                                knlEnv              * aEnv );


/** @} qlnx */

#endif /* _QLNXODE_H_ */
