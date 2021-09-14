/*******************************************************************************
 * dtdIntervalDayToSecond.h
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


#ifndef _DTDINTERVAL_DAYTOSECOND_H_
#define _DTDINTERVAL_DAYTOSECOND_H_ 1

/**
 * @file dtdIntervalDayToSecond.h
 * @brief DataType Layer dtdIntervalDayToSecond Definition
 */

/**
 * @defgroup dtdIntervalDayToSecond dtdIntervalDayToSecond
 * @ingroup dtd
 * @{
 */

stlStatus dtdIntervalDayToSecondGetLength( stlInt64              aPrecision,
                                           dtlStringLengthUnit   aStringUnit,
                                           stlInt64            * aLength,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack );

stlStatus dtdIntervalDayToSecondGetLengthFromString( stlInt64              aPrecision,
                                                     stlInt64              aScale,
                                                     dtlStringLengthUnit   aStringUnit,
                                                     stlChar             * aString,
                                                     stlInt64              aStringLength,
                                                     stlInt64            * aLength,
                                                     dtlFuncVector       * aVectorFunc,
                                                     void                * aVectorArg,
                                                     stlErrorStack       * aErrorStack );

stlStatus dtdIntervalDayToSecondGetLengthFromInteger( stlInt64          aPrecision,
                                                      stlInt64          aScale,
                                                      stlInt64          aInteger,
                                                      stlInt64        * aLength,
                                                      stlErrorStack   * aErrorStack );

stlStatus dtdIntervalDayToSecondGetLengthFromReal( stlInt64          aPrecision,
                                                   stlInt64          aScale,
                                                   stlFloat64        aReal,
                                                   stlInt64        * aLength,
                                                   stlErrorStack   * aErrorStack );

stlStatus dtdIntervalDayToSecondSetValueFromString( stlChar              * aString,
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

stlStatus dtdIntervalDayToSecondSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdIntervalDayToSecondSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdIntervalDayToSecondReverseByteOrder( void            * aValue,
                                                  stlBool           aIsSameEndian,
                                                  stlErrorStack   * aErrorStack );

stlStatus dtdIntervalDayToSecondToString( dtlDataValue    * aValue,
                                          stlInt64          aPrecision,
                                          stlInt64          aScale,
                                          stlInt64          aAvailableSize,
                                          void            * aBuffer,
                                          stlInt64        * aLength,
                                          dtlFuncVector   * aVectorFunc,
                                          void            * aVectorArg,
                                          stlErrorStack   * aErrorStack );

stlStatus dtdIntervalDayToSecondToStringForAvailableSize( dtlDataValue    * aValue,
                                                          stlInt64          aPrecision,
                                                          stlInt64          aScale,
                                                          stlInt64          aAvailableSize,
                                                          void            * aBuffer,
                                                          stlInt64        * aLength,
                                                          dtlFuncVector   * aVectorFunc,
                                                          void            * aVectorArg,
                                                          stlErrorStack   * aErrorStack );

stlStatus dtdIntervalDayToSecondToFormatString( dtlDataValue    * aValue,
                                                stlInt64          aLeadingPrecision,
                                                stlInt64          aFractionalSecondPrecision,
                                                stlInt64          aAvailableSize,
                                                void            * aBuffer,
                                                stlInt64        * aLength,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack );

stlStatus dtdIntervalDayToSecondSetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdIntervalDayToSecondSetValueFromUtf32WCharString( void                 * aString,
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

stlStatus dtdIntervalDayToSecondToUtf16WCharString( dtlDataValue    * aValue,
                                                    stlInt64          aPrecision,
                                                    stlInt64          aScale,
                                                    stlInt64          aAvailableSize,
                                                    void            * aBuffer,
                                                    stlInt64        * aLength,
                                                    dtlFuncVector   * aVectorFunc,
                                                    void            * aVectorArg,
                                                    stlErrorStack   * aErrorStack );


stlStatus dtdIntervalDayToSecondToUtf32WCharString( dtlDataValue    * aValue,
                                                    stlInt64          aPrecision,
                                                    stlInt64          aScale,
                                                    stlInt64          aAvailableSize,
                                                    void            * aBuffer,
                                                    stlInt64        * aLength,
                                                    dtlFuncVector   * aVectorFunc,
                                                    void            * aVectorArg,
                                                    stlErrorStack   * aErrorStack );

/** @} */

#endif /* _DTDINTERVAL_DAYTOSECOND_H_ */
