/*******************************************************************************
 * qldAlterDatabaseIrrecoverable.h
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

#ifndef _QLD_ALTER_DATABASE_IRRECOVERABLE_H_
#define _QLD_ALTER_DATABASE_IRRECOVERABLE_H_ 1

/**
 * @file qldAlterDatabaseIrrecoverable.h
 * @brief ALTER DATABASE .. IRRECOVERABLE SEGMENT Statement
 */

/**
 * @brief Init Plan of Alter Database .. IRRECOVERABLE SEGMENT Statement
 */
/**
 * @brief ALTER TABLESPACE .. IRRECOVERABLE SEGMENT 를 위한 Init Plan
 */
typedef struct qldInitAlterDatabaseIrrecoverable
{
    stlInt32    mSegmentCount;
    stlInt64  * mSegmentArray;
} qldInitAlterDatabaseIrrecoverable;

stlStatus qldValidateAlterDatabaseIrrecoverable( smlTransId      aTransID,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          stlChar       * aSQLString,
                                          qllNode       * aParseTree,
                                          qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseIrrecoverable( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseIrrecoverable( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );

stlStatus qldRecheckAlterDatabaseIrrecoverable( smlTransId      aTransID,
                                         smlStatement  * aStmt,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseIrrecoverable( smlTransId      aTransID,
                                         smlStatement  * aStmt,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_IRRECOVERABLE_STMT_H_ */


