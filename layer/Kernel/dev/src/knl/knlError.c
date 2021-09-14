/*******************************************************************************
 * knlError.c
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
 * @file knlError.c
 * @brief Kernel Layer Error Routines
 */

#include <stl.h>

/**
 * @addtogroup knlError
 * @{
 */

/**
 * @brief Kernel Layer 에러 테이블.
 * 내부에러에 대응되는 외부에러와 에러 메세지로 구성됨.
 */
stlErrorRecord gKernelErrorTable[] =
{
    {/* KNL_ERRCODE_INITIALIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Failed to initialize kernel layer"
    },
    {/* KNL_ERRCODE_INITIALIZED_YET */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "kernel layer is not initialized yet"
    },
    {/* KNL_ERRCODE_INVALID_ARGUMENT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid argument"
    },
    {/* KNL_ERRCODE_INVALID_SHM_TYPE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid shared memory segment type"
    },
    {/* KNL_ERRCODE_INVALID_STATIC_SHM */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "static shared memory segment is corrupted"
    },
    {/* KNL_ERRCODE_EXCEEDED_SHM_SEGMENT_COUNT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "maximum number of shared memory segment num exceeded"
    },
    {/* KNL_ERRCODE_INTERNAL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "kernel layer internal error - %s"
    },
    {/* KNL_ERRCODE_TOO_LONG_IDENTIFIER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "identifier is too long"
    },
    {/* KNL_ERRCODE_TOO_LONG_COMMENT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "comment is too long"
    },
    {/* KNL_ERRCODE_FXTABLE_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_BASE_TABLE_OR_VIEW_NOT_FOUND,
        "fixed table does not exist"
    },
    {/* KNL_ERRCODE_INSUFFICIENT_STATIC_AREA */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Insufficient static area"
    },
    {/* KNL_ERRCODE_UNABLE_EXTEND_MEMORY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to extend memory: [MAX: %ld, TOTAL: %ld, ALLOC: %ld] DESC: %s"
    },
    {/* KNL_ERRCODE_FILE_DESCRIPTOR_FULL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "too many files are opened"
    },
    {/* KNL_ERRCODE_PROPERTY_NOT_EXIST */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Property(%s) does not exist"
    },
    {/* KNL_ERRCODE_PROPERTY_INVALID_STARTUP_PHASE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to change property at this startup phase.(%s)"
    },
    {/* KNL_ERRCODE_PROPERTY_NOT_MODIFIABLE */
        STL_EXT_ERRCODE_ATTEMPT_TO_ASSIGN_TO_NON_UPDATABLE_COLUMN_NO_SUBCLASS,
        "Unable to change read-only property.(%s)"
    },
    {/* KNL_ERRCODE_PROPERTY_INVALID_VALUE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NO_SUBCLASS,
        "Invalid property value.(%s)"
    },
    {/* KNL_ERRCODE_PROPERTY_INVALID_SES_MODIFY */
        STL_EXT_ERRCODE_ATTEMPT_TO_ASSIGN_TO_NON_UPDATABLE_COLUMN_NO_SUBCLASS,
        "Specified property cannot be modified.(%s)"
    },
    {/* KNL_ERRCODE_PROPERTY_INVALID_SCOPE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NO_SUBCLASS,
        "Specified property cannot be modified with this SCOPE option.(%s)"
    },
    {/* KNL_ERRCODE_PROPERTY_INVALID_MODE */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NO_SUBCLASS,
        "Specified property cannot be modified.(%s)"
    },
    {/* KNL_ERRCODE_PROPERTY_INVALID_HOME_DIR */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NO_SUBCLASS,
        "Invalid GOLDILOCKS_DATA Directory"
    },
    {/* KNL_ERRCODE_PROPERTY_INVALID_SID */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NO_SUBCLASS,
        "Invalid SID"
    },
    {/* KNL_ERRCODE_PROPERTY_INVALID_DATATYPE */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_DATA_TYPE,
        "Invalid property data type"
    },
    {/* KNL_ERRCODE_PROPERTY_INVALID_ID */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NO_SUBCLASS,
        "Invalid Property ID"
    },
    {/* KNL_ERRCODE_EXCEEDED_PROCESS_COUNT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "exceeded maximum number of processes"
    },
    {/* KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "File does not exist (%s)"
    },
    {/* KNL_ERRCODE_PROPERTY_FILE_EXIST */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "File exist (%s)"
    },
    {/* KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "File Process Error"
    },
    {/* KNL_ERRCODE_PROPERTY_NOT_READONLY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Property is not ReadOnly"
    },
    {/* KNL_ERRCODE_PROPERTY_EXIST_READONLY_PROPERTY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "ReadOnly Property Already Setted"
    },
    {/* KNL_ERRCODE_PROPERTY_FILE_PARSING_ERROR */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Property File Parsing Error Line (%d)"
    },
    {/* KNL_ERRCODE_FILTER_NOT_SUPPORT_RETURN_TYPE_CLASS */
        STL_EXT_ERRCODE_FEATURE_NOT_SUPPORTED_NO_SUBCLASS,
        "Unsupported return type is used"
    },
    {/* KNL_ERRCODE_FILTER_NOT_SUPPORT_FUNCTION */
        STL_EXT_ERRCODE_FEATURE_NOT_SUPPORTED_NO_SUBCLASS,
        "Function does not exist"
    },
    {/* KNL_ERRCODE_FILTER_CANNOT_MAKE_RANGE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "It can not make range"
    },
    {/* KNL_ERRCODE_SERVICE_NOT_AVAILABLE */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "Service is not available"
    },
    {/* KNL_ERRCODE_INVALID_BREAKPOINT_NAME */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Specified Breakpoint name is not registered yet"
    },
    {/* KNL_ERRCODE_BREAKPOINT_GENERAL_ABORT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "General abort error of Breakpoint"
    },
    {/* KNL_ERRCODE_BREAKPOINT_INVALID_SESS_NAME */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Session Name"
    },
    {/* KNL_ERRCODE_EXCEEDED_IDLE_TIMEOUT */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_TIMEOUT_EXPIRED,
        "Exceeded maximum idle time"
    },
    {/* KNL_ERRCODE_EXCEEDED_QUERY_TIMEOUT */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_TIMEOUT_EXPIRED,
        "Exceeded maximum query time"
    },
    {/* KNL_ERRCODE_COMMUNICATION_LINK_FAILURE */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE,
        "Communication link failure"
    },
    {/* KNL_ERRCODE_USER_SESSION_ID_DOSE_NOT_EXIST */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "User session ID does not exist"
    },
    {/* KNL_ERRCODE_SERVER_REJECTED_THE_CONNECTION */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_SQLSERVER_REJECTED_ESTABLISHMENT_OF_SQLCONNECTION,
        "Server rejected the connection"
    },
    {/* KNL_ERRCODE_OPERATION_CANCELED */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_OPERATION_CANCELED,
        "operation canceled"
    },
    {/* KNL_ERRCODE_INVALID_PROCESS_ID */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "invalid process identifier"
    },
    {/* KNL_ERRCODE_MISMATCH_BINARY_VERSION */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_SQLSERVER_REJECTED_ESTABLISHMENT_OF_SQLCONNECTION,
        "mismatched binary version - server(%s.%d.%d.%d.%d), client(%s.%d.%d.%d.%d)"
    },
    {/* KNL_ERRCODE_SESSION_CANNOT_BE_KILLED_NOW */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "session cannot be killed now"
    },
    {/* KNL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED */
        STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE,
        "data is outside the range of the data type to which the number is being converted"
    },
    {/* KNL_ERRCODE_C_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION */
        STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW,
        "one or more fields of data value were truncated during conversion"
    },
    {/* KNL_ERRCODE_NOT_IMPLEMENTED */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
        "not implemented feature, in a function "
    },
    {   /* KNL_ERRCODE_INSUFFICIENT_PLAN_CACHE_MEMORY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "insufficient plan cache memory"
    },
    {
        /* KNL_ERRCODE_FAIL_ATTACH_FILE_NOT_EXIST */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Unable to attache the shared memory segment. File does not exist(%s)"
    },
    {/* KNL_ERRCODE_INVALID_DUMP_OPTION_STRING */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid dump option string: %s"
    },
    {/* KNL_ERRCODE_MODIFIED_DUMP_OBJECT */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "dump object modified by concurrent statement"
    },
    {/* STL_ERRCODE_INVALID_NUMA_MAP */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid numa map (%s)"
    },
    {   /* KNL_ERRCODE_ADMIN_MUST_DEDICATED */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "AS ADMIN must be executed in dedicated session"
    },
    {/* KNL_ERRCODE_FILE_ALREADY_IN_USE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "unable to create file '%s' - already in use"
    },
    {
        0,
        NULL
    }
};

/** @} */
