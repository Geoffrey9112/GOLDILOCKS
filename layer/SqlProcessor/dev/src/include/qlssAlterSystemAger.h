/*******************************************************************************
 * qlssAlterSystemAger.h
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

#ifndef _QLSS_ALTER_SYSTEM_AGER_H_
#define _QLSS_ALTER_SYSTEM_AGER_H_ 1

/**
 * @file qlssAlterSystemAger.h
 * @brief System Control Language
 */


/**
 * @brief ALTER SYSTEM AGER Init Plan
 */
typedef struct qlssInitSystemAger
{
    qlpAgerBehavior   mBehavior;            /**< START/STOP/LOOPBACK */
} qlssInitSystemAger;



stlStatus qlssValidateSystemAger( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qlssOptCodeSystemAger( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlssOptDataSystemAger( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlssExecuteSystemAger( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

#endif /* _QLSS_ALTER_SYSTEM_AGER_H_ */

