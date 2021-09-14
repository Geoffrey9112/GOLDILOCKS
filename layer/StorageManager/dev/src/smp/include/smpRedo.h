/*******************************************************************************
 * smpRedo.h
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


#ifndef _SMPREDO_H_
#define _SMPREDO_H_ 1

/**
 * @file smpRedo.h
 * @brief Page Access Redo Component Internal Routines
 */

/**
 * @defgroup smpRedo Page Access Redo
 * @ingroup smpInternal
 * @{
 */

stlStatus smpRedoInitPage( smlRid    * aDataRid,
                           void      * aData,
                           stlUInt16   aDataSize,
                           smpHandle * aPageHandle,
                           smlEnv    * aEnv );

stlStatus smpRedoInitPageBody( smlRid    * aDataRid,
                               void      * aData,
                               stlUInt16   aDataSize,
                               smpHandle * aPageHandle,
                               smlEnv    * aEnv );

stlStatus smpRedoInitFixedPageBody( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv );

stlStatus smpRedoBeginUseRts( smlRid    * aDataRid,
                              void      * aData,
                              stlUInt16   aDataSize,
                              smpHandle * aPageHandle,
                              smlEnv    * aEnv );

stlStatus smpRedoEndUseRts( smlRid    * aDataRid,
                            void      * aData,
                            stlUInt16   aDataSize,
                            smpHandle * aPageHandle,
                            smlEnv    * aEnv );

/** @} */
    
#endif /* _SMPREDO_H_ */
