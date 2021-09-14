/*******************************************************************************
 * ztppMacro.h
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


#ifndef _ZTPPMACRO_H_
#define _ZTPPMACRO_H_ 1

#include <dtl.h>

/**
 * @file ztppMacro.h
 * @brief Preprocessor macro handler (translation phse 3~4)
 */

/**
 * @defgroup ztppMacro Preprocessor macro handler (translation phse 3~4)
 * @ingroup ztp
 * @{
 */

stlInt32  ztppCompareMacroSymbolFunc(void *aKeyA, void *aKeyB);
stlUInt32 ztppMacroSymbolHashFunc(void * aKey, stlUInt32 aBucketCount);

stlStatus ztppCreateSymbolTable(stlAllocator   *aAllocator,
                                stlErrorStack  *aErrorStack);

stlStatus ztppDestroySymbolTable(stlErrorStack  *aErrorStack);

stlStatus ztppAddMacroSymbol(stlErrorStack   *aErrorStack,
                             ztpMacroSymbol  *aMacroSymbol);

stlStatus ztppLookupMacroSymbol(stlErrorStack    *aErrorStack,
                                stlChar          *aMacroName,
                                ztpMacroSymbol  **aMacroSymbol);

stlStatus ztppDeleteMacroSymbol(stlErrorStack   *aErrorStack,
                                ztpMacroSymbol  *aMacroSymbol);



/** @} */

#endif /* _ZTPPMACRO_H_ */
