/*******************************************************************************
 * qltAlterSpaceOnline.c
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
 * @file qltAlterSpaceOnline.c
 * @brief ALTER TABLESPACE .. ONOFF 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qltAlterSpaceOnline ALTER TABLESPACE .. ONLINE
 * @ingroup qltAlterSpace
 * @{
 */


/**
 * @brief ALTER TABLESPACE .. ONLINE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltValidateAlterSpaceOnline( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv )
{
    qltInitAlterSpaceOnline * sInitPlan = NULL;
    qlpAlterSpaceOnline     * sParseTree = NULL;
    stlBool                   sObjectExist = STL_FALSE;
    stlInt64                  sSpaceID;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_ONLINE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLESPACE_ONLINE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterSpaceOnline *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltInitAlterSpaceOnline),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltInitAlterSpaceOnline) );

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
     * Tablespce Online 검증
     */
    
    sSpaceID = ellGetTablespaceID( &sInitPlan->mSpaceHandle );
                                    
    STL_TRY_THROW( smlIsOnlineTablespace( sSpaceID ) == STL_FALSE,
                   RAMP_ERR_NOT_OFFLINE_TABLESPACE );

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSpaceHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
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

    STL_CATCH( RAMP_ERR_NOT_OFFLINE_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_OFFLINE_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief ALTER TABLESPACE .. ONOFF 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptCodeAlterSpaceOnline( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. ONOFF 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptDataAlterSpaceOnline( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. ONOFF 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltExecuteAlterSpaceOnline( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv )
{
    qltInitAlterSpaceOnline * sInitPlan = NULL;
    stlInt64                  sSpaceID;
    stlTime                   sTime = 0;
    stlInt64                  sPendingRelationId;
    ellObjectList           * sInnerTableIndexObjList;
    ellDictHandle           * sObjectHandle = NULL;
    ellObjectItem           * sObjectItem = NULL;
    ellDictHandle             sIndexSpaceHandle;
    stlBool                   sExist = STL_TRUE;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_ONLINE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * 기본 정보 설정 
     ***************************************************************/

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

    sInitPlan = (qltInitAlterSpaceOnline *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( qltLock4AlterSpaceOnOffline( aTransID,
                                          aStmt,
                                          aDBCStmt,
                                          & sInitPlan->mSpaceHandle,
                                          & sLocked,
                                          aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );

    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qltInitAlterSpaceOnline *) aSQLStmt->mInitPlan;

    /***************************************************************
     * execute ONLINE
     ***************************************************************/
    
    sSpaceID = ellGetTablespaceID( & sInitPlan->mSpaceHandle );
    
    STL_TRY_THROW( smlIsOnlineTablespace( sSpaceID ) == STL_FALSE,
                   RAMP_ERR_NOT_OFFLINE_TABLESPACE );
    
    STL_TRY( smlOnlineTablespace( aStmt,
                                  sSpaceID,
                                  &sPendingRelationId,
                                  SML_ENV(aEnv) )
             == STL_SUCCESS );
                                   
    /***************************************************************
     * ALTER 수행시간을 기록 
     ***************************************************************/

    STL_TRY( ellSetTimeAlterTablespace( aTransID,
                                        aStmt,
                                        sSpaceID,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 과서 OFFLINE 당시 남았던 pending operation을 수행한다.
     * - smlFinalizeOnlineTablespace()이 호출된 이후에는 실패해서는 안된다.
     */
    
    STL_TRY( smlFinalizeOnlineTablespace( aStmt,
                                          sSpaceID,
                                          sPendingRelationId,
                                          SML_ENV(aEnv) )
             == STL_SUCCESS );
                                        
    KNL_BREAKPOINT( KNL_BREAKPOINT_QLTEXECUTEALTERSPACEONLINE_AFTER_SMLFINALIZEONLINETABLESPACE,
                    KNL_ENV(aEnv) );
                                   
    /**
     * Unusable nologging index를 usable 상태로 만든다
     */

    STL_TRY( ellGetIndexObjectList4SpaceOnline( aTransID,
                                                aStmt,
                                                &(sInitPlan->mSpaceHandle),
                                                &QLL_EXEC_DDL_MEM(aEnv),
                                                &sInnerTableIndexObjList,
                                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_RING_FOREACH_ENTRY( & sInnerTableIndexObjList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( ellGetTablespaceDictHandleByID( aTransID,
                                                 aSQLStmt->mViewSCN,
                                                 ellGetIndexTablespaceID( sObjectHandle ),
                                                 &sIndexSpaceHandle,
                                                 &sExist,
                                                 ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_DASSERT( sExist == STL_TRUE );

        /**
         * Startup 시 Build 되지 않았던 인덱스만 Build 한다.
         */
        if( ellGetTablespaceUsageType( &sIndexSpaceHandle ) != ELL_SPACE_USAGE_TYPE_TEMPORARY )
        {
            continue;
        }
            
        STL_TRY( smlBuildUnusableIndex( aStmt,
                                        ellGetIndexHandle( sObjectHandle ),
                                        SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }                                        

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLTEXECUTEALTERSPACEONLINE_AFTER_SMLONLINETABLESPACE,
                    KNL_ENV(aEnv) );
                                   
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_OFFLINE_TABLESPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_OFFLINE_TABLESPACE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}




/** @} qltAlterSpaceOnline */
