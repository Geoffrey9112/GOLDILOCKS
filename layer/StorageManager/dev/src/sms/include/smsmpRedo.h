/*******************************************************************************
 * smsmpRedo.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smsmpRedo.h 1551 2011-07-28 03:36:30Z mycomman $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMSMPREDO_H_
#define _SMSMPREDO_H_ 1

/**
 * @file smsmpRedo.h
 * @brief Storage Manager Layer Memory Pending Redo Component Internal Routines
 */

/**
 * @defgroup smsmpRedo Memory Pending Segment Redo
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmpRedoUpdateSegHdr( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv );
stlStatus smsmpRedoUpdateExtHdr( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv );

/** @} */
    
#endif /* _SMSMPREDO_H_ */
