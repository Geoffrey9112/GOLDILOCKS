/*******************************************************************************
 * sthWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sthUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <sth.h>
#include <stc.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlLog.h>

stlSize gSthGranulatiry = 0;

/*
 * @brief 공유메모리 세그먼트를 생성한다.
 * @param aShm 생성된 공유메모리 세그먼트
 * @param aShmName 생성할 공유메모리 세그먼트 이름
 * @param aShmKey 생성할 공유메모리 키 이름
 * @param aShmAddr 생성할 공유메모리 주소
 * @param aReqSize 생성할 공유메모리 세그먼트 크기
 * @param aHugeTlb Huge TLB 사용 여부
 * @param aErrorStack 에러 스택
 * @see shmget()
 */
stlStatus sthCreate(stlShm        * aShm,
                    stlChar       * aShmName,
                    stlInt32        aShmKey,
                    void          * aShmAddr,
                    stlSize         aReqSize,
                    stlBool         aHugeTlb,
                    stlErrorStack * aErrorStack)
{
    SYSTEM_INFO sSi;
    stlSize     sSize;
    DWORD       sSizeLo;
    DWORD       sSizeHi;
    HANDLE      sMap = NULL;
    DWORD       sError;
    void *      sBase = NULL;
    stlInt32    sState = 0;
    stlSize     sNameLen;

    STL_PARAM_VALIDATE(aShm != NULL, aErrorStack);
    STL_PARAM_VALIDATE(aShmName != NULL, aErrorStack);

    sNameLen = stlStrlen(aShmName);
    STL_TRY_THROW(sNameLen <= STL_MAX_SHM_NAME, RAMP_ERR_NAMETOOLONG);

    if (gSthGranulatiry == 0)
    {
        GetSystemInfo(&sSi);
        gSthGranulatiry = sSi.dwAllocationGranularity;
    }

    /* Compute the granualar multiple of the pagesize */
    sSize = gSthGranulatiry * (1 + (aReqSize - 1) / gSthGranulatiry);
    sSizeLo = (DWORD)sSize;

#if (STL_SIZEOF_VOIDP == 8)
    sSizeHi = (DWORD)(sSize >> 32);
#else
    sSizeHi = 0;
#endif

    sMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, sSizeHi, sSizeLo, aShmName);

    STL_TRY_THROW(sMap != NULL, RAMP_ERR_SHM_CREATE);

    sState = 1;

    sError = GetLastError();

    STL_TRY_THROW(sError != ERROR_ALREADY_EXISTS, RAMP_ERR_SHM_EXIST);

    sBase = MapViewOfFile(sMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sSize);
	
    STL_TRY_THROW(sBase != NULL, RAMP_ERR_SHM_CREATE);

    stlStrncpy(aShm->mName, aShmName, STL_MAX_SHM_NAME + 1);

    aShm->mAddr   = sBase;
    aShm->mSize   = sSize;
    aShm->mHandle = sMap;
    aShm->mKey    = 0;

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_NAMETOOLONG)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_NAMETOOLONG,
                     NULL,
                     aErrorStack,
                     aShmName);
    }

    STL_CATCH(RAMP_ERR_SHM_CREATE)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_SHM_CREATE,
                     NULL,
                     aErrorStack);
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_CATCH(RAMP_ERR_SHM_EXIST)
    {
        stlPushError(STL_ERROR_LEVEL_ABORT,
                     STL_ERRCODE_SHM_EXIST,
                     NULL,
                     aErrorStack);
        steSetSystemError(GetLastError(), aErrorStack);
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            CloseHandle(sMap);
        default:
            break;
    }

    return STL_FAILURE;
}

/*
 * @brief 생성된 공유메모리 세그먼트를 attach 한다.
 * @param aShm 생성된 공유메모리 세그먼트
 * @param aShmName 생성된 공유메모리 세그먼트 이름
 * @param aShmKey 생성된 공유메모리 키
 * @param aShmAddr 생성된 공유메모리 주소
 * @param aErrorStack 에러 스택
 * @see shmat()
 */
stlStatus sthAttach( stlShm        * aShm,
                     stlChar       * aShmName,
                     stlInt32        aShmKey,
                     void          * aShmAddr,
                     stlErrorStack * aErrorStack )
{
    HANDLE   sMap  = NULL;
    void   * sBase = NULL;

    stlSize         sNameLen;
    stlInt32        sState = 0;

    STL_PARAM_VALIDATE( aShm != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aShmName != NULL, aErrorStack );
    
    sNameLen = stlStrlen( aShmName );
    STL_TRY_THROW( sNameLen <= STL_MAX_SHM_NAME, RAMP_ERR_NAMETOOLONG );

    sMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, aShmName);

    STL_TRY_THROW(sMap != NULL, RAMP_ERR_SHM_ATTACH);

    sState = 1;

    sBase = MapViewOfFile(sMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

    STL_TRY_THROW(sBase != NULL, RAMP_ERR_SHM_ATTACH);

    aShm->mAddr = sBase;
    aShm->mSize = 0; /**@todo check size */
    aShm->mHandle = sMap;
    aShm->mKey = 0;
	
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aShmName );
    }
    
    STL_CATCH(RAMP_ERR_SHM_ATTACH)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SHM_ATTACH,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            CloseHandle(sMap);
        default:
            break;
    }
    
    return STL_FAILURE;
}

/*
 * @brief 공유메모리 세그먼트를 detach 한다.
 * @param aShm 생성된 공유메모리 세그먼트
 * @param aErrorStack 에러 스택
 * @see shmdt()
 */
stlStatus sthDetach(stlShm        * aShm,
                    stlErrorStack * aErrorStack)
{
    STL_PARAM_VALIDATE(aShm != NULL, aErrorStack);

    STL_TRY_THROW(UnmapViewOfFile(aShm->mAddr) != FALSE, RAMP_ERR_SHM_DETACH);

    STL_TRY_THROW(CloseHandle(aShm->mHandle) != FALSE, RAMP_ERR_SHM_DETACH);

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_SHM_DETACH)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SHM_DETACH,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*
 * @brief 공유메모리 세그먼트를 제거한다.
 * @param aShm 생성된 공유메모리 세그먼트
 * @param aErrorStack 에러 스택
 * @see shmctl()
 */
stlStatus sthDestroy(stlShm        * aShm,
                     stlErrorStack * aErrorStack)
{
    STL_PARAM_VALIDATE( aShm != NULL, aErrorStack );

    STL_TRY_THROW(UnmapViewOfFile(aShm->mAddr) != FALSE, RAMP_ERR_SHM_DESTROY);

    STL_TRY_THROW(CloseHandle(aShm->mHandle) != FALSE, RAMP_ERR_SHM_DESTROY);

    aShm->mName[0] = '\0';

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_SHM_DESTROY)
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SHM_DESTROY,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;    
}

/*
 * @brief 공유메모리 세그먼트를 주소를 반환한다.
 * @param aShm 생성된 공유메모리 세그먼트
 * @return 공유메모리 세그먼트 주소
 * @remark 반환되는 주소는 호출한 프로세스에서만 유효한 주소이다.
 */
void * sthGetBaseAddr(stlShm * aShm)
{
    return aShm->mAddr;
}

/*
 * @brief 공유메모리 세그먼트를 크기를 반환한다.
 * @param aShm 생성된 공유메모리 세그먼트
 * @return 공유메모리 세그먼트 크기
 */
stlSize sthGetSize(stlShm * aShm)
{
    return aShm->mSize;
}

/*
 * @brief 지정된 주소로 공유메모리 세그먼트를 생성할수 있는 기능이 있는지 반환한다.
 * @return 지원 여부
 */
stlBool sthSupportShareAddressShm()
{
    return STL_FALSE;
}
