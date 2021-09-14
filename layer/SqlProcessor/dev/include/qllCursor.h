/*******************************************************************************
 * qllCursor.h
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

#ifndef _QLL_CURSOR_H_
#define _QLL_CURSOR_H_ 1

/**
 * @file qllCursor.h
 * @brief CURSOR 처리 함수 
 */

/**
 * @defgroup qllCursor SQL Cursor
 * @ingroup qllAPI
 * @{
 */


/****************************************************************
 * Cursor 속성 획득 함수
 ****************************************************************/

ellCursorStandardType   qllGetCursorStandardType( qllStatement * aSQLStmt );

ellCursorSensitivity    qllGetCursorSensitivity( qllStatement * aSQLStmt );     
ellCursorScrollability  qllGetCursorScrollability( qllStatement * aSQLStmt );   
ellCursorHoldability    qllGetCursorHoldability( qllStatement * aSQLStmt );     
ellCursorUpdatability   qllGetCursorUpdatability( qllStatement * aSQLStmt );    
ellCursorReturnability  qllGetCursorReturnability( qllStatement * aSQLStmt );   

/*
 * Cursor 제어 함수 
 */

qllResultSetDesc * qllGetCursorResultSetDesc( qllStatement * aSQLStmt );

stlStatus qllSetNamedCursorOpen( smlStatement           * aStmt,
                                 qllDBCStmt             * aDBCStmt,
                                 qllStatement           * aSQLStmt,
                                 ellCursorInstDesc      * aCursorInstDesc,
                                 qllEnv                 * aEnv );

stlStatus qllCloseCursor( qllDBCStmt   * aDBCStmt,
                          qllStatement * aSQLStmt,
                          qllEnv       * aEnv );

/*
 * Cursor Fetch 함수 
 */


void qllAddCallCntCursorFetch( qllEnv * aEnv );

stlStatus qllFetchCursorForwardBlock( smlTransId               aTransID,
                                      smlStatement           * aStmt,
                                      qllDBCStmt             * aDBCStmt,
                                      qllResultSetDesc       * aResultSetDesc,
                                      stlBool                * aEOF,
                                      qllEnv                 * aEnv );

stlStatus qllFetchCursorScrollBlock( smlTransId               aTransID,
                                     smlStatement           * aStmt,
                                     qllDBCStmt             * aDBCStmt,
                                     qllResultSetDesc       * aResultSetDesc,
                                     stlInt64                 aStartPosition,
                                     stlBool                * aEOF,
                                     qllEnv                 * aEnv );

stlStatus qllFetchCursorOneRow( smlTransId               aTransID,
                                smlStatement           * aStmt,
                                qllDBCStmt             * aDBCStmt,
                                qllResultSetDesc       * aResultSetDesc,
                                qllFetchOrientation      aFetchOrient,
                                stlInt64                 aFetchPosition,
                                qllCursorRowStatus     * aRowStatus,
                                stlInt32               * aValidBlockIdx,
                                qllEnv                 * aEnv );

/*
 * Cursor 생성, 변경 함수 
 */

stlStatus qllDeclareNamedCursor( stlChar                  * aCursorName,
                                 ellCursorInstDesc       ** aInstDesc,
                                 qllEnv                   * aEnv );


stlStatus qllRenameCursor( stlChar            * aOrgName,
                           stlChar            * aNewName,
                           ellCursorInstDesc ** aNewInstDesc,
                           qllEnv             * aEnv );

stlStatus qllDeleteCursor( ellCursorDeclDesc * aDeclDesc,
                           ellCursorInstDesc * aInstDesc,
                           qllEnv            * aEnv );





/** @} qllCursor */


#endif /* _QLL_CURSOR_H_ */
