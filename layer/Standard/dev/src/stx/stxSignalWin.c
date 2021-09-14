/*******************************************************************************
 * stxSignalWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stxSignalUnix.c 13496 2014-08-29 05:38:43Z leekmo $
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
    HANDLE sProcess = NULL;
    BOOL   sResult  = FALSE;

    STL_PARAM_VALIDATE( aProc != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aSignalNo != STL_SIGNAL_UNKNOWN, aErrorStack );

    sProcess = OpenProcess( PROCESS_TERMINATE, FALSE, aProc->mProcID );

    STL_TRY_THROW( sProcess != NULL, RAMP_ERR_KILL );

    sResult = TerminateProcess( sProcess, aSignalNo );

    CloseHandle( sProcess );

    STL_TRY_THROW( sResult != 0, RAMP_ERR_KILL );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_KILL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SIGNAL_KILL,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxSetSignalHandler( stlInt32        aSignalNo,
                               stlSignalFunc   aNewFunc,
                               stlSignalFunc * aOldFunc,
                               stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stxSetSignalHandlerEx( stlInt32          aSignalNo,
                                 stlSignalExFunc   aNewFunc,
                                 stlSignalExFunc * aOldFunc,
                                 stlErrorStack   * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stxBlockSignal( stlInt32        aSignalNo,
                          stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stxUnblockSignal( stlInt32        aSignalNo,
                            stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stxBlockThreadSignals( stlInt32      * aSignalNo,
                                 stlInt32        aSignalCount,
                                 stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;	
}

stlStatus stxUnblockThreadSignals( stlInt32      * aSignalNo,
                                   stlInt32        aSignalCount,
                                   stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

void stxGetSignalOriginProcess( void    * aSigInfo,
                                stlProc * aOriginProc )
{
    /**
     * 사용하지 않는다.
     */
    aOriginProc->mProcID = 0;
}
