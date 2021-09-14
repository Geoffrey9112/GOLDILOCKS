/*******************************************************************************
 * knlExpression.h
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


#ifndef _KNL_EXPRESSION_H_
#define _KNL_EXPRESSION_H_ 1

/**
 * @file knlExpression.h
 * @brief Kernel Layer Expression Routines
 */

/**
 * @defgroup knlExpression Expression
 * @ingroup knExternal
 * @{
 */


/*
 * Expression
 * : 하나의 comparison stack으로 구성
 * : VALUE와 FUNCTION만으로 이루어진 구조
 * : 수행 결과 type은 DB type
 * : 수행 결과는 function의 결과 유형과 동일 ( dtlFuncReturnTypeClass 참고 )
 */


/**
 * @brief Expression Entry 구조
 * @remark Expression Entry는 SCAN 또는 FILTER의 연산을 수행하기 위한 STACK을 구성하기 위한 entry이다.
 *    <BR>
 *    <BR> dtlExprEntry는 Query Processor의 최적화(Optimization 단계) 이후 구성되며,
 *    <BR> execution시에 column의 value을 설정하여 사용한다.
 *    <BR>
 *    <BR> * Action 이란?
 *    <BR> => comparison stack의 top entry를 수행한 결과 또는 해당 entry를 expression result stack에 push한 후,
 *    <BR>    push한 값(dtlDataValue)에 대한 행동 지침.
 *    <BR> => dtlStackEntryActionType에 따라 역할이 구분되며, action 수행 결과값을 정할 수 있다.
 *    <BR>    또한, action 조건에 부합하면 주어진 comparison stack의 entry로 jump하여 stack 연산을 수행할 수 있다.
 *    <BR> => 분기문의 구성이나 수행이 불필요한 연산들을 건너띄기 의한 용도로 사용.
 *    <BR> 
 *    <BR> * Operation Group 이란?
 *    <BR> => function 또는 comparison operator과 이에 종속된 entry들을 묶어 operation group이라 부른다.
 *    <BR> => Operation Group 내에 구성된 또 다른 function 또는 comparison operator들도
 *    <BR>    각기 operation group을 구성하며, 이들 group간에는 부모/자식의 관계를 형성하지는 않는다.
 *    <BR> ex) func1( A, func2( B ), C )
 *    <BR> 
 *    <BR>        ============
 *    <BR>        |          |
 *    <BR>        |    C     |              <-| Group 1
 *    <BR>        |    B     | <-| Group 2    |
 *    <BR>        |  func2   | <-|            |
 *    <BR>        |    A     |              <-|
 *    <BR>        |  func1   |              <-|
 *    <BR>        ============
 *    <BR>
 *    <BR>   => Group 1 : func1, A, C   ( mOperGroupEntryNo = func1 entry # )
 *    <BR>   => Group 2 : func2, B      ( mOperGroupEntryNo = func2 entry # )
 */


/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

/** expression stack의 max entry count */
#define KNL_EXPR_MAX_ENTRY_CNT  ( STL_INT32_MAX )

/** expression stack의 exit entry number */
#define KNL_EXPR_EXIT_ENTRY     ( 0 )
#define KNL_EXPR_ROOT_GROUP_NO  ( 0 )

#define KNL_ARGUMENT_MAX_CNT    ( 8 )

#define KNL_EXPR_STACK_SIZE( sMaxEntryCount )                   \
    ( ( sMaxEntryCount + 1 ) * STL_SIZEOF( knlExprEntry ) )

#define KNL_SET_EVAL_EXPR_INFO( aEvalInfo, aBlockIdx, aBlockCnt )       \
    {                                                                   \
        (aEvalInfo)->mBlockIdx   = (aBlockIdx);                         \
        (aEvalInfo)->mBlockCount = (aBlockCnt);                         \
    }


/**
 * @brief Expression Type
 */
typedef enum knlExprType
{
    KNL_EXPR_TYPE_INVALID = 0,
    KNL_EXPR_TYPE_VALUE,         /**< value : value, column, bind */
    KNL_EXPR_TYPE_FUNCTION,      /**< function */
    KNL_EXPR_TYPE_LIST,          /**< list function, list column, row expr */
    // KNL_EXPR_TYPE_ARRAY
} knlExprType;


///////////////////////////
// Code Area 
///////////////////////////


/**
 * @brief Function Information for Expression Stack
 */
typedef struct knlFunctionInfo
{
    stlUInt32   mOffset;           /**< data 영역의 offset 값 */
    stlUInt32   mUnitEntryCount;   /**< 하나의 unit내 entry의 개수
                                    * (현재 function entry도 포함) */
    stlUInt16   mArgumentCount;    /**< function argument개수 */
    stlChar     mPadding[6];
} knlFunctionInfo;


/**
 * @brief Function Information for Expression Stack
 */
typedef struct knlValueInfo
{
    stlUInt32   mOffset;           /**< data 영역의 offset 값 */
} knlValueInfo;

/**
 * @brief List Information for Expression Stack
 */
typedef struct knlListExprInfo
{
    stlUInt32            mOffset;          /**< data 영역의 offset값 */
    dtlListPredicate     mPredicate;       /**< list function의 predicate */
    stlUInt16            mArgumentCount;   /**< list function이 갖는 entry 개수 entry의 위치표현도 포함 */
    stlChar              mPadding[2];
    stlUInt32            mListJumpNo;
} knlListExprInfo;

/**
 * @brief Expression Entry Data의 구조
 */
typedef union knlExprEntryData
{
    knlFunctionInfo    mFuncInfo;   /**< function */
    knlValueInfo       mValueInfo;  /**< value, column, bind param, bind column */
    knlListExprInfo    mListInfo;   /**< list function, list column, row expr */
} knlExprEntryData;

/**
 * @brief Expression Entry의 구조
 */
typedef struct knlExprEntry
{
    /** mExpr을 구분하는 정보 */
    knlExprType           mExprType;           /**< expression type */

    /** Operation Group 분석을 위한 정보 */
    stlUInt32             mOperGroupEntryNo;   /**< 해당 entry가 속한
                                                * Group의 시작 entry 번호 */

    /** entry 수행 여부 관련 정보 */
    stlUInt32             mActionJumpEntryNo;  /**< expression result stack의
                                                * top entry에 대한 ACTION 수행시
                                                * 이동할 expression entry의 위치 */
    
    dtlActionType         mActionType;         /**< expression result stack의
                                                * top entry에 대한 ACTION 타입 */
    
    dtlActionReturnValue  mActionReturnValue;  /**< expression result stack의
                                                * top entry에 대한 ACTION 결과 */

    knlExprEntryData      mExpr;               /**< Expression 정보 */

} knlExprEntry;


/**
 * @brief List Entry Value의 구조
 */
typedef struct knlListEntry
{
    knlExprType           mType;
    stlUInt32             mOffset;
    stlUInt16             mIdx;
    stlChar               mPadding[6];
} knlListEntry;

/**
 * @brief Duplicated Function의 결과를 저장하기 위한 Entry 
 * 
 * @remark Expr Stack의 일부에 dup entry 영역을 구성함
 */
// typedef struct dtlDupEntry
// {
//     /* 수행 이전에 설정될 부분 */
//     stlChar     * mResultBuffer;        /**< function 수행 결과가 저장될 result buffer */

//     /* entry 구성시 설정될 부분 */
//     stlUInt16     mDupCount;            /**< dup entry를 참조하는 duplicated function 개수 */

//     /* 수행 중 설정될 부분 */
//     stlBool       mIsProcessed;         /**< dup entry에 수행 결과가 저장되어 있는지 여부 */
//     stlChar       mPadding[5];
// } dtlDupEntry;

/**
 * @brief Expression Composition
 */ 
typedef enum knlExprComposition
{
    KNL_EXPR_COMPOSITION_NONE = 0,
    KNL_EXPR_COMPOSITION_COMPLEX,          /**< complex composition */
    KNL_EXPR_COMPOSITION_SIMPLE,           /**< simple composition */
    KNL_EXPR_COMPOSITION_VALUE_ONLY        /**< value only */
} knlExprComposition;

/**
 * @brief Expression Stack
 */ 
typedef struct knlExprStack
{
    stlUInt32            mMaxEntryCount;    /**< stack의 크기 (knlExprEntry 개수) */
    stlUInt32            mEntryCount;       /**< stack내에 저장된 entry 개수 */
    knlExprComposition   mExprComposition;  /**< expression composition */
    stlChar              mPadding[4];
    knlExprEntry       * mEntries;          /**< expr entry로 구성된 stack : Array */

} knlExprStack;


/**
 * @brief Argument Stack
 */ 
typedef struct knlArgStack
{
    stlUInt32        mMaxEntryCount;    /**< stack의 크기 (knlArgEntry 개수) */
    stlUInt32        mEntryCount;       /**< stack내에 저장된 entry 개수 */
    dtlValueEntry  * mEntries;          /**< argument entry로 구성된 stack : Array */
} knlArgStack;


/**
 * @brief List Function Stack
 */ 
typedef struct knlListStack
{
    stlUInt32        mMaxEntryCount;    /**< stack의 크기 (knlArgEntry 개수) */
    stlUInt32        mEntryCount;       /**< stack내에 저장된 entry 개수 */
    knlListEntry   * mEntries;          /**< argument entry로 구성된 stack : Array */
} knlListStack;


///////////////////////////
// Data Area 
///////////////////////////

#define KNL_CAST_FUNC_ARG_COUNT  ( DTL_CAST_INPUT_ARG_CNT )

/**
 * @brief List function Type
 */ 
#define KNL_LIST_FUNC_START_ID          ( KNL_BUILTIN_FUNC_IS_EQUAL_ROW )
#define KNL_IN_PHYSICAL_FUNC_START_ID   ( KNL_BUILTIN_FUNC_IN )
#define KNL_LIST_FUNC_END_ID            ( KNL_BUILTIN_FUNC_DUMP - 1 )

#define KNL_LIST_FUNC_ID_PTR( sFuncID )         \
    ( sFuncID - KNL_LIST_FUNC_START_ID )

#define KNL_IN_PHYSICAL_FUNC_ID_PTR( sFuncID )         \
    ( sFuncID - KNL_IN_PHYSICAL_FUNC_START_ID )

typedef enum knlListFuncType
{
    KNL_LIST_FUNCTION_SINGLE_ROW_ONLY_VALUES = 0,
    /**< Row당 하나의 value만을 갖고 있고 모두 KNL_EXPR_TYPE_VALUE만으로 구성되있다. */

    KNL_LIST_FUNCTION_SINGLE_ROW_ALL_EXPR,
    /**< Row당 하나의 value만을 갖고 있고 모든 EXPR를 포함한다. */
    
    KNL_LIST_FUNCTION_MULTI_ROW_ONLY_VALUES,
    /**< Row당 여러개의 value를 갖고 있고 모두 KNL_EXPR_TYPE_VALUE만으로 구성되있다. */

    KNL_LIST_FUNCTION_MULTI_ROW_ALL_EXPR,
    /**< Row당 여러개의 value를 갖고 있고  모든 EXPR를 포함한다. */

    
    KNL_LIST_FUNCTION_NA,                       /**< N/A */
    
}knlListFuncType ;



/**
 * @brief builtin List function의 함수 원형
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aRowValueCnt      input Row Value Count
 * @param[in]  aInputArgument    input argument
 * @param[in]  aContextInfo      Context info
 * @param[in]  aBlockIdx         EvalInfo Block Idx
 * @param[in]  aIsResult         List Function Result 
 * 
 *  
 * @param[out] aResultValue      결과
 *             <BR> result결과가 arrary인 경우의 확장을 위해 void *를 output 인자로 받는다.
 *             <BR>
 *             <BR> 결과값의 갯수 차이에 따라 사용상의 차이가 발생하는데, 
 *             <BR> (1) 결과값이 하나인 함수들은 dtlDataValue로 casting해서 결과값을 만든다.
 *             <BR> (2) 결과값이 여러개인 함수들에 대한 고려는 이후에 ...
 *             <BR>
 * @param[out] aEnv              environment
 *             <BR> result결과가 arrary인 경우의 확장을 위해
 *             <BR> Environment를 void *의 output 인자로 받는다.
 *             <BR>
 *             <BR> DataType layer와 Execution layer에서 사용상의 차이가 발생하는데,
 *             <BR> result결과가 여러개인 함수는 execution library에 구현하기로 논의됨.
 */
typedef stlStatus (*knlBuiltInListFuncPtr) ( stlUInt16              aInputArgumentCnt,
                                             stlUInt16              aRowValueCnt,
                                             knlListEntry         * aInputArgument,
                                             knlExprContextInfo   * aContextInfo,
                                             stlInt32               aBlockIdx,
                                             stlBool              * aIsResult,
                                             void                 * aResultValue,
                                             void                 * aEnv );


/**
 * @brief builtIn function의 function pointer를 얻는 함수
 * @param[out] aFuncPtr      function pointer
 * @param[out] aErrorStack   에러스택정보 
 */ 
typedef stlStatus (*knlGetBuiltInListFuncPtr) ( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack );

/**
 * @brief List Function Argument Stack 
 */ 
typedef struct knlListFunction
{
    knlListStack          * mListStack;
    knlBuiltInListFuncPtr   mListFuncPtr;        /**< List function pointer */
    stlUInt16               mValueCount;         /**< Row의 Value Argument 개수*/
    stlChar                 mPadding[2];
    knlListFuncType         mListFuncType;       
    dtlDataValue            mResultValue;        /**< function result value buffer */
} knlListFunction;


/**
 * @brief Function Data for Expression Context
 */
typedef struct knlFunctionData
{   
    dtlBuiltInFuncPtr       mFuncPtr;        /**< function pointer */
    dtlDataValue            mResultValue;    /**< function result value buffer */

    void                  * mInfo;           /**< function의 부가적인 info
                                              * 예) to_char 의 format string info
                                              *     like 의 patter string info
                                              *     cast의 src value의 data type info
                                              */
} knlFunctionData;

/**
 * @brief Expression Entry를 구성하기 위한 Function 정보
 * 
 * @remark
 *    <BR> < Execution 전에 전처리가 필요한 정보들 >
 *    <BR> 1. mFuncPtr : mFuncID를 통해 실제 function pointer를 얻어온다.
 *    <BR> 2. mResultBuffer : function 수행의 결과가 저장될 공간을 할당 받아 놓는다.
 *    <BR>                    미리 할당 받은 메모리 공간을 사용하여,
 *    <BR>                    function 수행시마다 메모리를 할당 받지 않도록 한다.
 *    <BR>                    만약, duplicated function이면 mResultBuffer 대신,
 *    <BR>                    dup entry에 공간을 할당 받아 놓도록 하여 
 *    <BR>                    duplicated function들이 중복으로 할당 받지 않게 한다.
 *    <BR>                    (function 수행시에도 mResultBuffer 대신 dup entry 사용)
 *    <BR>
 *    <BR> < Duplicated Function >
 *    <BR> 한 function이 저장되어 있는 comparison stack내에
 *    <BR> 한 function과 동일한 구조(argument)를 가지는 동일한 function이 존재한다면,
 *    <BR> 이들을 Duplicated Function으로 정의한다.
 *    <BR> 
 *    <BR> Duplicated Function으로 지정된 function들은 수행한 결과를 공유하도록 하여
 *    <BR> 동일한 함수에 대한 반복 수행을 방지한다.
 *    <BR>
 *    <BR> A. Duplicated Function의 조건
 *    <BR>   1. 함수의 결과가 일정한 수의 dtlDataValue를 반환
 *    <BR>   2. 함수의 Argument에 대한 Expression Entry 구성이 동일
 *    <BR>   3. 동일한 입력에 대한 항상 동일한 결과를 산출하는 function
 */
// typedef struct knlFunctionInfo
// {
//     /* 기본 함수 정보 */
//     stlUInt32           mFuncID;           /**< function ID */
//     stlUInt16           mArgumentCount;    /**< function 수행에 필요한 argument개수
//                                             * argument : dtlDataValue */
//     stlUInt16           mUnitEntryCount;   /**< 하나의 unit내에 있는 entry의 개수 (현재 function entry도 포함) */

//     dtlFuncReturnTypeClass 
//                         mReturnType;       /**< dtlFuncReturnTypeClass : return value들의 type은 모두 일치해야함 */
//     dtlDataType         mReturnDataType;   /**< return value의 data type */

//     stlUInt32           mResultBufferSize; /**< fuction 수행 결과가 저장될 result buffer의 크기 */
//     stlUInt32           mFuncIdx;          /**< function pointer를 얻기위한 식별자 */
    
//     /* Duplicated Function 수행을 위한 정보 */
//     stlBool             mIsDuplicated;     /**< 현재 function과 동일한 구성을 가지는 function이 있는지 여부 */
//     stlUInt16           mDupEntryNo;       /**< duplicated function 결과물이 저장될 duplication entry의 번호 */
    
//     /* execution 전에 전처리 과정을 통해 설정되는 정보 */
//     dtlBuiltInFuncPtr   mFuncPtr;          /**< function pointer */
//     stlChar           * mResultBuffer;     /**< fuction 수행 결과가 저장될 result buffer */
// } dtlFunctionInfo;


/**
 * @brief Cast Function Info
 */
struct knlCastFuncInfo
{
    dtlBuiltInFuncPtr   mCastFuncPtr;    /**< cast function pointer */
    dtlDataValue        mCastResultBuf;  /**< cast result buffer */

    dtlValueEntry       mArgs[KNL_CAST_FUNC_ARG_COUNT];
    /**< arguments :  (순서대로)
     * value / src precision / dest precision /
     * dest scale / dest interval indicator
     * value 부분은 evaluate시 top argument로 대체.
     */

    dtlDataTypeInfo   * mSrcTypeInfo;

};

/**
 * @brief Expression Context
 */
typedef struct knlExprContext
{
    union
    {
        knlFunctionData    * mFuncData;     /**< function data */
        knlValueBlock      * mValueInfo;    /**< value block */
        knlListFunction    * mListFunc;     /**< list function data */
    } mInfo;
    
    knlCastFuncInfo   * mCast;
} knlExprContext;


/**
 * @brief Expression Context 정보
 */
struct knlExprContextInfo
{
    stlInt32             mCount;           /**< context 개수 */
    knlExprContext     * mContexts;        /**< context 리스트 (array) */
    knlValueStack      * mArgStack;        /**< expression stack 평가시 argument들을 임시 저장하기 위한 argument stack */
};


/**
 * @brief Expression Evaluation 정보
 */
typedef struct knlExprEvalInfo
{
    knlExprStack        * mExprStack;
    knlExprContextInfo  * mContext;
    knlValueBlockList   * mResultBlock;     /**< predicate 결과가 저장될 result value block */
    
    stlInt32              mBlockIdx;
    stlInt32              mBlockCount;
} knlExprEvalInfo;


/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/* 생성하는 곳에서 entry count의 정확한 분석이 이루어져야 한다. */
stlStatus knlExprCreate( knlExprStack   ** aExpr,
                         stlUInt32         aMaxEntryCount,
                         knlRegionMem    * aMemMgr,
                         knlEnv          * aEnv );

stlStatus knlExprInit( knlExprStack    * aExpr,
                       stlUInt32         aMaxEntryCount,
                       knlRegionMem    * aMemMgr,
                       knlEnv          * aEnv );

stlStatus knlExprCopy( knlExprStack    * aSrcExpr,
                       knlExprStack   ** aDstExpr,
                       knlRegionMem    * aMemMgr,
                       knlEnv          * aEnv );

stlStatus knlAnalyzeExprStack( knlExprStack * aExprStack,
                               knlEnv       * aEnv );

stlStatus knlExprAddFunction( knlExprStack            * aExpr,
                              knlBuiltInFunc            aFuncID,
                              stlBool                   aIsExprCompositionComplex,
                              stlUInt32                 aFuncOffset,
                              stlUInt16                 aArgCnt,
                              stlUInt32                 aUnitEntryCnt,
                              dtlActionType             aActionType,
                              dtlActionReturnValue      aActionReturnValue,
                              stlUInt32                 aActionJumpNo,
                              stlUInt32               * aGroupNo,
                              knlRegionMem            * aMemMgr,
                              knlEnv                  * aEnv );

stlStatus knlExprAddValue( knlExprStack          * aExprStack,
                           stlUInt32               aGroupEntryNo,
                           stlUInt32               aValueOffset,
                           dtlActionType           aActionType,
                           dtlActionReturnValue    aActionReturnValue,
                           stlUInt32               aActionJumpNo,
                           knlRegionMem          * aMemMgr,
                           knlEnv                * aEnv );

stlStatus knlExprAddListExpr( knlExprStack            * aExprStack,
                              knlBuiltInFunc            aFuncID,
                              stlBool                   aIsExprCompositionComplex,
                              stlUInt32                 aListOffset,
                              dtlListPredicate          aPredicate,
                              stlUInt16                 aArgCnt,
                              stlUInt32                 aUnitEntryCnt,
                              dtlActionType             aActionType,
                              dtlActionReturnValue      aActionReturnValue,
                              stlUInt32                 aActionJumpNo,
                              stlUInt32                 aListJumpNo,
                              stlBool                   aAddFirstExpr,
                              knlRegionMem            * aMemMgr,
                              knlEnv                  * aEnv );

stlStatus knlEvaluateBlockExpression( knlExprEvalInfo  * aBlockExprEvalInfo,
                                      knlEnv           * aEnv );
stlStatus knlEvaluateBlockValueOnlyExpression( knlExprEvalInfo  * aBlockExprEvalInfo,
                                               knlEnv           * aEnv );
stlStatus knlEvaluateBlockSimpleExpression( knlExprEvalInfo  * aBlockExprEvalInfo,
                                            knlEnv           * aEnv );
stlStatus knlEvaluateBlockComplexExpression( knlExprEvalInfo  * aBlockExprEvalInfo,
                                             knlEnv           * aEnv );

stlStatus knlEvaluateJoinFilter( knlExprEvalInfo     * aJoinEvalInfo,
                                 stlInt32              aRightBlockIdx,
                                 stlBool             * aResult,
                                 knlEnv              * aEnv );

stlStatus knlEvaluateOneExpression( knlExprEvalInfo     * aExprEvalInfo,
                                    knlEnv              * aEnv );
stlStatus knlEvaluateValueOnlyExpression( knlExprEvalInfo     * aExprEvalInfo,
                                          knlEnv              * aEnv );
stlStatus knlEvaluateSimpleExpression( knlExprEvalInfo     * aExprEvalInfo,
                                       knlEnv              * aEnv );
stlStatus knlEvaluateComplexExpression( knlExprEvalInfo     * aExprEvalInfo,
                                        knlEnv              * aEnv );


stlStatus knlCreateContextInfo( stlInt32               aContextCnt,
                                knlExprContextInfo  ** aContextInfo,
                                knlRegionMem         * aMemMgr,
                                knlEnv               * aEnv );

/*******************************************************************************
 * VALUE BLOCK
 ******************************************************************************/

stlStatus knlCreateStackBlock( knlValueBlock        ** aValueBlock,
                               dtlDataValue          * aDataValue,
                               stlInt64                aBufferSize,
                               dtlDataType             aDBType,
                               stlInt64                aArgNum1,
                               stlInt64                aArgNum2,
                               dtlStringLengthUnit     aStringLengthUnit,
                               dtlIntervalIndicator    aIntervalIndicator,
                               knlRegionMem          * aRegionMem,
                               knlEnv                * aEnv );

/** @} */

#endif /* _KNL_EXPRESSION_H_ */
