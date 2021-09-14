/*******************************************************************************
 * strSnprintf.c
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

#include <str.h>
#include <math.h>
#if STC_HAVE_CTYPE_H
#include <ctype.h>
#endif
#if STC_HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if STC_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if STC_HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#if STC_HAVE_LIMITS_H
#include <limits.h>
#endif
#if STC_HAVE_STRING_H
#include <string.h>
#endif

const stlChar gStrNullString[] = "(null)";
#define STR_NULL_STRING      ((stlChar *)gStrNullString)
#define STR_NULL_STRING_LEN  6

#define STR_FLOAT_DIGITS     6
#define STR_EXPONENT_LENGTH  10

static const stlChar  strLowDigits[]   = "0123456789abcdef";
static const stlChar  strUpperDigits[] = "0123456789ABCDEF";

/*
 * STR_NUM_BUF_SIZE is the size of the buffer used for arithmetic conversions
 *
 * NOTICE: this is a magic number; do not decrease it
 */
#define STR_NUM_BUF_SIZE     512

/*
 * cvt - IEEE floating point formatting routines.
 *       Derived from UNIX V7, Copyright(C) Caldera International Inc.
 */

/*
 *    strECvt converts to decimal
 *      the number of digits is specified by aNDigits
 *      aDecPoint is set to the position of the decimal point
 *      aSign is set to 0 for positive, 1 for negative
 */

#define STR_NUM_OF_DIGITS 330

/* buf must have at least STR_NUM_OF_DIGITS bytes */
static stlChar *strCvt(stlFloat64   aFloatValue,
                       stlInt32     aNDigits,
                       stlInt32    *aDecPoint,
                       stlInt32    *aSign,
                       stlInt32     aEFlag,
                       stlChar     *aBuffer)
{
    stlInt32     sR2;
    stlFloat64   sIntegralPart;
    stlFloat64   sFractionalPart;
    stlChar     *sBuffer;
    stlChar     *sTempBuffer;

    if(aNDigits >= (STR_NUM_OF_DIGITS - 1))
    {
        aNDigits = STR_NUM_OF_DIGITS - 2;
    }

    sR2 = 0;
    *aSign = 0;
    sBuffer = &aBuffer[0];
    if(aFloatValue < 0)
    {
        *aSign = 1;
        aFloatValue = -aFloatValue;
    }
    aFloatValue = stlModf(aFloatValue, &sIntegralPart);
    sTempBuffer = &aBuffer[STR_NUM_OF_DIGITS-1];
    /*
     * Do integer part
     */
    if((stlInt64)sIntegralPart != 0)
    {
        sTempBuffer = &aBuffer[STR_NUM_OF_DIGITS-1];
        while( (sTempBuffer > &aBuffer[0]) && ((stlInt64)sIntegralPart != 0) )
        {
            sFractionalPart = stlModf(sIntegralPart / 10, &sIntegralPart);
            --sTempBuffer;
            *sTempBuffer = (stlInt32)((sFractionalPart + .03) * 10) + '0';
            sR2++;
        }
        while(sTempBuffer < &aBuffer[STR_NUM_OF_DIGITS-1])
        {
            *sBuffer = *sTempBuffer;
            sBuffer++;
            sTempBuffer++;
        }
    }
    else
    {
        if(aFloatValue > 0)
        {
            sFractionalPart = aFloatValue * 10;
            while(sFractionalPart < 1)
            {
                aFloatValue = sFractionalPart;
                sR2--;
                sFractionalPart = aFloatValue * 10;
            }
        }
    }

    sTempBuffer = &aBuffer[aNDigits];
    if (aEFlag == 0)
    {
        sTempBuffer += sR2;
    }

    if (sTempBuffer < &aBuffer[0])
    {
        *aDecPoint = -aNDigits;
        aBuffer[0] = STR_NULCHAR;
        STL_THROW(EXIT_FUNCTION);
    }
    *aDecPoint = sR2;
    while( (sBuffer <= sTempBuffer) && (sBuffer < &aBuffer[STR_NUM_OF_DIGITS]) )
    {
        aFloatValue *= 10;
        aFloatValue = stlModf(aFloatValue, &sFractionalPart);
        *sBuffer = (stlInt32) sFractionalPart + '0';
        sBuffer++;
    }
    if( sTempBuffer >= &aBuffer[STR_NUM_OF_DIGITS] )
    {
        aBuffer[STR_NUM_OF_DIGITS - 1] = STR_NULCHAR;
        STL_THROW(EXIT_FUNCTION);
    }
    sBuffer = sTempBuffer;
    *sTempBuffer += 5;
    while( *sTempBuffer > '9' )
    {
        *sTempBuffer = '0';
        if( sTempBuffer > aBuffer )
        {
            --sTempBuffer;
            ++(*sTempBuffer);
        }
        else
        {
            *sTempBuffer = '1';
            (*aDecPoint)++;
            if( aEFlag == 0 )
            {
                if( sBuffer > aBuffer )
                {
                    *sBuffer = '0';
                }
                sBuffer++;
            }
        }
    }
    *sBuffer = STR_NULCHAR;

    STL_RAMP(EXIT_FUNCTION);
    return (aBuffer);
}

static stlChar *strECvt(stlFloat64  aFloatValue,
                        stlInt32    aNDigits,
                        stlInt32   *aDecPoint,
                        stlInt32   *aSign,
                        stlChar    *aBuffer)
{
    return (strCvt(aFloatValue, aNDigits, aDecPoint, aSign, 1, aBuffer));
}

static stlChar *strFCvt(stlFloat64  aFloatValue,
                        stlInt32    aNDigits,
                        stlInt32   *aDecPoint,
                        stlInt32   *aSign,
                        stlChar    *aBuffer)
{
    return (strCvt(aFloatValue, aNDigits, aDecPoint, aSign, 0, aBuffer));
}

/*
 * strGCvt  - Floating output conversion to
 * minimal length string
 */

static stlChar *strGCvt(stlFloat64   aFloatValue,
                        stlInt32     aNDigits,
                        stlChar     *aBuffer,
                        stlBool      aAltForm)
{
    stlInt32   sSign;
    stlInt32   sDecPoint;
    stlChar   *sP1;
    stlChar   *sP2;
    stlInt32   i;
    stlChar    sBuffer1[STR_NUM_OF_DIGITS];

    sP1 = strECvt(aFloatValue, aNDigits, &sDecPoint, &sSign, sBuffer1);
    sP2 = aBuffer;
    if(sSign)
    {
        *sP2 = '-';
        sP2 ++;
    }

    for(i = aNDigits - 1; (i > 0) && (sP1[i] == '0'); i--)
    {
        aNDigits--;
    }

    if( ((sDecPoint >= 0) && (sDecPoint - aNDigits > 4))
        || ((sDecPoint < 0) && (sDecPoint < -3)) )         /* use E-style */
    {
        sDecPoint--;
        *sP2 = *sP1;
        sP2++;
        sP1++;

        *sP2 = '.';
        sP2++;

        for(i = 1; i < aNDigits; i++)
        {
            *sP2 = *sP1;
            sP2++;
            sP1++;
        }

        *sP2 = 'e';
        sP2++;

        if(sDecPoint < 0)
        {
            sDecPoint = -sDecPoint;
            *sP2 = '-';
            sP2++;
        }
        else
        {
            *sP2 = '+';
            sP2++;
        }

        if( (sDecPoint / 100) > 0 )
        {
            *sP2 = (sDecPoint / 100) + '0';
            sP2++;
        }
        else
        {
            /* Do Nothing */
        }

        if (sDecPoint / 10 > 0)
        {
            *sP2 = ((sDecPoint % 100) / 10) + '0';
            sP2++;
        }
        else
        {
            /* Do Nothing */
        }

        *sP2 = (sDecPoint % 10) + '0';
        sP2++;
    }
    else
    {
        if(sDecPoint <= 0)
        {
            if(*sP1 != '0')
            {
                *sP2 = '.';
                sP2++;
            }
            else
            {
                /* Do Nothing */
            }

            while(sDecPoint < 0)
            {
                sDecPoint++;
                *sP2 = '0';
                sP2++;
            }
        }
        for(i = 1; i <= aNDigits; i++)
        {
            *sP2 = *sP1;
            sP2++;
            sP1++;
            if(i == sDecPoint)
            {
                *sP2 = '.';
                sP2++;
            }
        }
        if(aNDigits < sDecPoint)
        {
            while(aNDigits++ < sDecPoint)
            {
                *sP2 = '0';
                sP2++;
            }
            *sP2 = '.';
            sP2++;
        }
    }
    if((sP2[-1] == '.') && (aAltForm != STL_YES))
    {
        sP2--;
    }
    else
    {
        /* Do Nothing */
    }

    *sP2 = STR_NULCHAR;
    return(aBuffer);
}

/*
 * The STR_INS_CHAR macro inserts a character in the buffer and writes
 * the buffer back to disk if necessary
 * It uses the stlChar pointers sp and bep:
 *      sp points to the next available character in the buffer
 *      bep points to the end-of-buffer+1
 * While using this macro, note that the nextb pointer is NOT updated.
 *
 * NOTE: Evaluation of the c argument should not have any side-effects
 */
#define STR_INS_CHAR(c, sp, bep, cc)                \
{                                                   \
    if(sp != NULL)                                  \
    {                                               \
        if(sp >= bep)                               \
        {                                           \
            /* Buffer overrun */                    \
            aVBuffer->mCurrentPosition = sp;        \
            if( aFlushFunc == NULL )                \
            {                                       \
                STL_THROW(ERR_BUFFER_OVERRUN);      \
            }                                       \
            aFlushFunc( aVBuffer );                 \
            sp = aVBuffer->mCurrentPosition;        \
            bep = aVBuffer->mEndPosition;           \
        }                                           \
        *sp = (c);                                  \
        sp++;                                       \
    }                                               \
    cc++;                                           \
}

#define STR_NUM(c) (c - '0')

#define STR_TO_DEC(str, num)                        \
    num = STR_NUM(*str);                            \
    str++;                                          \
    while(stlIsdigit(*str) != 0)                    \
    {                                               \
        num *= 10;                                  \
        num += STR_NUM(*str);                       \
        str++;                                      \
    }

/*
 * This macro does zero padding so that the precision
 * requirement is satisfied. The padding is done by
 * adding '0's to the left of the string that is going
 * to be printed. We don't allow precision to be large
 * enough that we continue past the start of s.
 *
 * NOTE: this makes use of the magic info that s is
 * always based on num_buf with a size of STR_NUM_BUF_SIZE.
 */
#define STR_FIX_PRECISION(adjust, precision, s, s_len)    \
    if(adjust == STL_YES) {                               \
        stlSize p = (precision + 1 < STR_NUM_BUF_SIZE)    \
                     ? precision : STR_NUM_BUF_SIZE - 1;  \
        while (s_len < p)                                 \
        {                                                 \
            --s;                                          \
            *s = '0';                                     \
            s_len++;                                      \
        }                                                 \
    }

/*
 * Macro that does padding. The padding is done by printing
 * the character ch.
 */
#define STR_PAD(width, len, ch)                     \
do                                                  \
{                                                   \
    STR_INS_CHAR(ch, sCurrentPosition, sEndPosition, sCharacterCount); \
    width--;                                        \
}                                                   \
while (width > len)


/*
 * Convert aNumValue to its decimal format.
 * Return value:
 *   - a pointer to a string containing the number (no sign)
 *   - aLength contains the length of the string
 *   - aIsNegative is set to TRUE or FALSE depending on the sign
 *     of the number (always set to FALSE if is_unsigned is TRUE)
 *
 * The caller provides a buffer for the string: that is the aBufferEnd argument
 * which is a pointer to the END of the buffer + 1 (i.e. if the buffer
 * is declared as buf[ 100 ], aBufferEnd should be &buf[ 100 ])
 *
 * Note: we have 2 versions. One is used when we need to use quads
 * (strConv10Int64), the other when we don't (strConv10). We're assuming the
 * latter is faster.
 */
static stlChar *strConv10(stlInt32   aNumValue,
                          stlInt32   aIsUnsigned,
                          stlInt32  *aIsNegative,
                          stlChar   *aBufferEnd,
                          stlSize   *aLength)
{
    stlChar    *sBufferPtr = aBufferEnd;
    stlUInt32   sMagnitude = aNumValue;
    stlInt32    sComplement;
    stlUInt32   sNewMagnitude;

    if(aIsUnsigned == STL_TRUE)
    {
        *aIsNegative = STL_FALSE;
    }
    else
    {
        *aIsNegative = (aNumValue < 0) ? STL_TRUE : STL_FALSE;

        /*
         * On a 2's complement machine, negating the most negative integer
         * results in a number that cannot be represented as a signed integer.
         * Here is what we do to obtain the number's magnitude:
         *      a. add 1 to the number
         *      b. negate it (becomes positive)
         *      c. convert it to unsigned
         *      d. add 1
         */
        if (*aIsNegative == STL_TRUE)
        {
            sComplement = aNumValue + 1;
            sMagnitude  = ((stlUInt32) -sComplement) + 1;
        }
        else
        {
            /* Do Nothing */
        }
    }

    /*
     * We use a do-while loop so that we write at least 1 digit
     */
    do
    {
        sNewMagnitude = sMagnitude / 10;

        --sBufferPtr;
        *sBufferPtr = (stlChar)((sMagnitude - (sNewMagnitude * 10)) + '0');
        sMagnitude = sNewMagnitude;
    }
    while(sMagnitude != 0);

    *aLength = aBufferEnd - sBufferPtr;

    return(sBufferPtr);
}

static stlChar *strConv10Int64(stlInt64   aNumValue,
                               stlInt32   aIsUnsigned,
                               stlInt32  *aIsNegative,
                               stlChar   *aBufferEnd,
                               stlSize   *aLength)
{
    stlChar   *sBufferPtr = aBufferEnd;
    stlUInt64  sMagnitude = aNumValue;
    stlInt64   sComplement;
    stlUInt64  sNewMagnitude;

    /*
     * We see if we can use the faster non-int64 version by checking the
     * number against the largest long value it can be. If <=, we
     * punt to the quicker version.
     */
    if( ((sMagnitude <= STL_UINT32_MAX) && (aIsUnsigned == STL_TRUE))
        || ((aNumValue <= STL_INT32_MAX) && (aNumValue >= STL_INT32_MIN) && (aIsUnsigned != STL_TRUE)) )
    {
        sBufferPtr = strConv10((stlInt32)aNumValue, aIsUnsigned, aIsNegative, aBufferEnd, aLength);
        STL_THROW(EXIT_FUNCTION);
    }

    if(aIsUnsigned == STL_TRUE) {
        *aIsNegative = STL_FALSE;
    }
    else
    {
        *aIsNegative = (aNumValue < 0) ? STL_TRUE : STL_FALSE;

        /*
         * On a 2's complement machine, negating the most negative integer
         * results in a number that cannot be represented as a signed integer.
         * Here is what we do to obtain the number's magnitude:
         *      a. add 1 to the number
         *      b. negate it (becomes positive)
         *      c. convert it to unsigned
         *      d. add 1
         */
        if(*aIsNegative == STL_TRUE)
        {
            sComplement = aNumValue + 1;
            sMagnitude = ((stlUInt64) -sComplement) + 1;
        }
    }

    /*
     * We use a do-while loop so that we write at least 1 digit
     */
    do
    {
        sNewMagnitude = sMagnitude / 10;
        --sBufferPtr;
        *sBufferPtr = (stlChar)((sMagnitude - (sNewMagnitude * 10)) + '0');
        sMagnitude = sNewMagnitude;
    }
    while(sMagnitude != 0);

    *aLength = aBufferEnd - sBufferPtr;

    STL_RAMP(EXIT_FUNCTION);

    return(sBufferPtr);
}


/*
 * Convert a floating point number to a string formats 'f', 'F', 'e' or 'E'.
 * The result is placed in aBuffer, and aLength denotes the length of the string
 * The sign is returned in the aIsNegative argument (and is not placed
 * in aBuffer).
 */
static stlChar *strConvFloatingPoint(stlChar     aFormat,
                                     stlFloat64  aFloatValue,
                                     stlBool     aAddDecimalPoint,
                                     stlInt32    aPrecision,
                                     stlInt32   *aIsNegative,
                                     stlChar    *aBuffer,
                                     stlSize    *aLength)
{
    stlChar  *sTargetString = aBuffer;
    stlChar  *sIntermediateString;
    stlInt32  sDecimalPoint;
    stlChar   sBuffer1[STR_NUM_OF_DIGITS];
    stlChar   sTemp[STR_EXPONENT_LENGTH];        /* for exponent conversion */
    stlSize   sExponentStringLength;
    stlInt32  sExponentIsNegative;


    if( (aFormat == 'f') || (aFormat == 'F') )
    {
        sIntermediateString = strFCvt(aFloatValue, aPrecision, &sDecimalPoint, aIsNegative, sBuffer1);
    }
    else /* either e or E format */
    {
        sIntermediateString = strECvt(aFloatValue, aPrecision + 1, &sDecimalPoint, aIsNegative, sBuffer1);
    }

    /*
     * Check for Infinity and NaN
     */
    if(stlIsalpha(*sIntermediateString))
    {
        *aLength = stlStrlen(sIntermediateString);
        stlMemcpy(aBuffer, sIntermediateString, *aLength + 1);
        *aIsNegative = STL_FALSE;

        STL_THROW(EXIT_FUNCTION);
    }

    if( (aFormat == 'f') || (aFormat == 'F') )
    {
        if(sDecimalPoint <= 0)
        {
            *sTargetString = '0';
            sTargetString++;
            if(aPrecision > 0)
            {
                *sTargetString = '.';
                sTargetString++;
                while(sDecimalPoint < 0)
                {
                    sDecimalPoint++;
                    *sTargetString = '0';
                    sTargetString++;
                }
            }
            else
            {
                if(aAddDecimalPoint == STL_YES)
                {
                    *sTargetString = '.';
                    sTargetString++;
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
        else
        {
            while(sDecimalPoint > 0)
            {
                sDecimalPoint--;
                *sTargetString = *sIntermediateString;
                sTargetString++;
                sIntermediateString++;
            }

            if( (aPrecision > 0) || (aAddDecimalPoint == STL_YES) )
            {
                *sTargetString = '.';
                sTargetString++;
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    else
    {
        *sTargetString = *sIntermediateString;
        sTargetString++;
        sIntermediateString++;

        if( (aPrecision > 0) || (aAddDecimalPoint == STL_YES) )
        {
            *sTargetString = '.';
            sTargetString++;
        }
        else
        {
            /* Do Nothing */
        }
    }

    /*
     * copy the rest of sIntermediateString, the STR_NULCHAR is NOT copied
     */
    while(*sIntermediateString != STR_NULCHAR)
    {
        *sTargetString = *sIntermediateString;
        sTargetString++;
        sIntermediateString++;
    }

    if( (aFormat != 'f') && (aFormat != 'F') )
    {
        *sTargetString = aFormat;                /* either e or E */
        sTargetString++;
        sDecimalPoint--;
        if (sDecimalPoint != 0)
        {
            sIntermediateString = strConv10((stlInt32) sDecimalPoint,
                                            STL_FALSE,
                                            &sExponentIsNegative,
                                            &sTemp[STR_EXPONENT_LENGTH],
                                            &sExponentStringLength);
            *sTargetString = sExponentIsNegative == STL_TRUE ? '-' : '+';
            sTargetString++;

            /*
             * Make sure the exponent has at least 2 digits
             */
            if(sExponentStringLength == 1)
            {
                *sTargetString = '0';
                sTargetString++;
            }
            else
            {
                /* Do Nothing */
            }

            while(sExponentStringLength != 0)
            {
                sExponentStringLength--;
                *sTargetString = *sIntermediateString;
                sTargetString++;
                sIntermediateString++;
            }
        }
        else
        {
            *sTargetString = '+';
            sTargetString++;
            *sTargetString = '0';
            sTargetString++;
            *sTargetString = '0';
            sTargetString++;
        }
    }

    *aLength = sTargetString - aBuffer;

    STL_RAMP(EXIT_FUNCTION);

    return (aBuffer);
}


/*
 * Convert aNumberValue to a base X number where X is a power of 2. aNBits determines X.
 * For example, if aNBits is 3, we do base 8 conversion
 * Return value:
 *      a pointer to a string containing the number
 *
 * The caller provides a buffer for the string: that is the aBufferEnd argument
 * which is a pointer to the END of the buffer + 1 (i.e. if the buffer
 * is declared as buf[ 100 ], aBufferEnd should be &buf[ 100 ])
 *
 * As with strConv10, we have a faster version which is used when
 * the number isn't int64 size.
 */
static stlChar *strConvPowerOf2(stlUInt32  aNumberValue,
                                stlInt32   aNBits,
                                stlChar    aFormat,
                                stlChar   *aBufferEnd,
                                stlSize   *aLength)
{
    stlInt32              sMask = (1 << aNBits) - 1;
    stlChar              *sReturnBuffer = aBufferEnd;
    const stlChar        *sDigits = (aFormat == 'X') ? strUpperDigits : strLowDigits;

    do
    {
        --sReturnBuffer;
        *sReturnBuffer = sDigits[aNumberValue & sMask];
        aNumberValue >>= aNBits;
    }
    while(aNumberValue != 0);

    *aLength = aBufferEnd - sReturnBuffer;

    return(sReturnBuffer);
}

static stlChar *strConvPowerOf2Int64(stlUInt64  aNumberValue,
                                     stlInt32   aNBits,
                                     stlChar    aFormat,
                                     stlChar   *aBufferEnd,
                                     stlSize   *aLength)
{
    stlInt32              sMask = (1 << aNBits) - 1;
    stlChar              *sReturnBuffer = aBufferEnd;
    const stlChar        *sDigits = (aFormat == 'X') ? strUpperDigits : strLowDigits;

    if(aNumberValue <= STL_UINT32_MAX)
    {
        sReturnBuffer = strConvPowerOf2((stlUInt32)aNumberValue,
                                        aNBits,
                                        aFormat,
                                        aBufferEnd,
                                        aLength);
        STL_THROW(EXIT_FUNCTION);
    }
    else
    {
        /* Do Nothing */
    }

    do
    {
        --sReturnBuffer;
        *sReturnBuffer = sDigits[aNumberValue & sMask];
        aNumberValue >>= aNBits;
    }
    while(aNumberValue != 0);

    *aLength = aBufferEnd - sReturnBuffer;

    STL_RAMP(EXIT_FUNCTION);
        
    return (sReturnBuffer);
}

/*
 * Do format conversion placing the output in buffer
 */
stlInt32 strVformatter( void                (* aFlushFunc)(strVformatterBuffer *),
                        strVformatterBuffer  * aVBuffer,
                        stlChar              * aFormat,
                        va_list                aVarArgList )
{
    stlChar        *sCurrentPosition;
    stlChar        *sEndPosition;
    stlInt32        sCharacterCount = 0;
    stlSize         i;

    stlChar        *sStringBuffer = NULL;
    stlChar        *sSubString;
    stlSize         sStringBufferLength = 0;

    stlSize         sMinWidth = 0;
    stlSize         sPrecision = 0;
    strDirection    sAdjust;
    stlChar         sPadChar;
    stlChar         sPrefixChar;

    stlFloat64      sFPNum;
    stlInt64        sInt64 = 0;
    stlUInt64       sUInt64;
    stlInt32        sINum = 0;
    stlUInt32       sUINum;

    stlChar         sNumBuffer[STR_NUM_BUF_SIZE];
    stlChar         sCharBuffer[2];                /* for printing %% and %<unknown> */

    strVarTypeEnum  sVarType = STR_IS_INT32;
    stlChar        *sWalk;
    /*
     * Flag variables
     */
    stlBool         sAlternateForm;
    stlBool         sPrintSign;
    stlBool         sPrintBlank;
    stlBool         sPrintSomething;
    stlBool         sAdjustPrecision;
    stlBool         sAdjustWidth;
    stlInt32        sIsNegative;

    /*
     * va_arg types
     */
    stlInt32        sTypeInt32;

    sCurrentPosition = aVBuffer->mCurrentPosition;
    sEndPosition = aVBuffer->mEndPosition;

    while(*aFormat != STR_NULCHAR)
    {
        if (*aFormat != '%')
        {
            STR_INS_CHAR(*aFormat, sCurrentPosition, sEndPosition, sCharacterCount);
        }
        else
        {
            /*
             * Default variable settings
             */
            sPrintSomething = STL_YES;
            sAdjust = STR_RIGHT;
            sAlternateForm = STL_NO;
            sPrintSign = STL_NO;
            sPrintBlank = STL_NO;
            sPadChar = ' ';
            sPrefixChar = STR_NULCHAR;

            aFormat++;

            /*
             * Try to avoid checking for flags, width or precision
             */
            if(stlIslower(*aFormat) == 0)
            {
                /*
                 * Recognize flags: -, #, BLANK, +
                 */
                for(;; aFormat++)
                {
                    if (*aFormat == '-')
                    {
                        sAdjust = STR_LEFT;
                    }
                    else if (*aFormat == '+')
                    {
                        sPrintSign = STL_YES;
                    }
                    else if (*aFormat == '#')
                    {
                        sAlternateForm = STL_YES;
                    }
                    else if (*aFormat == ' ')
                    {
                        sPrintBlank = STL_YES;
                    }
                    else if (*aFormat == '0')
                    {
                        sPadChar = '0';
                    }
                    else
                    {
                        break;
                    }
                }

                /*
                 * Check if a width was specified
                 */
                if(stlIsdigit(*aFormat) != 0)
                {
                    STR_TO_DEC(aFormat, sMinWidth);
                    sAdjustWidth = STL_YES;
                }
                else
                {
                    if(*aFormat == '*')
                    {
                        stlInt32 sTypeInt32 = va_arg(aVarArgList, stlInt32);
                        aFormat++;
                        sAdjustWidth = STL_YES;
                        if(sTypeInt32 < 0)
                        {
                            sAdjust = STR_LEFT;
                            sMinWidth = (stlSize)(-sTypeInt32);
                        }
                        else
                        {
                            sMinWidth = (stlSize)sTypeInt32;
                        }
                    }
                    else
                    {
                        sAdjustWidth = STL_NO;
                    }
                }

                /*
                 * Check if a precision was specified
                 */
                if(*aFormat == '.')
                {
                    sAdjustPrecision = STL_YES;
                    aFormat++;
                    if(stlIsdigit(*aFormat) != 0)
                    {
                        STR_TO_DEC(aFormat, sPrecision);
                    }
                    else
                    {
                        if (*aFormat == '*')
                        {
                            sTypeInt32 = va_arg(aVarArgList, stlInt32);
                            aFormat++;
                            sPrecision = (sTypeInt32 < 0) ? 0 : (stlSize)sTypeInt32;
                        }
                        else
                        {
                            sPrecision = 0;
                        }
                    }
                }
                else
                {
                    sAdjustPrecision = STL_NO;
                }
            }
            else
            {
                sAdjustPrecision = STL_NO;
                sAdjustWidth = STL_NO;
            }

            if(*aFormat == 'l')
            {
                sVarType = STR_IS_INT64;
                aFormat++;
            }
            else if(*aFormat == 'h')
            {
                sVarType = STR_IS_INT16;
                aFormat++;
            }
            else
            {
                sVarType = STR_IS_INT32;
            }

            /*
             * Argument extraction and printing.
             * First we determine the argument type.
             * Then, we convert the argument to a string.
             * On exit from the switch, s points to the string that
             * must be printed, sStringBufferLength has the length of the string
             * The sPrecision requirements, if any, are reflected in sStringBufferLength.
             *
             * NOTE: sPadChar may be set to '0' because of the 0 flag.
             *   It is reset to ' ' by non-numeric formats
             */
            switch(*aFormat)
            {
                case 'u':
                    if(sVarType == STR_IS_INT64)
                    {
                        sInt64 = va_arg(aVarArgList, stlUInt64);
                        sStringBuffer = strConv10Int64(sInt64,
                                                       STL_TRUE,
                                                       &sIsNegative,
                                                       &sNumBuffer[STR_NUM_BUF_SIZE],
                                                       &sStringBufferLength);
                    }
                    else
                    {
                        if(sVarType == STR_IS_INT16)
                        {
                            sINum = (stlInt32) (stlUInt16) va_arg(aVarArgList, stlUInt32);
                        }
                        else
                        {
                            sINum = (stlInt32) va_arg(aVarArgList, stlUInt32);
                        }

                        sStringBuffer = strConv10(sINum,
                                                  STL_TRUE,
                                                  &sIsNegative,
                                                  &sNumBuffer[STR_NUM_BUF_SIZE],
                                                  &sStringBufferLength);
                    }
                    STR_FIX_PRECISION(sAdjustPrecision, sPrecision, sStringBuffer, sStringBufferLength);
                    break;

                case 'd':
                case 'i':
                    if(sVarType == STR_IS_INT64)
                    {
                        sInt64 = va_arg(aVarArgList, stlInt64);
                        sStringBuffer = strConv10Int64(sInt64,
                                                       STL_FALSE,
                                                       &sIsNegative,
                                                       &sNumBuffer[STR_NUM_BUF_SIZE],
                                                       &sStringBufferLength);
                    }
                    else
                    {
                        if(sVarType == STR_IS_INT16)
                        {
                            sINum = (stlInt16) va_arg(aVarArgList, stlInt32);
                        }
                        else
                        {
                            sINum = va_arg(aVarArgList, stlInt32);
                        }

                        sStringBuffer = strConv10(sINum,
                                                  STL_FALSE,
                                                  &sIsNegative,
                                                  &sNumBuffer[STR_NUM_BUF_SIZE],
                                                  &sStringBufferLength);
                    }
                
                    STR_FIX_PRECISION(sAdjustPrecision, sPrecision, sStringBuffer, sStringBufferLength);

                    if(sIsNegative == STL_TRUE)
                    {
                        sPrefixChar = '-';
                    }
                    else
                    {
                        if(sPrintSign == STL_YES)
                        {
                            sPrefixChar = '+';
                        }
                        else
                        {
                            if(sPrintBlank == STL_YES)
                            {
                                sPrefixChar = ' ';
                            }
                            else
                            {
                                /* Do Nothing */
                            }
                        }
                    }
                    break;

                case 'o':
                    if(sVarType == STR_IS_INT64)
                    {
                        sUInt64 = va_arg(aVarArgList, stlUInt64);
                        sStringBuffer = strConvPowerOf2Int64(sUInt64,
                                                             3,
                                                             *aFormat,
                                                             &sNumBuffer[STR_NUM_BUF_SIZE],
                                                             &sStringBufferLength);
                    }
                    else
                    {
                        if(sVarType == STR_IS_INT16)
                        {
                            sUINum = (stlUInt16) va_arg(aVarArgList, stlUInt32);
                        }
                        else
                        {
                            sUINum = va_arg(aVarArgList, stlUInt32);
                        }
                        
                        sStringBuffer = strConvPowerOf2(sUINum,
                                                        3,
                                                        *aFormat,
                                                        &sNumBuffer[STR_NUM_BUF_SIZE],
                                                        &sStringBufferLength);
                    }
                    STR_FIX_PRECISION(sAdjustPrecision, sPrecision, sStringBuffer, sStringBufferLength);
                    if( (sAlternateForm == STL_YES) && (*sStringBuffer != '0') )
                    {
                        --sStringBuffer;
                        *sStringBuffer = '0';
                        sStringBufferLength++;
                    }
                    break;


                case 'x':
                case 'X':
                    if(sVarType == STR_IS_INT64)
                    {
                        sUInt64 = va_arg(aVarArgList, stlUInt64);
                        sStringBuffer = strConvPowerOf2Int64(sUInt64,
                                                             4,
                                                             *aFormat,
                                                             &sNumBuffer[STR_NUM_BUF_SIZE],
                                                             &sStringBufferLength);
                    }
                    else
                    {
                        if(sVarType == STR_IS_INT16)
                        {
                            sUINum = (stlUInt16) va_arg(aVarArgList, stlUInt32);
                        }
                        else
                        {
                            sUINum = va_arg(aVarArgList, stlUInt32);
                        }

                        sStringBuffer = strConvPowerOf2(sUINum,
                                                        4,
                                                        *aFormat,
                                                        &sNumBuffer[STR_NUM_BUF_SIZE],
                                                        &sStringBufferLength);
                    }
                    STR_FIX_PRECISION(sAdjustPrecision, sPrecision, sStringBuffer, sStringBufferLength);
                    if( (sAlternateForm == STL_YES) && (sINum != 0) )
                    {
                        --sStringBuffer;
                        *sStringBuffer = *aFormat;        /* 'x' or 'X' */
                        --sStringBuffer;
                        *sStringBuffer = '0';
                        sStringBufferLength += 2;
                    }
                    break;

                case 's':
                    sStringBuffer = va_arg(aVarArgList, stlChar *);
                    if(sStringBuffer != NULL)
                    {
                        if(sAdjustPrecision == STL_NO)
                        {
                            sStringBufferLength = stlStrlen(sStringBuffer);
                        }
                        else
                        {
                            /* From the C library standard in section 7.9.6.1:
                             * ...if the precision is specified, no more then
                             * that many characters are written.  If the
                             * precision is not specified or is greater
                             * than the size of the array, the array shall
                             * contain a null character.
                             *
                             * My reading is is precision is specified and
                             * is less then or equal to the size of the
                             * array, no null character is required.  So
                             * we can't do a stlStrlen.
                             *
                             * This figures out the length of the string
                             * up to the precision.  Once it's long enough
                             * for the specified precision, we don't care
                             * anymore.
                             *
                             * NOTE: you must do the length comparison
                             * before the check for the null character.
                             * Otherwise, you'll check one beyond the
                             * last valid character.
                             */
                            for (sWalk = sStringBuffer, sStringBufferLength = 0;
                                 (sStringBufferLength < sPrecision) && (*sWalk != STR_NULCHAR);
                                 ++sWalk, ++sStringBufferLength)
                            {
                                /* Do Nothing */
                            }
                        }
                    }
                    else
                    {
                        sStringBuffer = STR_NULL_STRING;
                        sStringBufferLength = STR_NULL_STRING_LEN;
                    }
                    sPadChar = ' ';
                    break;

                case 'f':
                case 'F':

                case 'e':
                case 'E':
                    sFPNum = va_arg(aVarArgList, stlFloat64);
                    /*
                     * We use &sNumBuffer[ 1 ], so that we have room for the sign
                     */
                    sStringBuffer = NULL;
#ifdef STC_HAVE_ISNAN
                    if(isnan(sFPNum) != 0)
                    {
                        sStringBuffer = "nan";
                        sStringBufferLength = 3;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
#endif
#ifdef STC_HAVE_ISINF
                    if( (sStringBuffer == NULL) && (isinf(sFPNum) != 0) )
                    {
                        sStringBuffer = "inf";
                        sStringBufferLength = 3;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
#endif
                    if(sStringBuffer == NULL)
                    {
                        sStringBuffer = strConvFloatingPoint(*aFormat,
                                                             sFPNum,
                                                             sAlternateForm,
                                                             (stlInt32)((sAdjustPrecision == STL_NO) ? STR_FLOAT_DIGITS : sPrecision),
                                                             &sIsNegative,
                                                             &sNumBuffer[1],
                                                             &sStringBufferLength);
                        if(sIsNegative == STL_TRUE)
                        {
                            sPrefixChar = '-';
                        }
                        else
                        {
                            if(sPrintSign == STL_YES)
                            {
                                sPrefixChar = '+';
                            }
                            else
                            {
                                if(sPrintBlank == STL_YES)
                                {
                                    sPrefixChar = ' ';
                                }
                                else
                                {
                                    /* Do Nothing */
                                }
                            }
                        }
                    }
                    break;

                case 'g':
                case 'G':
                    if(sAdjustPrecision == STL_NO)
                    {
                        sPrecision = STR_FLOAT_DIGITS;
                    }
                    else
                    {
                        if(sPrecision == 0)
                        {
                            sPrecision = 1;
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }

                    /*
                     * * We use &sNumBuffer[ 1 ], so that we have room for the sign
                     */
                    sStringBuffer = strGCvt(va_arg(aVarArgList, stlFloat64),
                                            (stlInt32) sPrecision,
                                            &sNumBuffer[1],
                                            sAlternateForm);
                    if(*sStringBuffer == '-')
                    {
                        sPrefixChar = *sStringBuffer++;
                    }
                    else
                    {
                        if(sPrintSign == STL_YES)
                        {
                            sPrefixChar = '+';
                        }
                        else
                        {
                            if(sPrintBlank == STL_YES)
                            {
                                sPrefixChar = ' ';
                            }
                            else
                            {
                                /* Do Nothing */
                            }
                        }
                    }

                    sStringBufferLength = stlStrlen(sStringBuffer);
                    sSubString = strchr(sStringBuffer, '.');
                    if( (sAlternateForm == STL_YES) && (sSubString == NULL) )
                    {
                        sStringBuffer[sStringBufferLength] = '.';
                        sStringBufferLength++;
                        sStringBuffer[sStringBufferLength] = STR_NULCHAR; /* delimit for following strchr() */
                    }

                    sSubString = strchr(sStringBuffer, 'e');
                    if( (*aFormat == 'G') && (sSubString != NULL) )
                    {
                        *sSubString = 'E';
                    }
                    else
                    {
                        /* Do Nothing */
                    }

                    break;

                case 'c':
                    sCharBuffer[0] = (stlChar) (va_arg(aVarArgList, stlInt32));
                    sStringBuffer = &sCharBuffer[0];
                    sStringBufferLength = 1;
                    sPadChar = ' ';
                    break;


                case '%':
                    sCharBuffer[0] = '%';
                    sStringBuffer = &sCharBuffer[0];
                    sStringBufferLength = 1;
                    sPadChar = ' ';
                    break;


                case 'n':
                    if(sVarType == STR_IS_INT64)
                    {
                        *(va_arg(aVarArgList, stlInt64 *)) = sCharacterCount;
                    }
                    else if(sVarType == STR_IS_INT16)
                    {
                        *(va_arg(aVarArgList, stlInt16 *)) = sCharacterCount;
                    }
                    else
                    {
                        *(va_arg(aVarArgList, stlInt32 *)) = sCharacterCount;
                    }
                    sPrintSomething = STL_NO;
                    break;

                case 'P':
#if STL_SIZEOF_VOIDP == 8
                    if(STL_SIZEOF(void *) <= STL_SIZEOF(stlUInt64))
                    {
                        sUInt64 = (stlUInt64) va_arg(aVarArgList, void *);
                        sStringBuffer = strConvPowerOf2Int64(sUInt64,
                                                             4,
                                                             'X',
                                                             &sNumBuffer[STR_NUM_BUF_SIZE],
                                                             &sStringBufferLength);
                    }
#else
                    if(STL_SIZEOF(void *) <= STL_SIZEOF(stlUInt32))
                    {
                        sUINum = (stlUInt32) va_arg(aVarArgList, void *);
                        sStringBuffer = strConvPowerOf2(sUINum,
                                                        4,
                                                        'X',
                                                        &sNumBuffer[STR_NUM_BUF_SIZE],
                                                        &sStringBufferLength);
                    }
#endif
                    else {
                        sStringBuffer = "%P";
                        sStringBufferLength = 2;
                        sPrefixChar = STR_NULCHAR;
                    }
                    sPadChar = ' ';
                    break;

                case 'p':
#if STL_SIZEOF_VOIDP == 8
                    if(STL_SIZEOF(void *) <= STL_SIZEOF(stlUInt64))
                    {
                        sUInt64 = (stlUInt64) va_arg(aVarArgList, void *);
                        sStringBuffer = strConvPowerOf2Int64(sUInt64,
                                                             4,
                                                             'x',
                                                             &sNumBuffer[STR_NUM_BUF_SIZE],
                                                             &sStringBufferLength);
                    }
#else
                    if(STL_SIZEOF(void *) <= STL_SIZEOF(stlUInt32))
                    {
                        sUINum = (stlUInt32) va_arg(aVarArgList, void *);
                        sStringBuffer = strConvPowerOf2(sUINum,
                                                        4,
                                                        'x',
                                                        &sNumBuffer[STR_NUM_BUF_SIZE],
                                                        &sStringBufferLength);
                    }
#endif
                    else {
                        sStringBuffer = "%p";
                        sStringBufferLength = 2;
                        sPrefixChar = STR_NULCHAR;
                    }
                    sPadChar = ' ';
                    break;

                case STR_NULCHAR:
                    /*
                     * The last character of the format string was %.
                     * We ignore it.
                     */
                    continue;


                    /*
                     * The default case is for unrecognized %'s.
                     * We print %<char> to help the user identify what
                     * option is not understood.
                     * This is also useful in case the user wants to pass
                     * the output of format_converter to another function
                     * that understands some other %<char> (like syslog).
                     * Note that we can't point s inside aFormat because the
                     * unknown <char> could be preceded by width etc.
                     */
                default:
                    sCharBuffer[0] = '%';
                    sCharBuffer[1] = *aFormat;
                    sStringBuffer = sCharBuffer;
                    sStringBufferLength = 2;
                    sPadChar = ' ';
                    break;
            }

            if( (sPrefixChar != STR_NULCHAR) &&
                (sStringBuffer != STR_NULL_STRING) &&
                (sStringBuffer != sCharBuffer) )
            {
                --sStringBuffer;
                *sStringBuffer = sPrefixChar;
                sStringBufferLength++;
            }

            if( (sAdjustWidth == STL_YES) &&
                (sAdjust == STR_RIGHT) &&
                (sMinWidth > sStringBufferLength) )
            {
                if( (sPadChar == '0') && (sPrefixChar != STR_NULCHAR) )
                {
                    STR_INS_CHAR(*sStringBuffer, sCurrentPosition, sEndPosition, sCharacterCount);
                    sStringBuffer++;
                    sStringBufferLength--;
                    sMinWidth--;
                }
                STR_PAD(sMinWidth, sStringBufferLength, sPadChar);
            }

            /*
             * Print the string s.
             */
            if( sPrintSomething == STL_YES )
            {
                for(i = sStringBufferLength; i != 0; i--)
                {
                    STR_INS_CHAR(*sStringBuffer, sCurrentPosition, sEndPosition, sCharacterCount);
                    sStringBuffer++;
                }
            }

            if( (sAdjustWidth == STL_YES) &&
                (sAdjust == STR_LEFT) &&
                (sMinWidth > sStringBufferLength) )
            {
                STR_PAD(sMinWidth, sStringBufferLength, sPadChar);
            }
        }
        aFormat++;
    }
    aVBuffer->mCurrentPosition = sCurrentPosition;

    return sCharacterCount;

    STL_CATCH(ERR_BUFFER_OVERRUN);
    {
        sCharacterCount = -1;
    }
    STL_FINISH;

    return sCharacterCount;
}


stlInt32 strSnprintf(stlChar        *aOutputBuffer,
                     stlSize         aOutputBufferLength,
                     const stlChar  *aFormat,
                     ...)
{
    stlInt32             sCharacterCount;
    va_list              sVarArgList;
    strVformatterBuffer  sVBuffer;

    if (aOutputBufferLength == 0)
    {
        /* NOTE: This is a special case; we just want to return the number
         * of chars that would be written (minus \0) if the buffer
         * size was infinite. We leverage the fact that STR_INS_CHAR
         * just does actual inserts iff the buffer pointer is non-NULL.
         * In this case, we don't care what aOutputBuffer is; it can be NULL, since
         * we don't touch it at all.
         */
        sVBuffer.mCurrentPosition = NULL;
        sVBuffer.mEndPosition = NULL;
    }
    else
    {
        /* save one byte for nul terminator */
        sVBuffer.mCurrentPosition = aOutputBuffer;
        sVBuffer.mEndPosition = aOutputBuffer + aOutputBufferLength - 1;
    }

    va_start(sVarArgList, aFormat);
    sCharacterCount = strVformatter( NULL,
                                     &sVBuffer,
                                     (stlChar *)aFormat,
                                     sVarArgList );
    va_end(sVarArgList);
    if(aOutputBufferLength != 0)
    {
        *sVBuffer.mCurrentPosition = STR_NULCHAR;
    }
    return (sCharacterCount == -1) ? (stlInt32)aOutputBufferLength - 1 : sCharacterCount;
}


stlInt32 strVsnprintf(stlChar        *aOutputBuffer,
                      stlSize         aOutputBufferLength,
                      const stlChar  *aFormat,
                      va_list         aVarArgList)
{
    stlInt32             sCharacterCount;
    strVformatterBuffer  sVBuffer;

    if (aOutputBufferLength == 0)
    {
        /* See above note */
        sVBuffer.mCurrentPosition = NULL;
        sVBuffer.mEndPosition = NULL;
    }
    else
    {
        /* save one byte for nul terminator */
        sVBuffer.mCurrentPosition = aOutputBuffer;
        sVBuffer.mEndPosition = aOutputBuffer + aOutputBufferLength - 1;
    }

    sCharacterCount = strVformatter( NULL,
                                     &sVBuffer,
                                     (stlChar *)aFormat,
                                     aVarArgList );
    if(aOutputBufferLength != 0)
    {
        *sVBuffer.mCurrentPosition = STR_NULCHAR;
    }
    return (sCharacterCount == -1) ? (stlInt32)aOutputBufferLength - 1 : sCharacterCount;
}
