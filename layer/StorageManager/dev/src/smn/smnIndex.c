/*******************************************************************************
 * smnTable.c
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
#include <sms.h>
#include <sme.h>
#include <smf.h>
#include <smn.h>
#include <smp.h>
#include <smxm.h>
#include <smxl.h>
#include <smt.h>
#include <smg.h>
#include <sma.h>
#include <smnDef.h>
#include <smnmpbDef.h>

/**
 * @file smnIndex.c
 * @brief Storage Manager Layer Index Manager Internal Routines
 */

/**
 * @addtogroup smn
 * @{
 */

extern smnIndexModule gSmnmpbModule;
extern smnIndexModule gSmnmihModule;

smnIndexModule * gSmnIndexModules[SML_INDEX_TYPE_MAX + 1] = 
{
    &gSmnmpbModule,    /* persistent btree */
    &gSmnmihModule,    /* instant hash index */    
    NULL               /* index max */
};


/**
 * @brief Index를 Access하기위한 함수 vecter를 반환한다.
 * @param[in] aIndexType Index의 타입
 */
inline smnIndexModule * smnGetIndexModule( smlIndexType aIndexType )
{
    return gSmnIndexModules[aIndexType];
}


/**
 * @brief Index header의 runtime 정보를 초기화 한다.
 * @param[in] aRelationHandle index 핸들
 * @param[in] aEnv Storage Manager Environment
 */
inline stlStatus smnInitVolatileInfo( void * aRelationHandle, smlEnv * aEnv )
{
    smnIndexHeader * sIdxHdr = (smnIndexHeader*)aRelationHandle;

    sIdxHdr->mIndexSmoNo = 0;
    sIdxHdr->mIsAgingEmptyNode = STL_FALSE;
    sIdxHdr->mBaseRelHandle = NULL;

    return STL_SUCCESS;
}

/**
 * @brief Index header에 root page id를 세팅한다.
 * @param[in] aMiniTrans logging시 사용할 mini transaction
 * @param[in] aRelationHandle index 핸들
 * @param[in] aNewRootPid 세팅할 root pid
 * @param[in] aEnv Storage Manager Environment
 */
inline stlStatus smnSetRootPageId( smxmTrans * aMiniTrans,
                                   void      * aRelationHandle,
                                   smlPid      aNewRootPid,
                                   smlEnv    * aEnv )
{
    smnIndexHeader * sHeader;
    smlRid           sSegRid;
    smpHandle        sPageHandle;
    stlChar          sLogBuf[SMP_PAGE_SIZE];
    stlChar        * sLogPtr;

    sSegRid = smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle));
    STL_TRY( smpAcquire( aMiniTrans,
                         sSegRid.mTbsId,
                         sSegRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sHeader = (smnIndexHeader*)((stlChar*)smpGetBody(SMP_FRAME(&sPageHandle)) +
                                smsGetHeaderSize(SME_GET_SEGMENT_HANDLE(aRelationHandle)));

    sHeader->mRootPageId = aNewRootPid;
    ((smnIndexHeader*)aRelationHandle)->mRootPageId = aNewRootPid;

    sLogPtr = sLogBuf;
    stlMemcpy( sLogPtr, &aNewRootPid, STL_SIZEOF(smlPid) );
    sLogPtr += STL_SIZEOF(smlPid);
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_SET_ROOT_PAGE_ID,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           sSegRid.mTbsId,
                           sSegRid.mPageId,
                           (stlChar*)sHeader - (stlChar*)SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Index header에 mirror root page id를 세팅한다.
 * @param[in] aMiniTrans logging시 사용할 mini transaction
 * @param[in] aRelationHandle index 핸들
 * @param[in] aNewRootPid 세팅할 root pid
 * @param[in] aEnv Storage Manager Environment
 */
inline stlStatus smnSetMirrorRootPageId( smxmTrans * aMiniTrans,
                                         void      * aRelationHandle,
                                         smlPid      aNewRootPid,
                                         smlEnv    * aEnv )
{
    smnIndexHeader * sHeader;
    smlRid           sSegRid;
    smpHandle        sPageHandle;
    stlChar          sLogBuf[SMP_PAGE_SIZE];
    stlChar        * sLogPtr;
    smpHandle      * sIndexSegHandle;

    sSegRid = smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle));

    sIndexSegHandle = smxmGetPageHandle( aMiniTrans,
                                         sSegRid.mTbsId,
                                         sSegRid.mPageId,
                                         KNL_LATCH_MODE_EXCLUSIVE );

    if( sIndexSegHandle == NULL )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             sSegRid.mTbsId,
                             sSegRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        sIndexSegHandle = &sPageHandle;
    }

    sHeader = (smnIndexHeader*)((stlChar*)smpGetBody(SMP_FRAME(sIndexSegHandle)) +
                                smsGetHeaderSize(SME_GET_SEGMENT_HANDLE(aRelationHandle)));

    sHeader->mMirrorRootPid = aNewRootPid;
    ((smnIndexHeader*)aRelationHandle)->mMirrorRootPid = aNewRootPid;

    sLogPtr = sLogBuf;
    stlMemcpy( sLogPtr, &aNewRootPid, STL_SIZEOF(smlPid) );
    sLogPtr += STL_SIZEOF(smlPid);
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_SET_MIRROR_ROOT_PAGE_ID,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           sSegRid.mTbsId,
                           sSegRid.mPageId,
                           (stlChar*)sHeader - (stlChar*)SMP_FRAME(sIndexSegHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 물리적으로 인덱스 릴레이션을 삭제한다.
 * @param[in] aRelHandle 대상 릴레이션 핸들
 * @param[in] aOnStartup Startup 중인지 여부
 * @param[out] aDone Aging 성공 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smnDropAgingInternal( void        * aRelHandle,
                                stlBool       aOnStartup,
                                stlBool     * aDone,
                                smlEnv      * aEnv )
{
    smxlTransId  sTransId;
    stlUInt64    sTransSeq;
    smlIndexType sIndexType;
    stlInt32     sState = 0;

    *aDone = STL_FALSE;
    
    if( smxlAllocTrans( SML_TIL_READ_COMMITTED,
                        STL_FALSE, /* aIsGlobalTrans */
                        &sTransId,
                        &sTransSeq,
                        aEnv )
        == STL_FAILURE )
    {
        STL_TRY_THROW( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) )
                       != KNL_ERRCODE_SERVICE_NOT_AVAILABLE,
                       RAMP_SUCCESS );
        STL_TRY( STL_FALSE );
    }

    smxlSetTransAttr( sTransId, SMXL_ATTR_UPDATABLE );
    
    sState = 1;

    sIndexType = SML_GET_INDEX_TYPE( SME_GET_RELATION_TYPE( aRelHandle ) );
        
    STL_TRY( gSmnIndexModules[sIndexType]->mDropAging( sTransId,
                                                       aRelHandle,
                                                       aOnStartup,
                                                       SML_ENV( aEnv ) )
             == STL_SUCCESS );
        
    sState = 0;
    STL_TRY( smxlCommit( sTransId,
                         NULL, /* aComment */
                         SML_TRANSACTION_CWM_WAIT,
                         aEnv ) == STL_SUCCESS );
    STL_TRY( smxlFreeTrans( sTransId,
                            SML_PEND_ACTION_COMMIT,
                            STL_FALSE, /* aCleanup */
                            SML_TRANSACTION_CWM_WAIT,
                            aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;

    STL_RAMP( RAMP_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxlRollback( sTransId,
                            SML_TRANSACTION_CWM_NO_WAIT,
                            STL_FALSE, /* aCleanup */
                            aEnv );
        (void)smxlFreeTrans( sTransId,
                             SML_PEND_ACTION_ROLLBACK,
                             STL_FALSE, /* aCleanup */
                             SML_TRANSACTION_CWM_NO_WAIT,
                             aEnv );
    }
    
    return STL_FAILURE;
}

/**
 * @brief Ager에 의해서 물리적으로 인덱스를 삭제한다.
 * @param[in] aData Aging 정보
 * @param[in] aDataSize Aging 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smnDropAging( void      * aData,
                        stlUInt32   aDataSize,
                        stlBool   * aDone,
                        void      * aEnv )
{
    smlIndexType sIndexType;
    stlInt32     sOffset = 0;
    smlRid       sSegmentRid;
    smlScn       sObjectScn;
    void       * sRelHandle;
    smlScn       sAgableScn;
    smlScn       sObjectAgableScn;
    stlInt64     sSegmentId;

    *aDone = STL_FALSE;

    STL_READ_MOVE_INT64( &sObjectScn, aData, sOffset );
    STL_READ_MOVE_INT64( &sObjectAgableScn, aData, sOffset );
    STL_READ_MOVE_INT32( &sIndexType, aData, sOffset );
    SMG_READ_MOVE_RID( &sSegmentRid, aData, sOffset );

    SMS_MAKE_SEGMENT_ID( &sSegmentId, &sSegmentRid );
    STL_DASSERT( sObjectAgableScn != SML_INFINITE_SCN );
    
    sAgableScn = smxlGetAgableStmtScn( SML_ENV(aEnv) );

    if( sObjectAgableScn <= sAgableScn )
    {
        STL_TRY( smeGetRelationHandle( sSegmentRid,
                                       &sRelHandle,
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY( smnDropAgingInternal( sRelHandle,
                                       STL_FALSE,  /* aOnStartup */
                                       aDone,
                                       SML_ENV( aEnv ) )
                 == STL_SUCCESS );

        if( (*aDone) == STL_TRUE )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[AGER] aging index - object scn(%ld), type(%d), physical id(%ld)\n",
                                sObjectScn,
                                sIndexType,
                                sSegmentId )
                     == STL_SUCCESS );
        }
        
        /**
         * smnDropAgingInternal에서 sRelHandle이 물리적으로 해제되기 때문에,
         * 이후로 sRelHandle을 사용하면 안된다
         */
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    *aDone = STL_TRUE;

    return STL_FAILURE;
}

stlStatus smnRebuildIndex( void     * aIndexHandle,
                           smlEnv   * aEnv )
{
    smnIndexHeader    * sHeader;
    smlIndexType        sIndexType;
    smnIndexModule    * sModule;
    void              * sBaseTableHandle;
    smlStatement      * sStatement;
    stlInt32            sState = 0;
    knlRegionMark       sMemMark;
    knlValueBlockList * sIndexColList = NULL;
    knlValueBlockList * sCurCol;
    knlValueBlockList * sPrevCol = NULL;
    stlInt16            i;
    stlBool             sIsLoggingTbs;
    void              * sSegHandle;
    smlRid              sSegmentRid;
    stlInt64            sSegmentId;
    smxlTransId         sTransId;
    stlUInt64           sTransSeq;
    void              * sValue;
    smlTbsId            sBaseTbsId;
    
    sHeader = (smnIndexHeader*)aIndexHandle;
    sSegHandle = SME_GET_SEGMENT_HANDLE( aIndexHandle );
    sSegmentRid = smsGetSegRid( sSegHandle );
    sIndexType = SML_GET_INDEX_TYPE(SME_GET_RELATION_TYPE(aIndexHandle));
    sModule = smnGetIndexModule( sIndexType );

    sIsLoggingTbs = smtIsLogging( smsGetTbsId(sSegHandle) );

    SMS_MAKE_SEGMENT_ID( &sSegmentId, &sSegmentRid );
    
    STL_TRY( smxlAllocTrans( SML_TIL_READ_COMMITTED,
                             STL_FALSE, /* aIsGlobalTrans */
                             &sTransId,
                             &sTransSeq,
                             aEnv ) == STL_SUCCESS );
    sState = 1;

    smxlSetTransAttr( sTransId, SMXL_ATTR_UPDATABLE );
    
    /**
     * reset segment
     * - reset segment cache & free pages
     */
                    
    STL_TRY( smsReset( sTransId,
                       sSegHandle,
                       sIsLoggingTbs,  /* aFreePages */
                       aEnv )
             == STL_SUCCESS );

    /**
     * prepare rebuild Index
     */
                    
    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 2;

    sPrevCol = NULL;
    for( i = 0; i < sHeader->mKeyColCount; i++ )
    {
        STL_TRY( knlInitBlockNoBuffer( &sCurCol,
                                       STL_LAYER_STORAGE_MANAGER,
                                       1,
                                       sHeader->mKeyColTypes[i],
                                       &aEnv->mOperationHeapMem,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
        if( i == 0 )
        {
            sIndexColList = sCurCol;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevCol, sCurCol );
        }

        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    sHeader->mKeyColSize[i],
                                    &sValue,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        KNL_SET_BLOCK_DATA_PTR( sCurCol, 0, sValue, sHeader->mKeyColSize[i] );
        KNL_SET_BLOCK_COLUMN_ORDER( sCurCol, sHeader->mKeyColOrder[i] );
        
        sPrevCol = sCurCol;
    }

    STL_TRY( smeGetRelationHandle( sHeader->mBaseTableSegRid,
                                   &sBaseTableHandle,
                                   aEnv)
             == STL_SUCCESS );

    STL_TRY( smaAllocStatement( sTransId,
                                SMXL_TO_SML_TRANS_ID( sTransId, sTransSeq ),
                                NULL,   /* aUpdateRelHandle */
                                SML_STMT_ATTR_READONLY,
                                SML_LOCK_TIMEOUT_INFINITE,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                &sStatement,
                                aEnv )
             == STL_SUCCESS );
    sState = 3;

    /**
     * rebuild Index
     */

    sBaseTbsId = smsGetTbsId( SME_GET_SEGMENT_HANDLE( sBaseTableHandle ) );
                              
    if( (SMF_IS_ONLINE_TBS( sBaseTbsId ) == STL_FALSE) ||
        (smsIsUsableSegment( SME_GET_SEGMENT_HANDLE(sBaseTableHandle) ) == STL_FALSE) )
    {
        /**
         * Base Table이 Offline Tablespace 위에 존재하거나,
         * Unusable Segment를 갖는다면 UNUSABLE 상태를 만든다.
         */
        
        if( smsIsUsableSegment( SME_GET_SEGMENT_HANDLE(aIndexHandle) ) == STL_TRUE )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                ".... REBUILD INDEX(%ld) - segment become unusable \n",
                                sSegmentId )
                     == STL_SUCCESS );

            STL_TRY( smsSetUnusableSegment( sStatement,
                                            SME_GET_SEGMENT_HANDLE(aIndexHandle),
                                            aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        if( smsIsUsableSegment( SME_GET_SEGMENT_HANDLE(aIndexHandle) ) == STL_FALSE )
        {
            /**
             * no logging index가 unusable 상태라도 startup시에는 다시 usable 상태로
             * 변경하고 rebuild를 시도해야 한다.
             * - startup시 rebuild 도중 에러가 발생한 경우
             */
            
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                ".... REBUILD INDEX(%ld) - segment become usable \n",
                                sSegmentId )
                     == STL_SUCCESS );

            STL_TRY( smsSetUsableSegment( sStatement,
                                          SME_GET_SEGMENT_HANDLE(aIndexHandle),
                                          aEnv )
                     == STL_SUCCESS );
        }

        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV(aEnv),
                            KNL_LOG_LEVEL_INFO,
                            ".... REBUILD INDEX(%ld)\n",
                            sSegmentId )
                 == STL_SUCCESS );

        if( sModule->mBuild( sStatement,
                             aIndexHandle,
                             sBaseTableHandle,
                             sHeader->mKeyColCount,
                             sIndexColList,
                             sHeader->mKeyColFlags,
                             0,   /* aParallelFactor */
                             aEnv )
            == STL_FAILURE )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "........ failed to build index ( physical id : %ld, error code : %d )\n",
                                sSegmentId,
                                stlGetLastErrorCode(KNL_ERROR_STACK(aEnv)) )
                     == STL_SUCCESS );

            stlPushError( STL_ERROR_LEVEL_WARNING,
                          SML_ERRCODE_INDEX_REBUILD,
                          "index become unusable",
                          KNL_ERROR_STACK(aEnv),
                          sSegmentId );
                          
            STL_TRY( smsSetUnusableSegment( sStatement,
                                            SME_GET_SEGMENT_HANDLE(aIndexHandle),
                                            aEnv )
                     == STL_SUCCESS );
            
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "........ segment become unusable \n" )
                     == STL_SUCCESS );
        }
    }

    /**
     * finialize rebuild Index
     */
                    
    sState = 2;
    STL_TRY( smaFreeStatement( sStatement, aEnv ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxlCommit( sTransId,
                         NULL, /* aComment */
                         SML_TRANSACTION_CWM_WAIT,
                         aEnv ) == STL_SUCCESS );
    STL_TRY( smxlFreeTrans( sTransId,
                            SML_PEND_ACTION_COMMIT,
                            STL_FALSE, /* aCleanup */
                            SML_TRANSACTION_CWM_WAIT,
                            aEnv ) == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void)smaFreeStatement( sStatement, aEnv );
        case 2:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        case 1:
            (void)smxlRollback( sTransId,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                STL_FALSE, /* aCleanup */
                                aEnv );
            (void)smxlFreeTrans( sTransId,
                                 SML_PEND_ACTION_ROLLBACK,
                                 STL_FALSE, /* aCleanup */
                                 SML_TRANSACTION_CWM_NO_WAIT,
                                 aEnv );
       default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Startup 시점에 No logging Index를 재구축한다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smnRebuildAtStartup( smlEnv * aEnv )
{
    smlRid           sSegMapRid;
    stlInt32         sState = 0;
    smlPid           sSegHdrPid;
    smlRid           sSegmentRid;
    void           * sSegMapHandle;
    void           * sRelHandle;
    void           * sSegHandle;
    void           * sSegIterator;
    stlInt32         sRelType;
    stlBool          sIsLoggingTbs;
    smnIndexHeader * sHeader;
    smlStatement   * sStatement;

    sSegMapRid = SMS_SEGMENT_MAP_RID;
    sSegMapHandle = smsGetSegmentMapHandle();

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
                                  sSegMapHandle,
                                  &sSegIterator,
                                  aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smsGetFirstPage( sSegMapHandle,
                              sSegIterator,
                              &sSegHdrPid,
                              aEnv ) == STL_SUCCESS );

    while( sSegHdrPid != SMP_NULL_PID )
    {
        SML_MAKE_RID( &sSegmentRid,
                      sSegMapRid.mTbsId,
                      sSegHdrPid,
                      0 );

        STL_TRY( smsGetSegmentHandle( sSegmentRid,
                                      &sSegHandle,
                                      aEnv )
                 == STL_SUCCESS );

        if( smsHasRelHdr( sSegHandle ) == STL_TRUE )
        {
            STL_TRY( smeGetRelationHandle( sSegmentRid,
                                           &sRelHandle,
                                           aEnv )
                     == STL_SUCCESS );

            sRelType = SME_GET_RELATION_TYPE( sRelHandle );
            
            if( SML_RELATION_IS_PERSISTENT_INDEX( sRelType ) == STL_TRUE )
            {
                /**
                 * 아래는 rebuild 해야 하는 경우를 나타낸다.
                 * 
                 * -------------------------------------------
                 *  Tbs \ Index |    Logging   |  No Logging |
                 * -------------------------------------------
                 *   Temp. Tbs  |       O      |      O      |
                 * -------------------------------------------
                 *   Pers. Tbs  |       X      |      O      |
                 * -------------------------------------------
                 */
                
                sHeader = (smnIndexHeader*)sRelHandle;
                sIsLoggingTbs = smtIsLogging( smsGetTbsId(sSegHandle) );
        
                if( sHeader->mLoggingFlag == STL_FALSE )
                {
                    STL_TRY( smnRebuildIndex( sRelHandle,
                                              aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    if( sIsLoggingTbs == STL_FALSE )
                    {
                        STL_TRY( smnRebuildIndex( sRelHandle,
                                                  aEnv )
                             == STL_SUCCESS );
                    }
                }
            }
        }
                                       
        STL_TRY( smsGetNextPage( sSegMapHandle,
                                 sSegIterator,
                                 &sSegHdrPid,
                                 aEnv ) == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( smsFreeSegIterator( sSegMapHandle,
                                 sSegIterator,
                                 aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smaFreeStatement( sStatement,
                               aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smsFreeSegIterator( sSegMapHandle, sSegIterator, aEnv );
        case 1:
            (void)smaFreeStatement( sStatement, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smnBuildUnusableIndexes( smlTbsId   aTbsId,
                                   smlEnv   * aEnv )
{
    smlRid           sSegMapRid;
    stlInt32         sState = 0;
    smlPid           sSegHdrPid;
    smlRid           sSegmentRid;
    void           * sSegMapHandle;
    void           * sRelHandle;
    void           * sSegHandle;
    void           * sSegIterator;
    stlInt32         sRelType;
    stlBool          sIsLoggingTbs;
    smnIndexHeader * sHeader;
    smlStatement   * sStatement;

    sSegMapRid = SMS_SEGMENT_MAP_RID;
    sSegMapHandle = smsGetSegmentMapHandle();

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
                                  sSegMapHandle,
                                  &sSegIterator,
                                  aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smsGetFirstPage( sSegMapHandle,
                              sSegIterator,
                              &sSegHdrPid,
                              aEnv ) == STL_SUCCESS );

    while( sSegHdrPid != SMP_NULL_PID )
    {
        SML_MAKE_RID( &sSegmentRid,
                      sSegMapRid.mTbsId,
                      sSegHdrPid,
                      0 );

        STL_TRY( smsGetSegmentHandle( sSegmentRid,
                                      &sSegHandle,
                                      aEnv )
                 == STL_SUCCESS );

        if( smsHasRelHdr( sSegHandle ) == STL_TRUE )
        {
            STL_TRY( smeGetRelationHandle( sSegmentRid,
                                           &sRelHandle,
                                           aEnv )
                     == STL_SUCCESS );

            sRelType = SME_GET_RELATION_TYPE( sRelHandle );
            
            if( SML_RELATION_IS_PERSISTENT_INDEX( sRelType ) == STL_TRUE )
            {
                /**
                 * 아래는 rebuild 해야 하는 경우를 나타낸다.
                 * 
                 * -------------------------------------------
                 *  Tbs \ Index |    Logging   |  No Logging |
                 * -------------------------------------------
                 *   Temp. Tbs  |       O      |      O      |
                 * -------------------------------------------
                 *   Pers. Tbs  |       X      |      O      |
                 * -------------------------------------------
                 */
                
                sHeader = (smnIndexHeader*)sRelHandle;
                sIsLoggingTbs = smtIsLogging( smsGetTbsId(sSegHandle) );
        
                if( sHeader->mLoggingFlag == STL_FALSE )
                {
                    STL_TRY( smnRebuildIndex( sRelHandle,
                                              aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    if( sIsLoggingTbs == STL_FALSE )
                    {
                        STL_TRY( smnRebuildIndex( sRelHandle,
                                                  aEnv )
                             == STL_SUCCESS );
                    }
                }
            }
        }
                                       
        STL_TRY( smsGetNextPage( sSegMapHandle,
                                 sSegIterator,
                                 &sSegHdrPid,
                                 aEnv ) == STL_SUCCESS );
    }
    
    sState = 1;
    STL_TRY( smsFreeSegIterator( sSegMapHandle,
                                 sSegIterator,
                                 aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smaFreeStatement( sStatement,
                               aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smsFreeSegIterator( sSegMapHandle, sSegIterator, aEnv );
        case 1:
            (void)smaFreeStatement( sStatement, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */
