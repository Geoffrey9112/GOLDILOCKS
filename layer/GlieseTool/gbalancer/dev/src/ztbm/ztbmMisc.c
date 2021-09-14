/*******************************************************************************
 * ztbmMisc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztbmMisc.c 12157 2014-04-23 03:50:31Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztbmMisc.c
 * @brief Gliese Balancer Misc Routines
 */

#include <stl.h>
#include <cml.h>
#include <sll.h>
#include <ztbDef.h>
#include <ztbm.h>

stlGetOptOption gZtbmOptOption[] =
{
    { "msgqkey",        'm',  STL_TRUE,   "message queue key for startup" },
    { NULL,             '\0', STL_FALSE,  NULL }
};


stlStatus ztbmDaemonize( stlBool         aDaemonize,
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


