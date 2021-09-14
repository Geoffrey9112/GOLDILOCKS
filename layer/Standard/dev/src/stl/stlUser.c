/*******************************************************************************
 * stlUser.c
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

/**
 * @file stlUser.c
 * @brief Standard Layer User Routines
 */

#include <stlUser.h>
#include <stu.h>

/**
 * @addtogroup stlUser
 * @{
 */

/**
 * @brief 호출 프로세스의 UserID와 GroupID를 반환한다.
 * @param[out] aUserID 반환되는 사용자의 UserID
 * @param[in,out] aUserBuffer aUserID를 위한 버퍼
 * @param[in,out] aUserBufferLength aUserBuffer의 길이
 * @param[out] aGroupID 반환되는 사용자의 GroupID
 * @param[in,out] aGroupBuffer aGroupID를 위한 버퍼
 * @param[in,out] aGroupBufferLength aGroupBuffer의 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * @endcode
 */
stlStatus stlGetCurrentUserID(stlUserID     * aUserID,
                              stlChar       * aUserBuffer,
                              stlInt32        aUserBufferLength,
                              stlGroupID    * aGroupID,
                              stlChar       * aGroupBuffer,
                              stlInt32        aGroupBufferLength,
                              stlErrorStack * aErrorStack)
{
    return stuGetCurrentUserID(aUserID,
                               aUserBuffer,
                               aUserBufferLength,
                               aGroupID,
                               aGroupBuffer,
                               aGroupBufferLength,
                               aErrorStack);
}

/**
 * @brief 특정 사용자의 UserID으로 사용자 이름을 반환한다.
 * @param[out] aUserName 사용자 이름이 저장되는 버퍼
 * @param[in] aUserNameLen 사용자 이름이 저장되는 버퍼의 길이
 * @param[in] aUserID UserID
 * @param[out] aErrorStack 에러 스택
 * @remark @a aUserName은 항상 NULL로 끝나는 것을 보장한다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     프로세스나 시스템에서 열수 있는 파일 개수의 제한을 넘어선 경우나,
 *     메모리가 부족한 경우에 발행함.
 * [STL_ERRCODE_NO_ENTRY]
 *     주어진 UserID에 해당하는 사용자 정보가 없는 경우에 발생함.
 * [STL_ERRCODE_STRING_TRUNCATION]
 *     사용자 이름이 주어진 버퍼 크기를 초과하는 경우에 발생함.
 * @endcode
 */
stlStatus stlGetUserNameByUserID(stlChar       * aUserName,
                                 stlInt32        aUserNameLen,
                                 stlUserID       aUserID,
                                 stlErrorStack * aErrorStack)
{
    return stuGetUserNameByUserID(aUserName,
                                  aUserNameLen,
                                  aUserID,
                                  aErrorStack);
}

/**
 * @brief 특정 사용자의 이름으로 사용자의 UserID와 GroupID를 반환한다.
 * @param[out] aUserID 반환되는 사용자의 UserID
 * @param[in,out] aUserBuffer aUserID를 위한 버퍼
 * @param[in,out] aUserBufferLength aUserBuffer의 길이
 * @param[out] aGroupID 반환되는 사용자의 GroupID
 * @param[in,out] aGroupBuffer aGroupID를 위한 버퍼
 * @param[in,out] aGroupBufferLength aGroupBuffer의 길이
 * @param[in] aUserName 사용자의 이름
 * @param[out] aErrorStack 에러 스택
 * @remark
 *     @a aUserName은 항상 NULL로 끝나는 것을 보장해야 한다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     프로세스나 시스템에서 열수 있는 파일 개수의 제한을 넘어선 경우나,
 *     메모리가 부족한 경우에 발행함.
 * [STL_ERRCODE_NO_ENTRY]
 *     주어진 사용자 이름에 해당하는 사용자 정보가 없는 경우에 발생함.
 * @endcode
 */
stlStatus stlGetUserIDByUserName(stlUserID     * aUserID,
                                 stlChar       * aUserBuffer,
                                 stlInt32        aUserBufferLength,
                                 stlGroupID    * aGroupID,
                                 stlChar       * aGroupBuffer,
                                 stlInt32        aGroupBufferLength,
                                 const stlChar * aUserName,
                                 stlErrorStack * aErrorStack)
{
    return stuGetUserIDByUserName(aUserID,
                                  aUserBuffer,
                                  aUserBufferLength, 
                                  aGroupID,
                                  aGroupBuffer,
                                  aGroupBufferLength,
                                  aUserName,
                                  aErrorStack);
}

/**
 * @brief 특정 사용자의 이름으로 사용자의 홈디렉토리 경로를 반환한다.
 * @param[out] aDirName 반환되는 사용자의 홈디렉토리 경로
 * @param[in] aDirNameLen 사용자 홈디렉토리 경로가 저장되는 버퍼의 길이
 * @param[in] aUserName 사용자의 이름
 * @param[out] aErrorStack 에러 스택
 * @remark
 *     @a aDirName은 항상 NULL로 끝나는 것을 보장한다.<BR>
 *     @a aUserName은 항상 NULL로 끝나는 것을 보장해야 한다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     프로세스나 시스템에서 열수 있는 파일 개수의 제한을 넘어선 경우나,
 *     메모리가 부족한 경우에 발행함.
 * [STL_ERRCODE_NO_ENTRY]
 *     주어진 사용자 이름에 해당하는 사용자 정보가 없는 경우에 발생함.
 * [STL_ERRCODE_STRING_TRUNCATION]
 *     홈 디렉토리 경로의 길이가 aDirName의 크기를 초과하는 경우에 발생함.
 * @endcode
 */
stlStatus stlGetHomeDirByUserName(stlChar       * aDirName,
                                  stlInt32        aDirNameLen,
                                  const stlChar * aUserName,
                                  stlErrorStack * aErrorStack)
{
    return stuGetHomeDirByUserName(aDirName,
                                   aDirNameLen,
                                   aUserName,
                                   aErrorStack);
}

/**
 * @brief 사용자의 UserID를 비교한다.
 * @param[in] aUserID1 사용자의 UserID
 * @param[in] aUserID2 다른 사용자의 UserID
 * @return 같다면 STL_TRUE, 그렇지 않다면 STL_FALSE
 */
stlBool stlCompareUserID( stlUserID       aUserID1,
                          stlUserID       aUserID2 )
{
    return stuCompareUserID( aUserID1,
                             aUserID2 );
}

/**
 * @brief 특정 GroupID로 그룹 이름을 반환한다.
 * @param[out] aGroupName 반환되는 사용자 그룹 이름
 * @param[in] aGroupNameLen 사용자 그룹 이름 버퍼의 길이
 * @param[in] aGroupID GroupID
 * @param[out] aErrorStack 에러 스택
 * @remark @a aGroupName은 항상 NULL로 끝나는 것을 보장한다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     프로세스나 시스템에서 열수 있는 파일 개수의 제한을 넘어선 경우나,
 *     메모리가 부족한 경우에 발행함.
 * [STL_ERRCODE_NO_ENTRY]
 *     주어진 GroupID에 해당하는 그룹 정보가 없는 경우에 발생함.
 * [STL_ERRCODE_STRING_TRUNCATION]
 *     그룹 이름이 주어진 버퍼 크기를 초과하는 경우에 발생함.
 * @endcode
 */
stlStatus stlGetGroupNameByGroupID(stlChar       * aGroupName,
                                   stlInt32        aGroupNameLen,
                                   stlGroupID      aGroupID,
                                   stlErrorStack * aErrorStack)
{
    return stuGetGroupNameByGroupID(aGroupName,
                                    aGroupNameLen,
                                    aGroupID,
                                    aErrorStack);
}

/**
 * @brief 특정 그룹 이름으로 GroupID를 반환한다.
 * @param[out] aGroupID 반환되는 사용자의 GroupID
 * @param[in,out] aGroupBuffer aGroupID를 위한 버퍼
 * @param[in,out] aGroupBufferLength aGroupBuffer의 길이
 * @param[in] aGroupName 그룹 이름
 * @param[out] aErrorStack 에러 스택
 * @remark @a aGroupName은 항상 NULL로 끝나는 것을 보장해야 한다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     프로세스나 시스템에서 열수 있는 파일 개수의 제한을 넘어선 경우나,
 *     메모리가 부족한 경우에 발행함.
 * [STL_ERRCODE_NO_ENTRY]
 *     주어진 그룹 이름에 해당하는 그룹 정보가 없는 경우에 발생함.
 * @endcode
 */
stlStatus stlGetGroupIDByGroupName(stlGroupID    * aGroupID,
                                   stlChar       * aGroupBuffer,
                                   stlInt32        aGroupBufferLength,
                                   const stlChar * aGroupName,
                                   stlErrorStack * aErrorStack)
{
    return stuGetGroupIDByGroupName(aGroupID,
                                    aGroupBuffer,
                                    aGroupBufferLength,
                                    aGroupName,
                                    aErrorStack);
}

/**
 * @brief groupID를 비교한다.
 * @param[in] aGroupID1 GroupID
 * @param[in] aGroupID2 다른 GroupID
 * @return 같다면 STL_TRUE, 그렇지 않다면 STL_FALSE
 */
stlBool stlCompareGroupID( stlGroupID      aGroupID1,
                           stlGroupID      aGroupID2 )
{
    return stuCompareGroupID( aGroupID1,
                              aGroupID2 );
}

/** @} */
