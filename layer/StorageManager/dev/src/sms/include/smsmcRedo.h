/*******************************************************************************
 * smsmcRedo.h
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


#ifndef _SMSMCREDO_H_
#define _SMSMCREDO_H_ 1

/**
 * @file smsmcRedo.h
 * @brief Storage Manager Layer Memory Circular Redo Component Internal Routines
 */

/**
 * @defgroup smsmcRedo Memory Circular Segment Redo
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmcRedoUpdateSegHdr( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv );
stlStatus smsmcRedoUpdateExtHdr( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv );

/** @} */
    
#endif /* _SMSMCREDO_H_ */
