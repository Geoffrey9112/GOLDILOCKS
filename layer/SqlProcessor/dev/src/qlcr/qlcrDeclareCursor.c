/*******************************************************************************
 * qlcrDeclareCursor.c
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
 * @file qlcrDeclareCursor.c
 * @brief DECLARE CURSOR 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>
#include <qlcrDeclareCursor.h>


/**
 * @addtogroup qlcrDeclareCursor
 * @{
 */


/**
 * @brief DECLARE CURSOR 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrValidateDeclareCursor( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     stlChar       * aSQLString,
                                     qllNode       * aParseTree,
                                     qllEnv        * aEnv )
{
    qlpDeclareCursor      * sParseTree = NULL;
    qlcrInitDeclareCursor * sInitPlan = NULL;

    qllDBCStmt      sCursorDBCStmt;
    qllStatement    sCursorSQLStmt;

    stlBool    sInitCursorDBC = STL_FALSE;
    stlBool    sInitCursorSQL = STL_FALSE;
    
    ellCursorDeclDesc     * sDeclDesc = NULL;

    stlChar  * sString = NULL;
    stlInt32   sStrLen = 0;
    stlInt32   sStrPos = 0;

    qllNode * sOriginParseTree = NULL;
    
    /**
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DECLARE_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_DECLARE_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpDeclareCursor *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlcrInitDeclareCursor),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlcrInitDeclareCursor) );

    /**
     * Cursor DBC/SQL Stmt 초기화 
     */
    
    STL_TRY( qllInitDBC( & sCursorDBCStmt, aEnv ) == STL_SUCCESS );
    sInitCursorDBC = STL_TRUE;
    
     STL_TRY( qllInitSQL( & sCursorSQLStmt, aEnv ) == STL_SUCCESS );
    sInitCursorSQL = STL_TRUE;
    
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

    STL_TRY( ellFindCursorDeclDesc( sInitPlan->mCursorName,
                                    sInitPlan->mProcID,
                                    & sDeclDesc,
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sDeclDesc == NULL, RAMP_ERR_SAME_CURSOR_EXIST );
                   
    /**********************************************************
     * Cursor Origin Parsing
     **********************************************************/
    
    /**
     * Cursor Origin 정보 생성 
     */

    sInitPlan->mOriginType = ELL_CURSOR_ORIGIN_STANDING_CURSOR;

    switch ( sInitPlan->mOriginType )
    {
        case ELL_CURSOR_ORIGIN_STANDING_CURSOR:
            {
                /**
                 * Cursor Query 의 Bind Count 임 
                 */

                aSQLStmt->mBindCount = 0;
                
                /**
                 * Cursor Origin 정보 설정
                 */
                
                sStrPos = sParseTree->mCursorQuery->mNodePos;
                sStrLen = stlStrlen(aSQLString) - sStrPos;
                
                STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aDBCStmt ),
                                            sStrLen + 1,
                                            (void **) & sInitPlan->mCursorOrigin,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                stlMemcpy( sInitPlan->mCursorOrigin, & aSQLString[sStrPos], sStrLen );
                sInitPlan->mCursorOrigin[sStrLen] = '\0';
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
    
     /**
      * Plan Memory를 사용하지 않는다.
      */

     qllPlanCacheMemOFF( aDBCStmt );
     
     STL_TRY( qlgParseSQL( & sCursorDBCStmt,
                           & sCursorSQLStmt,
                           QLL_PHASE_VALIDATION,
                           sInitPlan->mCursorOrigin,
                           STL_FALSE, /* aIsRecompile */
                           STL_FALSE, /* Search Plan Cache */
                           & sOriginParseTree,
                           aEnv )
              == STL_SUCCESS );

    /**********************************************************
     * Cursor Property 획득
     **********************************************************/
    
    if ( sParseTree->mStandardType == NULL )
    {
        /**
         * ISO Standard Cursor
         */

        sInitPlan->mStmtCursorProperty.mStandardType = ELL_CURSOR_STANDARD_ISO;

        /**
         * Sensitivity
         */
        switch ( QLP_GET_INT_VALUE(sParseTree->mSensitivity) )
        {
            case ELL_CURSOR_SENSITIVITY_NA:
                {
                    sInitPlan->mStmtCursorProperty.mSensitivity
                        = ELL_CURSOR_DEFAULT_SENSITIVITY;
                    break;
                }
            case ELL_CURSOR_SENSITIVITY_INSENSITIVE:
            case ELL_CURSOR_SENSITIVITY_SENSITIVE:
            case ELL_CURSOR_SENSITIVITY_ASENSITIVE:
                {
                    sInitPlan->mStmtCursorProperty.mSensitivity
                        = QLP_GET_INT_VALUE(sParseTree->mSensitivity);
                    break;
                }
            default:
                {
                    STL_ASSERT(0);
                    break;
                }
        }

        /**
         * Scrollability
         */
        switch ( QLP_GET_INT_VALUE(sParseTree->mScrollability) )
        {
            case ELL_CURSOR_SCROLLABILITY_NA:
                {
                    sInitPlan->mStmtCursorProperty.mScrollability
                        = ELL_CURSOR_DEFAULT_SCROLLABILITY;
                    break;
                }
            case ELL_CURSOR_SCROLLABILITY_NO_SCROLL:
            case ELL_CURSOR_SCROLLABILITY_SCROLL:
                {
                    sInitPlan->mStmtCursorProperty.mScrollability
                        = QLP_GET_INT_VALUE(sParseTree->mScrollability);
                    break;
                }
            default:
                {
                    STL_ASSERT(0);
                    break;
                }
        }
    }
    else
    {
        /**
         * ODBC Cursor
         */
        
        sInitPlan->mStmtCursorProperty.mStandardType
            = QLP_GET_INT_VALUE(sParseTree->mStandardType);

        /**
         * Sensitivity
         */

        switch( sInitPlan->mStmtCursorProperty.mStandardType )
        {
            case ELL_CURSOR_STANDARD_ODBC_STATIC:
                {
                    sInitPlan->mStmtCursorProperty.mSensitivity
                        = ELL_CURSOR_SENSITIVITY_INSENSITIVE;
                    break;
                }
            case ELL_CURSOR_STANDARD_ODBC_KEYSET:
                {
                    sInitPlan->mStmtCursorProperty.mSensitivity
                        = ELL_CURSOR_SENSITIVITY_ASENSITIVE;
                    break;
                }
            case ELL_CURSOR_STANDARD_ODBC_DYNAMIC:
                {
                    /**
                     * Dynamic Cursor 는 Sensitive 해야 함
                     */
                    
                    sInitPlan->mStmtCursorProperty.mSensitivity
                        = ELL_CURSOR_SENSITIVITY_SENSITIVE;
                    break;
                }
            default:
                {
                    STL_ASSERT(0);
                    break;
                }
        }

        /**
         * Scrollability
         */
        
        sInitPlan->mStmtCursorProperty.mScrollability = ELL_CURSOR_SCROLLABILITY_SCROLL;
    }
        
    /**
     * Updatablility
     */
    
    switch ( sCursorSQLStmt.mStmtType )
    {
        case QLL_STMT_SELECT_TYPE:
            {
                sInitPlan->mStmtCursorProperty.mUpdatability
                    = ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY;
                break;
            }
        case QLL_STMT_SELECT_FOR_UPDATE_TYPE :
            {
                sInitPlan->mStmtCursorProperty.mUpdatability
                    = ELL_CURSOR_UPDATABILITY_FOR_UPDATE;
                break;
            }
        case QLL_STMT_INSERT_RETURNING_QUERY_TYPE:
        case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE:
        case QLL_STMT_DELETE_RETURNING_QUERY_TYPE:
            {
                sInitPlan->mStmtCursorProperty.mUpdatability
                    = ELL_CURSOR_UPDATABILITY_FOR_READ_ONLY;
                break;
            }
        default:
            {
                STL_THROW( RAMP_ERR_INVALID_CURSOR_QUERY );
                break;
            }
    }

    /**
     * Holdability
     */
    
    switch ( QLP_GET_INT_VALUE(sParseTree->mHoldability) )
    {
        case ELL_CURSOR_HOLDABILITY_UNSPECIFIED:
            {
                if ( sInitPlan->mStmtCursorProperty.mUpdatability
                     == ELL_CURSOR_UPDATABILITY_FOR_UPDATE )
                {
                    sInitPlan->mStmtCursorProperty.mHoldability
                        = ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD;
                }
                else
                {
                    switch ( sCursorSQLStmt.mStmtType )
                    {
                        case QLL_STMT_SELECT_TYPE:
                            {
                                sInitPlan->mStmtCursorProperty.mHoldability
                                    = ELL_CURSOR_HOLDABILITY_WITH_HOLD;
                                break;
                            }
                        case QLL_STMT_SELECT_FOR_UPDATE_TYPE :
                            {
                                sInitPlan->mStmtCursorProperty.mHoldability
                                    = ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD;
                                break;
                            }
                        case QLL_STMT_INSERT_RETURNING_QUERY_TYPE:
                        case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE:
                        case QLL_STMT_DELETE_RETURNING_QUERY_TYPE:
                            {
                                sInitPlan->mStmtCursorProperty.mHoldability
                                    = ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD;
                                break;
                            }
                        default:
                            {
                                STL_THROW( RAMP_ERR_INVALID_CURSOR_QUERY );
                                break;
                            }
                    }
                }
                break;
            }
        case ELL_CURSOR_HOLDABILITY_WITH_HOLD:
        case ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD:
            {
                sInitPlan->mStmtCursorProperty.mHoldability
                    = QLP_GET_INT_VALUE(sParseTree->mHoldability);
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * Cursor Returnability
     * @todo 향후 Cursor Returnability 지원해야 함.
     */

    sInitPlan->mStmtCursorProperty.mReturnability = ELL_CURSOR_DEFAULT_RETURNABILITY;

    /**
     * Origin Statement 의 Cursor Property 설정
     */

    qllSetSQLStmtCursorProperty( & sCursorSQLStmt,
                                 sInitPlan->mStmtCursorProperty.mStandardType,
                                 sInitPlan->mStmtCursorProperty.mSensitivity,
                                 sInitPlan->mStmtCursorProperty.mScrollability,
                                 sInitPlan->mStmtCursorProperty.mHoldability,
                                 sInitPlan->mStmtCursorProperty.mUpdatability,
                                 sInitPlan->mStmtCursorProperty.mReturnability );
    sCursorSQLStmt.mStmtCursorProperty.mIsDbcCursor = STL_FALSE;
    
    /**********************************************************
     * Cursor Origin Query Validation 
     **********************************************************/
     
    /**
     * Cursor Origin Validation 
     */

    sCursorSQLStmt.mViewSCN = aSQLStmt->mViewSCN;
    
    STL_TRY( qlgValidateSQL( aTransID,
                             & sCursorDBCStmt,
                             & sCursorSQLStmt,
                             QLL_PHASE_VALIDATION,
                             sInitPlan->mCursorOrigin,
                             sOriginParseTree,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 구문별 권한 검사가 이루어지며, 별도의 권한 검사 없음
     */

    /**
     * Cursor SQL/DBC Stmt 종료
     */
    
    sInitCursorSQL = STL_FALSE;
    STL_TRY( qllFiniSQL( & sCursorDBCStmt, & sCursorSQLStmt, aEnv ) == STL_SUCCESS );
    
    sInitCursorDBC = STL_FALSE;
    STL_TRY( qllFiniDBC( & sCursorDBCStmt, aEnv ) == STL_SUCCESS );
    
    /**********************************************************
     * Init Plan 연결 
     **********************************************************/

    /**
     * Init Plan 정보 설정
     */
    
    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SAME_CURSOR_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_ALREADY_DECLARED,
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
                      "qlcrValidateDeclareCursor()" );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CURSOR_QUERY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_CURSOR_QUERY,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mCursorQuery->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      qlgGetStmtTypeKeyString(sParseTree->mCursorQuery->mType - QLL_STMT_NA) );
    }
    
    STL_FINISH;

    if ( sInitCursorSQL == STL_TRUE )
    {
        (void) qllFiniSQL( & sCursorDBCStmt, & sCursorSQLStmt, aEnv );
    }
    
    if ( sInitCursorDBC == STL_TRUE )
    {
        (void) qllFiniDBC( & sCursorDBCStmt, aEnv );
    }
    
    return STL_FAILURE;
}

/**
 * @brief DECLARE CURSOR 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrCodeOptimizeDeclareCursor( smlTransId      aTransID,
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
 * @brief DECLARE CURSOR 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrDataOptimizeDeclareCursor( smlTransId      aTransID,
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
 * @brief DECLARE CURSOR 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrExecuteFuncDeclareCursor( smlTransId      aTransID,
                                        smlStatement  * aStmt,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv )
{
    qlcrInitDeclareCursor * sInitPlan = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_DECLARE_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 기본 정보 획득
     */

    sInitPlan = (qlcrInitDeclareCursor *) aSQLStmt->mInitPlan;

    /*****************************************************
     * 미구현 사항 
     *****************************************************/

    switch ( sInitPlan->mStmtCursorProperty.mStandardType )
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

    switch ( sInitPlan->mStmtCursorProperty.mReturnability )
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

    /*****************************************************
     * 커서를 선언한다.
     *****************************************************/

    STL_TRY( qlcrDeclareSQLCursor( sInitPlan->mCursorName,
                                   sInitPlan->mProcID,
                                   sInitPlan->mOriginType,
                                   sInitPlan->mCursorOrigin,
                                   & sInitPlan->mStmtCursorProperty,
                                   aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlcrExecuteFuncDeclareCursor()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}
    


/**
 * @brief CURSOR 를 선언한다.
 * @param[in]  aCursorName     Cursor Name
 * @param[in]  aProcID         Procedure ID (Procedure가 아니라면 ELL_DICT_OBJECT_ID_NA)
 * @param[in]  aOriginType     Cursor Origin Type
 * @param[in]  aCursorOrigin   Cursor Origin Text
 * @param[in]  aCursorProperty Cursor Property
 * @param[in]  aEnv            Environment
 */
stlStatus qlcrDeclareSQLCursor( stlChar                  * aCursorName,
                                stlInt64                   aProcID,
                                ellCursorOriginType        aOriginType,
                                stlChar                  * aCursorOrigin,
                                ellCursorProperty        * aCursorProperty,
                                qllEnv                   * aEnv )
{
    ellCursorDeclDesc * sDeclDesc = NULL;

    ellCursorInstDesc * sInstDesc = NULL;
    qllDBCStmt        * sCursorDBCStmt = NULL;
    qllStatement      * sCursorSQLStmt = NULL;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCursorName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aOriginType > ELL_CURSOR_ORIGIN_NA) &&
                        (aOriginType < ELL_CURSOR_ORIGIN_MAX),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorOrigin   != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorProperty != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 동일한 커서가 존재하는 지 검사
     */
    
    STL_TRY( ellFindCursorDeclDesc( aCursorName,
                                    aProcID,
                                    & sDeclDesc,
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );
        
    STL_TRY_THROW( sDeclDesc == NULL, RAMP_ERR_SAME_CURSOR_EXIST );

    switch ( aOriginType )
    {
        case ELL_CURSOR_ORIGIN_STANDING_CURSOR:
            {
                STL_TRY( ellInsertStandingCursorDeclDesc( aCursorName,
                                                          aProcID,
                                                          aCursorOrigin,
                                                          aCursorProperty,
                                                          & sDeclDesc,
                                                          ELL_ENV(aEnv) )
                         == STL_SUCCESS );
                STL_ASSERT( sDeclDesc != NULL );
                break;
            }
        case ELL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR:
        case ELL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR:
        case ELL_CURSOR_ORIGIN_RECEIVED_CURSOR:
            {
                /**
                 * @todo Standing Cursor 만이 구현되어 있음
                 */
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }
    sState = 1;

    /**
     * Cursor Instance Descriptor 를 추가
     */

    STL_TRY( ellInsertCursorInstDesc( sDeclDesc,
                                      STL_SIZEOF( qllDBCStmt ),
                                      STL_SIZEOF( qllStatement ),
                                      & sInstDesc,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    /**
     * Cursor 의 DBC Statement 와 SQL Statement 초기화
     */

    sCursorDBCStmt = (qllDBCStmt *)   sInstDesc->mCursorDBCStmt;
    sCursorSQLStmt = (qllStatement *) sInstDesc->mCursorSQLStmt;

    STL_TRY( qllInitDBC( sCursorDBCStmt,
                         aEnv ) == STL_SUCCESS );

    STL_TRY( qllInitSQL( sCursorSQLStmt, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SAME_CURSOR_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_ALREADY_DECLARED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aCursorName );
    }

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlcrDeclareSQLCursor()" );
    }
    
    STL_FINISH;

    switch (sState)
    {
        case 2:
            (void) ellRemoveCursorInstDesc( sInstDesc, ELL_ENV(aEnv) );
        case 1:
            (void) ellRemoveCursorDeclDesc( sDeclDesc, ELL_ENV(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}



/** @} qlcrDeclareCursor */
