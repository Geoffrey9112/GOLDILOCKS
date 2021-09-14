/*******************************************************************************
 * stu.h
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


#ifndef _STU_H_
#define _STU_H_ 1

#include <stlDef.h>

#if defined(STC_HAVE_PWD_H)
#include <pwd.h>
#endif

STL_BEGIN_DECLS

stlStatus stuGetCurrentUserID(stlUserID     * aUserID,
                              stlChar       * aUserBuffer,
                              stlInt32        aUserBufferLength,
                              stlGroupID    * aGroupID,
                              stlChar       * aGroupBuffer,
                              stlInt32        aGroupBufferLength,
                              stlErrorStack * aErrorStack);

stlStatus stuGetUserNameByUserID(stlChar       * aUserName,
                                 stlInt32        aUserNameLen,
                                 stlUserID       aUserID,
                                 stlErrorStack * aErrorStack);

stlStatus stuGetUserIDByUserName(stlUserID     * aUserID,
                                 stlChar       * aUserBuffer,
                                 stlInt32        aUserBufferLength,
                                 stlGroupID    * aGroupID,
                                 stlChar       * aGroupBuffer,
                                 stlInt32        aGroupBufferLength,
                                 const stlChar * aUserName,
                                 stlErrorStack * aErrorStack);

stlStatus stuGetHomeDirByUserName(stlChar       * aDirName,
                                  stlInt32        aDirNameLen,
                                  const stlChar * aUserName,
                                  stlErrorStack * aErrorStack);

stlBool stuCompareUserID( stlUserID aUserID1,
                          stlUserID aUserID2 );

stlStatus stuGetGroupNameByGroupID(stlChar       * aGroupName,
                                   stlInt32        aGroupNameLen,
                                   stlGroupID      aGroupID,
                                   stlErrorStack * aErrorStack);

stlStatus stuGetGroupIDByGroupName(stlGroupID    * aGroupID,
                                   stlChar       * aGroupBuffer,
                                   stlInt32        aGroupBufferLength,
                                   const stlChar * aGroupName,
                                   stlErrorStack * aErrorStack);

stlBool stuCompareGroupID( stlGroupID aGroupID1,
                           stlGroupID aGroupID2 );

stlStatus stuCleanup();

STL_END_DECLS

#endif /* _STU_H_ */
