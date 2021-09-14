/*******************************************************************************
 * dtdIntervalYearToMonth.h
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


#ifndef _DTDINTERVAL_YEARTOMONTH_H_
#define _DTDINTERVAL_YEARTOMONTH_H_ 1

/**
 * @file dtdIntervalYearToMonth.h
 * @brief DataType Layer dtdIntervalYearToMonth Definition
 */

/**
 * @defgroup dtdIntervalYearToMonth dtdIntervalYearToMonth
 * @ingroup dtd
 * @{
 */

stlStatus dtdIntervalYearToMonthGetLength( stlInt64              aPrecision,
                                           dtlStringLengthUnit   aStringUnit,
                                           stlInt64            * aLength,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack );

stlStatus dtdIntervalYearToMonthGetLengthFromString( stlInt64              aPrecision,
                                                     stlInt64              aScale,
                                                     dtlStringLengthUnit   aStringUnit,
                                                     stlChar             * aString,
                                                     stlInt64              aStringLength,
                                                     stlInt64            * aLength,
                                                     dtlFuncVector       * aVectorFunc,
                                                     void                * aVectorArg,
                                                     stlErrorStack       * aErrorStack );

stlStatus dtdIntervalYearToMonthGetLengthFromInteger( stlInt64          aPrecision,
                                                      stlInt64          aScale,
                                                      stlInt64          aInteger,
                                                      stlInt64        * aLength,
                                                      stlErrorStack   * aErrorStack );

stlStatus dtdIntervalYearToMonthGetLengthFromReal( stlInt64          aPrecision,
                                                   stlInt64          aScale,
                                                   stlFloat64        aReal,
                                                   stlInt64        * aLength,
                                                   stlErrorStack   * aErrorStack );

stlStatus dtdIntervalYearToMonthSetValueFromString( stlChar              * aString,
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

stlStatus dtdIntervalYearToMonthSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdIntervalYearToMonthSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdIntervalYearToMonthReverseByteOrder( void            * aValue,
                                                  stlBool           aIsSameEndian,
                                                  stlErrorStack   * aErrorStack );

stlStatus dtdIntervalYearToMonthToFormatString( dtlDataValue    * aValue,
                                                stlInt64          aLeadingPrecision,
                                                stlInt64          aAvailableSize,
                                                void            * aBuffer,
                                                stlInt64        * aLength,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack );

stlStatus dtdIntervalYearToMonthToString( dtlDataValue    * aValue,
                                          stlInt64          aPrecision,
                                          stlInt64          aScale,
                                          stlInt64          aAvailableSize,
                                          void            * aBuffer,
                                          stlInt64        * aLength,
                                          dtlFuncVector   * aVectorFunc,
                                          void            * aVectorArg,
                                          stlErrorStack   * aErrorStack );

stlStatus dtdIntervalYearToMonthToStringForAvailableSize( dtlDataValue    * aValue,
                                                          stlInt64          aPrecision,
                                                          stlInt64          aScale, 
                                                          stlInt64          aAvailableSize,
                                                          void            * aBuffer,
                                                          stlInt64        * aLength,
                                                          dtlFuncVector   * aVectorFunc,
                                                          void            * aVectorArg,
                                                          stlErrorStack   * aErrorStack );

stlStatus dtdIntervalYearToMonthSetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdIntervalYearToMonthSetValueFromUtf32WCharString( void                 * aString,
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

stlStatus dtdIntervalYearToMonthToUtf16WCharString( dtlDataValue    * aValue,
                                                    stlInt64          aPrecision,
                                                    stlInt64          aScale,
                                                    stlInt64          aAvailableSize,
                                                    void            * aBuffer,
                                                    stlInt64        * aLength,
                                                    dtlFuncVector   * aVectorFunc,
                                                    void            * aVectorArg,
                                                    stlErrorStack   * aErrorStack );

stlStatus dtdIntervalYearToMonthToUtf32WCharString( dtlDataValue    * aValue,
                                                    stlInt64          aPrecision,
                                                    stlInt64          aScale,
                                                    stlInt64          aAvailableSize,
                                                    void            * aBuffer,
                                                    stlInt64        * aLength,
                                                    dtlFuncVector   * aVectorFunc,
                                                    void            * aVectorArg,
                                                    stlErrorStack   * aErrorStack );


/** @} */

#endif /* _DTDINTERVAL_YEARTOMONTH_H_ */
