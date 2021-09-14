/*******************************************************************************
 * ztpErrDef.h
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


#ifndef _ZTPERRDEF_H_
#define _ZTPERRDEF_H_ 1

#include <stl.h>

/**
 * @file ztpErrDef.h
 * @brief Gliese SQL Definition
 */

/**
 * @defgroup ztpError Error
 * @{
 */

/**
 * @brief syntax error
 */
#define ZTP_ERRCODE_SYNTAX                                  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 0 )

/**
 * @brief feature not supported
 */
#define ZTP_ERRCODE_NOT_SUPPORTED                           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 1 )

/**
 * @brief Host variable \"%s\" not declared
 */
#define ZTP_ERRCODE_NOT_DECLARED_HOST_VARIABLE              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 2 )

/**
 * @brief Cursor \"%s\" not declared
 */
#define ZTP_ERRCODE_NOT_DECLARED_CURSOR                     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 3 )

/**
 * @brief \"%s\": file not exist
 */
#define ZTP_ERRCODE_FILE_NOT_EXIST                          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 4 )

/**
 * @breif expression type does not match usage
 */
#define ZTP_ERRCODE_EXPRESSION_TYPE_DOES_NOT_MATCH_USAGE        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 5 )

/**
 * @brief Fatal error while doing embedded SQL preprocessing
 */
#define ZTP_ERRCODE_FATAL_ERROR_WHILE_DOING_EMBEDDED_SQL_PREPROCESSING   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 6 )

/**
 * @brief Precompiler can't create symbol table
 */
#define ZTP_ERRCODE_CREATE_SYMTAB   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 7 )

/**
 * @brief Precompiler can't destroy symbol table
 */
#define ZTP_ERRCODE_DESTROY_SYMTAB   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 8 )

/**
 * @brief identifier is too long
 */
#define ZTP_ERRCODE_IDENTIFIER_LENGTH_EXCEEDED          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 9 )

/**
 * @brief token "%s" is not valid in preprocessor expressions
 */
#define ZTP_ERRCODE_INVALID_PP_TOKEN                            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 10 )

/**
 * @brief multi-character character constant
 */
#define ZTP_ERRCODE_MULTI_CHARACTER_CHARACTER_CONSTANT          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 11 )

/**
 * @brief floating constant in preprocessor expression
 */
#define ZTP_ERRCODE_FLOATING_CONSTANT_IN_PP                     \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 12 )

/**
 * @brief invalid suffix "%s" on integer constant
 */
#define ZTP_ERRCODE_INVALID_SUFFIX_ON_INTEGER_CONSTANT          \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 13 )

/**
 * @brief cursor name "%s" already declared
 */
#define ZTP_ERRCODE_CURSOR_NAME_ALREADY_DECLARED                \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 14 )

/**
 * @brief cursor "%s" is not a dynamic cursor
 */
#define ZTP_ERRCODE_CURSOR_IS_NOT_A_DYNAMIC_CURSOR              \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 15 )

/**
 * @brief "struct %s" has no member named "%s"
 */
#define ZTP_ERRCODE_MEMBER_NOT_EXIST                            \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 16 )

/**
 * @brief storage size of "%s" isn't known
 */
#define ZTP_ERRCODE_UNKNOWN_STORAGE_SIZE                        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 17 )

/**
 * @brief variable "%s" must be integer type
 */
#define ZTP_ERRCODE_TYPE_MUST_BE_INTEGER                        \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 18 )

/**
 * @brief variable "%s" must be string type
 */
#define ZTP_ERRCODE_TYPE_MUST_BE_STRING                         \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 19 )

/**
 * @brief invalid indicator variable type: indicator variable "%s" must have numeric type(char/short/int/long)
 */
#define ZTP_ERRCODE_INVALID_INDICATOR_TYPE                      \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 20 )

/**
 * @brief duplicate %s
 */
#define ZTP_ERRCODE_DUPLICATE_KEYWORD                           \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 21 )

/**
 * @brief both %s and %s in declaration specifiers
 */
#define ZTP_ERRCODE_BOTH_KEYWORD                                \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 22 )

/**
 * @brief two or more data types in declaration specifiers
 */
#define ZTP_ERRCODE_MANY_TYPES                                  \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 23 )

/**
 * @brief C++ punctuation sequences are not permitted
 */
#define ZTP_ERRCODE_CPP_PUNCTUATION_SEQUENCES                   \
    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GPEC, 24 )

#define ZTP_MAX_ERROR     25

/** @} */

#endif /* _ZTPERRDEF_H_ */
