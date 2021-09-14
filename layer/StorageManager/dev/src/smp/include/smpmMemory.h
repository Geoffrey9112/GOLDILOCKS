/*******************************************************************************
 * smpmMemory.h
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


#ifndef _SMPMMEMORY_H_
#define _SMPMMEMORY_H_ 1

/**
 * @file smpmMemory.h
 * @brief Memory Page Access Component Internal Routines
 */

/**
 * @defgroup smpmMemory Memory Page Access
 * @ingroup smpInternal
 * @{
 */

stlStatus smpmInit( smlTbsId        aTbsId,
                    smlDatafileId   aDatafileId,
                    smlEnv        * aEnv );

stlStatus smpmExtend( smlStatement  * aStatement,
                      smlTbsId        aTbsId,
                      smlDatafileId   aDatafileId,
                      stlBool         aNeedPending,
                      smlEnv        * aEnv );

stlStatus smpmShrink( smlStatement  * aStatement,
                      smlTbsId        aTbsId,
                      smlDatafileId   aDatafileId,
                      stlInt16        aPchArrayState,
                      smlEnv        * aEnv );

stlStatus smpmShrinkPending( stlUInt32   aActionFlag,
                             smgPendOp * aPendOp,
                             smlEnv    * aEnv );

stlStatus smpmDiscardDirtyPages( smpPchArrayList * aPchArrayList,
                                 smlEnv          * aEnv );

stlStatus smpmCreate( smxmTrans  * aMiniTrans,
                      smlTbsId     aTbsId,
                      smlPid       aPageId,
                      smpPageType  aPageType,
                      smpHandle  * aPageHandle,
                      smlEnv     * aEnv );

stlStatus smpmFix( smlTbsId     aTbsId,
                   smlPid       aPageId,
                   smpHandle  * aPageHandle,
                   smlEnv     * aEnv );

/** @} */
    
#endif /* _SMPMMEMORY_H_ */
