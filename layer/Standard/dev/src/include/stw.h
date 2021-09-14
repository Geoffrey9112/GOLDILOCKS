/*******************************************************************************
 * stw.h
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


#ifndef _STW_H_
#define _STW_H_ 1

STL_BEGIN_DECLS

stlStatus stwPoll( stlPollFd     * aPollFd,
                   stlInt32        aPollFdNum,
                   stlInt32      * aSigFdNum,
                   stlInterval     aTimeout,
                   stlErrorStack * aErrorStack );

stlStatus stwCreatePollSet( stlPollSet    * aPollSet,
                            stlInt32        aPollFdNum,
                            stlErrorStack * aErrorStack );

stlStatus stwDestroyPollSet( stlPollSet    * aPollSet,
                             stlErrorStack * aErrorStack );

stlStatus stwAddSockPollSet( stlPollSet        * aPollSet,
                             const stlPollFdEx * aPollFd,
                             stlErrorStack     * aErrorStack );

stlStatus stwRemoveSockPollSet( stlPollSet        * aPollSet,
                                const stlPollFdEx * aPollFd,
                                stlErrorStack     * aErrorStack );

stlStatus stwPollPollSet( stlPollSet     * aPollSet,
                          stlInterval      aTimeout,
                          stlInt32       * aSigFdNum,
                          stlPollFdEx   ** aResultSet,
                          stlErrorStack  * aErrorStack );

STL_END_DECLS

#endif /* _STW_H_ */
