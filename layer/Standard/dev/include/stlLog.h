/*******************************************************************************
 * stlLog.h
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


#ifndef _STLLOG_H_
#define _STLLOG_H_ 1

/**
 * @file stlLog.h
 * @brief Standard Layer Log Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS

/**
 * @defgroup stlLog Log
 * @ingroup STL 
 * @{
 */

void stlLogCallstack( void * aSigInfo, void * aContext );

void stlLogMessage( const stlChar * aFormat, ... );

/** @} */
    
STL_END_DECLS

#endif /* _STLUSER_H_ */
