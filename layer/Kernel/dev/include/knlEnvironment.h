/*******************************************************************************
 * knlEnvironment.h
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


#ifndef _KNLENVIRONMENT_H_
#define _KNLENVIRONMENT_H_ 1

/**
 * @file knlEnvironment.h
 * @brief Kernel Layer Environment Routines
 */

/**
 * @defgroup knlEnvironment Environment
 * @ingroup knExternal
 * @{
 */

stlStatus knlCreateEnvPool( stlUInt32       aEnvCount,
                            stlUInt32       aEnvSize,                            
                            stlErrorStack * aErrorStack );

stlStatus knlDestroyEnvPool( stlErrorStack * aErrorStack );
    
stlStatus knlAllocEnv( void           ** aEnv,
                       stlErrorStack   * aErrorStack );

stlStatus knlFreeEnv( void           * aEnv,
                      stlErrorStack  * aErrorStack );

stlStatus knlInitializeEnv( knlEnv     * aEnv,
                            knlEnvType   aEnvType );

stlStatus knlFinalizeEnv( knlEnv * aEnv );

void knlSetRequestGroupId( stlInt32   aRequestGroupId,
                           knlEnv   * aEnv );

stlStatus knlBindNumaNode( knlEnv * aEnv );
stlStatus knlBindNumaMasterNode( knlEnv * aEnv );
stlStatus knlUnbindNumaNode( stlBool   aIsCleanup,
                             knlEnv  * aEnv );

stlStatus knlCleanupHeapMem( knlEnv * aEnv );

void * knlGetFirstEnv();
void * knlGetLastEnv();
void * knlGetNextEnv( void * aEnv );
void * knlGetFirstMyProcessEnv();
void * knlGetNextMyProcessEnv( knlEnv * aEnv );

void * knlGetEnv( stlUInt32 aEnvId );

void * knlGetMyEnv();

stlBool knlIsEqualEnv( void * aEnv1, void * aEnv2 );

stlStatus knlGetEnvId( void      * aEnv,
                       stlUInt32 * aId );

void knlGetEnvStateString( knlEnv   * aEnv,
                           stlChar  * aStateString,
                           stlInt16   aBufferLength );
stlBool knlIsUsedEnv( void * aEnv );

void knlSetTopLayer( stlLayerClass    aLayer,
                     knlEnv         * aEnv );
stlInt16 knlGetTopLayer( knlEnv * aEnv );

stlStatus openFxKnEnvCallback( void   * aStmt,
                               void   * aDumpObjHandle,
                               void   * aPathCtrl,
                               knlEnv * aEnv );

stlStatus closeFxKnEnvCallback( void   * aDumpObjHandle,
                                void   * aPathCtrl,
                                knlEnv * aEnv );

stlStatus buildRecordFxKnEnvCallback( void              * aDumpObjHandle,
                                      void              * aPathCtrl,
                                      knlValueBlockList * aValueList,
                                      stlInt32            aBlockIdx,
                                      stlBool           * aTupleExist,
                                      knlEnv            * aEnv );

stlStatus openFxKnProcStatCallback( void   * aStmt,
                                    void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv );

stlStatus closeFxKnProcStatCallback( void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv );

stlStatus buildRecordFxKnProcStatCallback( void              * aDumpObjHandle,
                                           void              * aPathCtrl,
                                           knlValueBlockList * aValueList,
                                           stlInt32            aBlockIdx,
                                           stlBool           * aTupleExist,
                                           knlEnv            * aEnv );

/** @} */
    
#endif /* _KNLENVIRONMENT_H_ */
