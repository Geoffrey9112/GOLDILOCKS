/*******************************************************************************
 * smf.h
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


#ifndef _SMF_H_
#define _SMF_H_ 1

/**
 * @file smf.h
 * @brief Storage Manager Layer Data File Internal Routines
 */

extern smfWarmupEntry * gSmfWarmupEntry;

/**
 * @defgroup smf Data File
 * @ingroup smInternal
 * @{
 */

stlStatus smfStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv );
stlStatus smfStartupMount( smlEnv * aEnv );
stlStatus smfStartupPreOpen( smlEnv * aEnv );
stlStatus smfStartupOpen( smlEnv * aEnv );
stlStatus smfWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv );
stlStatus smfCooldown( smlEnv * aEnv );
stlStatus smfBootdown( smlEnv * aEnv );
stlStatus smfShutdownPostClose( smlEnv * aEnv );
stlStatus smfShutdownClose( smlEnv * aEnv );
stlStatus smfShutdownDismount( smlEnv * aEnv );
stlStatus smfRegisterFxTables( smlEnv * aEnv );
stlStatus smfValidateMount( smlEnv * aEnv );
stlStatus smfValidateOpen( stlUInt8   aLogOption,
                           smrLsn     aLastLsn,
                           smlEnv   * aEnv );

stlStatus smfCreateTbs( smlStatement * aStatement,
                        smlTbsAttr   * aTbsAttr,
                        stlBool        aUndoLogging,
                        smlTbsId     * aTbsId,
                        smlEnv       * aEnv );
stlStatus smfOnlineTbs( smlStatement * aStatement,
                        smlTbsId       aTbsId,
                        smlEnv       * aEnv );
stlStatus smfOfflineTbs( smlStatement       * aStatement,
                         smlTbsId             aTbsId,
                         smlOfflineBehavior   aOfflineBehavior,
                         smlEnv             * aEnv );
stlStatus smfSetOfflineRelationId( smlStatement       * aStatement,
                                   smlTbsId             aTbsId,
                                   stlInt64             aRelationId,
                                   smlEnv             * aEnv );

stlStatus smfValidateOnlineTbs( smlTbsId   aTbsId,
                                smlEnv   * aEnv );
stlInt64 smfGetPendingRelationId( smlTbsId aTbsId );
stlStatus smfAddDatafile( smlStatement    * aStatement,
                          smlTbsId          aTbsId,
                          smlDatafileAttr * aDatafileAttr,
                          smlDatafileId   * aDatafileId,
                          smlEnv          * aEnv );
stlStatus smfDropDatafile( smlStatement    * aStatement,
                           smlTbsId          aTbsId,
                           smlDatafileId     aDatafileId,
                           smlEnv          * aEnv );
stlStatus smfRenameDatafiles( smlStatement    * aStatement,
                              smlTbsId          aTbsId,
                              smlDatafileAttr * aOrgDatafileAttr,
                              smlDatafileAttr * aNewDatafileAttr,
                              smlEnv          * aEnv );
stlStatus smfCreateCtrlFile( smlEnv * aEnv );
stlStatus smfSaveCtrlFile( smlEnv * aEnv );

stlStatus smfInsertBackupRecord( stlChar  * aBackupFileName,
                                 stlTime    aBeginTime,
                                 smlTbsId   aTbsId,
                                 stlUInt8   aBackupObjectType,
                                 stlUInt8   aBackupOption,
                                 stlInt16   aLevel,
                                 smrLsn     aBackupLsn,
                                 smrLid     aBackupLid,
                                 smlEnv   * aEnv );
stlStatus smfWriteCtrlFile( stlFile       * aFile,
                            smfCtrlBuffer * aBuffer,
                            stlChar       * aContents,
                            stlInt32        aSize,
                            stlSize       * aWrittenBytes,
                            smlEnv        * aEnv );
stlStatus smfFlushCtrlFile( stlFile       * aFile,
                            smfCtrlBuffer * aBuffer,
                            smlEnv        * aEnv );
stlStatus smfReadCtrlFile( stlFile       * aFile,
                           smfCtrlBuffer * aBuffer,
                           stlChar       * aContents,
                           stlInt32        aSize,
                           smlEnv        * aEnv );
stlStatus smfMoveCtrlFilePosition( stlFile       * aFile,
                                   smfCtrlBuffer * aBuffer,
                                   stlInt32        aWhere,
                                   stlOffset       aOffset,
                                   smlEnv        * aEnv );
stlStatus smfLoadCtrlFile( smlEnv * aEnv );
stlStatus smfUnloadCtrlFile( smlEnv * aEnv );
stlStatus smfDropTbs( smlStatement * aStatement,
                      smlTbsId       aTbsId,
                      stlBool        aAndDatafiles,
                      smlEnv       * aEnv );
stlStatus smfDropUnregisteredTbs( smlTbsId   aTbsId,
                                  smlEnv   * aEnv );
stlStatus smfDropTbsAging( void      * aData,
                           stlUInt32   aDataSize,
                           stlBool   * aDone,
                           void      * aEnv );
stlStatus smfDropDatafileAging( void      * aData,
                                stlUInt32   aDataSize,
                                stlBool   * aDone,
                                void      * aEnv );
stlStatus smfRefineAtStartupPreOpen( smlEnv * aEnv );
stlStatus smfRefineAtStartupOpen( smlEnv * aEnv );
stlStatus smfRecoverTbsAndDatafileState( smlEnv * aEnv );
stlInt16 smfGetIoSlaveId( smlTbsId      aTbsId,
                          smlDatafileId aDatafileId );

stlBool smfExistTbs( smlTbsId aTbsId );
stlBool smfExistDatafile( smlTbsId      aTbsId,
                          smlDatafileId aDatafileId );
inline stlUInt16 smfGetTbsState( smlTbsId aTbsId );
inline stlUInt16 smfGetDatafileState( smlTbsId      aTbsId,
                                      smlDatafileId aDatafileId );
inline stlChar * smfGetDatafileName( smlTbsId      aTbsId,
                                     smlDatafileId aDatafileId );
stlStatus smfGetPchArray( smlTbsId         aTbsId,
                          smlDatafileId    aDatafileId,
                          void          ** aPchArray,
                          smlEnv         * aEnv );
stlStatus smfGetPchAndFrame( smlTbsId         aTbsId,
                             smlDatafileId    aDatafileId,
                             stlInt64         aPageSeqId,
                             void          ** aPch,
                             void          ** aFrame,
                             smlEnv         * aEnv );
smlTbsId smfGetTbsIdByHandle( void * aTbsHandle );
void * smfGetTbsHandle( smlTbsId aTbsId );

stlUInt32 smfGetMaxPageCount( smlTbsId      aTbsId,
                              smlDatafileId aDatafileId );
stlUInt32 smfGetMaxPageCountAtMount( smlTbsId      aTbsId,
                                     smlDatafileId aDatafileId );
inline stlUInt16 smfGetPageCountInExt( smlTbsId aTbsId );
stlUInt16 smfGetShmIndex( smlTbsId      aTbsId,
                          smlDatafileId aDatafileId );
stlUInt64 smfGetShmChunk( smlTbsId      aTbsId,
                          smlDatafileId aDatafileId );
stlStatus smfFirstTbs( smlTbsId * aTbsId,
                       stlBool  * aExist,
                       smlEnv   * aEnv );
stlStatus smfNextTbs( smlTbsId * aTbsId,
                      stlBool  * aExist,
                      smlEnv   * aEnv );
stlStatus smfNextDatafile( smlTbsId        aTbsId,
                           smlDatafileId * aDatafile,
                           stlBool       * aExist,
                           smlEnv        * aEnv );
stlStatus smfFirstDatafile( smlTbsId        aTbsId,
                            smlDatafileId * aDatafile,
                            stlBool       * aExist,
                            smlEnv        * aEnv );
stlStatus smfFindDatafileId( smlTbsId        aTbsId,
                             stlChar       * aDatafileName,
                             smlDatafileId * aDatafileId,
                             stlBool       * aFound,
                             smlEnv        * aEnv );
stlStatus smfFindExtendibleDatafile( smlTbsId          aTbsId,
                                     smlDatafileId    *aDatafileId,
                                     stlBool          *aFound,
                                     smlEnv           *aEnv );
stlStatus smfExtendDatafile( smxmTrans        *aMiniTrans,
                             smlTbsId          aTbsId,
                             smlDatafileId     aDatafileId,
                             smlEnv           *aEnv );
stlStatus smfWritePage( smlTbsId     aTbsId,
                        smlPid       aPageId,
                        void       * aPage,
                        stlInt32     aPageCount,
                        stlInt64     aChecksumType,
                        stlBool    * aTakenOffline,
                        smlEnv     * aEnv );
stlUInt64 smfGetTimestamp( smlTbsId      aTbsId,
                           smlDatafileId aDatafileId );
stlStatus smfGetTbsName( smlTbsId   aTbsId,
                         stlChar  * aTbsName,
                         smlEnv   * aEnv );
stlStatus smfChooseIoGroupId( stlChar  * aDatafileName,
                              stlInt16 * aIoGroupId,
                              smlEnv   * aEnv );
stlInt16 smfGetDatafileShmState( smlTbsId       aTbsId,
                                 smlDatafileId  aDatafileId );
void smfSetDatafileShmState( smlTbsId       aTbsId,
                             smlDatafileId  aDatafileId,
                             stlInt16       aShmState );
void smfSetDatafileCorruptionCount( smlTbsId       aTbsId,
                                    smlDatafileId  aDatafileId,
                                    stlUInt32      aCorruptionCount );
stlUInt32 smfGetDatafileCorruptionCount( smlTbsId       aTbsId,
                                         smlDatafileId  aDatafileId );
stlStatus smfExistControlFiles( smlEnv * aEnv );

stlStatus smfValidateControlfile4Mount( smlEnv   * aEnv );

stlStatus smfValidateControlfile4Open( stlUInt8   aLogOption,
                                       smlEnv   * aEnv );
smlServerState smfGetServerState();
void smfSetServerState( smlServerState aServerState );
stlInt64 smfGetDataStoreMode();
void smfSetDataStoreMode( stlInt64 aDataStoreMode );
void smfSetLastChkptLsn( smrLsn aChkptLsn );
inline stlInt64 smfGetCtrlFileCount();
smlDataAccessMode smfGetDataAccessMode();
void smfSetDataAccessMode( smlDataAccessMode aDataAccessMode );
stlUInt16 smfGetTransTableSize();
stlUInt16 smfGetUndoRelCount();
void smfSetTransTableSize( stlUInt16 aTransTableSize );
void smfSetUndoRelCount( stlUInt16 aUndoRelCount );
void smfSetResetLogsScn( smlScn aSystemScn );
smlScn smfGetResetLogsScn();
inline stlUInt16 smfGetTbsCount();
inline void smfSetCtrlFileLoadState( smfCtrlLoadState  aState );
inline smfCtrlLoadState smfGetCtrlFileLoadState();

stlStatus smfAdjustCtrlfile( smlEnv * aEnv );
stlStatus smfOpenCtrlSection( stlFile        * aFile,
                              smfCtrlSection   aSection,
                              smlEnv         * aEnv );
stlStatus smfOpenCtrlSectionWithPath( stlFile        * aFile,
                                      stlChar        * aFilePath,
                                      smfCtrlSection   aSection,
                                      stlBool        * aIsValid,
                                      smlEnv         * aEnv );
stlStatus smfCloseCtrlSection( stlFile * aFile,
                               smlEnv  * aEnv );
stlStatus smfLoadSysCtrlSection( smlEnv * aEnv );
stlStatus smfLoadDbCtrlSection( smlEnv * aEnv );
stlStatus smfLoadBackupCtrlSection( smlEnv * aEnv );

void smfGetSystemInfo( smfSystemInfo * aSystemInfo );
stlStatus smfDumpSysCtrlSection( stlChar * aFilePath,
                                 smlEnv  * aEnv );
stlStatus smfDumpIncBackupCtrlSection( stlChar * aFilePath,
                                       smlEnv  * aEnv );
stlStatus smfAcquireDatabaseFileLatch( smlEnv * aEnv );
stlStatus smfReleaseDatabaseFileLatch( smlEnv * aEnv );
stlStatus smfCreateDbCtrlSection( stlFile  * aFile,
                                  stlSize  * aWrittenBytes,
                                  smlEnv   * aEnv );
stlStatus smfDumpDbCtrlSection( stlChar * aFilePath,
                                smlEnv  * aEnv );
stlStatus smfWriteDbCtrlSection( stlFile  * aFile,
                                 stlSize  * aWrittenBytes,
                                 smlEnv   * aEnv );
stlStatus smfFirstTbsPersData( smfTbsPersData  * aTbsPersData,
                               void           ** aIterator,
                               smlEnv          * aEnv );
stlStatus smfNextTbsPersData( smfTbsPersData  * aTbsPersData,
                              void           ** aIterator,
                              smlEnv          * aEnv );
stlBool smfIsValidTbs( smlTbsId aTbsId );
stlBool smfIsValidDatafile( smlTbsId      aTbsId,
                            smlDatafileId aDatafileId );
stlBool smfIsRedoableDatafile( smlTbsId      aTbsId,
                               smlDatafileId aDatafileId );
stlStatus smfRenameTbs( smlStatement * aStatement,
                        smlTbsId       aTbsId,
                        stlChar      * aTbsName,
                        smlEnv       * aEnv );
stlStatus smfParallelLoad( stlInt32   aPhysicalTbsId,
                           smlEnv   * aEnv );
stlStatus smfLoadEventHandler( void      * aData,
                               stlUInt32   aDataSize,
                               stlBool   * aDone,
                               void      * aEnv );
void smfServerStateString( smlServerState   aServerState,
                           stlChar        * aString );
void smfDataStoreModeString( stlInt64    aDataStoreMode,
                             stlChar   * aString );
void smfTbsAttrString( stlUInt16   aAttr,
                       stlChar   * aString );
void smfTbsStateString( stlUInt16   aTbsState,
                        stlChar   * aString );
void smfTbsOfflineStateString( stlUInt16   aOfflineState,
                               stlChar   * aString );
void smfDatafileStateString( stlUInt16   aDatafileState,
                             stlChar   * aString );

stlStatus smfGetMinChkptLsn( smrLid  * aChkptLid,
                             smrLsn  * aMinChkptLsn,
                             smlEnv  * aEnv );

stlStatus smfGetTbsMinChkptLsn( smlTbsId     aTbsId,
                                smrLid     * aChkptLid,
                                smrLsn     * aMinChkptLsn,
                                smlEnv     * aEnv );

stlStatus smfChangeDatafileHeader( smlStatement  * aStatement,
                                   stlInt32        aReason,
                                   smlTbsId        aTbsId,
                                   smlDatafileId   aDatafileId,
                                   smrLid          aChkptLid,
                                   smrLsn          aChkptLsn,
                                   smlEnv        * aEnv );

stlStatus smfSetTablespaceOfflineState( smlTbsId   aTbsId,
                                        stlInt8    aState,
                                        smrLsn     aOfflineLsn,
                                        smlEnv   * aEnv );

inline smrLsn smfGetOfflineLsn( smlTbsId    aTbsId );

inline void smfSetOfflineLsn( smlTbsId aTbsId,
                              smrLsn   aLsn );

stlStatus smfSetBackupWaitingState( smlTbsId     aTbsId,
                                    stlBool    * aIsSuccess,
                                    smlEnv     * aEnv );

stlStatus smfResetBackupWaitingState( smlTbsId    aTbsId,
                                      smlEnv    * aEnv );

stlStatus smfWriteCheckpointLsn( stlInt32        aTbsId,
                                 smlDatafileId   aDatafileId,
                                 smrLsn          aLastChkptLsn,
                                 smlEnv        * aEnv );

stlStatus smfDumpDatafileHeader( stlChar * aFilePath,
                                 smlEnv  * aEnv );

stlStatus smfDumpDatafilePage( stlChar  * aFilePath,
                               stlInt64   aPageSeqId,
                               stlInt64   aFetchCnt,
                               smlEnv   * aEnv );

stlStatus smfDumpIncBackupFile( stlChar * aFilePath,
                                stlInt8   aDumpBody,
                                stlUInt16 aTbsId,
                                stlInt64  aPageSeqId,
                                stlInt64  aFetchCnt,
                                smlEnv  * aEnv );

stlStatus smfCheckDatafile4IncompleteMediaRecovery( smrLsn   aLsn,
                                                    smlEnv * aEnv );

stlStatus smfExistTablespace( stlInt64   aTbsId,
                              smlEnv   * aEnv );

stlStatus smfGetDatafileHeader( smlTbsId         aTbsId,
                                smlDatafileId    aDatafileId,
                                void           * aBuffer,
                                smlEnv         *  aEnv );

stlStatus smfBackup( smlBackupMessage * aBackupMsg,
                     smlEnv           * aEnv );

stlStatus smfRestoreDatafiles( smlTbsId   aTbsId,
                               smlEnv   * aEnv );

stlStatus smfRestoreDatafile4MediaRecovery( smrMediaRecoveryInfo * aRecoveryInfo,
                                            smlEnv               * aEnv );

stlStatus smfMergeBackup( smlTbsId   aTbsId,
                          stlUInt8   aUntilType,
                          stlInt64   aUntilValue,
                          smlEnv   * aEnv );

stlStatus smfBackupCtrlFile( stlUInt8   aBackupType,
                             stlChar  * aTarget,
                             smlEnv   * aEnv );

stlStatus smfGetFirstIncBackup( smfBackupSegment  * aBackupSegment,
                                smfBackupChunk   ** aBackupChunk,
                                smfBackupIterator * aBackupIterator,
                                stlBool           * aExistBackup,
                                smlEnv            * aEnv );
stlStatus smfGetNextIncBackup( smfBackupSegment  * aBackupSegment,
                               smfBackupChunk   ** aBackupChunk,
                               smfBackupIterator * aBackupIterator,
                               stlBool           * aExistBackup,
                               smlEnv            * aEnv );

stlStatus smfGetFirstIncBackup4Disk( stlFile           * aFile,
                                     smfSysPersData    * aSysPersData,
                                     smfBackupIterator * aBackupIterator,
                                     stlChar           * aBackupChunk,
                                     stlBool           * aExistBackup,
                                     smlEnv            * aEnv );

stlStatus smfGetNextIncBackup4Disk( stlFile           * aFile,
                                    smfSysPersData    * aSysPersData,
                                    smfBackupIterator * aBackupIterator,
                                    stlChar           * aBackupChunk,
                                    stlBool           * aExistBackup,
                                    smlEnv            * aEnv );

stlStatus smfWriteBackupFileHeader( stlFile                * aFile,
                                    smfIncBackupFileHeader * aFileHeader,
                                    stlSize                * aWrittenBytes,
                                    smlEnv                 * aEnv );

stlStatus smfGetAbsBackupFileName( stlChar   * aFileName,
                                   stlInt32    aBackupDirIdx,
                                   stlChar   * aAbsFileName,
                                   smlEnv    * aEnv );

stlStatus smfMakeBackupFileName( smlTbsId                 aTbsId,
                                 stlInt16                 aBackupLevel,
                                 smfIncBackupObjectType   aBackupObjectType,
                                 stlInt64                 aBackupSeq,
                                 stlChar                * aFileName,
                                 stlChar                * aAbsFileName,
                                 stlTime                * aBeginTime,
                                 smlEnv                 * aEnv );

stlStatus smfGetTbsMinRestoredChkptInfo( smlTbsId    aTbsId,
                                         smrLsn    * aRestoredChkptLsn,
                                         smrLid    * aRestoredChkptLid,
                                         smlEnv    * aEnv );

void smfSetPrevBackupInfo( smlTbsId   aTbsId,
                           stlInt16   aBackupLevel,
                           smrLsn     aBackupLsn );

void smfInitIncrementalBackupInfo( smlTbsId   aTbsId );

smrLsn smfGetRestoredLsn( void  * aDatafileInfo );

stlInt64 smfGetTotalBackupCount( smfBackupSegment * aBackupSegment );

stlStatus smfDeleteBackupList( stlUInt8   aTarget,
                               stlBool    aDeleteFile,
                               smlEnv   * aEnv );
stlStatus smfDeleteAllBackupList( smlEnv  * aEnv );

stlStatus smfRestoreCtrlFile( stlChar * aBackupCtrlFile,
                              smlEnv  * aEnv );

stlStatus smfValidateTbs( smlEnv * aEnv );

stlStatus smfReadBackupFileHeader( stlFile                * aFile,
                                   smfIncBackupFileHeader * aFileHeader,
                                   smlEnv                 * aEnv );

stlStatus smfReadBackupFileTail( stlFile              * aFile,
                                 stlOffset              aOffset,
                                 stlSize                aTailSize,
                                 smfIncBackupFileTail * aFileTail,
                                 smlEnv               * aEnv );

stlStatus smfVerifyDatabaseCorruption( smlEnv  * aEnv );
stlStatus smfIsCorruptedTbs( smlTbsId   aTbsId,
                             stlBool  * aIsCorrupted,
                             smlEnv   * aEnv );
stlStatus smfVerifyTbsCorruption( smlTbsId  aTbsId,
                                  smlEnv  * aEnv );

stlStatus smfExistValidIncBackup( smlTbsId   aTbsId,
                                  stlBool  * aExist,
                                  smlEnv   * aEnv );

stlStatus smfValidateOfflineTablespace( smlOfflineBehavior   aOfflineBehavior,
                                        smlEnv             * aEnv );

stlStatus smfValidateOfflineTbsConsistency( smlEnv   * aEnv );

stlStatus smfRegisterDbFiles( smlEnv * aEnv );

stlStatus smfValidateCtrlFileChecksum( stlFile        * aFile,
                                       smfSysPersData * aSysPersData,
                                       stlBool        * aIsValid,
                                       smlEnv         * aEnv );

stlStatus smfSaveCtrlFile4Syncher( void      * aSmfWarmupEntry,
                                   void      * aSmrWarmupEntry,
                                   stlBool     aSilent,
                                   stlLogger * aLogger,
                                   smlEnv    * aEnv );

stlStatus smfValidateMediaRecoveryDatafile( smlTbsId   aTbsId,
                                            smrLsn     aLastLsn,
                                            stlUInt8   aRecoveryObject,
                                            smlEnv   * aEnv);

/** @} */
    
#endif /* _SMF_H_ */
