/*******************************************************************************
 * qlrDropView.h
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

#ifndef _QLR_DROP_VIEW_H_
#define _QLR_DROP_VIEW_H_ 1

/**
 * @file qlrDropView.h
 * @brief DROP VIEW
 */

/**
 * @brief DROP VIEW 을 위한 Init Plan
 */
typedef struct qlrInitDropView
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    stlBool          mIfExists;         /**< IF EXISTS 옵션 사용 여부 */
    stlBool          mViewExist;        /**< View 존재 여부 */

    ellDictHandle    mSchemaHandle;     /**< Schema Handle */
    ellDictHandle    mViewHandle;       /**< View Handle */
} qlrInitDropView;



stlStatus qlrValidateDropView( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               stlChar       * aSQLString,
                               qllNode       * aParseTree,
                               qllEnv        * aEnv );

stlStatus qlrOptCodeDropView( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

stlStatus qlrOptDataDropView( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

stlStatus qlrExecuteDropView( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

/*
 * Execution 보조 함수
 */


#endif /* _QLR_DROP_VIEW_H_ */
