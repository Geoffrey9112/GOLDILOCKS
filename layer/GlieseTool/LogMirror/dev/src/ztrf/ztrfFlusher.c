/*******************************************************************************
 * ztrfFlusher.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztrfFlusher.c
 * @brief GlieseTool LogMirror Flusher Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool       gRunState;
extern ztrSlaveMgr   gSlaveMgr;
extern ztrConfigure  gZtrConfigure;

stlBool  gZtrFlusherState    = STL_TRUE;

stlBool  gZtrDoRecovery      = STL_FALSE;
stlInt64 gZtrRecoveryFileSeq = -1;

stlStatus ztrfDoFlush( stlErrorStack * aErrorStack )
{
    ztrChunkItem   * sChunkItem  = NULL;
    ztrLogFileHdr  * sLogFileHdr = NULL;
    stlChar          sFileName[ STL_MAX_FILE_PATH_LENGTH ];
    stlFile          sFile;
    stlInt32         sState     = 0;
    stlBool          sFileOpen  = STL_FALSE;
    stlBool          sIsTimeout = STL_TRUE;
    stlSize          sWriteSize = 0;
    stlBool          sIsExist   = STL_FALSE;
    
    while( sIsTimeout == STL_TRUE ) 
    {
        STL_TRY( stlTimedAcquireSemaphore( &(gSlaveMgr.mWaitFlushSem),
                                           STL_SET_SEC_TIME( 1 ),
                                           &sIsTimeout,
                                           aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( gZtrFlusherState == STL_TRUE );
    }
    
    if( gZtrDoRecovery == STL_TRUE )
    {
        STL_TRY( ztrmGetLogFileName( sFileName,
                                     gZtrRecoveryFileSeq,
                                     aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztrmLogMsg( aErrorStack,
                             "[FLUSHER] RECOVERY FILE [%s]\n", 
                             sFileName ) == STL_SUCCESS );
        
        STL_TRY( stlExistFile( sFileName,
                               &sIsExist,
                               aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sIsExist == STL_TRUE, RAMP_ERR_INVALID_FILE );
        
        STL_TRY( stlOpenFile( &sFile,
                              sFileName,
                              STL_FOPEN_WRITE | STL_FOPEN_APPEND | STL_FOPEN_DIRECT  | STL_FOPEN_LARGEFILE,
                              STL_FPERM_OS_DEFAULT,
                              aErrorStack ) == STL_SUCCESS );
        sFileOpen = STL_TRUE;
        sState    = 1;
    }
    
    while( 1 )
    {
        STL_TRY( ztrfGetChunkItemToRead( &sChunkItem,
                                         aErrorStack ) == STL_SUCCESS );
        
        if( sChunkItem->mHasLogFileHdr == STL_TRUE )
        {
            if( sFileOpen == STL_TRUE )
            {
                STL_TRY( stlCloseFile( &sFile,
                                       aErrorStack ) == STL_SUCCESS );
            }
            
            sLogFileHdr = (ztrLogFileHdr*)sChunkItem->mBuffer;

            STL_TRY( ztrmGetLogFileName( sFileName,
                                         sLogFileHdr->mLogFileSeqNo,
                                         aErrorStack ) == STL_SUCCESS );
            
            STL_TRY( ztrmLogMsg( aErrorStack,
                                 "[FLUSHER] CREATE LOGFILE\n"
                                 "-- GROUP ID        : %d\n"
                                 "-- BLOCK SIZE      : %d\n"
                                 "-- MAX BLOCK COUNT : %ld\n"
                                 "-- LOG FILE SEQ NO : %ld\n"
                                 "-- FILE SIZE       : %ld\n"
                                 "-- FILE NAME       : %s\n", 
                                 sLogFileHdr->mLogGroupId,
                                 sLogFileHdr->mBlockSize,
                                 sLogFileHdr->mMaxBlockCount,
                                 sLogFileHdr->mLogFileSeqNo,
                                 sLogFileHdr->mFileSize,
                                 sFileName ) == STL_SUCCESS );
            
            STL_TRY( stlOpenFile( &sFile,
                                  sFileName,
                                  STL_FOPEN_WRITE | STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE | STL_FOPEN_DIRECT  | STL_FOPEN_LARGEFILE,
                                  STL_FPERM_OS_DEFAULT,
                                  aErrorStack ) == STL_SUCCESS );
            sFileOpen = STL_TRUE;
            sState    = 1;
        }
        
        STL_TRY( stlWriteFile( &sFile,
                               sChunkItem->mBuffer,
                               sChunkItem->mWriteIdx,
                               &sWriteSize,
                               aErrorStack ) == STL_SUCCESS );
        
        STL_ASSERT( sChunkItem->mWriteIdx == sWriteSize );
        
        if( sChunkItem->mHasLogFileHdr == STL_TRUE )
        {
            /**
             * Redo LogFile을 먼저 쓴 후에 Control File을 Update 해야 한다.
             */
            STL_TRY( ztrfUpdateControlFile( sChunkItem->mBuffer,
                                            aErrorStack ) == STL_SUCCESS );
            
        }
        
        STL_TRY( ztrfPushChunkToWaitWriteList( sChunkItem, 
                                               aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break;
    }
    
    /**
     * Flusher가 종료했을땐 Receiver도 함께 종료해야 함!!
     */
    gRunState = STL_FALSE;
    
    return STL_FAILURE;
}

stlStatus ztrfUpdateControlFile( stlChar       * aBuffer,
                                 stlErrorStack * aErrorStack )
{
    stlInt32         sState     = 0;
    stlSize          sWriteSize = 0;
    stlFile          sFile;
    stlChar          sFileName[ STL_MAX_FILE_PATH_LENGTH ];
    
    STL_TRY( ztrmGetControlFileName( sFileName,
                                     aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlOpenFile( &sFile,
                          sFileName,
                          STL_FOPEN_WRITE | STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE | STL_FOPEN_DIRECT,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( stlWriteFile( &sFile,
                           aBuffer,
                           ZTR_DEFAULT_BLOCK_SIZE,
                           &sWriteSize,
                           aErrorStack ) == STL_SUCCESS );
    
    STL_ASSERT( ZTR_DEFAULT_BLOCK_SIZE == sWriteSize );
    
    sState = 0;
    
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch ( sState )
    {
        case 1:
            (void) stlCloseFile( &sFile,
                                 aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus ztrfReadControlFile( ztrLogFileHdr * aLogFileHdr,
                               stlBool       * aIsExist,
                               stlErrorStack * aErrorStack )
{
    stlFile      sFile;
    stlChar      sFileName[ STL_MAX_FILE_PATH_LENGTH ];
    stlChar    * sBufferOrg = NULL;
    stlChar    * sBuffer    = NULL;
    stlInt32     sState     = 0;
    stlSize      sReadSize  = 0;
    stlBool      sIsExist   = STL_FALSE;
    
    STL_TRY( ztrmGetControlFileName( sFileName,
                                     aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sIsExist,
                           aErrorStack ) == STL_SUCCESS );
    
    if( sIsExist == STL_TRUE )
    {
        STL_TRY( stlAllocHeap( (void**)&sBufferOrg,
                               ZTR_DEFAULT_BLOCK_SIZE + ZTR_MEM_ALIGN,
                               aErrorStack ) == STL_SUCCESS );
        sState = 1;
        
        STL_TRY( stlOpenFile( &sFile,
                              sFileName,
                              STL_FOPEN_READ | STL_FOPEN_DIRECT,
                              STL_FPERM_OS_DEFAULT,
                              aErrorStack ) == STL_SUCCESS );
        sState = 2;
    
        sBuffer = (void*)STL_ALIGN( (stlUInt64)sBufferOrg, ZTR_MEM_ALIGN );
    
        STL_TRY( stlReadFile( &sFile,
                              sBuffer,
                              ZTR_DEFAULT_BLOCK_SIZE,
                              &sReadSize,
                              aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sReadSize == ZTR_DEFAULT_BLOCK_SIZE, RAMP_ERR_INVALID_CONTROL_FILE );
        
        *aIsExist = STL_TRUE;
        stlMemcpy( (stlChar*)aLogFileHdr, sBuffer, STL_SIZEOF( ztrLogFileHdr ) );
        
        sState = 1;
        STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
        
        sState = 0;
        stlFreeHeap( sBufferOrg );
    }
    else
    {
        *aIsExist = STL_FALSE;
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_CONTROL_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_CONTROL_FILE,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    if( sIsExist == STL_TRUE )
    {
        switch( sState )
        {
            case 2:
                (void) stlCloseFile( &sFile,
                                     aErrorStack );
            case 1:
                stlFreeHeap( sBufferOrg );
            default:
                break;
        }
    }
    
    return STL_FAILURE;
}

stlStatus ztrfGetLastBlockSeq( stlInt64        aLogFileSeqNo,
                               stlInt64      * aLastBlockSeq,
                               stlErrorStack * aErrorStack )
{
    stlChar      sFileName[ STL_MAX_FILE_PATH_LENGTH ];
    stlBool      sIsExist = STL_FALSE;
    stlFileInfo  sFileInfo;
    
    STL_TRY( ztrmGetLogFileName( sFileName,
                                 aLogFileSeqNo,
                                 aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           &sIsExist,
                           aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sIsExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
    
    STL_TRY( stlGetFileStatByName( &sFileInfo,
                                   sFileName,
                                   STL_FINFO_MIN,
                                   aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileInfo.mSize > ZTR_DEFAULT_BLOCK_SIZE &&
                   sFileInfo.mSize % ZTR_DEFAULT_BLOCK_SIZE == 0, RAMP_ERR_INVALID_FILE );
    
    *aLastBlockSeq = sFileInfo.mSize / ZTR_DEFAULT_BLOCK_SIZE - 1;  //BlockSeq .. 1Base.
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_FILE_NAME,
                      NULL,
                      aErrorStack,
                      sFileName );
    }
    STL_CATCH( RAMP_ERR_INVALID_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTR_ERRCODE_INVALID_CONTROL_FILE,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */

