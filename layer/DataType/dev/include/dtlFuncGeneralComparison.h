/*******************************************************************************
 * dtlFuncGeneralComparison.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _DTL_FUNC_GENERAL_COMPARISON_H_
#define _DTL_FUNC_GENERAL_COMPARISON_H_ 1


/**
 * @file dtlFuncGeneralComparison.h
 * @brief DataType Layer Function 함수 
 */

#define DTL_GENERAL_COMPARISON_FUN_INPUT_MIN_ARG_CNT ( 1 )
#define DTL_GENERAL_COMPARISON_FUN_INPUT_MAX_ARG_CNT ( 1024 )

/*
 * COMMON
 */
stlStatus dtlGetFuncInfoGeneralComparison( stlUInt16               aDataTypeArrayCount,
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

/*
 * GREATEST
 */

stlStatus dtlGreatest( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aInfo,
                       dtlFuncVector  * aVectorFunc,
                       void           * aVectorArg,
                       void           * aEnv );

stlStatus dtlGetFuncInfoGreatest( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrGreatest( stlUInt32             aFuncPtrIdx,
                                 dtlBuiltInFuncPtr   * aFuncPtr,
                                 stlErrorStack       * aErrorStack );

/*
 * LEAST
 */ 

stlStatus dtlLeast( stlUInt16        aInputArgumentCnt,
                    dtlValueEntry  * aInputArgument,
                    void           * aResultValue,
                    void           * aInfo,
                    dtlFuncVector  * aVectorFunc,
                    void           * aVectorArg,
                    void           * aEnv );

stlStatus dtlGetFuncInfoLeast( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrLeast( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );


#endif /* _DTL_FUNC_GENERAL_COMPARISON_H_ */
