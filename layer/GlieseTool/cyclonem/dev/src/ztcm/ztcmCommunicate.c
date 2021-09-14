/*******************************************************************************
 * ztcmCommunicate.c
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
 * @file ztcmCommunicate.c
 * @brief GlieseTool Cyclone Communicate Routines
 */

#include <goldilocks.h>
#include <ztcl.h>
#include <ztc.h>

stlInt64 gZtcmTrCnt = 0;
stlInt64 gZtcmTrCntOld = 0;

stlStatus ztcmSendProtocolHdrNData( stlContext     * aContext,
                                    ztcProtocolHdr * aHeader,
                                    stlChar        * aData,
                                    stlErrorStack  * aErrorStack )
{
    stlUInt8 sBuff[STL_SIZEOF(ztcProtocolHdr)];
    stlInt32 sEndian = STL_BIG_ENDIAN;

    /**
     * TODO lym endian
     * cymon(monitor) context는 endian을 network endian이 아닌 platfrom endian으로 한다.
     * ZTC_INIT_REQUEST_CLIENT_TYPE는 STL_PLATFORM_ENDIAN으로 보낸다.
     */
    if( gMasterMgr != NULL )
    {
        if( (&gMasterMgr->mCMMgr.mMonitorContext == aContext) ||
            (aHeader->mType == ZTC_INIT_REQUEST_CLIENT_TYPE) )
        {
            sEndian = STL_PLATFORM_ENDIAN;
        }
    }

    STL_TRY( ztcmWriteProtocolHdr( aHeader, sEndian, sBuff, aErrorStack ) == STL_SUCCESS );
    STL_TRY( ztcmSendData( aContext,
                           (stlChar*)&sBuff,
                           STL_SIZEOF( ztcProtocolHdr ),
                           aErrorStack ) == STL_SUCCESS );
    
    if( aHeader->mLength > 0 )
    {
        STL_DASSERT( aData != NULL );
        
        STL_TRY( ztcmSendData( aContext,
                               aData,
                               aHeader->mLength,
                               aErrorStack ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
       
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcmReceiveProtocolHdrWithPoll( stlContext     * aContext,
                                          ztcProtocolHdr * aHeader,
                                          stlBool          aTimeoutCheck,
                                          stlErrorStack  * aErrorStack )
{
    stlUInt8        sBuff[STL_SIZEOF(ztcProtocolHdr)];
    stlInt32        sSigFdNum;
    stlInt32        sTryTimeout = ZTC_COMM_HEARTBEAT_TIMEOUT;
    stlInt32        sTryCount   = 0;
    stlInt32        sEndian = STL_BIG_ENDIAN;

    /**
     * cymon(monitor) context는 endian을 network endian이 아닌 platfrom endian으로 한다.
     */
    if( gMasterMgr != NULL )
    {
        if( &gMasterMgr->mCMMgr.mMonitorContext == aContext )
        {
            sEndian = STL_PLATFORM_ENDIAN;
        }
    }

    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );

        if( stlPoll( &aContext->mPollFd,
                     1,
                     &sSigFdNum,
                     STL_SET_SEC_TIME( 1 ),
                     aErrorStack ) == STL_SUCCESS )
        {
            break;
        }
        else
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_TIMEDOUT );
            (void)stlPopError( aErrorStack );
            
            STL_TRY( gRunState == STL_TRUE );
            
            /**
             * Timeout을 체크할 경우에만 ...
             */
            if( aTimeoutCheck == STL_TRUE )
            {
                sTryCount++;
            
                if( sTryCount > sTryTimeout )
                {
/**
 * DEBUG Mode일 때는 Heartbeat을 체크하지 않도록 한다.
 */
#ifndef STL_DEBUG
                    STL_TRY( ztcmLogMsg( aErrorStack,
                                         "[RECEIVER] Master not responding timeout.(Heartbeat)\n" ) == STL_SUCCESS );
                    STL_TRY( STL_FALSE );
#endif
                }
            }

        }
    }

    STL_TRY( ztcmReceiveData( aContext, 
                              (stlChar*)&sBuff,
                              STL_SIZEOF( ztcProtocolHdr ),
                              aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmReadProtocolHdr( sBuff, sEndian, aHeader, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
        
}



stlStatus ztcmReceiveProtocolHdr( stlContext     * aContext,
                                  ztcProtocolHdr * aHeader,
                                  stlErrorStack  * aErrorStack )
{
    stlUInt8   sBuff[STL_SIZEOF(ztcProtocolHdr)];
    stlInt32   sEndian = STL_BIG_ENDIAN;

    /**
     * TODO lym
     * cymon(monitor) context는 endian을 network endian이 아닌 platfrom endian으로 한다.
     */
    if( gMasterMgr != NULL )
    {
        if( &gMasterMgr->mCMMgr.mMonitorContext == aContext )
        {
            sEndian = STL_PLATFORM_ENDIAN;
        }
    }

    STL_TRY( ztcmReceiveData( aContext, 
                              (stlChar*)&sBuff,
                              STL_SIZEOF( ztcProtocolHdr ),
                              aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( ztcmReadProtocolHdr( sBuff, sEndian, aHeader, aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmSendData( stlContext    * aContext,
                        stlChar       * aData,
                        stlInt32        aSize,
                        stlErrorStack * aErrorStack )
{
    stlSize sSentSize = 0;
    stlSize sSize;

    while( sSentSize < aSize )
    {
        sSize = aSize - sSentSize;
        
        if( stlSend( STL_SOCKET_IN_CONTEXT( *aContext ),
                     aData + sSentSize,
                     &sSize,
                     aErrorStack ) == STL_FAILURE )
        {
            STL_TRY_THROW( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_AGAIN,
                           RAMP_ERR_COMMUNICATION_LINK_FAILURE );
            (void)stlPopError( aErrorStack );
        }
        else
        {
            /* send를 성공했을 때만 sSentSize를 더해야 한다. */
            sSentSize += sSize;
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcmPubData( ztcMasterCMMgr     * aCMMgr,
                       ztclPublisher      * aContext,
                       stlChar            * aData,
                       stlSize              aSize,
                       stlInt32           * aTrSeq,
                       stlErrorStack      * aErrorStack )
{
    ztcTransHdr * sHeader;

#if 0
    stlInt32      sEnd = 0;
//    stlDebug("mPubData %ld, %s \n", aSize, aDataType);
    sHeader = (ztcTransHdr *)aData;
    if( sHeader->mLogType == ZTC_CAPTURE_TYPE_COMMIT )
    {
        sEnd = 1;
    }
    else
    {
        sEnd = 0;
    }
    STL_TRY_THROW( gZtcLibFunc.mPubData( aContext,
                                         aData,
                                         aSize,
                                         *aTrSeq,
                                         sEnd )
                   == ZTCL_SUCCESS,
                   RAMP_ERROR_PUB );
    (*aTrSeq)++;

    return STL_SUCCESS;
#else
    sHeader = (ztcTransHdr *)aData;

//    printf("send log type %d \n", sHeader->mLogType);
    if( sHeader->mLogType == ZTC_CAPTURE_TYPE_DELETE )
    {
        /**
         * Capture에서 Undo를 위해서 저장한 데이터이며 분석할 필요 없음
         * 실제 분석할 내용은 Delete Supplemental Log이다.
         */
        STL_THROW( RAMP_SUCCESS );
    }

    if( aCMMgr->mSendBufLen + aSize >= ZTC_MAX_SEND_BUFF_SIZE )
    {
        STL_DASSERT( aCMMgr->mSendBufLen != 0 );
        STL_TRY_THROW( gZtcLibFunc.mPubData( aContext,
                                             aCMMgr->mSendBuf,
                                             aCMMgr->mSendBufLen,
                                             *aTrSeq,
                                             0 )
                       == ZTCL_SUCCESS,
                       RAMP_ERROR_PUB );
        (*aTrSeq)++;

        aCMMgr->mSendBufLen = 0;
    }

    STL_DASSERT( aCMMgr->mSendBufLen + aSize <= ZTC_MAX_SEND_BUFF_SIZE );

    if( sHeader->mLogType == ZTC_CAPTURE_TYPE_COMMIT )
    {
        stlMemcpy( &aCMMgr->mSendBuf[aCMMgr->mSendBufLen], aData, aSize );
        sHeader = (ztcTransHdr *)&aCMMgr->mSendBuf[aCMMgr->mSendBufLen];
        sHeader->mMsgId = STL_UINT64_MAX;

        aCMMgr->mSendBufLen += aSize;

        STL_TRY_THROW( gZtcLibFunc.mPubData( aContext,
                                             aCMMgr->mSendBuf,
                                             aCMMgr->mSendBufLen,
                                             *aTrSeq,
                                             1 )
                       == ZTCL_SUCCESS,
                       RAMP_ERROR_PUB );
        (*aTrSeq)++;

        aCMMgr->mSendBufLen = 0;
        gZtcmTrCnt++;
    }
    else
    {
        stlMemcpy( &aCMMgr->mSendBuf[aCMMgr->mSendBufLen], aData, aSize );
        sHeader = (ztcTransHdr *)&aCMMgr->mSendBuf[aCMMgr->mSendBufLen];
        sHeader->mMsgId = STL_UINT64_MAX;

        aCMMgr->mSendBufLen += aSize;
    }
#endif

#if 0
    if( aSpool == ZTC_SEND_NOW )
    {
        if( aCMMgr->mSendBufLen == 0 )
        {
            if( aSize > 0 )
            {
                STL_TRY_THROW( gZtcLibFunc.mPubData( aContext,
                                                     aData,
                                                     aSize,
                                                     aDataType )
                               == ZTCL_SUCCESS,
                               RAMP_ERROR_PUB );
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            if( aCMMgr->mSendBufLen + aSize >= ZTC_MAX_SEND_BUFF_SIZE )
            {
                STL_TRY_THROW( gZtcLibFunc.mPubData( aContext,
                                                     aCMMgr->mSendBuf,
                                                     aCMMgr->mSendBufLen,
                                                     aDataType )
                               == ZTCL_SUCCESS,
                               RAMP_ERROR_PUB );

                aCMMgr->mSendBufLen = 0;
            }

            stlMemcpy( &aCMMgr->mSendBuf[aCMMgr->mSendBufLen], aData, aSize );

            sHeader = (ztcTransHdr *)&aCMMgr->mSendBuf[aCMMgr->mSendBufLen];
            sHeader->mMsgId = STL_UINT64_MAX;

            aCMMgr->mSendBufLen += aSize;

            STL_TRY_THROW( gZtcLibFunc.mPubData( aContext,
                                                 aCMMgr->mSendBuf,
                                                 aCMMgr->mSendBufLen,
                                                 aDataType )
                           == ZTCL_SUCCESS,
                           RAMP_ERROR_PUB );
        }

        aCMMgr->mSendBufLen = 0;
    }
    else
    {
        STL_TRY_THROW( aCMMgr->mSendBufLen + aSize <= ZTC_MAX_SEND_BUFF_SIZE, RAMP_ERROR_BUF_OVERFLOW );

        stlMemcpy( &aCMMgr->mSendBuf[aCMMgr->mSendBufLen], aData, aSize );
        sHeader = (ztcTransHdr *)&aCMMgr->mSendBuf[aCMMgr->mSendBufLen];
        sHeader->mMsgId = STL_UINT64_MAX;

        aCMMgr->mSendBufLen += aSize;
    }
#endif

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_PUB )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_MIDDLEWARE_ERROR,
                      NULL,
                      aErrorStack );
    }
#if 0
    STL_CATCH( RAMP_ERROR_BUF_OVERFLOW )
    {
        ztcmLogMsg( aErrorStack, "send buffer over flow." );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_INTERNAL_ERROR,
                      NULL,
                      aErrorStack );
    }
#endif
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmReceiveData( stlContext    * aContext,
                           stlChar       * aData,
                           stlInt32        aSize,     
                           stlErrorStack * aErrorStack )
{
    stlSize sRecvSize = 0;
    stlSize sSize;
    
    while( sRecvSize < aSize )
    {
        sSize = aSize - sRecvSize;
        
        if( stlRecv( STL_SOCKET_IN_CONTEXT( *aContext ),
                     aData + sRecvSize,
                     &sSize,
                     aErrorStack ) == STL_FAILURE )
        {
            STL_TRY_THROW( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_AGAIN,
                           RAMP_ERR_COMMUNICATION_LINK_FAILURE );
            (void)stlPopError( aErrorStack );
        }
        
        sRecvSize += sSize;
    }

//    stlHex("recv", aData, aSize);

    /**
     * DATA를 받더라도 종료되었을 경우 처리하지 않는다.
     */
    STL_TRY( gRunState == STL_TRUE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcmGetPeerIPAddr( stlContext    * aContext,
                             stlChar       * aIPAddr,
                             stlSize         aSize,
                             stlErrorStack * aErrorStack )
{
    stlSockAddr  sPeerAddr;
    
    STL_TRY( stlGetPeerName( STL_SOCKET_IN_CONTEXT( *aContext ),
                             &sPeerAddr,
                             aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlGetSockAddrIp( &sPeerAddr,
                               aIPAddr,
                               aSize,
                               aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcmWriteProtocolHdr( ztcProtocolHdr    * aProtocolHdr,
                                stlInt32            aEndian,
                                stlUInt8          * aBuffer,
                                stlErrorStack     * aErrorStack )
{
    stlInt32 sIndex = 0;

    if( aEndian == STL_PLATFORM_ENDIAN )
    {
        STL_WRITE_MOVE_INT32( aBuffer, &aProtocolHdr->mCategory, sIndex );
        STL_WRITE_MOVE_INT32( aBuffer, &aProtocolHdr->mType, sIndex );
        STL_WRITE_MOVE_INT64( aBuffer, &aProtocolHdr->mLength, sIndex );
    }
    else
    {
        ZTC_WRITE32( aBuffer, &aProtocolHdr->mCategory, sIndex );
        ZTC_WRITE32( aBuffer, &aProtocolHdr->mType, sIndex );
        ZTC_WRITE64( aBuffer, &aProtocolHdr->mLength, sIndex );
    }

    STL_DASSERT( sIndex == STL_SIZEOF( ztcProtocolHdr ) );

    return STL_SUCCESS;
}

stlStatus ztcmReadProtocolHdr( stlUInt8          * aBuffer,
                               stlInt32            aEndian,
                               ztcProtocolHdr    * aProtocolHdr,
                               stlErrorStack     * aErrorStack )
{
    stlInt32 sIndex = 0;

    if( aEndian == STL_PLATFORM_ENDIAN )
    {
        STL_READ_MOVE_INT32( &aProtocolHdr->mCategory, aBuffer, sIndex );
        STL_READ_MOVE_INT32( &aProtocolHdr->mType, aBuffer, sIndex );
        STL_READ_MOVE_INT64( &aProtocolHdr->mLength, aBuffer, sIndex );
    }
    else
    {
        ZTC_READ32( &aProtocolHdr->mCategory, aBuffer, sIndex );
        ZTC_READ32( &aProtocolHdr->mType, aBuffer, sIndex );
        ZTC_READ64( &aProtocolHdr->mLength, aBuffer, sIndex );
    }

    /**
     * TODO 일단 임시로 두가지 byteorder 다 가능하도록 함.
     * cyclone, cymon 모두 network endian으로 변경해야 함.
     */
    STL_DASSERT( ZTC_CATEGORY_MAX < UINT16_MAX );

    if( (aProtocolHdr->mCategory < ZTC_CATEGORY_NONE) || (aProtocolHdr->mCategory > ZTC_CATEGORY_MAX) )
    {
        /**
         * host endian 으로 바꾼다.
         */
        aProtocolHdr->mCategory = ztcToHost32( aProtocolHdr->mCategory );
        aProtocolHdr->mType = ztcToHost32( aProtocolHdr->mType );
        aProtocolHdr->mLength = ztcToHost64( aProtocolHdr->mLength );
    }

    STL_DASSERT( sIndex == STL_SIZEOF( ztcProtocolHdr ) );

    return STL_SUCCESS;
}



stlStatus ztcmWriteColumnInfo( ztcColumnInfo    * aColumnInfo,
                               stlUInt8         * aBuffer,
                               stlErrorStack    * aErrorStack )
{
    stlInt32 sIndex = 0;

    sIndex += STL_SIZEOF( aColumnInfo->mLink );
    ZTC_WRITE64( aBuffer, &aColumnInfo->mColumnId, sIndex );
    ZTC_WRITE64( aBuffer, &aColumnInfo->mDataTypeId, sIndex );
    ZTC_WRITE( aBuffer, aColumnInfo->mColumnName, sIndex, STL_SIZEOF(aColumnInfo->mColumnName) );
    ZTC_WRITE( aBuffer, aColumnInfo->mDataTypeStr, sIndex, STL_SIZEOF(aColumnInfo->mDataTypeStr) );
    ZTC_WRITE32( aBuffer, &aColumnInfo->mHashValue, sIndex );
    ZTC_WRITE8( aBuffer, &aColumnInfo->mIsPrimary, sIndex );
    ZTC_WRITE8( aBuffer, &aColumnInfo->mIsNullable, sIndex );
    ZTC_WRITE8( aBuffer, &aColumnInfo->mIsUnique, sIndex );
    ZTC_WRITE8( aBuffer, &aColumnInfo->mPadding1, sIndex );

    STL_DASSERT( STL_SIZEOF( SQLULEN ) == 8 );
    STL_DASSERT( STL_SIZEOF( SQLLEN ) == 8 );
    STL_DASSERT( STL_SIZEOF( SQLSMALLINT ) == 2 );

    ZTC_WRITE64( aBuffer, &aColumnInfo->mColumnSize, sIndex );
    ZTC_WRITE64( aBuffer, &aColumnInfo->mBufferLength, sIndex );
    ZTC_WRITE16( aBuffer, &aColumnInfo->mValueType, sIndex );
    ZTC_WRITE16( aBuffer, &aColumnInfo->mParameterType, sIndex );
    ZTC_WRITE16( aBuffer, &aColumnInfo->mDecimalDigits, sIndex );

    /**
     * SQL_CHAR, SQL_VARCHAR 일 경우, String Length Unit을 저장
     */
    ZTC_WRITE16( aBuffer, &aColumnInfo->mStringLengthUnit, sIndex );

    /**
     * Numeric 일 경우 Precision, Scale을 저장
     */
    ZTC_WRITE32( aBuffer, &aColumnInfo->mNumericPrecision, sIndex );
    ZTC_WRITE32( aBuffer, &aColumnInfo->mNumericScale, sIndex );

    /**
     * INTERVAL 일 경우 LeadingPrecision, SecondPrecision을 저장
     */
    ZTC_WRITE32( aBuffer, &aColumnInfo->mLeadingPrecision, sIndex );
    ZTC_WRITE16( aBuffer, &aColumnInfo->mSecondPrecision, sIndex );
    ZTC_WRITE16( aBuffer, &aColumnInfo->mPad1, sIndex );

    /**
     * Value를 저장하기 위한 공간
     */
    ZTC_WRITE64( aBuffer, &aColumnInfo->mValueMaxSize, sIndex );
    ZTC_WRITE64( aBuffer, &aColumnInfo->mStrValueMaxSize, sIndex );

    STL_DASSERT( sIndex == STL_SIZEOF( ztcColumnInfo ) );

    return STL_SUCCESS;
}

stlStatus ztcmReadColumnInfo( stlUInt8         * aBuffer,
                              ztcColumnInfo    * aColumnInfo,
                              stlErrorStack    * aErrorStack )
{
    stlInt32 sIndex = 0;

    sIndex += STL_SIZEOF( aColumnInfo->mLink );
    ZTC_READ64( &aColumnInfo->mColumnId, aBuffer, sIndex );
    ZTC_READ64( &aColumnInfo->mDataTypeId, aBuffer, sIndex );
    ZTC_READ( aColumnInfo->mColumnName, aBuffer, sIndex, STL_SIZEOF(aColumnInfo->mColumnName) );
    ZTC_READ( aColumnInfo->mDataTypeStr, aBuffer, sIndex, STL_SIZEOF(aColumnInfo->mDataTypeStr) );
    ZTC_READ32( &aColumnInfo->mHashValue, aBuffer, sIndex );
    ZTC_READ8( &aColumnInfo->mIsPrimary, aBuffer, sIndex );
    ZTC_READ8( &aColumnInfo->mIsNullable, aBuffer, sIndex );
    ZTC_READ8( &aColumnInfo->mIsUnique, aBuffer, sIndex );
    ZTC_READ8( &aColumnInfo->mPadding1, aBuffer, sIndex );

    STL_DASSERT( STL_SIZEOF( SQLULEN ) == 8 );
    STL_DASSERT( STL_SIZEOF( SQLLEN ) == 8 );
    STL_DASSERT( STL_SIZEOF( SQLSMALLINT ) == 2 );

    ZTC_READ64( &aColumnInfo->mColumnSize, aBuffer, sIndex );
    ZTC_READ64( &aColumnInfo->mBufferLength, aBuffer, sIndex );
    ZTC_READ16( &aColumnInfo->mValueType, aBuffer, sIndex );
    ZTC_READ16( &aColumnInfo->mParameterType, aBuffer, sIndex );
    ZTC_READ16( &aColumnInfo->mDecimalDigits, aBuffer, sIndex );

    /**
     * SQL_CHAR, SQL_VARCHAR 일 경우, String Length Unit을 저장
     */
    ZTC_READ16( &aColumnInfo->mStringLengthUnit, aBuffer, sIndex );

    /**
     * Numeric 일 경우 Precision, Scale을 저장
     */
    ZTC_READ32( &aColumnInfo->mNumericPrecision, aBuffer, sIndex );
    ZTC_READ32( &aColumnInfo->mNumericScale, aBuffer, sIndex );

    /**
     * INTERVAL 일 경우 LeadingPrecision, SecondPrecision을 저장
     */
    ZTC_READ32( &aColumnInfo->mLeadingPrecision, aBuffer, sIndex );
    ZTC_READ16( &aColumnInfo->mSecondPrecision, aBuffer, sIndex );
    ZTC_READ16( &aColumnInfo->mPad1, aBuffer, sIndex );

    /**
     * Value를 저장하기 위한 공간
     */
    ZTC_READ64( &aColumnInfo->mValueMaxSize, aBuffer, sIndex );
    ZTC_READ64( &aColumnInfo->mStrValueMaxSize, aBuffer, sIndex );

    STL_DASSERT( sIndex == STL_SIZEOF( ztcColumnInfo ) );

    return STL_SUCCESS;
}

stlStatus ztcmChangeByteOrderCaptureInfo( stlInt32            aByteOrder,
                                          ztcCaptureInfo    * aCaptureInfo,
                                          stlErrorStack     * aErrorStack )
{
    if( aByteOrder != STL_PLATFORM_ENDIAN )
    {
        aCaptureInfo->mRestartLSN       = ztcBSwap64( aCaptureInfo->mRestartLSN );
        aCaptureInfo->mLastCommitLSN    = ztcBSwap64( aCaptureInfo->mLastCommitLSN );
        aCaptureInfo->mFileSeqNo        = ztcBSwap64( aCaptureInfo->mFileSeqNo );
        aCaptureInfo->mRedoLogGroupId   = ztcBSwap32( aCaptureInfo->mRedoLogGroupId );
        aCaptureInfo->mBlockSeq         = ztcBSwap32( aCaptureInfo->mBlockSeq );
        aCaptureInfo->mReadLogNo        = ztcBSwap32( aCaptureInfo->mReadLogNo );
        aCaptureInfo->mWrapNo           = ztcBSwap16( aCaptureInfo->mWrapNo );
    }

    return STL_SUCCESS;
}

stlStatus ztcmChangeByteOrderTransHdr( stlInt32         aByteOrder,
                                       ztcTransHdr    * aTransHdr,
                                       stlErrorStack  * aErrorStack )
{
    if( aByteOrder != STL_PLATFORM_ENDIAN )
    {
        aTransHdr->mLSN         = ztcBSwap64( aTransHdr->mLSN );
        aTransHdr->mTransId     = ztcBSwap64( aTransHdr->mTransId );
        aTransHdr->mRecordId    = ztcBSwap64( aTransHdr->mRecordId );
        aTransHdr->mTablePId    = ztcBSwap64( aTransHdr->mTablePId );
        aTransHdr->mTableId     = ztcBSwap32( aTransHdr->mTableId );
        aTransHdr->mSlotIdx     = ztcBSwap32( aTransHdr->mSlotIdx );
        aTransHdr->mLogType     = ztcBSwap16( aTransHdr->mLogType );
        aTransHdr->mSize        = ztcBSwap16( aTransHdr->mSize );
        aTransHdr->mMsgId       = ztcBSwap64( aTransHdr->mMsgId );
    }

    return STL_SUCCESS;
}


/** @} */
