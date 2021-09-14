/*******************************************************************************
 * dtlOperCompareType.h
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
#ifndef _DTL_OPER_COMPARE_TYPE_H_
#define _DTL_OPER_COMPARE_TYPE_H_ 1


/**
 * @file dtlOperCompareType.h
 * @brief DataType Layer의 Data Type간 비교 함수
 */

/**
 * @defgroup dtlOperCompareType Data Type간 비교 함수
 * @ingroup dtlOperCompare
 * @{
 */

/*******************************************************************************
 * COMPARE INLINE 함수
 ******************************************************************************/

/* CHAR */
inline stlBool dtlIsEqualFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 dtlCharType  aRightVal,
                                                 stlInt64     aRightLen );

inline stlBool dtlIsNotEqualFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    dtlCharType  aRightVal,
                                                    stlInt64     aRightLen );

inline stlBool dtlIsLessThanFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    dtlCharType  aRightVal,
                                                    stlInt64     aRightLen );

inline stlBool dtlIsLessThanEqualFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         dtlCharType  aRightVal,
                                                         stlInt64     aRightLen );

inline stlBool dtlIsGreaterThanFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       dtlCharType  aRightVal,
                                                       stlInt64     aRightLen );

inline stlBool dtlIsGreaterThanEqualFixedLengthChar_INLINE( dtlCharType  aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            dtlCharType  aRightVal,
                                                            stlInt64     aRightLen );

/* BINARY */
inline stlBool dtlIsEqualFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                   stlInt64       aLeftLen,
                                                   dtlBinaryType  aRightVal,
                                                   stlInt64       aRightLen );

inline stlBool dtlIsNotEqualFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                      stlInt64       aLeftLen,
                                                      dtlBinaryType  aRightVal,
                                                      stlInt64       aRightLen );

inline stlBool dtlIsLessThanFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                      stlInt64       aLeftLen,
                                                      dtlBinaryType  aRightVal,
                                                      stlInt64       aRightLen );

inline stlBool dtlIsLessThanEqualFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                           stlInt64       aLeftLen,
                                                           dtlBinaryType  aRightVal,
                                                           stlInt64       aRightLen );

inline stlBool dtlIsGreaterThanFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                         stlInt64       aLeftLen,
                                                         dtlBinaryType  aRightVal,
                                                         stlInt64       aRightLen );

inline stlBool dtlIsGreaterThanEqualFixedLengthBinary_INLINE( dtlBinaryType  aLeftVal,
                                                              stlInt64       aLeftLen,
                                                              dtlBinaryType  aRightVal,
                                                              stlInt64       aRightLen );

/* NUMERIC : IS EQUAL*/
inline stlBool dtlIsEqualNumericToSmallInt_INLINE( dtlNumericType  * aLeftVal,
                                                   stlInt64          aLeftLen,
                                                   dtlSmallIntType   aRightVal,
                                                   stlInt64          aRightLen );

inline stlBool dtlIsEqualNumericToInteger_INLINE( dtlNumericType  * aLeftVal,
                                                  stlInt64          aLeftLen,
                                                  dtlIntegerType    aRightVal,
                                                  stlInt64          aRightLen );

inline stlBool dtlIsEqualNumericToBigInt_INLINE( dtlNumericType  * aLeftVal,
                                                 stlInt64          aLeftLen,
                                                 dtlBigIntType     aRightVal,
                                                 stlInt64          aRightLen );

/* NUMERIC : IS NOT EQUAL */
inline stlBool dtlIsNotEqualNumericToSmallInt_INLINE( dtlNumericType  * aLeftVal,
                                                      stlInt64          aLeftLen,
                                                      dtlSmallIntType   aRightVal,
                                                      stlInt64          aRightLen );

inline stlBool dtlIsNotEqualNumericToInteger_INLINE( dtlNumericType  * aLeftVal,
                                                     stlInt64          aLeftLen,
                                                     dtlIntegerType    aRightVal,
                                                     stlInt64          aRightLen );

inline stlBool dtlIsNotEqualNumericToBigInt_INLINE( dtlNumericType  * aLeftVal,
                                                    stlInt64          aLeftLen,
                                                    dtlBigIntType     aRightVal,
                                                    stlInt64          aRightLen );

/* NUMERIC : IS LESS THAN */
inline stlBool dtlIsLessThanSmallIntToNumeric_INLINE( dtlSmallIntType   aLeftVal,
                                                      stlInt64          aLeftLen,
                                                      dtlNumericType  * aRightVal,
                                                      stlInt64          aRightLen );

inline stlBool dtlIsLessThanIntegerToNumeric_INLINE( dtlIntegerType    aLeftVal,
                                                     stlInt64          aLeftLen,
                                                     dtlNumericType  * aRightVal,
                                                     stlInt64          aRightLen );

inline stlBool dtlIsLessThanBigIntToNumeric_INLINE( dtlBigIntType     aLeftVal,
                                                    stlInt64          aLeftLen,
                                                    dtlNumericType  * aRightVal,
                                                    stlInt64          aRightLen );

/* NUMERIC : IS LESS THAN EQUAL */
inline stlBool dtlIsLessThanEqualSmallIntToNumeric_INLINE( dtlSmallIntType   aLeftVal,
                                                           stlInt64          aLeftLen,
                                                           dtlNumericType  * aRightVal,
                                                           stlInt64          aRightLen );

inline stlBool dtlIsLessThanEqualIntegerToNumeric_INLINE( dtlIntegerType    aLeftVal,
                                                          stlInt64          aLeftLen,
                                                          dtlNumericType  * aRightVal,
                                                          stlInt64          aRightLen );

inline stlBool dtlIsLessThanEqualBigIntToNumeric_INLINE( dtlBigIntType     aLeftVal,
                                                         stlInt64          aLeftLen,
                                                         dtlNumericType  * aRightVal,
                                                         stlInt64          aRightLen );

/* NUMERIC : IS GREATER THAN */
inline stlBool dtlIsGreaterThanSmallIntToNumeric_INLINE( dtlSmallIntType   aLeftVal,
                                                         stlInt64          aLeftLen,
                                                         dtlNumericType  * aRightVal,
                                                         stlInt64          aRightLen );

inline stlBool dtlIsGreaterThanIntegerToNumeric_INLINE( dtlIntegerType    aLeftVal,
                                                        stlInt64          aLeftLen,
                                                        dtlNumericType  * aRightVal,
                                                        stlInt64          aRightLen );

inline stlBool dtlIsGreaterThanBigIntToNumeric_INLINE( dtlBigIntType     aLeftVal,
                                                       stlInt64          aLeftLen,
                                                       dtlNumericType  * aRightVal,
                                                       stlInt64          aRightLen );

/* NUMERIC : IS GREATER THAN EQUAL */
inline stlBool dtlIsGreaterThanEqualSmallIntToNumeric_INLINE( dtlSmallIntType   aLeftVal,
                                                              stlInt64          aLeftLen,
                                                              dtlNumericType  * aRightVal,
                                                              stlInt64          aRightLen );

inline stlBool dtlIsGreaterThanEqualIntegerToNumeric_INLINE( dtlIntegerType    aLeftVal,
                                                             stlInt64          aLeftLen,
                                                             dtlNumericType  * aRightVal,
                                                             stlInt64          aRightLen );

inline stlBool dtlIsGreaterThanEqualBigIntToNumeric_INLINE( dtlBigIntType     aLeftVal,
                                                            stlInt64          aLeftLen,
                                                            dtlNumericType  * aRightVal,
                                                            stlInt64          aRightLen );

inline stlBool dtlIsEqualNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                               stlInt64          aLeftLen,
                                               dtlRealType       aRightVal,
                                               stlInt64          aRightLen );

inline stlBool dtlIsNotEqualNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                                  stlInt64          aLeftLen,
                                                  dtlRealType       aRightVal,
                                                  stlInt64          aRightLen );

inline stlBool dtlIsLessThanNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                                  stlInt64          aLeftLen,
                                                  dtlRealType       aRightVal,
                                                  stlInt64          aRightLen );

inline stlBool dtlIsLessThanEqualNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                                       stlInt64          aLeftLen,
                                                       dtlRealType       aRightVal,
                                                       stlInt64          aRightLen );

inline stlBool dtlIsGreaterThanNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                                     stlInt64          aLeftLen,
                                                     dtlRealType       aRightVal,
                                                     stlInt64          aRightLen );

inline stlBool dtlIsGreaterThanEqualNumericToReal_INLINE( dtlNumericType  * aLeftVal,
                                                          stlInt64          aLeftLen,
                                                          dtlRealType       aRightVal,
                                                          stlInt64          aRightLen );

inline stlBool dtlIsEqualNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                 stlInt64          aLeftLen,
                                                 dtlDoubleType     aRightVal,
                                                 stlInt64          aRightLen );

inline stlBool dtlIsNotEqualNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                    stlInt64          aLeftLen,
                                                    dtlDoubleType     aRightVal,
                                                    stlInt64          aRightLen );

inline stlBool dtlIsLessThanNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                    stlInt64          aLeftLen,
                                                    dtlDoubleType     aRightVal,
                                                    stlInt64          aRightLen );

inline stlBool dtlIsLessThanEqualNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                         stlInt64          aLeftLen,
                                                         dtlDoubleType     aRightVal,
                                                         stlInt64          aRightLen );

inline stlBool dtlIsGreaterThanNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                       stlInt64          aLeftLen,
                                                       dtlDoubleType     aRightVal,
                                                       stlInt64          aRightLen );

inline stlBool dtlIsGreaterThanEqualNumericToDouble_INLINE( dtlNumericType  * aLeftVal,
                                                            stlInt64          aLeftLen,
                                                            dtlDoubleType     aRightVal,
                                                            stlInt64          aRightLen );

// inline stlBool dtlIsEqualNumericToNumeric_INLINE( dtlNumericType  * aLeftVal,
//                                                   stlInt64          aLeftLen,
//                                                   dtlNumericType  * aRightVal,
//                                                   stlInt64          aRightLen );

// inline stlBool dtlIsNotEqualNumericToNumeric_INLINE( dtlNumericType  * aLeftVal,
//                                                      stlInt64          aLeftLen,
//                                                      dtlNumericType  * aRightVal,
//                                                      stlInt64          aRightLen );

// inline stlBool dtlIsLessThanNumericToNumeric_INLINE( dtlNumericType  * aLeftVal,
//                                                      stlInt64          aLeftLen,
//                                                      dtlNumericType  * aRightVal,
//                                                      stlInt64          aRightLen );

// inline stlBool dtlIsLessThanEqualNumericToNumeric_INLINE( dtlNumericType  * aLeftVal,
//                                                           stlInt64          aLeftLen,
//                                                           dtlNumericType  * aRightVal,
//                                                           stlInt64          aRightLen );

// inline stlBool dtlIsGreaterThanNumericToNumeric_INLINE( dtlNumericType  * aLeftVal,
//                                                         stlInt64          aLeftLen,
//                                                         dtlNumericType  * aRightVal,
//                                                         stlInt64          aRightLen );

// inline stlBool dtlIsGreaterThanEqualNumericToNumeric_INLINE( dtlNumericType  * aLeftVal,
//                                                              stlInt64          aLeftLen,
//                                                              dtlNumericType  * aRightVal,
//                                                              stlInt64          aRightLen );

/*******************************************************************************
 * BINARY INTEGER (TYPE)
 ******************************************************************************/

/* Boolean vs Boolean */
stlStatus dtlOperIsEqualBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsNotEqualBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

stlStatus dtlOperIsLessThanBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsLessThanEqualBooleanToBoolean( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

/* Small Int vs Small Int */
stlStatus dtlOperIsEqualSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsNotEqualSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                       dtlValueEntry  * aInputArgument,
                                                       void           * aResultValue,
                                                       void           * aInfo,
                                                       dtlFuncVector  * aVectorFunc,
                                                       void           * aVectorArg,
                                                       void           * aEnv );

stlStatus dtlOperIsLessThanSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsLessThanEqualSmallIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

/* Small Int vs Integer */
stlStatus dtlOperIsEqualSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsNotEqualSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );

stlStatus dtlOperIsLessThanSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsLessThanEqualSmallIntToInteger( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

/* Small Int vs Big Int */
stlStatus dtlOperIsEqualSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsNotEqualSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

stlStatus dtlOperIsLessThanSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsLessThanEqualSmallIntToBigInt( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );


/* Integer vs Small Int */
stlStatus dtlOperIsEqualIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsNotEqualIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );

stlStatus dtlOperIsLessThanIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsLessThanEqualIntegerToSmallInt( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

/* Integer vs Integer */
stlStatus dtlOperIsEqualIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsNotEqualIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

stlStatus dtlOperIsLessThanIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsLessThanEqualIntegerToInteger( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

/* Integer vs Big Int */
stlStatus dtlOperIsEqualIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsNotEqualIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

stlStatus dtlOperIsLessThanIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanEqualIntegerToBigInt( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );


/* Big Int vs Small Int */
stlStatus dtlOperIsEqualBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsNotEqualBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

stlStatus dtlOperIsLessThanBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsLessThanEqualBigIntToSmallInt( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

/* Big Int vs Integer */
stlStatus dtlOperIsEqualBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsNotEqualBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

stlStatus dtlOperIsLessThanBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanEqualBigIntToInteger( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

/* Big Int vs Big Int */
stlStatus dtlOperIsEqualBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtlOperIsNotEqualBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsGreaterThanBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

stlStatus dtlOperIsLessThanBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanEqualBigIntToBigInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

/*******************************************************************************
 * BINARY REAL (TYPE)
 ******************************************************************************/

/* Real vs Real */
stlStatus dtlOperIsEqualRealToReal( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtlOperIsNotEqualRealToReal( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtlOperIsGreaterThanRealToReal( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualRealToReal( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsLessThanRealToReal( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtlOperIsLessThanEqualRealToReal( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

/* Real vs Double */
stlStatus dtlOperIsEqualRealToDouble( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );

stlStatus dtlOperIsNotEqualRealToDouble( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsGreaterThanRealToDouble( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualRealToDouble( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsLessThanRealToDouble( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsLessThanEqualRealToDouble( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );


/* Double vs Real */
stlStatus dtlOperIsEqualDoubleToReal( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );

stlStatus dtlOperIsNotEqualDoubleToReal( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsGreaterThanDoubleToReal( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualDoubleToReal( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsLessThanDoubleToReal( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsLessThanEqualDoubleToReal( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

/* Double vs Double */
stlStatus dtlOperIsEqualDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtlOperIsNotEqualDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsGreaterThanDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

stlStatus dtlOperIsLessThanDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanEqualDoubleToDouble( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

/* Float : not exist */

/*******************************************************************************
 * NUMERIC (TYPE)
 ******************************************************************************/

/* Numeric vs SmallInt */
stlStatus dtlOperIsEqualNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsNotEqualNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );

stlStatus dtlOperIsLessThanNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsLessThanEqualNumericToSmallInt( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

/* SmallInt vs Numeric */
stlStatus dtlOperIsEqualSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsNotEqualSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );

stlStatus dtlOperIsLessThanSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsLessThanEqualSmallIntToNumeric( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

/* Numeric vs Integer */
stlStatus dtlOperIsEqualNumericToInteger( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsNotEqualNumericToInteger( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanNumericToInteger( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualNumericToInteger( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

stlStatus dtlOperIsLessThanNumericToInteger( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsLessThanEqualNumericToInteger( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

/* Integer vs Numeric */
stlStatus dtlOperIsEqualIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsNotEqualIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

stlStatus dtlOperIsLessThanIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsLessThanEqualIntegerToNumeric( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

/* Numeric vs BigInt */
stlStatus dtlOperIsEqualNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsNotEqualNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

stlStatus dtlOperIsLessThanNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanEqualNumericToBigInt( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

/* BigInt vs Numeric */
stlStatus dtlOperIsEqualBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsNotEqualBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

stlStatus dtlOperIsLessThanBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanEqualBigIntToNumeric( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

/* Numeric vs Real */
stlStatus dtlOperIsEqualNumericToReal( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtlOperIsNotEqualNumericToReal( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsGreaterThanNumericToReal( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualNumericToReal( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

stlStatus dtlOperIsLessThanNumericToReal( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsLessThanEqualNumericToReal( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

/* Real vs Numeric */
stlStatus dtlOperIsEqualRealToNumeric( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtlOperIsNotEqualRealToNumeric( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsGreaterThanRealToNumeric( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualRealToNumeric( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

stlStatus dtlOperIsLessThanRealToNumeric( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsLessThanEqualRealToNumeric( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

/* Numeric vs Double */
stlStatus dtlOperIsEqualNumericToDouble( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsNotEqualNumericToDouble( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanNumericToDouble( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualNumericToDouble( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

stlStatus dtlOperIsLessThanNumericToDouble( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanEqualNumericToDouble( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

/* Double vs Numeric */
stlStatus dtlOperIsEqualDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsNotEqualDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

stlStatus dtlOperIsLessThanDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanEqualDoubleToNumeric( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

/* Numeric vs Numeric */
stlStatus dtlOperIsEqualNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsNotEqualNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

stlStatus dtlOperIsLessThanNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsLessThanEqualNumericToNumeric( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

/* Decimal : not exist */

/*******************************************************************************
 * CHARACTER STRING (TYPE)
 ******************************************************************************/

/* 고정길이 Character String type간의 비교 */
stlStatus dtlOperIsEqualFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsNotEqualFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

stlStatus dtlOperIsLessThanFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanEqualFixedLengthChar( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

/* 가변길이 Character String type을 포함한 비교 */
stlStatus dtlOperIsEqualVariableLengthChar( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsNotEqualVariableLengthChar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanVariableLengthChar( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualVariableLengthChar( stlUInt16        aInputArgumentCnt,
                                                       dtlValueEntry  * aInputArgument,
                                                       void           * aResultValue,
                                                       void           * aInfo,
                                                       dtlFuncVector  * aVectorFunc,
                                                       void           * aVectorArg,
                                                       void           * aEnv );

stlStatus dtlOperIsLessThanVariableLengthChar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsLessThanEqualVariableLengthChar( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

/* long variable Character String type을 포함한 비교 */
stlStatus dtlOperIsEqualCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );
stlStatus dtlOperIsEqualLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );
stlStatus dtlOperIsEqualVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );
stlStatus dtlOperIsEqualLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );
stlStatus dtlOperIsEqualLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

stlStatus dtlOperIsNotEqualCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );
stlStatus dtlOperIsNotEqualLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );
stlStatus dtlOperIsNotEqualVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );
stlStatus dtlOperIsNotEqualLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );
stlStatus dtlOperIsNotEqualLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );

stlStatus dtlOperIsGreaterThanCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );
stlStatus dtlOperIsGreaterThanLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );
stlStatus dtlOperIsGreaterThanVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );
stlStatus dtlOperIsGreaterThanLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );
stlStatus dtlOperIsGreaterThanLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                         dtlValueEntry  * aInputArgument,
                                                         void           * aResultValue,
                                                         void           * aInfo,
                                                         dtlFuncVector  * aVectorFunc,
                                                         void           * aVectorArg,
                                                         void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                       dtlValueEntry  * aInputArgument,
                                                       void           * aResultValue,
                                                       void           * aInfo,
                                                       dtlFuncVector  * aVectorFunc,
                                                       void           * aVectorArg,
                                                       void           * aEnv );
stlStatus dtlOperIsGreaterThanEqualLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
                                                       dtlValueEntry  * aInputArgument,
                                                       void           * aResultValue,
                                                       void           * aInfo,
                                                       dtlFuncVector  * aVectorFunc,
                                                       void           * aVectorArg,
                                                       void           * aEnv );
stlStatus dtlOperIsGreaterThanEqualVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                          dtlValueEntry  * aInputArgument,
                                                          void           * aResultValue,
                                                          void           * aInfo,
                                                          dtlFuncVector  * aVectorFunc,
                                                          void           * aVectorArg,
                                                          void           * aEnv );
stlStatus dtlOperIsGreaterThanEqualLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
                                                          dtlValueEntry  * aInputArgument,
                                                          void           * aResultValue,
                                                          void           * aInfo,
                                                          dtlFuncVector  * aVectorFunc,
                                                          void           * aVectorArg,
                                                          void           * aEnv );
stlStatus dtlOperIsGreaterThanEqualLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                              dtlValueEntry  * aInputArgument,
                                                              void           * aResultValue,
                                                              void           * aInfo,
                                                              dtlFuncVector  * aVectorFunc,
                                                              void           * aVectorArg,
                                                              void           * aEnv );

stlStatus dtlOperIsLessThanCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );
stlStatus dtlOperIsLessThanLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );
stlStatus dtlOperIsLessThanVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );
stlStatus dtlOperIsLessThanLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );
stlStatus dtlOperIsLessThanLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );

stlStatus dtlOperIsLessThanEqualCharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );
stlStatus dtlOperIsLessThanEqualLongvarcharAndChar( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );
stlStatus dtlOperIsLessThanEqualVarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                       dtlValueEntry  * aInputArgument,
                                                       void           * aResultValue,
                                                       void           * aInfo,
                                                       dtlFuncVector  * aVectorFunc,
                                                       void           * aVectorArg,
                                                       void           * aEnv );
stlStatus dtlOperIsLessThanEqualLongvarcharAndVarchar( stlUInt16        aInputArgumentCnt,
                                                       dtlValueEntry  * aInputArgument,
                                                       void           * aResultValue,
                                                       void           * aInfo,
                                                       dtlFuncVector  * aVectorFunc,
                                                       void           * aVectorArg,
                                                       void           * aEnv );
stlStatus dtlOperIsLessThanEqualLongvarcharAndLongvarchar( stlUInt16        aInputArgumentCnt,
                                                           dtlValueEntry  * aInputArgument,
                                                           void           * aResultValue,
                                                           void           * aInfo,
                                                           dtlFuncVector  * aVectorFunc,
                                                           void           * aVectorArg,
                                                           void           * aEnv );

/* Character Large Object : not exist */

/*******************************************************************************
 * BINARY STRING (GROUP)
 ******************************************************************************/

/* 고정길이 Binary String type간의 비교 */
stlStatus dtlOperIsEqualFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsNotEqualFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );

stlStatus dtlOperIsLessThanFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsLessThanEqualFixedLengthBinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

/* 가변길이 Binary String type을 포함한 비교 */
stlStatus dtlOperIsEqualVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsNotEqualVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsGreaterThanVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                                         dtlValueEntry  * aInputArgument,
                                                         void           * aResultValue,
                                                         void           * aInfo,
                                                         dtlFuncVector  * aVectorFunc,
                                                         void           * aVectorArg,
                                                         void           * aEnv );

stlStatus dtlOperIsLessThanVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsLessThanEqualVariableLengthBinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );

/* long variable binary string type을 포함한 비교 */
stlStatus dtlOperIsEqualBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );
stlStatus dtlOperIsEqualLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );
stlStatus dtlOperIsEqualVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );
stlStatus dtlOperIsEqualLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );
stlStatus dtlOperIsEqualLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                       dtlValueEntry  * aInputArgument,
                                                       void           * aResultValue,
                                                       void           * aInfo,
                                                       dtlFuncVector  * aVectorFunc,
                                                       void           * aVectorArg,
                                                       void           * aEnv );

stlStatus dtlOperIsNotEqualBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );
stlStatus dtlOperIsNotEqualLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );
stlStatus dtlOperIsNotEqualVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );
stlStatus dtlOperIsNotEqualLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );
stlStatus dtlOperIsNotEqualLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                          dtlValueEntry  * aInputArgument,
                                                          void           * aResultValue,
                                                          void           * aInfo,
                                                          dtlFuncVector  * aVectorFunc,
                                                          void           * aVectorArg,
                                                          void           * aEnv );

stlStatus dtlOperIsGreaterThanBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );
stlStatus dtlOperIsGreaterThanLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );
stlStatus dtlOperIsGreaterThanVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                         dtlValueEntry  * aInputArgument,
                                                         void           * aResultValue,
                                                         void           * aInfo,
                                                         dtlFuncVector  * aVectorFunc,
                                                         void           * aVectorArg,
                                                         void           * aEnv );
stlStatus dtlOperIsGreaterThanLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                         dtlValueEntry  * aInputArgument,
                                                         void           * aResultValue,
                                                         void           * aInfo,
                                                         dtlFuncVector  * aVectorFunc,
                                                         void           * aVectorArg,
                                                         void           * aEnv );
stlStatus dtlOperIsGreaterThanLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                             dtlValueEntry  * aInputArgument,
                                                             void           * aResultValue,
                                                             void           * aInfo,
                                                             dtlFuncVector  * aVectorFunc,
                                                             void           * aVectorArg,
                                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                           dtlValueEntry  * aInputArgument,
                                                           void           * aResultValue,
                                                           void           * aInfo,
                                                           dtlFuncVector  * aVectorFunc,
                                                           void           * aVectorArg,
                                                           void           * aEnv );
stlStatus dtlOperIsGreaterThanEqualLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                           dtlValueEntry  * aInputArgument,
                                                           void           * aResultValue,
                                                           void           * aInfo,
                                                           dtlFuncVector  * aVectorFunc,
                                                           void           * aVectorArg,
                                                           void           * aEnv );
stlStatus dtlOperIsGreaterThanEqualVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                              dtlValueEntry  * aInputArgument,
                                                              void           * aResultValue,
                                                              void           * aInfo,
                                                              dtlFuncVector  * aVectorFunc,
                                                              void           * aVectorArg,
                                                              void           * aEnv );
stlStatus dtlOperIsGreaterThanEqualLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                              dtlValueEntry  * aInputArgument,
                                                              void           * aResultValue,
                                                              void           * aInfo,
                                                              dtlFuncVector  * aVectorFunc,
                                                              void           * aVectorArg,
                                                              void           * aEnv );
stlStatus dtlOperIsGreaterThanEqualLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                                  dtlValueEntry  * aInputArgument,
                                                                  void           * aResultValue,
                                                                  void           * aInfo,
                                                                  dtlFuncVector  * aVectorFunc,
                                                                  void           * aVectorArg,
                                                                  void           * aEnv );

stlStatus dtlOperIsLessThanBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );
stlStatus dtlOperIsLessThanLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );
stlStatus dtlOperIsLessThanVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );
stlStatus dtlOperIsLessThanLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );
stlStatus dtlOperIsLessThanLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                          dtlValueEntry  * aInputArgument,
                                                          void           * aResultValue,
                                                          void           * aInfo,
                                                          dtlFuncVector  * aVectorFunc,
                                                          void           * aVectorArg,
                                                          void           * aEnv );

stlStatus dtlOperIsLessThanEqualBinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv );
stlStatus dtlOperIsLessThanEqualLongvarbinaryAndBinary( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv );
stlStatus dtlOperIsLessThanEqualVarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                           dtlValueEntry  * aInputArgument,
                                                           void           * aResultValue,
                                                           void           * aInfo,
                                                           dtlFuncVector  * aVectorFunc,
                                                           void           * aVectorArg,
                                                           void           * aEnv );
stlStatus dtlOperIsLessThanEqualLongvarbinaryAndVarbinary( stlUInt16        aInputArgumentCnt,
                                                           dtlValueEntry  * aInputArgument,
                                                           void           * aResultValue,
                                                           void           * aInfo,
                                                           dtlFuncVector  * aVectorFunc,
                                                           void           * aVectorArg,
                                                           void           * aEnv );
stlStatus dtlOperIsLessThanEqualLongvarbinaryAndLongvarbinary( stlUInt16        aInputArgumentCnt,
                                                               dtlValueEntry  * aInputArgument,
                                                               void           * aResultValue,
                                                               void           * aInfo,
                                                               dtlFuncVector  * aVectorFunc,
                                                               void           * aVectorArg,
                                                               void           * aEnv );

/* Binary Large Object : not exist */


/*******************************************************************************
 * DATE TIME (GROUP)
 ******************************************************************************/

/* Date vs Date */

stlStatus dtlOperIsEqualDateToDate( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtlOperIsNotEqualDateToDate( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtlOperIsLessThanDateToDate( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtlOperIsLessThanEqualDateToDate( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanDateToDate( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualDateToDate( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

/* Date vs Timestamp */

stlStatus dtlOperIsEqualDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsNotEqualDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanEqualDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsGreaterThanDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

/* Time without timezone vs Time without timezone */

stlStatus dtlOperIsEqualTimeToTime( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtlOperIsNotEqualTimeToTime( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtlOperIsLessThanTimeToTime( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtlOperIsLessThanEqualTimeToTime( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanTimeToTime( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualTimeToTime( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

/* Timestamp Without Time Zone vs Date */

stlStatus dtlOperIsEqualTimestampToDate( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsNotEqualTimestampToDate( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanTimestampToDate( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanEqualTimestampToDate( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsGreaterThanTimestampToDate( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualTimestampToDate( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );


/* Timestamp Without Time Zone vs Timestamp Without Time Zone */

stlStatus dtlOperIsEqualTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsNotEqualTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsLessThanTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsLessThanEqualTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv );

stlStatus dtlOperIsGreaterThanTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                                         dtlValueEntry  * aInputArgument,
                                                         void           * aResultValue,
                                                         void           * aInfo,
                                                         dtlFuncVector  * aVectorFunc,
                                                         void           * aVectorArg,
                                                         void           * aEnv );

/* Time With Time Zone vs Time With Time Zone */

stlStatus dtlOperIsEqualTimeTzToTimeTz( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtlOperIsNotEqualTimeTzToTimeTz( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanTimeTzToTimeTz( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanEqualTimeTzToTimeTz( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanTimeTzToTimeTz( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualTimeTzToTimeTz( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

/* Timestamp With Time Zone vs Timestamp With Time Zone */

stlStatus dtlOperIsEqualTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

stlStatus dtlOperIsNotEqualTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

stlStatus dtlOperIsLessThanTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

stlStatus dtlOperIsLessThanEqualTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                          dtlValueEntry  * aInputArgument,
                                                          void           * aResultValue,
                                                          void           * aInfo,
                                                          dtlFuncVector  * aVectorFunc,
                                                          void           * aVectorArg,
                                                          void           * aEnv );

stlStatus dtlOperIsGreaterThanTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                             dtlValueEntry  * aInputArgument,
                                                             void           * aResultValue,
                                                             void           * aInfo,
                                                             dtlFuncVector  * aVectorFunc,
                                                             void           * aVectorArg,
                                                             void           * aEnv );

/* dtlIntervalYearToMonth */

stlStatus dtlOperIsEqualIntervalYearToMonth( stlUInt16       aInputArgumentCnt,
                                             dtlValueEntry * aInputArgument,
                                             void          * aResultValue,
                                             void          * aInfo,
                                             dtlFuncVector * aVectorFunc,
                                             void          * aVectorArg,
                                             void          * aEnv );

stlStatus dtlOperIsNotEqualIntervalYearToMonth( stlUInt16       aInputArgumentCnt,
                                                dtlValueEntry * aInputArgument,
                                                void          * aResultValue,
                                                void          * aInfo,
                                                dtlFuncVector * aVectorFunc,
                                                void          * aVectorArg,
                                                void          * aEnv );

stlStatus dtlOperIsLessThanIntervalYearToMonth( stlUInt16       aInputArgumentCnt,
                                                dtlValueEntry * aInputArgument,
                                                void          * aResultValue,
                                                void          * aInfo,
                                                dtlFuncVector * aVectorFunc,
                                                void          * aVectorArg,
                                                void          * aEnv );

stlStatus dtlOperIsLessThanEqualIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

stlStatus dtlOperIsGreaterThanIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv );

/* dtlIntervalDayToSecond */

stlStatus dtlOperIsEqualIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsNotEqualIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsLessThanIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsLessThanEqualIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

stlStatus dtlOperIsGreaterThanIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv );

/* dtlRowId */

stlStatus dtlOperIsEqualRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );

stlStatus dtlOperIsNotEqualRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsLessThanRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsLessThanEqualRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

/*******************************************************************************
 * BINARY INTEGER  VS  BINARY REAL (GROUP)
 ******************************************************************************/

/* SmallInt vs Real */
stlStatus dtlOperIsEqualSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtlOperIsNotEqualSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanEqualSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualSmallIntToReal( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

/* SmallInt vs Double */
stlStatus dtlOperIsEqualSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsNotEqualSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsLessThanSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsLessThanEqualSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

stlStatus dtlOperIsGreaterThanSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualSmallIntToDouble( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

/* Integer vs Real */
stlStatus dtlOperIsEqualIntegerToReal( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtlOperIsNotEqualIntegerToReal( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsLessThanIntegerToReal( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsLessThanEqualIntegerToReal( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanIntegerToReal( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualIntegerToReal( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

/* Integer vs Double */
stlStatus dtlOperIsEqualIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsNotEqualIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanEqualIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsGreaterThanIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualIntegerToDouble( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

/* BigInt vs Real */
stlStatus dtlOperIsEqualBigIntToReal( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );

stlStatus dtlOperIsNotEqualBigIntToReal( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsLessThanBigIntToReal( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsLessThanEqualBigIntToReal( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanBigIntToReal( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualBigIntToReal( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

/* BigInt vs Double */
stlStatus dtlOperIsEqualBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtlOperIsNotEqualBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanEqualBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualBigIntToDouble( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

/* Real vs SmallInt */
stlStatus dtlOperIsEqualRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtlOperIsNotEqualRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanEqualRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualRealToSmallInt( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

/* Real vs Integer */
stlStatus dtlOperIsEqualRealToInteger( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtlOperIsNotEqualRealToInteger( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsLessThanRealToInteger( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsLessThanEqualRealToInteger( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanRealToInteger( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualRealToInteger( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

/* Real vs BigInt */
stlStatus dtlOperIsEqualRealToBigInt( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );

stlStatus dtlOperIsNotEqualRealToBigInt( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsLessThanRealToBigInt( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsLessThanEqualRealToBigInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanRealToBigInt( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualRealToBigInt( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

/* Double vs SmallInt */
stlStatus dtlOperIsEqualDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv );

stlStatus dtlOperIsNotEqualDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsLessThanDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                             dtlValueEntry  * aInputArgument,
                                             void           * aResultValue,
                                             void           * aInfo,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             void           * aEnv );

stlStatus dtlOperIsLessThanEqualDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv );

stlStatus dtlOperIsGreaterThanDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualDoubleToSmallInt( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv );

/* Double vs Integer */
stlStatus dtlOperIsEqualDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

stlStatus dtlOperIsNotEqualDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv );

stlStatus dtlOperIsLessThanEqualDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv );

stlStatus dtlOperIsGreaterThanDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualDoubleToInteger( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv );

/* Double vs BigInt */
stlStatus dtlOperIsEqualDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv );

stlStatus dtlOperIsNotEqualDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlOperIsLessThanEqualDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlOperIsGreaterThanDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlOperIsGreaterThanEqualDoubleToBigInt( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv );

/*******************************************************************************
 * GET FUNCTION INFO 
 ******************************************************************************/

stlStatus dtlGetFuncInfoIsEqual( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoIsNotEqual( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoIsLessThan( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoIsLessThanEqual( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoIsGreaterThan( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoIsGreaterThanEqual( stlUInt16               aDataTypeArrayCount,
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

/*******************************************************************************
 * GET FUNCTION POINTER 
 ******************************************************************************/

stlStatus dtlGetFuncPtrIsEqual( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrIsNotEqual( stlUInt32             aFuncPtrIdx,
                                   dtlBuiltInFuncPtr   * aFuncPtr,
                                   stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrIsLessThan( stlUInt32             aFuncPtrIdx,
                                   dtlBuiltInFuncPtr   * aFuncPtr,
                                   stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrIsLessThanEqual( stlUInt32             aFuncPtrIdx,
                                        dtlBuiltInFuncPtr   * aFuncPtr,
                                        stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrIsGreaterThan( stlUInt32             aFuncPtrIdx,
                                      dtlBuiltInFuncPtr   * aFuncPtr,
                                      stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrIsGreaterThanEqual( stlUInt32             aFuncPtrIdx,
                                           dtlBuiltInFuncPtr   * aFuncPtr,
                                           stlErrorStack       * aErrorStack );


/******************************************************************************/


/** @} dtlOperCompareType */

#endif /* _DTL_OPER_COMPARE_TYPE_H_ */
