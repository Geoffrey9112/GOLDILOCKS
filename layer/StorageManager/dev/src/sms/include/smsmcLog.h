/*******************************************************************************
 * smsmbLog.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMSMBLOG_H_
#define _SMSMBLOG_H_ 1

/**
 * @file smsmbLog.h
 * @brief Storage Manager Layer Memory Bitmap Logging Component Internal Routines
 */

/**
 * @defgroup smsmbLog Memory Bitmap Segment Logging
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmcWriteLogUpdateSegHdr( smxmTrans  * aMiniTrans,
                                     smlRid       aSegRid,
                                     smsmcCache * aCache,
                                     smlEnv     * aEnv );
stlStatus smsmcWriteLogUpdateExtHdr( smxmTrans   * aMiniTrans,
                                     smlTbsId      aTbsId,
                                     smlPid        aPageId,
                                     smsmcExtHdr * aExtHdr,
                                     smlEnv      * aEnv );

/** @} */
    
#endif /* _SMSMBLOG_H_ */
