/*******************************************************************************
 * qlrCreateTableAsSelect.c
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
 * @file qlrCreateTableAsSelect.c
 * @brief CREATE TABLE AS SELECT 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrCreateTableAsSelect CREATE TABLE AS SELECT
 * @ingroup qlr
 * @{
 */

/**
 * @brief CREATE TABLE AS SELECT  구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateCreateTableAsSelect( smlTransId      aTransID,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          stlChar       * aSQLString,
                                          qllNode       * aParseTree,
                                          qllEnv        * aEnv )
{
    qlpTableDef          * sParseTree = NULL;
    qlrInitCreateTable   * sInitPlan = NULL;
    qlvInitExprList      * sQueryExprList = NULL;
    stlInt32               sQBIndex;
    qlvStmtInfo            sStmtInfo;
    qllTarget            * sTargetInfo = NULL;
    qlrInitColumn        * sCurColumn;
    stlInt32               i;

    /**
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
        

    /**
     * DDL에 Bind Param 올 수 없음
     */
    
    STL_DASSERT( aSQLStmt->mBindCount == 0 );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpTableDef *) aParseTree;
    
    /**
     * Init Plan 생성 및
     * CREATE TABLE 공통 Validation 부분 수행  
     */
    
    STL_TRY( qlrValidateCreateTableCommon( aTransID,
                                           aDBCStmt,
                                           aSQLStmt,
                                           aSQLString,
                                           sParseTree,
                                           & sInitPlan,
                                           & sQueryExprList,
                                           aEnv )
             == STL_SUCCESS );

    /**
     * With Data Option 
     */

    sInitPlan->mWithData = sParseTree->mWithData;
    

    /**
     * statement information 설정
     */

    sQBIndex = 0;

    sStmtInfo.mTransID       = aTransID;
    sStmtInfo.mHasHintError  = STL_FALSE;
    sStmtInfo.mQBIndex       = &sQBIndex;
    stlStrcpy( sStmtInfo.mQBPrefix, QLV_QUERY_BLOCK_NAME_PREFIX_INS );
    sStmtInfo.mDBCStmt       = aDBCStmt;
    sStmtInfo.mSQLStmt       = aSQLStmt;
    sStmtInfo.mSQLString     = aSQLString;
    sStmtInfo.mQueryExprList = sQueryExprList;

    /**
     * Subquery validation  
     */

    STL_TRY( qlvQueryNodeValidation( & sStmtInfo,
                                     STL_FALSE,
                                     sInitPlan->mCommonInit.mValidationObjList,
                                     NULL,
                                     (qllNode *) sParseTree->mSubquery,
                                     & sInitPlan->mSubQueryInitNode,
                                     aEnv )
             == STL_SUCCESS );

    /**
     * Subquery 내 relation list 정보 획득 
     */
    
    STL_TRY( ellGetObjectList( sInitPlan->mCommonInit.mValidationObjList,
                               ELL_OBJECT_TABLE,
                               & sInitPlan->mUnderlyingRelationList,
                               QLL_INIT_PLAN(aDBCStmt),
                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;

    /**
     * Column Definition 및 NOT NULL constraint 추가
     * ( Column Definition을 생성하기 위해서는 TargetInfo가 필요한데,
     *   TargetInfo를 생성하는 함수(qlfMakeQueryTarget)가 
     *   SQLStmt에 InitPlan이 연결되었다고 가정하고 있기 때문에
     *   여기에서 Column Definition을 생성한다. )
     */

    /**
     * Make Target Infomation  
     */

    STL_TRY( qlfMakeQueryTarget( aSQLStmt,
                                 QLL_INIT_PLAN( aDBCStmt ),
                                 & sTargetInfo, 
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Make Column Definitions
     */
    STL_TRY( qlrMakeColumnDefinition( & sStmtInfo,
                                      & sInitPlan->mSchemaHandle,
                                      sInitPlan->mTableType,
                                      sInitPlan->mTableName,
                                      sParseTree->mColumnNameList,
                                      sTargetInfo,
                                      sInitPlan->mSubQueryInitNode,
                                      & sInitPlan->mColCount,
                                      & sInitPlan->mColumnList,
                                      & sInitPlan->mAllConstraint,
                                      aEnv )
             == STL_SUCCESS );

    if( sInitPlan->mTableType == SML_MEMORY_PERSISTENT_HEAP_COLUMNAR_TABLE )
    {
        /**
         * Columnar Table은 하나의 page에 모든 column value들이 다 저장되어야 하며
         * 최소 두개 이상의 row를 저장할 수 있는 제약사항을 가지고 있다.
         * => continuous column 및 여러 개의 page에 나뉘어 저장하는 구조로 확장 시 수정
         */
        sInitPlan->mPhysicalAttr.mColumnarAttr.mMinRowCount = SML_COLUMNAR_TABLE_MIN_ROW_COUNT_MIN;
        sInitPlan->mPhysicalAttr.mColumnarAttr.mContColumnThreshold =
            SML_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD_MAX;
        sInitPlan->mPhysicalAttr.mColumnarAttr.mColCount = sInitPlan->mColCount;

        STL_TRY( knlAllocRegionMem(
                     QLL_INIT_PLAN(aDBCStmt),
                     STL_SIZEOF(stlInt16) * sInitPlan->mColCount,
                     (void **) & (sInitPlan->mPhysicalAttr.mColumnarAttr.mColLengthArray),
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sCurColumn = sInitPlan->mColumnList;
        i = 0;

        while( sCurColumn != NULL )
        {
            sInitPlan->mPhysicalAttr.mColumnarAttr.mColLengthArray[i++] = sCurColumn->mColLength;

            sCurColumn = sCurColumn->mNext;
        }
    }

    /**
     * Validation Object 추가 
     */

    STL_TRY( ellAppendObjectList( sInitPlan->mCommonInit.mValidationObjList,
                                  sInitPlan->mAllConstraint.mConstSchemaList,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellAppendObjectList( sInitPlan->mCommonInit.mValidationObjList,
                                  sInitPlan->mAllConstraint.mConstSpaceList,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief CREATE TABLE AS SELECT  구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeCreateTableAsSelect( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv )
{
    qlrInitCreateTable    * sInitPlan          = NULL;
    qlrCodeCreateTable    * sCodePlan          = NULL;
    qllOptNodeList        * sOptNodeList       = NULL;
    qllEstimateCost       * sOptCost           = NULL;
    qloInitStmtExprList   * sStmtExprList      = NULL;
    qloInitExprList         sQueryExprList;
    stlInt32                sInternalBindIdx   = 0;
    qlncPlanCommon        * sCandQueryPlan     = NULL;
    qloExprInfo           * sTotalExprInfo     = NULL;
    qloCodeOptParamInfo     sCodeOptParamInfo;
    qllOptimizationNode   * sSubQueryOptNode   = NULL;
    knlExprStack          * sConstExprStack    = NULL;
    qlvRefExprItem        * sExprItem          = NULL;
    stlUInt64               sExprCnt           = 0;
    qlvInitExpression     * sExpr              = NULL;
    knlExprEntry          * sExprEntry         = NULL;
    qloDBType             * sDBType            = NULL;

    qloValidTblStatList     sTableStatList;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /****************************************************************
     * 정보 획득
     ****************************************************************/
    
    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_CODE_OPTIMIZE,
                                  aEnv )
             == STL_SUCCESS );
    
    sInitPlan = (qlrInitCreateTable *) aSQLStmt->mInitPlan;

    /**
     * WITH NO DATA 이면, SUBQUERY OPTIMIZE 수행할 필요 없음 
     */
    
    STL_TRY_THROW( sInitPlan->mWithData == STL_TRUE, RAMP_WITHOUT_DATA );  

    /****************************************************************
     * Code Plan 생성 및 초기화 
     ****************************************************************/

    /**
     * Code Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrCodeCreateTable),
                                (void **) & sCodePlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sCodePlan, 0x00, STL_SIZEOF(qlrCodeCreateTable) );

    /**
     * Optimization Node List 생성
     */

    STL_TRY( qlnoCreateOptNodeList( & sOptNodeList,
                                    QLL_CODE_PLAN( aDBCStmt ),
                                    aEnv )
             == STL_SUCCESS );

    sCodePlan->mOptNodeList = sOptNodeList;

    /**
     * Optimization Cost 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qllEstimateCost ),
                                (void **) & sOptCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    /****************************************************************
     * Expression List 구성
     ****************************************************************/

    /**
     * Statement 단위 Expression List 생성
     */
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qloInitStmtExprList ),
                                (void **) & sStmtExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mAllExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mColumnExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mRowIdColumnExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sStmtExprList->mConstExprStack = NULL;
    sStmtExprList->mTotalExprInfo  = NULL;

    
    /**
     * Init Statement Expression List 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitStmtExprList ),
                                (void**) &(sStmtExprList->mInitExprList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( &sStmtExprList->mInitExprList->mPseudoColExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( &sStmtExprList->mInitExprList->mConstExprList,
                                   QLL_CODE_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    
    /**
     * 임시 Query 단위 Expression List 생성
     */

    sQueryExprList.mInitExprList = NULL;
    sQueryExprList.mStmtExprList = sStmtExprList;
    

    /****************************************************************
     * Query Optimization
     ****************************************************************/

    sInternalBindIdx = 0;
    sTableStatList.mCount = 0;
    sTableStatList.mHead = NULL;
    sTableStatList.mTail = NULL;

    STL_TRY( qlncQueryOptimizer( aTransID,
                                 aDBCStmt,
                                 aSQLStmt,
                                 sInitPlan->mSubQueryInitNode,
                                 sStmtExprList->mInitExprList,
                                 &sTableStatList,   /* Valid Table Stat List */
                                 &sInternalBindIdx,
                                 &sCandQueryPlan,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Code Optimization
     */

    sCodeOptParamInfo.mDBCStmt         = aDBCStmt;
    sCodeOptParamInfo.mSQLStmt         = aSQLStmt;
    sCodeOptParamInfo.mQueryExprList   = & sQueryExprList;
    sCodeOptParamInfo.mOptNodeList     = sOptNodeList;
    sCodeOptParamInfo.mOptCost         = sOptCost;
    sCodeOptParamInfo.mOptQueryNode    = NULL;
    sCodeOptParamInfo.mCandPlan        = sCandQueryPlan;
    sCodeOptParamInfo.mOptNode         = NULL;
    sCodeOptParamInfo.mRootQuerySetIdx = 0;
    sCodeOptParamInfo.mRootQuerySet    = NULL;

    QLNO_CODE_OPTIMIZE( &sCodeOptParamInfo, aEnv );

    sSubQueryOptNode = sCodeOptParamInfo.mOptNode;


    /****************************************************************
     * Add Expression 수행
     ****************************************************************/

     /**
     * Statement 단위 Constant Expression 들을 전체 Expr List에 추가함
     */

    sExprItem = sStmtExprList->mInitExprList->mConstExprList->mHead;
    while( sExprItem != NULL )
    {
        STL_DASSERT( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT );

        sExpr = sExprItem->mExprPtr->mExpr.mConstExpr->mOrgExpr;
        STL_TRY( qloAddExpr( sExpr,
                             sStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloAddExpr( sExprItem->mExprPtr,
                             sStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );

        sExprItem = sExprItem->mNext;
    }
    
    /**
     * 각 노드의 Expr 정보들을 추가함
     */

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   sOptNodeList,
                   sSubQueryOptNode,
                   & sQueryExprList,
                   aEnv );

    /****************************************************************
     * Expression 을 위한 DB type 정보 공간 설정
     ****************************************************************/

    /**
     * DB type 정보 관리를 위한 공간 할당
     */

    STL_TRY( qloCreateExprInfo( sStmtExprList->mAllExprList->mCount,
                                & sTotalExprInfo,
                                QLL_CODE_PLAN( aDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    sStmtExprList->mTotalExprInfo = sTotalExprInfo;

    /****************************************************************
     * Constant Expression 공간 설정
     ****************************************************************/

    /**
     * Constant Expression 내부의 Expression 개수 세기
     */
    
    sExprCnt = 0;
    sExprItem = sStmtExprList->mInitExprList->mConstExprList->mHead;
    while( sExprItem != NULL )
    {
        /* constant expression 은 제외한 개수 세기 */
        sExprCnt += sExprItem->mExprPtr->mIncludeExprCnt;
        sExprItem = sExprItem->mNext;
    }
    sExprCnt++;

    /**
     * Constant Expression Stack 공간 할당
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sConstExprStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Constant Expression Stack 초기화
     */
        
    STL_TRY( knlExprInit( sConstExprStack,
                          sExprCnt,
                          QLL_CODE_PLAN( aDBCStmt ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * Statement Expression List의 Constant Expression Stack 설정
     */

    sStmtExprList->mConstExprStack = sConstExprStack;

    /****************************************************************
     * Constant Expression Stack 구성
     ****************************************************************/
    
    sExprItem = sStmtExprList->mInitExprList->mConstExprList->mHead;
    while( sExprItem != NULL )
    {
        sExpr = sExprItem->mExprPtr->mExpr.mConstExpr->mOrgExpr;

        STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                       sExpr,
                                       aSQLStmt->mBindContext,
                                       sConstExprStack,
                                       sConstExprStack,
                                       sConstExprStack->mEntryCount + 1,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       sTotalExprInfo,
                                       & sExprEntry,
                                       QLL_CODE_PLAN( aDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        sDBType = & sTotalExprInfo->mExprDBType[ sExpr->mOffset ];
        
        STL_TRY( qloSetExprDBType( sExprItem->mExprPtr,
                                   sTotalExprInfo,
                                   sDBType->mDBType,
                                   sDBType->mArgNum1,
                                   sDBType->mArgNum2,
                                   sDBType->mStringLengthUnit,
                                   sDBType->mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );

        sExprItem = sExprItem->mNext;
    }

    if( sStmtExprList->mInitExprList->mConstExprList->mCount > 1 )
    {
        sConstExprStack->mExprComposition = KNL_EXPR_COMPOSITION_COMPLEX;
    }
    else
    {
        STL_TRY( knlAnalyzeExprStack( sConstExprStack, KNL_ENV(aEnv) ) == STL_SUCCESS );
    } 

    /****************************************************************
     * Code Plan 설정
     ****************************************************************/

    sCodePlan->mOptNodeList     = sOptNodeList;
    sCodePlan->mSubQueryOptNode = sSubQueryOptNode;
    sCodePlan->mStmtExprList    = sStmtExprList;
    
    /**
     * Code Plan 연결 
     */

    aSQLStmt->mCodePlan = (void *) sCodePlan;

    /****************************************************************
     * Complete Building Optimization Node
     * => Expression List 및 Node List을 한 후,
     *    Optimization Node의 추가적인 정보 구성
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   sOptNodeList,
                   sSubQueryOptNode,
                   & sQueryExprList,
                   aEnv );
    
    STL_RAMP( RAMP_WITHOUT_DATA );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CREATE TABLE AS SELECT  구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataCreateTableAsSelect( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv )
{
    qlrInitCreateTable * sInitPlan    = NULL;
    qlrCodeCreateTable * sCodePlan    = NULL;
    qlrDataCreateTable * sDataPlan    = NULL;
    qllDataArea        * sDataArea    = NULL;
    qloDataOptExprInfo   sDataOptInfo;
    qlvInitExpression  * sExpr        = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Valid Plan 획득 
     ****************************************************************/

    STL_TRY( qlrOptDataCreateTable( aTransID,
                                    aDBCStmt,
                                    aSQLStmt,
                                    aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlrInitCreateTable *) aSQLStmt->mInitPlan;

    /**
     * WITH NO DATA 이면, Data Area 생성할 필요 없음 
     */
    
    STL_TRY_THROW( sInitPlan->mWithData == STL_TRUE, RAMP_WITHOUT_DATA );  

    sCodePlan = (qlrCodeCreateTable *) aSQLStmt->mCodePlan;
    sDataPlan = (qlrDataCreateTable *) aSQLStmt->mDataPlan;

    /****************************************************************
     * Data Area 구성
     ****************************************************************/

    /**
     * Data Area 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qllDataArea ),
                                (void **) & sDataArea,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sDataArea, 0x00, STL_SIZEOF( qllDataArea ) );

    sDataPlan->mDataArea = sDataArea;

    /**
     * Block Allocation Count 설정 => BLOCK READ COUNT로 설정
     */

    sDataArea->mBlockAllocCnt =  knlGetPropertyBigIntValueByID( KNL_PROPERTY_BLOCK_READ_COUNT,
                                                                KNL_ENV(aEnv) );

    /**
     * Execution 수행시 첫번째 Execution 인지 여부
     */

    sDataArea->mIsFirstExecution = STL_TRUE;
    
        
    /**
     * Execution Evaluate Cost 평가 여부 설정
     * @todo 향후 Session Property로 부터 정보 획득하도록 구현 필요
     */
    
    sDataArea->mIsEvaluateCost = STL_FALSE;

    /**
     * Execution Node List 할당
     */
    
    sDataArea->mExecNodeCnt = sCodePlan->mOptNodeList->mCount;

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qllExecutionNode ) * sDataArea->mExecNodeCnt,
                                (void **) & sDataArea->mExecNodeList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sDataArea->mExecNodeList,
               0x00,
               STL_SIZEOF( qllExecutionNode ) * sDataArea->mExecNodeCnt );

    /**
     * Expression Context Data 공간 할당
     */

    STL_DASSERT( sCodePlan->mStmtExprList->mTotalExprInfo != NULL );
    STL_TRY( knlCreateContextInfo( sCodePlan->mStmtExprList->mTotalExprInfo->mExprCnt,
                                   & sDataArea->mExprDataContext,
                                   & QLL_DATA_PLAN( aDBCStmt ),
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );
    /**
     * 전체 Pseudo Column 공간 확보 : Data Optimization 전에 수행 
     */

    STL_TRY( qlndAllocPseudoBlockList( aSQLStmt,
                                       sCodePlan->mStmtExprList->mInitExprList->mPseudoColExprList,
                                       sDataArea->mBlockAllocCnt,
                                       & sDataArea->mPseudoColBlock,
                                       & QLL_DATA_PLAN( aDBCStmt ),
                                       aEnv )
             == STL_SUCCESS );

    /**
     * ROWID Pseudo Column Block 할당
     */

    STL_TRY( qlndCreateColumnBlockList( & sDataArea->mRowIdColumnBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY( qlndAllocRowIdColumnBlockList( aSQLStmt,
                                            sDataArea->mRowIdColumnBlock,
                                            sCodePlan->mStmtExprList->mRowIdColumnExprList,
                                            sDataArea->mBlockAllocCnt,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
             == STL_SUCCESS );

    /**
     * Data Optimization Info 설정
     */

    sDataOptInfo.mStmtExprList      = sCodePlan->mStmtExprList;
    sDataOptInfo.mDataArea          = sDataArea;
    sDataOptInfo.mSQLStmt           = aSQLStmt;
    sDataOptInfo.mWriteRelationNode = NULL;
    sDataOptInfo.mBindContext       = aSQLStmt->mBindContext;
    sDataOptInfo.mRegionMem         = & QLL_DATA_PLAN( aDBCStmt );

    /**
     * Read Column Block 할당
     */

    STL_TRY( qlndCreateColumnBlockList( & sDataArea->mReadColumnBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY( qlndAddColumnListToColumnBlockList(
                 aSQLStmt,
                 & sDataOptInfo,
                 sDataArea->mReadColumnBlock,
                 sCodePlan->mStmtExprList->mColumnExprList,
                 sDataArea->mBlockAllocCnt,
                 aEnv )
             == STL_SUCCESS );

    /**
     * Write Column Block 할당
     */

    sDataArea->mWriteColumnBlock = NULL;
    
    /**
     * Predicate Result Column 공간 확보
     */

    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                & sDataArea->mPredResultBlock,
                                sDataArea->mBlockAllocCnt,
                                STL_TRUE,
                                STL_LAYER_SQL_PROCESSOR,
                                KNL_ANONYMOUS_TABLE_ID, /* Table ID 지정이 의미가 없음 */
                                0,
                                DTL_TYPE_BOOLEAN,
                                gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                                gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                                & QLL_DATA_PLAN( aDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    /**
     * Fetch 결과가 저장될 Target Column Block 초기화 
     */

    sDataArea->mTargetBlock = NULL;


     /**
     * Expression 을 위한 Data Context 생성 
     * - Expression Data Context 는
     *   Expression 의 Code Stack 이 참조할 수 있도록
     *   Value Block 과 Context 의 관계를 형성한다.
     */

    STL_TRY( qloDataOptimizeExpressionAll( & sDataOptInfo,
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Write Row Count 초기화
     */
    
    sDataArea->mWriteRowCnt = 0;
                                            

    /**
     * Fetch Row Count 초기화
     */

    sDataArea->mFetchRowCnt = 0;

    /****************************************************************
     * Data Optimization For Execution Nodes
     ****************************************************************/

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sCodePlan->mSubQueryOptNode,
                        sDataArea,
                        aEnv );

    /**
     * Constant Expression Result Column 공간 확보
     */
    
    if( sCodePlan->mStmtExprList->mInitExprList->mConstExprList->mCount > 0 )
    {
        sExpr = sCodePlan->mStmtExprList->mInitExprList->mConstExprList->mHead->mExprPtr;
        
        STL_TRY( knlInitShareBlockFromBlock(
                     & sDataArea->mConstResultBlock,
                     sDataArea->mExprDataContext->mContexts[ sExpr->mOffset ].mInfo.mValueInfo,
                     & QLL_DATA_PLAN( aDBCStmt ),
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sDataArea->mConstResultBlock = NULL;
    }

    /****************************************************************
     * INSERT 할 row를 위한 공간 
     ****************************************************************/
    
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlRowBlock ),
                                (void **) & sDataPlan->mRowBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( smlInitRowBlock( sDataPlan->mRowBlock,
                              sDataArea->mBlockAllocCnt,
                              & QLL_DATA_PLAN( aDBCStmt ),
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    STL_RAMP( RAMP_WITHOUT_DATA );
   
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief CREATE TABLE AS SELECT  구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrExecuteCreateTableAsSelect( smlTransId      aTransID,
                                         smlStatement  * aStmt,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv )
{
    stlTime               sTime = 0;
    stlBool               sLocked = STL_TRUE;
    
    ellDictHandle       * sAuthHandle = NULL;
    qlrInitCreateTable  * sInitPlan = NULL;

    qlrCodeCreateTable  * sCodePlan = NULL;
    qlrDataCreateTable  * sDataPlan = NULL;
    qllDataArea         * sDataArea = NULL;
    qllOptimizationNode * sSubQueryOptNode = NULL;
    stlInt32              sElementStmtAttr = 0;
    smlStatement        * sSelectStmt = NULL;
    stlInt32              sState = 0;
    qllOptNodeItem      * sOptNode = NULL;
    stlInt16              sNodeCnt = 0;
    qlnxFetchNodeInfo     sFetchNodeInfo;
    stlBool               sEOF = STL_FALSE;
    qllExecutionNode    * sSubQueryExecNode = NULL;
    stlInt64              sUsedBlockCnt = 0;
    knlValueBlockList   * sTargetColBlock = NULL;
    
        
    /***************************************************************
     * Parameter Validation
     ***************************************************************/

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * Create Table 
     ***************************************************************/

    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( aStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Run-Time Validation
     */
    
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

    sInitPlan = (qlrInitCreateTable *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * CREATE TABLE 구문을 위한 DDL Lock 획득
     */

    STL_TRY( ellLock4CreateTable( aTransID,
                                  aStmt,
                                  sAuthHandle,
                                  & sInitPlan->mSchemaHandle,
                                  & sInitPlan->mSpaceHandle,
                                  sInitPlan->mAllConstraint.mConstSchemaList,
                                  sInitPlan->mAllConstraint.mConstSpaceList,
                                  & sLocked,
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );

    /**
     * SELECT TABLE을 위한 IS Lock 획득
     */

    STL_DASSERT( sInitPlan->mUnderlyingRelationList != NULL );
    
    STL_TRY( ellLockUnderlyingTableList4ViewDDL( aTransID,
                                                 aStmt,
                                                 SML_LOCK_IS,
                                                 sInitPlan->mUnderlyingRelationList,
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

    STL_TRY( qlrExecuteCreateTableCommon( aTransID,
                                          aStmt,
                                          aDBCStmt,
                                          aSQLStmt,
                                          aEnv )
             == STL_SUCCESS );
    
    sInitPlan   = (qlrInitCreateTable *) aSQLStmt->mInitPlan;

    /**
     * WITH NO DATA 이면, INSERT SELECT 수행할 필요 없음 
     */
    
    STL_TRY_THROW( sInitPlan->mWithData == STL_TRUE, RAMP_WITHOUT_DATA );  

    /***************************************************************
     * INSERT SELECT 
     ***************************************************************/
    
    sCodePlan   = (qlrCodeCreateTable *) aSQLStmt->mCodePlan;
    sDataPlan   = (qlrDataCreateTable *) aSQLStmt->mDataPlan;
    
    sDataArea        = sDataPlan->mDataArea;
    sSubQueryOptNode = sCodePlan->mSubQueryOptNode;

    /**
     * transaction id 설정
     */
    
    sDataArea->mTransID = aTransID;

    /**
     * INSERT SELECT를 위한 statement 생성
     * CREATE TABLE 처리 시에는 갱신 정보를 봐야 하고,
     * SELECT 처리 시에는 읽은 시점 데이터 (Snapshot)을 봐야 하기 때문에
     * 다른 statement를 쓴다.
     */
    
    sElementStmtAttr = SML_STMT_ATTR_READONLY;

    STL_TRY( smlAllocStatement( aTransID,
                                NULL, /* aUpdateRelHandle */
                                sElementStmtAttr,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_TRUE,  /* aNeedSnapshotIterator */
                                &sSelectStmt,
                                SML_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    /**
     * Initialize Subquery Execution Node
     */

    if ( sDataArea->mIsFirstExecution == STL_TRUE )
    {
        sOptNode = sCodePlan->mOptNodeList->mTail;

        sState = 2;
        for( sNodeCnt = 0 ; 
             sNodeCnt < sDataArea->mExecNodeCnt ;
             sNodeCnt++ )
        {
            QLNX_INITIALIZE_NODE( aTransID,
                                  sSelectStmt,
                                  aDBCStmt,
                                  aSQLStmt,
                                  sOptNode->mNode,
                                  sDataArea,
                                  aEnv );
            sOptNode = sOptNode->mPrev;
        }

        /* 초기화 Flag 설정 */
        sDataArea->mIsFirstExecution = STL_FALSE;
    }
    else
    {
        /* Nothing to do */
    }

    /**
     * CONSTANT EXPRESSION STACK 평가 
     */

    STL_DASSERT( sDataArea->mTransID == aTransID );
    
    STL_TRY( qlxEvaluateConstExpr( sCodePlan->mStmtExprList->mConstExprStack,
                                   sDataArea,
                                   KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * Execute Select
     */

    QLNX_EXECUTE_NODE( aTransID,
                       sSelectStmt,
                       aDBCStmt,
                       aSQLStmt,
                       sDataArea->mExecNodeList[ sCodePlan->mSubQueryOptNode->mIdx ].mOptNode,
                       sDataArea,
                       aEnv );

    /**
     * Insert Select 수행을 위한 정보 획득 
     */

    sDataArea->mWriteRowCnt = 0;

    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               sSelectStmt,
                               aDBCStmt,
                               aSQLStmt,
                               sDataArea );

    sSubQueryExecNode = & sDataArea->mExecNodeList[ sCodePlan->mSubQueryOptNode->mIdx ];
    if( sSubQueryExecNode->mNodeType == QLL_PLAN_NODE_QUERY_SPEC_TYPE )
    {
        sTargetColBlock = ((qlnxQuerySpec *)sSubQueryExecNode->mExecNode)->mTargetColBlock;
    }
    else
    {
        STL_DASSERT( sSubQueryExecNode->mNodeType == QLL_PLAN_NODE_QUERY_SET_TYPE );
        sTargetColBlock = ((qlnxQuerySet *)sSubQueryExecNode->mExecNode)->mTargetColBlock;
    }

    /**
     * Insert Select 수행
     */

    sEOF = STL_FALSE;
    while( sEOF == STL_FALSE )
    {
        QLNX_FETCH_NODE( & sFetchNodeInfo,
                         sSubQueryOptNode->mIdx,
                         0, /* Start Idx */
                         0,
                         QLL_FETCH_COUNT_MAX,
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        if( sUsedBlockCnt == 0 )
        {
            STL_THROW( RAMP_INSERT_FINISH );
        }
        else
        {
            /* Do Nothing */
        }

        /**
         * Row 추가
         */

        STL_TRY( smlInsertRecord( aStmt,
                                  sDataPlan->mTablePhyHandle,
                                  sTargetColBlock,
                                  NULL, /* unique violation */
                                  sDataPlan->mRowBlock,
                                  SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sDataArea->mWriteRowCnt += sUsedBlockCnt;
    }

    STL_RAMP( RAMP_INSERT_FINISH );
    
    
    /**
     * Finalize Execution Node
     */

    for( sNodeCnt = 0 ; 
         sNodeCnt < sDataArea->mExecNodeCnt ;
         sNodeCnt++ )
    {
        QLNX_FINALIZE_NODE( sDataArea->mExecNodeList[ sNodeCnt ].mOptNode,
                            sDataArea,
                            aEnv );
    }
    sState = 1;
    sDataArea->mIsFirstExecution = STL_TRUE;

    sState = 0;
    STL_TRY( smlFreeStatement( sSelectStmt, SML_ENV( aEnv ) ) == STL_SUCCESS );
    
    STL_RAMP( RAMP_WITHOUT_DATA );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 2 :
            for( sNodeCnt = 0 ; 
                 sNodeCnt < sDataArea->mExecNodeCnt ;
                 sNodeCnt++ )
            {
                QLNX_FINALIZE_NODE_IGNORE_FAILURE( sDataArea->mExecNodeList[ sNodeCnt ].mOptNode,
                                                   sDataArea,
                                                   aEnv );
            }
        case 1 :
            (void)smlFreeStatement( sSelectStmt,
                                    SML_ENV( aEnv ) );
        default :
            break;
    }

        
    return STL_FAILURE;
}

/**
 * @brief Init Plan에 컬럼 정의를 생성한다. 
 * @param[in] aStmtInfo         Stmt Information
 * @param[in] aSchemaHandle     Schema Handle
 * @param[in] aRelationType     Table Type
 * @param[in] aTableName        Table Name
 * @param[in] aColumnNameList   CREATE TABLE tblName ColumnNameList
 * @param[in] aTargetInfo       AS SELECT의 TargetInfo 
 * @param[in] aSubquery         AS SELECT
 * @param[out] aColumnCount     생성한 Column definition count
 * @param[out] aColumnList      생성한 Column definition list 
 * @param[out] aAllConstraint   생성한 NOT NULL Constraints
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus qlrMakeColumnDefinition( qlvStmtInfo             * aStmtInfo,
                                   ellDictHandle           * aSchemaHandle,
                                   smlRelationType           aRelationType,
                                   stlChar                 * aTableName,
                                   qlpList                 * aColumnNameList,
                                   qllTarget               * aTargetInfo,
                                   qlvInitNode             * aSubquery,
                                   stlInt32                * aColumnCount,
                                   qlrInitColumn          ** aColumnList,
                                   qlrInitTableConstraint  * aAllConstraint,
                                   qllEnv                  * aEnv )
{
    stlInt32               sTargetCount = 0;
    qlvInitTarget        * sTargetList = NULL;
    qlvInitTarget        * sTargetCol = NULL;
    qllTarget            * sCurTargetInfo  = NULL;
    qlrInitColumn        * sNewColumn = NULL;
    qlrInitColumn        * sNewColumnList = NULL;
    qlrInitColumn        * sLastNewColumn = NULL;
    qlpValue             * sDefaultNullNode = NULL;
    stlChar             ** sColumnName;
    stlBool                sDummy = STL_FALSE;
    stlBool                sExistNotNullConst = STL_FALSE;
    stlInt32               sTargetColIdx = 0;
    dtlStringLengthUnit    sCharUnit = DTL_STRING_LENGTH_UNIT_NA;
    stlInt64               sColLen = 0;

    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubquery != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAllConstraint != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**********************************************************
     * Column Definition List를 만들기 위한 초기화 작업 
     **********************************************************/

    switch( aSubquery->mType )
    {
        case QLV_NODE_TYPE_QUERY_SET :
            {
                sTargetCount = ((qlvInitQuerySetNode*) aSubquery)->mSetTargetCount;
                sTargetList  = ((qlvInitQuerySetNode*) aSubquery)->mSetTargets;
                break;
            }
        case QLV_NODE_TYPE_QUERY_SPEC :
            {
                sTargetCount = ((qlvInitQuerySpecNode*) aSubquery)->mTargetCount;
                sTargetList  = ((qlvInitQuerySpecNode*) aSubquery)->mTargets;
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    /**
     * Column 갯수 설정 
     */

    *aColumnCount = sTargetCount;

    /**
     * Column Name List 구축 
     */
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                STL_SIZEOF(stlChar *) * sTargetCount,
                                (void **) & sColumnName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sColumnName, 0x00, STL_SIZEOF(stlChar *) * sTargetCount );

    STL_TRY( qlrValidateUserColList( aStmtInfo->mDBCStmt,
                                     aStmtInfo->mSQLString,
                                     NULL,
                                     aColumnNameList,
                                     sTargetCount,
                                     sTargetList,
                                     sColumnName,
                                     STL_FALSE, /* IsForce, For CREATE VIEW AS SELECT */
                                     & sDummy,  /* Not Use, For CREATE VIEW AS SELECT */
                                     STL_FALSE, /* Check Sequence */
                                     aEnv )
             == STL_SUCCESS );
    

    /**********************************************************
     * Column Definition List 생성 
     **********************************************************/

    sCurTargetInfo = aTargetInfo;
    for ( sTargetColIdx = 0; sTargetColIdx < sTargetCount; sTargetColIdx++ )
    {
        sTargetCol = & sTargetList[sTargetColIdx];
            
        /**
         * New Column의 Init Plan 생성
         */
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                    STL_SIZEOF(qlrInitColumn),
                                    (void **) & sNewColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sNewColumn, 0x00, STL_SIZEOF(qlrInitColumn) );

        qlrSetColumnInfoFromTarget( sNewColumn,
                                    sTargetColIdx,
                                    sColumnName[sTargetColIdx],
                                    sCurTargetInfo );
        
        /**
         * NOT NULL 설정 및 NOT NULL constraint 생성 
         */

        sNewColumn->mIsNullable = STL_TRUE;
        if ( sCurTargetInfo->mIsNullable == STL_FALSE )
        {
            if ( sCurTargetInfo->mBaseColumnName != NULL )
            {
                STL_TRY( qlrExistNotNullConst( aStmtInfo,
                                               sTargetCol->mExpr,
                                               sCurTargetInfo,
                                               & sExistNotNullConst,
                                               aEnv )
                     == STL_SUCCESS );
                
                if ( sExistNotNullConst == STL_TRUE )
                {
                    /* 명시적인 NOT NULL constraint가 존재하는 경우,
                     * 새로운 Column에도 NOT NULL constraint를 생성함 */
                    sNewColumn->mIsNullable = STL_FALSE;
                    
                    STL_TRY( qlrMakeNotNullConstraint( aStmtInfo,
                                                       aSchemaHandle,
                                                       aTableName,
                                                       sColumnName[sTargetColIdx],
                                                       sTargetCol,
                                                       sTargetColIdx,
                                                       aAllConstraint,
                                                       aEnv )
                     == STL_SUCCESS );
                }
                else
                {
                    /* Primary Key, Identity Column등에 의해
                     * 생성된 NOT NULL속성은 복사하지 않음 */
                }
            }
            else
            {
                /* Target이 Base Column이 아님  */
            }
        }
        else
        {
            /* Target이 Nullable */ 
        }
        
        /**
         * Default 정보 
         */
        sNewColumn->mColumnDefault = NULL;
        sNewColumn->mNullDefault = STL_FALSE;

        /**
         * DEFAULT NULL Expression 구성
         */
        
        STL_TRY( qlrMakeDefaultNullNode( aStmtInfo->mDBCStmt, 
                                         & sDefaultNullNode,
                                         aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( qlvValidateNullBNF( aStmtInfo,
                                     sDefaultNullNode,
                                     & sNewColumn->mInitDefault,
                                     KNL_BUILTIN_FUNC_INVALID,
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlvRewriteExpr( sNewColumn->mInitDefault,
                                 DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                 aStmtInfo->mQueryExprList,
                                 & sNewColumn->mInitDefault,
                                 QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                 aEnv )
                 == STL_SUCCESS );
        
        /**
         * 그 외 
         */
        sNewColumn->mPhysicalOrdinalPosition = sTargetColIdx;
        sNewColumn->mLogicalOrdinalPosition  = sTargetColIdx;
        sNewColumn->mIsIdentity = STL_FALSE;
        sNewColumn->mIdentityOption = ELL_IDENTITY_GENERATION_NA;
        stlMemset( & sNewColumn->mIdentityAttr, 0x00, STL_SIZEOF(smlSequenceAttr) );

        if( aRelationType == SML_MEMORY_PERSISTENT_HEAP_COLUMNAR_TABLE )
        {
            STL_TRY_THROW( (sNewColumn->mTypeDef.mDBType != DTL_TYPE_LONGVARCHAR) &&
                           (sNewColumn->mTypeDef.mDBType != DTL_TYPE_LONGVARBINARY),
                           RAMP_ERR_COLUMNAR_TABLE_NOT_SUPPORT_COLUMN_TYPE );
            
            sCharUnit = sNewColumn->mTypeDef.mStringLengthUnit;
            
            if ( sCharUnit == DTL_STRING_LENGTH_UNIT_NA )
            {
                sCharUnit = ellGetDbCharLengthUnit();
            }
            else
            {
                /* nothing to do */
            }
            
            STL_TRY( dtlGetDataValueBufferSize( sNewColumn->mTypeDef.mDBType,
                                                sNewColumn->mTypeDef.mArgNum1,
                                                sCharUnit,
                                                & sColLen,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY_THROW( sColLen <= SML_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD_MAX,
                           RAMP_ERR_COLUMNAR_TABLE_INVALID_COLUMN_LENGTH );
            
            sNewColumn->mColLength = sColLen;
        }

        /**
         * NewColumn을 list에 연결 
         */
        if ( sNewColumnList == NULL )
        {
            sNewColumnList = sNewColumn;
            sLastNewColumn = sNewColumn;
        }
        else
        {
            sLastNewColumn->mNext = sNewColumn;
            sLastNewColumn        = sLastNewColumn->mNext;
        }
        
        sCurTargetInfo = sCurTargetInfo->mNext;
    }

    *aColumnList = sNewColumnList;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMNAR_TABLE_NOT_SUPPORT_COLUMN_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMNAR_TABLE_NOT_SUPPORT_COLUMN_TYPE,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sTargetCol->mDisplayPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_COLUMNAR_TABLE_INVALID_COLUMN_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMNAR_TABLE_INVALID_COLUMN_LENGTH,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sTargetCol->mDisplayPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      SML_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD_MAX );
    }
    
    STL_FINISH;
        
    return STL_FAILURE;
}

/**
 * @brief 해당 칼럼에 NOT NULL Constraint가 존재하는지 검사 
 * @param[in]  aStmtInfo          Stmt Information
 * @param[in]  aTargetExpr        Target Expression
 * @param[in]  aTargetInfo        Target Info
 * @param[out] aExistNotNullConst Exist NotNullConst
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus qlrExistNotNullConst( qlvStmtInfo          * aStmtInfo,
                                qlvInitExpression    * aTargetExpr,
                                qllTarget            * aTargetInfo,
                                stlBool              * aExistNotNullConst,
                                qllEnv               * aEnv )
{
    stlInt64               sTableID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle          sTableHandle;
    ellDictHandle        * sColumnHandle;
    stlBool                sTableExist = STL_FALSE;
    stlInt32               sNotNullCnt = 0;
    ellDictHandle        * sNotNullHandleArray = NULL;
    stlInt32               sNotNullIdx = 0;
    ellDictHandle        * sNotNullColumnHandle = NULL;
    stlBool                sExistNotNullConst = STL_FALSE;

    ellInitDictHandle( & sTableHandle );

    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetInfo != NULL, QLL_ERROR_STACK(aEnv) );

    /**********************************************************
     * NOT NULL constraint 획득  
     **********************************************************/

    sColumnHandle = aTargetInfo->mBaseColumnHandle;

    if ( sColumnHandle != NULL )
    {
        /* Get Table Handle */
        sTableID = ellGetColumnTableID( sColumnHandle );

        STL_TRY( ellGetTableDictHandleByID( aStmtInfo->mTransID,
                                            aStmtInfo->mSQLStmt->mViewSCN,
                                            sTableID,
                                            & sTableHandle,
                                            & sTableExist,
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        STL_DASSERT( sTableExist == STL_TRUE );

        STL_TRY( ellGetTableCheckNotNullDictHandle(
                 aStmtInfo->mTransID,
                 aStmtInfo->mSQLStmt->mViewSCN,
                 QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                 & sTableHandle,
                 & sNotNullCnt,
                 & sNotNullHandleArray,
                 ELL_ENV(aEnv) )
             == STL_SUCCESS );
    }
    else
    {
        /* Column Handle이 없는 경우, */
    }

    /**********************************************************
     * Target Column에 NOT NULL Constraint가 존재하는 경우
     * NOT NULL 정보를 설정하고, NOT NULL constraint를 생성한다. 
     **********************************************************/
    
    for( sNotNullIdx = 0 ; sNotNullIdx < sNotNullCnt ; sNotNullIdx++ )
    {
        sNotNullColumnHandle =
            ellGetCheckNotNullColumnHandle( &sNotNullHandleArray[sNotNullIdx] );
        
        if( ellGetColumnID( sNotNullColumnHandle ) ==
            ellGetColumnID( sColumnHandle ) )
        {
            /**
             * Target Column이 NOT NULL column 
             * Constraint 상태가 Not Deferrable, Validate 일 경우에만
             * Not Null 정보를 설정한다. 
             */

            if ( (ellGetConstraintAttrIsDeferrable( & sNotNullHandleArray[sNotNullIdx] ) == STL_FALSE) &&
                 (ellGetConstraintAttrIsEnforced( & sNotNullHandleArray[sNotNullIdx] ) == STL_TRUE) &&
                 (ellGetConstraintAttrIsValidate( & sNotNullHandleArray[sNotNullIdx] ) == STL_TRUE) )
            {
                sExistNotNullConst = STL_TRUE;
            }
            else
            {
                sExistNotNullConst = STL_FALSE;
            }
            
            break;
        }
        else
        {
            /* Do Nothing */
        }
    }

    *aExistNotNullConst = sExistNotNullConst;
    
    return STL_SUCCESS;
    
    STL_FINISH;
        
    return STL_FAILURE;
}

/**
 * @brief Target Column의 NOT NULL 정보를 얻어오고, NOT NULL constraint를 생성 
 * @param[in]  aStmtInfo      Stmt Information
 * @param[in]  aSchemaHandle  Schema Handle
 * @param[in]  aTableName     Table Name
 * @param[in]  aColumnName    Column Name
 * @param[in]  aTarget        Target
 * @param[in]  aColumnIdx     Column Index
 * @param[out] aAllConstraint All Constraint
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus qlrMakeNotNullConstraint( qlvStmtInfo            * aStmtInfo,
                                    ellDictHandle          * aSchemaHandle,
                                    stlChar                * aTableName,
                                    stlChar                * aColumnName,
                                    qlvInitTarget          * aTarget,
                                    stlInt32                 aColumnIdx,
                                    qlrInitTableConstraint * aAllConstraint,
                                    qllEnv                 * aEnv )
{
    ellDictHandle  sConstHandle;
    qlrInitConst * sNewConst = NULL;
    stlChar        sNameBuffer[STL_MAX_SQL_IDENTIFIER_LENGTH + 1] = {0,};
    stlBool        sObjectExist = STL_FALSE;
    qlrInitConst * sCurConst = NULL;
    stlBool        sDuplicate = STL_FALSE;
    stlInt32       i = 0;
    
    ellInitDictHandle( & sConstHandle );

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTarget != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAllConstraint != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Init Constraint 영역을 확보
     */
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                STL_SIZEOF(qlrInitConst),
                                (void **) & sNewConst,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sNewConst, 0x00, STL_SIZEOF(qlrInitConst) );
    
    sNewConst->mConstType = ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL;
    
    /**
     * Constraint Characteristics 설정
     * ( Not Deferrable & Validate )일 경우에만 NOT NULL constraint를 가져온다.
     */

    sNewConst->mDeferrable   = ELL_TABLE_CONSTRAINT_DEFERRABLE_DEFAULT;
    sNewConst->mInitDeferred = ELL_TABLE_CONSTRAINT_INITIALLY_DEFERRED_DEFAULT;
    sNewConst->mEnforced     = ELL_TABLE_CONSTRAINT_ENFORCED_DEFAULT;
    
    /**
     * mColumnCnt
     * mColumnIdx
     */
    
    sNewConst->mColumnCnt = 1;
    sNewConst->mColumnIdx[0] = aColumnIdx;

    /**
     * Constraint의 Schema Handle 설정
     * - Table 의 Schema 와 동일한 Schema 를 갖는다.
     */
    
    stlMemcpy( & sNewConst->mConstSchemaHandle,
               aSchemaHandle,
               STL_SIZEOF( ellDictHandle ) );

    /**
     * Constraint Name 을 자동으로 생성
     */
    
    ellMakeCheckNotNullName( sNameBuffer,
                             STL_MAX_SQL_IDENTIFIER_LENGTH + 1,
                             aTableName,
                             aColumnName );
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                stlStrlen( sNameBuffer ) + 1,
                                (void **) & sNewConst->mConstName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( sNewConst->mConstName, sNameBuffer );
    
    /**
     * Constraint Schema 에 대해 ADD CONSTRAINT ON SCHEMA 권한 검사 
     */
    
    STL_TRY( qlaCheckSchemaPriv( aStmtInfo->mTransID,
                                 aStmtInfo->mDBCStmt,
                                 aStmtInfo->mSQLStmt,
                                 ELL_SCHEMA_PRIV_ACTION_ADD_CONSTRAINT,
                                 & sNewConst->mConstSchemaHandle,
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Table 정의에 동일한 Constraint Name 이 존재하는 지 검사
     */
    
    STL_TRY( qlrCheckSameConstNameInTableDef( aAllConstraint,
                                              & sNewConst->mConstSchemaHandle,
                                              sNewConst->mConstName,
                                              & sObjectExist,
                                              aEnv )
             == STL_SUCCESS );
    
    if ( sObjectExist == STL_TRUE )
    {
        STL_THROW( RAMP_ERR_AUTO_NAME_EXISTS );
    }
    else
    {
        /* 존재하지 않는 Constraint Name 임. */
    }
    
    /**
     * Schema 에 동일한 Constraint Name 이 존재하는 지 검사
     */
    
    STL_TRY( ellGetConstraintDictHandleWithSchema( aStmtInfo->mTransID,
                                                   aStmtInfo->mSQLStmt->mViewSCN,
                                                   & sNewConst->mConstSchemaHandle,
                                                   sNewConst->mConstName,
                                                   & sConstHandle,
                                                   & sObjectExist,
                                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    if ( sObjectExist == STL_TRUE )
    {
        STL_THROW( RAMP_ERR_AUTO_NAME_EXISTS );
    }
    else
    {
        /* 존재하지 않는 Constraint Name 임. */
    }
    
    /**
     * mIndexSpaceHandle
     * mIndexName
     * mIndexAttr
     * mIndexKeyFlag
     */
    
    ellInitDictHandle( & sNewConst->mIndexSpaceHandle );
    sNewConst->mIndexName = NULL;
    stlMemset( & sNewConst->mIndexAttr, 0x00, STL_SIZEOF(smlIndexAttr) );
    stlMemset( sNewConst->mIndexKeyFlag,
               0x00,
               STL_SIZEOF(stlUInt8) * DTL_INDEX_COLUMN_MAX_COUNT );
    
    /**
     * mCheckClause
     */
    
    sNewConst->mCheckClause = NULL;

    
    /**
     * mRefColumnHandle
     */
    
    for ( i = 0; i < DTL_INDEX_COLUMN_MAX_COUNT; i++ )
    {
        ellInitDictHandle( & sNewConst->mRefColumnHandle[i] );
    }
    
    /**
     * mMatchOption
     * mUpdateRule
     * mDeleteRule
     */
    
    sNewConst->mMatchOption = ELL_REFERENTIAL_MATCH_OPTION_NA;
    sNewConst->mUpdateRule  = ELL_REFERENTIAL_ACTION_RULE_NA;
    sNewConst->mDeleteRule  = ELL_REFERENTIAL_ACTION_RULE_NA;

    /**
     * Check Not Null Constraint 에 연결
     */

    STL_TRY( ellAddNewObjectItem( aAllConstraint->mConstSchemaList,
                                  & sNewConst->mConstSchemaHandle,
                                  & sDuplicate,
                                  QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( aAllConstraint->mCheckNotNull == NULL )
    {
        aAllConstraint->mCheckNotNull = sNewConst;
    }
    else
    {
        for ( sCurConst = aAllConstraint->mCheckNotNull;
              sCurConst->mNext != NULL;
              sCurConst = sCurConst->mNext )
        {
        }
        sCurConst->mNext = sNewConst;
    }

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_AUTO_NAME_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_AUTO_CONSTRAINT_NAME_EXISTS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aTarget->mDisplayPos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;
        
    return STL_FAILURE;
}

    
/** @} qlrCreateTableAsSelct */
