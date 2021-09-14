/*******************************************************************************
 * dtlFunction.c
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

/**
 * @file dtlFunction.c
 * @brief DataType Layer Function관련 함수 
 */

#include <dtl.h>


/**
 * @ingroup dtlFunction Function관련 함수
 * @{
 */

/**
 * @brief Function 의 Return Type Class String 상수
 */
const stlChar * const gDtlFuncReturnClassString[DTL_FUNC_RETURN_TYPE_CLASS_MAX] =
{
    "N/A",                 /**< DTL_FUNC_RETURN_TYPE_CLASS_INVALID */

    "single scalar value"  /**< DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE */
};


/**
 * @brief Function 의 Iteration Time String 상수
 */
const stlChar * const gDtlIterationTimeString[DTL_ITERATION_TIME_MAX] = 
{
    "execute by each aggregation",        /**< DTL_ITERATION_TIME_FOR_EACH_AGGREGATION */
    "execute by each expression",         /**< DTL_ITERATION_TIME_FOR_EACH_EXPR */
    "execute by each query",              /**< DTL_ITERATION_TIME_FOR_EACH_QUERY */
    "execute by each statement",          /**< DTL_ITERATION_TIME_FOR_EACH_STATEMENT */
    "execute by arguments",               /**< DTL_ITERATION_TIME_NONE */
};


/**
 * @brief Function 의 Action Type String 상수
 */
const stlChar * const gDtlActionTypeString[DTL_ACTION_TYPE_MAX] =
{
    "normal action",                  /**< DTL_ACTION_TYPE_NORMAL */
    "conditional jump if null",       /**< DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL */
    "conditional jump if not null",   /**< DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL */
    "conditional jump if true",       /**< DTL_ACTION_TYPE_CONDITION_JUMP_IS_TRUE */
    "conditional jump if false",      /**< DTL_ACTION_TYPE_CONDITION_JUMP_IS_FALSE */
    "conditional jump if not true",   /**< DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_TRUE */
    "conditional jump if not false",  /**< DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_FALSE */
    "conditional jump if false, else if null",
                            /**< DTL_ACTION_TYPE_CONDITION_JUMP_IS_FALSE_THEN_IS_NULL */
    "conditional jump if true, else if null",
                            /**< DTL_ACTION_TYPE_CONDITION_JUMP_IS_TRUE_THEN_IS_NULL */
    "unconditional jump",             /**< DTL_ACTION_TYPE_UNCONDITION_JUMP */
    
};

/**
 * @brief Function 의 Exception Return String 상수
 */
const stlChar * const gDtlActionReturnString[DTL_ACTION_RETURN_MAX] =
{
    "exceptional return normal",         /**< DTL_ACTION_RETURN_PASS */
    "exceptional return null",           /**< DTL_ACTION_RETURN_NULL */
    "exceptional return true",           /**< DTL_ACTION_RETURN_TRUE */
    "exceptional return false",          /**< DTL_ACTION_RETURN_FALSE */
    "exceptional return false or null",  /**< DTL_ACTION_RETURN_FALSE_THEN_NULL */
    "exceptional return true or null",   /**< DTL_ACTION_RETURN_TRUE_THEN_NULL */
    "exceptional return empty"           /**< DTL_ACTION_RETURN_EMPTY */
};


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
                                 stlErrorStack         * aErrorStack )
{
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    /**
     * output 설정
     */

    *aFuncResultDataType = DTL_TYPE_MAX;
    *aFuncPtrIdx = 0;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/*******************************************************************************
 * GET FUNCTION POINTER 
 ******************************************************************************/

stlStatus dtlGetFuncPtrInvalid( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );

    /**
     * output 설정
     */

    *aFuncPtr = dtlInvalidFunc;

    
    return STL_SUCCESS;
        
    STL_FINISH;
    
    return STL_FAILURE;
}



/*******************************************************************************
 * INVALID FUNCTION 
 ******************************************************************************/

stlStatus dtlInvalidFunc( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}

stlStatus dtlAllocMem( dtlAllocInfo  *aInfo,
                       stlInt32       aSize,
                       void        ** aResultPtr )
{
    aSize = STL_ALIGN_DEFAULT( aSize );
    
    STL_TRY( aSize <= aInfo->mBufSize - DTL_ALLOC_INFO_USED_SIZE( aInfo ) );

    *aResultPtr = aInfo->mPos;
    aInfo->mPos += aSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
