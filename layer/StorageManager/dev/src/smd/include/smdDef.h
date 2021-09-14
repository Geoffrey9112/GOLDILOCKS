/*******************************************************************************
 * smdDef.h
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


#ifndef _SMDDEF_H_
#define _SMDDEF_H_ 1

/**
 * @file smdDef.h
 * @brief Storage Manager Layer Table Internal Definition
 */

/**
 * @defgroup smdInternal Component Internal Routines
 * @ingroup smd
 * @{
 */

/**
 * @addtogroup smrUndoLogType
 * @{
 */

#include <smdmisTable.h>

#define SMD_UNDO_LOG_MEMORY_HEAP_INSERT           0  /**< Memory Heap Table Insert Row undo */
#define SMD_UNDO_LOG_MEMORY_HEAP_UPDATE           1  /**< Memory Heap Table Update Row undo */
#define SMD_UNDO_LOG_MEMORY_HEAP_MIGRATE          2  /**< Memory Heap Table Migrate Row undo */
#define SMD_UNDO_LOG_MEMORY_HEAP_UPDATE_LINK      3  /**< Memory Heap Table update next piece link undo */
#define SMD_UNDO_LOG_MEMORY_HEAP_DELETE           4  /**< Memory Heap Table Delete Row undo */
#define SMD_UNDO_LOG_MEMORY_HEAP_CREATE           5  /**< Memory Heap Table Create undo */
#define SMD_UNDO_LOG_MEMORY_HEAP_DROP             6  /**< Memory Heap Table Drop undo */
#define SMD_UNDO_LOG_MEMORY_HEAP_APPEND           7  /**< Memory Heap Table Append undo */
#define SMD_UNDO_LOG_MEMORY_HEAP_ALTER_ATTR       8  /**< Memory Heap Table Alter Attribute undo */
#define SMD_UNDO_LOG_MEMORY_PENDING_CREATE        9  /**< Memory Pending Table Create undo */
#define SMD_UNDO_LOG_MEMORY_PENDING_DROP         10  /**< Memory Pending Table Drop undo */
#define SMD_UNDO_LOG_MEMORY_HEAP_BLOCK_INSERT    11  /**< Memory Heap Table Insert Block Row undo */
#define SMD_UNDO_LOG_MEMORY_PENDING_INSERT       12  /**< Memory Pending Table Insert undo */
#define SMD_UNDO_LOG_ALLOC_COLUMN_HDR_PAGE       13  /**< Alloc Column Hdr Page Undo */
/** @} */

#define SMD_UNDO_LOG_MAX_COUNT                14

typedef struct smdTablePendArg
{
    void        * mHandle;
    smxlTransId   mTransId;
} smdTablePendArg;

typedef struct smdTableHeader
{
    smeRelationHeader   mRelHeader;
    stlUInt16           mLoggingFlag;     /**< 0 : no logging,  1 : logging */
    stlUInt16           mLockingFlag;     /**< 0 : no locking,  1 : locking */
    stlInt16            mIniTrans;        /**< 최대 RTS 개수 */
    stlInt16            mMaxTrans;        /**< 최대 RTS 개수 */
    stlInt16            mPctFree;         /**< INSERTABLE -> UPDATE_ONLY 로 변경되는 free 영역의 percent */
    stlInt16            mPctUsed;         /**< UPDATE_ONLY -> INSERTABLE로 변경되는 최소 사용공간의 percent */
    stlInt16            mRowCount;        /**< Columnar Table에서 하나의 page에 저장할 row 수 */
    stlInt16            mContColumnThreshold; /**< Table 생성 시 설정된 continuous column threshold */
    stlInt32            mMemberPageCount; /**< Columnar Table에서 row를 저장하기 위해 필요한 data page 수 */
    stlInt32            mColCount;        /* Total Column Count */
    stlUInt64           mLockHandle;

    stlInt16          * mColLengthArray;  /**< Columnar Table에서 모든 column의 max length array */
    stlInt32          * mLastColIdArray;  /**< Columnar Table의 각 member page에 저장되는 마지막 column Id array */
} smdTableHeader;

#define SMD_GET_LOGGING_FLAG(relHandle)          (((smdTableHeader*)relHandle)->mLoggingFlag)
#define SMD_GET_LOCKING_FLAG(relHandle)          (((smdTableHeader*)relHandle)->mLockingFlag)
#define SMD_GET_INITRANS(relHandle)              (((smdTableHeader*)relHandle)->mIniTrans)
#define SMD_GET_MAXTRANS(relHandle)              (((smdTableHeader*)relHandle)->mMaxTrans)
#define SMD_GET_PCTFREE(relHandle)               (((smdTableHeader*)relHandle)->mPctFree)
#define SMD_GET_PCTUSED(relHandle)               (((smdTableHeader*)relHandle)->mPctUsed)
#define SMD_GET_LOCK_HANDLE(relHandle)           ((void*)(((smdTableHeader*)relHandle)->mLockHandle))
#define SMD_SET_LOCK_HANDLE(relHandle,lockHandle)                           \
{                                                                           \
    ((smdTableHeader*)relHandle)->mLockHandle = (stlUInt64)(lockHandle);    \
}

#define SMD_GET_COL_COUNT(relHandle)             (((smdTableHeader*)relHandle)->mColCount)
#define SMD_SET_COL_LENGTH_ARRAY(relHandle, colLenArray)                \
    (((smdTableHeader*)relHandle)->mColLengthArray = (colLenArray))
#define SMD_GET_COL_LENGTH_ARRAY(relHandle)             \
    (((smdTableHeader*)relHandle)->mColLengthArray)
#define SMD_GET_NTH_COL_LENGTH(relHandle, n)                            \
    (((((smdTableHeader*)relHandle)->mColLengthArray) == NULL)? 0:      \
     ((smdTableHeader*)relHandle)->mColLengthArray[(n)])
#define SMD_GET_MEMBER_PAGE_COUNT(relHandle)            \
    (((smdTableHeader*)relHandle)->mMemberPageCount)
#define SMD_SET_LAST_COL_ID_ARRAY(relHandle, lastColIdArray)            \
    (((smdTableHeader*)relHandle)->mLastColIdArray = (lastColIdArray))
#define SMD_GET_LAST_COL_ID_ARRAY(relHandle)            \
    (((smdTableHeader*)relHandle)->mLastColIdArray)
#define SMD_GET_ROW_COUNT(relHandle)            \
    (((smdTableHeader*)relHandle)->mRowCount)
#define SMD_SET_CONT_COLUMN_THRESHOLD(relHandle, threshold)             \
    (((smdTableHeader*)relHandle)->mContColumnThreshold = (threshold))
#define SMD_GET_CONT_COLUMN_THRESHOLD(relHandle)                \
    (((smdTableHeader*)relHandle)->mContColumnThreshold)

#define SMD_IS_VOLATILE_SORT_TABLE(aHandle)     SMDMIS_TABLE_IS_VOLATILE((smdmisTableHeader *)(aHandle))

    
typedef stlStatus (*smdCreateFunc)( smlStatement  * aStatement,
                                    smlTbsId        aTbsId,
                                    smlTableAttr  * aAttr,
                                    stlBool         aLoggingUndo,
                                    stlInt64      * aSegmentId,
                                    void         ** aRelationHandle,
                                    smlEnv        * aEnv);
typedef stlStatus (*smdCreateIotFunc)( smlStatement      * aStatement,
                                       smlTbsId            aTbsId,
                                       smlTableAttr      * aTableAttr,
                                       smlIndexAttr      * aIndexAttr,
                                       knlValueBlockList * aRowColList,
                                       stlUInt16           aKeyColCount,
                                       stlUInt8          * aKeyColFlags,
                                       stlInt64          * aSegmentId,
                                       void             ** aRelationHandle,
                                       smlEnv            * aEnv );
typedef stlStatus (*smdCreateUndoFunc)( smxlTransId   aTransId,
                                        void        * aRelationHandle,
                                        smlEnv      * aEnv );
typedef stlStatus (*smdDropFunc)( smlStatement * aStatement,
                                  void         * aRelationHandle,
                                  smlEnv       * aEnv );
typedef stlStatus (*smdCopyFunc)( smlStatement      * aStatement,
                                  void              * aSrcRelHandle,
                                  void              * aDstRelHandle,
                                  knlValueBlockList * aColumnList,
                                  smlEnv            * aEnv );
typedef stlStatus (*smdAddColumnsFunc)( smlStatement      * aStatement,
                                        void              * aRelationHandle,
                                        knlValueBlockList * aAddColumns,
                                        smlRowBlock       * aRowBlock,
                                        smlEnv            * aEnv );
typedef stlStatus (*smdDropColumnsFunc)( smlStatement      * aStatement,
                                         void              * aRelationHandle,
                                         knlValueBlockList * aDropColumns,
                                         smlEnv            * aEnv );
typedef stlStatus (*smdAlterTableAttrFunc)( smlStatement  * aStatement,
                                            void          * aRelationHandle,
                                            smlTableAttr  * aTableAttr,
                                            smlIndexAttr  * aIndexAttr,
                                            smlEnv        * aEnv );
typedef stlStatus (*smdMergeTableAttrFunc)( void          * aRelationHandle,
                                            smlTableAttr  * aTableAttr,
                                            smlEnv        * aEnv );
typedef stlStatus (*smdCheckExistRowFunc)( smlStatement * aStatement,
                                           void         * aRelationHandle,
                                           stlBool      * aExistRow,
                                           smlEnv       * aEnv );
typedef stlStatus (*smdCreateForTruncateFunc)( smlStatement          * aStatement,
                                               void                  * aOrgRelHandle,
                                               smlDropStorageOption    aDropStorageOption,
                                               void                 ** aNewRelHandle,
                                               stlInt64              * aNewSegmentId,
                                               smlEnv                * aEnv );
typedef stlStatus (*smdDropAgingFunc)( smxlTransId   aTransId,
                                       void        * aRelationHandle,
                                       stlBool       aOnStartup,
                                       smlEnv      * aEnv );
typedef stlStatus (*smdTruncateAgingFunc)( smxlTransId   aTransId,
                                           smlRid        aSegmentRid,
                                           smlEnv      * aEnv );
typedef stlStatus (*smdInitializeFunc)( smlStatement * aStatement,
                                        void         * aRelationHandle,
                                        smlEnv       * aEnv );
typedef stlStatus (*smdFinalizeFunc)( smlStatement * aStatement,
                                      void         * aRelationHandle,
                                      smlEnv       * aEnv );
typedef stlStatus (*smdInsertFunc)( smlStatement      * aStatement,
                                    void              * aRelationHandle,
                                    stlInt32            aValueIdx,
                                    knlValueBlockList * aInsertCols,
                                    knlValueBlockList * aUniqueViolation,
                                    smlRid            * aRowRid,
                                    smlEnv            * aEnv );
typedef stlStatus (*smdBlockInsertFunc)( smlStatement      * aStatement,
                                         void              * aRelationHandle,
                                         knlValueBlockList * aInsertCols,
                                         knlValueBlockList * aUniqueViolation,
                                         smlRowBlock       * aRowBlock,
                                         smlEnv            * aEnv );
typedef stlStatus (*smdUpdateFunc)( smlStatement      * aStatement,
                                    void              * aRelationHandle,
                                    smlRid            * aRowRid,
                                    smlScn              aRowScn,
                                    stlInt32            aValueIdx,
                                    knlValueBlockList * aAfterCols,
                                    knlValueBlockList * aBeforeCols,
                                    knlValueBlockList * aPrimaryKey,
                                    stlBool           * aVerionConflict,
                                    stlBool           * aSkipped,
                                    smlEnv            * aEnv );
typedef stlStatus (*smdDeleteFunc)( smlStatement      * aStatement,
                                    void              * aRelationHandle,
                                    smlRid            * aRowRid,
                                    smlScn              aRowScn,
                                    stlInt32            aValueIdx,
                                    knlValueBlockList * aPrimaryKey,
                                    stlBool           * aVerionConflict,
                                    stlBool           * aSkipped,
                                    smlEnv            * aEnv );
typedef stlStatus (*smdMergeFunc)( smlStatement       * aStatement,
                                   void               * aRelationHandle,
                                   knlValueBlockList  * aInsertCols,
                                   smlMergeRecordInfo * aMergeReocordInfo,
                                   smlEnv             * aEnv );
typedef stlStatus (*smdRowCountFunc)( void        * aRelationHandle,
                                      stlInt64    * aRowCount,
                                      smlEnv      * aEnv );
typedef stlStatus (*smdFetchFunc)( smlStatement        * aStatement,
                                   smlFetchRecordInfo  * aFetchRecordInfo,
                                   smlRid              * aRowRid,
                                   stlInt32              aBlockIdx,
                                   stlBool             * aIsMatched,
                                   stlBool             * aDeleted,
                                   stlBool             * aUpdated,
                                   smlEnv              * aEnv );
typedef stlStatus (*smdFetchWithRowidFunc)( smlStatement        * aStatement,
                                            smlFetchRecordInfo  * aFetchRecordInfo,
                                            stlInt64              aSegmentId,
                                            stlInt64              aSegmentSeq,
                                            smlRid              * aRowRid,
                                            stlInt32              aBlockIdx,
                                            stlBool             * aIsMatched,
                                            stlBool             * aDeleted,
                                            smlEnv              * aEnv );
typedef stlStatus (*smdLockRowFunc)( smlStatement * aStatement,
                                     void         * aRelationHandle, 
                                     smlRid       * aRowRid,
                                     smlScn         aRowScn,
                                     stlUInt16      aLockMode,
                                     stlInt64       aTimeInterval,
                                     stlBool      * aVerionConflict,
                                     smlEnv       * aEnv );
typedef stlStatus (*smdCompareKeyValueFunc)( smlRid              * aRowRid,
                                             smpHandle           * aTablePageHandle,
                                             stlBool             * aTablePageLatchRelease,
                                             smxlTransId           aMyTransId,
                                             smlScn                aMyViewScn,
                                             smlScn                aMyCommitScn,
                                             smlTcn                aMyTcn,
                                             knlKeyCompareList   * aKeyCompList,
                                             stlBool             * aIsMatch,
                                             smlScn              * aCommitScn,
                                             smlEnv              * aEnv );
typedef stlStatus (*smdExtractKeyValueFunc)( smpHandle           * aPageHandle,
                                             stlBool               aFetchFirst,
                                             smpRowItemIterator  * aRowIterator,
                                             smdValueBlockList   * aColumnList,
                                             stlInt32            * aKeyValueSize,
                                             stlInt16            * aRowSeq,
                                             stlBool             * aHasNullInBlock,
                                             stlBool             * aIsSuccess,
                                             smlEnv              * aEnv );
typedef stlStatus (*smdExtractValidKeyValueFunc)( void              * aRelationHandle,
                                                  smxlTransId         aTransId,
                                                  smlScn              aViewScn,
                                                  smlTcn              aTcn,
                                                  smlRid            * aRowRid,
                                                  knlValueBlockList * aColumnList,
                                                  smlEnv            * aEnv );
typedef stlStatus (*smdSortFunc)( void * RelationHandle, void * aTransactionHandle );
typedef void * (*smdGetSlotBodyFunc)( void * aSlot );
typedef stlStatus (*smdTruncateFunc)( smlStatement * aStatement,
                                      void         * aRelationHandle,
                                      smlEnv       * aEnv );
typedef stlStatus (*smdSetFlagFunc)( smlStatement      *aStatement,
                                     void              *aRelationHandle,
                                     smlRowBlock       *aRids,
                                     knlValueBlockList *aFlags,
                                     smlEnv            *aEnv );
typedef stlStatus (*smdCleanupFunc)( void    *aRelationHandle,
                                     smlEnv  *aEnv );
typedef stlStatus (*smdBuildCacheFunc)( void    * aRelationHandle,
                                        void    * aColumnMeta,
                                        smlEnv  * aEnv );

typedef stlStatus (*smdFetchRecord4IndexFunc)( smiIterator         * aIterator,
                                               void                * aBaseRelHandle,
                                               smlFetchRecordInfo  * aFetchRecordInfo,
                                               smpHandle           * aTablePageHandle,
                                               stlBool             * aTablePageLatchReleased,
                                               smlRid              * aRowRid,
                                               stlInt32              aBlockIdx,
                                               smlScn                aAgableViewScn,
                                               stlBool             * aIsMatched,
                                               smlEnv              * aEnv );

/** TODO : insertOrUpdate interface */

typedef struct smdTableModule
{
    smdCreateFunc                  mCreate;
    smdCreateIotFunc               mCreateIot;
    smdCreateUndoFunc              mCreateUndo;
    smdDropFunc                    mDrop;
    smdCopyFunc                    mCopy;
    smdAddColumnsFunc              mAddColumns;
    smdDropColumnsFunc             mDropColumns;
    smdAlterTableAttrFunc          mAlterTableAttr;
    smdMergeTableAttrFunc          mMergeTableAttr;
    smdCheckExistRowFunc           mCheckExistRow;
    smdCreateForTruncateFunc       mCreateForTruncate;
    smdDropAgingFunc               mDropAging;
    smdInitializeFunc              mInitialize;
    smdFinalizeFunc                mFinalize;
    smdInsertFunc                  mInsert;
    smdBlockInsertFunc             mBlockInsert;
    smdUpdateFunc                  mUpdate;
    smdDeleteFunc                  mDelete;
    smdMergeFunc                   mMerge;
    smdRowCountFunc                mRowCount;
    smdFetchFunc                   mFetch;
    smdFetchWithRowidFunc          mFetchWithRowid;
    smdLockRowFunc                 mLockRow;
    smdCompareKeyValueFunc         mCompareKeyValue;
    smdExtractKeyValueFunc         mExtractKeyValue;
    smdExtractValidKeyValueFunc    mExtractValidKeyValue;
    smdSortFunc                    mSort;
    smdGetSlotBodyFunc             mGetSlotBody;
    smdTruncateFunc                mTruncate;
    smdSetFlagFunc                 mSetFlag;
    smdCleanupFunc                 mCleanup;
    smdBuildCacheFunc              mBuildCache;
    smdFetchRecord4IndexFunc       mFetch4Index;
} smdTableModule;

typedef struct smdTableRedoModule
{
    smrRedoFunc       mRedoInsert;
    smrRedoFunc       mRedoUninsert;
    smrRedoFunc       mRedoUpdate;
    smrRedoFunc       mRedoUnupdate;
    smrRedoFunc       mRedoMigrate;
    smrRedoFunc       mRedoUnmigrate;
    smrRedoFunc       mRedoDelete;
    smrRedoFunc       mRedoUndelete;
    smrRedoFunc       mRedoUpdateLink;
    smrRedoFunc       mRedoInsertForUpdate;
    smrRedoFunc       mRedoDeleteForUpdate;
    smrRedoFunc       mRedoUpdateLogicalHdr;
    smrRedoFunc       mRedoAppend;
    smrRedoFunc       mRedoUnappend;
    smrRedoFunc       mRedoPendingInsert;
    smrRedoFunc       mRedoCompaction;
} smdTableRedoModule;

typedef stlStatus (*smdUndoFunc)( smxmTrans * aMiniTrans,
                                  smlRid      aTargetRid,
                                  void      * aLogBody,
                                  stlInt16    aLogSize,
                                  smpHandle * aPageHandle,
                                  smlEnv    * aEnv );

typedef struct smdTableUndoModule
{
    smdUndoFunc      mUndoInsert;
    smdUndoFunc      mUndoUpdate;
    smdUndoFunc      mUndoMigrate;
    smdUndoFunc      mUndoUpdateLink;
    smdUndoFunc      mUndoDelete;
    smdUndoFunc      mUndoCreate;
    smdUndoFunc      mUndoDrop;
    smdUndoFunc      mUndoAppend;
    smdUndoFunc      mUndoAlterAttr;
    smdUndoFunc      mUndoPendingCreate;
    smdUndoFunc      mUndoPendingDrop;
    smdUndoFunc      mUndoBlockInsert;
    smdUndoFunc      mUndoPendingInsert;
} smdTableUndoModule;

typedef struct smdTableMtxUndoModule
{
    smrRedoFunc       mUndoInsert;
} smdTableMtxUndoModule;

#define SMD_TABLE_TYPE_SIZE  ( 16 )

typedef struct smdFxTableCacheRec
{
    smlTbsId      mTbsId;
    stlInt64      mPhysicalId;
    stlChar       mTableType[SMD_TABLE_TYPE_SIZE];
    stlBool       mLoggingFlag;
    stlBool       mLockingFlag;
    stlInt16      mIniTrans;
    stlInt16      mMaxTrans;
    stlInt16      mPctFree;
    stlInt16      mPctUsed;
} smdFxTableCacheRec;

typedef struct smdFxTableCachePathCtrl
{
    void    * mSegIterator;
    stlBool   mFetchStarted;
} smdFxTableCachePathCtrl;

typedef stlStatus (*smdCreateSortFunc)( smlStatement       * aStatement,
                                        smlTbsId             aTbsId,
                                        smlSortTableAttr   * aAttr,
                                        void               * aBaseTableHandle,
                                        stlUInt16            aKeyColCount,
                                        knlValueBlockList  * aKeyColList,
                                        stlUInt8           * aKeyColFlags,
                                        stlInt64           * aRelationId,
                                        void              ** aRelationHandle,
                                        smlEnv             * aEnv );

typedef struct smdSortTableModule
{
    smdCreateSortFunc        mCreateSortTable;
} smdSortTableModule;

/**
 * @brief SMD_TRY_INTERNAL_ERROR 매크로
 * @remarks aParams 전체는 반드시 ()안에 들어 있어야 한다. <BR>
 * @par example
 * SML_ASSERT( aLsn < 10, aExpression, (aEnv, "LSN(%d)\n", aLsn) );
 */
#define SMD_TRY_INTERNAL_ERROR( aExpression, aEnv, aParams )    \
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
            STL_TRY( smdInternalError aParams == STL_SUCCESS ); \
        }                                                       \
    } while( 0 )

typedef struct smdInstHandleListNode smdInstHandleListNode;

/**
 * smlSessionEnv의 mInstTableHandleList에 매달리는 리스트의 노드
 */
struct smdInstHandleListNode
{
    stlInt64                mInstTableId;
    smeRelationHeader     * mHandle;
    smdInstHandleListNode * mNext;
    smdInstHandleListNode * mPrev;
};

#define SMD_FIRST_SESS_INST_TABLE_ID        1

#define SMD_GET_SESS_INST_HANDLE_LIST(aEnv) \
    ( (smdInstHandleListNode *)(SML_SESS_ENV(aEnv)->mInstTableList ) )

#define SMD_SET_SESS_INST_HANDLE_LIST(aEnv, aFirst)     \
    ( SML_SESS_ENV(aEnv)->mInstTableList = (aFirst) )

/** @} */
    
#endif /* _SMDDEF_H_ */
