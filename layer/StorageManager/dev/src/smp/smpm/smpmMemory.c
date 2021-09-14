/*******************************************************************************
 * smpmMemory.c
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
#include <smDef.h>
#include <smfDef.h>
#include <smf.h>
#include <smpDef.h>
#include <smp.h>
#include <smg.h>
#include <smrDef.h>
#include <smr.h>
#include <smxl.h>
#include <smxm.h>
#include <smpManager.h>
#include <smpmMemory.h>

/**
 * @file smpmMemory.c
 * @brief Storage Manager Layer Memory Page Access Routines
 */

/**
 * @addtogroup smpmMemory
 * @{
 */

extern smpWarmupEntry * gSmpWarmupEntry;

smpAccessFuncs gMemoryPageAccessFuncs =
{
    smpmInit,
    smpmExtend,
    smpmShrink,
    smpmCreate,
    smpmFix
};

stlStatus smpmInit( smlTbsId        aTbsId,
                    smlDatafileId   aDatafileId,
                    smlEnv        * aEnv )
{
    smpCtrlHeader  * sPchArray;
    stlUInt64        i;
    stlUInt32        sMaxPageCount;
    stlUInt64        sShmChunk;
    smpPhyHdr      * sPhyHdr;
    void           * sFrame;
    stlBool          sIsValid = STL_FALSE;
    stlUInt32        sCorruptionCount = 0;
    stlInt32         sState = 0;
    knlRegionMark    sMemMark;

    STL_TRY_THROW( smfGetDatafileShmState(aTbsId, aDatafileId) <
                   SMF_DATAFILE_SHM_STATE_PCH_INIT,
                   RAMP_FINISH );

    STL_TRY( smfGetPchArray( aTbsId,
                             aDatafileId,
                             (void**)&sPchArray,
                             aEnv ) == STL_SUCCESS );

    sMaxPageCount = smfGetMaxPageCount( aTbsId, aDatafileId );
    sShmChunk = smfGetShmChunk( aTbsId, aDatafileId );
    
    for( i = 0; i < sMaxPageCount; i++ )
    {
        STL_TRY( smpInitPch( aTbsId,
                             SMP_MAKE_PID( aDatafileId, i ),
                             SML_TBS_DEVICE_MEMORY,
                             sShmChunk + (i * SMP_PAGE_SIZE),
                             &sPchArray[i],
                             aEnv ) == STL_SUCCESS );
        
        STL_TRY( knlGetPhysicalAddr( sPchArray[i].mFrame,
                                     (void**)&sFrame,
                                     (knlEnv*)aEnv ) == STL_SUCCESS );

        sPhyHdr = (smpPhyHdr*) sFrame;

        /**
         * Shared Memory에 Load 된 Data의 Page Validation.
         */ 
        STL_TRY( smpValidateChecksum( sFrame,
                                      &sIsValid,
                                      aEnv )
                 == STL_SUCCESS );

        if( sIsValid == STL_FALSE )
        {
            KNL_INIT_REGION_MARK( &sMemMark );
            STL_TRY( knlMarkRegionMem( &aEnv->mLogHeapMem,
                                       &sMemMark,
                                       KNL_ENV( aEnv ) ) == STL_SUCCESS );
            sState = 1;
            
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[PAGE VALIDATION] page corrupted "
                                "( tablespace : %d, page id : %d, sequence : %d )\n"
                                ".... datafile '%s'\n"
                                "PAGE : \n%s\n",
                                sPchArray[i].mTbsId,
                                sPchArray[i].mPageId,
                                SMP_PAGE_SEQ_ID( sPchArray[i].mPageId ),
                                smfGetDatafileName( sPchArray[i].mTbsId,
                                                    SMP_DATAFILE_ID( sPchArray[i].mPageId ) ),
                                knlDumpBinaryForAssert( sFrame,
                                                        SMP_PAGE_SIZE,
                                                        &aEnv->mLogHeapMem,
                                                        KNL_ENV(aEnv) ) )
                     == STL_SUCCESS );
            
            sPchArray[i].mIsCorrupted = STL_TRUE;
            sCorruptionCount++;

            sState = 0;
            STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mLogHeapMem,
                                               &sMemMark,
                                               STL_FALSE, /* aFreeChunk */
                                               KNL_ENV( aEnv ) ) == STL_SUCCESS );
        }

        /*
         * 메모리 DB는 System Boot-up 직후에 Volatile Area를 초기화 한다.
         */
        stlMemset( sPhyHdr->mVolatileArea,
                   0x00,
                   SMP_VOLATILE_AREA_SIZE );

        /**
         * Page의 Timestamp값이 Datafile의 Timestamp값과 다른 경우
         * - REUSE를 사용해서 생성한 datafile 일 경우에 발생함.
         * - 사용하지 않은 페이지로 간주한다.
         */
        if( sPhyHdr->mTimestamp == smfGetTimestamp( aTbsId, aDatafileId ) )
        {
            /*
             * Recovery Lsn을 설정한다.
             */
            sPchArray[i].mPageType = sPhyHdr->mPageType;
            sPchArray[i].mRecoveryLsn = sPhyHdr->mLsn;
            sPchArray[i].mRecoveryLpsn = SMR_MAX_LPSN;
        }
        else
        {
            sPchArray[i].mPageType = SMP_PAGE_TYPE_INIT;
            sPchArray[i].mRecoveryLsn = SMR_INVALID_LSN;
            sPchArray[i].mRecoveryLpsn = SMR_INVALID_LPSN;
        }
    }    

    smfSetDatafileCorruptionCount( aTbsId,
                                   aDatafileId,
                                   sCorruptionCount );

    smfSetDatafileShmState( aTbsId,
                            aDatafileId,
                            SMF_DATAFILE_SHM_STATE_PCH_INIT );
    
    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlFreeUnmarkedRegionMem( &aEnv->mLogHeapMem,
                                         &sMemMark,
                                         STL_FALSE, /* aFreeChunk */
                                         KNL_ENV( aEnv ) );
    }
    
    return STL_FAILURE;
}

stlStatus smpmExtend( smlStatement  * aStatement,
                      smlTbsId        aTbsId,
                      smlDatafileId   aDatafileId,
                      stlBool         aNeedPending,
                      smlEnv        * aEnv )
{
    stlUInt32           sMaxPageCount;
    smpPchArrayList   * sPchArrayList;
    smpPchArrayList   * sTailPchArrayList;
    smpAccessPendArgs   sPendingArgs;
    stlBool             sIsTimedOut;
    stlInt32            sState = 0;
    stlBool             sExistPchArray = STL_FALSE;

    sPchArrayList = gSmpWarmupEntry->mPchArrayList;
    
    while( sPchArrayList != NULL )
    {
        if( (sPchArrayList->mTbsId == aTbsId) &&
            (sPchArrayList->mDatafileId == aDatafileId) )
        {
            if( sPchArrayList->mState != SMP_PCHARRAY_STATE_INACTIVE )
            {
                sExistPchArray = STL_TRUE;
            }
            break;
        }
        
        sPchArrayList = sPchArrayList->mNext;
    }

    /**
     * sPchArrayList가 존재하고 STATE가 INACTIVE가 아니라면 SKIP
     */
    STL_TRY_THROW( sExistPchArray != STL_TRUE, RAMP_SUCCESS );
    
    if( sPchArrayList == NULL )
    {
        sMaxPageCount = smfGetMaxPageCount( aTbsId, aDatafileId );
    
        STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smpPchArrayList ),
                                       (void**)&sPchArrayList,
                                       aEnv ) == STL_SUCCESS );

        stlMemset( sPchArrayList, 0x00, STL_SIZEOF(smpPchArrayList) );

        sPchArrayList->mTbsId = aTbsId;
        sPchArrayList->mDatafileId = aDatafileId;
        sPchArrayList->mPchArray = SMF_GET_LOGICAL_PCH_ARRAY( aTbsId, aDatafileId );
        sPchArrayList->mPchCount = sMaxPageCount;
        sPchArrayList->mState = SMP_PCHARRAY_STATE_ACTIVE;

        STL_TRY( knlAcquireLatch( &gSmpWarmupEntry->mLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  KNL_ENV(aEnv) ) == STL_SUCCESS );
        sState = 1;
    
    
        if( gSmpWarmupEntry->mPchArrayList == NULL )
        {
            gSmpWarmupEntry->mPchArrayList = sPchArrayList;
        }
        else
        {
            sTailPchArrayList = gSmpWarmupEntry->mPchArrayList;
            while( sTailPchArrayList->mNext != NULL )
            {
                sTailPchArrayList = sTailPchArrayList->mNext;
            }

            sTailPchArrayList->mNext = sPchArrayList;
        }
    
        sState = 1;
        STL_TRY( knlReleaseLatch( &gSmpWarmupEntry->mLatch,
                                  KNL_ENV(aEnv) ) == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sPchArrayList->mState == SMP_PCHARRAY_STATE_INACTIVE );
        sPchArrayList->mState = SMP_PCHARRAY_STATE_ACTIVE;
    }

    stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smpAccessPendArgs) );
    sPendingArgs.mTbsId = aTbsId;
    sPendingArgs.mDatafileId = aDatafileId;

    if( (aStatement != NULL) && (aNeedPending == STL_TRUE) )
    {
        STL_TRY( smgAddPendOp( aStatement,
                               SMG_PEND_SHRINK_MEMORY,
                               SML_PEND_ACTION_ROLLBACK,
                               (void*)&sPendingArgs,
                               STL_SIZEOF( smpAccessPendArgs ),
                               0,
                               aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)knlReleaseLatch( &gSmpWarmupEntry->mLatch, KNL_ENV(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smpmShrink( smlStatement  * aStatement,
                      smlTbsId        aTbsId,
                      smlDatafileId   aDatafileId,
                      stlInt16        aPchArrayState,
                      smlEnv        * aEnv )
{
    smpAccessPendArgs sPendingArgs;
    smgPendOp         sPendOp;

    stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smpAccessPendArgs) );
    sPendingArgs.mTbsId = aTbsId;
    sPendingArgs.mDatafileId = aDatafileId;
    sPendingArgs.mPchArrayState = aPchArrayState;
    
    sPendOp.mArgs = (void*)(&sPendingArgs);
    sPendOp.mEventMem = NULL;
    sPendOp.mEventMemSize = 0;

    if( aStatement != NULL )
    {
        STL_TRY( smgAddPendOp( aStatement,
                               SMG_PEND_SHRINK_MEMORY,
                               SML_PEND_ACTION_COMMIT,
                               (void*)&sPendingArgs,
                               STL_SIZEOF( smpAccessPendArgs ),
                               0,
                               aEnv ) == STL_SUCCESS );
    }
    else
    {
        /**
         * Undo시에는 직접 삭제한다.
         */
        STL_TRY( smpmShrinkPending( SML_PEND_ACTION_COMMIT,
                                    &sPendOp,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpmShrinkPending( stlUInt32   aActionFlag,
                             smgPendOp * aPendOp,
                             smlEnv    * aEnv )
{
    smpPchArrayList   * sPchArrayList;
    smpAccessPendArgs * sArgs;

    sArgs = (smpAccessPendArgs*)aPendOp->mArgs;

    sPchArrayList = gSmpWarmupEntry->mPchArrayList;

    while( 1 )
    {
        /**
         * 원하는 PCH Array를 찾지 못하는 경우도 있다.
         * - Datafile Undo시에 강제적으로 Shrink 시킴과 동시에 Pending Operation도
         *   추가된 상황이라면 찾지 못할수도 있다.
         */
        
        if( sPchArrayList == NULL )
        {
            break;
        }

        /**
         * 이미 지워진 경우라면 다음으로 이동한다.
         */
        if( sPchArrayList->mState == SMP_PCHARRAY_STATE_DROPPED )
        {
            sPchArrayList = sPchArrayList->mNext;
            continue;
        }
        
        if( (sPchArrayList->mTbsId == sArgs->mTbsId) &&
            (sPchArrayList->mDatafileId == sArgs->mDatafileId) )
        {
            sPchArrayList->mState = sArgs->mPchArrayState;//SMP_PCHARRAY_STATE_DROPPED;

            stlMemBarrier();

            /**
             * DB Flusher에 의한 Dirty 페이지 접근 차단한다.
             * - Dirty된 페이지가 삭제된 데이터파일로의 저장 시도를 막아야 한다.
             */
            STL_TRY( smpmDiscardDirtyPages( sPchArrayList,
                                            aEnv )
                     == STL_SUCCESS );
            break;
        }

        sPchArrayList = sPchArrayList->mNext;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpmDiscardDirtyPages( smpPchArrayList * aPchArrayList,
                                 smlEnv          * aEnv )
{
    smpCtrlHeader   * sPchArray;
    stlUInt32         i;
    stlBool           sIsTimedOut;
    
    STL_TRY( knlGetPhysicalAddr( aPchArrayList->mPchArray,
                                 (void**)&sPchArray,
                                 (knlEnv*)aEnv ) == STL_SUCCESS );

    for( i = 0; i < aPchArrayList->mPchCount; i++ )
    {
        if( sPchArray[i].mDirty == STL_TRUE )
        {
            STL_TRY( knlAcquireLatch( &sPchArray[i].mPageLatch,
                                      KNL_LATCH_MODE_EXCLUSIVE,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      STL_INFINITE_TIME,
                                      &sIsTimedOut,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );

            sPchArray[i].mDirty = STL_FALSE;

            STL_TRY( knlReleaseLatch( &sPchArray[i].mPageLatch,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpmCreate( smxmTrans  * aMiniTrans,
                      smlTbsId     aTbsId,
                      smlPid       aPageId,
                      smpPageType  aPageType,
                      smpHandle  * aPageHandle,
                      smlEnv     * aEnv )
{
    smpCtrlHeader * sPch;
    
    sPch = aPageHandle->mPch;

    if( sPch->mPageType == SMP_PAGE_TYPE_INIT )
    {
        sPch->mTbsId = aTbsId;
        sPch->mPageId = aPageId;
        sPch->mPageType = aPageType;
        STL_ASSERT( sPch->mDirty == STL_FALSE );
        sPch->mDirty = STL_FALSE;
        sPch->mDeviceType = SML_TBS_DEVICE_MEMORY;
        sPch->mRecoveryLsn = SMR_INVALID_LSN;
    }
    else
    {
        sPch->mPageType = aPageType;
    }

    STL_TRY( knlGetPhysicalAddr( sPch->mFrame,
                                 &aPageHandle->mFrame,
                                 (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpmFix( smlTbsId     aTbsId,
                   smlPid       aPageId,
                   smpHandle  * aPageHandle,
                   smlEnv     * aEnv )
{
    smlDatafileId    sDatafileId;
    stlInt64         sPageSeqId;
    smpCtrlHeader  * sPch;
    void           * sFrame;
    
    sDatafileId = SMP_DATAFILE_ID( aPageId );
    sPageSeqId = SMP_PAGE_SEQ_ID( aPageId );

    KNL_SYSTEM_FATAL_DASSERT( sPageSeqId < smfGetMaxPageCount( aTbsId, sDatafileId ),
                              aEnv );
    
    STL_TRY( smfGetPchAndFrame( aTbsId,
                                sDatafileId,
                                sPageSeqId,
                                (void**)&sPch,
                                (void**)&sFrame,
                                aEnv )
             == STL_SUCCESS );
    
    aPageHandle->mPch = sPch;
    aPageHandle->mFrame = sFrame;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
