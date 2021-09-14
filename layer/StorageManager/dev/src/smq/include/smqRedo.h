/*******************************************************************************
 * smqRedo.h
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


#ifndef _SMQREDO_H_
#define _SMQREDO_H_ 1

/**
 * @file smqRedo.h
 * @brief Sequence Redo Component Internal Routines
 */

/**
 * @defgroup smqRedo Sequence Redo
 * @ingroup smqInternal
 * @{
 */

stlStatus smqRedoUpdateSeqValue( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv );

/** @} */
    
#endif /* _SMQREDO_H_ */
