/*******************************************************************************
 * strStringsWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: strStringsUnix.c 15370 2015-07-09 06:20:14Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlTypes.h>
#include <str.h>
#include <ste.h>
#include <stlError.h>

#include <conio.h>

stlStatus strStrToFloat64( const stlChar  *aNumPtr,
                           stlInt64        aLength,
                           stlChar       **aEndPtr,
                           stlFloat64     *aResult,
                           stlErrorStack  *aErrorStack )
{
    stlFloat64      sResult       = 0.0;
    stlChar       * sEndPtr       = NULL;
    const stlChar * sStr          = aNumPtr;
    const stlChar * sFence;
    stlBool         sIsNegative   = STL_FALSE;
    stlInt32        sExp          = 0;
    stlInt32        sChar         = 0;
    stlInt32        sSign         = 1;
    stlBool         sConversion   = STL_FALSE;
    stlInt32        i             = 0;
    stlInt32        sDigit        = 0;
    stlFloat64      sPow          = 0.0;
    stlBool         sStartDigit   = STL_FALSE;

    while( stlIsspace( *sStr ) == STL_TRUE )
    {
        ++sStr;
    }

    if( *sStr == '-' )
    {
        sIsNegative = STL_TRUE;
        ++sStr;
    }
    else if( *sStr == '+' )
    {
        ++sStr;
    }

    /* INF or INFINITY.  */
    if( (( sStr[0] == 'i') || (sStr[0] == 'I')) &&
        (( sStr[1] == 'n') || (sStr[1] == 'N')) &&
        (( sStr[2] == 'f') || (sStr[2] == 'F')) )
    {
        if( ((sStr[3] == 'i') || (sStr[3] == 'I')) &&
            ((sStr[4] == 'n') || (sStr[4] == 'N')) &&
            ((sStr[5] == 'i') || (sStr[5] == 'I')) &&
            ((sStr[6] == 't') || (sStr[6] == 'T')) &&
            ((sStr[7] == 'y') || (sStr[7] == 'Y')) )
	{
            sEndPtr = (stlChar*)(sStr + 8);
	}
        else
	{
            sEndPtr = (stlChar*)(sStr + 3);
	}

        sConversion = STL_TRUE;
        sResult     = STL_FLOAT64_INFINITY;
        STL_THROW( RAMP_SUCCESS );
    }

    /* NAN or NAN(foo).  */
    if ( ((sStr[0] == 'n') || (sStr[0] == 'N')) &&
         ((sStr[1] == 'a') || (sStr[1] == 'A')) &&
         ((sStr[2] == 'n') || (sStr[2] == 'N')) )
    {
        sStr += 3;
        if( *sStr == '(' )
        {
            ++sStr;
            while( (*sStr != '\0') && (*sStr != ')') )
            {
                ++sStr;
            }
            
            if(*sStr == ')')
            {
                ++sStr;
            }
        }
        
        sEndPtr = (stlChar*)sStr;

        /**
         * @bug NAN 매크로가 정의되어 있는지를 확인해야 한다.
         */
        sConversion = STL_TRUE;
        sResult     = NAN;
        STL_THROW( RAMP_SUCCESS );
    }

    if( aLength < 0 )
    {
        sChar = *sStr++;
        while( (sChar != '\0') && (stlIsdigit( sChar ) == STL_TRUE ) )
        {
            if( sDigit < STL_DBL_DIG )
            {
                sResult = sResult * 10.0 + ( sChar - '0' );
            }
            else if( sDigit == STL_DBL_DIG )
            {
                if( sChar - '0' >= 5 )
                {
                    sResult++;
                }

                sExp++;
            }
            else
            {
                sExp++;
            }
            sConversion = STL_TRUE;

            if( sChar != '0' )
            {
                sStartDigit = STL_TRUE;
            }

            if ( sStartDigit == STL_TRUE )
            {
                sDigit++;
            }

            sChar = *sStr++;
        }

        if( sChar == '.' )
        {
            sChar = *sStr++;
            while( (sChar != '\0') && (stlIsdigit( sChar ) == STL_TRUE ) )
            {
                if( sDigit < STL_DBL_DIG )
                {
                    sResult = sResult * 10.0 + ( sChar - '0' );
                    sExp--;
                }
                else if( sDigit == STL_DBL_DIG )
                {
                    if( sChar - '0' >= 5 )
                    {
                        sResult++;
                    }
                }
                sConversion = STL_TRUE;
                if( sChar != '0' )
                {
                    sStartDigit = STL_TRUE;
                }
                
                if( sStartDigit == STL_TRUE )
                {
                    sDigit++;
                }

                sChar = *sStr++;
            }
        }

        if( (sChar == 'e') || (sChar == 'E' ) )
        {
            sChar = *sStr++;

            if( sChar == '+' )
            {
                sChar = *sStr++;
            }
            else if( sChar == '-' )
            {
                sChar = *sStr++;
                sSign = -1;
            }

            while( stlIsdigit( sChar ) == STL_TRUE )
            {
                i = i * 10.0 + ( sChar - '0' );
                sChar = *sStr++;
            }

            sExp += i * sSign;
        }

        sEndPtr = (stlChar*)(sStr - 1);
    }
    else
    {
        sFence = aNumPtr + aLength;

        while( sStr < sFence )
        {
            sChar = *sStr++;
            if( stlIsdigit( sChar ) == STL_FALSE )
            {
                break;
            }

            if( sDigit < STL_DBL_DIG )
            {
                sResult = sResult * 10.0 + ( sChar - '0' );
            }
            else if( sDigit == STL_DBL_DIG )
            {
                if( sChar - '0' >= 5 )
                {
                    sResult++;
                }
                sExp++;
            }
            else
            {
                sExp++;
            }

            sConversion = STL_TRUE;

            if( sChar != '0' )
            {
                sStartDigit = STL_TRUE;
            }

            if ( sStartDigit == STL_TRUE )
            {
                sDigit++;
            }
        }

        if( sChar == '.' )
        {
            while( sStr < sFence )
            {
                sChar = *sStr++;
                if( stlIsdigit( sChar ) == STL_FALSE )
                {
                    break;
                }

                if( sDigit < STL_DBL_DIG )
                {
                    sResult = sResult * 10.0 + ( sChar - '0' );
                    sExp--;
                }
                else if( sDigit == STL_DBL_DIG )
                {
                    if( sChar - '0' >= 5 )
                    {
                        sResult++;
                    }
                }
                sConversion = STL_TRUE;
                if( sChar != '0' )
                {
                    sStartDigit = STL_TRUE;
                }
                
                if( sStartDigit == STL_TRUE )
                {
                    sDigit++;
                }
            }
        }

        if( (sChar == 'e') || (sChar == 'E' ) )
        {
            sChar = *sStr;

            if( sChar == '+' )
            {
                sStr++;
            }
            else if( sChar == '-' )
            {
                sStr++;
                sSign = -1;
            }

            while( sStr < sFence )
            {
                sChar = *sStr++;
                if( stlIsdigit( sChar ) == STL_FALSE )
                {
                    break;
                }

                i = i * 10.0 + ( sChar - '0' );
            }

            sExp += i * sSign;
        }

        sEndPtr = (stlChar*)sStr;
    }

    STL_TRY_THROW( sConversion == STL_TRUE, RAMP_ERR_STRING_IS_NOT_NUMBER );

    STL_TRY_THROW( stlGet10Pow( sExp, &sPow, aErrorStack ) == STL_TRUE,
                   RAMP_ERR_OUT_OF_RANGE);
    sResult *= sPow;

    if( sIsNegative == STL_TRUE )
    {
        sResult *= -1;
    }

    STL_TRY_THROW( stlIsinfinite( sResult ) == STL_IS_INFINITE_FALSE,
                   RAMP_ERR_OUT_OF_RANGE );

    STL_RAMP( RAMP_SUCCESS );

    if( aResult != NULL )
    {
        *aResult = sResult;
    }

    if( aEndPtr != NULL )
    {
        *aEndPtr = sEndPtr;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_STRING_IS_NOT_NUMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_STRING_IS_NOT_NUMBER,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OUT_OF_RANGE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus strStrToFloat32( const stlChar  *aNumPtr,
                           stlInt64        aLength,
                           stlChar       **aEndPtr,
                           stlFloat32     *aResult,
                           stlErrorStack  *aErrorStack)
{
    stlFloat32      sResult       = 0.0;
    stlChar       * sEndPtr       = NULL;
    const stlChar * sStr          = aNumPtr;
    const stlChar * sFence;
    stlBool         sIsNegative   = STL_FALSE;
    stlInt32        sExp          = 0;
    stlInt32        sChar         = 0;
    stlInt32        sSign         = 1;
    stlBool         sConversion   = STL_FALSE;
    stlInt32        i             = 0;
    stlInt32        sDigit        = 0;
    stlFloat64      sPow          = 0.0;
    stlBool         sStartDigit   = STL_FALSE;

    while( stlIsspace( *sStr ) == STL_TRUE )
    {
        ++sStr;
    }

    if( *sStr == '-' )
    {
        sIsNegative = STL_TRUE;
        ++sStr;
    }
    else if( *sStr == '+' )
    {
        ++sStr;
    }

    /* INF or INFINITY.  */
    if( (( sStr[0] == 'i') || (sStr[0] == 'I')) &&
        (( sStr[1] == 'n') || (sStr[1] == 'N')) &&
        (( sStr[2] == 'f') || (sStr[2] == 'F')) )
    {
        if( ((sStr[3] == 'i') || (sStr[3] == 'I')) &&
            ((sStr[4] == 'n') || (sStr[4] == 'N')) &&
            ((sStr[5] == 'i') || (sStr[5] == 'I')) &&
            ((sStr[6] == 't') || (sStr[6] == 'T')) &&
            ((sStr[7] == 'y') || (sStr[7] == 'Y')) )
	{
            sEndPtr = (stlChar*)(sStr + 8);
	}
        else
	{
            sEndPtr = (stlChar*)(sStr + 3);
	}

        sConversion = STL_TRUE;
        sResult     = STL_FLOAT32_INFINITY;
        STL_THROW( RAMP_SUCCESS );
    }

    /* NAN or NAN(foo).  */
    if ( ((sStr[0] == 'n') || (sStr[0] == 'N')) &&
         ((sStr[1] == 'a') || (sStr[1] == 'A')) &&
         ((sStr[2] == 'n') || (sStr[2] == 'N')) )
    {
        sStr += 3;
        if( *sStr == '(' )
	{
            ++sStr;
            while( (*sStr != '\0') && (*sStr != ')') )
            {
                ++sStr;
            }

            if(*sStr == ')')
            {
                ++sStr;
            }
	}

        sEndPtr = (stlChar*)sStr;

        /**
         * @bug NAN 매크로가 정의되어 있는지를 확인해야 한다.
         */
        sConversion = STL_TRUE;
        sResult     = NAN;
        STL_THROW( RAMP_SUCCESS );
    }

    if( aLength < 0 )
    {
        sChar = *sStr++;
        while( (sChar != '\0') && (stlIsdigit( sChar ) == STL_TRUE ) )
        {
            if( sDigit < STL_FLT_DIG )
            {
                sResult = sResult * 10.0 + ( sChar - '0' );
            }
            else if( sDigit == STL_FLT_DIG )
            {
                if( sChar - '0' >= 5 )
                {
                    sResult++;
                }

                sExp++;
            }
            else
            {
                sExp++;
            }
            sConversion = STL_TRUE;

            if( sChar != '0' )
            {
                sStartDigit = STL_TRUE;
            }

            if ( sStartDigit == STL_TRUE )
            {
                sDigit++;
            }

            sChar = *sStr++;
        }

        if( sChar == '.' )
        {
            sChar = *sStr++;
            while( (sChar != '\0') && (stlIsdigit( sChar ) == STL_TRUE ) )
            {
                if( sDigit < STL_FLT_DIG )
                {
                    sResult = sResult * 10.0 + ( sChar - '0' );
                    sExp--;
                }
                else if( sDigit == STL_FLT_DIG )
                {
                    if( sChar - '0' >= 5 )
                    {
                        sResult++;
                    }
                }
                sConversion = STL_TRUE;
                if( sChar != '0' )
                {
                    sStartDigit = STL_TRUE;
                }

                if ( sStartDigit == STL_TRUE )
                {
                    sDigit++;
                }

                sChar = *sStr++;
            }
        }

        if( (sChar == 'e') || (sChar == 'E' ) )
        {
            sChar = *sStr++;

            if( sChar == '+' )
            {
                sChar = *sStr++;
            }
            else if( sChar == '-' )
            {
                sChar = *sStr++;
                sSign = -1;
            }

            while( stlIsdigit( sChar ) == STL_TRUE )
            {
                i = i * 10.0 + ( sChar - '0' );
                sChar = *sStr++;
            }

            sExp += i * sSign;
        }

        sEndPtr = (stlChar*)(sStr - 1);
    }
    else
    {
        sFence = aNumPtr + aLength;

        while( sStr < sFence )
        {
            sChar = *sStr++;
            if( stlIsdigit( sChar ) == STL_FALSE )
            {
                break;
            }

            if( sDigit < STL_FLT_DIG )
            {
                sResult = sResult * 10.0 + ( sChar - '0' );
            }
            else if( sDigit == STL_FLT_DIG )
            {
                if( sChar - '0' >= 5 )
                {
                    sResult++;
                }
                sExp++;
            }
            else
            {
                sExp++;
            }

            sConversion = STL_TRUE;

            if( sChar != '0' )
            {
                sStartDigit = STL_TRUE;
            }

            if ( sStartDigit == STL_TRUE )
            {
                sDigit++;
            }
        }

        if( sChar == '.' )
        {
            while( sStr < sFence )
            {
                sChar = *sStr++;
                if( stlIsdigit( sChar ) == STL_FALSE )
                {
                    break;
                }

                if( sDigit < STL_FLT_DIG )
                {
                    sResult = sResult * 10.0 + ( sChar - '0' );
                    sExp--;
                }
                else if( sDigit == STL_FLT_DIG )
                {
                    if( sChar - '0' >= 5 )
                    {
                        sResult++;
                    }
                }
                sConversion = STL_TRUE;
                if( sChar != '0' )
                {
                    sStartDigit = STL_TRUE;
                }

                if ( sStartDigit == STL_TRUE )
                {
                    sDigit++;
                }
            }
        }

        if( (sChar == 'e') || (sChar == 'E' ) )
        {
            sChar = *sStr;

            if( sChar == '+' )
            {
                sStr++;
            }
            else if( sChar == '-' )
            {
                sStr++;
                sSign = -1;
            }

            while( sStr < sFence )
            {
                sChar = *sStr++;
                if( stlIsdigit( sChar ) == STL_FALSE )
                {
                    break;
                }

                i = i * 10.0 + ( sChar - '0' );
            }

            sExp += i * sSign;
        }

        sEndPtr = (stlChar*)sStr;
    }

    STL_TRY_THROW( sConversion == STL_TRUE, RAMP_ERR_STRING_IS_NOT_NUMBER );

    STL_TRY_THROW( stlGet10Pow( sExp, &sPow, aErrorStack ) == STL_TRUE,
                   RAMP_ERR_OUT_OF_RANGE );
    sResult *= sPow;

    if( sIsNegative == STL_TRUE )
    {
        sResult *= -1;
    }

    STL_TRY_THROW( stlIsinfinite( sResult ) == STL_IS_INFINITE_FALSE,
                   RAMP_ERR_OUT_OF_RANGE );

    STL_RAMP( RAMP_SUCCESS );

    if( aResult != NULL )
    {
        *aResult = sResult;
    }

    if( aEndPtr != NULL )
    {
        *aEndPtr = sEndPtr;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_STRING_IS_NOT_NUMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_STRING_IS_NOT_NUMBER,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OUT_OF_RANGE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus strStrToInt64(const stlChar  *aNumPtr,
                        stlInt64        aLength,
                        stlChar       **aEndPtr,
                        stlInt32        aBase,
                        stlInt64       *aResult,
                        stlErrorStack  *aErrorStack)
{
    stlInt64        sResult = 0;
    stlChar       * sEndPtr = NULL;
    const stlChar * sStr;
    const stlChar * sFence;
    stlInt64        sValue;
    stlChar         sChar;
    stlBool         sIsNegative = STL_FALSE;
    stlInt32        sAny = 0;

    errno = 0;
    
    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */

    sStr = aNumPtr;    
    do
    {
        sChar = *sStr++;
    } while( stlIsspace(sChar) == STL_TRUE );

    if( sChar == '-' )
    {
        sIsNegative = STL_TRUE;
        sChar       = *sStr++;
    }
    else if( sChar == '+' )
    {
        sChar = *sStr++;
    }

    if( ((aBase == 0) || (aBase == 16)) &&
        (sChar == '0') &&
        ((*sStr == 'x') || (*sStr == 'X')) )
    {
        sChar = sStr[1];
        sStr += 2;
        aBase = 16;
    }
    
    if( aBase == 0 )
    {
        if( sChar == '0' )
        {
            aBase = 8;
        }
        else
        {
            aBase = 10;
        }
    }

    if( (aBase < 2 ) || (aBase > 36) )
    {
        sEndPtr = (stlChar*)aNumPtr;

        STL_THROW( RAMP_ERR_INVALID_ARGUMENT );
    }

    /* The classic bsd implementation requires div/mod operators
     * to compute a cutoff.  Benchmarking proves that is very, very
     * evil to some 32 bit processors.  Instead, look for underflow
     * in both the mult and add/sub operation.  Unlike the bsd impl,
     * we also work strictly in a signed int64 word as we haven't
     * implemented the unsigned type in win32.
     * 
     * Set 'any' if any `digits' consumed; make it negative to indicate
     * overflow.
     */

    sValue = 0;

    if( aLength < 0 )
    {
        for( ; ; sChar = *sStr++ )
        {
            if( (sChar >= '0') && (sChar <= '9') )
            {
                sChar -= '0';
            }
#if (('Z' - 'A') == 25)  
            else if( (sChar >= 'A') && (sChar <= 'Z') )
            {
                sChar -= 'A' - 10;
            }
            else if( (sChar >= 'a') && (sChar <= 'z') )
            {
                sChar -= 'a' - 10;
            }
#else
#error "CANNOT COMPILE strStrToInt64(), only ASCII supported"
#endif
            else
            {
                break;
            }

            if( sChar >= aBase )
            {
                break;
            }

            sValue *= aBase;

            if ( (sAny < 0) /* already noted an over/under flow - short circuit */ ||
                 ((sIsNegative == STL_TRUE) &&
                  (sValue > sResult || (sValue -= sChar) > sResult)) /* underflow */ ||
                 ((sIsNegative == STL_FALSE) &&
                  (sValue < sResult || (sValue += sChar) < sResult)) /* overflow */ )
            {
                sAny = -1;
            }
            else
            {
                sResult = sValue;
                sAny = 1;
            }
        }
    }
    else
    {
        sFence = aNumPtr + aLength;

        for( ; sStr <= sFence; sChar = *sStr++ )
        {
            if( (sChar >= '0') && (sChar <= '9') )
            {
                sChar -= '0';
            }
#if (('Z' - 'A') == 25)  
            else if( (sChar >= 'A') && (sChar <= 'Z') )
            {
                sChar -= 'A' - 10;
            }
            else if( (sChar >= 'a') && (sChar <= 'z') )
            {
                sChar -= 'a' - 10;
            }
#else
#error "CANNOT COMPILE strStrToInt64(), only ASCII supported"
#endif
            else
            {
                break;
            }

            if( sChar >= aBase )
            {
                break;
            }

            sValue *= aBase;

            if ( (sAny < 0) /* already noted an over/under flow - short circuit */ ||
                 ((sIsNegative == STL_TRUE) &&
                  (sValue > sResult || (sValue -= sChar) > sResult)) /* underflow */ ||
                 ((sIsNegative == STL_FALSE) &&
                  (sValue < sResult || (sValue += sChar) < sResult)) /* overflow */ )
            {
                sAny = -1;
            }
            else
            {
                sResult = sValue;
                sAny = 1;
            }
        }
    }

    if( sAny < 0 )
    {
        if( sIsNegative == STL_TRUE )
        {
            sResult = STL_INT64_MIN;
        }
        else
        {
            sResult = STL_INT64_MAX;
        }

        STL_THROW( RAMP_ERR_OUT_OF_RANGE );
    }

    STL_TRY_THROW( sAny != 0, RAMP_ERR_STRING_IS_NOT_NUMBER );
    
    sEndPtr = (stlChar*)(sStr - 1);

    if( aResult != NULL )
    {
        *aResult = sResult;
    }

    if( aEndPtr != NULL )
    {
        *aEndPtr = sEndPtr;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_STRING_IS_NOT_NUMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_STRING_IS_NOT_NUMBER,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OUT_OF_RANGE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus strStrToUInt64( const stlChar  *aNumPtr,
                          stlInt64        aLength,
                          stlChar       **aEndPtr,
                          stlInt32        aBase,
                          stlUInt64      *aResult,
                          stlErrorStack  *aErrorStack)
{
    stlUInt64       sResult = 0;
    stlChar       * sEndPtr = NULL;
    const stlChar * sStr;
    const stlChar * sFence;
    stlUInt64       sValue;
    stlChar         sChar;
    stlInt32        sAny = 0;

    errno = 0;
    
    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */

    sStr = aNumPtr;    
    do
    {
        sChar = *sStr++;
    } while( stlIsspace(sChar) == STL_TRUE );

    if( sChar == '-' )
    {
        sResult = STL_UINT64_MAX;

        STL_THROW( RAMP_ERR_OUT_OF_RANGE );
    }
    else if( sChar == '+' )
    {
        sChar = *sStr++;
    }

    if( ((aBase == 0) || (aBase == 16)) &&
        (sChar == '0') &&
        ((*sStr == 'x') || (*sStr == 'X')) )
    {
        sChar = sStr[1];
        sStr += 2;
        aBase = 16;
    }
    
    if( aBase == 0 )
    {
        if( sChar == '0' )
        {
            aBase = 8;
        }
        else
        {
            aBase = 10;
        }
    }

    if( (aBase < 2 ) || (aBase > 36) )
    {
        sEndPtr = (stlChar*)aNumPtr;

        STL_THROW( RAMP_ERR_INVALID_ARGUMENT );
    }

    /* The classic bsd implementation requires div/mod operators
     * to compute a cutoff.  Benchmarking proves that is very, very
     * evil to some 32 bit processors.  Instead, look for underflow
     * in both the mult and add/sub operation.  Unlike the bsd impl,
     * we also work strictly in a signed int64 word as we haven't
     * implemented the unsigned type in win32.
     * 
     * Set 'any' if any `digits' consumed; make it negative to indicate
     * overflow.
     */

    sValue = 0;

    if( aLength < 0 )
    {
        for( ; ; sChar = *sStr++ )
        {
            if( (sChar >= '0') && (sChar <= '9') )
            {
                sChar -= '0';
            }
#if (('Z' - 'A') == 25)  
            else if( (sChar >= 'A') && (sChar <= 'Z') )
            {
                sChar -= 'A' - 10;
            }
            else if( (sChar >= 'a') && (sChar <= 'z') )
            {
                sChar -= 'a' - 10;
            }
#else
#error "CANNOT COMPILE strStrToUInt64(), only ASCII supported"
#endif
            else
            {
                break;
            }

            if( sChar >= aBase )
            {
                break;
            }

            sValue *= aBase;

            if ( (sAny < 0) /* already noted an over/under flow - short circuit */ ||
                 ((sValue < sResult || (sValue += sChar) < sResult)) /* overflow */ )
            {
                sAny = -1;
            }
            else
            {
                sResult = sValue;
                sAny = 1;
            }
        }
    }
    else
    {
        sFence = aNumPtr + aLength;

        for( ; sStr <= sFence; sChar = *sStr++ )
        {
            if( (sChar >= '0') && (sChar <= '9') )
            {
                sChar -= '0';
            }
#if (('Z' - 'A') == 25)  
            else if( (sChar >= 'A') && (sChar <= 'Z') )
            {
                sChar -= 'A' - 10;
            }
            else if( (sChar >= 'a') && (sChar <= 'z') )
            {
                sChar -= 'a' - 10;
            }
#else
#error "CANNOT COMPILE strStrToInt64(), only ASCII supported"
#endif
            else
            {
                break;
            }

            if( sChar >= aBase )
            {
                break;
            }

            sValue *= aBase;

            if ( (sAny < 0) /* already noted an over/under flow - short circuit */ ||
                 ((sValue < sResult || (sValue += sChar) < sResult)) /* overflow */ )
            {
                sAny = -1;
            }
            else
            {
                sResult = sValue;
                sAny = 1;
            }
        }
    }

    if( sAny < 0 )
    {
        sResult = STL_UINT64_MAX;

        STL_THROW( RAMP_ERR_OUT_OF_RANGE );
    }

    STL_TRY_THROW( sAny != 0, RAMP_ERR_STRING_IS_NOT_NUMBER );
    
    sEndPtr = (stlChar*)(sStr - 1);

    if( aResult != NULL )
    {
        *aResult = sResult;
    }

    if( aEndPtr != NULL )
    {
        *aEndPtr = sEndPtr;
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_STRING_IS_NOT_NUMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_STRING_IS_NOT_NUMBER,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OUT_OF_RANGE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus strGetPassword( const stlChar * aPrompt,
                          stlBool         aEcho,
                          stlChar       * aBuffer,
                          stlSize         aBufferLength,
                          stlErrorStack * aErrorStack )
{
    stlChar   sTmp;
    stlChar * sPos = aBuffer;
    stlChar * sEnd = aBuffer + aBufferLength;
    
    STL_PARAM_VALIDATE( aBuffer != NULL,   aErrorStack );
    STL_PARAM_VALIDATE( aBufferLength > 0, aErrorStack );

    _cputs( aPrompt ? aPrompt : "Enter password: " );

    for (;;)
    {
        sTmp = _getch();

        /* backspace */
        if( (sTmp == '\b') || ((stlInt32)sTmp == 127) )
        {
            if( sPos != aBuffer )
            {
                if( aEcho == STL_TRUE )
                {
                    _cputs( "\b \b" );
                }
                
                sPos--;
                
                continue;
            }
        }
        
        if( (sTmp == '\n') || (sTmp == '\r') || (sTmp == 3) )
        {
            break;
        }

        if( (iscntrl(sTmp) == 1) || (sPos == sEnd) )
        {        
            continue;
        }
        
        if( aEcho == STL_TRUE )
        {
            _cputs( "*" );
            fflush( stdout );
        }
        
        *(sPos++) = sTmp;
    }
    
    while( (sPos != aBuffer) && (isspace(sPos[-1]) == ' ') )
    {
        /* Allow dummy space at end */
        sPos--;
    }
    
    *sPos = 0;

    if( aEcho == STL_TRUE )
    {
        _cputs( "\n" );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
