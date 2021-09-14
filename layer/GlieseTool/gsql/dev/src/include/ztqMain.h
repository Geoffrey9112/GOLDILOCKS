/*******************************************************************************
 * ztqMain.h
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


#ifndef _ZTQMAIN_H_
#define _ZTQMAIN_H_ 1

/**
 * @file ztqMain.h
 * @brief Main Routines
 */

/**
 * @defgroup ztqMain Main
 * @ingroup ztq
 * @{
 */

stlStatus ztqProcessCommandStream( stlFile       * aInFile,
                                   stlBool         aDisplayPrompt,
                                   stlBool         aEcho,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack );

stlStatus ztqProcessCommand( stlChar       * aCommandString,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack );

stlStatus ztqProcessSql( stlChar       * aCommandString,
                         stlAllocator  * aAllocator,
                         stlBool         aPrintResult,
                         stlErrorStack * aErrorStack );

stlBool ztqIsGSQLCommand( stlChar * aString );

/** @} */

#endif /* _ZTQMAIN_H_ */
