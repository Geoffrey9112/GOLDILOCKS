/*******************************************************************************
 * cmlDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlDef.h 6781 2012-12-20 03:09:11Z kyungoh $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _CMLDEF_H_
#define _CMLDEF_H_ 1

/*
 * 용어 정리
 * Protocol         : command하나
 * ProtocolSet      : head-tail로 구성된 Protocol 집합. 종속적인 관계임
 * ProtocolSentence : 한번의 요청과 응답에 해당하는 protocolSet들
 * Packet           : ProtocolSentence을 Network로 전달하는 단위.
 *                    ProtocolSentence는 여러개의 Packet으로 나누어 전달될수 있음.
 *                    Packet마다 header가 붙고 header에 ProtocolSentence의 끝을 의미하는 period field가 있음.
 *
 *
 * cs를 위한 구조체 정리.
 *
 *
 * iml   : imlHandle,   imlProtocolSentence
 * ---------------------------------------------------
 * shlib : zlhlHandle
 * ---------------------------------------------------
 * cml   : cmlHandle <- cmlProtocolSentence <- cmlMemoryManager
 *
 *
 *
 * ODBC(tcp)
 *
 *               heap             heap                   heap
 * --------------------------------------------------------------
 * cml       : cmlHandle <- cmlProtocolSentence <- cmlMemoryManager
 *
 *
 *
 * dedicate-server(tcp)
 *
 *              heap              heap                   heap
 * --------------------------------------------------------------
 * iml       : imlHandle,   imlProtocolSentence
 * --------------------------------------------------------------
 * shlib     : zlhlHandle
 * --------------------------------------------------------------
 * cml       : cmlHandle <- cmlProtocolSentence <- cmlMemoryManager
 * --------------------------------------------------------------
 * life cycle:  모두 session( = process )
 *
 *
 * shared-server(ipc)
 *
 *             shared-m             heap                   heap
 * --------------------------------------------------------------
 * iml       : imlHandle,   imlProtocolSentence
 * --------------------------------------------------------------
 * shlib     : zlhlHandle
 * --------------------------------------------------------------
 * cml       : cmlHandle <- cmlProtocolSentence <- cmlMemoryManager
 * --------------------------------------------------------------
 * life cycle: session               job                process
 *
 *   imlHandle은 dispatcher에서 관리 나머지는 shared-server에서 관리
 */



/**
 * @file cmlDef.h
 * @brief Communication Layer Definitions
 */

/**
 * @defgroup cmExternal Communication Layer Routines
 * @{
 */
#define CML_PACKET_HEADER_SIZE                  (4 + 4)
#define CML_MTU_SIZE                            (32)
#define CML_MAX_PROTOCOLSET_STACK_DEPTH         (7)
#define CML_VERSION_SIZE                        (6)

/**
 * @defgroup cmlError Error
 * @{
 */

/**
 * @brief Communication link failure
 */
#define CML_ERRCODE_COMMUNICATION_LINK_FAILURE      STL_MAKE_ERRCODE( STL_ERROR_MODULE_COMMUNICATION, 0 )
#define CML_ERRCODE_INVALID_COMMUNICATION_PROTOCOL  STL_MAKE_ERRCODE( STL_ERROR_MODULE_COMMUNICATION, 1 )
#define CML_ERRCODE_PROPERTY_INVALID_HOME_DIR       STL_MAKE_ERRCODE( STL_ERROR_MODULE_COMMUNICATION, 2 )
#define CML_ERRCODE_NOT_SUPPORT                     STL_MAKE_ERRCODE( STL_ERROR_MODULE_COMMUNICATION, 3 )
#define CML_ERRCODE_INVALID_INTERPROCESS_PROTOCOL   STL_MAKE_ERRCODE( STL_ERROR_MODULE_COMMUNICATION, 4 )

#define CML_MAX_ERROR                               (5)

/** @} */

/**
 * @addtogroup cmlGeneral
 * @{
 */


typedef struct cmlProtocolSetStack
{
    stlInt8 mTop;
    stlBool mHasError[CML_MAX_PROTOCOLSET_STACK_DEPTH];
} cmlProtocolSetStack;

/**
 * @brief   cml session type.
 * @remark  cmlSessionType 의 각 항목 값은  dev/src/sunje/goldilocks/jdbc/cm/Protocol.java의 CONNECT_SESSION_xxx 와 값을 맞춰야 함.
 */
typedef enum cmlSessionType
{
    CML_SESSION_NONE = 0,
    CML_SESSION_DEDICATE,
    CML_SESSION_SHARED,
    CML_SESSION_DEFAULT,        /**< client에서 session type을 default로 선택시 protocol 내의 값으로 전달됨 */
    CML_SESSION_MAX
} cmlSessionType;



/**
 *  @brief socket or IPC 통신을 위한 구조체
 */
typedef struct cmlCommunication
{
    stlContext            * mContext;
} cmlCommunication;

/**
 * @brief 현재 connect의 상태
 */
typedef enum cmlConnectStatus
{
    CML_CONN_STATUS_INIT = 0,
    CML_CONN_STATUS_CONNECTED,      /**< CML_COMMAND_CONNECT를 처리하고 나면 CML_CONN_STATUS_CONNECTED 상태로 변경된다. */
    CML_CONN_STATUS_MAX
} cmlConnectStatus;

/**
 * @brief Command처리시 메모리 관리를 위한 구조체
 * @remark life-cycle은 process이다.
 */
typedef struct cmlMemoryManager
{
    stlAllocator             mAllocator;
    stlInt64                 mPacketBufferSize;
    stlChar                * mWriteBuffer;
    stlChar                * mReadBuffer;
    stlRegionMark            mOperationMark;
    stlInt64                 mOperationBufferSize;
    stlChar                * mOperationBuffer;
} cmlMemoryManager;

/**
 * @brief ProtocolSentence별로 관리가 필요한 구조체
 * @remark life-cycle은 job이다. imlProtocolSentence에 포함되어 있다.\n
 *         cmlProtocolSentence는 protocolSentence 마다 초기화(cmlInitializeProtocolSentence) 되어야 하지만 \n
 *         ODBC에서는 처음에 한번만 호출한다. (ODBC에서는 protocolSentence 개념이 명확히 구현되어 있지 않음)
 *
 */
typedef struct cmlProtocolSentence
{
    cmlMemoryManager       * mMemoryMgr;
    stlChar                * mWriteStartPos;
    stlChar                * mWriteProtocolPos;
    stlChar                * mWriteEndPos;
    stlChar                * mReadStartPos;
    stlChar                * mReadProtocolPos;
    stlChar                * mReadEndPos;
    stlInt8                  mReadPeriod;       /**< 받은 packet의 period값 CML_PERIOD_CONTINUE or CML_PERIOD_END */
    stlInt32                 mReadPayloadSize;  /**< 받은 packet의 payload size */
    stlInt64                 mCachedStatementId;
} cmlProtocolSentence;

/**
 * @brief  protocol을 관리하는 구조체
 * @remark shared mode에서는 이 부분이 shared memory로 관리되어 여러 shared server에서 공유하게 된다.
 *         life-cycle은 session이다. zlhlHandle에 포함되어 있다.
 */
typedef struct cmlHandle
{
    cmlCommunication         mCommunication;
    stlInt8                  mEndian;            /**< protocol에 적용할 endian */
    stlUInt8                 mSendSequence;      /**< send period sequence */
    stlUInt8                 mRecvSequence;      /**< recv period sequence */
    cmlProtocolSentence    * mProtocolSentence;
} cmlHandle;

typedef enum
{
    CML_DBC_ODBC = 1,
    CML_DBC_JDBC,
    CML_DBC_MAX
} cmlDbcType;

#define CML_CONTROL_NONE      ( 0 )
#define CML_CONTROL_HEAD      ( 1 )
#define CML_CONTROL_TAIL      ( 2 )


#define CML_RESULT_SUCCESS ( 0 )
#define CML_RESULT_WARNING ( 1 )
#define CML_RESULT_ABORT   ( 2 )
#define CML_RESULT_FATAL   ( 3 )
#define CML_RESULT_IGNORE  ( 4 )



/** @} */

/**
 * @addtogroup cmlCommand
 * @{
 */

/** @} */

/**
 * @addtogroup cmlProtocol
 * @{
 */

#define CML_STATEMENT_ID_UNDEFINED       ( -1 )

/**
 * inter-process 통신의 version
 * 현재 inter-process 통신에서 inter-process protocol 버전이 다르면 무시하도록 되어 있음. (하위호환 기능 없음.)
 * => inter-process protocol 이 변경되면 patch version을 올리면됨.
 */
#define CML_INTERPROCESS_PROTOCOL_MAJOR_VERSION      ((stlInt16) 1 )
#define CML_INTERPROCESS_PROTOCOL_MINOR_VERSION      ((stlInt16) 1 )
#define CML_INTERPROCESS_PROTOCOL_PATCH_VERSION      ((stlInt16) 1 )


/**
 * 정책등으로 protocol호환 안되는 경우 major version(2byte) 증가
 * header또는 command변경으로 호환 처리 가능한 변경시 minor version (2byte) 증가
 * command가 추가되어 버전 관리 없이 호환 가능한 경우 patch version(2byte) 증가
 * 상위 버전이 올라가면 하위 버전 초기화
 *
 * branch에 protocol 변경은 불가함. (호환성 부분에서 많은 문제점 발생)
 */
#define CML_PROTOCOL_MAJOR_VERSION      ((stlInt16) 1 )
#define CML_PROTOCOL_MINOR_VERSION      ((stlInt16) 2 )
#define CML_PROTOCOL_PATCH_VERSION      ((stlInt16) 1 )

/**
 * packet header중 period 와 sequence가 1byte를 사용함.
 * sequence는 period를 보내고 나면 증가함.
 * period는 상위 1bit를 사용 나머지 7bit는 sequence가 사용함.
 */
#define CML_PERIOD_CONTINUE  ( 0 )
#define CML_PERIOD_END       ( 0x80 )
#define CML_PERIOD_MASK      ( 0x80 )
#define CML_PERIOD_SEQ_MASK  ( 0x7f )
#define CML_GET_PERIOD( aPeriod, aSrc ) { (aPeriod) = (stlUInt8)(aSrc) & CML_PERIOD_MASK; }

typedef enum
{
    /*
     * protocol set version 1
     */
    
    CML_COMMAND_NONE = 0,
    CML_COMMAND_HANDSHAKE,
    CML_COMMAND_CONNECT,
    CML_COMMAND_DISCONNECT,
    CML_COMMAND_GETCONNATTR,
    CML_COMMAND_SETCONNATTR,  /*  5 */
    CML_COMMAND_FREESTMT,
    CML_COMMAND_CANCEL,
    CML_COMMAND_DESCRIBECOL,
    CML_COMMAND_TRANSACTION,
    CML_COMMAND_PREPARE,      /* 10 */
    CML_COMMAND_EXECDIRECT,
    CML_COMMAND_EXECUTE,
    CML_COMMAND_GETOUTBINDDATA,
    CML_COMMAND_FETCH,
    CML_COMMAND_TARGETTYPE,   /* 15 */
    CML_COMMAND_BINDTYPE,
    CML_COMMAND_BINDDATA,
    CML_COMMAND_GETSTMTATTR,
    CML_COMMAND_SETSTMTATTR,
    CML_COMMAND_NUMPARAMS,    /* 20 */
    CML_COMMAND_NUMRESULTCOLS,
    CML_COMMAND_GETCURSORNAME,
    CML_COMMAND_SETCURSORNAME,
    CML_COMMAND_SETPOS,
    CML_COMMAND_CLOSECURSOR,  /* 25 */
    CML_COMMAND_STARTUP,
    CML_COMMAND_XA_CLOSE,
    CML_COMMAND_XA_COMMIT,
    CML_COMMAND_XA_ROLLBACK,
    CML_COMMAND_XA_PREPARE,   /* 30 */
    CML_COMMAND_XA_START,
    CML_COMMAND_XA_END,
    CML_COMMAND_XA_FORGET,
    CML_COMMAND_XA_RECOVER,
    CML_COMMAND_PARAMETERTYPE,
    CML_COMMAND_PERIOD,
    CML_COMMAND_CONTROLPACKET,   
    CML_COMMAND_PRE_HANDSHAKE,   /* gsql <-> listener */
    /*
     * protocol set version 2
     */
    
    CML_COMMAND_MAX
    
} cmlCommandType;


/**
 * 상태 정보 buffer 최대 크기
 */
#define  CML_STATUS_INFO_LENGTH             (1024 * 4)  /* 4KB */

/**
 * Unix Domain Socket 통신의 command type
 */
typedef enum cmlUdsCmdType
{
    CML_UDS_CMD_REQUEST_FD,
    CML_UDS_CMD_FORWARD_FD,
    CML_UDS_CMD_ACK,
    CML_UDS_CMD_REQUEST_STATUS,
    CML_UDS_CMD_RESPONSE_STATUS,
    CML_UDS_CMD_REQUEST_QUIT,
    CML_UDS_CMD_MAX,
} cmlUdsCmdType;

/**
 * Uds통신의 버전 구조체
 */
typedef struct cmlUdsVersion
{
    stlUInt16       mMajorVersion;
    stlUInt16       mMinorVersion;
    stlUInt16       mPatchVersion;
} cmlUdsVersion;

/**
 * fd를 요청하는 구조체
 */
typedef struct cmlUdsRequestFd
{
    cmlUdsVersion           mVersion;
    cmlUdsCmdType           mCmdType;
} cmlUdsRequestFd;

/**
 * fd를 전달하는 구조체
 */
typedef struct cmlUdsForwardFd
{
    cmlUdsVersion           mVersion;
    cmlUdsCmdType           mCmdType;
    stlInt64                mKey;
    stlTime                 mSenderStartTime;
    stlInt64                mSequence;
    stlInt64                mIdx;
    stlUInt16               mMajorVersion;
    stlUInt16               mMinorVersion;
    stlUInt16               mPatchVersion;
} cmlUdsForwardFd;

/**
 * ack or nack 구조체
 */
typedef struct cmlUdsAck
{
    cmlUdsVersion           mVersion;
    cmlUdsCmdType           mCmdType;
    stlErrorStack           mErrorStack;
    stlTime                 mSenderStartTime;
    stlInt64                mSequence;
    stlInt64                mIdx;
} cmlUdsAck;

/**
 * 상태를 요청하는 구조체
 */
typedef struct cmlUdsRequestStatus
{
    cmlUdsVersion           mVersion;
    cmlUdsCmdType           mCmdType;
} cmlUdsRequestStatus;

/**
 * 상태를 전달하는 구조체
 */
typedef struct cmlUdsResponseStatus
{
    cmlUdsVersion           mVersion;
    cmlUdsCmdType           mCmdType;
    stlChar                 mData[CML_STATUS_INFO_LENGTH];
} cmlUdsResponseStatus;

/**
 * 종료 요청 구조체
 */
typedef struct cmlUdsRequestQuit
{
    cmlUdsVersion           mVersion;
    cmlUdsCmdType           mCmdType;
} cmlUdsRequestQuit;

/**
 * 공통 구조체
 */
typedef struct cmlUdsCommon
{
    cmlUdsVersion           mVersion;
    cmlUdsCmdType           mCmdType;
} cmlUdsCommon;

/**
 * Uds통신의 Union 구조체
 */
typedef struct cmlUdsPacket
{
    union cmlUdsType {
        cmlUdsCommon            mCommon;
        cmlUdsRequestFd         mRequestFd;
        cmlUdsForwardFd         mForwardFd;
        cmlUdsAck               mAck;
        cmlUdsRequestStatus     mRequestStatus;
        cmlUdsResponseStatus    mResponseStatus;
        cmlUdsRequestQuit       mRequestQuit;
    } mCmd;
} cmlUdsPacket;

typedef struct cmlTargetType
{
    stlInt8  mType;
    stlInt8  mStringLengthUnit;
    stlInt8  mIntervalIndicator;
    stlInt64 mArgNum1;
    stlInt64 mArgNum2;
} cmlTargetType;


#define CML_STATIC_ADDR_FILE_NAME   ( "conf"STL_PATH_SEPARATOR"shmstatic_info" )



/**
 * @brief Create 된 Shared Memory정보를 저장하기 위한 구조체.
 */
typedef struct cmlShmStaticFileInfo
{
    stlChar    mName[STL_MAX_SHM_NAME+1]; /**< 공유 메모리 세그먼트의 이름 */
    stlInt64   mAddr;                     /**< 공유 메모리 세그먼트의 주소 */
    stlInt64   mKey;                      /**< 공유 메모리 세그먼트의 키 */
} cmlShmStaticFileInfo;



#define CML_NCHAR_CHARACTERSET_OFFSET    (100)

/**
 * @brief handshake buffer size
 * @remark handshake시 할당되는 버퍼 크기로 handshake size보다 커야 함.
 */
#define CML_HANDSHAKE_BUFFER_SIZE        ( 1024 )

/**
 *  @brief buffer size
 */
#define CML_BUFFER_SIZE                  ( 32 * 1024 )


/*
 * CALLBACK FUNCTIONS
 */

typedef void (*cmlCheckControlCallback) ( void           * aHandle,
                                          stlInt64       * aStatementId,
                                          stlInt8          aControl,
                                          stlBool        * aIgnore,
                                          stlBool        * aProtocolError,
                                          stlErrorStack  * aErrorStack );

typedef stlStatus (*cmlGetBindValueCallback) ( void           * aHandle,
                                               stlInt64         aStatementId,
                                               stlInt32         aColumnNumber,
                                               stlInt8          aControl,
                                               dtlDataValue  ** aBindValue,
                                               stlErrorStack  * aErrorStack );

typedef stlStatus (*cmlReallocLongVaryingMemCallback) ( void          * aHandle,
                                                        dtlDataValue  * aBindValue,
                                                        void          * aCallbackContext,
                                                        stlErrorStack * aErrorStack );

typedef stlStatus (*cmlGetTargetTypeCallback) ( void               * aHandle,
                                                dtlValueBlockList  * aTargetColumn,
                                                stlInt8            * aDataType,
                                                stlInt8            * aStringLengthUnit,
                                                stlInt8            * aIntervalIndicator,
                                                stlInt64           * aArgNum1,
                                                stlInt64           * aArgNum2,
                                                stlErrorStack      * aErrorStack );

/** @} */

/** @} */

#endif /* _CMLDEF_H_ */




