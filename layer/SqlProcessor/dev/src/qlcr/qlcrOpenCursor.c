/*******************************************************************************
 * qlcrOpenCursor.c
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
 * @file qlcrOpenCursor.c
 * @brief OPEN CURSOR 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>
#include <qlcrOpenCursor.h>


/**
 * @addtogroup qlcrOpenCursor
 * @{
 */


/**
 * @brief OPEN CURSOR 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrValidateOpenCursor( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{
    stlBool  sIsOpenCursor = STL_FALSE;
    
    qlpOpenCursor      * sParseTree = NULL;
    qlcrInitOpenCursor * sInitPlan = NULL;

    qllNode            * sCursorParseTree = NULL;
    
    stlChar  * sString = NULL;
    stlInt32   sStrLen = 0;

    qllDBCStmt   * sInstantDBCStmt = NULL;
    qllStatement * sInstantSQLStmt = NULL;
    
    ellCursorDeclDesc  * sDeclDesc = NULL;

    /**
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_OPEN_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_OPEN_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpOpenCursor *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlcrInitOpenCursor),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlcrInitOpenCursor) );

    /**********************************************************
     * Cursor Name Validation
     **********************************************************/

    /**
     * Cursor Key 정보 생성 
     */
    
    sString = QLP_GET_PTR_VALUE( sParseTree->mCursorName );
    sStrLen = stlStrlen(sString);

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                sStrLen + 1,
                                (void **) & sInitPlan->mCursorName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemcpy( sInitPlan->mCursorName, sString, sStrLen );
    sInitPlan->mCursorName[sStrLen] = '\0';

    sInitPlan->mProcID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Cursor Name 존재 여부 확인
     */

    STL_TRY( ellFindCursorInstDesc( sInitPlan->mCursorName,
                                    sInitPlan->mProcID,
                                    & sInitPlan->mInstDesc,
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sInitPlan->mInstDesc != NULL, RAMP_ERR_CURSOR_NOT_EXIST );

    /**
     * Cursor CLOSED 여부 확인
     */
    
    STL_TRY_THROW( ellCursorIsOpen( sInitPlan->mInstDesc ) == STL_FALSE,
                   RAMP_ERR_CURSOR_ALREADY_OPENED );

    /**
     * ODBC 등에서 선언한 Cursor 는 OPEN 구문으로 접근할 수 없다.
     */
     
    STL_TRY_THROW( ellCursorHasOrigin( sInitPlan->mInstDesc ) == STL_TRUE,
                    RAMP_ERR_INVALID_DECLARED_CURSOR );
    
    /**********************************************************
     * Cursor Origin Parsing
     **********************************************************/

    sDeclDesc = sInitPlan->mInstDesc->mDeclDesc;

    switch ( sDeclDesc->mOriginType )
    {
        case ELL_CURSOR_ORIGIN_STANDING_CURSOR:
            {
                sStrLen = stlStrlen(sDeclDesc->mCursorOrigin);
                STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                            sStrLen + 1,
                                            (void **) & sInitPlan->mCursorQuery,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                stlMemcpy( sInitPlan->mCursorQuery, sDeclDesc->mCursorOrigin, sStrLen );
                sInitPlan->mCursorQuery[sStrLen] = '\0';
                break;
            }
        case ELL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR:
        case ELL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR:
        case ELL_CURSOR_ORIGIN_RECEIVED_CURSOR:
            {
                /**
                 * @todo Standing Cursor 이외의 Cursor를 구현해야 함.
                 */
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**********************************************************
     * Instant Cursor 의 DBC stmt 와 SQL stmt 정보 획득
     **********************************************************/

    sDeclDesc = sInitPlan->mInstDesc->mDeclDesc;

    sInstantDBCStmt = (qllDBCStmt *)   sInitPlan->mInstDesc->mCursorDBCStmt;
    sInstantSQLStmt = (qllStatement *) sInitPlan->mInstDesc->mCursorSQLStmt;

    /**********************************************************
     * Instant Cursor 의 SQL stmt 의 Parsing => Validation => Optimization
     **********************************************************/
    
    /**
     * Instant Cursor 의 Parsing
     */
     
    STL_TRY( qllResetSQL( sInstantDBCStmt, sInstantSQLStmt, aEnv ) == STL_SUCCESS );

    /**
     * Instant Cursor 의 SQL Statement 의 Cursor Property 설정
     */

    qllSetSQLStmtCursorProperty( sInstantSQLStmt,
                                 sDeclDesc->mCursorProperty.mStandardType,
                                 sDeclDesc->mCursorProperty.mSensitivity,
                                 sDeclDesc->mCursorProperty.mScrollability,
                                 sDeclDesc->mCursorProperty.mHoldability,
                                 sDeclDesc->mCursorProperty.mUpdatability,
                                 sDeclDesc->mCursorProperty.mReturnability );
    sInstantSQLStmt->mStmtCursorProperty.mIsDbcCursor = STL_FALSE;

    /**
     * Cursor Origin 의 Parsing
     */
    
    STL_TRY( qlgParseSQL( sInstantDBCStmt,
                          sInstantSQLStmt,
                          QLL_PHASE_VALIDATION,
                          sInitPlan->mCursorQuery,
                          STL_FALSE, /* aIsRecompile */
                          STL_TRUE,  /* Search Plan Cache */
                          & sCursorParseTree,
                          aEnv )
             == STL_SUCCESS );
    
    /**
     * Cursor Query 의 Statement 정보 기록
     */

    sInitPlan->mCursorStmtAttr = sInstantSQLStmt->mStmtAttr;
    sInitPlan->mCursorStmtType = sInstantSQLStmt->mStmtType;
    
    /**********************************************************
     * Init Plan 연결 
     **********************************************************/

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CURSOR_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCursorName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mCursorName ) );
    }
    
    STL_CATCH( RAMP_ERR_CURSOR_ALREADY_OPENED )
    {
        sIsOpenCursor = STL_TRUE;
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_ALREADY_OPEN,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCursorName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mCursorName ) );
    }

    STL_CATCH( RAMP_ERR_INVALID_DECLARED_CURSOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_NOT_DECLARED_CURSOR,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCursorName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mCursorName ) );
    }

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlcrValidateOpenCursor()" );
    }
    
    STL_FINISH;

    if ( sIsOpenCursor == STL_TRUE )
    {
        /* OPEN 된 상태임 */
    }
    else
    {
        if ( sInstantSQLStmt != NULL )
        {
            (void) qllResetSQL( sInstantDBCStmt, sInstantSQLStmt, aEnv );
        }
    }

    return STL_FAILURE;
}

/**
 * @brief OPEN CURSOR 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrCodeOptimizeOpenCursor( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv )
{
    stlBool  sIsOpenCursor = STL_FALSE;
    
    qlcrInitOpenCursor * sInitPlan = NULL;

    qllDBCStmt   * sInstantDBCStmt = NULL;
    qllStatement * sInstantSQLStmt = NULL;
    
    ellCursorDeclDesc  * sDeclDesc = NULL;
    
    qllNode      * sInstantParseTree = NULL;
    
    /**
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_OPEN_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sInitPlan = (qlcrInitOpenCursor *) aSQLStmt->mInitPlan;

    /**********************************************************
     * Cursor 가 이미 OPEN 되어 있는지 검사 
     **********************************************************/

    /**
     * Cursor CLOSED 여부 확인
     */

    STL_TRY_THROW( ellCursorIsOpen( sInitPlan->mInstDesc ) == STL_FALSE,
                   RAMP_ERR_CURSOR_ALREADY_OPEN );

    /**
     * ODBC 등에서 선언한 Cursor 는 OPEN 구문으로 접근할 수 없다.
     */
     
    STL_TRY_THROW( ellCursorHasOrigin( sInitPlan->mInstDesc ) == STL_TRUE,
                    RAMP_ERR_INVALID_DECLARED_CURSOR );
    
    /**********************************************************
     * Instant Cursor 의 DBC stmt 와 SQL stmt 정보 획득
     **********************************************************/

    sDeclDesc = sInitPlan->mInstDesc->mDeclDesc;

    sInstantDBCStmt = (qllDBCStmt *)   sInitPlan->mInstDesc->mCursorDBCStmt;
    sInstantSQLStmt = (qllStatement *) sInitPlan->mInstDesc->mCursorSQLStmt;

    
    /**********************************************************
     * Instant Cursor 의 SQL stmt 의 Parsing => Validation => Optimization
     **********************************************************/
    
    /**
     * Instant Cursor 의 Parsing
     */
     
    STL_TRY( qllResetSQL( sInstantDBCStmt, sInstantSQLStmt, aEnv ) == STL_SUCCESS );

    /**
     * Instant Cursor 의 SQL Statement 의 Cursor Property 설정
     */

    qllSetSQLStmtCursorProperty( sInstantSQLStmt,
                                 sDeclDesc->mCursorProperty.mStandardType,
                                 sDeclDesc->mCursorProperty.mSensitivity,
                                 sDeclDesc->mCursorProperty.mScrollability,
                                 sDeclDesc->mCursorProperty.mHoldability,
                                 sDeclDesc->mCursorProperty.mUpdatability,
                                 sDeclDesc->mCursorProperty.mReturnability );
    sInstantSQLStmt->mStmtCursorProperty.mIsDbcCursor = STL_FALSE;

    /**
     * Parsing Cursor Query
     */
    
    STL_TRY( qlgParseSQL( sInstantDBCStmt,
                          sInstantSQLStmt,
                          QLL_PHASE_CODE_OPTIMIZE,
                          sInitPlan->mCursorQuery,
                          STL_FALSE, /* aIsRecompile */
                          STL_TRUE,  /* Search Plan Cache */
                          & sInstantParseTree,
                          aEnv )
             == STL_SUCCESS );

    /**
     * Memory Allocator를 선정한다.
     */
    
    if( qllIsCacheableStmt( sInstantSQLStmt, aEnv ) == STL_TRUE )
    {
        qllPlanCacheMemON( sInstantDBCStmt );
    }
    else
    {
        qllPlanCacheMemOFF( sInstantDBCStmt );
    }
    
    /**
     * Instant Cursor SQL Statemnet 의 View SCN 설정
     * - optimization 단계에서 validation 을 수행하므로 View SCN 을 재설정해야 함.
     */
    
    sInstantSQLStmt->mViewSCN = smlGetSystemScn();
    sInstantSQLStmt->mStmt = NULL;

    /**
     * Instant Cursor 의 Validation 
     */
    
    STL_TRY( qlgValidateSQL( aTransID,
                             sInstantDBCStmt,
                             sInstantSQLStmt,
                             QLL_PHASE_CODE_OPTIMIZE,
                             sInitPlan->mCursorQuery,
                             sInstantParseTree,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Instant Cursor 의 Code Optimization
     */
    
    sInstantSQLStmt->mBindContext = aSQLStmt->mBindContext; 
    STL_TRY( qlgOptimizeCodeSQL( aTransID,
                                 sInstantDBCStmt,
                                 sInstantSQLStmt,
                                 QLL_PHASE_CODE_OPTIMIZE,
                                 aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Code Plan 연결 
     **********************************************************/

    aSQLStmt->mCodePlan = NULL;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CURSOR_ALREADY_OPEN )
    {
        sIsOpenCursor = STL_TRUE;
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_ALREADY_OPEN,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }

    STL_CATCH( RAMP_ERR_INVALID_DECLARED_CURSOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_NOT_DECLARED_CURSOR,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }
    
    STL_FINISH;

    if ( sIsOpenCursor == STL_TRUE )
    {
        /* open 된 cursor 임 */
    }
    else
    {
        if ( sInstantSQLStmt != NULL )
        {
            (void) qllResetSQL( sInstantDBCStmt, sInstantSQLStmt, aEnv );
        }
    }

    return STL_FAILURE;
}

/**
 * @brief OPEN CURSOR 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrDataOptimizeOpenCursor( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv )
{
    stlBool  sIsOpenCursor = STL_FALSE;
    
    qlcrInitOpenCursor * sInitPlan = NULL;

    qllDBCStmt   * sInstantDBCStmt = NULL;
    qllStatement * sInstantSQLStmt = NULL;
    
    /**
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_OPEN_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sInitPlan = (qlcrInitOpenCursor *) aSQLStmt->mInitPlan;

    /**********************************************************
     * Cursor 가 이미 OPEN 되어 있는지 검사 
     **********************************************************/

    /**
     * Cursor CLOSED 여부 확인
     */
    
    STL_TRY_THROW( ellCursorIsOpen( sInitPlan->mInstDesc ) == STL_FALSE,
                   RAMP_ERR_CURSOR_ALREADY_OPEN );

    /**
     * ODBC 등에서 선언한 Cursor 는 OPEN 구문으로 접근할 수 없다.
     */
     
    STL_TRY_THROW( ellCursorHasOrigin( sInitPlan->mInstDesc ) == STL_TRUE,
                    RAMP_ERR_INVALID_DECLARED_CURSOR );
    
    /**********************************************************
     * Instant Cursor 의 DBC stmt 와 SQL stmt 정보 획득
     **********************************************************/

    sInstantDBCStmt = (qllDBCStmt *)   sInitPlan->mInstDesc->mCursorDBCStmt;
    sInstantSQLStmt = (qllStatement *) sInitPlan->mInstDesc->mCursorSQLStmt;

    /**********************************************************
     * Cursor Origin 의 Data Optimization
     **********************************************************/
    
    /**
     * Cursor Origin 의 Data Optimization
     */

    sInstantSQLStmt->mBindContext = aSQLStmt->mBindContext; 
    
    STL_TRY( qlgOptimizeDataSQL( aTransID,
                                 sInstantDBCStmt,
                                 sInstantSQLStmt,
                                 QLL_PHASE_DATA_OPTIMIZE,
                                 aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Data Plan 연결 
     **********************************************************/

    aSQLStmt->mDataPlan = NULL;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CURSOR_ALREADY_OPEN )
    {
        sIsOpenCursor = STL_TRUE;
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_ALREADY_OPEN,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }

    STL_CATCH( RAMP_ERR_INVALID_DECLARED_CURSOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_NOT_DECLARED_CURSOR,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }
    
    STL_FINISH;

    if ( sIsOpenCursor == STL_TRUE )
    {
        /* OPEN 된 Cursor 임 */
    }
    else
    {
        if ( sInstantSQLStmt != NULL )
        {
            (void) qllResetSQL( sInstantDBCStmt, sInstantSQLStmt, aEnv );
        }
    }
    
    return STL_FAILURE;
}






/**
 * @brief OPEN CURSOR 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrExecuteFuncOpenCursor( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv )
{
    stlInt32 sState = 0;

    stlBool sIsOpenCursor = STL_FALSE;
    
    qlcrInitOpenCursor * sInitPlan = NULL;

    qllDBCStmt   * sInstantDBCStmt = NULL;
    qllStatement * sInstantSQLStmt = NULL;
    
    qllAccessMode            sStmtAccessMode = QLL_ACCESS_MODE_NONE;
    stlInt32                 sStmtAttr = 0;
    smlStatement           * sSmStmt = NULL;

    /**
     * Parameter Validation
     */

    /* OPEN 구문은 SM statement 를 내부에서 할당하여 사용 */
    STL_PARAM_VALIDATE( aStmt == NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_OPEN_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sInitPlan = (qlcrInitOpenCursor *) aSQLStmt->mInitPlan;
    
    sInstantDBCStmt = (qllDBCStmt *)   sInitPlan->mInstDesc->mCursorDBCStmt;
    sInstantSQLStmt = (qllStatement *) sInitPlan->mInstDesc->mCursorSQLStmt;

    /**********************************************************
     * Cursor 가 이미 OPEN 되어 있는지 검사 
     **********************************************************/

    /**
     * Cursor CLOSED 여부 확인
     */
    
    STL_TRY_THROW( ellCursorIsOpen( sInitPlan->mInstDesc ) == STL_FALSE,
                   RAMP_ERR_CURSOR_ALREADY_OPEN );

    /**
     * ODBC 등에서 선언한 Cursor 는 OPEN 구문으로 접근할 수 없다.
     */
     
    STL_TRY_THROW( ellCursorHasOrigin( sInitPlan->mInstDesc ) == STL_TRUE,
                    RAMP_ERR_INVALID_DECLARED_CURSOR );
    
    /**********************************************************
     * Instant Cursor 의 DBC stmt 와 SQL stmt 정보 획득
     **********************************************************/

    if ( sInstantSQLStmt->mCodePlan == NULL )
    {
        /**
         * OPEN 이 수행된 후 CLOSE 가 호출됨.
         * OPEN 구문을 Recompile 해야 함.
         */

        STL_TRY( qlgRecompileSQL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  STL_FALSE,  /* aSearchPlanCache */
                                  QLL_PHASE_EXECUTE,
                                  aEnv ) == STL_SUCCESS );

        /**
         * 정보를 다시 획득
         */
        
        sInitPlan = (qlcrInitOpenCursor *) aSQLStmt->mInitPlan;
        
        sInstantDBCStmt = (qllDBCStmt *)   sInitPlan->mInstDesc->mCursorDBCStmt;
        sInstantSQLStmt = (qllStatement *) sInitPlan->mInstDesc->mCursorSQLStmt;
    }
    else
    {
        /**
         * Execution 이 진행 중임
         */
    }
    
    /**********************************************************
     * Instant Cursor 를 수행 
     **********************************************************/

    /**
     * OPEN 구문은 Cursor Origin 구문 유형에 따라 SM statement 를 내부에서 할당하여 사용함.
     */

    sStmtAccessMode = qllGetAccessMode( sInstantSQLStmt );
    
    if( qllGetTransAccessMode( aEnv ) == QLL_ACCESS_MODE_READ_ONLY )
    {
        STL_TRY_THROW( sStmtAccessMode != QLL_ACCESS_MODE_READ_WRITE,
                       RAMP_ERR_TRANSACTION_IS_READ_ONLY );
    }
    
    switch( sStmtAccessMode )
    {
        case QLL_ACCESS_MODE_READ_WRITE :
            /**
             * DML은 qp로부터 aUpdateRelHandle을 얻어와 정확한 값을 설정해야 한다.
             */
            
            sStmtAttr = SML_STMT_ATTR_UPDATABLE;
            if( qllIsLockableStmt( sInstantSQLStmt ) == STL_TRUE )
            {
                sStmtAttr |= SML_STMT_ATTR_LOCKABLE;
            }
            
            STL_TRY( smlAllocStatement(
                         aTransID,
                         NULL, /* aUpdateRelHandle, qlgExecuteSQL()에서 설정함 */
                         sStmtAttr,
                         SML_LOCK_TIMEOUT_INFINITE,
                         STL_TRUE, /* aNeedSnapshotIterator */
                         &sSmStmt,
                         SML_ENV( aEnv ) ) == STL_SUCCESS );
            break;
        case QLL_ACCESS_MODE_READ_ONLY :
            
            sStmtAttr = SML_STMT_ATTR_READONLY;
            if( qllIsLockableStmt( sInstantSQLStmt ) == STL_TRUE )
            {
                sStmtAttr |= SML_STMT_ATTR_LOCKABLE;
            }

            if ( sInstantSQLStmt->mResultSetDesc->mCursorProperty.mHoldability
                 == ELL_CURSOR_HOLDABILITY_WITH_HOLD )
            {
                sStmtAttr |= SML_STMT_ATTR_HOLDABLE;
            }
            
            STL_TRY( smlAllocStatement( aTransID,
                                        NULL, /* aUpdateRelHandle */
                                        sStmtAttr,
                                        SML_LOCK_TIMEOUT_INVALID,
                                        STL_TRUE, /* aNeedSnapshotIterator */
                                        &sSmStmt,
                                        SML_ENV( aEnv ) ) == STL_SUCCESS );
            break;
        case QLL_ACCESS_MODE_UNKNOWN:
            STL_ASSERT(0);
            break;
        default:
            STL_ASSERT(0);
            break;
    }
    sState = 1;

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLCR_EXECUTE_OPEN_CURSOR_BEFORE_QUERY_EXECUTE, KNL_ENV(aEnv) );

    /**
     * Instant Cursor SQL Statement 의 View SCN 설정
     */
    
    /* recompile 시 View SCN 설정을 위한 statement 연결 */
    sInstantSQLStmt->mStmt = sSmStmt;
    
    /* snapshot iterator 를 사용하는 경우 */
    sInstantSQLStmt->mViewSCN = sSmStmt->mScn;

    /**
     * Instant Cursor SQL 수행 
     */
    
    STL_TRY( qlgExecuteSQL( aTransID,
                            sSmStmt,
                            sInstantDBCStmt,
                            sInstantSQLStmt,
                            QLL_PHASE_EXECUTE,
                            aEnv )
             == STL_SUCCESS );

    /**********************************************************
     * Cursor 를 OPEN 상태로 변경 
     **********************************************************/

    /**
     * Cursor 를 OPEN 상태로 설정
     */

    STL_TRY( qlcrSetNamedCursorOpen( sSmStmt,
                                     sInstantDBCStmt,
                                     sInstantSQLStmt,
                                     sInitPlan->mInstDesc,
                                     aEnv )
             == STL_SUCCESS );
             
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CURSOR_ALREADY_OPEN )
    {
        sIsOpenCursor = STL_TRUE;
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_ALREADY_OPEN,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }

    STL_CATCH( RAMP_ERR_INVALID_DECLARED_CURSOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_NOT_DECLARED_CURSOR,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mCursorName );
    }
    
    STL_CATCH( RAMP_ERR_TRANSACTION_IS_READ_ONLY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TRANSACTION_IS_READ_ONLY,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if ( sIsOpenCursor == STL_TRUE )
    {
        /* OPEN 된 Cursor 임 */
    }
    else
    {
        if ( sInstantSQLStmt != NULL )
        {
            (void) qllResetSQL( sInstantDBCStmt, sInstantSQLStmt, aEnv );
        }
    }

    switch (sState)
    {
        case 1:
            (void)smlFreeStatement( sSmStmt, SML_ENV( aEnv ) );
        default:
            break;
    }
    
    return STL_FAILURE;
}
    

/**
 * @brief Named Cursor 가 OPEN 상태임을 설정함
 * @param[in]  aCursorStmt      Cursor Statement
 * @param[in]  aCursorDBCStmt   Cursor DBC Statement
 * @param[in]  aCursorSQLStmt   Cursor SQL Statement
 * @param[in]  aCursorInstDesc  Named Cursor Instance Descriptor
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlcrSetNamedCursorOpen( smlStatement           * aCursorStmt,
                                  qllDBCStmt             * aCursorDBCStmt,
                                  qllStatement           * aCursorSQLStmt,
                                  ellCursorInstDesc      * aCursorInstDesc,
                                  qllEnv                 * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCursorStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorInstDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorSQLStmt->mResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Result Set Descriptor 에 named cursor 의 instance descriptor 연결
     */

    aCursorSQLStmt->mResultSetDesc->mInstDesc = aCursorInstDesc;

    /**
     * Cursor 가 OPEN 되었음을 Session Object Manager 에 등록
     */

    ellCursorSetOpen( aCursorInstDesc,
                      aCursorStmt,
                      aCursorDBCStmt,
                      aCursorSQLStmt,
                      & aCursorSQLStmt->mResultSetDesc->mCursorProperty,
                      aCursorSQLStmt->mRetrySQL,
                      ELL_ENV(aEnv) );
                   
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlcrOpenCursor */
