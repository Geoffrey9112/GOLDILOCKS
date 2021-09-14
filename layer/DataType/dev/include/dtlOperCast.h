/*******************************************************************************
 * dtlOperCast.h
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


#ifndef _DTL_OPER_CAST_H_
#define _DTL_OPER_CAST_H_ 1

/**
 * @file dtlOperCast.h
 * @brief DataType Layer Casting 함수 
 */

/**
 * @addtogroup dtlOperCast
 *
 * @remark
 * <BR> cast함수의 input argument 설명.
 * <BR> 1. input argument count = 5
 * <BR> 2. input argument
 * <BR> ...(0) source value  
 * <BR> ...(1) dest precision
 * <BR> ...(2) dest scale
 * <BR> ...(3) dest string length unit
 * <BR> ...(4) dest interval indicator
 * <BR>
 *
 * 
 * @{
 */


#define DTL_CAST_INPUT_ARG_CNT ( 5 )

extern dtlBuiltInFuncPtr dtlCastFunctionList[DTL_TYPE_MAX][DTL_TYPE_MAX];

extern stlBool dtlIsNoDataLossForCast[DTL_TYPE_MAX][DTL_TYPE_MAX];

#define DTL_GET_CAST_FUNC_PTR_IDX( aType1, aType2 ) ( (aType1) * 100 + (aType2) )


/*******************************************************************************
 * GET FUNCTION INFO 
 ******************************************************************************/

stlStatus dtlGetFuncInfoCast( stlUInt16               aDataTypeArrayCount,
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

stlStatus dtlGetFuncPtrCast( stlUInt32             aFuncPtrIdx,
                             dtlBuiltInFuncPtr   * aFuncPtr,
                             stlErrorStack       * aErrorStack );


/*******************************************************************************
 * etc
 ******************************************************************************/

stlBool dtlIsApplicableCast( dtlDataType           aSrcType,
                             dtlIntervalIndicator  aSrcIntervalIndicator,
                             dtlDataType           aDstType,
                             dtlIntervalIndicator  aDstIntervalIndicator );
    

/** @} */

#endif /* _DTL_OPER_CAST_H_ */
