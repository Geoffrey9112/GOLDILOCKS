/*******************************************************************************
 * qldAlterDatabaseRecoverDatafile.c
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

/**
 * @file qldAlterDatabaseRecoverDatafile.c
 * @brief ALTER DATABASE RECOVER DATAFILE .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>

/**
 * @addtogroup qldAlterDatabaseRecoverDatafile
 * @{
 */

/**
 * @brief ALTER DATABASE RECOVER DATAFILE .. 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseRecoverDatafile( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   stlChar       * aSQLString,
                                                   qllNode       * aParseTree,
                                                   qllEnv        * aEnv )
{
    qldInitAlterDatabaseRecoverDatafile * sInitPlan = NULL;
    qlpAlterDatabaseRecoverDatafile     * sParseTree = NULL;
    qlpListElement                      * sDatafileElement = NULL;
    qlpDatafileList                     * sDatafileList    = NULL;
    
    smlMediaRecoveryDatafile            * sDatafileInfo = NULL;
    smlMediaRecoveryDatafile              sTempDatafileInfo;

    stlInt64                              sDatafileCount = 0;
    stlInt32                              sDatafileId;
    stlInt32                              sIdx = 0;
    stlInt32                              i;
    stlChar                             * sFileName;
    stlBool                               sFileExist = STL_FALSE;
    stlBool                               sAged = STL_FALSE;
    stlBool                               sFoundSameTbs;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RECOVER_DATAFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_RECOVER_DATAFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sParseTree = (qlpAlterDatabaseRecoverDatafile *) aParseTree;

    /**
     * Init Plan 생성
     */
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF( qldInitAlterDatabaseRecoverDatafile ),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF( qldInitAlterDatabaseRecoverDatafile ) );

    /**
     * sInitPlan의 멤버 변수 RecoveryInfo에 속한 Datafile 정보를 연결한다.
     */
    sDatafileCount = QLP_LIST_GET_COUNT( sParseTree->mRecoverList );

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF( smlMediaRecoveryDatafile ) * sDatafileCount,
                                (void **) &sDatafileInfo,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    stlMemset( sDatafileInfo,
               0x00,
               sDatafileCount * STL_SIZEOF( smlMediaRecoveryDatafile ) );

    QLP_LIST_FOR_EACH( sParseTree->mRecoverList, sDatafileElement )
    {
        sDatafileList = (qlpDatafileList *) QLP_LIST_GET_POINTER_VALUE( sDatafileElement );

        /**
         * DATAFILE 정보
         */ 
        sFileName = QLP_GET_PTR_VALUE( sDatafileList->mDatafileName );
        STL_TRY( smlExistDatafile( sFileName,
                                   &sFileExist,
                                   SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_NOT_EXIST_FILE );
        
        STL_TRY( smlIsUsedDatafile( sFileName,
                                    &sDatafileInfo[sIdx].mTbsId,
                                    &sDatafileId,
                                    &sFileExist,
                                    &sAged,
                                    SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * tablespace에서 사용중인 데이터파일에 대해서만 복구 가능하다.
         */
        STL_TRY_THROW( (sDatafileId != SML_INVALID_DATAFILE_ID) &&
                       (sFileExist == STL_TRUE) &&
                       (sAged == STL_FALSE),
                       RAMP_ERR_NOT_EXIST_FILE_IN_DATABASE );

        sDatafileInfo[sIdx].mDatafileId = (smlDatafileId) sDatafileId;

        /**
         * USING BACKUP OPTION
         */ 
        if( sDatafileList->mUsingBackup == STL_TRUE )
        {
            sDatafileInfo[sIdx].mUsingBackup = STL_TRUE;
            sFileName = QLP_GET_PTR_VALUE( sDatafileList->mBackupName );
            STL_TRY( smlGetAbsBackupFileName( sFileName,
                                              sDatafileInfo[sIdx].mBackupName,
                                              &sFileExist,
                                              SML_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_NOT_EXIST_FILE );
        }
        else
        {
            sDatafileInfo[sIdx].mUsingBackup = STL_FALSE;
            
            STL_TRY( smlExistValidIncBackup( sDatafileInfo[sIdx].mTbsId,
                                             &sFileExist,
                                             SML_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_NOT_EXIST_VALID_BACKUP );
        }

        /**
         * CORRUPTION OPTION
         */ 
        sDatafileInfo[sIdx].mForCorruption = sDatafileList->mForCorruption;
        
        /**
         * Tablespace Id로 오름차순 정렬.
         */
        for( i = 0; i < sIdx; i++ )
        {
            if( sDatafileInfo[i].mTbsId > sDatafileInfo[sIdx].mTbsId )
            {
                stlMemcpy( &sTempDatafileInfo,
                           &sDatafileInfo[i],
                           STL_SIZEOF( smlMediaRecoveryDatafile ) );

                stlMemcpy( &sDatafileInfo[i],
                           &sDatafileInfo[sIdx],
                           STL_SIZEOF( smlMediaRecoveryDatafile ) );

                stlMemcpy( &sDatafileInfo[sIdx],
                           &sTempDatafileInfo,
                           STL_SIZEOF( smlMediaRecoveryDatafile ) );
            }
            else if( sDatafileInfo[i].mTbsId == sDatafileInfo[sIdx].mTbsId )
            {
                /**
                 * 같은 TbsId, DatfileId를 갖는 Datafile 중복 제거.
                 */ 
                if( sDatafileInfo[i].mDatafileId == sDatafileInfo[sIdx].mDatafileId )
                {
                    sIdx--;
                    sDatafileCount--;
                    break;
                }
            }
            else
            {
                /* nothing to do */
            }
        }

        sIdx++;
    }

    if( knlGetCurrStartupPhase() < KNL_STARTUP_PHASE_OPEN )
    {
        /**
         * ALTER DATABASE ON DATABASE 권한 검사
         */
        STL_TRY( qlaCheckDatabasePriv( aTransID,
                                       aDBCStmt,
                                       aSQLStmt,
                                       ELL_DB_PRIV_ACTION_ALTER_DATABASE,
                                       aEnv )
                == STL_SUCCESS );

        for( sIdx = 0; sIdx < sDatafileCount; sIdx++ )
        {
            sFoundSameTbs = STL_FALSE;
            
            for( i = sIdx - 1; i >= 0; i-- )
            {
                if( sDatafileInfo[sIdx].mTbsId == sDatafileInfo[i].mTbsId )
                {
                    sFoundSameTbs = STL_TRUE;
                    break;
                }
            }

            if( sFoundSameTbs == STL_FALSE )
            {
                STL_TRY( smlValidateMediaRecovery( sDatafileInfo[sIdx].mTbsId,
                                                   SML_RECOVERY_OBJECT_TYPE_DATAFILE,
                                                   SML_ENV(aEnv) ) == STL_SUCCESS );
            }
        }
    }

    /**
     * Init Plan 연결 
     */
    sInitPlan->mRecoveryInfo.mDatafileCount = sDatafileCount;
    sInitPlan->mRecoveryInfo.mDatafileInfo  = sDatafileInfo;

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FILE_NOT_EXIST,
                      sFileName,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_EXIST_FILE_IN_DATABASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DATAFILE_NOT_EXISTS_DATABASE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_EXIST_VALID_BACKUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_EXIST_VALID_INCREMENTAL_BACKUP,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE RECOVER DATAFILE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseRecoverDatafile( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}

/**
 * @brief ALTER DATABASE RECOVER DATAFILE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseRecoverDatafile( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}

/**
 * @brief ALTER DATABASE RECOVER DATAFILE 를 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseRecoverDatafile( smlTransId      aTransID,
                                                  smlStatement  * aStmt,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv )
{
    qldInitAlterDatabaseRecoverDatafile * sInitPlan = NULL;
    smlDatafileRecoveryInfo             * sRecoveryInfo;
    stlInt32                              sOffset = 0;
    stlChar                             * sData = NULL;
    stlInt64                              sDataSize;
    stlInt32                              i;
    stlInt32                              j;
    stlBool                               sFoundSameTbs;
    stlBool                               sExist = STL_FALSE;
    stlBool                               sLocked = STL_TRUE;
    ellDictHandle                         sSpaceHandle;
    
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RECOVER_DATAFILE_TYPE,
                        QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK( aEnv ) );

    /**
     * Init Plan 획득
     */

    sInitPlan = (qldInitAlterDatabaseRecoverDatafile *) aSQLStmt->mInitPlan;

    /**
     * Data Marshalling
     * +++++++++++++++++++++++++++++++++++++++++++++++++++++++
     * | smlDatafileRecoveryInfo                             |
     * +++++++++++++++++++++++++++++++++++++++++++++++++++++++
     * | smlMediaRecoveryDatafile ( Datafile Info Array )    |
     * +++++++++++++++++++++++++++++++++++++++++++++++++++++++
     */
    sRecoveryInfo = &sInitPlan->mRecoveryInfo;
        
    /***************************************************************
     * DDL 준비
     ***************************************************************/

    /***************************************************************
     * execute RECOVER PAGE
     ***************************************************************/

    sRecoveryInfo->mCommonInfo.mRecoveryType = SML_RECOVERY_TYPE_COMPLETE_MEDIA_RECOVERY;
    sRecoveryInfo->mCommonInfo.mObjectType   = SML_RECOVERY_OBJECT_TYPE_DATAFILE;
    sRecoveryInfo->mCommonInfo.mTbsId        = SML_INVALID_TBS_ID;
    sRecoveryInfo->mCommonInfo.mImrOption    = SML_IMR_OPTION_NONE;
    sRecoveryInfo->mCommonInfo.mUntilValue   = -1;
    sRecoveryInfo->mCommonInfo.mImrCondition = SML_IMR_CONDITION_NONE;

    if( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_OPEN )
    {
        KNL_BREAKPOINT( KNL_BREAKPOINT_QLDEXECUTERECOVERPAGE_BEFORE_ACQUIRE_LOCK,
                        KNL_ENV(aEnv) );
        
        for( i = 0; i < sRecoveryInfo->mDatafileCount; i++ )
        {
            sFoundSameTbs = STL_FALSE;

            for( j = i - 1; j >= 0; j-- )
            {
                if( sRecoveryInfo->mDatafileInfo[j].mTbsId ==
                    sRecoveryInfo->mDatafileInfo[i].mTbsId )
                {
                    sFoundSameTbs = STL_TRUE;
                    break;
                }
            }

            if( sFoundSameTbs == STL_FALSE )
            {
                STL_TRY_THROW( smlIsOnlineTablespace( sRecoveryInfo->mDatafileInfo[i].mTbsId )
                               == STL_FALSE,
                               RAMP_ERR_NOT_OFFLINE );

                STL_RAMP( RAMP_RETRY );
                STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
                
                STL_TRY( ellGetTablespaceDictHandleByID( aTransID,
                                                         aSQLStmt->mViewSCN,
                                                         sRecoveryInfo->mDatafileInfo[i].mTbsId,
                                                         &sSpaceHandle,
                                                         &sExist,
                                                         ELL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                STL_TRY( ellLock4AlterTablespace( aTransID,
                                                  aStmt,
                                                  &sSpaceHandle,
                                                  & sLocked,
                                                  ELL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
            }
        }

        KNL_BREAKPOINT( KNL_BREAKPOINT_QLDEXECUTERECOVERPAGE_AFTER_ACQUIRE_LOCK,
                        KNL_ENV(aEnv) );
        
        STL_TRY( smlMediaRecoverDatafile( sRecoveryInfo,
                                          SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        sDataSize = STL_OFFSETOF( smlDatafileRecoveryInfo, mDatafileInfo ) +
            ( STL_SIZEOF( smlMediaRecoveryDatafile ) * sRecoveryInfo->mDatafileCount );

        STL_TRY( knlAllocRegionMem( & QLL_EXEC_DDL_MEM(aEnv),
                                    sDataSize,
                                    (void **) & sData,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sOffset = 0;
        STL_WRITE_MOVE_BYTES( sData,
                              sRecoveryInfo,
                              STL_OFFSETOF( smlDatafileRecoveryInfo, mDatafileInfo ),
                              sOffset );

        STL_WRITE_MOVE_BYTES( sData,
                              sRecoveryInfo->mDatafileInfo,
                              STL_SIZEOF(smlMediaRecoveryDatafile) * sRecoveryInfo->mDatafileCount,
                              sOffset );
    
        /**
         * Media Recovery를 수행한다.
         */
        STL_TRY( knlAddEnvEvent( SML_EVENT_MEDIA_RECOVER,
                                 NULL,                   /* aEventMem*/
                                 sData,                  /* aData*/
                                 sDataSize,              /* aDataSize*/
                                 0,                      /* aTargetEnvId*/
                                 KNL_EVENT_WAIT_POLLING,
                                 STL_FALSE, /* aForceSuccess */
                                 KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_OFFLINE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_OFFLINE_TABLESPACE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} qldAlterDatabaseRecoverDatafile */
