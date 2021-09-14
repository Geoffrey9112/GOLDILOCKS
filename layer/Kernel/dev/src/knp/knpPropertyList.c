/*******************************************************************************
 * knpPropertyList.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *       시스템에서 사용될 Property 리스트 등록
 *
 ******************************************************************************/

/**
 * @file knpPropertyList.c
 * @brief Kernel Layer Property Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knlRing.h>
#include <knpDef.h>
#include <knpPropertyManager.h>
#include <knpDataType.h>

/**
 * @addtogroup knpProperty
 * @{
 */


/**
 * @brief Property 정보를 생성하고 Default값으로 세팅한다.
 * @param[in] aKnlEnv
 */

/*
typedef struct knpPropertyDesc
{
    stlChar               * mName;                  Property 이름 
    stlChar               * mDesc;                  Property 설명 
    stlChar               * mValue;                 Property Default 값 
    stlUInt32               mFlag;                  Property 속성 
    stlUInt16               mID;                    Property ID - knlPropertyID
    stlUInt16               mGroup;                 Property가 속한 그룹 
    stlUInt32               mDataType;              Property에 저장될 값의 데이터타입 
    stlUInt16               mStartupPhase;          Property를 설정할 수 있는 Phase 
    stlUInt16               mStartupPhaseDomain;    Property를 설정할 수 있는 Phase 범위 
    stlUInt64               mMin;                   Property가 숫자형일 경우 최소값 
    stlUInt64               mMax;                   Property가 숫자형일 경우 최대값 
}knpPropertyDesc;
*/  

const stlChar *const gPropertyGroupList[] =
{
    "TBD",
    "TBD",
    "TBD",
    NULL
};

#define KNP_PROPERTY_MIN_MAX( aPropertyId )     \
    aPropertyId ## _MIN, aPropertyId ## _MAX

knpPropertyDesc gPropertyContentList[] =
{
    {
        "SHARED_MEMORY_STATIC_NAME",
        "Shared Memory Static Name",
        "_STATIC",
        (
            KNP_ATTR_MD_WRITABLE     | 
            KNP_ATTR_MD_SYS_FALSE    | 
            KNP_ATTR_MD_SES_FALSE    | 
            KNP_ATTR_DOMAIN_EXTERNAL | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_SHARED_MEMORY_NAME,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SHARED_MEMORY_NAME )
    },
    {
        "SHARED_MEMORY_STATIC_KEY",
        "Shared Memory Static KEY",
        "542353",
        (
            KNP_ATTR_MD_WRITABLE     | 
            KNP_ATTR_MD_SYS_FALSE    | 
            KNP_ATTR_MD_SES_FALSE    | 
            KNP_ATTR_DOMAIN_EXTERNAL | 
            KNP_ATTR_RANGE_NOCHECK   | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_SHARED_MEMORY_KEY,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SHARED_MEMORY_KEY )
    },
    {
        "SHARED_MEMORY_STATIC_SIZE",
        "Shared Memory Static Size",
        "524288000",        // 500M
        (
            KNP_ATTR_MD_WRITABLE     | 
            KNP_ATTR_MD_SYS_FALSE    | 
            KNP_ATTR_MD_SES_FALSE    | 
            KNP_ATTR_DOMAIN_EXTERNAL | 
            KNP_ATTR_RANGE_CHECK     | 
            KNP_ATTR_UNIT_SIZE
        ), 
        KNL_PROPERTY_SHARED_MEMORY_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SHARED_MEMORY_SIZE )
    },
    {
        "SHARED_MEMORY_ADDRESS",
        "Shared Memory Address",
        "1610612736",                /* 0x60000000 */
        (
            KNP_ATTR_MD_WRITABLE     | 
            KNP_ATTR_MD_SYS_FALSE    | 
            KNP_ATTR_MD_SES_FALSE    | 
            KNP_ATTR_DOMAIN_EXTERNAL | 
            KNP_ATTR_RANGE_NOCHECK   | 
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_SHARED_MEMORY_ADDRESS,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SHARED_MEMORY_ADDRESS )
    },
    {
        "PRIVATE_STATIC_AREA_SIZE",
        "Private(heap) Memory Static Size",
        "104857600",          // 100M
        (
            KNP_ATTR_MD_WRITABLE       | 
            KNP_ATTR_MD_SYS_IMMEDIATE  | 
            KNP_ATTR_MD_SES_TRUE       | 
            KNP_ATTR_DOMAIN_EXTERNAL   | 
            KNP_ATTR_RANGE_CHECK       | 
            KNP_ATTR_UNIT_SIZE
        ), 
        KNL_PROPERTY_PRIVATE_STATIC_AREA_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PRIVATE_STATIC_AREA_SIZE )
    },
    {
        "PROCESS_MAX_COUNT",
        "Process Max Count",
        "128",
        (
            KNP_ATTR_MD_WRITABLE     | 
            KNP_ATTR_MD_SYS_FALSE    | 
            KNP_ATTR_MD_SES_FALSE    | 
            KNP_ATTR_DOMAIN_EXTERNAL | 
            KNP_ATTR_RANGE_CHECK     | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_PROCESS_MAX_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PROCESS_MAX_COUNT )
    },
    {   /* deprecated : port no는 listener config에 있음 */
        "PORT_NO",
        "Port Number",
        "29323",
        (
            KNP_ATTR_MD_WRITABLE     | 
            KNP_ATTR_MD_SYS_FALSE    | 
            KNP_ATTR_MD_SES_FALSE    | 
            KNP_ATTR_DOMAIN_INTERNAL | 
            KNP_ATTR_RANGE_CHECK     | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_PORT_NO,
        Group2,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PORT_NO )
    },
    {
        "CLIENT_MAX_COUNT",
        "Maximum Session Count",
        "128",
        (
            KNP_ATTR_MD_WRITABLE     | 
            KNP_ATTR_MD_SYS_FALSE    | 
            KNP_ATTR_MD_SES_FALSE    | 
            KNP_ATTR_DOMAIN_EXTERNAL | 
            KNP_ATTR_RANGE_CHECK     | 
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CLIENT_MAX_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CLIENT_MAX_COUNT )
    },
    {
        "DATA_STORE_MODE",
        "Data store mode(cds:1,tds:2)",
        "2",
        (
            KNP_ATTR_MD_WRITABLE     |
            KNP_ATTR_MD_SYS_FALSE    |
            KNP_ATTR_MD_SES_FALSE    |
            KNP_ATTR_DOMAIN_EXTERNAL |
            KNP_ATTR_RANGE_CHECK     |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DATA_STORE_MODE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DATA_STORE_MODE )
    },
    {
        "USE_INSERTABLE_PAGE_HINT",
        "hint to find insertable page",
        "YES",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_USE_INSERTABLE_PAGE_HINT,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_USE_INSERTABLE_PAGE_HINT )
    },
    {
        "DEFAULT_INDEX_LOGGING",
        "default logging flag of indexes",
        "NO",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DEFAULT_INDEX_LOGGING,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DEFAULT_INDEX_LOGGING )
    },
    {
        "DEFAULT_TABLE_LOCKING",
        "default locking flag of tables",
        "YES",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DEFAULT_TABLE_LOCKING,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DEFAULT_TABLE_LOCKING )
    },
    {
        "DEFAULT_INITRANS",
        "default initrans value of tables",
        "4",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DEFAULT_INITRANS,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DEFAULT_INITRANS )
    },
    {
        "DEFAULT_MAXTRANS",
        "default maxtrans value of tables",
        "8",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DEFAULT_MAXTRANS,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DEFAULT_MAXTRANS )
    },
    {
        "DEFAULT_PCTFREE",
        "default pctfree value of tables or indexes",
        "10",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DEFAULT_PCTFREE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DEFAULT_PCTFREE )
    },
    {
        "DEFAULT_PCTUSED",
        "default pctused value of tables",
        "60",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DEFAULT_PCTUSED,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DEFAULT_PCTUSED )
    },
    {
        "LOG_BUFFER_SIZE",
        "default log buffer size",
        "10485760",                 /* 10 Mega */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_LOG_BUFFER_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOG_BUFFER_SIZE )
    },
    {
        "PENDING_LOG_BUFFER_COUNT",
        "default pending log buffer count",
        "4",                        /* 4 개 */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_PENDING_LOG_BUFFER_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PENDING_LOG_BUFFER_COUNT )
    },
    {
        "CONTROL_FILE_COUNT",
        "control file count",
        "2",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CONTROL_FILE_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CONTROL_FILE_COUNT )
    },
    {
        "CONTROL_FILE_0",
        "control file #0 name",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "wal" STL_PATH_SEPARATOR "control_0.ctl",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CONTROL_FILE_0,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CONTROL_FILE_0 )
    },
    {
        "CONTROL_FILE_1",
        "control file #1 name",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "wal" STL_PATH_SEPARATOR "control_1.ctl",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CONTROL_FILE_1,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CONTROL_FILE_1 )
    },
    {
        "CONTROL_FILE_2",
        "control file #2 name",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "wal" STL_PATH_SEPARATOR "control_2.ctl",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CONTROL_FILE_2,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CONTROL_FILE_2 )
    },
    {
        "CONTROL_FILE_3",
        "control file #3 name",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "wal" STL_PATH_SEPARATOR "control_3.ctl",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CONTROL_FILE_3,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CONTROL_FILE_3 )
    },
    {
        "CONTROL_FILE_4",
        "control file #4 name",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "wal" STL_PATH_SEPARATOR "control_4.ctl",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CONTROL_FILE_4,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CONTROL_FILE_4 )
    },
    {
        "CONTROL_FILE_5",
        "control file #5 name",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "wal" STL_PATH_SEPARATOR "control_5.ctl",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CONTROL_FILE_5,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CONTROL_FILE_5 )
    },
    {
        "CONTROL_FILE_6",
        "control file #6 name",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "wal" STL_PATH_SEPARATOR "control_6.ctl",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CONTROL_FILE_6,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CONTROL_FILE_6 )
    },
    {
        "CONTROL_FILE_7",
        "control file #7 name",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "wal" STL_PATH_SEPARATOR "control_7.ctl",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CONTROL_FILE_7,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CONTROL_FILE_7 )
    },
    {
        "LOG_BLOCK_SIZE",
        "log block size",
        "512",
        (
            KNP_ATTR_MD_READ_ONLY      |
            KNP_ATTR_MD_SYS_NONE       |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_LOG_BLOCK_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOG_BLOCK_SIZE )
    },
    {
        "LOG_FILE_SIZE",
        "log file size",
        "104857600",                /* 100 Mega */
        (
            KNP_ATTR_MD_READ_ONLY      |
            KNP_ATTR_MD_SYS_NONE       |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_LOG_FILE_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOG_FILE_SIZE )
    },
    {
        "LOG_DIR",
        "default log directory",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "wal",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_LOG_DIR,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOG_DIR )
    },
    {
        "LOG_GROUP_COUNT",
        "initial count of log group",
        "4",
        (
            KNP_ATTR_MD_READ_ONLY      |
            KNP_ATTR_MD_SYS_NONE       |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_NOCHECK     |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_LOG_GROUP_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOG_GROUP_COUNT )
    },
    {
        "TRANSACTION_TABLE_SIZE",
        "transaction table size",
        "1024",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_TRANSACTION_TABLE_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_TRANSACTION_TABLE_SIZE )
    },
    {
        "UNDO_RELATION_COUNT",
        "undo relation count",
        "128",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_UNDO_RELATION_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_UNDO_RELATION_COUNT )
    },
    {
        "MINIMUM_UNDO_PAGE_COUNT",
        "minimum undo page count",
        "16",                     /* 128K */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_MINIMUM_UNDO_PAGE_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_MINIMUM_UNDO_PAGE_COUNT )
    },
    {
        "TRANSACTION_COMMIT_WRITE_MODE",
        "transaction commit write mode(0:no_wait, 1:wait)",
        "0",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_NOCHECK     |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_TRANSACTION_COMMIT_WRITE_MODE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_TRANSACTION_COMMIT_WRITE_MODE )
    },
    {
        "SYSTEM_TABLESPACE_DIR",
        "system tablespace directory",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_SYSTEM_TABLESPACE_DIR,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SYSTEM_TABLESPACE_DIR )
    },
    {
        "SYSTEM_MEMORY_DICT_TABLESPACE_SIZE",
        "default system memory dictionary tablespace size",
        "134217728",
        (
            KNP_ATTR_MD_READ_ONLY      |
            KNP_ATTR_MD_SYS_NONE       |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_SYSTEM_MEMORY_DICT_TABLESPACE_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SYSTEM_MEMORY_DICT_TABLESPACE_SIZE )
    },
    {
        "SYSTEM_MEMORY_UNDO_TABLESPACE_SIZE",
        "default system memory undo tablespace size",
        "33554432",
        (
            KNP_ATTR_MD_READ_ONLY      |
            KNP_ATTR_MD_SYS_NONE       |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_SYSTEM_MEMORY_UNDO_TABLESPACE_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SYSTEM_MEMORY_UNDO_TABLESPACE_SIZE )
    },
    {
        "SYSTEM_MEMORY_DATA_TABLESPACE_SIZE",
        "default system memory data tablespace size",
        "209715200",                      /* 200 Mega */
        (
            KNP_ATTR_MD_READ_ONLY      |
            KNP_ATTR_MD_SYS_NONE       |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_SYSTEM_MEMORY_DATA_TABLESPACE_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SYSTEM_MEMORY_DATA_TABLESPACE_SIZE )
    },
    {
        "SYSTEM_MEMORY_TEMP_TABLESPACE_SIZE",
        "default system memory temporary tablespace size",
        "209715200",                      /* 200 Mega */
        (
            KNP_ATTR_MD_READ_ONLY      |
            KNP_ATTR_MD_SYS_NONE       |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_SYSTEM_MEMORY_TEMP_TABLESPACE_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SYSTEM_MEMORY_TEMP_TABLESPACE_SIZE )
    },
    {
        "USER_MEMORY_DATA_TABLESPACE_SIZE",
        "default user memory data tablespace size",
        "33554432",
        (
            KNP_ATTR_MD_READ_ONLY      |
            KNP_ATTR_MD_SYS_NONE       |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_USER_MEMORY_DATA_TABLESPACE_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_USER_MEMORY_DATA_TABLESPACE_SIZE )
    },
    {
        "USER_MEMORY_TEMP_TABLESPACE_SIZE",
        "default user memory temporary tablespace size",
        "33554432",
        (
            KNP_ATTR_MD_READ_ONLY      |
            KNP_ATTR_MD_SYS_NONE       |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_USER_MEMORY_TEMP_TABLESPACE_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_USER_MEMORY_TEMP_TABLESPACE_SIZE )
    },
    {
        "SYSTEM_LOGGER_DIR",
        "system logger directory",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "trc",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_SYSTEM_LOGGER_DIR,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SYSTEM_LOGGER_DIR )
    },
    {
        "ARCHIVELOG_MODE",
        "archive log mode(0:NOARCHIVELOG mode, 1:ARCHIVELOG mode)",
        "0",
        (
            KNP_ATTR_MD_READ_ONLY      |
            KNP_ATTR_MD_SYS_NONE       |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_MODE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_MODE )
    },
    {
        /* deprecated : archive log direcotry 다중화 */
        "ARCHIVELOG_DIR",
        "default archive log directory",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "archive_log",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_DIR,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_DIR )
    },
    {
        "ARCHIVELOG_FILE",
        "default archive log file",
        "archive",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_FILE,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_FILE )
    },
    {
        "PARALLEL_LOAD_FACTOR",           /* 이름  */
        "parallel load factor",           /* 설명  */
        "1",                              /* 기본값 */
        (
            KNP_ATTR_MD_WRITABLE       |  /* 쓰기 가능 */
            KNP_ATTR_MD_SYS_IMMEDIATE  |  /* ALTER SYSTEM */
            KNP_ATTR_MD_SES_FALSE      |  /* ALTER SESSION */
            KNP_ATTR_DOMAIN_EXTERNAL   |  /* 공개 */
            KNP_ATTR_RANGE_CHECK       |  /* 범위 검사 */
            KNP_ATTR_UNIT_NONE            /* Size, Time */
        ),
        KNL_PROPERTY_PARALLEL_LOAD_FACTOR,        /* 식별자 */
        Group1,                                   /* 그룹 */
        KNL_PROPERTY_TYPE_BIGINT,                 /* Data Type */
        KNL_STARTUP_PHASE_NO_MOUNT,               /* Startup Phase */
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,  /* 이전, 이후 */
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_LOAD_FACTOR )
    },
    {
        "PARALLEL_IO_FACTOR",             /* 이름  */
        "parallel load factor",           /* 설명  */
        "1",                              /* 기본값 */
        (
            KNP_ATTR_MD_WRITABLE       |  /* 쓰기 가능 */
            KNP_ATTR_MD_SYS_DEFERRED   |  /* ALTER SYSTEM */
            KNP_ATTR_MD_DB_FALSE       |  /* ALTER DATABASE */
            KNP_ATTR_DOMAIN_EXTERNAL   |  /* 공개 */
            KNP_ATTR_RANGE_CHECK       |  /* 범위 검사 */
            KNP_ATTR_UNIT_NONE            /* Size, Time */
        ),
        KNL_PROPERTY_PARALLEL_IO_FACTOR,          /* 식별자 */
        Group1,                                   /* 그룹 */
        KNL_PROPERTY_TYPE_BIGINT,                 /* Data Type */
        KNL_STARTUP_PHASE_NO_MOUNT,        /* Startup Phase */
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,  /* 이전, 이후 */
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_FACTOR )
    },
    {
        "PARALLEL_IO_GROUP_1",
        "parallel load group #1",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_1,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_1 )
    },
    {
        "PARALLEL_IO_GROUP_2",
        "parallel load group #2",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_2,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_2 )
    },
    {
        "PARALLEL_IO_GROUP_3",
        "parallel load group #3",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_3,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_3 )
    },
    {
        "PARALLEL_IO_GROUP_4",
        "parallel load group #4",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_4,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_4 )
    },
    {
        "PARALLEL_IO_GROUP_5",
        "parallel load group #5",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_5,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_5 )
    },
    {
        "PARALLEL_IO_GROUP_6",
        "parallel load group #6",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_6,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_6 )
    },
    {
        "PARALLEL_IO_GROUP_7",
        "parallel load group #7",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_7,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_7 )
    },
    {
        "PARALLEL_IO_GROUP_8",
        "parallel load group #8",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_8,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_8 )
    },
    {
        "PARALLEL_IO_GROUP_9",
        "parallel load group #9",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_9,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_9 )
    },
    {
        "PARALLEL_IO_GROUP_10",
        "parallel load group #10",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_10,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_10 )
    },
    {
        "PARALLEL_IO_GROUP_11",
        "parallel load group #11",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_11,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_11 )
    },
    {
        "PARALLEL_IO_GROUP_12",
        "parallel load group #12",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_12,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_12 )
    },
    {
        "PARALLEL_IO_GROUP_13",
        "parallel load group #13",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_13,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_13 )
    },
    {
        "PARALLEL_IO_GROUP_14",
        "parallel load group #14",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_14,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_14 )
    },
    {
        "PARALLEL_IO_GROUP_15",
        "parallel load group #15",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_15,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_15 )
    },
    {
        "PARALLEL_IO_GROUP_16",
        "parallel load group #16",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "db",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PARALLEL_IO_GROUP_16,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PARALLEL_IO_GROUP_16 )
    },
    {
        "BLOCK_READ_COUNT",               /* 이름  */
        "value count for a block read",   /* 설명  */
        "20",                             /* 기본값 */
        (
            KNP_ATTR_MD_WRITABLE       |  /* 쓰기 가능 */
            KNP_ATTR_MD_SYS_DEFERRED   |  /* ALTER SYSTEM */
            KNP_ATTR_MD_DB_FALSE       |  /* ALTER DATABASE */
            KNP_ATTR_MD_SES_TRUE       |  /* ALTER SESSION */
            KNP_ATTR_DOMAIN_EXTERNAL   |  /* 공개 */
            KNP_ATTR_RANGE_CHECK       |  /* 범위 검사 */
            KNP_ATTR_UNIT_NONE            /* Size, Time */
        ),
        KNL_PROPERTY_BLOCK_READ_COUNT,            /* 식별자 */
        Group1,                                   /* 그룹 */
        KNL_PROPERTY_TYPE_BIGINT,                 /* Data Type */
        KNL_STARTUP_PHASE_NO_MOUNT,               /* Startup Phase */
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,  /* 이전, 이후 */
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BLOCK_READ_COUNT )
    },
    {
        /**
         * @todo
         * 임시로 NLS_DATE_FORMAT 의 ALTER SYSTEM 속성을 KNP_ATTR_MD_SYS_DEFERRED로 해놓았습니다.
         * TEST용으로 사용되고 있으므로 속성을 변경하면 Property Testcase에서 DIFF가 발생하며, 그냥 Overwrite하시면 안됩니다.
         * 나중에 해당 속성을 갖는 Property가 나올 경우에 변경할 예정입니다.
         */
        "NLS_DATE_FORMAT",                         /* 이름  */
        "nls date format",                         /* 설명  */
        "YYYY-MM-DD",                              /* 기본값 */  
        (                                          
            KNP_ATTR_MD_WRITABLE       |           /* 쓰기 가능 */
            KNP_ATTR_MD_DB_FALSE       |           /* ALTER DATABASE */
            KNP_ATTR_MD_SYS_DEFERRED   |           /* ALTER SYSTEM */
            KNP_ATTR_MD_SES_TRUE       |           /* ALTER SESSION */
            KNP_ATTR_DOMAIN_EXTERNAL   |           /* 공개 */
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_NLS_DATE_FORMAT,              /* 식별자 */
        Group1,                                    /* 그룹 */ 
        KNL_PROPERTY_TYPE_VARCHAR,                 /* Data Type */
        KNL_STARTUP_PHASE_NO_MOUNT,                /* Startup Phase */
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,   /* 이전, 이후 */
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_NLS_DATE_FORMAT )
    },
    {
        "NLS_TIME_FORMAT",                         /* 이름  */
        "nls time format",                         /* 설명  */
        "HH24:MI:SS.FF6",                          /* 기본값 */  
        (                                          
            KNP_ATTR_MD_WRITABLE       |           /* 쓰기 가능 */
            KNP_ATTR_MD_DB_FALSE       |           /* ALTER DATABASE */
            KNP_ATTR_MD_SYS_FALSE      |           /* ALTER SYSTEM */
            KNP_ATTR_MD_SES_TRUE       |           /* ALTER SESSION */
            KNP_ATTR_DOMAIN_EXTERNAL   |           /* 공개 */
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_NLS_TIME_FORMAT,          /* 식별자 */
        Group1,                                    /* 그룹 */ 
        KNL_PROPERTY_TYPE_VARCHAR,                 /* Data Type */
        KNL_STARTUP_PHASE_NO_MOUNT,                /* Startup Phase */
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,   /* 이전, 이후 */
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_NLS_TIME_FORMAT )
    },
    {
        "NLS_TIME_WITH_TIME_ZONE_FORMAT",          /* 이름  */
        "nls time with time zone format",          /* 설명  */
        "HH24:MI:SS.FF6 TZH:TZM",                  /* 기본값 */  
        (                                          
            KNP_ATTR_MD_WRITABLE       |           /* 쓰기 가능 */
            KNP_ATTR_MD_DB_FALSE       |           /* ALTER DATABASE */
            KNP_ATTR_MD_SYS_FALSE      |           /* ALTER SYSTEM */
            KNP_ATTR_MD_SES_TRUE       |           /* ALTER SESSION */
            KNP_ATTR_DOMAIN_EXTERNAL   |           /* 공개 */
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_NLS_TIME_WITH_TIME_ZONE_FORMAT, /* 식별자 */
        Group1,                                    /* 그룹 */ 
        KNL_PROPERTY_TYPE_VARCHAR,                 /* Data Type */
        KNL_STARTUP_PHASE_NO_MOUNT,                /* Startup Phase */
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,   /* 이전, 이후 */
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_NLS_TIME_WITH_TIME_ZONE_FORMAT )
    },
    {
        "NLS_TIMESTAMP_FORMAT",                    /* 이름  */
        "nls timestamp format",                    /* 설명  */
        "YYYY-MM-DD HH24:MI:SS.FF6",               /* 기본값 */  
        (                                          
            KNP_ATTR_MD_WRITABLE       |           /* 쓰기 가능 */
            KNP_ATTR_MD_DB_FALSE       |           /* ALTER DATABASE */
            KNP_ATTR_MD_SYS_FALSE      |           /* ALTER SYSTEM */
            KNP_ATTR_MD_SES_TRUE       |           /* ALTER SESSION */
            KNP_ATTR_DOMAIN_EXTERNAL   |           /* 공개 */
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_NLS_TIMESTAMP_FORMAT,         /* 식별자 */
        Group1,                                    /* 그룹 */ 
        KNL_PROPERTY_TYPE_VARCHAR,                 /* Data Type */
        KNL_STARTUP_PHASE_NO_MOUNT,                /* Startup Phase */
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,   /* 이전, 이후 */
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_NLS_TIMESTAMP_FORMAT )
    },
    {
        "NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT",     /* 이름  */
        "nls timestamp with time zone format",     /* 설명  */
        "YYYY-MM-DD HH24:MI:SS.FF6 TZH:TZM",       /* 기본값 */  
        (                                          
            KNP_ATTR_MD_WRITABLE       |           /* 쓰기 가능 */
            KNP_ATTR_MD_DB_FALSE       |           /* ALTER DATABASE */
            KNP_ATTR_MD_SYS_FALSE      |           /* ALTER SYSTEM */
            KNP_ATTR_MD_SES_TRUE       |           /* ALTER SESSION */
            KNP_ATTR_DOMAIN_EXTERNAL   |           /* 공개 */
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT, /* 식별자 */
        Group1,                                    /* 그룹 */ 
        KNL_PROPERTY_TYPE_VARCHAR,                 /* Data Type */
        KNL_STARTUP_PHASE_NO_MOUNT,                /* Startup Phase */
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,   /* 이전, 이후 */
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT )
    },        
    {
        "DDL_LOCK_TIMEOUT",
        "a time limit (sec) for how long DDL statemets will wait",
        "0",     /* NOWAIT */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DDL_LOCK_TIMEOUT,
        Group1,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DDL_LOCK_TIMEOUT )
    },
    {
        "SHOW_CALLSTACK",
        "show error callstack",
        "NO",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_SHOW_CALLSTACK,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SHOW_CALLSTACK )
    },
    {
        "AGING_INTERVAL",
        "aging interval time(ms)",
        "10",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_AGING_INTERVAL,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_AGING_INTERVAL )
    },
    {
        "MAXIMUM_FLUSH_LOG_BLOCK_COUNT",
        "maximum number of log block count to be flushing",
        "100000",   /* (100000 * 512 Bytes) == 50 MBytes */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_MAXIMUM_FLUSH_LOG_BLOCK_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_MAXIMUM_FLUSH_LOG_BLOCK_COUNT )
    },
    {
        "MAXIMUM_FLUSH_PAGE_COUNT",
        "maximum number of page count to be flushing",
        "1024",   /* (1024 * 8192 Bytes) == 8 MBytes */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_MAXIMUM_FLUSH_PAGE_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_MAXIMUM_FLUSH_PAGE_COUNT )
    },
    {   /* deprecated */
        "TIMER_INTERVAL",
        "timer interval time(ms)",
        "10",   /* 10 ms */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_TIMER_INTERVAL,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_TIMER_INTERVAL )
    },
    {
        "QUERY_TIMEOUT",
        "query timeout(s)",
        "0",   /* infinite */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_QUERY_TIMEOUT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_QUERY_TIMEOUT )
    },
    {
        "IDLE_TIMEOUT",
        "idle timeout(s)",
        "0",   /* infinite */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_IDLE_TIMEOUT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_IDLE_TIMEOUT )
    },
    {
        "SPIN_COUNT",
        "retry the operation a number of times before giving up",
        "1000",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_SPIN_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SPIN_COUNT )
    },
    {
        "BUSY_WAIT_COUNT",
        "maximum count of busy waiting",
        "50",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BUSY_WAIT_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BUSY_WAIT_COUNT )
    },
    {
        "DATABASE_FILE_IO",
        "i/o type for database file(0: direct io, 1: buffered io)",
        "0",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DATABASE_FILE_IO,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DATABASE_FILE_IO )
    },
    {
        "LOG_FILE_IO",
        "i/o type for redo log file(0: direct io, 1: buffered io)",
        "0",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_LOG_FILE_IO,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOG_FILE_IO )
    },
    {
        "SYSTEM_FILE_IO",                                            /* controlfile io type */
        "i/o type for system file(0: direct io, 1: buffered io)",
        "0",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_SYSTEM_FILE_IO,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SYSTEM_FILE_IO )
    },
    {
        "CACHE_ALIGNED_SIZE",
        "cache aligned size",
        "64",
        (
            KNP_ATTR_MD_WRITABLE     | 
            KNP_ATTR_MD_SYS_FALSE    | 
            KNP_ATTR_MD_SES_FALSE    | 
            KNP_ATTR_DOMAIN_INTERNAL | 
            KNP_ATTR_RANGE_CHECK     | 
            KNP_ATTR_UNIT_SIZE
        ), 
        KNL_PROPERTY_CACHE_ALIGNED_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CACHE_ALIGNED_SIZE )
    },
    {
        "TRACE_DDL",
        "write trace messages for DDL",
        "NO",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_FALSE     | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_TRACE_DDL,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_TRACE_DDL )
    },
    {
        "SUPPLEMENTAL_LOG_DATA_PRIMARY_KEY",
        "supplemental log data of primary key columns be logged in redo log files",
        "NO",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_FALSE     | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_SUPPLEMENTAL_LOG_DATA_PRIMARY_KEY,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SUPPLEMENTAL_LOG_DATA_PRIMARY_KEY )
    },
    {
        "INDEX_BUILD_DIRECTION",
        "index build direction ( 0: top-down,  1: bottom-up )",
        "1",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_TRUE      | 
            KNP_ATTR_DOMAIN_INTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_INDEX_BUILD_DIRECTION,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INDEX_BUILD_DIRECTION )
    },
    {
        "INDEX_BUILD_PARALLEL_FACTOR",
        "index build parallel factor",
        "0",                       /* depend on number of core */
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_TRUE      | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_INDEX_BUILD_PARALLEL_FACTOR,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INDEX_BUILD_PARALLEL_FACTOR )
    },
    {
        "MEMORY_MERGE_RUN_COUNT",
        "merge run count for memory index",
        "32",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_TRUE      | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_MEMORY_MERGE_RUN_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_MEMORY_MERGE_RUN_COUNT )
    },
    {
        "MEMORY_SORT_RUN_SIZE",
        "sort run size for memory index",
        "8192",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_TRUE      | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_SIZE
        ), 
        KNL_PROPERTY_MEMORY_SORT_RUN_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_MEMORY_SORT_RUN_SIZE )
    },
    {
        "DATABASE_ACCESS_MODE",
        "database access mode ( 0: read only, 1: read write )",
        "1",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_FALSE     | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_DATABASE_ACCESS_MODE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DATABASE_ACCESS_MODE )
    },
    {
        "INST_TABLE_INST_MEM_MAX",
        "instant mode mem max ( unit: extent count )",
        "4",
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_TRUE      |
            KNP_ATTR_DOMAIN_INTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_INST_TABLE_INST_MEM_MAX,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INST_TABLE_INST_MEM_MAX )
    },
    {
        "INST_TABLE_SORT_RUN_COUNT",
        "max sort run count for internal merge",
        "32",
        (   
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_TRUE      |
            KNP_ATTR_DOMAIN_INTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_INST_TABLE_SORT_RUN_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INST_TABLE_SORT_RUN_COUNT )
    },
    {
        "INST_TABLE_FIXED_COL_MAX_LEN",
        "max length for fixed column of instant table ( the larger column becomes variable type )",
        "64",
        (   
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_TRUE      |
            KNP_ATTR_DOMAIN_INTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_INST_TABLE_FIXED_COL_MAX_LEN,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INST_TABLE_FIXED_COL_MAX_LEN )
    },
    {
        "INST_TABLE_VAR_COL_PIECE_MIN_LEN",
        "minimum length of variable column piece of instant table",
        "32", /* 이 길이보다 작은 공간이 남아 있으면 그 block은 버린다. */
        (   
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_TRUE      |
            KNP_ATTR_DOMAIN_INTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_INST_TABLE_VAR_COL_PIECE_MIN_LEN,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INST_TABLE_VAR_COL_PIECE_MIN_LEN )
    },
    {
        "INST_TABLE_FIXED_ROW_MAX_LEN",
        "instant table max fixed row length",
        "1000",
        (   
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_TRUE      |
            KNP_ATTR_DOMAIN_INTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_INST_TABLE_FIXED_ROW_MAX_LEN,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INST_TABLE_FIXED_ROW_MAX_LEN )
    },
    {
        "INST_TABLE_INDEX_NODE_COUNT_PER_BLOCK",
        "instant table index node count per block",
        "64",  /* 256K / 64 = 4K */
        (   
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_TRUE      |
            KNP_ATTR_DOMAIN_INTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_INST_TABLE_NODE_COUNT_PER_BLOCK,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INST_TABLE_NODE_COUNT_PER_BLOCK )
    },
    {
        "DEVELOPMENT_OPTION", /* 이 프로퍼티는 런타임시 디버깅 및 테스트를 위해 분기를 컨트롤하기 위한 개발자용 프로퍼티임 */
        "development option for run time debugging",
        "0",
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_TRUE      |
            KNP_ATTR_DOMAIN_INTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DEVELOPMENT_OPTION,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DEVELOPMENT_OPTION )
    },
    {
        "INDEX_ROOT_MIRRORING_DEPTH",
        "index node depth that can be mirrored",
        "3",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_INDEX_ROOT_MIRRORING_DEPTH,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INDEX_ROOT_MIRRORING_DEPTH )
    },
    {
        "INDEX_ROOT_MAXIMIZE_FANOUT",
        "maximize index root node",
        "YES",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_INDEX_ROOT_MAXIMIZE_FANOUT,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INDEX_ROOT_MAXIMIZE_FANOUT )
    },
    {
        "USE_LARGE_PAGES",
        "use large pages",
        "NO",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_USE_LARGE_PAGES,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_USE_LARGE_PAGES )
    },
    {
        "LOG_SYNC_INTERVAL",
        "interval for synchronize log",
        "3",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_LOG_SYNC_INTERVAL,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOG_SYNC_INTERVAL )
    },
    {
        "INST_TABLE_SORT_METHOD",
        "instant table sort method",
        "1",
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_TRUE      |
            KNP_ATTR_DOMAIN_INTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_INST_TABLE_SORT_METHOD,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INST_TABLE_SORT_METHOD )
    },
    {
        "LOCK_HASH_TABLE_SIZE",
        "lock manager hash table size",
        "65519",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_LOCK_HASH_TABLE_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOCK_HASH_TABLE_SIZE )
    },
    {
        "INST_TABLE_MAX_KEY_LEN_FOR_LSD_RADIX",
        "instant table max key length for LSD radix sort",
        "20",  /* sort table의 key row가 이 길이 이하이면 LSD radix 방식을 사용한다. */
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_TRUE      |
            KNP_ATTR_DOMAIN_INTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_INST_TABLE_MAX_KEY_LEN_FOR_LSD_RADIX,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_INST_TABLE_MAX_KEY_LEN_FOR_LSD_RADIX )
    },
    {
        "SESSION_PROPAGATE_REDO_LOG",
        "propagation redo-log for CDC ",
        "YES",
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_DEFERRED  |
            KNP_ATTR_MD_SES_TRUE      |
            KNP_ATTR_DOMAIN_INTERNAL  |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PROPAGATE_REDO_LOG,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PROPAGATE_REDO_LOG )
    },
    {
        "MAXIMUM_CONCURRENT_ACTIVITIES",
        "maximum number of active statements that the driver can support for a connection",
        "1024",
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_DEFERRED  |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_MAXIMUM_CONCURRENT_ACTIVITIES,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_MAXIMUM_CONCURRENT_ACTIVITIES )
    },
    {
        "NET_BUFFER_SIZE",
        "TCP network buffer size",
        "32768",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_NET_BUFFER_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_NET_BUFFER_SIZE )
    },
    {
        "PROCESS_LOGGER_DIR",
        "process trace logger directory",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "trc",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PROCESS_LOGGER_DIR,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PROCESS_LOGGER_DIR )
    },
    {
        "TRACE_LOG_ID",
        "trace log ID",
        "0",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_TRUE      | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_TRACE_LOG_ID,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_TRACE_LOG_ID )
    },
    {
        "LOGGER_FILE_ID",
        "trace logger file identifier",
        "0",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_TRUE      | 
            KNP_ATTR_DOMAIN_INTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_LOGGER_FILE_ID,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOGGER_FILE_ID )
    },
    {
        "GENERATE_CORE_DUMP",
        "generate core dump",
#if defined( STL_DEBUG )
        "1",
#else
        "0",
#endif    
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_FALSE     | 
            KNP_ATTR_DOMAIN_INTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_GENERATE_CORE_DUMP,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_GENERATE_CORE_DUMP )
    },
    {
        "SESSION_FATAL_BEHAVIOR",
        "session fatal behavior ( 0: kill thread, 1: kill process )",
        "0",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_FALSE     | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_SESSION_FATAL_BEHAVIOR,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SESSION_FATAL_BEHAVIOR )
    },
    {
        "REFINE_RELATION",
        "refine aged relations",
        "YES",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_FALSE     | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_REFINE_RELATION,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_REFINE_RELATION )
    },
    {
        "MEDIA_RECOVERY_LOG_BUFFER_SIZE",
        "default log buffer size for media recovery",
        "10485760",                 /* 10M */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_MEDIA_RECOVERY_LOG_BUFFER_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_MEDIA_RECOVERY_LOG_BUFFER_SIZE )
    },
    {
        "NUMA",
        "enable numa",
        "0",                     /* DISABLED */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_NUMA,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_NUMA )
    },
    {
        "NUMA_MAP",
        "numa node map",
        "x",                   /* no binding */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_NUMA_MAP,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_NUMA_MAP )
    },
    {
        "IN_DOUBT_DECISION",
        "decision for in-doubt transaction",
        "2",                 /* commit */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_IN_DOUBT_DECISION,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_IN_DOUBT_DECISION )
    },
    {
        "TRACE_XA",
        "logging trace log for xa interfaces",
        "0",                 /* off */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_TRACE_XA,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_TRACE_XA )
    },
    {
        /* deprecated : backup directory 다중화 */
        "BACKUP_DIR",
        "default backup directory",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BACKUP_DIR,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BACKUP_DIR )
    },
    {
        "CONTROL_FILE_TEMP_NAME",
        "temporary file name for control file",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup" STL_PATH_SEPARATOR "control.tmp",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CONTROL_FILE_TEMP_NAME,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CONTROL_FILE_TEMP_NAME )
    },
    {
        "DEFAULT_REMOVAL_OBSOLETE_BACKUP_LIST",
        "default removal flag of obsolete incremental backup lists",
        "NO",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DEFAULT_REMOVAL_OBSOLETE_BACKUP_LIST,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DEFAULT_REMOVAL_OBSOLETE_BACKUP_LIST )
    },
    {
        "DEFAULT_REMOVAL_BACKUP_FILE",
        "default removal flag of incremental backup files",
        "NO",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DEFAULT_REMOVAL_BACKUP_FILE,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DEFAULT_REMOVAL_BACKUP_FILE )
    },
    {
        "PLAN_CACHE",
        "caching sql plan",
        "YES",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PLAN_CACHE,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PLAN_CACHE )
    },
    {
        "PLAN_CACHE_SIZE",
        "sql plan cache size",
        "104857600",                 /* 100M */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_PLAN_CACHE_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PLAN_CACHE_SIZE )
    },
    {
        "PLAN_CLOCK_COUNT",
        "count of plan clock",
        "8",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_FALSE      |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PLAN_CLOCK_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PLAN_CLOCK_COUNT )
    },
    {
        "LOG_MIRROR_MODE",
        "log Mirror mode(0:disable, 1:enable)",
        "0",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_LOG_MIRROR_MODE,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOG_MIRROR_MODE )
    },
    {
        "LOG_MIRROR_SHARED_MEMORY_STATIC_SIZE",
        "Shared Memory Static Size for Log Mirror",
        "104857600",               /* 100 Mega */
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_FALSE     | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_SIZE
        ), 
        KNL_PROPERTY_LOG_MIRROR_SHARED_MEMORY_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOG_MIRROR_SHARED_MEMORY_SIZE )
    },
    {
        "PAGE_CHECKSUM_TYPE",
        "page checksum type(0:LSN, 1:CRC)",
        "0",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_PAGE_CHECKSUM_TYPE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_PAGE_CHECKSUM_TYPE )
    },
    {
        "HINT_ERROR",
        "enable hint error",
        "NO",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_HINT_ERROR,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_HINT_ERROR )
    },
    {
        "DATABASE_TEST_OPTION",
        "database test mode ( 0:no option, 1:block alter system ), 2:block tablespace",
        "0",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DATABASE_TEST_OPTION,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DATABASE_TEST_OPTION )
    },
    {
        "MAXIMUM_NAMED_CURSOR_COUNT",
        "maximum number of named cursor that the driver can support for a connection",
        "128",
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_DEFERRED  |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_MAXIMUM_NAMED_CURSOR_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_MAXIMUM_NAMED_CURSOR_COUNT )
    },
    {
        "SNAPSHOT_STATEMENT_TIMEOUT",
        "snapshot statement timeout ( sec )",
        "22118400",            /* 1 year */
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_SNAPSHOT_STATEMENT_TIMEOUT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SNAPSHOT_STATEMENT_TIMEOUT )
    },
    {
        "TIMEZONE",
        "timezone",
        "+09:00",
        (
            KNP_ATTR_MD_READ_ONLY     |
            KNP_ATTR_MD_SYS_NONE      |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_TIMEZONE,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_TIMEZONE )
    },
    {
        "CHARACTER_SET",
        "character set",
        "UTF8",
        (
            KNP_ATTR_MD_READ_ONLY     |
            KNP_ATTR_MD_SYS_NONE      |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CHARACTER_SET,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CHARACTER_SET )
    },
    {
        "CHAR_LENGTH_UNITS",
        "char length units",
        "OCTETS",
        (
            KNP_ATTR_MD_READ_ONLY     |
            KNP_ATTR_MD_SYS_NONE      |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_CHAR_LENGTH_UNITS,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_CHAR_LENGTH_UNITS )
    },
    {
        "DDL_AUTOCOMMIT",
        "DDL autocommit",
        "NO",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DDL_AUTOCOMMIT,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DDL_AUTOCOMMIT )
    },
    {
        "UNDO_SHRINK_THRESHOLD",
        "threshold bytes to attempt to shrink undo segment ( byte )",
        "10485760",               /* 10M byte */
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_UNDO_SHRINK_THRESHOLD,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_UNDO_SHRINK_THRESHOLD )
    },
    {
        "READABLE_ARCHIVELOG_DIR_COUNT",
        "readable archive log directory count",
        "1",
        (
            KNP_ATTR_MD_WRITABLE       | 
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   | 
            KNP_ATTR_RANGE_CHECK       | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_READABLE_ARCHIVELOG_DIR_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_READABLE_ARCHIVELOG_DIR_COUNT )
    },
    {
        "ARCHIVELOG_DIR_1",
        "archive log directory #1 - default",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "archive_log",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_DIR_1,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_DIR_1 )
    },
    {
        "ARCHIVELOG_DIR_2",
        "archive log directory #2",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "archive_log",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_DIR_2,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_DIR_2 )
    },
    {
        "ARCHIVELOG_DIR_3",
        "archive log directory #3",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "archive_log",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_DIR_3,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_DIR_3 )
    },
    {
        "ARCHIVELOG_DIR_4",
        "archive log directory #4",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "archive_log",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_DIR_4,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_DIR_4 )
    },
    {
        "ARCHIVELOG_DIR_5",
        "archive log directory #5",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "archive_log",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_DIR_5,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_DIR_5 )
    },
    {
        "ARCHIVELOG_DIR_6",
        "archive log directory #6",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "archive_log",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_DIR_6,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_DIR_6 )
    },
    {
        "ARCHIVELOG_DIR_7",
        "archive log directory #7",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "archive_log",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_DIR_7,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_DIR_7 )
    },
    {
        "ARCHIVELOG_DIR_8",
        "archive log directory #8",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "archive_log",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_DIR_8,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_DIR_8 )
    },
    {
        "ARCHIVELOG_DIR_9",
        "archive log directory #9",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "archive_log",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_DIR_9,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_DIR_9 )
    },
    {
        "ARCHIVELOG_DIR_10",
        "archive log directory #10",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "archive_log",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_ARCHIVELOG_DIR_10,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_ARCHIVELOG_DIR_10 )
    },
    {
        "READABLE_BACKUP_DIR_COUNT",
        "readable incremental backup directory count",
        "1",
        (
            KNP_ATTR_MD_WRITABLE       | 
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       | 
            KNP_ATTR_DOMAIN_EXTERNAL   | 
            KNP_ATTR_RANGE_CHECK       | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_READABLE_BACKUP_DIR_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_READABLE_BACKUP_DIR_COUNT )
    },
    {
        "BACKUP_DIR_1",
        "backup directory #1 - default",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BACKUP_DIR_1,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BACKUP_DIR_1 )
    },
    {
        "BACKUP_DIR_2",
        "backup directory #2",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BACKUP_DIR_2,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BACKUP_DIR_2 )
    },
    {
        "BACKUP_DIR_3",
        "backup directory #3",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BACKUP_DIR_3,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BACKUP_DIR_3 )
    },
    {
        "BACKUP_DIR_4",
        "backup directory #4",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BACKUP_DIR_4,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BACKUP_DIR_4 )
    },
    {
        "BACKUP_DIR_5",
        "backup directory #5",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BACKUP_DIR_5,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BACKUP_DIR_5 )
    },
    {
        "BACKUP_DIR_6",
        "backup directory #6",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BACKUP_DIR_6,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BACKUP_DIR_6 )
    },
    {
        "BACKUP_DIR_7",
        "backup directory #7",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BACKUP_DIR_7,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BACKUP_DIR_7 )
    },
    {
        "BACKUP_DIR_8",
        "backup directory #8",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BACKUP_DIR_8,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BACKUP_DIR_8 )
    },
    {
        "BACKUP_DIR_9",
        "backup directory #9",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BACKUP_DIR_9,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BACKUP_DIR_9 )
    },
    {
        "BACKUP_DIR_10",
        "backup directory #10",
        "<GOLDILOCKS_DATA>" STL_PATH_SEPARATOR "backup",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_DEFERRED   |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BACKUP_DIR_10,
        Group3,
        KNL_PROPERTY_TYPE_VARCHAR,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BACKUP_DIR_10 )
    },
    {
        "TRACE_LOG_TIME_DETAIL",
        "detail trace log time",
        "0",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_TRUE      | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_TRACE_LOG_TIME_DETAIL,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_TRACE_LOG_TIME_DETAIL )
    },
    {
        "LOG_MIRROR_TIMEOUT",
        "logmirror retry timeout(sec)",
        "0",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_FALSE      |
            KNP_ATTR_DOMAIN_EXTERNAL   |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_LOG_MIRROR_TIMEOUT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_LOG_MIRROR_TIMEOUT )
    },
    {
        "DISPATCHER_CM_BUFFER_SIZE",
        "communication buffer size",    /* shared 모드에서 사용하는 shared memory 크기 */
        "31457280",                     /* 30MB */
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_FALSE     |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_DISPATCHER_CM_BUFFER_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DISPATCHER_CM_BUFFER_SIZE )
    },
    {
        "DISPATCHER_CM_UNIT_SIZE",
        "communication unit size",      /* shared 모드에서 사용하는 packet 저장 단위 크기 */
        "1024",                         /* 1024B */
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_DISPATCHER_CM_UNIT_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DISPATCHER_CM_UNIT_SIZE )
    },
    {
        "SHARED_SESSION",
        "shared session",               /* shared 모드 사용 여부 */
        "YES",
        (   /* CodeReview : 속성 확인 필요 */
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_SHARED_SESSION,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SHARED_SESSION )
    },
    {
        "DISPATCHERS",
        "dispatchers",                  /* dispatcher 개수 */
        "2",                            /* 2 */
        (   /* CodeReview : 속성 확인 필요 */
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_INCREASE_CHECK   |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_DISPATCHERS,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DISPATCHERS )
    },
    {
        "SHARED_SERVERS",
        "shared servers",               /* shared server개수 */
        "10",                           /* 10 */
        (   /* CodeReview : 속성 확인 필요 */
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_INCREASE_CHECK   |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_SHARED_SERVERS,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SHARED_SERVERS )
    },
    {
        "DISPATCHER_QUEUE_SIZE",
        "dispatcher queue size",        /* dispatcher의 queue size */
        "1024",                         /* 1024 */
        (   /* CodeReview : 속성 확인 필요 */
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_DISPATCHER_QUEUE_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DISPATCHER_QUEUE_SIZE )
    },
    {
        "DISPATCHER_CONNECTIONS",
        "dispatchar connections",       /* dispatcher당 max connection개수 */
        "950",                          /* 950 */
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_DISPATCHER_CONNECTIONS,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DISPATCHER_CONNECTIONS )
    },
    {
        "SHARED_REQUEST_QUEUE_COUNT",
        "count of global request queue", /* global shared request queue 개수 */
        "1",                             /* 1 */
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_FALSE     |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_SHARED_REQUEST_QUEUE_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NONE,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_ONLY,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_SHARED_REQUEST_QUEUE_COUNT )
    },
    {
        "MAXIMUM_SESSION_CM_BUFFER_SIZE",
        "maximum communication bytes per shared mode session",       /* maximum communication bytes per shared mode session */
        "20971520",                           /* 20M */
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_TRUE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_MAXIMUM_SESSION_CM_BUFFER_SIZE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_BELOW,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_MAXIMUM_SESSION_CM_BUFFER_SIZE )
    },
    {
        "BULK_IO_PAGE_COUNT",
        "page count for bulk IO operation",
        "3840",                           /* 30M */
        (
            KNP_ATTR_MD_WRITABLE      |
            KNP_ATTR_MD_SYS_IMMEDIATE |
            KNP_ATTR_MD_SES_FALSE     |
            KNP_ATTR_DOMAIN_EXTERNAL  |
            KNP_ATTR_RANGE_CHECK      |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_BULK_IO_PAGE_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_BULK_IO_PAGE_COUNT )
    },
    {
        "DEFAULT_TABLE_TYPE",
        "default table type(0:ROW, 1:COLUMNAR)",
        "0",
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_NONE
        ),
        KNL_PROPERTY_DEFAULT_TABLE_TYPE,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_DEFAULT_TABLE_TYPE )
    },
    {
        "COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD",
        "threshold of continuos column in columnar table",      /*columnar table에서 continuous column을 member page에 저장하는 길이 */
        "2000",                             /* 2000 */
        (
            KNP_ATTR_MD_WRITABLE       |
            KNP_ATTR_MD_SYS_IMMEDIATE  |
            KNP_ATTR_MD_SES_TRUE       |
            KNP_ATTR_DOMAIN_INTERNAL   |
            KNP_ATTR_RANGE_CHECK       |
            KNP_ATTR_UNIT_SIZE
        ),
        KNL_PROPERTY_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_COLUMNAR_TABLE_CONTINUOUS_COLUMN_THRESHOLD )
    },
    {
        "RECOMPILE_PAGE_PERCENT",
        "recompile page percent",
        "30",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_TRUE      | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_RECOMPILE_PAGE_PERCENT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_RECOMPILE_PAGE_PERCENT )
    },
    {
        "RECOMPILE_CHECK_MINIMUM_PAGE_COUNT",
        "minimum page count for recompile check",
        "64",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_TRUE      | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_RECOMPILE_CHECK_MINIMUM_PAGE_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_RECOMPILE_CHECK_MINIMUM_PAGE_COUNT )
    },
    {
        "TRANSACTION_MAXIMUM_UNDO_PAGE_COUNT",
        "the maximum number of undo pages that the transaction be allocated",
        "838860800",    /* 100G */
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_TRUE      | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_TRANSACTION_MAXIMUM_UNDO_PAGE_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_TRANSACTION_MAXIMUM_UNDO_PAGE_COUNT )
    },
    {
        "TRACE_LOGIN",
        "write login trace messages",
        "NO",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_FALSE     | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_TRACE_LOGIN,
        Group3,
        KNL_PROPERTY_TYPE_BOOL,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_TRACE_LOGIN )
    },
    {
        "MAXIMUM_FLANGE_COUNT",
        "maximum flange count in a plan clock",
        "1024",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_FALSE     | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_MAXIMUM_FLANGE_COUNT,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_MAXIMUM_FLANGE_COUNT )
    },
    {
        "AGING_PLAN_INTERVAL",
        "aging plan interval time(s)",
        "0",
        (
            KNP_ATTR_MD_WRITABLE      | 
            KNP_ATTR_MD_SYS_IMMEDIATE | 
            KNP_ATTR_MD_SES_FALSE     | 
            KNP_ATTR_DOMAIN_EXTERNAL  | 
            KNP_ATTR_RANGE_CHECK      | 
            KNP_ATTR_UNIT_NONE
        ), 
        KNL_PROPERTY_AGING_PLAN_INTERVAL,
        Group3,
        KNL_PROPERTY_TYPE_BIGINT,
        KNL_STARTUP_PHASE_NO_MOUNT,
        KNP_PROPERTY_PHASE_DOMAIN_INCLUDE_UPPER,        
        KNP_PROPERTY_MIN_MAX( KNL_PROPERTY_AGING_PLAN_INTERVAL )
    },
    
    /** 
     * 아래의 NULL 정보는 수정하시면 안됩니다.
     */
    {
        NULL,
        NULL,
        NULL,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    }
};




/**
 * @brief 개발자가 입력한 Property를 등록한다.
 * @param[in,out] aKnlEnv     Kernel Environment
 * @remarks
 * <BR> gPropertyContentList[] 배열에 저장된 PROPERTY를 등록한다.
 */
stlStatus knpBuildPropertyWithDefault( knlEnv  * aKnlEnv )
{
    
    knpPropertyDesc  * sPropertyDesc  = NULL;
    stlInt32           sIdx           = 0;    
    
    /**
     * Property List를 등록한다. 
     */
    for ( sPropertyDesc         = gPropertyContentList;
          sPropertyDesc->mName != NULL;
          sPropertyDesc         = &gPropertyContentList[sIdx] )
    {
        STL_TRY( knpRegistProperty( sPropertyDesc, 
                                    aKnlEnv ) == STL_SUCCESS );
        
        sIdx++;
    }

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
