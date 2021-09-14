/*******************************************************************************
 * dtdBigInt.h
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


#ifndef _DTDBIGINT_H_
#define _DTDBIGINT_H_ 1

/**
 * @file dtdBigInt.h
 * @brief DataType Layer dtlBigInt Definition
 */

/**
 * @addtogroup dtlBigInt dtlBigInt(8바이트 integer)
 * @ingroup dtd
 * @{
 */

stlStatus dtdBigIntGetLength( stlInt64              aPrecision,
                              dtlStringLengthUnit   aStringUnit,
                              stlInt64            * aLength,
                              dtlFuncVector       * aVectorFunc,
                              void                * aVectorArg,
                              stlErrorStack       * aErrorStack );

stlStatus dtdBigIntGetLengthFromString( stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64            * aLength,
                                        dtlFuncVector       * aVectorFunc,
                                        void                * aVectorArg,
                                        stlErrorStack       * aErrorStack );

stlStatus dtdBigIntGetLengthFromInteger( stlInt64          aPrecision,
                                         stlInt64          aScale,
                                         stlInt64          aInteger,
                                         stlInt64        * aLength,
                                         stlErrorStack   * aErrorStack );

stlStatus dtdBigIntGetLengthFromReal( stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlFloat64        aReal,
                                      stlInt64        * aLength,
                                      stlErrorStack   * aErrorStack );

stlStatus dtdBigIntSetValueFromString( stlChar              * aString,
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

stlStatus dtdBigIntSetValueFromInteger( stlInt64               aInteger,
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
stlStatus dtdBigIntSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdBigIntReverseByteOrder( void            * aValue,
                                     stlBool           aIsSameEndian,
                                     stlErrorStack   * aErrorStack );

stlStatus dtdBigIntToString( dtlDataValue    * aValue,
                             stlInt64          aPrecision,
                             stlInt64          aScale,
                             stlInt64          aAvailableSize,
                             void            * aBuffer,
                             stlInt64        * aLength,
                             dtlFuncVector   * aVectorFunc,
                             void            * aVectorArg,
                             stlErrorStack   * aErrorStack );

stlStatus dtdBigIntToStringForAvailableSize( dtlDataValue    * aValue,
                                             stlInt64          aPrecision,
                                             stlInt64          aScale,
                                             stlInt64          aAvailableSize,
                                             void            * aBuffer,
                                             stlInt64        * aLength,
                                             dtlFuncVector   * aVectorFunc,
                                             void            * aVectorArg,
                                             stlErrorStack   * aErrorStack );

/** @} */

#endif /* _DTDBIGINT_H_ */

