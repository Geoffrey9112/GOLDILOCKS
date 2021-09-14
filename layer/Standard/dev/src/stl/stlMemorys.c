/*******************************************************************************
 * stlMemorys.c
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
 * @file stlMemorys.c
 * @brief Standard Layer Memory Routines
 */

#include <stc.h>
#include <stlDef.h>
#include <stlError.h>
#include <ste.h>
#include <stlMemorys.h>
#include <stlLog.h>

/**
 * @addtogroup stlMemorys
 * @{
 */

const stlCopyFunction gCopyFunc[9] =
{
    stlCopy0Bytes,
    stlCopy1Bytes,
    stlCopy2Bytes,
    stlCopy3Bytes,
    stlCopy4Bytes,
    stlCopy5Bytes,
    stlCopy6Bytes,
    stlCopy7Bytes,
    stlCopy8Bytes
};

/**
 * @brief Heap 메모리를 할당한다.
 * @param[out] aAddr 할당할 Heap 메모리 포인터의 주소
 * @param[in] aSize 할당할 Heap 메모리 크기
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     커널 메모리를 할당받을수 없는 경우에 발생함.
 * @endcode
 */
stlStatus stlAllocHeap( void         ** aAddr,
                        stlSize         aSize,
                        stlErrorStack * aErrorStack )
{
    *aAddr = NULL;

    STL_TRY_THROW( aSize != 0, RAMP_EXIT_FUNCTION );

    *aAddr = malloc( aSize );

    STL_TRY_THROW( *aAddr != NULL, RAMP_ERR_OUT_OF_MEMORY );

    STL_RAMP(RAMP_EXIT_FUNCTION);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OUT_OF_MEMORY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_OUT_OF_MEMORY,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 할당된 Heap 메모리를 해제한다.
 * @param[in] aAddr 해제할 Heap 메모리 포인터
 */
void stlFreeHeap( void * aAddr )
{
    STL_ASSERT( aAddr != NULL );
    
    free( aAddr );
}

/**
 * @brief 메모리를 특정 값으로 설정한다.
 * @param[in] aAddr 설정할 메모리 주소
 * @param[in] aConstant 메모리에 설정할 값
 * @param[in] aSize 설정할 크기
 * @return 메모리 영역 @a aAddr 에 대한 포인터를 반환한다.
 */
void * stlMemset( void   * aAddr,
                  stlInt32 aConstant,
                  stlSize  aSize )
{
    STL_ASSERT( aSize >= 0 );

    return memset( aAddr, aConstant, aSize );
}

void stlCopy0Bytes( void * aDest, const void * aSrc )
{
}

void stlCopy1Bytes( void * aDest, const void * aSrc )
{
    STL_WRITE_INT8( aDest, aSrc );
}

void stlCopy2Bytes( void * aDest, const void * aSrc )
{
    STL_WRITE_INT16( aDest, aSrc );
}

void stlCopy3Bytes( void * aDest, const void * aSrc )
{
    STL_WRITE_INT16( aDest, aSrc );
    ((stlChar*)aDest)[2] = ((stlChar*)aSrc)[2];
}

void stlCopy4Bytes( void * aDest, const void * aSrc )
{
    STL_WRITE_INT32( aDest, aSrc );
}

void stlCopy5Bytes( void * aDest, const void * aSrc )
{
    STL_WRITE_INT32( aDest, aSrc );
    ((stlChar*)aDest)[4] = ((stlChar*)aSrc)[4];
}

void stlCopy6Bytes( void * aDest, const void * aSrc )
{
    STL_WRITE_INT32( aDest, aSrc );
    ((stlChar*)aDest)[4] = ((stlChar*)aSrc)[4];
    ((stlChar*)aDest)[5] = ((stlChar*)aSrc)[5];
}

void stlCopy7Bytes( void * aDest, const void * aSrc )
{
    STL_WRITE_INT32( aDest, aSrc );
    ((stlChar*)aDest)[4] = ((stlChar*)aSrc)[4];
    ((stlChar*)aDest)[5] = ((stlChar*)aSrc)[5];
    ((stlChar*)aDest)[6] = ((stlChar*)aSrc)[6];
}

void stlCopy8Bytes( void * aDest, const void * aSrc )
{
    STL_WRITE_INT64( aDest, aSrc );
}

#if 0
/**
 * @brief 메모리 영역 @a aSrc 를 메모리 영역 @a aDest 로 @a aSize 바이트 복사한다.
 * @param[in] aDest 복사될 메모리
 * @param[in] aSrc 복사할 메모리
 * @param[in] aSize 복사할 크기
 * @return 메모리 영역 @a aDest 에 대한 포인터를 반환한다.
 * @remark @a aSrc 와 @a aDest 의 메모리 영역은 겹치면 안된다.
 *         만일 메모리 영역을 겹쳐서 사용하길 원한다면 stlMemmove()를 사용한다.
 */

void * stlMemcpy( void *       aDest,
                  const void * aSrc,
                  stlSize      aSize )
{
    STL_ASSERT( aSize >= 0 );

    return memcpy( aDest, aSrc, aSize );
}
#endif

/**
 * @brief 메모리 영역 @a aSrc 를 메모리 영역 @a aDest 로 @a aSize 바이트 복사한다.
 * @param[in] aDest 복사될 메모리
 * @param[in] aSrc 복사할 메모리
 * @param[in] aSize 복사할 크기
 * @return 메모리 영역 @a aDest 에 대한 포인터를 반환한다.
 * @remark @a aSrc의 @a aSize 만큼 임시 영역에 복사한 후 @a aDest에 복사하므로
 *         @a aSrc 와 @a aDest 의 메모리 영역은 겹칠 수 있다.
 */
void * stlMemmove( void *       aDest,
                   const void * aSrc,
                   stlSize      aSize )
{
    STL_ASSERT( aSize >= 0 );

    return memmove( aDest, aSrc, aSize );
}

/**
 * @brief 메모리 영역 @a aSrc 에서부터 @a aSize byte 중에  @a aChar 값의 byte 위치가 있으면 해당 위치를 반환한다
 * @param[in] aSrc 체크할 메모리 시작 포인터
 * @param[in] aChar 체크할 byte 값
 * @param[in] aSize 체크할 byte stream의 길이
 * @return 최초로 나타나는 aChar값의 위치에 대한 포인터를 반환한다.
 */
void * stlMemchr( void *       aSrc,
                  stlInt32     aChar,
                  stlSize      aSize )
{
    STL_ASSERT( aSize >= 0 );

    return memchr( aSrc, aChar, aSize );
}

/**
 * @brief @a aValue1이 @a aValue2를 @a aSize 길이 만큼 메모리 비교를 수행한다.
 * @param[in] aValue1 비교할 대상1의 메모리 시작 포인터
 * @param[in] aValue2 비교할 대상2의 메모리 시작 포인터
 * @param[in] aSize 두 값을 비교할 byte stream의 길이
 * @return @a aValue1이 @a aValue2보다 크면 양의 정수를 반환하고, 작으면 음의 정수를 반환한다.
 *         @a aValue1과 @a aValue2가 같으면 0을 반환한다.
 */
stlInt32 stlMemcmp( void *       aValue1,
                    void *       aValue2,
                    stlSize      aSize )
{
    STL_ASSERT( aSize >= 0 );

    return memcmp( aValue1, aValue2, aSize );
}

/** @} */
