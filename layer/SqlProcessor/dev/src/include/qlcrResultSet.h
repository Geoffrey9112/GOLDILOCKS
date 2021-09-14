/*******************************************************************************
 * qlcrResultSet.h
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

#ifndef _QLCR_RESULT_SET_H_
#define _QLCR_RESULT_SET_H_ 1

/**
 * @file qlcrResultSet.h
 * @brief Result Set 처리 함수 
 */


/**
 * @defgroup qlcrResultSet Result Set 처리 함수 
 * @ingroup qlcrCursor
 * @{
 */

/***************************************************************
 * 초기화 함수 
 ***************************************************************/

stlStatus qlcrValidateDMLQueryResultCursorProperty( qlvStmtInfo        * aStmtInfo,
                                                    qllEnv             * aEnv );


stlStatus qlcrDataOptInitResultSetDesc( qllStatement           * aQueryStmt,
                                        knlValueBlockList      * aTargetBlock,
                                        knlRegionMem           * aRegionMem,
                                        qllEnv                 * aEnv );

stlStatus qlcrCloseResultSet( qllStatement * aQueryStmt,
                              qllEnv       * aEnv );


/***************************************************************
 * 정보 획득 함수 
 ***************************************************************/

smlTransReadMode qlcrGetRIDFetchTRM( qllResultSetDesc * aResultSetDesc );
smlStmtReadMode  qlcrGetRIDFetchSRM( qllResultSetDesc * aResultSetDesc );


/***************************************************************
 * Sensitivity
 ***************************************************************/

stlStatus qlcrDataOptSetSensitiveResultSet( qllDBCStmt              * aDBCStmt,
                                            qllStatement            * aQueryStmt,
                                            qllResultSetDesc        * aResultSetDesc,
                                            qllEnv                  * aEnv );

void qlcrDataOptSetInsensitiveResultSet( qllResultSetDesc  * aResultSetDesc );


stlStatus qlcrDataOptBuildScanItemInfo( qllDBCStmt              * aDBCStmt,
                                        qllStatement            * aQueryStmt,
                                        qllResultSetDesc        * aResultSetDesc,
                                        qllEnv                  * aEnv );

void qlcrDataOptAddScanItem( qllResultSetDesc       * aResultSetDesc,
                             void                   * aInitBaseTable,
                             void                   * aTablePhyHandle,
                             knlValueBlockList      * aTableReadBlock,
                             smlRowBlock            * aTableRowBlock,
                             qllCursorScanItem      * aScanItem );


/***************************************************************
 * Updatability
 ***************************************************************/

stlStatus qlcrDataOptSetUpdatableResultSet( qllDBCStmt              * aDBCStmt,
                                            qllStatement            * aQueryStmt,
                                            qllResultSetDesc        * aResultSetDesc,
                                            qllEnv                  * aEnv );

void qlcrDataOptSetReadOnlyResultSet( qllResultSetDesc  * aResultSetDesc );

stlStatus qlcrDataOptBuildLockItemInfo( qllDBCStmt              * aDBCStmt,
                                        qllStatement            * aQueryStmt,
                                        qlvInitSelect           * aInitSelect,
                                        qllResultSetDesc        * aResultSetDesc,
                                        qllEnv                  * aEnv );

stlStatus qlcrDataOptAddCursorItemUsingQueryNode( qllStatement            * aQueryStmt,
                                                  qllDataArea             * aDataArea,
                                                  qllOptimizationNode     * aOptNode,
                                                  qllCursorItemType         aItemType,
                                                  qlvInitNode             * aInitTableNode,
                                                  qllResultSetDesc        * aResultSetDesc,
                                                  knlRegionMem            * aRegionMem,
                                                  qllEnv                  * aEnv );

void qlcrDataOptAddLockItem( qllResultSetDesc       * aResultSetDesc,
                             void                   * aTablePhyHandle,
                             smlRowBlock            * aTableRowBlock,
                             qllCursorLockItem      * aLockItem );


stlStatus qlcrDataOptBuildNoRetryLockItemInfo( qllDBCStmt              * aDBCStmt,
                                               qllDataArea             * aDataArea,
                                               qllResultSetDesc        * aResultSetDesc,
                                               qllEnv                  * aEnv );

/***************************************************************
 * Result Set Materialization
 ***************************************************************/

stlStatus qlcrDataOptSetMaterializedResultSet( qllDBCStmt              * aDBCStmt,
                                               qllStatement            * aQueryStmt,
                                               qllResultSetDesc        * aResultSetDesc,
                                               qllEnv                  * aEnv );

void qlcrDataOptSetNonMaterializedResultSet( qllResultSetDesc  * aResultSetDesc );

stlStatus qlcrFetchBlockFromMaterialResult( smlTransId                aTransID,
                                            qllResultSetDesc        * aResultSetDesc,
                                            stlBool                 * aHasData,
                                            qllEnv                  * aEnv );


/***************************************************************
 * 정보 획득 Macro
 ***************************************************************/

#define QLCR_NEED_TO_SET_ROWBLOCK( _aSQLStmt )                  \
    ( ( ( ((qlvInitPlan*) qllGetInitPlan( (_aSQLStmt) ))        \
          ->mResultCursorProperty.mSensitivity                  \
          == ELL_CURSOR_SENSITIVITY_SENSITIVE ) ||              \
        ( ((qlvInitPlan*) qllGetInitPlan( (_aSQLStmt) ))        \
          ->mResultCursorProperty.mUpdatability                 \
          == ELL_CURSOR_UPDATABILITY_FOR_UPDATE ) )             \
      ? STL_TRUE : STL_FALSE )


/** @} qlcrResultSet */


#endif /* _QLCR_RESULT_SET_H_ */
