/*******************************************************************************
 * qlcrOpenCursor.h
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

#ifndef _QLCR_OPEN_CURSOR_H_
#define _QLCR_OPEN_CURSOR_H_ 1

/**
 * @file qlcrOpenCursor.h
 * @brief OPEN CURSOR 처리 함수 
 */


/**
 * @defgroup qlcrOpenCursor OPEN cursor
 * @ingroup qlcrCursor
 * @{
 */

/**
 * @brief OPEN CURSOR init plan
 */
typedef struct qlcrInitOpenCursor
{
    stlChar                   * mCursorName;
    stlInt64                    mProcID;

    stlChar                   * mCursorQuery;      /**< Cursor Query */
    stlInt32                    mCursorStmtAttr;   /**< Cursor Statement 속성 */
    qllNodeType                 mCursorStmtType;   /**< Cursor Statement Type */
    
    ellCursorInstDesc         * mInstDesc;
} qlcrInitOpenCursor;


    
stlStatus qlcrValidateOpenCursor( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qlcrCodeOptimizeOpenCursor( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

stlStatus qlcrDataOptimizeOpenCursor( smlTransId      aTransID,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );

stlStatus qlcrExecuteFuncOpenCursor( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     qllEnv        * aEnv );

stlStatus qlcrSetNamedCursorOpen( smlStatement           * aStmt,
                                  qllDBCStmt             * aCursorDBCStmt,
                                  qllStatement           * aCursorSQLStmt,
                                  ellCursorInstDesc      * aCursorInstDesc,
                                  qllEnv                 * aEnv );

/** @} qlcrOpenCursor */


#endif /* _QLCR_OPEN_CURSOR_H_ */
