/*******************************************************************************
 * smklManger.c
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
#include <smxl.h>
#include <sme.h>
#include <smg.h>
#include <smklDef.h>
#include <smklManager.h>
#include <smklWaitGraph.h>

/**
 * @file smklWaitGraph.c
 * @brief Storage Manager Layer Lock Wait Graph Internal Routines
 */

/**
 * @addtogroup smklWaitGraph
 * @{
 */

inline stlStatus smklInitWaitTable( stlInt64   aTransTableSize )
{
    stlMemset( gSmklWarmupEntry->mLockWaitTable,
               0x00,
               STL_SIZEOF( stlChar ) * aTransTableSize * aTransTableSize );

    return STL_SUCCESS;
}

stlStatus smklCheckDeadlock( smxlTransId   aTransId,
                             stlBool     * aIsDeadlock,
                             smlEnv      * aEnv )
{
    stlUInt16        sMyLockSlotId;
    smklDeadlockLink sMyLink;
    stlInt32         sResult;

    sMyLockSlotId = smklGetLockSlotId( aTransId );
    
    sMyLink.mPrev = NULL;
    sMyLink.mSlotId = sMyLockSlotId;
    
    sResult = smklCheckDeadlockInternal( sMyLockSlotId,
                                         sMyLockSlotId,
                                         &sMyLink );


    if( sResult == SMKL_DEADLOCK_TRUE )
    {
        *aIsDeadlock = STL_TRUE;
    }
    else
    {
        *aIsDeadlock = STL_FALSE;
    }
    
    return STL_SUCCESS;
}

stlInt32 smklCheckDeadlockInternal( stlUInt16          aMyLockSlotId,
                                    stlUInt16          aTargetLockSlotId,
                                    smklDeadlockLink * aDeadlockLink )
{
    stlInt32           i;
    smklDeadlockLink * sLink;
    smklDeadlockLink   sMyLink;
    stlInt32           sResult = SMKL_DEADLOCK_FALSE;
    
    for( i = 0; i < SMKL_LOCK_WAIT_TABLE_SIZE; i++ )
    {
        if( SMKL_HAS_EDGE( aTargetLockSlotId, i ) == STL_TRUE )
        {
            if( i == aMyLockSlotId )
            {
                sResult = SMKL_DEADLOCK_TRUE;
                break;
            }

            /**
             * 자신을 제외한 부분에 Cycle이 형성되는지 검사한다.
             * - 만약 Cycle이 있다면 Deadlock이 아닌것으로 해석한다.
             */
            
            sLink = aDeadlockLink;
            
            while( sLink != NULL )
            {
                if( sLink->mSlotId == i )
                {
                    sResult = SMKL_DEADLOCK_EXIT;
                    STL_THROW( RAMP_SUCCESS );
                }
                sLink = sLink->mPrev;
            }

            sMyLink.mSlotId = i;
            sMyLink.mPrev = aDeadlockLink;
            
            if( i >= SMKL_LOCK_TABLE_SIZE )
            {
                sResult = smklCheckRequestGroupDeadlockInternal( aMyLockSlotId,
                                                                 i,
                                                                 &sMyLink );
            }
            else
            {
                sResult = smklCheckDeadlockInternal( aMyLockSlotId,
                                                     i,
                                                     &sMyLink );
            }

            if( (sResult == SMKL_DEADLOCK_TRUE) ||
                (sResult == SMKL_DEADLOCK_EXIT) )
            {
                break;
            }
        }
    }

    STL_RAMP( RAMP_SUCCESS );

    return sResult;
}

/**
 * @brief 주어진 트랜잭션에 대기하는 모든 트랜잭션의 edge를 삭제한다.
 * @param[in] aTransId Transaction 아이디
 */
inline void smklResetWaitSlot( smxlTransId aTransId )
{
    stlMemset( &SMKL_LOCK_WAIT_TABLE( smklGetLockSlotId(aTransId) *
                                      SMKL_LOCK_WAIT_TABLE_SIZE ),
               0x00,
               STL_SIZEOF( stlChar ) * SMKL_LOCK_WAIT_TABLE_SIZE );
}

inline void smklResolveDeadlock( smxlTransId aTransId )
{
    stlInt32   i;
    stlUInt16  sLockSlotId;
    
    sLockSlotId = smklGetLockSlotId( aTransId );
    
    for( i = 0; i < SMKL_LOCK_WAIT_TABLE_SIZE; i++ )
    {
        if( SMKL_HAS_EDGE( sLockSlotId, i ) == STL_TRUE )
        {
            SMKL_SET_DEADLOCK_EDGE( sLockSlotId, i );
        }
    }
}

stlInt32 smklCheckRequestGroupDeadlockInternal( stlUInt16          aMyLockSlotId,
                                                stlUInt16          aTargetLockSlotId,
                                                smklDeadlockLink * aDeadlockLink )
{
    smklDeadlockLink   sMyLink;
    smklDeadlockLink * sLink;
    stlInt32           sResult = SMKL_DEADLOCK_FALSE;
    stlInt32           sDeadlockWorkerCount = 0;
    stlInt32           sTotalWorkerCount = 0;
    stlInt32           i;

    sTotalWorkerCount = SMKL_GROUP_WORKER_COUNT( SMKL_LOCK_WAIT_TABLE_SIZE - (aTargetLockSlotId+1) );

    STL_TRY_THROW( sTotalWorkerCount != 0, RAMP_SKIP );
    
    for( i = 0; i < SMKL_LOCK_WAIT_TABLE_SIZE; i++ )
    {
        if( SMKL_HAS_EDGE( aTargetLockSlotId, i ) == STL_TRUE )
        {
            if( i == aMyLockSlotId )
            {
                sDeadlockWorkerCount++;
                continue;
            }

            /**
             * 자신을 제외한 부분에 Cycle이 형성되는지 검사한다.
             * - 만약 Cycle이 있다면 Deadlock이 아닌것으로 해석한다.
             */
            
            sLink = aDeadlockLink;
            
            while( sLink != NULL )
            {
                if( sLink->mSlotId == i )
                {
                    break;
                }
                sLink = sLink->mPrev;
            }

            sMyLink.mSlotId = i;
            sMyLink.mPrev = aDeadlockLink;
            
            sResult = smklCheckDeadlockInternal( aMyLockSlotId,
                                                 i,
                                                 &sMyLink );

            /**
             * Holder가 Request Group에 대기하고 있을수 있기 때문에
             * DEADLOCK_EXIT(partial cycle)는 Deadlock이라고 판단한다.
             */
            if( sResult == SMKL_DEADLOCK_FALSE )
            {
                break;
            }
            
            sDeadlockWorkerCount++;
        }
    }

    if( sTotalWorkerCount == sDeadlockWorkerCount )
    {
        sResult = SMKL_DEADLOCK_TRUE;
    }

    STL_RAMP( RAMP_SKIP );

    return sResult;
}

/** @} */
