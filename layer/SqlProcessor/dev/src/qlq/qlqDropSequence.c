/*******************************************************************************
 * qlqDropSequence.c
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
 * @file qlqDropSequence.c
 * @brief DROP SEQUENCE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlqDropSequence DROP SEQUENCE
 * @ingroup qlq
 * @{
 */


/**
 * @brief DROP SEQUECE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlqValidateDropSeq( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              stlChar       * aSQLString,
                              qllNode       * aParseTree,
                              qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlqInitDropSeq  * sInitPlan = NULL;
    qlpDropSequence * sParseTree = NULL;

    stlBool   sObjectExist = STL_FALSE;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_SEQUENCE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DROP_SEQUENCE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    sParseTree = (qlpDropSequence *) aParseTree;
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlqInitDropSeq),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlqInitDropSeq) );

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
     * Sequence Name Validation
     **********************************************************/
    
    if ( sParseTree->mName->mSchema == NULL )
    {
        /**
         * Schema 가 명시되지 않은 경우
         */

        STL_TRY( ellGetSequenceDictHandleWithAuth( aTransID,
                                                   aSQLStmt->mViewSCN,
                                                   sAuthHandle,
                                                   sParseTree->mName->mObject,
                                                   & sInitPlan->mSeqHandle,
                                                   & sObjectExist,
                                                   ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /**
             * Sequence 가 존재함
             */
            
            sInitPlan->mSeqExist = STL_TRUE;
        }
        else
        {
            /**
             * Sequence 가 없음 
             */
            
            sInitPlan->mSeqExist = STL_FALSE;
            
            /**
             * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
             */
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_SEQ );
            }
            else
            {
                STL_THROW( RAMP_ERR_SEQ_NOT_EXIST );
            }
        }

        /**
         * Sequence 의 Schema Handle 획득
         */
        
        STL_TRY( ellGetSchemaDictHandleByID(
                     aTransID,
                     aSQLStmt->mViewSCN,
                     ellGetSequenceSchemaID( & sInitPlan->mSeqHandle ),
                     & sInitPlan->mSchemaHandle,
                     & sObjectExist,
                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SEQ_NOT_EXIST );
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

            sInitPlan->mSeqExist = STL_FALSE;
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_SEQ );
            }
            else
            {
                STL_THROW( RAMP_ERR_SCHEMA_NOT_EXIST );
            }
        }
        
        STL_TRY( ellGetSequenceDictHandleWithSchema( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     & sInitPlan->mSchemaHandle,
                                                     sParseTree->mName->mObject,
                                                     & sInitPlan->mSeqHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sObjectExist == STL_TRUE )
        {
            /**
             * Sequence 가 존재함
             */
            
            sInitPlan->mSeqExist = STL_TRUE;
        }
        else
        {
            /**
             * Sequence 가 없음
             */
            
            sInitPlan->mSeqExist = STL_FALSE;
            
            /**
             * If Exists 옵션이 있다면 무조건 SUCCESS 한다.
             */
            
            if ( sInitPlan->mIfExists == STL_TRUE )
            {
                STL_THROW( RAMP_IF_EXISTS_NO_SEQ );
            }
            else
            {
                STL_THROW( RAMP_ERR_SEQ_NOT_EXIST );
            }
        }
    }

    /**
     * DROP SEQUENCE 권한 검사
     */

    STL_TRY( qlaCheckPrivDropSequence( aTransID,
                                       aDBCStmt,
                                       aSQLStmt,
                                       & sInitPlan->mSeqHandle,
                                       aEnv )
             == STL_SUCCESS );

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSeqHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/
    
    /**
     * IF EXISTS 구문이 있을 경우, Sequence 가 존재하지 않는다면,
     * 수행없이 SUCCESS 하게 되고, 권한 검사 및 Writable 은 검사하지 않는다.
     */
    
    STL_RAMP( RAMP_IF_EXISTS_NO_SEQ );
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SEQ_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_NOT_EXIST,
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

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief DROP SEQUENCE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlqOptCodeDropSeq( smlTransId      aTransID,
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
 * @brief DROP SEQUENCE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlqOptDataDropSeq( smlTransId      aTransID,
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
 * @brief DROP SEQUENCE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlqExecuteDropSeq( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllEnv        * aEnv )
{
    qlqInitDropSeq * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DROP_SEQUENCE_TYPE,
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

    sInitPlan = (qlqInitDropSeq *) aSQLStmt->mInitPlan;

    /**
     * IF EXISTS 옵션 처리
     */

    if ( (sInitPlan->mIfExists == STL_TRUE) &&
         (sInitPlan->mSeqExist != STL_TRUE) )
    {
        /**
         * IF EXISTS 옵션을 사용하고, Sequence 가 존재하지 않는 경우
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

    STL_TRY( ellLock4DropSequence( aTransID,
                                   aStmt,
                                   & sInitPlan->mSeqHandle,
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

    sInitPlan = (qlqInitDropSeq *) aSQLStmt->mInitPlan;
    
    /**
     * Sequence 와 관련된 Object 목록 획득
     * - 현재로선 없어 보임.
     */

    /**
     * Drop Behavior 의 적합성 검사
     * - 현재로선 유형별 차이가 없어 보임.
     */

    /***************************************************************
     * 시퀀스 제거
     ***************************************************************/
    
    STL_TRY( smlDropSequence( aStmt,
                              ellGetSequenceHandle( & sInitPlan->mSeqHandle ),
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     * Sequence 와 관련된 Dictionary 제거 
     ***************************************************************/

    /**
     * Dictionary Row 를 제거
     */
    
    STL_TRY( ellRemoveDict4DropSequence( aTransID,
                                         aStmt,
                                         & sInitPlan->mSeqHandle,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Cache 재구성
     */

    STL_TRY( ellRefineCache4DropSequence( aTransID,
                                          aStmt,
                                          & sInitPlan->mSeqHandle,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * If Exists 옵션을 사용하고, Sequence 가 존재하지 않는 경우
     */
    
    STL_RAMP( RAMP_IF_EXISTS_SUCCESS );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    

/**
 * @brief Sequence List 로부터 Sequence 객체를 제거한다.
 * @param[in] aStmt        Statement
 * @param[in] aSeqList     Sequence List
 * @param[in] aEnv         Environment
 * @remarks
 */
stlStatus qlqDropSequence4SeqList( smlStatement  * aStmt,
                                   ellObjectList * aSeqList,
                                   qllEnv        * aEnv )
{
    ellObjectItem   * sObjectItem   = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Sequence List 를 순회하며 Sequence 객체를 삭제한다.
     */
    
    STL_RING_FOREACH_ENTRY( & aSeqList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( smlDropSequence( aStmt,
                                  ellGetSequenceHandle( sObjectHandle ),
                                  SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlqDropSequence */
