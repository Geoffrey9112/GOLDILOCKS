/*******************************************************************************
 * stxThreadUnix.c
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

#include <stc.h>
#include <stlDef.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlLog.h>
#include <stt.h>

#if STC_HAVE_PTHREAD_H
#include <pthread.h>
#endif

static const pthread_once_t stxThreadOnceInit = PTHREAD_ONCE_INIT;

stlStatus stxInitThreadAttr( stlThreadAttr * aThreadAttr,
                             stlErrorStack * aErrorStack )
{
    stlInt32 sStatus;

    STL_PARAM_VALIDATE( aThreadAttr != NULL, aErrorStack );
    
    sStatus = pthread_attr_init( &aThreadAttr->mAttribute );

    if( sStatus != 0 )
    {
#ifdef STC_HAVE_ZOS_PTHREADS
        sStatus = errno;
#endif
        STL_THROW( RAMP_ERR );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_THREAD_ATTR,
                      NULL,
                      aErrorStack );
        steSetSystemError( sStatus, aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxSetThreadAttrStackSize( stlThreadAttr * aThreadAttr,
                                     stlSize         aStackSize,
                                     stlErrorStack * aErrorStack )
{
    stlInt32 sStatus;

    STL_PARAM_VALIDATE( aThreadAttr != NULL, aErrorStack );
    
    sStatus = pthread_attr_setstacksize( &aThreadAttr->mAttribute,
                                         aStackSize );
    if( sStatus != 0 )
    {
#ifdef STC_HAVE_ZOS_PTHREADS
        sStatus = errno;
#endif
        STL_THROW( RAMP_ERR );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_THREAD_ATTR,
                      NULL,
                      aErrorStack );
        steSetSystemError( sStatus, aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

void * stxDummyWorker( void * aOpaque )
{
    stlThread * sThread;

    sThread = (stlThread *)aOpaque;
    return sThread->mFunction( sThread, sThread->mArgs );
}

stlStatus stxCreateThread( stlThread     * aThread,
                           stlThreadAttr * aThreadAttr,
                           stlThreadFunc   aThreadFunc,
                           void          * aArgs,
                           stlErrorStack * aErrorStack )
{
    stlInt32 sStatus;

    STL_PARAM_VALIDATE( aThread != NULL, aErrorStack );
    
    aThread->mArgs = aArgs;
    aThread->mFunction = aThreadFunc;

    if( aThreadAttr == NULL )
    {
        sStatus = pthread_create( &aThread->mHandle,
                                  NULL,
                                  stxDummyWorker,
                                  (void*)aThread );
    }
    else
    {
        sStatus = pthread_create( &aThread->mHandle,
                                  &aThreadAttr->mAttribute,
                                  stxDummyWorker,
                                  (void*)aThread );
    }

    aThread->mProc.mProcID = getpid();
    
    if( sStatus != 0 )
    {
#ifdef STC_HAVE_ZOS_PTHREADS
        sStatus = errno;
#endif
        STL_THROW( RAMP_ERR );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        switch( sStatus )
        {
            case EAGAIN:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INSUFFICIENT_RESOURCE,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_THREAD_CREATE,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( sStatus, aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlBool stxIsEqualThread( stlThreadHandle * aThread1,
                          stlThreadHandle * aThread2 )
{
    stlInt32 sStatus;

    sStatus = pthread_equal( aThread1->mHandle, aThread2->mHandle );

    if( sStatus == 0 )
    {
        return STL_FALSE;
    }

    if( aThread1->mProc.mProcID != aThread2->mProc.mProcID )
    {
        return STL_FALSE;
    }
    
    return STL_TRUE;
}

stlStatus stxExitThread( stlThread     * aThread,
                         stlStatus       aReturnStatus,
                         stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aThread != NULL, aErrorStack );
    
    aThread->mExitStatus = aReturnStatus;
    pthread_exit( NULL );
    
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
    pthread_cancel( aThread->mHandle );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxThreadSelf( stlThreadHandle * aThreadHandle,
                         stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( aThreadHandle != NULL, aErrorStack );
    aThreadHandle->mHandle = pthread_self();
    aThreadHandle->mProc.mProcID = getpid();
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxSetThreadAffinity( stlThreadHandle * aThreadHandle,
                                stlCpuSet       * aCpuSet,
                                stlErrorStack   * aErrorStack )
{
#if defined( STL_HAVE_CPUSET )
    stlInt32  sStatus;
    
    STL_PARAM_VALIDATE( aThreadHandle != NULL, aErrorStack );
    
    sStatus = pthread_setaffinity_np( aThreadHandle->mHandle,
                                      STL_SIZEOF( stlCpuSet ),
                                      aCpuSet );
    
    if( sStatus != 0 )
    {
#ifdef STC_HAVE_ZOS_PTHREADS
        sStatus = errno;
#endif
        STL_THROW( RAMP_ERR );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR )
    {
        switch( sStatus )
        {
            case EINVAL:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INVALID_ARGUMENT,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_SET_THREAD_AFFINITY,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( sStatus, aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
#else
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTSUPPORT,
                  "pthread_setaffinity_np",
                  aErrorStack );
    
    return STL_FAILURE;
#endif
}

void stxNativeExitThread()
{
    pthread_exit( NULL );
}

stlStatus stxJoinThread( stlThread     * aThread,
                         stlStatus     * aReturnStatus,
                         stlErrorStack * aErrorStack )
{
    stlInt32  sStatus;

    STL_PARAM_VALIDATE( aThread != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aReturnStatus != NULL, aErrorStack );
    
    sStatus = pthread_join( aThread->mHandle,
                            NULL );

    if( sStatus != 0 )
    {
#ifdef STC_HAVE_ZOS_PTHREADS
        sStatus = errno;
#endif
        STL_THROW( RAMP_ERR );
    }

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
#ifdef STC_HAVE_PTHREAD_YIELD

#ifdef STC_HAVE_ZOS_PTHREADS
    pthread_yield( NULL );
#else
    pthread_yield();
#endif /* STC_HAVE_ZOS_PTHREADS */

#else

#ifdef STC_HAVE_SCHED_YIELD
    sched_yield();
#endif

#endif /* STC_HAVE_PTHREAD_YIELD */
}

stlStatus stxInitThreadOnce( stlThreadOnce * aThreadOnce,
                             stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aThreadOnce != NULL, aErrorStack );
    
    aThreadOnce->mOnce = stxThreadOnceInit;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxThreadOnce( stlThreadOnce * aThreadOnce, 
                         void         (* aInitFunc)(void),
                         stlErrorStack * aErrorStack )
{
    stlInt32 sStatus;
    
    STL_PARAM_VALIDATE( aThreadOnce != NULL, aErrorStack );
    
    sStatus = pthread_once( &aThreadOnce->mOnce,
                            aInitFunc );

    STL_TRY_THROW( sStatus == 0, RAMP_ERR );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_THREAD_ONCE,
                      NULL,
                      aErrorStack );
        steSetSystemError( sStatus, aErrorStack );
    }
   
    STL_FINISH;

    return STL_FAILURE;
}

