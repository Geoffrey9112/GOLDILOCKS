/*******************************************************************************
 * stxProcWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stxProcUnix.c 13496 2014-08-29 05:38:43Z leekmo $
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
#include <stlFile.h>
#include <stlLog.h>
#include <stlMemorys.h>

#include <process.h>
#include <Psapi.h>

#if STC_HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#if STC_HAVE_UNISTD_H
#include <unistd.h>
#endif

#if STC_HAVE_PROCFS_H
#include <procfs.h>
#endif

#if STC_HAVE_SYS_PROCFS_H
#include <sys/procfs.h>
#endif

#if STC_HAVE_SYS_PSTAT_H
#include <sys/pstat.h>
#endif

#if STC_HAVE_SIGNAL_H
#include <signal.h>
#endif

stlBool stxHasSpace( char * aStr )
{
    char    * sCh = NULL;
    stlBool   sFound = STL_FALSE;

    for( sCh = aStr; *sCh; ++sCh )
    {
        if( stlIsspace(*sCh) == STL_TRUE )
        {
            sFound = STL_TRUE;
            break;
        }
    }

    return (sFound == STL_TRUE) ? STL_TRUE : STL_FALSE;
}

stlStatus stxCreateProc( const stlChar * aPath,
                         stlChar * const aArgv[],
                         stlInt32 *      aExitCode,                                
                         stlErrorStack * aErrorStack )
{
    stlChar             sCmd[STL_PATH_MAX];
    stlSize             sPos = 0;
    BOOL                sReturn;
    STARTUPINFO         sStartupInfo = {0, };
    PROCESS_INFORMATION sProcessInfo;
    DWORD               sExitCode;
    stlInt32            i;

    *aExitCode = -1;
    
    sStartupInfo.cb = STL_SIZEOF(STARTUPINFO);

    if( stxHasSpace(aArgv[0]) == STL_TRUE )
    {
        sPos = stlSnprintf( sCmd, STL_PATH_MAX, "\"%s\"", aArgv[0] );
    }
    else
    {
        sPos = stlSnprintf( sCmd, STL_PATH_MAX, "%s", aArgv[0] );
    }
    
    for( i = 1; aArgv && aArgv[i]; ++i )
    {
        if( (stxHasSpace(aArgv[i]) == STL_TRUE) || (aArgv[i][0] != '\0') )
        {
            sPos += stlSnprintf( sCmd + sPos,
                                 STL_PATH_MAX - sPos,
                                 " \"%s\"",
                                 aArgv[i] );
        }
        else
        {
            sPos += stlSnprintf( sCmd + sPos,
                                 STL_PATH_MAX - sPos,
                                 " %s",
                                 aArgv[i] );
        }
    }

    sReturn = CreateProcess( NULL,
                             sCmd,
                             NULL,
                             NULL,
                             FALSE,
                             NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
                             NULL,
                             NULL,
                             &sStartupInfo,
                             &sProcessInfo );

    STL_TRY_THROW( sReturn != 0, RAMP_ERR_CRAETE_PROCESS );

    WaitForSingleObject( sProcessInfo.hProcess, INFINITE );

    sReturn = GetExitCodeProcess( sProcessInfo.hProcess, &sExitCode );

    STL_TRY_THROW( sReturn != 0, RAMP_ERR_GET_EXIT_CODE_PROCESS );

    *aExitCode = sExitCode;
    
    CloseHandle( sProcessInfo.hProcess );
    CloseHandle( sProcessInfo.hThread );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CRAETE_PROCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_PROC_EXECUTE,
                      NULL,
                      aErrorStack );

        steSetSystemError( GetLastError(), aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_GET_EXIT_CODE_PROCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_PROC_EXECUTE,
                      NULL,
                      aErrorStack );

        steSetSystemError( GetLastError(), aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxForkProc( stlProc       * aProc,
                       stlBool       * aIsChild,
                       stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stxWaitProc( stlProc       * aProc,
                       stlInt32      * aExitCode,
                       stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stxWaitAnyProc( stlInt32      * aExitCode,
                          stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stxExecuteProc( const stlChar * aPath,
                          stlChar * const aArgv[],
                          stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stxExecuteProcAndWait( stlChar * const aArgv[],
                                 stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlStatus stxWakeupExecProc( stlInt64      * aMsgKey,
                             stlErrorStack * aErrorStack)
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}
stlStatus stxGetCurrentProc( stlProc       * aProc,
                             stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aProc != NULL, aErrorStack );
    
    aProc->mProcID = GetCurrentProcessId();
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxGetParentProc( stlProc       * aProc,
                            stlErrorStack * aErrorStack )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  STL_ERRCODE_NOTIMPL,
                  NULL,
                  aErrorStack );

    return STL_FAILURE;
}

stlBool stxIsExistProc( stlProc * aProc )
{
    /**
     *@todo 구현해야 함
     */
    return STL_FAILURE;
}

stlStatus stxGetProcName( stlProc       * aProc,
                          stlChar       * aName,
                          stlSize         aSize,
                          stlErrorStack * aErrorStack )
{
    stlChar   sPath[STL_PATH_MAX] = { 0, };
    stlChar * sPos;

    GetProcessImageFileName(GetCurrentProcess(),
                            sPath,
                            STL_PATH_MAX);
    
    if( aName != NULL )
    {
        sPos = stlStrrchr((const stlChar*)sPath, '\\');

        if( sPos == NULL )
        {
            stlSnprintf(aName, aSize, "%s", sPath);
        }
        else
        {
            stlSnprintf( aName, aSize, "%s", sPos + 1 );
        }
    }
    
    return STL_SUCCESS;
}

stlStatus stxHideArgument( stlInt32        aArgc,
                           stlChar       * aArgv[],
                           stlErrorStack * aErrorStack,
                           stlInt32        aCount,
                           va_list         aVarArgList )
{
    stlInt32 i;
    stlInt32 sIdx;
    stlChar  sCmdLine[STL_PATH_MAX];

    for( i = 0; i < aCount; i ++ )
    {
        sIdx = va_arg( aVarArgList, stlInt32 );

        /*
         * 프로세스 이름은 남겨야 하기 때문에, aArgv[0]은 skip 한다.
         */
        if( (sIdx != 0) && (sIdx < aArgc) )
        {
            aArgv[sIdx] = NULL;
        }
    }

    stlMemset( sCmdLine, 0, STL_PATH_MAX );

    GetConsoleOriginalTitle( sCmdLine, STL_PATH_MAX );
    stlStrcat( sCmdLine, " - " );

    for( i = 0; i < aArgc; i ++ )
    {
        if( aArgv[i] != NULL )
        {
            stlStrcat( sCmdLine, aArgv[i] );
            stlStrcat( sCmdLine, " " );
        }
    }

    STL_TRY_THROW( SetConsoleTitle( sCmdLine ) != 0, RAMP_ERR_NOT_SUPPORT_OS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORT_OS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OS_NOTSUPPORT,
                      NULL,
                      aErrorStack );

        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}
