/*******************************************************************************
 * stlLogger.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stlLogger.c 13675 2014-10-06 14:17:27Z lym999 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file stlLogger.c
 * @brief Standard Layer Logger Routines
 */

#include <stlDef.h>
#include <stl.h>
#include <stg.h>

/**
 * @addtogroup stlLogger
 * @{
 */

/**
 * @brief Logger를 생성한다.
 * @param[in] aLogger        대상 Logger 포인터
 * @param[in] aFileName      File Name String
 * @param[in] aFilePath      File Path String
 * @param[in] aNeedLock      동시성 제어 필요 여부
 * @param[in] aMaxFileSize   파일의 최대 크기
 * @param[in] aErrorStack    대상 에러스택 포인터
 */
stlStatus stlCreateLogger( stlLogger     * aLogger,
                           stlChar       * aFileName,
                           stlChar       * aFilePath,
                           stlBool         aNeedLock,
                           stlInt32        aMaxFileSize,
                           stlErrorStack * aErrorStack )
{
    stlInt32 sState = 0;
    
    STL_PARAM_VALIDATE( aLogger != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aFileName != NULL, aErrorStack );

    stlMemset( aLogger, 0x00, STL_SIZEOF(stlLogger) );
    
    STL_DASSERT( stlStrlen( aFileName ) < STL_MAX_FILE_NAME_LENGTH );
    STL_DASSERT( stlStrlen( aFilePath ) < STL_MAX_FILE_PATH_LENGTH );

    stlStrncpy( aLogger->mFileName,
                aFileName,
                STL_MAX_FILE_NAME_LENGTH );

    stlStrncpy( aLogger->mFilePath,
                aFilePath,
                STL_MAX_FILE_PATH_LENGTH );

    aLogger->mMaxFileSize = aMaxFileSize;
    aLogger->mLastFileNo = 0;

    STL_TRY( stlAllocHeap( (void**)&(aLogger->mLogMsg),
                           STL_MAX_LARGE_ERROR_MESSAGE,
                           aErrorStack )
             == STL_SUCCESS );
    sState = 1;
    
    aLogger->mNeedLock = aNeedLock;
    
    if( aNeedLock == STL_TRUE )
    {
        STL_TRY( stlCreateSemaphore( &(aLogger->mSemaphore),
                                     "sem",
                                     0,
                                     aErrorStack ) == STL_SUCCESS );
        sState = 2;
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) stlFreeHeap( aLogger->mLogMsg );
        case 1:
            (void) stlDestroySemaphore( &(aLogger->mSemaphore), aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief Logger Message를 기록한다.
 * @param[in] aLogger        대상 Logger 포인터
 * @param[in] aErrorStack    대상 에러스택 포인터
 * @param[in] aFormat        String Format
 * @param[in] ...            포맷에 사용될 파라미터 리스트
 */
stlStatus stlLogMsg( stlLogger     * aLogger,
                     stlErrorStack * aErrorStack,
                     const stlChar * aFormat,
                     ... )
{
    va_list    sVarArgList;
    stlInt32   sState  = 0;
    
    /** stlCreateLogger() 호출되었는지 체크 */
    STL_TRY_THROW( aLogger->mLogMsg != NULL, RAMP_ERR_NOT_CREATE_LOGGER );

    if( aLogger->mNeedLock == STL_TRUE )
    {
        STL_TRY( stlAcquireSemaphore( &(aLogger->mSemaphore),
                                      aErrorStack )
                 == STL_SUCCESS );
        sState = 1;
    }

    /**
     * Print timestamp header
     */
    
    STL_TRY( stgPrintTimestamp( aLogger,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Print message body
     */
    
    va_start( sVarArgList, aFormat );
    
    STL_TRY( stgAddLogMsg( aLogger,
                           aFormat,
                           sVarArgList,
                           aErrorStack )
             == STL_SUCCESS );
    
    va_end( sVarArgList );
    
    if( aLogger->mNeedLock == STL_TRUE )
    {
        sState = 0;
        STL_TRY( stlReleaseSemaphore( &(aLogger->mSemaphore),
                                      aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_NOT_CREATE_LOGGER )
    {
        STL_DASSERT( STL_FALSE );
    }
    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) stlReleaseSemaphore( &(aLogger->mSemaphore), aErrorStack ) ;
        default:
            break;
    }
    
    return STL_SUCCESS;
}

/**
 * @brief Logger를 종료한다.
 * @param[in] aLogger        대상 Logger 포인터
 * @param[in] aErrorStack    대상 에러스택 포인터
 */
stlStatus stlDestoryLogger( stlLogger     * aLogger,
                            stlErrorStack * aErrorStack )
{
    stlInt32 sState = 0;
    
    STL_PARAM_VALIDATE( aLogger != NULL, aErrorStack );

    if( aLogger->mNeedLock == STL_TRUE )
    {
        sState = 1;
        aLogger->mNeedLock = STL_FALSE;
        STL_TRY( stlDestroySemaphore( &(aLogger->mSemaphore),
                                      aErrorStack )
                 == STL_SUCCESS );
    }

    if( aLogger->mLogMsg != NULL )
    {
        stlFreeHeap( aLogger->mLogMsg );
        sState = 0;
        aLogger->mLogMsg = NULL;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) stlFreeHeap( aLogger->mLogMsg );
        case 1:
            (void) stlDestroySemaphore( &(aLogger->mSemaphore), aErrorStack );
        default:
            break;
    }

    return STL_FAILURE;
}


/** @} */
