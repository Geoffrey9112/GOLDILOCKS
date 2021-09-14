/*******************************************************************************
 * smsmpLog.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smsmpLog.h 1555 2011-07-28 07:30:00Z mycomman $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMSMPLOG_H_
#define _SMSMPLOG_H_ 1

/**
 * @file smsmpLog.h
 * @brief Storage Manager Layer Memory Pending Logging Component Internal Routines
 */

/**
 * @defgroup smsmpLog Memory Pending Segment Logging
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmpWriteLogUpdateSegHdr( smxmTrans  * aMiniTrans,
                                     smlRid       aSegRid,
                                     smsmpCache * aCache,
                                     smlEnv     * aEnv );
stlStatus smsmpWriteLogUpdateExtHdr( smxmTrans   * aMiniTrans,
                                     smlTbsId      aTbsId,
                                     smlPid        aPageId,
                                     smsmpExtHdr * aExtHdr,
                                     smlEnv      * aEnv );

/** @} */
    
#endif /* _SMSMPLOG_H_ */
