/*******************************************************************************
 * qliAlterIndexPhysicalAttr.c
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
 * @file qliAlterIndexPhysicalAttr.c
 * @brief ALTER INDEX .. STORAGE .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qliAlterIndexPhysicalAttr ALTER INDEX .. STORAGE ..
 * @ingroup qliAlterIndex
 * @{
 */

/**
 * @brief ALTER INDEX .. STORAGE .. 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qliValidateAlterPhysicalAttr( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv )
{
    qliInitAlterPhysicalAttr  * sInitPlan = NULL;
    qlpAlterIndexPhysicalAttr * sParseTree = NULL;
    ellDictHandle             * sAuthHandle = NULL;
    stlInt64                    sBigintValue = 0;
    stlBool                     sObjectExist = STL_FALSE;
    stlInt64                    sTableID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_INDEX_PHYSICAL_ATTR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_INDEX_PHYSICAL_ATTR_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpAlterIndexPhysicalAttr *) aParseTree;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qliInitAlterPhysicalAttr),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qliInitAlterPhysicalAttr) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Index Name Validation
     **********************************************************/

    if ( sParseTree->mIndexName->mSchema == NULL )
    {
        /**
         * Schema 가 명시되지 않은 경우
         */

        STL_TRY( ellGetIndexDictHandleWithAuth( aTransID,
                                                aSQLStmt->mViewSCN,
                                                sAuthHandle,
                                                sParseTree->mIndexName->mObject,
                                                & sInitPlan->mIndexHandle,
                                                & sObjectExist,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_INDEX_NOT_EXIST );

        /**
         * Schema Handle 획득
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
                                         sParseTree->mIndexName->mSchema,
                                         & sInitPlan->mSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXIST );
        
        STL_TRY( ellGetIndexDictHandleWithSchema( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  & sInitPlan->mSchemaHandle,
                                                  sParseTree->mIndexName->mObject,
                                                  & sInitPlan->mIndexHandle,
                                                  & sObjectExist,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_INDEX_NOT_EXIST );
    }
    
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

    /**
     * ALTER INDEX 권한 검사
     */

    STL_TRY( qlaCheckPrivAlterIndex( aTransID,
                                     aDBCStmt,
                                     aSQLStmt,
                                     & sInitPlan->mIndexHandle,
                                     aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Physical Attribute Validation
     **********************************************************/

    /**
     * PCTFREE Validation
     */
    
    if ( sParseTree->mPctFree == NULL )
    {
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_INDEX_PCTFREE_NO;
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(sParseTree->mPctFree),
                           stlStrlen( QLP_GET_PTR_VALUE(sParseTree->mPctFree) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_PCTFREE );

        STL_TRY_THROW( (sBigintValue >= SML_MIN_PCTFREE) &&
                       (sBigintValue <= SML_MAX_PCTFREE),
                       RAMP_ERR_INVALID_PCTFREE );
        
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_INDEX_PCTFREE_YES;
        sInitPlan->mPhysicalAttr.mPctFree = (stlInt16) sBigintValue;
    }

    /**
     * INITRANS Validation
     */
    
    if ( sParseTree->mIniTrans == NULL )
    {
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_INDEX_INITRANS_NO;
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(sParseTree->mIniTrans),
                           stlStrlen( QLP_GET_PTR_VALUE(sParseTree->mIniTrans) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_INITRANS );

        STL_TRY_THROW( (sBigintValue >= SML_MIN_INITRANS ) &&
                       (sBigintValue <= SML_MAX_INITRANS),
                       RAMP_ERR_INVALID_INITRANS );
        
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_INDEX_INITRANS_YES;
        sInitPlan->mPhysicalAttr.mIniTrans = (stlInt16) sBigintValue;
    }
    
    /**
     * MAXTRANS Validation
     */
    
    if ( sParseTree->mMaxTrans == NULL )
    {
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_INDEX_MAXTRANS_NO;
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(sParseTree->mMaxTrans),
                           stlStrlen( QLP_GET_PTR_VALUE(sParseTree->mMaxTrans) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_MAXTRANS );

        STL_TRY_THROW( (sBigintValue >= SML_MIN_MAXTRANS ) &&
                       (sBigintValue <= SML_MAX_MAXTRANS),
                       RAMP_ERR_INVALID_MAXTRANS );
        
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_INDEX_MAXTRANS_YES;
        sInitPlan->mPhysicalAttr.mMaxTrans = (stlInt16)sBigintValue;
    }

    if( (sInitPlan->mPhysicalAttr.mValidFlags & SML_TABLE_MAXTRANS_YES) &&
        (sInitPlan->mPhysicalAttr.mValidFlags & SML_TABLE_INITRANS_YES) )
    {
        STL_TRY_THROW( sInitPlan->mPhysicalAttr.mMaxTrans >=
                       sInitPlan->mPhysicalAttr.mIniTrans,
                       RAMP_ERR_TRANS_RANGE );
    }

    /**
     * Segment Attribute Validation
     */

    STL_TRY( qlrValidateSegmentAttr( aSQLString,
                                     (smlTbsId) ellGetIndexTablespaceID( & sInitPlan->mIndexHandle ),
                                     sParseTree->mSegInitialSize,
                                     sParseTree->mSegNextSize,
                                     sParseTree->mSegMinSize,
                                     sParseTree->mSegMaxSize,
                                     & sInitPlan->mPhysicalAttr.mSegAttr,
                                     aEnv )
             == STL_SUCCESS );

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mTableHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    /****************************************************
     * Init Plan 연결 
     ****************************************************/
    
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
                                           sParseTree->mIndexName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mIndexName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sParseTree->mIndexName->mSchema );
    }

    STL_CATCH( RAMP_ERR_INVALID_PCTFREE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_PCTVALUE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mPctFree->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_INITRANS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_INITRANS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mIniTrans->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_MAXTRANS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_MAXTRANS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mMaxTrans->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_TRANS_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TRANS_RANGE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mIniTrans->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER INDEX .. STORAGE .. 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qliOptCodeAlterPhysicalAttr( smlTransId      aTransID,
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
 * @brief ALTER INDEX .. STORAGE .. 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qliOptDataAlterPhysicalAttr( smlTransId      aTransID,
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
 * @brief ALTER INDEX .. STORAGE .. 구문을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qliExecuteAlterPhysicalAttr( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    qliInitAlterPhysicalAttr * sInitPlan = NULL;
    
    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_INDEX_PHYSICAL_ATTR_TYPE,
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

    sInitPlan = (qliInitAlterPhysicalAttr *) aSQLStmt->mInitPlan;
    
    /**
     * Lock 획득 
     */
    
    STL_TRY( ellLock4AlterIndexPhysicalAttr( aTransID,
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


    sInitPlan = (qliInitAlterPhysicalAttr *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Index 구조를 변경
     ***************************************************************/
    
    /**
     * Index 구조를 변경
     */
    
    STL_TRY( smlAlterIndexAttr( aStmt,
                                ellGetIndexHandle(&sInitPlan->mIndexHandle),
                                &sInitPlan->mPhysicalAttr,
                                SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Index 구조가 변경된 시간을 설정 
     */

    STL_TRY( ellSetTimeAlterIndex( aTransID,
                                   aStmt,
                                   ellGetIndexID( & sInitPlan->mIndexHandle ),
                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} qliAlterIndexPhysicalAttr */
