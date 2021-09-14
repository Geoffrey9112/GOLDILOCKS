/*******************************************************************************
 * stlEnv.c
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
 * @file stlEnv.c
 * @brief Standard Layer Environment Routines
 */

#include <stlEnv.h>
#include <stiEnv.h>

/**
 * @addtogroup stlEnv
 * @{
 */

/**
 * @brief 환경변수의 값을 반환한다.
 * @param[out] aValue 환경변수의 값
 * @param[in] aValueLen 환경변수 버퍼의 길이
 * @param[in] aEnvVar 환경변수의 이름
 * @param[out] aIsFound 환경변수가 존재하는지 여부
 * @param[out] aErrorStack 에러 스택
 * @see getenv()
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_STRING_TRUNCATION]
 *     얻은 환경 변수값의 길이가 aValuelen보다 큰 경우에 발생함.
 * @endcode
 */
stlStatus stlGetEnv(stlChar       * aValue,
                    stlInt32        aValueLen,
                    const stlChar * aEnvVar,
                    stlBool       * aIsFound,
                    stlErrorStack * aErrorStack)
{
    return stiGetEnv(aValue, aValueLen, aEnvVar, aIsFound, aErrorStack);
}

/**
 * @brief 환경변수에 값을 설정한다.
 * @param[in] aEnvVar 환경변수의 이름
 * @param[in] aValue 환경변수의 값
 * @param[out] aErrorStack 에러 스택
 * @see setenv()
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     OS 커널 메모리가 부족한 경우에 발생함.
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     aEnvVar가 NULL인 경우에 발생함.
 *     aEnvVar의 문자열 길이가 0인 경우에 발생함.
 *     aEnvVar이 스펙과 다른 문자를 포함하고 있는 경우('=')에 발생함.
 * @endcode
 */
stlStatus stlSetEnv(const stlChar * aEnvVar,
                    stlChar       * aValue,
                    stlErrorStack * aErrorStack)
{
    return stiSetEnv(aEnvVar, aValue, aErrorStack);
}

/**
 * @brief 환경변수를 삭제한다.
 * @param[in] aEnvVar 환경변수의 이름
 * @param[out] aErrorStack 에러 스택
 * @see unsetenv()
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     OS 커널 메모리가 부족한 경우에 발생함.
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     aEnvVar가 NULL인 경우에 발생함.
 *     aEnvVar의 문자열 길이가 0인 경우에 발생함.
 *     aEnvVar이 스펙과 다른 문자를 포함하고 있는 경우('=')에 발생함.
 * @endcode
 */
stlStatus stlDelEnv(const stlChar * aEnvVar,
                    stlErrorStack * aErrorStack)
{
    return stiDelEnv(aEnvVar, aErrorStack);
}

/** @} */
