/*******************************************************************************
 * ellNodeDef.h
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

#ifndef _ELLNODEDEF_H_
#define _ELLNODEDEF_H_ 1

/**
 * @file ellNodeDef.h
 * @brief Execution Layer 
 */


/**
 * @ingroup ellNodeDef
 * @{
 */

/*
 * 아래 자료구조는 ellDef.h로 우선 옮김.
 * 
 * typedef enum ellNodeType
 * {
 *   ELL_NODE_TYPE_COLUMN    = 0,
 *   ELL_NODE_TYPE_CONSTANT,
 *
 *   // FUNCTION / EXPRESTION / ...
 *   ELL_NODE_TYPE_EXPR,    
 *   ELL_NODE_TYPE_FUNCTION_EXPR,
 *   ELL_NODE_TYPE_COMPARISON_EXPR,
 *   ELL_NODE_TYPE_BOOL_EXPR,
 *   ELL_NODE_TYPE_SIMPLE_CASE_EXPR,
 *   ELL_NODE_TYPE_SEARCHED_CASE_EXPR,
 *   ELL_NODE_TYPE_COALESCE_EXPR,
 *   ELL_NODE_TYPE_GREATEST_LEAST_EXPR,
 *   ELL_NODE_TYPE_NULLIF_EXPR,
 *   ELL_NODE_TYPE_NVL_EXPR
 *
 * } ellNodeType;
 * 
 *
 *
 * typedef struct ellNode
 * {
 *   ellNodeType    mType;
 * } ellNode;
 */

typedef struct ellNodeListCell
{
	union
	{
		void      * mPtrValue;
		stlInt64    mIntValue;
            /* oid oid_value; */
	} mData;
    
	struct ellNodeListCell   * mNext;
} ellNodeListCell;


typedef struct ellNodeList
{
	ellNodeType        mType;
	stlInt32           mCount;
	ellNodeListCell  * mHead;
	ellNodeListCell  * mTail;
} ellNodeList;


typedef struct ellNodeExpr
{
    ellNodeType     mType;
} ellNodeExpr;


typedef struct ellNodeColumn
{
    ellNodeExpr     mExpr;
    stlInt32        mColOrder;
    dtlDataType     mColType;
    stlInt32        mTokenLocation;
} ellNodeColumn;

typedef struct ellNodeConstant
{
    ellNodeExpr     mType;
    dtlDataValue  * mValue;
    stlInt32        mTokenLocation;
} ellNodeConstant;

/**
 * 예제) 1 + 2
 *       => function name : "+"
 *          arguments : 1, 2
 *          result : dtlDataValue
 */
typedef struct ellNodeFunctionExpr
{
    ellNodeExpr     mExpr;
    stlInt32        mFunction;    /* @todo ell에서 function 정의하기 => stlInt32 대체 */
    ellNodeList     mArgs;
    dtlDataValue    mReturnType;
    stlInt32        mTokenLocation;
} ellNodeFuncExpr;

/**
 * 예제) i1 < i2
 *       => comparison operator name : "<"
 *          arguments : i1, i2
 *          result : dtlTruthValue
 */
typedef struct ellNodeComparisonExpr
{
    ellNodeExpr     mExpr;
    stlInt32        mCompOperator;    /* @todo ell에서 operation 정의하기 => stlInt32 대체 */
    ellNodeList     mArgs;
    stlInt32        mTokenLocation;
} ellNodeComparisonExpr;

/**
 * 예제) FALSE AND TRUE
 *       => boolean operator name : "AND"
 *          arguments : FALSE, TRUE
 *          result : dtlTruthValue
 */
typedef struct ellNodeBoolExpr
{
    ellNodeExpr     mExpr;
    stlInt32        mBoolOperator;    /* @todo ell에서 boolean operation 정의하기 => stlInt32 대체 */
    ellNodeList     mArgs;
    stlInt32        mTokenLocation;    
} ellNodeBoolExpr;

typedef struct ellSimpleCaseExpr
{
    /**
     * @todo 임시로 ... 자료구조 확정해야 함.
     */
    ellNodeType    mType;
    
    ellNodeList  * mWhenClause;
    
} ellSimpleCaseExpr;

typedef struct ellSearchedCaseExpr
{
    /**
     * @todo 임시로 ... 자료구조 확정해야 함.
     */
    ellNodeType     mType;
    
} ellSearchedCaseExpr;

typedef struct ellCoalesceExpr
{
    /**
     * @todo 임시로 ... 자료구조 확정해야 함.
     */
    ellNodeType    mType;

    ellNodeList  * mValueList;
    
} ellCoalesceExpr;

typedef struct ellNullIfExpr
{
    /**
     * @todo 임시로 ... 자료구조 확정해야 함.
     */
    ellNodeType     mType;
    
} ellNullIfExpr;

typedef struct ellGreatestLeastExpr
{
    /**
     * @todo 임시로 ... 자료구조 확정해야 함.
     */
    ellNodeType     mType;
    
} ellGreatestLeastExpr;

typedef struct ellNvlExpr
{
    /**
     * @todo 임시로 ... 자료구조 확정해야 함.
     */
    ellNodeType     mType;
    
} ellNvlExpr;

/** @} ellNodeDef */


#endif /* _ELLNODEDEF_H_ */
