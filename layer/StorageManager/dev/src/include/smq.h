/*******************************************************************************
 * smq.h
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


#ifndef _SMQ_H_
#define _SMQ_H_ 1

/**
 * @file smq.h
 * @brief Storage Manager Layer Sequence Internal Routines
 */

/**
 * @defgroup smq Sequence
 * @ingroup smInternal
 * @{
 */

stlStatus smqMapCreate( smlStatement * aStatement,
                        smlEnv       * aEnv );
stlStatus smqCreateSequence( smlStatement     * aStatement,
                             smlSequenceAttr  * aAttr,
                             stlInt64         * aSequenceId,
                             void            ** aSequenceHandle,
                             smlEnv           * aEnv );
stlStatus smqAlterSequence( smlStatement     * aStatement,
                            void             * aOrgSeqHandle,
                            smlSequenceAttr  * aAttr,
                            stlInt64         * aNewSeqId,
                            void            ** aNewSeqHandle,
                            smlEnv           * aEnv );
stlStatus smqDropSequence( smlStatement * aStatement,
                           void         * aSequenceHandle,
                           smlEnv       * aEnv );
stlStatus smqDropPending( stlUInt32   aActionFlag,
                          smgPendOp * aPendOp,
                          smlEnv    * aEnv );
stlStatus smqDropAging( void      * aData,
                        stlUInt32   aDataSize,
                        stlBool   * aDone,
                        void      * aEnv );
stlStatus smqGetNextVal( void      * aSequenceHandle,
                         stlInt64  * aNextValue,
                         smlEnv    * aEnv );
stlStatus smqGetCurrSessVal( void      * aSequenceHandle,
                             stlInt64  * aCurrValue,
                             smlEnv    * aEnv );
stlStatus smqGetCurrVal( void      * aSequenceHandle,
                         stlInt64  * aCurrValue,
                         smlEnv    * aEnv );
stlStatus smqSetCurrVal( void      * aSequenceHandle,
                         stlInt64    aCurrValue,
                         smlEnv    * aEnv );
stlStatus smqBuildCachesAtStartup( smlEnv * aEnv );
void smqFillSequenceAttr( void            * aSequenceHandle,
                          smlSequenceAttr * aAttr );
stlStatus smqGetSequenceHandle( smlRid      aSequenceRid,
                                void     ** aSequenceHandle,
                                smlEnv    * aEnv );

stlStatus smqStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv );
stlStatus smqStartupOpen( smlEnv * aEnv );
stlStatus smqWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv );
stlStatus smqCooldown( smlEnv * aEnv );
stlStatus smqBootdown( smlEnv * aEnv );
stlStatus smqShutdownClose( smlEnv * aEnv );
stlStatus smqRegisterFxTables( smlEnv * aEnv );

/** @} */
    
#endif /* _SMQ_H_ */
