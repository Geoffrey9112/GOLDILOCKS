/*******************************************************************************
 * smr.h
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


#ifndef _SMR_H_
#define _SMR_H_ 1

/**
 * @file smr.h
 * @brief Storage Manager Layer Recovery Internal Routines
 */

/**
 * @defgroup smr Recovery
 * @ingroup smInternal
 * @{
 */

stlStatus smrStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv );
stlStatus smrStartupMount( smlEnv * aEnv );
stlStatus smrStartupPreOpen( knlStartupInfo * aStartupInfo,
                             smlEnv         * aEnv );
stlStatus smrStartupOpen( smlEnv  * aEnv );
stlStatus smrMediaRecover( stlBool  * aIsFinishedRecovery,
                           smlEnv   * aEnv );
stlStatus smrWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv );
stlStatus smrValidateOpen( stlChar   aLogOption,
                           smlEnv  * aEnv );
stlStatus smrCooldown( smlEnv * aEnv );
stlStatus smrBootdown( smlEnv * aEnv );
stlStatus smrShutdownPostClose( smlEnv * aEnv );
stlStatus smrShutdownClose( smlEnv * aEnv );
stlStatus smrShutdownDismount( smlEnv * aEnv );
stlStatus smrRegisterFxTables( smlEnv * aEnv );
stlStatus smrFormatLogFile( stlChar  * aLogFileName,
                            stlInt64   aLogFileSize,
                            stlInt16   aBlockSize,
                            stlBool    aIsReuseFile,
                            smlEnv   * aEnv );
void smrRegisterRedoVectors( smgComponentClass   aComponentClass,
                             smrRedoVector     * aRedoVectors );
stlStatus smrSetLogFlusherState( stlInt16   aState,
                                 smlEnv   * aEnv );
stlStatus smrSetCheckpointerState( stlInt16   aState,
                                   smlEnv   * aEnv );
stlUInt64 smrGetRecoveryPhase();
void smrSetRecoveryPhase( stlUInt64   aRecoveryPhase );
void smrGetRecoveryInfo( smrRecoveryInfo * aRecoveryInfo );
inline void smrGetConsistentChkptLsnAndLid( smrLsn * aLsn,
                                            smrLid * aLid );
inline void smrSetConsistentChkptLsnAndLid( smrLsn   aLsn,
                                            smrLid   aLid );
inline smrLsn smrGetLastCheckpointLsn();
inline smrLid smrGetLastCheckpointLid();
inline stlTime smrGetDatabaseCreationTime();
void smrGetChkptInfo( smrChkptInfo * aChkptInfo );
inline stlInt16 smrGetCheckpointState();
inline stlInt64 smrGetLogFlushingState();
smrLsn smrGetSystemLsn();
stlStatus smrSwitchLogGroup( smlEnv * aEnv );
stlStatus smrRenameLogFile( stlInt32           aFileCount,
                            smlLogMemberAttr * aFromList,
                            smlLogMemberAttr * aToList,
                            smlEnv           * aEnv );

stlStatus smrWakeupFlusher( smlEnv * aEnv );
stlStatus smrWriteLog( smxlTransId    aTransId,
                       void         * aLogData,
                       stlUInt32      aLogSize,
                       stlUInt16      aPieceCount,
                       smlRid         aSegRid,
                       stlBool        aSwitchBlock,
                       smrSbsn      * aWrittenSbsn,
                       smrLsn       * aWrittenLsn,
                       smlEnv       * aEnv );
stlStatus smrWriteLogWithBlock( smxlTransId    aTransId,
                                void         * aLogBlock,
                                stlUInt32      aLogSize,
                                stlUInt16      aPieceCount,
                                smlRid         aSegRid,
                                smrSbsn      * aWrittenSbsn,
                                smrLsn       * aWrittenLsn,
                                smlEnv       * aEnv );
stlStatus smrWriteChkptLog( smrLsn    aOldestLsn,
                            smlEnv  * aEnv );
stlStatus smrWriteSupplementalLog( smxlTransId    aTransId,
                                   void         * aLogData,
                                   stlUInt32      aLogSize,
                                   stlUInt16      aLogType,
                                   smlRid         aSegRid,
                                   smlEnv       * aEnv );
stlStatus smrFlushLog( smrLsn    aLsn,
                       smrSbsn   aSbsn,
                       stlBool   aSwitchBlock,
                       stlInt64  aWriteMode,
                       smlEnv  * aEnv );
stlStatus smrWaitEnableLogging( smlEnv * aEnv );
stlStatus smrStopLogFlushing( smlEnv  * aEnv );
stlStatus smrStartLogFlushing( smlEnv  * aEnv );
stlStatus smrFlushStreamLogForFlusher( stlInt64 * aFlushedLsn,
                                       stlInt64 * aFlushedSbsn,
                                       smlEnv   * aEnv );
stlStatus smrFlushAllLogs( stlBool    aForceDiskIo,
                           smlEnv   * aEnv );
stlStatus smrCheckpointEventHandler( void      * aData,
                                     stlUInt32   aDataSize,
                                     stlBool   * aDone,
                                     void      * aEnv );
inline stlStatus smrCheckpoint( smpFlushBehavior   aFlushBehavior,
                                smlEnv           * aEnv );
stlStatus smrWaitPageFlusher( smrLsn   aLsn,
                              smlEnv * aEnv );

stlStatus smrArchivelogEventHandler( void      * aData,
                                     stlUInt32   aDataSize,
                                     stlBool   * aDone,
                                     void      * aEnv );
inline stlStatus smrArchivelog( smrLsn   aLsn,
                                smlEnv * aEnv );

void smrActivateLogFlusher();
void smrDeactivateLogFlusher();

stlStatus smrValidateCheckpointLog( stlUInt8   aLogOption,
                                    smlEnv   * aEnv );
stlStatus smrRecover( stlUInt8   aLogOption,
                      smlEnv   * aEnv );

stlStatus smrMediaRecoverDatabase( stlBool * aIsFinishedRecovery,
                                   smlEnv  * aEnv );

stlStatus smrMediaRecoverTablespace( smlEnv * aEnv );

stlStatus smrMediaRecoverDatafile( smlEnv * aEnv );

stlStatus smrMtxUndo( smrLogPieceHdr * aLogPieceHdr,
                      stlChar        * aLogPieceBody,
                      smlEnv         * aEnv );

stlStatus smrCreateLogStream( smlLogStreamAttr * aAttr,
                              smlEnv           * aEnv );
stlStatus smrDropLogStream( smlLogStreamAttr * aAttr,
                            smlEnv           * aEnv );
stlStatus smrAddLogGroupEventHandler( void      * aData,
                                      stlUInt32   aDataSize,
                                      stlBool   * aDone,
                                      void      * aEnv );
stlStatus smrAddLogMemberEventHandler( void      * aData,
                                       stlUInt32   aDataSize,
                                       stlBool   * aDone,
                                       void      * aEnv );
stlStatus smrSwitchLogGroupEventHandler( void      * aData,
                                         stlUInt32   aDataSize,
                                         stlBool   * aDone,
                                         void      * aEnv );
stlStatus smrDoPendAddLogGroup( stlUInt32   aActionFlag,
                                smgPendOp * aPendOp,
                                smlEnv    * aEnv );
stlStatus smrDropLogGroupEventHandler( void      * aData,
                                       stlUInt32   aDataSize,
                                       stlBool   * aDone,
                                       void      * aEnv );
stlStatus smrDropLogMemberEventHandler( void      * aData,
                                        stlUInt32   aDataSize,
                                        stlBool   * aDone,
                                        void      * aEnv);

stlStatus smrDoPendDropLogGroup( stlUInt32   aActionFlag,
                                 smgPendOp * aPendOp,
                                 smlEnv    * aEnv );
stlStatus smrDoPendAddLogFile( stlUInt32   aActionFlag,
                               smgPendOp * aPendOp,
                               smlEnv    * aEnv );
stlStatus smrDoPendAddLogMember( stlUInt32   aActionFlag,
                                 smgPendOp * aPendOp,
                                 smlEnv    * aEnv );
stlStatus smrDoPendDropLogMember( stlUInt32   aActionFlag,
                                  smgPendOp * aPendOp,
                                  smlEnv    * aEnv );
stlStatus smrRenameLogFileEventHandler( void      * aData,
                                        stlUInt32   aDataSize,
                                        stlBool   * aDone,
                                        void      * aEnv );
stlStatus smrDoPendRenameLogFile( stlUInt32   aActionFlag,
                                  smgPendOp * aPendOp,
                                  smlEnv    * aEnv );
stlStatus smrIsUsedLogFile( stlChar   * aFileName,
                            stlBool   * aExist,
                            stlInt16  * aGroupId,
                            smlEnv    * aEnv );
stlStatus smrCheckLogGroupId( stlInt32   aLogGroupId,
                              stlBool  * aFound,
                              smlEnv   * aEnv );
stlStatus smrLoadLogCtrlSection( stlInt64 * aMinLogFileSize,
                                 smlEnv * aEnv );
stlStatus smrDumpLogCtrlSection( stlChar * aFilePath,
                                 smlEnv  * aEnv );
stlStatus smrCreateLogCtrlSection( stlFile  * aFile,
                                   stlSize  * aWrittenBytes,
                                   smlEnv   * aEnv );
stlStatus smrWriteLogCtrlSection( stlFile  * aFile,
                                  stlSize  * aWrittenBytes,
                                  smlEnv   * aEnv );

stlStatus smrDumpLogFile( stlChar * aFilePath,
                          stlInt64  aTargetLsn,
                          stlInt64  aLsnOffset,
                          stlInt64  aFetchTotalCnt,
                          smlEnv  * aEnv );

inline smrLsn smrGetCurRedoLogLsn();

inline stlBool smrIsLogFlushedByLsn( smrLsn aLsn );

inline stlBool smrIsLogFlusherActive();
inline stlBool smrIsCheckpointerActive();
stlBool smrProceedMediaRecovery( smlEnv * aEnv );
stlBool smrProceedOnlineMediaRecovery( smlEnv * aEnv );
stlInt64 smrGetFrontLsn();
stlInt64 smrGetFrontSbsn();
stlInt64 smrGetRearSbsn();
void smrLogStreamStateString( stlUInt16   aState,
                              stlUInt32   aSize,
                              stlChar   * aString );
void smrLogGroupStateString( stlUInt16   aState,
                             stlUInt32   aSize,
                             stlChar   * aString );

inline void smrSetArchivelogMode( stlInt32   aMode );
inline stlInt32 smrGetArchivelogMode();
stlStatus smrChangeArchivelogMode( stlInt32   aMode,
                                   smlEnv   * aEnv );

stlStatus smrAlterArchivelogEventHandler( void      * aData,
                                          stlUInt32   aDataSize,
                                          stlBool   * aDone,
                                          void      * aEnv );

stlStatus smrAdjustLogSection( smlEnv * aEnv );

inline stlBool smrGetRecoveryType( smlEnv  * aEnv );
inline stlUInt8 smrGetRecoveryObjectType( smlEnv  * aEnv );
stlBool smrProceedIncompleteMediaRecovery( smlEnv  * aEnv );
stlBool smrIsLogfileCrossed( stlInt64   aFileSeqNo,
                             smlEnv   * aEnv );

stlStatus smrIsRedoableRid4DatafileRecovery( smrMediaRecoveryInfo * aRecoveryInfo,
                                             smlRid               * aDataRid,
                                             smrLsn                 aCurLsn,
                                             stlBool              * aFound,
                                             smlEnv               * aEnv );

stlBool smrIsExistSameTbs4DatafileRecovery( smrMediaRecoveryInfo    * aRecoveryInfo,
                                            smrRecoveryDatafileInfo * aDatafileInfo );

stlStatus smrCreateMediaRecoveryDatafileInfo( smlDatafileRecoveryInfo * aRecoveryInfo,
                                              smrMediaRecoveryInfo    * aMediaRecoveryInfo,
                                              smlEnv                  * aEnv );

void smrSetRecoveryType( stlChar aRecoveryType );

stlStatus smrArchivingLogFiles4ResetLogs( smlEnv * aEnv );

stlStatus smrSetRecoveryPhaseAtMount( stlUInt8   aRecoveryType,
                                      smlEnv   * aEnv );

void smrSetDatabaseMediaRecoveryInfo( void  * aMediaRecoveryInfo );
void * smrGetDatabaseMediaRecoveryInfo();

stlStatus smrInitRecoveryContextAtMount( smlMediaRecoveryInfo * aRecoveryInfo,
                                         smlEnv               * aEnv );

stlStatus smrFiniRecoveryContextAtMount( smlEnv  * aEnv );

stlStatus smrInitRecoveryContextAtOpen( smlRecoveryType    aRecoveryType,
                                        smlTbsId           aTbsId,
                                        smlEnv           * aEnv );

stlStatus smrFiniRecoveryContextAtOpen( smlEnv  * aEnv );

void smrSetIncompleteRecoveryCondition( stlUInt8   aType,
                                        smlEnv   * aEnv );

stlInt64 smrGetUntilValue( smlEnv  * aEnv );
stlUInt8 smrGetImrOption( smlEnv  * aEnv );

stlBool smrIsSuitableLogBlockSize( stlInt64   aBlockSize );

stlStatus smrAdjustGlobalVariables( stlInt64  * aBlockSize,
                                    smlEnv    * aEnv );

stlStatus smrRegisterDbFiles( smlEnv * aEnv );


stlStatus smrReproduceRecoveryWarmupEntry( void       *  aSrcWarmupEntry,
                                           void       ** aDestWarmupEntry,
                                           void       *  aSmfWarmupEntry,
                                           smlEnv     *  aEnv );

stlStatus smrFlushAllLogs4Syncher( void      * aSmrWarmupEntry,
                                   stlBool   * aSwitchedLogfile,
                                   stlBool     aSilent,
                                   stlLogger * aLogger,
                                   smlEnv    * aEnv );

stlStatus smrWriteLogCtrlSection4Syncher( stlFile  * aFile,
                                          void     * aWarmupEntry,
                                          stlSize  * aWrittenBytes,
                                          smlEnv   * aEnv );

stlStatus smrDestroyLogMirrorArea( smlEnv * aEnv );


stlStatus smrReadLastLsnFromLogfile( smrLsn  * aLastLsn,
                                     smlEnv  * aEnv );

/** @} */
    
#endif /* _SMR_H_ */
