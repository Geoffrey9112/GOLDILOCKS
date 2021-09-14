/*******************************************************************************
 * smqManager.h
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


#ifndef _SMQMANAGER_H_
#define _SMQMANAGER_H_ 1

/**
 * @file smqManager.h
 * @brief Storage Manager Layer Sequence Manager Internal Routines
 */

/**
 * @defgroup smqManager Sequence Manager
 * @ingroup smqInternal
 * @{
 */

stlStatus smqGetNextValPositiveCycle( smqCache * aCache,
                                      smlEnv   * aEnv );

stlStatus smqGetNextValNegativeCycle( smqCache * aCache,
                                      smlEnv   * aEnv );

stlStatus smqGetNextValPositiveNocycle( smqCache * aCache,
                                        smlEnv   * aEnv );

stlStatus smqGetNextValNegativeNocycle( smqCache * aCache,
                                        smlEnv   * aEnv );

stlStatus smqCreateSeqInternal( smlStatement     * aStatement,
                                smqCache         * aCache,
                                stlInt64         * aSequenceId,
                                smlEnv           * aEnv );
/** @} */
    
#endif /* _SMQMANAGER_H_ */
