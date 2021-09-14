/*******************************************************************************
 * ellFunction.c
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
 * @file ellFunction.c
 * @brief Execution Library Layer Function Routines
 */


#include <ell.h>

/**
 * @ingroup ellFunction
 * @{
 */

/*
 * @brief builtin function들에 대한 정보를 유지하는 배열 
 */

dtlBuiltInFuncInfo gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_MAX ] =
{
    /**
     * {
     *     ID,
     *     Name,
     *     InputArgumentCountMin,
     *     InputArgumentCountMax,
     *     ReturnTypeClass,
     *     IterationTime,
     *     IsNeedCastInGroup,
     *     IsPhysicalFilter
     *     ReturnNullable
     *     ExceptionType,
     *     ExceptionReturnValue,
     *     mIsExprCompositionComplex,
     *     GetFuncDetailInfo function,
     *     GetFuncPointer function
     * }
     */

    /**
     * empty : always return false
     */
    
    {
        KNL_BUILTIN_FUNC_INVALID,
        "EMPTY",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_INVALID,
        DTL_ITERATION_TIME_INVALID,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_INVALID,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },
    
    
    /**
     * assign operations
     */
    
    {
        KNL_BUILTIN_FUNC_ASSIGN,
        "ASSIGN",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_INVALID,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        knlGetFuncInfoAssign,
        knlGetFuncPtrAssign
    },

    
    /**
     * unconditional jump operations
     */
    {
        KNL_BUILTIN_FUNC_RETURN_EMPTY,
        "RETURN EMPTY",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_INVALID,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_INVALID,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_RETURN_NULL,
        "RETURN NULL",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_INVALID,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_INVALID,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_NULL,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_RETURN_PASS,
        "RETURN PASS",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_INVALID,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_INVALID,
        DTL_ACTION_TYPE_UNCONDITION_JUMP,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },
   
    
    /**
     * comparision operations
     */
    
    {
        KNL_BUILTIN_FUNC_IS_EQUAL,
        "= (IS EQUAL TO)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoIsEqual,
        dtlGetFuncPtrIsEqual
    },
    
    {
        KNL_BUILTIN_FUNC_IS_NOT_EQUAL,
        "<> (IS NOT EQUAL TO)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoIsNotEqual,
        dtlGetFuncPtrIsNotEqual
    },
    
    {
        KNL_BUILTIN_FUNC_IS_LESS_THAN,
        "< (IS LESS THAN)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoIsLessThan,
        dtlGetFuncPtrIsLessThan
    },
    
    {
        KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL,
        "<= (IS LESS THAN OR EQUAL TO)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoIsLessThanEqual,
        dtlGetFuncPtrIsLessThanEqual
    },

    {
        KNL_BUILTIN_FUNC_IS_GREATER_THAN,
        "> (IS GREATER THAN)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoIsGreaterThan,
        dtlGetFuncPtrIsGreaterThan
    },
    
    {
        KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL,
        ">= (IS GREATER THAN OR EQUAL TO)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoIsGreaterThanEqual,
        dtlGetFuncPtrIsGreaterThanEqual
    },

    {
        KNL_BUILTIN_FUNC_IS_NULL,
        "IS NULL",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoIsNull,
        dtlGetFuncPtrIsNull
    },
    
    {
        KNL_BUILTIN_FUNC_IS_NOT_NULL,
        "IS NOT NULL",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoIsNotNull,
        dtlGetFuncPtrIsNotNull
    },

    /**
     * logical operations
     */
    {
        KNL_BUILTIN_FUNC_AND,
        "AND",
        2,
        1024,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_FALSE,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoAnd,
        dtlGetFuncPtrAnd
    },

    {
        KNL_BUILTIN_FUNC_OR,
        "OR",
        2,
        1024,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_TRUE,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoOr,
        dtlGetFuncPtrOr
    },

    {
        KNL_BUILTIN_FUNC_NOT,
        "NOT",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoNot,
        dtlGetFuncPtrNot
    },

    {
        KNL_BUILTIN_FUNC_IS,
        "IS",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_FALSE,
        STL_TRUE,
        dtlGetFuncInfoIs,
        dtlGetFuncPtrIs
    },

    {
        KNL_BUILTIN_FUNC_IS_NOT,
        "IS NOT",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_TRUE,
        STL_TRUE,
        dtlGetFuncInfoIsNot,
        dtlGetFuncPtrIsNot
    },

    {
        KNL_BUILTIN_FUNC_IS_UNKNOWN,
        "IS UNKNOWN",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoIsUnknown,
        dtlGetFuncPtrIsUnknown
    },

    {
        KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN,
        "IS NOT UNKNOWN",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoIsNotUnknown,
        dtlGetFuncPtrIsNotUnknown
    },
    
    /**
     * between
     */
    
    /*
     * BETWEEN 은  >= AND <= 로 변경되어 처리된다.
     * NOT BETWEEN 은 > OR < 로 변경되어 처리된다.
     * 그래서, getfunctioninfo와 getfuncptr은 연결하지 않았음.
     */
    
    {
        KNL_BUILTIN_FUNC_BETWEEN,
        "BETWEEN",
        3,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_TRUE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },
    
    {
        KNL_BUILTIN_FUNC_NOT_BETWEEN,
        "NOT BETWEEN",
        3,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_TRUE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_BETWEEN_SYMMETRIC,
        "BETWEEN SYMMETRIC",
        3,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_TRUE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoBetweenSymmetric,
        dtlGetFuncPtrBetweenSymmetric
    },

    {
        KNL_BUILTIN_FUNC_NOT_BETWEEN_SYMMETRIC,
        "NOT BETWEEN SYMMETRIC",
        3,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_TRUE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoBetweenSymmetric,
        dtlGetFuncPtrNotBetweenSymmetric
    },

    /**
     * like
     */
    {
        KNL_BUILTIN_FUNC_LIKE,
        "LIKE",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_TRUE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoLike,
        dtlGetFuncPtrLike
    },
    {
        KNL_BUILTIN_FUNC_NOT_LIKE,
        "NOT LIKE",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_TRUE,
        STL_TRUE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoLike,
        dtlGetFuncPtrNotLike
    },

    /**
     * like pattern
     */
    {
        KNL_BUILTIN_FUNC_LIKE_PATTERN,
        "LIKE PATTERN",
        1,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoLikePattern,
        dtlGetFuncPtrLikePattern
    },

    /**
     * cast
     */
    
    {
        KNL_BUILTIN_FUNC_CAST,
        "CAST",
        DTL_CAST_INPUT_ARG_CNT,
        DTL_CAST_INPUT_ARG_CNT,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoCast,
        dtlGetFuncPtrCast
    },
    
    /**
     * Mathematical ( with Arithmetic )
     */
    
    {
        KNL_BUILTIN_FUNC_ADD,
        "+ (ADDITION)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoAdd,
        dtlGetFuncPtrAdd
    },

    {
        KNL_BUILTIN_FUNC_SUB,
        "- (SUBTRACTION)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoSub,
        dtlGetFuncPtrSub
    },

    {
        KNL_BUILTIN_FUNC_MUL,
        "* (MULTIPLICATION)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoMul,
        dtlGetFuncPtrMul
    },

    {
        KNL_BUILTIN_FUNC_DIV,
        "/ (DIVISION)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoDiv,
        dtlGetFuncPtrDiv
    },

    {
        KNL_BUILTIN_FUNC_MOD,
        "MOD",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoMod,
        dtlGetFuncPtrMod
    },

    {
        KNL_BUILTIN_FUNC_POSITIVE,
        "+ (POSITIVE)",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoPositive,
        dtlGetFuncPtrPositive
    },

    {
        KNL_BUILTIN_FUNC_NEGATIVE,
        "- (NEGATIVE)",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoNegative,
        dtlGetFuncPtrNegative
    },

    {
        KNL_BUILTIN_FUNC_POWER,
        "POWER",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoPower,
        dtlGetFuncPtrPower
    },

    {
        KNL_BUILTIN_FUNC_SQRT,
        "SQRT",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoSqrt,
        dtlGetFuncPtrSqrt
    },

    {
        KNL_BUILTIN_FUNC_CBRT,
        "CBRT",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoCbrt,
        dtlGetFuncPtrCbrt
    },

    {
        KNL_BUILTIN_FUNC_FACTORIAL,
        "FACTORIAL",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoFactorial,
        dtlGetFuncPtrFactorial
    },

    {
        KNL_BUILTIN_FUNC_ABS,
        "ABS",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoAbs,
        dtlGetFuncPtrAbs
    },

    {
        KNL_BUILTIN_FUNC_CEIL,
        "CEIL",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoCeil,
        dtlGetFuncPtrCeil
    },

    {
        KNL_BUILTIN_FUNC_DEGREES,
        "DEGREES",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoDegrees,
        dtlGetFuncPtrDegrees
    },

    {
        KNL_BUILTIN_FUNC_EXP,
        "EXP",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoExp,
        dtlGetFuncPtrExp
    },

    {
        KNL_BUILTIN_FUNC_FLOOR,
        "FLOOR",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoFloor,
        dtlGetFuncPtrFloor
    },

    {
        KNL_BUILTIN_FUNC_LN,
        "LN",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoLn,
        dtlGetFuncPtrLn
    },

    {
        KNL_BUILTIN_FUNC_LOG10,
        "LOG10",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoLog10,
        dtlGetFuncPtrLog10
    },

    {
        KNL_BUILTIN_FUNC_LOG,
        "LOG",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoLog,
        dtlGetFuncPtrLog
    },

    {
        KNL_BUILTIN_FUNC_PI,
        "PI",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,        
        STL_FALSE,
        dtlGetFuncInfoPi,
        dtlGetFuncPtrPi
    },

    {
        KNL_BUILTIN_FUNC_RADIANS,
        "RADIANS",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoRadians,
        dtlGetFuncPtrRadians
    },

    {
        KNL_BUILTIN_FUNC_RANDOM,
        "RANDOM",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_EXPR,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoRandom,
        dtlGetFuncPtrRandom
    },

    {
        KNL_BUILTIN_FUNC_ROUND,
        "ROUND",
        1,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoRound,
        dtlGetFuncPtrRound
    },

    {
        KNL_BUILTIN_FUNC_SIGN,
        "SIGN",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoSign,
        dtlGetFuncPtrSign
    },

    {
        KNL_BUILTIN_FUNC_TRUNC,
        "TRUNC",
        1,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoTrunc,
        dtlGetFuncPtrTrunc
    },

    {
        KNL_BUILTIN_FUNC_WIDTHBUCKET,
        "WIDTH_BUCKET",
        4,
        4,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoWidthBucket,
        dtlGetFuncPtrWidthBucket
    },

    {
        KNL_BUILTIN_FUNC_ACOS,
        "ACOS",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoAcos,
        dtlGetFuncPtrAcos
    },

    {
        KNL_BUILTIN_FUNC_ASIN,
        "ASIN",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoAsin,
        dtlGetFuncPtrAsin
    },

    {
        KNL_BUILTIN_FUNC_ATAN,
        "ATAN",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoAtan,
        dtlGetFuncPtrAtan
    },

    {
        KNL_BUILTIN_FUNC_ATAN2,
        "ATAN2",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoAtan2,
        dtlGetFuncPtrAtan2
    },

    {
        KNL_BUILTIN_FUNC_COS,
        "COS",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoCos,
        dtlGetFuncPtrCos
    },

    {
        KNL_BUILTIN_FUNC_COT,
        "COT",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoCot,
        dtlGetFuncPtrCot
    },

    {
        KNL_BUILTIN_FUNC_SIN,
        "SIN",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoSin,
        dtlGetFuncPtrSin
    },

    {
        KNL_BUILTIN_FUNC_TAN,
        "TAN",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoTan,
        dtlGetFuncPtrTan
    },


    /**
     * Bitwise
     */
    
    {
        KNL_BUILTIN_FUNC_BITWISE_AND,
        "BITAND",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoBitwiseAnd,
        dtlGetFuncPtrBitwiseAnd
    },
    
    {
        KNL_BUILTIN_FUNC_BITWISE_OR,
        "BITOR",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoBitwiseOr,
        dtlGetFuncPtrBitwiseOr
    },
    
    {
        KNL_BUILTIN_FUNC_BITWISE_XOR,
        "BITXOR",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoBitwiseXor,
        dtlGetFuncPtrBitwiseXor
    },
    
    {
        KNL_BUILTIN_FUNC_BITWISE_NOT,
        "BITNOT",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoBitwiseNot,
        dtlGetFuncPtrBitwiseNot
    },
    
    {
        KNL_BUILTIN_FUNC_BITWISE_SHIFTLEFT,
        "SHIFT_LEFT",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoShiftLeft,
        dtlGetFuncPtrShiftLeft
    },
    
    {
        KNL_BUILTIN_FUNC_BITWISE_SHIFTRIGHT,
        "SHIFT_RIGHT",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoShiftRight,
        dtlGetFuncPtrShiftRight
    },

    /**
     * String
     */
    
    {
        KNL_BUILTIN_FUNC_CONCATENATE,
        "|| (CONCATENATE)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ALL_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoConcatenate,
        dtlGetFuncPtrConcatenate
    },

    {
        KNL_BUILTIN_FUNC_BITLENGTH,
        "BIT_LENGTH",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoBitLength,
        dtlGetFuncPtrBitLength
    },

    {
        KNL_BUILTIN_FUNC_CHARLENGTH,
        "CHAR_LENGTH",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoCharLength,
        dtlGetFuncPtrCharLength
    },

    {
        KNL_BUILTIN_FUNC_LOWER,
        "LOWER",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoLower,
        dtlGetFuncPtrLower
    },

    {
        KNL_BUILTIN_FUNC_OCTETLENGTH,
        "OCTET_LENGTH",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoOctetLength,
        dtlGetFuncPtrOctetLength
    },

    {
        KNL_BUILTIN_FUNC_OVERLAY,
        "OVERLAY",
        3,
        5,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoOverlay,
        dtlGetFuncPtrOverlay
    },

    {
        KNL_BUILTIN_FUNC_POSITION,
        "POSITION",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoPosition,
        dtlGetFuncPtrPosition
    },

    {
        KNL_BUILTIN_FUNC_INSTR,
        "INSTR",
        2,
        4,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoInstr,
        dtlGetFuncPtrInstr
    },
    
    {
        KNL_BUILTIN_FUNC_SUBSTRING,
        "SUBSTRING",
        2,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_ALWAYS,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,        
        STL_FALSE,
        dtlGetFuncInfoSubstring,
        dtlGetFuncPtrSubstring
    },

    {
        KNL_BUILTIN_FUNC_SUBSTRB,
        "SUBSTRB",
        2,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_ALWAYS,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoSubstrb,
        dtlGetFuncPtrSubstrb
    },
    
    {
        KNL_BUILTIN_FUNC_TRIM,
        "TRIM",
        2,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_ALWAYS,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoTrim,
        dtlGetFuncPtrTrim
    },

    {
        KNL_BUILTIN_FUNC_LTRIM,
        "LTRIM",
        1,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_ALWAYS,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoLtrim,
        dtlGetFuncPtrLtrim
    },

    {
        KNL_BUILTIN_FUNC_RTRIM,
        "RTRIM",
        1,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_ALWAYS,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoRtrim,
        dtlGetFuncPtrRtrim
    },
    
    {
        KNL_BUILTIN_FUNC_UPPER,
        "UPPER",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoUpper,
        dtlGetFuncPtrUpper
    },

    {
        KNL_BUILTIN_FUNC_INITCAP,
        "INITCAP",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoInitcap,
        dtlGetFuncPtrInitcap
    },

    {
        KNL_BUILTIN_FUNC_LPAD,
        "LPAD",
        2,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_CHECK,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,        
        STL_FALSE,
        dtlGetFuncInfoLpad,
        dtlGetFuncPtrLpad
    },

    {
        KNL_BUILTIN_FUNC_RPAD,
        "RPAD",
        2,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_CHECK,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,        
        STL_FALSE,
        dtlGetFuncInfoRpad,
        dtlGetFuncPtrRpad
    },
    
    {
        KNL_BUILTIN_FUNC_REPEAT,
        "REPEAT",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_ALWAYS,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoRepeat,
        dtlGetFuncPtrRepeat
    },

    {
        KNL_BUILTIN_FUNC_REPLACE,
        "REPLACE",
        2,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_ALWAYS,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoReplace,
        dtlGetFuncPtrReplace
    },

    {
        KNL_BUILTIN_FUNC_SPLIT_PART,
        "SPLIT_PART",
        3,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_ALWAYS,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoSplitPart,
        dtlGetFuncPtrSplitPart
    },
    
    {
        KNL_BUILTIN_FUNC_TRANSLATE,
        "TRANSLATE",
        3,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoTranslate,
        dtlGetFuncPtrTranslate
    },

    {
        KNL_BUILTIN_FUNC_CHR,
        "CHR",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoChr,
        dtlGetFuncPtrChr
    },
    
    /**
     * Date & Time
     */

    {
        KNL_BUILTIN_FUNC_ADDDATE,
        "ADDDATE",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoAddDate,
        dtlGetFuncPtrAddDate
    },    
    {
        KNL_BUILTIN_FUNC_ADDTIME,
        "ADDTIME",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoAddTime,
        dtlGetFuncPtrAddTime
    },
    {
        KNL_BUILTIN_FUNC_DATEADD,
        "DATEADD",
        3,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoDateAdd,
        dtlGetFuncPtrDateAdd
    },
    {
        KNL_BUILTIN_FUNC_DATE_ADD,
        "DATE_ADD",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoAddDate,
        dtlGetFuncPtrAddDate
    },
    {
        KNL_BUILTIN_FUNC_EXTRACT,
        "EXTRACT",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoExtract,
        dtlGetFuncPtrExtract
    },
    {
        KNL_BUILTIN_FUNC_DATE_PART,
        "DATE_PART",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoExtract,
        dtlGetFuncPtrExtract
    },
    
    {
        KNL_BUILTIN_FUNC_LAST_DAY,
        "LAST_DAY",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoLastDay,
        dtlGetFuncPtrLastDay
    },   
    
    {
        KNL_BUILTIN_FUNC_ADD_MONTHS,
        "ADD_MONTHS",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoAddMonths,
        dtlGetFuncPtrAddMonths
    },   

    {
        KNL_BUILTIN_FUNC_FROM_UNIXTIME,
        "FROM_UNIXTIME",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoFromUnixTime,
        dtlGetFuncPtrFromUnixTime
    },

    {
        KNL_BUILTIN_FUNC_DATEDIFF,
        "DATEDIFF",
        3,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoDateDiff,
        dtlGetFuncPtrDateDiff
    },    
    
    
    /**
     * ROWID
     */   

    {
        KNL_BUILTIN_FUNC_ROWID_OBJECT_ID,
        "ROWID_OBEJCT_ID",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoRowIdObjectId,
        dtlGetFuncPtrRowIdObjectId
    },

    {
        KNL_BUILTIN_FUNC_ROWID_PAGE_ID,
        "ROWID_PAGE_ID",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoRowIdPageId,
        dtlGetFuncPtrRowIdPageId
    },

    {
        KNL_BUILTIN_FUNC_ROWID_ROW_NUMBER,
        "ROWID_ROW_NUMBER",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoRowIdRowNumber,
        dtlGetFuncPtrRowIdRowNumber
    },

    {
        KNL_BUILTIN_FUNC_ROWID_TABLESPACE_ID,
        "ROWID_TABLESPACE_ID",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_TRUE,
        dtlGetFuncInfoRowIdTablespaceId,
        dtlGetFuncPtrRowIdTablespaceId
    },

    /**
     * CASE EXPRESSION
     */   
    
    {
        KNL_BUILTIN_FUNC_CASE,
        "CASE",
        1,    
        1024,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_CHECK,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_TRUE,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_NULLIF,
        "NULLIF",
        2,    
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_CHECK,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_TRUE,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_COALESCE,
        "COALESCE",
        2,    
        1024,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_CHECK,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_TRUE,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_DECODE,
        "DECODE",
        3,    
        255,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_CHECK,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_TRUE,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_CASE2,
        "CASE2",
        2,    
        1024,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_CHECK,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_TRUE,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },    

    
    /**
     * NVL
     */   

    {
        KNL_BUILTIN_FUNC_NVL,
        "NVL",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_CHECK,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_TRUE,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_NVL2,
        "NVL2",
        3,
        3,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_CHECK,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_TRUE,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    /**
     * TO_CHAR
     */

    {
        KNL_BUILTIN_FUNC_TO_CHAR,
        "TO_CHAR",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToChar,
        dtlGetFuncPtrToChar
    },

    /**
     * TO_CHAR_FORMAT
     */

    {
        KNL_BUILTIN_FUNC_TO_CHAR_FORMAT,
        "TO_CHAR_FORMAT",
        0,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToCharFormat,
        dtlGetFuncPtrToCharFormat
    },

    /**
     * TO_DATETIME
     */

    {
        KNL_BUILTIN_FUNC_TO_DATE,
        "TO_DATE",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToDate,
        dtlGetFuncPtrToDate
    },

    {
        KNL_BUILTIN_FUNC_TO_TIME,
        "TO_TIME",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToTime,
        dtlGetFuncPtrToTime
    },

    {
        KNL_BUILTIN_FUNC_TO_TIME_WITH_TIME_ZONE,
        "TO_TIME_WITH_TIME_ZONE",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToTimeWithTimeZone,
        dtlGetFuncPtrToTimeWithTimeZone
    },

    {
        KNL_BUILTIN_FUNC_TO_TIMESTAMP,
        "TO_TIMESTAMP",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToTimestamp,
        dtlGetFuncPtrToTimestamp
    },

    {
        KNL_BUILTIN_FUNC_TO_TIMESTAMP_WITH_TIME_ZONE,
        "TO_TIMESTAMP_WITH_TIME_ZONE",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToTimestampWithTimeZone,
        dtlGetFuncPtrToTimestampWithTimeZone
    },
    
    /**
     * TO_DATETIME_FORMAT
     */

    {
        KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT,
        "TO_DATETIME_FORMAT",
        1,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToDateTimeFormat,
        dtlGetFuncPtrToDateTimeFormat
    },

    /**
     * TO_NUMBER
     */

    {
        KNL_BUILTIN_FUNC_TO_NUMBER,
        "TO_NUMBER",
        1,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToNumber,
        dtlGetFuncPtrToNumber
    },

    /**
     * TO_NUMBER_FORMAT
     */
    
    {
        KNL_BUILTIN_FUNC_TO_NUMBER_FORMAT,
        "TO_NUMBER_FORMAT",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToNumberFormat,
        dtlGetFuncPtrToNumberFormat
    },

    /**
     * TO_NATIVE_REAL
     */

    {
        KNL_BUILTIN_FUNC_TO_NATIVE_REAL,
        "TO_NATIVE_REAL",
        1,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToNativeReal,
        dtlGetFuncPtrToNativeReal
    },

    /**
     * TO_NATIVE_DOUBLE
     */

    {
        KNL_BUILTIN_FUNC_TO_NATIVE_DOUBLE,
        "TO_NATIVE_DOUBLE",
        1,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoToNativeDouble,
        dtlGetFuncPtrToNativeDouble
    },

    /**
     * general_comparision_function
     */

    /**
     * GREATEST
     */

    {
        KNL_BUILTIN_FUNC_GREATEST,
        "GREATEST",
        DTL_GENERAL_COMPARISON_FUN_INPUT_MIN_ARG_CNT,  /* 1 */
        DTL_GENERAL_COMPARISON_FUN_INPUT_MAX_ARG_CNT,  /* 1024 */
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoGreatest,
        dtlGetFuncPtrGreatest
    },

    /**
     * LEAST
     */

    {
        KNL_BUILTIN_FUNC_LEAST,
        "LEAST",
        DTL_GENERAL_COMPARISON_FUN_INPUT_MIN_ARG_CNT,  /* 1 */
        DTL_GENERAL_COMPARISON_FUN_INPUT_MAX_ARG_CNT,  /* 1024 */ 
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoLeast,
        dtlGetFuncPtrLeast
    },
    
        
    
    /*************************************************************
     * LIST FUNCTION
     *************************************************************/

    /**
     * LIST OPERATION
     */

    {
        KNL_BUILTIN_FUNC_IS_EQUAL_ROW,
        "= (IS EQUAL TO ROW)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_IS_NOT_EQUAL_ROW,
        "<> (IS NOT EQUAL TO ROW)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },
    
    {
        KNL_BUILTIN_FUNC_IS_LESS_THAN_ROW,
        "< (IS LESS THAN TO ROW)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL_ROW,
        "<= (IS LESS THAN EQUAL TO ROW)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },
    
    {
        KNL_BUILTIN_FUNC_IS_GREATER_THAN_ROW,
        "> (IS GREATER THAN TO ROW)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW,
        ">= (IS GREATER THAN EQUAL TO ROW)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    /**
     * IN
     */
    {
        KNL_BUILTIN_FUNC_IN,
        "IN",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_NOT_IN,
        "NOT IN",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },
    

    /**
     * EXISTS
     */
    {
        KNL_BUILTIN_FUNC_EXISTS,
        "EXISTS",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
     },

    {
        KNL_BUILTIN_FUNC_NOT_EXISTS,
        "NOT EXISTS",
        1,
        1,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    
    /**
     * LIST OPERATION ( ANY )
     */

    {
        KNL_BUILTIN_FUNC_EQUAL_ANY,
        "= ANY (EQUAL ANY)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_NOT_EQUAL_ANY,
        "<> ANY (NOT EQUAL ANY)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

     {
        KNL_BUILTIN_FUNC_LESS_THAN_ANY,
        "< ANY (LESS THAN ANY)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY,
        "<= ANY (LESS THAN OR EQUAL ANY)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_GREATER_THAN_ANY,
        "> ANY (GREATER THAN ANY)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY,
        ">= ANY (GREATER THAN OR EQUAL ANY)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },
    /**
     * LIST OPERATION ( ALL )
     */

    {
        KNL_BUILTIN_FUNC_EQUAL_ALL,
        "= ALL (EQUAL ALL)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_NOT_EQUAL_ALL,
        "<> ALL (NOT EQUAL ALL)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

     {
        KNL_BUILTIN_FUNC_LESS_THAN_ALL,
        "< ALL (LESS THAN ALL)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL,
        "<= ALL (LESS THAN OR EQUAL ALL)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_GREATER_THAN_ALL,
        "> ALL (GREATER THAN ALL)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    {
        KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL,
        ">= ALL (GREATER THAN OR EQUAL ALL)",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_EMPTY,
        STL_TRUE,
        dtlGetFuncInfoInvalid,
        dtlGetFuncPtrInvalid
    },

    /*************************************************************
     * DUMP
     *************************************************************/
     {
        KNL_BUILTIN_FUNC_DUMP,
        "DUMP",
        2,
        2,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_NONE,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT,
        DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL,
        DTL_ACTION_RETURN_NULL,
        STL_FALSE,
        dtlGetFuncInfoDump,
        dtlGetFuncPtrDump
     },

    
    /*************************************************************
     * SYSTEM
     *************************************************************/
    
    {
        KNL_BUILTIN_FUNC_CURRENT_CATALOG,
        "CURRENT_CATALOG",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoCatalogName,
        ellGetFuncPtrCatalogName
    },

    {
        KNL_BUILTIN_FUNC_VERSION,
        "VERSION",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoVersion,
        ellGetFuncPtrVersion
    },

    
    /*************************************************************
     * SESSION
     *************************************************************/
    
    {
        KNL_BUILTIN_FUNC_SESSION_ID,
        "SESSION_ID",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoSessionID,
        ellGetFuncPtrSessionID
    },

    {
        KNL_BUILTIN_FUNC_SESSION_SERIAL,
        "SESSION_SERIAL",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoSessionSerial,
        ellGetFuncPtrSessionSerial
    },

    {
        KNL_BUILTIN_FUNC_USER_ID,
        "USER_ID",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoUserID,
        ellGetFuncPtrUserID
    },

    {
        KNL_BUILTIN_FUNC_CURRENT_USER,
        "CURRENT_USER",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoCurrentUser,
        ellGetFuncPtrCurrentUser
    },

    {
        KNL_BUILTIN_FUNC_SESSION_USER,
        "SESSION_USER",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoSessionUser,
        ellGetFuncPtrSessionUser
    },

    {
        KNL_BUILTIN_FUNC_LOGON_USER,
        "LOGON_USER",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoLogonUser,
        ellGetFuncPtrLogonUser
    },
    
    {
        KNL_BUILTIN_FUNC_CURRENT_SCHEMA,
        "CURRENT_SCHEMA",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,        
        STL_FALSE,
        ellGetFuncInfoCurrentSchema,
        ellGetFuncPtrCurrentSchema
    },
    
    
    /*************************************************************
     * Date & Time for Session
     *************************************************************/

    {
        KNL_BUILTIN_FUNC_CLOCK_DATE,
        "CLOCK_DATE",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_EXPR,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoClockDate,
        ellGetFuncPtrClockDate
    },

    {
        KNL_BUILTIN_FUNC_STATEMENT_DATE,
        "STATEMENT_DATE",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoStatementDate,
        ellGetFuncPtrStatementDate
    },

    {
        KNL_BUILTIN_FUNC_TRANSACTION_DATE,
        "TRANSACTION_DATE",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,        
        STL_FALSE,
        ellGetFuncInfoTransactionDate,
        ellGetFuncPtrTransactionDate
    },

    {
        KNL_BUILTIN_FUNC_CLOCK_TIME,
        "CLOCK_TIME",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_EXPR,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoClockTime,
        ellGetFuncPtrClockTime
    },

    {
        KNL_BUILTIN_FUNC_STATEMENT_TIME,
        "STATEMENT_TIME",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoStatementTime,
        ellGetFuncPtrStatementTime
    },

    {
        KNL_BUILTIN_FUNC_TRANSACTION_TIME,
        "TRANSACTION_TIME",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,        
        STL_FALSE,
        ellGetFuncInfoTransactionTime,
        ellGetFuncPtrTransactionTime
    },
    
    {
        KNL_BUILTIN_FUNC_CLOCK_TIMESTAMP,
        "CLOCK_TIMESTAMP",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_EXPR,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoClockTimestamp,
        ellGetFuncPtrClockTimestamp
    },

    {
        KNL_BUILTIN_FUNC_STATEMENT_TIMESTAMP,
        "STATEMENT_TIMESTAMP",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoStatementTimestamp,
        ellGetFuncPtrStatementTimestamp
    },

    {
        KNL_BUILTIN_FUNC_TRANSACTION_TIMESTAMP,
        "TRANSACTION_TIMESTAMP",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,        
        STL_FALSE,
        ellGetFuncInfoTransactionTimestamp,
        ellGetFuncPtrTransactionTimestamp
    },

    {
        KNL_BUILTIN_FUNC_CLOCK_LOCALTIME,
        "CLOCK_LOCALTIME",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_EXPR,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoClockLocalTime,
        ellGetFuncPtrClockLocalTime
    },

    {
        KNL_BUILTIN_FUNC_STATEMENT_LOCALTIME,
        "STATEMENT_LOCALTIME",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoStatementLocalTime,
        ellGetFuncPtrStatementLocalTime
    },

    {
        KNL_BUILTIN_FUNC_TRANSACTION_LOCALTIME,
        "TRANSACTION_LOCALTIME",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,        
        STL_FALSE,
        ellGetFuncInfoTransactionLocalTime,
        ellGetFuncPtrTransactionLocalTime
    },
    
    {
        KNL_BUILTIN_FUNC_CLOCK_LOCALTIMESTAMP,
        "CLOCK_LOCALTIMESTAMP",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_EXPR,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoClockLocalTimestamp,
        ellGetFuncPtrClockLocalTimestamp
    },

    {
        KNL_BUILTIN_FUNC_STATEMENT_LOCALTIMESTAMP,
        "STATEMENT_LOCALTIMESTAMP",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoStatementLocalTimestamp,
        ellGetFuncPtrStatementLocalTimestamp
    },

    {
        KNL_BUILTIN_FUNC_TRANSACTION_LOCALTIMESTAMP,
        "TRANSACTION_LOCALTIMESTAMP",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,        
        STL_FALSE,
        ellGetFuncInfoTransactionLocalTimestamp,
        ellGetFuncPtrTransactionLocalTimestamp
    },

    {
        KNL_BUILTIN_FUNC_SYSDATE,
        "SYSDATE",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoSysDate,
        ellGetFuncPtrSysDate
    },

    {
        KNL_BUILTIN_FUNC_SYSTIME,
        "SYSTIME",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoSysTime,
        ellGetFuncPtrSysTime        
    },

    {
        KNL_BUILTIN_FUNC_SYSTIMESTAMP,
        "SYSTIMESTAMP",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoSysTimestamp,
        ellGetFuncPtrSysTimestamp
    },
    

    {
        KNL_BUILTIN_FUNC_STATEMENT_VIEW_SCN,
        "STATEMENT_VIEW_SCN",
        0,
        0,
        DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE,
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
        STL_FALSE,
        STL_FALSE,
        DTL_RETURN_NULLABLE_NEVER,
        DTL_ACTION_TYPE_NORMAL,
        DTL_ACTION_RETURN_PASS,
        STL_FALSE,
        ellGetFuncInfoStatementViewScn,
        ellGetFuncPtrStatementViewScn
    }
    
};


knlGetBuiltInListFuncPtr gListFunctionPtr[ KNL_LIST_FUNC_END_ID ][ KNL_LIST_FUNCTION_NA ] =
{
    
    /**********************************************
     * LIST OPERATION
     **********************************************/
    
    /**< IS EQUAL ROW */
    {
        knlGetListFuncPtrInvalid,                  /**< IS EQUAL ROW FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrInvalid,                  /**< IS EQUAL ROW FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrEqualRow,                 /**< IS EQUAL ROW FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrEqualRow                  /**< IS EQUAL ROW FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< IS NOT EQUAL ROW */
    {
        knlGetListFuncPtrInvalid,                  /**< NOT IS EQUAL ROW FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrInvalid,                  /**< NOT IS EQUAL ROW FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrNotEqualRow,              /**< NOT IS EQUAL ROW FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrNotEqualRow               /**< NOT IS EQUAL ROW FUNCTION ( MULTI  ROW ALL EXPR )       */
    },
    /**< IS LESS THAN ROW */
    {
        knlGetListFuncPtrInvalid,                  /**< IS LESS THAN ROW FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrInvalid,                  /**< IS LESS THAN ROW FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrLessThanRow,              /**< IS LESS THAN ROW FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrLessThanRow               /**< IS LESS THAN ROW FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< IS LESS THAN EQUAL ROW */
    {
        knlGetListFuncPtrInvalid,                  /**< IS LESS THAN EQUAL ROW FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrInvalid,                  /**< IS LESS THAN EQUAL ROW FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrLessThanEqualRow,         /**< IS LESS THAN EQUAL ROW FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrLessThanEqualRow          /**< IS LESS THAN EQUAL ROW FUNCTION ( MULTI  ROW ALL EXPR )       */
    },
    /**< IS GREATER THAN ROW */
    {
        knlGetListFuncPtrInvalid,                  /**< IS GREATER THAN ROW FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrInvalid,                  /**< IS GREATER THAN ROW FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrGreaterThanRow,           /**< IS GREATER THAN ROW FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrGreaterThanRow            /**< IS GREATER THAN ROW FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< IS GREATER THAN EQUAL ROW */
    {
        knlGetListFuncPtrInvalid,                  /**< IS GREATER THAN EQUAL ROW FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrInvalid,                  /**< IS GREATER THAN EQUAL ROW FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrGreaterThanEqualRow,      /**< IS GREATER THAN EQUAL ROW FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrGreaterThanEqualRow       /**< IS GREATER THAN EQUAL ROW FUNCTION ( MULTI  ROW ALL EXPR )       */
    },
    
   /*************************************************************
     * IN
     *************************************************************/

    /**< IN () */
    {
        knlGetListFuncPtrSingleInOnlyValue,    /**< IN FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrSingleIn,             /**< IN FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiInOnlyValue,     /**< IN FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiIn               /**< IN FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< NOT IN () */
    {
        knlGetListFuncPtrSingleNotInOnlyValue, /**< NOT IN FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrSingleNotIn,          /**< NOT IN FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiNotInOnlyValue,  /**< NOT IN FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiNotIn            /**< NOT IN FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /*************************************************************
     * EXISTS
     *************************************************************/

    /**< EXISTS () */
    {
        NULL,              /**< IN FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        NULL,              /**< IN FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        NULL,              /**< IN FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        NULL               /**< IN FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< NOT EXISTS () */
    {
        NULL,              /**< IN FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        NULL,              /**< IN FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        NULL,              /**< IN FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        NULL               /**< IN FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**********************************************
     * LIST OPERATION (ANY)
     **********************************************/
    
    /**< EQUAL ANY */
    {
        knlGetListFuncPtrSingleInOnlyValue,    /**< EQUAL ANY FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrSingleIn,             /**< EQUAL ANY FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiInOnlyValue,     /**< EQUAL ANY FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiIn               /**< EQUAL ANY FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< NOT EQUAL ANY */
     {
        knlGetListFuncPtrNotEqualAny,          /**< NOT EQUAL ANY FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrNotEqualAny,          /**< NOT EQUAL ANY FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiNotEqualAny,     /**< NOT EQUAL ANY FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiNotEqualAny      /**< NOT EQUAL ANY FUNCTION ( MULTI  ROW ALL EXPR )       */
    },
    
    /**< LESS THAN ANY */
    {
        knlGetListFuncPtrLessThanAny,           /**< LESS THAN ANY FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrLessThanAny,           /**< LESS THAN ANY FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiLessThanAny,      /**< LESS THAN ANY FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiLessThanAny       /**< LESS THAN ANY FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< LESS THAN EQUAL ANY */
    {
        knlGetListFuncPtrLessThanEqualAny,         /**< LESS THAN EQUAL FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrLessThanEqualAny,         /**< LESS THAN EQUAL FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiLessThanEqualAny,    /**< LESS THAN EQUAL FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiLessThanEqualAny     /**< LESS THAN EQUAL FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< GREATER THAN ANY */
    {
        knlGetListFuncPtrGreaterThanAny,           /**< GREATER THAN FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrGreaterThanAny,           /**< GREATER THAN FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiGreaterThanAny,      /**< GREATER THAN FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiGreaterThanAny       /**< GREATER THAN FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< GREATER THAN EQUAL ANY */
    {
        knlGetListFuncPtrGreaterThanEqualAny,      /**< GREATER THAN EQUAL FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrGreaterThanEqualAny,      /**< GREATER THAN EQUAL FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiGreaterThanEqualAny, /**< GREATER THAN EQUAL FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiGreaterThanEqualAny  /**< GREATER THAN EQUAL FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**********************************************
     * LIST OPERATION (ALL)
     **********************************************/
    
    /**< EQUAL ALL */
    {
        knlGetListFuncPtrEqualAll,           /**< EQUAL ALL FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrEqualAll,           /**< EQUAL ALL FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiEqualAll,      /**< EQUAL ALL FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiEqualAll       /**< EQUAL ALL FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< NOT EQUAL ALL */
     {
        knlGetListFuncPtrNotEqualAll,         /**< NOT EQUAL ALL FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrNotEqualAll,         /**< NOT EQUAL ALL FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiNotEqualAll,    /**< NOT EQUAL ALL FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiNotEqualAll     /**< NOT EQUAL ALL FUNCTION ( MULTI  ROW ALL EXPR )       */
    },
    
    /**< LESS THAN ALL */
    {
        knlGetListFuncPtrLessThanAll,         /**< LESS THAN ALL FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrLessThanAll,         /**< LESS THAN ALL FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiLessThanAll,    /**< LESS THAN ALL FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiLessThanAll     /**< LESS THAN ALL FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< LESS THAN EQUAL ALL */
    {
        knlGetListFuncPtrLessThanEqualAll,       /**< LESS THAN EQUAL ALL FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrLessThanEqualAll,       /**< LESS THAN EQUAL ALL FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiLessThanEqualAll,  /**< LESS THAN EQUAL ALL FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiLessThanEqualAll   /**< LESS THAN EQUAL ALL FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< GREATER THAN ALL */
    {
        knlGetListFuncPtrGreaterThanAll,      /**< GREATER THAN ALL FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrGreaterThanAll,      /**< GREATER THAN ALL FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiGreaterThanAll, /**< GREATER THAN ALL FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiGreaterThanAll  /**< GREATER THAN ALL FUNCTION ( MULTI  ROW ALL EXPR )       */
    },

    /**< GREATER THAN EQUAL ALL */
    {
        knlGetListFuncPtrGreaterThanEqualAll,  /**< GREATER THAN EQUAL ALL FUNCTION ( SINGLE VALUE IN ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrGreaterThanEqualAll,  /**< GREATER THAN EQUAL ALL FUNCTION ( SINGLE VALUE IN ROW ALL EXPR )       */
        knlGetListFuncPtrMultiGreaterThanEqualAll,  /**< GREATER THAN EQUAL ALL FUNCTION ( MULTI  ROW ONLY VALUE EXPR )    */
        knlGetListFuncPtrMultiGreaterThanEqualAll   /**< GREATER THAN EQUAL ALL FUNCTION ( MULTI  ROW ALL EXPR )       */
    }
};      


/** @} ellFunction */
 
