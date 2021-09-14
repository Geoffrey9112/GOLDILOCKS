/*******************************************************************************
 * smgPendOp.c
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
 * @file smgPendOp.c
 * @brief Storage Manager Layer Pending Operation Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smg.h>

/**
 * @addtogroup smg
 * @{
 */

smgPendOpFunc gSmgPendOpTable[SMG_MAX_PEND_COUNT];

void smgRegisterPendOp( smgPendOpId   aPendOpId,
                        smgPendOpFunc aFunc )
{
    gSmgPendOpTable[aPendOpId] = aFunc;
}
                        

/**
 * @brief Pending Operation을 등록한다.
 * @param[in] aStatement Statement 포인터
 * @param[in] aPendOpId Pending Operation Indentifier
 * @param[in] aActionFlag Pending Action Flag
 * @param[in] aArgs Argument List
 * @param[in] aArgsByteSize Argument의 바이트 단위의 사이즈
 * @param[in] aEventByteSize Event의 바이트 단위의 사이즈
 * @param[in,out] aEnv Environment 구조체
 * @see @a aActionFlag : smgPendActionFlags
 * @remarks aEvnetByteSize 는 Commit & Rollback시 Event Memory확보를 실패하는 경우는
 * 방어하기 위해서 Pending Operation추가시 미리 공간을 확보하기 위함이다.
 */
stlStatus smgAddPendOp( smlStatement  * aStatement,
                        smgPendOpId     aPendOpId,
                        stlUInt32       aActionFlag,
                        void          * aArgs,
                        stlUInt32       aArgsByteSize,
                        stlUInt32       aEventByteSize,
                        smlEnv        * aEnv )
{
    smgPendOp * sPendOp;
    void      * sEventMem = NULL;
    stlUInt32   sEventMemSize = aEventByteSize;
    stlInt32    sState = 0;

    /**
     * Pending Operation을 위한 메모리는 Session이나 Transaction Level이 적당하다.
     * 하지만 자주 발생하지 않는 연산이기 때문에 Database Level의 Dynamic 메모리를
     * 사용한다.
     * 또한 Cleanup시에 Pending Operation을 수행하기 위해서는 반드시 Shared Memory를
     * 사용해야 한다.
     */
    STL_TRY( smgAllocShmMem4Mount( STL_ALIGN( STL_SIZEOF(smgPendOp), 8 ) + aArgsByteSize,
                                   (void**)&sPendOp,
                                   aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_RING_INIT_DATALINK( sPendOp, STL_OFFSETOF( smgPendOp, mOpLink ) );

    sPendOp->mPendOpId = aPendOpId;
    sPendOp->mArgs = (void*)( (stlChar*)sPendOp + STL_ALIGN(STL_SIZEOF(smgPendOp), 8) );
    sPendOp->mActionFlag = aActionFlag;

    if( aEventByteSize > 0 )
    {
        STL_TRY( knlAllocEnvEventMem( aEventByteSize,
                                      (void**)&sEventMem,
                                      KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sState = 2;
    }
    
    sPendOp->mEventMem = sEventMem;
    sPendOp->mEventMemSize = sEventMemSize;
    
    if( aStatement == NULL )
    {
        sPendOp->mTimestamp = 0;
    }
    else
    {
        sPendOp->mTimestamp = aStatement->mTimestamp;
    }
    
    stlMemcpy( sPendOp->mArgs, aArgs, aArgsByteSize );

    STL_RING_ADD_LAST( &SML_SESS_ENV(aEnv)->mPendOp, sPendOp );
                           
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) knlFreeEnvEventMem( sEventMem, KNL_ENV(aEnv) );
        case 1:
            (void) smgFreeShmMem4Mount( sPendOp, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Pending Operation을 수행한다.
 * @param[in] aActionFlag Pending Action Flag
 * @param[in] aTimestamp Statement Timestamp
 * @param[in,out] aEnv Environment 구조체
 * @see @a aActionFlag : smgPendActionFlags
 * @remark @a aTimestamp이 0이면 모든 Pending Operation이 수행되며, aStatement가
 *    <BR> 0보다 큰 경우 @a aTimestamp 이후의 Pending Operation만 수행한다.
 */
stlStatus smgExecutePendOp( stlUInt32    aActionFlag,
                            stlInt64     aTimestamp,
                            smlEnv     * aEnv )
{
    smgPendOp * sPendOp;
    smgPendOp * sPrvPendOp;

    if( aActionFlag == SML_PEND_ACTION_ROLLBACK )
    {
        STL_RING_FOREACH_ENTRY_REVERSE_SAFE( &SML_SESS_ENV(aEnv)->mPendOp,
                                             sPendOp,
                                             sPrvPendOp )
        {
            if( sPendOp->mTimestamp < aTimestamp )
            {
                break;
            }
        
            if( aActionFlag & sPendOp->mActionFlag )
            {
                STL_TRY( gSmgPendOpTable[sPendOp->mPendOpId]( aActionFlag,
                                                              sPendOp,
                                                              aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                if( sPendOp->mEventMem != NULL )
                {
                    STL_TRY( knlFreeEnvEventMem( sPendOp->mEventMem,
                                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS);
                }
            }

            STL_RING_UNLINK( &SML_SESS_ENV(aEnv)->mPendOp, sPendOp );
        
            STL_TRY( smgFreeShmMem4Mount( sPendOp,
                                          aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        STL_RING_FOREACH_ENTRY_SAFE( &SML_SESS_ENV(aEnv)->mPendOp,
                                     sPendOp,
                                     sPrvPendOp )
        {
            if( aActionFlag & sPendOp->mActionFlag )
            {
                STL_TRY( gSmgPendOpTable[sPendOp->mPendOpId]( aActionFlag,
                                                              sPendOp,
                                                              aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                if( sPendOp->mEventMem != NULL )
                {
                    STL_TRY( knlFreeEnvEventMem( sPendOp->mEventMem,
                                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS);
                }
            }

            STL_RING_UNLINK( &SML_SESS_ENV(aEnv)->mPendOp, sPendOp );
        
            STL_TRY( smgFreeShmMem4Mount( sPendOp,
                                          aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    /**
     * Pending operation은 Commit 이후에 호출되기 때문에 실패해서는 안된다.
     */
    STL_ASSERT( STL_FALSE );

    return STL_FAILURE;
}

/** @} */
