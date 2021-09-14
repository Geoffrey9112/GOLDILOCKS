/*******************************************************************************
 * smxlUndoRelation.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
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
#include <smxlDef.h>
#include <smrDef.h>
#include <smr.h>
#include <smp.h>
#include <sms.h>
#include <smxl.h>
#include <smxm.h>
#include <smkl.h>
#include <sma.h>
#include <sme.h>
#include <smd.h>
#include <smxlUndoRelMgr.h>
#include <smxlUndoRelation.h>

extern smxlWarmupEntry  * gSmxlWarmupEntry;
extern smfWarmupEntry   * gSmfWarmupEntry;
extern smxlUndoFunc     * gRecoveryUndoFuncs[];
extern stlInt64           gDataStoreMode;

/**
 * @file smxlUndoRelation.c
 * @brief Storage Manager Layer Local Transactional Undo Relation Internal Routines
 */

/**
 * @addtogroup smxl
 * @{
 */

/**
 * @brief Transaction Record를 Undo segment에 기록한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in,out] aEnv Environment 구조체
 */

stlStatus smxlInsertTransRecord( smxlTransId   aTransId,
                                 smlEnv      * aEnv )
{
    smxlUndoRelEntry * sUndoRelEntry;
    smxlUndoRelCache * sUndoRelCache;
    smlRid             sUndoSegRid;
    smpHandle          sPageHandle;
    stlInt32           sState = 0;
    smxmTrans          sMiniTrans;

    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );
    
    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( aTransId );

    if( smxlIsTransRecordRecorded( aTransId ) == STL_FALSE )
    {
        if( sUndoRelEntry == NULL )
        {
            STL_TRY( smxlAllocUndoRelationInternal( aTransId,
                                                    aEnv ) == STL_SUCCESS );
            sState = 1;
        }

        sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( aTransId );
        sUndoRelCache = (smxlUndoRelCache*)smxlGetUndoRelHandle( sUndoRelEntry );

        sUndoSegRid = smsGetSegRid( sUndoRelCache->mSegHandle );

        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            sUndoSegRid,
                            SMXM_ATTR_REDO,
                            aEnv )
                 == STL_SUCCESS );
        sState = 2;

        STL_TRY( smxlInsertTransRecordInternal( &sMiniTrans,
                                                &sPageHandle,
                                                aEnv )
                 == STL_SUCCESS );

        sState = 1;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smxmRollback( &sMiniTrans, aEnv );
        case 1:
            (void) smxlFreeUndoRelation( aTransId, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}
    

/**
 * @brief Mini-Transaction을 이용해서 Transaction Record를 Undo segment에 기록한다.
 * @param[in] aMiniTrans Mini-Transaction
 * @param[in,out] aEnv Environment 구조체
 */

stlStatus smxlInsertTransRecordWithMiniTrans( smxmTrans  * aMiniTrans,
                                              smlEnv     * aEnv )
{
    smpHandle          sPageHandle;
    stlInt32           sState = 0;
    smxlTransId        sTransId;
    smxlUndoRelEntry * sUndoRelEntry;

    sTransId = smxmGetTransId( aMiniTrans );

    STL_TRY_THROW( sTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );

    STL_TRY_THROW( smxlIsTransRecordRecorded( sTransId ) == STL_FALSE, RAMP_SUCCESS );
    
    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( sTransId );
    
    if( sUndoRelEntry == NULL )
    {
        STL_TRY( smxlAllocUndoRelationInternal( sTransId,
                                                aEnv ) == STL_SUCCESS );
        sState = 1;
    }

    STL_TRY( smxlInsertTransRecordInternal( aMiniTrans,
                                            &sPageHandle,
                                            aEnv )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) smxlFreeUndoRelation( sTransId, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}
    

/**
 * @brief Transaction Record의 기록을 준비한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in,out] aEnv Environment 구조체
 * @remark Transaction Record는 실제 기록하지는 않지만, 트랜잭션 레코드의 위치를 전파한다.
 * <BR> 해당 함수를 제공하는 이유는 Logging 회수를 줄이기 위함이다.
 * <BR> Transaction Record 하나를 기록하기 위해서 Mini-Transaction을 Commit하는 것은
 * <BR> 큰 비용에 해당한다.
 * <BR> 실제 Tranaction Record는 Undo Record의 Logging과 함께 기록된다.
 */

stlStatus smxlPrepareTransRecord( smxlTransId   aTransId,
                                  smlEnv      * aEnv )
{
    smxlUndoRelEntry * sUndoRelEntry;
    smxlUndoRelCache * sUndoRelCache;
    smlRid             sUndoSegRid;
    stlInt32           sState = 0;
    smxmTrans          sMiniTrans;
    smxlTransId        sTransId;

    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );
    
    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( aTransId );

    if( smxlIsTransRecordRecorded( aTransId ) == STL_FALSE )
    {
        if( sUndoRelEntry == NULL )
        {
            STL_TRY( smxlAllocUndoRelationInternal( aTransId,
                                                    aEnv ) == STL_SUCCESS );
            sState = 1;
        }

        sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( aTransId );
        sUndoRelCache = (smxlUndoRelCache*)smxlGetUndoRelHandle( sUndoRelEntry );

        sUndoSegRid = smsGetSegRid( sUndoRelCache->mSegHandle );

        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            sUndoSegRid,
                            SMXM_ATTR_REDO,
                            aEnv )
                 == STL_SUCCESS );
        sState = 2;

        STL_TRY( smxlPrepareTransRecordInternal( &sMiniTrans,
                                                 &sTransId,
                                                 aEnv )
                 == STL_SUCCESS );

        sState = 1;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        /**
         * Mini-Transaction Commit이후에 Transaction Table의 TransId를 변경한다.
         * - 아직 Rollback이 필요한 상황이 아니기 때문에 Redo Log에 Undo Relation 정보가
         *   설정되서는 안된다.
         */
        smxlUpdateTransId( sTransId, aEnv );
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smxmRollback( &sMiniTrans, aEnv );
        case 1:
            (void) smxlFreeUndoRelation( aTransId, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}
    

/**
 * @brief 주어진 페이지 개수만큼의 공간을 확보한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aPageCount Transaction Identifier
 * @param[in,out] aEnv Environment 구조체
 */

stlStatus smxlPrepareUndoSpace( smxlTransId   aTransId,
                                stlUInt32     aPageCount,
                                smlEnv      * aEnv )
{
    smxlUndoRelEntry * sUndoRelEntry;
    smxlUndoRelCache * sUndoRelCache;
    stlInt64           sStolenPageCount = 0;
    stlStatus          sStatus;
    stlInt32           sState = 0;
    smxmTrans          sMiniTrans;
#ifdef STL_DEBUG
    stlInt32           sErrorStackDepth = 0;
#endif
    
    STL_PARAM_VALIDATE( aPageCount > 0, KNL_ERROR_STACK(aEnv) );

    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );
    
    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( aTransId );
    STL_ASSERT( sUndoRelEntry != NULL );
    sUndoRelCache = (smxlUndoRelCache*)smxlGetUndoRelHandle( sUndoRelEntry );
    STL_ASSERT( sUndoRelCache != NULL );

    sStatus = smsPreparePage( aTransId,
                              sUndoRelCache->mSegHandle,
                              aPageCount,
                              aEnv );

    if( sStatus == STL_FAILURE )
    {
        if( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) )
            == SML_ERRCODE_NO_MORE_EXTENDIBLE_DATAFILE )
        {
#ifdef STL_DEBUG
            sErrorStackDepth = stlGetErrorStackDepth( KNL_ERROR_STACK(aEnv) );
#endif
            
            STL_TRY( smxmBegin( &sMiniTrans,
                                aTransId,
                                smsGetSegRid( sUndoRelCache->mSegHandle ),
                                SMXM_ATTR_REDO,
                                aEnv ) == STL_SUCCESS );
            sState = 1;
            
            /**
             * Undo Space를 확보할수 없다면 다른 세그먼트에서 steal한다.
             */
            STL_TRY( smxlStealPages( &sMiniTrans,
                                     sUndoRelCache,
                                     &sStolenPageCount,
                                     aEnv )
                     == STL_SUCCESS );

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
            
            STL_DASSERT( sErrorStackDepth == stlGetErrorStackDepth( KNL_ERROR_STACK(aEnv) ) );
            
            /**
             * Steal 연산이 실패하면 에러를 발생시킨다.
             */
            STL_TRY( sStolenPageCount > 0 );
            
            /**
             * 공간 확보를 성공했다면 기존 에러를 버리고 성공
             */
            (void)stlPopError( KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_SUCCESS );
        }
        
        /**
         * 공간에 관련된 문제가 아니라면 pass
         */
        STL_TRY( 0 );
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }
    
    return STL_FAILURE;
}


/**
 * @brief Undo segment를 할당한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in,out] aEnv Environment 구조체
 */

stlStatus smxlAllocUndoRelation( smxlTransId   aTransId,
                                 smlEnv      * aEnv )
{
    smxlUndoRelEntry * sUndoRelEntry;

    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );
    
    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( aTransId );

    STL_TRY_THROW( sUndoRelEntry == NULL, RAMP_SUCCESS );
    
    STL_TRY( smxlAllocUndoRelationInternal( aTransId,
                                            aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    

/**
 * @brief Undo Record를 기록한다.
 * @param[in] aMiniTrans       Mini-Transaction 포인터
 * @param[in] aComponentClass  Component Class
 * @param[in] aLogType         Log Type
 * @param[in] aData            로깅할 데이터
 * @param[in] aDataSize        로깅할 데이터 사이즈
 * @param[in] aTargetRid       회복할 레코드 주소
 * @param[out] aUndoRid        기록된 undo record의 RID
 * @param[in,out] aEnv Environment 구조체
 */

stlStatus smxlInsertUndoRecord( smxmTrans  * aMiniTrans,
                                stlChar      aComponentClass,
                                stlUInt16    aLogType,
                                stlChar    * aData,
                                stlUInt16    aDataSize,
                                smlRid       aTargetRid,
                                smlRid     * aUndoRid,
                                smlEnv     * aEnv )
{
    smxlUndoRelCache * sUndoRelCache;
    smxlTransId        sTransId;
    smxmSavepoint      sSavepoint;
    stlBool            sIsNewPage = STL_FALSE;
    stlChar          * sUndoRecord;
    smxlUndoRecHdr     sUndoRecHdr;
    stlInt32           sLogOffset = 0;
    stlInt16           sSlotSeq;
    smpHandle          sPageHandle;
    smxlUndoRelEntry * sUndoRelEntry;
    smlRid             sUndoSegRid;
    smlRid             sTargetSegRid;
    stlInt64         * sRecycleNo;
    stlInt32           sState = 0;
    smlTbsId           sTbsId;
    smlPid             sPid;

    sTransId = smxmGetTransId( aMiniTrans );
    *aUndoRid = SML_INVALID_RID;

    STL_TRY_THROW( sTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );

    if( smxlIsTransRecordRecorded( sTransId ) == STL_FALSE )
    {
        STL_TRY( smxlInsertTransRecordInternal( aMiniTrans,
                                                &sPageHandle,
                                                aEnv )
                 == STL_SUCCESS );
    }
    
    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( sTransId );

    /**
     * Undo Segment에 Binding되어 있어야 한다.
     */
    STL_ASSERT( sUndoRelEntry != NULL );
    
    sUndoRelCache = (smxlUndoRelCache*)smxlGetUndoRelHandle( sUndoRelEntry );
    
    STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                &sSavepoint,
                                aEnv ) == STL_SUCCESS );
    
    STL_TRY( smsGetInsertablePage( aMiniTrans,
                                   sUndoRelCache->mSegHandle,
                                   SMP_PAGE_TYPE_UNDO,
                                   NULL,  /* aIsAgableFunc */
                                   NULL, /* aSeachHint */
                                   &sPageHandle,
                                   &sIsNewPage,
                                   aEnv ) == STL_SUCCESS );

    sRecycleNo = (stlInt64*)smpGetVolatileArea( &sPageHandle );
    sRecycleNo[0] += 1;
    stlMemBarrier();
    sState = 1;
        
    while( 1 )
    {
        if( sIsNewPage == STL_TRUE )
        {
            /**
             * Transaction Record의 READ는 NO-LATCH 모드로 수행된다.
             * 페이지의 구조가 변경되는 경우는 Reader에게 페이지 변경상태를
             * 알려야 한다.
             */
            STL_TRY( smpInitBody( &sPageHandle,
                                  SMP_ORDERED_SLOT_TYPE,
                                  0,
                                  NULL,
                                  0,
                                  0,
                                  STL_TRUE,  /* aUseFreeSlotList */
                                  aMiniTrans,
                                  aEnv ) == STL_SUCCESS );
            
            STL_TRY( smpUpdateAgableScn( aMiniTrans,
                                         &sPageHandle,
                                         SML_INFINITE_SCN,
                                         aEnv )
                     == STL_SUCCESS );

            smxlIncUndoPageCount( sTransId );
        }

        STL_TRY( smpAllocSlot4Undo( &sPageHandle,
                                    aDataSize + STL_SIZEOF( smxlUndoRecHdr ),
                                    STL_FALSE, /* aIsForTest */
                                    &sUndoRecord,
                                    &sSlotSeq ) == STL_SUCCESS );

        STL_ASSERT( (sIsNewPage != STL_TRUE) || (sUndoRecord != NULL) );

        if( sUndoRecord == NULL )
        {
            sState = 0;
            stlMemBarrier();
            sRecycleNo[1] += 1;
    
            STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                              &sSavepoint,
                                              aEnv ) == STL_SUCCESS );

            STL_TRY_THROW( smxlGetUndoPageCount( sTransId ) <
                           knlGetPropertyBigIntValueByID( KNL_PROPERTY_TRANSACTION_MAXIMUM_UNDO_PAGE_COUNT,
                                                          KNL_ENV(aEnv) ),
                           RAMP_ERR_EXCEED_TRANSACTION_UNDO );
    
            STL_TRY( smxlAllocPage( aMiniTrans,
                                    sUndoRelCache,
                                    &sPageHandle,
                                    aEnv )
                     == STL_SUCCESS );

            sIsNewPage = STL_TRUE;
            
            sRecycleNo = (stlInt64*)smpGetVolatileArea( &sPageHandle );
            sRecycleNo[0] += 1;
            stlMemBarrier();
            sState = 1;
        }
        else
        {
            break;
        }
    }

    sTbsId = smsGetTbsId( sUndoRelCache->mSegHandle );
    sPid = smpGetPageId( &sPageHandle );
    SML_MAKE_RID( &sUndoRelCache->mTransLstUndoRid,
                  sTbsId,
                  sPid,
                  sSlotSeq );
    sTargetSegRid = smxmGetSegRid( aMiniTrans );
    sUndoSegRid = smsGetSegRid( sUndoRelCache->mSegHandle );
    sUndoRecHdr.mType = aLogType;
    sUndoRecHdr.mComponentClass = aComponentClass;
    sUndoRecHdr.mTargetDataRid = aTargetRid;
    sUndoRecHdr.mTargetSegPid = sTargetSegRid.mPageId;
    sUndoRecHdr.mUndoSegPid = sUndoSegRid.mPageId;
    sUndoRecHdr.mSize = aDataSize;
    sUndoRecHdr.mFlag = SMXL_UNDO_RECORD;
    sUndoRecHdr.mRedoLogged = STL_FALSE;
    sUndoRecHdr.mPadding[0] = 0;

    if( (smxmGetAttr(aMiniTrans) & SMXM_ATTR_REDO) == SMXM_ATTR_REDO )
    {
        sUndoRecHdr.mRedoLogged = STL_TRUE;
    }

    
    SMXL_WRITE_MOVE_UNDO_REC_HDR( sUndoRecord,
                                  &sUndoRecHdr,
                                  sLogOffset );

    STL_WRITE_MOVE_BYTES( sUndoRecord,
                          aData,
                          aDataSize,
                          sLogOffset );

    sState = 0;
    stlMemBarrier();
    sRecycleNo[1] += 1;
    
    if( (smxmGetAttr(aMiniTrans) & SMXM_ATTR_REDO) == SMXM_ATTR_REDO )
    {
        /**
         * No logging table/index에 대해서는 로그를 기로하지 않는다
         */
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_TRANSACTION,
                               SMR_LOG_INSERT_UNDO_RECORD,
                               SMR_REDO_TARGET_UNDO_SEGMENT,
                               sUndoRecord,
                               sLogOffset,
                               sTbsId,
                               sPid,
                               sSlotSeq,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    *aUndoRid = sUndoRelCache->mTransLstUndoRid;

    STL_ASSERT( sRecycleNo[0] == sRecycleNo[1] );

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXCEED_TRANSACTION_UNDO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEED_TRANSACTION_UNDO,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        sRecycleNo[1] += 1;
    }

    return STL_FAILURE;
}


/**
 * @brief PREPARED transaction을 위해 Lock 정보를 recover한다.
 * @param[in]     aTransId      Local Transaction Identifier
 * @param[in]     aRecoverLock  Lock 정보를 이전상태로 원복할지 여부
 * @param[out]    aXid          Global Transaction Identifier
 * @param[in,out] aEnv          Environment 구조체
 */

stlStatus smxlRecoverLock4PreparedTransaction( smxlTransId   aTransId,
                                               stlBool       aRecoverLock,
                                               stlXid      * aXid,
                                               smlEnv      * aEnv )
{
    smxlUndoRelEntry * sUndoRelEntry;
    smxlUndoRelCache * sUndoRelCache;
    stlChar          * sUndoRecord;
    smxlUndoRecHdr     sUndoRecHdr;
    smlTbsId           sTbsId;
    smlPid             sPageId;
    stlInt16           sSlotSeq;
    void             * sSegIterator;
    stlBool            sExit = STL_FALSE;
    stlInt32           sState = 0;
    smpHandle          sPageHandle;
    smklLockLog        sLockLog;
    smklItemLog      * sItemLog;
    void             * sRelationHandle;
    stlInt32           i;
    stlInt32           sRecordOffset;
    stlBool            sTableExist = STL_TRUE;
    smlStatement     * sStatement;
    

    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );

    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( aTransId );

    STL_TRY_THROW( sUndoRelEntry != NULL, RAMP_SUCCESS );
    
    sUndoRelCache = (smxlUndoRelCache*)smxlGetUndoRelHandle( sUndoRelEntry );

    sTbsId = smsGetTbsId( sUndoRelCache->mSegHandle );
    
    STL_TRY( smaAllocStatement( SML_INVALID_TRANSID,
                                SML_INVALID_TRANSID,
                                NULL,
                                SML_STMT_ATTR_READONLY,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE,
                                & sStatement,
                                aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smsAllocSegIterator( sStatement,
                                  sUndoRelCache->mSegHandle,
                                  &sSegIterator,
                                  aEnv ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( smsGetFirstPage( sUndoRelCache->mSegHandle,
                              sSegIterator,
                              &sPageId,
                              aEnv ) == STL_SUCCESS );
    
    while( sPageId != SMP_NULL_PID )
    {
        STL_TRY( smpFix( sTbsId,
                         sPageId,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 3;
        
        sSlotSeq = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle ) - 1;

        while( sSlotSeq >= 0 )
        {
            /**
             * Hole record가 존재할수 있기 때문에 사용되지 않은 slot은 skip한다.
             */
            if( smpIsNthOffsetSlotFree( &sPageHandle, sSlotSeq ) == STL_TRUE )
            {
                sSlotSeq--;
                continue;
            }
            
            sUndoRecord = smpGetNthRowItem( &sPageHandle,
                                            sSlotSeq );
            
            sRecordOffset = 0;
            SMXL_READ_MOVE_UNDO_REC_HDR( &sUndoRecHdr,
                                         sUndoRecord,
                                         sRecordOffset );

            if( (sUndoRecHdr.mComponentClass != SMG_COMPONENT_TRANSACTION) ||
                (sUndoRecHdr.mType != SMXL_UNDO_LOG_LOCK_FOR_PREPARE) )
            {
                sExit = STL_TRUE;
                break;
            }

            stlMemcpy( &sLockLog, sUndoRecord + sRecordOffset, sUndoRecHdr.mSize );
            stlMemcpy( aXid, &sLockLog.mXid, STL_SIZEOF(stlXid) );

            if( aRecoverLock == STL_TRUE )
            {
                for( i = 0; i < sLockLog.mLockCount; i++ )
                {
                    sItemLog = &sLockLog.mLockItemArray[i];

                    if( sItemLog->mItemType == SMKL_ITEM_RELATION )
                    {
                        STL_TRY( smeGetRelationHandle( sItemLog->mRid,
                                                       &sRelationHandle,
                                                       aEnv )
                                 == STL_SUCCESS );
                    
                        STL_TRY( smklLock( aTransId,
                                           sItemLog->mGrantedMode,
                                           sRelationHandle,
                                           SML_LOCK_TIMEOUT_INFINITE,
                                           &sTableExist,
                                           aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_TRY( smklLockRecord( aTransId,
                                                 sItemLog->mGrantedMode,
                                                 sItemLog->mRid,
                                                 SML_LOCK_TIMEOUT_INFINITE,
                                                 aEnv )
                                 == STL_SUCCESS );
                    }
                }
            }
            else
            {
                sExit = STL_TRUE;
                break;
            }

            sSlotSeq--;
        }

        sState = 2;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
        
        if( sExit == STL_TRUE )
        {
            break;
        }
        
        STL_TRY( smsGetNextPage( sUndoRelCache->mSegHandle,
                                 sSegIterator,
                                 &sPageId,
                                 aEnv ) == STL_SUCCESS );
    }

    sState = 1;
    STL_TRY( smsFreeSegIterator( sUndoRelCache->mSegHandle,
                                 sSegIterator,
                                 aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smaFreeStatement( sStatement,
                               aEnv ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void)smpUnfix( &sPageHandle, aEnv );
        case 2:
            (void)smsFreeSegIterator( sUndoRelCache->mSegHandle,
                                      sSegIterator,
                                      aEnv );
        case 1:
            (void)smaFreeStatement( sStatement, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief Undo Record를 기록한다.
 * @param[in] aTransId          Transaction Identifier
 * @param[in] aLowUndoRid       Rollback 구간을 지정하기 위한 RID
 * @param[in] aRestartRollback  Restart과정중의 Rollback 여부
 * @param[in,out] aEnv    Environment 구조체
 */

stlStatus smxlRollbackUndoRecords( smxlTransId   aTransId,
                                   smlRid        aLowUndoRid,
                                   stlBool       aRestartRollback,
                                   smlEnv      * aEnv )
{
    smxlUndoRelEntry * sUndoRelEntry;
    smxlUndoRelCache * sUndoRelCache;
    stlChar          * sUndoRecord;
    smxlUndoRecHdr     sUndoRecHdr;
    stlInt32           sRecordOffset;
    smlTbsId           sTbsId;
    smlPid             sPageId;
    stlInt16           sSlotSeq;
    void             * sSegIterator;
    stlBool            sExit = STL_FALSE;
    stlInt32           sState = 0;
    smpHandle          sPageHandle;
    smxmTrans          sMiniTrans;
    smxlUndoFunc     * sUndoFuncs;
    smlRid             sTargetSegRid;
    stlInt64           sSegmentId;
    stlInt32           sLogAttr;
    smxlTransId        sTransId;
    stlBool            sIncompleteRecovery = smrProceedIncompleteMediaRecovery( aEnv );
    smlStatement     * sStatement;
#ifdef STL_DEBUG
    smxlTransRecord    sTransRecord;
#endif

    knlDisableSessionEvent( KNL_ENV(aEnv) );
    sState = 1;
    
    STL_TRY_THROW( aTransId != SML_INVALID_TRANSID, RAMP_SUCCESS );

    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( aTransId );

    STL_TRY_THROW( sUndoRelEntry != NULL, RAMP_SUCCESS );
    
    sUndoRelCache = (smxlUndoRelCache*)smxlGetUndoRelHandle( sUndoRelEntry );

    sTbsId = smsGetTbsId( sUndoRelCache->mSegHandle );
    sTransId = smxlGetTransId( aTransId );
    
    STL_TRY( smaAllocStatement( SML_INVALID_TRANSID,
                                SML_INVALID_TRANSID,
                                NULL,
                                SML_STMT_ATTR_READONLY,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE,
                                & sStatement,
                                aEnv ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( smsAllocSegIterator( sStatement,
                                  sUndoRelCache->mSegHandle,
                                  &sSegIterator,
                                  aEnv ) == STL_SUCCESS );
    sState = 3;
    
    STL_TRY( smsGetFirstPage( sUndoRelCache->mSegHandle,
                              sSegIterator,
                              &sPageId,
                              aEnv ) == STL_SUCCESS );

    sLogAttr = (sIncompleteRecovery == STL_TRUE)? SMXM_ATTR_NOLOG_DIRTY: SMXM_ATTR_REDO;

    while( sPageId != SMP_NULL_PID )
    {
        STL_TRY( smpFix( sTbsId,
                         sPageId,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
        
        sSlotSeq = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle ) - 1;

        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
        
        while( sSlotSeq >= 0 )
        {
            /**
             * 특정 지점까지 롤백하는 경우라면( ROLLBACK TO SAVEPOINT, Statement Rollback )
             */
            if( (aLowUndoRid.mPageId == sPageId) && (aLowUndoRid.mOffset == sSlotSeq ) )
            {
                sExit = STL_TRUE;
                break;
            }
            
            STL_TRY( smxmBegin( &sMiniTrans,
                                ( (sIncompleteRecovery == STL_TRUE)?
                                  SML_INVALID_TRANSID : sTransId ),
                                SML_INVALID_RID,
                                sLogAttr,
                                aEnv ) == STL_SUCCESS );
            sState = 4;
            
            STL_TRY( smpAcquire( &sMiniTrans,
                                 sTbsId,
                                 sPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );

            /**
             * Hole record가 존재할수 있기 때문에 사용되지 않은 slot은 skip한다.
             */
            if( smpIsNthOffsetSlotFree( &sPageHandle, sSlotSeq ) == STL_TRUE )
            {
                sSlotSeq--;
                
                sState = 3;
                STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
                
                continue;
            }
            
            sUndoRecord = smpGetNthRowItem( &sPageHandle,
                                            sSlotSeq );

            sRecordOffset = 0;
            SMXL_READ_MOVE_UNDO_REC_HDR( &sUndoRecHdr,
                                         sUndoRecord,
                                         sRecordOffset );

            /**
             * No logging으로 기록되었다면 기록될 당시의 attribute로 설정한다.
             */
            if( sUndoRecHdr.mRedoLogged == STL_FALSE )
            {
                /**
                 * no-logged undo record는 restart 중에 rollback해서는 안된다.
                 */
                if( aRestartRollback == STL_TRUE )
                {
                    sSlotSeq--;
                
                    sState = 3;
                    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
                
                    continue;
                }
                
                STL_TRY( smxmResetAttr( &sMiniTrans,
                                        SMXM_ATTR_NOLOG_DIRTY,
                                        aEnv )
                         == STL_SUCCESS );
            }

            if( sUndoRecHdr.mFlag & SMXL_TRANSACTION_RECORD )
            {

#ifdef STL_DEBUG
                STL_READ_MOVE_BYTES( &sTransRecord,
                                     sUndoRecord,
                                     STL_SIZEOF( smxlTransRecord ),
                                     sRecordOffset );
                STL_ASSERT( sTransRecord.mTransId == sTransId );
#endif
                
                sExit = STL_TRUE;
                
                sState = 3;
                STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
                
                break;
            }

            /**
             * 이미 지워진 경우는 Skip한다.
             */
            if( sUndoRecHdr.mFlag & SMXL_UNDO_RECORD_DELETED )
            {
                sSlotSeq--;
                
                sState = 3;
                STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
                
                continue;
            }
            
            SML_MAKE_RID( &sTargetSegRid,
                          SML_MEMORY_DICT_SYSTEM_TBS_ID,
                          sUndoRecHdr.mTargetSegPid,
                          0 );

            smxmSetSegmentRid( &sMiniTrans, sTargetSegRid );
            
            sUndoRecHdr.mFlag |= SMXL_UNDO_RECORD_DELETED;
            SMXL_WRITE_UNDO_REC_DELETE_FLAG( sUndoRecord,
                                             &sUndoRecHdr );

            if( sIncompleteRecovery != STL_TRUE )
            {
                if( sUndoRecHdr.mRedoLogged == STL_TRUE )
                {
                    STL_TRY( smxmWriteLog( &sMiniTrans,
                                           SMG_COMPONENT_TRANSACTION,
                                           SMR_LOG_UPDATE_UNDO_RECORD_FLAG,
                                           SMR_REDO_TARGET_PAGE,
                                           (void*)&sUndoRecHdr.mFlag,
                                           STL_SIZEOF( sUndoRecHdr.mFlag ),
                                           sTbsId,
                                           sPageId,
                                           sSlotSeq,
                                           SMXM_LOGGING_REDO,
                                           aEnv ) == STL_SUCCESS );
                }
            }
            
            sUndoFuncs = gRecoveryUndoFuncs[(stlInt16)sUndoRecHdr.mComponentClass];
            STL_ASSERT( sUndoFuncs != NULL );

            /**
             * Segment를 가질수 있는 Component라면 
             */

            if( sUndoRecHdr.mComponentClass > SMG_COMPONENT_RELATION )
            {
                /**
                 * Tablespace Offline 인 경우는 Pending Operation으로 등록한다.
                 * - Target Record/Key의 Tablespace가 Offline인 경우
                 */

                if( SMF_IS_ONLINE_TBS( sUndoRecHdr.mTargetDataRid.mTbsId ) == STL_TRUE )
                {
                    /**
                     * Irrecoverable Segment는 rollback 하지 않는다.
                     * - Relation 이상의 Undo Record에만 해당됨
                     */
                    if( aRestartRollback == STL_TRUE )
                    {
                        SMS_MAKE_SEGMENT_ID( &sSegmentId, &sTargetSegRid );
                    
                        if( smsIsIrrecoverableSegment( sSegmentId ) == STL_FALSE )
                        {
                            STL_TRY( sUndoFuncs[sUndoRecHdr.mType]( &sMiniTrans,
                                                                    sUndoRecHdr.mTargetDataRid,
                                                                    sUndoRecord + sRecordOffset,
                                                                    sUndoRecHdr.mSize,
                                                                    aEnv )
                                     == STL_SUCCESS );
                        }
                        else
                        {
                            STL_TRY( knlLogMsg( NULL, /* aLogger */
                                                KNL_ENV(aEnv),
                                                KNL_LOG_LEVEL_INFO,
                                                "[ROLLBACK] skip rollback record on unusable segment"
                                                " ( segment id : %ld, rid : [%d,%d,%d] )",
                                                sSegmentId,
                                                sUndoRecHdr.mTargetDataRid.mTbsId,
                                                sUndoRecHdr.mTargetDataRid.mPageId,
                                                sUndoRecHdr.mTargetDataRid.mOffset )
                                     == STL_SUCCESS );
                        }
                    }
                    else
                    {
                        STL_TRY( sUndoFuncs[sUndoRecHdr.mType]( &sMiniTrans,
                                                                sUndoRecHdr.mTargetDataRid,
                                                                sUndoRecord + sRecordOffset,
                                                                sUndoRecHdr.mSize,
                                                                aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else
                {
                    STL_TRY( smdInsertPendingOperation( sTransId,
                                                        sUndoRecHdr.mTargetDataRid.mTbsId,
                                                        SMD_PENDING_OPERATION_UNDO,
                                                        sUndoRecord,
                                                        sRecordOffset + sUndoRecHdr.mSize,
                                                        aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                STL_TRY( sUndoFuncs[sUndoRecHdr.mType]( &sMiniTrans,
                                                        sUndoRecHdr.mTargetDataRid,
                                                        sUndoRecord + sRecordOffset,
                                                        sUndoRecHdr.mSize,
                                                        aEnv )
                         == STL_SUCCESS );
            }
            
            sSlotSeq--;

            sState = 3;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
        }

        if( sExit == STL_TRUE )
        {
            break;
        }
        
        STL_TRY( smsGetNextPage( sUndoRelCache->mSegHandle,
                                 sSegIterator,
                                 &sPageId,
                                 aEnv ) == STL_SUCCESS );
    }

    sState = 2;
    STL_TRY( smsFreeSegIterator( sUndoRelCache->mSegHandle,
                                 sSegIterator,
                                 aEnv ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( smaFreeStatement( sStatement,
                               aEnv ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );

    sState = 0;
    knlEnableSessionEvent( KNL_ENV(aEnv) );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 4:
            (void)smxmRollback( &sMiniTrans, aEnv );
        case 3:
            (void)smsFreeSegIterator( sUndoRelCache->mSegHandle,
                                      sSegIterator,
                                      aEnv );
        case 2:
            (void)smaFreeStatement( sStatement, aEnv );
        case 1:
            knlEnableSessionEvent( KNL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 RID로 부터 Undo Record Body를 얻는다.
 * @param[in] aPageHandle Undo Record가 저장되어 있는 페이지의 Page Handle
 * @param[in] aUndoRid Undo Record의 물리적 위치
 * @param[out] aUndoRecType Undo Record의 타입
 * @param[out] aUndoRecClass Undo Record의 클래스
 * @param[out] aUndoRecBody Undo Record data의 포인터
 * @param[out] aUndoRecBodySize Undo Record data의 길이
 */
inline stlStatus smxlGetUndoRecord( smpHandle  * aPageHandle,
                                    smlRid     * aUndoRid,
                                    stlInt16   * aUndoRecType,
                                    stlInt8    * aUndoRecClass,
                                    stlChar   ** aUndoRecBody,
                                    stlInt16   * aUndoRecBodySize )
{
    stlChar * sUndoRecord;
    
    sUndoRecord = smpGetNthRowItem( aPageHandle, aUndoRid->mOffset );

    SMXL_READ_UNDO_REC_TYPE( aUndoRecType, sUndoRecord );
    SMXL_READ_UNDO_REC_CLASS( aUndoRecClass, sUndoRecord );
    *aUndoRecBody = sUndoRecord + SMXL_UNDO_REC_HDR_SIZE;
    SMXL_READ_UNDO_REC_SIZE( aUndoRecBodySize, sUndoRecord );

    return STL_SUCCESS;
}
                             

/**
 * @brief 주어진 RID로 부터 Undo Record Body를 unsafe하게 얻는다.
 * @param[in] aPageHandle Undo Record가 저장되어 있는 페이지의 Page Handle
 * @param[in] aUndoRid Undo Record의 물리적 위치
 * @param[in] aUndoRecType Undo Record의 타입
 * @param[in] aUndoRecClass Undo Record의 클래스
 * @param[in] aUndoRecBuf Undo Record를 가져갈 버퍼
 * @param[in] aUndoRecBufSize aUndoRecBuf의 크기
 * @param[out] aIsSuccess 주어진 타입의 undo record가 맞는지
 * @param[out] aUndoRecSize Undo Record data의 실제 길이
 */
inline stlStatus smxlGetUnsafeUndoRecord( smpHandle * aPageHandle,
                                          smlRid    * aUndoRid,
                                          stlInt16    aUndoRecType,
                                          stlInt8     aUndoRecClass,
                                          stlChar   * aUndoRecBuf,
                                          stlInt16    aUndoRecBufSize,
                                          stlBool   * aIsSuccess,
                                          stlInt16  * aUndoRecSize )
{
    stlChar  * sUndoRecord;
    stlInt16   sRowItemCount;
    stlInt16   sUndoRecType;
    stlInt8    sUndoRecClass;
    stlInt16   sUndoRecBodySize;

    *aIsSuccess = STL_FALSE;

    sRowItemCount = SMP_GET_PAGE_ROW_ITEM_COUNT( aPageHandle );

    /**
     * 페이지가 재사용되었다.
     */
    STL_TRY_THROW( sRowItemCount > aUndoRid->mOffset, RAMP_SKIP );

    sUndoRecord = smpGetUnsafeNthRowItem( aPageHandle,
                                          aUndoRid->mOffset );

    SMXL_READ_UNDO_REC_TYPE( &sUndoRecType, sUndoRecord );
    SMXL_READ_UNDO_REC_CLASS( &sUndoRecClass, sUndoRecord );
    SMXL_READ_UNDO_REC_SIZE( &sUndoRecBodySize, sUndoRecord );

    /**
     * Undo Record의 위치가 Page 범위를 넘어서면 재사용되었다고
     * 판단한다.
     */
    STL_TRY_THROW( ((stlChar*)SMP_FRAME(aPageHandle) < sUndoRecord) &&
                   ((stlChar*)SMP_FRAME(aPageHandle) + SMP_PAGE_SIZE - sUndoRecBodySize > sUndoRecord),
                   RAMP_SKIP );

    STL_TRY_THROW( (sUndoRecType == aUndoRecType) &&
                   (sUndoRecClass == aUndoRecClass) &&
                   (sUndoRecBodySize <= aUndoRecBufSize),
                   RAMP_SKIP );

    stlMemcpy( aUndoRecBuf, sUndoRecord + SMXL_UNDO_REC_HDR_SIZE, sUndoRecBodySize );
    *aUndoRecSize = sUndoRecBodySize;

    /**
     * 페이지가 Tablespace로 반납되었다가 재생성된 경우라면
     */
    STL_TRY_THROW( ((smpPhyHdr*)SMP_FRAME(aPageHandle))->mPageType == SMP_PAGE_TYPE_UNDO,
                   RAMP_SKIP );

    *aIsSuccess = STL_TRUE;

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;
}


smlRid smxlGetUndoSegRidFromLog( stlChar * aUndoRecord )
{
    smxlUndoRecHdr sUndoRecHdr;
    smlRid         sUndoSegRid;

    SMXL_READ_UNDO_REC_HDR( &sUndoRecHdr,
                            aUndoRecord );

    SML_MAKE_RID( &sUndoSegRid,
                  SML_MEMORY_DICT_SYSTEM_TBS_ID,
                  sUndoRecHdr.mUndoSegPid,
                  0 );

    return sUndoSegRid;
}

smlScn smxlGetTransViewScnFromUndoLog( stlChar * aUndoRecord )
{
    smxlUndoRecHdr  sUndoRecHdr;
    smxlTransRecord sTransRecord;
    stlInt32        sLogOffset = 0;

    SMXL_READ_MOVE_UNDO_REC_HDR( &sUndoRecHdr,
                                 aUndoRecord,
                                 sLogOffset );


    STL_READ_MOVE_BYTES( &sTransRecord,
                         aUndoRecord,
                         STL_SIZEOF( smxlTransRecord ),
                         sLogOffset );

    return sTransRecord.mTransViewScn;
}

smlRid smxlGetLstUndoRid( smxlTransId aTransId )
{
    smxlUndoRelEntry * sUndoRelEntry;
    smxlUndoRelCache * sUndoRelCache;
    smlRid             sUndoRid;
    
    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( aTransId );

    if( sUndoRelEntry == NULL )
    {
        sUndoRid = SML_INVALID_RID;
    }
    else
    {
        sUndoRelCache = (smxlUndoRelCache*)smxlGetUndoRelHandle( sUndoRelEntry );
        STL_ASSERT( sUndoRelCache != NULL );
        sUndoRid = sUndoRelCache->mTransLstUndoRid;
    }

    return sUndoRid;
}

/** @} */

/**
 * @addtogroup smxlUndoRelation
 * @{
 */

stlStatus smxlInsertTransRecordInternal( smxmTrans * aMiniTrans,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv )
{
    smxlUndoRelCache * sUndoRelCache;
    smxlTransId        sTransId;
    smxmSavepoint      sSavepoint;
    stlBool            sIsNewPage = STL_FALSE;
    stlChar          * sUndoRecord;
    smxlUndoRecHdr     sUndoRecHdr;
    stlInt32           sLogOffset = 0;
    stlInt16           sSlotSeq;
    smpHandle          sPageHandle;
    smxlUndoRelEntry * sUndoRelEntry;
    smlRid             sUndoSegRid;
    void             * sData;
    stlInt16           sDataSize;
    smxlTransRecord    sTransRecord;
    stlInt64         * sRecycleNo;
    stlInt32           sState = 0;
    smlScn             sMinTransViewScn;
    smlTbsId           sTbsId;
    smlPid             sPid;

    sTransId = smxmGetTransId( aMiniTrans );
    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( sTransId );
    STL_ASSERT( sUndoRelEntry != NULL );
    sUndoRelCache = (smxlUndoRelCache*)smxlGetUndoRelHandle( sUndoRelEntry );

    STL_ASSERT( smxlIsTransRecordRecorded( sTransId ) == STL_FALSE );
    
    sDataSize = STL_SIZEOF( smxlTransRecord );
    sData = (void*)&sTransRecord;
    sTransRecord.mCommitScn = SML_INFINITE_SCN;
    sTransRecord.mTransViewScn = smxlGetTransViewScn( sTransId );
    sTransRecord.mTransId = sTransId;
    sTransRecord.mGlobalTrans = smxlIsGlobalTrans( sTransId );
    sTransRecord.mCommitTime = 0;
    sTransRecord.mCommentSize = 0;
    stlMemset( sTransRecord.mPadding, 0x00, 5 );
    
    STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                &sSavepoint,
                                aEnv ) == STL_SUCCESS );
    
    STL_TRY( smsGetInsertablePage( aMiniTrans,
                                   sUndoRelCache->mSegHandle,
                                   SMP_PAGE_TYPE_UNDO,
                                   NULL,  /* aIsAgableFunc */
                                   NULL,   /* aSeachHint */
                                   &sPageHandle,
                                   &sIsNewPage,
                                   aEnv ) == STL_SUCCESS );

    sRecycleNo = (stlInt64*)smpGetVolatileArea( &sPageHandle );
    sRecycleNo[0] += 1;
    stlMemBarrier();
    sState = 1;
        
    while( 1 )
    {
        if( sIsNewPage == STL_TRUE )
        {
            STL_TRY( smpInitBody( &sPageHandle,
                                  SMP_ORDERED_SLOT_TYPE,
                                  0,
                                  NULL,
                                  0,
                                  0,
                                  STL_TRUE,  /* aUseFreeSlotList */
                                  aMiniTrans,
                                  aEnv ) == STL_SUCCESS );

            STL_TRY( smpUpdateAgableScn( aMiniTrans,
                                         &sPageHandle,
                                         SML_INFINITE_SCN,
                                         aEnv )
                     == STL_SUCCESS );

            smxlIncUndoPageCount( sTransId );
        }

        STL_TRY( smpAllocSlot4Undo( &sPageHandle,
                                    sDataSize + STL_SIZEOF( smxlUndoRecHdr ),
                                    STL_FALSE, /* aIsForTest */
                                    &sUndoRecord,
                                    &sSlotSeq ) == STL_SUCCESS );

        STL_ASSERT( (sIsNewPage != STL_TRUE) || (sUndoRecord != NULL) );

        if( sUndoRecord == NULL )
        {
            sState = 0;
            stlMemBarrier();
            sRecycleNo[1] += 1;
    
            STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                              &sSavepoint,
                                              aEnv ) == STL_SUCCESS );

            STL_TRY( smxlAllocPage( aMiniTrans,
                                    sUndoRelCache,
                                    &sPageHandle,
                                    aEnv )
                     == STL_SUCCESS );

            sIsNewPage = STL_TRUE;
            
            sRecycleNo = (stlInt64*)smpGetVolatileArea( &sPageHandle );
            sRecycleNo[0] += 1;
            stlMemBarrier();
            sState = 1;
        }
        else
        {
            break;
        }
    }

    sTbsId = smsGetTbsId( sUndoRelCache->mSegHandle );
    sPid = smpGetPageId( &sPageHandle );
    SML_MAKE_RID( &sUndoRelCache->mTransLstUndoRid,
                  sTbsId,
                  sPid,
                  sSlotSeq );
    sUndoSegRid = smsGetSegRid( sUndoRelCache->mSegHandle );
    sUndoRecHdr.mType = SMR_LOG_INSERT_TRANSACTION_RECORD;
    sUndoRecHdr.mComponentClass = SMG_COMPONENT_TRANSACTION;
    sUndoRecHdr.mTargetDataRid = sUndoRelCache->mTransLstUndoRid;
    sUndoRecHdr.mTargetSegPid = sUndoSegRid.mPageId;
    sUndoRecHdr.mUndoSegPid = sUndoSegRid.mPageId;
    sUndoRecHdr.mSize = sDataSize;
    sUndoRecHdr.mFlag = SMXL_TRANSACTION_RECORD;
    sUndoRecHdr.mRedoLogged = STL_FALSE;
    sUndoRecHdr.mPadding[0] = 0;

    sTransId = SMXL_MAKE_TRANS_ID( sUndoRelCache->mTransLstUndoRid.mPageId,
                                   sUndoRelCache->mTransLstUndoRid.mOffset,
                                   SMXL_TRANS_SLOT_ID( sTransId ) );
    
    if( (smxmGetAttr(aMiniTrans) & SMXM_ATTR_REDO) == SMXM_ATTR_REDO )
    {
        sUndoRecHdr.mRedoLogged = STL_TRUE;
    }
    
    SMXL_WRITE_MOVE_UNDO_REC_HDR( sUndoRecord,
                                  &sUndoRecHdr,
                                  sLogOffset );

    sTransRecord.mTransId = sTransId;
    STL_WRITE_MOVE_BYTES( sUndoRecord,
                          sData,
                          sDataSize,
                          sLogOffset );

    sMinTransViewScn = smpGetAgableScn( &sPageHandle );

    if( sTransRecord.mTransViewScn < sMinTransViewScn )
    {
        /**
         * Redo Logging을 하지 않는다.
         * - SMR_LOG_INSERT_TRANSACTION_RECORD에서 Physio-logical Redo을 수행한다.
         */
        STL_TRY( smpUpdateAgableScn( NULL,  /* aMiniTrans */
                                     &sPageHandle,
                                     sTransRecord.mTransViewScn,
                                     aEnv )
                 == STL_SUCCESS );
    }

    sState = 0;
    stlMemBarrier();
    sRecycleNo[1] += 1;

    if( (smxmGetAttr(aMiniTrans) & SMXM_ATTR_REDO) == SMXM_ATTR_REDO )
    {
        /**
         * No logging table/index에 대해서는 로그를 기록하지 않는다
         */
    
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_TRANSACTION,
                               SMR_LOG_INSERT_TRANSACTION_RECORD,
                               SMR_REDO_TARGET_UNDO_SEGMENT,
                               sUndoRecord,
                               sLogOffset,
                               smsGetTbsId( sUndoRelCache->mSegHandle ),
                               smpGetPageId( &sPageHandle ),
                               sSlotSeq,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    /*
     * Transaction Id에 물리적 정보 설정
     */
    smxlUpdateTransId( sTransId, aEnv );
    smxlUpdateBeginLsn( sTransId, aEnv );
    smxlSetUndoSegRid( sTransId, sUndoSegRid );

    STL_TRY( smaUpdateTransIdOnStmt( sTransId,
                                     aEnv ) == STL_SUCCESS );
    
    sUndoRelCache->mTransId = sTransId;

    /**
     * 마지막 할당된 Transaction 정보를 Segment에 기록한다.
     */
    STL_TRY( smsSetTransToHandle( sUndoRelCache->mSegHandle,
                                  sTransId,
                                  smxlGetTransViewScn( sTransId ),
                                  aEnv )
             == STL_SUCCESS );

    *aPageHandle = sPageHandle;

    /**
     * Transaction Record가 기록되었음을 선언한다.
     */
    smxlSetTransRecordRecorded( sTransId );
    
    STL_ASSERT( sRecycleNo[0] == sRecycleNo[1] );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        sRecycleNo[1] += 1;
    }

    return STL_FAILURE;
}

stlStatus smxlPrepareTransRecordInternal( smxmTrans   * aMiniTrans,
                                          smxlTransId * aTransId,
                                          smlEnv      * aEnv )
{
    smxlUndoRelCache * sUndoRelCache;
    smxlTransId        sTransId;
    smxmSavepoint      sSavepoint;
    stlBool            sIsNewPage = STL_FALSE;
    stlChar          * sUndoRecord;
    stlInt16           sSlotSeq;
    smpHandle          sPageHandle;
    smxlUndoRelEntry * sUndoRelEntry;
    stlInt64         * sRecycleNo;
    stlInt32           sState = 0;
    
    sTransId = smxmGetTransId( aMiniTrans );
    sUndoRelEntry = (smxlUndoRelEntry*)smxlGetUndoRelEntry( sTransId );
    STL_ASSERT( sUndoRelEntry != NULL );
    sUndoRelCache = (smxlUndoRelCache*)smxlGetUndoRelHandle( sUndoRelEntry );

    STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                &sSavepoint,
                                aEnv ) == STL_SUCCESS );
    
    STL_TRY( smsGetInsertablePage( aMiniTrans,
                                   sUndoRelCache->mSegHandle,
                                   SMP_PAGE_TYPE_UNDO,
                                   NULL,  /* aIsAgableFunc */
                                   NULL,   /* aSeachHint */
                                   &sPageHandle,
                                   &sIsNewPage,
                                   aEnv ) == STL_SUCCESS );

    sRecycleNo = (stlInt64*)smpGetVolatileArea( &sPageHandle );
    sRecycleNo[0] += 1;
    stlMemBarrier();
    sState = 1;
        
    while( 1 )
    {
        if( sIsNewPage == STL_TRUE )
        {
            STL_TRY( smpInitBody( &sPageHandle,
                                  SMP_ORDERED_SLOT_TYPE,
                                  0,
                                  NULL,
                                  0,
                                  0,
                                  STL_TRUE,  /* aUseFreeSlotList */
                                  aMiniTrans,
                                  aEnv ) == STL_SUCCESS );

            STL_TRY( smpUpdateAgableScn( aMiniTrans,
                                         &sPageHandle,
                                         SML_INFINITE_SCN,
                                         aEnv )
                     == STL_SUCCESS );
            
            smxlIncUndoPageCount( sTransId );
        }

        STL_TRY( smpAllocSlot4Undo( &sPageHandle,
                                    STL_SIZEOF( smxlTransRecord ) + STL_SIZEOF( smxlUndoRecHdr ),
                                    STL_TRUE, /* aIsForTest */
                                    &sUndoRecord,
                                    &sSlotSeq ) == STL_SUCCESS );

        if( sUndoRecord == NULL )
        {
            sState = 0;
            stlMemBarrier();
            sRecycleNo[1] += 1;
    
            STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                              &sSavepoint,
                                              aEnv ) == STL_SUCCESS );

            STL_TRY( smxlAllocPage( aMiniTrans,
                                    sUndoRelCache,
                                    &sPageHandle,
                                    aEnv )
                     == STL_SUCCESS );

            sIsNewPage = STL_TRUE;
            
            sRecycleNo = (stlInt64*)smpGetVolatileArea( &sPageHandle );
            sRecycleNo[0] += 1;
            stlMemBarrier();
            sState = 1;
        }
        else
        {
            break;
        }
    }

    sState = 0;
    stlMemBarrier();
    sRecycleNo[1] += 1;
    
    /*
     * Transaction Id에 물리적 정보 설정
     */
    sTransId = SMXL_MAKE_TRANS_ID( smpGetPageId( &sPageHandle ),
                                   sSlotSeq,
                                   SMXL_TRANS_SLOT_ID( sTransId ) );

    STL_TRY( smaUpdateTransIdOnStmt( sTransId,
                                     aEnv ) == STL_SUCCESS );
    
    sUndoRelCache->mTransId = sTransId;

    /**
     * 마지막 할당된 Transaction 정보를 Segment에 기록한다.
     */
    STL_TRY( smsSetTransToHandle( sUndoRelCache->mSegHandle,
                                  sTransId,
                                  smxlGetTransViewScn( sTransId ),
                                  aEnv )
             == STL_SUCCESS );

    *aTransId = sTransId;

    STL_ASSERT( sRecycleNo[0] == sRecycleNo[1] );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        sRecycleNo[1] += 1;
    }

    return STL_FAILURE;
}

stlStatus smxlAllocPage( smxmTrans        * aMiniTrans,
                         smxlUndoRelCache * aUndoRelCache,
                         smpHandle        * aPageHandle,
                         smlEnv           * aEnv )
{
    stlStatus  sStatus;
    stlInt64   sStolenPageCount = 0;
#ifdef STL_DEBUG
    stlInt32   sErrorStackDepth = 0;
#endif
    
    while( 1 )
    {
        sStatus = smsAllocPage( aMiniTrans,
                                aUndoRelCache->mSegHandle,
                                SMP_PAGE_TYPE_UNDO,
                                NULL, /* aIsAgableFunc */
                                aPageHandle,
                                aEnv );

        if( sStatus == STL_FAILURE )
        {
            if( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) )
                == SML_ERRCODE_NO_MORE_EXTENDIBLE_DATAFILE )
            {
#ifdef STL_DEBUG
                sErrorStackDepth = stlGetErrorStackDepth( KNL_ERROR_STACK(aEnv) );
#endif
                
                /**
                 * Undo Space를 확보할수 없다면 다른 세그먼트에서 steal한다.
                 */
                STL_TRY( smxlStealPages( aMiniTrans,
                                         aUndoRelCache,
                                         &sStolenPageCount,
                                         aEnv )
                         == STL_SUCCESS );

                /**
                 * Steal 연산이 실패하면 에러를 발생시킨다.
                 */
                if( sStolenPageCount == 0 )
                {
                    (void) knlLogMsg( NULL,
                                      KNL_ENV( aEnv ),
                                      KNL_LOG_LEVEL_WARNING,
                                      "[TABLESPACE] datafile extending was failed\n" );
                    STL_TRY( STL_FALSE );
                }

                STL_DASSERT( sErrorStackDepth == stlGetErrorStackDepth( KNL_ERROR_STACK(aEnv) ) );
            
                /**
                 * 공간 확보를 성공했다면 기존 에러를 버리고 재시도 한다.
                 */
                (void)stlPopError( KNL_ERROR_STACK(aEnv) );

                continue;
            }
                    
            /**
             * 공간에 관련된 문제가 아니라면 pass
             */
            STL_TRY( 0 );
        }

        /**
         * New Page 할당에 성공함
         */
        break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smxlStealPages( smxmTrans        * aMiniTrans,
                          smxlUndoRelCache * aUndoRelCache,
                          stlInt64         * aStolenPageCount,
                          smlEnv           * aEnv )
{
    stlInt32           i;
    stlBool            sIsSuccess;
    smxlUndoRelCache * sUndoRelCache;
    stlInt64           sStolenPageCount = 0;
    stlInt64           sAllocPageCount = 0;
    stlInt32           sLatchState = 0;
    stlInt32           sFreeState = 0;
    stlInt64           sMinUndoPageCount;


    /**
     * Circular segment를 뒤지면서 가용한 Page들을 얻는다.
     */
    
    sMinUndoPageCount = knlGetPropertyBigIntValueByID( KNL_PROPERTY_MINIMUM_UNDO_PAGE_COUNT,
                                                       KNL_ENV(aEnv) );

    gSmxlWarmupEntry->mTryStealUndoCount++;
    
    for( i = 0; i < SMXL_UNDO_REL_COUNT; i++ )
    {
        if( SMXL_UNDO_REL_ENTRY(i).mIsFree == STL_FALSE )
        {
            continue;
        }

        sUndoRelCache = (smxlUndoRelCache*)SMXL_UNDO_REL_ENTRY(i).mRelHandle;
        sAllocPageCount = smsGetAllocPageCount( sUndoRelCache->mSegHandle );

        /**
         * 세그먼트의 할당된 페이지를 가져갈만큼 충분한 페이지를 소유하고 있지
         * 않다면 다른 세그먼트를 찾는다.
         */
        if( sAllocPageCount <= sMinUndoPageCount )
        {
            continue;
        }

        STL_TRY( knlTryLatch( &SMXL_UNDO_REL_ENTRY(i).mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              &sIsSuccess,
                              KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sIsSuccess == STL_FALSE )
        {
            continue;
        }
        sLatchState = 1;

        if( SMXL_UNDO_REL_ENTRY(i).mIsFree == STL_TRUE )
        {
            /**
             * 충분한 페이지를 가지고 있고, 현재 트랜잭션에 의해서 사용되고
             * 있지 않는 경우 steal 연산을 시도한다.
             */
            SMXL_UNDO_REL_ENTRY(i).mIsFree = STL_FALSE;
            sFreeState = 1;

            sLatchState = 0;
            STL_TRY( knlReleaseLatch( &SMXL_UNDO_REL_ENTRY(i).mLatch,
                                      (knlEnv*)aEnv )
                     == STL_SUCCESS );

            STL_TRY( smsStealPages( aMiniTrans,
                                    aUndoRelCache->mSegHandle,
                                    sUndoRelCache->mSegHandle,
                                    &sStolenPageCount,
                                    aEnv )
                     == STL_SUCCESS );
            
            sFreeState = 0;
            SMXL_UNDO_REL_ENTRY(i).mIsFree = STL_TRUE;
            
            /**
             * Steal 연산이 실패하면 다른 세그먼트에 시도한다.
             */
            if( sStolenPageCount == 0 )
            {
                continue;
            }

            break;
        }

        sLatchState = 0;
        STL_TRY( knlReleaseLatch( &SMXL_UNDO_REL_ENTRY(i).mLatch,
                                  (knlEnv*)aEnv )
                 == STL_SUCCESS );
    }

    *aStolenPageCount = sStolenPageCount;

    return STL_SUCCESS;

    STL_FINISH;

    if( sLatchState == 1 )
    {
        (void)knlReleaseLatch( &SMXL_UNDO_REL_ENTRY(i).mLatch, (knlEnv*)aEnv );
    }

    if( sFreeState == 1 )
    {
        SMXL_UNDO_REL_ENTRY(i).mIsFree = STL_TRUE;
    }
    
    return STL_FAILURE;
}
    
/** @} */

