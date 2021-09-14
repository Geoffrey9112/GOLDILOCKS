/*******************************************************************************
 * stlPoll.h
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


#ifndef _STLPOLL_H_
#define _STLPOLL_H_ 1

/**
 * @file stlPoll.h
 * @brief Standard Layer Poll Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS


/**
 * @defgroup stlPoll Poll
 * @ingroup STL 
 * @{
 */

stlStatus stlPoll( stlPollFd     * aPollFd,
                   stlInt32        aPollFdNum,
                   stlInt32      * aSigFdNum,
                   stlInterval     aTimeout,
                   stlErrorStack * aErrorStack );

stlStatus stlCreatePollSet( stlPollSet    * aPollSet,
                            stlInt32        aPollFdNum,
                            stlErrorStack * aErrorStack );

stlStatus stlDestroyPollSet( stlPollSet    * aPollSet,
                             stlErrorStack * aErrorStack );

stlStatus stlAddSockPollSet( stlPollSet        * aPollSet,
                             const stlPollFdEx * aPollFd,
                             stlErrorStack     * aErrorStack );

stlStatus stlRemoveSockPollSet( stlPollSet        * aPollSet,
                                const stlPollFdEx * aPollFd,
                                stlErrorStack     * aErrorStack );

stlStatus stlPollPollSet( stlPollSet     * aPollSet,
                          stlInterval      aTimeout,
                          stlInt32       * aSigFdNum,
                          stlPollFdEx   ** aResultSet,
                          stlErrorStack  * aErrorStack );

stlStatus stlDispatchPollSet( stlPollSet       * aPollSet,
                              stlInterval        aTimeout,
                              stlPollSetCallback aCallback,
                              stlErrorStack    * aErrorStack );

/** @} */
    
STL_END_DECLS

#endif /* _STLATOMIC_H_ */
