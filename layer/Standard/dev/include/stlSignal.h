/*******************************************************************************
 * stlSignal.h
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


#ifndef _STLSIGNAL_H_
#define _STLSIGNAL_H_ 1

/**
 * @file stlSignal.h
 * @brief Standard Layer Signal Routines
 */


STL_BEGIN_DECLS


/**
 * @defgroup stlSignal Signal
 * @ingroup STL 
 * @{
 */

stlStatus stlKillProc( stlProc       * aProc,
                       stlInt32        aSignal,
                       stlErrorStack * aErrorStack );

stlStatus stlSetSignalHandler( stlInt32        aSignalNo,
                               stlSignalFunc   aNewFunc,
                               stlSignalFunc * aOldFunc,
                               stlErrorStack * aErrorStack );

stlStatus stlSetSignalHandlerEx( stlInt32          aSignalNo,
                                 stlSignalExFunc   aNewFunc,
                                 stlSignalExFunc * aOldFunc,
                                 stlErrorStack   * aErrorStack );

stlStatus stlBlockSignal( stlInt32        aSignalNo,
                          stlErrorStack * aErrorStack );

stlStatus stlUnblockSignal( stlInt32        aSignalNo,
                            stlErrorStack * aErrorStack );

stlStatus stlBlockControllableSignals( stlInt32        aExclSignalNo,
                                       stlErrorStack * aErrorStack );
stlStatus stlUnblockControllableSignals( stlInt32        aExclSignalNo,
                                         stlErrorStack * aErrorStack );

stlStatus stlBlockThreadSignals( stlInt32      * aSignalNo,
                                 stlInt32        aSignalCount,
                                 stlErrorStack * aErrorStack );

stlStatus stlUnblockThreadSignals( stlInt32      * aSignalNo,
                                   stlInt32        aSignalCount,
                                   stlErrorStack * aErrorStack );

stlStatus stlBlockControllableThreadSignals( stlErrorStack * aErrorStack );
stlStatus stlUnblockControllableThreadSignals( stlErrorStack * aErrorStack );

stlStatus stlBlockRealtimeThreadSignals( stlErrorStack * aErrorStack );
stlStatus stlUnblockRealtimeThreadSignals( stlErrorStack * aErrorStack );
void stlGetSignalOriginProcess( void * aSigInfo, stlProc * aOriginProc );

/** @} */
    
STL_END_DECLS

#endif /* _STLSIGNAL_H_ */
