
/*******************************************************************************
 * zlvSqlToC.c
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

#include <cml.h>
#include <goldilocks.h>
#include <zlDef.h>
#include <zle.h>
#include <zlvSqlToC.h>

/**
 * @file zlvSqlToC.c
 * @brief Gliese API Internal Converting Data from SQL to C Data Types Routines.
 */

/**
 * @addtogroup zlvSqlToC
 * @{
 */

#define ZLV_C_NUMERIC_VAL_MAX_DIGIT   39
#define ZLV_C_NUMERIC_SCALE_MAX_DIGIT 5

stlInt16 zlvGetNumericPrecision( stlFloat64 aValue )
{
    return ( (stlInt32)( (stlInt32)(stlLog10( aValue ) ) / 2 ) + 1 ) * 2;
}

stlInt16 zlvGetNumericPrecisionuFromUInt64( stlUInt64 aValue )
{
    stlUInt128   sUInt128Val;
    stlInt64     sPrecision;
    
    sUInt128Val.mHighDigit = 0;
    sUInt128Val.mLowDigit  = aValue;

    stlGetPrecisionUInt128( sUInt128Val, sPrecision );
    
    return ( (stlInt32)( ( sPrecision - 1 ) / 2 ) + 1 ) * 2;
}

stlStatus zlvMakeCNumericFromString( zlsStmt            * aStmt,
                                     SQL_NUMERIC_STRUCT * aCNumeric,
                                     stlChar            * aString,
                                     stlInt32             aLength,
                                     stlInt32             aPrecision,
                                     stlInt16             aScale,
                                     stlBool            * aIsTruncated,
                                     stlErrorStack      * aErrorStack )
{
    stlInt32    sStringScale   = 0;
    stlInt32    sScale         = 0;
    stlInt32    sTmpScale      = 0;
    stlInt32    sRealExponent  = 0;
    stlInt32    sLeadingZero   = 0;
    stlInt32    sTrailingZero  = 0;
    stlInt32    sDigitCount    = 0;
    stlChar     sDigitString[ZLV_C_NUMERIC_VAL_MAX_DIGIT + 1];
    stlInt64    sDigitStringLength;
    stlInt64    sExponentDigit = 0;
    stlInt32    sTruncatedCount = 0;
    stlChar   * sPointPos      = NULL;
    stlChar   * sStart         = NULL;
    stlChar   * sEndPos        = NULL;
    stlChar   * sDigitEndPos   = NULL;
    stlChar   * sCurrent       = NULL;
    stlChar   * sExponentPos   = NULL;
    stlChar   * sParseEndInt64 = NULL;
    stlChar   * sTmpPos        = NULL;
    stlInt64    sExpValue      = 0;
    stlInt32    sIdx           = 0;
    stlInt32    sIdx2;
    stlInt32    sVar;
    stlInt32    sVarLength     = 0;
    stlInt32    sZeroLen       = 0;
    SQLCHAR   * sNumericVal;
    stlBool     sIsTruncated   = STL_FALSE;
    stlBool     sIsValid       = STL_FALSE;
    stlStatus   sRet           = STL_FAILURE;

    
    aCNumeric->precision = aPrecision;
    *aIsTruncated = STL_FALSE;

    /**
     * erase leading space
     */
    for( sIdx = 0; sIdx < aLength; sIdx++ )
    {
        if( stlIsspace( aString[sIdx] ) == STL_FALSE )
        {
            sStart = &aString[sIdx];
            break;
        }
    }

    /**
     * set sign
     */
    if( *sStart == '-' )
    {
        aCNumeric->sign = 0;
        sStart++;
    }
    else if( *sStart == '+' )
    {
        aCNumeric->sign = 1;
        sStart++;
    }
    else
    {
        aCNumeric->sign = 1;
    }

    /**
     * erase space
     * ex ) ' + 123' 값도 유효한 값이다.
     */
    while( sStart != &aString[aLength-1] )
    {
        if( stlIsspace( *sStart ) == STL_FALSE )
        {
            break;
        }
        else
        {
            sStart++;
        }
    }

    /**
     * 문자열에 유효한 숫자가 없는 경우 0 이다.
     * ex ) ' ' , ' - ' , ' + '
     */
    if( sStart == &aString[aLength] )
    {
        sDigitString[0] = '0';
        sIdx            = 1;
        aCNumeric->sign = 1;

        STL_THROW( RAMP_SKIP_SET_VALUE );
    }

    /**
     * 0.XXXXXX 인 경우 0 을 무시한다.
     */
    while( *sStart == '0' )
    {
        sStart++;
    }

    for( sCurrent = aString + aLength - 1; sCurrent >= sStart; sCurrent-- )
    {
        if( stlIsspace( *sCurrent ) == STL_TRUE )
        {
            aLength--;
        }
        else
        {
            break;
        }
    }
    
    sEndPos = aString + aLength;

    for( sCurrent = sStart; sCurrent < sEndPos; sCurrent++ )
    {
        if( ( *sCurrent == 'E' ) || ( *sCurrent == 'e' ) )
        {
            sExponentPos = sCurrent;
        }
        else if( *sCurrent == '.' )
        {
            sPointPos = sCurrent;
        }
    }

    if( sExponentPos == NULL )
    {
        sDigitEndPos = sEndPos;
    }
    else
    {
        sDigitEndPos = sExponentPos;
    }

    if( sPointPos != NULL )
    {
        /**
         * 12345.12345
         * .123...
         * .000123
         */
        sStringScale = sDigitEndPos - sPointPos - 1;

        /**
         * erase to trailing zero
         * 12345.123450000
         * .00012300000
         * .12300000000
         */
        sCurrent = sDigitEndPos -1;
        while( *sCurrent == '0' )
        {
            sStringScale--;
            sDigitEndPos--;
            sCurrent--;
        }

        if( *sStart == '.' )
        {
            sCurrent = sStart + 1;
            while( *sCurrent == '0' )
            {
                sCurrent++;
                sLeadingZero++;
            }
        }

        sDigitCount = sDigitEndPos - sStart - 1 /* decimal point */ - sLeadingZero;
        sRealExponent = (sStart == sPointPos) ?
            -( sLeadingZero + 1 ) :
            ( sPointPos - sStart -1 );
    }
    else
    {
        /**
         * 1234500000
         */
        sZeroLen = 0;
        sCurrent = sDigitEndPos -1;
        while( *sCurrent == '0' )
        {
            sStringScale--;
            sDigitEndPos--;
            sCurrent--;
        }
        

        sDigitCount = sDigitEndPos - sStart - sZeroLen;
        sRealExponent = sDigitEndPos - sStart -1 - sStringScale;
    }

    /**
     * Digit 파트가 의미적으로 0 값인 경우
     * ex ) '0000000'
     */
    if( sDigitCount < 0 )
    {
        sDigitString[0] = '0';
        sIdx            = 1;
        aCNumeric->sign = 1;

        STL_THROW( RAMP_SKIP_SET_VALUE );
    }

    if( sExponentPos != NULL )
    {
        sExponentDigit = sEndPos - sExponentPos - 1;

        STL_TRY_THROW( sExponentDigit < ZLV_C_NUMERIC_SCALE_MAX_DIGIT,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        STL_TRY( stlStrToInt64( sExponentPos + 1,
                                sExponentDigit,
                                &sParseEndInt64,
                                10,
                                &sExpValue,
                                aErrorStack ) == STL_SUCCESS );

        STL_TRY_THROW( ( sParseEndInt64 == NULL ) ||
                       ( sParseEndInt64 == ( sExponentPos + sExponentDigit + 1 ) ),
                       RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );

        sStringScale -= sExpValue;
        sRealExponent += sExpValue;
    }
    else
    {
        /* do nothing */
    }
    
    

    /**
     * 소스 데이터 validation check
     */
    STL_TRY_THROW( sDigitCount <= ZLV_CHAR_TO_C_NUMERIC_PRECISION,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

    STL_TRY_THROW( (-ZLV_CHAR_TO_C_NUMERIC_MIN_SCALE + ZLV_CHAR_TO_C_NUMERIC_PRECISION -1) >= sRealExponent ,
                   RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
   
    /**
     * 사용자 scale 적용 후 validation check
     */
    if( sStringScale <= 0 )
    {
        /**
         * ex ) 123
         *      123000
         */
        if( aScale >= 0 )
        {
            /**
             * precision overflow
             */
            STL_TRY_THROW( sDigitCount + (-sStringScale) + (aScale) <= ZLV_CHAR_TO_C_NUMERIC_PRECISION,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

            sScale = sStringScale;
            sTruncatedCount = 0;
        }
        else if( aScale < 0 )
        {
            /**
             * 정수 digit trunc 시
             */
            STL_TRY_THROW( (-sStringScale + aScale) >= 0,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

            if(  sStringScale < ZLV_CHAR_TO_C_NUMERIC_MIN_SCALE )
            {
                sTrailingZero = (-sStringScale) + ZLV_CHAR_TO_C_NUMERIC_MIN_SCALE;
                sScale        = ZLV_CHAR_TO_C_NUMERIC_MIN_SCALE;
            }
            else
            {
                sScale = sStringScale;
            }
            sTruncatedCount = 0;
        }
    }
    else if( sStringScale < sDigitCount )
    {
        /**
         * ex ) 123.123
         *      123000.123
         */
        if( aScale >= 0 )
        {
            STL_TRY_THROW( sDigitCount + (-sStringScale) + (aScale) <= ZLV_CHAR_TO_C_NUMERIC_PRECISION,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

            sTmpScale = aScale - sStringScale;
            if( sTmpScale >= 0 )
            {
                sScale = sStringScale;
                sTruncatedCount = 0;
            }
            else
            {
                sScale = aScale;
                sTruncatedCount = sTmpScale;

                sIsTruncated = STL_TRUE;
            }
        }
        else if( aScale < 0 )
        {
            /**
             * ex ) 12300.123 , aScale = -3 ( error )
             *      12300.123 , aScale = -2 ( trunc )
             */
            sTmpPos   = sPointPos -1;
            sTmpScale = 0;
            for( sTmpPos = sPointPos -1; sTmpPos != sStart; sTmpPos-- )
            {
                if( *sTmpPos != '0' )
                {
                    break;
                }

                sTmpScale++;
            }
            
            sIsTruncated    = STL_TRUE;
            sTruncatedCount = - sTmpScale - sStringScale;
            sScale = -sTmpScale;

            STL_TRY_THROW( sTmpScale + aScale >= 0,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        }
    }
    else
    {
        /**
         * ex ) 0.123
         *      0.000123
         */
        sLeadingZero = sStringScale - sDigitCount;
        if( aScale >= 0 )
        {
            /**
             * 소수점 trunc
             */
            
            STL_TRY_THROW( aScale - sLeadingZero <= ZLV_CHAR_TO_C_NUMERIC_PRECISION,
                           RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

            sTmpScale = aScale - sStringScale;
            if( sTmpScale >= 0 )
            {
                sScale          = sStringScale;
                sTruncatedCount = 0;
            }
            else if( sTmpScale < 0 )
            {
                sScale          = aScale;
                sTruncatedCount = sTmpScale;
                
                sIsTruncated = STL_TRUE;
            }
        }
        else if( aScale < 0 )
        {
            sTruncatedCount = -sDigitCount;
            
            sIsTruncated = STL_TRUE;
        }
    }

    /**
     * Digit 복사
     */
    while( (sTruncatedCount != 0) && (sDigitEndPos != sStart) )
    {
        if( stlIsdigit( *(sDigitEndPos-1) ) == STL_TRUE )
        {
            sTruncatedCount++;
        }
        sDigitEndPos--;
    }

    for( sCurrent = sStart, sIdx = 0; sCurrent < sDigitEndPos; sCurrent++ )
    {
        /**
         * '.' 와 leading zero 는 저장하지 않음
         */
        if( *sCurrent == '.' )
        {
            sTruncatedCount--;
            continue;
        }
        else if( (*sCurrent == '0' && sIsValid == STL_FALSE) )
        {
            continue;
        }

        STL_TRY_THROW( stlIsdigit( *sCurrent ) == STL_TRUE,
                       RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION );
                    
        sDigitString[sIdx] = *sCurrent;
        sIdx++;

        /**
         * 출력 순서 : sStart -> sEndPod
         * 중간의 zero 를 출력하기 위한 변수 설정
         */
        sIsValid = STL_TRUE;
    }

    
    if( sTrailingZero > 0 )
    {
        /**
         * trainig zero padding
         * ex)  1E+165
         *   -> 10000 ... E+127
         */
        while( sTrailingZero > 0 )
        {
            sDigitString[ sIdx ] = '0';
            sIdx++;
            sTrailingZero--;
        }
    }
    else
    {
        /**
         * trunc 에 의해 0 이 남은 경우 삭제
         */
        sIdx2 = sIdx;
        while( sIdx2 > 0 )
        {
            if( sDigitString[ sIdx2 -1 ] - '0' == 0 )
            {
                sIdx--;
                sScale--;
            }
            else
            {
                break;
            }

            sIdx2--;
        }
    }


    STL_RAMP( RAMP_SKIP_SET_VALUE );

    sDigitStringLength = sIdx;
        
    sDigitString[sDigitStringLength] = 0;

    sNumericVal = aCNumeric->val;
    stlMemset( sNumericVal, 0x00, SQL_MAX_NUMERIC_LEN );

    for( sIdx = 0; sIdx < sDigitStringLength; sIdx++ )
    {
        sDigitString[sIdx] -= '0';
    }

    for( sIdx = 0; sIdx < sDigitStringLength; )
    {
        for( sIdx2 = sIdx, sVar = 0; sIdx2 < sDigitStringLength; sIdx2++ )
        {
            sVar *= 10;
            sVar += sDigitString[sIdx2];
            sDigitString[sIdx2] = sVar / 256;
            sVar %= 256;
        }

        STL_TRY_THROW( sVarLength < SQL_MAX_NUMERIC_LEN ,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        *sNumericVal = sVar;
        sNumericVal++;
        sVarLength++;

        while( sIdx != sDigitStringLength )
        {
            if( sDigitString[ sIdx ] == 0 )
            {
                sIdx++;
            }
            else
            {
                break;
            }
        }
    }

    aCNumeric->scale = sScale;

    if( sIsTruncated == STL_TRUE )
    {
        *aIsTruncated = STL_TRUE;
        
        STL_THROW( RAMP_WARNING_FRACTIONAL_TRUNCATION );
    }
    else
    {
        *aIsTruncated = STL_FALSE;
    }

    sRet = STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );

        sRet = STL_FAILURE;
    }
    
    STL_CATCH( RAMP_ERR_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CHARACTER_VALUE_FOR_CAST_SPECIFICATION,
                      "The C type was an exact or approximate numeric, a datetime, or an interval data type; "
                      "the SQL type of the column was a character data type; "
                      "and the value in the column was not a valid literal of the bound C type.",
                      aErrorStack );

        sRet = STL_FAILURE;
    }

    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet = STL_SUCCESS;
    }

    STL_FINISH;
    
    return sRet;
}

stlStatus zlvMakeCNumericFromInteger( zlsStmt            * aStmt,
                                      SQL_NUMERIC_STRUCT * aCNumeric,
                                      stlUInt64            aUnsignedInteger,
                                      stlInt16             aScale,
                                      stlInt16             aPrecision,
                                      stlBool              aIsNegative,
                                      stlErrorStack      * aErrorStack )
{
    stlUInt128   sUInt128Val;
    SQLCHAR    * sVal;
    stlInt32     sScale = 0;
    stlInt64     sPrecision = 0;
    stlUInt64    sTmpValue = 0;
    

    while( STL_TRUE )
    {
        sTmpValue = aUnsignedInteger % 10;
        if( sTmpValue > 0 )
        {
            break;
        }

        aUnsignedInteger = aUnsignedInteger / 10;
        if( aUnsignedInteger == 0 )
        {
            break;
        }

        sScale--;
    }

    sUInt128Val.mHighDigit = 0;
    sUInt128Val.mLowDigit  = aUnsignedInteger;

    stlGetPrecisionUInt128( sUInt128Val, sPrecision );

    if( aScale < 0 )
    {
        STL_TRY_THROW( sScale <= aScale,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
    }
    else
    {
        STL_TRY_THROW( sPrecision + (-sScale) + aScale < ZLV_C_NUMERIC_VAL_MAX_DIGIT,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE);
    }

    aCNumeric->precision = aPrecision;
    aCNumeric->scale     = sScale;

    if( aIsNegative == STL_FALSE )
    {
        aCNumeric->sign = 1;
    }
    else
    {
        aCNumeric->sign = 0;
    }

#ifdef STL_IS_BIGENDIAN
    sVal = (SQLCHAR*)&sUInt128Val.mLowDigit;
    aCNumeric->val[0]  = sVal[7];
    aCNumeric->val[1]  = sVal[6];
    aCNumeric->val[2]  = sVal[5];
    aCNumeric->val[3]  = sVal[4];
    aCNumeric->val[4]  = sVal[3];
    aCNumeric->val[5]  = sVal[2];
    aCNumeric->val[6]  = sVal[1];
    aCNumeric->val[7]  = sVal[0];

    sVal = (SQLCHAR*)&sUInt128Val.mHighDigit;
    aCNumeric->val[8]  = sVal[7];
    aCNumeric->val[9]  = sVal[6];
    aCNumeric->val[10] = sVal[5];
    aCNumeric->val[11] = sVal[4];
    aCNumeric->val[12] = sVal[3];
    aCNumeric->val[13] = sVal[2];
    aCNumeric->val[14] = sVal[1];
    aCNumeric->val[15] = sVal[0];
#else
    sVal = (SQLCHAR*)&sUInt128Val.mLowDigit;
    aCNumeric->val[0]  = sVal[0];
    aCNumeric->val[1]  = sVal[1];
    aCNumeric->val[2]  = sVal[2];
    aCNumeric->val[3]  = sVal[3];
    aCNumeric->val[4]  = sVal[4];
    aCNumeric->val[5]  = sVal[5];
    aCNumeric->val[6]  = sVal[6];
    aCNumeric->val[7]  = sVal[7];

    sVal = (SQLCHAR*)&sUInt128Val.mHighDigit;
    aCNumeric->val[8]  = sVal[0];
    aCNumeric->val[9]  = sVal[1];
    aCNumeric->val[10] = sVal[2];
    aCNumeric->val[11] = sVal[3];
    aCNumeric->val[12] = sVal[4];
    aCNumeric->val[13] = sVal[5];
    aCNumeric->val[14] = sVal[6];
    aCNumeric->val[15] = sVal[7];
#endif

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}
                                      
                                    
stlStatus zlvMakeCNumericFromSecond( zlsStmt            * aStmt,
                                     SQL_NUMERIC_STRUCT * aCNumeric,
                                     stlUInt64            aSecond,
                                     stlUInt32            aFraction,
                                     stlInt16             aScale,
                                     stlBool              aIsNegative,
                                     stlBool            * aIsTruncated,
                                     stlErrorStack      * aErrorStack )
{
    stlUInt128   sUInt128Val;
    SQLCHAR    * sVal;
    stlUInt128   sSecondDigit;
    stlUInt128   sFractionDigit;
    stlUInt64    sTmpValue;
    stlUInt64    sTmpSecond;
    stlUInt64    sSecondDigitCount;
    stlUInt32    sTmpFraction;
    stlFloat64   sPow;
    stlInt16     sScale;
    stlInt32     sZeroLen = 0;
    stlStatus    sRet;

    /**
     * 실제 scale 보다 작은 aScale 값인 경우
     * truncated 로 간주
     */
    sUInt128Val.mHighDigit = 0;
    sUInt128Val.mLowDigit  = (stlUInt64)aSecond;

    stlGetPrecisionUInt128( sUInt128Val, sSecondDigitCount );

   
    if( aFraction != 0 )
    {
        sTmpFraction = aFraction;
        while( STL_TRUE )
        {
            if( sTmpFraction == 0 )
            {
                break;
            }
            
            sTmpValue = sTmpFraction % 10;
            if( sTmpValue > 0 )
            {
                break;
            }

            sTmpFraction = sTmpFraction / 10;

            sZeroLen++;
        }

        if( (DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION-sZeroLen) > aScale )
        {
            *aIsTruncated = STL_TRUE;
        }
    }

    if( aScale > 0 )
    {        
        STL_TRY_THROW( (sSecondDigitCount + aScale) < ZLV_C_NUMERIC_VAL_MAX_DIGIT,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        
        sScale = ((aScale > DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ?
                  DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION : aScale);
        
        STL_TRY_THROW( stlGet10Pow( (DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION - sScale),
                                    &sPow,
                                    aErrorStack )
                       == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        
        stlSetToUInt128FromUInt64( aFraction / sPow,
                                   &sFractionDigit );

        STL_TRY_THROW( stlGet10Pow( sScale, &sPow, aErrorStack ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        STL_TRY_THROW( stlMulUInt64( aSecond,
                                     sPow,
                                     &sSecondDigit ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        STL_TRY_THROW( stlAddUInt128( sSecondDigit,
                                      sFractionDigit,
                                      &sUInt128Val ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
    }
    else
    {
        sScale     = aScale;
        sTmpSecond = aSecond;
        sZeroLen   = 0;
        
        while( STL_TRUE )
        {
            if( sTmpSecond == 0 )
            {
                break;
            }
            
            sTmpValue = sTmpSecond % 10;
            if( sTmpValue > 0 )
            {
                break;
            }

            sTmpSecond = sTmpSecond / 10;

            sZeroLen++;
        }

        /**
         * second 값이 trunc 되는 경우
         * ex ) second : 12 , scale = -1  ( ERROR )
         *      second : 10 , scale = -1  ( O )
         */
        if( (sZeroLen + sScale) < 0 )
        {
            STL_THROW( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );
        }
        
        STL_TRY_THROW( stlGet10Pow( -aScale, &sPow, aErrorStack ) == STL_TRUE,
                       RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE );

        sUInt128Val.mLowDigit = aSecond / sPow;
    }

    /**
     * scale 연산후 남은 trailing zero 제거
     */
    while( STL_TRUE )
    {
        if( sUInt128Val.mLowDigit == 0 )
        {
            break;
        }
        
        sTmpValue = sUInt128Val.mLowDigit % 10;
        if( sTmpValue > 0 )
        {
            break;
        }

        sUInt128Val.mLowDigit = sUInt128Val.mLowDigit / 10;

        sScale--;
    }

    if( aIsNegative == STL_FALSE )
    {
        aCNumeric->sign = 1;
    }
    else
    {
        aCNumeric->sign = 0;
    }

    aCNumeric->precision = ZLV_INTERVAL_SECOND_TO_C_NUMERIC_PRECISION;
    aCNumeric->scale     = sScale;

#ifdef STL_IS_BIGENDIAN
    sVal = (SQLCHAR*)&sUInt128Val.mLowDigit;
    aCNumeric->val[0]  = sVal[7];
    aCNumeric->val[1]  = sVal[6];
    aCNumeric->val[2]  = sVal[5];
    aCNumeric->val[3]  = sVal[4];
    aCNumeric->val[4]  = sVal[3];
    aCNumeric->val[5]  = sVal[2];
    aCNumeric->val[6]  = sVal[1];
    aCNumeric->val[7]  = sVal[0];

    sVal = (SQLCHAR*)&sUInt128Val.mHighDigit;
    aCNumeric->val[8]  = sVal[7];
    aCNumeric->val[9]  = sVal[6];
    aCNumeric->val[10] = sVal[5];
    aCNumeric->val[11] = sVal[4];
    aCNumeric->val[12] = sVal[3];
    aCNumeric->val[13] = sVal[2];
    aCNumeric->val[14] = sVal[1];
    aCNumeric->val[15] = sVal[0];
#else
    sVal = (SQLCHAR*)&sUInt128Val.mLowDigit;
    aCNumeric->val[0]  = sVal[0];
    aCNumeric->val[1]  = sVal[1];
    aCNumeric->val[2]  = sVal[2];
    aCNumeric->val[3]  = sVal[3];
    aCNumeric->val[4]  = sVal[4];
    aCNumeric->val[5]  = sVal[5];
    aCNumeric->val[6]  = sVal[6];
    aCNumeric->val[7]  = sVal[7];

    sVal = (SQLCHAR*)&sUInt128Val.mHighDigit;
    aCNumeric->val[8]  = sVal[0];
    aCNumeric->val[9]  = sVal[1];
    aCNumeric->val[10] = sVal[2];
    aCNumeric->val[11] = sVal[3];
    aCNumeric->val[12] = sVal[4];
    aCNumeric->val[13] = sVal[5];
    aCNumeric->val[14] = sVal[6];
    aCNumeric->val[15] = sVal[7];
#endif

    sRet = STL_SUCCESS;

    STL_TRY_THROW( *aIsTruncated == STL_FALSE,
                   RAMP_WARNING_FRACTIONAL_TRUNCATION );

    STL_CATCH( RAMP_WARNING_FRACTIONAL_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_FRACTIONAL_TRUNCATION,
                      "The data returned for a column was truncated. "
                      "For numeric data types, the fractional part of the number was truncated. "
                      "For time, timestamp, and interval data types that contain a time component, "
                      "the fractional part of the time was truncated.",
                      aErrorStack );

        sRet = STL_SUCCESS;
    }

    STL_CATCH( RAMP_ERR_NUMERIC_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
                      "Returning the numeric value as numeric or string for one or "
                      "more bound columns would have caused the whole "
                      "(as opposed to fractional) part of the number to be truncated.",
                      aErrorStack );

        sRet = STL_FAILURE;
    }

    STL_FINISH;

    return sRet;
}

stlStatus zlvGetSqlToCFunc( zlsStmt       * aStmt,
                            dtlDataValue  * aDataValue,
                            stlInt16        aCType,
                            zlvSqlToCFunc * aFuncPtr,
                            stlErrorStack * aErrorStack )
{
    dtlIntervalYearToMonthType * sIntervalYearToMonthType;
    dtlIntervalDayToSecondType * sIntervalDayToSecondType;

    switch( aDataValue->mType )
    {
        case DTL_TYPE_CHAR :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlCharToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlCharToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlCharToCLongVarChar;
                    break;
                case SQL_C_STINYINT :
                case SQL_C_TINYINT :
                    *aFuncPtr = zlvSqlCharToCSTinyInt;
                    break;
                case SQL_C_UTINYINT :
                    *aFuncPtr = zlvSqlCharToCUTinyInt;
                    break;
                case SQL_C_SBIGINT :
                    *aFuncPtr = zlvSqlCharToCSBigInt;
                    break;
                case SQL_C_UBIGINT :
                    *aFuncPtr = zlvSqlCharToCUBigInt;
                    break;
                case SQL_C_SSHORT :
                case SQL_C_SHORT :
                    *aFuncPtr = zlvSqlCharToCSShort;
                    break;
                case SQL_C_USHORT :
                    *aFuncPtr = zlvSqlCharToCUShort;
                    break;
                case SQL_C_SLONG :
                case SQL_C_LONG :
                    *aFuncPtr = zlvSqlCharToCSLong;
                    break;
                case SQL_C_ULONG :
                    *aFuncPtr = zlvSqlCharToCULong;
                    break;
                case SQL_C_NUMERIC :
                    *aFuncPtr = zlvSqlCharToCNumeric;
                    break;
                case SQL_C_FLOAT :
                    *aFuncPtr = zlvSqlCharToCFloat;
                    break;
                case SQL_C_DOUBLE :
                    *aFuncPtr = zlvSqlCharToCDouble;
                    break;
                case SQL_C_BIT :
                    *aFuncPtr = zlvSqlCharToCBit;
                    break;
                case SQL_C_BOOLEAN :
                    *aFuncPtr = zlvSqlCharToCBoolean;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlCharToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlCharToCLongVarBinary;
                    break;
                case SQL_C_TYPE_DATE :
                    *aFuncPtr = zlvSqlCharToCDate;
                    break;
                case SQL_C_TYPE_TIME :
                    *aFuncPtr = zlvSqlCharToCTime;
                    break;
                case SQL_C_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlCharToCTimeTz;
                    break;
                case SQL_C_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvSqlCharToCTimestamp;
                    break;
                case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlCharToCTimestampTz;
                    break;
                case SQL_C_INTERVAL_MONTH :
                    *aFuncPtr = zlvSqlCharToCIntervalMonth;
                    break;
                case SQL_C_INTERVAL_YEAR :
                    *aFuncPtr = zlvSqlCharToCIntervalYear;
                    break;
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvSqlCharToCIntervalYearToMonth;
                    break;
                case SQL_C_INTERVAL_DAY :
                    *aFuncPtr = zlvSqlCharToCIntervalDay;
                    break;
                case SQL_C_INTERVAL_HOUR :
                    *aFuncPtr = zlvSqlCharToCIntervalHour;
                    break;
                case SQL_C_INTERVAL_MINUTE :
                    *aFuncPtr = zlvSqlCharToCIntervalMinute;
                    break;
                case SQL_C_INTERVAL_SECOND :
                    *aFuncPtr = zlvSqlCharToCIntervalSecond;
                    break;
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvSqlCharToCIntervalDayToHour;
                    break;
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvSqlCharToCIntervalDayToMinute;
                    break;
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvSqlCharToCIntervalDayToSecond;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvSqlCharToCIntervalHourToMinute;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvSqlCharToCIntervalHourToSecond;
                    break;
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvSqlCharToCIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_VARCHAR :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlVarCharToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlVarCharToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlVarCharToCLongVarChar;
                    break;
                case SQL_C_STINYINT :
                case SQL_C_TINYINT :
                    *aFuncPtr = zlvSqlVarCharToCSTinyInt;
                    break;
                case SQL_C_UTINYINT :
                    *aFuncPtr = zlvSqlVarCharToCUTinyInt;
                    break;
                case SQL_C_SBIGINT :
                    *aFuncPtr = zlvSqlVarCharToCSBigInt;
                    break;
                case SQL_C_UBIGINT :
                    *aFuncPtr = zlvSqlVarCharToCUBigInt;
                    break;
                case SQL_C_SSHORT :
                case SQL_C_SHORT :
                    *aFuncPtr = zlvSqlVarCharToCSShort;
                    break;
                case SQL_C_USHORT :
                    *aFuncPtr = zlvSqlVarCharToCUShort;
                    break;
                case SQL_C_SLONG :
                case SQL_C_LONG :
                    *aFuncPtr = zlvSqlVarCharToCSLong;
                    break;
                case SQL_C_ULONG :
                    *aFuncPtr = zlvSqlVarCharToCULong;
                    break;
                case SQL_C_NUMERIC :
                    *aFuncPtr = zlvSqlVarCharToCNumeric;
                    break;
                case SQL_C_FLOAT :
                    *aFuncPtr = zlvSqlVarCharToCFloat;
                    break;
                case SQL_C_DOUBLE :
                    *aFuncPtr = zlvSqlVarCharToCDouble;
                    break;
                case SQL_C_BIT :
                    *aFuncPtr = zlvSqlVarCharToCBit;
                    break;
                case SQL_C_BOOLEAN :
                    *aFuncPtr = zlvSqlVarCharToCBoolean;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlVarCharToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlVarCharToCLongVarBinary;
                    break;
                case SQL_C_TYPE_DATE :
                    *aFuncPtr = zlvSqlVarCharToCDate;
                    break;
                case SQL_C_TYPE_TIME :
                    *aFuncPtr = zlvSqlVarCharToCTime;
                    break;
                case SQL_C_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlVarCharToCTimeTz;
                    break;
                case SQL_C_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvSqlVarCharToCTimestamp;
                    break;
                case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlVarCharToCTimestampTz;
                    break;
                case SQL_C_INTERVAL_MONTH :
                    *aFuncPtr = zlvSqlVarCharToCIntervalMonth;
                    break;
                case SQL_C_INTERVAL_YEAR :
                    *aFuncPtr = zlvSqlVarCharToCIntervalYear;
                    break;
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvSqlVarCharToCIntervalYearToMonth;
                    break;
                case SQL_C_INTERVAL_DAY :
                    *aFuncPtr = zlvSqlVarCharToCIntervalDay;
                    break;
                case SQL_C_INTERVAL_HOUR :
                    *aFuncPtr = zlvSqlVarCharToCIntervalHour;
                    break;
                case SQL_C_INTERVAL_MINUTE :
                    *aFuncPtr = zlvSqlVarCharToCIntervalMinute;
                    break;
                case SQL_C_INTERVAL_SECOND :
                    *aFuncPtr = zlvSqlVarCharToCIntervalSecond;
                    break;
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvSqlVarCharToCIntervalDayToHour;
                    break;
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvSqlVarCharToCIntervalDayToMinute;
                    break;
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvSqlVarCharToCIntervalDayToSecond;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvSqlVarCharToCIntervalHourToMinute;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvSqlVarCharToCIntervalHourToSecond;
                    break;
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvSqlVarCharToCIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_LONGVARCHAR :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlLongVarCharToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlLongVarCharToCWchar;
                    break;
                case SQL_C_LONGVARCHAR:
                    *aFuncPtr = zlvSqlLongVarCharToCLongVarChar;
                    break;
                case SQL_C_STINYINT :
                case SQL_C_TINYINT :
                    *aFuncPtr = zlvSqlLongVarCharToCSTinyInt;
                    break;
                case SQL_C_UTINYINT :
                    *aFuncPtr = zlvSqlLongVarCharToCUTinyInt;
                    break;
                case SQL_C_SBIGINT :
                    *aFuncPtr = zlvSqlLongVarCharToCSBigInt;
                    break;
                case SQL_C_UBIGINT :
                    *aFuncPtr = zlvSqlLongVarCharToCUBigInt;
                    break;
                case SQL_C_SSHORT :
                case SQL_C_SHORT :
                    *aFuncPtr = zlvSqlLongVarCharToCSShort;
                    break;
                case SQL_C_USHORT :
                    *aFuncPtr = zlvSqlLongVarCharToCUShort;
                    break;
                case SQL_C_SLONG :
                case SQL_C_LONG :
                    *aFuncPtr = zlvSqlLongVarCharToCSLong;
                    break;
                case SQL_C_ULONG :
                    *aFuncPtr = zlvSqlLongVarCharToCULong;
                    break;
                case SQL_C_NUMERIC :
                    *aFuncPtr = zlvSqlLongVarCharToCNumeric;
                    break;
                case SQL_C_FLOAT :
                    *aFuncPtr = zlvSqlLongVarCharToCFloat;
                    break;
                case SQL_C_DOUBLE :
                    *aFuncPtr = zlvSqlLongVarCharToCDouble;
                    break;
                case SQL_C_BIT :
                    *aFuncPtr = zlvSqlLongVarCharToCBit;
                    break;
                case SQL_C_BOOLEAN :
                    *aFuncPtr = zlvSqlLongVarCharToCBoolean;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlLongVarCharToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlLongVarCharToCLongVarBinary;
                    break;
                case SQL_C_TYPE_DATE :
                    *aFuncPtr = zlvSqlLongVarCharToCDate;
                    break;
                case SQL_C_TYPE_TIME :
                    *aFuncPtr = zlvSqlLongVarCharToCTime;
                    break;
                case SQL_C_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlLongVarCharToCTimeTz;
                    break;
                case SQL_C_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvSqlLongVarCharToCTimestamp;
                    break;
                case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlLongVarCharToCTimestampTz;
                    break;
                case SQL_C_INTERVAL_MONTH :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalMonth;
                    break;
                case SQL_C_INTERVAL_YEAR :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalYear;
                    break;
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalYearToMonth;
                    break;
                case SQL_C_INTERVAL_DAY :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalDay;
                    break;
                case SQL_C_INTERVAL_HOUR :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalHour;
                    break;
                case SQL_C_INTERVAL_MINUTE :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalMinute;
                    break;
                case SQL_C_INTERVAL_SECOND :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalSecond;
                    break;
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalDayToHour;
                    break;
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalDayToMinute;
                    break;
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalDayToSecond;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalHourToMinute;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalHourToSecond;
                    break;
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvSqlLongVarCharToCIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_NATIVE_BIGINT :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlBigIntToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlBigIntToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlBigIntToCLongVarChar;
                    break;
                case SQL_C_STINYINT :
                case SQL_C_TINYINT :
                    *aFuncPtr = zlvSqlBigIntToCSTinyInt;
                    break;
                case SQL_C_UTINYINT :
                    *aFuncPtr = zlvSqlBigIntToCUTinyInt;
                    break;
                case SQL_C_SBIGINT :
                    *aFuncPtr = zlvSqlBigIntToCSBigInt;
                    break;
                case SQL_C_UBIGINT :
                    *aFuncPtr = zlvSqlBigIntToCUBigInt;
                    break;
                case SQL_C_SSHORT :
                case SQL_C_SHORT :
                    *aFuncPtr = zlvSqlBigIntToCSShort;
                    break;
                case SQL_C_USHORT :
                    *aFuncPtr = zlvSqlBigIntToCUShort;
                    break;
                case SQL_C_SLONG :
                case SQL_C_LONG :
                    *aFuncPtr = zlvSqlBigIntToCSLong;
                    break;
                case SQL_C_ULONG :
                    *aFuncPtr = zlvSqlBigIntToCULong;
                    break;
                case SQL_C_NUMERIC :
                    *aFuncPtr = zlvSqlBigIntToCNumeric;
                    break;
                case SQL_C_FLOAT :
                    *aFuncPtr = zlvSqlBigIntToCFloat;
                    break;
                case SQL_C_DOUBLE :
                    *aFuncPtr = zlvSqlBigIntToCDouble;
                    break;
                case SQL_C_BIT :
                    *aFuncPtr = zlvSqlBigIntToCBit;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlBigIntToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlBigIntToCLongVarBinary;
                    break;
                case SQL_C_INTERVAL_MONTH :
                    *aFuncPtr = zlvSqlBigIntToCIntervalMonth;
                    break;
                case SQL_C_INTERVAL_YEAR :
                    *aFuncPtr = zlvSqlBigIntToCIntervalYear;
                    break;
                case SQL_C_INTERVAL_DAY :
                    *aFuncPtr = zlvSqlBigIntToCIntervalDay;
                    break;
                case SQL_C_INTERVAL_HOUR :
                    *aFuncPtr = zlvSqlBigIntToCIntervalHour;
                    break;
                case SQL_C_INTERVAL_MINUTE :
                    *aFuncPtr = zlvSqlBigIntToCIntervalMinute;
                    break;
                case SQL_C_INTERVAL_SECOND :
                    *aFuncPtr = zlvSqlBigIntToCIntervalSecond;
                    break;
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvSqlBigIntToCIntervalYearToMonth;
                    break;
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvSqlBigIntToCIntervalDayToHour;
                    break;
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvSqlBigIntToCIntervalDayToMinute;
                    break;
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvSqlBigIntToCIntervalDayToSecond;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvSqlBigIntToCIntervalHourToMinute;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvSqlBigIntToCIntervalHourToSecond;
                    break;
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvSqlBigIntToCIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_FLOAT :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlNumericToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlNumericToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlNumericToCLongVarChar;
                    break;
                case SQL_C_STINYINT :
                case SQL_C_TINYINT :                    
                    *aFuncPtr = zlvSqlNumericToCSTinyInt;
                    break;
                case SQL_C_UTINYINT :
                    *aFuncPtr = zlvSqlNumericToCUTinyInt;
                    break;
                case SQL_C_SBIGINT :
                    *aFuncPtr = zlvSqlNumericToCSBigInt;
                    break;
                case SQL_C_UBIGINT :
                    *aFuncPtr = zlvSqlNumericToCUBigInt;
                    break;
                case SQL_C_SSHORT :
                case SQL_C_SHORT :
                    *aFuncPtr = zlvSqlNumericToCSShort;
                    break;
                case SQL_C_USHORT :
                    *aFuncPtr = zlvSqlNumericToCUShort;
                    break;
                case SQL_C_SLONG :
                case SQL_C_LONG :
                    *aFuncPtr = zlvSqlNumericToCSLong;
                    break;
                case SQL_C_ULONG :
                    *aFuncPtr = zlvSqlNumericToCULong;
                    break;
                case SQL_C_NUMERIC :
                    *aFuncPtr = zlvSqlNumericToCNumeric;
                    break;
                case SQL_C_FLOAT :
                    *aFuncPtr = zlvSqlNumericToCFloat;
                    break;
                case SQL_C_DOUBLE :
                    *aFuncPtr = zlvSqlNumericToCDouble;
                    break;
                case SQL_C_BIT :
                    *aFuncPtr = zlvSqlNumericToCBit;
                    break; 
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlNumericToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlNumericToCLongVarBinary;
                    break;
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvSqlNumericToCIntervalYearToMonth;
                    break;
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvSqlNumericToCIntervalDayToHour;
                    break;
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvSqlNumericToCIntervalDayToMinute;
                    break;
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvSqlNumericToCIntervalDayToSecond;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvSqlNumericToCIntervalHourToMinute;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvSqlNumericToCIntervalHourToSecond;
                    break;
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvSqlNumericToCIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_NUMBER :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlNumericToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlNumericToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlNumericToCLongVarChar;
                    break;
                case SQL_C_STINYINT :
                case SQL_C_TINYINT :                    
                    *aFuncPtr = zlvSqlNumericToCSTinyInt;
                    break;
                case SQL_C_UTINYINT :
                    *aFuncPtr = zlvSqlNumericToCUTinyInt;
                    break;
                case SQL_C_SBIGINT :
                    *aFuncPtr = zlvSqlNumericToCSBigInt;
                    break;
                case SQL_C_UBIGINT :
                    *aFuncPtr = zlvSqlNumericToCUBigInt;
                    break;
                case SQL_C_SSHORT :
                case SQL_C_SHORT :
                    *aFuncPtr = zlvSqlNumericToCSShort;
                    break;
                case SQL_C_USHORT :
                    *aFuncPtr = zlvSqlNumericToCUShort;
                    break;
                case SQL_C_SLONG :
                case SQL_C_LONG :
                    *aFuncPtr = zlvSqlNumericToCSLong;
                    break;
                case SQL_C_ULONG :
                    *aFuncPtr = zlvSqlNumericToCULong;
                    break;
                case SQL_C_NUMERIC :
                    *aFuncPtr = zlvSqlNumericToCNumeric;
                    break;
                case SQL_C_FLOAT :
                    *aFuncPtr = zlvSqlNumericToCFloat;
                    break;
                case SQL_C_DOUBLE :
                    *aFuncPtr = zlvSqlNumericToCDouble;
                    break;
                case SQL_C_BIT :
                    *aFuncPtr = zlvSqlNumericToCBit;
                    break; 
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlNumericToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlNumericToCLongVarBinary;
                    break;
                case SQL_C_INTERVAL_MONTH :
                    *aFuncPtr = zlvSqlNumericToCIntervalMonth;
                    break;
                case SQL_C_INTERVAL_YEAR :
                    *aFuncPtr = zlvSqlNumericToCIntervalYear;
                    break;
                case SQL_C_INTERVAL_DAY :
                    *aFuncPtr = zlvSqlNumericToCIntervalDay;
                    break;
                case SQL_C_INTERVAL_HOUR :
                    *aFuncPtr = zlvSqlNumericToCIntervalHour;
                    break;
                case SQL_C_INTERVAL_MINUTE :
                    *aFuncPtr = zlvSqlNumericToCIntervalMinute;
                    break;
                case SQL_C_INTERVAL_SECOND :
                    *aFuncPtr = zlvSqlNumericToCIntervalSecond;
                    break;
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvSqlNumericToCIntervalYearToMonth;
                    break;
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvSqlNumericToCIntervalDayToHour;
                    break;
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvSqlNumericToCIntervalDayToMinute;
                    break;
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvSqlNumericToCIntervalDayToSecond;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvSqlNumericToCIntervalHourToMinute;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvSqlNumericToCIntervalHourToSecond;
                    break;
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvSqlNumericToCIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_NATIVE_SMALLINT :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlSmallIntToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlSmallIntToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlSmallIntToCLongVarChar;
                    break;
                case SQL_C_STINYINT :
                case SQL_C_TINYINT :
                    *aFuncPtr = zlvSqlSmallIntToCSTinyInt;
                    break;
                case SQL_C_UTINYINT :
                    *aFuncPtr = zlvSqlSmallIntToCUTinyInt;
                    break;
                case SQL_C_SBIGINT :
                    *aFuncPtr = zlvSqlSmallIntToCSBigInt;
                    break;
                case SQL_C_UBIGINT :
                    *aFuncPtr = zlvSqlSmallIntToCUBigInt;
                    break;
                case SQL_C_SSHORT :
                case SQL_C_SHORT :
                    *aFuncPtr = zlvSqlSmallIntToCSShort;
                    break;
                case SQL_C_USHORT :
                    *aFuncPtr = zlvSqlSmallIntToCUShort;
                    break;
                case SQL_C_SLONG :
                case SQL_C_LONG :
                    *aFuncPtr = zlvSqlSmallIntToCSLong;
                    break;
                case SQL_C_ULONG :
                    *aFuncPtr = zlvSqlSmallIntToCULong;
                    break;
                case SQL_C_NUMERIC :
                    *aFuncPtr = zlvSqlSmallIntToCNumeric;
                    break;
                case SQL_C_FLOAT :
                    *aFuncPtr = zlvSqlSmallIntToCFloat;
                    break;
                case SQL_C_DOUBLE :
                    *aFuncPtr = zlvSqlSmallIntToCDouble;
                    break;
                case SQL_C_BIT :
                    *aFuncPtr = zlvSqlSmallIntToCBit;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlSmallIntToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlSmallIntToCLongVarBinary;
                    break;
                case SQL_C_INTERVAL_MONTH :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalMonth;
                    break;
                case SQL_C_INTERVAL_YEAR :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalYear;
                    break;
                case SQL_C_INTERVAL_DAY :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalDay;
                    break;
                case SQL_C_INTERVAL_HOUR :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalHour;
                    break;
                case SQL_C_INTERVAL_MINUTE :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalMinute;
                    break;
                case SQL_C_INTERVAL_SECOND :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalSecond;
                    break;
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalYearToMonth;
                    break;
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalDayToHour;
                    break;
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalDayToMinute;
                    break;
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalDayToSecond;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalHourToMinute;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalHourToSecond;
                    break;
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvSqlSmallIntToCIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_NATIVE_INTEGER :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlIntegerToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlIntegerToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlIntegerToCLongVarChar;
                    break;
                case SQL_C_STINYINT :
                case SQL_C_TINYINT :
                    *aFuncPtr = zlvSqlIntegerToCSTinyInt;
                    break;
                case SQL_C_UTINYINT :
                    *aFuncPtr = zlvSqlIntegerToCUTinyInt;
                    break;
                case SQL_C_SBIGINT :
                    *aFuncPtr = zlvSqlIntegerToCSBigInt;
                    break;
                case SQL_C_UBIGINT :
                    *aFuncPtr = zlvSqlIntegerToCUBigInt;
                    break;
                case SQL_C_SSHORT :
                case SQL_C_SHORT :
                    *aFuncPtr = zlvSqlIntegerToCSShort;
                    break;
                case SQL_C_USHORT :
                    *aFuncPtr = zlvSqlIntegerToCUShort;
                    break;
                case SQL_C_SLONG :
                case SQL_C_LONG :
                    *aFuncPtr = zlvSqlIntegerToCSLong;
                    break;
                case SQL_C_ULONG :
                    *aFuncPtr = zlvSqlIntegerToCULong;
                    break;
                case SQL_C_NUMERIC :
                    *aFuncPtr = zlvSqlIntegerToCNumeric;
                    break;
                case SQL_C_FLOAT :
                    *aFuncPtr = zlvSqlIntegerToCFloat;
                    break;
                case SQL_C_DOUBLE :
                    *aFuncPtr = zlvSqlIntegerToCDouble;
                    break;
                case SQL_C_BIT :
                    *aFuncPtr = zlvSqlIntegerToCBit;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlIntegerToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlIntegerToCLongVarBinary;
                    break;
                case SQL_C_INTERVAL_MONTH :
                    *aFuncPtr = zlvSqlIntegerToCIntervalMonth;
                    break;
                case SQL_C_INTERVAL_YEAR :
                    *aFuncPtr = zlvSqlIntegerToCIntervalYear;
                    break;
                case SQL_C_INTERVAL_DAY :
                    *aFuncPtr = zlvSqlIntegerToCIntervalDay;
                    break;
                case SQL_C_INTERVAL_HOUR :
                    *aFuncPtr = zlvSqlIntegerToCIntervalHour;
                    break;
                case SQL_C_INTERVAL_MINUTE :
                    *aFuncPtr = zlvSqlIntegerToCIntervalMinute;
                    break;
                case SQL_C_INTERVAL_SECOND :
                    *aFuncPtr = zlvSqlIntegerToCIntervalSecond;
                    break;
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvSqlIntegerToCIntervalYearToMonth;
                    break;
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvSqlIntegerToCIntervalDayToHour;
                    break;
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvSqlIntegerToCIntervalDayToMinute;
                    break;
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvSqlIntegerToCIntervalDayToSecond;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvSqlIntegerToCIntervalHourToMinute;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvSqlIntegerToCIntervalHourToSecond;
                    break;
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvSqlIntegerToCIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_NATIVE_REAL :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlRealToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlRealToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlRealToCLongVarChar;
                    break;
                case SQL_C_STINYINT :
                case SQL_C_TINYINT :
                    *aFuncPtr = zlvSqlRealToCSTinyInt;
                    break;
                case SQL_C_UTINYINT :
                    *aFuncPtr = zlvSqlRealToCUTinyInt;
                    break;
                case SQL_C_SBIGINT :
                    *aFuncPtr = zlvSqlRealToCSBigInt;
                    break;
                case SQL_C_UBIGINT :
                    *aFuncPtr = zlvSqlRealToCUBigInt;
                    break;
                case SQL_C_SSHORT :
                case SQL_C_SHORT :
                    *aFuncPtr = zlvSqlRealToCSShort;
                    break;
                case SQL_C_USHORT :
                    *aFuncPtr = zlvSqlRealToCUShort;
                    break;
                case SQL_C_SLONG :
                case SQL_C_LONG :
                    *aFuncPtr = zlvSqlRealToCSLong;
                    break;
                case SQL_C_ULONG :
                    *aFuncPtr = zlvSqlRealToCULong;
                    break;
                case SQL_C_NUMERIC :
                    *aFuncPtr = zlvSqlRealToCNumeric;
                    break;
                case SQL_C_FLOAT :
                    *aFuncPtr = zlvSqlRealToCFloat;
                    break;
                case SQL_C_DOUBLE :
                    *aFuncPtr = zlvSqlRealToCDouble;
                    break;
                case SQL_C_BIT :
                    *aFuncPtr = zlvSqlRealToCBit;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlRealToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlRealToCLongVarBinary;
                    break;
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvSqlRealToCIntervalYearToMonth;
                    break;
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvSqlRealToCIntervalDayToHour;
                    break;
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvSqlRealToCIntervalDayToMinute;
                    break;
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvSqlRealToCIntervalDayToSecond;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvSqlRealToCIntervalHourToMinute;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvSqlRealToCIntervalHourToSecond;
                    break;
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvSqlRealToCIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_NATIVE_DOUBLE :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlDoubleToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlDoubleToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlDoubleToCLongVarChar;
                    break;
                case SQL_C_STINYINT :
                case SQL_C_TINYINT :
                    *aFuncPtr = zlvSqlDoubleToCSTinyInt;
                    break;
                case SQL_C_UTINYINT :
                    *aFuncPtr = zlvSqlDoubleToCUTinyInt;
                    break;
                case SQL_C_SBIGINT :
                    *aFuncPtr = zlvSqlDoubleToCSBigInt;
                    break;
                case SQL_C_UBIGINT :
                    *aFuncPtr = zlvSqlDoubleToCUBigInt;
                    break;
                case SQL_C_SSHORT :
                case SQL_C_SHORT :
                    *aFuncPtr = zlvSqlDoubleToCSShort;
                    break;
                case SQL_C_USHORT :
                    *aFuncPtr = zlvSqlDoubleToCUShort;
                    break;
                case SQL_C_SLONG :
                case SQL_C_LONG :
                    *aFuncPtr = zlvSqlDoubleToCSLong;
                    break;
                case SQL_C_ULONG :
                    *aFuncPtr = zlvSqlDoubleToCULong;
                    break;
                case SQL_C_NUMERIC :
                    *aFuncPtr = zlvSqlDoubleToCNumeric;
                    break;
                case SQL_C_FLOAT :
                    *aFuncPtr = zlvSqlDoubleToCFloat;
                    break;
                case SQL_C_DOUBLE :
                    *aFuncPtr = zlvSqlDoubleToCDouble;
                    break;
                case SQL_C_BIT :
                    *aFuncPtr = zlvSqlDoubleToCBit;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlDoubleToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlDoubleToCLongVarBinary;
                    break;
                case SQL_C_INTERVAL_YEAR_TO_MONTH :
                    *aFuncPtr = zlvSqlDoubleToCIntervalYearToMonth;
                    break;
                case SQL_C_INTERVAL_DAY_TO_HOUR :
                    *aFuncPtr = zlvSqlDoubleToCIntervalDayToHour;
                    break;
                case SQL_C_INTERVAL_DAY_TO_MINUTE :
                    *aFuncPtr = zlvSqlDoubleToCIntervalDayToMinute;
                    break;
                case SQL_C_INTERVAL_DAY_TO_SECOND :
                    *aFuncPtr = zlvSqlDoubleToCIntervalDayToSecond;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                    *aFuncPtr = zlvSqlDoubleToCIntervalHourToMinute;
                    break;
                case SQL_C_INTERVAL_HOUR_TO_SECOND :
                    *aFuncPtr = zlvSqlDoubleToCIntervalHourToSecond;
                    break;
                case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                    *aFuncPtr = zlvSqlDoubleToCIntervalMinuteToSecond;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_BOOLEAN :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlBooleanToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlBooleanToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlBooleanToCLongVarChar;
                    break;
                case SQL_C_STINYINT :
                case SQL_C_TINYINT :
                    *aFuncPtr = zlvSqlBooleanToCSTinyInt;
                    break;
                case SQL_C_UTINYINT :
                    *aFuncPtr = zlvSqlBooleanToCUTinyInt;
                    break;
                case SQL_C_SBIGINT :
                    *aFuncPtr = zlvSqlBooleanToCSBigInt;
                    break;
                case SQL_C_UBIGINT :
                    *aFuncPtr = zlvSqlBooleanToCUBigInt;
                    break;
                case SQL_C_SSHORT :
                case SQL_C_SHORT :
                    *aFuncPtr = zlvSqlBooleanToCSShort;
                    break;
                case SQL_C_USHORT :
                    *aFuncPtr = zlvSqlBooleanToCUShort;
                    break;
                case SQL_C_SLONG :
                case SQL_C_LONG :
                    *aFuncPtr = zlvSqlBooleanToCSLong;
                    break;
                case SQL_C_ULONG :
                    *aFuncPtr = zlvSqlBooleanToCULong;
                    break;
                case SQL_C_NUMERIC :
                    *aFuncPtr = zlvSqlBooleanToCNumeric;
                    break;
                case SQL_C_FLOAT :
                    *aFuncPtr = zlvSqlBooleanToCFloat;
                    break;
                case SQL_C_DOUBLE :
                    *aFuncPtr = zlvSqlBooleanToCDouble;
                    break;
                case SQL_C_BIT :
                    *aFuncPtr = zlvSqlBooleanToCBit;
                    break;
                case SQL_C_BOOLEAN :
                    *aFuncPtr = zlvSqlBooleanToCBoolean;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlBooleanToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlBooleanToCLongVarBinary;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_BINARY :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlBinaryToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlBinaryToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlBinaryToCLongVarChar;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlBinaryToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlBinaryToCLongVarBinary;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_VARBINARY :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlVarBinaryToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlVarBinaryToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlVarBinaryToCLongVarChar;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlVarBinaryToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlVarBinaryToCLongVarBinary;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_LONGVARBINARY :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlLongVarBinaryToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlLongVarBinaryToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlLongVarBinaryToCLongVarChar;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlLongVarBinaryToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlLongVarBinaryToCLongVarBinary;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_TIME :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlTimeToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlTimeToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlTimeToCLongVarChar;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlTimeToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlTimeToCLongVarBinary;
                    break;
                case SQL_C_TYPE_TIME :
                    *aFuncPtr = zlvSqlTimeToCTime;
                    break;
                case SQL_C_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlTimeToCTimeTz;
                    break;
                case SQL_C_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvSqlTimeToCTimestamp;
                    break;
                case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlTimeToCTimestampTz;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_DATE :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlDateToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlDateToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlDateToCLongVarChar;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlDateToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlDateToCLongVarBinary;
                    break;
                case SQL_C_TYPE_DATE :
                    *aFuncPtr = zlvSqlDateToCDate;
                    break;
                case SQL_C_TYPE_TIME :
                    *aFuncPtr = zlvSqlDateToCTime;
                    break;
                case SQL_C_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlDateToCTimeTz;
                    break;
                case SQL_C_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvSqlDateToCTimestamp;
                    break;
                case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlDateToCTimestampTz;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_TIMESTAMP :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlTimestampToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlTimestampToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlTimestampToCLongVarChar;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlTimestampToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlTimestampToCLongVarBinary;
                    break;
                case SQL_C_TYPE_DATE :
                    *aFuncPtr = zlvSqlTimestampToCDate;
                    break;
                case SQL_C_TYPE_TIME :
                    *aFuncPtr = zlvSqlTimestampToCTime;
                    break;
                case SQL_C_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlTimestampToCTimeTz;
                    break;
                case SQL_C_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvSqlTimestampToCTimestamp;
                    break;
                case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlTimestampToCTimestampTz;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_TIME_WITH_TIME_ZONE :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlTimeTzToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlTimeTzToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlTimeTzToCLongVarChar;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlTimeTzToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlTimeTzToCLongVarBinary;
                    break;
                case SQL_C_TYPE_TIME :
                    *aFuncPtr = zlvSqlTimeTzToCTime;
                    break;
                case SQL_C_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlTimeTzToCTimeTz;
                    break;
                case SQL_C_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvSqlTimeTzToCTimestamp;
                    break;
                case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlTimeTzToCTimestampTz;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE :
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlTimestampTzToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlTimestampTzToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlTimestampTzToCLongVarChar;
                    break;
                case SQL_C_BINARY :
                    *aFuncPtr = zlvSqlTimestampTzToCBinary;
                    break;
                case SQL_C_LONGVARBINARY :
                    *aFuncPtr = zlvSqlTimestampTzToCLongVarBinary;
                    break;
                case SQL_C_TYPE_DATE :
                    *aFuncPtr = zlvSqlTimestampTzToCDate;
                    break;
                case SQL_C_TYPE_TIME :
                    *aFuncPtr = zlvSqlTimestampTzToCTime;
                    break;
                case SQL_C_TYPE_TIME_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlTimestampTzToCTimeTz;
                    break;
                case SQL_C_TYPE_TIMESTAMP :
                    *aFuncPtr = zlvSqlTimestampTzToCTimestamp;
                    break;
                case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
                    *aFuncPtr = zlvSqlTimestampTzToCTimestampTz;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            sIntervalYearToMonthType = (dtlIntervalYearToMonthType*)aDataValue->mValue;
            
            switch( sIntervalYearToMonthType->mIndicator )
            {
                case DTL_INTERVAL_INDICATOR_YEAR :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalYearToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalYearToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalYearToCLongVarChar;
                            break;
                        case SQL_C_STINYINT :
                        case SQL_C_TINYINT :
                            *aFuncPtr = zlvSqlIntervalYearToCSTinyInt;
                            break;
                        case SQL_C_UTINYINT :
                            *aFuncPtr = zlvSqlIntervalYearToCUTinyInt;
                            break;
                        case SQL_C_SBIGINT :
                            *aFuncPtr = zlvSqlIntervalYearToCSBigInt;
                            break;
                        case SQL_C_UBIGINT :
                            *aFuncPtr = zlvSqlIntervalYearToCUBigInt;
                            break;
                        case SQL_C_SSHORT :
                        case SQL_C_SHORT :
                            *aFuncPtr = zlvSqlIntervalYearToCSShort;
                            break;
                        case SQL_C_USHORT :
                            *aFuncPtr = zlvSqlIntervalYearToCUShort;
                            break;
                        case SQL_C_SLONG :
                        case SQL_C_LONG :
                            *aFuncPtr = zlvSqlIntervalYearToCSLong;
                            break;
                        case SQL_C_ULONG :
                            *aFuncPtr = zlvSqlIntervalYearToCULong;
                            break;
                        case SQL_C_NUMERIC :
                            *aFuncPtr = zlvSqlIntervalYearToCNumeric;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalYearToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalYearToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_MONTH :
                            *aFuncPtr = zlvSqlIntervalYearToCIntervalMonth;
                            break;
                        case SQL_C_INTERVAL_YEAR :
                            *aFuncPtr = zlvSqlIntervalYearToCIntervalYear;
                            break;
                        case SQL_C_INTERVAL_YEAR_TO_MONTH :
                            *aFuncPtr = zlvSqlIntervalYearToCIntervalYearToMonth;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                case DTL_INTERVAL_INDICATOR_MONTH :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalMonthToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalMonthToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalMonthToCLongVarChar;
                            break;
                        case SQL_C_STINYINT :
                        case SQL_C_TINYINT :
                            *aFuncPtr = zlvSqlIntervalMonthToCSTinyInt;
                            break;
                        case SQL_C_UTINYINT :
                            *aFuncPtr = zlvSqlIntervalMonthToCUTinyInt;
                            break;
                        case SQL_C_SBIGINT :
                            *aFuncPtr = zlvSqlIntervalMonthToCSBigInt;
                            break;
                        case SQL_C_UBIGINT :
                            *aFuncPtr = zlvSqlIntervalMonthToCUBigInt;
                            break;
                        case SQL_C_SSHORT :
                        case SQL_C_SHORT :
                            *aFuncPtr = zlvSqlIntervalMonthToCSShort;
                            break;
                        case SQL_C_USHORT :
                            *aFuncPtr = zlvSqlIntervalMonthToCUShort;
                            break;
                        case SQL_C_SLONG :
                        case SQL_C_LONG :
                            *aFuncPtr = zlvSqlIntervalMonthToCSLong;
                            break;
                        case SQL_C_ULONG :
                            *aFuncPtr = zlvSqlIntervalMonthToCULong;
                            break;
                        case SQL_C_NUMERIC :
                            *aFuncPtr = zlvSqlIntervalMonthToCNumeric;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalMonthToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalMonthToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_MONTH :
                            *aFuncPtr = zlvSqlIntervalMonthToCIntervalMonth;
                            break;
                        case SQL_C_INTERVAL_YEAR :
                            *aFuncPtr = zlvSqlIntervalMonthToCIntervalYear;
                            break;
                        case SQL_C_INTERVAL_YEAR_TO_MONTH :
                            *aFuncPtr = zlvSqlIntervalMonthToCIntervalYearToMonth;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalYearToMonthToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalYearToMonthToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalYearToMonthToCLongVarChar;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalYearToMonthToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalYearToMonthToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_MONTH :
                            *aFuncPtr = zlvSqlIntervalYearToMonthToCIntervalMonth;
                            break;
                        case SQL_C_INTERVAL_YEAR :
                            *aFuncPtr = zlvSqlIntervalYearToMonthToCIntervalYear;
                            break;
                        case SQL_C_INTERVAL_YEAR_TO_MONTH :
                            *aFuncPtr = zlvSqlIntervalYearToMonthToCIntervalYearToMonth;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            sIntervalDayToSecondType = (dtlIntervalDayToSecondType*)aDataValue->mValue;

            switch( sIntervalDayToSecondType->mIndicator )
            {
                case DTL_INTERVAL_INDICATOR_DAY :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalDayToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalDayToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalDayToCLongVarChar;
                            break;
                        case SQL_C_STINYINT :
                        case SQL_C_TINYINT :
                            *aFuncPtr = zlvSqlIntervalDayToCSTinyInt;
                            break;
                        case SQL_C_UTINYINT :
                            *aFuncPtr = zlvSqlIntervalDayToCUTinyInt;
                            break;
                        case SQL_C_SBIGINT :
                            *aFuncPtr = zlvSqlIntervalDayToCSBigInt;
                            break;
                        case SQL_C_UBIGINT :
                            *aFuncPtr = zlvSqlIntervalDayToCUBigInt;
                            break;
                        case SQL_C_SSHORT :
                        case SQL_C_SHORT :
                            *aFuncPtr = zlvSqlIntervalDayToCSShort;
                            break;
                        case SQL_C_USHORT :
                            *aFuncPtr = zlvSqlIntervalDayToCUShort;
                            break;
                        case SQL_C_SLONG :
                        case SQL_C_LONG :
                            *aFuncPtr = zlvSqlIntervalDayToCSLong;
                            break;
                        case SQL_C_ULONG :
                            *aFuncPtr = zlvSqlIntervalDayToCULong;
                            break;
                        case SQL_C_NUMERIC :
                            *aFuncPtr = zlvSqlIntervalDayToCNumeric;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalDayToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalDayToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_DAY :
                            *aFuncPtr = zlvSqlIntervalDayToCIntervalDay;
                            break;
                        case SQL_C_INTERVAL_HOUR :
                            *aFuncPtr = zlvSqlIntervalDayToCIntervalHour;
                            break;
                        case SQL_C_INTERVAL_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToCIntervalMinute;
                            break;
                        case SQL_C_INTERVAL_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToCIntervalSecond;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_HOUR :
                            *aFuncPtr = zlvSqlIntervalDayToCIntervalDayToHour;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToCIntervalDayToMinute;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToCIntervalDayToSecond;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToCIntervalHourToMinute;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToCIntervalHourToSecond;
                            break;
                        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToCIntervalMinuteToSecond;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                case DTL_INTERVAL_INDICATOR_HOUR :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalHourToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalHourToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalHourToCLongVarChar;
                            break;
                        case SQL_C_STINYINT :
                        case SQL_C_TINYINT :
                            *aFuncPtr = zlvSqlIntervalHourToCSTinyInt;
                            break;
                        case SQL_C_UTINYINT :
                            *aFuncPtr = zlvSqlIntervalHourToCUTinyInt;
                            break;
                        case SQL_C_SBIGINT :
                            *aFuncPtr = zlvSqlIntervalHourToCSBigInt;
                            break;
                        case SQL_C_UBIGINT :
                            *aFuncPtr = zlvSqlIntervalHourToCUBigInt;
                            break;
                        case SQL_C_SSHORT :
                        case SQL_C_SHORT :
                            *aFuncPtr = zlvSqlIntervalHourToCSShort;
                            break;
                        case SQL_C_USHORT :
                            *aFuncPtr = zlvSqlIntervalHourToCUShort;
                            break;
                        case SQL_C_SLONG :
                        case SQL_C_LONG :
                            *aFuncPtr = zlvSqlIntervalHourToCSLong;
                            break;
                        case SQL_C_ULONG :
                            *aFuncPtr = zlvSqlIntervalHourToCULong;
                            break;
                        case SQL_C_NUMERIC :
                            *aFuncPtr = zlvSqlIntervalHourToCNumeric;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalHourToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalHourToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_DAY :
                            *aFuncPtr = zlvSqlIntervalHourToCIntervalDay;
                            break;
                        case SQL_C_INTERVAL_HOUR :
                            *aFuncPtr = zlvSqlIntervalHourToCIntervalHour;
                            break;
                        case SQL_C_INTERVAL_MINUTE :
                            *aFuncPtr = zlvSqlIntervalHourToCIntervalMinute;
                            break;
                        case SQL_C_INTERVAL_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToCIntervalSecond;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_HOUR :
                            *aFuncPtr = zlvSqlIntervalHourToCIntervalDayToHour;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalHourToCIntervalDayToMinute;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToCIntervalDayToSecond;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalHourToCIntervalHourToMinute;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToCIntervalHourToSecond;
                            break;
                        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToCIntervalMinuteToSecond;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                case DTL_INTERVAL_INDICATOR_MINUTE :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalMinuteToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalMinuteToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalMinuteToCLongVarChar;
                            break;
                        case SQL_C_STINYINT :
                        case SQL_C_TINYINT :
                            *aFuncPtr = zlvSqlIntervalMinuteToCSTinyInt;
                            break;
                        case SQL_C_UTINYINT :
                            *aFuncPtr = zlvSqlIntervalMinuteToCUTinyInt;
                            break;
                        case SQL_C_SBIGINT :
                            *aFuncPtr = zlvSqlIntervalMinuteToCSBigInt;
                            break;
                        case SQL_C_UBIGINT :
                            *aFuncPtr = zlvSqlIntervalMinuteToCUBigInt;
                            break;
                        case SQL_C_SSHORT :
                        case SQL_C_SHORT :
                            *aFuncPtr = zlvSqlIntervalMinuteToCSShort;
                            break;
                        case SQL_C_USHORT :
                            *aFuncPtr = zlvSqlIntervalMinuteToCUShort;
                            break;
                        case SQL_C_SLONG :
                        case SQL_C_LONG :
                            *aFuncPtr = zlvSqlIntervalMinuteToCSLong;
                            break;
                        case SQL_C_ULONG :
                            *aFuncPtr = zlvSqlIntervalMinuteToCULong;
                            break;
                        case SQL_C_NUMERIC :
                            *aFuncPtr = zlvSqlIntervalMinuteToCNumeric;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalMinuteToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalMinuteToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_DAY :
                            *aFuncPtr = zlvSqlIntervalMinuteToCIntervalDay;
                            break;
                        case SQL_C_INTERVAL_HOUR :
                            *aFuncPtr = zlvSqlIntervalMinuteToCIntervalHour;
                            break;
                        case SQL_C_INTERVAL_MINUTE :
                            *aFuncPtr = zlvSqlIntervalMinuteToCIntervalMinute;
                            break;
                        case SQL_C_INTERVAL_SECOND :
                            *aFuncPtr = zlvSqlIntervalMinuteToCIntervalSecond;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_HOUR :
                            *aFuncPtr = zlvSqlIntervalMinuteToCIntervalDayToHour;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalMinuteToCIntervalDayToMinute;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalMinuteToCIntervalDayToSecond;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalMinuteToCIntervalHourToMinute;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalMinuteToCIntervalHourToSecond;
                            break;
                        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalMinuteToCIntervalMinuteToSecond;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                case DTL_INTERVAL_INDICATOR_SECOND :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalSecondToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalSecondToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalSecondToCLongVarChar;
                            break;
                        case SQL_C_STINYINT :
                        case SQL_C_TINYINT :
                            *aFuncPtr = zlvSqlIntervalSecondToCSTinyInt;
                            break;
                        case SQL_C_UTINYINT :
                            *aFuncPtr = zlvSqlIntervalSecondToCUTinyInt;
                            break;
                        case SQL_C_SBIGINT :
                            *aFuncPtr = zlvSqlIntervalSecondToCSBigInt;
                            break;
                        case SQL_C_UBIGINT :
                            *aFuncPtr = zlvSqlIntervalSecondToCUBigInt;
                            break;
                        case SQL_C_SSHORT :
                        case SQL_C_SHORT :
                            *aFuncPtr = zlvSqlIntervalSecondToCSShort;
                            break;
                        case SQL_C_USHORT :
                            *aFuncPtr = zlvSqlIntervalSecondToCUShort;
                            break;
                        case SQL_C_SLONG :
                        case SQL_C_LONG :
                            *aFuncPtr = zlvSqlIntervalSecondToCSLong;
                            break;
                        case SQL_C_ULONG :
                            *aFuncPtr = zlvSqlIntervalSecondToCULong;
                            break;
                        case SQL_C_NUMERIC :
                            *aFuncPtr = zlvSqlIntervalSecondToCNumeric;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalSecondToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalSecondToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_DAY :
                            *aFuncPtr = zlvSqlIntervalSecondToCIntervalDay;
                            break;
                        case SQL_C_INTERVAL_HOUR :
                            *aFuncPtr = zlvSqlIntervalSecondToCIntervalHour;
                            break;
                        case SQL_C_INTERVAL_MINUTE :
                            *aFuncPtr = zlvSqlIntervalSecondToCIntervalMinute;
                            break;
                        case SQL_C_INTERVAL_SECOND :
                            *aFuncPtr = zlvSqlIntervalSecondToCIntervalSecond;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_HOUR :
                            *aFuncPtr = zlvSqlIntervalSecondToCIntervalDayToHour;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalSecondToCIntervalDayToMinute;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalSecondToCIntervalDayToSecond;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalSecondToCIntervalHourToMinute;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalSecondToCIntervalHourToSecond;
                            break;
                        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalSecondToCIntervalMinuteToSecond;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCLongVarChar;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_DAY :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCIntervalDay;
                            break;
                        case SQL_C_INTERVAL_HOUR :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCIntervalHour;
                            break;
                        case SQL_C_INTERVAL_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCIntervalMinute;
                            break;
                        case SQL_C_INTERVAL_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCIntervalSecond;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_HOUR :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCIntervalDayToHour;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCIntervalDayToMinute;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCIntervalDayToSecond;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCIntervalHourToMinute;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCIntervalHourToSecond;
                            break;
                        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToHourToCIntervalMinuteToSecond;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCLongVarChar;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_DAY :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCIntervalDay;
                            break;
                        case SQL_C_INTERVAL_HOUR :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCIntervalHour;
                            break;
                        case SQL_C_INTERVAL_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCIntervalMinute;
                            break;
                        case SQL_C_INTERVAL_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCIntervalSecond;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_HOUR :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCIntervalDayToHour;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCIntervalDayToMinute;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCIntervalDayToSecond;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCIntervalHourToMinute;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCIntervalHourToSecond;
                            break;
                        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToMinuteToCIntervalMinuteToSecond;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCLongVarChar;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_DAY :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCIntervalDay;
                            break;
                        case SQL_C_INTERVAL_HOUR :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCIntervalHour;
                            break;
                        case SQL_C_INTERVAL_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCIntervalMinute;
                            break;
                        case SQL_C_INTERVAL_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCIntervalSecond;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_HOUR :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCIntervalDayToHour;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCIntervalDayToMinute;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCIntervalDayToSecond;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCIntervalHourToMinute;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCIntervalHourToSecond;
                            break;
                        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalDayToSecondToCIntervalMinuteToSecond;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCLongVarChar;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_DAY :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCIntervalDay;
                            break;
                        case SQL_C_INTERVAL_HOUR :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCIntervalHour;
                            break;
                        case SQL_C_INTERVAL_MINUTE :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCIntervalMinute;
                            break;
                        case SQL_C_INTERVAL_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCIntervalSecond;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_HOUR :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCIntervalDayToHour;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCIntervalDayToMinute;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCIntervalDayToSecond;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCIntervalHourToMinute;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCIntervalHourToSecond;
                            break;
                        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToMinuteToCIntervalMinuteToSecond;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCLongVarChar;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_DAY :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCIntervalDay;
                            break;
                        case SQL_C_INTERVAL_HOUR :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCIntervalHour;
                            break;
                        case SQL_C_INTERVAL_MINUTE :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCIntervalMinute;
                            break;
                        case SQL_C_INTERVAL_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCIntervalSecond;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_HOUR :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCIntervalDayToHour;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCIntervalDayToMinute;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCIntervalDayToSecond;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCIntervalHourToMinute;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCIntervalHourToSecond;
                            break;
                        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalHourToSecondToCIntervalMinuteToSecond;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND :
                    switch( aCType )
                    {
                        case SQL_C_CHAR :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCChar;
                            break;
                        case SQL_C_WCHAR :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCWchar;
                            break;
                        case SQL_C_LONGVARCHAR :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCLongVarChar;
                            break;
                        case SQL_C_BINARY :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCBinary;
                            break;
                        case SQL_C_LONGVARBINARY :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCLongVarBinary;
                            break;
                        case SQL_C_INTERVAL_DAY :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCIntervalDay;
                            break;
                        case SQL_C_INTERVAL_HOUR :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCIntervalHour;
                            break;
                        case SQL_C_INTERVAL_MINUTE :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCIntervalMinute;
                            break;
                        case SQL_C_INTERVAL_SECOND :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCIntervalSecond;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_HOUR :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCIntervalDayToHour;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCIntervalDayToMinute;
                            break;
                        case SQL_C_INTERVAL_DAY_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCIntervalDayToSecond;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCIntervalHourToMinute;
                            break;
                        case SQL_C_INTERVAL_HOUR_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCIntervalHourToSecond;
                            break;
                        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
                            *aFuncPtr = zlvSqlIntervalMinuteToSecondToCIntervalMinuteToSecond;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                            break;
                    }
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;
        case DTL_TYPE_ROWID:
            switch( aCType )
            {
                case SQL_C_CHAR :
                    *aFuncPtr = zlvSqlRowIdToCChar;
                    break;
                case SQL_C_WCHAR :
                    *aFuncPtr = zlvSqlRowIdToCWchar;
                    break;
                case SQL_C_LONGVARCHAR :
                    *aFuncPtr = zlvSqlRowIdToCLongVarChar;
                    break;
                default :
                    STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
                    break;
            }
            break;            
        default :
            STL_THROW( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION,
                      "The data value of a column in the result set could not be converted "
                      "to the data type specified by TargetType in SQLBindCol.",
                      aErrorStack );

        *aFuncPtr = zlvInvalidSqlToC;
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus zlvInvalidSqlToC( zlsStmt           * aStmt,
                            dtlValueBlockList * aValueBlockList,
                            dtlDataValue      * aDataValue,
                            SQLLEN            * aOffset,
                            void              * aTargetValuePtr,
                            SQLLEN            * aIndicator,
                            zldDescElement    * aArdRec,
                            SQLRETURN         * aReturn,
                            stlErrorStack     * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  ZLE_ERRCODE_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION,
                  "The data value of a column in the result set could not be converted "
                  "to the data type specified by TargetType in SQLBindCol.",
                  aErrorStack );

    return STL_FAILURE;
}

/** @} */
