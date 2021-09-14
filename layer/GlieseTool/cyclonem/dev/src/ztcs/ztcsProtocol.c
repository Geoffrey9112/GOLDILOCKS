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

extern stlBool         gRunSyncState;
extern ztcMonitorInfo  gMonitorInfo;
extern stlInt64        gZtcmTrCnt;
extern stlInt64        gZtcmTrCntOld;

stlStatus ztcsDoProtocol( stlErrorStack * aErrorStack )
{
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;

    /**
     * 1. HandShake
     * 2. SYNC DATA ( if exist...)
     * 3. Data 전송
     */
//    stlDebug("before ztcsHandshakeProtocol ok \n");

    STL_TRY( ztcsHandshakeProtocol( aErrorStack ) == STL_SUCCESS );
//stlDebug("ztcsHandshakeProtocol ok \n");
    /**
     * Sync Mode일 경우 데이터 Sync를 먼저 수행한다.
     */
    if( sCMMgr->mClientType == 'S' )
    {
        gRunSyncState = STL_TRUE;

        STL_TRY( ztcsSyncProtocol( STL_TRUE, 
                                   aErrorStack ) == STL_SUCCESS );
        
//        STL_TRY( ztcnSetSyncStartPosition( aErrorStack ) == STL_SUCCESS );
//        stlDebug("ztcnSetSyncStartPosition ok \n");
        
        STL_TRY( ztcnSync( aErrorStack ) == STL_SUCCESS );
//        stlDebug("ztcnSync ok \n");
        
        STL_TRY( ztcsSyncProtocol( STL_FALSE, 
                                   aErrorStack ) == STL_SUCCESS );
//        stlDebug("ztcsSyncProtocol ok \n");
    }
    else
    {
        STL_TRY( ztcsSyncProtocol( STL_FALSE, 
                                   aErrorStack ) == STL_SUCCESS );
    }
//    stlDebug("ztcsSyncProtocol ok \n");
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcsDoPubProtocol( stlErrorStack * aErrorStack )
{
    ztcCaptureInfo  sCaptureInfo;
    stlSize         sLen;

    /**
     * 1. HandShake
     * 2. SYNC DATA ( if exist...)
     * 3. Data 전송
     */

    /**
     * 현재 Redo Log 정보 세팅
     */
    sLen = STL_SIZEOF(sCaptureInfo);
    if( gZtcLibFunc.mLoadData( gMasterMgr->mCMMgr.mPubContext,
                               &sCaptureInfo,
                               &sLen ) == ZTCL_SUCCESS )
    {
        STL_DASSERT( sLen == STL_SIZEOF(sCaptureInfo) );

        STL_TRY( ztcmSetLogFileInfo( &sCaptureInfo,
                                     aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /**
         * 처음 시작한 경우에는 DB에서 현재 쓰고 있는 Redo Log위치 정보를 알아내야 한다.
         */
        STL_TRY_THROW( ztcmGetDbConnected() == STL_TRUE, RAMP_ERR_NO_RESTART_INFO );

        STL_TRY( ztcmSetCurrentLogFileInfo( aErrorStack ) == STL_SUCCESS );
    }

    /**
     * Sync가 끝난 이후에 Capture 시작해야 한다.
     */
    STL_TRY( ztccWakeUpCapture( aErrorStack ) == STL_SUCCESS );

    /**
     * Sync가 끝난 이후에 Slave가 재접속하게 되면 위의 과정을 다시 수행하게 된다.
     * 즉, Slave 재접속할 때마다 Sync가 재수행되므로, 한번 Sync과정을 수행한 이후에는 Sync를 수행하지 않도록 해야한다.
     */
    while( gRunState == STL_TRUE )
    {
        stlSleep( STL_SET_MSEC_TIME(100) );

        if( gZtcPerf == STL_TRUE )
        {
            stlSleep( STL_SET_MSEC_TIME(900) );
            printf("send tr %ld \n", gZtcmTrCnt - gZtcmTrCntOld );
            gZtcmTrCntOld = gZtcmTrCnt;
        }
    }

    return STL_SUCCESS;


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
                                       &sSndHeader,
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

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcsSendLogByteOrder( stlErrorStack * aErrorStack )
{
    stlInt32   sLogByteOrder = STL_PLATFORM_ENDIAN;
    
    ztcProtocolHdr       sSndHeader;
    ztcMasterCMMgr     * sCMMgr     = &gMasterMgr->mCMMgr;
    
    /**
    * Send Request TableCount
    */
   sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
   sSndHeader.mType     = ZTC_HANDSHAKE_LOG_BYTE_ORDER;
   sSndHeader.mLength   = STL_SIZEOF( sLogByteOrder );

    /**
     * Request TableCount
     */
   sLogByteOrder = ztcToNet32( sLogByteOrder );
   STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                      &sSndHeader,
                                      (stlChar *)&sLogByteOrder,
                                      aErrorStack ) == STL_SUCCESS );

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
//stlDebug("ztcsRequestTableCount %d \n", sTableCount);
    for( sIdx = 0; sIdx < sTableCount; sIdx++ )
    {
        stlMemset( sData, 0x00, STL_SIZEOF( sData ) );
        
        STL_TRY( ztcsRequestTableName( sData,
                                       aErrorStack ) == STL_SUCCESS );
//        stlDebug("ztcsRequestTableName ok \n");
        sOffset = 0;
        
        /**
         * Read Master Schema
         */
        ZTC_READ32( &sSize, sData, sOffset );
        stlStrncpy( sMasterSchema, sData + sOffset, sSize + 1 );
        sOffset += sSize;
        
        /**
         * Read Master Table
         */
        ZTC_READ32( &sSize, sData, sOffset );
        stlStrncpy( sMasterTable, sData + sOffset, sSize + 1 );
        sOffset += sSize;
        
        /**
         * Read Slave Schema
         */
        ZTC_READ32( &sSize, sData, sOffset );
        stlStrncpy( sSlaveSchema, sData + sOffset, sSize + 1 );
        sOffset += sSize;
        
        /**
         * Read Slave Table
         */
        ZTC_READ32( &sSize, sData, sOffset );
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
                                               &sSndHeader,
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
//        stlDebug("ztcsResponseTableId ok \n");

        STL_TRY( ztcsCompareColumnInfo( sTableMeta,
                                        aErrorStack ) == STL_SUCCESS );
//        stlDebug("ztcsCompareColumnInfo ok \n");
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
    stlInt32         sTableId = ztcToNet32( aTableId );
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
                                           &sSndHeader,
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
    ztcColumnInfo * sColumnInfo;
    
    STL_TRY( ztcsResponseColumnCount( aTableMeta->mColumnCount,
                                      aErrorStack ) == STL_SUCCESS );
//    stlDebug("ztcsResponseColumnCount ok \n");

    STL_RING_FOREACH_ENTRY( &aTableMeta->mColumnMetaList, sColumnInfo )
    {
//        stlDebug("ztcsResponseColumnInfo mDataTypeId %ld \n", sColumnInfo->mDataTypeId);

        STL_TRY( ztcsResponseColumnInfo( sColumnInfo,
                                         aErrorStack ) == STL_SUCCESS );
//        stlDebug("ztcsResponseColumnInfo ok \n");
    }
    
    return STL_SUCCESS;   

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcsResponseColumnInfo( ztcColumnInfo         * aColumnInfo,
                                  stlErrorStack         * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;
    stlUInt8         sBuff[STL_SIZEOF( ztcColumnInfo )];
    
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mSenderContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_REQUEST_COLUMN_INFO )
    {
        sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
        sSndHeader.mType     = ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO;
        sSndHeader.mLength   = STL_SIZEOF( ztcColumnInfo );
        
        STL_TRY( ztcmWriteColumnInfo( aColumnInfo, sBuff, aErrorStack ) == STL_SUCCESS );

        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mSenderContext),
                                           &sSndHeader,
                                           (stlChar*)sBuff,
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
    stlInt32         sColumnCount = ztcToNet32( aColumnCount );
    
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
                                           &sSndHeader,
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
                                       &sSndHeader,
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

        sTableCount = ztcToHost32( sTableCount );
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
                                       &sSndHeader,
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




stlStatus ztcsSendEndProtocol( stlErrorStack * aErrorStack )
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
                                       &sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
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

//    stlDebug("send %d, %d \n", sSndHeader.mCategory, sSndHeader.mType);

    STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                       &sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
//    stlDebug("send ZTC_CATEGORY_CONTROL ZTC_CONTROL_SYNC_DONE \n");
    /**
     * SYNC START 일 경우에는 결과를 받아야 한다.
     */
    if( aStart == STL_TRUE )
    {
        STL_TRY( ztcmReceiveProtocolHdr( &sCMMgr->mSenderContext,
                                         &sRcvHeader,
                                         aErrorStack ) == STL_SUCCESS );
//        stlDebug("recv %d, %d \n", sRcvHeader.mCategory, sRcvHeader.mType);

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
//    stlDebug("ztcmSendProtocolHdrNData(ZTC_SYNC_REQUEST_CONN_INFO) \n");
    STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mSenderContext,
                                       &sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
//    stlDebug("ztcmSendProtocolHdrNData(ZTC_SYNC_REQUEST_CONN_INFO) ok \n");
    
    /**
     * Recv FileInfo
     */
//    stlDebug("ztcmReceiveProtocolHdr(ZTC_SYNC_RESPONSE_CONN_INFO) \n");
    STL_TRY( ztcmReceiveProtocolHdr( &sCMMgr->mSenderContext,
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
//    stlDebug("ztcmReceiveProtocolHdr ok\n");
    
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
        ZTC_READ32( &sPort, sRcvData, sOffset );
        gMasterMgr->mSlaveGoldilocksListenPort = sPort;
//        stlDebug("ztcmReceiveProtocolHdr sPort %d\n", sPort);

        /**
         * READ Propate Mode
         */
        STL_READ_MOVE_INT32( &sPropagateMode, sRcvData, sOffset );
        gMasterMgr->mSlavePropagateMode = (stlBool)sPropagateMode;
        sOffset += STL_SIZEOF( stlInt32 );

        /**
         * READ USERID
         */
        ZTC_READ32( &sSize, sRcvData, sOffset );
//        stlDebug("ztcmReceiveProtocolHdr size %d\n", sSize);

        stlStrncpy( gMasterMgr->mSlaveUserId, sRcvData + sOffset, sSize + 1 );
//        stlDebug("ztcmReceiveProtocolHdr id %s\n", gMasterMgr->mSlaveUserId);
        sOffset += sSize;
        
        /**
         * READ USERPW
         */
        ZTC_READ32( &sSize, sRcvData, sOffset );
        stlStrncpy( gMasterMgr->mSlaveUserPw, sRcvData + sOffset, sSize + 1 );

        /**
         * ip address
         */
//        stlDebug("ztcmGetPeerIPAddr 1\n");
        STL_TRY( ztcmGetPeerIPAddr( &sCMMgr->mSenderContext,
                                    gMasterMgr->mSlaveIp,
                                    STL_SIZEOF( gMasterMgr->mSlaveIp ),
                                    aErrorStack )
                 == STL_SUCCESS );
//        stlDebug("ztcmGetPeerIPAddr 2 %s\n", gMasterMgr->mSlaveIp);
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
//        stlDebug("ztcsSetSyncMode ok\n");
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
                                                           &sSndHeader,
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
