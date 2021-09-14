/*******************************************************************************
 * smtTbs.c
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
#include <smg.h>
#include <smp.h>
#include <sma.h>
#include <smt.h>
#include <smr.h>
#include <smxm.h>
#include <smxl.h>
#include <smtDef.h>
#include <smtTbs.h>
#include <smtExtMgr.h>
#include <smlGeneral.h>

/**
 * @file smtTbs.c
 * @brief Storage Manager Layer Tablespace Routines
 */

/**
 * @addtogroup smt
 * @{
 */

/**
 * @brief Tablespace를 생성한다.
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsAttr 생성할 Tablespace 속성 구조체
 * @param[in] aReservedPageCount 테이블스페이스 사용자를 위한 예약된(Customized) 페이지 개수
 * @param[in] aUndoLogging Undo Logging 여부 ( 데이터베이스 생성시에는 Undo Logging을 할수 없음 )
 * @param[out] aTbsId 생성한 Tablespace의 Identifier
 * @param[in,out] aEnv Environment 정보
 * @note aReservedPageCount는 테이블스페이스를 사용하는 모든 개발자들에게 특별한 페이지를
 * <BR> 확보해주기 위한 방법이며, 해당 인터페이스가 만들어진 계기는 Segment Map Header이다.
 * <BR> Segment Map의 Header는 Segment Map안의 페이지들을 사용할수 없기 때문에 Tablespace내에
 * <BR> 특별한 페이지 위치를 예약하고, 이를 Header로 사용한다.
 */
stlStatus smtCreateTbs( smlStatement * aStatement,
                        smlTbsAttr   * aTbsAttr,
                        stlInt32       aReservedPageCount,
                        stlBool        aUndoLogging,
                        smlTbsId     * aTbsId,
                        smlEnv       * aEnv )
{
    smlTbsId       sTbsId;
    smlDatafileId  sDatafileId;
    stlBool        sExist = STL_FALSE;
    
    STL_TRY( smfCreateTbs( aStatement,
                           aTbsAttr,
                           aUndoLogging,
                           &sTbsId,
                           aEnv ) == STL_SUCCESS );

    STL_TRY( smfFirstDatafile( sTbsId,
                               &sDatafileId,
                               &sExist,
                               aEnv ) == STL_SUCCESS );

    while( sExist == STL_TRUE )
    {
        STL_TRY( smpInit( sTbsId,
                          sDatafileId,
                          aEnv ) == STL_SUCCESS );
        
        STL_TRY( smpExtend( aStatement,
                            sTbsId,
                            sDatafileId,
                            STL_TRUE,
                            aEnv ) == STL_SUCCESS );
        
        STL_TRY( smtFormatDatafile( aStatement,
                                    sTbsId,
                                    sDatafileId,
                                    aReservedPageCount,
                                    aEnv ) == STL_SUCCESS );

        /*
         * 0번 Datafile에만 Reserve시킨다
         */
        aReservedPageCount = 0;

        STL_TRY( smfNextDatafile( sTbsId,
                                  &sDatafileId,
                                  &sExist,
                                  aEnv ) == STL_SUCCESS );
    }
    
    *aTbsId = sTbsId;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace를 삭제한다.
 * @param[in]     aStatement    Statement 포인터
 * @param[in]     aTbsId        삭제할 Tablespace의 Identifier
 * @param[in]     aAndDatafiles 데이터파일의 삭제 여부
 * @param[in,out] aEnv          Environment 정보
 */
stlStatus smtDropTbs( smlStatement * aStatement,
                      smlTbsId       aTbsId,
                      stlBool        aAndDatafiles,
                      smlEnv       * aEnv )
{
    smlDatafileId  sDatafileId;
    stlBool        sExist = STL_FALSE;

    STL_TRY_THROW( smfGetTbsState( aTbsId ) != SMF_TBS_STATE_DROPPING, RAMP_SUCCESS );
    STL_TRY_THROW( smfGetTbsState( aTbsId ) != SMF_TBS_STATE_DROPPED, RAMP_SUCCESS );
    
    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sExist,
                               aEnv ) == STL_SUCCESS );

    while( sExist == STL_TRUE )
    {
        STL_TRY( smpShrink( aStatement,
                            aTbsId,
                            sDatafileId,
                            SMP_PCHARRAY_STATE_DROPPED, /* aPchArrayState */
                            aEnv ) == STL_SUCCESS );
        
        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sExist,
                                  aEnv ) == STL_SUCCESS );
    }
    
    STL_TRY( smfDropTbs( aStatement,
                         aTbsId,
                         aAndDatafiles,
                         aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace에 Datafile을 추가한다.
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsId 대상 Tablespace Identifier
 * @param[in] aDatafileAttr 추가할 Datafile 속성 구조체
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtAddDatafile( smlStatement    * aStatement,
                          smlTbsId          aTbsId,
                          smlDatafileAttr * aDatafileAttr,
                          smlEnv          * aEnv )
{
    smlDatafileId sDatafileId;
    
    STL_TRY( smfAddDatafile( aStatement,
                             aTbsId,
                             aDatafileAttr,
                             &sDatafileId,
                             aEnv ) == STL_SUCCESS );

    STL_TRY( smpInit( aTbsId,
                      sDatafileId,
                      aEnv ) == STL_SUCCESS );
    
    STL_TRY( smpExtend( aStatement,
                        aTbsId,
                        sDatafileId,
                        STL_TRUE,
                        aEnv ) == STL_SUCCESS );

    STL_TRY( smtFormatDatafile( aStatement,
                                aTbsId,
                                sDatafileId,
                                0,
                                aEnv ) == STL_SUCCESS );

    aDatafileAttr->mDatafileId = sDatafileId;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace에서 하나의 Datafile을 삭제한다.
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsId 대상 Tablespace Identifier
 * @param[in] aDatafileName 삭제할 Datafile 이름
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtDropDatafile( smlStatement * aStatement,
                           smlTbsId       aTbsId,
                           stlChar      * aDatafileName,
                           smlEnv       * aEnv )
{
    smlDatafileId       sDatafileId;
    stlBool             sFound = STL_FALSE;
    smxmTrans           sMiniTrans;
    smlRid              sUndoRid;
    smpHandle           sPageHandle;
    smtExtBlockMapHdr * sExtBlockMapHdr;
    stlInt32            sState = 0;
    smlPid              sExtBlockMapPid;

    STL_TRY_THROW( smfGetTbsState( aTbsId )
                   != SMF_TBS_STATE_DROPPING, RAMP_SUCCESS );
    STL_TRY_THROW( smfGetTbsState( aTbsId )
                   != SMF_TBS_STATE_DROPPED, RAMP_SUCCESS );
    
    STL_TRY( smfFindDatafileId( aTbsId,
                                aDatafileName,
                                &sDatafileId,
                                &sFound,
                                aEnv ) == STL_SUCCESS );

    STL_DASSERT( sFound == STL_TRUE );
    STL_DASSERT( sDatafileId != 0 );
    
    STL_TRY_THROW( smfGetDatafileState( aTbsId, sDatafileId )
                   != SMF_DATAFILE_STATE_DROPPING, RAMP_SUCCESS );
    STL_TRY_THROW( smfGetDatafileState( aTbsId, sDatafileId )
                   != SMF_DATAFILE_STATE_AGING, RAMP_SUCCESS );
    STL_TRY_THROW( smfGetDatafileState( aTbsId, sDatafileId )
                   != SMF_DATAFILE_STATE_DROPPED, RAMP_SUCCESS );

    STL_TRY( smxmBegin( &sMiniTrans,
                        SMA_GET_TRANS_ID(aStatement),
                        SML_INVALID_RID,
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 0;

    sExtBlockMapPid = SMP_MAKE_PID( sDatafileId, SMT_EXT_BLOCK_MAP_PAGE_SEQ );
    
    STL_TRY( smpAcquire( &sMiniTrans,
                         aTbsId,
                         sExtBlockMapPid,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
    sState = 1;

    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME(&sPageHandle) );

    STL_TRY_THROW( (sExtBlockMapHdr->mState & SMT_EBMS_MASK) == SMT_EBMS_FREE,
                   RAMP_ERR_NON_EMPTY_DATAFILE );
    
    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_TABLESPACE,
                                   SMT_UNDO_LOG_MEMORY_UPDATE_EXT_BLOCK_MAP_HDR,
                                   (void*)&(sExtBlockMapHdr->mState),
                                   STL_SIZEOF(stlInt32),
                                   (smlRid){ aTbsId, 0, sExtBlockMapPid },
                                   &sUndoRid,
                                   aEnv )
             == STL_SUCCESS );

    sExtBlockMapHdr->mState = SMT_EBMS_DROP;

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_TABLESPACE,
                           SMR_LOG_UPDATE_EXT_BLOCK_MAP_HDR_STATE,
                           SMR_REDO_TARGET_PAGE,
                           (void*)&(sExtBlockMapHdr->mState),
                           STL_SIZEOF(stlInt32),
                           aTbsId,
                           sExtBlockMapPid,
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    STL_TRY( smpShrink( aStatement,
                        aTbsId,
                        sDatafileId,
                        SMP_PCHARRAY_STATE_DROPPED, /* aPchArrayState */
                        aEnv ) == STL_SUCCESS );
    
    STL_TRY( smfDropDatafile( aStatement,
                              aTbsId,
                              sDatafileId,
                              aEnv ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NON_EMPTY_DATAFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NON_EMPTY_DATAFILE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void) smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

inline stlBool smtIsLogging( smlTbsId  aTbsId )
{
    return SMF_IS_LOGGING_TBS( aTbsId );
}

/** @} */

/**
 * @addtogroup smtTbs
 * @{
 */

/**
 * @brief Tablespace에서 하나의 Datafile을 포맷한다.
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsId 대상 Tablespace Identifier
 * @param[in] aDatafileId 삭제할 Datafile Identifier
 * @param[in] aReservedPageCount 예약되어 있는 페이지 개수
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtFormatDatafile( smlStatement  * aStatement,
                             smlTbsId        aTbsId,
                             smlDatafileId   aDatafileId,
                             stlInt32        aReservedPageCount,
                             smlEnv        * aEnv )
{
    smlPid      sExtBlockPid;
    stlInt32    sExtCount;
    stlInt32    sMaxExtCount;
    stlUInt32   sPageSeqId = 0;
    stlInt32    sRemainExtCount;
    stlInt32    sHdrPageCount;
    stlUInt16   sPageCountInExt;
    smxlTransId sTransId;

    if( aStatement == NULL )
    {
        sTransId = SML_INVALID_TRANSID;
    }
    else
    {
        sTransId = SMA_GET_TRANS_ID(aStatement);
    }

    sPageCountInExt = smfGetPageCountInExt( aTbsId );
    sRemainExtCount = smfGetMaxPageCount( aTbsId, aDatafileId )
                      / sPageCountInExt;

    STL_TRY( smtCreateExtBlockMap( sTransId,
                                   aTbsId,
                                   SMP_MAKE_PID( aDatafileId, sPageSeqId ),
                                   aEnv )
             == STL_SUCCESS );

    sPageSeqId = 1;
    sHdrPageCount = 1;
    
    /*
     * Skip Reserved Space
     */
    sPageSeqId += aReservedPageCount;
    sHdrPageCount += aReservedPageCount;
    
    sExtBlockPid = SMP_MAKE_PID( aDatafileId, sPageSeqId );
    sMaxExtCount = SMT_MAX_EXT_BIT_COUNT( sPageCountInExt );
    
    while( 1 )
    {
        sPageSeqId++;
        sHdrPageCount++;
        
        if( sRemainExtCount > sMaxExtCount )
        {
            sExtCount = sMaxExtCount;
        }
        else
        {
            sExtCount = sRemainExtCount - (sHdrPageCount / sPageCountInExt);

            if( (sHdrPageCount % sPageCountInExt) > 0 )
            {
                sExtCount--;
            }
        }
        
        STL_TRY( smtCreateExtBlock( sTransId,
                                    aTbsId,
                                    sExtBlockPid,
                                    sExtCount,
                                    sExtCount,
                                    aEnv ) == STL_SUCCESS );

        STL_TRY( smtAddExtBlock( aStatement,
                                 aTbsId,
                                 sExtBlockPid,
                                 aEnv ) == STL_SUCCESS );

        sPageSeqId += sExtCount * sPageCountInExt;
        sExtBlockPid = SMP_MAKE_PID( aDatafileId, sPageSeqId );

        STL_ASSERT( sPageSeqId < smfGetMaxPageCount( aTbsId, aDatafileId ) );

        if( sExtCount != sMaxExtCount )
        {
            break;
        }
        
        sRemainExtCount -= sExtCount;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief MOUNT phase에서 ADD DATAFILE을 수행한다.
 * @param[in] aData Datafile 정보
 * @param[in] aDataSize Datafile 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtAddDatafileEventHandler( void      * aData,
                                      stlUInt32   aDataSize,
                                      stlBool   * aDone,
                                      void      * aEnv )
{
    smlStatement    * sStatement;
    smlDatafileAttr * sDatafileAttr;
    smlDatafileAttr * sCurDatafileAttr;
    smlTbsId          sTbsId;
    stlInt32          sFileCount;
    stlInt32          i;
    stlInt32          sState = 0;

    *aDone = STL_FALSE;

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    /* MOUNT phase에서 불완전 복구가 수행중이지 않을 때만 수행가능하다. */
    STL_TRY_THROW( smrProceedIncompleteMediaRecovery(aEnv) != STL_TRUE,
                   RAMP_ERR_RECOVERY_IN_PROGRESS );

    STL_DASSERT( smlEnterStorageManager( SML_ENV( aEnv ) ) == STL_SUCCESS );

    sDatafileAttr = (smlDatafileAttr *)aData;
    sTbsId = sDatafileAttr->mTbsId;
    sFileCount = aDataSize / STL_SIZEOF(smlDatafileAttr);

    STL_TRY( smaAllocStatement( SML_INVALID_TRANSID,
                                SML_INVALID_TRANSID,
                                NULL,
                                ( SML_STMT_ATTR_UPDATABLE |
                                  SML_STMT_ATTR_LOCKABLE ),
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE,
                                & sStatement,
                                SML_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    for( i = 0; i < sFileCount; i++ )
    {
        sCurDatafileAttr = (smlDatafileAttr *)(sDatafileAttr + i);

        STL_TRY( smtAddDatafile( sStatement,
                                 sTbsId,
                                 sCurDatafileAttr,
                                 SML_ENV( aEnv ) ) == STL_SUCCESS );

        STL_TRY( knlCheckEnvEvent( KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }
    
    STL_TRY( smgExecutePendOp( SML_PEND_ACTION_COMMIT,
                               0, /* aStatementTcn */
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
        
    for( i = 0; i < sFileCount; i++ )
    {
        sCurDatafileAttr = (smlDatafileAttr *)(sDatafileAttr + i);

        STL_TRY( smpFlushPagesForTablespace( sStatement,
                                             sTbsId,
                                             SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL,
                                             sCurDatafileAttr->mDatafileId,
                                             SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( smaFreeStatement( sStatement, SML_ENV( aEnv ) ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ONLY_MOUNT_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_APPROPRIATE_PHASE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      gPhaseString[KNL_STARTUP_PHASE_MOUNT] );
    }

    STL_CATCH( RAMP_ERR_RECOVERY_IN_PROGRESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_RECOVERY_IN_PROGRESS,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sState > 0 )
    {
        (void)smaFreeStatement( sStatement, SML_ENV( aEnv ) );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_FAILURE;
}

/**
 * @brief MOUNT phase에서 OFFLINE TABLESPACE를 수행한다.
 * @param[in] aData Tablespace 정보
 * @param[in] aDataSize Tablespace 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtOfflineTablespaceEventHandler( void      * aData,
                                            stlUInt32   aDataSize,
                                            stlBool   * aDone,
                                            void      * aEnv )
{
    smlStatement       * sStatement;
    smlOfflineTbsArg     sOfflineArg;
    stlInt32             sState = 0;
        
    *aDone = STL_FALSE;

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    /* MOUNT phase에서 불완전 복구가 수행중이지 않을 때만 수행가능하다. */
    STL_TRY_THROW( smrProceedIncompleteMediaRecovery(aEnv) != STL_TRUE,
                   RAMP_ERR_RECOVERY_IN_PROGRESS );

    stlMemcpy( &sOfflineArg, aData, aDataSize );

    /* Offline Tablespace를 수행할 수 있는지 체크한다. */
    STL_TRY( smfValidateOfflineTablespace( sOfflineArg.mOfflineBehavior,
                                           SML_ENV(aEnv) )
             == STL_SUCCESS );

    STL_DASSERT( smlEnterStorageManager( SML_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( smaAllocStatement( SML_INVALID_TRANSID,
                                SML_INVALID_TRANSID,
                                NULL,
                                ( SML_STMT_ATTR_UPDATABLE |
                                  SML_STMT_ATTR_LOCKABLE ),
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE,
                                & sStatement,
                                SML_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * setting offline
     */

    STL_TRY( smfOfflineTbs( sStatement,
                            sOfflineArg.mTbsId,
                            sOfflineArg.mOfflineBehavior,
                            SML_ENV( aEnv) )
             == STL_SUCCESS );

    STL_TRY( smgExecutePendOp( SML_PEND_ACTION_COMMIT,
                               0, /* aStatementTcn */
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( smaFreeStatement( sStatement, SML_ENV( aEnv ) ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ONLY_MOUNT_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_APPROPRIATE_PHASE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      gPhaseString[KNL_STARTUP_PHASE_MOUNT] );
    }

    STL_CATCH( RAMP_ERR_RECOVERY_IN_PROGRESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_RECOVERY_IN_PROGRESS,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sState > 0 )
    {
        (void)smaFreeStatement( sStatement, SML_ENV( aEnv ) );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_FAILURE;
}

/**
 * @brief MOUNT phase에서 RENAME DATAFILE을 수행한다.
 * @param[in] aData Datafile 정보
 * @param[in] aDataSize Datafile 정보 크기
 * @param[out] aDone 성공여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtRenameDatafileEventHandler( void      * aData,
                                         stlUInt32   aDataSize,
                                         stlBool   * aDone,
                                         void      * aEnv )
{
    smlStatement       * sStatement;
    smlTbsId             sTbsId;
    smlDatafileAttr    * sOrgDatafileAttr;
    smlDatafileAttr    * sNewDatafileAttr;
    stlInt32             sFileCount;
    stlInt32             i;
    stlInt32             sState = 0;
        
    *aDone = STL_FALSE;

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    /* MOUNT phase에서 불완전 복구가 수행중이지 않을 때만 수행가능하다. */
    STL_TRY_THROW( smrProceedIncompleteMediaRecovery(aEnv) != STL_TRUE,
                   RAMP_ERR_RECOVERY_IN_PROGRESS );

    STL_DASSERT( smlEnterStorageManager( SML_ENV( aEnv ) ) == STL_SUCCESS );

    sFileCount = aDataSize / (STL_SIZEOF(smlDatafileAttr) * 2);
    sOrgDatafileAttr = (smlDatafileAttr *)aData;
    sNewDatafileAttr = (smlDatafileAttr *)(sOrgDatafileAttr + sFileCount);

    sTbsId = sOrgDatafileAttr->mTbsId;

    for( i = 0; i < sFileCount - 1; i++ )
    {
        sOrgDatafileAttr[i].mNext = &sOrgDatafileAttr[i + 1];
        sNewDatafileAttr[i].mNext = &sNewDatafileAttr[i + 1];
    }

    STL_TRY( smaAllocStatement( SML_INVALID_TRANSID,
                                SML_INVALID_TRANSID,
                                NULL,
                                ( SML_STMT_ATTR_UPDATABLE |
                                  SML_STMT_ATTR_LOCKABLE ),
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE,
                                & sStatement,
                                SML_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 0;

    STL_TRY( smfRenameDatafiles( sStatement,
                                 sTbsId,
                                 sOrgDatafileAttr,
                                 sNewDatafileAttr,
                                 aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( smaFreeStatement( sStatement, SML_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( smgExecutePendOp( SML_PEND_ACTION_COMMIT,
                               0, /* aStatementTcn */
                               aEnv )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ONLY_MOUNT_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_APPROPRIATE_PHASE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      gPhaseString[KNL_STARTUP_PHASE_MOUNT] );
    }

    STL_CATCH( RAMP_ERR_RECOVERY_IN_PROGRESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_RECOVERY_IN_PROGRESS,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sState > 0 )
    {
        (void)smaFreeStatement( sStatement, SML_ENV( aEnv ) );
    }

    (void)smgExecutePendOp( SML_PEND_ACTION_ROLLBACK,
                            0, /* aStatementTcn */
                            aEnv );

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_FAILURE;
}

/**
 * @brief Open Database Resetlogs 이후 Dictionary Tbs에 등록되지 않은 Tbs 삭제.
 * @param[in] aTbsId  tablespace id
 * @param[in,out] aEnv environment
 */ 
stlStatus smtDropUnregisteredTbs( smlTbsId   aTbsId,
                                  smlEnv   * aEnv )
{
    smlDatafileId  sDatafileId;
    stlBool        sExist = STL_FALSE;

    STL_TRY( smfFirstDatafile( aTbsId,
                               &sDatafileId,
                               &sExist,
                               aEnv ) == STL_SUCCESS );

    while( sExist == STL_TRUE )
    {
        STL_TRY( smpShrink( NULL, /* aStatement */
                            aTbsId,
                            sDatafileId,
                            SMP_PCHARRAY_STATE_DROPPED, /* aPchArrayState */
                            aEnv ) == STL_SUCCESS );
        
        STL_TRY( smfNextDatafile( aTbsId,
                                  &sDatafileId,
                                  &sExist,
                                  aEnv ) == STL_SUCCESS );
    }
    
    STL_TRY( smfDropUnregisteredTbs( aTbsId,
                                     aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    /**
     * 이 함수는 실패하면 안되고, 필요하다면 추후 Pedning Op 추가해야 한다.
     */ 
    STL_DASSERT( STL_FALSE );

    return STL_FAILURE;
}

/** @} */
