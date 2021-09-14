/*******************************************************************************
 * stxProcUnix.c
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
#include <stlFile.h>
#include <stlLog.h>
#include <stlMsgQueue.h>
#include <stlThreadProc.h>
#include <stlTime.h>
#include <stx.h>
#include <stlMemorys.h>

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

stlStatus stxCreateProc( const stlChar * aPath,
                         stlChar * const aArgv[],
                         stlInt32 *      aExitCode,                                
                         stlErrorStack * aErrorStack )
{
    pid_t    sPid;
    stlInt32 sStatus;
    stlInt32 sExitCode;
    stlInt32 sWaitOptions = WUNTRACED;

    sPid = fork();

    STL_TRY_THROW( sPid >= 0, RAMP_ERR_INSUFFICIENT_RESOURCE );

    if( sPid == 0 )
    {
        STL_TRY_THROW( execvp( aPath, aArgv ) != -1, RAMP_ERR_EXEC );
    }
    else
    {
        do
        {
            sStatus = waitpid( sPid,
                               &sExitCode,
                               sWaitOptions );
        } while( (sStatus < 0) && (errno == EINTR) );

        STL_TRY_THROW( sStatus > 0, RAMP_ERR_WAITPID );

        *aExitCode = WEXITSTATUS( sExitCode );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_RESOURCE )
    {
        switch( errno )
        {
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INSUFFICIENT_RESOURCE,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
   
    STL_CATCH( RAMP_ERR_EXEC )
    {
        switch( errno )
        {
            case EPERM:
            case EACCES:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_ACCESS,
                              NULL,
                              aErrorStack,
                              aPath );
                break;
            case EMFILE:
            case ENFILE:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INSUFFICIENT_RESOURCE,
                              NULL,
                              aErrorStack );
                break;
            case ENOENT:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_NO_ENTRY,
                              NULL,
                              aErrorStack,
                              aPath );
                break;
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_PROC_EXECUTE,
                              NULL,
                              aErrorStack );
                break;
                
        }
        steSetSystemError( errno, aErrorStack );

        /**
         * @todo 자식 프로세스의 에러를 부모에게 전달할 방법이 필요
         */
        exit( -1 );        
    }
   
    STL_CATCH( RAMP_ERR_WAITPID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_PROC_WAIT,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
   
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxForkProc( stlProc       * aProc,
                       stlBool       * aIsChild,
                       stlErrorStack * aErrorStack )
{
    pid_t   sPid;
    stlBool sIsChild;

    STL_PARAM_VALIDATE( aProc != NULL, aErrorStack );
    
    sPid = fork();

    STL_TRY_THROW( sPid >= 0, RAMP_ERR_INSUFFICIENT_RESOURCE );

    if( sPid == 0 )
    {
        sIsChild = STL_TRUE;
    }
    else
    {
        sIsChild = STL_FALSE;
    }

    aProc->mProcID = sPid;

    if( aIsChild != NULL )
    {
        *aIsChild = sIsChild;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INSUFFICIENT_RESOURCE )
    {
        switch( errno )
        {
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INSUFFICIENT_RESOURCE,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
   
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxWaitProc( stlProc       * aProc,
                       stlInt32      * aExitCode,
                       stlErrorStack * aErrorStack )
{
    stlInt32 sStatus;
    stlInt32 sExitCode;
    stlInt32 sWaitOptions = WUNTRACED;
    
    STL_PARAM_VALIDATE( aProc != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aExitCode != NULL, aErrorStack );
    
    do
    {
        sStatus = waitpid( aProc->mProcID,
                           &sExitCode,
                           sWaitOptions );
    } while( (sStatus < 0) && (errno == EINTR) );

    STL_TRY_THROW( sStatus > 0, RAMP_ERR );

    aProc->mProcID = sStatus;
    *aExitCode = WEXITSTATUS( sExitCode );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_PROC_WAIT,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
   
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxWaitAnyProc( stlInt32      * aExitCode,
                          stlErrorStack * aErrorStack )
{
    stlInt32 sStatus;
    stlInt32 sExitCode = 0;
    stlInt32 sWaitOptions = WUNTRACED;
    
    STL_PARAM_VALIDATE( aExitCode != NULL, aErrorStack );
    
    do
    {
        sStatus = waitpid( 0,
                           &sExitCode,
                           sWaitOptions );
    } while( (sStatus < 0) && (errno == EINTR) );

    STL_TRY_THROW( sStatus > 0, RAMP_ERR );

    *aExitCode = WEXITSTATUS( sExitCode );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_PROC_WAIT,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
   
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxExecuteProc( const stlChar * aPath,
                          stlChar * const aArgv[],
                          stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aPath != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aArgv != NULL, aErrorStack );
    
    STL_TRY_THROW( execvp( aPath, aArgv ) != -1, RAMP_ERR );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR )
    {
        switch( errno )
        {
            case EPERM:
            case EACCES:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_ACCESS,
                              NULL,
                              aErrorStack,
                              aPath );
                break;
            case EMFILE:
            case ENFILE:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INSUFFICIENT_RESOURCE,
                              NULL,
                              aErrorStack );
                break;
            case ENOENT:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_NO_ENTRY,
                              NULL,
                              aErrorStack,
                              aPath );
                break;
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_PROC_EXECUTE,
                              NULL,
                              aErrorStack );
                break;
                
        }
        steSetSystemError( errno, aErrorStack );
    }
   
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxExecuteProcAndWait( stlChar        * const aArgv[],
                                 stlErrorStack  * aErrorStack )
{
    stlInt32            sState = 0;
    stlChar           * sArgv[STX_MAX_PROCESS_ARGS];
    stlInt32            sIdx;
    stlMsgQueue         sMsgQueue;
    stlChar             sMsgKeyString[1024];
    stlProc             sProc;
    stlBool             sIsChild;
    stlInt32            sMsgKey;
    stlMsgBuffer      * sMsgBuffer;
    stlSize             sReceivedSize;

    STL_TRY( stlCreateMsgQueue4Startup( &sMsgQueue,
                                        &sMsgKey,
                                        aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    for( sIdx = 0; aArgv[sIdx] != NULL ; sIdx++ )
    {
        sArgv[sIdx] = aArgv[sIdx];
    }

    STL_DASSERT( sIdx + 2 < STX_MAX_PROCESS_ARGS );

    stlSnprintf( sMsgKeyString,
                 1024,
                 "--msgqkey=%d",
                 sMsgKey );

    sArgv[sIdx] = sMsgKeyString;
    sIdx++;
    sArgv[sIdx] = NULL;
    sIdx++;

    STL_TRY( stlForkProc( &sProc,
                          &sIsChild,
                          aErrorStack )
             == STL_SUCCESS );

    if( sIsChild == STL_TRUE )
    {
        STL_TRY( stlExecuteProc( sArgv[0],
                                 sArgv,
                                 aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( stlAllocMsgBuffer( STL_SIZEOF( stlErrorStack ),
                                    &sMsgBuffer,
                                    aErrorStack )
                 == STL_SUCCESS );
        sState = 2;

        STL_TRY( stlRecvMsgQueue( &sMsgQueue,
                                  sMsgBuffer,
                                  STL_SIZEOF( stlErrorStack ),
                                  STL_MSGQ_TYPE_EXEC_PROCESS,
                                  0,
                                  &sReceivedSize,
                                  aErrorStack ) == STL_SUCCESS );
        /**
         * error를 받았으면 실패 처리
         */
        if( sReceivedSize == STL_SIZEOF( stlErrorStack ) )
        {
            stlAppendErrors( aErrorStack, (stlErrorStack*)sMsgBuffer->mText );
            STL_TRY( STL_FALSE );
        }

        STL_DASSERT( sReceivedSize == 0 );

        sState = 1;
        STL_TRY( stlFreeMsgBuffer( sMsgBuffer, aErrorStack ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( stlDestroyMsgQueue( &sMsgQueue, aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            stlFreeMsgBuffer( sMsgBuffer, aErrorStack );
        case 1:
            stlDestroyMsgQueue( &sMsgQueue, aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


stlStatus stxWakeupExecProc( stlInt64          aMsgKey,
                             stlErrorStack   * aErrorStack )
{
    stlInt32        sState = 0;
    stlMsgQueue     sMsgQueue;
    stlMsgBuffer  * sMsgBuffer;

    STL_TRY( stlGetMsgQueue( &sMsgQueue,
                             aMsgKey,
                             aErrorStack )
             == STL_SUCCESS );

    sState = 1;

    if( stlGetErrorStackDepth(aErrorStack) > 0 )
    {
        STL_TRY( stlAllocMsgBuffer( STL_SIZEOF( stlErrorStack ),
                                    &sMsgBuffer,
                                    aErrorStack )
                 == STL_SUCCESS );

        /**
         * mErrorCallStack도 전달이 되는데 문제 없을지 확인 필요.
         */
        STL_TRY( stlCopyToMsgBuffer( sMsgBuffer,
                                     (void*)aErrorStack,
                                     STL_SIZEOF(stlErrorStack),
                                     STL_MSGQ_TYPE_EXEC_PROCESS,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( stlSendMsgQueue( &sMsgQueue,
                                  sMsgBuffer,
                                  STL_SIZEOF(stlErrorStack),
                                  aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( stlAllocMsgBuffer( 0,
                                    &sMsgBuffer,
                                    aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( stlCopyToMsgBuffer( sMsgBuffer,
                                     NULL,
                                     0,
                                     STL_MSGQ_TYPE_EXEC_PROCESS,
                                     aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( stlSendMsgQueue( &sMsgQueue,
                                  sMsgBuffer,
                                  0,
                                  aErrorStack )
                 == STL_SUCCESS );
    }

    sState = 0;
    (void)stlFreeMsgBuffer( sMsgBuffer, aErrorStack );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlFreeMsgBuffer( sMsgBuffer, aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus stxGetCurrentProc( stlProc       * aProc,
                             stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aProc != NULL, aErrorStack );
    
    aProc->mProcID = getpid();
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stxGetParentProc( stlProc       * aProc,
                            stlErrorStack * aErrorStack )
{
    STL_PARAM_VALIDATE( aProc != NULL, aErrorStack );

    aProc->mProcID = getppid();
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlBool stxIsExistProc( stlProc * aProc )
{
    int     sRet;
    stlBool sIsExistProc = STL_FALSE;
    
    /*
     * If sig is 0, then no signal is sent, but error checking is still performed;
     * this can be used to check for the existence of a process ID or process group ID.
     */ 
    sRet = kill( aProc->mProcID, 0 );

    if( sRet == 0 )
    {
        sIsExistProc = STL_TRUE;
    }
    else
    {
        switch( errno )
        {
            /*
             *  ESRCH
             *
             *  The pid or process group does not exist.
             *  Note that an existing process might be a zombie,
             *  a process which already committed termination,
             *  but has not yet been wait(2)ed for.
             */
            case ESRCH :
                sIsExistProc = STL_FALSE;
                break;
            /*
             * EPERM
             *
             * The process does not have permission to send the signal to
             * any of the target processes.
             */
            case EPERM :
            default :
                sIsExistProc = STL_TRUE;
                break;
        }
    }

    return sIsExistProc;
}

stlStatus stxGetProcName( stlProc       * aProc,
                          stlChar       * aName,
                          stlSize         aSize,
                          stlErrorStack * aErrorStack )
{
    stlFile sFile;
    stlBool sIsOpen = STL_FALSE;
    
#if defined(STC_TARGET_OS_AIX) || defined(STC_TARGET_OS_SOLARIS)

    stlChar  sPath[STL_PATH_MAX];
    psinfo_t sProcInfo;

    stlSnprintf( sPath, STL_PATH_MAX, "/proc/%d/psinfo", aProc->mProcID );

    STL_TRY( stlOpenFile( &sFile,
                          sPath,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sIsOpen = STL_TRUE;

    STL_TRY( stlReadFile( &sFile,
                          (void*)&sProcInfo,
                          STL_SIZEOF( psinfo_t ),
                          NULL,
                          aErrorStack ) == STL_SUCCESS );

    if( aName != NULL )
    {
        stlSnprintf( aName, aSize, "%s", sProcInfo.pr_fname );
    }

    sIsOpen = STL_FALSE;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
#elif defined(STC_TARGET_OS_HPUX)

    struct pst_status sProcInfo;
    
    STL_TRY_THROW( pstat_getproc( &sProcInfo,
                                  STL_SIZEOF( struct pst_status ),
                                  0,
                                  aProc->mProcID ) != -1, 
                   RAMP_ERR_NOT_SUPPORT_OS );

    if( aName != NULL )
    {
        stlSnprintf( aName, aSize, "%s", sProcInfo.pst_ucomm );
    }

#elif defined(STC_TARGET_OS_LINUX)

    stlChar   sPath[STL_PATH_MAX];
    stlChar   sCmdline[STL_PATH_MAX];
    stlChar * sPos;
    
    stlSnprintf( sPath, STL_PATH_MAX, "/proc/%d/cmdline", aProc->mProcID );

    STL_TRY( stlOpenFile( &sFile,
                          sPath,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sIsOpen = STL_TRUE;

    STL_TRY( stlReadFile( &sFile,
                          (void*)sCmdline,
                          STL_PATH_MAX,
                          NULL,
                          aErrorStack ) == STL_SUCCESS );

    if( aName != NULL )
    {
        sPos = stlStrrchr( (const stlChar*)sCmdline, '/' );

        if( sPos == NULL )
        {
            stlSnprintf( aName, aSize, "%s", sCmdline );
        }
        else
        {
            stlSnprintf( aName, aSize, "%s", sPos + 1 );
        }
    }

    sIsOpen = STL_FALSE;
    STL_TRY( stlCloseFile( &sFile,
                           aErrorStack ) == STL_SUCCESS );
    
#else

    /*
     * 지원하지 못하는 OS
     */

    STL_THROW( RAMP_ERR_NOT_SUPPORT_OS );
    
#endif

    return STL_SUCCESS;

#if defined(STC_TARGET_OS_HPUX)
    
    STL_CATCH( RAMP_ERR_NOT_SUPPORT_OS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OS_NOTSUPPORT,
                      NULL,
                      aErrorStack );
    }
    
#endif
    
    STL_FINISH;

    if( sIsOpen == STL_TRUE )
    {
        (void)stlCloseFile( &sFile, aErrorStack );
    }

    return STL_FAILURE;
}

stlStatus stxHideArgument( stlInt32        aArgc,
                           stlChar       * aArgv[],
                           stlErrorStack * aErrorStack,
                           stlInt32        aCount,
                           va_list         aVarArgList )
{
    stlInt32  i;
    stlInt32  sIdx;

#if defined(STC_TARGET_OS_LINUX) || defined(STC_TARGET_OS_AIX) || defined(STC_TARGET_OS_SOLARIS)

    stlSize   sArgLen;

    for( i = 0; i < aCount; i ++ )
    {
        sIdx = va_arg( aVarArgList, stlInt32 );

        /*
         * 프로세스 이름은 남겨야 하기 때문에, aArgv[0]은 skip 한다.
         */
        if( (sIdx != 0) && (sIdx < aArgc) )
        {
            sArgLen = stlStrlen( aArgv[sIdx] );
            stlMemset( aArgv[sIdx], ' ', sArgLen );
        }
    }

#elif defined(STC_TARGET_OS_HPUX)

    union pstun  sPst;
    stlChar      sCmdLine[STL_PATH_MAX];

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
    for( i = 0; i < aArgc; i ++ )
    {
        if( aArgv[i] != NULL )
        {
            stlStrcat( sCmdLine, aArgv[i] );
            stlStrcat( sCmdLine, " " );
        }
    }

    sPst.pst_command = sCmdLine;
    STL_TRY_THROW( pstat( PSTAT_SETCMD,
                          sPst,
                          stlStrlen( sCmdLine ),
                          0,
                          0 ) != -1,
                   RAMP_ERR_NOT_SUPPORT_OS );

#else

    /*
     * 지원하지 못하는 OS
     */

    STL_THROW( RAMP_ERR_NOT_SUPPORT_OS );

#endif

    return STL_SUCCESS;

#if !defined(STC_TARGET_OS_LINUX) && !defined(STC_TARGET_OS_AIX) && !defined(STC_TARGET_OS_SOLARIS)

    STL_CATCH( RAMP_ERR_NOT_SUPPORT_OS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OS_NOTSUPPORT,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;

#endif
}

