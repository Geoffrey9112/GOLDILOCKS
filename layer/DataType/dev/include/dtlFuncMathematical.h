/*******************************************************************************
 * dtlFuncMathematical.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlFuncMathematical.h 1841 2011-09-07 07:14:06Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/
#ifndef _DTL_FUNC_MATHEMATICAL_H_
#define _DTL_FUNC_MATHEMATICAL_H_ 1


/**
 * @file dtlFuncMathematical.h
 * @brief DataType Layer Function 함수 
 */


/*
 * Power
 */

stlStatus dtlBigIntPower( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlNumericPower( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtlDoublePower( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlGetFuncInfoPower( stlUInt16               aDataTypeArrayCount,
                               stlBool               * aIsConstantData,
                               dtlDataType           * aDataTypeArray,
                               dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                               stlUInt16               aFuncArgDataTypeArrayCount,
                               dtlDataType           * aFuncArgDataTypeArray,
                               dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                               dtlDataType           * aFuncResultDataType,
                               dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                               stlUInt32             * aFuncPtrIdx,
                               dtlFuncVector         * aVectorFunc,
                               void                  * aVectorArg,                               
                               stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrPower( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );



/*
 * Sqrt
 */

// stlStatus dtlBigIntSqrt( stlUInt16        aInputArgumentCnt,
//                          dtlValueEntry  * aInputArgument,
//                          void           * aResultValue,
//                          void           * aInfo,
//                          dtlFuncVector  * aVectorFunc,
//                          void           * aVectorArg,
//                          void           * aEnv );

stlStatus dtlNumericSqrt( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlDoubleSqrt( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlGetFuncInfoSqrt( stlUInt16               aDataTypeArrayCount,
                              stlBool               * aIsConstantData,
                              dtlDataType           * aDataTypeArray,
                              dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                              stlUInt16               aFuncArgDataTypeArrayCount,
                              dtlDataType           * aFuncArgDataTypeArray,
                              dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                              dtlDataType           * aFuncResultDataType,
                              dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                              stlUInt32             * aFuncPtrIdx,
                              dtlFuncVector         * aVectorFunc,
                              void                  * aVectorArg,                              
                              stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrSqrt( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );



/*
 * Cbrt
 */

stlStatus dtlBigIntCbrt( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlNumericCbrt( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlDoubleCbrt( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlGetFuncInfoCbrt( stlUInt16               aDataTypeArrayCount,
                              stlBool               * aIsConstantData,
                              dtlDataType           * aDataTypeArray,
                              dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                              stlUInt16               aFuncArgDataTypeArrayCount,
                              dtlDataType           * aFuncArgDataTypeArray,
                              dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                              dtlDataType           * aFuncResultDataType,
                              dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                              stlUInt32             * aFuncPtrIdx,
                              dtlFuncVector         * aVectorFunc,
                              void                  * aVectorArg,                              
                              stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrCbrt( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );


/*
 * Factorial
 */

stlStatus dtlBigIntFact( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlNumericFact( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlGetFuncInfoFactorial( stlUInt16               aDataTypeArrayCount,
                                   stlBool               * aIsConstantData,
                                   dtlDataType           * aDataTypeArray,
                                   dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                   stlUInt16               aFuncArgDataTypeArrayCount,
                                   dtlDataType           * aFuncArgDataTypeArray,
                                   dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                   dtlDataType           * aFuncResultDataType,
                                   dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                   stlUInt32             * aFuncPtrIdx,
                                   dtlFuncVector         * aVectorFunc,
                                   void                  * aVectorArg,                                   
                                   stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrFactorial( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack );



/*
 * Abs
 */

stlStatus dtlAbs( stlUInt16        aInputArgumentCnt,
                  dtlValueEntry  * aInputArgument,
                  void           * aResultValue,
                  void           * aInfo,
                  dtlFuncVector  * aVectorFunc,
                  void           * aVectorArg,
                  void           * aEnv );

stlStatus dtlGetFuncInfoAbs( stlUInt16               aDataTypeArrayCount,
                             stlBool               * aIsConstantData,
                             dtlDataType           * aDataTypeArray,
                             dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                             stlUInt16               aFuncArgDataTypeArrayCount,
                             dtlDataType           * aFuncArgDataTypeArray,
                             dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                             dtlDataType           * aFuncResultDataType,
                             dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                             stlUInt32             * aFuncPtrIdx,
                             dtlFuncVector         * aVectorFunc,
                             void                  * aVectorArg,                             
                             stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrAbs( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );


/*
 * Ceil
 */

stlStatus dtlBigIntCeil( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlDoubleCeil( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlNumericCeil( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlGetFuncInfoCeil( stlUInt16               aDataTypeArrayCount,
                              stlBool               * aIsConstantData,
                              dtlDataType           * aDataTypeArray,
                              dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                              stlUInt16               aFuncArgDataTypeArrayCount,
                              dtlDataType           * aFuncArgDataTypeArray,
                              dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                              dtlDataType           * aFuncResultDataType,
                              dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                              stlUInt32             * aFuncPtrIdx,
                              dtlFuncVector         * aVectorFunc,
                              void                  * aVectorArg,                              
                              stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrCeil( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );


/*
 * Degrees
 */

// stlStatus dtlBigIntDegrees( stlUInt16        aInputArgumentCnt,
//                             dtlValueEntry  * aInputArgument,
//                             void           * aResultValue,
//                             void           * aInfo,
//                             dtlFuncVector  * aVectorFunc,
//                             void           * aVectorArg,
//                             void           * aEnv );

stlStatus dtlNumericDegrees( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv );

stlStatus dtlDoubleDegrees( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );

stlStatus dtlGetFuncInfoDegrees( stlUInt16               aDataTypeArrayCount,
                                 stlBool               * aIsConstantData,
                                 dtlDataType           * aDataTypeArray,
                                 dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                 stlUInt16               aFuncArgDataTypeArrayCount,
                                 dtlDataType           * aFuncArgDataTypeArray,
                                 dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                 dtlDataType           * aFuncResultDataType,
                                 dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                 stlUInt32             * aFuncPtrIdx,
                                 dtlFuncVector         * aVectorFunc,
                                 void                  * aVectorArg,                                 
                                 stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrDegrees( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack );



/*
 * Exp
 */

// stlStatus dtlBigIntExp( stlUInt16        aInputArgumentCnt,
//                         dtlValueEntry  * aInputArgument,
//                         void           * aResultValue,
//                         void           * aInfo,
//                         dtlFuncVector  * aVectorFunc,
//                         void           * aVectorArg,
//                         void           * aEnv );

stlStatus dtlNumericExp( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlDoubleExp( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtlGetFuncInfoExp( stlUInt16               aDataTypeArrayCount,
                             stlBool               * aIsConstantData,
                             dtlDataType           * aDataTypeArray,
                             dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                             stlUInt16               aFuncArgDataTypeArrayCount,
                             dtlDataType           * aFuncArgDataTypeArray,
                             dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                             dtlDataType           * aFuncResultDataType,
                             dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                             stlUInt32             * aFuncPtrIdx,
                             dtlFuncVector         * aVectorFunc,
                             void                  * aVectorArg,                             
                             stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrExp( stlUInt32             aFuncPtrIdx,
                            dtlBuiltInFuncPtr   * aFuncPtr,
                            stlErrorStack       * aErrorStack );


/*
 * Floor
 */

stlStatus dtlBigIntFloor( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlNumericFloor( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtlDoubleFloor( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlGetFuncInfoFloor( stlUInt16               aDataTypeArrayCount,
                               stlBool               * aIsConstantData,
                               dtlDataType           * aDataTypeArray,
                               dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                               stlUInt16               aFuncArgDataTypeArrayCount,
                               dtlDataType           * aFuncArgDataTypeArray,
                               dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                               dtlDataType           * aFuncResultDataType,
                               dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                               stlUInt32             * aFuncPtrIdx,
                               dtlFuncVector         * aVectorFunc,
                               void                  * aVectorArg,                               
                               stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrFloor( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );


/*
 * Ln
 */

stlStatus dtlBigIntLn( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aInfo,
                       dtlFuncVector  * aVectorFunc,
                       void           * aVectorArg,
                       void           * aEnv );

stlStatus dtlNumericLn( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtlDoubleLn( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aInfo,
                       dtlFuncVector  * aVectorFunc,
                       void           * aVectorArg,
                       void           * aEnv );

stlStatus dtlGetFuncInfoLn( stlUInt16               aDataTypeArrayCount,
                            stlBool               * aIsConstantData,
                            dtlDataType           * aDataTypeArray,
                            dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                            stlUInt16               aFuncArgDataTypeArrayCount,
                            dtlDataType           * aFuncArgDataTypeArray,
                            dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                            dtlDataType           * aFuncResultDataType,
                            dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                            stlUInt32             * aFuncPtrIdx,
                            dtlFuncVector         * aVectorFunc,
                            void                  * aVectorArg,                            
                            stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrLn( stlUInt32             aFuncPtrIdx,
                           dtlBuiltInFuncPtr   * aFuncPtr,
                           stlErrorStack       * aErrorStack );


/*
 * Log10
 */

stlStatus dtlBigIntLog10( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlNumericLog10( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtlDoubleLog10( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlGetFuncInfoLog10( stlUInt16               aDataTypeArrayCount,
                               stlBool               * aIsConstantData,
                               dtlDataType           * aDataTypeArray,
                               dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                               stlUInt16               aFuncArgDataTypeArrayCount,
                               dtlDataType           * aFuncArgDataTypeArray,
                               dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                               dtlDataType           * aFuncResultDataType,
                               dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                               stlUInt32             * aFuncPtrIdx,
                               dtlFuncVector         * aVectorFunc,
                               void                  * aVectorArg,                               
                               stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrLog10( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );


/*
 * Log
 */

stlStatus dtlDoubleLog( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtlNumericLog( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlGetFuncInfoLog( stlUInt16               aDataTypeArrayCount,
                             stlBool               * aIsConstantData,
                             dtlDataType           * aDataTypeArray,
                             dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                             stlUInt16               aFuncArgDataTypeArrayCount,
                             dtlDataType           * aFuncArgDataTypeArray,
                             dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                             dtlDataType           * aFuncResultDataType,
                             dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                             stlUInt32             * aFuncPtrIdx,
                             dtlFuncVector         * aVectorFunc,
                             void                  * aVectorArg,                             
                             stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrLog( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );


/*
 * Pi
 */

stlStatus dtlPi( stlUInt16        aInputArgumentCnt,
                 dtlValueEntry  * aInputArgument,
                 void           * aResultValue,
                 void           * aInfo,
                 dtlFuncVector  * aVectorFunc,
                 void           * aVectorArg,
                 void           * aEnv );

stlStatus dtlGetFuncInfoPi( stlUInt16               aDataTypeArrayCount,
                            stlBool               * aIsConstantData,
                            dtlDataType           * aDataTypeArray,
                            dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                            stlUInt16               aFuncArgDataTypeArrayCount,
                            dtlDataType           * aFuncArgDataTypeArray,
                            dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                            dtlDataType           * aFuncResultDataType,
                            dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                            stlUInt32             * aFuncPtrIdx,
                            dtlFuncVector         * aVectorFunc,
                            void                  * aVectorArg,                            
                            stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrPi( stlUInt32             aFuncPtrIdx,
                           dtlBuiltInFuncPtr   * aFuncPtr,
                           stlErrorStack       * aErrorStack );


/*
 * Radians
 */

// stlStatus dtlBigIntRadians( stlUInt16        aInputArgumentCnt,
//                             dtlValueEntry  * aInputArgument,
//                             void           * aResultValue,
//                             void           * aInfo,
//                             dtlFuncVector  * aVectorFunc,
//                             void           * aVectorArg,
//                             void           * aEnv );

stlStatus dtlNumericRadians( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv );

stlStatus dtlDoubleRadians( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );

stlStatus dtlGetFuncInfoRadians( stlUInt16               aDataTypeArrayCount,
                                 stlBool               * aIsConstantData,
                                 dtlDataType           * aDataTypeArray,
                                 dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                 stlUInt16               aFuncArgDataTypeArrayCount,
                                 dtlDataType           * aFuncArgDataTypeArray,
                                 dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                 dtlDataType           * aFuncResultDataType,
                                 dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                 stlUInt32             * aFuncPtrIdx,
                                 dtlFuncVector         * aVectorFunc,
                                 void                  * aVectorArg,                                 
                                 stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrRadians( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack );




/*
 * Random
 */

stlStatus dtlBigIntRandom( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtlNumericRandom( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );

stlStatus dtlDoubleRandom( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtlGetFuncInfoRandom( stlUInt16               aDataTypeArrayCount,
                                stlBool               * aIsConstantData,
                                dtlDataType           * aDataTypeArray,
                                dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                stlUInt16               aFuncArgDataTypeArrayCount,
                                dtlDataType           * aFuncArgDataTypeArray,
                                dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                dtlDataType           * aFuncResultDataType,
                                dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                stlUInt32             * aFuncPtrIdx,
                                dtlFuncVector         * aVectorFunc,
                                void                  * aVectorArg,                                
                                stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrRandom( stlUInt32             aFuncPtrIdx,
                               dtlBuiltInFuncPtr   * aFuncPtr,
                               stlErrorStack       * aErrorStack );



/*
 * Round
 */

stlStatus dtlBigIntRound( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlNumericRound( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtlDoubleRound( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlDateRound( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtlGetFuncInfoRound( stlUInt16               aDataTypeArrayCount,
                               stlBool               * aIsConstantData,
                               dtlDataType           * aDataTypeArray,
                               dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                               stlUInt16               aFuncArgDataTypeArrayCount,
                               dtlDataType           * aFuncArgDataTypeArray,
                               dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                               dtlDataType           * aFuncResultDataType,
                               dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                               stlUInt32             * aFuncPtrIdx,
                               dtlFuncVector         * aVectorFunc,
                               void                  * aVectorArg,                               
                               stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrRound( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );



/*
 * Sign
 */

stlStatus dtlBigIntSign( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlNumericSign( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlDoubleSign( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlGetFuncInfoSign( stlUInt16               aDataTypeArrayCount,
                              stlBool               * aIsConstantData,
                              dtlDataType           * aDataTypeArray,
                              dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                              stlUInt16               aFuncArgDataTypeArrayCount,
                              dtlDataType           * aFuncArgDataTypeArray,
                              dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                              dtlDataType           * aFuncResultDataType,
                              dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                              stlUInt32             * aFuncPtrIdx,
                              dtlFuncVector         * aVectorFunc,
                              void                  * aVectorArg,                              
                              stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrSign( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );




/*
 * Trunc
 */

stlStatus dtlBigIntTrunc( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlNumericTrunc( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtlDoubleTrunc( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlDateTrunc( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtlGetFuncInfoTrunc( stlUInt16               aDataTypeArrayCount,
                               stlBool               * aIsConstantData,
                               dtlDataType           * aDataTypeArray,
                               dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                               stlUInt16               aFuncArgDataTypeArrayCount,
                               dtlDataType           * aFuncArgDataTypeArray,
                               dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                               dtlDataType           * aFuncResultDataType,
                               dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                               stlUInt32             * aFuncPtrIdx,
                               dtlFuncVector         * aVectorFunc,
                               void                  * aVectorArg,                               
                               stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrTrunc( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );



/*
 * WidthBucket
 */
stlStatus dtlBigIntWidthBucket( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

stlStatus dtlNumericWidthBucket( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtlDoubleWidthBucket( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

stlStatus dtlGetFuncInfoWidthBucket( stlUInt16               aDataTypeArrayCount,
                                     stlBool               * aIsConstantData,
                                     dtlDataType           * aDataTypeArray,
                                     dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                     stlUInt16               aFuncArgDataTypeArrayCount,
                                     dtlDataType           * aFuncArgDataTypeArray,
                                     dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                     dtlDataType           * aFuncResultDataType,
                                     dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                     stlUInt32             * aFuncPtrIdx,
                                     dtlFuncVector         * aVectorFunc,
                                     void                  * aVectorArg,                                     
                                     stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrWidthBucket( stlUInt32             aFuncPtrIdx,
                                    dtlBuiltInFuncPtr   * aFuncPtr,
                                    stlErrorStack       * aErrorStack );



/*
 * Acos
 */

// stlStatus dtlBigIntAcos( stlUInt16        aInputArgumentCnt,
//                          dtlValueEntry  * aInputArgument,
//                          void           * aResultValue,
//                          void           * aInfo,
//                          dtlFuncVector  * aVectorFunc,
//                          void           * aVectorArg,
//                          void           * aEnv );

stlStatus dtlNumericAcos( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlDoubleAcos( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlGetFuncInfoAcos( stlUInt16               aDataTypeArrayCount,
                              stlBool               * aIsConstantData,
                              dtlDataType           * aDataTypeArray,
                              dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                              stlUInt16               aFuncArgDataTypeArrayCount,
                              dtlDataType           * aFuncArgDataTypeArray,
                              dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                              dtlDataType           * aFuncResultDataType,
                              dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                              stlUInt32             * aFuncPtrIdx,
                              dtlFuncVector         * aVectorFunc,
                              void                  * aVectorArg,                              
                              stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrAcos( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );



/*
 * Asin
 */

// stlStatus dtlBigIntAsin( stlUInt16        aInputArgumentCnt,
//                          dtlValueEntry  * aInputArgument,
//                          void           * aResultValue,
//                          void           * aInfo,
//                          dtlFuncVector  * aVectorFunc,
//                          void           * aVectorArg,
//                          void           * aEnv );

stlStatus dtlNumericAsin( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlDoubleAsin( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlGetFuncInfoAsin( stlUInt16               aDataTypeArrayCount,
                              stlBool               * aIsConstantData,
                              dtlDataType           * aDataTypeArray,
                              dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                              stlUInt16               aFuncArgDataTypeArrayCount,
                              dtlDataType           * aFuncArgDataTypeArray,
                              dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                              dtlDataType           * aFuncResultDataType,
                              dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                              stlUInt32             * aFuncPtrIdx,
                              dtlFuncVector         * aVectorFunc,
                              void                  * aVectorArg,                              
                              stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrAsin( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );




/*
 * Atan
 */

// stlStatus dtlBigIntAtan( stlUInt16        aInputArgumentCnt,
//                          dtlValueEntry  * aInputArgument,
//                          void           * aResultValue,
//                          void           * aInfo,
//                          dtlFuncVector  * aVectorFunc,
//                          void           * aVectorArg,
//                          void           * aEnv );

stlStatus dtlNumericAtan( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlDoubleAtan( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlGetFuncInfoAtan( stlUInt16               aDataTypeArrayCount,
                              stlBool               * aIsConstantData,
                              dtlDataType           * aDataTypeArray,
                              dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                              stlUInt16               aFuncArgDataTypeArrayCount,
                              dtlDataType           * aFuncArgDataTypeArray,
                              dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                              dtlDataType           * aFuncResultDataType,
                              dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                              stlUInt32             * aFuncPtrIdx,
                              dtlFuncVector         * aVectorFunc,
                              void                  * aVectorArg,                              
                              stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrAtan( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );



/*
 * Atan2
 */

// stlStatus dtlBigIntAtan2( stlUInt16        aInputArgumentCnt,
//                           dtlValueEntry  * aInputArgument,
//                           void           * aResultValue,
//                           void           * aInfo,
//                           dtlFuncVector  * aVectorFunc,
//                           void           * aVectorArg,
//                           void           * aEnv );

stlStatus dtlNumericAtan2( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtlDoubleAtan2( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlGetFuncInfoAtan2( stlUInt16               aDataTypeArrayCount,
                               stlBool               * aIsConstantData,
                               dtlDataType           * aDataTypeArray,
                               dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                               stlUInt16               aFuncArgDataTypeArrayCount,
                               dtlDataType           * aFuncArgDataTypeArray,
                               dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                               dtlDataType           * aFuncResultDataType,
                               dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                               stlUInt32             * aFuncPtrIdx,
                               dtlFuncVector         * aVectorFunc,
                               void                  * aVectorArg,                               
                               stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrAtan2( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );


/*
 * Cot
 */

// stlStatus dtlBigIntCot( stlUInt16        aInputArgumentCnt,
//                         dtlValueEntry  * aInputArgument,
//                         void           * aResultValue,
//                         void           * aInfo,
//                         dtlFuncVector  * aVectorFunc,
//                         void           * aVectorArg,
//                         void           * aEnv );

stlStatus dtlNumericCot( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlDoubleCot( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtlGetFuncInfoCot( stlUInt16               aDataTypeArrayCount,
                             stlBool               * aIsConstantData,
                             dtlDataType           * aDataTypeArray,
                             dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                             stlUInt16               aFuncArgDataTypeArrayCount,
                             dtlDataType           * aFuncArgDataTypeArray,
                             dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                             dtlDataType           * aFuncResultDataType,
                             dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                             stlUInt32             * aFuncPtrIdx,
                             dtlFuncVector         * aVectorFunc,
                             void                  * aVectorArg,                             
                             stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrCot( stlUInt32             aFuncPtrIdx,
                            dtlBuiltInFuncPtr   * aFuncPtr,
                            stlErrorStack       * aErrorStack );




/*
 * Cos
 */

// stlStatus dtlBigIntCos( stlUInt16        aInputArgumentCnt,
//                         dtlValueEntry  * aInputArgument,
//                         void           * aResultValue,
//                         void           * aInfo,
//                         dtlFuncVector  * aVectorFunc,
//                         void           * aVectorArg,
//                         void           * aEnv );

stlStatus dtlNumericCos( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlDoubleCos( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtlGetFuncInfoCos( stlUInt16               aDataTypeArrayCount,
                             stlBool               * aIsConstantData,
                             dtlDataType           * aDataTypeArray,
                             dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                             stlUInt16               aFuncArgDataTypeArrayCount,
                             dtlDataType           * aFuncArgDataTypeArray,
                             dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                             dtlDataType           * aFuncResultDataType,
                             dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                             stlUInt32             * aFuncPtrIdx,
                             dtlFuncVector         * aVectorFunc,
                             void                  * aVectorArg,                             
                             stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrCos( stlUInt32             aFuncPtrIdx,
                            dtlBuiltInFuncPtr   * aFuncPtr,
                            stlErrorStack       * aErrorStack );




/*
 * Sin
 */

// stlStatus dtlBigIntSin( stlUInt16        aInputArgumentCnt,
//                         dtlValueEntry  * aInputArgument,
//                         void           * aResultValue,
//                         void           * aInfo,
//                         dtlFuncVector  * aVectorFunc,
//                         void           * aVectorArg,
//                         void           * aEnv );

stlStatus dtlNumericSin( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlDoubleSin( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtlGetFuncInfoSin( stlUInt16               aDataTypeArrayCount,
                             stlBool               * aIsConstantData,
                             dtlDataType           * aDataTypeArray,
                             dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                             stlUInt16               aFuncArgDataTypeArrayCount,
                             dtlDataType           * aFuncArgDataTypeArray,
                             dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                             dtlDataType           * aFuncResultDataType,
                             dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                             stlUInt32             * aFuncPtrIdx,
                             dtlFuncVector         * aVectorFunc,
                             void                  * aVectorArg,                             
                             stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrSin( stlUInt32             aFuncPtrIdx,
                            dtlBuiltInFuncPtr   * aFuncPtr,
                            stlErrorStack       * aErrorStack );



/*
 * Tan
 */

// stlStatus dtlBigIntTan( stlUInt16        aInputArgumentCnt,
//                         dtlValueEntry  * aInputArgument,
//                         void           * aResultValue,
//                         void           * aInfo,
//                         dtlFuncVector  * aVectorFunc,
//                         void           * aVectorArg,
//                         void           * aEnv );

stlStatus dtlNumericTan( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlDoubleTan( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );


stlStatus dtlGetFuncInfoTan( stlUInt16               aDataTypeArrayCount,
                             stlBool               * aIsConstantData,
                             dtlDataType           * aDataTypeArray,
                             dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                             stlUInt16               aFuncArgDataTypeArrayCount,
                             dtlDataType           * aFuncArgDataTypeArray,
                             dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                             dtlDataType           * aFuncResultDataType,
                             dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                             stlUInt32             * aFuncPtrIdx,
                             dtlFuncVector         * aVectorFunc,
                             void                  * aVectorArg,                             
                             stlErrorStack         * aErrorStack );

stlStatus dtlGetFuncPtrTan( stlUInt32             aFuncPtrIdx,
                            dtlBuiltInFuncPtr   * aFuncPtr,
                            stlErrorStack       * aErrorStack );




#endif /* _DTL_FUNC_MATHEMATICAL_H_ */
