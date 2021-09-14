/*******************************************************************************
 * smdRedo.h
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


#ifndef _SMDREDO_H_
#define _SMDREDO_H_ 1

/**
 * @file smdRedo.h
 * @brief Table Redo Component Internal Routines
 */

/**
 * @defgroup smdRedo Table Redo
 * @ingroup smdInternal
 * @{
 */

stlStatus smdRedoMemoryHeapInsert( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapUninsert( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapUpdate( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapUnupdate( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapMigrate( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapUnmigrate( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapDelete( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapUndelete( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapUpdateLink( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapInsertForUpdate( smlRid    * aDataRid,
                                            void      * aData,
                                            stlUInt16   aDataSize,
                                            smpHandle * aPageHandle,
                                            smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapDeleteForUpdate( smlRid    * aDataRid,
                                            void      * aData,
                                            stlUInt16   aDataSize,
                                            smpHandle * aPageHandle,
                                            smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapUpdateLogicalHdr( smlRid    * aDataRid,
                                             void      * aData,
                                             stlUInt16   aDataSize,
                                             smpHandle * aPageHandle,
                                             smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapAppend( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapUnappend( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv );

stlStatus smdRedoMemoryPendingInsert( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdRedoMemoryHeapCompaction( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv );

/** @} */
    
#endif /* _SMDREDO_H_ */
