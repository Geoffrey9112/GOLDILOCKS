/*******************************************************************************
 * qlxInsertReturnQuery.c
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
 * @file qlxInsertReturnQuery.c
 * @brief SQL Processor Layer : Execution of INSERT RETURN INTO statement
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlxInsertReturnQuery
 * @{
 */


/**
 * @brief INSERT RETURN Query 구문을 실행한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement 객체
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlxExecuteInsertReturnQuery( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    qllResultSetDesc       * sResultSetDesc = NULL;
    qlnInstantTable        * sInstantTable = NULL;
    smlFetchInfo           * sFetchInfo = NULL;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_QUERY_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mCodePlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mDataPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**************************************************
     * INSERT RETURN 구문 수행 
     **************************************************/
    
    /**
     * 기본 INSERT RETURN Execution 
     */
    

    STL_TRY( qlxExecuteInsertReturnInto( aTransID,
                                         aStmt,
                                         aDBCStmt,
                                         aSQLStmt,
                                         aEnv )
             == STL_SUCCESS );

    /**************************************************
     * Fetch 를 위한 Iterator 할당 
     **************************************************/

    sResultSetDesc = aSQLStmt->mResultSetDesc;
    sInstantTable  = sResultSetDesc->mInstantTable;
    
    /**
     * Instant Table 에 대한 Iterator 할당
     */

    STL_TRY( smlAllocIterator( sInstantTable->mStmt,
                               sInstantTable->mTableHandle,
                               SML_TRM_SNAPSHOT,
                               SML_SRM_SNAPSHOT,
                               & sInstantTable->mIteratorProperty,
                               SML_SCAN_FORWARD,
                               & sInstantTable->mIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
    

    /**
     * Instant Table 에 대한 Iterator Fetch 정보 초기화
     */

    sFetchInfo = & sResultSetDesc->mInstantFetchInfo;

    sFetchInfo->mSkipCnt     = 0;
    sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;
    sFetchInfo->mIsEndOfScan = STL_FALSE;


    /********************************************
     * Result Set Fetch 정보 설정
     ********************************************/

    /**
     * Result Set 시작을 설정
     */
    
    sResultSetDesc->mIsOpen = STL_TRUE;
    
    /**
     * Result Set 이 모두 Materialzed 됨
     */

    sResultSetDesc->mKnownLastPosition = STL_TRUE;
    sResultSetDesc->mLastPositionIdx = sInstantTable->mRecCnt;

    /**
     * Result Set 에 대한 Fetch 가 수행되기 전임 
     */
    
    sResultSetDesc->mBlockStartIdx = -1;
    sResultSetDesc->mBlockEOFIdx   = 0;
        
    sResultSetDesc->mBlockStartPositionIdx = 0;
    sResultSetDesc->mBlockEOFPositionIdx   = 1;

    return STL_SUCCESS;

    STL_FINISH;

    if ( sInstantTable != NULL )
    {
        (void) qlnxDropInstantTable( sInstantTable,
                                     aEnv );
    }
        
    return STL_FAILURE;
}



/** @} qlxInsertReturnQuery */


