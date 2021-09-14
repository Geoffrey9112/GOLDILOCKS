/*******************************************************************************
 * knlXaHash.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlXaHash.h 1832 2011-09-06 03:19:05Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _KNLXAHASH_H_
#define _KNLXAHASH_H_ 1

/**
 * @file knlXaHash.h
 * @brief Kernel Layer Conditional Variable Routines
 */

/**
 * @defgroup knlXaHash XA Context Hash
 * @ingroup knExternal
 * @{
 */

stlStatus knlFindXaContext( stlXid        * aXid,
                            stlInt32        aHashValue,
                            knlXaContext ** aContext,
                            stlBool       * aFound,
                            knlEnv        * aEnv );

stlStatus knlLinkXaContext( knlXaContext * aContext,
                            stlBool      * aIsSuccess,
                            knlEnv       * aEnv );

stlStatus knlUnlinkXaContext( knlXaContext * aContext,
                              knlEnv       * aEnv );

stlInt32 knlGetXidHashValue( stlXid * aXid );

stlBool knlIsEqualXid( stlXid       * aXid,
                       stlInt32       aHashValue,
                       knlXaContext * aContext );

/** @} */
    
#endif /* _KNLXAHASH_H_ */
