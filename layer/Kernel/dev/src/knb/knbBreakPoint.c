/*******************************************************************************
 * knbBreakPoint.c
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
 * @file knbBreakPoint.c
 * @brief Kernel Layer Break Point Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knlShmManager.h>
#include <knlDynamicMem.h>
#include <knDef.h>
#include <knbBreakPoint.h>
#include <knlTraceLogger.h>
#include <knlSession.h>
#include <kniStartup.h>

/**
 * @addtogroup knbBreakPoint
 * @{
 */

extern stlChar       * gKnlBpNames[];
extern knsEntryPoint * gKnEntryPoint;


stlStatus knbInitializeBpInfo( knbBreakPointInfo ** aBpInfo,
                               knlEnv             * aEnv )
{
    stlInt16   i;

    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF(knbBreakPointInfo),
                                      (void**)aBpInfo,
                                      aEnv ) == STL_SUCCESS );

    (*aBpInfo)->mActionCount   = 0;
    (*aBpInfo)->mCurActionSeq  = 0;
    (*aBpInfo)->mProcCount = 0;

    for( i = 0; i < KNL_MAX_ACTION_COUNT; i++ )
    {
        (*aBpInfo)->mArray[i].mBpId = KNL_BREAKPOINT_MAX;
        (*aBpInfo)->mArray[i].mSessType = KNL_BREAKPOINT_SESSION_TYPE_NONE;
        (*aBpInfo)->mArray[i].mProcId = -1;
        (*aBpInfo)->mArray[i].mSkipCount = -1;
        (*aBpInfo)->mArray[i].mActionType = KNL_BREAKPOINT_ACTION_MAX;
        (*aBpInfo)->mArray[i].mValue = -1;
    }

    for( i = 0; i < KNL_MAX_BP_PROCESS_COUNT; i++ )
    {
        (*aBpInfo)->mProcList[i].mProcName[0] = '\0';
        (*aBpInfo)->mProcList[i].mProcEnv = NULL;
        (*aBpInfo)->mProcList[i].mStatus = KNL_PROC_STATUS_IDLE;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus knbRegisterProcess( stlChar  * aName,
                              knlEnv   * aEnv )
{
    stlInt32   sProcId = gKnEntryPoint->mBreakPointInfo->mProcCount;

    stlStrcpy( gKnEntryPoint->mBreakPointInfo->mProcList[sProcId].mProcName, aName );
    gKnEntryPoint->mBreakPointInfo->mProcList[sProcId].mProcName[stlStrlen(aName)] = '\0';
    gKnEntryPoint->mBreakPointInfo->mProcList[sProcId].mProcEnv = aEnv;
    gKnEntryPoint->mBreakPointInfo->mProcList[sProcId].mStatus = KNL_PROC_STATUS_IDLE;
    gKnEntryPoint->mBreakPointInfo->mProcCount++;

    return STL_SUCCESS;
}

void knbFindProcIdFromName( stlChar    * aProcName,
                            stlInt32   * aProcId )
{
    stlInt32 i;

    *aProcId = -1;
    
    for( i = 0; i < gKnEntryPoint->mBreakPointInfo->mProcCount; i ++ )
    {
        if( stlStrcmp( aProcName, gKnEntryPoint->mBreakPointInfo->mProcList[i].mProcName ) == 0 )
        {
            break;
        }
    }

    if( i >= gKnEntryPoint->mBreakPointInfo->mProcCount )
    {
        *aProcId = -1;
    }
    else
    {
        *aProcId = i;
    }
}



void knbFindBreakpointIdFromName( stlChar         * aBpName,
                                  knlBreakPointId * aBpId )
{
    stlInt32 i;
    
    for( i = 0; i < KNL_BREAKPOINT_MAX; i ++ )
    {
        if( stlStrcmp( aBpName, gKnlBpNames[i] ) == 0 )
        {
            break;
        }
    }

    *aBpId = (knlBreakPointId)i;
}


stlStatus knbRegisterAction( knlBreakPointId           aBpId,
                             knlBreakPointSessType     aSessType,
                             stlInt32                  aOwnerProcId,
                             stlInt32                  aSkipCount,
                             knlBreakPointAction       aActionType,
                             knlBreakPointPostAction   aPostActionType,
                             stlInt32                  aValue,
                             knlEnv                  * aEnv )
{
    stlInt16    sBpSeq = gKnEntryPoint->mBreakPointInfo->mActionCount;
    knbAction * sAction = &gKnEntryPoint->mBreakPointInfo->mArray[sBpSeq];

    sAction->mProcId         = aOwnerProcId;
    sAction->mSessType       = aSessType;
    sAction->mBpId           = aBpId;
    sAction->mSkipCount      = aSkipCount;
    sAction->mActionType     = aActionType;
    sAction->mValue          = aValue;
    sAction->mPostActionType = aPostActionType;

    gKnEntryPoint->mBreakPointInfo->mActionCount++;

    return STL_SUCCESS;
}


stlStatus knbClearActionInfo( knlEnv   * aEnv )
{
    stlInt16         i;
    knlEnv         * sProcEnv;
    knlLatchWaiter * sWaiter;

    /* 설정된 모든 break point를 삭제한다 */
    for( i = 0; i < KNL_MAX_ACTION_COUNT; i++ )
    {
        gKnEntryPoint->mBreakPointInfo->mArray[i].mBpId = KNL_BREAKPOINT_MAX;
        gKnEntryPoint->mBreakPointInfo->mArray[i].mSessType = KNL_BREAKPOINT_SESSION_TYPE_NONE;
        gKnEntryPoint->mBreakPointInfo->mArray[i].mProcId = -1;
        gKnEntryPoint->mBreakPointInfo->mArray[i].mSkipCount = -1;
        gKnEntryPoint->mBreakPointInfo->mArray[i].mActionType = KNL_BREAKPOINT_ACTION_MAX;
        gKnEntryPoint->mBreakPointInfo->mArray[i].mPostActionType = KNL_BREAKPOINT_POST_ACTION_MAX;
        gKnEntryPoint->mBreakPointInfo->mArray[i].mValue = -1;
    }

    /* bp count를 설정한다 */
    gKnEntryPoint->mBreakPointInfo->mActionCount   = 0;
    gKnEntryPoint->mBreakPointInfo->mCurActionSeq  = 0;

    /* process들을 깨운다  */
    for( i = 0; i < KNL_MAX_BP_PROCESS_COUNT; i++ )
    {
        if( gKnEntryPoint->mBreakPointInfo->mProcList[i].mStatus == KNL_PROC_STATUS_WAITING )
        {
            sProcEnv = gKnEntryPoint->mBreakPointInfo->mProcList[i].mProcEnv;
            sWaiter = KNL_WAITITEM(sProcEnv);
            gKnEntryPoint->mBreakPointInfo->mProcList[i].mStatus = KNL_PROC_STATUS_IDLE;
            STL_TRY( stlReleaseSemaphore( &sWaiter->mSem, KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus knbClearProcessInfo( knlEnv   * aEnv )
{
    stlInt16         i;

    /* process count를 리셋한다 */
    gKnEntryPoint->mBreakPointInfo->mProcCount = 0;

    /* breakpoint table을 reset하고 process들을 깨운다 */
    STL_TRY( knbClearActionInfo( aEnv ) == STL_SUCCESS );

    /* process table을 reset한다 */
    for( i = 0; i < KNL_MAX_BP_PROCESS_COUNT; i++ )
    {
        gKnEntryPoint->mBreakPointInfo->mProcList[i].mProcName[0] = '\0';
        gKnEntryPoint->mBreakPointInfo->mProcList[i].mProcEnv = NULL;
        gKnEntryPoint->mBreakPointInfo->mProcList[i].mStatus = KNL_PROC_STATUS_IDLE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlInt32  knbGetActionCount( )

{
    return gKnEntryPoint->mBreakPointInfo->mActionCount;
}

stlInt32  knbGetProcCount( )

{
    return gKnEntryPoint->mBreakPointInfo->mProcCount;
}

stlStatus knbDoBreakPoint( knlBreakPointId   aBpId,
                           knlEnv          * aEnv )
{
    stlInt16     sCurActionSeq = gKnEntryPoint->mBreakPointInfo->mCurActionSeq;
    knbAction  * sCurAction = &gKnEntryPoint->mBreakPointInfo->mArray[sCurActionSeq];
    knlEnv     * sNextProcEnv;
    knlEnv     * sCurProcEnv;
/*    stlChar          sLogMsg[100]; */
    knlLatchWaiter * sMyWaiter;
    knlLatchWaiter * sNextWaiter;
    stlBool          sIsTimedOut = STL_TRUE;
    knlBreakPointPostAction sPostActionType;

    /* breakpoint를 아무도 사용중이지 않거나 모두 지나갔으면 skip */
    STL_TRY_THROW( (gKnEntryPoint->mBreakPointInfo->mProcCount > 0) &&
                   (sCurActionSeq < gKnEntryPoint->mBreakPointInfo->mActionCount),
                   RAMP_SKIP_BP );

    if( sCurAction->mSessType == KNL_BREAKPOINT_SESSION_TYPE_USER )
    {
        /* 현재 Proc의 BPAction이 아니면 skip */
        STL_TRY_THROW( (gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcEnv
                        == aEnv), RAMP_SKIP_BP );
    }
    else
    {
        /* System session의 breakpoint는 Proc를 check하지 않는다. */
        STL_TRY_THROW( sCurAction->mSessType == KNL_BREAKPOINT_SESSION_TYPE_SYSTEM,
                       RAMP_SKIP_BP );
    }

    STL_TRY_THROW( aBpId == sCurAction->mBpId, RAMP_SKIP_BP );

    /**
     * Register된 Process env를 이용한다.
     * SYSTEM session의 경우 SYSTEM proc env가 아니라 Register된 session env를
     * 이용하기 때문에 이를 이용하여 연산(memory alloc, concurrency control 등) 시 주의해야 한다.
     */
    if( sCurAction->mSessType == KNL_BREAKPOINT_SESSION_TYPE_SYSTEM )
    {
        sCurProcEnv = gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcEnv;
    }
    else
    {
        sCurProcEnv = aEnv;
    }

    if( sCurAction->mSkipCount > 0 )
    {
        /*
        stlSnprintf( sLogMsg,
                     100,
                     ">>> %s decreases skip count %d -> %d",
                     gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcName,
                     sCurAction->mSkipCount, sCurAction->mSkipCount - 1 );
        knlLogMsg( NULL, aEnv, KNL_LOG_LEVEL_INFO, "%s", sLogMsg);
*/
        sCurAction->mSkipCount--;
    }
    else
    {
        switch( sCurAction->mActionType )
        {
            case KNL_BREAKPOINT_ACTION_WAKEUP:
                {
                    /* next process를 깨워줌 */
                    sNextProcEnv = gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mValue].mProcEnv;
                    sNextWaiter = KNL_WAITITEM(sNextProcEnv);
                    gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mValue].mStatus = KNL_PROC_STATUS_ACTIVE;
                   /* 
                    stlSnprintf( sLogMsg,
                                 100,
                                 ">>> %s Awaking %s By Breakpoint %d",
                                 gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcName,
                                 gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mValue].mProcName,
                                 sCurActionSeq);
                    knlLogMsg( NULL, aEnv, KNL_LOG_LEVEL_INFO, "%s", sLogMsg);
*/
                    sPostActionType = sCurAction->mPostActionType;
                    
                    /* 현 process는 sleep 하다가 나간다 */
                    sMyWaiter = KNL_WAITITEM( sCurProcEnv );
                    gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mStatus = KNL_PROC_STATUS_WAITING;
                    
                    gKnEntryPoint->mBreakPointInfo->mCurActionSeq++;
                    STL_TRY( stlReleaseSemaphore( &sNextWaiter->mSem, KNL_ERROR_STACK(sCurProcEnv) )
                             == STL_SUCCESS );
                    
                    while( STL_TRUE )
                    {
                        STL_TRY( stlTimedAcquireSemaphore( &sMyWaiter->mSem,
                                                           STL_INFINITE_TIME,
                                                           &sIsTimedOut,
                                                           KNL_ERROR_STACK(sCurProcEnv) )
                                 == STL_SUCCESS );

                        if( (gKnEntryPoint->mBreakPointInfo->mProcCount == 0) ||
                            (gKnEntryPoint->mBreakPointInfo->mActionCount == 0) )
                        {
                            /* 모든 breakpoint를 수행했거나 reset되었으면 나간다 */
                            break;
                        }
                        else
                        {
                            sCurActionSeq = gKnEntryPoint->mBreakPointInfo->mCurActionSeq;
                            if( sCurActionSeq < gKnEntryPoint->mBreakPointInfo->mActionCount )
                            {
                                sCurAction = &gKnEntryPoint->mBreakPointInfo->mArray[sCurActionSeq];
                                if( gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcEnv == sCurProcEnv )
                                {
                                    /* 현재 프로세스가 수행할 차례이면 나간다 */ 
                                    break;
                                }
                                else
                                {
                                    /* 내가 깨어날 차례가 아닌데 깨어났다 : BUG */
                                    STL_ASSERT( STL_FALSE );
                                }
                            }
                            else
                            {
                                /* 마지막 breakpoint가 대기하고 있던 나를 다시 깨운다 */
                                STL_ASSERT( sCurActionSeq == gKnEntryPoint->mBreakPointInfo->mActionCount );
                                break;
                            }
                        }
                    }/* while */

/*                     stlSnprintf( sLogMsg, */
/*                                  100, */
/*                                  ">>> %s Awaked From Sleep After Breakpoint %d", */
/*                                  gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcName, */
/*                                  gKnEntryPoint->mBreakPointInfo->mCurActionSeq); */
/*                     knlLogMsg( NULL, aEnv, KNL_LOG_LEVEL_INFO, "%s", sLogMsg); */
                    
                    if( sPostActionType == KNL_BREAKPOINT_POST_ACTION_ABORT )
                    {
                        STL_THROW( RAMP_ERR_BREAKPOINT_GENERAL_ABORT );
                    }
                    else
                    {
                        if( sPostActionType == KNL_BREAKPOINT_POST_ACTION_SESSION_KILL )
                        {
                            knlKillSession( KNL_SESS_ENV(sCurProcEnv) );
                            stlExitProc( -1 );
                        }
                        else
                        {
                            if( sPostActionType == KNL_BREAKPOINT_POST_ACTION_SYSTEM_KILL )
                            {
                                kniSystemFatal( STL_FALSE, STL_FALSE );
                                stlExitProc( -1 );
                            }
                        }
                    }
                    break;
                }
            case KNL_BREAKPOINT_ACTION_SLEEP:
                {
                    /*
                    stlSnprintf( sLogMsg,
                                 100,
                                 ">>> %s Sleep By Breakpoint %d",
                                 gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcName,
                                 gKnEntryPoint->mBreakPointInfo->mCurActionSeq);
                    knlLogMsg( NULL, aEnv, KNL_LOG_LEVEL_INFO, "%s", sLogMsg);
*/
                    stlSleep(((stlInt64)sCurAction->mValue) * 1000000L);
                    gKnEntryPoint->mBreakPointInfo->mCurActionSeq++;
/*
                    stlSnprintf( sLogMsg,
                                 100,
                                 ">>> %s Awaked From Sleep By Breakpoint %d",
                                 gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcName,
                                 gKnEntryPoint->mBreakPointInfo->mCurActionSeq);
                    knlLogMsg( NULL, aEnv, KNL_LOG_LEVEL_INFO, "%s", sLogMsg);
                    */
                    break;
                }
            case KNL_BREAKPOINT_ACTION_ABORT:
                {
                    /*
                    stlSnprintf( sLogMsg,
                                 100,
                                 ">>> %s Aborted By Breakpoint %d",
                                 gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcName,
                                 gKnEntryPoint->mBreakPointInfo->mCurActionSeq);
                    knlLogMsg( NULL, aEnv, KNL_LOG_LEVEL_FATAL, "%s", sLogMsg);
                    */
                    gKnEntryPoint->mBreakPointInfo->mCurActionSeq++;
                    STL_THROW( RAMP_ERR_BREAKPOINT_GENERAL_ABORT );
                    break;
                }
            case KNL_BREAKPOINT_ACTION_KILL:
                {
                    /*
                    stlSnprintf( sLogMsg,
                                 100,
                                 ">>> %s Killed By Breakpoint %d",
                                 gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcName,
                                 gKnEntryPoint->mBreakPointInfo->mCurActionSeq);
                    knlLogMsg( NULL, aEnv, KNL_LOG_LEVEL_FATAL, "%s", sLogMsg);
                    */
                    gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mStatus = KNL_PROC_STATUS_IDLE;
                    gKnEntryPoint->mBreakPointInfo->mCurActionSeq++;

                    if( sCurAction->mSessType == KNL_BREAKPOINT_SESSION_TYPE_SYSTEM )
                    {
                        kniSystemFatal( STL_FALSE, STL_FALSE );
                    }

                    knlKillSession( KNL_SESS_ENV(sCurProcEnv) );
                    stlExitProc( -1 );
                    break;
                }
            default :
                {
                    STL_ASSERT( STL_FALSE );
                }
        }
    }

    STL_RAMP( RAMP_SKIP_BP );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BREAKPOINT_GENERAL_ABORT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_BREAKPOINT_GENERAL_ABORT,
                      NULL,
                      KNL_ERROR_STACK(sCurProcEnv) );
    }
    STL_FINISH;

    return STL_FAILURE;
}


stlStatus knbBreakPointWaitToStart( knlEnv * aEnv )
{
    stlInt32         sCurActionSeq;
    knbAction      * sCurAction;
    knlLatchWaiter * sMyWaiter;
    stlBool          sIsTimedOut;
    stlInt32         sProcCount = gKnEntryPoint->mBreakPointInfo->mProcCount;
    stlInt32         sMyProcId;
    stlInt32         i;
    stlInt32         j;
/*    stlChar          sLogMsg[100]; */

    while( STL_TRUE )
    {
        if( sProcCount < 0 )
        {
            break;
        }
        for( i = 0; i < sProcCount; i ++ )
        {
            if( aEnv == gKnEntryPoint->mBreakPointInfo->mProcList[i].mProcEnv )
            {
                break;
            }
        }
        if( i == sProcCount )
        {
            break;
        }

        sCurActionSeq = gKnEntryPoint->mBreakPointInfo->mCurActionSeq;
        if( sCurActionSeq >= gKnEntryPoint->mBreakPointInfo->mActionCount )
        {
            break;
        }
        sCurAction    = &gKnEntryPoint->mBreakPointInfo->mArray[sCurActionSeq];
        sMyWaiter     = KNL_WAITITEM(aEnv);
        sIsTimedOut   = STL_TRUE;

        if( gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcEnv != aEnv )
        {
            sMyProcId = i;
            /*
               stlSnprintf( sLogMsg,
               100,
               ">>> %s Sleep At Start Point",
               gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcName );
               knlLogMsg( NULL, aEnv, KNL_LOG_LEVEL_INFO, "%s", sLogMsg);
               */
            gKnEntryPoint->mBreakPointInfo->mProcList[sMyProcId].mStatus = KNL_PROC_STATUS_WAITING;

            while( STL_TRUE )
            {
                /*
                   stlSnprintf( sLogMsg,
                   100,
                   "[%d, %d]",
                   sCurAction->mProcId, sCurAction->mProcId );
                   knlLogMsg( NULL, aEnv, KNL_LOG_LEVEL_INFO, "%s", sLogMsg);
                   */
                STL_TRY( stlTimedAcquireSemaphore( &sMyWaiter->mSem,
                                                   STL_INFINITE_TIME,
                                                   &sIsTimedOut,
                                                   KNL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );

                if( (sIsTimedOut == STL_FALSE) ||
                    (gKnEntryPoint->mBreakPointInfo->mProcList[sMyProcId].mStatus
                     == KNL_PROC_STATUS_ACTIVE) )
                {
                    break;
                }

            }
            /*
               stlSnprintf( sLogMsg,
               100,
               ">>> %s Awaked At Start Point",
               gKnEntryPoint->mBreakPointInfo->mProcList[sCurAction->mProcId].mProcName );
               knlLogMsg( NULL, aEnv, KNL_LOG_LEVEL_INFO, "%s", sLogMsg);
               */
        }
        else
        {
            if( sCurActionSeq == 0 )
            {
                /* 다른 모든 proc들이 waiting 상태이길 기다린다 */
                while( STL_TRUE )
                {
                    for( j = 0; j < sProcCount; j ++ )
                    {
                        if( (j != i) &&
                            (gKnEntryPoint->mBreakPointInfo->mProcList[j].mStatus != KNL_PROC_STATUS_WAITING) )
                        {
                            break;
                        }
                    }
                    if( j == sProcCount )
                    {
                        break;
                    }

                    stlSleep(100);
                }
                gKnEntryPoint->mBreakPointInfo->mProcList[i].mStatus = KNL_PROC_STATUS_ACTIVE;
            }
        }

        break;
    }
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
/** @} */
