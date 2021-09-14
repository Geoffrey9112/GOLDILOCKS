/*******************************************************************************
 * smlIterator.c
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

#include <dtl.h>
#include <sml.h>
#include <smDef.h>
#include <sme.h>
#include <smxl.h>

/**
 * @file smlIterator.c
 * @brief Storage Manager Layer Iterator Routines
 */

/**
 * @addtogroup smlIterator
 * @{
 */

/**
 * @brief Iterator 진입을 선언한다.
 * @param[in] aEnv Environment Pointer
 */
stlStatus smlEnterIterator( smlEnv * aEnv )
{
    stlInt32  sState = 0;
    stlInt32  sSemValue;

    sState = 0;
    STL_TRY_THROW( aEnv->mMiniTransStackTop == -1, RAMP_ERR_INTERNAL );
    
    sState = 1;
    STL_TRY_THROW( SML_SESS_ENV(aEnv)->mFixCount == 0, RAMP_ERR_INTERNAL );

    if( KNL_IS_SHARED_ENV( aEnv ) == STL_TRUE )
    {
        STL_TRY( stlGetSemaphoreValue( &(KNL_ENV(aEnv)->mWaitItem.mSem),
                                       &sSemValue,
                                       KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        
        sState = 2;
        STL_TRY_THROW( sSemValue == 0, RAMP_ERR_INTERNAL );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      "smlEnterIterator()",
                      sState );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Iterator 떠남을 선언한다.
 * @param[in] aEnv Environment Pointer
 */
stlStatus smlLeaveIterator( smlEnv * aEnv )
{
    stlInt32  sState = 0;
    stlInt32  sSemValue;

    STL_TRY_THROW( aEnv->mMiniTransStackTop == -1, RAMP_ERR_INTERNAL );
    sState = 0;
    
    sState = 1;
    STL_TRY_THROW( SML_SESS_ENV(aEnv)->mFixCount == 0, RAMP_ERR_INTERNAL );
    
    sState = 2;
    STL_TRY_THROW( KNL_SESS_ENV(aEnv)->mOpenFileCount == 0, RAMP_ERR_INTERNAL );
    
    if( knlGetCurrStartupPhase() < KNL_STARTUP_PHASE_OPEN )
    {
        sState = 3;
        STL_TRY_THROW( STL_RING_IS_EMPTY( &SML_SESS_ENV(aEnv)->mPendOp ) == STL_TRUE, RAMP_ERR_INTERNAL );
    }

    if( KNL_IS_SHARED_ENV( aEnv ) == STL_TRUE )
    {
        STL_TRY( stlGetSemaphoreValue( &(KNL_ENV(aEnv)->mWaitItem.mSem),
                                       &sSemValue,
                                       KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        sState = 4;
        STL_TRY_THROW( sSemValue == 0, RAMP_ERR_INTERNAL );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      "smlLeaveIterator()",
                      sState );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 Iterator를 다시 초기화한다
 * @param[in,out] aIterator    초기화할 iterator
 * @param[in]     aEnv         Storage Manager Environment
 */
stlStatus smlResetIterator( void    * aIterator,
                            smlEnv  * aEnv )
{
    smiIterator * sIterator = (smiIterator*)aIterator;

    STL_DASSERT( smlEnterIterator( aEnv ) == STL_SUCCESS );
    
    STL_TRY( sIterator->mResetIterator( aIterator,
                                        aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 대상 아이템셋중 다음 아이템 정보를 얻는다.
 * @param[in]       aIterator    Iterator 구조체
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in,out]   aEnv         Environment 구조체
 */
stlStatus smlFetchNext( void          * aIterator,
                        smlFetchInfo  * aFetchInfo,
                        smlEnv        * aEnv )
{
    smiIterator * sIterator = (smiIterator*)aIterator;

    STL_DASSERT( smlEnterIterator( aEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( sIterator->mState != SMI_STATE_CLOSED,
                   RAMP_ERR_FETCH_SEQUENCE );

    /**
     * Event 검사
     */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Fetch 전 Block List 의 정보를 초기화한다.
     */

    if ( aFetchInfo->mBlockJoinFetchInfo != NULL )
    {
        /**
         * Blocked Join 인 경우, Result 를 다 채울때까지 초기화하지 않는다.
         */
    }
    else
    {
        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aFetchInfo->mColList, 0, 0 );
        if( aFetchInfo->mRowIdColList != NULL )
        {
            KNL_SET_ONE_BLOCK_SKIP_CNT( aFetchInfo->mRowIdColList, 0 );
            KNL_SET_ONE_BLOCK_USED_CNT( aFetchInfo->mRowIdColList, 0 );
        }
        
        if ( aFetchInfo->mFetchRecordInfo != NULL )
        {
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aFetchInfo->mFetchRecordInfo->mColList, 0, 0 );
            if( aFetchInfo->mFetchRecordInfo->mRowIdColList != NULL )
            {
                KNL_SET_ONE_BLOCK_SKIP_CNT( aFetchInfo->mFetchRecordInfo->mRowIdColList, 0 );
                KNL_SET_ONE_BLOCK_USED_CNT( aFetchInfo->mFetchRecordInfo->mRowIdColList, 0 );
            }
        }
    }

    if( sIterator->mTransReadMode == SML_TRM_COMMITTED )
    {
        /**
         * 최신 버전을 볼수 있어야 한다.
         */
        sIterator->mViewScn = smxlGetSystemScn();
    }
    
    STL_TRY( sIterator->mFetchNext( aIterator,
                                    aFetchInfo,
                                    aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FETCH_SEQUENCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FETCH_SEQUENCE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 대상 아이템셋중 이전 아이템 정보를 얻는다.
 * @param[in]       aIterator    Iterator 구조체
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in,out]   aEnv         Environment 구조체
 */
stlStatus smlFetchPrev( void          * aIterator,
                        smlFetchInfo  * aFetchInfo,
                        smlEnv        * aEnv )
{
    smiIterator * sIterator = (smiIterator*)aIterator;

    STL_DASSERT( smlEnterIterator( aEnv ) == STL_SUCCESS );
    
    STL_PARAM_VALIDATE( aFetchInfo->mRowBlock != NULL, KNL_ERROR_STACK(aEnv) );
    STL_TRY_THROW( sIterator->mState != SMI_STATE_CLOSED,
                   RAMP_ERR_FETCH_SEQUENCE );
    
    /**
     * Event 검사
     */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Fetch 전 Block List 의 정보를 초기화한다.
     */
    
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aFetchInfo->mColList, 0, 0 );
    if( aFetchInfo->mRowIdColList != NULL )
    {
        KNL_SET_ONE_BLOCK_SKIP_CNT( aFetchInfo->mRowIdColList, 0 );
        KNL_SET_ONE_BLOCK_USED_CNT( aFetchInfo->mRowIdColList, 0 );
    }

    if ( aFetchInfo->mFetchRecordInfo != NULL )
    {
        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aFetchInfo->mFetchRecordInfo->mColList, 0, 0 );
        if( aFetchInfo->mFetchRecordInfo->mRowIdColList != NULL )
        {
            KNL_SET_ONE_BLOCK_SKIP_CNT( aFetchInfo->mFetchRecordInfo->mRowIdColList, 0 );
            KNL_SET_ONE_BLOCK_USED_CNT( aFetchInfo->mFetchRecordInfo->mRowIdColList, 0 );
        }
    }
    
    if( sIterator->mTransReadMode == SML_TRM_COMMITTED )
    {
        /**
         * 최신 버전을 볼수 있어야 한다.
         */
        sIterator->mViewScn = smxlGetSystemScn();
    }
    
    STL_TRY( sIterator->mFetchPrev( aIterator,
                                    aFetchInfo,
                                    aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FETCH_SEQUENCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FETCH_SEQUENCE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}


/**
 * @brief Iterator의 위치를 주어진 위치 바로 전으로 이동시킨다
 * @param[in] aIterator Iterator 구조체
 * @param[in] aRowRid 이전에 저장한 Row RID
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlMoveToRowId( void   * aIterator,
                          smlRid * aRowRid,
                          smlEnv * aEnv )
{
    smiIterator * sIterator = (smiIterator*)aIterator;

    STL_DASSERT( smlEnterIterator( aEnv ) == STL_SUCCESS );
    
    STL_TRY( sIterator->mMoveToRowId( aIterator, aRowRid, aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Iterator의 위치를 주어진 position number 이전 row로 이동시킨다
 * @param[in] aIterator Iterator 구조체
 * @param[in] aPosNum 이동시킬 position number. 0-base이다. 즉 0을 주면 BeforeFirst와 같은 효과이다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlMoveToPosNum( void   * aIterator,
                          stlInt64 aPosNum,
                          smlEnv * aEnv )
{
    smiIterator * sIterator = (smiIterator*)aIterator;

    STL_DASSERT( smlEnterIterator( aEnv ) == STL_SUCCESS );

    STL_TRY( sIterator->mMoveToPosNum( aIterator, aPosNum, aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}

/**
 * @brief Iterator에서 다음 key group으로 이동한다.
 * @param[in] aIterator Iterator 구조체
 * @param[in,out] aFetchInfo   Fetch 정보
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlMoveToNextKey( void          * aIterator,
                            smlFetchInfo  * aFetchInfo,
                            smlEnv        * aEnv )
{
    smiIterator * sIterator = (smiIterator*)aIterator;

    STL_DASSERT( smlEnterIterator( aEnv ) == STL_SUCCESS );

    STL_TRY( sIterator->mMoveToNextKey( aIterator, aFetchInfo, aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}

/**
 * @brief Iterator에서 현재 group의 처음으로 다시 이동한다.
 * @param[in] aIterator Iterator 구조체
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlResetGroup( void    * aIterator,
                         smlEnv  * aEnv )
{
    smiIterator * sIterator = (smiIterator*)aIterator;

    STL_DASSERT( smlEnterIterator( aEnv ) == STL_SUCCESS );

    STL_TRY( sIterator->mResetGroup( aIterator, aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}

/** @} */
