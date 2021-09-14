/*******************************************************************************
 * smsmbExtMap.h
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


#ifndef _SMSMBEXTMAP_H_
#define _SMSMBEXTMAP_H_ 1

/**
 * @file smsmbExtMap.h
 * @brief Storage Manager Layer Memory Bitmap Extent Map Component Internal Routines
 */

/**
 * @defgroup smsmbExtMap Memory Bitmap Extent Map
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmbAllocNewExtent( smxlTransId   aTransId,
                               smlTbsId      aTbsId,
                               smsmbCache  * aCache,
                               smlPid      * aFirstDataPid,
                               smlEnv      * aEnv );
stlStatus smsmbInitExtMap( smxmTrans  * aMiniTrans,
                           smlTbsId     aTbsId,
                           smpHandle  * aPageHandle,
                           smlPid       aPrvMapPid,
                           smlEnv     * aEnv );
stlStatus smsmbAddExtent( smxmTrans  * aMiniTrans,
                          smlTbsId     aTbsId,
                          smpHandle  * aPageHandle,
                          smlExtId     aExtId,
                          smlEnv     * aEnv );

/** @} */
    
#endif /* _SMSMBEXTMAP_H_ */
