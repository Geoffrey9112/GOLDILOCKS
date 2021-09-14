/*******************************************************************************
 * stg.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stg.h 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>

#ifndef _STG_H_
#define _STG_H_ 1

STL_BEGIN_DECLS

stlStatus stgOpenFile( stlLogger     * aLogger,
                       stlInt64        aLogSize,
                       stlFile       * aFile,
                       stlErrorStack * aErrorStack );

stlStatus stgCloseFile( stlLogger     * aLogger,
                        stlFile       * aFile,
                        stlErrorStack * aErrorStack );

stlStatus stgPrintTimestamp( stlLogger     * aLogger,
                             stlErrorStack * aErrorStack );

stlStatus stgAddLogMsg( stlLogger     * aLogger,
                        const stlChar * aFormat,
                        va_list         aVarArgList,
                        stlErrorStack * aErrorStack );

stlStatus stgPrintFileHeader( stlLogger     * aLogger,
                              stlErrorStack * aErrorStack );

STL_END_DECLS

#endif /* _STG_H_ */
