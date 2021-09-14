/*******************************************************************************
 * knlFunction.h
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


#ifndef _KNL_FUNCTION_H_
#define _KNL_FUNCTION_H_ 1

/**
 * @file knlFunction.h
 * @brief Kernel Layer Function 
 */

/**
 * @defgroup knlFunction Function
 * @ingroup knExternal
 * @{
 */


/**
 * @addtogroup knlFunctionList
 * @{
 */

/**
 * @brief KERNEL LIST FUNCTION 매크로 
 * @remark List Function은 ValueBlock을 직접 참조하여 Value를 가져오므로 <BR>
 *  매크로가 function마다 수행된다.
 */
#define KNL_LIST_FUNC_MAX_VALUE_CNT       (2000)


#define KNL_LIST_FUNC_GET_LEFT_VALUE( aValue, aType, aContext, aCast, aBlockIdx, aEnv  )    \
    {                                                                                       \
        if( (aType) == KNL_EXPR_TYPE_VALUE )                                                \
        {                                                                                   \
            if( ((aContext)->mCast) == NULL )                                               \
            {                                                                               \
                (aValue) =                                                                  \
                    KNL_GET_VALUE_BLOCK_DATA_VALUE( ((aContext)->mInfo.mValueInfo),         \
                                                    (aBlockIdx) );                          \
            }                                                                               \
            else                                                                            \
            {                                                                               \
                STL_TRY_THROW( DTL_IS_NULL( ( KNL_GET_VALUE_BLOCK_DATA_VALUE(               \
                                                  ((aContext)->mInfo.mValueInfo),           \
                                                  (aBlockIdx) )                             \
                                                ) ) == STL_FALSE, RAMP_LIST_EXIT );         \
                                                                                            \
                ((aContext)->mCast->mArgs[0].mValue.mDataValue) =                           \
                    KNL_GET_VALUE_BLOCK_DATA_VALUE( ((aContext)->mInfo.mValueInfo),         \
                                                    (aBlockIdx) );                          \
                                                                                            \
                if( (aCast) != NULL )                                                       \
                {                                                                           \
                    if( ((aContext)->mCast->mCastFuncPtr) ==                                \
                        ((aCast)->mCast->mCastFuncPtr) )                                    \
                    {                                                                       \
                        (aValue) = & ((aCast)->mCast->mCastResultBuf);                      \
                        STL_THROW( RAMP_EXIT_CAST );                                        \
                    }                                                                       \
                }                                                                           \
                                                                                            \
                (aCast) = (aContext);                                                       \
                                                                                            \
                STL_TRY( ((aContext)->mCast->mCastFuncPtr)(                                 \
                             DTL_CAST_INPUT_ARG_CNT,                                        \
                             ((aContext)->mCast->mArgs),                                    \
                             (void *) & ((aContext)->mCast->mCastResultBuf),                \
                             (void *)((aContext)->mCast->mSrcTypeInfo),                     \
                             KNL_DT_VECTOR((aEnv)),                                         \
                             (aEnv),                                                        \
                             KNL_ERROR_STACK( (aEnv) ) )                                    \
                         == STL_SUCCESS );                                                  \
                                                                                            \
                (aValue) = & ((aContext)->mCast->mCastResultBuf);                           \
            }                                                                               \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            if( ((aContext)->mCast) != NULL )                                               \
            {                                                                               \
                (aValue) = & ((aContext)->mCast->mCastResultBuf);                           \
            }                                                                               \
            else                                                                            \
            {                                                                               \
                if( (aType) == KNL_EXPR_TYPE_FUNCTION )                                     \
                {                                                                           \
                    (aValue) = & ((aContext)->mInfo.mFuncData->mResultValue);               \
                }                                                                           \
                else                                                                        \
                {     /* KNL_EXPR_TYPE_LIST */                                              \
                    (aValue) = & ((aContext)->mInfo.mListFunc->mResultValue);               \
                }                                                                           \
            }                                                                               \
        }                                                                                   \
                                                                                            \
        STL_RAMP( RAMP_EXIT_CAST );                                                         \
    }

#define KNL_LIST_FUNC_GET_RIGHT_VALUE( aValue, aType, aContext, aBlockIdx, aEnv  )      \
    {                                                                                   \
        if( (aType) == KNL_EXPR_TYPE_VALUE )                                            \
        {                                                                               \
            (aValue) =                                                                  \
                KNL_GET_VALUE_BLOCK_DATA_VALUE( ((aContext)->mInfo.mValueInfo),         \
                                                (aBlockIdx) );                          \
            if( ((aContext)->mCast) == NULL )                                           \
            {                                                                           \
                /* Do Nothing */                                                        \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                STL_TRY_THROW( DTL_IS_NULL( (aValue) ) == STL_FALSE, RAMP_LIST_EXIT );  \
                                                                                        \
                ((aContext)->mCast->mArgs[0].mValue.mDataValue) = (aValue);             \
                                                                                        \
                STL_TRY( ((aContext)->mCast->mCastFuncPtr)(                             \
                             DTL_CAST_INPUT_ARG_CNT,                                    \
                             ((aContext)->mCast->mArgs),                                \
                             (void *) & ((aContext)->mCast->mCastResultBuf),            \
                             (void *)((aContext)->mCast->mSrcTypeInfo),                 \
                             KNL_DT_VECTOR((aEnv)),                                     \
                             (aEnv),                                                    \
                             KNL_ERROR_STACK( (aEnv) ) )                                \
                         == STL_SUCCESS );                                              \
                                                                                        \
                (aValue) = & ((aContext)->mCast->mCastResultBuf);                       \
            }                                                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            if( ((aContext)->mCast) != NULL )                                           \
            {                                                                           \
                (aValue) = & ((aContext)->mCast->mCastResultBuf);                       \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                if( (aType) == KNL_EXPR_TYPE_FUNCTION )                                 \
                {                                                                       \
                    (aValue) = & ((aContext)->mInfo.mFuncData->mResultValue);           \
                }                                                                       \
                else                                                                    \
                {     /* KNL_EXPR_TYPE_LIST */                                          \
                    (aValue) = & ((aContext)->mInfo.mListFunc->mResultValue);           \
                }                                                                       \
            }                                                                           \
        }                                                                               \
    }

#define KNL_LIST_FUNC_GET_MULTI_VALUE( aValue, aType, aContext, aBlockIdx, aEnv  )      \
    {                                                                                   \
        if( (aType) == KNL_EXPR_TYPE_VALUE )                                            \
        {                                                                               \
            (aValue) =                                                                  \
                KNL_GET_VALUE_BLOCK_DATA_VALUE( ((aContext)->mInfo.mValueInfo),         \
                                                (aBlockIdx) );                          \
            if( ((aContext)->mCast) == NULL )                                           \
            {                                                                           \
                /* Do Nothing */                                                        \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                STL_TRY_THROW( DTL_IS_NULL( (aValue) ) == STL_FALSE, RAMP_LIST_EXIT );  \
                                                                                        \
                ((aContext)->mCast->mArgs[0].mValue.mDataValue) = (aValue);             \
                                                                                        \
                STL_TRY( ((aContext)->mCast->mCastFuncPtr)(                             \
                             DTL_CAST_INPUT_ARG_CNT,                                    \
                             ((aContext)->mCast->mArgs),                                \
                             (void *) & ((aContext)->mCast->mCastResultBuf),            \
                             (void *)((aContext)->mCast->mSrcTypeInfo),                 \
                             KNL_DT_VECTOR((aEnv)),                                     \
                             (aEnv),                                                    \
                             KNL_ERROR_STACK( (aEnv) ) )                                \
                         == STL_SUCCESS );                                              \
                                                                                        \
                (aValue) = & ((aContext)->mCast->mCastResultBuf);                       \
            }                                                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            if( ((aContext)->mCast) != NULL )                                           \
            {                                                                           \
                (aValue) = & ((aContext)->mCast->mCastResultBuf);                       \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                if( (aType) == KNL_EXPR_TYPE_FUNCTION )                                 \
                {                                                                       \
                    (aValue) = & ((aContext)->mInfo.mFuncData->mResultValue);           \
                }                                                                       \
                else                                                                    \
                {     /* KNL_EXPR_TYPE_LIST */                                          \
                    (aValue) = & ((aContext)->mInfo.mListFunc->mResultValue);           \
                }                                                                       \
            }                                                                           \
        }                                                                               \
    }

/*******************************************************************************
 * GET FUNCTION POINTER 
 ******************************************************************************/

stlStatus knlGetListFuncPtrInvalid( knlBuiltInListFuncPtr   * aFuncPtr,
                                    stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrNotSupport( knlBuiltInListFuncPtr   * aFuncPtr,
                                       stlErrorStack           * aErrorStack );

/*******************************************************************************
 * INVALID FUNCTION 
 ******************************************************************************/

stlStatus knlInvalidListFunc( stlUInt16              aInputArgumentCnt,
                              stlUInt16              aRowValueCnt,
                              knlListEntry         * aInputArgument,
                              knlExprContextInfo   * aContextInfo,
                              stlInt32               aBlockIdx,
                              stlBool              * aIsResult,
                              void                 * aResultValue,
                              void                 * aEnv );

stlStatus knlNotSupportListFunc( stlUInt16              aInputArgumentCnt,
                                 stlUInt16              aRowValueCnt,
                                 knlListEntry         * aInputArgument,
                                 knlExprContextInfo   * aContextInfo,
                                 stlInt32               aBlockIdx,
                                 stlBool              * aIsResult,
                                 void                 * aResultValue,
                                 void                 * aEnv );

/*************************************************
 * ASSIGN FUNCTION
 ************************************************/

stlStatus knlGetFuncInfoAssign( stlUInt16               aDataTypeArrayCount,
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

stlStatus knlGetFuncPtrAssign( stlUInt32             aFuncPtrIdx,
                               dtlBuiltInFuncPtr   * aFuncPtr,
                               stlErrorStack       * aErrorStack );

stlStatus knlAssign( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aInfo,
                     dtlFuncVector  * aVectorFunc,
                     void           * aVectorArg,
                     void           * aEnv );


/*************************************************
 * LIST FUNCTION
 ************************************************/


/****************************************************************************
 * IN FUNCTION
 ****************************************************************************/

/**
 * GET FUNCTION POINTER
 */ 

stlStatus knlGetListFuncPtrSingleInOnlyValue( knlBuiltInListFuncPtr   * aFuncPtr,
                                              stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrSingleIn( knlBuiltInListFuncPtr   * aFuncPtr,
                                     stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiInOnlyValue( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiIn( knlBuiltInListFuncPtr   * aFuncPtr,
                                    stlErrorStack           * aErrorStack );


/**
 * FUNCTION
 */

stlStatus knlSingleInOnlyValue( stlUInt16              aInputArgumentCnt,
                                stlUInt16              aRowValueCnt,
                                knlListEntry         * aInputArgument,
                                knlExprContextInfo   * aContextInfo,
                                stlInt32               aBlockIdx,
                                stlBool              * aIsResult,
                                void                 * aResultValue,
                                void                 * aEnv );

stlStatus knlSingleIn( stlUInt16              aInputArgumentCnt,
                       stlUInt16              aRowValueCnt,
                       knlListEntry         * aInputArgument,
                       knlExprContextInfo   * aContextInfo,
                       stlInt32               aBlockIdx,
                       stlBool              * aIsResult,
                       void                 * aResultValue,
                       void                 * aEnv );

stlStatus knlMultiInOnlyValue( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv );

stlStatus knlMultiIn( stlUInt16              aInputArgumentCnt,
                      stlUInt16              aRowValueCnt,
                      knlListEntry         * aInputArgument,
                      knlExprContextInfo   * aContextInfo,
                      stlInt32               aBlockIdx,
                      stlBool              * aIsResult,
                      void                 * aResultValue,
                      void                 * aEnv );

/****************************************************************************
 * NOT IN FUNCTION
 ****************************************************************************/

/**
 * GET FUNCTION POINTER
 */ 

stlStatus knlGetListFuncPtrSingleNotInOnlyValue( knlBuiltInListFuncPtr   * aFuncPtr,
                                                 stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrSingleNotIn( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiNotInOnlyValue( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiNotIn( knlBuiltInListFuncPtr   * aFuncPtr,
                                       stlErrorStack           * aErrorStack );

/**
 * FUNCTION
 */

stlStatus knlSingleNotInOnlyValue( stlUInt16              aInputArgumentCnt,
                                   stlUInt16              aRowValueCnt,
                                   knlListEntry         * aInputArgument,
                                   knlExprContextInfo   * aContextInfo,
                                   stlInt32               aBlockIdx,
                                   stlBool              * aIsResult,
                                   void                 * aResultValue,
                                   void                 * aEnv );

stlStatus knlSingleNotIn( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv );


stlStatus knlMultiNotInOnlyValue( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv );

stlStatus knlMultiNotIn( stlUInt16              aInputArgumentCnt,
                         stlUInt16              aRowValueCnt,
                         knlListEntry         * aInputArgument,
                         knlExprContextInfo   * aContextInfo,
                         stlInt32               aBlockIdx,
                         stlBool              * aIsResult,
                         void                 * aResultValue,
                         void                 * aEnv );

/************************************************
 * LIST OPERATION
 ************************************************/
/*******************************************************************************************
 * LIST OPERATION( EQUAL )
 ******************************************************************************************/

/**
 * GET FUNCTION POINTER
 */ 

stlStatus knlGetListFuncPtrEqualRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                     stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrNotEqualRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrLessThanRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrLessThanEqualRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrGreaterThanRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                           stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrGreaterThanEqualRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack );

/**
 * FUNCTION
 */

stlStatus knlEqualRow( stlUInt16              aInputArgumentCnt,
                       stlUInt16              aRowValueCnt,
                       knlListEntry         * aInputArgument,
                       knlExprContextInfo   * aContextInfo,
                       stlInt32               aBlockIdx,
                       stlBool              * aIsResult,
                       void                 * aResultValue,
                       void                 * aEnv );

stlStatus knlNotEqualRow( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv );

stlStatus knlLessThanRow( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv );

stlStatus knlLessThanEqualRow( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv );

stlStatus knlGreaterThanRow( stlUInt16              aInputArgumentCnt,
                             stlUInt16              aRowValueCnt,
                             knlListEntry         * aInputArgument,
                             knlExprContextInfo   * aContextInfo,
                             stlInt32               aBlockIdx,
                             stlBool              * aIsResult,
                             void                 * aResultValue,
                             void                 * aEnv );

stlStatus knlGreaterThanEqualRow( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv );

/************************************************
 * LIST OPERATION (ANY)
 ************************************************/
/*******************************************************************************************
 * LIST OPERATION PTR ( =ANY, !=ANY, < ANY, <= ANY, > ANY, >= ANY)
 ******************************************************************************************/

/**
 * GET FUNCTION POINTER (SINGLE)
 */ 
stlStatus knlGetListFuncPtrNotEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrLessThanAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrLessThanEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrGreaterThanAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                           stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrGreaterThanEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack );

/**
 * GET FUNCTION POINTER (MULTI)
 */ 
stlStatus knlGetListFuncPtrMultiNotEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiLessThanAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiLessThanEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                                  stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiGreaterThanAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiGreaterThanEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                                     stlErrorStack           * aErrorStack );

/**
 * FUNCTION
 */

stlStatus knlNotEqualAny( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv );


stlStatus knlLessThanAny( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv );


stlStatus knlLessThanEqualAny( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv );

stlStatus knlGreaterThanAny( stlUInt16              aInputArgumentCnt,
                             stlUInt16              aRowValueCnt,
                             knlListEntry         * aInputArgument,
                             knlExprContextInfo   * aContextInfo,
                             stlInt32               aBlockIdx,
                             stlBool              * aIsResult,
                             void                 * aResultValue,
                             void                 * aEnv );

stlStatus knlGreaterThanEqualAny( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv );
/**
 * MULTI
 */

stlStatus knlMultiNotEqualAny( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv );

stlStatus knlMultiLessThanAny( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv );

stlStatus knlMultiLessThanEqualAny( stlUInt16              aInputArgumentCnt,
                                    stlUInt16              aRowValueCnt,
                                    knlListEntry         * aInputArgument,
                                    knlExprContextInfo   * aContextInfo,
                                    stlInt32               aBlockIdx,
                                    stlBool              * aIsResult,
                                    void                 * aResultValue,
                                    void                 * aEnv );

stlStatus knlMultiGreaterThanAny( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv );

stlStatus knlMultiGreaterThanEqualAny( stlUInt16              aInputArgumentCnt,
                                       stlUInt16              aRowValueCnt,
                                       knlListEntry         * aInputArgument,
                                       knlExprContextInfo   * aContextInfo,
                                       stlInt32               aBlockIdx,
                                       stlBool              * aIsResult,
                                       void                 * aResultValue,
                                       void                 * aEnv );

/************************************************
 * LIST OPERATION ( ALL )
 ************************************************/

/*******************************************************************************************
 * LIST OPERATION
 * GET FUNCTION POINTER 
 ******************************************************************************************/

/**
 * GET FUNCTION POINTER ( SINGLE )
 */

stlStatus knlGetListFuncPtrEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                     stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrNotEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrLessThanAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrLessThanEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrGreaterThanAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                           stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrGreaterThanEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack );


/**
 * GET FUNCTION POINTER ( MULTI )
 */
stlStatus knlGetListFuncPtrMultiEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                          stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiNotEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiLessThanAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiLessThanEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                                  stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiGreaterThanAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack );

stlStatus knlGetListFuncPtrMultiGreaterThanEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                                     stlErrorStack           * aErrorStack );
/**
 * FUNCTION
 */


stlStatus knlEqualAll( stlUInt16              aInputArgumentCnt,
                       stlUInt16              aRowValueCnt,
                       knlListEntry         * aInputArgument,
                       knlExprContextInfo   * aContextInfo,
                       stlInt32               aBlockIdx,
                       stlBool              * aIsResult,
                       void                 * aResultValue,
                       void                 * aEnv );


stlStatus knlNotEqualAll( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv );

stlStatus knlLessThanAll( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv );

stlStatus knlLessThanEqualAll( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv );

stlStatus knlGreaterThanAll( stlUInt16              aInputArgumentCnt,
                             stlUInt16              aRowValueCnt,
                             knlListEntry         * aInputArgument,
                             knlExprContextInfo   * aContextInfo,
                             stlInt32               aBlockIdx,
                             stlBool              * aIsResult,
                             void                 * aResultValue,
                             void                 * aEnv );

stlStatus knlGreaterThanEqualAll( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv );

/**
 * MULTI
 */
stlStatus knlMultiEqualAll( stlUInt16              aInputArgumentCnt,
                            stlUInt16              aRowValueCnt,
                            knlListEntry         * aInputArgument,
                            knlExprContextInfo   * aContextInfo,
                            stlInt32               aBlockIdx,
                            stlBool              * aIsResult,
                            void                 * aResultValue,
                            void                 * aEnv );


stlStatus knlMultiNotEqualAll( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv );

stlStatus knlMultiLessThanAll( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv );

stlStatus knlMultiLessThanEqualAll( stlUInt16              aInputArgumentCnt,
                                    stlUInt16              aRowValueCnt,
                                    knlListEntry         * aInputArgument,
                                    knlExprContextInfo   * aContextInfo,
                                    stlInt32               aBlockIdx,
                                    stlBool              * aIsResult,
                                    void                 * aResultValue,
                                    void                 * aEnv );

stlStatus knlMultiGreaterThanAll( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv );

stlStatus knlMultiGreaterThanEqualAll( stlUInt16              aInputArgumentCnt,
                                       stlUInt16              aRowValueCnt,
                                       knlListEntry         * aInputArgument,
                                       knlExprContextInfo   * aContextInfo,
                                       stlInt32               aBlockIdx,
                                       stlBool              * aIsResult,
                                       void                 * aResultValue,
                                       void                 * aEnv );

/** @} */

/** @} */

#endif /* _KNL_FUNCTION_H_ */
