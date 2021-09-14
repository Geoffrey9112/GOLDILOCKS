/*******************************************************************************
 * knbBreakPoint.h
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


#ifndef _KNBBREAKPOINT_H_ 
#define _KNBBREAKPOINT_H_ 1

/**
 * @file knbBreakPoint.h
 * @brief Kernel Layer Break Point Internal Routines
 */

/**
 * @defgroup knbBreakPoint Break Point
 * @ingroup knInternal
 * @internal
 * @{
 */


stlStatus knbInitializeBpInfo( knbBreakPointInfo ** aBpInfo,
                               knlEnv             * aEnv );

stlStatus knbClearActionInfo( knlEnv   * aEnv );

stlStatus knbClearProcessInfo( knlEnv   * aEnv );

stlStatus knbRegisterProcess( stlChar  * aName,
                              knlEnv   * aEnv );

void knbFindProcIdFromName( stlChar         * aProcName,
                            stlInt32        * aProcId );

void knbFindBreakpointIdFromName( stlChar         * aBpName,
                                  knlBreakPointId * aBpId );

stlStatus knbRegisterAction( knlBreakPointId           aBpId,
                             knlBreakPointSessType     aSessType,
                             stlInt32                  aOwnerProcId,
                             stlInt32                  aSkipCount,
                             knlBreakPointAction       aActionType,
                             knlBreakPointPostAction   aPostActionType,
                             stlInt32                  aValue,
                             knlEnv                  * aEnv );

stlInt32  knbGetActionCount( );

stlInt32  knbGetProcCount( );

stlStatus knbDoBreakPoint( knlBreakPointId   aBpId,
                           knlEnv          * aEnv );

stlStatus knbBreakPointWaitToStart( knlEnv * aEnv );

/** @} */
    
#endif /* _KNBBREAKPOINT_H_ */
