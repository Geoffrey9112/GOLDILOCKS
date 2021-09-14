/*******************************************************************************
 * dtlOperLogical.h
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
#ifndef _DTL_OPER_LOGICAL_H_
#define _DTL_OPER_LOGICAL_H__ 1


/**
 * @file dtlOperLogical.h
 * @brief DataType Layer Data Value 간 logical 함수
 */

/**
 * @defgroup dtlOperLogical Logical Operation 
 * @ingroup dtlOperation
 * @{
 */

/*******************************************************************************
 * logical condition function
 ******************************************************************************/

stlStatus dtlFuncAnd( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aInfo,
                      dtlFuncVector  * aVectorFunc,
                      void           * aVectorArg,
                      void           * aEnv );

stlStatus dtlFuncOr( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aInfo,
                     dtlFuncVector  * aVectorFunc,
                     void           * aVectorArg,
                     void           * aEnv );

stlStatus dtlFuncNot( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aInfo,
                      dtlFuncVector  * aVectorFunc,
                      void           * aVectorArg,
                      void           * aEnv );

stlStatus dtlFuncIs( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aInfo,
                     dtlFuncVector  * aVectorFunc,
                     void           * aVectorArg,
                     void           * aEnv );

stlStatus dtlFuncIsNot( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

/*******************************************************************************
 * GET FUNCTION INFO 
 ******************************************************************************/

stlStatus dtlGetFuncInfoAnd( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoOr( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoNot( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoIs( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoIsNot( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoIsUnknown( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoIsNotUnknown( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrAnd( stlUInt32             aFuncPtrIdx,
                            dtlBuiltInFuncPtr   * aFuncPtr,
                            stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrOr( stlUInt32             aFuncPtrIdx,
                           dtlBuiltInFuncPtr   * aFuncPtr,
                           stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrNot( stlUInt32             aFuncPtrIdx,
                            dtlBuiltInFuncPtr   * aFuncPtr,
                            stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrIs( stlUInt32             aFuncPtrIdx,
                           dtlBuiltInFuncPtr   * aFuncPtr,
                           stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrIsNot( stlUInt32             aFuncPtrIdx,
                              dtlBuiltInFuncPtr   * aFuncPtr,
                              stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrIsUnknown( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrIsNotUnknown( stlUInt32             aFuncPtrIdx,
                                     dtlBuiltInFuncPtr   * aFuncPtr,
                                     stlErrorStack       * aErrorStack );

/** @} dtlOperLogical */

#endif /* _DTL_OPER_LOGICAL_H_ */

