/*******************************************************************************
 * dtlFunction.h
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
#ifndef _DTL_FUNCTION_H_
#define _DTL_FUNCTION_H_ 1


/**
 * @file dtlFunction.h
 * @brief DataType Layer Function 함수 
 */

/*******************************************************************************
 * GET FUNCTION INFO 
 ******************************************************************************/

stlStatus dtlGetFuncInfoInvalid( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrInvalid( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack );


/*******************************************************************************
 * INVALID FUNCTION 
 ******************************************************************************/

stlStatus dtlInvalidFunc( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

/*******************************************************************************
 * FUNC MISC
 ******************************************************************************/

dtlTimestampTzType dtlGetCurrentTimestamp();


/*
 * Dump
 */

stlStatus dtlGetFuncInfoDump( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrDump( stlUInt32             aFuncPtrIdx,
                             dtlBuiltInFuncPtr   * aFuncPtr,
                             stlErrorStack       * aErrorStack );

stlStatus dtlDump( stlUInt16        aInputArgumentCnt,
                   dtlValueEntry  * aInputArgument,
                   void           * aResultValue,
                   void           * aInfo,
                   dtlFuncVector  * aVectorFunc,
                   void           * aVectorArg,
                   void           * aEnv );

#endif /* _DTL_FUNCTION_H_ */
