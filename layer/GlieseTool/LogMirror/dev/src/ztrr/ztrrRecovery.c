/*******************************************************************************
 * ztrrRecovery.c
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
 * @file ztrrRecovery.c
 * @brief GlieseTool LogMirror Receiver Recovery Routines
 */

#include <goldilocks.h>
#include <ztr.h>

extern stlBool       gRunState;
extern ztrSlaveMgr   gSlaveMgr;


stlStatus ztrrGetRecoveryInfo( ztrRecoveryinfo * aRecoveryInfo,
                               stlBool         * aIsExist,
                               stlErrorStack   * aErrorStack )
{
    ztrLogFileHdr    sLogFileHdr;
    stlBool          sIsExist = STL_FALSE;
    
    STL_TRY( ztrfReadControlFile( &sLogFileHdr,
                                  &sIsExist,
                                  aErrorStack ) == STL_SUCCESS );
    
    if( sIsExist == STL_TRUE )
    {
        aRecoveryInfo->mLogFileSeqNo = sLogFileHdr.mLogFileSeqNo;
        aRecoveryInfo->mLogGroupId   = sLogFileHdr.mLogGroupId;
                
        STL_TRY( ztrfGetLastBlockSeq( sLogFileHdr.mLogFileSeqNo,
                                      &aRecoveryInfo->mLastBlockSeq,
                                      aErrorStack ) == STL_SUCCESS );
        
        *aIsExist = STL_TRUE;        
    }
    else
    {
        *aIsExist = STL_FALSE;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}




/** @} */

