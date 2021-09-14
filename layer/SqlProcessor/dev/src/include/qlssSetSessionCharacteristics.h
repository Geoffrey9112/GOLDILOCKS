/*******************************************************************************
 * qlssSetSessionCharacteristics.h
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

#ifndef _QLSS_SET_SESSION_CHARACTERISTICS_H_
#define _QLSS_SET_SESSION_CHARACTERISTICS_H_ 1

/**
 * @file qlssSetSessionCharacteristics.h
 * @brief Session Control Language
 */

/**
 * @brief SET SESSION CHARACTERISTICS Init Plan
 * - qlssInitSetTransactionMode 자료 구조를 공유
 */


stlStatus qlssValidateSetSessionCharacteristics( smlTransId      aTransID,
                                                 qllDBCStmt    * aDBCStmt,
                                                 qllStatement  * aSQLStmt,
                                                 stlChar       * aSQLString,
                                                 qllNode       * aParseTree,
                                                 qllEnv        * aEnv );

stlStatus qlssOptCodeSetSessionCharacteristics( smlTransId      aTransID,
                                                qllDBCStmt    * aDBCStmt,
                                                qllStatement  * aSQLStmt,
                                                qllEnv        * aEnv );

stlStatus qlssOptDataSetSessionCharacteristics( smlTransId      aTransID,
                                                qllDBCStmt    * aDBCStmt,
                                                qllStatement  * aSQLStmt,
                                                qllEnv        * aEnv );

stlStatus qlssExecuteSetSessionCharacteristics( smlTransId      aTransID,
                                                smlStatement  * aStmt,
                                                qllDBCStmt    * aDBCStmt,
                                                qllStatement  * aSQLStmt,
                                                qllEnv        * aEnv );


#endif /* _QLSS_SET_SESSION_CHARACTERISTICS_H_ */

