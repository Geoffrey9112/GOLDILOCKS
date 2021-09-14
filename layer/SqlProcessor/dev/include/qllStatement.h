/*******************************************************************************
 * qllStatement.h
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

#ifndef _QLLSTATEMENT_H_
#define _QLLSTATEMENT_H_ 1

/**
 * @file qllStatement.h
 * @brief SQL Processor Layer Statement Definition
 */

/*
 * N/A Function Pointer
 */

stlStatus qllValidateFuncNA( smlTransId     aTransID,
                             qllDBCStmt   * aDBCStmt,
                             qllStatement * aSQLStmt,
                             stlChar      * aSQLString,
                             qllNode      * aParseTree,
                             qllEnv       * aEnv );

stlStatus qllCodeOptimizeFuncNA( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv );

stlStatus qllDataOptimizeFuncNA( smlTransId     aTransID,
                                 qllDBCStmt   * aDBCStmt,
                                 qllStatement * aSQLStmt,
                                 qllEnv       * aEnv );

stlStatus qllExecuteFuncNA( smlTransId     aTransID,
                            smlStatement * aStmt,
                            qllDBCStmt   * aDBCStmt,
                            qllStatement * aSQLStmt,
                            qllEnv       * aEnv );

stlStatus qllInitSQL( qllStatement * aSQLStmt,
                      qllEnv       * aEnv );

stlStatus qllResetSQL( qllDBCStmt   * aDBCStmt,
                       qllStatement * aSQLStmt,
                       qllEnv       * aEnv );

stlStatus qllFiniSQL( qllDBCStmt   * aDBCStmt,
                      qllStatement * aSQLStmt,
                      qllEnv       * aEnv );

stlStatus qllClearOptInfoSQL( qllStatement  * aSQLStmt,
                              qllEnv        * aEnv );

stlStatus qllCleanMemBeforeOptimizeCodeSQL( qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv );

stlStatus qllCleanMemBeforeOptimizeDataSQL( qllDBCStmt   * aDBCStmt,
                                            qllStatement * aSQLStmt,
                                            qllEnv       * aEnv );

qllNodeType qllGetStmtType( qllStatement * aSQLStmt );

qllAccessMode qllGetAccessMode( qllStatement * aSQLStmt );
qllAccessMode qllGetAccessMode4UnknownStmt( qllStatement * aSQLStmt );

stlBool qllIsDDL( qllStatement * aSQLStmt );
stlBool qllIsCacheableStmt( qllStatement * aSQLStmt,
                            qllEnv       * aEnv );
stlBool qllNeedFetch( qllStatement * aSQLStmt );
stlBool qllNeedStateTransition( qllStatement * aSQLStmt );
stlBool qllSupportGlobalTransaction( qllStatement * aSQLStmt );
stlBool qllNeedAutocommit( qllStatement * aSQLStmt );
stlBool qllNeedSnapshotIterator( qllStatement * aSQLStmt );
stlBool qllSupportAtomicExecution( qllStatement * aSQLStmt );
stlBool qllIsLockableStmt( qllStatement * aSQLStmt );
stlBool qllIsLockableStmt4UnknownStmt( qllStatement * aSQLStmt );

stlStatus qllIsRecentAccessObject( smlTransId         aTransID,
                                   ellObjectList    * aAccessObjList,
                                   void             * aTableStatList,
                                   stlBool          * aIsValid,
                                   qllEnv           * aEnv );

stlChar * qllGetSQLString( qllStatement * aSQLStmt );
 
void qllSetSQLStmtCursorProperty( qllStatement             * aSQLStmt,
                                  ellCursorStandardType      aStandardType,
                                  ellCursorSensitivity       aSensitivity,
                                  ellCursorScrollability     aScrollability,
                                  ellCursorHoldability       aHoldability,
                                  ellCursorUpdatability      aUpdatability,
                                  ellCursorReturnability     aReturnability );

stlStatus qllGetQueryTargetContext( qllStatement        * aSQLStmt,
                                    knlRegionMem        * aRegionMem,
                                    void               ** aTargetExprList,
                                    knlExprStack       ** aTargetCodeStack,
                                    knlExprContextInfo ** aTargetDataContext,
                                    qllEnv              * aEnv );

/*
 * Data Control Language
 */

inline qllDCLType qllGetDCLType( qllStatement * aSQLStmt );
inline stlInt16 qllGetDCLAccessMode( qllStatement * aSQLStmt );
inline smlIsolationLevel qllGetDCLIsolationLevel( qllStatement * aSQLStmt );
inline stlInt16 qllGetDCLUniqueMode( qllStatement * aSQLStmt );
inline stlInt16 qllGetDCLGMTOffset( qllStatement * aSQLStmt );

/*
 * Atomic Attribute
 */

void    qllSetAtomicAttr( qllStatement  * aSQLStmt, stlBool aIsAtomic );
stlBool qllGetAtomicAttr( qllStatement  * aSQLStmt );


/*
 * Init Plan
 */

inline void * qllGetInitPlan( qllStatement  * aSQLStmt );


#endif /* _QLLSTATEMENT_H_ */


