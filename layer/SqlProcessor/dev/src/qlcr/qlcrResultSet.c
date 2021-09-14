/*******************************************************************************
 * qlcrResultSet.c
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
 * @file qlcrResultSet.c
 * @brief Result Set 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlcrResultSet
 * @{
 */



/********************************************************************
 * 초기화 함수 
 ********************************************************************/



/**
 * @brief DML RETURNING Query 의 Result Set 의 Cursor Property 를 설정한다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aEnv                 Environment
 * @remarks
 */
stlStatus qlcrValidateDMLQueryResultCursorProperty( qlvStmtInfo        * aStmtInfo,
                                                    qllEnv             * aEnv )
{
    qlvInitPlan * sInitPlan;
    
    qllStmtCursorProperty  * sStmtProperty = NULL;
    ellCursorProperty      * sResultSetProperty = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sInitPlan = (qlvInitPlan*) qllGetInitPlan( aStmtInfo->mSQLStmt );
    
    sStmtProperty      = & aStmtInfo->mSQLStmt->mStmtCursorProperty;
    sResultSetProperty = & sInitPlan->mResultCursorProperty;
    
    /*****************************************************
     * Result Set 의 Standard Type 설정 
     *****************************************************/

    sResultSetProperty->mStandardType = sStmtProperty->mProperty.mStandardType;

    /*****************************************************
     * Result Set 의 Sensitivity 설정 
     *****************************************************/

    /**
     * DML Query 는 INSENSITIVE 만 가능함.
     */
    
    switch (sStmtProperty->mProperty.mSensitivity)
    {
        case ELL_CURSOR_SENSITIVITY_INSENSITIVE:
            sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_INSENSITIVE;
            break;
        case ELL_CURSOR_SENSITIVITY_SENSITIVE:
            STL_THROW( RAMP_ERR_NOT_SENSITIVE_QUERY );
            break;
        case ELL_CURSOR_SENSITIVITY_ASENSITIVE:
            sResultSetProperty->mSensitivity = ELL_CURSOR_SENSITIVITY_INSENSITIVE;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**
     * Dynamic Cursor 는 Sensitive 해야 함.
     */
    
    if ( sStmtProperty->mProperty.mStandardType == ELL_CURSOR_STANDARD_ODBC_DYNAMIC )
    {
        STL_TRY_THROW( sResultSetProperty->mSensitivity == ELL_CURSOR_SENSITIVITY_SENSITIVE,
                       RAMP_ERR_INVALID_DYNAMIC_CURSOR_SENSITIVE );
    }
    
    /*****************************************************
     * Result Set 의 Scrollability 설정 
     *****************************************************/
    
    sResultSetProperty->mScrollability = sStmtProperty->mProperty.mScrollability;

    /*****************************************************
     * Result Set 의 Updatability 설정
     *****************************************************/

    /**
     * Result Set 의 Updatability 설정
     * SELECT 구문과 달리 Driver 의 UPDATABLE 모드에 관계없이 무조건 READ_ONLY 로 결정됨.
     * 
     * --------------------------------------------------------------------
     *  DML 구문              |   Driver READ_ONLY   |  Driver LOCK (UPDATABLE)
     * --------------------------------------------------------------------
     * non-updatable query      |   READ ONLY          | READ ONLY
     * --------------------------------------------------------------------
     */

    sResultSetProperty->mUpdatability = ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY;

    /*****************************************************
     * Result Set 의 Holdability 설정
     *****************************************************/

    /**
     * DML Query 는 WITHOUT HOLD 만 가능함.
     */
    
    switch ( sStmtProperty->mProperty.mHoldability )
    {
        case ELL_CURSOR_HOLDABILITY_UNSPECIFIED:
            {
                sResultSetProperty->mHoldability
                    = ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD;
                break;
            }
        case ELL_CURSOR_HOLDABILITY_WITH_HOLD:
            {
                if ( sStmtProperty->mIsDbcCursor == STL_TRUE )
                {
                    sResultSetProperty->mHoldability = ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD;
                }
                else
                {
                    STL_THROW( RAMP_ERR_INVALID_HOLDABILITY );
                }
                break;
            }
        case ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD:
            {
                sResultSetProperty->mHoldability = sStmtProperty->mProperty.mHoldability;
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }
    
    /*****************************************************
     * @todo Result Set 의 Returnability 설정
     *****************************************************/

    /**
     * @todo Returnability 설정
     */
    
    sResultSetProperty->mReturnability = sStmtProperty->mProperty.mReturnability;

    /*****************************************************
     * 미구현 사항 
     *****************************************************/

    switch ( sResultSetProperty->mStandardType )
    {
        case ELL_CURSOR_STANDARD_ISO:
        case ELL_CURSOR_STANDARD_ODBC_STATIC:
        case ELL_CURSOR_STANDARD_ODBC_KEYSET:
            break;
        case ELL_CURSOR_STANDARD_ODBC_DYNAMIC:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    switch ( sResultSetProperty->mReturnability )
    {
        case ELL_CURSOR_RETURNABILITY_WITH_RETURN:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;
        case ELL_CURSOR_RETURNABILITY_WITHOUT_RETURN:
            break;
        default:
            STL_ASSERT(0);
            break;
    }
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SENSITIVE_QUERY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_QUERY_IS_NOT_UPDATABLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_DYNAMIC_CURSOR_SENSITIVE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_CURSOR_PROPERTY,
                      "dynamic cursor should be sensitive",
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_HOLDABILITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_CURSOR_PROPERTY,
                      "holdable cursor should be read only cursor",
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlcrValidateDMLQueryResultCursorProperty()" );
    }
    
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL Statement 의 Result Set Descriptor 를 초기화한다.
 * @param[in]     aQueryStmt       Fetch 시 사용할 SQL Statement (nullable)
 *                            <BR> if NULL: INSERT SELECT, CREATE AS SELECT
 * @param[in]     aTargetBlock     Target Block List
 * @param[in]     aRegionMem       Region Memory
 * @param[in]     aEnv             Environment
 * @remarks
 * 명시적 Cursor 를 사용하기 위해서는 aInstDesc 가 null 이 아니어야 함.
 */
stlStatus qlcrDataOptInitResultSetDesc( qllStatement           * aQueryStmt,
                                        knlValueBlockList      * aTargetBlock,
                                        knlRegionMem           * aRegionMem,
                                        qllEnv                 * aEnv )
{
    qllResultSetDesc * sResultSetDesc = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aQueryStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetBlock != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Result Set Descriptor 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qllResultSetDesc ),
                                (void **) & sResultSetDesc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /*****************************************************
     * Result Set Property 정보 설정
     *****************************************************/
    
    stlMemcpy( & sResultSetDesc->mCursorProperty,
               & ((qlvInitPlan*) qllGetInitPlan( aQueryStmt ))->mResultCursorProperty,
               STL_SIZEOF(ellCursorProperty) );
    
    /*****************************************************
     * Result Set 상태 정보 
     *****************************************************/

    sResultSetDesc->mIsOpen        = STL_FALSE;
    sResultSetDesc->mStmtTimestamp = 0;
    
    
    /*****************************************************
     * Standing Cursor Information 설정
     *****************************************************/

    sResultSetDesc->mQueryStmt = aQueryStmt;
    sResultSetDesc->mInstDesc  = NULL;

    /*****************************************************
     * Result Set 의 Target 정보 설정 
     *****************************************************/

    sResultSetDesc->mTargetBlock = aTargetBlock;

    STL_TRY( qllGetQueryTargetContext( aQueryStmt,
                                       aRegionMem,
                                       & sResultSetDesc->mTargetExprList,
                                       & sResultSetDesc->mTargetCodeStack,
                                       & sResultSetDesc->mTargetDataContext,
                                       aEnv )
             == STL_SUCCESS );

    /*****************************************************
     * Execute-Time Information
     *****************************************************/

    sResultSetDesc->mInstantTable = NULL;
    
    /*****************************************************
     * Fetch-Time Information
     *****************************************************/

    sResultSetDesc->mKnownLastPosition = STL_FALSE;
    sResultSetDesc->mLastPositionIdx = 0;
        
    sResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_BEFORE_THE_FIRST_ROW;
    sResultSetDesc->mPositionRowIdx = 0;
    sResultSetDesc->mBlockRowIdx    = -1;

    sResultSetDesc->mBlockStartIdx = -1;
    sResultSetDesc->mBlockEOFIdx   = 0;

    sResultSetDesc->mBlockStartPositionIdx = 0;
    sResultSetDesc->mBlockEOFPositionIdx   = 1;

    /**
     * Output 설정
     */

    aQueryStmt->mResultSetDesc = sResultSetDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Result Set Cursor 를 닫는다.
 * @param[in] aQueryStmt     Query SQL Statement
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus qlcrCloseResultSet( qllStatement    * aQueryStmt,
                              qllEnv          * aEnv )
{
    stlInt32 sState = 0;
    
    qllOptimizationNode  * sOptNode  = NULL;
    qllDataArea          * sDataArea = NULL;
    qlnInstantTable      * sInstantTable = NULL;

    qllResultSetDesc   * sResultSetDesc = NULL;
    
    ellCursorInstDesc  * sInstDesc = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aQueryStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryStmt->mResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sResultSetDesc = aQueryStmt->mResultSetDesc;
    STL_TRY_THROW( sResultSetDesc->mIsOpen == STL_TRUE, RAMP_SUCCESS );
    
    /*****************************************************
     * Result Set 상태 정보 
     *****************************************************/

    sResultSetDesc->mIsOpen        = STL_FALSE;
    sResultSetDesc->mStmtTimestamp = 0;
    
    
    /*****************************************************
     * Fetch-Time 멤버 변수를 초기화한다.
     *****************************************************/

    sResultSetDesc->mKnownLastPosition = STL_FALSE;
    sResultSetDesc->mLastPositionIdx = 0;
    
    sResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_BEFORE_THE_FIRST_ROW;
    sResultSetDesc->mPositionRowIdx = 0;
    sResultSetDesc->mBlockRowIdx = -1;

    sResultSetDesc->mBlockStartIdx = -1;
    sResultSetDesc->mBlockEOFIdx   = 0;

    sResultSetDesc->mBlockStartPositionIdx = 0;
    sResultSetDesc->mBlockEOFPositionIdx   = 1;


    /*****************************************************
     * Finalize Nodes
     *****************************************************/
    
    sOptNode  = (qllOptimizationNode *) aQueryStmt->mCodePlan;
    sDataArea = (qllDataArea *) aQueryStmt->mDataPlan;

    STL_DASSERT( sOptNode != NULL );
    STL_DASSERT( sDataArea != NULL );

    sState = 1;
    QLNX_FINALIZE_NODE( sOptNode,
                        sDataArea,
                        aEnv );

    /***************************************************************
     * Long Varying Data 원복 
     ***************************************************************/
    
    /**
     * Long Varying Data를 초기 크기로 원복한다.
     */

    sState = 2;
    if( aQueryStmt->mLongTypeValueList.mCount > 0 )
    {
        STL_TRY( qlndResetLongTypeValues( & aQueryStmt->mLongTypeValueList,
                                          aEnv )
                 == STL_SUCCESS );
    }
    
    /***************************************************************
     * Materialized Instant Table 제거 
     ***************************************************************/

    
    sInstantTable = (qlnInstantTable *) sResultSetDesc->mInstantTable;
    
    sState = 3;
    
    if ( sInstantTable != NULL )
    {
        /**
         * Iterator 해제
         */

        STL_TRY( qlnxDropInstantTable( sInstantTable,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Nothing To Do
         */
    }
    
    /***************************************************************
     * Named Cursor 정보 변경
     ***************************************************************/
    
    /**
     * Named 커서인 경우 Instance Cursor 의 OPEN 정보를 변경한다.
     */
        
    sState = 4;
    sInstDesc = sResultSetDesc->mInstDesc;
    
    if ( sInstDesc != NULL )
    {
        if ( ellCursorIsOpen( sInstDesc ) == STL_TRUE )
        {
            /**
             * Cursor 가 CLOSED 상태임을 설정 
             */

            if ( ellCursorHasOrigin( sInstDesc ) == STL_TRUE )
            {
                /**
                 * SQL Cursor 인 경우
                 */

                /**
                 * OPEN 구문이 할당한 SM Stmt 를 종료한다.
                 */

                STL_DASSERT( sInstDesc->mCursorStmt != NULL );
                sState = 0;
                STL_TRY( smlFreeStatement( sInstDesc->mCursorStmt, SML_ENV( aEnv ) ) == STL_SUCCESS );
                sInstDesc->mCursorStmt = NULL;
            }
            else
            {
                /**
                 * ODBC Cursor 인 경우 Session 이 할당한 SM Stmt 이므로 상위에서 관리한다.
                 */
            }

            /**
             * Cursor 가 CLOSED 상태임을 설정 
             */
            
            ellCursorSetClosed( sInstDesc, ELL_ENV(aEnv) );
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        /**
         * UnNamed 커서임.
         */
    }

    sState = 0;
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT(0);
    
    switch (sState)
    {
        case 1:
            {
                QLNX_FINALIZE_NODE_IGNORE_FAILURE( sOptNode,
                                                   sDataArea,
                                                   aEnv );
            }
        case 2:
            {
                if( aQueryStmt->mLongTypeValueList.mCount > 0 )
                {
                    (void) qlndResetLongTypeValues( & aQueryStmt->mLongTypeValueList,
                                                    aEnv );
                }
            }
        case 3:
            {
                sInstantTable = (qlnInstantTable *) sResultSetDesc->mInstantTable;
                if ( sInstantTable != NULL )
                {
                    (void) qlnxDropInstantTable( sInstantTable, aEnv );
                }
            }
        case 4:
            {
                sInstDesc = sResultSetDesc->mInstDesc;
                if ( sInstDesc != NULL )
                {
                    if ( ellCursorIsOpen( sInstDesc ) == STL_TRUE )
                    {
                        if ( ellCursorHasOrigin( sInstDesc ) == STL_TRUE )
                        {
                            (void) smlFreeStatement( sInstDesc->mCursorStmt, SML_ENV( aEnv ) );
                            sInstDesc->mCursorStmt = NULL;
                        }
                    
                        ellCursorSetClosed( sInstDesc, ELL_ENV(aEnv) );
                    }
                }
            }
        default:
            {
                break;
            }
    }
    
    return STL_FAILURE;
}




/***************************************************************
 * 정보 획득 함수 
 ***************************************************************/



/**
 * @brief Cursor Result Set 으로부터 RID Fetch 의 Transaction Read Mode 를 얻는다.
 */
smlTransReadMode qllCursorGetRIDFetchTRM( qllResultSetDesc * aResultSetDesc )
{
    return aResultSetDesc->mRIDFetchTRM;
}

/**
 * @brief Cursor Result Set 으로부터 RID Fetch 의 Statement Read Mode 를 얻는다.
 */
smlStmtReadMode  qllCursorGetRIDFetchSRM( qllResultSetDesc * aResultSetDesc )
{
    return aResultSetDesc->mRIDFetchSRM;
}




/***************************************************************
 * Sensitivity
 ***************************************************************/



/**
 * @brief SENSITIVE Result Set 정보를 설정한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aQueryStmt      SQL Statement
 * @param[in,out] aResultSetDesc  Result Set Descriptor
 * @param[in]     aEnv            Environment
 * @remarks
 */
stlStatus qlcrDataOptSetSensitiveResultSet( qllDBCStmt              * aDBCStmt,
                                            qllStatement            * aQueryStmt,
                                            qllResultSetDesc        * aResultSetDesc,
                                            qllEnv                  * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc->mCursorProperty.mSensitivity == ELL_CURSOR_SENSITIVITY_SENSITIVE,
                        QLL_ERROR_STACK(aEnv) );
    
    /*****************************************************
     * SENSITIVE cursor 를 위한 정보 설정 
     *****************************************************/

    /**
     * Standard Type 에 따른 Fetch RID Read Mode 설정
     */

    switch( aResultSetDesc->mCursorProperty.mStandardType )
    {
        case ELL_CURSOR_STANDARD_ISO:
            {
                aResultSetDesc->mRIDFetchTRM = SML_TRM_SNAPSHOT;
                aResultSetDesc->mRIDFetchSRM = SML_SRM_RECENT;
                break;
            }
        case ELL_CURSOR_STANDARD_ODBC_KEYSET:
            {
                aResultSetDesc->mRIDFetchTRM = SML_TRM_COMMITTED;  
                aResultSetDesc->mRIDFetchSRM = SML_SRM_RECENT;  
                break;
            }
        case ELL_CURSOR_STANDARD_ODBC_DYNAMIC:
            {
                aResultSetDesc->mRIDFetchTRM = SML_TRM_SNAPSHOT;  /* N/A */
                aResultSetDesc->mRIDFetchSRM = SML_SRM_SNAPSHOT;  /* N/A */
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * Scan Item List 구축
     */
    
    aResultSetDesc->mScanItemList = NULL;

    STL_TRY( qlcrDataOptBuildScanItemInfo( aDBCStmt,
                                           aQueryStmt,
                                           aQueryStmt->mResultSetDesc,
                                           aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief INSENSITIVE Result Set 정보를 설정한다.
 * @param[in,out] aResultSetDesc  Result Set Descriptor
 * @remarks
 */
void qlcrDataOptSetInsensitiveResultSet( qllResultSetDesc  * aResultSetDesc )
{
    /**
     * Parameter Validation
     */

    STL_DASSERT( aResultSetDesc->mCursorProperty.mSensitivity == ELL_CURSOR_SENSITIVITY_INSENSITIVE );

    /**
     * Fetch RID Read Mode 설정
     * - FetchRecord() 함수가 사용되지 않으나 초기화한다.
     */
    
    aResultSetDesc->mRIDFetchTRM = SML_TRM_SNAPSHOT;  /* N/A */
    aResultSetDesc->mRIDFetchSRM = SML_SRM_SNAPSHOT;  /* N/A */

    /**
     * Scan Item List 구축
     */
    
    aResultSetDesc->mScanItemList = NULL;
}





/**
 * @brief CURSOR의 Result Set Descriptor에 Scan Item 정보를 구축한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aQueryStmt      SQL Statement
 * @param[in,out] aResultSetDesc  Result Set Descriptor
 * @param[in]     aEnv            Environment
 */
stlStatus qlcrDataOptBuildScanItemInfo( qllDBCStmt              * aDBCStmt,
                                        qllStatement            * aQueryStmt,
                                        qllResultSetDesc        * aResultSetDesc,
                                        qllEnv                  * aEnv )
{
    qllDataArea           * sDataArea        = NULL;
    qlnoSelectStmt        * sOptSelectStmt   = NULL;
    qllOptimizationNode   * sOptNode         = NULL;
    
    qlvInitNode           * sTableNode       = NULL;
    qllCursorScanItem     * sScanItem        = NULL;
    qlvRefTableItem       * sScanTableItem   = NULL;
    
    qlvRefExprItem        * sExprItem        = NULL;
    qlvInitColumn         * sInitColumn      = NULL;
    qlvInitInnerColumn    * sInitInnerColumn = NULL;
    
    knlValueBlockList     * sNewBlockList    = NULL;
    knlValueBlockList     * sPrevBlockList   = NULL;

    qlvRefColExprList     * sColExprList     = NULL;
    qlvRefColListItem     * sColExprItem     = NULL;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ((qllOptimizationNode *)aQueryStmt->mCodePlan)->mType ==
                        QLL_PLAN_NODE_STMT_SELECT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 기본 정보 획득 
     */
    
    sOptSelectStmt =
        (qlnoSelectStmt *) ((qllOptimizationNode *)aQueryStmt->mCodePlan)->mOptNode;
    sDataArea = (qllDataArea *) aQueryStmt->mDataPlan;

    sOptNode = sOptSelectStmt->mQueryNode;

    /**
     * FROM 절에 명시된 Table Node를 바탕으로 Scan Item으로 구성
     */

    for ( sScanTableItem = sOptSelectStmt->mScanTableList->mHead;
          sScanTableItem != NULL;
          sScanTableItem = sScanTableItem->mNext )
    {
        STL_TRY( qlcrDataOptAddCursorItemUsingQueryNode( aQueryStmt,
                                                         sDataArea,
                                                         sOptNode,
                                                         QLL_CURSOR_ITEM_SCAN,
                                                         sScanTableItem->mTableNode,
                                                         aResultSetDesc,
                                                         & QLL_DATA_PLAN(aDBCStmt),
                                                         aEnv )
                 == STL_SUCCESS );
    }
            
    STL_DASSERT( aResultSetDesc->mScanItemList != NULL );
    
    /**
     * Cursor의 Target Expression List가 참조하고 있는 임시 Ref Column List를 추출한다.
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF( qlvRefColExprList ),
                                (void **) & sColExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sColExprList->mHead = NULL;
        
    sExprItem = ((qlvRefExprList *) (aResultSetDesc->mTargetExprList))->mHead;

    while( sExprItem != NULL )
    {
        /**
         * Target은 무조건 Inner Column으로 감싸고 있으므로
         * Original Expression을 이용하여 탐색한다.
         */

        STL_DASSERT( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        STL_TRY( qlvGetRefColList( sExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                   sExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                   sColExprList,
                                   & QLL_DATA_PLAN(aDBCStmt),
                                   aEnv )
                 == STL_SUCCESS );
                              
        sExprItem = sExprItem->mNext;
    }

    
    /**
     * Target Expression을 분석하여 Scan Item 정보를 구축
     */
    
    sColExprItem = sColExprList->mHead;
                
    while( sColExprItem != NULL )
    {
        sExprItem = sColExprItem->mColExprList->mHead;

        while( sExprItem != NULL )
        {
            /**
             * Base Table 의 Column 정보를 얻는다.
             */
            
            if( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_COLUMN )
            {
                sInitColumn = sExprItem->mExprPtr->mExpr.mColumn;
            }
            else
            {
                /**
                 * Inner Column인 경우 Column을 찾아 Original Expression을 	 	 
                 * 계속 따라간다. 	 	 
                 */ 	 	 
	 	 	 
                sInitInnerColumn = sExprItem->mExprPtr->mExpr.mInnerColumn; 	 	 
                while( (sInitInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_SUB_TABLE) &&
                       (sInitInnerColumn->mOrgExpr->mType != QLV_EXPR_TYPE_COLUMN) )
                {
                    STL_DASSERT( sInitInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

                    sInitInnerColumn = sInitInnerColumn->mOrgExpr->mExpr.mInnerColumn;
                }

                if( sInitInnerColumn->mRelationNode->mType == QLV_NODE_TYPE_SUB_TABLE )
                {
                    /* View인 경우 */
                    sExprItem = sExprItem->mNext;
                    continue;
                }

                STL_DASSERT( sInitInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_COLUMN );
                sInitColumn = sInitInnerColumn->mOrgExpr->mExpr.mColumn;
            }
            

            /**
             * Scan Item 을 찾아 Column Read Block 을 공유한다.
             */
            
            if( sTableNode != sInitColumn->mRelationNode )
            {
                /**
                 * 관련된 Scan Item 찾기
                 */

                sScanItem = aResultSetDesc->mScanItemList;
                while( sScanItem != NULL )
                {
                    if( sScanItem->mInitBaseTable == sInitColumn->mRelationNode )
                    {
                        break;
                    }
                    else
                    {
                        sScanItem = sScanItem->mNext;
                    }
                }

                if ( sScanItem == NULL )
                {
                    /**
                     * 해당 Column 이 Sensitive Table 의 Column 이 아님
                     */
                    
                    sTableNode = NULL;
                }
                else
                {
                    /**
                     * add column block
                     */
                    
                    sNewBlockList = NULL;
                    STL_TRY( qlndFindColumnBlockList( sDataArea,
                                                      sDataArea->mReadColumnBlock,
                                                      sExprItem->mExprPtr,
                                                      & QLL_DATA_PLAN(aDBCStmt),
                                                      & sNewBlockList,
                                                      aEnv )
                             == STL_SUCCESS );
                    
                    STL_DASSERT( sNewBlockList != NULL );

                    STL_TRY( knlInitShareBlock( & sNewBlockList,
                                                sNewBlockList,
                                                & QLL_DATA_PLAN(aDBCStmt),
                                                KNL_ENV( aEnv ) )
                             == STL_SUCCESS );

                    
                    /**
                     * ExprItem의 Expression이 InnerColumn인 경우
                     * 실제 base table의 column order를 구하여
                     * new block list에 설정해주어야 한다.
                     * -------------------------------------
                     *  상위에서 column/inner column 상관없이
                     *  실제 base table에 대한 column idx를 구했으므로
                     *  무조건 new block list의 column idx를 변경해준다.
                     */
                    
                    KNL_SET_BLOCK_TABLE_ID( sNewBlockList,
                                            ellGetColumnTableID( sInitColumn->mColumnHandle ) ); 

                    KNL_SET_BLOCK_COLUMN_ORDER( sNewBlockList,
                                                ellGetColumnIdx( sInitColumn->mColumnHandle ) );
                    
                    sPrevBlockList = sNewBlockList;
                    sScanItem->mRIDFetchInfo.mColList = sNewBlockList;

                    /**
                     * Table Node 설정
                     */
                    
                    sTableNode = sInitColumn->mRelationNode;
                }
            }
            else
            {
                /* 분석 중인 Base Table Node에 대한 Scan Item 정보 구성 */

                /**
                 * add column block
                 */

                sNewBlockList = NULL;
                STL_TRY( qlndFindColumnBlockList( sDataArea,
                                                  sDataArea->mReadColumnBlock,
                                                  sExprItem->mExprPtr,
                                                  & QLL_DATA_PLAN(aDBCStmt),
                                                  & sNewBlockList,
                                                  aEnv )
                         == STL_SUCCESS );
                    
                STL_DASSERT( sNewBlockList != NULL );

                STL_TRY( knlInitShareBlock( & sNewBlockList,
                                            sNewBlockList,
                                            & QLL_DATA_PLAN(aDBCStmt),
                                            KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                
                /**
                 * ExprItem의 Expression이 InnerColumn인 경우
                 * 실제 base table의 column order를 구하여
                 * new block list에 설정해주어야 한다.
                 * -------------------------------------
                 *  상위에서 column/inner column 상관없이
                 *  실제 base table에 대한 column idx를 구했으므로
                 *  무조건 new block list의 column idx를 변경해준다.
                 */

                KNL_SET_BLOCK_TABLE_ID( sNewBlockList,
                                        ellGetColumnTableID( sInitColumn->mColumnHandle ) ); 

                KNL_SET_BLOCK_COLUMN_ORDER( sNewBlockList,
                                            ellGetColumnIdx( sInitColumn->mColumnHandle ) );

                STL_DASSERT( KNL_GET_BLOCK_COLUMN_ORDER( sPrevBlockList ) <
                             KNL_GET_BLOCK_COLUMN_ORDER( sNewBlockList ) );
                
                KNL_LINK_BLOCK_LIST( sPrevBlockList, sNewBlockList );
                sPrevBlockList = sNewBlockList;
            }

            sExprItem = sExprItem->mNext;
            
        } /* while : qlvRefExprItem */

        sColExprItem = sColExprItem->mNext;
    
    } /* while : qlvRefColListItem */

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}    





/**
 * @brief Cursor Result Set 에 할당받은 Scan Item 을 추가한다.
 * @param[in] aResultSetDesc      Result Set Descriptor
 * @param[in] aInitBaseTable      Base Table Node
 * @param[in] aTablePhyHandle     Table Physical Handle
 * @param[in] aTableReadBlock     Table Read Block
 * @param[in] aTableRowBlock      Table Row Block
 * @param[in,out] aScanItem       Scan Table Information
 * @remarks
 */
void qlcrDataOptAddScanItem( qllResultSetDesc       * aResultSetDesc,
                             void                   * aInitBaseTable,
                             void                   * aTablePhyHandle,
                             knlValueBlockList      * aTableReadBlock,
                             smlRowBlock            * aTableRowBlock,
                             qllCursorScanItem      * aScanItem )
{
    smlFetchRecordInfo  * sFetchRecordInfo;
    
    /**
     * Parameter Validation
     */

    STL_ASSERT( aResultSetDesc != NULL );
    STL_ASSERT( aInitBaseTable != NULL );
    STL_ASSERT( aTablePhyHandle != NULL );
    /* STL_ASSERT( aTableReadBlock != NULL ); */
    STL_ASSERT( aTableRowBlock != NULL );
    STL_ASSERT( aScanItem != NULL );

    /**
     * Scan Item 정보 설정
     */
    
    aScanItem->mScanRowBlock  = aTableRowBlock;
    aScanItem->mInitBaseTable = aInitBaseTable;

    sFetchRecordInfo = & aScanItem->mRIDFetchInfo;
    
    sFetchRecordInfo->mRelationHandle = aTablePhyHandle;
    sFetchRecordInfo->mColList        = aTableReadBlock;
    sFetchRecordInfo->mRowIdColList   = NULL;
    sFetchRecordInfo->mPhysicalFilter = NULL;
    
    sFetchRecordInfo->mTransReadMode  = aResultSetDesc->mRIDFetchTRM;
    sFetchRecordInfo->mStmtReadMode   = aResultSetDesc->mRIDFetchSRM;
    sFetchRecordInfo->mScnBlock       = aTableRowBlock->mScnBlock;         
    sFetchRecordInfo->mLogicalFilter  = NULL;
    sFetchRecordInfo->mLogicalFilterEvalInfo = NULL;
    
    sFetchRecordInfo->mViewScn = 0;
    sFetchRecordInfo->mViewTcn = 0;

    /**
     * Scan Item 정보 연결
     */
    
    if ( aResultSetDesc->mScanItemList == NULL )
    {
        aScanItem->mNext = NULL;
        aResultSetDesc->mScanItemList = aScanItem;
    }
    else
    {
        aScanItem->mNext = aResultSetDesc->mScanItemList;
        aResultSetDesc->mScanItemList = aScanItem;
    }
}



/***************************************************************
 * Updatability
 ***************************************************************/



/**
 * @brief FOR UPDATE Result Set 정보를 설정한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aQueryStmt      Query Statement
 * @param[in,out] aResultSetDesc  Result Set Descriptor
 * @param[in]     aEnv            Environment
 * @remarks
 */
stlStatus qlcrDataOptSetUpdatableResultSet( qllDBCStmt              * aDBCStmt,
                                            qllStatement            * aQueryStmt,
                                            qllResultSetDesc        * aResultSetDesc,
                                            qllEnv                  * aEnv )
{
    qlvInitSelect * sInitPlan = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc->mCursorProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_UPDATE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sInitPlan = (qlvInitSelect *) qllGetInitPlan( aQueryStmt );

    /*****************************************************
     * UPDATABLE Cursor 를 위한 정보 설정 
     *****************************************************/

    aResultSetDesc->mLockWaitMode = sInitPlan->mLockWaitMode;
    aResultSetDesc->mLockTimeInterval = sInitPlan->mLockTimeInterval;

    aResultSetDesc->mLockItemList = NULL;

    STL_TRY( qlcrDataOptBuildLockItemInfo( aDBCStmt,
                                           aQueryStmt,
                                           sInitPlan,
                                           aResultSetDesc,
                                           aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief READ ONLY Result Set 정보를 설정한다.
 * @param[in,out] aResultSetDesc  Result Set Descriptor
 * @remarks
 */
void qlcrDataOptSetReadOnlyResultSet( qllResultSetDesc  * aResultSetDesc )
{
    /**
     * Parameter Validation
     */

    STL_DASSERT( aResultSetDesc->mCursorProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY );

    /**
     * READ ONLY Query 임
     */

    aResultSetDesc->mLockWaitMode     = QLL_CURSOR_LOCK_WAIT_NA;
    aResultSetDesc->mLockTimeInterval = SML_LOCK_TIMEOUT_INVALID;
    aResultSetDesc->mLockItemList     = NULL;
}

/**
 * @brief CURSOR의 Result Set Descriptor에 Lock Item 정보를 구축한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aQueryStmt      SQL Statement
 * @param[in]     aInitSelect     SELECT Init Plan
 * @param[in,out] aResultSetDesc  Result Set Descriptor
 * @param[in]     aEnv            Environment
 */

stlStatus qlcrDataOptBuildLockItemInfo( qllDBCStmt              * aDBCStmt,
                                        qllStatement            * aQueryStmt,
                                        qlvInitSelect           * aInitSelect,
                                        qllResultSetDesc        * aResultSetDesc,
                                        qllEnv                  * aEnv )
{
    qllDataArea           * sDataArea        = NULL;
    qlnoSelectStmt        * sOptSelectStmt   = NULL;
    qllOptimizationNode   * sOptNode         = NULL;

    qlvRefTableItem       * sLockTableItem   = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitSelect != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 기본 정보 획득
     */

    sDataArea = (qllDataArea *) aQueryStmt->mDataPlan;
    sOptSelectStmt =
        (qlnoSelectStmt *) ((qllOptimizationNode *)aQueryStmt->mCodePlan)->mOptNode;

    
    /**
     * Lock Table List 를 순회하며 Result Set 에 Lock Item 정보를 추가한다.
     */

    sOptNode = sOptSelectStmt->mQueryNode;
    
    for ( sLockTableItem = sOptSelectStmt->mLockTableList->mHead;
          sLockTableItem != NULL;
          sLockTableItem = sLockTableItem->mNext )
    {
        STL_TRY( qlcrDataOptAddCursorItemUsingQueryNode( aQueryStmt,
                                                         sDataArea,
                                                         sOptNode,
                                                         QLL_CURSOR_ITEM_LOCK,
                                                         sLockTableItem->mTableNode,
                                                         aResultSetDesc,
                                                         & QLL_DATA_PLAN(aDBCStmt),
                                                         aEnv )
                 == STL_SUCCESS );
    }
            
    STL_DASSERT( aResultSetDesc->mLockItemList != NULL );

    if( aInitSelect->mSerialAction == QLV_SERIAL_NO_RETRY )
    {
        /**
         * WHERE, JOIN, ORDER BY 가 없는 다음과 같은 FOR UPDATE
         * - SELECT .. FROM t1 FOR UPDATE;
         * - RETRY 를 수행하지 않고 FetchRecord 를 통해 해결한다.
         */

        STL_TRY( qlcrDataOptBuildNoRetryLockItemInfo( aDBCStmt,
                                                      sDataArea,
                                                      aResultSetDesc,
                                                      aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * - RETRY 를 수행하므로 FetchRecord 를 수행하지 않는다.
         * - nothing to do 
         */
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}





/**
 * @brief Query Node를 기반으로 Lock Item 정보를 추가한다.
 * @param[in]     aQueryStmt      SQL Statement
 * @param[in]     aDataArea       Data Area
 * @param[in]     aOptNode        Optimization Node
 * @param[in]     aItemType       Cursor Item Type
 * @param[in]     aInitTableNode  Lock Table 의 Init Node
 * @param[in,out] aResultSetDesc  Result Set Descriptor
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 */
stlStatus qlcrDataOptAddCursorItemUsingQueryNode( qllStatement            * aQueryStmt,
                                                  qllDataArea             * aDataArea,
                                                  qllOptimizationNode     * aOptNode,
                                                  qllCursorItemType         aItemType,
                                                  qlvInitNode             * aInitTableNode,
                                                  qllResultSetDesc        * aResultSetDesc,
                                                  knlRegionMem            * aRegionMem,
                                                  qllEnv                  * aEnv )
{
    qllOptimizationNode   * sOptNode         = NULL;
    qllOptimizationNode   * sLeftOptNode     = NULL;
    qllExecutionNode      * sExecNode        = NULL;

    qlnoTableAccess       * sOptTableAccess  = NULL;
    qlnoIndexAccess       * sOptIndexAccess  = NULL;
    qlnoRowIdAccess       * sOptRowIdAccess  = NULL;
    qlnoConcat            * sOptConcat       = NULL;

    void                  * sTablePhyHandle  = NULL;
    smlRowBlock           * sTableRowBlock   = NULL;

    qlnxRowBlockItem      * sRowBlockItem    = NULL;

    qllCursorLockItem     * sLockItem = NULL;
    qllCursorScanItem     * sScanItem = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aQueryStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aItemType > QLL_CURSOR_ITEM_NA) &&
                        (aItemType < QLL_CURSOR_ITEM_MAX),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Query Node 에서 Init Table Node 와 동일한 Code Table Node 를 찾는다.
     */

    sOptNode = aOptNode;

    sExecNode = & aDataArea->mExecNodeList[ sOptNode->mIdx ];
    sRowBlockItem =
        ((qlnxCommonInfo*)sExecNode->mExecNode)->mRowBlockList->mHead;

    while( sRowBlockItem != NULL )
    {
        sLeftOptNode = sRowBlockItem->mLeafOptNode;

        while( STL_TRUE )
        {
            if( sLeftOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE )
            {
                sLeftOptNode = ((qlnoSubQueryList *)(sLeftOptNode->mOptNode))->mChildNode;
            }
            else if( sLeftOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE )
            {
                sLeftOptNode = ((qlnoSubQueryFilter *)(sLeftOptNode->mOptNode))->mChildNode;
                STL_DASSERT( sLeftOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE );
                sLeftOptNode = ((qlnoSubQueryList *)(sLeftOptNode->mOptNode))->mChildNode;
            }
            else
            {
                break;
            }
        }
        
        switch( sLeftOptNode->mType )
        {
            case QLL_PLAN_NODE_TABLE_ACCESS_TYPE :
                {
                    /**
                     * Table Access Node
                     */

                    sOptTableAccess =
                        (qlnoTableAccess *) sLeftOptNode->mOptNode;

                    if( qlvIsSameBaseTable( sOptTableAccess->mRelationNode, aInitTableNode ) == STL_TRUE )
                    {
                        sTablePhyHandle = ellGetTableHandle( sOptTableAccess->mTableHandle );
                        sTableRowBlock = sRowBlockItem->mRowBlock;

                        STL_THROW( RAMP_FOUND );
                    }
                    else
                    {
                        /**
                         * 동일한 Table 이 아님
                         */
                    }

                    break;
                }
            case QLL_PLAN_NODE_INDEX_ACCESS_TYPE :
                {
                    /**
                     * Index Access Node
                     */

                    sOptIndexAccess
                        = (qlnoIndexAccess *) sLeftOptNode->mOptNode;

                    if( qlvIsSameBaseTable( sOptIndexAccess->mRelationNode, aInitTableNode ) == STL_TRUE )
                    {
                        sTablePhyHandle = sOptIndexAccess->mTablePhysicalHandle;
                        sTableRowBlock = sRowBlockItem->mRowBlock;

                        STL_THROW( RAMP_FOUND );
                    }
                    else
                    {
                        /**
                         * 동일한 Table 이 아님
                         */
                    }

                    break;
                }
            case QLL_PLAN_NODE_ROWID_ACCESS_TYPE :
                {
                    /**
                     * RowId Access Node
                     */

                    sOptRowIdAccess =
                        (qlnoRowIdAccess *) sLeftOptNode->mOptNode;

                    if( qlvIsSameBaseTable( sOptRowIdAccess->mRelationNode, aInitTableNode ) == STL_TRUE )
                    {
                        sTablePhyHandle = sOptRowIdAccess->mPhysicalHandle;
                        sTableRowBlock = sRowBlockItem->mRowBlock;

                        STL_THROW( RAMP_FOUND );
                    }
                    else
                    {
                        /**
                         * 동일한 Table 이 아님
                         */
                    }

                    break;
                }
            case QLL_PLAN_NODE_CONCAT_TYPE :
                {
                    /**
                     * Index Access Node
                     */

                    sOptConcat
                        = (qlnoConcat *) sLeftOptNode->mOptNode;
                    sOptIndexAccess
                        = (qlnoIndexAccess *) sOptConcat->mChildNodeArr[0]->mOptNode;

                    if( qlvIsSameBaseTable( sOptIndexAccess->mRelationNode, aInitTableNode ) == STL_TRUE )
                    {
                        sTablePhyHandle = sOptIndexAccess->mTablePhysicalHandle;
                        sTableRowBlock = sRowBlockItem->mRowBlock;

                        STL_THROW( RAMP_FOUND );
                    }
                    else
                    {
                        /**
                         * 동일한 Table 이 아님
                         */
                    }

                    break;
                }
            default:
                STL_DASSERT( 0 );
                break;
        }

        sRowBlockItem = sRowBlockItem->mNext;
    }


    /**
     * 반드시 존재해야 함.
     */
    
    STL_RAMP( RAMP_FOUND );

    STL_DASSERT( sTablePhyHandle != NULL );
    STL_DASSERT( sTableRowBlock != NULL );

    switch ( aItemType )
    {
        case QLL_CURSOR_ITEM_SCAN:
            {
                /**
                 * Scan Item 공간 할당
                 */
                
                STL_TRY( knlAllocRegionMem( aRegionMem,                         
                                            STL_SIZEOF(qllCursorScanItem),      
                                            (void **) & sScanItem,            
                                            KNL_ENV(aEnv) )                     
                         == STL_SUCCESS );                                      
                stlMemset( sScanItem, 0x00, STL_SIZEOF(qllCursorScanItem) );    
                
                /**
                 * Result Set 에 Scan Item 등록 
                 */
                
                qlcrDataOptAddScanItem( aResultSetDesc,
                                        aInitTableNode,
                                        sTablePhyHandle,
                                        NULL,
                                        sTableRowBlock,
                                        sScanItem );
                break;
            }
        case QLL_CURSOR_ITEM_LOCK:
            {
                /**
                 * Lock Item 공간 할당
                 */
                
                STL_TRY( knlAllocRegionMem( aRegionMem,                         
                                            STL_SIZEOF(qllCursorLockItem),      
                                            (void **) & sLockItem,            
                                            KNL_ENV(aEnv) )                     
                         == STL_SUCCESS );                                      
                stlMemset( sLockItem, 0x00, STL_SIZEOF(qllCursorLockItem) );    
                
                /**
                 * Result Set 에 Lock Item 등록 
                 */
                
                qlcrDataOptAddLockItem( aResultSetDesc,
                                        sTablePhyHandle,
                                        sTableRowBlock,
                                        sLockItem );
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }
                
                
    return STL_SUCCESS;

   
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Cursor Result Set 에 할당받은 Lock Item 을 추가한다.
 * @param[in] aResultSetDesc      Result Set Descriptor
 * @param[in] aTablePhyHandle     Table Physical Handle
 * @param[in] aTableRowBlock      Table Row Block
 * @param[in,out] aLockItem       Lock Table Information
 * @remarks
 */
void qlcrDataOptAddLockItem( qllResultSetDesc       * aResultSetDesc,
                             void                   * aTablePhyHandle,
                             smlRowBlock            * aTableRowBlock,
                             qllCursorLockItem      * aLockItem )
{
    /**
     * Parameter Validation
     */

    STL_ASSERT( aResultSetDesc != NULL );
    STL_ASSERT( aTablePhyHandle != NULL );
    STL_ASSERT( aTableRowBlock != NULL );
    STL_ASSERT( aLockItem != NULL );

    /**
     * Lock Item 정보 설정
     */
    
    aLockItem->mTablePhyHandle  = aTablePhyHandle;
    aLockItem->mLockRowBlock    = aTableRowBlock;

    /**
     * Lock Item 정보 연결
     */
    
    if ( aResultSetDesc->mLockItemList == NULL )
    {
        aLockItem->mNext = NULL;
        aResultSetDesc->mLockItemList = aLockItem;
    }
    else
    {
        aLockItem->mNext = aResultSetDesc->mLockItemList;
        aResultSetDesc->mLockItemList = aLockItem;
    }
}



/**
 * @brief NO_RETRY 를 위한 Lock Item Information 을 구축한다.
 * @param[in] aDBCStmt        DBC Statement
 * @param[in] aDataArea       Data Area
 * @param[in] aResultSetDesc  Result Set Descriptor
 * @param[in] aEnv            Environment
 * @remarks
 * SELECT c1 + c2 FROM t1 FOR UPDATE 와 같이
 * WHERE 절, JOIN, ORDER BY 가 없는 Lock Item 정보를 구축한다.
 */
stlStatus qlcrDataOptBuildNoRetryLockItemInfo( qllDBCStmt              * aDBCStmt,
                                               qllDataArea             * aDataArea,
                                               qllResultSetDesc        * aResultSetDesc,
                                               qllEnv                  * aEnv )
{
    qllCursorLockItem     * sLockTableItem   = NULL;
    
    qlvRefColExprList     * sColExprList     = NULL;
    qlvRefColListItem     * sColExprItem     = NULL;
    
    qlvRefExprItem        * sExprItem        = NULL;
    qlvInitColumn         * sInitColumn      = NULL;
    qlvInitInnerColumn    * sInitInnerColumn = NULL; 

    knlValueBlockList     * sColumnBlockList = NULL;
    knlValueBlockList     * sPrevBlockList   = NULL;
    knlValueBlockList     * sNewBlockList    = NULL;

    smlFetchRecordInfo    * sFetchRecordInfo = NULL;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc->mLockItemList->mNext == NULL,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */
    
    sLockTableItem = aResultSetDesc->mLockItemList;

        
    /******************************************
     * Target Expression 에 Lock Table 과 관련한
     * 컬럼이 존재할 경우 연산 재수행을 위한 컬럼 정보를 저장한다.
     ******************************************/

    /**
     * Target Expression 에 포함된 Column List를 추출
     */
    
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF( qlvRefColExprList ),
                                (void **) & sColExprList,
                                KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    sColExprList->mHead = NULL;
    
    sExprItem = ((qlvRefExprList *) (aResultSetDesc->mTargetExprList))->mHead;
    
    while( sExprItem != NULL )
    {
        /**
         * Target은 무조건 Inner Column으로 감싸고 있으므로
         * Original Expression을 이용하여 탐색한다.
         */
            
        STL_DASSERT( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
        
        STL_TRY( qlvGetRefColList( sExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                   sExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                                   sColExprList,
                                   & QLL_DATA_PLAN(aDBCStmt),
                                   aEnv )
                 == STL_SUCCESS );
        
        sExprItem = sExprItem->mNext;
    }
    

    /**
     * Column List 중 Lock Table 에 해당하는 Column 정보를 구축
     */
    
    sColExprItem = sColExprList->mHead;
                
    while( sColExprItem != NULL )
    {
        sExprItem = sColExprItem->mColExprList->mHead;
        
        while( sExprItem != NULL )
        {
            /**
             * Read Column List 에서 찾아 이를 Share
             */

            sNewBlockList = NULL;
            STL_TRY( qlndFindColumnBlockList( aDataArea,
                                              aDataArea->mReadColumnBlock,
                                              sExprItem->mExprPtr,
                                              & QLL_DATA_PLAN(aDBCStmt),
                                              & sNewBlockList,
                                              aEnv )
                     == STL_SUCCESS );
            STL_DASSERT( sNewBlockList != NULL );

            STL_TRY( knlInitShareBlock( & sNewBlockList,
                                        sNewBlockList,
                                        & QLL_DATA_PLAN(aDBCStmt),
                                        KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            
            /**
             * Base Table 의 Column 정보를 얻는다.
             */
            
            if( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_COLUMN )
            {
                sInitColumn = sExprItem->mExprPtr->mExpr.mColumn;
            }
            else
            {
                /**
                 * Inner Column인 경우 Column을 찾아 Original Expression을 	 	 
                 * 계속 따라간다. 	 	 
                 */ 	 	 
	 	 	 
                sInitInnerColumn = sExprItem->mExprPtr->mExpr.mInnerColumn; 	 	 
                while( (sInitInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_SUB_TABLE) &&
                       (sInitInnerColumn->mOrgExpr->mType != QLV_EXPR_TYPE_COLUMN) )
                { 	 	 
                    STL_DASSERT( sInitInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
	 	 	 
                    sInitInnerColumn = sInitInnerColumn->mOrgExpr->mExpr.mInnerColumn; 	 	 
                } 	 	 

                if( sInitInnerColumn->mRelationNode->mType == QLV_NODE_TYPE_SUB_TABLE )
                {
                    /* View인 경우 */
                    sExprItem = sExprItem->mNext;
                    continue;
                }

                STL_DASSERT( sInitInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_COLUMN ); 	 	 
                sInitColumn = sInitInnerColumn->mOrgExpr->mExpr.mColumn; 	 	 
            }
            
            KNL_SET_BLOCK_TABLE_ID( sNewBlockList,
                                    ellGetColumnTableID( sInitColumn->mColumnHandle ) ); 

            KNL_SET_BLOCK_COLUMN_ORDER( sNewBlockList,
                                        ellGetColumnIdx( sInitColumn->mColumnHandle ) );
                    

            /**
             * Value Block List 구성
             */

            if( sPrevBlockList == NULL )
            {
                sColumnBlockList = sNewBlockList;
            }
            else
            {
                STL_DASSERT( KNL_GET_BLOCK_COLUMN_ORDER( sPrevBlockList ) <
                             KNL_GET_BLOCK_COLUMN_ORDER( sNewBlockList ) );

                KNL_LINK_BLOCK_LIST( sPrevBlockList, sNewBlockList );
            }
            sPrevBlockList = sNewBlockList;


            /**
             * 다음 Expression Item
             */
                
            sExprItem = sExprItem->mNext;
            
        } /* while : qlvRefExprItem */

        sColExprItem = sColExprItem->mNext;
        
    } /* while : qlvRefColListItem */

        
    /******************************************
     * Fetch Record Information 구축 
     ******************************************/

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlFetchRecordInfo ),
                                (void **) & sFetchRecordInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sFetchRecordInfo->mRelationHandle = sLockTableItem->mTablePhyHandle;
    sFetchRecordInfo->mColList        = sColumnBlockList;
    sFetchRecordInfo->mRowIdColList   = NULL;
    sFetchRecordInfo->mPhysicalFilter = NULL;
    
    sFetchRecordInfo->mTransReadMode  = SML_TRM_COMMITTED;
    sFetchRecordInfo->mStmtReadMode   = SML_SRM_SNAPSHOT;
    sFetchRecordInfo->mScnBlock       = sLockTableItem->mLockRowBlock->mScnBlock;
    sFetchRecordInfo->mLogicalFilter  = NULL;
    sFetchRecordInfo->mLogicalFilterEvalInfo = NULL;
    
    sFetchRecordInfo->mViewScn = 0;
    sFetchRecordInfo->mViewTcn = 0;
    
    sLockTableItem->mFetchRecordInfo = sFetchRecordInfo;
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/***************************************************************
 * Result Set Materialization
 ***************************************************************/

/**
 * @brief Materialized Result Set 정보를 설정한다.
 * @param[in]  aDBCStmt       DBC Statement
 * @param[in]  aQueryStmt     Query Statement
 * @param[in]  aResultSetDesc Result Set Descriptor
 * @param[in]  aEnv           Environment
 * @remarks
 * Updatable 이거나 Scrollable 인 경우 저장할 정보를 결정한다.
 */ 
stlStatus qlcrDataOptSetMaterializedResultSet( qllDBCStmt              * aDBCStmt,
                                               qllStatement            * aQueryStmt,
                                               qllResultSetDesc        * aResultSetDesc,
                                               qllEnv                  * aEnv )
{
    qllCursorScanItem * sScanItem = NULL;
    qllCursorLockItem * sLockItem = NULL;

    stlInt32               sColOrder     = 0;
    knlValueBlockList    * sBlockItem    = NULL;
    knlValueBlockList    * sNewBlock     = NULL;
    knlValueBlockList    * sLastBlock    = NULL;
    qllDataArea          * sDataArea     = NULL;
    qlnInstantTable      * sInstantTable = NULL;
    smlFetchInfo         * sFetchInfo    = NULL;
    stlInt32  i = 0;
    
    /**
     * Parameter Valdiation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );

    /*************************************************
     * Materialized Result Set 정보를 설정
     *************************************************/

    sDataArea = (qllDataArea *) aQueryStmt->mDataPlan;
    
    aResultSetDesc->mMaterialized = STL_TRUE;

    /*************************************************
     * Sensitivity 에 따른 Result Set 정보 설정
     *************************************************/
    
    aResultSetDesc->mStoreBlockList = NULL;

    if ( aResultSetDesc->mCursorProperty.mSensitivity == ELL_CURSOR_SENSITIVITY_SENSITIVE )
    {
        /**
         * Sensitive 인 경우 Scan Item 정보를 저장
         */

        STL_DASSERT( aResultSetDesc->mScanItemList != NULL );

        sColOrder = 0;
        sNewBlock = NULL;
        
        for ( sScanItem = aResultSetDesc->mScanItemList;
              sScanItem != NULL;
              sScanItem = sScanItem->mNext )
        {
            /**
             * SCN value block 생성
             */
            
            STL_TRY( qlndInitBlockList( & aQueryStmt->mLongTypeValueList, 
                                        & sNewBlock,
                                        sDataArea->mBlockAllocCnt,
                                        STL_TRUE, /* IsSepBuff */
                                        STL_LAYER_SQL_PROCESSOR,
                                        0, /* TableID */
                                        sColOrder,
                                        DTL_TYPE_BINARY,
                                        STL_SIZEOF( smlScn ),
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
                     == STL_SUCCESS );

            /**
             * Value Pointer 를 Scan Item SCN 정보와 공유하도록 Link 변경
             * Instant Table 에 저장시 별도의 length 설정을 하지 않도록 길이값을 미리 기록함
             */

            for ( i = 0; i < sDataArea->mBlockAllocCnt; i++ )
            {
                sNewBlock->mValueBlock->mDataValue[i].mLength = STL_SIZEOF( smlScn );
                sNewBlock->mValueBlock->mDataValue[i].mValue
                    = & sScanItem->mScanRowBlock->mScnBlock[i];
            }
            
            /**
             * link
             */
            
            if( sLastBlock == NULL )
            {
                aResultSetDesc->mStoreBlockList = sNewBlock;
            }
            else
            {
                KNL_LINK_BLOCK_LIST( sLastBlock, sNewBlock );
            }
            sLastBlock = sNewBlock;
            sColOrder++;
            
            /**
             * RID value block 생성
             */
            STL_TRY( qlndInitBlockList( & aQueryStmt->mLongTypeValueList,
                                        & sNewBlock,
                                        sDataArea->mBlockAllocCnt,
                                        STL_TRUE, /* IsSepBuff */
                                        STL_LAYER_SQL_PROCESSOR,
                                        0, /* TableID */
                                        sColOrder,
                                        DTL_TYPE_BINARY,
                                        STL_SIZEOF( smlRid ),
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
                     == STL_SUCCESS );

            /**
             * Value Pointer 를 Scan Item RID 정보와 공유하도록 Link 변경
             */

            for ( i = 0; i < sDataArea->mBlockAllocCnt; i++ )
            {
                sNewBlock->mValueBlock->mDataValue[i].mLength = STL_SIZEOF( smlRid );
                sNewBlock->mValueBlock->mDataValue[i].mValue
                    = & sScanItem->mScanRowBlock->mRidBlock[i];
            }
            
            /**
             * link
             */
            
            KNL_LINK_BLOCK_LIST( sLastBlock, sNewBlock );
            sLastBlock = sNewBlock;
            sColOrder++;
        }
    }
    else
    {
        /**
         * Insensitive 인 경우 Target Value 정보를 공유 
         */

        sColOrder = 0;
        for ( sBlockItem = aResultSetDesc->mTargetBlock;
              sBlockItem != NULL;
              sBlockItem = sBlockItem->mNext )
        {
            STL_TRY( knlInitShareBlock( & sNewBlock,
                                        sBlockItem,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sNewBlock->mColumnOrder = sColOrder;

            if ( aResultSetDesc->mStoreBlockList == NULL )
            {
                aResultSetDesc->mStoreBlockList = sNewBlock;
            }
            else
            {
                KNL_LINK_BLOCK_LIST( sLastBlock, sNewBlock );
            }

            sLastBlock = sNewBlock;
            sColOrder++;
        }
    }

    /*************************************************
     * Updatability 에 따른 Result Set 정보 설정
     *************************************************/

    if ( aResultSetDesc->mCursorProperty.mUpdatability
         == ELL_CURSOR_UPDATABILITY_FOR_UPDATE )
    {
        /**
         * FOR UPDATE 인 경우 Lock Item 정보를 저장
         */

        STL_DASSERT( sColOrder > 0 );
        STL_DASSERT( sLastBlock != NULL );
        
        STL_DASSERT( aResultSetDesc->mLockItemList != NULL );

        sNewBlock = NULL;
        
        for ( sLockItem = aResultSetDesc->mLockItemList;
              sLockItem != NULL;
              sLockItem = sLockItem->mNext )
        {
            /**
             * SCN value block 생성
             */
            
            STL_TRY( qlndInitBlockList( & aQueryStmt->mLongTypeValueList,
                                        & sNewBlock,
                                        sDataArea->mBlockAllocCnt,
                                        STL_TRUE, /* IsSepBuff */
                                        STL_LAYER_SQL_PROCESSOR,
                                        0, /* TableID */
                                        sColOrder,
                                        DTL_TYPE_BINARY,
                                        STL_SIZEOF( smlScn ),
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
                     == STL_SUCCESS );

            /**
             * Value Pointer 를 Lock Item SCN 정보와 공유하도록 Link 변경
             */

            for ( i = 0; i < sDataArea->mBlockAllocCnt; i++ )
            {
                sNewBlock->mValueBlock->mDataValue[i].mLength = STL_SIZEOF( smlScn );
                sNewBlock->mValueBlock->mDataValue[i].mValue
                    = & sLockItem->mLockRowBlock->mScnBlock[i];
            }
            
            /**
             * link
             */
            
            KNL_LINK_BLOCK_LIST( sLastBlock, sNewBlock );
            sLastBlock = sNewBlock;
            sColOrder++;
            
            /**
             * RID value block 생성
             */
            STL_TRY( qlndInitBlockList( & aQueryStmt->mLongTypeValueList,
                                        & sNewBlock,
                                        sDataArea->mBlockAllocCnt,
                                        STL_TRUE, /* IsSepBuff */
                                        STL_LAYER_SQL_PROCESSOR,
                                        0, /* TableID */
                                        sColOrder,
                                        DTL_TYPE_BINARY,
                                        STL_SIZEOF( smlRid ),
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
                     == STL_SUCCESS );

            /**
             * Value Pointer 를 Lock Item RID 정보와 공유하도록 Link 변경
             */

            for ( i = 0; i < sDataArea->mBlockAllocCnt; i++ )
            {
                sNewBlock->mValueBlock->mDataValue[i].mLength = STL_SIZEOF( smlRid );
                sNewBlock->mValueBlock->mDataValue[i].mValue
                    = & sLockItem->mLockRowBlock->mRidBlock[i];
            }
            
            /**
             * link
             */
            
            KNL_LINK_BLOCK_LIST( sLastBlock, sNewBlock );
            sLastBlock = sNewBlock;
            sColOrder++;
        }
        
    }
    else
    {
        /**
         * READ ONLY 인 경우 별도로 저장할 정보가 없다.
         */
    }
    
    /*************************************************
     * Instant Table 정보 설정
     *************************************************/
    
    /**
     * Instant Table 자료구조 공간 확보 
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnInstantTable ),
                                (void **) & sInstantTable,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );

   
    /**
     * Instant Table 연결
     */
    
    aResultSetDesc->mInstantTable = sInstantTable;

    
    /**
     * Instant Table에 대한 Fetch Info
     */

    sFetchInfo = & aResultSetDesc->mInstantFetchInfo;

    sFetchInfo->mRange           = NULL;
    sFetchInfo->mPhysicalFilter  = NULL;
    sFetchInfo->mKeyCompList     = NULL;
    sFetchInfo->mLogicalFilter   = NULL;

    sFetchInfo->mColList         = aResultSetDesc->mStoreBlockList;
    sFetchInfo->mRowIdColList    = NULL;

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlRowBlock ),
                                (void **) & sFetchInfo->mRowBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( smlInitRowBlock( sFetchInfo->mRowBlock,
                              sDataArea->mBlockAllocCnt,
                              & QLL_DATA_PLAN( aDBCStmt ),
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    sFetchInfo->mTableLogicalId  = KNL_ANONYMOUS_TABLE_ID;
    sFetchInfo->mSkipCnt         = 0;
    sFetchInfo->mFetchCnt        = QLL_FETCH_COUNT_MAX;
    sFetchInfo->mIsEndOfScan     = STL_FALSE;

    sFetchInfo->mFilterEvalInfo  = NULL;
    sFetchInfo->mFetchRecordInfo = NULL;
    sFetchInfo->mAggrFetchInfo   = NULL;
    sFetchInfo->mBlockJoinFetchInfo   = NULL;


    /*************************************************
     * Lock Row 정보 설정 
     *************************************************/
    
    if ( aResultSetDesc->mCursorProperty.mUpdatability
         == ELL_CURSOR_UPDATABILITY_FOR_UPDATE )
    {
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( stlBool ) * sDataArea->mBlockAllocCnt,
                                    (void **) & aResultSetDesc->mRowLocked,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( aResultSetDesc->mRowLocked,
                   0x00,
                   STL_SIZEOF( stlBool ) * sDataArea->mBlockAllocCnt );
    }
    else
    {
        aResultSetDesc->mRowLocked = NULL;
    }
         
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Non-Materialized Result Set 정보를 설정한다.
 * @param[in]  aResultSetDesc Result Set Descriptor
 * @remarks
 */ 
void qlcrDataOptSetNonMaterializedResultSet( qllResultSetDesc  * aResultSetDesc )
{
    aResultSetDesc->mMaterialized = STL_FALSE;
    aResultSetDesc->mStoreBlockList = NULL;
}






/**
 * @brief Materialized Result Set 으로부터 Block Fetch 한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aResultSetDesc  Result Set Desc
 * @param[out] aHasData        Data 존재 여부 
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus qlcrFetchBlockFromMaterialResult( smlTransId          aTransID,
                                            qllResultSetDesc  * aResultSetDesc,
                                            stlBool           * aHasData,
                                            qllEnv            * aEnv )
{
    qlnInstantTable      * sInstantTable = NULL;

    stlInt64               sReadCnt = 0;
    
    QLL_OPT_DECLARE( stlTime sBeginTime );
    QLL_OPT_DECLARE( stlTime sEndTime );


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv)  );
    STL_PARAM_VALIDATE( aResultSetDesc->mStoreBlockList != NULL, QLL_ERROR_STACK(aEnv)  );
    STL_PARAM_VALIDATE( aResultSetDesc->mInstantTable != NULL, QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginTime );


    /**
     * 기본 정보 획득
     */
    
    sInstantTable = (qlnInstantTable *) aResultSetDesc->mInstantTable;

    /**
     * fetch block
     */
    
    STL_TRY( smlFetchNext( sInstantTable->mIterator,
                           & aResultSetDesc->mInstantFetchInfo,
                           SML_ENV(aEnv) )
             == STL_SUCCESS );

    sReadCnt = aResultSetDesc->mInstantFetchInfo.mRowBlock->mUsedBlockSize;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aResultSetDesc->mStoreBlockList, 0, sReadCnt );
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aResultSetDesc->mTargetBlock, 0, sReadCnt );
    
    if( sReadCnt == 0 )
    {
        *aHasData = STL_FALSE;
    }
    else
    {
        *aHasData = STL_TRUE;
    }
    
    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aResultSetDesc->mQueryStmt->mOptInfo.mFetchTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aResultSetDesc->mQueryStmt->mOptInfo.mFetchCallCount, 1 );

    return STL_SUCCESS;
    
    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndTime );
    QLL_OPT_ADD_ELAPSED_TIME( aResultSetDesc->mQueryStmt->mOptInfo.mFetchTime, sBeginTime, sEndTime );
    QLL_OPT_ADD_COUNT( aResultSetDesc->mQueryStmt->mOptInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}

/** @} qlcrResultSet */




