/*******************************************************************************
 * smdTable.c
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
#include <smlSuppLogDef.h>
#include <smlGeneral.h>
#include <smDef.h>
#include <smxm.h>
#include <smxl.h>
#include <smg.h>
#include <smp.h>
#include <smf.h>
#include <sma.h>
#include <sme.h>
#include <smd.h>
#include <smdmphDef.h>
#include <smdmph.h>
#include <smdmpfDef.h>
#include <smdmpf.h>
#include <sms.h>
#include <smkl.h>
#include <smdDef.h>
#include <smlTable.h>
#include <smlRecord.h>

/**
 * @file smdTable.c
 * @brief Storage Manager Layer Table Manager Internal Routines
 */

/**
 * @addtogroup smd
 * @{
 */

extern smdTableModule gSmdmphTableModule;
extern smdTableModule gSmdmpfTableModule;
extern smdTableModule gSmdmppTableModule;
extern smdTableModule gSmdmifTableModule;
extern smdTableModule gSmdmisTableModule;
extern smdTableModule gSmdmihTableModule;

extern smdWarmupEntry * gSmdWarmupEntry;

smdTableModule * gSmdTableModules[SML_TABLE_TYPE_MAX] =
{
    &gSmdmphTableModule,
    &gSmdmpfTableModule,
    &gSmdmppTableModule,
    &gSmdmifTableModule,
    &gSmdmisTableModule,
    &gSmdmihTableModule
};

/**
 * @brief Table을 Access하기위한 함수 vecter를 반환한다.
 * @param[in] aTableType Table의 타입
 */
inline smdTableModule * smdGetTableModule( smlTableType aTableType )
{
    return gSmdTableModules[aTableType];
}

/**
 * @brief 물리적으로 테이블 릴레이션을 삭제한다.
 * @param[in] aRelHandle 대상 릴레이션 핸들
 * @param[in] aOnStartup Startup 중인지 여부
 * @param[out] aDone Aging 성공 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smdDropAgingInternal( void        * aRelHandle,
                                stlBool       aOnStartup,
                                stlBool     * aDone,
                                smlEnv      * aEnv )
{
    smxlTransId  sTransId;
    stlUInt64    sTransSeq;
    smlTableType sTableType;
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
    sState = 1;

    smxlSetTransAttr( sTransId, SMXL_ATTR_UPDATABLE );

    if( aRelHandle != NULL )
    {
        sTableType = SML_GET_TABLE_TYPE( SME_GET_RELATION_TYPE( aRelHandle ) );
        
        STL_TRY( gSmdTableModules[sTableType]->mDropAging( sTransId,
                                                           aRelHandle,
                                                           aOnStartup,
                                                           SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
        
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
 * @brief Ager에 의해서 물리적으로 테이블을 삭제한다.
 * @param[in] aData Aging 정보
 * @param[in] aDataSize Aging 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smdDropAging( void      * aData,
                        stlUInt32   aDataSize,
                        stlBool   * aDone,
                        void      * aEnv )
{
    smlTableType sTableType;
    stlInt32     sOffset = 0;
    smlRid       sSegmentRid;
    smlScn       sObjectScn;
    void       * sRelHandle;
    smlScn       sAgableScn;
    smlScn       sObjectAgableScn;
    stlInt64     sSegmentId;
    stlBool      sForTruncate;

    *aDone = STL_FALSE;

    STL_READ_MOVE_INT64( &sObjectScn, aData, sOffset );
    STL_READ_MOVE_INT64( &sObjectAgableScn, aData, sOffset );
    STL_READ_MOVE_INT32( &sTableType, aData, sOffset );
    SMG_READ_MOVE_RID( &sSegmentRid, aData, sOffset );

    if( sOffset < aDataSize )
    {
        STL_READ_MOVE_INT8( &sForTruncate, aData, sOffset );
    }
    else
    {
        sForTruncate = STL_FALSE;
    }

    SMS_MAKE_SEGMENT_ID( &sSegmentId, &sSegmentRid );
    STL_DASSERT( sObjectAgableScn != SML_INFINITE_SCN );
    
    sAgableScn = smxlGetAgableStmtScn( SML_ENV(aEnv) );

    if( sObjectAgableScn <= sAgableScn )
    {
        STL_TRY( smeGetRelationHandle( sSegmentRid,
                                       &sRelHandle,
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY( smdDropAgingInternal( sRelHandle,
                                       STL_FALSE,  /* aOnStartup */
                                       aDone,
                                       SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        if( (*aDone) == STL_TRUE )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[AGER] aging table - object scn(%ld), object view scn(%ld), "
                                "type(%d), physical id(%ld)\n",
                                sObjectScn,
                                sObjectAgableScn,
                                sTableType,
                                sSegmentId )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    *aDone = STL_TRUE;

    return STL_FAILURE;
}

/**
 * @brief Ager에 의해서 물리적으로 LOCK ITEM을 삭제한다.
 * @param[in] aData Aging 정보
 * @param[in] aDataSize Aging 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smdLockItemAging( void      * aData,
                            stlUInt32   aDataSize,
                            stlBool   * aDone,
                            void      * aEnv )
{
    stlInt32       sOffset = 0;
    smlScn         sAgableStmtScn;
    smlScn         sObjectAgableScn;
    void         * sLockItem;
    smlRid         sSegmentRid;
    stlInt64       sSegmentId;

    *aDone = STL_FALSE;

    STL_READ_MOVE_INT64( &sObjectAgableScn, aData, sOffset );
    SMG_READ_MOVE_RID( &sSegmentRid, aData, sOffset );
    STL_READ_MOVE_POINTER( &sLockItem, aData, sOffset );

    SMS_MAKE_SEGMENT_ID( &sSegmentId, &sSegmentRid );
    STL_DASSERT( sObjectAgableScn != SML_INFINITE_SCN );
    
    sAgableStmtScn = smxlGetAgableStmtScn( SML_ENV(aEnv) );

    if( sObjectAgableScn <= sAgableStmtScn )
    {
        *aDone = STL_TRUE;
        
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[AGER] aging lock item - object scn(%ld), agable stmt scn(%ld), "
                            "physical id(%ld)\n",
                            sObjectAgableScn,
                            sAgableStmtScn,
                            sSegmentId )
                 == STL_SUCCESS );
    
        STL_TRY( smklAgingLockHandle( sLockItem,
                                      SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    *aDone = STL_TRUE;

    return STL_FAILURE;
}

stlStatus smdSetSegmentPending( stlUInt32   aActionFlag,
                                smgPendOp * aPendOp,
                                smlEnv    * aEnv )
{
    smdPendArg * sPendArg = (smdPendArg*)aPendOp->mArgs;

    STL_TRY( smfSetOfflineRelationId( NULL,
                                      sPendArg->mTbsId,
                                      sPendArg->mRelationId,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

inline void smdGetSystemInfo( smdSystemInfo * aSystemInfo )
{
    aSystemInfo->mVersionConflictCount = gSmdWarmupEntry->mVersionConflictCount;
}

inline void * smdGetLockHandle( void * aRelationHandle )
{
    return (void*)(((smdTableHeader*)aRelationHandle)->mLockHandle);
}

inline stlStatus smdInitVolatileInfo( void * aRelationHandle, smlEnv * aEnv )
{
    void * sTmpLockHandle;
    smdTableHeader * sTblHdr = (smdTableHeader*)aRelationHandle;

    STL_TRY( smklAllocLockHandle( aRelationHandle,
                                  &sTmpLockHandle,
                                  aEnv ) == STL_SUCCESS );
    sTblHdr->mLockHandle = (stlUInt64)sTmpLockHandle;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdCompareKeyValue( void                * aRelationHandle,
                              smlRid              * aRowRid,
                              smpHandle           * aPageHandle,
                              stlBool             * aPageLatchRelease,
                              smxlTransId           aMyTransId,
                              smlScn                aMyViewScn,
                              smlScn                aMyCommitScn,
                              smlTcn                aMyTcn,
                              knlKeyCompareList   * aKeyCompList,
                              stlBool             * aIsMatch,
                              smlScn              * aCommitScn,
                              smlEnv              * aEnv )
{
    smlTableType sTableType = SML_GET_TABLE_TYPE(SME_GET_RELATION_TYPE(aRelationHandle));
    smdTableModule * sModule = smdGetTableModule(sTableType);

    STL_TRY( sModule->mCompareKeyValue( aRowRid,
                                        aPageHandle,
                                        aPageLatchRelease,
                                        aMyTransId,
                                        aMyViewScn,
                                        aMyCommitScn,
                                        aMyTcn,
                                        aKeyCompList,
                                        aIsMatch,
                                        aCommitScn,
                                        aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief latest version의 레코드들에서 n개의 인덱스 키들을 추출한다.
 * @param[in] aRelationHandle base table relation handle
 * @param[in] aPageHandle target page handle
 * @param[in] aFetchFirst first fetch 여부
 * @param[in] aRowIterator row iterator
 * @param[in] aColumnList 추출할 대상 column들
 * @param[in] aKeyValueSize 각 인덱스 키들의 총 크기를 저장할 array
 * @param[in] aRowSeq Target record의 offset을 저장할 array
 * @param[in] aHasNullInBlock Target record들중 null column이 있는지 여부
 * @param[in] aIsSuccess 한건이라도 키를 추출했는지 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smdExtractKeyValue( void                * aRelationHandle,
                              smpHandle           * aPageHandle,
                              stlBool               aFetchFirst,
                              smpRowItemIterator  * aRowIterator,
                              smdValueBlockList   * aColumnList,
                              stlInt32            * aKeyValueSize,
                              stlInt16            * aRowSeq,
                              stlBool             * aHasNullInBlock,
                              stlBool             * aIsSuccess,
                              smlEnv              * aEnv )
{
    smlTableType sTableType = SML_GET_TABLE_TYPE(SME_GET_RELATION_TYPE(aRelationHandle));
    smdTableModule * sModule = smdGetTableModule(sTableType);

    STL_TRY( sModule->mExtractKeyValue( aPageHandle,
                                        aFetchFirst,
                                        aRowIterator,
                                        aColumnList,
                                        aKeyValueSize,
                                        aRowSeq,
                                        aHasNullInBlock,
                                        aIsSuccess,
                                        aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 특정 시점(snapshot)의 레코드에서 인덱스 키를 재구성한다.
 * @param[in] aRelationHandle base table relation handle
 * @param[in] aTransId Transaction Identifier
 * @param[in] aViewScn Snapshot 기준 scn
 * @param[in] aTcn Snapshot 기준 tcn
 * @param[in] aRid Target record identifier
 * @param[in] aColumnList Target Column List
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smdExtractValidKeyValue( void                * aRelationHandle,
                                   smxlTransId           aTransId,
                                   smlScn                aViewScn,
                                   smlTcn                aTcn,
                                   smlRid              * aRid,
                                   knlValueBlockList   * aColumnList,
                                   smlEnv              * aEnv )
{
    smlTableType sTableType = SML_GET_TABLE_TYPE(SME_GET_RELATION_TYPE(aRelationHandle));
    smdTableModule * sModule = smdGetTableModule(sTableType);

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aColumnList, 0, 0 );
    
    STL_TRY( sModule->mExtractValidKeyValue( aRelationHandle,
                                             aTransId,
                                             aViewScn,
                                             aTcn,
                                             aRid,
                                             aColumnList,
                                             aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdExecutePendingOperation( smlStatement * aStatement,
                                      void         * aRelationHandle,
                                      smlEnv       * aEnv )
{
    stlInt32             sState = 0;
    smlRowBlock          sRowBlock;
    smlRid               sRowRid;
    smlScn               sRowScn;
    smiIterator        * sIterator;
    smdTableModule     * sModule;
    smlIteratorProperty  sProperty;
    smlFetchInfo         sFetchInfo;

    sModule = smdGetTableModule( SML_TABLE_TYPE_MEMORY_PERSISTENT_PENDING );
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    aEnv )
             == STL_SUCCESS );
 
    SML_INIT_ITERATOR_PROP( sProperty );

    STL_TRY( smaAllocIterator( aStatement,
                               aRelationHandle,
                               SML_TRM_COMMITTED,
                               SML_SRM_RECENT,
                               &sProperty,
                               SML_SCAN_FORWARD,
                               (void**)&sIterator,
                               aEnv )
             == STL_SUCCESS );
    sState = 1;

    SML_SET_FETCH_INFO_FOR_TABLE( & sFetchInfo,/* Fetch Info */
                                  NULL,        /* physical filter */
                                  NULL,        /* logical filter */
                                  NULL,        /* read column list */
                                  NULL,        /* rowid column list */
                                  & sRowBlock, /* row block */
                                  0,           /* skip count */
                                  0,           /* fetch count (meaningless) */
                                  STL_FALSE,   /* group key fetch */
                                  NULL         /* filter evaluate info */
        );

    while( STL_TRUE )
    {
        STL_TRY( sIterator->mFetchNext( sIterator,
                                        &sFetchInfo,
                                        aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }

        /**
         * Delete와 동시에 Pending Operation을 수행한다.
         */
        
        STL_TRY( sModule->mDelete( aStatement,
                                   aRelationHandle,
                                   &sRowRid,
                                   sRowScn,
                                   0,    /* aBlockIdx */
                                   NULL, /* aPrimaryKey */
                                   NULL, /* aVersionConflict */
                                   NULL, /* aSkipped */
                                   aEnv ) == STL_SUCCESS );
    }
    
    sState = 0;
    STL_TRY( smaFreeIterator( sIterator, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) smaFreeIterator( sIterator, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smdInsertPendingOperation( smxlTransId    aTransId,
                                     smlTbsId       aTbsId,
                                     stlInt32       aOperationType,
                                     void         * aData,
                                     stlInt32       aDataSize,
                                     smlEnv       * aEnv )
{
    stlChar             sBuffer[SMP_PAGE_SIZE];
    stlInt32            sOffset = 0;
    stlInt64            sRelationId;
    void              * sRelationHandle;
    smlRid              sRid;
    smdTableModule    * sModule;
    knlRegionMark       sMemMark;
    stlInt32            sState = 0;
    knlValueBlockList * sInsertColumn = NULL;
    dtlDataValue      * sDataValue;
    smlStatement      * sStatement;
    smlRid              sRelationRid;

    STL_DASSERT( (aDataSize + STL_SIZEOF(stlInt32)) < SMP_PAGE_SIZE );
    
    STL_WRITE_MOVE_INT32( sBuffer, &aOperationType, sOffset );
    STL_WRITE_MOVE_BYTES( sBuffer, aData, aDataSize, sOffset );

    sRelationId = smfGetPendingRelationId( aTbsId );
    SMG_WRITE_RID( &sRelationRid, &sRelationId );

    STL_TRY( smeGetRelationHandle( sRelationRid,
                                   &sRelationHandle,
                                   aEnv ) == STL_SUCCESS );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlInitBlockNoBuffer( &sInsertColumn,
                                   STL_LAYER_STORAGE_MANAGER,
                                   1,
                                   DTL_TYPE_BINARY,
                                   &aEnv->mOperationHeapMem,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sDataValue = KNL_GET_BLOCK_DATA_VALUE( sInsertColumn, 0 );
    sDataValue->mBufferSize = SMP_PAGE_SIZE;
    sDataValue->mLength = sOffset;
    sDataValue->mValue = sBuffer;

    STL_TRY( smaAllocStatement( aTransId,
                                SML_INVALID_TRANSID,
                                NULL, /* aRelationHandle */
                                ( SML_STMT_ATTR_UPDATABLE |
                                  SML_STMT_ATTR_LOCKABLE ),
                                SML_LOCK_TIMEOUT_INFINITE,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                &sStatement,
                                aEnv )
             == STL_SUCCESS );
    sState = 2;
    
    sModule = smdGetTableModule( SML_TABLE_TYPE_MEMORY_PERSISTENT_PENDING );
    STL_TRY( sModule->mInsert( sStatement,
                               sRelationHandle,
                               0,  /* aBlockIdx */
                               sInsertColumn,
                               NULL, /* unique violation */
                               &sRid,
                               aEnv ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( smaFreeStatement( sStatement, aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smaRollbackStatement( sStatement, STL_TRUE, aEnv );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            KNL_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smdCreatePendingRelation( smlStatement * aStatement,
                                    smlTbsId       aTbsId,
                                    smlEnv       * aEnv )
{
    smdTableModule  * sModule;
    smdPendArg        sPendArg;
    stlInt64          sRelationId = SML_INVALID_RELATION_ID;
    void            * sRelationHandle;

    /**
     * create pending segment
     */

    sModule = smdGetTableModule( SML_TABLE_TYPE_MEMORY_PERSISTENT_PENDING );

    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );
    
    STL_TRY( sModule->mCreate( aStatement,
                               SML_MEMORY_DICT_SYSTEM_TBS_ID,
                               NULL,      /* aAttr */
                               STL_TRUE,  /* aUndoLogging */
                               &sRelationId,
                               &sRelationHandle,
                               aEnv )
             == STL_SUCCESS );
        
    sPendArg.mRelationId = sRelationId;
    sPendArg.mTbsId = aTbsId;
        
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_SET_SEGMENT,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendArg,
                           STL_SIZEOF( smdPendArg ),
                           0,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdDropPendingRelation( smlStatement * aStatement,
                                  smlTbsId       aTbsId,
                                  smlEnv       * aEnv )
{
    smdTableModule  * sModule;
    stlInt64          sRelationId = SML_INVALID_RELATION_ID;
    void            * sRelationHandle;

    sModule = smdGetTableModule( SML_TABLE_TYPE_MEMORY_PERSISTENT_PENDING );
    sRelationId = smfGetPendingRelationId( aTbsId );

    if( sRelationId != SML_INVALID_RELATION_ID )
    {
        STL_TRY( smlGetRelationHandle( sRelationId,
                                       &sRelationHandle,
                                       aEnv )
                 == STL_SUCCESS );
    
        STL_TRY( sModule->mTruncate( aStatement,
                                     sRelationHandle,
                                     aEnv) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                                   

stlStatus smdTruncatePendingRelation( smlStatement * aStatement,
                                      smlTbsId       aTbsId,
                                      smlEnv       * aEnv )
{
    smdTableModule  * sModule;
    stlInt64          sRelationId = SML_INVALID_RELATION_ID;
    void            * sRelationHandle;

    sModule = smdGetTableModule( SML_TABLE_TYPE_MEMORY_PERSISTENT_PENDING );
    sRelationId = smfGetPendingRelationId( aTbsId );

    if( sRelationId != SML_INVALID_RELATION_ID )
    {
        STL_TRY( smlGetRelationHandle( sRelationId,
                                       &sRelationHandle,
                                       aEnv )
                 == STL_SUCCESS );
    
        STL_TRY( sModule->mTruncate( aStatement,
                                     sRelationHandle,
                                     aEnv) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                                   

stlStatus smdAddHandleToSession( smlStatement      * aStatement,
                                 smeRelationHeader * aRelHeader,
                                 smlEnv            * aEnv )
{
    smdInstHandleListNode * sNode;
    smdInstHandleListNode * sFirst;

    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smdInstHandleListNode ),
                                 (void**)&sNode,
                                 aEnv ) == STL_SUCCESS );

    sFirst = SMD_GET_SESS_INST_HANDLE_LIST( aEnv );
    if( sFirst == NULL )
    {
        sNode->mInstTableId = SMD_FIRST_SESS_INST_TABLE_ID;
        sNode->mPrev = NULL;
        sNode->mNext = NULL;
    }
    else
    {
        sNode->mInstTableId = sFirst->mInstTableId + 1;
        sNode->mPrev = sFirst->mPrev;
        sNode->mNext = sFirst;
        sFirst->mPrev = sNode;
    }
    sNode->mHandle = aRelHeader;

    SMD_SET_SESS_INST_HANDLE_LIST( aEnv, sNode );
    aStatement->mActiveInstantCount += 1;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdRemoveHandleFromSession( smlStatement      * aStatement,
                                      smeRelationHeader * aRelHeader,
                                      smlEnv            * aEnv )
{
    smdInstHandleListNode * sNode;

    sNode = SMD_GET_SESS_INST_HANDLE_LIST( aEnv );
    while( sNode != NULL )
    {
        if( sNode->mHandle == aRelHeader )
        {
            if( sNode->mPrev != NULL )
            {
                sNode->mPrev->mNext = sNode->mNext;
            }
            else
            {
                SMD_SET_SESS_INST_HANDLE_LIST( aEnv, sNode->mNext );
            }

            if( sNode->mNext != NULL )
            {
                sNode->mNext->mPrev = sNode->mPrev;
            }

            STL_TRY( smgFreeSessShmMem( sNode,
                                        aEnv ) == STL_SUCCESS );
            aStatement->mActiveInstantCount -= 1;

            break;
        }
        sNode = sNode->mNext;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlInt64 smdGetActiveInstantRelationCount(  smlSessionEnv * aSessEnv )
{
    smdInstHandleListNode * sNode;
    stlInt64                sActiveCount = 0;

    sNode = aSessEnv->mInstTableList;
    
    while( sNode != NULL )
    {
        sActiveCount += 1;
        sNode = sNode->mNext;
    }

    return sActiveCount;
}

stlStatus smdCleanupAllInstTables( smlEnv * aEnv )
{
    smdTableModule        * sTableModule;
    smdInstHandleListNode * sNode;
    smlSessionEnv         * sSessionEnv;
    stlInt32                sRelationType;

    sSessionEnv = SML_SESS_ENV(aEnv);
    
    sNode = (smdInstHandleListNode*)(sSessionEnv->mInstTableList);
    
    while( sNode != NULL )
    {
        sRelationType = SME_GET_RELATION_TYPE( sNode->mHandle );
        
        if( SML_RELATION_IS_INSTANT_TABLE( sRelationType ) == STL_TRUE )
        {
            sTableModule = smdGetTableModule( SML_GET_TABLE_TYPE(sRelationType) );

            STL_TRY( sTableModule->mCleanup( sNode->mHandle,
                                             aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_DASSERT( SML_RELATION_IS_INSTANT_INDEX( sRelationType ) == STL_TRUE );
            
            /**
             * instant index는 base table의 segment를 사용해서 공간을 할당하기
             * 때문에 삭제할 필요가 없다.
             */
        }

        sSessionEnv->mInstTableList = sNode->mNext;
        sNode = (smdInstHandleListNode*)sSessionEnv->mInstTableList;
    }

    return STL_SUCCESS;     
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smdBuildCache( void      * aRelationHandle,
                         smlEnv    * aEnv )
{
    smlTableType sTableType;

    sTableType = SML_GET_TABLE_TYPE( SME_GET_RELATION_TYPE(aRelationHandle) );

    if( (sTableType < SML_TABLE_TYPE_MAX) && 
        (gSmdTableModules[sTableType]->mBuildCache != NULL) )
    {
        STL_TRY( gSmdTableModules[sTableType]->mBuildCache( aRelationHandle,
                                                            NULL,
                                                            aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;     
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smdFetchRecord4Index( smiIterator         * aIterator,
                                void                * aBaseRelHandle,
                                smlFetchRecordInfo  * aFetchRecordInfo,
                                smpHandle           * aTablePageHandle,
                                stlBool             * aTablePageLatchReleased,
                                smlRid              * aRowRid,
                                stlInt32              aBlockIdx,
                                smlScn                aAgableViewScn,
                                stlBool             * aIsMatched,
                                smlEnv              * aEnv )
{
    smlTableType sTableType;

    sTableType = SML_GET_TABLE_TYPE( SME_GET_RELATION_TYPE(aBaseRelHandle) );

    if( gSmdTableModules[sTableType]->mFetch4Index != NULL )
    {
        STL_TRY( gSmdTableModules[sTableType]->mFetch4Index( aIterator,
                                                             aBaseRelHandle,
                                                             aFetchRecordInfo,
                                                             aTablePageHandle,
                                                             aTablePageLatchReleased,
                                                             aRowRid,
                                                             aBlockIdx,
                                                             aAgableViewScn,
                                                             aIsMatched,
                                                             aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;     
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smdCreatePending( stlUInt32   aActionFlag,
                            smgPendOp * aPendOp,
                            smlEnv    * aEnv )
{
    smdTablePendArg   * sPendArg = (smdTablePendArg*)aPendOp->mArgs;
    smeRelationHeader * sRelationHeader;

    sRelationHeader = (smeRelationHeader*)(sPendArg->mHandle);

    STL_TRY( smxlGetCommitScnFromCache( sPendArg->mTransId,
                                        &sRelationHeader->mObjectScn,
                                        aEnv ) == STL_SUCCESS );
    sRelationHeader->mCreateScn = sRelationHeader->mObjectScn;
    
    return STL_SUCCESS;     
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smdDropPending( stlUInt32   aActionFlag,
                          smgPendOp * aPendOp,
                          smlEnv    * aEnv )
{
    smdTablePendArg   * sPendArg = (smdTablePendArg*)aPendOp->mArgs;
    smeRelationHeader * sRelationHeader;
    stlInt32            sOffset = 0;
    stlChar             sBuffer[32];
    smlRid              sSegRid;
    smlTableType        sTableType;
    smlScn              sObjectAgableScn;

    sRelationHeader = (smeRelationHeader*)(sPendArg->mHandle);

    STL_TRY( smxlGetCommitScnFromCache( sPendArg->mTransId,
                                        &sRelationHeader->mObjectScn,
                                        aEnv ) == STL_SUCCESS );
    sRelationHeader->mDropScn = sRelationHeader->mObjectScn;

    sSegRid = smsGetSegRid( SME_GET_SEGMENT_HANDLE(sPendArg->mHandle) );
    sTableType = SML_GET_TABLE_TYPE( sRelationHeader->mRelationType );

    /**
     * Trans 1 : |----- commit(scn:100) -----|----- pending -----|----- end trans -----|
     * 
     * Trans 2 :                             |-----> alloc statement(view scn:100)
     *
     * 'Trans 2'가 Trans 1의 commit 이후에 statement를 할당했다면 SCN을 최신으로
     * 얻게되고, Trans 1이 수행했던 DDL의 Version들이 재사용 가능하게 된다.
     * 이를 방지하기 위해서는 Pending operation 수행시 System Scn을 강제적으로
     * 늘려야 한다.
     */
    sObjectAgableScn = smxlGetNextSystemScn();

    STL_WRITE_MOVE_INT64( sBuffer, &sRelationHeader->mObjectScn, sOffset );
    STL_WRITE_MOVE_INT64( sBuffer, &sObjectAgableScn, sOffset );
    STL_WRITE_MOVE_INT32( sBuffer, &sTableType, sOffset );
    SMG_WRITE_MOVE_RID( sBuffer, &sSegRid, sOffset );

    STL_DASSERT( sOffset < STL_SIZEOF( sBuffer ) );
    STL_DASSERT( sOffset <= aPendOp->mEventMemSize );

    STL_TRY( knlAddEnvEvent( SML_EVENT_DROP_TABLE,
                             aPendOp->mEventMem,
                             sBuffer,
                             sOffset,
                             SML_AGER_ENV_ID,
                             KNL_EVENT_WAIT_NO,
                             STL_TRUE, /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;     
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smdFreeLockPending( stlUInt32   aActionFlag,
                              smgPendOp * aPendOp,
                              smlEnv    * aEnv )
{
    smdTablePendArg   * sPendArg = (smdTablePendArg*)aPendOp->mArgs;
    stlInt32            sOffset = 0;
    stlChar             sBuffer[24];
    smlScn              sObjectAgableScn;
    void              * sLockHandle;
    smlRid              sSegRid;

    sLockHandle = SMD_GET_LOCK_HANDLE( sPendArg->mHandle );

    if( sLockHandle != NULL )
    {
        STL_TRY( smklMarkFreeLockHandle( sLockHandle, aEnv ) == STL_SUCCESS );
    }

    sSegRid = smsGetSegRid( SME_GET_SEGMENT_HANDLE(sPendArg->mHandle) );
    
    sObjectAgableScn = smxlGetNextSystemScn();

    STL_WRITE_MOVE_INT64( sBuffer, &sObjectAgableScn, sOffset );
    SMG_WRITE_MOVE_RID( sBuffer, &sSegRid, sOffset );
    STL_WRITE_MOVE_POINTER( sBuffer, &sLockHandle, sOffset );

    STL_DASSERT( sOffset <= STL_SIZEOF( sBuffer ) );
    STL_DASSERT( sOffset <= aPendOp->mEventMemSize );

    STL_TRY( knlAddEnvEvent( SML_EVENT_AGING_LOCK_ITEM,
                             aPendOp->mEventMem,
                             sBuffer,
                             sOffset,
                             SML_AGER_ENV_ID,
                             ( KNL_EVENT_WAIT_NO | KNL_EVENT_FAIL_BEHAVIOR_CONTINUE ),
                             STL_TRUE, /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;     
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Supplemental 로그를 기록한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aLogType Log Type
 * @param[in] aValueIdx Value block에서 현재 처리할 row의 순서
 * @param[in] aPrimaryKey 해당 레코드의 primary key
 * @param[in] aAfterCols After Image
 * @param[in] aRowRid Target Record Identifier
 * @param[in,out] aEnv Environment 정보
 * @see @a aLogType : smrLogType
 * @see @a aRedoTargetType : smlSupplementalLogType
 */
stlStatus smdWriteSupplementalLog( smxmTrans         * aMiniTrans,
                                   stlUInt16           aLogType,
                                   stlInt32            aValueIdx,
                                   knlValueBlockList * aPrimaryKey,
                                   knlValueBlockList * aAfterCols,
                                   smlRid            * aRowRid,
                                   smlEnv            * aEnv )
{
    stlUInt64           sLogData[SMR_MAX_LOGPIECE_SIZE/STL_SIZEOF(stlUInt64)];
    stlChar           * sLogPtr = ((stlChar*)sLogData) + STL_SIZEOF(stlUInt16) + STL_SIZEOF(stlBool);
    stlUInt16           sColumnCount = 0;
    stlInt64            sColLen;
    stlUInt8            sColLenSize;
    knlValueBlockList * sCurCol = aPrimaryKey;
    knlValueBlockList * sUpdateCol;
    dtlDataValue      * sDataValue;
    stlBool             sIsZero;
    stlBool             sUpdatePk = STL_FALSE;

    STL_TRY( smxlInsertTransRecordWithMiniTrans( aMiniTrans,
                                                 aEnv )
             == STL_SUCCESS );
            
    while( sCurCol != NULL )
    {
        sIsZero = STL_FALSE;
        
        sDataValue = KNL_GET_BLOCK_DATA_VALUE(sCurCol, aValueIdx);
        sColLen = sDataValue->mLength;
        STL_DASSERT( sColLen > 0 );

        if( DTL_NUMERIC_IS_ZERO(sDataValue->mValue, sDataValue->mLength) )
        {
            if( dtlDataTypeGroup[sDataValue->mType] == DTL_GROUP_NUMBER )
            {
                sIsZero = STL_TRUE;
                sColLen = 0;
            }
        }
        
        SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
        
        SMP_WRITE_COLUMN( sLogPtr,
                          sDataValue->mValue,
                          sDataValue->mLength,
                          sIsZero );
        sLogPtr += (sColLenSize + sColLen);

        sColumnCount++;
        
        if( sUpdatePk == STL_FALSE )
        {
            sUpdateCol = aAfterCols;
            
            while( sUpdateCol != NULL )
            {
                if( sUpdateCol->mColumnOrder == sCurCol->mColumnOrder )
                {
                    sUpdatePk = STL_TRUE;
                    break;
                }
                
                sUpdateCol = sUpdateCol->mNext;
            }
        }
        
        sCurCol = sCurCol->mNext;
    }

    STL_WRITE_INT8( (stlChar*)sLogData, &sUpdatePk );
    STL_WRITE_INT16( ((stlChar*)(sLogData) + STL_SIZEOF(stlBool)), &sColumnCount );

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_EXTERNAL,
                           aLogType,
                           SMR_REDO_TARGET_NONE,
                           (stlChar*)sLogData,
                           sLogPtr - (stlChar*)sLogData,
                           aRowRid->mTbsId,
                           aRowRid->mPageId,
                           aRowRid->mOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Update column list를 가진 Supplemental 로그를 기록한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aLogCols Update 컬럼정보
 * @param[in] aValueIdx Value block에서 현재 처리할 row의 순서
 * @param[in] aRowRid Target Record Identifier
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smdWriteSupplUpdateColLogs( smxmTrans         * aMiniTrans,
                                      knlValueBlockList * aLogCols,
                                      stlInt32            aValueIdx,
                                      smlRid            * aRowRid,
                                      smlEnv            * aEnv )
{
    stlUInt32           sLogData[SMP_PAGE_SIZE / STL_SIZEOF(stlUInt32)];
    stlChar           * sLogPtr = ((stlChar*)sLogData) + STL_SIZEOF(stlUInt16);
    stlUInt16           sColumnCount = 0;
    knlValueBlockList * sCurCol = aLogCols;
    stlInt32            sColOrd;
    stlInt16            sLogPieceCount = 0;
    stlUInt32           sAttr = smxmGetAttr( aMiniTrans );
    smxlTransId         sTransId = smxmGetTransId( aMiniTrans );
    smlRid              sSegRid = smxmGetSegRid( aMiniTrans );

    /**
     * Update 컬럼이 비정상적으로 많아 supplimental log의 크기가 
     * 너무 커지는 것을 방지하고자, 임의로 8K 단위로 log piece를
     * 끊고, 또 한 log record 당 log piece의 개수를 3개로 제한한다
     */
    while( sCurCol != NULL )
    {
        sColOrd = KNL_GET_BLOCK_COLUMN_ORDER( sCurCol );
        STL_WRITE_INT32( sLogPtr, &sColOrd );
        sLogPtr += STL_SIZEOF(stlUInt32);
        sColumnCount++;
        sCurCol = sCurCol->mNext;

        if( sLogPtr + STL_SIZEOF(stlUInt32) >= ((stlChar*)sLogData) + SMP_PAGE_SIZE )
        {
            /* 최대 8K를 넘어섬 */
            stlMemcpy( sLogData, &sColumnCount, STL_SIZEOF(stlUInt16) );
            STL_TRY( smxmWriteLog( aMiniTrans,
                                   SMG_COMPONENT_EXTERNAL,
                                   SML_SUPPL_LOG_UPDATE_COLS,
                                   SMR_REDO_TARGET_NONE,
                                   (stlChar*)sLogData,
                                   sLogPtr - (stlChar*)sLogData,
                                   aRowRid->mTbsId,
                                   aRowRid->mPageId,
                                   aRowRid->mOffset,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );

            sLogPtr = ((stlChar*)sLogData) + STL_SIZEOF(stlInt16);
            sColumnCount = 0;
            sLogPieceCount++;

            if( sLogPieceCount >= 3 )
            {
                STL_TRY( smxmCommit( aMiniTrans, aEnv ) == STL_SUCCESS );
                STL_TRY( smxmBegin( aMiniTrans,
                                    sTransId,
                                    sSegRid,
                                    sAttr,
                                    aEnv )
                         == STL_SUCCESS );
                sLogPieceCount = 0;
            }
        }
    }

    if( sColumnCount > 0 )
    {
        stlMemcpy( sLogData, &sColumnCount, STL_SIZEOF(stlUInt16) );
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_EXTERNAL,
                               SML_SUPPL_LOG_UPDATE_COLS,
                               SMR_REDO_TARGET_NONE,
                               (stlChar*)sLogData,
                               sLogPtr - (stlChar*)sLogData,
                               aRowRid->mTbsId,
                               aRowRid->mPageId,
                               aRowRid->mOffset,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Update column list를 가진 Before Supplemental 로그를 기록한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aLogCols Update 컬럼정보
 * @param[in] aValueIdx Value block에서 현재 처리할 row의 순서
 * @param[in] aRowRid Target Record Identifier
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smdWriteSupplUpdateBeforeColLogs( smxmTrans         * aMiniTrans,
                                            knlValueBlockList * aLogCols,
                                            stlInt32            aValueIdx,
                                            smlRid            * aRowRid,
                                            smlEnv            * aEnv )
{
    stlUInt32           sLogData[(SMP_PAGE_SIZE / STL_SIZEOF(stlUInt32))*2];
    stlChar           * sLogPtr = ((stlChar*)sLogData) + STL_SIZEOF(stlUInt16);
    stlUInt16           sColumnCount = 0;
    knlValueBlockList * sCurCol = aLogCols;
    stlInt32            sColOrd;
    stlInt16            sLogPieceCount = 0;
    stlUInt32           sAttr = smxmGetAttr( aMiniTrans );
    smxlTransId         sTransId = smxmGetTransId( aMiniTrans );
    smlRid              sSegRid = smxmGetSegRid( aMiniTrans );
    dtlDataValue      * sDataValue;
    stlUInt8            sColLenSize;
    stlChar           * sValue;
    stlInt64            sLength;
    stlInt64            sWriteSize;

    /**
     * Update 컬럼이 비정상적으로 많아 supplimental log의 크기가 
     * 너무 커지는 것을 방지하고자, 임의로 8K 단위로 log piece를
     * 끊고, 또 한 log record 당 log piece의 개수를 3개로 제한한다
     */
    while( sCurCol != NULL )
    {
        sColOrd = KNL_GET_BLOCK_COLUMN_ORDER( sCurCol );
        STL_WRITE_INT32( sLogPtr, &sColOrd );
        sLogPtr += STL_SIZEOF(stlUInt32);

        sDataValue = KNL_GET_BLOCK_DATA_VALUE( sCurCol, aValueIdx );
        SMP_GET_COLUMN_LEN_SIZE( sDataValue->mLength, &sColLenSize );

        sValue = sDataValue->mValue;
        sLength = sDataValue->mLength;

        if( (sLogPtr + sLength + sColLenSize) >= (((stlChar*)sLogData) + SMP_PAGE_SIZE) )
        {
            while( 1 )
            {
                SMP_GET_COLUMN_LEN_SIZE( sLength, &sColLenSize );
                
                if( (sLogPtr + sLength + sColLenSize) >=
                    (((stlChar*)sLogData) + SMP_PAGE_SIZE) )
                {
                    sWriteSize = STL_MIN( sLength,
                                          (SMP_PAGE_SIZE - (sLogPtr - ((stlChar*)sLogData))) );
                }
                else
                {
                    break;
                }

                sColumnCount++;
                
                SMP_GET_COLUMN_LEN_SIZE( sWriteSize, &sColLenSize );
                
                SMP_WRITE_COLUMN( sLogPtr,
                                  sValue,
                                  sWriteSize,
                                  STL_FALSE );

                sLogPtr += (sWriteSize + sColLenSize);
            
                /* 최대 8K를 넘어섬 */
                stlMemcpy( sLogData, &sColumnCount, STL_SIZEOF(stlUInt16) );
                STL_TRY( smxmWriteLog( aMiniTrans,
                                       SMG_COMPONENT_EXTERNAL,
                                       SML_SUPPL_LOG_UPDATE_BEFORE_COLS,
                                       SMR_REDO_TARGET_NONE,
                                       (stlChar*)sLogData,
                                       sLogPtr - (stlChar*)sLogData,
                                       aRowRid->mTbsId,
                                       aRowRid->mPageId,
                                       aRowRid->mOffset,
                                       SMXM_LOGGING_REDO,
                                       aEnv ) == STL_SUCCESS );

                sLength -= sWriteSize;
                sValue += sWriteSize;
                
                sLogPtr = ((stlChar*)sLogData) + STL_SIZEOF(stlInt16);
                
                STL_WRITE_INT32( sLogPtr, &sColOrd );
                sLogPtr += STL_SIZEOF(stlUInt32);
                
                sColumnCount = 0;
                sLogPieceCount++;

                if( sLogPieceCount >= 3 )
                {
                    STL_TRY( smxmCommit( aMiniTrans, aEnv ) == STL_SUCCESS );
                    STL_TRY( smxmBegin( aMiniTrans,
                                        sTransId,
                                        sSegRid,
                                        sAttr,
                                        aEnv )
                             == STL_SUCCESS );
                    sLogPieceCount = 0;
                }
            }
        }
        
        sColumnCount++;
        
        SMP_GET_COLUMN_LEN_SIZE( sLength, &sColLenSize );
        
        SMP_WRITE_COLUMN( sLogPtr,
                          sValue,
                          sLength,
                          STL_FALSE );

        sLogPtr += (sLength + sColLenSize);
            
        sCurCol = sCurCol->mNext;
    }
    
    if( sColumnCount > 0 )
    {
        stlMemcpy( sLogData, &sColumnCount, STL_SIZEOF(stlUInt16) );
        
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_EXTERNAL,
                               SML_SUPPL_LOG_UPDATE_BEFORE_COLS,
                               SMR_REDO_TARGET_NONE,
                               (stlChar*)sLogData,
                               sLogPtr - (stlChar*)sLogData,
                               aRowRid->mTbsId,
                               aRowRid->mPageId,
                               aRowRid->mOffset,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
