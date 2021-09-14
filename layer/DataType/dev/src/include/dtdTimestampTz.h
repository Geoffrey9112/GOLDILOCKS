/*******************************************************************************
 * dtdTimestampTz.h
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


#ifndef _DTDTIMESTAMPTZ_H_
#define _DTDTIMESTAMPTZ_H_ 1

/**
 * @file dtdTimestampTz.h
 * @brief DataType Layer dtlTimestampTz Definition
 */

/**
 * @defgroup dtlTimestampTz dtlTimestampTz
 * @ingroup dtd
 * @{
 */

stlStatus dtdTimestampTzGetLength( stlInt64              aPrecision,
                                   dtlStringLengthUnit   aStringUnit,
                                   stlInt64            * aLength,
                                   dtlFuncVector       * aVectorFunc,
                                   void                * aVectorArg,
                                   stlErrorStack       * aErrorStack );

stlStatus dtdTimestampTzGetLengthFromString( stlInt64              aPrecision,
                                             stlInt64              aScale,
                                             dtlStringLengthUnit   aStringUnit,
                                             stlChar             * aString,
                                             stlInt64              aStringLength,
                                             stlInt64            * aLength,
                                             dtlFuncVector       * aVectorFunc,
                                             void                * aVectorArg,
                                             stlErrorStack       * aErrorStack );

stlStatus dtdTimestampTzGetLengthFromInteger( stlInt64          aPrecision,
                                              stlInt64          aScale,
                                              stlInt64          aInteger,
                                              stlInt64        * aLength,
                                              stlErrorStack   * aErrorStack );

stlStatus dtdTimestampTzGetLengthFromReal( stlInt64          aPrecision,
                                           stlInt64          aScale,
                                           stlFloat64        aReal,
                                           stlInt64        * aLength,
                                           stlErrorStack   * aErrorStack );

stlStatus dtdTimestampTzSetValueFromString( stlChar              * aString,
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

stlStatus dtdTimestampTzSetValueFromTypedLiteral( stlChar             * aString,
                                                  stlInt64              aStringLength,
                                                  stlInt64              aAvailableSize,
                                                  dtlDataValue        * aValue,
                                                  dtlFuncVector       * aVectorFunc,
                                                  void                * aVectorArg,
                                                  stlErrorStack       * aErrorStack );

stlStatus dtdTimestampTzSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdTimestampTzSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdTimestampTzReverseByteOrder( void            * aValue,
                                          stlBool           aIsSameEndian,
                                          stlErrorStack   * aErrorStack );

stlStatus dtdTimestampTzToString( dtlDataValue    * aValue,
                                  stlInt64          aPrecision,
                                  stlInt64          aScale,
                                  stlInt64          aAvailableSize,
                                  void            * aBuffer,
                                  stlInt64        * aLength,
                                  dtlFuncVector   * aVectorFunc,
                                  void            * aVectorArg,
                                  stlErrorStack   * aErrorStack );

stlStatus dtdTimestampTzToStringForAvailableSize( dtlDataValue    * aValue,
                                                  stlInt64          aPrecision,
                                                  stlInt64          aScale,
                                                  stlInt64          aAvailableSize,
                                                  void            * aBuffer,
                                                  stlInt64        * aLength,
                                                  dtlFuncVector   * aVectorFunc,
                                                  void            * aVectorArg,
                                                  stlErrorStack   * aErrorStack );

stlStatus dtdTimestampTzSetValueFromFormatInfo( stlChar              * aString,
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

stlStatus dtdTimestampTzSetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdTimestampTzSetValueFromUtf32WCharString( void                 * aString,
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

stlStatus dtdTimestampTzToUtf16WCharString( dtlDataValue    * aValue,
                                            stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            stlInt64          aAvailableSize,
                                            void            * aBuffer,
                                            stlInt64        * aLength,
                                            dtlFuncVector   * aVectorFunc,
                                            void            * aVectorArg,
                                            stlErrorStack   * aErrorStack );


stlStatus dtdTimestampTzToUtf32WCharString( dtlDataValue    * aValue,
                                            stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            stlInt64          aAvailableSize,
                                            void            * aBuffer,
                                            stlInt64        * aLength,
                                            dtlFuncVector   * aVectorFunc,
                                            void            * aVectorArg,
                                            stlErrorStack   * aErrorStack );


/** @} */

#endif /* _DTDTIMESTAMPTZ_H_ */
