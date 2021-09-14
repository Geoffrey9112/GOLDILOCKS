/*******************************************************************************
 * knt.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knt.h 7850 2013-03-28 01:27:42Z htkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _KNT_H_
#define _KNT_H_ 1

/**
 * @file knt.h
 * @brief XA related hash internal prototypes
 */

stlStatus kntInitHashTable( knlEnv * aEnv );

kntHashBucket * kntGetHashBucket( stlInt32 aHashValue );

knlXaContext * kntFindHashItem( kntHashBucket * aHashBucket,
                                stlInt32        aHashValue,
                                stlXid        * aXid );

#endif /* _KNT_H_ */
