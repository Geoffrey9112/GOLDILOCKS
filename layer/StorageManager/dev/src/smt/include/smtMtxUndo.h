/*******************************************************************************
 * smtMtxUndo.h
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


#ifndef _SMTMTXUNDO_H_
#define _SMTMTXUNDO_H_ 1

/**
 * @file smtMtxUndo.h
 * @brief Storage Manager Layer Tablespace Mini-Transaction Undo Component Internal Routines
 */

/**
 * @defgroup smtMtxUndo Tablespace Mini-Transaction Undo
 * @ingroup smtInternal
 * @{
 */

stlStatus smtMtxUndoAllocExtent( smlRid    * aDataRid,
                              void      * aData,
                              stlUInt16   aDataSize,
                              smpHandle * aPageHandle,
                              smlEnv    * aEnv );
stlStatus smtMtxUndoAddExtentBlock( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv );
stlStatus smtMtxUndoUpdateExtBlockDescState( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

/** @} */
    
#endif /* _SMTMTXUNDO_H_ */
