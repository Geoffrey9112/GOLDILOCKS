/*******************************************************************************
 * dtdChar.h
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


#ifndef _DTDCHAR_H_
#define _DTDCHAR_H_ 1

/**
 * @file dtdChar.h
 * @brief DataType Layer dtlChar Definition
 */

/**
 * @defgroup dtlChar dtlChar
 * @ingroup dtd
 * @{
 */

stlStatus dtdCharGetLength( stlInt64              aPrecision,
                            dtlStringLengthUnit   aStringUnit,
                            stlInt64            * aLength,
                            dtlFuncVector       * aVectorFunc,
                            void                * aVectorArg,
                            stlErrorStack       * aErrorStack );

stlStatus dtdCharGetLengthFromString( stlInt64              aPrecision,
                                      stlInt64              aScale,
                                      dtlStringLengthUnit   aStringUnit,
                                      stlChar             * aString,
                                      stlInt64              aStringLength,
                                      stlInt64            * aLength,
                                      dtlFuncVector       * aVectorFunc,
                                      void                * aVectorArg,
                                      stlErrorStack       * aErrorStack );

stlStatus dtdCharGetLengthFromInteger( stlInt64          aPrecision,
                                       stlInt64          aScale,
                                       stlInt64          aInteger, 
                                       stlInt64        * aLength,
                                       stlErrorStack   * aErrorStack );

stlStatus dtdCharGetLengthFromReal( stlInt64          aPrecision,
                                    stlInt64          aScale,
                                    stlFloat64        aReal, 
                                    stlInt64        * aLength,
                                    stlErrorStack   * aErrorStack );

stlStatus dtdCharSetValueFromString( stlChar              * aString,
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

stlStatus dtdCharSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdCharSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdCharReverseByteOrder( void            * aValue,
                                   stlBool           aIsSameEndian,
                                   stlErrorStack   * aErrorStack );


stlStatus dtdCharToString( dtlDataValue    * aValue,
                           stlInt64          aPrecision,
                           stlInt64          aScale,
                           stlInt64          aAvailableSize,
                           void            * aBuffer,
                           stlInt64        * aLength,
                           dtlFuncVector   * aVectorFunc,
                           void            * aVectorArg,
                           stlErrorStack   * aErrorStack );

stlStatus dtdCharToStringForAvailableSize( dtlDataValue    * aValue,
                                           stlInt64          aPrecision,
                                           stlInt64          aScale,
                                           stlInt64          aAvailableSize,
                                           void            * aBuffer,
                                           stlInt64        * aLength,
                                           dtlFuncVector   * aVectorFunc,
                                           void            * aVectorArg,
                                           stlErrorStack   * aErrorStack );

stlStatus dtdCharSetValueFromStringWithoutPaddNull( stlChar              * aString,
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

stlStatus dtdCharSetValueFromIntegerWithoutPaddNull( stlInt64               aInteger,
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

stlStatus dtdCharSetValueFromRealWithoutPaddNull( stlFloat64             aReal,
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

#endif /* _DTDCHAR_H_ */
