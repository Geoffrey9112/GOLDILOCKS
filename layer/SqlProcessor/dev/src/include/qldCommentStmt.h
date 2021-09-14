/*******************************************************************************
 * qldCommentStmt.h
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

#ifndef _QLD_COMMENT_STMT_H_
#define _QLD_COMMENT_STMT_H_ 1

/**
 * @file qldCommentStmt.h
 * @brief COMMENT Statement
 */

/**
 * @brief Init Plan of Comment Statement
 */
typedef struct qldInitCommentStmt
{
    qlvInitPlan      mCommonInit;         /**< Common Init Plan */
    
    ellObjectType    mObjectType;         /**< Object Type */

    ellDictHandle    mObjectHandle;       /**< Object Dictionary Handle */
    ellDictHandle    mRelatedTableHandle; /**< Object 가 종속된 Table Handle */

    stlChar        * mCommentString;      /**< Comment String */
} qldInitCommentStmt;
    

stlStatus qldValidateCommentStmt( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qldOptCodeCommentStmt( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qldOptDataCommentStmt( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qldExecuteCommentStmt( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

#endif /* _QLD_COMMENT_STMT_H_ */


