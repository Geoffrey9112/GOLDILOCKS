/*******************************************************************************
 * dtlDataValue.h
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
#ifndef _DTL_DATA_VALUE_H_
#define _DTL_DATA_VALUE_H_ 1


/**
 * @file dtlDataValue.h
 * @brief DataType Layer Data Value 제어 함수 
 */

/**
 * @addtogroup dtlDataValueControl
 * @{
 */

stlStatus dtlGetDataValueBufferSize( dtlDataType           aType,
                                     stlInt64              aPrecision,
                                     dtlStringLengthUnit   aStringUnit,
                                     stlInt64            * aLength,
                                     dtlFuncVector       * aVectorFunc,
                                     void                * aVectorArg,
                                     stlErrorStack       * aErrorStack );

stlStatus dtlGetDataValueBufferSizeFromString( dtlDataType           aType,
                                               stlInt64              aPrecision,
                                               stlInt64              aScale,
                                               dtlStringLengthUnit   aStringUnit,
                                               stlChar             * aString,
                                               stlInt64              aStringLength,
                                               stlInt64            * aLength,
                                               dtlFuncVector       * aVectorFunc,
                                               void                * aVectorArg,
                                               stlErrorStack       * aErrorStack );

stlStatus dtlGetDataValueBufferSizeFromInteger( dtlDataType       aType,
                                                stlInt64          aPrecision,
                                                stlInt64          aScale,
                                                stlInt64          aInteger, 
                                                stlInt64        * aLength,
                                                stlErrorStack   * aErrorStack );

stlStatus dtlGetDataValueBufferSizeFromReal( dtlDataType       aType,
                                             stlInt64          aPrecision,
                                             stlInt64          aScale,
                                             stlFloat64        aReal, 
                                             stlInt64        * aLength,
                                             stlErrorStack   * aErrorStack );

stlStatus dtlInitDataValue( dtlDataType       aType,
                            stlInt32          aBufferSize,
                            dtlDataValue    * aValue,
                            stlErrorStack   * aErrorStack );

stlStatus dtlSetValueFromString( dtlDataType           aType,
                                 stlChar             * aString,
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

stlStatus dtlSetValueFromInteger( dtlDataType            aType,
                                  stlInt64               aInteger,
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

stlStatus dtlSetValueFromReal( dtlDataType            aType,
                               stlFloat64             aReal,
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

stlStatus dtlSetDateTimeValueFromTypedLiteral( dtlDataType           aType,
                                               stlChar             * aString,
                                               stlInt64              aStringLength,
                                               stlInt64              aAvailableSize,
                                               dtlDataValue        * aValue,
                                               dtlFuncVector       * aVectorFunc,
                                               void                * aVectorArg,
                                               stlErrorStack       * aErrorStack );

stlStatus dtlReverseByteOrderValue( dtlDataType       aType,
                                    void            * aValue,
                                    stlBool           aIsSameEndian,
                                    stlErrorStack   * aErrorStack );

stlStatus dtlToStringDataValue( dtlDataValue    * aValue,
                                stlInt64          aPrecision,
                                stlInt64          aScale,
                                stlInt64          aAvailableSize,
                                void            * aBuffer,
                                stlInt64        * aLength,
                                dtlFuncVector   * aVectorFunc,
                                void            * aVectorArg,
                                stlErrorStack   * aErrorStack );

stlStatus dtlToStringForAvailableSizeDataValue( dtlDataValue    * aValue,
                                                stlInt64          aPrecision,
                                                stlInt64          aScale,
                                                stlInt64          aAvailableSize,
                                                void            * aBuffer,
                                                stlInt64        * aLength,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack );

stlStatus dtlToWCharStringDataValue( dtlUnicodeEncoding    aUnicodeEncoding,
                                     dtlDataValue        * aValue,
                                     stlInt64              aPrecision,
                                     stlInt64              aScale,
                                     stlInt64              aAvailableSize,
                                     void                * aBuffer,
                                     stlInt64            * aLength,
                                     dtlFuncVector       * aVectorFunc,
                                     void                * aVectorArg,
                                     stlErrorStack       * aErrorStack );
    
stlStatus dtlToBinaryStringFromString( stlChar         * aHex,
                                       stlInt64          aHexLength,
                                       stlInt64          aAvailableSize,
                                       stlInt8         * aNumber,
                                       stlInt64        * aNumberLength, 
                                       stlErrorStack   * aErrorStack );

stlStatus dtlResizeNumberStringToString( stlChar         * aNumberString,
                                         stlInt64          aNumberStringLength,
                                         stlInt64          aAvailableSize,
                                         stlChar         * aResult,
                                         stlInt64        * aResultLength,
                                         stlErrorStack   * aErrorStack );

stlStatus dtlGetStringLengthFromFloat32( stlFloat32      * aFloat32,
                                         stlInt64        * aIntegralLength,
                                         stlInt64        * aEntirePrintLength,
                                         stlInt64        * aExpPrintLength,
                                         stlErrorStack   * aErrorStack );

stlStatus dtlGetStringLengthFromFloat64( stlFloat64      * aFloat64,
                                         stlInt64        * aIntegralLength,
                                         stlInt64        * aEntirePrintLength,
                                         stlInt64        * aExpPrintLength,
                                         stlErrorStack   * aErrorStack );

stlStatus dtlGetValidNumberLengthFromFloat32( stlFloat32      * aFloat32,
                                              stlInt64        * aValidNumberLength,
                                              stlErrorStack   * aErrorStack );

stlStatus dtlGetValidNumberLengthFromFloat64( stlFloat64      * aFloat64,
                                              stlInt64        * aValidNumberLength,
                                              stlErrorStack   * aErrorStack );

stlStatus dtlGetIntegerFromString( stlChar        * aString,
                                   stlInt32         aStringLength,
                                   stlInt64       * aInteger,
                                   stlErrorStack  * aErrorStack );

stlStatus dtlGetNumericFromString( stlChar         * aString,
                                   stlInt64          aStringLength,
                                   stlInt64          aPrecision,
                                   stlInt64          aScale,
                                   dtlDataValue    * aNumeric,
                                   stlErrorStack   * aErrorStack );

stlStatus dtlGetIntegerFromStringWithoutRound( stlChar        * aString,
                                               stlInt32         aStringLength,
                                               stlInt64       * aInteger,
                                               stlErrorStack  * aErrorStack );

stlStatus dtlValidateBoolean( stlInt64         aInteger,
                              stlBool        * aIsValid,
                              stlErrorStack  * aErrorStack );

void dtlValidateFloat( stlChar        * aString,
                       stlBool        * aIsValid,
                       stlErrorStack  * aErrorStack );

stlStatus dtlValidateBitString( stlChar        * aBitString,
                                stlBool        * aIsValid,
                                stlInt64       * aPrecision,
                                stlErrorStack  * aErrorStack );

stlStatus dtlGetTimeInfoFromDate( dtlDateType   * aType,
                                  stlInt32      * aYear,
                                  stlInt32      * aMonth,
                                  stlInt32      * aDay,
                                  stlInt32      * aHour,
                                  stlInt32      * aMinute,
                                  stlInt32      * aSecond,                                  
                                  stlErrorStack * aErrorStack );

stlStatus dtlGetTimeInfoFromTime( dtlTimeType   * aType,
                                  stlInt32      * aHour,
                                  stlInt32      * aMinute,
                                  stlInt32      * aSecond,
                                  stlInt32      * aFraction,
                                  stlErrorStack * aErrorStack );

stlStatus dtlGetTimeInfoFromTimeTz( dtlTimeTzType   * aType,
                                    stlInt32        * aHour,
                                    stlInt32        * aMinute,
                                    stlInt32        * aSecond,
                                    stlInt32        * aFraction,
                                    stlInt32        * aTimeZoneSign,
                                    stlUInt32       * aTimeZoneHour,
                                    stlUInt32       * aTimeZoneMinute,
                                    stlErrorStack   * aErrorStack );

stlStatus dtlGetTimeInfoFromTimestamp( dtlTimestampType * aType,
                                       stlInt32         * aYear,
                                       stlInt32         * aMonth,
                                       stlInt32         * aDay,
                                       stlInt32         * aHour,
                                       stlInt32         * aMinute,
                                       stlInt32         * aSecond,
                                       stlInt32         * aFraction,
                                       stlErrorStack    * aErrorStack );

stlStatus dtlGetTimeInfoFromTimestampTz( dtlTimestampTzType * aType,
                                         stlInt32           * aYear,
                                         stlInt32           * aMonth,
                                         stlInt32           * aDay,
                                         stlInt32           * aHour,
                                         stlInt32           * aMinute,
                                         stlInt32           * aSecond,
                                         stlInt32           * aFraction,
                                         stlInt32           * aTimeZoneSign,
                                         stlUInt32          * aTimeZoneHour,
                                         stlUInt32          * aTimeZoneMinute,
                                         stlErrorStack      * aErrorStack );

stlStatus dtlGetTimeInfoFromYearMonthInterval( dtlIntervalYearToMonthType * aType,
                                               stlUInt32                  * aYear,
                                               stlUInt32                  * aMonth,
                                               stlBool                    * aIsNegative,
                                               stlErrorStack              * aErrorStack );

stlStatus dtlGetTimeInfoFromDaySecondInterval( dtlIntervalDayToSecondType * aType,
                                               stlUInt32                  * aDay,
                                               stlUInt32                  * aHour,
                                               stlUInt32                  * aMinute,
                                               stlUInt32                  * aSecond,
                                               stlUInt32                  * aFraction,
                                               stlBool                    * aIsNegative,
                                               stlErrorStack              * aErrorStack );

stlStatus dtlGetIntervalSecondInfoFromDtlNumericValue(
    dtlDataValue         * aNumericType,
    dtlIntervalIndicator   aIntervalIndicator,
    stlInt64               aLeadingPrecision,
    stlInt64               aSecondFractionalPrecision,
    stlUInt32            * aSecond,
    stlUInt32            * aFractionalSecond,
    stlBool              * aIsNegative,
    stlBool              * aSuccessWithInfo,
    stlErrorStack        * aErrorStack );

stlStatus dtlGetStringFromBinaryString( stlInt8         * aNumber,
                                        stlInt64          aNumberLength,
                                        stlInt64          aAvailableSize,
                                        stlChar         * aHex,
                                        stlInt64        * aHexLength, 
                                        stlErrorStack   * aErrorStack );

stlStatus dtlGetWCharStringFromBinaryString( dtlUnicodeEncoding   aUniEncoding,
                                             stlInt8            * aNumber,
                                             stlInt64             aNumberLength,
                                             stlInt64             aAvailableSize,
                                             void               * aHex,
                                             stlInt64           * aHexLength, 
                                             stlErrorStack      * aErrorStack );

stlStatus dtlAdjustTime( dtlTimeType   * aTimeType,
                         stlInt32        aTimePrecision,
                         stlErrorStack * aErrorStack );

stlStatus dtlAdjustTimeTz( dtlTimeTzType   * aTimeTzType,
                           stlInt32          aTimeTzPrecision,
                           stlErrorStack   * aErrorStack );

stlStatus dtlAdjustTimestamp( dtlTimestampType * aTimestampType,
                              stlInt32           aTimestampPrecision,
                              stlErrorStack    * aErrorStack );

stlStatus dtlAdjustTimestampTz( dtlTimestampTzType * aTimestampTzType,
                                stlInt32             aTimestampTzPrecision,
                                stlErrorStack      * aErrorStack );

stlStatus dtlAdjustIntervalYearToMonth( dtlIntervalIndicator   aIntervalIndicator,
                                        stlInt32               aLeadingPrecision,
                                        stlUInt32              aYear,
                                        stlUInt32              aMonth,
                                        stlBool              * aSuccessWithInfo,
                                        stlErrorStack        * aErrorStack );

stlStatus dtlAdjustIntervalDayToSecond( dtlIntervalIndicator   aIntervalIndicator,
                                        stlInt32               aLeadingPrecision,
                                        stlInt32               aFractionalSecondPrecision,
                                        stlUInt32              aDay,
                                        stlUInt32              aHour,
                                        stlUInt32              aMinute,
                                        stlUInt32              aSecond,
                                        stlUInt32            * aFractionalSecond,
                                        stlBool              * aSuccessWithInfo,
                                        stlErrorStack        * aErrorStack );

stlStatus dtlInt64ToNumeric( stlInt64          aInt64,
                             dtlDataValue    * aResult,
                             stlErrorStack   * aErrorStack );

stlStatus dtlNumericToInt64( dtlDataValue    * aNumericType,
                             stlInt64        * aResult,
                             stlBool         * aIsTruncated,
                             stlErrorStack   * aErrorStack );

stlStatus dtlNumericToUInt64( dtlDataValue    * aNumericType,
                              stlUInt64       * aResult,
                              stlBool         * aIsTruncated,
                              stlErrorStack   * aErrorStack );

stlStatus dtlNumericToFloat32( dtlDataValue    * aNumericType,
                               stlFloat32      * aResult,
                               stlErrorStack   * aErrorStack );

stlStatus dtlNumericToFloat64( dtlDataValue    * aNumericType,
                               stlFloat64      * aResult,
                               stlErrorStack   * aErrorStack );

stlBool dtlIsValidateDate( stlInt32        aYear,
                           stlInt32        aMonth,
                           stlInt32        aDay );

stlBool dtlIsValidateTime( stlInt32        aHour,
                           stlInt32        aMinute,
                           stlInt32        aSecond );

stlBool dtlIsValidateTimestamp( stlInt32         aYear,
                                stlInt32         aMonth,
                                stlInt32         aDay,
                                stlInt32         aHour,
                                stlInt32         aMinute,
                                stlInt32         aSecond,
                                stlInt32         aFraction );

stlBool dtlIsValidateTimeZone( stlInt32  aTimeZoneHour,
                               stlInt32  aTimeZoneMinute );

stlStatus dtlDateSetValueFromStlTime( stlTime           aStlTime,
                                      stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlInt64          aAvailableSize,
                                      dtlDataValue    * aValue,
                                      dtlFuncVector   * aVectorFunc,
                                      void            * aVectorArg,
                                      stlErrorStack   * aErrorStack );

stlStatus dtlTimeSetValueFromStlTime( stlTime           aStlTime,
                                      stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlInt64          aAvailableSize,
                                      dtlDataValue    * aValue,
                                      dtlFuncVector   * aVectorFunc,
                                      void            * aVectorArg,
                                      stlErrorStack   * aErrorStack );

stlStatus dtlTimeTzSetValueFromStlTime( stlTime           aStlTime,
                                        stlInt64          aPrecision,
                                        stlInt64          aScale,
                                        stlInt64          aAvailableSize,
                                        dtlDataValue    * aValue,
                                        dtlFuncVector   * aVectorFunc,
                                        void            * aVectorArg,
                                        stlErrorStack   * aErrorStack );

stlStatus dtlTimestampSetValueFromStlTime( stlTime           aStlTime,
                                           stlInt64          aPrecision,
                                           stlInt64          aScale,
                                           stlInt64          aAvailableSize,
                                           dtlDataValue    * aValue,
                                           dtlFuncVector   * aVectorFunc,
                                           void            * aVectorArg,
                                           stlErrorStack   * aErrorStack );

stlStatus dtlTimestampTzSetValueFromStlTime( stlTime           aStlTime,
                                             stlInt64          aPrecision,
                                             stlInt64          aScale,
                                             stlInt64          aAvailableSize,
                                             dtlDataValue    * aValue,
                                             dtlFuncVector   * aVectorFunc,
                                             void            * aVectorArg,
                                             stlErrorStack   * aErrorStack );

stlStatus dtlSysTimestampTzSetValueFromStlTime( stlTime           aStlTime,
                                                stlInt64          aPrecision,
                                                stlInt64          aScale,
                                                stlInt64          aAvailableSize,
                                                dtlDataValue    * aValue,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack );

stlStatus dtlIntervalYearToMonthSetValueFromTimeInfo(
    stlUInt32                    aYear,
    stlUInt32                    aMonth,
    stlBool                      aIsNegative,
    stlInt64                     aLeadingPrecision,
    dtlIntervalIndicator         aIntervalIndicator,
    stlInt64                     aAvailableSize,
    dtlDataValue               * aValue,
    stlBool                    * aSuccessWithInfo,    
    stlErrorStack              * aErrorStack );

stlStatus dtlIntervalDayToSecondSetValueFromTimeInfo(
    stlUInt32                    aDay,
    stlUInt32                    aHour,
    stlUInt32                    aMinute,
    stlUInt32                    aSecond,
    stlUInt32                    aFractionalSecond,
    stlBool                      aIsNegative,
    stlInt64                     aLeadingPrecision,
    stlInt64                     aFractionalSecondPrecision,
    dtlIntervalIndicator         aIntervalIndicator,
    stlInt64                     aAvailableSize,
    dtlDataValue               * aValue,
    stlBool                    * aSuccessWithInfo,
    stlErrorStack              * aErrorStack );

stlStatus dtlCharSetValueFromUInt64IntegerWithoutPaddNull( stlUInt64               aUInt64Value,
                                                           stlInt64                aPrecision,
                                                           stlInt64                aScale,
                                                           dtlStringLengthUnit     aStringUnit,
                                                           dtlIntervalIndicator    aIntervalIndicator,
                                                           stlInt64                aAvailableSize,
                                                           dtlDataValue          * aValue,
                                                           stlBool               * aSuccessWithInfo,
                                                           dtlFuncVector         * aVectorFunc,
                                                           void                  * aVectorArg,
                                                           stlErrorStack         * aErrorStack );

stlStatus dtlVarcharSetValueFromUInt64Integer( stlUInt64              aUInt64Value,
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

stlStatus dtlLongvarcharSetValueFromUInt64Integer( stlUInt64              aUInt64Value,
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

stlStatus dtlNumericSetValueFromUInt64Integer( stlUInt64              aUInt64Value,
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

stlStatus dtlGetStringLengthFromNumeric( dtlDataValue    * aNumeric,
                                         stlInt64        * aValidNumberLength,
                                         stlInt64        * aEntirePrintLength,
                                         stlInt64        * aExpPrintLength,
                                         stlErrorStack   * aErrorStack );

stlStatus dtlGetValidNumberLengthFromNumeric( dtlDataValue    * aNumeric,
                                              stlInt64        * aValidNumberLength,
                                              stlErrorStack   * aErrorStack );

stlStatus dtlIntervalYearToMonthSetNotTruncatedValueFromString(
    stlChar              * aString,
    stlInt64               aStringLength,
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

stlStatus dtlIntervalDayToSecondSetNotTruncatedValueFromString(
    stlChar              * aString,
    stlInt64               aStringLength,
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

stlStatus dtlRowIdSetValueFromRowIdInfo( stlInt64        aObjectId,
                                         stlUInt16       aTbsId,
                                         stlUInt32       aPageId,
                                         stlInt16        aRowNumber,
                                         dtlDataValue  * aValue,
                                         stlErrorStack * aErrorStack );

stlStatus dtlNumericMultiplication( dtlDataValue   * aValue1,
                                    dtlDataValue   * aValue2,
                                    dtlDataValue   * aResult,
                                    stlErrorStack  * aErrorStack );

stlStatus dtlNumAddition( dtlDataValue   * aValue1,
                          dtlDataValue   * aValue2,
                          dtlDataValue   * aResult,
                          stlErrorStack  * aErrorStack );

extern dtlNumericSumFunc gDtlNumericSum[2][2];

#define dtlNumericSum( aSumValue, aSrcValue, aErrorStack )                                          \
    (gDtlNumericSum[ DTL_IS_POSITIVE(aSumValue) ][ DTL_IS_POSITIVE(aSrcValue) ]) ( aSumValue,       \
                                                                                   aSrcValue,       \
                                                                                   aErrorStack )


stlStatus dtlIntervalYearToMonthToFormatString( dtlExpTime      * aDtlExpTime,
                                                stlInt64          aLeadingPrecision,
                                                stlChar         * aStr,
                                                stlInt64        * aLength,
                                                stlErrorStack   * aErrorStack );

stlStatus dtlIntervalYearToMonthToFormatUtf16String( dtlExpTime      * aDtlExpTime,
                                                     stlInt64          aLeadingPrecision,
                                                     stlUInt16       * aStr,
                                                     stlInt64        * aLength,
                                                     stlErrorStack   * aErrorStack );

stlStatus dtlIntervalYearToMonthToFormatUtf32String( dtlExpTime      * aDtlExpTime,
                                                     stlInt64          aLeadingPrecision,
                                                     stlUInt32       * aStr,
                                                     stlInt64        * aLength,
                                                     stlErrorStack   * aErrorStack );

stlStatus dtlIntervalDayToSecondToFormatString( dtlIntervalIndicator  aIntervalIndicator,
                                                dtlExpTime          * aDtlExpTime,
                                                dtlFractionalSecond   aFractionalSecond,
                                                stlInt64              aLeadingPrecision,
                                                stlInt64              aFractionalSecondPrecision,
                                                stlChar             * aStr,
                                                stlInt64            * aLength,
                                                stlErrorStack       * aErrorStack );

stlStatus dtlIntervalDayToSecondToFormatUtf16String( dtlIntervalIndicator  aIntervalIndicator,
                                                     dtlExpTime          * aDtlExpTime,
                                                     dtlFractionalSecond   aFractionalSecond,
                                                     stlInt64              aLeadingPrecision,
                                                     stlInt64              aFractionalSecondPrecision,
                                                     stlUInt16           * aStr,
                                                     stlInt64            * aLength,
                                                     stlErrorStack       * aErrorStack );

stlStatus dtlIntervalDayToSecondToFormatUtf32String( dtlIntervalIndicator  aIntervalIndicator,
                                                     dtlExpTime          * aDtlExpTime,
                                                     dtlFractionalSecond   aFractionalSecond,
                                                     stlInt64              aLeadingPrecision,
                                                     stlInt64              aFractionalSecondPrecision,
                                                     stlUInt32           * aStr,
                                                     stlInt64            * aLength,
                                                     stlErrorStack       * aErrorStack );

stlStatus dtlDateSetValueFromTimeInfo( stlInt32           aYear,
                                       stlInt32           aMonth,
                                       stlInt32           aDay,
                                       stlInt32           aHour,
                                       stlInt32           aMinute,
                                       stlInt32           aSecond,
                                       stlInt64           aAvailableSize,
                                       dtlDataValue     * aValue,
                                       dtlFuncVector    * aVectorFunc,
                                       void             * aVectorArg,
                                       stlErrorStack    * aErrorStack );

stlStatus dtlTimeSetValueFromTimeInfo( stlInt32           aHour,
                                       stlInt32           aMinute,
                                       stlInt32           aSecond,
                                       stlInt32           aFraction,
                                       stlInt64           aPrecision,
                                       stlInt64           aAvailableSize,
                                       dtlDataValue     * aValue,
                                       dtlFuncVector    * aVectorFunc,
                                       void             * aVectorArg,
                                       stlErrorStack    * aErrorStack );

stlStatus dtlTimeTzSetValueFromTimeInfo( stlInt32           aHour,
                                         stlInt32           aMinute,
                                         stlInt32           aSecond,
                                         stlInt32           aFraction,
                                         stlInt32           aTimeZoneHour,
                                         stlInt32           aTimeZoneMinute,
                                         stlInt64           aPrecision,
                                         stlInt64           aAvailableSize,
                                         dtlDataValue     * aValue,
                                         dtlFuncVector    * aVectorFunc,
                                         void             * aVectorArg,
                                         stlErrorStack    * aErrorStack );

stlStatus dtlTimestampSetValueFromTimeInfo( stlInt32           aYear,
                                            stlInt32           aMonth,
                                            stlInt32           aDay,
                                            stlInt32           aHour,
                                            stlInt32           aMinute,
                                            stlInt32           aSecond,
                                            stlInt32           aFraction,
                                            stlInt64           aPrecision,
                                            stlInt64           aAvailableSize,
                                            dtlDataValue     * aValue,
                                            dtlFuncVector    * aVectorFunc,
                                            void             * aVectorArg,
                                            stlErrorStack    * aErrorStack );

stlStatus dtlTimestampTzSetValueFromTimeInfo( stlInt32           aYear,
                                              stlInt32           aMonth,
                                              stlInt32           aDay,
                                              stlInt32           aHour,
                                              stlInt32           aMinute,
                                              stlInt32           aSecond,
                                              stlInt32           aFraction,
                                              stlInt32           aTimeZoneHour,
                                              stlInt32           aTimeZoneMinute,
                                              stlInt64           aPrecision,
                                              stlInt64           aAvailableSize,
                                              dtlDataValue     * aValue,
                                              dtlFuncVector    * aVectorFunc,
                                              void             * aVectorArg,
                                              stlErrorStack    * aErrorStack );

stlStatus dtlTimestampTzToDate( dtlTimestampTzType  * aTimestampTz,
                                dtlDateType         * aDate,
                                stlErrorStack       * aErrorStack );

stlStatus dtlTimestampTzToTimeTz( dtlTimestampTzType  * aTimestampTz,
                                  stlInt64              aPrecision,
                                  dtlTimeTzType       * aTimeTz,
                                  stlErrorStack       * aErrorStack );

stlStatus dtlTimestampTzToTime( dtlTimestampTzType  * aTimestampTz,
                                stlInt64              aPrecision,
                                dtlTimeType         * aTime,
                                stlErrorStack       * aErrorStack );

stlStatus dtlTimestampTzToTimestamp( dtlTimestampTzType  * aTimestampTz,
                                     stlInt64              aPrecision,
                                     dtlTimestampType    * aTimestamp,
                                     stlErrorStack       * aErrorStack );

stlStatus dtlFromTimestampToStlTime( dtlTimestampType * aTimestamp,
                                     stlTime          * aStlTime,
                                     dtlFuncVector    * aVectorFunc,
                                     void             * aVectorArg,
                                     stlErrorStack    * aErrorStack );

stlStatus dtlFuncStlTimeAddNumber( stlTime          aStlTime,
                                   dtlDataValue   * aNumber,
                                   stlTime        * aResultStlTime,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   stlErrorStack  * aErrorStack );

stlBool dtlIsValidDataValue( dtlDataValue * aDataValue );

/** @} dtlDataValueControl */


#endif /* _DTL_DATA_VALUE_H_ */
