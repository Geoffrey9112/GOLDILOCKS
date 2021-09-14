/*******************************************************************************
 * dtdNumeric.h
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


#ifndef _DTDNUMERIC_H_
#define _DTDNUMERIC_H_ 1

/**
 * @file dtdNumeric.h
 * @brief DataType Layer dtlNumeric Definition
 */

/**
 * @defgroup dtlNumeric dtlNumeric
 * @ingroup dtd
 * @{
 */

stlStatus dtdNumericGetLength( stlInt64              aPrecision,
                               dtlStringLengthUnit   aStringUnit,
                               stlInt64            * aLength,
                               dtlFuncVector       * aVectorFunc,
                               void                * aVectorArg,
                               stlErrorStack       * aErrorStack );

stlStatus dtdFloatGetLength( stlInt64              aPrecision,
                             dtlStringLengthUnit   aStringUnit,
                             stlInt64            * aLength,
                             dtlFuncVector       * aVectorFunc,
                             void                * aVectorArg,
                             stlErrorStack       * aErrorStack );

stlStatus dtdNumericGetLengthFromString( stlInt64              aPrecision,
                                         stlInt64              aScale,
                                         dtlStringLengthUnit   aStringUnit,
                                         stlChar             * aString,
                                         stlInt64              aStringLength,
                                         stlInt64            * aLength,
                                         dtlFuncVector       * aVectorFunc,
                                         void                * aVectorArg,
                                         stlErrorStack       * aErrorStack );

stlStatus dtdFloatGetLengthFromString( stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64            * aLength,
                                       dtlFuncVector       * aVectorFunc,
                                       void                * aVectorArg,
                                       stlErrorStack       * aErrorStack );

stlStatus dtdNumericGetLengthFromInteger( stlInt64          aPrecision,
                                          stlInt64          aScale,
                                          stlInt64          aInteger,
                                          stlInt64        * aLength,
                                          stlErrorStack   * aErrorStack );

stlStatus dtdFloatGetLengthFromInteger( stlInt64          aPrecision,
                                        stlInt64          aScale,
                                        stlInt64          aInteger,
                                        stlInt64        * aLength,
                                        stlErrorStack   * aErrorStack );

stlStatus dtdNumericGetLengthFromReal( stlInt64          aPrecision,
                                       stlInt64          aScale,
                                       stlFloat64        aReal,
                                       stlInt64        * aLength,
                                       stlErrorStack   * aErrorStack );

stlStatus dtdFloatGetLengthFromReal( stlInt64          aPrecision,
                                     stlInt64          aScale,
                                     stlFloat64        aReal,
                                     stlInt64        * aLength,
                                     stlErrorStack   * aErrorStack );

stlStatus dtdNumericSetValueFromString( stlChar              * aString,
                                        stlInt64               aStringLength,
                                        stlInt64               aPrecision,
                                        stlInt64               aScale,
                                        dtlStringLengthUnit    aStringUnit,
                                        dtlIntervalIndicator   aIntervalIndicator,
                                        stlInt64               aAvailableSize,
                                        dtlDataValue         * aValue,
                                        stlBool              * aSuccessWithInfo,
                                        dtlFuncVector        * aSourceVectorFunc,
                                        void                 * aSourceVectorArg,
                                        dtlFuncVector        * aDestVectorFunc,
                                        void                 * aDestVectorArg,
                                        stlErrorStack        * aErrorStack );

stlStatus dtdFloatSetValueFromString( stlChar              * aString,
                                      stlInt64               aStringLength,
                                      stlInt64               aPrecision,
                                      stlInt64               aScale,
                                      dtlStringLengthUnit    aStringUnit,
                                      dtlIntervalIndicator   aIntervalIndicator,
                                      stlInt64               aAvailableSize,
                                      dtlDataValue         * aValue,
                                      stlBool              * aSuccessWithInfo,
                                      dtlFuncVector        * aSourceVectorFunc,
                                      void                 * aSourceVectorArg,
                                      dtlFuncVector        * aDestVectorFunc,
                                      void                 * aDestVectorArg,
                                      stlErrorStack        * aErrorStack );

stlStatus dtdNumericSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdFloatSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdNumericSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdFloatSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdNumericSetValueFromFloat32( stlFloat32             aFloat32,
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

stlStatus dtdFloatSetValueFromFloat32( stlFloat32             aFloat32,
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

stlStatus dtdNumericSetValueFromFloat64( stlFloat64             aFloat64,
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

stlStatus dtdFloatSetValueFromFloat64( stlFloat64             aFloat64,
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

stlStatus dtdNumericReverseByteOrder( void            * aValue,
                                      stlBool           aIsSameEndian,
                                      stlErrorStack   * aErrorStack );

stlStatus dtdNumericToString( dtlDataValue    * aValue,
                              stlInt64          aPrecision,
                              stlInt64          aScale,
                              stlInt64          aAvailableSize,
                              void            * aBuffer,
                              stlInt64        * aLength,
                              dtlFuncVector   * aVectorFunc,
                              void            * aVectorArg,
                              stlErrorStack   * aErrorStack );

stlStatus dtdNumericToStringForAvailableSize( dtlDataValue    * aValue,
                                              stlInt64          aPrecision,
                                              stlInt64          aScale,
                                              stlInt64          aAvailableSize,
                                              void            * aBuffer,
                                              stlInt64        * aLength,
                                              dtlFuncVector   * aVectorFunc,
                                              void            * aVectorArg,
                                              stlErrorStack   * aErrorStack );

stlStatus dtdNumericSetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdNumericSetValueFromUtf32WCharString( void                 * aString,
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

stlStatus dtdFloatSetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdFloatSetValueFromUtf32WCharString( void                 * aString,
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

/** @} */

#endif /* _DTDNUMERIC_H_ */
