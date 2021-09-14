/*******************************************************************************
 * ztlDef.h
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


#ifndef _ZTLDEF_H_
#define _ZTLDEF_H_ 1

#include <stl.h>
#include <cml.h>

/**
 * @file ztlDef.h
 * @brief Gliese Listener Definitions
 */

/**
 * @defgroup ztlExternal Gliese Listener Routines
 * @{
 */

/**
 * @brief ipaddress max size
 */
#define  ZTLM_MAX_IPADDRESS                 (16)


/**
 * @brief Parent/Child 동기화
 */

#define  ZTLM_INIT_MARK_LENGTH       (4)
#define  ZTLM_INIT_MARK_STR          "INIT"
#define  ZTLM_CHILD_MARK_STR         "CHLD"

/**
 * @brief tcp filtering할 type
 */
typedef enum ztlmTcpFilterType
{
    ZTLM_TCP_FILTER_NONE = 0,
    ZTLM_TCP_FILTER_INVITED,  /**< ip list만 허용 */
    ZTLM_TCP_FILTER_EXCLUDED  /**< ip list만 거부 */
} ztlmTcpFilterType;

/**
 * @brief filtering list를 관리하는 구조체
 */
typedef struct ztlmTcpFilter
{
    stlInt32            mIpCount;
    stlInt32          * mIpAddress;    /**< ipaddress list */
} ztlmTcpFilter;


#define ZTLM_LISTENER_CONF_FILE     "conf"STL_PATH_SEPARATOR"goldilocks.listener.conf"

#define ZTLM_DB_HOME_DIR            "<" STL_ENV_DB_DATA ">"


#define ZTLC_INDEX_LISTEN_SOCKET      0
#define ZTLC_INDEX_UDS                1
#define ZTLC_INDEX_SESSION            2

#define ZTL_MAX_LOGGER_SIZE           ( 10 * 1024 * 1024 )  /**< 10M bytes */

/**
 * @brief client와 통신을 위한 구조체.\n
 * listener에서 Dedicate, shared 구분하기 위해 client와 통신(preHandshake)을 한다.
 *
 */
typedef struct ztlcContext
{
    stlInt64        mIdx;                   /**< 자신의 index */
    stlInt64        mSequence;              /**< dispatcher로 fd를 전송하고 ack를 받을때 fd를 구별하는 용도 */
    stlContext      mStlContext;            /**< socket context */
    stlPollFdEx     mPollFdEx;
    stlTime         mLastTime;              /**< for timeout */
    stlSize         mRecvedSize;            /**< handshake를 위한 recv size */
    stlChar         mRecvBuf[CML_VERSION_SIZE + CML_MTU_SIZE]; /**< handshake를 위한 recv buffer */
    stlBool         mUsed;                  /**< struct data에 대한 사용 여부 */
    stlBool         mReceivedPreHandshake;  /**< preHandshake 수신 완료 여부 */
    stlBool         mIsFdSending;           /**< dispatcher로 전송중인지 여부 */
    cmlSessionType  mSessionType;
    stlInt16        mMajorVersion;          /**< client의 버전 */
    stlInt16        mMinorVersion;          /**< client의 버전 */
    stlInt16        mPatchVersion;          /**< client의 버전 */
} ztlcContext;
extern stlBool       gPassingReady;

/**
 * @brief ztlcContext, stlPollFd등을 관리하기 위한 구조체
 */
typedef struct ztlcListener
{
    stlInt64                mMaxContextCount;       /**< 할당된 mContext 개수 */
    ztlcContext           * mContext;               /**< ztlcContext array */
    stlPollSet              mPollSet;               /**< PollSet사용하기 위한 stlPollSet */
    stlUInt64               mNextSequence;          /**< ztlcContext의 mSequence에 넣어질 sequence */
    cmlMemoryManager        mMemoryMgr;             /**< memory manager */
    stlBool                 mValidFdReceiverAddr;   /**< FdReciverAddr이 유효한 상태인지 여부 */
    stlSockAddr             mFdReceiverAddr;        /**< accept한 fd를 전달할 address */
    stlTime                 mStartTime;             /**< listener 시작 time */

} ztlcListener;


/**
 *  @brief listener configuration 관리 구조체
 */
typedef struct ztlmConfig
{
    stlChar             mTcpHost[ZTLM_MAX_IPADDRESS];                       /**< listen시 bind할 ip address */
    stlInt64            mTcpPort;                                           /**< listen port */
    stlInt64            mBacklog;                                           /**< 대기 가능한 client 수 */
    cmlSessionType      mDefaultCsMode;                                    /**< client에서 session type을 default로 했을때 설정되는 session type값 */
    ztlmTcpFilterType   mTcpValidNodeChecking;                              /**< client ip address 접근 제한 사용 여부 */
    stlChar             mTcpInvitedFile[STL_MAX_INI_PROPERTY_VALUE + 1];    /**< client ip address 접근 가능 목록의 파일명 */
    stlChar             mTcpExcludedFile[STL_MAX_INI_PROPERTY_VALUE + 1];   /**< client ip address 접근 거부 목록의 파일명 */
    stlInt64            mTimeout;                                           /**< dedicate server나 dispatcher를 할당못받고 timeout시 disconnect */
    stlChar             mLogDir[STL_MAX_FILE_PATH_LENGTH + 1];            /**< listener log directory */
} ztlmConfig;

/**
 * @brief Listener main
 */
extern stlBool              gZtlSilentMode;
extern stlBool              gZtlDaemonize;
extern stlBool              gZtlQuitListen;
extern stlBool              gZtlNoExecute;
extern ztlmConfig           gZtlConfig;
extern stlChar              gZtlUdsPath[STL_MAX_FILE_PATH_LENGTH];
extern stlChar              gZtlConfFilePath[STL_MAX_FILE_PATH_LENGTH];
extern stlProc              gZtlProcess;
extern stlBool              gZtlAlreadyStart;


/**
 * @brief Listener error code
 */
#define ZTLE_ERRCODE_CONF_FILE_NOT_EXIST                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 0 )
#define ZTLE_ERRCODE_UNDEFINED_ENV_VARIABLE                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 1 )
#define ZTLE_ERRCODE_ALREADY_START                          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 2 )
#define ZTLE_ERRCODE_OVERFLOW_FD                            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 3 )
#define ZTLE_ERRCODE_INVALID_COMMUNICATION_PROTOCOL         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 4 )
#define ZTLE_ERRCODE_ACCESS_DENIED                          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 5 )
#define ZTLE_ERRCODE_TIMEDOUT                               STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 6 )
#define ZTLE_ERRCODE_SHARED_MODE_INACTIVE                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 7 )
#define ZTLE_ERRCODE_INVALID_PROPERTY_DEFAULT_CS_MODE       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 8 )
#define ZTLE_ERRCODE_INVALID_PROPERTY_VALIDNODE_CHECKING    STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 9 )
#define ZTLE_ERRCODE_INVALID_PROPERTY_BOOLEAN_TYPE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 10 )
#define ZTLE_ERRCODE_INVALID_PROPERTY_INT_TYPE              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 11 )
#define ZTLE_ERRCODE_INVALID_PROPERTY_STRING_TYPE           STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 12 )
#define ZTLE_ERRCODE_BUFFER_OVERFLOW                        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER, 13 )

#define ZTLE_MAX_ERROR                      (14)

extern stlErrorRecord gZtleErrorTable[];



/** @} */

#endif /* _ZTLDEF_H_ */
