/*******************************************************************************
 * qliDropIndex.c
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
 * @file qliDropIndex.c
 * @brief DROP INDEX 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qliDropIndex DROP INDEX
 * @ingroup qli
 * @{
 */


/**
 * @brief DROP INDEX 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qliValidateDropIndex( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qliInitDropIndex * sInitPlan = NULL;
    qlpDropIndex     * sParseTree = NULL;

    stlBool   sObjectExist = STL_FALSE;

    stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_INDEX_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DROP_INDEX_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    sParseTree = (qlpDropIndex *) aParseTree;
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qliInitDropIndex),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qliInitDropIndex) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * mIfExists
     */

    sInitPlan->mIfExists = sParseTree->mIfExists;
    
    /**********************************************************
     * Index Name Validation
     **********************************************************/
    
    if ( sParseTree->mName->mSchema == NULL )
    {
        /**
         * Schema 가 명시되지 않은 경우
         */

        STL_TRY( ellGetIndexDictHandleWithAuth( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sAuthHandle,
                                                sParseTree->mName->mObject,
                                                & sInitPlan->mIndexHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /**
             * Index 가 존재함
             */
            
            sInitPlan->mIndexExist = STL_TRUE;
        }
        else
        {
            /**
             * Index 가 없음 
             */
            
            sInitPlan->mIndexExist = STL_FALSE;
            
            /**
             * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
             */
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_INDEX );
            }
            else
            {
                STL_THROW( RAMP_ERR_INDEX_NOT_EXIST );
            }
        }

        /**
         * Sequence 의 Schema Handle 획득
         */
        
        STL_TRY( ellGetSchemaDictHandleByID(
                     aTransID,
                     aSQLStmt->mViewSCN,
                     ellGetIndexSchemaID( & sInitPlan->mIndexHandle ),
                     & sInitPlan->mSchemaHandle,
                     & sObjectExist,
                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_INDEX_NOT_EXIST );
    }
    else
    {
        /**
         * Schema 가 명시된 경우
         */

        STL_TRY( ellGetSchemaDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sParseTree->mName->mSchema,
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

            sInitPlan->mIndexExist = STL_FALSE;
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_INDEX );
            }
            else
            {
                STL_THROW( RAMP_ERR_SCHEMA_NOT_EXIST );
            }
        }
        
        STL_TRY( ellGetIndexDictHandleWithSchema( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  & sInitPlan->mSchemaHandle,
                                                  sParseTree->mName->mObject,
                                                  & sInitPlan->mIndexHandle,
                                                  & sObjectExist,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /**
             * Index 가 존재함
             */
            
            sInitPlan->mIndexExist = STL_TRUE;
        }
        else
        {
            /**
             * Index 가 없음
             */
            
            sInitPlan->mIndexExist = STL_FALSE;
            
            /**
             * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
             */
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_INDEX );
            }
            else
            {
                STL_THROW( RAMP_ERR_INDEX_NOT_EXIST );
            }
        }
    }

    /**
     * Built-In Index 인지 검사
     */

    STL_TRY_THROW( ellIsBuiltInIndex( & sInitPlan->mIndexHandle ) != STL_TRUE,
                   RAMP_ERR_BUILT_IN_INDEX );
    
    /**
     * Index 가 Key Constraint 에 의해 자동 생성된 것인지를 검사
     */

    STL_TRY_THROW( ellGetIndexIsByConstraint( & sInitPlan->mIndexHandle ) != STL_TRUE,
                   RAMP_ERR_INDEX_WITH_KEY_CONSTRAINT );

    /**
     * DROP INDEX 권한 검사
     */

    STL_TRY( qlaCheckPrivDropIndex( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    & sInitPlan->mIndexHandle,
                                    aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * 관련 객체 정보 획득 
     **********************************************************/
    
    /**
     * Table Handle 정보 획득
     */

    sTableID = ellGetIndexTableID( & sInitPlan->mIndexHandle );

    STL_TRY( ellGetTableDictHandleByID( aTransID,
                                        aSQLStmt->mViewSCN,
                                        sTableID,
                                        & sInitPlan->mTableHandle,
                                        & sObjectExist,
                                        ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_INDEX_NOT_EXIST );

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSchemaHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mTableHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**********************************************************
     * Init Plan 연결 
     **********************************************************/
    
    /**
     * IF EXISTS 구문이 있을 경우, Index 가 존재하지 않는다면,
     * 수행없이 SUCCESS 하게 되고, 권한 검사 및 Writable 은 검사하지 않는다.
     */
    
    STL_RAMP( RAMP_IF_EXISTS_NO_INDEX );
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INDEX_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INDEX_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mName->mSchema );
    }

    STL_CATCH( RAMP_ERR_BUILT_IN_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BUILT_IN_INDEX_IS_NOT_MODIFIABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mName->mObject );
    }

    STL_CATCH( RAMP_ERR_INDEX_WITH_KEY_CONSTRAINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INDEX_WITH_KEY_CONSTRAINT,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief DROP INDEX 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qliOptCodeDropIndex( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief DROP INDEX 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qliOptDataDropIndex( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    return STL_SUCCESS;
}
    

    

/**
 * @brief DROP INDEX 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qliExecuteDropIndex( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    qliInitDropIndex * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_INDEX_TYPE,
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
    
    sInitPlan = (qliInitDropIndex *) aSQLStmt->mInitPlan;

    /**
     * IF EXISTS 옵션 처리
     */

    if ( (sInitPlan->mIfExists == STL_TRUE) &&
         (sInitPlan->mIndexExist != STL_TRUE) )
    {
        /**
         * IF EXISTS 옵션을 사용하고, Index 가 존재하지 않는 경우
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

    STL_TRY( ellLock4DropIndex( aTransID,
                                aStmt,
                                & sInitPlan->mTableHandle,
                                & sInitPlan->mIndexHandle,
                                & sLocked,
                                ELL_ENV(aEnv) )
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
    
    sInitPlan = (qliInitDropIndex *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Index 제거
     ***************************************************************/

    STL_TRY( smlDropIndex( aStmt,
                           ellGetIndexHandle( & sInitPlan->mIndexHandle ),
                           SML_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * Index 와 관련된 Dictionary 제거 
     ***************************************************************/

    /**
     * Dictionary Row 를 제거
     */
    
    STL_TRY( ellDeleteDict4DropIndex( aTransID,
                                      aStmt,
                                      & sInitPlan->mIndexHandle,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Cache 를 재구성 
     */

    STL_TRY( ellRefineCache4DropIndex( aTransID,
                                      & sInitPlan->mTableHandle,
                                      & sInitPlan->mIndexHandle,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * If Exists 옵션을 사용하고, Index 가 존재하지 않는 경우
     */
    
    STL_RAMP( RAMP_IF_EXISTS_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index List 로부터 Index 객체를 제거한다.
 * @param[in] aStmt        Statement
 * @param[in] aIndexList   Index List
 * @param[in] aEnv         Environment
 * @remarks
 */
stlStatus qliDropIndexes4IndexList( smlStatement  * aStmt,
                                    ellObjectList * aIndexList,
                                    qllEnv        * aEnv )
{
    ellObjectItem   * sObjectItem   = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Index List 를 순회하며 Index 객체를 삭제한다.
     */
    
    STL_RING_FOREACH_ENTRY( & aIndexList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( smlDropIndex( aStmt,
                               ellGetIndexHandle(sObjectHandle),
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index List 삭제에 대한 Cache 를 재구성한다.
 * @param[in] aTransID           Transaction ID
 * @param[in] aStmt              Statement
 * @param[in] aIndexList         Index List
 * @param[in] aIndexTableList    Index Table List
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus qliRefineCache4DropIndexList( smlTransId         aTransID,
                                        smlStatement     * aStmt,
                                        ellObjectList    * aIndexList,
                                        ellObjectList    * aIndexTableList,
                                        qllEnv           * aEnv )
{
    ellObjectItem   * sIndexItem   = NULL;
    ellDictHandle   * sIndexHandle = NULL;
    
    ellObjectItem   * sTableItem   = NULL;
    ellDictHandle   * sTableHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexTableList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Key List 를 순회하며, Cache 를 Refine 한다.
     */
    
    sTableItem = STL_RING_GET_FIRST_DATA( & aIndexTableList->mHeadList );
    
    STL_RING_FOREACH_ENTRY( & aIndexList->mHeadList, sIndexItem )
    {
        sIndexHandle = & sIndexItem->mObjectHandle;

        sTableHandle = & sTableItem->mObjectHandle;

        /**
         * Drop Index 를 위한 Refine Cache
         */
        
        STL_TRY( ellRefineCache4DropIndex( aTransID,
                                           sTableHandle,
                                           sIndexHandle,
                                           ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sTableItem = STL_RING_GET_NEXT_DATA( & aIndexTableList->mHeadList, sTableItem );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/** @} qliDropIndex */
