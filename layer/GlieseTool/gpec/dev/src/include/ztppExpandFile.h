/*******************************************************************************
 * ztppExpandFile.h
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


#ifndef _ZTPPEXPANDFILE_H_
#define _ZTPPEXPANDFILE_H_ 1

#include <dtl.h>

/**
 * @file ztppExpandFile.h
 * @brief Preprocessor expand file (translation phse 3~4)
 */

/**
 * @defgroup ztppExpandFile Preprocessor expand file (translation phse 3~4)
 * @ingroup ztp
 * @{
 */

stlStatus ztppReadFile(stlFile        *aInFile,
                       stlAllocator   *aAllocator,
                       stlErrorStack  *aErrorStack,
                       stlChar       **aBuffer,
                       stlSize        *aLength);
stlStatus ztppSendStringToOutFile(ztpPPParseParam *aParam,
                                  stlChar         *aStr);
stlStatus ztppBypassCCode(ztpPPParseParam *aParam);
stlStatus ztppBypassCCodeByLoc(ztpPPParseParam *aParam,
                               stlInt32         aStartLoc,
                               stlInt32         aEndLoc);
stlStatus ztppExpandHeaderFileInternal(stlAllocator    *aAllocator,
                                       stlErrorStack   *aErrorStack,
                                       stlFile         *aResultFile,
                                       stlChar         *aReadFilePath);
stlStatus ztppExpandHeaderFile(stlAllocator    *aAllocator,
                               stlErrorStack   *aErrorStack,
                               stlChar         *aReadFilePath,
                               stlChar         *aResultFilePath);

/** @} */

#endif /* _ZTPPEXPANDFILE_H_ */
