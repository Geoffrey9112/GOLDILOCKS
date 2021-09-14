/*******************************************************************************
 * qlcrCloseCursor.c
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
 * @file qlcrCloseCursor.c
 * @brief CLOSE CURSOR 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>
#include <qlcrCloseCursor.h>


/**
 * @addtogroup qlcrCloseCursor
 * @{
 */


/**
 * @brief CLOSE CURSOR 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrValidateCloseCursor( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv )
{
    qlpCloseCursor      * sParseTree = NULL;
    
    qlcrInitCloseCursor * sInitPlan = NULL;

    /**
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CLOSE_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_CLOSE_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpCloseCursor *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlcrInitCloseCursor),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlcrInitCloseCursor) );

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
     * 권한 검사 없음
     */

    
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
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief CLOSE CURSOR 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrCodeOptimizeCloseCursor( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    /**
     * Nothing To Do
     */
    
    return STL_SUCCESS;
}

/**
 * @brief CLOSE CURSOR 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrDataOptimizeCloseCursor( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    /**
     * Nothing To Do
     */
    
    return STL_SUCCESS;
}




/**
 * @brief CLOSE CURSOR 구문을 Execute 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlcrExecuteFuncCloseCursor( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv )
{
    qlcrInitCloseCursor * sInitPlan = NULL;

    qllStatement      * sCursorSQLStmt = NULL;

    /**
     * Parameter Validation
     */

    /* CLOSE 구문은 SM statement 를 내부에서 할당하여 사용 */
    STL_PARAM_VALIDATE( aStmt == NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CLOSE_CURSOR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sInitPlan = (qlcrInitCloseCursor *) aSQLStmt->mInitPlan;

    /**********************************************************
     * Cursor 가 이미 OPEN 되어 있는지 검사 
     **********************************************************/

    /**
     * Cursor CLOSED 여부 확인
     */
    
    STL_TRY_THROW( ellCursorIsOpen( sInitPlan->mInstDesc ) == STL_TRUE,
                   RAMP_ERR_CURSOR_IS_NOT_OPEN );

    /**
     * Cursor 종료
     */

    sCursorSQLStmt = (qllStatement *) sInitPlan->mInstDesc->mCursorSQLStmt;
    
    STL_TRY( qlcrCloseResultSet( sCursorSQLStmt, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_CURSOR_IS_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CURSOR_IS_NOT_OPEN,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sInitPlan->mInstDesc->mCursorKey.mCursorName );
    }
    
    
    STL_FINISH;

    return STL_FAILURE;
}
    






/**
 * @brief Transaction 종료시 Open 된 Not Holdable Cursor 를 닫는다.
 * @param[in]   aEnv              Environment
 * @remarks 
 */
stlStatus qlcrCloseAllOpenNotHoldableCursor4TransEnd( qllEnv * aEnv )
{
    return qlcrCloseAllOpenNotHoldableCursor4RollbackSavepoint( 0, aEnv );
}


/**
 * @brief Savepoint 이후에 Open 된 Not Holdable Cursor 를 닫는다.
 * @param[in]  aSavepointTimestamp   Savepoint Timestamp
 * @param[in]  aEnv                  Environment
 * @todo
 * 현재는 Cursor 를 가지는 Result Set 만을 닫고 있다.
 * 향후 QP Session 에서 QP Statement 목록을 관리하게 되면,
 * Unnamed cursor(cursor 와 연결되지 않은 result set) 도 닫을 수 있도록 하자
 * @remarks 
 */
stlStatus qlcrCloseAllOpenNotHoldableCursor4RollbackSavepoint( stlInt64     aSavepointTimestamp,
                                                               qllEnv     * aEnv )
{
    ellCursorInstDesc      * sInstDesc = NULL;

    qllStatement           * sCursorSQLStmt = NULL;
    qllResultSetDesc       * sResultSetDesc = NULL;

    stlRingHead  * sHeadPtr = NULL;
    stlRingEntry * sDataPtr = NULL;
    stlRingEntry * sNextPtr = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSavepointTimestamp >= 0, QLL_ERROR_STACK(aEnv) );

    /**
     * Open Cursor 목록을 순회하며 WITHOUT HOLD cursor 를 닫는다.
     */
    
    sHeadPtr = ellGetCursorOpenList( ELL_ENV(aEnv) );

    STL_RING_FOREACH_ENTRY_SAFE( sHeadPtr, sDataPtr, sNextPtr )
    {
        sInstDesc = (ellCursorInstDesc *) sDataPtr;

        sCursorSQLStmt = (qllStatement *)     sInstDesc->mCursorSQLStmt;
        sResultSetDesc = (qllResultSetDesc *) sCursorSQLStmt->mResultSetDesc;
        
        if ( sResultSetDesc->mCursorProperty.mHoldability
             == ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD )
        {
            if ( sResultSetDesc->mStmtTimestamp >= aSavepointTimestamp )
            {
                /**
                 * Savepoint 이후에 열린 커서임
                 * - sDataPtr 의 link 도 함께 제거함.
                 */
                
                STL_TRY( qlcrCloseResultSet( sCursorSQLStmt, aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * Savepoint 이전에 열린 커서임
                 */
            }
        }
        else
        {
            /**
             * WITH HOLD 커서로 닫지 않는다.
             */
        }
    }
                                                     
    return STL_SUCCESS;

    STL_FINISH;

    STL_RING_FOREACH_ENTRY_SAFE( sHeadPtr, sDataPtr, sNextPtr )
    {
        sNextPtr = STL_RING_GET_NEXT_DATA(sHeadPtr, sDataPtr);
        
        sInstDesc = (ellCursorInstDesc *) sDataPtr;

        sCursorSQLStmt = (qllStatement *)     sInstDesc->mCursorSQLStmt;
        sResultSetDesc = (qllResultSetDesc *) sCursorSQLStmt->mResultSetDesc;
        
        if ( sResultSetDesc->mCursorProperty.mHoldability
             == ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD )
        {
            if ( sResultSetDesc->mStmtTimestamp >= aSavepointTimestamp )
            {
                /**
                 * Savepoint 이후에 열린 커서임
                 */
                (void) qlcrCloseResultSet( sCursorSQLStmt, aEnv );
            }
            else
            {
                /**
                 * Savepoint 이전에 열린 커서임
                 */
            }
        }
        else
        {
            /**
             * WITH HOLD 커서로 닫지 않는다.
             */
        }

        sDataPtr = sNextPtr;
    }
    
    return STL_FAILURE;
}


/**
 * @brief SQL-Cursor 를 모두 정리한다.
 * @param[in]  aEnv Environment
 * @remarks
 * SQL-Cursor 만을 정리한다.
 */
stlStatus qlcrFiniSQLCursor( qllEnv * aEnv )
{
    ellCursorInstDesc      * sInstDesc = NULL;
    ellCursorInstDesc      * sCurrDesc = NULL;

    qllStatement * sCursorSQLStmt = NULL;

    stlUInt32   sBucketSeq = 0;
    
    /**
     * SQL-Cursor 목록을 순회하며 모든 cursor 를 닫는다.
     */

    STL_TRY( ellGetFirstSQLCursor( & sBucketSeq,
                                   & sInstDesc,
                                   ELL_ENV(aEnv) )
             == STL_SUCCESS );

    while ( sInstDesc != NULL )
    {
        sCursorSQLStmt = (qllStatement *) sInstDesc->mCursorSQLStmt;
        
        if ( ellCursorIsOpen( sInstDesc ) == STL_TRUE )
        {
            /**
             * SQL-Cursor 를 닫는다
             */

            STL_TRY( qlcrCloseResultSet( sCursorSQLStmt, aEnv ) == STL_SUCCESS );
        }

        /**
         * SQL-Cursor 를 위한 DBC, SQL 자원을 정리한다.
         */

        /* Session 정상 종료시 ODBC/JDBC named cursor 가 존재해서는 안된다 */
        STL_DASSERT( ellCursorHasOrigin( sInstDesc ) == STL_TRUE );

        /**
         * 다음 SQL-Cursor 를 얻는다
         */
        
        sCurrDesc = sInstDesc;
        
        STL_TRY( ellGetNextSQLCursor( & sBucketSeq,
                                      & sInstDesc,
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * 현재 SQL-Cursor 를 제거한다.
         */

        STL_TRY( qllDeleteCursor( sCurrDesc->mDeclDesc, sCurrDesc, aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT(0);
    
    return STL_FAILURE;
}

/** @} qlcrCloseCursor */
