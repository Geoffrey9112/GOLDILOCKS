/*******************************************************************************
 * qlrAlterTablePhysicalAttr.c
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
 * @file qlrAlterTablePhysicalAttr.c
 * @brief ALTER TABLE .. STORAGE .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrAlterTablePhysicalAttr ALTER TABLE .. STORAGE ..
 * @ingroup qlrAlterTable
 * @{
 */


/**
 * @brief ALTER TABLE .. STORAGE .. 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateAlterPhysicalAttr( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv )
{
    qlrInitAlterPhysicalAttr  * sInitPlan = NULL;
    qlpAlterTablePhysicalAttr * sParseTree = NULL;
    ellDictHandle             * sAuthHandle = NULL;
    stlInt64                    sBigintValue = 0;
    void                      * sTableHandle = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_PHYSICAL_ATTR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLE_PHYSICAL_ATTR_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpAlterTablePhysicalAttr *) aParseTree;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitAlterPhysicalAttr),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitAlterPhysicalAttr) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /**********************************************************
     * Table Name Validation
     **********************************************************/

    STL_TRY( qlrGetAlterTableHandle( aTransID,
                                     aDBCStmt,
                                     aSQLStmt,
                                     aSQLString,
                                     sAuthHandle,
                                     sParseTree->mTableName,
                                     & sInitPlan->mSchemaHandle,
                                     & sInitPlan->mTableHandle,
                                     aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Columnar Table Validation
     **********************************************************/

    sTableHandle = ellGetTableHandle( & sInitPlan->mTableHandle );

    STL_TRY_THROW( smlIsAlterableTable( sTableHandle ) == STL_TRUE,
                   RAMP_ERR_NOT_IMPLEMENTED );

    /**********************************************************
     * Physical Attribute Validation
     **********************************************************/

    /**
     * PCTFREE Validation
     */
    
    if ( sParseTree->mPctFree == NULL )
    {
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_PCTFREE_NO;
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
        
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_PCTFREE_YES;
        sInitPlan->mPhysicalAttr.mPctFree = (stlInt16) sBigintValue;
    }

    /**
     * PCTUSED Validation
     */
    
    if ( sParseTree->mPctUsed == NULL )
    {
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_PCTUSED_NO;
    }
    else
    {
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(sParseTree->mPctUsed),
                           stlStrlen( QLP_GET_PTR_VALUE(sParseTree->mPctUsed) ),
                           & sBigintValue,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_PCTUSED );

        STL_TRY_THROW( (sBigintValue >= SML_MIN_PCTUSED) &&
                       (sBigintValue <= SML_MAX_PCTUSED),
                       RAMP_ERR_INVALID_PCTUSED );
        
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_PCTUSED_YES;
        sInitPlan->mPhysicalAttr.mPctUsed = (stlInt16) sBigintValue;
    }

    /**
     * INITRANS Validation
     */
    
    if ( sParseTree->mIniTrans == NULL )
    {
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_INITRANS_NO;
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
    
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_INITRANS_YES;
        sInitPlan->mPhysicalAttr.mIniTrans = (stlInt16) sBigintValue;
    }

    /**
     * MAXTRANS Validation
     */
    
    if ( sParseTree->mMaxTrans == NULL )
    {
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_MAXTRANS_NO;
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
        
        sInitPlan->mPhysicalAttr.mValidFlags |= SML_TABLE_MAXTRANS_YES;
        sInitPlan->mPhysicalAttr.mMaxTrans = (stlInt16)sBigintValue;
    }

    if( (sInitPlan->mPhysicalAttr.mValidFlags & SML_TABLE_MAXTRANS_YES) &&
        (sInitPlan->mPhysicalAttr.mValidFlags & SML_TABLE_INITRANS_YES) )
    {
        STL_TRY_THROW( sInitPlan->mPhysicalAttr.mMaxTrans >=
                       sInitPlan->mPhysicalAttr.mIniTrans,
                       RAMP_ERR_TRANS_RANGE );
    }

    STL_TRY_THROW( sParseTree->mSegInitialSize == NULL, RAMP_ERR_MODIFY_STORAGE_INITIAL );
    
    /**
     * Segment Attribute Validation
     */

    STL_TRY( qlrValidateSegmentAttr( aSQLString,
                                     (smlTbsId) ellGetTableTablespaceID( & sInitPlan->mTableHandle ),
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
        
    STL_CATCH( RAMP_ERR_MODIFY_STORAGE_INITIAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_MODIFY_STORAGE_INITIAL,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mSegInitialSize->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
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

    STL_CATCH( RAMP_ERR_INVALID_PCTUSED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_PCTVALUE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mPctUsed->mNodePos,
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

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "ALTER TABLE STORAGE" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER TABLE .. STORAGE .. 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeAlterPhysicalAttr( smlTransId      aTransID,
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
 * @brief ALTER TABLE .. STORAGE .. 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataAlterPhysicalAttr( smlTransId      aTransID,
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
 * @brief ALTER TABLE .. STORAGE .. 구문을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteAlterPhysicalAttr( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    qlrInitAlterPhysicalAttr * sInitPlan = NULL;
    stlTime                    sTime = 0;
    stlBool   sLocked = STL_TRUE;
    


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_PHYSICAL_ATTR_TYPE,
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

    sInitPlan = (qlrInitAlterPhysicalAttr *) aSQLStmt->mInitPlan;
    
    /**
     * Lock 획득 
     */
    
    STL_TRY( ellLock4AlterPhysicalAttr( aTransID,
                                        aStmt,
                                        & sInitPlan->mTableHandle,
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

    sInitPlan = (qlrInitAlterPhysicalAttr *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Table 구조를 변경
     ***************************************************************/

    
    /**
     * Table 구조를 변경
     */
    
    STL_TRY( smlAlterTableAttr( aStmt,
                                ellGetTableHandle(&sInitPlan->mTableHandle),
                                &sInitPlan->mPhysicalAttr,
                                NULL,  /* aIndexAttr */
                                SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Table 구조가 변경된 시간을 설정 
     */

    STL_TRY( ellSetTime4ModifyTableElement( aTransID,
                                            aStmt,
                                            & sInitPlan->mTableHandle,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Cache Refine
     */
    
    STL_TRY( ellRefineCache4AlterPhysicalAttr( aTransID,
                                               &sInitPlan->mTableHandle,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} qlrAlterTablePhysicalAttr */
