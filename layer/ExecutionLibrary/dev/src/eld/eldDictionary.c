/*******************************************************************************
 * eldDictionary.c
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
 * @file eldDictionary.c
 * @brief Execution Layer 의 Dictionary 관리 공통 모듈 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <elDef.h>
#include <elgPendOp.h>
#include <eldIntegrity.h>
#include <eldDictionary.h>

/**
 * @addtogroup eldMemory
 * @{
 */

/**
 * @brief Dictionary Cache 구축 중인지의 여부
 */
stlBool gEldCacheBuilding = STL_FALSE;

/**
 * @brief Dictionary Table 들의 Physical Handle
 */
void ** gEldTablePhysicalHandle = NULL;

/**
 * @brief Dictionary Table 들의 Identity Column의 Handle
 */
void ** gEldIdentityColumnHandle = NULL;

/**
 * @brief DATA_TYPE_DESCRIPTOR.DTD_IDENTIFIER 에 구축된 index handle.
 * Start-Up 과정의 과부하를 줄이기 위해 Index Cache 구축전에 획득함.
 */
void * gEldNonCacheIndexPhyHandle4DTD = NULL;

/**
 * @brief VIEWS.TABLE_ID 에 구축된 index handle.
 * Start-Up 과정의 과부하를 줄이기 위해 Index Cache 구축전에 획득함.
 */
void * gEldNonCacheIndexPhyHandle4ViewedTableID = NULL;

/**
 * @brief COLUMNS.TABLE_ID 에 구축된 index handle.
 * Start-Up 과정의 과부하를 줄이기 위해 Index Cache 구축전에 획득함.
 */
void * gEldNonCacheIndexPhyHandle4ColTableID = NULL;

/**
 * @brief INDEX_KEY_COLUMN_USAGE.INDEX_ID 에 구축된 index handle.
 * Start-Up 과정의 과부하를 줄이기 위해 Index Cache 구축전에 획득함.
 */
void * gEldNonCacheIndexPhyHandle4IndexKeyColIndexID = NULL;

/**
 * @brief CHECK_CONSTRAINTS.CONSTRAINT_ID 에 구축된 index handle.
 * Start-Up 과정의 과부하를 줄이기 위해 Index Cache 구축전에 획득함.
 */
void * gEldNonCacheIndexPhyHandle4CheckConstID = NULL;

/**
 * @brief KEY_COLUMN_USAGE.CONSTRAINT_ID 에 구축된 index handle.
 * Start-Up 과정의 과부하를 줄이기 위해 Index Cache 구축전에 획득함.
 */
void * gEldNonCacheIndexPhyHandle4KeyColConstID = NULL;

/**
 * @brief REFERENTIAL_CONSTRAINTS.CONSTRAINT_ID 에 구축된 index handle.
 * Start-Up 과정의 과부하를 줄이기 위해 Index Cache 구축전에 획득함.
 */
void * gEldNonCacheIndexPhyHandle4RefConstID = NULL;

/**
 * @brief CHECK_COLUMN_USAGE.CONSTRAINT_ID 에 구축된 index handle.
 * Start-Up 과정의 과부하를 줄이기 위해 Index Cache 구축전에 획득함.
 */
void * gEldNonCacheIndexPhyHandle4ChkColConstID = NULL;

/**
 * @brief Dictionary Operation 을 위한 메모리 할당
 * @param[out] aAddr      메모리 주소 
 * @param[in]  aSize      할당받을 공간의 크기            
 * @param[in]  aEnv       Environment 포인터
 * @remarks
 */
stlStatus eldAllocOPMem( void ** aAddr, stlUInt32 aSize, ellEnv * aEnv )
{
    STL_PARAM_VALIDATE( aAddr != NULL, ELL_ERROR_STACK(aEnv) );

    STL_TRY( knlAllocRegionMem( & aEnv->mMemDictOP,
                                aSize,
                                aAddr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Dictionary Operation 를 위해 할당받은 모든 메모리를 해제한다. 
 * @param[in]  aEnv       Environment 포인터
 * @remarks
 */
stlStatus eldClearOPMem( ellEnv * aEnv )
{
    ellLongTypeBlock * sLongTypeBlock = aEnv->mLongTypeBlock;
    
    while( sLongTypeBlock != NULL )
    {
        STL_TRY( knlFreeLongVaryingValues( sLongTypeBlock->mValueBlockList,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sLongTypeBlock = sLongTypeBlock->mNext;
    }
    
    aEnv->mLongTypeBlock = NULL;
    
    STL_TRY( knlClearRegionMem( & aEnv->mMemDictOP, KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Pending Operation 을 위한 메모리 할당
 * @param[out] aAddr      메모리 주소 
 * @param[in]  aArgSize   Pending Argument Size
 * @param[in]  aEnv       Environment 포인터
 * @remarks
 */
stlStatus eldAllocPendingMem( void ** aAddr, stlUInt32 aArgSize, ellEnv * aEnv )
{
    STL_PARAM_VALIDATE( aAddr != NULL, ELL_ERROR_STACK(aEnv) );

    STL_TRY( knlAllocDynamicMem( ELL_MEM_PEND_OP,
                                 STL_SIZEOF(elgPendOP) + aArgSize,
                                 aAddr,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Pending Operation 를 위해 할당받은 모든 메모리를 해제한다.
 * @param[in]  aAddr      Memory 주소 
 * @param[in]  aEnv       Environment 포인터
 * @remarks
 * life cycle 단위 : Transaction 
 */
stlStatus eldClearPendingMem( void * aAddr, ellEnv * aEnv )
{
    STL_PARAM_VALIDATE( aAddr != NULL, ELL_ERROR_STACK(aEnv) );

    STL_TRY( knlFreeDynamicMem( ELL_MEM_PEND_OP,
                                aAddr,
                                (knlEnv*) aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Dictionary Cache 구성을 위한 메모리 할당
 * @param[out] aAddr      메모리 주소 
 * @param[in]  aSize      할당받을 공간의 크기            
 * @param[in]  aEnv       Environment 포인터
 * @remarks
 */
stlStatus eldAllocCacheMem( void ** aAddr, stlUInt32 aSize, ellEnv * aEnv )
{
    void * sMemory = NULL;

    elgPendOP * sPendBuffer = NULL;
    void      * sAgingEventBuffer = NULL;
    
    eldMemFreePendArg   sPendArg;
    
    stlMemset( & sPendArg, 0x00, STL_SIZEOF(eldMemFreePendArg) );
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aAddr != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Pending Operation 을 위한 Buffer 확보
     */
    
    STL_TRY( eldAllocPendingMem( (void **) & sPendBuffer,
                                 STL_SIZEOF(eldMemFreePendArg),
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Aging Event Buffer 확보
     */
    
    STL_TRY( knlAllocEnvEventMem( STL_SIZEOF(eldMemFreeAgingEvent),
                                  & sAgingEventBuffer,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * 메모리 할당
     */
    
    STL_TRY( knlAllocDynamicMem( ELL_MEM_DICT_CACHE,
                                 aSize,
                                 & sMemory,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Statement Rollback 을 위한 Pending Operation 등록
     * - 할당된 메모리를 해제한다.
     */

    sPendArg.mMemMgr = ELL_MEM_DICT_CACHE;
    sPendArg.mAddr   = sMemory;
    
    elgAddStmtRollbackFreeCacheMemPendOP( sPendBuffer,
                                          (void *) & sPendArg,
                                          STL_SIZEOF(eldMemFreePendArg),
                                          sAgingEventBuffer,
                                          aEnv );

    /**
     * Output 설정
     */

    *aAddr = sMemory;
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sPendBuffer, ELL_ENV(aEnv) );
        sPendBuffer = NULL;
    }

    if ( sAgingEventBuffer != NULL )
    {
        (void) knlFreeEnvEventMem( sAgingEventBuffer, KNL_ENV(aEnv) );
        sAgingEventBuffer = NULL;
    }
    
    if ( sMemory != NULL )
    {
        (void) knlFreeDynamicMem( ELL_MEM_DICT_CACHE, sMemory, KNL_ENV(aEnv) );
        sMemory = NULL;
    }
    
    return STL_FAILURE;
}




/**
 * @brief Dictionary Hash Element 구성을 위한 메모리 할당
 * @param[out] aAddr      메모리 주소 
 * @param[in]  aEnv       Environment 포인터
 * @remarks
 */
stlStatus eldAllocHashElementMem( eldcHashElement ** aAddr, ellEnv * aEnv )
{
    eldcHashElement * sMemory = NULL;

    elgPendOP * sPendBuffer = NULL;
    void      * sAgingEventBuffer = NULL;
    
    eldMemFreePendArg   sPendArg;
    
    stlMemset( & sPendArg, 0x00, STL_SIZEOF(eldMemFreePendArg) );
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aAddr != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Pending Operation 을 위한 Buffer 확보
     */
    
    STL_TRY( eldAllocPendingMem( (void **) & sPendBuffer,
                                 STL_SIZEOF(eldMemFreePendArg),
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Aging Event Buffer 확보
     */
    
    STL_TRY( knlAllocEnvEventMem( STL_SIZEOF(eldMemFreeAgingEvent),
                                  & sAgingEventBuffer,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * 메모리 할당
     */
    
    STL_TRY( knlAllocDynamicMem( ELL_MEM_HASH_ELEMENT,
                                 STL_SIZEOF(eldcHashElement),
                                 (void **) & sMemory,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sMemory->mVersionInfo.mMemUseCnt++;
    STL_DASSERT( (sMemory->mVersionInfo.mMemUseCnt % 2) == 1 );
    
    /**
     * Statement Rollback 을 위한 Pending Operation 등록
     * - 할당된 메모리를 해제한다.
     */

    sPendArg.mMemMgr = ELL_MEM_HASH_ELEMENT;
    sPendArg.mAddr   = sMemory;
    
    elgAddStmtRollbackFreeCacheMemPendOP( sPendBuffer,
                                          (void *) & sPendArg,
                                          STL_SIZEOF(eldMemFreePendArg),
                                          sAgingEventBuffer,
                                          aEnv );

    /**
     * Output 설정
     */

    *aAddr = sMemory;
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sPendBuffer, ELL_ENV(aEnv) );
        sPendBuffer = NULL;
    }

    if ( sAgingEventBuffer != NULL )
    {
        (void) knlFreeEnvEventMem( sAgingEventBuffer, KNL_ENV(aEnv) );
        sAgingEventBuffer = NULL;
    }
    
    if ( sMemory != NULL )
    {
        sMemory->mVersionInfo.mMemUseCnt++;
        (void) knlFreeDynamicMem( ELL_MEM_HASH_ELEMENT, sMemory, KNL_ENV(aEnv) );
        sMemory = NULL;
    }
    
    return STL_FAILURE;
}






/**
 * @brief Dictionary Hash Related Object 구성을 위한 메모리 할당
 * @param[out] aAddr      메모리 주소 
 * @param[in]  aEnv       Environment 포인터
 * @remarks
 */
stlStatus eldAllocHashRelatedMem( eldcHashRelatedObjList ** aAddr, ellEnv * aEnv )
{
    eldcHashRelatedObjList * sMemory = NULL;

    elgPendOP * sPendBuffer = NULL;
    void      * sAgingEventBuffer = NULL;
    
    eldMemFreePendArg   sPendArg;
    
    stlMemset( & sPendArg, 0x00, STL_SIZEOF(eldMemFreePendArg) );
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aAddr != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Pending Operation 을 위한 Buffer 확보
     */
    
    STL_TRY( eldAllocPendingMem( (void **) & sPendBuffer,
                                 STL_SIZEOF(eldMemFreePendArg),
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Aging Event Buffer 확보
     */
    
    STL_TRY( knlAllocEnvEventMem( STL_SIZEOF(eldMemFreeAgingEvent),
                                  & sAgingEventBuffer,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * 메모리 할당
     */
    
    STL_TRY( knlAllocDynamicMem( ELL_MEM_HASH_RELATED,
                                 STL_SIZEOF(eldcHashRelatedObjList),
                                 (void **) & sMemory,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sMemory->mVersionInfo.mMemUseCnt++;
    STL_DASSERT( (sMemory->mVersionInfo.mMemUseCnt % 2) == 1 );
    
    /**
     * Statement Rollback 을 위한 Pending Operation 등록
     * - 할당된 메모리를 해제한다.
     */

    sPendArg.mMemMgr = ELL_MEM_HASH_RELATED;
    sPendArg.mAddr   = sMemory;
    
    elgAddStmtRollbackFreeCacheMemPendOP( sPendBuffer,
                                          (void *) & sPendArg,
                                          STL_SIZEOF(eldMemFreePendArg),
                                          sAgingEventBuffer,
                                          aEnv );

    /**
     * Output 설정
     */

    *aAddr = sMemory;
        
    return STL_SUCCESS;

    STL_FINISH;

    if ( sPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sPendBuffer, ELL_ENV(aEnv) );
        sPendBuffer = NULL;
    }

    if ( sAgingEventBuffer != NULL )
    {
        (void) knlFreeEnvEventMem( sAgingEventBuffer, KNL_ENV(aEnv) );
        sAgingEventBuffer = NULL;
    }
    
    if ( sMemory != NULL )
    {
        sMemory->mVersionInfo.mMemUseCnt++;
        (void) knlFreeDynamicMem( ELL_MEM_HASH_RELATED,
                                  sMemory,
                                  KNL_ENV(aEnv) );
        sMemory = NULL;
    }
    
    return STL_FAILURE;
}



/**
 * @brief Dictionary Cache 구성을 위해 할당한 메모리를 해제하는 Aging Event 를 등록
 * @param[in] aTransID     Transaction ID
 * @param[in] aAgingEvent  Aging Event
 * @param[in] aEventBuffer Aging Event Buffer
 * @param[in] aEnv         Environment 포인터
 * @remarks
 * 실제 메모리를 해제하지 않고 Ager에 등록함.
 */
stlStatus eldAddAgingEventDictMem( smlTransId     aTransID,
                                   void         * aAgingEvent,
                                   void         * aEventBuffer,
                                   ellEnv       * aEnv )
{
    eldMemFreeAgingEvent * sEvent = NULL;
    stlChar                sDataBuffer[STL_SIZEOF(eldMemFreeAgingEvent)];
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aAgingEvent != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aEventBuffer != NULL, ELL_ERROR_STACK(aEnv) );

    sEvent = (eldMemFreeAgingEvent *) aAgingEvent;
    
    /**
     * Aging Event를 추가한다.
     */
    
    sEvent->mSCN = smlGetNextSystemScn();

    /**
     * Aging Parameter Marshalling
     */

    stlMemcpy( sDataBuffer, aAgingEvent, STL_SIZEOF(eldMemFreeAgingEvent) );

    /*
     * Aging Env에 Aging Event 추가
     */
    STL_TRY( knlAddEnvEvent( ELG_EVENT_FREE_DICTIONARY_MEMORY,
                             aEventBuffer, /* aEventMem */
                             (void*)sDataBuffer,
                             STL_SIZEOF(eldMemFreeAgingEvent),
                             smlGetAgerEnvId(),
                             ( KNL_EVENT_WAIT_NO |
                               KNL_EVENT_FAIL_BEHAVIOR_CONTINUE ),
                             STL_TRUE, /* aForceSuccess */
                             KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Ager에 의해서 물리적으로 Dictionary Memory를 해제한다.
 * @param[in]     aData      Aging 파라미터 정보
 * @param[in]     aDataSize  Aging 파라미터 정보 크기
 * @param[out]    aDone      성공여부
 * @param[in,out] aEnv       Environment 정보
 */
stlStatus eldAgingDictMem( void      * aData,
                           stlUInt32   aDataSize,
                           stlBool   * aDone,
                           void      * aEnv )
{
    eldMemFreeAgingEvent    * sEvent;
    
    eldcHashElement         * sHashElement = NULL;
    eldcHashElement         * sCurrElement = NULL;
    eldcHashElement         * sPrevElement = NULL;
    
    eldcHashRelatedObjList  * sHashRelated = NULL;
    eldcHashRelatedObjList ** sStartRelated = NULL;
    eldcHashRelatedObjList  * sCurrRelated = NULL;
    eldcHashRelatedObjList  * sPrevRelated = NULL;
    
    eldcHashBucket          * sBucket = NULL;
    knlDynamicMem           * sMemMgr = NULL;
    void                    * sAddr = NULL;
    stlBool                   sTimeout  = STL_FALSE;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aData != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDone != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Aging Parameter Unmarshalling
     */

    sEvent = (eldMemFreeAgingEvent*) aData;
    sPrevElement = NULL;
    
    *aDone = STL_FALSE;
    
    if( (smlIsAgableByViewScn( sEvent->mSCN, SML_ENV( aEnv ) ) == STL_TRUE) ||
        (sEvent->mSCN == SML_INFINITE_SCN) )
    {
        /**
         * Agable SCN 이거나 ROLLBACK TO SAVEPOINT 에 의해 반납된 경우
         */

        switch( sEvent->mEventType )
        {
            case ELD_AGING_FREE_DICTIONARY_MEMORY:

                sMemMgr = sEvent->mArg1;
                sAddr   = sEvent->mArg2;
                
                if ( sMemMgr == ELL_MEM_HASH_ELEMENT )
                {
                    sHashElement = (eldcHashElement *) sAddr;
                
                    STL_DASSERT( (sHashElement->mVersionInfo.mMemUseCnt % 2) == 1 );
                    sHashElement->mVersionInfo.mMemUseCnt++;
                }
                else if ( sMemMgr == ELL_MEM_HASH_RELATED )
                {
                    sHashRelated = (eldcHashRelatedObjList*) sAddr;
                
                    STL_DASSERT( (sHashRelated->mVersionInfo.mMemUseCnt % 2) == 1 );
                    sHashRelated->mVersionInfo.mMemUseCnt++;
                }
                else
                {
                    /* nothing to do */
                }

                /**
                 * Memory 해제
                 */
            
                STL_TRY( knlFreeDynamicMem( sMemMgr,
                                            sAddr,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                *aDone = STL_TRUE;
        
                break;

            case ELD_AGING_UNLINK_HASH_ELEMENT:

                /**
                 * Logical Aging of Hash Element
                 */
                
                sBucket      = sEvent->mArg1;
                sHashElement = sEvent->mArg2;
                
                STL_TRY( knlAcquireLatch( & sBucket->mInsDelLatch,
                                          KNL_LATCH_MODE_EXCLUSIVE,
                                          KNL_LATCH_PRIORITY_NORMAL,
                                          STL_INFINITE_TIME,   /* interval */
                                          & sTimeout,
                                          KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                for ( sCurrElement = sBucket->mElement;
                      sCurrElement != NULL;
                      sCurrElement = sCurrElement->mNext )
                {
                    if ( sCurrElement == sHashElement )
                    {
                        break;
                    }
        
                    sPrevElement = sCurrElement;
                }
                STL_ASSERT( sCurrElement != NULL );

                /**
                 * 연결 해제
                 */
    
                if ( sPrevElement == NULL )
                {
                    sBucket->mElement = sCurrElement->mNext;
                }
                else
                {
                    sPrevElement->mNext = sCurrElement->mNext;
                }
                
                STL_TRY( knlReleaseLatch( & sBucket->mInsDelLatch,
                                          KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * Event를 Physical Aging Event 로 전환한다.
                 */
                
                sEvent->mEventType = ELD_AGING_FREE_HASH_ELEMENT;
                sEvent->mSCN = smlGetNextSystemScn();
                
                break;
                
            case ELD_AGING_UNLINK_HASH_RELATED:
                
                /**
                 * Logical Aging of Hash Related
                 */
                
                sHashElement  = sEvent->mArg1;
                sStartRelated = sEvent->mArg2;
                sHashRelated  = sEvent->mArg3;
                
                STL_TRY( knlAcquireLatch( & sHashElement->mWriteLatch,
                                          KNL_LATCH_MODE_EXCLUSIVE,
                                          KNL_LATCH_PRIORITY_NORMAL,
                                          STL_INFINITE_TIME,   /* interval */
                                          & sTimeout,
                                          KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                for ( sCurrRelated = *(sStartRelated);
                      sCurrRelated != NULL;
                      sCurrRelated = sCurrRelated->mNext )
                {
                    if ( sCurrRelated == sHashRelated )
                    {
                        break;
                    }
        
                    sPrevRelated = sCurrRelated;
                }
                STL_ASSERT( sCurrRelated != NULL );
                
                /**
                 * 연결 해제
                 */
    
                if ( sPrevRelated == NULL )
                {
                    *(sStartRelated) = sCurrRelated->mNext;
                }
                else
                {
                    sPrevRelated->mNext = sCurrRelated->mNext;
                }

                STL_TRY( knlReleaseLatch( & sHashElement->mWriteLatch,
                                          KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                /**
                 * Event를 Physical Aging Event 로 전환한다.
                 */
                
                sEvent->mEventType = ELD_AGING_FREE_HASH_RELATED;
                sEvent->mSCN = smlGetNextSystemScn();
                
                break;
                
            case ELD_AGING_FREE_HASH_ELEMENT:

                /**
                 * Physical Aging of Hash Element
                 */
                
                sHashElement = sEvent->mArg2;

                if( sHashElement->mObjectDesc != NULL )
                {
                    STL_TRY( knlFreeDynamicMem( ELL_MEM_DICT_CACHE,
                                                sHashElement->mObjectDesc,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }

                STL_DASSERT( (sHashElement->mVersionInfo.mMemUseCnt % 2) == 1 );
                sHashElement->mVersionInfo.mMemUseCnt++;
                    
                STL_TRY( knlFreeDynamicMem( ELL_MEM_DICT_CACHE,
                                            sHashElement->mHashData,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_TRY( knlFreeDynamicMem( ELL_MEM_HASH_ELEMENT,
                                            sHashElement,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                *aDone = STL_TRUE;
                
                break;
                
            case ELD_AGING_FREE_HASH_RELATED:
                
                /**
                 * Physical Aging of Hash Related
                 */
                
                sHashRelated = sEvent->mArg3;

                STL_DASSERT( (sHashRelated->mVersionInfo.mMemUseCnt % 2) == 1 );
                sHashRelated->mVersionInfo.mMemUseCnt++;
                    
                STL_TRY( knlFreeDynamicMem( ELL_MEM_HASH_RELATED,
                                            sHashRelated,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                *aDone = STL_TRUE;
                
                break;
                
            default:
                STL_DASSERT( 0 );
                break;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    /**
     * Aging이 실패하면 memory leak을 허용한다.
     */
    
    *aDone = STL_TRUE;

    return STL_FAILURE;
}
    



/**
 * @brief Dictionary Column 의 Precision 값을 얻는다.
 * @param[in]  aDomainNO   Dictionary Column 의 Domain NO
 * @return
 * Precision 
 * @remarks
 */
stlInt64 eldGetPrecisionFromDomain( eldtDefinitionDomainNO  aDomainNO )
{
    stlInt64    sPrecision = 0;
    
    eldtDefinitionDomainDesc * sDomainDesc = NULL;

    sDomainDesc = eldtGetDictDomainDesc( aDomainNO );
    
    switch ( aDomainNO )
    {
        case ELDT_DOMAIN_SQL_IDENTIFIER:
            {
                sPrecision = sDomainDesc->mLength;
                break;
            }
        case ELDT_DOMAIN_SHORT_DESC:
            {
                sPrecision = sDomainDesc->mLength;
                break;
            }
        case ELDT_DOMAIN_LONG_DESC:
            {
                sPrecision = sDomainDesc->mLength;
                break;
            }
        case ELDT_DOMAIN_SQL_SYNTAX:
            {
                sPrecision = sDomainDesc->mLength;
                break;
            }
        case ELDT_DOMAIN_CARDINAL_NUMBER:
            {
                sPrecision = sDomainDesc->mPrecision;
                break;
            }
        case ELDT_DOMAIN_BIG_NUMBER:
            {
                sPrecision = sDomainDesc->mPrecision;
                break;
            }
        case ELDT_DOMAIN_SERIAL_NUMBER:
            {
                sPrecision = sDomainDesc->mPrecision;
                break;
            }
    
        case ELDT_DOMAIN_YES_OR_NO:
            {
                sPrecision = sDomainDesc->mPrecision;
                break;
            }
    
        case ELDT_DOMAIN_TIME_STAMP:
            {
                sPrecision = sDomainDesc->mPrecision;
                break;
            }
        case ELDT_DOMAIN_INTERVAL:
            {
                sPrecision = sDomainDesc->mPrecision;
                break;
            }
        case ELDT_DOMAIN_FLOATING_NUMBER:
            {
                sPrecision = sDomainDesc->mPrecision;
                break;
            }
        default:
            {
                /* Invalid Domain */
                STL_ASSERT( 0 );
                break;
            }
    }

    /**
     * Output 설정
     */

    return sPrecision;
}

/**
 * @brief Dictionary Table Value List 로부터 Data Value 를 얻는다.
 * @param[in]  aTableValueList Dictionary Table Value List
 * @param[in]  aTableID        Dictionary Table ID
 * @param[in]  aColumnOrder    Column Order
 * @return
 * Data Value
 * @remarks
 */
dtlDataValue * eldFindDataValue( knlValueBlockList * aTableValueList,
                                 stlInt64            aTableID,
                                 stlInt32            aColumnOrder )
{
    knlValueBlockList * sColumnValue = NULL;

    sColumnValue = ellFindColumnValueList( aTableValueList,
                                           aTableID,
                                           aColumnOrder );

    return KNL_GET_BLOCK_DATA_VALUE( sColumnValue, 0 );
}

/**
 * @brief Definition Table Descriptor 를 위한 Value 공간을 할당받는다.
 * @param[in]  aDictTableID     Dictionary Table 의 ID
 * @param[out] aMemMark         Memory Mark 포인터
 * @param[out] aTableValueList  Value List를 구성할 공간 
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldAllocTableValueList( eldtDefinitionTableID     aDictTableID,
                                  eldMemMark              * aMemMark,
                                  knlValueBlockList      ** aTableValueList,
                                  ellEnv                  * aEnv )
{
    stlInt32           i = 0;
    
    stlInt32                   sColumnCnt  = 0;
    
    knlValueBlockList        * sTableValueList = NULL;
    knlValueBlockList        * sValueList = NULL;
    knlValueBlockList        * sPrevValueList = NULL;
    ellLongTypeBlock         * sLongTypeBlock;

    eldtDefinitionColumnDesc * sColumnArrayDesc = NULL;
    eldtDefinitionDomainDesc * sDomainDesc = NULL;

    stlInt32   sState = 0;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( ( (aDictTableID > ELDT_TABLE_ID_NA) &&
                          (aDictTableID < ELDT_TABLE_ID_MAX) ),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMemMark != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Memory Mark 획득 
     */

    KNL_INIT_REGION_MARK( &(aMemMark->mMemMark) );
    
    aMemMark->mLongTypeBlock = aEnv->mLongTypeBlock;

    STL_TRY( knlMarkRegionMem( & aEnv->mMemDictOP,
                               & aMemMark->mMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;
    
    /**
     * Column 개수 획득 
     */
    
    STL_TRY( eldtGetDictTableColumnCnt( aDictTableID,
                                        & sColumnCnt,
                                        aEnv )
             == STL_SUCCESS );

    /**
     * Value List를 위한 정보를 설정한다.
     * Data Type 별로 필요한 공간을 할당받는다.
     * Value List의 연결 관계를 만들어 준다.
     */

    STL_TRY( eldtGetDictColumnDescArray( aDictTableID,
                                         & sColumnArrayDesc,
                                         aEnv )
             == STL_SUCCESS );
    
    for ( i = 0; i < sColumnCnt; i++ )
    {
        /**
         * 컬럼의 Domain Descriptor 획득
         */
        
        sDomainDesc = eldtGetDictDomainDesc( sColumnArrayDesc[i].mDomainNO );

        /**
         * Value Block 생성
         */

        STL_TRY( knlInitBlock( & sValueList,
                               KNL_NO_BLOCK_READ_CNT,
                               STL_TRUE, /* 컬럼임 */
                               STL_LAYER_EXECUTION_LIBRARY,
                               aDictTableID, /* Table ID 지정이 의미가 없음 */
                               i,
                               sDomainDesc->mDataTypeID,
                               eldGetPrecisionFromDomain(sDomainDesc->mDomainNO),
                               DTL_SCALE_NA,
                               DTL_STRING_LENGTH_UNIT_OCTETS,
                               ELDT_DOMAIN_INTERVAL_INDICATOR,
                               & aEnv->mMemDictOP,
                               KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( (sDomainDesc->mDataTypeID == DTL_TYPE_LONGVARCHAR) ||
            (sDomainDesc->mDataTypeID == DTL_TYPE_LONGVARBINARY) )
        {
            /**
             * Long Type Block들을 연결한다.
             */
            
            STL_TRY( knlAllocRegionMem( & aEnv->mMemDictOP,
                                        STL_SIZEOF(ellLongTypeBlock),
                                        (void **) & sLongTypeBlock,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sLongTypeBlock->mValueBlockList = sValueList;
            sLongTypeBlock->mNext = aEnv->mLongTypeBlock;
            
            aEnv->mLongTypeBlock = sLongTypeBlock;
        }
        
        /**
         * Link 설정 
         */

        if ( sTableValueList == NULL )
        {
            sTableValueList = sValueList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
        }

        sPrevValueList = sValueList;
    }

    /**
     * Output 설정
     */
    
    *aTableValueList = sTableValueList;

    sState = 0;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( aMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}



/**
 * @brief Definition Table Descriptor 를 위한 Value 공간을 해제한다.
 * @param[in]  aMemMark         Memory Mark 포인터
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldFreeTableValueList( eldMemMark  * aMemMark,
                                 ellEnv      * aEnv )
{
    ellLongTypeBlock * sLongTypeBlock = aEnv->mLongTypeBlock;
    
    while( sLongTypeBlock != aMemMark->mLongTypeBlock )
    {
        STL_TRY( knlFreeLongVaryingValues( sLongTypeBlock->mValueBlockList,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sLongTypeBlock = sLongTypeBlock->mNext;
    }

    aEnv->mLongTypeBlock = aMemMark->mLongTypeBlock;
    
    STL_TRY( knlFreeUnmarkedRegionMem( & aEnv->mMemDictOP,
                                       & aMemMark->mMemMark,
                                       STL_FALSE,  /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Definition Key Constraint Descriptor 로부터 인덱스를 위한 Value List 를 생성한다.
 * @param[in]  aKeyConstDesc    Definition Key Constraint Descriptor
 * @param[in]  aTableValueList  Index 가 참조하는 Table 의 Value Array
 * @param[out] aIndexValueList  Index 를 위한 Value Array 공간 
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldShareKeyConstIndexValueList( eldtDefinitionKeyConstDesc * aKeyConstDesc,
                                          knlValueBlockList          * aTableValueList,
                                          knlValueBlockList         ** aIndexValueList,
                                          ellEnv                     * aEnv )
{
    stlInt32           i = 0;
    
    stlInt32                   sColumnCnt  = 0;
    
    knlValueBlockList        * sDataValueList = NULL;
    knlValueBlockList        * sValueList = NULL;
    knlValueBlockList        * sPrevValueList = NULL;
    
    eldtDefinitionColumnDesc * sColumnArrayDesc = NULL;
    knlValueBlockList        * sColumnValue = NULL;

    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aKeyConstDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexValueList != NULL, ELL_ERROR_STACK(aEnv) );
    

    /**
     * Key Constraint의 Key 개수만큼 Value List 공간을 할당받는다.
     * Table Value List 와 Data Value 공간은 공유하므로 별도로 할당받지 않는다.
     */

    sColumnCnt = aKeyConstDesc->mKeyCount;

    /**
     * Base Table 의 컬럼 정보를 얻는다.
     */

    STL_TRY( eldtGetDictColumnDescArray( aKeyConstDesc->mTableID,
                                         & sColumnArrayDesc,
                                         aEnv )
             == STL_SUCCESS );
    
    for ( i = 0; i < sColumnCnt; i++ )
    {
        /**
         * Value List 의 공간을 할당하지 않고 Table Value List 의 공간을 연결한다.
         */

        sColumnValue = ellFindColumnValueList( aTableValueList,
                                               aKeyConstDesc->mTableID,
                                               aKeyConstDesc->mColOrder[i] );

        STL_TRY( knlInitShareBlock( & sValueList,
                                    sColumnValue,
                                    & aEnv->mMemDictOP,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Link 설정 
         */

        if ( sDataValueList == NULL )
        {
            sDataValueList = sValueList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
        }

        sPrevValueList = sValueList;
    }

    *aIndexValueList = sDataValueList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Definition Index Descriptor 로부터 인덱스를 위한 Value List 를 생성한다.
 * @param[in]  aIndexDesc       Definition Index Descriptor
 * @param[in]  aTableValueList  Index 가 참조하는 Table 의 Value List
 * @param[out] aIndexValueList  Index 를 위한 Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldShareNormalIndexValueList( eldtDefinitionIndexDesc * aIndexDesc,
                                        knlValueBlockList       * aTableValueList,
                                        knlValueBlockList      ** aIndexValueList,
                                        ellEnv                  * aEnv )
{
    stlInt32           i = 0;
    
    stlInt32                   sColumnCnt  = 0;
    eldtDefinitionColumnDesc * sColumnArrayDesc = NULL;

    knlValueBlockList * sDataValueList = NULL;
    knlValueBlockList * sValueList = NULL;
    knlValueBlockList * sPrevValueList = NULL;

    knlValueBlockList * sColumnValue = NULL;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aIndexDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexValueList != NULL, ELL_ERROR_STACK(aEnv) );


    /**
     * Table Value Array 와 Data Value 를 공유하므로 Data Value 공간은 할당받지 않는다.
     */

    sColumnCnt = aIndexDesc->mKeyCount;

    /**
     * Base Table 의 컬럼 정보를 얻는다.
     */

    STL_TRY( eldtGetDictColumnDescArray( aIndexDesc->mTableID,
                                         & sColumnArrayDesc,
                                         aEnv )
             == STL_SUCCESS );
    
    for ( i = 0; i < sColumnCnt; i++ )
    {
        /**
         * Column Value 를 검색 
         */
        
        sColumnValue = ellFindColumnValueList( aTableValueList,
                                               aIndexDesc->mTableID,
                                               aIndexDesc->mColOrder[i] );
        /**
         * Table Value List 와 공유
         */

        STL_TRY( knlInitShareBlock( & sValueList,
                                    sColumnValue,
                                    & aEnv->mMemDictOP,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * Link 설정
         */

        if ( sDataValueList == NULL )
        {
            sDataValueList = sValueList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
        }

        sPrevValueList = sValueList;
    }

    *aIndexValueList = sDataValueList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Fixed Table Descriptor 를 위한 Value 공간을 할당받는다.
 * @param[in]  aFxTableHeapDesc Fixed Table 의 Heap Descriptor
 * @param[out] aMemMark         Memory Mark 포인터
 * @param[out] aTableValueList  Value List를 구성할 공간 
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldAllocFixedValueList( void               * aFxTableHeapDesc,
                                  eldMemMark         * aMemMark,
                                  knlValueBlockList ** aTableValueList,
                                  ellEnv             * aEnv )
{
    stlInt32           i = 0;
    
    dtlDataType        sDataType = DTL_TYPE_MAX;
    
    knlValueBlockList * sDataValueList = NULL;
    knlValueBlockList * sValueList = NULL;
    knlValueBlockList * sPrevValueList = NULL;
    ellLongTypeBlock  * sLongTypeBlock = NULL;
    
    stlInt32           sColumnCnt  = 0;
    knlFxColumnDesc  * sFxColumnArrayDesc = NULL;

    stlInt32   sState = 0;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableValueList != NULL  , ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMemMark != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Memory Mark 획득 
     */

    KNL_INIT_REGION_MARK( &(aMemMark->mMemMark) );
    
    aMemMark->mLongTypeBlock = aEnv->mLongTypeBlock;

    STL_TRY( knlMarkRegionMem( & aEnv->mMemDictOP,
                               & aMemMark->mMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;
    
    /**
     * Fixed Table 의 컬럼 개수를 획득
     */
    
    STL_TRY( knlGetFixedColumnCount( aFxTableHeapDesc,
                                     & sColumnCnt,
                                     KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sColumnCnt > 0 );

    /**
     * Fixed Table 의 Column Descriptor 를 획득
     */

    STL_TRY( knlGetFixedColumnDesc( aFxTableHeapDesc,
                                    & sFxColumnArrayDesc,
                                    KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sFxColumnArrayDesc != NULL );

    for ( i = 0; i < sColumnCnt; i++ )
    {
        /**
         * Fixed Column 의 DB Type 을 획득
         */
        
        STL_TRY( knlGetDataTypeIDOfFixedColumn( & sFxColumnArrayDesc[i],
                                                & sDataType,
                                                KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Value Block을 생성
         */
        
        STL_TRY( knlInitBlock( & sValueList,
                               KNL_NO_BLOCK_READ_CNT,
                               STL_TRUE, /* 컬럼임 */
                               STL_LAYER_EXECUTION_LIBRARY,
                               KNL_ANONYMOUS_TABLE_ID, /* Table ID 지정이 의미가 없음 */
                               i,
                               sDataType,
                               sFxColumnArrayDesc[i].mLength,
                               gDefaultDataTypeDef[sDataType].mArgNum2, // _NA
                               gDefaultDataTypeDef[sDataType].mStringLengthUnit,  // _NA
                               gDefaultDataTypeDef[sDataType].mIntervalIndicator, // _NA
                               & aEnv->mMemDictOP,
                               KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( (sDataType == DTL_TYPE_LONGVARCHAR) ||
            (sDataType == DTL_TYPE_LONGVARBINARY) )
        {
            /**
             * Long Type Block들을 연결한다.
             */
            
            STL_TRY( knlAllocRegionMem( & aEnv->mMemDictOP,
                                        STL_SIZEOF(ellLongTypeBlock),
                                        (void **) & sLongTypeBlock,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sLongTypeBlock->mValueBlockList = sValueList;
            sLongTypeBlock->mNext = aEnv->mLongTypeBlock;
            
            aEnv->mLongTypeBlock = sLongTypeBlock;
        }
        
        /**
         * Link 설정 
         */
        
        if ( sDataValueList == NULL )
        {
            sDataValueList = sValueList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
        }

        sPrevValueList = sValueList;
    }

    *aTableValueList = sDataValueList;

    sState = 0;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( aMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}



/**
 * @brief 컬럼 정보를 이용해 Data Value List 정보를 설정한다.
 * @param[in]  aDictTableID     Dictionary Table의 ID
 * @param[in]  aColumnOrder     컬럼 순서 
 * @param[in]  aValueList       Data Value List 공간
 * @param[in]  aInputData       입력 Data Pointer
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldSetColumnValue( eldtDefinitionTableID      aDictTableID,
                             stlInt32                   aColumnOrder,
                             knlValueBlockList        * aValueList,
                             void                     * aInputData,
                             ellEnv                   * aEnv )
{

    stlInt64     sInt64Value = 0;
    stlBool      sBoolValue  = STL_FALSE;
    stlChar    * sBoolString = NULL;
    stlBool      sSuccessWithInfo = STL_FALSE;

    knlValueBlockList * sValueBlock = NULL;
    dtlDataValue      * sDataValue = NULL;
    dtlDataValue      * sNumberValue = NULL;

    eldtDefinitionColumnDesc * sColumnArrayDesc = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( ( (aDictTableID > ELDT_TABLE_ID_NA) && (aDictTableID < ELDT_TABLE_ID_MAX) ),
                        ELL_ERROR_STACK(aEnv) );
    
    STL_PARAM_VALIDATE( aColumnOrder >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Column 정보 획득 
     */
    
    STL_TRY( eldtGetDictColumnDescArray( aDictTableID,
                                         & sColumnArrayDesc,
                                         aEnv )
             == STL_SUCCESS );
    
    /**
     * Domain 에 따라 C 값을 DB 값으로 셋팅한다.
     */

    sValueBlock = ellFindColumnValueList( aValueList, sColumnArrayDesc->mTableID, aColumnOrder );
    sDataValue  = KNL_GET_BLOCK_DATA_VALUE( sValueBlock, 0 );
    
    if ( aInputData == NULL )
    {
        DTL_SET_NULL( sDataValue );
    }
    else
    {
        switch ( sColumnArrayDesc[aColumnOrder].mDomainNO )
        {
            case ELDT_DOMAIN_SQL_IDENTIFIER:
                {
                    STL_TRY( dtlSetValueFromString(
                                 sDataValue->mType,
                                 (stlChar *) aInputData,
                                 stlStrlen((stlChar *) aInputData),
                                 ELDT_DOMAIN_PRECISION_SQL_IDENTIFIER, 
                                 DTL_SCALE_NA,
                                 DTL_STRING_LENGTH_UNIT_OCTETS,
                                 DTL_INTERVAL_INDICATOR_NA,
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValueBlock), 
                                 sDataValue,
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 ELL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case ELDT_DOMAIN_SHORT_DESC:
                {
                    STL_TRY( dtlSetValueFromString(
                                 sDataValue->mType,
                                 (stlChar *) aInputData,
                                 stlStrlen((stlChar *) aInputData),
                                 ELDT_DOMAIN_PRECISION_SHORT_DESC, 
                                 DTL_SCALE_NA,
                                 DTL_STRING_LENGTH_UNIT_OCTETS,
                                 DTL_INTERVAL_INDICATOR_NA,
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValueBlock), 
                                 sDataValue,
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 ELL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case ELDT_DOMAIN_LONG_DESC:
                {
                    STL_TRY( dtlSetValueFromString(
                                 sDataValue->mType,
                                 (stlChar *) aInputData,
                                 stlStrlen((stlChar *) aInputData),
                                 ELDT_DOMAIN_PRECISION_LONG_DESC, 
                                 DTL_SCALE_NA,
                                 DTL_STRING_LENGTH_UNIT_OCTETS,
                                 DTL_INTERVAL_INDICATOR_NA,
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValueBlock), 
                                 sDataValue,
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 ELL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                    
                    break;
                }
            case ELDT_DOMAIN_SQL_SYNTAX:
                {
                    STL_TRY( dtlSetValueFromString(
                                 sDataValue->mType,
                                 (stlChar *) aInputData,
                                 stlStrlen((stlChar *) aInputData),
                                 ELDT_DOMAIN_PRECISION_SQL_SYNTAX, 
                                 DTL_SCALE_NA,
                                 DTL_STRING_LENGTH_UNIT_OCTETS,
                                 DTL_INTERVAL_INDICATOR_NA,
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValueBlock), 
                                 sDataValue,
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 ELL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                    
                    break;
                }
            case ELDT_DOMAIN_CARDINAL_NUMBER:
                {
                    sInt64Value = *(stlInt32 *) aInputData;
                
                    STL_TRY( dtlSetValueFromInteger(
                                 sDataValue->mType,
                                 sInt64Value,
                                 DTL_PRECISION_NA,                                 
                                 DTL_SCALE_NA,
                                 DTL_STRING_LENGTH_UNIT_NA,
                                 DTL_INTERVAL_INDICATOR_NA,
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValueBlock), 
                                 sDataValue,
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 ELL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                    
                    break;
                }
            case ELDT_DOMAIN_BIG_NUMBER:
                {
                    sInt64Value = *(stlInt64 *) aInputData;
                
                    STL_TRY( dtlSetValueFromInteger(
                                 sDataValue->mType,
                                 sInt64Value,
                                 DTL_PRECISION_NA, 
                                 DTL_SCALE_NA,
                                 DTL_STRING_LENGTH_UNIT_NA,
                                 DTL_INTERVAL_INDICATOR_NA,
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValueBlock), 
                                 sDataValue,
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 ELL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );

                    break;
                }
            case ELDT_DOMAIN_SERIAL_NUMBER:
                {
                    sInt64Value = *(stlInt64 *) aInputData;
                
                    STL_TRY( dtlSetValueFromInteger(
                                 sDataValue->mType,
                                 sInt64Value,
                                 DTL_PRECISION_NA, 
                                 DTL_SCALE_NA,
                                 DTL_STRING_LENGTH_UNIT_NA,
                                 DTL_INTERVAL_INDICATOR_NA,
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValueBlock), 
                                 sDataValue,
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 ELL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                    
                    break;
                }
    
            case ELDT_DOMAIN_YES_OR_NO:
                {
                    sBoolValue = *(stlBool *) aInputData;
                    if ( sBoolValue == STL_TRUE )
                    {
                        sBoolString = DTL_BOOLEAN_STRING_TRUE;
                    }
                    else
                    {
                        sBoolString = DTL_BOOLEAN_STRING_FALSE;
                    }
                
                    STL_TRY( dtlSetValueFromString(
                                 sDataValue->mType,
                                 sBoolString,
                                 stlStrlen(sBoolString),
                                 DTL_PRECISION_NA, 
                                 DTL_SCALE_NA,
                                 DTL_STRING_LENGTH_UNIT_NA,
                                 DTL_INTERVAL_INDICATOR_NA,
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValueBlock), 
                                 sDataValue,
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 ELL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                    
                    break;
                }
    
            case ELDT_DOMAIN_TIME_STAMP:
                {
                    sInt64Value = *(stlInt64 *) aInputData;

                    STL_TRY( dtlTimestampSetValueFromStlTime(
                                 sInt64Value,
                                 ELDT_DOMAIN_PRECISION_TIME_STAMP, 
                                 DTL_SCALE_NA,
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValueBlock), 
                                 sDataValue,
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 ELL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                
                    break;
                }
            case ELDT_DOMAIN_INTERVAL:
                {
                    STL_TRY( dtlSetValueFromString( sDataValue->mType,
                                                    (stlChar *) aInputData,
                                                    stlStrlen((stlChar *) aInputData),
                                                    ELDT_DOMAIN_INTERVAL_START_PRECISION,
                                                    DTL_PRECISION_NA,
                                                    DTL_STRING_LENGTH_UNIT_NA,
                                                    ELDT_DOMAIN_INTERVAL_INDICATOR,
                                                    KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValueBlock), 
                                                    sDataValue,
                                                    & sSuccessWithInfo,
                                                    KNL_DT_VECTOR(aEnv),
                                                    aEnv,
                                                    KNL_DT_VECTOR(aEnv),
                                                    aEnv,
                                                    ELL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case ELDT_DOMAIN_FLOATING_NUMBER:
                {
                    STL_DASSERT( sDataValue->mType == DTL_TYPE_NUMBER );
                    sNumberValue = (dtlDataValue *) aInputData;
                    STL_DASSERT( sNumberValue->mType == DTL_TYPE_NUMBER );

                    DTL_COPY_DATA_VALUE( sDataValue->mBufferSize, sNumberValue, sDataValue );
                    break;
                }
            default:
                {
                    /* Invalid Domain */
                    STL_ASSERT(0);
                    break;
                }
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} eldMemory */





/**
 * @addtogroup eldInitFini
 * @{
 */


/**
 * @brief 서비스 운영에 필요한 Dictionary Cache 정보를 구축한다.
 * @param[in]  aEnv     Environment
 * @remarks
 */
stlStatus eldBuildDictionaryCache( ellEnv * aEnv )
{
    /**
     * Cache 구축이 시작됨
     */
    
    gEldCacheBuilding = STL_TRUE;
    
    /**
     * SQL-Object Cache 를 구축
     */
    
    STL_TRY( eldcBuildObjectCache( aEnv ) == STL_SUCCESS );
    
    /**
     * Privilege Cache 를 구축
     */
    
    STL_TRY( eldcBuildPrivCache( aEnv ) == STL_SUCCESS );

    /**
     * Cache 구축이 종료됨 
     */
    
    gEldCacheBuilding = STL_FALSE;

    return STL_SUCCESS;

    STL_FINISH;

    gEldCacheBuilding = STL_FALSE;
    
    return STL_FAILURE;
}


/**
 * @brief Dicitionary Handle을 설정한다.
 * @param[in] aEnv    Environment
 * @remarks
 * - OPEN 단계에서 호출됨.
 */

stlStatus eldSetDictionaryHandle( ellEnv * aEnv )
{
    stlInt32 sState = 0;
    
    smlTransId          sTransID  = SML_INVALID_TRANSID;
    smlStatement      * sStmt     = NULL; 

    /**
     * Storage Component 할당
     */
    
    /* Statement */
    STL_TRY( smlAllocStatement( sTransID,
                                NULL, /* aUpdateRelHandle */
                                SML_STMT_ATTR_READONLY,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                & sStmt,
                                SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Dictionary Table Handle 설정
     */

    STL_TRY( eldSetDictionaryTableHandle( sTransID,
                                          sStmt,
                                          aEnv )
             == STL_SUCCESS );
    
    /**
     * Dictionary Identity Column Handle 설정
     */

    STL_TRY( eldSetDictionaryIdentityHandle( sTransID,
                                             sStmt,
                                             aEnv )
             == STL_SUCCESS );

    /**
     * Storage Component 해제 
     */
    
    /* Statement */
    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /**  	 	 
     * Dictionary Dump Handle 설정  	 	 
     */
    
    STL_TRY( eldSetDictionaryDumpHandle( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch ( sState )
    {
        case 1:
            (void) smlFreeStatement( sStmt, SML_ENV( aEnv ) );
        default:
            break;
    }
    
    return STL_FAILURE;
}



/**
 * @brief OPEN 단계에서 Dictionary Table들의 handle을 설정한다.
 * @param[in]   aTransID   Transaction ID
 * @param[in]   aStmt      Statement
 * @param[in]   aEnv       Environment
 * @remarks
 */

stlStatus eldSetDictionaryTableHandle( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       ellEnv       * aEnv )
{
    void                  * sFirstPhysicalHandle = NULL;

    stlInt64                sSchemaID = 0;
    eldtDefinitionTableID   sTableID = 0;
    stlInt64                sPhysicalID = 0;
    void                  * sTableHandle = NULL;

    stlInt32                   sColumnCnt = 0;
    knlValueBlockList        * sDataValueList = NULL;
    eldtDefinitionColumnDesc * sColumnArrayDesc = NULL;

    stlBool               sBeginSelect = STL_FALSE;
    
    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;
    
    knlPredicateList    * sFilterPred = NULL;
    knlExprContextInfo  * sFilterContext = NULL;
    knlValueBlockList   * sFilterColumn = NULL;

    dtlDataValue        * sDataValue = NULL;
    smlRid                sRowRid;
    smlScn                sRowScn;
    smlRowBlock           sRowBlock;

    smlFetchInfo          sFetchInfo;
    knlExprEvalInfo       sFilterEvalInfo;
    eldMemMark            sMemMark;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLES,
                                     & sMemMark,
                                     & sDataValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * - Column 갯수 획득
     */
    STL_TRY( eldtGetDictTableColumnCnt( ELDT_TABLE_ID_TABLES,
                                        & sColumnCnt,
                                        aEnv )
             == STL_SUCCESS );
    
    /**
     * - Column 정보 획득 
     */
    STL_TRY( eldtGetDictColumnDescArray( ELDT_TABLE_ID_TABLES,
                                         & sColumnArrayDesc,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * DEFINITION_SCHEMA.TABLES 의 Physical ID 와 Handle 을 획득
     */

    STL_TRY( smlGetRelationHandle( smlGetFirstDicTableId(),
                                   & sFirstPhysicalHandle,
                                   SML_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * 조건 생성
     * WHERE SCHEMA_ID = ELDT_SCHEMA_ID_DEFINITION_SCHEMA
     */

    sSchemaID = ELDT_SCHEMA_ID_DEFINITION_SCHEMA;
    sFilterColumn = ellFindColumnValueList( sDataValueList,
                                            ELDT_TABLE_ID_TABLES,
                                            ELDT_Tables_ColumnOrder_SCHEMA_ID );
    
    STL_TRY( eldMakeOneFilter( & sFilterPred,
                               & sFilterContext,
                               sFilterColumn,
                               KNL_BUILTIN_FUNC_IS_EQUAL,
                               & sSchemaID,
                               STL_SIZEOF(stlInt64),
                               aEnv )
             == STL_SUCCESS );
                                   
    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForTableAccessQuery( aStmt,
                                          sFirstPhysicalHandle,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sDataValueList,
                                          sFilterPred,
                                          sFilterContext,
                                          & sFetchInfo,
                                          & sFilterEvalInfo,
                                          & sIterator,         
                                          aEnv )
             == STL_SUCCESS );
    
    sBeginSelect = STL_TRUE;

    while ( 1 )
    {
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv )
                 == STL_SUCCESS );
        
        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }

        /**
         * DEFINITION_SCHEMA 의 Table 들에 대한 Physical Handle 을 설정
         */

        sDataValue = eldFindDataValue( sDataValueList,
                                       ELDT_TABLE_ID_TABLES,
                                       ELDT_Tables_ColumnOrder_TABLE_ID );
                                               
        sTableID =  *(stlInt64*) sDataValue->mValue;
        
        sDataValue = eldFindDataValue( sDataValueList,
                                       ELDT_TABLE_ID_TABLES,
                                       ELDT_Tables_ColumnOrder_PHYSICAL_ID );
        sPhysicalID =  *(stlInt64*) sDataValue->mValue;
        
        STL_ASSERT( (sTableID > ELDT_TABLE_ID_NA ) &&
                    (sTableID < ELDT_TABLE_ID_MAX) );
        
        STL_TRY( smlGetRelationHandle( sPhysicalID,
                                       & sTableHandle,
                                       SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        gEldTablePhysicalHandle[sTableID] = sTableHandle;
    }

    /**
     * SELECT 종료
     */
    
    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief OPEN 단계에서 Dictionary 의 Identity Column 들의 handle을 설정한다.
 * @param[in]   aTransID   Transaction ID
 * @param[in]   aStmt      Statement
 * @param[in]   aEnv       Environment
 * @remarks
 */

stlStatus eldSetDictionaryIdentityHandle( smlTransId     aTransID,
                                          smlStatement * aStmt,
                                          ellEnv       * aEnv )
{
    stlInt64                sSchemaID = 0;
    stlInt64                sTableID = 0;
    stlBool                 sIsIdentity = STL_FALSE;
    stlInt64                sIdentityID = 0;
    void                  * sIdentityHandle = NULL;

    stlInt32                   sColumnCnt = 0;
    knlValueBlockList        * sDataValueList  = NULL;
    eldtDefinitionColumnDesc * sColumnArrayDesc = NULL;

    stlBool               sBeginSelect = STL_FALSE;
    
    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    knlPredicateList    * sFilterPred = NULL;
    knlExprContextInfo  * sFilterContext = NULL;
    knlValueBlockList   * sFilterColumn = NULL;
    dtlDataValue        * sDataValue = NULL;
    smlRid                sRowRid;
    smlScn                sRowScn;
    smlRowBlock           sRowBlock;

    smlFetchInfo          sFetchInfo;
    knlExprEvalInfo       sFilterEvalInfo;
    eldMemMark            sMemMark;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_COLUMNS,
                                     & sMemMark,
                                     & sDataValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * - Column 갯수 획득
     */
    STL_TRY( eldtGetDictTableColumnCnt( ELDT_TABLE_ID_COLUMNS,
                                        & sColumnCnt,
                                        aEnv )
             == STL_SUCCESS );
    
    /**
     * - Column 정보 획득 
     */
    STL_TRY( eldtGetDictColumnDescArray( ELDT_TABLE_ID_COLUMNS,
                                         & sColumnArrayDesc,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * 조건 생성
     * WHERE SCHEMA_ID = ELDT_SCHEMA_ID_DEFINITION_SCHEMA
     */

    sSchemaID = ELDT_SCHEMA_ID_DEFINITION_SCHEMA;
    sFilterColumn = ellFindColumnValueList( sDataValueList,
                                            ELDT_TABLE_ID_COLUMNS,
                                            ELDT_Columns_ColumnOrder_SCHEMA_ID );
    
    STL_TRY( eldMakeOneFilter( & sFilterPred,
                               & sFilterContext,
                               sFilterColumn,
                               KNL_BUILTIN_FUNC_IS_EQUAL,
                               & sSchemaID,
                               STL_SIZEOF(stlInt64),
                               aEnv )
             == STL_SUCCESS );
                                   
    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForTableAccessQuery( aStmt,
                                          gEldTablePhysicalHandle[ELDT_TABLE_ID_COLUMNS],
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sDataValueList,
                                          sFilterPred,
                                          sFilterContext,
                                          & sFetchInfo,
                                          & sFilterEvalInfo,
                                          & sIterator,         
                                          aEnv )
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    while ( 1 )
    {
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv )
                 == STL_SUCCESS );
        
        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }

        /**
         * DEFINITION_SCHEMA 의 Identity Column 들에 대한 Sequence Handle 을 설정
         */

        sDataValue = eldFindDataValue( sDataValueList,
                                       ELDT_TABLE_ID_COLUMNS,
                                       ELDT_Columns_ColumnOrder_IS_IDENTITY );
        
        sIsIdentity = *(stlBool*) sDataValue->mValue;
        
        if ( sIsIdentity == STL_TRUE )
        {
            /* DEFINITION_SCHEMA 의 Identity Column 임 */

            sDataValue = eldFindDataValue( sDataValueList,
                                           ELDT_TABLE_ID_COLUMNS,
                                           ELDT_Columns_ColumnOrder_TABLE_ID );
            sTableID = *(stlInt64*) sDataValue->mValue;

            sDataValue = eldFindDataValue( sDataValueList,
                                           ELDT_TABLE_ID_COLUMNS,
                                           ELDT_Columns_ColumnOrder_IDENTITY_PHYSICAL_ID );
            sIdentityID = *(stlInt64*) sDataValue->mValue;

            STL_ASSERT( (sTableID > ELDT_TABLE_ID_NA ) &&
                        (sTableID < ELDT_TABLE_ID_MAX) );
            
            STL_TRY( smlGetSequenceHandle( sIdentityID,
                                           & sIdentityHandle,
                                           SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            gEldIdentityColumnHandle[sTableID] = sIdentityHandle;
        }
        else
        {
            /**
             * Identity Column 이 아님 
             */
        }
    }

    /**
     * SELECT 종료
     */
    
    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    

    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Dictionary Cache 를 Dump 하기 위한 Handle을 설정한다.
 * @param[in] aEnv   Environment
 */
stlStatus eldSetDictionaryDumpHandle( ellEnv * aEnv )
{
    /***************************************************************
     * SQL-Object Cache 를 위한 Dump Handle
     ***************************************************************/
    
    /**
     * SQL-Column Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLColumnDumpHandle();
    
    /**
     * SQL-Index Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLIndexDumpHandle();
    
    /**
     * SQL-Constraint Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLConstDumpHandle();
    
    /**
     * SQL-Table Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLTableDumpHandle();
    
    /**
     * SQL-Sequence Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLSequenceDumpHandle();
    
    /**
     * SQL-Schema Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLSchemaDumpHandle();
    
    /**
     * SQL-Tablespace Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLTablespaceDumpHandle();
    
    /**
     * SQL-Authorization Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLAuthDumpHandle();
    
    /**
     * SQL-Catalog Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLCatalogDumpHandle();

    /**
     * SQL-Synonym Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLSynonymDumpHandle();

    /**
     * SQL-PublicSynonym Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLPublicSynonymDumpHandle();

    /**
     * SQL-Profile Cache 를 위한 Dump Handle 설정
     */

    eldcSetSQLProfileDumpHandle();
    
    /***************************************************************
     * Privilege Cache 를 위한 Dump Handle
     ***************************************************************/

    /**
     * Database Privilege Cache 를 위한 Dump Handle 설정 
     */

    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_DATABASE );
    
    /**
     * Tablespace Privilege Cache 를 위한 Dump Handle 설정 
     */

    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_SPACE );
    
    /**
     * Schema Privilege Cache 를 위한 Dump Handle 설정 
     */

    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_SCHEMA );
    
    /**
     * Table Privilege Cache 를 위한 Dump Handle 설정 
     */

    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_TABLE );
    
    /**
     * Usage Privilege Cache 를 위한 Dump Handle 설정 
     */

    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_USAGE );
    
    /**
     * Column Privilege Cache 를 위한 Dump Handle 설정 
     */

    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_COLUMN );
    
    return STL_SUCCESS;
}



/** @} eldInitFini */



/**
 * @addtogroup eldQuery
 * @{
 */


/**
 * @brief Dictionary Table에 대한 Table Access 검색을 위한 Transaction 관련 정보를 초기화한다.
 * @param[in]  aStmt                   Statement
 * @param[in]  aTableHandle            검색할 Table Handle
 * @param[in]  aIteratorProperty       iterator property
 * @param[in]  aTableReadColList       Table Read Column List
 * @param[in]  aRowBlock               Row Block
 * @param[in]  aFilterPred             Table 에 적용할 조건 
 * @param[in]  aFilterContext          Table Scan시 사용할 context 정보
 * @param[in]  aFetchInfo              Fetch 정보
 * @param[in]  aFilterEvalInfo         Filter 수행을 위한 정보
 * @param[out] aIterator               Iterator
 * @param[in]  aEnv                    Environment
 * @remarks
 */
stlStatus eldBeginForTableAccessQuery( smlStatement         * aStmt,
                                       void                 * aTableHandle,
                                       smlIteratorProperty  * aIteratorProperty,
                                       smlRowBlock          * aRowBlock,
                                       knlValueBlockList    * aTableReadColList,
                                       knlPredicateList     * aFilterPred,
                                       knlExprContextInfo   * aFilterContext,
                                       smlFetchInfo         * aFetchInfo,
                                       knlExprEvalInfo      * aFilterEvalInfo,
                                       void                ** aIterator,
                                       ellEnv               * aEnv )
{
    stlInt32  sState = 0;
    
    void                 * sIterator = NULL;
    
    smlTransReadMode  sTransReadMode;
    smlStmtReadMode   sStmtReadMode;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aIteratorProperty != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRowBlock != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aTableReadColList != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchInfo != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aIterator != NULL, ELL_ERROR_STACK( aEnv ) );


    /**
     * OUTPUT 초기화
     */
    
    *aIterator  = NULL;

    
    /**
     * Table Iterator 생성
     */
    
    SML_INIT_ITERATOR_PROP_PTR( aIteratorProperty );

    if ( aStmt->mNeedSnapshotIterator == STL_TRUE )
    {
        /* DDL validation 과정에서 정보를 얻기 위해 snapshot iterator 를 사용하는 경우 */
        sTransReadMode = SML_TRM_SNAPSHOT;
        sStmtReadMode  = SML_SRM_SNAPSHOT;
    }
    else
    {
        /* DDL execution 과정에서 DELETE/INSERT 등을 위해 recent iterator 를 사용하는 경우 */
        sTransReadMode = SML_TRM_COMMITTED;
        sStmtReadMode  = SML_SRM_RECENT;
    }
    
    STL_TRY( smlAllocIterator( aStmt,
                               aTableHandle,
                               sTransReadMode,
                               sStmtReadMode,
                               aIteratorProperty,
                               SML_SCAN_FORWARD,
                               & sIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;
    
    /**
     * Fetch Info 설정 : TABLE ACCESS
     */

    SML_SET_FETCH_INFO_FOR_TABLE( aFetchInfo,
                                  NULL,                 /* aPhysicalFilter */
                                  NULL,                 /* aLogicalFilter */
                                  aTableReadColList,
                                  NULL,                 /* aRowIDColumnList */
                                  aRowBlock,
                                  0,                    /* aSkipCnt */
                                  SML_FETCH_COUNT_MAX,  /* aFetchCnt */
                                  STL_FALSE,            /* aGroupKeyFetch */
                                  aFilterEvalInfo );
    
    if( aFilterEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        aFilterEvalInfo->mExprStack   = KNL_PRED_GET_FILTER( aFilterPred );
        aFilterEvalInfo->mContext     = aFilterContext;

        STL_TRY( knlInitBlock( & aFilterEvalInfo->mResultBlock,
                               1,
                               STL_TRUE,
                               STL_LAYER_EXECUTION_LIBRARY,
                               0,
                               0,
                               DTL_TYPE_BOOLEAN,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                               & aEnv->mMemDictOP,
                               KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        aFilterEvalInfo->mBlockIdx   = 0;
        aFilterEvalInfo->mBlockCount = 1;
    }
    
    
    /**
     * OUTPUT 설정
     */
    
    *aIterator = sIterator;

    
    return STL_SUCCESS;

    STL_FINISH;

    /**
     * 에러 발생시 자원 정리
     */

    switch (sState)
    {
        case 1:
            (void) smlFreeIterator( sIterator,
                                    SML_ENV( aEnv ) );
            *aIterator = NULL;
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief Dictionary Index에 대한 Index Access 검색을 위한 Transaction 관련 정보를 초기화한다.
 * @param[in]  aStmt                   Statement
 * @param[in]  aTableHandle            검색할 Table Handle
 * @param[in]  aIndexHandle            검색할 Index Handle
 * @param[in]  aKeyCompList            Key Compare List
 * @param[in]  aIteratorProperty       iterator property
 * @param[in]  aRowBlock               Row Block
 * @param[in]  aTableReadColList       Table Read Column List
 * @param[in]  aIndexReadColList       Index Read Column List
 * @param[in]  aRangePred              Index 에 적용할 조건 
 * @param[in]  aRangeContext           Index Scan 시 사용할 context 정보
 * @param[in]  aFetchInfo              Fetch 정보
 * @param[in]  aFetchRecordInfo        Fetch Record 수행을 위한 정보
 * @param[out] aIterator               Iterator
 * @param[in]  aEnv                    Environment
 * @remarks
 */
stlStatus eldBeginForIndexAccessQuery( smlStatement         * aStmt,
                                       void                 * aTableHandle,
                                       void                 * aIndexHandle,
                                       knlKeyCompareList    * aKeyCompList,
                                       smlIteratorProperty  * aIteratorProperty,
                                       smlRowBlock          * aRowBlock,
                                       knlValueBlockList    * aTableReadColList,
                                       knlValueBlockList    * aIndexReadColList,
                                       knlPredicateList     * aRangePred,
                                       knlExprContextInfo   * aRangeContext,
                                       smlFetchInfo         * aFetchInfo,
                                       smlFetchRecordInfo   * aFetchRecordInfo,
                                       void                ** aIterator,
                                       ellEnv               * aEnv )
{
    stlInt32 sState = 0;
    
    void                 * sIterator = NULL;
    smlFetchRecordInfo   * sFetchRecordInfo = NULL;
    
    smlTransReadMode  sTransReadMode;
    smlStmtReadMode   sStmtReadMode;

    knlRange * sRange = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aKeyCompList != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aIteratorProperty != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRowBlock != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aIndexReadColList != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchInfo != NULL, ELL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aIterator != NULL, ELL_ERROR_STACK( aEnv ) );


    /**
     * OUTPUT 초기화
     */
    
    *aIterator  = NULL;

    /**
     * Index Iterator 생성
     */
    
    SML_INIT_ITERATOR_PROP_PTR( aIteratorProperty );

    if ( aStmt->mNeedSnapshotIterator == STL_TRUE )
    {
        /* DDL validation 과정에서 정보를 얻기 위해 snapshot iterator 를 사용하는 경우 */
        sTransReadMode = SML_TRM_SNAPSHOT;
        sStmtReadMode  = SML_SRM_SNAPSHOT;
    }
    else
    {
        /* DDL execution 과정에서 DELETE/INSERT 등을 위해 recent iterator 를 사용하는 경우 */
        sTransReadMode = SML_TRM_COMMITTED;
        sStmtReadMode  = SML_SRM_RECENT;
    }
    
    STL_TRY( smlAllocIterator( aStmt,
                               aIndexHandle,
                               sTransReadMode,
                               sStmtReadMode,
                               aIteratorProperty,
                               SML_SCAN_FORWARD,
                               & sIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;
    
    /**
     * Fetch Record Info 설정 : INDEX ACCESS
     */

    if( aTableReadColList == NULL )
    {
        sFetchRecordInfo = NULL;
    }
    else
    {
        STL_DASSERT( aFetchRecordInfo != NULL );
        
        sFetchRecordInfo = aFetchRecordInfo;
        
        sFetchRecordInfo->mRelationHandle = aTableHandle;
        sFetchRecordInfo->mColList        = aTableReadColList;
        sFetchRecordInfo->mRowIdColList   = NULL;
        sFetchRecordInfo->mPhysicalFilter = NULL;
        
        sFetchRecordInfo->mTransReadMode  = sTransReadMode;
        sFetchRecordInfo->mStmtReadMode   = sStmtReadMode;
        sFetchRecordInfo->mScnBlock       = aRowBlock->mScnBlock;
        sFetchRecordInfo->mLogicalFilter  = NULL;
        sFetchRecordInfo->mLogicalFilterEvalInfo = NULL;
    }


    /**
     * Fetch Info 설정 : INDEX ACCESS
     */

    /* Range 정보 추출 */
    if( aRangePred == NULL )
    {
        sRange = NULL;
    }
    else
    {
        STL_DASSERT( aRangeContext != NULL );

        STL_TRY( knlAllocRegionMem( & aEnv->mMemDictOP,
                                    STL_SIZEOF( knlRange ),
                                    (void **) & sRange,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        STL_TRY( knlGetCompareListFromRangeElem(
                     KNL_PRED_GET_MIN_RANGE( aRangePred ),
                     aRangeContext,
                     & aEnv->mMemDictOP,
                     & sRange->mMinRange,
                     KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlGetCompareListFromRangeElem(
                     KNL_PRED_GET_MAX_RANGE( aRangePred ),
                     aRangeContext,
                     & aEnv->mMemDictOP,
                     & sRange->mMaxRange,
                     KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        sRange->mNext = NULL;
    }

    SML_SET_FETCH_INFO_FOR_INDEX( aFetchInfo,
                                  sRange,
                                  NULL,                  /* aPhysicalFilter */
                                  aKeyCompList,   
                                  NULL,                  /* aLogicalFilter */
                                  aIndexReadColList,     /* aReadColumnList */
                                  NULL,                  /* aRowIDColumnList */
                                  aRowBlock,  
                                  0,                     /* aSkipCnt */
                                  SML_FETCH_COUNT_MAX,   /* aFetchCnt */
                                  STL_FALSE,             /* aGroupKeyFetch */
                                  NULL,                  /* aFilterEvalInfo */
                                  sFetchRecordInfo );
    
    /**
     * OUTPUT 설정
     */
    
    *aIterator = sIterator;

    
    return STL_SUCCESS;

    STL_FINISH;

    /**
     * 에러 발생시 자원 정리
     */

    switch (sState)
    {
        case 1:
            (void) smlFreeIterator( sIterator,
                                    SML_ENV( aEnv ) );
            *aIterator = NULL;
        default:
            break;
    }

    return STL_FAILURE;
}

    
/* /\** */
/*  * @brief Dictionary Table 검색을 위한 Transaction 관련 정보를 초기화한다. */
/*  * @param[in]  aStmt                   Statement */
/*  * @param[in]  aTableHandle            검색할 Table Handle */
/*  * @param[in]  aFilterPredicate        Table 에 적용할 조건  */
/*  * @param[out] aTableIterator          Table Iterator */
/*  * @param[in]  aTableIteratorProperty  Table Iterator Property */
/*  * @param[in]  aIndexHandle            사용할 Index Handle */
/*  * @param[in]  aRangePredicate         Index 에 적용할 조건  */
/*  * @param[out] aIndexIterator          Index Iterator */
/*  * @param[in]  aIndexIteratorProperty  Index Iterator Property */
/*  * @param[in]  aFetchRecordInfo        Fetch Record Info */
/*  * @param[in]  aEnv                    Environment */
/*  * @remarks */
/*  *\/ */
/* stlStatus eldBeginForQuery( smlStatement        * aStmt, */
/*                             void                * aTableHandle, */
/*                             knlPredicateList    * aFilterPredicate, */
/*                             void               ** aTableIterator, */
/*                             smlIteratorProperty * aTableIteratorProperty, */
/*                             void                * aIndexHandle, */
/*                             knlPredicateList    * aRangePredicate, */
/*                             void               ** aIndexIterator, */
/*                             smlIteratorProperty * aIndexIteratorProperty, */
/*                             smlFetchRecordInfo  * aFetchRecordInfo, */
/*                             ellEnv              * aEnv ) */
/* { */
/*     void  * sTableIterator  = NULL; */
/*     void  * sIndexIterator  = NULL; */

/*     knlCompareList  * sMinRangeList; */
/*     knlCompareList  * sMaxRangeList; */
        
/*     /\* */
/*      * Parameter Validation */
/*      *\/ */

/*     STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) ); */
/*     STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) ); */

/*     /\** */
/*      * Iterator 초기화 */
/*      *\/ */

/*     if ( aIndexHandle == NULL ) */
/*     { */
/*         /\** */
/*          * Table Iterator 생성 */
/*          *\/ */
        
/*         SML_INIT_ITERATOR_PROP_PTR( aTableIteratorProperty ); */
            
/*         STL_TRY( smlAllocIterator( aStmt, */
/*                                    aTableHandle, */
/*                                    SML_TRM_COMMITTED, */
/*                                    SML_SRM_RECENT, */
/*                                    aTableIteratorProperty, */
/*                                    aFilterPredicate, */
/*                                    NULL, */
/*                                    NULL, */
/*                                    NULL, */
/*                                    NULL, */
/*                                    NULL, */
/*                                    SML_SCAN_FORWARD, */
/*                                    & sTableIterator, */
/*                                    SML_ENV( aEnv ) ) */
/*                  == STL_SUCCESS ); */

/*         sIndexIterator = NULL; */
/*     } */
/*     else */
/*     { */
/*         /\** */
/*          * Index Iterator 생성 */
/*          *\/ */

/*         SML_INIT_ITERATOR_PROP_PTR( aIndexIteratorProperty ); */
        
/*         if( aRangePredicate == NULL ) */
/*         { */
/*             sMinRangeList = NULL; */
/*             sMaxRangeList = NULL; */
/*         } */
/*         else */
/*         { */
/*             STL_TRY( knlGetCompareListFromRangeElem( */
/*                          KNL_PRED_GET_MIN_RANGE( aRangePredicate ), */
/*                          KNL_PRED_GET_CONTEXT( aRangePredicate ), */
/*                          & aEnv->mMemDictOP, */
/*                          & sMinRangeList, */
/*                          KNL_ENV( aEnv ) ) */
/*                      == STL_SUCCESS ); */

/*             STL_TRY( knlGetCompareListFromRangeElem( */
/*                          KNL_PRED_GET_MAX_RANGE( aRangePredicate ), */
/*                          KNL_PRED_GET_CONTEXT( aRangePredicate ), */
/*                          & aEnv->mMemDictOP, */
/*                          & sMaxRangeList, */
/*                          KNL_ENV( aEnv ) ) */
/*                      == STL_SUCCESS ); */
/*         } */

/*         STL_TRY( smlAllocIterator( aStmt, */
/*                                    aIndexHandle, */
/*                                    SML_TRM_COMMITTED, */
/*                                    SML_SRM_RECENT, */
/*                                    aIndexIteratorProperty, */
/*                                    aRangePredicate, */
/*                                    sMinRangeList, */
/*                                    sMaxRangeList, */
/*                                    NULL, */
/*                                    NULL, */
/*                                    aFetchRecordInfo, */
/*                                    SML_SCAN_FORWARD, */
/*                                    & sIndexIterator, */
/*                                    SML_ENV( aEnv ) ) */
/*                  == STL_SUCCESS ); */
        
/*         sTableIterator = NULL; */
/*     } */
    
    
/*    /\** */
/*      * OUTPUT 설정 */
/*      *\/ */
    
/*     *aIterator = sIterator; */

    
/*     return STL_SUCCESS; */

/*     STL_FINISH; */

/*     /\** */
/*      * 에러 발생시 자원 정리 */
/*      *\/ */
    
/*     if( sIterator != NULL ) */
/*     { */
/*         (void) smlFreeIterator( sIterator, */
/*                                 SML_ENV( aEnv ) ); */
/*         *aIterator = NULL; */
/*     } */

/*     return STL_FAILURE; */
/* } */


/**
 * @brief 함수 eldBeginForQuery() 로부터 할당받은 자원을 해제한다.
 * @param[in]  aIterator      Iterator
 * @param[in]  aEnv           Envirionment
 * @remarks
 */
stlStatus eldEndForQuery( void    * aIterator,
                          ellEnv  * aEnv )
{
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aIterator != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * Iterator 자원 해제 
     */

    if( aIterator != NULL )
    {
        STL_TRY( smlFreeIterator( aIterator,
                                  SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        aIterator = NULL;
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    /**
     * 에러 발생시 자원 정리
     */
    
    aIterator = NULL;

    return STL_FAILURE;
}


/**
 * @brief WHERE column [op] value 에 해당하는 filter 를 만든다.
 * @param[out]     aFilterPred       Filter Predicate
 * @param[out]     aFilterContext    Context for Filter
 * @param[in]      aColumnValue      Column 에 해당하는 Value Block
 * @param[in]      aFuncID           function ID ( 비교 연산자 )
 * @param[in]      aConstant         Constant Value
 * @param[in]      aConstantLength   Constant Length
 * @param[in]      aEnv              Environment
 * @remarks
 */
stlStatus eldMakeOneFilter( knlPredicateList    ** aFilterPred,
                            knlExprContextInfo  ** aFilterContext,
                            knlValueBlockList    * aColumnValue,
                            knlBuiltInFunc         aFuncID,
                            void                 * aConstant,
                            stlInt64               aConstantLength,
                            ellEnv               * aEnv )
{
    knlValueBlockList   * sArgList = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aFilterPred != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterContext != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnValue != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstant != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstantLength > 0, ELL_ERROR_STACK(aEnv) );

    
    /**
     * value list 설정 : column
     */

    STL_TRY( knlInitShareBlock( & sArgList,
                                aColumnValue,
                                & aEnv->mMemDictOP,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * value list 설정 : constant value 
     */

    STL_DASSERT( (KNL_GET_BLOCK_DB_TYPE( sArgList ) != DTL_TYPE_LONGVARCHAR) &&
                 (KNL_GET_BLOCK_DB_TYPE( sArgList ) != DTL_TYPE_LONGVARBINARY) );
    
    STL_TRY( knlInitCopyBlock( & sArgList->mNext,
                               STL_LAYER_EXECUTION_LIBRARY,
                               KNL_GET_BLOCK_ALLOC_CNT(sArgList),
                               sArgList,
                               & aEnv->mMemDictOP,
                               KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sArgList->mNext, 0 ),
               aConstant,
               aConstantLength );

    KNL_SET_BLOCK_DATA_LENGTH( sArgList->mNext, 0, aConstantLength );
    

    /**
     * Filter 생성 
     */

    STL_TRY( ellMakeOneFilter( aFuncID,
                               sArgList,
                               aFilterPred,
                               aFilterContext,
                               & aEnv->mMemDictOP,
                               aEnv )
             == STL_SUCCESS );
    
             
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief WHERE column = value 에 해당하는 Equal Key Range 를 만든다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aTableID          Dictionary Table ID
 * @param[in]  aColumnValue      Column Value
 * @param[in]  aConstant         Constant Value
 * @param[in]  aConstantLength   Constant Length
 * @param[out] aIndexHandle      Range 를 사용할 Index 의 Physical Handle
 * @param[out] aKeyCompList      Index Key Compare List
 * @param[out] aIndexValueList   조건에 맞는 Data 를 얻어올 Index Value List
 * @param[out] aRangePred        Range Predicate
 * @param[out] aRangeContext     Context for Range Predicate
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus eldMakeOneEquiRange( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               eldtDefinitionTableID   aTableID,
                               knlValueBlockList     * aColumnValue,
                               void                  * aConstant,
                               stlInt64                aConstantLength,
                               void                 ** aIndexHandle,
                               knlKeyCompareList    ** aKeyCompList,
                               knlValueBlockList    ** aIndexValueList,
                               knlPredicateList     ** aRangePred,
                               knlExprContextInfo   ** aRangeContext,
                               ellEnv                * aEnv )
{
    stlInt32  i = 0;
    
    knlPredicateList  * sRange = NULL;

    stlBool             sObjectExist = STL_FALSE;
    
    ellDictHandle       sTableHandle;

    stlInt32            sPrimaryKeyCnt;
    ellDictHandle     * sPrimaryHandle = NULL;

    stlInt32            sUniqueKeyCnt;
    ellDictHandle     * sUniqueHandle = NULL;

    stlInt32            sForeignKeyCnt;
    ellDictHandle     * sForeignHandle = NULL;

    stlInt32            sUniqueIndexCnt;
    ellDictHandle     * sUniqueIndexHandle = NULL;
    
    stlInt32            sNonUniqueIndexCnt;
    ellDictHandle     * sNonUniqueIndexHandle = NULL;

    ellDictHandle     * sTempIndexHandle = NULL;
    
    stlInt32            sKeyCnt = 0;
    ellIndexKeyDesc   * sKeyDesc = NULL;

    ellDictHandle     * sIndexDictHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;

    knlKeyCompareList * sKeyCompList = NULL;
    
    knlValueBlockList * sKeyColumn = NULL;
    knlValueBlockList * sKeyValue  = NULL;

    knlExprContextInfo  * sRangeContext = NULL;
    
    ellInitDictHandle( &sTableHandle );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aTableID > ELDT_TABLE_ID_NA) &&
                        (aTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnValue != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstant != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstantLength > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyCompList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRangePred != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRangeContext != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Table 정보 획득 
     */

    STL_TRY( eldcGetTableHandleByID( aTransID,
                                     ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                     aTableID,
                                     & sTableHandle,
                                     & sObjectExist,
                                     aEnv )
             == STL_SUCCESS );
    STL_ASSERT( sObjectExist == STL_TRUE );

    /**
     * Column 과 동일한 Key 를 갖는 인덱스 검색
     */

    while ( 1 )
    {
        /**
         * Unique Index 에서 검색 
         */
    
        STL_TRY( ellGetTableNonUniqueIndexDictHandle( aTransID,
                                                      ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                      & aEnv->mMemDictOP,
                                                      & sTableHandle,
                                                      & sUniqueIndexCnt,
                                                      & sUniqueIndexHandle,
                                                      aEnv )
                 == STL_SUCCESS );
    
        sIndexDictHandle = NULL;
        for ( i = 0; i < sUniqueIndexCnt; i++ )
        {
            sKeyCnt = ellGetIndexKeyCount( & sUniqueIndexHandle[i] );
            if ( sKeyCnt == 1 )
            {
                sKeyDesc = ellGetIndexKeyDesc( & sUniqueIndexHandle[i] );

                /**
                 * 조건의 컬럼과 인덱스의 Key 가 동일한지 검사
                 */
                if ( aColumnValue->mColumnOrder
                     == ellGetColumnIdx( & sKeyDesc->mKeyColumnHandle ) )
                {
                    sIndexDictHandle = & sUniqueIndexHandle[i];
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                /* Composite Index 임 */
                continue;
            }
        }

        if ( sIndexDictHandle != NULL )
        {
            break;
        }
        else
        {
            /* go go */
        }

        /**
         * Non-Unique Index 에서 검색 
         */
    
        STL_TRY( ellGetTableNonUniqueIndexDictHandle( aTransID,
                                                      ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                      & aEnv->mMemDictOP,
                                                      & sTableHandle,
                                                      & sNonUniqueIndexCnt,
                                                      & sNonUniqueIndexHandle,
                                                      aEnv )
                 == STL_SUCCESS );
    
        sIndexDictHandle = NULL;
        for ( i = 0; i < sNonUniqueIndexCnt; i++ )
        {
            sKeyCnt = ellGetIndexKeyCount( & sNonUniqueIndexHandle[i] );
            if ( sKeyCnt == 1 )
            {
                sKeyDesc = ellGetIndexKeyDesc( & sNonUniqueIndexHandle[i] );

                /**
                 * 조건의 컬럼과 인덱스의 Key 가 동일한지 검사
                 */
                if ( aColumnValue->mColumnOrder
                     == ellGetColumnIdx( & sKeyDesc->mKeyColumnHandle ) )
                {
                    sIndexDictHandle = & sNonUniqueIndexHandle[i];
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                /* Composite Index 임 */
                continue;
            }
        }

        if ( sIndexDictHandle != NULL )
        {
            break;
        }
        else
        {
            /* go go */
        }

        /**
         * Primary Key Index 에서 검색 
         */
    
        STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                                  ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                  & aEnv->mMemDictOP,
                                                  & sTableHandle,
                                                  & sPrimaryKeyCnt,
                                                  & sPrimaryHandle,
                                                  aEnv )
                 == STL_SUCCESS );
    
        sIndexDictHandle = NULL;
        for ( i = 0; i < sPrimaryKeyCnt; i++ )
        {
            sTempIndexHandle = ellGetConstraintIndexDictHandle( & sPrimaryHandle[i] );
            sKeyCnt = ellGetIndexKeyCount( sTempIndexHandle );
            if ( sKeyCnt == 1 )
            {
                sKeyDesc = ellGetIndexKeyDesc( sTempIndexHandle );

                /**
                 * 조건의 컬럼과 인덱스의 Key 가 동일한지 검사
                 */
                if ( aColumnValue->mColumnOrder 
                     == ellGetColumnIdx( & sKeyDesc->mKeyColumnHandle ) )
                {
                    sIndexDictHandle = sTempIndexHandle;
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                /* Composite Index 임 */
                continue;
            }
        }

        if ( sIndexDictHandle != NULL )
        {
            break;
        }
        else
        {
            /* go go */
        }

        /**
         * Unique Key Index 에서 검색 
         */
    
        STL_TRY( ellGetTableUniqueKeyDictHandle( aTransID,
                                                 ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                 & aEnv->mMemDictOP,
                                                 & sTableHandle,
                                                 & sUniqueKeyCnt,
                                                 & sUniqueHandle,
                                                 aEnv )
                 == STL_SUCCESS );
    
        sIndexDictHandle = NULL;
        for ( i = 0; i < sUniqueKeyCnt; i++ )
        {
            sTempIndexHandle = ellGetConstraintIndexDictHandle( & sUniqueHandle[i] );
            sKeyCnt = ellGetIndexKeyCount( sTempIndexHandle );
            if ( sKeyCnt == 1 )
            {
                sKeyDesc = ellGetIndexKeyDesc( sTempIndexHandle );

                /**
                 * 조건의 컬럼과 인덱스의 Key 가 동일한지 검사
                 */
                if ( aColumnValue->mColumnOrder 
                     == ellGetColumnIdx( & sKeyDesc->mKeyColumnHandle ) )
                {
                    sIndexDictHandle = sTempIndexHandle;
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                /* Composite Index 임 */
                continue;
            }
        }

        if ( sIndexDictHandle != NULL )
        {
            break;
        }
        else
        {
            /* go go */
        }
        
        /**
         * Foreign Key Index 에서 검색 
         */
    
        STL_TRY( ellGetTableForeignKeyDictHandle( aTransID,
                                                  ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                  & aEnv->mMemDictOP,
                                                  & sTableHandle,
                                                  & sForeignKeyCnt,
                                                  & sForeignHandle,
                                                  aEnv )
                 == STL_SUCCESS );
    
        sIndexDictHandle = NULL;
        for ( i = 0; i < sForeignKeyCnt; i++ )
        {
            sTempIndexHandle = ellGetConstraintIndexDictHandle( & sForeignHandle[i] );
            sKeyCnt = ellGetIndexKeyCount( sTempIndexHandle );
            if ( sKeyCnt == 1 )
            {
                sKeyDesc = ellGetIndexKeyDesc( sTempIndexHandle );

                /**
                 * 조건의 컬럼과 인덱스의 Key 가 동일한지 검사
                 */
                if ( aColumnValue->mColumnOrder 
                     == ellGetColumnIdx( & sKeyDesc->mKeyColumnHandle ) )
                {
                    sIndexDictHandle = sTempIndexHandle;
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                /* Composite Index 임 */
                continue;
            }
        }

        if ( sIndexDictHandle != NULL )
        {
            break;
        }
        else
        {
            /* go go */
        }
        
        break;
    }

    STL_ASSERT( sIndexDictHandle != NULL );

    /**
     * Index 의 Key Compare List 획득
     */


    STL_TRY( ellGetKeyCompareList( sIndexDictHandle,
                                   & aEnv->mMemDictOP,
                                   & sKeyCompList,
                                   aEnv )
             == STL_SUCCESS );
                                   
    /**
     * Index 의 Value 공간 확보 
     */

    STL_TRY( ellAllocIndexValueListForSELECT(
                 sIndexDictHandle,
                 ellGetIndexKeyCount( sIndexDictHandle ),
                 & aEnv->mMemDictOP,
                 KNL_NO_BLOCK_READ_CNT,
                 & sIndexValueList,
                 aEnv )
             == STL_SUCCESS );

    /**
     * Key Column 설정 
     */

    STL_TRY( knlInitShareBlock( & sKeyColumn,
                                sIndexValueList,
                                & aEnv->mMemDictOP,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
                                
    /**
     * Key Value 설정 
     */

    STL_DASSERT( (KNL_GET_BLOCK_DB_TYPE( aColumnValue ) != DTL_TYPE_LONGVARCHAR) &&
                 (KNL_GET_BLOCK_DB_TYPE( aColumnValue ) != DTL_TYPE_LONGVARBINARY) );
    
    STL_TRY( knlInitCopyBlock( & sKeyValue,
                               STL_LAYER_EXECUTION_LIBRARY,
                               KNL_NO_BLOCK_READ_CNT,
                               aColumnValue,
                               & aEnv->mMemDictOP,
                               KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sKeyValue, 0 ),
               aConstant,
               aConstantLength );

    KNL_SET_BLOCK_DATA_LENGTH( sKeyValue, 0, aConstantLength );
    
    /**
     * Equal Key Range 생성
     */
    
    STL_TRY( knlCreateRangePred( 1,
                                 STL_TRUE,
                                 & sRange,
                                 & sRangeContext,
                                 & aEnv->mMemDictOP,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /* context info 구성 */
    sRangeContext->mContexts[ 0 ].mInfo.mValueInfo = sKeyColumn->mValueBlock;
    sRangeContext->mContexts[ 1 ].mInfo.mValueInfo = sKeyValue->mValueBlock;
    
    sKeyDesc = ellGetIndexKeyDesc( sIndexDictHandle );
    STL_TRY( knlAddRange( STL_TRUE,
                          0, /* sKeyColumn */
                          1, /* sKeyValue */
                          STL_TRUE,
                          ( sKeyDesc->mKeyOrdering == ELL_INDEX_COLUMN_ASCENDING ),
                          ( sKeyDesc->mKeyNullOrdering == ELL_INDEX_COLUMN_NULLS_FIRST ),
                          STL_TRUE, /* Null Always STOP */
                          sRange,
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( knlAddRange( STL_FALSE,
                          0, /* sKeyColumn */
                          1, /* sKeyValue */
                          STL_TRUE,
                          ( sKeyDesc->mKeyOrdering == ELL_INDEX_COLUMN_ASCENDING ),
                          ( sKeyDesc->mKeyNullOrdering == ELL_INDEX_COLUMN_NULLS_FIRST ),
                          STL_TRUE, /* Null Always STOP */
                          sRange,
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * Index Column Order 설정
     */
    
    STL_TRY( ellSetIndexColumnOrder( sIndexValueList,
                                     sIndexDictHandle,
                                     aEnv )
             == STL_SUCCESS );

    
    /**
     * Output 설정
     */

    *aIndexHandle    = ellGetIndexHandle( sIndexDictHandle );
    *aKeyCompList    = sKeyCompList;
    *aIndexValueList = sIndexValueList;
    *aRangePred      = sRange;
    *aRangeContext   = sRangeContext;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Dictionary Table 의 Row 를 Fetch 한다.
 * @param[in]  aIterator           Index Iterator
 * @param[in]  aFetchInfo          Fetch Info
 * @param[in]  aEnv                Environment
 * @remarks
 */
stlStatus eldFetchNext( void          * aIterator,
                        smlFetchInfo  * aFetchInfo,
                        ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIterator != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFetchInfo != NULL, ELL_ERROR_STACK(aEnv) );

    STL_TRY( smlFetchNext( aIterator,
                           aFetchInfo,
                           SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Object ID 에 대한 Row Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDictTableID     Object ID 가 속한 Dictionary Table ID
 * @param[in]  aObjectColumnIdx Object ID 가 속한 Column Idx
 * @param[in]  aObjectID        Object ID
 * @param[in]  aLockMode        다음 중 하나의 값이어야 함.
 *                         <BR> - SML_LOCK_S
 *                         <BR> - SML_LOCK_X
 * @param[in]  aLockSuccess  Lock 획득 여부                         
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldLockRowByObjectID( smlTransId              aTransID,
                                smlStatement          * aStmt,
                                eldtDefinitionTableID   aDictTableID,
                                stlInt32                aObjectColumnIdx,
                                stlInt64                aObjectID,
                                stlInt32                aLockMode,
                                stlBool               * aLockSuccess,
                                ellEnv                * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    stlBool             sFetchOne = STL_FALSE;
    stlBool             sBeginSelect = STL_FALSE;
    stlBool             sLocked = STL_TRUE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;
    
    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    knlValueBlockList * sFilterColumn = NULL;
    stlBool             sRecordExist = STL_TRUE;
    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDictTableID > ELDT_TABLE_ID_NA) &&
                        (aDictTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectColumnIdx >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aLockMode == SML_LOCK_S) || (aLockMode == SML_LOCK_X),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Dictionary Table 에 대한 검색 조건 생성 
     */

    sTableHandle = gEldTablePhysicalHandle[aDictTableID];
    
    STL_TRY( eldAllocTableValueList( aDictTableID,
                                     & sMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            aDictTableID,
                                            aObjectColumnIdx );

    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  aDictTableID,
                                  sFilterColumn,
                                  & aObjectID,
                                  STL_SIZEOF(stlInt64),
                                  & sIndexHandle,
                                  & sKeyCompList,
                                  & sIndexValueList,
                                  & sRangePred,
                                  & sRangeContext,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * OBJECT_ID 에 해당하는 Row RID 를 획득 
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;
    
    sLocked = STL_TRUE;
    sFetchOne = STL_FALSE;
    while (1)
    {
        /**
         * 레코드를 읽는다.
         */
        
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo, 
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        
        /**
         * Row Lock 를 획득 
         */

        STL_TRY( smlLockRecordForDdl( aStmt,
                                      sTableHandle,
                                      aLockMode,
                                      sRowRid,
                                      &sRecordExist,
                                      SML_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sRecordExist != STL_TRUE )
        {
            sLocked = STL_FALSE;
            STL_THROW( RAMP_FINISH );
        }
                                
        /**
         * 한 건만 존재해야 한다.
         */
        
        STL_ASSERT( sFetchOne == STL_FALSE );
        sFetchOne = STL_TRUE;
    }

    if ( sFetchOne != STL_TRUE )
    {
        sLocked = STL_FALSE;
        STL_THROW( RAMP_FINISH );
    }

    /*************************************************
     * 작업 종료
     *************************************************/
    
    STL_RAMP( RAMP_FINISH );

    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aLockSuccess = sLocked;
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}



/**
 * @brief Object Handle 에 대한 Row Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDictTableID     Object ID 가 속한 Dictionary Table ID
 * @param[in]  aObjectColumnIdx Object ID 가 속한 Column Idx
 * @param[in]  aObjectHandle    Object Dictionary Handle
 * @param[in]  aLockMode        다음 중 하나의 값이어야 함.
 *                         <BR> - SML_LOCK_S
 *                         <BR> - SML_LOCK_X
 * @param[out] aLockSuccess     Lock 획득 여부                         
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldLockRowByObjectHandle( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    eldtDefinitionTableID   aDictTableID,
                                    stlInt32                aObjectColumnIdx,
                                    ellDictHandle         * aObjectHandle,
                                    stlInt32                aLockMode,
                                    stlBool               * aLockSuccess,
                                    ellEnv                * aEnv )
{
    stlInt64  sObjectID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDictTableID > ELDT_TABLE_ID_NA) &&
                        (aDictTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectColumnIdx >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aLockMode == SML_LOCK_S) || (aLockMode == SML_LOCK_X),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Object ID 획득
     */
    
    sObjectID = ellGetDictObjectID( aObjectHandle );

    /**
     * Lock Row 수행
     */
    
    STL_TRY( eldLockRowByObjectID( aTransID,
                                   aStmt,
                                   aDictTableID,
                                   aObjectColumnIdx,
                                   sObjectID,
                                   aLockMode,
                                   aLockSuccess,
                                   aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}









/**
 * @brief DDL Object 와 관련된 Dictionary Object List 를 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aTableID          Dictionary Table ID
 * @param[in]  aTargetObjectType Target Object 의 유형
 * @param[in]  aTargetColumnIdx  Target Column 의 Ordinal Position
 * @param[in]  aFilterColumnIdx  Filter Column 의 Ordinal Position
 * @param[in]  aFilterValue      Filter Value (Object ID)
 * @param[in]  aRegionMem        Region Memory
 * @param[out] aObjectList       Dictionary Object List
 * @param[in]  aEnv          
 * @remarks
 * 다음과 같은 구문에 해당한다.
 * - SELECT aTargetColumnIdx
 * - ..FROM DEFINITION_SCHEMA.aTableID
 * - .WHERE aFilterColumnIdx = aFilterValue;
 */
stlStatus eldGetObjectList( smlTransId              aTransID,
                            smlStatement          * aStmt,
                            eldtDefinitionTableID   aTableID,
                            ellObjectType           aTargetObjectType,
                            stlInt32                aTargetColumnIdx,
                            stlInt32                aFilterColumnIdx,
                            stlInt64                aFilterValue,
                            knlRegionMem          * aRegionMem,
                            ellObjectList         * aObjectList,
                            ellEnv                * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;

    knlValueBlockList * sFilterColumn = NULL;
    
    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;

    knlValueBlockList   * sTargetColumn = NULL;
    knlValueBlockList   * sTableType = NULL;
    ellTableType          sTableTypeID = ELL_TABLE_TYPE_NA; 
    
    dtlDataValue        * sObjectValue = NULL;
    stlInt64              sObjectID = ELL_DICT_OBJECT_ID_NA;
    stlBool               sObjectExist = STL_FALSE;
    ellDictHandle         sObjectHandle;

    eldcGetObjectHandleByID  sGetHandleFunc = NULL;
    
    smlFetchInfo          sFetchInfo;
    smlFetchRecordInfo    sFetchRecordInfo;
    
    ellInitDictHandle ( & sObjectHandle );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aTableID > ELDT_TABLE_ID_NA) &&
                        (aTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aTargetObjectType > ELL_OBJECT_NA) &&
                        (aTargetObjectType < ELL_OBJECT_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetColumnIdx >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterColumnIdx >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterValue > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectList != NULL, ELL_ERROR_STACK(aEnv) );
    

    /**
     * Object Handle 을 획득 함수 결정 
     */
    
    switch( aTargetObjectType )
    {
        case ELL_OBJECT_COLUMN:
            sGetHandleFunc = eldcGetColumnHandleByID;
            break;
        case ELL_OBJECT_TABLE:
            sGetHandleFunc = eldcGetTableHandleByID;
            break;
        case ELL_OBJECT_CONSTRAINT:
            sGetHandleFunc = eldcGetConstHandleByID;
            break;
        case ELL_OBJECT_INDEX:
            sGetHandleFunc = eldcGetIndexHandleByID;
            break;
        case ELL_OBJECT_SEQUENCE:
            sGetHandleFunc = eldcGetSequenceHandleByID;
            break;
        case ELL_OBJECT_SYNONYM:
            sGetHandleFunc = eldcGetSynonymHandleByID;
            break;
        case ELL_OBJECT_SCHEMA:
            sGetHandleFunc = eldcGetSchemaHandleByID;
            break;
        case ELL_OBJECT_TABLESPACE:
            sGetHandleFunc = eldcGetTablespaceHandleByID;
            break;
        case ELL_OBJECT_AUTHORIZATION:
            sGetHandleFunc = eldcGetAuthHandleByID;
            break;
        case ELL_OBJECT_CATALOG:
            STL_ASSERT(0);
            break;
        default:
            STL_ASSERT(0);
            break;
    }
                        
    /**
     * DEFINITION_SCHEMA.aTableID 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[aTableID];
    
    /**
     * Target Column
     */

    STL_TRY( knlInitBlock( & sTargetColumn,
                           KNL_NO_BLOCK_READ_CNT,
                           STL_TRUE, /* 컬럼임 */
                           STL_LAYER_EXECUTION_LIBRARY,
                           aTableID,
                           aTargetColumnIdx,
                           DTL_TYPE_NATIVE_BIGINT,
                           gDefaultDataTypeDef[DTL_TYPE_NATIVE_BIGINT].mArgNum1,
                           gDefaultDataTypeDef[DTL_TYPE_NATIVE_BIGINT].mArgNum2,
                           gDefaultDataTypeDef[DTL_TYPE_NATIVE_BIGINT].mStringLengthUnit,
                           gDefaultDataTypeDef[DTL_TYPE_NATIVE_BIGINT].mIntervalIndicator,
                           & aEnv->mMemDictOP,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Table 일 경우 Naming Space 를 함께 사용하고 있는 Sequence, Synonym 은 제거해야 함.
     */

    if ( aTargetObjectType == ELL_OBJECT_TABLE )
    {
        STL_TRY( knlInitBlock( & sTableType,
                               KNL_NO_BLOCK_READ_CNT,
                               STL_TRUE, /* 컬럼임 */
                               STL_LAYER_EXECUTION_LIBRARY,
                               aTableID,
                               ELDT_Tables_ColumnOrder_TABLE_TYPE_ID,
                               DTL_TYPE_NATIVE_INTEGER,
                               gDefaultDataTypeDef[DTL_TYPE_NATIVE_INTEGER].mArgNum1,
                               gDefaultDataTypeDef[DTL_TYPE_NATIVE_INTEGER].mArgNum2,
                               gDefaultDataTypeDef[DTL_TYPE_NATIVE_INTEGER].mStringLengthUnit,
                               gDefaultDataTypeDef[DTL_TYPE_NATIVE_INTEGER].mIntervalIndicator,
                               & aEnv->mMemDictOP,
                               KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sTargetColumn->mNext = sTableType;
    }
    else
    {
        /**
         * nothing to do
         */
    }
    
    /**
     * 조건 생성
     * WHERE aFilterColumnIdx = aFilterValue
     */

    STL_TRY( knlInitBlock( & sFilterColumn,
                           KNL_NO_BLOCK_READ_CNT,
                           STL_TRUE, /* 컬럼임 */
                           STL_LAYER_EXECUTION_LIBRARY,
                           aTableID,
                           aFilterColumnIdx,
                           DTL_TYPE_NATIVE_BIGINT,
                           gDefaultDataTypeDef[DTL_TYPE_NATIVE_BIGINT].mArgNum1,
                           gDefaultDataTypeDef[DTL_TYPE_NATIVE_BIGINT].mArgNum2,
                           gDefaultDataTypeDef[DTL_TYPE_NATIVE_BIGINT].mStringLengthUnit,
                           gDefaultDataTypeDef[DTL_TYPE_NATIVE_BIGINT].mIntervalIndicator,
                           & aEnv->mMemDictOP,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  aTableID,
                                  sFilterColumn,
                                  & aFilterValue,
                                  STL_SIZEOF(stlInt64),
                                  & sIndexHandle,
                                  & sKeyCompList,
                                  & sIndexValueList,
                                  & sRangePred,
                                  & sRangeContext,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTargetColumn,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * DDL 관련 Object List 를 구성한다.
     */

    while(1)
    {
        /**
         * 레코드를 읽는다.
         */
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo, 
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        else
        {
            /**
             * Object ID 를 획득
             */
            
            sObjectValue = KNL_GET_BLOCK_DATA_VALUE( sTargetColumn, 0 );
            if ( DTL_IS_NULL( sObjectValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sObjectID,
                           sObjectValue->mValue,
                           sObjectValue->mLength );
            }

            if ( aTargetObjectType == ELL_OBJECT_TABLE )
            {
                /**
                 * Table Type 이 Sequence, Synonym 인지 확인
                 */

                sObjectValue = KNL_GET_BLOCK_DATA_VALUE( sTableType, 0 );
                stlMemcpy( & sTableTypeID,
                           sObjectValue->mValue,
                           sObjectValue->mLength );
                
                if ( ( sTableTypeID == ELL_TABLE_TYPE_SEQUENCE ) ||
                     ( sTableTypeID == ELL_TABLE_TYPE_SYNONYM ) )
                {
                    /**
                     * Naming Resolution 을 위해 저장된 Sequence, Synonym 임.
                     * - 읽지 말아야 할 대상임.
                     */
                    continue;
                }
                else
                {
                    /**
                     * Table 객체임
                     * - go go 
                     */
                }
            }
            
            /**
             * Object Handle 획득 
             */
            
            STL_TRY( sGetHandleFunc( aTransID,
                                     ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                     sObjectID,
                                     & sObjectHandle,
                                     & sObjectExist,
                                     aEnv )
                     == STL_SUCCESS );

            STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_OBJECT_NOT_EXIST );
            
            /**
             * DDL 관련 Object List 에 추가
             */
            
            STL_TRY( ellAddNewObjectItem( aObjectList,
                                          & sObjectHandle,
                                          NULL,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
        }
    }
    

    /**
     * SELECT 종료
     */
    
    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OBJECT_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INVALID_OBJECT,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }
    
    return STL_FAILURE;
}





/**
 * @brief 조건에 부합하는 Privilege List 를 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aPrivObject       Privilege Object 의 유형
 * @param[in]  aFilterColumnIdx  Filter Column 의 Ordinal Position
 * @param[in]  aFilterValue      Filter Value (Object ID)
 * @param[in]  aRegionMem        Region Memory
 * @param[out] aPrivList         Privilege List
 * @param[in]  aEnv          
 * @remarks
 */
stlStatus eldGetPrivList( smlTransId              aTransID,
                          smlStatement          * aStmt,
                          ellPrivObject           aPrivObject,
                          stlInt32                aFilterColumnIdx,
                          stlInt64                aFilterValue,
                          knlRegionMem          * aRegionMem,
                          ellPrivList           * aPrivList,
                          ellEnv                * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;

    eldtDefinitionTableID   sDictTableID = ELL_DICT_OBJECT_ID_NA;
    stlInt32                sDictGrantorColumnNO = 0;
    stlInt32                sDictGranteeColumnNO = 0;
    stlInt32                sDictObjectColumnNO = 0;
    stlInt32                sDictPrivColumnNO = 0;
    stlInt32                sDictGrantColumnNO = 0;
    
    
    void                  * sTableHandle = NULL;

    knlValueBlockList * sFilterColumn = NULL;
    
    void              * sIndexHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;

    smlFetchInfo          sFetchInfo;
    smlFetchRecordInfo    sFetchRecordInfo;

    dtlDataValue        * sDataValue = NULL;
    
    ellDictHandle         sPrivHandle;
    stlBool               sObjectExist = STL_FALSE;
    
    ellPrivDesc           sPrivDesc;

    ellPrivList  * sAllPrivList = NULL;

    eldMemMark     sMemMark;
    
    ellPrivItem   * sPrivItem = NULL;
    
    stlBool sDuplicate = STL_FALSE;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aPrivObject > ELL_PRIV_NA) && (aPrivObject < ELL_PRIV_OWNER),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterColumnIdx >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterValue > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivList != NULL, ELL_ERROR_STACK(aEnv) );
    

    /**
     * Privilege Descriptor 컬럼 정보 획득
     */
    
    /**
     * Dictionary Table 결정
     */
    
    switch( aPrivObject )
    {
        case ELL_PRIV_DATABASE:
            sDictTableID = ELDT_TABLE_ID_DATABASE_PRIVILEGES;
            sDictGrantorColumnNO = ELDT_DBPriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_DBPriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = 0;
            sDictPrivColumnNO    = ELDT_DBPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sDictGrantColumnNO   = ELDT_DBPriv_ColumnOrder_IS_GRANTABLE;
            break;
        case ELL_PRIV_SPACE:
            sDictTableID = ELDT_TABLE_ID_TABLESPACE_PRIVILEGES;
            sDictGrantorColumnNO = ELDT_SpacePriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_SpacePriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = ELDT_SpacePriv_ColumnOrder_TABLESPACE_ID;
            sDictPrivColumnNO    = ELDT_SpacePriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sDictGrantColumnNO   = ELDT_SpacePriv_ColumnOrder_IS_GRANTABLE;
            break;
        case ELL_PRIV_SCHEMA:
            sDictTableID = ELDT_TABLE_ID_SCHEMA_PRIVILEGES;
            sDictGrantorColumnNO = ELDT_SchemaPriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_SchemaPriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = ELDT_SchemaPriv_ColumnOrder_SCHEMA_ID;
            sDictPrivColumnNO    = ELDT_SchemaPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sDictGrantColumnNO   = ELDT_SchemaPriv_ColumnOrder_IS_GRANTABLE;
            break;
        case ELL_PRIV_TABLE:
            sDictTableID = ELDT_TABLE_ID_TABLE_PRIVILEGES;
            sDictGrantorColumnNO = ELDT_TablePriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_TablePriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = ELDT_TablePriv_ColumnOrder_TABLE_ID;
            sDictPrivColumnNO    = ELDT_TablePriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sDictGrantColumnNO   = ELDT_TablePriv_ColumnOrder_IS_GRANTABLE;
            break;
        case ELL_PRIV_USAGE:
            sDictTableID = ELDT_TABLE_ID_USAGE_PRIVILEGES;
            sDictGrantorColumnNO = ELDT_UsagePriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_UsagePriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = ELDT_UsagePriv_ColumnOrder_OBJECT_ID;
            sDictPrivColumnNO    = ELDT_UsagePriv_ColumnOrder_OBJECT_TYPE_ID;
            sDictGrantColumnNO   = ELDT_UsagePriv_ColumnOrder_IS_GRANTABLE;
            break;
        case ELL_PRIV_COLUMN:
            sDictTableID = ELDT_TABLE_ID_COLUMN_PRIVILEGES;
            sDictGrantorColumnNO = ELDT_ColumnPriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_ColumnPriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = ELDT_ColumnPriv_ColumnOrder_COLUMN_ID;
            sDictPrivColumnNO    = ELDT_ColumnPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sDictGrantColumnNO   = ELDT_ColumnPriv_ColumnOrder_IS_GRANTABLE;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**
     * Dictionary Table 에 대한 검색 조건 생성 
     */
    
    sTableHandle = gEldTablePhysicalHandle[sDictTableID];

    STL_TRY( eldAllocTableValueList( sDictTableID,
                                     & sMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            sDictTableID,
                                            aFilterColumnIdx );

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  sDictTableID,
                                  sFilterColumn,
                                  & aFilterValue,
                                  STL_SIZEOF(stlInt64),
                                  & sIndexHandle,
                                  & sKeyCompList,
                                  & sIndexValueList,
                                  & sRangePred,
                                  & sRangeContext,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * Privilege Object List 를 구성한다.
     */

    STL_TRY( ellInitPrivList( & sAllPrivList,
                              STL_TRUE,  /* aForRevoke, 모든 Privilege 목록을 얻기 위함 */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    while(1)
    {
        /**
         * 레코드를 읽는다.
         */
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo, 
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        else
        {
            /**
             * Privilege Descriptor 구성
             */

            /*
             * mGrantorID
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           sDictTableID,
                                           sDictGrantorColumnNO );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sPrivDesc.mGrantorID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sPrivDesc.mGrantorID) == sDataValue->mLength );
            }
            
            /*
             * mGranteeID
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           sDictTableID,
                                           sDictGranteeColumnNO );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sPrivDesc.mGranteeID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sPrivDesc.mGranteeID) == sDataValue->mLength );
            }
            
            /*
             * mObjectID
             */

            if ( aPrivObject == ELL_PRIV_DATABASE )
            {
                sPrivDesc.mObjectID = ellGetDbCatalogID();
            }
            else
            {
                sDataValue = eldFindDataValue( sTableValueList,
                                               sDictTableID,
                                               sDictObjectColumnNO );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sPrivDesc.mObjectID,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sPrivDesc.mObjectID) == sDataValue->mLength );
                }
            }
            
            /*
             * mPrivObject
             */
            
            sPrivDesc.mPrivObject = aPrivObject;
            
            /*
             * mPrivAction
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           sDictTableID,
                                           sDictPrivColumnNO );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sPrivDesc.mPrivAction.mAction,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sPrivDesc.mPrivAction.mAction) == sDataValue->mLength );
            }

            /*
             * mWithGrant
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           sDictTableID,
                                           sDictGrantColumnNO );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sPrivDesc.mWithGrant,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sPrivDesc.mWithGrant) == sDataValue->mLength );
            }
            
            /**
             * Privilege Handle 획득 
             */
            
            STL_TRY( eldcGetRevokePrivHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              aPrivObject,
                                              sPrivDesc.mGrantorID,
                                              sPrivDesc.mGranteeID,
                                              sPrivDesc.mObjectID,
                                              sPrivDesc.mPrivAction.mAction,
                                              & sPrivHandle,
                                              & sObjectExist,
                                              aEnv )
                     == STL_SUCCESS );

            STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_OBJECT_NOT_EXIST );
            
            /**
             * Privilege List 에 추가
             */
            
            STL_TRY( ellAddNewPrivItem( sAllPrivList,
                                        aPrivObject,
                                        & sPrivHandle,
                                        NULL,  /* aPrivDesc */
                                        & sDuplicate,
                                        aRegionMem,
                                        aEnv )
                     == STL_SUCCESS );

            /* 모든 Privilege Descroptor 정보를 검사하므로, 중복이 있어서는 안됨 */
            /* WITH GRANT OPTION 과 w/o GRANT OPTION 은 동시에 존재하지 않음 */
            STL_ASSERT( sDuplicate == STL_FALSE );            
        }
    }

    /**
     * SELECT 종료
     */
    
    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    /**
     * Output 설정
     */

    STL_RING_FOREACH_ENTRY( & sAllPrivList->mHeadList, sPrivItem )
    {
        STL_TRY( ellAddNewPrivItem( aPrivList,
                                    sPrivItem->mPrivDesc.mPrivObject,
                                    & sPrivItem->mPrivHandle,
                                    NULL,  /* aPrivDesc */
                                    & sDuplicate,
                                    aRegionMem,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OBJECT_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INVALID_OBJECT,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}



/**
 * @brief DDL Object 와 관련된 Dictionary Record 를 삭제한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aTableID          Dictionary Table ID
 * @param[in]  aFilterColumnIdx  Filter Column 의 Ordinal Position
 * @param[in]  aFilterValue      Filter Value (Object ID)
 * @param[in]  aEnv              Environment
 * @remarks
 * 다음과 같은 구문에 해당한다.
 * - DELETE FROM DEFINITION_SCHEMA.aTableID
 * - .WHERE aFilterColumnIdx = aFilterValue;
 */
stlStatus eldDeleteObjectRows( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               eldtDefinitionTableID   aTableID,
                               stlInt32                aFilterColumnIdx,
                               stlInt64                aFilterValue,
                               ellEnv                * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;
    
    knlValueBlockList * sFilterColumn = NULL;
    
    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlBool  sVersionConflict = STL_FALSE;
    stlBool  sSkipped = STL_FALSE;

    stlInt32 sState = 0;
   
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aTableID > ELDT_TABLE_ID_NA) &&
                        (aTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aTableID != ELDT_TABLE_ID_DATABASE_PRIVILEGES) &&
                        (aTableID != ELDT_TABLE_ID_TABLESPACE_PRIVILEGES) &&
                        (aTableID != ELDT_TABLE_ID_SCHEMA_PRIVILEGES) &&
                        (aTableID != ELDT_TABLE_ID_TABLE_PRIVILEGES) &&
                        (aTableID != ELDT_TABLE_ID_COLUMN_PRIVILEGES) &&
                        (aTableID != ELDT_TABLE_ID_USAGE_PRIVILEGES),
                        ELL_ERROR_STACK(aEnv) );
    
    STL_PARAM_VALIDATE( aFilterColumnIdx >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterValue > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.aTableID 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[aTableID];

    STL_TRY( eldAllocTableValueList( aTableID,
                                     & sMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            aTableID,
                                            aFilterColumnIdx );

    STL_RAMP( RAMP_RETRY );

    if ( sVersionConflict == STL_TRUE )
    {
        sBeginSelect = STL_FALSE;
        STL_TRY( eldEndForQuery( sIterator,
                                 aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlResetStatement( aStmt,
                                    STL_FALSE, /* aDoRollback */
                                    SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    /**
     * 조건 생성
     * WHERE aFilterColumnIdx = aFilterValue
     */

    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  aTableID,
                                  sFilterColumn,
                                  & aFilterValue,
                                  STL_SIZEOF(stlInt64),
                                  & sIndexHandle,
                                  & sKeyCompList,
                                  & sIndexValueList,
                                  & sRangePred,
                                  & sRangeContext,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * DELETE 준비
     */

    /**
     * Dictionary Read 전용 fetch record 정보 설정
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * 삭제할 Row 를 찾는다.
     */

    while(1)
    {
        /**
         * 레코드를 읽는다.
         */
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo, 
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        else
        {
            /**
             * 레코드 삭제 
             */

            STL_TRY( smlDeleteRecord( aStmt,
                                      sTableHandle,
                                      & sRowRid,
                                      sRowScn,
                                      0,     /* aValueIdx */
                                      NULL,  /* aPrimaryKey */
                                      & sVersionConflict,
                                      & sSkipped,
                                      SML_ENV(aEnv) )
                     == STL_SUCCESS );

            if ( sSkipped == STL_TRUE )
            {
                continue;
            }

            if ( sVersionConflict == STL_TRUE )
            {
                STL_THROW( RAMP_RETRY );
            }
            
            /**
             * 인덱스 갱신 및 무결성 검증
             */

            STL_TRY( eldRefineIntegrityRowDelete( aTransID,
                                                  aStmt,
                                                  STL_FALSE,   /* UPDATE 를 위한 삭제 여부 */
                                                  aTableID,
                                                  & sRowBlock,
                                                  sTableValueList,
                                                  aEnv )
                     == STL_SUCCESS );
        }
    }
    

    /**
     * DELETE 종료
     */
    
    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}








/**
 * @brief Privilege 와 관련된 Dictionary Record 와 Privilege Cache 를 삭제한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aTableID          Dictionary Table ID
 * @param[in]  aFilterColumnIdx  Filter Column 의 Ordinal Position
 * @param[in]  aFilterValue      Filter Value (Object ID)
 * @param[in]  aEnv              Environment
 * @remarks
 * 다음과 같은 구문에 해당한다.
 * - DELETE FROM DEFINITION_SCHEMA.aTableID
 * - .WHERE aFilterColumnIdx = aFilterValue;
 */
stlStatus eldDeletePrivRowsAndCache( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     eldtDefinitionTableID   aTableID,
                                     stlInt32                aFilterColumnIdx,
                                     stlInt64                aFilterValue,
                                     ellEnv                * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;
    
    knlValueBlockList * sFilterColumn = NULL;
    
    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    
    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;

    stlInt32            sGrantorColumnNO = 0;
    stlInt32            sGranteeColumnNO = 0;
    stlInt32            sObjectColumnNO = 0;
    stlInt32            sActionColumnNO = 0;
    
    ellPrivObject       sPrivObject = ELL_PRIV_NA;
    stlInt64            sGrantorID  = ELL_DICT_OBJECT_ID_NA;
    stlInt64            sGranteeID  = ELL_DICT_OBJECT_ID_NA;
    stlInt64            sObjectID   = ELL_DICT_OBJECT_ID_NA;
    stlInt32            sPrivAction = ELL_DICT_OBJECT_ID_NA;
    
    dtlDataValue     * sDataValue = NULL;
    eldMemMark         sMemMark;

    stlBool  sVersionConflict = STL_FALSE;
    stlBool  sSkipped = STL_FALSE;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aTableID > ELDT_TABLE_ID_NA) &&
                        (aTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aTableID == ELDT_TABLE_ID_DATABASE_PRIVILEGES) ||
                        (aTableID == ELDT_TABLE_ID_TABLESPACE_PRIVILEGES) ||
                        (aTableID == ELDT_TABLE_ID_SCHEMA_PRIVILEGES) ||
                        (aTableID == ELDT_TABLE_ID_TABLE_PRIVILEGES) ||
                        (aTableID == ELDT_TABLE_ID_COLUMN_PRIVILEGES) ||
                        (aTableID == ELDT_TABLE_ID_USAGE_PRIVILEGES),
                        ELL_ERROR_STACK(aEnv) );
    
    STL_PARAM_VALIDATE( aFilterColumnIdx >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterValue > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );

    /**
     * Privilege 관련 컬럼 정보 설정 
     */

    switch( aTableID )
    {
        case ELDT_TABLE_ID_DATABASE_PRIVILEGES:
            sGrantorColumnNO = ELDT_DBPriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_DBPriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = -1;
            sActionColumnNO  = ELDT_DBPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sPrivObject      = ELL_PRIV_DATABASE;
            break;
        case ELDT_TABLE_ID_TABLESPACE_PRIVILEGES:
            sGrantorColumnNO = ELDT_SpacePriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_SpacePriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = ELDT_SpacePriv_ColumnOrder_TABLESPACE_ID;
            sActionColumnNO  = ELDT_SpacePriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sPrivObject      = ELL_PRIV_SPACE;
            break;
        case ELDT_TABLE_ID_SCHEMA_PRIVILEGES:
            sGrantorColumnNO = ELDT_SchemaPriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_SchemaPriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = ELDT_SchemaPriv_ColumnOrder_SCHEMA_ID;
            sActionColumnNO  = ELDT_SchemaPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sPrivObject      = ELL_PRIV_SCHEMA;
            break;
        case ELDT_TABLE_ID_TABLE_PRIVILEGES:
            sGrantorColumnNO = ELDT_TablePriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_TablePriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = ELDT_TablePriv_ColumnOrder_TABLE_ID;
            sActionColumnNO  = ELDT_TablePriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sPrivObject      = ELL_PRIV_TABLE;
            break;
        case ELDT_TABLE_ID_COLUMN_PRIVILEGES:
            sGrantorColumnNO = ELDT_ColumnPriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_ColumnPriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = ELDT_ColumnPriv_ColumnOrder_COLUMN_ID;
            sActionColumnNO  = ELDT_ColumnPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sPrivObject      = ELL_PRIV_COLUMN;
            break;
        case ELDT_TABLE_ID_USAGE_PRIVILEGES:
            sGrantorColumnNO = ELDT_UsagePriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_UsagePriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = ELDT_UsagePriv_ColumnOrder_OBJECT_ID;
            sActionColumnNO  = ELDT_UsagePriv_ColumnOrder_OBJECT_TYPE_ID;
            sPrivObject      = ELL_PRIV_USAGE;
            break;
        default:
            STL_ASSERT(0);
            break;
    }
    
    /**
     * DEFINITION_SCHEMA.aTableID 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[aTableID];

    STL_TRY( eldAllocTableValueList( aTableID,
                                     & sMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            aTableID,
                                            aFilterColumnIdx );

    STL_RAMP( RAMP_RETRY );

    if ( sVersionConflict == STL_TRUE )
    {
        sBeginSelect = STL_FALSE;
        STL_TRY( eldEndForQuery( sIterator,
                                 aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlResetStatement( aStmt,
                                    STL_FALSE, /* aDoRollback */
                                    SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    /**
     * 조건 생성
     * WHERE aFilterColumnIdx = aFilterValue
     */

    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  aTableID,
                                  sFilterColumn,
                                  & aFilterValue,
                                  STL_SIZEOF(stlInt64),
                                  & sIndexHandle,
                                  & sKeyCompList,
                                  & sIndexValueList,
                                  & sRangePred,
                                  & sRangeContext,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * DELETE 준비
     */

    /**
     * Dictionary Read 전용 fetch record 정보 설정
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * 삭제할 Row 를 찾는다.
     */

    while(1)
    {
        /**
         * 레코드를 읽는다.
         */
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo, 
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        else
        {
            /*******************************************************
             * Privilege Row 삭제
             *******************************************************/

            /**
             * 레코드 삭제 
             */

            STL_TRY( smlDeleteRecord( aStmt,
                                      sTableHandle,
                                      & sRowRid,
                                      sRowScn,
                                      0,     /* aValueIdx */
                                      NULL,  /* aPrimaryKey */
                                      & sVersionConflict,
                                      & sSkipped,
                                      SML_ENV(aEnv) )
                     == STL_SUCCESS );

            if ( sSkipped == STL_TRUE )
            {
                continue;
            }

            if ( sVersionConflict == STL_TRUE )
            {
                STL_THROW( RAMP_RETRY );
            }
            
            /**
             * 인덱스 갱신 및 무결성 검증
             */

            STL_TRY( eldRefineIntegrityRowDelete( aTransID,
                                                  aStmt,
                                                  STL_FALSE,   /* UPDATE 를 위한 삭제 여부 */
                                                  aTableID,
                                                  & sRowBlock,
                                                  sTableValueList,
                                                  aEnv )
                     == STL_SUCCESS );

            /*******************************************************
             * Privilege Cache 삭제
             *******************************************************/

            /**
             * Privilege Argument 구성
             */

            /*
             * sGrantorID
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           aTableID,
                                           sGrantorColumnNO );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sGrantorID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sGrantorID) == sDataValue->mLength );
            }
    
            /*
             * sGranteeID
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           aTableID,
                                           sGranteeColumnNO );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sGranteeID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sGranteeID) == sDataValue->mLength );
            }

            /*
             * sObjectID
             */
            
            if ( sPrivObject == ELL_PRIV_DATABASE )
            {
                sObjectID = ellGetDbCatalogID();
            }
            else
            {
                sDataValue = eldFindDataValue( sTableValueList,
                                               aTableID,
                                               sObjectColumnNO );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sObjectID,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sObjectID) == sDataValue->mLength );
                }
            }

            /*
             * sPrivAction
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           aTableID,
                                           sActionColumnNO );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sPrivAction,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sPrivAction) == sDataValue->mLength );
            }
            
            /**
             * Privilege Cache 삭제
             */
            
            STL_TRY( eldcDeletePrivCache( aTransID,
                                          sPrivObject,
                                          sGrantorID,
                                          sGranteeID,
                                          sObjectID,
                                          sPrivAction,
                                          aEnv )
                     == STL_SUCCESS );
        }
    }
    

    /**
     * DELETE 종료
     */
    
    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}



/**
 * @brief Privilege Descriptor 에 해당하는 Dictionary Record 와 Privilege Cache 를 삭제한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aPrivDesc         Privilege Descriptor
 * @param[in]  aEnv              Environment
 * @remarks
 * 다음과 같은 구문에 해당한다.
 * - DELETE FROM DEFINITION_SCHEMA.PrivilegeTable
 * -  WHERE Primary Key = aPrivDesc;
 */
stlStatus eldDeletePrivRowAndCacheByPrivDesc( smlTransId     aTransID,
                                              smlStatement * aStmt,
                                              ellPrivDesc  * aPrivDesc,
                                              ellEnv       * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;

    ellDictHandle       sTableDictHandle;
    stlBool             sObjectExist = STL_FALSE;
    
    void              * sTablePhyHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;
    
    stlInt32            sPrimaryKeyCnt = 0;
    ellDictHandle     * sPrimaryKeyHandle = NULL;
    ellDictHandle     * sPrimaryIndexHandle = NULL;
    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    stlInt32            sIndexKeyCnt = 0;
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    knlValueBlockList * sKeyColumn = NULL;
    knlValueBlockList * sKeyValue = NULL;
    
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    
    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    eldtDefinitionTableID sDictTableID = ELDT_TABLE_ID_NA;
    
    stlInt32            sGrantorColumnNO = 0;
    stlInt32            sGranteeColumnNO = 0;
    stlInt32            sObjectColumnNO = 0;
    stlInt32            sActionColumnNO = 0;

    stlBool  sVersionConflict = STL_FALSE;
    stlBool  sSkipped = STL_FALSE;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Privilege 관련 컬럼 정보 설정 
     */

    switch( aPrivDesc->mPrivObject )
    {
        case ELL_PRIV_DATABASE:
            sDictTableID     = ELDT_TABLE_ID_DATABASE_PRIVILEGES;
            sGrantorColumnNO = ELDT_DBPriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_DBPriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = -1;
            sActionColumnNO  = ELDT_DBPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            break;
        case ELL_PRIV_SPACE:
            sDictTableID     = ELDT_TABLE_ID_TABLESPACE_PRIVILEGES;
            sGrantorColumnNO = ELDT_SpacePriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_SpacePriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = ELDT_SpacePriv_ColumnOrder_TABLESPACE_ID;
            sActionColumnNO  = ELDT_SpacePriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            break;
        case ELL_PRIV_SCHEMA:
            sDictTableID     = ELDT_TABLE_ID_SCHEMA_PRIVILEGES;
            sGrantorColumnNO = ELDT_SchemaPriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_SchemaPriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = ELDT_SchemaPriv_ColumnOrder_SCHEMA_ID;
            sActionColumnNO  = ELDT_SchemaPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            break;
        case ELL_PRIV_TABLE:
            sDictTableID     = ELDT_TABLE_ID_TABLE_PRIVILEGES;
            sGrantorColumnNO = ELDT_TablePriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_TablePriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = ELDT_TablePriv_ColumnOrder_TABLE_ID;
            sActionColumnNO  = ELDT_TablePriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            break;
        case ELL_PRIV_COLUMN:
            sDictTableID     = ELDT_TABLE_ID_COLUMN_PRIVILEGES;
            sGrantorColumnNO = ELDT_ColumnPriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_ColumnPriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = ELDT_ColumnPriv_ColumnOrder_COLUMN_ID;
            sActionColumnNO  = ELDT_ColumnPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            break;
        case ELL_PRIV_USAGE:
            sDictTableID     = ELDT_TABLE_ID_USAGE_PRIVILEGES;
            sGrantorColumnNO = ELDT_UsagePriv_ColumnOrder_GRANTOR_ID;
            sGranteeColumnNO = ELDT_UsagePriv_ColumnOrder_GRANTEE_ID;
            sObjectColumnNO  = ELDT_UsagePriv_ColumnOrder_OBJECT_ID;
            sActionColumnNO  = ELDT_UsagePriv_ColumnOrder_OBJECT_TYPE_ID;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /*************************************************************
     * Dictionary Table 과 Primary Key 정보 획득
     *************************************************************/
    
    /**
     * DEFINITION_SCHEMA.aTableID 테이블의 table handle 을 획득
     */
    
    sTablePhyHandle = gEldTablePhysicalHandle[sDictTableID];

    STL_TRY( eldAllocTableValueList( sDictTableID,
                                     & sMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellGetTableDictHandleByID( aTransID,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sDictTableID,
                                        & sTableDictHandle,
                                        & sObjectExist,
                                        aEnv )
             == STL_SUCCESS );
    STL_ASSERT( sObjectExist == STL_TRUE );

    /**
     * Primary Key Index Handle 획득
     */

    STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              & aEnv->mMemDictOP,
                                              & sTableDictHandle,
                                              & sPrimaryKeyCnt,
                                              & sPrimaryKeyHandle,
                                              aEnv )
             == STL_SUCCESS );

    STL_ASSERT( sPrimaryKeyCnt == 1 );

    sPrimaryIndexHandle = ellGetConstraintIndexDictHandle( sPrimaryKeyHandle );
    sIndexHandle = ellGetIndexHandle( sPrimaryIndexHandle );
    sIndexKeyCnt = ellGetIndexKeyCount( sPrimaryIndexHandle );

    STL_TRY( ellGetKeyCompareList( sPrimaryIndexHandle,
                                   & aEnv->mMemDictOP,
                                   & sKeyCompList,
                                   aEnv )
             == STL_SUCCESS );
                                   
    /*************************************************************
     * Composite Key Range 구성 
     *************************************************************/
    
    /**
     * Composite Key Range 생성
     * WHERE Primary Key( ... ) = Privilege Desc( ... )
     */

    STL_TRY( knlCreateRangePred( sIndexKeyCnt,
                                 STL_TRUE,
                                 & sRangePred,
                                 & sRangeContext,
                                 & aEnv->mMemDictOP,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( ellAllocIndexValueListForSELECT(
                 sPrimaryIndexHandle,
                 sIndexKeyCnt,
                 & aEnv->mMemDictOP,
                 KNL_NO_BLOCK_READ_CNT,
                 & sIndexValueList,
                 aEnv )
             == STL_SUCCESS );
    
    switch( aPrivDesc->mPrivObject )
    {
        case ELL_PRIV_DATABASE:
            {
                /**
                 * GRANTOR_ID
                 */
                
                sKeyColumn = ellFindColumnValueList( sIndexValueList,
                                                     sDictTableID,
                                                     sGrantorColumnNO );

                STL_TRY( knlInitCopyBlock( & sKeyValue,
                                           STL_LAYER_EXECUTION_LIBRARY,
                                           KNL_NO_BLOCK_READ_CNT,
                                           sKeyColumn,
                                           & aEnv->mMemDictOP,
                                           KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sKeyValue, 0 ),
                           & aPrivDesc->mGrantorID,
                           STL_SIZEOF(aPrivDesc->mGrantorID) );
                
                KNL_SET_BLOCK_DATA_LENGTH( sKeyValue, 0, STL_SIZEOF(aPrivDesc->mGrantorID) );
                
                sRangeContext->mContexts[ 0 ].mInfo.mValueInfo = sKeyColumn->mValueBlock;
                sRangeContext->mContexts[ 1 ].mInfo.mValueInfo = sKeyValue->mValueBlock;

                STL_TRY( knlAddRange( STL_TRUE,  /* aIsMinRange */
                                      0,         /* sKeyColumn */
                                      1,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                STL_TRY( knlAddRange( STL_FALSE, /* aIsMinRange */
                                      0,         /* sKeyColumn */
                                      1,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                /**
                 * GRANTEE_ID
                 */
                
                sKeyColumn = ellFindColumnValueList( sIndexValueList,
                                                     sDictTableID,
                                                     sGranteeColumnNO );

                STL_TRY( knlInitCopyBlock( & sKeyValue,
                                           STL_LAYER_EXECUTION_LIBRARY,
                                           KNL_NO_BLOCK_READ_CNT,
                                           sKeyColumn,
                                           & aEnv->mMemDictOP,
                                           KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sKeyValue, 0 ),
                           & aPrivDesc->mGranteeID,
                           STL_SIZEOF(aPrivDesc->mGranteeID) );
                
                KNL_SET_BLOCK_DATA_LENGTH( sKeyValue, 0, STL_SIZEOF(aPrivDesc->mGranteeID) );
                
                sRangeContext->mContexts[ 2 ].mInfo.mValueInfo = sKeyColumn->mValueBlock;
                sRangeContext->mContexts[ 3 ].mInfo.mValueInfo = sKeyValue->mValueBlock;

                STL_TRY( knlAddRange( STL_TRUE,  /* aIsMinRange */
                                      2,         /* sKeyColumn */
                                      3,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                STL_TRY( knlAddRange( STL_FALSE, /* aIsMinRange */
                                      2,         /* sKeyColumn */
                                      3,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                /**
                 * PRIVILEGE_TYPE_ID
                 */
                
                sKeyColumn = ellFindColumnValueList( sIndexValueList,
                                                     sDictTableID,
                                                     sActionColumnNO );

                STL_TRY( knlInitCopyBlock( & sKeyValue,
                                           STL_LAYER_EXECUTION_LIBRARY,
                                           KNL_NO_BLOCK_READ_CNT,
                                           sKeyColumn,
                                           & aEnv->mMemDictOP,
                                           KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sKeyValue, 0 ),
                           & aPrivDesc->mPrivAction.mAction,
                           STL_SIZEOF(aPrivDesc->mPrivAction.mAction) );
                
                KNL_SET_BLOCK_DATA_LENGTH( sKeyValue, 0, STL_SIZEOF(aPrivDesc->mPrivAction.mAction) );
                
                sRangeContext->mContexts[ 4 ].mInfo.mValueInfo = sKeyColumn->mValueBlock;
                sRangeContext->mContexts[ 5 ].mInfo.mValueInfo = sKeyValue->mValueBlock;

                STL_TRY( knlAddRange( STL_TRUE,  /* aIsMinRange */
                                      4,         /* sKeyColumn */
                                      5,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                STL_TRY( knlAddRange( STL_FALSE, /* aIsMinRange */
                                      4,         /* sKeyColumn */
                                      5,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                break;
            }
        case ELL_PRIV_SPACE:
        case ELL_PRIV_SCHEMA:
        case ELL_PRIV_TABLE:
        case ELL_PRIV_COLUMN:
        case ELL_PRIV_USAGE:
            {
                /**
                 * GRANTOR_ID
                 */
                
                sKeyColumn = ellFindColumnValueList( sIndexValueList,
                                                     sDictTableID,
                                                     sGrantorColumnNO );

                STL_TRY( knlInitCopyBlock( & sKeyValue,
                                           STL_LAYER_EXECUTION_LIBRARY,
                                           KNL_NO_BLOCK_READ_CNT,
                                           sKeyColumn,
                                           & aEnv->mMemDictOP,
                                           KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sKeyValue, 0 ),
                           & aPrivDesc->mGrantorID,
                           STL_SIZEOF(aPrivDesc->mGrantorID) );
                
                KNL_SET_BLOCK_DATA_LENGTH( sKeyValue, 0, STL_SIZEOF(aPrivDesc->mGrantorID) );
                
                sRangeContext->mContexts[ 0 ].mInfo.mValueInfo = sKeyColumn->mValueBlock;
                sRangeContext->mContexts[ 1 ].mInfo.mValueInfo = sKeyValue->mValueBlock;

                STL_TRY( knlAddRange( STL_TRUE,  /* aIsMinRange */
                                      0,         /* sKeyColumn */
                                      1,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                STL_TRY( knlAddRange( STL_FALSE, /* aIsMinRange */
                                      0,         /* sKeyColumn */
                                      1,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                /**
                 * GRANTEE_ID
                 */
                
                sKeyColumn = ellFindColumnValueList( sIndexValueList,
                                                     sDictTableID,
                                                     sGranteeColumnNO );

                STL_TRY( knlInitCopyBlock( & sKeyValue,
                                           STL_LAYER_EXECUTION_LIBRARY,
                                           KNL_NO_BLOCK_READ_CNT,
                                           sKeyColumn,
                                           & aEnv->mMemDictOP,
                                           KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sKeyValue, 0 ),
                           & aPrivDesc->mGranteeID,
                           STL_SIZEOF(aPrivDesc->mGranteeID) );
                
                KNL_SET_BLOCK_DATA_LENGTH( sKeyValue, 0, STL_SIZEOF(aPrivDesc->mGranteeID) );
                
                sRangeContext->mContexts[ 2 ].mInfo.mValueInfo = sKeyColumn->mValueBlock;
                sRangeContext->mContexts[ 3 ].mInfo.mValueInfo = sKeyValue->mValueBlock;

                STL_TRY( knlAddRange( STL_TRUE,  /* aIsMinRange */
                                      2,         /* sKeyColumn */
                                      3,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                STL_TRY( knlAddRange( STL_FALSE, /* aIsMinRange */
                                      2,         /* sKeyColumn */
                                      3,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                /**
                 * OBJECT_ID
                 */
                
                sKeyColumn = ellFindColumnValueList( sIndexValueList,
                                                     sDictTableID,
                                                     sObjectColumnNO );

                STL_TRY( knlInitCopyBlock( & sKeyValue,
                                           STL_LAYER_EXECUTION_LIBRARY,
                                           KNL_NO_BLOCK_READ_CNT,
                                           sKeyColumn,
                                           & aEnv->mMemDictOP,
                                           KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sKeyValue, 0 ),
                           & aPrivDesc->mObjectID,
                           STL_SIZEOF(aPrivDesc->mObjectID) );
                
                KNL_SET_BLOCK_DATA_LENGTH( sKeyValue, 0, STL_SIZEOF(aPrivDesc->mObjectID) );
                
                sRangeContext->mContexts[ 4 ].mInfo.mValueInfo = sKeyColumn->mValueBlock;
                sRangeContext->mContexts[ 5 ].mInfo.mValueInfo = sKeyValue->mValueBlock;

                STL_TRY( knlAddRange( STL_TRUE,  /* aIsMinRange */
                                      4,         /* sKeyColumn */
                                      5,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                STL_TRY( knlAddRange( STL_FALSE, /* aIsMinRange */
                                      4,         /* sKeyColumn */
                                      5,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                
                /**
                 * PRIVILEGE_TYPE_ID
                 */
                
                sKeyColumn = ellFindColumnValueList( sIndexValueList,
                                                     sDictTableID,
                                                     sActionColumnNO );

                STL_TRY( knlInitCopyBlock( & sKeyValue,
                                           STL_LAYER_EXECUTION_LIBRARY,
                                           KNL_NO_BLOCK_READ_CNT,
                                           sKeyColumn,
                                           & aEnv->mMemDictOP,
                                           KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sKeyValue, 0 ),
                           & aPrivDesc->mPrivAction.mAction,
                           STL_SIZEOF(aPrivDesc->mPrivAction.mAction) );
                
                KNL_SET_BLOCK_DATA_LENGTH( sKeyValue, 0, STL_SIZEOF(aPrivDesc->mPrivAction.mAction) );
                
                sRangeContext->mContexts[ 6 ].mInfo.mValueInfo = sKeyColumn->mValueBlock;
                sRangeContext->mContexts[ 7 ].mInfo.mValueInfo = sKeyValue->mValueBlock;

                STL_TRY( knlAddRange( STL_TRUE,  /* aIsMinRange */
                                      6,         /* sKeyColumn */
                                      7,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                STL_TRY( knlAddRange( STL_FALSE, /* aIsMinRange */
                                      6,         /* sKeyColumn */
                                      7,         /* sKeyValue */
                                      STL_TRUE,  /* aIsIncludeEqual */
                                      STL_TRUE,  /* aIsAsc */
                                      STL_FALSE, /* aIsNullFirst */
                                      STL_TRUE,  /* aIsNullAlwaysStop */
                                      sRangePred,
                                      KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                break;
            }
        default:
            STL_ASSERT(0);
            break;
    }
    
    STL_TRY( ellSetIndexColumnOrder( sIndexValueList, sPrimaryIndexHandle, aEnv ) == STL_SUCCESS );

    /*************************************************************
     * Dictionary Row 삭제 및 Dictionary Cache 제거 
     *************************************************************/

    STL_RAMP( RAMP_RETRY );

    if ( sVersionConflict == STL_TRUE )
    {
        sBeginSelect = STL_FALSE;
        STL_TRY( eldEndForQuery( sIterator,
                                 aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlResetStatement( aStmt,
                                    STL_FALSE, /* aDoRollback */
                                    SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    /**
     * DELETE 준비
     */

    /**
     * Dictionary Read 전용 fetch record 정보 설정
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTablePhyHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * 삭제할 Row 를 찾는다.
     */

    while(1)
    {
        /**
         * 레코드를 읽는다.
         */
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo, 
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        else
        {
            /*******************************************************
             * Privilege Record 삭제
             *******************************************************/
            
            /**
             * 레코드 삭제 
             */

            STL_TRY( smlDeleteRecord( aStmt,
                                      sTablePhyHandle,
                                      & sRowRid,
                                      sRowScn,
                                      0,     /* aValueIdx */
                                      NULL,  /* aPrimaryKey */
                                      & sVersionConflict,
                                      & sSkipped,
                                      SML_ENV(aEnv) )
                     == STL_SUCCESS );

            if ( sSkipped == STL_TRUE )
            {
                continue;
            }

            if ( sVersionConflict == STL_TRUE )
            {
                STL_THROW( RAMP_RETRY );
            }
            
            /**
             * 인덱스 갱신 및 무결성 검증
             */

            STL_TRY( eldRefineIntegrityRowDelete( aTransID,
                                                  aStmt,
                                                  STL_FALSE,   /* UPDATE 를 위한 삭제 여부 */
                                                  sDictTableID,
                                                  & sRowBlock,
                                                  sTableValueList,
                                                  aEnv )
                     == STL_SUCCESS );

            /*******************************************************
             * Privilege Cache 삭제
             *******************************************************/

            /**
             * Privilege Cache 삭제
             */
            
            STL_TRY( eldcDeletePrivCache( aTransID,
                                          aPrivDesc->mPrivObject,
                                          aPrivDesc->mGrantorID,
                                          aPrivDesc->mGranteeID,
                                          aPrivDesc->mObjectID,
                                          aPrivDesc->mPrivAction.mAction,
                                          aEnv )
                     == STL_SUCCESS );
        }
    }
    

    /**
     * DELETE 종료
     */
    
    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}




/**
 * @brief Table Desciptor 의 한 컬럼을 변경한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDictTableID     변경할 Dictionary Table ID
 * @param[in]  aObjectColumnIdx Object ID 컬럼 Idx
 * @param[in]  aModifyData      변경할 Data
 * @param[in]  aObjectID        Object ID
 * @param[in]  aModifyColumnIdx 변경할 컬럼 Idx
 * @param[in]  aModifyData      변경할 Data (nullable)
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus eldModifyDictDesc( smlTransId              aTransID,
                             smlStatement          * aStmt,
                             eldtDefinitionTableID   aDictTableID,
                             stlInt32                aObjectColumnIdx,
                             stlInt64                aObjectID,
                             stlInt32                aModifyColumnIdx,
                             void                  * aModifyData,
                             ellEnv                * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    knlValueBlockList * sRowValueList = NULL;

    eldMemMark          sMemMark;

    stlBool             sMemAlloc = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDictTableID > ELDT_TABLE_ID_NA) && (aDictTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectColumnIdx >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectID >= ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aModifyColumnIdx >= 0, ELL_ERROR_STACK(aEnv) );

    /**
     * 갱신을 위한 Record 삭제
     */

    STL_TRY( eldDeleteDictDesc4Modify( aTransID,
                                       aStmt,
                                       aDictTableID,
                                       aObjectColumnIdx,
                                       aObjectID,
                                       & sMemMark,
                                       & sRowValueList,
                                       aEnv )
             == STL_SUCCESS );
    sMemAlloc = STL_TRUE;
    
    /**
     * 컬럼 정보 변경 
     */

    STL_TRY( eldSetColumnValue( aDictTableID,
                                aModifyColumnIdx,
                                sRowValueList,
                                aModifyData,
                                aEnv )
             == STL_SUCCESS );

    /**
     * 변경 시간 설정 
     */

    switch (aDictTableID)
    {
        case ELDT_TABLE_ID_TABLES:
            STL_TRY( eldSetColumnValue( aDictTableID,
                                        ELDT_Tables_ColumnOrder_MODIFIED_TIME,
                                        sRowValueList,
                                        & sSessEnv->mTimeDDL,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELDT_TABLE_ID_COLUMNS:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_AUTHORIZATIONS:
            STL_TRY( eldSetColumnValue( aDictTableID,
                                        ELDT_Auth_ColumnOrder_MODIFIED_TIME,
                                        sRowValueList,
                                        & sSessEnv->mTimeDDL,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELDT_TABLE_ID_CATALOG_NAME:
            STL_TRY( eldSetColumnValue( aDictTableID,
                                        ELDT_Catalog_ColumnOrder_MODIFIED_TIME,
                                        sRowValueList,
                                        & sSessEnv->mTimeDDL,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELDT_TABLE_ID_CHECK_COLUMN_USAGE:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_CHECK_CONSTRAINTS:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_CHECK_TABLE_USAGE:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_COLUMN_PRIVILEGES:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_KEY_COLUMN_USAGE:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_SCHEMATA:
            STL_TRY( eldSetColumnValue( aDictTableID,
                                        ELDT_Schemata_ColumnOrder_MODIFIED_TIME,
                                        sRowValueList,
                                        & sSessEnv->mTimeDDL,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELDT_TABLE_ID_SEQUENCES:
            STL_TRY( eldSetColumnValue( aDictTableID,
                                        ELDT_Sequences_ColumnOrder_MODIFIED_TIME,
                                        sRowValueList,
                                        & sSessEnv->mTimeDDL,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELDT_TABLE_ID_TABLE_CONSTRAINTS:
            STL_TRY( eldSetColumnValue( aDictTableID,
                                        ELDT_TableConst_ColumnOrder_MODIFIED_TIME,
                                        sRowValueList,
                                        & sSessEnv->mTimeDDL,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELDT_TABLE_ID_TABLE_PRIVILEGES:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_USAGE_PRIVILEGES:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_VIEW_TABLE_USAGE:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_VIEWS:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_TYPE_INFO:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_DATABASE_PRIVILEGES:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_INDEXES:
            STL_TRY( eldSetColumnValue( aDictTableID,
                                        ELDT_Indexes_ColumnOrder_MODIFIED_TIME,
                                        sRowValueList,
                                        & sSessEnv->mTimeDDL,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELDT_TABLE_ID_SCHEMA_PRIVILEGES:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_TABLESPACE_PRIVILEGES:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_TABLESPACES:
            STL_TRY( eldSetColumnValue( aDictTableID,
                                        ELDT_Space_ColumnOrder_MODIFIED_TIME,
                                        sRowValueList,
                                        & sSessEnv->mTimeDDL,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELDT_TABLE_ID_USER_SCHEMA_PATH:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_USERS:
            /* nothing to do */
            break;
        case ELDT_TABLE_ID_PROFILES:
            STL_TRY( eldSetColumnValue( aDictTableID,
                                        ELDT_Profile_ColumnOrder_MODIFIED_TIME,
                                        sRowValueList,
                                        & sSessEnv->mTimeDDL,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        default:
            STL_ASSERT(0);
            break;
    }
        
    /**
     * 갱신된 Row 추가 
     */

    sMemAlloc = STL_FALSE;
    STL_TRY( eldInsertDictDesc4Modify( aTransID,
                                       aStmt,
                                       aDictTableID,
                                       & sMemMark,
                                       sRowValueList,
                                       aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sMemAlloc == STL_TRUE )
    {
        (void) eldFreeTableValueList( & sMemMark, aEnv );
    }
    
    return STL_FAILURE;
}


/**
 * @brief Object Dictionary Descriptor 를 갱신하기 위해 삭제한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aDictTableID       변경할 Dictionary Table ID
 * @param[in]  aObjectColumnIdx   Object ID 컬럼 Idx
 * @param[in]  aObjectID          Object ID
 * @param[out] aMemMark           Memory Marker
 * @param[out] aDeletedValueList  삭제된 Row 의 Value List
 * @param[in]  aEnv               Environment
 * @remarks
 * 함수 eldInsertDictDesc4Modify() 와 pair 로 사용해야 함.
 */
stlStatus eldDeleteDictDesc4Modify( smlTransId               aTransID,
                                    smlStatement           * aStmt,
                                    eldtDefinitionTableID    aDictTableID,
                                    stlInt32                 aObjectColumnIdx,
                                    stlInt64                 aObjectID,
                                    eldMemMark             * aMemMark,
                                    knlValueBlockList     ** aDeletedValueList,
                                    ellEnv                 * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    knlValueBlockList * sFilterColumn = NULL;
    
    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    smlRowBlock         sRowBlock;
    smlRid              sRowRid;
    smlScn              sRowScn;
    
    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;

    stlBool  sVersionConflict = STL_FALSE;
    stlBool  sSkipped = STL_FALSE;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDictTableID > ELDT_TABLE_ID_NA) && (aDictTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectColumnIdx >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectID >= ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMemMark != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDeletedValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * DEFINITION_SCHEMA.aDictTableID 테이블의 Row 정보 구성 
     */
    
    sTableHandle = gEldTablePhysicalHandle[aDictTableID];

    STL_TRY( eldAllocTableValueList( aDictTableID,
                                     aMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 조건 생성
     *  FROM DEFINITION_SCHEMA.aDictTableID
     * WHERE aObjectColumnIdx = aObjectID
     */
    
    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            aDictTableID,
                                            aObjectColumnIdx );

    STL_RAMP( RAMP_RETRY );

    if ( sVersionConflict == STL_TRUE )
    {
        sBeginSelect = STL_FALSE;
        STL_TRY( eldEndForQuery( sIterator,
                                 aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlResetStatement( aStmt,
                                    STL_FALSE, /* aDoRollback */
                                    SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  aDictTableID,
                                  sFilterColumn,
                                  & aObjectID,
                                  STL_SIZEOF(stlInt64),
                                  & sIndexHandle,
                                  & sKeyCompList,
                                  & sIndexValueList,
                                  & sRangePred,
                                  & sRangeContext,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Dictionary Read 전용 fetch record 정보 설정
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );
    
    sBeginSelect = STL_TRUE;

    /**
     * 레코드를 읽는다.
     */
    
    STL_TRY( eldFetchNext( sIterator,
                           & sFetchInfo, 
                           aEnv )
             == STL_SUCCESS );


    STL_ASSERT( sFetchInfo.mIsEndOfScan == STL_FALSE );

    /**
     * 레코드 삭제 
     */
    
    STL_TRY( smlDeleteRecord( aStmt,
                              sTableHandle,
                              & sRowRid,
                              sRowScn,
                              0,     /* aValueIdx */
                              NULL,  /* aPrimaryKey */
                              & sVersionConflict,
                              & sSkipped,
                              SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    if ( sSkipped == STL_TRUE )
    {
        STL_DASSERT(0);
        STL_THROW( RAMP_ERR_OBJECT_NOT_EXIST );
    }
    
    if ( sVersionConflict == STL_TRUE )
    {
        STL_THROW( RAMP_RETRY );
    }
            
    /**
     * Row 삭제에 대한 인덱스 갱신 및 무결성 검증
     */

    STL_TRY( eldRefineIntegrityRowDelete( aTransID,
                                          aStmt,
                                          STL_TRUE,   /* UPDATE 를 위한 삭제 여부 */
                                          aDictTableID,
                                          & sRowBlock,
                                          sTableValueList,
                                          aEnv )
             == STL_SUCCESS );

    /**
     * Iterator 종료
     */
    
    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    
    /**
     * Output 설정
     */

    *aDeletedValueList = sTableValueList;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OBJECT_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INVALID_OBJECT,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( aMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Object Dictionary Descriptor 를 갱신하기 위해 Row 를 추가한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aDictTableID       추가할 Dictionary Table ID
 * @param[in]  aMemMark           Memory Marker
 * @param[in]  aInsertValueList   추가할 Row 의 Value List
 * @param[in]  aEnv               Environment
 * @remarks
 * 함수 eldDeleteDictDesc4Modify() 호출 후 pair 로 사용해야 함.
 */
stlStatus eldInsertDictDesc4Modify( smlTransId               aTransID,
                                    smlStatement           * aStmt,
                                    eldtDefinitionTableID    aDictTableID,
                                    eldMemMark             * aMemMark,
                                    knlValueBlockList      * aInsertValueList,
                                    ellEnv                 * aEnv )
{
    stlBool  sMemAlloc = STL_TRUE;

    smlRowBlock         sRowBlock;
    smlRid              sRowRid;
    smlScn              sRowScn;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDictTableID > ELDT_TABLE_ID_NA) && (aDictTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMemMark != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInsertValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 갱신된 Row 추가 
     */

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aInsertValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[aDictTableID],
                              aInsertValueList,
                              NULL, /* unique violation */
                              & sRowBlock,
                              SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * Row 추가에 대한 인덱스 갱신 및 무결성 검증
     */
    
    STL_TRY( eldRefineIntegrityRowInsert( aTransID,
                                          aStmt,
                                          aDictTableID,
                                          & sRowBlock,
                                          aInsertValueList,
                                          aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sMemAlloc = STL_FALSE;
    STL_TRY( eldFreeTableValueList( aMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sMemAlloc == STL_TRUE )
    {
        (void) eldFreeTableValueList( aMemMark, aEnv );
    }
    
    return STL_FAILURE;
}

/** @} eldQuery */
