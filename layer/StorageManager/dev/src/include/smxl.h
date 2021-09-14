/*******************************************************************************
 * smxl.h
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


#ifndef _SMXL_H_
#define _SMXL_H_ 1

/**
 * @file smxl.h
 * @brief Storage Manager Layer Local Transaction Internal Routines
 */

/**
 * @defgroup smxl Transaction Manager
 * @ingroup smInternal
 * @{
 */

void smxlRegisterUndoFuncs( smgComponentClass   aComponentClass,
                            smxlUndoFunc      * aUndoFuncs );
stlStatus smxlStartupNoMount( void   ** aWarmupEntry,
                              smlEnv  * aEnv );
stlStatus smxlStartupMount( smlEnv * aEnv );
stlStatus smxlStartupPreOpen( smlEnv * aEnv );
stlStatus smxlStartupOpen( smlEnv * aEnv );
stlStatus smxlWarmup( void   * aWarmupEntry,
                      smlEnv * aEnv );
stlStatus smxlCooldown( smlEnv * aEnv );
stlStatus smxlBootdown( smlEnv * aEnv );
stlStatus smxlShutdownDismount( smlEnv * aEnv );
stlStatus smxlShutdownPostClose( smlEnv * aEnv );
stlStatus smxlShutdownClose( smlEnv * aEnv );
stlStatus smxlRegisterFxTables( smlEnv * aEnv );

stlInt64 smxlGetDataStoreMode();
stlStatus smxlInitTrans( stlInt64   aTransTableSize,
                         stlBool    aIsShared,
                         smlEnv   * aEnv );
stlStatus smxlResetTrans( smlEnv * aEnv );
stlStatus smxlFinTrans( smlEnv * aEnv );
stlStatus smxlValidateTransId( smlTransId   aTransId,
                               smlEnv     * aEnv );
stlBool smxlIsGlobalTrans( smlTransId aTransId );
stlStatus smxlAllocTrans( smlIsolationLevel   aIsolationLevel,
                          stlBool             aIsGlobalTrans,
                          smxlTransId       * aTransId,
                          stlUInt64         * aTransSeq,
                          smlEnv            * aEnv );
stlStatus smxlWaitTrans( smxlTransId   aTransIdA,
                         smxlTransId   aTransIdB,
                         stlInt64      aTimeInterval,
                         smlEnv      * aEnv );
stlInt32 smxlGetActiveTransCount();
stlStatus smxlSuspendOnTransSlot( smxlTransId    aMyTransId,
                                  stlInt16       aTargetTransSlotId,
                                  stlInt64       aTimeInterval,
                                  stlChar      * aValidationLockMode,
                                  smlEnv       * aEnv );
stlStatus smxlSuspend( smxlTransId   aTransId,
                       stlInt64      aTimeInterval,
                       smlEnv      * aEnv );
stlStatus smxlResume( smxlTransId   aTransId,
                      smlEnv      * aTargetEnv,
                      smlEnv      * aEnv );
void smxlBindTrans( smxlTransId aTransId );
void smxlSetTransViewScn( smxlTransId aTransId,
                          smlScn     aTransViewScn );
stlStatus smxlFreeTrans( smxlTransId   aTransId,
                         stlInt32      aPendActionType,
                         stlBool       aCleanup,
                         stlInt64      aWriteMode,
                         smlEnv      * aEnv );
stlStatus smxlRollback( smxlTransId   aTransId,
                        stlInt64      aWriteMode,
                        stlBool       aCleanup,
                        smlEnv      * aEnv );
stlStatus smxlCommitInDoubtTransaction( smxlTransId   aTransId,
                                        smxlTransId   aInDoubtTransId,
                                        stlChar     * aComment,
                                        smlEnv      * aEnv );
stlStatus smxlRollbackInDoubtTransaction( smxlTransId   aTransId,
                                          smxlTransId   aInDoubtTransId,
                                          stlBool       aRestartRollback,
                                          smlEnv      * aEnv );
stlBool smxlIsRepreparable( smxlTransId aTransId );
void smxlSetRepreparable( smxlTransId aTransId,
                          stlBool    aRepreparable );

stlStatus smxlWaitAllTrans( smlEnv * aEnv );
stlTime smxlBeginTransTime( smxlTransId   aTransId,
                            smlEnv      * aEnv );
stlStatus smxlRestartRollback( smxlTransId   aTransId,
                               smlEnv      * aEnv );
stlStatus smxlCommit( smxlTransId   aTransId,
                      stlChar     * aComment,
                      stlInt64      aWriteMode,
                      smlEnv      * aEnv );
stlStatus smxlPrepare( stlXid      * aXid,
                       smxlTransId   aTransId,
                       stlInt64      aWriteMode,
                       smlEnv      * aEnv );
stlStatus smxlFetchPreparedTransaction( smxlTransId * aTransId,
                                        stlUInt64   * aTransSeq,
                                        stlBool       aRecoverLock,
                                        stlXid      * aXid,
                                        smlEnv      * aEnv );
stlStatus smxlPreparePreparedTransaction( smxlTransId   aTransId,
                                          smlEnv      * aEnv );
stlStatus smxlAllocShmMem( smxlTransId   aTransId,
                           stlInt32      aSize,
                           void       ** aAddr,
                           smlEnv      * aEnv );
void smxlUpdateTransId( smxlTransId   aTransId,
                        smlEnv      * aEnv );
void smxlUpdateBeginLsn( smxlTransId   aTransId,
                         smlEnv      * aEnv );
inline smxlTransId smxlGetTransId( smxlTransId aTransId );
inline smlIsolationLevel smxlGetIsolationLevel( smxlTransId aTransId );
inline smxlTransId smxlGetUnsafeTransId( smxlTransId aTransId );
void smxlGetSystemInfo( smxlSystemInfo * aSystemInfo );
void smxlAddTryLogCount( smxlTransId aTransId,
                         stlInt32   aTryLogCount );
stlStatus smxlGetFirstActiveTrans( smxlTransId       * aTransId,
                                   smxlTransSnapshot * aTransSnapshot,
                                   smlEnv            * aEnv );
stlStatus smxlGetNextActiveTrans( smxlTransId       * aTransId,
                                  smxlTransSnapshot * aTransSnapshot,
                                  smlEnv            * aEnv );

smlScn smxlGetTransViewScn( smxlTransId aTransId );
smlScn smxlGetAgableTbsScn( smlTbsId   aTbsId,
                            smlEnv   * aEnv );
smlScn smxlGetAgableScn( smlEnv * aEnv );
smlScn smxlGetAgableStmtScn( smlEnv * aEnv );
smlScn smxlGetAgableViewScn( smlEnv * aEnv );
smlScn smxlGetMinTransViewScn( smlEnv * aEnv );
stlStatus smxlBuildAgableScn( smlEnv * aEnv );
stlStatus smxlBuildAgableScn4Restart( smlEnv * aEnv );
stlStatus smxlGetCommitScn( smxlTransId   aTransId,
                            smlScn        aTransViewScn,
                            smlScn      * aCommitScn,
                            smlEnv      * aEnv );
stlStatus smxlGetSafeCommitScn( smxlTransId   aTransId,
                                smlScn        aTransViewScn,
                                smlScn      * aCommitScn,
                                stlBool     * aIsSuccess,
                                smlEnv      * aEnv );
smlTcn smxlGetNextTcn( smxlTransId aTransId );
smlTcn smxlGetTcn( smxlTransId aTransId );
void smxlSetTransState( smxlTransId aTransId,
                        stlInt32   aState );
stlInt32 smxlGetTransState( smxlTransId aTransId );
stlInt64 smxlGetNextTimestamp( smxlTransId aTransId );
stlInt64 smxlGetCurTimestamp( smxlTransId aTransId );
stlStatus smxlSetAgerState( stlInt16   aState,
                            smlEnv   * aEnv );

void smxlGetViewScn( smxlTransId   aTransId,
                     smlScn      * aScn );
void smxlSetSystemScn( smlScn aSystemScn );
smlScn smxlGetSystemScn();
void smxlGetAtomicSystemScn( smlScn * aScn );
smlScn smxlGetCommitScnFromLog( stlChar * aCommitLog );
smxlTransId smxlGetTransIdFromLog( stlChar * aCommitLog );
smlScn smxlGetTransViewScnFromLog( stlChar * aCommitLog );
stlStatus smxlGetCommitScnFromCache( smxlTransId   aTransId,
                                     smlScn      * aCommitScn,
                                     smlEnv      * aEnv );

stlStatus smxlAttachUndoRelations( smlEnv * aEnv );
stlStatus smxlCreateUndoRelations( smxlTransId   aTransId,
                                   smlEnv      * aEnv );
stlStatus smxlResetUndoRelations( smlEnv * aEnv );
stlStatus smxlAllocUndoRelationInternal( smxlTransId   aTransId,
                                         smlEnv      * aEnv );
stlStatus smxlAllocUndoRelation( smxlTransId   aTransId,
                                 smlEnv      * aEnv );
stlStatus smxlBindUndoRelation( smxlTransId   aTransId,
                                smlEnv      * aEnv );
void smxlSetUndoSegRid( smxlTransId aTransId,
                        smlRid     aUndoSegRid );
void * smxlGetUndoSegHandleByUndoRelId( stlInt32 aUndoRelId );
smlRid smxlGetLstUndoRid( smxlTransId aTransId );
smlRid smxlGetUndoSegRidFromLog( stlChar * aUndoRecord );
smlScn smxlGetTransViewScnFromUndoLog( stlChar * aUndoRecord );
stlStatus smxlRollbackUndoRecords( smxlTransId   aTransId,
                                   smlRid        aLowUndoRid,
                                   stlBool       aRestartRollback,
                                   smlEnv      * aEnv );
stlStatus smxlRecoverLock4PreparedTransaction( smxlTransId   aTransId,
                                               stlBool       aRecoverLock,
                                               stlXid      * aXid,
                                               smlEnv      * aEnv );
inline void smxlSetTransAttr( smxlTransId aTransId,
                              stlInt32    aAttr );
inline stlInt32 smxlGetTransAttr( smxlTransId aTransId );
inline stlBool smxlIsUpdatable( smxlTransId aTransId );
inline stlBool smxlIsLockable( smxlTransId aTransId );
inline stlInt64 smxlGetTransTableSize( );

stlBool smxlIsTransRecordRecorded( smxlTransId aTransId );
void smxlSetTransRecordRecorded( smxlTransId aTransId );
void smxlIncUndoPageCount( smxlTransId aTransId );
stlInt64 smxlGetUndoPageCount( smxlTransId aTransId );

/*
 * Media Recovery Transaction Table Modules
 */
void smxlBindTrans4MediaRecovery( smxlTransId   aTransId,
                                  smlEnv      * aEnv );
void smxlSetTransViewScn4MediaRecovery( smxlTransId   aTransId,
                                        smlScn        aTransViewScn,
                                        smlEnv      * aEnv );
stlStatus smxlFreeTrans4MediaRecovery( smxlTransId   aTransId,
                                       smlEnv      * aEnv );
inline smxlTransId smxlGetUnsafeTransId4MediaRecovery( smxlTransId   aTransId,
                                                      smlEnv      * aEnv );
void smxlSetUndoSegRid4MediaRecovery( smxlTransId   aTransId,
                                      smlRid        aUndoSegRid,
                                      smlEnv      * aEnv );
inline void smxlSetTransRecordRecorded4MediaRecovery( smxlTransId   aTransId,
                                                      smlEnv      * aEnv );

stlStatus smxlInsertTransRecord( smxlTransId   aTransId,
                                 smlEnv      * aEnv );
stlStatus smxlInsertTransRecordWithMiniTrans( smxmTrans  * aMiniTrans,
                                              smlEnv     * aEnv );
stlStatus smxlPrepareTransRecord( smxlTransId   aTransId,
                                  smlEnv      * aEnv );
stlStatus smxlPrepareUndoSpace( smxlTransId  aTransId,
                                stlUInt32    aPageCount,
                                smlEnv     * aEnv );
stlStatus smxlInsertUndoRecord( smxmTrans  * aMiniTrans,
                                stlChar      aComponentClass,
                                stlUInt16    aLogType,
                                stlChar    * aData,
                                stlUInt16    aDataSize,
                                smlRid       aTargetRid,
                                smlRid     * aUndoRid,
                                smlEnv     * aEnv );
inline stlStatus smxlGetUndoRecord( smpHandle  * aPageHandle,
                                    smlRid     * aUndoRid,
                                    stlInt16   * aUndoRecType,
                                    stlInt8    * aUndoRecClass,
                                    stlChar   ** aUndoRecBody,
                                    stlInt16   * aUndoRecBodySize );
 
inline stlStatus smxlGetUnsafeUndoRecord( smpHandle * aPageHandle,
                                          smlRid    * aUndoRid,
                                          stlInt16    aUndoRecType,
                                          stlInt8     aUndoRecClass,
                                          stlChar   * aUndoRecBuf,
                                          stlInt16    aUndoRecBufSize,
                                          stlBool   * aIsSuccess,
                                          stlInt16  * aUndoRecSize );
   
stlStatus smxlMarkSavepoint( smxlTransId   aTransId,
                             stlChar     * aSavepointName,
                             smlEnv      * aEnv );
stlStatus smxlReleaseSavepoint( smxlTransId   aTransId,
                                stlChar     * aSavepointName,
                                smlEnv      * aEnv );
stlStatus smxlRollbackToSavepoint( smxlTransId   aTransId,
                                   stlChar     * aSavepointName,
                                   smlEnv      * aEnv );
stlBool smxlExistSavepoint( smxlTransId   aTransId,
                            stlChar     * aSavepointName,
                            stlInt64    * aSavepointTimestamp,
                            smlEnv      * aEnv );
stlBool smxlNeedRollback( smxlTransId aTransId );
smlScn smxlGetNextSystemScn();

stlStatus smxlRestructure( smlEnv * aEnv );
stlStatus smxlRestructureTransTable( smlEnv * aEnv );

stlStatus smxlAdjustGlobalVariables( smlEnv * aEnv );
stlStatus smxlReplaceTransactionTable( smxlTrans  ** aOrgTransTable,
                                       stlInt32    * aOrgTransTableSize,
                                       smxlTrans   * aNewTransTable,
                                       smlEnv      * aEnv );
stlStatus smxlRestoreTransactionTable( smxlTrans  * aOrgTransTable,
                                       stlInt32     aOrgTransTableSize,
                                       smlEnv     * aEnv );

stlInt64 smxlGetTransSeq( smxlTransId aTransId );

stlStatus smxlShrinkTo( smxlTransId     aTransId,
                        smlEnv        * aEnv );

/** @} */
    
#endif /* _SMXL_H_ */
