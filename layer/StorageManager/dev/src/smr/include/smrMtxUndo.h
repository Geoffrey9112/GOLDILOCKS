/*******************************************************************************
 * smrMtxUndo.h
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


#ifndef _SMRMTXUNDO_H_
#define _SMRMTXUNDO_H_ 1

/**
 * @file smrMtxUndo.h
 * @brief Storage Manager Layer Recovery Mini-Transaction Undo Component Internal Routines
 */

/**
 * @defgroup smrMtxUndo Recovery Mini-Transaction Undo
 * @ingroup smrInternal
 * @{
 */

stlStatus smrMtxUndoBytes( smlRid    * aDataRid,
                           void      * aData,
                           stlUInt16   aDataSize,
                           smpHandle * aPageHandle,
                           smlEnv    * aEnv );

/** @} */

#endif /* _SMRMTXUNDO_H_ */
