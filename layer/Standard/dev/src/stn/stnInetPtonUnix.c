/*******************************************************************************
 * stnInetPtonUnix.c
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
#define INT16SZ sizeof(stlInt16)
#endif

#ifndef INADDRSZ
#define INADDRSZ    4
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

static stlStatus stnGetBinAddrFromStr4 __P((const stlChar  * aSrc,
                                            stlChar        * aDst,
                                            stlErrorStack  * aErrorStack));
#if STC_HAVE_IPV6
static stlStatus stnGetBinAddrFromStr6 __P((const stlChar  * aSrc,
                                            stlChar        * aDst,
                                            stlErrorStack  * aErrorStack));
#endif

static const stlChar stnDigits[]   = "0123456789";
static const stlChar stnXDigitsL[] = "0123456789abcdef";
static const stlChar stnXDigitsU[] = "0123456789ABCDEF";

/* int
 * stlGetBinAddrFromStr(af, aSrc, aDst)
 *      convert from presentation format (which usually means ASCII printable)
 *      to network format (which is usually some kind of binary format).
 * return:
 *      1 if the address was valid for the specified address family
 *      0 if the address wasn't valid (`aDst' is untouched in this case)
 *      -1 if some other error occurred (`aDst' is untouched in this case, too)
 * author:
 *      Paul Vixie, 1996.
 */
stlStatus stnGetBinAddrFromStr( stlInt32        aAf,
                                const stlChar * aSrc,
                                void          * aDst,
                                stlErrorStack * aErrorStack)
{
    stlChar sMsg[64];

    STL_PARAM_VALIDATE( aSrc != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aDst != NULL, aErrorStack );

    switch( aAf )
    {
        case AF_INET:
        {
            STL_TRY( stnGetBinAddrFromStr4(aSrc, aDst, aErrorStack) == STL_SUCCESS );
            break;
        }
#if STC_HAVE_IPV6
        case AF_INET6:
        {
            STL_TRY( stnGetBinAddrFromStr6(aSrc, aDst, aErrorStack) == STL_SUCCESS );
            break;
        }
#endif
        default:
        {
            STL_THROW( RAMP_ERR_NOSUPPORT );
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOSUPPORT )
    {
        stlSnprintf(sMsg, 64, "not supported address format(%d)", aAf);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NOTSUPPORT,
                      sMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/* int
 * inet_pton4(aSrc, aDst)
 *      like inet_aton() but without all the hexadecimal and shorthand.
 * return:
 *      1 if `aSrc' is a valid dotted quad, else 0.
 * notice:
 *      does not touch `aDst' unless it's returning 1.
 * author:
 *      Paul Vixie, 1996.
 */
static stlStatus stnGetBinAddrFromStr4( const stlChar  * aSrc,
                                        stlChar        * aDst,
                                        stlErrorStack  * aErrorStack )
{
    stlInt32               sSawDigit;
    stlInt32               sOctets;
    stlInt32               sChar;
    stlChar                sTmp[INADDRSZ];
    stlChar              * sTmpPtr;
    const stlChar        * sCharPtr;
    stlUInt32              sNewChar;
    stlChar                sMsg[64];

    STL_PARAM_VALIDATE( aSrc != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aDst != NULL, aErrorStack );

    sSawDigit = 0;
    sOctets = 0;
    sTmpPtr = sTmp;
    *sTmpPtr = 0;

    sChar = *aSrc;
    while( sChar != '\0' )
    {
        sCharPtr = stlStrchr(stnDigits, sChar);
        if( sCharPtr != NULL )
        {
            sNewChar = ((*sTmpPtr) * 10) + (stlUInt32)(sCharPtr - stnDigits);
            STL_TRY_THROW( sNewChar <= 255, RAMP_ERR_INVALID_ADDRESS );
            *sTmpPtr = sNewChar;
            if( sSawDigit == 0 )
            {
                sOctets++;
                STL_TRY_THROW( sOctets <= 4, RAMP_ERR_INVALID_ADDRESS );
                sSawDigit = 1;
            }
        }
        else if( (sChar == '.') && (sSawDigit != 0) )
        {
            STL_TRY_THROW( sOctets != 4, RAMP_ERR_INVALID_ADDRESS );
            sTmpPtr++;
            *sTmpPtr = 0;
            sSawDigit = 0;
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_ADDRESS );
        }
        aSrc++;
        sChar = *aSrc;
    }

    STL_TRY_THROW( sOctets >= 4, RAMP_ERR_INVALID_ADDRESS );

    stlMemcpy(aDst, sTmp, INADDRSZ);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ADDRESS )
    {
        stlSnprintf( sMsg, 64, "invalid address(%s)", aSrc );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INET_PTON_INVAL,
                      sMsg,
                      aErrorStack );
   }

    STL_FINISH;

    return STL_FAILURE;
}

#if STC_HAVE_IPV6
/* int
 * inet_pton6(aSrc, aDst)
 *      convert presentation level address to network order binary form.
 * return:
 *      1 if `aSrc' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *      (1) does not touch `aDst' unless it's returning 1.
 *      (2) :: in a full address is silently ignored.
 * credit:
 *      inspired by Mark Andrews.
 * author:
 *      Paul Vixie, 1996.
 */
static stlStatus stnGetBinAddrFromStr6( const stlChar  * aSrc,
                                        stlChar        * aDst,
                                        stlErrorStack  * aErrorStack )
{
    stlChar                 sTmp[IN6ADDRSZ];
    stlChar               * sTmpPtr;
    stlChar               * sEndPtr;
    stlChar               * sColonPtr;
    const stlChar         * sXDigits;
    const stlChar         * sCurToken;
    stlInt32                sChar;
    stlInt32                sSawXDigit;
    stlInt32               sVal;
    const stlChar         * sCharPtr;
    stlSize                 sLen;
    stlSize                 i;
    stlChar                 sMsg[64];

    STL_PARAM_VALIDATE( aSrc != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aDst != NULL, aErrorStack );

    sTmpPtr = sTmp;
    stlMemset( sTmpPtr, '\0', IN6ADDRSZ );
    sEndPtr = sTmpPtr + IN6ADDRSZ;
    sColonPtr = NULL;
    /* Leading :: requires some special handling. */
    if (*aSrc == ':')
    {
        aSrc++;
        STL_TRY_THROW( *aSrc == ':', RAMP_ERR_INVALID_ADDRESS );
    }
    sCurToken = aSrc;
    sSawXDigit = 0;
    sVal = 0;
    sChar = *aSrc;
    while( sChar != '\0' )
    {
        sXDigits = stnXDigitsL;
        sCharPtr = stlStrchr(sXDigits, sChar);
        if( sCharPtr == NULL )
        {
            sXDigits = stnXDigitsU;
            sCharPtr = stlStrchr(sXDigits, sChar);
        }
        if( sCharPtr != NULL )
        {
            sVal <<= 4;
            sVal |= (sCharPtr - sXDigits);
            STL_TRY_THROW( sVal <= 0xffff, RAMP_ERR_INVALID_ADDRESS );
            sSawXDigit = 1;
            aSrc++;
            sChar = *aSrc;
            continue;
        }
        if( sChar == ':' )
        {
            sCurToken = aSrc;
            if( sSawXDigit == 0 )
            {
                STL_TRY_THROW( sColonPtr == NULL, RAMP_ERR_INVALID_ADDRESS );
                sColonPtr = sTmpPtr;
                aSrc++;
                sChar = *aSrc;
                continue;
            }
            STL_TRY_THROW( sTmpPtr + INT16SZ <= sEndPtr, RAMP_ERR_INVALID_ADDRESS );
            *sTmpPtr++ = ((stlChar) (sVal >> 8)) & 0xff;
            *sTmpPtr++ = ((stlChar) sVal) & 0xff;
            sSawXDigit = 0;
            sVal = 0;
            aSrc++;
            sChar = *aSrc;
            continue;
        }
        if( (sChar == '.') && ((sTmpPtr + INADDRSZ) <= sEndPtr) )
        {
            STL_TRY( stnGetBinAddrFromStr4(sCurToken, sTmpPtr, aErrorStack) == STL_SUCCESS); 
            sTmpPtr += INADDRSZ;
            sSawXDigit = 0;
            break;  /* '\0' was seen by inet_pton4(). */
        } 
        STL_THROW( RAMP_ERR_INVALID_ADDRESS );
    }
    if( sSawXDigit != 0 )
    {
        STL_TRY_THROW( sTmpPtr + INT16SZ <= sEndPtr, RAMP_ERR_INVALID_ADDRESS );
        *sTmpPtr++ = (stlChar) (sVal >> 8) & 0xff;
        *sTmpPtr++ = (stlChar) sVal & 0xff;
    }
    if( sColonPtr != NULL )
    {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        sLen = sTmpPtr - sColonPtr;
        for( i = 1; i <= sLen; i++ )
        {
            sEndPtr[- i] = sColonPtr[sLen - i];
            sColonPtr[sLen - i] = 0;
        }
        sTmpPtr = sEndPtr;
    }

    STL_TRY_THROW( sTmpPtr == sEndPtr, RAMP_ERR_INVALID_ADDRESS );
    stlMemcpy(aDst, sTmp, IN6ADDRSZ);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ADDRESS )
    {
        stlSnprintf(sMsg, 64, "invalid address(%s)", aSrc);
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INET_PTON_INVAL,
                      sMsg,
                      aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;
}
#endif
