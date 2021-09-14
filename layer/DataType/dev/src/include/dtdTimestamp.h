/*******************************************************************************
 * dtdTimestamp.h
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


#ifndef _DTDTIMESTAMP_H_
#define _DTDTIMESTAMP_H_ 1

/**
 * @file dtdTimestamp.h
 * @brief DataType Layer dtlTimestamp Definition
 */

/**
 * @defgroup dtlTimestamp dtlTimestamp
 * @ingroup dtd
 * @{
 */

stlStatus dtdTimestampGetLength( stlInt64              aPrecision,
                                 dtlStringLengthUnit   aStringUnit,
                                 stlInt64            * aLength,
                                 dtlFuncVector       * aVectorFunc,
                                 void                * aVectorArg,
                                 stlErrorStack       * aErrorStack );

stlStatus dtdTimestampGetLengthFromString( stlInt64              aPrecision,
                                           stlInt64              aScale,
                                           dtlStringLengthUnit   aStringUnit,
                                           stlChar             * aString,
                                           stlInt64              aStringLength,
                                           stlInt64            * aLength,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack );

stlStatus dtdTimestampGetLengthFromInteger( stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            stlInt64          aInteger,
                                            stlInt64        * aLength,
                                            stlErrorStack   * aErrorStack );

stlStatus dtdTimestampGetLengthFromReal( stlInt64          aPrecision,
                                         stlInt64          aScale,
                                         stlFloat64        aReal,
                                         stlInt64        * aLength,
                                         stlErrorStack   * aErrorStack );

stlStatus dtdTimestampSetValueFromString( stlChar              * aString,
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

stlStatus dtdTimestampSetValueFromTypedLiteral( stlChar             * aString,
                                                stlInt64              aStringLength,
                                                stlInt64              aAvailableSize,
                                                dtlDataValue        * aValue,
                                                dtlFuncVector       * aVectorFunc,
                                                void                * aVectorArg,
                                                stlErrorStack       * aErrorStack );

stlStatus dtdTimestampSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdTimestampSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdTimestampReverseByteOrder( void            * aValue,
                                        stlBool           aIsSameEndian,
                                        stlErrorStack   * aErrorStack );

stlStatus dtdTimestampToString( dtlDataValue    * aValue,
                                stlInt64          aPrecision,
                                stlInt64          aScale,
                                stlInt64          aAvailableSize,
                                void            * aBuffer,
                                stlInt64        * aLength,
                                dtlFuncVector   * aVectorFunc,
                                void            * aVectorArg,
                                stlErrorStack   * aErrorStack );

stlStatus dtdTimestampToStringForAvailableSize( dtlDataValue    * aValue,
                                                stlInt64          aPrecision,
                                                stlInt64          aScale,
                                                stlInt64          aAvailableSize,
                                                void            * aBuffer,
                                                stlInt64        * aLength,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack );

stlStatus dtdTimestampSetValueFromFormatInfo( stlChar              * aString,
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

stlStatus dtdTimestampSetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdTimestampSetValueFromUtf32WCharString( void                 * aString,
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

stlStatus dtdTimestampToUtf16WCharString( dtlDataValue    * aValue,
                                          stlInt64          aPrecision,
                                          stlInt64          aScale,
                                          stlInt64          aAvailableSize,
                                          void            * aBuffer,
                                          stlInt64        * aLength,
                                          dtlFuncVector   * aVectorFunc,
                                          void            * aVectorArg,
                                          stlErrorStack   * aErrorStack );

stlStatus dtdTimestampToUtf32WCharString( dtlDataValue    * aValue,
                                          stlInt64          aPrecision,
                                          stlInt64          aScale,
                                          stlInt64          aAvailableSize,
                                          void            * aBuffer,
                                          stlInt64        * aLength,
                                          dtlFuncVector   * aVectorFunc,
                                          void            * aVectorArg,
                                          stlErrorStack   * aErrorStack );


/** @} */

#endif /* _DTDTIMESTAMP_H_ */
