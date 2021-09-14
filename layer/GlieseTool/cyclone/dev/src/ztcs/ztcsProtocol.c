/*******************************************************************************
 * ztcsProtocol.c
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
 * @file ztcsProtocol.c
 * @brief GlieseTool Cyclone Sender Protocol Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern stlBool         gRunState;
extern ztcMasterMgr  * gMasterMgr;
extern ztcMonitorInfo  gMonitorInfo;

stlStatus ztcsDoProtocol( stlErrorStack * aErrorStack )
{
    /**
     * Sender Restart 시에 남아있는 Chunk를 Capture에서 사용하도록 옮긴다.
     */
    STL_TRY( ztcsPushAllChunkToWaitWriteList( aErrorStack ) == STL_SUCCESS );
    
    /**
     * 1. HandShake
     * 2. SYNC DATA ( if exist...)
     * 3. Data 전송
     */
    
    STL_TRY( ztcsHandshakeProtocol( aErrorStack ) == STL_SUCCESS );
    
    /**
     * Sync Mode일 경우 데이터 Sync를 먼저 수행한다.
     */
    if( ztcmGetSyncMode() == STL_TRUE )
    {
        STL_TRY( ztcsSyncProtocol( STL_TRUE, 
                                   aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcnSetSyncStartPosition( aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcnSync( aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcsSyncProtocol( STL_FALSE, 
                                   aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztcsSyncProtocol( STL_FALSE, 
                                   aErrorStack ) == STL_SUCCESS );
    }
    
    /**
     * Sync가 끝난 이후에 Capture 시작해야 한다.
     */
    STL_TRY( ztccWakeUpCapture( aErrorStack ) == STL_SUCCESS );
    
    /**
     * Sync가 끝난 이후에 Slave가 재접속하게 되면 위의 과정을 다시 수행하게 된다.
     * 즉, Slave 재접속할 때마다 Sync가 재수행되므로, 한번 Sync과정을 수행한 이후에는 Sync를 수행하지 않도록 해야한다.
     */
    ztcmSetSyncMode( STL_FALSE );
            
    if( ztcsSendFlowProtocol( aErrorStack ) != STL_SUCCESS )
    {
        if( gRunState == STL_FALSE )
        {
            STL_TRY( ztcsEndProtocol( aErrorStack ) == STL_SUCCESS );
        }
                   
        STL_TRY( gRunState == STL_FALSE );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcsInitProtocol( stlContext    * aContext,
                            stlChar       * aClientType,
                            stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    stlChar          sClientType;

    /**
     * Send Request ClientType
     */
    sSndHeader.mCategory = ZTC_CATEGORY_INIT;
    sSndHeader.mType     = ZTC_INIT_REQUEST_CLIENT_TYPE;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztcmSendProtocolHdrNData( aContext,
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    /**
     * Recv ClientType
     */
    STL_TRY( ztcmReceiveProtocolHdr( aContext,
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_INIT && 
        sRcvHeader.mType == ZTC_INIT_RESPONSE_CLIENT_TYPE )
    {
        STL_TRY( ztcmReceiveData( aContext,
                                  (stlChar*)&sClientType,
                                  STL_SIZEOF( sClientType ),
                                  aErrorStack ) == STL_SUCCESS );
                
        stlMemcpy( aClientType, &sClientType, STL_SIZEOF( sClientType ) );
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );   
    }
    
    return STL_SUCCESS;
        
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header(Init)" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcsHandshakeProtocol( stlErrorStack * aErrorStack )
{
    STL_TRY( ztcsRequestTableInfo( aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcsRequestCaptureInfo( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcsRequestTableInfo( stlErrorStack * aErrorStack )
{
    stlChar    sData[STL_MAX_SQL_IDENTIFIER_LENGTH * 4];
    stlInt32   sTableCount = 0;
    stlInt32   sIdx        = 0;
    stlInt32   sOffset     = 0;
    stlInt32   sSize       = 0;
    
    stlChar    sMasterSchema[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sMasterTable[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sSlaveSchema[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlChar    sSlaveTable[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    
    ztcProtocolHdr       sSndHeader;
    ztcMasterCMMgr     * sCMMgr     = &gMasterMgr->mCMMgr;
    ztcMasterTableMeta * sTableMeta = NULL;
    
    /**
     * Request TableCount
     */
    STL_TRY( ztcsRequestTableCount( &sTableCount,
                                    aErrorStack ) == STL_SUCCESS );
    
    for( sIdx = 0; sIdx < sTableCount; sIdx++ )
    {
        stlMemset( sData, 0x00, STL_SIZEOF( sData ) );
        
        STL_TRY( ztcsRequestTableName( sData,
                                       aErrorStack ) == STL_SUCCESS );
        
        sOffset = 0;
        
        /**
         * Read Master Schema
         */
        STL_READ_MOVE_INT32( &sSize, sData, sOffset );
        stlStrncpy( sMasterSchema, sData + sOffset, sSize + 1 );
        sOffset += sSize;
        
        /**
         * Read Master Table
         */
        STL_READ_MOVE_INT32( &sSize, sData, sOffset );
        stlStrncpy( sMasterTable, sData + sOffset, sSize + 1 );
        sOffset += sSize;
        
        /**
         * Read Slave Schema
         */
        STL_READ_MOVE_INT32( &sSize, sData, sOffset );
        stlStrncpy( sSlaveSchema, sData + sOffset, sSize + 1 );
        sOffset += sSize;
        
        /**
         * Read Slave Table
         */
        STL_READ_MOVE_INT32( &sSize, sData, sOffset );
        stlStrncpy( sSlaveTable, sData + sOffset, sSize + 1 );
        sOffset += sSize;
        
        if( stlStrlen( sMasterSchema ) == 0 ||
            stlStrlen( sMasterTable ) == 0 ||
            stlStrlen( sSlaveSchema ) == 0 ||
            stlStrlen( sSlaveTable ) == 0 )
        {
            sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
            sSndHeader.mType     = ZTC_HANDSHAKE_RESPONSE_INVALID_TABLE_NAME;
            sSndHeader.mLength   = 0;
            
            STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                               sSndHeader,
                                               NULL,
                                               aErrorStack ) == STL_SUCCESS );
            
            STL_THROW( RAMP_ERR_INVALID_TABLE_NAME );
        }
    
        STL_TRY( ztcmFindTableMeta( &sTableMeta,
                                    sMasterSchema,
                                    sMasterTable,
                                    aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sTableMeta != NULL, RAMP_ERR_INVALID_TABLE_NAME );
        
        stlStrncpy( sTableMeta->mSlaveSchema, sSlaveSchema, stlStrlen( sSlaveSchema ) + 1 );
        stlStrncpy( sTableMeta->mSlaveTable, sSlaveTable, stlStrlen( sSlaveTable ) + 1 );
        
        STL_TRY( ztcsResponseTableId( sTableMeta->mTableId,
                                      aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcsCompareColumnInfo( sTableMeta,
                                        aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_TABLE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      sMasterTable,
                      aErrorStack,
                      "Invalid Table Name" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcsResponseTableId( stlInt32        aTableId,
                               stlErrorStack * aErrorStack )
{
    stlInt32         sTableId = aTableId;
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;

    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mSenderContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_REQUEST_TABLE_ID )
    {
        sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
        sSndHeader.mType     = ZTC_HANDSHAKE_RESPONSE_TABLE_ID;
        sSndHeader.mLength   = STL_SIZEOF( sTableId );
        
        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mSenderContext),
                                           sSndHeader,
                                           (stlChar*)&sTableId,
                                           aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );   
    }
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header(Res Table Id)" );
    }
    STL_FINISH;
    
    return STL_FAILURE;    
}

stlStatus ztcsCompareColumnInfo( ztcMasterTableMeta * aTableMeta,
                                 stlErrorStack      * aErrorStack )
{
    ztcProtocolColumnInfo sColumnInfo;
    ztcMasterColumnMeta  * sColumnMeta;
    
    STL_TRY( ztcsResponseColumnCount( aTableMeta->mColumnCount,
                                      aErrorStack ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( &aTableMeta->mColumnMetaList, sColumnMeta )
    {
        stlMemset( &sColumnInfo, 0x00, STL_SIZEOF( ztcProtocolColumnInfo ) );
        
        stlStrncpy( sColumnInfo.mColumnName, sColumnMeta->mColumnName, stlStrlen( sColumnMeta->mColumnName ) + 1 );
        stlStrncpy( sColumnInfo.mDataType, sColumnMeta->mDataType, stlStrlen( sColumnMeta->mDataType ) + 1 );
        sColumnInfo.mIsPrimary  = sColumnMeta->mIsPrimary;
        sColumnInfo.mIsUnique   = sColumnMeta->mIsUnique;
        sColumnInfo.mIsNullable = sColumnMeta->mIsNullable;
        
        STL_TRY( ztcsResponseColumnInfo( &sColumnInfo,
                                         aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;   

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcsResponseColumnInfo( ztcProtocolColumnInfo * aColumnInfo,
                                  stlErrorStack         * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mSenderContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_REQUEST_COLUMN_INFO )
    {
        sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
        sSndHeader.mType     = ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO;
        sSndHeader.mLength   = STL_SIZEOF( ztcProtocolColumnInfo );
        
        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mSenderContext),
                                           sSndHeader,
                                           (stlChar*)aColumnInfo,
                                           aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );   
    }
    
    /**
     * Result
     */ 
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mSenderContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
        
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE )
    {
        switch( sRcvHeader.mType )
        {
            case ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_SUCCESS:
            {
                //Nothing To do.
                break;
            }
            case ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_FAIL:
            {
                STL_THROW( RAMP_ERR_COLUMN_COMPARE_FAIL );
                break;
            }
            default:
                break;
        }
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );   
    }
    
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header(Res Column Info)" );
    }   
    STL_CATCH( RAMP_ERR_COLUMN_COMPARE_FAIL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "fail to compare column" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcsResponseColumnCount( stlInt32        aColumnCount,
                                   stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcMasterCMMgr * sCMMgr       = &gMasterMgr->mCMMgr;
    stlInt32         sColumnCount = aColumnCount;
    
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mSenderContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_REQUEST_COLUMN_COUNT )
    {
        sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
        sSndHeader.mType     = ZTC_HANDSHAKE_RESPONSE_COLUMN_COUNT;
        sSndHeader.mLength   = STL_SIZEOF( sColumnCount );
        
        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mSenderContext),
                                           sSndHeader,
                                           (stlChar*)&sColumnCount,
                                           aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );   
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header(Res Column Count)" );
    }   
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcsRequestTableCount( stlInt32      * aTableCount,
                                 stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    stlInt32         sTableCount = 0;
    
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    
     /**
     * Send Request TableCount
     */
    sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
    sSndHeader.mType     = ZTC_HANDSHAKE_REQUEST_TABLE_COUNT;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmReceiveProtocolHdr( &sCMMgr->mSenderContext,
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_RESPONSE_TABLE_COUNT )
    {
        STL_TRY( ztcmReceiveData( &sCMMgr->mSenderContext,
                                  (stlChar*)&sTableCount,
                                  sRcvHeader.mLength,
                                  aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
    }
    
    STL_TRY_THROW( sTableCount > 0, RAMP_ERR_INVALID_COUNT );
    
    *aTableCount = sTableCount;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header(Req Table Count)" );
    }
    STL_CATCH( RAMP_ERR_INVALID_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "fail to compare table count" );
    }
    STL_FINISH;
    
    return STL_FAILURE;    
}


stlStatus ztcsRequestTableName( stlChar       * aData,
                                stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    
     /**
     * Send Request TableCount
     */
    sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
    sSndHeader.mType     = ZTC_HANDSHAKE_REQUEST_TABLE_NAME;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmReceiveProtocolHdr( &sCMMgr->mSenderContext,
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_RESPONSE_TABLE_NAME )
    {
        STL_TRY( ztcmReceiveData( &sCMMgr->mSenderContext,
                                  aData,
                                  sRcvHeader.mLength,
                                  aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sRcvHeader.mLength > 0, RAMP_ERR_INVALID_TABLE_NAME );
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header(Req Table Name)" );
    }
    STL_CATCH( RAMP_ERR_INVALID_TABLE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Table Name" );
    }
    STL_FINISH;
    
    return STL_FAILURE;   
}


stlStatus ztcsRequestCaptureInfo( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcCaptureInfo   sCaptureInfo;
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    
    /**
     * Send Request FileInfo
     */
    sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
    sSndHeader.mType     = ZTC_HANDSHAKE_REQUEST_CAPTURE_INFO;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    /**
     * Recv FileInfo
     */
    STL_TRY( ztcmReceiveProtocolHdr( &sCMMgr->mSenderContext,
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_RESPONSE_CAPTURE_INFO )
    {
        STL_TRY( ztcmReceiveData( &sCMMgr->mSenderContext,
                                  (stlChar*)&sCaptureInfo,
                                  STL_SIZEOF( ztcCaptureInfo ),
                                  aErrorStack ) == STL_SUCCESS );
        
        /**
         * VALID한 정보이면 Capture 위치를 재설정해야 한다.
         */
        if( sCaptureInfo.mLastCommitLSN != ZTC_INVALID_LSN &&
            sCaptureInfo.mFileSeqNo     != ZTC_INVALID_FILE_SEQ_NO &&
            sCaptureInfo.mWrapNo        != ZTC_INVALID_WRAP_NO )
        {
            STL_TRY( ztcmSetLogFileInfo( &sCaptureInfo,
                                         aErrorStack ) == STL_SUCCESS );
        }
        else
        {
            /**
             * 처음 시작한 경우에는 DB에서 현재 쓰고 있는 Redo Log위치 정보를 알아내야 한다.
             */
            STL_TRY_THROW( ztcmGetDbConnected() == STL_TRUE, RAMP_ERR_NO_RESTART_INFO );
            
            /**
             * 현재 Redo Log 정보 세팅
             */
            STL_TRY( ztcmSetCurrentLogFileInfo( aErrorStack ) == STL_SUCCESS );
        }
        
        /**
         * CAPTURE는 SYNC가 끝난 이후에 WAKEUP 한다.
         */
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );   
    }

    /**
     * Slave가 요청한 FileNo가 정상적인 FileNo인지를 확인한다.
     * 만약 생성되지 않은 FileNo를 요청할 경우에는 Slave와의 통신을 Give-up 해야 한다.
     */
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header(Req Capture Info)" );
    }
    STL_CATCH( RAMP_ERR_NO_RESTART_INFO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_NO_RESTART_INFO,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcsSendFlowProtocol( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr    sSndHeader;
    ztcProtocolHdr    sRcvHeader;
    
    ztcMasterCMMgr  * sCMMgr = &gMasterMgr->mCMMgr;
    
    stlInt32          sState      = 0;
    stlInt32          sReadSize   = 0;
    stlInt64          sBufferSize = ZTC_SENDER_BUFFER_SIZE;
    stlChar         * sBuffer     = NULL;
    
    STL_TRY( stlAllocHeap( (void**)&sBuffer, 
                           sBufferSize, 
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    stlMemset( sBuffer, 0x00, sBufferSize );
    
    while( 1 )
    {
        STL_TRY( ztcsReadDataFromChunk( sBuffer,
                                        &sReadSize,
                                        aErrorStack ) == STL_SUCCESS );
        
        /**
         * Send Data
         */
        sSndHeader.mCategory = ZTC_CATEGORY_DATA;
        sSndHeader.mType     = ZTC_DATA_SEND;
        sSndHeader.mLength   = sReadSize;
        
        STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                           sSndHeader,
                                           sBuffer,
                                           aErrorStack ) == STL_SUCCESS );
        
        //stlPrintf("SEND DATA [%d]\n", sSndHeader.mLength );
        
        /**
         * 정상적으로 받았는지 확인
         */
        STL_TRY( ztcmReceiveProtocolHdr( &sCMMgr->mSenderContext,
                                         &sRcvHeader,
                                         aErrorStack ) == STL_SUCCESS );
        
        if( sRcvHeader.mCategory == ZTC_CATEGORY_DATA )
        {
            switch( sRcvHeader.mType )
            {
                case ZTC_DATA_RECEIVE_OK:
                {
                    /** Nothing To do */
                    break;
                }
                default:
                {
                    STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
                    break;
                }
            }
        }
        else if( sRcvHeader.mCategory == ZTC_CATEGORY_CONTROL )
        {
            switch( sRcvHeader.mType )
            {
                case ZTC_CONTROL_STOP:
                {
                    STL_TRY( ztcmLogMsg( aErrorStack,
                                     "[SENDER] Slave Stopped normally.\n" ) == STL_SUCCESS );
                    
                    /**
                     * 굳이 FAIL로 처리할 필요는 없지만, CheckSlaveState와 동일하게 처리하기 위해서 FAIL로 처리한다.
                     */
                    STL_TRY( STL_FALSE );
                    break;
                }
                default:
                    break;
            }
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
        }
        
        STL_TRY( gRunState == STL_TRUE );
    }
    
    sState = 0;
    stlFreeHeap( sBuffer );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header" );
    }
    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            stlFreeHeap( sBuffer );
        default:
            break;
    }
    
    return STL_FAILURE; 
}


stlStatus ztcsEndProtocol( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    
    /**
     * Send Data
     */
    sSndHeader.mCategory = ZTC_CATEGORY_CONTROL;
    sSndHeader.mType     = ZTC_CONTROL_STOP;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


stlStatus ztcsCheckSlaveState( stlErrorStack * aErrorStack )
{
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    stlInt32         sSigFdNum;
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    stlInt32         sTryTimeout = ZTC_COMM_HEARTBEAT_TIMEOUT;
    stlInt32         sTryCount   = 0;
    
    sSndHeader.mCategory = ZTC_CATEGORY_CONTROL;
    sSndHeader.mType     = ZTC_CONTROL_HEARTBEAT;
    sSndHeader.mLength   = 0;
    
    while( 1 )
    {
        STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                           sSndHeader,
                                           NULL,
                                           aErrorStack ) == STL_SUCCESS );
        
        if( stlPoll( &sCMMgr->mSenderContext.mPollFd,
                     1,
                     &sSigFdNum,
                     STL_SET_SEC_TIME( 1 ),
                     aErrorStack ) != STL_SUCCESS )
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
            
            (void)stlPopError( aErrorStack );

            sTryCount++;
            if( sTryCount > sTryTimeout )
            {
/**
 * DEBUG Mode일 때는 Heartbeat을 체크하지 않도록 한다.
 */
#ifndef STL_DEBUG
                //HeartBeat 응답 없음...   
                STL_TRY( ztcmLogMsg( aErrorStack,
                                     "[SENDER] Slave not responding timeout.\n" ) == STL_SUCCESS );  
                
                STL_TRY( STL_FALSE );
#endif
            }

        }
        else
        {
            if( ztcmReceiveProtocolHdr( &sCMMgr->mSenderContext,
                                        &sRcvHeader,
                                        aErrorStack ) == STL_SUCCESS )
            {
                if( sRcvHeader.mCategory == ZTC_CATEGORY_CONTROL )
                {
                    if( sRcvHeader.mType == ZTC_CONTROL_STOP )
                    {
                        STL_TRY( ztcmLogMsg( aErrorStack,
                                             "[SENDER] Slave Stopped normally.\n" ) == STL_SUCCESS );
                        
                        STL_TRY( STL_FALSE );
                    }
                    else if( sRcvHeader.mType == ZTC_CONTROL_HEARTBEAT )
                    {
                        //Normal State... check Done..
                        break;
                    }
                }
                else if( sRcvHeader.mCategory == ZTC_CATEGORY_DATA &&
                         sRcvHeader.mType == ZTC_DATA_RECEIVE_OK )
                {
                    //Nothing To Do.
                }
                else
                {
                    STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
                }
            }
            else
            {
                if( stlGetLastErrorCode( aErrorStack ) == ZTC_ERRCODE_COMMUNICATION_LINK_FAILURE )
                {
                    (void)stlPopError( aErrorStack );
                    
                    STL_TRY( ztcmLogMsg( aErrorStack,
                                         "[SENDER] Slave stopped abnormally.\n" ) == STL_SUCCESS );
                }
                STL_TRY( STL_FALSE );
            }
        }
    } //end of while

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header at heartbeat" );
    }
    STL_FINISH;
    
    return STL_FAILURE;    
    
}

stlStatus ztcsSetSyncMode( stlBool         aStart,
                           stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    
    /**
     * Send Data
     */
    if( aStart == STL_TRUE )
    {
        sSndHeader.mCategory = ZTC_CATEGORY_CONTROL;
        sSndHeader.mType     = ZTC_CONTROL_SYNC_START;
        sSndHeader.mLength   = 0;
    }
    else
    {
        sSndHeader.mCategory = ZTC_CATEGORY_CONTROL;
        sSndHeader.mType     = ZTC_CONTROL_SYNC_DONE;
        sSndHeader.mLength   = 0;
    }
    
    STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    /**
     * SYNC START 일 경우에는 결과를 받아야 한다.
     */
    if( aStart == STL_TRUE )
    {
        STL_TRY( ztcmReceiveProtocolHdr( &sCMMgr->mSenderContext,
                                         &sRcvHeader,
                                         aErrorStack ) == STL_SUCCESS );
        
        if( sRcvHeader.mCategory == ZTC_CATEGORY_SYNC )
        {
            switch( sRcvHeader.mType )
            {
                case ZTC_SYNC_MODE_OK:
                {
                    /**
                     * Nothing To do.
                     */
                    break;
                }
                case ZTC_SYNC_MODE_NOT_OK:
                {
                    STL_THROW( RAMP_ERR_SLAVE_NOT_SYNC );
                    break;
                }
                default:
                    break;
            }
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
        }
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header" );
    }
    STL_CATCH( RAMP_ERR_SLAVE_NOT_SYNC )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_SLAVE_NOT_SYNC_MODE,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;    
}


stlStatus ztcsRequestSyncConnInfo( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcMasterCMMgr * sCMMgr  = &gMasterMgr->mCMMgr;
    stlInt32         sOffset = 0;
    stlInt32         sSize   = 0;
    stlInt32         sPort   = 0;
    stlInt32         sPropagateMode = 0;
    stlChar          sRcvData[STL_MAX_SQL_IDENTIFIER_LENGTH * 2];
    
    /**
     * Send Request FileInfo
     */
    sSndHeader.mCategory = ZTC_CATEGORY_SYNC;
    sSndHeader.mType     = ZTC_SYNC_REQUEST_CONN_INFO;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    /**
     * Recv FileInfo
     */
    STL_TRY( ztcmReceiveProtocolHdr( &sCMMgr->mSenderContext,
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_SYNC && 
        sRcvHeader.mType == ZTC_SYNC_RESPONSE_CONN_INFO )
    {
        STL_TRY( ztcmReceiveData( &sCMMgr->mSenderContext,
                                  sRcvData,
                                  sRcvHeader.mLength,
                                  aErrorStack ) == STL_SUCCESS );
        
        sOffset = 0;
        
        /**
         * READ Goldilocks Listen Port
         */
        STL_READ_MOVE_INT32( &sPort, sRcvData, sOffset );
        gMasterMgr->mSlaveGoldilocksListenPort = sPort;
        sOffset += STL_SIZEOF( stlInt32 );
        
        /**
         * READ Propate Mode
         */
        STL_READ_MOVE_INT32( &sPropagateMode, sRcvData, sOffset );
        gMasterMgr->mSlavePropagateMode = (stlBool)sPropagateMode;
        sOffset += STL_SIZEOF( stlInt32 );
        
        /**
         * READ USERID
         */
        STL_READ_MOVE_INT32( &sSize, sRcvData, sOffset );
        stlStrncpy( gMasterMgr->mSlaveUserId, sRcvData + sOffset, sSize + 1 );
        sOffset += sSize;
        
        /**
         * READ USERPW
         */
        STL_READ_MOVE_INT32( &sSize, sRcvData, sOffset );
        stlStrncpy( gMasterMgr->mSlaveUserPw, sRcvData + sOffset, sSize + 1 );
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header" );
    }
    STL_FINISH;
    
    return STL_FAILURE;   
}


stlStatus ztcsSyncProtocol( stlBool         aStart,
                            stlErrorStack * aErrorStack )
{
    if( aStart == STL_TRUE )
    {
        /**
         * 1. SYNC START
         * 2. REQUEST ID/PW
         */
        STL_TRY( ztcsSetSyncMode( STL_TRUE, 
                                  aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcsRequestSyncConnInfo( aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /**
         * SYNC END
         */
        STL_TRY( ztcsSetSyncMode( STL_FALSE, 
                                  aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}

stlStatus ztcsMonitorProtocol( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr    sSndHeader;
    ztcProtocolHdr    sRcvHeader;
    stlInt32          sSigFdNum;
    ztcMasterCMMgr  * sCMMgr = &gMasterMgr->mCMMgr;
    
    /**
     * Recv Request MonitorInfo.
     */
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );
        
        if( stlPoll( &sCMMgr->mMonitorContext.mPollFd,
                     1,
                     &sSigFdNum,
                     STL_SET_SEC_TIME( 1 ),
                     aErrorStack ) == STL_SUCCESS )
        {
            if( ztcmReceiveProtocolHdr( &sCMMgr->mMonitorContext,
                                        &sRcvHeader,
                                        aErrorStack ) == STL_SUCCESS )
            {
                if( sRcvHeader.mCategory == ZTC_CATEGORY_MONITOR )
                {
                    if( sRcvHeader.mType == ZTC_MONITOR_REQUEST_MONITOR_INFO )
                    {
                        // Send Monitor Info...
                        sSndHeader.mCategory = ZTC_CATEGORY_MONITOR;
                        sSndHeader.mType     = ZTC_MONITOR_RESPONSE_MONITOR_INFO;
                        sSndHeader.mLength   = STL_SIZEOF( ztcMonitorInfo );
                        
                        STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mMonitorContext,
                                                           sSndHeader,
                                                           (stlChar*)&gMonitorInfo,
                                                           aErrorStack ) == STL_SUCCESS );
                    }
                    else
                    {
                        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
                    }
                }
                else
                {
                    STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
                }
            }
            else
            {
                if( stlGetLastErrorCode( aErrorStack ) == ZTC_ERRCODE_COMMUNICATION_LINK_FAILURE )
                {
                    (void)stlPopError( aErrorStack );
                }
                //Link 끊겼을 경우...
                
                STL_TRY( STL_FALSE );
            }
        
        }
        else
        {
            //NOT RESPONSE...
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( aErrorStack );
        }
    }
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Monitor" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
