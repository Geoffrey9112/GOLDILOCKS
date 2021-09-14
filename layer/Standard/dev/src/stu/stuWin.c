/*******************************************************************************
 * stuWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stuUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlTypes.h>
#include <stlDef.h>
#include <stu.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlDynamicAllocator.h>
#include <stlSpinLock.h>

#include <UserEnv.h>

/*
 * @brief 호출 프로세스의 UserID와 GroupID를 반환한다.
 * @param aUserID 반환되는 사용자의 UserID
 * @param aGroupID 반환되는 사용자의 GroupID
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
    HANDLE sToken;
    DWORD  sLength;
    TOKEN_USER * sUser = (TOKEN_USER*)aUserBuffer;
    TOKEN_PRIMARY_GROUP * sGroup = (TOKEN_PRIMARY_GROUP*)aGroupBuffer;

    *aUserID = NULL;

    STL_TRY_THROW(OpenProcessToken(GetCurrentProcess(),
                                   STANDARD_RIGHTS_READ | READ_CONTROL | TOKEN_QUERY, &sToken) == TRUE,
                  RAMP_ERR_OPEN_ACCESS_TOKEN);

    if (GetTokenInformation(sToken,
                            TokenUser,
                            sUser,
                            aUserBufferLength,
                            &sLength) == FALSE)
    {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            STL_THROW(RAMP_ERR_INSUFFICIENT_RESOURCE);
        }

        STL_THROW(RAMP_ERR_NO_ENTRY);
    }

    *aUserID = sUser->User.Sid;

    if (GetTokenInformation(sToken,
                            TokenPrimaryGroup,
                            sGroup,
                            aGroupBufferLength,
                            &sLength) == FALSE)
    {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            STL_THROW(RAMP_ERR_INSUFFICIENT_RESOURCE);
        }

        STL_THROW(RAMP_ERR_NO_ENTRY);
    }

    *aGroupID = sGroup->PrimaryGroup;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_OPEN_ACCESS_TOKEN)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_ACCESS,
                     NULL,
                     aErrorStack);
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_CATCH(RAMP_ERR_NO_ENTRY)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_NO_ENTRY,
                     NULL,
                     aErrorStack);
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_CATCH(RAMP_ERR_INSUFFICIENT_RESOURCE)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_INSUFFICIENT_RESOURCE,
                     NULL,
                     aErrorStack);
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;
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
    SID_NAME_USE sType;
    stlChar      sName[MAX_PATH];
    stlChar      sDomain[MAX_PATH];
    DWORD        sNameLength = STL_SIZEOF(sName);
    DWORD        sDomainLength = STL_SIZEOF(sDomain);

    STL_TRY_THROW(aUserID != NULL, RAMP_ERR_INVALID_ARGUMENT);

    STL_TRY_THROW(LookupAccountSid(NULL,
                                   aUserID,
                                   sName,
                                   &sNameLength,
                                   sDomain,
                                   &sDomainLength,
                                   &sType) == TRUE, 
                  RAMP_ERR_INSUFFICIENT_RESOURCE);

    STL_TRY_THROW((sType == SidTypeUser) ||
                  (sType == SidTypeAlias) ||
                  (sType == SidTypeWellKnownGroup),
                  RAMP_ERR_INVALID_ARGUMENT);

    stlStrncpy(aUserName, sName, aUserNameLen);

    STL_TRY_THROW(aUserNameLen > stlStrlen(sName), RAMP_ERR_TRUNCATION);

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_ARGUMENT)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_INVALID_ARGUMENT,
                     NULL,
                     aErrorStack);
    }

    STL_CATCH(RAMP_ERR_INSUFFICIENT_RESOURCE)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INSUFFICIENT_RESOURCE,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
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
 * @param aGroupID 반환되는 사용자의 GroupID
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
    SID_NAME_USE   sSIDType;
    stlChar        sAnyDomain[256] = { 0, };
    stlChar        sBuffer[256] = { 0, };
    stlChar      * sDomain = NULL;
    DWORD          sSIDLen;
    DWORD          sDomainLength = STL_SIZEOF(sAnyDomain);
    stlChar      * sPos;

    if ((sPos = stlStrchr(aUserName, '/')) != NULL)
    {
        stlStrncpy(sBuffer, aUserName, sPos - aUserName);

        aUserName = sPos + 1;
        sDomain = sBuffer;
    }
    else if ((sPos = stlStrchr(aUserName, '\\')) != NULL)
    {
        stlStrncpy(sBuffer, aUserName, sPos - aUserName);

        aUserName = sPos + 1;
        sDomain = sBuffer;
    }
    else
    {
        sDomain = NULL;
	
    }

    *aUserID = (stlUserID)aUserBuffer;

    STL_TRY_THROW(LookupAccountName(sDomain,
                                    aUserName,
                                    *aUserID,
                                    &sSIDLen,
                                    sAnyDomain,
                                    &sDomainLength,
                                    &sSIDType),
                  RAMP_ERR_NO_ENTRY);

    *aGroupID = NULL;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_NO_ENTRY)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_NO_ENTRY,
                     NULL,
                     aErrorStack);
        steSetSystemError(GetLastError(), aErrorStack);
    }

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
    HANDLE sHandle;
    DWORD  sLength = aDirNameLen;

    STL_TRY_THROW(OpenProcessToken(GetCurrentProcess(),
                                   TOKEN_QUERY,
                                   &sHandle) == TRUE,
                  RAMP_ERR_OPEN_ACCESS_TOKEN);

    if (GetUserProfileDirectory(sHandle, aDirName, &sLength) == FALSE)
    {
        if (sLength > aDirNameLen)
        {
            STL_THROW(RAMP_ERR_TRUNCATION);
        }

        STL_THROW(RAMP_ERR_INSUFFICIENT_RESOURCE);
    }

    CloseHandle(sHandle);

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_OPEN_ACCESS_TOKEN)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_ACCESS,
                     NULL,
                     aErrorStack);
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_CATCH( RAMP_ERR_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_STRING_TRUNCATION,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH(RAMP_ERR_INSUFFICIENT_RESOURCE)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_INSUFFICIENT_RESOURCE,
                     NULL,
                     aErrorStack);
        steSetSystemError(GetLastError(), aErrorStack);
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

    if (EqualSid(aUserID1, aUserID2) == FALSE)
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
    SID_NAME_USE sType;
    stlChar      sName[MAX_PATH];
    stlChar      sDomain[MAX_PATH];
    DWORD        sNameLength = STL_SIZEOF(sName);
    DWORD        sDomainLength = STL_SIZEOF(sDomain);

    STL_TRY_THROW(aGroupID != NULL, RAMP_ERR_INVALID_ARGUMENT);

    STL_TRY_THROW(LookupAccountSid(NULL,
                                   aGroupID,
                                   sName,
                                   &sNameLength,
                                   sDomain,
                                   &sDomainLength,
                                   &sType) == TRUE,
                  RAMP_ERR_INSUFFICIENT_RESOURCE);

    STL_TRY_THROW((sType == SidTypeGroup) ||
                  (sType == SidTypeAlias) ||
                  (sType == SidTypeWellKnownGroup),
                  RAMP_ERR_INVALID_ARGUMENT);

    stlStrncpy(aGroupName, sName, aGroupNameLen);

    STL_TRY_THROW(aGroupNameLen > stlStrlen(sName), RAMP_ERR_TRUNCATION);

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_INVALID_ARGUMENT)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_INVALID_ARGUMENT,
                     NULL,
                     aErrorStack);
    }

    STL_CATCH(RAMP_ERR_INSUFFICIENT_RESOURCE)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_INSUFFICIENT_RESOURCE,
                     NULL,
                     aErrorStack);
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_CATCH(RAMP_ERR_TRUNCATION)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_STRING_TRUNCATION,
                     NULL,
                     aErrorStack);
    }

    STL_FINISH;

    return STL_FAILURE;    
}

/*
 * @brief 특정 그룹 이름으로 GroupID를 반환한다.
 * @param aGroupID 반환되는 사용자의 GroupID
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
    SID_NAME_USE   sSIDType;
    stlChar        sAnyDomain[256] = { 0, };
    stlChar        sBuffer[256] = { 0, };
    stlChar      * sDomain = NULL;
    DWORD          sSIDLen;
    DWORD          sDomainLength = STL_SIZEOF(sAnyDomain);
    stlChar      * sPos;

    if ((sPos = stlStrchr(aGroupName, '/')) != NULL)
    {
        stlStrncpy(sBuffer, aGroupName, sPos - aGroupName);

        aGroupName = sPos + 1;
        sDomain = sBuffer;
    }
    else if ((sPos = stlStrchr(aGroupName, '\\')) != NULL)
    {
        stlStrncpy(sBuffer, aGroupName, sPos - aGroupName);

        aGroupName = sPos + 1;
        sDomain = sBuffer;
    }
    else
    {
        sDomain = NULL;

    }

    *aGroupID = (stlGroupID)aGroupBuffer;

    STL_TRY_THROW(LookupAccountName(sDomain,
                                    aGroupName,
                                    *aGroupID,
                                    &sSIDLen,
                                    sAnyDomain,
                                    &sDomainLength,
                                    &sSIDType),
                  RAMP_ERR_NO_ENTRY);

    *aGroupID = NULL;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_NO_ENTRY)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_NO_ENTRY,
                     NULL,
                     aErrorStack);
        steSetSystemError(GetLastError(), aErrorStack);
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

    if( EqualSid(aGroupID1, aGroupID2) == FALSE )
    {
        sIsEqual = STL_FALSE;
    }

    return sIsEqual;
}

