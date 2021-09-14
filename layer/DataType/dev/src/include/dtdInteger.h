/*******************************************************************************
 * dtdInteger.h
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


#ifndef _DTDINTEGER_H_
#define _DTDINTEGER_H_ 1

/**
 * @file dtdInteger.h
 * @brief DataType Layer dtlInteger Definition
 */

/**
 * @defgroup dtlInteger dtlInteger(4바이트 integer)
 * @ingroup dtd
 * @{
 */

stlStatus dtdIntegerGetLength( stlInt64              aPrecision,
                               dtlStringLengthUnit   aStringUnit,
                               stlInt64            * aLength,
                               dtlFuncVector       * aVectorFunc,
                               void                * aVectorArg,
                               stlErrorStack       * aErrorStack );

stlStatus dtdIntegerGetLengthFromString( stlInt64              aPrecision,
                                         stlInt64              aScale,
                                         dtlStringLengthUnit   aStringUnit,
                                         stlChar             * aString,
                                         stlInt64              aStringLength,
                                         stlInt64            * aLength,
                                         dtlFuncVector       * aVectorFunc,
                                         void                * aVectorArg,
                                         stlErrorStack       * aErrorStack );

stlStatus dtdIntegerGetLengthFromInteger( stlInt64          aPrecision,
                                          stlInt64          aScale,
                                          stlInt64          aInteger,
                                          stlInt64        * aLength,
                                          stlErrorStack   * aErrorStack );

stlStatus dtdIntegerGetLengthFromReal( stlInt64          aPrecision,
                                       stlInt64          aScale,
                                       stlFloat64        aReal,
                                       stlInt64        * aLength,
                                       stlErrorStack   * aErrorStack );

stlStatus dtdIntegerSetValueFromString( stlChar              * aString,
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

stlStatus dtdIntegerSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdIntegerSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdIntegerReverseByteOrder( void            * aValue,
                                      stlBool           aIsSameEndian,
                                      stlErrorStack   * aErrorStack );

stlStatus dtdIntegerToString( dtlDataValue    * aValue,
                              stlInt64          aPrecision,
                              stlInt64          aScale,
                              stlInt64          aAvailableSize,
                              void            * aBuffer,
                              stlInt64        * aLength,
                              dtlFuncVector   * aVectorFunc,
                              void            * aVectorArg,
                              stlErrorStack   * aErrorStack );

stlStatus dtdIntegerToStringForAvailableSize( dtlDataValue    * aValue,
                                              stlInt64          aPrecision,
                                              stlInt64          aScale,
                                              stlInt64          aAvailableSize,
                                              void            * aBuffer,
                                              stlInt64        * aLength,
                                              dtlFuncVector   * aVectorFunc,
                                              void            * aVectorArg,
                                              stlErrorStack   * aErrorStack );
/** @} */

#endif /* _DTDINTEGER_H_ */

