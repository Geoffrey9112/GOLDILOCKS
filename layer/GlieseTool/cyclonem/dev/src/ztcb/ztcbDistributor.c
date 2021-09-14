/*******************************************************************************
 * ztcbDistributor.c
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
 * @file ztcbDistributor.c
 * @brief GlieseTool Cyclone Distributor Routines
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcSlaveMgr  * gSlaveMgr;

stlStatus ztcbDistributeFlow( stlErrorStack * aErrorStack )
{
    stlBool       sIsMasterRow  = STL_FALSE;
    stlBool       sDoRead       = STL_FALSE;
    stlBool       sKeyChanged   = STL_FALSE;
    stlBool       sOccupied     = STL_TRUE;
    stlInt32      sFirstColOrd  = 0;
    stlInt32      sState        = 0;
    stlUInt64     sHashValue    = 0;
//    stlUInt64     sAftHashValue = 0;
    
    stlChar     * sTransPtr  = NULL;
    stlChar     * sTrans     = NULL;
    stlChar     * sTransData = NULL;
    ztcTransHdr * sTransHdr  = NULL;
    ztcTransHdr   sTmpTransHdr;
    
    ztcSlaveTableInfo  * sTableInfo  = NULL;
    ztcApplierMgr      * sApplierMgr = NULL;
    stlBool              sFirstData = STL_TRUE;
    
    STL_TRY( stlAllocHeap( (void**)&sTransPtr,
                           STL_SIZEOF( ztcTransHdr ) + ZTC_LOG_PIECE_MAX_SIZE + ZTC_MEM_ALIGN,
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    sTrans     = (void*)STL_ALIGN( (stlUInt64)sTransPtr, ZTC_MEM_ALIGN );
    sTransHdr  = (ztcTransHdr*)sTrans;
    sTransData = &sTrans[ STL_SIZEOF( ztcTransHdr ) ];


    /**
     * Transaction Header 읽기
     */
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );
        
        sTableInfo = NULL;
        
        if( sDoRead == STL_FALSE )
        {
            /**
             * TransHeader
             */
            STL_TRY( ztcbReadDataFromChunk( (stlChar*)sTransHdr,
                                            STL_SIZEOF( ztcTransHdr ),
                                            aErrorStack ) == STL_SUCCESS );
//            stlDebug("ztcbReadDataFromChunk ok : byte order %d \n", gSlaveMgr->mLogByteOrder);

            STL_TRY( ztcmChangeByteOrderTransHdr( gSlaveMgr->mLogByteOrder,
                                                  sTransHdr,
                                                  aErrorStack )
                     == STL_SUCCESS );

//            stlDebug("sTransHdr->mSize %d \n", sTransHdr->mSize);

            STL_DASSERT( sTransHdr->mSize <= ZTC_LOG_PIECE_MAX_SIZE );
        }

        /**
         * 실행후 처음 받는 data가 ZTC_CAPTURE_TYPE_BEGIN가 아니면
         * ZTC_CAPTURE_TYPE_BEGIN 나올때까지 data는 무시한다.
         */
        if( (sFirstData == STL_TRUE) &&
            (sTransHdr->mLogType != ZTC_CAPTURE_TYPE_BEGIN) )
        {
            if( sTransHdr->mSize > 0 )
            {
                STL_TRY( ztcbSkipDataFromChunk( sTransHdr->mSize,
                                                aErrorStack ) == STL_SUCCESS );
            }
            (void)gZtcLibFunc.mAckData( gSlaveMgr->mCMMgr.mSubContext, sTransHdr->mMsgId );

            continue;
        }
        else
        {
            sFirstData = STL_FALSE;
        }

        sDoRead = STL_FALSE;
//        printf("recv mLogType : %d \n", sTransHdr->mLogType);
        STL_DASSERT( sTransHdr->mLogType != ZTC_CAPTURE_TYPE_NONE );

        switch( sTransHdr->mLogType )
        {
            case ZTC_CAPTURE_TYPE_BEGIN:
            {
                STL_DASSERT( sApplierMgr == NULL );

                STL_TRY( ztcbGetNextApplierMgr( &sApplierMgr ) == STL_SUCCESS );

                /**
                 * 중간 데이터가 올 수 있나??? - cyclonem 에서....
                 */
                STL_DASSERT( sOccupied == STL_TRUE );

                /**
                 * 해당 Applier가 Commit되기를 대기하는 Applier가 있다는 얘기이므로
                 * 기존의 Transaction을 Commit하도록 하는 Flag를 세팅해서 Applier에서 인식하도록 함
                 */
                sTransHdr->mTxBeginFlag = sApplierMgr->mWaitDistState;

                STL_TRY( ztcbReadDataFromChunk( sTransData,
                                                sTransHdr->mSize,
                                                aErrorStack ) == STL_SUCCESS );

                STL_TRY( ztcbPushTransaction( sApplierMgr,
                                              sTrans,
                                              sTransHdr->mSize,
                                              sOccupied,
                                              aErrorStack ) == STL_SUCCESS );

                if( sTransHdr->mTxBeginFlag == STL_TRUE )
                {
                    /**
                     * Important!! : 기존의 Tx가 Commit되기를 위해서
                     */
                    STL_TRY( ztcaPushFirstChunkToWaitReadList( sApplierMgr,
                                                               aErrorStack ) == STL_SUCCESS );
                }

                sApplierMgr->mWaitDistState = STL_FALSE;
                break;
            }
            case ZTC_CAPTURE_TYPE_COMMIT:
            {
                STL_DASSERT( sApplierMgr != NULL );

                /**
                 * Default Value
                 */
                sTransHdr->mTxCommitFlag = STL_FALSE;

                /**
                 * TransData
                 */
                STL_TRY( ztcbReadDataFromChunk( sTransData,
                                                sTransHdr->mSize,
                                                aErrorStack ) == STL_SUCCESS );

                STL_TRY( ztcbPushTransaction( sApplierMgr,
                                              sTrans,
                                              sTransHdr->mSize,
                                              sOccupied,
                                              aErrorStack ) == STL_SUCCESS );

                /**
                 * Important!! : 만약 Sub-Buffer에 Tx의 부분이 저장되어 있다면... Buffer 관리 로직에 따라서 아직 처리되지 않고
                 * Buffer가 채워지길 기다리고 있을 것이다. 따라서 Commit이 되면 기다리지 않고 곧바로 처리할 수 있도록 한다.
                 */
                if( sOccupied == STL_FALSE )
                {
                    //Sub Chunk의 WriteChunk를 WaitReadCkList로 옮겨야 한다.
                    STL_TRY( ztcbPushFirstSubChunkToWaitReadList( aErrorStack ) == STL_SUCCESS );
                }

                /**
                 * re-Init...
                 */
                sApplierMgr          = NULL;
                sTransHdr->mTransId  = ZTC_INVALID_TRANSID;
                sTransHdr->mRecordId = 0;
                sOccupied            = STL_TRUE;
                break;
            }   
            case ZTC_CAPTURE_TYPE_INSERT:
            {
                STL_TRY( ztcmFindSlaveTableInfo( &sTableInfo,
                                                 sTransHdr->mTableId,
                                                 aErrorStack ) == STL_SUCCESS );
                
                if( sTableInfo != NULL )
                {
                    sHashValue = 0;

                    stlDebug("table id %d, table name %s \n",
                             sTransHdr->mTableId,
                             sTableInfo->mSlaveTable);

                    while( gRunState == STL_TRUE )
                    {
                        STL_DASSERT( sTransHdr->mSize <= ZTC_LOG_PIECE_MAX_SIZE );
                        STL_DASSERT( sApplierMgr != NULL );

                        /**
                         * TransData
                         */
                        STL_TRY( ztcbReadDataFromChunk( sTransData,
                                                        sTransHdr->mSize,
                                                        aErrorStack ) == STL_SUCCESS );

                        STL_TRY( ztcaIsMasterRow( sTransData,
                                                  &sIsMasterRow,
                                                  &sFirstColOrd,
                                                  aErrorStack ) == STL_SUCCESS );

                        /**
                         * Build Hash Value..
                         */
                        STL_TRY( ztcbAnalyzeInsert( sTableInfo,
                                                    &sHashValue,
                                                    sFirstColOrd,
                                                    sTransData,
                                                    aErrorStack ) == STL_SUCCESS );
                        if( sIsMasterRow == STL_TRUE )
                        {
                            break;   
                        }
                        stlDebug("ztcbPushTransaction : mSize %x \n", sTransHdr->mSize);

                        STL_TRY( ztcbPushTransaction( sApplierMgr,
                                                      sTrans,
                                                      sTransHdr->mSize,
                                                      sOccupied,
                                                      aErrorStack ) == STL_SUCCESS );

                        /**
                         * TransHeader
                         */
                        STL_TRY( ztcbReadDataFromChunk( (stlChar*)sTransHdr,
                                                        STL_SIZEOF( ztcTransHdr ),
                                                        aErrorStack ) == STL_SUCCESS );

                        STL_TRY( ztcmChangeByteOrderTransHdr( gSlaveMgr->mLogByteOrder,
                                                              sTransHdr,
                                                              aErrorStack )
                                 == STL_SUCCESS );

                        STL_DASSERT( sTransHdr->mSize <= ZTC_LOG_PIECE_MAX_SIZE );

                        STL_DASSERT( sTransHdr->mTableId == sTableInfo->mTableId );
                    } //end of while

                    /**
                     * Record ID를 사용하여 Slot을 Setting한다.
                     * 하나의 Record가 여러개의 Log로 나눠서 저장될 경우에는 MasterRow의 RID만 사용하면 된다.
                     * ... 위의 내용이 변경되어 Primary Key의 Hash Value로 사용한다.
                     * Note..
                     * Insert의 경우에는 하나의 Row를 구성하는 데이터가 Column의 역순으로 기록된다.
                     * 그리고 Delete, Update는 Primary key가 Supplemental Log에 기록되지만 Insert에는 Supplemental Log가 없다.
                     * 위의 이유로 인하여 하나의 Row를 구성하는 데이터 중에 앞 부분의 로그는 Applier Buffer에 저장되고 Master Row는 Sub Buffer에 저장될 수 있다.
                     * 이러한 내용이 문제를 일으킬 것 같지만, Applier는 하나의 Row를 구성하는 모든 데이터가 와야지만 분석을 완료하고 Execute를 수행하므로 문제 없이 수행될 것이다.
                     *
                     */
                    STL_TRY( ztcbOccupySlot( sApplierMgr,
                                             sTransHdr->mTransId,
                                             sHashValue,
                                             &sTransHdr->mSlotIdx,
                                             &sOccupied,
                                             aErrorStack ) == STL_SUCCESS );

                    STL_TRY( ztcbPushTransaction( sApplierMgr,
                                                  sTrans,
                                                  sTransHdr->mSize,
                                                  sOccupied,
                                                  aErrorStack ) == STL_SUCCESS );
                } //end of if
                break;
            }
            case ZTC_CAPTURE_TYPE_SUPPL_DEL:
            {
                STL_TRY( ztcmFindSlaveTableInfo( &sTableInfo,
                                                 sTransHdr->mTableId,
                                                 aErrorStack ) == STL_SUCCESS );

                if( sTableInfo != NULL )
                {
                    sHashValue = 0;
                    
                    stlDebug("table id %d, table name %s \n",
                             sTransHdr->mTableId,
                             sTableInfo->mSlaveTable);

                    STL_DASSERT( sTransHdr->mSize <= ZTC_LOG_PIECE_MAX_SIZE );
                    STL_DASSERT( sApplierMgr != NULL );
                
                    /**
                     * TransData
                     */
                    STL_TRY( ztcbReadDataFromChunk( sTransData,
                                                    sTransHdr->mSize,
                                                    aErrorStack ) == STL_SUCCESS );

                    /**
                     * Build Hash Value..
                     */
                    STL_TRY( ztcbAnalyzeSuppLog( sTableInfo,
                                                 &sHashValue,
                                                 sTransData,
                                                 aErrorStack ) == STL_SUCCESS );

                    /**
                     * Record ID를 사용하여 Slot을 Setting한다.
                     */
                    STL_TRY( ztcbOccupySlot( sApplierMgr,
                                             sTransHdr->mTransId,
                                             sHashValue,
                                             &sTransHdr->mSlotIdx,
                                             &sOccupied,
                                             aErrorStack ) == STL_SUCCESS );

                    STL_TRY( ztcbPushTransaction( sApplierMgr,
                                                  sTrans,
                                                  sTransHdr->mSize,
                                                  sOccupied,
                                                  aErrorStack ) == STL_SUCCESS );
                }
                break;
            }
            case ZTC_CAPTURE_TYPE_DELETE:
            {
                STL_DASSERT( 0 );
//                (void)gZtcLibFunc.mAckData( gSlaveMgr->mCMMgr.mSubContext, sTransHdr->mMsgId );

                /**
                 * Capture에서 Undo를 위해서 저장한 데이터이며 분석할 필요 없음
                 * 실제 분석할 내용은 Delete Supplemental Log이다.
                 */
                break;   
            }
            case ZTC_CAPTURE_TYPE_SUPPL_UPD:
            {
                STL_TRY( ztcmFindSlaveTableInfo( &sTableInfo,
                                                 sTransHdr->mTableId,
                                                 aErrorStack ) == STL_SUCCESS );
                
                if( sTableInfo != NULL )
                {
                    sHashValue    = 0;
//                    sAftHashValue = 0;

                    stlDebug("table id %d, table name %s \n",
                             sTransHdr->mTableId,
                             sTableInfo->mSlaveTable);

                    STL_DASSERT( sTransHdr->mSize <= ZTC_LOG_PIECE_MAX_SIZE );
                    STL_DASSERT( sApplierMgr != NULL );
                    
                    /**
                     * TransData
                     */
                    STL_TRY( ztcbReadDataFromChunk( sTransData,
                                                    sTransHdr->mSize,
                                                    aErrorStack ) == STL_SUCCESS );
                    
                    /**
                     * Build Hash Value.. for Before PK.
                     */
                    STL_TRY( ztcbAnalyzeSuppLog( sTableInfo,
                                                 &sHashValue,
                                                 sTransData,
                                                 aErrorStack ) == STL_SUCCESS );
                    
                    /**
                     * 해당 Value의 Slot을 획득한다.
                     */
                    STL_TRY( ztcbOccupySlot( sApplierMgr,
                                             sTransHdr->mTransId,
                                             sHashValue,
                                             &sTransHdr->mSlotIdx,
                                             &sOccupied,
                                             aErrorStack ) == STL_SUCCESS );
                    
                    STL_TRY( ztcbPushTransaction( sApplierMgr,
                                                  sTrans,
                                                  sTransHdr->mSize,
                                                  sOccupied,
                                                  aErrorStack ) == STL_SUCCESS );

                    /**
                     * Updated Column 정보 얻어오기
                     */
                    while( gRunState == STL_TRUE )
                    {
                        /**
                         * TransHeader
                         */
                        STL_TRY( ztcbReadDataFromChunk( (stlChar*)sTransHdr,
                                                        STL_SIZEOF( ztcTransHdr ),
                                                        aErrorStack ) == STL_SUCCESS );

                        STL_TRY( ztcmChangeByteOrderTransHdr( gSlaveMgr->mLogByteOrder,
                                                              sTransHdr,
                                                              aErrorStack )
                                 == STL_SUCCESS );

                        STL_DASSERT( sTransHdr->mSize <= ZTC_LOG_PIECE_MAX_SIZE );

                        if( sTransHdr->mLogType != ZTC_CAPTURE_TYPE_SUPPL_UPD_COLS )
                        {
                            break;
                        }

                        /**
                         * TransData
                         */
                        STL_TRY( ztcbReadDataFromChunk( sTransData,
                                                        sTransHdr->mSize,
                                                        aErrorStack ) == STL_SUCCESS );

                        /**
                         * Primary Key가 Update되었는지를 확인한다.
                         * Primary Key가 Update되었을 경우에는 변경되는 변경된 Key값의 Lock을 획득해야 한다.
                         */
                        STL_TRY( ztcbAnalyzeSuppUpdateColLog( sTableInfo,
                                                              sTransData, 
                                                              sTransHdr->mSize, 
                                                              &sKeyChanged,
                                                              aErrorStack ) == STL_SUCCESS );

                        STL_TRY( ztcbPushTransaction( sApplierMgr,
                                                      sTrans,
                                                      sTransHdr->mSize,
                                                      sOccupied,
                                                      aErrorStack ) == STL_SUCCESS );
                    }

                    /**
                     * UPDATE 관련 LOG 분석
                     */
                    while( gRunState == STL_TRUE )
                    {
                        /**
                         * TransData
                         */
                        STL_DASSERT( sTransHdr->mSize <= ZTC_LOG_PIECE_MAX_SIZE );

                        STL_TRY( ztcbReadDataFromChunk( sTransData,
                                                        sTransHdr->mSize,
                                                        aErrorStack ) == STL_SUCCESS );

                        if( sKeyChanged == STL_TRUE )
                        {
                            switch( sTransHdr->mLogType )
                            {
                                case ZTC_CAPTURE_TYPE_DELETE_FOR_UPDATE:
                                {
                                    /** Nothing To Do.. */
                                    break;   
                                }
                                case ZTC_CAPTURE_TYPE_UPDATE:
                                {
                                    STL_TRY( ztcbAnalyzeUpdate( sTableInfo,
                                                                sTransData,
                                                                sTransHdr->mSize,
                                                                aErrorStack ) == STL_SUCCESS );
                                    break;
                                }
                                case ZTC_CAPTURE_TYPE_INSERT_FOR_UPDATE:
                                {
                                    STL_TRY( ztcbAnalyzeInsert4Update( sTableInfo,
                                                                       sTransData,
                                                                       sTransHdr->mSize,
                                                                       aErrorStack ) == STL_SUCCESS );
                                    break;
                                }
                                case ZTC_CAPTURE_TYPE_APPEND_FOR_UPDATE:
                                {
                                    /**
                                     * Nothing To do...
                                     * TODO: Primary Key 변경시 해당 Log에 반영이 안되는지를 반드시 확인해야 한다.!!!
                                     */
                                    break;
                                }
                                case ZTC_CAPTURE_TYPE_SUPPL_UPD_BEF_COLS:
                                {
                                    /**
                                     * Nothing To Do...
                                     */
                                    break;
                                }
                                default:
                                    //assert?
                                    break;
                            }
                        }

                        /**
                         * Read TransHeader
                         * Update의 끝을 알 수 없으므로 TransHeader를 읽어보고 Update의 끝일 경우를 판단하고 처리한다.
                         */
                        STL_TRY( ztcbReadDataFromChunk( (stlChar*)&sTmpTransHdr,
                                                        STL_SIZEOF( ztcTransHdr ),
                                                        aErrorStack ) == STL_SUCCESS );

                        STL_TRY( ztcmChangeByteOrderTransHdr( gSlaveMgr->mLogByteOrder,
                                                              &sTmpTransHdr,
                                                              aErrorStack )
                                 == STL_SUCCESS );

                        STL_DASSERT( sTmpTransHdr.mSize <= ZTC_LOG_PIECE_MAX_SIZE );

                        if( sTmpTransHdr.mLogType != ZTC_CAPTURE_TYPE_DELETE_FOR_UPDATE &&
                            sTmpTransHdr.mLogType != ZTC_CAPTURE_TYPE_UPDATE &&
                            sTmpTransHdr.mLogType != ZTC_CAPTURE_TYPE_INSERT_FOR_UPDATE &&
                            sTmpTransHdr.mLogType != ZTC_CAPTURE_TYPE_APPEND_FOR_UPDATE &&
                            sTmpTransHdr.mLogType != ZTC_CAPTURE_TYPE_SUPPL_UPD_BEF_COLS )
                        {
                            sDoRead = STL_TRUE;

                            /**
                             * After Value의 Slot을 획득한다.
                             * !! After value를 보지 않으므로 현재는 처리하지 않도록 한다.
                             */
/*
                            if( sKeyChanged == STL_TRUE )
                            {
                                STL_TRY( ztcbBuildAfterHashValue( sTableInfo,
                                                                  &sAftHashValue,
                                                                  aErrorStack ) == STL_SUCCESS );
                                
                                STL_TRY( ztcbOccupySlot( sApplierMgr,
                                                         sTransHdr->mTransId,
                                                         sAftHashValue,
                                                         &sTransHdr->mSlotIdx,
                                                         &sOccupied,
                                                         aErrorStack ) == STL_SUCCESS );

                                ztcmLogMsg( aErrorStack, "sKeyChanged == STL_TRUE \n" );
                            }
*/
                            
                            STL_TRY( ztcbPushTransaction( sApplierMgr,
                                                          sTrans,
                                                          sTransHdr->mSize,
                                                          sOccupied,
                                                          aErrorStack ) == STL_SUCCESS );
                            
                            stlMemcpy( (stlChar*)sTransHdr, (stlChar*)&sTmpTransHdr, STL_SIZEOF( ztcTransHdr ) );
                            break;
                        }
                        else
                        {
                            STL_TRY( ztcbPushTransaction( sApplierMgr,
                                                          sTrans,
                                                          sTransHdr->mSize,
                                                          sOccupied,
                                                          aErrorStack ) == STL_SUCCESS );
                            
                            stlMemcpy( (stlChar*)sTransHdr, (stlChar*)&sTmpTransHdr, STL_SIZEOF( ztcTransHdr ) );
                        }
                    }

                } //end of if
                break;   
            }
            default:
//                ztcmLogMsg( aErrorStack, "not used log %d, %ld \n", sTransHdr->mLogType, sTransHdr->mMsgId );

                (void)gZtcLibFunc.mAckData( gSlaveMgr->mCMMgr.mSubContext, sTransHdr->mMsgId );

                /**
                 * CAPTURE는 되었지만 APPLY안되는 TABLE일 경우에는 처리하지 않는다.
                 */
                break;
        }

        /**
         * 적용하지 않을 데이터면 Skip 한다.
         */
        if( sTransHdr->mSize > 0 && 
            sTableInfo == NULL &&
            sTransHdr->mLogType != ZTC_CAPTURE_TYPE_COMMIT &&
            sTransHdr->mLogType != ZTC_CAPTURE_TYPE_BEGIN )
        {
            STL_TRY( ztcbSkipDataFromChunk( sTransHdr->mSize, 
                                            aErrorStack ) == STL_SUCCESS );
        }

    }// end of Distribute while
    
    sState = 0;
    stlFreeHeap( sTransPtr );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    if( sState > 0 )
    {
        stlFreeHeap( sTransPtr );
    }
    
    return STL_FAILURE;
}

stlStatus ztcbSubDistributeFlow( stlErrorStack * aErrorStack )
{
    stlChar       * sTransPtr   = NULL;
    stlChar       * sTrans      = NULL;
    stlChar       * sTransData  = NULL;
    ztcTransHdr   * sTransHdr   = NULL;
    ztcApplierMgr * sApplierMgr = NULL;
    stlInt32        sState      = 0;
    stlInt64        sAppId      = ZTC_INVALID_APPLIER_ID;
    stlInt64        sReadAppId  = ZTC_INVALID_APPLIER_ID;

    STL_TRY( stlAllocHeap( (void**)&sTransPtr,
                           STL_SIZEOF( ztcTransHdr ) + ZTC_LOG_PIECE_MAX_SIZE + ZTC_MEM_ALIGN,
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;

    sTrans     = (void*)STL_ALIGN( (stlUInt64)sTransPtr, ZTC_MEM_ALIGN );
    sTransHdr  = (ztcTransHdr*)sTrans;
    sTransData = &sTrans[ STL_SIZEOF( ztcTransHdr ) ];

    /**
     * 해당 Tx 데이터를 모두 Applier Buffer에 복사가 되면.....
     * Applier->mWaitSubDistState = STL_FALSE 로 해야한다. 그래야 Distributor에서 해당 Applier에 계속 데이터를 넣을 것이다.
     */
    while( 1 )
    {
        STL_TRY( gRunState == STL_TRUE );

        /**
         * TransHeader를 읽는다.
         */
        STL_TRY( ztcbReadDataFromSubChunk( (stlChar*)sTransHdr,
                                           STL_SIZEOF( ztcTransHdr ),
                                           aErrorStack ) == STL_SUCCESS );

        sReadAppId = sTransHdr->mSubDistAppId;

        if( sAppId == ZTC_INVALID_APPLIER_ID )
        {
            sAppId = sReadAppId;

            sApplierMgr = ztcbGetApplierMgr( (stlInt32)sAppId );
        }
        else
        {
            /**
             * 하나의 Tx는 반드시 하나의 Applier에서 처리되어야 한다....
             */
            STL_DASSERT( sAppId == sReadAppId );
        }

        /**
         * TransData를 읽는다.
         */
        STL_TRY( ztcbReadDataFromSubChunk( sTransData,
                                           sTransHdr->mSize,
                                           aErrorStack ) == STL_SUCCESS );
        
        if( sTransHdr->mLogType == ZTC_CAPTURE_TYPE_COMMIT )
        {
            /**
             * 해당 Commit은 반드시 먼저 Commit해야 한다.
             */
            sTransHdr->mTxCommitFlag = STL_TRUE;

            STL_TRY( ztcbPushTransToApplier( sApplierMgr,
                                             sTrans,
                                             sTransHdr->mSize,
                                             aErrorStack ) == STL_SUCCESS );

            /**
             * NOTE 성능관련 체크해봐야함
             * !! Applier가 곧바로 처리할 수 있게 해야 한다.
             */
            STL_TRY( ztcaPushFirstChunkToWaitReadList( sApplierMgr,
                                                       aErrorStack ) == STL_SUCCESS );


            ztcmAcquireSpinLock( &(sApplierMgr->mSubDistSpinLock) );
            sApplierMgr->mWaitSubDistState = ZTC_SUBDIST_USED_FALSE;
            ztcmReleaseSpinLock( &(sApplierMgr->mSubDistSpinLock) );

            sAppId = ZTC_INVALID_APPLIER_ID;
        }
        else
        {
            /**
             * Occypy Slot -- SubRef
             */
            if( sTransHdr->mSlotIdx != ZTC_INVALID_SLOT_IDX )
            {
                STL_TRY( ztcbOccupySubSlot( sApplierMgr,
                                            sTransHdr->mSlotIdx,
                                            aErrorStack ) == STL_SUCCESS );
            }

            /**
             * Applier에 전송할 수 있는 상태
             */
            STL_TRY( ztcbPushTransToApplier( sApplierMgr,
                                             sTrans,
                                             sTransHdr->mSize,
                                             aErrorStack ) == STL_SUCCESS );
        }
    }

    sState = 0;
    stlFreeHeap( sTransPtr );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlFreeHeap( sTransPtr );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztcbGetNextApplierMgr( ztcApplierMgr ** aApplierMgr )
{
    stlInt32         sApplierCount = gSlaveMgr->mApplierCount;
    ztcApplierMgr  * sApplierMgr   = NULL;

    STL_DASSERT( gSlaveMgr->mCurApplierIdx < sApplierCount );

    /**
     * 만약 Applier가 Sub Buffer의 데이터를 대기하고 있다고 한다면
     * Distributor에서 해당 Applier에 데이터를 주면 안된다... 데이터가 꼬일 수 있기 때문에....
     */
    while( 1 )
    {
        gSlaveMgr->mCurApplierIdx++;

        if( gSlaveMgr->mCurApplierIdx == sApplierCount )
        {
            gSlaveMgr->mCurApplierIdx = 0;
        }

        sApplierMgr = &gSlaveMgr->mApplierMgr[gSlaveMgr->mCurApplierIdx];

        stlMemBarrier();

        if( sApplierMgr->mWaitSubDistState == ZTC_SUBDIST_USED_FALSE )
        {
            break;
        }

        STL_TRY( gRunState == STL_TRUE );
    }

    STL_DASSERT( sApplierMgr != NULL );

    *aApplierMgr = sApplierMgr;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


ztcApplierMgr * ztcbGetApplierMgr( stlInt32 aAppId )
{
    STL_DASSERT( aAppId < gSlaveMgr->mApplierCount &&
                aAppId >= 0 );

    return &gSlaveMgr->mApplierMgr[ aAppId ];
}

stlStatus ztcbFlushApplierChunk( stlErrorStack * aErrorStack )
{
    stlInt64        sApplierCount = gSlaveMgr->mApplierCount;
    stlInt64        sIdx          = 0;
    ztcApplierMgr * sApplierMgr   = NULL;
    
    for( sIdx = 0; sIdx < sApplierCount; sIdx++ )
    {
        sApplierMgr = &gSlaveMgr->mApplierMgr[ sIdx ];

        stlMemBarrier();

        if( sApplierMgr->mWaitSubDistState == ZTC_SUBDIST_USED_FALSE )
        {
            STL_TRY( ztcaPushFirstChunkToWaitReadList( sApplierMgr,
                                                       aErrorStack ) == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcbOccupySlot( ztcApplierMgr * aApplierMgr,
                          stlInt64        aTransId,
                          stlUInt64       aHashValue,
                          stlInt32      * aSlotIdx,
                          stlBool       * aOccupied,
                          stlErrorStack * aErrorStack )
{
    ztcRecStateItem  * sItem       = NULL;
    ztcApplierMgr    * sApplierMgr = NULL;
    stlInt32           sIdx        = ZTC_INVALID_SLOT_IDX;
    stlInt32           sAppId      = ZTC_INVALID_APPLIER_ID;
    stlBool            sOccupied   = STL_TRUE;
    
    ZTC_GET_DISTRIBUTE_SLOT_IDX( aHashValue, &sIdx );
    
    sItem = &gSlaveMgr->mItemArr[ sIdx ];
    
    STL_DASSERT( sItem != NULL );

    //stlPrintf("OCCUPY SLOT[APPID:%d][SLOTID:%d]\n", aApplierMgr->mAppId, sIdx );
//    printf("sIdx %d, hash %lu \n", sIdx, aHashValue);
    
    ztcmAcquireSpinLock( &(sItem->mSpinLock) );
    
    if( *aOccupied == STL_FALSE )
    {
        /**
         * 기존의 분석에서 Occupy에 실패했기 때문에 해당 데이터는 기존의 데이터와 마찬가지로 SubDistributor에서 대기해야 한다.
         * - 따라서 해당 SlotItem의 SubRefCount를 ++한뒤 SubBuffer로 복사한다.
         */
        sItem->mSubRefCount++;
        sOccupied = STL_FALSE;
        sAppId    = sItem->mAppId;          //INVALID일수도 있음!!!
    }
    else
    {
        /**
         * 기존의 데이터 분석에서 모두 Occupy에 성공
         */
        if( sItem->mSubRefCount == 0 )
        {
            /**
             * Sub-Distributor에서 대기하고 있지 않으며...
             */
            if( sItem->mAppId == ZTC_INVALID_APPLIER_ID )
            {
                /**
                 * 다른 Applier에서 사용하고 있지 않은 경우..
                 * Occupy 한뒤에 Applier Buffer에 데이터 전송..
                 */

                STL_DASSERT( sItem->mRefCount == 0 );
                
                sItem->mAppId = aApplierMgr->mAppId;
                sItem->mRefCount++;
                sOccupied = STL_TRUE;
            }
            else
            {
                if( aApplierMgr->mAppId == sItem->mAppId )
                {
                    /**
                     * 같은 Applier에서 사용중인 경우
                     * Applier Buffer로 데이터 전송
                     */
                    sItem->mRefCount++;
                    sOccupied = STL_TRUE;
                }
                else
                {
                    sItem->mSubRefCount++;
                    sOccupied = STL_FALSE;
                    sAppId    = sItem->mAppId;

                    ztcmAcquireSpinLock( &(aApplierMgr->mSubDistSpinLock) );
                    aApplierMgr->mWaitSubDistState = ZTC_SUBDIST_USED_TRUE;
//                    printf("1 mWaitSubDistState = ZTC_SUBDIST_USED_TRUE \n");
                    ztcmReleaseSpinLock( &(aApplierMgr->mSubDistSpinLock) );
                }
            }
        }
        else
        {
            /**
             * Sub-Distributor에서 대기중일 경우...
             * Sub Distributor로 보내야 함 그리고 해당 Applier에 Sub Distributor에서 데이터를 대기하고 있다고 세팅...
             */
            sItem->mSubRefCount++;
            sOccupied = STL_FALSE;
            sAppId    = sItem->mAppId;

            ztcmAcquireSpinLock( &(aApplierMgr->mSubDistSpinLock) );
            aApplierMgr->mWaitSubDistState = ZTC_SUBDIST_USED_TRUE;
//            printf("2 mWaitSubDistState = ZTC_SUBDIST_USED_TRUE \n");
            ztcmReleaseSpinLock( &(aApplierMgr->mSubDistSpinLock) );
        }
    }

    ztcmReleaseSpinLock( &(sItem->mSpinLock) );

    if( sAppId != ZTC_INVALID_APPLIER_ID )
    {
        /**
         * Important
         * 해당 Slot이 다른 Applier에서 Occupy되었다면 해당 Applier가 Commit되어야만 Sub-Distributor에서 다시 해당 Slot을 Occupy할 수 있다.
         * 만약 해당 Applier가 Commit 되지 않은 상태에서 새로운 Tx가 지속적으로 수행된다면 Dead-lock이 발생한다.
         * Distributor에서 순차적으로 수행되는 값이므로 (mWaitDistState) 동시성 제어가 필요없다.
         */
        sApplierMgr = ztcbGetApplierMgr( sAppId );
        sApplierMgr->mWaitDistState = STL_TRUE;
    }
    
    *aSlotIdx  = sIdx;
    *aOccupied = sOccupied;
    
    return STL_SUCCESS;
}

stlStatus ztcbPushTransaction( ztcApplierMgr * aApplierMgr,
                               stlChar       * aTrans,
                               stlUInt16       aSize,
                               stlBool         aOccupied,
                               stlErrorStack * aErrorStack )
{
    ztcTransHdr * sTransHdr  = NULL;

    /**
     * Slave가 종료될 경우 Applier Thread가 먼저 종료될 수 있다.
     * 이 경우 Distributor Thread에서 Applier Buffer에 수신한 데이터를 넘겨주려고 할때 해당 Buffer는 이미 Fin되었기 때문에
     * Applier Buffer에 넣기 전에 현재 운영 상태를 반드시 체크해야 한다.
     */
    STL_TRY( gRunState == STL_TRUE );
    
    if( aOccupied == STL_TRUE )
    {
        /**
         * 정상적으로 Occupy되었으면 Applier Buffer에 바로 전송한다.
         */
        STL_TRY( ztcbPushTransToApplier( aApplierMgr,
                                         aTrans,
                                         aSize,
                                         aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        sTransHdr = (ztcTransHdr*)aTrans;
        sTransHdr->mSubDistAppId = (stlInt16)aApplierMgr->mAppId;

        /**
         * Sub-Distributor에서 처리하도록 Sub-Buffer로 전송한다.
         */
        STL_TRY( ztcbPushTransToSubBuffer( aApplierMgr,
                                           aTrans,
                                           aSize,
                                           aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcbPushTransToSubBuffer( ztcApplierMgr * aApplierMgr,
                                    stlChar       * aTrans,
                                    stlUInt16       aSize,
                                    stlErrorStack * aErrorStack )
{
    STL_TRY( gRunState == STL_TRUE );

    STL_TRY( ztcbWriteDataToSubChunk( aApplierMgr,
                                      aTrans,
                                      STL_SIZEOF( ztcTransHdr ) + aSize,
                                      aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcbPushTransToApplier( ztcApplierMgr * aApplierMgr,
                                  stlChar       * aTrans,
                                  stlUInt16       aSize,
                                  stlErrorStack * aErrorStack )
{
    STL_TRY( gRunState == STL_TRUE );
    
    STL_TRY( ztcaWriteDataToChunk( aApplierMgr,
                                   aTrans,
                                   STL_SIZEOF( ztcTransHdr ) + aSize,
                                   aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcbOccupySubSlot( ztcApplierMgr * aApplierMgr,
                             stlInt32        aSlotIdx,
                             stlErrorStack * aErrorStack )
{
    ztcRecStateItem  * sItem = NULL;

    STL_PARAM_VALIDATE( aSlotIdx <= ZTC_DISTRIBUTOR_SLOT_SIZE, aErrorStack );

    sItem = &gSlaveMgr->mItemArr[ aSlotIdx ];

    ztcmAcquireSpinLock( &(sItem->mSpinLock) );

    STL_DASSERT( sItem->mSubRefCount > 0 );

    if( sItem->mAppId == ZTC_INVALID_APPLIER_ID )
    {
        /**
         * Applier에서 이미 수행되었거나, Tx 중간 데이터라서 Occupy 하지 않은 경우...
         */
        STL_DASSERT( sItem->mRefCount == 0 );

        sItem->mAppId = aApplierMgr->mAppId;
        sItem->mSubRefCount--;
        sItem->mRefCount++;
    }
    else
    {
        /**
         * Applier에서 수행되는중..
         */
        if( aApplierMgr->mAppId == sItem->mAppId )
        {
            /**
             * 같은 Applier이므로 곧바로 Applier Buffer로 데이터 전송 가능
             */
            sItem->mSubRefCount--;
            sItem->mRefCount++;
        }
        else
        {
            /**
             * 다른 Applier에서 사용중이므로 대기해야 함
             */
            ztcmReleaseSpinLock( &(sItem->mSpinLock) );

            while( 1 )
            {
                ztcmAcquireSpinLock( &(sItem->mSpinLock) );

                if( sItem->mAppId == ZTC_INVALID_APPLIER_ID )
                {
                    break;
                }

                ztcmReleaseSpinLock( &(sItem->mSpinLock) );

                /**
                 * valgrind test시 applier에서 lock을 못잡는 
                 * 현상이 있어서 아래 yield 추가함
                 */
                stlYieldThread();

                STL_TRY( gRunState == STL_TRUE );
            }

            STL_DASSERT( sItem->mRefCount == 0 );

            sItem->mAppId = aApplierMgr->mAppId;
            sItem->mSubRefCount--;
            sItem->mRefCount++;
        }
    }

    ztcmReleaseSpinLock( &(sItem->mSpinLock) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */

