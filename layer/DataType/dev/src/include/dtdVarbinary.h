/*******************************************************************************
 * dtdVarbinary.h
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

#ifndef _DTDVARBINARY_H_
#define _DTDVARBINARY_H_ 1

/**
 * @file dtdVarbinary.h
 * @brief DataType Layer dtlVarbinary Definition
 */

/**
 * @defgroup dtlVarbinary dtlVarbinary
 * @ingroup dtd
 * @{
 */

stlStatus dtdVarbinaryGetLength( stlInt64              aPrecision,
                                 dtlStringLengthUnit   aStringUnit,
                                 stlInt64            * aLength,
                                 dtlFuncVector       * aVectorFunc,
                                 void                * aVectorArg,
                                 stlErrorStack       * aErrorStack );

stlStatus dtdVarbinaryGetLengthFromString( stlInt64              aPrecision,
                                           stlInt64              aScale,
                                           dtlStringLengthUnit   aStringUnit,
                                           stlChar             * aString,
                                           stlInt64              aStringLength,
                                           stlInt64            * aLength,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack );

stlStatus dtdVarbinaryGetLengthFromInteger( stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            stlInt64          aInteger,
                                            stlInt64        * aLength,
                                            stlErrorStack   * aErrorStack );

stlStatus dtdVarbinaryGetLengthFromReal( stlInt64          aPrecision,
                                         stlInt64          aScale,
                                         stlFloat64        aReal,
                                         stlInt64        * aLength,
                                         stlErrorStack   * aErrorStack );

stlStatus dtdVarbinarySetValueFromString( stlChar              * aString,
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

stlStatus dtdVarbinarySetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdVarbinarySetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdVarbinaryReverseByteOrder( void            * aValue,
                                        stlBool           aIsSameEndian,
                                        stlErrorStack   * aErrorStack );

stlStatus dtdVarbinaryToString( dtlDataValue    * aValue,
                                stlInt64          aPrecision,
                                stlInt64          aScale,
                                stlInt64          aAvailableSize,
                                void            * aBuffer,
                                stlInt64        * aLength,
                                dtlFuncVector   * aVectorFunc,
                                void            * aVectorArg,
                                stlErrorStack   * aErrorStack );

stlStatus dtdVarbinaryToStringForAvailableSize( dtlDataValue    * aValue,
                                                stlInt64          aPrecision,
                                                stlInt64          aScale,
                                                stlInt64          aAvailableSize,
                                                void            * aBuffer,
                                                stlInt64        * aLength,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack );

/** @} */

#endif /* _DTDVARBINARY_H_ */
