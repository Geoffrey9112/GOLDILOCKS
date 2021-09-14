/*******************************************************************************
 * smxlRedo.h
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


#ifndef _SMXLREDO_H_
#define _SMXLREDO_H_ 1

/**
 * @file smxlRedo.h
 * @brief Storage Manager Layer Local Transactional Redo Component Internal Routines
 */

/**
 * @defgroup smxlRedo Recovery Redo
 * @ingroup smxlInternal
 * @{
 */

stlStatus smxlRedoInsertUndoRecord( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv );
stlStatus smxlRedoInsertTransRecord( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv );
stlStatus smxlRedoUpdateUndoRecordFlag( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );
stlStatus smxlRedoCommit( smlRid    * aDataRid,
                          void      * aData,
                          stlUInt16   aDataSize,
                          smpHandle * aPageHandle,
                          smlEnv    * aEnv );
stlStatus smxlRedoRollback( smlRid    * aDataRid,
                            void      * aData,
                            stlUInt16   aDataSize,
                            smpHandle * aPageHandle,
                            smlEnv    * aEnv );
stlStatus smxlRedoPrepare( smlRid    * aDataRid,
                           void      * aData,
                           stlUInt16   aDataSize,
                           smpHandle * aPageHandle,
                           smlEnv    * aEnv );

/** @} */

#endif /* _SMXLREDO_H_ */
