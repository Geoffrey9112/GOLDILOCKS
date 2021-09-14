/*******************************************************************************
 * qlcrFetchCursor.c
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
 * @file qlcrFetchCursor.c
 * @brief FETCH CURSOR 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>
#include <qlcrFetchCursor.h>


/**
 * @addtogroup qlcrFetchCursor
 * @{
 */


/**
 * @brief FETCH CURSOR 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrValidateFetchCursor( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv )
{
    qlpFetchCursor       * sParseTree = NULL;
    qlcrInitFetchCursor  * sInitPlan = NULL;
    qlvStmtInfo            sStmtInfo;

    ellObjectList        * sObjList = NULL;
    
    /**
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_FETCH_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_FETCH_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpFetchCursor *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlcrInitFetchCursor),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlcrInitFetchCursor) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             (qlvInitPlan *) sInitPlan,
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * statement information 설정
     */

    sStmtInfo.mTransID       = aTransID;
    sStmtInfo.mDBCStmt       = aDBCStmt;
    sStmtInfo.mSQLStmt       = aSQLStmt;
    sStmtInfo.mSQLString     = aSQLString;
    sStmtInfo.mQueryExprList = NULL;
    

    /**********************************************************
     * Cursor Name Validation
     **********************************************************/

    /**
     * Cursor Name 존재 여부 확인
     */

    STL_TRY( ellFindCursorInstDesc( QLP_GET_PTR_VALUE( sParseTree->mCursorName ),
                                    ELL_DICT_OBJECT_ID_NA,
                                    & sInitPlan->mInstDesc,
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sInitPlan->mInstDesc != NULL, RAMP_ERR_CURSOR_NOT_EXIST );

    /**
     * ODBC 등에서 선언한 Cursor 는 OPEN 구문으로 접근할 수 없다.
     */
     
    STL_TRY_THROW( ellCursorHasOrigin( sInitPlan->mInstDesc ) == STL_TRUE,
                    RAMP_ERR_INVALID_DECLARED_CURSOR );
    
    /**
     * Cursor OPEN 여부는 검사하지 않는다.
     * 아직 OPEN 이 수행되지 않고, Prepare-Execute 로 수행할 수 있기 때문이다.
     */

    /**
     * 구문별 권한 검사가 이루어지며, 별도의 권한 검사 없음
     */
    
    /**********************************************************
     * Fetch Orientation Validation
     **********************************************************/

    sInitPlan->mFetchOrient = sParseTree->mFetchOrient;
    
    switch ( sParseTree->mFetchOrient )
    {
        case QLL_FETCH_NEXT:
            break;
        default:
            STL_TRY_THROW( sInitPlan->mInstDesc->mDeclDesc->mCursorProperty.mScrollability
                           == ELL_CURSOR_SCROLLABILITY_SCROLL,
                           RAMP_ERR_CURSOR_NOT_SCROLLABLE );
            break;
    }

    /**********************************************************
     * Fetch Position Validation
     **********************************************************/

    /**
     * Fetch Position
     */

    if( sParseTree->mFetchPosition == NULL )
    {
        sInitPlan->mFetchPosition = NULL;
        sInitPlan->mPositionValue = 0;
    }
    else
    {
        STL_TRY( ellInitObjectList( & sObjList, QLL_INIT_PLAN( aDBCStmt ), ELL_ENV(aEnv) ) == STL_SUCCESS );
        
        STL_TRY( qlvValidateValueExpr( & sStmtInfo,
                                       QLV_EXPR_PHRASE_OFFSETLIMIT, /* simple value specification */
                                       sObjList,                    /* only for param validate */
                                       NULL,                        /* aRefTableList */
                                       STL_FALSE,                   /* Atomic */
                                       STL_FALSE,                   /* Row Expr */
                                       0,                           /* Allowed Aggregation Depth */
                                       sParseTree->mFetchPosition,
                                       & sInitPlan->mFetchPosition,
                                       NULL,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_QUERY,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       QLL_INIT_PLAN( aDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        if( sInitPlan->mFetchPosition->mType == QLV_EXPR_TYPE_VALUE )
        {
            STL_TRY_THROW( dtlGetIntegerFromString(
                               sInitPlan->mFetchPosition->mExpr.mValue->mData.mString,
                               stlStrlen( sInitPlan->mFetchPosition->mExpr.mValue->mData.mString ),
                               & sInitPlan->mPositionValue,
                               QLL_ERROR_STACK( aEnv ) )
                           == STL_SUCCESS,
                           RAMP_ERR_INVALID_FETCH_POSITION );
        }
        else
        {
            STL_DASSERT( sInitPlan->mFetchPosition->mType == QLV_EXPR_TYPE_BIND_PARAM );
            sInitPlan->mPositionValue = 0;
        }
    }

    /**********************************************************
     * INTO 절 Validation
     **********************************************************/

    /**
     * OPEN 구문이 수행되지 않았을 경우 Query Target 을 알 수 없다.
     */
    
    sInitPlan->mIntoTargetCount = QLP_LIST_GET_COUNT(sParseTree->mIntoList->mParameters);
    
    /**
     * INTO 개수만큼 INTO 절의 Out Paramter 공간 확보
     */
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlvInitExpression) * sInitPlan->mIntoTargetCount,
                                (void **) & sInitPlan->mIntoTargetArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sInitPlan->mIntoTargetArray,
               0x00,
               STL_SIZEOF(qlvInitExpression) * sInitPlan->mIntoTargetCount );

    /**
     * INTO phrase validation
     */

    STL_TRY( qlvValidateIntoPhrase( & sStmtInfo,
                                    sInitPlan->mIntoTargetCount,
                                    sParseTree->mIntoList,
                                    sInitPlan->mIntoTargetArray,
                                    aEnv )
             == STL_SUCCESS );
    
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

    STL_CATCH( RAMP_ERR_CURSOR_NOT_SCROLLABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_IS_NOT_SCROLLABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCursorName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mCursorName ) );
    }

    STL_CATCH( RAMP_ERR_INVALID_FETCH_POSITION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FETCH_POSITION_MUST_BE_NUMERIC_VALUE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mFetchPosition->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief FETCH CURSOR 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrCodeOptimizeFetchCursor( smlTransId      aTransID,
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
 * @brief FETCH CURSOR 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrDataOptimizeFetchCursor( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    qlcrInitFetchCursor  * sInitPlan = NULL;
    qlcrDataFetchCursor  * sDataPlan = NULL;

    qllDataArea          * sOriginDataArea = NULL;
    qllStatement         * sOriginSQLStmt  = NULL;
    
    stlInt32               sOriginIntoCount      = 0;
    qlvRefExprList       * sOriginTargetExprList = NULL;
    qloExprInfo          * sOriginExprInfo       = NULL;

    qllOptimizationNode  * sOptNode       = NULL;

    qlnoSelectStmt       * sOptSelectStmt = NULL;
    qlnoInsertStmt       * sOptInsertStmt = NULL;
    qlnoUpdateStmt       * sOptUpdateStmt = NULL;
    qlnoDeleteStmt       * sOptDeleteStmt = NULL;
    
    knlValueBlockList    * sTargetBlock    = NULL;
    knlCastFuncInfo      * sIntoAssignInfo = NULL;
    stlInt32               i               = 0;

    knlValueBlockList    * sBindValueBlock = NULL;
    knlCastFuncInfo      * sFetchPositionCastFuncInfo = NULL;
    qloDBType              sFetchPositionDBType;


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_FETCH_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sInitPlan = (qlcrInitFetchCursor *) aSQLStmt->mInitPlan;

    /**
     * Data Plan 생성
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF(qlcrDataFetchCursor),
                                (void **) & sDataPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sDataPlan, 0x00, STL_SIZEOF(qlcrDataFetchCursor) );

    /**********************************************************
     * OPEN 여부 확인 
     **********************************************************/
    
    /**
     * FETCH 의 최초 수행 시점으므로 OPEN 구문은 수행되어 있어야 함
     */

    STL_TRY_THROW( ellCursorIsOpen( sInitPlan->mInstDesc ) == STL_TRUE,
                   RAMP_ERR_CURSOR_IS_NOT_OPEN );

    /**
     * ODBC 등에서 선언한 Cursor 는 OPEN 구문으로 접근할 수 없다.
     */
     
    STL_TRY_THROW( ellCursorHasOrigin( sInitPlan->mInstDesc ) == STL_TRUE,
                    RAMP_ERR_INVALID_DECLARED_CURSOR );
    
    
    /**********************************************************
     * Cursor Origin 정보 획득 
     **********************************************************/
    
    /**
     * Cursor Origin 정보 획득 
     */

    sOriginSQLStmt = (qllStatement *) sInitPlan->mInstDesc->mCursorSQLStmt;

    sDataPlan->mCursorSCN = sOriginSQLStmt->mViewSCN;
    
    switch (sOriginSQLStmt->mStmtType)
    {
        case QLL_STMT_SELECT_TYPE:
        case QLL_STMT_SELECT_FOR_UPDATE_TYPE :
            {
                sOptNode = (qllOptimizationNode*)sOriginSQLStmt->mCodePlan;
                sOptSelectStmt = (qlnoSelectStmt*)sOptNode->mOptNode;

                sOriginIntoCount        = sOptSelectStmt->mTargetCnt;
                sOriginTargetExprList   = sOptSelectStmt->mTargetColList;
                sOriginExprInfo         = sOptSelectStmt->mStmtExprList->mTotalExprInfo;
                break;
            }
        case QLL_STMT_INSERT_RETURNING_QUERY_TYPE:
            {
                sOptNode = (qllOptimizationNode*)sOriginSQLStmt->mCodePlan;
                sOptInsertStmt = (qlnoInsertStmt*)sOptNode->mOptNode;

                sOriginIntoCount        = sOptInsertStmt->mReturnExprCnt;
                sOriginTargetExprList   = sOptInsertStmt->mReturnExprList;
                sOriginExprInfo         = sOptInsertStmt->mStmtExprList->mTotalExprInfo;
                break;
            }
        case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE:
            {
                sOptNode = (qllOptimizationNode*)sOriginSQLStmt->mCodePlan;
                sOptUpdateStmt = (qlnoUpdateStmt*)sOptNode->mOptNode;

                sOriginIntoCount        = sOptUpdateStmt->mReturnExprCnt;
                sOriginTargetExprList   = sOptUpdateStmt->mReturnExprList;
                sOriginExprInfo         = sOptUpdateStmt->mStmtExprList->mTotalExprInfo;
                break;
            }
        case QLL_STMT_DELETE_RETURNING_QUERY_TYPE:
            {
                sOptNode = (qllOptimizationNode*)sOriginSQLStmt->mCodePlan;
                sOptDeleteStmt = (qlnoDeleteStmt*)sOptNode->mOptNode;

                sOriginIntoCount        = sOptDeleteStmt->mReturnExprCnt;
                sOriginTargetExprList   = sOptDeleteStmt->mReturnExprList;
                sOriginExprInfo         = sOptDeleteStmt->mStmtExprList->mTotalExprInfo;
                break;
            }
        default:
            STL_ASSERT(0);
            break;
    }
            
    /**********************************************************
     * FETCH INTO 정보 검증 
     **********************************************************/
    
    /**
     * Target 과 FETCH INTO 절의 개수가 동일한 지 확인 
     */

    STL_TRY_THROW( sOriginIntoCount == sInitPlan->mIntoTargetCount,
                   RAMP_ERR_INTO_PARAMETER_COUNT_MISMATCH );


    /***************************************************
     * INTO 절 Data Optimization
     ***************************************************/

    /**
     * INTO 절의 Data 영역 공간 확보
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF(qloDataOutParam) * sInitPlan->mIntoTargetCount,
                                (void **) & sDataPlan->mIntoTargetArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sDataPlan->mIntoTargetArray,
               0x00,
               STL_SIZEOF(qloDataOutParam) * sInitPlan->mIntoTargetCount );
    
    /**
     * INTO 절의 Data Optimization
     */

    sOriginDataArea = (qllDataArea*)(sOriginSQLStmt->mDataPlan);
    STL_TRY( qlndDataOptIntoPhrase( aSQLStmt,
                                    sOriginExprInfo,
                                    aSQLStmt->mBindContext,
                                    sInitPlan->mIntoTargetCount,
                                    sOriginTargetExprList,
                                    sInitPlan->mIntoTargetArray,
                                    sOriginDataArea,
                                    & QLL_DATA_PLAN(aDBCStmt),
                                    aEnv )
             == STL_SUCCESS );


    /**
     * DataArea에 있는 Into절에 대한 Value Block과 Assign Info를 
     * Fetch Cursor의 DataPlan에 있는 Out Bind Param에 연결
     */

    sIntoAssignInfo = sOriginDataArea->mIntoAssignInfo;
    for( sTargetBlock = sOriginDataArea->mIntoBlock, i = 0;
         sTargetBlock != NULL;
         sTargetBlock = sTargetBlock->mNext, i++ )
    {
        sDataPlan->mIntoTargetArray[i].mOutBlock = sTargetBlock;
        sDataPlan->mIntoTargetArray[i].mAssignInfo = sIntoAssignInfo[i];
    }

    sDataPlan->mHasData = STL_FALSE;
    sDataPlan->mValidBlockIdx = 0;

    
    /**********************************************************
     * FETCH INTO 정보 검증 
     **********************************************************/
    
    if( sInitPlan->mFetchPosition == NULL )
    {
        sDataPlan->mCastFuncInfo = NULL;
    }
    else 
    {
        if( sInitPlan->mFetchPosition->mType != QLV_EXPR_TYPE_VALUE )
        {
            /**
             * Cast Argument 공간 할당
             */
            
            STL_TRY( knlGetExecuteParamValueBlock( aSQLStmt->mBindContext,
                                                   sInitPlan->mFetchPosition->mExpr.mBindParam->mBindParamIdx + 1,
                                                   & sBindValueBlock,
                                                   KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
            
            STL_DASSERT( sBindValueBlock != NULL );

            /*
             * cast 가능 여부
             */
            STL_TRY_THROW( dtlIsApplicableCast(
                               KNL_GET_BLOCK_DB_TYPE( sBindValueBlock ),
                               KNL_GET_BLOCK_INTERVAL_INDICATOR( sBindValueBlock ),
                               DTL_TYPE_NATIVE_BIGINT,
                               DTL_INTERVAL_INDICATOR_NA ) == STL_TRUE,
                           RAMP_ERR_INVALID_FETCH_POSITION );
            
            sFetchPositionDBType.mDBType = DTL_TYPE_NATIVE_BIGINT;
            sFetchPositionDBType.mSourceDBType = KNL_GET_BLOCK_DB_TYPE( sBindValueBlock );
            sFetchPositionDBType.mIsNeedCast = STL_TRUE;
            sFetchPositionDBType.mCastFuncIdx = DTL_GET_CAST_FUNC_PTR_IDX( sFetchPositionDBType.mSourceDBType, sFetchPositionDBType.mDBType);
            sFetchPositionDBType.mArgNum1 = DTL_PRECISION_NA;
            sFetchPositionDBType.mArgNum2 = DTL_SCALE_NA;
            sFetchPositionDBType.mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
            sFetchPositionDBType.mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
            
            sFetchPositionDBType.mSourceArgNum1 = KNL_GET_BLOCK_ARG_NUM1( sBindValueBlock );
            sFetchPositionDBType.mSourceArgNum2 = KNL_GET_BLOCK_ARG_NUM2( sBindValueBlock );
            sFetchPositionDBType.mSourceStringLengthUnit = KNL_GET_BLOCK_STRING_LENGTH_UNIT( sBindValueBlock );
            sFetchPositionDBType.mSourceIntervalIndicator = KNL_GET_BLOCK_INTERVAL_INDICATOR( sBindValueBlock );
            
            STL_TRY( qloDataSetInternalCastInfo( aSQLStmt,
                                                 & QLL_DATA_PLAN( aDBCStmt ),
                                                 & sFetchPositionDBType,
                                                 & sFetchPositionCastFuncInfo,
                                                 STL_TRUE,  /* aIsAllocCastFuncInfo */
                                                 STL_FALSE, /* aIsAllocCastResultBuf */
                                                 aEnv )
                     == STL_SUCCESS );
            
            sDataPlan->mCastFuncInfo = sFetchPositionCastFuncInfo;
        }
        else
        {
            sDataPlan->mCastFuncInfo = NULL;
        }
    }
    

    /**********************************************************
     * Data Plan 연결 
     **********************************************************/

    aSQLStmt->mDataPlan = (void *) sDataPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CURSOR_IS_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_IS_NOT_OPEN,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mInstDesc->mCursorKey.mCursorName );
    }

    STL_CATCH( RAMP_ERR_INVALID_DECLARED_CURSOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_NOT_DECLARED_CURSOR,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mInstDesc->mCursorKey.mCursorName );
    }

    
    STL_CATCH( RAMP_ERR_INTO_PARAMETER_COUNT_MISMATCH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NUMBER_OF_ELEMENTS_INTO_LIST_MISMATCH,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_FETCH_POSITION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FETCH_POSITION_MUST_BE_NUMERIC_VALUE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
                   
    STL_FINISH;

    return STL_FAILURE;
}






/**
 * @brief FETCH CURSOR 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrExecuteFuncFetchCursor( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv )
{
    qlcrInitFetchCursor * sInitPlan = NULL;
    qlcrDataFetchCursor * sDataPlan = NULL;

    knlValueBlockList * sTargetBlock = NULL;
    dtlDataValue      * sTargetValue = NULL;
    dtlDataValue      * sIntoValue = NULL;

    stlInt64    sFetchPosition = 0;

    qlvInitBindParam      * sBindParam = NULL;
    knlValueBlockList     * sValueBlock = NULL;
    dtlDataValue            sDataValue;

    qllCursorRowStatus  sCursorRowStatus = QLL_CURSOR_ROW_STATUS_NA;
    
    ellCursorInstDesc * sInstDesc = NULL;

    qllStatement      * sCursorSQLStmt = NULL;
    
    qllResultSetDesc  * sResultSetDesc = NULL;

    stlInt32   i = 0;
    
    /**
     * Parameter Validation
     */

    /* FETCH 구문은 SM statement 를 내부에서 할당하여 사용 */
    STL_PARAM_VALIDATE( aStmt == NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_FETCH_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mDataPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sInitPlan = (qlcrInitFetchCursor *) aSQLStmt->mInitPlan;
    sDataPlan = (qlcrDataFetchCursor *) aSQLStmt->mDataPlan;

    sInstDesc = sInitPlan->mInstDesc;
    sCursorSQLStmt = (qllStatement *) sInstDesc->mCursorSQLStmt;

    /**********************************************************
     * OPEN 여부 확인 
     **********************************************************/
    
    /**
     * FETCH 수행을 할 때마다 OPEN 되어 있는지 확인해야 함.
     */

    STL_TRY_THROW( ellCursorIsOpen( sInstDesc ) == STL_TRUE,
                   RAMP_ERR_CURSOR_IS_NOT_OPEN );

    /**
     * ODBC 등에서 선언한 Cursor 는 OPEN 구문으로 접근할 수 없다.
     */
     
    STL_TRY_THROW( ellCursorHasOrigin( sInitPlan->mInstDesc ) == STL_TRUE,
                    RAMP_ERR_INVALID_DECLARED_CURSOR );

    /**
     * Cursor 가 변경되었는 지 확인
     */

    if ( sCursorSQLStmt->mViewSCN != sDataPlan->mCursorSCN )
    {
        /* re-open 된 cursor 임 */
        
        /**
         * FETCH 구문을 Re-Compile 함
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
        
        sInitPlan = (qlcrInitFetchCursor *) aSQLStmt->mInitPlan;
        sDataPlan = (qlcrDataFetchCursor *) aSQLStmt->mDataPlan;
        
        sInstDesc = sInitPlan->mInstDesc;
        sCursorSQLStmt = (qllStatement *) sInstDesc->mCursorSQLStmt;
    }
    else
    {
        /* nothing to do */
    }
    
    /******************************************************
     * Fetch Position Value 계산 
     ******************************************************/

    if( sInitPlan->mFetchPosition == NULL )
    {
        sFetchPosition = 0;
    }
    else if( sInitPlan->mFetchPosition->mType == QLV_EXPR_TYPE_VALUE )
    {
        sFetchPosition = sInitPlan->mPositionValue;
    }
    else
    {
        /**
         * Host Variable 인 경우
         */
        
        STL_DASSERT( sInitPlan->mFetchPosition->mType == QLV_EXPR_TYPE_BIND_PARAM );
        STL_DASSERT( sDataPlan->mCastFuncInfo != NULL );

        sBindParam  = sInitPlan->mFetchPosition->mExpr.mBindParam;
        sValueBlock = aSQLStmt->mBindContext->mMap[ sBindParam->mBindParamIdx ]->mINValueBlock;

        sDataPlan->mCastFuncInfo->mArgs[ 0 ].mValue.mDataValue = KNL_GET_BLOCK_DATA_VALUE( sValueBlock, 0 );
        STL_TRY_THROW( DTL_IS_NULL( sDataPlan->mCastFuncInfo->mArgs[ 0 ].mValue.mDataValue ) == STL_FALSE,
                       RAMP_ERR_INVALID_FETCH_POSITION );

        sDataValue.mType   = DTL_TYPE_NATIVE_BIGINT;
        sDataValue.mValue  = (void *) & sFetchPosition;
        sDataValue.mBufferSize = STL_SIZEOF(stlInt64);
        sDataValue.mLength = 0;
        
        STL_TRY_THROW( sDataPlan->mCastFuncInfo->mCastFuncPtr(
                           DTL_CAST_INPUT_ARG_CNT,
                           sDataPlan->mCastFuncInfo->mArgs,
                           & sDataValue,
                           (void *)(sDataPlan->mCastFuncInfo->mSrcTypeInfo),
                           KNL_DT_VECTOR(aEnv),
                           aEnv,
                           QLL_ERROR_STACK( aEnv ) )
                       == STL_SUCCESS,
                       RAMP_ERR_INVALID_FETCH_POSITION );
    }
    
    /******************************************************
     * Fetch One Row 를 수행 
     ******************************************************/

    /**
     * Result Descriptor 를 이용해 하나의 Row 를 읽는다
     * - Instance Descriptor 에 저장되어 있는 Statement 정보를 사용한다.
     */

    sResultSetDesc = (qllResultSetDesc *) sCursorSQLStmt->mResultSetDesc;
    
    STL_TRY( qllFetchCursorOneRow( sInstDesc->mCursorStmt->mSmlTransId,
                                   sInstDesc->mCursorStmt,
                                   (qllDBCStmt *) sInstDesc->mCursorDBCStmt,
                                   sResultSetDesc,
                                   sInitPlan->mFetchOrient,
                                   sFetchPosition,
                                   & sCursorRowStatus,
                                   & sDataPlan->mValidBlockIdx,
                                   aEnv )
             == STL_SUCCESS );
    
    /******************************************************
     * INTO 절을 처리 
     ******************************************************/

    if ( (sDataPlan->mValidBlockIdx < 0) || (sCursorRowStatus == QLL_CURSOR_ROW_STATUS_DELETED) )
    {
        /**
         * Fetch Row 결과가 없거나 삭제된 경우
         */

        sDataPlan->mHasData = STL_FALSE;
    }
    else
    {
        sDataPlan->mHasData = STL_TRUE;
        
        for ( sTargetBlock = sResultSetDesc->mTargetBlock, i = 0;
              sTargetBlock != NULL;
              sTargetBlock = sTargetBlock->mNext, i++ )
        {
            /**
             * TARGET Value 공간 획득
             */
        
            sTargetValue =
                KNL_GET_BLOCK_DATA_VALUE( sTargetBlock, sDataPlan->mValidBlockIdx );

            /**
             * TARGET value 를 INTO 에 저장
             */

            sIntoValue = KNL_GET_BLOCK_DATA_VALUE(sDataPlan->mIntoTargetArray[i].mOutBlock, 0);
            
            if ( DTL_IS_NULL( sTargetValue ) == STL_TRUE )
            {
                DTL_SET_NULL( sIntoValue );
            }
            else
            {
                sDataPlan->mIntoTargetArray[i].mAssignInfo.mArgs[0].mValue.mDataValue =
                    sTargetValue;

                STL_TRY( sDataPlan->mIntoTargetArray[i].mAssignInfo.mCastFuncPtr(
                             KNL_CAST_FUNC_ARG_COUNT,
                             sDataPlan->mIntoTargetArray[i].mAssignInfo.mArgs,
                             sIntoValue,
                             (void *)(sDataPlan->mIntoTargetArray[i].mAssignInfo.mSrcTypeInfo),
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
            }
        }
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CURSOR_IS_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_IS_NOT_OPEN,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInstDesc->mCursorKey.mCursorName );
    }

    STL_CATCH( RAMP_ERR_INVALID_DECLARED_CURSOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_NOT_DECLARED_CURSOR,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mInstDesc->mCursorKey.mCursorName );
    }

    STL_CATCH( RAMP_ERR_INVALID_FETCH_POSITION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FETCH_POSITION_MUST_BE_NUMERIC_VALUE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}
    





/** @} qlcrFetchCursor */
