/*******************************************************************************
 * ztdError.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztdError.c 3636 2012-05-23 07:14:23Z lkh $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>

stlErrorRecord gZtdErrorTable[] =
{
    {   /* ZTD_ERRCODE_SERVICE_NOT_AVAILABLE */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "Service is not available"
    },
    {   /* ZTD_ERRCODE_INVALID_CONTROL_FILE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid control file format"
    },
    {   /* ZTD_ERRCODE_BUFFER_OVERFLOW */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "buffer exceeded overflow area"
    },
    {   /* ZTD_ERRCODE_USE_SAME_FILE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "open same file(%s)"
    },
    {   /* ZTD_ERRCODE_INVALID_HANDLE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "occur invalid handle error"
    },
    {   /* ZTD_ERRCODE_UNACCEPTABLE_VALUE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "occur error while operating SQLGetDiagRec/SQLGetDiagFiled"
    },
    {   /* ZTD_ERRCODE_DELIMITER_QUALIFIER */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "using same character for delimiter and qualifier"
    },
    {   /* ZTD_ERRCODE_MULTI_THREAD_COUNT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid thread unit count.(1~32)"
    },
    {   /* ZTD_ERRCODE_ARRAY_SIZE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid array size.(1~65535)"
    },
    {   /* ZTD_ERRCODE_EXPORT_MULTI_THREAD */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "unavailable multithreading in export mode."
    },
    {   /* ZTD_ERRCODE_COMMUNICATION_LINK_FAILURE */
        STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE,
        "Communication link failure"
    },
    {   /* ZTD_ERRCODE_INVALID_FILE_SIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid file size (minimum size : %d)"
    },
    {   /* ZTD_ERRCODE_TEXT_MAX_FILE_SIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "unavailable file size in text mode."
    },
    {   /* ZTD_ERRCODE_INVALID_COLUMN_TYPE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid column type."
    },
    {   /* ZTD_ERRCODE_INVALID_COLUMN_COUNT */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid column count."
    },
    {   /* ZTD_ERRCODE_INVALID_DATA_SIZE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid data size."
    },
    {   /* ZTD_ERRCODE_INVALID_BINARY_FILE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid header of binary file."
    },
    {
        0,
        NULL
    }
};
