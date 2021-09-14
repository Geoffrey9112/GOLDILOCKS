/*******************************************************************************
 * ellError.c
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
 * @file ellError.c
 * @brief Execution Library Layer Error Routines
 */


#include <stl.h>


/**
 * @addtogroup ellError
 * @{
 */

/**
 * @brief Execution Library Layer 에러 테이블.
 * 내부에러에 대응되는 외부에러와 에러 메세지로 구성됨.
 */
stlErrorRecord gExecutionLibraryErrorTable[] =
{
    {   /* ELL_ERRCODE_INITIALIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "failed to initialize execution library layer"
    },
    {   /* ELL_ERRCODE_INITIALIZED_YET */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "execution library layer is not initialized yet"
    },
    {   /* ELL_ERRCODE_INVALID_SCHEMA_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "\"%s\": schema does not exist"
    },
    {   /* ELL_ERRCODE_INVALID_TABLE_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "\"%s\": table or view does not exist"
    },
    {   /* ELL_ERRCODE_INVALID_INDEX_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "\"%s\": index does not exist"
    },
    {   /* ELL_ERRCODE_INVALID_COLUMN_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "\"%s\": column does not exist"
    },
    {   /* ELL_ERRCODE_INTEGRITY_VIOLATION */
        STL_EXT_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION_NO_SUBCLASS,
        "\"%s\": dictionary integrity constraint violation by concurrent DDL execution"
    },
    {
        /* ELL_ERRCODE_ELK_NOT_ENOUGH_STACK_SPACE */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_STRING_LENGTH_OR_BUFFER_LENGTH,
        "not enough stack space to add entries"
    },
    {   /* ELL_ERRCODE_TABLE_MODIFIED_IN_VALIDATION */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "table related objects are modified while validation"
    },
    {   /* ELL_ERRCODE_INVALID_ARGUMENT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid argument"
    },
    {   /* ELL_ERRCODE_INVALID_OBJECT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "object modified by concurrent DDL execution"
    },
    {   /* ELL_ERRCODE_NOT_IMPLEMENTED */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
        "not implemented feature, in a function %s"
    },
    {   /* ELL_ERRCODE_USER_DROPPED_BY_OTHER_SESSION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "user dropped by other session"
    },
    {   /* ELL_ERRCODE_USER_DEFAULT_SCHEMA_DROPPED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "user default schema dropped"
    },
    {   /* ELL_ERRCODE_USER_DEFAULT_DATA_TABLESPACE_DROPPED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "user default data tablespace dropped"
    },
    {   /* ELL_ERRCODE_USER_DEFAULT_TEMPORARY_TABLESPACE_DROPPED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "user default temporary tablespace dropped"
    },
    {   /* ELL_ERRCODE_LIMIT_ON_THE_NUMBER_OF_NAMED_CURSOR_EXCEEDED */
        STL_EXT_ERRCODE_PASS_THROUGH_SPECIFIC_CONDITION_INVALID_CURSOR_ALLOCATION,
        "limit(%d) on the number of named cursor exceeded"
    },
    
    {
        0,
        NULL
    }
};

/** @} ellError */
