/*******************************************************************************
 * knlEvent.c
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
 * @file knlEvent.c
 * @brief Kernel Layer Event Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knlLatch.h>
#include <knlRing.h>
#include <knlEnvironment.h>
#include <knlDynamicMem.h>
#include <knlSession.h>
#include <kneEvent.h>
#include <knaLatch.h>
#include <knlTraceLogger.h>

extern knsEntryPoint * gKnEntryPoint;
extern stlInt32        gProcessTableID;

/**
 * @addtogroup knlEvent
 * @{
 */

/**
 * @brief 레이어별 이벤트 테이블 배열.
 * 각 레이어별 이벤트 테이블 주소가 저장된다.
 */
knlEventTable * gLayerEventTables[STL_LAYER_MAX];

/**
 * @brief Event Table을 등록한다.
 * @param[in] aLayerClass Layer Class
 * @param[in] aEventTable 등록할 Event Table
 */
void knlRegisterEventTable( stlLayerClass   aLayerClass,
                            knlEventTable * aEventTable )
{
    gLayerEventTables[aLayerClass] = aEventTable;
}

/**
 * @brief Event 객체를 초기화한다.
 * @param[in] aEvent 초기화할 Event 객체
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlInitEvent( knlEvent * aEvent,
                        knlEnv   * aEnv )
{
    STL_RING_INIT_HEADLINK( &aEvent->mEventHead,
                            STL_OFFSETOF( kneEventData, mEventLink ) );

    STL_INIT_SPIN_LOCK( aEvent->mSpinLock );

    aEvent->mEventCount = 0;
    aEvent->mEventListener = NULL;
    aEvent->mCurEsn = 0;
    aEvent->mCompletedEsn = 0;
    
    return STL_SUCCESS;
}

/**
 * @brief Event Handler를 등록한다.
 * @param[in] aEventId Event Identifier
 * @param[in] aEventHandler 등록할 Event Handler
 */
void knlRegisterEventHandler( stlInt32        aEventId,
                              knlEventHandler aEventHandler )
{
    stlInt32 sLayerClass;
    stlInt32 sEventValue;
    
    sLayerClass = KNL_GET_EVENT_LAYER( aEventId );
    sEventValue = KNL_GET_EVENT_VALUE( aEventId );
    
    gLayerEventTables[sLayerClass][sEventValue].mEventHandler = aEventHandler;
}

/**
 * @brief Event Listener를 초기화한다.
 * @param[in] aEventListener 초기화할 Event Listener
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlInitEventListener( knlEventListener * aEventListener,
                                knlEnv           * aEnv )
{
    STL_TRY( stlCreateSemaphore( &aEventListener->mSem,
                                 "EnvSem",
                                 0,
                                 KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Event Listener를 종료한다.
 * @param[in] aEventListener 종료할 Event Listener
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlFiniEventListener( knlEventListener * aEventListener,
                                knlEnv           * aEnv )
{
    STL_TRY( stlDestroySemaphore( &aEventListener->mSem,
                                  KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Event Listener를 Environment에 등록한다.
 * @param[in] aEventListener 등록할 Event Listener
 * @param[in,out] aEnv environment 정보
 */
void knlRegisterEventListener( knlEventListener * aEventListener,
                               knlEnv           * aEnv )
{
    aEnv->mEvent.mEventListener = aEventListener;
}

/**
 * @brief Event Listener에서 Event가 발생하기를 대기한다.
 * @param[in] aEventListener 대기할 대상 Event Listener
 * @param[in] aTimeout 대기 시간
 * @param[in] aIsTimedOut 시간 초과 여부
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlWaitEvent( knlEventListener * aEventListener,
                        stlInterval        aTimeout,
                        stlBool          * aIsTimedOut,
                        knlEnv           * aEnv )
{
    KNL_CHECK_THREAD_CANCELLATION( aEnv );
    
    if( aTimeout == STL_INFINITE_TIME )
    {
        while( 1 )
        {
            KNL_CHECK_THREAD_CANCELLATION( aEnv );
            
            STL_TRY( stlTimedAcquireSemaphore( &aEventListener->mSem,
                                               KNL_CHECK_CANCELLATION_TIME,
                                               aIsTimedOut,
                                               KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            if( *aIsTimedOut == STL_FALSE )
            {
                break;
            }
        }
    }
    else
    {
        STL_TRY( stlTimedAcquireSemaphore( &aEventListener->mSem,
                                           aTimeout,
                                           aIsTimedOut,
                                           KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Event Listener를 깨운다.
 * @param[in] aEventListener 깨울 대상 Event Listener
 * @param[in,out] aEnv environment 정보
 */
stlStatus knlWakeupEventListener( knlEventListener * aEventListener,
                                  knlEnv           * aEnv )
{
    STL_TRY( stlReleaseSemaphore( &aEventListener->mSem,
                                  KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Process Event를 추가한다.
 * @param[in] aEventId Event Identifier
 * @param[in] aData Event 처리에 필요한 사용자 Context
 * @param[in] aDataSize @a aData의 바이트단위 크기
 * @param[in] aProcessId 대상 프로세스 아이디
 * @param[in,out] aEnv environment 정보
 * @note @a aProcessId가 KNL_INVALID_PROCESS_ID라면 전체 Process에 Event를 추가한다.
 */
stlStatus knlAddProcessEvent( stlUInt32    aEventId,
                              void       * aData,
                              stlInt32     aDataSize,
                              stlInt32     aProcessId,
                              knlEnv     * aEnv )
{
    kniProcessManager * sProcessManager;
    kniProcessInfo    * sProcessInfoArray;
    kniProcessInfo    * sProcessInfo;
    stlInt32            i;
    stlBool             sIsSuccess;
    stlInt32            sState = 0;

    STL_TRY_THROW( gKnEntryPoint->mProcessManager != NULL,
                   RAMP_ERR_INVALID_STATIC_SHM );

    STL_TRY_THROW( aProcessId != KNL_INVALID_PROCESS_ID, RAMP_SUCCESS );

    STL_TRY( knlAcquireLatch( &(gKnEntryPoint->mProcessManager->mLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    sProcessManager = gKnEntryPoint->mProcessManager;
    sProcessInfoArray = (void*)sProcessManager + STL_SIZEOF( kniProcessManager );

    if( aProcessId != KNL_BROADCAST_PROCESS_ID )
    {
        STL_TRY( kneAddEvent( &(sProcessInfoArray[aProcessId].mEvent),
                              aEventId,
                              NULL, /* aEventMem */
                              aData,
                              aDataSize,
                              KNL_EVENT_FAIL_BEHAVIOR_STOP,
                              NULL, /* aWaitEnv */
                              KNL_EVENT_WAIT_NO,
                              NULL, /* aEsn */
                              aEnv ) == STL_SUCCESS );
    }
    else
    {
        for( i = 0; i < sProcessManager->mMaxProcessCount; i++ )
        {
            sProcessInfo = &(sProcessInfoArray[i]);
        
            if( sProcessInfo->mProc.mProcID == -1 )
            {
                continue;
            }

            /*
             * 자기 자신은 제외
             */
            if( i == gProcessTableID )
            {
                continue;
            }

            STL_TRY( kneAddEvent( &(sProcessInfo->mEvent),
                                  aEventId,
                                  NULL, /* aEventMem */
                                  aData,
                                  aDataSize,
                                  KNL_EVENT_FAIL_BEHAVIOR_STOP,
                                  NULL, /* aWaitEnv */
                                  KNL_EVENT_WAIT_NO,
                                  NULL, /* aEsn */
                                  aEnv ) == STL_SUCCESS );
        }
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &(gKnEntryPoint->mProcessManager->mLatch),
                              aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STATIC_SHM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &(gKnEntryPoint->mProcessManager->mLatch),
                               aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Environment Event를 추가한다.
 * @param[in] aEventId Event Identifier
 * @param[in] aEventMem 미리 확보된 Event Memory
 * @param[in] aData Event 처리에 필요한 사용자 Context
 * @param[in] aDataSize @a aData의 바이트단위 크기
 * @param[in] aTargetEnvId 대상 Environment 아이디
 * @param[in] aEventAttr Event Attribute
 * @param[in] aForceSuccess 반드시 성공되어야 하는지 여부
 * @param[in,out] aEnv environment 정보
 * @note @a aTargetEnvId가 KNL_INVALID_ENV_ID라면 전체 Environment에 Event를 추가한다.
 */
stlStatus knlAddEnvEvent( stlUInt32      aEventId,
                          void         * aEventMem,
                          void         * aData,
                          stlUInt32      aDataSize,
                          stlUInt32      aTargetEnvId,
                          stlUInt32      aEventAttr,
                          stlBool        aForceSuccess,
                          knlEnv       * aEnv )
{
    knlEnv       * sTargetEnv;
    stlBool        sIsSuccess;
    stlInt32       sState = 0;
    stlUInt32      sTargetEnvId;
    void         * sWaitEnv = NULL;
    stlInt32       sErrorCount;
    stlBool        sIsTimedOut;
    stlInt64       sEsn = 0;
    stlUInt32      sWaitMode;

    STL_TRY_THROW( aTargetEnvId != KNL_INVALID_ENV_ID, RAMP_SUCCESS );

    STL_RAMP( RAMP_RETRY );
    
    /**
     * Event 검사
     */

    if( aForceSuccess == STL_FALSE )
    {
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    }
    
    STL_TRY( knlAcquireLatch( &(gKnEntryPoint->mEnvManager->mLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    sWaitMode = aEventAttr & KNL_EVENT_WAIT_MASK;
    
    if( aTargetEnvId != KNL_BROADCAST_ENV_ID )
    {
        sTargetEnv = knlGetEnv( aTargetEnvId );

        if( (sWaitMode == KNL_EVENT_WAIT_BLOCKED) ||
            (sWaitMode == KNL_EVENT_WAIT_POLLING) )
        {
            sWaitEnv = aEnv;
        }

        sErrorCount = stlGetFailureErrorCount( KNL_ERROR_STACK( aEnv ) );
        
        if( kneAddEvent( &sTargetEnv->mEvent,
                         aEventId,
                         aEventMem,
                         aData,
                         aDataSize,
                         (aEventAttr & KNL_EVENT_FAIL_BEHAVIOR_MASK),
                         sWaitEnv,
                         sWaitMode,
                         &sEsn,
                         aEnv )
            == STL_FAILURE )
        {
            if( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) == KNL_ERRCODE_UNABLE_EXTEND_MEMORY )
            {
                if( aForceSuccess == STL_TRUE )
                {
                    stlPopError( KNL_ERROR_STACK(aEnv) );
                
                    sState = 0;
                    STL_TRY( knlReleaseLatch( &(gKnEntryPoint->mEnvManager->mLatch),
                                              aEnv ) == STL_SUCCESS );
                
                    STL_THROW( RAMP_RETRY );
                }
            }

            STL_TRY( STL_FALSE );
        }

        sState = 0;
        STL_TRY( knlReleaseLatch( &(gKnEntryPoint->mEnvManager->mLatch),
                                  aEnv ) == STL_SUCCESS );
    
        if( sTargetEnv->mEvent.mEventListener != NULL )
        {
            STL_TRY( knlWakeupEventListener( sTargetEnv->mEvent.mEventListener,
                                             aEnv ) == STL_SUCCESS );
        }
        
        if( sWaitMode == KNL_EVENT_WAIT_BLOCKED )
        {
            while( 1 )
            {
                KNL_CHECK_THREAD_CANCELLATION( aEnv );
                
                STL_TRY( knlSuspend( KNL_CHECK_CANCELLATION_TIME,
                                     &sIsTimedOut,
                                     aEnv ) == STL_SUCCESS );

                if( sIsTimedOut == STL_FALSE )
                {
                    break;
                }
            }

            /**
             * Event Listener에서 에러가 발생한 경우 Event Listener가
             * aEnv에 에러를 설정해준다.
             */
            if( sErrorCount < stlGetFailureErrorCount( KNL_ERROR_STACK(aEnv) ) )
            {
                /**
                 * Warning은 SUCCESS를 반환해야 한다.
                 */
                STL_TRY( stlHasWarningError( KNL_ERROR_STACK(aEnv) ) == STL_TRUE );
            }
        }
        else if( sWaitMode == KNL_EVENT_WAIT_POLLING )
        {
            while( sTargetEnv->mEvent.mCompletedEsn < sEsn )
            {
                /**
                 * Event가 취소되었을 경우, 아직 event handler가
                 * event를 처리하지 않았다면 취소한다.
                 */
                if( knlCheckQueryTimeout( aEnv ) == STL_FAILURE )
                {
                    STL_TRY( kneCancelEvent( &sTargetEnv->mEvent,
                                             sEsn,
                                             aEnv ) == STL_SUCCESS );

                    STL_TRY( STL_FALSE );
                }

                stlSleep( 10000 );
            }
            
            /**
             * Event Listener에서 에러가 발생한 경우 Event Listener가
             * aEnv에 에러를 설정해준다.
             */
            if( sErrorCount < stlGetFailureErrorCount( KNL_ERROR_STACK(aEnv) ) )
            {
                /**
                 * Warning은 SUCCESS를 반환해야 한다.
                 */
                STL_TRY( stlHasWarningError( KNL_ERROR_STACK(aEnv) ) == STL_TRUE );
            }
        }
        else
        {
            /*
             * nothing to do
             */
        }
    }
    else
    {
        /**
         * Broadcast type은 미리 확보된 메모리를 사용할수 없다.
         */
        
        STL_DASSERT( aEventMem == NULL );
        
        sTargetEnv = knlGetFirstEnv();

        while( sTargetEnv != NULL )
        {
            /*
             * 자기 자신은 제외
             */
            STL_TRY( knlGetEnvId( sTargetEnv, &sTargetEnvId ) == STL_SUCCESS );
            
            if( sTargetEnvId == aTargetEnvId )
            {
                sTargetEnv = knlGetNextEnv( sTargetEnv );
                continue;
            }
            
            if( knlIsUsedEnv( sTargetEnv ) == STL_TRUE )
            {
                STL_TRY( kneAddEvent( &sTargetEnv->mEvent,
                                      aEventId,
                                      NULL,
                                      aData,
                                      aDataSize,
                                      KNL_EVENT_FAIL_BEHAVIOR_STOP,
                                      NULL, /* aWaitEnv */
                                      KNL_EVENT_WAIT_NO,
                                      NULL, /* aEsn */
                                      aEnv ) == STL_SUCCESS );
            }

            sTargetEnv = knlGetNextEnv( sTargetEnv );
        }
        
        sState = 0;
        STL_TRY( knlReleaseLatch( &(gKnEntryPoint->mEnvManager->mLatch),
                                  aEnv ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &(gKnEntryPoint->mEnvManager->mLatch),
                               aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Environment Event Memory를 할당한다.
 * @param[in]     aDataSize    바이트단위 크기
 * @param[out]    aEventMem    할당 받은 메모리 포인터
 * @param[in,out] aEnv         environment 정보
 * @note BROADCAST type은 지원하지 않는다.
 */
stlStatus knlAllocEnvEventMem( stlUInt32    aDataSize,
                               void      ** aEventMem,
                               knlEnv     * aEnv )
{
    STL_TRY( kneAllocEventMem( aDataSize,
                               aEventMem,
                               aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Environment Event Memory를 해제한다.
 * @param[in]     aEventMem    바이트단위 크기
 * @param[in,out] aEnv         environment 정보
 * @note BROADCAST type은 지원하지 않는다.
 */
stlStatus knlFreeEnvEventMem( void    * aEventMem,
                              knlEnv  * aEnv )
{
    STL_TRY( knlFreeDynamicMem( &gKnEntryPoint->mEventMem,
                                aEventMem,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Session이 실행 중인 Event가 취소되었는지 체크한다.
 * @param[in,out] aEnv  environment 정보
 */
stlStatus knlCheckEnvEvent( knlEnv  * aEnv )
{
    /* knlSessionEnv * sSessEnv; */
    kneEventData  * sEventData = NULL;

    /* sSessEnv = KNL_SESS_ENV( aEnv ); */

    sEventData = (kneEventData *)(aEnv->mExecuteEvent);

    STL_DASSERT( sEventData != NULL );

    STL_TRY_THROW( sEventData->mCanceled == STL_FALSE,
                   RAMP_ERR_OPERATION_CANCELED );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OPERATION_CANCELED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_OPERATION_CANCELED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Environment Event를 처리한다.
 * @param[out] aEventId   처리가 완료된 Event Id
 * @param[out] aIsCanceled Event Cancel 여부
 * @param[out] aIsSuccess Event 처리 성공 여부
 * @param[in,out] aEnv environment 정보
 * @note 정상적으로 Event가 처리되면 처리된 Event는 삭제된다.
 * <BR> Environment Event는 그 특성상 다수개가 연결되어 있을수도 있으며,
 * <BR> 처리시간이 오래 걸릴수 있는 특징을 갖기 때문에
 * <BR> 다수의 Event가 연결되어 있다하더라도, 최상위 Event 한개만 처리한다.
 */
stlStatus knlExecuteEnvEvent( knlEnv    * aEnv,
                              stlInt32  * aEventId,
                              stlBool   * aIsCanceled,
                              stlBool   * aIsSuccess )
{
    kneEventData  * sEventData = NULL;
    stlInt32        sEventLayer;
    stlInt32        sEventValue = -1;
    knlEventTable * sEventTable;
    stlBool         sDone = STL_FALSE;
    stlInt32        sState = 0;
    stlUInt64       sDummy = 0;
    
    *aEventId = sEventValue;
    *aIsCanceled = STL_FALSE;
    *aIsSuccess = STL_FALSE;

    if( aEnv->mEvent.mEventCount == 0 )
    {
        STL_THROW( RAMP_SUCCESS );
    }
    
    sEventData = STL_RING_GET_FIRST_DATA( &aEnv->mEvent.mEventHead );

    while( STL_RING_IS_HEADLINK( &aEnv->mEvent.mEventHead, sEventData ) == STL_FALSE )
    {
        /**
         * Event handler를 수행하기 전에 event가 cancel되었는지 체크한다.
         */
        if( sEventData->mCanceled == STL_FALSE )
        {
            sEventLayer = KNL_GET_EVENT_LAYER( sEventData->mEventId );
            sEventValue = KNL_GET_EVENT_VALUE( sEventData->mEventId );
            sEventTable = gLayerEventTables[sEventLayer];

            *aEventId = sEventData->mEventId;
            aEnv->mExecuteEvent = sEventData;

            STL_TRY( (sEventTable[sEventValue].mEventHandler)( sEventData->mData,
                                                               sEventData->mDataSize,
                                                               &sDone,
                                                               aEnv )
                     == STL_SUCCESS );

            aEnv->mExecuteEvent = NULL;

            if( (sDone == STL_FALSE) &&
                (sEventData->mFailBehavior == KNL_EVENT_FAIL_BEHAVIOR_CONTINUE) )
            {
                
                sEventData = STL_RING_GET_NEXT_DATA( &aEnv->mEvent.mEventHead,
                                                     sEventData );
                continue;
            }
        }
        else
        {
            *aEventId = sEventData->mEventId;
            *aIsCanceled = STL_TRUE;
            sDone = STL_TRUE;
        }

        break;
    }
    
    if( sDone == STL_TRUE )
    {
        /**
         * Event가 성공했음을 polling wait하는 session에 알린다.
         */

        if( sEventData->mWaitMode == KNL_EVENT_WAIT_POLLING )
        {
            KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv );
            
            stlAcquireSpinLock( &aEnv->mEvent.mSpinLock, &sDummy );
            sState = 1;

            if( sEventData->mWaitEnv != NULL )
            {
                stlAppendErrors( KNL_ERROR_STACK( sEventData->mWaitEnv ),
                                 KNL_ERROR_STACK( aEnv ) );
            }
            
            sState = 0;
            stlReleaseSpinLock( &aEnv->mEvent.mSpinLock );
            
            KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
        }
        
        aEnv->mEvent.mCompletedEsn = STL_MAX( aEnv->mEvent.mCompletedEsn,
                                              sEventData->mEsn );
        
        if( sEventData->mWaitMode == KNL_EVENT_WAIT_BLOCKED )
        {
            STL_DASSERT( sEventData->mWaitEnv != NULL );
        
            stlAppendErrors( KNL_ERROR_STACK( sEventData->mWaitEnv ),
                             KNL_ERROR_STACK( aEnv ) );
            STL_TRY( knlResume( KNL_ENV(sEventData->mWaitEnv),
                                aEnv ) == STL_SUCCESS );
        }

        STL_TRY( kneRemoveEvent( &aEnv->mEvent,
                                 sEventData,
                                 STL_TRUE,
                                 aEnv )
                 == STL_SUCCESS );

        STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );

        *aIsSuccess = STL_TRUE;
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlReleaseSpinLock( &aEnv->mEvent.mSpinLock );
            KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
        default:
            break;
    }
    
    if( sEventData->mWaitMode == KNL_EVENT_WAIT_POLLING )
    {
        KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv );
            
        stlAcquireSpinLock( &aEnv->mEvent.mSpinLock, &sDummy );
        sState = 1;

        if( sEventData->mWaitEnv != NULL )
        {
            stlAppendErrors( KNL_ERROR_STACK( sEventData->mWaitEnv ),
                             KNL_ERROR_STACK( aEnv ) );
        }
            
        sState = 0;
        stlReleaseSpinLock( &aEnv->mEvent.mSpinLock );
            
        KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
    }
        
    aEnv->mEvent.mCompletedEsn = STL_MAX( aEnv->mEvent.mCompletedEsn,
                                          sEventData->mEsn );
        
    if( sEventData->mWaitMode == KNL_EVENT_WAIT_BLOCKED )
    {
        stlAppendErrors( KNL_ERROR_STACK( sEventData->mWaitEnv ),
                         KNL_ERROR_STACK( aEnv ) );
        
        (void)knlResume( KNL_ENV(sEventData->mWaitEnv), aEnv );
    }

    (void)kneRemoveEvent( &aEnv->mEvent, sEventData, STL_TRUE, aEnv );

    STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );

    /**
     * Event Executor는 event excution이 실패하더라고 STL_SUCCESS를
     * 반환해야 한다.
     * <BR> event caller에게만 에러를 반환한다.
     */
    return STL_SUCCESS;
}

/**
 * @brief Process Event를 처리한다.
 * @param[in,out] aEnv environment 정보
 * @note 정상적으로 Event가 처리되면 처리된 Event는 삭제된다.
 * <BR> 해당 Process에 연결된 모든 Event를 처리한다.
 */
stlStatus knlExecuteProcessEvent( knlEnv * aEnv )
{
    kniProcessManager * sProcessManager;
    kniProcessInfo    * sProcessInfoArray;
    kniProcessInfo    * sProcessInfo;
    stlInt32            sState = 0;
    knlEvent          * sEvent;
    stlBool             sDone;
    kneEventData      * sEventData;
    kneEventData      * sNextEventData;
    stlInt32            sEventLayer;
    stlInt32            sEventValue;
    knlEventTable     * sEventTable;
    stlUInt64           sDummy = 0;
    
    STL_TRY_THROW( gKnEntryPoint->mProcessManager != NULL,
                   RAMP_ERR_INVALID_STATIC_SHM );
    STL_TRY_THROW( gProcessTableID != -1,
                   RAMP_ERR_INITIALIZED_YET );

    sProcessManager = gKnEntryPoint->mProcessManager;
    sProcessInfoArray = (void*)sProcessManager + STL_SIZEOF( kniProcessManager );
    sProcessInfo = &(sProcessInfoArray[gProcessTableID]);
    sEvent = &sProcessInfo->mEvent;

    /*
     * 이벤트가 존재할 경우
     */
    STL_TRY_THROW( sEvent->mEventCount > 0, RAMP_SUCCESS );

    KNL_ENTER_NON_CANCELLABLE_SECTION( aEnv );
    
    stlAcquireSpinLock( &sEvent->mSpinLock, &sDummy );
    sState = 1;

    if( sEvent->mEventCount <= 0 )
    {
        /*
         * 이미 이벤트가 반영된 경우
         */

        STL_ASSERT( sEvent->mEventCount == 0 );

        sState = 0;
        stlReleaseSpinLock( &sEvent->mSpinLock );
        
        KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
        
        STL_THROW( RAMP_SUCCESS );
    }

    STL_RING_FOREACH_ENTRY_SAFE( &sEvent->mEventHead,
                                 sEventData,
                                 sNextEventData )
    {
        sEventLayer = KNL_GET_EVENT_LAYER( sEventData->mEventId );
        sEventValue = KNL_GET_EVENT_VALUE( sEventData->mEventId );
        sEventTable = gLayerEventTables[sEventLayer];

        STL_TRY( (sEventTable[sEventValue].mEventHandler)( sEventData->mData,
                                                           sEventData->mDataSize,
                                                           &sDone,
                                                           aEnv )
                 == STL_SUCCESS );

        STL_TRY( kneRemoveEvent( sEvent,
                                 NULL,
                                 STL_FALSE,
                                 aEnv ) == STL_SUCCESS );
    }
    
    sState = 0;
    stlReleaseSpinLock( &sEvent->mSpinLock );

    KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
    
    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STATIC_SHM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_INITIALIZED_YET )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INITIALIZED_YET,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        stlReleaseSpinLock( &sEvent->mSpinLock );
        KNL_LEAVE_NON_CANCELLABLE_SECTION( aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Event Id에 해당하는 Description을 구한다.
 * @param[in] aEventId Event Id
 */
stlChar * knlGetEventDesc( stlInt32  aEventId )
{
    stlInt32        sEventLayer;
    stlInt32        sEventValue = -1;
    knlEventTable * sEventTable;

    if( aEventId < 0 )
    {
        return NULL;
    }
    else
    {
        sEventLayer = KNL_GET_EVENT_LAYER( aEventId );
        sEventValue = KNL_GET_EVENT_VALUE( aEventId );
        sEventTable = gLayerEventTables[sEventLayer];

        return sEventTable[sEventValue].mEventDesc;
    }
}

/** @} */
