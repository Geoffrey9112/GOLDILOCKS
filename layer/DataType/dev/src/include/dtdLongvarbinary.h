/*******************************************************************************
 * dtdLongvarbinary.h
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


#ifndef _DTDLONGVARBINARY_H_
#define _DTDLONGVARBINARY_H_ 1

/**
 * @file dtdLongvarbinary.h
 * @brief DataType Layer dtlLongvarbinary Definition
 */

/**
 * @defgroup dtlLongvarbinary dtlLongvarbinary
 * @ingroup dtd
 * @{
 */

stlStatus dtdLongvarbinaryGetLength( stlInt64              aPrecision,
                                     dtlStringLengthUnit   aStringUnit,
                                     stlInt64            * aLength,
                                     dtlFuncVector       * aVectorFunc,
                                     void                * aVectorArg,
                                     stlErrorStack       * aErrorStack );

stlStatus dtdLongvarbinaryGetLengthFromString( stlInt64              aPrecision,
                                               stlInt64              aScale,
                                               dtlStringLengthUnit   aStringUnit,
                                               stlChar             * aString,
                                               stlInt64              aStringLength,
                                               stlInt64            * aLength,
                                               dtlFuncVector       * aVectorFunc,
                                               void                * aVectorArg,
                                               stlErrorStack       * aErrorStack );

stlStatus dtdLongvarbinaryGetLengthFromInteger( stlInt64          aPrecision,
                                                stlInt64          aScale,
                                                stlInt64          aInteger,
                                                stlInt64        * aLength,
                                                stlErrorStack   * aErrorStack );

stlStatus dtdLongvarbinaryGetLengthFromReal( stlInt64          aPrecision,
                                             stlInt64          aScale,
                                             stlFloat64        aReal,
                                             stlInt64        * aLength,
                                             stlErrorStack   * aErrorStack );

stlStatus dtdLongvarbinarySetValueFromString( stlChar              * aString,
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

stlStatus dtdLongvarbinarySetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdLongvarbinarySetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdLongvarbinaryReverseByteOrder( void            * aValue,
                                            stlBool           aIsSameEndian,
                                            stlErrorStack   * aErrorStack );

stlStatus dtdLongvarbinaryToString( dtlDataValue    * aValue,
                                    stlInt64          aPrecision,
                                    stlInt64          aScale,
                                    stlInt64          aAvailableSize,
                                    void            * aBuffer,
                                    stlInt64        * aLength,
                                    dtlFuncVector   * aVectorFunc,
                                    void            * aVectorArg,
                                    stlErrorStack   * aErrorStack );

stlStatus dtdLongvarbinaryToStringForAvailableSize( dtlDataValue    * aValue,
                                                    stlInt64          aPrecision,
                                                    stlInt64          aScale,
                                                    stlInt64          aAvailableSize,
                                                    void            * aBuffer,
                                                    stlInt64        * aLength,
                                                    dtlFuncVector   * aVectorFunc,
                                                    void            * aVectorArg,
                                                    stlErrorStack   * aErrorStack );

stlStatus dtdLongvarbinarySetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdLongvarbinarySetValueFromUtf32WCharString( void                 * aString,
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

#endif /* _DTDLONGVARBINARY_H_ */
