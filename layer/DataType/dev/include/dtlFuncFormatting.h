/*******************************************************************************
 * dtlFuncFormatting.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *x
 * NOTES
 *
 *
 ******************************************************************************/
#ifndef _DTL_FUNC_FORMATTING_H_
#define _DTL_FUNC_FORMATTING_H_ 1

/**
 * @file dtlFuncFormatting.h
 * @brief DataType Layer Function 함수 
 */

/******************************************************************************
 * TO_DATETIME_FORMATTING
 ******************************************************************************/

stlStatus dtlGetFuncInfoToDateTimeFormat( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrToDateTimeFormat( stlUInt32             aFuncPtrIdx,
                                         dtlBuiltInFuncPtr   * aFuncPtr,
                                         stlErrorStack       * aErrorStack );

stlStatus dtlToDateTimeFormat( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtlGetDateTimeFormatInfoForToDateTime( dtlDataType      aDataType,
                                                 stlChar        * aFormatString,
                                                 stlInt64         aFormatStringLen,
                                                 stlBool          aIsSaveToCharMaxResultLen,
                                                 void           * aToDateTimeFormatInfoBuffer,
                                                 stlInt32         aToDateTimeFormatInfoBufferSize,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 stlErrorStack  * aErrorStack );


/******************************************************************************
 * TO_NUMBER_FORMATTING
 ******************************************************************************/

stlStatus dtlGetFuncInfoToNumberFormat( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrToNumberFormat( stlUInt32             aFuncPtrIdx,
                                       dtlBuiltInFuncPtr   * aFuncPtr,
                                       stlErrorStack       * aErrorStack );

stlStatus dtlToNumberFormat( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv );


/******************************************************************************
 * TO_CHAR_FORMATTING
 ******************************************************************************/

extern dtlBuiltInFuncPtr  gDtlToCharFormatFunc[ DTL_TYPE_MAX ];

stlStatus dtlGetFuncInfoToCharFormat( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrIdxToCharFormat( dtlDataType      aDataType,
                                        stlUInt32      * aFuncPtrIdx,
                                        stlErrorStack  * aErrorStack );

stlStatus dtlGetFuncPtrToCharFormat( stlUInt32             aFuncPtrIdx,
                                     dtlBuiltInFuncPtr   * aFuncPtr,
                                     stlErrorStack       * aErrorStack );

stlStatus dtlCharStringToCharFormat( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     void           * aInfo,
                                     dtlFuncVector  * aVectorFunc,
                                     void           * aVectorArg,
                                     void           * aEnv );

stlStatus dtlDateToCharFormat( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtlTimeToCharFormat( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtlTimeWithTimeZoneToCharFormat( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );

stlStatus dtlTimestampToCharFormat( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtlTimestampWithTimeZoneToCharFormat( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv );

stlStatus dtlIntervalYearToMonthToCharFormat( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlIntervalDayToSecondToCharFormat( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv );

stlStatus dtlNumericToCharFormat( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv );

stlStatus dtlDoubleToCharFormat( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtlRealToCharFormat( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtlGetDateTimeFormatInfoForToChar( dtlDataType                aDataType,
                                             stlChar                  * aFormatString,
                                             stlInt64                   aFormatStringLen,
                                             void                     * aToCharFormatInfoBuffer,
                                             stlInt32                   aToCharFormatInfoBufferSize,
                                             dtlFuncVector            * aVectorFunc,
                                             void                     * aVectorArg,
                                             stlErrorStack            * aErrorStack );

stlBool dtlIsExistTimeZoneStr( stlChar   * aDateTimeString,
                               stlInt64    aDateTimeStringLength,
                               stlBool     aIsTimestampStr );

stlStatus dtlGetToCharDateTime( dtlDataType             aDataType,
                                dtlExpTime            * aDtlExpTime,
                                dtlFractionalSecond     aFractionalSecond,
                                stlInt32              * aTimeZone,
                                dtlDateTimeFormatInfo * aToCharFormatInfo,
                                stlInt64                aAvailableSize,
                                stlChar               * aResult,
                                stlInt64              * aLength,
                                stlErrorStack         * aErrorStack );

/******************************************************************************
 * TO_CHAR
 ******************************************************************************/

stlStatus dtlToChar( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aInfo,
                     dtlFuncVector  * aVectorFunc,
                     void           * aVectorArg,
                     void           * aEnv );


/******************************************************************************
 * TO_DATE
 ******************************************************************************/

stlStatus dtlToDate( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aInfo,
                     dtlFuncVector  * aVectorFunc,
                     void           * aVectorArg,
                     void           * aEnv );

stlStatus dtlGetFuncInfoToDate( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrToDate( stlUInt32             aFuncPtrIdx,
                               dtlBuiltInFuncPtr   * aFuncPtr,
                               stlErrorStack       * aErrorStack );

/******************************************************************************
 * TO_TIME
 ******************************************************************************/

stlStatus dtlToTime( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aInfo,
                     dtlFuncVector  * aVectorFunc,
                     void           * aVectorArg,
                     void           * aEnv );

stlStatus dtlGetFuncInfoToTime( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrToTime( stlUInt32             aFuncPtrIdx,
                               dtlBuiltInFuncPtr   * aFuncPtr,
                               stlErrorStack       * aErrorStack );


/******************************************************************************
 * TO_TIME_WITH_TIME_ZONE
 ******************************************************************************/

stlStatus dtlToTimeWithTimeZone( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtlGetFuncInfoToTimeWithTimeZone( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrToTimeWithTimeZone( stlUInt32             aFuncPtrIdx,
                                           dtlBuiltInFuncPtr   * aFuncPtr,
                                           stlErrorStack       * aErrorStack );


/******************************************************************************
 * TO_TIMESTAMP
 ******************************************************************************/

stlStatus dtlToTimestamp( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtlGetFuncInfoToTimestamp( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrToTimestamp( stlUInt32             aFuncPtrIdx,
                                    dtlBuiltInFuncPtr   * aFuncPtr,
                                    stlErrorStack       * aErrorStack );


/******************************************************************************
 * TO_TIMESTAMP_WITH_TIME_ZONE
 ******************************************************************************/

stlStatus dtlToTimestampWithTimeZone( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );

stlStatus dtlGetFuncInfoToTimestampWithTimeZone( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrToTimestampWithTimeZone( stlUInt32             aFuncPtrIdx,
                                                dtlBuiltInFuncPtr   * aFuncPtr,
                                                stlErrorStack       * aErrorStack );


/*******************************************************************************
 * ODBC / CDC 를 위한 DEFAULT DATETIME FORMAT FUNCTION
 * ODBC에서는 datetime의 input 은 session format을 따르고,
 *            datetime의 output string을 출력할 경우, 아래와 같은 format을 따른다.
 *******************************************************************************/

#define DTL_DATE_FORMAT_FOR_ODBC                     ( "SYYYY-MM-DD HH24:MI:SS" )
#define DTL_TIME_FORMAT_FOR_ODBC                     ( "HH24:MI:SS.FF6" )
#define DTL_TIME_WITH_TIME_ZONE_FORMAT_FOR_ODBC      ( "HH24:MI:SS.FF6 TZH:TZM" )
#define DTL_TIMESTAMP_FORMAT_FOR_ODBC                ( "SYYYY-MM-DD HH24:MI:SS.FF6" )
#define DTL_TIMESTAMP_WITH_TIME_ZONE_FORMAT_FOR_ODBC ( "SYYYY-MM-DD HH24:MI:SS.FF6 TZH:TZM" )

extern dtlDateTimeFormatInfo  * gDtlNLSDateFormatInfoForODBC;
extern dtlDateTimeFormatInfo  * gDtlNLSTimeFormatInfoForODBC;
extern dtlDateTimeFormatInfo  * gDtlNLSTimeWithTimeZoneFormatInfoForODBC;
extern dtlDateTimeFormatInfo  * gDtlNLSTimestampFormatInfoForODBC;
extern dtlDateTimeFormatInfo  * gDtlNLSTimestampWithTimeZoneFormatInfoForODBC;

stlStatus dtlBuildNLSDateTimeFormatInfoForODBC( stlErrorStack * aErrorStack );



/*******************************************************************************
 * TO_NUMBER
 *******************************************************************************/

stlStatus dtlToNumber( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aInfo,
                       dtlFuncVector  * aVectorFunc,
                       void           * aVectorArg,
                       void           * aEnv );

stlStatus dtlGetFuncInfoToNumber( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrToNumber( stlUInt32             aFuncPtrIdx,
                                 dtlBuiltInFuncPtr   * aFuncPtr,
                                 stlErrorStack       * aErrorStack );


/*******************************************************************************
 * TO_NATIVE_REAL
 *******************************************************************************/

stlStatus dtlToNativeReal( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtlGetFuncInfoToNativeReal( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrToNativeReal( stlUInt32             aFuncPtrIdx,
                                     dtlBuiltInFuncPtr   * aFuncPtr,
                                     stlErrorStack       * aErrorStack );


/*******************************************************************************
 * TO_NATIVE_DOUBLE
 *******************************************************************************/

stlStatus dtlToNativeDouble( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv );

stlStatus dtlGetFuncInfoToNativeDouble( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrToNativeDouble( stlUInt32             aFuncPtrIdx,
                                       dtlBuiltInFuncPtr   * aFuncPtr,
                                       stlErrorStack       * aErrorStack );



#endif /* _DTL_FUNC_FORMATTING_H_ */
