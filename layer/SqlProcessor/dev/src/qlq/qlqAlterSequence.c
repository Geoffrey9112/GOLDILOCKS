/*******************************************************************************
 * qlqAlterSequence.c
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
 * @file qlqAlterSequence.c
 * @brief ALTER SEQUENCE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlqAlterSequence ALTER SEQUENCE
 * @ingroup qlq
 * @{
 */


/**
 * @brief ALTER SEQUECE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlqValidateAlterSeq( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               stlChar       * aSQLString,
                               qllNode       * aParseTree,
                               qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlqInitAlterSeq  * sInitPlan = NULL;
    qlpAlterSequence * sParseTree = NULL;

    stlBool   sObjectExist = STL_FALSE;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SEQUENCE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_SEQUENCE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    sParseTree = (qlpAlterSequence *) aParseTree;
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlqInitAlterSeq),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlqInitAlterSeq) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
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

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SEQ_NOT_EXIST );

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

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXIST );

        STL_TRY( ellGetSequenceDictHandleWithSchema( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     & sInitPlan->mSchemaHandle,
                                                     sParseTree->mName->mObject,
                                                     & sInitPlan->mSeqHandle,
                                                     & sObjectExist,
                                                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SEQ_NOT_EXIST );
    }

    /**
     * ALTER SEQUENCE 권한 검사
     */

    STL_TRY( qlaCheckPrivAlterSequence( aTransID,
                                        aDBCStmt,
                                        aSQLStmt,
                                        & sInitPlan->mSeqHandle,
                                        aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Sequence Attribute 정보 설정
     **********************************************************/

    STL_TRY( qlqValidateAlterSeqOption( aSQLString,
                                        ellGetSequenceHandle( & sInitPlan->mSeqHandle ),
                                        & sInitPlan->mSeqAttr,
                                        sParseTree->mOptions,
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
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlqValidateAlterSeq()" ); */
    /* } */

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
 * @brief ALTER SEQUENCE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlqOptCodeAlterSeq( smlTransId      aTransID,
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
 * @brief ALTER SEQUENCE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlqOptDataAlterSeq( smlTransId      aTransID,
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
 * @brief ALTER SEQUENCE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlqExecuteAlterSeq( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv )
{
    qlqInitAlterSeq * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    stlInt64        sNewIdentityPhyID = 0;
    void          * sNewIdentityHandle = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SEQUENCE_TYPE,
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

    sInitPlan = (qlqInitAlterSeq *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4AlterSequence( aTransID,
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

    sInitPlan = (qlqInitAlterSeq *) aSQLStmt->mInitPlan;

    /***************************************************************
     *  시쿼스 옵션의 물리적 변경 
     ***************************************************************/

    STL_TRY( smlAlterSequence( aStmt,
                               ellGetSequenceHandle( & sInitPlan->mSeqHandle ),
                               & sInitPlan->mSeqAttr,           
                               & sNewIdentityPhyID,  
                               & sNewIdentityHandle, 
                               SML_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     *  시쿼스의 Dictionary 변경 
     ***************************************************************/

    STL_TRY( ellModifySequenceOption( aTransID,
                                      aStmt,
                                      ellGetSequenceID( & sInitPlan->mSeqHandle ),
                                      sNewIdentityPhyID,
                                      sInitPlan->mSeqAttr.mStartWith,
                                      sInitPlan->mSeqAttr.mMinValue,
                                      sInitPlan->mSeqAttr.mMaxValue,
                                      sInitPlan->mSeqAttr.mIncrementBy,
                                      sInitPlan->mSeqAttr.mCycle,
                                      sInitPlan->mSeqAttr.mCacheSize,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /***************************************************************
     *  시쿼스의 Dictionary Cache 재구성
     ***************************************************************/

    STL_TRY( ellRefineCache4AlterSequence( aTransID,
                                           aStmt,
                                           & sInitPlan->mSeqHandle,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlqExecuteAlterSeq()" ); */
    /* } */

    STL_FINISH;

    return STL_FAILURE;
}
    

/**
 * @brief ALTER SEQUENCE 옵션이 유효한지 검사한다.
 * @param[in] aSQLString   SQL String
 * @param[in] aSeqPhyHandle   Sequence Physical Handle
 * @param[in,out] aSeqAttr Sequence Attribute
 * @param[in] aSeqOption   Sequence Option Parse Tree
 * @param[in] aEnv         Environment
 * @remarks
 */
stlStatus qlqValidateAlterSeqOption( stlChar           * aSQLString,
                                     void              * aSeqPhyHandle,
                                     smlSequenceAttr   * aSeqAttr,
                                     qlpSequenceOption * aSeqOption,
                                     qllEnv            * aEnv )
{
    stlInt64 sBigintValue = 0;
    stlInt64 sCurrValue = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqPhyHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqAttr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqOption != NULL, QLL_ERROR_STACK(aEnv) );

    /**********************************************************
     * Alter Sequence Option 정보 획득
     **********************************************************/
    
    /**
     * 기존 Sequence 의 Attribute 정보를 획득
     * - 모든 Sequence Attribute 는 유효한 값이다.
     */
    
    STL_TRY( smlGetSequenceAttr( aSeqPhyHandle,
                                 aSeqAttr,
                                 SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * START WITH 은 변경할 수 없음.
     */
    
    STL_DASSERT ( aSeqOption->mStartWith == NULL );

    /**
     * INCREMENT BY
     */
            
    aSeqAttr->mValidFlags |= SML_SEQ_INCREMENTBY_YES;
    if ( aSeqOption->mIncrement != NULL )
    {
        STL_TRY( dtlGetIntegerFromString(
                     QLP_GET_PTR_VALUE(aSeqOption->mIncrement),
                     stlStrlen( QLP_GET_PTR_VALUE(aSeqOption->mIncrement) ),
                     & sBigintValue,
                     QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
                
        aSeqAttr->mIncrementBy = sBigintValue;
    }
    else
    {
        /* nothing to do */
    }

    /**
     * MAXVALUE
     */
            
    aSeqAttr->mValidFlags |= SML_SEQ_MAXVALUE_YES;
    if ( aSeqOption->mMaxValue != NULL )
    {
        STL_TRY( dtlGetIntegerFromString(
                     QLP_GET_PTR_VALUE(aSeqOption->mMaxValue),
                     stlStrlen( QLP_GET_PTR_VALUE(aSeqOption->mMaxValue) ),
                     & sBigintValue,
                     QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
                
        aSeqAttr->mMaxValue = sBigintValue;
    }
    else
    {
        if ( aSeqOption->mIsSetting[QLP_SEQUENCE_OPTION_MAXVALUE] == STL_TRUE )
        {
            /**
             * NO MAXVALUE 인 경우
             */

            if( aSeqAttr->mIncrementBy > 0 )
            {
                aSeqAttr->mMaxValue = SML_POSITIVE_MAXVALUE_DEFAULT;
            }
            else
            {
                aSeqAttr->mMaxValue = SML_NEGATIVE_MAXVALUE_DEFAULT;
            }
        }
        else
        {
            /* nothing to do */
        }
    }

    /**
     * MINVALUE
     */
            
    aSeqAttr->mValidFlags |= SML_SEQ_MINVALUE_YES;
    if ( aSeqOption->mMinValue != NULL )
    {
        STL_TRY( dtlGetIntegerFromString(
                     QLP_GET_PTR_VALUE(aSeqOption->mMinValue),
                     stlStrlen( QLP_GET_PTR_VALUE(aSeqOption->mMinValue) ),
                     & sBigintValue,
                     QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
                
        aSeqAttr->mMinValue = sBigintValue;
    }
    else
    {
        if ( aSeqOption->mIsSetting[QLP_SEQUENCE_OPTION_MINVALUE] == STL_TRUE )
        {
            /**
             * NO MINVALUE 인 경우
             */

            if( aSeqAttr->mIncrementBy > 0 )
            {
                aSeqAttr->mMinValue = SML_POSITIVE_MINVALUE_DEFAULT;
            }
            else
            {
                aSeqAttr->mMinValue = SML_NEGATIVE_MINVALUE_DEFAULT;
            }
        }
        else
        {
            /* nothing to do */
        }
    }

    /**
     * CACHE
     */
            
    aSeqAttr->mValidFlags |= SML_SEQ_CACHESIZE_YES;
    if ( aSeqOption->mCache != NULL )
    {
        STL_TRY( dtlGetIntegerFromString(
                     QLP_GET_PTR_VALUE(aSeqOption->mCache),
                     stlStrlen( QLP_GET_PTR_VALUE(aSeqOption->mCache) ),
                     & sBigintValue,
                     QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        aSeqAttr->mCacheSize = sBigintValue;
    }
    else
    {
        if ( aSeqOption->mNoCache != NULL )
        {
            /**
             * NO CACHE
             */
            
            aSeqAttr->mCacheSize = 1;
        }
        else
        {
            /* nothing to do */
        }
    }

    /**
     * CYCLE
     */

    aSeqAttr->mValidFlags |= SML_SEQ_CYCLE_YES;
    if ( aSeqOption->mCycle != NULL )
    {
        aSeqAttr->mCycle = QLP_GET_INT_VALUE( aSeqOption->mCycle );
        aSeqAttr->mValidFlags |= SML_SEQ_CYCLE_YES;
    }
    else
    {
        /* nothing to do */
    }
    
    /**
     * RESTART
     */

    if ( aSeqOption->mIsSetting[QLP_SEQUENCE_OPTION_RESTART_WITH] == STL_TRUE )
    {
        if ( aSeqOption->mRestartWith == NULL )
        {
            aSeqAttr->mValidFlags |= SML_SEQ_RESTART_DEFAULT;
        }
        else
        {
            aSeqAttr->mValidFlags |= SML_SEQ_RESTART_VALUE;

            STL_TRY( dtlGetIntegerFromString(
                         QLP_GET_PTR_VALUE(aSeqOption->mRestartWith),
                         stlStrlen( QLP_GET_PTR_VALUE(aSeqOption->mRestartWith) ),
                         & sBigintValue,
                         QLL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );

            aSeqAttr->mRestartValue = sBigintValue;
        }
    }
    else
    {
        aSeqAttr->mValidFlags |= SML_SEQ_RESTART_NO;
    }

    /**********************************************************
     * Alter Sequence Option 의 유효성 검증
     **********************************************************/
    
    /**
     * CREATE SEQUENCE 에 해당하는 유효성 검증
     */
    
    STL_TRY( qlqValidateSeqOptRelation( aSQLString,
                                        aSeqAttr,
                                        aSeqOption,
                                        aEnv )
             == STL_SUCCESS );

    /**
     * Sequence 의 현재값 획득
     */

    STL_TRY( smlGetCurrSequenceVal( aSeqPhyHandle,
                                    & sCurrValue,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * MIN/MAX 검증을 위한 현재값 보정
     */
    
    if ( aSeqOption->mIsSetting[QLP_SEQUENCE_OPTION_RESTART_WITH] == STL_TRUE )
    {
        if ( aSeqOption->mRestartWith == NULL )
        {
            sCurrValue = aSeqAttr->mStartWith;
        }
        else
        {
            sCurrValue = aSeqAttr->mRestartValue;
        }
    }
    else
    {
        /* nothing to do */
    }
    

    /**
     * RESTART value 는 MINVALUE 와 MAXVALUE 사이에 있어야 함
     */
    
    if ( aSeqOption->mIsSetting[QLP_SEQUENCE_OPTION_RESTART_WITH] == STL_TRUE )
    {
        STL_TRY_THROW( sCurrValue >= aSeqAttr->mMinValue, RAMP_ERR_RESTART_VALUE_LESS_THAN_MINVALUE );
        STL_TRY_THROW( sCurrValue <= aSeqAttr->mMaxValue, RAMP_ERR_RESTART_VALUE_GREATER_THAN_MAXVALUE );
    }
    
    /**
     * MINVALUE 는 current value 보다 클 수 없다.
     * MINVALUE 는 START WITH 보다 클 수 없다.
     */

    if ( aSeqOption->mMinValue != NULL )
    {
        STL_TRY_THROW( aSeqAttr->mMinValue <= sCurrValue, RAMP_ERR_MINVALUE_GREATER_THAN_CURRENT_VALUE );
        STL_TRY_THROW( aSeqAttr->mMinValue <= aSeqAttr->mStartWith, RAMP_ERR_MINVALUE_GREATER_THAN_START_VALUE );
    }
    
    /**
     * MAXVALUE 는 current value 보다 작을 수 없다.
     * MAXVALUE 는 START WITH 보다 작을 수 없다.
     */

    if ( aSeqOption->mMaxValue != NULL )
    {
        STL_TRY_THROW( aSeqAttr->mMaxValue >= sCurrValue, RAMP_ERR_MAXVALUE_LESS_THAN_CURRENT_VALUE );
        STL_TRY_THROW( aSeqAttr->mMaxValue >= aSeqAttr->mStartWith, RAMP_ERR_MAXVALUE_LESS_THAN_START_VALUE );
    }

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_RESTART_VALUE_LESS_THAN_MINVALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RESTART_VALUE_CANNOT_BE_MADE_TO_LESS_THAN_MINVALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqOption->mRestartNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_RESTART_VALUE_GREATER_THAN_MAXVALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RESTART_VALUE_CANNOT_BE_MADE_TO_EXCEED_MAXVALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqOption->mRestartNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_MINVALUE_GREATER_THAN_CURRENT_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MINVALUE_CANNOT_BE_MADE_TO_EXCEED_THE_CURRENT_VALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqOption->mMinValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_MINVALUE_GREATER_THAN_START_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MINVALUE_CANNOT_BE_MADE_TO_EXCEED_THE_START_WITH_VALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqOption->mMinValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_MAXVALUE_LESS_THAN_CURRENT_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MAXVALUE_CANNOT_BE_MADE_TO_LESS_THAN_THE_CURRENT_VALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqOption->mMaxValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_MAXVALUE_LESS_THAN_START_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MAXVALUE_CANNOT_BE_MADE_TO_LESS_THAN_THE_START_WITH_VALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqOption->mMaxValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlqAlterSequence */
