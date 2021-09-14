/*******************************************************************************
 * smlGeneral.c
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
#include <smg.h>
#include <smr.h>
#include <sms.h>
#include <smt.h>
#include <smf.h>
#include <sme.h>
#include <smd.h>
#include <smxl.h>
#include <smkl.h>
#include <smlGeneral.h>

/**
 * @file smlGeneral.c
 * @brief Storage Manager Layer Startup Routines
 */

/**
 * @addtogroup smlGeneral
 * @{
 */

extern stlErrorRecord   gStorageManagerErrorTable[SML_MAX_ERROR + 1];
extern stlFatalHandler  gSmOldFatalHandler;
extern smdWarmupEntry * gSmdWarmupEntry;

knlStartupFunc gStartupSM[KNL_STARTUP_PHASE_MAX] =
{
    NULL,
    smgStartupNoMount,
    smgStartupMount,
    smgStartupPreOpen,
    smgStartupOpen
};

knlWarmupFunc gWarmupSM = smgWarmup;

knlShutdownFunc gShutdownSM[KNL_SHUTDOWN_PHASE_MAX] =
{
    NULL,
    smgShutdownInit,
    smgShutdownDismount,
    smgShutdownPostClose,
    smgShutdownClose
};

knlCooldownFunc    gCooldownSM = smgCooldown;

/**
 * @brief StorageManager layer를 초기화한다.
 * @remark 프로세스당 한번 호출되어야 한다.
 */
stlStatus smlInitialize()
{
    if( stlGetProcessInitialized( STL_LAYER_STORAGE_MANAGER ) == STL_FALSE )
    {
        /**
         * 하위 Layer 초기화
         */

        STL_TRY( sclInitialize() == STL_SUCCESS );

        /**
         * FATAL Handler 등록
         */
        STL_TRY( stlHookFatalHandler( smgFatalHandler,
                                      &gSmOldFatalHandler )
                 == STL_SUCCESS );
    
        /**
         * 에러 테이블 등록
         */
    
        stlRegisterErrorTable( STL_ERROR_MODULE_STORAGE_MANAGER,
                               gStorageManagerErrorTable );
    
        stlSetProcessInitialized( STL_LAYER_STORAGE_MANAGER );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Envrionment를 초기화 한다.
 * @param[out] aEnv     초기화될 Environment 포인터
 * @param[in]  aEnvType Env 종류
 */
stlStatus smlInitializeEnv( smlEnv     * aEnv,
                            knlEnvType   aEnvType )
{
    stlInt32  sState = 0;

    STL_TRY( sclInitializeEnv( SCL_ENV(aEnv), aEnvType ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlCreateRegionMem( &aEnv->mOperationHeapMem,
                                 KNL_ALLOCATOR_STORAGE_MANAGER_REGION_OPERATION,
                                 NULL,  /* aParentMem */
                                 KNL_MEM_STORAGE_TYPE_HEAP,
                                 SMG_OPERATION_HEAPMEM_CHUNK_SIZE,
                                 SMG_OPERATION_HEAPMEM_CHUNK_SIZE,
                                 (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( knlCreateRegionMem( &aEnv->mLogHeapMem,
                                 KNL_ALLOCATOR_STORAGE_MANAGER_LOG_OPERATION,
                                 NULL,  /* aParentMem */
                                 KNL_MEM_STORAGE_TYPE_HEAP,
                                 SMR_MAX_LOGPIECE_SIZE * 3,
                                 SMR_MAX_LOGPIECE_SIZE,
                                 (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 3;

    KNL_INIT_DYNAMIC_MEMORY( &(aEnv->mDynamicHeapMem) );

    STL_TRY( knlCreateDynamicMem( &aEnv->mDynamicHeapMem,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_STORAGE_MANAGER_DYNAMIC_OPERATION,
                                  KNL_MEM_STORAGE_TYPE_HEAP,
                                  SMG_OPERATION_HEAPMEM_CHUNK_SIZE,
                                  SMG_OPERATION_HEAPMEM_CHUNK_SIZE,
                                  NULL, /* aMemController*/
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 4;
    
    stlMemset( aEnv->mExclMiniTransStack, 0x00, SML_MAX_MINITRANS_COUNT );
    aEnv->mMiniTransStackTop = -1;
    
    knlSetTopLayer( STL_LAYER_STORAGE_MANAGER, KNL_ENV( aEnv ) );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 4:
            (void) knlDestroyDynamicMem( &aEnv->mDynamicHeapMem, KNL_ENV(aEnv) );
        case 3:
            (void) knlDestroyRegionMem( &aEnv->mLogHeapMem, KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyRegionMem( &aEnv->mOperationHeapMem, KNL_ENV(aEnv) );
        case 1:
            (void) sclFinalizeEnv( SCL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief Storage Manager 진입을 선언한다.
 * @param[in] aEnv Environment Pointer
 */
stlStatus smlEnterStorageManager( smlEnv * aEnv )
{
    stlInt32  sState = 0;
    stlInt32  sSemValue;

    sState = 0;
    STL_TRY_THROW( aEnv->mMiniTransStackTop == -1, RAMP_ERR_INTERNAL );
    
    sState = 1;
    STL_TRY_THROW( SML_SESS_ENV(aEnv)->mFixCount == 0, RAMP_ERR_INTERNAL );
    
    sState = 2;
    aEnv->mCurLatchCount = KNL_ENV(aEnv)->mLatchCount;

    if( KNL_IS_SHARED_ENV( aEnv ) == STL_TRUE )
    {
        STL_TRY( stlGetSemaphoreValue( &(KNL_ENV(aEnv)->mWaitItem.mSem),
                                       &sSemValue,
                                       KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        sState = 3;
        STL_TRY_THROW( sSemValue == 0, RAMP_ERR_INTERNAL );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      "smlEnterStorageManager()",
                      sState );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Storage Manager 진입을 선언한다.
 * @param[in] aValidateErrorStack  Validate Error Stack
 * @param[in] aEnv                 Environment Pointer
 */
stlStatus smlLeaveStorageManager( stlBool   aValidateErrorStack,
                                  smlEnv  * aEnv )
{
    stlInt32  sState = 0;
    stlInt32  sSemValue;

    STL_TRY_THROW( aEnv->mMiniTransStackTop == -1, RAMP_ERR_INTERNAL );
    sState = 0;
    
    sState = 1;
    STL_TRY_THROW( SML_SESS_ENV(aEnv)->mFixCount == 0, RAMP_ERR_INTERNAL );
    
    sState = 2;
    STL_TRY_THROW( KNL_SESS_ENV(aEnv)->mOpenFileCount == 0, RAMP_ERR_INTERNAL );
    
    sState = 3;
    STL_TRY_THROW( aEnv->mCurLatchCount == KNL_ENV(aEnv)->mLatchCount, RAMP_ERR_INTERNAL );

    if( (knlGetCurrStartupPhase() > KNL_STARTUP_PHASE_NONE) &&
        (knlGetCurrStartupPhase() < KNL_STARTUP_PHASE_OPEN) )
    {
        sState = 4;
        STL_TRY_THROW( STL_RING_IS_EMPTY( &SML_SESS_ENV(aEnv)->mPendOp ) == STL_TRUE, RAMP_ERR_INTERNAL );
    }

    if( KNL_IS_SHARED_ENV( aEnv ) == STL_TRUE )
    {
        STL_TRY( stlGetSemaphoreValue( &(KNL_ENV(aEnv)->mWaitItem.mSem),
                                       &sSemValue,
                                       KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        sState = 5;
        STL_TRY_THROW( sSemValue == 0, RAMP_ERR_INTERNAL );
    }

    if( aValidateErrorStack == STL_TRUE )
    {
        sState = 6;
        STL_TRY_THROW( stlGetErrorStackDepth( KNL_ERROR_STACK(aEnv) ) > 0, RAMP_ERR_INTERNAL );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      "smlLeaveStorageManager()",
                      sState );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 
 * @param[out] aTargetSessEnv
 * @param[in]  aSessEnv 
 */
stlStatus smlHandoverSession( smlSessionEnv * aTargetSessEnv,
                              smlSessionEnv * aSessEnv )
{
    smgPendOp * sFirstPendOp;
    smgPendOp * sLastPendOp;
    
    sFirstPendOp = STL_RING_GET_FIRST_DATA( &aSessEnv->mPendOp );
    sLastPendOp = STL_RING_GET_LAST_DATA( &aSessEnv->mPendOp );

    if( STL_RING_IS_EMPTY( &aSessEnv->mPendOp ) == STL_FALSE )
    {
        STL_RING_UNSPLICE( &aSessEnv->mPendOp,
                           sFirstPendOp,
                           sLastPendOp );
    
        STL_RING_SPLICE_FIRST( &aTargetSessEnv->mPendOp,
                               sFirstPendOp,
                               sLastPendOp );
    }

    aTargetSessEnv->mInstTableList = aSessEnv->mInstTableList;
    aSessEnv->mInstTableList = NULL;

    return STL_SUCCESS;
}


/**
 * @brief Envrionment를 종료시킨다.
 * @param[in] aEnv 종료할 Environment 포인터
 */
stlStatus smlFinalizeEnv( smlEnv * aEnv )
{
    STL_TRY( knlDestroyRegionMem( &aEnv->mLogHeapMem,
                                  (knlEnv*)aEnv ) == STL_SUCCESS );
    KNL_INIT_REGION_MEMORY( &aEnv->mLogHeapMem );

    STL_TRY( knlDestroyRegionMem( &aEnv->mOperationHeapMem,
                                  (knlEnv*)aEnv ) == STL_SUCCESS );
    KNL_INIT_REGION_MEMORY( &aEnv->mOperationHeapMem );

    knlValidateDynamicMem( &aEnv->mDynamicHeapMem, KNL_ENV(aEnv) );
    STL_TRY( knlDestroyDynamicMem( &aEnv->mDynamicHeapMem,
                                   KNL_ENV(aEnv) ) == STL_SUCCESS );
    KNL_INIT_DYNAMIC_MEMORY( &aEnv->mDynamicHeapMem );

    STL_TRY( sclFinalizeEnv( (sclEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session Envrionment를 초기화 한다.
 * @param[out]    aSessionEnv  초기화될 Session Environment 포인터
 * @param[in]     aConnectionType Session Connection Type
 * @param[in]     aSessionType Session Type
 * @param[in]     aSessEnvType Session Envrionment Type
 * @param[in,out] aEnv         Environment 포인터
 */
stlStatus smlInitializeSessionEnv( smlSessionEnv     * aSessionEnv,
                                   knlConnectionType   aConnectionType,
                                   knlSessionType      aSessionType,
                                   knlSessEnvType      aSessEnvType,
                                   smlEnv            * aEnv )
{
    stlUInt32 sMemType;
    stlInt32  sState = 0;
    stlInt32  sIdxStmt = 0;
    stlInt32  sIdxStmtMem = 0;

    STL_TRY( sclInitializeSessionEnv( (sclSessionEnv*)aSessionEnv,
                                      aConnectionType,
                                      aSessionType,
                                      aSessEnvType,
                                      SCL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    aSessionEnv->mTransId = SML_INVALID_TRANSID;
    aSessionEnv->mStatement[0] = NULL;
    aSessionEnv->mStatement[1] = NULL;
    aSessionEnv->mStatement[2] = NULL;
    STL_RING_INIT_HEADLINK( &aSessionEnv->mPendOp,
                            STL_OFFSETOF( smgPendOp, mOpLink ) );
    STL_RING_INIT_HEADLINK( &aSessionEnv->mReadOnlyStmt,
                            STL_OFFSETOF( smlStatement, mStmtLink ) );
    STL_RING_INIT_HEADLINK( &aSessionEnv->mUpdatableStmt,
                            STL_OFFSETOF( smlStatement, mStmtLink ) );

    /**
     * 가능한 Share Memory 사용을 줄이기 위해서 Dedicated Session이라면
     * Heap Memory를 사용한다.
     */
    if( aSessionType == KNL_SESSION_TYPE_DEDICATE )
    {
        sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
    }
    else
    {
        if( aSessEnvType == KNL_SESS_ENV_HEAP )
        {
            sMemType = KNL_MEM_STORAGE_TYPE_HEAP;
        }
        else
        {
            sMemType = KNL_MEM_STORAGE_TYPE_SHM;
        }
    }

    STL_TRY( knlCreateRegionMem( &aSessionEnv->mSessionMem,
                                 KNL_ALLOCATOR_STORAGE_MANAGER_SESSION_REGION,
                                 NULL,  /* aParentMem */
                                 sMemType,
                                 SMG_SESSION_MEM_CHUNK_SIZE,
                                 SMG_SESSION_MEM_CHUNK_SIZE,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( smeCreateRelHint( aSessionEnv, aEnv ) == STL_SUCCESS );
    STL_TRY( smsCreateSegHint( aSessionEnv, aEnv ) == STL_SUCCESS );
    STL_TRY( smtCreateTbsHint( aSessionEnv, aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlCreateDynamicMem( &aSessionEnv->mStatementMem,
                                  NULL, /* aParentMem */
                                  KNL_ALLOCATOR_STORAGE_MANAGER_PARENT_STATEMENT,
                                  sMemType,
                                  SMG_PARENT_STATEMENT_MEM_CHUNK_SIZE,
                                  SMG_PARENT_STATEMENT_MEM_CHUNK_SIZE,
                                  NULL,  /* aController */
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 3;

    KNL_INIT_DYNAMIC_MEMORY( &aSessionEnv->mDynamicMem );

    for( sIdxStmt = 0; sIdxStmt < SML_CACHED_STMT_MAX_COUNT; sIdxStmt++ )
    {
        STL_TRY( knlAllocDynamicMem( &aSessionEnv->mStatementMem,
                                     STL_SIZEOF( smlStatement ),
                                     (void**)&aSessionEnv->mStatement[sIdxStmt],
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        aSessionEnv->mStatement[sIdxStmt]->mIsUsing   = STL_FALSE;
        aSessionEnv->mStatement[sIdxStmt]->mIsCached  = STL_TRUE;
        sState = 4;
    }

    for( sIdxStmtMem = 0; sIdxStmtMem < SML_CACHED_STMT_MAX_COUNT; sIdxStmtMem++ )
    {
        STL_TRY( knlCreateRegionMem( &aSessionEnv->mStatement[sIdxStmtMem]->mStatementMem,
                                     KNL_ALLOCATOR_STORAGE_MANAGER_STATEMENT,
                                     &aSessionEnv->mStatementMem,
                                     KNL_DYNAMIC_MEMORY_TYPE( &aSessionEnv->mStatementMem ),
                                     SMG_STATEMENT_MEM_CHUNK_SIZE,
                                     SMG_STATEMENT_MEM_CHUNK_SIZE,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 5;
    }

#ifdef STL_DEBUG
    aSessionEnv->mFixCount = 0;
#endif
    aSessionEnv->mSegmentRetryCount = 0;
    aSessionEnv->mRelationRetryCount = 0;
    aSessionEnv->mVersionConflictCount = 0;
    aSessionEnv->mMinSnapshotStmtViewScn = SML_INFINITE_SCN;
    aSessionEnv->mMinStmtViewScn = SML_INFINITE_SCN;
    aSessionEnv->mMinSnapshotStmtBeginTime = STL_INVALID_STL_TIME;
    aSessionEnv->mTransViewScn = SML_INFINITE_SCN;
    aSessionEnv->mDisabledViewScn = SML_INFINITE_SCN;
    aSessionEnv->mMinTransViewScn = SML_INFINITE_SCN;
    aSessionEnv->mInstTableList = NULL;
    aSessionEnv->mMediaRecoveryInfo = NULL;

    STL_TRY( knlCreateStaticHash( &aSessionEnv->mSeqHash,
                                  SMQ_SESSION_SEQUENCE_BUCKET_COUNT,
                                  STL_OFFSETOF( smqSessSeqItem, mLink ),
                                  STL_OFFSETOF( smqSessSeqItem, mSeqKey ),
                                  KNL_STATICHASH_MODE_NOLATCH,
                                  STL_FALSE,
                                  &aSessionEnv->mSessionMem,
                                  KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    knlSetSessionTopLayer( (knlSessionEnv*)aSessionEnv, STL_LAYER_STORAGE_MANAGER );
    
    STL_TRY( knlSetInitializedFlag( (knlSessionEnv*)aSessionEnv,
                                    STL_LAYER_STORAGE_MANAGER,
                                    KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 5:
            sIdxStmtMem = (sIdxStmtMem == SML_CACHED_STMT_MAX_COUNT ) ?
                (SML_CACHED_STMT_MAX_COUNT - 1) : sIdxStmtMem;
            
            for( ; sIdxStmtMem >= 0; sIdxStmtMem-- )
            {
                (void) knlDestroyRegionMem( &aSessionEnv->mStatement[sIdxStmtMem]->mStatementMem,
                                            KNL_ENV(aEnv) );
            }
        case 4:
            sIdxStmt = (sIdxStmt == SML_CACHED_STMT_MAX_COUNT ) ?
                (SML_CACHED_STMT_MAX_COUNT - 1) : sIdxStmt;
            
            for( ; sIdxStmt >= 0; sIdxStmt-- )
            {
                (void) knlFreeDynamicMem( &aSessionEnv->mStatementMem,
                                          aSessionEnv->mStatement[sIdxStmt],
                                          KNL_ENV(aEnv) );
            }
        case 3:
            (void) knlDestroyDynamicMem( &aSessionEnv->mStatementMem, KNL_ENV(aEnv) );
        case 2:
            (void) knlDestroyRegionMem( &aSessionEnv->mSessionMem, KNL_ENV(aEnv) );
        case 1:
            (void) sclFinalizeSessionEnv( (sclSessionEnv*)aSessionEnv, SCL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Session Envrionment를 종료 한다.
 * @param[out]    aSessionEnv  Session Environment 포인터
 * @param[in,out] aEnv         Environment 포인터
 */
stlStatus smlFinalizeSessionEnv( smlSessionEnv * aSessionEnv,
                                 smlEnv        * aEnv )
{
    stlInt32 i;

    smlAddSessStat2SystemStat( aSessionEnv );

    /**
     * check instant relation leak
     */
    STL_DASSERT( smdGetActiveInstantRelationCount( aSessionEnv ) == 0 );

    for( i = SML_CACHED_STMT_MAX_COUNT - 1; i >= 0; i-- )
    {
        STL_TRY( knlDestroyRegionMem( &aSessionEnv->mStatement[i]->mStatementMem,
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        KNL_INIT_REGION_MEMORY( &aSessionEnv->mStatement[i]->mStatementMem );
    }
    
    for( i = SML_CACHED_STMT_MAX_COUNT - 1; i >= 0; i-- )
    {
        STL_TRY( knlFreeDynamicMem( &aSessionEnv->mStatementMem,
                                    aSessionEnv->mStatement[i],
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        aSessionEnv->mStatement[i] = NULL;
    }
    
    if( KNL_DYNAMIC_MEMORY_IS_CREATED( &aSessionEnv->mDynamicMem ) == STL_TRUE )
    {
        knlValidateDynamicMem( &aSessionEnv->mDynamicMem, KNL_ENV(aEnv) );
        STL_TRY( knlDestroyDynamicMem( &aSessionEnv->mDynamicMem,
                                       KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        KNL_INIT_DYNAMIC_MEMORY( &aSessionEnv->mDynamicMem );
    }

    knlValidateDynamicMem( &aSessionEnv->mStatementMem, KNL_ENV(aEnv) );
    STL_TRY( knlDestroyDynamicMem( &aSessionEnv->mStatementMem,
                                   KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    KNL_INIT_DYNAMIC_MEMORY( &aSessionEnv->mStatementMem );

    STL_TRY( knlDestroyRegionMem( &aSessionEnv->mSessionMem,
                                  KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    KNL_INIT_REGION_MEMORY( &aSessionEnv->mSessionMem );

    STL_DASSERT( STL_RING_IS_EMPTY( &aSessionEnv->mPendOp ) == STL_TRUE );
    STL_RING_INIT_HEADLINK( &aSessionEnv->mPendOp,
                            STL_OFFSETOF( smgPendOp, mOpLink ) );
    
    STL_DASSERT( STL_RING_IS_EMPTY( &aSessionEnv->mReadOnlyStmt ) == STL_TRUE );
    STL_RING_INIT_HEADLINK( &aSessionEnv->mReadOnlyStmt,
                            STL_OFFSETOF( smlStatement, mStmtLink ) );
    
    STL_DASSERT( STL_RING_IS_EMPTY( &aSessionEnv->mUpdatableStmt ) == STL_TRUE );
    STL_RING_INIT_HEADLINK( &aSessionEnv->mUpdatableStmt,
                            STL_OFFSETOF( smlStatement, mStmtLink ) );
    
    STL_TRY( sclFinalizeSessionEnv( (sclSessionEnv*)aSessionEnv,
                                    SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Ager Cleanup 연산을 위해서 Session Envrionment를 종료 한다.
 * @param[out]    aDeadSessEnv  Session Environment 포인터
 * @param[in,out] aEnv          Environment 포인터
 */
stlStatus smlCleanupSessionEnv( smlSessionEnv * aDeadSessEnv,
                                smlEnv        * aEnv )
{
    stlInt32 i;

    /**
     * instant table들이 있으면 clean up 해준다.
     */
    
    STL_TRY( smdCleanupAllInstTables( aEnv ) == STL_SUCCESS );

    /**
     * Session의 allocator를 해제한다.
     */


    if( KNL_DYNAMIC_MEMORY_TYPE( &aDeadSessEnv->mStatementMem ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        for( i = SML_CACHED_STMT_MAX_COUNT - 1; i >= 0; i-- )
        {
            if( KNL_REGION_MEMORY_TYPE( &aDeadSessEnv->mStatement[i]->mStatementMem ) == KNL_MEM_STORAGE_TYPE_SHM )
            {
                STL_TRY( knlDestroyRegionMem( &aDeadSessEnv->mStatement[i]->mStatementMem,
                                              KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                KNL_INIT_REGION_MEMORY( &aDeadSessEnv->mStatement[i]->mStatementMem );
            }

            STL_TRY( knlFreeDynamicMem( &aDeadSessEnv->mStatementMem,
                                        aDeadSessEnv->mStatement[i],
                                        KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        }
    }

    if( KNL_DYNAMIC_MEMORY_IS_CREATED( &aDeadSessEnv->mDynamicMem ) == STL_TRUE )
    {
        STL_TRY( knlDestroyDynamicMem( &aDeadSessEnv->mDynamicMem,
                                       KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        KNL_INIT_DYNAMIC_MEMORY( &aDeadSessEnv->mDynamicMem );
    }

    if( KNL_DYNAMIC_MEMORY_TYPE( &aDeadSessEnv->mStatementMem ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        STL_TRY( knlDestroyDynamicMem( &aDeadSessEnv->mStatementMem,
                                       KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        KNL_INIT_DYNAMIC_MEMORY( &aDeadSessEnv->mStatementMem );
    }

    if( KNL_REGION_MEMORY_TYPE( &aDeadSessEnv->mSessionMem ) == KNL_MEM_STORAGE_TYPE_SHM )
    {
        STL_TRY( knlDestroyRegionMem( &aDeadSessEnv->mSessionMem,
                                      KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        KNL_INIT_REGION_MEMORY( &aDeadSessEnv->mSessionMem );
    }
    
    aDeadSessEnv->mTransId = SML_INVALID_TRANSID;
    aDeadSessEnv->mTransViewScn = SML_INFINITE_SCN;
    aDeadSessEnv->mMinTransViewScn = SML_INFINITE_SCN;

    STL_RING_INIT_HEADLINK( &aDeadSessEnv->mPendOp,
                            STL_OFFSETOF( smgPendOp, mOpLink ) );
    STL_RING_INIT_HEADLINK( &aDeadSessEnv->mReadOnlyStmt,
                            STL_OFFSETOF( smlStatement, mStmtLink ) );
    STL_RING_INIT_HEADLINK( &aDeadSessEnv->mUpdatableStmt,
                            STL_OFFSETOF( smlStatement, mStmtLink ) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session에 Active Statement가 존재하는지 검사한다.
 * @param[in,out] aEnv         Storage Manager Environment
 */
stlBool smlHasActiveStatement( smlEnv * aEnv )
{
    stlBool sHasActiveStatement = STL_FALSE;
    
    if( STL_RING_IS_EMPTY( &(SML_SESS_ENV(aEnv)->mUpdatableStmt) ) == STL_FALSE )
    {
        sHasActiveStatement = STL_TRUE;
    }
    if( STL_RING_IS_EMPTY( &(SML_SESS_ENV(aEnv)->mReadOnlyStmt) ) == STL_FALSE )
    {
        sHasActiveStatement = STL_TRUE;
    }

    return sHasActiveStatement;
}
                            
/**
 * @brief 저장 관리자 레이어의 상태를 얻는다.
 * @param[out] aServerState 서버의 상태
 * @param[in] aEnv 종료할 Environment 포인터
 * @note 해당 함수는 MOUNT 단계 이후에 호출할수 있다.
 */
inline stlStatus smlGetServerState( smlServerState * aServerState,
                                    smlEnv         * aEnv )
{
    *aServerState = smfGetServerState();

    return STL_SUCCESS;
}

/**
 * @brief 데이터베이스 접근 모드를 얻는다.
 * @return smlDataAccessMode
 */
inline smlDataAccessMode smlGetDataAccessMode()
{
    return smfGetDataAccessMode();
}

/**
 * @brief 데이터베이스 접근 모드를 설정한다.
 * @param[in] aAccessMode 설정할 데이터베이스 접근 모드
 * @note 해당 함수는 OPEN 단계 이후에 호출할수 있다.
 */
inline void smlSetDataAccessMode( smlDataAccessMode aAccessMode )
{
    smfSetDataAccessMode( aAccessMode );
}

/**
 * @brief Pending Operation을 수행한다.
 * @param[in] aActionFlag Pending Action Flag
 * @param[in,out] aEnv Environment 구조체
 * @see @a aActionFlag : smgPendActionFlags
 */
stlStatus smlExecutePendOp( stlUInt32    aActionFlag,
                            smlEnv     * aEnv )
{
    return smgExecutePendOp( aActionFlag, 0 /* aStatementTcn */, aEnv );
}

/**
 * @brief Checkpoint Environment Identifier를 WarmupEntry에 등록한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smlRegisterCheckpointEnv( smlEnv * aEnv )
{
    smgSetCheckpointEnvId( aEnv );
}

/**
 * @brief Log Flusher Environment Identifier를 WarmupEntry에 등록한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smlRegisterLogFlusherEnv( smlEnv * aEnv )
{
    smgSetLogFlusherEnvId( aEnv );
}

/**
 * @brief Page Flusher Environment Identifier를 WarmupEntry에 등록한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smlRegisterPageFlusherEnv( smlEnv * aEnv )
{
    smgSetPageFlusherEnvId( aEnv );
}

/**
 * @brief Ager Environment Identifier를 WarmupEntry에 등록한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smlRegisterAgerEnv( smlEnv * aEnv )
{
    smgSetAgerEnvId( aEnv );
}

/**
 * @brief Cleanup Environment Identifier를 WarmupEntry에 등록한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smlRegisterCleanupEnv( smlEnv * aEnv )
{
    smgSetCleanupEnvId( aEnv );
}

/**
 * @brief Archivelog Environment Identifier를 WarmupEntry에 등록한다.
 * @param[in,out] aEnv Environment 구조체
 */
void smlRegisterArchivelogEnv( smlEnv * aEnv )
{
    smgSetArchivelogEnvId( aEnv );
}

/**
 * @brief IO Slave Environment Identifier를 WarmupEntry에 등록한다.
 * @param[in] aThreadIdx IO Slave Thread identifier
 * @param[in,out] aEnv Environment 구조체
 */
void smlRegisterIoSlaveEnv( stlInt64   aThreadIdx,
                            smlEnv   * aEnv )
{
    smgSetIoSlaveEnvId( aThreadIdx, aEnv );
}

/**
 * @brief Checkpoint Environment Identifier를 얻는다.
 * @return Checkpoint Environment Identifier
 */
stlUInt32 smlGetCheckpointEnvId()
{
    return smgGetCheckpointEnvId();
}

/**
 * @brief Log Flusher Environment Identifier를 얻는다.
 * @return Log Flusher Environment Identifier
 */
stlUInt32 smlGetLogFlusherEnvId()
{
    return smgGetLogFlusherEnvId();
}

/**
 * @brief Page Flusher Environment Identifier를 얻는다.
 * @return Page Flusher Environment Identifier
 */
stlUInt32 smlGetPageFlusherEnvId()
{
    return smgGetPageFlusherEnvId();
}

/**
 * @brief Ager Environment Identifier를 얻는다.
 * @return Ager Environment Identifier
 */
stlUInt32 smlGetAgerEnvId()
{
    return smgGetAgerEnvId();
}

/**
 * @brief Cleanup Environment Identifier를 얻는다.
 * @return Cleanup Environment Identifier
 */
stlUInt32 smlGetCleanupEnvId()
{
    return smgGetCleanupEnvId();
}

/**
 * @brief Archivelog Environment Identifier를 얻는다.
 * @return Archivelog Environment Identifier
 */
stlUInt32 smlGetArchivelogEnvId()
{
    return smgGetArchivelogEnvId();
}

/**
 * @brief IO Slave Environment Identifier를 얻는다.
 * @param[in] aThreadIdx IO Slave Thread identifier
 * @return IO Slave Environment Identifier
 */
stlUInt32 smlGetIoSlaveEnvId( stlInt64 aThreadIdx )
{
    return smgGetIoSlaveEnvId( aThreadIdx );
}

/**
 * @brief Ager Thread를 비활성화시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlDeactivateAger( smlEnv * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlAddEnvEvent( SML_EVENT_DEACTIVATE_AGER,
                             NULL,            /* aEventMem */
                             NULL,            /* aData */
                             0,               /* aDataSize */
                             SML_AGER_ENV_ID,
                             KNL_EVENT_WAIT_BLOCKED,
                             STL_FALSE,  /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Ager Thread를 비활성화시킨다.
 * @param[in] aBuildAgableScn Agable Scn을 build할지 여부
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlLoopbackAger( stlBool   aBuildAgableScn,
                           smlEnv  * aEnv )
{
    stlBool sBuildAgableScn = aBuildAgableScn;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlAddEnvEvent( SML_EVENT_LOOPBACK,
                             NULL,                 /* aEventMem */
                             &sBuildAgableScn,     /* aData */
                             STL_SIZEOF(stlBool),  /* aDataSize */
                             SML_AGER_ENV_ID,
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE,  /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Ager Thread를 활성화시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlActivateAger( smlEnv * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    smgSetAgerState( SML_AGER_STATE_ACTIVE );
    
    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[AGER] activated \n" )
             == STL_SUCCESS );
        
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Log Buffer를 Flush하는 것을 활성화시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlActivateLogFlushing( smlEnv * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smrStartLogFlushing( aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[FLUSHING LOG] enabled \n" )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Log Buffer를 Flush하는 것을 비활성화시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlDeactivateLogFlushing( smlEnv * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smrStopLogFlushing( aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[FLUSHING LOG] disabled \n" )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Ager의 상태를 얻는다.
 * @return Ager의 상태
 * @see smlAgerState
 */
stlInt16 smlGetAgerState()
{
    return smgGetAgerState();
}

/**
 * @brief 상위레이어에서 특정 시점동안 Aging을 하지 못하도록 View Scn을 설정한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in,out] aSessionEnv Session Environment 구조체
 * @remark 일반적으로 유효성 검사 단계에서 사용된다.
 */
void smlSetScnToDisableAging( smlTransId      aTransId,
                              smlSessionEnv * aSessionEnv )
{
    STL_ASSERT( aSessionEnv->mDisabledViewScn == SML_INFINITE_SCN );
    smxlGetAtomicSystemScn( &aSessionEnv->mDisabledViewScn );
}

/**
 * @brief smlSetScnToDisableAging으로 설정된 View Scn을 리셋한다.
 * @param[in,out] aSessionEnv Session Environment 구조체
 * @remark 일반적으로 유효성 검사 단계에서 사용된다.
 */
void smlUnsetScnToEnableAging( smlSessionEnv * aSessionEnv )
{
    aSessionEnv->mDisabledViewScn = SML_INFINITE_SCN;
}

/**
 * @brief Session 의 수행 통계 정보를 System 통계 정보로 추가한다.
 * @param[in]  aSessEnv   Session Environment
 * @remarks
 * System 통계 정보를 수행시마다 누적하는 것은 Multi CPU 환경에서
 * Cache-Miss 로 Scalability 저하 요인이 된다.
 */
void smlAddSessStat2SystemStat( smlSessionEnv * aSessEnv )
{
    if( aSessEnv != NULL )
    {
        if ( knlIsSharedSessionEnv( (knlSessionEnv *) aSessEnv ) == STL_TRUE )
        {
            gSmdWarmupEntry->mVersionConflictCount += aSessEnv->mVersionConflictCount;
        }
        else
        {
            /* heap env 임 */
        }
    }
    else
    {
        /* nothing to do */
    }
}

/**
 * @brief Request Group의 Worker 증가를 설정한다.
 * @param[in] aRequestGroupId Request Group Identifier
 */
void smlIncRequestGroupWorker( stlInt32 aRequestGroupId )
{
    smklIncRequestGroupWorker( aRequestGroupId );
}

/**
 * @brief Request Group의 Worker 감소를 설정한다.
 * @param[in] aRequestGroupId Request Group Identifier
 */
void smlDecRequestGroupWorker( stlInt32 aRequestGroupId )
{
    smklDecRequestGroupWorker( aRequestGroupId );
}

/**
 * @brief Open Database 이전에 Open에 대한 Valiation을 수행한다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlValidateMountDatabase( smlEnv * aEnv )
{
    /**
     * Validate Datafile Layer
     */
    STL_TRY( smfValidateMount( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Open Database 이전에 Open에 대한 Valiation을 수행한다.
 * @param[in]  aAccessMode   Database Access Mode
 * @param[in]  aLogOption    NORESETLOGS/RESETLOGS
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlValidateOpenDatabase( smlDataAccessMode    aAccessMode,
                                   stlChar              aLogOption,
                                   smlEnv             * aEnv )
{
    smrLsn  sLastLsn;
    
    /**
     * CDS나 DS모드는 이전에 TDS모드로 운영되었다면 TDS운영당시 정상으로 SHUTDOWN된
     * 경우에만 이용가능하다.
     */
    if( smxlGetDataStoreMode() != SML_DATA_STORE_MODE_TDS )
    {
        if( (smfGetDataStoreMode() == SML_DATA_STORE_MODE_TDS) &&
            (smfGetServerState() != SML_SERVER_STATE_SHUTDOWN) )
        {
            STL_THROW( RAMP_ERR_INVALID_SERVER_STATE );
        }

        STL_TRY_THROW( smrGetArchivelogMode() == SML_NOARCHIVELOG_MODE,
                       RAMP_ERR_TDS_ONLY_ARCHIVELOG );
    }

    /**
     * ACCESS READ-ONLY는 이전에 서버가 정상적으로 종료된 상태여야만 한다.
     */
    if( aAccessMode == SML_DATA_ACCESS_MODE_READ_ONLY )
    {
        if( smfGetServerState() != SML_SERVER_STATE_SHUTDOWN )
        {
            STL_THROW( RAMP_ERR_RECOVER_READ_ONLY_DATABASE );
        }
    }

    /**
     * Validate Layer
     */
    STL_TRY( smrReadLastLsnFromLogfile( &sLastLsn,
                                        aEnv ) == STL_SUCCESS );
    
    STL_TRY( smfValidateOpen( aLogOption,
                              sLastLsn,
                              aEnv ) == STL_SUCCESS );

    STL_TRY( smrValidateOpen( aLogOption,
                              aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SERVER_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_UNABLE_RECOVER_IN_CDS_MODE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_RECOVER_READ_ONLY_DATABASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_UNABLE_RECOVER_IN_READ_ONLY_MODE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_TDS_ONLY_ARCHIVELOG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_UNABLE_TO_ARCHIVELOG_IN_CDS_MODE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
