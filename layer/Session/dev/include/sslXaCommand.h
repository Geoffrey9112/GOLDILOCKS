/*******************************************************************************
 * sslXaCommand.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sslXaCommand.h 8840 2013-06-28 06:11:37Z htkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _SSLXACOMMAND_H_
#define _SSLXACOMMAND_H_ 1

/**
 * @file sslXaCommand.h
 * @brief Session XA Command
 */

stlInt32 sslXaClose( sslEnv * aEnv );

stlInt32 sslXaStart( stlXid    * aXid,
                     stlInt32    aRmId,
                     stlInt64    aFlags,
                     sslEnv    * aEnv );

stlInt32 sslXaEnd( stlXid    * aXid,
                   stlInt32    aRmId,
                   stlInt64    aFlags,
                   sslEnv    * aEnv );

stlInt32 sslXaCommit( stlXid    * aXid,
                      stlInt32    aRmId,
                      stlInt64    aFlags,
                      sslEnv    * aEnv );

stlInt32 sslXaRollback( stlXid    * aXid,
                        stlInt32    aRmId,
                        stlInt64    aFlags,
                        sslEnv    * aEnv );

stlInt32 sslXaPrepare( stlXid    * aXid,
                       stlInt32    aRmId,
                       stlInt64    aFlags,
                       sslEnv    * aEnv );

stlInt32 sslXaForget( stlXid    * aXid,
                      stlInt32    aRmId,
                      stlInt64    aFlags,
                      sslEnv    * aEnv );

stlInt32 sslXaRecover( stlXid    * aXids,
                       stlInt64    aCount,
                       stlInt32    aRmId,
                       stlInt64    aFlags,
                       sslEnv    * aEnv );

stlStatus sslCleanupContext( knlXaContext  * aContext,
                             sslSessionEnv * aDeadSessionEnv,
                             sslEnv        * aEnv );

#endif /* _SSLXACOMMAND_H_ */
