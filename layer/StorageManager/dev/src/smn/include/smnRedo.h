/*******************************************************************************
 * smnRedo.h
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


#ifndef _SMNREDO_H_
#define _SMNREDO_H_ 1

/**
 * @file smnRedo.h
 * @brief Index Redo Component Internal Routines
 */

/**
 * @defgroup smnRedo Index Redo
 * @ingroup smnInternal
 * @{
 */

stlStatus smnRedoMemoryBtreeInsertLeaf( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeDupKey( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeUnwindDupKey( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeInsertIntl( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeDeleteLeaf( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeUndeleteLeaf( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeDeleteIntl( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smnRedoSetRootPid( smlRid    * aDataRid,
                             void      * aData,
                             stlUInt16   aDataSize,
                             smpHandle * aPageHandle,
                             smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeNewRoot( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeSplitNewPage( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeSplitOldPage( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeSetPrevPageId( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeSetNextPageId( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeAgingCompact( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeSetChildPid( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeSetNextFreePage( smlRid    * aDataRid,
                                             void      * aData,
                                             stlUInt16   aDataSize,
                                             smpHandle * aPageHandle,
                                             smlEnv    * aEnv );

stlStatus smnRedoAddEmptyNode( smlRid    * aDataRid,
                               void      * aData,
                               stlUInt16   aDataSize,
                               smpHandle * aPageHandle,
                               smlEnv    * aEnv );

stlStatus smnRedoRemoveEmptyNode( smlRid    * aDataRid,
                                  void      * aData,
                                  stlUInt16   aDataSize,
                                  smpHandle * aPageHandle,
                                  smlEnv    * aEnv );

stlStatus smnRedoPageImage( smlRid    * aDataRid,
                            void      * aData,
                            stlUInt16   aDataSize,
                            smpHandle * aPageHandle,
                            smlEnv    * aEnv );

stlStatus smnRedoMemoryBtreeSetNodeFlag( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv );

stlStatus smnRedoAlterDatatype( smlRid    * aDataRid,
                                void      * aData,
                                stlUInt16   aDataSize,
                                smpHandle * aPageHandle,
                                smlEnv    * aEnv );

stlStatus smnRedoSetMirrorRootPid( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv );

/** @} */

#endif /* _SMDREDO_H_ */
