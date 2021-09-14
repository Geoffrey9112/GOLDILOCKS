/*******************************************************************************
 * smpStartup.c
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
#include <smpDef.h>
#include <smf.h>
#include <smp.h>
#include <smr.h>
#include <smg.h>
#include <smxl.h>
#include <smpmMemory.h>
#include <smlGeneral.h>

/**
 * @file smpStartup.c
 * @brief Storage Manager Layer Page Startup Routines
 */

smpWarmupEntry * gSmpWarmupEntry;

extern knlFxTableDesc       gSmpSignpostTableDesc;
extern knlFxTableDesc       gSmpRtsTableDesc;
extern knlFxTableDesc       gSmpSegHdrTableDesc;
extern knlFxTableDesc       gSmpPageDumpTableDesc;
extern knlFxTableDesc       gSmpPchTableDesc;
extern smrRedoVector        gSmpRedoVectors[];
extern smxlUndoFunc         gSmpUndoFuncs[];
extern knlLatchScanCallback gPchLatchScanCallback;

/**
 * @addtogroup smp
 * @{
 */

/**
 * @brief Page Access Component를 NO MOUNT 모드로 전이한다.
 * @param[out] aWarmupEntry Page Access Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smpStartupNoMount( void   ** aWarmupEntry,
                             smlEnv  * aEnv )
{
    smpWarmupEntry * sWarmupEntry;
    
    /**
     * structure validation
     */
    STL_DASSERT( (STL_SIZEOF(smpCtrlHeader) % 64) == 0 );
    
    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF( smpWarmupEntry ),
                                      (void**)&sWarmupEntry,
                                      KNL_ENV(aEnv) ) == STL_SUCCESS );

    sWarmupEntry->mPchArrayList  = NULL;
    
    gSmpWarmupEntry = sWarmupEntry;
    *aWarmupEntry = (void*)sWarmupEntry;

    sWarmupEntry->mMinFlushedLsn = -1;
    sWarmupEntry->mPageFlushingTime = 0;
    sWarmupEntry->mFlushedPageCount = 0;
    
    STL_TRY( knlInitLatch( &sWarmupEntry->mLatch,
                           STL_TRUE,
                           KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlRegisterLatchScanCallback( &gPchLatchScanCallback,
                                           KNL_ENV(aEnv) ) == STL_SUCCESS );

    smrRegisterRedoVectors( SMG_COMPONENT_PAGE_ACCESS,
                            gSmpRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_PAGE_ACCESS, 
                           gSmpUndoFuncs );

    STL_TRY( smpRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    smgRegisterPendOp( SMG_PEND_SHRINK_MEMORY,
                       smpmShrinkPending );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page Access Component를 MOUNT 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smpStartupMount( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Page Access Component를 PREOPEN 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smpStartupPreOpen( smlEnv * aEnv )
{
    smlTbsId              sTbsId;
    stlBool               sTbsExist;
    stlBool               sDatafileExist;
    smlDatafileId         sDatafileId;
    stlThread             sThread[ SMP_MAX_PARALLEL_LOAD_FACTOR ];
    stlInt64              sParallelFactor;
    stlStatus             sReturnStatus[ SMP_MAX_PARALLEL_LOAD_FACTOR ];
    smpParallelLoadArgs   sArgs[ SMP_MAX_PARALLEL_LOAD_FACTOR ];
    smpParallelLoadJob  * sJob;
    knlRegionMark         sMemMark;
    stlInt32              sState = 0;
    stlInt32              i;
    stlInt32              sParallelId;
    
    /**
     * Initialization
     */
    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;

    for( i = 0; i < SMP_MAX_PARALLEL_LOAD_FACTOR; i++ )
    {
        STL_RING_INIT_HEADLINK( &sArgs[i].mJobs,
                                STL_OFFSETOF( smpParallelLoadJob, mJobLink ) );
        STL_INIT_ERROR_STACK( &(sArgs[i].mErrorStack) );
    }
    
    /**
     * Read property
     */

    sParallelFactor = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PARALLEL_LOAD_FACTOR,
                                                     KNL_ENV( aEnv ) );

    /**
     * Distribute parallel jobs
     */
    
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );
    sParallelId = 0;

    while( sTbsExist == STL_TRUE )
    {
        /**
         * 아래 경우는 PCH를 초기화할 필요 없다.
         * - Aging Tablespace
         * - OFFLINE Tablespace
         */
        if( (smfGetTbsState( sTbsId ) != SMF_TBS_STATE_AGING) &&
            (SMF_IS_ONLINE_TBS( sTbsId ) == STL_TRUE) )
        {
            STL_TRY( smfFirstDatafile( sTbsId,
                                       &sDatafileId,
                                       &sDatafileExist,
                                       aEnv ) == STL_SUCCESS );

            while( sDatafileExist == STL_TRUE )
            {
                /**
                 * 이미 초기화 되어 있는 PCH는 초기화 하지 않는다.
                 */
                if( smfGetDatafileShmState( sTbsId, sDatafileId ) >=
                    SMF_DATAFILE_SHM_STATE_PCH_INIT )
                {
                    STL_TRY( smfNextDatafile( sTbsId,
                                              &sDatafileId,
                                              &sDatafileExist,
                                              aEnv ) == STL_SUCCESS );
                    continue;
                }
                
                STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                            STL_SIZEOF( smpParallelLoadJob ),
                                            (void**)&sJob,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_RING_INIT_DATALINK( sJob, STL_OFFSETOF( smpParallelLoadJob, mJobLink ) );
            
                sJob->mTbsId = sTbsId;
                sJob->mDatafileId = sDatafileId;

                STL_RING_ADD_LAST( &sArgs[sParallelId % sParallelFactor].mJobs, sJob );
                sParallelId++;
            
                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sDatafileExist,
                                          aEnv ) == STL_SUCCESS );
            }
        }
        
        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }

    /**
     * Parallel load
     */

    sParallelFactor = STL_MIN( sParallelId, sParallelFactor );

    for( i = 0; i < sParallelFactor; i++ )
    {
        STL_TRY( stlCreateThread( &sThread[i],
                                  NULL,                 /* aThreadAttr */
                                  smpParallelLoadThread,
                                  (void*)&sArgs[i],     /* aArgs */
                                  KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
                         
    }

    /**
     * Waiting parallel loading
     */
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
     * Extending Pch Array
     */
    
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );

    sParallelId = 0;
    while( sTbsExist == STL_TRUE )
    {
        /**
         * 아래 경우는 Data를 Extend할 필요 없다.
         * - Aging Tablespace
         * - OFFLINE Tablespace
         */
        if( (smfGetTbsState( sTbsId ) != SMF_TBS_STATE_AGING) &&
            (SMF_IS_ONLINE_TBS( sTbsId ) == STL_TRUE) )
        {
            STL_TRY( smfFirstDatafile( sTbsId,
                                       &sDatafileId,
                                       &sDatafileExist,
                                       aEnv ) == STL_SUCCESS );
            
            while( sDatafileExist == STL_TRUE )
            {
                STL_TRY( smpExtend( NULL,
                                    sTbsId,
                                    sDatafileId,
                                    STL_FALSE,
                                    aEnv ) == STL_SUCCESS );
                
                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sDatafileExist,
                                          aEnv ) == STL_SUCCESS );
            }
        }
        
        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }

        
    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                        &sMemMark,
                                        STL_TRUE, /* aFreeChunk */
                                        (knlEnv*)aEnv );
    }
    
    return STL_FAILURE;
}

/**
 * @brief Page Access Component를 OPEN 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smpStartupOpen( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Page Access Component를 Warmup 시킨다.
 * @param[in] aWarmupEntry Page Access Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smpWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv )
{
    gSmpWarmupEntry = (smpWarmupEntry*)aWarmupEntry;

    STL_TRY( knlRegisterLatchScanCallback( &gPchLatchScanCallback,
                                           KNL_ENV(aEnv) ) == STL_SUCCESS );

    smrRegisterRedoVectors( SMG_COMPONENT_PAGE_ACCESS,
                            gSmpRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_PAGE_ACCESS, 
                           gSmpUndoFuncs );

    STL_TRY( smpRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    smgRegisterPendOp( SMG_PEND_SHRINK_MEMORY,
                       smpmShrinkPending );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page Access Component를 Cooldown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smpCooldown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Page Access Component를 Bootdown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smpBootdown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Page Access Component를 Post Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpShutdownPostClose( smlEnv * aEnv )
{
    smpPchArrayList  * sPchArrayList;
    smpPchArrayList  * sNxtPchArrayList;
    
    sPchArrayList = gSmpWarmupEntry->mPchArrayList;
    
    while( sPchArrayList != NULL )
    {
        sNxtPchArrayList = sPchArrayList->mNext;
        
        STL_TRY( smgFreeShmMem4Mount( sPchArrayList,
                                      aEnv )
                 == STL_SUCCESS );

        sPchArrayList = sNxtPchArrayList;
    }
    
    gSmpWarmupEntry->mPchArrayList = NULL;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page Access Component를 Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpShutdownClose( smlEnv * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus smpRegisterFxTables( smlEnv * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gSmpSignpostTableDesc,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmpRtsTableDesc,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmpSegHdrTableDesc,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmpPageDumpTableDesc,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmpPchTableDesc,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );  

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void * STL_THREAD_FUNC smpParallelLoadThread( stlThread * aThread,
                                              void      * aArg )
{
    smpParallelLoadArgs  * sArgs = (smpParallelLoadArgs*)aArg;
    smpParallelLoadJob   * sJob;
    stlErrorStack          sErrorStack;
    stlInt32               sState     = 0;
    smlEnv               * sEnv;
    smlSessionEnv        * sSessionEnv;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( stlBlockRealtimeThreadSignals( &sErrorStack ) == STL_SUCCESS );
    
    /**
     * Allocation Env
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
     * Init pch chunk
     */
    
    STL_RING_FOREACH_ENTRY( &sArgs->mJobs, sJob )
    {
        STL_TRY( smpInit( sJob->mTbsId,
                          sJob->mDatafileId,
                          sEnv ) == STL_SUCCESS );
    }

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
    }

    stlExitThread( aThread, STL_FAILURE, &sErrorStack );
    
    return NULL;
}



/** @} */
