/*******************************************************************************
 * qlssAlterSystemReset.h
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

#ifndef _QLSS_ALTER_SYSTEM_RESET_H_
#define _QLSS_ALTER_SYSTEM_RESET_H_ 1

/**
 * @file qlssAlterSystemReset.h
 * @brief System Control Language
 */

/*********************************************************************
 * ALTER SYSTEM RESET
 *********************************************************************/

/**
 * @brief ALTER SYSTEM RESET Init Plan
 * - qlssInitSystemSet 자료 구조를 공유 
 */

stlStatus qlssValidateSystemReset( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv );

stlStatus qlssOptCodeSystemReset( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlssOptDataSystemReset( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );

stlStatus qlssExecuteSystemReset( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  qllEnv        * aEnv );


#endif /* _QLSS_ALTER_SYSTEM_RESET_H_ */

