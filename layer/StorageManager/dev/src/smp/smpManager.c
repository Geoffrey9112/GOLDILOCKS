/*******************************************************************************
 * smpManager.c
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
#include <sms.h>
#include <smg.h>
#include <smrDef.h>
#include <smr.h>
#include <smxl.h>
#include <smxm.h>
#include <smpManager.h>
#include <smlGeneral.h>

/**
 * @file smpManager.c
 * @brief Storage Manager Layer Page Manager Routines
 */

extern smpWarmupEntry * gSmpWarmupEntry;
extern smpAccessFuncs gMemoryPageAccessFuncs;

smpAccessFuncs * gPageAccessFuncs[1] =
{
    &gMemoryPageAccessFuncs
};

/**
 * @addtogroup smp
 * @{
 */

/**
 * @brief Page Access 공간을 초기화 한다.( Memory Specific Function )
 * @param[in]  aTbsId Tablespace Identifier
 * @param[in]  aDatafileId Datafiel Identifier
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpInit( smlTbsId        aTbsId,
                   smlDatafileId   aDatafileId,
                   smlEnv        * aEnv )
{
    stlUInt32 sTbsTypeId;
    
    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SMF_GET_TBS_ATTR( aTbsId ) );
    
    STL_TRY( gPageAccessFuncs[sTbsTypeId]->mInit( aTbsId,
                                                  aDatafileId,
                                                  aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page Access 공간을 확장한다.( Memory Specific Function )
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aDatafileId Datafiel Identifier
 * @param[in] aNeedPending Pending 연산이 필요한지 여부
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpExtend( smlStatement  * aStatement,
                     smlTbsId        aTbsId,
                     smlDatafileId   aDatafileId,
                     stlBool         aNeedPending,
                     smlEnv        * aEnv )
{
    stlUInt32 sTbsTypeId;
    
    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SMF_GET_TBS_ATTR( aTbsId ) );
    
    STL_TRY( gPageAccessFuncs[sTbsTypeId]->mExtend( aStatement,
                                                    aTbsId,
                                                    aDatafileId,
                                                    aNeedPending,
                                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page Access 공간을 확장한다.( Memory Specific Function )
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aNeedPending Pending 연산이 필요한지 여부
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpExtendTablespace( smlStatement  * aStatement,
                               smlTbsId        aTbsId,
                               stlBool         aNeedPending,
                               smlEnv        * aEnv )
{
    stlBool         sDatafileExist;
    smlDatafileId   sDatafileId;
    
    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sDatafileExist,
                               aEnv ) == STL_SUCCESS );
        
    while( sDatafileExist == STL_TRUE )
    {
        STL_TRY( smpInit( aTbsId,
                          sDatafileId,
                          aEnv ) == STL_SUCCESS );
            
        STL_TRY( smpExtend( NULL,
                            aTbsId,
                            sDatafileId,
                            STL_FALSE,
                            aEnv ) == STL_SUCCESS );
                
        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sDatafileExist,
                                  aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page Access 공간을 축소한다.( Memory Specific Function )
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aDatafileId Datafiel Identifier
 * @param[in] aPchArrayState  PchArrayList가 지닐 State 
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpShrink( smlStatement  * aStatement,
                     smlTbsId        aTbsId,
                     smlDatafileId   aDatafileId,
                     stlInt16        aPchArrayState,
                     smlEnv        * aEnv )
{
    stlUInt32 sTbsTypeId;
    
    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SMF_GET_TBS_ATTR( aTbsId ) );
    
    STL_TRY( gPageAccessFuncs[sTbsTypeId]->mShrink( aStatement,
                                                    aTbsId,
                                                    aDatafileId,
                                                    aPchArrayState,
                                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mount Phase에서 Upload한 Offline Tablespace를 내린다.
 * @param[in]     aTbsId   tablespace id
 * @param[in,out] aEnv     environment
 */ 
stlStatus smpShrinkCorruptedTbs( smlTbsId   aTbsId,
                                 smlEnv   * aEnv )
{
    stlBool   sIsCorrupted = STL_FALSE;
    
    STL_TRY( smfIsCorruptedTbs( aTbsId,
                                &sIsCorrupted,
                                aEnv )
             == STL_SUCCESS );

    if( sIsCorrupted == STL_TRUE )
    {
        STL_TRY( smpShrinkTablespace( aTbsId, aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpShrinkTablespace( smlTbsId   aTbsId,
                               smlEnv   * aEnv )
{
    smlDatafileId   sDatafileId;
    stlBool         sExist;

    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sExist,
                               aEnv ) == STL_SUCCESS );

    while( sExist )
    {
        /**
         * database의 Corruption verify는 shrink 작업 이후에 datafile을 찾기 위해
         * 다시 한번 verify 작업을 하는데 중복 작업을 막기 위해 ShmState를 확인한다.
         * tablespace는 verify 작업을 하고 shrink를 한다.
         */
        STL_TRY( smpShrinkDatafile( aTbsId, sDatafileId, aEnv ) == STL_SUCCESS );

        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sExist,
                                  aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpShrinkDatafile( smlTbsId         aTbsId,
                             smlDatafileId    aDatafileId,
                             smlEnv         * aEnv )
{
    if( smfGetDatafileShmState(aTbsId, aDatafileId) > SMF_DATAFILE_SHM_STATE_ALLOCATED )
    {
        STL_TRY( smpShrink( NULL,   /* aStatement */
                            aTbsId,
                            aDatafileId,
                            SMP_PCHARRAY_STATE_INACTIVE,  /* aPchArrayState */
                            aEnv )
                 == STL_SUCCESS );

        smfSetDatafileShmState( aTbsId,
                                aDatafileId,
                                SMF_DATAFILE_SHM_STATE_ALLOCATED );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ShutDown시에 Page Access 공간내의 모든 dirty된 페이지를 디스크로 Flush한다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpFlushPage4Shutdown( smlEnv  * aEnv )
{
    smpParallelWriteJob  * sJob;
    stlInt32               sState = 0;
    stlChar                sData[STL_SIZEOF(void*)];
    smpPchArrayList      * sPchArrayList;
    smpParallelWriteArgs   sArg;
    smpParallelWriteArgs * sArgContext;
    stlBool                sDone;
    knlRegionMark          sMemMark;
    
    STL_RING_INIT_HEADLINK( &sArg.mJobs, STL_OFFSETOF( smpParallelWriteJob, mJobLink ) );
    STL_INIT_ERROR_STACK( &sArg.mErrorStack );
    sArg.mBehavior = SMP_FLUSH_DIRTY_PAGE;
    sArg.mLogging = STL_TRUE;
    sArg.mForCheckpoint = STL_FALSE;

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    sPchArrayList = gSmpWarmupEntry->mPchArrayList;

    while( sPchArrayList != NULL )
    {
        /**
         * 이미 지워진 경우라면 다음으로 이동한다.
         */
        if( sPchArrayList->mState == SMP_PCHARRAY_STATE_DROPPED )
        {
            sPchArrayList = sPchArrayList->mNext;
            continue;
        }

        if( sPchArrayList->mState == SMP_PCHARRAY_STATE_INACTIVE )
        {
            sPchArrayList = sPchArrayList->mNext;
            continue;
        }

        if( (SMF_IS_LOGGING_TBS( sPchArrayList->mTbsId ) == STL_FALSE) ||
            (SMF_IS_ONLINE_TBS( sPchArrayList->mTbsId ) == STL_FALSE) )
        {
            sPchArrayList = sPchArrayList->mNext;
            continue;
        }
        
        STL_TRY( knlAllocRegionMem( & SML_ENV(aEnv)->mOperationHeapMem,
                                    STL_SIZEOF( smpParallelWriteJob ),
                                    (void**)&sJob,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_RING_INIT_DATALINK( sJob, STL_OFFSETOF( smpParallelWriteJob, mJobLink ) );

        sJob->mPchArrayList = sPchArrayList;

        STL_RING_ADD_LAST( &sArg.mJobs, sJob );
        
        sPchArrayList = sPchArrayList->mNext;
    }

    sArgContext = &sArg;
    STL_WRITE_POINTER( sData, &sArgContext );
    
    STL_TRY( smpFlushPagesIoSlaveEventHandler( sData,
                                               STL_SIZEOF(void*),
                                               &sDone,
                                               aEnv )
             == STL_SUCCESS );
        
    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    if( sState == 1 )
    {
        (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                         &sMemMark,
                                         STL_FALSE, /* aFreeChunk */
                                         KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

/**
 * @brief 특정 데이타파일에 관련된 Dirty된 페이지들을 디스크에 기록한다.
 * @param[in]     aStatement  Statement 포인터
 * @param[in]     aTbsId      Tablespace Identifier
 * @param[in]     aReason     Flush page를 수행하는 원인(checkpoint, restore, flush)
 * @param[in]     aDatafileId Datafile Identifier
 * @param[in,out] aEnv        Environment 구조체
 * @note 특정 tablespace의 데이타파일을 flush하는 동안 BEGIN BACKUP이 수행되지 못하거나
 * <BR>수행될 수 없도록 설정한 후 flush를 수행하므로 flush 모듈 내부에서는 BEGIN BACKUP을
 * <BR>고려치 않는다.
 */
stlStatus smpFlushPagesForTablespace( smlStatement * aStatement,
                                      smlTbsId       aTbsId,
                                      stlInt32       aReason,
                                      stlInt32       aDatafileId,
                                      smlEnv       * aEnv )
{
    smpPchArrayList * sPchArrayList;
    smpCtrlHeader   * sPchArray;
    void            * sFrame;
    stlUInt32         i;
    stlBool           sLoggingTbs;
    knlRegionMark     sMemMark;
    stlInt32          sState = 0;
    stlChar         * sBuffer;
    stlChar         * sPageFrames;
    stlInt64          sFlushPageCount = 0;
    smlTbsId          sFirstTbsId = 0;
    smlPid            sFirstPageId = SMP_NULL_PID;
    stlInt64          sMaxFlushPageCount;
    smrLsn            sMaxFlushedLsn = SMR_INVALID_LSN;
    smrSbsn           sMaxFlushedSbsn = SMR_INVALID_SBSN;
    stlInt64          sChecksumType;
    stlBool           sTakenOffline = STL_FALSE;
    
    sMaxFlushPageCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MAXIMUM_FLUSH_PAGE_COUNT,
                                                        KNL_ENV( aEnv ) );

    sChecksumType = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PAGE_CHECKSUM_TYPE,
                                                   KNL_ENV( aEnv ) );
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                (sMaxFlushPageCount + 1) * SMP_PAGE_SIZE,
                                (void**)&sBuffer,
                                (knlEnv*)aEnv )
             == STL_SUCCESS );

    /**
     * Online Tablespace가 아니거나 Media Recovery 또는 Restore가 진행 중이지 않은
     * Offline Tablespace는 flush를 skip한다.
     */
    STL_TRY_THROW( ((SMF_IS_ONLINE_TBS( aTbsId ) == STL_TRUE) ||
                    (SMF_IS_TBS_PROCEED_MEDIA_RECOVERY( aTbsId ) == STL_TRUE) ||
                    (SMF_IS_TBS_PROCEED_RESTORE( aTbsId ) == STL_TRUE)),
                   RAMP_FLUSH_END );

    sPageFrames = (stlChar*)STL_ALIGN( (stlUInt64)sBuffer, SMP_PAGE_SIZE );

    sPchArrayList = gSmpWarmupEntry->mPchArrayList;

    while( sPchArrayList != NULL )
    {
        sTakenOffline = STL_FALSE;
        sFlushPageCount = 0;
        sMaxFlushedLsn = SMR_INVALID_LSN;
        sMaxFlushedSbsn = SMR_INVALID_SBSN;
        
        STL_TRY_THROW( aTbsId == sPchArrayList->mTbsId, RAMP_SKIP_FLUSH );
        STL_TRY_THROW( (aDatafileId == SML_INVALID_DATAFILE_ID) ||
                       (aDatafileId == sPchArrayList->mDatafileId),
                       RAMP_SKIP_FLUSH );
        
        /**
         * PchArrayList ACTIVE 상태가 아니라면 무시한다.
         */
        if( sPchArrayList->mState == SMP_PCHARRAY_STATE_DROPPED )
        {
            break;
        }
        
        STL_TRY( knlGetPhysicalAddr( sPchArrayList->mPchArray,
                                     (void**)&sPchArray,
                                     (knlEnv*)aEnv ) == STL_SUCCESS );

        sLoggingTbs = SMF_IS_LOGGING_TBS( sPchArrayList->mTbsId );
        STL_TRY_THROW( sLoggingTbs == STL_TRUE, RAMP_SKIP_FLUSH );

        for( i = 0; i < sPchArrayList->mPchCount; i++ )
        {
            STL_TRY( knlGetPhysicalAddr( sPchArray[i].mFrame,
                                         (void**)&sFrame,
                                         (knlEnv*)aEnv ) == STL_SUCCESS );

            if( sPchArray[i].mDirty == STL_TRUE )
            {
                if( sPchArray[i].mFirstDirtyLsn != SMR_INVALID_LSN )
                {
                    ((smpPhyHdr*)sFrame)->mLsn = sPchArray[i].mRecoveryLsn;
                    
                    sMaxFlushedLsn = STL_MAX( sMaxFlushedLsn, sPchArray[i].mRecoveryLsn );
                    sMaxFlushedSbsn = STL_MAX( sMaxFlushedSbsn, sPchArray[i].mRecoverySbsn );
                }
                
                sPchArray[i].mDirty = STL_FALSE;

                STL_DASSERT( ((smpPhyHdr*)sFrame)->mTimestamp ==
                             smfGetTimestamp( sPchArrayList->mTbsId,
                                              sPchArrayList->mDatafileId ) );

                stlMemcpy( &sPageFrames[sFlushPageCount * SMP_PAGE_SIZE],
                           sFrame,
                           SMP_PAGE_SIZE );
                
                if( sFlushPageCount == 0 )
                {
                    sFirstTbsId = sPchArray[i].mTbsId;
                    sFirstPageId = sPchArray[i].mPageId;
                }
                
                sFlushPageCount++;

                /**
                 * 연속된 페이지 크기가 정해진 크기를 넘어서면 디스크에 기록한다.
                 */
                if( sFlushPageCount >= sMaxFlushPageCount )
                {
                    STL_TRY( smrFlushLog( sMaxFlushedLsn,
                                          sMaxFlushedSbsn,
                                          STL_TRUE, /* aSwitchBlock */
                                          SML_TRANSACTION_CWM_WAIT,
                                          aEnv ) == STL_SUCCESS );

                    STL_TRY( smfWritePage( sFirstTbsId,
                                           sFirstPageId,
                                           (void*)sPageFrames,
                                           sFlushPageCount,
                                           sChecksumType,
                                           &sTakenOffline,
                                           aEnv ) == STL_SUCCESS );
                    
                    sFlushPageCount = 0;
                    sMaxFlushedLsn = SMR_INVALID_LSN;
                    sMaxFlushedSbsn = SMR_INVALID_SBSN;

                    STL_TRY_THROW( sTakenOffline == STL_FALSE, RAMP_SKIP_FLUSH );
                }
            }
            else
            {
                if( sFlushPageCount == 0 )
                {
                    continue;
                }
                
                stlMemcpy( &sPageFrames[sFlushPageCount * SMP_PAGE_SIZE],
                           sFrame,
                           SMP_PAGE_SIZE );
                
                sFlushPageCount++;
                
                /**
                 * 연속된 페이지가 저장되지 않는다면 이전 연속된 페이지를
                 * 디스크에 기록한다.
                 */
                if( sFlushPageCount >= sMaxFlushPageCount )
                {
                    STL_TRY( smrFlushLog( sMaxFlushedLsn,
                                          sMaxFlushedSbsn,
                                          STL_TRUE, /* aSwitchBlock */
                                          SML_TRANSACTION_CWM_WAIT,
                                          aEnv ) == STL_SUCCESS );

                    STL_TRY( smfWritePage( sFirstTbsId,
                                           sFirstPageId,
                                           (void*)sPageFrames,
                                           sFlushPageCount,
                                           sChecksumType,
                                           &sTakenOffline,
                                           aEnv ) == STL_SUCCESS );
                    sFlushPageCount = 0;
                    sMaxFlushedLsn = SMR_INVALID_LSN;
                    sMaxFlushedSbsn = SMR_INVALID_SBSN;

                    STL_TRY_THROW( sTakenOffline == STL_FALSE, RAMP_SKIP_FLUSH );
                }
            }
        }
        
        /**
         * 기록되지 않은 나머지 페이지를 디스크에 기록한다.
         */
        if( sFlushPageCount > 0 )
        {
            STL_TRY( smrFlushLog( sMaxFlushedLsn,
                                  sMaxFlushedSbsn,
                                  STL_TRUE, /* aSwitchBlock */
                                  SML_TRANSACTION_CWM_WAIT,
                                  aEnv ) == STL_SUCCESS );

            STL_TRY( smfWritePage( sFirstTbsId,
                                   sFirstPageId,
                                   (void*)sPageFrames,
                                   sFlushPageCount,
                                   sChecksumType,
                                   &sTakenOffline,
                                   aEnv ) == STL_SUCCESS );

            STL_TRY_THROW( sTakenOffline == STL_FALSE, RAMP_SKIP_FLUSH );
        }

        /**
         * tablespace의 datafile을 flush한 후 datafile header에
         * 가장 최근에 수행된 checkpoint Lsn을 박는다.
         */
        STL_TRY( smfChangeDatafileHeader( aStatement,
                                          aReason,
                                          sPchArrayList->mTbsId,
                                          sPchArrayList->mDatafileId,
                                          SMR_INVALID_LID,
                                          SMR_INVALID_LSN,
                                          aEnv )
                 == STL_SUCCESS );

        STL_RAMP( RAMP_SKIP_FLUSH );

        sPchArrayList = sPchArrayList->mNext;
    }

    STL_RAMP( RAMP_FLUSH_END );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                        &sMemMark,
                                        STL_FALSE, /* aFreeChunk */
                                        KNL_ENV(aEnv) );
    }
    
    return STL_FAILURE;
}


stlStatus smpFlushPagesIoSlaveEventHandler( void      * aData,
                                            stlUInt32   aDataSize,
                                            stlBool   * aDone,
                                            void      * aEnv )
{
    smpPchArrayList * sPchArrayList = NULL;
    smpCtrlHeader   * sPchArray;
    stlUInt32         i = 0;
    smrLsn            sMinFlushedLsn;
    stlTime           sBeginTime;
    stlInt64          sTotalFlushedPageCount = 0;
    knlRegionMark     sMemMark;
    stlInt32          sState = 0;
    stlChar         * sBuffer;
    stlChar         * sPageFrames;
    stlChar         * sFrame;
    stlInt32          sFlushPageCount = 0;
    smlTbsId          sFirstTbsId = 0;
    smlPid            sFirstPageId = SMP_NULL_PID;
    stlInt64          sMaxFlushPageCount;
    smrLsn            sMaxFlushedLsn = SMR_INVALID_LSN;
    smrSbsn           sMaxFlushedSbsn = SMR_INVALID_SBSN;
    stlInt32          sOffset = 0;
    stlBool           sIsSuccess;
    stlBool           sTakenOffline;
    stlBool           sLatched = STL_FALSE;
    stlInt64          sChecksumType;
    
    smpParallelWriteArgs * sArgs;
    smpParallelWriteJob  * sJob;
    smlEnv               * sEnv;

    *aDone = STL_FALSE;

    sEnv = (smlEnv*)aEnv;

    STL_READ_MOVE_POINTER( &sArgs, aData, sOffset );

    sMaxFlushPageCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MAXIMUM_FLUSH_PAGE_COUNT,
                                                        KNL_ENV( sEnv ) );

    sChecksumType = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PAGE_CHECKSUM_TYPE,
                                                   KNL_ENV( sEnv ) );
    /**
     * System Lsn은 아직 사용하지 않은 Lsn을 의미한다.
     * 따라서, 지금까지 사용된 Lsn중에 최대값은 "smrGetSystemLsn() - 1"이다.
     */
    sMinFlushedLsn = smrGetSystemLsn() - 1;

    sBeginTime = stlNow();

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &sEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(sEnv) )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &sEnv->mOperationHeapMem,
                                (sMaxFlushPageCount + 1) * SMP_PAGE_SIZE,
                                (void**)&sBuffer,
                                (knlEnv*)sEnv )
             == STL_SUCCESS );

    sPageFrames = (stlChar*)STL_ALIGN( (stlUInt64)sBuffer, SMP_PAGE_SIZE );

    STL_RING_FOREACH_ENTRY( &sArgs->mJobs, sJob )
    {
        sTakenOffline = STL_FALSE;
        sPchArrayList = sJob->mPchArrayList;

        /**
         * PchArrayList SACTIVE 상태가 아니라면 무시한다.
         */
        if( (sPchArrayList->mState == SMP_PCHARRAY_STATE_DROPPED) ||
            (sPchArrayList->mState == SMP_PCHARRAY_STATE_INACTIVE) )
        {
            break;
        }
        
        if( sArgs->mForCheckpoint == STL_TRUE )
        {
            /**
             * Checkpoint에 의해 datafile이 flush될 때 Backup이 함께 진행될 수 없다.
             * Tablespace의 datafile에 대해 Backup이 진행 중인지 체크하고,
             * Backup이 Begin되었으면 datafile의 flush를 skip하고,
             * 만약 Backup이 진행중이지 않으면 flush가 끝날 때까지 Backup이 begin되지
             * 않도록 BackupWaitinState를 set한다.
             */
            STL_TRY( smfSetBackupWaitingState( sPchArrayList->mTbsId,
                                               &sIsSuccess,
                                               aEnv ) == STL_SUCCESS );

            if( sIsSuccess != STL_TRUE )
            {
                /**
                 * datafile flush가 skip되었음
                 */
                SMF_TBS_SET_SKIP_FLUSH_STATE( sPchArrayList->mTbsId,
                                              sPchArrayList->mDatafileId,
                                              STL_TRUE );
                continue;
            }
            sLatched = STL_TRUE;
        }

        sFlushPageCount = 0;
        sMaxFlushedLsn = SMR_INVALID_LSN;
        sMaxFlushedSbsn = SMR_INVALID_SBSN;
        
        STL_TRY( knlGetPhysicalAddr( sPchArrayList->mPchArray,
                                     (void**)&sPchArray,
                                     KNL_ENV(sEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[IO SLAVE] flush datafile ( tablespace : %d, datafile : %d )\n",
                            sPchArrayList->mTbsId,
                            sPchArrayList->mDatafileId )
                 == STL_SUCCESS );
    
        for( i = 0; i < sPchArrayList->mPchCount; i++ )
        {
            if( (sPchArray[i].mDirty == STL_FALSE) && (sFlushPageCount == 0) )
            {
                continue;
            }
            
            STL_TRY( knlGetPhysicalAddr( sPchArray[i].mFrame,
                                         (void**)&sFrame,
                                         (knlEnv*)aEnv ) == STL_SUCCESS );

            while( 1 )
            {
                STL_TRY( knlTryLatch( &sPchArray[i].mPageLatch,
                                      KNL_LATCH_MODE_SHARED,
                                      &sIsSuccess,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );

                if( sIsSuccess == STL_FALSE )
                {
                    STL_TRY( knlCheckEndStatement( KNL_ENV(aEnv) ) == STL_SUCCESS );
                    stlBusyWait();
                }
                else
                {
                    sState = 2;
                    break;
                }
            }

            /**
             * latch 획득후 다시 checking
             */
            if( (sPchArray[i].mDirty == STL_FALSE) && (sFlushPageCount == 0) )
            {
                sState = 1;
                STL_TRY( knlReleaseLatch( &(sPchArray[i].mPageLatch),
                                          KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                continue;
            }
            
            if( sPchArray[i].mDirty == STL_TRUE )
            {
                if( sPchArray[i].mFirstDirtyLsn != SMR_INVALID_LSN )
                {
                    ((smpPhyHdr*)sFrame)->mLsn = sPchArray[i].mRecoveryLsn;
                    sPchArray[i].mFirstDirtyLsn = SMR_INVALID_LSN;
                    
                    sMaxFlushedLsn = STL_MAX( sMaxFlushedLsn, sPchArray[i].mRecoveryLsn );
                    sMaxFlushedSbsn = STL_MAX( sMaxFlushedSbsn, sPchArray[i].mRecoverySbsn );
                }
                else
                {
                    if( sArgs->mBehavior == SMP_FLUSH_LOGGED_PAGE )
                    {
                        /**
                         * 현재까지 Dirty Page가 없다면 Skip한다.
                         */
                        if( sFlushPageCount == 0 )
                        {
                            sState = 1;
                            STL_TRY( knlReleaseLatch( &(sPchArray[i].mPageLatch),
                                                      KNL_ENV( aEnv ) )
                                     == STL_SUCCESS );
                            
                            continue;
                        }
                    }
                }

                sPchArray[i].mDirty = STL_FALSE;

                STL_DASSERT( ((smpPhyHdr*)sFrame)->mTimestamp ==
                             smfGetTimestamp( sPchArrayList->mTbsId,
                                              sPchArrayList->mDatafileId ) );

                stlMemcpy( &sPageFrames[sFlushPageCount * SMP_PAGE_SIZE],
                           sFrame,
                           SMP_PAGE_SIZE );
                
                if( sFlushPageCount == 0 )
                {
                    sFirstTbsId = sPchArray[i].mTbsId;
                    sFirstPageId = sPchArray[i].mPageId;
                }
                
                sFlushPageCount++;
                sTotalFlushedPageCount++;
            }
            else
            {
                STL_DASSERT( sFlushPageCount > 0 );
                
                stlMemcpy( &sPageFrames[sFlushPageCount * SMP_PAGE_SIZE],
                           sFrame,
                           SMP_PAGE_SIZE );

                sFlushPageCount++;
                sTotalFlushedPageCount++;
            }

            sState = 1;
            STL_TRY( knlReleaseLatch( &(sPchArray[i].mPageLatch),
                                      KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            /**
             * 연속된 페이지 크기가 정해진 크기를 넘어서면 디스크에 기록한다.
             */
            if( sFlushPageCount >= sMaxFlushPageCount )
            {
                if( (sArgs->mLogging == STL_TRUE) && (sMaxFlushedLsn != SMR_INVALID_LSN) )
                {
                    STL_TRY( smrFlushLog( sMaxFlushedLsn,
                                          sMaxFlushedSbsn,
                                          STL_TRUE, /* aSwitchBlock */
                                          SML_TRANSACTION_CWM_WAIT,
                                          aEnv ) == STL_SUCCESS );
                }

                STL_TRY( smfWritePage( sFirstTbsId,
                                       sFirstPageId,
                                       (void*)sPageFrames,
                                       sFlushPageCount,
                                       sChecksumType,
                                       &sTakenOffline,
                                       aEnv ) == STL_SUCCESS );
                sFlushPageCount = 0;
                sMaxFlushedLsn = SMR_INVALID_LSN;
                sMaxFlushedSbsn = SMR_INVALID_SBSN;

                STL_TRY_THROW( sTakenOffline == STL_FALSE, RAMP_SKIP_FLUSH );
                
                /**
                 * PchArrayList SACTIVE 상태가 아니라면 무시한다.
                 */
                if( (sPchArrayList->mState == SMP_PCHARRAY_STATE_DROPPED) ||
                    (sPchArrayList->mState == SMP_PCHARRAY_STATE_INACTIVE) )
                {
                    break;
                }
            }
        }
            
        /**
         * 기록되지 않은 나머지 페이지를 디스크에 기록한다.
         */
        if( sFlushPageCount > 0 )
        {
            if( (sArgs->mLogging == STL_TRUE) && (sMaxFlushedLsn != SMR_INVALID_LSN) )
            {
                STL_TRY( smrFlushLog( sMaxFlushedLsn,
                                      sMaxFlushedSbsn,
                                      STL_TRUE, /* aSwitchBlock */
                                      SML_TRANSACTION_CWM_WAIT,
                                      sEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( smfWritePage( sFirstTbsId,
                                   sFirstPageId,
                                   (void*)sPageFrames,
                                   sFlushPageCount,
                                   sChecksumType,
                                   &sTakenOffline,
                                   sEnv ) == STL_SUCCESS );
            
            STL_TRY_THROW( sTakenOffline == STL_FALSE, RAMP_SKIP_FLUSH );
        }
    
        sFlushPageCount = 0;
        sMaxFlushedLsn = SMR_INVALID_LSN;
        sMaxFlushedSbsn = SMR_INVALID_SBSN;

        STL_RAMP( RAMP_SKIP_FLUSH );
        
        if( sLatched == STL_TRUE )
        {
            sLatched = STL_FALSE;
            STL_TRY( smfResetBackupWaitingState( sPchArrayList->mTbsId,
                                                 aEnv )
                     == STL_SUCCESS );
        }
    }

    STL_ASSERT( gSmpWarmupEntry->mMinFlushedLsn <= sMinFlushedLsn );
    
    sArgs->mSystemInfo.mMinFlushedLsn = sMinFlushedLsn;
    sArgs->mSystemInfo.mPageFlushingTime = (stlNow() - sBeginTime) / 1000;
    sArgs->mSystemInfo.mFlushedPageCount = sTotalFlushedPageCount;

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &sEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    *aDone = STL_TRUE;

    stlAppendErrors( &sArgs->mErrorStack,
                     KNL_ERROR_STACK( sEnv ) );

    return STL_SUCCESS;

    STL_FINISH;

    if( sLatched == STL_TRUE )
    {
        (void) smfResetBackupWaitingState( sPchArrayList->mTbsId, aEnv );
    }

    switch( sState )
    {
        case 2:
            (void) knlReleaseLatch( &(sPchArray[i].mPageLatch), KNL_ENV(aEnv) );
        case 1:
            (void) knlFreeUnmarkedRegionMem( &sEnv->mOperationHeapMem,
                                             &sMemMark,
                                             STL_FALSE, /* aFreeChunk */
                                             KNL_ENV(aEnv) );
        default:
            break;
    }

    stlAppendErrors( &sArgs->mErrorStack,
                     KNL_ERROR_STACK( sEnv ) );

    *aDone = STL_TRUE;

    return STL_FAILURE;
}


void smpGetSystemInfo( smpSystemInfo * aSystemInfo )
{
    aSystemInfo->mMinFlushedLsn = gSmpWarmupEntry->mMinFlushedLsn;
    aSystemInfo->mPageFlushingTime = gSmpWarmupEntry->mPageFlushingTime;
    aSystemInfo->mFlushedPageCount = gSmpWarmupEntry->mFlushedPageCount;
}

/**
 * @brief 시스템에서 Flush된 최소 Lsn을 반환한다.
 * @return Stable Minimum Log Sequence Number
 */
smrLsn smpGetMinFlushedLsn()
{
    return gSmpWarmupEntry->mMinFlushedLsn;
}

/**
 * @brief Page Flushing을 수행한다.
 * @param[in] aData 체크포인트 정보
 * @param[in] aDataSize 체크포인트 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smpPageFlushEventHandler( void      * aData,
                                    stlUInt32   aDataSize,
                                    stlBool   * aDone,
                                    void      * aEnv )
{
    stlInt64               sParallelFactor;
    smpParallelWriteJob  * sJob;
    stlInt32               sState = 0;
    stlInt32               i;
    stlChar                sData[STL_SIZEOF(void*)];
    stlInt64               sOffset = 0;
    smpPchArrayList      * sPchArrayList;
    stlBool                sNeedFlush;
    stlBool                sBuildAgableScn;
    stlInt16               sIoSlaveIdx;
    smpParallelWriteArgs * sArgs;
    smpParallelWriteArgs * sArgsContext;
    smrLsn                 sMinFlushedLsn;
    stlTime                sPageFlushingTime = 0;
    stlInt64               sTotalFlushedPageCount = 0;
    smpFlushArg            sArg;
    knlRegionMark          sMemMark;
    
    *aDone = STL_FALSE;

    STL_WRITE_BYTES( &sArg, aData, STL_SIZEOF(smpFlushArg) );

    sMinFlushedLsn = smrGetSystemLsn() - 1;
    
    sParallelFactor = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PARALLEL_IO_FACTOR,
                                                     KNL_ENV( aEnv ) );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( & SML_ENV(aEnv)->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * shared memory allocation 실패로 인한 checkpoint 실패를 방어하기
     * 위해서 heap memory를 사용해야 한다.
     * - gliese master에서 사용하는 heap memory는 상당히 작기 때문에
     *   극단적인 상황에서도 heap memory를 충분히 할당 받을수 있다.
     */
    
    STL_TRY( knlAllocRegionMem( & SML_ENV(aEnv)->mOperationHeapMem,
                                STL_SIZEOF(smpParallelWriteArgs) * sParallelFactor,
                                (void**)&sArgs,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;
    
    for( i = 0; i < sParallelFactor; i++ )
    {
        STL_RING_INIT_HEADLINK( &sArgs[i].mJobs,
                                STL_OFFSETOF( smpParallelWriteJob, mJobLink ) );

        STL_INIT_ERROR_STACK( &sArgs[i].mErrorStack );
        sArgs[i].mBehavior = sArg.mBehavior;
        sArgs[i].mLogging = sArg.mLogging;
        sArgs[i].mForCheckpoint = sArg.mForCheckpoint;
    }
    
    sPchArrayList = gSmpWarmupEntry->mPchArrayList;

    while( sPchArrayList != NULL )
    {
        /**
         * Flush를 skip하는 경우
         * 1. PchArrayList State가 ACTIVE가 아닌 경우
         * 2. NOLOGGING TABLESPACE의 경우
         */
        if( (sPchArrayList->mState != SMP_PCHARRAY_STATE_ACTIVE)      ||
            (SMF_IS_LOGGING_TBS( sPchArrayList->mTbsId ) == STL_FALSE) )
        {
            sPchArrayList = sPchArrayList->mNext;
            continue;
        }

        /**
         * Flush 전에 datafile info에 flush가 skip되었는지 여부를
         * STL_FALSE(flush가 skip되지 않았음)로 초기화한다.
         * IO Slave에서 flush가 skip된 datafile을 표시한다.
         */
        if( sArg.mForCheckpoint == STL_TRUE )
        {
            SMF_TBS_SET_SKIP_FLUSH_STATE( sPchArrayList->mTbsId,
                                          sPchArrayList->mDatafileId,
                                          STL_FALSE );
        }

        /**
         * OFFLINE TABLESPACE의 경우 Flush가 skip되었음을 trc에 기록
         */
        if( SMF_IS_ONLINE_TBS( sPchArrayList->mTbsId ) == STL_FALSE )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[FLUSHER] flush datafile was skipped"
                                "( tablespace(%d) is offline )\n",
                                sPchArrayList->mTbsId )
                     == STL_SUCCESS );

            sPchArrayList = sPchArrayList->mNext;
            continue;
        }
        
        if( sArg.mBehavior == SMP_FLUSH_LOGGED_PAGE )
        {
            STL_TRY( smpNeedFlushPchArray( sPchArrayList,
                                           &sNeedFlush,
                                           aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sNeedFlush = STL_TRUE;
        }

        if( sNeedFlush == STL_TRUE )
        {
            STL_TRY( knlAllocRegionMem( & SML_ENV(aEnv)->mOperationHeapMem,
                                        STL_SIZEOF( smpParallelWriteJob ),
                                        (void**)&sJob,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_RING_INIT_DATALINK( sJob, STL_OFFSETOF( smpParallelWriteJob, mJobLink ) );

            sJob->mPchArrayList = sPchArrayList;

            sIoSlaveIdx = smfGetIoSlaveId( sPchArrayList->mTbsId,
                                           sPchArrayList->mDatafileId );
            
            STL_RING_ADD_LAST( &sArgs[sIoSlaveIdx].mJobs, sJob );
        }
        
        sPchArrayList = sPchArrayList->mNext;
    }        
        
    for( i = 0; i < sParallelFactor; i++ )
    {
        if( STL_RING_IS_EMPTY( &sArgs[i].mJobs ) == STL_FALSE )
        {
            sArgsContext = &sArgs[i];
            
            sOffset = 0;
            STL_WRITE_MOVE_POINTER( sData, &sArgsContext, sOffset );
            
            STL_TRY( knlAddEnvEvent( SML_EVENT_IO_SLAVE_FLUSH_DATAFILE,
                                     NULL,            /* aEventMem */
                                     sData,
                                     sOffset,
                                     SML_IO_SLAVE_ENV_ID(i),
                                     KNL_EVENT_WAIT_NO,
                                     STL_TRUE, /* aForceSuccess */
                                     KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    
    for( i = 0; i < sParallelFactor; i++ )
    {
        if( STL_RING_IS_EMPTY( &sArgs[i].mJobs ) == STL_FALSE )
        {
            sBuildAgableScn = STL_FALSE;

            STL_TRY( knlAddEnvEvent( SML_EVENT_LOOPBACK,
                                     NULL,            /* aEventMem */
                                     &sBuildAgableScn,
                                     STL_SIZEOF(stlBool),
                                     SML_IO_SLAVE_ENV_ID(i),
                                     KNL_EVENT_WAIT_POLLING,
                                     STL_TRUE, /* aForceSuccess */
                                     KNL_ENV(aEnv) )
                     == STL_SUCCESS);
        }
    }

    for( i = 0; i < sParallelFactor; i++ )
    {
        if( STL_RING_IS_EMPTY( &sArgs[i].mJobs ) == STL_FALSE )
        {
            if( stlGetErrorStackDepth( &sArgs[i].mErrorStack ) > 0 )
            {
                stlAppendErrors( KNL_ERROR_STACK( aEnv ),
                                 &sArgs[i].mErrorStack );

                STL_TRY( stlHasWarningError( KNL_ERROR_STACK( aEnv ) ) == STL_TRUE );
            }

            sMinFlushedLsn = STL_MIN( sArgs[i].mSystemInfo.mMinFlushedLsn,
                                      sMinFlushedLsn );
            sPageFlushingTime = STL_MAX( sArgs[i].mSystemInfo.mPageFlushingTime,
                                         sPageFlushingTime );
            sTotalFlushedPageCount += sArgs[i].mSystemInfo.mFlushedPageCount;
        }
    }

    gSmpWarmupEntry->mMinFlushedLsn = sMinFlushedLsn;
    gSmpWarmupEntry->mPageFlushingTime = sPageFlushingTime;
    gSmpWarmupEntry->mFlushedPageCount = sTotalFlushedPageCount;
    
    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( & SML_ENV(aEnv)->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[PAGE FLUSHER] flushed lsn(%ld), flushed page count(%ld)]\n",
                        sMinFlushedLsn,
                        sTotalFlushedPageCount )
             == STL_SUCCESS );
    
    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;
    
    if( sState == 1 )
    {
        (void) knlFreeUnmarkedRegionMem( & SML_ENV(aEnv)->mOperationHeapMem,
                                         &sMemMark,
                                         STL_FALSE, /* aFreeChunk */
                                         KNL_ENV(aEnv) );
    }

    *aDone = STL_TRUE;

    return STL_FAILURE;
}

/**
 * @brief Tablespace 관련 Page Flushing을 수행한다.
 * @param[in] aData 체크포인트 정보
 * @param[in] aDataSize 체크포인트 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smpPageFlushTbsEventHandler( void      * aData,
                                       stlUInt32   aDataSize,
                                       stlBool   * aDone,
                                       void      * aEnv )
{
    smpFlushTbsArg  sArg;

    *aDone = STL_FALSE;

    STL_WRITE_BYTES( &sArg, aData, STL_SIZEOF(smpFlushTbsArg) );

    STL_TRY( smpFlushPagesForTablespace( NULL,        /* aStatement */
                                         sArg.mTbsId,
                                         sArg.mReason,
                                         sArg.mDatafileId,
                                         SML_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[PAGE FLUSHER] FLUSHED_LSN(%ld)]\n",
                        smpGetMinFlushedLsn() )
             == STL_SUCCESS );
    
    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;
    
    *aDone = STL_TRUE;

    return STL_FAILURE;
}

/**
 * @brief New 페이지를 위한 New Page Frame을 얻고, Physical Page Header를 초기화 한다.
 * @param[in] aMiniTrans Mini Transaction 포인터
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageId Page Identifier
 * @param[in] aPageType Page Type
 * @param[in] aSegmentId Segment Identifier
 * @param[in] aScn 페이지 타입에 따른 초기 Scn
 * @param[in] aPageHandle Page Access Handle
 * @param[in] aScn Page Access Handle
 * @param[in,out] aEnv Environment 구조체
 * @see aPageType : smpPageType
 * @remark aScn은 Undo Page에서는 Minimum transaction view scn으로 사용되고,
 * <BR) 그외 Page타입에서는 Free Scn으로 사용된다.
 */
stlStatus smpCreate( smxmTrans  * aMiniTrans,
                     smlTbsId     aTbsId,
                     smlPid       aPageId,
                     smpPageType  aPageType,
                     stlInt64     aSegmentId,
                     smlScn       aScn,
                     smpHandle  * aPageHandle,
                     smlEnv     * aEnv )
{
    stlUInt32       sTbsTypeId;
    stlBool         sIsTimedOut;
    smlDatafileId   sDatafileId;
    stlInt64        sPageSeqId;
    void          * sPchArray;

    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SMF_GET_TBS_ATTR( aTbsId ) );

    sDatafileId = SMP_DATAFILE_ID( aPageId );
    sPageSeqId = SMP_PAGE_SEQ_ID( aPageId );

    STL_TRY( smfGetPchArray( aTbsId,
                             sDatafileId,
                             &sPchArray,
                             aEnv ) == STL_SUCCESS );

    aPageHandle->mPch = sPchArray + (STL_SIZEOF(smpCtrlHeader) * sPageSeqId);

    STL_TRY( knlAcquireLatch( &aPageHandle->mPch->mPageLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( gPageAccessFuncs[sTbsTypeId]->mCreate( aMiniTrans,
                                                    aTbsId,
                                                    aPageId,
                                                    aPageType,
                                                    aPageHandle,
                                                    aEnv )
             == STL_SUCCESS );
    
    smxmPushPage( aMiniTrans,
                  KNL_LATCH_MODE_EXCLUSIVE,
                  *aPageHandle,
                  aEnv );

    STL_TRY( smpInitHdr( aMiniTrans,
                         aTbsId,
                         aPageHandle,
                         SML_INVALID_RID,
                         aPageType,
                         aSegmentId,
                         aScn,
                         aEnv ) == STL_SUCCESS );

#ifdef STL_DEBUG
    SML_SESS_ENV(aEnv)->mFixCount++;
#endif
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 aPageId에 해당하는 Page Frame을 메모리에 고정시키고
 *        페이지에 주어진 aLatchMode로 잠금을 획득한다.
 * @param[in] aMiniTrans Mini Transaction 포인터
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageId Page Identifier
 * @param[in] aLatchMode Latch Mode
 * @param[in] aPageHandle Page Access Handle
 * @param[in,out] aEnv Environment 구조체
 * @see aLatchMode : knlLatchMode
 */
stlStatus smpAcquire( smxmTrans  * aMiniTrans,
                      smlTbsId     aTbsId,
                      smlPid       aPageId,
                      stlUInt32    aLatchMode,
                      smpHandle  * aPageHandle,
                      smlEnv     * aEnv )
{
    stlBool sIsTimedOut;

    STL_TRY( smpFix( aTbsId,
                     aPageId,
                     aPageHandle,
                     aEnv ) == STL_SUCCESS );

    STL_TRY( knlAcquireLatch( &aPageHandle->mPch->mPageLatch,
                              aLatchMode,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    smxmPushPage( aMiniTrans,
                  aLatchMode,
                  *aPageHandle,
                  aEnv );

    STL_DASSERT( (aTbsId == smpGetTbsId(aPageHandle)) &&
                 (aPageId == smpGetPageId(aPageHandle)) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 aPageHandle에 해당하는 페이지에 주어진 aLatchMode로 잠금을 획득한다.
 * @param[in] aMiniTrans Mini Transaction 포인터
 * @param[in] aLatchMode Latch Mode
 * @param[in] aPageHandle Page Access Handle
 * @param[in,out] aEnv Environment 구조체
 * @see aLatchMode : knlLatchMode
 */
stlStatus smpAcquireWithHandle( smxmTrans  * aMiniTrans,
                                stlUInt32    aLatchMode,
                                smpHandle  * aPageHandle,
                                smlEnv     * aEnv )
{
    stlBool sIsTimedOut;

    STL_TRY( knlAcquireLatch( &aPageHandle->mPch->mPageLatch,
                              aLatchMode,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    
    smxmPushPage( aMiniTrans,
                  aLatchMode,
                  *aPageHandle,
                  aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 페이지의 Snapshot을 얻는다.
 * @param[in] aPageHandle Page Access Handle
 * @param[in] aPageFrame Snapshot pointer
 * @param[in,out] aEnv Environment 구조체
 * @see aLatchMode : knlLatchMode
 */
stlStatus smpSnapshot( smpHandle * aPageHandle,
                       stlChar   * aPageFrame,
                       smlEnv    * aEnv )
{
    stlUInt64 sExclLockSeqA;
    stlUInt64 sExclLockSeqB;
    stlBool   sIsTimedOut;

    sExclLockSeqA = knlGetExclLockSeq( &aPageHandle->mPch->mPageLatch );

    if( sExclLockSeqA & STL_INT64_C(0x0000000000000001) )
    {
        STL_TRY( knlAcquireLatch( &aPageHandle->mPch->mPageLatch,
                                  KNL_LATCH_MODE_SHARED,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  STL_INFINITE_TIME,
                                  &sIsTimedOut,
                                  (knlEnv*)aEnv ) == STL_SUCCESS );
        
        stlMemcpy( aPageFrame, aPageHandle->mFrame, SMP_PAGE_SIZE );
        
        STL_TRY( knlReleaseLatch( &aPageHandle->mPch->mPageLatch,
                                  (knlEnv*)aEnv ) == STL_SUCCESS );
    }
    else
    {
        stlMemBarrier();
        stlMemcpy( aPageFrame, aPageHandle->mFrame, SMP_PAGE_SIZE );

        sExclLockSeqB = knlGetExclLockSeq( &aPageHandle->mPch->mPageLatch );

        if( (sExclLockSeqA & STL_INT64_C(0x0000000000000001)) ||
            (sExclLockSeqA != sExclLockSeqB) )
        {
            STL_TRY( knlAcquireLatch( &aPageHandle->mPch->mPageLatch,
                                      KNL_LATCH_MODE_SHARED,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      STL_INFINITE_TIME,
                                      &sIsTimedOut,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );
        
            stlMemcpy( aPageFrame, aPageHandle->mFrame, SMP_PAGE_SIZE );
        
            STL_TRY( knlReleaseLatch( &aPageHandle->mPch->mPageLatch,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 aPageId에 aLatchMode로 잠금을 시도한다. <BR>
 *        성공하면 해당하는 Page Frame을 메모리에 고정시키고, <BR>
 *        실패하면 즉시 리턴된다.
 * @param[in] aMiniTrans Mini Transaction 포인터
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageId Page Identifier
 * @param[in] aLatchMode Latch Mode
 * @param[out] aPageHandle Page Access Handle
 * @param[out] aIsSuccess 성공 여부
 * @param[in,out] aEnv Environment 구조체
 * @see aLatchMode : knlLatchMode
 */
stlStatus smpTryAcquire( smxmTrans  * aMiniTrans,
                         smlTbsId     aTbsId,
                         smlPid       aPageId,
                         stlUInt32    aLatchMode,
                         smpHandle  * aPageHandle,
                         stlBool    * aIsSuccess,
                         smlEnv     * aEnv )
{
    stlInt32 sState = 0;
    
    STL_TRY( smpFix( aTbsId,
                     aPageId,
                     aPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlTryLatch( &aPageHandle->mPch->mPageLatch,
                          aLatchMode,
                          aIsSuccess,
                          (knlEnv*)aEnv ) == STL_SUCCESS );

    if( *aIsSuccess == STL_TRUE )
    {
        smxmPushPage( aMiniTrans,
                      aLatchMode,
                      *aPageHandle,
                      aEnv );
    }
    else
    {
        sState = 0;
        STL_TRY( smpUnfix( aPageHandle, aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( aPageHandle, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 aPageHandle을 해제한다.
 * @param[in] aPageHandle Page Access Handle
 * @param[in,out] aEnv Environment 구조체
 * @note 주어진 페이지에 Fix Count를 감소시키고, 획득한 잠금을 해제한다.
 */
stlStatus smpRelease( smpHandle * aPageHandle,
                      smlEnv    * aEnv )
{
    STL_TRY( knlReleaseLatch( &aPageHandle->mPch->mPageLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    
    STL_TRY( smpUnfix( aPageHandle,
                       aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 aPageId에 aLatchMode로 잠금을 테스트한다.
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageId Page Identifier
 * @param[out] aIsSuccess 성공 여부
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpTestExclusive( smlTbsId     aTbsId,
                            smlPid       aPageId,
                            stlBool    * aIsSuccess,
                            smlEnv     * aEnv )
{
    stlInt32  sState = 0;
    smpHandle sPageHandle;

    *aIsSuccess = STL_FALSE;
    
    STL_TRY( smpFix( aTbsId,
                     aPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    if( knlCurLatchMode( &sPageHandle.mPch->mPageLatch ) == KNL_LATCH_MODE_INIT )
    {
        *aIsSuccess = STL_TRUE;
    }
    
    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 페이지의 Page Frame을 메모리에 고정시킨다.
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageId Page Identifier
 * @param[in] aPageHandle Page Access Handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpFix( smlTbsId     aTbsId,
                  smlPid       aPageId,
                  smpHandle  * aPageHandle,
                  smlEnv     * aEnv )
{
    stlUInt32 sTbsTypeId;
    
    sTbsTypeId = SML_TBS_DEVICE_TYPE_ID( SMF_GET_TBS_ATTR( aTbsId ) );

    STL_TRY( gPageAccessFuncs[sTbsTypeId]->mFix( aTbsId,
                                                 aPageId,
                                                 aPageHandle,
                                                 aEnv )
             == STL_SUCCESS );

#ifdef STL_DEBUG
    SML_SESS_ENV(aEnv)->mFixCount++;
#endif
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 고정된 Page Frame을 해제한다.
 * @param[in] aPageHandle Page Access Handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpUnfix( smpHandle * aPageHandle,
                    smlEnv    * aEnv )
{
#ifdef STL_DEBUG
    SML_SESS_ENV(aEnv)->mFixCount--;
    STL_ASSERT( SML_SESS_ENV(aEnv)->mFixCount >= 0 );
#endif
    return STL_SUCCESS;
}

/**
 * @brief Page가 유효한지 검증힌다.
 * @param[in] aPageHandle Page Handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpValidatePage( smpHandle * aPageHandle,
                           smlEnv    * aEnv )
{
#ifdef STL_VALIDATE_DATABASE
    smpPhyHdr  * sPhyHdr;
    stlChar    * sFrame;
    
    sFrame = SMP_FRAME( aPageHandle );
    sPhyHdr = (smpPhyHdr*)sFrame;
    
    KNL_ASSERT( sPhyHdr->mPageType < SMP_PAGE_TYPE_MAX,
                aEnv,
                SMP_FORMAT_PHYSICAL_HDR( sPhyHdr ) );
    
    if( (sPhyHdr->mPageType == SMP_PAGE_TYPE_TABLE_DATA) ||
        (sPhyHdr->mPageType == SMP_PAGE_TYPE_INDEX_DATA) )
    {
        STL_TRY( smpValidateOffsetTableHeader( sFrame,
                                               aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#else
    return STL_SUCCESS;
#endif
}

/**
 * @brief Physical Page Hdr를 초기화 한다.
 * @param[in] aMiniTrans Mini-transaction Pointer
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageHandle Page Access Handle
 * @param[in] aParentRid Parent Record Identifier
 * @param[in] aPageType 페이지 타입
 * @param[in] aSegmentId Segment Identifier
 * @param[in] aScn 페이지 타입에 따른 초기 Scn
 * @param[in,out] aEnv Environment 구조체
 * @see aPageType: smpPageType
 * @remark aScn은 Undo Page에서는 Minimum transaction view scn으로 사용되고,
 * <BR) 그외 Page타입에서는 Free Scn으로 사용된다.
 */
stlStatus smpInitHdr( smxmTrans   * aMiniTrans,
                      smlTbsId      aTbsId,
                      smpHandle   * aPageHandle,
                      smlRid        aParentRid,
                      smpPageType   aPageType,
                      stlInt64      aSegmentId,
                      smlScn        aScn,
                      smlEnv      * aEnv )
{
    stlUInt64 sTimestamp;
    
    aPageHandle->mPch->mPageType = aPageType;
    
    sTimestamp = smfGetTimestamp( aTbsId,
                                  SMP_DATAFILE_ID( smpGetPageId(aPageHandle) ) );
    
    SMP_INIT_PHYSICAL_HDR( SMP_GET_PHYSICAL_HDR( aPageHandle->mFrame ),
                           aPageHandle->mPch,
                           aPageType,
                           aScn,
                           sTimestamp,
                           aSegmentId,
                           aParentRid );

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_PAGE_ACCESS,
                           SMR_LOG_INIT_PAGE,
                           SMR_REDO_TARGET_PAGE,
                           (void*)SMP_GET_PHYSICAL_HDR( aPageHandle->mFrame ),
                           STL_SIZEOF( smpPhyHdr ),
                           aTbsId,
                           smpGetPageId( aPageHandle ),
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page의 Freeness 정보를 갱신한다.
 * @param[in] aMiniTrans Mini-transaction Pointer
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageHandle Page Access Handle
 * @param[in] aFreeness 변경할 페이지 Freeness
 * @param[in,out] aEnv Environment 구조체
 * @see aPageType: smpPageType
 */
stlStatus smpUpdateFreeness( smxmTrans   * aMiniTrans,
                             smlTbsId      aTbsId,
                             smpHandle   * aPageHandle,
                             smpFreeness   aFreeness,
                             smlEnv      * aEnv )
{
    smpPhyHdr   * sPhyHdr;
    smpFreeness   sFreeness[2];
    
    sPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );

    sFreeness[0] = sPhyHdr->mFreeness;
    sPhyHdr->mFreeness = aFreeness;
    sFreeness[1] = sPhyHdr->mFreeness;
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sFreeness,
                           STL_SIZEOF( smpFreeness ) * 2,
                           aTbsId,
                           smpGetPageId( aPageHandle ),
                           STL_OFFSETOF( smpPhyHdr, mFreeness ),
                           SMXM_LOGGING_REDO_UNDO,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page의 Agable 여부를 판단한다.
 * @param[in] aPageHandle Page Access Handle
 * @param[in] aIsAgableFunc Agable Callback Function
 * @param[in,out] aEnv Environment 구조체
 */
stlBool smpIsAgable( smpHandle       * aPageHandle,
                     smpIsAgableFunc   aIsAgableFunc,
                     smlEnv          * aEnv )
{
    smlScn  sPageScn;
    stlBool sIsAgable = STL_FALSE;

    sPageScn = smpGetAgableScn(aPageHandle);

    STL_DASSERT( smpGetFreeness( aPageHandle ) == SMP_FREENESS_FREE );
    
    if( sPageScn == 0 )
    {
        return STL_TRUE;
    }

    /**
     * 페이지를 삭제할 당시 살아 있던 트랜잭션들이 모두 종료되었을때 재사용한다.
     */
    
    if( SMXL_IS_AGABLE_VIEW_SCN( smxlGetAgableViewScn(aEnv), sPageScn ) == STL_TRUE )
    {
        return STL_TRUE;
    }

    if( aIsAgableFunc != NULL )
    {
        (void) (*aIsAgableFunc)( aPageHandle, &sIsAgable, aEnv );
    }

    return sIsAgable;
}

/**
 * @brief Page의 AgableScn 정보를 갱신한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aPageHandle Page Access Handle
 * @param[in] aAgableScn 변경할 페이지의 AgableScn
 * @param[in,out] aEnv Environment 구조체
 * @see aPageType: smpPageType
 */
stlStatus smpUpdateAgableScn( smxmTrans   * aMiniTrans,
                              smpHandle   * aPageHandle,
                              smlScn        aAgableScn,
                              smlEnv      * aEnv )
{
    smpPhyHdr * sPhyHdr;
    smlScn      sAgableScn[2];
    
    sPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );
    
    sAgableScn[0] = sPhyHdr->mAgableScn;
    sAgableScn[1] = aAgableScn;
    
    sPhyHdr->mAgableScn = aAgableScn;

    if( aMiniTrans != NULL )
    {
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_RECOVERY,
                               SMR_LOG_BYTES,
                               SMR_REDO_TARGET_PAGE,
                               (void*)sAgableScn,
                               STL_SIZEOF( smlScn ) * 2,
                               smpGetTbsId( aPageHandle ),
                               smpGetPageId( aPageHandle ),
                               (stlInt64)&sPhyHdr->mAgableScn - (stlInt64)SMP_FRAME(aPageHandle),
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page의 Freeness를 얻는다.
 * @param[in] aPageHandle Page Access Handle
 */
inline smpFreeness smpGetFreeness( smpHandle * aPageHandle )
{
    smpPhyHdr * sPhyHdr;
    
    sPhyHdr = (smpPhyHdr*)(aPageHandle->mFrame);
    return sPhyHdr->mFreeness;
}

/**
 * @brief Page의 AgableScn을 얻는다.
 * @param[in] aPageHandle Page Access Handle
 */
inline smlScn smpGetAgableScn( smpHandle * aPageHandle )
{
    smpPhyHdr * sPhyHdr;
    
    sPhyHdr = (smpPhyHdr*)(aPageHandle->mFrame);
    return sPhyHdr->mAgableScn;
}

/**
 * @brief Page에 Dirty 상태를 설정한다.
 * @param[in] aPageHandle Page Access Handle
 * @param[in,out] aEnv Environment 구조체
 * @note 반드시 페이지의 recovery lsn을 설정한 후 dirty page를 설정해야 한다.
 */
stlStatus smpSetDirty( smpHandle * aPageHandle,
                       smlEnv    * aEnv )
{
    STL_DASSERT( aPageHandle->mPch->mPageType != SMP_PAGE_TYPE_INIT );
    aPageHandle->mPch->mDirty = STL_TRUE;
        
    return STL_SUCCESS;
}

/**
 * @brief Page Body의 물리적 포인터를 얻는다.
 * @param[in] aPage Page 포인터
 * @note aPage에서 smpPhyHdr 만큼 이후의 주소
 */
void * smpGetBody( void * aPage )
{
    return aPage + STL_SIZEOF( smpPhyHdr );
}

/**
 * @brief 현재 페이지에 걸려있는 Latch Mode를 얻는다.
 * @param[in] aPageHandle Page Access Handle
 * @return Latch Mode
 * @see knlLatchMode
 */
inline stlUInt16 smpCurLatchMode( smpHandle * aPageHandle )
{
    return knlCurLatchMode( &aPageHandle->mPch->mPageLatch );
}

/**
 * @brief 페이지에 First dirty Lsn과 Recovery Lsn을 설정한다.
 * @param[in] aPageHandle Page Access Handle
 * @param[in] aSbsn 설정할 Recovery Sbsn
 * @param[in] aLsn  설정할 Recovery Lsn
 * @param[in] aLpsn 설정할 Recovery Lpsn
 * @note 페이지를 갱신하고 recovery lsn을 설정한 후 dirty page flag를 설정한다.
 * 이는 recovery lsn을 설정할 때 first dirty lsn을 함께 설정하는데,
 * 만약 dirty page flag를 설정한 후 recovery lsn을 설정하면 first dirty lsn을
 * 설정할 수 없기 때문이다.
 */
inline void smpSetRecoveryLsn( smpHandle * aPageHandle,
                               smrSbsn     aSbsn,
                               smrLsn      aLsn,
                               stlInt16    aLpsn )
{
    smpCtrlHeader * sPch = aPageHandle->mPch;
    
    /**
     * First Dirty Lsn이 설정되지 않았다면 First dirty Lsn을 설정한다.
     */
    if( sPch->mFirstDirtyLsn == SMR_INVALID_LSN )
    {
        sPch->mFirstDirtyLsn = aLsn;
    }
    
    sPch->mRecoveryLsn = aLsn;
    sPch->mRecoverySbsn = aSbsn;
    sPch->mRecoveryLpsn = aLpsn;
}

/**
 * @brief 페이지 Recovery Lsn을 얻는다.
 * @param[in] aPageHandle Page Access Handle
 * @return Recovery Lsn
 */
inline smrLsn smpGetRecoveryLsn( smpHandle * aPageHandle )
{
    return aPageHandle->mPch->mRecoveryLsn;
}

/**
 * @brief 페이지 Recovery Lpsn을 얻는다.
 * @param[in] aPageHandle Page Access Handle
 * @return Recovery Lpsn
 */
inline stlInt16 smpGetRecoveryLpsn( smpHandle * aPageHandle )
{
    return aPageHandle->mPch->mRecoveryLpsn;
}

/**
 * @brief 페이지 핸들로부터 Page Id를 얻는다.
 * @param[in] aPageHandle Page Access Handle
 * @return Page Id
 */
smlPid smpGetPageId( smpHandle * aPageHandle )
{
    return aPageHandle->mPch->mPageId;
}

/**
 * @brief 페이지 핸들로부터 Tablespace Id를 얻는다.
 * @param[in] aPageHandle Page Access Handle
 * @return Tablespace Id
 */
smlTbsId smpGetTbsId( smpHandle * aPageHandle )
{
    return aPageHandle->mPch->mTbsId;
}

/**
 * @brief 페이지 핸들로부터 Page Type를 얻는다.
 * @param[in] aPageHandle Page Access Handle
 * @return Page Type
 * @see smpPageType
 */
smpPageType smpGetPageType( smpHandle * aPageHandle )
{
    return (smpPageType)(aPageHandle->mPch->mPageType);
}

/**
 * @brief 페이지 핸들로부터 Segment Identifer를 얻는다.
 * @param[in] aPageHandle Page Access Handle
 * @return Segment Identifier
 */
stlInt64 smpGetSegmentId( smpHandle * aPageHandle )
{
    smpPhyHdr * sPhyHdr;
    
    sPhyHdr = (smpPhyHdr*)(aPageHandle->mFrame);
    return sPhyHdr->mSegmentId;
}

/**
 * @brief MaxViewScn을 갱신한다.
 * @param[in] aPageHandle Page Access Handle
 * @param[in] aViewScn    저장할 SCN
 */
void smpSetMaxViewScn( smpHandle * aPageHandle,
                       smlScn      aViewScn )
{
    smpPhyHdr * sPhyHdr;

    sPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );
    sPhyHdr->mMaxViewScn = STL_MAX( aViewScn, sPhyHdr->mMaxViewScn );
}

/**
 * @brief MaxViewScn을 얻는다.
 * @param[in] aPageHandle Page Access Handle
 */
smlScn smpGetMaxViewScn( smpHandle * aPageHandle )
{
    smpPhyHdr * sPhyHdr;

    sPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );
    return sPhyHdr->mMaxViewScn;
}

/**
 * @brief Volatile Area에 aData를 기록한다.
 * @param[in] aPageHandle Page Access Handle
 * @param[in] aData 저장할 데이터
 * @param[in] aDataSize 저장할 데이터 길이
 */
void smpSetVolatileArea( smpHandle * aPageHandle,
                         void      * aData,
                         stlInt32    aDataSize )
{
    smpPhyHdr * sPhyHdr;
    
    sPhyHdr = (smpPhyHdr*)(aPageHandle->mFrame);
    stlMemcpy( sPhyHdr->mVolatileArea, aData, aDataSize );
}

/**
 * @brief Volatile Area를 초기화 한다.
 * @param[in] aPageHandle Page Access Handle
 */
void smpInitVolatileArea( smpHandle * aPageHandle )
{
    smpPhyHdr * sPhyHdr;
    
    sPhyHdr = (smpPhyHdr*)(aPageHandle->mFrame);
    stlMemset( sPhyHdr->mVolatileArea, 0x00, SMP_VOLATILE_AREA_SIZE );
}

/**
 * @brief Volatile Area를 얻는다.
 * @param[in] aPageHandle Page Access Handle
 * @return Volatile Area
 */
void * smpGetVolatileArea( smpHandle * aPageHandle )
{
    return ((smpPhyHdr*)(aPageHandle->mFrame))->mVolatileArea;
}

/** @} */

/**
 * @addtogroup smpManager
 * @{
 */

stlStatus smpInitPch( smlTbsId          aTbsId,
                      smlPid            aPageId,
                      stlUInt32         aDeviceType,
                      knlLogicalAddr    aFrame,
                      smpCtrlHeader   * aPch,
                      smlEnv          * aEnv )
{
    STL_TRY( knlInitDbLatch( &aPch->mPageLatch,
                             KNL_GET_SHM_INDEX( aFrame ),
                             (knlEnv*)aEnv ) == STL_SUCCESS );
                               
    aPch->mFrame         = aFrame;
    aPch->mFixedCount    = 0;
    aPch->mTbsId         = aTbsId;
    aPch->mPageId        = aPageId;
    aPch->mDirty         = STL_FALSE;
    aPch->mDeviceType    = aDeviceType;
    aPch->mFirstDirtyLsn = SMR_INVALID_LSN;
    aPch->mRecoveryLsn   = SMR_INVALID_LSN;
    aPch->mRecoverySbsn  = SMR_INVALID_SBSN;
    aPch->mRecoveryLpsn  = SMR_INVALID_LPSN;
    aPch->mIsCorrupted   = STL_FALSE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 페이지의 Maximum Lsn을 얻는다.
 * @param[in] aPageHandle Page Access Handle
 * @return Max value between the recovery Lsn and the page Lsn
 */
inline smrLsn smpGetPageMaxLsn( smpHandle     * aPageHandle )
{
    smrLsn    sLsn;

    STL_ASSERT( aPageHandle         != NULL );
    STL_ASSERT( aPageHandle->mPch   != NULL );
    STL_ASSERT( aPageHandle->mFrame != NULL );

    sLsn = aPageHandle->mPch->mRecoveryLsn;

    if( sLsn == SMR_INVALID_LSN )
    {
        sLsn = ((smpPhyHdr*)aPageHandle->mFrame)->mLsn;
    }

    return sLsn;
}

/**
 * @brief 주어진 Pch Array가 디스크로 Flush될 필요가 있는지 검사한다.
 * @param[in]     aPchArrayList Pch Array
 * @param[out]    aNeedFlush    Flush 여부
 * @param[in,out] aEnv          Environment 구조체
 * @remarks
 * <BR> Flush가 필요없는 경우
 * <BR> (1) DATA_STORE_MODE_DS에서의 메모리 테이블 스페이스
 * <BR> (2) DATA_STORE_MODE_CDS에서의 메모리 테이블 스페이스
 * <BR> (3) NO LOGGING 테이블 스페이스
 * <BR> (4) OFFLINE 테이블 스페이스
 */
stlStatus smpNeedFlushPchArray( smpPchArrayList * aPchArrayList,
                                stlBool         * aNeedFlush,
                                smlEnv          * aEnv )
{
    stlInt32   sTbsAttr;
    stlUInt32  sTbsType;

    *aNeedFlush = STL_FALSE;
    
    sTbsAttr = SMF_GET_TBS_ATTR( aPchArrayList->mTbsId );
    sTbsType = SML_TBS_DEVICE_TYPE( sTbsAttr );

    if( sTbsType == SML_TBS_DEVICE_MEMORY )
    {
        if( smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS )
        {
            *aNeedFlush = STL_TRUE;
        }
    }
    else
    {
        *aNeedFlush = STL_TRUE;
    }

    /**
     * do not flush the NOLOGGING tablespace
     */
    if( SMF_IS_LOGGING_TBS( aPchArrayList->mTbsId ) == STL_FALSE )
    {
        *aNeedFlush = STL_FALSE;
    }

    /**
     * do not flush the OFFLINE tablespace
     */
    if( SMF_IS_ONLINE_TBS( aPchArrayList->mTbsId ) == STL_FALSE )
    {
        *aNeedFlush = STL_FALSE;
    }

    return STL_SUCCESS;
}

/**
 * @brief Incremental Backup을 수행위해 Pages를 Backup.
 * @param[in]     aFile           Backup File Desc
 * @param[in]     aFileName       Backup File Name
 * @param[in]     aPrevBackupLsn  Previous Backup Lsn
 * @param[in]     aTbsId          Tablespace Id
 * @param[out]    aBackupLsn      Backup 직전 최종 checkpoint Lsn
 * @param[out]    aBackupLid      Backup 직전 최종 checkpoint Lid
 * @param[in,out] aEnv            Environment 정보
 */
stlStatus smpBackupPages( stlFile  * aFile,
                          stlChar  * aFileName,
                          smrLsn     aPrevBackupLsn,
                          smlTbsId   aTbsId,
                          smrLsn   * aBackupLsn,
                          smrLid   * aBackupLid,
                          smlEnv   * aEnv )
{
    smpPchArrayList        * sPchArrayList;
    smpCtrlHeader          * sPchArray;
    void                   * sFrame;
    stlInt64                 i;
    stlInt32                 sIdx   = 0;
    stlInt32                 sState = 0;
    stlChar                * sBodyBuffer;
    stlChar                * sTailBuffer;
    stlChar                * sPageFrames;
    stlBool                  sIsTimedOut;
    stlOffset                sFileOffset;
    smfIncBackupFileHeader   sFileHeader;
    smfIncBackupFileTail   * sFileTail;
    stlInt64                 sMaxFlushPageCount;
    stlUInt64                sBackupPageCount    = 0;
    stlUInt64                sTbsBackupPageCount = 0;
    stlInt32                 sTbsCount           = 0;
    stlInt16                 sCurTbsCount;
    stlSize                  sWrittenBytes;
    stlSize                  sTailSize;
    smrLsn                   sMaxLsn;
    smrLsn                   sMaxBackupLsn = SMR_INVALID_LSN;
    smrSbsn                  sMaxBackupSbsn = SMR_INVALID_SBSN;
    knlRegionMark            sMemMark;
    smlTbsId                 sTbsId = SML_INVALID_TBS_ID;
    smlDatafileId            sDatafileId = SML_INVALID_DATAFILE_ID;
    stlBool                  sTbsExist = STL_FALSE;
    stlBool                  sDatafileExist = STL_FALSE;
    stlInt64                 sChecksumType;
    smpPhyHdr              * sPhyHdr = NULL;
    
    sMaxFlushPageCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MAXIMUM_FLUSH_PAGE_COUNT,
                                                        KNL_ENV( aEnv ) );

    sChecksumType = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PAGE_CHECKSUM_TYPE,
                                                   KNL_ENV( aEnv ) );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                (sMaxFlushPageCount + 1) * SMP_PAGE_SIZE,
                                (void**)&sBodyBuffer,
                                (knlEnv*)aEnv )
             == STL_SUCCESS );

    sPageFrames = (stlChar *)STL_ALIGN( (stlUInt64)sBodyBuffer, SMP_PAGE_SIZE );

    /**
     * Backup File Tail
     */
    sCurTbsCount = smfGetTbsCount();
    sTailSize = STL_ALIGN( sCurTbsCount * STL_SIZEOF( smfIncBackupFileTail ), SMP_PAGE_SIZE );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sTailSize + SMP_PAGE_SIZE,
                                (void**)&sTailBuffer,
                                (knlEnv*)aEnv )
             == STL_SUCCESS );

    sFileTail = (smfIncBackupFileTail *)STL_ALIGN( (stlUInt64)sTailBuffer, SMP_PAGE_SIZE );
    stlMemset( (void *)sFileTail, 0x00, sTailSize );

    stlMemset( (void *)&sFileHeader, 0x00, STL_SIZEOF(smfIncBackupFileHeader) );

    smrGetConsistentChkptLsnAndLid( &sFileHeader.mBackupChkptLsn,
                                    &sFileHeader.mBackupChkptLid );

    sMaxLsn = sFileHeader.mBackupChkptLsn;
        
    /**
     * Backup File Header를 초기화하여 기록한다.
     */
    sFileHeader.mTbsId    = aTbsId;
    sFileHeader.mTbsCount = 0;
    sFileHeader.mBodySize = 0;
    sFileHeader.mPrevBackupLsn = aPrevBackupLsn;
    sFileHeader.mMaxBackupLsn = sMaxLsn;

    STL_TRY( smfWriteBackupFileHeader( aFile,
                                       &sFileHeader,
                                       &sWrittenBytes,
                                       aEnv ) == STL_SUCCESS );

    sFileOffset = sWrittenBytes;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[INCREMENTAL BACKUP] started - '%s'\n",
                        aFileName )
             == STL_SUCCESS );

    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );
    
    while( sTbsExist == STL_TRUE )
    {
        /**
         * Tablespace Backup인 경우 해당 tablespace만 backup 수행
         */ 
        if( (aTbsId != SML_INVALID_TBS_ID) && (aTbsId != sTbsId) )
        {
            STL_TRY( smfNextTbs( &sTbsId,
                                 &sTbsExist,
                                 aEnv ) == STL_SUCCESS );
            continue;
        }

        /**
         * Incremental Backup이 begin된 tablespace만 backup 수행
         */
        if( (SMF_TBS_BACKUP_STATE(sTbsId) & SMF_BACKUP_STATE_INCREMENTAL_BACKUP)
            != SMF_BACKUP_STATE_INCREMENTAL_BACKUP )
        {
            STL_TRY( smfNextTbs( &sTbsId,
                                 &sTbsExist,
                                 aEnv ) == STL_SUCCESS );
            continue;
        }
        
        STL_TRY( smfFirstDatafile( sTbsId,
                                   &sDatafileId,
                                   &sDatafileExist,
                                   aEnv ) == STL_SUCCESS );
        
        while( sDatafileExist == STL_TRUE )
        {
            /**
             * Datafile의 상태를 체크한다.
             */
            if( smfGetDatafileState(sTbsId, sDatafileId) != SMF_DATAFILE_STATE_CREATED )
            {
                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sDatafileExist,
                                          aEnv ) == STL_SUCCESS );
                continue;
            }

            smpGetPchArray( sTbsId,
                            sDatafileId,
                            &sPchArrayList );

            /**
             * Tbs의 상태가 offline이어도 Backup을 진행하지만,
             * offline Tbs의 pch가 NULL일 수도 있다.(Mount phase에서 Offline)
             */
            if( SMF_IS_ONLINE_TBS( sTbsId ) == STL_TRUE )
            {
                STL_TRY_THROW( sPchArrayList != NULL, RAMP_ERR_INTERNAL );
            }
            else
            {
                if( sPchArrayList == NULL )
                {
                    STL_TRY( smfNextDatafile( sTbsId,
                                              &sDatafileId,
                                              &sDatafileExist,
                                              aEnv ) == STL_SUCCESS );
                    continue;
                }
            }

            STL_TRY( knlGetPhysicalAddr( sPchArrayList->mPchArray,
                                         (void**)&sPchArray,
                                         (knlEnv*) aEnv) == STL_SUCCESS );
        
            for( sIdx = 0; sIdx < sPchArrayList->mPchCount; sIdx++ )
            {
                /* PchArray의 RecoveryLsn 조건 부합한 Page만 latch 잡고 backup */
                if( sPchArray[sIdx].mRecoveryLsn > aPrevBackupLsn )
                {
                    STL_TRY( knlAcquireLatch( &sPchArray[sIdx].mPageLatch,
                                              KNL_LATCH_MODE_SHARED,
                                              KNL_LATCH_PRIORITY_NORMAL,
                                              STL_INFINITE_TIME,
                                              &sIsTimedOut,
                                              (knlEnv*) aEnv ) == STL_SUCCESS );
                    sState = 2;
                    
                    if( sPchArray[sIdx].mRecoveryLsn > aPrevBackupLsn )
                    {
                        STL_TRY( knlGetPhysicalAddr( sPchArray[sIdx].mFrame,
                                                     (void**) &sFrame,
                                                     (knlEnv*) aEnv ) == STL_SUCCESS );
                        
                        sMaxLsn = STL_MAX( sMaxLsn, sPchArray[sIdx].mRecoveryLsn );
                        
                        sMaxBackupLsn = STL_MAX( sMaxBackupLsn, sPchArray[sIdx].mRecoveryLsn );
                        sMaxBackupSbsn = STL_MAX( sMaxBackupSbsn, sPchArray[sIdx].mRecoverySbsn );
                        
                        stlMemcpy( &sPageFrames[ sBackupPageCount * SMP_PAGE_SIZE ],
                                   sFrame,
                                   SMP_PAGE_SIZE );

                        sPhyHdr =  (smpPhyHdr*)(sPageFrames + (sBackupPageCount * SMP_PAGE_SIZE));
                        sPhyHdr->mLsn =sPchArray[sIdx].mRecoveryLsn;
                            
                        sBackupPageCount++;
                        sTbsBackupPageCount++;
                    }
                    
                    sState = 1;
                    STL_TRY( knlReleaseLatch( &sPchArray[sIdx].mPageLatch,
                                              (knlEnv*) aEnv ) == STL_SUCCESS );
                }

                if( sBackupPageCount >= sMaxFlushPageCount )
                {
                    STL_TRY( smrFlushLog( sMaxBackupLsn,
                                          sMaxBackupSbsn,
                                          STL_TRUE, /* aSwitchBlock */
                                          SML_TRANSACTION_CWM_WAIT,
                                          aEnv ) == STL_SUCCESS );

                    for( i = 0; i < sBackupPageCount; i++ )
                    {
                        STL_TRY( smpMakePageChecksum( sPageFrames + (SMP_PAGE_SIZE * i),
                                                      sChecksumType,
                                                      aEnv )
                                 == STL_SUCCESS );
                    }

                    STL_TRY( stlWriteFile( aFile,
                                           sPageFrames,
                                           sBackupPageCount * SMP_PAGE_SIZE,
                                           NULL,
                                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

                    sBackupPageCount = 0;
                    sMaxBackupLsn = SMR_INVALID_LSN;
                    sMaxBackupSbsn = SMR_INVALID_SBSN;
                }
            }

            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                ".... BACKUP (TABLESPACE(%d), DATAFILE(%d)) done\n",
                                sPchArrayList->mTbsId,
                                sPchArrayList->mDatafileId )
                     == STL_SUCCESS );

            STL_TRY( smfNextDatafile( sTbsId,
                                      &sDatafileId,
                                      &sDatafileExist,
                                      aEnv ) == STL_SUCCESS );
            /**
             * Datafile 단위로 session check
             */
            STL_TRY( knlCheckEndSession( KNL_SESS_ENV(aEnv),
                                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }

        if( sTbsBackupPageCount != 0 )
        {
            sFileTail[sTbsCount].mTbsId = sTbsId;
            sFileTail[sTbsCount].mTbsOffset = sFileOffset;
            sFileOffset += (sTbsBackupPageCount * SMP_PAGE_SIZE);
            sFileTail[sTbsCount].mBackupPageCount = sTbsBackupPageCount;
            sFileHeader.mBodySize += sTbsBackupPageCount * SMP_PAGE_SIZE;
            
            sTbsBackupPageCount = 0;
            sTbsCount++;
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }
 
    if( sBackupPageCount > 0 )
    {
        STL_TRY( smrFlushLog( sMaxBackupLsn,
                              sMaxBackupSbsn,
                              STL_TRUE, /* aSwitchBlock */
                              SML_TRANSACTION_CWM_WAIT,
                              aEnv ) == STL_SUCCESS );

        for( i = 0; i < sBackupPageCount; i++ )
        {
            STL_TRY( smpMakePageChecksum( sPageFrames + (SMP_PAGE_SIZE * i),
                                          sChecksumType,
                                          aEnv )
                     == STL_SUCCESS );
        }

        STL_TRY( stlWriteFile( aFile,
                               sPageFrames,
                               sBackupPageCount * SMP_PAGE_SIZE,
                               NULL,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    }
    
    /**
     * Tail 기록.
     */
    if( sTbsCount != 0 )
    {
        STL_TRY( stlWriteFile( aFile,
                               sFileTail,
                               sTailSize,
                               NULL,
                               KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        
    }
    
    for( i = 0; i < sCurTbsCount; i++ )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV(aEnv),
                            KNL_LOG_LEVEL_INFO,
                            ".... BACKUP TAIL INFO: TABLESPACE(%d), OFFSET(%ld), PAGE COUNT(%ld)\n",
                            sFileTail[i].mTbsId,
                            sFileTail[i].mTbsOffset,
                            sFileTail[i].mBackupPageCount )
                 == STL_SUCCESS );
    }
    
    /**
     * Backup File Header를 기록한다.
     */
    sFileHeader.mTbsCount = sTbsCount;
    sFileHeader.mMaxBackupLsn = sMaxLsn;

    STL_TRY( smfWriteBackupFileHeader( aFile,
                                       &sFileHeader,
                                       &sWrittenBytes,
                                       aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    *aBackupLsn = sFileHeader.mBackupChkptLsn;
    *aBackupLid = sFileHeader.mBackupChkptLid;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[INCREMENTAL BACKUP] finished - '%s'\n",
                        aFileName )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- smpBackupPages()",
                      KNL_ERROR_STACK(aEnv) );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }
    
    STL_FINISH;
    
    switch( sState )
    {
        case 2:
            (void) knlReleaseLatch( &sPchArray[sIdx].mPageLatch, KNL_ENV(aEnv) );
        case 1:
            (void) knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                             &sMemMark,
                                             STL_TRUE, /* aFreeChunk */
                                             KNL_ENV(aEnv) );
    }
    
    return STL_FAILURE;
}

void smpGetPchArray( smlTbsId            aTbsId,
                     smlDatafileId       aDatafileId,
                     smpPchArrayList  ** aPchArray )
{
    smpPchArrayList   * sPchArrayList;

    sPchArrayList = gSmpWarmupEntry->mPchArrayList;

    while( sPchArrayList != NULL )
    {
        if( (aTbsId == sPchArrayList->mTbsId) &&
            (aDatafileId == sPchArrayList->mDatafileId) )
        {
            break;
        }

        sPchArrayList = sPchArrayList->mNext;
    }

    *aPchArray = sPchArrayList;
}

stlStatus smpApplyBackup( stlChar        * aBackupFileName,
                          stlChar        * aPageFrame,
                          smlDatafileId  * aDatafileId,
                          void          ** aPchArrayList,
                          smlEnv         * aEnv )
{
    smpPchArrayList   * sPchArrayList;
    smpCtrlHeader     * sPchArray;
    void              * sFrame;
    smpPid              sPid;
    stlBool             sIsValid;

    STL_TRY( smpValidateChecksum( aPageFrame,
                                  &sIsValid,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_ERR_CORRUPTED_BACKUP );

    sPid = ((smpPhyHdr *)aPageFrame)->mSelfId;

    if( (*aPchArrayList == NULL) ||
        (SMP_DATAFILE_ID(sPid.mPageId) != *aDatafileId) )
    {
        smpGetPchArray( sPid.mTbsId,
                        SMP_DATAFILE_ID(sPid.mPageId),
                        &sPchArrayList );

        /**
         * Backup file에는 존재하지만 drop된 datafile의 경우 apply skip
         */
        STL_TRY_THROW( sPchArrayList != NULL, SKIP_APPLY_PAGE );

        *aPchArrayList = sPchArrayList;
        *aDatafileId   = SMP_DATAFILE_ID(sPid.mPageId);
    }
    else
    {
        sPchArrayList = *aPchArrayList;
    }

    STL_TRY( knlGetPhysicalAddr( sPchArrayList->mPchArray,
                                 (void**)&sPchArray,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    if( sPchArray[SMP_PAGE_SEQ_ID(sPid.mPageId)].mRecoveryLsn < ((smpPhyHdr *)aPageFrame)->mLsn )
    {
        STL_TRY( knlGetPhysicalAddr( sPchArray[SMP_PAGE_SEQ_ID(sPid.mPageId)].mFrame,
                                     (void**)&sFrame,
                                     KNL_ENV( aEnv ) ) == STL_SUCCESS );

/*
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "    [RESTORE] merge backup page ( %d, %d, %d : %ld, %ld, %ld, %d )\n",
                            sPid.mTbsId,
                            SMP_DATAFILE_ID(sPid.mPageId),
                            SMP_PAGE_SEQ_ID(sPid.mPageId),
                            sPchArray[SMP_PAGE_SEQ_ID(sPid.mPageId)].mRecoveryLsn,
                            sPchArray[SMP_PAGE_SEQ_ID(sPid.mPageId)].mFirstDirtyLsn,
                            ((smpPhyHdr *)aPageFrame)->mLsn,
                            ((smpPhyHdr *)aPageFrame)->mPageType )
                 == STL_SUCCESS );
*/

        STL_DASSERT( sFrame != NULL );

        /**
         * Media recovery는 MOUNT phase에서 수행되거나
         * OPEN phase에서 Offline Tablespace에 대해서만 수행되므로
         * Pch array에 backup을 반영하기 위해 page latch를 잡을 필요는 없다.
         */
        stlMemcpy( sFrame, aPageFrame, SMP_PAGE_SIZE );
        sPchArray[SMP_PAGE_SEQ_ID(sPid.mPageId)].mRecoveryLsn = ((smpPhyHdr *)aPageFrame)->mLsn;
        sPchArray[SMP_PAGE_SEQ_ID(sPid.mPageId)].mFirstDirtyLsn = ((smpPhyHdr *)aPageFrame)->mLsn;
        sPchArray[SMP_PAGE_SEQ_ID(sPid.mPageId)].mPageType = ((smpPhyHdr *)aPageFrame)->mPageType;
        sPchArray[SMP_PAGE_SEQ_ID(sPid.mPageId)].mDirty = STL_TRUE;
        sPchArray[SMP_PAGE_SEQ_ID(sPid.mPageId)].mRecoveryLpsn = SMR_MAX_LPSN;

        /* volatile area를 초기화한다. */
        stlMemset( ((smpPhyHdr *)sFrame)->mVolatileArea,
                   0x00,
                   SMP_VOLATILE_AREA_SIZE );
    }

    STL_RAMP( SKIP_APPLY_PAGE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CORRUPTED_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_BACKUP_IS_CORRUPTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      aBackupFileName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief page_checksum property가 true이면 PhyTail에 checksum 저장.
 * @param [in,out] aPageFrame     physical page body
 * @param [in]     aChecksumType  checksum type
 * @param [in,out] aEnv           environment
 */ 
stlStatus smpMakePageChecksum( void      * aPageFrame,
                               stlInt64    aChecksumType,
                               smlEnv    * aEnv )
{
    smpPhyHdr   * sHdr;
    smpPhyTail  * sTail;
    stlUInt32     sChecksum = STL_CRC32_INIT_VALUE;
    stlInt64      sChecksumType;

    sHdr  = (smpPhyHdr *) aPageFrame;
    sTail = SMP_GET_PHYSICAL_TAIL( aPageFrame );
    sChecksumType = aChecksumType << SMP_PAGE_CHECKSUM_TYPE_PRECISION;
    
    switch( sChecksumType )
    {
        case SMP_PAGE_CHECKSUM_TYPE_CRC :

            STL_TRY( stlCrc32( aPageFrame,
                               SMP_PAGE_SIZE - STL_SIZEOF( smpPhyTail ),
                               &sChecksum,
                               KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            sTail->mChecksum = ((stlInt64)sChecksum) & SMP_PAGE_CHECKSUM_CRC_MASK;
            sTail->mChecksum |= SMP_PAGE_CHECKSUM_TYPE_CRC;
            break;
        case SMP_PAGE_CHECKSUM_TYPE_LSN :
            sTail->mChecksum = sHdr->mLsn & SMP_PAGE_CHECKSUM_LSN_MASK;
            sTail->mChecksum |= SMP_PAGE_CHECKSUM_TYPE_LSN;
            break;
        default: 
            STL_DASSERT( 0 );
            break;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page의 Checksum을 validation 검사.
 * @param [in]     aPageFrame  page frame
 * @param [out]    aIsValid    validation resul
 * @param [in,out] aEnv        environment
 */ 
stlStatus smpValidateChecksum( void      * aPageFrame,
                               stlBool   * aIsValid,
                               smlEnv    * aEnv )
{
    smpPhyHdr   * sHdr;
    smpPhyTail  * sTail;
    stlInt64      sChecksumType;
    stlUInt32     sChecksum = STL_CRC32_INIT_VALUE;
    stlUInt32     sTailChecksum;
    stlBool       sIsSame = STL_TRUE;
    smrLsn        sTailLsn = 0;
    smrLsn        sHdrLsn  = 0;
    
    sHdr  = (smpPhyHdr *) aPageFrame;
    sTail = SMP_GET_PHYSICAL_TAIL( aPageFrame );

    sChecksumType = sTail->mChecksum & SMP_PAGE_CHECKSUM_TYPE_MASK;
    
    switch( sChecksumType )
    {
        case SMP_PAGE_CHECKSUM_TYPE_CRC:
            STL_TRY( stlCrc32( aPageFrame,
                               SMP_PAGE_SIZE - STL_SIZEOF( smpPhyTail ),
                               &sChecksum,
                               KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            sTailChecksum = (stlUInt32)(sTail->mChecksum & SMP_PAGE_CHECKSUM_CRC_MASK );

            sIsSame = ( sTailChecksum == sChecksum ) ? STL_TRUE : STL_FALSE;
            break;
        case SMP_PAGE_CHECKSUM_TYPE_LSN:
            sTailLsn = sTail->mChecksum & SMP_PAGE_CHECKSUM_LSN_MASK;
            sHdrLsn  = sHdr->mLsn  & SMP_PAGE_CHECKSUM_LSN_MASK;
            
            sIsSame = ( sTailLsn == sHdrLsn ) ? STL_TRUE : STL_FALSE;
            break;
        default:
            sIsSame = STL_FALSE;
            break;
    }

    *aIsValid = sIsSame;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Full Backup File을 이용하여 Page를 Restore
 * @param[in]     aFile          backup file pointer
 * @param[in]     aDatafileInfo  backup target인 datafile info
 * @param[in]     aRestoredLsn   restored lsn, pch에 저장된다.
 * @param[in,out] aEnv           environment
 */ 
stlStatus smpRestoreDatafileByFullBackup( stlFile                 * aFile,
                                          smrRecoveryDatafileInfo * aDatafileInfo,
                                          smrLsn                    aRestoredLsn,
                                          smlEnv                  * aEnv )
{
    smfTbsInfo      * sTbsInfo;
    smfDatafileInfo * sDatafileInfo;
    smpPchArrayList * sPchArrayList;
    smpCtrlHeader   * sPchArray;
    smpPhyHdr       * sPhyHdr;
    stlChar         * sChunk;
    stlOffset         sOffset;
    stlUInt64         sIdx;
    stlUInt64         sTotalPageCount;
    stlUInt64         sReadPageCount = 0;
    stlUInt64         sPageCount;
    stlUInt64         sSeqId = 0;
    stlBool           sIsValid = STL_FALSE;
    stlUInt64         sBulkIoPageCount;

    sTotalPageCount = smfGetMaxPageCount( aDatafileInfo->mTbsId,
                                          aDatafileInfo->mDatafileId );

    /**
     * get smfDatafileInfo.
     */
    sTbsInfo      = smfGetTbsHandle( aDatafileInfo->mTbsId );
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileInfo->mDatafileId];
            
    /**
     * Datafile이 LOAD된 물리적 위치를 얻는다.
     */
    STL_TRY( knlGetPhysicalAddr( sDatafileInfo->mChunk,
                                 (void**)&sChunk,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * 물리적 주소에 직접 Copy 후에 PchArray에 RecoveryLsn을 설정한다. 
     */
    smpGetPchArray( aDatafileInfo->mTbsId,
                    aDatafileInfo->mDatafileId,
                    &sPchArrayList );

    STL_TRY( knlGetPhysicalAddr( sPchArrayList->mPchArray,
                                 (void**)&sPchArray,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );
        
    /**
     * First Page ID의 위치를 찾는다.
     * Datafile Header Page를 위해 + SMP_PAGE_SIZE.
     */
    sOffset = SMP_PAGE_SIZE;
    
    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    sBulkIoPageCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_BULK_IO_PAGE_COUNT,
                                                      KNL_ENV(aEnv) );

    while( sReadPageCount < sTotalPageCount )
    {
        sPageCount = STL_MIN( (sTotalPageCount - sReadPageCount),
                              sBulkIoPageCount );

        /**
         * 물리적 주소에 직접 ReadFile을 수행.
         */
        STL_TRY( stlReadFile( aFile,
                              sChunk + (sReadPageCount * SMP_PAGE_SIZE),
                              sPageCount * SMP_PAGE_SIZE,
                              NULL,
                              KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        for( sIdx = 0; sIdx < sPageCount; sIdx++, sSeqId++ )
        {
            STL_TRY( smpValidateChecksum( sChunk + (sSeqId * SMP_PAGE_SIZE),
                                          &sIsValid,
                                          aEnv )
                     == STL_SUCCESS );

            if( sIsValid == STL_TRUE )
            {
                if( sPchArray[sSeqId].mIsCorrupted == STL_TRUE )
                {
                    sDatafileInfo->mCorruptionCount--;
                    sPchArray[sSeqId].mIsCorrupted = STL_FALSE;
                }
            }
            else
            {
                sPchArray[sSeqId].mIsCorrupted = STL_TRUE;
                STL_THROW( RAMP_ERR_CORRUPTED_BACKUP);
            }

            sPhyHdr = (smpPhyHdr *)( sChunk + ((stlInt64)sSeqId * SMP_PAGE_SIZE) );
            sPhyHdr->mTimestamp = smfGetTimestamp( aDatafileInfo->mTbsId,
                                                   aDatafileInfo->mDatafileId );
            sPchArray[sSeqId].mRecoveryLsn = sPhyHdr->mLsn;
            sPchArray[sSeqId].mPageType    = sPhyHdr->mPageType;
            sPchArray[sSeqId].mDirty = STL_TRUE;
        }

        sReadPageCount += sPageCount;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CORRUPTED_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_BACKUP_IS_CORRUPTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      aDatafileInfo->mBackupName );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Full Backup File을 이용하여 Page를 Restore, Corrupted Page만 대상으로 한다.
 * @param[in]     aFile          backup file pointer
 * @param[in]     aDatafileInfo  backup target인 datafile info
 * @param[in]     aRestoredLsn   restored lsn, pch에 저장된다.
 * @param[in,out] aEnv           environment
 */ 
stlStatus smpRestoreDatafileByFullBackup4Corruption( stlFile                 * aFile,
                                                     smrRecoveryDatafileInfo * aDatafileInfo,
                                                     smrLsn                    aRestoredLsn,
                                                     smlEnv                  * aEnv )
{
    stlInt32                i;
    smpPchArrayList       * sPchArrayList;
    smpCtrlHeader         * sPchArray;
    smpPhyHdr             * sPhyHdr;
    stlChar                 sBuffer[SMP_PAGE_SIZE * 2];
    stlChar               * sPageFrame;
    void                  * sFrame;
    stlOffset               sOffset;
    stlSize                 sReadSize;
    stlBool                 sIsValid = STL_FALSE;
    smlPid                  sSeqPid;
    smrCorruptedPageInfo  * sPageList;
    smfTbsInfo            * sTbsInfo;
    smfDatafileInfo       * sDatafileInfo;
    
    sPageFrame = (stlChar *)STL_ALIGN( (stlInt64)sBuffer, SMP_PAGE_SIZE );

    /**
     * get smfDatafileInfo.
     */
    sTbsInfo      = smfGetTbsHandle( aDatafileInfo->mTbsId );
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileInfo->mDatafileId];
    
    smpGetPchArray( aDatafileInfo->mTbsId,
                    aDatafileInfo->mDatafileId,
                    &sPchArrayList );

    STL_TRY( knlGetPhysicalAddr( sPchArrayList->mPchArray,
                                 (void**)&sPchArray,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_RING_FOREACH_ENTRY( &aDatafileInfo->mCorruptedPageList, sPageList )
    {
        for( i = 0; i < sPageList->mPageCount; i++ )
        {
            sSeqPid = sPageList->mPages[i];
            sOffset = (SMP_PAGE_SIZE * (stlInt64)sSeqPid) + SMP_PAGE_SIZE;

            STL_TRY( stlSeekFile( aFile,
                                  STL_FSEEK_SET,
                                  &sOffset,
                                  KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            
            STL_TRY( stlReadFile( aFile,
                                  sPageFrame,
                                  SMP_PAGE_SIZE,
                                  &sReadSize,
                                  KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            STL_DASSERT( sReadSize == SMP_PAGE_SIZE );

            STL_TRY( knlGetPhysicalAddr( sPchArray[sSeqPid].mFrame,
                                         (void**)&sFrame,
                                         KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            STL_TRY( smpValidateChecksum( sPageFrame,
                                          &sIsValid,
                                          aEnv )
                     == STL_SUCCESS );

            if( sIsValid == STL_TRUE )
            {
                if( sPchArray[sSeqPid].mIsCorrupted == STL_TRUE )
                {
                    sDatafileInfo->mCorruptionCount--;
                    sPchArray[sSeqPid].mIsCorrupted = STL_FALSE;
                }
            }
            else
            {
                sPchArray[sSeqPid].mIsCorrupted = STL_TRUE;
                STL_THROW( RAMP_ERR_CORRUPTED_BACKUP);
            }
            
            stlMemcpy( sFrame, sPageFrame, SMP_PAGE_SIZE );

            sPhyHdr = (smpPhyHdr *)sFrame;
            sPhyHdr->mTimestamp = smfGetTimestamp( aDatafileInfo->mTbsId,
                                                   aDatafileInfo->mDatafileId );
            
            sPchArray[sSeqPid].mRecoveryLsn = sPhyHdr->mLsn;
            sPchArray[sSeqPid].mPageType    = sPhyHdr->mPageType;
            sPchArray[sSeqPid].mDirty       = STL_TRUE;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CORRUPTED_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_BACKUP_IS_CORRUPTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      aDatafileInfo->mBackupName );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief 하나의 Incremental backup file에서 특정 Tablespace에 속한 Target Page를 Restore한다.
 * @param [in]     aFile           Incremental backup file pointer
 * @param [in]     aDatafileInfo   Datafile, Page Information
 * @param [in]     aBackupIterator Incrermental Backup File 정보
 * @param [in]     aFileTail       Incremental Backup File Tail
 * @param [in]     aBuffer         Backup File Read위한 Buffer     
 * @param [in]     aBufferSize     Buffer Size
 * @param [in,out] aEnv            Envrionment
 */ 
stlStatus smpRestoreDatafileByIncBackup( stlFile                 * aFile,
                                         smrRecoveryDatafileInfo * aDatafileInfo,
                                         smfBackupIterator       * aBackupIterator,
                                         smfIncBackupFileTail    * aFileTail,
                                         stlChar                 * aBuffer,
                                         stlInt32                  aBufferSize,
                                         smlEnv                  * aEnv )
{
    smlTbsId              sTbsId;
    smlDatafileId         sDatafileId;
    smlPid                sPageId;
    stlOffset             sTbsOffset;
    stlSize               sSize;
    stlInt64              sTbsBackupSize;
    stlInt64              sDoneSize;
    stlInt64              sReadSize;
    stlInt64              sPageCount;
    stlInt64              sIdxBuffer;
    smpPchArrayList     * sPchArrayList;
    smpCtrlHeader       * sPchArray;
    smpPhyHdr           * sPhyHdr;
    stlInt32              sPchMaxCount;
    void                * sFrame;
    smfTbsInfo          * sTbsInfo;
    smfDatafileInfo     * sDatafileInfo;
    stlBool               sIsValid;

    sTbsId         = aDatafileInfo->mTbsId;
    sDatafileId    = aDatafileInfo->mDatafileId;
    sPageId        = SMP_MAKE_PID( sDatafileId, 0 );    

    /**
     * get smfDatafileInfo.
     */
    sTbsInfo      = smfGetTbsHandle( aDatafileInfo->mTbsId );
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileInfo->mDatafileId];
    
    smpGetPchArray( sTbsId,
                    sDatafileId,
                    &sPchArrayList );

    sPchMaxCount = smfGetMaxPageCount( sTbsId,
                                       sDatafileId );
    
    STL_TRY( knlGetPhysicalAddr( sPchArrayList->mPchArray,
                                 (void**)&sPchArray,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY_THROW( aFileTail != NULL, RAMP_SKIP );

    sTbsOffset     = aFileTail->mTbsOffset;
    sTbsBackupSize = aFileTail->mBackupPageCount * SMP_PAGE_SIZE;
    
    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sTbsOffset,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    sDoneSize = 0;
    while( STL_TRUE )
    {
        if( sDoneSize >= sTbsBackupSize )
        {
            break;
        }

        sReadSize = STL_MIN( aBufferSize, (sTbsBackupSize - sDoneSize) );

        STL_TRY( stlReadFile( aFile,
                              aBuffer,
                              sReadSize,
                              &sSize,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        STL_DASSERT( sReadSize == sSize );

        sPageCount = sReadSize / SMP_PAGE_SIZE;
        sIdxBuffer = 0;
        while( STL_TRUE )
        {
            if( sIdxBuffer >= sPageCount )
            {
                break;
            }

            if( SMP_PAGE_SEQ_ID( sPageId ) >= sPchMaxCount )
            {
                break;
            }

            sPhyHdr = (smpPhyHdr *)( aBuffer + (sIdxBuffer * SMP_PAGE_SIZE) );
 
            if( sPhyHdr->mSelfId.mPageId == sPageId )
            {
                STL_TRY( knlGetPhysicalAddr( sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mFrame,
                                             (void**)&sFrame,
                                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

                STL_DASSERT( sFrame != NULL );

                /**
                 * Incremental Backup Page의 Validation.
                 */ 
                STL_TRY( smpValidateChecksum( &aBuffer[sIdxBuffer * SMP_PAGE_SIZE],
                                              &sIsValid,
                                              aEnv )
                         == STL_SUCCESS );

                if( sIsValid == STL_TRUE )
                {
                    if( sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mIsCorrupted == STL_TRUE )
                    {
                        sDatafileInfo->mCorruptionCount--;
                        sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mIsCorrupted = STL_FALSE;
                    }
                }
                else
                {
                    sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mIsCorrupted = STL_TRUE;
                    STL_THROW( RAMP_ERR_CORRUPTED_BACKUP);
                }

                stlMemcpy( sFrame, &aBuffer[sIdxBuffer * SMP_PAGE_SIZE], SMP_PAGE_SIZE );

                sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mRecoveryLsn = sPhyHdr->mLsn;
                sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mPageType    = sPhyHdr->mPageType;
                sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mDirty       = STL_TRUE;

                sPageId++;
                sIdxBuffer++;
            }
            else if( sPhyHdr->mSelfId.mPageId > sPageId )
            {
                /**
                 * IncBackup에 없는 Page의 초기화 작업
                 */ 
                if( aBackupIterator->mBackupRecord.mLevel == 0 )
                {
                    STL_TRY( knlGetPhysicalAddr( sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mFrame,
                                                 (void**)&sFrame,
                                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );

                    STL_DASSERT( sFrame != NULL );

                    if( sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mIsCorrupted == STL_TRUE )
                    {
                        sDatafileInfo->mCorruptionCount--;
                        sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mIsCorrupted = STL_FALSE;
                    }
                    
                    stlMemset( sFrame, 0x00, SMP_PAGE_SIZE );
                    
                    ((smpPhyHdr*)sFrame)->mTimestamp = smfGetTimestamp( sTbsId,
                                                                        sDatafileId );
                    
                    sPchArray[SMP_PAGE_SEQ_ID(sPageId)].mRecoveryLsn = 0;
                    sPchArray[SMP_PAGE_SEQ_ID(sPageId)].mPageType    = SMP_PAGE_TYPE_INIT;
                    sPchArray[SMP_PAGE_SEQ_ID(sPageId)].mDirty       = STL_TRUE;
                }
                
                sPageId++;
            }
            else
            {
                sIdxBuffer++;
            }
        }

        sDoneSize += sReadSize;
    }

    STL_RAMP( RAMP_SKIP );
    
    /**
     * Backup Level이 0인 경우 남은 Page 초기화해야 한다.
     */ 
    if( aBackupIterator->mBackupRecord.mLevel == 0 )
    {
        while( SMP_PAGE_SEQ_ID( sPageId ) < sPchMaxCount )
        {
            STL_TRY( knlGetPhysicalAddr( sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mFrame,
                                         (void**)&sFrame,
                                         KNL_ENV( aEnv ) ) == STL_SUCCESS );

            STL_DASSERT( sFrame != NULL );

            if( sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mIsCorrupted == STL_TRUE )
            {
                sDatafileInfo->mCorruptionCount--;
                sPchArray[SMP_PAGE_SEQ_ID( sPageId )].mIsCorrupted = STL_FALSE;

                stlMemset( sFrame, 0x00, SMP_PAGE_SIZE );
            
                ((smpPhyHdr*)sFrame)->mTimestamp = smfGetTimestamp( sTbsId,
                                                                    sDatafileId );

                sPchArray[SMP_PAGE_SEQ_ID(sPageId)].mRecoveryLsn = 0;
                sPchArray[SMP_PAGE_SEQ_ID(sPageId)].mPageType    = SMP_PAGE_TYPE_INIT;
                sPchArray[SMP_PAGE_SEQ_ID(sPageId)].mDirty       = STL_TRUE;
            }
            
            sPageId++;
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CORRUPTED_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_BACKUP_IS_CORRUPTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      aBackupIterator->mFileName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 하나의 Incremental backup file에서 특정 Tablespace에 속한 Target Page를 Restore한다.
 * <BR> Corrupted Page 대상으로 진행.
 * @param [in]     aFile           Incremental backup file pointer
 * @param [in]     aDatafileInfo   Datafile, Page Information
 * @param [in]     aBackupIterator Incrermental Backup File 정보
 * @param [in]     aFileTail       Incremental Backup File Tail
 * @param [in]     aBuffer         Backup File Read위한 Buffer     
 * @param [in]     aBufferSize     Buffer Size
 * @param [in,out] aEnv            Envrionment
 */ 
stlStatus smpRestoreDatafileByIncBackup4Corruption( stlFile                 * aFile,
                                                    smrRecoveryDatafileInfo * aDatafileInfo,
                                                    smfBackupIterator       * aBackupIterator,
                                                    smfIncBackupFileTail    * aFileTail,
                                                    stlChar                 * aBuffer,
                                                    stlInt32                  aBufferSize,
                                                    smlEnv                  * aEnv )
{
    smlTbsId                sTbsId;
    smlDatafileId           sDatafileId;
    smlPid                  sPageId;
    smlPid                  sSeqPid;
    stlOffset               sTbsOffset;
    stlSize                 sSize;
    stlInt64                sTbsBackupSize;
    stlInt64                sDoneSize;
    stlInt64                sReadSize;
    stlInt64                sPageCount;
    stlInt64                sIdxBuffer;
    smpPchArrayList       * sPchArrayList;
    smpCtrlHeader         * sPchArray;
    smpPhyHdr             * sPhyHdr;
    void                  * sFrame;
    smrCorruptedPageInfo  * sPageListInfo = NULL;
    stlInt32                i = 0;
    stlBool                 sIsValid;

    sTbsId         = aDatafileInfo->mTbsId;
    sDatafileId    = aDatafileInfo->mDatafileId;
    
    smpGetPchArray( sTbsId,
                    sDatafileId,
                    &sPchArrayList );

    STL_TRY( knlGetPhysicalAddr( sPchArrayList->mPchArray,
                                 (void**)&sPchArray,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY_THROW( aFileTail != NULL, RAMP_SKIP );

    sTbsOffset     = aFileTail->mTbsOffset;
    sTbsBackupSize = aFileTail->mBackupPageCount * SMP_PAGE_SIZE;
    
    STL_TRY( stlSeekFile( aFile,
                          STL_FSEEK_SET,
                          &sTbsOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    sDoneSize  = 0;
    sIdxBuffer = 0;
    sPageCount = 0;
    
    STL_RING_FOREACH_ENTRY( &aDatafileInfo->mCorruptedPageList, sPageListInfo )
    {
        i = 0;
        while( STL_TRUE )
        {
            /* Page List를 모두 순회. */
            if( i >= sPageListInfo->mPageCount )
            {
                break;
            }            

            /* Backup을 다시 읽는다. */
            if( sIdxBuffer >= sPageCount )
            {
                STL_TRY_THROW( sDoneSize != sTbsBackupSize, RAMP_SKIP );

                sReadSize = STL_MIN( aBufferSize, (sTbsBackupSize - sDoneSize) );
                
                STL_TRY( stlReadFile( aFile,
                                      aBuffer,
                                      sReadSize,
                                      &sSize,
                                      KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

                STL_DASSERT( sReadSize == sSize );
                
                sPageCount = sReadSize / SMP_PAGE_SIZE;
                sIdxBuffer = 0;
                sDoneSize += sReadSize;
            }
            
            sPhyHdr = (smpPhyHdr *)(aBuffer + (sIdxBuffer * SMP_PAGE_SIZE) );
            
            sSeqPid = sPageListInfo->mPages[i];
            sPageId = SMP_MAKE_PID( sDatafileId, sSeqPid );
            
            if( sPageId == sPhyHdr->mSelfId.mPageId )
            {
                STL_TRY( knlGetPhysicalAddr( sPchArray[sSeqPid].mFrame,
                                             (void**)&sFrame,
                                             KNL_ENV(aEnv) ) == STL_SUCCESS );

                STL_DASSERT( sFrame != NULL );

                STL_DASSERT( sPchArray[sSeqPid].mIsCorrupted == STL_FALSE );
                
                /**
                 * Incremental Backup Page의 Validation.
                 */ 
                STL_TRY( smpValidateChecksum( &aBuffer[sIdxBuffer * SMP_PAGE_SIZE],
                                              &sIsValid,
                                              aEnv )
                         == STL_SUCCESS );

                if( sIsValid == STL_FALSE )
                {
                    sPchArray[sSeqPid].mIsCorrupted = STL_TRUE;
                    STL_THROW( RAMP_ERR_CORRUPTED_BACKUP);
                }
                
                stlMemcpy( sFrame, &aBuffer[sIdxBuffer * SMP_PAGE_SIZE], SMP_PAGE_SIZE );

                sPchArray[sSeqPid].mRecoveryLsn = sPhyHdr->mLsn;
                sPchArray[sSeqPid].mPageType    = sPhyHdr->mPageType;
                sPchArray[sSeqPid].mDirty       = STL_TRUE;
                
                sIdxBuffer++;
                i++;
            }
            else if( sPhyHdr->mSelfId.mPageId < sPageId )
            {
                sIdxBuffer++;
            }
            else
            {   
                i++;
            }                                
        }
    }
    
    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CORRUPTED_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_BACKUP_IS_CORRUPTED,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      aBackupIterator->mFileName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Corruption 옵션을 사용한 Datafile Recovery 중 Corruption Page List를 생성한다.
 * @param[in,out] aDatafileInfo  Datafile info
 * @param[in,out] aEnv  Environment
 */ 
stlStatus smpMakeCorruptedPageList( smrRecoveryDatafileInfo * aDatafileInfo,
                                    smlEnv                  * aEnv )
{
    smpPchArrayList       * sPchArrayList = NULL;
    smpCtrlHeader         * sPchArray     = NULL;
    smrCorruptedPageInfo  * sPageListInfo;
    stlUInt64               sMaxPageCount;
    stlUInt64               sIdxPch  = 0;
    stlInt32                sIdxList = 0;
    smpPhyHdr             * sPhyHdr;
    void                  * sFrame;
    smfTbsInfo            * sTbsInfo;
    smfDatafileInfo       * sDatafileInfo;
    stlInt32                sState = 0;
    
    smpGetPchArray( aDatafileInfo->mTbsId,
                    aDatafileInfo->mDatafileId,
                    &sPchArrayList );

    sTbsInfo      = smfGetTbsHandle( aDatafileInfo->mTbsId );
    sDatafileInfo = sTbsInfo->mDatafileInfoArray[aDatafileInfo->mDatafileId];

    STL_TRY( knlGetPhysicalAddr( sPchArrayList->mPchArray,
                                 (void**)&sPchArray,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );

    sMaxPageCount = smfGetMaxPageCount( aDatafileInfo->mTbsId,
                                        aDatafileInfo->mDatafileId );

    STL_TRY( knlAllocDynamicMem( &aEnv->mDynamicHeapMem,
                                 STL_SIZEOF( smrCorruptedPageInfo ),
                                 (void**) &sPageListInfo,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    stlMemset( sPageListInfo, 0x00, STL_SIZEOF( smrCorruptedPageInfo ) );

    STL_RING_INIT_DATALINK( sPageListInfo,
                            STL_OFFSETOF( smrCorruptedPageInfo, mCorruptedPageLink ) );

    STL_RING_ADD_LAST( &aDatafileInfo->mCorruptedPageList, sPageListInfo );

    sState = 1;

    /* PchArray 순회하면서 IsCorrupted Flag 확인. */
    for( sIdxPch = 0; sIdxPch < sMaxPageCount; sIdxPch++ )
    {
        if( sPchArray[sIdxPch].mIsCorrupted == STL_TRUE )
        {
            /* PageList의 count 확인을 하고 확장한다. */
            if( sIdxList >= SMR_RECOVERY_PAGE_LIST_MAX_COUNT )
            {
                sPageListInfo->mPageCount = sIdxList;

                STL_TRY( knlAllocDynamicMem( &aEnv->mDynamicHeapMem,
                                             STL_SIZEOF( smrCorruptedPageInfo ),
                                             (void**) &sPageListInfo,
                                             KNL_ENV(aEnv) ) == STL_SUCCESS );

                stlMemset( sPageListInfo, 0x00, STL_SIZEOF( smrCorruptedPageInfo ) );

                STL_RING_INIT_DATALINK( sPageListInfo,
                                        STL_OFFSETOF( smrCorruptedPageInfo,
                                                      mCorruptedPageLink ) );

                /* 추가된 corrupted page list info를 ring에 추가한다. */
                STL_RING_ADD_LAST( &aDatafileInfo->mCorruptedPageList, sPageListInfo );

                sIdxList = 0;
            }

            STL_TRY( knlGetPhysicalAddr( sPchArray[sIdxPch].mFrame,
                                         (void**) &sFrame,
                                         KNL_ENV(aEnv) ) == STL_SUCCESS );

            STL_DASSERT( sFrame != NULL );

            sDatafileInfo->mCorruptionCount--;
            sPchArray[sIdxPch].mIsCorrupted = STL_FALSE;

            stlMemset( sFrame, 0x00, SMP_PAGE_SIZE );

            sPhyHdr = (smpPhyHdr*) sFrame;
            sPhyHdr->mTimestamp = smfGetTimestamp( aDatafileInfo->mTbsId,
                                                   aDatafileInfo->mDatafileId );

            sPchArray[sIdxPch].mRecoveryLsn = 0;
            sPchArray[sIdxPch].mPageType    = SMP_PAGE_TYPE_INIT;
            sPchArray[sIdxPch].mDirty       = STL_TRUE;
            
            sPageListInfo->mPages[sIdxList] = SMP_PAGE_SEQ_ID( sPchArray[sIdxPch].mPageId );
            sIdxList++;
        }
    }

    sPageListInfo->mPageCount = sIdxList;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smpDestroyCorruptedPageList( aDatafileInfo, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Datafile Recovery 중 Restore 이후 Corrupted Page List를 해제한다.
 * @param[in] aDatafileInfo Datafile Info
 * @param[in,out] aEnv  Environment
 */ 
stlStatus smpDestroyCorruptedPageList( smrRecoveryDatafileInfo * aDatafileInfo,
                                       smlEnv                  * aEnv )
{
    smrCorruptedPageInfo  * sPageListInfo = NULL;
    smrCorruptedPageInfo  * sNextPageListInfo = NULL;

    STL_RING_FOREACH_ENTRY_SAFE( &aDatafileInfo->mCorruptedPageList, sPageListInfo, sNextPageListInfo )
    {
        STL_TRY( knlFreeDynamicMem( &aEnv->mDynamicHeapMem,
                                    (void*)sPageListInfo,
                                    KNL_ENV(aEnv) ) == STL_SUCCESS );

        STL_RING_UNLINK( &aDatafileInfo->mCorruptedPageList, sPageListInfo );

        if( sNextPageListInfo == NULL )
        {
            break;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
