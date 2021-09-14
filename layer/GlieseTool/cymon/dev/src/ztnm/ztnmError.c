/*******************************************************************************
 * ztnError.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>

stlErrorRecord gZtnErrorTable[] =
{
    {   /* ZTN_ERRCODE_INVALID_HOME */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "GOLDILOCKS_DATA system environment is invalid"
    },
    {   /* ZTN_ERRCODE_INVALID_FILE_NAME */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "\"%s\": file does not exist"
    },
    {   /* ZTN_ERRCODE_SERVICE_NOT_AVAILABLE */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "Service is not available"
    },
    {   /* ZTN_ERRCODE_CONFIGURE_FILE_PARSE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "configure file processing error"
    },
    {   /* ZTN_ERRCODE_DUPLICATE_GROUP_NAME */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Duplicate group name (%s)"
    },
    {   /* ZTN_ERRCODE_INVALID_VALUE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Value : %s(%ld)"
    },
    {   /* ZTN_ERRCODE_INVALID_GROUP_NAME */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid group name"
    },
    {   /* ZTN_ERRCODE_INVALID_PORT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Port Number (1 ~ 65535)"
    },
    {   /* ZTN_ERRCODE_INVALID_GROUP_COUNT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "There is no group in configure"
    },
    {   /* ZTN_ERRCODE_ALREADY_RUNNING */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cymon is already running"
    },
    {   /* ZTN_ERRCODE_COMMUNICATION_LINK_FAILURE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Communication link failure"
    },
    {   /* ZTN_ERRCODE_PROTOCOL */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Protocol Error Occurred (%s)"
    },
    {   /* ZTN_ERRCODE_ALREADY_CONN */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Already Monitor Connected"
    },
    {   /* ZTN_ERRCODE_NOT_RUNNING */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cymon is not running"
    },
    {   /* ZTN_ERRCODE_INTERNAL_ERROR */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "internal error occurred (%s)"
    },
    {   /* ZTN_ERRCODE_UPDATE_INFO_FAIL */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Fail to update monitoring info."
    },
    {   /* ZTN_ERRCODE_INVALID_CONN_INFO */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "USER_ID or USER_PW does not exist."
    },
    {   /* ZTN_ERRCODE_INVALID_ENCRYPTED_PASSWD */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid encrypt/decrypt key"
    },
    {   /* ZTN_ERRCODE_NO_KEY_PARAMETER */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "no '--key' option"
    },
    {   /* ZTN_ERRCODE_PASSWD_ALREADY_EXISTS */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "password already exists(use one of USER_PW or USER_ENCRYPT_PW)"
    },

    {
        0,
        NULL
    }
};

