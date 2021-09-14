/*******************************************************************************
 * qltAlterSpaceOffline.h
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

#ifndef _QLT_ALTER_SPACE_OFFLINE_H_
#define _QLT_ALTER_SPACE_OFFLINE_H_ 1

/**
 * @file qltAlterSpaceOffline.h
 * @brief ALTER TABLESPACE .. OFFLINE
 */

/**
 * @brief ALTER TABLESPACE OFFLINE 를 위한 Init Plan
 */
typedef struct qltInitAlterSpaceOffline
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle       mSpaceHandle;      /**< Tablespace Handle */
    smlOfflineBehavior  mOfflineBehavior;  /**< offline behavior */
} qltInitAlterSpaceOffline;

stlStatus qltValidateAlterSpaceOffline( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv );


stlStatus qltOptCodeAlterSpaceOffline( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qltOptDataAlterSpaceOffline( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qltExecuteAlterSpaceOffline( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );


stlStatus qltLock4AlterSpaceOnOffline( smlTransId       aTransId,
                                       smlStatement   * aStmt,
                                       qllDBCStmt     * aDBCStmt,
                                       ellDictHandle  * aSpaceHandle,
                                       stlBool        * aLockSuccess,
                                       qllEnv         * aEnv );

#endif /* _QLT_ALTER_SPACE_OFFLINE_H_ */
