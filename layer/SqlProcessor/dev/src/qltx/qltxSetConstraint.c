/*******************************************************************************
 * qltxSetConstraint.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qltxSetConstraint.c 14792 2015-04-16 09:04:27Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qltxSetConstraint.c
 * @brief SET CONSTRAINTS 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qltxSetConstraint SET CONSTRAINTS
 * @ingroup qltx
 * @{
 */


/**
 * @brief SET CONSTRAINTS 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxValidateSetConstraint( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     stlChar       * aSQLString,
                                     qllNode       * aParseTree,
                                     qllEnv        * aEnv )
{
    qlpSetConstraint      * sParseTree = NULL;
    qltxInitSetConstraint * sInitPlan = NULL;

    qlpListElement    * sListElem  = NULL;
    qlpObjectName     * sObjectName = NULL;

    ellDictHandle     * sAuthHandle = NULL;
    ellDictHandle       sConstHandle;
    ellDictHandle       sSchemaHandle;
    stlBool             sObjectExist = STL_FALSE;
    stlBool             sDuplicate = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_SET_CONSTRAINTS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_SET_CONSTRAINTS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpSetConstraint *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltxInitSetConstraint),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltxInitSetConstraint) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );

    /***********************************************************
     * SET CONSTRAINT 정보 획득
     ***********************************************************/

    sInitPlan->mSetMode = sParseTree->mConstMode;

    /***********************************************************
     * Constraint 정보 획득
     ***********************************************************/

    /**
     * Parameter Validation
     */
    
    switch ( sInitPlan->mSetMode )
    {
        case QLL_SET_CONSTRAINT_MODE_ALL_IMMEDIATE:
        case QLL_SET_CONSTRAINT_MODE_ALL_DEFERRED:
            {
                STL_DASSERT( sParseTree->mConstNameList == NULL );
                break;
            }
        case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_IMMEDIATE:
        case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_DEFERRED:
            {
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }

    /**
     * Constraint Name 을 명시한 경우, Constraint 정보를 획득한다.
     */
    
    if ( sParseTree->mConstNameList == NULL )
    {
        sInitPlan->mConstraintList = NULL;
    }
    else
    {
        sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
        
        STL_TRY( ellInitObjectList( & sInitPlan->mConstraintList,
                                    QLL_INIT_PLAN(aDBCStmt),
                                    ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLP_LIST_FOR_EACH( sParseTree->mConstNameList, sListElem )
        {
            sObjectName = (qlpObjectName *) QLP_LIST_GET_POINTER_VALUE( sListElem );

            /**
             * Constraint Handle 획득
             */
            
            if ( sObjectName->mSchema == NULL )
            {
                /**
                 * Schema Name 이 없는 경우
                 */
                
                STL_TRY( ellGetConstraintDictHandleWithAuth( aTransID,
                                                             aSQLStmt->mViewSCN,
                                                             sAuthHandle,
                                                             sObjectName->mObject,
                                                             & sConstHandle,
                                                             & sObjectExist,
                                                             ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_CONSTRAINT_NOT_EXISTS );
            }
            else
            {
                /**
                 * Schema Name 이 명시된 경우
                 */
                
                STL_TRY( ellGetSchemaDictHandle( aTransID,
                                                 aSQLStmt->mViewSCN,
                                                 sObjectName->mSchema,
                                                 & sSchemaHandle,
                                                 & sObjectExist,
                                                 ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXISTS );

                
                STL_TRY( ellGetConstraintDictHandleWithSchema( aTransID,
                                                               aSQLStmt->mViewSCN,
                                                               & sSchemaHandle,
                                                               sObjectName->mObject,
                                                               & sConstHandle,
                                                               & sObjectExist,
                                                               ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_CONSTRAINT_NOT_EXISTS );
            }

            /**
             * Constraint 가 DEFERRABLE 인지 검사
             */

            STL_TRY_THROW( ellGetConstraintAttrIsDeferrable( & sConstHandle ) == STL_TRUE,
                           RAMP_ERR_NOT_DEFERRABLE );

            /**
             * Constraint 목록에 추가 
             */

            STL_TRY( ellAddNewObjectItem( sInitPlan->mConstraintList,
                                          & sConstHandle,
                                          & sDuplicate,
                                          QLL_INIT_PLAN(aDBCStmt),
                                          ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sDuplicate == STL_FALSE, RAMP_ERR_DUPLICATE );
        }
    }
    
    /********************************************************
     * 권한 검사 
     ********************************************************/

    /**
     * SET CONSTRAINTS 구문은 권한 검사를 하지 않는다.
     */
    
    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    if ( sInitPlan->mConstraintList != NULL )
    {
        STL_TRY( ellAppendObjectList( sInitPlan->mCommonInit.mValidationObjList,
                                      sInitPlan->mConstraintList,
                                      QLL_INIT_PLAN(aDBCStmt),
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }

    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mSchema );
    }

    STL_CATCH( RAMP_ERR_CONSTRAINT_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CONSTRAINT_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mObject );
    }

    STL_CATCH( RAMP_ERR_NOT_DEFERRABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CONSTRAINT_IS_NOT_DEFERRABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mObject );
    }

    STL_CATCH( RAMP_ERR_DUPLICATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_CONSTRAINT_NAME_SPECIFIED,
                      qlgMakeErrPosString( aSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief SET CONSTRAINTS 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxOptCodeSetConstraint( smlTransId      aTransID,
                                    qllDBCStmt    * aDBCStmt,
                                    qllStatement  * aSQLStmt,
                                    qllEnv        * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief SET CONSTRAINTS 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltxOptDataSetConstraint( smlTransId      aTransID,
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
 * @brief SET CONSTRAINTS 를 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltxExecuteSetConstraint( smlTransId      aTransID,
                                    smlStatement  * aStmt,
                                    qllDBCStmt    * aDBCStmt,
                                    qllStatement  * aSQLStmt,
                                    qllEnv        * aEnv )
{
    qltxInitSetConstraint * sInitPlan = NULL;

    ellObjectItem * sObjectItem = NULL;
    
    /***************************************************************
     *  Run-Time Validation
     ***************************************************************/

    /**
     * Valid Plan 획득
     */

    /* Control Language 이나 DDL 과 별 차이 없음 */
    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qltxInitSetConstraint *) aSQLStmt->mInitPlan;

    /***************************************************************
     * IMMEDIATE 인 경우 제약조건을 위배하고 있는지 검사
     ***************************************************************/

    switch ( sInitPlan->mSetMode )
    {
        case QLL_SET_CONSTRAINT_MODE_ALL_IMMEDIATE:
            {
                STL_TRY_THROW( qlrcHasViolatedDeferredConstraint( aEnv ) == STL_FALSE,
                               RAMP_ERR_ALL_INTEGRITY_CONSTRAINT_VIOLATION );
                break;
            }
        case QLL_SET_CONSTRAINT_MODE_ALL_DEFERRED:
            {
                /* nothing to do */
                break;
            }
        case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_IMMEDIATE:
            {
                STL_RING_FOREACH_ENTRY( & sInitPlan->mConstraintList->mHeadList, sObjectItem )
                {
                    STL_TRY_THROW( qlrcIsViolatedConstraint( & sObjectItem->mObjectHandle, aEnv ) == STL_FALSE,
                                   RAMP_ERR_SPECIFIC_INTEGRITY_CONSTRAINT_VIOLATION );
                }
                
                break;
            }
        case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_DEFERRED:
            {
                /* nothing to do */
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    /***************************************************************
     * SET CONSTRAINT 를 등록
     ***************************************************************/

    switch ( sInitPlan->mSetMode )
    {
        case QLL_SET_CONSTRAINT_MODE_ALL_IMMEDIATE:
        case QLL_SET_CONSTRAINT_MODE_ALL_DEFERRED:
            {
                STL_TRY( qlrcAddSetConstraintMode( sInitPlan->mSetMode,
                                                   NULL, /* aConstraintHandle */
                                                   aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_IMMEDIATE:
        case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_DEFERRED:
            {
                STL_RING_FOREACH_ENTRY( & sInitPlan->mConstraintList->mHeadList, sObjectItem )
                {
                    STL_TRY( qlrcAddSetConstraintMode( sInitPlan->mSetMode,
                                                       & sObjectItem->mObjectHandle,
                                                       aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ALL_INTEGRITY_CONSTRAINT_VIOLATION )
    {
        stlChar sErrMsg[STL_MAX_ERROR_MESSAGE] = {0,};

        (void) qlrcSetViolatedHandleMessage( aTransID,
                                             sErrMsg,
                                             STL_MAX_ERROR_MESSAGE,
                                             NULL, /* aConstHandle */
                                             aEnv );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION,
                      sErrMsg,
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_SPECIFIC_INTEGRITY_CONSTRAINT_VIOLATION )
    {
        stlChar sErrMsg[STL_MAX_ERROR_MESSAGE] = {0,};

        (void) qlrcSetViolatedHandleMessage( aTransID,
                                             sErrMsg,
                                             STL_MAX_ERROR_MESSAGE,
                                             & sObjectItem->mObjectHandle,
                                             aEnv );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION,
                      sErrMsg,
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/** @} qltxSetConstraint */
