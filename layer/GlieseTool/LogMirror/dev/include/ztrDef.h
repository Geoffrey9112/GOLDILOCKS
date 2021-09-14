
/*******************************************************************************
 * ztrDef.h
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


#ifndef _ZTRDEF_H_
#define _ZTRDEF_H_ 1
/**
 * @file ztrDef.h
 * @brief LogMirror Definitions
 */

/**
 * @defgroup ztrError Error
 * @{
 */

/**
 * @brief error
 */

#define ZTR_ERRCODE_SERVICE_NOT_AVAILABLE        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 0 )
#define ZTR_ERRCODE_PROTOCOL                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 1 )
#define ZTR_ERRCODE_INVALID_FILE_NAME            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 2 )
#define ZTR_ERRCODE_CONFIGURE_FILE_PARSE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 3 )
#define ZTR_ERRCODE_INVALID_CONTROL_FILE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 4 )
#define ZTR_ERRCODE_COMMUNICATION_LINK_FAILURE   STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 5 )
#define ZTR_ERRCODE_ALREADY_RUNNING              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 6 )
#define ZTR_ERRCODE_INVALID_HOME                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 7 )
#define ZTR_ERRCODE_INVALID_SHM_KEY              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 8 )
#define ZTR_ERRCODE_INVALID_VALUE                STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 9 )
#define ZTR_ERRCODE_INVALID_PORT                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 10 )
#define ZTR_ERRCODE_MASTER_DISCONNT              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 11 )
#define ZTR_ERRCODE_INVALID_LOGFILE              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 12 )
#define ZTR_ERRCODE_INVALID_ARCHIVELOG           STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 13 )
#define ZTR_ERRCODE_ARCHIVELOG_READ_TIMEOUT      STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 14 )
#define ZTR_ERRCODE_NOT_RUNNING                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 15 )
#define ZTR_ERRCODE_INVALID_CONN_INFO            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 16 )
#define ZTR_ERRCODE_RECV_FAIL                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 17 )
#define ZTR_ERRCODE_SERVICE_FAIL                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR, 18 )

#define ZTR_MAX_ERROR     19

/** @} */

/**
 * @defgroup ztrExternal LogMirror Routines
 * @{
 */


/**
 * @addtogroup ztrDef
 * @{
 */

#define ZTR_SPINLOCK_STATUS_INIT         ( 0 )
#define ZTR_SPINLOCK_STATUS_LOCKED       ( 1 )
#define ZTR_DEFAULT_PORT                 ( 20123 )
#define ZTR_DATA_SOURCE_IDENTIFIER       "[" STL_PRODUCT_NAME "]"
#define ZTR_DEFAULT_MASTER_PID_FILE      ".lmmst.pid"
#define ZTR_DEFAULT_SLAVE_PID_FILE       ".lmslv.pid"
#define ZTR_DEFAULT_CONTROL_FILE         "_lmLog.ctl"
#define ZTR_TRACE_BUFFER_SIZE            ( 1024 * 2 )

#define ZTR_DEFAULT_MASTER_CONFIGURE_FILE    "logmirror.master.conf"
#define ZTR_DEFAULT_SLAVE_CONFIGURE_FILE     "logmirror.slave.conf"

#define ZTR_DEFAULT_SLAVE_CHUNK_SIZE     ( 1024 * 1024 * 50 )           //MAX FLUSH SIZE와 상관있음..
#define ZTR_DEFAULT_SLAVE_CHUNK_COUNT    ( 10 )
#define ZTR_DEFAULT_COMM_BUFFER_SIZE     ( 1024 * 8 )
#define ZTR_COMMAND_BUFFER_SIZE          ( 1024 * 8 )
#define ZTR_READ_BUFFER_BLOCK_COUNT      ( 1024 * 2 * 50 )              //READ BYTE == BLOCK_COUNT * 512 Byte

#define ZTR_PROTOCOL_INIT_STR            "_LOGMIRROR"
#define ZTR_LOG_FILE_PREFIX              "lmLog_"

#define ZTR_REGION_INIT_SIZE             ( 1024 * 8 )
#define ZTR_REGION_NEXT_SIZE             ( 1024 * 8 )



#define ZTR_INFINIBAND_BUFFER_SIZE         ( 1024 * 4 )
#define ZTR_INFINIBAND_BUFFER_COUNT_MASTER ( 1024 )
#define ZTR_INFINIBAND_BUFFER_COUNT_SLAVE  ( 100 )


#define ZTR_TCP_SOCKET_BUFF_SIZE         ( 1024 * 1024 * 16 )

#define ZTR_INVALID_PIECE_COUNT          ( -1 )

#define ZTR_INVALID_FILE_SEQ_NO          ( -1 )

#define ZTR_DEFAULT_BLOCK_SIZE     ( 512 )
#define ZTR_MEM_ALIGN              ( 512 )

#define ZTR_SIGNAL_DEFINE                STL_SIGNAL_USR2     //ZL에서 INITIALIZE하기 때문에 QUIT을 사용할 수가 없다.

#define ZTR_INIT_SPIN_LOCK( aSpinLock )     \
{                                           \
    aSpinLock = ZTR_SPINLOCK_STATUS_INIT;   \
}

/** @} */

/**
 * @addtogroup ztrStructure
 * @{
 */

typedef stlUInt32  ztrSpinLock;


typedef struct ztrConfigureParam
{
    void           * mYyScanner;
    stlErrorStack  * mErrorStack;
    stlChar        * mBuffer;
    stlInt32         mPosition;
    stlInt32         mLength;
    stlChar          mErrMsg[1024];
    stlStatus        mErrStatus;
} ztrConfigureParam;


typedef struct ztrConfigure
{
    stlChar        mUserId[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mUserPw[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mHomeDir[STL_MAX_FILE_PATH_LENGTH];
    stlChar        mUserConfigure[STL_MAX_FILE_PATH_LENGTH];
    stlChar        mLogPath[STL_MAX_FILE_PATH_LENGTH];
    stlChar        mDsn[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mIbDevName[STL_MAX_SQL_IDENTIFIER_LENGTH];
    stlChar        mMasterIp[16];
    stlChar        mHostIp[16];
    stlSignalFunc  mOldSignalFunc;
    stlInt64       mShmKey;
    stlSemaphore   mLogSem;
    stlInt32       mPort;
    stlInt32       mHostPort;
    stlInt32       mIbPort;
    stlBool        mIsMaster;
    stlBool        mIsSilent;
    stlBool        mIsInfiniband;
} ztrConfigure;


typedef struct ztrChunkItem
{
    stlRingEntry   mLink;
    stlChar      * mOrgBuffer;      //Allocated Ptr
    stlChar      * mBuffer;         //Aligned Ptr
    stlInt64       mWriteIdx;
    stlInt64       mReadIdx;            //지워??
    stlBool        mHasLogFileHdr;
} ztrChunkItem;


typedef struct ztrRecoveryMgr
{
    stlRingHead        mLogFileList;
    SQLHENV            mEnvHandle;
    SQLHDBC            mDbcHandle;
    stlAllocator       mRegionMem;
    stlChar          * mSendBuff;
    stlInt64           mBufferSize;
    stlInt64           mWriteIdx;
    stlChar            mArchivePath[STL_MAX_FILE_PATH_LENGTH];
    stlChar            mArchiveFilePrefix[STL_MAX_FILE_NAME_LENGTH];
    stlBool            mArchiveMode;
    stlChar            mPadding[7];
} ztrRecoveryMgr;

typedef struct ztrLogFileInfo
{
    stlRingEntry    mLink;
    stlChar         mName[STL_MAX_FILE_PATH_LENGTH];
    stlInt64        mFileSeqNo;
    stlInt32        mBlockSeqNo;
    stlInt16        mGroupId;
    stlBool         mActive;
    stlChar         mPadding;
} ztrLogFileInfo;


typedef struct ztrMasterMgr
{
    stlShm              mSharedMem;
    stlAllocator        mRegionMem;
    ztrRecoveryMgr      mRecoveryMgr;
    stlThread           mThread;
    void              * mListenHandle;
    void              * mSenderHandle;
    ztrLogMirrorInfo  * mLmInfo;
    ztrLogMirrorHdr   * mLmHdr;
    stlChar           * mLogData;
    stlChar           * mSendBuff;
    stlChar           * mRecvBuff;
} ztrMasterMgr;


typedef struct ztrSlaveMgr
{
    stlThread      mRcvThread;
    stlThread      mFshThread;
    void         * mRecvHandle;
    stlAllocator   mRegionMem;
    
    stlChar      * mSendBuff;
    stlChar      * mRecvBuff;
    
    stlInt64       mRecvBuffSize;
    
    stlBool        mWaitWriteState;
    stlBool        mWaitReadState;
    stlChar        mPadd1[6];
    
    /**
     * Flush를 대기하는 ...
     */
    stlSemaphore   mWaitFlushSem;
    /**
     * Receiver에서 사용
     */
    stlRingHead    mWaitWriteCkList;
    stlRingHead    mWriteCkList;
    stlSemaphore   mWaitWriteSem;
    ztrSpinLock    mWriteSpinLock;
    stlChar        mPadd2[4];
    
    /**
     * Flusher에서 사용
     */
    stlRingHead    mWaitReadCkList;
    stlRingHead    mReadCkList;
    stlSemaphore   mWaitReadSem;
    ztrSpinLock    mReadSpinLock;
    stlChar        mPadd3[4];
} ztrSlaveMgr;

typedef struct ztrRecoveryinfo
{
    stlInt64    mLogFileSeqNo;
    stlInt64    mLastBlockSeq;
    stlInt16    mLogGroupId;
} ztrRecoveryinfo;


typedef struct ztrLogCursor
{
    stlChar    * mBuffer;
    stlChar    * mBufferOrg;
    stlFile      mFile;
    stlInt64     mFileBlockSeq;
    stlInt64     mPrevLastLsn;
    stlInt32     mMaxBlockCount;
    stlInt32     mBufferBlockSeq;
    stlInt32     mBufferBlockCount;
    stlInt16     mBlockSize;
    stlInt16     mWrapNo;
} ztrLogCursor;


typedef enum
{
    ZTR_RECOVERY_RESULT_EOF = 0,
    ZTR_RECOVERY_RESULT_READ_DONE,
    ZTR_RECOVERY_RESULT_INVALID_WRAP_NO,
    ZTR_RECOVERY_RESULT_ERR,
    ZTR_RECOVERY_RESULT_MAX
} ztrLogReadResult;


typedef enum
{
    ZTR_LOG_STATUS_NONE = 0,
    ZTR_LOG_STATUS_INVALID_LOG,
    ZTR_LOG_STATUS_INVALID_WRAP_NO,
    ZTR_LOG_STATUS_MAX
} ztrLogStatus;


typedef enum
{
    ZTR_CATEGORY_NONE = 0,
    ZTR_CATEGORY_INIT,
    ZTR_CATEGORY_HANDSHAKE,
    ZTR_CATEGORY_DATA,
    ZTR_CATEGORY_CONTROL,
    ZTR_CATEGORY_ERROR,
    ZTR_CATEGORY_MAX
} ztrProtocolCategory;


typedef enum
{
    ZTR_INIT_REQUEST_CLIENT_TYPE = 0,
    ZTR_INIT_RESPONSE_CLIENT_TYPE,
    
    ZTR_HANDSHAKE_REQUEST_RECOVERY_INFO,
    ZTR_HANDSHAKE_RESPONSE_RECOVERY_INFO,
    ZTR_HANDSHAKE_RESPONSE_NO_RECOVERY_INFO,
    
    ZTR_HANDSHAKE_REQUEST_BUFFER_INFO,
    ZTR_HANDSHAKE_RESPONSE_BUFFER_INFO,

    ZTR_DATA_SEND,
    ZTR_DATA_RECEIVE_OK,
    
    ZTR_CONTROL_STOP,
    
    ZTR_ERROR_PROTOCOL_ERROR,
    ZTR_PROTOCOL_MAX
} ztrProtocolType;


typedef stlStatus (* ztrCreateListenerCallback)( stlErrorStack * aErrorStack );

typedef stlStatus (* ztrSendHdrNDataCallback)( void            * aHandle,
                                               ztrLogMirrorHdr * aHeader,
                                               stlChar         * aData,
                                               stlErrorStack   * aErrorStack );

typedef stlStatus (* ztrRecvHdrNDataCallback)( void            * aHandle,
                                               ztrLogMirrorHdr * aHeader,
                                               stlChar         * aData,
                                               stlErrorStack   * aErrorStack );

typedef stlStatus (* ztrRecvHdrNDataWithPollCallback)( void            * aHandle,
                                                       ztrLogMirrorHdr * aHeader,
                                                       stlChar         * aData,
                                                       stlBool         * aIsTimeout,
                                                       stlErrorStack   * aErrorStack );

typedef stlStatus (* ztrCreateReceiverCallback)( stlErrorStack   * aErrorStack );

typedef stlStatus (* ztrDestoryHandleCallback)( void            * aHandle,
                                                stlErrorStack   * aErrorStack );

typedef stlStatus (* ztrCheckSlaveStateCallback)( void            * aHandle,
                                                  stlErrorStack   * aErrorStack );

typedef struct ztrCommunicateCallback
{
    ztrCreateListenerCallback        mCreateListenerFunc;
    ztrCreateReceiverCallback        mCreateReceiverFunc;
    ztrDestoryHandleCallback         mDestroyHandleFunc;
    ztrSendHdrNDataCallback          mSendHdrNDataFunc;
    ztrRecvHdrNDataCallback          mRecvHdrNDataFunc;
    ztrRecvHdrNDataWithPollCallback  mRecvHdrNDataWithPollFunc;
    ztrCheckSlaveStateCallback       mCheckSlaveStateFunc;
} ztrCommunicateCallback;



/** @} */

/** @} */

#endif /* _ZTRDEF_H_ */
