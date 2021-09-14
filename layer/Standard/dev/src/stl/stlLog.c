/*******************************************************************************
 * stlLog.c
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
 * @file stlLog.c
 * @brief Standard Layer Log Routines
 */

#include <stc.h>
#include <stlDef.h>
#include <stl.h>
#include <stlError.h>
#include <ste.h>

/**
 * @addtogroup stlLog
 * @{
 */

/**
 * @brief 콜스택을 Strandard Error로 출력한다.
 */
void stlLogCallstack( void * aSigInfo, void * aContext )
{
    void        * sFrameArray[STL_MAX_CALLSTACK_FRAME_SIZE];
    stlFile       sErrFile;
    stlErrorStack sDummyErrorStack;

    STL_INIT_ERROR_STACK( &sDummyErrorStack );

    stlOpenStderrFile( &sErrFile, NULL );

    stlPutStringFile( "=================================================\n",
                      &sErrFile,
                      &sDummyErrorStack );
    stlPutStringFile( "CALL STACK\n",
                      &sErrFile,
                      &sDummyErrorStack );
    stlPutStringFile( "=================================================\n",
                      &sErrFile,
                      &sDummyErrorStack );
    stlBacktraceToFile( &sErrFile,
                        sFrameArray,
                        STL_MAX_CALLSTACK_FRAME_SIZE,
                        aSigInfo,
                        aContext );
}

/**
 * @brief 주어진 포맷과 인자를 Strandard Error로 출력한다.
 * @param[in] aFormat 출력 포맷 스트링
 * @param[in] ... 출력 인자 리스트
 * @see @a aFormat : stlStringFormat
 */
void stlLogMessage( const stlChar * aFormat, ... )
{
    stlChar       sBuffer[STL_MAX_ERROR_MESSAGE];
    va_list       sVarArgList;
    stlErrorStack sDummyErrorStack;
    stlFile       sErrFile;

    STL_INIT_ERROR_STACK( &sDummyErrorStack );
    
    va_start( sVarArgList, aFormat );
    stlVsnprintf( sBuffer,
                  STL_MAX_ERROR_MESSAGE,
                  aFormat,
                  sVarArgList );
    va_end( sVarArgList );

    stlOpenStderrFile( &sErrFile, NULL );

    /*
     * 실패하면 무시한다.
     */
    stlPutStringFile( sBuffer,
                      &sErrFile,
                      &sDummyErrorStack );
}

/** @} */
