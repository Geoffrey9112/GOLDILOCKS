/*******************************************************************************
 * smsmbFxTable.h
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


#ifndef _SMSMBFXTABLE_H_
#define _SMSMBFXTABLE_H_ 1

/**
 * @file smsmbFxTable.h
 * @brief Storage Manager Layer Memory Bitmap Fixed Table Component Internal Routines
 */

/**
 * @defgroup smsmbFxTable Memory Bitmap Fixed Table
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmbPushChildPage( smsmbFxBitmapIterator * aIterator,
                              smsmbFxBitmap         * aFxBitmap,
                              smlEnv                * aEnv );
void smsmbPushFxBitmap( smsmbFxBitmapIterator * aIterator,
                        smsmbFxBitmap           aFxBitmap );
smsmbFxBitmap * smsmbPopFxBitmap( smsmbFxBitmapIterator * aIterator );
stlStatus smsmbBuildBitmapHdr( smsmbFxBitmapHdrIterator * aIterator,
                               smsmbFxBitmapHdr         * aFxBitmapHdr,
                               smlEnv                   * aEnv );
stlStatus smsmbPushChildPageHdr( smsmbFxBitmapHdrIterator * aIterator,
                                 smsmbFxBitmapHdr         * aFxBitmapHdr,
                                 smlEnv                   * aEnv );
void smsmbPushFxBitmapHdr( smsmbFxBitmapHdrIterator * aIterator,
                           smsmbFxBitmapHdr           aFxBitmapHdr );
smsmbFxBitmapHdr * smsmbPopFxBitmapHdr( smsmbFxBitmapHdrIterator * aIterator );

/** @} */
    
#endif /* _SMSMBFXTABLE_H_ */
