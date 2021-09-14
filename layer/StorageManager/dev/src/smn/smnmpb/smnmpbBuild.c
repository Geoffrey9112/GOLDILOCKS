/*******************************************************************************
 * smnmpbBuild.c
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


#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <sml.h>
#include <smDef.h>
#include <sms.h>
#include <sme.h>
#include <smp.h>
#include <smn.h>
#include <smg.h>
#include <sma.h>
#include <smd.h>
#include <smxm.h>
#include <smxl.h>
#include <smnDef.h>
#include <smnmpbDef.h>
#include <smnmpb.h>

extern knlCompareFunc gSmnmpbCompareFuncs[2][2][2];

/**
 * @file smnmpbBuild.c
 * @brief Storage Manager Layer Memory B-Tree Index Build Routines
 */

/**
 * @addtogroup smn
 * @{
 */

stlStatus smnmpbBuild( smlStatement      * aStatement,
                       void              * aIndexHandle,
                       void              * aBaseTableHandle,
                       stlUInt16           aKeyColCount,
                       knlValueBlockList * aIndexColList,
                       stlUInt8          * aKeyColFlags,
                       stlUInt16           aParallelFactor,
                       smlEnv            * aEnv )
{
    stlInt64   sBuildDirection;
    void     * sSegmentHandle;

    sBuildDirection = knlGetPropertyBigIntValueByID( KNL_PROPERTY_INDEX_BUILD_DIRECTION,
                                                     KNL_ENV( aEnv ) );

    /**
     * Base Table의 유효성 검사
     */
    STL_TRY( smsValidateSegment( SME_GET_SEGMENT_HANDLE(aBaseTableHandle),
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Index validation
     */
    
#ifdef STL_DEBUG

    smnIndexHeader    * sIndexHeader = (smnIndexHeader*)aIndexHandle;
    knlValueBlockList * sBlockList = aIndexColList;
    stlInt32            i;
    
    STL_DASSERT( sIndexHeader->mKeyColCount == aKeyColCount );
    
    for( i = 0; i < aKeyColCount; i++ )
    {
        STL_DASSERT( sIndexHeader->mKeyColTypes[i] == KNL_GET_BLOCK_DB_TYPE( sBlockList ) );
        STL_DASSERT( sIndexHeader->mKeyColFlags[i] == aKeyColFlags[i] );
        sBlockList = sBlockList->mNext;
    }

#endif
    
    
    
    /**
     * 기본 Extent를 할당한다.
     */

    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aIndexHandle );
    
    STL_TRY( smsCreate( SMA_GET_TRANS_ID( aStatement ),
                        smsGetTbsId( sSegmentHandle ),
                        SML_SEG_TYPE_MEMORY_BITMAP,
                        sSegmentHandle,
                        0, /* aInitialExtents */
                        aEnv )
             == STL_SUCCESS );

    /**
     * Build now
     */
    
    if( sBuildDirection == SMN_BUILD_DIRECTION_TOP_DOWN )
    {
        STL_TRY( smnmpbTopDownBuild( aStatement,
                                     aIndexHandle,
                                     aBaseTableHandle,
                                     aKeyColCount,
                                     aIndexColList,
                                     aKeyColFlags,
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sBuildDirection == SMN_BUILD_DIRECTION_BOTTOM_UP );
        
        STL_TRY( smnmpbBottomUpBuild( aStatement,
                                      aIndexHandle,
                                      aBaseTableHandle,
                                      aKeyColCount,
                                      aIndexColList,
                                      aKeyColFlags,
                                      aParallelFactor,
                                      aEnv )
                 == STL_SUCCESS );
    }

	return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbBuildUnusable( smlStatement      * aStatement,
                               void              * aIndexHandle,
                               smlEnv            * aEnv )
{
    void              * sSegmentHandle;
    stlInt32            sState = 0;
    knlRegionMark       sMemMark;
    knlValueBlockList * sIndexColList = NULL;
    knlValueBlockList * sCurCol;
    knlValueBlockList * sPrevCol = NULL;
    stlInt16            i;
    smnIndexHeader    * sIndexHeader;
    void              * sValue;
    void              * sBaseTableHandle;

    sIndexHeader = (smnIndexHeader*)aIndexHandle;
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aIndexHandle );
    
    /**
     * prepare rebuild Index
     */
                    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    sPrevCol = NULL;
    for( i = 0; i < sIndexHeader->mKeyColCount; i++ )
    {
        STL_TRY( knlInitBlockNoBuffer( &sCurCol,
                                       STL_LAYER_STORAGE_MANAGER,
                                       1,
                                       sIndexHeader->mKeyColTypes[i],
                                       &aEnv->mOperationHeapMem,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
        if( i == 0 )
        {
            sIndexColList = sCurCol;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevCol, sCurCol );
        }

        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    sIndexHeader->mKeyColSize[i],
                                    &sValue,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        KNL_SET_BLOCK_DATA_PTR( sCurCol, 0, sValue, sIndexHeader->mKeyColSize[i] );
        KNL_SET_BLOCK_COLUMN_ORDER( sCurCol, sIndexHeader->mKeyColOrder[i] );
        
        sPrevCol = sCurCol;
    }

    STL_TRY( smeGetRelationHandle( sIndexHeader->mBaseTableSegRid,
                                   &sBaseTableHandle,
                                   aEnv)
             == STL_SUCCESS );

    /**
     * Logging
     */

    STL_TRY( smsSetUsableSegment( aStatement,
                                  sSegmentHandle,
                                  aEnv )
             == STL_SUCCESS);
    
    /**
     * Build Index
     */
    
    STL_TRY( smnmpbBuild( aStatement,
                          aIndexHandle,
                          sBaseTableHandle,
                          sIndexHeader->mKeyColCount,
                          sIndexColList,
                          sIndexHeader->mKeyColFlags,
                          0,   /* aParallelFactor */
                          aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv )
             == STL_SUCCESS );

	return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smnmpbTopDownBuild( smlStatement      * aStatement,
                              void              * aIndexHandle,
                              void              * aBaseTableHandle,
                              stlUInt16           aKeyColCount,
                              knlValueBlockList * aIndexColList,
                              stlUInt8          * aKeyColFlags,
                              smlEnv            * aEnv )
{
    smxmTrans               sMiniTrans;
    smpHandle               sSegPage;
    smpHandle               sPageHandle;
    smnIndexHeader        * sIdxHdr;
    stlUInt16               sState = 0;
    stlInt16                i;
    stlInt16                j;
    stlInt16                k;
    smlRid                  sSegRid = smsGetSegRid( SME_GET_SEGMENT_HANDLE(aIndexHandle) );
    stlUInt32               sAttr = SMXM_ATTR_REDO;
    stlUInt8                sKeyColTypes[SML_MAX_INDEX_KEY_COL_COUNT];
    smnmpbLogicalHdr        sLogicalHdr;
    knlValueBlockList     * sCol;
    smlIteratorProperty     sProperty;
    void                  * sIterator;
    stlInt32                sIterState = 0;
    smlRid                  sRowRid;
    smlScn                  sRowScn;
    stlUInt64               sSmoNo = 0;
    stlBool                 sIsUniqueIndex = ((smnIndexHeader*)aIndexHandle)->mUniqueness;
    stlBool                 sIsLogging = ((smnIndexHeader*)aIndexHandle)->mLoggingFlag;
    knlValueBlockList       sTableColList[SML_MAX_INDEX_KEY_COL_COUNT];
    knlValueBlockList     * sCurIndexCol = aIndexColList;
    smlRowBlock             sRowBlock;
    smlUniqueViolationScope sUniqueViolationScope;
    smlRid                  sUndoRid = SML_INVALID_RID;
    smlFetchInfo            sFetchInfo;
    
    if( sIsLogging == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }
    
    sRowBlock.mBlockSize = 1;
    sRowBlock.mUsedBlockSize = 0;
    sRowBlock.mRidBlock = &sRowRid;
    sRowBlock.mScnBlock = &sRowScn;
    
    /* 1. Index 생성시 logical header에 key column 정보를 저장하기 위해 *
     *    root page 한 개는 최소한 있어야 함                            */
    STL_TRY( smxmBegin( &sMiniTrans,
                        aStatement->mTransId,
                        sSegRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         sSegRid.mTbsId,
                         sSegRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sSegPage,
                         aEnv ) == STL_SUCCESS );

    sIdxHdr = (smnIndexHeader*)( (stlChar*)smpGetBody(SMP_FRAME(&sSegPage)) +
                                 smsGetHeaderSize(SME_GET_SEGMENT_HANDLE(aIndexHandle)) );

    STL_TRY( smsAllocPage( &sMiniTrans,
                           SME_GET_SEGMENT_HANDLE(aIndexHandle),
                           SMP_PAGE_TYPE_INDEX_DATA,
                           NULL,  /* aIsAgableFunc */
                           &sPageHandle,
                           aEnv ) == STL_SUCCESS );

    /* SMO No 초기화 */
    smpSetVolatileArea( &sPageHandle,
                        &sSmoNo,
                        STL_SIZEOF(stlUInt64) );

    sIdxHdr->mRootPageId = sPageHandle.mPch->mPageId;
    sIdxHdr->mMirrorRootPid = SMP_NULL_PID;
    ((smnIndexHeader*)aIndexHandle)->mRootPageId = sPageHandle.mPch->mPageId;
    ((smnIndexHeader*)aIndexHandle)->mMirrorRootPid = SMP_NULL_PID;

    for( i = 0,sCol = aIndexColList; sCol != NULL; i++,sCol = sCol->mNext )
    {
        sKeyColTypes[i] = KNL_GET_BLOCK_DB_TYPE(sCol);
    }

    stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
    sLogicalHdr.mPrevPageId = SMP_NULL_PID;
    sLogicalHdr.mNextPageId = SMP_NULL_PID;
    sLogicalHdr.mNextFreePageId = SMP_NULL_PID;
    sLogicalHdr.mLevel    = 0;
    sLogicalHdr.mKeyColCount = aKeyColCount;
    stlMemcpy( sLogicalHdr.mKeyColTypes, sKeyColTypes, aKeyColCount );
    stlMemcpy( sLogicalHdr.mKeyColFlags, aKeyColFlags, aKeyColCount );
    
    STL_TRY( smpInitBody( &sPageHandle,
                          SMP_ORDERED_SLOT_TYPE,
                          STL_SIZEOF(smnmpbLogicalHdr),
                          (void*)&sLogicalHdr,
                          SMN_GET_INITRANS(aIndexHandle),
                          SMN_GET_MAXTRANS(aIndexHandle),
                          STL_FALSE,
                          &sMiniTrans,
                          aEnv) == STL_SUCCESS );

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_SET_ROOT_PAGE_ID,
                           SMR_REDO_TARGET_PAGE,
                           (stlChar*)&sIdxHdr->mRootPageId,
                           STL_SIZEOF(smlPid),
                           sSegPage.mPch->mTbsId,
                           sSegPage.mPch->mPageId,
                           (stlChar*)sIdxHdr - (stlChar*)SMP_FRAME(&sSegPage),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_SET_MIRROR_ROOT_PAGE_ID,
                           SMR_REDO_TARGET_PAGE,
                           (stlChar*)&sIdxHdr->mMirrorRootPid,
                           STL_SIZEOF(smlPid),
                           sSegPage.mPch->mTbsId,
                           sSegPage.mPch->mPageId,
                           (stlChar*)sIdxHdr - (stlChar*)SMP_FRAME(&sSegPage),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    /**
     * 2. 테이블의 기존 데이터를 모두 insert
     * TODO : 현재는 MockUp이라, Bottom-up build가 구현될때까지는 insert로 일단 처리함
     * WARNING : Index build시 모든 key는 stable 상태로 처리하기 때문에
     * SERIALIZABLE인 tx는 잘못된 view를 볼 수 있다
     */

    /* make base table column list for fetching rows */
    sTableColList[0] = *sCurIndexCol;
    sCurIndexCol = sCurIndexCol->mNext;
    for( i = 1; i < aKeyColCount; i++ )
    {
        for( j = 0; j < i; j++ )
        {
            if( KNL_GET_BLOCK_COLUMN_ORDER(&sTableColList[j]) >
                KNL_GET_BLOCK_COLUMN_ORDER(sCurIndexCol) )
            {
                for( k = i - 1; k >= j; k--)
                {
                    sTableColList[k + 1] = sTableColList[k];
                }
                break;
            }
        }
        sTableColList[j] = *sCurIndexCol;
        sCurIndexCol = sCurIndexCol->mNext;
    }
    
    for( i = 0; i < aKeyColCount - 1; i++ )
    {
        KNL_LINK_BLOCK_LIST( &sTableColList[i], &sTableColList[i + 1] );
    }
    KNL_LINK_BLOCK_LIST( &sTableColList[aKeyColCount - 1], NULL );

    SML_INIT_ITERATOR_PROP( sProperty );

    STL_TRY( smlAllocIterator( aStatement,
                               aBaseTableHandle,
                               SML_TRM_COMMITTED,
                               SML_SRM_SNAPSHOT,
                               &sProperty,
                               SML_SCAN_FORWARD,
                               &sIterator,
                               aEnv ) == STL_SUCCESS );
    sIterState = 1;
    
    SML_SET_FETCH_INFO_FOR_TABLE( & sFetchInfo,        /* Fetch Info */
                                  NULL,                /* physical filter */
                                  NULL,                /* logical filter */
                                  sTableColList,       /* read column list */
                                  NULL,                /* rowid column list */
                                  & sRowBlock,         /* row block */
                                  0,                   /* skip count */
                                  SML_FETCH_COUNT_MAX, /* fetch count */
                                  STL_FALSE,           /* group key fetch */
                                  NULL                 /* filter evaluate info */
        );
        
    i = 0;
    while( STL_TRUE )
    {
        STL_TRY( smlFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv ) == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }

        STL_TRY( smnmpbInsert( aStatement,
                               aIndexHandle,
                               aIndexColList,
                               0,
                               &sRowRid,
                               &sUndoRid,
                               STL_TRUE,  /* aIsIndexBuilding */
                               STL_FALSE, /* aIsDeferred */
                               &sUniqueViolationScope,
                               aEnv )
                 == STL_SUCCESS );

        if( (sIsUniqueIndex == STL_TRUE) &&
            (sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE) )
        {
            STL_THROW( RAMP_ERR_UNIQUENESS_VIOLATION );
        }
        i++;

        /**
         * Query Timeout 검사
         */
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    sIterState = 0;
    STL_TRY( smlFreeIterator( sIterator, aEnv ) == STL_SUCCESS );

	return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_UNIQUENESS_VIOLATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INDEX_UNIQUENESS_VIOLATION,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        smxmRollback( &sMiniTrans, aEnv );
    }

    if( sIterState == 1 )
    {
        smlFreeIterator( sIterator, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smnmpbBottomUpBuild( smlStatement      * aStatement,
                               void              * aIndexHandle,
                               void              * aBaseTableHandle,
                               stlUInt16           aKeyColCount,
                               knlValueBlockList * aIndexColList,
                               stlUInt8          * aKeyColFlags,
                               stlUInt16           aParallelFactor,
                               smlEnv            * aEnv )
{
    stlInt64              sParallelFactor = aParallelFactor;
    stlThread             sThread[ SML_MAX_INDEX_BUILD_PARALLEL_FACTOR ];
    stlInt32              i;
    smnmpbParallelArgs    sArgs[ SML_MAX_INDEX_BUILD_PARALLEL_FACTOR ];
    stlStatus             sReturnStatus[ SML_MAX_INDEX_BUILD_PARALLEL_FACTOR ];
    stlInt64              sMergeRunCount;
    stlInt32              sBuildExtentPageCount;
    stlInt64              sPhysicalId;
    smnIndexHeader      * sIndexHeader;
    void                * sSegmentHandle;
    stlInt8               sKeyColOrder[SML_MAX_INDEX_KEY_COL_COUNT];
    stlCpuInfo            sCpuInfo;
    stlInt64              sPageCount;
    stlInt64              sSortRunSize;
    stlUInt8              sKeyColTypes[SML_MAX_INDEX_KEY_COL_COUNT];
    knlValueBlockList   * sSrcColumn = NULL;
    smnmpbStatistics    * sStatistics;
    stlTime               sBeginTime;

    sStatistics = SMNMPB_GET_MODULE_SPECIFIC_DATA( aIndexHandle );
    
    /**
     * adjust parallel factor
     */
    
    if( aParallelFactor == 0 )
    {
        /**
         * as property
         */
        sParallelFactor = knlGetPropertyBigIntValueByID( KNL_PROPERTY_INDEX_BUILD_PARALLEL_FACTOR,
                                                         KNL_ENV( aEnv ) );
    }

    if( sParallelFactor == 0 )
    {
        /**
         * as number of core
         */
        STL_TRY( stlGetCpuInfo( &sCpuInfo,
                                KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        sParallelFactor = sCpuInfo.mCpuCount;
        
        sPageCount = smsGetAllocPageCount( SME_GET_SEGMENT_HANDLE(aBaseTableHandle) );
        
        if( sPageCount < SMN_INDEX_BUILD_PARALLEL_THRESHOLD )
        {
            /**
             * as allocated page count of base table
             */
            sParallelFactor = 1;
        }
    }

    if( sParallelFactor > SML_MAX_INDEX_BUILD_PARALLEL_FACTOR )
    {
        /**
         * as maximum parallel factor
         */
        sParallelFactor = SML_MAX_INDEX_BUILD_PARALLEL_FACTOR;
    }
    
    if( sParallelFactor > SMN_MAX_MERGED_RUN_LIST )
    {
        /**
         * as maximum merged run list
         */
        sParallelFactor = SMN_MAX_MERGED_RUN_LIST;
    }

    sMergeRunCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MEMORY_MERGE_RUN_COUNT,
                                                    KNL_ENV( aEnv ) );
    sSortRunSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MEMORY_SORT_RUN_SIZE,
                                                  KNL_ENV( aEnv ) );

    /**
     * adjust build extent page count
     */
    sBuildExtentPageCount = smsGetUsablePageCountInExt( SME_GET_SEGMENT_HANDLE(aIndexHandle) );

    while( sBuildExtentPageCount > ((sSortRunSize / SMP_PAGE_SIZE) * 4) )
    {
        sBuildExtentPageCount /= 2;
    }
    
    /**
     * Parallel sort & merge keys
     */

    for( i = 0; i < aKeyColCount; i++ )
    {
        if( (aKeyColFlags[i] & DTL_KEYCOLUMN_INDEX_ORDER_MASK)
            == DTL_KEYCOLUMN_INDEX_ORDER_ASC )
        {
            sKeyColOrder[i] = 1;
        }
        else
        {
            sKeyColOrder[i] = -1;
        }
    }

    sSrcColumn = aIndexColList;
    i = 0;
    
    while( sSrcColumn != NULL )
    {
        sKeyColTypes[i] = KNL_GET_BLOCK_DB_TYPE( sSrcColumn );
        sSrcColumn = sSrcColumn->mNext;
        i++;
    }
    
    sIndexHeader = (smnIndexHeader*)aIndexHandle;
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aIndexHandle );
    
    for( i = 0; i < sParallelFactor; i++ )
    {
        sArgs[i].mParentEnv            = aEnv;
        sArgs[i].mParallelId           = i;
        sArgs[i].mStatement            = aStatement;
        sArgs[i].mIndexHandle          = aIndexHandle;
        sArgs[i].mBaseTableHandle      = aBaseTableHandle;
        sArgs[i].mKeyColCount          = aKeyColCount;
        sArgs[i].mIndexColList         = aIndexColList;
        sArgs[i].mKeyColFlags          = aKeyColFlags;
        sArgs[i].mKeyColOrder          = sKeyColOrder;
        sArgs[i].mKeyColTypes          = sKeyColTypes;
        sArgs[i].mParallelFactor       = sParallelFactor;
        sArgs[i].mTransId              = SMA_GET_TRANS_ID( aStatement );
        sArgs[i].mCutOffRunCount       = 1;
        sArgs[i].mTotalRunCount        = 0;
        sArgs[i].mBuildExtentPageCount = sBuildExtentPageCount;
        sArgs[i].mMaxMergeRunCount     = sMergeRunCount;
        sArgs[i].mIsUniqueIndex        = sIndexHeader->mUniqueness;
        sArgs[i].mIsPrimaryIndex       = sIndexHeader->mPrimary;
        sArgs[i].mIsLogging            = sIndexHeader->mLoggingFlag;
        sArgs[i].mTbsId                = smsGetTbsId( sSegmentHandle );
        sArgs[i].mSortElapsedTime      = 0;
        sArgs[i].mMergeElapsedTime     = 0;
        
        sArgs[i].mSortCompare          = smnmpbGetCompareFunc( SMNMPB_COMPARE_QUICK_SORT,
                                                               sIndexHeader->m1ByteFastBuild,
                                                               sArgs[i].mIsPrimaryIndex );
        sArgs[i].mMergeCompare         = smnmpbGetCompareFunc( SMNMPB_COMPARE_HEAP_SORT,
                                                               sIndexHeader->m1ByteFastBuild,
                                                               sArgs[i].mIsPrimaryIndex );
                                                               
        STL_INIT_ERROR_STACK( &sArgs[i].mErrorStack );
        
        stlMemset( sArgs[i].mMergedRunList, 0x00, STL_SIZEOF(smlPid) * SMN_MAX_MERGED_RUN_LIST );
        stlMemset( sArgs[i].mFreeExtentArray, 0x00, STL_SIZEOF(smlPid) * SMNMPB_MAX_FREE_EXTENT_COUNT );

        STL_TRY( smsAllocSegIterator( aStatement,
                                      SME_GET_SEGMENT_HANDLE(aBaseTableHandle),
                                      &sArgs[i].mSegIterator,
                                      aEnv )
                 == STL_SUCCESS );
    }

    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Sort & Merge
     */

    if( sParallelFactor == 1 )
    {
        STL_TRY( smnmpbSortAndMerge( &sArgs[0],
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        for( i = 0; i < sParallelFactor; i++ )
        {
            STL_TRY( stlCreateThread( &sThread[i],
                                      NULL,                 /* aThreadAttr */
                                      smnmpbSortAndMergeThread,
                                      (void*)&sArgs[i],     /* aArgs */
                                      KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        }
        
        for( i = 0; i < sParallelFactor; i++ )
        {
            STL_TRY( stlJoinThread( &sThread[i],
                                    &sReturnStatus[i],
                                    KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );
        }

        for( i = 0; i < sParallelFactor; i++ )
        {
            if( sReturnStatus[i] == STL_FAILURE )
            {
                stlAppendErrors( KNL_ERROR_STACK( aEnv ),
                                 &sArgs[i].mErrorStack );

                STL_TRY( STL_FALSE );
            }
        }
    }

    sStatistics->mSortElapsedTime = 0;
    sStatistics->mMergeElapsedTime = 0;
    
    for( i = 0; i < sParallelFactor; i++ )
    {
        sStatistics->mSortElapsedTime = STL_MAX( sStatistics->mSortElapsedTime,
                                                 sArgs[i].mSortElapsedTime );
        sStatistics->mMergeElapsedTime = STL_MAX( sStatistics->mMergeElapsedTime,
                                                  sArgs[i].mMergeElapsedTime );
    }

    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

    sBeginTime = stlNow();

    /**
     * Build tree
     */

    if( sParallelFactor == 1 )
    {
        STL_TRY( smnmpbBuildTree( &sArgs[0],
                                  aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Grouping runs
         */

        STL_TRY( smnmpbGroupingRuns( sArgs,
                                     aEnv )
                 == STL_SUCCESS );
    
        /**
         * Build sub-tree
         */
        
        for( i = 0; i < sParallelFactor; i++ )
        {
            STL_TRY( stlCreateThread( &sThread[i],
                                      NULL,                 /* aThreadAttr */
                                      smnmpbBuildTreeThread,
                                      (void*)&sArgs[i],     /* aArgs */
                                      KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        }
        
        for( i = 0; i < sParallelFactor; i++ )
        {
            STL_TRY( stlJoinThread( &sThread[i],
                                    &sReturnStatus[i],
                                    KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );
        }

        for( i = 0; i < sParallelFactor; i++ )
        {
            if( sReturnStatus[i] == STL_FAILURE )
            {
                stlAppendErrors( KNL_ERROR_STACK( aEnv ),
                                 &sArgs[i].mErrorStack );

                STL_TRY( STL_FALSE );
            }
        }

        /**
         * Merge sub-tree
         */
        
        STL_TRY( smnmpbMergeTree( sArgs, aEnv ) == STL_SUCCESS );
    }

    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Finalize build tree
     * - set root page identifier
     * - set max key
     * - free unused extents
     */
    
    STL_TRY( smnmpbFinalizeBuildTree( &sArgs[0],
                                      aEnv )
             == STL_SUCCESS );
    
    sStatistics->mBuildElapsedTime = stlNow() - sBeginTime;
    
	return STL_SUCCESS;

    STL_FINISH;

    sPhysicalId = smsGetSegmentId( SME_GET_SEGMENT_HANDLE(aIndexHandle) );
    (void)knlLogMsg( NULL,
                     KNL_ENV(aEnv),
                     KNL_LOG_LEVEL_INFO,
                     "Index creation failed ( physical id : %ld, error code : %d )\n",
                     sPhysicalId,
                     stlGetLastErrorCode(KNL_ERROR_STACK(aEnv)) );
    
    return STL_FAILURE;
}

void * STL_THREAD_FUNC smnmpbSortAndMergeThread( stlThread * aThread,
                                                 void      * aArg )
{
    smnmpbParallelArgs  * sArgs = (smnmpbParallelArgs*)aArg;
    stlErrorStack         sErrorStack;
    stlInt32              sState = 0;
    smlEnv              * sEnv = NULL;
    smlSessionEnv       * sSessionEnv;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( stlBlockRealtimeThreadSignals( &sErrorStack ) == STL_SUCCESS );
    
    /**
     * Allocate Environments
     */
    STL_TRY( knlAllocEnv( (void**)&sEnv,
                          &sErrorStack )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitializeEnv( sEnv,
                               KNL_ENV_SHARED )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( knlAllocSessionEnv( (void**)&sSessionEnv,
                                 KNL_ENV( sEnv ) )
             == STL_SUCCESS );
    sState = 3;

    STL_TRY( smlInitializeSessionEnv( sSessionEnv,
                                      KNL_CONNECTION_TYPE_DA,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_SHARED,
                                      sEnv )
             == STL_SUCCESS );
    sState = 4;

    KNL_LINK_SESS_ENV( sEnv, sSessionEnv );

    /**
     * Sort and Merge
     */
    
    STL_TRY( smnmpbSortAndMerge( sArgs,
                                 sEnv )
             == STL_SUCCESS );

    STL_DASSERT( sSessionEnv->mFixCount == 0 );
    
    /**
     * Free Env
     */
    
    sState = 3;
    STL_TRY( smlFinalizeSessionEnv( sSessionEnv,
                                    sEnv )
             == STL_SUCCESS );

    sState = 2;
    STL_TRY( knlFreeSessionEnv( (void*)sSessionEnv,
                                KNL_ENV( sEnv ) )
             == STL_SUCCESS );

    sState = 1;
    STL_TRY( smlFinalizeEnv( sEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlFreeEnv( (void*)sEnv,
                         &sErrorStack )
             == STL_SUCCESS );

    stlExitThread( aThread, STL_SUCCESS, &sErrorStack );
    
    return NULL;

    STL_FINISH;

    if( sState > 0 )
    {
        stlAppendErrors( &sArgs->mErrorStack, KNL_ERROR_STACK(sEnv) );
    }
    else
    {
        stlAppendErrors( &sArgs->mErrorStack, &sErrorStack );
    }
    
    switch( sState )
    {
        case 4 :
            (void)smlFinalizeSessionEnv( sSessionEnv,
                                         sEnv );
        case 3 :
            (void)knlFreeSessionEnv( (void*)sSessionEnv,
                                     KNL_ENV( sEnv ) );
        case 2 :
            (void)smlFinalizeEnv( sEnv );
        case 1 :
            (void)knlFreeEnv( (void*)sEnv, &sErrorStack );
        default :
            break;
    }

    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
}

void * STL_THREAD_FUNC smnmpbBuildTreeThread( stlThread * aThread,
                                              void      * aArg )
{
    smnmpbParallelArgs  * sArgs = (smnmpbParallelArgs*)aArg;
    stlErrorStack         sErrorStack;
    stlInt32              sState = 0;
    smlEnv              * sEnv = NULL;
    smlSessionEnv       * sSessionEnv;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( stlBlockRealtimeThreadSignals( &sErrorStack ) == STL_SUCCESS );
    
    /**
     * Allocate Environments
     */
    STL_TRY( knlAllocEnv( (void**)&sEnv,
                          &sErrorStack )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitializeEnv( sEnv,
                               KNL_ENV_SHARED )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( knlAllocSessionEnv( (void**)&sSessionEnv,
                                 KNL_ENV( sEnv ) )
             == STL_SUCCESS );
    sState = 3;

    STL_TRY( smlInitializeSessionEnv( sSessionEnv,
                                      KNL_CONNECTION_TYPE_DA,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_SHARED,
                                      sEnv )
             == STL_SUCCESS );
    sState = 4;

    KNL_LINK_SESS_ENV( sEnv, sSessionEnv );

    /**
     * Build Subtree
     */

    STL_TRY( smnmpbBuildTree( sArgs,
                              sEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( sSessionEnv->mFixCount == 0 );

    /**
     * Free Env
     */
    
    sState = 3;
    STL_TRY( smlFinalizeSessionEnv( sSessionEnv,
                                    sEnv )
             == STL_SUCCESS );

    sState = 2;
    STL_TRY( knlFreeSessionEnv( (void*)sSessionEnv,
                                KNL_ENV( sEnv ) )
             == STL_SUCCESS );

    sState = 1;
    STL_TRY( smlFinalizeEnv( sEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( knlFreeEnv( (void*)sEnv,
                         &sErrorStack )
             == STL_SUCCESS );

    stlExitThread( aThread, STL_SUCCESS, &sErrorStack );
    
    return NULL;

    STL_FINISH;

    if( sState > 0 )
    {
        stlAppendErrors( &sArgs->mErrorStack, KNL_ERROR_STACK(sEnv) );
    }
    else
    {
        stlAppendErrors( &sArgs->mErrorStack, &sErrorStack );
    }
    
    switch( sState )
    {
        case 4 :
            (void)smlFinalizeSessionEnv( sSessionEnv,
                                         sEnv );
        case 3 :
            (void)knlFreeSessionEnv( (void*)sSessionEnv,
                                     KNL_ENV( sEnv ) );
        case 2 :
            (void)smlFinalizeEnv( sEnv );
        case 1 :
            (void)knlFreeEnv( (void*)sEnv, &sErrorStack );
        default :
            break;
    }

    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
}

stlStatus smnmpbSortAndMerge( smnmpbParallelArgs * aArgs,
                              smlEnv             * aEnv )
{
    knlValueBlockList   * sSrcColumn = NULL;
    stlInt32              i;
    stlInt32              j;
    stlInt32              k;
    smnmpbCompareInfo     sCompareInfo;
    dtlDataType           sDataType;
    knlQueueInfo          sRunQueueInfo;
    knlQueueInfo          sFreeQueueInfo;
    stlBool               sIsEmpty;
    smlPid                sExtentPid;
    stlInt64              sBlockReadCount;
    knlColumnInReadRow  * sColumnInRow;
    smdValueBlockList     sIndexColList[SML_MAX_INDEX_KEY_COL_COUNT];
    smdValueBlockList     sTableColList[SML_MAX_INDEX_KEY_COL_COUNT];
    smdValueBlock       * sValueBlock;
    stlInt32            * sKeyValueSize;
    stlInt16            * sRowSeq;
    dtlDataValue        * sKeyValue;
    stlUInt64           * sKeyColSize;
    stlChar             * sValue;
    stlTime               sBeginTime;
    knlRegionMark         sMemMark;
    stlInt32              sState = 0;

    sBeginTime = stlNow();
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * Create index column list
     */

    sKeyColSize = ((smnIndexHeader*)aArgs->mIndexHandle)->mKeyColSize;
    
    sBlockReadCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_BLOCK_READ_COUNT,
                                                     KNL_ENV( aEnv ) );

    sSrcColumn = aArgs->mIndexColList;
    i = 0;
    
    for( i = 0; i < aArgs->mKeyColCount; i++ )
    {
        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    STL_SIZEOF(smdValueBlock),
                                    (void **) & sValueBlock,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    STL_SIZEOF(knlColumnInReadRow) * sBlockReadCount,
                                    (void **) & sColumnInRow,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    STL_SIZEOF(dtlDataValue) * sBlockReadCount,
                                    (void **) & sKeyValue,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    sBlockReadCount * sKeyColSize[i],
                                    (void **) & sValue,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( j = 0; j < sBlockReadCount; j++ )
        {
            sKeyValue[j].mValue = sValue + ( sKeyColSize[i] * j );
            sKeyValue[j].mBufferSize = sKeyColSize[i];
        }

        sValueBlock->mKeyValue = sKeyValue;
        sValueBlock->mColumnInRow = sColumnInRow;
        sValueBlock->mAllocBlockCnt = sBlockReadCount;
        sValueBlock->mUsedBlockCnt = 0;
        sValueBlock->mSkipBlockCnt = 0;
        
        sIndexColList[i].mColumnOrder = KNL_GET_BLOCK_COLUMN_ORDER(sSrcColumn);
        sIndexColList[i].mValueBlock = sValueBlock;
        
        sDataType = KNL_GET_BLOCK_DB_TYPE( sSrcColumn );
        sCompareInfo.mPhysicalCompare[i] = dtlPhysicalCompareFuncList[sDataType][sDataType];
        
        sSrcColumn = sSrcColumn->mNext;
    }

    STL_DASSERT( sSrcColumn == NULL );

    /**
     * Linking index column arrary
     */
    
    sIndexColList[aArgs->mKeyColCount - 1].mNext = NULL;
    
    for( i = 0; i < aArgs->mKeyColCount - 1; i++ )
    {
        sIndexColList[i].mNext = &sIndexColList[i + 1];
    }
    
    sCompareInfo.mSortBlock = NULL;
    sCompareInfo.mCompareCount = 0;
    sCompareInfo.mUniqueViolated = 0;
    sCompareInfo.mKeyColCount = aArgs->mKeyColCount;
    sCompareInfo.mKeyColFlags = aArgs->mKeyColFlags;
    sCompareInfo.mKeyColOrder = aArgs->mKeyColOrder;
    
    /**
     * make base table column list for fetching rows
     */
    
    sTableColList[0] = sIndexColList[0];

    for( i = 1; i < aArgs->mKeyColCount; i++ )
    {
        for( j = 0; j < i; j++ )
        {
            if( sTableColList[j].mColumnOrder > sIndexColList[i].mColumnOrder )
            {
                for( k = i - 1; k >= j; k--)
                {
                    sTableColList[k + 1] = sTableColList[k];
                }
                break;
            }
        }
        
        sTableColList[j] = sIndexColList[i];
    }
    
    /**
     * Linking table column arrary
     */
    
    sTableColList[aArgs->mKeyColCount - 1].mNext = NULL;
    
    for( i = 0; i < aArgs->mKeyColCount - 1; i++ )
    {
        sTableColList[i].mNext = &sTableColList[i + 1];
    }

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                STL_SIZEOF(stlInt32) * sBlockReadCount,
                                (void **) & sKeyValueSize,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                STL_SIZEOF(stlInt16) * sBlockReadCount,
                                (void **) & sRowSeq,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Initialize Run Queue & Free Page Queue
     */
    
    STL_TRY( knlInitQueue( &sRunQueueInfo,
                           1024,
                           STL_SIZEOF(smlPid),
                           &aEnv->mDynamicHeapMem,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( knlInitQueue( &sFreeQueueInfo,
                           1024,
                           STL_SIZEOF(smlPid),
                           &aEnv->mDynamicHeapMem,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 3;
    
    /**
     * Extract and sort keys
     */

    STL_TRY( smnmpbSortKeys( aArgs,
                             &sRunQueueInfo,
                             &sFreeQueueInfo,
                             sIndexColList,
                             sTableColList,
                             sKeyValueSize,
                             sRowSeq,
                             &sCompareInfo,
                             aEnv )
             == STL_SUCCESS );

    aArgs->mSortElapsedTime = stlNow() - sBeginTime;
    
    sBeginTime = stlNow();
    
    /**
     * Merge sorted runs
     */
    
    STL_TRY( smnmpbMergeKeys( aArgs,
                              &sRunQueueInfo,
                              aArgs->mMergedRunList,
                              &sCompareInfo,
                              &sFreeQueueInfo,
                              aEnv )
             == STL_SUCCESS );

    /**
     * Fill Free Page Array
     */

    for( i = 0; i < SMNMPB_MAX_FREE_EXTENT_COUNT; i++ )
    {
        STL_TRY( knlDequeue( &sFreeQueueInfo,
                             (void*)&sExtentPid,
                             &sIsEmpty,
                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sIsEmpty == STL_TRUE )
        {
            break;
        }

        aArgs->mFreeExtentArray[i] = sExtentPid;
    }

    /**
     * 아직도 Free Extent가 남아 있다면 Segment로 반납한다.
     */

    STL_TRY( smnmpbFreeExtents( aArgs->mTransId,
                                aArgs->mIndexHandle,
                                aArgs->mBuildExtentPageCount,
                                &sFreeQueueInfo,
                                aEnv )
             == STL_SUCCESS );
    
    aArgs->mMergeElapsedTime = stlNow() - sBeginTime;

    sState = 2;
    STL_TRY( knlFiniQueue( &sRunQueueInfo,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( knlFiniQueue( &sFreeQueueInfo,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) knlFiniQueue( &sRunQueueInfo, KNL_ENV(aEnv) );
        case 2:
            (void) knlFiniQueue( &sFreeQueueInfo, KNL_ENV(aEnv) );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_TRUE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smnmpbSortKeys( smnmpbParallelArgs  * aArgs,
                          knlQueueInfo        * aRunQueueInfo,
                          knlQueueInfo        * aFreeQueueInfo,
                          smdValueBlockList   * aIndexColList,
                          smdValueBlockList   * aTableColList,
                          stlInt32            * aKeyValueSize,
                          stlInt16            * aRowSeq,
                          smnmpbCompareInfo   * aCompareInfo,
                          smlEnv              * aEnv )
{
    smlPid               sPageId;
    smpHandle            sPageHandle;
    smpRowItemIterator   sSlotIterator;
    stlInt32             sState = 0;
    stlBool              sIsExtractSuccess;
    stlBool              sIsWriteSuccess;
    stlChar           ** sSortBlock[2];
    knlQSortContext      sSortContext;
    smnmpbSortBlockHdr * sSortBlockHdr;
    void               * sSegmentHandle;
    smdValueBlockList  * sColumn;
    smlTbsId             sTbsId;
    stlBool              sHasNullInBlock = STL_FALSE;
    stlInt32             i;
    stlInt64             sSortRunSize;
    stlInt32             sSortBlockCount;
    stlInt32             sSortBlockIdx = 0;
    knlHeapQueueContext  sHeapQueueContext;
    smnmpbSortIterator   sIterator[SMN_MAX_MERGED_RUN_LIST];
    stlInt64             sKeyCount;
    knlRegionMark        sMemMark;

    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aArgs->mBaseTableHandle );
    sTbsId = smsGetTbsId( sSegmentHandle );

    sSortRunSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MEMORY_SORT_RUN_SIZE,
                                                     KNL_ENV( aEnv ) );
    sSortBlockCount = aArgs->mBuildExtentPageCount / (sSortRunSize / SMP_PAGE_SIZE);

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * refine sort block count
     */
    
    if( sSortBlockCount == 0 )
    {
        sSortBlockCount = 1;
    }

    if( sSortBlockCount > SMN_MAX_MERGED_RUN_LIST )
    {
        sSortBlockCount = SMN_MAX_MERGED_RUN_LIST;
    }

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sSortBlockCount * STL_SIZEOF(smnmpbSortBlockHdr),
                                (void**)&sSortBlockHdr,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sSortBlockCount * STL_SIZEOF(stlChar*),
                                (void**)&sSortBlock[0],
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sSortBlockCount * STL_SIZEOF(stlChar*),
                                (void**)&sSortBlock[1],
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    for( i = 0; i < sSortBlockCount; i++ )
    {
        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    sSortRunSize,
                                    (void**)&sSortBlock[0][i],
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    sSortRunSize,
                                    (void**)&sSortBlock[1][i],
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        sSortBlockHdr[i].mKeyCount = 0;
        sSortBlockHdr[i].mTotalSize = sSortRunSize;
        sSortBlockHdr[i].mLowWaterMark = sSortBlockHdr[i].mTotalSize;
        sSortBlockHdr[i].mHighWaterMark = 0;
        sSortBlockHdr[i].mActiveSlotIdx = 0;

        sIterator[i].mSortBlockHdr = &sSortBlockHdr[i];
    }
    
    sHeapQueueContext.mCompare = aArgs->mMergeCompare;
    sHeapQueueContext.mFetchNext = smnmpbSortFetchNext;
    sHeapQueueContext.mCompareInfo = aCompareInfo;
    sHeapQueueContext.mIterator = &sIterator;

    sSortContext.mCompare = aArgs->mSortCompare;
    sSortContext.mElemType = KNL_QSORT_ELEM_2BYTE;
    sSortContext.mIsUnique = STL_TRUE;
    sSortContext.mCmpInfo  = aCompareInfo;
        
    STL_TRY( smsGetFirstPage( sSegmentHandle,
                              aArgs->mSegIterator,
                              &sPageId,
                              aEnv )
             == STL_SUCCESS );

    while( sPageId != SMP_NULL_PID )
    {
        if( (sPageId % aArgs->mParallelFactor) != aArgs->mParallelId )
        {
            STL_TRY( smsGetNextPage( sSegmentHandle,
                                     aArgs->mSegIterator,
                                     &sPageId,
                                     aEnv )
                     == STL_SUCCESS );
            continue;
        }
        
        STL_TRY( smpFix( sTbsId,
                         sPageId,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 2;

        /**
         * Extract keys
         */

        STL_TRY( smdExtractKeyValue( aArgs->mBaseTableHandle,
                                     &sPageHandle,
                                     STL_TRUE,   /* aFetchFirst */
                                     &sSlotIterator,
                                     aTableColList,
                                     aKeyValueSize,
                                     aRowSeq,
                                     &sHasNullInBlock,
                                     &sIsExtractSuccess,
                                     aEnv )
                 == STL_SUCCESS );

        while( sIsExtractSuccess == STL_TRUE )
        {
            STL_TRY_THROW( (aArgs->mIsPrimaryIndex != STL_TRUE) ||
                           (sHasNullInBlock != STL_TRUE),
                           RAMP_ERR_NOT_NULL_CONSTRAINT );

            STL_TRY( smnmpbWriteKeyToSortBlock( &sSortBlockHdr[sSortBlockIdx],
                                                sSortBlock[0][sSortBlockIdx],
                                                sPageId,
                                                aRowSeq,
                                                aIndexColList,
                                                SMA_GET_TCN( aArgs->mStatement ),
                                                aKeyValueSize,
                                                &sIsWriteSuccess,
                                                aEnv )
                     == STL_SUCCESS );
                
            while( sIsWriteSuccess == STL_FALSE )
            {
                /**
                 * Parent Event 검사
                 */

                STL_TRY( knlCheckParentQueryTimeout( KNL_ENV(aArgs->mParentEnv),
                                                     KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                /**
                 * Quick Sort
                 */

                aCompareInfo->mSortBlock = sSortBlock[0][sSortBlockIdx];
                
                knlQSort( (void*)sSortBlock[0][sSortBlockIdx],
                          sSortBlockHdr[sSortBlockIdx].mKeyCount,
                          &sSortContext );

                STL_TRY_THROW( (aArgs->mIsUniqueIndex != STL_TRUE) ||
                               (aCompareInfo->mUniqueViolated == 0),
                               RAMP_ERR_UNIQUE_VIOLATED );

                sSortBlockIdx++;

                if( sSortBlockIdx == sSortBlockCount )
                {
                    /**
                     * Write sorted keys
                     */

                    for( i = 0; i < sSortBlockCount; i++ )
                    {
                        sIterator[i].mSortBlock = sSortBlock[0][i];
                    }
                    
                    STL_TRY( smnmpbWriteKeyToSortedRun( aArgs->mTransId,
                                                        aArgs->mIndexHandle,
                                                        &sHeapQueueContext,
                                                        sSortBlockCount,
                                                        aRunQueueInfo,
                                                        aFreeQueueInfo,
                                                        aArgs->mBuildExtentPageCount,
                                                        aEnv )
                             == STL_SUCCESS );
                
                    STL_TRY( smnmpbCompactSortBlock( sSortBlockHdr,
                                                     sSortBlock,
                                                     sSortBlockCount,
                                                     aEnv )
                             == STL_SUCCESS );

                    sSortBlockIdx = 0;
                }
                  
                STL_TRY( smnmpbWriteKeyToSortBlock( &sSortBlockHdr[sSortBlockIdx],
                                                    sSortBlock[0][sSortBlockIdx],
                                                    sPageId,
                                                    aRowSeq,
                                                    aIndexColList,
                                                    SMA_GET_TCN( aArgs->mStatement ),
                                                    aKeyValueSize,
                                                    &sIsWriteSuccess,
                                                    aEnv )
                         == STL_SUCCESS );
            }

            /**
             * Reset Value Block List
             */
            
            sColumn = aIndexColList;
            
            while( sColumn != NULL )
            {
                sColumn->mValueBlock->mUsedBlockCnt = 0;
                sColumn->mValueBlock->mSkipBlockCnt = 0;
                sColumn = sColumn->mNext;
            }

            /**
             * Extract keys
             */

            STL_TRY( smdExtractKeyValue( aArgs->mBaseTableHandle,
                                         &sPageHandle,
                                         STL_FALSE,   /* aFetchFirst */
                                         &sSlotIterator,
                                         aTableColList,
                                         aKeyValueSize,
                                         aRowSeq,
                                         &sHasNullInBlock,
                                         &sIsExtractSuccess,
                                         aEnv )
                     == STL_SUCCESS );
        }

        sState = 1;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

        STL_TRY( smsGetNextPage( sSegmentHandle,
                                 aArgs->mSegIterator,
                                 &sPageId,
                                 aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Sort Block에 남아 있는 키들을 정렬해서 저장한다.
     */

    if( sSortBlockHdr[sSortBlockIdx].mKeyCount > 0 )
    {
        aCompareInfo->mSortBlock = sSortBlock[0][sSortBlockIdx];

        knlQSort( sSortBlock[0][sSortBlockIdx],
                  sSortBlockHdr[sSortBlockIdx].mKeyCount,
                  &sSortContext );

        STL_TRY_THROW( (aArgs->mIsUniqueIndex != STL_TRUE) ||
                       (aCompareInfo->mUniqueViolated == 0),
                       RAMP_ERR_UNIQUE_VIOLATED );
        
        while( STL_TRUE )
        {
            /**
             * Write sorted keys
             */

            for( i = 0; i < sSortBlockCount; i++ )
            {
                sIterator[i].mSortBlock = sSortBlock[0][i];
            }
                    
            STL_TRY( smnmpbWriteKeyToSortedRun( aArgs->mTransId,
                                                aArgs->mIndexHandle,
                                                &sHeapQueueContext,
                                                sSortBlockCount,
                                                aRunQueueInfo,
                                                aFreeQueueInfo,
                                                aArgs->mBuildExtentPageCount,
                                                aEnv )
                     == STL_SUCCESS );
        
            STL_TRY( smnmpbCompactSortBlock( sSortBlockHdr,
                                             sSortBlock,
                                             sSortBlockCount,
                                             aEnv )
                     == STL_SUCCESS );

            for( sKeyCount = 0, i = 0 ; i < sSortBlockCount; i++ )
            {
                sKeyCount += sSortBlockHdr[i].mKeyCount;
            }

            if( sKeyCount == 0 )
            {
                break;
            }
        }
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_NULL_CONSTRAINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_NULL_CONSTRAINT_VIOLATION,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_UNIQUE_VIOLATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INDEX_UNIQUENESS_VIOLATION,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smpUnfix( &sPageHandle, aEnv );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_TRUE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smnmpbWriteKeyToSortBlock( smnmpbSortBlockHdr * aSortBlockHdr,
                                     stlChar            * aSortBlock,
                                     smlPid               aRowPid,
                                     stlInt16           * aRowSeq,
                                     smdValueBlockList  * aColumnList,
                                     smlTcn               aTcn,
                                     stlInt32           * aKeyValueSize,
                                     stlBool            * aIsSuccess,
                                     smlEnv             * aEnv )
{
    stlInt16             sAllocSize;
    stlChar            * sRunKey;
    stlChar            * sKeyBody;
    smdValueBlockList  * sColumn;
    knlColumnInReadRow * sColumnInRow;
    stlUInt8             sColLenSize;
    stlInt32             i;
    stlUInt8             sInsTcn = aTcn;

    *aIsSuccess = STL_FALSE;

    if( sInsTcn >= SMNMPB_MAX_TCN )
    {
        sInsTcn = SMNMPB_INVALID_TCN;
    }

    for( i = aColumnList->mValueBlock->mSkipBlockCnt;
         i < aColumnList->mValueBlock->mUsedBlockCnt;
         i++ )
    {
        sAllocSize = ( aKeyValueSize[i] + SMNMPB_RUN_KEY_HDR_LEN );

        STL_TRY_THROW( aKeyValueSize[i] <= SMN_MAX_KEY_BODY_SIZE,
                       RAMP_ERR_TOO_LARGE_KEY);
        
        STL_TRY_THROW( (aSortBlock + aSortBlockHdr->mLowWaterMark - sAllocSize) >
                       (aSortBlock + aSortBlockHdr->mHighWaterMark + STL_SIZEOF(smnmpbSortOffset)),
                       RAMP_FINISH );
    
        sRunKey = aSortBlock + aSortBlockHdr->mLowWaterMark - sAllocSize;
        sKeyBody = ( sRunKey + SMNMPB_RUN_KEY_HDR_LEN );
        *((smnmpbSortOffset*)(aSortBlock + aSortBlockHdr->mHighWaterMark)) = (aSortBlockHdr->mLowWaterMark -
                                                                      sAllocSize);

        SMNMPB_RUN_KEY_SET_TOTAL_SIZE( sRunKey, &sAllocSize );
        SMNMPB_RUN_KEY_SET_ROW_PID( sRunKey, &aRowPid );
        SMNMPB_RUN_KEY_SET_ROW_OFFSET( sRunKey, &aRowSeq[i] );
        SMNMPB_RUN_KEY_SET_TCN( sRunKey, &sInsTcn );

        sColumn = aColumnList;
    
        while( sColumn != NULL )
        {
            sColumnInRow = &(sColumn->mValueBlock->mColumnInRow[i]);
            SMP_GET_COLUMN_LEN_SIZE( sColumnInRow->mLength, &sColLenSize );
            SMP_WRITE_COLUMN_ZERO_INSENS( sKeyBody,
                                          sColumnInRow->mValue,
                                          sColumnInRow->mLength );

            sKeyBody += (sColumnInRow->mLength + sColLenSize);
            sColumn->mValueBlock->mSkipBlockCnt++;
            
            sColumn = sColumn->mNext;
        }
        
        aSortBlockHdr->mHighWaterMark += STL_SIZEOF(smnmpbSortOffset);
        aSortBlockHdr->mLowWaterMark -= sAllocSize;
        aSortBlockHdr->mKeyCount++;
    }

    *aIsSuccess = STL_TRUE;
        
    STL_RAMP( RAMP_FINISH );
        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LARGE_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEEDED_MAX_KEY_SIZE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      SMN_MAX_KEY_BODY_SIZE );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbWriteKeyToSortedRun( smxlTransId           aTransId,
                                     void                * aIndexHandle,
                                     knlHeapQueueContext * aHeapQueueContext,
                                     stlInt32              aSortBlockCount,
                                     knlQueueInfo        * aRunQueueInfo,
                                     knlQueueInfo        * aFreeQueueInfo,
                                     stlInt32              aBuildExtentPageCount,
                                     smlEnv              * aEnv )
{
    stlInt16              sKeySize;
    void                * sSegmentHandle;
    smlTbsId              sTbsId;
    stlInt32              sState = 0;
    smlPid                sFirstPid;
    smnmpbSortIterator  * sIterator;
    stlChar             * sSrcKey;
    stlChar             * sDstKey;
    smpHandle             sPageHandle;
    stlInt16              sSlotId;
    stlInt32              sPageSeq = 0;
    stlInt64              sIdx;
    stlInt64              sIteratorIdx;
    stlInt64              i;
    knlHeapQueueInfo      sHeapQueueInfo;
    knlHeapQueueEntry     sHeapQueueArray[SMN_MAX_MERGED_RUN_LIST*2];

    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aIndexHandle );
    sTbsId = smsGetTbsId( sSegmentHandle );

    STL_TRY( smnmpbAllocExtent4Build( aTransId,
                                      aBuildExtentPageCount,
                                      sSegmentHandle,
                                      aFreeQueueInfo,
                                      &sFirstPid,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * Enqueue first data page
     */
    
    STL_TRY( knlEnqueue( aRunQueueInfo,
                         (void*)&sFirstPid,
                         KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( smpFix( sTbsId,
                     sFirstPid,
                     &sPageHandle,
                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlHeapify( &sHeapQueueInfo,
                         aHeapQueueContext,
                         sHeapQueueArray,
                         SMN_MAX_MERGED_RUN_LIST * 2,
                         aSortBlockCount,
                         KNL_ENV(aEnv) )
             == STL_SUCCESS );

    while( STL_TRUE )
    {
        STL_TRY( knlGetMinHeap( &sHeapQueueInfo,
                                (void**)&sSrcKey,
                                &sIteratorIdx,
                                KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sSrcKey == NULL )
        {
            break;
        }
    
        SMNMPB_RUN_KEY_GET_TOTAL_SIZE( sSrcKey, &sKeySize );

        STL_TRY( smpAllocSlot( &sPageHandle,
                               sKeySize,
                               STL_FALSE,
                               &sDstKey,
                               &sSlotId )
                 == STL_SUCCESS );

        if( sDstKey == NULL )
        {
            sState = 0;
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

            sPageSeq++;
            
            if( sPageSeq >= aBuildExtentPageCount )
            {
                /**
                 * 더이상 저장할 공간이 없다면 연산을 멈춘다.
                 * - Min Heap Array는 Heap Tree에 참여한 다음 Key부터 active slot으로 간주한다.
                 *   따라서 Heap의 모든 block이 write되지 않은 상태에서 연산이 멈추면
                 *   Heap Tree에 참여한 key들은 compact대상이 아니어서 key를 잃어버리게 된다.
                 *   이를 위해 empty가 아닌 block들의 active slot을 보정한다.
                 * - Min Heap에서 꺼내온 Key도 active slot이 아니므로 추가로 보정한다.
                 */

                sIdx = sHeapQueueInfo.mArraySize >> 1;

                for( i = 0; i < sHeapQueueInfo.mRunCount; i++, sIdx++ )
                {
                    sIterator = &(((smnmpbSortIterator *)(aHeapQueueContext->mIterator))[i]);

                    if( sHeapQueueArray[sIdx].mIteratorIdx != -1 )
                    {
                        sIterator->mSortBlockHdr->mActiveSlotIdx--;
                    }

                    if( i == sIteratorIdx )
                    {
                        sIterator->mSortBlockHdr->mActiveSlotIdx--;
                    }
                }

                break;
            }
            
            STL_TRY( smpFix( sTbsId,
                             sFirstPid + sPageSeq,
                             &sPageHandle,
                             aEnv )
                     == STL_SUCCESS );
            sState = 1;

            STL_TRY( smpAllocSlot( &sPageHandle,
                                   sKeySize,
                                   STL_FALSE,
                                   &sDstKey,
                                   &sSlotId )
                     == STL_SUCCESS );

            STL_ASSERT( sDstKey != NULL );
        }

        stlMemcpy( sDstKey, sSrcKey, sKeySize );
    }

    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }
    
    return STL_FAILURE;
}

stlStatus smnmpbCompactSortBlock( smnmpbSortBlockHdr   * aSortBlockHdr,
                                  stlChar            *** aSortBlock,
                                  stlInt32               aSortBlockCount,
                                  smlEnv               * aEnv )
{
    stlChar          ** sSrcSortBlock;
    stlChar          ** sDstSortBlock;
    stlChar           * sSrcKey;
    stlChar           * sDstKey;
    stlInt16            sKeySize;
    smnmpbSortOffset  * sDstOffsetList;
    smnmpbSortOffset  * sSrcOffsetList;
    stlInt64            sKeyCount;
    stlInt64            i;
    stlInt64            j;
    stlInt64            k;
    stlInt64            sActiveSlotIdx;

    sSrcSortBlock = aSortBlock[0];
    sDstSortBlock = aSortBlock[1];

    for( i = 0; i < aSortBlockCount; i++ )
    {
        sSrcOffsetList = (smnmpbSortOffset*)sSrcSortBlock[i];
        sDstOffsetList = (smnmpbSortOffset*)sDstSortBlock[i];
    
        sKeyCount = aSortBlockHdr[i].mKeyCount;
        sActiveSlotIdx = aSortBlockHdr[i].mActiveSlotIdx;

        aSortBlockHdr[i].mKeyCount = 0;
        aSortBlockHdr[i].mLowWaterMark = aSortBlockHdr[i].mTotalSize;
        aSortBlockHdr[i].mHighWaterMark = 0;
        aSortBlockHdr[i].mActiveSlotIdx = 0;
    
        for( j = sActiveSlotIdx, k = 0; j < sKeyCount; j++, k++ )
        {
            sSrcKey = sSrcSortBlock[i] + sSrcOffsetList[j];
            SMNMPB_RUN_KEY_GET_TOTAL_SIZE( sSrcKey, &sKeySize );

            sDstKey = sDstSortBlock[i] + aSortBlockHdr[i].mLowWaterMark - sKeySize;
            sDstOffsetList[k] = aSortBlockHdr[i].mLowWaterMark - sKeySize;
        
            stlMemcpy( sDstKey, sSrcKey, sKeySize );

            aSortBlockHdr[i].mHighWaterMark += STL_SIZEOF(smnmpbSortOffset);
            aSortBlockHdr[i].mLowWaterMark -= sKeySize;
            aSortBlockHdr[i].mKeyCount++;
        }
    }

    aSortBlock[0] = sDstSortBlock;
    aSortBlock[1] = sSrcSortBlock;
    
    return STL_SUCCESS;
}

stlStatus smnmpbSortFetchNext( void      * aIterator,
                               stlInt64    aIteratorIdx,
                               void     ** aItem,
                               knlEnv    * aEnv )
{
    smnmpbSortIterator * sIterator;
    void               * sItem = NULL;
    smnmpbSortOffset   * sOffsetList;
    smnmpbSortBlockHdr * sSortBlockHdr;

    sIterator = &(((smnmpbSortIterator*)aIterator)[aIteratorIdx]);
    sSortBlockHdr = sIterator->mSortBlockHdr;
    sOffsetList = (smnmpbSortOffset*)(sIterator->mSortBlock);

    if( sSortBlockHdr->mActiveSlotIdx < sSortBlockHdr->mKeyCount )
    {
        sItem = sIterator->mSortBlock + sOffsetList[sSortBlockHdr->mActiveSlotIdx];
        sSortBlockHdr->mActiveSlotIdx++;
    }
    else
    {
        /**
         * 더 이상 Key가 없다.
         */
    }

    *aItem = sItem;

    return STL_SUCCESS;
}

stlStatus smnmpbBuildFetchNext( void      * aIterator,
                                stlInt64    aIteratorIdx,
                                void     ** aItem,
                                knlEnv    * aEnv )
{
    smnmpbBuildIterator * sIterator;
    void                * sItem = NULL;

    sIterator = &(((smnmpbBuildIterator*)aIterator)[aIteratorIdx]);

    if( sIterator->mPageSeq == -1 )
    {
        if( sIterator->mNextExtentPid != SMP_NULL_PID )
        {
            STL_TRY( smpFix( sIterator->mTbsId,
                             sIterator->mNextExtentPid,
                             &sIterator->mPageHandle,
                             SML_ENV(aEnv) )
                     == STL_SUCCESS );

            sIterator->mCurExtentPid = sIterator->mNextExtentPid;
            sIterator->mNextExtentPid = SMNMPB_GET_NEXT_EXTENT( &sIterator->mPageHandle );
            sIterator->mPageSeq = 0;

            SMP_GET_FIRST_ROWITEM( SMP_FRAME( &sIterator->mPageHandle ),
                                   &sIterator->mRowItemIterator,
                                   sItem );
            
            if( sItem == NULL )
            {
                STL_TRY( smpUnfix( &sIterator->mPageHandle,
                                   SML_ENV(aEnv) )
                         == STL_SUCCESS );
                SMP_MAKE_DUMMY_HANDLE( &sIterator->mPageHandle, NULL );
                
                STL_TRY( knlEnqueue( sIterator->mFreeQueueInfo,
                                     (void*)&(sIterator->mCurExtentPid),
                                     KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
        }
        else
        {
            /**
             * 더 이상 Key가 없다.
             */
        }
    }
    else
    {
        SMP_GET_NEXT_ROWITEM( &sIterator->mRowItemIterator, sItem );

        if( sItem == NULL )
        {
            STL_TRY( smpUnfix( &sIterator->mPageHandle,
                               SML_ENV(aEnv) )
                     == STL_SUCCESS );
            SMP_MAKE_DUMMY_HANDLE( &sIterator->mPageHandle, NULL );

            while( 1 )
            {
                sIterator->mPageSeq++;
                
                if( sIterator->mPageSeq >= sIterator->mBuildExtentPageCount )
                {
                    STL_TRY( knlEnqueue( sIterator->mFreeQueueInfo,
                                         (void*)&(sIterator->mCurExtentPid),
                                         KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                
                    if( sIterator->mNextExtentPid != SMP_NULL_PID )
                    {
                        STL_TRY( smpFix( sIterator->mTbsId,
                                         sIterator->mNextExtentPid,
                                         &sIterator->mPageHandle,
                                         SML_ENV(aEnv) )
                                 == STL_SUCCESS );

                        sIterator->mCurExtentPid = sIterator->mNextExtentPid;
                        sIterator->mNextExtentPid = SMNMPB_GET_NEXT_EXTENT( &sIterator->mPageHandle );
                        sIterator->mPageSeq = 0;
                    
                        SMP_GET_FIRST_ROWITEM( SMP_FRAME( &sIterator->mPageHandle ),
                                               &sIterator->mRowItemIterator,
                                               sItem );
                    
                        if( sItem != NULL )
                        {
                            break;
                        }

                        STL_TRY( smpUnfix( &sIterator->mPageHandle,
                                           SML_ENV(aEnv) )
                                 == STL_SUCCESS );
                        SMP_MAKE_DUMMY_HANDLE( &sIterator->mPageHandle, NULL );
                    }
                    else
                    {
                        /**
                         * 더 이상 Key가 없다.
                         */
                        break;
                    }
                }
                else
                {
                    STL_TRY( smpFix( sIterator->mTbsId,
                                     sIterator->mCurExtentPid + sIterator->mPageSeq,
                                     &sIterator->mPageHandle,
                                     SML_ENV(aEnv) )
                             == STL_SUCCESS );
                
                    SMP_GET_FIRST_ROWITEM( SMP_FRAME( &sIterator->mPageHandle ),
                                           &sIterator->mRowItemIterator,
                                           sItem );
                
                    if( sItem != NULL )
                    {
                        break;
                    }
                    
                    STL_TRY( smpUnfix( &sIterator->mPageHandle,
                                       SML_ENV(aEnv) )
                             == STL_SUCCESS );
                    SMP_MAKE_DUMMY_HANDLE( &sIterator->mPageHandle, NULL );
                }
            }
        }
    }

    *aItem = sItem;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbGroupingFetchNext( void      * aIterator,
                                   stlInt64    aIteratorIdx,
                                   void     ** aItem,
                                   knlEnv    * aEnv )
{
    smnmpbBuildIterator * sIterator;
    void                * sItem = NULL;
    stlInt32              sTotalKeyCount;
    stlInt32              i;
    smlPid                sNextExtentPid;
    smpHandle             sPageHandle;

    sIterator = &(((smnmpbBuildIterator*)aIterator)[aIteratorIdx]);

    if( sIterator->mNextExtentPid != SMP_NULL_PID )
    {
        if( SMP_IS_DUMMY_HANDLE( &sIterator->mPageHandle ) == STL_FALSE )
        {
            STL_TRY( smpUnfix( &sIterator->mPageHandle,
                               SML_ENV(aEnv) )
                     == STL_SUCCESS );
            SMP_MAKE_DUMMY_HANDLE( &sIterator->mPageHandle, NULL );
        }
    
        for( i = (sIterator->mBuildExtentPageCount - 1); i >= 0; i-- )
        {
            STL_TRY( smpFix( sIterator->mTbsId,
                             sIterator->mNextExtentPid + i,
                             &sIterator->mPageHandle,
                             SML_ENV(aEnv) )
                     == STL_SUCCESS );

            sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sIterator->mPageHandle );
                
            if( sTotalKeyCount == 0 )
            {
                STL_TRY( smpUnfix( &sIterator->mPageHandle,
                                   SML_ENV(aEnv) )
                         == STL_SUCCESS );
                SMP_MAKE_DUMMY_HANDLE( &sIterator->mPageHandle, NULL );
                
                continue;
            }

            sItem = smpGetNthRowItem( &sIterator->mPageHandle, sTotalKeyCount - 1 );
            break;
        }

        STL_DASSERT( sItem != NULL );

        STL_TRY( smpFix( sIterator->mTbsId,
                         sIterator->mNextExtentPid,
                         &sPageHandle,
                         SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sNextExtentPid = SMNMPB_GET_NEXT_EXTENT( &sPageHandle );
        
        STL_TRY( smpUnfix( &sPageHandle, SML_ENV(aEnv) ) == STL_SUCCESS );
        
        sIterator->mCurExtentPid = sIterator->mNextExtentPid;
        sIterator->mNextExtentPid = sNextExtentPid;
    }
    else
    {
        /**
         * 더 이상 Key가 없다.
         */
        
        sIterator->mCurExtentPid = SMP_NULL_PID;
        
        if( SMP_IS_DUMMY_HANDLE( &sIterator->mPageHandle ) == STL_FALSE )
        {
            STL_TRY( smpUnfix( &sIterator->mPageHandle,
                               SML_ENV(aEnv) )
                     == STL_SUCCESS );
            SMP_MAKE_DUMMY_HANDLE( &sIterator->mPageHandle, NULL );
        }
    }
    
    *aItem = sItem;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbWriteKeyToMergedRun( smnmpbParallelArgs     * aArgs,
                                     knlQueueInfo           * aRunQueueInfo,
                                     smnmpbMergeRunIterator * aMergeIterator,
                                     void                   * aKey,
                                     knlQueueInfo           * aFreeQueueInfo,
                                     smlEnv                 * aEnv )
{
    smlPid        sExtentPid;
    stlInt16      sKeySize;
    stlInt16      sSlotId;
    stlChar     * sDstKey = NULL;
    smpHandle     sPageHandle;
    
    SMNMPB_RUN_KEY_GET_TOTAL_SIZE( aKey, &sKeySize );
    
    if( aMergeIterator->mPageSeq == -1 )
    {
        /**
         * Writing first key
         */
        
        STL_TRY( smnmpbAllocExtent4Build( aArgs->mTransId,
                                          aMergeIterator->mBuildExtentPageCount,
                                          SME_GET_SEGMENT_HANDLE( aArgs->mIndexHandle ),
                                          aFreeQueueInfo,
                                          &sExtentPid,
                                          aEnv )
                 == STL_SUCCESS );
        
        if( aMergeIterator->mFirstExtentPid == SMP_NULL_PID )
        {
            aMergeIterator->mFirstExtentPid = sExtentPid;
        }
    
        aMergeIterator->mPrevExtentPid = aMergeIterator->mCurExtentPid;
        aMergeIterator->mCurExtentPid = sExtentPid;
        aMergeIterator->mPageSeq = 0;

        STL_TRY( smpFix( aMergeIterator->mTbsId,
                         sExtentPid,
                         &aMergeIterator->mPageHandle,
                         aEnv )
                 == STL_SUCCESS );

        SMNMPB_SET_NEXT_EXTENT( &aMergeIterator->mPageHandle, SMP_NULL_PID );
        aArgs->mTotalRunCount += 1;
    }
    else
    {
        /**
         * Writing next keys
         */
        
        STL_TRY( smpAllocSlot( &aMergeIterator->mPageHandle,
                               sKeySize,
                               STL_FALSE,
                               &sDstKey,
                               &sSlotId )
                 == STL_SUCCESS );

        if( sDstKey == NULL )
        {
            STL_TRY( smpUnfix( &aMergeIterator->mPageHandle, aEnv ) == STL_SUCCESS );

            aMergeIterator->mPageSeq++;
            
            if( aMergeIterator->mPageSeq >= aMergeIterator->mBuildExtentPageCount )
            {
                STL_TRY( smnmpbAllocExtent4Build( aArgs->mTransId,
                                                  aMergeIterator->mBuildExtentPageCount,
                                                  SME_GET_SEGMENT_HANDLE( aArgs->mIndexHandle ),
                                                  aFreeQueueInfo,
                                                  &sExtentPid,
                                                  aEnv )
                         == STL_SUCCESS );
                aArgs->mTotalRunCount += 1;

                /**
                 * Linking Merged Run
                 */
                
                STL_TRY( smpFix( aMergeIterator->mTbsId,
                                 aMergeIterator->mCurExtentPid,
                                 &sPageHandle,
                                 aEnv )
                         == STL_SUCCESS );

                SMNMPB_SET_NEXT_EXTENT( &sPageHandle, sExtentPid );

                STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

                /**
                 * Next Extent로 이동
                 */
                
                aMergeIterator->mPrevExtentPid = aMergeIterator->mCurExtentPid;
                aMergeIterator->mCurExtentPid = sExtentPid;
                aMergeIterator->mPageSeq = 0;

                STL_TRY( smpFix( aMergeIterator->mTbsId,
                                 sExtentPid,
                                 &aMergeIterator->mPageHandle,
                                 aEnv )
                         == STL_SUCCESS );
                
                SMNMPB_SET_NEXT_EXTENT( &aMergeIterator->mPageHandle, SMP_NULL_PID );
            }
            else
            {
                STL_TRY( smpFix( aMergeIterator->mTbsId,
                                 aMergeIterator->mCurExtentPid + aMergeIterator->mPageSeq,
                                 &aMergeIterator->mPageHandle,
                                 aEnv )
                         == STL_SUCCESS );
            }
        }
    }

    if( sDstKey == NULL )
    {
        STL_TRY( smpAllocSlot( &aMergeIterator->mPageHandle,
                               sKeySize,
                               STL_FALSE,
                               &sDstKey,
                               &sSlotId )
                 == STL_SUCCESS );

        STL_DASSERT( sDstKey != NULL );
    }

    stlMemcpy( sDstKey, aKey, sKeySize );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbMergeKeys( smnmpbParallelArgs  * aArgs,
                           knlQueueInfo        * aRunQueueInfo,
                           smlPid              * aMergedRunList,
                           smnmpbCompareInfo   * aCompareInfo,
                           knlQueueInfo        * aFreeQueueInfo,
                           smlEnv              * aEnv )
{
    knlHeapQueueEntry       sHeapQueueArray[SMN_MAX_MERGED_RUN_LIST*2];
    knlHeapQueueContext     sHeapQueueContext;
    knlHeapQueueInfo        sHeapQueueInfo;
    smnmpbBuildIterator     sIterator[SMN_MAX_MERGED_RUN_LIST];
    stlInt64                sQueueSize;
    stlInt32                sRunCount = 0;
    stlInt32                i;
    stlBool                 sIsEmpty;
    stlChar               * sKey;
    smnmpbMergeRunIterator  sMergeIterator;
    smlPid                  sExtentPid;
    stlInt32                sState = 0;
    
    sHeapQueueContext.mCompare = aArgs->mMergeCompare;
    sHeapQueueContext.mFetchNext = smnmpbBuildFetchNext;
    sHeapQueueContext.mCompareInfo = aCompareInfo;
    sHeapQueueContext.mIterator = &sIterator;

    aArgs->mTotalRunCount = aArgs->mCutOffRunCount;
    
    while( 1 )
    {
        sQueueSize = knlGetQueueSize( aRunQueueInfo );

        /**
         * 정해진 개수 미만으로 Merge run이 남았다면
         */
        
        if( knlGetQueueSize( aRunQueueInfo ) <= aArgs->mCutOffRunCount )
        {
            break;
        }

        /**
         * Merge Run 개수 결정
         */
        
        sRunCount = aArgs->mMaxMergeRunCount;
        aArgs->mTotalRunCount = 0;
        
        if( sQueueSize < sRunCount )
        {
            sRunCount = (sQueueSize - aArgs->mCutOffRunCount) + 1;
        }

        /**
         * Merge runs
         */

        for( i = 0; i < sRunCount; i++ )
        {
            STL_TRY( knlDequeue( aRunQueueInfo,
                                 &sIterator[i].mNextExtentPid,
                                 &sIsEmpty,
                                 KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sIterator[i].mTbsId = aArgs->mTbsId;
            sIterator[i].mBuildExtentPageCount = aArgs->mBuildExtentPageCount;
            sIterator[i].mCurExtentPid = SMP_NULL_PID;
            sIterator[i].mPageSeq = -1;
            sIterator[i].mFreeQueueInfo = aFreeQueueInfo;

            STL_DASSERT( sIsEmpty != STL_TRUE );
        }
        
        sMergeIterator.mTbsId = aArgs->mTbsId;
        sMergeIterator.mFirstExtentPid = SMP_NULL_PID;
        sMergeIterator.mCurExtentPid = SMP_NULL_PID;
        sMergeIterator.mPrevExtentPid = SMP_NULL_PID;
        sMergeIterator.mPageSeq = -1;
        SMP_MAKE_DUMMY_HANDLE( &sMergeIterator.mPageHandle, NULL );
        sMergeIterator.mBuildExtentPageCount = aArgs->mBuildExtentPageCount;

        STL_TRY( knlHeapify( &sHeapQueueInfo,
                             &sHeapQueueContext,
                             sHeapQueueArray,
                             SMN_MAX_MERGED_RUN_LIST * 2,
                             sRunCount,
                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 1;

        while( STL_TRUE )
        {
            STL_TRY( knlGetMinHeap( &sHeapQueueInfo,
                                    (void**)&sKey,
                                    NULL,
                                    KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( sKey == NULL )
            {
                break;
            }
            
            STL_TRY( smnmpbWriteKeyToMergedRun( aArgs,
                                                aRunQueueInfo,
                                                &sMergeIterator,
                                                sKey,
                                                aFreeQueueInfo,
                                                aEnv )
                     == STL_SUCCESS );
            
            /**
             * Parent Event 검사
             */

            STL_TRY( knlCheckParentQueryTimeout( KNL_ENV(aArgs->mParentEnv),
                                                 KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        sState = 0;

        if( SMP_IS_DUMMY_HANDLE( &sMergeIterator.mPageHandle ) == STL_FALSE )
        {
            STL_TRY( smpUnfix( &sMergeIterator.mPageHandle,
                               aEnv ) == STL_SUCCESS );
            SMP_MAKE_DUMMY_HANDLE( &sMergeIterator.mPageHandle, NULL );
        }

        STL_TRY_THROW( (aArgs->mIsUniqueIndex != STL_TRUE) ||
                       (aCompareInfo->mUniqueViolated == 0),
                       RAMP_ERR_UNIQUE_VIOLATED );
        
        STL_TRY( knlEnqueue( aRunQueueInfo,
                             (void*)&(sMergeIterator.mFirstExtentPid),
                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * Fill Merged Run List
     */

    i = 0;
    while( 1 )
    {
        STL_TRY( knlDequeue( aRunQueueInfo,
                             (void*)&sExtentPid,
                             &sIsEmpty,
                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sIsEmpty == STL_TRUE )
        {
            break;
        }
        
        aMergedRunList[i++] = sExtentPid;
    }
    
    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_UNIQUE_VIOLATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INDEX_UNIQUENESS_VIOLATION,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        if( SMP_IS_DUMMY_HANDLE( &sMergeIterator.mPageHandle ) == STL_FALSE )
        {
            (void) smpUnfix( &sMergeIterator.mPageHandle, aEnv );
        }

        for( i = 0; i < sRunCount; i++ )
        {
            if( SMP_IS_DUMMY_HANDLE( &sIterator[i].mPageHandle ) == STL_FALSE )
            {
                (void) smpUnfix( &sIterator[i].mPageHandle, aEnv );
            }
        }
    }
    
    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );
    
    return STL_FAILURE;
}


stlStatus smnmpbFreeExtents( smxlTransId    aTransId,
                             void         * aIndexHandle,
                             stlInt32       aPageCount,
                             knlQueueInfo * aFreeQueueInfo,
                             smlEnv       * aEnv )
{
    void      * sSegmentHandle;
    smlPid      sExtentPid;
    stlBool     sIsEmpty;
    stlInt32    i;
    smxmTrans   sMiniTrans;
    stlUInt32   sAttr = SMXM_ATTR_REDO;
    smpHandle   sPageHandle;
    stlInt32    sState = 0;

    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aIndexHandle );
    
    while( 1 )
    {
        STL_TRY( knlDequeue( aFreeQueueInfo,
                             (void*)&sExtentPid,
                             &sIsEmpty,
                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sIsEmpty == STL_TRUE )
        {
            break;
        }

        for( i = 0; i < aPageCount; i++ )
        {
            STL_TRY( smxmBegin( &sMiniTrans,
                                aTransId,
                                smsGetSegRid(sSegmentHandle),
                                sAttr | SMXM_ATTR_NO_VALIDATE_PAGE,
                                aEnv ) == STL_SUCCESS );
            sState = 1;

            STL_TRY( smpAcquire( &sMiniTrans,
                                 smsGetTbsId(sSegmentHandle),
                                 sExtentPid + i,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv )
                     == STL_SUCCESS );
            
            STL_TRY( smsUpdatePageStatus( &sMiniTrans,
                                          sSegmentHandle,
                                          &sPageHandle,
                                          SMP_FREENESS_FREE,
                                          0,  /* aScn */
                                          aEnv )
                     == STL_SUCCESS );

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans,
                                 aEnv )
                     == STL_SUCCESS);
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smnmpbBuildTree( smnmpbParallelArgs  * aArgs,
                           smlEnv              * aEnv )
{
    knlHeapQueueEntry        sHeapQueueArray[SMN_MAX_MERGED_RUN_LIST*2];
    void                   * sSegmentHandle;
    knlHeapQueueContext      sHeapQueueContext;
    knlHeapQueueInfo         sHeapQueueInfo;
    smnmpbBuildIterator      sIterator[SMN_MAX_MERGED_RUN_LIST];
    stlInt32                 i;
    stlChar                * sKey;
    smnmpbCompareInfo        sCompareInfo;
    dtlDataType              sDataType;
    smnmpbTreeIterator       sTreeIterator;
    smxmTrans                sMiniTrans;
    stlInt32                 sState = 0;
    smpHandle                sPageHandle;
    stlInt32                 sAttr;
    smlRid                   sSegmentRid;
    stlInt16                 sHeight = 0;
    knlQueueInfo             sFreeQueueInfo;
    smlPid                   sMergedRunList[SMN_MAX_MERGED_RUN_LIST];
    stlInt64                 sMergedRunCount = 0;
    stlBool                  sIsEmpty = STL_TRUE;
    smlPid                   sExtentPid;
    knlValueBlockList      * sSrcColumn = NULL;

    sTreeIterator.mMiniTransState = 0;
    
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aArgs->mIndexHandle );
    sSegmentRid = smsGetSegRid( sSegmentHandle );

    if( aArgs->mIsLogging == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }
    
    /**
     * Fill Free Page Queue
     */

    STL_TRY( knlInitQueue( &sFreeQueueInfo,
                           1024,
                           STL_SIZEOF(smlPid),
                           &aEnv->mDynamicHeapMem,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    for( i = 0; i < SMNMPB_MAX_FREE_EXTENT_COUNT; i++ )
    {
        if( aArgs->mFreeExtentArray[i] == SMP_NULL_PID )
        {
            continue;
        }

        STL_TRY( knlEnqueue( &sFreeQueueInfo,
                             (void*)&(aArgs->mFreeExtentArray[i]),
                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        aArgs->mFreeExtentArray[i] = SMP_NULL_PID;
    }

    /**
     * refine merge run list
     */

    stlMemset( sMergedRunList, 0x00, STL_SIZEOF(smlPid) * SMN_MAX_MERGED_RUN_LIST );
    
    for( i = 0; i < SMN_MAX_MERGED_RUN_LIST; i++ )
    {
        if( aArgs->mMergedRunList[i] == SMP_NULL_PID )
        {
            continue;
        }

        sMergedRunList[sMergedRunCount] = aArgs->mMergedRunList[i];
        sMergedRunCount++;
    }
    
    
    /**
     * Initialize Structures
     */
    
    aArgs->mHeight = 0;
    stlMemset( aArgs->mPageId,
               0x00,
               (SMNMPB_MAX_INDEX_DEPTH + 1) * STL_SIZEOF(smlPid) );
    stlMemset( aArgs->mFirstPageId,
               0x00,
               (SMNMPB_MAX_INDEX_DEPTH + 1) * STL_SIZEOF(smlPid) );

    sSrcColumn = aArgs->mIndexColList;
    i = 0;
    
    while( sSrcColumn != NULL )
    {
        sDataType = KNL_GET_BLOCK_DB_TYPE( sSrcColumn );
        sCompareInfo.mPhysicalCompare[i] = dtlPhysicalCompareFuncList[sDataType][sDataType];
        sSrcColumn = sSrcColumn->mNext;
        i++;
    }
    
    sCompareInfo.mSortBlock = NULL;
    sCompareInfo.mCompareCount = 0;
    sCompareInfo.mUniqueViolated = 0;
    sCompareInfo.mKeyColCount = aArgs->mKeyColCount;
    sCompareInfo.mKeyColFlags = aArgs->mKeyColFlags;
    sCompareInfo.mKeyColOrder = aArgs->mKeyColOrder;
    
    sHeapQueueContext.mCompare = aArgs->mMergeCompare;
    sHeapQueueContext.mFetchNext = smnmpbBuildFetchNext;
    sHeapQueueContext.mCompareInfo = &sCompareInfo;
    sHeapQueueContext.mIterator = &sIterator;

    for( i = 0; i < sMergedRunCount; i++ )
    {
        sIterator[i].mTbsId = aArgs->mTbsId;
        sIterator[i].mBuildExtentPageCount = aArgs->mBuildExtentPageCount;
        sIterator[i].mNextExtentPid = sMergedRunList[i];
        sIterator[i].mCurExtentPid = SMP_NULL_PID;
        sIterator[i].mPageSeq = -1;
        sIterator[i].mFreeQueueInfo = &sFreeQueueInfo;
        SMP_MAKE_DUMMY_HANDLE( &sIterator[i].mPageHandle, NULL );
    }

    sTreeIterator.mCompareInfo = &sCompareInfo;
    sTreeIterator.mTransId = aArgs->mTransId;
    sTreeIterator.mTbsId = aArgs->mTbsId;
    sTreeIterator.mMiniTrans = &sMiniTrans;
    sTreeIterator.mMiniTransState = 0;
    sTreeIterator.mMiniTransAttr = sAttr;
    sTreeIterator.mIndexHandle = aArgs->mIndexHandle;
    sTreeIterator.mHeight = 0;
    sTreeIterator.mPageSeq = 0;
    sTreeIterator.mKeyColCount = aArgs->mKeyColCount;
    sTreeIterator.mKeyColFlags = aArgs->mKeyColFlags;
    sTreeIterator.mKeyColTypes = aArgs->mKeyColTypes;
    sTreeIterator.mFreeQueueInfo = &sFreeQueueInfo;
    sTreeIterator.mExtentPid = SMP_NULL_PID;
    sTreeIterator.mBuildExtentPageCount = aArgs->mBuildExtentPageCount;
    SMP_MAKE_DUMMY_HANDLE( &sTreeIterator.mLeafPageHandle, NULL );

    stlMemset( sTreeIterator.mPageId,
               0x00,
               (SMNMPB_MAX_INDEX_DEPTH + 1) * STL_SIZEOF(smlPid) );
    stlMemset( sTreeIterator.mFirstPageId,
               0x00,
               (SMNMPB_MAX_INDEX_DEPTH + 1) * STL_SIZEOF(smlPid) );

    /**
     * Building Tree
     */
    
    STL_TRY( knlHeapify( &sHeapQueueInfo,
                         &sHeapQueueContext,
                         sHeapQueueArray,
                         SMN_MAX_MERGED_RUN_LIST * 2,
                         sMergedRunCount,
                         KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( smxmBegin( sTreeIterator.mMiniTrans,
                        sTreeIterator.mTransId,
                        sSegmentRid,
                        sTreeIterator.mMiniTransAttr,
                        aEnv )
             == STL_SUCCESS );
    sTreeIterator.mMiniTransState = 1;
    
    while( STL_TRUE )
    {
        STL_TRY( knlGetMinHeap( &sHeapQueueInfo,
                                (void**)&sKey,
                                NULL,
                                KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sKey == NULL )
        {
            break;
        }

        STL_TRY( smnmpbWriteKeyToTree( aArgs,
                                       &sTreeIterator,
                                       sKey,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * Parent Event 검사
         */

        STL_TRY( knlCheckParentQueryTimeout( KNL_ENV(aArgs->mParentEnv),
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    if( sTreeIterator.mMiniTransState == 1 )
    {
        sTreeIterator.mMiniTransState = 0;
        STL_TRY( smxmCommit( sTreeIterator.mMiniTrans, aEnv ) == STL_SUCCESS );
    }

    /**
     * Free pages
     */

    if( sTreeIterator.mExtentPid != SMP_NULL_PID )
    {
        while( sTreeIterator.mPageSeq < sTreeIterator.mBuildExtentPageCount )
        {
            STL_TRY( smxmBegin( &sMiniTrans,
                                aArgs->mTransId,
                                sSegmentRid,
                                sAttr | SMXM_ATTR_NO_VALIDATE_PAGE,
                                aEnv )
                     == STL_SUCCESS );
            sState = 2;
    
            STL_TRY( smpAcquire( &sMiniTrans,
                                 aArgs->mTbsId,
                                 sTreeIterator.mExtentPid + sTreeIterator.mPageSeq,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );

            STL_TRY( smsUpdatePageStatus( &sMiniTrans,
                                          sSegmentHandle,
                                          &sPageHandle,
                                          SMP_FREENESS_FREE,
                                          0,  /* aScn */
                                          aEnv )
                     == STL_SUCCESS );

            sState = 1;
            STL_TRY( smxmCommit( &sMiniTrans,
                                 aEnv )
                     == STL_SUCCESS);

            sTreeIterator.mPageSeq++;
        }
    }

    /**
     * Tree 결과를 return하기 위해서...
     */

    sHeight = 0;

    while( sTreeIterator.mPageId[sHeight] != SMP_NULL_PID )
    {
        aArgs->mPageId[sHeight] = sTreeIterator.mPageId[sHeight];
        aArgs->mFirstPageId[sHeight] = sTreeIterator.mFirstPageId[sHeight];
        sHeight++;
    }

    aArgs->mHeight = sHeight - 1;
    
    /**
     * Fill Free Extent Array
     */

    for( i = 0; i < SMNMPB_MAX_FREE_EXTENT_COUNT; i++ )
    {
        STL_TRY( knlDequeue( &sFreeQueueInfo,
                             (void*)&sExtentPid,
                             &sIsEmpty,
                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sIsEmpty == STL_TRUE )
        {
            break;
        }
        
        aArgs->mFreeExtentArray[i] = sExtentPid;
    }

    /**
     * 아직도 Free Extent가 남아 있다면 Segment로 반납한다.
     */

    STL_TRY( smnmpbFreeExtents( aArgs->mTransId,
                                aArgs->mIndexHandle,
                                sTreeIterator.mBuildExtentPageCount,
                                &sFreeQueueInfo,
                                aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFiniQueue( &sFreeQueueInfo,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );

    return STL_SUCCESS;

    STL_FINISH;

    if( sTreeIterator.mMiniTransState == 1 )
    {
        (void)smxmCommit( sTreeIterator.mMiniTrans, aEnv );
    }

    switch( sState )
    {
        case 2:
            (void) smxmCommit( &sMiniTrans, aEnv );
        case 1:
            (void) knlFiniQueue( &sFreeQueueInfo, KNL_ENV(aEnv) );
        default:
            break;
    }

    for( i = 0; i < sMergedRunCount; i++ )
    {
        if( SMP_IS_DUMMY_HANDLE( &sIterator[i].mPageHandle ) == STL_FALSE )
        {
            (void) smpUnfix( &sIterator[i].mPageHandle, aEnv );
        }
    }

    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );
    
    return STL_FAILURE;
}

stlStatus smnmpbFinalizeBuildTree( smnmpbParallelArgs  * aArgs,
                                   smlEnv              * aEnv )
{
    knlQueueInfo             sFreeQueueInfo;
    void                   * sSegmentHandle;
    smlRid                   sSegmentRid;
    stlInt32                 sAttr;
    stlInt32                 sState = 0;
    stlInt32                 sQueueState = 0;
    stlInt16                 sHeight = 0;
    smpHandle                sPageHandle;
    smpHandle                sRootPageHandle;
    smpHandle                sMirrorPageHandle;
    stlChar                * sKeySlot;
    stlInt16                 sSlotId;
    smnIndexHeader         * sHeader;
    stlUInt64                sSmoNo = 0;
    smnmpbLogicalHdr         sLogicalHdr;
    smxmTrans                sMiniTrans;
    stlInt32                 i;
    stlInt64                 sMirrorDepth;
    
    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );

    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aArgs->mIndexHandle );
    sSegmentRid = smsGetSegRid( sSegmentHandle );
    
    if( aArgs->mIsLogging == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }
    
    /**
     * Logging right-most nodes & Set max key
     */
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        aArgs->mTransId,
                        sSegmentRid,
                        sAttr,
                        aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    while( aArgs->mPageId[sHeight] != SMP_NULL_PID )
    {
        STL_TRY( smpAcquire( &sMiniTrans,
                             aArgs->mTbsId,
                             aArgs->mPageId[sHeight],
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv )
                 == STL_SUCCESS );

        if( sHeight > 0 )
        {
            SMNMPB_GET_LOGICAL_HEADER(&sPageHandle)->mFlag |= SMNMPB_NODE_FLAG_LAST_SIBLING;

            STL_TRY( smpAllocSlot( &sPageHandle,
                                   STL_SIZEOF(smlPid),
                                   STL_FALSE,
                                   &sKeySlot,
                                   &sSlotId ) == STL_SUCCESS );
            
            SMNMPB_INTL_SLOT_SET_CHILD_PID( sKeySlot, &aArgs->mPageId[sHeight-1] );
            SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(&sPageHandle) );
        }
            
        STL_TRY( smxmWriteLog( &sMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                               SMR_REDO_TARGET_PAGE,
                               SMP_FRAME(&sPageHandle),
                               SMP_PAGE_SIZE,
                               aArgs->mTbsId,
                               smpGetPageId(&sPageHandle),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

        sHeight++;
    }

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );

    /**
     * 세그먼트 헤더에 인덱스 루트 페이지를 설정한다.
     */

    STL_TRY( smxmBegin( &sMiniTrans,
                        aArgs->mTransId,
                        sSegmentRid,
                        sAttr,
                        aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smpAcquire( &sMiniTrans,
                         sSegmentRid.mTbsId,
                         sSegmentRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sHeader = (smnIndexHeader*)((stlChar*)smpGetBody(SMP_FRAME(&sPageHandle)) +
                                smsGetHeaderSize(sSegmentHandle));
    sHeader->mRootPageId = aArgs->mPageId[aArgs->mHeight];
    sHeader->mMirrorRootPid = SMP_NULL_PID;
    sHeader->mEmptyNodeCount = 0;
    sHeader->mFirstEmptyNode = SMP_NULL_PID;
    sHeader->mLastEmptyNode  = SMP_NULL_PID;

    SMN_SET_ROOT_PAGE_ID( aArgs->mIndexHandle,
                          sHeader->mRootPageId );
    SMN_SET_MIRROR_ROOT_PID( aArgs->mIndexHandle,
                             sHeader->mMirrorRootPid );
    
    if( sHeader->mRootPageId == SMP_NULL_PID )
    {
        STL_TRY( smsAllocPage( &sMiniTrans,
                               sSegmentHandle,
                               SMP_PAGE_TYPE_INDEX_DATA,
                               NULL,  /* aIsAgableFunc */
                               &sRootPageHandle,
                               aEnv ) == STL_SUCCESS );

        /* SMO No 초기화 */
        smpSetVolatileArea( &sRootPageHandle,
                            &sSmoNo,
                            STL_SIZEOF(stlUInt64) );

        sHeader->mRootPageId = smpGetPageId( &sRootPageHandle );

        stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
        sLogicalHdr.mPrevPageId = SMP_NULL_PID;
        sLogicalHdr.mNextPageId = SMP_NULL_PID;
        sLogicalHdr.mNextFreePageId = SMP_NULL_PID;
        sLogicalHdr.mLevel    = 0;
        sLogicalHdr.mKeyColCount = aArgs->mKeyColCount;
        stlMemcpy( sLogicalHdr.mKeyColTypes,
                   aArgs->mKeyColTypes,
                   aArgs->mKeyColCount );
        stlMemcpy( sLogicalHdr.mKeyColFlags,
                   aArgs->mKeyColFlags,
                   aArgs->mKeyColCount );
    
        STL_TRY( smpInitBody( &sRootPageHandle,
                              SMP_ORDERED_SLOT_TYPE,
                              STL_SIZEOF(smnmpbLogicalHdr),
                              (void*)&sLogicalHdr,
                              SMN_GET_INITRANS(aArgs->mIndexHandle),
                              SMN_GET_MAXTRANS(aArgs->mIndexHandle),
                              STL_FALSE,
                              &sMiniTrans,
                              aEnv) == STL_SUCCESS );

        STL_TRY( smxmWriteLog( &sMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_SET_ROOT_PAGE_ID,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar*)&sHeader->mRootPageId,
                               STL_SIZEOF(smlPid),
                               smpGetTbsId( &sPageHandle ),
                               smpGetPageId( &sPageHandle ),
                               (stlChar*)sHeader - (stlChar*)SMP_FRAME(&sPageHandle),
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
        
        STL_TRY( smnSetMirrorRootPageId( &sMiniTrans,
                                         aArgs->mIndexHandle,
                                         SMP_NULL_PID,
                                         aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smxmWriteLog( &sMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_SET_ROOT_PAGE_ID,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar*)&sHeader->mRootPageId,
                               STL_SIZEOF(smlPid),
                               sSegmentRid.mTbsId,
                               sSegmentRid.mPageId,
                               (stlChar*)sHeader - (stlChar*)SMP_FRAME(&sPageHandle),
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

        /**
         * mirror page 설정
         */

        sMirrorDepth = knlGetPropertyBigIntValueByID( KNL_PROPERTY_INDEX_ROOT_MIRRORING_DEPTH,
                                                      KNL_ENV( aEnv ) );

        /**
         * Dictionary의 index는 mirroring 대상에서 제외한다.
         */
        if( (aArgs->mTbsId != SML_MEMORY_DICT_SYSTEM_TBS_ID) &&
            (sHeight >= sMirrorDepth) )
        {
            STL_TRY( smpAcquire( &sMiniTrans,
                                 aArgs->mTbsId,
                                 sHeader->mRootPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sRootPageHandle,
                                 aEnv ) == STL_SUCCESS );
            
            /**
             * Root page의 mirroring 효과를 최대화하기 위해 fan-out을 최대화 한다.
             */
            STL_TRY( smnmpbMaximizeFanoutRootPage( &sMiniTrans,
                                                   aArgs,
                                                   &sRootPageHandle,
                                                   aEnv )
                     == STL_SUCCESS );

            STL_TRY( smsAllocPage( &sMiniTrans,
                                   sSegmentHandle,
                                   SMP_PAGE_TYPE_INDEX_DATA,
                                   NULL,  /* aIsAgableFunc */
                                   &sMirrorPageHandle,
                                   aEnv ) == STL_SUCCESS );
            
            STL_TRY( smnmpbMirrorRoot( aArgs->mIndexHandle,
                                       &sMiniTrans,
                                       &sMirrorPageHandle,
                                       aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smnSetMirrorRootPageId( &sMiniTrans,
                                             aArgs->mIndexHandle,
                                             SMP_NULL_PID,
                                             aEnv )
                     == STL_SUCCESS );
        }
    }

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );

    SMN_SET_ROOT_PAGE_ID( aArgs->mIndexHandle,
                          sHeader->mRootPageId );
    SMN_SET_MIRROR_ROOT_PID( aArgs->mIndexHandle,
                             sHeader->mMirrorRootPid );
    
    ((smnIndexHeader*)(aArgs->mIndexHandle))->mEmptyNodeCount = 0;
    ((smnIndexHeader*)(aArgs->mIndexHandle))->mFirstEmptyNode = SMP_NULL_PID;
    ((smnIndexHeader*)(aArgs->mIndexHandle))->mLastEmptyNode  = SMP_NULL_PID;

    /**
     * Fill Free Extent Array
     */

    STL_TRY( knlInitQueue( &sFreeQueueInfo,
                           1024,
                           STL_SIZEOF(smlPid),
                           &aEnv->mDynamicHeapMem,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sQueueState = 1;

    for( i = 0; i < SMNMPB_MAX_FREE_EXTENT_COUNT; i++ )
    {
        if( aArgs->mFreeExtentArray[i] == SMP_NULL_PID )
        {
            continue;
        }

        STL_TRY( knlEnqueue( &sFreeQueueInfo,
                             (void*)&(aArgs->mFreeExtentArray[i]),
                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        aArgs->mFreeExtentArray[i] = SMP_NULL_PID;
    }

    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );

    /**
     * 아직도 Free Extent가 남아 있다면 Segment로 반납한다.
     */

    STL_TRY( smnmpbFreeExtents( aArgs->mTransId,
                                aArgs->mIndexHandle,
                                aArgs->mBuildExtentPageCount,
                                &sFreeQueueInfo,
                                aEnv )
             == STL_SUCCESS );

    sQueueState = 0;
    STL_TRY( knlFiniQueue( &sFreeQueueInfo,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        smxmRollback( &sMiniTrans, aEnv );
    }

    if( sQueueState == 1 )
    {
        (void) knlFiniQueue( &sFreeQueueInfo, KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

stlStatus smnmpbWriteKeyToTree( smnmpbParallelArgs * aArgs,
                                smnmpbTreeIterator * aIterator,
                                void               * aRunKey,
                                smlEnv             * aEnv )
{
    smnmpbLogicalHdr   sLogicalHdr;
    stlInt16           sTreeKeySize;
    stlChar          * sSrcKeyBody;
    stlChar          * sDstKey;
    stlChar          * sPropKey;
    stlChar          * sPropKeyBody;
    stlInt16           sPropKeyBodySize;
    smlPid             sPrevPageId;
    smlPid             sRowPid;
    stlInt16           sRowOffset;
    stlInt16           sSlotId;
    stlInt16           sTotalKeyCount;
    dtlCompareResult   sResult = DTL_COMPARISON_EQUAL;
    stlUInt8           sInsRtsSeq = SMP_RTS_SEQ_STABLE;
    stlUInt8           sDelRtsSeq = SMP_RTS_SEQ_NULL;
    stlUInt8           sRtsVerNo = 0;
    stlUInt8           sInsTcn;
    stlUInt8           sDelTcn = SMNMPB_INVALID_TCN;

    /**
     * Run Key format을 Tree Key format으로 변경한다.
     */

    SMNMPB_RUN_KEY_GET_TOTAL_SIZE( aRunKey, &sTreeKeySize );

    sTreeKeySize -= SMNMPB_RUN_KEY_HDR_LEN;
    sTreeKeySize += SMNMPB_MIN_LEAF_KEY_HDR_LEN;

    sSrcKeyBody = aRunKey + SMNMPB_RUN_KEY_HDR_LEN;

    /**
     * Key를 Tree에 삽입한다.
     */
    
    if( aIterator->mPageId[0] == SMP_NULL_PID )
    {
        /**
         * Writing first key
         */
        STL_TRY( smnmpbAllocPage4Build( aIterator,
                                        &aIterator->mPageId[0],
                                        aEnv )
                 == STL_SUCCESS );

        aIterator->mHeight = 0;

        STL_TRY( smpAcquire( aIterator->mMiniTrans,
                             aIterator->mTbsId,
                             aIterator->mPageId[0],
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &aIterator->mLeafPageHandle,
                             aEnv )
                 == STL_SUCCESS );

        aIterator->mFirstPageId[0] = aIterator->mPageId[0];

        stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
        sLogicalHdr.mPrevPageId = SMP_NULL_PID;
        sLogicalHdr.mNextPageId = SMP_NULL_PID;
        sLogicalHdr.mNextFreePageId = SMP_NULL_PID;
        sLogicalHdr.mLevel = 0;
        sLogicalHdr.mKeyColCount = aIterator->mKeyColCount;
        stlMemcpy( &sLogicalHdr.mKeyColTypes,
                   aIterator->mKeyColTypes,
                   aIterator->mKeyColCount );
        stlMemcpy( &sLogicalHdr.mKeyColFlags,
                   aIterator->mKeyColFlags,
                   aIterator->mKeyColCount );
    
        STL_TRY( smpInitBody( &aIterator->mLeafPageHandle,
                              SMP_ORDERED_SLOT_TYPE,
                              STL_SIZEOF(smnmpbLogicalHdr),
                              (void*)&sLogicalHdr,
                              SMN_GET_INITRANS(aIterator->mIndexHandle),
                              SMN_GET_MAXTRANS(aIterator->mIndexHandle),
                              STL_FALSE,
                              aIterator->mMiniTrans,
                              aEnv) == STL_SUCCESS );
    }
    else
    {
        if( aArgs->mIsUniqueIndex == STL_TRUE )
        {
            sResult = smnmpbCompare4MergeTree( aIterator->mCompareInfo,
                                               aIterator->mPrevKey + SMNMPB_MIN_LEAF_KEY_HDR_LEN,
                                               sSrcKeyBody );
        
            STL_TRY_THROW( (aArgs->mIsUniqueIndex != STL_TRUE) ||
                           (sResult != DTL_COMPARISON_EQUAL )  ||
                           (aIterator->mCompareInfo->mUniqueViolated == 0),
                           RAMP_ERR_UNIQUE_VIOLATED );
        }
    }
    
    STL_TRY( smpAllocSlot( &aIterator->mLeafPageHandle,
                           sTreeKeySize,
                           STL_FALSE,
                           &sDstKey,
                           &sSlotId )
             == STL_SUCCESS );

    if( sDstKey == NULL )
    {
        /**
         * Propagate Key
         */
        sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&aIterator->mLeafPageHandle);
        sPropKey = smpGetNthRowItem( &aIterator->mLeafPageHandle,
                                     sTotalKeyCount - 1 );

        sPropKeyBody = sPropKey + SMNMPB_MIN_LEAF_KEY_HDR_LEN;
        sPropKeyBodySize = ( smnmpbGetLeafKeySize( sPropKey,
                                                   aIterator->mKeyColCount ) -
                             SMNMPB_MIN_LEAF_KEY_HDR_LEN );

        SMNMPB_GET_ROW_PID( sPropKey, &sRowPid );
        SMNMPB_GET_ROW_SEQ( sPropKey, &sRowOffset );
        
        STL_TRY( smnmpbPropageKey4Build( aIterator,
                                         1,
                                         aIterator->mPageId[0],
                                         sPropKeyBody,
                                         sPropKeyBodySize,
                                         sRowPid,
                                         sRowOffset,
                                         aEnv )
                 == STL_SUCCESS );

        sPrevPageId = aIterator->mPageId[0];
        
        /**
         * Link to sibling node
         */
        STL_TRY( smnmpbAllocPage4Build( aIterator,
                                        &aIterator->mPageId[0],
                                        aEnv )
                 == STL_SUCCESS );
        
        SMNMPB_SET_NEXT_PAGE( &aIterator->mLeafPageHandle,
                              aIterator->mPageId[0] );

        /**
         * Bulk logging
         */
        STL_TRY( smxmWriteLog( aIterator->mMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                               SMR_REDO_TARGET_PAGE,
                               SMP_FRAME(&aIterator->mLeafPageHandle),
                               SMP_PAGE_SIZE,
                               aIterator->mTbsId,
                               smpGetPageId(&aIterator->mLeafPageHandle),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

        aIterator->mMiniTransState = 0;
        STL_TRY( smxmCommit( aIterator->mMiniTrans, aEnv ) == STL_SUCCESS );

        STL_TRY( smxmBegin( aIterator->mMiniTrans,
                            aIterator->mTransId,
                            smsGetSegRid( SME_GET_SEGMENT_HANDLE(aIterator->mIndexHandle) ),
                            aIterator->mMiniTransAttr,
                            aEnv )
                 == STL_SUCCESS );
        aIterator->mMiniTransState = 1;

        STL_TRY( smpAcquire( aIterator->mMiniTrans,
                             aIterator->mTbsId,
                             aIterator->mPageId[0],
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &aIterator->mLeafPageHandle,
                             aEnv )
                 == STL_SUCCESS );

        stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
        sLogicalHdr.mPrevPageId = sPrevPageId;
        sLogicalHdr.mNextPageId = SMP_NULL_PID;
        sLogicalHdr.mNextFreePageId = SMP_NULL_PID;
        sLogicalHdr.mLevel = 0;
        sLogicalHdr.mKeyColCount = aIterator->mKeyColCount;
        stlMemcpy( &sLogicalHdr.mKeyColTypes,
                   aIterator->mKeyColTypes,
                   aIterator->mKeyColCount );
        stlMemcpy( &sLogicalHdr.mKeyColFlags,
                   aIterator->mKeyColFlags,
                   aIterator->mKeyColCount );
    
        STL_TRY( smpInitBody( &aIterator->mLeafPageHandle,
                              SMP_ORDERED_SLOT_TYPE,
                              STL_SIZEOF(smnmpbLogicalHdr),
                              (void*)&sLogicalHdr,
                              SMN_GET_INITRANS(aIterator->mIndexHandle),
                              SMN_GET_MAXTRANS(aIterator->mIndexHandle),
                              STL_FALSE,
                              aIterator->mMiniTrans,
                              aEnv) == STL_SUCCESS );
        
        STL_TRY( smpAllocSlot( &aIterator->mLeafPageHandle,
                               sTreeKeySize,
                               STL_FALSE,
                               &sDstKey,
                               &sSlotId )
                 == STL_SUCCESS );
    }

    SMNMPB_RUN_KEY_GET_ROW_PID( aRunKey, &sRowPid );
    SMNMPB_RUN_KEY_GET_ROW_OFFSET( aRunKey, &sRowOffset );
    SMNMPB_RUN_KEY_GET_TCN( aRunKey, &sInsTcn );
    
    SMNMPB_INIT_KEY_HDR_BITMAP( sDstKey );
    SMNMPB_SET_INS_TCN( sDstKey, &sInsTcn );
    SMNMPB_SET_DEL_TCN( sDstKey, &sDelTcn );
    SMNMPB_UNSET_DELETED( sDstKey );
    SMNMPB_SET_ROW_PID( sDstKey, &sRowPid );
    SMNMPB_SET_ROW_SEQ( sDstKey, &sRowOffset );
    SMNMPB_SET_INS_RTS_INFO( sDstKey, &sInsRtsSeq, &sRtsVerNo );
    SMNMPB_SET_DEL_RTS_INFO( sDstKey, &sDelRtsSeq, &sRtsVerNo);
    SMNMPB_UNSET_TBK( sDstKey );

    stlMemcpy( sDstKey + SMNMPB_MIN_LEAF_KEY_HDR_LEN,
               sSrcKeyBody,
               sTreeKeySize - SMNMPB_MIN_LEAF_KEY_HDR_LEN );
    
    if( aArgs->mIsUniqueIndex == STL_TRUE )
    {
        stlMemcpy( aIterator->mPrevKey, sDstKey, sTreeKeySize );
    }

    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(&aIterator->mLeafPageHandle) );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_UNIQUE_VIOLATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INDEX_UNIQUENESS_VIOLATION,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbAllocPage4Build( smnmpbTreeIterator * aIterator,
                                 smlPid             * aPageId,
                                 smlEnv             * aEnv )
{
    stlBool    sIsEmpty;
    stlInt32   sUsablePageCount;
    void     * sSegmentHandle;
    smlPid     sExtentPid;
    
    /**
     * 처음 할당 받는 경우나 기존 extent를 다 사용한 경우는
     * 새로운 extent를 할당 받는다.
     */
    
    if( (aIterator->mExtentPid == SMP_NULL_PID) ||
        (aIterator->mPageSeq >= aIterator->mBuildExtentPageCount) )
    {
        STL_TRY( knlDequeue( aIterator->mFreeQueueInfo,
                             &aIterator->mExtentPid,
                             &sIsEmpty,
                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    
        if( sIsEmpty == STL_TRUE )
        {
            sSegmentHandle = SME_GET_SEGMENT_HANDLE( aIterator->mIndexHandle );
            sUsablePageCount = smsGetUsablePageCountInExt( sSegmentHandle );
            
            STL_TRY( smsAllocExtent( aIterator->mTransId,
                                     sSegmentHandle,
                                     SMP_PAGE_TYPE_INDEX_DATA,
                                     &aIterator->mExtentPid,
                                     aEnv )
                     == STL_SUCCESS );
            
            sExtentPid = aIterator->mExtentPid + aIterator->mBuildExtentPageCount;
            
            while( sExtentPid < (aIterator->mExtentPid + sUsablePageCount) )
            {
                STL_TRY( knlEnqueue( aIterator->mFreeQueueInfo,
                                     &sExtentPid,
                                     KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sExtentPid += aIterator->mBuildExtentPageCount;
            }

            STL_DASSERT( sExtentPid == (aIterator->mExtentPid + sUsablePageCount) );
        }

        aIterator->mPageSeq = 0;
    }

    *aPageId = aIterator->mExtentPid + aIterator->mPageSeq;
    aIterator->mPageSeq++;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbPropageKey4Build( smnmpbTreeIterator * aIterator,
                                  stlInt16             aHeight,
                                  smlPid               aChildPid,
                                  stlChar            * aPropKeyBody,
                                  stlInt16             aPropKeyBodySize,
                                  smlPid               aRowPageId,
                                  stlInt16             aRowOffset,
                                  smlEnv             * aEnv )
{
    smpHandle          sPageHandle;
    smnmpbLogicalHdr   sLogicalHdr;
    stlChar          * sDstKey;
    stlChar          * sPropKey;
    stlChar          * sPropKeyBody;
    stlInt16           sPropKeyBodySize;
    smlPid             sRowPid;
    stlInt16           sRowOffset;
    stlInt16           sSlotId;
    stlInt16           sTotalKeyCount;
    stlInt16           sPctFree;
    
    /**
     * Internal Key를 삽입한다.
     */

    sPctFree = SMNMPB_GET_PCTFREE( aIterator->mIndexHandle );

    if( aIterator->mPageId[aHeight] == SMP_NULL_PID )
    {
        STL_TRY_THROW( aHeight < SMNMPB_MAX_INDEX_DEPTH,
                       RAMP_ERR_MAX_INDEX_DEPTH );
        
        STL_TRY( smnmpbAllocPage4Build( aIterator,
                                        &aIterator->mPageId[aHeight],
                                        aEnv )
                 == STL_SUCCESS );

        aIterator->mHeight = aHeight;

        STL_TRY( smpAcquire( aIterator->mMiniTrans,
                             aIterator->mTbsId,
                             aIterator->mPageId[aHeight],
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv )
                 == STL_SUCCESS );

        aIterator->mFirstPageId[aHeight] = aIterator->mPageId[aHeight];

        stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
        sLogicalHdr.mPrevPageId = SMP_NULL_PID;
        sLogicalHdr.mNextPageId = SMP_NULL_PID;
        sLogicalHdr.mNextFreePageId = SMP_NULL_PID;
        sLogicalHdr.mLevel = aHeight;
        sLogicalHdr.mKeyColCount = aIterator->mKeyColCount;
        stlMemcpy( &sLogicalHdr.mKeyColTypes,
                   aIterator->mKeyColTypes,
                   aIterator->mKeyColCount );
        stlMemcpy( &sLogicalHdr.mKeyColFlags,
                   aIterator->mKeyColFlags,
                   aIterator->mKeyColCount );
    
        STL_TRY( smpInitBody( &sPageHandle,
                              SMP_ORDERED_SLOT_TYPE,
                              STL_SIZEOF(smnmpbLogicalHdr),
                              (void*)&sLogicalHdr,
                              0,
                              0,
                              STL_FALSE,
                              aIterator->mMiniTrans,
                              aEnv) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smpAcquire( aIterator->mMiniTrans,
                             aIterator->mTbsId,
                             aIterator->mPageId[aHeight],
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv )
                 == STL_SUCCESS );
    }
    
#ifdef STL_DEBUG
    sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);

    if( sTotalKeyCount > 0 )
    {
        sDstKey = smpGetNthRowItem( &sPageHandle,
                                    sTotalKeyCount - 1 );

        STL_DASSERT( smnmpbCompare4MergeTree( aIterator->mCompareInfo,
                                              sDstKey + SMNMPB_INTL_SLOT_HDR_SIZE,
                                              aPropKeyBody )
                     != DTL_COMPARISON_GREATER );
    }
#endif
    
    /**
     * Max Key 크기를 포함해서 할당 가능한지 판단한다.
     */

    if( smpIsPageOverInsertLimit( SMP_FRAME(&sPageHandle),
                                  sPctFree )
        == STL_TRUE )
    {
        sDstKey = NULL;
    }
    else
    {
        STL_TRY( smpAllocSlot( &sPageHandle,
                               (aPropKeyBodySize + SMNMPB_INTL_SLOT_HDR_SIZE +
                                STL_SIZEOF(smlPid) + STL_SIZEOF(smpOffsetSlot)),
                               STL_TRUE,
                               &sDstKey,
                               &sSlotId )
                 == STL_SUCCESS );
    }

    if( sDstKey == NULL )
    {
        /**
         * Propagate Key
         */
        sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);
        sPropKey = smpGetNthRowItem( &sPageHandle,
                                     sTotalKeyCount - 1 );

        sPropKeyBody = sPropKey + SMNMPB_INTL_SLOT_HDR_SIZE;
        sPropKeyBodySize = ( smnmpbGetIntlKeySize( sPropKey,
                                                   aIterator->mKeyColCount ) -
                             SMNMPB_INTL_SLOT_HDR_SIZE);

        SMNMPB_INTL_SLOT_GET_ROW_PID( sPropKey, &sRowPid );
        SMNMPB_INTL_SLOT_GET_ROW_OFFSET( sPropKey, &sRowOffset );
        
        STL_TRY( smnmpbPropageKey4Build( aIterator,
                                         aHeight + 1,
                                         aIterator->mPageId[aHeight],
                                         sPropKeyBody,
                                         sPropKeyBodySize,
                                         sRowPid,
                                         sRowOffset,
                                         aEnv )
                 == STL_SUCCESS );

        /**
         * Link to sibling node
         */
        STL_TRY( smnmpbAllocPage4Build( aIterator,
                                        &aIterator->mPageId[aHeight],
                                        aEnv )
                 == STL_SUCCESS );
        
        /**
         * Bulk logging
         */
        STL_TRY( smxmWriteLog( aIterator->mMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                               SMR_REDO_TARGET_PAGE,
                               SMP_FRAME(&sPageHandle),
                               SMP_PAGE_SIZE,
                               aIterator->mTbsId,
                               smpGetPageId(&sPageHandle),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

        STL_TRY( smpAcquire( aIterator->mMiniTrans,
                             aIterator->mTbsId,
                             aIterator->mPageId[aHeight],
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv )
                 == STL_SUCCESS );

        stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
        sLogicalHdr.mPrevPageId = SMP_NULL_PID;
        sLogicalHdr.mNextPageId = SMP_NULL_PID;
        sLogicalHdr.mNextFreePageId = SMP_NULL_PID;
        sLogicalHdr.mLevel = aHeight;
        sLogicalHdr.mKeyColCount = aIterator->mKeyColCount;
        stlMemcpy( &sLogicalHdr.mKeyColTypes,
                   aIterator->mKeyColTypes,
                   aIterator->mKeyColCount );
        stlMemcpy( &sLogicalHdr.mKeyColFlags,
                   aIterator->mKeyColFlags,
                   aIterator->mKeyColCount );
    
        STL_TRY( smpInitBody( &sPageHandle,
                              SMP_ORDERED_SLOT_TYPE,
                              STL_SIZEOF(smnmpbLogicalHdr),
                              (void*)&sLogicalHdr,
                              0,
                              0,
                              STL_FALSE,
                              aIterator->mMiniTrans,
                              aEnv) == STL_SUCCESS );
    }

    STL_TRY( smpAllocSlot( &sPageHandle,
                           aPropKeyBodySize + SMNMPB_INTL_SLOT_HDR_SIZE,
                           STL_FALSE,
                           &sDstKey,
                           &sSlotId )
             == STL_SUCCESS );

    SMNMPB_INTL_SLOT_SET_CHILD_PID( sDstKey, &aChildPid );
    SMNMPB_INTL_SLOT_SET_ROW_PID( sDstKey, &aRowPageId );
    SMNMPB_INTL_SLOT_SET_ROW_OFFSET( sDstKey, &aRowOffset );
    
    stlMemcpy( sDstKey + SMNMPB_INTL_SLOT_HDR_SIZE,
               aPropKeyBody,
               aPropKeyBodySize );

    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(&sPageHandle) );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MAX_INDEX_DEPTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEEDED_MAX_INDEX_DEPTH,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      aHeight );
    }
 
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbGroupingRuns( smnmpbParallelArgs * aArgs,
                              smlEnv             * aEnv )
{
    knlHeapQueueEntry        sHeapQueueArray[SMN_MAX_MERGED_RUN_LIST*2];
    knlHeapQueueContext      sHeapQueueContext;
    knlHeapQueueInfo         sHeapQueueInfo;
    smlPid                   sMergedRunList[SMN_MAX_MERGED_RUN_LIST];
    stlInt32                 sMergedRunCount;
    stlInt32                 i;
    stlInt32                 j;
    smnmpbBuildIterator      sIterator[SMN_MAX_MERGED_RUN_LIST];
    smlPid                   sPrevExtentPid[SMN_MAX_MERGED_RUN_LIST];
    smnmpbCompareInfo        sCompareInfo;
    dtlDataType              sDataType;
    knlValueBlockList      * sSrcColumn = NULL;
    knlQueueInfo             sFreeQueueInfo;
    stlBool                  sIsEmpty = STL_TRUE;
    smlPid                   sExtentPid;
    stlChar                  sMaxKey[SMP_PAGE_SIZE];
    stlInt64                 sTotalRunCount = 0;
    stlInt64                 sMaxRunCount = 0;
    stlInt64                 sIteratorIdx;
    knlHeapQueueEntry      * sTopEntry;
    stlChar                * sKey;
    smpHandle                sPageHandle;
    stlInt64                 sFreeExtentCount;
    stlInt64                 sRunCount;
    stlInt32                 sState = 0;
    
    sSrcColumn = aArgs->mIndexColList;
    i = 0;
    
    while( sSrcColumn != NULL )
    {
        sDataType = KNL_GET_BLOCK_DB_TYPE( sSrcColumn );
        sCompareInfo.mPhysicalCompare[i] = dtlPhysicalCompareFuncList[sDataType][sDataType];
        sSrcColumn = sSrcColumn->mNext;
        i++;
    }
    
    sCompareInfo.mSortBlock = NULL;
    sCompareInfo.mCompareCount = 0;
    sCompareInfo.mUniqueViolated = 0;
    sCompareInfo.mKeyColCount = aArgs->mKeyColCount;
    sCompareInfo.mKeyColFlags = aArgs->mKeyColFlags;
    sCompareInfo.mKeyColOrder = aArgs->mKeyColOrder;
    
    sHeapQueueContext.mCompare = aArgs->mMergeCompare;
    sHeapQueueContext.mFetchNext = smnmpbGroupingFetchNext;
    sHeapQueueContext.mCompareInfo = &sCompareInfo;
    sHeapQueueContext.mIterator = &sIterator;

    /**
     * Merge Merged Run Lists
     */
    
    STL_TRY( knlInitQueue( &sFreeQueueInfo,
                           1024,
                           STL_SIZEOF(smlPid),
                           &aEnv->mDynamicHeapMem,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    stlMemset( sMergedRunList, 0x00, STL_SIZEOF(smlPid) * SMN_MAX_MERGED_RUN_LIST );
    sMergedRunCount = 0;
    
    for( i = 0; i < aArgs[0].mParallelFactor; i++ )
    {
        /**
         * merge runs
         */
        
        for( j = 0; j < SMN_MAX_MERGED_RUN_LIST; j++ )
        {
            if( aArgs[i].mMergedRunList[j] == SMP_NULL_PID )
            {
                continue;
            }

            /**
             * 항상 1개의 run임을 보장해야 한다.
             */
            STL_DASSERT( j < 1 );
            
            sMergedRunList[sMergedRunCount] = aArgs[i].mMergedRunList[j];
            sMergedRunCount++;
        }

        stlMemset( aArgs[i].mMergedRunList,
                   0x00,
                   STL_SIZEOF(smlPid) * SMN_MAX_MERGED_RUN_LIST );
        
        /**
         * merge free extents
         */
        
        for( j = 0; j < SMNMPB_MAX_FREE_EXTENT_COUNT; j++ )
        {
            if( aArgs[i].mFreeExtentArray[j] == SMP_NULL_PID )
            {
                continue;
            }

            STL_TRY( knlEnqueue( &sFreeQueueInfo,
                                 (void*)&(aArgs[i].mFreeExtentArray[j]),
                                 KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            aArgs[i].mFreeExtentArray[j] = SMP_NULL_PID;
        }

        stlMemset( aArgs[i].mFreeExtentArray,
                   0x00,
                   STL_SIZEOF(smlPid) * SMNMPB_MAX_FREE_EXTENT_COUNT );

        sTotalRunCount += aArgs[i].mTotalRunCount;
    }

    /**
     * decide number of run to be merging
     */

    sMaxRunCount = (sTotalRunCount / aArgs[0].mParallelFactor);

    /**
     * grouping runs
     */
    
    for( i = 0; i < sMergedRunCount; i++ )
    {
        sIterator[i].mTbsId = aArgs->mTbsId;
        sIterator[i].mBuildExtentPageCount = aArgs->mBuildExtentPageCount;
        sIterator[i].mNextExtentPid = sMergedRunList[i];
        sIterator[i].mCurExtentPid = SMP_NULL_PID;
        sIterator[i].mPageSeq = -1;
        SMP_MAKE_DUMMY_HANDLE( &sIterator[i].mPageHandle, NULL );
        sIterator[i].mFreeQueueInfo = &sFreeQueueInfo;
    }

    for( i = 0; i < aArgs[0].mParallelFactor; i++ )
    {
        sRunCount = 0;
        stlMemset( sPrevExtentPid, 0x00, STL_SIZEOF(smlPid) * SMN_MAX_MERGED_RUN_LIST );
        
        STL_TRY( knlHeapify( &sHeapQueueInfo,
                             &sHeapQueueContext,
                             sHeapQueueArray,
                             SMN_MAX_MERGED_RUN_LIST * 2,
                             sMergedRunCount,
                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        while( STL_TRUE )
        {
            sTopEntry = knlGetMinQueueEntry( &sHeapQueueInfo );
            sIteratorIdx = sTopEntry->mIteratorIdx;
            sExtentPid = sIterator[sIteratorIdx].mCurExtentPid;
            
            STL_TRY( knlGetMinHeap( &sHeapQueueInfo,
                                    (void**)&sKey,
                                    NULL,
                                    KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            if( sKey == NULL )
            {
                break;
            }
            
            /**
             * linking to run list
             */

            if( sPrevExtentPid[sIteratorIdx] == SMP_NULL_PID )
            {
                aArgs[i].mMergedRunList[sIteratorIdx] = sExtentPid;
            }
            else
            {
                STL_TRY( smpFix( aArgs->mTbsId,
                                 sPrevExtentPid[sIteratorIdx],
                                 &sPageHandle,
                                 SML_ENV(aEnv) )
                         == STL_SUCCESS );
            
                SMNMPB_SET_NEXT_EXTENT( &sPageHandle, sExtentPid );

                STL_TRY( smpUnfix( &sPageHandle,
                                   aEnv ) == STL_SUCCESS );
            }
            
            sPrevExtentPid[sIteratorIdx] = sExtentPid;
            
            STL_TRY( smpFix( aArgs->mTbsId,
                             sExtentPid,
                             &sPageHandle,
                             SML_ENV(aEnv) )
                     == STL_SUCCESS );

            SMNMPB_SET_NEXT_EXTENT( &sPageHandle, SMP_NULL_PID );

            STL_TRY( smpUnfix( &sPageHandle,
                               aEnv ) == STL_SUCCESS );

            sRunCount++;
            
            /**
             * 마지막 group은 모든 extent를 추출해야 한다.
             */
            if( (i + 1) < aArgs[0].mParallelFactor )
            {
                if( sRunCount >= sMaxRunCount )
                {
                    break;
                }
            }
        }

        /**
         * latch 상태 정리
         */
        
        for( j = 0; j < sMergedRunCount; j++ )
        {
            if( SMP_IS_DUMMY_HANDLE( &sIterator[j].mPageHandle ) == STL_FALSE )
            {
                STL_TRY( smpUnfix( &sIterator[j].mPageHandle,
                                   SML_ENV(aEnv) )
                         == STL_SUCCESS );
                SMP_MAKE_DUMMY_HANDLE( &sIterator[j].mPageHandle, NULL );
            }
        }

        if( sKey == NULL )
        {
            break;
        }

        /**
         * get max key
         */
        
        STL_TRY( smnmpbGetMaxKey( aArgs,
                                  sExtentPid,
                                  sMaxKey,
                                  aEnv )
                 == STL_SUCCESS );

        /**
         * split run
         */
        
        for( j = 0; j < sMergedRunCount; j++ )
        {
            STL_TRY( smnmpbSplitRun( aArgs,
                                     &sIterator[j],
                                     sMaxKey,
                                     &sCompareInfo,
                                     &sFreeQueueInfo,
                                     &sExtentPid,
                                     aEnv )
                     == STL_SUCCESS );

            /**
             * linking to run list
             */

            if( sExtentPid != SMP_NULL_PID )
            {
                if( sPrevExtentPid[j] == SMP_NULL_PID )
                {
                    aArgs[i].mMergedRunList[j] = sExtentPid;
                }
                else
                {
                    STL_TRY( smpFix( aArgs->mTbsId,
                                     sPrevExtentPid[j],
                                     &sPageHandle,
                                     SML_ENV(aEnv) )
                             == STL_SUCCESS );
            
                    SMNMPB_SET_NEXT_EXTENT( &sPageHandle, sExtentPid );

                    STL_TRY( smpUnfix( &sPageHandle,
                                       aEnv ) == STL_SUCCESS );
                }
            
                STL_TRY( smpFix( aArgs->mTbsId,
                                 sExtentPid,
                                 &sPageHandle,
                                 SML_ENV(aEnv) )
                         == STL_SUCCESS );

                SMNMPB_SET_NEXT_EXTENT( &sPageHandle, SMP_NULL_PID );

                STL_TRY( smpUnfix( &sPageHandle,
                                   aEnv ) == STL_SUCCESS );
            }
        }
    }

    /**
     * distribte free extens
     */

    if( knlGetQueueSize( &sFreeQueueInfo ) > 0 )
    {
        sFreeExtentCount = ( knlGetQueueSize( &sFreeQueueInfo ) / aArgs[0].mParallelFactor );
        sFreeExtentCount = STL_MIN( sFreeExtentCount, SMNMPB_MAX_FREE_EXTENT_COUNT );
            
        for( i = 0; i < aArgs[0].mParallelFactor; i++ )
        {
            for( j = 0; j < sFreeExtentCount; j++ )
            {
                STL_TRY( knlDequeue( &sFreeQueueInfo,
                                     (void*)&sExtentPid,
                                     &sIsEmpty,
                                     KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( sIsEmpty == STL_TRUE )
                {
                    break;
                }

                aArgs[i].mFreeExtentArray[j] = sExtentPid;
            }
        }
    }

    /**
     * 아직도 Free Extent가 남아 있다면 Segment로 반납한다.
     */

    STL_TRY( smnmpbFreeExtents( aArgs->mTransId,
                                aArgs->mIndexHandle,
                                aArgs->mBuildExtentPageCount,
                                &sFreeQueueInfo,
                                aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFiniQueue( &sFreeQueueInfo,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) knlFiniQueue( &sFreeQueueInfo, KNL_ENV(aEnv) );
        default:
            break;
    }
    
    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );
    
    return STL_FAILURE;
}

stlStatus smnmpbMergeTree( smnmpbParallelArgs * aArgs,
                           smlEnv             * aEnv )
{
    stlInt32             i;
    stlInt32             j;
    stlInt32             sHeight;
    smnmpbTreeIterator   sIterator;
    smxmTrans            sMiniTrans;
    stlInt32             sAttr;
    knlQueueInfo         sFreeQueueInfo;
    smnmpbCompareInfo    sCompareInfo;
    dtlDataType          sDataType;
    knlValueBlockList  * sSrcColumn = NULL;
    stlInt32             sState = 0;
    smpHandle            sPageHandle;

    if( aArgs->mIsLogging == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }
    
    /**
     * Fill Free Page Queue
     */

    STL_TRY( knlInitQueue( &sFreeQueueInfo,
                           1024,
                           STL_SIZEOF(smlPid),
                           &aEnv->mDynamicHeapMem,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    for( i = 0; i < aArgs->mParallelFactor; i++ )
    {
        for( j = 0; j < SMNMPB_MAX_FREE_EXTENT_COUNT; j++ )
        {
            if( aArgs[i].mFreeExtentArray[j] == SMP_NULL_PID )
            {
                continue;
            }

            STL_TRY( knlEnqueue( &sFreeQueueInfo,
                                 (void*)&(aArgs[i].mFreeExtentArray[j]),
                                 KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            aArgs[i].mFreeExtentArray[j] = SMP_NULL_PID;
        }

        stlMemset( aArgs[i].mFreeExtentArray,
                   0x00,
                   STL_SIZEOF(smlPid) * SMNMPB_MAX_FREE_EXTENT_COUNT );
    }

    sSrcColumn = aArgs->mIndexColList;
    i = 0;
    
    while( sSrcColumn != NULL )
    {
        sDataType = KNL_GET_BLOCK_DB_TYPE( sSrcColumn );
        sCompareInfo.mPhysicalCompare[i] = dtlPhysicalCompareFuncList[sDataType][sDataType];
        sSrcColumn = sSrcColumn->mNext;
        i++;
    }
    
    sCompareInfo.mSortBlock = NULL;
    sCompareInfo.mCompareCount = 0;
    sCompareInfo.mUniqueViolated = 0;
    sCompareInfo.mKeyColCount = aArgs->mKeyColCount;
    sCompareInfo.mKeyColFlags = aArgs->mKeyColFlags;
    sCompareInfo.mKeyColOrder = aArgs->mKeyColOrder;

    sIterator.mCompareInfo = &sCompareInfo;
    sIterator.mTransId = aArgs->mTransId;
    sIterator.mTbsId = aArgs->mTbsId;
    sIterator.mMiniTrans = &sMiniTrans;
    sIterator.mMiniTransState = 0;
    sIterator.mMiniTransAttr = sAttr;
    sIterator.mIndexHandle = aArgs->mIndexHandle;
    sIterator.mHeight = aArgs[0].mHeight;
    sIterator.mPageSeq = 0;
    sIterator.mKeyColCount = aArgs->mKeyColCount;
    sIterator.mKeyColFlags = aArgs->mKeyColFlags;
    sIterator.mKeyColTypes = aArgs->mKeyColTypes;
    sIterator.mFreeQueueInfo = &sFreeQueueInfo;
    sIterator.mExtentPid = SMP_NULL_PID;
    sIterator.mBuildExtentPageCount = aArgs->mBuildExtentPageCount;

    for( i = 0; i < SMNMPB_MAX_INDEX_DEPTH + 1; i++ )
    {
        sIterator.mPageId[i] = aArgs[0].mPageId[i];
        sIterator.mFirstPageId[i] = aArgs[0].mFirstPageId[i];
    }
    
    for( i = 1; i < aArgs->mParallelFactor; i++ )
    {
        sHeight = 0;
        while( aArgs[i].mFirstPageId[sHeight] != SMP_NULL_PID )
        {
            STL_DASSERT( sIterator.mPageId[sHeight] != SMP_NULL_PID );

            STL_TRY( smnmpbMergeTreeLevel( &sIterator,
                                           &aArgs[i],
                                           sHeight,
                                           &sCompareInfo,
                                           aEnv )
                     == STL_SUCCESS );

            sHeight++;
            
            /**
             * Parent Event 검사
             */

            STL_TRY( knlCheckParentQueryTimeout( KNL_ENV(aArgs->mParentEnv),
                                                 KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        
        for( j = 0; j < aArgs[i].mHeight; j++ )
        {
            STL_DASSERT( aArgs[i].mPageId[j] != SMP_NULL_PID );
            sIterator.mPageId[j] = aArgs[i].mPageId[j];
        }
    }

    /**
     * Iterator에 남아있는 페이지를 삭제한다.
     */

    if( sIterator.mExtentPid != SMP_NULL_PID )
    {
        while( sIterator.mPageSeq < sIterator.mBuildExtentPageCount )
        {
            STL_TRY( smxmBegin( &sMiniTrans,
                                aArgs->mTransId,
                                smsGetSegRid( SME_GET_SEGMENT_HANDLE( aArgs->mIndexHandle ) ),
                                sAttr | SMXM_ATTR_NO_VALIDATE_PAGE,
                                aEnv )
                     == STL_SUCCESS );
            sState = 2;
        
            STL_TRY( smpAcquire( &sMiniTrans,
                                 aArgs->mTbsId,
                                 sIterator.mExtentPid + sIterator.mPageSeq,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv )
                     == STL_SUCCESS );
        
            STL_TRY( smsUpdatePageStatus( &sMiniTrans,
                                          SME_GET_SEGMENT_HANDLE( aArgs->mIndexHandle ),
                                          &sPageHandle,
                                          SMP_FREENESS_FREE,
                                          0,  /* aScn */
                                          aEnv )
                     == STL_SUCCESS );
        
            sState = 1;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

            sIterator.mPageSeq++;
        }
    }

    /**
     * 남은 Free extent를 segment로 반납한다.
     */
    
    STL_TRY( smnmpbFreeExtents( aArgs->mTransId,
                                aArgs->mIndexHandle,
                                aArgs->mBuildExtentPageCount,
                                &sFreeQueueInfo,
                                aEnv )
             == STL_SUCCESS );

    /**
     * Tree 결과를 return하기 위해서...
     */

    sHeight = 0;

    while( sIterator.mPageId[sHeight] != SMP_NULL_PID )
    {
        aArgs->mPageId[sHeight] = sIterator.mPageId[sHeight];
        aArgs->mFirstPageId[sHeight] = sIterator.mFirstPageId[sHeight];
        sHeight++;
    }

    aArgs->mHeight = sHeight - 1;

    sState = 0;
    STL_TRY( knlFiniQueue( &sFreeQueueInfo,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_DASSERT( SML_SESS_ENV(aEnv)->mFixCount == 0 );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smxmCommit( &sMiniTrans, aEnv );
        case 1:
            (void) knlFiniQueue( &sFreeQueueInfo, KNL_ENV(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smnmpbMergeTreeLevel( smnmpbTreeIterator * aIterator,
                                smnmpbParallelArgs * aArgs,
                                stlInt16             aHeight,
                                smnmpbCompareInfo  * aCompareInfo,
                                smlEnv             * aEnv )
{
    smnmpbLogicalHdr   sLogicalHdr;
    stlChar          * sPropKey;
    stlChar          * sPropKeyBody;
    stlInt16           sPropKeyBodySize;
    smlPid             sPrevPageId;
    smlPid             sRowPid;
    stlInt16           sRowOffset;
    stlInt16           sTotalKeyCount;
    smpHandle          sPageHandleA;
    smpHandle          sPageHandleB;
    smpHandle          sPageHandleC;
    smpHandle          sPageHandleP;
    smpRowItemIterator sKeyIterator;
    stlInt16           sKeySize;
    stlChar          * sSrcKey;
    stlChar          * sDstKey;
    stlInt16           sSlotId;
    smlRid             sSegmentRid;
    void             * sSegmentHandle;
    dtlCompareResult   sResult = DTL_COMPARISON_EQUAL;

    aIterator->mMiniTransState = 0;
    
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aArgs->mIndexHandle );
    sSegmentRid = smsGetSegRid( sSegmentHandle );

    STL_TRY( smxmBegin( aIterator->mMiniTrans,
                        aIterator->mTransId,
                        sSegmentRid,
                        aIterator->mMiniTransAttr,
                        aEnv )
             == STL_SUCCESS );
    aIterator->mMiniTransState = 1;
    
    STL_TRY( smpAcquire( aIterator->mMiniTrans,
                         aIterator->mTbsId,
                         aIterator->mPageId[aHeight],
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandleA,
                         aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smpAcquire( aIterator->mMiniTrans,
                         aIterator->mTbsId,
                         aArgs->mFirstPageId[aHeight],
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandleB,
                         aEnv )
             == STL_SUCCESS );
    
    SMP_GET_FIRST_ROWITEM( SMP_FRAME(&sPageHandleB),
                           &sKeyIterator,
                           sSrcKey );

    /**
     * check uniqueness
     */

    if( (aArgs->mIsUniqueIndex == STL_TRUE) &&
        (aHeight == 0) )
    {
        sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandleA);
        sDstKey = smpGetNthRowItem( &sPageHandleA, sTotalKeyCount - 1 );

        sResult = smnmpbCompare4MergeTree( aCompareInfo,
                                           sDstKey + SMNMPB_MIN_LEAF_KEY_HDR_LEN,
                                           sSrcKey + SMNMPB_MIN_LEAF_KEY_HDR_LEN );
        
        STL_TRY_THROW( (aArgs->mIsUniqueIndex != STL_TRUE) ||
                       (sResult != DTL_COMPARISON_EQUAL )  ||
                       (aCompareInfo->mUniqueViolated == 0),
                       RAMP_ERR_UNIQUE_VIOLATED );
    }
                
    while( sSrcKey != NULL )
    {
        if( aHeight == 0 )
        {
            sKeySize = smnmpbGetLeafKeySize( sSrcKey,
                                             aIterator->mKeyColCount );
        }
        else
        {
            sKeySize = smnmpbGetIntlKeySize( sSrcKey,
                                             aIterator->mKeyColCount );
        }
        
        /**
         * Max Key 크기를 포함해서 할당 가능한지 판단한다.
         */
        STL_TRY( smpAllocSlot( &sPageHandleA,
                               ( sKeySize +
                                 STL_SIZEOF(smlPid) +
                                 STL_SIZEOF(smpOffsetSlot) ),
                               STL_TRUE,
                               &sDstKey,
                               &sSlotId )
                 == STL_SUCCESS );

        if( sDstKey == NULL )
        {
            /**
             * Propagate Key
             */
            sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandleA);
            sPropKey = smpGetNthRowItem( &sPageHandleA,
                                         sTotalKeyCount - 1 );

            if( aHeight == 0 )
            {
                sPropKeyBody = sPropKey + SMNMPB_MIN_LEAF_KEY_HDR_LEN;
                sPropKeyBodySize = ( smnmpbGetLeafKeySize( sPropKey,
                                                           aIterator->mKeyColCount ) -
                                     SMNMPB_MIN_LEAF_KEY_HDR_LEN );

                SMNMPB_GET_ROW_PID( sPropKey, &sRowPid );
                SMNMPB_GET_ROW_SEQ( sPropKey, &sRowOffset );
            }
            else
            {
                sPropKeyBody = sPropKey + SMNMPB_INTL_SLOT_HDR_SIZE;
                sPropKeyBodySize = ( smnmpbGetIntlKeySize( sPropKey,
                                                           aIterator->mKeyColCount ) -
                                     SMNMPB_INTL_SLOT_HDR_SIZE);

                SMNMPB_INTL_SLOT_GET_ROW_PID( sPropKey, &sRowPid );
                SMNMPB_INTL_SLOT_GET_ROW_OFFSET( sPropKey, &sRowOffset );
            }
        
            STL_TRY( smnmpbPropageKey4Build( aIterator,
                                             aHeight + 1,
                                             aIterator->mPageId[aHeight],
                                             sPropKeyBody,
                                             sPropKeyBodySize,
                                             sRowPid,
                                             sRowOffset,
                                             aEnv )
                     == STL_SUCCESS );

            sPrevPageId = aIterator->mPageId[aHeight];

            /**
             * Link to sibling node
             */
            STL_TRY( smnmpbAllocPage4Build( aIterator,
                                            &aIterator->mPageId[aHeight],
                                            aEnv )
                     == STL_SUCCESS );
            if( aHeight == 0 )
            {
                SMNMPB_SET_NEXT_PAGE( &sPageHandleA,
                                      aIterator->mPageId[aHeight] );
            }
            else
            {
                sPrevPageId = SMP_NULL_PID;
            }

            /**
             * Bulk logging
             */
            STL_TRY( smxmWriteLog( aIterator->mMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                                   SMR_REDO_TARGET_PAGE,
                                   SMP_FRAME(&sPageHandleA),
                                   SMP_PAGE_SIZE,
                                   aIterator->mTbsId,
                                   smpGetPageId(&sPageHandleA),
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );

            STL_TRY( smpAcquire( aIterator->mMiniTrans,
                                 aIterator->mTbsId,
                                 aIterator->mPageId[aHeight],
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandleA,
                                 aEnv )
                     == STL_SUCCESS );

            stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
            sLogicalHdr.mPrevPageId = sPrevPageId;
            sLogicalHdr.mNextPageId = SMP_NULL_PID;
            sLogicalHdr.mNextFreePageId = SMP_NULL_PID;
            sLogicalHdr.mLevel = aHeight;
            sLogicalHdr.mKeyColCount = aIterator->mKeyColCount;
            stlMemcpy( &sLogicalHdr.mKeyColTypes,
                       aIterator->mKeyColTypes,
                       aIterator->mKeyColCount );
            stlMemcpy( &sLogicalHdr.mKeyColFlags,
                       aIterator->mKeyColFlags,
                       aIterator->mKeyColCount );

            if( aHeight == 0 )
            {
                STL_TRY( smpInitBody( &sPageHandleA,
                                      SMP_ORDERED_SLOT_TYPE,
                                      STL_SIZEOF(smnmpbLogicalHdr),
                                      (void*)&sLogicalHdr,
                                      SMN_GET_INITRANS(aIterator->mIndexHandle),
                                      SMN_GET_MAXTRANS(aIterator->mIndexHandle),
                                      STL_FALSE,
                                      aIterator->mMiniTrans,
                                      aEnv) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( smpInitBody( &sPageHandleA,
                                      SMP_ORDERED_SLOT_TYPE,
                                      STL_SIZEOF(smnmpbLogicalHdr),
                                      (void*)&sLogicalHdr,
                                      0,
                                      0,
                                      STL_FALSE,
                                      aIterator->mMiniTrans,
                                      aEnv) == STL_SUCCESS );
            }
        
        }
        
        STL_TRY( smpAllocSlot( &sPageHandleA,
                               sKeySize,
                               STL_FALSE,
                               &sDstKey,
                               &sSlotId )
                 == STL_SUCCESS );
        
        stlMemcpy( sDstKey, sSrcKey, sKeySize );
        SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(&sPageHandleA) );
        
        SMP_GET_NEXT_ROWITEM( &sKeyIterator, sSrcKey );
    }


    /**
     *  [A] <-> [B] <-> [C]
     *    ==>
     *  [A] <-> [A'] <-> [C] or [A] <-> [C]  
     */
    
    /**
     * A 의 High Key를 부모노드에 올린다.
     */
    
    if( aHeight < aArgs->mHeight )
    {
        sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandleA);
        sPropKey = smpGetNthRowItem( &sPageHandleA,
                                     sTotalKeyCount - 1 );
    
        if( aHeight == 0 )
        {
            sPropKeyBody = sPropKey + SMNMPB_MIN_LEAF_KEY_HDR_LEN;
            sPropKeyBodySize = ( smnmpbGetLeafKeySize( sPropKey,
                                                       aIterator->mKeyColCount ) -
                                 SMNMPB_MIN_LEAF_KEY_HDR_LEN );

            SMNMPB_GET_ROW_PID( sPropKey, &sRowPid );
            SMNMPB_GET_ROW_SEQ( sPropKey, &sRowOffset );
        }
        else
        {
            sPropKeyBody = sPropKey + SMNMPB_INTL_SLOT_HDR_SIZE;
            sPropKeyBodySize = ( smnmpbGetIntlKeySize( sPropKey,
                                                       aIterator->mKeyColCount ) -
                                 SMNMPB_INTL_SLOT_HDR_SIZE);

            SMNMPB_INTL_SLOT_GET_ROW_PID( sPropKey, &sRowPid );
            SMNMPB_INTL_SLOT_GET_ROW_OFFSET( sPropKey, &sRowOffset );
        }

        STL_TRY( smnmpbPropageKey4Build( aIterator,
                                         aHeight + 1,
                                         aIterator->mPageId[aHeight],
                                         sPropKeyBody,
                                         sPropKeyBodySize,
                                         sRowPid,
                                         sRowOffset,
                                         aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * A 와 C 를 연결한다.
     */

    if( aHeight < aArgs->mHeight )
    {
        if( aHeight == 0 )
        {
            STL_TRY( smpAcquire( aIterator->mMiniTrans,
                                 aIterator->mTbsId,
                                 SMNMPB_GET_NEXT_PAGE( &sPageHandleB ),
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandleC,
                                 aEnv )
                     == STL_SUCCESS );

            SMNMPB_SET_PREV_PAGE( &sPageHandleC,
                                  smpGetPageId( &sPageHandleA ) );
            SMNMPB_SET_NEXT_PAGE( &sPageHandleA,
                                  smpGetPageId( &sPageHandleC ) );
        
            STL_TRY( smxmWriteLog( aIterator->mMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                                   SMR_REDO_TARGET_PAGE,
                                   SMP_FRAME(&sPageHandleA),
                                   SMP_PAGE_SIZE,
                                   aIterator->mTbsId,
                                   smpGetPageId(&sPageHandleA),
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );
        
            STL_TRY( smxmWriteLog( aIterator->mMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                                   SMR_REDO_TARGET_PAGE,
                                   SMP_FRAME(&sPageHandleC),
                                   SMP_PAGE_SIZE,
                                   aIterator->mTbsId,
                                   smpGetPageId(&sPageHandleC),
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smxmWriteLog( aIterator->mMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                                   SMR_REDO_TARGET_PAGE,
                                   SMP_FRAME(&sPageHandleA),
                                   SMP_PAGE_SIZE,
                                   aIterator->mTbsId,
                                   smpGetPageId(&sPageHandleA),
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );
        }
    }

    /**
     * B 를 삭제한다.
     */
    
    STL_TRY( smsUpdatePageStatus( aIterator->mMiniTrans,
                                  sSegmentHandle,
                                  &sPageHandleB,
                                  SMP_FREENESS_FREE,
                                  0,  /* aScn */
                                  aEnv )
             == STL_SUCCESS );

    /**
     * B 를 참조하고 있는 부모노드의 internal key 삭제
     */

    if( aHeight < aArgs->mHeight )
    {
        STL_TRY( smpAcquire( aIterator->mMiniTrans,
                             aIterator->mTbsId,
                             aArgs->mFirstPageId[aHeight+1],
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandleP,
                             aEnv )
                 == STL_SUCCESS );

        sDstKey = smpGetNthRowItem( &sPageHandleP, 0 );
        sKeySize = smnmpbGetIntlKeySize( sDstKey, aIterator->mKeyColCount );

        STL_TRY( smpFreeNthSlot( &sPageHandleP,
                                 0,  /* aSlotSeq */
                                 sKeySize )
                 == STL_SUCCESS );
    }

    aIterator->mMiniTransState = 0;
    STL_TRY( smxmCommit( aIterator->mMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_UNIQUE_VIOLATED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INDEX_UNIQUENESS_VIOLATION,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( aIterator->mMiniTransState == 1 )
    {
        (void)smxmCommit( aIterator->mMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

    
stlStatus smnmpbAllocExtent4Build( smxlTransId    aTransId,
                                   stlInt32       aPageCount,
                                   void         * aSegmentHandle,
                                   knlQueueInfo * aFreeQueueInfo,
                                   smlPid       * aExtentPid,
                                   smlEnv       * aEnv )
{
    stlBool               sIsEmpty = STL_TRUE;
    smpHandle             sPageHandle;
    stlInt32              sUsablePageCount;
    smnmpbBbuildPageHdr   sPageHdr;
    smlTbsId              sTbsId;
    stlInt32              sState = 0;
    stlInt32              i;
    smlPid                sExtentPid;
    
    sUsablePageCount = smsGetUsablePageCountInExt( aSegmentHandle );
    sTbsId = smsGetTbsId( aSegmentHandle );

    STL_TRY( knlDequeue( aFreeQueueInfo,
                         aExtentPid,
                         &sIsEmpty,
                         KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    if( sIsEmpty == STL_TRUE )
    {
        STL_TRY( smsAllocExtent( aTransId,
                                 aSegmentHandle,
                                 SMP_PAGE_TYPE_INDEX_DATA,
                                 aExtentPid,
                                 aEnv )
                 == STL_SUCCESS );

        stlMemset( &sPageHdr, 0x00, STL_SIZEOF( smnmpbBbuildPageHdr ) );
        
        /**
         * Initialize data pages
         */
    
        for( i = 0; i < sUsablePageCount; i++ )
        {
            STL_TRY( smpFix( sTbsId,
                             *aExtentPid + i,
                             &sPageHandle,
                             aEnv )
                     == STL_SUCCESS );
            sState = 1;

            STL_TRY( smpInitBody( &sPageHandle,
                                  SMP_ORDERED_SLOT_TYPE,
                                  STL_SIZEOF(smnmpbBbuildPageHdr),
                                  (stlChar*)&sPageHdr,
                                  0,
                                  0,
                                  STL_FALSE,
                                  NULL,
                                  aEnv )
                     == STL_SUCCESS );

            sState = 0;
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
        }

        sExtentPid = (*aExtentPid + aPageCount);
        
        while( sExtentPid < (*aExtentPid + sUsablePageCount) )
        {
            STL_TRY( knlEnqueue( aFreeQueueInfo,
                                 &sExtentPid,
                                 KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sExtentPid += aPageCount;
        }

        STL_DASSERT( sExtentPid == (*aExtentPid + sUsablePageCount) );
    }
    else
    {
        /**
         * Reset page body & logical hdr
         */
        
        for( i = 0; i < aPageCount; i++ )
        {
            STL_TRY( smpFix( sTbsId,
                             *aExtentPid + i,
                             &sPageHandle,
                             aEnv )
                     == STL_SUCCESS );
            sState = 1;

            STL_TRY( smpResetBody( &sPageHandle ) == STL_SUCCESS );

            SMNMPB_SET_NEXT_EXTENT( &sPageHandle, SMP_NULL_PID );

            sState = 0;
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }
    
    return STL_FAILURE;
}

stlStatus smnmpbGetMaxKey( smnmpbParallelArgs * aArgs,
                           smlPid               aExtentPid,
                           stlChar            * aMaxKey,
                           smlEnv             * aEnv )
{
    stlInt32    i;
    smpHandle   sPageHandle;
    stlInt32    sKeyCount;
    stlInt16    sKeySize;
    stlChar   * sSrcKey;
    smlPid      sTargetPid;

    sTargetPid = aExtentPid + (aArgs->mBuildExtentPageCount - 1);
    
    for( i = 0; i < aArgs->mBuildExtentPageCount; i++ )
    {
        STL_TRY( smpFix( aArgs->mTbsId,
                         aExtentPid + i,
                         &sPageHandle,
                         SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );

        if( sKeyCount == 0 )
        {
            sTargetPid = aExtentPid + (i - 1);

            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
            
            break;
        }
            
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smpFix( aArgs->mTbsId,
                     sTargetPid,
                     &sPageHandle,
                     SML_ENV(aEnv) )
             == STL_SUCCESS );

    sKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );
    
    sSrcKey = smpGetNthRowItem( &sPageHandle,
                                sKeyCount - 1 );

    SMNMPB_RUN_KEY_GET_TOTAL_SIZE( sSrcKey, &sKeySize );

    stlMemcpy( aMaxKey, sSrcKey, sKeySize );

    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbSplitRun( smnmpbParallelArgs  * aArgs,
                          smnmpbBuildIterator * aIterator,
                          stlChar             * aMaxKey,
                          smnmpbCompareInfo   * aCompareInfo,
                          knlQueueInfo        * aFreeQueueInfo,
                          smlPid              * aExtentPid,
                          smlEnv              * aEnv )
{
    smlPid               sExtentPidA;
    smlPid               sExtentPidB = aIterator->mCurExtentPid;
    dtlCompareResult     sResult;
    stlInt32             i;
    stlInt32             j;
    stlInt32             sTotalKeyCount;
    stlInt32             sState = 0;
    stlInt32             sStateA = 0;
    stlInt32             sStateB = 0;
    smpHandle            sPageHandle;
    smpHandle            sPageHandleA;
    smpHandle            sPageHandleB;
    stlChar            * sKey;
    smpRowItemIterator   sRowItemIterator;
    stlInt16             sKeySize;
    stlInt16             sSlotId;
    stlChar            * sDstKey = NULL;

    *aExtentPid = SMP_NULL_PID;
    
    STL_TRY_THROW( aIterator->mCurExtentPid != SMP_NULL_PID, RAMP_SUCCESS );

    STL_TRY( smpFix( aIterator->mTbsId,
                     aIterator->mCurExtentPid,
                     &sPageHandle,
                     SML_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    sKey = smpGetNthRowItem( &sPageHandle, 0 );

    sResult = aArgs->mMergeCompare( aCompareInfo,
                                    aMaxKey,
                                    sKey );

    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( sResult == DTL_COMPARISON_GREATER, RAMP_SUCCESS );

    /**
     * allocate A and B extent
     */
    
    STL_TRY( smnmpbAllocExtent4Build( aArgs->mTransId,
                                      aArgs->mBuildExtentPageCount,
                                      SME_GET_SEGMENT_HANDLE(aArgs->mIndexHandle),
                                      aFreeQueueInfo,
                                      &sExtentPidA,
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smnmpbAllocExtent4Build( aArgs->mTransId,
                                      aArgs->mBuildExtentPageCount,
                                      SME_GET_SEGMENT_HANDLE(aArgs->mIndexHandle),
                                      aFreeQueueInfo,
                                      &sExtentPidB,
                                      aEnv )
             == STL_SUCCESS );

    /**
     * A 파트 구성
     */
    
    for( i = 0; i < aArgs->mBuildExtentPageCount; i++ )
    {
        STL_TRY( smpFix( aIterator->mTbsId,
                         aIterator->mCurExtentPid + i,
                         &sPageHandle,
                         SML_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 1;

        sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );
        
        /**
         * grouping시 max의 최소값을 추출했기 때문에 남아 있는 run들중 aMaxKey보다
         * 모두 작은 run은 존재할수 없다.
         */
        STL_DASSERT( sTotalKeyCount > 0 );

        sKey = smpGetNthRowItem( &sPageHandle, sTotalKeyCount - 1 );

        sResult = aArgs->mMergeCompare( aCompareInfo,
                                        aMaxKey,
                                        sKey );

        if( sResult == DTL_COMPARISON_LESS )
        {
            sState = 0;
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
            
            break;
        }
        
        STL_DASSERT( sResult == DTL_COMPARISON_GREATER );

        STL_TRY( smpFix( aIterator->mTbsId,
                         sExtentPidA + i,
                         &sPageHandleA,
                         SML_ENV(aEnv) )
                 == STL_SUCCESS );
        sStateA = 1;

        stlMemcpy( SMP_FRAME( &sPageHandleA ) + STL_SIZEOF(smpPhyHdr),
                   SMP_FRAME( &sPageHandle ) + STL_SIZEOF(smpPhyHdr),
                   SMP_PAGE_SIZE - STL_SIZEOF(smpPhyHdr));
            
        sStateA = 0;
        STL_TRY( smpUnfix( &sPageHandleA, aEnv ) == STL_SUCCESS );
        
        sState = 0;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    /**
     * 적어도 한 페이지는 split되어야 한다.
     */
    STL_DASSERT( i < aArgs->mBuildExtentPageCount );

    /**
     * 페이지를 분할해서 A와 B에 분배한다.
     */
    
    STL_TRY( smpFix( aIterator->mTbsId,
                     aIterator->mCurExtentPid + i,
                     &sPageHandle,
                     SML_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smpFix( aIterator->mTbsId,
                     sExtentPidA + i,
                     &sPageHandleA,
                     SML_ENV(aEnv) )
             == STL_SUCCESS );
    sStateA = 1;
    
    STL_TRY( smpFix( aIterator->mTbsId,
                     sExtentPidB,
                     &sPageHandleB,
                     SML_ENV(aEnv) )
             == STL_SUCCESS );
    sStateB = 1;

    SMNMPB_SET_NEXT_EXTENT( &sPageHandleB, aIterator->mNextExtentPid );
    
    SMP_GET_FIRST_ROWITEM( SMP_FRAME( &sPageHandle ),
                           &sRowItemIterator,
                           sKey );
        
    while( sKey != NULL )
    {
        sResult = aArgs->mMergeCompare( aCompareInfo,
                                        aMaxKey,
                                        sKey );

        SMNMPB_RUN_KEY_GET_TOTAL_SIZE( sKey, &sKeySize );

        if( sResult == DTL_COMPARISON_GREATER )
        {
            STL_TRY( smpAllocSlot( &sPageHandleA,
                                   sKeySize,
                                   STL_FALSE,
                                   &sDstKey,
                                   &sSlotId )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smpAllocSlot( &sPageHandleB,
                                   sKeySize,
                                   STL_FALSE,
                                   &sDstKey,
                                   &sSlotId )
                     == STL_SUCCESS );
        }

        stlMemcpy( sDstKey, sKey, sKeySize );

        SMP_GET_NEXT_ROWITEM( &sRowItemIterator, sKey );
    }

    sStateB = 0;
    STL_TRY( smpUnfix( &sPageHandleB, aEnv ) == STL_SUCCESS );

    sStateA = 0;
    STL_TRY( smpUnfix( &sPageHandleA, aEnv ) == STL_SUCCESS );

    /**
     * B 파트의 시작 위치 설정
     */
    
    if( SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle ) > 0 )
    {
        i = i + 1;
        j = 1;
    }
    else
    {
        j = 0;
    }
        
    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    
    /**
     * 남은 페이지는 B 파트로 
     */
    
    for( ; i < aArgs->mBuildExtentPageCount; i++, j++ )
    {
        STL_TRY( smpFix( aIterator->mTbsId,
                         aIterator->mCurExtentPid + i,
                         &sPageHandle,
                         SML_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 1;

        STL_TRY( smpFix( aIterator->mTbsId,
                         sExtentPidB + j,
                         &sPageHandleB,
                         SML_ENV(aEnv) )
                 == STL_SUCCESS );
        sStateB = 1;

        stlMemcpy( SMP_FRAME( &sPageHandleB ) + STL_SIZEOF(smpPhyHdr),
                   SMP_FRAME( &sPageHandle ) + STL_SIZEOF(smpPhyHdr),
                   SMP_PAGE_SIZE - STL_SIZEOF(smpPhyHdr));
            
        sStateB = 0;
        STL_TRY( smpUnfix( &sPageHandleB, aEnv ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    /**
     * 기존 extent는 free queue 에 저장
     */
    
    STL_TRY( knlEnqueue( aFreeQueueInfo,
                         &aIterator->mCurExtentPid,
                         KNL_ENV(aEnv) )
             == STL_SUCCESS );

    *aExtentPid = sExtentPidA;

    STL_RAMP( RAMP_SUCCESS );

    aIterator->mCurExtentPid = SMP_NULL_PID;
    aIterator->mNextExtentPid = sExtentPidB;
    aIterator->mPageSeq = -1;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    if( sStateA == 1 )
    {
        (void)smpUnfix( &sPageHandleA, aEnv );
    }

    if( sStateB == 1 )
    {
        (void)smpUnfix( &sPageHandleB, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smnmpbMaximizeFanoutRootPage( smxmTrans           * aMiniTrans,
                                        smnmpbParallelArgs  * aArgs,
                                        smpHandle           * aRootPageHandle,
                                        smlEnv              * aEnv )
{
    stlUInt16                sUnusedSpace;
    smnmpbLogicalHdr         sLogicalHdr;
    smlPid                   sOrgChildPid;
    smlPid                   sPrevPageId;
    smlPid                   sNewChildPid;
    smlPid                   sOrgRowPid;
    stlInt16                 sOrgRowOffset = 0;
    smlPid                   sNewRowPid;
    stlInt16                 sNewRowOffset;
    stlInt16                 sSlotId;
    stlChar                * sOrgKey;
    stlChar                * sOrgKeySlot;
    stlChar                * sNewKeySlot;
    stlInt16                 sOrgKeySize;
    stlInt16                 sNewKeySize;
    stlChar                * sPropKeyBody;
    stlInt16                 sPropKeyBodySize = 0;
    stlInt32                 sTotalKeyCount;
    stlInt32                 i;
    stlBool                  sIsSuccess = STL_TRUE;
    stlBool                  sIsSplitted = STL_FALSE;
    stlInt64                 sRootPageFrame[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
    smpHandle                sTmpRootHandle;
    stlBool                  sMaximizeRoot;

    sMaximizeRoot = knlGetPropertyBoolValueByID( KNL_PROPERTY_INDEX_ROOT_MAXIMIZE_FANOUT,
                                                 KNL_ENV( aEnv ) );


    STL_TRY_THROW( sMaximizeRoot == STL_TRUE, SKIP_SPLIT );

    sTmpRootHandle.mFrame = sRootPageFrame;
    sTmpRootHandle.mPch = aRootPageHandle->mPch;

    while( STL_TRUE )
    {
        /**
         * Split이 실패하면 더이상 split하지 않는다.
         */
        STL_TRY_THROW( sIsSuccess == STL_TRUE, SKIP_SPLIT );

        sUnusedSpace = smpGetUnusedSpace( SMP_FRAME( aRootPageHandle ) );
        sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( aRootPageHandle );

        /**
         * 1. Original root page의 복사본을 만든 후 body를 초기화한다.
         */
        stlMemcpy( SMP_FRAME(&sTmpRootHandle),
                   SMP_FRAME(aRootPageHandle),
                   SMP_PAGE_SIZE );

        stlMemcpy( (stlChar*)&sLogicalHdr,
                   SMNMPB_GET_LOGICAL_HEADER(aRootPageHandle),
                   STL_SIZEOF(smnmpbLogicalHdr) );

        STL_TRY( smpInitBody( aRootPageHandle,
                              SMP_ORDERED_SLOT_TYPE,
                              STL_SIZEOF(smnmpbLogicalHdr),
                              (void*)&sLogicalHdr,
                              0,
                              0,
                              STL_FALSE,
                              aMiniTrans,
                              aEnv) == STL_SUCCESS );

        sPrevPageId = SMP_NULL_PID;

        /**
         * 2. Root page 복사본에서 key slot을 하나씩 가져와서
         *    해당 child page를 split한다.
         */
        for( i = 0; i < sTotalKeyCount; i++ )
        {
            sOrgKey = smpGetNthRowItem( &sTmpRootHandle, i );
            SMNMPB_INTL_SLOT_GET_CHILD_PID( sOrgKey, &sOrgChildPid );

            /**
             * Last key check
             */
            if( i == (sTotalKeyCount - 1) )
            {
                /**
                 * Last key이면 key value가 없고, child page id만 있다.
                 */
                sOrgKeySize = STL_SIZEOF(smlPid);
                sOrgRowPid = SMP_NULL_PID;
            }
            else
            {
                /**
                 * Last key가 아니면 key slot에서 size와 row info를 구한다.
                 */
                sOrgKeySize = smnmpbGetIntlKeySize( sOrgKey,
                                                    SMNMPB_GET_KEY_COLUMN_COUNT(&sTmpRootHandle) );
                SMNMPB_INTL_SLOT_GET_ROW_PID( sOrgKey, &sOrgRowPid );
                SMNMPB_INTL_SLOT_GET_ROW_OFFSET( sOrgKey, &sOrgRowOffset );
            }

            /**
             * Child page를 split한다.
             */
            if( sIsSuccess == STL_TRUE )
            {
                STL_TRY( smnmpbSplitChildNode4Build( aArgs,
                                                     sUnusedSpace,
                                                     sOrgChildPid,
                                                     sPrevPageId,
                                                     &sPropKeyBody,
                                                     &sPropKeyBodySize,
                                                     &sNewChildPid,
                                                     &sNewRowPid,
                                                     &sNewRowOffset,
                                                     &sIsSuccess,
                                                     aEnv )
                         == STL_SUCCESS );
            }

            sNewKeySize = sPropKeyBodySize + SMNMPB_INTL_SLOT_HDR_SIZE;

            /**
             * Child page에서 split이 성공했으면 root page에 original child page와
             * new child page를 기록한다. 그렇지 않으면 original key를 그대로 복사한다.
             */
            if( sIsSuccess == STL_TRUE )
            {
                STL_TRY( smpAllocSlot( aRootPageHandle,
                                       sNewKeySize,
                                       STL_FALSE,
                                       &sNewKeySlot,
                                       &sSlotId ) == STL_SUCCESS );

                STL_ASSERT( sNewKeySlot != NULL );

                SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aRootPageHandle) );

                STL_TRY( smpAllocSlot( aRootPageHandle,
                                       sOrgKeySize,
                                       STL_FALSE,
                                       &sOrgKeySlot,
                                       &sSlotId ) == STL_SUCCESS );

                STL_ASSERT( sOrgKeySlot != NULL );

                SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aRootPageHandle) );

                /**
                 * New Key Slot을 채운다.
                 * : Original child page Id와 propagate된 key를 기록한다.
                 */
                SMNMPB_INTL_SLOT_SET_CHILD_PID( sNewKeySlot, &sOrgChildPid );
                SMNMPB_INTL_SLOT_SET_ROW_PID( sNewKeySlot, &sNewRowPid );
                SMNMPB_INTL_SLOT_SET_ROW_OFFSET( sNewKeySlot, &sNewRowOffset );

                stlMemcpy( sNewKeySlot + SMNMPB_INTL_SLOT_HDR_SIZE,
                           sPropKeyBody,
                           sPropKeyBodySize );

                /**
                 * Org Key Slot을 채운다.
                 * : New child page Id와 original key를 기록한다.
                 */
                SMNMPB_INTL_SLOT_SET_CHILD_PID( sOrgKeySlot, &sNewChildPid );

                if( sOrgRowPid != SMP_NULL_PID )
                {
                    /**
                     * Last key가 아니면 key value를 채운다.
                     */
                    SMNMPB_INTL_SLOT_SET_ROW_PID( sOrgKeySlot, &sOrgRowPid );
                    SMNMPB_INTL_SLOT_SET_ROW_OFFSET( sOrgKeySlot, &sOrgRowOffset );

                    stlMemcpy( sOrgKeySlot + SMNMPB_INTL_SLOT_HDR_SIZE,
                               sOrgKey + SMNMPB_INTL_SLOT_HDR_SIZE,
                               sOrgKeySize - SMNMPB_INTL_SLOT_HDR_SIZE );
                }

                sUnusedSpace -= (STL_SIZEOF(smpOffsetSlot) + sPropKeyBodySize + SMNMPB_INTL_SLOT_HDR_SIZE);
                sPrevPageId = sNewChildPid;

                sIsSplitted = STL_TRUE;
            }
            else
            {
                /**
                 * 이전 child page가 split되었다면 prev page id를 변경해야 한다.
                 */
                STL_TRY( smnmpbAdjustPrevPageId( aArgs,
                                                 sOrgChildPid,
                                                 sPrevPageId,
                                                 aEnv ) == STL_SUCCESS );

                /**
                 * Split을 하지 못했을 경우 original key를 그대로 쓴다.
                 */
                STL_TRY( smpAllocSlot( aRootPageHandle,
                                       sOrgKeySize,
                                       STL_FALSE,
                                       &sOrgKeySlot,
                                       &sSlotId ) == STL_SUCCESS );

                STL_ASSERT( sOrgKeySlot != NULL );

                SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aRootPageHandle) );

                stlMemcpy( sOrgKeySlot, sOrgKey, sOrgKeySize );

                sPrevPageId = sOrgChildPid;
            }
        }
    }

    STL_RAMP( SKIP_SPLIT );

    /**
     * 3. fan-out이 증가된 root page를 logging한다.
     *  : 한번이라도 child page가 split되었으면 logging한다.
     */
    if( sIsSplitted == STL_TRUE )
    {
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                               SMR_REDO_TARGET_PAGE,
                               SMP_FRAME(aRootPageHandle),
                               SMP_PAGE_SIZE,
                               aArgs->mTbsId,
                               smpGetPageId(aRootPageHandle),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbSplitChildNode4Build( smnmpbParallelArgs  * aArgs,
                                      stlUInt16             aUnusedSpace,
                                      smlPid                aPageId,
                                      smlPid                aPrevPageId,
                                      stlChar            ** aPropKeyBody,
                                      stlInt16            * aPropKeyBodySize,
                                      smlPid              * aSplittedPid,
                                      smlPid              * aRowPid,
                                      stlInt16            * aRowOffset,
                                      stlBool             * aIsSuccess,
                                      smlEnv              * aEnv )
{
    void                * sSegmentHandle;
    smlRid                sSegmentRid;
    stlInt32              sAttr;
    smxmTrans             sMiniTrans;
    smpHandle             sOrgChildPage;
    smpHandle             sNewChildPage;
    stlUInt64             sSmoNo = 0;
    stlInt32              sState = 0;
    stlInt32              sTotalKeyCount;
    smnmpbLogicalHdr    * sOrgPageLogicalHdr;
    smnmpbLogicalHdr      sNewPageLogicalHdr;
    stlChar             * sPropKey;
    stlChar             * sPropKeyBody;
    stlUInt16             sKeySize;
    stlUInt16             sPropKeyBodySize;
    stlUInt16             sKeyColCount;
    stlUInt16             sLeftKeyCount;
    stlUInt16             sSplitFromSeq;

    *aIsSuccess = STL_FALSE;
    *aRowPid = SMP_NULL_PID;

    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aArgs->mIndexHandle );
    sSegmentRid = smsGetSegRid( sSegmentHandle );

    if( aArgs->mIsLogging == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }

    STL_TRY( smxmBegin( &sMiniTrans,
                        aArgs->mTransId,
                        sSegmentRid,
                        sAttr,
                        aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         aArgs->mTbsId,
                         aPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sOrgChildPage,
                         aEnv ) == STL_SUCCESS );

    sOrgPageLogicalHdr = SMNMPB_GET_LOGICAL_HEADER( &sOrgChildPage );

    sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sOrgChildPage);

    STL_TRY_THROW( sTotalKeyCount > 2, RAMP_SKIP_SPLIT );

    sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(&sOrgChildPage);

    /**
     * Total count를 이용하여 split 위치를 구하고 propagate될 key를 가져온다.
     */
    sLeftKeyCount = sTotalKeyCount / 2;
    sPropKey = smpGetNthRowItem( &sOrgChildPage, sLeftKeyCount );
    sSplitFromSeq = sLeftKeyCount + 1;

    /**
     * 새로운 page를 할당하기 전에 root node에 propagate될 key가
     * 저장될 공간이 있는지 체크한다.
     */
    if( SMNMPB_IS_LEAF(&sOrgChildPage) == STL_TRUE )
    {
        sKeySize = smnmpbGetLeafKeySize( sPropKey, sKeyColCount );
        sPropKeyBodySize = sKeySize - SMNMPB_MIN_LEAF_KEY_HDR_LEN;
    }
    else
    {
        sKeySize = smnmpbGetIntlKeySize( sPropKey, sKeyColCount );
        sPropKeyBodySize = sKeySize - SMNMPB_INTL_SLOT_HDR_SIZE;
    }

    /**
     * Propagate 될 key의 크기가 parent node의 남은 공간보다 크다면 split하지 않는다.
     */
    STL_TRY_THROW( (STL_SIZEOF(smpOffsetSlot) + sPropKeyBodySize + SMNMPB_INTL_SLOT_HDR_SIZE) < aUnusedSpace,
                   RAMP_SKIP_SPLIT );

    /**
     * Split을 위해 새로운 page를 할당하여 초기화 및 logical header를 설정한다.
     */
    STL_TRY( smsAllocPage( &sMiniTrans,
                           sSegmentHandle,
                           SMP_PAGE_TYPE_INDEX_DATA,
                           NULL,  /* aIsAgableFunc */
                           &sNewChildPage,
                           aEnv ) == STL_SUCCESS );

    smpSetVolatileArea( &sNewChildPage,
                        &sSmoNo,
                        STL_SIZEOF(stlUInt64) );

    stlMemset( &sNewPageLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
    sNewPageLogicalHdr.mPrevPageId = aPageId;
    sNewPageLogicalHdr.mNextPageId = sOrgPageLogicalHdr->mNextPageId;
    sNewPageLogicalHdr.mNextFreePageId = SMP_NULL_PID;
    sNewPageLogicalHdr.mLevel    = sOrgPageLogicalHdr->mLevel;
    sNewPageLogicalHdr.mFlag     = sOrgPageLogicalHdr->mFlag;
    sNewPageLogicalHdr.mKeyColCount = sOrgPageLogicalHdr->mKeyColCount;
    stlMemcpy( sNewPageLogicalHdr.mKeyColTypes,
               aArgs->mKeyColTypes,
               aArgs->mKeyColCount );
    stlMemcpy( sNewPageLogicalHdr.mKeyColFlags,
               aArgs->mKeyColFlags,
               aArgs->mKeyColCount );

    if( SMNMPB_IS_LEAF(&sOrgChildPage) == STL_TRUE )
    {
        STL_TRY( smpInitBody( &sNewChildPage,
                              SMP_ORDERED_SLOT_TYPE,
                              STL_SIZEOF(smnmpbLogicalHdr),
                              (void*)&sNewPageLogicalHdr,
                              SMN_GET_INITRANS(aArgs->mIndexHandle),
                              SMN_GET_MAXTRANS(aArgs->mIndexHandle),
                              STL_FALSE,
                              &sMiniTrans,
                              aEnv) == STL_SUCCESS );

        sPropKeyBody = sPropKey + SMNMPB_MIN_LEAF_KEY_HDR_LEN;
        SMNMPB_GET_ROW_PID( sPropKey, aRowPid );
        SMNMPB_GET_ROW_SEQ( sPropKey, aRowOffset );

        /**
         * Original Page에서 New Page로 key를 옮긴다.
         */
        STL_TRY( smnmpbCopyLeafKeys( &sOrgChildPage,
                                     &sNewChildPage,
                                     sSplitFromSeq,
                                     SMP_GET_PAGE_ROW_ITEM_COUNT( &sOrgChildPage ) - 1,
                                     aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smpInitBody( &sNewChildPage,
                              SMP_ORDERED_SLOT_TYPE,
                              STL_SIZEOF(smnmpbLogicalHdr),
                              (void*)&sNewPageLogicalHdr,
                              0,
                              0,
                              STL_FALSE,
                              &sMiniTrans,
                              aEnv) == STL_SUCCESS );

        sPropKeyBody = sPropKey + SMNMPB_INTL_SLOT_HDR_SIZE;
        SMNMPB_INTL_SLOT_GET_ROW_PID( sPropKey, aRowPid );
        SMNMPB_INTL_SLOT_GET_ROW_OFFSET( sPropKey, aRowOffset );

        /**
         * Original Page에서 New Page로 key를 옮긴다.
         */
        STL_TRY( smnmpbCopyIntlKeys( &sOrgChildPage,
                                     &sNewChildPage,
                                     sSplitFromSeq,
                                     SMP_GET_PAGE_ROW_ITEM_COUNT( &sOrgChildPage ) - 1,
                                     aEnv ) == STL_SUCCESS );
    }

    *aPropKeyBody = sPropKeyBody;
    *aPropKeyBodySize = sPropKeyBodySize;
    *aSplittedPid = smpGetPageId(&sNewChildPage);

    /**
     * Original Page에서 옮겨진 key를 제거한다.
     */
    sOrgPageLogicalHdr->mNextPageId = smpGetPageId(&sNewChildPage);
    sOrgPageLogicalHdr->mFlag = SMNMPB_NODE_FLAG_INIT;
    sOrgPageLogicalHdr->mPrevPageId = aPrevPageId;

    STL_TRY( smnmpbTruncateNode( NULL,
                                 &sOrgChildPage,
                                 sSplitFromSeq,
                                 aEnv ) == STL_SUCCESS );

    /**
     * Original Page를 logging한다.
     */
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                           SMR_REDO_TARGET_PAGE,
                           SMP_FRAME(&sOrgChildPage),
                           SMP_PAGE_SIZE,
                           aArgs->mTbsId,
                           smpGetPageId(&sOrgChildPage),
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    /**
     * New Page를 logging한다.
     */
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                           SMR_REDO_TARGET_PAGE,
                           SMP_FRAME(&sNewChildPage),
                           SMP_PAGE_SIZE,
                           aArgs->mTbsId,
                           smpGetPageId(&sNewChildPage),
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    /**
     * page logging
     */

    *aIsSuccess = STL_TRUE;

    STL_RAMP( RAMP_SKIP_SPLIT );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smnmpbAdjustPrevPageId( smnmpbParallelArgs  * aArgs,
                                  smlPid                aPageId,
                                  smlPid                aPrevPageId,
                                  smlEnv              * aEnv )
{
    void                * sSegmentHandle;
    smlRid                sSegmentRid;
    stlInt32              sAttr;
    stlInt32              sState = 0;
    stlInt32              sFixState = 0;
    smxmTrans             sMiniTrans;
    smpHandle             sPageHandle;
    smnmpbLogicalHdr    * sLogicalHdr;

    STL_TRY( smpFix( aArgs->mTbsId,
                     aPageId,
                     &sPageHandle,
                     aEnv )
             == STL_SUCCESS );
    sFixState = 1;

    sLogicalHdr = SMNMPB_GET_LOGICAL_HEADER( &sPageHandle );

    sFixState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

    /**
     * 기존의 prev page Id가 변함이 없으면 adjust할 필요 없다.
     */
    STL_TRY_THROW( sLogicalHdr->mPrevPageId != aPrevPageId,
                   RAMP_SKIP_ADJUST );

    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aArgs->mIndexHandle );
    sSegmentRid = smsGetSegRid( sSegmentHandle );

    if( aArgs->mIsLogging == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }

    STL_TRY( smxmBegin( &sMiniTrans,
                        aArgs->mTransId,
                        sSegmentRid,
                        sAttr,
                        aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         aArgs->mTbsId,
                         aPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sLogicalHdr = SMNMPB_GET_LOGICAL_HEADER( &sPageHandle );

    sLogicalHdr->mPrevPageId = aPrevPageId;

    /**
     * Original Page를 logging한다.
     */
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                           SMR_REDO_TARGET_PAGE,
                           SMP_FRAME(&sPageHandle),
                           SMP_PAGE_SIZE,
                           aArgs->mTbsId,
                           smpGetPageId(&sPageHandle),
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SKIP_ADJUST );

    return STL_SUCCESS;

    STL_FINISH;

    if( sFixState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */
