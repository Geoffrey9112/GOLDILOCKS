/*******************************************************************************
 * dtdReal.h
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


#ifndef _DTDREAL_H_
#define _DTDREAL_H_ 1

/**
 * @file dtdReal.h
 * @brief DataType Layer dtlReal Definition
 */

/**
 * @defgroup dtlReal dtlReal
 * @ingroup dtd
 * @{
 */

stlStatus dtdRealGetLength( stlInt64              aPrecision,
                            dtlStringLengthUnit   aStringUnit,
                            stlInt64            * aLength,
                            dtlFuncVector       * aVectorFunc,
                            void                * aVectorArg,
                            stlErrorStack       * aErrorStack );

stlStatus dtdRealGetLengthFromString( stlInt64              aPrecision,
                                      stlInt64              aScale,
                                      dtlStringLengthUnit   aStringUnit,
                                      stlChar             * aString,
                                      stlInt64              aStringLength,
                                      stlInt64            * aLength,
                                      dtlFuncVector       * aVectorFunc,
                                      void                * aVectorArg,
                                      stlErrorStack       * aErrorStack );

stlStatus dtdRealGetLengthFromInteger( stlInt64          aPrecision,
                                       stlInt64          aScale,
                                       stlInt64          aInteger,
                                       stlInt64        * aLength,
                                       stlErrorStack   * aErrorStack );

stlStatus dtdRealGetLengthFromReal( stlInt64          aPrecision,
                                    stlInt64          aScale,
                                    stlFloat64        aReal,
                                    stlInt64        * aLength,
                                    stlErrorStack   * aErrorStack );

stlStatus dtdRealSetValueFromString( stlChar              * aString,
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

stlStatus dtdRealSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdRealSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdRealReverseByteOrder( void            * aValue,
                                   stlBool           aIsSameEndian,
                                   stlErrorStack   * aErrorStack );

stlStatus dtdRealToString( dtlDataValue    * aValue,
                           stlInt64          aPrecision,
                           stlInt64          aScale,
                           stlInt64          aAvailableSize,
                           void            * aBuffer,
                           stlInt64        * aLength,
                           dtlFuncVector   * aVectorFunc,
                           void            * aVectorArg,
                           stlErrorStack   * aErrorStack );

stlStatus dtdRealToStringForAvailableSize( dtlDataValue    * aValue,
                                           stlInt64          aPrecision,
                                           stlInt64          aScale,
                                           stlInt64          aAvailableSize,
                                           void            * aBuffer,
                                           stlInt64        * aLength,
                                           dtlFuncVector   * aVectorFunc,
                                           void            * aVectorArg,
                                           stlErrorStack   * aErrorStack );

stlStatus dtdRealToExponentString( dtlDataValue    * aValue,
                                   stlInt64          aAvailableSize,
                                   void            * aBuffer,
                                   stlInt64        * aLength,
                                   dtlFuncVector   * aVectorFunc,
                                   void            * aVectorArg,
                                   stlBool         * aSuccessWithInfo,
                                   stlErrorStack   * aErrorStack );

stlStatus dtdRealToNonTruncateExponentString( dtlDataValue    * aValue,
                                              stlInt64          aAvailableSize,
                                              void            * aBuffer,
                                              stlInt64        * aLength,
                                              dtlFuncVector   * aVectorFunc,
                                              void            * aVectorArg,
                                              stlErrorStack   * aErrorStack );

/** @} */

#endif /* _DTDREAL_H_ */

