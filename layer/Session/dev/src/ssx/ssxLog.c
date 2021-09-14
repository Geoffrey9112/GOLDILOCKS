/*******************************************************************************
 * ssxLog.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ssxLog.c 9033 2013-07-17 08:40:27Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file ssxLog.c
 * @brief XA Context Logging Routines
 */

#include <qll.h>
#include <sslDef.h>
#include <sslEnv.h>
#include <ssDef.h>
#include <ssx.h>

extern ssgWarmupEntry  * gSsgWarmupEntry;


stlStatus ssxLogMsg( stlXid    * aXid,
                     stlChar   * aCmdString,
                     stlInt64    aFlags,
                     stlInt32    aReturn,
                     sslEnv    * aEnv )
{
    stlChar   sXidString[STL_XID_STRING_SIZE];
    stlUInt32 sSessionId;

    STL_TRY_THROW( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_XA,
                                                KNL_ENV( aEnv ) )
                   == STL_TRUE, RAMP_FINISH );
    
    if( aXid != NULL )
    {
        STL_TRY( dtlXidToString( aXid,
                                 STL_XID_STRING_SIZE,
                                 sXidString,
                                 KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        sXidString[0] = '\0';
    }

    STL_TRY( knlGetSessionEnvId( KNL_SESS_ENV(aEnv),
                                 &sSessionId,
                                 KNL_ENV(aEnv) ) == STL_SUCCESS );

    if( aReturn == SSL_XA_OK )
    {
        (void) knlLogMsgUnsafe( &gSsgWarmupEntry->mXaLogger,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "%s() complete - session(%d), xid(%s), flags(%lx)\n",
                                aCmdString,
                                sSessionId,
                                sXidString,
                                aFlags );
    }
    else
    {
        (void) knlLogMsgUnsafe( &gSsgWarmupEntry->mXaLogger,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "%s() failed - session(%d), xid(%s), flags(%lx), xa_error(%d)\n",
                                aCmdString,
                                sSessionId,
                                sXidString,
                                aFlags,
                                aReturn );

        (void) knlLogErrorStackUnsafe( &gSsgWarmupEntry->mXaLogger,
                                       KNL_ERROR_STACK( aEnv ),
                                       KNL_ENV( aEnv ) );
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

