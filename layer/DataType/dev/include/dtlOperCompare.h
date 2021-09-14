/*******************************************************************************
 * dtlOperCompare.h
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
#ifndef _DTL_OPER_COMPARE_H_
#define _DTL_OPER_COMPARE_H__ 1


/**
 * @file dtlOperCompare.h
 * @brief DataType Layer Data Value 간 비교 함수 
 */

/**
 * @addtogroup dtlOperCompare
 * @{
 */

/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

/* Type vs Type */
extern const dtlCompareType dtlCompOperArgType[ DTL_TYPE_MAX ][ DTL_TYPE_MAX ][ 2 ][ 2 ];

/*******************************************************************************
 * DECLARE FUNCTION POINTER
 ******************************************************************************/

extern dtlBuiltInFuncPtr dtlCompOperArg1FuncList[ DTL_COMP_OPER_MAX - DTL_COMP_OPER_IS_NULL ];

extern dtlBuiltInFuncPtr dtlCompOperArg2FuncList[ DTL_TYPE_MAX ][ DTL_TYPE_MAX ][ DTL_COMP_OPER_IS_NULL ];

/*******************************************************************************
 * GET FUNCTION POINTER
 ******************************************************************************/

// stlStatus dtlGetCompareFunction( dtlComparisonOper    aOperator,
//                                  dtlDataType          aLeftDataType,
//                                  dtlDataType          aRightDataType,
//                                  dtlBuiltInFuncPtr  * aFunctionPtr,
//                                  stlErrorStack      * aErrorStack );

// stlStatus dtlGetCompareFunctionID( dtlComparisonOper   aOperator,
//                                    dtlDataType         aLeftDataType,
//                                    dtlDataType         aRightDataType,
//                                    stlUInt32         * aFunctionID,
//                                    stlErrorStack     * aErrorStack );

/*******************************************************************************
 * COMMON
 ******************************************************************************/

stlStatus dtlOperIsNull( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtlOperIsNotNull( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );

/*******************************************************************************
 * GET FUNCTION INFO 
 ******************************************************************************/

stlStatus dtlGetFuncInfoIsNull( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncInfoIsNotNull( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrIsNull( stlUInt32             aFuncPtrIdx,
                               dtlBuiltInFuncPtr   * aFuncPtr,
                               stlErrorStack       * aErrorStack );

stlStatus dtlGetFuncPtrIsNotNull( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack );

/*******************************************************************************
 * ETC
 ******************************************************************************/

stlStatus dtlGetCompareConversionInfo( stlBool                    aIsLeftConstantData,
                                       stlBool                    aIsRightConstantData,
                                       dtlDataType                aLeftInputDataType,
                                       dtlDataType                aRightInputDataType,
                                       const dtlCompareType    ** aCompareType,
                                       stlErrorStack            * aErrorStack );

void dtlSetCompareCastIntervalIndicator( dtlDataType           * aDataTypeArray,
                                         dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                         dtlDataType           * aFuncArgDataTypeArray,
                                         dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray );

/** @} dtlOperCompare */

#endif /* _DTL_OPER_COMPARE_H_ */

