/*******************************************************************************
 * dtdTime.h
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


#ifndef _DTDTIME_H_
#define _DTDTIME_H_ 1

/**
 * @file dtdTime.h
 * @brief DataType Layer dtlTime Definition
 */

/**
 * @defgroup dtlTime dtlTime
 * @ingroup dtd
 * @{
 */

stlStatus dtdTimeGetLength( stlInt64              aPrecision,
                            dtlStringLengthUnit   aStringUnit,
                            stlInt64            * aLength,
                            dtlFuncVector       * aVectorFunc,
                            void                * aVectorArg,
                            stlErrorStack       * aErrorStack );

stlStatus dtdTimeGetLengthFromString( stlInt64              aPrecision,
                                      stlInt64              aScale,
                                      dtlStringLengthUnit   aStringUnit,
                                      stlChar             * aString,
                                      stlInt64              aStringLength,
                                      stlInt64            * aLength,
                                      dtlFuncVector       * aVectorFunc,
                                      void                * aVectorArg,
                                      stlErrorStack       * aErrorStack );

stlStatus dtdTimeGetLengthFromInteger( stlInt64          aPrecision,
                                       stlInt64          aScale,
                                       stlInt64          aInteger,
                                       stlInt64        * aLength,
                                       stlErrorStack   * aErrorStack );

stlStatus dtdTimeGetLengthFromReal( stlInt64          aPrecision,
                                    stlInt64          aScale,
                                    stlFloat64        aReal,
                                    stlInt64        * aLength,
                                    stlErrorStack   * aErrorStack );

stlStatus dtdTimeSetValueFromString( stlChar              * aString,
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

stlStatus dtdTimeSetValueFromTypedLiteral( stlChar             * aString,
                                           stlInt64              aStringLength,
                                           stlInt64              aAvailableSize,
                                           dtlDataValue        * aValue,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack );

stlStatus dtdTimeSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdTimeSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdTimeReverseByteOrder( void            * aValue,
                                   stlBool           aIsSameEndian,
                                   stlErrorStack   * aErrorStack );

stlStatus dtdTimeToString( dtlDataValue    * aValue,
                           stlInt64          aPrecision,
                           stlInt64          aScale,
                           stlInt64          aAvailableSize,
                           void            * aBuffer,
                           stlInt64        * aLength,
                           dtlFuncVector   * aVectorFunc,
                           void            * aVectorArg,
                           stlErrorStack   * aErrorStack );

stlStatus dtdTimeToStringForAvailableSize( dtlDataValue    * aValue,
                                           stlInt64          aPrecision,
                                           stlInt64          aScale,
                                           stlInt64          aAvailableSize,
                                           void            * aBuffer,
                                           stlInt64        * aLength,
                                           dtlFuncVector   * aVectorFunc,
                                           void            * aVectorArg,
                                           stlErrorStack   * aErrorStack );

stlStatus dtdTimeSetValueFromFormatInfo( stlChar              * aString,
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

stlStatus dtdTimeSetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdTimeSetValueFromUtf32WCharString( void                 * aString,
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

stlStatus dtdTimeToUtf16WCharString( dtlDataValue    * aValue,
                                     stlInt64          aPrecision,
                                     stlInt64          aScale,
                                     stlInt64          aAvailableSize,
                                     void            * aBuffer,
                                     stlInt64        * aLength,
                                     dtlFuncVector   * aVectorFunc,
                                     void            * aVectorArg,
                                     stlErrorStack   * aErrorStack );

stlStatus dtdTimeToUtf32WCharString( dtlDataValue    * aValue,
                                     stlInt64          aPrecision,
                                     stlInt64          aScale,
                                     stlInt64          aAvailableSize,
                                     void            * aBuffer,
                                     stlInt64        * aLength,
                                     dtlFuncVector   * aVectorFunc,
                                     void            * aVectorArg,
                                     stlErrorStack   * aErrorStack );


/** @} */

#endif /* _DTDTIME_H_ */
