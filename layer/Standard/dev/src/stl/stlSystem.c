/*******************************************************************************
 * stlSystem.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stlSystem.c 447 2011-03-31 03:22:29Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file stlSystem.c
 * @brief Standard Layer System(OS/HARDWARE) Routines
 */

#include <stlDef.h>
#include <sts.h>

/**
 * @addtogroup stlSystem
 * @{
 */

stlStatus stlGetCpuInfo( stlCpuInfo    * aCpuInfo,
                         stlErrorStack * aErrorStack )
{
    return stsGetCpuInfo( aCpuInfo, aErrorStack );
}

/**
 * @brief 자원 제한값을 가져온다.
 * @param[in]  aLimitType  resource 종류
 * @param[out] aLimit      가져올 현재 제한 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus stlGetResourceLimit( stlResourceLimitType    aLimitType,
                               stlResourceLimit      * aLimit,
                               stlErrorStack         * aErrorStack )
{
    return stsGetResourceLimit( aLimitType,
                                aLimit,
                                aErrorStack );
}

/**
 * @brief 자원 제한값을 설정한다.
 * @param[in]  aLimitType  resource 종류
 * @param[in]  aLimit      설정할 현재 제한 값
 * @param[out] aErrorStack 에러 스택
 */
stlStatus stlSetResourceLimit( stlResourceLimitType    aLimitType,
                               stlResourceLimit      * aLimit,
                               stlErrorStack         * aErrorStack )
{
    return stsSetResourceLimit( aLimitType,
                                aLimit,
                                aErrorStack );
}


/** @} */
