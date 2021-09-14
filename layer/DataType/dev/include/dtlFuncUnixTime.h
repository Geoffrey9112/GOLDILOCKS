/*******************************************************************************
 * dtlFuncUnixTime.h
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

#ifndef _DTL_FUNC_UNIX_TIME_H_
#define _DTL_FUNC_UNIX_TIME_H_ 1

/**
 * @file dtlFuncUnixTime.h
 * @brief DataType Layer Function 함수 
 */


/**
 * @ingroup dtfDateTime
 * @internal
 * @{
 */

stlStatus dtlFromUnixTime( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtlGetFuncInfoFromUnixTime( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrFromUnixTime( stlUInt32             aFuncPtrIdx,
                                     dtlBuiltInFuncPtr   * aFuncPtr,
                                     stlErrorStack       * aErrorStack );

/** @} */

#endif /* _DTL_FUNC_UNIX_TIME_H_ */
