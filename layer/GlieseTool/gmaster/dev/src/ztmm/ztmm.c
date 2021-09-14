/*******************************************************************************
 * ztmm.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztmm.c 4674 2012-05-24 03:49:52Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ztmm.c
 * @brief Gliese Master Main Routines
 */

#include <stl.h>
#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <ell.h>
#include <qll.h>
#include <ssl.h>
#include <sll.h>
#include <ztmDef.h>
#include <ztmb.h>

/**
 * @brief 시스템 Environment
 * @remark startup, shutdown 과 세션 Environment 획득 시 사용된다.
 */
stlBool gZtmSilentMode;

stlGetOptOption gZtmmOptOption[] =
{ 
    { "msgqkey",  'm',  STL_TRUE,    "message queue key for startup" },
    { "silent",   'i',  STL_FALSE,   "don't print message" }, 
    { NULL,       '\0', STL_FALSE,  NULL } 
}; 


stlStatus ztmmDaemonize( stlBool         aDaemonize,
                         stlErrorStack * aErrorStack )
{
    stlProc            sProc;
    stlBool            sIsChild = STL_TRUE;

    /**
     * Demonize
     */
    if( aDaemonize == STL_TRUE )
    {
        STL_TRY( stlForkProc( &sProc, &sIsChild, aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        /* Debug 등을 위하여 Daemonize를 하지 않을때는 fork를 하지 않는다 */
        sIsChild = STL_TRUE;
    }

    if( sIsChild == STL_FALSE )
    {
        /*
         * Parent process goes to exit
         */
        stlExit(0);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

    
int main( int     aArgc,
          char ** aArgv )
{
    stlGetOpt         sOpt;
    stlChar           sCh;
    const stlChar   * sOptionArg;
    stlErrorStack     sErrorStack;
    stlChar         * sEndPtr;
    stlInt64          sMsgKey = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( sllInitialize() == STL_SUCCESS );

    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               &sErrorStack )
             == STL_SUCCESS );

    while( STL_TRUE )
    {
        if( stlGetOptionLong( &sOpt,
                              gZtmmOptOption,
                              &sCh,
                              &sOptionArg,
                              &sErrorStack ) != STL_SUCCESS )
        {
            if( stlGetLastErrorCode( &sErrorStack ) != STL_ERRCODE_EOF )
            {
                if( stlGetLastErrorCode( &sErrorStack ) == STL_ERRCODE_INVALID_ARGUMENT )
                {
                    STL_THROW( RAMP_ERR_INVALID_OPTION );
                }
                    
                STL_TRY( STL_FALSE );
            }
                
            stlPopError( &sErrorStack );
            break;
        }
        
        switch( sCh )
        {
            case 'i':
                gZtmSilentMode = STL_TRUE;
                break;
            case 'm':
                STL_TRY( stlStrToInt64( sOptionArg,
                                        STL_NTS,
                                        &sEndPtr,
                                        10,
                                        &sMsgKey,
                                        &sErrorStack )
                         == STL_SUCCESS );
                break;
            default:
                STL_THROW( RAMP_ERR_INVALID_OPTION );
                break;
        }
    }

    STL_TRY_THROW( sMsgKey != 0, RAMP_ERR_INVALID_OPTION );

    /**
     * Demonize
     */
    STL_TRY( ztmmDaemonize( STL_TRUE, &sErrorStack ) == STL_SUCCESS );

    STL_TRY( ztmbStartup( &sMsgKey,
                          &sErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OPTION )
    {
        STL_DASSERT( STL_FALSE );
    }
    
    STL_FINISH;

    /**
     * stlSendExecResult는 한번만 보내야 함
     */
    if( sMsgKey != 0 )
    {
        (void)stlWakeupExecProc( sMsgKey, &sErrorStack );
    }

    return STL_FAILURE;
}
