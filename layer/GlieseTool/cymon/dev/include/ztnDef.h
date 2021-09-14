/*******************************************************************************
 * ztnDef.h
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


#ifndef _ZTNDEF_H_
#define _ZTNDEF_H_ 1

/**
 * @file ztnDef.h
 * @brief Cyclone Monitor Definitions
 */

/**
 * @defgroup ztnError Error
 * @{
 */

/**
 * @brief error
 */

#define ZTN_ERRCODE_INVALID_HOME                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 0 )
#define ZTN_ERRCODE_INVALID_FILE_NAME            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 1 )
#define ZTN_ERRCODE_SERVICE_NOT_AVAILABLE        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 2 )
#define ZTN_ERRCODE_CONFIGURE_FILE_PARSE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 3 )
#define ZTN_ERRCODE_DUPLICATE_GROUP_NAME         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 4 )
#define ZTN_ERRCODE_INVALID_VALUE                STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 5 )
#define ZTN_ERRCODE_INVALID_GROUP_NAME           STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 6 )
#define ZTN_ERRCODE_INVALID_PORT                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 7 )
#define ZTN_ERRCODE_INVALID_GROUP_COUNT          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 8 )
#define ZTN_ERRCODE_ALREADY_RUNNING              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 9 )
#define ZTN_ERRCODE_COMMUNICATION_LINK_FAILURE   STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 10 )
#define ZTN_ERRCODE_PROTOCOL                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 11 )
#define ZTN_ERRCODE_ALREADY_CONN                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 12 )
#define ZTN_ERRCODE_NOT_RUNNING                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 13 )
#define ZTN_ERRCODE_INTERNAL_ERROR               STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 14 )
#define ZTN_ERRCODE_UPDATE_INFO_FAIL             STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 15 )
#define ZTN_ERRCODE_INVALID_CONN_INFO            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 16 )
#define ZTN_ERRCODE_INVALID_ENCRYPTED_PASSWD     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 17 )
#define ZTN_ERRCODE_NO_KEY_PARAMETER             STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 18 )
#define ZTN_ERRCODE_PASSWD_ALREADY_EXISTS        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_CYMON, 19 )

#define ZTN_MAX_ERROR     20

/** @} */


/**
 * @defgroup ztnExternal Cyclone Monitor Routines
 * @{
 */

/**
 * @addtogroup ztnDef
 * @{
 */

#define ZTN_SIGNAL_DEFINE                STL_SIGNAL_USR2     //ZL에서 INITIALIZE하기 때문에 QUIT을 사용할 수가 없다.

#define ZTN_DEFAULT_CONFIGURE_FILE       "cyclone.master.conf"

#define ZTN_DEFAULT_PID_FILE             ".cymon.pid"

#define ZTN_REGION_INIT_SIZE             ( 1024 * 8 )

#define ZTN_REGION_NEXT_SIZE             ( 1024 * 8 )

#define ZTN_TRACE_BUFFER_SIZE            ( 1024 * 2 )

#define ZTN_DATA_SOURCE_IDENTIFIER       "[" STL_PRODUCT_NAME "]"

#define ZTN_COMMAND_BUFFER_SIZE          ( 1024 * 8 )

#define ZTN_INFO_TABLE_NAME              "CYCLONE_MONITOR_INFO"

#define ZTN_DEFAULT_LOG_BLOCK_SIZE       ( 512 )

#define ZTN_DEFAULT_MONITOR_CYCLE        ( 2 )

#define ZTN_MAX_ENCRYPT_BLOCK            ( 8 )
#define ZTN_MAX_CIPHER_SIZE              (STL_MAX_SQL_IDENTIFIER_LENGTH + ZTN_MAX_ENCRYPT_BLOCK)
#define ZTN_MAX_BASE64_SIZE              ((STL_MAX_SQL_IDENTIFIER_LENGTH + ZTN_MAX_ENCRYPT_BLOCK) * 2)

/** @} */

/**
 * @addtogroup ztrStructure
 * @{
 */

typedef struct ztnConfigureParam
{
    void           * mYyScanner;
    stlErrorStack  * mErrorStack;
    stlChar        * mBuffer;
    stlInt32         mPosition;
    stlInt32         mLength;
    stlChar          mErrMsg[1024];
    stlStatus        mErrStatus;
} ztnConfigureParam;


typedef struct ztnConfigure
{
    stlSignalFunc  mOldSignalFunc;
    stlChar        mHomeDir[STL_MAX_FILE_PATH_LENGTH];
    stlChar        mUserConfigure[STL_MAX_FILE_PATH_LENGTH];
    
    stlChar        mUserId[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mUserPw[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mDsn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlInt64       mCycle;
    stlChar        mHostIp[16];
    stlInt32       mHostPort;
    stlInt32       mGroupCount;
    stlBool        mIsSilent;
    stlChar        mPadding[3];
    stlChar        mKey[16];
} ztnConfigure;


typedef struct ztnMonitorInfo
{
    stlChar  mSlaveIp[32];
    stlInt64 mFileSeqNo;
    stlInt32 mBlockSeq;
    stlInt32 mMasterState;
    stlInt32 mSlaveState;
    stlInt32 mPort;
} ztnMonitorInfo;


typedef struct ztnLogFileInfo
{
    stlInt64 mFileSize;
    stlInt32 mBlockCount;
    stlInt16 mGroupId;
    stlInt16 mPadd;
} ztnLogFileInfo;


typedef struct ztnGroupItem
{
    stlRingEntry   mLink;
    stlContext     mContext;
    stlChar        mName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mUserId[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mUserPw[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mDsn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mHostIp[16];
    stlInt32       mHostPort;
    stlInt32       mPort;
    ztnMonitorInfo mMonitorInfo;
    stlBool        mIsConnect;
} ztnGroupItem;


typedef struct ztnParameter
{
    stlChar          mMasterState[16];
    stlChar          mSlaveState[16];
    stlChar          mSlaveIp[32];
    stlChar          mGroupName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    stlInt64         mRedoFileSeqNo;
    stlInt64         mCaptureFileSeqNo;
    stlInt32         mRedoBlockSeq;
    stlInt32         mCaptureBlockSeq;
    stlInt64         mInterval;
    stlInt64         mIntervalSize;
    
    SQLLEN           mMasterStateInd;
    SQLLEN           mSlaveStateInd;
    SQLLEN           mSlaveIpInd;
    SQLLEN           mGroupNameInd;
    SQLLEN           mRedoFileSeqNoInd;
    SQLLEN           mCaptureFileSeqNoInd;
    SQLLEN           mRedoBlockSeqInd;
    SQLLEN           mCaptureBlockSeqInd;
    SQLLEN           mIntervalInd;
    SQLLEN           mIntervalSizeInd;
} ztnParameter;


typedef struct ztnManager
{
    stlAllocator     mAllocator;
    stlRingHead      mGroupList;
    
    SQLHENV          mEnvHandle;
    SQLHDBC          mDbcHandle;
    SQLHSTMT         mUpdStmtHandle;
    
    ztnLogFileInfo * mLogFileInfo;
    stlInt32         mLogFileCount;
    
    ztnParameter     mParameter;
    
} ztnManager;


typedef struct ztnProtocolHdr
{
    stlInt32    mCategory;
    stlInt32    mType;
    stlInt64    mLength;
} ztnProtocolHdr;


typedef enum
{
    ZTN_CATEGORY_NONE = 0,
    ZTN_CATEGORY_INIT,
    ZTN_CATEGORY_HANDSHAKE,
    ZTN_CATEGORY_SYNC,
    ZTN_CATEGORY_DATA,
    ZTN_CATEGORY_CONTROL,
    ZTN_CATEGORY_MONITOR,
    ZTN_CATEGORY_ERROR
} ztnProtocolCategory;


typedef enum
{
    ZTN_INIT_REQUEST_CLIENT_TYPE = 0,
    ZTN_INIT_RESPONSE_CLIENT_TYPE,
    ZTN_INIT_CLIENT_OK,    
    
    ZTN_HANDSHAKE_REQUEST_TABLE_COUNT,
    ZTN_HANDSHAKE_RESPONSE_TABLE_COUNT,
    
    ZTN_HANDSHAKE_REQUEST_TABLE_NAME,
    ZTN_HANDSHAKE_RESPONSE_TABLE_NAME,
    ZTN_HANDSHAKE_RESPONSE_INVALID_TABLE_NAME,
    
    ZTN_HANDSHAKE_REQUEST_TABLE_ID,
    ZTN_HANDSHAKE_RESPONSE_TABLE_ID,
    
    ZTN_HANDSHAKE_REQUEST_COLUMN_COUNT,
    ZTN_HANDSHAKE_RESPONSE_COLUMN_COUNT,
    
    ZTN_HANDSHAKE_REQUEST_COLUMN_INFO,
    ZTN_HANDSHAKE_RESPONSE_COLUMN_INFO,
    ZTN_HANDSHAKE_RESPONSE_COLUMN_INFO_SUCCESS,
    ZTN_HANDSHAKE_RESPONSE_COLUMN_INFO_FAIL,
    
    ZTN_HANDSHAKE_REQUEST_CAPTURE_INFO,
    ZTN_HANDSHAKE_RESPONSE_CAPTURE_INFO,
    
    ZTN_SYNC_REQUEST_CONN_INFO,
    ZTN_SYNC_RESPONSE_CONN_INFO,
    ZTN_SYNC_MODE_OK,
    ZTN_SYNC_MODE_NOT_OK,

    ZTN_DATA_SEND,
    ZTN_DATA_RECEIVE_OK,
    
    ZTN_CONTROL_STOP,
    
    ZTN_CONTROL_HEARTBEAT,
    
    ZTN_CONTROL_SYNC_START,
    ZTN_CONTROL_SYNC_DONE,
        
    ZTN_MONITOR_REQUEST_MONITOR_INFO,
    ZTN_MONITOR_RESPONSE_MONITOR_INFO,
    
    ZTN_ERROR_SLAVE_ALREADY_CONNECTED,
    ZTN_ERROR_PROTOCOL_ERROR,
    ZTN_ERROR_MASTER,
    ZTN_ERROR_SLAVE,
    
    ZTN_PROTOCOL_MAX
} ztnProtocolTYPE;


typedef enum
{
    ZTN_MASTER_STATE_NA = 0,
    ZTN_MASTER_STATE_RUNNING,
    ZTN_MASTER_STATE_READY,
    ZTN_MASTER_STATE_MAX
} ztnMasterState;


typedef enum
{
    ZTN_SLAVE_STATE_NA = 0,
    ZTN_SLAVE_STATE_RUNNING,
    ZTN_SLAVE_STATE_MAX
} ztnSlaveState;


/** @} */

/** @} */

#endif /* _ZTNDEF_H_ */
