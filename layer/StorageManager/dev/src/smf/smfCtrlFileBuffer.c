/*******************************************************************************
 * smfCtrlFileBuffer.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smfCtrlFileBuffer.c 9234 2013-07-31 08:24:41Z kyungoh $
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
#include <smfDef.h>
#include <smf.h>
#include <smg.h>
#include <smxl.h>
#include <smrDef.h>
#include <smr.h>
#include <smfCtrlFileBuffer.h>

/**
 * @file smfCtrlFileBuffer.c
 * @brief Storage Manager Layer Control File Manager Internal Routines
 * @todo Control file에 대한 동시성 제어 로직이 추가되어야 함.
 * @todo smfSaveCtrlFile은 에러가 발생하지 않도록 설계되어야 한다.
 */

/**
 * @addtogroup smf
 * @{
 */

stlStatus smfFlushCtrlFile( stlFile       * aFile,
                            smfCtrlBuffer * aBuffer,
                            smlEnv        * aEnv )
{
    stlSize     sSize;
    stlSize     sWrittenBytes;

    /**
     * buffer에 기록된 내용이 있을 경우 disk에 flush한다.
     */
    if( aBuffer->mOffset > 0 )
    {
        sSize = STL_ALIGN( (stlInt64)aBuffer->mOffset, SMF_CONTROLFILE_IO_BLOCK_SIZE );

        STL_DASSERT( sSize <= aBuffer->mMaxSize );

        STL_TRY( stlWriteFile( aFile,
                               aBuffer->mBuffer,
                               sSize,
                               &sWrittenBytes,
                               KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        aBuffer->mOffset = 0;
    }

    stlMemset( aBuffer->mBuffer, 0x00, aBuffer->mMaxSize );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfWriteCtrlFile( stlFile       * aFile,
                            smfCtrlBuffer * aBuffer,
                            stlChar       * aContents,
                            stlInt32        aSize,
                            stlSize       * aWrittenBytes,
                            smlEnv        * aEnv )
{
    stlInt32    sLeftSize;
    stlInt32    sSize;
    stlInt32    sOffset = 0;

    sLeftSize = aSize;

    while( STL_TRUE )
    {
        /**
         * buffer 크기가 부족하면 buffer를 flush한 후 buffer에 기록한다.
         */
        if( sLeftSize > (aBuffer->mMaxSize - aBuffer->mOffset) )
        {
            sSize = aBuffer->mMaxSize - aBuffer->mOffset;

            stlMemcpy( (aBuffer->mBuffer + aBuffer->mOffset),
                       aContents + sOffset,
                       sSize );

            aBuffer->mOffset += sSize;
            sOffset += sSize;
            sLeftSize -= sSize;

            STL_TRY( smfFlushCtrlFile( aFile,
                                       aBuffer,
                                       aEnv ) == STL_SUCCESS );
        }
        else
        {
            stlMemcpy( (aBuffer->mBuffer + aBuffer->mOffset),
                       aContents + sOffset,
                       sLeftSize );

            aBuffer->mOffset += sLeftSize;
            break;
        }
    }

    *aWrittenBytes = aSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfReadCtrlFile( stlFile       * aFile,
                           smfCtrlBuffer * aBuffer,
                           stlChar       * aContents,
                           stlInt32        aSize,
                           smlEnv        * aEnv )
{
    stlInt32    sLeftSize;
    stlInt32    sOffset = 0;
    stlInt32    sSize;
    stlSize     sReadBytes;

    /**
     * Buffer에 캐싱된 내용이 있으면 buffer를 읽는다.
     */
    if( aBuffer->mReadSize > aBuffer->mOffset )
    {
        sSize = aBuffer->mReadSize - aBuffer->mOffset;

        if( sSize < aSize )
        {
            stlMemcpy( aContents,
                       aBuffer->mBuffer + aBuffer->mOffset,
                       sSize );

            sLeftSize = aSize - sSize;
            sOffset = sSize;

            aBuffer->mOffset = aBuffer->mReadSize = 0;
        }
        else
        {
            stlMemcpy( aContents,
                       aBuffer->mBuffer + aBuffer->mOffset,
                       aSize );

            sLeftSize = 0;
            aBuffer->mOffset += aSize;
        }
    }
    else
    {
        sLeftSize = aSize;
        aBuffer->mOffset = aBuffer->mReadSize = 0;
    }

    /**
     * 요청된 size가 buffer에 캐싱된 내용보다 더 큰 경우 controlfile을 읽어서 buffer에 채운다.
     */
    while( sLeftSize > 0 )
    {
        STL_TRY( stlReadFile( aFile,
                              aBuffer->mBuffer,
                              SMF_CONTROLFILE_IO_BLOCK_SIZE,
                              &sReadBytes,
                              KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY_THROW( SMF_CONTROLFILE_IO_BLOCK_SIZE == sReadBytes,
                       RAMP_ERR_CORRUPTED_CONTROLFILE );

        if( sLeftSize > SMF_CONTROLFILE_IO_BLOCK_SIZE )
        {
            stlMemcpy( (aContents + sOffset),
                       aBuffer->mBuffer,
                       SMF_CONTROLFILE_IO_BLOCK_SIZE );

            sOffset += SMF_CONTROLFILE_IO_BLOCK_SIZE;
            sLeftSize -= SMF_CONTROLFILE_IO_BLOCK_SIZE;
        }
        else
        {
            stlMemcpy( (aContents + sOffset), aBuffer->mBuffer, sLeftSize );

            aBuffer->mReadSize = SMF_CONTROLFILE_IO_BLOCK_SIZE;
            aBuffer->mOffset = sLeftSize;
            break;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CORRUPTED_CONTROLFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CORRUPTED_CONTROLFILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Controlfile을 읽기 위해 현재 위치 혹은 file의 첫 위치에서 offset만큼 position을 옮긴다.
 * @param[in] aFile   Controlfile 포인터
 * @param[in] aBuffer Controlfile을 읽기 위한 buffer
 * @param[in] aWhere  Position을 옮길 시작 위치(STL_FSEEK_SET, STL_FSEEK_CUR)
 * @param[in] aOffset Position을 옮길 위치
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smfMoveCtrlFilePosition( stlFile       * aFile,
                                   smfCtrlBuffer * aBuffer,
                                   stlInt32        aWhere,
                                   stlOffset       aOffset,
                                   smlEnv        * aEnv )
{
    stlInt32    sLeftSize = 0;
    stlOffset   sOffset;
    stlSize     sReadBytes;

    STL_DASSERT( aOffset >= 0 );

    switch( aWhere )
    {
        case STL_FSEEK_SET:
            /**
             * Controlfile의 처음부터 절대 위치를 skip한다.
             */ 
            sOffset = (aOffset / SMF_CONTROLFILE_IO_BLOCK_SIZE) * SMF_CONTROLFILE_IO_BLOCK_SIZE;
            sLeftSize = aOffset - sOffset;

            STL_TRY( stlSeekFile( aFile,
                                  STL_FSEEK_SET,
                                  &sOffset,
                                  KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

            aBuffer->mReadSize = 0;
            aBuffer->mOffset = 0;
            break;

        case STL_FSEEK_CUR:
            /**
             * Buffer를 이용하여 읽은 controlfile의 마지막 위치부터
             * 주어진 크기만큼 skip한다.
             */ 
            if( aOffset < (aBuffer->mReadSize - aBuffer->mOffset) )
            {
                /**
                 * buffer내에서 skip 가능한 경우 buffer의 offset만 변경
                 */
                aBuffer->mOffset += aOffset;
                sLeftSize = 0;
            }
            else
            {
                /**
                 * buffer보다 더 큰 경우 controlfile을 direct io하는 한 block보다
                 * 더 큰 경우 block 단위로 skip한다.
                 */
                sLeftSize = aOffset - (aBuffer->mReadSize - aBuffer->mOffset);

                if( sLeftSize >= SMF_CONTROLFILE_IO_BLOCK_SIZE )
                {
                    sOffset = (sLeftSize / SMF_CONTROLFILE_IO_BLOCK_SIZE) * SMF_CONTROLFILE_IO_BLOCK_SIZE;
                    sLeftSize = sLeftSize - sOffset;

                    STL_TRY( stlSeekFile( aFile,
                                          STL_FSEEK_CUR,
                                          &sOffset,
                                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
                }

                aBuffer->mReadSize = 0;
                aBuffer->mOffset = 0;
            }
            break;

        case STL_FSEEK_END:
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }

    STL_TRY( stlReadFile( aFile,
                          (aBuffer->mBuffer + aBuffer->mOffset),
                          SMF_CONTROLFILE_IO_BLOCK_SIZE,
                          &sReadBytes,
                          KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY_THROW( SMF_CONTROLFILE_IO_BLOCK_SIZE == sReadBytes, RAMP_ERR_CORRUPTED_CONTROLFILE );

    aBuffer->mReadSize += sReadBytes;
    aBuffer->mOffset += sLeftSize;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CORRUPTED_CONTROLFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CORRUPTED_CONTROLFILE,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
