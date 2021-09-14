/*******************************************************************************
 * smeRedo.h
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


#ifndef _SMEREDO_H_
#define _SMEREDO_H_ 1

/**
 * @file smeRedo.h
 * @brief Relation Redo Component Internal Routines
 */

/**
 * @defgroup smeRedo Relation Redo
 * @ingroup smeInternal
 * @{
 */

stlStatus smeRedoInitRelationHeader( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv );

stlStatus smeRedoColumnLength( smlRid    * aDataRid,
                               void      * aData,
                               stlUInt16   aDataSize,
                               smpHandle * aPageHandle,
                               smlEnv    * aEnv );

/** @} */
    
#endif /* _SMEREDO_H_ */
