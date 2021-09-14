/*******************************************************************************
 * ssx.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ssx.h 7850 2013-03-28 01:27:42Z htkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _SSX_H_
#define _SSX_H_ 1

/**
 * @file ssx.h
 * @brief XA related internal prototypes
 */

/*
 * XA context related functions
 */

stlStatus ssxLatchContext( knlXaContext * aContext,
                           sslEnv       * aEnv );

stlStatus ssxUnlatchContext( knlXaContext * aContext,
                             sslEnv       * aEnv );

stlStatus ssxWaitContext( knlXaContext * aContext,
                          stlBool      * aIsSuccess,
                          sslEnv       * aEnv );

stlStatus ssxWakeupContext( knlXaContext * aContext,
                            sslEnv       * aEnv );

stlStatus ssxAllocContextArray( sslEnv * aEnv );

stlStatus ssxCreateContext( stlUInt32       aSessionId,
                            stlXid        * aXid,
                            knlXaContext ** aContext,
                            sslEnv        * aEnv );

stlStatus ssxInitContext( knlXaContext * aContext,
                          sslEnv       * aEnv );

stlStatus ssxDestroyContext( knlXaContext * aContext,
                             sslEnv       * aEnv );

stlStatus ssxAssociate( stlUInt32      aSessionId,
                        knlXaContext * aContext,
                        sslEnv       * aEnv );

stlStatus ssxDissociate( knlXaContext  * aContext,
                         sslSessionEnv * aSessionEnv,
                         stlBool         aIsCleanUp,
                         sslEnv        * aEnv );


/*
 * transction relatioed functions
 */

stlStatus ssxGetLocalTrans( ssxLocalTrans * sLocalTrans,
                            sslEnv        * aEnv );

stlStatus ssxSetLocalTrans( ssxLocalTrans * sLocalTrans,
                            sslEnv        * aEnv );

stlStatus ssxCommit( smlTransId     aTransId,
                     knlXaContext * aXaContext,
                     sslEnv       * aEnv );

stlStatus ssxRollback( smlTransId     aTransId,
                       knlXaContext * aXaContext,
                       sslEnv       * aEnv );


/*
 * Recover scanning related functions
 */

stlStatus ssxDeleteRecover( stlXid   * aXid,
                            stlBool  * aDeleted,
                            sslEnv   * aEnv );

stlStatus ssxResetRecover( sslEnv * aEnv );

stlStatus ssxOpenRecover( sslEnv * aEnv );

stlBool ssxIsOpenRecover( sslEnv * aEnv );

stlStatus ssxFetchRecover( stlXid   * aXids,
                           stlInt64   aRequireCount,
                           stlInt64 * aFetchedCount,
                           sslEnv   * aEnv );

stlStatus ssxCloseRecover( sslEnv * aEnv );


/*
 * Trace logging related functions
 */

stlStatus ssxLogMsg( stlXid    * aXid,
                     stlChar   * aCmdString,
                     stlInt64    aFlags,
                     stlInt32    aReturn,
                     sslEnv    * aEnv );

#endif /* _SSX_H_ */
