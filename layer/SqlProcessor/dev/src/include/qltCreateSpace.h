/*******************************************************************************
 * qltCreateSpace.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qltCreateSpace.h 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLT_CREATE_SPACE_H_
#define _QLT_CREATE_SPACE_H_ 1

/**
 * @file qltCreateSpace.h
 * @brief CREATE TABLESPACE
 */

/**
 * @brief CREATE TABLESPACE 를 위한 Init Plan
 */
typedef struct qltInitCreateSpace
{
    qlvInitPlan      mCommonInit;         /**< Common Init Plan */
    
    stlChar                * mSpaceName;      /**< Tablespace Name */
    smlTbsAttr             * mSpaceAttr;      /**< Tablespace atrribute */
    ellTablespaceMediaType   mMediaType;      /**< Media type */
    ellTablespaceUsageType   mUsageType;      /**< Usage type */
} qltInitCreateSpace;

stlStatus qltValidateCreateSpace( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qltOptCodeCreateSpace( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qltOptDataCreateSpace( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qltExecuteCreateSpace( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

#endif /* _QLT_CREATE_SPACE_H_ */
