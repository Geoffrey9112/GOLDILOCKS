/*******************************************************************************
 * qllCursor.c
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
 * @file qllCursor.c
 * @brief CURSOR 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qllCursor
 * @{
 */

/****************************************************************
 * Cursor 속성 획득 함수
 ****************************************************************/

/**
 * @brief SQL Statement 의 Cursor Type 을 획득한다.
 * @param[in]  aSQLStmt   SQL Statement
 * @remarks
 */
ellCursorStandardType   qllGetCursorStandardType( qllStatement * aSQLStmt )
{
    ellCursorStandardType sType = ELL_CURSOR_STANDARD_NA;
    
    qlvInitPlan * sInitPlan = NULL;

    sInitPlan = qlvGetQueryInitPlan( aSQLStmt );
    
    if ( sInitPlan != NULL )
    {
        sType = sInitPlan->mResultCursorProperty.mStandardType;
    }
    else
    {
        sType = ELL_CURSOR_STANDARD_NA;
    }

    return sType;
}

/**
 * @brief SQL Statement 의 Sensitivity 을 획득한다.
 * @param[in]  aSQLStmt   SQL Statement
 * @remarks
 */
ellCursorSensitivity    qllGetCursorSensitivity( qllStatement * aSQLStmt )
{
    ellCursorSensitivity sType = ELL_CURSOR_SENSITIVITY_NA;
    
    qlvInitPlan * sInitPlan = NULL;

    sInitPlan = qlvGetQueryInitPlan( aSQLStmt );
    
    if ( sInitPlan != NULL )
    {
        sType = sInitPlan->mResultCursorProperty.mSensitivity;
    }
    else
    {
        sType = ELL_CURSOR_SENSITIVITY_NA;
    }

    return sType;
}

/**
 * @brief SQL Statement 의 Scrollability 를 획득한다.
 * @param[in]  aSQLStmt   SQL Statement
 * @remarks
 */
ellCursorScrollability  qllGetCursorScrollability( qllStatement * aSQLStmt )
{
    ellCursorScrollability sType = ELL_CURSOR_SCROLLABILITY_NA;
    
    qlvInitPlan * sInitPlan = NULL;

    sInitPlan = qlvGetQueryInitPlan( aSQLStmt );
    
    if ( sInitPlan != NULL )
    {
        sType = sInitPlan->mResultCursorProperty.mScrollability;
    }
    else
    {
        sType = ELL_CURSOR_SCROLLABILITY_NA;
    }

    return sType;
}

/**
 * @brief SQL Statement 의 Holdability 를 획득한다.
 * @param[in]  aSQLStmt   SQL Statement
 * @remarks
 */
ellCursorHoldability    qllGetCursorHoldability( qllStatement * aSQLStmt )
{
    ellCursorHoldability sType = ELL_CURSOR_HOLDABILITY_NA;
    
    qlvInitPlan * sInitPlan = NULL;

    sInitPlan = qlvGetQueryInitPlan( aSQLStmt );
    
    if ( sInitPlan != NULL )
    {
        sType = sInitPlan->mResultCursorProperty.mHoldability;
    }
    else
    {
        sType = ELL_CURSOR_HOLDABILITY_NA;
    }

    return sType;
}

/**
 * @brief SQL Statement 의 Updatability 를 획득한다.
 * @param[in]  aSQLStmt   SQL Statement
 * @remarks
 */
ellCursorUpdatability   qllGetCursorUpdatability( qllStatement * aSQLStmt )
{
    ellCursorUpdatability sType = ELL_CURSOR_UPDATABILITY_NA;
    
    qlvInitPlan * sInitPlan = NULL;

    sInitPlan = qlvGetQueryInitPlan( aSQLStmt );
    
    if ( sInitPlan != NULL )
    {
        sType = sInitPlan->mResultCursorProperty.mUpdatability;
    }
    else
    {
        sType = ELL_CURSOR_UPDATABILITY_NA;
    }

    return sType;
}

/**
 * @brief SQL Statement 의 Returnability 를 획득한다.
 * @param[in]  aSQLStmt   SQL Statement
 * @remarks
 */
ellCursorReturnability  qllGetCursorReturnability( qllStatement * aSQLStmt )
{
    ellCursorReturnability sType = ELL_CURSOR_RETURNABILITY_NA;
    
    qlvInitPlan * sInitPlan = NULL;

    sInitPlan = qlvGetQueryInitPlan( aSQLStmt );
    
    if ( sInitPlan != NULL )
    {
        sType = sInitPlan->mResultCursorProperty.mReturnability;
    }
    else
    {
        sType = ELL_CURSOR_RETURNABILITY_NA;
    }

    return sType;
}


/****************************************************************
 * Cursor 제어 함수 
 ****************************************************************/

/**
 * @brief SQL Statement 로부터 Result Set Desciptor 를 얻는다.
 * @param[in]  aSQLStmt        SQL statement
 * @return qllResultSetDesc
 * @remarks
 */
qllResultSetDesc * qllGetCursorResultSetDesc( qllStatement * aSQLStmt )
{
    STL_ASSERT( aSQLStmt->mLastSuccessPhase == QLL_PHASE_EXECUTE );
    STL_ASSERT( aSQLStmt->mResultSetDesc != NULL );
    return aSQLStmt->mResultSetDesc;
}


/**
 * @brief Named Cursor 가 OPEN 상태임을 설정함
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aSQLStmt         SQL Statement
 * @param[in]  aCursorInstDesc  Named Cursor Instance Descriptor
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qllSetNamedCursorOpen( smlStatement           * aStmt,
                                 qllDBCStmt             * aDBCStmt,
                                 qllStatement           * aSQLStmt,
                                 ellCursorInstDesc      * aCursorInstDesc,
                                 qllEnv                 * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorInstDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mLastSuccessPhase == QLL_PHASE_EXECUTE, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Named Cursor 가 OPEN 상태임을 설정
     */
    
    STL_TRY( qlcrSetNamedCursorOpen( aStmt,
                                     aDBCStmt,
                                     aSQLStmt,
                                     aCursorInstDesc,
                                     aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Result Set Cursor 가 존재할 경우 Cursor 를 닫는다.
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qllCloseCursor( qllDBCStmt   * aDBCStmt,
                          qllStatement * aSQLStmt,
                          qllEnv       * aEnv )
{
    qllResultSetDesc * sResultDesc = NULL;
        
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Result Set Descriptor 가 없다면 Skip 한다.
     */
    
    sResultDesc = aSQLStmt->mResultSetDesc;

    if( sResultDesc != NULL )
    {
        STL_TRY( qlcrCloseResultSet( aSQLStmt, aEnv ) == STL_SUCCESS );
    }
    else
    {
        /**< ODBC Result Set 이 없음 */
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/****************************************************************
 * Cursor Fetch 함수 
 ****************************************************************/



/**
 * @brief Cursor Fetch Call 회수를 증가한다.
 * @param[in] aEnv Environment
 */
void qllAddCallCntCursorFetch( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    aEnv->mFetchCnt++;
    sSessEnv->mFetchCnt++;
}




/**
 * @brief Cursor Result Set Desciptor 를 이용해 Forward Only Block Fetch 를 수행한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Cursor Result Set Desc
 * @param[out] aEOF             End Of File
 * @param[in]  aEnv             Environment
 * @remarks
 * Forward Only Cursor 용 Block Read 
 */
stlStatus qllFetchCursorForwardBlock( smlTransId               aTransID,
                                      smlStatement           * aStmt,
                                      qllDBCStmt             * aDBCStmt,
                                      qllResultSetDesc       * aResultSetDesc,
                                      stlBool                * aEOF,
                                      qllEnv                 * aEnv )
{
    stlInt64 sSkipCnt = 0;
    stlInt64 sUsedCnt = 0;

    stlBool  sSelectEOF = STL_FALSE;
    stlBool  sHasData = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aEOF != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Fixed Table 정보 누적
     */

    qllAddCallCntCursorFetch( aEnv );

    /*************************************************************
     * Fetch Block
     *************************************************************/

    STL_TRY_THROW( aResultSetDesc->mIsOpen == STL_TRUE, RAMP_RESULT_SET_NOT_EXIST );
    
    if ( aResultSetDesc->mMaterialized == STL_TRUE )
    {
        /**
         * Materialized Fetch 가 수행됨
         * - SELECT .. FOR UPDATE
         * - DML RETURNING Query
         */

        if( aResultSetDesc->mInstantFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            sHasData = STL_FALSE;
        }
        else
        {
            STL_TRY( qlcrFetchBlockFromMaterialResult( aTransID,
                                                       aResultSetDesc,
                                                       & sHasData,
                                                       aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * Pipelined Fetch 를 수행할 수 있는 Query 는 SELECT 만 존재함.
         */
        
        KNL_BREAKPOINT( KNL_BREAKPOINT_QLLFETCHSQL_BEFORE_FETCH,
                        (knlEnv*)aEnv );
        
        STL_TRY( qlfFetchSelect( aTransID,
                                 aStmt,
                                 aDBCStmt,
                                 aResultSetDesc->mQueryStmt,
                                 & sSelectEOF,
                                 aEnv )
                 == STL_SUCCESS );

        sSkipCnt = KNL_GET_BLOCK_SKIP_CNT( aResultSetDesc->mTargetBlock );
        sUsedCnt = KNL_GET_BLOCK_USED_CNT( aResultSetDesc->mTargetBlock );

        if ( sUsedCnt > 0 )
        {
            sHasData = STL_TRUE;
        }
        else
        {
            STL_DASSERT( sSelectEOF == STL_TRUE );
            sHasData = STL_FALSE;
        }
    }

    /*************************************************************
     * Block Position 설정
     *************************************************************/

    if ( sHasData == STL_TRUE )
    {
        sSkipCnt = KNL_GET_BLOCK_SKIP_CNT( aResultSetDesc->mTargetBlock );
        sUsedCnt = KNL_GET_BLOCK_USED_CNT( aResultSetDesc->mTargetBlock );
    
        aResultSetDesc->mBlockStartIdx = sSkipCnt;
        aResultSetDesc->mBlockEOFIdx   = sUsedCnt;
        
        aResultSetDesc->mBlockStartPositionIdx = aResultSetDesc->mBlockEOFPositionIdx;
        aResultSetDesc->mBlockEOFPositionIdx += (sUsedCnt - sSkipCnt);

        if ( aResultSetDesc->mMaterialized == STL_TRUE )
        {
            STL_ASSERT( aResultSetDesc->mKnownLastPosition == STL_TRUE );

            /* data 가 존재하고 끝까지 읽은 경우 */
            if ( aResultSetDesc->mBlockEOFPositionIdx > aResultSetDesc->mLastPositionIdx )
            {
                *aEOF = STL_TRUE;
            }
            else
            {
                *aEOF = STL_FALSE;
            }
        }
        else
        {
            if ( sSelectEOF == STL_TRUE )
            {
                aResultSetDesc->mKnownLastPosition = STL_TRUE;
            }
            else
            {
                aResultSetDesc->mKnownLastPosition = STL_FALSE;
            }

            aResultSetDesc->mLastPositionIdx += (sUsedCnt - sSkipCnt);
            *aEOF = sSelectEOF;
        }
    }
    else
    {
        aResultSetDesc->mKnownLastPosition = STL_TRUE;
        /* aResultSetDesc->mLastPositionIdx  */
        
        aResultSetDesc->mBlockStartIdx = -1;
        aResultSetDesc->mBlockEOFIdx   = 0;
        
        aResultSetDesc->mBlockStartPositionIdx = 0;
        aResultSetDesc->mBlockEOFPositionIdx   = 1;

        *aEOF = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_RESULT_SET_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RESULT_SET_NOT_EXIST,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    /**
     * fetch 시 정상적인 에러가 발생할 수 있으므로 SQL Statement를 초기화 하면 안된다.
     */
    
    (void) qlcrCloseResultSet( aResultSetDesc->mQueryStmt, aEnv );

    return STL_FAILURE;
}


/**
 * @brief Cursor Result Set Desciptor 를 이용해 Scroll Block Fetch 를 수행한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Cursor Result Set Desc
 * @param[in]  aStartPosition   Start Position ( 음수일 경우는 Result Set 의 END 로부터의 위치 )
 * @param[out] aEOF             End Of File
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qllFetchCursorScrollBlock( smlTransId               aTransID,
                                     smlStatement           * aStmt,
                                     qllDBCStmt             * aDBCStmt,
                                     qllResultSetDesc       * aResultSetDesc,
                                     stlInt64                 aStartPosition,
                                     stlBool                * aEOF,
                                     qllEnv                 * aEnv )
{
    STL_TRY( qlcrFetchCursorMaterializedBlock( aTransID,
                                               aStmt,
                                               aDBCStmt,
                                               aResultSetDesc,
                                               aStartPosition,
                                               aEOF,
                                               aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Named Cursor 의 Result Set Desc를 이용해 하나의 Row 를 읽는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Result Set Descriptor
 * @param[in]  aFetchOrient     Fetch Orientation
 * @param[in]  aFetchPosition   Fetch Position
 * @param[out] aRowStatus       Cursor Row 의 상태 변경 여부 
 * @param[out] aValidBlockIdx   Target Block 에서의 현재 Row 위치 (결과가 없는 경우 음수임)
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qllFetchCursorOneRow( smlTransId               aTransID,
                                smlStatement           * aStmt,
                                qllDBCStmt             * aDBCStmt,
                                qllResultSetDesc       * aResultSetDesc,
                                qllFetchOrientation      aFetchOrient,
                                stlInt64                 aFetchPosition,
                                qllCursorRowStatus     * aRowStatus,
                                stlInt32               * aValidBlockIdx,
                                qllEnv                 * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aFetchOrient > QLL_FETCH_NA) &&
                        (aFetchOrient < QLL_FETCH_MAX),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowStatus != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidBlockIdx != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * FETCH Orientation 에 따른 구문 수행 
     */

    STL_TRY( gFetchInternalFunc[aFetchOrient] ( aTransID,
                                                aStmt,
                                                aDBCStmt,
                                                aResultSetDesc,
                                                aFetchPosition,
                                                aRowStatus,
                                                aValidBlockIdx,
                                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/****************************************************************
 * Cursor 생성, 변경 함수 
 ****************************************************************/


/**
 * @brief Named CURSOR 를 선언한다.
 * @param[in]  aCursorName     Cursor Name
 * @param[out] aInstDesc       Cursor Instance Descriptor
 * @param[in]  aEnv            Environment
 */
stlStatus qllDeclareNamedCursor( stlChar                  * aCursorName,
                                 ellCursorInstDesc       ** aInstDesc,
                                 qllEnv                   * aEnv )
{
    ellCursorDeclDesc * sDeclDesc = NULL;
    ellCursorInstDesc * sInstDesc = NULL;

    stlInt32   sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCursorName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstDesc != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 동일한 커서가 존재하는 지 검사
     */
    
    STL_TRY( ellFindCursorDeclDesc( aCursorName,
                                    ELL_DICT_OBJECT_ID_NA,
                                    & sDeclDesc,
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );
        
    STL_TRY_THROW( sDeclDesc == NULL, RAMP_ERR_SAME_CURSOR_EXIST );

    /**
     * Cursor Declaration Descriptor 를 추가
     * - DBC Named Cursor 는 별도의 Cursor DBCStmt 와 SQLStmt 가 존재하지 않는다.
     */

    STL_TRY( ellInsertDBCCursorDeclDesc( aCursorName,
                                         & sDeclDesc,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;
    STL_ASSERT( sDeclDesc != NULL );

    /**
     * Cursor Instance Descriptor 를 추가
     */

    STL_TRY( ellInsertCursorInstDesc( sDeclDesc,
                                      0, /* aCursorDBCStmtSize */
                                      0, /* aCursorSQLStmtSize */
                                      & sInstDesc,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;

    /**
     * Output 설정
     */

    *aInstDesc = sInstDesc;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SAME_CURSOR_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_ALREADY_DECLARED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aCursorName );
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





                            

/**
 * @brief Cursor 이름을 변경한다.
 * @param[in] aOrgName            Original Name
 * @param[in] aNewName            New Name
 * @param[out] aNewInstDesc       New Cursor Instance Descriptor
 * @param[in] aEnv                Environment
 * @remarks
 */
stlStatus qllRenameCursor( stlChar            * aOrgName,
                           stlChar            * aNewName,
                           ellCursorInstDesc ** aNewInstDesc,
                           qllEnv             * aEnv )
{
    stlInt32    sState = 0;
    
    ellCursorInstDesc  * sOrgInstDesc = NULL;
    ellCursorDeclDesc  * sOrgDeclDesc = NULL;

    ellCursorDeclDesc  * sNewDeclDesc = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aOrgName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNewName != NULL, QLL_ERROR_STACK(aEnv) );

    /********************************************
     * Validation
     ********************************************/
    
    /**
     * New Cursor Name 존재 여부 확인
     */

    STL_TRY( ellFindCursorInstDesc( aNewName,
                                    ELL_DICT_OBJECT_ID_NA,
                                    aNewInstDesc,
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( *aNewInstDesc == NULL, RAMP_ERR_NEW_CURSOR_EXIST );

    /**
     * Org Cursor Name 존재 여부 확인
     */
    
    STL_TRY( ellFindCursorInstDesc( aOrgName,
                                    ELL_DICT_OBJECT_ID_NA,
                                    & sOrgInstDesc,
                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sOrgInstDesc != NULL, RAMP_ERR_CURSOR_NOT_EXIST );
    sOrgDeclDesc = sOrgInstDesc->mDeclDesc;

    /**
     * ODBC Named Cursor 인지 확인 
     */
    
    STL_TRY_THROW( ellCursorHasOrigin( sOrgInstDesc ) == STL_FALSE,
                   RAMP_ERR_CURSOR_IS_NOT_ODBC_CURSOR );
    
    /**
     * Cursor CLOSED 여부 확인
     */
    
    STL_TRY_THROW( ellCursorIsOpen( sOrgInstDesc ) == STL_FALSE,
                   RAMP_ERR_CURSOR_ALREADY_OPENED );

    
    
    /********************************************
     * Original Cursor 정보를 이용하여 New CURSOR 등록 
     ********************************************/

    /**
     * Cursor Declare Descriptor 를 추가
     * - DBC Named Cursor 는 별도의 Cursor DBCStmt 와 SQLStmt 가 존재하지 않는다.
     */
    
    STL_TRY( ellInsertDBCCursorDeclDesc( aNewName,
                                         & sNewDeclDesc,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sNewDeclDesc != NULL );
    sState = 1;

    STL_TRY( ellInsertCursorInstDesc( sNewDeclDesc,
                                      0, /* aCursorDBCStmtSize */
                                      0, /* aCursorSQLStmtSize */
                                      aNewInstDesc,
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 2;
    
    /********************************************
     * Original CURSOR 를 제거 
     ********************************************/

    /**
     * Cursor Instance Descriptor 를 제거
     */

    STL_ASSERT( ellRemoveCursorInstDesc( sOrgInstDesc, ELL_ENV(aEnv) )
                == STL_SUCCESS );

    /**
     * Cursor Declare Descriptor 를 제거
     */

    STL_ASSERT( ellRemoveCursorDeclDesc( sOrgDeclDesc, ELL_ENV(aEnv) )
                == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CURSOR_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_NOT_EXIST,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aOrgName );
    }

    STL_CATCH( RAMP_ERR_NEW_CURSOR_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_ALREADY_DECLARED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aNewName );
    }

    STL_CATCH( RAMP_ERR_CURSOR_IS_NOT_ODBC_CURSOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_NOT_ODBC_CURSOR,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aOrgName );
    }
    
    STL_CATCH( RAMP_ERR_CURSOR_ALREADY_OPENED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_ALREADY_OPEN,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      aOrgName );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            /**
             * 반드시 성공해야 함
             */
            STL_ASSERT(0);
        case 1:
            (void) ellRemoveCursorDeclDesc( sNewDeclDesc, ELL_ENV(aEnv) );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Cursor 를 제거한다.
 * @param[in] aDeclDesc       Cursor Declaration Descriptor
 * @param[in] aInstDesc       Cursor Instance Descriptor
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus qllDeleteCursor( ellCursorDeclDesc * aDeclDesc,
                           ellCursorInstDesc * aInstDesc,
                           qllEnv            * aEnv )
{
    qllDBCStmt   * sCursorDBCStmt = NULL;
    qllStatement * sCursorSQLStmt = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDeclDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstDesc != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * SQL Cursor 인 경우 SQL Statement 와 DBC Statement 를 종료한다.
     * @todo PSM 등이 존재하지 않기 때문에 현재는 SQL Cursor 를 제거하는 경우가 없다.
     */

    if( ellCursorHasOrigin( aInstDesc ) == STL_TRUE )
    {
        /**
         * SQL cursor 임
         */

        sCursorDBCStmt = (qllDBCStmt *)   aInstDesc->mCursorDBCStmt;
        sCursorSQLStmt = (qllStatement *) aInstDesc->mCursorSQLStmt;
        
        STL_TRY( qllFiniSQL( sCursorDBCStmt, sCursorSQLStmt, aEnv ) == STL_SUCCESS );
        STL_TRY( qllFiniDBC( sCursorDBCStmt, aEnv ) == STL_SUCCESS );
    }
    else
    {
        /**
         * ODBC/JDBC named cursor 임
         */
    }
    
    /**
     * Cursor Instance Descriptor 를 제거
     */
    
    STL_ASSERT( ellRemoveCursorInstDesc( aInstDesc, ELL_ENV(aEnv) )
                == STL_SUCCESS );

    /**
     * Cursor Declare Descriptor 를 제거
     */

    STL_ASSERT( ellRemoveCursorDeclDesc( aDeclDesc, ELL_ENV(aEnv) )
                == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    (void) ellRemoveCursorInstDesc( aInstDesc, ELL_ENV(aEnv) );
    (void) ellRemoveCursorDeclDesc( aDeclDesc, ELL_ENV(aEnv) );
    
    return STL_FAILURE;
}






/** @} qllCursor */
