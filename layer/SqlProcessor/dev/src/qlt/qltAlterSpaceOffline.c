/*******************************************************************************
 * qltAlterSpaceOffline.c
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
 * @file qltAlterSpaceOffline.c
 * @brief ALTER TABLESPACE .. ONOFF 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qltAlterSpaceOffline ALTER TABLESPACE .. OFFLINE
 * @ingroup qltAlterSpace
 * @{
 */


/**
 * @brief ALTER TABLESPACE .. OFFLINE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltValidateAlterSpaceOffline( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv )
{
    qltInitAlterSpaceOffline * sInitPlan = NULL;
    qlpAlterSpaceOffline     * sParseTree = NULL;
    stlBool                    sObjectExist = STL_FALSE;
    stlInt64                   sSpaceID;
    ellTablespaceUsageType     sUsageType = ELL_SPACE_USAGE_TYPE_NA;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_OFFLINE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLESPACE_OFFLINE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterSpaceOffline *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltInitAlterSpaceOffline),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltInitAlterSpaceOffline) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /*************************************************
     * TABLESPACE name 검증 
     *************************************************/
    
    /**
     * Tablespace 존재 여부
     */

    STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         QLP_GET_PTR_VALUE( sParseTree->mName ),
                                         & sInitPlan->mSpaceHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SPACE_NOT_EXIST );

    /**
     * ALTER TABLESPACE ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_TABLESPACE,
                                   aEnv )
             == STL_SUCCESS );
    
    /*************************************************
     * TABLESPACE 검증 
     *************************************************/

    /**
     * Tablespce Offline 검증
     */
    
    sSpaceID = ellGetTablespaceID( &sInitPlan->mSpaceHandle );
                                    
    STL_TRY_THROW( smlIsOnlineTablespace( sSpaceID ) == STL_TRUE,
                   RAMP_ERR_NOT_ONLINE_TABLESPACE );

    /**
     * Usage Type 검증
     */
    
    sUsageType = ellGetTablespaceUsageType( & sInitPlan->mSpaceHandle );
    
    STL_TRY_THROW( sUsageType != ELL_SPACE_USAGE_TYPE_TEMPORARY,
                   RAMP_ERR_OFFLINE_TEMPORARY_TABLESPACE );

    STL_TRY_THROW( sUsageType != ELL_SPACE_USAGE_TYPE_UNDO,
                   RAMP_ERR_OFFLINE_UNDO_TABLESPACE );

    /**
     * Dictionary Tablespace 검사
     */
    
    STL_TRY_THROW( sSpaceID != SML_MEMORY_DICT_SYSTEM_TBS_ID,
                   RAMP_ERR_OFFLINE_DICTIONARY_TABLESPACE );

    if( sParseTree->mOfflineBehavior == NULL )
    {
        sInitPlan->mOfflineBehavior = SML_OFFLINE_BEHAVIOR_NORMAL;
    }
    else
    {
        sInitPlan->mOfflineBehavior = QLP_GET_INT_VALUE( sParseTree->mOfflineBehavior );
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSpaceHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /********************************************************
     * Validation Offline Tablespace
     ********************************************************/

    STL_TRY( smlValidateOfflineTablespace( sInitPlan->mOfflineBehavior,
                                           SML_ENV(aEnv) )
             == STL_SUCCESS );

    /*************************************************
     * Plan 연결 
     *************************************************/
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SPACE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }

    STL_CATCH( RAMP_ERR_NOT_ONLINE_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_ONLINE_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_OFFLINE_DICTIONARY_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_OFFLINE_DICTIONARY_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }
    
    STL_CATCH( RAMP_ERR_OFFLINE_UNDO_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_OFFLINE_UNDO_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_OFFLINE_TEMPORARY_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_OFFLINE_TEMPORARY_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief ALTER TABLESPACE .. OFFLINE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptCodeAlterSpaceOffline( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. OFFLINE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptDataAlterSpaceOffline( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. OFFLINE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltExecuteAlterSpaceOffline( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    qltInitAlterSpaceOffline * sInitPlan = NULL;
    stlInt64                   sSpaceID;
    stlTime                    sTime = 0;
    knlStartupPhase            sStartupPhase = KNL_STARTUP_PHASE_NONE;
    smlOfflineTbsArg           sOfflineArg;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_OFFLINE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * 기본 정보 설정 
     ***************************************************************/

    sStartupPhase = knlGetCurrStartupPhase();
    
    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( aStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /***************************************************************
     *  Run-Time Validation
     ***************************************************************/

    STL_RAMP( RAMP_RETRY );
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Valid Plan 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qltInitAlterSpaceOffline *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */


    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( qltLock4AlterSpaceOnOffline( aTransID,
                                              aStmt,
                                              aDBCStmt,
                                              & sInitPlan->mSpaceHandle,
                                              & sLocked,
                                              aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    }

    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qltInitAlterSpaceOffline *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Online Validation
     ***************************************************************/
    
    sSpaceID = ellGetTablespaceID( & sInitPlan->mSpaceHandle );
    
    STL_TRY_THROW( smlIsOnlineTablespace( sSpaceID ) == STL_TRUE,
                   RAMP_ERR_NOT_ONLINE_TABLESPACE );

    /**
     * Tablespace Offline Immediate의 경우, Online으로 변경 시
     * Media recovery가 수행되어야 하기 때문에
     * Archivelog Mode가 아니면 수행할 수 없다.
     */
    if( sInitPlan->mOfflineBehavior == SML_OFFLINE_BEHAVIOR_IMMEDIATE )
    {
        STL_TRY_THROW( smlIsLoggingTablespace( sSpaceID ) == STL_TRUE,
                       RAMP_ERR_NO_LOGGING_TABLESPACE );
    }

    /***************************************************************
     * execute OFFLINE
     ***************************************************************/

    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( smlOfflineTablespace( aStmt,
                                       sSpaceID,
                                       sInitPlan->mOfflineBehavior,
                                       SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sOfflineArg.mTbsId           = sSpaceID;
        sOfflineArg.mOfflineBehavior = sInitPlan->mOfflineBehavior;

        STL_TRY( knlAddEnvEvent( SML_EVENT_OFFLINE_TBS,
                                 NULL,                               /* aEventMem */
                                 &sOfflineArg,                       /* aData */
                                 STL_SIZEOF(smlOfflineTbsArg),       /* aDataSize */
                                 0,                                  /* aTargetEnvId */
                                 KNL_EVENT_WAIT_POLLING,
                                 STL_FALSE, /* aForceSuccess */
                                 KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLTEXECUTEALTERSPACEOFFLINE_AFTER_SMLOFFLINETABLESPACE,
                    KNL_ENV(aEnv) );
                                   
    /***************************************************************
     * ALTER 수행시간을 기록 
     ***************************************************************/

    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( ellSetTimeAlterTablespace( aTransID,
                                            aStmt,
                                            sSpaceID,
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
                                        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_ONLINE_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_ONLINE_TABLESPACE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NO_LOGGING_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NO_LOGGING_TABLESPACE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}





/**
 * @brief ONOFF를 위해 락을 설정한다.
 * @param[in]  aTransId          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aDBCStmt          DBC Statement
 * @param[in]  aSpaceHandle      Tablespace Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus qltLock4AlterSpaceOnOffline( smlTransId       aTransId,
                                       smlStatement   * aStmt,
                                       qllDBCStmt     * aDBCStmt,
                                       ellDictHandle  * aSpaceHandle,
                                       stlBool        * aLockSuccess,
                                       qllEnv         * aEnv )
{
    ellObjectList * sInnerTableList = NULL;
    ellObjectList * sOuterTableInnerPrimaryKeyList = NULL;
    ellObjectList * sOuterTableInnerUniqueKeyList = NULL;
    ellObjectList * sOuterTableInnerIndexList = NULL;
    ellObjectList * sInnerForeignKeyList = NULL;
    ellObjectList * sOuterForeignKeyList = NULL;

    ellObjectList * sOuterTableInnerPrimaryKeyTableList = NULL;
    ellObjectList * sOuterTableInnerPrimaryKeyIndexList = NULL;

    ellObjectList * sOuterTableInnerUniqueKeyTableList = NULL;
    ellObjectList * sOuterTableInnerUniqueKeyIndexList = NULL;

    ellObjectList * sOuterTableInnerIndexTableList = NULL;
    
    ellObjectList * sInnerForeignKeyParentTableList = NULL;
    ellObjectList * sInnerForeignKeyChildTableList = NULL;
    ellObjectList * sInnerForeignKeyIndexList = NULL;

    ellObjectList * sOuterForeignKeyParentTableList = NULL;
    ellObjectList * sOuterForeignKeyChildTableList = NULL;
    ellObjectList * sOuterForeignKeyIndexList = NULL;

    stlBool  sLocked = STL_TRUE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, QLL_ERROR_STACK(aEnv) );

    STL_TRY( ellLock4AlterTablespace( aTransId,
                                      aStmt,
                                      aSpaceHandle,
                                      & sLocked,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    STL_TRY( ellGetObjectList4DropSpace( aTransId,
                                         aStmt,
                                         aSpaceHandle,
                                         & QLL_EXEC_DDL_MEM(aEnv),
                                         & sInnerTableList,
                                         & sOuterTableInnerPrimaryKeyList,
                                         & sOuterTableInnerUniqueKeyList,
                                         & sOuterTableInnerIndexList,
                                         & sInnerForeignKeyList,
                                         & sOuterForeignKeyList,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***************************************************************
     * 관련 객체의 추가 정보 회득 
     ***************************************************************/
    
    /**
     * Outer Table Inner Primary Key 와 관련된 정보 획득
     */

    STL_TRY( ellGetTableList4ConstList( aTransId,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerPrimaryKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerPrimaryKeyTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellGetIndexList4KeyList( aTransId,
                                      sOuterTableInnerPrimaryKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sOuterTableInnerPrimaryKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Outer Table Inner Unique Key 와 관련된 정보 획득
     */

    STL_TRY( ellGetTableList4ConstList( aTransId,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerUniqueKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerUniqueKeyTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellGetIndexList4KeyList( aTransId,
                                      sOuterTableInnerUniqueKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sOuterTableInnerUniqueKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Table Inner Index 와 관련된 정보 획득
     */

    STL_TRY( ellGetTableList4IndexList( aTransId,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterTableInnerIndexList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterTableInnerIndexTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Inner Foreign Key 와 관련된 정보 획득
     */

    STL_TRY( ellGetParentTableList4ForeignKeyList( aTransId,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   sInnerForeignKeyList,
                                                   & QLL_EXEC_DDL_MEM(aEnv),
                                                   & sInnerForeignKeyParentTableList,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( ellGetTableList4ConstList( aTransId,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sInnerForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sInnerForeignKeyChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellGetIndexList4KeyList( aTransId,
                                      sInnerForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sInnerForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Outer Foreign Key 와 관련된 정보 획득
     */

    STL_TRY( ellGetParentTableList4ForeignKeyList( aTransId,
                                                   ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                   sOuterForeignKeyList,
                                                   & QLL_EXEC_DDL_MEM(aEnv),
                                                   & sOuterForeignKeyParentTableList,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( ellGetTableList4ConstList( aTransId,
                                        ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                        sOuterForeignKeyList,
                                        & QLL_EXEC_DDL_MEM(aEnv),
                                        & sOuterForeignKeyChildTableList,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellGetIndexList4KeyList( aTransId,
                                      sOuterForeignKeyList,
                                      & QLL_EXEC_DDL_MEM(aEnv),
                                      & sOuterForeignKeyIndexList,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***************************************************************
     * 관련 객체에 대한 Lock 설정 
     ***************************************************************/

    /**
     * Inner Table List 에 X lock 획득
     */
    
    STL_TRY( ellLockRelatedTableList4DDL( aTransId,
                                          aStmt,
                                          SML_LOCK_X,
                                          sInnerTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    /**
     * Outer Table Inner Primary Key 의 Table List 에 X lock 획득 
     */

    STL_TRY( ellLockRelatedTableList4DDL( aTransId,
                                          aStmt,
                                          SML_LOCK_X,
                                          sOuterTableInnerPrimaryKeyTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    STL_TRY( ellLock4ConstraintList( aTransId,
                                     aStmt,
                                     sOuterTableInnerPrimaryKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    /**
     * Outer Table Inner Unique Key 의 Table List 에 S lock 획득 
     */

    STL_TRY( ellLockRelatedTableList4DDL( aTransId,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterTableInnerUniqueKeyTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    STL_TRY( ellLock4ConstraintList( aTransId,
                                     aStmt,
                                     sOuterTableInnerUniqueKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    /**
     * Outer Table Inner Index 의 Table List 에 S lock 획득 
     */

    STL_TRY( ellLockRelatedTableList4DDL( aTransId,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterTableInnerIndexTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );

    STL_TRY( ellLock4IndexList( aTransId,
                                aStmt,
                                sOuterTableInnerIndexList,
                                & sLocked,
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    /**
     * Inner Foreign Key 의 Parent Table 과 Child Table 에 S lock 획득 
     */

    STL_TRY( ellLockRelatedTableList4DDL( aTransId,
                                          aStmt,
                                          SML_LOCK_S,
                                          sInnerForeignKeyParentTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    STL_TRY( ellLockRelatedTableList4DDL( aTransId,
                                          aStmt,
                                          SML_LOCK_S,
                                          sInnerForeignKeyChildTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    STL_TRY( ellLock4ConstraintList( aTransId,
                                     aStmt,
                                     sInnerForeignKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    /**
     * Outer Foreign Key 의 Parent Table 과 Child Table 에 S lock 획득 
     */

    STL_TRY( ellLockRelatedTableList4DDL( aTransId,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterForeignKeyParentTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    STL_TRY( ellLockRelatedTableList4DDL( aTransId,
                                          aStmt,
                                          SML_LOCK_S,
                                          sOuterForeignKeyChildTableList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    STL_TRY( ellLock4ConstraintList( aTransId,
                                     aStmt,
                                     sOuterForeignKeyList,
                                     & sLocked,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    /**
     * 작업 종료
     */

    STL_RAMP( RAMP_FINISH );

    *aLockSuccess = sLocked;
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qltAlterSpaceOffline */
