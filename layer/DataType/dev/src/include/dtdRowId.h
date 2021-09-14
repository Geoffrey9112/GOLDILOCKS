/*******************************************************************************
 * dtdRowId.h
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


#ifndef _DTDROWID_H_
#define _DTDROWID_H_ 1

/**
 * @file dtdRowId.h
 * @brief DataType Layer dtlRowId Definition
 */

/**
 * @defgroup dtlRowId dtlRowId
 * @ingroup dtd
 * @{
 */

stlStatus dtdRowIdGetLength( stlInt64              aPrecision,
                             dtlStringLengthUnit   aStringUnit,
                             stlInt64            * aLength,
                             dtlFuncVector       * aVectorFunc,
                             void                * aVectorArg,
                             stlErrorStack       * aErrorStack );

stlStatus dtdRowIdGetLengthFromString( stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64            * aLength,
                                       dtlFuncVector       * aVectorFunc,
                                       void                * aVectorArg,
                                       stlErrorStack       * aErrorStack );

stlStatus dtdRowIdGetLengthFromInteger( stlInt64          aPrecision,
                                        stlInt64          aScale,
                                        stlInt64          aInteger,
                                        stlInt64        * aLength,
                                        stlErrorStack   * aErrorStack );

stlStatus dtdRowIdGetLengthFromReal( stlInt64          aPrecision,
                                     stlInt64          aScale,
                                     stlFloat64        aReal,
                                     stlInt64        * aLength,
                                     stlErrorStack   * aErrorStack );

stlStatus dtdRowIdSetValueFromString( stlChar              * aString,
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

stlStatus dtdRowIdSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtdRowIdSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtdRowIdReverseByteOrder( void            * aValue,
                                    stlBool           aIsSameEndian,
                                    stlErrorStack   * aErrorStack );

stlStatus dtdRowIdToString( dtlDataValue    * aValue,
                            stlInt64          aPrecision,
                            stlInt64          aScale,
                            stlInt64          aAvailableSize,
                            void            * aBuffer,
                            stlInt64        * aLength,
                            dtlFuncVector   * aVectorFunc,
                            void            * aVectorArg,
                            stlErrorStack   * aErrorStack );

stlStatus dtdRowIdToStringForAvailableSize( dtlDataValue    * aValue,
                                            stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            stlInt64          aAvailableSize,
                                            void            * aBuffer,
                                            stlInt64        * aLength,
                                            dtlFuncVector   * aVectorFunc,
                                            void            * aVectorArg,
                                            stlErrorStack   * aErrorStack );

stlStatus dtdRowIdSetValueFromUtf16WCharString( void                 * aString,
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

stlStatus dtdRowIdSetValueFromUtf32WCharString( void                 * aString,
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

stlStatus dtdRowIdToUtf16WCharString( dtlDataValue    * aValue,
                                      stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlInt64          aAvailableSize,
                                      void            * aBuffer,
                                      stlInt64        * aLength,
                                      dtlFuncVector   * aVectorFunc,
                                      void            * aVectorArg,
                                      stlErrorStack   * aErrorStack );

stlStatus dtdRowIdToUtf32WCharString( dtlDataValue    * aValue,
                                      stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlInt64          aAvailableSize,
                                      void            * aBuffer,
                                      stlInt64        * aLength,
                                      dtlFuncVector   * aVectorFunc,
                                      void            * aVectorArg,
                                      stlErrorStack   * aErrorStack );

/** @} */

#endif /* _DTDROWID_H_ */

