/*******************************************************************************
 * ztnmMonitor.c
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
 * @file ztnmMonitor.c
 * @brief GlieseTool Cyclone Monitor Routines
 */

#include <goldilocks.h>
#include <ztn.h>

extern ztnManager   gZtnManager;
extern ztnConfigure gZtnConfigure;
extern stlBool      gZtnRunState;

stlStatus ztnmDoMonitor( stlErrorStack * aErrorStack )
{
    stlBool     sConn  = STL_FALSE;
    stlInt16    sGroupId;
    stlInt64    sFileSeqNo;
    stlInt32    sBlockSeq;
    stlInt64    sCycle = ztnmGetMonitorCycle();
    
    while( 1 )
    {
        STL_INIT_ERROR_STACK( aErrorStack );

        if( sConn != STL_TRUE )
        {
            if( ztnmCreateConnection( &gZtnManager.mDbcHandle,
                                      gZtnManager.mEnvHandle,
                                      aErrorStack ) == STL_SUCCESS )
            {
                sConn = STL_TRUE;
                
                STL_TRY( ztnmSQLAllocStmt( gZtnManager.mDbcHandle,
                                           &gZtnManager.mUpdStmtHandle,
                                           aErrorStack ) == STL_SUCCESS );
    
                
                STL_TRY( ztnmCheckTableExist( gZtnManager.mDbcHandle,
                                              aErrorStack ) == STL_SUCCESS );
                
                STL_TRY( ztnmBuildLogFileInfo( gZtnManager.mDbcHandle,
                                               aErrorStack ) == STL_SUCCESS );
                
                STL_TRY( ztnmInsertBlankMonitorInfo( gZtnManager.mDbcHandle,
                                                     aErrorStack ) == STL_SUCCESS );
                
                STL_TRY( ztnmInitUpdateMonitorStmt( gZtnManager.mDbcHandle,
                                                    gZtnManager.mUpdStmtHandle,
                                                    &gZtnManager.mParameter,
                                                    aErrorStack ) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( stlGetLastErrorCode( aErrorStack ) != ZTN_ERRCODE_INVALID_CONN_INFO );
                sConn = STL_FALSE;

            }
        }
        else
        {
            if( ztnmGetDbStatus( gZtnManager.mDbcHandle,
                                 &sGroupId,
                                 &sFileSeqNo,
                                 &sBlockSeq,
                                 aErrorStack ) == STL_SUCCESS )
            {
                STL_TRY( ztnmGetCycloneMonitorInfo( aErrorStack ) == STL_SUCCESS );
                
                //UPDATE Status
                STL_TRY( ztnmUpdateMonitorInfo( gZtnManager.mDbcHandle,
                                                gZtnManager.mUpdStmtHandle,
                                                &gZtnManager.mParameter,
                                                sGroupId,
                                                sFileSeqNo,
                                                sBlockSeq,
                                                aErrorStack ) == STL_SUCCESS );
            }
            else
            {
                /**
                 * 에러가 발생하면 Connection 끊고 다시 접속하도록 함.
                 */
                (void) ztnmSQLFreeStmt( gZtnManager.mUpdStmtHandle,
                                        aErrorStack );
                
                (void) ztnmDestroyConnection( gZtnManager.mDbcHandle,
                                              aErrorStack );
                
                sConn = STL_FALSE;
            }
        }
        
        if( gZtnRunState == STL_FALSE )
        {
            break;
        }
        
        stlSleep( STL_SET_SEC_TIME( sCycle ) );
    }
    
    if( sConn == STL_TRUE )
    {
        sConn = STL_FALSE;
        
        STL_TRY( ztnmSQLFreeStmt( gZtnManager.mUpdStmtHandle,
                                  aErrorStack ) == STL_SUCCESS );
        
        STL_TRY( ztnmDestroyConnection( gZtnManager.mDbcHandle,
                                        aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;   
    
    STL_FINISH;
    
    if( sConn == STL_TRUE )
    {
        (void) ztnmSQLFreeStmt( gZtnManager.mUpdStmtHandle,
                                aErrorStack );
        
        (void)ztnmDestroyConnection( gZtnManager.mDbcHandle,
                                     aErrorStack );
    }
    
    return STL_FAILURE;
}

stlStatus ztnmGetCycloneMonitorInfo( stlErrorStack * aErrorStack )
{
    ztnGroupItem * sGroup;
    
    STL_RING_FOREACH_ENTRY( &gZtnManager.mGroupList, sGroup )
    {
        if( sGroup->mIsConnect == STL_FALSE )
        {
            /**
             * 지속적인 모니터링을 하므로 Cyclone이 종료되거나 오류가 발생해도 Monitor는 지속적으로 운영하도록한다.
             */
            if( ztnmConnectToCyclone( sGroup, 
                                      aErrorStack ) == STL_SUCCESS )
            {
                if( ztnmInitProtocol( sGroup,
                                      aErrorStack ) == STL_SUCCESS )
                {
                    sGroup->mIsConnect = STL_TRUE;
                }
                else
                {
                    sGroup->mIsConnect = STL_FALSE;   
                }
            }
            else
            {
                STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_CONNECT ||
                         stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );

                (void)stlPopError( aErrorStack );
                continue;
            }
        }
        
        if( ztnmDoProtocol( sGroup, aErrorStack ) != STL_SUCCESS )
        {
            /**
             * Failure상황에서는 연결이 끊긴 상황이여야만 한다.
             */
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == ZTN_ERRCODE_COMMUNICATION_LINK_FAILURE );
            
            (void)stlPopError( aErrorStack );
            
            sGroup->mIsConnect = STL_FALSE;
        }
    }
    
    return STL_SUCCESS;  
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztnmDoProtocol( ztnGroupItem  * aGroup,
                          stlErrorStack * aErrorStack )
{
    STL_TRY( ztnmMonitorProtocol( aGroup,
                                  aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;   
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztnmInitProtocol( ztnGroupItem  * aGroup,
                            stlErrorStack * aErrorStack )
{
    ztnProtocolHdr  sSndHeader;
    ztnProtocolHdr  sRcvHeader;
    stlChar         sClientType;
    
    /**
     * Recv Request ClientType
     */
    STL_TRY( ztnmReceiveProtocolHdr( &(aGroup->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );

    if( sRcvHeader.mCategory == ZTN_CATEGORY_INIT && 
        sRcvHeader.mType     == ZTN_INIT_REQUEST_CLIENT_TYPE )
    {
        sClientType = 'm';
                
        sSndHeader.mCategory = ZTN_CATEGORY_INIT;
        sSndHeader.mType     = ZTN_INIT_RESPONSE_CLIENT_TYPE;
        sSndHeader.mLength   = STL_SIZEOF( sClientType );
                
        STL_TRY( ztnmSendProtocolHdrNData( &(aGroup->mContext),
                                           sSndHeader,
                                           (stlChar*)&sClientType,
                                           aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        sSndHeader.mCategory = ZTN_CATEGORY_ERROR;
        sSndHeader.mType     = ZTN_ERROR_PROTOCOL_ERROR;
        sSndHeader.mLength   = 0;
        
        STL_TRY( ztnmSendProtocolHdrNData( &(aGroup->mContext),
                                           sSndHeader,
                                           NULL,
                                           aErrorStack ) == STL_SUCCESS );
        
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
    }
    
    /**
     * Recv Master State
     */
    STL_TRY( ztnmReceiveProtocolHdr( &(aGroup->mContext),
                                     &sRcvHeader,
                                     aErrorStack ) == STL_SUCCESS );
    
    if( sRcvHeader.mCategory == ZTN_CATEGORY_INIT &&
        sRcvHeader.mType     == ZTN_INIT_CLIENT_OK )
    {
        //Do Next Step...
    }
    else if( sRcvHeader.mCategory == ZTN_CATEGORY_ERROR && 
             sRcvHeader.mType     == ZTN_ERROR_SLAVE_ALREADY_CONNECTED )
    {
        STL_THROW( RAMP_ERR_ALREADY_CONNECTED );
    }
    else
    {
        sSndHeader.mCategory = ZTN_CATEGORY_ERROR;
        sSndHeader.mType     = ZTN_ERROR_PROTOCOL_ERROR;
        sSndHeader.mLength   = 0;
        
        STL_TRY( ztnmSendProtocolHdrNData( &(aGroup->mContext),
                                           sSndHeader,
                                           NULL,
                                           aErrorStack ) == STL_SUCCESS );
        
        STL_THROW( RAMP_ERR_INVALID_PROTOCOL );
    }
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_PROTOCOL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_PROTOCOL,
                      NULL,
                      aErrorStack,
                      "initialize" );
    }
    STL_CATCH( RAMP_ERR_ALREADY_CONNECTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTN_ERRCODE_ALREADY_CONN,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;
    
    /**
     * INIT PROTOCOL 실패시 연결을 끊는다...
     */
    (void) stlFinalizeContext( &(aGroup->mContext),
                               aErrorStack );
    
    return STL_FAILURE;
}

stlStatus ztnmMonitorProtocol( ztnGroupItem  * aGroup,
                               stlErrorStack * aErrorStack )
{
    ztnProtocolHdr  sSndHeader;
    ztnProtocolHdr  sRcvHeader;
    stlInt32        sSigFdNum;
    ztnMonitorInfo  sMonitorInfo;
    /**
     * Send Request MonitorInfo.
     */
    sSndHeader.mCategory = ZTN_CATEGORY_MONITOR;
    sSndHeader.mType     = ZTN_MONITOR_REQUEST_MONITOR_INFO;
    sSndHeader.mLength   = 0;
    
    STL_TRY( ztnmSendProtocolHdrNData( &(aGroup->mContext),
                                       sSndHeader,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
    
    /**
     * Recv Response MonitorInfo
     * 1초간 응답대기
     */
    if( stlPoll( &aGroup->mContext.mPollFd,
                 1,
                 &sSigFdNum,
                 STL_SET_SEC_TIME( 1 ),
                 aErrorStack ) == STL_SUCCESS )
    {
        if( ztnmReceiveProtocolHdr( &(aGroup->mContext),
                                    &sRcvHeader,
                                    aErrorStack ) == STL_SUCCESS )
        {
            if( sRcvHeader.mCategory == ZTN_CATEGORY_MONITOR )
            {
                switch( sRcvHeader.mType )
                {
                    case ZTN_MONITOR_RESPONSE_MONITOR_INFO:
                    {
                        STL_TRY( ztnmReceiveData( &(aGroup->mContext),
                                                  (stlChar*)&sMonitorInfo,
                                                  STL_SIZEOF( ztnMonitorInfo ),
                                                  aErrorStack ) == STL_SUCCESS );
                        
                        stlMemcpy( (stlChar*)&aGroup->mMonitorInfo,
                                   (stlChar*)&sMonitorInfo,
                                   STL_SIZEOF( ztnMonitorInfo ) );
                        break;
                    }
                    default:
                        break;
                }
            }
            
        }
        else
        {
            /**
             * 실패로 처리해야 한다.
             */
            STL_TRY( STL_FALSE );
        }
    }
    else
    {
        STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
        (void)stlPopError( aErrorStack );
    }
    
    return STL_SUCCESS;   
    
    STL_FINISH;
    
    (void) stlFinalizeContext( &(aGroup->mContext),
                               aErrorStack );
    
    return STL_FAILURE;
}

/** @} */
