/*******************************************************************************
 * sto.h
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


#ifndef _STO_H_
#define _STO_H_ 1

#include <stlDef.h>

STL_BEGIN_DECLS

stlStatus stoOpen( stlChar        * aLibraryPath,
                   stlDsoHandle   * aLibraryHandle,
                   stlErrorStack  * aErrorStack );

stlStatus stoClose( stlDsoHandle    aLibraryHandle,
                    stlErrorStack * aErrorStack );

stlStatus stoGetSymbol( stlDsoHandle      aLibraryHandle,
                        stlChar         * aSymbolName,
                        stlDsoSymHandle * aSymbolAddr,
                        stlErrorStack   * aErrorStack );

STL_END_DECLS

#endif /* _STO_H_ */
