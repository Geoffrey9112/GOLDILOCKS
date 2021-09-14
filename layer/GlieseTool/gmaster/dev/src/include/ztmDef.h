/*******************************************************************************
 * ztmDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmDef.h 4233 2012-03-29 05:32:53Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ZTMDEF_H_
#define _ZTMDEF_H_ 1

/**
 * @file ztmDef.h
 * @brief Gliese Master Definitions
 */

/**
 * @defgroup ztmExternal Gliese Master Routines
 * @{
 */

/**
 * @addtogroup ztmBoot
 * @{
 */

typedef enum
{
    ZTM_STATE_INIT = 0,
    ZTM_STATE_RUN,
    ZTM_STATE_STOP_AGER,
    ZTM_STATE_STOP_CHECK_POINT,
    ZTM_STATE_STOP_PAGE_FLUSH,
    ZTM_STATE_STOP_IO_SLAVES,
    ZTM_STATE_STOP_LOG_FLUSH,
    ZTM_STATE_STOP_ARCHIVELOG,
    ZTM_STATE_STOP_CLEANUP,
    ZTM_STATE_STOP_TIMER,
    ZTM_STATE_STOP_PROCESS_MONITOR,
    ZTM_STATE_STOPPED
} ztmState;

typedef enum
{
    ZTM_SERVER_STATUS_INIT = 0,
    ZTM_SERVER_STATUS_START,
    ZTM_SERVER_STATUS_SHUTDOWN,
    ZTM_SERVER_STATUS_SHUTDOWN_ABORT,
    ZTM_SERVER_STATUS_EXIT
} ztmServerStatus;

typedef struct ztmWarmupEntry
{
    ztmState         mState;
    stlInt64         mIoSlaveCount;
    ztmServerStatus  mServerStatus;
    knlEventListener mStartupEvent;
    knlEventListener mCheckpointEvent;
    knlEventListener mPageFlushEvent;
    knlEventListener mLogFlusherEvent;
    knlEventListener mArchivelogEvent;
    knlEventListener mIoSlaveEvent[SML_MAX_PARALLEL_IO_GROUP];
} ztmWarmupEntry;

/**
 * @brief Gliese Environment
 */
typedef struct ztmEnv
{
    sslEnv                  mEnv;               /**< Session Environment */
} ztmEnv;

typedef struct ztmSessionEnv
{
    sslSessionEnv           mSessionEnv;
} ztmSessionEnv;

/** @} */

typedef enum
{
    ZTM_THREAD_STATE_INIT,
    ZTM_THREAD_STATE_RUN,
    ZTM_THREAD_STATE_FAIL,
    ZTM_THREAD_STATE_MAX
} ztmThreadState;

typedef struct ztmThreadArgs
{
    stlErrorStack    mErrorStack;       /**< thread에서 발생된 error를 상위 process로 전달 하기 위함 */
    stlInt64         mThreadIndex;      /**< thread의 index, IoSlaveThread에서만 사용됨. */
} ztmThreadArgs;


typedef struct ztmThread
{
    ztmThreadState   mState;
    stlThread        mThread;
    void           * mEnv;
    ztmThreadArgs    mArgs;
} ztmThread;

#define ZTM_SET_THREAD_STATE( aThread, aFlag )  { (aThread)->mState = (aFlag); }
#define ZTM_IS_STARTED( aThread )               ( (aThread)->mState == ZTM_THREAD_STATE_RUN )
#define ZTM_IS_FAILED( aThread )                ( (aThread)->mState == ZTM_THREAD_STATE_FAIL )
#define ZTM_SET_ENV( aThread, aEnv )            { (aThread)->mEnv = (aEnv); }
#define ZTM_WAIT_THREAD( aThread )                      \
    do                                                  \
    {                                                   \
        if( (ZTM_IS_STARTED( aThread ) == STL_TRUE) ||  \
            (ZTM_IS_FAILED( aThread )  == STL_TRUE) )   \
        {                                               \
            break;                                      \
        }                                               \
        stlYieldThread();                               \
                                                        \
    } while( 1 );

extern stlBool gZtmSilentMode;

#define ZTM_PRINT( aString )             \
{                                        \
    if( gZtmSilentMode == STL_FALSE )    \
    {                                    \
        stlPrintf( aString );            \
    }                                    \
}

#define ZTM_AGER_SLEEP_TIME           ( 10 )                      /**< Ager Sleep Time(ms) */
#define ZTM_TIMER_SLEEP_TIME          ( STL_SET_SEC_TIME(1) )     /**< Timer Sleep Time(s) */
#define ZTM_CLEANUP_SLEEP_TIME        ( STL_SET_SEC_TIME(1) )     /**< cleanup Sleep Time(s) */
#define ZTM_MONITOR_SLEEP_TIME        ( STL_SET_SEC_TIME(1) )     /**< monitor Sleep Time(s) */
#define ZTM_SHRINK_UNDO_INTERVAL_TIME (10*1000)                   /**< 10 sec */


#define ZTM_THREAD_NAME_AGER                "ager"
#define ZTM_THREAD_NAME_CHECK_POINT         "checkpoint"
#define ZTM_THREAD_NAME_PAGE_FLUSH          "database writer"
#define ZTM_THREAD_NAME_IO_SLAVE            "i/o slave"
#define ZTM_THREAD_NAME_LOG_FLUSH           "log writer"
#define ZTM_THREAD_NAME_ARCHIVELOG          "archiver"
#define ZTM_THREAD_NAME_CLEANUP             "cleanup"
#define ZTM_THREAD_NAME_TIMER               "timer"
#define ZTM_THREAD_NAME_PROCESS_MONITOR     "monitor"


/** @} */

#endif /* _ZTMDEF_H_ */
