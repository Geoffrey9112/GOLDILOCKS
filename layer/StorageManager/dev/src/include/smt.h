/*******************************************************************************
 * smt.h
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


#ifndef _SMT_H_
#define _SMT_H_ 1

/**
 * @file smt.h
 * @brief Storage Manager Layer Tablespace Internal Routines
 */

/**
 * @defgroup smt Tablespace
 * @ingroup smInternal
 * @{
 */

stlStatus smtStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv );
stlStatus smtStartupOpen( smlEnv * aEnv );
stlStatus smtWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv );
stlStatus smtCooldown( smlEnv * aEnv );
stlStatus smtBootdown( smlEnv * aEnv );
stlStatus smtShutdownClose( smlEnv * aEnv );
stlStatus smtRegisterFxTables( smlEnv * aEnv );

stlStatus smtCreateTbs( smlStatement * aStatement,
                        smlTbsAttr   * aTbsAttr,
                        stlInt32       aReservedPageCount,
                        stlBool        aUndoLogging,
                        smlTbsId     * aTbsId,
                        smlEnv       * aEnv );
stlStatus smtDropTbs( smlStatement * aStatement,
                      smlTbsId       aTbsId,
                      stlBool        aAndDatafiles,
                      smlEnv       * aEnv );
stlStatus smtAddDatafile( smlStatement    * aStatement,
                          smlTbsId          aTbsId,
                          smlDatafileAttr * aDatafileAttr,
                          smlEnv          * aEnv );
stlStatus smtDropDatafile( smlStatement * aStatement,
                           smlTbsId       aTbsId,
                           stlChar      * aDatafileName,
                           smlEnv       * aEnv );

stlStatus smtAllocExtent( smxmTrans  * aMiniTrans,
                          smlTbsId     aTbsId,
                          smlExtId   * aExtId,
                          smlPid     * aFirstDataPid,
                          smlEnv     * aEnv );
stlStatus smtFreeExtent( smxmTrans  * aMiniTrans,
                         smlTbsId     aTbsId,
                         smlExtId     aExtId,
                         stlBool    * aIsSuccess,
                         smlEnv     * aEnv );
stlStatus smtCreateTbsHint( smlSessionEnv * aSessionEnv,
                            smlEnv        * aEnv );

stlBool smtIsLogging( smlTbsId aTbsId );
stlStatus smtAddExtBlockPending( stlUInt32   aActionFlag,
                                 smgPendOp * aPendOp,
                                 smlEnv    * aEnv );

stlStatus smtAddDatafileEventHandler( void      * aData,
                                      stlUInt32   aDataSize,
                                      stlBool   * aDone,
                                      void      * aEnv );

stlStatus smtOfflineTablespaceEventHandler( void      * aData,
                                            stlUInt32   aDataSize,
                                            stlBool   * aDone,
                                            void      * aEnv );

stlStatus smtRenameDatafileEventHandler( void      * aData,
                                         stlUInt32   aDataSize,
                                         stlBool   * aDone,
                                         void      * aEnv );

stlStatus smtDropUnregisteredTbs( smlTbsId   aTbsId,
                                  smlEnv   * aEnv );

/** @} */
    
#endif /* _SMT_H_ */
