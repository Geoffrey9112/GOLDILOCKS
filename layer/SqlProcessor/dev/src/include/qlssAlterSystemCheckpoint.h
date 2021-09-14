/*******************************************************************************
 * qlssAlterSystemCheckpoint.h
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

#ifndef _QLSS_ALTER_SYSTEM_CHECKPOINT_H_
#define _QLSS_ALTER_SYSTEM_CHECKPOINT_H_ 1

/**
 * @file qlssAlterSystemCheckpoint.h
 * @brief System Control Language
 */

/**
 * @brief ALTER SYSTEM CHECKPOINT Init Plan
 */
typedef struct qlssInitSystemCheckpoint
{
    stlInt64   mDummy;      /**< prepare/execution 의 유효성 검사를 위한 dummy plan */
} qlssInitSystemCheckpoint;


stlStatus qlssValidateSystemChkpt( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv );

stlStatus qlssOptCodeSystemChkpt( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlssOptDataSystemChkpt( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlssExecuteSystemChkpt( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );


#endif /* _QLSS_ALTER_SYSTEM_CHECKPOINT_H_ */

