/*******************************************************************************
 * stlShm.c
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
 * @file stlShm.c
 * @brief Standard Layer Shared Memory Routines
 */

#include <stlShm.h>
#include <sth.h>
#include <stlError.h>

/**
 * @addtogroup stlShm
 * @{
 */

/**
 * @brief 공유메모리 세그먼트를 생성하고, 생성된 공유메모리 정보를 aShm에 설정한다.
 * @param[in] aShm 생성된 공유메모리 세그먼트
 * @param[in] aShmName 생성할 공유메모리 세그먼트 이름
 * @param[in] aShmKey 생성할 공유메모리 키
 * @param[in] aShmAddr 생성할 공유메모리 주소
 * @param[in] aReqSize 생성할 공유메모리 세그먼트 크기
 * @param[in] aHugeTlb Huge TLB 사용 여부
 * @param[out] aErrorStack 에러 스택
 * @remark
 *     @a aShmName은 Null Terminated String이다.
 *     @a aShm에 설정되는 정보는 아래와 같다.
 *        - 공유 메모리 주소.
 *        - 실제 할당받은 공유 메모리 크기.
 *        - 공유 메모리 키.
 *        - 공유 메모리 이름.
 *        - 공유 메모리 핸들.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NAMETOOLONG]
 *     aShmName이 STL_MAX_SHM_NAME보다 큰 경우에 발생함.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     OS 커널 메모리가 부족한 경우에 발생함.
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     프로세스나 시스템에서 열수 있는 파일 개수의 제한을 넘어선 경우에 발생하거나,
 *     프로세스나 시스템에서 할당받을수 있는 공유 메모리 제한을 넘어선 경우에 발생함.
 * [STL_ERRCODE_SHM_EXIST]
 *     입력받은 key로 이미 공유 메모리 세그먼트가 존재함.
 * [STL_ERRCODE_SHM_CREATE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlCreateShm( stlShm        * aShm,
                        stlChar       * aShmName,
                        stlInt32        aShmKey,
                        void          * aShmAddr,
                        stlSize         aReqSize,
                        stlBool         aHugeTlb,
                        stlErrorStack * aErrorStack )
{
    return sthCreate( aShm,
                      aShmName,
                      aShmKey,
                      aShmAddr,
                      aReqSize,
                      aHugeTlb,
                      aErrorStack );
}

/**
 * @brief 생성된 공유메모리 세그먼트를 attach 한다.
 * @param[in] aShm 생성된 공유메모리 세그먼트
 * @param[in] aShmName 생성된 공유메모리 세그먼트 이름
 * @param[in] aShmKey 생성된 공유메모리 키
 * @param[in] aShmAddr 생성된 공유메모리 주소
 * @param[out] aErrorStack 에러 스택
 * @remark @a aShmName은 Null Terminated String이다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NAMETOOLONG]
 *     aShmName이 STL_MAX_SHM_NAME보다 큰 경우에 발생함.
 * [STL_ERRCODE_SHM_ATTACH]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlAttachShm( stlShm        * aShm,
                        stlChar       * aShmName,
                        stlInt32        aShmKey,
                        void          * aShmAddr,
                        stlErrorStack * aErrorStack )
{
    return sthAttach( aShm, aShmName, aShmKey, aShmAddr, aErrorStack );
}

/**
 * @brief 공유메모리 세그먼트를 detach 한다.
 * @param[in] aShm 생성된 공유메모리 세그먼트
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SHM_DETACH]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlDetachShm(stlShm        * aShm,
                       stlErrorStack * aErrorStack)
{
    return sthDetach(aShm, aErrorStack);
}

/**
 * @brief 공유메모리 세그먼트를 제거한다.
 * @param[in] aShm 생성된 공유메모리 세그먼트
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_SHM_DESTROY]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
stlStatus stlDestroyShm(stlShm        * aShm,
                        stlErrorStack * aErrorStack)
{
    return sthDestroy(aShm, aErrorStack);
}

/**
 * @brief 공유메모리 주소를 반환한다.
 * @param[in] aShm 생성된 공유메모리 세그먼트
 * @return 공유메모리 세그먼트 주소
 * @remark 반환되는 주소는 호출한 프로세스에서만 유효한 주소이다.
 */
void * stlGetShmBaseAddr(stlShm * aShm)
{
    return sthGetBaseAddr(aShm);
}

/**
 * @brief 공유메모리 KEY를 반환한다.
 * @param[in] aShm 생성된 공유메모리 세그먼트
 * @return 공유메모리 KEY
 */
stlUInt32 stlGetShmKey(stlShm * aShm)
{
    return (stlUInt32)aShm->mKey;
}

/**
 * @brief 공유메모리 세그먼트를 크기를 반환한다.
 * @param[in] aShm 생성된 공유메모리 세그먼트
 * @return 공유메모리 세그먼트 크기
 */
stlSize stlGetShmSize(stlShm * aShm)
{
    return sthGetSize(aShm);
}

/**
 * @brief 지정된 주소로 공유메모리 세그먼트를 생성할수 있는 기능이 있는지 반환한다.
 * @return 지원 여부
 */
stlBool stlSupportShareAddressShm()
{
    return sthSupportShareAddressShm();
}

/** @} */
