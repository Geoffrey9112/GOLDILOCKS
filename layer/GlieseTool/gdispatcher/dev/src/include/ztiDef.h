/*******************************************************************************
 * ztiDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztiDef.h 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ZTIDEF_H_
#define _ZTIDEF_H_ 1

#include <sll.h>

/**
 * @file ztiDef.h
 * @brief Gliese Dispatcher Definitions
 */

/**
 * @defgroup  zti Gliese Dispatcher
 * @{
 */

/**
 * @brief dispatcher에서 client와 통신을 하기 위한 구조체
 */
typedef struct ztiContext
{
    stlContext          mStlContext;        /**< mHandle->mContext는 pointer임. */
    stlPollFdEx         mPollFdEx;
    stlChar           * mReadBuffer;
    stlInt32            mReadSize;          /**< 현재 socket에서 읽은 byte */
    stlInt32            mPayloadSize;       /**< 받은 packet의 payload size */
    stlUInt8            mPeriod;
    stlBool             mUsed;              /**< 구조체 사용 여부 */
    stlBool             mIsBrokenSocket;    /**< socket이 끊어졌는지 여부 */
    stlBool             mPeriodEndForSend;  /**< client로 전송 대기중인 buffer에 SCL_QCMD_NORMAL_PERIOD_END 존재 여부 */
    stlTime             mLastTime;          /**< for timeout */
    stlInt32            mJobCount;          /**< shared-server에서 수행해야 하는 JOB의 개수 */
    sllHandle         * mSllHandle;         /**< 여러 shared server가 공유해야 하는 정보 */
    stlTime             mDispatcherTime;    /**< dispatcher가 생성된 시간 */
    stlChar           * mDataForSend;       /**< client로 전송중인 data */
    stlInt32            mDataLenForSend;    /**< client로 전송중인 data의 length */
    stlSize             mSentLenForSend;    /**< client로 전송중인 data의 보낸 length */
    stlInt32            mCmUnitCountForSend;/**< client로 전송 대기중인 buffer count */
} ztiContext;

/**
 * @brief ztiContext, stlPollFd등을 관리하기 위한 구조체
 */
typedef struct ztiDispatcher
{
    stlInt64            mMaxContextCount;       /**< 할당된 mContext 개수 */
    ztiContext        * mContext;               /**< ztiContext array */
    stlPollSet          mPollSet;               /**< stlPollSet사용하기 위한 stlPollSet  */
    sllDispatcher     * mZlhlDispatcher;        /**< shared memory의 dispatcher structure */
    stlInt32            mId;                    /**< dispatcher identifier */
    stlBool             mExitProcess;           /**< main thread와 send thread가 같이 죽기 위한 flag */
    stlBool             mRetry;                 /**< retry중인 send data가 있는지 여부 */
    stlThread           mSendThread;
} ztiDispatcher;

#define ZTI_INDEX_UDS                0
#define ZTI_INDEX_SESSION            1

extern sllSharedMemory    * gZtiSharedMemory;

extern stlErrorRecord       gZtieErrorTable[];

extern stlGetOptOption      gZtimOptOption[];

/* stlPoll의 timeout */
#define ZTI_POLL_TIMEOUT                STL_SET_SEC_TIME(1)

#define ZTI_HEAPMEM_CHUNK_SIZE          ( 1024 * 1024 )

/**
 * @brief dispatcher error code
 */
#define ZTIE_ERRCODE_ALREADY_START       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GDISPATCHER, 0 )
#define ZTIE_ERRCODE_OVERFLOW_FD         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GDISPATCHER, 1 )


/** @} */

#endif /* _ZTIDEF_H_ */
