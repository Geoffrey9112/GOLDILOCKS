/*******************************************************************************
 * kniStartup.c
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

/**
 * @file kniStartup.c
 * @brief Kernel Layer Startup Internal Routines
 */

#include <knl.h>

#include <knDef.h>
#include <knaLatch.h>
#include <knsSegment.h>
#include <kniProcess.h>
#include <kniStartup.h>
#include <kneEvent.h>
#include <kngLogger.h>
#include <knbBreakPoint.h>
#include <knt.h>
#include <knpc.h>

#include <knpDef.h>
#include <knpPropertyManager.h>

extern knsEntryPoint * gKnEntryPoint;
extern knlFxTableDesc  gLatchTableDesc;
extern knlFxTableDesc  gShmTableDesc;
extern knlFxTableDesc  gPropertyTableDesc;
extern knlFxTableDesc  gSPropertyTableDesc;
extern knlFxTableDesc  gProcessInfoTableDesc;
extern knlFxTableDesc  gKnEnvTableDesc;
extern knlFxTableDesc  gKnnSignalTableDesc;
extern knlFxTableDesc  gKnDualTableDesc;
extern knlFxTableDesc  gKnProcStatTabDesc;
extern knlFxTableDesc  gKnSessStatTabDesc;
extern knlFxTableDesc  gKniSystemInfoTableDesc;
extern knlFxTableDesc  gKnInstanceTableDesc;
extern knlFxTableDesc  gKnDumpMemoryTableDesc;
extern knlLatchScanCallback gShmManagerLatchCallback;
extern knlFxTableDesc  gKnpcSqlCacheTableDesc;
extern knlFxTableDesc  gKnpcPlanClockTableDesc;
extern knlFxTableDesc  gKnDynamicMemTableDesc;
extern knlFxTableDesc  gKnDumpDynamicMemTableDesc;
extern knlFxTableDesc  gKnAllocatorTableDesc;
extern knlFxTableDesc  gKnfCrashFileTableDesc;
extern knlFxTableDesc  gKnDbFileTableDesc;

extern stlProc       gProcessInfo;
extern stlInt32      gProcessTableID;
extern knlEventTable gKnEventTable[];
extern stlUInt16     gCurLatchClassCount;
stlFatalHandler      gKnOldFatalHandler;
stlSignalExFunc      gKnOldHupSignalHandler;
stlSignalExFunc      gKnOldTermSignalHandler;
stlSignalExFunc      gKnOldAbrtSignalHandler;
stlSignalExFunc      gKnOldQuitSignalHandler;
stlUInt32            gKnSignalCas = KNI_SIGNAL_CAS_INIT;
stlUInt64            gKnProcessSar = KNL_SAR_SESSION;
kntHashBucket      * gKnXaHashTable;

/**
 * @addtogroup kniStartup
 * @{
 */

/**
 * @brief Kernel Layer를 No Mount상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[out] aEnv Environment 구조체
 */
stlStatus kniStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv )
{
    stlInt64          sProcessCount = 0;
    stlCpuInfo        sCpuInfo;
    stlInt64          sSpinCount = 0;
    stlInt64          sBusyWaitCount = 0;
    stlInt32          sState = 0;
    stlBool           sIsHeap = STL_TRUE;
    
#if defined( STL_HAVE_CPUSET )
    stlCpuSet         sCpuSet;
    stlInt32          i;
#endif
    
    gKnEntryPoint = NULL;
    gCurLatchClassCount = 0;

    /* 현재 Process의 정보를 저장한다. */
    STL_TRY( kniSetCurrentProcessInfo( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /* Heap Memory 영역에 PROPERTY를 구성한다. */
    STL_TRY( knlInitializePropertyInHeap( KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;
    
    /* Session Environment에 Property를 복사한다. */
    STL_TRY( knlCopyPropertyValueToSessionEnv( KNL_SESS_ENV(aEnv),
                                               aEnv )
             == STL_SUCCESS );
    
#if defined( STL_HAVE_CPUSET )
    
    /**
     * Static Area를 0번 Cpu와 관련된 numa node에 binding한다.
     */
    
    if( knlGetPropertyBoolValueByID( KNL_PROPERTY_NUMA, aEnv ) )
    {
        STL_CPU_ZERO( &sCpuSet );
        STL_CPU_SET( KNI_NUMA_MASTER_CPU_ID, &sCpuSet );

        STL_TRY( stlSetThreadAffinity( &(((knlEnv*)aEnv)->mThread),
                                       &sCpuSet,
                                       KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }
    
#endif

    /* Process 내부적으로 사용할 Dynamic Memory 관리자를 생성한다 */
    STL_TRY( knlCreateProcDynamicMem( KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 2;

    /* static area를 생성한다. */
    STL_TRY( knsCreateStaticArea( KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 3;

    /**
     * fill version info
     */
    stlGetVersionInfo( &gKnEntryPoint->mVersionInfo );

    /* Fixed Table 의 Relation Header Information */
    gKnEntryPoint->mFxRelationInfo = NULL;
    
    /** 
     * Shared Memory Static Area에 PROPERTY를 구성한다. 
     * Heap Memory에 구성된 PROPERTY는 제거한다.
     */
    STL_TRY( knlInitializePropertyInShm( KNL_ENV(aEnv) ) == STL_SUCCESS );
    sIsHeap = STL_FALSE;

    /* Latch 의 Spin Count를 설정한다. */ 
    sSpinCount = knlGetPropertyBigIntValueAtShmByID( KNL_PROPERTY_SPIN_COUNT );

    stlSetMaxSpinCount( (stlUInt32)sSpinCount );
    
    /* Latch 의 Busy Wait Count를 설정한다. */ 
    sBusyWaitCount = knlGetPropertyBigIntValueAtShmByID( KNL_PROPERTY_BUSY_WAIT_COUNT );

    stlSetMaxBusyWaitCount( (stlUInt32)sBusyWaitCount );
    
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_PROCESS_MAX_COUNT,
                                      &sProcessCount,
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    /* Process Table을 생성한다. */
    STL_TRY( kniCreateProcessTable( sProcessCount,
                                    KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /* Process Table에서 사용 가능한 Process Element를 할당 받는다. */
    STL_TRY( kniAllocProcessTable( KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /* 시스템 Logger를 초기화 한다. */
    STL_TRY( knlCreateLogger( &gKnEntryPoint->mSystemLogger,
                              KNI_SYSTEM_LOGGER_NAME,
                              KNI_SYSTEM_LOGGER_MAXSIZE,
                              KNL_LOG_LEVEL_FATAL,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /* 시스템 이벤트를 관리할 객체들을 할당한다. */
    STL_TRY( knlCreateDynamicMem( &gKnEntryPoint->mEventMem,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_KERNEL_EVENT,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  KNM_EVENT_MEM_INIT_SIZE,
                                  KNM_EVENT_MEM_NEXT_SIZE,
                                  NULL, /* aMemController */
                                  aEnv ) == STL_SUCCESS );

    /* create allocator for SQL fixing block */
    STL_TRY( knlCreateDynamicMem( &gKnEntryPoint->mSqlFixMem,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_KERNEL_SQL_FIXING_BLOCK,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  8192,
                                  8192,
                                  NULL, /* aMemController */
                                  aEnv ) == STL_SUCCESS );

    knlRegisterEventTable( STL_LAYER_KERNEL, gKnEventTable );

    /* break point 정보를 초기화 한다 */
    STL_TRY( knbInitializeBpInfo( &gKnEntryPoint->mBreakPointInfo,
                                  KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /* X$LATCHES에 공유 메모리 관리자의 latch 등록 */
    STL_TRY( knlRegisterLatchScanCallback( &gShmManagerLatchCallback,
                                           KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /* Fixed Table Manager를 초기화한다. */
    STL_TRY( knlInitializeFxTableMgr( KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 4;

    /*Fixed Table 을 등록 */
    STL_TRY( kniRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF( kntHashBucket ) * KNT_XA_HASH_SIZE,
                                                  (void**)&gKnXaHashTable,
                                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    gKnEntryPoint->mXaHashSize = KNT_XA_HASH_SIZE;
    gKnEntryPoint->mXaHashTable = gKnXaHashTable;

    STL_TRY( kntInitHashTable( aEnv ) == STL_SUCCESS );
    
    /**
     * Plan Cache 초기화
     */

    STL_TRY( knpcInitPlanCache( gKnEntryPoint, aEnv ) == STL_SUCCESS );

    STL_TRY( knlInitializeFileMgr( KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( kngPrintStartMsg( &gKnEntryPoint->mSystemLogger,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Numa inforation 초기화 및 구성
     */
    STL_TRY( kniInitializeNumaInfo( &(gKnEntryPoint->mNumaInfo),
                                    aEnv )
             == STL_SUCCESS );

    /* database file중 kernel layer에서 사용하는 file을 file manager에 등록한다. */
    STL_TRY( kniRegisterDbFiles( KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( stlGetCpuInfo( &sCpuInfo,
                             KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    gKnEntryPoint->mCpuSetSize = sCpuInfo.mCpuCount;
    gKnEntryPoint->mEnableDiskIo = STL_TRUE;

#if defined( STL_HAVE_CPUSET )
    
    if( knlGetPropertyBoolValueByID( KNL_PROPERTY_NUMA, aEnv ) )
    {
        STL_CPU_ZERO( &sCpuSet );

        for( i = 0; i < gKnEntryPoint->mCpuSetSize; i++ )
        {
            STL_CPU_SET( i, &sCpuSet );
        }
    
        STL_TRY( stlSetThreadAffinity( &(((knlEnv*)aEnv)->mThread),
                                       &sCpuSet,
                                       KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }
    
#endif
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 4:
            (void) knlFinalizeFxTableMgr( KNL_ENV(aEnv) );
        case 3:
            (void) knsDetachStaticArea( KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyProcDynamicMem( KNL_ENV(aEnv) );
        case 1:
            if( sIsHeap == STL_TRUE )
            {
                (void) knlFinalizePropertyInHeap( KNL_ENV(aEnv) );
            }
        default:
            break;
    }

    gKnEntryPoint = NULL;
    gCurLatchClassCount = 0;
    
    return STL_FAILURE;
}

/**
 * @brief Kernel Layer를 Mount 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[out] aEnv Environment 구조체
 */
stlStatus kniStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv )
{
    STL_PARAM_VALIDATE( gKnEntryPoint->mStartupPhase == KNL_STARTUP_PHASE_NO_MOUNT,
                        KNL_ERROR_STACK( aEnv ) );

    STL_TRY( knlMarkStaticArea( &(gKnEntryPoint->mStaticMark4Mount),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Kernel Layer를 PREOPEN 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[out] aEnv Environment 구조체
 */
stlStatus kniStartupPreOpen( knlStartupInfo * aStartupInfo,
                             void           * aEnv )
{
    STL_PARAM_VALIDATE( gKnEntryPoint->mStartupPhase == KNL_STARTUP_PHASE_MOUNT,
                        KNL_ERROR_STACK( aEnv ) );

    STL_TRY( knlMarkStaticArea( &(gKnEntryPoint->mStaticMark4Open),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /*
     * fork로 인해 프로세스 정보가 변경되었을 수 있으므로
     * 현재 Process의 정보로 다시 설정한다.
     */
    STL_TRY( kniSetCurrentProcessInfo( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Kernel Layer를 Warm-up 단계로 전환시킨다.
 * @param[out] aEnv Environment 구조체
 */
stlStatus kniWarmup( void * aEnv )
{
    stlInt32 sState = 0;
    stlInt64 sSpinCount;
    stlInt64 sBusyWaitCount = 0;
    
    gKnEntryPoint = NULL;
    gCurLatchClassCount = 0;

    /* 프로세스 테이블 ID를 초기화한다. */
    KNI_INIT_PROCESS_TABLE_ID();

    /* 현재 Process의 정보를 저장한다. */
    STL_TRY( kniSetCurrentProcessInfo( KNL_ENV(aEnv) ) == STL_SUCCESS );

    /* Process 내부적으로 사용할 Dynamic Memory 관리자를 생성한다 */
    STL_TRY( knlCreateProcDynamicMem( KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;

    /* static area를 attach 한다. */
    STL_TRY( knsAttachStaticArea( KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 2;

    /* Warmup이 허용된 상태인지 검사한다 */
    STL_TRY( knlValidateSar( KNL_SAR_WARMUP,
                             KNL_ENV(aEnv)->mTopLayer,
                             KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /** 
     * Shared Memory Static Area에 구성된 PROPERTY를 attach 한다.
     */
    STL_TRY( knpInitializePropertyAtWarmup( KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /* Session Environment에 Property를 복사한다. */
    STL_TRY( knlCopyPropertyValueToSessionEnv( KNL_SESS_ENV(aEnv),
                                               aEnv )
             == STL_SUCCESS );
    /**
     * 개발 편의성을 위해서 RELEASE 모드에서만 binary 호환성을 검사한다.
     */
#ifndef STL_DEBUG
    /* binary version validation */
    STL_TRY( knlValidateBinaryVersion( aEnv ) == STL_SUCCESS );
#endif

    knlRegisterEventTable( STL_LAYER_KERNEL, gKnEventTable );

    /* 모든 공유 메모리 세그먼트를 프로세스에 attach 한다. */
    STL_TRY( knsAttachAllSegment( KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 3;

    /* Latch 의 Spin Count를 설정한다. */ 
    sSpinCount = knlGetPropertyBigIntValueAtShmByID( KNL_PROPERTY_SPIN_COUNT );

    stlSetMaxSpinCount( (stlUInt32)sSpinCount );
    
    /* Latch 의 Busy Wait Count를 설정한다. */ 
    sBusyWaitCount = knlGetPropertyBigIntValueAtShmByID( KNL_PROPERTY_BUSY_WAIT_COUNT );

    stlSetMaxBusyWaitCount( (stlUInt32)sBusyWaitCount );

    /* Fixed Table Manager를 초기화한다. */
    STL_TRY( knlInitializeFxTableMgr( KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 4;

    /*Fixed Table 을 등록 */
    STL_TRY( kniRegisterFxTables( aEnv ) == STL_SUCCESS );

    gKnXaHashTable = gKnEntryPoint->mXaHashTable;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState > 2 )
    {
        knlLogMsgUnsafe( NULL,
                         KNL_ENV( aEnv ),
                         KNL_LOG_LEVEL_FATAL,
                         "[KERNEL WARMUP FAIL] %s\n", 
                         stlGetLastErrorMessage( KNL_ERROR_STACK( aEnv ) ) );
    }

    switch( sState )
    {
        case 4:
            (void)knlFinalizeFxTableMgr( KNL_ENV( aEnv ) );
        case 3:
            (void)knsDetachAllSegment( KNL_ENV( aEnv ) );
        case 2:
            (void)knsDetachStaticArea( KNL_ENV( aEnv ) );
        case 1:
            (void)knlDestroyProcDynamicMem( KNL_ENV( aEnv ) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Kernel Layer를 Cool-down 시킨다.
 * @param[out] aEnv Environment 구조체
 */
stlStatus kniCooldown( void * aEnv )
{
    /* Fixed Table Manager를 종료한다. */
    STL_TRY( knlFinalizeFxTableMgr( KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /* 사용중인 Process Element를 반환한다. */
    STL_TRY( kniFreeProcessTable( KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( knsDetachAllSegment( KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    STL_TRY( knsDetachStaticArea( KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    STL_TRY( knlDestroyProcDynamicMem( KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    /* 프로세스 테이블 ID를 초기화한다. */
    KNI_INIT_PROCESS_TABLE_ID();

    gKnEntryPoint = NULL;
    gCurLatchClassCount = 0;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Kernel Layer를 INIT 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[out] aEnv Environment 구조체
 */
stlStatus kniShutdownInit( knlShutdownInfo * aShutdownInfo,
                           void            * aEnv )
{
    STL_TRY( knlFinalizeFileMgr( KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /* Fixed Table Manager를 종료한다. */
    STL_TRY( knlFinalizeFxTableMgr( KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( knlDestroyProcDynamicMem( KNL_ENV( aEnv ) ) == STL_SUCCESS );

    if( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT )
    {
        STL_TRY( knlDestroyLogger( &gKnEntryPoint->mSystemLogger,
                                   KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }
    
    gKnEntryPoint->mStartupPhase = KNL_STARTUP_PHASE_NONE;
    
    /*
     * knsDestroyAllSegment()를 가장 마지막에 수행해야 함
     * ENV 를 Environment Manager로부터 할당받은 경우 ENV 가 이 단계에서 사라짐
     */
    STL_TRY( knsDestroyAllSegment( KNL_ENV( aEnv ) ) == STL_SUCCESS );

    gKnEntryPoint = NULL;
    gCurLatchClassCount = 0;
    
    STL_TRY( cmlTerminate() == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Kernel Layer를 DISMOUNT 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[out] aEnv Environment 구조체
 */
stlStatus kniShutdownDismount( knlShutdownInfo * aShutdownInfo,
                               void            * aEnv )
{
    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    STL_TRY( knlRestoreStaticArea( &(gKnEntryPoint->mStaticMark4Mount),
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    gKnEntryPoint->mStartupPhase = KNL_STARTUP_PHASE_NO_MOUNT;

    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Kernel Layer를 POST_CLOSE 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[out] aEnv Environment 구조체
 */
stlStatus kniShutdownPostClose( knlShutdownInfo * aShutdownInfo,
                                void            * aEnv )
{
    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    STL_TRY( knlRestoreStaticArea( &(gKnEntryPoint->mStaticMark4Open),
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    gKnEntryPoint->mStartupPhase = KNL_STARTUP_PHASE_MOUNT;

    STL_RAMP( RAMP_SHUTDOWN_ABORT );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Kernel Layer의 Signal Handler.
 * @param[in] aSigNo   Signal Number
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 */
void kniSignalHandler( stlInt32   aSigNo,
                       void     * aSigInfo,
                       void     * aContext )
{
    knlEnv        * sMyEnv = NULL;
    knlSessionEnv * sMySessEnv;
    knlEnv        * sProcEnv = NULL;
    knlSessionEnv * sProcSessEnv;
    stlInt32        sActiveSessionCount;
    stlSignalExFunc sOldSignalHandler = NULL;
    stlUInt64       sOldProcessSar = 0;

    /**
     * shared memory에 attach한 적이 없다면 무시한다.
     */
    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE, RAMP_SKIP );

    switch( aSigNo )
    {
        case STL_SIGNAL_HUP:
        case STL_SIGNAL_TERM:
        case STL_SIGNAL_QUIT:
        case STL_SIGNAL_ABRT:

            sMyEnv = knlGetMyEnv();

            if( sMyEnv != NULL )
            {
                sMySessEnv = KNL_SESS_ENV(sMyEnv);

                /**
                 * cleaning session 시 dead env가 ager session과 연결될 수 있기 때문에
                 * 이를 filtering 해야 한다.
                 */
                if( sMySessEnv != NULL )
                {
                    if( sMySessEnv->mTopLayer != STL_LAYER_SESSION )  /* filter ager session */
                    {
                        if( KNL_GET_SESSION_ACCESS_MASK( sMySessEnv ) == KNL_SESSION_ACCESS_ENTER )
                        {
                            sMySessEnv->mNeedPendingSignal = STL_TRUE;
                            sMySessEnv->mPendingSignalNo = aSigNo;
                        
                            (void)knlEndSession( sMySessEnv,
                                                 KNL_END_SESSION_OPTION_IMMEDIATE,
                                                 sMyEnv );

                            STL_THROW( RAMP_FINISH );
                        }
                    }
                }
            }

            knlLockSignal();
            
            /**
             * 이미 Process가 다른 Thread에 의해서 죽고 있다면 나는 무시한다.
             */

            sOldProcessSar = stlAtomicCas64( &gKnProcessSar,
                                             KNL_SAR_NONE,
                                             KNL_SAR_SESSION );

            if( sOldProcessSar == KNL_SAR_NONE )
            {
                /**
                 * 이미 signal이 수행되고 있다면
                 */

                if( aSigNo == STL_SIGNAL_ABRT )
                {
                    /**
                     * abort signal 이후에 abort signal이 반복적으로 호출된 경우는
                     * 쓰레드를 죽인다.
                     * - abort 함수는 user signal handler를 호출한 이후에 default signal handler을
                     *   부르기 때문에 multi-session이 살아 있는 상태에서 프로세스가 종료될수 있다.
                     * - enter 상태가 아닌 경우는 죽여도 상관없다.
                     * - enter 상태에서 발생한 경우라면 내부에 defect가 있음을 의미한다.
                     */
                    stlNativeExitThread();
                }
                
                knlUnlockSignal();
                STL_THROW( RAMP_FINISH );
            }

            sProcEnv = knlGetFirstMyProcessEnv();

            while( sProcEnv != NULL )
            {
                sProcSessEnv = KNL_SESS_ENV(sProcEnv);

                if( (sProcSessEnv != NULL) &&
                    (knlIsTerminatedSessionEnv( sProcSessEnv ) == STL_FALSE) &&
                    (sProcSessEnv->mTopLayer != STL_LAYER_SESSION) )  /* filter ager session */
                {
                    KNL_SET_SESSION_STATUS( sProcSessEnv, KNL_SESSION_STATUS_SIGNALED );
                    
                    (void)knlEndSession( sProcSessEnv,
                                         KNL_END_SESSION_OPTION_IMMEDIATE,
                                         sProcEnv );
                    
                    knlKillSession( sProcSessEnv );
                }

                sProcEnv = knlGetNextMyProcessEnv( sProcEnv );
            }

            knlUnlockSignal();

            /**
             * My proccess에 속한 모든 active session이 종료되기를 기다린다.
             */
            
            while( STL_TRUE )
            {
                sActiveSessionCount = 0;
                
                sProcEnv = knlGetFirstMyProcessEnv();
                
                while( sProcEnv != NULL )
                {
                    sProcSessEnv = KNL_SESS_ENV(sProcEnv);
                    
                    if( (sProcSessEnv != NULL) &&
                        (sProcSessEnv->mTopLayer != STL_LAYER_SESSION) )  /* filter ager session */
                    {
                        if( KNL_GET_SESSION_ACCESS_MASK( sProcSessEnv ) == KNL_SESSION_ACCESS_ENTER )
                        {
                            sActiveSessionCount++;
                        }
                    }

                    sProcEnv = knlGetNextMyProcessEnv( sProcEnv );
                }

                if( sActiveSessionCount == 0 )
                {
                    break;
                }

                stlSleep( 10000 );
            }

            break;
            
        case STL_SIGNAL_INT:

            sMyEnv = knlGetMyEnv();

            while( sMyEnv != NULL )
            {
                sMySessEnv = KNL_SESS_ENV(sMyEnv);
                
                if( (sMySessEnv != NULL) &&
                    (sMySessEnv->mTopLayer != STL_LAYER_SESSION) )  /* filter ager session */
                {
                    (void)knlEndStatement( sMySessEnv,
                                           KNL_END_STATEMENT_OPTION_CANCEL,
                                           sMyEnv );
                }

                sMyEnv = knlGetNextMyProcessEnv( sMyEnv );
            }

            STL_THROW( RAMP_FINISH );
            break;
            
        /**
         * 프로토콜상 USR1은 자신이 종료되어야 하는 시그널이다.
         */
        case STL_SIGNAL_USR1:
            stlExitProc( STL_FAILURE );
            break;
        default:
            break;
    }
    
    STL_RAMP( RAMP_SKIP );
    
    switch( aSigNo )
    {
        case STL_SIGNAL_HUP:
            sOldSignalHandler = gKnOldHupSignalHandler;
            break;
        case STL_SIGNAL_TERM:
            sOldSignalHandler = gKnOldTermSignalHandler;
            break;
        case STL_SIGNAL_ABRT:
            sOldSignalHandler = gKnOldAbrtSignalHandler;
            break;
        case STL_SIGNAL_QUIT:
            sOldSignalHandler = gKnOldQuitSignalHandler;
            break;
        default:
            break;
    }

    if( sOldSignalHandler != NULL )
    {
        if( sOldSignalHandler != (stlSignalExFunc)STL_SIGNAL_IGNORE )
        {
            sOldSignalHandler( aSigNo, aSigInfo, aContext );
        }
    }
    
    stlExitProc( STL_FAILURE );

    STL_RAMP( RAMP_FINISH );
}

/**
 * @brief Kernel Layer의 Fatal Handler.
 * @param[in] aCauseString Fatal 유발 원인을 알수 있는 스트링
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 */
void kniFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext )
{
    knlEnv            sHeapEnv;
    knlEnv          * sMyEnv = NULL;
    knlEnv          * sMyProcessEnv = NULL;
    stlInt32          sActiveSessionCount = 0;
    knlSessionEnv   * sMySessEnv;
    stlUInt32         sEnvId = -1;
    stlUInt32         sSessionEnvId = -1;
    stlProc           sOriginProc = { 0 };
    stlChar           sOriginProcName[128] = { 0, };

    stlGetSignalOriginProcess( aSigInfo, & sOriginProc );

    STL_TRY( knlInitializeEnv( &sHeapEnv,
                               KNL_ENV_HEAP )
             == STL_SUCCESS );

    /**
     * Sending Process의 Process 이름을 얻는다.
     */
    if( sOriginProc.mProcID > 0 )
    {
        (void) stlGetProcName( & sOriginProc,
                               sOriginProcName,
                               128,
                               KNL_ERROR_STACK(&sHeapEnv) );
    }
        
    stlBlockControllableSignals( STL_SIGNAL_UNKNOWN, KNL_ERROR_STACK( &sHeapEnv ) );

    /**
     * shared memory에 attach한 적이 없다면 무시한다.
     */
    STL_TRY_THROW( knsValidateStaticArea() == STL_TRUE, RAMP_FINISH );
    
    sMyEnv = knlGetMyEnv();
    
    /**
     * 자신의 Env를 찾지 못하는 경우
     * - Environment를 allocate하지 않은 경우이다.
     * - SQLAllocHandle(env) 와 SQLConnect 이전에 Fatal이 발생한 경우
     * - gmaster 에서 Fatal이 발생한 경우
     */
    STL_TRY_THROW( sMyEnv != NULL, RAMP_SKIP_FATAL );
    STL_TRY_THROW( knlGetEnvId( sMyEnv, &sEnvId ) == STL_SUCCESS,
                   RAMP_ERR_SYSTEM_FATAL );

    STL_TRY_THROW( gProcessTableID != 0, RAMP_ERR_SYSTEM_FATAL );
    
    sMySessEnv = KNL_SESS_ENV(sMyEnv);

    if( sMySessEnv != NULL )
    {
        STL_TRY_THROW( knlGetSessionEnvId( sMySessEnv,
                                           &sSessionEnvId,
                                           sMyEnv )
                       == STL_SUCCESS, RAMP_ERR_SYSTEM_FATAL );
    }

    /**
     * system fatal을 해결할수 있는지 확인한다.
     */
    
    STL_TRY_THROW( knlResolveSystemFatal( STL_TRUE, sMyEnv ) == STL_TRUE,
                   RAMP_ERR_SYSTEM_FATAL );

    if( aCauseString == NULL )
    {
        (void)knlLogMsgUnsafe( NULL,
                               &sHeapEnv,
                               KNL_LOG_LEVEL_FATAL,
                               "[SESSION FATAL] ENV(%d), SESSION(%d), THREAD(%d,%ld), ORIGIN(%d,%s)\n",
                               sEnvId,
                               sSessionEnvId,
                               sMyEnv->mThread.mProc.mProcID,
                               sMyEnv->mThread.mHandle,
                               sOriginProc.mProcID,
                               sOriginProcName );
    }
    else
    {
        (void)knlLogMsgUnsafe( NULL,
                               &sHeapEnv,
                               KNL_LOG_LEVEL_FATAL,
                               "[SESSION FATAL] ENV(%d), SESSION(%d), THREAD(%d,%ld), ORIGIN(%d,%s), CAUSE(\"%s\")",
                               sEnvId,
                               sSessionEnvId,
                               sMyEnv->mThread.mProc.mProcID,
                               sMyEnv->mThread.mHandle,
                               sOriginProc.mProcID,
                               sOriginProcName,
                               aCauseString );
    }

    (void)knlLogCallStackUnsafe( NULL, &sHeapEnv );

    if( sMySessEnv != NULL )
    {
        /**
         * Ager에 의해서 이미 cleaning 되고 있다면 skip 한다.
         */
        STL_TRY_THROW( sMySessEnv->mTopLayer != STL_LAYER_SESSION, RAMP_SKIP_FATAL );
        
        STL_TRY_THROW( knlGetSessionEnvId( sMySessEnv,
                                           &sSessionEnvId,
                                           sMyEnv )
                       == STL_SUCCESS, RAMP_ERR_SYSTEM_FATAL );
        
        KNL_SET_SESSION_STATUS( sMySessEnv, KNL_SESSION_STATUS_SIGNALED );
            
        if( sMySessEnv->mSessionType == KNL_SESSION_TYPE_DEDICATE )
        {
            KNL_SET_SESSION_ACCESS_KEY( sMySessEnv,
                                        KNL_SESSION_ACCESS_LEAVE,
                                        KNL_GET_SESSION_SEQ( sMySessEnv ) );
            
#ifdef STL_DEBUG
            stlLogCallstack( aSigInfo, aContext );
            abort();
#endif
    
            knlKillSession( sMySessEnv );
        }
        else
        {
            /**
             * Shared Session은 Dispatcher가 처리한다.
             */
        }
    }

    STL_RAMP( RAMP_SKIP_FATAL );
        
    stlUnblockControllableSignals( STL_SIGNAL_UNKNOWN, KNL_ERROR_STACK( &sHeapEnv ) );
    
    /**
     * 프로세스 종료를 원한다면
     */
    
    if( knlGetPropertyBigIntValueAtShmByID( KNL_PROPERTY_SESSION_FATAL_BEHAVIOR )
        == KNL_SESSION_FATAL_BEHAVIOR_KILL_PROCESS )
    {
        /**
         * My proccess에 속한 모든 active session을 종료시킨다.
         */

        gKnProcessSar = KNL_SAR_NONE;

        sMyProcessEnv = knlGetFirstMyProcessEnv();

        while( sMyProcessEnv != NULL )
        {
            if( sMyProcessEnv != sMyEnv )
            {
                sMySessEnv = KNL_SESS_ENV(sMyProcessEnv);

                if( (sMySessEnv != NULL) &&
                    (knlIsTerminatedSessionEnv( sMySessEnv ) == STL_FALSE) &&
                    (sMySessEnv->mTopLayer != STL_LAYER_SESSION) )  /* filter ager session */
                {
                    KNL_SET_SESSION_STATUS( sMySessEnv, KNL_SESSION_STATUS_SIGNALED );
                    
                    (void)knlEndSession( sMySessEnv,
                                         KNL_END_SESSION_OPTION_IMMEDIATE,
                                         sMyProcessEnv );
                    
                    knlKillSession( sMySessEnv );
                }
            }
            
            sMyProcessEnv = knlGetNextMyProcessEnv( sMyProcessEnv );
        }

        /**
         * My proccess에 속한 모든 active session이 종료되기를 기다린다.
         */
            
        while( STL_TRUE )
        {
            sActiveSessionCount = 0;
                
            sMyProcessEnv = knlGetFirstMyProcessEnv();
                
            while( sMyProcessEnv != NULL )
            {
                if( sMyProcessEnv != sMyEnv )
                {
                    sMySessEnv = KNL_SESS_ENV(sMyProcessEnv);
                    
                    if( (sMySessEnv != NULL) &&
                        (sMySessEnv->mTopLayer != STL_LAYER_SESSION) )  /* filter ager session */
                    {
                        if( KNL_GET_SESSION_ACCESS_MASK( sMySessEnv ) == KNL_SESSION_ACCESS_ENTER )
                        {
                            sActiveSessionCount++;
                        }
                    }
                }

                sMyProcessEnv = knlGetNextMyProcessEnv( sMyProcessEnv );
            }

            if( sActiveSessionCount == 0 )
            {
                break;
            }

            stlSleep( 10000 );
        }
        
        /**
         * 강제적인 core dump를 원한다면
         */
    
        if( knlGetPropertyBoolValueAtShmByID( KNL_PROPERTY_GENERATE_CORE_DUMP )
            == STL_TRUE )
        {
            abort();
        }
    }

    STL_RAMP( RAMP_FINISH );
    
    /**
     * Standard Layer의 Fatal Handler가 호출되고, 그 함수에서 종료된다.
     */
    
    gKnOldFatalHandler( aCauseString, aSigInfo, aContext );

    return;
    
    STL_CATCH( RAMP_ERR_SYSTEM_FATAL )
    {
        if( aCauseString == NULL )
        {
            (void)knlLogMsgUnsafe( NULL,
                                   &sHeapEnv,
                                   KNL_LOG_LEVEL_FATAL,
                                   "[SYSTEM FATAL] ORIGIN(%d,%s)",
                                   sOriginProc.mProcID,
                                   sOriginProcName );
        }
        else
        {
            (void)knlLogMsgUnsafe( NULL,
                                   &sHeapEnv,
                                   KNL_LOG_LEVEL_FATAL,
                                   "[SYSTEM FATAL] ORIGIN(%d,%s), CAUSE(\"%s\")",
                                   sOriginProc.mProcID,
                                   sOriginProcName,
                                   aCauseString );
        }

        (void)knlLogCallStackUnsafe( NULL, &sHeapEnv );

        (void)stlPrintFirstErrorCallstack( KNL_ERROR_STACK(&sHeapEnv) );

        (void)kniSystemFatal( STL_TRUE, STL_TRUE );
    }

    STL_FINISH;

    abort();
}


/**
 * @brief SYSTEM FATAL을 처리한다.
 * @note aDestroyShm 이 true 인 경우는 shared memory 접근으로
 *       인하여 core가 발생할수 있다.
 */
void kniSystemFatal( stlBool aDestroyShm,
                     stlBool aMakeCore )
{
    kniProcessManager * sProcMgr;
    kniProcessInfo    * sProcArray;
    kniProcessInfo    * sProcInfo;
    stlInt32            i;
    knlEnv              sEnv;

    /**
     * NO MOUNT/WARMUP 단계에서의 Fatal은 Kernel Entry Point가 NULL일수 있다.
     * - Process가 Shared Memory에 Attach 하기 전에 발생한 Fatal이기 때문에
     *   자기 자신만 종료해도 Resource의 Leak은 발생하지 않는다.
     */
    STL_TRY( gKnEntryPoint != NULL );
    
    STL_TRY( knlInitializeEnv( &sEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
    
    /**
     * 새로운 SESSION의 생성과 DATABASE의 접근을 막는다
     */
    STL_TRY( knlSetSar( KNL_SAR_NONE, &sEnv ) == STL_SUCCESS );

    /**
     * disk partial write를 막아야 한다.
     */
    knlDisableDiskIo();

    stlMemBarrier();

    /**
     * Disk IO를 진행하는 Session이 종료되기를 기다린다.
     */
    while( 1 )
    {
        if( knlHasFileOpenedSession() == STL_FALSE )
        {
            break;
        }

        stlSleep( 10000 );
    }
    
    /**
     * Gliese Master와 자신을 제외한 모든 프로세스를 종료시킨다.
     */
    sProcMgr = gKnEntryPoint->mProcessManager;
    sProcArray = (void*)sProcMgr + STL_SIZEOF( kniProcessManager );

    for( i = 1; i < sProcMgr->mMaxProcessCount; i++ )
    {
        sProcInfo = &sProcArray[i];

        /**
         * Active Process Table이 아닌 경우는 Skip
         */
        if( sProcInfo->mProc.mProcID == -1 )
        {
            continue;
        }
        
        /**
         * 자신의 프로세스 아이디라면 Skip
         */
        if( gProcessTableID == i )
        {
            continue;
        }

        /**
         * USR1 시그널을 받은 Process는 self termination한다.
         */
        STL_TRY( stlKillProc( &sProcInfo->mProc,
                              STL_SIGNAL_USR1,
                              KNL_ERROR_STACK( &sEnv ) )
                 == STL_SUCCESS );
    }
    
    /**
     * Gliese Master를 종료시킨다.
     * - 자신이 Gliese Master라면 caller에서 종료하도록 한다.
     */
    if( gProcessTableID != 0 )
    {
        sProcInfo = &sProcArray[0];
    
        STL_TRY( stlKillProc( &sProcInfo->mProc,
                              STL_SIGNAL_USR1,
                              KNL_ERROR_STACK( &sEnv ) )
                 == STL_SUCCESS );
    }

    if( aMakeCore == STL_TRUE )
    {
        /**
         * debug mode에서는 problem tracking을 위해 resource를 문제가 발생한
         * 상태에서 유지한다.
         */
        abort();
    }
    else
    {
        /**
         * Make Core가 FALSE인 경우 Segment를 해제 시에 Shm Mem 해제하고
         * Static Area를 해제 할 때 시스템 쓰레드를 정리하지 않은 상태여서 Core가 발생한다.
         */
        stlExitProc( -1 );
    }

    if( aDestroyShm == STL_TRUE )
    {
        (void)knsDestroyAllSegment( &sEnv );
    }

    return;

    STL_FINISH;

    return;
}

/**
 * @brief Numa 정보를 초기화하고 구성한다.
 * @param[in]   aNumaInfo     Target Numa Information
 * @param[out]  aEnv          Environment 구조체
 */
stlStatus kniInitializeNumaInfo( kniNumaInfo * aNumaInfo,
                                 knlEnv      * aEnv )
{
    stlChar           sNumaMap[KNL_PROPERTY_STRING_MAX_LENGTH];
    kniNumaNodeInfo * sNodeInfo;
    stlUInt64         sNodeIdx;
    stlInt16          sCpuIdx;
    stlChar         * sCurNumaMap;
    stlChar         * sCurCpu;
    stlChar         * sEndPtr;
    
    /**
     * configure numa map
     * "node_id:node_id:node_id..."
     */

    stlMemset( aNumaInfo, 0x00, STL_SIZEOF( kniNumaInfo ) );
    
    if( knlGetPropertyBoolValueByID( KNL_PROPERTY_NUMA, aEnv ) )
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_NUMA_MAP,
                                          sNumaMap,
                                          aEnv )
                 == STL_SUCCESS );

        sCurNumaMap = sNumaMap;
        sCpuIdx = 0;
    
        while( 1 )
        {
            sCurCpu = stlStrtok( sCurNumaMap,
                                 ":",
                                 &sCurNumaMap );

            if( sCurCpu == NULL )
            {
                break;
            }

            /**
             * 해당 Cpu는 사용하지 않는다.
             * - map 전체가 "x:x:x: ... " 라면 numa 를 off 한다.
             */
            if( stlStrcasecmp( sCurCpu, "x" ) == 0 )
            {
                continue;
            }
            
            /**
             * @todo 정교한 유효성 검사가 필요함.
             */
            STL_TRY_THROW( stlStrToUInt64( sCurCpu,
                                           STL_NTS,
                                           &sEndPtr,
                                           10,
                                           &sNodeIdx,
                                           KNL_ERROR_STACK( aEnv ) )
                           == STL_SUCCESS,
                           RAMP_ERR_INVALID_NUMA_MAP );

            STL_TRY_THROW( sNodeIdx >= 0 && sNodeIdx < KNI_MAX_NUMA_NODE_COUNT,
                           RAMP_ERR_INVALID_NUMA_MAP );

            sNodeInfo = &(aNumaInfo->mNodeInfo[sNodeIdx]);

            if( sNodeInfo->mCpuCount == 0 )
            {
                aNumaInfo->mNodeSeq[aNumaInfo->mNodeCount] = sNodeIdx;
                aNumaInfo->mNodeCount++;
            }
            
            sNodeInfo->mCpuArray[sNodeInfo->mCpuCount] = sCpuIdx;
            sNodeInfo->mCpuCount++;
            
            sCpuIdx++;
        }

        /**
         * Master Node Number
         */
        aNumaInfo->mMasterNodeId = aNumaInfo->mNodeSeq[0];
    }
    else
    {
        aNumaInfo->mNodeCount = 0;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_NUMA_MAP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_NUMA_MAP,
                      sCurCpu,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Kernel Layer에서 사용되는 database file을 register한다.
 * @param[out] aEnv Environment 구조체
 */
stlStatus kniRegisterDbFiles( knlEnv * aEnv )
{
    stlChar   sFileName[KNL_PROPERTY_STRING_MAX_LENGTH];

    stlMemset( sFileName, 0x00, KNL_PROPERTY_STRING_MAX_LENGTH );
    stlSnprintf( sFileName,
                 KNL_PROPERTY_STRING_MAX_LENGTH,
                 "%s"STL_PATH_SEPARATOR"%s",
                 ((knpPropertyMgr *)(gKnEntryPoint->mPropertyMem))->mHomeDir,
                 KNP_DEFAULT_PROPERTY_FILE );

    /* goldilocks properties file */
    STL_TRY( knlAddDbFile( sFileName,
                           KNL_DATABASE_FILE_TYPE_CONFIG,
                           aEnv ) == STL_SUCCESS );

    stlMemset( sFileName, 0x00, KNL_PROPERTY_STRING_MAX_LENGTH );
    stlSnprintf( sFileName,
                 KNL_PROPERTY_STRING_MAX_LENGTH,
                 "%s"STL_PATH_SEPARATOR"%s",
                 ((knpPropertyMgr *)(gKnEntryPoint->mPropertyMem))->mHomeDir,
                 KNP_DEFAULT_BINARY_PROPERTY_FILE );

    /* goldilocks binary properties file */
    STL_TRY( knlAddDbFile( sFileName,
                           KNL_DATABASE_FILE_TYPE_CONFIG,
                           aEnv ) == STL_SUCCESS );

    /* System trace log file */
    STL_TRY( knlAddDbFile( gKnEntryPoint->mSystemLogger.mName,
                           KNL_DATABASE_FILE_TYPE_TRACE_LOG,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus kniRegisterFxTables( knlEnv * aEnv )
{
    /* X$INSTANCE Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gKnInstanceTableDesc,
                                 aEnv )
             == STL_SUCCESS );

    /* X$LATCH Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gLatchTableDesc,
                                 aEnv )
             == STL_SUCCESS );

    /* X$SHM_SEGMENT Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gShmTableDesc,
                                 aEnv )
             == STL_SUCCESS );
    
    /* X$PROPERTY Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gPropertyTableDesc,
                                 aEnv )
             == STL_SUCCESS );

    /* X$SPROPERTY Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gSPropertyTableDesc,
                                 aEnv )
             == STL_SUCCESS );
   
    
    /* X$PROCESS_INFO Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gProcessInfoTableDesc,
                                 aEnv )
             == STL_SUCCESS );

    /* X$KN_ENV Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gKnEnvTableDesc,
                                 aEnv )
             == STL_SUCCESS );

    /* D$SIGNAL Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gKnnSignalTableDesc,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * X$DUAL Fixed Table을 등록한다
     */
    STL_TRY( knlRegisterFxTable( &gKnDualTableDesc,
                                 aEnv )
             == STL_SUCCESS );

    /* X$KN_PROC_STAT Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gKnProcStatTabDesc,
                                 aEnv )
             == STL_SUCCESS );

    /* X$KN_SESS_STAT Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gKnSessStatTabDesc,
                                 aEnv )
             == STL_SUCCESS );

    /* D$DUMP_MEMORY Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gKnDumpMemoryTableDesc,
                                 aEnv )
             == STL_SUCCESS );

    /* X$KN_SYSTEM_INFO Fixed Table을 등록한다 */
    STL_TRY( knlRegisterFxTable( &gKniSystemInfoTableDesc,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * X$SQL_CACHE Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gKnpcSqlCacheTableDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( knlRegisterFxTable( & gKnpcPlanClockTableDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( knlRegisterFxTable( & gKnDynamicMemTableDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( knlRegisterFxTable( & gKnDumpDynamicMemTableDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( knlRegisterFxTable( & gKnAllocatorTableDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * D$CRASH_FILE Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( &gKnfCrashFileTableDesc,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( knlRegisterFxTable( &gKnDbFileTableDesc,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$INSTANCE Table 정의
 */
knlFxColumnDesc gKnInstanceColumnDesc[] =
{
    {
        "VERSION",
        "database version",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( kniInstanceFxRecord, mVersion ),
        KNI_INSTANCE_VERSION_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "STARTUP_TIME",
        "time when the instance was started",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( kniInstanceFxRecord, mStartupTime ),
        STL_SIZEOF( stlTime ),
        STL_FALSE  /* nullable */
    },
    {
        "STATUS",
        "status of the instance",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( kniInstanceFxRecord, mStatus ),
        KNI_INSTANCE_STATUS_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};

/**
 * @brief X$INSTANCE Table의 open 함수
 * @param[in] aStmt Statement Pointer
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus openFxInstanceCallback( void   * aStmt,
                                  void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv )
{
    *(stlBool*)aPathCtrl = STL_TRUE;

    return STL_SUCCESS;
}

/**
 * @brief X$INSTANCE Table의 close 함수
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus closeFxInstanceCallback( void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$INSTANCE Table의 레코드 구축 함수
 * @param[in] aDumpObjHandle Dump Object Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aValueList 값을 담을 공간
 * @param[in]  aBlockIdx   Block Idx
 * @param[out] aTupleExist 레코드 존재 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus buildRecordFxInstanceCallback( void              * aDumpObjHandle,
                                         void              * aPathCtrl,
                                         knlValueBlockList * aValueList,
                                         stlInt32            aBlockIdx,
                                         stlBool           * aTupleExist,
                                         knlEnv            * aEnv )
{
    kniInstanceFxRecord  sRec;

    *aTupleExist = STL_FALSE;

    if( *(stlBool*)aPathCtrl == STL_TRUE )
    {
        sRec.mStartupTime = knlGetStartupTime();
        stlGetVersionString( sRec.mVersion, KNI_INSTANCE_VERSION_LENGTH );

        switch( knlGetCurrStartupPhase() )
        {
            case KNL_STARTUP_PHASE_NO_MOUNT:
                stlSnprintf( sRec.mStatus,
                             KNI_INSTANCE_STATUS_LENGTH,
                             "STARTED" );
                break;
            case KNL_STARTUP_PHASE_MOUNT:
                stlSnprintf( sRec.mStatus,
                             KNI_INSTANCE_STATUS_LENGTH,
                             "MOUNTED" );
                break;
            case KNL_STARTUP_PHASE_OPEN:
                stlSnprintf( sRec.mStatus,
                             KNI_INSTANCE_STATUS_LENGTH,
                             "OPEN" );
                break;
            default:
                stlSnprintf( sRec.mStatus,
                             KNI_INSTANCE_STATUS_LENGTH,
                             "UNKNOWN" );
                break;
        }
        
        STL_TRY( knlBuildFxRecord( gKnInstanceColumnDesc,
                                   &sRec,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;

        *(stlBool*)aPathCtrl = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$INSTANCE Table 정보
 */
knlFxTableDesc gKnInstanceTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxInstanceCallback,
    closeFxInstanceCallback,
    buildRecordFxInstanceCallback,
    STL_SIZEOF( stlBool ),
    "X$INSTANCE",
    "current instance information fixed table",
    gKnInstanceColumnDesc
};

/**
 * X$KN_SYSTEM_INFO
 */
knlFxSystemInfo gKniSystemInfoRows[KNI_SYSTEM_INFO_ROW_COUNT] =
{
    {
        "SYSTEM_SAR",
        0,
        KNL_STAT_CATEGORY_NONE,
        "system available resource( 0:none, 1:session 2:database )"
    },
    {
        "MAX_ENVIRONMENT_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "maximum environment count"
    },
    {
        "FREE_ENVIRONMENT_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "available environment identifier"
    },
    {
        "MAX_SESSION_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "maximum session count"
    },
    {
        "FREE_SESSION_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "available session identifier"
    },
    {
        "MAX_PROCESS_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "maximum process count"
    },
    {
        "FREE_PROCESS_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        "available process identifier"
    },
    {
        "CACHE_ALIGNED_SIZE",
        0,
        KNL_STAT_CATEGORY_NONE,
        "cache aligned size"
    },
    {
        "CPU_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "count of CPUs"
    },
    {
        "SYSTEM_TIME",
        0,
        KNL_STAT_CATEGORY_NONE,
        "system time"
    },
    {
        "CLOCK_AGING_COUNT",
        0,
        KNL_STAT_CATEGORY_NONE,
        "clock aging count"
    },
    {
        "PLAN_CACHE_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "total size of plan caches"
    },
    {
        "FIXED_STATIC_ALLOC_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "fixed allocated size on static area"
    },
    {
        "VARIABLE_STATIC_ALLOC_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "variable allocated size on static area"
    },
    {
        "VARIABLE_STATIC_FRAG_FREE_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "variable fragmented free size on static area"
    },
    {
        "EVENT_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "event memory total size"
    }
};

stlStatus kniOpenSystemInfoFxTableCallback( void   * aStmt,
                                            void   * aDumpObjHandle,
                                            void   * aPathCtrl,
                                            knlEnv * aKnlEnv )
{
    kniFxSystemInfoPathCtrl * sPathCtrl;
    kneManager              * sEnvManager;
    kncManager              * sSessionManager;
    kniProcessManager       * sProcessManager;
    stlInt64                * sValues;
    stlInt64                  sRowIdx = 0;

    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( gKnEntryPoint->mEnvManager != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( gKnEntryPoint->mSessionEnvManager != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( gKnEntryPoint->mProcessManager != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl  = (kniFxSystemInfoPathCtrl*)aPathCtrl;
    sPathCtrl->mIterator = 0;

    sValues = sPathCtrl->mValues;

    /**
     * SAR 설정
     */
    sValues[sRowIdx++] = gKnEntryPoint->mSar;
    
    /**
     * Environment 설정
     */
    sEnvManager = gKnEntryPoint->mEnvManager;

    sValues[sRowIdx++] = sEnvManager->mCount;

    if( sEnvManager->mFreeEnvList != NULL )
    {
        sValues[sRowIdx++] = sEnvManager->mFreeEnvList->mIdx;
    }
    else
    {
        sValues[sRowIdx++] = -1;
    }
    
    /**
     * Session 설정
     */
    sSessionManager = gKnEntryPoint->mSessionEnvManager;

    sValues[sRowIdx++] = sSessionManager->mCount;

    if( sSessionManager->mFreeList != NULL )
    {
        sValues[sRowIdx++] = sSessionManager->mFreeList->mIdx;
    }
    else
    {
        sValues[sRowIdx++] = -1;
    }
    
    /**
     * Process 설정
     */
    sProcessManager = gKnEntryPoint->mProcessManager;

    sValues[sRowIdx++] = sProcessManager->mMaxProcessCount;
    sValues[sRowIdx++] = sProcessManager->mFirstFreeProcess;
    
    /**
     * cache aligned size 설정
     */
    sValues[sRowIdx++] = gKnEntryPoint->mCacheAlignedSize;

    /**
     * cpu set size 설정
     */
    sValues[sRowIdx++] = gKnEntryPoint->mCpuSetSize;

    /**
     * system time 설정
     */
    sValues[sRowIdx++] = gKnEntryPoint->mSystemTime;

    /**
     * plan cache
     */
    sValues[sRowIdx++] = gKnEntryPoint->mClockAgingCount;
    sValues[sRowIdx++] = gKnEntryPoint->mMemController.mTotalSize;

    /**
     * static area info
     */
    sValues[sRowIdx++] = gKnEntryPoint->mFixedOffset;
    sValues[sRowIdx++] = knsGetVariableStaticAreaTotalSize();
    sValues[sRowIdx++] = knsGetVariableStaticAreaFragSize();
    sValues[sRowIdx++] = gKnEntryPoint->mEventMem.mTotalSize;

    STL_DASSERT( sRowIdx == KNI_SYSTEM_INFO_ROW_COUNT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus kniCloseSystemInfoFxTableCallback( void   * aDumpObjHandle,
                                             void   * aPathCtrl,
                                             knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}

stlStatus kniBuildRecordSystemInfoFxTableCallback( void              * aDumpObjHandle,
                                                   void              * aPathCtrl,
                                                   knlValueBlockList * aValueList,
                                                   stlInt32            aBlockIdx,
                                                   stlBool           * aTupleExist,
                                                   knlEnv            * aKnlEnv )
{
    kniFxSystemInfoPathCtrl * sPathCtrl;
    knlFxSystemInfo           sFxSystemInfo;
    knlFxSystemInfo         * sRowDesc;
    stlInt64                * sValues;

    *aTupleExist = STL_FALSE;
    
    sPathCtrl = (kniFxSystemInfoPathCtrl*)aPathCtrl;
    sValues = sPathCtrl->mValues;

    sRowDesc = &gKniSystemInfoRows[sPathCtrl->mIterator];

    if( sPathCtrl->mIterator < KNI_SYSTEM_INFO_ROW_COUNT )
    {
        sFxSystemInfo.mName     = sRowDesc->mName;
        sFxSystemInfo.mValue    = sValues[sPathCtrl->mIterator];
        sFxSystemInfo.mComment  = sRowDesc->mComment;
        sFxSystemInfo.mCategory = sRowDesc->mCategory;

        STL_TRY( knlBuildFxRecord( gKnlSystemInfoColumnDesc,
                                   &sFxSystemInfo,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;
        sPathCtrl->mIterator += 1;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


knlFxTableDesc gKniSystemInfoTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    kniOpenSystemInfoFxTableCallback,
    kniCloseSystemInfoFxTableCallback,
    kniBuildRecordSystemInfoFxTableCallback,
    STL_SIZEOF( kniFxSystemInfoPathCtrl ),
    "X$KN_SYSTEM_INFO",
    "system information of kernel manager layer",
    gKnlSystemInfoColumnDesc
};

/** @} */
