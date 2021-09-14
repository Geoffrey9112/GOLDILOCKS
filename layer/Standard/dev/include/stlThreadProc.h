/*******************************************************************************
 * stlThreadProc.h
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


#ifndef _STLTHREADPROC_H_
#define _STLTHREADPROC_H_ 1

/**
 * @file stlThreadProc.h
 * @brief Standard Layer Thread & Process Routines
 */


STL_BEGIN_DECLS


/**
 * @defgroup stlThreadProc Thread & Process
 * @ingroup STL 
 * @{
 */

stlStatus stlInitThreadAttr( stlThreadAttr * aThreadAttr,
                             stlErrorStack * aErrorStack );

stlStatus stlSetThreadAttrStackSize( stlThreadAttr * aThreadAttr,
                                     stlSize         aStackSize,
                                     stlErrorStack * aErrorStack );

stlStatus stlCreateThread( stlThread     * aThread,
                           stlThreadAttr * aThreadAttr,
                           stlThreadFunc   aThreadFunc,
                           void          * aArgs,
                           stlErrorStack * aErrorStack );

stlBool stlIsEqualThread( stlThreadHandle * aThread1,
                          stlThreadHandle * aThread2 );

stlStatus stlExitThread( stlThread     * aThread,
                         stlStatus       aReturnStatus,
                         stlErrorStack * aErrorStack );

inline stlStatus stlCancelThread( stlThread     * aThread,
                                  stlStatus       aReturnStatus,
                                  stlErrorStack * aErrorStack );

stlStatus stlThreadSelf( stlThreadHandle * aThread,
                         stlErrorStack   * aErrorStack );

stlStatus stlSetThreadAffinity( stlThreadHandle * aThreadHandle,
                                stlCpuSet       * aCpuSet,
                                stlErrorStack   * aErrorStack );

void stlNativeExitThread();

stlStatus stlJoinThread( stlThread     * aThread,
                         stlStatus     * aReturnStatus,
                         stlErrorStack * aErrorStack );

void stlYieldThread( void );

stlStatus stlInitThreadOnce( stlThreadOnce * aThreadOnce,
                             stlErrorStack * aErrorStack );

stlStatus stlSetThreadOnce( stlThreadOnce * aThreadOnce, 
                            void         (* aInitFunc)(void),
                            stlErrorStack * aErrorStack );

stlStatus stlCreateProc( const stlChar * aPath,
                         stlChar * const aArgv[],
                         stlInt32 *      aExitCode,                                
                         stlErrorStack * aErrorStack );

stlStatus stlForkProc( stlProc       * aProc,
                       stlBool       * aIsChild,
                       stlErrorStack * aErrorStack );

stlStatus stlWaitProc( stlProc       * aProc,
                       stlInt32      * aExitCode,
                       stlErrorStack * aErrorStack );
inline stlStatus stlWaitAnyProc( stlInt32      * aExitCode,
                                 stlErrorStack * aErrorStack );

stlStatus stlExecuteProc( const stlChar * aPath,
                          stlChar * const aArgv[],
                          stlErrorStack * aErrorStack );

stlStatus stlExecuteProcAndWait( stlChar        * const aArgv[],
                                 stlErrorStack  * aErrorStack );

stlStatus stlWakeupExecProc( stlInt64          aMsgKey,
                             stlErrorStack   * aErrorStack );

stlBool stlIsEqualProc( stlProc * aProc1,
                        stlProc * aProc2 );

stlStatus stlExitProc( stlInt32 aStatus );

stlStatus stlGetCurrentProc( stlProc       * aProc,
                             stlErrorStack * aErrorStack );

stlStatus stlGetParentProc( stlProc       * aProc,
                            stlErrorStack * aErrorStack );

stlBool stlIsActiveProc( stlProc * aProc );

stlStatus stlGetProcName( stlProc       * aProc,
                          stlChar       * aName,
                          stlSize         aSize,
                          stlErrorStack * aErrorStack );

stlStatus stlHideArgument( stlInt32        aArgc,
                           stlChar       * aArgv[],
                           stlErrorStack * aErrorStack,
                           stlInt32        aCount,
                           ... );

/** @} */

STL_END_DECLS

#endif /* _STLTHREADPROC_H_ */
