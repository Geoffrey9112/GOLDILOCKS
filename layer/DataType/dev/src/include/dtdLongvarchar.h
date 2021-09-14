/*******************************************************************************
 * dtdLongvarchar.h
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


#ifndef _DTDLONGVARCHAR_H_
#define _DTDLONGVARCHAR_H_ 1

/**
 * @file dtdLongvarchar.h
 * @brief DataType Layer dtlLongvarchar Definition
 */

/**
 * @defgroup dtlLongvarchar dtlLongvarchar
 * @ingroup dtd
 * @{
 */

stlStatus dtdLongvarcharGetLength( stlInt64              aPrecision,
                                   dtlStringLengthUnit   aStringUnit,
                                   stlInt64            * aLength,
                                   dtlFuncVector       * aVectorFunc,
                                   void                * aVectorArg,
                                   stlErrorStack       * aErrorStack );

stlStatus dtdLongvarcharGetLengthFromString( stlInt64              aPrecision,
                                             stlInt64              aScale,
                                             dtlStringLengthUnit   aStringUnit,
                                             stlChar             * aString,
                                             stlInt64              aStringLength,
                                             stlInt64            * aLength,
                                             dtlFuncVector       * aVectorFunc,
                                             void                * aVectorArg,
                                             stlErrorStack       * aErrorStack );

stlStatus dtdLongvarcharGetLengthFromInteger( stlInt64          aPrecision,
                                              stlInt64          aScale,
                                              stlInt64          aInteger, 
                                              stlInt64        * aLength,
                                              stlErrorStack   * aErrorStack );

stlStatus dtdLongvarcharGetLengthFromReal( stlInt64          aPrecision,
                                           stlInt64          aScale,
                                           stlFloat64        aReal, 
                                           stlInt64        * aLength,
                                           stlErrorStack   * aErrorStack );

stlStatus dtdLongvarcharSetValueFromString( stlChar              * aString,
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

stlStatus dtdLongvarcharSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdLongvarcharSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdLongvarcharReverseByteOrder( void            * aValue,
                                          stlBool           aIsSameEndian,
                                          stlErrorStack   * aErrorStack );

stlStatus dtdLongvarcharToString( dtlDataValue    * aValue,
                                  stlInt64          aPrecision,
                                  stlInt64          aScale,
                                  stlInt64          aAvailableSize,
                                  void            * aBuffer,
                                  stlInt64        * aLength,
                                  dtlFuncVector   * aVectorFunc,
                                  void            * aVectorArg,
                                  stlErrorStack   * aErrorStack );

stlStatus dtdLongvarcharToStringForAvailableSize( dtlDataValue    * aValue,
                                                  stlInt64          aPrecision,
                                                  stlInt64          aScale,
                                                  stlInt64          aAvailableSize,
                                                  void            * aBuffer,
                                                  stlInt64        * aLength,
                                                  dtlFuncVector   * aVectorFunc,
                                                  void            * aVectorArg,
                                                  stlErrorStack   * aErrorStack );

stlStatus dtdLongvarcharSetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdLongvarcharSetValueFromUtf32WCharString( void                 * aString,
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

stlStatus dtdLongvarcharSetValueFromWCharString( dtlUnicodeEncoding     aUniEncoding,
                                                 void                 * aString,
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

#endif /* _DTDLONGVARCHAR_H_ */
