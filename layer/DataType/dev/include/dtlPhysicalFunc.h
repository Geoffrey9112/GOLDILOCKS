/*******************************************************************************
 * dtlPhysicalFunc.h
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
#ifndef _DTL_PHYSICAL_FUNC_H_
#define _DTL_PHYSICAL_FUNC_H__ 1


/**
 * @file dtlPhysicalFunc.h
 * @brief Align이 보장되지 않은 value간의 연산
 */

/**
 * @addtogroup dtlPhysicalFunction
 * @{
 */


/*******************************************************************************
 * 1-Argument Column FUNCTION
 ******************************************************************************/

stlBool dtlPhysicalFuncIsTrue( void       * aLeftVal,
                               stlInt64     aLeftLen,
                               void       * aRightVal,
                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsFalse( void       * aLeftVal,
                                stlInt64     aLeftLen,
                                void       * aRightVal,
                                stlInt64     aRightLen );


/*******************************************************************************
 * 1-Argument FUNCTIONS
 ******************************************************************************/

stlBool dtlPhysicalFuncIs( void       * aLeftVal,
                           stlInt64     aLeftLen,
                           void       * aRightVal,
                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNot( void       * aLeftVal,
                              stlInt64     aLeftLen,
                              void       * aRightVal,
                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNull( void       * aLeftVal,
                               stlInt64     aLeftLen,
                               void       * aRightVal,
                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotNull( void       * aLeftVal,
                                  stlInt64     aLeftLen,
                                  void       * aRightVal,
                                  stlInt64     aRightLen );


/*******************************************************************************
 * 2-Argument FUNCTIONS
 ******************************************************************************/

stlBool dtlPhysicalFuncIsEqualBooleanToBoolean( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualSmallIntToSmallInt( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualSmallIntToInteger( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualSmallIntToBigInt( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualSmallIntToReal( void       * aLeftVal,
                                              stlInt64     aLeftLen,
                                              void       * aRightVal,
                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualSmallIntToDouble( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualSmallIntToNumeric( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualIntegerToSmallInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualIntegerToInteger( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualIntegerToBigInt( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualIntegerToReal( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualIntegerToDouble( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualIntegerToNumeric( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualBigIntToSmallInt( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualBigIntToInteger( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualBigIntToBigInt( void       * aLeftVal,
                                              stlInt64     aLeftLen,
                                              void       * aRightVal,
                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualBigIntToReal( void       * aLeftVal,
                                            stlInt64     aLeftLen,
                                            void       * aRightVal,
                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualBigIntToDouble( void       * aLeftVal,
                                              stlInt64     aLeftLen,
                                              void       * aRightVal,
                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualBigIntToNumeric( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualRealToSmallInt( void       * aLeftVal,
                                              stlInt64     aLeftLen,
                                              void       * aRightVal,
                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualRealToInteger( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualRealToBigInt( void       * aLeftVal,
                                            stlInt64     aLeftLen,
                                            void       * aRightVal,
                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualRealToReal( void       * aLeftVal,
                                          stlInt64     aLeftLen,
                                          void       * aRightVal,
                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualRealToDouble( void       * aLeftVal,
                                            stlInt64     aLeftLen,
                                            void       * aRightVal,
                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualRealToNumeric( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualDoubleToSmallInt( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualDoubleToInteger( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualDoubleToBigInt( void       * aLeftVal,
                                              stlInt64     aLeftLen,
                                              void       * aRightVal,
                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualDoubleToReal( void       * aLeftVal,
                                            stlInt64     aLeftLen,
                                            void       * aRightVal,
                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualDoubleToDouble( void       * aLeftVal,
                                              stlInt64     aLeftLen,
                                              void       * aRightVal,
                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualDoubleToNumeric( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualNumericToSmallInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualNumericToInteger( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualNumericToBigInt( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualNumericToReal( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualNumericToDouble( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualNumericToNumeric( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualFixedLengthChar( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualVariableLengthChar( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualCharAndLongvarchar( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualLongvarcharAndChar( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualVarcharAndLongvarchar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualLongvarcharAndVarchar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualLongvarcharAndLongvarchar( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualFixedLengthBinary( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualVariableLengthBinary( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualBinaryAndLongvarbinary( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualLongvarbinaryAndBinary( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualVarbinaryAndLongvarbinary( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualLongvarbinaryAndVarbinary( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualLongvarbinaryAndLongvarbinary( void       * aLeftVal,
                                                             stlInt64     aLeftLen,
                                                             void       * aRightVal,
                                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualDateToDate( void       * aLeftVal,
                                          stlInt64     aLeftLen,
                                          void       * aRightVal,
                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualDateToTimestamp( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualTimeToTime( void       * aLeftVal,
                                          stlInt64     aLeftLen,
                                          void       * aRightVal,
                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualTimeTzToTimeTz( void       * aLeftVal,
                                              stlInt64     aLeftLen,
                                              void       * aRightVal,
                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualTimestampToDate( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualTimestampToTimestamp( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualTimestampTzToTimestampTz( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualIntervalYearToMonth( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualIntervalDayToSecond( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsEqualRowIdToRowId( void       * aLeftVal,
                                            stlInt64     aLeftLen,
                                            void       * aRightVal,
                                            stlInt64     aRightLen );

/*******************************************************************************
 * 2-Argument FUNCTIONS - IsNotEqual
 ******************************************************************************/

stlBool dtlPhysicalFuncIsNotEqualBooleanToBoolean( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualSmallIntToSmallInt( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualSmallIntToInteger( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualSmallIntToBigInt( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualSmallIntToReal( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualSmallIntToDouble( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualSmallIntToNumeric( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualIntegerToSmallInt( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualIntegerToInteger( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualIntegerToBigInt( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualIntegerToReal( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualIntegerToDouble( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualIntegerToNumeric( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualBigIntToSmallInt( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualBigIntToInteger( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualBigIntToBigInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualBigIntToReal( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualBigIntToDouble( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualBigIntToNumeric( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualRealToSmallInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualRealToInteger( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualRealToBigInt( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualRealToReal( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualRealToDouble( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualRealToNumeric( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualDoubleToSmallInt( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualDoubleToInteger( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualDoubleToBigInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualDoubleToReal( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualDoubleToDouble( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualDoubleToNumeric( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualNumericToSmallInt( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualNumericToInteger( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualNumericToBigInt( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualNumericToReal( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualNumericToDouble( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualNumericToNumeric( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualFixedLengthChar( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualVariableLengthChar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualCharAndLongvarchar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualLongvarcharAndChar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualVarcharAndLongvarchar( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualLongvarcharAndVarchar( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualLongvarcharAndLongvarchar( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualFixedLengthBinary( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualVariableLengthBinary( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualBinaryAndLongvarbinary( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualLongvarbinaryAndBinary( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualVarbinaryAndLongvarbinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualLongvarbinaryAndVarbinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualLongvarbinaryAndLongvarbinary( void       * aLeftVal,
                                                                stlInt64     aLeftLen,
                                                                void       * aRightVal,
                                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualDateToDate( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualDateToTimestamp( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualTimeToTime( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualTimeTzToTimeTz( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualTimestampToDate( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualTimestampToTimestamp( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualTimestampTzToTimestampTz( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualIntervalYearToMonth( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualIntervalDayToSecond( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsNotEqualRowIdToRowId( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

/*******************************************************************************
 * 2-Argument FUNCTIONS - IsLessThan
 ******************************************************************************/

stlBool dtlPhysicalFuncIsLessThanBooleanToBoolean( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanSmallIntToSmallInt( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanSmallIntToInteger( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanSmallIntToBigInt( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanSmallIntToReal( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanSmallIntToDouble( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanSmallIntToNumeric( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanIntegerToSmallInt( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanIntegerToInteger( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanIntegerToBigInt( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanIntegerToReal( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanIntegerToDouble( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanIntegerToNumeric( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanBigIntToSmallInt( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanBigIntToInteger( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanBigIntToBigInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanBigIntToReal( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanBigIntToDouble( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanBigIntToNumeric( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanRealToSmallInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanRealToInteger( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanRealToBigInt( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanRealToReal( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanRealToDouble( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanRealToNumeric( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanDoubleToSmallInt( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanDoubleToInteger( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanDoubleToBigInt( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanDoubleToReal( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanDoubleToDouble( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanDoubleToNumeric( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanNumericToSmallInt( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanNumericToInteger( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanNumericToBigInt( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanNumericToReal( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanNumericToDouble( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanNumericToNumeric( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanFixedLengthChar( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanVariableLengthChar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanCharAndLongvarchar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanLongvarcharAndChar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanVarcharAndLongvarchar( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanLongvarcharAndVarchar( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanLongvarcharAndLongvarchar( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanFixedLengthBinary( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanVariableLengthBinary( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanBinaryAndLongvarbinary( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanLongvarbinaryAndBinary( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanVarbinaryAndLongvarbinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanLongvarbinaryAndVarbinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanLongvarbinaryAndLongvarbinary( void       * aLeftVal,
                                                                stlInt64     aLeftLen,
                                                                void       * aRightVal,
                                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanDateToDate( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanDateToTimestamp( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanTimeToTime( void       * aLeftVal,
                                             stlInt64     aLeftLen,
                                             void       * aRightVal,
                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanTimeTzToTimeTz( void       * aLeftVal,
                                                 stlInt64     aLeftLen,
                                                 void       * aRightVal,
                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanTimestampToDate( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanTimestampToTimestamp( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanTimestampTzToTimestampTz( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanIntervalYearToMonth( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanIntervalDayToSecond( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanRowIdToRowId( void       * aLeftVal,
                                               stlInt64     aLeftLen,
                                               void       * aRightVal,
                                               stlInt64     aRightLen );

/*******************************************************************************
 * 2-Argument FUNCTIONS - IsLessThanEqual
 ******************************************************************************/

stlBool dtlPhysicalFuncIsLessThanEqualBooleanToBoolean( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualSmallIntToSmallInt( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualSmallIntToInteger( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualSmallIntToBigInt( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualSmallIntToReal( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualSmallIntToDouble( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualSmallIntToNumeric( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualIntegerToSmallInt( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualIntegerToInteger( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualIntegerToBigInt( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualIntegerToReal( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualIntegerToDouble( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualIntegerToNumeric( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualBigIntToSmallInt( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualBigIntToInteger( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualBigIntToBigInt( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualBigIntToReal( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualBigIntToDouble( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualBigIntToNumeric( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualRealToSmallInt( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualRealToInteger( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualRealToBigInt( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualRealToReal( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualRealToDouble( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualRealToNumeric( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualDoubleToSmallInt( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualDoubleToInteger( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualDoubleToBigInt( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualDoubleToReal( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualDoubleToDouble( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualDoubleToNumeric( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualNumericToSmallInt( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualNumericToInteger( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualNumericToBigInt( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualNumericToReal( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualNumericToDouble( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualNumericToNumeric( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualFixedLengthChar( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualVariableLengthChar( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualCharAndLongvarchar( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualLongvarcharAndChar( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualVarcharAndLongvarchar( void       * aLeftVal,
                                                             stlInt64     aLeftLen,
                                                             void       * aRightVal,
                                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualLongvarcharAndVarchar( void       * aLeftVal,
                                                             stlInt64     aLeftLen,
                                                             void       * aRightVal,
                                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualLongvarcharAndLongvarchar( void       * aLeftVal,
                                                                 stlInt64     aLeftLen,
                                                                 void       * aRightVal,
                                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualFixedLengthBinary( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualVariableLengthBinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualBinaryAndLongvarbinary( void       * aLeftVal,
                                                              stlInt64     aLeftLen,
                                                              void       * aRightVal,
                                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualLongvarbinaryAndBinary( void       * aLeftVal,
                                                              stlInt64     aLeftLen,
                                                              void       * aRightVal,
                                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualVarbinaryAndLongvarbinary( void       * aLeftVal,
                                                                 stlInt64     aLeftLen,
                                                                 void       * aRightVal,
                                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualLongvarbinaryAndVarbinary( void       * aLeftVal,
                                                                 stlInt64     aLeftLen,
                                                                 void       * aRightVal,
                                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualLongvarbinaryAndLongvarbinary( void       * aLeftVal,
                                                                     stlInt64     aLeftLen,
                                                                     void       * aRightVal,
                                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualDateToDate( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualDateToTimestamp( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualTimeToTime( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualTimeTzToTimeTz( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualTimestampToDate( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualTimestampToTimestamp( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualTimestampTzToTimestampTz( void       * aLeftVal,
                                                                stlInt64     aLeftLen,
                                                                void       * aRightVal,
                                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualIntervalYearToMonth( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualIntervalDayToSecond( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsLessThanEqualRowIdToRowId( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

/*******************************************************************************
 * 2-Argument FUNCTIONS - IsGreaterThan
 ******************************************************************************/

stlBool dtlPhysicalFuncIsGreaterThanBooleanToBoolean( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanSmallIntToSmallInt( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanSmallIntToInteger( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanSmallIntToBigInt( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanSmallIntToReal( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanSmallIntToDouble( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanSmallIntToNumeric( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanIntegerToSmallInt( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanIntegerToInteger( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanIntegerToBigInt( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanIntegerToReal( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanIntegerToDouble( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanIntegerToNumeric( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanBigIntToSmallInt( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanBigIntToInteger( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanBigIntToBigInt( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanBigIntToReal( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanBigIntToDouble( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanBigIntToNumeric( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanRealToSmallInt( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanRealToInteger( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanRealToBigInt( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanRealToReal( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanRealToDouble( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanRealToNumeric( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanDoubleToSmallInt( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanDoubleToInteger( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanDoubleToBigInt( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanDoubleToReal( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanDoubleToDouble( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanDoubleToNumeric( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanNumericToSmallInt( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanNumericToInteger( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanNumericToBigInt( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanNumericToReal( void       * aLeftVal,
                                                   stlInt64     aLeftLen,
                                                   void       * aRightVal,
                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanNumericToDouble( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanNumericToNumeric( void       * aLeftVal,
                                                      stlInt64     aLeftLen,
                                                      void       * aRightVal,
                                                      stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanFixedLengthChar( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanVariableLengthChar( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanCharAndLongvarchar( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanLongvarcharAndChar( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanVarcharAndLongvarchar( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanLongvarcharAndVarchar( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanLongvarcharAndLongvarchar( void       * aLeftVal,
                                                               stlInt64     aLeftLen,
                                                               void       * aRightVal,
                                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanFixedLengthBinary( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanVariableLengthBinary( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanBinaryAndLongvarbinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanLongvarbinaryAndBinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanVarbinaryAndLongvarbinary( void       * aLeftVal,
                                                               stlInt64     aLeftLen,
                                                               void       * aRightVal,
                                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanLongvarbinaryAndVarbinary( void       * aLeftVal,
                                                               stlInt64     aLeftLen,
                                                               void       * aRightVal,
                                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanLongvarbinaryAndLongvarbinary( void       * aLeftVal,
                                                                   stlInt64     aLeftLen,
                                                                   void       * aRightVal,
                                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanDateToDate( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanDateToTimestamp( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanTimeToTime( void       * aLeftVal,
                                                stlInt64     aLeftLen,
                                                void       * aRightVal,
                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanTimeTzToTimeTz( void       * aLeftVal,
                                                    stlInt64     aLeftLen,
                                                    void       * aRightVal,
                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanTimestampToDate( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanTimestampToTimestamp( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanTimestampTzToTimestampTz( void       * aLeftVal,
                                                              stlInt64     aLeftLen,
                                                              void       * aRightVal,
                                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanIntervalYearToMonth( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanIntervalDayToSecond( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanRowIdToRowId( void       * aLeftVal,
                                                  stlInt64     aLeftLen,
                                                  void       * aRightVal,
                                                  stlInt64     aRightLen );

/*******************************************************************************
 * 2-Argument FUNCTIONS - IsGreaterThanEqual
 ******************************************************************************/

stlBool dtlPhysicalFuncIsGreaterThanEqualBooleanToBoolean( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualSmallIntToSmallInt( void       * aLeftVal,
                                                             stlInt64     aLeftLen,
                                                             void       * aRightVal,
                                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualSmallIntToInteger( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualSmallIntToBigInt( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualSmallIntToReal( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualSmallIntToDouble( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualSmallIntToNumeric( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualIntegerToSmallInt( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualIntegerToInteger( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualIntegerToBigInt( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualIntegerToReal( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualIntegerToDouble( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualIntegerToNumeric( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualBigIntToSmallInt( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualBigIntToInteger( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualBigIntToBigInt( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualBigIntToReal( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualBigIntToDouble( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualBigIntToNumeric( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualRealToSmallInt( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualRealToInteger( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualRealToBigInt( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualRealToReal( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualRealToDouble( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualRealToNumeric( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualDoubleToSmallInt( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualDoubleToInteger( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualDoubleToBigInt( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualDoubleToReal( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualDoubleToDouble( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualDoubleToNumeric( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualNumericToSmallInt( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualNumericToInteger( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualNumericToBigInt( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualNumericToReal( void       * aLeftVal,
                                                        stlInt64     aLeftLen,
                                                        void       * aRightVal,
                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualNumericToDouble( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualNumericToNumeric( void       * aLeftVal,
                                                           stlInt64     aLeftLen,
                                                           void       * aRightVal,
                                                           stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualFixedLengthChar( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualVariableLengthChar( void       * aLeftVal,
                                                             stlInt64     aLeftLen,
                                                             void       * aRightVal,
                                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualCharAndLongvarchar( void       * aLeftVal,
                                                             stlInt64     aLeftLen,
                                                             void       * aRightVal,
                                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualLongvarcharAndChar( void       * aLeftVal,
                                                             stlInt64     aLeftLen,
                                                             void       * aRightVal,
                                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualVarcharAndLongvarchar( void       * aLeftVal,
                                                             stlInt64     aLeftLen,
                                                             void       * aRightVal,
                                                             stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualLongvarcharAndVarchar( void       * aLeftVal,
                                                                stlInt64     aLeftLen,
                                                                void       * aRightVal,
                                                                stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualLongvarcharAndLongvarchar( void       * aLeftVal,
                                                                    stlInt64     aLeftLen,
                                                                    void       * aRightVal,
                                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualFixedLengthBinary( void       * aLeftVal,
                                                            stlInt64     aLeftLen,
                                                            void       * aRightVal,
                                                            stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualVariableLengthBinary( void       * aLeftVal,
                                                               stlInt64     aLeftLen,
                                                               void       * aRightVal,
                                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualBinaryAndLongvarbinary( void       * aLeftVal,
                                                                 stlInt64     aLeftLen,
                                                                 void       * aRightVal,
                                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualLongvarbinaryAndBinary( void       * aLeftVal,
                                                                 stlInt64     aLeftLen,
                                                                 void       * aRightVal,
                                                                 stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualVarbinaryAndLongvarbinary( void       * aLeftVal,
                                                                    stlInt64     aLeftLen,
                                                                    void       * aRightVal,
                                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualLongvarbinaryAndVarbinary( void       * aLeftVal,
                                                                    stlInt64     aLeftLen,
                                                                    void       * aRightVal,
                                                                    stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualLongvarbinaryAndLongvarbinary( void       * aLeftVal,
                                                                        stlInt64     aLeftLen,
                                                                        void       * aRightVal,
                                                                        stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualDateToDate( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualDateToTimestamp( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualTimeToTime( void       * aLeftVal,
                                                     stlInt64     aLeftLen,
                                                     void       * aRightVal,
                                                     stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualTimeTzToTimeTz( void       * aLeftVal,
                                                         stlInt64     aLeftLen,
                                                         void       * aRightVal,
                                                         stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualTimestampToDate( void       * aLeftVal,
                                                          stlInt64     aLeftLen,
                                                          void       * aRightVal,
                                                          stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualTimestampToTimestamp( void       * aLeftVal,
                                                               stlInt64     aLeftLen,
                                                               void       * aRightVal,
                                                               stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualTimestampTzToTimestampTz( void       * aLeftVal,
                                                                   stlInt64     aLeftLen,
                                                                   void       * aRightVal,
                                                                   stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualIntervalYearToMonth( void       * aLeftVal,
                                                              stlInt64     aLeftLen,
                                                              void       * aRightVal,
                                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualIntervalDayToSecond( void       * aLeftVal,
                                                              stlInt64     aLeftLen,
                                                              void       * aRightVal,
                                                              stlInt64     aRightLen );

stlBool dtlPhysicalFuncIsGreaterThanEqualRowIdToRowId( void       * aLeftVal,
                                                       stlInt64     aLeftLen,
                                                       void       * aRightVal,
                                                       stlInt64     aRightLen );

stlBool dtlInPhysicalFuncListToList( void       * aLeftVal,
                                     stlInt64     aLeftLen,
                                     void       * aRightVal,
                                     stlInt64     aRightCount );

stlBool dtlNotInPhysicalFuncListToList( void       * aLeftVal,
                                        stlInt64     aLeftLen,
                                        void       * aRightVal,
                                        stlInt64     aRightCount );

stlBool dtlNotEqualAnyPhysicalFunc( void       * aLeftVal,
                                    stlInt64     aLeftLen,
                                    void       * aRightVal,
                                    stlInt64     aRightCount );

stlBool dtlLessThanAnyPhysicalFunc( void       * aLeftVal,
                                    stlInt64     aLeftLen,
                                    void       * aRightVal,
                                    stlInt64     aRightCount );

stlBool dtlLessThanEqualAnyPhysicalFunc( void       * aLeftVal,
                                         stlInt64     aLeftLen,
                                         void       * aRightVal,
                                         stlInt64     aRightCount );

stlBool dtlGreaterThanAnyPhysicalFunc( void       * aLeftVal,
                                       stlInt64     aLeftLen,
                                       void       * aRightVal,
                                       stlInt64     aRightCount );

stlBool dtlGreaterThanEqualAnyPhysicalFunc( void       * aLeftVal,
                                            stlInt64     aLeftLen,
                                            void       * aRightVal,
                                            stlInt64     aRightCount );

stlBool dtlEqualAllPhysicalFunc( void       * aLeftVal,
                                 stlInt64     aLeftLen,
                                 void       * aRightVal,
                                 stlInt64     aRightCount );

stlBool dtlLessThanAllPhysicalFunc( void       * aLeftVal,
                                    stlInt64     aLeftLen,
                                    void       * aRightVal,
                                    stlInt64     aRightCount );

stlBool dtlLessThanEqualAllPhysicalFunc( void       * aLeftVal,
                                         stlInt64     aLeftLen,
                                         void       * aRightVal,
                                         stlInt64     aRightCount );

stlBool dtlGreaterThanAllPhysicalFunc( void       * aLeftVal,
                                       stlInt64     aLeftLen,
                                       void       * aRightVal,
                                       stlInt64     aRightCount );

stlBool dtlGreaterThanEqualAllPhysicalFunc( void       * aLeftVal,
                                            stlInt64     aLeftLen,
                                            void       * aRightVal,
                                            stlInt64     aRightCount );



/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

extern const dtlPhysicalFunc dtlInPhysicalFuncPointer[ DTL_IN_PHYSICAL_FILTER_CNT ]; 

extern const dtlPhysicalFunc dtlPhysicalFuncArg1FuncList[ DTL_PHYSICAL_FUNC_MAX - DTL_PHYSICAL_FUNC_IS ];

extern const dtlPhysicalFunc dtlPhysicalFuncArg2FuncList[ DTL_TYPE_MAX ][ DTL_TYPE_MAX ][ DTL_PHYSICAL_FUNC_IS ];

/** @} dtlPhysicalFunction */

#endif /* _DTL_PHYSICAL_FUNC_H_ */


