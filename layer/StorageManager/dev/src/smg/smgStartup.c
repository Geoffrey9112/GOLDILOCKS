/*******************************************************************************
 * smgStartup.c
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
 * @file smgStartup.c
 * @brief Storage Manager Layer General Startup Internal Routines
 */

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
#include <smrDef.h>
#include <smr.h>
#include <smt.h>
#include <sms.h>
#include <sme.h>
#include <smd.h>
#include <smn.h>
#include <smq.h>
#include <smxl.h>
#include <smkl.h>
#include <smgDef.h>
#include <smg.h>
#include <smlGeneral.h>

extern knlFxTableDesc gSmgEnvTableDesc;
extern knlFxTableDesc gSmgSystemInfoTableDesc;
extern knlFxTableDesc gSmgSessionEnvTableDesc;
extern knlFxTableDesc gSmgSessionStatTableDesc;
extern knlEventTable  gSmgEventTable[];
extern smrRedoVector  gSmgRedoVectors[];
extern smxlUndoFunc   gSmgUndoFuncs[];

void           ** gStorageManagerWarmupEntry;
void           ** gComponentWarmupEntry;
smgWarmupEntry  * gSmgWarmupEntry;
stlFatalHandler   gSmOldFatalHandler;
knlStartupPhase   gSmWarmupPhase = KNL_STARTUP_PHASE_NONE;

knlStartupFunc * gSmgStartupLayer[STL_LAYER_MAX] =
{
    NULL,       /* STL_LAYER_NONE */
    NULL,       /* STL_LAYER_STANDARD */
    NULL,       /* STL_LAYER_DATATYPE */
    NULL,       /* STL_LAYER_COMMUNICATION */
    gStartupKN, /* STL_LAYER_KERNEL */
    gStartupSC, /* STL_LAYER_SERVER_COMMUNICATION */
    gStartupSM, /* STL_LAYER_STORAGE_MANAGER */
    NULL,       /* STL_LAYER_EXECUTION_LIBRARY */
    NULL,       /* STL_LAYER_SQL_PROCESSOR */
    NULL,       /* STL_LAYER_PSM_PROCESSOR */
    NULL,       /* STL_LAYER_SESSION */
    NULL,       /* STL_LAYER_INSTANCE_MANAGER */
    NULL,       /* STL_LAYER_GLIESE_LIBRARY */
    NULL,       /* STL_LAYER_GLIESE_TOOL */
    NULL,       /* STL_LAYER_GOLDILOCKS_LIBRARY */
    NULL,       /* STL_LAYER_GOLDILOCKS_TOOL */
};


knlShutdownFunc * gSmgShutdownLayer[STL_LAYER_MAX] =
{
    NULL,        /* STL_LAYER_NONE */
    NULL,        /* STL_LAYER_STANDARD */
    NULL,        /* STL_LAYER_DATATYPE */
    NULL,        /* STL_LAYER_COMMUNICATION */
    gShutdownKN, /* STL_LAYER_KERNEL */
    gShutdownSC, /* STL_LAYER_SERVER_COMMUNICATION */
    gShutdownSM, /* STL_LAYER_STORAGE_MANAGER */
    NULL,        /* STL_LAYER_EXECUTION_LIBRARY */
    NULL,        /* STL_LAYER_SQL_PROCESSOR */
    NULL,        /* STL_LAYER_PSM_PROCESSOR */
    NULL,        /* STL_LAYER_SESSION */
    NULL,        /* STL_LAYER_INSTANCE_MANAGER */
    NULL,        /* STL_LAYER_GLIESE_LIBRARY */
    NULL,        /* STL_LAYER_GLIESE_TOOL */
    NULL,        /* STL_LAYER_GOLDILOCKS_LIBRARY */
    NULL,        /* STL_LAYER_GOLDILOCKS_TOOL */
};

/**
 * @addtogroup smg
 * @{
 */

/**
 * @brief Storage Manager Layer를 No Mount상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv )
{
    smgWarmupEntry * sWarmupEntry;
    stlInt32         sState = 0;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[STARTUP-SM] NO-MOUNT PHASE\n" )
             == STL_SUCCESS );
    
    /*
     * Storage Manager Layer Warmup Entry 초기화
     */
    STL_TRY( knlGetEntryPoint( STL_LAYER_STORAGE_MANAGER,
                               (void**)&gStorageManagerWarmupEntry,
                               (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF(void*) * SMG_COMPONENT_MAX,
                                      (void**)&gComponentWarmupEntry,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF( smgWarmupEntry ),
                                      (void**)&sWarmupEntry,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );

    /*
     * System Env를 위한 각 Level별 Allocator 초기화
     * 1. Database level shared memory
     */
    *gStorageManagerWarmupEntry = gComponentWarmupEntry;

    KNL_INIT_DYNAMIC_MEMORY( &(sWarmupEntry->mDatabaseMem4Mount) );
    KNL_INIT_DYNAMIC_MEMORY( &(sWarmupEntry->mDatabaseMem4Open) );
    
    sWarmupEntry->mCheckpointEnvId = KNL_INVALID_ENV_ID;
    sWarmupEntry->mPageFlusherEnvId = KNL_INVALID_ENV_ID;
    sWarmupEntry->mAgerEnvId = KNL_INVALID_ENV_ID;
    sWarmupEntry->mCleanupEnvId = KNL_INVALID_ENV_ID;
    sWarmupEntry->mArchivelogEnvId = KNL_INVALID_ENV_ID;
    sWarmupEntry->mAgerState = SML_AGER_STATE_ACTIVE;
    
    /*
     * Fixed Table 등록
     */
    
    STL_TRY( smgRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    smrRegisterRedoVectors( SMG_COMPONENT_EXTERNAL,
                            gSmgRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_EXTERNAL, 
                           gSmgUndoFuncs );

    knlRegisterEventTable( STL_LAYER_STORAGE_MANAGER, gSmgEventTable );
    
    /*
     * Warmup Entry 설정
     */
    gComponentWarmupEntry[SMG_COMPONENT_GENERAL] = sWarmupEntry;
    gSmgWarmupEntry = sWarmupEntry;
    
    /*
     * 모듈별 No-Mount 초기화
     */
    STL_TRY( smfStartupNoMount( (void**)&gComponentWarmupEntry[SMG_COMPONENT_DATAFILE],
                                (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smpStartupNoMount( (void**)&gComponentWarmupEntry[SMG_COMPONENT_PAGE_ACCESS],
                                (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( smrStartupNoMount( (void**)&gComponentWarmupEntry[SMG_COMPONENT_RECOVERY],
                                (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 3;
    
    STL_TRY( smtStartupNoMount( (void**)&gComponentWarmupEntry[SMG_COMPONENT_TABLESPACE],
                                (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 4;
    
    STL_TRY( smsStartupNoMount( (void**)&gComponentWarmupEntry[SMG_COMPONENT_SEGMENT],
                                (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 5;
    
    STL_TRY( smeStartupNoMount( (void**)&gComponentWarmupEntry[SMG_COMPONENT_RELATION],
                                (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 6;
    
    STL_TRY( smdStartupNoMount( (void**)&gComponentWarmupEntry[SMG_COMPONENT_TABLE],
                                (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 7;
    
    STL_TRY( smnStartupNoMount( (void**)&gComponentWarmupEntry[SMG_COMPONENT_INDEX],
                                (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 8;
    
    STL_TRY( smqStartupNoMount( (void**)&gComponentWarmupEntry[SMG_COMPONENT_SEQUENCE],
                                (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 9;
    
    STL_TRY( smxlStartupNoMount( (void**)&gComponentWarmupEntry[SMG_COMPONENT_TRANSACTION],
                                 (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 10;
    
    STL_TRY( smklStartupNoMount( (void**)&gComponentWarmupEntry[SMG_COMPONENT_LOCK],
                                 (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 11;
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[STARTUP-SM] DATA_STORE_MODE(%d)\n",
                        smxlGetDataStoreMode() )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 11:
            (void) smklBootdown( SML_ENV(aEnv) );
        case 10:
            (void) smxlBootdown( SML_ENV(aEnv) );
        case 9:
            (void) smqBootdown( SML_ENV(aEnv) );
        case 8:
            (void) smnBootdown( SML_ENV(aEnv) );
        case 7:
            (void) smdBootdown( SML_ENV(aEnv) );
        case 6:
            (void) smeBootdown( SML_ENV(aEnv) );
        case 5:
            (void) smsBootdown( SML_ENV(aEnv) );
        case 4:
            (void) smtBootdown( SML_ENV(aEnv) );
        case 3:
            (void) smrBootdown( SML_ENV(aEnv) );
        case 2:
            (void) smpBootdown( SML_ENV(aEnv) );
        case 1:
            (void) smfBootdown( SML_ENV(aEnv) );
        default:
            break;
    }

    gSmWarmupPhase = KNL_STARTUP_PHASE_NONE;
    
    return STL_FAILURE;
}

/**
 * @brief Storage Manager Layer를 Mount 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv )
{
    stlInt32 sState = 0;
    
    STL_DASSERT( KNL_IS_SHARED_ENV( KNL_ENV(aEnv) ) == STL_TRUE );

    gSmgWarmupEntry->mIoSlaveCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_PARALLEL_IO_FACTOR,
                                                                    KNL_ENV(aEnv) );

    STL_TRY( knlCreateDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Mount,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_STORAGE_MANAGER_MOUNT,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  SMG_DATABASE_SHMMEM_CHUNK_SIZE,
                                  SMG_DATABASE_SHMMEM_CHUNK_SIZE,
                                  NULL, /* aMemController */
                                  (knlEnv*)aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smfStartupMount( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( smpStartupMount( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 3;
    
    STL_TRY( smrStartupMount( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 4;
    
    STL_TRY( smxlStartupMount( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 5;
    
    STL_TRY( smklStartupMount( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 6;
    
    STL_TRY( smsStartupMount( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 7;

    STL_TRY( knlAddProcessEvent( SML_EVENT_ESCALATE_WARMUP_PHASE,
                                 NULL,  /* aData */
                                 0,     /* aDataSize */
                                 KNL_BROADCAST_PROCESS_ID,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( smgRegisterDbFiles( SML_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[STARTUP-SM] MOUNT PHASE\n" )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 7:
        case 6:
            (void) smklShutdownDismount( SML_ENV(aEnv) );
        case 5:
            (void) smxlShutdownDismount( SML_ENV(aEnv) );
        case 4:
            (void) smrShutdownDismount( SML_ENV(aEnv) );
        case 3:
        case 2:
            (void) smfShutdownDismount( SML_ENV(aEnv) );
        case 1:
            (void) knlDestroyDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Mount, KNL_ENV(aEnv) );
            KNL_INIT_DYNAMIC_MEMORY( &(gSmgWarmupEntry->mDatabaseMem4Mount) );
        default:
            break;
    }

    gSmWarmupPhase = KNL_STARTUP_PHASE_NO_MOUNT;
    
    return STL_FAILURE;
}

/**
 * @brief Storage Manager Layer를 PREOPEN 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgStartupPreOpen( knlStartupInfo * aStartupInfo,
                             void           * aEnv )
{
    stlInt32 sState = 0;
    
    STL_DASSERT( KNL_IS_SHARED_ENV( KNL_ENV(aEnv) ) == STL_TRUE );

    STL_TRY( smlValidateOpenDatabase( aStartupInfo->mDataAccessMode,
                                      aStartupInfo->mLogOption,
                                      SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    smlSetDataAccessMode( aStartupInfo->mDataAccessMode );
        
    STL_TRY( knlCreateDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_STORAGE_MANAGER_OPEN,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  SMG_DATABASE_SHMMEM_CHUNK_SIZE,
                                  SMG_DATABASE_SHMMEM_CHUNK_SIZE,
                                  NULL, /* aMemController */
                                  (knlEnv*)aEnv )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Layer별 Startup 상태로 전이
     */
    
    STL_TRY( smxlStartupPreOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( smfStartupPreOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 3;
    
    STL_TRY( smpStartupPreOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 4;

    STL_TRY( smfVerifyDatabaseCorruption( aEnv ) == STL_SUCCESS );

    STL_TRY( smrStartupPreOpen( aStartupInfo,
                                SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 5;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[STARTUP-SM] PRE-OPEN PHASE\n" )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 5:
            (void) smrShutdownPostClose( SML_ENV(aEnv) );
        case 4:
            (void) smpShutdownPostClose( SML_ENV(aEnv) );
        case 3:
            (void) smfShutdownPostClose( SML_ENV(aEnv) );
        case 2:
            (void) smxlShutdownPostClose( SML_ENV(aEnv) );
        case 1:
            (void) knlDestroyDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open, KNL_ENV(aEnv) );
            KNL_INIT_DYNAMIC_MEMORY( &(gSmgWarmupEntry->mDatabaseMem4Open) );
        default:
            break;
    }

    gSmWarmupPhase = KNL_STARTUP_PHASE_MOUNT;
    
    return STL_FAILURE;
}

/**
 * @brief Storage Manager Layer를 Service 가능한 상태로 전환시킨다.
 * @param[in] aStartupInfo Startup 시 정보 (Recovery type(restart or media), Tablespace Id)
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgStartupOpen( knlStartupInfo * aStartupInfo,
                          void           * aEnv )
{
    stlInt32 sState = 0;
    
    STL_DASSERT( KNL_IS_SHARED_ENV( KNL_ENV(aEnv) ) == STL_TRUE );
    
    /**
     * Layer별 Startup 상태로 전이
     */

    STL_TRY( smfStartupOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smpStartupOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( smrStartupOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 3;
    
    STL_TRY( smtStartupOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 4;
    
    STL_TRY( smsStartupOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 5;
    
    STL_TRY( smeStartupOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 6;
    
    STL_TRY( smqStartupOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 7;
    
    STL_TRY( smxlStartupOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 8;
    
    STL_TRY( smnStartupOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 9;
    
    
    STL_TRY( knlAddProcessEvent( SML_EVENT_ESCALATE_WARMUP_PHASE,
                                 NULL,  /* aData */
                                 0,     /* aDataSize */
                                 KNL_BROADCAST_PROCESS_ID,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[STARTUP-SM] OPEN PHASE\n" )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 9:
            (void) smnShutdownClose( SML_ENV(aEnv) );
        case 8:
            (void) smxlShutdownClose( SML_ENV(aEnv) );
        case 7:
            (void) smqShutdownClose( SML_ENV(aEnv) );
        case 6:
            (void) smeShutdownClose( SML_ENV(aEnv) );
        case 5:
            (void) smsShutdownClose( SML_ENV(aEnv) );
        case 4:
            (void) smtShutdownClose( SML_ENV(aEnv) );
        case 3:
            (void) smrShutdownClose( SML_ENV(aEnv) );
        case 2:
            (void) smpShutdownClose( SML_ENV(aEnv) );
        case 1:
            (void) smfShutdownClose( SML_ENV(aEnv) );
        default:
            break;
    }

    gSmWarmupPhase = KNL_STARTUP_PHASE_MOUNT;
    
    return STL_FAILURE;
}

/**
 * @brief Storage manager warmup phase를 증가 시킨다.
 * @param[in] aData Media Recovery Info
 * @param[in] aDataSize Media Recovery 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 * @remarks 서버가 NOMOUNT일때 Warmup한 클라이언트는 서버의 PHASE가 올라가면
 *          클라이언트의 PHASE도 같이 올라가야 한다.
 */
stlStatus smgEscalateWarmupPhase( void      * aData,
                                  stlUInt32   aDataSize,
                                  stlBool   * aDone,
                                  void      * aEnv )
{
    *aDone = STL_TRUE;
    
    if( knlGetCurrStartupPhase() != gSmWarmupPhase )
    {
        if( gSmWarmupPhase < KNL_STARTUP_PHASE_OPEN )
        {
            STL_TRY( smrAdjustGlobalVariables( NULL, SML_ENV(aEnv) ) == STL_SUCCESS );
            STL_TRY( smxlAdjustGlobalVariables( SML_ENV(aEnv) ) == STL_SUCCESS );
            STL_TRY( smklAdjustGlobalVariables( SML_ENV(aEnv) ) == STL_SUCCESS );
            gSmWarmupPhase = knlGetCurrStartupPhase();
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief MOUNT phase에서 Media Recovery를 수행한다.
 * @param[in] aData Media Recovery Info
 * @param[in] aDataSize Media Recovery 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smgMediaRecoverEventHandler( void      * aData,
                                       stlUInt32   aDataSize,
                                       stlBool   * aDone,
                                       void      * aEnv )
{
    smlDatafileRecoveryInfo   sDatafileRecoveryInfo;
    stlInt32                  sState = 0;
    stlBool                   sIsFinishedRecovery = STL_TRUE;

    *aDone = STL_FALSE;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    STL_PARAM_VALIDATE( aData != NULL, KNL_ERROR_STACK(aEnv) );

    STL_DASSERT( KNL_IS_SHARED_ENV( KNL_ENV(aEnv) ) == STL_TRUE );

    /**
     * Prepare for Media Recovery
     */
    STL_TRY( smgPrepareMediaRecoveryAtMount( (smlMediaRecoveryInfo *)aData,
                                             aDataSize,
                                             &sDatafileRecoveryInfo,
                                             SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Media Recovery 를 수행한다.
     */
    STL_TRY( smrMediaRecover( &sIsFinishedRecovery,
                              SML_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Media Recovery 완료 후 MOUNT phase로 복귀한다.
     */
    if( sIsFinishedRecovery == STL_TRUE )
    {
        sState = 0;
        STL_TRY( smgFinalizeMediaRecoveryAtMount( SML_ENV( aEnv ) ) == STL_SUCCESS );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ONLY_MOUNT_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_APPROPRIATE_PHASE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      gPhaseString[KNL_STARTUP_PHASE_MOUNT] );
    }

    STL_FINISH;

    /* Media Recovery가 끝났을 때만 exception processing 한다. */
    if( (sIsFinishedRecovery == STL_TRUE) && (sState == 1) )
    {
        (void)smgFinalizeMediaRecoveryAtMount( SML_ENV( aEnv ) );
    }

    /**
     * CONST test 에서 SYSTEM abort 시 error을 사용자 세션에 직접 넣는 경우가
     * 있기 때문에 ErrorStack에 대한 validation은 하지 않는다.
     * @todo Breakpoint SYSTEM abort 시 executor에 error을 넣어야 한다.
     */
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;
    
    return STL_FAILURE;
}

/**
 * @brief 제거된 Datafile을 restore한다.
 * @param[in] aData Media Recovery Info
 * @param[in] aDataSize Media Recovery 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smgRestoreEventHandler( void      * aData,
                                  stlUInt32   aDataSize,
                                  stlBool   * aDone,
                                  void      * aEnv )
{
    smlRestoreInfo         sRestoreInfo;
    stlInt32               sState = 0;

    *aDone = STL_FALSE;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    STL_PARAM_VALIDATE( aData != NULL, KNL_ERROR_STACK(aEnv) );

    stlMemcpy( (void *)&sRestoreInfo, aData, aDataSize );

    STL_DASSERT( KNL_IS_SHARED_ENV( KNL_ENV(aEnv) ) == STL_TRUE );

    STL_TRY( smgStartupPreOpenBeforeMediaRecovery( aEnv ) == STL_SUCCESS );

    STL_TRY( knlCreateDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_STORAGE_MANAGER_OPEN,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  SMG_DATABASE_SHMMEM_CHUNK_SIZE,
                                  SMG_DATABASE_SHMMEM_CHUNK_SIZE,
                                  NULL, /* aMemController */
                                  (knlEnv*)aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[RESTORE] begin (%s, %ld)\n",
                        (sRestoreInfo.mTbsId == SML_INVALID_TBS_ID)? "DATABASE" : "TABLESPACE",
                        sRestoreInfo.mUntilValue )
             == STL_SUCCESS );

    /**
     * Datafile을 validation하고 존재하지 않는 datafile은 생성한다.
     */
    STL_TRY( smfRestoreDatafiles( sRestoreInfo.mTbsId,
                                  SML_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * Restore를 수행하기 위해 transaction, file and page layer를 startup open한다.
     */

    STL_TRY( smxlStartupPreOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smfStartupPreOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( smpStartupPreOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 4;

    /**
     * Backup을 이용하여 GOLDILOCKS_DATA/ directory의 datafile을 merge한다.
     */
    STL_TRY( smfMergeBackup( sRestoreInfo.mTbsId,
                             sRestoreInfo.mUntilType,
                             sRestoreInfo.mUntilValue,
                             SML_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[RESTORE] end\n" )
             == STL_SUCCESS );

    /**
     * Restore 완료 후 transaction, file and page layer를 shutdown close한다.
     */
    sState = 3;
    STL_TRY( smpShutdownPostClose( SML_ENV(aEnv) ) == STL_SUCCESS );

    sState = 2;
    STL_TRY( smfShutdownPostClose( SML_ENV(aEnv) ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( smxlShutdownPostClose( SML_ENV(aEnv) ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlDestroyDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open,
                                   (knlEnv*)aEnv ) == STL_SUCCESS );
    KNL_INIT_DYNAMIC_MEMORY( &(gSmgWarmupEntry->mDatabaseMem4Open) );
    
    STL_TRY( smgShutdownPostCloseAfterMediaRecovery( aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ONLY_MOUNT_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_APPROPRIATE_PHASE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      gPhaseString[KNL_STARTUP_PHASE_MOUNT] );
    }

    STL_FINISH;

    switch( sState )
    {
        case 4:
            (void)smpShutdownClose( SML_ENV(aEnv) );
        case 3:
            (void)smfShutdownClose( SML_ENV(aEnv) );
        case 2:
            (void)smxlShutdownClose( SML_ENV(aEnv) );
        case 1:
            (void) knlDestroyDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open, KNL_ENV(aEnv) );
            KNL_INIT_DYNAMIC_MEMORY( &(gSmgWarmupEntry->mDatabaseMem4Open) );
        default:
            break;
    }

    (void)smgShutdownPostCloseAfterMediaRecovery( aEnv );
    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;
    
    return STL_FAILURE;
}

/**
 * @brief Storage Manager Layer를 Warm-up 단계로 전환시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgWarmup( void * aEnv )
{
    STL_TRY( knlGetEntryPoint( STL_LAYER_STORAGE_MANAGER,
                               (void**)&gStorageManagerWarmupEntry,
                               (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( smgRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    smrRegisterRedoVectors( SMG_COMPONENT_EXTERNAL,
                            gSmgRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_EXTERNAL, 
                           gSmgUndoFuncs );

    knlRegisterEventTable( STL_LAYER_STORAGE_MANAGER, gSmgEventTable );
    
    gComponentWarmupEntry = *gStorageManagerWarmupEntry;

    gSmgWarmupEntry = gComponentWarmupEntry[SMG_COMPONENT_GENERAL];
    
    STL_TRY( smfWarmup( gComponentWarmupEntry[SMG_COMPONENT_DATAFILE],
                        (smlEnv*)aEnv ) == STL_SUCCESS );
    STL_TRY( smpWarmup( gComponentWarmupEntry[SMG_COMPONENT_PAGE_ACCESS],
                        (smlEnv*)aEnv ) == STL_SUCCESS );
    STL_TRY( smrWarmup( gComponentWarmupEntry[SMG_COMPONENT_RECOVERY],
                        (smlEnv*)aEnv ) == STL_SUCCESS );
    STL_TRY( smtWarmup( gComponentWarmupEntry[SMG_COMPONENT_TABLESPACE],
                        (smlEnv*)aEnv ) == STL_SUCCESS );
    STL_TRY( smsWarmup( gComponentWarmupEntry[SMG_COMPONENT_SEGMENT],
                        (smlEnv*)aEnv ) == STL_SUCCESS );
    STL_TRY( smeWarmup( gComponentWarmupEntry[SMG_COMPONENT_RELATION],
                        (smlEnv*)aEnv ) == STL_SUCCESS );
    STL_TRY( smdWarmup( gComponentWarmupEntry[SMG_COMPONENT_TABLE],
                        (smlEnv*)aEnv ) == STL_SUCCESS );
    STL_TRY( smnWarmup( gComponentWarmupEntry[SMG_COMPONENT_INDEX],
                        (smlEnv*)aEnv ) == STL_SUCCESS );
    STL_TRY( smqWarmup( gComponentWarmupEntry[SMG_COMPONENT_SEQUENCE],
                        (smlEnv*)aEnv ) == STL_SUCCESS );
    STL_TRY( smxlWarmup( gComponentWarmupEntry[SMG_COMPONENT_TRANSACTION],
                         (smlEnv*)aEnv ) == STL_SUCCESS );
    STL_TRY( smklWarmup( gComponentWarmupEntry[SMG_COMPONENT_LOCK],
                         (smlEnv*)aEnv ) == STL_SUCCESS );
    
    gSmWarmupPhase = knlGetCurrStartupPhase();
    
    return STL_SUCCESS;

    STL_FINISH;

    knlLogMsgUnsafe( NULL,
                     KNL_ENV( aEnv ),
                     KNL_LOG_LEVEL_FATAL,
                     "[STORAGE MANAGER WARMUP FAIL] %s\n",
                     stlGetLastErrorMessage( KNL_ERROR_STACK( aEnv ) ) );
    
    return STL_FAILURE;
}

/**
 * @brief Storage Manager Layer를 Cool-down 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgCooldown( void * aEnv )
{
    STL_TRY( smklCooldown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smxlCooldown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smqCooldown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smnCooldown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smdCooldown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smeCooldown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smsCooldown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smtCooldown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smrCooldown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smpCooldown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smfCooldown( SML_ENV(aEnv) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Storage Manager Layer를 INIT 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgShutdownInit( knlShutdownInfo * aShutdownInfo,
                           void           * aEnv )
{
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[SHUTDOWN-SM] INIT\n",
                        smxlGetDataStoreMode() )
             == STL_SUCCESS );
    
    STL_TRY( smklBootdown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smxlBootdown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smqBootdown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smnBootdown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smdBootdown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smeBootdown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smsBootdown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smtBootdown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smrBootdown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smpBootdown( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smfBootdown( SML_ENV(aEnv) ) == STL_SUCCESS );

    gSmWarmupPhase = KNL_STARTUP_PHASE_NONE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Storage Manager Layer를 DISMOUNT 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgShutdownDismount( knlShutdownInfo * aShutdownInfo,
                               void           * aEnv )
{
    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    STL_TRY( smklShutdownDismount( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smxlShutdownDismount( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smrShutdownDismount( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smfShutdownDismount( SML_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[SHUTDOWN-SM] DISMOUNT\n",
                        smxlGetDataStoreMode() )
             == STL_SUCCESS );

    if( KNL_DYNAMIC_MEMORY_IS_CREATED( &gSmgWarmupEntry->mDatabaseMem4Mount ) == STL_TRUE )
    {
        STL_TRY( knlDestroyDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Mount,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
        KNL_INIT_DYNAMIC_MEMORY( &(gSmgWarmupEntry->mDatabaseMem4Mount) );
    }
    
    gSmWarmupPhase = KNL_STARTUP_PHASE_NO_MOUNT;

    STL_RAMP( RAMP_SHUTDOWN_ABORT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Storage Manager Layer를 POST_CLOSE 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgShutdownPostClose( knlShutdownInfo * aShutdownInfo,
                                void            * aEnv )
{
    if( aShutdownInfo->mShutdownMode <= KNL_SHUTDOWN_MODE_ABORT )
    {
        STL_TRY( smrDestroyLogMirrorArea( SML_ENV(aEnv) ) == STL_SUCCESS );
        STL_THROW( RAMP_SHUTDOWN_ABORT );
    }

    STL_TRY( smrShutdownPostClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smpShutdownPostClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smfShutdownPostClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[SHUTDOWN-SM] POST CLOSE\n",
                        smxlGetDataStoreMode() )
             == STL_SUCCESS );
    
    if( KNL_DYNAMIC_MEMORY_IS_CREATED( &gSmgWarmupEntry->mDatabaseMem4Open ) == STL_TRUE )
    {
        STL_TRY( knlDestroyDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
        KNL_INIT_DYNAMIC_MEMORY( &(gSmgWarmupEntry->mDatabaseMem4Open) );
    }

    gSmWarmupPhase = KNL_STARTUP_PHASE_MOUNT;

    STL_RAMP( RAMP_SHUTDOWN_ABORT );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Storage Manager Layer를 CLOSE 시킨다.
 * @param[in] aShutdownInfo 종료 시 정보
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgShutdownClose( knlShutdownInfo * aShutdownInfo,
                            void            * aEnv )
{
    smrChkptArg    sArg;

    STL_TRY_THROW( aShutdownInfo->mShutdownMode > KNL_SHUTDOWN_MODE_ABORT,
                   RAMP_SHUTDOWN_ABORT );

    /**
     * SM Layer에서 Shutdown Close를 위한 checkpoint를 수행한다.
     */

    STL_TRY_THROW( aShutdownInfo->mShutdownMode >= KNL_SHUTDOWN_MODE_IMMEDIATE,
                   RAMP_SKIP_CHECKPOINT );
    STL_TRY_THROW( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN,
                   RAMP_SKIP_CHECKPOINT );
    STL_TRY_THROW( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE,
                   RAMP_SKIP_CHECKPOINT );
    STL_TRY_THROW( smfGetServerState() != SML_SERVER_STATE_RECOVERING,
                   RAMP_SKIP_CHECKPOINT );

    if( smxlGetDataStoreMode() >= SML_DATA_STORE_MODE_TDS )
    {
        /* TDS mode에서는 dirty page flush type으로 checkpoint한다. */
        stlMemset( (void *)&sArg, 0x00, STL_SIZEOF(smrChkptArg) );
        sArg.mLogfileSeq = STL_INT64_MAX;
        sArg.mFlushBehavior = SMP_FLUSH_DIRTY_PAGE;

        STL_TRY( knlAddEnvEvent( SML_EVENT_CHECKPOINT,
                                 NULL,            /* aEventMem */
                                 (void*)&sArg,
                                 STL_SIZEOF( smrChkptArg ),
                                 SML_CHECKPOINT_ENV_ID,
                                 KNL_EVENT_WAIT_POLLING,
                                 STL_TRUE,  /* aForceSuccess */
                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }
    else
    {
        /* CDS mode에서는 직접 dirty page flush하고 checkpoint log를 기록한다. */
        STL_TRY( smpFlushPage4Shutdown( aEnv ) == STL_SUCCESS );

        STL_TRY( smrWriteChkptLog( smrGetSystemLsn(),
                                   aEnv ) == STL_SUCCESS );
    }

    smfSetServerState( SML_SERVER_STATE_SHUTDOWN );
    smfSetDataStoreMode( smxlGetDataStoreMode() );

    STL_RAMP( RAMP_SKIP_CHECKPOINT );

    /**
     * Layer별 Startup 상태로 전이
     */
    
    STL_TRY( smnShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smklShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smxlShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smqShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smeShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smsShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smtShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smrShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smpShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( smfShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[SHUTDOWN-SM] CLOSE\n",
                        smxlGetDataStoreMode() )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_SHUTDOWN_ABORT );
    
    return STL_SUCCESS;

    STL_FINISH;

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INTERNAL,
                  NULL,
                  KNL_ERROR_STACK(aEnv),
                  "smgShutdownClose()",
                  -1 );

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus smgRegisterFxTables( void * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gSmgEnvTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmgSystemInfoTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmgSessionEnvTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmgSessionStatTableDesc,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Open Database Level Shared Memory로 부터 메모리를 할당한다.
 * @param[in] aSize 할당 크기
 * @param[out] aAddr 할당받은 주소
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgAllocShmMem4Open( stlSize    aSize,
                               void    ** aAddr,
                               smlEnv   * aEnv )
{
    STL_TRY( knlCacheAlignedAllocDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open,
                                             aSize,
                                             aAddr,
                                             (knlEnv*)aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Open Database Level Shared Memory에서 cache align에 맞게 할당한 메모리를 해제한다.
 * @param[in] aAddr 해제할 메모리 주소
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgFreeShmMem4Open( void   * aAddr,
                              smlEnv * aEnv )
{
    STL_TRY( knlCacheAlignedFreeDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open,
                                            aAddr,
                                            (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mounted Database Level Shared Memory로 부터 메모리를 할당한다.
 * @param[in] aSize 할당 크기
 * @param[out] aAddr 할당받은 주소
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgAllocShmMem4Mount( stlSize    aSize,
                                void    ** aAddr,
                                smlEnv   * aEnv )
{
    STL_TRY( knlCacheAlignedAllocDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Mount,
                                             aSize,
                                             aAddr,
                                             (knlEnv*)aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Mounted Database Level Shared Memory에서 cache align에 맞게 할당한 메모리를 해제한다.
 * @param[in] aAddr 해제할 메모리 주소
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgFreeShmMem4Mount( void   * aAddr,
                               smlEnv * aEnv )
{
    STL_TRY( knlCacheAlignedFreeDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Mount,
                                            aAddr,
                                            (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session dynamic shared memory로부터 메모리를 할당한다.
 * @param[in] aSize 할당 크기
 * @param[out] aAddr 할당받은 주소
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgAllocSessShmMem( stlSize     aSize,
                              void     ** aAddr,
                              smlEnv    * aEnv )
{
    if( KNL_DYNAMIC_MEMORY_IS_CREATED( &SML_SESS_ENV( aEnv )->mDynamicMem ) == STL_FALSE )
    {
        STL_TRY( knlCreateDynamicMem( &SML_SESS_ENV( aEnv )->mDynamicMem,
                                      NULL, /* aParentMem */
                                      KNL_ALLOCATOR_STORAGE_MANAGER_SESSION_DYNAMIC,
                                      KNL_MEM_STORAGE_TYPE_SHM,
                                      SMG_SESSION_DYNAMIC_CHUNK_SIZE,
                                      SMG_SESSION_DYNAMIC_CHUNK_SIZE,
                                      NULL, /* aMemController */
                                      KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    STL_TRY( knlCacheAlignedAllocDynamicMem( &SML_SESS_ENV( aEnv )->mDynamicMem,
                                             aSize,
                                             aAddr,
                                             KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session dynamic shared memory에서 cache align에 맞게 할당한 메모리를 해제한다.
 * @param[in] aAddr 해제할 메모리 주소
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smgFreeSessShmMem( void   * aAddr,
                             smlEnv * aEnv )
{
    STL_DASSERT( KNL_DYNAMIC_MEMORY_IS_CREATED( &SML_SESS_ENV( aEnv )->mDynamicMem ) == STL_TRUE );

    STL_TRY( knlCacheAlignedFreeDynamicMem( &SML_SESS_ENV( aEnv )->mDynamicMem,
                                            aAddr,
                                            (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Checkpoint Environment Identifier를 얻는다.
 * @return Checkpoint Environment Identifier
 */
stlUInt32 smgGetCheckpointEnvId()
{
    return gSmgWarmupEntry->mCheckpointEnvId;
}

/**
 * @brief Log Flusher Environment Identifier를 얻는다.
 * @return Log Flusher Environment Identifier
 */
stlUInt32 smgGetLogFlusherEnvId()
{
    return gSmgWarmupEntry->mLogFlusherEnvId;
}

/**
 * @brief Page Flusher Environment Identifier를 얻는다.
 * @return Page Flusher Environment Identifier
 */
stlUInt32 smgGetPageFlusherEnvId()
{
    return gSmgWarmupEntry->mPageFlusherEnvId;
}

/**
 * @brief Ager Environment Identifier를 얻는다.
 * @return Ager Environment Identifier
 */
stlUInt32 smgGetAgerEnvId()
{
    return gSmgWarmupEntry->mAgerEnvId;
}

/**
 * @brief Ager Environment Identifier를 얻는다.
 * @return Ager Environment Identifier
 */
stlUInt32 smgGetCleanupEnvId()
{
    return gSmgWarmupEntry->mCleanupEnvId;
}

/**
 * @brief Archivelog Environment Identifier를 얻는다.
 * @return Archivelog Environment Identifier
 */
stlUInt32 smgGetArchivelogEnvId()
{
    return gSmgWarmupEntry->mArchivelogEnvId;
}

/**
 * @brief IO Slave Environment Identifier를 얻는다.
 * @param[in] aThreadIdx IO Slave Thread identifier
 * @return IO Slave Environment Identifier
 */
stlUInt32 smgGetIoSlaveEnvId( stlInt64 aThreadIdx )
{
    return gSmgWarmupEntry->mIoSlaveEnvId[aThreadIdx];
}

/**
 * @brief Checkpoint Environment Identifier를 WarmupEntry에 설정한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smgSetCheckpointEnvId( smlEnv * aEnv )
{
    knlGetEnvId( aEnv, &gSmgWarmupEntry->mCheckpointEnvId );
}

/**
 * @brief Log Flusher Environment Identifier를 WarmupEntry에 설정한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smgSetLogFlusherEnvId( smlEnv * aEnv )
{
    knlGetEnvId( aEnv, &gSmgWarmupEntry->mLogFlusherEnvId );
}

/**
 * @brief Page Flusher Environment Identifier를 WarmupEntry에 설정한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smgSetPageFlusherEnvId( smlEnv * aEnv )
{
    knlGetEnvId( aEnv, &gSmgWarmupEntry->mPageFlusherEnvId );
}

/**
 * @brief Ager Environment Identifier를 WarmupEntry에 설정한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smgSetAgerEnvId( smlEnv * aEnv )
{
    knlGetEnvId( aEnv, &gSmgWarmupEntry->mAgerEnvId );
}

/**
 * @brief Cleanup Environment Identifier를 WarmupEntry에 설정한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smgSetCleanupEnvId( smlEnv * aEnv )
{
    knlGetEnvId( aEnv, &gSmgWarmupEntry->mCleanupEnvId );
}

/**
 * @brief Archivelog Environment Identifier를 WarmupEntry에 설정한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smgSetArchivelogEnvId( smlEnv * aEnv )
{
    knlGetEnvId( aEnv, &gSmgWarmupEntry->mArchivelogEnvId );
}

/**
 * @brief IO Slave Environment Identifier를 WarmupEntry에 설정한다.
 * @param[in] aThreadIdx IO Slave Thread identifier
 * @param[in,out] aEnv Environment 구조체
 */
void smgSetIoSlaveEnvId( stlInt64   aThreadIdx,
                         smlEnv   * aEnv )
{
    STL_DASSERT( aThreadIdx < gSmgWarmupEntry->mIoSlaveCount );
    knlGetEnvId( aEnv, &gSmgWarmupEntry->mIoSlaveEnvId[aThreadIdx] );
}

/**
 * @brief Ager의 상태를 얻는다.
 * @return Ager의 상태
 * @see smlAgerState
 */
stlInt16 smgGetAgerState()
{
    return gSmgWarmupEntry->mAgerState;
}

/**
 * @brief Ager의 상태를 설정한다.
 * @param[in] aState 설정할 Ager 상태
 * @see @a aState : smlAgerState
 */
void smgSetAgerState( stlInt16 aState )
{
    gSmgWarmupEntry->mAgerState = aState;
}

/**
 * @brief Storage Manager Layer의 Fatal Handler.
 * @param[in] aCauseString Fatal 유발 원인을 알수 있는 스트링
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 */
void smgFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext )
{
    knlEnv  * sMyEnv = NULL;
    
    sMyEnv = knlGetMyEnv();
    
    STL_TRY_THROW( sMyEnv != NULL, RAMP_FINISH );
    STL_TRY_THROW( knlGetTopLayer(KNL_ENV(sMyEnv))
                   >= STL_LAYER_STORAGE_MANAGER,
                   RAMP_FINISH );
        
    if( KNL_GET_FATAL_HANDLING( sMyEnv, STL_LAYER_STORAGE_MANAGER ) == STL_FALSE )
    {
        /**
         * 무한 Fatal을 방지해야 한다.
         */
        
        KNL_SET_FATAL_HANDLING( sMyEnv,  STL_LAYER_STORAGE_MANAGER );

        /**
         * 지금까지 기록되었던 모든 로그를 디스크로 반영한다.
         */
        if( smxlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS )
        {
            if( (smfGetServerState() == SML_SERVER_STATE_SERVICE) ||
                (smfGetServerState() == SML_SERVER_STATE_RECOVERED) )
            {
                /**
                 * SYSYTEM FATAL 상황에서만 로그를 반영한다.
                 */
                if( KNL_GET_ENTER_CRITICAL_SECTION_COUNT( sMyEnv ) != 0 )
                {
                    (void)smrFlushAllLogs( STL_TRUE, SML_ENV(sMyEnv) );
                }
            }
        }
    }

    STL_RAMP( RAMP_FINISH );
    
    /**
     * Kernel Layer의 Fatal Handler를 호출한다.
     */
    gSmOldFatalHandler( aCauseString, aSigInfo, aContext );
}

/**
 * @brief 주어진 파일이름에 해당하는 파일포인터를 얻는다.
 * @param[out] aNewFile    얻은 파일포인터
 * @param[in]  aFileName   대상 파일이름
 * @param[in]  aFlag       파일 Flag 정보
 * @param[in]  aPerm       파일의 퍼미션 정보
 * @param[out] aEnv         Environment
 * @see aFlag : stlFileOpenFlags
 * @see aPerm : stlFilePermFlags
 */
stlStatus smgOpenFile( stlFile        * aNewFile, 
                       const stlChar  * aFileName, 
                       stlInt32         aFlag, 
                       stlInt32         aPerm,
                       smlEnv         * aEnv )
{
    stlUInt32 sEnvId = 0;

    if( knlIsSharedSessionEnv( KNL_SESS_ENV(aEnv) ) == STL_TRUE )
    {
        knlGetEnvId( aEnv, &sEnvId );

        /**
         * gmaster main thread는 처리하지 않는다.
         */
        
        if( sEnvId > 0 )
        {
            /**
             * 이미 열려 있는 파일을 가지고 있는 경우이거나
             * 이미 Non-Cancellable Section에 진입한 상태에서는 기다리지 않는다.
             */
    
            if( (knlHasOpenedFile( KNL_SESS_ENV(aEnv) ) == STL_FALSE) &&
                (KNL_GET_ENTER_NON_CANCELLABLE_SECTION_COUNT(aEnv) == 0) )
            {
                while( knlAllowDiskIo( KNL_ENV(aEnv) ) == STL_FALSE )
                {
                    stlSleep( 10000 );
                }
            }
        }
    }

    STL_TRY( stlOpenFile( aNewFile,
                          aFileName,
                          aFlag,
                          aPerm,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv );
    knlIncOpenFileCount( KNL_SESS_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                       
/**
 * @brief 열린 파일포인터를 닫는다.
 * @param[in]  aFile   닫을 파일포인터
 * @param[out] aEnv    Environment
 */
stlStatus smgCloseFile( stlFile * aFile,
                        smlEnv  * aEnv )
{
    knlDecOpenFileCount( KNL_SESS_ENV(aEnv) );

    STL_TRY( stlCloseFile( aFile,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief MOUNT phase에서 Media Recovery 수행을 위해 system을 PREOPEN 상태로 전환시킨다.
 * @param[out] aEnv Environment
 */
stlStatus smgStartupPreOpenBeforeMediaRecovery( void  * aEnv )
{
    knlStartupInfo   sStartupInfo;
    stlInt32         i;

    KNL_INIT_STARTUP_ARG( &sStartupInfo );
    sStartupInfo.mDataAccessMode = SML_DATA_ACCESS_MODE_READ_WRITE;
    sStartupInfo.mLogOption      = SML_STARTUP_LOG_OPTION_NORESETLOGS;
    
    for( i = 0; i < STL_LAYER_STORAGE_MANAGER; i++ )
    {
        if( gSmgStartupLayer[i] == NULL )
        {
            continue;
        }

        if( gSmgStartupLayer[i][KNL_STARTUP_PHASE_PREOPEN] == NULL )
        {
            continue;
        }

        STL_TRY( gSmgStartupLayer[i][KNL_STARTUP_PHASE_PREOPEN]( &sStartupInfo,
                                                                 aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief MOUNT phase에서 Media Recovery 완료를 위해 system을 MOUNT 상태로 전환시킨다.
 * @param[out] aEnv Environment
 */
stlStatus smgShutdownPostCloseAfterMediaRecovery( void  * aEnv )
{
    knlShutdownInfo   sShutdownInfo;
    stlInt32          i;

    KNL_INIT_SHUTDOWN_INFO( &sShutdownInfo );
    sShutdownInfo.mShutdownMode  = KNL_SHUTDOWN_MODE_NORMAL;

    for( i = (STL_LAYER_STORAGE_MANAGER - 1); i >= 0; i-- )
    {
        if( gSmgShutdownLayer[i] == NULL )
        {
            continue;
        }

        if( gSmgShutdownLayer[i][KNL_SHUTDOWN_PHASE_POSTCLOSE] == NULL )
        {
            continue;
        }

        STL_TRY( gSmgShutdownLayer[i][KNL_SHUTDOWN_PHASE_POSTCLOSE]( &sShutdownInfo,
                                                                     aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief MOUNT phase에서 media recovery를 수행하기 위해 prepare한다.
 * @param[in] aRecoveryInfo Media Recovery Info
 * @param[in] aDataSize Media Recovery 정보 크기
 * @param[out] aDatafileRecoveryInfo Datafile Media Recovery Info
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smgPrepareMediaRecoveryAtMount( smlMediaRecoveryInfo    * aRecoveryInfo,
                                          stlUInt32                 aDataSize,
                                          smlDatafileRecoveryInfo * aDatafileRecoveryInfo,
                                          smlEnv                  * aEnv )
{
    smrMediaRecoveryInfo    * sMediaRecoveryInfo;
    stlInt32                  sState = 0;

    /* System의 Media Recovery 정보를 가져온다. */
    sMediaRecoveryInfo = (smrMediaRecoveryInfo *)smrGetDatabaseMediaRecoveryInfo();

    if( sMediaRecoveryInfo != NULL )
    {
        /* Media recovery가 진행 중이면 불완전 복구가 진행 중이다. */
        STL_TRY_THROW( smrProceedIncompleteMediaRecovery(aEnv) == STL_TRUE,
                       RAMP_ERR_INTERNAL );

        /* Logfile type을 system의 media recovery 정보에 설정한다. */
        smrSetIncompleteRecoveryCondition( aRecoveryInfo->mImrCondition, aEnv );

        /* 특정 logfile을 이용하여 복구하는 경우 logfile이름을 media recovery 정보에 복사한다. */
        if( aRecoveryInfo->mImrCondition == SML_IMR_CONDITION_MANUAL )
        {
            stlMemset( sMediaRecoveryInfo->mLogfileName, 0x00, STL_MAX_FILE_PATH_LENGTH );
            stlMemcpy( sMediaRecoveryInfo->mLogfileName,
                       (void *)aRecoveryInfo + STL_SIZEOF(smlMediaRecoveryInfo),
                       aDataSize - STL_SIZEOF(smlMediaRecoveryInfo) );
        }

        /**
         * Session Env에 Media Recovery Info를 연결한다.
         */
        SMR_SET_MEDIA_RECOVERY_INFO( aEnv, sMediaRecoveryInfo );

        /* 불완전 복구가 이미 진행중이기 때문에 초기는 skip한다. */
        STL_THROW( RAMP_SKIP_PREPARE );
    }

    STL_TRY_THROW( smrProceedIncompleteMediaRecovery(aEnv) == STL_FALSE,
                   RAMP_ERR_INTERNAL );

    /* 복구가 진행 중이지 않다면 복구를 위한 메모리 생성 및 초기 값들을 설정한다. */
    STL_TRY( smgStartupPreOpenBeforeMediaRecovery( aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlCreateDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_STORAGE_MANAGER_OPEN,
                                  KNL_MEM_STORAGE_TYPE_SHM,
                                  SMG_DATABASE_SHMMEM_CHUNK_SIZE,
                                  SMG_DATABASE_SHMMEM_CHUNK_SIZE,
                                  NULL, /* aMemController */
                                  (knlEnv*)aEnv )
             == STL_SUCCESS );
    sState = 2;

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smrMediaRecoveryInfo ),
                                  (void**)&sMediaRecoveryInfo,
                                  aEnv ) == STL_SUCCESS );
    sState = 3;

    /**
     * Session Env에 Media Recovery Info를 연결한다.
     */
    SMR_SET_MEDIA_RECOVERY_INFO( aEnv, sMediaRecoveryInfo );

    smrInitRecoveryContextAtMount( aRecoveryInfo, aEnv );
    sState = 4;

    STL_TRY( smfStartupPreOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 5;

    STL_TRY( smpStartupPreOpen( SML_ENV(aEnv) ) == STL_SUCCESS );
    sState = 6;

    if( aRecoveryInfo->mObjectType == SML_RECOVERY_OBJECT_TYPE_DATAFILE )
    {
        stlMemcpy( (void *)&(aDatafileRecoveryInfo->mCommonInfo),
                   aRecoveryInfo,
                   STL_SIZEOF(smlMediaRecoveryInfo) );

        aDatafileRecoveryInfo->mDatafileCount =
            ((smlDatafileRecoveryInfo *)aRecoveryInfo)->mDatafileCount;

        aDatafileRecoveryInfo->mDatafileInfo =
            SML_GET_DATAFILE_RECOVERY_DATAFILE_OFFSET( (void *)aRecoveryInfo );

        STL_TRY( smrCreateMediaRecoveryDatafileInfo( aDatafileRecoveryInfo,
                                                     sMediaRecoveryInfo,
                                                     aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smfVerifyDatabaseCorruption( aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smrSetRecoveryPhaseAtMount( SML_RECOVERY_AT_MOUNT_RECOVERY_BEGIN,
                                         aEnv )
             == STL_SUCCESS );
    sState = 7;

    STL_RAMP( RAMP_SKIP_PREPARE );

    /**
     * SMR Warmup Entry에 media recovery 정보를 연결한다.
     */
    smrSetDatabaseMediaRecoveryInfo( (void *)sMediaRecoveryInfo );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- smgPrepareMediaRecoveryAtMount()",
                      KNL_ERROR_STACK(aEnv) );

        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 7:
            (void) smrSetRecoveryPhaseAtMount( SML_RECOVERY_AT_MOUNT_RECOVERY_END,
                                               aEnv );
        case 6:
            (void) smpShutdownClose( SML_ENV(aEnv) );
        case 5:
            (void) smfShutdownClose( SML_ENV(aEnv) );
        case 4:
            (void) smrFiniRecoveryContextAtMount( SML_ENV(aEnv) );
        case 3:
            (void) smgFreeShmMem4Open( (void *)sMediaRecoveryInfo,
                                      aEnv );
        case 2:
            (void) knlDestroyDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open,
                                         KNL_ENV(aEnv) );
            KNL_INIT_DYNAMIC_MEMORY( &(gSmgWarmupEntry->mDatabaseMem4Open) );

            SMR_SET_MEDIA_RECOVERY_INFO( aEnv, NULL );
            smrSetDatabaseMediaRecoveryInfo( NULL );
        case 1:
            (void) smgShutdownPostCloseAfterMediaRecovery( aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief MOUNT phase에서 media recovery를 수행을 완료한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smgFinalizeMediaRecoveryAtMount( smlEnv  * aEnv )
{
    stlInt32   sState = 7;

    sState = 6;
    STL_TRY( smrSetRecoveryPhaseAtMount( SML_RECOVERY_AT_MOUNT_RECOVERY_END,
                                         aEnv )
             == STL_SUCCESS );

    sState = 5;
    STL_TRY( smpShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );

    sState = 4;
    STL_TRY( smfShutdownClose( SML_ENV(aEnv) ) == STL_SUCCESS );

    sState = 3;
    STL_TRY( smgShutdownPostCloseAfterMediaRecovery( aEnv ) == STL_SUCCESS );

    sState = 2;
    STL_TRY( smrFiniRecoveryContextAtMount( SML_ENV(aEnv) ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( smgFreeShmMem4Open( smrGetDatabaseMediaRecoveryInfo(),
                                 aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlDestroyDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open,
                                   (knlEnv*)aEnv ) == STL_SUCCESS );
    KNL_INIT_DYNAMIC_MEMORY( &(gSmgWarmupEntry->mDatabaseMem4Open) );

    SMR_SET_MEDIA_RECOVERY_INFO( aEnv, NULL );
    smrSetDatabaseMediaRecoveryInfo( NULL );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 7:
            (void) smrSetRecoveryPhaseAtMount( SML_RECOVERY_AT_MOUNT_RECOVERY_END,
                                               aEnv );
        case 6:
            (void) smpShutdownClose( SML_ENV(aEnv) );
        case 5:
            (void) smfShutdownClose( SML_ENV(aEnv) );
        case 4:
            (void) smgShutdownPostCloseAfterMediaRecovery( aEnv );
        case 3:
            (void) smrFiniRecoveryContextAtMount( SML_ENV(aEnv) );
        case 2:
            (void) smgFreeShmMem4Open( smrGetDatabaseMediaRecoveryInfo(),
                                       aEnv );
        case 1:
            (void) knlDestroyDynamicMem( &gSmgWarmupEntry->mDatabaseMem4Open,
                                         KNL_ENV(aEnv) );
            KNL_INIT_DYNAMIC_MEMORY( &(gSmgWarmupEntry->mDatabaseMem4Open) );

            SMR_SET_MEDIA_RECOVERY_INFO( aEnv, NULL );
            smrSetDatabaseMediaRecoveryInfo( NULL );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smgPrepareMediaRecoveryAtOpen( smlRecoveryObjectType   aObjectType,
                                         smlTbsId                aTbsId,
                                         knlRegionMark         * aMemMark,
                                         smlEnv                * aEnv )
{
    smrMediaRecoveryInfo  * sMediaRecoveryInfo;
    stlInt32                sState = 0;

    KNL_INIT_REGION_MARK(aMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               aMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                STL_SIZEOF( smrMediaRecoveryInfo ),
                                (void**)&sMediaRecoveryInfo,
                                (knlEnv*)aEnv ) == STL_SUCCESS );

    /**
     * Session Env에 Media Recovery Info를 연결한다.
     */
    SMR_SET_MEDIA_RECOVERY_INFO( aEnv, sMediaRecoveryInfo );

    STL_TRY( smrInitRecoveryContextAtOpen( aObjectType,
                                           aTbsId,
                                           aEnv ) == STL_SUCCESS );
    sState = 2;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smrFiniRecoveryContextAtOpen( aEnv );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            aMemMark,
                                            STL_TRUE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }

    SMR_SET_MEDIA_RECOVERY_INFO( aEnv, NULL );

    return STL_FAILURE;
}

stlStatus smgFinalizeMediaRecoveryAtOpen( knlRegionMark  * aMemMark,
                                          smlEnv         * aEnv )
{
    stlInt32    sState = 2;

    sState = 1;
    STL_TRY( smrFiniRecoveryContextAtOpen( aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       aMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    SMR_SET_MEDIA_RECOVERY_INFO( aEnv, NULL );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smrFiniRecoveryContextAtOpen( aEnv );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            aMemMark,
                                            STL_TRUE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }

    SMR_SET_MEDIA_RECOVERY_INFO( aEnv, NULL );

    return STL_FAILURE;
}

/**
 * @brief SM Layer에서 사용되는 database file을 register한다.
 * @param[out] aEnv Environment 구조체
 */
stlStatus smgRegisterDbFiles( smlEnv * aEnv )
{
    /* control files, datafiles */
    STL_TRY( smfRegisterDbFiles( aEnv ) == STL_SUCCESS );

    /* log files */
    STL_TRY( smrRegisterDbFiles( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
