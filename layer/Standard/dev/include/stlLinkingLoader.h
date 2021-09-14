/*******************************************************************************
 * stlLinkingLoader.h
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


#ifndef _STLLINKINGLOADER_H_
#define _STLLINKINGLOADER_H_ 1

/**
 * @file stlLinkingLoader.h
 * @brief Standard Layer Time Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS

/**
 * @defgroup stlLinkingLoader Dynamic Linking Loader
 * @ingroup STL 
 * @{
 */

stlStatus stlOpenLibrary( stlChar        * aLibraryPath,
                          stlDsoHandle   * aLibraryHandle,
                          stlErrorStack  * aErrorStack );

stlStatus stlCloseLibrary( stlDsoHandle    aLibraryHandle,
                           stlErrorStack * aErrorStack );

stlStatus stlGetSymbol( stlDsoHandle      aLibraryHandle,
                        stlChar         * aSymbolName,
                        stlDsoSymHandle * aSymbolAddr,
                        stlErrorStack   * aErrorStack );

/** @} */
    
STL_END_DECLS

#endif /* _STLLINKINGLOADER_H_ */
