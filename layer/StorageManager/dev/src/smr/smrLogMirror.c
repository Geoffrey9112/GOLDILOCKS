/*******************************************************************************
 * smrLogMirror.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $ id: $
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
#include <smlSuppLogDef.h>
#include <smlGeneral.h>
#include <smDef.h>
#include <smrDef.h>
#include <smrLogMirrorDef.h>
#include <smrLogMirror.h>
/**
 * @file smrLogMirror.c
 * @brief Storage Manager Layer LogMirror Internal Routines
 */

extern smrWarmupEntry * gSmrWarmupEntry;

stlUInt16 gSmrLogMirrorShmAddrIdx = SMR_LOGMIRROR_INVALID_IDX;

/**
 * @addtogroup smrLogMirror
 * @{
 */


stlStatus smrWriteLogMirrorBuffer( void      * aBuffer,
                                   stlInt64    aFlushBytes,
                                   stlBool     aIsFileHdr,
                                   smlEnv    * aEnv )
{
    void             * sBaseAddr      = NULL;
    smrLogMirrorInfo * sLogMirrorInfo = NULL;
    stlChar          * sLogMirrorData = NULL;
    smrLogMirrorHdr  * sLogMirrorHdr  = NULL;
    stlInt64           sSendSize      = 0;
    stlInt32           sLoopCnt       = 0;
    stlInt32           sIdx           = 0;
    stlBool            sIsTimeout     = STL_FALSE;
    stlInt64           sRetryTimeout  = 0;
    stlInt64           sRetryTime     = 0;
    
    STL_TRY_THROW( gSmrLogMirrorShmAddrIdx != SMR_LOGMIRROR_INVALID_IDX, RAMP_SKIP );

    STL_DASSERT( aFlushBytes > 0 );
    
    STL_TRY( knlGetShmSegmentAddrUnsafe( gSmrLogMirrorShmAddrIdx,
                                         &sBaseAddr,
                                         KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    sLogMirrorInfo = sBaseAddr; 
    sLogMirrorHdr  = sBaseAddr + SMR_LOGMIRROR_BUFF_OFFSET;
    sLogMirrorData = sBaseAddr + SMR_LOGMIRROR_BUFF_OFFSET + STL_SIZEOF( smrLogMirrorHdr );
    
    if( sLogMirrorInfo->mConnected == SMR_LOGMIRROR_CONNECTED )
    {
        /**
         * LogMirror가 시작하는 시점은 Redo Log가 Switch되는 시점이다.
         * 후에 정책이 결정되면 변경해야 하는 Code 임..
         *
         * LogMirror가 Restart 할 경우에 Recovery가 끝난 이후에 SMR_LOGMIRROR_STARTED로 변경하기도 함
         */
        if( aIsFileHdr == STL_TRUE )
        {
            sLogMirrorInfo->mIsStarted = SMR_LOGMIRROR_STARTED;
        }
        
        if( sLogMirrorInfo->mIsStarted == SMR_LOGMIRROR_STARTED )
        {
            sLoopCnt = ( aFlushBytes / sLogMirrorInfo->mSize ) + 1;
        
            for( sIdx = 0; sIdx < sLoopCnt; sIdx++ )
            {
                sSendSize = STL_MIN( sLogMirrorInfo->mSize,
                                     aFlushBytes - ( sLogMirrorInfo->mSize * sIdx ) );
                
                stlAcquireSpinLock( &sLogMirrorInfo->mSpinLock,
                                    NULL /* Miss count */ );
            
                sLogMirrorInfo->mWriteSeq++;
                
                sLogMirrorHdr->mWriteSeq     = sLogMirrorInfo->mWriteSeq;
                sLogMirrorHdr->mSize         = sSendSize;
                sLogMirrorHdr->mCurrentPiece = sIdx + 1;
                sLogMirrorHdr->mTotalPiece   = sLoopCnt;
                sLogMirrorHdr->mIsLogFileHdr = aIsFileHdr;
            
                stlMemcpy( sLogMirrorData,
                           aBuffer + ( sLogMirrorInfo->mSize * sIdx ), 
                           sSendSize );
            
                stlReleaseSpinLock( &sLogMirrorInfo->mSpinLock );
                
                /**
                 * LogMirrorStat을 Wait 상태로 변경
                 */
                gSmrWarmupEntry->mRecoveryInfo.mLogMirrorStat = 1;
                
                /**
                 * LogMirror Slave에 보내도록 한다.
                 */
                STL_TRY( stlReleaseSemaphore( &(sLogMirrorInfo->mWaitDataSem),
                                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
                
                /**
                 * 나눠서 보낸 데이터가 정상적으로 처리되었는지 대기한다.
                 */
                if( sIdx + 1 != sLoopCnt )
                {
                    while( 1 )
                    {
                        /**
                         * 대기 시간이 길면 Fail로 처리하고 DB가 정상적으로 운영되도록 해야함
                         */
                        STL_TRY( stlTimedAcquireSemaphore( &(sLogMirrorInfo->mWaitSendSem),
                                                           STL_SET_SEC_TIME( 1 ),
                                                           &sIsTimeout,
                                                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
                    
                        if( sIsTimeout != STL_TRUE )
                        {
                            break;
                        }
                        else
                        {
                            sRetryTime++;
                        
                            /**
                             * LogMirror Timeout 프로퍼티는 Immediate속성을 갖기 때문에 매번 읽도록 해야함
                             */
                            sRetryTimeout = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_MIRROR_TIMEOUT,
                                                                           KNL_ENV( aEnv ) );
                        
                            /**
                             * 10초마다 한번씩 로그에 기록한다.
                             */
                            if( sRetryTime % 10 == 0 )
                            {
                                STL_TRY( knlLogMsg( NULL,
                                                    KNL_ENV( aEnv ),
                                                    KNL_LOG_LEVEL_INFO,
                                                    "[LOGMIRROR] waiting for synchronizing message...\n" ) 
                                         == STL_SUCCESS );
                            }
                    
                            if( sRetryTimeout > 0 ) // 0 is infinite
                            {
                                if( sRetryTime >= sRetryTimeout )
                                {
                                    /**
                                     * Timeout이 발생한 이후의 처리
                                     * - LogMirror가 Disconnect 됐다고 세팅한 후에 이후 처리과정 Skip 하도록 함
                                     */
                                    STL_TRY( knlLogMsg( NULL,
                                                        KNL_ENV( aEnv ),
                                                        KNL_LOG_LEVEL_INFO,
                                                        "[LOGMIRROR] retry timeout. (Give-up LOGMIRROR-SERVICE)\n" ) 
                                            == STL_SUCCESS );
                                    
                                    sLogMirrorInfo->mConnected = SMR_LOGMIRROR_DISCONNECTED;
                                    sLogMirrorInfo->mIsStarted = SMR_LOGMIRROR_NOT_STARTED;
                                    gSmrWarmupEntry->mRecoveryInfo.mLogMirrorStat = 0;
                                    STL_THROW( RAMP_SKIP );
                                }
                            }
                        }
                    } //end of while 
                } //end of if
            } //end of for ... loop
        } //end of if
    }
    
    STL_RAMP( RAMP_SKIP );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    sLogMirrorInfo->mConnected = SMR_LOGMIRROR_DISCONNECTED;
    sLogMirrorInfo->mIsStarted = SMR_LOGMIRROR_NOT_STARTED;
    gSmrWarmupEntry->mRecoveryInfo.mLogMirrorStat = 0;
    
    return STL_FAILURE;
}


stlStatus smrCheckLogMirrorBuffer( smlEnv * aEnv )
{
    void             * sBaseAddr      = NULL;
    smrLogMirrorInfo * sLogMirrorInfo = NULL;
    stlInt64           sRetryTimeout  = 0;
    stlTime            sStartTime     = 0;
    stlTime            sEndTime       = 0;
    stlBool            sWriteTrc      = STL_FALSE;
    
    stlInt64   sWriteSeq = 0;
    stlInt64   sSendSeq  = 0;
    stlInt64   sCount    = 0;
    
    STL_TRY_THROW( gSmrLogMirrorShmAddrIdx != SMR_LOGMIRROR_INVALID_IDX, RAMP_SKIP );
    
    STL_TRY( knlGetShmSegmentAddrUnsafe( gSmrLogMirrorShmAddrIdx,
                                         &sBaseAddr,
                                         KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    sLogMirrorInfo = sBaseAddr; 
    
    if( sLogMirrorInfo->mConnected == SMR_LOGMIRROR_CONNECTED &&
        sLogMirrorInfo->mIsStarted == SMR_LOGMIRROR_STARTED )
    {
        sStartTime = stlNow();
        
        while( 1 )
        {
            stlAcquireSpinLock( &sLogMirrorInfo->mSpinLock,
                                NULL /* Miss count */ );
            
            sWriteSeq = sLogMirrorInfo->mWriteSeq;
            sSendSeq  = sLogMirrorInfo->mSendSeq;
            
            stlReleaseSpinLock( &sLogMirrorInfo->mSpinLock );
            
            /**
             * 계속 확인하도록 하며.. CPU를 많이 사용한다.
             * 중간에 Sleep 좀 해야하나?
             */ 
            if( sWriteSeq == sSendSeq ||
                sLogMirrorInfo->mConnected == SMR_LOGMIRROR_DISCONNECTED )
            {
                break;
            }
            sCount++;
            
            /**
             * 1000 번에 한번씩 체크하도록 함
             */
            if( sCount % 1000 == 0 )
            {
                /**
                 * LogMirror Timeout 프로퍼티는 Immediate속성을 갖기 때문에 매번 읽도록 해야함
                 */
                sRetryTimeout = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_MIRROR_TIMEOUT,
                                                               KNL_ENV( aEnv ) );
                
                sEndTime = stlNow();
                
                /**
                 * 10초마다 한번씩 로그에 기록한다.
                 */
                if( STL_GET_SEC_TIME( sEndTime - sStartTime ) % 10 == 0 )
                {
                    if( sWriteTrc == STL_TRUE )
                    {
                        STL_TRY( knlLogMsg( NULL,
                                            KNL_ENV( aEnv ),
                                            KNL_LOG_LEVEL_INFO,
                                            "[LOGMIRROR] waiting for synchronizing message....\n" ) 
                                 == STL_SUCCESS );
                    }
                    sWriteTrc = STL_FALSE;
                }
                else
                {
                    sWriteTrc = STL_TRUE;
                }
                
                if( sRetryTimeout > 0 )  // 0 is infinite
                {
                    if( STL_GET_SEC_TIME( sEndTime - sStartTime ) >= sRetryTimeout )
                    {
                        /**
                         * Timeout이 발생한 이후의 처리
                         * - LogMirror가 Disconnect 됐다고 세팅한 후에 이후 처리과정 Skip 하도록 함
                         */
                        STL_TRY( knlLogMsg( NULL,
                                            KNL_ENV( aEnv ),
                                            KNL_LOG_LEVEL_INFO,
                                            "[LOGMIRROR] retry timeout.. (Give-up LOGMIRROR-SERVICE)\n" ) 
                                == STL_SUCCESS );
                    
                        sLogMirrorInfo->mConnected = SMR_LOGMIRROR_DISCONNECTED;
                        sLogMirrorInfo->mIsStarted = SMR_LOGMIRROR_NOT_STARTED;
                        gSmrWarmupEntry->mRecoveryInfo.mLogMirrorStat = 0;
                        STL_THROW( RAMP_SKIP );
                    }
                }
                sCount = 0;
            }
            
        }//end of while
        
        /**
         * 다음을 처리하기 위해서 LogMirrorStat을 normal 상태로 변경
         */
        gSmrWarmupEntry->mRecoveryInfo.mLogMirrorStat = 0;
    }
    
    STL_RAMP( RAMP_SKIP );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    sLogMirrorInfo->mConnected = SMR_LOGMIRROR_DISCONNECTED;
    sLogMirrorInfo->mIsStarted = SMR_LOGMIRROR_NOT_STARTED;
    gSmrWarmupEntry->mRecoveryInfo.mLogMirrorStat = 0;
    
    return STL_FAILURE;
}


stlStatus smrCreateLogMirrorArea( smlEnv * aEnv )
{
    stlBool            sLogMirrorMode = STL_FALSE;
    stlInt64           sSize          = 0;
    stlUInt16          sIdx           = 0;
    smrLogMirrorInfo * sLogMirrorInfo = NULL;
    void             * sBaseAddr      = NULL;
    
    /**
     * LogMirror SharedMemeory를 생성한다.
     */
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_LOG_MIRROR_MODE,
                                      &sLogMirrorMode,
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    if( sLogMirrorMode == STL_TRUE )
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_LOG_MIRROR_SHARED_MEMORY_SIZE,
                                          &sSize,
                                          KNL_ENV( aEnv ) ) == STL_SUCCESS );
        
        STL_TRY( knlCreateDatabaseArea( SMR_LOGMIRROR_DEFAULT_SHM_NAME,
                                        &sIdx,
                                        sSize,
                                        KNL_ENV(aEnv) ) == STL_SUCCESS );
        
        gSmrLogMirrorShmAddrIdx = sIdx;
        
        /**
         * Initialize LogMirror Buffer
         */
        STL_TRY( knlGetShmSegmentAddrUnsafe( sIdx,
                                             &sBaseAddr,
                                             KNL_ENV( aEnv ) ) == STL_SUCCESS );
        
        sLogMirrorInfo = sBaseAddr; 
        
        sLogMirrorInfo->mWriteSeq   = 0;
        sLogMirrorInfo->mSendSeq    = 0;
        sLogMirrorInfo->mSize       = STL_ALIGN( sSize - SMR_LOGMIRROR_BUFF_OFFSET - STL_SIZEOF( smrLogMirrorHdr ), 512 ) - 512;   //16M로 Align 되며 512Byte로 Align되도록 해야한다.
        sLogMirrorInfo->mConnected  = SMR_LOGMIRROR_DISCONNECTED;
        sLogMirrorInfo->mIsStarted  = SMR_LOGMIRROR_NOT_STARTED;
        
        stlInitSpinLock( &sLogMirrorInfo->mSpinLock );
        
        STL_TRY( stlCreateSemaphore( &(sLogMirrorInfo->mWaitDataSem),
                                     "LMWDsem",
                                     0,
                                     KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
        
        STL_TRY( stlCreateSemaphore( &(sLogMirrorInfo->mWaitSendSem),
                                     "LMWSsem",
                                     0,
                                     KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smrDestroyLogMirrorArea( smlEnv * aEnv )
{
    stlBool            sLogMirrorMode = STL_FALSE;
    smrLogMirrorInfo * sLogMirrorInfo = NULL;
    void             * sBaseAddr      = NULL;
    
    /**
     * LogMirror를 초기화한다.
     */
    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_LOG_MIRROR_MODE,
                                      &sLogMirrorMode,
                                      KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    if( sLogMirrorMode == STL_TRUE && 
        gSmrLogMirrorShmAddrIdx != SMR_LOGMIRROR_INVALID_IDX )
    {
        STL_TRY( knlGetShmSegmentAddrUnsafe( gSmrLogMirrorShmAddrIdx,
                                             &sBaseAddr,
                                             KNL_ENV( aEnv ) ) == STL_SUCCESS );
        
        sLogMirrorInfo = sBaseAddr; 
        
        sLogMirrorInfo->mConnected  = SMR_LOGMIRROR_DISCONNECTED;
        sLogMirrorInfo->mIsStarted  = SMR_LOGMIRROR_NOT_STARTED;
        
        STL_TRY( stlDestroySemaphore( &(sLogMirrorInfo->mWaitDataSem),
                                      KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
        
        STL_TRY( stlDestroySemaphore( &(sLogMirrorInfo->mWaitSendSem),
                                      KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
        
        gSmrLogMirrorShmAddrIdx = SMR_LOGMIRROR_INVALID_IDX;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
