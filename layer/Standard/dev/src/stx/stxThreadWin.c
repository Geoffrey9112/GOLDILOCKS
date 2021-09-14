/*******************************************************************************
 * stxThreadWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stxThreadUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlLog.h>
#include <stt.h>

#if STC_HAVE_PROCESS_H
#include <process.h>
#endif

DWORD gStxTls = 0;

stlStatus stxInitThreadAttr( stlThreadAttr * aThreadAttr,
                             stlErrorStack * aErrorStack )
{
    return STL_SUCCESS;
}

stlStatus stxSetThreadAttrStackSize( stlThreadAttr * aThreadAttr,
                                     stlSize         aStackSize,
                                     stlErrorStack * aErrorStack )
{
    aThreadAttr->mStackSize = aStackSize;

    return STL_SUCCESS;
}

void * stxDummyWorker( void * aOpaque )
{
    stlThread * sThread;

    sThread = (stlThread *)aOpaque;

    TlsSetValue(gStxTls, sThread->mHandle);
    return sThread->mFunction( sThread, sThread->mArgs );
}

stlStatus stxCreateThread( stlThread     * aThread,
                           stlThreadAttr * aThreadAttr,
                           stlThreadFunc   aThreadFunc,
                           void          * aArgs,
                           stlErrorStack * aErrorStack )
{
    HANDLE   sHandle;
    stlSize  sStackSize = 0;
    DWORD    sTemp;

    STL_PARAM_VALIDATE( aThread != NULL, aErrorStack );
    
    aThread->mArgs = aArgs;
    aThread->mFunction = aThreadFunc;
    aThread->mHandle = NULL;

    if (aThreadAttr != NULL)
    {
        sStackSize = aThreadAttr->mStackSize;
    }

    sHandle = CreateThread(NULL,
                           sStackSize,
                           (DWORD (STL_THREAD_FUNC *)(void *))stxDummyWorker,
                           aThread,
                           0,
                           &sTemp);

    STL_TRY_THROW(sHandle != 0, RAMP_ERR);

    aThread->mHandle = sHandle;
		
    aThread->mProc.mProcID = GetCurrentProcessId();
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_THREAD_CREATE,
                     NULL,
                     aErrorStack);

        steSetSystemError( GetLastError(), aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlBool stxIsEqualThread( stlThreadHandle * aThread1,
                          stlThreadHandle * aThread2 )
{
    return (aThread1->mHandle == aThread2->mHandle);
}

stlStatus stxExitThread( stlThread     * aThread,
                         stlStatus       aReturnStatus,
                         stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aThread != NULL, aErrorStack );
    
    aThread->mExitStatus = aReturnStatus;
    ExitThread( 0 );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxCancelThread( stlThread     * aThread,
                           stlStatus       aReturnStatus,
                           stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aThread != NULL, aErrorStack );
    
    aThread->mExitStatus = aReturnStatus;
    TerminateThread( aThread->mHandle, 0 );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxThreadSelf( stlThreadHandle * aThreadHandle,
                         stlErrorStack   * aErrorStack )
{
    HANDLE   sHandle;
    HANDLE   sProc;
    stlInt32 sReturn;

    STL_PARAM_VALIDATE( aThreadHandle != NULL, aErrorStack );

    sProc = GetCurrentProcess();
    sHandle = (HANDLE)TlsGetValue(gStxTls);

    if (sHandle != 0)
    {
        aThreadHandle->mHandle = sHandle;
    }
    else
    {
        sHandle = GetCurrentThread();

        sReturn = DuplicateHandle(sProc, sHandle, sProc, &sHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

        STL_ASSERT(sReturn != 0);

        TlsSetValue(gStxTls, sHandle);
        
        aThreadHandle->mHandle = sHandle;
    }
    
    aThreadHandle->mProc.mProcID = GetCurrentProcessId();
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxSetThreadAffinity( stlThreadHandle * aThreadHandle,
                                stlCpuSet       * aCpuSet,
                                stlErrorStack   * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTSUPPORT,
                  "pthread_setaffinity_np",
                  aErrorStack );

    return STL_FAILURE;
}

void stxNativeExitThread()
{
    ExitThread( 0 );
}

stlStatus stxJoinThread( stlThread     * aThread,
                         stlStatus     * aReturnStatus,
                         stlErrorStack * aErrorStack )
{
    stlInt32  sStatus;

    STL_PARAM_VALIDATE( aThread != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aThread->mHandle != 0, aErrorStack );
    STL_PARAM_VALIDATE( aReturnStatus != NULL, aErrorStack );
    
    sStatus = WaitForSingleObject(aThread->mHandle, INFINITE);

    STL_TRY_THROW(sStatus == WAIT_OBJECT_0, RAMP_ERR);

    *aReturnStatus = aThread->mExitStatus;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_THREAD_JOIN,
                      NULL,
                      aErrorStack );
        steSetSystemError( sStatus, aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}
                         
void stxYieldThread( void )
{
    /**
     * @todo
     */
}

stlStatus stxInitThreadOnce( stlThreadOnce * aThreadOnce,
                             stlErrorStack * aErrorStack )
{
    return STL_SUCCESS;
}

stlStatus stxThreadOnce( stlThreadOnce * aThreadOnce, 
                         void         (* aInitFunc)(void),
                         stlErrorStack * aErrorStack )
{
    LONG sReturn;
    
    STL_PARAM_VALIDATE( aThreadOnce != NULL, aErrorStack );

    sReturn = InterlockedExchange(&aThreadOnce->mOnce, 1);

    if (sReturn == 0)
    {
        aInitFunc();
    }
    
    return STL_SUCCESS;
   
    STL_FINISH;

    return STL_FAILURE;
}

