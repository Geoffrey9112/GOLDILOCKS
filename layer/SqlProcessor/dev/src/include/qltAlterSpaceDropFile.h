/*******************************************************************************
 * qltAlterSpaceDropFile.h
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

#ifndef _QLT_ALTER_SPACE_DROP_FILE_H_
#define _QLT_ALTER_SPACE_DROP_FILE_H_ 1

/**
 * @file qltAlterSpaceDropFile.h
 * @brief ALTER TABLESPACE .. DROP FILE
 */

/**
 * @brief ALTER TABLESPACE ADD FILE 를 위한 Init Plan
 */
typedef struct qltInitAlterSpaceDropFile
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle   mSpaceHandle;   /**< Tablespace Handle */
    stlChar       * mFileName;      /**< File Name */
} qltInitAlterSpaceDropFile;

stlStatus qltValidateAlterSpaceDropFile( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         stlChar       * aSQLString,
                                         qllNode       * aParseTree,
                                         qllEnv        * aEnv );


stlStatus qltOptCodeAlterSpaceDropFile( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv );

stlStatus qltOptDataAlterSpaceDropFile( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv );

stlStatus qltExecuteAlterSpaceDropFile( smlTransId      aTransID,
                                        smlStatement  * aStmt,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv );


#endif /* _QLT_ALTER_SPACE_ADD_FILE_H_ */
