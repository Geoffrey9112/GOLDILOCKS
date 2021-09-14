/*******************************************************************************
 * stxSignalUnix.c
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

#if STC_HAVE_SIGNAL_H
#include <signal.h>
#endif
#if STC_HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

stlStatus stxKillProc( stlProc       * aProc,
                       stlInt32        aSignalNo,
                       stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aProc != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aSignalNo != STL_SIGNAL_UNKNOWN, aErrorStack );
    
    STL_TRY_THROW( kill( aProc->mProcID,
                         aSignalNo ) != -1, RAMP_ERR_KILL );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_KILL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SIGNAL_KILL,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

#if defined(__NetBSD__) || defined(DARWIN)
void stxAvoidZombies( stlInt32 aSignalNo )
{
    stlInt32 sExitStatus;

    while( waitpid( -1, &sExitStatus, WNOHANG ) > 0 )
    {
        /* do nothing */
    }
}

void stxAvoidZombiesEx( stlInt32 aSignalNo, void * aSigInfo, void * aContext )
{
    stlInt32 sExitStatus;

    while( waitpid( -1, &sExitStatus, WNOHANG ) > 0 )
    {
        /* do nothing */
    }
}
#endif /* DARWIN */

stlStatus stxSetSignalHandler( stlInt32        aSignalNo,
                               stlSignalFunc   aNewFunc,
                               stlSignalFunc * aOldFunc,
                               stlErrorStack * aErrorStack )
{
    struct sigaction sNewAction;
    struct sigaction sOldAction;
    sigset_t         sSigBlockMask;

    STL_PARAM_VALIDATE( aSignalNo != STL_SIGNAL_UNKNOWN, aErrorStack );
    
    sigemptyset( &sSigBlockMask );

    if( aSignalNo != STL_SIGNAL_HUP )
    {
        sigaddset( &sSigBlockMask, STL_SIGNAL_HUP );
    }
    if( aSignalNo != STL_SIGNAL_TERM )
    {
        sigaddset( &sSigBlockMask, STL_SIGNAL_TERM );
    }
    if( aSignalNo != STL_SIGNAL_QUIT )
    {
        sigaddset( &sSigBlockMask, STL_SIGNAL_QUIT );
    }
    if( aSignalNo != STL_SIGNAL_ABRT )
    {
        sigaddset( &sSigBlockMask, STL_SIGNAL_ABRT );
    }
    
    sNewAction.sa_handler = (stlSignalFunc)aNewFunc;
    sNewAction.sa_mask = sSigBlockMask;
    sNewAction.sa_flags = 0;
#ifdef SA_INTERRUPT             /* SunOS */
    sNewAction.sa_flags |= SA_INTERRUPT;
#endif
#if defined(__osf__) && defined(__alpha)
    /* XXX jeff thinks this should be enabled whenever SA_NOCLDWAIT is defined */

    /* this is required on Tru64 to cause child processes to
     * disappear gracefully - XPG4 compatible 
     */
    if( (aSignalNo == SIGCHLD) && (aNewFunc == SIG_IGN) )
    {
        sNewAction.sa_flags |= SA_NOCLDWAIT;
    }
#endif
#if defined(__NetBSD__) || defined(DARWIN)
    /* ignoring SIGCHLD or leaving the default disposition doesn't avoid zombies,
     * and there is no SA_NOCLDWAIT flag, so catch the signal and reap status in 
     * the handler to avoid zombies
     */
    if( (aSignalNo == SIGCHLD) && (aNewfunc == SIG_IGN) )
    {
        sNewAction.sa_handler = stxAvoidZombies;
    }
#endif
    STL_TRY_THROW( sigaction( aSignalNo,
                              &sNewAction,
                              &sOldAction ) >= 0, RAMP_ERR_SIGACTION );

    if( aOldFunc != NULL )
    {
        *aOldFunc = (stlSignalFunc)sOldAction.sa_handler;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SIGACTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SIGNAL_SET_HANDLER,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

typedef void (*stxSignalFunc)( stlInt32, siginfo_t*, void* );

stlStatus stxSetSignalHandlerEx( stlInt32          aSignalNo,
                                 stlSignalExFunc   aNewFunc,
                                 stlSignalExFunc * aOldFunc,
                                 stlErrorStack   * aErrorStack )
{
    struct sigaction sNewAction;
    struct sigaction sOldAction;
    sigset_t         sSigBlockMask;

    STL_PARAM_VALIDATE( aSignalNo != STL_SIGNAL_UNKNOWN, aErrorStack );
    
    sigemptyset( &sSigBlockMask );

    if( aSignalNo != STL_SIGNAL_HUP )
    {
        sigaddset( &sSigBlockMask, STL_SIGNAL_HUP );
    }
    if( aSignalNo != STL_SIGNAL_TERM )
    {
        sigaddset( &sSigBlockMask, STL_SIGNAL_TERM );
    }
    if( aSignalNo != STL_SIGNAL_QUIT )
    {
        sigaddset( &sSigBlockMask, STL_SIGNAL_QUIT );
    }
    if( aSignalNo != STL_SIGNAL_ABRT )
    {
        sigaddset( &sSigBlockMask, STL_SIGNAL_ABRT );
    }
    
    sNewAction.sa_sigaction = (stxSignalFunc)aNewFunc;
    sNewAction.sa_mask = sSigBlockMask;
    sNewAction.sa_flags = SA_SIGINFO;
#ifdef SA_INTERRUPT             /* SunOS */
    sNewAction.sa_flags |= SA_INTERRUPT;
#endif
#if defined(__osf__) && defined(__alpha)
    /* XXX jeff thinks this should be enabled whenever SA_NOCLDWAIT is defined */

    /* this is required on Tru64 to cause child processes to
     * disappear gracefully - XPG4 compatible 
     */
    if( (aSignalNo == SIGCHLD) && (aNewFunc == SIG_IGN) )
    {
        sNewAction.sa_flags |= SA_NOCLDWAIT;
    }
#endif
#if defined(__NetBSD__) || defined(DARWIN)
    /* ignoring SIGCHLD or leaving the default disposition doesn't avoid zombies,
     * and there is no SA_NOCLDWAIT flag, so catch the signal and reap status in 
     * the handler to avoid zombies
     */
    if( (aSignalNo == SIGCHLD) && (aNewfunc == SIG_IGN) )
    {
        sNewAction.sa_sigaction = stxAvoidZombiesEx;
    }
#endif
    STL_TRY_THROW( sigaction( aSignalNo,
                              &sNewAction,
                              &sOldAction ) >= 0, RAMP_ERR_SIGACTION );

    if( aOldFunc != NULL )
    {
        *aOldFunc = (stlSignalExFunc)sOldAction.sa_sigaction;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SIGACTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SIGNAL_SET_HANDLER,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxBlockSignal( stlInt32        aSignalNo,
                          stlErrorStack * aErrorStack )
{
    sigset_t sSignalMask;

    STL_TRY_THROW( aSignalNo != STL_SIGNAL_UNKNOWN, RAMP_IGNORE );
        
    sigemptyset( &sSignalMask );
    sigaddset( &sSignalMask, aSignalNo );

    STL_TRY_THROW( sigprocmask( SIG_BLOCK,
                                &sSignalMask,
                                NULL ) == 0, RAMP_ERR_SIGBLOCK );

    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SIGBLOCK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SIGNAL_BLOCK,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxUnblockSignal( stlInt32        aSignalNo,
                            stlErrorStack * aErrorStack )
{
    sigset_t sSignalMask;

    STL_TRY_THROW( aSignalNo != STL_SIGNAL_UNKNOWN, RAMP_IGNORE );
        
    sigemptyset( &sSignalMask );
    sigaddset( &sSignalMask, aSignalNo );

    STL_TRY_THROW( sigprocmask( SIG_UNBLOCK,
                                &sSignalMask,
                                NULL ) == 0, RAMP_ERR_SIGBLOCK );

    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SIGBLOCK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SIGNAL_BLOCK,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxBlockThreadSignals( stlInt32      * aSignalNo,
                                 stlInt32        aSignalCount,
                                 stlErrorStack * aErrorStack )
{
    sigset_t sSignalMask;
    stlInt32 i;

    sigemptyset( &sSignalMask );
    
    for( i = 0; i < aSignalCount; i++ )
    {
        STL_TRY_THROW( aSignalNo[i] != STL_SIGNAL_UNKNOWN, RAMP_IGNORE );
        sigaddset( &sSignalMask, aSignalNo[i] );
    }

    STL_TRY_THROW( pthread_sigmask( SIG_BLOCK,
                                    &sSignalMask,
                                    NULL ) == 0, RAMP_ERR_SIGBLOCK );

    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SIGBLOCK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SIGNAL_BLOCK,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxUnblockThreadSignals( stlInt32      * aSignalNo,
                                   stlInt32        aSignalCount,
                                   stlErrorStack * aErrorStack )
{
    sigset_t sSignalMask;
    stlInt32 i;

    sigemptyset( &sSignalMask );
    
    for( i = 0; i < aSignalCount; i++ )
    {
        STL_TRY_THROW( aSignalNo[i] != STL_SIGNAL_UNKNOWN, RAMP_IGNORE );
        sigaddset( &sSignalMask, aSignalNo[i] );
    }

    STL_TRY_THROW( pthread_sigmask( SIG_UNBLOCK,
                                    &sSignalMask,
                                    NULL ) == 0, RAMP_ERR_SIGBLOCK );

    STL_RAMP( RAMP_IGNORE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SIGBLOCK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SIGNAL_BLOCK,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

void stxGetSignalOriginProcess( void    * aSigInfo,
                                stlProc * aOriginProc )
{
    siginfo_t * sSigInfo = (siginfo_t*)aSigInfo;

    if( sSigInfo == NULL )
    {
        aOriginProc->mProcID = 0;
    }
    else
    {
        aOriginProc->mProcID = sSigInfo->si_pid;
    }
}
