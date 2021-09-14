/*******************************************************************************
 * qldAlterDatabaseIncompleteRecover.h
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

#ifndef _QLD_ALTER_DATABASE_INCOMPLETE_RECOVER_STMT_H_
#define _QLD_ALTER_DATABASE_INCOMPLETE_RECOVER_STMT_H_ 1

/**
 * @file qldAlterDatabaseIncompleteRecover.h
 * @brief ALTER DATABASE .. INCOMPLETE RECOVER Statement
 */

/**
 * @brief Init Plan of Alter Database INCOMPLETE RECOVER Statement
 */
/**
 * @brief ALTER DATABASE INCOMPLETE RECOVER ¸¦ À§ÇÑ Init Plan
 */
typedef struct qldInitAlterDatabaseIncompleteRecover
{
    smlRecoveryType          mRecoveryType;    /**< Recovery Type @see smlRecoveryType */
    smlImrOption             mImrOption;       /**< Incomplete Media Recovery Option */
    smlImrCondition          mImrCondition;    /**< Incomplete Media Recovery Condition */
    stlInt64                 mUntilValue;      /**< Until Value for Incomplete Media Recovery */
    stlChar                * mLogfileName;     /**< Logfile Name for Incomplete Media Recovery */
} qldInitAlterDatabaseIncompleteRecover;

stlStatus qldValidateAlterDatabaseIncompleteRecover( smlTransId      aTransID,
                                                     qllDBCStmt    * aDBCStmt,
                                                     qllStatement  * aSQLStmt,
                                                     stlChar       * aSQLString,
                                                     qllNode       * aParseTree,
                                                     qllEnv        * aEnv );

stlStatus qldOptCodeAlterDatabaseIncompleteRecover( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );

stlStatus qldOptDataAlterDatabaseIncompleteRecover( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );

stlStatus qldExecuteAlterDatabaseIncompleteRecover( smlTransId      aTransID,
                                                    smlStatement  * aStmt,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv );

#endif /* _QLD_ALTER_DATABASE_INCOMPLETE_RECOVER_STMT_H_ */


