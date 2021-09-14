/*******************************************************************************
 * ztpuFile.h
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


#ifndef _ZTPUFILE_H_
#define _ZTPUFILE_H_ 1

#include <dtl.h>

/**
 * @file ztpuFile.h
 * @brief Gliese Embedded SQL in C precompiler utility functions
 */

/**
 * @defgroup ztpuFile Gliese Embedded SQL in C precompiler file utility functions
 * @ingroup ztp
 * @{
 */


stlStatus ztpuFindFileName(stlChar **aFileName,
                           stlChar  *aFilePath);

stlStatus ztpuMatchInOutFilePath();

stlStatus ztpuMakeInFilePath();

stlStatus ztpuReadFile(stlFile        *aInFile,
                       stlAllocator   *aAllocator,
                       stlErrorStack  *aErrorStack,
                       stlChar       **aBuffer,
                       stlSize        *aLength);

stlStatus ztpuWriteFile(stlFile       *aFile,
                        void          *aBuffer,
                        stlSize        aLength,
                        stlErrorStack *aErrorStack);

stlStatus ztpuWriteOutputHeader(stlFile       *aFile,
                                stlChar       *aInFileName,
                                stlChar       *aOutFileName,
                                stlChar       *aOutputHeader,
                                stlSize        aOutputHeaderSize,
                                stlErrorStack *aErrorStack);

stlStatus ztpuSendStringToOutFile(stlFile        *aOutFile,
                                  stlChar        *aStr,
                                  stlErrorStack  *aErrorStack);

/** @} */

#endif /* _ZTPUFILE_H_ */
