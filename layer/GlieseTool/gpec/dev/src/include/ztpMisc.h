/*******************************************************************************
 * ztpMisc.h
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


#ifndef _ZTPMISC_H_
#define _ZTPMISC_H_ 1

#include <dtl.h>

/**
 * @file ztpMisc.h
 * @brief Gliese Embedded SQL in C precompiler miscellaneous functions
 */

/**
 * @defgroup ztpMisc Gliese Embedded SQL in C precompiler miscellaneous functions
 * @ingroup ztp
 * @{
 */

void ztpPrintErrorStack(stlErrorStack * aErrorStack);

stlChar * ztpMakeErrPosString( stlChar * aSQLString,
                               stlInt32  aPosition );

stlStatus ztpExtractNParseSql(ztpCParseParam   *aParam,
                              stlInt32          aStartPos,
                              stlInt32         *aEndPos);

/************************************************************************
 * Cursor
 ************************************************************************/

stlStatus ztpAddCursorToSymTab(ztpSymTabHeader  *aSymTab,
                               ztpDeclCursor    *aDeclCursor);
void ztpFindCursorInSymTab( stlChar        * aCursorName,
                            ztpDeclCursor ** aDeclCursor );
stlStatus ztpMakeNameFromNameTag(ztpCParseParam *aParam,
                                 ztpNameTag     *aSrc,
                                 stlChar        *aDest);
stlStatus ztpMakeExceptionString(ztpCParseParam *aParam,
                                 stlInt32        aCondType,
                                 stlChar        *aExceptionStr);
stlStatus ztpCreateSymTab(stlAllocator   *aAllocator,
                          stlErrorStack  *aErrorStack);
stlStatus ztpDestroySymTab(stlAllocator   *aAllocator,
                           stlErrorStack  *aErrorStack);

/** @} */

#endif /* _ZTPMISC_H_ */
