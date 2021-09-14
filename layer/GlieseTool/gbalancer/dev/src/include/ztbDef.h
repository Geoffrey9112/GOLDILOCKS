/*******************************************************************************
 * ztbDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztbDef.h 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ZTBDEF_H_
#define _ZTBDEF_H_ 1

#include <knl.h>

/**
 * @file ztbDef.h
 * @brief Gliese Balancer Definitions
 */

/**
 * @defgroup  ztb Gliese Balancer
 * @{
 */
#define ZTBE_ERRCODE_OVERFLOW_FD                            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GBALANCER, 0 )

#define ZTBE_MAX_ERROR                      (1)

extern stlErrorRecord       gZtbeErrorTable[];


/**
 * @brief Balancer error code
 */


#define ZTB_MAX_CONTEXT_COUNT           (1000)                    /**< client fd관리 가능한 최대 개수 */
#define ZTB_FD_TIMEOUT                  (STL_SET_SEC_TIME(20))    /**< fd관리 timeout(ms) */
#define ZTB_HEAPMEM_CHUNK_SIZE          ( 1024 * 1024 )

typedef struct ztbmContext
{
    stlInt64            mIdx;                   /**< 자신의 index */
    stlInt64            mSequence;
    stlTime             mTime;
    stlContext          mContext;
    stlBool             mUsed;
    stlBool             mForwarding;            /**< fd를 전송중인지 여부 */
    stlInt16            mMajorVersion;          /**< client의 버전 */
    stlInt16            mMinorVersion;          /**< client의 버전 */
    stlInt16            mPatchVersion;          /**< client의 버전 */
} ztbmContext;

/**
 * balancer에서 관리하는 dispatcher 정보
 *
 */
typedef struct ztbmDispatcher
{
    stlChar             mUdsPath[STL_MAX_FILE_PATH_LENGTH];
    stlBool             mIsReady;                               /**< balancer가 판단하는 dispatcher의 준비 상태 */
    stlInt32            mSocketCount;                           /**< socket balance를 위한 socket 개수 */
} ztbmDispatcher;

/** @} */

#endif /* _ZTBDEF_H_ */
