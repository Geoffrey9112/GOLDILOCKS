/*******************************************************************************
 * ztpMain.h
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


#ifndef _ZTPMAIN_H_
#define _ZTPMAIN_H_ 1

/**
 * @file ztpMain.h
 * @brief Gliese Embedded SQL in C precompiler main 
 */

/**
 * @defgroup ztpMain Gliese Embedded SQL in C precompiler
 * @ingroup ztp
 * @{
 */

stlStatus ztpParseOptions( stlInt32       aArgc,
                           stlChar       *aArgv[],
                           stlAllocator  *aAllocator,
                           stlErrorStack *aErrorStack );
stlStatus ztpShowVersion();
stlStatus ztpPrintUsage(stlChar *aProgramName);
stlStatus ztpConvertFileStream(ztpConvertContext *aContext);


/** @} */

#endif /* _ZTPMAIN_H_ */
