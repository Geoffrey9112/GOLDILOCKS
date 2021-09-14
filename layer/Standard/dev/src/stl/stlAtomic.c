/*******************************************************************************
 * stlAtomic.c
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
 * @file stlAtomic.c
 * @brief Standard Layer Atomic Routines
 */

#include <stlDef.h>
#include <sta.h>

/**
 * @addtogroup stlAtomic
 * @{
 */

/**
 * @brief aMem으로 부터 32비트 정수를 원자적으로 읽는다.
 * @param aMem 읽어야할 포인터
 */
stlUInt32 stlAtomicRead32( volatile stlUInt32 *aMem )
{
    return staRead32( aMem );
}

/**
 * @brief aMem에 aVal를 원자적으로 설정한다.
 * @param aMem 설정할 포인터
 * @param aVal 설정시킬 32비트 정수값
 */
void stlAtomicSet32( volatile stlUInt32 * aMem,
                     stlUInt32            aVal )
{
    return staSet32( aMem, aVal );
}

/**
 * @brief aMem의 기존 값에 aVal 만큼 32비트 정수를 원자적으로 증가시킨다.
 * @param aMem 설정할 포인터
 * @param aVal 증가시킬 32비트 정수값
 * @return *aMem의 기존값(old value)
 */
stlUInt32 stlAtomicAdd32( volatile stlUInt32 * aMem,
                          stlUInt32            aVal )
{
    return staAdd32( aMem, aVal );
}

/**
 * @brief aMem의 기존 값에 aVal 만큼 32비트 정수를 원자적으로 감소시킨다.
 * @param aMem 설정할 포인터
 * @param aVal 감소시킬 32비트 정수값
 * @return *aMem의 기존값(old value)
 */
void stlAtomicSub32( volatile stlUInt32 * aMem,
                     stlUInt32            aVal )
{
    staSub32( aMem, aVal );
}

/**
 * @brief aMem의 기존 값을 1만큼 원자적으로 증가시킨다.
 * @param aMem 설정할 포인터
 * @return *aMem의 기존값(old value)
 */
stlUInt32 stlAtomicInc32( volatile stlUInt32 * aMem )
{
    return staAdd32( aMem, 1 );
}

/**
 * @brief aMem의 기존 값을 1만큼 원자적으로 감소시킨다.
 * @param aMem 설정할 포인터
 * @return 감소시킨 값이 0이라면 0을 리턴하고, 그렇지 않다면 0이 아닌 값을 리턴한다.
 */
stlInt32 stlAtomicDec32( volatile stlUInt32 * aMem )
{
    return staDec32( aMem );
}

/**
 * @brief aMem의 기존값을 aVal로 원자적으로 교체한다.
 * @param aMem 설정할 포인터
 * @param aVal 교체하고자 하는 32비트 정수값
 * @return *aMem의 기존값(old value)
 */
stlUInt32 stlAtomicXchg32( volatile stlUInt32 * aMem,
                           stlUInt32            aVal )
{
    return staXchg32( aMem, aVal );
}

#if( (STL_SIZEOF_VOIDP == 8) || defined(DOXYGEN) )

/**
 * @brief aMem으로 부터 64비트 정수를 원자적으로 읽는다.
 * @param aMem 읽어야할 포인터
 */
stlUInt64 stlAtomicRead64( volatile stlUInt64 * aMem )
{
    return staRead64( aMem );
}

/**
 * @brief aMem에 aVal를 원자적으로 설정한다.
 * @param aMem 설정할 포인터
 * @param aVal 설정시킬 64비트 정수값
 */
void stlAtomicSet64( volatile stlUInt64 * aMem,
                     stlUInt64            aVal )
{
    staSet64( aMem, aVal );
}

/**
 * @brief aMem의 기존 값에 aVal 만큼 64비트 정수를 원자적으로 증가시킨다.
 * @param aMem 설정할 포인터
 * @param aVal 증가시킬 64비트 정수값
 * @return *aMem의 기존값(old value)
 */
stlUInt64 stlAtomicAdd64( volatile stlUInt64 * aMem,
                          stlUInt64            aVal )
{
    return staAdd64( aMem, aVal );
}

/**
 * @brief aMem의 기존 값에 aVal 만큼 64비트 정수를 원자적으로 감소시킨다.
 * @param aMem 설정할 포인터
 * @param aVal 감소시킬 64비트 정수값
 * @return *aMem의 기존값(old value)
 */
void stlAtomicSub64( volatile stlUInt64 * aMem,
                     stlUInt64            aVal )
{
    staSub64( aMem, aVal );
}

/**
 * @brief aMem의 기존 값을 1만큼 원자적으로 증가시킨다.
 * @param aMem 설정할 포인터
 * @return *aMem의 기존값(old value)
 */
stlUInt64 stlAtomicInc64( volatile stlUInt64 * aMem )
{
    return staAdd64( aMem, 1 );
}

/**
 * @brief aMem의 기존 값을 1만큼 원자적으로 감소시킨다.
 * @param aMem 설정할 포인터
 * @return 감소시킨 값이 0이라면 0을 리턴하고, 그렇지 않다면 0이 아닌 값을 리턴한다.
 */
stlInt64 stlAtomicDec64( volatile stlUInt64 * aMem )
{
    return staDec64( aMem );
}

/**
 * @brief aMem의 기존값이 aCmp와 같다면 aWith로 교체한다.
 * @param aMem 설정할 포인터
 * @param aWith 교체하고자 하는 64비트 정수값
 * @param aCmp 비교값
 * @return *aMem의 기존값(old value)
 */
stlUInt64 stlAtomicCas64( volatile stlUInt64 * aMem,
                          stlUInt64            aWith,
                          stlUInt64            aCmp )
{
     return staCas64( aMem, aWith, aCmp );
}

/**
 * @brief aMem의 기존값을 aVal로 원자적으로 교체한다.
 * @param aMem 설정할 포인터
 * @param aVal 교체하고자 하는 64비트 정수값
 * @return *aMem의 기존값(old value)
 */
stlUInt64 stlAtomicXchg64( volatile stlUInt64 * aMem,
                           stlUInt64            aVal )
{
    return staXchg64( aMem, aVal );
}

#endif

/** @} */
