/*******************************************************************************
 * kniStartup.h
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


#ifndef _KNISTARTUP_H_
#define _KNISTARTUP_H_ 1

/**
 * @file kniStartup.h
 * @brief Kernel Layer Startup Internal Routines
 */

/**
 * @defgroup kniStartup Startup
 * @ingroup knInternal
 * @internal
 * @{
 */

stlStatus kniStartupNoMount( knlStartupInfo  * aStartupInfo,
                             void            * aEnv );
stlStatus kniStartupMount( knlStartupInfo  * aStartupInfo,
                           void            * aEnv );
stlStatus kniStartupPreOpen( knlStartupInfo  * aStartupInfo,
                             void            * aEnv );
stlStatus kniWarmup( void * aEnv );
stlStatus kniCooldown( void * aEnv );
stlStatus kniShutdownInit( knlShutdownInfo * aShutdownInfo,
                           void            * aEnv );
stlStatus kniShutdownDismount( knlShutdownInfo * aShutdownInfo,
                               void            * aEnv );
stlStatus kniShutdownPostClose( knlShutdownInfo *  aShutdownInfo,
                                void            * aEnv );
stlStatus kniBootdown( void * aEnv );
void kniFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext );
void kniSignalHandler( stlInt32   aSigNo,
                       void     * aSigInfo,
                       void     * aContext );
void kniSystemFatal( stlBool aDestroyShm,
                     stlBool aMakeCore );
stlStatus kniInitializeNumaInfo( kniNumaInfo * aNumaInfo,
                                 knlEnv      * aEnv );

stlStatus kniRegisterDbFiles( knlEnv * aEnv );

stlStatus kniRegisterFxTables( knlEnv * aEnv );

stlStatus openFxInstanceCallback( void   * aStmt,
                                  void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv );
stlStatus closeFxInstanceCallback( void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv );
stlStatus buildRecordFxInstanceCallback( void              * aDumpObjHandle,
                                         void              * aPathCtrl,
                                         knlValueBlockList * aValueList,
                                         stlInt32            aBlockIdx,
                                         stlBool           * aTupleExist,
                                         knlEnv            * aEnv );

stlStatus kniOpenSystemInfoFxTableCallback( void   * aStmt,
                                            void   * aDumpObjHandle,
                                            void   * aPathCtrl,
                                            knlEnv * aKnlEnv );
stlStatus kniCloseSystemInfoFxTableCallback( void   * aDumpObjHandle,
                                             void   * aPathCtrl,
                                             knlEnv * aKnlEnv );
stlStatus kniBuildRecordSystemInfoFxTableCallback( void              * aDumpObjHandle,
                                                   void              * aPathCtrl,
                                                   knlValueBlockList * aValueList,
                                                   stlInt32            aBlockIdx,
                                                   stlBool           * aTupleExist,
                                                   knlEnv            * aKnlEnv );

/** @} */
    
#endif /* _KNISTARTUP_H_ */
