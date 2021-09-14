/*******************************************************************************
 * smfRedo.h
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


#ifndef _SMFREDO_H_
#define _SMFREDO_H_ 1

/**
 * @file smfRedo.h
 * @brief Storage Manager Layer Datafile Redo Internal Routines
 */

/**
 * @defgroup smfRedo Datafile Redo
 * @ingroup smInternal
 * @{
 */

stlStatus smfRedoMemoryFileCreate( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv );

stlStatus smfRedoMemoryFileCreateClr( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDatasize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smfRedoMemoryFileDrop( smlRid    * aDataRid,
                                 void      * aData,
                                 stlUInt16   aDataSize,
                                 smpHandle * aPageHandle,
                                 smlEnv    * aEnv );

stlStatus smfRedoMemoryTbsCreate( smlRid    * aDataRid,
                                  void      * aData,
                                  stlUInt16   aDataSize,
                                  smpHandle * aPageHandle,
                                  smlEnv    * aEnv );

stlStatus smfRedoMemoryTbsCreateClr( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv );

stlStatus smfRedoMemoryTbsDrop( smlRid    * aDataRid,
                                void      * aData,
                                stlUInt16   aDataSize,
                                smpHandle * aPageHandle,
                                smlEnv    * aEnv );

stlStatus smfRedoMemoryTbsDropClr( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv );

stlStatus smfRedoMemoryDatafileAdd( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv );

stlStatus smfRedoMemoryDatafileAddClr( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv );

stlStatus smfRedoMemoryDatafileDrop( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv );

stlStatus smfRedoMemoryDatafileDropClr( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smfRedoMemoryTbsRename( smlRid    * aDataRid,
                                  void      * aData,
                                  stlUInt16   aDataSize,
                                  smpHandle * aPageHandle,
                                  smlEnv    * aEnv );

stlStatus smfRedoMemoryTbsRenameClr( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv );

/** @} */
    
#endif /* _SMFREDO_H_ */
