/*******************************************************************************
 * smeRelation.c
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
#include <smxm.h>
#include <smxl.h>
#include <smg.h>
#include <smp.h>
#include <sma.h>
#include <sme.h>
#include <sms.h>
#include <smn.h>
#include <smd.h>
#include <smf.h>
#include <smkl.h>
#include <smeDef.h>
#include <smdDef.h>
#include <smnDef.h>

/**
 * @file smeRelation.c
 * @brief Storage Manager Layer Relation Manager Internal Routines
 */

/**
 * @addtogroup sme
 * @{
 */

extern smeIteratorModule gSmdpfhIteratorModule;
extern smeIteratorModule gSmdpdhIteratorModule;
extern smeIteratorModule gSmdmphIteratorModule;
extern smeIteratorModule gSmdmpfIteratorModule;
extern smeIteratorModule gSmdmppIteratorModule;
extern smeIteratorModule gSmnmpbIteratorModule;
extern smeIteratorModule gSmdmifIteratorModule;
extern smeIteratorModule gSmdmisIteratorModule;
extern smeIteratorModule gSmdmihIteratorModule;
extern smeIteratorModule gSmdmisVolIteratorModule;
extern smeIteratorModule gSmnmihIteratorModule;

extern smeGroupIteratorModule gSmdmisGroupIteratorModule;

smeIteratorModule * gSmeIteratorModules[SML_RELATION_TYPE_MAX] = 
{
    &gSmdpfhIteratorModule,      /**< for Fixed Table */
    &gSmdpdhIteratorModule,      /**< for Dump Table */
    &gSmdmphIteratorModule,      /**< for Memory Heap Table */
    &gSmdmpfIteratorModule,      /**< for Memory Heap Columnar Table */
    &gSmdmppIteratorModule,      /**< for Memory Pending Table */
    &gSmdmifIteratorModule,      /**< for Instant Flat Table */
    &gSmdmisIteratorModule,      /**< for Instant Sort Table */
    &gSmdmihIteratorModule,      /**< for Instant Hash Index */
    &gSmnmpbIteratorModule,      /**< for Memory Btree Index */
    &gSmnmihIteratorModule       /**< for Memory Hash Instant Index */
};

smeGroupIteratorModule * gSmeGroupIteratorModules[SML_RELATION_TYPE_MAX] =
{
    NULL,                        /**< for Fixed Table */
    NULL,                        /**< for Dump Table */
    NULL,                        /**< for Memory Heap Table */
    NULL,                        /**< for Memory Heap Columnar Table */
    NULL,                        /**< for Memory Pending Table */
    NULL,                        /**< for Instant Flat Table */
    &gSmdmisGroupIteratorModule, /**< for Instant Sort Table */
    NULL,                        /**< for Instant Hash Table */
    NULL                         /**< for Memory Btree Index */
};

/**
 * @brief Relation을 Fetch하기위한 함수 vecter를 반환한다.
 * @param[in] aRelationHandle Relation Handle
 */
inline smeIteratorModule * smeGetIteratorModule( void *aRelationHandle )
{
    smlRelationType sRelationType = SME_GET_RELATION_TYPE( aRelationHandle );

    if( sRelationType == SML_MEMORY_INSTANT_SORT_TABLE )
    {
        if( SMD_IS_VOLATILE_SORT_TABLE( aRelationHandle ) == STL_TRUE )
        {
            return &gSmdmisVolIteratorModule;
        }
    }
    
    return gSmeIteratorModules[sRelationType];
}

/**
 * @brief Relation을 Group Fetch하기위한 함수 vecter를 반환한다.
 * @param[in] aRelationType Relation의 타입
 */
inline smeGroupIteratorModule * smeGetGroupIteratorModule( smlRelationType aRelationType )
{
    return gSmeGroupIteratorModules[aRelationType];
}

/**
 * @brief Relation 핸들을 구축한다.
 * @param[in]     aTbsId      테이블스페이스 아이디
 * @param[in]     aSegPid     세그먼트 페이지 아이디
 * @param[in]     aSegHandle  세그먼트 핸들
 * @param[in,out] aEnv        Environment 구조체
 * @remarks 구축된 Relation 핸들은 aSegPid의 Volatile Area에 저장되고,
 *     <BR> 저장된 핸들은 smeGetRelationHandle을 통해서 접근가능하다.
 */
stlStatus smeBuildCache( smlTbsId   aTbsId,
                         smlPid     aSegPid,
                         void     * aSegHandle,
                         smlEnv   * aEnv )
{
    smpHandle           sPageHandle;
    stlChar           * sHeader;
    smeRelationHeader * sRelationHandle;
    stlUInt32           sState = 0;
    void              * sCaches[2];
    smeRelationHeader   sTmpRelHdr;
    smnIndexModule    * sModule;

    STL_TRY( smpFix( aTbsId,
                     aSegPid,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;
    
    sHeader = (stlChar*)smpGetBody( SMP_FRAME(&sPageHandle) ) + smsGetHeaderSize(aSegHandle);

    /* Relation 종류에 따른 handle 크기를 구하기 위해 임시 복사 */
    stlMemcpy( &sTmpRelHdr,
               sHeader, 
               STL_SIZEOF(smeRelationHeader) );
   
    /* unit test case에서는 relation header를 안만드는 경우도 있음 */
    if( smsHasRelHdr( aSegHandle ) == STL_TRUE )
    {
        STL_TRY( smgAllocShmMem4Open( sTmpRelHdr.mRelHeaderSize,
                                      (void**)&sRelationHandle,
                                      aEnv ) == STL_SUCCESS );

        /* 실제 Relation Handle로 데이터 복사 */
        stlMemcpy( sRelationHandle,
                   sHeader, 
                   sTmpRelHdr.mRelHeaderSize );

        sCaches[0] = aSegHandle;
        sCaches[1] = sRelationHandle;
        smpSetVolatileArea( &sPageHandle, sCaches, STL_SIZEOF(void*) * 2 );

        sState = 0;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

        /* volatile 정보 초기화 */
        SME_SET_SEGMENT_HANDLE(sRelationHandle, aSegHandle);
        SME_SET_OBJECT_SCN(sRelationHandle, 0);

        STL_TRY( smdBuildCache( sRelationHandle,
                                aEnv ) == STL_SUCCESS );

        if( SML_RELATION_IS_PERSISTENT_TABLE( sTmpRelHdr.mRelationType ) == STL_TRUE )
        {
            smdInitVolatileInfo( sRelationHandle, aEnv );
        }
        else if( SML_RELATION_IS_PERSISTENT_INDEX( sTmpRelHdr.mRelationType ) == STL_TRUE )
        {
            smnInitVolatileInfo( sRelationHandle, aEnv );

            sModule = smnGetIndexModule( SML_GET_INDEX_TYPE(sTmpRelHdr.mRelationType) );

            STL_TRY( sModule->mInitModuleSpecificData( sRelationHandle,
                                                       aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_ASSERT(STL_FALSE);
        }
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Startup 시점에 Aging하지 못한 Relation들을 Refine한다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smeRefineAtStartup( smlEnv * aEnv )
{
    smlRid         sSegMapRid;
    stlInt32       sState = 0;
    smlPid         sSegHdrPid;
    smlRid         sSegmentRid;
    void         * sSegMapHandle;
    void         * sRelHandle;
    void         * sSegHandle;
    void         * sSegIterator;
    stlInt32       sRelType;
    smxlTransId    sTransId;
    stlUInt64      sTransSeq;
    stlInt8        sSegState;
    stlInt64       sSegmentId;
    stlBool        sDone = STL_FALSE;
    smlStatement * sStatement;

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

        SMS_MAKE_SEGMENT_ID( &sSegmentId, &sSegmentRid );
        
        STL_TRY( smsGetSegmentHandle( sSegmentRid,
                                      &sSegHandle,
                                      aEnv )
                 == STL_SUCCESS );

        if( sSegHandle == NULL )
        {
            STL_TRY( smsGetSegmentState( sSegmentRid,
                                         &sSegState,
                                         aEnv )
                     == STL_SUCCESS );

            /**
             * Header 페이지만 할당된 경우라면 Header만 삭제한다.
             */
            if( sSegState == SMS_STATE_ALLOC_HDR )
            {
                STL_TRY( knlLogMsg( NULL,
                                    KNL_ENV(aEnv),
                                    KNL_LOG_LEVEL_INFO,
                                    ".... REFINE SEGMENT(%ld)\n",
                                    sSegmentId )
                         == STL_SUCCESS );
            
                STL_TRY( smxlAllocTrans( SML_TIL_READ_COMMITTED,
                                         STL_FALSE, /* aIsGlobalTrans */
                                         &sTransId,
                                         &sTransSeq,
                                         aEnv ) == STL_SUCCESS );
                sState = 3;

                smxlSetTransAttr( sTransId, SMXL_ATTR_UPDATABLE );
    
                STL_TRY( smsFreeSegMapPage( sTransId,
                                            sSegmentRid,
                                            aEnv )
                         == STL_SUCCESS );
                
                sState = 2;
                STL_TRY( smxlCommit( sTransId,
                                     NULL, /* aComment */
                                     SML_TRANSACTION_CWM_WAIT,
                                     aEnv ) == STL_SUCCESS );
                STL_TRY( smxlFreeTrans( sTransId,
                                        SML_PEND_ACTION_COMMIT,
                                        STL_FALSE, /* aCleanup */
                                        SML_TRANSACTION_CWM_WAIT,
                                        aEnv ) == STL_SUCCESS );
            }
            
            STL_TRY( smsGetNextPage( sSegMapHandle,
                                     sSegIterator,
                                     &sSegHdrPid,
                                     aEnv )
                     == STL_SUCCESS );
            continue;
        }

        /**
         * 현재까지 Relation을 갖지 않는 Segment는 Refine대상에 속하지 않는다.
         * 향후 그러한 segment가 만들어지면 해당 로직도 수정되어야 한다.
         */
        
        if( smsHasRelHdr( sSegHandle ) == STL_TRUE )
        {
            STL_TRY( smeGetRelationHandle( sSegmentRid,
                                           &sRelHandle,
                                           aEnv )
                     == STL_SUCCESS );

            sRelType = SME_GET_RELATION_TYPE( sRelHandle );
            
            if( SME_GET_RELATION_STATE( sRelHandle ) == SME_RELATION_STATE_DROPPING )
            {
                if( SML_RELATION_IS_PERSISTENT_TABLE( sRelType ) == STL_TRUE )
                {
                    STL_TRY( knlLogMsg( NULL,
                                        KNL_ENV(aEnv),
                                        KNL_LOG_LEVEL_INFO,
                                        ".... REFINE TABLE(%ld)\n",
                                        sSegmentId )
                             == STL_SUCCESS );
            
                    STL_TRY( smdDropAgingInternal( sRelHandle,
                                                   STL_TRUE,  /* aOnStartup */
                                                   &sDone,
                                                   aEnv )
                             == STL_SUCCESS );

                    STL_DASSERT( sDone == STL_TRUE );
                }
                
                if( SML_RELATION_IS_PERSISTENT_INDEX( sRelType ) == STL_TRUE )
                {
                    STL_TRY( knlLogMsg( NULL,
                                        KNL_ENV(aEnv),
                                        KNL_LOG_LEVEL_INFO,
                                        ".... REFINE INDEX(%ld)\n",
                                        sSegmentId )
                             == STL_SUCCESS );
            
                    STL_TRY( smnDropAgingInternal( sRelHandle,
                                                   STL_TRUE, /* aOnStartup */
                                                   &sDone,
                                                   aEnv )
                             == STL_SUCCESS );

                    STL_DASSERT( sDone == STL_TRUE );
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
        case 3:
            (void)smxlRollback( sTransId,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                STL_FALSE, /* aCleanup */
                                aEnv );
            (void)smxlFreeTrans( sTransId,
                                 SML_PEND_ACTION_ROLLBACK,
                                 STL_FALSE, /* aCleanup */
                                 SML_TRANSACTION_CWM_NO_WAIT,
                                 aEnv );
        case 2:
            (void)smsFreeSegIterator( sSegMapHandle, sSegIterator, aEnv );
        case 1:
            (void)smaFreeStatement( sStatement, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}


/**
 * @brief Startup 시점에 Relation Handle들을 구축한다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smeBuildCachesAtStartup( smlEnv * aEnv )
{
    smlRid          sSegMapRid;
    stlInt32        sState = 0;
    smlPid          sSegHdrPid;
    void          * sSegMapHandle;
    void          * sSegHandle;
    void          * sSegIterator;
    smlStatement  * sStatement;

    sSegMapRid = SMS_SEGMENT_MAP_RID;

    STL_TRY( smsBuildCache( sSegMapRid.mTbsId,
                            sSegMapRid.mPageId,
                            &sSegMapHandle,
                            aEnv ) == STL_SUCCESS );
    
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
        STL_TRY( smsBuildCache( sSegMapRid.mTbsId,
                                sSegHdrPid,
                                &sSegHandle,
                                aEnv )
                 == STL_SUCCESS );

        /**
         * Segment Handle을 할당할수 없는 경우는 Relation Cache를 만들지 않는다.
         * - Segment Build Cache Interface가 존재하지 않을때
         * - Segment Header Page만 할당되고, Body는 할당되지 않았을때
         */
        if( sSegHandle != NULL )
        {
            if( smsHasRelHdr( sSegHandle ) == STL_TRUE )
            {
                STL_TRY( smeBuildCache( sSegMapRid.mTbsId,
                                        sSegHdrPid,
                                        sSegHandle,
                                        aEnv ) == STL_SUCCESS );
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


/**
 * @brief 주어진 aSegRid에 해당하는 Relation 핸들을 얻는다.
 * @param[in]     aSegRid         Segment의 물리적 주소(segment rid)
 * @param[out]    aRelationHandle 얻은 릴레이션 핸들
 * @param[in,out] aEnv            Environment 정보
 * @note 해당 함수는 aSegRid에 해당하는 페이지 접근으로 인하여 DISK I/O를
 * <BR>  유발할수도 있다.
 * <BR>  극단적인 성능을 요한다면 해당 함수의 사용은 피하는것이 좋다.
 */
stlStatus smeGetRelationHandle( smlRid    aSegRid,
                                void   ** aRelationHandle,
                                smlEnv  * aEnv )
{
    smpHandle    sPageHandle;
    void      ** sVolatileArea;
    stlInt32     sState = 0;
    
    STL_TRY( smpFix( aSegRid.mTbsId,
                     aSegRid.mPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    sVolatileArea = (void**)smpGetVolatileArea( &sPageHandle );

    *aRelationHandle = (void*)sVolatileArea[1];
    
    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;

}

/**
 * @brief RELATION 힌트 정보를 생성한다.
 * @param[in]     aSessionEnv Session Environment 정보
 * @param[in,out] aEnv        Environment 정보
 */
stlStatus smeCreateRelHint( smlSessionEnv * aSessionEnv,
                            smlEnv        * aEnv )
{
    stlInt32      i;
    smeHintPool * sRelHintPool;
    
    STL_TRY( knlAllocRegionMem( &aSessionEnv->mSessionMem,
                                STL_SIZEOF( smeHintPool ),
                                &aSessionEnv->mRelHintPool,
                                KNL_ENV(aEnv) ) == STL_SUCCESS );

    sRelHintPool = aSessionEnv->mRelHintPool;

    sRelHintPool->mSequence = 0;
    for( i = 0; i < SME_MAX_SEGMENT_HINT; i++ )
    {
        sRelHintPool->mRelHint[i].mRelHandle = NULL;
        sRelHintPool->mRelHint[i].mValidScn = 0;
        sRelHintPool->mRelHint[i].mHintPid = SMP_NULL_PID;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief RELATION 힌트 정보를 검색한다.
 * @param[in]     aCache      대상 Relation Cache
 * @param[in]     aValidScn   유효한 RELATION을 찾기 위한 SCN
 * @param[in,out] aEnv        Environment 정보
 */
smeHint * smeFindRelHint( void   * aCache,
                          smlScn   aValidScn,
                          smlEnv * aEnv )
{
    stlInt32       i;
    smeHintPool  * sRelHintPool;
    smeHint      * sRelHint = NULL;

    sRelHintPool = (smeHintPool*)SML_SESS_ENV(aEnv)->mRelHintPool;
    
    for( i = 0; i < SME_MAX_SEGMENT_HINT; i++ )
    {
        if( (aCache == sRelHintPool->mRelHint[i].mRelHandle) &&
            (aValidScn == sRelHintPool->mRelHint[i].mValidScn) )
        {
            sRelHint = (smeHint*)&(sRelHintPool->mRelHint[i]);
            break;
        }
    }

    return sRelHint;
}

/**
 * @brief RELATION 힌트 정보를 추가한다.
 * @param[in]     aHintPid    캐시에 저장할 Page 아이디
 * @param[in]     aCache      대상 Relation Cache
 * @param[in]     aValidScn   대상 Relation Cache가 갖는 SCN
 * @param[in,out] aEnv        Environment 정보
 * @return 추가된 힌트 정보
 */
smeHint * smeAddNewRelHint( smlPid     aHintPid,
                            void     * aCache,
                            smlScn     aValidScn,
                            smlEnv   * aEnv )
{
    smeHintPool  * sRelHintPool;
    smeHint      * sRelHint = NULL;

    sRelHintPool = (smeHintPool*)SML_SESS_ENV(aEnv)->mRelHintPool;

    sRelHint = (smeHint*)&(sRelHintPool->mRelHint[sRelHintPool->mSequence % SME_MAX_SEGMENT_HINT]);
    sRelHint->mRelHandle = aCache;
    sRelHint->mValidScn = aValidScn;
    sRelHint->mHintPid = aHintPid;
    
    sRelHintPool->mSequence += 1;

    return sRelHint;
}

void smeSetJoinFilter4InstRelation( smlBlockJoinFetchInfo  * aJoinFetchInfo )
{
    knlDataValueList  * sConstList = NULL;
    knlValueBlockList * sBlockList = NULL;

    dtlDataValue * sSrcVal = NULL;

    /* Left Constant */
    sConstList = aJoinFetchInfo->mLeftConst4JoinFilter;
    sBlockList = aJoinFetchInfo->mLeftBlock4JoinFilter;
    while( sConstList != NULL )
    {
        sSrcVal = KNL_GET_BLOCK_DATA_VALUE( sBlockList, aJoinFetchInfo->mLeftBlockIdx );
        
        sConstList->mConstVal->mValue = sSrcVal->mValue;
        sConstList->mConstVal->mLength = sSrcVal->mLength;

        sConstList = sConstList->mNext;
        sBlockList = sBlockList->mNext;
    }

    /* Right Constant */
    sConstList = aJoinFetchInfo->mRightConst4JoinFilter;
    sBlockList = aJoinFetchInfo->mRightBlock4JoinFilter;
    while( sConstList != NULL )
    {
        sSrcVal = KNL_GET_BLOCK_DATA_VALUE( sBlockList, aJoinFetchInfo->mResultBlockIdx );
        
        sConstList->mConstVal->mValue = sSrcVal->mValue;
        sConstList->mConstVal->mLength = sSrcVal->mLength;

        sConstList = sConstList->mNext;
        sBlockList = sBlockList->mNext;
    }
}

void smeSetPhysicalFilter4InstRelation( smlFetchInfo  *  aFetchInfo )
{
    knlPhysicalFilter * sPhysicalFilter = NULL;

    /* Index Key Physical Filter */
    sPhysicalFilter = aFetchInfo->mPhysicalFilter;
    while( sPhysicalFilter != NULL )
    {
        if( sPhysicalFilter->mConstVal != NULL )
        {
            if( sPhysicalFilter->mColIdx2 == KNL_PHYSICAL_LIST_VALUE_CONSTANT )
            {
                /* Do Nothing */
            }
            else
            {
                sPhysicalFilter->mColVal2->mValue  = sPhysicalFilter->mConstVal->mValue;
                sPhysicalFilter->mColVal2->mLength = sPhysicalFilter->mConstVal->mLength;
            }
        }
        sPhysicalFilter = sPhysicalFilter->mNext;
    }

    /* Table Physical Filter */
    if( aFetchInfo->mFetchRecordInfo != NULL )
    {
        sPhysicalFilter = aFetchInfo->mFetchRecordInfo->mPhysicalFilter;
        while( sPhysicalFilter != NULL )
        {
            if( sPhysicalFilter->mConstVal != NULL )
            {
                if( sPhysicalFilter->mColIdx2 == KNL_PHYSICAL_LIST_VALUE_CONSTANT )
                {
                    /* Do Nothing */
                }
                else
                {
                    sPhysicalFilter->mColVal2->mValue  = sPhysicalFilter->mConstVal->mValue;
                    sPhysicalFilter->mColVal2->mLength = sPhysicalFilter->mConstVal->mLength;
                }
            }
            sPhysicalFilter = sPhysicalFilter->mNext;
        }
    }
}

    
/** @} */
