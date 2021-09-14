/*******************************************************************************
 * dtfArithmetic.h
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


#ifndef _DTF_ARITHMETIC_H_
#define _DTF_ARITHMETIC_H_ 1

/**
 * @file dtfArithmetic.h
 * @brief Arithmetic for DataType Layer
 */

/**
 * @addtogroup dtfArithmetic Arithmetic
 * @ingroup dtf
 * @{
 */

/*
 * Addition ( + )
 */

stlStatus dtfBigIntAdd( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfDoubleAdd( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfNumericAdd( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDateAddInteger( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfIntegerAddDate( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfDateAddNumeric( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfNumericAddDate( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfDateAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );

stlStatus dtfIntervalYearToMonthAddDate( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );

stlStatus dtfDateAddIntervalDay( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtfDateAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtfIntervalDayAddDate( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtfIntervalDayToSecondAddDate( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtfIntervalYearToMonthAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aEnv );

stlStatus dtfIntervalDayToSecondAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aEnv );

stlStatus dtfTimestampAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aEnv );

stlStatus dtfIntervalYearToMonthAddTimestamp( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aEnv );

stlStatus dtfTimestampAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aEnv );

stlStatus dtfIntervalDayToSecondAddTimestamp( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aEnv );

stlStatus dtfTimestampAddInteger( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aEnv );

stlStatus dtfIntegerAddTimestamp( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aEnv );

stlStatus dtfTimestampAddNumeric( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aEnv );

stlStatus dtfNumericAddTimestamp( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aEnv );

stlStatus dtfTimestampTzAddInteger( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv );

stlStatus dtfIntegerAddTimestampTz( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv );

stlStatus dtfTimestampTzAddNumeric( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv );

stlStatus dtfNumericAddTimestampTz( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv );

stlStatus dtfTimestampTzAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aEnv );

stlStatus dtfTimestampTzAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aEnv );

stlStatus dtfIntervalYearToMonthAddTimestampTz( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aEnv );

stlStatus dtfIntervalDayToSecondAddTimestampTz( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aEnv );

stlStatus dtfTimeAddInteger( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfIntegerAddTime( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfTimeAddNumeric( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfNumericAddTime( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfTimeAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );

stlStatus dtfIntervalYearToMonthAddTime( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );

stlStatus dtfTimeAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );

stlStatus dtfIntervalDayToSecondAddTime( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );

stlStatus dtfTimeTzAddInteger( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv );

stlStatus dtfIntegerAddTimeTz( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv );

stlStatus dtfTimeTzAddNumeric( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv );

stlStatus dtfNumericAddTimeTz( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv );

stlStatus dtfTimeTzAddIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

stlStatus dtfIntervalYearToMonthAddTimeTz( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

stlStatus dtfTimeTzAddIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

stlStatus dtfIntervalDayToSecondAddTimeTz( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

/*
 * Subtraction ( - )
 */

stlStatus dtfBigIntSub( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );

stlStatus dtfNumericSub( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfDoubleSub( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );

stlStatus dtfSmallIntPositive( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtfIntegerPositive( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );

stlStatus dtfBigIntPositive( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv );

stlStatus dtfSmallIntNegative( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,                             
                               void           * aEnv );

stlStatus dtfIntegerNegative( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,                             
                              void           * aEnv );

stlStatus dtfBigIntNegative( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,                             
                             void           * aEnv );

stlStatus dtfDateSubDate( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDateSubInteger( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfDateSubNumeric( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfDateSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );

stlStatus dtfDateSubIntervalDay( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtfDateSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtfTimeSubInteger( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfTimeSubNumeric( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfTimeSubTime( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfTimeSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );

stlStatus dtfTimeSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );

stlStatus dtfTimeTzSubInteger( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv );

stlStatus dtfTimeTzSubNumeric( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv );

stlStatus dtfTimeTzSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

stlStatus dtfTimeTzSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

stlStatus dtfTimestampSubInteger( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aEnv );

stlStatus dtfTimestampSubNumeric( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aEnv );

stlStatus dtfTimestampSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aEnv );

stlStatus dtfTimestampSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aEnv );

stlStatus dtfTimestampTzSubTimestampTz( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aEnv );

stlStatus dtfTimestampTzSubInteger( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv );

stlStatus dtfTimestampTzSubNumeric( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv );

stlStatus dtfTimestampTzSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aEnv );

stlStatus dtfTimestampTzSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aEnv );

stlStatus dtfIntervalYearToMonthSubIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aEnv );

stlStatus dtfIntervalDayToSecondSubIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aEnv );

stlStatus dtfTimestampSubTimestamp( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv );

/*
 * Multiplication ( * )
 */
  
stlStatus dtfBigIntMul( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );

stlStatus dtfNumericMul( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfDoubleMul( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );

stlStatus dtfIntervalYearToMonthMulBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

stlStatus dtfBigIntMulIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

stlStatus dtfIntervalYearToMonthMulNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aEnv );

stlStatus dtfNumericMulIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aEnv );

stlStatus dtfIntervalDayToSecondMulBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

stlStatus dtfBigIntMulIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

stlStatus dtfIntervalDayToSecondMulNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aEnv );

stlStatus dtfNumericMulIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aEnv );

/*
 * Division ( / )
 */ 

stlStatus dtfBigIntDiv( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );

stlStatus dtfDoubleDiv( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );

stlStatus dtfNumericDiv( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );


stlStatus dtfIntervalYearToMonthDivBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

stlStatus dtfIntervalYearToMonthDivNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aEnv );

stlStatus dtfIntervalDayToSecondDivBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aEnv );

stlStatus dtfIntervalDayToSecondDivNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aEnv );


stlStatus dtfBigIntMod( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );

stlStatus dtfRealPositive( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtfDoublePositive( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv );

stlStatus dtfRealNegative( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,                             
                           void           * aEnv );

stlStatus dtfDoubleNegative( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,                             
                             void           * aEnv );

stlStatus dtfDoubleMod( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );

stlStatus dtfNumericPositive( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );

stlStatus dtfNumericNegative( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,                              
                              void           * aEnv );

stlStatus dtfNumericMod( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfNumericIntArrDivisionForMod( stlBool               aLeftIsPositive,
                                          stlInt32              aLeftExponent,
                                          stlInt32              aLeftDigitCount,
                                          stlInt8             * aLeftIntArr,
                                          stlBool               aRightIsPositive,
                                          stlInt32              aRightExponent,
                                          stlInt32              aRightDigitCount,
                                          stlInt8             * aRightIntArr,
                                          stlBool             * aResultIsPositive,
                                          stlInt32            * aResultExponent,
                                          stlInt32            * aResultDigitCount,
                                          stlInt8             * aResultIntArr,
                                          stlErrorStack       * aErrorStack );

stlStatus dtfNumericIntArrMultiplicationForMod( stlBool               aLeftIsPositive,
                                                stlInt32              aLeftExponent,
                                                stlInt32              aLeftDigitCount,
                                                stlInt8             * aLeftIntArr,
                                                stlBool               aRightIsPositive,
                                                stlInt32              aRightExponent,
                                                stlInt32              aRightDigitCount,
                                                stlInt8             * aRightIntArr,
                                                stlInt32              aRightDigitCountAfterDecimal,
                                                stlBool             * aResultIsPositive,
                                                stlInt32            * aResultExponent,
                                                stlInt32            * aResultDigitCount,
                                                stlInt8             * aResultIntArr,
                                                stlErrorStack       * aErrorStack );

stlStatus dtfNumericIntArrSubtractionForMod( stlBool               aLeftIsPositive,
                                             stlInt32              aLeftExponent,
                                             stlInt32              aLeftDigitCount,
                                             stlInt8             * aLeftIntArr,
                                             stlBool               aRightIsPositive,
                                             stlInt32              aRightExponent,
                                             stlInt32              aRightDigitCount,
                                             stlInt8             * aRightIntArr,
                                             stlBool             * aResultIsPositive,
                                             stlInt32            * aResultExponent,
                                             stlInt32            * aResultDigitCount,
                                             stlInt8             * aResultIntArr,
                                             stlErrorStack       * aErrorStack );

stlStatus dtfNumericIntArrAddAbsForMod( stlBool               aLeftIsPositive,
                                        stlInt32              aLeftExponent,
                                        stlInt32              aLeftDigitCount,
                                        stlInt8             * aLeftIntArr,
                                        stlBool               aRightIsPositive,
                                        stlInt32              aRightExponent,
                                        stlInt32              aRightDigitCount,
                                        stlInt8             * aRightIntArr,
                                        stlBool             * aResultIsPositive,
                                        stlInt32            * aResultExponent,
                                        stlInt32            * aResultDigitCount,
                                        stlInt8             * aResultIntArr,
                                        stlErrorStack       * aErrorStack );

stlStatus dtfNumericIntArrSubAbsForMod( stlBool               aLeftIsPositive,
                                        stlInt32              aLeftExponent,
                                        stlInt32              aLeftDigitCount,
                                        stlInt8             * aLeftIntArr,
                                        stlBool               aRightIsPositive,
                                        stlInt32              aRightExponent,
                                        stlInt32              aRightDigitCount,
                                        stlInt8             * aRightIntArr,
                                        stlBool             * aResultIsPositive,
                                        stlInt32            * aResultExponent,
                                        stlInt32            * aResultDigitCount,
                                        stlInt8             * aResultIntArr,
                                        stlErrorStack       * aErrorStack );

stlInt32 dtfNumericIntArrCmpAbsForMod( stlInt32              aLeftExponent,
                                       stlInt32              aLeftDigitCount,
                                       stlInt8             * aLeftIntArr,
                                       stlInt32              aRightExponent,
                                       stlInt32              aRightDigitCount,
                                       stlInt8             * aRightIntArr );

void dtfIntArrRemoveLeadingAndTrailingZeroForMod( stlBool             * aIsPositive,
                                                  stlInt32            * aExponent,
                                                  stlInt32            * aDigitCount,
                                                  stlInt8             * aIntArr );


/*
 * DATEADD
 */

stlStatus dtfDateAddForDateAddDatepart( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtfDateAddForTimestampAddDatepart( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtfDateAddForTimestampWithTimeZoneAddDatepart( stlUInt16        aInputArgumentCnt,
                                                         dtlValueEntry  * aInputArgument,
                                                         void           * aResultValue,
                                                         dtlFuncVector  * aVectorFunc,
                                                         void           * aVectorArg,
                                                         void           * aEnv );

stlStatus dtfDateAddForTimeAddDatepart( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtfDateAddForTimeWithTimeZoneAddDatepart( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

/*
 * DATEDIFF
 */

stlStatus dtfDateDiffForDate( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );

stlStatus dtfDateDiffForTime( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );

stlStatus dtfDateDiffForTimestamp( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   void           * aInfo,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv );

stlStatus dtfDateDiffForTimestampWithTimeZone( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

/** @} */
    
#endif /* _DTF_ARITHMETIC_H_ */
