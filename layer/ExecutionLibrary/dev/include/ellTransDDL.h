/*******************************************************************************
 * ellTransDDL.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ellTransDDL.h 7252 2013-02-13 08:29:17Z leekmo $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ELL_TRANS_DDL_H_
#define _ELL_TRANS_DDL_H_ 1

/**
 * @file ellTransDDL.h
 * @brief Execution Layer 의 DDL Transaction 관리 공통 모듈 
 */

/**
 * @defgroup ellTransDDL DDL Transcation
 * @ingroup ellSqlObject
 * @{
 */

stlStatus ellBeginStmtDDL( smlStatement * aStmt,
                           stlTime        aTimeDDL,
                           ellEnv       * aEnv );
stlStatus ellEndStmtDDL( smlStatement * aStmt,
                         ellEnv       * aEnv );

stlStatus ellRollbackStmtDDL( smlTransId     aTransID,
                              ellEnv       * aEnv );

stlStatus ellPreActionCommitDDL( smlTransId       aTransID,
                                 ellEnv         * aEnv );

stlStatus ellPreActionRollbackDDL( smlTransId       aTransID,
                                   ellEnv         * aEnv );

stlStatus ellCommitTransDDL( smlTransId aTransID,
                             ellEnv   * aEnv );

stlStatus ellRollbackTransDDL( smlTransId aTransID,
                               ellEnv   * aEnv );

stlStatus ellRollbackToSavepointDDL( smlTransId aTransID,
                                     stlInt64   aSavepointTimestamp,
                                     ellEnv   * aEnv );

stlStatus ellCleanupHandoverSession( ellSessionEnv * aDstSessionEnv,
                                     ellSessionEnv * aSrcSessionEnv );

stlStatus ellCleanupPriorRollbackTransDDL( smlTransId     aTransID,
                                           ellEnv       * aEnv );

stlStatus ellCleanupAfterRollbackTransDDL( smlTransId     aTransID,
                                           ellEnv       * aEnv );


/** @} ellTransDDL */



#endif /* _ELL_TRANS_DDL_H_ */
