/*******************************************************************************
 * ztpeError.c
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

#include <stl.h>

stlErrorRecord gZtpErrorTable[] =
{
    {   /* ZTP_ERRCODE_SYNTAX */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "syntax error"
    },
    {   /* ZTP_ERRCODE_NOT_SUPPORTED */
        STL_EXT_ERRCODE_FEATURE_NOT_SUPPORTED_NO_SUBCLASS,
        "feature not supported"
    },
    {   /* ZTP_ERRCODE_NOT_DECLARED_HOST_VARIABLE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "Host variable \"%s\" not declared"
    },
    {   /* ZTP_ERRCODE_NOT_DECLARED_CURSOR */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "Cursor \"%s\" not declared"
    },
    {   /* ZTP_ERRCODE_FILE_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "\"%s\": file not exist"
    },
    {   /* ZTP_ERRCODE_EXPRESSION_TYPE_DOES_NOT_MATCH_USAGE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "expression type does not match usage"
    },
    {   /* ZTP_ERRCODE_FATAL_ERROR_WHILE_DOING_EMBEDDED_SQL_PREPROCESSING */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "Fatal error while doing embedded SQL preprocessing"
    },
    {   /* ZTP_ERRCODE_CREATE_SYMTAB */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Precompiler can't create symbol table"
    },
    {   /* ZTP_ERRCODE_DESTROY_SYMTAB */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Precompiler can't destroy symbol table"
    },
    {   /* ZTP_ERRCODE_IDENTIFIER_LENGTH_EXCEEDED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "identifier is too long"
    },
    {   /* ZTP_ERRCODE_INVALID_PP_TOKEN */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "token \"%s\" is not valid in preprocessor expressions"
    },
    {   /* ZTP_ERRCODE_MULTI_CHARACTER_CHARACTER_CONSTANT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "multi-character character constant"
    },
    {   /* ZTP_ERRCODE_FLOATING_CONSTANT_IN_PP */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "floating constant in preprocessor expression"
    },
    {   /* ZTP_ERRCODE_INVALID_SUFFIX_ON_INTEGER_CONSTANT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid suffix \"%s\" on integer constant"
    },
    {   /* ZTP_ERRCODE_CURSOR_NAME_ALREADY_DECLARED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "cursor name \"%s\" already declared"
    },
    {   /* ZTP_ERRCODE_CURSOR_IS_NOT_A_DYNAMIC_CURSOR */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_REQUIRED_FOR_DYNAMIC_PARAMETERS,
        "cursor \"%s\" is not a dynamic cursor"
    },
    {   /* ZTP_ERRCODE_MEMBER_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "\"%s\" has no member named \"%s\""
    },
    {   /* ZTP_ERRCODE_UNKNOWN_STORAGE_SIZE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "storage size of \"%s\" isn't known"
    },
    {   /* ZTP_ERRCODE_TYPE_MUST_BE_INTEGER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "variable \"%s\" must be integer type"
    },
    {   /* ZTP_ERRCODE_TYPE_MUST_BE_STRING */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "variable \"%s\" must be string type"
    },
    {   /* ZTP_ERRCODE_INVALID_INDICATOR_TYPE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid indicator variable type: indicator variable \"%s\" must have numeric type(char/short/int/long)"
    },
    {   /* ZTP_ERRCODE_DUPLICATE_KEYWORD */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "duplicate \"%s\""
    },
    {   /* ZTP_ERRCODE_BOTH_KEYWORD */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "both \"%s\" and \"%s\" in declaration specifiers"
    },
    {   /* ZTP_ERRCODE_MANY_TYPES */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "two or more data types in declaration specifiers"
    },
    {   /* ZTP_ERRCODE_CPP_PUNCTUATION_SEQUENCES */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "C++ punctuation sequences are not permitted"
    },

    {
        0,
        NULL
    }
};
