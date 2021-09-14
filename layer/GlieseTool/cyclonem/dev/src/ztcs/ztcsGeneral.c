/*******************************************************************************
 * ztcsGeneral.c
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

/**
 * @file ztcsGeneral.c
 * @brief GlieseTool Cyclone Master/Sender Communicator Routines
 */

#include <goldilocks.h>
#include <ztc.h>


stlStatus ztcsInitializeCM4Master( stlErrorStack * aErrorStack )
{
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;

//    sCMMgr->mSenderStatus  = STL_FALSE;
    sCMMgr->mMonitorStatus = STL_FALSE;
    
    sCMMgr->mSenderThread.mProc.mProcID = 0;
    sCMMgr->mPubThread.mProc.mProcID = 0;

    sCMMgr->mSendBufSize = ZTC_MAX_SEND_BUFF_SIZE;
    sCMMgr->mSendBufLen = 0;

    STL_TRY( stlAllocHeap( (void **)&sCMMgr->mSendBuf,
                           sCMMgr->mSendBufSize,
                           aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus ztcsFinalizeCM4Master( stlErrorStack * aErrorStack )
{
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;

    stlFreeHeap( sCMMgr->mSendBuf );
    
    return STL_SUCCESS;
}


/** @} */
