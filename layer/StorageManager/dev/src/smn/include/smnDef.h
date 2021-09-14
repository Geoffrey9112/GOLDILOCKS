/*******************************************************************************
 * smnDef.h
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


#ifndef _SMNDEF_H_
#define _SMNDEF_H_ 1

/**
 * @file smnDef.h
 * @brief Storage Manager Layer Index Internal Definition
 */

/**
 * @defgroup smnInternal Component Internal Routines
 * @ingroup smn
 * @{
 */

/**
 * @addtogroup smrUndoLogType
 * @{
 */
#define SMN_UNDO_LOG_MEMORY_BTREE_INSERT_LEAF       0  /**< Memory B-Tree Index Insert key undo */
#define SMN_UNDO_LOG_MEMORY_BTREE_DUPKEY            1  /**< Memory B-Tree Index duplicated key insert undo */
#define SMN_UNDO_LOG_MEMORY_BTREE_DELETE_LEAF       2  /**< Memory B-Tree Index Delete Key undo */
#define SMN_UNDO_LOG_MEMORY_BTREE_CREATE            3  /**< Memory B-Tree Index Create undo */
#define SMN_UNDO_LOG_MEMORY_BTREE_DROP              4  /**< Memory B-Tree Index Drop undo */
#define SMN_UNDO_LOG_MEMORY_BTREE_BLOCK_INSERT      5  /**< Memory B-Tree Key block insertion */
#define SMN_UNDO_LOG_MEMORY_BTREE_BLOCK_DELETE      6  /**< Memory B-Tree Key block deletion */
#define SMN_UNDO_LOG_ALTER_DATATYPE                 7  /**< Index Alter Datatype */
#define SMD_UNDO_LOG_MEMORY_BTREE_ALTER_ATTR        8  /**< Index Alter Attribute */
/** @} */

#define SMN_UNDO_LOG_MAX_COUNT                (9)

typedef struct smnIndexHeader
{
    smeRelationHeader   mRelHeader;
    stlUInt16           mLoggingFlag;     /**< 0 : no logging,  1 : logging */
    stlInt16            mIniTrans;        /**< 초기 RTS 개수 */
    stlInt16            mMaxTrans;        /**< 최대 RTS 개수 */
    stlBool             mUniqueness;      /**< Uniqueness Check Flag */
    stlBool             mPrimary;         /**< Primary Check Flag */
    stlInt16            mPctFree;         /**< BOTTOM-UP build시 node의 여유공간 */
    stlChar             mPadding1[2];
    smlPid              mMirrorRootPid;   /**< Index의 mirrored root Page ID */
    smlRid              mBaseTableSegRid; /**< Base Table의 Segment Rid */
    smlPid              mRootPageId;      /**< Index의 root Page ID */

    stlUInt16           mKeyColCount;
    stlBool             m1ByteFastBuild;
    stlChar             mPadding2[1];
    stlUInt8            mKeyColTypes[SML_MAX_INDEX_KEY_COL_COUNT];
    stlUInt8            mKeyColFlags[SML_MAX_INDEX_KEY_COL_COUNT];
    stlUInt32           mKeyColOrder[SML_MAX_INDEX_KEY_COL_COUNT];
    stlUInt64           mKeyColSize[SML_MAX_INDEX_KEY_COL_COUNT];

    /* empty node list : 모든 key가 deleted 상태인 leaf node들의 list
                         모든 key가 commit되어 dead key 상태가 되면 free 된다 */
    stlUInt32           mEmptyNodeCount;
    smlPid              mFirstEmptyNode;
    smlPid              mLastEmptyNode;
    /* volatile infos */
    stlBool             mIsAgingEmptyNode;
    stlChar             mPadding3[3];
    stlUInt64           mIndexSmoNo;
    void              * mBaseRelHandle;  /**< Volatil 정보 */
    void              * mModuleSpecificData;
} smnIndexHeader;


#define SMN_GET_LOGGING_FLAG(relHandle)    (((smnIndexHeader*)relHandle)->mLoggingFlag)
#define SMN_GET_INITRANS(relHandle)        (((smnIndexHeader*)relHandle)->mIniTrans)
#define SMN_GET_MAXTRANS(relHandle)        (((smnIndexHeader*)relHandle)->mMaxTrans)
#define SMN_GET_UNIQUENESS(relHandle)      (((smnIndexHeader*)relHandle)->mUniqueness)
#define SMN_GET_BASE_TABLE_SEGMENT_RID(relHandle)       (((smnIndexHeader*)relHandle)->mBaseTableSegRid)
#define SMN_GET_ROOT_PAGE_ID(relHandle)    (((smnIndexHeader*)relHandle)->mRootPageId)
#define SMN_GET_INDEX_SMO_NO(relHandle)    (((smnIndexHeader*)relHandle)->mIndexSmoNo)
#define SMN_GET_EMPTY_NODE_COUNT(relHandle)    (((smnIndexHeader*)relHandle)->mEmptyNodeCount)
#define SMN_GET_FIRST_EMPTY_NODE(relHandle)    (((smnIndexHeader*)relHandle)->mFirstEmptyNode)
#define SMN_GET_LAST_EMPTY_NODE(relHandle)     (((smnIndexHeader*)relHandle)->mLastEmptyNode)
#define SMN_GET_IS_AGING_EMPTY_NODE(relHandle) (((smnIndexHeader*)relHandle)->mIsAgingEmptyNode)
#define SMN_GET_BASE_REL_HANDLE(relHandle) (((smnIndexHeader*)relHandle)->mBaseRelHandle)
#define SMN_SET_BASE_REL_HANDLE(relHandle, aBaseRelHandle) (((smnIndexHeader*)relHandle)->mBaseRelHandle = aBaseRelHandle)
#define SMN_SET_MODULE_SPECIFIC_DATA(relHandle, aData) (((smnIndexHeader*)relHandle)->mModuleSpecificData = aData)

#define SMN_GET_NEXT_INDEX_SMO_NO(relHandle) (stlAtomicAdd64(&((smnIndexHeader*)relHandle)->mIndexSmoNo, 1) + 1)

#define SMN_GET_PAGE_SMO_NO(aPageHandle)     (*(stlUInt64*)smpGetVolatileArea(aPageHandle))
#define SMN_SET_PAGE_SMO_NO(aPageHandle,aSmoNo)                              \
{                                                                            \
    STL_ASSERT( *(stlUInt64*)smpGetVolatileArea(aPageHandle) <= aSmoNo );    \
    *(stlUInt64*)smpGetVolatileArea(aPageHandle) = aSmoNo;                   \
}

#define SMN_SET_ROOT_PAGE_ID(relHandle, aRootPid) (((smnIndexHeader*)relHandle)->mRootPageId = aRootPid)
#define SMN_SET_MIRROR_ROOT_PID(relHandle, aRootPid) (((smnIndexHeader*)relHandle)->mMirrorRootPid = aRootPid)
#define SMN_GET_MIRROR_ROOT_PID(relHandle) (((smnIndexHeader*)relHandle)->mMirrorRootPid)

#define SMN_GET_KEY_COL_SIZE( aRelHandle, aKeyOrder ) ( ((smnIndexHeader*)(aRelHandle))->mKeyColSize[(aKeyOrder)] )

/** TODO : argument들은 아직 정의되지 않았음 */
typedef stlStatus (*smnCreateFunc)( smlStatement      * aStatement,
                                    smlTbsId            aTbsId,
                                    smlIndexAttr      * aAttr,
                                    void              * aBaseTableHandle,
                                    stlUInt16           aKeyColCount,
                                    knlValueBlockList * aIndexColList,
                                    stlUInt8          * aKeyColFlags,
                                    stlInt64          * aSegmentId,
                                    void             ** aRelationHandle,
                                    smlEnv            * aEnv);
typedef stlStatus (*smnCreateUndoFunc)( smxlTransId   aTransId,
                                        void        * aRelationHandle,
                                        smlEnv      * aEnv );
typedef stlStatus (*smnDropFunc)( smlStatement * aStatement,
                                  void         * aRelationHandle,
                                  smlEnv       * aEnv );
typedef stlStatus (*smnAlterDataTypeFunc)( smlStatement  * aStatement,
                                           void          * aRelationHandle,
                                           stlInt32        aColumnOrder,
                                           dtlDataType     aDataType,
                                           stlInt64        aColumnSize,
                                           smlEnv        * aEnv );
typedef stlStatus (*smnAlterIndexAttrFunc)( smlStatement * aStatement,
                                            void         * aRelationHandle,
                                            smlIndexAttr * aIndexAttr,
                                            smlEnv       * aEnv );
typedef stlStatus (*smnCreateForTruncateFunc)( smlStatement          * aStatement,
                                               void                  * aOrgRelHandle,
                                               void                  * aNewBaseRelHandle,
                                               smlDropStorageOption    aDropStorageOption,
                                               void                 ** aNewRelHandle,
                                               stlInt64              * aNewRelationId,
                                               smlEnv                * aEnv );
typedef stlStatus (*smnDropAgingFunc)( smxlTransId   aTransId,
                                       void        * aRelHandle,
                                       stlBool       aOnStartup,
                                       smlEnv      * aEnv );
typedef stlStatus (*smnInitializeFunc)( smlStatement * aStatement,
                                        void         * aRelationHandle,
                                        smlEnv       * aEnv );
typedef stlStatus (*smnFinalizeFunc)( smlStatement * aStatement,
                                      void         * aRelationHandle,
                                      smlEnv       * aEnv );
typedef stlStatus (*smnBuildFunc)( smlStatement      * aStatement,
                                   void              * aIndexHandle,
                                   void              * aBaseTableHandle,
                                   stlUInt16           aKeyColCount,
                                   knlValueBlockList * aIndexColList,
                                   stlUInt8          * aKeyColFlags,
                                   stlUInt16           aParallelFactor,
                                   smlEnv            * aEnv );
typedef stlStatus (*smnBuildUnusableFunc)( smlStatement      * aStatement,
                                           void              * aIndexHandle,
                                           smlEnv            * aEnv );
typedef stlStatus (*smnBuildForTruncateFunc)( smlStatement * aStatement,
                                              void         * aOrgIndexHandle,
                                              void         * aNewIndexHandle,
                                              smlEnv       * aEnv );
typedef stlStatus (*smnInsertFunc)( smlStatement            * aStatement,
                                    void                    * aRelationHandle,
                                    knlValueBlockList       * aColumns,
                                    stlInt32                  aValueIdx,
                                    smlRid                  * aRowRid,
                                    smlRid                  * aUndoRid,
                                    stlBool                   aIsIndexBuilding,
                                    stlBool                   aIsDeferred,
                                    smlUniqueViolationScope * aUniqueViolationScope,
                                    smlEnv                  * aEnv );
typedef stlStatus (*smnDeleteFunc)( smlStatement            * aStatement,
                                    void                    * aRelationHandle,
                                    knlValueBlockList       * aColumns,
                                    stlInt32                  aValueIdx,
                                    smlRid                  * aRowRid,
                                    smlRid                  * aUndoRid,
                                    smlUniqueViolationScope * aUniqueViolationScope,
                                    smlEnv                  * aEnv );
typedef stlStatus (*smnBlockInsertFunc)( smlStatement            * aStatement,
                                         void                    * aRelationHandle,
                                         knlValueBlockList       * aColumns,
                                         smlRowBlock             * aRowBlock,
                                         stlBool                 * aBlockFilter,
                                         stlBool                   aIsDeferred,
                                         stlInt32                * aViolationCnt,
                                         smlUniqueViolationScope * aUniqueViolationScope,
                                         smlEnv                  * aEnv );
typedef stlStatus (*smnBlockDeleteFunc)( smlStatement            * aStatement,
                                         void                    * aRelationHandle,
                                         knlValueBlockList       * aColumns,
                                         smlRowBlock             * aRowBlock,
                                         stlBool                 * aBlockFilter,
                                         stlInt32                * aViolationCnt,
                                         smlUniqueViolationScope * aUniqueViolationScope,
                                         smlEnv                  * aEnv );
typedef void * (*smnGetSlotBodyFunc)( void * aSlot );
typedef stlStatus (*smnInitModuleSpecificData)( void   * aRelationHandle,
                                                smlEnv * aEnv );
typedef stlStatus (*smnDestModuleSpecificData)( void   * aRelationHandle,
                                                smlEnv * aEnv );
typedef stlStatus (*smnTruncateFunc)( smlStatement * aStatement,
                                      void         * aRelationHandle,
                                      smlEnv       * aEnv );


typedef struct smnIndexModule
{
    smnCreateFunc             mCreate;
    smnCreateUndoFunc         mCreateUndo;
    smnDropFunc               mDrop;
    smnAlterDataTypeFunc      mAlterDataType;
    smnAlterIndexAttrFunc     mAlterIndexAttr;
    smnCreateForTruncateFunc  mCreateForTruncate;
    smnDropAgingFunc          mDropAging;
    smnInitializeFunc         mInitialize;
    smnFinalizeFunc           mFinalize;
    smnBuildFunc              mBuild;
    smnBuildUnusableFunc      mBuildUnusable;
    smnBuildForTruncateFunc   mBuildForTruncate;
    smnInsertFunc             mInsert;
    smnDeleteFunc             mDelete;
    smnBlockInsertFunc        mBlockInsert;
    smnBlockDeleteFunc        mBlockDelete;
    smnGetSlotBodyFunc        mGetSlotBody;
    smnInitModuleSpecificData mInitModuleSpecificData;
    smnDestModuleSpecificData mDestModuleSpecificData;
    smnTruncateFunc           mTruncate;
} smnIndexModule;



typedef struct smnFxIndexHdrRec
{
    smlTbsId            mTbsId;
    stlInt64            mPhysicalId;
    stlBool             mLoggingFlag;        /**< FALSE : no logging,  TRUE : logging */
    stlInt16            mIniTrans;           /**< 초기 RTS 개수 */
    stlInt16            mMaxTrans;           /**< 최대 RTS 개수 */
    stlInt16            mPctFree;            /**< 여부공간의 백분율 */
    stlBool             mUniqueness;         /**< FALSE : Non-Unique,  TRUE : Unique. */
    stlBool             mPrimary;            /**< FALSE : Non-Primary, TRUE : Primary. */
    stlInt64            mBaseTableSegRid;    /**< Base Table의 Segment Rid */
    smlPid              mRootPageId;         /**< Index의 root Page ID */
    smlPid              mMirrorRootPid;      /**< Index의 mirrored root Page ID */
    stlBool             mIsUsable;
    
    stlUInt32           mEmptyNodeCount;
    smlPid              mFirstEmptyNode;
    smlPid              mLastEmptyNode;
    stlUInt32           mFreeNodeCount;
    smlPid              mFirstFreeNode;
    smlPid              mLastFreeNode;

    stlInt16            mKeyColCount;
    stlChar             mKeyColTypes[SMP_PAGE_SIZE/4];
    stlChar             mKeyColFlags[SMP_PAGE_SIZE/4];
    stlChar             mKeyColOrder[SMP_PAGE_SIZE/4];
    stlChar             mKeyColSize[SMP_PAGE_SIZE/4];
    
    stlUInt64           mIndexSmoNo;
    stlBool             mIsAgingEmptyNode;
    stlUInt64           mBaseRelHandle;  /**< Volatil 정보 */
} smnFxIndexHdrRec;

typedef struct smnFxIndexHdrPathCtrl
{
    void    * mSegIterator;
    stlBool   mFetchStarted;
} smnFxIndexHdrPathCtrl;


typedef struct smnFxIndexCacheRec
{
    smlTbsId            mTbsId;
    stlInt64            mPhysicalId;
    stlBool             mLoggingFlag;        /**< FALSE : no logging,  TRUE : logging */
    stlInt16            mIniTrans;           /**< 초기 RTS 개수 */
    stlInt16            mMaxTrans;           /**< 최대 RTS 개수 */
    stlBool             mUniqueness;         /**< FALSE : Non-Unique,  TRUE : Unique. */
    stlBool             mPrimary;            /**< FALSE : Non-Primary, TRUE : Primary. */
    stlInt64            mBaseTableSegRid;    /**< Base Table의 Segment Rid */
    smlPid              mRootPageId;         /**< Index의 root Page ID */
    
    stlUInt32           mEmptyNodeCount;
    smlPid              mFirstEmptyNode;
    smlPid              mLastEmptyNode;
    stlUInt32           mFreeNodeCount;
    smlPid              mFirstFreeNode;
    smlPid              mLastFreeNode;

    stlInt16            mKeyColCount;
    stlChar             mKeyColTypes[SMP_PAGE_SIZE/4];
    stlChar             mKeyColFlags[SMP_PAGE_SIZE/4];
    stlChar             mKeyColOrder[SMP_PAGE_SIZE/4];
    stlChar             mKeyColSize[SMP_PAGE_SIZE/4];
    
    stlUInt64           mIndexSmoNo;
    stlBool             mIsAgingEmptyNode;
    stlUInt64           mBaseRelHandle;  /**< Volatil 정보 */
} smnFxIndexCacheRec;

typedef struct smnFxIndexCachePathCtrl
{
    stlBool   mFetchStarted;
    void    * mSegIterator;
} smnFxIndexCachePathCtrl;

#define SMN_BUILD_DIRECTION_TOP_DOWN          (0)    /**< top-down build */
#define SMN_BUILD_DIRECTION_BOTTOM_UP         (1)    /**< bottom-up build */
#define SMN_MAX_MERGED_RUN_LIST               (64)   /**< maximum merged run list */
#define SMN_INDEX_BUILD_PARALLEL_THRESHOLD    (1024) /**< bottom-up build parallel threshold */
/** @} */
    
/**
 * @brief SMN_TRY_INTERNAL_ERROR 매크로
 * @remarks aParams 전체는 반드시 ()안에 들어 있어야 한다. <BR>
 * @par example
 * SML_ASSERT( aLsn < 10, aExpression, (aEnv, "LSN(%d)\n", aLsn) );
 */
#define SMN_TRY_INTERNAL_ERROR( aExpression, aEnv, aParams )    \
    do                                                          \
    {                                                           \
        if( !(aExpression) )                                    \
        {                                                       \
            knlLogMsg( NULL,                                    \
                       KNL_ENV(aEnv),                           \
                       KNL_LOG_LEVEL_FATAL,                     \
                       "ASSERT(%s)\n"                           \
                       "failed in file %s line %d\n",           \
                       #aExpression,                            \
                       __FILE__,                                \
                       __LINE__ );                              \
                                                                \
            STL_TRY( smnInternalError aParams == STL_SUCCESS ); \
        }                                                       \
    } while( 0 )

#define SMNMPB_RETRY_NONE            ( 1 )
#define SMNMPB_RETRY_CONCURRENCY     ( 2 )
#define SMNMPB_RETRY_NO_SPACE        ( 3 )

#endif /* _SMNDEF_H_ */
