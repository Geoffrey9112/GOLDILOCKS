/*******************************************************************************
 * qlcrDeclareCursor.h
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

#ifndef _QLCR_DECLARE_CURSOR_H_
#define _QLCR_DECLARE_CURSOR_H_ 1

/**
 * @file qlcrDeclareCursor.h
 * @brief DECLARE CURSOR 처리 함수 
 */


/**
 * @defgroup qlcrDeclareCursor DECLARE .. CURSOR
 * @ingroup qlcrCursor
 * @{
 */

/**
 * @brief DECLARE CURSOR init plan
 */
typedef struct qlcrInitDeclareCursor
{
    stlChar                   * mCursorName;
    stlInt64                    mProcID;

    ellCursorOriginType         mOriginType;
    stlChar                   * mCursorOrigin;

    /*
     * Cursor Properties
     */

    ellCursorProperty           mStmtCursorProperty;
    
} qlcrInitDeclareCursor;


    
stlStatus qlcrValidateDeclareCursor( smlTransId      aTransID,
                                     qllDBCStmt    * aDBCStmt,
                                     qllStatement  * aSQLStmt,
                                     stlChar       * aSQLString,
                                     qllNode       * aParseTree,
                                     qllEnv        * aEnv );

stlStatus qlcrCodeOptimizeDeclareCursor( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );

stlStatus qlcrDataOptimizeDeclareCursor( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         qllEnv        * aEnv );

stlStatus qlcrExecuteFuncDeclareCursor( smlTransId      aTransID,
                                        smlStatement  * aStmt,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv );


stlStatus qlcrDeclareSQLCursor( stlChar                  * aCursorName,
                                stlInt64                   aProcID,
                                ellCursorOriginType        aOriginType,
                                stlChar                  * aCursorOrigin,
                                ellCursorProperty        * aCursorProperty,
                                qllEnv                   * aEnv );

/** @} qlcrDeclareCursor */


#endif /* _QLCR_DECLARE_CURSOR_H_ */
