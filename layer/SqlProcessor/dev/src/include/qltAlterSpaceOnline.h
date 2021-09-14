/*******************************************************************************
 * qltAlterSpaceOnline.h
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

#ifndef _QLT_ALTER_SPACE_ONLINE_H_
#define _QLT_ALTER_SPACE_ONLINE_H_ 1

/**
 * @file qltAlterSpaceOnline.h
 * @brief ALTER TABLESPACE .. ONLINE
 */

/**
 * @brief ALTER TABLESPACE ONLINE 를 위한 Init Plan
 */
typedef struct qltInitAlterSpaceOnline
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle       mSpaceHandle;      /**< Tablespace Handle */
} qltInitAlterSpaceOnline;

stlStatus qltValidateAlterSpaceOnline( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       stlChar       * aSQLString,
                                       qllNode       * aParseTree,
                                       qllEnv        * aEnv );


stlStatus qltOptCodeAlterSpaceOnline( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

stlStatus qltOptDataAlterSpaceOnline( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

stlStatus qltExecuteAlterSpaceOnline( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );


#endif /* _QLT_ALTER_SPACE_ONLINE_H_ */
