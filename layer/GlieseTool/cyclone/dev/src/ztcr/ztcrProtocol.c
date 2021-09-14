/*******************************************************************************
 * ztcrProtocol.c
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
 * @file ztcrProtocol.c
 * @brief GlieseTool Cyclone Receiver Protocol Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern stlBool        gRunState;
extern ztcSlaveMgr  * gSlaveMgr;

stlStatus ztcrDoProtocol( stlErrorStack * aErrorStack )
{
    stlInt64  sApplierCount = gSlaveMgr->mApplierCount;
    stlInt64  sIdx          = 0;
    
    STL_TRY( ztcrInitProtocol( aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcrHandshakeProtocol( aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcrSyncProtocol( aErrorStack ) == STL_SUCCESS );
    
    /**
     * HandShake가 끝났으면 Appliers 깨워준다.
     * HandShake가 끝난 이후에 Table 정보가 정상적으로 구성되기 때문에 이렇게 처리한다.
     */
    for( sIdx = 0; sIdx < sApplierCount; sIdx ++ )
    {
        STL_TRY( stlReleaseSemaphore( &(gSlaveMgr->mApplierStartSem),
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    if( ztcrReceiveFlowProtocol( aErrorStack ) != STL_SUCCESS )
    {
        if( gRunState == STL_FALSE )
        {
            STL_TRY( ztcrEndProtocol( aErrorStack ) == STL_SUCCESS );
        }
        
        STL_TRY( gRunState == STL_FALSE );
    }
    
    return STL_SUCCESS; 
      
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcrInitProtocol( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr  sSndHeader;
    ztcProtocolHdr  sRcvHeader;
    stlChar         sClientType;
    ztcSlaveCMMgr * sCMMgr      = &(gSlaveMgr->mCMMgr);
    
    /**
     * Recv Request ClientType
     */
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );

    if( sRcvHeader.mCategory == ZTC_CATEGORY_INIT && 
        sRcvHeader.mType     == ZTC_INIT_REQUEST_CLIENT_TYPE )
    {
        sClientType = 's';
                
        sSndHeader.mCategory = ZTC_CATEGORY_INIT;
        sSndHeader.mType     = ZTC_INIT_RESPONSE_CLIENT_TYPE;
        sSndHeader.mLength   = STL_SIZEOF( sClientType );
                
        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                           sSndHeader,
                                           (stlChar*)&sClientType,
                                           aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        sSndHeader.mCategory = ZTC_CATEGORY_ERROR;
        sSndHeader.mType     = ZTC_ERROR_PROTOCOL_ERROR;
        sSndHeader.mLength   = 0;
        
        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                           sSndHeader,
                                           NULL,
                                           aErrorStack ) == STL_SUCCESS );
        
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
    }
    
    /**
     * Recv Master State
     */
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_INIT &&
        sRcvHeader.mType     == ZTC_INIT_CLIENT_OK )
    {
        //Do Next Step...
    }
    else if( sRcvHeader.mCategory == ZTC_CATEGORY_ERROR && 
             sRcvHeader.mType     == ZTC_ERROR_SLAVE_ALREADY_CONNECTED )
    {
        STL_THROW( RAMP_ERR_SLAVE_ALREADY_CONNECTED );
    }
    else
    {
        sSndHeader.mCategory = ZTC_CATEGORY_ERROR;
        sSndHeader.mType     = ZTC_ERROR_PROTOCOL_ERROR;
        sSndHeader.mLength   = 0;
        
        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                           sSndHeader,
                                           NULL,
                                           aErrorStack ) == STL_SUCCESS );
        
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
    }
    
    return STL_SUCCESS; 
      
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "initialize" );
    }
    STL_CATCH( RAMP_ERR_SLAVE_ALREADY_CONNECTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_ALREADY_SLAVE_CONN,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcrHandshakeProtocol( stlErrorStack * aErrorStack )
{
    STL_TRY( ztcrResponseTableInfo( aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcrResponseCaptureInfo( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
       
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcrResponseTableInfo( stlErrorStack * aErrorStack )
{
    stlInt32        sTableCount  = gSlaveMgr->mTableCount;
    stlInt32        sColumnCount = 0;
    stlInt32        sLoop        = 0;
    stlInt32        sTableId     = 0;
    stlUInt32       sBucketSeq   = 0;
    stlInt32        sOffset      = 0;
    stlInt32        sSize        = 0;
    stlChar         sData[STL_MAX_SQL_IDENTIFIER_LENGTH * 4];
    
    ztcSlaveTableInfo     * sTableInfo      = NULL;
    ztcSlaveTableInfo     * sPrevTableInfo  = NULL;
    ztcColumnInfo         * sColumnInfo     = NULL;
    ztcProtocolColumnInfo   sProtoColInfo;
    
    /**
     * TABLE INFO를 비교하는 과정이다.
     * Master의 Table에 포함된 Column정보가 다를 경우에는 CDC를 포기한다.
     * 물론, Master와 Slave간에 Column DataType은 같고, Slave의 Size가 더 클 경우에는 
     * 간단하게 생각해서 정상적으로 문제없이 진행되어야 할 것 같지만..
     * CDC가 Log를 전송하므로 DT Layer에서 생각지 못한 오류를 발생할 수 있을 것이다. 
     * 예를 들어 Numeric(3,4) 를 전송해서 Numeric(4,5)에 넣을 경우 Log를 읽어서 적용할 경우 문제가 없는지 확인한 후에 가능한지 여부를 확인해 봐야 할 것이다.
     * 그렇다면 Datatype별로 비교해야할 대상이 달라질 수 있을 것이다. (Interval, Timestamp의 경우와 같이)
     */
    
    STL_TRY( ztcrResponseTableCount( aErrorStack ) == STL_SUCCESS );
    
    /**
     * Hash에 있는 Table 정보 비교
     * cyclone_slave_meta에 이미 Table이 존재할 경우
     */
    STL_TRY( ztcmGetFirstHashItem( gSlaveMgr->mTableHash, 
                                   &sBucketSeq, 
                                   (void**)&sTableInfo, 
                                   aErrorStack ) == STL_SUCCESS );
    
    while( sTableInfo != NULL )
    {
        sLoop++;
        stlMemset( sData, 0x00, STL_SIZEOF( sData ) );
        sOffset = 0;
        
        /**
         * Write Master Schema
         */
        sSize = stlStrlen( sTableInfo->mMasterSchema );
        STL_WRITE_MOVE_INT32( sData, &sSize, sOffset );
        stlMemcpy( sData + sOffset, sTableInfo->mMasterSchema, sSize );
        sOffset += sSize;
        
        /**
         * Write Master Table
         */
        sSize = stlStrlen( sTableInfo->mMasterTable );
        STL_WRITE_MOVE_INT32( sData, &sSize, sOffset );
        stlMemcpy( sData + sOffset, sTableInfo->mMasterTable, sSize );
        sOffset += sSize;
        
        /**
         * Write Slave Schema
         */
        sSize = stlStrlen( sTableInfo->mSlaveSchema );
        STL_WRITE_MOVE_INT32( sData, &sSize, sOffset );
        stlMemcpy( sData + sOffset, sTableInfo->mSlaveSchema, sSize );
        sOffset += sSize;
        
        /**
         * Write Slave Table
         */
        sSize = stlStrlen( sTableInfo->mSlaveTable );
        STL_WRITE_MOVE_INT32( sData, &sSize, sOffset );
        stlMemcpy( sData + sOffset, sTableInfo->mSlaveTable, sSize );
        sOffset += sSize;
        
        STL_TRY( ztcrResponseTableName( sData,
                                        sOffset,
                                        aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcrRequestTableId( &sTableId,
                                     aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sTableId == sTableInfo->mTableId, RAMP_ERR_INVALID_TABLE_ID );
        
        STL_TRY( ztcrRequestColumnCount( &sColumnCount,
                                         aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sColumnCount == sTableInfo->mColumnCount, RAMP_ERR_INVALID_COL_COUNT );
        
        STL_RING_FOREACH_ENTRY( &sTableInfo->mColumnList, sColumnInfo )
        {
            stlMemset( &sProtoColInfo, 0x00, STL_SIZEOF( ztcProtocolColumnInfo ) );
            
            STL_TRY( ztcrRequestColumnInfo( &sProtoColInfo,
                                            aErrorStack ) == STL_SUCCESS );
            /**
             * ColumnName은 달라도 되도록 한다.
             */
            //STL_TRY_THROW( stlStrcmp( sProtoColInfo.mColumnName, sColumnInfo->mColumnName ) == 0, RAMP_ERR_COLUMN_NAME );
            STL_TRY_THROW( stlStrcmp( sProtoColInfo.mDataType, sColumnInfo->mDataTypeStr ) == 0, RAMP_ERR_DATA_TYPE );
            STL_TRY_THROW( sProtoColInfo.mIsPrimary == sColumnInfo->mIsPrimary, RAMP_ERR_PRIMARY_KEY );
            STL_TRY_THROW( sProtoColInfo.mIsUnique == sColumnInfo->mIsUnique, RAMP_ERR_UNIQUE );
            STL_TRY_THROW( sProtoColInfo.mIsNullable == sColumnInfo->mIsNullable, RAMP_ERR_NULLABLE );
            
            STL_TRY( ztcrResponseColumnInfoResult( ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_SUCCESS, 
                                                   aErrorStack ) == STL_SUCCESS );
        }   
        
        STL_TRY( ztcmGetNextHashItem( gSlaveMgr->mTableHash, 
                                      &sBucketSeq, 
                                      (void**)&sTableInfo, 
                                      aErrorStack ) == STL_SUCCESS );
    }
    
    /**
     * List에 있는 Table 정보 비교
     * cyclone_slave_meta에 Table이 존재하지 않을 경우.
     */
    STL_RING_FOREACH_ENTRY_REVERSE_SAFE( &gSlaveMgr->mTableList, sTableInfo, sPrevTableInfo )
    {
        sLoop++;
        stlMemset( sData, 0x00, STL_SIZEOF( sData ) );
        sOffset = 0;
        
        /**
         * Write Master Schema
         */
        sSize = stlStrlen( sTableInfo->mMasterSchema );
        STL_WRITE_MOVE_INT32( sData, &sSize, sOffset );
        stlMemcpy( sData + sOffset, sTableInfo->mMasterSchema, sSize );
        sOffset += sSize;
        
        /**
         * Write Master Table
         */
        sSize = stlStrlen( sTableInfo->mMasterTable );
        STL_WRITE_MOVE_INT32( sData, &sSize, sOffset );
        stlMemcpy( sData + sOffset, sTableInfo->mMasterTable, sSize );
        sOffset += sSize;
        
        /**
         * Write Slave Schema
         */
        sSize = stlStrlen( sTableInfo->mSlaveSchema );
        STL_WRITE_MOVE_INT32( sData, &sSize, sOffset );
        stlMemcpy( sData + sOffset, sTableInfo->mSlaveSchema, sSize );
        sOffset += sSize;
        
        /**
         * Write Slave Table
         */
        sSize = stlStrlen( sTableInfo->mSlaveTable );
        STL_WRITE_MOVE_INT32( sData, &sSize, sOffset );
        stlMemcpy( sData + sOffset, sTableInfo->mSlaveTable, sSize );
        sOffset += sSize;
        
        STL_TRY( ztcrResponseTableName( sData,
                                        sOffset,
                                        aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztcrRequestTableId( &sTableId,
                                     aErrorStack ) == STL_SUCCESS );
        
        sTableInfo->mTableId = sTableId;
        
        STL_TRY( ztcrRequestColumnCount( &sColumnCount,
                                         aErrorStack ) == STL_SUCCESS );
        
        STL_TRY_THROW( sColumnCount == sTableInfo->mColumnCount, RAMP_ERR_INVALID_COL_COUNT );
        
        STL_RING_FOREACH_ENTRY( &sTableInfo->mColumnList, sColumnInfo )
        {
            STL_TRY( ztcrRequestColumnInfo( &sProtoColInfo,
                                            aErrorStack ) == STL_SUCCESS );
            
            /**
             * ColumnName은 달라도 되도록 한다.
             */
            //STL_TRY_THROW( stlStrcmp( sProtoColInfo.mColumnName, sColumnInfo->mColumnName ) == 0, RAMP_ERR_COLUMN_NAME );
            STL_TRY_THROW( stlStrcmp( sProtoColInfo.mDataType, sColumnInfo->mDataTypeStr ) == 0, RAMP_ERR_DATA_TYPE );
            STL_TRY_THROW( sProtoColInfo.mIsPrimary == sColumnInfo->mIsPrimary, RAMP_ERR_PRIMARY_KEY );
            STL_TRY_THROW( sProtoColInfo.mIsUnique == sColumnInfo->mIsUnique, RAMP_ERR_UNIQUE );
            STL_TRY_THROW( sProtoColInfo.mIsNullable == sColumnInfo->mIsNullable, RAMP_ERR_NULLABLE );
            
            STL_TRY( ztcrResponseColumnInfoResult( ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_SUCCESS, 
                                                   aErrorStack ) == STL_SUCCESS );
        }
        
        /**
         * Meta Insert & Hash Insert
         */
        STL_TRY( ztcdInsertTableSlaveMeta( gSlaveMgr->mDbcHandle,
                                           sTableInfo,
                                           aErrorStack ) == STL_SUCCESS );
        
        STL_RING_UNLINK( &gSlaveMgr->mTableList, sTableInfo );
        
        STL_TRY( ztcmInsertStaticHash( gSlaveMgr->mTableHash,
                                       ztcmSlaveTableHashFunc,
                                       sTableInfo,
                                       aErrorStack ) == STL_SUCCESS );
    }
    
    STL_TRY_THROW( sLoop == sTableCount, RAMP_ERR_INVALID_TABLE_COUNT );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_TABLE_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "table Count" );
    }
    STL_CATCH( RAMP_ERR_INVALID_TABLE_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "table id" );
    }
    STL_CATCH( RAMP_ERR_INVALID_COL_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "column Count" );
        
        (void) ztcrResponseColumnInfoResult( ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_FAIL, 
                                             aErrorStack );
    }
    /*
    STL_CATCH( RAMP_ERR_COLUMN_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "column Name" );
        
        (void) ztcrResponseColumnInfoResult( ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_FAIL, 
                                             aErrorStack );
    }
    */
    STL_CATCH( RAMP_ERR_DATA_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "column Datatype" );
        
        (void) ztcrResponseColumnInfoResult( ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_FAIL, 
                                             aErrorStack );
    }
    STL_CATCH( RAMP_ERR_PRIMARY_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Primary key constraint" );
        
        (void) ztcrResponseColumnInfoResult( ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_FAIL, 
                                             aErrorStack );
    }
    STL_CATCH( RAMP_ERR_UNIQUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Unique constraint" );
        
        (void) ztcrResponseColumnInfoResult( ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_FAIL, 
                                             aErrorStack );
    }
    STL_CATCH( RAMP_ERR_NULLABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "nullable constraint" );
        
        (void) ztcrResponseColumnInfoResult( ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO_FAIL, 
                                             aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcrRequestTableId( stlInt32      * aTableId,
                              stlErrorStack * aErrorStack )
{
    stlInt32         sTableId   = 0;
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcSlaveCMMgr  * sCMMgr      = &(gSlaveMgr->mCMMgr);
    
    sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
    sSndHeader.mType     = ZTC_HANDSHAKE_REQUEST_TABLE_ID;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_RESPONSE_TABLE_ID )
    {
        STL_TRY( ztcmReceiveData( &sCMMgr->mContext,
                                  (stlChar*)&sTableId,
                                  sRcvHeader.mLength,
                                  aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );   
    }
    
    *aTableId = sTableId;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header(Req TableId)" );
    }   
    STL_FINISH;
    
    return STL_FAILURE;  
}


stlStatus ztcrResponseColumnInfoResult( stlInt32        aResult,
                                        stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcSlaveCMMgr  * sCMMgr = &(gSlaveMgr->mCMMgr);
    
    
    sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
    sSndHeader.mType     = aResult;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                       sSndHeader,
                                       0,
                                       aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcrRequestColumnInfo( ztcProtocolColumnInfo * aColumnInfo,
                                 stlErrorStack         * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcSlaveCMMgr  * sCMMgr      = &(gSlaveMgr->mCMMgr);
    
    sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
    sSndHeader.mType     = ZTC_HANDSHAKE_REQUEST_COLUMN_INFO;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_RESPONSE_COLUMN_INFO )
    {
        STL_TRY( ztcmReceiveData( &sCMMgr->mContext,
                                  (stlChar*)aColumnInfo,
                                  sRcvHeader.mLength,
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
                      "Invalid Protocol Header(Req Column Info)" );
    }   
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcrRequestColumnCount( stlInt32      * aColumnCount,
                                  stlErrorStack * aErrorStack )
{
    stlInt32        sColumnCount = 0;
    
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcSlaveCMMgr  * sCMMgr      = &(gSlaveMgr->mCMMgr);
    
    sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
    sSndHeader.mType     = ZTC_HANDSHAKE_REQUEST_COLUMN_COUNT;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_RESPONSE_COLUMN_COUNT )
    {
        STL_TRY( ztcmReceiveData( &sCMMgr->mContext,
                                  (stlChar*)&sColumnCount,
                                  sRcvHeader.mLength,
                                  aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );   
    }
    
    *aColumnCount = sColumnCount;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header(Req Column Count)" );
    }   
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcrResponseTableCount( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    stlInt32         sTableCount = gSlaveMgr->mTableCount;
    ztcSlaveCMMgr  * sCMMgr      = &(gSlaveMgr->mCMMgr);

    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_REQUEST_TABLE_COUNT )
    {
        sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
        sSndHeader.mType     = ZTC_HANDSHAKE_RESPONSE_TABLE_COUNT;
        sSndHeader.mLength   = STL_SIZEOF( sTableCount );
                
        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                           sSndHeader,
                                           (stlChar*)&sTableCount,
                                           aErrorStack ) == STL_SUCCESS );
        
    }
    else
    {
        sSndHeader.mCategory = ZTC_CATEGORY_ERROR;
        sSndHeader.mType     = ZTC_ERROR_PROTOCOL_ERROR;
        sSndHeader.mLength   = 0;
        
        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                           sSndHeader,
                                           NULL,
                                           aErrorStack ) == STL_SUCCESS );
        
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );   
    }
    
    
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header(Res Table Count)" );
    }   
    STL_FINISH;
    
    return STL_FAILURE;
    
}


stlStatus ztcrResponseTableName( stlChar       * aData,
                                 stlInt32        aSize,
                                 stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcProtocolHdr   sRcvHeader;
    ztcSlaveCMMgr  * sCMMgr = &(gSlaveMgr->mCMMgr);
    
    /**
     * Recv Request TableName
     */
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_REQUEST_TABLE_NAME )
    {
        sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
        sSndHeader.mType     = ZTC_HANDSHAKE_RESPONSE_TABLE_NAME;
        sSndHeader.mLength   = aSize;
    
        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                           sSndHeader,
                                           aData,
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
                      "Invalid Protocol Header(Res Table Name)" );
    }   
    STL_FINISH;
    
    return STL_FAILURE;

}


stlStatus ztcrResponseCaptureInfo( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr    sSndHeader;
    ztcProtocolHdr    sRcvHeader;
    ztcCaptureInfo    sCaptureInfo;
    ztcSlaveCMMgr   * sCMMgr = &(gSlaveMgr->mCMMgr);
    
    /**
     * Recv Request FileInfo
     */
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_HANDSHAKE && 
        sRcvHeader.mType == ZTC_HANDSHAKE_REQUEST_CAPTURE_INFO )
    {
        sSndHeader.mCategory = ZTC_CATEGORY_HANDSHAKE;
        sSndHeader.mType     = ZTC_HANDSHAKE_RESPONSE_CAPTURE_INFO;
        sSndHeader.mLength   = STL_SIZEOF( ztcCaptureInfo );

        stlMemcpy( &sCaptureInfo, &gSlaveMgr->mRestartInfo, STL_SIZEOF( ztcCaptureInfo ) );
        
        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                           sSndHeader,
                                           (stlChar*)&sCaptureInfo,
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
                      "Invalid Protocol Header(Res Capture Info)" );
    }   
    STL_FINISH;
    
    return STL_FAILURE;
    
}


stlStatus ztcrReceiveFlowProtocol( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr  sSndHeader;
    ztcProtocolHdr  sRcvHeader;
    stlInt64        sBufferSize = ZTC_RECEIVER_BUFFER_SIZE;
    stlInt32        sState      = 0;
    
    ztcSlaveCMMgr * sCMMgr = &(gSlaveMgr->mCMMgr);
    
    stlChar       * sBuffer;
    
    STL_TRY( stlAllocHeap( (void**)&sBuffer,
                           sBufferSize,
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    while( 1 )
    {
        /**
         * Recv Request 
         */
        STL_TRY( ztcmReceiveProtocolHdrWithPoll( &(sCMMgr->mContext),
                                                 &sRcvHeader,
                                                 STL_TRUE,
                                                 aErrorStack ) == STL_SUCCESS );
        
        if( sRcvHeader.mCategory == ZTC_CATEGORY_DATA )
        {
            switch( sRcvHeader.mType )
            {
                case ZTC_DATA_SEND:
                {
                    STL_TRY( ztcmReceiveData( &(sCMMgr->mContext),
                                              sBuffer,
                                              sRcvHeader.mLength,
                                              aErrorStack ) == STL_SUCCESS );

                    //stlPrintf("RCV DATA[%d]\n", sRcvHeader.mLength );
                    
                    STL_TRY( ztcrWriteDataToChunk( sBuffer, 
                                                   sRcvHeader.mLength,
                                                   aErrorStack ) == STL_SUCCESS );
                    
                    sSndHeader.mCategory = ZTC_CATEGORY_DATA;
                    sSndHeader.mType     = ZTC_DATA_RECEIVE_OK;
                    sSndHeader.mLength   = 0;
                    
                    STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                                       sSndHeader,
                                                       NULL,
                                                       aErrorStack ) == STL_SUCCESS );
                    break;
                }
                default:
                {
                    STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
                    break;
                }
            }
        }
        else if(sRcvHeader.mCategory == ZTC_CATEGORY_CONTROL )
        {
            switch( sRcvHeader.mType )
            {
                case ZTC_CONTROL_STOP:
                {
                    //Master가 종료했을때..   
                    STL_TRY( ztcmLogMsg( aErrorStack,
                                         "[RECEIVER] Master Stopped normally.\n" ) == STL_SUCCESS );
                    STL_THROW( RAMP_STOP );
                    break;
                }
                case ZTC_CONTROL_HEARTBEAT:
                {
                    sSndHeader.mCategory = ZTC_CATEGORY_CONTROL;
                    sSndHeader.mType     = ZTC_CONTROL_HEARTBEAT;
                    sSndHeader.mLength   = 0;
                    
                    STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                                       sSndHeader,
                                                       NULL,
                                                       aErrorStack ) == STL_SUCCESS );
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

    STL_RAMP( RAMP_STOP );
    
    //이거 해야하나?
    gRunState = STL_FALSE;
    
    sState = 0;
    stlFreeHeap( sBuffer );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "Invalid Protocol Header(Rcv Flow)" );
    }
    STL_FINISH;
    
    if( sState > 0 )
    {
        stlFreeHeap( sBuffer );
    }
    
    return STL_FAILURE;
}


stlStatus ztcrEndProtocol( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr   sSndHeader;
    ztcSlaveCMMgr  * sCMMgr = &(gSlaveMgr->mCMMgr);
    
    /**
     * Send Data
     */
    sSndHeader.mCategory = ZTC_CATEGORY_CONTROL;
    sSndHeader.mType     = ZTC_CONTROL_STOP;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mContext,
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}


stlStatus ztcrResponseSyncConnInfo( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr    sSndHeader;
    ztcProtocolHdr    sRcvHeader;
    ztcSlaveCMMgr   * sCMMgr = &(gSlaveMgr->mCMMgr);
    stlChar           sData[STL_MAX_SQL_IDENTIFIER_LENGTH * 2];
    stlInt32          sOffset = 0;
    stlInt32          sSize   = 0;
    stlInt32          sPort   = gSlaveMgr->mGoldilocksListenPort;
    stlInt32          sPropagateMode = (stlInt32)ztcmGetPropagateMode();
    stlChar           sID[ STL_MAX_SQL_IDENTIFIER_LENGTH ];
    stlChar           sPW[ STL_MAX_SQL_IDENTIFIER_LENGTH ];
    
    /**
     * Recv Request FileInfo
     */
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_SYNC && 
        sRcvHeader.mType == ZTC_SYNC_REQUEST_CONN_INFO )
    {
        
        STL_TRY( ztcmGetUserId( sID, aErrorStack ) == STL_SUCCESS );
        STL_TRY( ztcmGetUserPw( sPW, aErrorStack ) == STL_SUCCESS );
        
        /**
         * Write Goldilocks Listen Port
         */
        sSize = STL_SIZEOF( stlInt32 );
        STL_WRITE_MOVE_INT32( sData, &sPort, sOffset );
        sOffset += sSize;
        
        /**
         * Write Propate Mode
         */
        sSize = STL_SIZEOF( stlInt32 );
        STL_WRITE_MOVE_INT32( sData, &sPropagateMode, sOffset );
        sOffset += sSize;
        
        /**
         * Write UserID
         */
        sSize = stlStrlen( sID );
        STL_WRITE_MOVE_INT32( sData, &sSize, sOffset );
        stlMemcpy( sData + sOffset, sID, sSize );
        sOffset += sSize;
        
        /**
         * Write UserPW
         */
        sSize = stlStrlen( sPW );
        STL_WRITE_MOVE_INT32( sData, &sSize, sOffset );
        stlMemcpy( sData + sOffset, sPW, sSize );
        sOffset += sSize;
        
        sSndHeader.mCategory = ZTC_CATEGORY_SYNC;
        sSndHeader.mType     = ZTC_SYNC_RESPONSE_CONN_INFO;
        sSndHeader.mLength   = sOffset;
        
        STL_TRY( ztcmSendProtocolHdrNData( &(sCMMgr->mContext),
                                           sSndHeader,
                                           sData,
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
                      "Invalid Protocol Header(Res Sync Conn Info)" );
    }   
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcrSyncProtocol( stlErrorStack * aErrorStack )
{
    ztcProtocolHdr    sSndHeader;
    ztcProtocolHdr    sRcvHeader;
    ztcSlaveCMMgr   * sCMMgr = &(gSlaveMgr->mCMMgr);
    stlBool           sSyncWait = STL_FALSE;
    
    /**
     * Recv Request FileInfo
     */
    STL_TRY( ztcmReceiveProtocolHdr( &(sCMMgr->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTC_CATEGORY_CONTROL )
    {
        switch( sRcvHeader.mType )
        {
            case ZTC_CONTROL_SYNC_START:
            {
                STL_TRY( ztcmLogMsg( aErrorStack,
                                     "[RECEIVER] start the data synchronization.\n" ) == STL_SUCCESS );
                sSyncWait = STL_TRUE;
                
                if( ztcmGetSyncMode() != STL_TRUE )
                {
                    sSndHeader.mCategory = ZTC_CATEGORY_SYNC;
                    sSndHeader.mType     = ZTC_SYNC_MODE_NOT_OK;
                    sSndHeader.mLength   = 0;
                }
                else
                {
                    sSndHeader.mCategory = ZTC_CATEGORY_SYNC;
                    sSndHeader.mType     = ZTC_SYNC_MODE_OK;
                    sSndHeader.mLength   = 0;
                }
                
                STL_TRY( ztcmSendProtocolHdrNData( &sCMMgr->mContext,
                                                   sSndHeader,
                                                   NULL,
                                                   aErrorStack ) == STL_SUCCESS );
                
                STL_TRY_THROW( ztcmGetSyncMode() == STL_TRUE, RAMP_ERR_NOT_SYNC_MODE );
                
                STL_TRY( ztcrResponseSyncConnInfo( aErrorStack ) == STL_SUCCESS );
                
                break;
            }
            case ZTC_CONTROL_SYNC_DONE:
            {
                //Nothing To Do...
                sSyncWait = STL_FALSE;
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
    
    if( sSyncWait == STL_TRUE )
    {
        /**
         * Sync 완료 메시지를 대기한다.
         */
        STL_TRY( ztcmReceiveProtocolHdrWithPoll( &(sCMMgr->mContext),
                                                 &sRcvHeader,
                                                 STL_FALSE,
                                                 aErrorStack ) == STL_SUCCESS );
        
        if( sRcvHeader.mCategory == ZTC_CATEGORY_CONTROL )
        {
            switch( sRcvHeader.mType )
            {
                case ZTC_CONTROL_SYNC_DONE:
                {
                    STL_TRY( ztcmLogMsg( aErrorStack,
                                         "[RECEIVER] data synchronization done.\n" ) == STL_SUCCESS );
                    break;
                }
                case ZTC_CONTROL_STOP:
                {
                    //Master가 종료했을때..   
                    //이 경우가 올 수 있나??
                    STL_TRY( ztcmLogMsg( aErrorStack,
                                         "[RECEIVER] Master Stopped normally.\n" ) == STL_SUCCESS );
                    
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
                      "Invalid Protocol Header(Sync)" );
    }
    STL_CATCH( RAMP_ERR_NOT_SYNC_MODE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_SLAVE_NOT_SYNC_MODE,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
