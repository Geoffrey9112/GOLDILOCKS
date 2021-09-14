/*******************************************************************************
 * dtdDataType.h
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
#ifndef _DTDDATATYPE_H_
#define _DTDDATATYPE_H_ 1

/**
 * @file dtdDataType.h
 * @brief DataType Layer dtlDataValue에 대한 function pointer list
 */

/**
 * @defgroup dtdDataType Data Type Function Pointers 
 * @ingroup dtInternal
 * @{
 */

typedef stlStatus (*dtdGetLengthFunc) ( stlInt64              aPrecision,
                                        dtlStringLengthUnit   aStringUnit,
                                        stlInt64            * aLength,
                                        dtlFuncVector       * aVectorFunc,
                                        void                * aVectorArg,
                                        stlErrorStack       * aErrorStack );

typedef stlStatus (*dtdGetLengthFromStringFunc) ( stlInt64              aPrecision,
                                                  stlInt64              aScale,
                                                  dtlStringLengthUnit   aStringUnit,
                                                  stlChar             * aString,
                                                  stlInt64              aStringLength,
                                                  stlInt64            * aLength,
                                                  dtlFuncVector       * aVectorFunc,
                                                  void                * aVectorArg,
                                                  stlErrorStack       * aErrorStack );

typedef stlStatus (*dtdGetLengthFromIntegerFunc) ( stlInt64          aPrecision,
                                                   stlInt64          aScale,
                                                   stlInt64          aInteger, 
                                                   stlInt64        * aLength,
                                                   stlErrorStack   * aErrorStack );

typedef stlStatus (*dtdGetLengthFromRealFunc) ( stlInt64          aPrecision,
                                                stlInt64          aScale,
                                                stlFloat64        aReal, 
                                                stlInt64        * aLength,
                                                stlErrorStack   * aErrorStack );

typedef stlStatus (*dtdSetValueFromStringFunc) ( stlChar             * aString,
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
                                                 stlErrorStack       * aErrorStack );

typedef stlStatus (*dtdSetValueFromIntegerFunc) ( stlInt64               aInteger,
                                                  stlInt64               aPrecision,
                                                  stlInt64               aScale,
                                                  dtlStringLengthUnit    aStringUnit,
                                                  dtlIntervalIndicator   aIntervalIndicator,
                                                  stlInt64               aAvailableSize,
                                                  dtlDataValue         * aValue,
                                                  stlBool              * aSuccessWithInfo,
                                                  dtlFuncVector        * aVectorFunc,
                                                  void                 * aVectorArg,
                                                  stlErrorStack        * aErrorStack );

typedef stlStatus (*dtdSetValueFromRealFunc) ( stlFloat64             aReal,
                                               stlInt64               aPrecision,
                                               stlInt64               aScale,
                                               dtlStringLengthUnit    aStringUnit,
                                               dtlIntervalIndicator   aIntervalIndicator,
                                               stlInt64               aAvailableSize,
                                               dtlDataValue         * aValue,
                                               stlBool              * aSuccessWithInfo,
                                               dtlFuncVector        * aVectorFunc,
                                               void                 * aVectorArg,
                                               stlErrorStack        * aErrorStack );

typedef stlStatus (*dtdReverseByteOrderFunc) ( void            * aValue,
                                               stlBool           aIsSameEndian,
                                               stlErrorStack   * aErrorStack );

typedef stlStatus (*dtdToStringFunc) ( dtlDataValue    * aValue,
                                       stlInt64          aPrecision,
                                       stlInt64          aScale,
                                       stlInt64          aAvailableSize,
                                       void            * aBuffer,
                                       stlInt64        * aLength,
                                       dtlFuncVector   * aVectorFunc,
                                       void            * aVectorArg,
                                       stlErrorStack   * aErrorStack );

typedef stlStatus (*dtdToStringForAvailableSizeFunc) ( dtlDataValue    * aValue,
                                                       stlInt64          aPrecision,
                                                       stlInt64          aScale,
                                                       stlInt64          aAvailableSize,
                                                       void            * aBuffer,
                                                       stlInt64        * aLength,
                                                       dtlFuncVector   * aVectorFunc,
                                                       void            * aVectorArg,
                                                       stlErrorStack   * aErrorStack );

typedef stlStatus (*dtdDateTimeSetValueFromTypedLiteralFunc)( stlChar             * aString,
                                                              stlInt64              aStringLength,
                                                              stlInt64              aAvailableSize,
                                                              dtlDataValue        * aValue,
                                                              dtlFuncVector       * aVectorFunc,
                                                              void                * aVectorArg,
                                                              stlErrorStack       * aErrorStack );

typedef stlStatus (*dtdSetValueFromWCharStringFunc) ( void                * aString,
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
                                                      stlErrorStack       * aErrorStack );

typedef stlStatus (*dtdToWCharStringFunc) ( dtlDataValue    * aValue,
                                            stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            stlInt64          aAvailableSize,
                                            void            * aBuffer,
                                            stlInt64        * aLength,
                                            dtlFuncVector   * aVectorFunc,
                                            void            * aVectorArg,
                                            stlErrorStack   * aErrorStack );

typedef stlStatus (*dtdGetWCharStringFromBinaryStringFunc) ( stlInt8         * aNumber,
                                                             stlInt64          aNumberLength,
                                                             stlInt64          aAvailableSize,
                                                             void            * aHex,
                                                             stlInt64        * aHexLength, 
                                                             stlErrorStack   * aErrorStack );


/*******************************************************************************
 * dtdCommon.c 
 ******************************************************************************/

stlBool dtdIsBinaryPadding( stlChar   * aValue,
                            stlInt64    aLength );

stlInt64 dtdGetLenTrimPadding( dtlDataValue * aValue );

void dtdReverseByteOrder( stlChar         * aSrcValue,
                          stlInt64          aLength,
                          stlChar         * aBuffer );

stlStatus dtdGetNumberFromHex( stlChar           aHex,
                               stlInt8         * aNumber,
                               stlErrorStack   * aErrorStack );

stlStatus dtdToStringFromBinaryString( stlInt8         * aNumber,
                                       stlInt64          aNumberLength,
                                       stlInt64          aAvailableSize,
                                       stlChar         * aHex,
                                       stlInt64        * aHexLength, 
                                       stlErrorStack   * aErrorStack );

stlStatus dtdToUtf16WCharStringFromBinaryString( stlInt8         * aNumber,
                                                 stlInt64          aNumberLength,
                                                 stlInt64          aAvailableSize,
                                                 void            * aHex,
                                                 stlInt64        * aHexLength, 
                                                 stlErrorStack   * aErrorStack );

stlStatus dtdToUtf32WCharStringFromBinaryString( stlInt8         * aNumber,
                                                 stlInt64          aNumberLength,
                                                 stlInt64          aAvailableSize,
                                                 void            * aHex,
                                                 stlInt64        * aHexLength, 
                                                 stlErrorStack   * aErrorStack );

stlStatus dtdToBinaryStringFromString( stlChar         * aHex,
                                       stlInt64          aHexLength,
                                       stlInt64          aAvailableSize,
                                       stlInt8         * aNumber,
                                       stlInt64        * aNumberLength, 
                                       stlErrorStack   * aErrorStack );

stlStatus dtdToBinaryStringFromUtf16WCharString( stlUInt16          * aHex,
                                                 stlInt64             aHexLength,
                                                 stlInt64             aAvailableSize,
                                                 stlInt8            * aNumber,
                                                 stlInt64           * aNumberLength,
                                                 stlErrorStack      * aErrorStack );

stlStatus dtdToBinaryStringFromUtf32WCharString( stlUInt32          * aHex,
                                                 stlInt64             aHexLength,
                                                 stlInt64             aAvailableSize,
                                                 stlInt8            * aNumber,
                                                 stlInt64           * aNumberLength,
                                                 stlErrorStack      * aErrorStack );

stlStatus dtdGetLengthOfDecodedHex( stlChar         * aHex,
                                    stlInt64          aHexLength,
                                    stlInt64        * aNumberLength, 
                                    stlErrorStack   * aErrorStack );

stlStatus dtdToNumericFromString( stlChar         * aString,
                                  stlInt64          aStringLength,
                                  stlInt64          aPrecision,
                                  stlInt64          aScale,
                                  dtlDataValue    * aNumeric,
                                  stlErrorStack   * aErrorStack );

stlStatus dtdToNumericFromUtf16WCharString( stlUInt16       * aString,
                                            stlInt64          aStringLength,
                                            stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            dtlDataValue    * aNumeric,
                                            stlErrorStack   * aErrorStack );

stlStatus dtdToNumericFromUtf32WCharString( stlUInt32       * aString,
                                            stlInt64          aStringLength,
                                            stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            dtlDataValue    * aNumeric,
                                            stlErrorStack   * aErrorStack );

stlStatus dtdToNumericFromInt64( stlInt64          aInteger,
                                 stlInt64          aPrecision,
                                 stlInt64          aScale,
                                 dtlDataValue    * aNumeric,
                                 stlErrorStack   * aErrorStack );

stlStatus dtdToNumericFromUInt64( stlUInt64         aUInt64,
                                  stlInt64          aPrecision,
                                  stlInt64          aScale,
                                  dtlDataValue    * aNumeric,
                                  stlErrorStack   * aErrorStack );

stlStatus dtdGetStringLengthFromFloat32( stlFloat32      * aFloat32,
                                         stlInt64        * aIntegralLength,
                                         stlInt64        * aEntirePrintLength,
                                         stlInt64        * aExpPrintLength,
                                         stlErrorStack   * aErrorStack );

stlStatus dtdGetStringLengthFromFloat64( stlFloat64      * aFloat64,
                                         stlInt64        * aIntegralLength,
                                         stlInt64        * aEntirePrintLength,
                                         stlInt64        * aExpPrintLength,
                                         stlErrorStack   * aErrorStack );

stlStatus dtdGetValidNumberLengthFromFloat32( stlFloat32      * aFloat32,
                                              stlInt64        * aValidNumberLength,
                                              stlErrorStack   * aErrorStack );

stlStatus dtdGetValidNumberLengthFromFloat64( stlFloat64      * aFloat64,
                                              stlInt64        * aValidNumberLength,
                                              stlErrorStack   * aErrorStack );

stlStatus dtdToNumericFromFloat32( stlFloat32        aFloat32,
                                   stlInt64          aPrecision,
                                   stlInt64          aScale,
                                   dtlDataValue    * aNumeric,
                                   stlErrorStack   * aErrorStack );

stlStatus dtdToNumericFromFloat64( stlFloat64        aFloat64,
                                   stlInt64          aPrecision,
                                   stlInt64          aScale,
                                   dtlDataValue    * aNumeric,
                                   stlErrorStack   * aErrorStack );

stlStatus dtdValidateNumericFromString( stlChar         * aString,
                                        stlInt64          aStringLength,
                                        stlInt64          aPrecision,
                                        stlErrorStack   * aErrorStack );

stlStatus dtdToStringFromNumeric( dtlDataValue    * aNumeric,
                                  stlInt64          aAvailableSize,
                                  void            * aBuffer,
                                  stlInt64        * aLength,
                                  stlErrorStack   * aErrorStack );

stlStatus dtdCastNumericToString( dtlDataValue    * aNumeric,
                                  stlInt64          aAvailableSize,
                                  void            * aBuffer,
                                  stlInt64        * aLength,
                                  stlErrorStack   * aErrorStack );

stlStatus dtdCastNumericToNonTruncateString( dtlDataValue    * aNumeric,
                                             stlInt64          aAvailableSize,
                                             void            * aBuffer,
                                             stlInt64        * aLength,
                                             stlErrorStack   * aErrorStack );

stlStatus dtdToInt64FromStringWithoutRound( stlChar         * aString,
                                            stlInt64          aStringLength,
                                            stlInt64        * aResult,
                                            stlErrorStack   * aErrorStack );

stlStatus dtdToInt64FromString( stlChar         * aString,
                                stlInt64          aStringLength,
                                stlInt64        * aResult,
                                stlErrorStack   * aErrorStack );

stlStatus dtdToUInt64FromString( stlChar         * aString,
                                 stlInt64          aStringLength,
                                 stlUInt64       * aResult,
                                 stlErrorStack   * aErrorStack );

stlStatus dtdToFloat32FromString( stlChar         * aString,
                                  stlInt64          aStringLength,
                                  stlFloat32      * aResult,
                                  stlErrorStack   * aErrorStack );
    
stlStatus dtdToFloat64FromString( stlChar         * aString,
                                  stlInt64          aStringLength,
                                  stlFloat64      * aResult,
                                  stlErrorStack   * aErrorStack );

void dtdInt64ToString( stlInt64        aValue,
                       stlChar       * aResult,
                       stlInt64      * aResultLength );

void dtdUInt64ToString( stlUInt64        aValue,
                        stlChar        * aResult,
                        stlInt64       * aResultLength );

stlStatus dtdResizeNumberStringToString( stlChar         * aNumberString,
                                         stlInt64          aNumberStringLength,
                                         stlInt64          aAvailableSize,
                                         stlChar         * aResult,
                                         stlInt64        * aResultLength,
                                         stlErrorStack   * aErrorStack );                                        

stlStatus dtdToFloat32FromNumeric( dtlDataValue    * aNumeric,
                                   stlFloat32      * aResult,
                                   stlErrorStack   * aErrorStack );

stlStatus dtdToFloat64FromNumeric( dtlDataValue    * aNumeric,
                                   stlFloat64      * aResult,
                                   stlErrorStack   * aErrorStack );


stlStatus dtdToStringFromDtlRowId( dtlRowIdType   * aRowIdType,
                                   stlInt64         aAvailableSize,
                                   stlChar        * aBuffer,
                                   stlInt64       * aLength,
                                   stlErrorStack  * aErrorStack );

stlStatus dtdToUtf16WCharStringFromDtlRowId( dtlRowIdType   * aRowIdType,
                                             stlInt64         aAvailableSize,
                                             stlUInt16      * aBuffer,
                                             stlInt64       * aLength,
                                             stlErrorStack  * aErrorStack );

stlStatus dtdToUtf32WCharStringFromDtlRowId( dtlRowIdType   * aRowIdType,
                                             stlInt64         aAvailableSize,
                                             stlUInt32      * aBuffer,
                                             stlInt64       * aLength,
                                             stlErrorStack  * aErrorStack );

stlStatus dtdCharStringToStringForAvailableSize( dtlDataValue    * aValue,
                                                 stlInt64          aAvailableSize,
                                                 void            * aBuffer,
                                                 stlInt64        * aLength,
                                                 dtlFuncVector   * aVectorFunc,
                                                 void            * aVectorArg,
                                                 stlErrorStack   * aErrorStack );

stlStatus dtdDateTimeToStringForAvailableSize( dtlDataValue    * aValue,
                                               stlInt64          aPrecision,
                                               stlInt64          aScale,
                                               stlInt64          aAvailableSize,
                                               void            * aBuffer,
                                               stlInt64        * aLength,
                                               dtlFuncVector   * aVectorFunc,
                                               void            * aVectorArg,
                                               stlErrorStack   * aErrorStack );

stlStatus dtdGetPrecisionForDestStringUnitLength( dtlDataType            aDataType,
                                                  stlChar              * aString,
                                                  stlInt64               aStringLength,
                                                  dtlStringLengthUnit    aStringUnit,
                                                  stlInt64             * aPrecForDestStrUnitLength,
                                                  dtlFuncVector        * aVectorFunc,
                                                  void                 * aVectorArg,
                                                  stlErrorStack        * aErrorStack );


/*******************************************************************************
 * dtdDateTimeCommon.c 
 ******************************************************************************/

stlStatus dtdJulianDate2Date( stlInt32         aJulianDate,
                              stlInt32       * aYear,
                              stlInt32       * aMonth,
                              stlInt32       * aDay,
                              stlErrorStack  * aErrorStack );

void dtdTimestamp2Time( dtlTimestampType      aTimestamp,
                        stlInt32            * aHour,
                        stlInt32            * aMinute,
                        stlInt32            * aSecond,
                        dtlFractionalSecond * aFractionalSecond );

stlStatus dtdDateTime2dtlExpTime( dtlTimeOffset           aDateTime,
                                  dtlExpTime            * aDtlExpTime,
                                  dtlFractionalSecond   * aFractionalSecond,
                                  stlErrorStack         * aErrorStack );

stlStatus dtdTimestampTz2dtlExpTime( dtlTimestampTzType      aTimestampTzType,
                                     stlInt32              * aTimeZone,
                                     dtlExpTime            * aDtlExpTime,
                                     dtlFractionalSecond   * aFractionalSecond,
                                     stlErrorStack         * aErrorStack );

stlInt32 dtdTrimTrailingZeros( stlChar  * aStr,
                               stlInt32   aLength );

stlStatus dtdAppendSeconds( stlChar             * aStr,
                            stlInt32            * aResultStrSize,
                            stlInt32              aSecond,
                            dtlFractionalSecond   aFractionalSecond,
                            stlInt32              aPrecision,
                            stlBool               aFillZeros,
                            stlErrorStack       * aErrorStack );

// /*
//  * @todo 임시코드임.
//  */ 
// stlInt32 dtdAppendSecondsForWithTimeZone( stlChar             * aStr,
//                                           stlInt32              aSecond,
//                                           dtlFractionalSecond   aFractionalSecond,
//                                           stlInt32              aPrecision,
//                                           stlBool               aFillZeros );  

// stlStatus dtdEncodeDateTime( dtlExpTime           * aDtlExpTime,
//                              dtlFractionalSecond    aFractionalSecond,
//                              stlInt32             * aTimeZone,
//                              stlInt32               aStyle,
//                              stlInt32               aDateOrder,
//                              stlChar              * aStr,
//                              stlInt32             * aLength,
//                              stlErrorStack        * aErrorStack );

stlStatus dtdParseDateTime( const stlChar  * aTimeStr,
                            stlInt64         aTimeStrLength,
                            dtlDataType      aDataType,
                            stlChar        * aBuffer,
                            stlSize          aBufferLen,
                            stlChar       ** aField,
                            stlInt32       * aFieldType,
                            stlInt32         aMaxFields,
                            stlInt32       * aNumFields,
                            stlErrorStack  * aErrorStack );

stlStatus dtdParseDateTimeFromUtf16WChar( const stlUInt16    * aTimeStr,
                                          stlInt64             aTimeStrLength,
                                          dtlDataType          aDataType,
                                          stlChar            * aBuffer,
                                          stlSize              aBufferLen,
                                          stlChar           ** aField,
                                          stlInt32           * aFieldType,
                                          stlInt32             aMaxFields,
                                          stlInt32           * aNumFields,
                                          stlErrorStack      * aErrorStack );

stlStatus dtdParseDateTimeFromUtf32WChar( const stlUInt32    * aTimeStr,
                                          stlInt64             aTimeStrLength,
                                          dtlDataType          aDataType,
                                          stlChar            * aBuffer,
                                          stlSize              aBufferLen,
                                          stlChar           ** aField,
                                          stlInt32           * aFieldType,
                                          stlInt32             aMaxFields,
                                          stlInt32           * aNumFields,
                                          stlErrorStack      * aErrorStack );

const dtlDateToken * dtdDateBinarySearch( const stlChar      * aKey,
                                          const dtlDateToken * aBase,
                                          stlInt32             aBaseSize );

// stlStatus dtdDecodeDateTime( const stlChar         * aString,
//                              dtlDataType             aDataType,
//                              stlChar              ** aField,
//                              stlInt32              * aFieldType,
//                              stlInt32                aNumberFields,
//                              stlInt32                aDateOrder,
//                              stlInt32              * aDateType,
//                              dtlExpTime            * aDtlExpTime,
//                              dtlFractionalSecond   * aFractionalSecond,
//                              stlInt32              * aTimeZone,
//                              dtlFuncVector         * aVectorFunc,
//                              void                  * aVectorArg,
//                              stlErrorStack         * aErrorStack );

// stlStatus dtdDecodeTimezone( stlChar       * aStr,
//                              stlInt32      * aTimeZone,
//                              stlErrorStack * aErrorStack );

stlInt32 dtdDate2JulianDate( stlInt32         aYear,
                             stlInt32         aMonth,
                             stlInt32         aDay );

// stlStatus dtdDecodeNumberField( stlInt32               aLen,
//                                 stlChar              * aStr,
//                                 stlInt32               aFieldMask,
//                                 stlInt32             * aTimeMask,
//                                 dtlExpTime           * aDtlExpTime,
//                                 dtlFractionalSecond  * aFractionalSecond,
//                                 stlBool              * aIsTwoDigits,
//                                 stlInt32             * aDateType,
//                                 stlErrorStack        * aErrorStack );

// stlStatus dtdDecodeDate( stlChar       * aStr,
//                          stlInt32        aFieldMask,
//                          stlInt32        aDateOrder,
//                          stlInt32      * aTimeMask,
//                          stlBool       * aIsTwoDigits,
//                          dtlExpTime    * aDtlExpTime,
//                          stlErrorStack * aErrorStack );

stlStatus  dtdDecodeSpecial( stlInt32        aField,
                             stlChar       * aLowToken,
                             stlInt32      * aValue,
                             stlInt32      * aValueType,
                             stlErrorStack * aErrorStack );

// stlStatus dtdDecodeNumber( stlInt32               aFieldLen,
//                            stlChar              * aStr,
//                            stlBool                aHaveTextMonth,
//                            stlInt32               aFieldMask,
//                            stlInt32               aDateOrder,
//                            stlInt32             * aTimeMask,
//                            dtlExpTime           * aDtlExpTime,
//                            dtlFractionalSecond  * aFractionalSecond,
//                            stlBool              * aIsTwoDigits,
//                            stlErrorStack        * aErrorStack );

stlStatus dtdParseFractionalSecond( stlChar             * aBuffer,
                                    dtlFractionalSecond * aFractionalSecond,
                                    stlErrorStack       * aErrorStack );

stlStatus dtdDecodeTime( stlChar              * aStr,
                         stlInt32               aFieldMask,
                         dtlIntervalIndicator   aRange,
                         stlInt32             * aTimeMask,
                         dtlExpTime           * aDtlExpTime,
                         dtlFractionalSecond  * aFractionalSecond,
                         stlErrorStack        * aErrorStack );

// stlStatus dtdValidateDate( stlInt32         aFieldMask,
//                            stlBool          aIsJulian,
//                            stlBool          aIsTwoDigits,
//                            stlBool          aIsBc,
//                            dtlExpTime     * aDtlExpTime,
//                            stlErrorStack  * aErrorStack );

// stlStatus dtdEncodeDateOnly( dtlExpTime    * aDtlExpTime,
//                              stlInt32        aDateStyle,
//                              stlInt32        aDateOrder,
//                              stlChar       * aStr,
//                              stlInt32      * aLength,
//                              stlErrorStack * aErrorStack );

// stlStatus dtdDecodeTimeOnly( const stlChar       * aTimeStr,
//                              dtlDataType           aDataType,
//                              stlChar            ** aField,
//                              stlInt32            * aFieldType,
//                              stlInt32              aNumFields,
//                              stlInt32              aDateOrder,
//                              stlInt32            * aDateType,
//                              dtlExpTime          * aDtlExpTime,
//                              dtlFractionalSecond * aFractionalSecond,
//                              stlInt32            * aTimeZone,
//                              dtlFuncVector       * aVectorFunc,
//                              void                * aVectorArg,
//                              stlErrorStack       * aErrorStack );

void dtdDtlExpTime2Time( dtlExpTime          * aDtlExpTime,
                         dtlFractionalSecond   aFractionalSecond,
                         dtlTimeType         * aTimeType );

stlStatus dtdAdjustTime( dtlTimeType   * aTimeType,
                         stlInt32        aTimePrecision,
                         stlErrorStack * aErrorStack );

void dtdTime2dtlExpTime( dtlTimeType           aTimeType,
                         dtlExpTime          * aDtlExpTime,
                         dtlFractionalSecond * aFractionalSecond );

void dtdDtlExpTime2TimeTz( dtlExpTime          * aDtlExpTime,
                           dtlFractionalSecond   aFractionalSecond,
                           stlInt32              aTimeZone,
                           dtlTimeTzType       * aTimeTzType );

void dtdTimeTz2dtlExpTime( dtlTimeTzType        * aTimeTzType,
                           dtlExpTime           * aDtlExpTime,
                           dtlFractionalSecond  * aFractionalSecond,
                           stlInt32             * aTimeZone );

stlStatus dtdDtlExpTime2DateTimeOffset( dtlExpTime          * aDtlExpTime,
                                        dtlFractionalSecond   aFractionalSecond,
                                        dtlTimeOffset       * aDateTimeOffset,
                                        stlErrorStack       * aErrorStack );

stlStatus dtdDtlExpTime2TimestampTz( const stlChar       * aStr,
                                     dtlExpTime          * aDtlExpTime,
                                     dtlFractionalSecond   aFractionalSecond,
                                     stlInt32            * aTimeZone,
                                     dtlTimestampTzType  * aTimestampTzType,
                                     stlErrorStack       * aErrorStack );

void dtdTime2TimeOffset( stlInt32              aHour,
                         stlInt32              aMinute,
                         stlInt32              aSecond,
                         dtlFractionalSecond   aFractionalSecond,
                         dtlTimeOffset       * aTimeOffset );

void dtdTimestamp2Local( dtlTimestampType   aTimestampType,
                         stlInt32           aTimeZone,
                         dtlTimestampType * aLocalTime );

void dtdTime2Local( dtlTimeType        aTimeType,
                    stlInt32           aTimeZone,
                    dtlTimeType      * aLocalTime );

stlStatus dtdAdjustDate( dtlDateType      * aDateType,
                         stlErrorStack    * aErrorStack );

stlStatus dtdAdjustTimestamp( dtlTimestampType * aTimestampType,
                              stlInt32           aTimestampPrecision,
                              stlErrorStack    * aErrorStack );

void dtdIntervalYearToMonth2DtlExpTime( dtlIntervalYearToMonthType   aIntervalYearToMonthType,
                                        dtlExpTime                 * aDtlExpTime );

void dtdIntervalDayToSecond2DtlExpTime( dtlIntervalDayToSecondType   aIntervalDayToSecondType,
                                        dtlExpTime                 * aDtlExpTime,
                                        dtlFractionalSecond        * aFractionalSecond );

stlStatus dtdDecodeInterval( stlChar             ** aField,
                             stlInt32             * aFieldType,
                             stlInt32               aNumField,
                             dtlIntervalIndicator   aIntervalIndicator,
                             stlInt32             * aDateType,
                             dtlExpTime           * aDtlExpTime,
                             dtlFractionalSecond  * aFractionalSecond,
                             stlErrorStack        * aErrorStack );

stlStatus dtdIntervalValueTruncatedCheck( dtlExpTime           * aDtlExpTime,
                                          dtlIntervalIndicator   aIntervalIndicator,
                                          dtlFractionalSecond  * aFractionalSecond,
                                          stlErrorStack        * aErrorStack );

void dtdAdjustFractSecond( stlFloat64            aFrac,
                           dtlExpTime          * aDtlExpTime,
                           dtlFractionalSecond * aFractionalSecond,
                           stlInt32              aScale );

void dtdAdjustFractDays( stlFloat64            aFrac,
                         dtlExpTime          * aDtlExpTime,
                         dtlFractionalSecond * aFractionalSecond,
                         stlInt32              aScale );

stlInt32 dtdDecodeUnits( stlInt32   aField,
                         stlChar  * aLowToken,
                         stlInt32 * aValue );

// stlStatus dtdDecodeISO8601Interval( stlChar             * aStr,
//                                     stlInt32            * aDateType,
//                                     dtlExpTime          * aDtlExpTime,
//                                     dtlFractionalSecond * aFractionalSecond,
//                                     stlErrorStack       * aErrorStack );

// stlStatus dtdParseISO8601Number( stlChar        * aStr,
//                                  stlChar       ** aEndPtr,
//                                  stlInt32       * aIntegerPart,
//                                  stlFloat64     * aFractionalPart,
//                                  stlErrorStack  * aErrorStack );

// stlInt32 dtdISO8601IntegerWidth( stlChar * aFieldStart );

void dtdDtlExpTime2IntervalYearToMonth( dtlExpTime                 * aDtlExpTime,
                                        dtlIntervalIndicator         aIntervalIndicator,
                                        dtlIntervalYearToMonthType * aIntervalYearToMonthType );

void dtdDtlExpTime2IntervalDayToSecond( dtlExpTime                 * aDtlExpTime,
                                        dtlFractionalSecond          aFractionalSecond,
                                        dtlIntervalIndicator         aIntervalIndicator,
                                        dtlIntervalDayToSecondType * aIntervalDayToSecondType );

stlStatus dtdAdjustIntervalYearToMonth( dtlIntervalYearToMonthType * aIntervalYearToMonthType,
                                        dtlIntervalIndicator         aIntervalIndicator,
                                        stlInt32                     aLeadingPrecision,
                                        stlErrorStack              * aErrorStack );

stlStatus dtdAdjustIntervalDayToSecond( dtlIntervalDayToSecondType * aIntervalDayToSecondType,
                                        dtlIntervalIndicator         aIntervalIndicator,
                                        stlInt32                     aLeadingPrecision,
                                        stlInt32                     aFractionalSecondPrecision,
                                        stlErrorStack              * aErrorStack );

stlStatus dtdTimestamp2TimestampTz( dtlTimestampType     aTimestampType,
                                    dtlTimestampTzType * aTimestampTzType,
                                    dtlFuncVector      * aVectorFunc,
                                    void               * aVectorArg,
                                    stlErrorStack      * aErrorStack );

stlStatus dtdDateToTimestamp( dtlDateType        aDateType,
                              dtlTimestampType * aTimestampType,
                              dtlFuncVector    * aVectorFunc,
                              void             * aVectorArg,
                              stlErrorStack    * aErrorStack );

stlStatus dtdDateToTimestampTz( dtlDateType          aDateType,
                                dtlTimestampTzType * aTimestampTzType,
                                dtlFuncVector      * aVectorFunc,
                                void               * aVectorArg,
                                stlErrorStack      * aErrorStack );

stlStatus dtdIntegerToLeadingZeroFormatString( stlInt32         aValue,
                                               stlInt32         aSize,
                                               stlInt32       * aResultStrSize,
                                               stlChar        * aStr,
                                               stlErrorStack  * aErrorStack );

stlStatus dtdIntegerToLeadingZeroFormatUtf16String( stlInt32         aValue,
                                                    stlInt32         aSize,
                                                    stlInt32       * aResultStrSize,
                                                    stlUInt16      * aStr,
                                                    stlErrorStack  * aErrorStack );

stlStatus dtdIntegerToLeadingZeroFormatUtf32String( stlInt32         aValue,
                                                    stlInt32         aSize,
                                                    stlInt32       * aResultStrSize,
                                                    stlUInt32      * aStr,
                                                    stlErrorStack  * aErrorStack );


// 제거될 함수 
stlStatus dtdGetPropertyIntervalStyle( stlInt64      * aValue,
                                       stlErrorStack * aErrorStack );


stlStatus dtdDateSetValueFromStlTimeAndTimeZone( stlTime           aStlTime,
                                                 stlInt32          aTimeZoneOffset,
                                                 dtlDataValue    * aValue,
                                                 stlErrorStack   * aErrorStack );

stlStatus dtdTimeTzSetValueFromStlTimeAndTimeZone( stlTime           aStlTime,
                                                   stlInt32          aTimeZoneOffset,
                                                   stlInt64          aPrecision,
                                                   dtlDataValue    * aValue,
                                                   stlErrorStack   * aErrorStack );

stlStatus dtdTimestampTzSetValueFromStlTimeAndTimeZone( stlTime           aStlTime,
                                                        stlInt32          aTimeZoneOffset,
                                                        stlInt64          aPrecision,
                                                        dtlDataValue    * aValue,
                                                        stlErrorStack   * aErrorStack );

stlStatus dtdTimestampTzToDate( dtlTimestampTzType  * aTimestampTz,
                                dtlDateType         * aDate,
                                stlErrorStack       * aErrorStack );

stlStatus dtdTimestampTzToTimeTz( dtlTimestampTzType  * aTimestampTz,
                                  stlInt64              aPrecision,
                                  dtlTimeTzType       * aTimeTz,
                                  stlErrorStack       * aErrorStack );

stlStatus dtdTimestampTzToTime( dtlTimestampTzType  * aTimestampTz,
                                stlInt64              aPrecision,
                                dtlTimeType         * aTime,
                                stlErrorStack       * aErrorStack );

stlStatus dtdTimestampTzToTimestamp( dtlTimestampTzType  * aTimestampTz,
                                     stlInt64              aPrecision,
                                     dtlTimestampType    * aTimestamp,
                                     stlErrorStack       * aErrorStack );


/*******************************************************************************
 * dtdDataType.c 
 ******************************************************************************/

extern dtdGetLengthFunc dtdGetLengthFuncList[ DTL_TYPE_MAX ];

extern dtdGetLengthFromStringFunc dtdGetLengthFromStringFuncList[ DTL_TYPE_MAX ];

extern dtdGetLengthFromIntegerFunc dtdGetLengthFromIntegerFuncList[ DTL_TYPE_MAX ];

extern dtdGetLengthFromRealFunc dtdGetLengthFromRealFuncList[ DTL_TYPE_MAX ];

extern dtdSetValueFromStringFunc dtdSetValueFromStringFuncList[ DTL_TYPE_MAX ];

extern dtdSetValueFromIntegerFunc dtdSetValueFromIntegerFuncList[ DTL_TYPE_MAX ];

extern dtdSetValueFromRealFunc dtdSetValueFromRealFuncList[ DTL_TYPE_MAX ];

extern dtdReverseByteOrderFunc dtdReverseByteOrderFuncList[ DTL_TYPE_MAX ];

extern dtdToStringFunc dtdToStringFuncList[ DTL_TYPE_MAX ];

extern dtdToStringForAvailableSizeFunc dtdToStringForAvailableSizeFuncList[ DTL_TYPE_MAX ];

extern dtdDateTimeSetValueFromTypedLiteralFunc dtdDateTimeSetValueFromTypedLiteralFuncList[ DTL_TYPE_MAX ];

extern dtdSetValueFromWCharStringFunc dtdSetValueFromWCharStringFuncList[ DTL_TYPE_MAX ][ DTL_UNICODE_MAX ];

extern dtdToWCharStringFunc dtdToWCharStringFuncList[ DTL_TYPE_MAX ][ DTL_UNICODE_MAX ];

extern dtdGetWCharStringFromBinaryStringFunc dtdGetWCharStringFromBinaryStringFuncList[ DTL_UNICODE_MAX ];


/** @} */

#endif /* _DTDDATATYPE_H_ */

