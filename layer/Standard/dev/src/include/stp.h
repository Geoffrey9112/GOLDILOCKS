/*******************************************************************************
 * stp.h
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


#ifndef _STP_H_
#define _STP_H_ 1

#include <stlDef.h>

STL_BEGIN_DECLS

stlStatus stpCreate( stlSemaphore  * aSemaphore,
                     stlChar       * aSemName,
                     stlUInt32       aInitValue,
                     stlErrorStack * aErrorStack );

stlStatus stpAcquire( stlSemHandle  * aHandle,
                      stlErrorStack * aErrorStack);

stlStatus stpTryAcquire( stlSemHandle  * aHandle,
                         stlBool       * aIsSuccess,
                         stlErrorStack * aErrorStack);

stlStatus stpTimedAcquire( stlSemHandle  * aHandle,
                           stlInterval     aTimeout,
                           stlBool       * aIsTimedOut,
                           stlErrorStack * aErrorStack);

stlStatus stpRelease( stlSemHandle  * aHandle,
                      stlErrorStack * aErrorStack);

stlStatus stpDestroy( stlSemaphore  * aSemaphore,
                      stlErrorStack * aErrorStack);

stlStatus stpNamedCreate( stlNamedSemaphore * aSemaphore,
                          stlChar           * aSemName,
                          stlUInt32           aInitValue,
                          stlErrorStack     * aErrorStack );

stlStatus stpOpen( stlNamedSemaphore * aSemaphore,
                   stlChar           * aSemName,
                   stlErrorStack     * aErrorStack );

stlStatus stpNamedAcquire( stlNamedSemHandle  * aHandle,
                           stlErrorStack      * aErrorStack );

stlStatus stpNamedRelease( stlNamedSemHandle  * aHandle,
                           stlErrorStack      * aErrorStack );

stlStatus stpClose( stlNamedSemaphore * aSemaphore,
                    stlErrorStack     * aErrorStack);

stlStatus stpUnlink( stlNamedSemaphore * aSemaphore,
                     stlErrorStack     * aErrorStack);

stlStatus stpGetValue( stlSemaphore  * aSemaphore,
                       stlInt32      * aSemValue,
                       stlErrorStack * aErrorStack );

STL_END_DECLS

#endif /* _STP_H_ */
