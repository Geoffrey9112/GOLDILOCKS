/*******************************************************************************
 * smtRedo.h
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


#ifndef _SMTREDO_H_
#define _SMTREDO_H_ 1

/**
 * @file smtRedo.h
 * @brief Storage Manager Layer Tablespace Redo Component Internal Routines
 */

/**
 * @defgroup smtRedo Tablespace Redo
 * @ingroup smtInternal
 * @{
 */

stlStatus smtRedoAllocExtent( smlRid    * aDataRid,
                              void      * aData,
                              stlUInt16   aDataSize,
                              smpHandle * aPageHandle,
                              smlEnv    * aEnv );
stlStatus smtRedoFreeExtent( smlRid    * aDataRid,
                             void      * aData,
                             stlUInt16   aDataSize,
                             smpHandle * aPageHandle,
                             smlEnv    * aEnv );
stlStatus smtRedoAddExtentBlock( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv );
stlStatus smtRedoCreateExtentBlock( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv );
stlStatus smtRedoCreateExtentBlockMap( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv );
stlStatus smtRedoUpdateExtBlockDescState( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smtRedoUpdateExtBlockMapHdrState( smlRid    * aDataRid,
                                            void      * aData,
                                            stlUInt16   aDataSize,
                                            smpHandle * aPageHandle,
                                            smlEnv    * aEnv );

/** @} */
    
#endif /* _SMTREDO_H_ */
