/*******************************************************************************
 * smtExtMgr.c
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
#include <smf.h>
#include <smp.h>
#include <smtDef.h>
#include <smt.h>
#include <smg.h>
#include <sma.h>
#include <smxm.h>
#include <smtTbs.h>
#include <smtExtMgr.h>

/**
 * @file smtExtMgr.c
 * @brief Storage Manager Layer Extent Manager Routines
 */

/**
 * @addtogroup smt
 * @{
 */

/**
 * @brief Tablespace로부터 한개의 Extent를 할당 받는다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aTbsId 대상 Tablespace Identifier
 * @param[out] aExtId 할당받은 Extent Identifier
 * @param[out] aFirstDataPid Extent내의 첫번째 Data Page Identifier
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtAllocExtent( smxmTrans  * aMiniTrans,
                          smlTbsId     aTbsId,
                          smlExtId   * aExtId,
                          smlPid     * aFirstDataPid,
                          smlEnv     * aEnv )
{
    smpHandle       sExtMapHandle;
    smpHandle       sPageHandle;
    smtExtMapHdr  * sExtMapHdr;
    stlChar       * sExtBitArray;
    stlUInt32       sState = 0;
    stlBool         sFound = STL_FALSE;
    stlUInt16       i;
    smxmSavepoint   sSavepoint;
    smxmTrans       sMiniTrans;
    smpHandle     * sPageCachedHandle;
    smlPid          sExtMapPid;
    smpFreeness     sFreeness;
    smtHint       * sTbsHint;
    stlUInt16       sPageCountInExt;
    stlBool         sIsSuccess;
    stlBool         sIsRetry;

    sTbsHint = smtFindTbsHint( aTbsId, aEnv );
    sPageCountInExt = smfGetPageCountInExt( aTbsId );

    if( sTbsHint == NULL )
    {
        STL_TRY( smtFindExtBlockMap( aMiniTrans,
                                     aTbsId,
                                     &sExtMapPid,
                                     aEnv ) == STL_SUCCESS );
        if( sExtMapPid == SMP_NULL_PID )
        {
            STL_TRY( smtExtendTablespace( aMiniTrans,
                                          aTbsId,
                                          &sExtMapPid,
                                          aEnv)
                     == STL_SUCCESS );
        }

        STL_TRY( sExtMapPid != SMP_NULL_PID );
    }
    else
    {
        sExtMapPid = sTbsHint->mHintPid;
    }

    STL_RAMP( RAMP_RETRY );

    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                &sSavepoint,
                                aEnv ) == STL_SUCCESS );
    sPageCachedHandle = smxmGetPageHandle( aMiniTrans,
                                           aTbsId,
                                           sExtMapPid,
                                           KNL_LATCH_MODE_EXCLUSIVE );

    if( sPageCachedHandle == NULL )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             aTbsId,
                             sExtMapPid,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sExtMapHandle,
                             aEnv ) == STL_SUCCESS );
    }
    else
    {
        sExtMapHandle = *sPageCachedHandle;
    }

    STL_TRY_THROW( ((smpPhyHdr*)SMP_FRAME(&sExtMapHandle))->mPageType ==
                   SMP_PAGE_TYPE_EXT_MAP,
                   RAMP_ERR_INTERNAL );

    sFreeness = smpGetFreeness( &sExtMapHandle );

    if( sFreeness == SMP_FREENESS_FREE )
    {
        sExtMapHdr = (smtExtMapHdr*)smpGetBody( SMP_FRAME( &sExtMapHandle ) );
        sExtBitArray = SMT_EXT_BIT_ARRAY( sExtMapHdr );

        /**
         * 한번도 사용된적이 없다면 Extent Map Header의 상태를 변경해야 한다.
         */
        if( sExtMapHdr->mOnExtCount == 0 )
        {
            STL_TRY( smtUpdateExtBlockMapHdrState( aMiniTrans,
                                                   aTbsId,
                                                   SMP_DATAFILE_ID(sExtMapPid),
                                                   SMT_EBMS_USED,
                                                   &sIsSuccess,
                                                   &sIsRetry,
                                                   aEnv ) == STL_SUCCESS );
            
            if( sIsSuccess == STL_FALSE )
            {
                STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                                  &sSavepoint,
                                                  aEnv ) == STL_SUCCESS );

                /**
                 * 해당 데이터파일이 삭제중이라면 다시 시도한다.
                 */
                if( sIsRetry == STL_TRUE )
                {
                    STL_TRY( smtFindExtBlockMap( aMiniTrans,
                                                 aTbsId,
                                                 &sExtMapPid,
                                                 aEnv ) == STL_SUCCESS );
                    if( sExtMapPid == SMP_NULL_PID )
                    {
                        STL_TRY( smtExtendTablespace( aMiniTrans,
                                                      aTbsId,
                                                      &sExtMapPid,
                                                      aEnv)
                                 == STL_SUCCESS );
                    }
                }
            
                STL_THROW( RAMP_RETRY );
            }

        }
        
        for( i = 0; i < sExtMapHdr->mExtCount; i++ )
        {
            if( smtIsFreeExt( sExtBitArray, i ) == STL_TRUE )
            {
                if( sExtMapHdr->mOffExtCount == 1 )
                {
                    STL_TRY( smtUpdateExtBlockDescState( aMiniTrans,
                                                         aTbsId,
                                                         sExtMapPid,
                                                         SMT_EBS_FULL,
                                                         &sIsSuccess,
                                                         aEnv ) == STL_SUCCESS );

                    if( sIsSuccess == STL_FALSE )
                    {
                        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                                          &sSavepoint,
                                                          aEnv ) == STL_SUCCESS );
                        STL_THROW( RAMP_RETRY );
                    }

                    STL_TRY( smpUpdateFreeness( aMiniTrans,
                                                aTbsId,
                                                &sExtMapHandle,
                                                SMP_FREENESS_FULL,
                                                aEnv ) == STL_SUCCESS );
                }
                
                smtOnExtBit( sExtBitArray, i );

                STL_TRY( smxmWriteLog( aMiniTrans,
                                       SMG_COMPONENT_TABLESPACE,
                                       SMR_LOG_ALLOC_EXTENT,
                                       SMR_REDO_TARGET_PAGE,
                                       NULL,
                                       0,
                                       aTbsId,
                                       sExtMapPid,
                                       i,
                                       SMXM_LOGGING_REDO_UNDO,
                                       aEnv ) == STL_SUCCESS );

                *aFirstDataPid = sExtMapPid + 1 + (i * sPageCountInExt);
                *aExtId = SMT_MAKE_EXTENT_ID( sExtMapPid, i );

                sExtMapHdr->mOnExtCount += 1;
                sExtMapHdr->mOffExtCount -= 1;
                
                sFound = STL_TRUE;

                break;
            }
        }
    }
            
    if( sFound == STL_FALSE )
    {
        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          &sSavepoint,
                                          aEnv ) == STL_SUCCESS );
        
        STL_TRY( smtFindExtBlockMap( aMiniTrans,
                                     aTbsId,
                                     &sExtMapPid,
                                     aEnv ) == STL_SUCCESS );
        
        if( sExtMapPid == SMP_NULL_PID )
        {
            STL_TRY( smtExtendTablespace( aMiniTrans,
                                          aTbsId,
                                          &sExtMapPid,
                                          aEnv)
                     == STL_SUCCESS );
        }

        STL_TRY( sExtMapPid != SMP_NULL_PID );

        STL_THROW( RAMP_RETRY );
    }

    STL_TRY( smtValidateExtentMap( &sExtMapHandle,
                                   aEnv )
             == STL_SUCCESS );
    
    for( i = 0; i < sPageCountInExt; i++ )
    {
        if( ( i % 16 ) == 0 )
        {
            if( sState == 1 )
            {
                sState = 0;
                STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
            }
    
            STL_TRY( smxmBegin( &sMiniTrans,
                                smxmGetTransId( aMiniTrans ),
                                SML_INVALID_RID,
                                SMXM_ATTR_REDO | (aMiniTrans->mAttr & SMXM_ATTR_NO_VALIDATE_PAGE),
                                aEnv ) == STL_SUCCESS );
            sState = 1;
        }
    
        STL_TRY( smpCreate( &sMiniTrans,
                            aTbsId,
                            *aFirstDataPid + i,
                            SMP_PAGE_TYPE_UNFORMAT,
                            0, /* aSegmentId */
                            0,  /* aScn */
                            &sPageHandle,
                            aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    if( sTbsHint == NULL )
    {
        smtAddNewTbsHint( sExtMapPid, aTbsId, aEnv );
    }
    else
    {
        sTbsHint->mHintPid = sExtMapPid;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smtAllocExtent()",
                      0 );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmCommit( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Tablespace가 Full 상태이면 Datafile을 extend한다.<BR>
 * Tablespace의 datafile 가운데, auto extend인 datafile을 찾아서,<BR>
 * 현재 Size가 Max Size까지 도달하지 않았을 경우에 datafile을 확장 한 후<BR>
 * Extend Block을 반환한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aTbsId 대상 Tablespace Identifier
 * @param[out] aExtMapPid 확장된 Datafile의 Extent Block의 Page Identifier
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtExtendTablespace( smxmTrans  * aMiniTrans,
                               smlTbsId     aTbsId,
                               smlPid     * aExtMapPid,
                               smlEnv     * aEnv )
{
    smlDatafileId   sDatafileId;
    stlBool         sExtendible = STL_FALSE;
    stlChar         sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    STL_TRY_THROW( smfFindExtendibleDatafile( aTbsId,
                                              &sDatafileId,
                                              &sExtendible,
                                              aEnv )
                   == STL_SUCCESS, RAMP_ERR_NO_MORE_EXTENDIBLE_DATAFILE );
    STL_TRY_THROW( sExtendible == STL_TRUE, RAMP_ERR_NO_MORE_EXTENDIBLE_DATAFILE );
    STL_TRY( smfExtendDatafile( aMiniTrans,
                                aTbsId,
                                sDatafileId,
                                aEnv ) == STL_SUCCESS );
    STL_TRY( smtFindExtBlockMap( aMiniTrans,
                                 aTbsId,
                                 aExtMapPid,
                                 aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NO_MORE_EXTENDIBLE_DATAFILE )
    {
        smfGetTbsName( aTbsId, sTbsName, aEnv );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NO_MORE_EXTENDIBLE_DATAFILE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sTbsName );

        if( aTbsId != SML_MEMORY_UNDO_SYSTEM_TBS_ID )
        {
            (void) knlLogMsg( NULL,
                              KNL_ENV( aEnv ),
                              KNL_LOG_LEVEL_WARNING,
                              "[TABLESPACE] datafile extending was failed\n" );
        }
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace에 주어진 Extent를 반납한다.
 * @param[in] aMiniTrans Mini-Transaction 포인터
 * @param[in] aTbsId 대상 Tablespace Identifier
 * @param[in] aExtId 반납할 Extent Identifier
 * @param[in] aIsSuccess 래치 획득에 대한 성공 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtFreeExtent( smxmTrans  * aMiniTrans,
                         smlTbsId     aTbsId,
                         smlExtId     aExtId,
                         stlBool    * aIsSuccess,
                         smlEnv     * aEnv )
{
    smlPid         sExtPid;
    stlUInt16      sOffset;
    smpHandle      sExtMapHandle;
    smtExtMapHdr * sExtMapHdr;
    stlChar      * sExtBitArray;
    smpHandle    * sPageHandle;
    smpFreeness    sFreeness;
    smxmSavepoint  sSavepoint;
    stlBool        sIsSuccess;

    *aIsSuccess = STL_TRUE;
    
    sExtPid = SMT_EXTENT_PID( aExtId );
    sOffset = SMT_EXTENT_OFFSET( aExtId );

    KNL_SYSTEM_FATAL_DASSERT( sOffset < SMP_PAGE_SIZE, aEnv );

    STL_RAMP( RAMP_RETRY );
    
    STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                &sSavepoint,
                                aEnv ) == STL_SUCCESS );
    
    sPageHandle = smxmGetPageHandle( aMiniTrans,
                                     aTbsId,
                                     sExtPid,
                                     KNL_LATCH_MODE_EXCLUSIVE );

    if( sPageHandle == NULL )
    {
        STL_TRY( smpTryAcquire( aMiniTrans,
                                aTbsId,
                                sExtPid,
                                KNL_LATCH_MODE_EXCLUSIVE,
                                &sExtMapHandle,
                                aIsSuccess,
                                aEnv ) == STL_SUCCESS );

        STL_TRY_THROW( *aIsSuccess == STL_TRUE, RAMP_RETURN );
    }
    else
    {
        sExtMapHandle = *sPageHandle;
    }
                 
    sExtMapHdr = (smtExtMapHdr*)smpGetBody( SMP_FRAME( &sExtMapHandle ) );
    sExtBitArray = SMT_EXT_BIT_ARRAY( sExtMapHdr );

    KNL_SYSTEM_FATAL_DASSERT( smtIsFreeExt( sExtBitArray, sOffset ) == STL_FALSE,
                              aEnv );

    if( sExtMapHdr->mOffExtCount > 0 )
    {
        sFreeness = smpGetFreeness( &sExtMapHandle );

        if( sFreeness == SMP_FREENESS_FULL )
        {
            STL_TRY( smtUpdateExtBlockDescState( aMiniTrans,
                                                 aTbsId,
                                                 sExtPid,
                                                 SMT_EBS_FREE,
                                                 &sIsSuccess,
                                                 aEnv ) == STL_SUCCESS );

            if( sIsSuccess == STL_FALSE )
            {
                STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                                  &sSavepoint,
                                                  aEnv ) == STL_SUCCESS );
                STL_THROW( RAMP_RETRY );
            }

            STL_TRY( smpUpdateFreeness( aMiniTrans,
                                        aTbsId,
                                        &sExtMapHandle,
                                        SMP_FREENESS_FREE,
                                        aEnv ) == STL_SUCCESS );
        }
    }
    
    smtOffExtBit( sExtBitArray, sOffset );

    sExtMapHdr->mOnExtCount -= 1;
    sExtMapHdr->mOffExtCount += 1;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLESPACE,
                           SMR_LOG_FREE_EXTENT,
                           SMR_REDO_TARGET_PAGE,
                           NULL,
                           0,
                           aTbsId,
                           sExtPid,
                           sOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( smtValidateExtentMap( &sExtMapHandle,
                                   aEnv )
             == STL_SUCCESS );
    
    STL_RAMP( RAMP_RETURN );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 테이블스페이스 힌트 정보를 생성한다.
 * @param[in]     aSessionEnv Session Environment 정보
 * @param[in,out] aEnv        Environment 정보
 */
stlStatus smtCreateTbsHint( smlSessionEnv * aSessionEnv,
                            smlEnv        * aEnv )
{
    stlInt32      i;
    smtHintPool * sTbsHintPool;
    
    STL_TRY( knlAllocRegionMem( &aSessionEnv->mSessionMem,
                                STL_SIZEOF( smtHintPool ),
                                &aSessionEnv->mTbsHintPool,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
        
    sTbsHintPool = aSessionEnv->mTbsHintPool;

    sTbsHintPool->mSequence = 0;
    for( i = 0; i < SMS_MAX_TBS_HINT; i++ )
    {
        sTbsHintPool->mTbsHint[i].mTbsId = SMT_INVALID_TBS_HINT;
        sTbsHintPool->mTbsHint[i].mHintPid = SMP_NULL_PID;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

/**
 * @addtogroup smtExtMgr
 * @{
 */

void smtOnExtBit( stlChar * aBitArray, stlUInt16 aOffset )
{
    stlChar * sExtBit;
    stlChar   sExtBitMask;

    sExtBit = aBitArray + ( aOffset / STL_BIT_COUNT(stlChar) );

    sExtBitMask = 1 << (aOffset % STL_BIT_COUNT(stlChar));
    *sExtBit |= sExtBitMask;
}

void smtOffExtBit( stlChar * aBitArray, stlUInt16 aOffset )
{
    stlChar * sExtBit;
    stlChar   sExtBitMask;

    sExtBit = aBitArray + ( aOffset / STL_BIT_COUNT(stlChar) );

    sExtBitMask = 1 << (aOffset % STL_BIT_COUNT(stlChar));
    *sExtBit &= ~sExtBitMask;
}

stlBool smtIsFreeExt( stlChar * aBitArray, stlUInt16 aOffset )
{
    stlChar   sExtBitMask;
    stlChar   sIsFree;

    sExtBitMask = 1 << (aOffset % STL_BIT_COUNT(stlChar));

    sIsFree = sExtBitMask & *(aBitArray + (aOffset / STL_BIT_COUNT(stlChar)));

    if( sIsFree == 0 )
    {
        return STL_TRUE;
    }

    return STL_FALSE;
}

stlStatus smtUpdateExtBlockDescState( smxmTrans   * aMiniTrans,
                                      smlTbsId      aTbsId,
                                      smlPid        aExtMapPid,
                                      stlUInt32     aState,
                                      stlBool     * aIsSuccess,
                                      smlEnv      * aEnv )
{
    smlPid              sExtBlockMapPid;
    smpHandle           sExtBlockMapHandle;
    stlChar             sLogData[STL_SIZEOF(stlInt32) * 2];
    stlInt32            sLogOffset = 0;
    stlUInt32           sBeforeState = 0;
    stlUInt32           i;
    smtExtBlockMapHdr * sExtBlockMapHdr;
    smtExtBlockMap    * sExtBlockMap;
    smpHandle         * sCachedPageHandle;
    stlBool             sFound = STL_FALSE;

    *aIsSuccess = STL_TRUE;
    
    sExtBlockMapPid = SMP_MAKE_PID( SMP_DATAFILE_ID( aExtMapPid ),
                                    SMT_EXT_BLOCK_MAP_PAGE_SEQ );

    sCachedPageHandle = smxmGetPageHandle( aMiniTrans,
                                           aTbsId,
                                           sExtBlockMapPid,
                                           KNL_LATCH_MODE_EXCLUSIVE );

    if( sCachedPageHandle == NULL )
    {
        STL_TRY( smpTryAcquire( aMiniTrans,
                                aTbsId,
                                sExtBlockMapPid,
                                KNL_LATCH_MODE_EXCLUSIVE,
                                &sExtBlockMapHandle,
                                aIsSuccess,
                                aEnv ) == STL_SUCCESS );

        STL_TRY_THROW( *aIsSuccess == STL_TRUE, RAMP_RETURN );
    }
    else
    {
        sExtBlockMapHandle = *sCachedPageHandle;
    }
    
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( &sExtBlockMapHandle ) );
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));

    for( i = 0; i < sExtBlockMapHdr->mExtBlockDescCount; i++ )
    {
        if( sExtBlockMap->mExtBlockDesc[i].mExtBlock == aExtMapPid )
        {
            sBeforeState = sExtBlockMap->mExtBlockDesc[i].mState;
            aState |= sExtBlockMap->mExtBlockDesc[i].mState & ~SMT_EBS_MASK;
            sExtBlockMap->mExtBlockDesc[i].mState = aState;
            sFound = STL_TRUE;
            break;
        }
    }

    STL_ASSERT( sFound == STL_TRUE );
    
    STL_WRITE_MOVE_INT32( sLogData, &sBeforeState, sLogOffset );
    STL_WRITE_MOVE_INT32( sLogData, &aState, sLogOffset );

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_TABLESPACE,
                           SMR_LOG_UPDATE_EXT_BLOCK_DESC_STATE,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           sLogOffset,
                           aTbsId,
                           sExtBlockMapPid,
                           i,
                           SMXM_LOGGING_REDO_UNDO,
                           aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_RETURN );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smtUpdateExtBlockMapHdrState( smxmTrans   * aMiniTrans,
                                        smlTbsId      aTbsId,
                                        stlUInt32     aDatafileId,
                                        stlUInt32     aState,
                                        stlBool     * aIsSuccess,
                                        stlBool     * aIsRetry,
                                        smlEnv      * aEnv )
{
    smlPid              sExtBlockMapPid;
    smpHandle           sExtBlockMapHandle;
    stlChar             sLogData[STL_SIZEOF(stlInt32) * 2];
    stlInt32            sLogOffset = 0;
    smtExtBlockMapHdr * sExtBlockMapHdr;
    smpHandle         * sCachedPageHandle;

    *aIsSuccess = STL_TRUE;
    *aIsRetry = STL_FALSE;
    
    sExtBlockMapPid = SMP_MAKE_PID( aDatafileId, SMT_EXT_BLOCK_MAP_PAGE_SEQ );

    sCachedPageHandle = smxmGetPageHandle( aMiniTrans,
                                           aTbsId,
                                           sExtBlockMapPid,
                                           KNL_LATCH_MODE_EXCLUSIVE );

    if( sCachedPageHandle == NULL )
    {
        STL_TRY( smpTryAcquire( aMiniTrans,
                                aTbsId,
                                sExtBlockMapPid,
                                KNL_LATCH_MODE_EXCLUSIVE,
                                &sExtBlockMapHandle,
                                aIsSuccess,
                                aEnv ) == STL_SUCCESS );

        STL_TRY_THROW( *aIsSuccess == STL_TRUE, RAMP_RETURN );
    }
    else
    {
        sExtBlockMapHandle = *sCachedPageHandle;
    }
    
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( &sExtBlockMapHandle ) );

    /**
     * Dropping Datafile은 Skip 한다.
     */
    if( (sExtBlockMapHdr->mState & SMT_EBMS_MASK) == SMT_EBMS_DROP )
    {
        *aIsSuccess = STL_FALSE;
        *aIsRetry = STL_TRUE;
        STL_THROW( RAMP_RETURN );
    }
    
    if( (sExtBlockMapHdr->mState & SMT_EBMS_MASK) == SMT_EBMS_FREE )
    {
        sExtBlockMapHdr->mState = aState;
        
        sLogOffset = 0;
        STL_WRITE_MOVE_INT32( sLogData, &(sExtBlockMapHdr->mState), sLogOffset );
        
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_TABLESPACE,
                               SMR_LOG_UPDATE_EXT_BLOCK_MAP_HDR_STATE,
                               SMR_REDO_TARGET_PAGE,
                               (void*)sLogData,
                               sLogOffset,
                               aTbsId,
                               sExtBlockMapPid,
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_RETURN );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smtAddExtBlock( smlStatement * aStatement,
                          smlTbsId       aTbsId,
                          smlPid         aExtBlockPid,
                          smlEnv       * aEnv )
{
    smpHandle              sExtBlockMapHandle;
    smlPid                 sExtBlockMapPid;
    smtExtBlockMapHdr    * sExtBlockMapHdr;
    smtExtBlockMap       * sExtBlockMap;
    smtExtBlockDesc      * sExtBlockDesc;
    stlUInt32              sState = 0;
    smtExtBlockPendingArgs sPendingArgs;
    smxmTrans              sMiniTrans;
    smxlTransId            sTransId;

    if( aStatement == NULL )
    {
        sTransId = SML_INVALID_TRANSID;
    }
    else
    {
        sTransId = SMA_GET_TRANS_ID(aStatement);
    }

    sExtBlockMapPid = SMP_MAKE_PID( SMP_DATAFILE_ID( aExtBlockPid ),
                                    SMT_EXT_BLOCK_MAP_PAGE_SEQ );

    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        SML_INVALID_RID,
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
                
    STL_TRY( smpAcquire( &sMiniTrans,
                         aTbsId,
                         sExtBlockMapPid,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sExtBlockMapHandle,
                         aEnv ) == STL_SUCCESS );

    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( &sExtBlockMapHandle ) );
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));

    sExtBlockDesc = &sExtBlockMap->mExtBlockDesc[sExtBlockMapHdr->mExtBlockDescCount];
        
    if( sExtBlockMapHdr->mExtBlockDescCount < SMT_MAX_EXT_BLOCK_DESC_COUNT )
    {
        sExtBlockDesc->mExtBlock = aExtBlockPid;
        sExtBlockDesc->mState = SMT_EBS_PREPARED | SMT_EBS_FREE;
            
        sExtBlockMapHdr->mExtBlockDescCount++;

        stlMemset( &sPendingArgs, 0x00, STL_SIZEOF(smtExtBlockPendingArgs) );
        sPendingArgs.mExtBlockMapPid = sExtBlockMapPid;
        sPendingArgs.mTbsId = aTbsId;
        sPendingArgs.mOffset = sExtBlockMapHdr->mExtBlockDescCount - 1;

        if( aStatement != NULL )
        {
            STL_TRY( smgAddPendOp( aStatement,
                                   SMG_PEND_ADD_EXTENT_BLOCK,
                                   SML_PEND_ACTION_COMMIT,
                                   (void*)&sPendingArgs,
                                   STL_SIZEOF( smtExtBlockPendingArgs ),
                                   0,
                                   aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sExtBlockDesc->mState = SMT_EBS_FREE;
        }
            
        STL_TRY( smxmWriteLog( &sMiniTrans,
                               SMG_COMPONENT_TABLESPACE,
                               SMR_LOG_ADD_EXTENT_BLOCK,
                               SMR_REDO_TARGET_PAGE,
                               (void*)&aExtBlockPid,
                               STL_SIZEOF( aExtBlockPid ),
                               aTbsId,
                               sExtBlockMapPid,
                               sExtBlockMapHdr->mExtBlockDescCount - 1,
                               SMXM_LOGGING_REDO_UNDO,
                               aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_ASSERT( 0 );
    }

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smtAddExtBlockPending( stlUInt32   aActionFlag,
                                 smgPendOp * aPendOp,
                                 smlEnv    * aEnv )
{
    smtExtBlockPendingArgs * sArgs;
    smpHandle                sExtBlockMapHandle;
    stlUInt32                sState = 0;
    smtExtBlockMapHdr      * sExtBlockMapHdr;
    smtExtBlockMap         * sExtBlockMap;

    sArgs = (smtExtBlockPendingArgs*)aPendOp->mArgs;

    STL_TRY( smpAcquire( NULL,
                         sArgs->mTbsId,
                         sArgs->mExtBlockMapPid,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sExtBlockMapHandle,
                         aEnv ) == STL_SUCCESS );
    sState = 1;

    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( &sExtBlockMapHandle ) );
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));
    
    sExtBlockMap->mExtBlockDesc[sArgs->mOffset].mState = SMT_EBS_FREE;
    
    STL_TRY( smpSetDirty( &sExtBlockMapHandle, aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smpRelease( &sExtBlockMapHandle, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sExtBlockMapHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smtCreateExtBlock( smxlTransId   aTransId,
                             smlTbsId      aTbsId,
                             smlPid        aExtBlockPid,
                             stlUInt32     aExtCount,
                             stlUInt32     aMaxExtCount,
                             smlEnv      * aEnv )
{
    smpHandle      sExtMapHandle;
    smtExtMapHdr * sExtMapHdr;
    stlChar      * sExtBitArray;
    stlUInt32      i;
    stlUInt32      sState = 0;
    smxmTrans      sMiniTrans;
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        SML_INVALID_RID,
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smpCreate( &sMiniTrans,
                        aTbsId,
                        aExtBlockPid,
                        SMP_PAGE_TYPE_EXT_MAP,
                        0, /* aSegmentId */
                        0, /* aScn */
                        &sExtMapHandle,
                        aEnv ) == STL_SUCCESS );

    sExtMapHdr = (smtExtMapHdr*)smpGetBody( SMP_FRAME( &sExtMapHandle ) );
    sExtBitArray = SMT_EXT_BIT_ARRAY( sExtMapHdr );
    sExtMapHdr->mOnExtCount = 0;
    sExtMapHdr->mOffExtCount = aExtCount;

    for( i = 0; i < aExtCount; i++ )
    {
        smtOffExtBit( sExtBitArray, i );
    }
            
    sExtMapHdr->mExtCount = aExtCount;
    sExtMapHdr->mMaxExtCount = aMaxExtCount;

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_TABLESPACE,
                           SMR_LOG_CREATE_EXTENT_BLOCK,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sExtMapHdr,
                           STL_SIZEOF( smtExtMapHdr ),
                           aTbsId,
                           aExtBlockPid,
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
            
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}
                          
stlStatus smtCreateExtBlockMap( smxlTransId   aTransId,
                                smlTbsId      aTbsId,
                                smlPid        aExtBlockMapPid,
                                smlEnv      * aEnv )
{
    smpHandle           sExtBlockMapHandle;
    stlUInt32           sState = 0;
    smtExtBlockMapHdr * sExtBlockMapHdr;
    smtExtBlockMap    * sExtBlockMap;
    stlUInt32           i;
    smxmTrans           sMiniTrans;
    stlInt64            sHintSequence;
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        SML_INVALID_RID,
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smpCreate( &sMiniTrans,
                        aTbsId,
                        aExtBlockMapPid,
                        SMP_PAGE_TYPE_EXT_BLOCK_MAP,
                        0, /* aSegmentId */
                        0, /* aScn */
                        &sExtBlockMapHandle,
                        aEnv ) == STL_SUCCESS );

    sHintSequence = 0;
    smpSetVolatileArea( &sExtBlockMapHandle, &sHintSequence, STL_SIZEOF( stlInt64 ) );
    
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( &sExtBlockMapHandle ) );
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));

    for( i = 0; i < SMT_MAX_EXT_BLOCK_DESC_COUNT; i++ )
    {
        sExtBlockMap->mExtBlockDesc[i].mExtBlock = SMP_NULL_PID;
        sExtBlockMap->mExtBlockDesc[i].mState = SMT_EBS_UNALLOCATED;
    }

    sExtBlockMapHdr->mExtBlockDescCount = 0;
    sExtBlockMapHdr->mState = SMT_EBMS_FREE;

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_TABLESPACE,
                           SMR_LOG_CREATE_EXTENT_BLOCK_MAP,
                           SMR_REDO_TARGET_PAGE,
                           NULL,
                           0,
                           aTbsId,
                           aExtBlockMapPid,
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
    
}

stlStatus smtFindExtMap( smxmTrans     * aMiniTrans,
                         smlTbsId        aTbsId,
                         smlDatafileId   aDatafileId,
                         smlPid        * aExtMapPid,
                         smlEnv        * aEnv )
{
    smlPid              sExtBlockMapPid;
    stlUInt16           i;
    stlUInt16           j;
    smpHandle           sExtBlockMapHandle;
    smlPid              sFreeExtMap[SMT_MAX_EXT_MAP_HINT];
    stlInt64          * sHintSequence;
    smtExtBlockMapHdr * sExtBlockMapHdr;
    smtExtBlockMap    * sExtBlockMap;
    smpHandle         * sCachedPageHandle;
    stlInt32            sState = 0;

    *aExtMapPid = SMP_NULL_PID;
    sExtBlockMapPid = SMP_MAKE_PID( aDatafileId, SMT_EXT_BLOCK_MAP_PAGE_SEQ );

    sCachedPageHandle = smxmGetPageHandle( aMiniTrans,
                                           aTbsId,
                                           sExtBlockMapPid,
                                           KNL_LATCH_MODE_SHARED );
    
    if( sCachedPageHandle == NULL )
    {
        STL_TRY( smpAcquire( NULL,
                             aTbsId,
                             sExtBlockMapPid,
                             KNL_LATCH_MODE_SHARED,
                             &sExtBlockMapHandle,
                             aEnv ) == STL_SUCCESS );
        sState = 1;
    }
    else
    {
        sExtBlockMapHandle = *sCachedPageHandle;
    }

    sHintSequence = (stlInt64*)smpGetVolatileArea( &sExtBlockMapHandle );
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( &sExtBlockMapHandle ) );

    /**
     * Dropping Datafile은 Skip 한다.
     */
    STL_TRY_THROW( (sExtBlockMapHdr->mState & SMT_EBMS_MASK) != SMT_EBMS_DROP,
                   RAMP_RETURN );
    
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));

    for( i = 0, j = 0; (i < sExtBlockMapHdr->mExtBlockDescCount) &&
                       (j < SMT_MAX_EXT_MAP_HINT); i++ )
    {
        if( (sExtBlockMap->mExtBlockDesc[i].mState & SMT_EBS_PREPARED) ==
            SMT_EBS_PREPARED )
        {
            continue;
        }
        
        if( (sExtBlockMap->mExtBlockDesc[i].mState & SMT_EBS_FREE) ==
            SMT_EBS_FREE )
        {
            sFreeExtMap[j++] = sExtBlockMap->mExtBlockDesc[i].mExtBlock;
        }
    }

    if( j > 0 )
    {
        *aExtMapPid = sFreeExtMap[(*sHintSequence) % j];
        *sHintSequence = *sHintSequence + 1;
    }

    STL_RAMP( RAMP_RETURN );

    if( sState == 1 )
    {
        STL_TRY( smpRelease( &sExtBlockMapHandle, aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smpRelease( &sExtBlockMapHandle, aEnv );
    }
    
    return STL_FAILURE;
}

stlStatus smtFindExtBlockMap( smxmTrans * aMiniTrans,
                              smlTbsId    aTbsId,
                              smlPid    * aExtMapPid,
                              smlEnv    * aEnv )
{
    smlDatafileId sDatafileId;
    stlBool       sExist;
    smxmSavepoint sSavepoint;
    
    *aExtMapPid = SMP_NULL_PID;
    
    STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                &sSavepoint,
                                aEnv ) == STL_SUCCESS );
    
    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sExist,
                               aEnv ) == STL_SUCCESS );
    
    while( sExist == STL_TRUE )
    {
        if( (smfGetDatafileState( aTbsId, sDatafileId ) == SMF_DATAFILE_STATE_DROPPING) ||
            (smfGetDatafileState( aTbsId, sDatafileId ) == SMF_DATAFILE_STATE_CREATING) ||
            (smfGetDatafileState( aTbsId, sDatafileId ) == SMF_DATAFILE_STATE_AGING) )
        {
            STL_TRY( smfNextDatafile( aTbsId,
                                      &sDatafileId,
                                      &sExist,
                                      aEnv ) == STL_SUCCESS );
            continue;
        }
        
        STL_TRY( smtFindExtMap( aMiniTrans,
                                aTbsId,
                                sDatafileId,
                                aExtMapPid,
                                aEnv ) == STL_SUCCESS );

        if( *aExtMapPid != SMP_NULL_PID )
        {
            break;
        }
                                
        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          &sSavepoint,
                                          aEnv ) == STL_SUCCESS );
        
        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sExist,
                                  aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                      &sSavepoint,
                                      aEnv ) == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

smtHint * smtFindTbsHint( smlTbsId   aTbsId,
                          smlEnv   * aEnv )
{
    stlInt32       i;
    smtHintPool  * sTbsHintPool;
    smtHint      * sTbsHint = NULL;

    sTbsHintPool = (smtHintPool*)SML_SESS_ENV(aEnv)->mTbsHintPool;
    
    for( i = 0; i < SMS_MAX_TBS_HINT; i++ )
    {
        if( aTbsId == sTbsHintPool->mTbsHint[i].mTbsId )
        {
            sTbsHint = (smtHint*)&(sTbsHintPool->mTbsHint[i]);
            break;
        }
    }

    return sTbsHint;
}

void smtAddNewTbsHint( smlPid     aHintPid,
                       smlTbsId   aTbsId,
                       smlEnv   * aEnv )
{
    smtHintPool  * sTbsHintPool;
    smtHint      * sTbsHint = NULL;

    sTbsHintPool = (smtHintPool*)SML_SESS_ENV(aEnv)->mTbsHintPool;

    sTbsHint = (smtHint*)&(sTbsHintPool->mTbsHint[sTbsHintPool->mSequence % SMS_MAX_TBS_HINT]);
    sTbsHint->mTbsId = aTbsId;
    sTbsHint->mHintPid = aHintPid;
    
    sTbsHintPool->mSequence += 1;
}

stlStatus smtValidateExtentMap( smpHandle * aPageHandle,
                                smlEnv    * aEnv )
{
#ifdef STL_VALIDATE_DATABASE
    smtExtMapHdr * sExtMapHdr;
    stlChar      * sExtBitArray;
    stlInt32       i;
    stlInt32       sFreeExtCount = 0;
    
    sExtMapHdr = (smtExtMapHdr*)smpGetBody( SMP_FRAME( aPageHandle ) );
    sExtBitArray = SMT_EXT_BIT_ARRAY( sExtMapHdr );

    for( i = 0; i < sExtMapHdr->mExtCount; i++ )
    {
        if( smtIsFreeExt( sExtBitArray, i ) == STL_TRUE )
        {
            sFreeExtCount++;
        }
    }
    
    KNL_ASSERT( sFreeExtCount == sExtMapHdr->mOffExtCount,
                KNL_ENV(aEnv),
                ("TOTAL_EXT_COUNT(%d), ACTUAL_OFF_EXT_COUNT(%d), "
                 "ON_EXT_COUNT(%d), OFF_EXT_COUNT(%d)\n PAGE BODY\n%s",
                 sExtMapHdr->mExtCount,
                 sFreeExtCount,
                 sExtMapHdr->mOnExtCount,
                 sExtMapHdr->mOffExtCount,
                 knlDumpBinaryForAssert( SMP_FRAME(aPageHandle),
                                         SMP_PAGE_SIZE,
                                         &aEnv->mLogHeapMem,
                                         KNL_ENV(aEnv))) );

    KNL_ASSERT( (sExtMapHdr->mExtCount - sFreeExtCount) == sExtMapHdr->mOnExtCount,
                KNL_ENV(aEnv),
                ("TOTAL_EXT_COUNT(%d), ACTUAL_OFF_EXT_COUNT(%d), "
                 "ON_EXT_COUNT(%d), OFF_EXT_COUNT(%d)\n PAGE BODY\n%s",
                 sExtMapHdr->mExtCount,
                 sFreeExtCount,
                 sExtMapHdr->mOnExtCount,
                 sExtMapHdr->mOffExtCount,
                 knlDumpBinaryForAssert( SMP_FRAME(aPageHandle),
                                         SMP_PAGE_SIZE,
                                         &aEnv->mLogHeapMem,
                                         KNL_ENV(aEnv))) );

    if( sFreeExtCount == 0 )
    {
        KNL_ASSERT( smpGetFreeness(aPageHandle) == SMP_FREENESS_FULL,
                    KNL_ENV(aEnv),
                    ("ACTUAL_FREE_EXT_COUNT(%d), FREENESS(%d)\n PAGE BODY\n%s",
                     sFreeExtCount,
                     smpGetFreeness(aPageHandle),
                     knlDumpBinaryForAssert( SMP_FRAME(aPageHandle),
                                             SMP_PAGE_SIZE,
                                             &aEnv->mLogHeapMem,
                                             KNL_ENV(aEnv))) );
    }
    
    return STL_SUCCESS;
#else
    return STL_SUCCESS;
#endif    
}

/** @} */
