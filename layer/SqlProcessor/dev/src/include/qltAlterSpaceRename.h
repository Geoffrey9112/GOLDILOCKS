/*******************************************************************************
 * qltAlterSpaceRename.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qltAlterSpaceRename.h 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLT_ALTER_SPACE_RENAME_H_
#define _QLT_ALTER_SPACE_RENAME_H_ 1

/**
 * @file qltAlterSpaceRename.h
 * @brief ALTER TABLESPACE .. RENAME TO
 */

/**
 * @brief ALTER TABLESPACE RENAME TO 를 위한 Init Plan
 */
typedef struct qltInitAlterSpaceRename
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle    mSpaceHandle;  /**< Tablespace Handle */
    stlChar       *  mNewSpaceName; /**< New Tablespace Name */
} qltInitAlterSpaceRename;

stlStatus qltValidateAlterSpaceRename( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv );


stlStatus qltOptCodeAlterSpaceRename( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qltOptDataAlterSpaceRename( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qltExecuteAlterSpaceRename( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

#endif /* _QLT_ALTER_SPACE_RENAME_H_ */
