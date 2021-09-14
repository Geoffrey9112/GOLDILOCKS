/*******************************************************************************
 * ztppCommon.h
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


#ifndef _ZTPPCOMMON_H_
#define _ZTPPCOMMON_H_ 1

/**
 * @file ztppCommon.h
 * @brief C Preprocessor Parser for Gliese Embedded SQL
 */

/**
 * @defgroup ztppCommon C Preprocessor Parser
 * @ingroup ztp
 * @{
 */

stlStatus ztppFindMacro( stlChar          *aMacroName,
                         ztpMacroSymbol  **aMacroSymbol,
                         stlErrorStack    *aErrorStack );

stlStatus ztppConvertLiteral( stlChar          *aLiteral,
                              stlInt64         *aResult,
                              stlErrorStack    *aErrorStack );

stlStatus ztppReplaceMacro( stlChar             *aReplacedBuffer,
                            stlChar             *aMacroContents,
                            ztpArgNameList      *aArgNameList,
                            ztpPPParameterList  *aParameter,
                            stlInt32             aParameterCount,
                            stlErrorStack       *aErrorStack );

stlStatus ztppReplaceText( stlChar        *aSrcText,
                           stlChar        *aSrcPattern,
                           stlChar        *aDestText,
                           stlChar        *aDestPattern,
                           stlBool        *aIsReplace );

stlChar *ztppScanToken( stlChar            **aGivenString,
                        ztpPPMacroTokenId   *aTokenId );

/** @} */

#endif /* _ZTPPCOMMON_H_ */
