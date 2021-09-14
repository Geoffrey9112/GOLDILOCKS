/*******************************************************************************
 * qlyDropSynonym.c
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
 * @file qlyDropSynonym.c
 * @brief DROP SYNONYM 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlyDropSynonym DROP SYNONYM
 * @ingroup qly
 * @{
 */

/**
 * @brief DROP SYNONYM 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlyValidateDropSynonym( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{
    ellDictHandle       * sAuthHandle = NULL;
    
    qlyInitDropSynonym  * sInitPlan = NULL;
    qlpDropSynonym      * sParseTree = NULL;

    stlBool               sObjectExist = STL_FALSE;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_SYNONYM_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DROP_SYNONYM_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    sParseTree = (qlpDropSynonym *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlyInitDropSynonym),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlyInitDropSynonym) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Synonym 정보 획득 
     **********************************************************/

    sInitPlan->mIsPublic = sParseTree->mIsPublic;
    sInitPlan->mIfExists = sParseTree->mIfExists;


    /**********************************************************
     * Synonym Name Validation
     **********************************************************/
    
    if ( sParseTree->mIsPublic == STL_TRUE )
    {
        /**
         * PUBLIC SYNONYM
         */
        
        STL_TRY( ellGetPublicSynonymDictHandle( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sParseTree->mSynonymName->mObject,
                                                & sInitPlan->mSynonymHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /**
             * Synonym 이 존재함
             */
            
            sInitPlan->mSynonymExist = STL_TRUE;
        }
        else
        {
            /**
             * Synonym 이 없음 
             */

            sInitPlan->mSynonymExist = STL_FALSE;

            /**
             * If Exists 옵션이 있다면 무조건 SUCCESS
             */
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_SYNONYM );
            }
            else
            {
                STL_THROW( RAMP_ERR_PUBLIC_SYNONYM_NOT_EXIST );
            }
        }

        /**
         * DROP PUBLIC SYNONYM 권한 검사
         */
        
        STL_TRY( qlaCheckDatabasePriv( aTransID,
                                       aDBCStmt,
                                       aSQLStmt,
                                       ELL_DB_PRIV_ACTION_DROP_PUBLIC_SYNONYM,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * PRIVATE SYNONYM
         */

        if ( sParseTree->mSynonymName->mSchema == NULL )
        {
            /**
             * Schema Name 이 명시되지 않은 경우
             */

            STL_TRY( ellGetSynonymDictHandleWithAuth( aTransID,
                                                      aSQLStmt->mViewSCN,
                                                      sAuthHandle,
                                                      sParseTree->mSynonymName->mObject,
                                                      & sInitPlan->mSynonymHandle,
                                                      & sObjectExist,
                                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );

            if ( sObjectExist == STL_TRUE )
            {
                /**
                 * Synonym 이 존재함
                 */
                
                sInitPlan->mSynonymExist = STL_TRUE;
            }
            else
            {
                /**
                 * Table 이 없음
                 */
                
                sInitPlan->mSynonymExist = STL_FALSE;
                
                /**
                 * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
                 */
            
                if ( sInitPlan->mIfExists == STL_TRUE )
                {
                    STL_THROW( RAMP_IF_EXISTS_NO_SYNONYM );
                }
                else
                {
                    STL_THROW( RAMP_ERR_PRIVATE_SYNONYM_NOT_EXIST );
                }
            }

            /**
             * Synonym 의 Schema Handle 획득
             */
            
            STL_TRY( ellGetSchemaDictHandleByID(
                         aTransID,
                         aSQLStmt->mViewSCN,
                         ellGetSynonymSchemaID( & sInitPlan->mSynonymHandle ),
                         & sInitPlan->mSchemaHandle,
                         & sObjectExist,
                         ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            if ( sObjectExist == STL_TRUE )
            {
                /* Schema 가 존재함 */
            }
            else
            {
                /**
                 * Schema 가 존재하지 않을 경우,
                 * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
                 */
                
                sInitPlan->mSynonymExist = STL_FALSE;
                
                if ( sInitPlan->mIfExists == STL_TRUE )
                {
                    STL_THROW( RAMP_IF_EXISTS_NO_SYNONYM );
                }
                else
                {
                    STL_THROW( RAMP_ERR_SCHEMA_NOT_EXISTS );
                }
            }
        }
        else
        {
            /**
             * Schema Name 이 명시된 경우 
             */

            STL_TRY( ellGetSchemaDictHandle( aTransID,
                                             aSQLStmt->mViewSCN,
                                             sParseTree->mSynonymName->mSchema,
                                             & sInitPlan->mSchemaHandle,
                                             & sObjectExist,
                                             ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            if ( sObjectExist == STL_TRUE )
            {
                /* Schema 가 존재함 */
            }
            else
            {
                /**
                 * Schema 가 존재하지 않을 경우,
                 * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
                 */
                
                sInitPlan->mSynonymExist = STL_FALSE;
                
                if ( sInitPlan->mIfExists == STL_TRUE )
                {
                    STL_THROW( RAMP_IF_EXISTS_NO_SYNONYM );
                }
                else
                {
                    STL_THROW( RAMP_ERR_SCHEMA_NOT_EXISTS );
                }
            }

            STL_TRY( ellGetSynonymDictHandleWithSchema( aTransID,
                                                        aSQLStmt->mViewSCN,
                                                        & sInitPlan->mSchemaHandle,
                                                        sParseTree->mSynonymName->mObject,
                                                        & sInitPlan->mSynonymHandle,
                                                        & sObjectExist,
                                                        ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            if ( sObjectExist == STL_TRUE )
            {
                /**
                 * Synonym 이 존재함
                 */
                
                sInitPlan->mSynonymExist = STL_TRUE;
            }
            else
            {
                /**
                 * Synonym 이 없음 
                 */
                
                sInitPlan->mSynonymExist = STL_FALSE;
                
                /**
                 * If Exists 옵션이 있다면 무조건 SUCCESS
                 */
                
                if ( sInitPlan->mIfExists == STL_TRUE )
                {
                    STL_THROW( RAMP_IF_EXISTS_NO_SYNONYM );
                }
                else
                {
                    STL_THROW( RAMP_ERR_PRIVATE_SYNONYM_NOT_EXIST );
                }
            }
        }
        
        /**
         * DROP SYNONYM 권한 검사
         */
        
        STL_TRY( qlaCheckPrivDropSynonym( aTransID,
                                          aDBCStmt,
                                          aSQLStmt,
                                          & sInitPlan->mSynonymHandle,
                                          aEnv )
                 == STL_SUCCESS );
        
    }
    
    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSynonymHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/

     /**
     * IF EXISTS 구문이 있을 경우, Synonym이 존재하지 않는다면,
     * 수행없이 SUCCESS 하게 되고, 권한 검사 및 Writable 은 검사하지 않는다.
     */
    
    STL_RAMP( RAMP_IF_EXISTS_NO_SYNONYM );

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PRIVATE_SYNONYM_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PRIVATE_SYNONYM_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mSynonymName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mSynonymName->mObject );
    }
               
    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mSynonymName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mSynonymName->mSchema );
    }

    STL_CATCH( RAMP_ERR_PUBLIC_SYNONYM_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PUBLIC_SYNONYM_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mSynonymName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mSynonymName->mObject );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief DROP SYNONYM 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlyOptCodeDropSynonym( smlTransId      aTransID,
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
 * @brief DROP SYNONYM 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlyOptDataDropSynonym( smlTransId      aTransID,
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
 * @brief DROP SYNONYM 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlyExecuteDropSynonym( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    qlyInitDropSynonym * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_SYNONYM_TYPE,
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

    sInitPlan = (qlyInitDropSynonym *) aSQLStmt->mInitPlan;

    /**
     * IF EXISTS 옵션 처리
     */

    if ( (sInitPlan->mIfExists == STL_TRUE) &&
         (sInitPlan->mSynonymExist != STL_TRUE) )
    {
        /**
         * IF EXISTS 옵션을 사용하고, Synonym 이 존재하지 않는 경우
         */
        
        STL_THROW( RAMP_IF_EXISTS_SUCCESS );
    }
    else
    {
        /* go go */
    }

    /**
     * DDL Lock 획득 
     */

    if ( sInitPlan->mIsPublic == STL_TRUE )
    {
        STL_TRY( ellLock4DropPublicSynonym( aTransID,
                                            aStmt,
                                            & sInitPlan->mSynonymHandle,
                                            & sLocked,
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ellLock4DropSynonym( aTransID,
                                      aStmt,
                                      & sInitPlan->mSynonymHandle,
                                      & sLocked,
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

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

    sInitPlan = (qlyInitDropSynonym *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Synonym과 관련된 Dictionary 제거 
     ***************************************************************/

    /**
     * Dictionary Row 를 제거
     */

    if ( sInitPlan->mIsPublic == STL_TRUE )
    {
        STL_TRY( ellRemoveDict4DropPublicSynonym( aTransID,
                                                  aStmt,
                                                  & sInitPlan->mSynonymHandle,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * Cache 재구성
         */
        
        STL_TRY( ellRefineCache4DropPublicSynonym( aTransID,
                                                   aStmt,
                                                   & sInitPlan->mSynonymHandle,
                                                   ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        
        STL_TRY( ellRemoveDict4DropSynonym( aTransID,
                                            aStmt,
                                            & sInitPlan->mSynonymHandle,
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * Cache 재구성
         */
        
        STL_TRY( ellRefineCache4DropSynonym( aTransID,
                                             aStmt,
                                             & sInitPlan->mSynonymHandle,
                                             ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /**
     * If Exists 옵션을 사용하고, Synonym 이 존재하지 않는 경우
     */
    
    STL_RAMP( RAMP_IF_EXISTS_SUCCESS );

                 
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlyDropSynonym */
