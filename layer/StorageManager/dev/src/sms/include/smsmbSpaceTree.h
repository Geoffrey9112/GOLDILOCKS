/*******************************************************************************
 * smsmbSpaceTree.h
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


#ifndef _SMSMBSPACETREE_H_
#define _SMSMBSPACETREE_H_ 1

/**
 * @file smsmbSpaceTree.h
 * @brief Storage Manager Layer Memory Bitmap Space Tree Component Internal Routines
 */

/**
 * @defgroup smsmbSpaceTree Memory Bitmap Space Tree
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmbPropagateLeaf( smxmTrans  * aMiniTrans,
                              smlTbsId     aTbsId,
                              smsmbCache * aCache,
                              smlPid       aNewPageId,
                              smlRid     * aLeafRid,
                              smlEnv     * aEnv );
stlStatus smsmbExpandLeaf( smxmTrans  * aMiniTrans,
                           smlTbsId     aTbsId,
                           smsmbCache * aCache,
                           smpHandle  * aPageHandle,
                           smlEnv     * aEnv );
stlStatus smsmbPropagateInternal( smxmTrans  * aMiniTrans,
                                  smlTbsId     aTbsId,
                                  smsmbCache * aCache,
                                  stlInt16     aDepth,
                                  smlPid       aNewPageId,
                                  smlRid     * aPropagatedRid,
                                  smlEnv     * aEnv );
stlStatus smsmbExpandInternal( smxmTrans  * aMiniTrans,
                               smlTbsId     aTbsId,
                               smsmbCache * aCache,
                               stlInt16     aDepth,
                               smpHandle  * aPageHandle,
                               smlEnv     * aEnv );
stlStatus smsmbAllocLeaf( smxmTrans  * aMiniTrans,
                          smlTbsId     aTbsId,
                          smlPid       aLastLeafPid,
                          smsmbCache * aCache,
                          smpHandle  * aPageHandle,
                          smlEnv     * aEnv );
stlStatus smsmbAllocInternal( smxmTrans  * aMiniTrans,
                              smlTbsId     aTbsId,
                              smsmbCache * aCache,
                              stlInt16     aDepth,
                              smpHandle  * aPageHandle,
                              smlEnv     * aEnv );
stlStatus smsmbFindInsertablePage( smxmTrans       * aMiniTrans,
                                   smlTbsId          aTbsId,
                                   smsmbCache      * aCache,
                                   smpFreeness       aFreeness,
                                   smlPid            aStartPid,
                                   stlBool           aUseInsertablePageHint,
                                   smpIsAgableFunc   aIsAgableFunc,
                                   stlInt32        * aDataPidCount,
                                   smlPid          * aDataPageId,
                                   smlRid          * aLeafRid,        
                                   smlEnv          * aEnv );
stlStatus smsmbFindFreePage( smxmTrans       * aMiniTrans,
                             smlTbsId          aTbsId,
                             smlPid            aStartPid,
                             stlBool           aUseInsertableHint,
                             smpIsAgableFunc   aIsAgableFunc,
                             smlPid          * aDataPageId,
                             smlRid          * aLeafRid,
                             smlEnv          * aEnv );
stlStatus smsmbInsertInternalBit( smxmTrans   * aMiniTrans,
                                  smlTbsId      aTbsId,
                                  smsmbCache  * aCache,
                                  smpHandle   * aPageHandle,
                                  stlInt16      aDepth,
                                  smlPid        aInsertPid,
                                  smpFreeness   aInsertFreeness,
                                  smlEnv      * aEnv );

stlStatus smsmbFindChildAndUpdateHintInternal( smlTbsId      aTbsId,
                                               smpHandle   * aPageHandle,
                                               smpFreeness   aFreeness,
                                               stlInt16      aChildCount,
                                               smlPid      * aChildArray,
                                               stlChar     * aFreenessArray,
                                               stlBool       aUseInsertablePageHint,
                                               smlPid      * aChildPid,
                                               smlRid      * aBitRid,
                                               smlEnv      * aEnv );

stlStatus smsmbFindChildAndUpdateHintLeaf( smlTbsId          aTbsId,
                                           smpHandle       * aPageHandle,
                                           smpFreeness       aFreeness,
                                           stlInt16          aChildCount,
                                           smlPid          * aChildArray,
                                           stlChar         * aFreenessArray,
                                           smlScn          * aScnArray,
                                           stlBool           aUseInsertablePageHint,
                                           smpIsAgableFunc   aIsAgableFunc,
                                           stlInt32        * aChildPidCount,
                                           smlPid          * aChildPid,
                                           smlRid          * aBitRid,
                                           smlEnv          * aEnv );

stlStatus smsmbFindFirstFreePage( smlTbsId          aTbsId,
                                  smpHandle       * aPageHandle,
                                  stlInt16          aChildCount,
                                  smlPid          * aChildArray,
                                  stlChar         * aFreenessArray,
                                  smlScn          * aScnArray,
                                  stlBool           aUseInsertableHint,
                                  smpIsAgableFunc   aIsAgableFunc,
                                  smlPid          * aChildPid,
                                  smlRid          * aBitRid,
                                  smlEnv          * aEnv );

/** @} */
    
#endif /* _SMSMBSPACETREE_H_ */
