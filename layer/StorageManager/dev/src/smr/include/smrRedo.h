/*******************************************************************************
 * smrRedo.h
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


#ifndef _SMRREDO_H_
#define _SMRREDO_H_ 1

/**
 * @file smrRedo.h
 * @brief Storage Manager Layer Recovery Redo Component Internal Routines
 */

/**
 * @defgroup smrRedo Recovery Redo
 * @ingroup smrInternal
 * @{
 */

stlStatus smrRedo( smrLogCursor * aCursor,
                   smlEnv       * aEnv );
stlStatus smrAnalyzeLog( smrLogCursor * aCursor,
                         stlBool      * aIsChkptEnd,
                         smrTrans     * aTransTable,
                         smlEnv       * aEnv );
stlStatus smrRedoPage( smrLogHdr      * aLogHdr,
                       smrLogPieceHdr * aLogPieceHdr,
                       stlChar        * aLogPieceBody,
                       smrLsn           aLsn,
                       stlInt16         aLpsn,
                       smlEnv         * aEnv );
stlStatus smrRedoUndoSegment( smrLogHdr      * aLogHdr,
                              smrLogPieceHdr * aLogPieceHdr,
                              stlChar        * aLogPieceBody,
                              smrLsn           aLsn,
                              stlInt16         aLpsn,
                              smlEnv         * aEnv );
stlStatus smrRedoTransaction( smrLogHdr      * aLogHdr,
                              smrLogPieceHdr * aLogPieceHdr,
                              stlChar        * aLogPieceBody,
                              smrLsn           aLsn,
                              stlInt16         aLpsn,
                              smlEnv         * aEnv );
stlStatus smrRedoCtrl( smrLogPieceHdr * aLogPieceHdr,
                       stlChar        * aLogPieceBody,
                       smrLsn           aLsn,
                       smlEnv         * aEnv );
stlStatus smrRedoTest( smrLogPieceHdr * aLogPieceHdr,
                       stlChar        * aLogPieceBody,
                       smrLsn           aLsn,
                       smlEnv         * aEnv );
stlStatus smrRedoBytes( smlRid    * aDataRid,
                        void      * aData,
                        stlUInt16   aDataSize,
                        smpHandle * aPageHandle,
                        smlEnv    * aEnv );

stlStatus smrRedoDummy( smlRid    * aDataRid,
                        void      * aData,
                        stlUInt16   aDataSize,
                        smpHandle * aPageHandle,
                        smlEnv    * aEnv );

stlStatus smrRedoChkptBegin( smlRid    * aDataRid,
                             void      * aData,
                             stlUInt16   aDataSize,
                             smpHandle * aPageHandle,
                             smlEnv    * aEnv );

stlStatus smrRedoChkptBody( smlRid    * aDataRid,
                            void      * aData,
                            stlUInt16   aDataSize,
                            smpHandle * aPageHandle,
                            smlEnv    * aEnv );

stlStatus smrRedoChkptEnd( smlRid    * aDataRid,
                           void      * aData,
                           stlUInt16   aDataSize,
                           smpHandle * aPageHandle,
                           smlEnv    * aEnv );

stlStatus smrAnalyzeChkptBody( void      * aData,
                               stlUInt16   aDataSize,
                               smrTrans  * aTransTable,
                               smlEnv    * aEnv );

stlBool smrIsCheckpointLog( smrLogCursor * aCursor,
                            smlEnv       * aEnv );

/** @} */

#endif /* _SMRREDO_H_ */
