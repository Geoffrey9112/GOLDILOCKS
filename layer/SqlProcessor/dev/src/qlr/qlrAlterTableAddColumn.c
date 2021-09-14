/*******************************************************************************
 * qlrAlterTableAddColumn.c
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
 * @file qlrAlterTableAddColumn.c
 * @brief ALTER TABLE .. ADD COLUMN .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qlrAlterTableAddColumn ALTER TABLE .. ADD COLUMN
 * @ingroup qlrAlterTable
 * @{
 */


/**
 * @brief ADD COLUMN 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrValidateAddColumn( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    qlrInitAddColumn       * sInitPlan = NULL;
    qlpAlterTableAddColumn * sParseTree = NULL;

    qlpListElement * sListElement = NULL;
    qlpColumnDef   * sColumnDef = NULL;

    qlvInitStmtExprList  * sStmtExprList  = NULL;
    qlvInitExprList      * sQueryExprList = NULL;
    
    ellDictHandle    sColumnHandle;

    qlrInitColumn  * sInitColumn = NULL;
    
    stlInt32         i = 0;
    stlInt32         sTableColCnt = 0;
    stlInt32         sTableUsedColCnt = 0;
    ellDictHandle  * sTableColHandle = NULL;

    stlBool         sObjectExist = STL_FALSE;

    stlInt32        sPrimaryKeyCnt = 0;   
    ellDictHandle * sPrimaryKeyHandle = NULL;
    void          * sRelationHandle = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ADD_COLUMN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLE_ADD_COLUMN_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    sParseTree = (qlpAlterTableAddColumn *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrInitAddColumn),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlrInitAddColumn) );

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

    sRelationHandle = ellGetTableHandle( & sInitPlan->mTableHandle );

    STL_TRY_THROW( smlIsAlterableTable( sRelationHandle ) == STL_TRUE,
                   RAMP_ERR_NOT_IMPLEMENTED );

    /**********************************************************
     * Expression List 생성
     **********************************************************/

    /**
     * Statement 단위 Expression List 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitStmtExprList ),
                                (void **) & sStmtExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mPseudoColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList->mConstExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sStmtExprList->mHasSubQuery = STL_FALSE;
    sInitPlan->mStmtExprList = sStmtExprList;


    /**
     * Query 단위 Expression List 설정
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitExprList ),
                                (void **) & sQueryExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mPseudoColExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mAggrExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sQueryExprList->mNestedAggrExprList,
                                   QLL_INIT_PLAN( aDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    sQueryExprList->mStmtExprList = sStmtExprList;
    sInitPlan->mQueryExprList = sQueryExprList;

    /**********************************************************
     * Column Definition Validation
     **********************************************************/

    /**
     * Column Definition 추가
     */
    
    sInitPlan->mHasIdentity = STL_FALSE;
    sInitPlan->mColumnList  = NULL;
    
    stlMemset( & sInitPlan->mAllConstraint, 0x00, STL_SIZEOF(qlrInitTableConstraint) );
    STL_TRY( ellInitObjectList( & sInitPlan->mAllConstraint.mConstSchemaList,
                                QLL_INIT_PLAN( aDBCStmt ),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sInitPlan->mAllConstraint.mConstSpaceList,
                                QLL_INIT_PLAN( aDBCStmt ),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    
    QLP_LIST_FOR_EACH( sParseTree->mColumnDefList, sListElement )
    {
        sColumnDef = (qlpColumnDef *) QLP_LIST_GET_POINTER_VALUE( sListElement );

        /**
         * 이미 존재하는 컬럼인지 검사
         */

        STL_TRY( ellGetColumnDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         & sInitPlan->mTableHandle,
                                         sColumnDef->mColumnName,
                                         & sColumnHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_FALSE, RAMP_ERR_COLUMN_EXIST );
        
        /**
         * Column Definition Validation
         */
        
        STL_TRY( qlrAddColumnDefinition( aTransID,
                                         aDBCStmt,
                                         aSQLStmt,
                                         aSQLString,
                                         sInitPlan->mQueryExprList,
                                         & sInitPlan->mSchemaHandle,
                                         & sInitPlan->mTableHandle,
                                         smlGetRelationType( sRelationHandle ),
                                         ellGetTableName( & sInitPlan->mTableHandle ),
                                         sColumnDef,
                                         & sInitPlan->mHasIdentity,
                                         & sInitPlan->mColumnList,
                                         & sInitPlan->mAllConstraint,
                                         aEnv )
                 == STL_SUCCESS );
    }

    /****************************************************************
     * Column Type Refinement
     * - DEFAULT expression 을 위해 정의되지 않은 Type 정보를 설정함.
     * - Dictionary 에는 정의되지 않은 정보를 그대로 사용함.
     ****************************************************************/
    
    /**
     * Column 개수 
     */

    sInitPlan->mColumnCount = 0;
    
    for ( sInitColumn = sInitPlan->mColumnList;
          sInitColumn != NULL;
          sInitColumn = sInitColumn->mNext )
    {
        sInitPlan->mColumnCount++;
    }
    
    STL_TRY( knlAllocRegionMem(
                 QLL_INIT_PLAN( aDBCStmt ),
                 STL_SIZEOF( qlvInitTypeDef ) * sInitPlan->mColumnCount,
                 (void **) & sInitPlan->mRefineColumnType,
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    i = 0;
    for ( sInitColumn = sInitPlan->mColumnList;
          sInitColumn != NULL;
          sInitColumn = sInitColumn->mNext )
    {
        qlrSetRefineColumnType( & sInitColumn->mTypeDef,
                                & sInitPlan->mRefineColumnType[i] );

        i++;
    }
          
    
    
    /**********************************************************
     * Default 존재 여부 
     **********************************************************/

    sTableColCnt = ellGetTableColumnCnt( & sInitPlan->mTableHandle );
    sTableUsedColCnt = ellGetTableUsedColumnCnt( & sInitPlan->mTableHandle );
    
    sInitPlan->mHasDefault = STL_FALSE;
    
    for ( sInitColumn = sInitPlan->mColumnList, i = 0;
          sInitColumn != NULL;
          sInitColumn = sInitColumn->mNext, i++ )
    {
        if ( (sInitColumn->mColumnDefault != NULL) &&
             (sInitColumn->mNullDefault == STL_FALSE) )
        {
            /**
             * 실제 DEFAULT value 가 존재함.
             */
            sInitPlan->mHasDefault = STL_TRUE;
        }
        else
        {
            /**
             * NO useful default value
             */
        }

        /**
         * Column 의 Physical Ordinal Position 조정
         */

        sInitColumn->mPhysicalOrdinalPosition = sTableColCnt + i;

        /**
         * Column 의 Logical Ordinal Position 조정
         */

        sInitColumn->mLogicalOrdinalPosition = sTableUsedColCnt + i;
    }
    
    /**********************************************************
     * Identity Validation
     **********************************************************/

    /**
     * 하나의 Table 에 Identity Column 이 2개 이상 존재할 수 없다.
     * - 기존의 Table 에 이미 존재하고 있는지 검사 
     */

    if ( sInitPlan->mHasIdentity == STL_TRUE )
    {
        sTableColCnt    = ellGetTableColumnCnt( & sInitPlan->mTableHandle );
        sTableColHandle = ellGetTableColumnDictHandle( & sInitPlan->mTableHandle );

        for ( i = 0; i < sTableColCnt; i++ )
        {
            /**
             * Unused 컬럼은 Identity 존재 여부 검사시 제외해야 함.
             */

            if ( ellGetColumnUnused( & sTableColHandle[i] ) == STL_TRUE )
            {
                continue;
            }
            
            if ( ellGetColumnIdentityHandle( & sTableColHandle[i] ) != NULL )
            {
                /**
                 * Identity 를 정의한 컬럼 정보 추출
                 */
                
                QLP_LIST_FOR_EACH( sParseTree->mColumnDefList, sListElement )
                {
                    sColumnDef =
                        (qlpColumnDef *) QLP_LIST_GET_POINTER_VALUE( sListElement );

                    if ( sColumnDef->mDefSecond != NULL )
                    {
                        if ( sColumnDef->mDefSecond->mIdentType == QLP_IDENTITY_INVALID )
                        {
                            /**
                             * Identity 정의가 없음
                             */
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        /**
                         * Second Definition 이 없음
                         */
                    }
                }

                STL_ASSERT( sColumnDef != NULL );
                STL_THROW( RAMP_ERR_MULTIPLE_IDENTITY );
            }
            else
            {
                /**
                 * go go
                 */
            }
        }
    }
    else
    {
        /**
         * nothing to do
         */
    }

    /*************************************************
     * PRIMARY KEY In-line Constraint 의 적합성 검사 
     *************************************************/
    
    /**
     * Primary Key 정보 획득
     */
    
    STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                              aSQLStmt->mViewSCN,
                                              QLL_INIT_PLAN(aDBCStmt),
                                              & sInitPlan->mTableHandle,
                                              & sPrimaryKeyCnt,
                                              & sPrimaryKeyHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Primary Key 가 이미 존재하고 있다면 Primary Key 제약 조건이 없어야 함.
     */

    if ( sPrimaryKeyCnt > 0 )
    {
        STL_TRY_THROW( sInitPlan->mAllConstraint.mPrimaryKey == NULL,
                       RAMP_ERR_MULTIPLE_PRIMARY_KEY );
    }
    else
    {
        /* 다수개의 Primary Key Column 추가는 이미 검사됨 */
    }


    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mTableHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

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
    
    /***********************************************************
     * Init Plan 연결 
     ***********************************************************/
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_BEING_ADDED_EXISTS_IN_TABLE,
                      qlgMakeErrPosString( aSQLString,
                                           sColumnDef->mColumnNamePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_MULTIPLE_IDENTITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_IDENTITY_COLUMN,
                      qlgMakeErrPosString( aSQLString,
                                           sColumnDef->mDefSecond->mIdentTypePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_MULTIPLE_PRIMARY_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_PRIMARY_KEY,
                      NULL,
                      QLL_ERROR_STACK( aEnv ) );
    }

    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "ALTER TABLE ADD COLUMN" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ADD COLUMN 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptCodeAddColumn( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    qlrInitAddColumn * sInitPlan = NULL;
    qlrCodeAddColumn * sCodePlan = NULL;

    qlrInitColumn      * sInitColumn = NULL;

    qloInitStmtExprList   * sStmtExprList   = NULL;
    qloInitExprList       * sQueryExprList  = NULL;

    qlvRefExprItem        * sExprItem       = NULL;
    stlUInt64               sExprCnt        = 0;

    qloExprInfo           * sTotalExprInfo  = NULL;
    knlExprStack          * sConstExprStack = NULL;

    knlExprEntry          * sExprEntry      = NULL;
    qloDBType             * sDBType         = NULL;
    qlvInitExpression     * sInitExpr       = NULL;
    knlExprStack          * sExprStack      = NULL;
    
    stlInt32 i = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ADD_COLUMN_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Valid Plan 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_CODE_OPTIMIZE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlrInitAddColumn *) aSQLStmt->mInitPlan;
    
    /**
     * Code Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrCodeAddColumn),
                                (void **) & sCodePlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sCodePlan, 0x00, STL_SIZEOF(qlrCodeAddColumn) );

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

    sStmtExprList->mInitExprList = sInitPlan->mStmtExprList;
    
    sStmtExprList->mConstExprStack = NULL;
    sStmtExprList->mTotalExprInfo  = NULL;

    /**
     * Query 단위 Expression List 생성
     */
    
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qloInitExprList ),
                                (void **) & sQueryExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sQueryExprList->mInitExprList = sInitPlan->mQueryExprList;
    sQueryExprList->mStmtExprList = sStmtExprList;

    sCodePlan->mStmtExprList  = sStmtExprList;
    sCodePlan->mQueryExprList = sQueryExprList;
    
    /************************************************************
     * Add DEFAULT expression
     ************************************************************/

    /**
     * Constant Expression 을 전체 list 에 추가 
     */
    
    sExprItem = sStmtExprList->mInitExprList->mConstExprList->mHead;
    while( sExprItem != NULL )
    {
        STL_DASSERT( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT );

        sInitExpr = sExprItem->mExprPtr->mExpr.mConstExpr->mOrgExpr;
        STL_TRY( qloAddExpr( sInitExpr,
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
     * 나머지 Expression 추가 
     */
    
    for ( i = 0, sInitColumn = sInitPlan->mColumnList;
          i < sInitPlan->mColumnCount;
          i++, sInitColumn = sInitColumn->mNext )
    {
        STL_TRY( qloAddExpr( sInitColumn->mInitDefault,
                             sStmtExprList->mAllExprList,
                             QLL_CODE_PLAN( aDBCStmt ),
                             aEnv )
                 == STL_SUCCESS );
    }

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

    /**
     * 각 Constant Expression을 하나의 Stack으로 구성
     */

    sExprItem = sStmtExprList->mInitExprList->mConstExprList->mHead;
    while( sExprItem != NULL )
    {
        sInitExpr = sExprItem->mExprPtr->mExpr.mConstExpr->mOrgExpr;

        STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                       sInitExpr,
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

        sDBType = & sTotalExprInfo->mExprDBType[ sInitExpr->mOffset ];
        
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
     * Complete DEFAULT Expression
     ****************************************************************/

    /**
     * Expression Stack 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem(
                 QLL_CODE_PLAN( aDBCStmt ),
                 STL_SIZEOF( knlExprStack ) * sInitPlan->mColumnCount,
                 (void **) & sCodePlan->mDefaultCodeStack,
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * DEFAULT 절 Expression Stack 구성
     */

    for ( i = 0, sInitColumn = sInitPlan->mColumnList;
          i < sInitPlan->mColumnCount;
          i++, sInitColumn = sInitColumn->mNext )
    {
        sInitExpr = sInitColumn->mInitDefault;
        sExprStack = & sCodePlan->mDefaultCodeStack[i];
            
    
        /**
         * 각 SET Expression 에 대한 Expression Stack 생성
         */
        
        STL_TRY( knlExprInit( sExprStack,
                              sInitExpr->mIncludeExprCnt,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        
        /**
         * 각 Value에 대한 Expression Stack 구성
         */
        
        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sInitExpr,
                                            aSQLStmt->mBindContext,
                                            sExprStack,
                                            sConstExprStack,
                                            sTotalExprInfo,
                                            & sExprEntry,
                                            QLL_CODE_PLAN( aDBCStmt ),
                                            aEnv )
                 == STL_SUCCESS );
        
            
        /**
         * DEFAULT expr 대한 DB type 설정
         */

        STL_TRY( qloCastExprDBType( sInitExpr,
                                    sTotalExprInfo,
                                    sInitPlan->mRefineColumnType[i].mDBType,
                                    sInitPlan->mRefineColumnType[i].mArgNum1,
                                    sInitPlan->mRefineColumnType[i].mArgNum2,
                                    sInitPlan->mRefineColumnType[i].mStringLengthUnit,
                                    sInitPlan->mRefineColumnType[i].mIntervalIndicator,
                                    STL_FALSE,
                                    STL_TRUE,
                                    aEnv )
                 == STL_SUCCESS );
    }    
    
    /**
     * Output 설정
     */

    aSQLStmt->mCodePlan = sCodePlan;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ADD COLUMN 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrOptDataAddColumn( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    qlrInitAddColumn * sInitPlan = NULL;
    qlrCodeAddColumn * sCodePlan = NULL;
    qlrDataAddColumn * sDataPlan = NULL;

    qllDataArea          * sDataArea      = NULL;
    qloDataOptExprInfo     sDataOptInfo;

    qlvInitExpression * sExpr          = NULL;

    stlInt32  i = 0;
    
    knlValueBlockList * sPrevBlockList = NULL;
    knlValueBlockList * sBlockList = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ADD_COLUMN_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Valid Plan 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_DATA_OPTIMIZE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qlrInitAddColumn *) aSQLStmt->mInitPlan;
    sCodePlan = (qlrCodeAddColumn *) aSQLStmt->mCodePlan;
    
    /**
     * Data Plan 생성
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF(qlrDataAddColumn),
                                (void **) & sDataPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sDataPlan, 0x00, STL_SIZEOF(qlrDataAddColumn) );

    /****************************************************************
     * Table 의 Column Element 공간 확보
     ****************************************************************/

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF(stlInt64) * sInitPlan->mColumnCount,
                                (void **) & sDataPlan->mColumnID,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sDataPlan->mColumnID,
               0x00,
               STL_SIZEOF(stlInt64) * sInitPlan->mColumnCount );

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF(ellAddColumnDesc) * sInitPlan->mColumnCount,
                                (void **) & sDataPlan->mAddColumnDesc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sDataPlan->mAddColumnDesc,
               0x00,
               STL_SIZEOF(ellAddColumnDesc) * sInitPlan->mColumnCount );

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
     * Block Allocation Count 설정 => Block Alloc Count로 설정
     */

    sDataArea->mBlockAllocCnt = knlGetPropertyBigIntValueByID( KNL_PROPERTY_BLOCK_READ_COUNT,
                                                               KNL_ENV(aEnv) );

    sDataArea->mIsFirstExecution = STL_TRUE;
    sDataArea->mIsEvaluateCost = STL_FALSE;


    /**
     * Execution Node List 할당
     */
    
    sDataArea->mExecNodeCnt = 0;
    sDataArea->mExecNodeList = NULL;

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
     * 전제 Pseudo Column 공간 확보 : Data Optimization 전에 수행 
     */

    STL_TRY( qlndAllocPseudoBlockList( aSQLStmt,
                                       sCodePlan->mStmtExprList->mInitExprList->mPseudoColExprList,
                                       sDataArea->mBlockAllocCnt,
                                       & sDataArea->mPseudoColBlock,
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

    
    /******************************************************************
     * 전체 expression 에 대한 공간 확보
     ******************************************************************/


    STL_TRY( qloDataOptimizeExpressionAll( & sDataOptInfo,
                                           aEnv )
             == STL_SUCCESS );
    
    
    /******************************************************************
     * Value Block List 공간 공유
     ******************************************************************/

    /**
     * Pseudo Column List 구성
     */

    if( sCodePlan->mStmtExprList->mInitExprList->mPseudoColExprList->mCount > 0 )
    {
        /* Total Pseudo Column List 구성 */
        STL_TRY( qlndBuildPseudoColBlockList( sDataArea,
                                              sCodePlan->mStmtExprList->mInitExprList->mPseudoColExprList,
                                              & sDataPlan->mTotalPseudoColBlock,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );

        /* Query Pseudo Column List 구성 */
        STL_TRY( qlndBuildPseudoColBlockList( sDataArea,
                                              sCodePlan->mQueryExprList->mInitExprList->mPseudoColExprList,
                                              & sDataPlan->mQueryPseudoColBlock,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sDataPlan->mTotalPseudoColBlock = NULL;
        sDataPlan->mQueryPseudoColBlock = NULL;
    }


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
     * DEFAULT expression Evaluation 정보  구성 
     ****************************************************************/

    /**
     * DEFAULT block list 구성
     */

    sDataPlan->mDefaultBlockList = NULL;
    
    for ( i = 0; i < sInitPlan->mColumnCount; i++ )
    {
        /**
         * Value Block 생성 
         */
        
        STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                    & sBlockList,
                                    sDataArea->mBlockAllocCnt,
                                    STL_TRUE, /* 컬럼임 */
                                    STL_LAYER_SQL_PROCESSOR,
                                    ellGetTableID( & sInitPlan->mTableHandle ),
                                    0,  /* aColumnOrder, Dictionary 추가 후 다시 설정해야 함 */
                                    sInitPlan->mRefineColumnType[i].mDBType,
                                    sInitPlan->mRefineColumnType[i].mArgNum1,
                                    sInitPlan->mRefineColumnType[i].mArgNum2,
                                    sInitPlan->mRefineColumnType[i].mStringLengthUnit,
                                    sInitPlan->mRefineColumnType[i].mIntervalIndicator,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    aEnv )
                 == STL_SUCCESS );

        /**
         * Link 설정 
         */

        if ( sDataPlan->mDefaultBlockList == NULL )
        {
            sDataPlan->mDefaultBlockList = sBlockList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevBlockList, sBlockList );
        }

        sPrevBlockList = sBlockList;
    }

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlExprEvalInfo ) * sInitPlan->mColumnCount,
                                (void **) & sDataPlan->mDefaultEvalInfo ,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for ( i = 0, sBlockList = sDataPlan->mDefaultBlockList;
          i < sInitPlan->mColumnCount;
          i++, sBlockList = sBlockList->mNext )
    {
        sDataPlan->mDefaultEvalInfo[i].mExprStack   = & sCodePlan->mDefaultCodeStack[i];
        sDataPlan->mDefaultEvalInfo[i].mContext     = sDataArea->mExprDataContext;
        sDataPlan->mDefaultEvalInfo[i].mResultBlock = sBlockList;
        sDataPlan->mDefaultEvalInfo[i].mBlockIdx    = 0;
        sDataPlan->mDefaultEvalInfo[i].mBlockCount  = 0;
    }
    
    /**
     * Row Block 공간 할당
     */
    
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
    
    /**
     * Data Plan 연결
     */

    aSQLStmt->mDataPlan = (void *) sDataPlan;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ADD COLUMN 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlrExecuteAddColumn( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv )
{
    qlrInitAddColumn * sInitPlan = NULL;
    qlrDataAddColumn * sDataPlan = NULL;

    stlTime sTime = 0;
    stlBool sLocked = STL_TRUE;
    
    stlBool sIsEmptyTable = STL_FALSE;

    qlrInitColumn * sCodeColumn = NULL;
    qlrInitConst  * sCodeConst = NULL;

    stlInt64   sCheckColumnID = ELL_DICT_OBJECT_ID_NA;
    
    stlInt64   sKeyIndexID = ELL_DICT_OBJECT_ID_NA;
    
    stlInt64   sKeyColumnID[DTL_INDEX_COLUMN_MAX_COUNT];

    ellObjectList * sAffectedViewList = NULL;

    stlInt32     i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLE_ADD_COLUMN_TYPE,
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

    sInitPlan = (qlrInitAddColumn *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    STL_TRY( ellLock4AddColumn( aTransID,
                                aStmt,
                                & sInitPlan->mTableHandle,
                                sInitPlan->mAllConstraint.mConstSchemaList,
                                sInitPlan->mAllConstraint.mConstSpaceList,
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

    sInitPlan = (qlrInitAddColumn *) aSQLStmt->mInitPlan;
    sDataPlan = (qlrDataAddColumn *) aSQLStmt->mDataPlan;

    /**********************************************************
     * 관련 객체 정보 획득 및 Lock 획득
     **********************************************************/
    
    /**
     * Empty Table 이 아닌 경우 In-line Constraint 가 적합한지 검사
     */

    STL_TRY( qlrCheckInlineEmptyTable( aTransID,
                                       aStmt,
                                       aDBCStmt,
                                       sInitPlan,
                                       & sIsEmptyTable,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Affected View List 획득 및 X Lock 획득
     */
    
    STL_TRY( ellGetAffectedViewListByRelation( aTransID,
                                               aStmt,
                                               & sInitPlan->mTableHandle,
                                               & QLL_EXEC_DDL_MEM(aEnv),
                                               & sAffectedViewList,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( ellLockRelatedTableList4DDL( aTransID,
                                          aStmt,
                                          SML_LOCK_X,
                                          sAffectedViewList,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    
    STL_TRY( ellGetAffectedViewListByRelation( aTransID,
                                               aStmt,
                                               & sInitPlan->mTableHandle,
                                               & QLL_EXEC_DDL_MEM(aEnv),
                                               & sAffectedViewList,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**********************************************************
     * Column 추가 
     **********************************************************/

    /**
     * Table 상위 객체 정보 설정
     */

    sDataPlan->mTableOwnerID   = ellGetTableOwnerID( & sInitPlan->mTableHandle );
    sDataPlan->mTableSchemaID  = ellGetTableSchemaID( & sInitPlan->mTableHandle );
    sDataPlan->mTableID        = ellGetTableID( & sInitPlan->mTableHandle );
    sDataPlan->mTablePhyHandle = ellGetTableHandle( & sInitPlan->mTableHandle );
    

    /**
     * Column 정보를 Dictionary 에 추가
     */

    for ( sCodeColumn = sInitPlan->mColumnList, i = 0;
          sCodeColumn != NULL;
          sCodeColumn = sCodeColumn->mNext, i++ )
    {
        STL_TRY( qlrExecuteAddColumnDesc( aTransID,
                                          aStmt,
                                          sDataPlan->mTableOwnerID,
                                          sDataPlan->mTableSchemaID,
                                          sDataPlan->mTableID,
                                          ELL_TABLE_TYPE_BASE_TABLE,
                                          sCodeColumn,
                                          & sDataPlan->mAddColumnDesc[i],
                                          & sDataPlan->mColumnID[i],
                                          aEnv )
                 == STL_SUCCESS );

    }

    /**************************************************************
     * Data 추가
     **************************************************************/

    /**
     * 물리적 또는 논리적으로 컬럼을 추가
     */
    
    if ( sIsEmptyTable == STL_TRUE )
    {
        /**
         * 물리적인 변경이 필요없음
         */
    }
    else
    {
        if ( (sInitPlan->mHasDefault == STL_TRUE) || (sInitPlan->mHasIdentity == STL_TRUE) )
        {
            /**
             * 물리적으로 컬럼을 추가해야 함.
             */

            STL_TRY( qlrAppendData4AddColumn( aTransID,
                                              aStmt,
                                              aDBCStmt,
                                              aSQLStmt,
                                              aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * 물리적인 변경이 필요없음
             */
        }
    }
    
    /**********************************************************
     * Primary Key Definition
     **********************************************************/

    if ( sInitPlan->mAllConstraint.mPrimaryKey == NULL )
    {
        /* Primary Key 정의가 없음 */
    }
    else
    {
        /**
         * Key Column 정보 구축
         */
        
        sCodeConst = sInitPlan->mAllConstraint.mPrimaryKey;

        for ( i = 0; i < sCodeConst->mColumnCnt; i++ )
        {
            sKeyColumnID[i]  = sDataPlan->mColumnID[sCodeConst->mColumnIdx[i]];
        }
        
        /**
         * Primary Key Index 생성 
         */

        STL_TRY( qlrExecuteAddKeyIndex( aTransID,
                                        aStmt,
                                        aDBCStmt,
                                        sDataPlan->mTableOwnerID,
                                        sDataPlan->mTableSchemaID,
                                        sDataPlan->mTableID,
                                        sDataPlan->mTablePhyHandle,
                                        sCodeConst,
                                        sKeyColumnID,
                                        sCodeConst->mColumnIdx,
                                        sDataPlan->mAddColumnDesc,
                                        & sKeyIndexID,
                                        aEnv )
                 == STL_SUCCESS );

        /**
         * Primary Key Constraint 추가
         */
        
        STL_TRY( qlrExecuteAddKeyConst( aTransID,
                                        aStmt,
                                        sDataPlan->mTableOwnerID,
                                        sDataPlan->mTableSchemaID,
                                        sDataPlan->mTableID,
                                        sCodeConst,
                                        sKeyIndexID,
                                        sKeyColumnID,
                                        aEnv )
                 == STL_SUCCESS );
    }

    /**********************************************************
     * Unique Key Definition
     **********************************************************/

    for ( sCodeConst = sInitPlan->mAllConstraint.mUniqueKey;
          sCodeConst != NULL;
          sCodeConst = sCodeConst->mNext )
    {
        /**
         * Key Column 정보 구축
         */
        
        for ( i = 0; i < sCodeConst->mColumnCnt; i++ )
        {
            sKeyColumnID[i]  = sDataPlan->mColumnID[sCodeConst->mColumnIdx[i]];
        }
        
        /**
         * Unique Key Index 생성 
         */

        STL_TRY( qlrExecuteAddKeyIndex( aTransID,
                                        aStmt,
                                        aDBCStmt,
                                        sDataPlan->mTableOwnerID,
                                        sDataPlan->mTableSchemaID,
                                        sDataPlan->mTableID,
                                        sDataPlan->mTablePhyHandle,
                                        sCodeConst,
                                        sKeyColumnID,
                                        sCodeConst->mColumnIdx,
                                        sDataPlan->mAddColumnDesc,
                                        & sKeyIndexID,
                                        aEnv )
                 == STL_SUCCESS );

        /**
         * Unique Key Constraint 추가
         */
        
        STL_TRY( qlrExecuteAddKeyConst( aTransID,
                                        aStmt,
                                        sDataPlan->mTableOwnerID,
                                        sDataPlan->mTableSchemaID,
                                        sDataPlan->mTableID,
                                        sCodeConst,
                                        sKeyIndexID,
                                        sKeyColumnID,
                                        aEnv )
                 == STL_SUCCESS );
    }

    /**********************************************************
     * Foreign Key Definition
     **********************************************************/

    for ( sCodeConst = sInitPlan->mAllConstraint.mUniqueKey;
          sCodeConst != NULL;
          sCodeConst = sCodeConst->mNext )
    {
        /**
         * @todo Foreign Key Constraint 를 구현해야 함
         */
    }

    /**********************************************************
     * Check Not Null Definition
     **********************************************************/

    for ( sCodeConst = sInitPlan->mAllConstraint.mCheckNotNull;
          sCodeConst != NULL;
          sCodeConst = sCodeConst->mNext )
    {
        /**
         * Check Column 정보 구축
         * - Check Constraint 는 하나의 컬럼에만 해당됨 
         */

        sCheckColumnID = sDataPlan->mColumnID[sCodeConst->mColumnIdx[0]];

        /**
         * Check Constraint 추가
         */

        STL_TRY( qlrExecuteAddCheckConst( aTransID,
                                          aStmt,
                                          sDataPlan->mTableOwnerID,
                                          sDataPlan->mTableSchemaID,
                                          sDataPlan->mTableID,
                                          sCodeConst,
                                          sCheckColumnID,
                                          aEnv )
                 == STL_SUCCESS );
    }
    
    /**********************************************************
     * Check Clause Definition
     **********************************************************/

    for ( sCodeConst = sInitPlan->mAllConstraint.mCheckClause;
          sCodeConst != NULL;
          sCodeConst = sCodeConst->mNext )
    {
        /**
         * Check Column 정보 구축
         * - Check Constraint 는 하나의 컬럼에만 해당됨 
         */

        sCheckColumnID = sDataPlan->mColumnID[sCodeConst->mColumnIdx[0]];

        /**
         * Check Constraint 추가
         */

        STL_TRY( qlrExecuteAddCheckConst( aTransID,
                                          aStmt,
                                          sDataPlan->mTableOwnerID,
                                          sDataPlan->mTableSchemaID,
                                          sDataPlan->mTableID,
                                          sCodeConst,
                                          sCheckColumnID,
                                          aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Affected View List 에 대해 IS_AFFECTED 를 TRUE 로 변경
     */

    STL_TRY( ellSetAffectedViewList( aTransID,
                                     aStmt,
                                     sAffectedViewList,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**********************************************************
     * Table 구조가 변경된 시간을 설정 
     **********************************************************/

    STL_TRY( ellSetTime4ModifyTableElement( aTransID,
                                            aStmt,
                                            & sInitPlan->mTableHandle,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**********************************************************
     * SQL-Table Cache 구축 
     **********************************************************/

    /**
     * SQL-Table Cache 구축
     */
    STL_TRY( ellRefineCache4AddColumn( aTransID,
                                       aStmt,
                                       & sInitPlan->mTableHandle,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlrExecuteAddColumn()" ); */
    /* } */
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief In-line Constraint 가 설정 가능한지 검사한다.
 * @param[in] aTransID       Transaction ID
 * @param[in] aStmt          Statement
 * @param[in] aDBCStmt       DBC Statement
 * @param[in] aInitPlan      Init Plan
 * @param[out] aIsEmptyTable Empty Table 여부 
 * @param[in] aEnv           Environment
 * @remarks
 * Table 에 Data 가 존재할 경우, In-line Constraint 를 설정하지 못할 수 있다.
 * Execution 시점에 다시 검사해야 함.
 */
stlStatus qlrCheckInlineEmptyTable( smlTransId         aTransID,
                                    smlStatement     * aStmt,
                                    qllDBCStmt       * aDBCStmt,
                                    qlrInitAddColumn * aInitPlan,
                                    stlBool          * aIsEmptyTable,
                                    qllEnv           * aEnv )
{
    stlBool         sIsEmptyTable = STL_FALSE;
    stlBool         sExistRow = STL_FALSE;

    qlrInitConst  * sInitConst = NULL;
    qlrInitColumn * sInitColumn = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    
    /*************************************************
     * 기본 정보 획득
     *************************************************/

    /**
     * Empty Table 인지 확인
     */

    STL_TRY( smlCheckTableExistRow( aStmt,
                                    ellGetTableHandle( & aInitPlan->mTableHandle ),
                                    & sExistRow,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    if ( sExistRow == STL_TRUE )
    {
        sIsEmptyTable = STL_FALSE;
    }
    else
    {
        sIsEmptyTable = STL_TRUE;
    }

    /*************************************************
     * PRIMARY KEY In-line Constraint 의 적합성 검사 
     *************************************************/

    /**
     * - NULL 검사는 not nullable 컬럼
     * - uniqueness 검사는 Index 생성시 검사
     */
    
    /*************************************************
     * UNIQUE KEY In-line Constraint 의 적합성 검사 
     *************************************************/

    /**
     * Index 생성시 검사됨.
     */
    

    /*************************************************
     * FOREIGN KEY In-line Constraint 의 적합성 검사 
     *************************************************/

    /**
     * @todo Data 추가시 검사해야 함.
     */
    
    /*************************************************
     * NOT NULL In-line Constraint 의 적합성 검사 
     *************************************************/

    /**
     * not nullable 컬럼에 대한 검사로 가능함.
     */

    if ( sIsEmptyTable == STL_TRUE )
    {
        /**
         * 비어 있는 테이블은 not nullable 컬럼을 추가할 수 있음.
         */
    }
    else
    {
        /**
         * 데이타가 존재하는 테이블에 not nullable 컬럼을 추가하려면, DEFAULT 또는 Identity Column 이어야 함.
         */

        for ( sInitColumn = aInitPlan->mColumnList;
              sInitColumn != NULL;
              sInitColumn = sInitColumn->mNext )
        {
            if ( sInitColumn->mIsNullable == STL_TRUE )
            {
                /**
                 * No Problem
                 */
            }
            else
            {
                /**
                 * Not Nullable 컬럼임
                 * - DEFAULT expression 이 존재하거나
                 * - Identity Column 이어야 함.
                 */

                STL_TRY_THROW( (sInitColumn->mColumnDefault != NULL) ||
                               (sInitColumn->mIsIdentity == STL_TRUE ),
                               RAMP_ERR_MANDOTARY_NOT_NULL );
            }
        }
    }
    
    /*************************************************
     * CHECK CLAUSE In-line Constraint 의 적합성 검사 
     *************************************************/

    for ( sInitConst = aInitPlan->mAllConstraint.mCheckClause;
          sInitConst != NULL;
          sInitConst = sInitConst->mNext )
    {
        /**
         * @todo ADD COLUMN 에 check constraint 가 존재할 경우의 적합성을 검사해야 함.
         * - Table Empty 가 아니라면 DEFAULT VALUE 와 CHECK 조건을 검사해야 함.
         */
    }

    /**
     * Output 설정
     */

    *aIsEmptyTable = sIsEmptyTable;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MANDOTARY_NOT_NULL )
    {
        stlPushError(
            STL_ERROR_LEVEL_ABORT,
            QLL_ERRCODE_TABLE_MUST_EMPTY_TO_ADD_MANDATORY_COLUMN,
            NULL,
            QLL_ERROR_STACK( aEnv ),
            gEllTableConstraintTypeString[ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL],
            sInitColumn->mColumnName );
    }

    STL_FINISH;

    return STL_FAILURE;
}
                               




/**
 * @brief ADD COLUMN 을 위한 Data 를 추가한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlrAppendData4AddColumn( smlTransId         aTransID,
                                   smlStatement     * aStmt,
                                   qllDBCStmt       * aDBCStmt,
                                   qllStatement     * aSQLStmt,
                                   qllEnv           * aEnv )
{
    qlrInitAddColumn * sInitPlan = NULL;
    qlrCodeAddColumn * sCodePlan = NULL;
    qlrDataAddColumn * sDataPlan = NULL;

    void                * sTableHandle = NULL;
    void                * sIterator = NULL;
    smlIteratorProperty   sIteratorProperty;
    smlFetchInfo          sFetchInfo;

    stlInt64              sUsedBlockCnt  = 0;
    stlInt64              sBlockIdx = 0;
    stlInt32              sColumnIdx = 0;

    qlrInitColumn       * sInitColumn = NULL;

    qlvInitPseudoCol   * sPseudoCode = NULL;
    
    knlValueBlockList   * sBlockList = NULL;
    knlExprEvalInfo     * sEvalInfo  = NULL;
    stlInt32              sState = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sInitPlan = (qlrInitAddColumn *) aSQLStmt->mInitPlan;
    sCodePlan = (qlrCodeAddColumn *) aSQLStmt->mCodePlan;
    sDataPlan = (qlrDataAddColumn *) aSQLStmt->mDataPlan;

    /******************************************************
     * Identity Column 의 Handle 정보 설정
     ******************************************************/

    /**
     * Identity Column 이 추가되는 경우 해당 handle 을 Init Expression 에 설정
     * - Execution 시점에 Validation 메모리의 변경하는 것이 예외 상황이나,
     * - plan cache 등으로 공유되지 않아 메모리 문제가 발생하지 않으며,
     * - Identity Handle 이 Execution 시점에 생성되므로 Execution 시점에 설정해야 함.
     */

    if ( sInitPlan->mHasIdentity == STL_TRUE )
    {
        /**
         * Identity Column 을 찾음
         */
        
        sColumnIdx = 0;
        for ( sInitColumn = sInitPlan->mColumnList;
              sInitColumn != NULL;
              sInitColumn = sInitColumn->mNext )
        {
            if ( sInitColumn->mIsIdentity == STL_TRUE )
            {
                break;
            }
            sColumnIdx++;
        }

        STL_DASSERT( sInitColumn != NULL );

        /**
         * Identity Handle 을 설정
         */
        
        sPseudoCode = sInitColumn->mInitDefault->mExpr.mPseudoCol;
        sPseudoCode->mArgs[0].mPseudoArg = sDataPlan->mAddColumnDesc[sColumnIdx].mIdentityHandle;
    }
    else
    {
        /* nothing to do */
    }
    
    /******************************************************
     * Default Block List 의 Column Physical Order 설정
     ******************************************************/

    sBlockList  = sDataPlan->mDefaultBlockList;
    
    sColumnIdx = 0;
    while( sBlockList != NULL )
    {
        sBlockList->mColumnOrder = sDataPlan->mAddColumnDesc[sColumnIdx].mOrdinalPosition;
        
        sBlockList = sBlockList->mNext;
        sColumnIdx++;
    }
    
    /******************************************************
     * CONSTANT EXPRESSION STACK 평가 
     ******************************************************/

    /**
     * transaction id 설정
     */

    sDataPlan->mDataArea->mTransID = aTransID;
    
    /**
     * STATEMENT 단위인 Constant Expression 및 Pseudo Column Expression 수행
     */

    STL_DASSERT( sDataPlan->mDataArea->mTransID == aTransID );
    
    STL_TRY( qlxEvaluateConstExpr( sCodePlan->mStmtExprList->mConstExprStack,
                                   sDataPlan->mDataArea,
                                   KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /***********************************************************************
     * OPEN Full Scan Iterator
     ***********************************************************************/
    
    /**
     * Full Scan Iterator 생성
     */

    SML_INIT_ITERATOR_PROP( sIteratorProperty );

    sTableHandle = ellGetTableHandle( & sInitPlan->mTableHandle );
    STL_TRY( smlAllocIterator( aStmt,
                               sTableHandle,
                               SML_TRM_COMMITTED,   /* DDL 임 */
                               SML_SRM_SNAPSHOT,
                               & sIteratorProperty,
                               SML_SCAN_FORWARD,
                               & sIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Fetch Information 초기화
     * - RID 정보만 얻을 수 있으면 된다.
     */
    
    SML_SET_FETCH_INFO_FOR_TABLE( & sFetchInfo,               /* Fetch Info */
                                  NULL,                       /* physical filter */
                                  NULL,                       /* logical filter */
                                  NULL,                       /* read column list */
                                  NULL,                       /* rowid column list */
                                  sDataPlan->mRowBlock,       /* row block */
                                  0,                          /* skip count */
                                  SML_FETCH_COUNT_MAX,        /* fetch count  */
                                  STL_FALSE,                  /* group key fetch */
                                  NULL );                     /* filter evaluate info */

    /***********************************************************************
     * Data 추가 
     ***********************************************************************/

    /**
     * Row 가 존재할 때까지 data 를 추가
     */
    
    while( 1 )
    {
        /**
         * Fetch Row
         */
        
        STL_TRY( smlFetchNext( sIterator,
                               & sFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sUsedBlockCnt = sFetchInfo.mRowBlock->mUsedBlockSize;

        /**
         * ROW 단위 Pseudo Column Expression 수행
         */
        
        if( sCodePlan->mQueryExprList->mInitExprList->mPseudoColExprList->mCount > 0 )
        {
            STL_TRY( qlxExecutePseudoColumnBlock( aTransID,
                                                  aStmt,
                                                  DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                                  & sInitPlan->mTableHandle,
                                                  sDataPlan->mRowBlock,
                                                  sCodePlan->mQueryExprList->mInitExprList->mPseudoColExprList,
                                                  sDataPlan->mQueryPseudoColBlock,
                                                  0,
                                                  sUsedBlockCnt,
                                                  aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* nothing to do */
        }

        /**
         * Block 개수만큼 DEFAULT expression 을 수행
         */
        
        KNL_SET_ALL_BLOCK_USED_CNT( sDataPlan->mDefaultBlockList, sUsedBlockCnt );
        
        for ( sBlockIdx = 0; sBlockIdx < sUsedBlockCnt; sBlockIdx++ )
        {
            /**
             * 추가된 컬럼에 대하여 DEFALUT expression 처리
             */

            sBlockList  = sDataPlan->mDefaultBlockList;
            sInitColumn = sInitPlan->mColumnList;
            sColumnIdx  = 0;
            
            while( sBlockList != NULL )
            {
                sEvalInfo = & sDataPlan->mDefaultEvalInfo[sColumnIdx];
                sEvalInfo->mBlockIdx   = sBlockIdx;
                sEvalInfo->mBlockCount = 1;

                /**
                 * DEFALUT expression 을 수행
                 */
                
                STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                                   KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                /**
                 * Not Nullable 컬럼의 null 존재 여부 검사
                 */
                
                if ( sInitColumn->mIsNullable == STL_TRUE )
                {
                    /* nothing to do */
                }
                else
                {
                    STL_TRY_THROW( DTL_IS_NULL( KNL_GET_BLOCK_DATA_VALUE( sBlockList, sBlockIdx ) ) == STL_FALSE,
                                   RAMP_ERR_NOT_NULL_INTEGRITY );
                }
                
                /**
                 * @todo CHECK Constraint 검사
                 */

                /**
                 * @todo REFERENCE Constraint 검사
                 */
            
                /**
                 * UNIQUE 제약과 PRIMARY KEY 제약은 Bottom-Up Build Index 를 통한 성능을 위해
                 * 모든 Data 를 추가한 후 index build 시 검사한다.
                 */
                
                sBlockList = sBlockList->mNext;
                sInitColumn = sInitColumn->mNext;
                sColumnIdx++;
            }
        }

        /**
         * 물리적으로 Data 를 추가
         */

        STL_TRY( smlAlterTableAddColumns( aStmt,
                                          sTableHandle,
                                          sDataPlan->mDefaultBlockList,
                                          sDataPlan->mRowBlock,
                                          SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * 종료 여부 판단
         */
        
        if ( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            /**
             * row 가 없음
             */
            
            break;
        }
        else
        {
            /**
             * row 가 존재함
             */
            continue;
        }
    }

    sState = 0;
    STL_TRY( smlFreeIterator( sIterator, SML_ENV(aEnv) ) == STL_SUCCESS );


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_NULL_INTEGRITY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_UPDATE_NULL,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sInitPlan->mSchemaHandle ),
                      ellGetTableName( & sInitPlan->mTableHandle ),
                      sInitColumn->mColumnName );
    }
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void) smlFreeIterator( sIterator, SML_ENV(aEnv) );
    }

    return STL_FAILURE;
}

/** @} qlrAlterTableAddColumn */
