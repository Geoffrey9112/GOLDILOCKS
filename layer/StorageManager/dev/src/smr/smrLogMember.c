/*******************************************************************************
 * smrLogMember.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smrLogStream.c 1012 2011-06-10 02:14:58Z leekmo $
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
#include <smf.h>
#include <smrDef.h>
#include <smr.h>
#include <sml.h>
#include <smrLogFile.h>
#include <smrLogStream.h>
#include <smrLogGroup.h>
#include <smrLogMember.h>

/**
 * @file smrLogStream.c
 * @brief Storage Manager Layer Recovery Log Stream Internal Routines
 */

extern smrWarmupEntry * gSmrWarmupEntry;

/**
 * @addtogroup smr
 * @{
 */

stlStatus smrAddLogMemberEventHandler( void      * aData,
                                       stlUInt32   aDataSize,
                                       stlBool   * aDone,
                                       void      * aEnv )
{
    stlChar          * sPtr = (stlChar*) aData;
    smlLogGroupAttr  * sLogGroupAttr;
    smlLogMemberAttr * sLogMemberAttr = NULL;
    stlInt32           sMemberCount;
    stlInt32           i;
    stlBool            sGroupExist = STL_FALSE;
    stlBool            sFileExist = STL_FALSE;

    *aDone = STL_FALSE;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smlGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    /**
     * Data Unmarshalling
     *  smlLogGroupAttr
     *  MemberCount
     *    smlLogMemberAttr [smlLogMemberAttr] ...
     */ 
    sLogGroupAttr = (smlLogGroupAttr*)sPtr;
    sPtr += STL_ALIGN( STL_SIZEOF(smlLogGroupAttr), 8 );
    
    stlMemcpy( &sMemberCount, sPtr, STL_SIZEOF(stlInt32) );
    sPtr += STL_ALIGN( STL_SIZEOF(stlInt32), 8 );

    for( i = 0; i < sMemberCount; i++ )
    {
        if( sLogMemberAttr == NULL )
        {
            sLogGroupAttr->mLogMemberAttr = (smlLogMemberAttr*)sPtr;
        }
        else
        {
            sLogMemberAttr->mNext = (smlLogMemberAttr*)sPtr;
        }
        sLogMemberAttr = (smlLogMemberAttr*)sPtr;
        sPtr += STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 );
    }

    /**
     * Validation 
     */

    sLogMemberAttr = sLogGroupAttr->mLogMemberAttr;
    while( sLogMemberAttr != NULL )
    {
        /* Group Id*/
        STL_TRY( smrCheckLogGroupId( sLogGroupAttr->mLogGroupId,
                                     &sGroupExist,
                                     aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sGroupExist == STL_TRUE, RAMP_ERR_NOT_EXIST_LOGGROUP_ID );

        /* LogStream 에 사용 확인 */
        STL_TRY( smrIsUsedLogFile( sLogMemberAttr->mName,
                                   &sFileExist,
                                   NULL, /* aGroupId*/
                                   aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sFileExist != STL_TRUE, RAMP_ERR_USED_LOGFILE );
        
        /* File Exist 확인 */
        if( sLogMemberAttr->mLogFileReuse == STL_FALSE )
        {
            STL_TRY( stlExistFile( sLogMemberAttr->mName,
                                   &sFileExist,
                                   KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sFileExist != STL_TRUE, RAMP_ERR_EXIST_LOGFILE );
        }
        
        sLogMemberAttr = sLogMemberAttr->mNext;
    }
    
    STL_TRY( smrAddLogMember( sLogGroupAttr,
                              sMemberCount,
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

    STL_CATCH( RAMP_ERR_NOT_EXIST_LOGGROUP_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_EXIST_LOGGROUP_ID,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sLogGroupAttr->mLogGroupId );
    }
    
    STL_CATCH( RAMP_ERR_USED_LOGFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ALREADY_USED_LOG_MEMBER,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sLogMemberAttr->mName );
    }
    
    STL_CATCH( RAMP_ERR_EXIST_LOGFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ALREADY_EXIST_FILE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sLogMemberAttr->mName );
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
 * @brief Log Member를 추가한다.
 * @param[in] aGroupAttr Log Group 속성
 * @param[in] aMemberCount Log Member Count
 * @param[in,out] aEnv Environment
 */
stlStatus smrAddLogMember( smlLogGroupAttr  * aGroupAttr,
                           stlInt32           aMemberCount, 
                           smlEnv           * aEnv )
{
    smrLogStream        * sLogStream;
    smrLogGroup         * sLogGroup;
    smrLogMember        * sLogMember = NULL;
    smrLogMember        * sTmpLogMember;
    smlLogMemberAttr    * sLogMemberAttr;
    stlInt32              sLogState;
    stlBool               sFound = STL_FALSE;
    smrPendAddLogMember   sPendArgs;
    stlBool               sLatched = STL_FALSE;
    stlBool               sCreated = STL_FALSE;
    stlInt64              sFileSize;
    stlInt32              sCurrCount;

    STL_PARAM_VALIDATE( aGroupAttr != NULL, KNL_ERROR_STACK( aEnv ) );
    
    stlMemset( &sPendArgs, 0x00, STL_SIZEOF( smrPendAddLogMember ) );
    sPendArgs.mLogGroupId = aGroupAttr->mLogGroupId;

    sLogStream = &gSmrWarmupEntry->mLogStream;

    /* Log Group의 File Size를 미리 구함 */
    STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    sLatched = STL_TRUE;
    /*
     * Log Group Number 범위 검사
     */
    STL_TRY( smrFindLogGroupById( sLogStream,
                                  aGroupAttr->mLogGroupId,
                                  &sLogGroup,
                                  aEnv ) == STL_SUCCESS );
    STL_TRY_THROW( sLogGroup != NULL, RAMP_ERR_INVALID_LOGGROUP_NUMBER );

    sCurrCount = sLogGroup->mLogGroupPersData.mLogMemberCount;
    STL_TRY_THROW( sCurrCount + aMemberCount < SMR_MAX_LOGMEMBER_COUNT,
                   RAMP_ERR_EXCEED_MAX_LOGMEMBER_COUNT );

    sFileSize = sLogGroup->mLogGroupPersData.mFileSize;

    sLatched = STL_FALSE;
    STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );

    /*
     * 입력받은 Log Group의 유효성 검사
     */
    sLogState = smrGetLogGroupState( sLogGroup );

    STL_TRY_THROW( ( sLogState == SMR_LOG_GROUP_STATE_CREATING ) ||
                   ( sLogState == SMR_LOG_GROUP_STATE_UNUSED ) ||
                   ( sLogState == SMR_LOG_GROUP_STATE_INACTIVE ),
                   RAMP_ERR_INVALID_LOGGROUP_STATE );

    sLogMemberAttr = aGroupAttr->mLogMemberAttr;
    while( sLogMemberAttr != NULL )
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

        STL_RAMP( RAMP_RECREATE_LOG_MEMBER );

        STL_TRY( smrCreateLogMember( sLogMemberAttr,
                                     sFileSize,
                                     sLogStream->mLogStreamPersData.mBlockSize,
                                     sLogMemberAttr->mLogFileReuse,
                                     &sLogMember,
                                     aEnv ) == STL_SUCCESS );
        sCreated = STL_TRUE;

        /* Log Group에 추가 */
        STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
        sLatched = STL_TRUE;

        /*
         * 입력받은 Log Stream의 유효성 검사
         */
        sLogState = smrGetLogStreamState( sLogStream );

        STL_ASSERT( ( sLogState == SMR_LOG_STREAM_STATE_CREATING ) ||
                    ( sLogState == SMR_LOG_STREAM_STATE_ACTIVE ) );

        /* Latch를 풀었다 다시 잡았으므로 Log Group을 다시 찾아 봄 */
        STL_TRY( smrFindLogGroupById( sLogStream,
                                      aGroupAttr->mLogGroupId,
                                      &sLogGroup,
                                      aEnv ) == STL_SUCCESS );
        STL_TRY_THROW( sLogGroup != NULL, RAMP_ERR_INVALID_LOGGROUP_NUMBER );
        if( sLogGroup->mLogGroupPersData.mFileSize != sFileSize )
        {
            /**
             * Latch를 놓은 사이에 Log Group Drop/Create되어
             * File size가 변경되었다. 재시도 한다
             */
            sFileSize = sLogGroup->mLogGroupPersData.mFileSize;

            sCreated = STL_FALSE;
            STL_TRY( smrDestroyLogMember( sLogMember, aEnv ) == STL_SUCCESS );
            sLogMember = NULL;

            sLatched = STL_FALSE;
            STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );

            STL_THROW( RAMP_RECREATE_LOG_MEMBER );
        }

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
        sCreated = STL_FALSE;

        sLatched = STL_FALSE;
        STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );

        stlStrcpy( sPendArgs.mName, sLogMemberAttr->mName );
        STL_TRY( smgAddPendOp( NULL,
                               SMG_PEND_ADD_LOG_MEMBER,
                               SML_PEND_ACTION_ROLLBACK,
                               (void*)&sPendArgs,
                               STL_SIZEOF( smrPendAddLogGroup ),
                               0,
                               aEnv ) == STL_SUCCESS );

        sLogMemberAttr = sLogMemberAttr->mNext;
    }
    
    STL_TRY( smgAddPendOp( NULL,
                           SMG_PEND_ADD_LOG_MEMBER,
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
    
    STL_CATCH( RAMP_ERR_INVALID_LOGGROUP_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_LOGGROUP_STATE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      aGroupAttr->mLogGroupId );
    }
    
    STL_CATCH( RAMP_ERR_EXISTS_LOGMEMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXISTS_LOGMEMBER,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sLogMemberAttr->mName );
    }

    STL_CATCH( RAMP_ERR_EXCEED_MAX_LOGMEMBER_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEED_MAX_LOGMEMBER_COUNT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      SMR_MAX_LOGMEMBER_COUNT );
    }

    STL_FINISH;

    if( sLatched == STL_TRUE )
    {
        (void)smfReleaseDatabaseFileLatch( aEnv );
    }
    if( sCreated == STL_TRUE )
    {
        (void)smrDestroyLogMember( sLogMember, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrCreateLogMember( smlLogMemberAttr  * aAttr,
                              stlInt64            aFileSize,
                              stlInt16            aBlockSize,
                              stlBool             aIsReuseFile,
                              smrLogMember     ** aLogMember,
                              smlEnv            * aEnv )
{
    stlInt32       sState = 0;
    smrLogMember * sLogMember;
    /*
     * Log Member 할당
     */
    STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smrLogMember ),
                                   (void**)&sLogMember,
                                   aEnv )
             == STL_SUCCESS );
    sState = 1;

    SMR_INIT_LOG_MEMBER( sLogMember );

    sLogMember->mLogMemberPersData.mState = SMR_LOG_MEMBER_STATE_UNUSED;
    stlStrcpy( sLogMember->mLogMemberPersData.mName, aAttr->mName );

    /* 
     * Log File Formating
     */
    STL_TRY( smrFormatLogFile( aAttr->mName,
                               aFileSize,
                               aBlockSize,
                               aIsReuseFile,
                               aEnv ) == STL_SUCCESS );

    *aLogMember = sLogMember;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgFreeShmMem4Mount( sLogMember, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrDestroyLogMember( smrLogMember * aLogMember,
                               smlEnv       * aEnv )
{
    stlBool   sIsRemoved;

    /* Pending 및 에러 처리 연산에서는 File 관련 에러를 무시한다 */
    if( stlRemoveFile( aLogMember->mLogMemberPersData.mName,
                       KNL_ERROR_STACK(aEnv) )
        == STL_FAILURE )
    {
        stlPopError( KNL_ERROR_STACK(aEnv) );
    }

    STL_TRY( knlRemoveDbFile( aLogMember->mLogMemberPersData.mName,
                              &sIsRemoved,
                              KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( smgFreeShmMem4Mount( aLogMember, aEnv ) == STL_SUCCESS );
 
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrDoPendAddLogMember( stlUInt32   aActionFlag,
                                 smgPendOp * aPendOp,
                                 smlEnv    * aEnv )
{
    smrLogStream        * sLogStream = &gSmrWarmupEntry->mLogStream;
    smrPendAddLogMember * sPendArgs = (smrPendAddLogMember*)aPendOp->mArgs;
    smrLogGroup         * sLogGroup;
    smrLogMember        * sLogMember;
    stlInt32              sState = 0;

    if( aActionFlag == SML_PEND_ACTION_ROLLBACK )
    {
        STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( smrFindLogGroupById( sLogStream,
                                      sPendArgs->mLogGroupId,
                                      &sLogGroup,
                                      aEnv ) == STL_SUCCESS );
        if( sLogGroup != NULL )
        {
            STL_RING_FOREACH_ENTRY( &(sLogGroup->mLogMemberList), sLogMember )
            {
                if( stlStrcmp( sLogMember->mLogMemberPersData.mName,
                               sPendArgs->mName ) == 0 )
                {
                    STL_RING_UNLINK( &sLogGroup->mLogMemberList, sLogMember );

                    (void)smrDestroyLogMember( sLogMember, aEnv );
                }
            }
        } /* if */

        sState = 0;
        STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseDatabaseFileLatch( aEnv );
    }
    return STL_FAILURE;
}

stlStatus smrDropLogMemberEventHandler( void      * aData,
                                        stlUInt32   aDataSize,
                                        stlBool   * aDone,
                                        void      * aEnv)
{
    stlChar           * sPtr = (stlChar*) aData;
    smlLogStreamAttr  * sLogStreamAttr;
    smlLogGroupAttr   * sGroupAttr  = NULL;
    smlLogMemberAttr  * sMemberAttr = NULL;
    stlInt32            sGroupCount;
    stlInt32            sMemberCount;
    stlInt32            i;
    stlInt32            j;
    stlBool             sGroupExist = STL_FALSE;
    stlInt16            sGroupId;

    *aDone = STL_FALSE;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smlGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    /**
     * Data Unmarshalling
     *   LogStreamAttr
     *   LogGroupAttr Count
     *     LogGroupAttr
     *     LogMemberAttr Count
     *       LogMemberAttr [LogMemberAttr] ...
     *     [LogGroupAttr] ...  
     */ 
    sLogStreamAttr = (smlLogStreamAttr*) sPtr;
    sPtr += STL_ALIGN( STL_SIZEOF(smlLogStreamAttr), 8 );

    stlMemcpy( &sGroupCount, sPtr, STL_SIZEOF(stlInt32) );
    sPtr += STL_ALIGN( STL_SIZEOF(stlInt32), 8 );

    for( i = 0; i < sGroupCount; i++ )
    {
        if( sGroupAttr == NULL )
        {
            sLogStreamAttr->mLogGroupAttr = (smlLogGroupAttr*) sPtr;
        }
        else
        {
            sGroupAttr->mNext = (smlLogGroupAttr*) sPtr;
        }
        sGroupAttr = (smlLogGroupAttr*) sPtr;
        sPtr += STL_ALIGN( STL_SIZEOF(smlLogGroupAttr), 8 );

        stlMemcpy( &sMemberCount, sPtr, STL_SIZEOF(stlInt32) );
        sPtr += STL_ALIGN( STL_SIZEOF(stlInt32), 8 );

        sMemberAttr = NULL;
        for( j = 0; j < sMemberCount; j++ )
        {
            if( sMemberAttr == NULL )
            {
                sGroupAttr->mLogMemberAttr = (smlLogMemberAttr*) sPtr;
            }
            else
            {
                sMemberAttr->mNext = (smlLogMemberAttr*) sPtr;
            }
            sMemberAttr = (smlLogMemberAttr*) sPtr;
            sPtr += STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 );
        }
    }

    /**
     * Validation
     */
    sGroupAttr = sLogStreamAttr->mLogGroupAttr;
    while( sGroupAttr != NULL )
    {
        sMemberAttr = sGroupAttr->mLogMemberAttr;
        while( sMemberAttr != NULL )
        {

            STL_TRY( smrIsUsedLogFile( sMemberAttr->mName,
                                       &sGroupExist,
                                       &sGroupId,
                                       aEnv )
                     == STL_SUCCESS );

            STL_TRY_THROW( (sGroupExist == STL_TRUE) &&
                           (sGroupAttr->mLogGroupId == sGroupId),
                           RAMP_ERR_LOGFILE_NOT_EXIST );

            sMemberAttr = sMemberAttr->mNext;
        }

        sGroupAttr = sGroupAttr->mNext;
    }
    
    STL_TRY( smrDropLogMember( sLogStreamAttr, aEnv ) == STL_SUCCESS );

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

    STL_CATCH( RAMP_ERR_LOGFILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_LOG_MEMBER_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sMemberAttr->mName );
    }
    
    
    STL_FINISH;

    (void)smgExecutePendOp( SML_PEND_ACTION_ROLLBACK,
                            0, /* aStatementTcn */
                            aEnv );

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    *aDone = STL_TRUE;
    
    return STL_FAILURE;
}

stlStatus smrDropLogMember( smlLogStreamAttr * aStreamAttr,
                            smlEnv           * aEnv )
{
    stlInt32             sState      = 0;
    smrLogStream       * sLogStream  = &gSmrWarmupEntry->mLogStream;
    smlLogGroupAttr    * sGroupAttr  = NULL;
    smrLogGroup        * sLogGroup;
    smlLogMemberAttr   * sMemberAttr = NULL;
    smrLogMember       * sLogMember = NULL;
    smrPendDropLogMember sPendArgs;

    STL_PARAM_VALIDATE( aStreamAttr != NULL, KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Validation: Group State, Member State, Member Min Count
     */
    sGroupAttr = aStreamAttr->mLogGroupAttr;
    while( sGroupAttr != NULL )
    {
        STL_TRY( smrFindLogGroupById( sLogStream,
                                      sGroupAttr->mLogGroupId,
                                      &sLogGroup,
                                      aEnv ) == STL_SUCCESS );

        STL_TRY_THROW( sLogGroup != NULL, RAMP_ERR_INVALID_LOGGROUP_NUMBER );

        /* LogGroup의 State는 UNUSED와 INACTIVE 이외의 State이면 안되다. */
        STL_TRY_THROW( ((sLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_UNUSED) ||
                        (sLogGroup->mLogGroupPersData.mState == SMR_LOG_GROUP_STATE_INACTIVE)),
                       RAMP_ERR_INVALID_LOGGROUP_STATE );

        sMemberAttr = sGroupAttr->mLogMemberAttr;
        while( sMemberAttr != NULL )
        {
            STL_RING_FOREACH_ENTRY( &sLogGroup->mLogMemberList, sLogMember )
            {
                if( stlStrcmp(sMemberAttr->mName, sLogMember->mLogMemberPersData.mName) == 0 )
                {
                    /* LogMember의 State는 UNUSED와 INACTIVE 이외의 State이면 안되다. */
                    STL_TRY_THROW( (sLogMember->mLogMemberPersData.mState ==
                                    SMR_LOG_MEMBER_STATE_UNUSED) ||
                                   (sLogMember->mLogMemberPersData.mState ==
                                    SMR_LOG_GROUP_STATE_INACTIVE),
                                   RAMP_ERR_INVALID_LOGMEMBER_STATE );

                    /**
                     * LogGroup의 Member Count는 SML_MINIMUM_LOG_MEMBER_COUNT 보다 커야한다
                     */
                    STL_TRY_THROW( sLogGroup->mLogGroupPersData.mLogMemberCount >
                                   SMR_MIN_LOGMEMBER_COUNT,
                                   RAMP_ERR_MINUMUM_LOG_MEMBER_COUNT );

                    /* PendArgs 설정 */
                    stlMemset( &sPendArgs, 0x00, STL_SIZEOF( smrPendDropLogMember ) );

                    sPendArgs.mLogGroupId = sLogGroup->mLogGroupPersData.mLogGroupId;
                    sPendArgs.mPrevMemberState = sLogMember->mLogMemberPersData.mState;
                    stlStrcpy( sPendArgs.mName, sLogMember->mLogMemberPersData.mName );

                    /* Member State와 Member Count 설정 */
                    sLogMember->mLogMemberPersData.mState = SMR_LOG_MEMBER_STATE_DROPPING;
                    sLogGroup->mLogGroupPersData.mLogMemberCount--;

                    STL_TRY( smgAddPendOp( NULL,
                                           SMG_PEND_DROP_LOG_MEMBER,
                                           SML_PEND_ACTION_COMMIT | SML_PEND_ACTION_ROLLBACK,
                                           (void*)&sPendArgs,
                                           STL_SIZEOF( smrPendDropLogMember ),
                                           0,
                                           aEnv ) == STL_SUCCESS );
                    break;
                }
            }
            sMemberAttr = sMemberAttr->mNext;
        }
        sGroupAttr = sGroupAttr->mNext;
    }

    sState = 0;
    STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );

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

    STL_CATCH( RAMP_ERR_INVALID_LOGMEMBER_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_LOGMEMBER_STATE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sLogMember->mLogMemberPersData.mName );
    }

    STL_CATCH( RAMP_ERR_MINUMUM_LOG_MEMBER_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_MINIMUM_LOGMEMBER_COUNT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sLogGroup->mLogGroupPersData.mLogGroupId,
                      SMR_MIN_LOGMEMBER_COUNT );
    }
    
    STL_FINISH;


    switch( sState )
    {
        case 1:
            (void)smfReleaseDatabaseFileLatch( aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


stlStatus smrDoPendDropLogMember( stlUInt32   aActionFlag,
                                  smgPendOp * aPendOp,
                                  smlEnv    * aEnv )
{
    smrLogStream         * sLogStream = &gSmrWarmupEntry->mLogStream;
    smrPendDropLogMember * sPendArgs = (smrPendDropLogMember*)aPendOp->mArgs;
    smrLogGroup          * sLogGroup;
    smrLogMember         * sLogMember;
    stlInt32               sState = 0;

    STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    sState = 1;

    if( aActionFlag == SML_PEND_ACTION_COMMIT )
    {
        STL_TRY( smrFindLogGroupById( sLogStream,
                                      sPendArgs->mLogGroupId,
                                      &sLogGroup,
                                      aEnv ) == STL_SUCCESS );
        if( sLogGroup != NULL )
        {
            STL_RING_FOREACH_ENTRY( &(sLogGroup->mLogMemberList), sLogMember )
            {
                if( stlStrcmp( sLogMember->mLogMemberPersData.mName,
                               sPendArgs->mName ) == 0 )
                {
                    STL_RING_UNLINK( &sLogGroup->mLogMemberList, sLogMember );

                    (void)smrDestroyLogMember( sLogMember, aEnv );
                    break;
                }
            }
        }
    }
    else
    {
        STL_TRY( smrFindLogGroupById( sLogStream,
                                      sPendArgs->mLogGroupId,
                                      &sLogGroup,
                                      aEnv ) == STL_SUCCESS );
        if( sLogGroup != NULL )
        {
            STL_RING_FOREACH_ENTRY( &(sLogGroup->mLogMemberList), sLogMember )
            {
                if( stlStrcmp( sLogMember->mLogMemberPersData.mName,
                               sPendArgs->mName ) == 0 )
                {
                    sLogMember->mLogMemberPersData.mState = sPendArgs->mPrevMemberState;
                    sLogGroup->mLogGroupPersData.mLogMemberCount++;
                    break;
                }
            }
        }
    }

    sState = 0;
    STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseDatabaseFileLatch( aEnv );
    }

    return STL_FAILURE;
}
 

stlStatus smrRenameLogFileEventHandler( void      * aData,
                                        stlUInt32   aDataSize,
                                        stlBool   * aDone,
                                        void      * aEnv )
{
    stlChar          * sPtr = (stlChar*)aData;
    smlLogMemberAttr * sFromList = NULL;
    smlLogMemberAttr * sToList = NULL;
    smlLogMemberAttr * sCurFromFile = NULL;
    smlLogMemberAttr * sCurToFile = NULL;
    smlLogMemberAttr * sPrevFromFile = NULL;
    smlLogMemberAttr * sPrevToFile = NULL;
    stlInt32           sFileCount;
    stlInt32           i;

    *aDone = STL_FALSE;

    stlMemcpy( (void*)&sFileCount, sPtr, STL_SIZEOF(stlInt32) );
    sPtr += STL_ALIGN( STL_SIZEOF(stlInt32), 8 );

    for( i = 0; i < sFileCount; i++ )
    {
        sCurFromFile = (smlLogMemberAttr*)sPtr;
        sPtr += STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 );
        sCurToFile = (smlLogMemberAttr*)sPtr;
        sPtr += STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 );
        if( sFromList == NULL )
        {
            sFromList = sCurFromFile;
            sToList = sCurToFile;
        }
        else
        {
            sPrevFromFile->mNext = sCurFromFile;
            sPrevToFile->mNext = sCurToFile;
            sPrevFromFile = sCurFromFile;
            sPrevToFile = sCurToFile;
        }
        sCurFromFile->mNext = NULL;
        sCurToFile->mNext = NULL;
        sPrevFromFile = sCurFromFile;
        sPrevToFile = sCurToFile;
    }

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    STL_DASSERT( smfGetDataAccessMode() == SML_DATA_ACCESS_MODE_READ_WRITE );

    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_ONLY_MOUNT_PHASE );

    STL_TRY( smrRenameLogFile( sFileCount, sFromList, sToList, aEnv ) == STL_SUCCESS );

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


stlStatus smrRenameLogFile( stlInt32           aFileCount,
                            smlLogMemberAttr * aFromList,
                            smlLogMemberAttr * aToList,
                            smlEnv           * aEnv )
{
    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_MOUNT,
                   RAMP_ERR_INVALID_PHASE );
    
    STL_TRY( smrRenameLogFileAtMount( aFileCount,
                                      aFromList,
                                      aToList,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_NOT_APPROPRIATE_PHASE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      gPhaseString[KNL_STARTUP_PHASE_OPEN] );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrRenameLogFileAtMount( stlInt32           aFileCount,
                                   smlLogMemberAttr * aFromList,
                                   smlLogMemberAttr * aToList,
                                   smlEnv           * aEnv )
{
    smlLogMemberAttr     * sCurFromFile = aFromList;
    smlLogMemberAttr     * sCurToFile = aToList;
    smrLogStream         * sLogStream = &gSmrWarmupEntry->mLogStream;
    smrLogGroup          * sLogGroup;
    smrLogMember         * sLogMember = NULL;
    stlBool                sExist;
    smrPendRenameLogFile   sPendArgs;
    stlInt32               sState = 0;

    /**
     * defence to saving control file
     */
    
    STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    sState = 1;

    while( sCurFromFile != NULL )
    {
        sExist = STL_FALSE;
        STL_RING_FOREACH_ENTRY( &sLogStream->mLogGroupList, sLogGroup )
        {
            STL_RING_FOREACH_ENTRY( &sLogGroup->mLogMemberList, sLogMember )
            {
                if( stlStrcmp( sCurToFile->mName, sLogMember->mLogMemberPersData.mName ) == 0 )
                {
                    sExist = STL_TRUE;
                    STL_THROW( RAMP_TO_FILE_FOUND );
                }
            }
        }

        STL_RAMP( RAMP_TO_FILE_FOUND );

        /**
         * validate log group
         */
    
        STL_TRY_THROW( sExist != STL_TRUE, RAMP_ERR_ALREADY_USED_MEMBER );

        sExist = STL_FALSE;
        STL_RING_FOREACH_ENTRY( &sLogStream->mLogGroupList, sLogGroup )
        {
            STL_RING_FOREACH_ENTRY( &sLogGroup->mLogMemberList, sLogMember )
            {
                if( stlStrcmp( sCurFromFile->mName, sLogMember->mLogMemberPersData.mName ) == 0 )
                {
                    sExist = STL_TRUE;
                    STL_THROW( RAMP_FROM_FILE_FOUND );
                }
            }
        }

        STL_RAMP( RAMP_FROM_FILE_FOUND );

        STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_MEMBER_NOT_EXIST );

        STL_TRY( stlExistFile( sCurToFile->mName,
                               &sExist,
                               KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );

        stlStrcpy( sLogMember->mLogMemberPersData.mName, sCurToFile->mName );

        sPendArgs.mLogGroupId = sLogGroup->mLogGroupPersData.mLogGroupId;
        stlStrcpy( sPendArgs.mFromName, sCurFromFile->mName );
        stlStrcpy( sPendArgs.mToName, sCurToFile->mName );
 
        STL_TRY( smgAddPendOp( NULL,
                               SMG_PEND_RENAME_LOG_FILE,
                               SML_PEND_ACTION_ROLLBACK,
                               (void*)&sPendArgs,
                               STL_SIZEOF( smrPendRenameLogFile ),
                               0,
                               aEnv ) == STL_SUCCESS );

        /**
         * database file mananger에 새로운 log file name을 등록한다.
         */
        STL_TRY( knlAddDbFile( sCurToFile->mName,
                               KNL_DATABASE_FILE_TYPE_REDO_LOG,
                               KNL_ENV(aEnv) ) == STL_SUCCESS );

        STL_TRY( smgAddPendOp( NULL,
                               SMG_PEND_RENAME_LOG_FILE,
                               SML_PEND_ACTION_COMMIT,
                               (void*)&sPendArgs,
                               STL_SIZEOF( smrPendRenameLogFile ),
                               0,
                               aEnv ) == STL_SUCCESS );

        sCurFromFile = sCurFromFile->mNext;
        sCurToFile = sCurToFile->mNext;

        STL_TRY( knlCheckEnvEvent( KNL_ENV(aEnv) ) == STL_SUCCESS );
    }
    sState = 0;
    STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );

    STL_TRY( smgAddPendOp( NULL,
                           SMG_PEND_RENAME_LOG_FILE,
                           SML_PEND_ACTION_COMMIT,
                           NULL,
                           0,
                           0,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MEMBER_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_LOG_MEMBER_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sCurFromFile->mName );
    }
 
    STL_CATCH( RAMP_ERR_ALREADY_USED_MEMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ALREADY_USED_LOG_MEMBER,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sCurToFile->mName );
    }
 
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sCurToFile->mName );
    }
 
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseDatabaseFileLatch( aEnv );
    }

    return STL_FAILURE;
}

stlStatus smrDoPendRenameLogFile( stlUInt32   aActionFlag,
                                  smgPendOp * aPendOp,
                                  smlEnv    * aEnv )
{
    smrLogStream         * sLogStream = &gSmrWarmupEntry->mLogStream;
    smrPendRenameLogFile * sPendArgs = (smrPendRenameLogFile*)aPendOp->mArgs;
    smrLogGroup          * sLogGroup;
    smrLogMember         * sLogMember;
    stlInt32               sState = 0;
    stlBool                sIsRemoved;

    if( aActionFlag == SML_PEND_ACTION_ROLLBACK )
    {
        STL_TRY( smfAcquireDatabaseFileLatch( aEnv ) == STL_SUCCESS );
        sState = 1;

        /* database file manager에서 new log file name을 제거하고,
         * 기존의 log file name을 등록한다. */
        STL_TRY( knlRemoveDbFile( sPendArgs->mToName,
                                  &sIsRemoved,
                                  KNL_ENV(aEnv) ) == STL_SUCCESS );

        STL_TRY( smrFindLogGroupById( sLogStream,
                                      sPendArgs->mLogGroupId,
                                      &sLogGroup,
                                      aEnv ) == STL_SUCCESS );
        if( sLogGroup != NULL )
        {
             STL_RING_FOREACH_ENTRY( &(sLogGroup->mLogMemberList), sLogMember )
            {
                if( stlStrcmp( sLogMember->mLogMemberPersData.mName,
                               sPendArgs->mToName ) == 0 )
                {
                    stlStrcpy( sLogMember->mLogMemberPersData.mName, sPendArgs->mFromName );
                    break;
                }
            }
        }

        sState = 0;
        STL_TRY( smfReleaseDatabaseFileLatch( aEnv ) == STL_SUCCESS );
    }
    else
    {
        /**
         * database file manager에서 기존의 log member file name을 제거한다.
         */
        STL_TRY( knlRemoveDbFile( sPendArgs->mFromName,
                                  &sIsRemoved,
                                  KNL_ENV(aEnv) ) == STL_SUCCESS );
    }

    STL_TRY( smfSaveCtrlFile( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smfReleaseDatabaseFileLatch( aEnv );
    }

    return STL_FAILURE;
}
 
stlStatus smrFirstLogMemberPersData( void                 * aLogGroupIterator,
                                     smrLogMemberPersData * aLogMemberPersData,
                                     void                ** aLogMemberIterator,
                                     smlEnv               * aEnv )
{
    smrLogGroup  * sLogGroup;
    smrLogMember * sLogMember;

    sLogGroup = (smrLogGroup *)aLogGroupIterator;

    if( STL_RING_IS_EMPTY( &(sLogGroup->mLogMemberList) ) == STL_TRUE )
    {
        *aLogMemberIterator = NULL;
    }
    else
    {
        sLogMember = STL_RING_GET_FIRST_DATA( &(sLogGroup->mLogMemberList) );
        *aLogMemberPersData = sLogMember->mLogMemberPersData;
        *aLogMemberIterator = (void*)sLogMember;
    }

    return STL_SUCCESS;
}

stlStatus smrNextLogMemberPersData( void                 * aLogGroupIterator,
                                    smrLogMemberPersData * aLogMemberPersData,
                                    void                ** aLogMemberIterator,
                                    smlEnv               * aEnv )
{
    smrLogGroup  * sLogGroup;
    smrLogMember * sLogMember;

    sLogGroup = (smrLogGroup *)aLogGroupIterator;
    sLogMember = STL_RING_GET_NEXT_DATA( &(sLogGroup->mLogMemberList),
                                         *aLogMemberIterator);

    if( STL_RING_IS_HEADLINK( &(sLogGroup->mLogMemberList),
                              sLogMember ) == STL_TRUE )
    {
        *aLogMemberIterator = NULL;
    }
    else
    {
        *aLogMemberPersData = sLogMember->mLogMemberPersData;
        *aLogMemberIterator = (void*)sLogMember;
    }

    return STL_SUCCESS;
}

stlStatus smrIsUsedLogFile( stlChar   * aFileName,
                            stlBool   * aExist,
                            stlInt16  * aGroupId,
                            smlEnv    * aEnv )
{
    smrLogStream * sLogStream = &gSmrWarmupEntry->mLogStream;
    smrLogGroup  * sLogGroup;
    smrLogMember * sLogMember;
    
    *aExist = STL_FALSE;

    STL_RING_FOREACH_ENTRY( &sLogStream->mLogGroupList, sLogGroup )
    {
        STL_RING_FOREACH_ENTRY( &sLogGroup->mLogMemberList, sLogMember )
        {
            if( stlStrcmp( aFileName, sLogMember->mLogMemberPersData.mName ) == 0 )
            {
                *aExist = STL_TRUE;

                if( aGroupId != NULL )
                {
                    *aGroupId = sLogGroup->mLogGroupPersData.mLogGroupId;
                }

                STL_THROW( RAMP_FILE_FOUND );
            }
        }
    }

    if( aGroupId != NULL )
    {
        *aGroupId = SML_INVALID_LOG_GROUP_ID;
    }

    STL_TRY( knlExistDbFile( aFileName,
                             aExist,
                             KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_RAMP( RAMP_FILE_FOUND );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
/** @} */
