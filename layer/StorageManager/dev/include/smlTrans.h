/*******************************************************************************
 * smlTrans.h
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


#ifndef _SMLTRANS_H_
#define _SMLTRANS_H_ 1

/**
 * @file smlTrans.h
 * @brief Storage Manager Transaction Routines
 */

/**
 * @defgroup smlTrans Transaction
 * @ingroup smExternal
 * @{
 */

stlStatus smlBeginTrans( smlIsolationLevel   aIsolationLevel,
                         stlBool             aIsGlobalTrans,
                         smlTransId        * aTransId,
                         smlEnv            * aEnv );

stlStatus smlEndTrans( smlTransId   aTransId,
                       stlInt32     aPendActionType,
                       stlInt64     aWriteMode,
                       smlEnv     * aEnv );

stlStatus smlCommit( smlTransId   aTransId,
                     stlChar    * aComment,
                     stlInt64     aWriteMode,
                     smlEnv     * aEnv );

stlStatus smlRollback( smlTransId   aTransId,
                       stlInt64     aWriteMode,
                       smlEnv     * aEnv );

stlStatus smlCommitInDoubtTransaction( smlTransId   aTransId,
                                       smlTransId   aInDoubtTransId,
                                       stlChar    * aComment,
                                       smlEnv     * aEnv );

stlStatus smlRollbackInDoubtTransaction( smlTransId   aTransId,
                                         stlInt64     aWriteMode,
                                         stlBool      aRestartRollback,
                                         smlEnv     * aEnv );

stlStatus smlFetchPreparedTransaction( smlTransId * aTransId,
                                       stlBool      aRecoverLock,
                                       stlXid     * aXid,
                                       smlEnv     * aEnv );

stlBool smlIsRepreparableTransaction( smlTransId aTransId );

stlStatus smlPrepare( stlXid     * aXid,
                      smlTransId   aTransId,
                      stlInt64     aWriteMode,
                      smlEnv     * aEnv );

stlTime smlBeginTransTime( smlTransId   aTransId,
                           smlEnv     * aEnv );

stlStatus smlRollbackTransForCleanup( smlTransId   aTransId,
                                      smlEnv     * aEnv );

stlStatus smlEndTransForCleanup( smlTransId   aTransId,
                                 smlEnv     * aEnv );

stlStatus smlSavepoint( smlTransId   aTransId,
                        stlChar    * aSavepointName,
                        smlEnv     * aEnv );

stlStatus smlReleaseSavepoint( smlTransId   aTransId,
                               stlChar    * aSavepointName,
                               smlEnv     * aEnv );

stlBool smlExistSavepoint( smlTransId   aTransId,
                           stlChar    * aSavepointName,
                           stlInt64   * aSavepointTimestamp,
                           smlEnv     * aEnv );

stlStatus smlRollbackToSavepoint( smlTransId   aTransId,
                                  stlChar    * aSavepointName,
                                  smlEnv     * aEnv );

stlStatus smlGetCommitScn( smlTransId   aTransId,
                           smlScn     * aCommitScn,
                           smlEnv     * aEnv );

smlScn smlGetNextSystemScn();
smlScn smlGetSystemScn();

smlScn smlGetTransViewScn( smlTransId   aTransId,
                           smlEnv     * aEnv );

stlStatus smlBuildAgableScn( smlEnv * aEnv );

stlBool smlIsAgableByViewScn( smlScn   aScn,
                              smlEnv * aEnv );

stlStatus smlSetAgerState( stlInt16   aState,
                           smlEnv   * aEnv );

stlInt64 smlGetDataStoreMode();

stlStatus smlWaitAllTransactionEnd( smlEnv * aEnv );

stlBool smlIsReadOnlyTrans( smlTransId aTransId );

/** @} */

#endif /* _SMLTRANS_H_ */
