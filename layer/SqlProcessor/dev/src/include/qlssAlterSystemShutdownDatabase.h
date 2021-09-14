/*******************************************************************************
 * qlssAlterSystemShutdownDatabase.h
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

#ifndef _QLSS_ALTER_SYSTEM_SHUTDOWN_DATABASE_H_
#define _QLSS_ALTER_SYSTEM_SHUTDOWN_DATABASE_H_ 1

/**
 * @file qlssAlterSystemShutdownDatabase.h
 * @brief System Control Language
 */


/**
 * @brief ALTER SYSTEM CLOSE DATABASE Init Plan
 */
typedef struct qlssInitSystemShutdownDatabase
{
    knlShutdownPhase  mShutdownPhase;    /**< MOUNT/OPEN */
    knlShutdownMode   mCloseOption;      /**< NORMAL/TRANSACTIONAL/IMMEDIATE/ABORT */
} qlssInitSystemShutdownDatabase;



stlStatus qlssValidateSystemShutdownDatabase( smlTransId      aTransID,
                                              qllDBCStmt    * aDBCStmt,
                                              qllStatement  * aSQLStmt,
                                              stlChar       * aSQLString,
                                              qllNode       * aParseTree,
                                              qllEnv        * aEnv );

stlStatus qlssOptCodeSystemShutdownDatabase( smlTransId      aTransID,
                                             qllDBCStmt    * aDBCStmt,
                                             qllStatement  * aSQLStmt,
                                             qllEnv        * aEnv );

stlStatus qlssOptDataSystemShutdownDatabase( smlTransId      aTransID,
                                             qllDBCStmt    * aDBCStmt,
                                             qllStatement  * aSQLStmt,
                                             qllEnv        * aEnv );

stlStatus qlssExecuteSystemShutdownDatabase( smlTransId      aTransID,
                                             smlStatement  * aStmt,
                                             qllDBCStmt    * aDBCStmt,
                                             qllStatement  * aSQLStmt,
                                             qllEnv        * aEnv );

stlStatus qlssWaitAllUserSession( qllEnv * aEnv );
stlStatus qlssCloseAllUserSession( qllEnv * aEnv );
stlStatus qlssWaitAllActiveStatement( qllEnv * aEnv );

#endif /* _QLSS_ALTER_SYSTEM_SHUTDOWN_DATABASE_H_ */

