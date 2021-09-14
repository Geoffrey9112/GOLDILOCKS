/*******************************************************************************
 * zlpeError.c
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

stlErrorRecord gZlplErrorTable[] =
{
    {   /* ZLPL_ERRCODE_SYNTAX */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "syntax error"
    },
    {   /* ZLPL_ERRCODE_NOT_SUPPORTED */
        STL_EXT_ERRCODE_FEATURE_NOT_SUPPORTED_NO_SUBCLASS,
        "feature not supported"
    },
    {   /* ZLPL_ERRCODE_NOT_DECLARED_HOST_VARIABLE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Bind variable \"%s\" was not declared"
    },
    {   /* ZLPL_ERRCODE_INITIALIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Application can't be initialized"
    },
    {   /* ZLPL_ERRCODE_FINALIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Application can't be finalized"
    },
    {   /* ZLPL_ERRCODE_SERVICE_NOT_AVAILABLE */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "Service is not available"
    },
    {   /* ZLPL_ERRCODE_NOT_PREPARED */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "Statement did not prepared"
    },
    {   /* ZLPL_ERRCODE_CONNECTION_NOT_ESTABLISHED */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_SQLCLIENT_UNABLE_TO_ESTABLISH_SQLCONNECTION,
        "Client can't establish SQL-connection"
    },
    {   /* ZLPL_ERRCODE_CURSOR_IS_NOT_OPEN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cursor \"%s\" is not open"
    },
    {   /* ZLPL_ERRCODE_CREATE_SEMAPHORE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Application can't create semaphore"
    },
    {   /* ZLPL_ERRCODE_DESTROY_SEMAPHORE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Application can't destroy semaphore"
    },
    {   /* ZLPL_ERRCODE_CREATE_ALLOCATOR */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Application can't create allocator"
    },
    {   /* ZLPL_ERRCODE_DESTROY_ALLOCATOR */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Application can't destroy allocator"
    },
    {   /* ZLPL_ERRCODE_CREATE_SYMTAB */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Application can't create symbol table"
    },
    {   /* ZLPL_ERRCODE_DESTROY_SYMTAB */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Application can't destroy symbol table"
    },
    {   /* ZLPL_ERRCODE_ALLOCATE_ENV */
        STL_EXT_ERRCODE_DRIVERS_SQLALLOCHANDLE_ON_SQL_HANDLE_ENV_FAILED,
        "Driver's SQLAllocHandle on SQL_HANDLE_ENV failed"
    },
    {   /* ZLPL_ERRCODE_SET_ENV_ATTR */
        STL_EXT_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION,
        "Driver does not support ODBC 3.0"
    },
    {   /* ZLPL_ERRCODE_ALLOCATE_MEM */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Application can't allocate memory"
    },
    {   /* ZLPL_ERRCODE_STATEMEMT_NAME_NOT_PREPARED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "Statement \"%s\" was not prepared"
    },
    {   /* ZLPL_ERRCODE_DESCRIPTOR_NAME_IS_TOO_LONG */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "Descriptor name is too long"
    },
    {   /* ZLPL_ERRCODE_NOT_IMPLEMENTED_FEATURE */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
        "Not implemented feature, in a function %s"
    },
    {   /* ZLPL_ERRCODE_USING_CLAUSE_REQUIRED_FOR_DYNAMIC_PARAMTERS */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_REQUIRED_FOR_DYNAMIC_PARAMETERS,
        "USING clause required for dynamic paramters"
    },
    {   /* ZLPL_ERRCODE_USING_CLAUSE_ILLEGAL_VARIABLE_NAME_NUMBER */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_DOES_NOT_MATCH_DYNAMIC_PARAMETER_SPEC,
        "USING clause: illegal variable name/number"
    },
    {   /* ZLPL_ERRCODE_USING_CLAUSE_NOT_ALL_VARIABLES_BOUND */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_DOES_NOT_MATCH_DYNAMIC_PARAMETER_SPEC,
        "USING clause: not all variables bound"
    },
    {   /* ZLPL_ERRCODE_INTO_CLAUSE_REQUIRED_FOR_RESULT_FIELDS */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_REQUIRED_FOR_RESULT_FIELDS,
        "INTO clause required for result fields"
    },
    {   /* ZLPL_ERRCODE_INTO_CLAUSE_ILLEGAL_VARIABLE_NAME_NUMBER */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_DOES_NOT_MATCH_DYNAMIC_PARAMETER_SPEC,
        "INTO clause: illegal variable name/number"
    },
    {   /* ZLPL_ERRCODE_INTO_CLAUSE_NOT_ALL_VARIABLES_BOUND */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_DOES_NOT_MATCH_DYNAMIC_PARAMETER_SPEC,
        "INTO clause: not all variables bound"
    },
    {   /* ZLPL_ERRCODE_NULL_VALUE_NO_INDICATOR_PARAMETER */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NULL_VALUE_NO_INDICATOR_PARAMETER,
        "null value, no indicator parameter"
    },
    {   /* ZLPL_ERRCODE_STATEMENT_IS_NOT_A_QUERY */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_PREPARED_STATEMENT_NOT_A_CURSOR_SPECIFICATION,
        "cusor \"%s\" statement is not a query"
    },
    {   /* ZLPL_ERRCODE_CONNECTION_NOT_OPEN */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_CONNECTION_DOES_NOT_EXIST,
        "connection not open"
    },
    {   /* ZLPL_ERRCODE_CURSOR_IS_NOT_UPDATABLE */
        STL_EXT_ERRCODE_INVALID_CURSOR_NAME_NO_SUBCLASS,
        "cursor \"%s\" is not updatable"
    },
    {   /* ZLPL_ERRCODE_CONNECTION_NAME_IN_USE */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_CONNECTION_NAME_IN_USE,
        "connection name \"%s\" in use"
    },
    {   /* ZLPL_ERRCODE_DUPLICATE_CONTEXT */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_CONNECTION_NAME_IN_USE,
        "duplicate connection context"
    },

    {
        0,
        NULL
    }
};

