/*******************************************************************************
 * sllCommunication.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *
 * NOTES
 *
 *
 ******************************************************************************/

#include <stl.h>
#include <stlStrings.h>
#include <cml.h>
#include <scl.h>
#include <sll.h>

/**
 * @file sllCommunication.c
 * @brief Server Library functions
 */

/**
 * @defgroup sllCommunication   communication network
 * @addtogroup sll
 * @{
 */



/**
 * @brief sllHandle을 초기화 한다.
 * @param[in] aSllHandle    sllHandle
 * @param[in] aContext       socket Context
 * @param[in] aIpc           ipc
 * @param[in] aClientAddr    client ip address
 * @param[in] aUserContext   user Context
 * @param[in] aMajorVersion  protocol major version
 * @param[in] aMinorVersion  protocol minor version
 * @param[in] aPatchVersion  protocol patch version
 * @param[in] aEnv           env
 * @remark : aContext, aIpc는 둘중에 하나만 설정되어야 함.
 */
stlStatus sllInitializeHandle( sllHandle        * aSllHandle,
                               stlContext       * aContext,
                               sclIpc           * aIpc,
                               stlSockAddr      * aClientAddr,
                               void             * aUserContext,
                               stlInt16           aMajorVersion,
                               stlInt16           aMinorVersion,
                               stlInt16           aPatchVersion,
                               sllEnv           * aEnv )
{
    aSllHandle->mSessionId = SSL_INVALID_SESSION_ID;
    aSllHandle->mSessionSeq = -1;
    aSllHandle->mDoneHandshake = STL_FALSE;

    stlMemset( aSllHandle->mRoleName, 0x00, STL_SIZEOF(aSllHandle->mRoleName) );

    stlMemcpy( &aSllHandle->mSclHandle.mClientAddr, aClientAddr, STL_SIZEOF(stlSockAddr) );

    aSllHandle->mUserContext = aUserContext;

    aSllHandle->mConnectStatus = CML_CONN_STATUS_INIT;

    STL_TRY( sclInitializeHandle( SCL_HANDLE(aSllHandle),
                                  aContext,
                                  aIpc,
                                  aMajorVersion,
                                  aMinorVersion,
                                  aPatchVersion,
                                  SCL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief sllHandle을 삭제한다.
 * @param[in] aSllHandle    sllHandle
 * @param[in] aEnv          env
 */
stlStatus sllFinalizeHandle( sllHandle    * aSllHandle,
                             sllEnv       * aEnv )
{
    return sclFinalizeHandle( SCL_HANDLE(aSllHandle),
                              SCL_ENV(aEnv) );
}

/**
 * @brief sllProtocolSentence을 초기화 한다.
 * @param[in] aSllHandle            sllHandle
 * @param[in] aProtocolSentence     sllProtocolSentence
 * @param[in] aMemoryMgr            memory manager
 * @param[in] aEnv                  env
 * @remark : da, client, dedicate 등에서 사용됨.
 */
stlStatus sllInitializeProtocolSentence( sllHandle               * aSllHandle,
                                         sllProtocolSentence     * aProtocolSentence,
                                         sclMemoryManager        * aMemoryMgr,
                                         sllEnv                  * aEnv )
{
    aProtocolSentence->mCachedStmtId = -1;
    aProtocolSentence->mCloseDatabase = STL_FALSE;

    return sclInitializeProtocolSentence( &aSllHandle->mSclHandle,
                                          &aProtocolSentence->mSclProtocolSentence,
                                          aMemoryMgr,
                                          SCL_ENV(aEnv) );
}


/**
 * @brief ProtocolSentence을 삭제한다.
 * @param[in] aProtocolSentence     sllProtocolSentence
 * @param[in] aEnv                  env
 */
stlStatus sllFinalizeProtocolSentence( sllProtocolSentence     * aProtocolSentence,
                                       sllEnv                   * aEnv )
{
    return sclFinalizeProtocolSentence( &aProtocolSentence->mSclProtocolSentence,
                                        SCL_ENV(aEnv) );
}

/** @} sllCommunication */

