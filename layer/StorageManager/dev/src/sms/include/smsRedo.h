/*******************************************************************************
 * smsRedo.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smsRedo.h 925 2011-06-02 11:32:26Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMSREDO_H_
#define _SMSREDO_H_ 1

/**
 * @file smsRedo.h
 * @brief Storage Manager Layer Segment Redo Component Internal Routines
 */

/**
 * @defgroup smsRedo Segment Redo
 * @ingroup smsInternal
 * @{
 */

stlStatus smsRedoInitSegHdr( smlRid    * aDataRid,
                             void      * aData,
                             stlUInt16   aDataSize,
                             smpHandle * aPageHandle,
                             smlEnv    * aEnv );

/** @} */
    
#endif /* _SMSREDO_H_ */
