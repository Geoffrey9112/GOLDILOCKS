/*******************************************************************************
 * dtlFuncBitWise.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlFuncBitWise.h 1841 2011-09-07 07:14:06Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/
#ifndef _DTL_FUNC_BITWISE_H_
#define _DTL_FUNC_BITWISE_H_ 1


/**
 * @file dtlFuncBitWise.h
 * @brief DataType Layer Function 함수 
 */

/*
 * And
 */
stlStatus dtlBigIntBitwiseAnd( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtlNumberBitwiseAnd( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtlGetFuncInfoBitwiseAnd( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrBitwiseAnd( stlUInt32             aFuncPtrIdx,
                                   dtlBuiltInFuncPtr   * aFuncPtr,
                                   stlErrorStack       * aErrorStack );



/*
 * Or
 */
stlStatus dtlBigIntBitwiseOr( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );

stlStatus dtlNumberBitwiseOr( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );

stlStatus dtlGetFuncInfoBitwiseOr( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrBitwiseOr( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack );


/*
 * Xor
 */
stlStatus dtlBigIntBitwiseXor( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtlNumberBitwiseXor( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtlGetFuncInfoBitwiseXor( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrBitwiseXor( stlUInt32             aFuncPtrIdx,
                                   dtlBuiltInFuncPtr   * aFuncPtr,
                                   stlErrorStack       * aErrorStack );


/*
 * Not
 */

stlStatus dtlBitwiseNot( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlGetFuncInfoBitwiseNot( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrBitwiseNot( stlUInt32             aFuncPtrIdx,
                                   dtlBuiltInFuncPtr   * aFuncPtr,
                                   stlErrorStack       * aErrorStack );


/*
 * ShiftLeft
 */

stlStatus dtlBitwiseShiftLeft( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv );

stlStatus dtlGetFuncInfoShiftLeft( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrShiftLeft( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack );


/*
 * ShiftRight
 */

stlStatus dtlBitwiseShiftRight( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

stlStatus dtlGetFuncInfoShiftRight( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrShiftRight( stlUInt32             aFuncPtrIdx,
                                   dtlBuiltInFuncPtr   * aFuncPtr,
                                   stlErrorStack       * aErrorStack );


#endif /* _DTL_FUNC_BITWISE_H_ */
