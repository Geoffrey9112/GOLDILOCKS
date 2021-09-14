/*******************************************************************************
 * dtdSmallInt.h
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


#ifndef _DTDSMALLINT_H_
#define _DTDSMALLINT_H_ 1

/**
 * @file dtdSmallInt.h
 * @brief DataType Layer dtlSmallInt Definition
 */

/**
 * @defgroup dtlSmallInt dtlSmallInt(2바이트 integer)
 * @ingroup dtd
 * @{
 */

stlStatus dtdSmallIntGetLength( stlInt64              aPrecision,
                                dtlStringLengthUnit   aStringUnit,
                                stlInt64            * aLength,
                                dtlFuncVector       * aVectorFunc,
                                void                * aVectorArg,
                                stlErrorStack       * aErrorStack );

stlStatus dtdSmallIntGetLengthFromString( stlInt64              aPrecision,
                                          stlInt64              aScale,
                                          dtlStringLengthUnit   aStringUnit,
                                          stlChar             * aString,
                                          stlInt64              aStringLength,
                                          stlInt64            * aLength,
                                          dtlFuncVector       * aVectorFunc,
                                          void                * aVectorArg,
                                          stlErrorStack       * aErrorStack );

stlStatus dtdSmallIntGetLengthFromInteger( stlInt64          aPrecision,
                                           stlInt64          aScale,
                                           stlInt64          aInteger,
                                           stlInt64        * aLength,
                                           stlErrorStack   * aErrorStack );

stlStatus dtdSmallIntGetLengthFromReal( stlInt64          aPrecision,
                                        stlInt64          aScale,
                                        stlFloat64        aReal,
                                        stlInt64        * aLength,
                                        stlErrorStack   * aErrorStack );

stlStatus dtdSmallIntSetValueFromString( stlChar              * aString,
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

stlStatus dtdSmallIntSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdSmallIntSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdSmallIntReverseByteOrder( void            * aValue,
                                       stlBool           aIsSameEndian,
                                       stlErrorStack   * aErrorStack );

stlStatus dtdSmallIntToString( dtlDataValue    * aValue,
                               stlInt64          aPrecision,
                               stlInt64          aScale,
                               stlInt64          aAvailableSize,
                               void            * aBuffer,
                               stlInt64        * aLength,
                               dtlFuncVector   * aVectorFunc,
                               void            * aVectorArg,
                               stlErrorStack   * aErrorStack );

stlStatus dtdSmallIntToStringForAvailableSize( dtlDataValue    * aValue,
                                               stlInt64          aPrecision,
                                               stlInt64          aScale,
                                               stlInt64          aAvailableSize,
                                               void            * aBuffer,
                                               stlInt64        * aLength,
                                               dtlFuncVector   * aVectorFunc,
                                               void            * aVectorArg,
                                               stlErrorStack   * aErrorStack );

/** @} */

#endif /* _DTDSMALLINT_H_ */
