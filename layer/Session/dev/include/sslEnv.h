/*******************************************************************************
 * sslEnv.h
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


#ifndef _SSLENV_H_
#define _SSLENV_H_ 1

/**
 * @file sslEnv.h
 * @brief Session Environment Routines
 */

stlStatus sslAllocEnv( sslEnv         ** aAllocEnv,
                       stlErrorStack   * aErrorStack );

stlStatus sslFreeEnv( sslEnv        * aFreeEnv,
                      stlErrorStack * aErrorStack );

stlStatus sslInitializeEnv( sslEnv     * aEnv,
                            knlEnvType   aEnvType );

stlStatus sslFinalizeEnv( sslEnv * aEnv );

#endif /* _SSLENV_H_ */
