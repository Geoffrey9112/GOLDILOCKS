/*******************************************************************************
 * strDigest.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: strDigest.h 350 2011-03-21 07:38:42Z lym999 $
 *
 * NOTES
 *    SHA1 알고리즘으로 digest 기능 구현.
 *    NO COPYRIGHT - THIS IS 100% IN THE PUBLIC DOMAIN
 *    ftp://ftp.funet.fi/pub/crypt/hash/sha/sha1.c
 *
 ******************************************************************************/


#ifndef _STRDIGEST_H_
#define _STRDIGEST_H_ 1

/**
 * @file strDigest.h
 * @brief Digest for Standard Layer
 */

/*
 * Be sure to get the above definitions right.  For instance, on my
 * x86 based FreeBSD box, I define LITTLE_ENDIAN and use the type
 * "unsigned long" for the quadbyte.  On FreeBSD on the Alpha, however,
 * while I still use LITTLE_ENDIAN, I must define the quadbyte type
 * as "unsigned int" instead.
 */

#define SHA1_BLOCK_LENGTH   64
#define SHA1_DIGEST_LENGTH  20


/* The SHA1 structure: */
typedef struct strSha1Ctx {
    stlUInt32   mState[5];
    stlUInt32   mCount[2];
    stlUInt8    mBuffer[SHA1_BLOCK_LENGTH];
} strSha1Ctx;


#define STR_DIGEST_SIZE     (STL_DIGEST_SIZE)

stlStatus strSha1Init( strSha1Ctx         * aSha1,
                       stlErrorStack      * aErrorStack );

stlStatus strSha1Update( strSha1Ctx         * aSha1,
                         stlUInt8           * aValue,
                         stlUInt64          aValueLen,
                         stlErrorStack      * aErrorStack );

stlStatus strSha1Final( strSha1Ctx          * aSha1,
                        stlUInt8              aResult[STR_DIGEST_SIZE],
                        stlErrorStack       * aErrorStack );

stlStatus strDigest( stlUInt8           * aValue,
                     stlUInt64            aValueLen,
                     stlUInt8             aResult[STR_DIGEST_SIZE],
                     stlErrorStack      * aErrorStack );

stlStatus strCrc32( void          * aData,
                    stlInt32        aSize,
                    stlUInt32     * aCrc32,
                    stlErrorStack * aErrorStack );

#endif  /* ! _STRDIGEST_H_ */
