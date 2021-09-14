/*******************************************************************************
 * stlTime.h
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


#ifndef _STLTIME_H_
#define _STLTIME_H_ 1

/**
 * @file stlTime.h
 * @brief Standard Layer Time Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS

/**
 * @defgroup stlTime Time
 * @ingroup STL 
 * @{
 */

stlStatus stlPutAnsiTime( stlTime        * aResult,
                          stlAnsiTime      aInput,
                          stlErrorStack  * aErrorStack );


stlTime stlNow();

void stlMakeExpTimeByGMT( stlExpTime * aExpTime,
                          stlTime      aTime);

void stlMakeExpTimeByLocalTz( stlExpTime * aExpTime,
                              stlTime      aTime);

void stlSleep( stlInterval aInterval );

void stlBusyWait( void );

stlStatus stlCreateTimer( stlInt32        aSigNo,
                          stlInt64        aNanoSecond,
                          stlTimer      * aTimerId,
                          stlErrorStack * aErrorStack );
stlStatus stlDestroyTimer( stlTimer        aTimerId,
                           stlErrorStack * aErrorStack );
/** @} */
    
STL_END_DECLS

#endif /* _STLTIME_H_ */
