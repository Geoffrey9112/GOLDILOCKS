/*******************************************************************************
 * qldAlterDatabaseIrrecoverable.c
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
 * @file qldAlterDatabaseIrrecoverable.c
 * @brief ALTER DATABASE ... IRRECOVERABLE SEGMENT 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qldAlterDatabaseIrrecoverable ALTER DATABASE ... IRRECOVERABLE SEGMENT
 * @ingroup qldAlterDatabase
 * @{
 */


/**
 * @brief ALTER DATABASE ... IRRECOVERABLE SEGMENT 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseIrrecoverable( smlTransId      aTransID,
                                                 qllDBCStmt    * aDBCStmt,
                                                 qllStatement  * aSQLStmt,
                                                 stlChar       * aSQLString,
                                                 qllNode       * aParseTree,
                                                 qllEnv        * aEnv )
{
    qlpAlterDatabaseIrrecoverable     * sParseTree = NULL;
    qldInitAlterDatabaseIrrecoverable * sInitPlan = NULL;
    qlpValue                          * sSegment = NULL;
    stlInt64                          * sSegmentArray = NULL;
    stlInt32                            sSegmentCount = 0;
    stlInt32                            sSegmentIdx = 0;
    qlpListElement                    * sListElement = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_IRRECOVERABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_IRRECOVERABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    sParseTree = (qlpAlterDatabaseIrrecoverable *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitAlterDatabaseIrrecoverable),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitAlterDatabaseIrrecoverable) );

    sSegmentCount = QLP_LIST_GET_COUNT( sParseTree->mSegmentList );

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                sSegmentCount * STL_SIZEOF(stlInt64),
                                (void **) & sSegmentArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sSegmentArray, 0x00, sSegmentCount * STL_SIZEOF(stlInt64) );
    
    QLP_LIST_FOR_EACH( sParseTree->mSegmentList, sListElement )
    {
        sSegment = QLP_LIST_GET_POINTER_VALUE( sListElement );
        
        STL_TRY( dtlGetIntegerFromString(
                     QLP_GET_PTR_VALUE(sSegment),
                     stlStrlen( QLP_GET_PTR_VALUE(sSegment) ),
                     & sSegmentArray[sSegmentIdx],
                     QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        STL_TRY_THROW( smlIsValidSegmentId( sSegmentArray[sSegmentIdx] )
                       == STL_TRUE, RAMP_ERR_INVALID_SEGMENT_ID );

        sSegmentIdx++;
    }
        
    sInitPlan->mSegmentCount = sSegmentIdx;
    sInitPlan->mSegmentArray = sSegmentArray;

    /**
     * ALTER DATABASE ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_DATABASE,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_SEGMENT_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_SEGMENT_ID,
                      qlgMakeErrPosString( aSQLString,
                                           sSegment->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE ... IRRECOVERABLE SEGMENT 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseIrrecoverable( smlTransId      aTransID,
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
 * @brief ALTER DATABASE ... IRRECOVERABLE SEGMENT 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseIrrecoverable( smlTransId      aTransID,
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
 * @brief ALTER DATABASE ... IRRECOVERABLE SEGMENT 구문의 Privilege 를 재검사한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aStmt      Statement
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldRecheckAlterDatabaseIrrecoverable( smlTransId      aTransID,
                                                smlStatement  * aStmt,
                                                qllDBCStmt    * aDBCStmt,
                                                qllStatement  * aSQLStmt,
                                                qllEnv        * aEnv )
{
    /**
     * @todo ALTER DATABASE 권한을 검사해야 함.
     */
    
    return STL_SUCCESS;
}



/**
 * @brief ALTER DATABASE ... IRRECOVERABLE SEGMENT 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseIrrecoverable( smlTransId      aTransID,
                                                smlStatement  * aStmt,
                                                qllDBCStmt    * aDBCStmt,
                                                qllStatement  * aSQLStmt,
                                                qllEnv        * aEnv )
{
    qldInitAlterDatabaseIrrecoverable * sInitPlan = NULL;
    stlInt32                            i;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_IRRECOVERABLE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qldInitAlterDatabaseIrrecoverable *) aSQLStmt->mInitPlan;
    
    for( i = 0; i < sInitPlan->mSegmentCount; i++ )
    {
        STL_TRY( smlAddIrrecoverableSegment( sInitPlan->mSegmentArray[i],
                                             SML_ENV(aEnv) )
                 == STL_SUCCESS);
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qldAlterDatabaseIrrecoverable */
