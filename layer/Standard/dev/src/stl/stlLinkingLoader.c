/*******************************************************************************
 * stlLinkingLoader.c
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
 * @file stlLinkingLoader.c
 * @brief Standard Dynamic Linking Loader Routines
 */

#include <stlDef.h>
#include <sto.h>

/**
 * @addtogroup stlLinkingLoader
 * @{
 */

/**
 * @brief 라이브러리를 Open(=load)한다.
 * @param[in]  aLibraryPath     Open할 라이브러리의 절대 경로(+이름)
 * @param[out] aLibraryHandle  Open된 라이브러리의 핸들
 * @param[in]  aErrorStack     에러스택 포인터
 */
stlStatus stlOpenLibrary( stlChar        * aLibraryPath,
                          stlDsoHandle   * aLibraryHandle,
                          stlErrorStack  * aErrorStack )
{
    return stoOpen( aLibraryPath,
                    aLibraryHandle,
                    aErrorStack );
}

/**
 * @brief 라이브러리를 Close한다.
 * @param[in] aLibraryHandle  Close할 라이브러리의 핸들
 * @param[in] aErrorStack     에러스택 포인터
 */
stlStatus stlCloseLibrary( stlDsoHandle    aLibraryHandle,
                           stlErrorStack * aErrorStack )
{
    return stoClose( aLibraryHandle, aErrorStack );
}

/**
 * @brief 라이브러리로 부터 심볼의 주소를 얻는다.
 * @param[in]  aLibraryHandle  대상 라이브러리의 핸들
 * @param[in]  aSymbolName     찾고자하는 심볼 이름 (null-termination string)
 * @param[out] aSymbolAddr     찾은 심볼의 주소
 * @param[in]  aErrorStack     에러스택 포인터
 */
stlStatus stlGetSymbol( stlDsoHandle      aLibraryHandle,
                        stlChar         * aSymbolName,
                        stlDsoSymHandle * aSymbolAddr,
                        stlErrorStack   * aErrorStack )
{
    return stoGetSymbol( aLibraryHandle,
                         aSymbolName,
                         aSymbolAddr,
                         aErrorStack );
}

/** @} */
