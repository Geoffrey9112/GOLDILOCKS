/*******************************************************************************
 * stlSemaphore.h
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


#ifndef _STLSEMAPHORE_H_
#define _STLSEMAPHORE_H_ 1

/**
 * @file stlSemaphore.h
 * @brief Standard Layer Semaphore Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS

/**
 * @defgroup stlSemaphore Semaphore
 * @ingroup STL 
 * @{
 */

/*
 * unnamed semaphore
 */
stlStatus stlCreateSemaphore(stlSemaphore  * aSemaphore,
                             stlChar       * aSemName,
                             stlUInt32       aInitValue,
                             stlErrorStack * aErrorStack);

stlStatus stlAcquireSemaphore(stlSemaphore  * aSemaphore,
                              stlErrorStack * aErrorStack);

stlStatus stlTryAcquireSemaphore(stlSemaphore  * aSemaphore,
                                 stlBool       * aIsSuccess,
                                 stlErrorStack * aErrorStack);

stlStatus stlTimedAcquireSemaphore(stlSemaphore  * aSemaphore,
                                   stlInterval     aTimeout,
                                   stlBool       * aIsTimedOut,
                                   stlErrorStack * aErrorStack);

stlStatus stlReleaseSemaphore(stlSemaphore  * aSemaphore,
                              stlErrorStack * aErrorStack);

stlStatus stlDestroySemaphore(stlSemaphore  * aSemaphore,
                              stlErrorStack * aErrorStack);



/*
 * named semaphore
 */
stlStatus stlCreateNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                   stlChar           * aSemName,
                                   stlUInt32           aInitValue,
                                   stlErrorStack     * aErrorStack );

stlStatus stlOpenNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                 stlChar           * aSemName,
                                 stlErrorStack     * aErrorStack );

stlStatus stlAcquireNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                    stlErrorStack     * aErrorStack );

stlStatus stlTryAcquireNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                       stlBool           * aIsSuccess,
                                       stlErrorStack     * aErrorStack );

stlStatus stlTimedAcquireNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                         stlInterval         aTimeout,
                                         stlBool           * aIsTimedOut,
                                         stlErrorStack     * aErrorStack );

stlStatus stlReleaseNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                    stlErrorStack     * aErrorStack );

stlStatus stlCloseNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                  stlErrorStack     * aErrorStack );
    
stlStatus stlDestroyNamedSemaphore( stlNamedSemaphore * aSemaphore,
                                   stlErrorStack     * aErrorStack );

stlStatus stlGetSemaphoreValue( stlSemaphore  * aSemaphore,
                                stlInt32      * aSemValue,
                                stlErrorStack * aErrorStack );

/** @} */
    
STL_END_DECLS

#endif /* _STLSEMAPHORE_H_ */
