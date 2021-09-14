/*******************************************************************************
 * qlcrCloseCursor.h
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

#ifndef _QLCR_CLOSE_CURSOR_H_
#define _QLCR_CLOSE_CURSOR_H_ 1

/**
 * @file qlcrCloseCursor.h
 * @brief CLOSE CURSOR 처리 함수 
 */


/**
 * @defgroup qlcrCloseCursor CLOSE cursor
 * @ingroup qlcrCursor
 * @{
 */

/**
 * @brief CLOSE CURSOR init plan
 */
typedef struct qlcrInitCloseCursor
{
    ellCursorInstDesc         * mInstDesc;
} qlcrInitCloseCursor;


    
stlStatus qlcrValidateCloseCursor( smlTransId      aTransID,
                                   qllDBCStmt    * aDBCStmt,
                                   qllStatement  * aSQLStmt,
                                   stlChar       * aSQLString,
                                   qllNode       * aParseTree,
                                   qllEnv        * aEnv );

stlStatus qlcrCodeOptimizeCloseCursor( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qlcrDataOptimizeCloseCursor( smlTransId      aTransID,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv );

stlStatus qlcrExecuteFuncCloseCursor( smlTransId      aTransID,
                                      smlStatement  * aStmt,
                                      qllDBCStmt    * aDBCStmt,
                                      qllStatement  * aSQLStmt,
                                      qllEnv        * aEnv );


stlStatus qlcrCloseAllOpenNotHoldableCursor4TransEnd( qllEnv * aEnv );

stlStatus qlcrCloseAllOpenNotHoldableCursor4RollbackSavepoint( stlInt64     aSavepointTimestamp,
                                                               qllEnv     * aEnv );

stlStatus qlcrFiniSQLCursor( qllEnv * aEnv );

/** @} qlcrCloseCursor */


#endif /* _QLCR_CLOSE_CURSOR_H_ */
