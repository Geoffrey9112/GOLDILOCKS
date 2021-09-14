/*******************************************************************************
 * kniProcess.c
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
 * @file kniProcess.c
 * @brief Kernel Layer Process Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <knlDef.h>
#include <knsSegment.h>
#include <knDef.h>
#include <knaLatch.h>
#include <kniProcess.h>

extern knsEntryPoint * gKnEntryPoint;

/**
 * @addtogroup kniProcess
 * @{
 */

/**
 * @brief 현재 Process의 정보
 */
stlProc  gProcessInfo;

/**
 * @brief 현재 프로세스의 Process Table ID
 */
stlInt32 gProcessTableID;

/**
 * @brief 현재 Process의 정보를 저장한다.
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kniSetCurrentProcessInfo( knlEnv * aEnv )
{
    kniProcessManager * sProcessManager;
    kniProcessInfo    * sProcessInfoArray;
    kniProcessInfo    * sProcessInfo;

    STL_TRY( stlGetCurrentProc( &gProcessInfo,
                                KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS);

    if( gProcessTableID != -1 )
    {
        sProcessManager = gKnEntryPoint->mProcessManager;

        sProcessInfoArray = (void*)sProcessManager + STL_SIZEOF( kniProcessManager );
        sProcessInfo      = &(sProcessInfoArray[gProcessTableID]);

        sProcessInfo->mProc = gProcessInfo;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Process Table을 생성한다.
 * @param[in] aMaxProcessCount 사용 가능한 프로세스의 최대 갯수
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kniCreateProcessTable( stlInt32   aMaxProcessCount,
                                 knlEnv   * aEnv )
{
    kniProcessManager * sProcessManager;
    kniProcessInfo    * sProcessInfo;
    stlInt32            sIdx;

    /*
     * 프로세스 관리자를 생성한다.
     */
    STL_TRY( knlCacheAlignedAllocFixedStaticArea( (STL_SIZEOF( kniProcessManager ) +
                                                   (STL_SIZEOF( kniProcessInfo ) *
                                                    aMaxProcessCount)),
                                                  (void**)&sProcessManager,
                                                  aEnv ) == STL_SUCCESS );

    sProcessManager->mMaxProcessCount  = aMaxProcessCount;
    sProcessManager->mFirstFreeProcess = 0;

    STL_TRY( knlInitLatch( &(sProcessManager->mLatch),
                           STL_TRUE,
                           aEnv ) == STL_SUCCESS );

    /*
     * 프로세스 Element를 초기화한다.
     */

    sProcessInfo = (void*)sProcessManager + STL_SIZEOF( kniProcessManager );
    
    for( sIdx = 0; sIdx < aMaxProcessCount; sIdx++ )
    {
        KNI_INIT_PROCESS_ID( &(sProcessInfo[sIdx].mProc) );

        STL_TRY( knlInitEvent( &(sProcessInfo[sIdx].mEvent),
                               aEnv ) == STL_SUCCESS );

        sProcessInfo[sIdx].mNextFreeProcess = sIdx + 1;
        sProcessInfo[sIdx].mAttachCount = 0;
        sProcessInfo[sIdx].mCreationTime = 0;
        sProcessInfo[sIdx].mProcessSeq = 0;
    }

    sProcessInfo[sIdx - 1].mNextFreeProcess = -1;

    gKnEntryPoint->mProcessManager = sProcessManager;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Process Table에 attach한다.
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kniAttachProcessTable( knlEnv * aEnv )
{
    stlInt32            sState = 0;
    kniProcessManager * sProcessManager;
    kniProcessInfo    * sProcessInfoArray;
    kniProcessInfo    * sProcessInfo = NULL;
    stlBool             sIsSuccess = STL_FALSE;
    stlInt32            sIdx;

    STL_TRY_THROW( gKnEntryPoint->mProcessManager != NULL,
                   RAMP_ERR_INVALID_STATIC_SHM );

    STL_ASSERT( gProcessInfo.mProcID != -1 );

    sProcessManager = gKnEntryPoint->mProcessManager;

    while( sIsSuccess == STL_FALSE )
    {
        STL_TRY( knlTryLatch( &(sProcessManager->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    }
    sState = 1;

    sProcessInfoArray = (void*)sProcessManager + STL_SIZEOF( kniProcessManager );

    for( sIdx = 0; sIdx < sProcessManager->mMaxProcessCount; sIdx++ )
    {
        sProcessInfo = &(sProcessInfoArray[sIdx]);

        if( stlIsEqualProc( &(sProcessInfo->mProc), &gProcessInfo ) == STL_TRUE )
        {
            break;
        }
    }

    if( sIdx != sProcessManager->mMaxProcessCount )
    {
        gProcessTableID = sIdx;
        sProcessInfo->mAttachCount++;
        aEnv->mProcessId = sIdx;
        aEnv->mProcessSeq = sProcessInfo->mProcessSeq;
    }
    else
    {
        gProcessTableID = sProcessManager->mFirstFreeProcess;
        
        STL_TRY_THROW( gProcessTableID != -1,
                       RAMP_ERR_EXCEEDED_PROCESS_COUNT );

        sProcessInfo = &(sProcessInfoArray[gProcessTableID]);

        sProcessManager->mFirstFreeProcess = sProcessInfo->mNextFreeProcess;

        sProcessInfo->mProc            = gProcessInfo;
        sProcessInfo->mAttachCount     = 1;
        sProcessInfo->mNextFreeProcess = -1;
        sProcessInfo->mCreationTime    = stlNow();
        sProcessInfo->mProcessSeq     += 1;
        
        aEnv->mProcessId = gProcessTableID;
        aEnv->mProcessSeq = sProcessInfo->mProcessSeq;
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &(sProcessManager->mLatch),
                              aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STATIC_SHM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_EXCEEDED_PROCESS_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_EXCEEDED_PROCESS_COUNT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            knlReleaseLatch( &(sProcessManager->mLatch),
                             aEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Process Table을 할당 받는다.
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kniAllocProcessTable( knlEnv * aEnv )
{
    stlInt32            sState = 0;
    kniProcessManager * sProcessManager;
    kniProcessInfo    * sProcessInfoArray;
    kniProcessInfo    * sProcessInfo;
    stlBool             sIsSuccess = STL_FALSE;
    stlInt32            sIdx;

    STL_TRY_THROW( gKnEntryPoint->mProcessManager != NULL,
                   RAMP_ERR_INVALID_STATIC_SHM );

    STL_ASSERT( gProcessInfo.mProcID != -1 );

    sProcessManager = gKnEntryPoint->mProcessManager;

    while( sIsSuccess == STL_FALSE )
    {
        STL_TRY( knlTryLatch( &(sProcessManager->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    }
    sState = 1;

    sProcessInfoArray = (void*)sProcessManager + STL_SIZEOF( kniProcessManager );

    for( sIdx = 0; sIdx < sProcessManager->mMaxProcessCount; sIdx++ )
    {
        sProcessInfo = &(sProcessInfoArray[sIdx]);

        if( stlIsEqualProc( &(sProcessInfo->mProc), &gProcessInfo ) == STL_TRUE )
        {
            break;
        }
    }

    STL_TRY_THROW( sIdx == sProcessManager->mMaxProcessCount,
                   RAMP_ERR_SERVER_REJECTED_THE_CONNECTION);
    
    gProcessTableID = sProcessManager->mFirstFreeProcess;
        
    STL_TRY_THROW( gProcessTableID != -1,
                   RAMP_ERR_EXCEEDED_PROCESS_COUNT);

    sProcessInfo = &(sProcessInfoArray[gProcessTableID]);

    sProcessManager->mFirstFreeProcess = sProcessInfo->mNextFreeProcess;

    sProcessInfo->mProc            = gProcessInfo;
    sProcessInfo->mAttachCount     = 0;
    sProcessInfo->mNextFreeProcess = -1;
    sProcessInfo->mCreationTime    = stlNow();
    sProcessInfo->mProcessSeq     += 1;

    aEnv->mProcessId = gProcessTableID;
    aEnv->mProcessSeq = sProcessInfo->mProcessSeq;
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &(sProcessManager->mLatch),
                              aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STATIC_SHM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_EXCEEDED_PROCESS_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_EXCEEDED_PROCESS_COUNT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_SERVER_REJECTED_THE_CONNECTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_SERVER_REJECTED_THE_CONNECTION,
                      "backend process identifier already exist in process table",
                      KNL_ERROR_STACK( aEnv ) );
        
        (void)knlLogMsgUnsafe( NULL,
                               aEnv,
                               KNL_LOG_LEVEL_WARNING,
                               "backend process identifier ( %d ) already exist in process table\n",
                               gProcessInfo.mProcID );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            knlReleaseLatch( &(sProcessManager->mLatch),
                             aEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 사용중인 Process Element에서 detach한다.
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kniDetachProcessTable( knlEnv * aEnv )
{
    stlInt32            sState = 0;
    kniProcessManager * sProcessManager;
    kniProcessInfo    * sProcessInfoArray;
    kniProcessInfo    * sProcessInfo;
    stlBool             sIsSuccess = STL_FALSE;

    STL_TRY_THROW( gKnEntryPoint->mProcessManager != NULL,
                   RAMP_ERR_INVALID_STATIC_SHM );

    STL_TRY_THROW( aEnv->mProcessId != -1, RAMP_ERR_INVALID_PROCESS_ID );

    sProcessManager = gKnEntryPoint->mProcessManager;

    while( sIsSuccess == STL_FALSE )
    {
        STL_TRY( knlTryLatch( &(sProcessManager->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    }
    sState = 1;

    sProcessInfoArray = (void*)sProcessManager + STL_SIZEOF( kniProcessManager );
    sProcessInfo = &(sProcessInfoArray[aEnv->mProcessId]);

    STL_TRY_THROW( sProcessInfo->mProc.mProcID != KNL_INVALID_PROCESS_ID,
                   RAMP_SUCCESS );

    STL_TRY_THROW( stlIsEqualProc(&sProcessInfo->mProc,
                                  &aEnv->mThread.mProc ) == STL_TRUE,
                   RAMP_SUCCESS );

    STL_TRY_THROW( sProcessInfo->mAttachCount > 0,
                   RAMP_SUCCESS );

    /**
     * cool-down 으로 인해서 이미 삭제된 경우는 무시한다.
     */
    STL_TRY_THROW( sProcessInfo->mProcessSeq == aEnv->mProcessSeq,
                   RAMP_SUCCESS );

    sProcessInfo->mAttachCount--;
    
    if( sProcessInfo->mAttachCount == 0 )
    {
        sProcessInfo->mNextFreeProcess = sProcessManager->mFirstFreeProcess;
        sProcessManager->mFirstFreeProcess = aEnv->mProcessId;

        KNI_INIT_PROCESS_ID( &(sProcessInfo->mProc) );
    }

    STL_RAMP( RAMP_SUCCESS );

    sState = 0;
    STL_TRY( knlReleaseLatch( &(sProcessManager->mLatch),
                              aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STATIC_SHM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_INVALID_PROCESS_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_PROCESS_ID,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
        
        (void)knlLogMsgUnsafe( NULL,
                               aEnv,
                               KNL_LOG_LEVEL_WARNING,
                               "negative process identifier\n" );
        
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            knlReleaseLatch( &(sProcessManager->mLatch),
                             aEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 사용중인 Process Element를 반환한다.
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus kniFreeProcessTable( knlEnv * aEnv )
{
    stlInt32            sState = 0;
    kniProcessManager * sProcessManager;
    kniProcessInfo    * sProcessInfoArray;
    kniProcessInfo    * sProcessInfo = NULL;
    stlBool             sIsSuccess = STL_FALSE;
    stlInt32            sIdx;

    STL_TRY_THROW( gKnEntryPoint->mProcessManager != NULL,
                   RAMP_ERR_INVALID_STATIC_SHM );

    /**
     * Warmup만 성공한 상태에서 Cooldown이 발생하면 gProcessTableID가 -1 일수 있다.
     */
    STL_TRY_THROW( gProcessTableID != -1, RAMP_FINISH );

    sProcessManager = gKnEntryPoint->mProcessManager;

    while( sIsSuccess == STL_FALSE )
    {
        STL_TRY( knlTryLatch( &(sProcessManager->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    }
    sState = 1;

    sProcessInfoArray = (void*)sProcessManager + STL_SIZEOF( kniProcessManager );
    
    for( sIdx = 0; sIdx < sProcessManager->mMaxProcessCount; sIdx++ )
    {
        sProcessInfo = &(sProcessInfoArray[sIdx]);

        if( stlIsEqualProc( &(sProcessInfo->mProc), &gProcessInfo ) == STL_TRUE )
        {
            break;
        }
    }

    if( sIdx != sProcessManager->mMaxProcessCount )
    {
        /**
         * 이미 삭제된 경우는 무시한다.
         */
        STL_TRY_THROW( sProcessInfo->mProc.mProcID != KNL_INVALID_PROCESS_ID,
                       RAMP_SUCCESS );

        STL_TRY_THROW( stlIsEqualProc(&sProcessInfo->mProc,
                                      &aEnv->mThread.mProc ) == STL_TRUE,
                       RAMP_SUCCESS );
        
        sProcessInfo->mAttachCount = 0;
        sProcessInfo->mNextFreeProcess = sProcessManager->mFirstFreeProcess;
        sProcessManager->mFirstFreeProcess = gProcessTableID;

        KNI_INIT_PROCESS_TABLE_ID();
        KNI_INIT_PROCESS_ID( &(sProcessInfo->mProc) );
    }

    STL_RAMP( RAMP_SUCCESS );

    sState = 0;
    STL_TRY( knlReleaseLatch( &(sProcessManager->mLatch),
                              aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STATIC_SHM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            knlReleaseLatch( &(sProcessManager->mLatch),
                             aEnv );
            break;
    }

    return STL_FAILURE;
}

/*
 * X$PROCESS_INFO
 */
/**
 * @brief X$PROCESS_INFO Table 정의
 */
knlFxColumnDesc gProcessInfoColumnDesc[] =
{
    {
        "ID",
        "process ID",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( kniProcessInfoFxRecord, mID ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "BACKEND_PID",
        "back-end process ID",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( kniProcessInfoFxRecord, mBackendPid ),
        STL_SIZEOF( stlProc ),
        STL_FALSE  /* nullable */
    },
    {
        "ATTACH_COUNT",
        "attached environment count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( kniProcessInfoFxRecord, mAttachCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "CREATION_TIME",
        "creation time",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( kniProcessInfoFxRecord, mCreationTime ),
        STL_SIZEOF( stlTime ),
        STL_FALSE  /* nullable */
    },
    {
        "SEQUENCE",
        "process sequence",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( kniProcessInfoFxRecord, mProcessSeq ),
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
 * @brief X$PROCESS_INFO Table의 open 함수
 * @param[in] aStmt Statement Pointer
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus openFxProcessInfoCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    stlInt32 * sCtl = (stlInt32*)aPathCtrl;

    *sCtl = 0;

    return STL_SUCCESS;
}

/**
 * @brief X$PROCESS_INFO Table의 close 함수
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus closeFxProcessInfoCallback( void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$PROCESS_INFO Table의 레코드 구축 함수
 * @param[in] aDumpObjHandle Dump Object Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aValueList 값을 담을 공간
 * @param[in]  aBlockIdx   Block Idx
 * @param[out] aTupleExist 레코드 존재 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus buildRecordFxProcessInfoCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aEnv )
{
    stlInt32             * sCtl = (stlInt32*)aPathCtrl;
    kniProcessManager    * sProcessManager;
    kniProcessInfo       * sProcessInfoArray;
    kniProcessInfo       * sProcessInfo;
    stlInt32               sState = 0;
    stlBool                sIsSuccess;
    stlInt32               sIdx;
    kniProcessInfoFxRecord sRec;

    *aTupleExist = STL_FALSE;

    STL_TRY_THROW( gKnEntryPoint->mProcessManager != NULL,
                   RAMP_ERR_INVALID_STATIC_SHM );

    sProcessManager = gKnEntryPoint->mProcessManager;

    STL_TRY( knlAcquireLatch( &(sProcessManager->mLatch),
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsSuccess,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    sProcessInfoArray = (void*)sProcessManager + STL_SIZEOF( kniProcessManager );

    for( sIdx = *sCtl; sIdx < sProcessManager->mMaxProcessCount; sIdx++ )
    {
        sProcessInfo = &(sProcessInfoArray[sIdx]);

        if( sProcessInfo->mProc.mProcID == -1 )
        {
            continue;
        }
        else
        {
            sRec.mID = sIdx;
            sRec.mBackendPid = sProcessInfo->mProc;
            sRec.mAttachCount = sProcessInfo->mAttachCount;
            sRec.mCreationTime = sProcessInfo->mCreationTime;
            sRec.mProcessSeq = sProcessInfo->mProcessSeq;

            STL_TRY( knlBuildFxRecord( gProcessInfoColumnDesc,
                                       &sRec,
                                       aValueList,
                                       aBlockIdx,
                                       aEnv )
                     == STL_SUCCESS );
            
            *aTupleExist = STL_TRUE;
            *sCtl = sIdx + 1;

            break;
        }
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &(sProcessManager->mLatch),
                              aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STATIC_SHM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_STATIC_SHM,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            knlReleaseLatch( &(sProcessManager->mLatch),
                             aEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief X$PROCESS_INFO Table 정보
 */
knlFxTableDesc gProcessInfoTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxProcessInfoCallback,
    closeFxProcessInfoCallback,
    buildRecordFxProcessInfoCallback,
    STL_SIZEOF( kniProcessInfoFxRecord ),
    "X$PROCESS_INFO",
    "process information fixed table",
    gProcessInfoColumnDesc
};


/** @} */
