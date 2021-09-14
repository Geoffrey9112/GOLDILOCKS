/*******************************************************************************
 * smg.h
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


#ifndef _SMG_H_
#define _SMG_H_ 1

/**
 * @file smg.h
 * @brief Storage Manager Layer General Component Internal Routines
 */

/**
 * @defgroup smg General
 * @ingroup smInternal
 * @{
 */

stlStatus smgStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv );
stlStatus smgStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv );
stlStatus smgStartupPreOpen( knlStartupInfo * aStartupInfo,
                             void           * aEnv );
stlStatus smgStartupOpen( knlStartupInfo * aStartupInfo,
                          void           * aEnv );
stlStatus smgMediaRecover( smlMediaRecoveryInfo * aMediaRecoveryInfo,
                           void                 * aEnv );
stlStatus smgWarmup( void * aEnv );
stlStatus smgCooldown( void * aEnv );
stlStatus smgShutdownInit( knlShutdownInfo * aShutdownInfo,
                           void            * aEnv );
stlStatus smgShutdownDismount( knlShutdownInfo * aShutdownInfo,
                               void            * aEnv );
stlStatus smgShutdownPostClose( knlShutdownInfo * aShutdownInfo,
                                void            * aEnv );
stlStatus smgShutdownClose( knlShutdownInfo * aShutdownInfo,
                            void            * aEnv );
stlStatus smgRegisterFxTables( void * aEnv );

stlStatus smgEscalateWarmupPhase( void      * aData,
                                  stlUInt32   aDataSize,
                                  stlBool   * aDone,
                                  void      * aEnv );

void smgRegisterPendOp( smgPendOpId   aPendOpId,
                        smgPendOpFunc aFunc );
stlStatus smgAddPendOp( smlStatement  * aStatement,
                        smgPendOpId     aPendOpId,
                        stlUInt32       aActionFlag,
                        void          * aArgs,
                        stlUInt32       aArgsByteSize,
                        stlUInt32       aEventByteSize,
                        smlEnv        * aEnv );
stlStatus smgExecutePendOp( stlUInt32    aActionFlag,
                            stlInt64     aTimestamp,
                            smlEnv     * aEnv );

stlStatus smgAllocShmMem4Open( stlSize    aSize,
                               void    ** aAddr,
                               smlEnv   * aEnv );
stlStatus smgFreeShmMem4Open( void   * aAddr,
                              smlEnv * aEnv );
stlStatus smgAllocShmMem4Mount( stlSize    aSize,
                                void    ** aAddr,
                                smlEnv   * aEnv );
stlStatus smgFreeShmMem4Mount( void   * aAddr,
                               smlEnv * aEnv );
stlStatus smgAllocSessShmMem( stlSize    aSize,
                              void     **aAddr,
                              smlEnv    *aEnv );
stlStatus smgFreeSessShmMem( void   *aAddr,
                             smlEnv *aEnv );

stlUInt32 smgGetCheckpointEnvId();
stlUInt32 smgGetLogFlusherEnvId();
stlUInt32 smgGetPageFlusherEnvId();
stlUInt32 smgGetAgerEnvId();
stlUInt32 smgGetCleanupEnvId();
stlUInt32 smgGetArchivelogEnvId();
stlUInt32 smgGetIoSlaveEnvId( stlInt64 aThreadIdx );

void smgSetCheckpointEnvId( smlEnv * aEnv );
void smgSetLogFlusherEnvId( smlEnv * aEnv );
void smgSetPageFlusherEnvId( smlEnv * aEnv );
void smgSetAgerEnvId( smlEnv * aEnv );
void smgSetCleanupEnvId( smlEnv * aEnv );
void smgSetArchivelogEnvId( smlEnv * aEnv );
void smgSetIoSlaveEnvId( stlInt64   aThreadIdx,
                         smlEnv   * aEnv );
stlInt16 smgGetAgerState();
void smgSetAgerState( stlInt16 aState );

stlStatus smgDeactivateAgerEventHandler( void      * aData,
                                         stlUInt32   aDataSize,
                                         stlBool   * aDone,
                                         void      * aEnv );
stlStatus smgWakeupLogFlusherEventHandler( void      * aData,
                                           stlUInt32   aDataSize,
                                           stlBool   * aDone,
                                           void      * aEnv );
stlStatus smgLoopbackEventHandler( void      * aData,
                                   stlUInt32   aDataSize,
                                   stlBool   * aDone,
                                   void      * aEnv );
stlStatus smgMediaRecoverEventHandler( void      * aData,
                                       stlUInt32   aDataSize,
                                       stlBool   * aDone,
                                       void      * aEnv );
stlStatus smgRestoreEventHandler( void      * aData,
                                  stlUInt32   aDataSize,
                                  stlBool   * aDone,
                                  void      * aEnv );

void smgFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext );

stlStatus smgOpenFile( stlFile        * aNewFile, 
                       const stlChar  * aFileName, 
                       stlInt32         aFlag, 
                       stlInt32         aPerm,
                       smlEnv         * aEnv );
stlStatus smgCloseFile( stlFile * aFile,
                        smlEnv  * aEnv );

stlStatus smgStartupPreOpenBeforeMediaRecovery( void  * aEnv );
stlStatus smgShutdownPostCloseAfterMediaRecovery( void  * aEnv );

stlStatus smgPrepareMediaRecoveryAtMount( smlMediaRecoveryInfo    * aMediaRecoveryInfo,
                                          stlUInt32                 aDataSize,
                                          smlDatafileRecoveryInfo * aDatafileRecoveryInfo,
                                          smlEnv                  * aEnv );

stlStatus smgPrepareMediaRecoveryAtOpen( smlRecoveryType   aRecoveryType,
                                         smlTbsId          aTbsId,
                                         knlRegionMark   * aMemMark,
                                         smlEnv          * aEnv );

stlStatus smgFinalizeMediaRecoveryAtMount( smlEnv  * aEnv );

stlStatus smgFinalizeMediaRecoveryAtOpen( knlRegionMark  * aMemMark,
                                          smlEnv         * aEnv );
stlStatus smgRegisterDbFiles( smlEnv * aEnv );

/** @} */
    
#endif /* _SMG_H_ */
