/*******************************************************************************
 * smfBackupMgr.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smfBackupMgr.c 9455 2013-08-27 08:23:04Z kyungoh $
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
#include <smg.h>
#include <sma.h>
#include <smfDef.h>
#include <smf.h>
#include <smp.h>
#include <smr.h>
#include <smd.h>
#include <smxl.h>
#include <smxm.h>
#include <smfBackupMgr.h>
#include <smfTbsMgr.h>
#include <smfRecovery.h>
#include <smlGeneral.h>

/**
 * @file smfBackupMgr.c
 * @brief Storage Manager Layer Backup Manager Internal Routines
 */

/**
 * @addtogroup smf
 * @{
 */

stlStatus smfSetBackupWaitingState( smlTbsId     aTbsId,
                                    stlBool    * aIsSuccess,
                                    smlEnv     * aEnv )
{
    smfTbsBackupInfo * sTbsBackupInfo;
    stlInt32           sState = 0;

    sTbsBackupInfo = &gSmfWarmupEntry->mTbsInfoArray[aTbsId]->mBackupInfo;

    /**
     * Tablespace Backup State ������ ���� latch ȹ��
     */
    STL_TRY( smfAcquireBackupTbs( aTbsId, aEnv ) == STL_SUCCESS );
    sState = 1;

    if( SMF_IS_TBS_BACKUP_IN_PROGRESS( aTbsId ) == STL_TRUE )
    {
        *aIsSuccess = STL_FALSE;
    }
    else
    {
        /**
         * mBackupWaitCount ���� ��Ű�� BACKUP_WAITING ���� ����
         */
        if( sTbsBackupInfo->mBackupWaitCount == 0 )
        {
            SMF_SET_TBS_BACKUP_STATE( aTbsId, SMF_BACKUP_STATE_WAITING );
        }

        sTbsBackupInfo->mBackupWaitCount++;
        *aIsSuccess = STL_TRUE;
    }

    sState = 0;
    STL_TRY( smfReleaseBackupTbs( aTbsId, aEnv ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_BACKUPWAITING_AFTER_SET,
                    KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseBackupTbs( aTbsId, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfResetBackupWaitingState( smlTbsId     aTbsId,
                                      smlEnv     * aEnv )
{
    smfTbsBackupInfo * sTbsBackupInfo;
    stlInt32           sState = 0;

    sTbsBackupInfo = &gSmfWarmupEntry->mTbsInfoArray[aTbsId]->mBackupInfo;

    /**
     * Tablespace Backup State ������ ���� latch ȹ��
     */
    STL_TRY( smfAcquireBackupTbs( aTbsId, aEnv ) == STL_SUCCESS );
    sState = 1;

    if( (SMF_TBS_BACKUP_STATE( aTbsId ) != SMF_BACKUP_STATE_WAITING) ||
        (sTbsBackupInfo->mBackupWaitCount == 0) )
    {
        /**
         * Backup�� ��ٸ��� �� DDL�̳� Flusher�� WAITING ���¸�
         * �����Ϸ��� �ϴµ�, �̹� WAITING�� �ƴϰų� wait count�� 0�̸�
         * ������ ������, service�ϴµ� ������ �����Ƿ� trc�� logging�� �Ѵ�.
         */
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV(aEnv),
                            KNL_LOG_LEVEL_WARNING,
                            "[WARNING] tablespace (%d) backup end fail\n",
                            aTbsId )
                 == STL_SUCCESS );

        sTbsBackupInfo->mBackupWaitCount = 0;
    }

    /**
     * mBackupWaitCount ���� ��Ų��.
     */
    if( sTbsBackupInfo->mBackupWaitCount > 0 )
    {
        sTbsBackupInfo->mBackupWaitCount--;
    }

    if( sTbsBackupInfo->mBackupWaitCount == 0 )
    {
        SMF_SET_TBS_BACKUP_STATE( aTbsId, SMF_BACKUP_STATE_NONE );
    }

    sState = 0;
    STL_TRY( smfReleaseBackupTbs( aTbsId, aEnv ) == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_BACKUPWAITING_AFTER_RESET,
                    KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseBackupTbs( aTbsId, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfAcquireBackupTbs( smlTbsId     aTbsId,
                               smlEnv     * aEnv )
{
    smfTbsBackupInfo * sTbsBackupInfo;
    stlBool            sIsTimedOut;

    sTbsBackupInfo = &gSmfWarmupEntry->mTbsInfoArray[aTbsId]->mBackupInfo;

    STL_TRY( knlAcquireLatch( &sTbsBackupInfo->mBackupLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfReleaseBackupTbs( smlTbsId     aTbsId,
                               smlEnv     * aEnv )
{
    smfTbsBackupInfo * sTbsBackupInfo;

    sTbsBackupInfo = &gSmfWarmupEntry->mTbsInfoArray[aTbsId]->mBackupInfo;

    STL_TRY( knlReleaseLatch( &sTbsBackupInfo->mBackupLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace backup�� �����Ѵ�.
 * @param[in]     aBackupType   Full/Incremental Backup
 * @param[in]     aCommand      Backup BEGIN/END
 * @param[in]     aTbsId        tablespace identifier
 * @param[in,out] aEnv          Environment ����
 */
stlStatus smfBackupTablespace( stlChar     aBackupType,
                               stlChar     aCommand,
                               smlTbsId    aTbsId,
                               smlEnv    * aEnv )
{
    stlBool           sSuccess;
    stlBool           sDropped;
    stlBool           sOfflined;
    stlChar           sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    smpFlushTbsArg    sArg;
    smrChkptArg       sChkptArg;

    switch( aCommand )
    {
        case SML_BACKUP_COMMAND_BEGIN:
            STL_TRY_THROW( SML_TBS_IS_TEMPORARY( SMF_GET_TBS_ATTR(aTbsId) ) == STL_FALSE,
                           RAMP_ERR_TEMPORARY_TABLESPACE );

            /**
             * Full Backup�� �����ϱ� ���� checkpoint�� �����Ѵ�.
             */
            stlMemset( (void *)&sChkptArg, 0x00, STL_SIZEOF(smrChkptArg) );
            sChkptArg.mLogfileSeq = STL_INT64_MAX;
            sChkptArg.mFlushBehavior = SMP_FLUSH_LOGGED_PAGE;

            STL_TRY( knlAddEnvEvent( SML_EVENT_CHECKPOINT,
                                     NULL,
                                     (void*)&sChkptArg,
                                     STL_SIZEOF( smrChkptArg ),
                                     SML_CHECKPOINT_ENV_ID,
                                     KNL_EVENT_WAIT_POLLING,
                                     STL_FALSE,  /* aForceSuccess */
                                     KNL_ENV( aEnv ) ) == STL_SUCCESS );

            STL_TRY( smfBackupTablespaceBegin( aBackupType,
                                               aTbsId,
                                               &sDropped,
                                               &sOfflined,
                                               SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sDropped  != STL_TRUE, RAMP_ERR_DROPPED_TABLESPACE );
            STL_TRY_THROW( sOfflined != STL_TRUE, RAMP_ERR_OFFLINE_TABLESPACE );
            break;

        case SML_BACKUP_COMMAND_END:
            STL_TRY( smfBackupTablespaceEnd( aBackupType,
                                             aTbsId,
                                             &sSuccess,
                                             SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sSuccess == STL_TRUE, RAMP_ERR_NOT_BACKUP_IN_PROGRESS );

            /**
             * Backup�� �Ϸ��� �� tablespace�� flush�Ѵ�.
             */
            sArg.mTbsId = aTbsId;
            sArg.mDatafileId = SML_INVALID_DATAFILE_ID;
            sArg.mReason = SMF_CHANGE_DATAFILE_HEADER_FOR_GENERAL;

            STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH_TBS,
                                     NULL,                         /* aEventMem */
                                     (void*)&sArg,                 /* aData */
                                     STL_SIZEOF(smpFlushTbsArg),   /* aDataSize */
                                     SML_PAGE_FLUSHER_ENV_ID,
                                     KNL_EVENT_WAIT_POLLING,
                                     STL_FALSE,  /* aForceSuccess */
                                     KNL_ENV( aEnv ) ) == STL_SUCCESS );

            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TEMPORARY_TABLESPACE )
    {
        smfGetTbsName( aTbsId, sTbsName, aEnv );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CANNOT_BACKUP_TEMPORARY_TABLESPACE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sTbsName );
    }

    STL_CATCH( RAMP_ERR_DROPPED_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CANNOT_BACKUP_DROPPED_TABLESPACE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_OFFLINE_TABLESPACE )
    {
        smfGetTbsName( aTbsId, sTbsName, aEnv );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CANNOT_BACKUP_OFFLINE_TABLESPACE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sTbsName );
    }

    STL_CATCH( RAMP_ERR_NOT_BACKUP_IN_PROGRESS )
    {
        smfGetTbsName( aTbsId, sTbsName, aEnv );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CANNOT_END_BACKUP,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sTbsName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief �־��� Tablespace�� Backup�ϱ� ���� Begin�Ѵ�.
 * @param[in]     aBackupType   Full/Incremental Backup
 * @param[in]     aTbsId tablespace identifier
 * @param[out]    aDropped backup�Ϸ��� tablespace�� drop�Ǿ����� ����
 * @param[out]    aOfflined backup�Ϸ��� tablespace�� offline�Ǿ����� ����
 * @param[in,out] aEnv    Environment ����
 */
stlStatus smfBackupTablespaceBegin( stlChar    aBackupType,
                                    smlTbsId   aTbsId,
                                    stlBool  * aDropped,
                                    stlBool  * aOfflined,
                                    smlEnv   * aEnv )
{
    smfTbsInfo  * sTbsInfo;
    stlInt32      sBackupState;
    stlInt32      sState = 0;
    stlChar       sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    *aDropped = STL_FALSE;
    *aOfflined = STL_FALSE;

    sTbsInfo = gSmfWarmupEntry->mTbsInfoArray[aTbsId];

    if( sTbsInfo == NULL )
    {
        *aDropped = STL_TRUE;
        STL_THROW( SKIP_BACKUP_TABLESPACE );
    }

    if( sTbsInfo->mTbsPersData.mState != SMF_TBS_STATE_CREATED )
    {
        *aDropped = STL_TRUE;
        STL_THROW( SKIP_BACKUP_TABLESPACE );
    }

    if( aBackupType == SML_BACKUP_TYPE_FULL )
    {
        sBackupState = SMF_BACKUP_STATE_FULL_BACKUP;
    }
    else
    {
        sBackupState = SMF_BACKUP_STATE_INCREMENTAL_BACKUP;
    }

    STL_TRY( smfAcquireBackupTbs( aTbsId, aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Tablespace�� Backup�� �������̸� exception ó��
     */
    STL_TRY_THROW( ((SMF_TBS_BACKUP_STATE( aTbsId ) & sBackupState) !=
                    sBackupState),
                   RAMP_ERR_CANNOT_BEGIN_BACKUP );

    /**
     * DDL�̳� Flush�� �������̸� exception
     */
    STL_TRY_THROW( (SMF_TBS_BACKUP_WAIT_COUNT( aTbsId ) == 0) &&
                   (SMF_TBS_BACKUP_STATE( aTbsId ) != SMF_BACKUP_STATE_WAITING),
                   RAMP_ERR_CANNOT_BACKUP_BUSY_TBS );

    if( (SMF_IS_ONLINE_TBS( aTbsId ) != STL_TRUE) && (aBackupType == SML_BACKUP_TYPE_FULL) )
    {
        sState = 0;
        STL_TRY( smfReleaseBackupTbs( aTbsId, aEnv ) == STL_SUCCESS );

        *aOfflined = STL_TRUE;
        STL_THROW( SKIP_BACKUP_TABLESPACE );
    }

    /**
     * ����ؾ� �� wait�� ������ BACKUP_PROGRESS �� ����
     */
    SMF_SET_TBS_BACKUP_STATE( aTbsId,
                              (SMF_TBS_BACKUP_STATE( aTbsId ) | sBackupState) );

    sState = 0;
    STL_TRY( smfReleaseBackupTbs( aTbsId, aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[BACKUP] tablespace (%d) backup has begun\n", aTbsId )
             == STL_SUCCESS );

    STL_RAMP( SKIP_BACKUP_TABLESPACE );

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMFBACKUPTBSBEGIN_AFTER_SETTING,
                    KNL_ENV(aEnv) );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_BEGIN_BACKUP )
    {
        smfGetTbsName( aTbsId, sTbsName, aEnv );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CANNOT_BEGIN_BACKUP,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sTbsName );
    }

    STL_CATCH( RAMP_ERR_CANNOT_BACKUP_BUSY_TBS )
    {
        smfGetTbsName( aTbsId, sTbsName, aEnv );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CANNOT_BACKUP_BUSY_TABLESPACE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sTbsName );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseBackupTbs( aTbsId, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief �־��� Tablespace�� Backup�� �����Ѵ�.
 * @param[in]     aBackupType   Full/Incremental Backup
 * @param[in]     aTbsId Tablespace identifier
 * @param[out]    aSuccess Backup ���� ���� ����
 * @param[in,out] aEnv   Environment ����
 */
stlStatus smfBackupTablespaceEnd( stlChar    aBackupType,
                                  smlTbsId   aTbsId,
                                  stlBool  * aSuccess,
                                  smlEnv   * aEnv )
{
    stlInt32   sBackupState;
    stlInt32   sState = 0;

    *aSuccess = STL_FALSE;

    if( aBackupType == SML_BACKUP_TYPE_FULL )
    {
        sBackupState = SMF_BACKUP_STATE_FULL_BACKUP;
    }
    else
    {
        sBackupState = SMF_BACKUP_STATE_INCREMENTAL_BACKUP;
    }

    STL_TRY( smfAcquireBackupTbs( aTbsId, aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Tablespace�� Backup�� �������̴�.
     */

    if( (SMF_TBS_BACKUP_STATE( aTbsId ) & sBackupState) == sBackupState )
    {
        SMF_SET_TBS_BACKUP_STATE( aTbsId,
                                  (SMF_TBS_BACKUP_STATE( aTbsId ) & ~sBackupState) );

        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[BACKUP] tablespace (%d) backup has ended\n",
                            aTbsId )
                 == STL_SUCCESS );

        *aSuccess = STL_TRUE;
    }

    sState = 0;
    STL_TRY( smfReleaseBackupTbs( aTbsId, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseBackupTbs( aTbsId, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Database backup�� �����Ѵ�.
 * @param[in]     aBackupType   Full/Incremental Backup
 * @param[in]     aCommand      Backup BEGIN/END
 * @param[in]     aBackupLevel  Incremental Backup Level
 * @param[in,out] aEnv          Environment ����
 */
stlStatus smfBackupDatabase( stlChar    aBackupType,
                             stlChar    aCommand,
                             stlInt16   aBackupLevel,
                             smlEnv   * aEnv )
{
    switch( aCommand )
    {
        case SML_BACKUP_COMMAND_BEGIN:
            STL_TRY( smfBackupDatabaseBegin( aBackupType,
                                             SML_ENV( aEnv ) )
                     == STL_SUCCESS );
            break;
        case SML_BACKUP_COMMAND_END:
            STL_TRY( smfBackupDatabaseEnd( aBackupType,
                                           aBackupLevel,
                                           SML_ENV( aEnv ) )
                     == STL_SUCCESS );
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Database backup�� ���� begin backup�� �����Ѵ�.
 * @param[in]     aBackupType   Full/Incremental Backup
 * @param[in,out] aEnv          Environment ����
 */
stlStatus smfBackupDatabaseBegin( stlChar    aBackupType,
                                  smlEnv   * aEnv )
{
    smfDbBackupInfo * sBackupInfo;
    smlTbsId          sLastTbsId;
    smlTbsId          sTbsId = SML_INVALID_TBS_ID;
    stlInt32          sState = 0;
    stlBool           sTbsExist;
    stlBool           sSuccess;
    stlBool           sDropped;
    stlBool           sOfflined;
    stlBool           sIsTimedOut;
    smrChkptArg       sArg;

    sBackupInfo = &(gSmfWarmupEntry->mBackupInfo);

    /**
     * Full Backup�� �����ϱ� ���� checkpoint�� �����Ѵ�.
     */
    stlMemset( (void *)&sArg, 0x00, STL_SIZEOF(smrChkptArg) );
    sArg.mLogfileSeq = STL_INT64_MAX;
    sArg.mFlushBehavior = SMP_FLUSH_LOGGED_PAGE;

    STL_TRY( knlAddEnvEvent( SML_EVENT_CHECKPOINT,
                             NULL, /* aEventMem */
                             (void*)&sArg,
                             STL_SIZEOF( smrChkptArg ),
                             SML_CHECKPOINT_ENV_ID,
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE,  /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( knlAcquireLatch( &sBackupInfo->mBackupLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * BACKUP_STATE_WAITING �� ��� Backup�� begin�� �� ����.
     */
    STL_TRY_THROW( sBackupInfo->mBackupState != SMF_BACKUP_STATE_WAITING,
                   RAMP_ERR_CANNOT_BEGIN_BACKUP );

    if( aBackupType == SML_BACKUP_TYPE_FULL )
    {
        STL_TRY_THROW( ((sBackupInfo->mBackupState & SMF_BACKUP_STATE_FULL_BACKUP)
                        != SMF_BACKUP_STATE_FULL_BACKUP),
                       RAMP_ERR_CANNOT_BEGIN_BACKUP );

        sBackupInfo->mBackupState |= SMF_BACKUP_STATE_FULL_BACKUP;
    }
    else
    {
        /**
         * if( aBackupType == SML_BACKUP_TYPE_INCREMENTAL )
         */
        STL_TRY_THROW( ((sBackupInfo->mBackupState & SMF_BACKUP_STATE_INCREMENTAL_BACKUP)
                        != SMF_BACKUP_STATE_INCREMENTAL_BACKUP),
                       RAMP_ERR_CANNOT_BEGIN_BACKUP );

        sBackupInfo->mBackupState |= SMF_BACKUP_STATE_INCREMENTAL_BACKUP;
    }
    sState = 2;

    /**
     * DATABASE BEGIN BACKUP�� ���� ��� tablespace�� ����
     * BEGIN BACKUP�� �����Ѵ�.
     */
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );

    while( sTbsExist == STL_TRUE )
    {
        if( SMF_TBS_AVAILABLE_BACKUP( sTbsId ) == STL_TRUE )
        {
            STL_TRY( smfBackupTablespaceBegin( aBackupType,
                                               sTbsId,
                                               &sDropped,
                                               &sOfflined,
                                               aEnv ) == STL_SUCCESS );

            if( aBackupType == SML_BACKUP_TYPE_FULL )
            {
                if( (sDropped != STL_TRUE) && (sOfflined != STL_TRUE) )
                {
                    sBackupInfo->mBegunTbsCount++;
                }
            }
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &sBackupInfo->mBackupLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_INFO,
                        "[BACKUP] database backup has begun\n" )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_BEGIN_BACKUP )
    {
        if( (sBackupInfo->mBackupState & SMF_BACKUP_STATE_FULL_BACKUP) ==
            SMF_BACKUP_STATE_FULL_BACKUP )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SML_ERRCODE_DATABASE_IN_BACKUP,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) );
        }

        if( sBackupInfo->mBackupState == SMF_BACKUP_STATE_WAITING )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          SML_ERRCODE_DATABASE_IN_SHUTDOWN,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) );
        }
    }

    STL_FINISH;

    /**
     * TABLESPACE BEGIN BACKUP�� �����ϸ� ������ ������
     * tablespace�� ���� TABLESPACE END BACKUP�� �����Ѵ�.
     */
    if( sTbsId != SML_INVALID_TBS_ID )
    {
        sLastTbsId = sTbsId;

        (void)smfFirstTbs( &sTbsId, &sTbsExist, aEnv );

        while( sTbsExist == STL_TRUE )
        {
            if(  SMF_TBS_AVAILABLE_BACKUP( sTbsId ) == STL_TRUE )
            {
                (void)smfBackupTablespaceEnd( aBackupType,
                                              sTbsId,
                                              &sSuccess,
                                              aEnv );
            }

            (void)smfNextTbs( &sTbsId, &sTbsExist, aEnv );

            if( sLastTbsId == sTbsId )
            {
                break;
            }
        }
    }

    switch( sState )
    {
        case 2:
            if( aBackupType == SML_BACKUP_TYPE_FULL )
            {
                sBackupInfo->mBegunTbsCount = 0;
                sBackupInfo->mBackupState &= ~SMF_BACKUP_STATE_FULL_BACKUP;
            }
            else
            {
                sBackupInfo->mBackupState &= ~SMF_BACKUP_STATE_INCREMENTAL_BACKUP;
            }
        case 1:
            (void)knlReleaseLatch( &sBackupInfo->mBackupLatch, (knlEnv*)aEnv );
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Database backup�� ���� end backup�� �����Ѵ�.
 * @param[in]     aBackupType   Full/Incremental Backup
 * @param[in]     aBackupLevel  Incremental Backup Level
 * @param[in,out] aEnv     Environment ����
 */
stlStatus smfBackupDatabaseEnd( stlChar     aBackupType,
                                stlInt16    aBackupLevel,
                                smlEnv    * aEnv )
{
    smfDbBackupInfo * sBackupInfo;
    smlTbsId          sTbsId = SML_INVALID_TBS_ID;
    stlUInt16         sSuccessCount = 0;
    stlInt32          sState = 0;
    stlUInt16         sBegunTbsCount;
    stlBool           sTbsExist;
    stlBool           sSuccess;
    stlBool           sIsTimedOut;
    smpFlushArg       sArg;

    sBackupInfo = &(gSmfWarmupEntry->mBackupInfo);

    STL_TRY( knlAcquireLatch( &sBackupInfo->mBackupLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    if( aBackupType == SML_BACKUP_TYPE_FULL )
    {
        sBegunTbsCount = sBackupInfo->mBegunTbsCount;
        STL_TRY_THROW( ((sBackupInfo->mBackupState & SMF_BACKUP_STATE_FULL_BACKUP)
                        == SMF_BACKUP_STATE_FULL_BACKUP),
                       RAMP_ERR_CANNOT_END_BACKUP_DATABASE );
    }
    else
    {
        STL_TRY_THROW( ((sBackupInfo->mBackupState & SMF_BACKUP_STATE_INCREMENTAL_BACKUP)
                        == SMF_BACKUP_STATE_INCREMENTAL_BACKUP),
                       RAMP_ERR_CANNOT_END_BACKUP_DATABASE );
    }

    sBegunTbsCount = sBackupInfo->mBegunTbsCount;

    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );

    while( sTbsExist == STL_TRUE )
    {
        if(  SMF_TBS_AVAILABLE_BACKUP( sTbsId ) == STL_TRUE )
        {
            if( aBackupType == SML_BACKUP_TYPE_INCREMENTAL )
            {
                /**
                 * Level 0 Database Incremental Backup�� ���
                 * ��� Tablespace�� incremental backup ������ reset�Ѵ�.
                 */
                if( aBackupLevel == 0 )
                {
                    smfInitIncrementalBackupInfo( sTbsId );
                }
            }

            STL_TRY( smfBackupTablespaceEnd( aBackupType,
                                             sTbsId,
                                             &sSuccess,
                                             aEnv ) == STL_SUCCESS );

            if( sSuccess == STL_TRUE )
            {
                sSuccessCount++;
            }
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }

    if( aBackupType == SML_BACKUP_TYPE_FULL )
    {
        sBackupInfo->mBegunTbsCount = 0;
        sBackupInfo->mBackupState &= ~SMF_BACKUP_STATE_FULL_BACKUP;

        STL_TRY_THROW( sSuccessCount != 0, RAMP_ERR_ALL_TBS_NOT_IN_BACKUP );

        if( sSuccessCount != sBegunTbsCount )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[BACKUP] END BACKUP succeeded but some tablespaces are not in backup\n" )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV( aEnv ),
                                KNL_LOG_LEVEL_INFO,
                                "[BACKUP] database backup has ended\n" )
                     == STL_SUCCESS );
        }
    }
    else
    {
        sBackupInfo->mBackupState &= ~SMF_BACKUP_STATE_INCREMENTAL_BACKUP;
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &sBackupInfo->mBackupLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    /**
     * Backup�� �Ϸ��� �� datafile�� flush�Ѵ�.
     */
    sArg.mBehavior = SMP_FLUSH_DIRTY_PAGE;
    sArg.mLogging  = STL_FALSE;
    sArg.mForCheckpoint = STL_FALSE;

    STL_TRY( knlAddEnvEvent( SML_EVENT_PAGE_FLUSH,
                             NULL,     /* aEventMem */
                             &sArg,
                             STL_SIZEOF(smpFlushArg),
                             SML_PAGE_FLUSHER_ENV_ID,
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE,  /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_END_BACKUP_DATABASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CANNOT_END_BACKUP_DATABASE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_ALL_TBS_NOT_IN_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ALL_TBS_NOT_IN_BACKUP,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    /**
     * TABLESPACE END BACKUP�� �����ϸ� ������
     * tablespace�� ���� TABLESPACE END BACKUP�� �����Ѵ�.
     */
    if( sTbsId == SML_INVALID_TBS_ID )
    {
        (void)smfFirstTbs( &sTbsId, &sTbsExist, aEnv );
    }
    else
    {
        (void)smfNextTbs( &sTbsId, &sTbsExist, aEnv );
    }

    while( sTbsExist == STL_TRUE )
    {
        (void)smfBackupTablespaceEnd( aBackupType,
                                      sTbsId,
                                      &sSuccess,
                                      aEnv );

        (void)smfNextTbs( &sTbsId, &sTbsExist, aEnv );
    }

    if( aBackupType == SML_BACKUP_TYPE_FULL )
    {
        sBackupInfo->mBegunTbsCount = 0;
        sBackupInfo->mBackupState &= ~SMF_BACKUP_STATE_FULL_BACKUP;
    }
    else
    {
        sBackupInfo->mBackupState &= ~SMF_BACKUP_STATE_INCREMENTAL_BACKUP;
    }

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sBackupInfo->mBackupLatch, (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Shutdown�� ���� Backup�� Begin���� ���ϵ��� ��
 * @param[in,out] aEnv Environment ����
 */
stlStatus smfPreventBackupForShutdown( smlEnv  * aEnv )
{
    smfDbBackupInfo * sBackupInfo;
    smlTbsId          sLastTbsId;
    smlTbsId          sTbsId = SML_INVALID_TBS_ID;
    stlInt32          sState = 0;
    stlBool           sTbsExist;
    stlBool           sIsTimedOut;
    stlBool           sIsSuccess;

    sBackupInfo = &(gSmfWarmupEntry->mBackupInfo);

    STL_TRY( knlAcquireLatch( &sBackupInfo->mBackupLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * DATABASE BACKUP�� BEGIN���� �ʵ��� WAITING�� �����Ѵ�.
     *  - �̴� shutdown �ÿ��� ȣ��ǹǷ�, ���� shutdown �߿� session��
     *    ����Ǿ� backup state�� waiting ���°� �Ǿ� ���� ��� �״�� �����Ѵ�.
     */
    if( sBackupInfo->mBackupState != SMF_BACKUP_STATE_WAITING )
    {
        STL_TRY_THROW( sBackupInfo->mBackupState == SMF_BACKUP_STATE_NONE,
                       RAMP_ERR_CANNOT_PREVENT_BACKUP );

        sBackupInfo->mBackupState = SMF_BACKUP_STATE_WAITING;
        sState = 2;
    }

    /**
     * TABLESPACE BACKUP�� BEGIN���� �ʵ��� ��� tablespace�� ����
     * BACKUP waiting�� �����Ѵ�.
     */
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );

    while( sTbsExist == STL_TRUE )
    {
        if( SMF_TBS_AVAILABLE_BACKUP( sTbsId ) == STL_TRUE )
        {
            /**
             * Tablespace�� ��� backup waiting�� ������ �����Ǿ �������.
             */
            STL_TRY( smfSetBackupWaitingState( sTbsId,
                                               &sIsSuccess,
                                               aEnv ) == STL_SUCCESS );

            /**
             * backup�� ���� ���̸� exception
             */
            STL_TRY_THROW( sIsSuccess == STL_TRUE, RAMP_ERR_CANNOT_PREVENT_BACKUP );
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlReleaseLatch( &sBackupInfo->mBackupLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_PREVENT_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_CANNOT_PREVENT_BACKUP,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    /**
     * TABLESPACE BACKUP���¸� waiting���� �����ϴٰ� �����ϸ� ������ ������
     * tablespace�� ���� waiting�� �����Ѵ�.
     */
    if( sTbsId != SML_INVALID_TBS_ID )
    {
        sLastTbsId = sTbsId;

        (void)smfFirstTbs( &sTbsId, &sTbsExist, aEnv );

        while( sTbsExist == STL_TRUE )
        {
            if( sLastTbsId == sTbsId )
            {
                break;
            }

            if(  SMF_TBS_AVAILABLE_BACKUP( sTbsId ) == STL_TRUE )
            {
                (void)smfResetBackupWaitingState( sTbsId, aEnv );
            }

            (void)smfNextTbs( &sTbsId, &sTbsExist, aEnv );
        }
    }

    switch( sState )
    {
        case 2:
            sBackupInfo->mBackupState = SMF_BACKUP_STATE_NONE;
        case 1:
            (void)knlReleaseLatch( &sBackupInfo->mBackupLatch, (knlEnv*)aEnv );
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Backup�� Prevent�� ���� rollback�Ѵ�.
 * @param[in,out] aEnv Environment ����
 */
stlStatus smfRollbackPreventBackup( smlEnv  * aEnv )
{
    smfDbBackupInfo * sBackupInfo;
    smlTbsId          sLastTbsId;
    smlTbsId          sTbsId = SML_INVALID_TBS_ID;
    stlInt32          sState = 0;
    stlBool           sTbsExist;
    stlBool           sIsTimedOut;

    sBackupInfo = &(gSmfWarmupEntry->mBackupInfo);

    STL_TRY( knlAcquireLatch( &sBackupInfo->mBackupLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * ��� tablespace�� ���� BACKUP waiting ���¸� �����Ѵ�.
     * BACKUP waiting�� �����Ѵ�.
     */
    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );

    while( sTbsExist == STL_TRUE )
    {
        if( SMF_TBS_AVAILABLE_BACKUP( sTbsId ) == STL_TRUE )
        {
            STL_TRY( smfResetBackupWaitingState( sTbsId, aEnv ) == STL_SUCCESS );
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }


    /**
     * DATABASE�� BACKUP waiting���¸� �����Ѵ�.
     */
    sBackupInfo->mBackupState = SMF_BACKUP_STATE_NONE;

    sState = 0;
    STL_TRY( knlReleaseLatch( &sBackupInfo->mBackupLatch,
                              (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    /**
     * TABLESPACE�� BACKUP waiting�� �����ϴٰ� �����ϸ� ������
     * tablespace�� ���� waiting�� �����Ѵ�.
     */
    if( sTbsId != SML_INVALID_TBS_ID )
    {
        sLastTbsId = sTbsId;

        (void)smfFirstTbs( &sTbsId, &sTbsExist, aEnv );

        while( sTbsExist == STL_TRUE )
        {
            if(  SMF_TBS_AVAILABLE_BACKUP( sTbsId ) == STL_TRUE )
            {
                (void)smfResetBackupWaitingState( sTbsId, aEnv );
            }

            (void)smfNextTbs( &sTbsId, &sTbsExist, aEnv );

            if( sLastTbsId == sTbsId )
            {
                break;
            }
        }
    }

    if( sBackupInfo->mBackupState == SMF_BACKUP_STATE_WAITING )
    {
        sBackupInfo->mBackupState = SMF_BACKUP_STATE_NONE;
    }

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sBackupInfo->mBackupLatch, (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Database Backup�� �����Ѵ�.
 * @param[in] aBackupMsg Backup ����
 * @param[in,out] aEnv Environment
 */
stlStatus smfBackup( smlBackupMessage * aBackupMsg,
                     smlEnv           * aEnv )
{
    /**
     * Backup Object
     * (1) NONE : shutdown�� ���� backup�� prevention��
     * (2) CONTROLFILE : controlfile backup
     * (3) DATABASE, TABLESPACE : full, incremental backup ��ü
     */
    switch( aBackupMsg->mBackupObject )
    {
        case SML_BACKUP_OBJECT_TYPE_NONE:
            switch( aBackupMsg->mCommand )
            {
                case SML_BACKUP_COMMAND_PREVENT:
                    STL_TRY( smfPreventBackupForShutdown( SML_ENV(aEnv) ) == STL_SUCCESS );
                    break;

                case SML_BACKUP_COMMAND_PREVENT_ROLLBACK:
                    STL_TRY( smfRollbackPreventBackup( SML_ENV(aEnv) ) == STL_SUCCESS );
                    break;
            }
            break;

        case SML_BACKUP_OBJECT_TYPE_CONTROLFILE:
            STL_TRY( smfBackupCtrlFile( SML_BACKUP_TYPE_FULL,
                                        aBackupMsg->mTarget,
                                        SML_ENV(aEnv) ) == STL_SUCCESS );
            break;

        case SML_BACKUP_OBJECT_TYPE_DATABASE:
        case SML_BACKUP_OBJECT_TYPE_TABLESPACE:
            switch( aBackupMsg->mBackupType )
            {
                case SML_BACKUP_TYPE_FULL:
                    STL_TRY( smfFullBackup( aBackupMsg->mCommand,
                                            aBackupMsg->mTbsId,
                                            SML_ENV( aEnv ) ) == STL_SUCCESS );
                    break;

                case SML_BACKUP_TYPE_INCREMENTAL:
                    STL_TRY( smfIncrementalBackup( aBackupMsg->mTbsId,
                                                   aBackupMsg->mIncrementalBackupOption,
                                                   aBackupMsg->mIncrementalBackupLevel,
                                                   SML_ENV( aEnv ) )
                             == STL_SUCCESS );
                    break;
            }
            break;
    }

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smfIncrementalBackup( smlTbsId   aTbsId,
                                stlChar    aBackupOption,
                                stlInt16   aBackupLevel,
                                smlEnv   * aEnv )
{
    stlFile        sFile;
    smrLsn         sPrevBackupLsn;
    smrLsn         sBackupLsn;
    smrLid         sBackupLid;
    stlInt64       sBackupSeq;
    stlInt32       sState = 0;
    stlInt32       sFlag;
    stlInt64       sDatabaseIo;
    stlChar        sFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH];
    stlChar        sAbsFileName[STL_MAX_FILE_PATH_LENGTH + STL_MAX_FILE_NAME_LENGTH];
    stlChar        sBackupObjectType;
    stlBool        sExist;
    stlTime        sBeginTime;

    sFlag = STL_FOPEN_LARGEFILE | STL_FOPEN_WRITE | STL_FOPEN_CREATE;

    sDatabaseIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_FILE_IO,
                                                 KNL_ENV(aEnv) );

    if( sDatabaseIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    /**
     * Incremental backup ����
     * 1. Begin Backup
     * 2. Backup datafile
     * 3. Controlfile�� backup list �߰��ϰ� controlfile backup
     * 4. End Backup
     */

    /**
     *  1. begin backup
     */
    STL_TRY( smfIncrementalBackupBegin( aTbsId,
                                        aBackupOption,
                                        aBackupLevel,
                                        &sPrevBackupLsn,
                                        &sBackupSeq,
                                        sFileName,
                                        sAbsFileName,
                                        &sBeginTime,
                                        aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     *  2. backup datafiles
     */
    STL_TRY( smgOpenFile( &sFile,
                          sAbsFileName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 2;
                              
    STL_TRY( smpBackupPages( &sFile,
                             sAbsFileName,
                             sPrevBackupLsn,
                             aTbsId,
                             &sBackupLsn,
                             &sBackupLid,
                             aEnv ) == STL_SUCCESS );

    sState = 1;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv ) == STL_SUCCESS );

    /**
     * 3. Controlfile�� Incremental backup list�� �߰��ϰ� Controlfile�� backup�Ѵ�.
     */
    sBackupObjectType = (aTbsId == SML_INVALID_TBS_ID )?
        SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_DATABASE :
        SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_TABLESPACE;

    STL_TRY( smfInsertBackupRecord( sFileName,
                                    sBeginTime,
                                    aTbsId,
                                    sBackupObjectType,
                                    aBackupOption,
                                    aBackupLevel,
                                    sBackupLsn,
                                    sBackupLid,
                                    aEnv ) == STL_SUCCESS );

    /**
     * 3.1 Database Backup�� ��� �߰��� controlfile�� backup�Ѵ�. 
     */
    if( aTbsId == SML_INVALID_TBS_ID )
    {
        STL_TRY( smfMakeBackupFileName( aTbsId,
                                        aBackupLevel,
                                        SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_CONTROLFILE,
                                        sBackupSeq,
                                        sFileName,
                                        sAbsFileName,
                                        &sBeginTime,
                                        aEnv ) == STL_SUCCESS );

        STL_TRY( stlExistFile( sAbsFileName,
                               &sExist,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        STL_TRY_THROW( sExist != STL_TRUE, RAMP_ERR_ALREADY_FILE_EXIST );

        STL_TRY( smfBackupCtrlFile( SML_BACKUP_TYPE_INCREMENTAL,
                                    sAbsFileName,
                                    aEnv ) == STL_SUCCESS );

        STL_TRY( smfInsertBackupRecord( sFileName,
                                        sBeginTime,
                                        aTbsId,
                                        SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_CONTROLFILE,
                                        aBackupOption,
                                        aBackupLevel,
                                        sBackupLsn,
                                        sBackupLid,
                                        aEnv ) == STL_SUCCESS );
    }
    
    /**
     *  4. end backup
     */
    sState = 0;
    STL_TRY( smfIncrementalBackupEnd( aTbsId,
                                      aBackupLevel,
                                      aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALREADY_FILE_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ALREADY_EXIST_FILE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sFileName );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smgCloseFile( &sFile, aEnv );
        case 1:
            (void)smfIncrementalBackupEnd( aTbsId, aBackupLevel, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smfFullBackup( stlChar    aCommand,
                         smlTbsId   aTbsId,
                         smlEnv   * aEnv )
{
    if( aTbsId == SML_INVALID_TBS_ID )
    {
        /**
         * full backup database
         */
        STL_TRY( smfBackupDatabase( SML_BACKUP_TYPE_FULL,
                                    aCommand,
                                    1,   /* don't care */
                                    SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * full backup tablespace
         */
        STL_TRY( smfBackupTablespace( SML_BACKUP_TYPE_FULL,
                                      aCommand,
                                      aTbsId,
                                      SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfIncrementalBackupBegin( smlTbsId    aTbsId,
                                     stlChar     aBackupOption,
                                     stlInt16    aBackupLevel,
                                     smrLsn    * aPrevBackupLsn,
                                     stlInt64  * aBackupSeq,
                                     stlChar   * aFileName,
                                     stlChar   * aAbsFileName,
                                     stlTime   * aBeginTime,
                                     smlEnv    * aEnv )
{
    stlInt64                sBackupSeq;
    smrLsn                  sPrevBackupLsn;
    smfIncBackupObjectType  sBackupObjectType;
    stlInt32                sState = 0;
    stlBool                 sExist;

    if( aTbsId == SML_INVALID_TBS_ID )
    {
        STL_TRY( smfBackupDatabase( SML_BACKUP_TYPE_INCREMENTAL,
                                    SML_BACKUP_COMMAND_BEGIN,
                                    aBackupLevel,
                                    aEnv )
                 == STL_SUCCESS );

        sBackupObjectType = SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_DATABASE;
    }
    else
    {
        STL_TRY( smfBackupTablespace( SML_BACKUP_TYPE_INCREMENTAL,
                                      SML_BACKUP_COMMAND_BEGIN,
                                      aTbsId,
                                      aEnv )
                 == STL_SUCCESS );

        sBackupObjectType = SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_TABLESPACE;
    }
    sState = 1;

    sPrevBackupLsn = smfGetPrevBackupLsn( aTbsId,
                                          aBackupOption,
                                          aBackupLevel );

    if( aBackupLevel != 0 )
    {
        STL_TRY_THROW( (sPrevBackupLsn != SMR_INVALID_LSN),
                       RAMP_ERR_NOT_EXIST_LEVEL_0_BACKUP );
    }

    sBackupSeq = smfGetBackupSequence( aTbsId );

    STL_TRY( smfMakeBackupFileName( aTbsId,
                                    aBackupLevel,
                                    sBackupObjectType,
                                    sBackupSeq,
                                    aFileName,
                                    aAbsFileName,
                                    aBeginTime,
                                    aEnv ) == STL_SUCCESS );

    STL_TRY( stlExistFile( aAbsFileName,
                           &sExist,
                           KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    STL_TRY_THROW( sExist != STL_TRUE, RAMP_ERR_ALREADY_FILE_EXIST );

    *aPrevBackupLsn = sPrevBackupLsn;
    *aBackupSeq = sBackupSeq;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_EXIST_LEVEL_0_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_EXIST_LEVEL_0_BACKUP,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_ALREADY_FILE_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ALREADY_EXIST_FILE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      aFileName );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        if( aTbsId == SML_INVALID_TBS_ID )
        {
            (void)smfBackupDatabase( SML_BACKUP_TYPE_INCREMENTAL,
                                     SML_BACKUP_COMMAND_END,
                                     1, /* don't care */
                                     aEnv );
        }
        else
        {
            (void)smfBackupTablespace( SML_BACKUP_TYPE_INCREMENTAL,
                                       SML_BACKUP_COMMAND_END,
                                       aTbsId,
                                       aEnv );
        }
    }

    return STL_FAILURE;
}

stlStatus smfIncrementalBackupEnd( smlTbsId    aTbsId,
                                   stlInt16    aBackupLevel,
                                   smlEnv    * aEnv )
{
    if( aTbsId == SML_INVALID_TBS_ID )
    {
        STL_TRY( smfBackupDatabase( SML_BACKUP_TYPE_INCREMENTAL,
                                    SML_BACKUP_COMMAND_END,
                                    aBackupLevel,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smfBackupTablespace( SML_BACKUP_TYPE_INCREMENTAL,
                                      SML_BACKUP_COMMAND_END,
                                      aTbsId,
                                      aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfGetBackupObjectName( smlTbsId                 aTbsId,
                                  smfIncBackupObjectType   aBackupObjectType,
                                  stlChar                * aString,
                                  smlEnv                 * aEnv )
{
    stlChar    sTbsName[STL_MAX_SQL_IDENTIFIER_LENGTH];

    aString[0] = '\0';

    switch( aBackupObjectType )
    {
        case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_DATABASE:
            stlStrcpy( aString, "database" );
            break;
        case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_TABLESPACE:
            STL_TRY( smfGetTbsName( aTbsId, sTbsName, aEnv ) == STL_SUCCESS );
            stlSnprintf( aString,
                         STL_MAX_SQL_IDENTIFIER_LENGTH,
                         "tablespace_%s",
                         sTbsName );
            break;
        case SMF_INCREMENTAL_BACKUP_OBJECT_TYPE_CONTROLFILE:
            stlStrcpy( aString, "control" );
            break;
        default:
            stlStrcpy( aString, "unknown" );
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfMakeBackupFileName( smlTbsId                 aTbsId,
                                 stlInt16                 aBackupLevel,
                                 smfIncBackupObjectType   aBackupObjectType,
                                 stlInt64                 aBackupSeq,
                                 stlChar                * aFileName,
                                 stlChar                * aAbsFileName,
                                 stlTime                * aBeginTime,
                                 smlEnv                 * aEnv )
{
    stlTime        sTime;
    stlExpTime     sExpTime;
    stlChar        sObjectString[STL_MAX_FILE_NAME_LENGTH];

    sTime = stlNow();
    stlMakeExpTimeByLocalTz( &sExpTime, sTime );

    STL_TRY( smfGetBackupObjectName( aTbsId,
                                     aBackupObjectType,
                                     sObjectString,
                                     aEnv ) == STL_SUCCESS );

    /**
     * Backup File Name ����.
     * : Level Year Month Day Hour Minute Second
     */
    stlSnprintf( aFileName,
                 STL_MAX_FILE_PATH_LENGTH,
                 "%sD%4d%02d%02dT%02d%02d%02dL%dS%ld.inc",
                 sObjectString,
                 sExpTime.mYear + 1900,
                 sExpTime.mMonth + 1,
                 sExpTime.mDay,
                 sExpTime.mHour,
                 sExpTime.mMinute,
                 sExpTime.mSecond,
                 aBackupLevel,
                 aBackupSeq );

    STL_TRY( smfGetAbsBackupFileName( aFileName,
                                      0,
                                      aAbsFileName,
                                      aEnv ) == STL_SUCCESS );

    *aBeginTime = sTime;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smfGetAbsBackupFileName( stlChar   * aFileName,
                                   stlInt32    aBackupDirIdx,
                                   stlChar   * aAbsFileName,
                                   smlEnv    * aEnv )
{
    stlChar    sBackupDir[KNL_PROPERTY_STRING_MAX_LENGTH];
    stlChar    sPropertyName[KNL_PROPERTY_STRING_MAX_LENGTH];
    stlInt32   sAbsFileNameLen;
    
    stlSnprintf( sPropertyName,
                 KNL_PROPERTY_STRING_MAX_LENGTH,
                 "BACKUP_DIR_%d",
                 aBackupDirIdx + 1 );

    STL_TRY( knlGetPropertyValueByName( sPropertyName,
                                        sBackupDir,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( stlMergeAbsFilePath( sBackupDir,
                                  aFileName,
                                  aAbsFileName,
                                  KNL_PROPERTY_STRING_MAX_LENGTH,
                                  &sAbsFileNameLen,
                                  KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
