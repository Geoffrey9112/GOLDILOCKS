/*******************************************************************************
 * sthUnix.c
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

#include <sth.h>
#include <stc.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlLog.h>

#if !defined(SHM_R)
#define SHM_R 0400
#endif
#if !defined(SHM_W)
#define SHM_W 0200
#endif

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
    struct shmid_ds sStat;
    stlSize         sNameLen;
    stlInt32        sState = 0;
    stlInt32        sFlag;

    STL_PARAM_VALIDATE( aShm != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aShmName != NULL, aErrorStack );
    
    sNameLen = stlStrlen( aShmName );
    STL_TRY_THROW( sNameLen <= STL_MAX_SHM_NAME, RAMP_ERR_NAMETOOLONG );

    sFlag = ( SHM_R | SHM_W | IPC_CREAT | IPC_EXCL );

    if( aHugeTlb == STL_TRUE )
    {
#if defined(STC_TARGET_OS_LINUX)
        sFlag |= SHM_HUGETLB;
#elif defined(STC_TARGET_OS_AIX)
        sFlag |= SHM_LGPAGE | SHM_PIN;
#endif
    }

    STL_DASSERT( aReqSize > 0 );

    aShm->mHandle = shmget( aShmKey,
                            aReqSize,
                            sFlag );
    STL_TRY_THROW( aShm->mHandle != -1, RAMP_ERR_SHMGET );
    sState = 1;

    aShm->mAddr = shmat( aShm->mHandle,
                         aShmAddr,
                         SHM_RND );
    STL_TRY_THROW( aShm->mAddr != (void*)-1, RAMP_ERR_SHMGET );
    sState = 2;

    STL_TRY_THROW( shmctl(aShm->mHandle, IPC_STAT, &sStat) != -1,
                   RAMP_ERR_SHMGET );

    aShm->mSize = sStat.shm_segsz;
    
    stlStrncpy( aShm->mName, aShmName, STL_MAX_SHM_NAME + 1 );
    aShm->mKey = aShmKey;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aShmName );
    }

    STL_CATCH( RAMP_ERR_SHMGET )
    {
        switch( errno )
        {
            case ENOMEM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_OUT_OF_MEMORY,
                              NULL,
                              aErrorStack );
                break;
            case ENOSPC:
            case ENFILE:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_INSUFFICIENT_RESOURCE,
                              NULL,
                              aErrorStack );
                break;
            case EEXIST:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_SHM_EXIST,
                              NULL,
                              aErrorStack );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_SHM_CREATE,
                              NULL,
                              aErrorStack );
                break;
                
        }
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 2:
            shmdt( aShm->mAddr );
        case 1:
            shmctl( aShm->mHandle, IPC_RMID, NULL );
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
    struct shmid_ds sStat;
    stlSize         sNameLen;
    stlInt32        sState = 0;

    STL_PARAM_VALIDATE( aShm != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aShmName != NULL, aErrorStack );
    
    sNameLen = stlStrlen( aShmName );
    STL_TRY_THROW( sNameLen <= STL_MAX_SHM_NAME, RAMP_ERR_NAMETOOLONG );

    aShm->mHandle = shmget( aShmKey,
                            0,
                            SHM_R | SHM_W );
    STL_TRY_THROW( aShm->mHandle != -1, RAMP_ERR_SHMGET );

    aShm->mAddr = shmat( aShm->mHandle,
                         aShmAddr,
                         SHM_RND );
    
    STL_TRY_THROW( aShm->mAddr != (void*)-1, RAMP_ERR_SHMAT );
    sState = 1;

    STL_TRY_THROW( shmctl(aShm->mHandle, IPC_STAT, &sStat) != -1, RAMP_ERR_SHMCTL );

    aShm->mSize = sStat.shm_segsz;

    stlStrncpy( aShm->mName, aShmName, STL_MAX_SHM_NAME + 1 );
    aShm->mKey = aShmKey;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      aShmName );
    }
    
    STL_CATCH( RAMP_ERR_SHMGET )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SHM_ATTACH,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SHMAT )
    {
        aShm->mAddr = NULL;

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SHM_ATTACH,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SHMCTL )
    {
        (void)shmdt(aShm->mAddr);

        aShm->mAddr = (void*)-1;

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SHM_ATTACH,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 1:
            shmdt( aShm->mAddr );
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
    STL_TRY_THROW(shmdt(aShm->mAddr) == 0, RAMP_ERR_SHMDT);

    aShm->mAddr = (void*)-1;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SHMDT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SHM_DETACH,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
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

    if( aShm->mAddr != (void*)-1 )
    {
        STL_TRY( sthDetach( aShm, aErrorStack ) == STL_SUCCESS );
    }
    
    aShm->mHandle = shmget( aShm->mKey,
                            0,
                            SHM_R | SHM_W );

    STL_TRY_THROW( aShm->mHandle != -1, RAMP_ERR_SHM );
    
    STL_TRY_THROW( shmctl( aShm->mHandle,
                           IPC_RMID,
                           NULL ) != -1, RAMP_ERR_SHM );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SHM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_SHM_DESTROY,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
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
#if defined(STC_TARGET_OS_HPUX)
    return STL_FALSE;
#else
    return STL_TRUE;
#endif
}
