/*******************************************************************************
 * sleError.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sleError.c 13675 2014-10-06 14:17:27Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#include <stl.h>

stlErrorRecord gServerLibraryErrorTable[] =
{
    {   /* SLL_ERRCODE_NOT_SUPPORTED */
        STL_EXT_ERRCODE_FEATURE_NOT_SUPPORTED_NO_SUBCLASS,
        "feature not supported"
    },
    {   /* SLL_ERRCODE_SERVER_IS_NOT_RUNNING */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Server is not running"
    },
    {   /* SLL_ERRCODE_UNDEFINED_ENV_VARIABLE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Environment Variable \"%s\" is not defined."
    },
    {   /* SLL_ERRCODE_INVALID_PROTOCOL */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_NO_SUBCLASS,
        "Invalid protocol: "
    },
    {   /* SLL_ERRCODE_STARTUP_MUST_DEDICATED */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "STARTUP must be executed in dedicated session"
    },
    {   /* SLL_ERRCODE_SESSION_KILLED */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "session killed"
    },

    {
        0,
        NULL
    }
};

