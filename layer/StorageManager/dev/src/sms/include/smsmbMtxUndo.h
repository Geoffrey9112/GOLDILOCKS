/*******************************************************************************
 * smsmbMtxUndo.h
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


#ifndef _SMSMBMTXUNDO_H_
#define _SMSMBMTXUNDO_H_ 1

/**
 * @file smsmbMtxUndo.h
 * @brief Storage Manager Layer Memory Bitmap MtxUndo Component Internal Routines
 */

/**
 * @defgroup smsmbMtxUndo Memory Bitmap Segment Mini-Transaction Undo
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmbMtxUndoUpdateLeafStatus( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );
stlStatus smsmbMtxUndoUpdateInternalStatus( smlRid    * aDataRid,
                                            void      * aData,
                                            stlUInt16   aDataSize,
                                            smpHandle * aPageHandle,
                                            smlEnv    * aEnv );
stlStatus smsmbMtxUndoAddExt( smlRid    * aDataRid,
                              void      * aData,
                              stlUInt16   aDataSize,
                              smpHandle * aPageHandle,
                              smlEnv    * aEnv );

/** @} */
    
#endif /* _SMSMBMTXUNDO_H_ */
