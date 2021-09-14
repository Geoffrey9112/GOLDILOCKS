/*******************************************************************************
 * dtdDataType.c
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

/**
 * @file dtdDataType.c
 * @brief DataType Layer Numeric type 정의
 */

#include <dtl.h>
#include <dtDef.h>

/**
 * @addtogroup dtdDataType
 * @{
 */

stlStatus dtdNotApplicableGetLength( stlInt64              aPrecision,
                                     dtlStringLengthUnit   aStringUnit,
                                     stlInt64            * aLength,
                                     dtlFuncVector       * aVectorFunc,
                                     void                * aVectorArg,
                                     stlErrorStack       * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableGetLength()" );

    return STL_FAILURE;
}

stlStatus dtdNotApplicableGetLengthFromString( stlInt64              aPrecision,
                                               stlInt64              aScale,
                                               dtlStringLengthUnit   aStringUnit,
                                               stlChar             * aString,
                                               stlInt64              aStringLength,
                                               stlInt64            * aLength,
                                               dtlFuncVector       * aVectorFunc,
                                               void                * aVectorArg,
                                               stlErrorStack       * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableGetLengthFromString()" );

    return STL_FAILURE;
}    

stlStatus dtdNotApplicableGetLengthFromInteger( stlInt64          aPrecision,
                                                stlInt64          aScale,
                                                stlInt64          aInteger, 
                                                stlInt64        * aLength,
                                                stlErrorStack   * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableGetLengthFromInteger()" );

    return STL_FAILURE;
}

stlStatus dtdNotApplicableGetLengthFromReal( stlInt64          aPrecision,
                                             stlInt64          aScale,
                                             stlFloat64        aReal, 
                                             stlInt64        * aLength,
                                             stlErrorStack   * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableGetLengthFromReal()" );

    return STL_FAILURE;
}

stlStatus dtdNotApplicableSetValueFromString( stlChar             * aString,
                                              stlInt64              aStringLength,
                                              stlInt64              aPrecision,
                                              stlInt64              aScale,
                                              dtlStringLengthUnit   aStringUnit,
                                              dtlIntervalIndicator  aIntervalIndicator,
                                              stlInt64              aAvailableSize,
                                              dtlDataValue        * aValue,
                                              stlBool             * aSuccessWithInfo,
                                              dtlFuncVector       * aSourceVectorFunc,
                                              void                * aSourceVectorArg,
                                              dtlFuncVector       * aDestVectorFunc,
                                              void                * aDestVectorArg,
                                              stlErrorStack       * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableSetValueFromString()" );

    return STL_FAILURE;
}

stlStatus dtdNotApplicableSetValueFromInteger( stlInt64               aInteger,
                                               stlInt64               aPrecision,
                                               stlInt64               aScale,
                                               dtlStringLengthUnit    aStringUnit,
                                               dtlIntervalIndicator   aIntervalIndicator,
                                               stlInt64               aAvailableSize,
                                               dtlDataValue         * aValue,
                                               stlBool              * aSuccessWithInfo,
                                               dtlFuncVector        * aVectorFunc,
                                               void                 * aVectorArg,
                                               stlErrorStack        * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableSetValueFromInteger()" );

    return STL_FAILURE;
}

stlStatus dtdNotApplicableSetValueFromReal( stlFloat64             aReal,
                                            stlInt64               aPrecision,
                                            stlInt64               aScale,
                                            dtlStringLengthUnit    aStringUnit,
                                            dtlIntervalIndicator   aIntervalIndicator,
                                            stlInt64               aAvailableSize,
                                            dtlDataValue         * aValue,
                                            stlBool              * aSuccessWithInfo,
                                            dtlFuncVector        * aVectorFunc,
                                            void                 * aVectorArg,
                                            stlErrorStack        * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableSetValueFromReal()" );

    return STL_FAILURE;
}

stlStatus dtdNotApplicableReverseByteOrder( void            * aValue,
                                            stlBool           aIsSameEndian,
                                            stlErrorStack   * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableReverseByteOrder()" );

    return STL_FAILURE;
}

stlStatus dtdNotApplicableToString( dtlDataValue    * aValue,
                                    stlInt64          aPrecision,
                                    stlInt64          aScale,
                                    stlInt64          aAvailableSize,
                                    void            * aBuffer,
                                    stlInt64        * aLength,
                                    dtlFuncVector   * aVectorFunc,
                                    void            * aVectorArg,
                                    stlErrorStack   * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableToString()" );

    return STL_FAILURE;
}

stlStatus dtdNotApplicableSetValueFromTypedLiteral( stlChar             * aString,
                                                    stlInt64              aStringLength,
                                                    stlInt64              aAvailableSize,
                                                    dtlDataValue        * aValue,
                                                    dtlFuncVector       * aVectorFunc,
                                                    void                * aVectorArg,
                                                    stlErrorStack       * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableSetValueFromTypedLiteral()" );
    
    return STL_FAILURE;
}

stlStatus dtdNotApplicableSetValueFromWCharString( void                * aString,
                                                   stlInt64              aStringLength,
                                                   stlInt64              aPrecision,
                                                   stlInt64              aScale,
                                                   dtlStringLengthUnit   aStringUnit,
                                                   dtlIntervalIndicator  aIntervalIndicator,
                                                   stlInt64              aAvailableSize,
                                                   dtlDataValue        * aValue,
                                                   stlBool             * aSuccessWithInfo,
                                                   dtlFuncVector       * aVectorFunc,
                                                   void                * aVectorArg,
                                                   stlErrorStack       * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableSetValueFromWCharString()" );

    return STL_FAILURE;
}

stlStatus dtdNotApplicableToWCharString( dtlDataValue    * aValue,
                                         stlInt64          aPrecision,
                                         stlInt64          aScale,
                                         stlInt64          aAvailableSize,
                                         void            * aBuffer,
                                         stlInt64        * aLength,
                                         dtlFuncVector   * aVectorFunc,
                                         void            * aVectorArg,
                                         stlErrorStack   * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableToWCharString()" );

    return STL_FAILURE;
}

stlStatus dtdNotApplicableGetWCharStringFromBinaryString( stlInt8         * aNumber,
                                                          stlInt64          aNumberLength,
                                                          stlInt64          aAvailableSize,
                                                          void            * aHex,
                                                          stlInt64        * aHexLength, 
                                                          stlErrorStack   * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  DTL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  aErrorStack,
                  "dtdNotApplicableGetWCharStringFromBinaryString()" );

    return STL_FAILURE;
}



dtdGetLengthFunc dtdGetLengthFuncList[ DTL_TYPE_MAX ] = 
{
    dtdBooleanGetLength,
    dtdSmallIntGetLength,
    dtdIntegerGetLength,
    dtdBigIntGetLength,
    dtdRealGetLength,
    dtdDoubleGetLength,
    dtdFloatGetLength,
    dtdNumericGetLength,
    dtdNotApplicableGetLength,
    dtdCharGetLength,
    dtdVarcharGetLength,
    dtdLongvarcharGetLength,
    dtdNotApplicableGetLength,
    dtdBinaryGetLength,
    dtdVarbinaryGetLength,
    dtdLongvarbinaryGetLength,
    dtdNotApplicableGetLength,
    dtdDateGetLength,
    dtdTimeGetLength,
    dtdTimestampGetLength,
    dtdTimeTzGetLength,
    dtdTimestampTzGetLength,
    dtdIntervalYearToMonthGetLength,
    dtdIntervalDayToSecondGetLength,
    dtdRowIdGetLength    
};

dtdGetLengthFromStringFunc dtdGetLengthFromStringFuncList[ DTL_TYPE_MAX ] = 
{
    dtdBooleanGetLengthFromString,
    dtdSmallIntGetLengthFromString,
    dtdIntegerGetLengthFromString,
    dtdBigIntGetLengthFromString,
    dtdRealGetLengthFromString,
    dtdDoubleGetLengthFromString,
    dtdFloatGetLengthFromString,    
    dtdNumericGetLengthFromString,
    dtdNotApplicableGetLengthFromString,
    dtdCharGetLengthFromString,
    dtdVarcharGetLengthFromString,
    dtdLongvarcharGetLengthFromString,
    dtdNotApplicableGetLengthFromString,
    dtdBinaryGetLengthFromString,
    dtdVarbinaryGetLengthFromString,
    dtdLongvarbinaryGetLengthFromString,
    dtdNotApplicableGetLengthFromString,
    dtdDateGetLengthFromString,
    dtdTimeGetLengthFromString,
    dtdTimestampGetLengthFromString,
    dtdTimeTzGetLengthFromString,
    dtdTimestampTzGetLengthFromString,
    dtdIntervalYearToMonthGetLengthFromString,
    dtdIntervalDayToSecondGetLengthFromString,
    dtdRowIdGetLengthFromString
};

dtdGetLengthFromIntegerFunc dtdGetLengthFromIntegerFuncList[ DTL_TYPE_MAX ] = 
{
    dtdBooleanGetLengthFromInteger,
    dtdSmallIntGetLengthFromInteger,
    dtdIntegerGetLengthFromInteger,
    dtdBigIntGetLengthFromInteger,
    dtdRealGetLengthFromInteger,
    dtdDoubleGetLengthFromInteger,
    dtdFloatGetLengthFromInteger,    
    dtdNumericGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
    dtdIntervalYearToMonthGetLengthFromInteger,
    dtdIntervalDayToSecondGetLengthFromInteger,
    dtdNotApplicableGetLengthFromInteger,
};

dtdGetLengthFromRealFunc dtdGetLengthFromRealFuncList[ DTL_TYPE_MAX ] = 
{
    dtdNotApplicableGetLengthFromReal,
    dtdSmallIntGetLengthFromReal,
    dtdRealGetLengthFromReal,
    dtdBigIntGetLengthFromReal,
    dtdRealGetLengthFromReal,
    dtdDoubleGetLengthFromReal,
    dtdFloatGetLengthFromReal,    
    dtdNumericGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
    dtdNotApplicableGetLengthFromReal,
};

dtdSetValueFromStringFunc dtdSetValueFromStringFuncList[ DTL_TYPE_MAX ] = 
{
    dtdBooleanSetValueFromString,
    dtdSmallIntSetValueFromString,
    dtdIntegerSetValueFromString,
    dtdBigIntSetValueFromString,
    dtdRealSetValueFromString,
    dtdDoubleSetValueFromString,
    dtdFloatSetValueFromString,    
    dtdNumericSetValueFromString,
    dtdNotApplicableSetValueFromString,
    dtdCharSetValueFromString,
    dtdVarcharSetValueFromString,
    dtdLongvarcharSetValueFromString,
    dtdNotApplicableSetValueFromString,
    dtdBinarySetValueFromString,
    dtdVarbinarySetValueFromString,
    dtdLongvarbinarySetValueFromString,
    dtdNotApplicableSetValueFromString,
    dtdDateSetValueFromString,
    dtdTimeSetValueFromString,
    dtdTimestampSetValueFromString,
    dtdTimeTzSetValueFromString,
    dtdTimestampTzSetValueFromString,
    dtdIntervalYearToMonthSetValueFromString,
    dtdIntervalDayToSecondSetValueFromString,
    dtdRowIdSetValueFromString    
};

dtdSetValueFromIntegerFunc dtdSetValueFromIntegerFuncList[ DTL_TYPE_MAX ] = 
{
    dtdBooleanSetValueFromInteger,
    dtdSmallIntSetValueFromInteger,
    dtdIntegerSetValueFromInteger,
    dtdBigIntSetValueFromInteger,
    dtdRealSetValueFromInteger,
    dtdDoubleSetValueFromInteger,
    dtdFloatSetValueFromInteger,    
    dtdNumericSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger,
    dtdCharSetValueFromInteger,
    dtdVarcharSetValueFromInteger,
    dtdLongvarcharSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger,
    dtdIntervalYearToMonthSetValueFromInteger,
    dtdIntervalDayToSecondSetValueFromInteger,
    dtdNotApplicableSetValueFromInteger
};

dtdSetValueFromRealFunc dtdSetValueFromRealFuncList[ DTL_TYPE_MAX ] = 
{
    dtdNotApplicableSetValueFromReal,
    dtdSmallIntSetValueFromReal,
    dtdIntegerSetValueFromReal,
    dtdBigIntSetValueFromReal,
    dtdRealSetValueFromReal,
    dtdDoubleSetValueFromReal,
    dtdFloatSetValueFromReal,    
    dtdNumericSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdCharSetValueFromReal,
    dtdVarcharSetValueFromReal,
    dtdLongvarcharSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal,
    dtdNotApplicableSetValueFromReal
};

dtdReverseByteOrderFunc dtdReverseByteOrderFuncList[ DTL_TYPE_MAX ] = 
{
    dtdBooleanReverseByteOrder,
    dtdSmallIntReverseByteOrder,
    dtdIntegerReverseByteOrder,
    dtdBigIntReverseByteOrder,
    dtdRealReverseByteOrder,
    dtdDoubleReverseByteOrder,
    dtdNumericReverseByteOrder,    
    dtdNumericReverseByteOrder,
    dtdNotApplicableReverseByteOrder,
    dtdCharReverseByteOrder,
    dtdVarcharReverseByteOrder,
    dtdLongvarcharReverseByteOrder,
    dtdNotApplicableReverseByteOrder,
    dtdBinaryReverseByteOrder,
    dtdVarbinaryReverseByteOrder,
    dtdLongvarbinaryReverseByteOrder,
    dtdNotApplicableReverseByteOrder,
    dtdDateReverseByteOrder,
    dtdTimeReverseByteOrder,
    dtdTimestampReverseByteOrder,
    dtdTimeTzReverseByteOrder,
    dtdTimestampTzReverseByteOrder,
    dtdIntervalYearToMonthReverseByteOrder,
    dtdIntervalDayToSecondReverseByteOrder,
    dtdRowIdReverseByteOrder    
};

dtdToStringFunc dtdToStringFuncList[ DTL_TYPE_MAX ] = 
{
    dtdBooleanToString,
    dtdSmallIntToString,
    dtdIntegerToString,
    dtdBigIntToString,
    dtdRealToString,
    dtdDoubleToString,
    dtdNumericToString,    
    dtdNumericToString,
    dtdNotApplicableToString,
    dtdCharToString,
    dtdVarcharToString,
    dtdLongvarcharToString,
    dtdNotApplicableToString,
    dtdBinaryToString,
    dtdVarbinaryToString,
    dtdLongvarbinaryToString,
    dtdNotApplicableToString,
    dtdDateToString,
    dtdTimeToString,
    dtdTimestampToString,
    dtdTimeTzToString,
    dtdTimestampTzToString,
    dtdIntervalYearToMonthToString,
    dtdIntervalDayToSecondToString,
    dtdRowIdToString    
};

dtdToStringForAvailableSizeFunc dtdToStringForAvailableSizeFuncList[ DTL_TYPE_MAX ] = 
{
    dtdBooleanToStringForAvailableSize,
    dtdSmallIntToStringForAvailableSize,
    dtdIntegerToStringForAvailableSize,
    dtdBigIntToStringForAvailableSize,
    dtdRealToStringForAvailableSize,
    dtdDoubleToStringForAvailableSize,
    dtdNumericToStringForAvailableSize,
    dtdNumericToStringForAvailableSize,
    dtdNotApplicableToString,
    dtdCharToStringForAvailableSize,
    dtdVarcharToStringForAvailableSize,
    dtdLongvarcharToStringForAvailableSize,
    dtdNotApplicableToString,
    dtdBinaryToStringForAvailableSize,
    dtdVarbinaryToStringForAvailableSize,
    dtdLongvarbinaryToStringForAvailableSize,
    dtdNotApplicableToString,
    dtdDateToStringForAvailableSize,
    dtdTimeToStringForAvailableSize,
    dtdTimestampToStringForAvailableSize,
    dtdTimeTzToStringForAvailableSize,
    dtdTimestampTzToStringForAvailableSize,
    dtdIntervalYearToMonthToStringForAvailableSize,
    dtdIntervalDayToSecondToStringForAvailableSize,
    dtdRowIdToStringForAvailableSize
};


dtdDateTimeSetValueFromTypedLiteralFunc dtdDateTimeSetValueFromTypedLiteralFuncList[ DTL_TYPE_MAX ] =
{
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdDateSetValueFromTypedLiteral,          // DATE
    dtdTimeSetValueFromTypedLiteral,          // TIME
    dtdTimestampSetValueFromTypedLiteral,     // TIMESTAMP
    dtdTimeTzSetValueFromTypedLiteral,        // TIME_TZ
    dtdTimestampTzSetValueFromTypedLiteral,   // TIMESTAMP_TZ
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral,
    dtdNotApplicableSetValueFromTypedLiteral
};


dtdSetValueFromWCharStringFunc dtdSetValueFromWCharStringFuncList[ DTL_TYPE_MAX ][ DTL_UNICODE_MAX ] = 
{
    /* DTL_TYPE_BOOLEAN */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdBooleanSetValueFromUtf16WCharString,       /* DTL_UNICODE_UTF16 */
      dtdBooleanSetValueFromUtf32WCharString },     /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NATIVE_SMALLINT */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF16 */
      dtdNotApplicableSetValueFromWCharString },    /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NATIVE_INTEGER */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF16 */
      dtdNotApplicableSetValueFromWCharString },    /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NATIVE_BIGINT */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF16 */
      dtdNotApplicableSetValueFromWCharString },    /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NATIVE_REAL */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF16 */
      dtdNotApplicableSetValueFromWCharString },    /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NATIVE_DOUBLE */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF16 */
      dtdNotApplicableSetValueFromWCharString },    /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_FLOAT */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdFloatSetValueFromUtf16WCharString,         /* DTL_UNICODE_UTF16 */
      dtdFloatSetValueFromUtf32WCharString },       /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NUMBER */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdNumericSetValueFromUtf16WCharString,       /* DTL_UNICODE_UTF16 */
      dtdNumericSetValueFromUtf32WCharString },     /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_DECIMAL */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF16 */
      dtdNotApplicableSetValueFromWCharString },    /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_CHAR */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF16 */
      dtdNotApplicableSetValueFromWCharString },    /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_VARCHAR */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdVarcharSetValueFromUtf16WCharString,       /* DTL_UNICODE_UTF16 */
      dtdVarcharSetValueFromUtf32WCharString },     /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_LONGVARCHAR */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdLongvarcharSetValueFromUtf16WCharString,   /* DTL_UNICODE_UTF16 */
      dtdLongvarcharSetValueFromUtf32WCharString }, /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_CLOB */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF16 */
      dtdNotApplicableSetValueFromWCharString },    /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_BINARY */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF16 */
      dtdNotApplicableSetValueFromWCharString },    /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_VARBINARY */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdVarbinarySetValueFromUtf16WCharString,     /* DTL_UNICODE_UTF16 */
      dtdVarbinarySetValueFromUtf32WCharString },   /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_LONGVARBINARY */
    { dtdNotApplicableSetValueFromWCharString,        /* DTL_UNICODE_UTF8  */
      dtdLongvarbinarySetValueFromUtf16WCharString,   /* DTL_UNICODE_UTF16 */
      dtdLongvarbinarySetValueFromUtf32WCharString }, /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_BLOB */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF16 */
      dtdNotApplicableSetValueFromWCharString },    /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_DATE */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdDateSetValueFromUtf16WCharString,          /* DTL_UNICODE_UTF16 */
      dtdDateSetValueFromUtf32WCharString },        /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_TIME */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdTimeSetValueFromUtf16WCharString,          /* DTL_UNICODE_UTF16 */
      dtdTimeSetValueFromUtf32WCharString },        /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_TIMESTAMP */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdTimestampSetValueFromUtf16WCharString,     /* DTL_UNICODE_UTF16 */
      dtdTimestampSetValueFromUtf32WCharString },   /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdTimeTzSetValueFromUtf16WCharString,        /* DTL_UNICODE_UTF16 */
      dtdTimeTzSetValueFromUtf32WCharString },      /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdTimestampTzSetValueFromUtf16WCharString,   /* DTL_UNICODE_UTF16 */
      dtdTimestampTzSetValueFromUtf32WCharString }, /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    { dtdNotApplicableSetValueFromWCharString,              /* DTL_UNICODE_UTF8  */
      dtdIntervalYearToMonthSetValueFromUtf16WCharString,   /* DTL_UNICODE_UTF16 */
      dtdIntervalYearToMonthSetValueFromUtf32WCharString }, /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
    { dtdNotApplicableSetValueFromWCharString,              /* DTL_UNICODE_UTF8  */
      dtdIntervalDayToSecondSetValueFromUtf16WCharString,   /* DTL_UNICODE_UTF16 */
      dtdIntervalDayToSecondSetValueFromUtf32WCharString }, /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_ROWID */
    { dtdNotApplicableSetValueFromWCharString,      /* DTL_UNICODE_UTF8  */
      dtdRowIdSetValueFromUtf16WCharString,         /* DTL_UNICODE_UTF16 */
      dtdRowIdSetValueFromUtf32WCharString },       /* DTL_UNICODE_UTF32 */ 
};


dtdToWCharStringFunc dtdToWCharStringFuncList[ DTL_TYPE_MAX ][ DTL_UNICODE_MAX ] = 
{
    /* DTL_TYPE_BOOLEAN */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NATIVE_SMALLINT */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NATIVE_INTEGER */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NATIVE_BIGINT */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NATIVE_REAL */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NATIVE_DOUBLE */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_FLOAT */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_NUMBER */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_DECIMAL */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_CHAR */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_VARCHAR */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_LONGVARCHAR */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_CLOB */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_BINARY */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_VARBINARY */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_LONGVARBINARY */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_BLOB */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF16 */
      dtdNotApplicableToWCharString },              /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_DATE */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdDateToUtf16WCharString,                    /* DTL_UNICODE_UTF16 */
      dtdDateToUtf32WCharString },                  /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_TIME */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdTimeToUtf16WCharString,                    /* DTL_UNICODE_UTF16 */
      dtdTimeToUtf32WCharString },                  /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_TIMESTAMP */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdTimestampToUtf16WCharString,               /* DTL_UNICODE_UTF16 */
      dtdTimestampToUtf32WCharString },             /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdTimeTzToUtf16WCharString,                  /* DTL_UNICODE_UTF16 */
      dtdTimeTzToUtf32WCharString },                /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdTimestampTzToUtf16WCharString,             /* DTL_UNICODE_UTF16 */
      dtdTimestampTzToUtf32WCharString },           /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdIntervalYearToMonthToUtf16WCharString,     /* DTL_UNICODE_UTF16 */
      dtdIntervalYearToMonthToUtf32WCharString },   /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdIntervalDayToSecondToUtf16WCharString,     /* DTL_UNICODE_UTF16 */
      dtdIntervalDayToSecondToUtf32WCharString },   /* DTL_UNICODE_UTF32 */ 

    /* DTL_TYPE_ROWID */
    { dtdNotApplicableToWCharString,                /* DTL_UNICODE_UTF8  */
      dtdRowIdToUtf16WCharString,                   /* DTL_UNICODE_UTF16 */
      dtdRowIdToUtf32WCharString },                 /* DTL_UNICODE_UTF32 */ 
};

dtdGetWCharStringFromBinaryStringFunc dtdGetWCharStringFromBinaryStringFuncList[ DTL_UNICODE_MAX ] = 
{
    dtdNotApplicableGetWCharStringFromBinaryString, /* DTL_UNICODE_UTF8  */
    dtdToUtf16WCharStringFromBinaryString,          /* DTL_UNICODE_UTF16 */
    dtdToUtf32WCharStringFromBinaryString           /* DTL_UNICODE_UTF32 */ 
};
    

/** @} */
