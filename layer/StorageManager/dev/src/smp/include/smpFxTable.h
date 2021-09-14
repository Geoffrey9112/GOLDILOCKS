/*******************************************************************************
 * smpFxTable.h
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


#ifndef _SMPFXTABLE_H_
#define _SMPFXTABLE_H_ 1

/**
 * @file smpFxTable.h
 * @brief Page Access Fixed Table Component Internal Routines
 */

/**
 * @defgroup smpFxTable Page Access Fixed Table
 * @ingroup smpInternal
 * @{
 */

stlStatus smpOpenLatchClass();

stlStatus smpGetNextLatchAndDesc( knlLatch ** aLatch, 
                                  stlChar   * aBuf,
                                  void      * aPrevPosition,
                                  knlEnv    * aEnv );

stlStatus smpCloseLatchClass();

/** @} */
    
#endif /* _SMPFXTABLE_H_ */
