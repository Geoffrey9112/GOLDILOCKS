/*******************************************************************************
 * dtlDataType.h
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
#ifndef _DTL_DATA_TYPE_H_
#define _DTL_DATA_TYPE_H_ 1


/**
 * @file dtlDataType.h
 * @brief DataType Layer 데이타 타입 제어 
 */

/**
 * @addtogroup dtlDataTypeInformation
 * @{
 */

stlStatus dtlGetCharLengthUnit( stlChar             * aName,
                                dtlStringLengthUnit * aCharLengthUnit,
                                stlErrorStack       * aErrorStack );

stlStatus dtlIsStaticType( dtlDataType       aType,
                           stlBool         * aIsStatic,
                           stlErrorStack   * aErrorStack );


stlInt32  dtlGetDisplaySize( dtlDataType            aType,
                             stlInt64               aPrecision,
                             stlInt64               aScale,
                             dtlIntervalIndicator   aIntervalIndicator,
                             dtlFuncVector        * aVectorFunc,
                             void                 * aVectorArg );


/*****************************************************************
 * Wrapping Functions (for Conversion)
 *****************************************************************/

/**
 * Set Value From String
 */

stlStatus dtlBooleanSetValueFromString( stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        dtlIntervalIndicator  aIntervalIndicator,
                                        stlInt64              aAvailableSize,
                                        dtlDataValue        * aValue,
                                        stlBool             * aSuccessWithInfo,
                                        dtlFuncVector       * aSourceVectorFunc,
                                        void                * aSourceVectorArg,
                                        dtlFuncVector       * aDestVectorFunc,
                                        void                * aDestVectorArg,
                                        stlErrorStack       * aErrorStack );

stlStatus dtlSmallIntSetValueFromString( stlChar             * aString,
                                         stlInt64              aStringLength,
                                         stlInt64              aPrecision,
                                         stlInt64              aScale,
                                         dtlStringLengthUnit   aStringUnit,
                                         dtlIntervalIndicator  aIntervalIndicator,
                                         stlInt64              aAvailableSize,
                                         dtlDataValue        * aValue,
                                         stlBool             * aSuccessWithInfo,
                                         dtlFuncVector       * aSourceVectorFunc,
                                         void                * aSourceVectorArg,
                                         dtlFuncVector       * aDestVectorFunc,
                                         void                * aDestVectorArg,
                                         stlErrorStack       * aErrorStack );

stlStatus dtlIntegerSetValueFromString( stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        dtlIntervalIndicator  aIntervalIndicator,
                                        stlInt64              aAvailableSize,
                                        dtlDataValue        * aValue,
                                        stlBool             * aSuccessWithInfo,
                                        dtlFuncVector       * aSourceVectorFunc,
                                        void                * aSourceVectorArg,
                                        dtlFuncVector       * aDestVectorFunc,
                                        void                * aDestVectorArg,
                                        stlErrorStack       * aErrorStack );

stlStatus dtlBigIntSetValueFromString( stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       dtlIntervalIndicator  aIntervalIndicator,
                                       stlInt64              aAvailableSize,
                                       dtlDataValue        * aValue,
                                       stlBool             * aSuccessWithInfo,
                                       dtlFuncVector       * aSourceVectorFunc,
                                       void                * aSourceVectorArg,
                                       dtlFuncVector       * aDestVectorFunc,
                                       void                * aDestVectorArg,
                                       stlErrorStack       * aErrorStack );

stlStatus dtlRealSetValueFromString( stlChar             * aString,
                                     stlInt64              aStringLength,
                                     stlInt64              aPrecision,
                                     stlInt64              aScale,
                                     dtlStringLengthUnit   aStringUnit,
                                     dtlIntervalIndicator  aIntervalIndicator,
                                     stlInt64              aAvailableSize,
                                     dtlDataValue        * aValue,
                                     stlBool             * aSuccessWithInfo,
                                     dtlFuncVector       * aSourceVectorFunc,
                                     void                * aSourceVectorArg,
                                     dtlFuncVector       * aDestVectorFunc,
                                     void                * aDestVectorArg,
                                     stlErrorStack       * aErrorStack );

stlStatus dtlDoubleSetValueFromString( stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       dtlIntervalIndicator  aIntervalIndicator,
                                       stlInt64              aAvailableSize,
                                       dtlDataValue        * aValue,
                                       stlBool             * aSuccessWithInfo,
                                       dtlFuncVector       * aSourceVectorFunc,
                                       void                * aSourceVectorArg,
                                       dtlFuncVector       * aDestVectorFunc,
                                       void                * aDestVectorArg,
                                       stlErrorStack       * aErrorStack );

stlStatus dtlNumericSetValueFromString( stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        dtlIntervalIndicator  aIntervalIndicator,
                                        stlInt64              aAvailableSize,
                                        dtlDataValue        * aValue,
                                        stlBool             * aSuccessWithInfo,
                                        dtlFuncVector       * aSourceVectorFunc,
                                        void                * aSourceVectorArg,
                                        dtlFuncVector       * aDestVectorFunc,
                                        void                * aDestVectorArg,
                                        stlErrorStack       * aErrorStack );

stlStatus dtlFloatSetValueFromString( stlChar             * aString,
                                      stlInt64              aStringLength,
                                      stlInt64              aPrecision,
                                      stlInt64              aScale,
                                      dtlStringLengthUnit   aStringUnit,
                                      dtlIntervalIndicator  aIntervalIndicator,
                                      stlInt64              aAvailableSize,
                                      dtlDataValue        * aValue,
                                      stlBool             * aSuccessWithInfo,
                                      dtlFuncVector       * aSourceVectorFunc,
                                      void                * aSourceVectorArg,
                                      dtlFuncVector       * aDestVectorFunc,
                                      void                * aDestVectorArg,
                                      stlErrorStack       * aErrorStack );

stlStatus dtlCharSetValueFromString( stlChar             * aString,
                                     stlInt64              aStringLength,
                                     stlInt64              aPrecision,
                                     stlInt64              aScale,
                                     dtlStringLengthUnit   aStringUnit,
                                     dtlIntervalIndicator  aIntervalIndicator,
                                     stlInt64              aAvailableSize,
                                     dtlDataValue        * aValue,
                                     stlBool             * aSuccessWithInfo,
                                     dtlFuncVector       * aSourceVectorFunc,
                                     void                * aSourceVectorArg,
                                     dtlFuncVector       * aDestVectorFunc,
                                     void                * aDestVectorArg,
                                     stlErrorStack       * aErrorStack );

stlStatus dtlCharSetValueFromStringWithoutPaddNull( stlChar             * aString,
                                                    stlInt64              aStringLength,
                                                    stlInt64              aPrecision,
                                                    stlInt64              aScale,
                                                    dtlStringLengthUnit   aStringUnit,
                                                    dtlIntervalIndicator  aIntervalIndicator,
                                                    stlInt64              aAvailableSize,
                                                    dtlDataValue        * aValue,
                                                    stlBool             * aSuccessWithInfo,
                                                    dtlFuncVector       * aSourceVectorFunc,
                                                    void                * aSourceVectorArg,
                                                    dtlFuncVector       * aDestVectorFunc,
                                                    void                * aDestVectorArg,
                                                    stlErrorStack       * aErrorStack );

stlStatus dtlVarcharSetValueFromString( stlChar             * aString,
                                        stlInt64              aStringLength,
                                        stlInt64              aPrecision,
                                        stlInt64              aScale,
                                        dtlStringLengthUnit   aStringUnit,
                                        dtlIntervalIndicator  aIntervalIndicator,
                                        stlInt64              aAvailableSize,
                                        dtlDataValue        * aValue,
                                        stlBool             * aSuccessWithInfo,
                                        dtlFuncVector       * aSourceVectorFunc,
                                        void                * aSourceVectorArg,
                                        dtlFuncVector       * aDestVectorFunc,
                                        void                * aDestVectorArg,
                                        stlErrorStack       * aErrorStack );

stlStatus dtlLongvarcharSetValueFromString( stlChar             * aString,
                                            stlInt64              aStringLength,
                                            stlInt64              aPrecision,
                                            stlInt64              aScale,
                                            dtlStringLengthUnit   aStringUnit,
                                            dtlIntervalIndicator  aIntervalIndicator,
                                            stlInt64              aAvailableSize,
                                            dtlDataValue        * aValue,
                                            stlBool             * aSuccessWithInfo,
                                            dtlFuncVector       * aSourceVectorFunc,
                                            void                * aSourceVectorArg,
                                            dtlFuncVector       * aDestVectorFunc,
                                            void                * aDestVectorArg,
                                            stlErrorStack       * aErrorStack );

stlStatus dtlBinarySetValueFromString( stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       dtlIntervalIndicator  aIntervalIndicator,
                                       stlInt64              aAvailableSize,
                                       dtlDataValue        * aValue,
                                       stlBool             * aSuccessWithInfo,
                                       dtlFuncVector       * aSourceVectorFunc,
                                       void                * aSourceVectorArg,
                                       dtlFuncVector       * aDestVectorFunc,
                                       void                * aDestVectorArg,
                                       stlErrorStack       * aErrorStack );

stlStatus dtlBinarySetValueFromStringWithoutPaddNull( stlChar             * aString,
                                                      stlInt64              aStringLength,
                                                      stlInt64              aPrecision,
                                                      stlInt64              aScale,
                                                      dtlStringLengthUnit   aStringUnit,
                                                      dtlIntervalIndicator  aIntervalIndicator,
                                                      stlInt64              aAvailableSize,
                                                      dtlDataValue        * aValue,
                                                      stlBool             * aSuccessWithInfo,
                                                      dtlFuncVector       * aSourceVectorFunc,
                                                      void                * aSourceVectorArg,
                                                      dtlFuncVector       * aDestVectorFunc,
                                                      void                * aDestVectorArg,
                                                      stlErrorStack       * aErrorStack );

stlStatus dtlVarbinarySetValueFromString( stlChar             * aString,
                                          stlInt64              aStringLength,
                                          stlInt64              aPrecision,
                                          stlInt64              aScale,
                                          dtlStringLengthUnit   aStringUnit,
                                          dtlIntervalIndicator  aIntervalIndicator,
                                          stlInt64              aAvailableSize,
                                          dtlDataValue        * aValue,
                                          stlBool             * aSuccessWithInfo,
                                          dtlFuncVector       * aSourceVectorFunc,
                                          void                * aSourceVectorArg,
                                          dtlFuncVector       * aDestVectorFunc,
                                          void                * aDestVectorArg,
                                          stlErrorStack       * aErrorStack );

stlStatus dtlLongvarbinarySetValueFromString( stlChar             * aString,
                                              stlInt64              aStringLength,
                                              stlInt64              aPrecision,
                                              stlInt64              aScale,
                                              dtlStringLengthUnit   aStringUnit,
                                              dtlIntervalIndicator  aIntervalIndicator,
                                              stlInt64              aAvailableSize,
                                              dtlDataValue        * aValue,
                                              stlBool             * aSuccessWithInfo,
                                              dtlFuncVector       * aSourceVectorFunc,
                                              void                * aSourceVectorArg,
                                              dtlFuncVector       * aDestVectorFunc,
                                              void                * aDestVectorArg,
                                              stlErrorStack       * aErrorStack );

stlStatus dtlDateSetValueFromString( stlChar             * aString,
                                     stlInt64              aStringLength,
                                     stlInt64              aPrecision,
                                     stlInt64              aScale,
                                     dtlStringLengthUnit   aStringUnit,
                                     dtlIntervalIndicator  aIntervalIndicator,
                                     stlInt64              aAvailableSize,
                                     dtlDataValue        * aValue,
                                     stlBool             * aSuccessWithInfo,
                                     dtlFuncVector       * aSourceVectorFunc,
                                     void                * aSourceVectorArg,
                                     dtlFuncVector       * aDestVectorFunc,
                                     void                * aDestVectorArg,
                                     stlErrorStack       * aErrorStack );

stlStatus dtlTimeSetValueFromString( stlChar             * aString,
                                     stlInt64              aStringLength,
                                     stlInt64              aPrecision,
                                     stlInt64              aScale,
                                     dtlStringLengthUnit   aStringUnit,
                                     dtlIntervalIndicator  aIntervalIndicator,
                                     stlInt64              aAvailableSize,
                                     dtlDataValue        * aValue,
                                     stlBool             * aSuccessWithInfo,
                                     dtlFuncVector       * aSourceVectorFunc,
                                     void                * aSourceVectorArg,
                                     dtlFuncVector       * aDestVectorFunc,
                                     void                * aDestVectorArg,
                                     stlErrorStack       * aErrorStack );

stlStatus dtlTimestampSetValueFromString( stlChar             * aString,
                                          stlInt64              aStringLength,
                                          stlInt64              aPrecision,
                                          stlInt64              aScale,
                                          dtlStringLengthUnit   aStringUnit,
                                          dtlIntervalIndicator  aIntervalIndicator,
                                          stlInt64              aAvailableSize,
                                          dtlDataValue        * aValue,
                                          stlBool             * aSuccessWithInfo,
                                          dtlFuncVector       * aSourceVectorFunc,
                                          void                * aSourceVectorArg,
                                          dtlFuncVector       * aDestVectorFunc,
                                          void                * aDestVectorArg,
                                          stlErrorStack       * aErrorStack );

stlStatus dtlTimeTzSetValueFromString( stlChar             * aString,
                                       stlInt64              aStringLength,
                                       stlInt64              aPrecision,
                                       stlInt64              aScale,
                                       dtlStringLengthUnit   aStringUnit,
                                       dtlIntervalIndicator  aIntervalIndicator,
                                       stlInt64              aAvailableSize,
                                       dtlDataValue        * aValue,
                                       stlBool             * aSuccessWithInfo,
                                       dtlFuncVector       * aSourceVectorFunc,
                                       void                * aSourceVectorArg,
                                       dtlFuncVector       * aDestVectorFunc,
                                       void                * aDestVectorArg,
                                       stlErrorStack       * aErrorStack );

stlStatus dtlTimestampTzSetValueFromString( stlChar             * aString,
                                            stlInt64              aStringLength,
                                            stlInt64              aPrecision,
                                            stlInt64              aScale,
                                            dtlStringLengthUnit   aStringUnit,
                                            dtlIntervalIndicator  aIntervalIndicator,
                                            stlInt64              aAvailableSize,
                                            dtlDataValue        * aValue,
                                            stlBool             * aSuccessWithInfo,
                                            dtlFuncVector       * aSourceVectorFunc,
                                            void                * aSourceVectorArg,
                                            dtlFuncVector       * aDestVectorFunc,
                                            void                * aDestVectorArg,
                                            stlErrorStack       * aErrorStack );

stlStatus dtlIntervalYearToMonthSetValueFromString( stlChar             * aString,
                                                    stlInt64              aStringLength,
                                                    stlInt64              aPrecision,
                                                    stlInt64              aScale,
                                                    dtlStringLengthUnit   aStringUnit,
                                                    dtlIntervalIndicator  aIntervalIndicator,
                                                    stlInt64              aAvailableSize,
                                                    dtlDataValue        * aValue,
                                                    stlBool             * aSuccessWithInfo,
                                                    dtlFuncVector       * aSourceVectorFunc,
                                                    void                * aSourceVectorArg,
                                                    dtlFuncVector       * aDestVectorFunc,
                                                    void                * aDestVectorArg,
                                                    stlErrorStack       * aErrorStack );

stlStatus dtlIntervalDayToSecondSetValueFromString( stlChar             * aString,
                                                    stlInt64              aStringLength,
                                                    stlInt64              aPrecision,
                                                    stlInt64              aScale,
                                                    dtlStringLengthUnit   aStringUnit,
                                                    dtlIntervalIndicator  aIntervalIndicator,
                                                    stlInt64              aAvailableSize,
                                                    dtlDataValue        * aValue,
                                                    stlBool             * aSuccessWithInfo,
                                                    dtlFuncVector       * aSourceVectorFunc,
                                                    void                * aSourceVectorArg,
                                                    dtlFuncVector       * aDestVectorFunc,
                                                    void                * aDestVectorArg,
                                                    stlErrorStack       * aErrorStack );

stlStatus dtlRowIdSetValueFromString( stlChar             * aString,
                                      stlInt64              aStringLength,
                                      stlInt64              aPrecision,
                                      stlInt64              aScale,
                                      dtlStringLengthUnit   aStringUnit,
                                      dtlIntervalIndicator  aIntervalIndicator,
                                      stlInt64              aAvailableSize,
                                      dtlDataValue        * aValue,
                                      stlBool             * aSuccessWithInfo,
                                      dtlFuncVector       * aSourceVectorFunc,
                                      void                * aSourceVectorArg,
                                      dtlFuncVector       * aDestVectorFunc,
                                      void                * aDestVectorArg,
                                      stlErrorStack       * aErrorStack );


/**
 * Set Value From Integer
 */

stlStatus dtlBooleanSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlSmallIntSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlIntegerSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlBigIntSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlRealSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlDoubleSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlNumericSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlFloatSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlCharSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlCharSetValueFromIntegerWithoutPaddNull( stlInt64               aInteger,
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

stlStatus dtlVarcharSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlLongvarcharSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlBinarySetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlVarbinarySetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlLongvarbinarySetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlIntervalYearToMonthSetValueFromInteger( stlInt64               aInteger,
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

stlStatus dtlIntervalDayToSecondSetValueFromInteger( stlInt64               aInteger,
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


/**
 * Set Value From Real
 */

stlStatus dtlSmallIntSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlIntegerSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlBigIntSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlRealSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlDoubleSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlNumericSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlFloatSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlCharSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlCharSetValueFromRealWithoutPaddNull( stlFloat64             aReal,
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

stlStatus dtlVarcharSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlLongvarcharSetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlBinarySetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlVarbinarySetValueFromReal( stlFloat64             aReal,
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

stlStatus dtlLongvarbinarySetValueFromReal( stlFloat64             aReal,
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

/**
 * Set Value From WCharString
 */

stlStatus dtlBooleanSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                             void                * aString,
                                             stlInt64              aStringLength,
                                             stlInt64              aPrecision,
                                             stlInt64              aScale,
                                             dtlStringLengthUnit   aStringUnit,
                                             dtlIntervalIndicator  aIntervalIndicator,
                                             stlInt64              aAvailableSize,
                                             dtlDataValue        * aValue,
                                             stlBool             * aSuccessWithInfo,
                                             dtlFuncVector       * aVectorFunc,
                                             void                * aVectorArg,
                                             stlErrorStack       * aErrorStack );

stlStatus dtlNumericSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                             void                * aString,
                                             stlInt64              aStringLength,
                                             stlInt64              aPrecision,
                                             stlInt64              aScale,
                                             dtlStringLengthUnit   aStringUnit,
                                             dtlIntervalIndicator  aIntervalIndicator,
                                             stlInt64              aAvailableSize,
                                             dtlDataValue        * aValue,
                                             stlBool             * aSuccessWithInfo,
                                             dtlFuncVector       * aVectorFunc,
                                             void                * aVectorArg,
                                             stlErrorStack       * aErrorStack );

stlStatus dtlFloatSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                           void                * aString,
                                           stlInt64              aStringLength,
                                           stlInt64              aPrecision,
                                           stlInt64              aScale,
                                           dtlStringLengthUnit   aStringUnit,
                                           dtlIntervalIndicator  aIntervalIndicator,
                                           stlInt64              aAvailableSize,
                                           dtlDataValue        * aValue,
                                           stlBool             * aSuccessWithInfo,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack );

stlStatus dtlCharSetValueFromWCharStringWithoutPaddNull( dtlUnicodeEncoding    aUnicodeEncoding,
                                                         void                * aString,
                                                         stlInt64              aStringLength,
                                                         stlInt64              aPrecision,
                                                         stlInt64              aScale,
                                                         dtlStringLengthUnit   aStringUnit,
                                                         dtlIntervalIndicator  aIntervalIndicator,
                                                         stlInt64              aAvailableSize,
                                                         dtlDataValue        * aValue,
                                                         stlBool             * aSuccessWithInfo,
                                                         dtlFuncVector       * aVectorFunc,
                                                         void                * aVectorArg,
                                                         stlErrorStack       * aErrorStack );

stlStatus dtlVarcharSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                             void                * aString,
                                             stlInt64              aStringLength,
                                             stlInt64              aPrecision,
                                             stlInt64              aScale,
                                             dtlStringLengthUnit   aStringUnit,
                                             dtlIntervalIndicator  aIntervalIndicator,
                                             stlInt64              aAvailableSize,
                                             dtlDataValue        * aValue,
                                             stlBool             * aSuccessWithInfo,
                                             dtlFuncVector       * aVectorFunc,
                                             void                * aVectorArg,
                                             stlErrorStack       * aErrorStack );

stlStatus dtlLongvarcharSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                                 void                * aString,
                                                 stlInt64              aStringLength,
                                                 stlInt64              aPrecision,
                                                 stlInt64              aScale,
                                                 dtlStringLengthUnit   aStringUnit,
                                                 dtlIntervalIndicator  aIntervalIndicator,
                                                 stlInt64              aAvailableSize,
                                                 dtlDataValue        * aValue,
                                                 stlBool             * aSuccessWithInfo,
                                                 dtlFuncVector       * aVectorFunc,
                                                 void                * aVectorArg,
                                                 stlErrorStack       * aErrorStack );

stlStatus dtlBinarySetValueFromWCharStringWithoutPaddNull( dtlUnicodeEncoding    aUnicodeEncoding,
                                                           void                * aString,
                                                           stlInt64              aStringLength,
                                                           stlInt64              aPrecision,
                                                           stlInt64              aScale,
                                                           dtlStringLengthUnit   aStringUnit,
                                                           dtlIntervalIndicator  aIntervalIndicator,
                                                           stlInt64              aAvailableSize,
                                                           dtlDataValue        * aValue,
                                                           stlBool             * aSuccessWithInfo,
                                                           dtlFuncVector       * aVectorFunc,
                                                           void                * aVectorArg,
                                                           stlErrorStack       * aErrorStack );

stlStatus dtlVarbinarySetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                               void                * aString,
                                               stlInt64              aStringLength,
                                               stlInt64              aPrecision,
                                               stlInt64              aScale,
                                               dtlStringLengthUnit   aStringUnit,
                                               dtlIntervalIndicator  aIntervalIndicator,
                                               stlInt64              aAvailableSize,
                                               dtlDataValue        * aValue,
                                               stlBool             * aSuccessWithInfo,
                                               dtlFuncVector       * aVectorFunc,
                                               void                * aVectorArg,
                                               stlErrorStack       * aErrorStack );

stlStatus dtlLongvarbinarySetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                                   void                * aString,
                                                   stlInt64              aStringLength,
                                                   stlInt64              aPrecision,
                                                   stlInt64              aScale,
                                                   dtlStringLengthUnit   aStringUnit,
                                                   dtlIntervalIndicator  aIntervalIndicator,
                                                   stlInt64              aAvailableSize,
                                                   dtlDataValue        * aValue,
                                                   stlBool             * aSuccessWithInfo,
                                                   dtlFuncVector       * aVectorFunc,
                                                   void                * aVectorArg,
                                                   stlErrorStack       * aErrorStack );

stlStatus dtlDateSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                          void                * aString,
                                          stlInt64              aStringLength,
                                          stlInt64              aPrecision,
                                          stlInt64              aScale,
                                          dtlStringLengthUnit   aStringUnit,
                                          dtlIntervalIndicator  aIntervalIndicator,
                                          stlInt64              aAvailableSize,
                                          dtlDataValue        * aValue,
                                          stlBool             * aSuccessWithInfo,
                                          dtlFuncVector       * aVectorFunc,
                                          void                * aVectorArg,
                                          stlErrorStack       * aErrorStack );

stlStatus dtlTimeSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                          void                * aString,
                                          stlInt64              aStringLength,
                                          stlInt64              aPrecision,
                                          stlInt64              aScale,
                                          dtlStringLengthUnit   aStringUnit,
                                          dtlIntervalIndicator  aIntervalIndicator,
                                          stlInt64              aAvailableSize,
                                          dtlDataValue        * aValue,
                                          stlBool             * aSuccessWithInfo,
                                          dtlFuncVector       * aVectorFunc,
                                          void                * aVectorArg,
                                          stlErrorStack       * aErrorStack );

stlStatus dtlTimestampSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                               void                * aString,
                                               stlInt64              aStringLength,
                                               stlInt64              aPrecision,
                                               stlInt64              aScale,
                                               dtlStringLengthUnit   aStringUnit,
                                               dtlIntervalIndicator  aIntervalIndicator,
                                               stlInt64              aAvailableSize,
                                               dtlDataValue        * aValue,
                                               stlBool             * aSuccessWithInfo,
                                               dtlFuncVector       * aVectorFunc,
                                               void                * aVectorArg,
                                               stlErrorStack       * aErrorStack );

stlStatus dtlTimeTzSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                            void                * aString,
                                            stlInt64              aStringLength,
                                            stlInt64              aPrecision,
                                            stlInt64              aScale,
                                            dtlStringLengthUnit   aStringUnit,
                                            dtlIntervalIndicator  aIntervalIndicator,
                                            stlInt64              aAvailableSize,
                                            dtlDataValue        * aValue,
                                            stlBool             * aSuccessWithInfo,
                                            dtlFuncVector       * aVectorFunc,
                                            void                * aVectorArg,
                                            stlErrorStack       * aErrorStack );

stlStatus dtlTimestampTzSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                                 void                * aString,
                                                 stlInt64              aStringLength,
                                                 stlInt64              aPrecision,
                                                 stlInt64              aScale,
                                                 dtlStringLengthUnit   aStringUnit,
                                                 dtlIntervalIndicator  aIntervalIndicator,
                                                 stlInt64              aAvailableSize,
                                                 dtlDataValue        * aValue,
                                                 stlBool             * aSuccessWithInfo,
                                                 dtlFuncVector       * aVectorFunc,
                                                 void                * aVectorArg,
                                                 stlErrorStack       * aErrorStack );

stlStatus dtlIntervalYearToMonthSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                                         void                * aString,
                                                         stlInt64              aStringLength,
                                                         stlInt64              aPrecision,
                                                         stlInt64              aScale,
                                                         dtlStringLengthUnit   aStringUnit,
                                                         dtlIntervalIndicator  aIntervalIndicator,
                                                         stlInt64              aAvailableSize,
                                                         dtlDataValue        * aValue,
                                                         stlBool             * aSuccessWithInfo,
                                                         dtlFuncVector       * aVectorFunc,
                                                         void                * aVectorArg,
                                                         stlErrorStack       * aErrorStack );

stlStatus dtlIntervalDayToSecondSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                                         void                * aString,
                                                         stlInt64              aStringLength,
                                                         stlInt64              aPrecision,
                                                         stlInt64              aScale,
                                                         dtlStringLengthUnit   aStringUnit,
                                                         dtlIntervalIndicator  aIntervalIndicator,
                                                         stlInt64              aAvailableSize,
                                                         dtlDataValue        * aValue,
                                                         stlBool             * aSuccessWithInfo,
                                                         dtlFuncVector       * aVectorFunc,
                                                         void                * aVectorArg,
                                                         stlErrorStack       * aErrorStack );

stlStatus dtlRowIdSetValueFromWCharString( dtlUnicodeEncoding    aUnicodeEncoding,
                                           void                * aString,
                                           stlInt64              aStringLength,
                                           stlInt64              aPrecision,
                                           stlInt64              aScale,
                                           dtlStringLengthUnit   aStringUnit,
                                           dtlIntervalIndicator  aIntervalIndicator,
                                           stlInt64              aAvailableSize,
                                           dtlDataValue        * aValue,
                                           stlBool             * aSuccessWithInfo,
                                           dtlFuncVector       * aVectorFunc,
                                           void                * aVectorArg,
                                           stlErrorStack       * aErrorStack );




stlBool dtlCheckKeyCompareType( dtlDataType  aType );


stlStatus dtlGetResultTypeForComparisonRule( stlUInt16               aDataTypeArrayCount,
                                             dtlDataType           * aDataTypeArray,
                                             dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                             dtlDataType           * aResultType,
                                             dtlDataTypeDefInfo    * aResultDefInfo,
                                             stlErrorStack         * aErrorStack );
    
void dtlComparisonResultType( dtlDataType        * aResultType,
                              dtlDataTypeDefInfo * aResultDefInfo,
                              dtlDataType          aLeftType,
                              dtlDataTypeDefInfo * aLeftDefInfo,
                              dtlDataType          aRightType,
                              dtlDataTypeDefInfo * aRightDefInfo );

stlStatus dtlVaildateDataTypeInfo( dtlDataType             aType,
                                   stlInt64                aPrecision,
                                   stlInt64                aScale,
                                   dtlStringLengthUnit     aStringLengthUnit,
                                   dtlIntervalIndicator    aIntervalIndicator,
                                   stlErrorStack         * aErrorStack );

/** @} */

#endif /* _DTL_DATA_TYPE_H_ */
