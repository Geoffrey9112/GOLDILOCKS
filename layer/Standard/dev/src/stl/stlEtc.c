/*******************************************************************************
 * stlEtc.c
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

/**
 * @file stlEtc.c
 * @brief Standard Layer ETC Routines
 */


/**
 * @addtogroup stlEtc
 * @{
 */


/**
 * @brief stlRand()에서 사용할 Seed 값을 세팅한다.
 * @param[in] aSeed stlRand() 에서 사용할 Seed 값
 */
void stlSRand( stlUInt32  aSeed )
{
    srand( aSeed );
}

/**
 * @brief 임의의 random 값을 반환한다[0 ~ RAND_MAX].
 */
stlUInt32 stlRand( )
{
    return rand( );
}

/**
 * @brief ascending 으로 정렬된 배열의 2진 검색
 * @param[in] aKey             찾고자 하는 값 주소
 * @param[in] aBase            정렬된 데이터 배열의 주소
 * @param[in] aBaseArrayCount  데이터 요소의 개수
 * @param[in] aSize            한 데이터 자료형의 크기 
 * @param[in] aCompareFunc     비교 함수 포인터 
 * @return 검색키와 일치하는 엔트리 주소를 반환, 없으면 NULL반환.
 * @remark 배열은 반드시 ascending순서로 정렬되어 있어야 한다. 
 * 
 */
void * stlBSearch( const void  * aKey,
                   const void  * aBase,
                   stlSize       aBaseArrayCount,
                   stlSize       aSize,
                   stlInt32      (*aCompareFunc)(const void *, const void *) )
{
    return bsearch( aKey,
                    aBase,
                    aBaseArrayCount,
                    aSize,
                    aCompareFunc );
}

/**
 * @brief 출력 무시
 * @param[in] aFormat 출력할 문자열의 format
 * @param[in] ... 출력 인자
 * @return 출력한 문자열의 길이
 */
inline stlInt32 stlIgnorePrintf( const stlChar *aFormat, ...)
{
    return 0;
}

/** @} */
