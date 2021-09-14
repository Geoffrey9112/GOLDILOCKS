/*******************************************************************************
 * stlBacktrace.h
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


#ifndef _STLBACKTRACE_H_
#define _STLBACKTRACE_H_ 1

/**
 * @file stlBacktrace.h
 * @brief Standard Layer Backtrace Routines
 */


STL_BEGIN_DECLS


/**
 * @defgroup stlBacktrace Backtrace
 * @ingroup STL 
 * @{
 */

stlInt32 stlBacktrace( void     ** aBuffer,
                       stlInt32    aSize,
                       void      * aSigInfo,
                       void      * aContext );

stlInt32 stlBacktraceToFile( stlFile   * aFile,
                             void     ** aBuffer,
                             stlInt32    aSize,
                             void      * aSigInfo,
                             void      * aContext );

void stlBacktraceSymbolsToStr( void     ** aFrame,
                               void      * aBuffer,
                               stlInt32    aFrameSize,
                               stlInt32    aBufferSize );

void stlBacktraceSymbolsToFile( void     ** aBuffer,
                                stlInt32    aSize,
                                stlFile   * aFile );

/** @} */
    
STL_END_DECLS

#endif /* _STLBACKTRACE_H_ */
