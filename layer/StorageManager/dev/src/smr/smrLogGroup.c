/*******************************************************************************
 * smrLogGroup.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smrLogGroup.c 1012 2011-06-10 02:14:58Z leekmo $
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
#include <smrDef.h>
#include <smr.h>
#include <smf.h>
#include <sml.h>
#include <smrLogBuffer.h>
#include <smrLogFile.h>
#include <smrLogStream.h>
#include <smrLogGroup.h>
#include <smrLogMember.h>

/**
 * @file smrLogGroup.c
 * @brief Storage Manager Layer Recovery Log Group Internal Routines
 */

extern smrWarmupEntry * gSmrWarmupEntry;

/**
 * @addtogroup smr
 * @{
 */

stlStatus smrAddLogGroupEventHandler( void      * aData,
                                      stlUInt32   aDataSize,
                                      stlBool   * aDone,
                                      void      * aEnv )
{
    stlChar * sPtr = (stlChar*)aData;
    smlLogStreamAttr * sLogStreamAttr;
    smlLogMemberAttr * sPrevLogMemberAttr = NULL;
    stlInt32           sLogMemberCount;
    stlInt32           i;

    *aDone = STL_FALSE;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    sLogStreamAttr = (smlLogStreamAttr*)sPtr;
    sPtr += STL_ALIGN( STL_SIZEOF(smlLogStreamAttr), 8 );
    sLogStreamAttr->mLogGroupAttr = (smlLogGroupAttr*)sPtr;
    sPtr += STL_ALIGN( STL_SIZEOF(smlLogGroupAttr), 8 );
    stlMemcpy( &sLogMemberCount, sPtr, STL_SIZEOF(stlInt32) );
    sPtr += STL_ALIGN( STL_SIZEOF(stlInt32), 8 );

    for( i = 0; i < sLogMemberCount; i++ )
    {
        if( sPrevLogMemberAttr == NULL )
        {
            sLogStreamAttr->mLogGroupAttr->mLogMemberAttr = (smlLogMemberAttr*)sPtr;
        }
        else
        {
            sPrevLogMemberAttr->mNext = (smlLogMemberAttr*)sPtr;
        }
        sPrevLogMemberAttr = (smlLogMemberAttr*)sPtr;
        sPtr += STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 );
    }

    STL_TRY( smrAddLogGroup( sLogStreamAttr,
                             STL_FALSE,         /* aCreateDb */
                             NULL,              /* aMinLogFileSize */
                             aEnv ) == STL_SUCCESS );

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

    STL_FINISH;

    (void)smgExecutePendOp( SML_PEND_ACTION_ROLLBACK,
                            0, /* aStatementTcn */
                            aEnv );

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;

    return STL_FAILURE;
}


/**
 * @brief Log Group을 추가한다.
 * @param[in] aAttr Log Stream 속성
 * @param[in] aCreateDb Database Creation을 위한 Log Group 생성 여부
 * @param[out] aMinLogFileSize 가장 작은 Log file의 크기
 * @param[in,out] aEnv Environment
 */
stlStatus smrAddLogGroup( smlLogStreamAttr * aAttr,
                          stlBool            aCreateDb,
                          stlInt64         * aMinLogFileSize,
                          smlEnv           * aEnv )
{
    smlLogGroupAttr     * sLogGroupAttr;
    smlLogMemberAttr    * sLogMemberAttr;
    smrLogStream        * sLogStream;
    smrLogGroup         * sLogGroup;
    smrLogGroup         * sTmpLogGroup;
    smrLogMember        * sLogMember = NULL;
    smrLogMember        * sTmpLogMember;
    smrLogMember        * sNextLogMember;
    stlInt32              sLogState;
    stlInt32              sState = 0;
    smrPendAddLogGroup    sPendArgs;
    stlBool               sFound = STL_FALSE;
    stlBool               sIsEmptyGroup = STL_FALSE;
    stlBool               sIsReuse;

    STL_PARAM_VALIDATE( aAttr != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    STL_PARAM_VALIDATE( aAttr->mLogGroupAttr != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    if( aMinLogFileSize != NULL )
    {
        *aMinLogFileSize = STL_INT64_MAX;
    }

    stlMemset( &sPendArgs, 0x00, STL_SIZEOF( smrPendAddLogGroup ) );

    sLogGroupAttr = aAttr->mLogGroupAttr;
    sLogStream = &gSmrWarmupEntry->mLogStream;

    /*
     * 입력받은 Log Stream의 유효성 검사
     */
    sLogState = smrGetLogStreamState( sLogStream );

    STL_TRY_THROW( ( sLogState == SMR_LOG_STREAM_STATE_CREATING ) ||
                   ( sLogState == SMR_LOG_STREAM_STATE_ACTIVE ),
                   RAMP_ERR_INTERNAL_ERROR );

    while( sLogGroupAttr != NULL )
    {
        /*
         * 새로운 Log Group 할당
         */
        STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smrLogGroup ),
                                       (void**)&sLogGroup,
                                       aEnv )
                 == STL_SUCCESS );
        sState = 1;

        SMR_INIT_LOG_GROUP( sLogGroup );

        /*
         * Persistent Information 설정
         */
        sLogGroup->mLogGroupPersData.mState          = SMR_LOG_GROUP_STATE_UNUSED;
        sLogGroup->mLogGroupPersData.mLogGroupId     = sLogGroupAttr->mLogGroupId;
        sLogGroup->mLogGroupPersData.mLogMemberCount = 0;
        sLogGroup->mLogGroupPersData.mPrevLastLsn    = SMR_INVALID_LSN;
        sLogGroup->mCurBlockSeq                      = 0;
        sLogGroup->mLogSwitchingSbsn                 = -1;

        if( ( sLogGroupAttr->mValidFlags & SML_LOG_GROUP_FILESIZE_MASK ) == SML_LOG_GROUP_FILESIZE_YES )
        {
            sLogGroup->mLogGroupPersData.mFileSize = sLogGroupAttr->mFileSize;
        }
        else
        {
            sLogGroup->mLogGroupPersData.mFileSize = SMR_LOG_GROUP_DEFAULT_FIILESIZE;
        }

        /*
         * Log Group을 추가할 때 REUSE option은 Log Group의 모든 log member에 적용됨
         */
        if( (sLogGroupAttr->mValidFlags & SML_LOG_GROUP_REUSE_MASK) == SML_LOG_GROUP_REUSE_YES )
        {
            sIsReuse = sLogGroupAttr->mLogFileReuse;
        }
        else
        {
            sIsReuse = STL_FALSE;
        }

        /* Log Member 생성 */
        sLogMemberAttr = sLogGroupAttr->mLogMemberAttr;
        sLogMember = NULL;
        while( sLogMemberAttr != NULL )
        {
            sFound = STL_FALSE;
            STL_TRY( smrIsUsedLogFile( sLogMemberAttr->mName,
                                       &sFound,
                                       NULL,
                                       aEnv ) == STL_SUCCESS );

            STL_TRY_THROW( sFound == STL_FALSE, RAMP_ERR_EXISTS_LOGMEMBER );

            if( aCreateDb != STL_TRUE )
            {
                STL_TRY( smgAddPendOp( NULL,
                                       SMG_PEND_ADD_LOG_FILE,
                                       SML_PEND_ACTION_ROLLBACK,
                                       (void*)sLogMemberAttr,
                                       STL_SIZEOF( smlLogMemberAttr ),
                                       0,
                                       aEnv ) == STL_SUCCESS );

                /* database file manager에 log file name을 등록한다. */
                STL_TRY( knlAddDbFile( sLogMemberAttr->mName,
                                       KNL_DATABASE_FILE_TYPE_REDO_LOG,
                                       KNL_ENV(aEnv) ) == STL_SUCCESS );
            }

            STL_TRY( smrCreateLogMember( sLogMemberAttr,
                                         sLogGroup->mLogGroupPersData.mFileSize,
                                         sLogStream->mLogStreamPersData.mBlockSize,
                                         sIsReuse,
                                         &sLogMember,
                                         aEnv) == STL_SUCCESS );
            /*
             * 기존 Log Member 검사
             */
            sFound = STL_FALSE;
            STL_RING_FOREACH_ENTRY( &(sLogGroup->mLogMemberList), sTmpLogMember )
            {
                if( stlStrcmp( sTmpLogMember->mLogMemberPersData.mName,
                               sLogMemberAttr->mName ) == 0 )
                {
                    sFound = STL_TRUE;
                }
            }

            STL_TRY_THROW( sFound == STL_FALSE, RAMP_ERR_EXISTS_LOGMEMBER );

            STL_RING_ADD_LAST( &sLogGroup->mLogMemberList, sLogMember );

            sLogGroup->mLogGroupPersData.mLogMemberCount++;
            sLogMember = NULL;

            sLogMemberAttr = sLogMemberAttr->mNext;

            if( aCreateDb != STL_TRUE )
            {
                /* Database 생성 중이 아닌 경우 Add Log Group 수행 시
                 * EVENT가 취소되었는지 체크 */
                STL_TRY( knlCheckEnvEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
            }
        }

        /* Log Group을 Log Stream에 추가 */
        STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
        sState = 2;

        STL_TRY( smrFindLogGroupById( sLogStream,
                                      sLogGroupAttr->mLogGroupId,
                                      &sTmpLogGroup,
                                      aEnv ) == STL_SUCCESS );

        STL_TRY_THROW( sTmpLogGroup == NULL, RAMP_ERR_INVALID_LOGGROUP_NUMBER );

        if( STL_RING_IS_EMPTY( &(sLogStream->mLogGroupList) ) == STL_TRUE )
        {
            sIsEmptyGroup = STL_TRUE;

            sLogGroup->mLogGroupPersData.mFileSeqNo = sLogStream->mLogStreamPersData.mCurFileSeqNo;
            sLogGroup->mLogGroupPersData.mState = SMR_LOG_GROUP_STATE_CURRENT;

            STL_RING_ADD_LAST( &(sLogStream->mLogGroupList), sLogGroup );
        }
        else
        {
            sLogGroup->mLogGroupPersData.mFileSeqNo = SMR_INVALID_LOG_FILE_SEQ_NO;
            STL_TRY( smrFindLogGroupBySeqNo( sLogStream,
                                             sLogStream->mLogStreamPersData.mCurFileSeqNo,
                                             &sTmpLogGroup,
                                             aEnv ) == STL_SUCCESS );

            STL_ASSERT( sTmpLogGroup != NULL );

            if( sIsEmptyGroup == STL_TRUE )
            {
                STL_RING_ADD_LAST( &(sLogStream->mLogGroupList), sLogGroup );
            }
            else
            {
                STL_RING_INSERT_AFTER( &(sLogStream->mLogGroupList), sTmpLogGroup, sLogGroup );
            }
        }

        sLogStream->mLogStreamPersData.mLogGroupCount++;

        sState = 0;
        STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );


        if( aMinLogFileSize != NULL )
        {
            *aMinLogFileSize = (*aMinLogFileSize > sLogGroup->mLogGroupPersData.mFileSize) ?
                sLogGroup->mLogGroupPersData.mFileSize : *aMinLogFileSize;
        }

        /*
         * Log Group 생성 후 logfile header를 설정한다.
         */
        STL_TRY( smrWriteLogfileHdr( sLogStream,
                                     sLogGroup,
                                     NULL,
                                     aEnv ) == STL_SUCCESS );

        sPendArgs.mLogGroupId = sLogGroupAttr->mLogGroupId;
        STL_TRY( smgAddPendOp( NULL,
                               SMG_PEND_ADD_LOG_GROUP,
                               SML_PEND_ACTION_ROLLBACK,
                               (void*)&sPendArgs,
                               STL_SIZEOF( smrPendAddLogGroup ),
                               0,
                               aEnv ) == STL_SUCCESS );

        sLogGroupAttr = sLogGroupAttr->mNext;
    } /* while */

    STL_TRY( smgAddPendOp( NULL,
                           SMG_PEND_ADD_LOG_GROUP,
                           SML_PEND_ACTION_COMMIT,
                           NULL,
                           0,
                           0,
                           aEnv ) == STL_SUCCESS );


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_LOGGROUP_NUMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_LOGGROUP_NUMBER,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_EXISTS_LOGMEMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXISTS_LOGMEMBER,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sLogMemberAttr->mName );
    }

    STL_CATCH( RAMP_ERR_INTERNAL_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smrAddLogGroup()",
                      sState );
    }
 
    STL_FINISH;

    if( sLogMember != NULL )
    {
        (void)smrDestroyLogMember( sLogMember, aEnv );
    }
    switch( sState )
    {
        case 2:
            {
                (void)smfReleaseDatabaseFileLatch( aEnv );
            }
        case 1:
            {
                STL_RING_FOREACH_ENTRY_SAFE( &(sLogGroup->mLogMemberList),
                                                sLogMember,
                                                sNextLogMember )
                {
                    STL_RING_UNLINK( &sLogGroup->mLogMemberList, sLogMember );
                    (void)smrDestroyLogMember( sLogMember, aEnv );
                }
                (void)smgFreeShmMem4Mount( sLogGroup, aEnv );
            }
    }

    return STL_FAILURE;
}


stlStatus smrDoPendAddLogGroup( stlUInt32   aActionFlag,
                                smgPendOp * aPendOp,
                                smlEnv    * aEnv )
{
    smrLogStream       * sLogStream = &gSmrWarmupEntry->mLogStream;
    smrPendAddLogGroup * sPendArgs = (smrPendAddLogGroup*)aPendOp->mArgs;
    smrLogGroup        * sLogGroup;
    stlInt32             sState = 0;
    smrLogMember       * sLogMember;
    smrLogMember       * sNextLogMember;

    if( aActionFlag == SML_PEND_ACTION_COMMIT )
    {
        STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( smrFindLogGroupById( sLogStream,
                                      sPendArgs->mLogGroupId,
                                      &sLogGroup,
                                      aEnv ) == STL_SUCCESS );
        if( sLogGroup != NULL )
        {
            /* 모든 log member들을 destroy 한다 */
            STL_RING_FOREACH_ENTRY_SAFE( &(sLogGroup->mLogMemberList),
                                         sLogMember,
                                         sNextLogMember )
            {
                STL_RING_UNLINK( &sLogGroup->mLogMemberList, sLogMember );
                (void)smrDestroyLogMember( sLogMember, aEnv );
            }

            /* log group을 제거한다 */
            STL_RING_UNLINK( &sLogStream->mLogGroupList, sLogGroup );

            STL_TRY( smgFreeShmMem4Mount( sLogGroup, aEnv ) == STL_SUCCESS );
        } /* if */

        sState = 0;
        STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseDatabaseFileLatch( aEnv );
    }
    return STL_FAILURE;
}

stlStatus smrDoPendAddLogFile( stlUInt32   aActionFlag,
                               smgPendOp * aPendOp,
                               smlEnv    * aEnv )
{
    smlLogMemberAttr    * sPendArgs = (smlLogMemberAttr *)aPendOp->mArgs;
    stlBool               sIsRemoved;

    STL_DASSERT( aActionFlag != SML_PEND_ACTION_COMMIT );

    STL_TRY( knlRemoveDbFile( sPendArgs->mName,
                              &sIsRemoved,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrDropLogGroupEventHandler( void      * aData,
                                       stlUInt32   aDataSize,
                                       stlBool   * aDone,
                                       void      * aEnv )
{
    smlLogStreamAttr * sLogStreamAttr = (smlLogStreamAttr*)aData;

    *aDone = STL_FALSE;

    sLogStreamAttr->mLogGroupAttr = (smlLogGroupAttr*)STL_ALIGN(((stlInt64)(sLogStreamAttr + 1)), 8);

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    STL_TRY( smrDropLogGroup( sLogStreamAttr, aEnv ) == STL_SUCCESS );
    
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

    STL_FINISH;

    (void)smgExecutePendOp( SML_PEND_ACTION_ROLLBACK,
                            0, /* aStatementTcn */
                            aEnv );

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    *aDone = STL_TRUE;

    return STL_FAILURE;
}

stlStatus smrDropLogGroup( smlLogStreamAttr * aAttr,
                           smlEnv           * aEnv )
{
    stlInt32               sState = 0;
    smrLogStream         * sLogStream = &gSmrWarmupEntry->mLogStream;
    smlLogGroupAttr      * sGroupAttr = aAttr->mLogGroupAttr;
    smrLogGroup          * sLogGroup;
    smrPendDropLogGroup    sPendArgs;
    stlInt32               sValidGroupCount;

    STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    sState = 1;

    sValidGroupCount = 0;
    STL_RING_FOREACH_ENTRY( &sLogStream->mLogGroupList, sLogGroup )
    {
        if( (sLogGroup->mLogGroupPersData.mState != SMR_LOG_GROUP_STATE_CREATING) &&
            (sLogGroup->mLogGroupPersData.mState != SMR_LOG_GROUP_STATE_DROPPING) )
        {
            sValidGroupCount++;
        }
    }
    STL_TRY_THROW( sValidGroupCount - 1 >= SML_MINIMUM_LOG_GROUP_COUNT, RAMP_ERR_MINIMUM_LOGGROUP_COUNT );

    STL_TRY( smrFindLogGroupById( sLogStream,
                                  sGroupAttr->mLogGroupId,
                                  &sLogGroup,
                                  aEnv ) == STL_SUCCESS );
    STL_TRY_THROW( sLogGroup != NULL, RAMP_ERR_INVALID_LOGGROUP_NUMBER );

    STL_TRY_THROW( (sLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_UNUSED) ||
                   (sLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_INACTIVE),
                   RAMP_ERR_INVALID_LOGGROUP_STATE );

    sPendArgs.mPrevGroupState = sLogGroup->mLogGroupPersData.mState;
    
    sLogGroup->mLogGroupPersData.mState = SMR_LOG_GROUP_STATE_DROPPING;

    sState = 0;
    STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );

    sPendArgs.mLogGroupId = sGroupAttr->mLogGroupId;
    STL_TRY( smgAddPendOp( NULL,
                           SMG_PEND_DROP_LOG_GROUP,
                           SML_PEND_ACTION_COMMIT | SML_PEND_ACTION_ROLLBACK,
                           (void*)&sPendArgs,
                           STL_SIZEOF( smrPendDropLogGroup ),
                           0,
                           aEnv ) == STL_SUCCESS );


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_LOGGROUP_NUMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_LOGGROUP_NUMBER,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_INVALID_LOGGROUP_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_LOGGROUP_STATE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sGroupAttr->mLogGroupId );
    }

    STL_CATCH( RAMP_ERR_MINIMUM_LOGGROUP_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_MINIMUM_LOGGROUP_COUNT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      SML_MINIMUM_LOG_GROUP_COUNT );
    }


    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseDatabaseFileLatch( aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrDoPendDropLogGroup( stlUInt32   aActionFlag,
                                 smgPendOp * aPendOp,
                                 smlEnv    * aEnv )
{
    smrLogStream       * sLogStream = &gSmrWarmupEntry->mLogStream;
    smrPendDropLogGroup * sPendArgs = (smrPendDropLogGroup*)aPendOp->mArgs;
    smrLogGroup        * sLogGroup;
    smrLogMember       * sLogMember;
    smrLogMember       * sNextLogMember;
    stlInt32             sState = 0;

    if( aActionFlag == SML_PEND_ACTION_COMMIT )
    {
        STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( smrFindLogGroupById( sLogStream,
                                      sPendArgs->mLogGroupId,
                                      &sLogGroup,
                                      aEnv ) == STL_SUCCESS );

        if( sLogGroup != NULL )
        {
            /* 모든 log member들을 destroy 한다 */
            STL_RING_FOREACH_ENTRY_SAFE( &(sLogGroup->mLogMemberList),
                                            sLogMember,
                                            sNextLogMember )
            {
                STL_RING_UNLINK( &sLogGroup->mLogMemberList, sLogMember );
                (void)smrDestroyLogMember( sLogMember, aEnv );
            }

            /* log group을 삭제한다 */
            STL_RING_UNLINK( &sLogStream->mLogGroupList, sLogGroup );

            STL_TRY( smgFreeShmMem4Mount( sLogGroup, aEnv ) == STL_SUCCESS );

            sLogStream->mLogStreamPersData.mLogGroupCount--;
        }

        sState = 0;
        STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );

        STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( smrFindLogGroupById( sLogStream,
                                      sPendArgs->mLogGroupId,
                                      &sLogGroup,
                                      aEnv ) == STL_SUCCESS );

        if( sLogGroup != NULL )
        {
            sLogGroup->mLogGroupPersData.mState = sPendArgs->mPrevGroupState;
        }

        sState = 0;
        STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseDatabaseFileLatch( aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrSwitchLogGroupEventHandler( void      * aData,
                                         stlUInt32   aDataSize,
                                         stlBool   * aDone,
                                         void      * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    STL_TRY( smrSwitchLogGroup( SML_ENV(aEnv) ) == STL_SUCCESS );

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

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    *aDone = STL_TRUE;

    return STL_FAILURE;
}

stlStatus smrSwitchLogGroup( smlEnv * aEnv )
{
    if( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( smrSwitchLogGroupAtOpen( aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smrSwitchLogGroupAtMount( aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smrSwitchLogGroupAtMount( smlEnv * aEnv )
{
    smrLogStream   * sLogStream;
    smrLogGroup    * sLogGroup = NULL;
    smrLogGroup    * sNextLogGroup = NULL;
    smrLsn           sBlockLsn;
    smrLogBlockHdr   sInitLogBlockHdr;
    stlInt16         sBlockSize;
    stlInt32         sBlockSeq = 0;
    stlChar        * sLogBuffer;
    stlChar        * sLogBlock;
    stlInt32         i;
    smrLsn           sFirstLsn;
    stlFile          sFile;
    stlOffset        sOffset;
    knlRegionMark    sMemMark;
    stlInt32         sState = 0;
    smrLsn           sPrevLastLsn = SMR_INVALID_LSN;
    stlInt64         sMaxBlockCount;
    smrLogMember   * sLogMember;
    stlInt64         sReadBlockCount;
    stlInt32         sFlag = 0;
    stlInt64         sLogIo;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    sBlockSize = sLogStream->mLogStreamPersData.mBlockSize;

    /**
     * find current logfile group
     */
    
    STL_RING_FOREACH_ENTRY( &sLogStream->mLogGroupList, sLogGroup )
    {
        if( sLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_CURRENT )
        {
            break;
        }
    }

    STL_DASSERT( sLogGroup != NULL );

    sNextLogGroup = smrGetNextLogGroup( sLogStream, sLogGroup );
    
    STL_TRY_THROW( SMR_IS_USABLE_LOG_FILE( sNextLogGroup ) == STL_TRUE, RAMP_ERR_ACTIVE );

    sFirstLsn = sLogGroup->mLogGroupPersData.mPrevLastLsn + 1;

    /**
     * find lastest log block
     */
    
    KNL_INIT_REGION_MARK( &sMemMark );

    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                sBlockSize * (SMR_BULK_IO_BLOCK_UNIT_COUNT + 1 ),
                                (void**)&sLogBuffer,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    sLogBlock = (stlChar*)STL_ALIGN( (stlUInt64)sLogBuffer,
                                     sBlockSize );
    stlMemset( sLogBlock, 0x00, sBlockSize * SMR_BULK_IO_BLOCK_UNIT_COUNT );

    sMaxBlockCount = sLogGroup->mLogGroupPersData.mFileSize / sBlockSize;
    sLogMember = STL_RING_GET_FIRST_DATA( &( sLogGroup->mLogMemberList ) );

    sFlag = (STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_LARGEFILE);
    sLogIo = knlGetPropertyBigIntValueByID( KNL_PROPERTY_LOG_FILE_IO,
                                            KNL_ENV(aEnv) );
        
    if( sLogIo == 0 )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    STL_TRY( smgOpenFile( &sFile,
                          sLogMember->mLogMemberPersData.mName,
                          sFlag,
                          STL_FPERM_OS_DEFAULT,
                          aEnv )
             == STL_SUCCESS );
    sState = 2;

    sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( 0, sBlockSize );

    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    
    STL_TRY( stlReadFile( &sFile,
                          (void*)sLogBlock,
                          sBlockSize,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

    sBlockLsn = SMR_READ_BLOCK_LSN( sLogBlock );
    STL_TRY_THROW( sBlockLsn >= sFirstLsn, RAMP_ERR_EMPTY_LOGFILE );
    STL_TRY_THROW( sBlockLsn != SMR_INIT_LSN, RAMP_ERR_EMPTY_LOGFILE );
    
    while( sBlockSeq < sMaxBlockCount - 1 )
    {
        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, sBlockSize );

        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        if( ( sMaxBlockCount - sBlockSeq ) >= SMR_BULK_IO_BLOCK_UNIT_COUNT )
        {
            sReadBlockCount = SMR_BULK_IO_BLOCK_UNIT_COUNT;
        }
        else
        {
            sReadBlockCount = sMaxBlockCount - sBlockSeq;
        }

        STL_TRY( stlReadFile( &sFile,
                              (void*)sLogBlock,
                              sBlockSize * sReadBlockCount,
                              NULL,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        for( i = 0; i < sReadBlockCount; i++ )
        {
            sBlockLsn = SMR_READ_BLOCK_LSN( sLogBlock + (sBlockSize * i) );
            
            if( sBlockLsn < sFirstLsn )
            {
                STL_THROW( RAMP_DO_INSERT );
                    
            }
            
            if( sBlockLsn == SMR_INIT_LSN )
            {
                STL_THROW( RAMP_DO_INSERT );
            }
            
            sPrevLastLsn = sBlockLsn;
            sBlockSeq++;
        }
    }

    STL_RAMP( RAMP_DO_INSERT );

    if( sBlockSeq < sMaxBlockCount )
    {
        sInitLogBlockHdr.mLsn = SMR_INVALID_LSN;
        sInitLogBlockHdr.mLogCount = 0;
        SMR_SET_BLOCK_INFO( &sInitLogBlockHdr,
                            SMR_FILE_END_TRUE,
                            SMR_CHAINED_BLOCK_FALSE,
                            SMR_INVALID_WRAP_NO,
                            SMR_BLOCK_WRITE_FINISH );
        sInitLogBlockHdr.mFirstLogOffset = 0;
        SMR_WRITE_LOG_BLOCK_HDR( sLogBlock, &sInitLogBlockHdr );

        sOffset = SMR_BLOCK_SEQ_FILE_OFFSET( sBlockSeq, sBlockSize );

        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
        
        STL_TRY( stlWriteFile( &sFile,
                               sLogBlock,
                               sBlockSize,
                               NULL,
                               KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    }

    STL_TRY( smrLogFileSwitchingForce( sLogStream,
                                       sLogGroup,
                                       sPrevLastLsn,
                                       aEnv )
             == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( smgCloseFile( &sFile,
                           aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_TRUE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EMPTY_LOGFILE )
    {
        sLogMember = STL_RING_GET_FIRST_DATA( &( sLogGroup->mLogMemberList ) );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EMPTY_LOGFILE_GROUP,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sLogMember->mLogMemberPersData.mName );
    }

    STL_CATCH( RAMP_ERR_ACTIVE )
    {
        sLogMember = STL_RING_GET_FIRST_DATA( &( sNextLogGroup->mLogMemberList ) );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NEXT_LOGGROUP_ACTIVE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sLogMember->mLogMemberPersData.mName );
    }

    STL_FINISH;
    
    switch( sState )
    {
        case 2 :
            (void)smgCloseFile( &sFile, aEnv );
        case 1 :
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_TRUE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default :
            break;
    }

    return STL_FAILURE;
}

stlStatus smrSwitchLogGroupAtOpen( smlEnv * aEnv )
{
    smrLogBuffer * sLogBuffer;
    smrLogStream * sLogStream;
    stlBool        sTimeout;
    stlInt32       sState = 0;
    smrLogGroup  * sLogGroup = NULL;
    smrLogGroup  * sNextLogGroup = NULL;
    smrLogMember * sLogMember;
    
    sLogStream = &gSmrWarmupEntry->mLogStream;
    sLogBuffer = sLogStream->mLogBuffer;

    STL_TRY( smrStopLogFlushing( aEnv ) == STL_SUCCESS );
    sState = 1;
    
    /**
     * find current logfile group
     */
    
    STL_RING_FOREACH_ENTRY( &sLogStream->mLogGroupList, sLogGroup )
    {
        if( sLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_CURRENT )
        {
            break;
        }
    }

    STL_DASSERT( sLogGroup != NULL );

    STL_TRY( knlAcquireLatch( &sLogBuffer->mBufferLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sTimeout,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );
    sState = 2;

    sNextLogGroup = smrGetNextLogGroup( sLogStream, sLogGroup );
    
    /**
     * check activity of next two log groups
     * that means, at least next two log groups should be usable to SWITCH LOGFILE
     */
    
    STL_TRY_THROW( SMR_IS_USABLE_LOG_FILE( sNextLogGroup ) == STL_TRUE,
                   RAMP_ERR_ACTIVE );

    sNextLogGroup = smrGetNextLogGroup( sLogStream, sNextLogGroup );
    
    STL_TRY_THROW( SMR_IS_USABLE_LOG_FILE( sNextLogGroup ) == STL_TRUE,
                   RAMP_ERR_ACTIVE );

    /**
     * check empty log group
     */
    
    STL_TRY_THROW( sLogStream->mCurLogGroup4Buffer->mLogSwitchingSbsn != sLogBuffer->mRearSbsn,
                   RAMP_ERR_EMPTY_LOGFILE );

    /**
     * switching log group
     */
    
    if( sLogBuffer->mBlockOffset > SMR_BLOCK_HDR_SIZE )
    {
        sLogStream->mCurLogGroup4Buffer->mLogSwitchingSbsn = sLogBuffer->mRearSbsn + 1;
        SMR_BUFFER_BLOCK_SWITCH( sLogBuffer );
    }
    else
    {
        sLogStream->mCurLogGroup4Buffer->mLogSwitchingSbsn = sLogBuffer->mRearSbsn;
    }
        
    sLogBuffer->mFileSeqNo += 1;
    sLogBuffer->mRearFileBlockSeqNo = 0;
    sLogStream->mCurLogGroup4Buffer = smrGetNextLogGroup( sLogStream,
                                                          sLogStream->mCurLogGroup4Buffer );
    

    sState = 1;
    STL_TRY( knlReleaseLatch( &sLogBuffer->mBufferLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV(aEnv),
                        KNL_LOG_LEVEL_INFO,
                        "[DATABASE] switch logfile\n" )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smrStartLogFlushing( aEnv ) == STL_SUCCESS );

    /**
     * Dummy Log를 기록한다.
     * - Switching Sbsn 으로 설정된 Block이 로그파일로 내려감을 보장하기 위함.
     */

    STL_TRY( smrWriteDummyLog( aEnv ) == STL_SUCCESS );

    /**
     * Switching을 유발시킴
     */

    STL_TRY( smrFlushAllLogs( STL_FALSE /* aForceDiskIo */,
                              aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EMPTY_LOGFILE )
    {
        sLogMember = STL_RING_GET_FIRST_DATA( &( sLogGroup->mLogMemberList ) );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EMPTY_LOGFILE_GROUP,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sLogMember->mLogMemberPersData.mName );
    }

    STL_CATCH( RAMP_ERR_ACTIVE )
    {
        sLogMember = STL_RING_GET_FIRST_DATA( &( sNextLogGroup->mLogMemberList ) );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NEXT_LOGGROUP_ACTIVE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sLogMember->mLogMemberPersData.mName );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            knlReleaseLatch( &sLogBuffer->mBufferLatch, KNL_ENV(aEnv) );
        case 1:
            smrStartLogFlushing( aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smrFirstLogGroupPersData( void                 * aLogStreamIterator,
                                    smrLogGroupPersData  * aLogGroupPersData,
                                    void                ** aLogGroupIterator,
                                    smlEnv               * aEnv )
{
    smrLogStream * sLogStream;
    smrLogGroup  * sLogGroup;

    sLogStream = (smrLogStream *)aLogStreamIterator;

    if( STL_RING_IS_EMPTY( &(sLogStream->mLogGroupList) ) == STL_TRUE )
    {
        *aLogGroupIterator = NULL;
    }
    else
    {
        sLogGroup = STL_RING_GET_FIRST_DATA( &(sLogStream->mLogGroupList) );
        *aLogGroupPersData = sLogGroup->mLogGroupPersData;
        *aLogGroupIterator = (void*)sLogGroup;
    }

    return STL_SUCCESS;
}

stlStatus smrNextLogGroupPersData( void                 * aLogStreamIterator,
                                   smrLogGroupPersData  * aLogGroupPersData,
                                   void                ** aLogGroupIterator,
                                   smlEnv               * aEnv )
{
    smrLogStream * sLogStream;
    smrLogGroup  * sLogGroup;

    sLogStream = (smrLogStream *)aLogStreamIterator;
    sLogGroup = STL_RING_GET_NEXT_DATA( &(sLogStream->mLogGroupList),
                                        *aLogGroupIterator);

    if( STL_RING_IS_HEADLINK( &(sLogStream->mLogGroupList),
                              sLogGroup ) == STL_TRUE )
    {
        *aLogGroupIterator = NULL;
    }
    else
    {
        *aLogGroupPersData = sLogGroup->mLogGroupPersData;
        *aLogGroupIterator = (void*)sLogGroup;
    }

    return STL_SUCCESS;
}

/**
 * @brief 주어진 Lsn을 포함한 Log group과 FileSeqNo를 반환한다.
 * @param[in] aLogStream Log Stream 포인터
 * @param[in] aLsn 주어진 Log Lsn
 * @param[out] aFileSeqNo aLsn을 포함하는 Log group의 FileSeqNo
 * @param[out] aLogGroup aLsn을 포함하는 Log group 포인터
 */
stlStatus smrFindLogGroupByLsn( smrLogStream  * aLogStream,
                                smrLsn          aLsn,
                                stlInt64      * aFileSeqNo,
                                smrLogGroup  ** aLogGroup )
{
    smrLogGroup  * sNextLogGroup;
    smrLogGroup  * sLogGroup = NULL;
    smrLsn         sMaxPrevLsn = SMR_INIT_LSN;

    *aFileSeqNo = SMR_INVALID_LOG_FILE_SEQ_NO;
    *aLogGroup = NULL;

    if( aLsn != SMR_INVALID_LSN )
    {
        /**
         * Online Log Group들의 PrevLastLsn 중 최대값을 구한다.
         */
        STL_RING_FOREACH_ENTRY( &aLogStream->mLogGroupList, sNextLogGroup )
        {
            if( SMR_IS_ONLINE_LOG_GROUP( sNextLogGroup ) )
            {
                if( sMaxPrevLsn == SMR_INIT_LSN )
                {
                    sMaxPrevLsn = sNextLogGroup->mLogGroupPersData.mPrevLastLsn;
                    sLogGroup = sNextLogGroup;
                }
                else
                {
                    if( sNextLogGroup->mLogGroupPersData.mPrevLastLsn > sMaxPrevLsn )
                    {
                        sMaxPrevLsn = sNextLogGroup->mLogGroupPersData.mPrevLastLsn;
                        sLogGroup = sNextLogGroup;
                    }
                }
            }
        }

        STL_TRY( sLogGroup != NULL );
        sNextLogGroup = sLogGroup;

        /**
         * 최대 PrevLastLsn을 가진 Log Group부터 역 scan하여 aLsn보다 작은
         * 최대 PrevLastLsn을 가진 Log Group을 찾는다.
         */
        while( 1 )
        {
            if( sNextLogGroup->mLogGroupPersData.mPrevLastLsn < aLsn )
            {
                *aLogGroup = sNextLogGroup;
                *aFileSeqNo = sNextLogGroup->mLogGroupPersData.mFileSeqNo;
                break;
            }

            sNextLogGroup = smrGetPrevLogGroup( aLogStream, sNextLogGroup );

            if( sNextLogGroup == sLogGroup )
            {
                break;
            }
        }
    }

    /**
     * aLsn을 포함한 Logfile이 archive되었을 경우 Log group이 없다.
     */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrFindLogGroupBySeqNo( smrLogStream  * aLogStream,
                                  stlInt64        aFileSeqNo,
                                  smrLogGroup  ** aLogGroup,
                                  smlEnv        * aEnv )
{
    smrLogGroup * sLogGroup;

    *aLogGroup = NULL;
        
    STL_RING_FOREACH_ENTRY( &aLogStream->mLogGroupList, sLogGroup )
    {
        if( sLogGroup->mLogGroupPersData.mFileSeqNo == aFileSeqNo )
        {
            *aLogGroup = sLogGroup;
            break;
        }
    }

    /* FileSeqNo로 찾을 경우에는 없을 수도 있음(Media Recovery시) */
    
    return STL_SUCCESS;
}

stlStatus smrFindLogGroupById( smrLogStream  * aLogStream,
                               stlInt64        aLogGroupId,
                               smrLogGroup  ** aLogGroup,
                               smlEnv        * aEnv )
{
    smrLogGroup * sLogGroup;

    *aLogGroup = NULL;
        
    STL_RING_FOREACH_ENTRY( &aLogStream->mLogGroupList, sLogGroup )
    {
        if( sLogGroup->mLogGroupPersData.mLogGroupId == aLogGroupId )
        {
            *aLogGroup = sLogGroup;
            break;
        }
    }

    /* LogGroupId로 찾을 경우에는 없을 수도 있음 */

    return STL_SUCCESS;
}

/**
 * @brief File Name을 이용하여 Log Stream에 해당 File이 존재하는지 확인하고 Group Id를 return한다.
 * @param [in]  aFileName       Log File Name
 * @param [out] aLogGroupExist  FileName이 속한 Group의 존재 여부
 * @param [out] aLogGroupId     FileName이 속한 Group의 Id
 * @param [in]  aEnv            environment
 */ 
stlStatus smrFindLogGroupIdByFileName( stlChar       * aFileName,
                                       stlBool       * aLogGroupExist,
                                       stlInt16      * aLogGroupId,
                                       smlEnv        * aEnv )
{
    smrLogStream * sLogStream;
    smrLogGroup  * sLogGroup;
    smrLogMember * sLogMember;
    
    *aLogGroupExist = STL_FALSE;

    sLogStream = &gSmrWarmupEntry->mLogStream;
    
    STL_RING_FOREACH_ENTRY( &sLogStream->mLogGroupList, sLogGroup )
    {
        STL_RING_FOREACH_ENTRY( &sLogGroup->mLogMemberList, sLogMember )
        {
            if( stlStrcmp( sLogMember->mLogMemberPersData.mName,
                           aFileName ) == 0 )
            {
                *aLogGroupExist = STL_TRUE;
                *aLogGroupId    = sLogGroup->mLogGroupPersData.mLogGroupId;
                break;
            }
        }
    }

    return STL_SUCCESS;
}

/**
 * @brief Log Stream의 상태를 반환한다.
 * @param[in] aLogGroup Log Group 포인터
 */
inline stlInt32 smrGetLogGroupState( smrLogGroup * aLogGroup )
{
    return aLogGroup->mLogGroupPersData.mState;
}

/**
 * @brief 주어진 LogGroup의 다음 로그 Group을 반환한다.
 * @param[in] aLogStream Log Stream 포인터
 * @param[in] aLogGroup 현재 Log Group 포인터
 */
smrLogGroup * smrGetNextLogGroup( smrLogStream * aLogStream,
                                  smrLogGroup  * aLogGroup )
{
    smrLogGroup * sLogGroup;
    
    sLogGroup = STL_RING_GET_NEXT_DATA( &aLogStream->mLogGroupList,
                                        aLogGroup );
    if( STL_RING_IS_HEADLINK( &aLogStream->mLogGroupList,
                              sLogGroup ) == STL_TRUE )
    {
        sLogGroup = STL_RING_GET_FIRST_DATA( &aLogStream->mLogGroupList );
    }

    return sLogGroup;
}

/**
 * @brief 주어진 LogGroup의 이전 로그 Group을 반환한다.
 * @param[in] aLogStream Log Stream 포인터
 * @param[in] aLogGroup 현재 Log Group 포인터
 */
smrLogGroup * smrGetPrevLogGroup( smrLogStream * aLogStream,
                                  smrLogGroup  * aLogGroup )
{
    smrLogGroup * sLogGroup;
    
    sLogGroup = STL_RING_GET_PREV_DATA( &aLogStream->mLogGroupList,
                                        aLogGroup );
    if( STL_RING_IS_HEADLINK( &aLogStream->mLogGroupList,
                              sLogGroup ) == STL_TRUE )
    {
        sLogGroup = STL_RING_GET_LAST_DATA( &aLogStream->mLogGroupList );
    }

    return sLogGroup;
}


stlStatus smrCheckLogGroupId( stlInt32   aLogGroupId,
                              stlBool  * aFound,
                              smlEnv   * aEnv )
{
    stlInt32       sState = 0;
    smrLogGroup  * sLogGroup = NULL;
    smrLogStream * sLogStream = &gSmrWarmupEntry->mLogStream;

    *aFound = STL_FALSE;

    STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smrFindLogGroupById( sLogStream,
                                  aLogGroupId,
                                  &sLogGroup,
                                  aEnv ) == STL_SUCCESS );

    if( sLogGroup != NULL )
    {
        *aFound = STL_TRUE;
    }

    sState = 0;
    STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseDatabaseFileLatch( aEnv );
    }

    return STL_FAILURE;
}

/** @} */
