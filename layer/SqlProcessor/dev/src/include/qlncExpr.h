/*******************************************************************************
 * qlncExpr.h
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLNCEXPR_H_
#define _QLNCEXPR_H_ 1

/**
 * @file qlncExpr.h
 * @brief SQL Processor Layer Candidate Optimization Node
 */

#include <qlDef.h>

/**
 * @addtogroup qlnc
 * @{
 */


/*******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

/**
 * @brief Expression이 사용되는 Phrase에 대한 Flags
 */
#define QLNC_EXPR_PHRASE_TARGET         (stlInt32)0x00000001    /**< Target절에 존재 */
#define QLNC_EXPR_PHRASE_WHERE          (stlInt32)0x00000002    /**< Where에 존재 */
#define QLNC_EXPR_PHRASE_GROUPBY        (stlInt32)0x00000004    /**< Group By에 존재 */
#define QLNC_EXPR_PHRASE_HAVING         (stlInt32)0x00000008    /**< Group By에 존재 */
#define QLNC_EXPR_PHRASE_ORDERBY        (stlInt32)0x00000010    /**< Order By에 존재 */
#define QLNC_EXPR_PHRASE_OFFSET_LIMIT   (stlInt32)0x00000020    /**< Offset/Limit에 존재 */
#define QLNC_EXPR_PHRASE_RETURN         (stlInt32)0x00000040    /**< Return에 존재 */

/**
 * @brief expression type
 */
enum qlncExprType
{
    QLNC_EXPR_TYPE_VALUE = 0,
    QLNC_EXPR_TYPE_BIND_PARAM,
    QLNC_EXPR_TYPE_COLUMN,
    QLNC_EXPR_TYPE_FUNCTION,
    QLNC_EXPR_TYPE_CAST,
    QLNC_EXPR_TYPE_PSEUDO_COLUMN,
    QLNC_EXPR_TYPE_PSEUDO_ARGUMENT,
    QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT,
    QLNC_EXPR_TYPE_SUB_QUERY,
    QLNC_EXPR_TYPE_REFERENCE,
    QLNC_EXPR_TYPE_INNER_COLUMN,
    QLNC_EXPR_TYPE_OUTER_COLUMN,
    QLNC_EXPR_TYPE_ROWID_COLUMN,
    QLNC_EXPR_TYPE_AGGREGATION,
    QLNC_EXPR_TYPE_CASE_EXPR,
    QLNC_EXPR_TYPE_LIST_FUNCTION,
    QLNC_EXPR_TYPE_LIST_COLUMN,
    QLNC_EXPR_TYPE_ROW_EXPR,

    QLNC_EXPR_TYPE_AND_FILTER,
    QLNC_EXPR_TYPE_OR_FILTER,

    QLNC_EXPR_TYPE_BOOLEAN_FILTER,
    QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER,

    QLNC_EXPR_TYPE_MAX
};


/**
 * @brief expression value type
 */
enum qlncValueType
{
    QLNC_VALUE_TYPE_INVALID = 0,
    QLNC_VALUE_TYPE_BOOLEAN,
    QLNC_VALUE_TYPE_BINARY_INTEGER,
    QLNC_VALUE_TYPE_BINARY_REAL,
    QLNC_VALUE_TYPE_BINARY_DOUBLE,
    QLNC_VALUE_TYPE_NUMERIC,
    QLNC_VALUE_TYPE_STRING_FIXED,
    QLNC_VALUE_TYPE_STRING_VARYING,
    QLNC_VALUE_TYPE_BITSTRING_FIXED,
    QLNC_VALUE_TYPE_BITSTRING_VARYING,
    QLNC_VALUE_TYPE_DATE_LITERAL_STRING,
    QLNC_VALUE_TYPE_TIME_LITERAL_STRING,
    QLNC_VALUE_TYPE_TIME_TZ_LITERAL_STRING,
    QLNC_VALUE_TYPE_TIMESTAMP_LITERAL_STRING,
    QLNC_VALUE_TYPE_TIMESTAMP_TZ_LITERAL_STRING,
    QLNC_VALUE_TYPE_INTERVAL_YEAR_TO_MONTH_STRING,
    QLNC_VALUE_TYPE_INTERVAL_DAY_TO_SECOND_STRING,
    QLNC_VALUE_TYPE_ROWID_STRING,
    QLNC_VALUE_TYPE_NULL
};

typedef enum
{
    QLNC_FILTER_TYPE_INVALID = 0,

    /* filter list */
    QLNC_FILTER_TYPE_AND,                       /**< AND */
    QLNC_FILTER_TYPE_OR,                        /**< OR */

    /* simple filter (unary argument) */
    QLNC_FILTER_TYPE_NOT,                       /**< NOT */
    QLNC_FILTER_TYPE_IS_NULL,                   /**< IS NULL */
    QLNC_FILTER_TYPE_IS_NOT_NULL,               /**< IS NOT NULL */
    QLNC_FILTER_TYPE_IS_UNKNOWN,                /**< IS UNKNOWN */
    QLNC_FILTER_TYPE_IS_NOT_UNKNOWN,            /**< IS NOT UNKNOWN */

    /* simple filter (binary argument) */
    QLNC_FILTER_TYPE_IS_EQUAL,                  /**< IS EQUAL */
    QLNC_FILTER_TYPE_IS_NOT_EQUAL,              /**< IS NOT EQUAL */
    QLNC_FILTER_TYPE_IS_LESS_THAN,              /**< IS LESS THAN */
    QLNC_FILTER_TYPE_IS_LESS_THAN_EQUAL,        /**< IS LESS_THAN EQUAL */
    QLNC_FILTER_TYPE_IS_GREATER_THAN,           /**< IS GREATER THAN */
    QLNC_FILTER_TYPE_IS_GREATER_THAN_EQUAL,     /**< IS GREATER THAN EQUAL */
    QLNC_FILTER_TYPE_IS,                        /**< IS */
    QLNC_FILTER_TYPE_IS_NOT,                    /**< IS NOT */

    /* complex filter (between) */
    QLNC_FILTER_TYPE_BETWEEN,                   /**< BETWEEN */
    QLNC_FILTER_TYPE_NOT_BETWEEN,               /**< NOT BETWEEN */
    QLNC_FILTER_TYPE_BETWEEN_SYMMETRIC,         /**< BETWEEN SYMMETRIC */
    QLNC_FILTER_TYPE_NOT_BETWEEN_SYMMETRIC,     /**< NOT BETWEEN SYMMETRIC */

    /* complex filter (In) */
    QLNC_FILTER_TYPE_IN,                        /**< IN */
    QLNC_FILTER_TYPE_NOT_IN,                    /**< NOT IN */

    /* complex filter (like) */
    QLNC_FILTER_TYPE_LIKE,                      /**< LIKE */
    QLNC_FILTER_TYPE_NOT_LIKE,                  /**< NOT LIKE */

    /* complex filter (cast) */
    QLNC_FILTER_TYPE_CAST,                      /**< CAST */

    /* complex filter (Case) */
    QLNC_FILTER_TYPE_CASE,                      /**< CASE */
    QLNC_FILTER_TYPE_NULLIF,                    /**< NULLIF */
    QLNC_FILTER_TYPE_COALESCE,                  /**< COALESCE */

    /* complex filter (Nvl) */
    QLNC_FILTER_TYPE_NVL,                       /**< NVL */
    QLNC_FILTER_TYPE_NVL2,                      /**< NVL2 */

    QLNC_FILTER_TYPE_MAX
} qlncFilterType;


#define QLNC_INIT_LIST( _aList )    \
{                                   \
    (_aList)->mCount = 0;           \
    (_aList)->mHead = NULL;         \
    (_aList)->mTail = NULL;         \
}


#define QLNC_COPY_LIST_INFO( _aSrcList, _aDstList ) \
{                                                   \
    (_aDstList)->mCount = (_aSrcList)->mCount;      \
    (_aDstList)->mHead = (_aSrcList)->mHead;        \
    (_aDstList)->mTail = (_aSrcList)->mTail;        \
}


#define QLNC_APPEND_ITEM_TO_LIST( _aList, _aItem )  \
{                                                   \
    if( (_aList)->mCount == 0 )                     \
    {                                               \
        (_aList)->mHead = (_aItem);                 \
        (_aList)->mTail = (_aItem);                 \
    }                                               \
    else                                            \
    {                                               \
        (_aList)->mTail->mNext = (_aItem);          \
        (_aList)->mTail = (_aItem);                 \
    }                                               \
    (_aList)->mCount++;                             \
}


#define QLNC_INIT_AND_FILTER( _aAndFilter )                     \
{                                                               \
    (_aAndFilter)->mCommon.mType = QLNC_EXPR_TYPE_AND_FILTER;   \
    (_aAndFilter)->mOrCount = 0;                                \
    (_aAndFilter)->mOrFilters = NULL;                           \
}


#define QLNC_IS_EXIST_AND_FILTER( _aFilter )                                    \
    ( ((_aFilter) != NULL) && (((qlncAndFilter*)(_aFilter))->mOrCount > 0) )


#define QLNC_SET_EXPR_COMMON( _aCommon,                                 \
                              _aMem,                                    \
                              _aEnv,                                    \
                              _aType,                                   \
                              _aExprPhraseFlag,                         \
                              _aIterationTime,                          \
                              _aPosition,                               \
                              _aColumnName,                             \
                              _aDataType )                              \
    {                                                                   \
        (_aCommon)->mType = (_aType);                                   \
        (_aCommon)->mExprPhraseFlag = (_aExprPhraseFlag);               \
        (_aCommon)->mIterationTime = (_aIterationTime);                 \
        (_aCommon)->mPosition = (_aPosition);                           \
        (_aCommon)->mDataType = (_aDataType);                           \
        (_aCommon)->mNullable = STL_TRUE;                               \
        QLNC_ALLOC_AND_COPY_STRING( (_aCommon)->mColumnName,            \
                                    (_aColumnName),                     \
                                    (_aMem),                            \
                                    (_aEnv) );                          \
    }


/*******************************************************************************
 * SRUCTURES
 ******************************************************************************/

/**
 * @brief Reference Expression Item
 */
struct qlncRefExprItem
{
    qlncExprCommon      * mExpr;
    qlncRefExprItem     * mNext;
};


/**
 * @brief Reference Expression List
 */
struct qlncRefExprList
{
    stlInt32              mCount;
    qlncRefExprItem     * mHead;
    qlncRefExprItem     * mTail;
};


/**
 * @brief Reference Column Item
 */
struct qlncRefColumnItem
{
    stlInt32              mIdx;
    stlInt32              mRefCount;
    qlncExprCommon      * mExpr;
    qlncRefColumnItem   * mNext;
};


/**
 * @brief Reference Column List
 */
struct qlncRefColumnList
{
    stlInt32              mCount;
    qlncRefColumnItem   * mHead;
    qlncRefColumnItem   * mTail;
};


/**
 * @brief Reference Node Item
 */
struct qlncRefNodeItem
{
    qlncNodeCommon      * mNode;
    qlncRefColumnList     mRefColumnList;
    qlncRefNodeItem     * mNext;
};


/**
 * @brief Reference Node List
 */
struct qlncRefNodeList
{
    stlInt32              mCount;
    qlncRefNodeItem     * mHead;
    qlncRefNodeItem     * mTail;
};


/**
 * @brief Named Column Item
 */
struct qlncNamedColumnItem
{
    qlncExprCommon      * mLeftExpr;
    qlncExprCommon      * mRightExpr;
    qlncNamedColumnItem * mNext;
};


/**
 * @brief Named ColumnList
 */
struct qlncNamedColumnList
{
    stlInt32              mCount;
    qlncNamedColumnItem * mHead;
    qlncNamedColumnItem * mTail;
};


/**
 * @brief Expression
 */
struct qlncExprCommon
{
    qlncExprType          mType;            /**< Expression Type */
    stlInt32              mExprPhraseFlag;  /**< Expression이 사용된 Phrase */
    dtlIterationTime      mIterationTime;   /**< 평가 반복 수행 옵션 */
    stlInt32              mPosition;        /**< Stmt내 현재 Expr 시작 위치 */
    stlChar             * mColumnName;      /**< column name */

    /* candidate plan을 위해 필요한 정보 */
    dtlDataType           mDataType;        /**< Data Type : target type */
    stlBool               mNullable;        /**< Nullable인지 여부 */
};


/**
 * @brief Value Expression
 */
struct qlncExprValue
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    qlncValueType         mValueType;       /**< Literal Value Type */
    stlInt32              mInternalBindIdx; /**< internal bind variable index */
    
    union
    {
        stlInt64          mInteger;         /**< boolean */
        stlChar         * mString;          /**< float, string, bitstring */
    } mData;                                /**< null : mData가 없음 */
};


/**
 * @brief Bind Parameter Expression
 */
struct qlncExprBindParam
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    knlBindType           mBindType;        /**< bind type */
    stlInt32              mBindParamIdx;    /**< bind parameter index */
    stlChar             * mHostName;        /**< parameter name */
    stlChar             * mIndicatorName;   /**< indicator name */
};


/**
 * @brief Column Expression
 */
struct qlncExprColumn
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    qlncNodeCommon      * mNode;            /**< Related Node */
    ellDictHandle       * mColumnHandle;    /**< Relation's Column Handle */
    stlInt64              mColumnID;        /**< Column ID */
    stlInt32              mColumnPos;       /**< Column Position */
};


/**
 * @brief Function Expression
 */
struct qlncExprFunction
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    knlBuiltInFunc        mFuncId;          /**< Function ID */
    stlInt32              mArgCount;        /**< Function Argument Count */
    qlncExprCommon     ** mArgs;            /**< Func Argument List : array */
};


/**
 * @brief Type Definition Expression
 */
struct qlncExprTypeDef
{
    dtlDataType           mDBType;              /**< DB Type */
    stlChar             * mUserTypeName;        /**< 사용자가 기술한 DB Type 이름 */
    stlInt64              mArgNum1;             /**< 첫번째 숫자 (TYPE 마다 용도가 다름) */
    stlInt64              mArgNum2;             /**< 두번째 숫자 (TYPE 마다 용도가 다름) */
    dtlStringLengthUnit   mStringLengthUnit;    /**< only for STRING 계열 Type */
    dtlIntervalIndicator  mIntervalIndicator;   /**< only for INTERVAL Type */
    stlInt64              mNumericRadix;        /**< only for NUMERIC Type */
};


/**
 * @brief Type Cast Expression
 */
struct qlncExprTypeCast
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    qlncExprTypeDef       mTypeDef;         /**< Target Type */
    stlInt32              mArgCount;        /**< Type Cast Argument Count */
    qlncExprCommon     ** mArgs;            /**< Cast Function Argument List : array */
};


/**
 * @brief Pseudo Column Expression
 */
struct qlncExprPseudoCol
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    knlPseudoCol          mPseudoId;        /**< pseudo column ID */
    stlInt32              mArgCount;        /**< pseudo column argument count */
    ellDictHandle       * mSeqDictHandle;   /**< sequence dictionary handle */
    ellPseudoArg        * mArgs;            /**< pseudo column argument list : array */

    /* list 구성시 자동으로 설정 */
    stlInt32              mPseudoOffset;    /**< offset of pseudo column list */
};


/**
 * @brief Constant Expression
 */
struct qlncExprConstExpr
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    qlncExprCommon      * mOrgExpr;         /**< Original Expression */
};


/**
 * @brief SubQuery Expression
 */
struct qlncExprSubQuery
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    qlncNodeCommon      * mNode;            /**< Query Node */
    stlInt32              mRefQBID;         /**< 참조하는 Query Block ID */
    stlInt8               mRefIdx;          /**< SubQuery내의 Target에서 참조하는 Target Idx */

    stlBool               mIsRelSubQuery;   /**< Relation Subquery 여부 */

    knlBuiltInFunc        mRelatedFuncId;   /**< related function's ID : knlBuiltInFunc */

    stlInt32              mTargetCount;     /**< Target Count */
    qlncTarget          * mTargetArr;       /**< Target Array Pointer */

    qlncRefExprList     * mRefExprList;     /**< Outer Table을 참조하는 Column Expression들 List */
};


/**
 * @brief Reference Expression
 */
struct qlncExprReference
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    qlncExprCommon      * mOrgExpr;         /**< Original Expression */
};


/**
 * @brief Inner Column Expression
 */
struct qlncExprInnerColumn
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    qlncNodeCommon      * mNode;            /**< Related Node */
    stlInt32              mIdx;             /**< Subquery의 Target Index(base 0), JoinTable의 Index */
    qlncExprCommon      * mOrgExpr;         /**< Original Expression */
};


/**
 * @brief Outer Column Expression
 */
struct qlncExprOuterColumn
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    qlncExprCommon      * mOrgExpr;         /**< Original Expression */
};


/**
 * @brief Rowid Column Expression
 */
struct qlncExprRowidColumn
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    qlncNodeCommon      * mNode;            /**< Related Node */
    stlInt32              mIdx;
};


/**
 * @brief Aggregation Expression
 */
struct qlncExprAggregation
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    knlBuiltInAggrFunc    mAggrId;          /**< Aggregation ID */
    stlInt32              mArgCount;        /**< Aggregation Argument Count */
    qlncExprCommon     ** mArgs;            /**< Aggregation Argument List : array */
    stlBool               mIsDistinct;      /**< Distinct 여부 */
    stlBool               mHasNestedAggr;   /**< Nested Aggregation을 포함하고 있는지 여부 */
    qlncExprCommon      * mFilter;          /**< Filter */
};


/**
 * @brief Case Expression
 */
struct qlncExprCaseExpr
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    knlBuiltInFunc        mFuncId;          /**< Function ID */    
    stlInt32              mCount;           /**< When Clause Count  */
    qlncExprCommon     ** mWhenArr;         /**< When Expr */
    qlncExprCommon     ** mThenArr;         /**< Then Expr */
    qlncExprCommon      * mDefResult;       /**< Default Result */
};


/**
 * @brief List Function Expression
 */
struct qlncExprListFunc
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    knlBuiltInFunc        mFuncId;          /**< Function ID */
    qlvListFuncType       mListFuncType;    /**< List Function Type : NESTED/CORRELATED */
    stlInt32              mArgCount;        /**< Argument Count */
    qlncExprCommon     ** mArgs;            /**< Argument List */
};


/**
 * @brief List Column Expression
 */
struct qlncExprListCol
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    dtlListPredicate      mPredicate;       /**< Predicate */
    stlUInt16             mArgCount;        /**< Argument Count */
    qlncExprCommon     ** mArgs;            /**< Argument List */
    knlListEntry       ** mEntry;           /**< List Stack Entry */
};


/**
 * @brief Row Expression
 */
struct qlncExprRowExpr
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    stlUInt32             mArgCount;        /**< Argument Count */
    qlncExprCommon     ** mArgs;            /**< Argument List */
    knlListEntry        * mEntry;           /**< List Stack Entry */
};


/**
 * @brief Pseudo Argument Expression
 */
struct qlncExprPseudoArg
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    void                * mPseudoArg;       /**< Pseudo Argument */
};


/**
 * @brief Target Expression
 */
struct qlncTarget
{
    stlInt32              mDisplayPos;      /**< Display Position */
    stlChar             * mDisplayName;     /**< Display Name */
    stlChar             * mAliasName;       /**< Alias Name */
    qlncExprCommon      * mExpr;            /**< Target Expression */

    qlvInitDataTypeInfo   mDataTypeInfo;    /**< Data Type Info */
};


/**
 * @brief Expression Map for List Column
 */
struct qlncListColMap
{
    stlInt32              mMapCount;        /**< Map Entry Count */
    qlncExprCommon     ** mValueExpr;       /**< Value Expression */
    qlncExprCommon     ** mListColExpr;     /**< List Column Expression */
    qlncExprCommon      * mOrgFunc;
};


/**
 * @brief Expression Map for List Column
 */
struct qlncListColMapList
{
    qlncListColMap        mListColMap;      /**< List Column Map */
    qlncListColMapList  * mNext;
};


/**
 * @brief And Filter
 */
struct qlncAndFilter
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    stlInt32              mOrCount;         /**< Or Filter Count */
    qlncOrFilter       ** mOrFilters;       /**< Or Filter Array */
};


/**
 * @brief Or Filter
 */
struct qlncOrFilter
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    stlInt32              mFilterCount;     /**< Filter Count */
    qlncExprCommon     ** mFilters;         /**< Filter Array */
    qlncRefExprList     * mSubQueryExprList;/**< SubQuery Expression List */
};


/**
 * @brief Constant Boolean Filter
 */
struct qlncConstBooleanFilter
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    qlncExprCommon      * mExpr;            /**< Expression */
    qlncRefExprList     * mSubQueryExprList;/**< SubQuery Expression List */
};


/**
 * @brief Boolean Filter
 */
struct qlncBooleanFilter
{
    qlncExprCommon        mCommon;          /**< Common Expr */
    knlBuiltInFunc        mFuncID;          /**< Functioin ID */
    qlncExprCommon      * mExpr;            /**< Expression */
    qlncRefNodeList       mRefNodeList;     /**< Filter에서 참조하는 Node List */
    qlncRefNodeList     * mLeftRefNodeList; /**< Filter의 Left Expression에서 참조하는 Node List */
    qlncRefNodeList     * mRightRefNodeList;/**< Filter의 Right Expression에서 참조하는 Node List */
    qlncRefExprList     * mSubQueryExprList;/**< SubQuery Expression List */
    stlBool               mPossibleJoinCond;/**< Join Condition이 가능한지 여부 */
    stlBool               mIsPhysicalFilter;/**< Physical Filter 여부 */
    stlChar               mPadding[6];
};


/**
 * @brief Convert Expression Param Info
 */
struct qlncConvertExprParamInfo
{
    qlncParamInfo         mParamInfo;
    qlncTableMapArray   * mTableMapArr;
    qlncTableMapArray   * mOuterTableMapArr;    /**< for Subquery Expression */
    qlvInitExpression   * mInitExpr;
    qlncExprCommon      * mCandExpr;
    qlncRefNodeList     * mRefNodeList;
    stlInt32              mExprPhraseFlag;
    stlChar               mPadding[4];
    qlncRefExprList     * mSubQueryExprList;    /**< SubQuery Expresion들을 관리하는 List */
    qlncExprSubQuery    * mExprSubQuery;        /**< for SubQuery Expression */
    qlncQBMapArray      * mQBMapArr;
};


/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

typedef stlStatus (*qlncConvertExprFuncPtr) ( qlncConvertExprParamInfo  * aConvertExprParamInfo,
                                              qllEnv                    * aEnv );

stlStatus qlncConvertExprValue( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                qllEnv                      * aEnv );

stlStatus qlncConvertExprBindParam( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv );

stlStatus qlncConvertExprColumn( qlncConvertExprParamInfo   * aConvertExprParamInfo,
                                 qllEnv                     * aEnv );

stlStatus qlncConvertExprFunction( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                   qllEnv                   * aEnv );

stlStatus qlncConvertExprTypeCast( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                   qllEnv                   * aEnv );

stlStatus qlncConvertExprPseudoCol( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv );

stlStatus qlncConvertExprPseudoArg( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv );

stlStatus qlncConvertExprConstExpr( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv );

stlStatus qlncConvertExprSubQuery( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                   qllEnv                   * aEnv );

stlStatus qlncConvertExprReference( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv );

stlStatus qlncConvertExprInnerColumn( qlncConvertExprParamInfo  * aConvertExprParamInfo,
                                      qllEnv                    * aEnv );

stlStatus qlncConvertExprOuterColumn( qlncConvertExprParamInfo  * aConvertExprParamInfo,
                                      qllEnv                    * aEnv );

stlStatus qlncConvertExprRowidColumn( qlncConvertExprParamInfo  * aConvertExprParamInfo,
                                      qllEnv                    * aEnv );

stlStatus qlncConvertExprAggregation( qlncConvertExprParamInfo  * aConvertExprParamInfo,
                                      qllEnv                    * aEnv );

stlStatus qlncConvertExprCaseExpr( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                   qllEnv                   * aEnv );

stlStatus qlncConvertExprListFunction( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                       qllEnv                   * aEnv );

stlStatus qlncConvertExprListColumn( qlncConvertExprParamInfo   * aConvertExprParamInfo,
                                     qllEnv                     * aEnv );

stlStatus qlncConvertExprRowExpr( qlncConvertExprParamInfo  * aConvertExprParamInfo,
                                  qllEnv                    * aEnv );

/* filter */
stlStatus qlncConvertAndFilter( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                qllEnv                      * aEnv );

stlStatus qlncConvertOrFilter( qlncConvertExprParamInfo     * aConvertExprParamInfo,
                               qllEnv                       * aEnv );

stlStatus qlncConvertBooleanFilter( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv );

stlStatus qlncConvertCompareBooleanFilter( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                           qllEnv                   * aEnv );

stlStatus qlncIsPhysicalFilter( stlChar         * aSQLString,
                                qlncNodeCommon  * aTableNode,
                                qlncExprCommon  * aExpr,
                                stlBool         * aIsPhysicalFilter,
                                qllEnv          * aEnv );

/* SubQuery Expression 관련 */
stlStatus qlncSetRefColumnInfoOnExprSubQuery( knlRegionMem      * aRegionMem,
                                              qlncExprCommon    * aRefColumn,
                                              qlncExprSubQuery  * aExprSubQuery,
                                              qllEnv            * aEnv );

/* 외부참조 함수들 */
stlStatus qlncConvertCondition( qlncCreateNodeParamInfo * aCreateNodeParamInfo,
                                qlvInitExpression       * aCondition,
                                stlInt32                  aExprPhraseFlag,
                                qlncAndFilter          ** aAndFilter,
                                qllEnv                  * aEnv );

stlStatus qlncConvertExpression( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                 qlvInitExpression          * aCondition,
                                 stlInt32                     aExprPhraseFlag,
                                 qlncRefExprList            * aSubQueryExprList,
                                 qlncExprCommon            ** aExpr,
                                 qllEnv                     * aEnv );

stlStatus qlncAppendOrFilterToAndFilterHead( qlncParamInfo  * aParamInfo,
                                             qlncAndFilter  * aAndFilter,
                                             stlInt32         aOrFilterCount,
                                             qlncOrFilter  ** aOrFilterPtrArr,
                                             qllEnv         * aEnv );

stlStatus qlncAppendOrFilterToAndFilterTail( qlncParamInfo  * aParamInfo,
                                             qlncAndFilter  * aAndFilter,
                                             stlInt32         aOrFilterCount,
                                             qlncOrFilter  ** aOrFilterPtrArr,
                                             qllEnv         * aEnv );

stlStatus qlncAppendFilterToAndFilter( qlncParamInfo    * aParamInfo,
                                       qlncAndFilter    * aAndFilter,
                                       qlncExprCommon   * aFilter,
                                       qllEnv           * aEnv );

stlStatus qlncFindAddRefColumnToRefNodeList( qlncConvertExprParamInfo   * aConvertExprParamInfo,
                                             qlncExprCommon             * aExpr,
                                             qllEnv                     * aEnv );

stlStatus qlncAddRefColumnToRefNodeList( qlncConvertExprParamInfo   * aConvertExprParamInfo,
                                         qlncExprCommon             * aExpr,
                                         qllEnv                     * aEnv );

stlStatus qlncAddColumnToRefColumnList( qlncParamInfo       * aParamInfo,
                                        qlncExprCommon      * aColumn,
                                        qlncRefColumnList   * aRefColumnList,
                                        qllEnv              * aEnv );

stlBool qlncIsExistColumnIdxInRefColumnList( qlncRefColumnList  * aRefColumnList,
                                             stlInt32             aColumnIdx );

stlStatus qlncCompactAndFilter( qlncAndFilter   * aAndFilter,
                                qllEnv          * aEnv );

stlStatus qlncCopyAndFilter( qlncParamInfo  * aParamInfo,
                             qlncAndFilter  * aSrcAndFilter,
                             qlncAndFilter ** aCopiedAndFilter,
                             qllEnv         * aEnv );

stlBool qlncIsPossibleJoinCondition( knlBuiltInFunc aFuncID );

stlStatus qlncWrapExprInInnerColumn( knlRegionMem       * aRegionMem,
                                     qlncExprCommon     * aExpr,
                                     qlncNodeCommon     * aNode,
                                     stlInt32             aIdx,
                                     qlncExprCommon    ** aWrappedExpr,
                                     qllEnv             * aEnv );

stlBool qlncIsRowidORFilter( qlncBaseTableNode  * aBaseTableNode,
                             qlncOrFilter       * aOrFilter );

stlStatus qlncAddExprListToRefExprList( knlRegionMem     * aRegionMem,
                                        qlncRefExprList  * aDestExprList,
                                        qlncRefExprList  * aSrcExprList,
                                        qllEnv           * aEnv );

stlBool qlncIsSameExpr( qlncExprCommon  * aExpr1,
                        qlncExprCommon  * aExpr2 );

stlStatus qlncDuplicateExpr( qlncParamInfo      * aParamInfo,
                             qlncExprCommon     * aSrcExpr,
                             qlncExprCommon    ** aDstExpr,
                             qllEnv             * aEnv );

stlBool qlncIsExistSubQueryFilter( qlncAndFilter    * aFilter );


stlStatus qlncGetInKeyRangeExprList( qlncCBOptParamInfo  * aCBOptParamInfo,
                                     qlncIndexScanInfo   * aIndexScanInfo,
                                     qlncAndFilter       * aAndFilter,
                                     qlncRefExprList     * aInKeyRangeExprList,
                                     qlncRefExprList     * aValueExprList,
                                     qllEnv              * aEnv );

stlStatus qlncGetInKeyRangeAndFilter( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qlncIndexScanInfo     * aIndexScanInfo,
                                      qlncAndFilter         * aAndFilter,
                                      qlncRefExprList       * aAndFilterList,
                                      qlncListColMapList   ** aListColMapList,
                                      qllEnv                * aEnv );

stlStatus qlncMakeFilter( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                          qlncRefNodeList             * aRefNodeList,
                          knlBuiltInFunc                aFuncID,
                          qlncRefExprList             * aSubQueryExprList,
                          stlChar                     * aColumnName,
                          qlncExprCommon             ** aResultFilter,
                          qllEnv                      * aEnv );

/** @} qlnc */

#endif /* _QLNCEXPR_H_ */
