/*******************************************************************************
 * ztqError.c
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

stlErrorRecord gZtqErrorTable[] =
{
    {   /* ZTQ_ERRCODE_SYNTAX */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "syntax error"
    },
    {   /* ZTQ_ERRCODE_NOT_SUPPORTED */
        STL_EXT_ERRCODE_FEATURE_NOT_SUPPORTED_NO_SUBCLASS,
        "feature not supported"
    },
    {   /* ZTQ_ERRCODE_NOT_DECLARED_HOST_VARIABLE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Bind variable \"%s\" not declared"
    },
    {   /* ZTQ_ERRCODE_INVALID_TABLE_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "\"%s\": table or view does not exist."
    },
    {   /* ZTQ_ERRCODE_INVALID_INDEX_NAME */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "\"%s\": index does not exist."
    },
    {   /* ZTQ_ERRCODE_SERVICE_NOT_AVAILABLE */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "Service is not available."
    },
    {   /* ZTQ_ERRCODE_NOT_PREPARED */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "Statement did not prepared."
    },
    {   /* ZTQ_ERRCODE_NAMED_STMT_DUPLICATE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "'%s': duplicate statement"
    },
    {   /* ZTQ_ERRCODE_NAMED_STMT_NOT_EXIST */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "'%s': statement does not exist"
    },
    {   /* ZTQ_ERRCODE_NAMED_STMT_NOT_PREPARED */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "'%s': statement is not prepared"
    },
    {   /* ZTQ_ERRCODE_INSUFFICIENT_PRIVILEGES */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "insufficient privileges"
    },
    {   /* ZTQ_ERRCODE_NOT_SUPPORT_FUNCTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "driver does not support %s function"
    },
    {   /* ZTQ_ERRCODE_INVALID_OBJECT_DESCRIBE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "invalid ojbect for describe"
    },
    {   /* ZTQ_ERRCODE_MISMATCH_DRIVER */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "driver does not support the requested connection mode( %s )"
    },
    {   /* ZTQ_ERRCODE_XA_RBROLLBACK */
        STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_NO_SUBCLASS,
        "the rollback was caused by an unspecified reason"
    },
    {   /* ZTQ_ERRCODE_XA_COMMFAIL */
        STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_NO_SUBCLASS,
        "the rollback was caused by a communication failure"
    },
    {   /* ZTQ_ERRCODE_XA_RBDEADLOCK */
        STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_SERIALIZATION_FAILURE,
        "a deadlock was detected"
    },
    {   /* ZTQ_ERRCODE_XA_RBINTEGRITY */
        STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION,
        "a condition that violates the integrity of the resources was detected"
    },
    {   /* ZTQ_ERRCODE_XA_RBOTHER */
        STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_NO_SUBCLASS,
        "the resource manager rolled back the transaction branch for a reason not on this list"
    },
    {   /* ZTQ_ERRCODE_XA_RBPROTO */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "a protocol error occurred in the resource manager"
    },
    {   /* ZTQ_ERRCODE_XA_RBTIMEOUT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "a transaction branch took too long"
    },
    {   /* ZTQ_ERRCODE_XA_RBTRANSIENT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "may retry the transaction branch"
    },
    {   /* ZTQ_ERRCODE_XA_RBEND */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the inclusive upper bound of the rollback codes"
    },
    {   /* ZTQ_ERRCODE_XA_NOMIGRATE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "resumption must occur where suspension occurred"
    },
    {   /* ZTQ_ERRCODE_XA_HEURHAZ */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the transaction branch may have been heuristically completed"
    },
    {   /* ZTQ_ERRCODE_XA_HEURCOM */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the transaction branch has been heuristically committed"
    },
    {   /* ZTQ_ERRCODE_XA_HEURRB */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the transaction branch has been heuristically rolled back"
    },
    {   /* ZTQ_ERRCODE_XA_HEURMIX */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the transaction branch has been heuristically committed and rolled back"
    },
    {   /* ZTQ_ERRCODE_XA_RETRY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "routine returned with no effect and may be re-issued"
    },
    {   /* ZTQ_ERRCODE_XA_RDONLY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the transaction branch was read-only and has been committed"
    },
    {   /* ZTQ_ERRCODE_XA_ASYNC */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "asynchronous operation already outstanding"
    },
    {   /* ZTQ_ERRCODE_XA_RMERR */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "a resource manager error occurred in the transaction branch"
    },
    {   /* ZTQ_ERRCODE_XA_NOTA */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the XID is not valid"
    },
    {   /* ZTQ_ERRCODE_XA_INVAL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid arguments were given"
    },
    {   /* ZTQ_ERRCODE_XA_PROTO */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "routine invoked in an improper context"
    },
    {   /* ZTQ_ERRCODE_XA_RMFAIL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "resource manager unavailable"
    },
    {   /* ZTQ_ERRCODE_XA_DUPID */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the XID already exists"
    },
    {   /* ZTQ_ERRCODE_XA_OUTSIDE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "resource manager doing work outside transaction"
    },
    {   /* ZTQ_ERRCODE_XA_UNKNOWN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "unknown xa error"
    },
    {   /* ZTQ_ERRCODE_XA_RBROLLBACK_ROLLBACKONLY */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "marked the transaction branch rollback-only for unspecified reason"
    },
    {   /* ZTQ_ERRCODE_XA_RMERR_OPEN */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot open the resource"
    },
    {   /* ZTQ_ERRCODE_XA_RMERR_CLOSE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cannot close the resource"
    },
    {   /* ZTQ_ERRCODE_NULL_SQL_STRING */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "the sql is empty"
    },
    {   /* ZTQ_ERRCODE_INVALID_HOST_TYPE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid host variable type"
    },
    {   /* ZTQ_ERRCODE_CONNECTION_DOES_NOT_EXIST */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_CONNECTION_DOES_NOT_EXIST,
        "connection does not exist"
    },
    {   /* ZTQ_ERRCODE_INVALID_NUMSZE_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid NUMSIZE value: NUMSIZE must be between %d and %d"
    },
    {   /* ZTQ_ERRCODE_INVALID_PAGESZE_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid PAGESIZE value: PAGESIZE must be between %d and %d"
    },
    {   /* ZTQ_ERRCODE_INVALID_LINESZE_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid LINESIZE value: LINESIZE must be between %d and %d"
    },
    {   /* ZTQ_ERRCODE_INVALID_COLSZE_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid COLSIZE value: COLSIZE must be between %d and %d"
    },
    {   /* ZTQ_ERRCODE_INVALID_HISTORY_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid HISTORY value: HISTORY must be less equal than %d"
    },
    {   /* ZTQ_ERRCODE_INVALID_DDLSIZE_VALUE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid DDLSIZE value: DDLSIZE must be between %d and %d"
    },
    {   /* ZTQ_ERRCODE_INVALID_OBJECT_IDENTIFIER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid object identifier"
    },
    {   /* ZTQ_ERRCODE_NOT_ENOUGH_DDLSIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "not enough DDLSIZE. \nuse command: \\set ddlsize {n}"
    },
    {   /* ZTQ_ERRCODE_CONFIRMATION_PASSWORD_MISMATCH */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "confirmation password mismatch"
    },
    
    {
        0,
        NULL
    }
};

