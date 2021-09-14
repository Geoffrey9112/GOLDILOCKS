/*******************************************************************************
 * smlSequence.h
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


#ifndef _SMLSEQUENCE_H_
#define _SMLSEQUENCE_H_ 1

/**
 * @file smlSequence.h
 * @brief Storage Manager Sequence Routines
 */

/**
 * @defgroup smlSequence Sequence
 * @ingroup smExternal
 * @{
 */

stlStatus smlCreateSequence( smlStatement     * aStatement,
                             smlSequenceAttr  * aAttr,
                             stlInt64         * aSequenceId,
                             void            ** aSequenceHandle,
                             smlEnv           * aEnv );
stlStatus smlAlterSequence( smlStatement     * aStatement,
                            void             * aOrgSeqHandle,
                            smlSequenceAttr  * aAttr,
                            stlInt64         * aNewSeqId,
                            void            ** aNewSeqHandle,
                            smlEnv           * aEnv );
stlStatus smlDropSequence( smlStatement * aStatement,
                           void         * aSequenceHandle,
                           smlEnv       * aEnv );
stlStatus smlGetNextSequenceVal( void      * aSequenceHandle,
                                 stlInt64  * aNextValue,
                                 smlEnv    * aEnv );
stlStatus smlGetCurrSessSequenceVal( void      * aSequenceHandle,
                                     stlInt64  * aCurrValue,
                                     smlEnv    * aEnv );
stlStatus smlGetCurrSequenceVal( void      * aSequenceHandle,
                                 stlInt64  * aCurrValue,
                                 smlEnv    * aEnv );
stlStatus smlSetCurrSequenceVal( void      * aSequenceHandle,
                                 stlInt64    aCurrValue,
                                 smlEnv    * aEnv );
stlStatus smlGetSequenceAttr( void            * aSequenceHandle,
                              smlSequenceAttr * aSequenceAttr,
                              smlEnv          * aEnv );

void smlFillDefaultSequenceAttr( smlSequenceAttr * aAttr );

stlStatus smlGetSequenceHandle( stlInt64    aSequenceId,
                                void     ** aSequenceHandle,
                                smlEnv    * aEnv );

/** @} */

#endif /* _SMLSEQUENCE_H_ */
