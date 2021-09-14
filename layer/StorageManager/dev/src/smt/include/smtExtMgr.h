/*******************************************************************************
 * smtExtMgr.h
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


#ifndef _SMTEXTMGR_H_
#define _SMTEXTMGR_H_ 1

/**
 * @file smtExtMgr.h
 * @brief Storage Manager Layer Extent Manager Component Internal Routines
 */

/**
 * @defgroup smtExtMgr Extent Manager
 * @ingroup smtInternal
 * @{
 */

void smtOnExtBit( stlChar * aBitArray, stlUInt16 aOffset );
void smtOffExtBit( stlChar * aBitArray, stlUInt16 aOffset );
stlBool smtIsFreeExt( stlChar * aBitArray, stlUInt16 aOffset );
stlStatus smtAddExtBlock( smlStatement * aStatement,
                          smlTbsId       aTbsId,
                          smlPid         aExtBlockPid,
                          smlEnv       * aEnv );
stlStatus smtCreateExtBlock( smxlTransId   aTransId,
                             smlTbsId      aTbsId,
                             smlPid        aExtBlockPid,
                             stlUInt32     aExtCount,
                             stlUInt32     aMaxExtCount,
                             smlEnv      * aEnv );
stlStatus smtCreateExtBlockMap( smxlTransId   aTransId,
                                smlTbsId      aTbsId,
                                smlPid        aExtBlockMapPid,
                                smlEnv      * aEnv );
smtHint * smtFindTbsHint( smlTbsId   aTbsId,
                          smlEnv   * aEnv );
void smtAddNewTbsHint( smlPid     aHintPid,
                       smlTbsId   aTbsId,
                       smlEnv   * aEnv );
stlStatus smtFindExtBlockMap( smxmTrans * aMiniTrans,
                              smlTbsId    aTbsId,
                              smlPid    * aExtMapPid,
                              smlEnv    * aEnv );
stlStatus smtUpdateExtBlockDescState( smxmTrans   * aMiniTrans,
                                      smlTbsId      aTbsId,
                                      smlPid        aExtMapPid,
                                      stlUInt32     aState,
                                      stlBool     * aIsSuccess,
                                      smlEnv      * aEnv );
stlStatus smtUpdateExtBlockMapHdrState( smxmTrans   * aMiniTrans,
                                        smlTbsId      aTbsId,
                                        stlUInt32     aDatafileId,
                                        stlUInt32     aState,
                                        stlBool     * aIsSuccess,
                                        stlBool     * aIsRetry,
                                        smlEnv      * aEnv );
stlStatus smtFindExtMap( smxmTrans     * aMiniTrans,
                         smlTbsId        aTbsId,
                         smlDatafileId   aDatafileId,
                         smlPid        * aExtMapPid,
                         smlEnv        * aEnv );
stlStatus smtExtendTablespace( smxmTrans  * aMiniTrans,
                               smlTbsId     aTbsId,
                               smlPid     * aExtMapPid,
                               smlEnv     * aEnv );

stlStatus smtValidateExtentMap( smpHandle * aPageHandle,
                                smlEnv    * aEnv );

/** @} */
    
#endif /* _SMTEXTMGR_H_ */
