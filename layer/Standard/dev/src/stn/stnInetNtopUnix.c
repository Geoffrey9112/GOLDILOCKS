/*******************************************************************************
 * stnInetNtopUnix.c
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

#include "stnUnix.h"

#ifndef IN6ADDRSZ
#define IN6ADDRSZ   16
#endif

#ifndef INT16SZ
#define INT16SZ STL_SIZEOF(stlInt16)
#endif

#ifndef __P
#define __P(x) x
#endif

#if !defined(EAFNOSUPPORT) && defined(WSAEAFNOSUPPORT)
#define EAFNOSUPPORT WSAEAFNOSUPPORT
#endif

/*
 * WARNING: Don't even consider trying to compile this on a system where
 * sizeof(int) < 4.  sizeof(int) > 4 is fine; all the world's not a VAX.
 */

static stlStatus stnGetStrAddrFromBin4 __P(( const stlChar * aSrc,
                                    stlChar       * aDst,
                                    stlSize         aSize,
                                    stlErrorStack * aErrorStack ));
#if STC_HAVE_IPV6
static stlStatus stnGetStrAddrFromBin6 __P(( const stlChar * aSrc,
                                    stlChar       * aDst,
                                    stlSize         aSize,
                                    stlErrorStack * aErrorStack ));
#endif

/* stlStatus 
 * stnGetStrAddrFromBin(aAf, aSrc, aDst, aSize)
 *      convert a network format address to presentation format.
 * return:
 *      stlStatus with error code(if exists).
 */
stlStatus stnGetStrAddrFromBin( stlInt32        aAf,
                                const void    * aSrc,
                                stlChar       * aDst,
                                stlSize         aSize,
                                stlErrorStack * aErrorStack )
{
    stlChar sMsg[64];

    STL_PARAM_VALIDATE( aSrc != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aDst != NULL, aErrorStack );

    switch( aAf )
    {
        case AF_INET:
            {
                STL_TRY( stnGetStrAddrFromBin4( aSrc, aDst, aSize, aErrorStack ) == STL_SUCCESS );
                break;
            }
#if STC_HAVE_IPV6
        case AF_INET6:
            {
                STL_TRY( stnGetStrAddrFromBin6( aSrc, aDst, aSize, aErrorStack ) == STL_SUCCESS );
                break;
            }
#endif
        default:
            {
                STL_THROW( RAMP_ERR_NOSUPPORT );
                break;
            }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOSUPPORT )
    {
        stlSnprintf(sMsg, 64, "not supported address format(%d)", aAf );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTSUPPORT ,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/* const char *
 * inet_ntop4(aSrc, aDst, aSize)
 *      format an IPv4 address, more or less like inet_ntoa()
 * return:
 *      `aDst' (as a const)
 * notes:
 *      (1) uses no statics
 *      (2) takes a u_char* not an in_addr as input
 * author:
 *      Paul Vixie, 1996.
 */
static stlStatus stnGetStrAddrFromBin4( const stlChar  * aSrc,
                                        stlChar        * aDst,
                                        stlSize          aSize,
                                        stlErrorStack  * aErrorStack )
{
    const stlSize   sMinSize = 16; /* space for 255.255.255.255\0 */
    stlInt32        sNum = 0;
    stlChar       * sNext = aDst;
    stlUInt8        sTmpChar;

    STL_PARAM_VALIDATE( aSrc != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aDst != NULL, aErrorStack );

    STL_TRY_THROW( aSize >= sMinSize, RAMP_ERR_NOSPACE );

    do
    {
        sTmpChar = *aSrc;
        aSrc++;
        if (sTmpChar > 99)
        {
            *sNext = '0' + (sTmpChar / 100);
            sNext++;
            sTmpChar %= 100;
            *sNext = '0' + (sTmpChar / 10);
            sNext++;
            sTmpChar %= 10;
        }
        else if (sTmpChar > 9)
        {
            *sNext = '0' + (sTmpChar / 10);
            sNext++;
            sTmpChar %= 10;
        }

        *sNext = '0' + sTmpChar;
        sNext++;
        *sNext = '.';
        sNext++;
        sNum++;
    } while( sNum < 4 );

    sNext--;  // mve to last '.' position
    *sNext = 0;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOSPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOSPACE,
                      "too short address string buffer",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


#if STC_HAVE_IPV6
/* const char *
 * inet_ntop6(aSrc, aDst, aSize)
 *      convert IPv6 binary address into presentation (printable) format
 * author:
 *      Paul Vixie, 1996.
 */
static stlStatus stnGetStrAddrFromBin6( const stlChar  * aSrc,
                                        stlChar        * aDst,
                                        stlSize          aSize,
                                        stlErrorStack  * aErrorStack )
{
    /*
     * Note that int32_t and int16_t need only be "at least" large enough
     * to contain a value of the specified aSize.  On some systems, like
     * Crays, there is no such thing as an integer variable with 16 bits.
     * Keep this in mind if you think this function should have been coded
     * to use pointer overlays.  All the world's not a VAX.
     */
    stlChar          sTmpBuf[STL_SIZEOF("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
    stlChar        * sTmpPtr;
    stlInt32         sBestBase = -1;
    stlInt32         sBestLen  = 0;
    stlInt32         sCurBase  = -1;
    stlInt32         sCurLen   = 0;
    stlInt32         sWords[IN6ADDRSZ / INT16SZ];
    stlInt32         i;
    const stlChar  * sNextSrc;
    const stlChar  * sSrcEnd;
    stlInt32      * sNextDest;
    stlInt32        sNextWord;

    STL_PARAM_VALIDATE( aSrc != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aDst != NULL, aErrorStack );

    /*
     * Preprocess:
     *  Copy the input (bytewise) array into a wordwise array.
     *  Find the longest run of 0x00's in aSrc[] for :: shorthanding.
     */
    sNextSrc = aSrc;
    sSrcEnd = aSrc + IN6ADDRSZ;
    sNextDest = sWords;
    i = 0;
    do
    {
        sNextWord = (stlUInt32)*sNextSrc;
        sNextSrc++;
        sNextWord <<= 8;
        sNextWord |= (stlUInt32)*sNextSrc;
        sNextSrc++;
        *sNextDest = sNextWord;
        sNextDest++;

        if( sNextWord == 0 )
        {
            if( sCurBase == -1 )
            {
                sCurBase = i;
                sCurLen = 1;
            }
            else
            {
                sCurLen++;
            }
        }
        else
        {
            if( sCurBase != -1 )
            {
                if( (sBestBase == -1) || (sCurLen > sBestLen) )
                {
                    sBestBase = sCurBase;
                    sBestLen = sCurLen;
                }
                sCurBase = -1;
            }
        }

        i++;
    } while( sNextSrc < sSrcEnd );

    if( sCurBase != -1 )
    {
        if( (sBestBase == -1) || (sCurLen > sBestLen) )
        {
            sBestBase = sCurBase;
            sBestLen = sCurLen;
        }
    }
    if( (sBestBase != -1) && (sBestLen < 2) )
    {
        sBestBase = -1;
    }

    /*
     * Format the result.
     */
    sTmpPtr = sTmpBuf;
    for( i = 0; i < (IN6ADDRSZ / INT16SZ); /* do nothing */ )
    {
        /* Are we inside the best run of 0x00's? */
        if( i == sBestBase )
        {
            *sTmpPtr = ':';
            sTmpPtr++;
            i += sBestLen;
            continue;
        }
        /* Are we following an initial run of 0x00s or any real hex? */
        if( i != 0 )
        {
            *sTmpPtr = ':';
            sTmpPtr++;
        }
        /* Is this address an encapsulated IPv4? */
        if( (i == 6) &&
            (sBestBase == 0) &&
            ((sBestLen == 6) || ((sBestLen == 5) && (sWords[5] == 0xffff))))
        {
            STL_TRY( stnGetStrAddrFromBin4( aSrc+12,
                                            sTmpPtr,
                                            STL_SIZEOF(sTmpBuf) - (sTmpPtr - sTmpBuf),
                                            aErrorStack ) == STL_SUCCESS );
            sTmpPtr += stlStrlen(sTmpPtr);
            break;
        }
        sTmpPtr += stlSnprintf(sTmpPtr, STL_SIZEOF(sTmpBuf) - (sTmpPtr - sTmpBuf), "%x", sWords[i]);
        i++;
    }
    /* Was it a trailing run of 0x00's? */
    if( (sBestBase != -1) && ((sBestBase + sBestLen) == (IN6ADDRSZ / INT16SZ)) )
    {
        *sTmpPtr = ':';
        sTmpPtr++;
    }
    *sTmpPtr = '\0';
    sTmpPtr++;

    /*
     * Check for overflow, copy, and we're done.
     */
    STL_TRY_THROW( (stlSize)(sTmpPtr - sTmpBuf) <= aSize, RAMP_ERR_NOSPACE );

    strcpy(aDst, sTmpBuf);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOSPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOSPACE,
                      "too short address string buffer",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}
#endif
