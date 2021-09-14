/*******************************************************************************
 * ztcmError.c
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

stlErrorRecord gZtcErrorTable[] =
{
    {   /* ZTC_ERRCODE_SERVICE_NOT_AVAILABLE */
        STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS,
        "Service is not available"
    },
    {   /* ZTC_ERRCODE_INVALID_FILE_NAME */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "\"%s\": file does not exist"
    },
    {   /* ZTC_ERRCODE_CONFIGURE_FILE_PARSE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "configure file processing error"
    },
    {   /* ZTC_ERRCODE_INVALID_TABLE */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "\"%s\": table does not exist"
    },
    {   /* ZTC_ERRCODE_INVALID_SCHEMA */
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "\"%s\": schema does not exist"
    },
    {   /* ZTC_ERRCODE_DUPLICATE_OBJECT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "\"%s.%s\": previously Added. Maybe duplicated"
    },
    {   /* ZTC_ERRCODE_INVALID_PRIMARY_KEY */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "\"%s%s\": table must have a primary key"
    },
    {   /* ZTC_ERRCODE_INTERNAL_ERROR */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "internal error occurred (%s)"
    },
    {   /* ZTC_ERRCODE_INVALID_DATA_TYPE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "invalid Datatype(%d)"
    },
    {   /* ZTC_ERRCODE_SUPPLEMENTAL_LOG */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "\"%s.%s\" table must set supplemental log or primary key."
    },
    {   /* ZTC_ERRCODE_ALREADY_RUNNING */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "group [%s] is already running"
    },
    {   /* ZTC_ERRCODE_NOT_RUNNING */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "cyclone is not running"
    },
    {   /* ZTC_ERRCODE_INVALID_HOME */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "GOLDILOCKS_DATA system environment is invalid"
    },
    {   /* ZTC_ERRCODE_LOG_FILE_REUSED */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "\"%s\" log file reused or invalid. restart cyclone with '--reset' option"
    },
    {   /* ZTC_ERRCODE_FAIL_ANALYZE_LOG */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "fail to analyze flow[%s]"
    },
    {   /* ZTC_ERRCODE_INVALID_PORT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Port Number (1 ~ 65535) [%s]"
    },
    {   /* ZTC_ERRCODE_COMMUNICATION_LINK_FAILURE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Communication link failure"
    },
    {   /* ZTC_ERRCODE_INVALID_COM_DATA */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Protocol value (%s)"
    },
    {   /* ZTC_ERRCODE_MASTER_DISCONNT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Master disconnect abnormally"
    },
    {   /* ZTC_ERRCODE_PROTOCOL */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Protocol Error Occurred (%s)"
    },
    {   /* ZTC_ERRCODE_ALREADY_SLAVE_CONN */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Already Slave Connected"
    },
    {   /* ZTC_ERRCODE_INVALID_SLOT_ID */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Transaction Slot Id"
    },
    {   /* ZTC_ERRCODE_INVALID_SUPPLEMENT_LOG */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Supplemental Log (%d)"
    },
    {   /* ZTC_ERRCODE_INVALID_DDL_OPERATION */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid operation for DDL"
    },
    {   /* ZTC_ERRCODE_INVALID_ANALYZE_VALUE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid value to analyze"
    },
    {   /* ZTC_ERRCODE_INVALID_GROUP_NAME */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid group name"
    },
    {   /* ZTC_ERRCODE_INVALID_MASTER_IP */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Not set master ip"
    },
    {   /* ZTC_ERRCODE_INVALID_CAPTURE_INFO */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid capture information"
    },
    {   /* ZTC_ERRCODE_INVALID_GROUP_COUNT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "There is no group in configure"
    },
    {   /* ZTC_ERRCODE_DUPLICATE_GROUP_NAME */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Duplicate group name (%s)"
    },
    {   /* ZTC_ERRCODE_INVALID_VALUE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Value : %s(%ld)"
    },
    {   /* ZTC_ERRCODE_TOO_LOW_VALUE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Too low value : %s(%ld)"
    },
    {   /* ZTC_ERRCODE_INVALID_HOST_PORT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Host Port(%d)"
    },
    {   /* ZTC_ERRCODE_REDOLOG_READ_TIMEOUT */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Redo Log file Read Timeout (%s)"
    },
    {   /* ZTC_ERRCODE_INVALID_ARCHIVELOG */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Archive Log File (%s)"
    },
    {   /* ZTC_ERRCODE_INVALID_TEMP_FILE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Temporary File (%s)"
    },
    {   /* ZTC_ERRCODE_FAIL_WRITE_FILE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Fail to write file (%s)"
    },
    {   /* ZTC_ERRCODE_FAIL_UPDATE_STATE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Fail to Update State (%s)"
    },
    {   /* ZTC_ERRCODE_FAIL_PROPAGATE_MODE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Fail to set Propagate mode at Session"
    },
    {   /* ZTC_ERRCODE_NO_ACTIVE_LOG_FILE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "There is no active redo log file."
    },
    {   /* ZTC_ERRCODE_NO_RESTART_INFO */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "There is no restart information. Master Database must be started first."
    },
    {   /* ZTC_ERRCODE_FAIL_ADD_TABLE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Can Not Add New Table. Master Database must be started first.(%s.%s)"
    },
    {   /* ZTC_ERRCODE_MUST_CONNECT_DB */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Master Database must be started for the first time."
    },
    {   /* ZTC_ERRCODE_INVALID_META_FILE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Meta File(%s)."
    },
    {   /* ZTC_ERRCODE_MUST_EXIST_LOG_FILE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Redo Log File does not exist(%s)."
    },
    {   /* ZTC_ERRCODE_INVALID_CONN_INFO */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "USER_ID or USER_PW does not exist."
    },
    {   /* ZTC_ERRCODE_SLAVE_NOT_SYNC_MODE */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Slave is not running with --sync option."
    },
    {   /* ZTC_ERRCODE_INVALID_SLAVE_CONN_INFO */ 
        STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS,
        "Invalid Connect Information for sync (%s)"
    },
    {
        0,
        NULL
    }
};

