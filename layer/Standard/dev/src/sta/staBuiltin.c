/*******************************************************************************
 * staBuiltin.c
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

#include <stlDef.h>

/*
 * @brief aMem으로 부터 32비트 정수를 원자적으로 읽는다.
 * @param aMem 읽어야할 포인터
 */
stlUInt32 staRead32( volatile stlUInt32 * aMem )
{
    return *aMem;
}

/*
 * @brief aMem에 aVal를 원자적으로 설정한다.
 * @param aMem 설정할 포인터
 * @param aVal 설정시킬 32비트 정수값
 */
void staSet32( volatile stlUInt32 * aMem,
               stlUInt32            aVal )
{
#ifndef WIN32
    *aMem = aVal;
#else
    InterlockedExchange( (long*)aMem, aVal );
#endif
}

/*
 * @brief aMem의 기존 값에 aVal 만큼 32비트 정수를 원자적으로 증가시킨다.
 * @param aMem 설정할 포인터
 * @param aVal 증가시킬 32비트 정수값
 * @return *aMem의 기존값(old value)
 */
stlUInt32 staAdd32( volatile stlUInt32 * aMem,
                    stlUInt32            aVal )
{
#ifndef WIN32
    return __sync_fetch_and_add( aMem, aVal );
#else
    return InterlockedExchangeAdd((long*)aMem, aVal);
#endif
}

/*
 * @brief aMem의 기존 값에 aVal 만큼 32비트 정수를 원자적으로 감소시킨다.
 * @param aMem 설정할 포인터
 * @param aVal 감소시킬 32비트 정수값
 * @return *aMem의 기존값(old value)
 */
void staSub32( volatile stlUInt32 * aMem,
               stlUInt32            aVal )
{
#ifndef WIN32
    __sync_fetch_and_sub( aMem, aVal );
#else
    InterlockedExchangeAdd((long*)aMem, -(stlInt64)aVal);
#endif
}

/*
 * @brief aMem의 기존 값을 1만큼 원자적으로 감소시킨다.
 * @param aMem 설정할 포인터
 * @return 감소시킨 값이 0이라면 0을 리턴하고, 그렇지 않다면 0이 아닌 값을 리턴한다.
 */
stlInt32 staDec32( volatile stlUInt32 * aMem )
{
#ifndef WIN32
    return __sync_sub_and_fetch( aMem, 1 );
#else
    return InterlockedDecrement((long*)aMem);
#endif
}

/*
 * @brief aMem의 기존값을 aVal로 원자적으로 교체한다.
 * @param aMem 설정할 포인터
 * @param aVal 교체하고자 하는 32비트 정수값
 * @return *aMem의 기존값(old value)
 */
stlUInt32 staXchg32( volatile stlUInt32 * aMem,
                     stlUInt32            aVal )
{
#ifndef WIN32
    return __sync_lock_test_and_set( aMem, aVal );
#else
    return InterlockedExchange((long*)aMem, aVal);
#endif
}

#if( (STL_SIZEOF_VOIDP == 8) || defined(DOXYGEN) )

/*
 * @brief aMem으로 부터 64비트 정수를 원자적으로 읽는다.
 * @param aMem 읽어야할 포인터
 */
stlUInt64 staRead64( volatile stlUInt64 * aMem )
{
    return *aMem;
}

/*
 * @brief aMem에 aVal를 원자적으로 설정한다.
 * @param aMem 설정할 포인터
 * @param aVal 설정시킬 64비트 정수값
 */
void staSet64( volatile stlUInt64 * aMem,
               stlUInt64            aVal )
{
#ifndef WIN32
    *aMem = aVal;
#else
    InterlockedExchange64( (stlInt64*)aMem, aVal );
#endif
}

/*
 * @brief aMem의 기존 값에 aVal 만큼 64비트 정수를 원자적으로 증가시킨다.
 * @param aMem 설정할 포인터
 * @param aVal 증가시킬 64비트 정수값
 * @return *aMem의 기존값(old value)
 */
stlUInt64 staAdd64( volatile stlUInt64 * aMem,
                    stlUInt64            aVal )
{
#ifndef WIN32
    return __sync_fetch_and_add( aMem, aVal );
#else
    return InterlockedExchangeAdd64((stlInt64*)aMem, aVal);
#endif
}

/*
 * @brief aMem의 기존 값에 aVal 만큼 64비트 정수를 원자적으로 감소시킨다.
 * @param aMem 설정할 포인터
 * @param aVal 감소시킬 64비트 정수값
 * @return *aMem의 기존값(old value)
 */
void staSub64( volatile stlUInt64 * aMem,
               stlUInt64            aVal )
{
#ifndef WIN32
    __sync_fetch_and_sub( aMem, aVal );
#else
    InterlockedExchangeAdd64((stlInt64*)aMem, -(stlInt64)aVal);
#endif
}

/*
 * @brief aMem의 기존 값을 1만큼 원자적으로 감소시킨다.
 * @param aMem 설정할 포인터
 * @return 감소시킨 값이 0이라면 0을 리턴하고, 그렇지 않다면 0이 아닌 값을 리턴한다.
 */
stlInt64 staDec64( volatile stlUInt64 * aMem )
{
#ifndef WIN32
    return __sync_sub_and_fetch( aMem, 1 );
#else
    return InterlockedDecrement64((stlInt64*)aMem);
#endif
}

/*
 * @brief aMem의 기존값이 aCmp와 같다면 aWith로 교체한다.
 * @param aMem 설정할 포인터
 * @param aWith 교체하고자 하는 64비트 정수값
 * @param aCmp 비교값
 * @return *aMem의 기존값(old value)
 */
stlUInt64 staCas64( volatile stlUInt64 * aMem,
                    stlUInt64            aWith,
                    stlUInt64            aCmp )
{
#ifndef WIN32
    return __sync_val_compare_and_swap( aMem,
                                        aCmp,
                                        aWith );
#else
    return InterlockedCompareExchange64((stlInt64*)aMem, aWith, aCmp);
#endif
}

/*
 * @brief aMem의 기존값을 aVal로 원자적으로 교체한다.
 * @param aMem 설정할 포인터
 * @param aVal 교체하고자 하는 64비트 정수값
 * @return *aMem의 기존값(old value)
 */
stlUInt64 staXchg64( volatile stlUInt64 * aMem,
                     stlUInt64           aVal )
{
#ifndef WIN32
    __sync_synchronize();
    return __sync_lock_test_and_set( aMem, aVal );
#else
    return InterlockedExchange64((stlInt64*)aMem, aVal);
#endif
}

#endif
