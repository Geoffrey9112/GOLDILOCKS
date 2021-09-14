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

extern ztcMasterMgr * gMasterMgr;
extern stlBool       gRunState;

stlStatus ztcsInitializeCM4Master( stlErrorStack * aErrorStack )
{
    ztcMasterCMMgr * sCMMgr = &gMasterMgr->mCMMgr;

    sCMMgr->mSenderStatus  = STL_FALSE;
    sCMMgr->mMonitorStatus = STL_FALSE;
    
    return STL_SUCCESS;
}


stlStatus ztcsFinalizeCM4Master( stlErrorStack * aErrorStack )
{
    
    return STL_SUCCESS;
}


/** @} */
