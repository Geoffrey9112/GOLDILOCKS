/*******************************************************************************
 * stuUnix.c
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

#include <stc.h>
#include <stu.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>

#define PWBUF_SIZE 2048
#define GRBUF_SIZE 8192

stlStatus stuGetPwNameSafe(const stlChar * aUserName,
                           struct passwd * aPw,
                           stlChar         aPwBuf[PWBUF_SIZE],
                           stlErrorStack * aErrorStack)
{
    struct passwd *sPwPtr;

    STL_TRY_THROW(getpwnam_r(aUserName, aPw, aPwBuf, PWBUF_SIZE, &sPwPtr) == 0,
                  RAMP_ERR_GETPWNAM_R);

    STL_TRY_THROW(sPwPtr != NULL, RAMP_ERR_NULL_PASSWD);

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_GETPWNAM_R )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INSUFFICIENT_RESOURCE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_CATCH( RAMP_ERR_NULL_PASSWD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NO_ENTRY,
                      NULL,
                      aErrorStack,
                      aUserName );
    }

    STL_FINISH;

    return STL_FAILURE;
    
}

/*
 * @brief 호출 프로세스의 UserID와 GroupID를 반환한다.
 * @param aUserID 반환되는 사용자의 UserID
 * @param aUserBuffer aUserID를 위한 버퍼
 * @param aUserBufferLength aUserBuffer의 길이
 * @param aGroupID 반환되는 사용자의 GroupID
 * @param aGroupBuffer aGroupID를 위한 버퍼
 * @param aGroupBufferLength aGroupBuffer의 길이
 * @param aErrorStack 에러 스택
 * @see getuid(), getgid()
 */
stlStatus stuGetCurrentUserID(stlUserID     * aUserID,
                              stlChar       * aUserBuffer,
                              stlInt32        aUserBufferLength,
                              stlGroupID    * aGroupID,
                              stlChar       * aGroupBuffer,
                              stlInt32        aGroupBufferLength,
                              stlErrorStack * aErrorStack)
{
    *aUserID = getuid();
    *aGroupID = getgid();

    return STL_SUCCESS;
}

/*
 * @brief 특정 사용자의 UserID으로 사용자 이름을 반환한다.
 * @param aUserName 사용자 이름이 저장되는 버퍼
 * @param aUserNameLen 사용자 이름이 저장되는 버퍼의 길이
 * @param aUserID UserID
 * @param aErrorStack 에러 스택
 * @see getpwuid_r()
 */
stlStatus stuGetUserNameByUserID(stlChar       * aUserName,
                                 stlInt32        aUserNameLen,
                                 stlUserID       aUserID,
                                 stlErrorStack * aErrorStack)
{
    struct passwd *sPwdPtr;
    struct passwd  sPwd;
    stlChar        sPwBuf[PWBUF_SIZE];

    STL_TRY_THROW(getpwuid_r(aUserID, &sPwd, sPwBuf, PWBUF_SIZE, &sPwdPtr) == 0,
                  RAMP_ERR_GETPWUID_R);

    STL_TRY_THROW(sPwdPtr != NULL, RAMP_ERR_NULL_PASSWD);

    stlStrncpy(aUserName, sPwdPtr->pw_name, aUserNameLen);
    STL_TRY_THROW(aUserNameLen > stlStrlen(sPwdPtr->pw_name), RAMP_ERR_TRUNCATION);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETPWUID_R )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INSUFFICIENT_RESOURCE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_CATCH( RAMP_ERR_NULL_PASSWD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NO_ENTRY,
                      NULL,
                      aErrorStack,
                      aUserName );
    }

    STL_CATCH( RAMP_ERR_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_STRING_TRUNCATION,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;    
}

/*
 * @brief 특정 사용자의 이름으로 사용자의 UserID 반환한다.
 * @param aUserID 반환되는 사용자의 UserID
 * @param aUserBuffer aUserID를 위한 버퍼
 * @param aUserBufferLength aUserBuffer의 길이
 * @param aGroupID 반환되는 사용자의 GroupID
 * @param aGroupBuffer aGroupID를 위한 버퍼
 * @param aGroupBufferLength aGroupBuffer의 길이
 * @param aUserName 사용자의 이름
 * @param aErrorStack 에러 스택
 * @see getpwnam_r()
 */
stlStatus stuGetUserIDByUserName(stlUserID     * aUserID,
                                 stlChar       * aUserBuffer,
                                 stlInt32        aUserBufferLength,
                                 stlGroupID    * aGroupID,
                                 stlChar       * aGroupBuffer,
                                 stlInt32        aGroupBufferLength,
                                 const stlChar * aUserName,
                                 stlErrorStack * aErrorStack)
{
    struct passwd sPw;
    stlChar       sPwBuf[PWBUF_SIZE];

    STL_TRY( stuGetPwNameSafe(aUserName, &sPw, sPwBuf, aErrorStack) == STL_SUCCESS );

    *aUserID = sPw.pw_uid;
    *aGroupID = sPw.pw_gid;
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;   
}

/*
 * @brief 특정 사용자의 이름으로 사용자의 홈디렉토리 경로를 반환한다.
 * @param aDirName 반환되는 사용자의 홈디렉토리 경로
 * @param aDirNameLen 사용자 홈디렉토리 경로가 저장되는 버퍼의 길이
 * @param aUserName 사용자의 이름
 * @param aErrorStack 에러 스택
 * @see getpwnam_r()
 */
stlStatus stuGetHomeDirByUserName(stlChar       * aDirName,
                                  stlInt32        aDirNameLen,
                                  const stlChar * aUserName,
                                  stlErrorStack * aErrorStack)
{
    struct passwd sPw;
    stlChar       sPwBuf[PWBUF_SIZE];

    STL_TRY( stuGetPwNameSafe(aUserName, &sPw, sPwBuf, aErrorStack) == STL_SUCCESS );

    stlStrncpy(aDirName, sPw.pw_dir, aDirNameLen);
    STL_TRY_THROW(aDirNameLen > stlStrlen(sPw.pw_dir), RAMP_ERR_TRUNCATION);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_STRING_TRUNCATION,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * @brief 사용자의 UserID를 비교한다.
 * @param aUserID1 사용자의 UserID
 * @param aUserID2 다른 사용자의 UserID
 * @param aErrorStack 에러 스택
 */
stlBool stuCompareUserID( stlUserID aUserID1,
                          stlUserID aUserID2 )
{
    stlBool sIsEqual = STL_TRUE;

    if( aUserID1 != aUserID2 )
    {
        sIsEqual = STL_FALSE;
    }

    return sIsEqual;
}

/*
 * @brief 특정 GroupID로 그룹 이름을 반환한다.
 * @param aGroupName 반환되는 사용자 그룹 이름
 * @param aGroupNameLen 사용자 그룹 이름 버퍼의 길이
 * @param aGroupID GroupID
 * @param aErrorStack 에러 스택
 * @see getgrgid_r()
 */
stlStatus stuGetGroupNameByGroupID(stlChar       * aGroupName,
                                   stlInt32        aGroupNameLen,
                                   stlGroupID      aGroupID,
                                   stlErrorStack * aErrorStack)
{
    stlInt32      sRet;
    struct group *sGroupPtr;
    struct group  sGroup;
    stlChar       sGroupBuf[GRBUF_SIZE];

    /* See comment in getpwnam_safe on error handling. */
    do
    {
        sRet = getgrgid_r( aGroupID,
                           &sGroup,
                           sGroupBuf,
                           GRBUF_SIZE,
                           &sGroupPtr);
    } while( (sRet == -1) && (errno == EINTR) );
    
    STL_TRY_THROW( sRet == 0, RAMP_ERR_GETGRGID_R);

    STL_TRY_THROW(sGroupPtr != NULL, RAMP_ERR_NULL_GROUP);

    stlStrncpy(aGroupName, sGroupPtr->gr_name, aGroupNameLen);
    STL_TRY_THROW(aGroupNameLen > stlStrlen(sGroupPtr->gr_name), RAMP_ERR_TRUNCATION);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETGRGID_R )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INSUFFICIENT_RESOURCE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_CATCH( RAMP_ERR_NULL_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NO_ENTRY,
                      NULL,
                      aErrorStack,
                      aGroupName );
    }

    STL_CATCH( RAMP_ERR_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_STRING_TRUNCATION,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;    
}

/*
 * @brief 특정 그룹 이름으로 GroupID를 반환한다.
 * @param aGroupID 반환되는 사용자의 GroupID
 * @param aGroupBuffer aGroupID를 위한 버퍼
 * @param aGroupBufferLength aGroupBuffer의 길이
 * @param aGroupName 그룹 이름
 * @param aErrorStack 에러 스택
 * @see getgrnam_r()
 */
stlStatus stuGetGroupIDByGroupName(stlGroupID    * aGroupID,
                                   stlChar       * aGroupBuffer,
                                   stlInt32        aGroupBufferLength,
                                   const stlChar * aGroupName,
                                   stlErrorStack * aErrorStack)
{
    stlInt32      sRet;
    struct group *sGroupPtr;
    struct group  sGroup;
    stlChar       sGroupBuf[GRBUF_SIZE];

    /* See comment in getpwnam_safe on error handling. */
    do
    {
        sRet = getgrnam_r( aGroupName,
                           &sGroup,
                           sGroupBuf,
                           GRBUF_SIZE,
                           &sGroupPtr);
    } while( (sRet == -1) && (errno == EINTR) );

    STL_TRY_THROW( sRet == 0, RAMP_ERR_GETGRNAM_R);

    STL_TRY_THROW(sGroupPtr != NULL, RAMP_ERR_NULL_GROUP);
    
    *aGroupID = sGroupPtr->gr_gid;    

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GETGRNAM_R )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INSUFFICIENT_RESOURCE,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_CATCH( RAMP_ERR_NULL_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NO_ENTRY,
                      NULL,
                      aErrorStack,
                      aGroupName );
    }

    STL_FINISH;

    return STL_FAILURE;
    
}

/*
 * @brief groupID를 비교한다.
 * @param aGroupID1 GroupID
 * @param aGroupID2 다른 GroupID
 * @param aErrorStack 에러 스택
 */
stlBool stuCompareGroupID( stlGroupID aGroupID1,
                           stlGroupID aGroupID2 )
{
    stlBool sIsEqual = STL_TRUE;

    if( aGroupID1 != aGroupID2 )
    {
        sIsEqual = STL_FALSE;
    }

    return sIsEqual;
}

