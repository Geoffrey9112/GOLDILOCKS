/*******************************************************************************
 * qlqCreateSequence.c
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
 * @file qlqCreateSequence.c
 * @brief CREATE SEQUENCE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlqCreateSequence CREATE SEQUENCE
 * @ingroup qlq
 * @{
 */


/**
 * @brief CREATE SEQUENCE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlqValidateCreateSeq( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlpSequenceDef   * sParseTree = NULL;
    qlqInitCreateSeq * sInitPlan = NULL;
    
    stlBool            sObjectExist = STL_FALSE;
    ellTableType       sObjectType = ELL_TABLE_TYPE_NA;

    ellDictHandle sObjectHandle;
    ellInitDictHandle( & sObjectHandle );
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_SEQUENCE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_CREATE_SEQUENCE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpSequenceDef *) aParseTree;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlqInitCreateSeq),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlqInitCreateSeq) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Schema Validation
     **********************************************************/

    /**
     * Schema 존재 여부 확인
     */
    
    if ( sParseTree->mName->mSchema == NULL )
    {
        /**
         * Schema Name 이 명시되지 않은 경우
         */

        STL_TRY( ellGetAuthFirstSchemaDictHandle( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  sAuthHandle,
                                                  & sInitPlan->mSchemaHandle,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Schema Name 이 명시된 경우
         */

        STL_TRY( ellGetSchemaDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         sParseTree->mName->mSchema,
                                         & sInitPlan->mSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXISTS );
    }
    

    /**
     * Wriable Schema 인지 검사
     */
    
    STL_TRY_THROW( ellGetSchemaWritable( & sInitPlan->mSchemaHandle ) == STL_TRUE,
                   RAMP_ERR_SCHEMA_NOT_WRITABLE );

    /**
     * CREATE SEQUENCE ON SCHEMA 권한 검사
     */

    STL_TRY( qlaCheckSchemaPriv( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 ELL_SCHEMA_PRIV_ACTION_CREATE_SEQUENCE,
                                 & sInitPlan->mSchemaHandle,
                                 aEnv )
             == STL_SUCCESS );
                                 
    /**********************************************************
     * Sequence Validation
     **********************************************************/

    /**
     * 같은 이름을 가진 Object( Table, Sequence, Synonym) 가 있는지 확인
     * - Oracle 호환성을 위해 Name Space 를 Table,Sequence, Synonym 이 같이 사용한다.
     * - 호환성을 위해 Dictionary 체계를 바꾸는 것보다 QP 단에서 검사한다.
     */
    
    STL_TRY( qlyExistSameNameObject( aTransID,
                                     aSQLStmt,
                                     aSQLString,
                                     & sInitPlan->mSchemaHandle,
                                     sParseTree->mName,
                                     & sObjectHandle,
                                     & sObjectExist,
                                     & sObjectType, /* Not Used In this function */
                                     aEnv )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sObjectExist != STL_TRUE, RAMP_ERR_SAME_NAME_OBJECT_EXISTS );

    /**
     * Sequence Name
     */
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( sParseTree->mName->mObject ) + 1,
                                (void **) & sInitPlan->mSeqName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sInitPlan->mSeqName, sParseTree->mName->mObject );

    /**********************************************************
     * Sequence Attribute Validation
     **********************************************************/

    stlMemset( & sInitPlan->mSeqAttr, 0x00, STL_SIZEOF(smlSequenceAttr) );

    STL_TRY( qlqValidateSeqGenOption( aSQLString,
                                      & sInitPlan->mSeqAttr,
                                      sParseTree->mOptions,
                                      aEnv )
             == STL_SUCCESS );

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSchemaHandle,
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

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mName->mSchema );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_WRITABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_WRITABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ) );
    }

    STL_CATCH( RAMP_ERR_SAME_NAME_OBJECT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_OBJECT_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ),
                      sParseTree->mName->mObject );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief CREATE SEQUENCE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlqOptCodeCreateSeq( smlTransId      aTransID,
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
 * @brief CREATE SEQUENCE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlqOptDataCreateSeq( smlTransId      aTransID,
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
 * @brief CREATE SEQUENCE 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlqExecuteCreateSeq( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    ellDictHandle  * sAuthHandle = NULL;
    
    qlqInitCreateSeq * sInitPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;

    stlInt64   sSeqID = ELL_DICT_OBJECT_ID_NA;
    stlInt64   sSeqTableID = ELL_DICT_OBJECT_ID_NA;
    stlInt64   sPhysicalID = 0;
    void     * sPhysicalHandle = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_SEQUENCE_TYPE,
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
    
    sInitPlan = (qlqInitCreateSeq *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * CREATE SEQUENCE 구문을 위한 DDL Lock 획득
     */

    STL_TRY( ellLock4CreateSequence( aTransID,
                                     aStmt,
                                     sAuthHandle,
                                     & sInitPlan->mSchemaHandle,
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
    
    sInitPlan = (qlqInitCreateSeq *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /**********************************************************
     * Sequence 객체 생성
     **********************************************************/
    
    /**
     * Sequence 생성
     */

    STL_TRY( smlCreateSequence( aStmt,
                                & sInitPlan->mSeqAttr,
                                & sPhysicalID,
                                & sPhysicalHandle,
                                SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**********************************************************
     * Sequence Dictionary 구축 
     **********************************************************/

    /**
     * Table Descriptor 추가
     * Sequence 와 Table 의 Naming Resolution 을 위해 추가한다.
     */

    STL_TRY( ellInsertTableDesc( aTransID,
                                 aStmt,
                                 ellGetAuthID( sAuthHandle ),
                                 ellGetSchemaID( & sInitPlan->mSchemaHandle ),
                                 & sSeqTableID,
                                 ELL_DICT_TABLESPACE_ID_NA,
                                 ELL_DICT_OBJECT_ID_NA,
                                 sInitPlan->mSeqName,
                                 ELL_TABLE_TYPE_SEQUENCE,
                                 NULL,
                                 ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Sequence Descriptor 추가
     */

    STL_TRY( ellInsertSequenceDesc( aTransID,
                                    aStmt,
                                    ellGetAuthID( sAuthHandle ),
                                    ellGetSchemaID( & sInitPlan->mSchemaHandle ),
                                    & sSeqID,
                                    sSeqTableID,
                                    SML_MEMORY_DICT_SYSTEM_TBS_ID,
                                    sPhysicalID,
                                    sInitPlan->mSeqName,
                                    sInitPlan->mSeqAttr.mStartWith,
                                    sInitPlan->mSeqAttr.mMinValue,
                                    sInitPlan->mSeqAttr.mMaxValue,
                                    sInitPlan->mSeqAttr.mIncrementBy,
                                    sInitPlan->mSeqAttr.mCycle,
                                    sInitPlan->mSeqAttr.mCacheSize,
                                    NULL,
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * SQL-Sequence Cache 구축
     */                                              

    STL_TRY( ellRefineCache4CreateSequence( aTransID,
                                            aStmt,
                                            ellGetAuthID( sAuthHandle ),
                                            sSeqID,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 생성한 객체에 대해 Lock 을 획득
     * 
     * commit -> pending -> trans end (unlock) 과정 중에
     * 해당 객체에 대한 DROP, ALTER 가 수행될 경우를 방지하기 위해
     * 생성한 객체에 대한 lock 을 획득해야 함.
     */

    STL_TRY( ellLockRowAfterCreateNonBaseTableObject( aTransID,
                                                      aStmt,
                                                      ELL_OBJECT_SEQUENCE,
                                                      sSeqID,
                                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sequence Generation Option 을 검증하고 정보를 추가한다.
 * @param[in] aSQLString   SQL String
 * @param[in,out] aSeqAttr Sequence Attribute
 * @param[in] aSeqOption   Sequence Option Parse Tree
 * @param[in] aEnv         Environemt
 * @remarks
 */
stlStatus qlqValidateSeqGenOption( stlChar           * aSQLString,
                                   smlSequenceAttr   * aSeqAttr,
                                   qlpSequenceOption * aSeqOption,
                                   qllEnv            * aEnv )
{
    stlInt64    sBigintValue = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqAttr != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Sequence Option 이 없는 경우
     */
    
    STL_TRY_THROW( aSeqOption != NULL, RAMP_NO_OPTION );
    
    /**
     * START WITH
     */
    
    if ( aSeqOption->mStartWith != NULL )
    {
        STL_TRY( dtlGetIntegerFromString(
                     QLP_GET_PTR_VALUE(aSeqOption->mStartWith),
                     stlStrlen( QLP_GET_PTR_VALUE(aSeqOption->mStartWith) ),
                     & sBigintValue,
                     QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
                
        aSeqAttr->mStartWith = sBigintValue;
        aSeqAttr->mValidFlags |= SML_SEQ_STARTWITH_YES;
    }
    else
    {
        aSeqAttr->mValidFlags |= SML_SEQ_STARTWITH_NO;
    }

    /**
     * INCREMENT BY
     */
            
    if ( aSeqOption->mIncrement != NULL )
    {
        STL_TRY( dtlGetIntegerFromString(
                     QLP_GET_PTR_VALUE(aSeqOption->mIncrement),
                     stlStrlen( QLP_GET_PTR_VALUE(aSeqOption->mIncrement) ),
                     & sBigintValue,
                     QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
                
        aSeqAttr->mIncrementBy = sBigintValue;
        aSeqAttr->mValidFlags |= SML_SEQ_INCREMENTBY_YES;
    }
    else
    {
        aSeqAttr->mValidFlags |= SML_SEQ_INCREMENTBY_NO;
    }

    /**
     * MAXVALUE
     */
            
    if ( aSeqOption->mMaxValue != NULL )
    {
        STL_TRY( dtlGetIntegerFromString(
                     QLP_GET_PTR_VALUE(aSeqOption->mMaxValue),
                     stlStrlen( QLP_GET_PTR_VALUE(aSeqOption->mMaxValue) ),
                     & sBigintValue,
                     QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
                
        aSeqAttr->mMaxValue = sBigintValue;
        aSeqAttr->mValidFlags |= SML_SEQ_MAXVALUE_YES;
    }
    else
    {
        aSeqAttr->mValidFlags |= SML_SEQ_MAXVALUE_NO;
    }

    /**
     * MINVALUE
     */
            
    if ( aSeqOption->mMinValue != NULL )
    {
        STL_TRY( dtlGetIntegerFromString(
                     QLP_GET_PTR_VALUE(aSeqOption->mMinValue),
                     stlStrlen( QLP_GET_PTR_VALUE(aSeqOption->mMinValue) ),
                     & sBigintValue,
                     QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
                
        aSeqAttr->mMinValue = sBigintValue;
        aSeqAttr->mValidFlags |= SML_SEQ_MINVALUE_YES;
    }
    else
    {
        aSeqAttr->mValidFlags |= SML_SEQ_MINVALUE_NO;
    }

    /**
     * CACHE
     */
            
    if ( aSeqOption->mCache != NULL )
    {
        STL_TRY( dtlGetIntegerFromString(
                     QLP_GET_PTR_VALUE(aSeqOption->mCache),
                     stlStrlen( QLP_GET_PTR_VALUE(aSeqOption->mCache) ),
                     & sBigintValue,
                     QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );

        aSeqAttr->mCacheSize = sBigintValue;
        aSeqAttr->mValidFlags |= SML_SEQ_CACHESIZE_YES;
    }
    else
    {
        aSeqAttr->mValidFlags |= SML_SEQ_CACHESIZE_NO;
    }

    /**
     * NO_CACHE
     */

    if ( aSeqOption->mNoCache != NULL )
    {
        aSeqAttr->mValidFlags |= SML_SEQ_NOCACHE_YES;
    }
    else
    {
        aSeqAttr->mValidFlags |= SML_SEQ_NOCACHE_NO;
    }
            
    /**
     * CYCLE
     */

    if ( aSeqOption->mCycle != NULL )
    {
        aSeqAttr->mCycle = QLP_GET_INT_VALUE( aSeqOption->mCycle );
        aSeqAttr->mValidFlags |= SML_SEQ_CYCLE_YES;
    }
    else
    {
        aSeqAttr->mValidFlags |= SML_SEQ_CYCLE_NO;
    }

            
    /**
     * 지정되지 않은 Sequence Attribute 를 채운다.
     */

    STL_RAMP( RAMP_NO_OPTION );
    smlFillDefaultSequenceAttr( aSeqAttr );

    /**
     * 자동 생성된 정보를 Validation 한다.
     */

    if ( aSeqOption != NULL )
    {
        STL_TRY( qlqValidateSeqOptRelation( aSQLString,
                                            aSeqAttr,
                                            aSeqOption,
                                            aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Sequence Option 을 사용하지 않은 경우 */
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 사용자가 정의하지 않은 Sequence Attribute 를 생성하고 그 관계를 Validation 한다.
 * @param[in]  aSQLString  SQL String
 * @param[in]  aSeqAttr       Sequence Attribute
 * @param[in]  aSeqOption  Sequence Option Parse Tree
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus qlqValidateSeqOptRelation( stlChar           * aSQLString,
                                     smlSequenceAttr   * aSeqAttr,
                                     qlpSequenceOption * aSeqOption,
                                     qllEnv            * aEnv )
{
    stlUInt64   sUDiffValue;
    stlUInt64   sUIncrementBy;

    /*
     * Paramter Validation
     *
     * invalidation시 에러의 위치를 좀 더 정확하게 나타내기 위하여
     * 사용자 입력 옵션을 기준으로 순서대로 체크하도록 한다.
     * 시스템이 default로 입력하는 값들은 모두 validation하다.
     * 따라서 사용자가 입력한 옵션에 대해서만 관련 validation check를 하면 된다.
     *
     * 체크해야할 내용은 아래와 같다.
     * 1. MinValue는 MaxValue보다 작아야 한다.
     * 2. IncrementBy는 0이 아니어야 한다.
     * 3. IncrementBy의 절대값은 MaxValue - MinValue보다 같거나 작아야 한다.
     * 4. StartWith는 MinValue보다 같거나 커야 한다.
     * 5. StartWith는 MaxValue보다 같거나 작아야 한다.
     * 6. 사용자가 NoCache를 설정한 경우 cache size는 0이어야 한다.
     * 7. NoCache가 아닌 경우 cache size는 2보다 같거나 커야 한다.
     * 8. Cycle 지정시 IncrementBy가 양의 값이면 사용자가 MaxValue를 설정해야 한다.
     * 9. Cycle 지정시 IncrementBy가 음의 값이면 사용자가 MinValue를 설정해야 한다.
     * 10. Cycle 지정시 CacheSize는 한 Cycle보다 같거나 작아야 한다.
     */

    STL_PARAM_VALIDATE( aSeqAttr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqOption != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 사용자가 MinValue 또는 MaxValue 입력
     *
     * (invalidation인 경우 MinValue를 우선으로 에러위치를 정한다.
     *  그 이유는 에러 내용이 MinValue를 기준이기 때문이다.)
     */
    if( (aSeqOption->mMinValue != NULL) || (aSeqOption->mMaxValue != NULL) )
    {
        /**
         * MinValue는 MaxValue보다 작아야 한다.
         */

        STL_TRY_THROW( (aSeqAttr->mMinValue < aSeqAttr->mMaxValue),
                       RAMP_ERR_MINVALUE_RANGE );
    }

    /**
     * 사용자가 IncrementBy 입력
     */
    if( aSeqOption->mIncrement != NULL )
    {
        /**
         * IncrementBy는 0이 아니어야 한다.
         */
        STL_TRY_THROW( (aSeqAttr->mIncrementBy != 0),
                       RAMP_ERR_INCREMENT_ZERO );

        /**
         * IncrementBy의 절대값은 MaxValue - MinValue보다 같거나 작아야 한다.
         * 값의 범위 특성상 MaxValue와 MinValue의 절대값은 1의 차이가 나므로
         * overflow에 대해 주의하여야 한다.
         */
        sUIncrementBy = ( aSeqAttr->mIncrementBy == STL_INT64_MIN ? 
                          (stlUInt64)STL_INT64_MAX + 1 : stlAbsInt64(aSeqAttr->mIncrementBy) );
        if( (aSeqAttr->mMaxValue < 0) == (aSeqAttr->mMinValue < 0) )
        {
            sUDiffValue = (stlUInt64)(aSeqAttr->mMaxValue - aSeqAttr->mMinValue);
        }
        else
        {
            /**
             * 위에서 "MinValue는 MaxValue보다 작아야 한다."는 체크를 했으므로
             * MaxValue >= 0 && MinValue <= 0 인 경우만 존재한다.
             */
            sUDiffValue = (stlUInt64)aSeqAttr->mMaxValue + 
                        (stlUInt64)( aSeqAttr->mMinValue == STL_INT64_MIN ? 
                                     (stlUInt64)STL_INT64_MAX + 1 : stlAbsInt64(aSeqAttr->mMinValue) );
        }

        STL_TRY_THROW( sUIncrementBy <= sUDiffValue, RAMP_ERR_INCREMENT_RANGE );
    }

    /**
     * 사용자가 StartWith 입력
     */
    if( aSeqOption->mStartWith != NULL )
    {
        /**
         * StartWith는 MinValue보다 같거나 커야 한다.
         */

        STL_TRY_THROW( (aSeqAttr->mStartWith >= aSeqAttr->mMinValue),
                       RAMP_ERR_STARTWITH_UNDER_MINVALUE );

        /**
         * StartWith는 MaxValue보다 같거나 작아야 한다.
         */

        STL_TRY_THROW( (aSeqAttr->mStartWith <= aSeqAttr->mMaxValue),
                       RAMP_ERR_STARTWITH_OVER_MAXVALUE );
    }

    /**
     * 사용자가 Cache 입력
     */
    if( aSeqOption->mCache != NULL )
    {
        /**
         * cache size는 2보다 같거나 커야 한다.
         */
        STL_TRY_THROW( aSeqAttr->mCacheSize >= 2, RAMP_ERR_CACHE_SIZE );

        /**
         * Cycle이 설정되어 있으면 cache size와의 관계에 의한 validation check를 한다.
         * Cycle 설정이 default로 설정되어 있을 수 있으므로 이를 고려한다.
         */
        if( aSeqAttr->mCycle == STL_TRUE )
        {
            /**
             * CacheSize는 한 Cycle보다 같거나 작아야 한다.
             * 값의 범위 특성상 MaxValue와 MinValue의 절대값은 1의 차이가 나므로
             * overflow에 대해 주의하여야 한다.
             */
            sUIncrementBy = ( aSeqAttr->mIncrementBy == STL_INT64_MIN ? 
                              (stlUInt64)STL_INT64_MAX + 1 : stlAbsInt64(aSeqAttr->mIncrementBy) );
            if( (aSeqAttr->mMaxValue < 0) == (aSeqAttr->mMinValue < 0) )
            {
                sUDiffValue = (stlUInt64)(aSeqAttr->mMaxValue - aSeqAttr->mMinValue);
            }
            else
            {
                /**
                 * 위에서 "MinValue는 MaxValue보다 작아야 한다."는 체크를 했으므로
                 * MaxValue >= 0 && MinValue <= 0 인 경우만 존재한다.
                 */
                sUDiffValue = (stlUInt64)aSeqAttr->mMaxValue + 
                            (stlUInt64)( aSeqAttr->mMinValue == STL_INT64_MIN ? 
                                         (stlUInt64)STL_INT64_MAX + 1 : stlAbsInt64(aSeqAttr->mMinValue) );
            }

            STL_TRY_THROW( (stlUInt64)aSeqAttr->mCacheSize <= (((sUDiffValue - 1) / sUIncrementBy) + 1),
                           RAMP_ERR_CACHE_RANGE );
        }
    }

    /**
     * 사용자가 Cycle 입력
     */
    if( (aSeqOption->mCycle != NULL) && (aSeqAttr->mCycle == STL_TRUE) )
    {
        /**
         * Cycle Sequence는 다음 조건을 만족해야 한다.
         */

        /**
         * Oracle 만의 제약임. 
         */
        
        /* if( aSeqAttr->mIncrementBy > 0 ) */
        /* { */
        /*     /\** */
        /*      * Ascending Sequence는 반드시 사용자가 MaxValue를 설정해야 한다. */
        /*      *\/ */

        /*     STL_TRY_THROW( aSeqOption->mMaxValue != NULL, */
        /*                    RAMP_ERR_ASCENDING_CYCLE_NO_MAXVALUE ); */
        /* } */
        /* else */
        /* { */
        /*     /\** */
        /*      * Descending Sequence는 반드시 사용자가 MinValue를 설정해야 한다. */
        /*      *\/ */

        /*     STL_TRY_THROW( aSeqOption->mMinValue != NULL, */
        /*                    RAMP_ERR_DESCENDING_CYCLE_NO_MINVALUE ); */
        /* } */

        /**
         * CacheSize는 한 Cycle보다 같거나 작아야 한다.
         * 값의 범위 특성상 MaxValue와 MinValue의 절대값은 1의 차이가 나므로
         * overflow에 대해 주의하여야 한다.
         */
        sUIncrementBy = ( aSeqAttr->mIncrementBy == STL_INT64_MIN ? 
                          (stlUInt64)STL_INT64_MAX + 1 : stlAbsInt64(aSeqAttr->mIncrementBy) );
        if( (aSeqAttr->mMaxValue < 0) == (aSeqAttr->mMinValue < 0) )
        {
            sUDiffValue = (stlUInt64)(aSeqAttr->mMaxValue - aSeqAttr->mMinValue);
        }
        else
        {
            /**
             * 위에서 "MinValue는 MaxValue보다 작아야 한다."는 체크를 했으므로
             * MaxValue >= 0 && MinValue <= 0 인 경우만 존재한다.
             */
            sUDiffValue = (stlUInt64)aSeqAttr->mMaxValue + 
                        (stlUInt64)( aSeqAttr->mMinValue == STL_INT64_MIN ? 
                                     (stlUInt64)STL_INT64_MAX + 1 : stlAbsInt64(aSeqAttr->mMinValue) );
        }

        STL_TRY_THROW( (stlUInt64)aSeqAttr->mCacheSize <= (((sUDiffValue - 1) / sUIncrementBy) + 1),
                       RAMP_ERR_CACHE_RANGE );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MINVALUE_RANGE )
    {
        if ( aSeqOption->mMinValue != NULL )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_SEQUENCE_MINVALUE_RANGE,
                          qlgMakeErrPosString( aSQLString,
                                               aSeqOption->mMinValue->mNodePos,
                                               aEnv ),
                          QLL_ERROR_STACK( aEnv ) );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_SEQUENCE_MINVALUE_RANGE,
                          qlgMakeErrPosString( aSQLString,
                                               aSeqOption->mMaxValue->mNodePos,
                                               aEnv ),
                          QLL_ERROR_STACK( aEnv ) );
        }
    }

    STL_CATCH( RAMP_ERR_INCREMENT_ZERO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_INCREMENT_ZERO,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqOption->mIncrement->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_INCREMENT_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_INCREMENT_RANGE,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqOption->mIncrement->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_STARTWITH_UNDER_MINVALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_STARTWITH_UNDER_MINVALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqOption->mStartWith->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_CATCH( RAMP_ERR_STARTWITH_OVER_MAXVALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_STARTWITH_OVER_MAXVALUE,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqOption->mStartWith->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_CACHE_SIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_CACHE_SIZE,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqOption->mCache->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_CACHE_RANGE )
    {
        if( aSeqOption->mCache != NULL )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_SEQUENCE_CACHE_RANGE,
                          qlgMakeErrPosString( aSQLString,
                                               aSeqOption->mCache->mNodePos,
                                               aEnv ),
                          QLL_ERROR_STACK( aEnv ) );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_SEQUENCE_CACHE_RANGE,
                          qlgMakeErrPosString( aSQLString,
                                               aSeqOption->mCycle->mNodePos,
                                               aEnv ),
                          QLL_ERROR_STACK( aEnv ) );
        }

    }

    /* STL_CATCH( RAMP_ERR_ASCENDING_CYCLE_NO_MAXVALUE ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_SEQUENCE_ASCENDING_CYCLE_NO_MAXVALUE, */
    /*                   qlgMakeErrPosString( aSQLString, */
    /*                                        aSeqOption->mCycle->mNodePos, */
    /*                                        aEnv ), */
    /*                   QLL_ERROR_STACK( aEnv ) ); */
    /* } */

    /* STL_CATCH( RAMP_ERR_DESCENDING_CYCLE_NO_MINVALUE ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_SEQUENCE_DESCENDING_CYCLE_NO_MINVALUE, */
    /*                   qlgMakeErrPosString( aSQLString, */
    /*                                        aSeqOption->mCycle->mNodePos, */
    /*                                        aEnv ), */
    /*                   QLL_ERROR_STACK( aEnv ) ); */
    /* } */

    STL_FINISH;

    return STL_FAILURE;
}




                            
/** @} qlqCreateSequence */
