/*******************************************************************************
 * smrPendBuffer.c
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
#include <smlGeneral.h>
#include <smDef.h>
#include <smrDef.h>
#include <smp.h>
#include <smr.h>
#include <smxl.h>
#include <smxm.h>
#include <smrLogBuffer.h>
#include <smrPendBuffer.h>

/**
 * @file smrPendBuffer.c
 * @brief Storage Manager Layer Recovery Pending Log Buffer Internal Routines
 */

extern smrWarmupEntry * gSmrWarmupEntry;

/**
 * @addtogroup smr
 * @{
 */

stlStatus smrInitPendLogBuffer( smrPendLogBuffer * aPendLogBuffer,
                                smlEnv           * aEnv )
{
    STL_PARAM_VALIDATE( aPendLogBuffer != NULL, KNL_ERROR_STACK(aEnv) );
    
    /**
     * ALIGN을 고려해 BLOCK SIZE 만큼의 추가 메모리를 할당한다.
     */
    STL_TRY( knlAllocFixedStaticArea( SML_PENDING_LOG_BUFFER_SIZE + SMR_PENDING_BLOCK_SIZE,
                                      (void**)&aPendLogBuffer->mBuffer,
                                      (knlEnv*)aEnv )
             == STL_SUCCESS );

    aPendLogBuffer->mBufferSize = STL_ALIGN( SML_PENDING_LOG_BUFFER_SIZE,
                                             SMR_PENDING_BLOCK_SIZE );
    aPendLogBuffer->mBufferBlockCount = ( aPendLogBuffer->mBufferSize /
                                          SMR_PENDING_BLOCK_SIZE );
    aPendLogBuffer->mFrontPbsn = 0;
    aPendLogBuffer->mRearPbsn = 0;
    aPendLogBuffer->mFrontLsn = SMR_INVALID_LSN;
    aPendLogBuffer->mRearLsn = SMR_INVALID_LSN;

    STL_TRY( knlInitLatch( &aPendLogBuffer->mLatch,
                           STL_TRUE,   /* aIsInShm */
                           KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrFormatPendLogBuffer( smrPendLogBuffer * aPendLogBuffer,
                                  smlEnv           * aEnv )
{
    STL_PARAM_VALIDATE( aPendLogBuffer != NULL, KNL_ERROR_STACK(aEnv) );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrWritePendingLog( smrLogStream   * aLogStream,
                              smrLogHdr      * aLogHdr,
                              smrLogDataType   aLogDataType,
                              void           * aLogData,
                              stlBool        * aIsSuccess,
                              smrLsn         * aWrittenLsn,
                              smlEnv         * aEnv )
{
    stlInt32             i;
    stlInt32             sState = 0;
    stlBool              sIsSuccess;
    smrPendLogBuffer   * sPendLogBuffer;
    smrPendLogBuffer   * sTargetBuffer= NULL;
    stlInt32             sNeedBlockCount;
    stlChar            * sBlock;
    stlInt32             sRearPos;
    smxmLogBlockCursor   sLogBlockCursor;

    STL_PARAM_VALIDATE( aLogStream != NULL, KNL_ERROR_STACK(aEnv) );

    *aIsSuccess = STL_TRUE;

    sPendLogBuffer = aLogStream->mPendLogBuffer;
    sNeedBlockCount = smrNeedPendBlockCount( aLogHdr->mSize );

    /**
     * Latch를 획득할수 있는 Pending Buffer를 찾는다.
     */
    for( i = 0; i < aLogStream->mPendBufferCount; i++ )
    {
        sPendLogBuffer = &aLogStream->mPendLogBuffer[i];

        STL_TRY( knlTryLatch( &sPendLogBuffer->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              &sIsSuccess,
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        if( sIsSuccess == STL_FALSE )
        {
            continue;
        }
            
        sState = 1;
        sTargetBuffer = sPendLogBuffer;

        sRearPos = SMR_PEND_REAR_POS( sPendLogBuffer );

        /**
         * 기록될 로그가 중간에 rounding 되지 않도록 설계한다.
         * - 불필요한 Memcpy 제거를 위해서
         */
        if( sRearPos + sNeedBlockCount > sPendLogBuffer->mBufferBlockCount )
        {
            /**
             * Delimeter를 설정한다.
             */
            sBlock = SMR_PEND_BUFFER_ADDR( sPendLogBuffer, sRearPos );
            *((smrLsn*)sBlock) = SMR_INVALID_LSN;

            /**
             * mRearPbsn은 No Latch로 읽기 때문에 Reader를 위해서 Barrier를 쳐야 한다.
             */
            stlMemBarrier();
            
            /**
             * Rear PBSN을 Buffer의 Top으로 증가시킨다.
             */
            sPendLogBuffer->mRearPbsn += (sPendLogBuffer->mBufferBlockCount -
                                          sRearPos);
        }
                
        /**
         * 기록될 로그의 사이즈가 버퍼 공간을 초과한다면 다음 Pending Buffer를 사용한다.
         */
        if( (sPendLogBuffer->mRearPbsn + sNeedBlockCount -
             sPendLogBuffer->mFrontPbsn) <
            (sPendLogBuffer->mBufferBlockCount - 1) )
        {
            break;
        }

        sTargetBuffer = NULL;

        sState = 0;
        STL_TRY( knlReleaseLatch( &sPendLogBuffer->mLatch,
                                  KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    if( sTargetBuffer == NULL )
    {
        *aIsSuccess = STL_FALSE;

        STL_THROW( RAMP_SUCCESS );
    }

    /**
     * Pending Buffer 로그를 기록한다.
     * - 기록 순서 : [Log Header] [Log Body]
     */

    aLogHdr->mLsn = stlAtomicInc64( (stlUInt64*)&(gSmrWarmupEntry->mLsn) );

    SMXM_INIT_LOG_BLOCK_CURSOR( &sLogBlockCursor );
    
    sBlock = SMR_PEND_REAR_BUFFER_ADDR( sPendLogBuffer );
    
    STL_DASSERT( (void*)sBlock >= sPendLogBuffer->mBuffer );
    
    stlMemcpy( sBlock, aLogHdr, STL_SIZEOF(smrLogHdr) );

    if( aLogDataType == SMR_LOGDATA_TYPE_BLOCKARRAY )
    {
        STL_TRY( smxmCopyLogFromBlock( &sLogBlockCursor,
                                       aLogData,
                                       sBlock + STL_SIZEOF(smrLogHdr),
                                       aLogHdr->mSize,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_ASSERT( aLogDataType == SMR_LOGDATA_TYPE_BINARY );
        stlMemcpy( sBlock + STL_SIZEOF(smrLogHdr),
                   aLogData,
                   aLogHdr->mSize );
    }

    if( aWrittenLsn != NULL )
    {
        *aWrittenLsn = aLogHdr->mLsn;
    }

    /**
     * mRearPbsn은 No Latch로 읽기 때문에 Reader를 위해서 Barrier를 쳐야 한다.
     * - mRearPbsn이 block의 lsn보다 먼저 수정되어서는 안된다.
     */
    stlMemBarrier();
    
    /**
     * PSBN을 사용한 Block 개수 만큼 증가 시킨다.
     */
    sPendLogBuffer->mRearPbsn += sNeedBlockCount;
    sPendLogBuffer->mRearLsn = aLogHdr->mLsn;
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &sTargetBuffer->mLatch,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        /**
         * 메모리 rollback을 할수 없기 때문에 System Fatal로 처리한다.
         */
        STL_ASSERT( 0 );
        knlReleaseLatch( &sTargetBuffer->mLatch, KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

stlInt32 smrNeedPendBlockCount( stlUInt32 aLogBodySize )
{
    stlInt32 sTotalSize;
    stlInt32 sNeedBlockCount;
    
    sTotalSize = STL_SIZEOF(smrLogHdr) + aLogBodySize + SMR_PENDING_BLOCK_SIZE;
    sNeedBlockCount = (sTotalSize) / SMR_PENDING_BLOCK_SIZE;
    
    return sNeedBlockCount;
}
                                   
stlStatus smrReadPendingLog( smrLogStream      * aLogStream,
                             smrPendLogCursor  * aPendLogCursor,
                             smrLogHdr        ** aLogHdr,
                             void             ** aLogData,
                             smlEnv            * aEnv )
{
    smrPendLogBuffer * sPendLogBuffer;
    stlInt32           i;
    smrLsn             sMinLsn = STL_INT64_MAX;
    smrPendLogBuffer * sTargetPendBuffer = NULL;
    smrLsn             sLsn;
    stlChar          * sBlock;
    stlInt32           sBlockCount;
    
    STL_PARAM_VALIDATE( aLogStream != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPendLogCursor != NULL, KNL_ERROR_STACK(aEnv) );

    for( i = 0; i < aLogStream->mPendBufferCount; i++ )
    {
        sPendLogBuffer = &aLogStream->mPendLogBuffer[i];

        /**
         * 이미 닫혀진 Log Buffer는 skip
         */
        if( aPendLogCursor->mState[i] == SMR_PEND_CURSOR_CLOSED )
        {
            continue;
        }

        /**
         * 기록된 로그가 없다면 skip
         */
        if( aPendLogCursor->mEndPbsn[i] == sPendLogBuffer->mFrontPbsn )
        {
            aPendLogCursor->mState[i] = SMR_PEND_CURSOR_CLOSED;
            continue;
        }
        
        sLsn = SMR_PEND_FRONT_LSN( sPendLogBuffer );

        /**
         * 다음 로그가 rounding이 되었는지 확인한다.
         */
        if( sLsn == SMR_INVALID_LSN )
        {
            /**
             * Front PBSN을 Buffer의 Top으로 증가시킨다.
             */
            sPendLogBuffer->mFrontPbsn += (sPendLogBuffer->mBufferBlockCount -
                                           SMR_PEND_FRONT_POS(sPendLogBuffer));
            
            if( aPendLogCursor->mEndPbsn[i] == sPendLogBuffer->mFrontPbsn )
            {
                aPendLogCursor->mState[i] = SMR_PEND_CURSOR_CLOSED;
                continue;
            }
        
            sLsn = SMR_PEND_FRONT_LSN( sPendLogBuffer );
        }

        /**
         * 기록된 로그가 원하는 로그보다 나중에 기록된 경우 read할 필요가 없다.
         */
        if( sLsn > aPendLogCursor->mEndLsn )
        {
            aPendLogCursor->mState[i] = SMR_PEND_CURSOR_CLOSED;
            continue;
        }

        /**
         * Target Pending Log Buffer 결정
         */
        if( sLsn < sMinLsn )
        {
            sMinLsn = sLsn;
            sTargetPendBuffer = sPendLogBuffer;
        }
    }

    /**
     * 원하는 로그가 없는 경우는 Cursor의 종료를 의미한다.
     */
    if( sTargetPendBuffer == NULL )
    {
        *aLogData = NULL;
        STL_THROW( RAMP_SUCCESS );
    }

    /**
     * 로그를 읽는다.
     */
    sBlock = SMR_PEND_FRONT_BUFFER_ADDR( sTargetPendBuffer );

    *aLogHdr = (smrLogHdr*)sBlock;
    *aLogData = sBlock + STL_SIZEOF( smrLogHdr );

    sBlockCount = smrNeedPendBlockCount( (*aLogHdr)->mSize );

    KNL_ASSERT( sBlockCount < sTargetPendBuffer->mBufferBlockCount,
                KNL_ENV(aEnv),
                ("NEED_BLOCK_COUNT(%d), PEND_BUFFER_BLOCK_COUNT(%d)",
                 sBlockCount,
                 sTargetPendBuffer->mBufferBlockCount) );

    KNL_ASSERT( (*aLogHdr)->mLsn <= aPendLogCursor->mEndLsn,
                KNL_ENV(aEnv),
                ("LSN(%ld), END_LSN(%ld)",
                 (*aLogHdr)->mLsn,
                 aPendLogCursor->mEndLsn) );

    /**
     * Global Log Buffer에 반영한 이후에 Front SBSN을 전진시킨다.
     */
    aPendLogCursor->mTargetPendBuffer = sTargetPendBuffer;
    aPendLogCursor->mReadBlockCount = sBlockCount;

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrPreparePendingLogCursor( smrLogStream     * aLogStream,
                                      smrLsn             aEndLsn,
                                      smrPendLogCursor * aPendLogCursor,
                                      smlEnv           * aEnv )
{
    smrPendLogBuffer * sPendLogBuffer;
    stlInt32           i;
    smrLsn             sLsn;
    
    STL_PARAM_VALIDATE( aLogStream != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPendLogCursor != NULL, KNL_ERROR_STACK(aEnv) );

    aPendLogCursor->mEndLsn = aEndLsn;
    aPendLogCursor->mTargetPendBuffer = NULL;
    aPendLogCursor->mReadBlockCount = 0;

    for( i = 0; i < aLogStream->mPendBufferCount; i++ )
    {
        sPendLogBuffer = &aLogStream->mPendLogBuffer[i];
        aPendLogCursor->mState[i] = SMR_PEND_CURSOR_CLOSED;
        aPendLogCursor->mEndPbsn[i] = sPendLogBuffer->mRearPbsn;

        /**
         * 기록된 로그가 없다면 skip
         */
        if( aPendLogCursor->mEndPbsn[i] == sPendLogBuffer->mFrontPbsn )
        {
            continue;
        }
        
        sLsn = SMR_PEND_FRONT_LSN( sPendLogBuffer );

        /**
         * 기록된 로그가 원하는 로그보다 최신일 경우 read할 필요가 없다.
         */
        if( sLsn > aEndLsn )
        {
            continue;
        }

        aPendLogCursor->mState[i] = SMR_PEND_CURSOR_ACTIVE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrMovePendingLogCursor( smrPendLogCursor * aPendLogCursor,
                                   smlEnv           * aEnv )
{
    /**
     * Front SBSN을 전진 시킨다.
     * - Log의 rounding 처리는 지금하지 않고 다음 Read 시점에 판단한다.
     */
    aPendLogCursor->mTargetPendBuffer->mFrontPbsn +=
        aPendLogCursor->mReadBlockCount;
    
    return STL_SUCCESS;
}

stlStatus smrWaitPendingLog( smrLogStream * aLogStream,
                             smrLsn         aEndLsn,
                             smlEnv       * aEnv )
{
    stlInt32             i;
    stlInt32             sState = 0;
    stlBool              sIsTimedOut;
    smrPendLogBuffer   * sPendLogBuffer;
    
    for( i = 0; i < aLogStream->mPendBufferCount; i++ )
    {
        sPendLogBuffer = &aLogStream->mPendLogBuffer[i];

        /**
         * Pending Log Buffer에 Latch가 획득된 상태라면
         * 누군가가 로그를 기록하고 있음을 의미하며,
         * 이러한 경우는 기록하고 있는 로그가 기록될때까지
         * 기다려야 한다.
         */
        if( knlGetExclLockSeq( &sPendLogBuffer->mLatch ) &
            STL_INT64_C(0x0000000000000001) )
        {
            STL_TRY( knlAcquireLatch( &sPendLogBuffer->mLatch,
                                      KNL_LATCH_MODE_EXCLUSIVE,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      STL_INFINITE_TIME,
                                      &sIsTimedOut,
                                      KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
            sState = 1;
        
            sState = 0;
            STL_TRY( knlReleaseLatch( &sPendLogBuffer->mLatch,
                                      KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sPendLogBuffer->mLatch, KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}

stlStatus smrApplyPendingLogs( smrLogStream * aLogStream,
                               smrLsn         aEndLsn,
                               smrSbsn      * aSbsn,
                               stlBool      * aRetry,
                               stlBool        aWaitFlusher,
                               smlEnv       * aEnv )
{
    smrLogHdr        * sLogHdr;
    void             * sLogData = NULL;
    smrPendLogCursor   sPendLogCursor;

    STL_PARAM_VALIDATE( aLogStream != NULL, KNL_ERROR_STACK(aEnv) );

    *aRetry = STL_FALSE;

    /**
     * Pending Buffer가 없다면 무시한다.
     */
    STL_TRY_THROW( aLogStream->mPendBufferCount > 0, RAMP_SUCCESS );

    /**
     * Pending Log Cursor를 준비한다.
     */
    STL_TRY( smrPreparePendingLogCursor( aLogStream,
                                         aEndLsn,
                                         &sPendLogCursor,
                                         aEnv )
             == STL_SUCCESS );
    
    /**
     * 모든 Pending Log들을 Log Buffer에 반영한다.
     */
    while( 1 )
    {
        STL_TRY( smrReadPendingLog( aLogStream,
                                    &sPendLogCursor,
                                    &sLogHdr,
                                    &sLogData,
                                    aEnv )
                 == STL_SUCCESS );

        if( sLogData == NULL )
        {
            break;
        }

        STL_TRY( smrWriteStreamLogInternal( aLogStream,
                                            sLogHdr,
                                            SMR_LOGDATA_TYPE_BINARY,
                                            sLogData,
                                            STL_TRUE,  /* aPendingLog */
                                            aWaitFlusher,
                                            NULL,      /* aWrittenLid */
                                            aSbsn,
                                            NULL,      /* aWrittenLsn */
                                            aRetry,
                                            aEnv )
                 == STL_SUCCESS );

        /**
         * 로그 기록중 Buffer Latch를 풀었다면 다시 시도해야 한다.
         */
        STL_TRY_THROW( *aRetry == STL_FALSE, RAMP_SUCCESS );

        STL_TRY( smrMovePendingLogCursor( &sPendLogCursor,
                                          aEnv )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
