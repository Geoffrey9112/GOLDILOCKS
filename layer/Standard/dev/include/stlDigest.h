/*******************************************************************************
 * stlDigest.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stlDigest.h 1533 2011-07-26 05:37:58Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _STL_DIGEST_H_
#define _STL_DIGEST_H_ 1

/**
 * @file stlDigest.h
 * @brief Standard Layer Digest Routines
 */

#include <stlDef.h>


STL_BEGIN_DECLS


/**
 * @defgroup stlDigest Digest
 * @ingroup STL 
 * @{
 */

stlStatus stlDigest( stlUInt8           * aValue,
                     stlUInt64            aValueLen,
                     stlUInt8             aResult[STL_DIGEST_SIZE],
                     stlErrorStack      * aErrorStack );


stlStatus stlMakeEncryptedPassword( stlChar       * aPlainText,
                                    stlInt32        aPlainTextLen,
                                    stlChar       * aEncryptedText,
                                    stlInt32        aEncryptedBufferSize,
                                    stlErrorStack * aErrorStack );

stlStatus stlCrc32( void          * aData,
                    stlInt32        aSize,
                    stlUInt32     * aCrc32,
                    stlErrorStack * aErrorStack );

/** @} */
    
STL_END_DECLS

#endif /* _STL_DIGEST_H_ */
