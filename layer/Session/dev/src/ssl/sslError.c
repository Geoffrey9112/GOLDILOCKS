/*******************************************************************************
 * sslError.c
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

/**
 * @file sslError.c
 * @brief Session Layer Error Routines
 */

/**
 * @addtogroup sslError
 * @{
 */

/**
 * @brief Session Layer 에러 테이블.
 * 내부에러에 대응되는 외부에러와 에러 메세지로 구성됨.
 */
stlErrorRecord gSessionErrorTable[] =
{
    {/* SSL_ERRCODE_INVALID_STATEMENT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Statement"
    },
    {/* SSL_ERRCODE_INVALID_ATTRIBUTE_VALUE */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_ATTRIBUTE_VALUE,
        "Invalid attribute value"
    },
    {/* SSL_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_ATTRIBUTE_IDENTIFIER,
        "Invalid attribute/option identifier"
    },
    {/* SSL_ERRCODE_TRANSACTION_IS_READ_ONLY */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "Transaction is READ ONLY"
    },
    {/* SSL_ERRCODE_STATEMENT_CANNOT_EXECUTE_AS_ATOMIC_ACTION */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "Statement cannot execute as atomic action"
    },
    {/* SSL_ERRCODE_INVALID_PREPARABLE_PHASE */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "Preparable phase is open phase"
    },
    {/* SSL_ERRCODE_COMMUNICATION_LINK_FAILURE */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE,
        "Communication link failure"
    },
    {   /* SSL_ERRCODE_NOT_IMPLEMENTED */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
        "not implemented feature, in a function %s"
    },
    {   /* SSL_ERRCODE_LIMIT_ON_THE_NUMBER_OF_HANDLES_EXCEEDED */
        STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_LIMIIT_ON_NUMBER_OF_HANDLES_EXCEEDED,
        "Limit on the number of SQL statement exceeded"
    },
    {   /* SSL_ERRCODE_NOT_ALLOW_COMMAND_IN_XA */
        STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS,
        "The command cannot be executed when global transaction is in the ACTIVE state"
    },
    {   /* SSL_ERRCODE_INVALID_DESCRIPTOR_INDEX */
        STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_INVALID_DESCRIPTOR_INDEX,
        "Invalid descriptor index"
    },
    {   /* SSL_ERRCODE_CHARACTERSET_NOT_EQUAL */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Character set not equal(%s and %s)"
    },


    {
        0,
        NULL
    }
};

/** @} */
