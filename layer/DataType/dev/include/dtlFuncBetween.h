/*******************************************************************************
 * dtlFuncBetween.h
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
#ifndef _DTL_FUNC_BETWEEN_H_
#define _DTL_FUNC_BETWEEN_H__ 1


/**
 * @file dtlFuncBetween.h
 * @brief DataType Layer Data Value 간 Between 함수 
 */

/**
 * @addtogroup dtlFuncBetween
 * @{
 */

/*******************************************************************************
 * BETWEEN SYMMETRIC 
 ******************************************************************************/

stlStatus dtlBetweenSymmetric( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtlGetFuncPtrBetweenSymmetric( stlUInt32             aFuncPtrIdx,
                                         dtlBuiltInFuncPtr   * aFuncPtr,
                                         stlErrorStack       * aErrorStack );

/*******************************************************************************
 * NOT BETWEEN SYMMETRIC 
 ******************************************************************************/

stlStatus dtlNotBetweenSymmetric( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv );

stlStatus dtlGetFuncPtrNotBetweenSymmetric( stlUInt32             aFuncPtrIdx,
                                            dtlBuiltInFuncPtr   * aFuncPtr,
                                            stlErrorStack       * aErrorStack );


/*******************************************************************************
 * GET FUNCTION INFO 
 ******************************************************************************/

stlStatus dtlGetFuncInfoBetweenSymmetric( stlUInt16               aDataTypeArrayCount,
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

/** @} dtlFuncBetween */

#endif /* _DTL_FUNC_BETWEEN_H_ */
