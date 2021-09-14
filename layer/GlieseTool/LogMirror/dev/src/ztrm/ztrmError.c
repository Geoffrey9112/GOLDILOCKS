/*******************************************************************************
 * ztrmError.c
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

stlErrorRecord gZtrErrorTable[] =
{
    {   /* ZTR_ERRCODE_SERVICE_NOT_AVAILABLE */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "Service is not available"
    },
    {   /* ZTR_ERRCODE_PROTOCOL */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Protocol value (%s)"
    },
    {   /* ZTR_ERRCODE_INVALID_FILE_NAME */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "\"%s\": file does not exist"
    },
    {   /* ZTR_ERRCODE_CONFIGURE_FILE_PARSE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "configure file processing error"
    },
    {   /* ZTR_ERRCODE_INVALID_CONTROL_FILE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid Control file"
    },
    {   /* ZTR_ERRCODE_COMMUNICATION_LINK_FAILURE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Communication link failure"
    },
    {   /* ZTR_ERRCODE_ALREADY_RUNNING */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "LogMirror is already running"
    },
    {   /* ZTR_ERRCODE_INVALID_HOME */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "GOLDILOCKS_DATA system environment is invalid"
    },
    {   /* ZTR_ERRCODE_INVALID_SHM_KEY */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "There is no Shared Memory Area for LogMirror"
    },
    {   /* ZTR_ERRCODE_INVALID_VALUE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Value : %s(%s)"
    },
    {   /* ZTR_ERRCODE_INVALID_PORT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid port : (%d)"
    },
    {   /* ZTR_ERRCODE_MASTER_DISCONNT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Master disconnect abnormally"
    },
    {   /* ZTR_ERRCODE_INVALID_LOGFILE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Log File(%s)"
    },
    {   /* ZTR_ERRCODE_INVALID_ARCHIVELOG */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Archive Log File does not exist"
    },
    {   /* ZTR_ERRCODE_ARCHIVELOG_READ_TIMEOUT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Archive Log file Read Timeout (%s)"
    },
    {   /* ZTR_ERRCODE_NOT_RUNNING */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "LogMirror is not running"
    },
    {   /* ZTR_ERRCODE_INVALID_CONN_INFO */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Connection Information does not exist"
    },
    {   /* ZTR_ERRCODE_RECV_FAIL */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Infiniband Receive Fail. (%ld)"
    },
    {   /* ZTR_ERRCODE_SERVICE_FAIL */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "GOLDILOCKS Shutdown or Give-up LogMirror Service."
    },
    {
        0,
        NULL
    }
};

