/*******************************************************************************
 * stx.h
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


#ifndef _STX_H_
#define _STX_H_ 1

STL_BEGIN_DECLS

#define STX_MAX_PROCESS_ARGS        20

stlStatus stxInitThreadAttr( stlThreadAttr * aThreadAttr,
                             stlErrorStack * aErrorStack );

stlStatus stxSetThreadAttrStackSize( stlThreadAttr * aThreadAttr,
                                     stlSize         aStackSize,
                                     stlErrorStack * aErrorStack );

stlStatus stxCreateThread( stlThread     * aThread,
                           stlThreadAttr * aThreadAttr,
                           stlThreadFunc   aThreadFunc,
                           void          * aArgs,
                           stlErrorStack * aErrorStack );

stlBool stxIsEqualThread( stlThreadHandle * aThread1,
                          stlThreadHandle * aThread2 );

stlStatus stxThreadSelf( stlThreadHandle * aThreadHandle,
                         stlErrorStack   * aErrorStack );

stlStatus stxSetThreadAffinity( stlThreadHandle * aThreadHandle,
                                stlCpuSet       * aCpuSet,
                                stlErrorStack   * aErrorStack );

stlStatus stxExitThread( stlThread     * aThread,
                         stlStatus       aReturnStatus,
                         stlErrorStack * aErrorStack );

stlStatus stxCancelThread( stlThread     * aThread,
                           stlStatus       aReturnStatus,
                           stlErrorStack * aErrorStack );

void stxNativeExitThread();

stlStatus stxJoinThread( stlThread     * aThread,
                         stlStatus     * aReturnStatus,
                         stlErrorStack * aErrorStack );

void stxYieldThread( void );

stlStatus stxInitThreadOnce( stlThreadOnce * aThreadOnce,
                             stlErrorStack * aErrorStack );

stlStatus stxThreadOnce( stlThreadOnce * aThreadOnce, 
                         void         (* aInitFunc)(void),
                         stlErrorStack * aErrorStack );

stlStatus stxCreateProc( const stlChar * aPath,
                         stlChar * const aArgv[],
                         stlInt32 *      aExitCode,                                
                         stlErrorStack * aErrorStack );

stlStatus stxForkProc( stlProc       * aProc,
                       stlBool       * aIsChild,
                       stlErrorStack * aErrorStack );

stlStatus stxWaitProc( stlProc       * aProc,
                       stlInt32      * aExitCode,
                       stlErrorStack * aErrorStack );
stlStatus stxWaitAnyProc( stlInt32      * aExitCode,
                          stlErrorStack * aErrorStack );

stlStatus stxExecuteProc( const stlChar * aPath,
                          stlChar * const aArgv[],
                          stlErrorStack * aErrorStack );

stlStatus stxExecuteProcAndWait( stlChar        * const aArgv[],
                                 stlErrorStack  * aErrorStack );

stlStatus stxWakeupExecProc( stlInt64          aMsgKey,
                             stlErrorStack   * aErrorStack );

stlStatus stxKillProc( stlProc       * aProc,
                       stlInt32        aSignal,
                       stlErrorStack * aErrorStack );

stlStatus stxSetSignalHandler( stlInt32        aSignalNo,
                               stlSignalFunc   aNewFunc,
                               stlSignalFunc * aOldFunc,
                               stlErrorStack * aErrorStack );

stlStatus stxSetSignalHandlerEx( stlInt32          aSignalNo,
                                 stlSignalExFunc   aNewFunc,
                                 stlSignalExFunc * aOldFunc,
                                 stlErrorStack   * aErrorStack );

stlStatus stxBlockSignal( stlInt32        aSignalNo,
                          stlErrorStack * aErrorStack );

stlStatus stxUnblockSignal( stlInt32        aSignalNo,
                            stlErrorStack * aErrorStack );

stlStatus stxGetCurrentProc( stlProc       * aProc,
                             stlErrorStack * aErrorStack );

stlStatus stxGetParentProc( stlProc       * aProc,
                            stlErrorStack * aErrorStack );

stlStatus stxBlockThreadSignals( stlInt32      * aSignalNo,
                                 stlInt32        aSignalCount,
                                 stlErrorStack * aErrorStack );

stlStatus stxUnblockThreadSignals( stlInt32      * aSignalNo,
                                   stlInt32        aSignalCount,
                                   stlErrorStack * aErrorStack );

void stxGetSignalOriginProcess( void    * aSigInfo,
                                stlProc * aOriginProc );

stlBool stxIsExistProc( stlProc * aProc );

stlStatus stxGetProcName( stlProc       * aProc,
                          stlChar       * aName,
                          stlSize         aSize,
                          stlErrorStack * aErrorStack );

stlStatus stxHideArgument( stlInt32        aArgc,
                           stlChar       * aArgv[],
                           stlErrorStack * aErrorStack,
                           stlInt32        aCount,
                           va_list         aVarArgList );

STL_END_DECLS

#endif /* _STX_H_ */
