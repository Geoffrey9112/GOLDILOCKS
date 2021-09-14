/*******************************************************************************
 * smrLogMirror.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smrLogMirror.h 9474 2013-08-29 03:22:39Z mkkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _SMRLOGMIRROR_H_
#define _SMRLOGMIRROR_H_ 1

/**
 * @file smrLogMirror.h
 * @brief Storage Manager Layer Log Mirror Component Internal Routines
 */

/**
 * @defgroup smrLogMirror  LogMirror
 * @ingroup smrInternal
 * @{
 */

stlStatus smrCreateLogMirrorArea( smlEnv * aEnv );

stlStatus smrCheckLogMirrorBuffer( smlEnv * aEnv );

stlStatus smrWriteLogMirrorBuffer( void      * aBuffer,
                                   stlInt64    aFlushBytes,
                                   stlBool     aIsFileHdr,
                                   smlEnv    * aEnv );


/** @} */

#endif /* _SMRLOGMIRROR_H_ */
