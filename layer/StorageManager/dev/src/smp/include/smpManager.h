/*******************************************************************************
 * smpManager.h
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


#ifndef _SMPMANAGER_H_
#define _SMPMANAGER_H_ 1

/**
 * @file smpManager.h
 * @brief Memory Page Access Component Internal Routines
 */

/**
 * @defgroup smpManager Page Access Manager
 * @ingroup smpInternal
 * @{
 */

stlStatus smpInitPch( smlTbsId          aTbsId,
                      smlPid            aPid,
                      stlUInt32         aDeviceType,
                      knlLogicalAddr    aFrame,
                      smpCtrlHeader   * aPch,
                      smlEnv          * aEnv );

stlStatus smpNeedFlushPchArray( smpPchArrayList * aPchArrayList,
                                stlBool         * aNeedFlush,
                                smlEnv          * aEnv );

void smpGetPchArray( smlTbsId            aTbsId,
                     smlDatafileId       aDatafileId,
                     smpPchArrayList  ** aPchArray );

/** @} */
    
#endif /* _SMPMANAGER_H_ */
