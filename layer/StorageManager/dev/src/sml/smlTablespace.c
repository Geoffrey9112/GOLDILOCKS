/*******************************************************************************
 * smlTablespace.c
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

#include <dtl.h>
#include <sml.h>
#include <smDef.h>
#include <smt.h>
#include <sma.h>
#include <smg.h>
#include <smp.h>
#include <smn.h>
#include <smf.h>
#include <smxl.h>
#include <smd.h>
#include <smr.h>
#include <smdDef.h>

/**
 * @file smlTablespace.c
 * @brief Storage Manager Layer Tablespace Routines
 */

extern smfInitialTbsData gSmfNoMountTbsInfoArray[];

/**
 * @addtogroup smlTablespace
 * @{
 */

/**
 * @brief Tablespace를 생성한다.
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsAttr 생성할 Tablespace 속성 구조체
 * @param[in] aReservedPageCount 테이블스페이스 사용자를 위한 예약된(Customized) 페이지 개수
 * @param[out] aTbsId 생성한 Tablespace의 Identifier
 * @param[in,out] aEnv Environment 정보
 * @note aReservedPageCount는 테이블스페이스를 사용하는 모든 개발자들에게 특별한 페이지를
 * <BR> 확보해주기 위한 방법이며, 해당 인터페이스가 만들어진 계기는 Segment Map Header이다.
 * <BR> Segment Map의 Header는 Segment Map안의 페이지들을 사용할수 없기 때문에 Tablespace내에
 * <BR> 특별한 페이지 위치를 예약하고, 이를 Header로 사용한다.
 */
stlStatus smlCreateTablespace( smlStatement * aStatement,
                               smlTbsAttr   * aTbsAttr,
                               stlInt32       aReservedPageCount,
                               smlTbsId     * aTbsId,
                               smlEnv       * aEnv )
{
    stlUInt32  sAttr;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( smtCreateTbs( aStatement,
                           aTbsAttr,
                           aReservedPageCount,
                           STL_TRUE,     /* aUndoLogging */
                           aTbsId,
                           aEnv ) == STL_SUCCESS );

    if( (aTbsAttr->mValidFlags & SML_TBS_ATTR_MASK) == SML_TBS_ATTR_NO )
    {
        sAttr = SML_TBS_DEFAULT_ATTR;
    }
    else
    {
        sAttr = aTbsAttr->mAttr;
    }

    /**
     * Data Tablepace만 Offline을 지원한다.
     */
    
    if( (sAttr & SML_TBS_USAGE_MASK) == SML_TBS_USAGE_DATA )
    {
        STL_TRY( smdCreatePendingRelation( aStatement,
                                           *aTbsId,
                                           aEnv )
                 == STL_SUCCESS );
    }
    
    if( smlGetDataStoreMode() < SML_DATA_STORE_MODE_TDS )
    {
        STL_TRY( smgExecutePendOp( SML_PEND_ACTION_COMMIT,
                                   0, /* aStatementTcn */
                                   aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smpFlushPagesForTablespace( aStatement,
                                             *aTbsId,
                                             SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL,
                                             SML_INVALID_DATAFILE_ID,
                                             aEnv )
                 == STL_SUCCESS );
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void)smgExecutePendOp( SML_PEND_ACTION_ROLLBACK,
                            0, /* aStatementTcn */
                            aEnv );

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Tablespace를 삭제한다.
 * @param[in]     aStatement    Statement 포인터
 * @param[in]     aTbsId        삭제할 Tablespace의 Identifier
 * @param[in]     aAndDatafiles 데이터파일의 삭제 여부
 * @param[in,out] aEnv          Environment 정보
 */
stlStatus smlDropTablespace( smlStatement * aStatement,
                             smlTbsId       aTbsId,
                             stlBool        aAndDatafiles,
                             smlEnv       * aEnv )
{
    stlInt32   sState = 0;
    stlBool    sIsSuccess;
    stlChar    sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    /**
     * BACKUP이 진행 중일 경우 DROP TABLESPACE이 수행될 수 없다.
     */
    STL_TRY( smfSetBackupWaitingState( aTbsId, &sIsSuccess, aEnv ) == STL_SUCCESS );

    /**
     * backup이 진행중이면 exception
     */
    STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_ERR_BACKUP_IN_PROGRESS );
    sState = 1;

    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( smdDropPendingRelation( aStatement,
                                     aTbsId,
                                     aEnv )
             == STL_SUCCESS);

    STL_TRY( smtDropTbs( aStatement,
                         aTbsId,
                         aAndDatafiles,
                         aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smfResetBackupWaitingState( aTbsId, aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BACKUP_IN_PROGRESS )
    {
        smfGetTbsName( aTbsId, sTbsName, aEnv );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_BACKUP_IN_PROGRESS,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsName );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfResetBackupWaitingState( aTbsId, aEnv );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Tablespace를 Offline 시킨다.
 * @param[in]     aStatement       Statement 포인터
 * @param[in]     aTbsId           Tablespace Identifier
 * @param[in]     aOfflineBehavior Offline 동작
 * @param[in,out] aEnv             Environment 정보
 */
stlStatus smlOfflineTablespace( smlStatement       * aStatement,
                                smlTbsId             aTbsId,
                                smlOfflineBehavior   aOfflineBehavior,
                                smlEnv             * aEnv )
{
    smpFlushTbsArg    sArg;
    stlInt32          sState = 0;
    stlBool           sIsSuccess;
    stlChar           sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];
        
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    /* Offline Tablespace를 수행할 수 있는지 체크한다. */
    STL_TRY( smfValidateOfflineTablespace( aOfflineBehavior,
                                           aEnv )
             == STL_SUCCESS );

    /**
     * Tablespace의 Backup과 offline을 동시에 수행할 수 없다.
     */
    STL_TRY( smfSetBackupWaitingState( aTbsId, &sIsSuccess, aEnv ) == STL_SUCCESS );

    /**
     * backup이 진행중이면 exception
     */
    STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_ERR_BACKUP_IN_PROGRESS );
    sState = 1;
    
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * OPEN phase에서만 Interface 직접 호출로 수행되며
     * MOUNT phase에서는 Event Handler로 수행된다.
     */
    STL_DASSERT( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN );

    /**
     * waiting ager
     */
    STL_TRY( smlLoopbackAger( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    /**
     * flush pages related to a tablespace
     */
    
    if( aOfflineBehavior == SML_OFFLINE_BEHAVIOR_NORMAL )
    {
        sArg.mTbsId = aTbsId;
        sArg.mDatafileId = SML_INVALID_DATAFILE_ID;
        sArg.mReason = SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL;

        STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH_TBS,
                                 NULL,                         /* aEventMem */
                                 (void*)&sArg,                 /* aData */
                                 STL_SIZEOF(smpFlushTbsArg),   /* aDataSize */
                                 SML_PAGE_FLUSHER_ENV_ID,
                                 KNL_EVENT_WAIT_POLLING,
                                 STL_FALSE, /* aForceSuccess */
                                 KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }
    
    /**
     * setting offline
     */

    STL_TRY( smfOfflineTbs( aStatement,
                            aTbsId,
                            aOfflineBehavior,
                            aEnv )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BACKUP_IN_PROGRESS )
    {
        smfGetTbsName( aTbsId, sTbsName, aEnv );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_BACKUP_IN_PROGRESS,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsName );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfResetBackupWaitingState( aTbsId, aEnv );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Tablespace를 Online 시킨다.
 * @param[in]     aStatement         Statement 포인터
 * @param[in]     aTbsId             Tablespace Identifier
 * @param[in]     aPendingRelationId Pending Relation Identifier
 * @param[in,out] aEnv               Environment 정보
 */
stlStatus smlOnlineTablespace( smlStatement       * aStatement,
                               smlTbsId             aTbsId,
                               stlInt64           * aPendingRelationId,
                               smlEnv             * aEnv )
{
    stlInt64     sRelationId;
    stlInt32     sState = 0;
    smfTbsInfo * sTbsInfo;
    stlInt8      sOfflineState;
    smrLsn       sOfflineLsn;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    sTbsInfo = (smfTbsInfo *) smfGetTbsHandle( aTbsId );
    sOfflineState = sTbsInfo->mTbsPersData.mOfflineState;
    sOfflineLsn = sTbsInfo->mTbsPersData.mOfflineLsn;
    
    /**
     * Consistent Offline 검사
     */

    STL_TRY( smfValidateOnlineTbs( aTbsId, aEnv ) == STL_SUCCESS );
    
    /**
     * waiting ager
     */
    STL_TRY( smlLoopbackAger( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );

    /**
     * setting online
     */

    sRelationId = smfGetPendingRelationId( aTbsId );

    STL_TRY( smfOnlineTbs( aStatement,
                           aTbsId,
                           aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smpExtendTablespace( aStatement,
                                  aTbsId,
                                  STL_FALSE, /* aNeedPending */
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * smpExtendTablespace에서 Page Validation 이후
     * corruption이 존재하는지 확인한다. 
     */

    STL_TRY( smfVerifyTbsCorruption( aTbsId,
                                     aEnv )
             == STL_SUCCESS );
    
    *aPendingRelationId = sRelationId;
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) smfSetTablespaceOfflineState( aTbsId,
                                                 sOfflineState,
                                                 sOfflineLsn,
                                                 aEnv );
        default:
            break;
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Offline 시에 남았던 Pending Operation을 수행한다.
 * @param[in]     aStatement         Statement 포인터
 * @param[in]     aTbsId             Tablespace Identifier
 * @param[in]     aPendingRelationId Pending Relation Identifier
 * @param[in,out] aEnv               Environment 정보
 * @remarks ALTER TABLESPACE ... ONLINE의 제일 마지막 단계에서 호출되어야 한다.
 */
stlStatus smlFinalizeOnlineTablespace( smlStatement * aStatement,
                                       smlTbsId       aTbsId,
                                       stlInt64       aPendingRelationId,
                                       smlEnv       * aEnv )

{
    void  * sRelationHandle;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    if( aPendingRelationId != SML_INVALID_RELATION_ID )
    {
        STL_TRY( smlGetRelationHandle( aPendingRelationId,
                                       &sRelationHandle,
                                       aEnv )
                 == STL_SUCCESS );
    
        /**
         * execute pending operation
         * - 수행된 pending operation은 rollback 될수 없다.
         */

        STL_TRY( smdExecutePendingOperation( aStatement,
                                             sRelationHandle,
                                             aEnv )
                 == STL_SUCCESS );
        
        /**
         * truncate pending relation
         */

        STL_TRY( smdTruncatePendingRelation( aStatement,
                                             aTbsId,
                                             aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}
/**
 * @brief Tablespace에 Datafile을 추가한다.
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsId 대상 Tablespace Identifier
 * @param[in] aDatafileAttr 추가할 Datafile 속성 구조체
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlAddDatafile( smlStatement    * aStatement,
                          smlTbsId          aTbsId,
                          smlDatafileAttr * aDatafileAttr,
                          smlEnv          * aEnv )
{
    smlDatafileAttr * sDatafileAttr;
    stlInt32          sState = 0;
    stlBool           sIsSuccess;
    stlChar           sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    sDatafileAttr = aDatafileAttr;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smfSetBackupWaitingState( aTbsId, &sIsSuccess, aEnv ) == STL_SUCCESS );

    /**
     * backup이 진행중이면 exception
     */
    STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_ERR_BACKUP_IN_PROGRESS );
    sState = 1;

    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );

    while( sDatafileAttr != NULL )
    {
        STL_TRY( smtAddDatafile( aStatement,
                                 aTbsId,
                                 sDatafileAttr,
                                 aEnv ) == STL_SUCCESS );
    
        sDatafileAttr = sDatafileAttr->mNext;
    }

    /**
     * OPEN phase에서만 Interface 직접 호출로 수행되며
     * MOUNT phase에서는 Event Handler로 수행된다.
     */
    STL_DASSERT( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN );

    if( smlGetDataStoreMode() < SML_DATA_STORE_MODE_TDS )
    {
        STL_TRY( smgExecutePendOp( SML_PEND_ACTION_COMMIT,
                                   0, /* aStatementTcn */
                                   aEnv )
                 == STL_SUCCESS );
        
        sDatafileAttr = aDatafileAttr;
        
        while( sDatafileAttr != NULL )
        {
            STL_TRY( smpFlushPagesForTablespace( aStatement,
                                                 aTbsId,
                                                 SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL,
                                                 sDatafileAttr->mDatafileId,
                                                 aEnv )
                     == STL_SUCCESS );
    
            sDatafileAttr = sDatafileAttr->mNext;
        }
    }

    sState = 0;
    STL_TRY( smfResetBackupWaitingState( aTbsId, aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BACKUP_IN_PROGRESS )
    {
        smfGetTbsName( aTbsId, sTbsName, aEnv );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_BACKUP_IN_PROGRESS,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsName );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfResetBackupWaitingState( aTbsId, aEnv );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 File이 File System에 존재하는지 검사한다.
 * @param[in] aFileName 검사할 Datafile Name
 * @param[out] aExist 파일 존재 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlExistDatafile( stlChar  * aFileName,
                            stlBool  * aExist,
                            smlEnv   * aEnv )
{
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar   sSystemTbsDir[STL_MAX_FILE_PATH_LENGTH];
    stlInt32  sFileNameLen;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlGetPropertyValueByName( "SYSTEM_TABLESPACE_DIR",
                                        sSystemTbsDir,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlMergeAbsFilePath( sSystemTbsDir,
                                  aFileName,
                                  sFileName,
                                  STL_MAX_FILE_PATH_LENGTH,
                                  &sFileNameLen,
                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlExistFile( sFileName,
                           aExist,
                           KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 Datafile이 Database에 존재하는지 검사한다.
 * @param[in] aFileName 검사할 Datafile Name
 * @param[out] aTbsId 찾은 Tablespace Identifier (nullable)
 * @param[out] aDatafileId 찾은 Datafile Identifier (nullable)
 * @param[out] aExist 파일 존재 여부
 * @param[out] aAged Aging 대상 파일 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlIsUsedDatafile( stlChar   * aFileName,
                             smlTbsId  * aTbsId,
                             stlInt32  * aDatafileId,
                             stlBool   * aExist,
                             stlBool   * aAged,
                             smlEnv    * aEnv )
{
    stlChar        sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar        sSystemTbsDir[STL_MAX_FILE_PATH_LENGTH];
    stlInt32       sFileNameLen;
    smlTbsId       sTbsId;
    smlTbsId       sTargetTbsId = SML_INVALID_TBS_ID;
    smlDatafileId  sDatafileId;
    smlDatafileId  sTargetDatafileId = SML_INVALID_DATAFILE_ID;
    stlBool        sTbsExist;
    stlBool        sFound;
    stlBool        sAged = STL_FALSE;
    
    *aExist = STL_FALSE;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlGetPropertyValueByName( "SYSTEM_TABLESPACE_DIR",
                                        sSystemTbsDir,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlMergeAbsFilePath( sSystemTbsDir,
                                  aFileName,
                                  sFileName,
                                  STL_MAX_FILE_PATH_LENGTH,
                                  &sFileNameLen,
                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );
                 
    while( sTbsExist == STL_TRUE )
    {
        STL_TRY( smfFindDatafileId( sTbsId,
                                    sFileName,
                                    &sDatafileId,
                                    &sFound,
                                    aEnv )
                 == STL_SUCCESS );

        if( sFound == STL_TRUE )
        {
            sTargetTbsId = sTbsId;
            sTargetDatafileId = sDatafileId;
            sAged = ( (smfGetDatafileState( sTbsId, sDatafileId ) == SMF_DATAFILE_STATE_AGING) ?
                      STL_TRUE : STL_FALSE );
            *aExist = STL_TRUE;
            break;
        }
        
        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }

    if( *aExist != STL_TRUE )
    {
        STL_TRY( knlExistDbFile( aFileName,
                                 aExist,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    if( aTbsId != NULL )
    {
        *aTbsId = sTargetTbsId;
    }
            
    if( aDatafileId != NULL )
    {
        *aDatafileId = sTargetDatafileId;
    }
            
    if( aAged != NULL )
    {
        *aAged = sAged;
    }
            
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 Memory 이름이 Database에 존재하는지 검사한다.
 * @param[in] aFileName 검사할 Datafile Name
 * @param[out] aTbsId 찾은 Tablespace Identifier (nullable)
 * @param[out] aDatafileId 찾은 Datafile Identifier (nullable)
 * @param[out] aExist 파일 존재 여부
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlIsUsedMemory( stlChar   * aFileName,
                           smlTbsId  * aTbsId,
                           stlInt32  * aDatafileId,
                           stlBool   * aExist,
                           smlEnv    * aEnv )
{
    smlTbsId       sTbsId;
    smlDatafileId  sDatafileId;
    stlBool        sTbsExist;
    stlBool        sFound;
    
    *aExist = STL_FALSE;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );
                 
    while( sTbsExist == STL_TRUE )
    {
        if( SML_TBS_IS_MEMORY_TEMPORARY( SMF_GET_TBS_ATTR(sTbsId) ) == STL_FALSE )
        {
            STL_TRY( smfNextTbs( &sTbsId,
                                 &sTbsExist,
                                 aEnv ) == STL_SUCCESS );
            continue;
        }
        
        STL_TRY( smfFindDatafileId( sTbsId,
                                    aFileName,
                                    &sDatafileId,
                                    &sFound,
                                    aEnv )
                 == STL_SUCCESS );

        if( sFound == STL_TRUE )
        {
            if( aTbsId != NULL )
            {
                *aTbsId = sTbsId;
            }
            
            if( aDatafileId != NULL )
            {
                *aDatafileId = sDatafileId;
            }
            
            *aExist = STL_TRUE;
            break;
        }
        
        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 Datafile의 이름을 절대 경로로 변경한다.
 * @param[in,out] aFileName Merge할 Datafile Name
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlConvAbsDatafilePath( stlChar  * aFileName,
                                  smlEnv   * aEnv )
{
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar   sSystemTbsDir[STL_MAX_FILE_PATH_LENGTH];
    stlInt32  sFileNameLen;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlGetPropertyValueByName( "SYSTEM_TABLESPACE_DIR",
                                        sSystemTbsDir,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( stlMergeAbsFilePath( sSystemTbsDir,
                                  aFileName,
                                  sFileName,
                                  STL_MAX_FILE_PATH_LENGTH,
                                  &sFileNameLen,
                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    stlStrncpy( aFileName,
                sFileName,
                STL_MAX_FILE_PATH_LENGTH );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Tablespace에서 하나의 Datafile을 삭제한다.
 * @param[in] aStatement Statement 포인터
 * @param[in] aTbsId 대상 Tablespace Identifier
 * @param[in] aDatafileName 삭제할 Datafile 이름
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlDropDatafile( smlStatement * aStatement,
                           smlTbsId       aTbsId,
                           stlChar      * aDatafileName,
                           smlEnv       * aEnv )
{
    stlChar   sFileName[STL_MAX_FILE_PATH_LENGTH];
    stlChar   sSystemTbsDir[STL_MAX_FILE_PATH_LENGTH];
    stlInt32  sFileNameLen;
    stlInt32  sState = 0;
    stlBool   sIsSuccess;
    stlChar   sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    /**
     * BACKUP이 진행 중일 경우 DROP DATAFILE이 수행될 수 없다.
     */
    STL_TRY( smfSetBackupWaitingState( aTbsId, &sIsSuccess, aEnv ) == STL_SUCCESS );

    /**
     * backup이 진행중이면 exception
     */
    STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_ERR_BACKUP_IN_PROGRESS );
    sState = 1;

    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );

    if( SML_TBS_IS_MEMORY_TEMPORARY( SMF_GET_TBS_ATTR(aTbsId) ) == STL_FALSE )
    {
        STL_TRY( knlGetPropertyValueByName( "SYSTEM_TABLESPACE_DIR",
                                            sSystemTbsDir,
                                            KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    
        STL_TRY( stlMergeAbsFilePath( sSystemTbsDir,
                                      aDatafileName,
                                      sFileName,
                                      STL_MAX_FILE_PATH_LENGTH,
                                      &sFileNameLen,
                                      KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        stlStrncpy( sFileName, aDatafileName, STL_MAX_FILE_PATH_LENGTH );
    }

    STL_TRY( smtDropDatafile( aStatement,
                              aTbsId,
                              sFileName,
                              aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smfResetBackupWaitingState( aTbsId, aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BACKUP_IN_PROGRESS )
    {
        smfGetTbsName( aTbsId, sTbsName, aEnv );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_BACKUP_IN_PROGRESS,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sTbsName );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfResetBackupWaitingState( aTbsId, aEnv );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Datafile들의 이름을 변경한다.
 * @param[in]     aStatement       Statement 포인터
 * @param[in]     aTbsId           대상 Tablespace Identifier
 * @param[in]     aOrgDatafileAttr 변경전 Datafile 속성 구조체
 * @param[in]     aNewDatafileAttr 변경후 Datafile 속성 구조체
 * @param[in,out] aEnv             Environment 정보
 */
stlStatus smlRenameDatafiles( smlStatement    * aStatement,
                              smlTbsId          aTbsId,
                              smlDatafileAttr * aOrgDatafileAttr,
                              smlDatafileAttr * aNewDatafileAttr,
                              smlEnv          * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    /**
     * OPEN phase에서만 Interface 직접 호출로 수행되며
     * MOUNT phase에서는 Event Handler로 수행된다.
     */
    STL_DASSERT( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN );

    STL_TRY( smfRenameDatafiles( aStatement,
                                 aTbsId,
                                 aOrgDatafileAttr,
                                 aNewDatafileAttr,
                                 aEnv )
             == STL_SUCCESS );

    STL_TRY( smgExecutePendOp( SML_PEND_ACTION_COMMIT,
                               0, /* aStatementTcn */
                               aEnv )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 첫번째 Tablespace 정보를 얻는다.
 * @param[out]    aIterator  Tablespace Iterator 포인터
 * @param[out]    aTbsId     대상 Tablespace Identifier
 * @param[out]    aTbsName   대상 Tablespace 이름
 * @param[out]    aTbsAttr   대상 Tablespace 속성 정보
 * @param[in,out] aEnv       Environment 정보
 */
stlStatus smlFetchFirstTbsInfo( void           ** aIterator,
                                smlTbsId        * aTbsId,
                                stlChar         * aTbsName,
                                stlUInt32       * aTbsAttr,
                                smlEnv          * aEnv )
{
    smfTbsPersData sTbsData;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    STL_TRY( smfFirstTbsPersData( &sTbsData,
                                  aIterator,
                                  aEnv )
             == STL_SUCCESS );

    if( *aIterator != NULL )
    {
        *aTbsId = sTbsData.mTbsId;
        *aTbsAttr = sTbsData.mAttr;
        stlStrncpy( aTbsName,
                    sTbsData.mName,
                    STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}
                           
/**
 * @brief 주어진 aIterator의 다음(next) Tablespace 정보를 얻는다.
 * @param[in,out] aIterator  Tablespace Iterator 포인터
 * @param[out]    aTbsId     대상 Tablespace Identifier
 * @param[out]    aTbsName   대상 Tablespace 이름
 * @param[out]    aTbsAttr   대상 Tablespace 속성 정보
 * @param[in,out] aEnv       Environment 정보
 */
stlStatus smlFetchNextTbsInfo( void           ** aIterator,
                               smlTbsId        * aTbsId,
                               stlChar         * aTbsName,
                               stlUInt32       * aTbsAttr,
                               smlEnv          * aEnv )
{
    smfTbsPersData sTbsData;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smfNextTbsPersData( &sTbsData,
                                 aIterator,
                                 aEnv )
             == STL_SUCCESS );

    if( *aIterator != NULL )
    {
        *aTbsId = sTbsData.mTbsId;
        *aTbsAttr = sTbsData.mAttr;
        stlStrncpy( aTbsName,
                    sTbsData.mName,
                    STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}
                           
/**
 * @brief 주어진 Tablespace의 유효성을 검사한다.
 * @param[in] aTbsId 검사할 Tablespace identifier
 * @param[in] aValidateBehavior Validation Behavior
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlValidateTablespace( smlTbsId   aTbsId,
                                 stlInt32   aValidateBehavior,
                                 smlEnv   * aEnv )
{
    stlChar sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    STL_TRY_THROW( SMF_IS_ONLINE_TBS( aTbsId ) == STL_TRUE,
                   RAMP_ERR_ACCESS_OFFLINE_TBS );

    STL_TRY_THROW( SMF_IS_ONLINE_TBS_FOR_SCAN( aTbsId ) == STL_TRUE,
                   RAMP_ERR_ACCESS_OFFLINE_TBS );

    if( aValidateBehavior & SME_VALIDATION_BEHAVIOR_UPDATE )
    {
        STL_DASSERT( (smfGetTbsState( aTbsId ) != SMF_TBS_STATE_DROPPING) &&
                     (smfGetTbsState( aTbsId ) != SMF_TBS_STATE_AGING)    &&
                     (smfGetTbsState( aTbsId ) != SMF_TBS_STATE_DROPPED) );
    }
    else
    {
        STL_DASSERT( smfGetTbsState( aTbsId ) != SMF_TBS_STATE_DROPPED );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_ACCESS_OFFLINE_TBS )
    {
        smfGetTbsName( aTbsId, sTbsName, aEnv );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ACCESS_OFFLINE_TBS,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sTbsName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 Tablespace가 ONLINE Tablespace인지 반환한다.
 * @param[in] aTbsId 검사할 Tablespace identifier
 * @return Online 여부
 */
stlBool smlIsOnlineTablespace( smlTbsId aTbsId )
{
    return SMF_IS_ONLINE_TBS( aTbsId );
}

/**
 * @brief 주어진 Tablespace가 Logging Tablespace인지 반환한다.
 * @param[in] aTbsId 검사할 Tablespace identifier
 * @return Logging 여부
 */
stlBool smlIsLoggingTablespace( smlTbsId aTbsId )
{
    return SMF_IS_LOGGING_TBS( aTbsId );
}

/**
 * @brief 주어진 Tablespace의 extent size(byte)를 반환한다.
 * @param[in] aTbsId Tablespace identifier
 * @return extent size
 */
stlInt64 smlGetTablespaceExtentSize( smlTbsId aTbsId )
{
    return SMF_GET_PAGE_COUNT_IN_EXTENT( aTbsId ) * SMP_PAGE_SIZE;
}

/**
 * @brief 주어진 Tablespace의 이름을 변경한다.
 * @param[in]     aStatement         Statement 포인터
 * @param[in]     aTbsId 대상        Tablespace identifier
 * @param[in]     aNewTablespaceName  변경될 Tablespace 이름
 * @param[in,out] aEnv                Environment 정보
 */
stlStatus smlRenameTablespace( smlStatement * aStatement,
                               smlTbsId       aTbsId,
                               stlChar      * aNewTablespaceName,
                               smlEnv       * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( smfRenameTbs( aStatement,
                           aTbsId,
                           aNewTablespaceName,
                           aEnv )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 Tablespace Id가 Controlfile에 존재하는지 검사한다.
 * @param[in] aTbsId 검사할 Tablespace Id
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlExistTablespace( stlInt64   aTbsId,
                              void     * aEnv )
{
    smlTbsId   sTbsId = aTbsId;

    STL_DASSERT( smlEnterStorageManager( SML_ENV( aEnv ) ) == STL_SUCCESS );
    
    STL_TRY( smfIsValidTbs( sTbsId ) == STL_TRUE );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

stlStatus smlValidateTbs( smlEnv * aEnv )
{
    return smfValidateTbs( aEnv );
}

/**
 * @brief Offline Tablespace를 수행하기 위해 Database의 상태, archivelog mode등을 확인한다.
 * @param[in] aOfflineBehavior Offline tablespace를 위한 behavior(NORMAL or IMMEDIATE)
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smlValidateOfflineTablespace( smlOfflineBehavior   aOfflineBehavior,
                                        smlEnv             * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( SML_ENV( aEnv ) ) == STL_SUCCESS );

    /* Offline Tablespace를 수행할 수 있는지 체크한다. */
    STL_TRY( smfValidateOfflineTablespace( aOfflineBehavior,
                                           aEnv )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}


stlStatus smlDropUnregisteredTablespace( smlTbsId   aTbsId,
                                         smlEnv   * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( SML_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( smtDropUnregisteredTbs( aTbsId,
                                     aEnv ) == STL_SUCCESS );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}

/** @} */
