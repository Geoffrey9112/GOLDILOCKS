/*******************************************************************************
 * smrLogStream.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smrLogStream.c 1012 2011-06-10 02:14:58Z leekmo $
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
#include <smrDef.h>
#include <smr.h>
#include <smrLogFile.h>
#include <smrLogStream.h>
#include <smrLogBuffer.h>
#include <smrPendBuffer.h>
#include <smrLogGroup.h>

/**
 * @file smrLogStream.c
 * @brief Storage Manager Layer Recovery Log Stream Internal Routines
 */

extern smrWarmupEntry * gSmrWarmupEntry;

/**
 * @addtogroup smr
 * @{
 */

/**
 * @brief Log Stream을 생성한다.
 * @param[in] aAttr Log Stream 속성
 * @param[in,out] aEnv Environment
 */
stlStatus smrCreateLogStream( smlLogStreamAttr * aAttr,
                              smlEnv           * aEnv )
{
    smrLogStream * sLogStream;
    stlChar      * sBuffer;
    stlInt64       sBufferSize;
    stlInt64       sPendLogBufferCount;
    stlInt64       sMinLogFileSize;
    stlUInt32      i;

    STL_PARAM_VALIDATE( aAttr != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sLogStream = &gSmrWarmupEntry->mLogStream;

    /*
     * Persistent Information 설정
     */
    sLogStream->mLogStreamPersData.mState         = SMR_LOG_STREAM_STATE_ACTIVE;
    sLogStream->mLogStreamPersData.mLogGroupCount = 0;
    sLogStream->mLogStreamPersData.mCurFileSeqNo++;

    if( ( aAttr->mValidFlags & SML_LOG_STREAM_BLOCKSIZE_MASK ) == SML_LOG_STREAM_BLOCKSIZE_YES )
    {
        sLogStream->mLogStreamPersData.mBlockSize = aAttr->mBlockSize;
    }
    else
    {
        sLogStream->mLogStreamPersData.mBlockSize = SMR_LOG_STREAM_DEFAULT_BLOCKSIZE;
    }

    STL_TRY( smrAddLogGroup( aAttr,
                             STL_TRUE,     /* aCreateDb */
                             &sMinLogFileSize,
                             aEnv ) == STL_SUCCESS );

    sLogStream->mCurLogGroup4Disk = STL_RING_GET_FIRST_DATA( &sLogStream->mLogGroupList );
    sLogStream->mCurLogGroup4Buffer = sLogStream->mCurLogGroup4Disk;

    /*
     * Log Buffer 설정
     */
    sBufferSize = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_BUFFER_SIZE,
                                                 KNL_ENV( aEnv ) );
    sPendLogBufferCount =
        knlGetPropertyBigIntValueByID( KNL_PROPERTY_PENDING_LOG_BUFFER_COUNT,
                                       KNL_ENV( aEnv ) );

    /**
     * 최소 로그파일 크기가 로그 버퍼와 펜딩로그버퍼들의 합보다 작으면
     * Checkpoint가 실패할 수 있으므로 로그버퍼의 크기를 조정한다
     */
    if( sBufferSize + (sPendLogBufferCount * SML_PENDING_LOG_BUFFER_SIZE) > sMinLogFileSize )
    {
        sBufferSize = sMinLogFileSize - (sPendLogBufferCount * SML_PENDING_LOG_BUFFER_SIZE);
        knlLogMsg( NULL,
                   KNL_ENV(aEnv),
                   KNL_LOG_LEVEL_WARNING,
                   "Too large Online Redo Log Buffer and pending log buffer size.\n"
                   "Online Redo Log Buffer is resized to %ld bytes.\n",
                   sBufferSize );
    }

    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF(smrLogBuffer),
                                      (void**)&sLogStream->mLogBuffer,
                                      KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * ALIGN을 고려해 BLOCK SIZE 만큼의 추가 메모리를 할당한다.
     */
    STL_TRY( knlAllocFixedStaticArea( sBufferSize + sLogStream->mLogStreamPersData.mBlockSize,
                                      (void**)&sBuffer,
                                      (knlEnv*)aEnv )
             == STL_SUCCESS );
        
    SMR_INIT_LOG_BUFFER( sLogStream->mLogBuffer,
                         sBufferSize,
                         sBuffer,
                         STL_TRUE, /* aIsShared */
                         sLogStream->mLogStreamPersData.mBlockSize );

    STL_TRY( smrFormatLogBuffer( sLogStream->mLogBuffer->mBuffer,
                                 sLogStream->mLogBuffer->mBufferSize,
                                 sLogStream->mLogStreamPersData.mBlockSize,
                                 aEnv ) == STL_SUCCESS );
    
    /**
     * Pending Log Buffer 할당 및 초기화
     */

    sLogStream->mPendBufferCount = sPendLogBufferCount;

    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF(smrPendLogBuffer) * sPendLogBufferCount,
                                                  (void**)&sLogStream->mPendLogBuffer,
                                                  (knlEnv*)aEnv )
             == STL_SUCCESS );
        
    for( i = 0; i < sPendLogBufferCount; i++ )
    {
        STL_TRY( smrInitPendLogBuffer( &sLogStream->mPendLogBuffer[i],
                                       aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smrFormatPendLogBuffer( &sLogStream->mPendLogBuffer[i],
                                         aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Log Stream을 삭제한다.
 * @param[in] aAttr Log Stream 속성
 * @param[in,out] aEnv Environment
 */
stlStatus smrDropLogStream( smlLogStreamAttr * aAttr,
                            smlEnv           * aEnv )
{
    STL_ASSERT( 0 );
    return STL_FAILURE;
}

/**
 * @brief Log Stream의 상태를 반환한다.
 * @param[in] aLogStream Log Stream
 */
inline stlInt32 smrGetLogStreamState( smrLogStream * aLogStream )
{
    return aLogStream->mLogStreamPersData.mState;
}

/** @} */
