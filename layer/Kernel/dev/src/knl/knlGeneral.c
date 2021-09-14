/*******************************************************************************
 * knlGeneral.c
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
 * @file knlGeneral.c
 * @brief Kernel Layer General Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knl.h>

#include <knlDef.h>
#include <knDef.h>
#include <kniProcess.h>
#include <kniStartup.h>
#include <knlRegionMem.h>

/**
 * @addtogroup knlGeneral
 * @{
 */

extern knsEntryPoint * gKnEntryPoint;
extern stlErrorRecord  gKernelErrorTable[KNL_MAX_ERROR + 1];
extern stlFatalHandler gKnOldFatalHandler;
extern stlSignalExFunc gKnOldHupSignalHandler;
extern stlSignalExFunc gKnOldTermSignalHandler;
extern stlSignalExFunc gKnOldAbrtSignalHandler;
extern stlSignalExFunc gKnOldQuitSignalHandler;
extern stlInt32        gProcessTableID;
extern stlUInt32       gKnSignalCas;
extern stlUInt64       gKnProcessSar;

knlStartupFunc gStartupKN[KNL_STARTUP_PHASE_MAX] =
{
    NULL,
    kniStartupNoMount,
    kniStartupMount,
    kniStartupPreOpen,
    NULL
};

knlWarmupFunc gWarmupKN = kniWarmup;

knlShutdownFunc gShutdownKN[KNL_SHUTDOWN_PHASE_MAX] =
{
    NULL,
    kniShutdownInit,
    kniShutdownDismount,
    kniShutdownPostClose,
    NULL
};

knlCooldownFunc gCooldownKN = kniCooldown;

const stlChar *const gPhaseString[] =
{
    "NONE",
    "NO_MOUNT",
    "MOUNT",
    "PRE_OPEN",
    "OPEN",
    "INVALID PHASE",
    NULL
};

/**
 * @brief 현재 Startup Phase 를 얻는다.
 * @remarks
 */
knlStartupPhase  knlGetCurrStartupPhase()
{
    knlStartupPhase sPhase = KNL_STARTUP_PHASE_NONE;

    if ( gKnEntryPoint == NULL )
    {
        sPhase = KNL_STARTUP_PHASE_NONE;
    }
    else
    {
        sPhase = gKnEntryPoint->mStartupPhase;
    }

    return sPhase;
}

/**
 * @brief Entry Point의 Startup Phase를 반환한다.
 * @param[in] aEntryPoint
 * @return startup phase
 */
knlStartupPhase knlGetStartupPhase( void * aEntryPoint )
{
    return ((knsEntryPoint*) aEntryPoint)->mStartupPhase;
}

/**
 * @brief 현재 Startup Phase에 대응되는 Shutdown Phase를 얻는다.
 * @remarks
 */
knlShutdownPhase knlGetShutdownPhasePair( knlStartupPhase aStartupPhase )
{
    knlShutdownPhase sPhase = KNL_STARTUP_PHASE_NONE;
    
    switch( aStartupPhase )
    {
        case KNL_STARTUP_PHASE_NONE :
            sPhase = KNL_SHUTDOWN_PHASE_NONE;
            break;
        case KNL_STARTUP_PHASE_NO_MOUNT :
            sPhase = KNL_SHUTDOWN_PHASE_INIT;
            break;
        case KNL_STARTUP_PHASE_MOUNT :
            sPhase = KNL_SHUTDOWN_PHASE_DISMOUNT;
            break;
        case KNL_STARTUP_PHASE_PREOPEN : 
            sPhase = KNL_SHUTDOWN_PHASE_POSTCLOSE;
            break;
        case KNL_STARTUP_PHASE_OPEN :
            sPhase = KNL_SHUTDOWN_PHASE_CLOSE;
            break;
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }

    return sPhase;
}

/**
 * @brief Startup Phase 를 설정한다.
 * @remarks
 */
void knlSetStartupPhase( knlStartupPhase aStartupPhase )
{
    STL_DASSERT( gKnEntryPoint != NULL );
    gKnEntryPoint->mStartupPhase = aStartupPhase;
}


/**
 * @brief Startup 시의 시간을 얻는다.
 * @remarks
 */
stlTime knlGetStartupTime()
{
    return gKnEntryPoint->mStartupTime;
}

/**
 * @brief Kernel layer를 초기화한다.
 * @remark 프로세스당 한번 호출되어야 한다.
 */
stlStatus knlInitialize()
{
    stlSignalExFunc sOldFunc;
    stlErrorStack   sErrorStack;

    STL_TRY_THROW( stlGetProcessInitialized( STL_LAYER_KERNEL ) == STL_FALSE,
                   RAMP_SUCCESS );

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    /**
     * 하위 Layer 초기화
     */

    STL_TRY( cmlInitialize() == STL_SUCCESS );

    /**
     * Kernel Layer 에러 테이블을 등록한다.
     */
    
    stlRegisterErrorTable( STL_ERROR_MODULE_KERNEL, gKernelErrorTable );

    /* 프로세스 테이블 ID를 초기화한다. */
    KNI_INIT_PROCESS_TABLE_ID();

    /**
     * FATAL Handler 등록
     */
    STL_TRY( stlHookFatalHandler( kniFatalHandler,
                                  &gKnOldFatalHandler )
             == STL_SUCCESS );

    /**
     * STL_SIGNAL_USR1 Handler 등록
     */
    STL_TRY( stlSetSignalHandlerEx( STL_SIGNAL_USR1,
                                    kniSignalHandler,
                                    &sOldFunc,
                                    &sErrorStack )
             == STL_SUCCESS );

    STL_ASSERT( sOldFunc != kniSignalHandler );

    /**
     * STL_SIGNAL_HUP Handler 등록
     */
    STL_TRY( stlSetSignalHandlerEx( STL_SIGNAL_HUP,
                                    kniSignalHandler,
                                    &gKnOldHupSignalHandler,
                                    &sErrorStack )
             == STL_SUCCESS );

    STL_ASSERT( gKnOldHupSignalHandler != kniSignalHandler );

    /**
     * STL_SIGNAL_TERM Handler 등록
     */
    STL_TRY( stlSetSignalHandlerEx( STL_SIGNAL_TERM,
                                    kniSignalHandler,
                                    &gKnOldTermSignalHandler,
                                    &sErrorStack )
             == STL_SUCCESS );

    STL_ASSERT( gKnOldTermSignalHandler != kniSignalHandler );

#ifndef STL_DEBUG
    /**
     * STL_SIGNAL_ABRT Handler 등록
     */
    STL_TRY( stlSetSignalHandlerEx( STL_SIGNAL_ABRT,
                                    kniSignalHandler,
                                    &gKnOldAbrtSignalHandler,
                                    &sErrorStack )
             == STL_SUCCESS );

    STL_ASSERT( gKnOldAbrtSignalHandler != kniSignalHandler );
#endif

    /**
     * STL_SIGNAL_QUIT Handler 등록
     */
    STL_TRY( stlSetSignalHandlerEx( STL_SIGNAL_QUIT,
                                    kniSignalHandler,
                                    &gKnOldQuitSignalHandler,
                                    &sErrorStack )
             == STL_SUCCESS );

    STL_ASSERT( gKnOldQuitSignalHandler != kniSignalHandler );
    
    /**
     * STL_SIGNAL_INT Handler 등록
     */
    STL_TRY( stlSetSignalHandlerEx( STL_SIGNAL_INT,
                                    kniSignalHandler,
                                    &sOldFunc,
                                    &sErrorStack )
             == STL_SUCCESS );
    
    STL_ASSERT( sOldFunc != kniSignalHandler );
    
    /**
     * Sinal을 무시한다.
     * - STL_SIGNAL_ALRM
     * - STL_SIGNAL_PIPE
     */
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_ALRM,
                                  STL_SIGNAL_IGNORE,
                                  NULL,  /* aOldHandler */
                                  &sErrorStack )
             == STL_SUCCESS );
    
    gKnEntryPoint = NULL;

    stlSetProcessInitialized( STL_LAYER_KERNEL );

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus knlExecutePendingSignal( stlInt32 aPendingSignalNo )
{
    knlEnv        * sMyEnv = NULL;
    knlSessionEnv * sMySessEnv;
    stlInt32        sActiveSessionCount = 1;
    stlSignalExFunc sOldSignalHandler = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( knlLockAndBlockSignal( &sErrorStack ) == STL_SUCCESS );
    
    /**
     * 이미 Process가 다른 Thread에 의해서 죽고 있다면 나는 무시한다.
     */
    if( gKnProcessSar == KNL_SAR_NONE )
    {
        STL_TRY( knlUnlockAndUnblockSignal( &sErrorStack ) == STL_SUCCESS );
        STL_THROW( RAMP_SKIP );
    }
    
    sMyEnv = knlGetFirstMyProcessEnv();
    
    while( sMyEnv != NULL )
    {
        sMySessEnv = KNL_SESS_ENV(sMyEnv);
        
        if( (sMySessEnv != NULL) &&
            (knlIsTerminatedSessionEnv( sMySessEnv ) == STL_FALSE) &&
            (sMySessEnv->mTopLayer != STL_LAYER_SESSION) )  /* filter ager session */
        {
            KNL_SET_SESSION_STATUS( sMySessEnv, KNL_SESSION_STATUS_SIGNALED );
            
            (void)knlEndSession( sMySessEnv,
                                 KNL_END_SESSION_OPTION_IMMEDIATE,
                                 sMyEnv );
                    
            knlKillSession( sMySessEnv );
        }

        sMyEnv = knlGetNextMyProcessEnv( sMyEnv );
    }

    /**
     * Process의 새로운 세션을 막는다.
     */
    gKnProcessSar = KNL_SAR_NONE;
    
    STL_TRY( knlUnlockAndUnblockSignal( &sErrorStack ) == STL_SUCCESS );
    
    /**
     * My proccess에 속한 모든 active session이 종료되기를 기다린다.
     */
            
    while( STL_TRUE )
    {
        sActiveSessionCount = 0;
        
        sMyEnv = knlGetFirstMyProcessEnv();
                
        while( sMyEnv != NULL )
        {
            sMySessEnv = KNL_SESS_ENV(sMyEnv);
            
            if( (sMySessEnv != NULL) &&
                (sMySessEnv->mTopLayer != STL_LAYER_SESSION) )  /* filter ager session */
            {
                if( KNL_GET_SESSION_ACCESS_MASK( sMySessEnv ) == KNL_SESSION_ACCESS_ENTER )
                {
                    sActiveSessionCount++;
                }
            }

            sMyEnv = knlGetNextMyProcessEnv( sMyEnv );
        }

        if( sActiveSessionCount == 0 )
        {
            break;
        }

        stlSleep( 10000 );
    }
        
    switch( aPendingSignalNo )
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
            sOldSignalHandler( aPendingSignalNo, NULL, NULL );
        }
    }
    
    stlExitProc( STL_FAILURE );

    STL_RAMP( RAMP_SKIP );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief System Available Resource 목록에 주어진 Resource를 추가한다.
 * @param[in]  aSarMask System Available Resource에 추가할 항목
 * @param[in]  aEnv     Envrironment
 * @see @a aSarMask knlSystemAvailableResource
 */
stlStatus knlAddSar( stlUInt32   aSarMask,
                     knlEnv    * aEnv )
{
    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, KNL_ERROR_STACK(aEnv) );

    gKnEntryPoint->mSar |= aSarMask;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief System Available Resource 목록에서 주어진 Resource를 제거한다.
 * @param[in]  aSarMask System Available Resource에서 제거할 항목
 * @param[in]  aEnv     Envrironment
 * @see @a aSarMask knlSystemAvailableResource
 */
stlStatus knlUnsetSar( stlUInt32   aSarMask,
                       knlEnv    * aEnv )
{
    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, KNL_ERROR_STACK(aEnv) );

    gKnEntryPoint->mSar = gKnEntryPoint->mSar & (~aSarMask);
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief System Available Resource 목록을 주어진 Resource로 설정한다.
 * @param[in]  aSarMask System Available Resource에 설정할 항목
 * @param[in]  aEnv     Envrironment
 * @see @a aSarMask knlSystemAvailableResource
 */
stlStatus knlSetSar( stlUInt32   aSarMask,
                     knlEnv    * aEnv )
{
    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, KNL_ERROR_STACK(aEnv) );

    gKnEntryPoint->mSar = aSarMask;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 사용할 Resource가 System Available Resource 목록을 위배하는지 여부를 검사한다.
 * @param[in]  aSarMask  검사할 항목
 * @param[in]  aTopLayer Top Layer
 * @param[in]  aEnv      Envrironment
 * @see @a aSarMask knlSystemAvailableResource
 * @code
 * [KNL_ERRCODE_NOT_AVAILABLE_RESOURCE]
 *     변환 과정을 수행하지 못할 때
 * @endcode
 */
stlStatus knlValidateSar( stlUInt32       aSarMask,
                          stlLayerClass   aTopLayer,
                          knlEnv        * aEnv )
{
    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * System Available Resource의 제한은 Top Layer가
     * User Layer 이상인 경우만 동작한다.
     */
    if( (KNL_IS_USER_LAYER(aTopLayer) == STL_TRUE) &&
        (gKnEntryPoint->mSar & aSarMask) != aSarMask )
    {
        STL_THROW( RAMP_ERR_SERVICE_NOT_AVAILABLE );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SERVICE_NOT_AVAILABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_SERVICE_NOT_AVAILABLE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 사용할 Resource가 System Available Resource 목록을 위배하는지 여부를 검사한다.
 * @param[in]  aSarMask 검사할 항목
 * @see @a aSarMask knlSystemAvailableResource
 * @return 위배하지 않았다면 STL_TRUE, 그렇지 않은 경우에 STL_FALSE
 */
stlBool knlCheckSar( stlUInt32 aSarMask )
{
    stlBool sAvailable = STL_TRUE;
    
    STL_ASSERT( gKnEntryPoint != NULL );

    if( (gKnEntryPoint->mSar & aSarMask) != aSarMask )
    {
        sAvailable = STL_FALSE;
    }
    
    return sAvailable;
}

stlStatus knlValidateProcessSar( stlUInt32       aSarMask,
                                 stlErrorStack * aErrorStack )
{
    if( (gKnProcessSar & aSarMask) != aSarMask )
    {
        STL_THROW( RAMP_ERR_SERVER_REJECTED_THE_CONNECTION );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SERVER_REJECTED_THE_CONNECTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_SERVER_REJECTED_THE_CONNECTION,
                      "process is shutting down",
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 강제적인 System fatal을 발생시킨다.
 * @param[in]  aCauseString  fatal을 유발시킨 원인을 알리는 문자열
 * @param[in]  aEnv          Envrironment
 */
void knlSystemFatal( stlChar * aCauseString,
                     knlEnv  * aEnv )
{

    stlBool sMakeCore;
    
    (void)knlLogMsgUnsafe( NULL,
                           aEnv,
                           KNL_LOG_LEVEL_FATAL,
                           "[SYSTEM FATAL] CAUSE(\"%s\")",
                           aCauseString );
    
    (void)knlLogErrorCallStack( NULL,
                                KNL_ENV(aEnv) );
    
    (void)knlLogErrorStack( NULL,
                            KNL_ERROR_STACK(aEnv),
                            KNL_ENV(aEnv) );

    sMakeCore = knlGetPropertyBoolValueByID( KNL_PROPERTY_GENERATE_CORE_DUMP,
                                             aEnv );
    
    kniSystemFatal( STL_TRUE, sMakeCore );
    stlExitProc( STL_FAILURE );
}

/**
 * @brief System fatal이 가능한지 검사한다.
 * @param[in]  aDoResolve  System Fatal의 해결 여부
 * @param[in]  aEnv        Envrironment
 * @return System fatal이 해결된다면 STL_TRUE, 그렇지 않다면 STL_FALSE
 */
stlBool knlResolveSystemFatal( stlBool   aDoResolve,
                               knlEnv  * aEnv )
{
    stlBool          sResult = STL_FALSE;
    knlLogicalAddr * sLatchArray;
    stlInt32         sLatchCount;
    stlInt32         i;
    knlLatch       * sLatch;
    
    if( KNL_GET_ENTER_CRITICAL_SECTION_COUNT( aEnv ) != 0 )
    {
        sLatchCount = KNL_ENV_LATCH_COUNT( aEnv );
        sLatchArray = (knlLogicalAddr*)KNL_ENV_LATCH_STACK( aEnv );

        /**
         * Latch 이외에 추가적인 Critical section에 들어가 있는 경우는
         * System Fatal로 처리한다.
         */
        STL_TRY_THROW( sLatchCount == KNL_GET_ENTER_CRITICAL_SECTION_COUNT( aEnv ),
                       RAMP_FINISH );
        
        /**
         * Exclusive Latch를 획득한 상태인지 검사한다.
         */

        for( i = 0; i < sLatchCount; i++ )
        {
            if( sLatchArray[i] == KNL_LOGICAL_ADDR_NULL )
            {
                continue;
            }

            sLatch = knlToPhysicalAddr( sLatchArray[i] );
            
            STL_TRY_THROW( sLatch->mCurMode != KNL_LATCH_MODE_EXCLUSIVE,
                           RAMP_FINISH );

            if( aDoResolve == STL_TRUE )
            {
                /**
                 * Shared Latch 라면 Latch를 푼다.
                 */
                (void) knlReleaseLatch( sLatch, aEnv );
            }
        }
    }

    sResult = STL_TRUE;
    
    STL_RAMP( RAMP_FINISH );

    return sResult;
}

stlStatus knlLockAndBlockSignal( stlErrorStack * aErrorStack )
{
    STL_TRY( stlBlockControllableThreadSignals( aErrorStack )
             == STL_SUCCESS );

    knlLockSignal();

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus knlUnlockAndUnblockSignal( stlErrorStack * aErrorStack )
{
    knlUnlockSignal();
    
    STL_TRY( stlUnblockControllableThreadSignals( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void knlLockSignal()
{
    stlUInt32 sOldCasState;

    while( STL_TRUE )
    {
        sOldCasState = stlAtomicCas32( &gKnSignalCas,
                                       KNI_SIGNAL_CAS_LOCKED,
                                       KNI_SIGNAL_CAS_INIT );

        if( sOldCasState == KNI_SIGNAL_CAS_INIT )
        {
            break;
        }
        
        stlSleep( 10000 );
    }
}

void knlUnlockSignal()
{
    stlAtomicCas32( &gKnSignalCas,
                    KNI_SIGNAL_CAS_INIT,
                    KNI_SIGNAL_CAS_LOCKED );
}

/**
 * @brief System 전체의 Disk IO를 불허한다.
 */
void knlDisableDiskIo( void )
{
    gKnEntryPoint->mEnableDiskIo = STL_FALSE;
}

/**
 * @brief System 전체의 Disk IO를 허용한다.
 */
void knlEnableDiskIo( void )
{
    gKnEntryPoint->mEnableDiskIo = STL_TRUE;
}

/**
 * @brief 현재 System이 Disk IO를 허용하는지 여부를 반환한다.
 * @param[in,out] aEnv  Environment 구조체
 */
stlBool knlAllowDiskIo( knlEnv * aEnv )
{
    KNL_CHECK_THREAD_CANCELLATION( aEnv );
    
    return gKnEntryPoint->mEnableDiskIo;
}

/**
 * @brief client binary version의 유효성을 검사한다.
 * @param[in,out] aEnv  Environment 구조체
 */
stlStatus knlValidateBinaryVersion( knlEnv * aEnv )
{
    stlVersionInfo sVersionInfo;
    stlChar        sServerProduct[STL_PRODUCT_VERSION_LENGTH + 1];
    stlChar        sClientProduct[STL_PRODUCT_VERSION_LENGTH + 1];
    
    STL_PARAM_VALIDATE( gKnEntryPoint != NULL, KNL_ERROR_STACK(aEnv) );

    stlGetVersionInfo( &sVersionInfo );
    
    STL_TRY_THROW( stlMatchVersion( &gKnEntryPoint->mVersionInfo,
                                    &sVersionInfo )
                   == STL_TRUE, RAMP_ERR_MISMATCH_BINARY_VERSION );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MISMATCH_BINARY_VERSION )
    {
        stlMemset( sServerProduct, 0x00, STL_PRODUCT_VERSION_LENGTH + 1 );
        stlMemset( sClientProduct, 0x00, STL_PRODUCT_VERSION_LENGTH + 1 );

        stlMemcpy( sServerProduct,
                   gKnEntryPoint->mVersionInfo.mProduct,
                   STL_PRODUCT_VERSION_LENGTH );
        
        stlMemcpy( sClientProduct,
                   sVersionInfo.mProduct,
                   STL_PRODUCT_VERSION_LENGTH );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_MISMATCH_BINARY_VERSION,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sServerProduct,
                      gKnEntryPoint->mVersionInfo.mMajor,
                      gKnEntryPoint->mVersionInfo.mMinor,
                      gKnEntryPoint->mVersionInfo.mPatch,
                      gKnEntryPoint->mVersionInfo.mRevision,
                      sClientProduct,
                      sVersionInfo.mMajor,
                      sVersionInfo.mMinor,
                      sVersionInfo.mPatch,
                      sVersionInfo.mRevision );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ASSERT 상황에서 Binary를 Dump하기 위해서 사용한다.
 * @param[in] aBinary     Dump할 Binary
 * @param[in] aSize       Dump할 Binary 크기
 * @param[in] aRegionMem  String할당을 위한 메모리 Allocator
 * @param[in,out] aEnv    Environment 구조체
 * @return HEX string
 * @note 메모리 릭이 발생하기 때문에 ASSERT 상황에서만 호출되어야 한다.
 */
void * knlDumpBinaryForAssert( stlChar      * aBinary,
                               stlInt32       aSize,
                               knlRegionMem * aRegionMem,
                               knlEnv       * aEnv )
{
    stlChar * sDumpString = NULL;
    stlChar * sPtr;
    stlInt32  sAccSize = 0;

    /**
     * 메모리 릭이 발생할수 있기 때문에 ASSERT 상황에서만 호출되어야 한다.
     */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                (aSize * 3) < 100 ? 100 : (aSize * 3),
                                (void**)&sDumpString,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
  
    sPtr = sDumpString;
    while( sAccSize < aSize )
    {
        stlSnprintf(sPtr, 10, "%4x : ", sAccSize);
        sPtr += stlStrlen(sPtr);

        STL_TRY( stlBinary2HexString( (stlUInt8*)(aBinary + sAccSize),
                                      (aSize - sAccSize > 32) ? 32 : aSize - sAccSize,
                                      sPtr,
                                      128,
                                      KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        sPtr += stlStrlen(sPtr);
        *sPtr = '\n';
        sPtr++;
        sAccSize += (aSize - sAccSize > 32) ? 32 : aSize - sAccSize;
    }

    return sDumpString;
    
    STL_FINISH;
             
    return sDumpString;
}

/**
 * X${layer}_SYSTEM_INFO들을 위한 표준 컬럼 정의
 */
knlFxColumnDesc gKnlSystemInfoColumnDesc[] =
{
    {
        "NAME",
        "name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knlFxSystemInfo, mName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "VALUE",
        "value",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knlFxSystemInfo, mValue ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "CATEGORY",
        "category",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knlFxSystemInfo, mCategory ),
        STL_SIZEOF( knlStatCategory ),
        STL_FALSE  /* nullable */
    },
    {
        "COMMENTS",
        "comments",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knlFxSystemInfo, mComment ),
        KNL_MAX_FIXEDTABLE_COMMENTS_LENGTH,
        STL_TRUE  /* nullable */
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
 * X${layer}_SESS_{ENV/STAT}을 위한 표준 컬럼 정의
 */
knlFxColumnDesc gKnlSessionInfoColumnDesc[] =
{
    {
        "NAME",
        "name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knlFxSessionInfo, mName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ID",
        "session identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knlFxSessionInfo, mSessId ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "CATEGORY",
        "category",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knlFxSessionInfo, mCategory ),
        STL_SIZEOF( knlStatCategory ),
        STL_FALSE  /* nullable */
    },
    {
        "VALUE",
        "value",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knlFxSessionInfo, mValue ),
        STL_SIZEOF( stlInt64 ),
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
 * X${layer}_STMT_STAT을 위한 표준 컬럼 정의
 */
knlFxColumnDesc gKnlStatementInfoColumnDesc[] =
{
    {
        "NAME",
        "name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knlFxStatementInfo, mName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "SESSION_ID",
        "session identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knlFxStatementInfo, mSessId ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "ID",
        "statement identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knlFxStatementInfo, mStmtId ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "CATEGORY",
        "category",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knlFxStatementInfo, mCategory ),
        STL_SIZEOF( knlStatCategory ),
        STL_FALSE  /* nullable */
    },
    {
        "VALUE",
        "value",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knlFxStatementInfo, mValue ),
        STL_SIZEOF( stlInt64 ),
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
 * X${layer}_PROC_{ENV/STAT}을 위한 표준 컬럼 정의
 */
knlFxColumnDesc gKnlProcInfoColumnDesc[] =
{
    {
        "NAME",
        "name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knlFxProcInfo, mName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ID",
        "environment identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knlFxProcInfo, mEnvId ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "CATEGORY",
        "category",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knlFxProcInfo, mCategory ),
        STL_SIZEOF( knlStatCategory ),
        STL_FALSE  /* nullable */
    },
    {
        "VALUE",
        "value",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knlFxProcInfo, mValue ),
        STL_SIZEOF( stlInt64 ),
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

/** @} */
