/*******************************************************************************
 * dtdTimeTz.h
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


#ifndef _DTDTIMETZ_H_
#define _DTDTIMETZ_H_ 1

/**
 * @file dtdTimeTz.h
 * @brief DataType Layer dtlTimeTz Definition
 */

/**
 * @defgroup dtlTimeTz dtlTimeTz
 * @ingroup dtd
 * @{
 */

stlStatus dtdTimeTzGetLength( stlInt64              aPrecision,
                              dtlStringLengthUnit   aStringUnit,
                              stlInt64            * aLength,
                              dtlFuncVector       * aVectorFunc,
                              void                * aVectorArg,
                              stlErrorStack       * aErrorStack );

stlStatus dtdTimeTzGetLengthFromString( stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64            * aLength,
                                        dtlFuncVector       * aVectorFunc,
                                        void                * aVectorArg,
                                        stlErrorStack       * aErrorStack );

stlStatus dtdTimeTzGetLengthFromInteger( stlInt64          aPrecision,
                                         stlInt64          aScale,
                                         stlInt64          aInteger,
                                         stlInt64        * aLength,
                                         stlErrorStack   * aErrorStack );

stlStatus dtdTimeTzGetLengthFromReal( stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlFloat64        aReal,
                                      stlInt64        * aLength,
                                      stlErrorStack   * aErrorStack );

stlStatus dtdTimeTzSetValueFromString( stlChar              * aString,
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

stlStatus dtdTimeTzSetValueFromTypedLiteral( stlChar             * aString,
                                             stlInt64              aStringLength,
                                             stlInt64              aAvailableSize,
                                             dtlDataValue        * aValue,
                                             dtlFuncVector       * aVectorFunc,
                                             void                * aVectorArg,
                                             stlErrorStack       * aErrorStack );

stlStatus dtdTimeTzSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdTimeTzSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdTimeTzReverseByteOrder( void            * aValue,
                                     stlBool           aIsSameEndian,
                                     stlErrorStack   * aErrorStack );

stlStatus dtdTimeTzToString( dtlDataValue    * aValue,
                             stlInt64          aPrecision,
                             stlInt64          aScale,
                             stlInt64          aAvailableSize,
                             void            * aBuffer,
                             stlInt64        * aLength,
                             dtlFuncVector   * aVectorFunc,
                             void            * aVectorArg,
                             stlErrorStack   * aErrorStack );

stlStatus dtdTimeTzToStringForAvailableSize( dtlDataValue    * aValue,
                                             stlInt64          aPrecision,
                                             stlInt64          aScale,
                                             stlInt64          aAvailableSize,
                                             void            * aBuffer,
                                             stlInt64        * aLength,
                                             dtlFuncVector   * aVectorFunc,
                                             void            * aVectorArg,
                                             stlErrorStack   * aErrorStack );

stlStatus dtdTimeTzSetValueFromFormatInfo( stlChar              * aString,
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

stlStatus dtdTimeTzSetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdTimeTzSetValueFromUtf32WCharString( void                 * aString,
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

stlStatus dtdTimeTzToUtf16WCharString( dtlDataValue    * aValue,
                                       stlInt64          aPrecision,
                                       stlInt64          aScale,
                                       stlInt64          aAvailableSize,
                                       void            * aBuffer,
                                       stlInt64        * aLength,
                                       dtlFuncVector   * aVectorFunc,
                                       void            * aVectorArg,
                                       stlErrorStack   * aErrorStack );

stlStatus dtdTimeTzToUtf32WCharString( dtlDataValue    * aValue,
                                       stlInt64          aPrecision,
                                       stlInt64          aScale,
                                       stlInt64          aAvailableSize,
                                       void            * aBuffer,
                                       stlInt64        * aLength,
                                       dtlFuncVector   * aVectorFunc,
                                       void            * aVectorArg,
                                       stlErrorStack   * aErrorStack );

/** @} */

#endif /* _DTDTIMETZ_H_ */
