/*******************************************************************************
 * stlBacktrace.c
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
#include <stf.h>
#include <stc.h>
#include <stb.h>

/**
 * @file stlBacktrace.c
 * @brief Standard Layer Backtrace Routines
 */


/**
 * @addtogroup stlBacktrace
 * @{
 */

/**
 * @brief Backtrace 정보를 얻는다.
 * @param[out] aBuffer Stack Frame들을 저장할 버퍼
 * @param[in] aSize aBuffer의 크기
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 * @return 저장된 Stack Frame의 Depth
 * @remark aSigInfo와 aContext는 Signal이 발생한 경우에만 설정한다.
 * <BR> 일반적인 경우에는 aSigInfo와 aContext는 각각 NULL 설정한다.
 */
inline stlInt32 stlBacktrace( void     ** aBuffer,
                              stlInt32    aSize,
                              void      * aSigInfo,
                              void      * aContext )
{
    return stbBacktrace( NULL, aBuffer, aSize, aSigInfo, aContext );
}

/**
 * @brief Backtrace 정보를 File에 기록한다.
 * @param[in] aFile 출력 파일
 * @param[in] aBuffer Stack Frame들을 저장할 버퍼
 * @param[in] aSize aBuffer의 크기
 * @param[in] aSigInfo Signal Information
 * @param[in] aContext 이전 Context 정보
 * @return 저장된 Stack Frame의 Depth
 * @remark aSigInfo와 aContext는 Signal이 발생한 경우에만 설정한다.
 * <BR> 일반적인 경우에는 aSigInfo와 aContext는 각각 NULL 설정한다.
 */
inline void stlBacktraceToFile( stlFile   * aFile,
                                void     ** aBuffer,
                                stlInt32    aSize,
                                void      * aSigInfo,
                                void      * aContext )
{
    stbBacktraceToFile( aFile,
                        aBuffer,
                        aSize,
                        aSigInfo,
                        aContext );
}

/**
 * @brief Backtrace 정보로부터 심볼 정보를 얻는다.
 * @param[in]  aFrame      Stack Frame들이 저장된 버퍼
 * @param[out] aBuffer     심볼 정보를 저장할 공간
 * @param[in]  aFrameSize  aFrame에 저장된 Frame의 Depth
 * @param[in]  aBufferSize aBuffer의 크기
 */
inline void stlBacktraceSymbolsToStr( void     ** aFrame,
                                      void      * aBuffer,
                                      stlInt32    aFrameSize,
                                      stlInt32    aBufferSize )
{
    return stbBacktraceSymbolsToStr( aFrame,
                                     aBuffer,
                                     aFrameSize,
                                     aBufferSize );
}

/**
 * @brief stlBacktraceSymbols와 유사하지만 심볼정보를 파일에 출력한다.
 * @param[in] aFrame     Stack Frame들이 저장된 버퍼
 * @param[in] aFrameSize aFrame에 저장된 Frame의 Depth
 * @param[in] aFile      출력할 파일 포인터
 */
inline void stlBacktraceSymbolsToFile( void     ** aFrame,
                                       stlInt32    aFrameSize,
                                       stlFile   * aFile )
{
    return stbBacktraceSymbolsToFile( aFrame, aFrameSize, aFile );
}

/** @} */
