/*******************************************************************************
 * stlLogger.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stlLogger.h 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _STLLOGGER_H_
#define _STLLOGGER_H_ 1

/**
 * @file stlLogger.h
 * @brief Standard Layer Trace Logger Routines
 */

STL_BEGIN_DECLS

/**
 * @defgroup stlLogger Trace Logger
 * @ingroup STL 
 * @{
 */

stlStatus stlCreateLogger( stlLogger     * aLogger,
                           stlChar       * aFileName,
                           stlChar       * aFilePath,
                           stlBool         aNeedLock,
                           stlInt32        aMaxFileSize,
                           stlErrorStack * aErrorStack );

stlStatus stlLogMsg( stlLogger     * aLogger,
                     stlErrorStack * aErrorStack,
                     const stlChar * aFormat,
                     ... );

stlStatus stlDestoryLogger( stlLogger     * aLogger,
                            stlErrorStack * aErrorStack );

/** @} */
    
STL_END_DECLS

#endif /* _STLLOGGER_H_ */
