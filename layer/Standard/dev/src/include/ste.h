/*******************************************************************************
 * ste.h
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

#ifndef _STE_H_
#define _STE_H_ 1

#include <stlDef.h>
#if defined( STC_HAVE_ERRNO_H )
#include <errno.h>
#endif

void steSetSystemError( stlInt32        aErrorNo,
                        stlErrorStack * aErrorStack );

void steDefaultFatalHandler( const stlChar * aCauseString,
                             void          * aSigInfo,
                             void          * aContext );

void steFatalHandler( stlInt32   aArg,
                      void     * aSigInfo,
                      void     * aContext );

#endif /* _STE_H_ */
