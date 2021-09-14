/*******************************************************************************
 * dtdDate.h
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


#ifndef _DTDDATE_H_
#define _DTDDATE_H_ 1

/**
 * @file dtdDate.h
 * @brief DataType Layer dtlDate Definition
 */

/**
 * @defgroup dtlDate dtlDate
 * @ingroup dtd
 * @{
 */

stlStatus dtdDateGetLength( stlInt64              aPrecision,
                            dtlStringLengthUnit   aStringUnit,
                            stlInt64            * aLength,
                            dtlFuncVector       * aVectorFunc,
                            void                * aVectorArg,
                            stlErrorStack       * aErrorStack );

stlStatus dtdDateGetLengthFromString( stlInt64              aPrecision,
                                      stlInt64              aScale,
                                      dtlStringLengthUnit   aStringUnit,
                                      stlChar             * aString,
                                      stlInt64              aStringLength,
                                      stlInt64            * aLength,
                                      dtlFuncVector       * aVectorFunc,
                                      void                * aVectorArg,
                                      stlErrorStack       * aErrorStack );

stlStatus dtdDateGetLengthFromInteger( stlInt64          aPrecision,
                                       stlInt64          aScale,
                                       stlInt64          aInteger,
                                       stlInt64        * aLength,
                                       stlErrorStack   * aErrorStack );

stlStatus dtdDateGetLengthFromReal( stlInt64          aPrecision,
                                    stlInt64          aScale,
                                    stlFloat64        aReal,
                                    stlInt64        * aLength,
                                    stlErrorStack   * aErrorStack );

stlStatus dtdDateSetValueFromString( stlChar              * aString,
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

stlStatus dtdDateSetValueFromTypedLiteral( stlChar             * aString,
                                           stlInt64              aStringLength,
                                           stlInt64              aAvailableSize,
                                           dtlDataValue        * aValue,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack );

stlStatus dtdDateSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdDateSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdDateReverseByteOrder( void            * aValue,
                                   stlBool           aIsSameEndian,
                                   stlErrorStack   * aErrorStack );

stlStatus dtdDateToString( dtlDataValue    * aValue,
                           stlInt64          aPrecision,
                           stlInt64          aScale,
                           stlInt64          aAvailableSize,
                           void            * aBuffer,
                           stlInt64        * aLength,
                           dtlFuncVector   * aVectorFunc,
                           void            * aVectorArg,
                           stlErrorStack   * aErrorStack );

stlStatus dtdDateToStringForAvailableSize( dtlDataValue    * aValue,
                                           stlInt64          aPrecision,
                                           stlInt64          aScale,
                                           stlInt64          aAvailableSize,
                                           void            * aBuffer,
                                           stlInt64        * aLength,
                                           dtlFuncVector   * aVectorFunc,
                                           void            * aVectorArg,
                                           stlErrorStack   * aErrorStack );

stlStatus dtdDateSetValueFromFormatInfo( stlChar              * aString,
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

stlStatus dtdDateSetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdDateSetValueFromUtf32WCharString( void                 * aString,
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

stlStatus dtdDateToUtf16WCharString( dtlDataValue    * aValue,
                                     stlInt64          aPrecision,
                                     stlInt64          aScale,
                                     stlInt64          aAvailableSize,
                                     void            * aBuffer,
                                     stlInt64        * aLength,
                                     dtlFuncVector   * aVectorFunc,
                                     void            * aVectorArg,
                                     stlErrorStack   * aErrorStack );

stlStatus dtdDateToUtf32WCharString( dtlDataValue    * aValue,
                                     stlInt64          aPrecision,
                                     stlInt64          aScale,
                                     stlInt64          aAvailableSize,
                                     void            * aBuffer,
                                     stlInt64        * aLength,
                                     dtlFuncVector   * aVectorFunc,
                                     void            * aVectorArg,
                                     stlErrorStack   * aErrorStack );

/** @} */

#endif /* _DTDDATE_H_ */
