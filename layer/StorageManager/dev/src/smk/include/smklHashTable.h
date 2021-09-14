/*******************************************************************************
 * smklHashTable.h
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


#ifndef _SMKLHASHTABLE_H_
#define _SMKLHASHTABLE_H_ 1

/**
 * @file smklHashTable.h
 * @brief Storage Manager Layer Lock Hash Table Internal Routines
 */

/**
 * @defgroup smklHashTable Lock Hash Table
 * @ingroup smInternal
 * @{
 */
stlStatus smklInitHashTable( smlEnv * aEnv );
stlStatus smklDestHashTable( smlEnv * aEnv );
stlUInt32 smklGetHashValue( smlRid aRid );
smklHashBucket * smklGetHashBucket( smlRid aRid );
smklItemRecord * smklFindHashItem( smklHashBucket * aHashBucket,
                                   smlRid           aRid );
void smklAddHashItem( smklHashBucket * aHashBucket,
                      smklItemRecord * aLockItem );
stlStatus smklRemoveHashItem( smklItemRecord * aLockItem,
                              smlEnv         * aEnv );

/** @} */
    
#endif /* _SMKLHASHTABLE_H_ */
