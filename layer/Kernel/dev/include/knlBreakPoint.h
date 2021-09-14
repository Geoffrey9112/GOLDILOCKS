/*******************************************************************************
 * knlBreakPoint.h
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


#ifndef _KNLBREAKPOINT_H_
#define _KNLBREAKPOINT_H_ 1

/**
 * @file knlBreakPoint.h
 * @brief Kernel Layer Break Point Routines
 */

/**
 * @defgroup knlBreakPoint Break Point
 * @ingroup knExternal
 * @{
 */

stlStatus knlClearProcessInfo( knlEnv   * aEnv );

stlStatus knlClearActionInfo( knlEnv   * aEnv );

stlStatus knlRegisterProcess( stlChar  * aName,
                                knlEnv   * aEnv );

void knlFindProcIdFromName( stlChar         * aProcName,
                            stlInt32        * aProcId );

void knlFindBreakpointIdFromName( stlChar         * aBpName,
                                  knlBreakPointId * aBpId );

stlStatus knlRegisterAction( knlBreakPointId           aBpId,
                             knlBreakPointSessType     aSessType,
                             stlInt32                  aOwnerProcId,
                             stlInt32                  aSkipCount,
                             knlBreakPointAction       aActionType,
                             knlBreakPointPostAction   aPostActionType,
                             stlInt32                  aValue,
                             knlEnv                  * aEnv );

stlInt32  knlGetActionCount( );

stlInt32  knlGetProcCount( );

stlStatus knlDoBreakPoint( knlBreakPointId   aBpId,
                           knlEnv          * aEnv );

stlStatus knlBreakPointWaitToStart( knlEnv * aEnv );

/** @} */
    
#endif /* _KNLBREAKPOINT_H_ */
