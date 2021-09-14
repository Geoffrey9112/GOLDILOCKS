/*******************************************************************************
 * stlUser.h
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


#ifndef _STLUSER_H_
#define _STLUSER_H_ 1

/**
 * @file stlUser.h
 * @brief Standard Layer Shared User Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS

/**
 * @defgroup stlUser User
 * @ingroup STL 
 * @{
 */

stlStatus stlGetCurrentUserID(stlUserID     * aUserID,
                              stlChar       * aUserBuffer,
                              stlInt32        aUserBufferLength,
                              stlGroupID    * aGroupID,
                              stlChar       * aGroupBuffer,
                              stlInt32        aGroupBufferLength,
                              stlErrorStack * aErrorStack);

stlStatus stlGetUserNameByUserID(stlChar       * aUserName,
                                 stlInt32        aUserNameLen,
                                 stlUserID       aUserID,
                                 stlErrorStack * aErrorStack);

stlStatus stlGetUserIDByUserName(stlUserID     * aUserID,
                                 stlChar       * aUserBuffer,
                                 stlInt32        aUserBufferLength,
                                 stlGroupID    * aGroupID,
                                 stlChar       * aGroupBuffer,
                                 stlInt32        aGroupBufferLength,
                                 const stlChar * aUserName,
                                 stlErrorStack * aErrorStack);

stlStatus stlGetHomeDirByUserName(stlChar       * aDirName,
                                  stlInt32        aDirNameLen,
                                  const stlChar * aUserName,
                                  stlErrorStack * aErrorStack);

stlBool stlCompareUserID( stlUserID       aUserID1,
                          stlUserID       aUserID2 );

stlStatus stlGetGroupNameByGroupID(stlChar       * aGroupName,
                                   stlInt32        aGroupNameLen,
                                   stlGroupID      aGroupID,
                                   stlErrorStack * aErrorStack);

stlStatus stlGetGroupIDByGroupName(stlGroupID    * aGroupID,
                                   stlChar       * aGroupBuffer,
                                   stlInt32        aGroupBufferLength,
                                   const stlChar * aGroupName,
                                   stlErrorStack * aErrorStack);

stlBool stlCompareGroupID( stlGroupID      aGroupID1,
                           stlGroupID      aGroupID2 );

/** @} */
    
STL_END_DECLS

#endif /* _STLUSER_H_ */
