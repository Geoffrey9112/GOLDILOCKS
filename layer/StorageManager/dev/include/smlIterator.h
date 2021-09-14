/*******************************************************************************
 * smlIterator.h
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


#ifndef _SMLITERATOR_H_
#define _SMLITERATOR_H_ 1

/**
 * @file smlIterator.h
 * @brief Storage Manager Iterator Routines
 */

/**
 * @defgroup smlIterator Iterator
 * @ingroup smExternal
 * @{
 */

stlStatus smlEnterIterator( smlEnv * aEnv );

stlStatus smlLeaveIterator( smlEnv * aEnv );

stlStatus smlResetIterator( void    * aIterator,
                            smlEnv  * aEnv );

stlStatus smlFetchNext( void          * aIterator,
                        smlFetchInfo  * aFetchInfo,
                        smlEnv        * aEnv );

stlStatus smlFetchPrev( void          * aIterator,
                        smlFetchInfo  * aFetchInfo,
                        smlEnv        * aEnv );

stlStatus smlMoveToRowId( void   * aIterator,
                          smlRid * aRowRid,
                          smlEnv * aEnv );

stlStatus smlMoveToPosNum( void   * aIterator,
                           stlInt64 aPosNum,
                           smlEnv * aEnv );

stlStatus smlMoveToNextKey( void          * aIterator,
                            smlFetchInfo  * aFetchInfo,
                            smlEnv        * aEnv );

stlStatus smlResetGroup( void    * aIterator,
                         smlEnv  * aEnv );

/** @} */

#endif /* _SMLITERATOR_H_ */
