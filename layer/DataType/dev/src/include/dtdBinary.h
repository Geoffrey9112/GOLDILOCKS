/*******************************************************************************
 * dtdBinary.h
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


#ifndef _DTDBINARY_H_
#define _DTDBINARY_H_ 1

/**
 * @file dtdBinary.h
 * @brief DataType Layer dtlBinary Definition
 */

/**
 * @defgroup dtlBinary dtlBinary
 * @ingroup dtd
 * @{
 */

stlStatus dtdBinaryGetLength( stlInt64              aPrecision,
                              dtlStringLengthUnit   aStringUnit,
                              stlInt64            * aLength,
                              dtlFuncVector       * aVectorFunc,
                              void                * aVectorArg,
                              stlErrorStack       * aErrorStack );

stlStatus dtdBinaryGetLengthFromString( stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64            * aLength,
                                        dtlFuncVector       * aVectorFunc,
                                        void                * aVectorArg,
                                        stlErrorStack       * aErrorStack );

stlStatus dtdBinaryGetLengthFromInteger( stlInt64          aPrecision,
                                         stlInt64          aScale,
                                         stlInt64          aInteger,
                                         stlInt64        * aLength,
                                         stlErrorStack   * aErrorStack );

stlStatus dtdBinaryGetLengthFromReal( stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlFloat64        aReal,
                                      stlInt64        * aLength,
                                      stlErrorStack   * aErrorStack );

stlStatus dtdBinarySetValueFromString( stlChar              * aString,
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

stlStatus dtdBinarySetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdBinarySetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdBinaryReverseByteOrder( void            * aValue,
                                     stlBool           aIsSameEndian,
                                     stlErrorStack   * aErrorStack );

stlStatus dtdBinaryToString( dtlDataValue    * aValue,
                             stlInt64          aPrecision,
                             stlInt64          aScale,
                             stlInt64          aAvailableSize,
                             void            * aBuffer,
                             stlInt64        * aLength,
                             dtlFuncVector   * aVectorFunc,
                             void            * aVectorArg,
                             stlErrorStack   * aErrorStack );

stlStatus dtdBinaryToStringForAvailableSize( dtlDataValue    * aValue,
                                             stlInt64          aPrecision,
                                             stlInt64          aScale,
                                             stlInt64          aAvailableSize,
                                             void            * aBuffer,
                                             stlInt64        * aLength,
                                             dtlFuncVector   * aVectorFunc,
                                             void            * aVectorArg,
                                             stlErrorStack   * aErrorStack );


stlStatus dtdBinarySetValueFromStringWithoutPaddNull( stlChar              * aString,
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

/** @} */

#endif /* _DTDBINARY_H_ */
