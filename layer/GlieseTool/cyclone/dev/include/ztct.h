/*******************************************************************************
 * ztct.h
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


#ifndef _ZTCT_H_
#define _ZTCT_H_ 1

/**
 * @file ztct.h
 * @brief GlieseTool Cyclone Threads Routines
 */


/**
 * @defgroup ztct Cyclone Threads Routines
 * @{
 */

stlStatus ztctCreateCaptureThread( stlErrorStack * aErrorStack );

stlStatus ztctJoinCaptureThread( stlErrorStack * aErrorStack );

stlStatus ztctCreateDistbtThread( stlErrorStack * aErrorStack );

stlStatus ztctJoinDistbtThread( stlErrorStack * aErrorStack );

stlStatus ztctCreateSubDistbtThread( stlErrorStack * aErrorStack );

stlStatus ztctJoinSubDistbtThread( stlErrorStack * aErrorStack );

stlStatus ztctCreateApplierThread( stlErrorStack * aErrorStack );

stlStatus ztctJoinApplierThread( stlErrorStack * aErrorStack );

stlStatus ztctCreateThreadCM4Master( stlErrorStack * aErrorStack );

stlStatus ztctJoinThreadCM4Master( stlErrorStack * aErrorStack );

stlStatus ztctCreateThreadCM4Slave( stlErrorStack * aErrorStack );

stlStatus ztctJoinThreadCM4Slave( stlErrorStack * aErrorStack );

stlStatus ztctCreateSenderThread( stlErrorStack * aErrorStack );

stlStatus ztctCreateMonitorThread( stlErrorStack * aErrorStack );

stlStatus ztctCreateSyncherThread( ztcSyncher    * aSyncher,
                                   stlErrorStack * aErrorStack );

stlStatus ztctJoinSyncherThread( ztcSyncher    * aSyncher,
                                 stlErrorStack * aErrorStack );

stlStatus ztctCreateStatusThread( stlErrorStack * aErrorStack );

stlStatus ztctJoinStatusThread( stlErrorStack * aErrorStack );

/** @} */

/** @} */

#endif /* _ZTCT_H_ */
