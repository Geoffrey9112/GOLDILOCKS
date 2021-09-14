/*******************************************************************************
 * ztleError.c
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

stlErrorRecord gZtleErrorTable[] =
{
    {   /* ZTLE_ERRCODE_CONF_FILE_NOT_EXIST */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Listener configuration file \"%s\" doesn't exist."
    },
    {   /* ZTLE_ERRCODE_UNDEFINED_ENV_VARIABLE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Environment Variable \"%s\" is not defined."
    },
    {   /* ZTLE_ERRCODE_ALREADY_START */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Listener is already started."
    },
    {   /* ZTLE_ERRCODE_OVERFLOW_FD */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "overflow fd"
    },
    {   /* ZTLE_ERRCODE_INVALID_COMMUNICATION_PROTOCOL */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_NO_SUBCLASS,
        "Invalid communication protocol"
    },
    {   /* ZTLE_ERRCODE_ACCESS_DENIED */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_SQLSERVER_REJECTED_ESTABLISHMENT_OF_SQLCONNECTION,
        "access denied"
    },
    {   /* ZTLE_ERRCODE_TIMEDOUT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Operation timed out"
    },
    {   /* ZTLE_ERRCODE_SHARED_MODE_INACTIVE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "shared mode inactive"
    },
    {   /* ZTLE_ERRCODE_INVALID_PROPERTY_DEFAULT_CS_MODE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid property value [ DEDICATED | SHARED ] : %s"
    },
    {   /* ZTLE_ERRCODE_INVALID_PROPERTY_VALIDNODE_CHECKING */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid property value [ NO | INVITED | EXCLUDED ] : %s"
    },
    {   /* ZTLE_ERRCODE_INVALID_PROPERTY_BOOLEAN_TYPE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid property value [ YES | NO | 1 | 0 ]: %s"
    },
    {   /* ZTLE_ERRCODE_INVALID_PROPERTY_INT_TYPE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid property value [ %ld ~ %ld ]: %s "
    },
    {   /* ZTLE_ERRCODE_INVALID_PROPERTY_STRING_TYPE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid property value [ %ld ~ %ld ]: %s "
    },
    {   /* ZTLE_ERRCODE_BUFFER_OVERFLOW */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "buffer over flow %s "
    },
    {
        0,
        NULL
    }
};

