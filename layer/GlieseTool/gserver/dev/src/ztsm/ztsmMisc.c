/*******************************************************************************
 * ztsmMisc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztsmMisc.c 8377 2013-05-06 04:07:18Z mycomman $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztsmMisc.c
 * @brief Gliese gserver Misc Routines
 */

#include <stl.h>
#include <cml.h>
#include <knl.h>
#include <scl.h>
#include <sml.h>
#include <ell.h>
#include <qll.h>
#include <ssl.h>
#include <ztsDef.h>
#include <ztsm.h>


stlGetOptOption gZtsmOptOption[] =
{
    { "msgqkey",        'm',  STL_TRUE,   "message queue key for startup" },
    { "dedicated",      'd',  STL_TRUE,   "dedicated server mode " },
    { "shared",         's',  STL_TRUE,   "shared server mode" },
    { "no-daemon",      'n',  STL_FALSE,  "gserver doesn't daemonize" },
    { NULL,             '\0', STL_FALSE,  NULL }
};


stlStatus ztsmDaemonize( stlBool         aDaemonize,
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

