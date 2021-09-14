/*******************************************************************************
 * qlcrFetchInternal.h
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

#ifndef _QLCR_FETCH_INTERNAL_H_
#define _QLCR_FETCH_INTERNAL_H_ 1

/**
 * @file qlcrFetchInternal.h
 * @brief CURSOR FETCH 처리 함수 
 */

/**
 * @defgroup qlcrFetchInternal Fetch Internal
 * @ingroup qlcrCursor
 * @{
 */


typedef stlStatus (* qlcrFetchFunc) ( smlTransId               aTransID,
                                      smlStatement           * aStmt,
                                      qllDBCStmt             * aDBCStmt,
                                      qllResultSetDesc       * aResultSetDesc,
                                      stlInt64                 aFetchPosition,
                                      qllCursorRowStatus     * aRowStatus,
                                      stlInt32               * aValidBlockIdx,
                                      qllEnv                 * aEnv );

extern qlcrFetchFunc gFetchInternalFunc[QLL_FETCH_MAX];

stlStatus qlcrCursorInternalFetchNA( smlTransId               aTransID,
                                     smlStatement           * aStmt,
                                     qllDBCStmt             * aDBCStmt,
                                     qllResultSetDesc       * aResultSetDesc,
                                     stlInt64                 aFetchPosition,
                                     qllCursorRowStatus     * aRowStatus,
                                     stlInt32               * aValidBlockIdx,
                                     qllEnv                 * aEnv );

stlStatus qlcrCursorInternalFetchNext( smlTransId               aTransID,
                                       smlStatement           * aStmt,
                                       qllDBCStmt             * aDBCStmt,
                                       qllResultSetDesc       * aResultSetDesc,
                                       stlInt64                 aFetchPosition,
                                       qllCursorRowStatus     * aRowStatus,
                                       stlInt32               * aValidBlockIdx,
                                       qllEnv                 * aEnv );

stlStatus qlcrCursorInternalFetchPrior( smlTransId               aTransID,
                                        smlStatement           * aStmt,
                                        qllDBCStmt             * aDBCStmt,
                                        qllResultSetDesc       * aResultSetDesc,
                                        stlInt64                 aFetchPosition,
                                        qllCursorRowStatus     * aRowStatus,
                                        stlInt32               * aValidBlockIdx,
                                        qllEnv                 * aEnv );

stlStatus qlcrCursorInternalFetchFirst( smlTransId               aTransID,
                                        smlStatement           * aStmt,
                                        qllDBCStmt             * aDBCStmt,
                                        qllResultSetDesc       * aResultSetDesc,
                                        stlInt64                 aFetchPosition,
                                        qllCursorRowStatus     * aRowStatus,
                                        stlInt32               * aValidBlockIdx,
                                        qllEnv                 * aEnv );

stlStatus qlcrCursorInternalFetchLast( smlTransId               aTransID,
                                       smlStatement           * aStmt,
                                       qllDBCStmt             * aDBCStmt,
                                       qllResultSetDesc       * aResultSetDesc,
                                       stlInt64                 aFetchPosition,
                                       qllCursorRowStatus     * aRowStatus,
                                       stlInt32               * aValidBlockIdx,
                                       qllEnv                 * aEnv );

stlStatus qlcrCursorInternalFetchCurrent( smlTransId               aTransID,
                                          smlStatement           * aStmt,
                                          qllDBCStmt             * aDBCStmt,
                                          qllResultSetDesc       * aResultSetDesc,
                                          stlInt64                 aFetchPosition,
                                          qllCursorRowStatus     * aRowStatus,
                                          stlInt32               * aValidBlockIdx,
                                          qllEnv                 * aEnv );

stlStatus qlcrCursorInternalFetchAbsolute( smlTransId               aTransID,
                                           smlStatement           * aStmt,
                                           qllDBCStmt             * aDBCStmt,
                                           qllResultSetDesc       * aResultSetDesc,
                                           stlInt64                 aFetchPosition,
                                           qllCursorRowStatus     * aRowStatus,
                                           stlInt32               * aValidBlockIdx,
                                           qllEnv                 * aEnv );

stlStatus qlcrCursorInternalFetchRelative( smlTransId               aTransID,
                                           smlStatement           * aStmt,
                                           qllDBCStmt             * aDBCStmt,
                                           qllResultSetDesc       * aResultSetDesc,
                                           stlInt64                 aFetchPosition,
                                           qllCursorRowStatus     * aRowStatus,
                                           stlInt32               * aValidBlockIdx,
                                           qllEnv                 * aEnv );


stlStatus qlcrCursorSensitiveFetchRID( smlStatement           * aStmt,
                                       qllResultSetDesc       * aResultSetDesc,
                                       qllCursorRowStatus     * aRowStatus,
                                       stlInt32                 aValidBlockIdx,
                                       qllEnv                 * aEnv );


stlStatus qlcrFetchCursorMaterializedBlock( smlTransId               aTransID,
                                            smlStatement           * aStmt,
                                            qllDBCStmt             * aDBCStmt,
                                            qllResultSetDesc       * aResultSetDesc,
                                            stlInt64                 aStartPosition,
                                            stlBool                * aEOF,
                                            qllEnv                 * aEnv );


/** @} qlcrCursor */


#endif /* _QLCR_FETCH_INTERNAL_H_ */
